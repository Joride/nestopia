////////////////////////////////////////////////////////////////////////////////////////
//
// Nestopia - NES/Famicom emulator written in C++
//
// Copyright (C) 2003-2007 Martin Freij
//
// This file is part of Nestopia.
//
// Nestopia is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// Nestopia is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Nestopia; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
////////////////////////////////////////////////////////////////////////////////////////

#include <cstring>
#include "NstLog.hpp"
#include "NstCpu.hpp"
#include "NstPpu.hpp"
#include "NstState.hpp"


namespace Nes
{
    namespace Core
    {
        Ppu::Tiles::Tiles()
        : padding0(0), padding1(0) {}
        
        Ppu::Oam::Oam()
        : limit(buffer + STD_LINE_SPRITES), spriteLimit(true), padding(0) {}
        
        Ppu::Output::Output(Video::Screen::Pixel* p)
        : pixels(p) {}
        
        Ppu::TileLut::TileLut()
        {
            for (uint i=0; i < 0x400; ++i)
            {
                block[i][0] = (i & 0x03) ? (i >> 6 & 0xC) | (i >> 0 & 0x3) : 0;
                block[i][1] = (i & 0x0C) ? (i >> 6 & 0xC) | (i >> 2 & 0x3) : 0;
                block[i][2] = (i & 0x30) ? (i >> 6 & 0xC) | (i >> 4 & 0x3) : 0;
                block[i][3] = (i & 0xC0) ? (i >> 6 & 0xC) | (i >> 6 & 0x3) : 0;
            }
        }
        
        
        
        
        
        
        Ppu::Ppu(Cpu& c)
        :
        cpu (c),
        output (screen.pixels)
        {
            SetRegion( cpu.GetRegion() );
            PowerOff();
        }
        
        
        
        
        
        void Ppu::PowerOff()
        {
            Reset( true, false );
        }
        
        void Ppu::Reset(bool hard)
        {
            Reset( hard, true );
        }
        
        void Ppu::Reset(const bool hard,const bool map)
        {
            if (map)
            {
                for (uint i=0x2000; i < 0x4000; i += 0x8)
                {
                    cpu.Map( i+0 ).Set( this, &Ppu::Peek_2xxx, &Ppu::Poke_2000 );
                    cpu.Map( i+1 ).Set( this, &Ppu::Peek_2xxx, &Ppu::Poke_2001 );
                    cpu.Map( i+2 ).Set( this, &Ppu::Peek_2002, &Ppu::Poke_2xxx );
                    cpu.Map( i+3 ).Set( this, &Ppu::Peek_2xxx, &Ppu::Poke_2003 );
                    cpu.Map( i+4 ).Set( this, &Ppu::Peek_2004, &Ppu::Poke_2004 );
                    cpu.Map( i+5 ).Set( this, &Ppu::Peek_2xxx, &Ppu::Poke_2005 );
                    cpu.Map( i+6 ).Set( this, &Ppu::Peek_2xxx, &Ppu::Poke_2006 );
                    cpu.Map( i+7 ).Set( this, &Ppu::Peek_2007, &Ppu::Poke_2007 );
                }
                
                cpu.Map( 0x4014U ).Set( this, &Ppu::Peek_4014, &Ppu::Poke_4014 );
            }
            
            if (hard)
            {
                static const byte powerUpPalette[] =
                {
                    0x3F,0x01,0x00,0x01,0x00,0x02,0x02,0x0D,
                    0x08,0x10,0x08,0x24,0x00,0x00,0x04,0x2C,
                    0x09,0x01,0x34,0x03,0x00,0x04,0x00,0x14,
                    0x08,0x3A,0x00,0x02,0x00,0x20,0x2C,0x08
                };
                
                std::memset( oam.ram, Oam::GARBAGE, Oam::SIZE );
                std::memcpy( palette.ram, powerUpPalette, Palette::SIZE );
                std::memset( nameTable.ram, NameTable::GARBAGE, NameTable::SIZE );
                
                io.latch = 0;
                io.buffer = 0;
                
                stage = WARM_UP_FRAMES;
                phase = &Ppu::WarmUp;
                
                regs.ctrl0 = 0;
                regs.ctrl1 = 0;
                regs.frame = 0;
                regs.status = 0;
                
                scroll.address = 0;
                scroll.toggle = 0;
                scroll.latch = 0;
                scroll.xFine = 0;
                
                output.burstPhase = 0;
            }
            else
            {
                stage = 0;
                phase = &Ppu::HDummy;
                
                regs.status = Regs::STATUS_VBLANK;
            }
            
            if (chr.Source().Empty())
            {
                chr.Source().Set( nameTable.ram, NameTable::SIZE, true, false );
                chr.SwapBanks<SIZE_2K,0x0000>(0,0,0,0);
            }
            
            if (nmt.Source().Empty())
            {
                nmt.Source().Set( nameTable.ram, NameTable::SIZE, true, true );
                nmt.SwapBanks<SIZE_2K,0x0000>(0,0);
            }
            
            chr.ResetAccessors();
            nmt.ResetAccessors();
            
            cycles.count = Cpu::CYCLE_MAX;
            cycles.spriteOverflow = Cpu::CYCLE_MAX;
            cycles.round = 0;
            
            regs.oam = 0;
            
            io.address = 0;
            io.pattern = 0;
            io.a12.Unset();
            
            scanline = SCANLINE_VBLANK;
            
            tiles.pattern[0] = 0;
            tiles.pattern[1] = 0;
            tiles.attribute = 0;
            tiles.mask = 0;
            
            yuvMap = __null;
            rgbMap = __null;
            
            oam.visible = oam.output;
            oam.evaluated = oam.buffer;
            oam.loaded = oam.buffer;
            oam.mask = 0;
            
            output.index = 0;
            output.target = __null;
            
            hActiveHook = Hook( this, &Ppu::Hook_Nop );
            hBlankHook = Hook( this, &Ppu::Hook_Nop );
            
            UpdateStates();
            
            screen.Clear();
        }
        
        void Ppu::SetHActiveHook(const Hook& hook)
        {
            hActiveHook = hook;
        }
        
        void Ppu::SetHBlankHook(const Hook& hook)
        {
            hBlankHook = hook;
        }
        
        void Ppu::UpdateStates()
        {
            io.enabled = regs.ctrl1 & (Regs::CTRL1_BG_ENABLED|Regs::CTRL1_SP_ENABLED);
            scroll.increase = (regs.ctrl0 & Regs::CTRL0_INC32) ? 32 : 1;
            scroll.pattern = (regs.ctrl0 & Regs::CTRL0_BG_OFFSET) << 8;
            tiles.show[0] = (regs.ctrl1 & Regs::CTRL1_BG_ENABLED) ? 0xFF : 0x00;
            tiles.show[1] = (regs.ctrl1 & Regs::CTRL1_FULL_BG_ENABLED) == Regs::CTRL1_FULL_BG_ENABLED ? 0xFF : 0x00;
            oam.show[0] = (regs.ctrl1 & Regs::CTRL1_SP_ENABLED) ? 0xFF : 0x00;
            oam.show[1] = (regs.ctrl1 & Regs::CTRL1_FULL_SP_ENABLED) == Regs::CTRL1_FULL_SP_ENABLED ? 0xFF : 0x00;
            
            UpdatePalette();
        }
        
        void Ppu::UpdatePalette()
        {
            for (uint i=0, c=Coloring(), e=Emphasis(); i < Palette::SIZE; ++i)
                output.palette[i] = (rgbMap ? rgbMap[palette.ram[i] & uint(Palette::COLOR)] : palette.ram[i]) & c | e;
        }
        
        void Ppu::SaveState(State::Saver& state,const dword baseChunk) const
        {
            state.Begin( baseChunk );
            
            {
                const byte data[11] =
                {
                    regs.ctrl0,
                    regs.ctrl1,
                    regs.status,
                    scroll.address & 0xFF,
                    scroll.address >> 8,
                    scroll.latch & 0xFF,
                    scroll.latch >> 8,
                    scroll.xFine | scroll.toggle << 3,
                    regs.oam,
                    io.buffer,
                    io.latch
                };
                
                state.Begin( AsciiId<'R','E','G'>::V ).Write( data ).End();
            }
            
            state.Begin( AsciiId<'P','A','L'>::V ).Compress( palette.ram ).End();
            state.Begin( AsciiId<'O','A','M'>::V ).Compress( oam.ram ).End();
            state.Begin( AsciiId<'N','M','T'>::V ).Compress( nameTable.ram ).End();
            
            if (cpu.GetRegion() == Region::NTSC)
                state.Begin( AsciiId<'F','R','M'>::V ).Write8( (regs.frame & Regs::FRAME_ODD) == 0 ).End();
            
            if (phase == &Ppu::WarmUp)
                state.Begin( AsciiId<'P','O','W'>::V ).Write8( WARM_UP_FRAMES-stage ).End();
            
            state.End();
        }
        
        void Ppu::LoadState(State::Loader& state)
        {
            phase = &Ppu::HDummy;
            regs.frame = 0;
            stage = 0;
            output.burstPhase = 0;
            
            while (const dword chunk = state.Begin())
            {
                switch (chunk)
                {
                    case AsciiId<'R','E','G'>::V:
                    {
                        State::Loader::Data<11> data( state );
                        
                        regs.ctrl0 = data[0];
                        regs.ctrl1 = data[1];
                        regs.status = data[2] & Regs::STATUS_BITS;
                        scroll.address = data[3] | (data[4] << 8 & 0x7F00);
                        scroll.latch = data[5] | (data[6] << 8 & 0x7F00);
                        scroll.xFine = data[7] & 0x7;
                        scroll.toggle = data[7] >> 3 & 0x1;
                        regs.oam = data[8];
                        io.buffer = data[9];
                        io.latch = data[10];
                        
                        break;
                    }
                        
                    case AsciiId<'P','A','L'>::V:
                        
                        state.Uncompress( palette.ram );
                        break;
                        
                    case AsciiId<'O','A','M'>::V:
                        
                        state.Uncompress( oam.ram );
                        break;
                        
                    case AsciiId<'N','M','T'>::V:
                        
                        state.Uncompress( nameTable.ram );
                        break;
                        
                    case AsciiId<'F','R','M'>::V:
                        
                        if (cpu.GetRegion() == Region::NTSC)
                            regs.frame = (state.Read8() & 0x1) ? 0 : Regs::FRAME_ODD;
                        
                        break;
                        
                    case AsciiId<'P','O','W'>::V:
                        
                        stage = WARM_UP_FRAMES - (state.Read8() & 0x7);
                        
                        if (stage > WARM_UP_FRAMES)
                            stage = WARM_UP_FRAMES;
                        
                        phase = &Ppu::WarmUp;
                        break;
                }
                
                state.End();
            }
            
            UpdateStates();
        }
        
        void Ppu::EnableCpuSynchronization()
        {
            cpu.AddHook( Hook(this,&Ppu::Hook_Sync) );
        }
        
        void Ppu::ChrMem::ResetAccessors()
        {
            accessors[0].Set( this, &ChrMem::Access_Pattern );
            accessors[1].Set( this, &ChrMem::Access_Pattern );
        }
        
        void Ppu::ChrMem::SetDefaultAccessor(uint i)
        {
            (__builtin_expect(!(!!(i < 2)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstPpu.cpp", 358, "!!(i < 2)") : (void)0);
            accessors[i].Set( this, &ChrMem::Access_Pattern );
        }
        
        void Ppu::NmtMem::ResetAccessors()
        {
            accessors[0][0].Set( this, &NmtMem::Access_Name_2000 );
            accessors[0][1].Set( this, &NmtMem::Access_Name_2000 );
            accessors[1][0].Set( this, &NmtMem::Access_Name_2400 );
            accessors[1][1].Set( this, &NmtMem::Access_Name_2400 );
            accessors[2][0].Set( this, &NmtMem::Access_Name_2800 );
            accessors[2][1].Set( this, &NmtMem::Access_Name_2800 );
            accessors[3][0].Set( this, &NmtMem::Access_Name_2C00 );
            accessors[3][1].Set( this, &NmtMem::Access_Name_2C00 );
        }
        
        void Ppu::NmtMem::SetDefaultAccessor(uint i,uint j)
        {
            (__builtin_expect(!(!!(i < 4 && j < 2)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstPpu.cpp", 376, "!!(i < 4 && j < 2)") : (void)0);
            
            accessors[i][j].Set
            (
             this,
             i == 0 ? &NmtMem::Access_Name_2000 :
             i == 1 ? &NmtMem::Access_Name_2400 :
             i == 2 ? &NmtMem::Access_Name_2800 :
             &NmtMem::Access_Name_2C00
             );
        }
        
        void Ppu::NmtMem::SetDefaultAccessors(uint i)
        {
            SetDefaultAccessor( i, 0 );
            SetDefaultAccessor( i, 1 );
        }
        
        void Ppu::SetRegion(const Region::Type region)
        {
            regs.frame = 0;
            output.burstPhase = 0;
            
            if (region == Region::NTSC)
            {
                cycles.one = Clocks::RP2C02_CC * 1;
                cycles.four = Clocks::RP2C02_CC * 4;
                cycles.eight = Clocks::RP2C02_CC * 8;
                cycles.six = Clocks::RP2C02_CC * 6;
            }
            else
            {
                cycles.one = Clocks::RP2C07_CC * 1;
                cycles.four = Clocks::RP2C07_CC * 4;
                cycles.eight = Clocks::RP2C07_CC * 8;
                cycles.six = Clocks::RP2C07_CC * 6;
            }
        }
        
        void Ppu::SetYuvMap(const byte* map,bool preEffect)
        {
            yuvMap = preEffect ? __null : map;
            rgbMap = preEffect ? map : __null;
            
            UpdatePalette();
        }
        
        
        
        
        
        void Ppu::BeginFrame(bool frameLock)
        {
            (__builtin_expect(!(!!(scanline == SCANLINE_VBLANK && (phase == &Ppu::HDummy || phase == &Ppu::WarmUp) && (cpu.GetRegion() == Region::NTSC) == (cycles.one == Clocks::RP2C02_CC) && cycles.spriteOverflow == Cpu::CYCLE_MAX)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstPpu.cpp", 435, "!!( scanline == SCANLINE_VBLANK && (phase == &Ppu::HDummy || phase == &Ppu::WarmUp) && (cpu.GetRegion() == Region::NTSC) == (cycles.one == Clocks::RP2C02_CC) && cycles.spriteOverflow == Cpu::CYCLE_MAX)") : (void)0);
            
            
            
            
            
            
            
            oam.limit = oam.buffer + ((uint(oam.spriteLimit) | uint(frameLock)) ? Oam::STD_LINE_SPRITES : Oam::MAX_LINE_SPRITES);
            
            output.target = output.pixels;
            
            Cycle frame;
            
            if (cycles.one == Clocks::RP2C02_CC)
            {
                regs.frame ^= Regs::FRAME_ODD;
                cycles.count = Clocks::RP2C02_HVINT;
                
                if (phase != &Ppu::WarmUp)
                    frame = Clocks::RP2C02_HVSYNC_0;
                else
                    frame = Clocks::RP2C02_HVSYNC_0 - Clocks::RP2C02_HVINT;
            }
            else
            {
                (__builtin_expect(!(!!(regs.frame == 0 && output.burstPhase == 0)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstPpu.cpp", 455, "!!(regs.frame == 0 && output.burstPhase == 0)") : (void)0);
                
                cycles.count = Clocks::RP2C07_HVINT;
                frame = Clocks::RP2C07_HVSYNC;
            }
            
            cpu.SetFrameCycles( frame );
        }
        
        void Ppu::Hook_Nop(void* p_) { static_cast<Ppu*>(p_)->Hook_M_Nop(); } inline void Ppu::Hook_M_Nop()
        {
        }
        
        void Ppu::Hook_Sync(void* p_) { static_cast<Ppu*>(p_)->Hook_M_Sync(); } inline void Ppu::Hook_M_Sync()
        {
            const Cycle elapsed = cpu.GetCycles();
            
            if (cycles.count < elapsed)
            {
                cycles.round = elapsed;
                
                
                
                
                do
                {
                    (*this.*phase)();
                }
                while (cycles.count < elapsed);
                
            }
        }
        
        void Ppu::EndFrame()
        {
            if (cycles.count != Cpu::CYCLE_MAX)
            {
                cycles.round = Cpu::CYCLE_MAX;
                
                
                
                
                do
                {
                    (*this.*phase)();
                }
                while (cycles.count != Cpu::CYCLE_MAX);
                
            }
        }
        
        void Ppu::Update(Cycle dataSetup)
        {
            dataSetup += cpu.GetCycles();
            
            if (cycles.count < dataSetup)
            {
                cycles.round = dataSetup;
                
                
                
                
                do
                {
                    (*this.*phase)();
                }
                while (cycles.count < dataSetup);
                
            }
        }
        
        void Ppu::SetMirroring(const byte (&banks)[4])
        {
            Update( cycles.one );
            
            (__builtin_expect(!(!!(banks[0] < 4 && banks[1] < 4 && banks[2] < 4 && banks[3] < 4)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstPpu.cpp", 530, "!!(banks[0] < 4 && banks[1] < 4 && banks[2] < 4 && banks[3] < 4)") : (void)0);
            nmt.SwapBanks<SIZE_1K,0x0000>( banks[0], banks[1], banks[2], banks[3] );
        }
        
        void Ppu::SetMirroring(Mirroring type)
        {
            (__builtin_expect(!(!!(type < 6)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstPpu.cpp", 536, "!!(type < 6)") : (void)0);
            
            typedef char Nestopia_assertion_at_line_546[(NMT_HORIZONTAL == 0 && NMT_VERTICAL == 1 && NMT_FOURSCREEN == 2 && NMT_ZERO == 3 && NMT_ONE == 4 && NMT_CONTROLLED == 5) ? 1 : -1];
            static const byte banks[6][4] =
            {
                {0,0,1,1},
                {0,1,0,1},
                {0,1,2,3},
                {0,0,0,0},
                {1,1,1,1},
                {0,1,0,1}
            };
            
            SetMirroring( banks[type] );
        }
        
        __attribute__((noinline)) Data Ppu::ChrMem::Access_Pattern(void* p_,Address i_) { return static_cast<Ppu::ChrMem*>(p_)->Access_M_Pattern(i_); } inline Data Ppu::ChrMem::Access_M_Pattern(Address address)
        {
            return Peek( address );
        }
        
        __attribute__((noinline)) Data Ppu::NmtMem::Access_Name_2000(void* p_,Address i_) { return static_cast<Ppu::NmtMem*>(p_)->Access_M_Name_2000(i_); } inline Data Ppu::NmtMem::Access_M_Name_2000(Address address)
        {
            return (*this)[0][address];
        }
        
        __attribute__((noinline)) Data Ppu::NmtMem::Access_Name_2400(void* p_,Address i_) { return static_cast<Ppu::NmtMem*>(p_)->Access_M_Name_2400(i_); } inline Data Ppu::NmtMem::Access_M_Name_2400(Address address)
        {
            return (*this)[1][address];
        }
        
        __attribute__((noinline)) Data Ppu::NmtMem::Access_Name_2800(void* p_,Address i_) { return static_cast<Ppu::NmtMem*>(p_)->Access_M_Name_2800(i_); } inline Data Ppu::NmtMem::Access_M_Name_2800(Address address)
        {
            return (*this)[2][address];
        }
        
        __attribute__((noinline)) Data Ppu::NmtMem::Access_Name_2C00(void* p_,Address i_) { return static_cast<Ppu::NmtMem*>(p_)->Access_M_Name_2C00(i_); } inline Data Ppu::NmtMem::Access_M_Name_2C00(Address address)
        {
            return (*this)[3][address];
        }
        
        inline uint Ppu::Chr::FetchPattern(uint address) const
        {
            address &= 0x1FFF;
            return accessors[address >> 12].Fetch( address );
        }
        
        inline uint Ppu::Nmt::FetchName(uint address) const
        {
            const uint offset = address & 0x03FF;
            return accessors[address >> 10 & 0x3][(offset + 0x40) >> 10].Fetch( offset );
        }
        
        inline uint Ppu::Nmt::FetchAttribute(uint address) const
        {
            return accessors[address >> 10 & 0x3][1].Fetch( 0x3C0 | (address >> 4 & 0x038) | (address >> 2 & 0x007) );
        }
        
        inline uint Ppu::FetchName() const
        {
            return nmt.FetchName( io.address ) << 4 | scroll.address >> 12 | scroll.pattern;
        }
        
        inline uint Ppu::FetchAttribute() const
        {
            return nmt.FetchAttribute( io.address ) >> ((scroll.address & 0x2) | (scroll.address >> 4 & 0x4)) & 0x3;
        }
        
        inline bool Ppu::IsDead() const
        {
            return !io.enabled || scanline >= 240;
        }
        
        inline void Ppu::UpdateScrollAddress(const uint newAddress)
        {
            const uint oldAddress = scroll.address;
            scroll.address = newAddress;
            
            if (io.a12 && (newAddress & 0x1000) > (oldAddress & 0x1000))
                io.a12.Toggle( cpu.GetCycles() );
        }
        
        inline uint Ppu::Coloring() const
        {
            return (regs.ctrl1 & Regs::CTRL1_MONOCHROME) ? Palette::MONO : Palette::COLOR;
        }
        
        inline uint Ppu::Emphasis() const
        {
            return (regs.ctrl1 & Regs::CTRL1_EMPHASIS) << 1;
        }
        
        void Ppu::Poke_2000(void* p_,Address i_,Data j_) { static_cast<Ppu*>(p_)->Poke_M_2000(i_,j_); } inline void Ppu::Poke_M_2000(Address,Data data)
        {
            Update( cycles.one );
            
            scroll.latch = (scroll.latch & 0x73FF) | (data & 0x03) << 10;
            scroll.increase = (data & Regs::CTRL0_INC32) ? 32 : 1;
            scroll.pattern = (data & Regs::CTRL0_BG_OFFSET) << 8;
            
            io.latch = data;
            data = regs.ctrl0;
            regs.ctrl0 = io.latch;
            
            if ((regs.ctrl0 & regs.status & Regs::CTRL0_NMI) > data)
                cpu.DoNMI();
        }
        
        void Ppu::Poke_2001(void* p_,Address i_,Data j_) { static_cast<Ppu*>(p_)->Poke_M_2001(i_,j_); } inline void Ppu::Poke_M_2001(Address,Data data)
        {
            Update( cycles.one );
            
            io.enabled = data & (Regs::CTRL1_BG_ENABLED|Regs::CTRL1_SP_ENABLED);
            
            tiles.show[0] = (data & Regs::CTRL1_BG_ENABLED) ? 0xFF : 0x00;
            tiles.show[1] = (data & Regs::CTRL1_FULL_BG_ENABLED) == Regs::CTRL1_FULL_BG_ENABLED ? 0xFF : 0x00;
            
            oam.show[0] = (data & Regs::CTRL1_SP_ENABLED) ? 0xFF : 0x00;
            oam.show[1] = (data & Regs::CTRL1_FULL_SP_ENABLED) == Regs::CTRL1_FULL_SP_ENABLED ? 0xFF : 0x00;
            
            const uint pos = ((output.index & 0xFFU) - 8) >= (256-16);
            
            tiles.mask = tiles.show[pos];
            oam.mask = oam.show[pos];
            
            io.latch = data;
            data = (regs.ctrl1 ^ data) & (Regs::CTRL1_EMPHASIS|Regs::CTRL1_MONOCHROME);
            regs.ctrl1 = io.latch;
            
            if (data)
            {
                const uint c = Coloring();
                const uint e = Emphasis();
                
                const byte* const __restrict__ map = rgbMap;
                
                if (!map)
                {
                    for (uint i=0; i < Palette::SIZE; ++i)
                        output.palette[i] = palette.ram[i] & c | e;
                }
                else
                {
                    for (uint i=0; i < Palette::SIZE; ++i)
                        output.palette[i] = map[palette.ram[i] & Palette::COLOR] & c | e;
                }
            }
        }
        
        Data Ppu::Peek_2002(void* p_,Address i_) { return static_cast<Ppu*>(p_)->Peek_M_2002(i_); } inline Data Ppu::Peek_M_2002(Address)
        {
            Update( cycles.one );
            
            uint status = regs.status & 0xFF;
            
            if (cycles.spriteOverflow <= cpu.GetCycles())
                status |= Regs::STATUS_SP_OVERFLOW;
            
            regs.status &= (Regs::STATUS_VBLANK^0xFFU);
            scroll.toggle = 0;
            io.latch = (io.latch & Regs::STATUS_LATCH) | status;
            
            return io.latch;
        }
        
        void Ppu::Poke_2003(void* p_,Address i_,Data j_) { static_cast<Ppu*>(p_)->Poke_M_2003(i_,j_); } inline void Ppu::Poke_M_2003(Address,Data data)
        {
            Update( cycles.one );
            
            regs.oam = data;
            io.latch = data;
        }
        
        void Ppu::Poke_2004(void* p_,Address i_,Data j_) { static_cast<Ppu*>(p_)->Poke_M_2004(i_,j_); } inline void Ppu::Poke_M_2004(Address,Data data)
        {
            Update( cycles.one );
            
            (__builtin_expect(!(!!(regs.oam < Oam::SIZE)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstPpu.cpp", 722, "!!(regs.oam < Oam::SIZE)") : (void)0);
            ((void)0);
            
            if (!IsDead())
                data = Oam::GARBAGE;
            
            byte* const __restrict__ value = oam.ram + regs.oam;
            regs.oam = (regs.oam + 1) & 0xFF;
            io.latch = data;
            *value = data;
        }
        
        Data Ppu::Peek_2004(void* p_,Address i_) { return static_cast<Ppu*>(p_)->Peek_M_2004(i_); } inline Data Ppu::Peek_M_2004(Address)
        {
            Update( cycles.one );
            
            (__builtin_expect(!(!!(regs.oam < Oam::SIZE)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstPpu.cpp", 738, "!!(regs.oam < Oam::SIZE)") : (void)0);
            
            uint data;
            
            if (IsDead())
            {
                data = oam.ram[regs.oam];
            }
            else
            {
                data = cpu.GetCycles() / cycles.one % 341 + (cpu.GetFrameCycles() == Clocks::RP2C02_HVSYNC_1);
                
                if (data < 64)
                {
                    data = Oam::GARBAGE;
                }
                else if (data < 192)
                {
                    data = oam.ram[(data-64) << 1 & 0xFC];
                }
                else if (data < 256)
                {
                    data = oam.ram[(data & 1) ? 0xFC : (data-192) << 1 & 0xFC];
                }
                else if (data < 320)
                {
                    data = Oam::GARBAGE;
                }
                else
                {
                    data = oam.ram[0];
                }
            }
            
            io.latch = data;
            
            return data;
        }
        
        void Ppu::Poke_2005(void* p_,Address i_,Data j_) { static_cast<Ppu*>(p_)->Poke_M_2005(i_,j_); } inline void Ppu::Poke_M_2005(Address,Data data)
        {
            Update( cycles.one );
            
            io.latch = data;
            
            if (scroll.toggle ^= 1)
            {
                scroll.latch = (scroll.latch & 0x7FE0) | (data >> 3);
                scroll.xFine = data & 0x7;
            }
            else
            {
                scroll.latch = (scroll.latch & 0x0C1F) | ((data << 2 | data << 12) & 0x73E0);
            }
        }
        
        void Ppu::Poke_2006(void* p_,Address i_,Data j_) { static_cast<Ppu*>(p_)->Poke_M_2006(i_,j_); } inline void Ppu::Poke_M_2006(Address,Data data)
        {
            Update( cycles.one );
            
            io.latch = data;
            
            if (scroll.toggle ^= 1)
            {
                scroll.latch = (scroll.latch & 0x00FF) | (data & 0x3F) << 8;
            }
            else
            {
                scroll.latch = (scroll.latch & 0x7F00) | data;
                
                UpdateScrollAddress( scroll.latch );
            }
        }
        
        void Ppu::Poke_2007(void* p_,Address i_,Data j_) { static_cast<Ppu*>(p_)->Poke_M_2007(i_,j_); } inline void Ppu::Poke_M_2007(Address,Data data)
        {
            Update( cycles.four );
            
            ((void)0);
            
            uint address = scroll.address;
            UpdateScrollAddress( (scroll.address + scroll.increase) & 0x7FFF );
            
            io.latch = data;
            
            if ((address & 0x3F00) == 0x3F00)
            {
                address &= 0x1F;
                
                const uint final = (!rgbMap ? data : rgbMap[data & Palette::COLOR]) & Coloring() | Emphasis();
                
                palette.ram[address] = data;
                output.palette[address] = final;
                
                if (!(address & 0x3))
                {
                    palette.ram[address ^ 0x10] = data;
                    output.palette[address ^ 0x10] = final;
                }
            }
            else
            {
                address &= 0x3FFF;
                
                if (address >= 0x2000)
                    nmt.Poke( address & 0xFFF, data );
                else
                    chr.Poke( address, data );
            }
        }
        
        Data Ppu::Peek_2007(void* p_,Address i_) { return static_cast<Ppu*>(p_)->Peek_M_2007(i_); } inline Data Ppu::Peek_M_2007(Address)
        {
            Update( cycles.one );
            
            ((void)0);
            
            const uint address = scroll.address & 0x3FFF;
            
            UpdateScrollAddress( (scroll.address + scroll.increase) & 0x7FFF );
            
            io.latch = (address & 0x3F00) != 0x3F00 ? io.buffer : palette.ram[address & 0x1F] & Coloring();
            io.buffer = (address >= 0x2000 ? nmt.FetchName( address ) : chr.FetchPattern( address ));
            
            return io.latch;
        }
        
        void Ppu::Poke_2xxx(void* p_,Address i_,Data j_) { static_cast<Ppu*>(p_)->Poke_M_2xxx(i_,j_); } inline void Ppu::Poke_M_2xxx(Address,Data data)
        {
            io.latch = data;
        }
        
        Data Ppu::Peek_2xxx(void* p_,Address i_) { return static_cast<Ppu*>(p_)->Peek_M_2xxx(i_); } inline Data Ppu::Peek_M_2xxx(Address)
        {
            return io.latch;
        }
        
        void Ppu::Poke_4014(void* p_,Address i_,Data j_) { static_cast<Ppu*>(p_)->Poke_M_4014(i_,j_); } inline void Ppu::Poke_M_4014(Address,Data data)
        {
            Update( cycles.one );
            
            (__builtin_expect(!(!!(regs.oam < Oam::SIZE)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstPpu.cpp", 879, "!!(regs.oam < Oam::SIZE)") : (void)0);
            ((void)0);
            
            cpu.StealCycles( cpu.GetClock() * Oam::DMA_CYCLES );
            
            if (IsDead())
            {
                data <<= 8;
                
                if (regs.oam == 0x00 && data < 0x2000)
                {
                    std::memcpy( oam.ram, cpu.GetRam() + (data & (Cpu::RAM_SIZE-1)), Oam::SIZE );
                    io.latch = oam.ram[0xFF];
                }
                else
                {
                    uint dst = (regs.oam - 1U) & 0xFF;
                    const uint end = dst;
                    uint tmp;
                    
                    do
                    {
                        oam.ram[(dst = (dst + 1) & 0xFF)] = (tmp = cpu.Peek( data++ ));
                    }
                    while (dst != end);
                    
                    io.latch = tmp;
                }
            }
            else
            {
                io.latch = Oam::GARBAGE;
                std::memset( oam.ram, Oam::GARBAGE, Oam::SIZE );
            }
        }
        
        Data Ppu::Peek_4014(void* p_,Address i_) { return static_cast<Ppu*>(p_)->Peek_M_4014(i_); } inline Data Ppu::Peek_M_4014(Address)
        {
            return 0x40;
        }
        
        inline void Ppu::Scroll::ClockX()
        {
            if ((address & X_TILE) != X_TILE)
                ++address;
            else
                address ^= (X_TILE|NAME_LOW);
        }
        
        inline void Ppu::Scroll::ResetX()
        {
            address &= (X_TILE|NAME_LOW) ^ 0x7FFFU;
            address |= latch & (X_TILE|NAME_LOW);
        }
        
        inline void Ppu::Scroll::ClockY()
        {
            if ((address & Y_FINE) != (7U << 12))
            {
                address += (1U << 12);
            }
            else switch (address & Y_TILE)
            {
                default: address = (address & (Y_FINE ^ 0x7FFFU)) + (1U << 5); break;
                case (29U << 5): address ^= NAME_HIGH;
                case (31U << 5): address &= (Y_FINE|Y_TILE) ^ 0x7FFFU; break;
            }
        }
        
        inline void Ppu::EvaluateSprites()
        {
            (__builtin_expect(!(!!(scanline >= 0 && scanline <= 239 && regs.oam < Oam::SIZE)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstPpu.cpp", 950, "!!(scanline >= 0 && scanline <= 239 && regs.oam < Oam::SIZE)") : (void)0);
            
            oam.evaluated = oam.buffer;
            
            if (io.enabled)
            {
                uint i = 0;
                const uint height = (regs.ctrl0 >> 2 & 8) + 8;
                
                for (;;)
                {
                    const byte* const __restrict__ obj = oam.ram + i + (regs.oam & (i >= 8 ? 0 : Oam::OFFSET_TO_0_1));
                    i += 4;
                    
                    uint y = uint(scanline) - obj[0];
                    
                    if (y >= height)
                    {
                        if (i == Oam::SIZE)
                            return;
                    }
                    else
                    {
                        Oam::Buffer* const __restrict__ eval = oam.evaluated;
                        
                        eval->comparitor = y ^ ((obj[2] & uint(Oam::Y_FLIP)) ? 0xF : 0x0);
                        
                        eval->attribute =
                        (
                         (i == 4) |
                         (obj[2] & uint(Oam::COLOR)) << 2 |
                         (obj[2] & uint(Oam::BEHIND|Oam::X_FLIP))
                         );
                        
                        eval->tile = obj[1];
                        eval->x = obj[3];
                        
                        ++oam.evaluated;
                        
                        if (i == Oam::SIZE)
                            return;
                        
                        if (oam.evaluated == oam.limit)
                            break;
                    }
                }
                
                if (cycles.spriteOverflow == Cpu::CYCLE_MAX)
                {
                    do
                    {
                        if (uint(scanline) - oam.ram[i] >= height)
                        {
                            i = ((i + 4) & 0x1FC) + ((i + 1) & 0x03);
                        }
                        else
                        {
                            cycles.spriteOverflow = cycles.count + cycles.one * (45 + i/4 * 2);
                            break;
                        }
                    }
                    while (i < Oam::SIZE);
                }
            }
            
        }
        
        void Ppu::LoadSprite()
        {
            (__builtin_expect(!(!!(oam.loaded < oam.evaluated)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstPpu.cpp", 1019, "!!(oam.loaded < oam.evaluated)") : (void)0);
            
            uint address;
            
            if (regs.ctrl0 & Regs::CTRL0_SP8X16)
            {
                address =
                (
                 ((oam.loaded->tile & uint(Oam::Buffer::TILE_LSB)) << 12) |
                 ((oam.loaded->tile & (Oam::Buffer::TILE_LSB ^ 0xFFU)) << 4) |
                 ((oam.loaded->comparitor & uint(Oam::Buffer::RANGE_MSB)) << 1)
                 );
            }
            else
            {
                address = (regs.ctrl0 & Regs::CTRL0_SP_OFFSET) << 9 | oam.loaded->tile << 4;
            }
            
            address |= oam.loaded->comparitor & uint(Oam::Buffer::XFINE);
            
            if (io.a12 && address & 0x1000)
                io.a12.Toggle( cycles.count );
            
            uint pattern[2] =
            {
                chr.FetchPattern( address | 0x0 ),
                chr.FetchPattern( address | 0x8 )
            };
            
            if (pattern[0] | pattern[1])
            {
                address = (oam.loaded->attribute & uint(Oam::X_FLIP)) ? 7 : 0;
                
                uint p =
                (
                 (pattern[0] >> 1 & 0x0055) | (pattern[1] << 0 & 0x00AA) |
                 (pattern[0] << 8 & 0x5500) | (pattern[1] << 9 & 0xAA00)
                 );
                
                Oam::Output* const __restrict__ entry = oam.visible++;
                
                entry->pixels[( address^=6 )] = ( p ) & 0x3;
                entry->pixels[( address^=2 )] = ( p >>= 2 ) & 0x3;
                entry->pixels[( address^=6 )] = ( p >>= 2 ) & 0x3;
                entry->pixels[( address^=2 )] = ( p >>= 2 ) & 0x3;
                entry->pixels[( address^=7 )] = ( p >>= 2 ) & 0x3;
                entry->pixels[( address^=2 )] = ( p >>= 2 ) & 0x3;
                entry->pixels[( address^=6 )] = ( p >>= 2 ) & 0x3;
                entry->pixels[( address^=2 )] = ( p >>= 2 );
                
                const uint attribute = oam.loaded->attribute;
                
                entry->x = oam.loaded->x;
                entry->palette = Palette::SPRITE_OFFSET + (attribute & uint(Oam::COLOR) << 2);
                entry->zero = (attribute & 0x1) ? 0x3 : 0x0;
                entry->behind = (attribute & Oam::BEHIND) ? 0x3 : 0x0;
            }
            
            ++oam.loaded;
        }
        
        inline void Ppu::LoadTiles()
        {
            const byte* const __restrict__ src[] =
            {
                tileLut.block[((tiles.pattern[0] & 0xAAU) >> 1 | (tiles.pattern[1] & 0xAAU) << 0) | (uint(tiles.attribute) << 8)],
                tileLut.block[((tiles.pattern[0] & 0x55U) >> 0 | (tiles.pattern[1] & 0x55U) << 1) | (uint(tiles.attribute) << 8)]
            };
            
            byte* const __restrict__ dst = tiles.pixels + tiles.index;
            tiles.index ^= 8U;
            
            dst[6] = src[0][0];
            dst[4] = src[0][1];
            dst[2] = src[0][2];
            dst[0] = src[0][3];
            dst[7] = src[1][0];
            dst[5] = src[1][1];
            dst[3] = src[1][2];
            dst[1] = src[1][3];
        }
        
        inline void Ppu::RenderPixel()
        {
            uint pixel = 0;
            
            if (io.enabled)
            {
                const uint index = output.index & 0xFF;
                pixel = tiles.pixels[(index + scroll.xFine) & 15] & tiles.mask;
                
                for (const Oam::Output* __restrict__ sprite=oam.output, *const end=oam.visible; sprite != end; ++sprite)
                {
                    uint x = index - sprite->x;
                    
                    if (x > 7)
                        continue;
                    
                    x = sprite->pixels[x] & oam.mask;
                    
                    if (x)
                    {
                        
                        
                        
                        if (pixel & sprite->zero)
                            regs.status |= Regs::STATUS_SP_ZERO_HIT;
                        
                        if (!(pixel & sprite->behind))
                            pixel = sprite->palette + x;
                        
                        break;
                    }
                }
            }
            else if ((scroll.address & 0x3F00) == 0x3F00)
            {
                pixel = scroll.address & 0x1F;
            }
            
            ++output.index;
            Video::Screen::Pixel* const __restrict__ target = output.target++;
            *target = output.palette[pixel];
        }
        
        void Ppu::HActive0()
        {
            LoadTiles();
            
            cycles.count += cycles.one;
            
            if (io.enabled)
                io.address = scroll.address;
            
            RenderPixel();
            
            if (stage != 8)
            {
                phase = &Ppu::HActive1;
            }
            else
            {
                EvaluateSprites();
                phase = &Ppu::HActive1;
            }
        }
        
        void Ppu::HActive1()
        {
            cycles.count += cycles.one;
            
            if (io.enabled)
                io.pattern = FetchName();
            
            RenderPixel();
            phase = &Ppu::HActive2;
        }
        
        void Ppu::HActive2()
        {
            cycles.count += cycles.one;
            
            if (io.enabled)
                io.address = scroll.address;
            
            RenderPixel();
            phase = &Ppu::HActive3;
        }
        
        void Ppu::HActive3()
        {
            cycles.count += cycles.one;
            
            if (io.enabled)
            {
                tiles.attribute = FetchAttribute();
                scroll.ClockX();
                
                if (stage == 31)
                    scroll.ClockY();
            }
            
            RenderPixel();
            phase = &Ppu::HActive4;
        }
        
        void Ppu::HActive4()
        {
            if (io.enabled)
            {
                io.address = io.pattern | 0x0;
                
                if (io.a12 && scroll.pattern)
                    io.a12.Toggle( cycles.count );
            }
            
            cycles.count += cycles.one;
            
            RenderPixel();
            phase = &Ppu::HActive5;
        }
        
        void Ppu::HActive5()
        {
            cycles.count += cycles.one;
            
            if (io.enabled)
                tiles.pattern[0] = chr.FetchPattern( io.address );
            
            RenderPixel();
            phase = &Ppu::HActive6;
        }
        
        void Ppu::HActive6()
        {
            cycles.count += cycles.one;
            
            if (io.enabled)
                io.address = io.pattern | 0x8;
            
            RenderPixel();
            phase = &Ppu::HActive7;
        }
        
        void Ppu::HActive7()
        {
            cycles.count += cycles.one;
            
            const uint noSpHitOn255 = regs.status;
            
            RenderPixel();
            
            tiles.mask = tiles.show[0];
            oam.mask = oam.show[0];
            
            if (io.enabled)
                tiles.pattern[1] = chr.FetchPattern( io.address );
            
            (__builtin_expect(!(!!(stage < 32)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstPpu.cpp", 1257, "!!(stage < 32)") : (void)0);
            
            stage = (stage + 1) & 31;
            
            if (stage)
            {
                phase = &Ppu::HActive0;
            }
            else
            {
                cycles.count += cycles.one;
                regs.status = noSpHitOn255;
                
                phase = &Ppu::HBlank;
            }
        }
        
        void Ppu::HBlank()
        {
            (__builtin_expect(!(!!(stage == 0)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstPpu.cpp", 1276, "!!(stage == 0)") : (void)0);
            
            hBlankHook.Execute();
            
            cycles.count += cycles.four - cycles.one;
            
            oam.loaded = oam.buffer;
            oam.visible = oam.output;
            
            if (io.enabled)
                scroll.ResetX();
            
            phase = &Ppu::HBlankSp;
        }
        
        void Ppu::HBlankSp()
        {
        hell:
            
            if (io.enabled)
            {
                if (oam.loaded == oam.evaluated)
                {
                    if (io.a12 && regs.ctrl0 & (Regs::CTRL0_SP_OFFSET|Regs::CTRL0_SP8X16))
                        io.a12.Toggle( cycles.count );
                }
                else
                {
                    LoadSprite();
                }
            }
            
            (__builtin_expect(!(!!(stage < 8)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstPpu.cpp", 1308, "!!(stage < 8)") : (void)0);
            
            stage = (stage + 1) & 7;
            
            if (stage)
            {
                cycles.count += cycles.eight;
                
                if (cycles.count < cycles.round)
                    goto hell;
                else
                    phase = &Ppu::HBlankSp;
            }
            else
            {
                if (oam.loaded != oam.evaluated && io.enabled)
                {
                    
                    
                    do
                    {
                        LoadSprite();
                    }
                    while (oam.loaded != oam.evaluated);
                }
                
                cycles.count += cycles.four;
                phase = &Ppu::HBlankBg;
            }
        }
        
        void Ppu::HBlankBg()
        {
            (__builtin_expect(!(!!(stage == 0)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstPpu.cpp", 1341, "!!(stage == 0)") : (void)0);
            
            hActiveHook.Execute();
            
            cycles.count += cycles.one;
            tiles.index = 0;
            
            if (io.enabled)
                io.address = scroll.address;
            
            phase = &Ppu::HBlankBg1;
        }
        
        void Ppu::HBlankBg0()
        {
            LoadTiles();
            
            cycles.count += cycles.one;
            
            if (io.enabled)
                io.address = scroll.address;
            
            phase = &Ppu::HBlankBg1;
        }
        
        void Ppu::HBlankBg1()
        {
            cycles.count += cycles.one;
            
            if (io.enabled)
                io.pattern = FetchName();
            
            phase = &Ppu::HBlankBg2;
        }
        
        void Ppu::HBlankBg2()
        {
            cycles.count += cycles.one;
            
            if (io.enabled)
                io.address = scroll.address;
            
            phase = &Ppu::HBlankBg3;
        }
        
        void Ppu::HBlankBg3()
        {
            cycles.count += cycles.one;
            
            if (io.enabled)
            {
                tiles.attribute = FetchAttribute();
                scroll.ClockX();
            }
            
            phase = &Ppu::HBlankBg4;
        }
        
        void Ppu::HBlankBg4()
        {
            if (io.enabled)
            {
                io.address = io.pattern | 0x0;
                
                if (io.a12 && scroll.pattern)
                    io.a12.Toggle( cycles.count );
            }
            
            cycles.count += cycles.one;
            phase = &Ppu::HBlankBg5;
        }
        
        void Ppu::HBlankBg5()
        {
            cycles.count += cycles.one;
            
            if (io.enabled)
                tiles.pattern[0] = chr.FetchPattern( io.address );
            
            phase = &Ppu::HBlankBg6;
        }
        
        void Ppu::HBlankBg6()
        {
            cycles.count += cycles.one;
            
            if (io.enabled)
                io.address = io.pattern | 0x8;
            
            phase = &Ppu::HBlankBg7;
        }
        
        void Ppu::HBlankBg7()
        {
            if (io.enabled)
                tiles.pattern[1] = chr.FetchPattern( io.address );
            
            (__builtin_expect(!(!!(stage < 2 && scanline < 240)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstPpu.cpp", 1438, "!!(stage < 2 && scanline < 240)") : (void)0);
            
            if (stage ^= 1)
            {
                cycles.count += cycles.one;
                phase = &Ppu::HBlankBg0;
            }
            else if (scanline++ != 239)
            {
                cycles.count += cycles.six;
                tiles.mask = tiles.show[1];
                oam.mask = oam.show[1];
                
                if (scanline != 0)
                {
                    phase = &Ppu::HActive0;
                }
                else
                {
                    output.index = 0;
                    
                    if (regs.ctrl1 & regs.frame)
                    {
                        (__builtin_expect(!(!!(cycles.one == Clocks::RP2C02_CC)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstPpu.cpp", 1461, "!!(cycles.one == Clocks::RP2C02_CC)") : (void)0);
                        
                        output.burstPhase = (output.burstPhase + 2) % 3;
                        cycles.count -= cycles.one;
                        cpu.SetFrameCycles( Clocks::RP2C02_HVSYNC_1 );
                    }
                    else if (cycles.one == Clocks::RP2C02_CC)
                    {
                        output.burstPhase = (output.burstPhase + 1) % 3;
                    }
                    
                    phase = &Ppu::HActive0;
                }
            }
            else
            {
                output.index = ~0U;
                cycles.count = cpu.GetFrameCycles() - cycles.one;
                phase = &Ppu::VBlankIn;
            }
        }
        
        void Ppu::VBlankIn()
        {
            cycles.count += cycles.one;
            regs.status |= Regs::STATUS_VBLANKING;
            phase = &Ppu::VBlank;
        }
        
        void Ppu::VBlank()
        {
            (__builtin_expect(!(!!(scanline != SCANLINE_VBLANK && regs.oam < Oam::SIZE)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstPpu.cpp", 1492, "!!(scanline != SCANLINE_VBLANK && regs.oam < Oam::SIZE)") : (void)0);
            ((void)0);
            
            regs.status = (regs.status & 0xFF) | (regs.status >> 1 & Regs::STATUS_VBLANK);
            cycles.count += cycles.one * 2;
            regs.oam = 0x00;
            scanline = SCANLINE_VBLANK;
            oam.visible = oam.output;
            
            if (cycles.spriteOverflow != Cpu::CYCLE_MAX)
            {
                cycles.spriteOverflow = Cpu::CYCLE_MAX;
                regs.status |= Regs::STATUS_SP_OVERFLOW;
            }
            
            phase = &Ppu::VBlankOut;
        }
        
        void Ppu::VBlankOut()
        {
            cycles.count = Cpu::CYCLE_MAX;
            phase = &Ppu::HDummy;
            
            if (regs.ctrl0 & regs.status & Regs::CTRL0_NMI)
                cpu.DoNMI( cpu.GetFrameCycles() );
        }
        
        void Ppu::HDummy()
        {
            (__builtin_expect(!(!!(scanline == SCANLINE_VBLANK)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstPpu.cpp", 1521, "!!(scanline == SCANLINE_VBLANK)") : (void)0);
            
            cycles.count += cycles.four;
            regs.status = 0;
            scanline = SCANLINE_HDUMMY;
            
            phase = &Ppu::HDummyBg;
        }
        
        void Ppu::HDummyBg()
        {
            (__builtin_expect(!(!!(scanline == SCANLINE_HDUMMY)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstPpu.cpp", 1532, "!!(scanline == SCANLINE_HDUMMY)") : (void)0);
            
        hell:
            
            if (io.a12 && scroll.pattern && io.enabled)
                io.a12.Toggle( cycles.count );
            
            cycles.count += cycles.eight;
            stage = (stage + 1) & 31;
            
            if (stage)
            {
                if (cycles.count < cycles.round)
                    goto hell;
                else
                    phase = &Ppu::HDummyBg;
            }
            else
            {
                phase = &Ppu::HDummySp;
            }
        }
        
        void Ppu::HDummySp()
        {
            (__builtin_expect(!(!!(scanline == SCANLINE_HDUMMY)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstPpu.cpp", 1557, "!!(scanline == SCANLINE_HDUMMY)") : (void)0);
            
        hell:
            
            if (io.a12 && regs.ctrl0 & (Regs::CTRL0_SP_OFFSET|Regs::CTRL0_SP8X16) && io.enabled)
                io.a12.Toggle( cycles.count );
            
            stage = (stage + 1) & 7;
            
            if (stage)
            {
                if (stage != 6)
                {
                    cycles.count += cycles.eight;
                    
                    if (cycles.count < cycles.round)
                        goto hell;
                    else
                        phase = &Ppu::HDummySp;
                }
                else
                {
                    cycles.count += cycles.four;
                    phase = &Ppu::HDummyScroll;
                }
            }
            else
            {
                cycles.count += cycles.four;
                phase = &Ppu::HBlankBg;
            }
        }
        
        void Ppu::HDummyScroll()
        {
            (__builtin_expect(!(!!(scanline == SCANLINE_HDUMMY)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstPpu.cpp", 1592, "!!(scanline == SCANLINE_HDUMMY)") : (void)0);
            
            cycles.count += cycles.four;
            
            if (io.enabled)
                scroll.address = scroll.latch;
            
            phase = &Ppu::HDummySp;
        }
        
        void Ppu::WarmUp()
        {
            (__builtin_expect(!(!!(stage && phase == &Ppu::WarmUp)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstPpu.cpp", 1604, "!!(stage && phase == &Ppu::WarmUp)") : (void)0);
            
            cycles.count = Cpu::CYCLE_MAX;
            
            if (!--stage)
            {
                regs.status |= Regs::STATUS_VBLANK;
                regs.oam = 0x00;
                phase = &Ppu::HDummy;
            }
        }
    }
}
