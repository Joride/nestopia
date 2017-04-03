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
#include "NstCpu.hpp"
#include "NstHook.hpp"
#include "NstState.hpp"
#include "api/NstApiUser.hpp"

// use this to turn of the print statements (with them, emulation is slooooowww)
//#define printf(x, ...)

namespace Nes
{
    namespace Core
    {
        dword Cpu::logged = 0;
        
        void (Cpu::*const Cpu::opcodes[0x100])() =
        {
            &Cpu::op0x00, &Cpu::op0x01, &Cpu::op0x02, &Cpu::op0x03,
            &Cpu::op0x04, &Cpu::op0x05, &Cpu::op0x06, &Cpu::op0x07,
            &Cpu::op0x08, &Cpu::op0x09, &Cpu::op0x0A, &Cpu::op0x0B,
            &Cpu::op0x0C, &Cpu::op0x0D, &Cpu::op0x0E, &Cpu::op0x0F,
            &Cpu::op0x10, &Cpu::op0x11, &Cpu::op0x12, &Cpu::op0x13,
            &Cpu::op0x14, &Cpu::op0x15, &Cpu::op0x16, &Cpu::op0x17,
            &Cpu::op0x18, &Cpu::op0x19, &Cpu::op0x1A, &Cpu::op0x1B,
            &Cpu::op0x1C, &Cpu::op0x1D, &Cpu::op0x1E, &Cpu::op0x1F,
            &Cpu::op0x20, &Cpu::op0x21, &Cpu::op0x22, &Cpu::op0x23,
            &Cpu::op0x24, &Cpu::op0x25, &Cpu::op0x26, &Cpu::op0x27,
            &Cpu::op0x28, &Cpu::op0x29, &Cpu::op0x2A, &Cpu::op0x2B,
            &Cpu::op0x2C, &Cpu::op0x2D, &Cpu::op0x2E, &Cpu::op0x2F,
            &Cpu::op0x30, &Cpu::op0x31, &Cpu::op0x32, &Cpu::op0x33,
            &Cpu::op0x34, &Cpu::op0x35, &Cpu::op0x36, &Cpu::op0x37,
            &Cpu::op0x38, &Cpu::op0x39, &Cpu::op0x3A, &Cpu::op0x3B,
            &Cpu::op0x3C, &Cpu::op0x3D, &Cpu::op0x3E, &Cpu::op0x3F,
            &Cpu::op0x40, &Cpu::op0x41, &Cpu::op0x42, &Cpu::op0x43,
            &Cpu::op0x44, &Cpu::op0x45, &Cpu::op0x46, &Cpu::op0x47,
            &Cpu::op0x48, &Cpu::op0x49, &Cpu::op0x4A, &Cpu::op0x4B,
            &Cpu::op0x4C, &Cpu::op0x4D, &Cpu::op0x4E, &Cpu::op0x4F,
            &Cpu::op0x50, &Cpu::op0x51, &Cpu::op0x52, &Cpu::op0x53,
            &Cpu::op0x54, &Cpu::op0x55, &Cpu::op0x56, &Cpu::op0x57,
            &Cpu::op0x58, &Cpu::op0x59, &Cpu::op0x5A, &Cpu::op0x5B,
            &Cpu::op0x5C, &Cpu::op0x5D, &Cpu::op0x5E, &Cpu::op0x5F,
            &Cpu::op0x60, &Cpu::op0x61, &Cpu::op0x62, &Cpu::op0x63,
            &Cpu::op0x64, &Cpu::op0x65, &Cpu::op0x66, &Cpu::op0x67,
            &Cpu::op0x68, &Cpu::op0x69, &Cpu::op0x6A, &Cpu::op0x6B,
            &Cpu::op0x6C, &Cpu::op0x6D, &Cpu::op0x6E, &Cpu::op0x6F,
            &Cpu::op0x70, &Cpu::op0x71, &Cpu::op0x72, &Cpu::op0x73,
            &Cpu::op0x74, &Cpu::op0x75, &Cpu::op0x76, &Cpu::op0x77,
            &Cpu::op0x78, &Cpu::op0x79, &Cpu::op0x7A, &Cpu::op0x7B,
            &Cpu::op0x7C, &Cpu::op0x7D, &Cpu::op0x7E, &Cpu::op0x7F,
            &Cpu::op0x80, &Cpu::op0x81, &Cpu::op0x82, &Cpu::op0x83,
            &Cpu::op0x84, &Cpu::op0x85, &Cpu::op0x86, &Cpu::op0x87,
            &Cpu::op0x88, &Cpu::op0x89, &Cpu::op0x8A, &Cpu::op0x8B,
            &Cpu::op0x8C, &Cpu::op0x8D, &Cpu::op0x8E, &Cpu::op0x8F,
            &Cpu::op0x90, &Cpu::op0x91, &Cpu::op0x92, &Cpu::op0x93,
            &Cpu::op0x94, &Cpu::op0x95, &Cpu::op0x96, &Cpu::op0x97,
            &Cpu::op0x98, &Cpu::op0x99, &Cpu::op0x9A, &Cpu::op0x9B,
            &Cpu::op0x9C, &Cpu::op0x9D, &Cpu::op0x9E, &Cpu::op0x9F,
            &Cpu::op0xA0, &Cpu::op0xA1, &Cpu::op0xA2, &Cpu::op0xA3,
            &Cpu::op0xA4, &Cpu::op0xA5, &Cpu::op0xA6, &Cpu::op0xA7,
            &Cpu::op0xA8, &Cpu::op0xA9, &Cpu::op0xAA, &Cpu::op0xAB,
            &Cpu::op0xAC, &Cpu::op0xAD, &Cpu::op0xAE, &Cpu::op0xAF,
            &Cpu::op0xB0, &Cpu::op0xB1, &Cpu::op0xB2, &Cpu::op0xB3,
            &Cpu::op0xB4, &Cpu::op0xB5, &Cpu::op0xB6, &Cpu::op0xB7,
            &Cpu::op0xB8, &Cpu::op0xB9, &Cpu::op0xBA, &Cpu::op0xBB,
            &Cpu::op0xBC, &Cpu::op0xBD, &Cpu::op0xBE, &Cpu::op0xBF,
            &Cpu::op0xC0, &Cpu::op0xC1, &Cpu::op0xC2, &Cpu::op0xC3,
            &Cpu::op0xC4, &Cpu::op0xC5, &Cpu::op0xC6, &Cpu::op0xC7,
            &Cpu::op0xC8, &Cpu::op0xC9, &Cpu::op0xCA, &Cpu::op0xCB,
            &Cpu::op0xCC, &Cpu::op0xCD, &Cpu::op0xCE, &Cpu::op0xCF,
            &Cpu::op0xD0, &Cpu::op0xD1, &Cpu::op0xD2, &Cpu::op0xD3,
            &Cpu::op0xD4, &Cpu::op0xD5, &Cpu::op0xD6, &Cpu::op0xD7,
            &Cpu::op0xD8, &Cpu::op0xD9, &Cpu::op0xDA, &Cpu::op0xDB,
            &Cpu::op0xDC, &Cpu::op0xDD, &Cpu::op0xDE, &Cpu::op0xDF,
            &Cpu::op0xE0, &Cpu::op0xE1, &Cpu::op0xE2, &Cpu::op0xE3,
            &Cpu::op0xE4, &Cpu::op0xE5, &Cpu::op0xE6, &Cpu::op0xE7,
            &Cpu::op0xE8, &Cpu::op0xE9, &Cpu::op0xEA, &Cpu::op0xEB,
            &Cpu::op0xEC, &Cpu::op0xED, &Cpu::op0xEE, &Cpu::op0xEF,
            &Cpu::op0xF0, &Cpu::op0xF1, &Cpu::op0xF2, &Cpu::op0xF3,
            &Cpu::op0xF4, &Cpu::op0xF5, &Cpu::op0xF6, &Cpu::op0xF7,
            &Cpu::op0xF8, &Cpu::op0xF9, &Cpu::op0xFA, &Cpu::op0xFB,
            &Cpu::op0xFC, &Cpu::op0xFD, &Cpu::op0xFE, &Cpu::op0xFF
        };
        Cpu::Cpu()
        :
        region ( Region::NTSC ),
        apu ( *this ),
        map ( this, &Cpu::Peek_Overflow, &Cpu::Poke_Overflow )
        {
            Reset( false, false );
            
//            std::string a = ListValuesByAdrres();
//            printf("%s", a.c_str());
            
            cycles.SetRegion( GetRegion() );
        }
        
        
        
        
        
        void Cpu::PowerOff()
        {
            Reset( false, true );
        }
        
        void Cpu::Reset(bool hard)
        {
            Reset( true, hard );
        }
        
        void Cpu::Reset(const bool on,const bool hard)
        {
            if (!on || hard)
            {
                ram.Reset();
                
                a = 0x00;
                x = 0x00;
                y = 0x00;
                sp = 0xFD;
                
                flags.nz = 0U ^ 1U;
                flags.c = 0;
                flags.v = 0;
                flags.d = 0;
            }
            else
            {
                sp = (sp - 3) & 0xFF;
            }
            
            pc = RESET_VECTOR;
            flags.i = Flags::I;
            jammed = false;
            ticks = 0;
            logged = 0;
            
            cycles.count = 0;
            cycles.round = 0;
            cycles.frame = (region == Region::NTSC ? Clocks::RP2C02_HVSYNC : Clocks::RP2C07_HVSYNC);
            
            interrupt.Reset();
            hooks.Clear();
            linker.Clear();
            
            if (on)
            {
                map( 0x0000, 0x07FF ).Set( &ram, &Cpu::Ram::Peek_Ram_0, &Cpu::Ram::Poke_Ram_0 );
                map( 0x0800, 0x0FFF ).Set( &ram, &Cpu::Ram::Peek_Ram_1, &Cpu::Ram::Poke_Ram_1 );
                map( 0x1000, 0x17FF ).Set( &ram, &Cpu::Ram::Peek_Ram_2, &Cpu::Ram::Poke_Ram_2 );
                map( 0x1800, 0x1FFF ).Set( &ram, &Cpu::Ram::Peek_Ram_3, &Cpu::Ram::Poke_Ram_3 );
                map( 0x2000, 0xFFFF ).Set( this, &Cpu::Peek_Nop, &Cpu::Poke_Nop );
                map( 0xFFFC ).Set( this, &Cpu::Peek_Jam_1, &Cpu::Poke_Nop );
                map( 0xFFFD ).Set( this, &Cpu::Peek_Jam_2, &Cpu::Poke_Nop );
                
                apu.Reset( hard );
            }
            else
            {
                map( 0x0000, 0xFFFF ).Set( this, &Cpu::Peek_Nop, &Cpu::Poke_Nop );
                
                if (hard)
                    apu.PowerOff();
            }
            
            cyclesSubtracted = 0; // added by joride for debugging purposes
            printf("AFTER RESET:");
            printCPU();
            
//            std::string a = ListValuesByAdrres();
//            printf("%s", a.c_str());
            
        }
        
        std::string Cpu::ListValuesByAdrres()
        {
            std::string string = "Address\tValue\n";
            for (uint16_t index = 0; index < 0xFFFF; index++)
            {
//                printf("0x%04X\t0x%02X\n", index, map.Peek8(index));
                
                char buffer [16];
                sprintf(buffer, "0x%04X\t0x%02X\n", index, map.Peek8(index));
                std::string toAppend = std::string(buffer);
                string = string + toAppend;
                
                
                
            }
            return string;
        }
        
        void Cpu::SetRegion(const Region::Type r)
        {
            if (region != r)
            {
                region = r;
                cycles.SetRegion( r );
                interrupt.SetRegion( r );
                
                ticks = ticks / uint(r == Region::NTSC ? Clocks::RP2A07_CC : Clocks::RP2A03_CC) *
                uint(r == Region::NTSC ? Clocks::RP2A03_CC : Clocks::RP2A07_CC);
                
                apu.UpdateRegion();
            }
        }
        
        void Cpu::AddHook(const Hook& hook)
        {
            hooks.Add( hook );
        }
        
        void Cpu::RemoveHook(const Hook& hook)
        {
            hooks.Remove( hook );
        }
        
        Cycle Cpu::ClockConvert(Cycle clock,Region::Type region)
        {
            return
            (
             clock / (region == Region::NTSC ? Clocks::RP2A07_CC : Clocks::RP2A03_CC) *
             (region == Region::NTSC ? Clocks::RP2A03_CC : Clocks::RP2A07_CC)
             );
        }
        
        
        
        
        
        bool Cpu::IsOddCycle() const
        {
            return uint((ticks + cycles.count) % cycles.clock[1]);
        }
        
        inline uint Cpu::Cycles::NmiEdge() const
        {
            return clock[0] + clock[0] / 2;
        }
        
        inline uint Cpu::Cycles::IrqEdge() const
        {
            return clock[1];
        }
        
        
        
        
        
        void Cpu::SaveState(State::Saver& state,const dword cpuChunk,const dword apuChunk) const
        {
            state.Begin( cpuChunk );
            
            {
                const byte data[7] =
                {
                    pc & 0xFF,
                    pc >> 8,
                    sp,
                    a,
                    x,
                    y,
                    flags.Pack()
                };
                
                state.Begin( AsciiId<'R','E','G'>::V ).Write( data ).End();
            }
            
            state.Begin( AsciiId<'R','A','M'>::V ).Compress( ram.mem ).End();
            
            {
                const byte data[5] =
                {
                    ((interrupt.nmiClock != CYCLE_MAX) ? 0x01U : 0x00U) |
                    ((interrupt.low & IRQ_FRAME) ? 0x02U : 0x00U) |
                    ((interrupt.low & IRQ_DMC) ? 0x04U : 0x00U) |
                    ((interrupt.low & IRQ_EXT) ? 0x08U : 0x00U) |
                    (jammed ? 0x40U : 0x00U) |
                    (region == Region::PAL ? 0x80U : 0x00U),
                    cycles.count & 0xFF,
                    cycles.count >> 8,
                    (interrupt.nmiClock != CYCLE_MAX) ? interrupt.nmiClock+1 : 0,
                    (interrupt.irqClock != CYCLE_MAX) ? interrupt.irqClock+1 : 0
                };
                
                state.Begin( AsciiId<'F','R','M'>::V ).Write( data ).End();
            }
            
            state.Begin( AsciiId<'C','L','K'>::V ).Write64( ticks ).End();
            
            state.End();
            
            apu.SaveState( state, apuChunk );
        }
        
        void Cpu::LoadState(State::Loader& state,const dword cpuChunk,const dword apuChunk,const dword baseChunk)
        {
            if (baseChunk == cpuChunk)
            {
                Region::Type stateRegion = GetRegion();
                ticks = 0;
                
                while (const dword chunk = state.Begin())
                {
                    switch (chunk)
                    {
                        case AsciiId<'R','E','G'>::V:
                        {
                            State::Loader::Data<7> data( state );
                            
                            pc = data[0] | data[1] << 8;
                            sp = data[2];
                            a = data[3];
                            x = data[4];
                            y = data[5];
                            
                            flags.Unpack( data[6] );
                            break;
                        }
                            
                        case AsciiId<'R','A','M'>::V:
                            
                            state.Uncompress( ram.mem );
                            break;
                            
                        case AsciiId<'F','R','M'>::V:
                        {
                            State::Loader::Data<5> data( state );
                            
                            stateRegion = (data[0] & 0x80) ? Region::PAL : Region::NTSC;
                            
                            interrupt.nmiClock = CYCLE_MAX;
                            interrupt.irqClock = CYCLE_MAX;
                            interrupt.low = 0;
                            
                            if (data[0] & (0x2|0x4|0x8))
                            {
                                interrupt.low =
                                (
                                 ((data[0] & 0x2) ? IRQ_FRAME : 0) |
                                 ((data[0] & 0x4) ? IRQ_DMC : 0) |
                                 ((data[0] & 0x8) ? IRQ_EXT : 0)
                                 );
                                
                                if (!flags.i)
                                    interrupt.irqClock = data[4] ? data[4] - 1 : 0;
                            }
                            
                            cycles.count = data[1] | data[2] << 8;
                            
                            if (data[0] & 0x1)
                                interrupt.nmiClock = data[3] ? data[3] - 1 : cycles.NmiEdge();
                            
                            jammed = data[0] >> 6 & 0x1;
                            
                            if (jammed)
                                interrupt.Reset();
                            
                            break;
                        }
                            
                        case AsciiId<'C','L','K'>::V:
                            
                            ticks = state.Read64();
                            break;
                    }
                    
                    state.End();
                }
                
                const Region::Type actualRegion = GetRegion();
                
                if (stateRegion != actualRegion)
                {
                    cycles.SetRegion( actualRegion );
                    interrupt.SetRegion( actualRegion );
                    
                    ticks = ticks / uint(actualRegion == Region::NTSC ? Clocks::RP2A07_CC : Clocks::RP2A03_CC) *
                    uint(actualRegion == Region::NTSC ? Clocks::RP2A03_CC : Clocks::RP2A07_CC);
                }
                
                if (cycles.count >= cycles.frame)
                    cycles.count = 0;
                
                ticks -= (ticks + cycles.count) % cycles.clock[0];
            }
            else if (baseChunk == apuChunk)
            {
                apu.LoadState( state );
            }
        }
        
        void Cpu::NotifyOp(const char (&code)[4],const dword which)
        {
            if (!(logged & which))
            {
                logged |= which;
                Api::User::eventCallback( Api::User::EVENT_CPU_UNOFFICIAL_OPCODE, code );
            }
        }
        
        Cpu::Hooks::Hooks()
        : hooks(new Hook [2]), size(0), capacity(2) {}
        
        Cpu::Hooks::~Hooks()
        {
            delete [] hooks;
        }
        
        void Cpu::Hooks::Clear()
        {
            size = 0;
        }
        
        void Cpu::Hooks::Add(const Hook& hook)
        {
            for (uint i=0, n=size; i < n; ++i)
            {
                if (hooks[i] == hook)
                    return;
            }
            
            if (size == capacity)
            {
                Hook* const __restrict__ next = new Hook [capacity+1];
                ++capacity;
                
                for (uint i=0, n=size; i < n; ++i)
                    next[i] = hooks[i];
                
                delete [] hooks;
                hooks = next;
            }
            
            hooks[size++] = hook;
        }
        
        void Cpu::Hooks::Remove(const Hook& hook)
        {
            for (uint i=0, n=size; i < n; ++i)
            {
                if (hooks[i] == hook)
                {
                    while (++i < n)
                        hooks[i-1] = hooks[i];
                    
                    --size;
                    return;
                }
            }
        }
        
        
        
        
        
        inline uint Cpu::Hooks::Size() const
        {
            return size;
        }
        
        inline const Hook* Cpu::Hooks::Ptr() const
        {
            return hooks;
        }
        
        
        
        
        
        Cpu::Linker::Chain::Chain(const Port& p,uint a,uint l)
        : Port(p), address(a), level(l) {}
        
        Cpu::Linker::Linker()
        : chain(__null) {}
        
        Cpu::Linker::~Linker()
        {
            Clear();
        }
        
        void Cpu::Linker::Clear()
        {
            if (Chain* next = chain)
            {
                chain = __null;
                
                do
                {
                    Chain* tmp = next->next;
                    delete next;
                    next = tmp;
                }
                while (next);
            }
        }
        
        const Io::Port* Cpu::Linker::Add(const Address address,const uint level,const Io::Port& port,IoMap& map)
        {
            (__builtin_expect(!(!!(level)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstCpu.cpp", 503, "!!(level)") : (void)0);
            
            Chain* const entry = new Chain( port, address, level );
            
            for (Chain *it=chain, *prev=__null; it; prev=it, it=it->next)
            {
                if (it->address == address)
                {
                    (__builtin_expect(!(!!(it->next && it->next->address == address)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstCpu.cpp", 511, "!!(it->next && it->next->address == address)") : (void)0);
                    
                    if (level > it->level)
                    {
                        entry->next = it;
                        
                        if (prev)
                            prev->next = entry;
                        else
                            chain = entry;
                        
                        map(address) = port;
                        
                        return it;
                    }
                    else for (;;)
                    {
                        it = it->next;
                        
                        (__builtin_expect(!(!!(level != it->level)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstCpu.cpp", 530, "!!(level != it->level)") : (void)0);
                        
                        if (level > it->level)
                        {
                            const Chain tmp( *it );
                            *it = *entry;
                            it->next = entry;
                            *entry = tmp;
                            
                            return entry;
                        }
                    }
                }
            }
            
            entry->next = new Chain( map[address], address );
            entry->next->next = __null;
            
            map(address) = port;
            
            if (Chain* it = chain)
            {
                while (it->next)
                    it = it->next;
                
                it->next = entry;
            }
            else
            {
                chain = entry;
            }
            
            return entry->next;
        }
        
        void Cpu::Linker::Remove(const Address address,const Io::Port& port,IoMap& map)
        {
            for (Chain *it=chain, *prev=__null; it; prev=it, it=it->next)
            {
                if (it->address == address && port == *it)
                {
                    const Chain* const next = it->next;
                    
                    (__builtin_expect(!(!!(it->level && next && next->address == address)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstCpu.cpp", 573, "!!(it->level && next && next->address == address)") : (void)0);
                    
                    *it = *next;
                    delete next;
                    
                    if (map(address) == port)
                        map(address) = *it;
                    
                    if (it->level == 0)
                    {
                        if (prev == __null)
                        {
                            it = it->next;
                            delete chain;
                            chain = it;
                        }
                        else if (prev->address != address)
                        {
                            prev->next = it->next;
                            delete it;
                        }
                    }
                    
                    break;
                }
            }
        }
        
        void Cpu::Cycles::SetRegion(Region::Type region)
        {
            count = ClockConvert( count, region );
            
            for (uint i=0; i < 8; ++i)
                clock[i] = (i+1) * (region == Region::NTSC ? Clocks::RP2A03_CC : Clocks::RP2A07_CC);
        }
        
        
        
        
        
        inline void Cpu::Cycles::NextRound(const Cycle next)
        {
            if (round > next)
                round = next;
        }
        
        uint Cpu::Flags::Pack() const
        {
            return
            (
             ((nz | nz >> 1) & N) |
             ((nz & 0xFF) ? 0 : Z) |
             c |
             (v ? V : 0) |
             i |
             d |
             R
             );
        }
        
        void Cpu::Flags::Unpack(const uint f)
        {
            nz = (~f & Z) | ((f & N) << 1);
            c = f & C;
            v = f & V;
            i = f & I;
            d = f & D;
        }
        
        
        
        
        
        void Cpu::Interrupt::Reset()
        {
            nmiClock = CYCLE_MAX;
            irqClock = CYCLE_MAX;
            low = 0;
        }
        
        void Cpu::Interrupt::SetRegion(Region::Type region)
        {
            if (nmiClock != CYCLE_MAX)
                nmiClock = ClockConvert( nmiClock, region );
            
            if (irqClock != CYCLE_MAX)
                irqClock = ClockConvert( nmiClock, region );
        }
        
        
        
        
        
        inline uint Cpu::Interrupt::Clock(const Cycle cycle)
        {
            if (cycle >= nmiClock)
            {
                nmiClock = CYCLE_MAX;
                irqClock = CYCLE_MAX;
                return NMI_VECTOR;
            }
            else if (cycle >= irqClock)
            {
                irqClock = CYCLE_MAX;
                return IRQ_VECTOR;
            }
            else
            {
                return 0;
            }
        }
        
        inline void Cpu::Interrupt::EndFrame(const Cycle frameCycles)
        {
            if (nmiClock != CYCLE_MAX)
            {
                ((void)0);
                nmiClock -= frameCycles;
            }
            
            if (irqClock != CYCLE_MAX)
                irqClock = irqClock > frameCycles ? irqClock - frameCycles : 0;
        }
        
        
        
        
        
        template<typename T,typename U>
        Cpu::IoMap::IoMap(Cpu* cpu,T peek,U poke)
        : Io::Map<SIZE_64K>( cpu, peek, poke ) {}
        
        
        
        
        
        inline uint Cpu::IoMap::Peek8(const uint address) const
        {   
            return ports[address].Peek( address );
        }
        
        inline uint Cpu::IoMap::Peek16(const uint address) const
        {
//            uint peekedAddress = ports[address].Peek( address );
//            uint peekedAddressPlusOne = ports[address + 1].Peek( address + 1 );
            
            Io::Port firstPort = ports[address];
            Io::Port secondPort = ports[address + 1];
            
            uint loByte = firstPort.Peek(address);
            uint hiByte = secondPort.Peek(address + 1);
            
            uint value = loByte | hiByte << 8;
            
            
            return ports[address].Peek( address ) | ports[address + 1].Peek( address + 1 ) << 8;
        }
        
        inline void Cpu::IoMap::Poke8(const uint address,const uint data) const
        {
            Io::Port destPort = ports[address];
            destPort.Poke( address, data );
//            ports[address].Poke( address, data );
        }
        
        
        
        
        
        void Cpu::Ram::Reset()
        {
            std::memset( mem + 0x000, 0xFF, 0x3F0 );
            std::memset( mem + 0x3F0, 0x00, 0x010 );
            std::memset( mem + 0x400, 0xFF, 0x1F0 );
            std::memset( mem + 0x5F0, 0x00, 0x010 );
            std::memset( mem + 0x600, 0xFF, 0x200 );
            
            mem[0x08] = 0xF7;
            mem[0x09] = 0xEF;
            mem[0x0A] = 0xDF;
            mem[0x0F] = 0xBF;
        }
        
        
        
        
        
        Data Cpu::Ram::Peek_Ram_0(void* p_,Address i_)
        {
            return static_cast<Cpu::Ram*>(p_)->Peek_M_Ram_0(i_);
        }
        inline Data Cpu::Ram::Peek_M_Ram_0(Address address)
        {
            return mem[address - 0x0000];
        }
        
        Data Cpu::Ram::Peek_Ram_1(void* p_,Address i_) { return static_cast<Cpu::Ram*>(p_)->Peek_M_Ram_1(i_); } inline Data Cpu::Ram::Peek_M_Ram_1(Address address) { return mem[address - 0x0800]; }
        Data Cpu::Ram::Peek_Ram_2(void* p_,Address i_) { return static_cast<Cpu::Ram*>(p_)->Peek_M_Ram_2(i_); } inline Data Cpu::Ram::Peek_M_Ram_2(Address address) { return mem[address - 0x1000]; }
        Data Cpu::Ram::Peek_Ram_3(void* p_,Address i_) { return static_cast<Cpu::Ram*>(p_)->Peek_M_Ram_3(i_); } inline Data Cpu::Ram::Peek_M_Ram_3(Address address) { return mem[address - 0x1800]; }
        
        void Cpu::Ram::Poke_Ram_0(void* p_,Address i_,Data j_) { static_cast<Cpu::Ram*>(p_)->Poke_M_Ram_0(i_,j_); } inline void Cpu::Ram::Poke_M_Ram_0(Address address,Data data) { mem[address - 0x0000] = data; }
        void Cpu::Ram::Poke_Ram_1(void* p_,Address i_,Data j_) { static_cast<Cpu::Ram*>(p_)->Poke_M_Ram_1(i_,j_); } inline void Cpu::Ram::Poke_M_Ram_1(Address address,Data data) { mem[address - 0x0800] = data; }
        void Cpu::Ram::Poke_Ram_2(void* p_,Address i_,Data j_) { static_cast<Cpu::Ram*>(p_)->Poke_M_Ram_2(i_,j_); } inline void Cpu::Ram::Poke_M_Ram_2(Address address,Data data) { mem[address - 0x1000] = data; }
        void Cpu::Ram::Poke_Ram_3(void* p_,Address i_,Data j_) { static_cast<Cpu::Ram*>(p_)->Poke_M_Ram_3(i_,j_); } inline void Cpu::Ram::Poke_M_Ram_3(Address address,Data data) { mem[address - 0x1800] = data; }
        
        Data Cpu::Peek_Nop(void* p_,Address i_)
        {
//            printf("%s\n", __FUNCTION__);
            return static_cast<Cpu*>(p_)->Peek_M_Nop(i_);
        }
        inline Data Cpu::Peek_M_Nop(Address address)
        {
            return address >> 8;
        }
        
        void Cpu::Poke_Nop(void* p_,Address i_,Data j_) { static_cast<Cpu*>(p_)->Poke_M_Nop(i_,j_); } inline void Cpu::Poke_M_Nop(Address,Data)
        {
        }
        
        Data Cpu::Peek_Overflow(void* p_,Address i_) { return static_cast<Cpu*>(p_)->Peek_M_Overflow(i_); } inline Data Cpu::Peek_M_Overflow(Address address)
        {
            pc &= 0xFFFF;
            return ram.mem[address & 0x7FF];
        }
        
        void Cpu::Poke_Overflow(void* p_,Address i_,Data j_) { static_cast<Cpu*>(p_)->Poke_M_Overflow(i_,j_); } inline void Cpu::Poke_M_Overflow(Address address,Data data)
        {
            pc &= 0xFFFF;
            ram.mem[address & 0x7FF] = data;
        }
        
        Data Cpu::Peek_Jam_1(void* p_,Address i_) { return static_cast<Cpu*>(p_)->Peek_M_Jam_1(i_); } inline Data Cpu::Peek_M_Jam_1(Address)
        {
            pc = (pc - 1) & 0xFFFF;
            return 0xFC;
        }
        
        Data Cpu::Peek_Jam_2(void* p_,Address i_) { return static_cast<Cpu*>(p_)->Peek_M_Jam_2(i_); } inline Data Cpu::Peek_M_Jam_2(Address)
        {
            return 0xFF;
        }
        
        inline uint Cpu::FetchZpg16(const uint address) const
        {
            return ram.mem[address & 0xFF] | uint(ram.mem[(address+1) & 0xFF]) << 8;
        }
        
        inline uint Cpu::FetchPc8()
        {
            
            const uint data = map.Peek8( pc );
            ++pc;
            return data;
        }
        
        inline uint Cpu::FetchPc16()
        {
            const uint data = map.Peek16( pc );
            pc += 2;
            return data;
        }
        
        inline uint Cpu::Imm_R()
        {
            const uint data = FetchPc8();
            cycles.count += cycles.clock[1];
            
            printf("%s value: 0x%2X\n", __FUNCTION__, data);
            
            return data;
        }
        
        uint Cpu::Abs_R()
        {
            uint data = FetchPc16();
            cycles.count += cycles.clock[2];
            uint address = data;
            if (address == 0x2002)
            {
                printf("reading 0x2002");
            }
                
            
            // figuring out how to start up the PPU:
            // what data is returned here causes the CPU.NZ flag to be set
            data = map.Peek8( data );
            cycles.count += cycles.clock[0];
            
            printf("%s  address: 0x%04X\tvalue: 0x%02X\n", __FUNCTION__, address, data);
            
            
            if (data == 0x90)
            {
                // to reset the printed count in printCPU() to zero, to sync
                // up with Nestament
                // (cyclesSubtracted + cycles.count - (7 * 12)) / 12
                cyclesSubtracted = -((int64_t)cycles.count) + 84;
                
                printf("going out of the loop\n");
            }
            
            return data;
        }
        
        uint Cpu::Abs_RW(uint& data)
        {
            printf("%s\n", __FUNCTION__);
            const uint address = FetchPc16();
            cycles.count += cycles.clock[2];
            
            data = map.Peek8( address );
            cycles.count += cycles.clock[0];
            
            map.Poke8( address, data );
            cycles.count += cycles.clock[0];
            
            return address;
        }
        
        inline uint Cpu::Abs_W()
        {
            const uint address = FetchPc16();
            cycles.count += cycles.clock[2];
            
            printf("%s: 0x%4X\tvalue: 0x%2X\n", __FUNCTION__, address, map.Peek8(address));
            
            return address;
        }
        
        
        
        
        
        inline uint Cpu::Zpg_R()
        {
            const uint address = FetchPc8();
            cycles.count += cycles.clock[2];
            
            printf("%s: 0x%4X\tvalue: 0x%2X\n", __FUNCTION__, address, map.Peek8(address));
            
            return ram.mem[address];
        }
        
        inline uint Cpu::Zpg_RW(uint& data)
        {
            
            const uint address = FetchPc8();
            cycles.count += cycles.clock[4];
            data = ram.mem[address];
            
            printf("%s: 0x%04X\tvalue: 0x%02X\n", __FUNCTION__, address, map.Peek8(address));
            
            return address;
        }
        
        inline uint Cpu::Zpg_W()
        {
            const uint address = FetchPc8();
            cycles.count += cycles.clock[2];
            printf("%s: 0x%4X\tvalue: 0x%2X\n", __FUNCTION__, address, map.Peek8(address));
            return address;
        }
        
        inline uint Cpu::ZpgReg_R(uint indexed)
        {
            indexed = (indexed + FetchPc8()) & 0xFF;
            cycles.count += cycles.clock[3];
            
            printf("%s: 0x%4X\tvalue: 0x%2X\n", __FUNCTION__, indexed, ram.mem[indexed]);

            
            return ram.mem[indexed];
        }
        
        inline uint Cpu::ZpgReg_RW(uint& data,uint indexed)
        {
            printf("%s\n", __FUNCTION__);
            indexed = (indexed + FetchPc8()) & 0xFF;
            cycles.count += cycles.clock[5];
            data = ram.mem[indexed];
            return indexed;
        }
        
        inline uint Cpu::ZpgReg_W(uint indexed)
        {
            indexed = (indexed + FetchPc8()) & 0xFF;
            cycles.count += cycles.clock[3];
            
            printf("%s: 0x%4X\tvalue: 0x%2X\n", __FUNCTION__, indexed, ram.mem[indexed]);
            
            return indexed;
        }
        
        
        
        
        
        inline uint Cpu::ZpgX_R()
        {
            printf("%s\n", __FUNCTION__);
            return ZpgReg_R( x );
        }
        inline uint Cpu::ZpgX_RW(uint& data)
        {
            printf("%s\n", __FUNCTION__);
            return ZpgReg_RW( data, x );
        }
        inline uint Cpu::ZpgX_W() { printf("%s\n", __FUNCTION__); return ZpgReg_W( x ); }
        inline uint Cpu::ZpgY_R() { printf("%s\n", __FUNCTION__); return ZpgReg_R( y ); }
        inline uint Cpu::ZpgY_RW(uint& data) { printf("%s\n", __FUNCTION__); return ZpgReg_RW( data, y ); }
        inline uint Cpu::ZpgY_W() { printf("%s\n", __FUNCTION__); return ZpgReg_W( y ); }
        
        
        
        
        
        uint Cpu::AbsReg_R(uint indexed)
        {
            uint data = pc;
            indexed += map.Peek8( data );
            data = (map.Peek8( data + 1 ) << 8) + indexed;
            cycles.count += cycles.clock[2];
            
            if (indexed & 0x100)
            {
                map.Peek8( data - 0x100 );
                cycles.count += cycles.clock[0];
            }
            
            uint address = data; // keep this for printing late
            data = map.Peek8( data );
            pc += 2;
            cycles.count += cycles.clock[0];
            
            printf("%s: %04X\tvalue: %02X\n", __FUNCTION__, address, data);
            
            return data;
        }
        
        uint Cpu::AbsReg_RW(uint& data,uint indexed)
        {
            uint address = pc;
            indexed += map.Peek8( address );
            address = (map.Peek8( address + 1 ) << 8) + indexed;
            
            map.Peek8( address - (indexed & 0x100) );
            pc += 2;
            cycles.count += cycles.clock[3];
            
            data = map.Peek8( address );
            cycles.count += cycles.clock[0];
            
            map.Poke8( address, data );
            cycles.count += cycles.clock[0];
            
            return address;
        }
        
        inline uint Cpu::AbsReg_W(uint indexed)
        {
            printf("%s\n", __FUNCTION__);
            uint address = pc;
            indexed += map.Peek8( address );
            address = (map.Peek8( address + 1 ) << 8) + indexed;
            
            map.Peek8( address - (indexed & 0x100) );
            pc += 2;
            cycles.count += cycles.clock[3];
            
            return address;
        }
        
        
        
        
        
        inline uint Cpu::AbsX_R()
        {
            printf("%s\n", __FUNCTION__);
            return AbsReg_R( x );
        }
        inline uint Cpu::AbsY_R() { printf("%s\n", __FUNCTION__); return AbsReg_R( y ); }
        inline uint Cpu::AbsX_RW(uint& data)
        {
            
            uint returnValue = AbsReg_RW( data, x );
            
            printf("%s: %04X\tvalue: %02X\n", __FUNCTION__, returnValue, map.Peek8(returnValue));
            return returnValue;
        }
        inline uint Cpu::AbsY_RW(uint& data) { printf("%s\n", __FUNCTION__); return AbsReg_RW( data, y ); }
        inline uint Cpu::AbsX_W() { printf("%s\n", __FUNCTION__); return AbsReg_W( x ); }
        inline uint Cpu::AbsY_W() { printf("%s\n", __FUNCTION__); return AbsReg_W( y ); }
        
        
        
        
        
        uint Cpu::IndX_R()
        {
            printf("%s\n", __FUNCTION__);
            uint data = FetchPc8() + x;
            cycles.count += cycles.clock[4];
            data = FetchZpg16( data );
            
            data = map.Peek8( data );
            cycles.count += cycles.clock[0];
            
            return data;
        }
        
        inline uint Cpu::IndX_RW(uint& data)
        {
            printf("%s\n", __FUNCTION__);
            uint address = FetchPc8() + x;
            cycles.count += cycles.clock[4];
            address = FetchZpg16( address );
            
            data = map.Peek8( address );
            cycles.count += cycles.clock[0];
            
            map.Poke8( address, data );
            cycles.count += cycles.clock[0];
            
            return address;
        }
        
        inline uint Cpu::IndX_W()
        {
            printf("%s\n", __FUNCTION__);
            const uint address = FetchPc8() + x;
            cycles.count += cycles.clock[4];
            return FetchZpg16( address );
        }
        
        
        
        
        
        uint Cpu::IndY_R()
        {
            printf("%s\n", __FUNCTION__);
            uint data = FetchPc8();
            cycles.count += cycles.clock[3];
            
            const uint indexed = ram.mem[data] + y;
            data = (uint(ram.mem[(data + 1) & 0xFF]) << 8) + indexed;
            
            if (indexed & 0x100)
            {
                map.Peek8( data - 0x100 );
                cycles.count += cycles.clock[0];
            }
            
            data = map.Peek8( data );
            cycles.count += cycles.clock[0];
            
            return data;
        }
        
        inline uint Cpu::IndY_RW(uint& data)
        {
            printf("%s\n", __FUNCTION__);
            uint address = FetchPc8();
            cycles.count += cycles.clock[4];
            
            const uint indexed = ram.mem[address] + y;
            address = (uint(ram.mem[(address + 1) & 0xFF]) << 8) + indexed;
            map.Peek8( address - (indexed & 0x100) );
            
            data = map.Peek8( address );
            cycles.count += cycles.clock[0];
            
            map.Poke8( address, data );
            cycles.count += cycles.clock[0];
            
            return address;
        }
        
        inline uint Cpu::IndY_W()
        {
            uint address = FetchPc8(); // 0
            cycles.count += cycles.clock[4];
            
            const uint indexed = ram.mem[address] + y;
            uint highByteOfTargetAddress = uint(ram.mem[(address + 1) & 0xFF]); // 7 (0x07)
            uint highByteOfAddress = highByteOfTargetAddress << 8; // 1792 (0x0700)
            uint finalAddress = highByteOfAddress | indexed; // 1792 (0x0700)
            
            address = (uint(ram.mem[(address + 1) & 0xFF]) << 8) + indexed; // 1792 (0x0700)
            
            map.Peek8( address - (indexed & 0x100) );
            
            printf("%s: 0x%4X\tvalue: 0x%04X\n", __FUNCTION__, address, map.Peek8(address));
            return address;
        }
        
        
        
        
        
        template<bool STATE>
        inline void Cpu::Branch(uint tmp)
        {
            if ((!!tmp) == STATE)
            {
                pc = ((tmp=pc+1) + sign_extend_8(uint(map.Peek8( pc )))) & 0xFFFF;
                cycles.count += cycles.clock[2 + ((tmp^pc) >> 8 & 1)];
                printf("%sing to: 0x%4X\n", __FUNCTION__, pc);
            }
            else
            {
                ++pc;
                cycles.count += cycles.clock[1];
//                printf("%s PC: 0x%4X\n", __FUNCTION__, pc);
            }
        }
        
        
        
        
        
        inline void Cpu::StoreMem(const uint address,const uint data)
        {
            map.Poke8( address, data );
            cycles.count += cycles.clock[0];
        }
        
        inline void Cpu::StoreZpg(const uint address,const uint data)
        {
            ram.mem[address] = data;
        }
        
        
        
        
        
        inline void Cpu::Push8(const uint data)
        {
            const uint p = sp;
            sp = (sp - 1) & 0xFF;
            
            ram.mem[0x100+p] = data;
        }
        
        inline void Cpu::Push16(const uint data) // data = 51140
        {   // sp = 255
            const uint p0 = sp; // 255
            const uint p1 = (p0 - 1) & 0xFF; //254
            sp = (p1 - 1) & 0xFF; // 253
            
            ram.mem[0x100+p1] = data & 0xFF; // [510] = 196
            ram.mem[0x100+p0] = data >> 8; // [511] = 199
        }
        
        inline uint Cpu::Pull8()
        {
            sp = (sp + 1) & 0xFF;
            
            return ram.mem[0x100+sp];
        }
        
        inline uint Cpu::Pull16()
        {
            // sp = 240
            const uint p0 = (sp + 1) & 0xFF; // (240 + 1) & 0xFF = 241
            const uint p1 = (p0 + 1) & 0xFF; // (p0 + 1) & 0xFF = 242
            sp = p1;
            
            
            uint returnValue = ram.mem[0x100+p0] | uint(ram.mem[0x100+p1]) << 8; // 226 | 61440 (240)
            return returnValue; // 61666
        }
        
        
        
        
        
        inline void Cpu::Lda(const uint data) {
            printf("%s\n", __FUNCTION__);
            a = data;
            flags.nz = data;
        }
        inline void Cpu::Ldx(const uint data)
        {
            printf("%s (0x%02X)\n", __FUNCTION__, data);
            x = data;
            flags.nz = data;
            
        }
        inline void Cpu::Ldy(const uint data)
        {
            printf("%s\n", __FUNCTION__);
            y = data;
            flags.nz = data;
        }
        
        
        
        
        
        inline uint Cpu::Sta() const
        {
            printf("%s\n", __FUNCTION__);
            return a;
        }
        inline uint Cpu::Stx() const { printf("%s\n", __FUNCTION__); return x; }
        inline uint Cpu::Sty() const
        {
            printf("%s\n", __FUNCTION__);
            return y;
        }
        
        
        
        
        
        inline void Cpu::Tax()
        {
            printf("%s\n", __FUNCTION__);
            cycles.count += cycles.clock[1];
            x = a;
            flags.nz = a;
        }
        
        inline void Cpu::Tay()
        {
            printf("%s\n", __FUNCTION__);
            cycles.count += cycles.clock[1];
            y = a;
            flags.nz = a;
        }
        
        inline void Cpu::Txa()
        {
            printf("%s\n", __FUNCTION__);
            cycles.count += cycles.clock[1];
            a = x;
            flags.nz = x;
        }
        
        inline void Cpu::Tya()
        {
            printf("%s\n", __FUNCTION__);
            cycles.count += cycles.clock[1];
            a = y;
            flags.nz = y;
        }
        
        
        
        
        
        inline void Cpu::JmpAbs()
        {
            printf("%s\n", __FUNCTION__);
            pc = map.Peek16( pc );
            cycles.count += cycles.clock[JMP_ABS_CYCLES-1];
        }
        
        inline void Cpu::JmpInd()
        {
            
            printf("%s\n", __FUNCTION__);
            const uint pos = map.Peek16( pc );
            pc = map.Peek8( pos ) | (map.Peek8( (pos & 0xFF00) | ((pos + 1) & 0x00FF) ) << 8);
            
            cycles.count += cycles.clock[JMP_IND_CYCLES-1];
        }
        
        inline void Cpu::Jsr()
        {
            // pc = 51139
            Push16( pc + 1 ); // c7c3 + 1
            pc = map.Peek16( pc ); // pc = 51139 -> 51175
            cycles.count += cycles.clock[JSR_CYCLES-1];
            printf("absoluteMemoryAddress(): %04X\tvalue: %02X\n", pc, map.Peek8(pc));
            printf("%s\n", __FUNCTION__);
        }
        
        inline void Cpu::Rts()
        {
            printf("%s\n", __FUNCTION__);
            
            /*
             const uint p0 = (sp + 1) & 0xFF;
             const uint p1 = (p0 + 1) & 0xFF;
             sp = p1;
             
             pc = ram.mem[0x100+p0] | uint(ram.mem[0x100+p1]) << 8;
             pc += 1
             */
            
            pc = Pull16() + 1;
            cycles.count += cycles.clock[RTS_CYCLES-1];
        }
        
        inline void Cpu::Rti()
        {
            printf("%s\n", __FUNCTION__);
            cycles.count += cycles.clock[RTI_CYCLES-1];
            
            {
                const uint packed = Pull8();
                pc = Pull16();
                flags.Unpack( packed );
            }
            
            if (!interrupt.low || flags.i)
            {
                interrupt.irqClock = CYCLE_MAX;
            }
            else
            {
                interrupt.irqClock = 0;
                cycles.round = 0;
            }
        }
        
        inline void Cpu::Bne()
        {
            printf("%s\n", __FUNCTION__);
            // this branches if flags.nz & 0x180 ( 0000 0001 1000 0000 ) is not zero
            Branch< true >( flags.nz & 0xFF );
        }
        inline void Cpu::Beq()
        {
            printf("%s\n", __FUNCTION__);
            
            // this branches if flags.nz & 0xFF is zero
            Branch< false >( flags.nz & 0xFF );
        }
        inline void Cpu::Bmi()
        {
            printf("%s\n", __FUNCTION__);
            
            // this branches if flags.nz & 0x180 is not zero
            Branch< true >( flags.nz & 0x180 );
        }
        inline void Cpu::Bpl()
        {
            printf("%s\n", __FUNCTION__);
            
            // this branches if flags.nz & 0x180 is zero
            Branch< false >( flags.nz & 0x180 );
        }
        inline void Cpu::Bcs() {
            printf("%s\n", __FUNCTION__);
            
            // this branches if flags.c is not zero
            Branch< true >( flags.c );
        }
        inline void Cpu::Bcc()
        {
            printf("%s\n", __FUNCTION__);
            
            // this branches if flags.c is zero
            Branch< false >( flags.c );
        }
        inline void Cpu::Bvs()
        {
            printf("%s\n", __FUNCTION__);
            
            // this branches if flags.v is not zero
            Branch< true >( flags.v );
        }
        inline void Cpu::Bvc()
        {
            printf("%s\n", __FUNCTION__);
            
            // this branches if flags.v is zero
            Branch< false >( flags.v );
        }
        
        
        
        
        
        inline void Cpu::Adc(const uint data)
        {
            printf("%s\n", __FUNCTION__);
            
            const uint tmp = a + data + flags.c;
            flags.v = ~(a ^ data) & (a ^ tmp) & 0x80;
            a = tmp & 0xFF;
            flags.nz = a;
            flags.c = tmp >> 8 & 0x1;
        }
        
        inline void Cpu::Sbc(const uint data)
        {
            printf("%s\n", __FUNCTION__);
            Adc( data ^ 0xFF );
        }
        
        
        
        
        
        inline void Cpu::And(const uint data)
        {
            printf("%s\n", __FUNCTION__);
            a &= data;
            flags.nz = a;
        }
        
        inline void Cpu::Ora(const uint data)
        {
            printf("%s\n", __FUNCTION__);
            a |= data;
            flags.nz = a;
        }
        
        inline void Cpu::Eor(const uint data)
        {
            printf("%s\n", __FUNCTION__);
//            int64_t count = (int64_t)(cyclesSubtracted + cycles.count - (7 * 12)) / 12;
//            if (count >= 55007)
//            {
//                printf("next time printCPU is called cycles will be 55017. As if Eor() adds 10 cycles.");
//            }
            
            
            a ^= data;
            flags.nz = a;
        }
        
        inline void Cpu::Bit(const uint data)
        {
            printf("%s\n", __FUNCTION__);
            flags.nz = ((data & a) != 0) | ((data & Flags::N) << 1);
            flags.v = data & Flags::V;
        }
        
        inline void Cpu::Cmp(uint data)
        {
            printf("%s\n", __FUNCTION__);
            data = a - data;
            flags.nz = data & 0xFF;
            flags.c = ~data >> 8 & 0x1;
        }
        
        inline void Cpu::Cpx(uint data)
        {
            printf("%s\n", __FUNCTION__);
            data = x - data;
            flags.nz = data & 0xFF;
            flags.c = ~data >> 8 & 0x1;
        }
        
        inline void Cpu::Cpy(uint data)
        {
            printf("%s\n", __FUNCTION__);
            data = y - data;
            flags.nz = data & 0xFF;
            flags.c = ~data >> 8 & 0x1;
        }
        
        
        inline uint Cpu::Asl(const uint data)
        {
            printf("%s\n", __FUNCTION__);
            flags.c = data >> 7;
            flags.nz = data << 1 & 0xFF;
            return flags.nz;
        }
        
        inline uint Cpu::Lsr(const uint data)
        {
            printf("%s\n", __FUNCTION__);
            flags.c = data & 0x01;
            flags.nz = data >> 1;
            return flags.nz;
        }
        
        inline uint Cpu::Rol(const uint data)
        {
            printf("%s\n", __FUNCTION__);
            flags.nz = (data << 1 & 0xFF) | flags.c;
            flags.c = data >> 7;
            
            return flags.nz;
        }
        
        inline uint Cpu::Ror(const uint data)
        {
            printf("%s\n", __FUNCTION__);
            
            flags.nz = (data >> 1) | (flags.c << 7);
            flags.c = data & 0x01;
            
            return flags.nz;
        }
        
        
        inline uint Cpu::Dec(const uint data)
        {
            
            printf("%s\n", __FUNCTION__);
            flags.nz = (data - 1) & 0xFF;
            return flags.nz;
        }
        
        inline uint Cpu::Inc(const uint data)
        {
            printf("%s\n", __FUNCTION__);
            flags.nz = (data + 1) & 0xFF;
            return flags.nz;
        }
        
        inline void Cpu::Dex()
        {
            printf("%s\n", __FUNCTION__);
            cycles.count += cycles.clock[1];
            x = (x - 1) & 0xFF;
            flags.nz = x;
        }
        
        inline void Cpu::Dey()
        {
            printf("%s\n", __FUNCTION__);
            cycles.count += cycles.clock[1];
            y = (y - 1) & 0xFF;
            flags.nz = y;
        }
        
        inline void Cpu::Inx()
        {
            printf("%s\n", __FUNCTION__);
            cycles.count += cycles.clock[1];
            x = (x + 1) & 0xFF;
            flags.nz = x;
        }
        
        inline void Cpu::Iny()
        {
            printf("%s\n", __FUNCTION__);
            cycles.count += cycles.clock[1];
            y = (y + 1) & 0xFF;
            flags.nz = y;
        }
        
        
        
        
        
        inline void Cpu::Clc()
        {
            printf("%s\n", __FUNCTION__);
            cycles.count += cycles.clock[1];
            flags.c = 0;
        }
        
        inline void Cpu::Sec()
        {
            printf("%s\n", __FUNCTION__);
            cycles.count += cycles.clock[1];
            flags.c = Flags::C;
        }
        
        inline void Cpu::Cld()
        {
            printf("%s\n", __FUNCTION__);
            cycles.count += cycles.clock[1];
            flags.d = 0;
        }
        
        inline void Cpu::Sed()
        {
            printf("%s\n", __FUNCTION__);
            cycles.count += cycles.clock[1];
            flags.d = Flags::D;
        }
        
        inline void Cpu::Clv()
        {
            printf("%s\n", __FUNCTION__);
            cycles.count += cycles.clock[1];
            flags.v = 0;
        }
        
        inline void Cpu::Sei()
        {
            printf("%s\n", __FUNCTION__);
            cycles.count += cycles.clock[1];
            
            if (!flags.i)
            {
                flags.i = Flags::I;
                interrupt.irqClock = CYCLE_MAX;
                
                
                
                if (interrupt.low)
                    DoISR( IRQ_VECTOR );
            }
        }
        
        inline void Cpu::Cli()
        {
            printf("%s\n", __FUNCTION__);
            cycles.count += cycles.clock[1];
            
            if (flags.i)
            {
                flags.i = 0;
                
                ((void)0);
                
                
                
                if (interrupt.low)
                {
                    interrupt.irqClock = cycles.count + 1;
                    cycles.NextRound( interrupt.irqClock );
                }
            }
        }
        
        
        
        
        
        inline void Cpu::Pha()
        {
            printf("%s\n", __FUNCTION__);
            cycles.count += cycles.clock[PHA_CYCLES-1];
            Push8( a );
        }
        
        inline void Cpu::Php()
        {
            
            printf("%s\n", __FUNCTION__);
            cycles.count += cycles.clock[PHP_CYCLES-1];
            Push8( flags.Pack() | Flags::B );
        }
        
        inline void Cpu::Pla()
        {
            printf("%s\n", __FUNCTION__);
            cycles.count += cycles.clock[PLA_CYCLES-1];
            a = Pull8();
            flags.nz = a;
        }
        
        inline void Cpu::Plp()
        {
            printf("%s\n", __FUNCTION__);
            cycles.count += cycles.clock[PLP_CYCLES-1];
            
            const uint i = flags.i;
            flags.Unpack( Pull8() );
            
            
            
            if (interrupt.low)
            {
                if (i > flags.i)
                {
                    interrupt.irqClock = cycles.count + 1;
                    cycles.NextRound( interrupt.irqClock );
                }
                else if (i < flags.i)
                {
                    interrupt.irqClock = CYCLE_MAX;
                    DoISR( IRQ_VECTOR );
                }
            }
        }
        
        inline void Cpu::Tsx()
        {
            printf("%s\n", __FUNCTION__);
            cycles.count += cycles.clock[1];
            x = sp;
            flags.nz = sp;
        }
        
        inline void Cpu::Txs()
        {
            printf("%s\n", __FUNCTION__);
            cycles.count += cycles.clock[1];
            sp = x;
        }
        
        __attribute__((noinline)) void Cpu::Anc(const uint data)
        {
            printf("%s\n", __FUNCTION__);
            a &= data;
            flags.nz = a;
            flags.c = flags.nz >> 7;
            NotifyOp("ANC",1UL << 0);
        }
        
        inline void Cpu::Ane(const uint data)
        {
            printf("%s\n", __FUNCTION__);
            a = (a | 0xEE) & x & data;
            flags.nz = a;
            NotifyOp("ANE",1UL << 1);
        }
        
        inline void Cpu::Arr(const uint data)
        {
            printf("%s\n", __FUNCTION__);
            a = ((data & a) >> 1) | (flags.c << 7);
            flags.nz = a;
            flags.c = a >> 6 & 0x1;
            flags.v = (a >> 6 ^ a >> 5) & 0x1;
            NotifyOp("ARR",1UL << 2);
        }
        
        inline void Cpu::Asr(const uint data)
        {
            printf("%s\n", __FUNCTION__);
            flags.c = data & a & 0x1;
            a = (data & a) >> 1;
            flags.nz = a;
            NotifyOp("ASR",1UL << 3);
        }
        
        __attribute__((noinline)) uint Cpu::Dcp(uint data)
        {
            printf("%s\n", __FUNCTION__);
            data = (data - 1) & 0xFF;
            Cmp( data );
            NotifyOp("DCP",1UL << 4);
            return data;
        }
        
        __attribute__((noinline)) uint Cpu::Isb(uint data)
        {
            printf("%s\n", __FUNCTION__);
            data = (data + 1) & 0xFF;
            Sbc( data );
            NotifyOp("ISB",1UL << 5);
            return data;
        }
        
        inline void Cpu::Las(const uint data)
        {
            printf("%s\n", __FUNCTION__);
            sp &= data;
            x = sp;
            a = sp;
            flags.nz = sp;
            NotifyOp("LAS",1UL << 6);
        }
        
        __attribute__((noinline)) void Cpu::Lax(const uint data)
        {
            printf("%s\n", __FUNCTION__);
            a = data;
            x = data;
            flags.nz = data;
            NotifyOp("LAX",1UL << 7);
        }
        
        inline void Cpu::Lxa(const uint data)
        {
            printf("%s\n", __FUNCTION__);
            a &= data;
            x = a;
            flags.nz = a;
            NotifyOp("LXA",1UL << 8);
        }
        
        __attribute__((noinline)) uint Cpu::Rla(uint data)
        {
            printf("%s\n", __FUNCTION__);
            const uint carry = flags.c;
            flags.c = data >> 7;
            data = (data << 1 & 0xFF) | carry;
            a &= data;
            flags.nz = a;
            NotifyOp("RLA",1UL << 9);
            return data;
        }
        
        __attribute__((noinline)) uint Cpu::Rra(uint data)
        {
            printf("%s\n", __FUNCTION__);
            const uint carry = flags.c << 7;
            flags.c = data & 0x01;
            data = (data >> 1) | carry;
            Adc( data );
            NotifyOp("RRA",1UL << 10);
            return data;
        }
        
        __attribute__((noinline)) uint Cpu::Sax()
        {
            printf("%s\n", __FUNCTION__);
            const uint data = a & x;
            NotifyOp("SAX",1UL << 11);
            return data;
        }
        
        inline void Cpu::Sbx(uint data)
        {
            printf("%s\n", __FUNCTION__);
            data = (a & x) - data;
            flags.c = (data <= 0xFF);
            x = data & 0xFF;
            flags.nz = x;
            NotifyOp("SBX",1UL << 12);
        }
        
        __attribute__((noinline)) uint Cpu::Sha(uint address)
        {
            printf("%s\n", __FUNCTION__);
            address = a & x & ((address >> 8) + 1);
            NotifyOp("SHA",1UL << 13);
            return address;
        }
        
        inline uint Cpu::Shs(uint address)
        {
            printf("%s\n", __FUNCTION__);
            sp = a & x;
            address = sp & ((address >> 8) + 1);
            NotifyOp("SHS",1UL << 14);
            return address;
        }
        
        inline uint Cpu::Shx(uint address)
        {
            printf("%s\n", __FUNCTION__);
            address = x & ((address >> 8) + 1);
            NotifyOp("SHX",1UL << 15);
            return address;
        }
        
        inline uint Cpu::Shy(uint address)
        {
            printf("%s\n", __FUNCTION__);
            address = y & ((address >> 8) + 1);
            NotifyOp("SHY",1UL << 16);
            return address;
        }
        
        __attribute__((noinline)) uint Cpu::Slo(uint data)
        {
            printf("%s\n", __FUNCTION__);
            flags.c = data >> 7;
            data = data << 1 & 0xFF;
            a |= data;
            flags.nz = a;
            NotifyOp("SLO",1UL << 17);
            return data;
        }
        
        __attribute__((noinline)) uint Cpu::Sre(uint data)
        {
            printf("%s\n", __FUNCTION__);
            flags.c = data & 0x01;
            data >>= 1;
            a ^= data;
            flags.nz = a;
            NotifyOp("SRE",1UL << 18);
            return data;
        }
        
        void Cpu::Dop()
        {
            printf("%s\n", __FUNCTION__);
            NotifyOp("DOP",1UL << 19);
        }
        
        void Cpu::Top(uint=0)
        {
            printf("%s\n", __FUNCTION__);
            NotifyOp("TOP",1UL << 20);
        }
        
        
        
        
        
        inline void Cpu::Brk()
        {
            printf("%s\n", __FUNCTION__);
            Push16( pc + 1 );
            Push8( flags.Pack() | Flags::B );
            flags.i = Flags::I;
            
            const uint vector = interrupt.Clock( cycles.count + cycles.clock[2] );
            
            pc = map.Peek16( vector ? vector : IRQ_VECTOR );
            cycles.count += cycles.clock[BRK_CYCLES-1];
        }
        
        __attribute__((noinline)) void Cpu::Jam()
        {
            
            printf("%s\n", __FUNCTION__);
            
            pc = (pc - 1) & 0xFFFF;
            cycles.count += cycles.clock[1];
            
            if (!jammed)
            {
                jammed = true;
                interrupt.Reset();
                Api::User::eventCallback( Api::User::EVENT_CPU_JAM );
            }
        }
        
        
        
        
        
        void Cpu::DoISR(const uint vector)
        {
            printf("%s\n", __FUNCTION__);
            if (!jammed)
            {
                Push16( pc );
                Push8( flags.Pack() );
                flags.i = Flags::I;
                pc = map.Peek16( vector );
                cycles.count += cycles.clock[INT_CYCLES-1];
            }
        }
        
        void Cpu::DoIRQ(const IrqLine line,const Cycle cycle)
        {
            printf("%s\n", __FUNCTION__);
            interrupt.low |= line;
            
            if (!flags.i && interrupt.irqClock == CYCLE_MAX)
            {
                interrupt.irqClock = cycle + cycles.IrqEdge();
                cycles.NextRound( interrupt.irqClock );
            }
        }
        
        void Cpu::DoNMI(const Cycle cycle)
        {
            printf("%s\n", __FUNCTION__);
            if (interrupt.nmiClock == CYCLE_MAX)
            {
                interrupt.nmiClock = cycle + cycles.NmiEdge();
                cycles.NextRound( interrupt.nmiClock );
            }
        }
        
        
        
        
        
        void Cpu::Boot()
        {
            printf("%s\n", __FUNCTION__);
            cycles.count = cycles.clock[RESET_CYCLES-1];
            cycles.round = 0;
            
            uint value = map.Peek16( RESET_VECTOR );
            
            pc = value;
        }
        
        void Cpu::ExecuteFrame(Sound::Output* sound)
        {
            printf("%s\n", __FUNCTION__);
            
            apu.BeginFrame( sound );
            
            Clock();
            
            switch (hooks.Size())
            {
                case 0: Run0(); break;
                case 1: Run1(); break;
                default: Run2(); break;
            }
//            uint value = map.Peek8(0x2000);
//            printf("\n>>0x2000:\t0x%02X\n", value);
//            if (value == 0x90)
//            {
//                
//                printf("--SET--");
//            }
        }
        
        void Cpu::EndFrame()
        {
            printf("%s\n", __FUNCTION__);
            apu.EndFrame();
            
            
            
            ticks += cycles.frame;
            cycles.count -= cycles.frame;
            cyclesSubtracted += cycles.frame;
            interrupt.EndFrame( cycles.frame );
        }
        
        void Cpu::Clock()
        {
            Cycle clock = apu.Clock( cycles.count );
            
            if (const uint vector = interrupt.Clock( cycles.count ))
                DoISR( vector );
            
            if (clock > interrupt.irqClock)
                clock = interrupt.irqClock;
            
            if (clock > interrupt.nmiClock)
                clock = interrupt.nmiClock;
            
            if (clock > cycles.frame)
                clock = cycles.frame;
            
            cycles.round = clock;
        }
        
        void Cpu::printCPU()
        {
            printf("\nCPU-cc:\t%ld\nsp:\t0x%02X\npc:\t0x%04X\nA:\t0x%02X\nX:\t0x%02X\nY:\t0x%02X\n",(int64_t)(cyclesSubtracted + cycles.count - (7 * 12)) / 12, sp, pc, a, x, y);
            
            /*
             C = 0x01,  // carry
             Z = 0x02,  // zero
             I = 0x04,  // interrupt enable/disable
             D = 0x08,  // decimal mode (not supported on the N2A03)
             B = 0x10,  // software interrupt
             R = 0x20,  // unused but always set
             V = 0x40,  // overflow
             N = 0x80   // negative
             */
            uint status = flags.Pack();
            printf("P:\t0x%02X\n", status);
            printf("\tC:\t%s\n", (status & 0x01) == 0x01 ? "1" : "0");
            printf("\tZ:\t%s\n", (status & 0x02) == 0x02 ? "1" : "0");
            printf("\tI:\t%s\n", (status  & 0x04) == 0x04 ? "1" : "0");
            printf("\tD:\t%s\n", (status  & 0x08) == 0x08 ? "1" : "0");
            printf("\tB:\t%s\n", (status & 0x10) == 0x10 ? "1" : "0");
            printf("\tR:\t%s\n", (status  & 0x20) == 0x20 ? "1" : "0");
            printf("\tV:\t%s\n", (status & 0x40) == 0x40 ? "1" : "0");
            printf("\tN:\t%s\n", (status & 0x80) == 0x80 ? "1" : "0");
            
//            printf("N	V	-	B	D	I	Z	C\n%s	%s	-	%s	%s	%s	%s	%s\n",
//                   ((flags.nz & Flags::N) == Flags::N) ? "1" : "0",
//                   ((flags.v & Flags::V) == Flags::V) ? "1" : "0",
//                   ((flags.B & Flags::B) == Flags::B) ? "1" : "0",
//                   ((flags.D & Flags::D) == Flags::D) ? "1" : "0",
//                   ((flags.I & Flags::I) == Flags::I) ? "1" : "0",
//                   ((flags.nz & Flags::Z) == Flags::Z) ? "1" : "0",
//                   ((flags.C & Flags::C) == Flags::C) ? "1" : "0");
            
//            if ((flags.C & Flags::C) == 0)
//            {
//                printf("caryyflag zero");
//            }
        }
        
        void Cpu::Run0()
        {
            do
            {
                do
                {
                    printCPU();
                    uint pc = FetchPc8(); // 800B
                    (*this.*opcodes[pc])();
                    
                }
                while (cycles.count < cycles.round);
                
                Clock();
            }
            while (cycles.count < cycles.frame);
        }
        
        void Cpu::Run1()
        {
            const Hook hook( *hooks.Ptr() );
            
            do
            {
                do
                {
                    printf("\n\n%s:\n", __FUNCTION__);
                    printCPU();
                    (*this.*opcodes[FetchPc8()])();
                    hook.Execute();
                }
                while (cycles.count < cycles.round);
                
                Clock();
            }
            while (cycles.count < cycles.frame);
        }
        
        void Cpu::Run2()
        {
            const Hook* const first = hooks.Ptr();
            const Hook* const last = first + (hooks.Size() - 1);
            
            do
            {
                do
                {
                    printf("\n\n%s:\n", __FUNCTION__);
                    printCPU();
                    (*this.*opcodes[FetchPc8()])();
                    
                    const Hook* __restrict__ hook = first;
                    
                    hook->Execute();
                    
                    do
                    {
                        (++hook)->Execute();
                    }
                    while (hook != last);
                }
                while (cycles.count < cycles.round);
                
                Clock();
            }
            while (cycles.count < cycles.frame);
        }
        
        uint Cpu::Peek(const uint address) const
        {
            return map.Peek8( address );
        }
        
        void Cpu::Poke(const uint address,const uint data) const
        {
            return map.Poke8( address, data );
        }
        
        void Cpu::op0x69() { printf("%s\n", __FUNCTION__); Adc( Imm_R() ); }
        void Cpu::op0x65() { printf("%s\n", __FUNCTION__); Adc( Zpg_R() ); }
        void Cpu::op0x75() { printf("%s\n", __FUNCTION__); Adc( ZpgX_R() ); }
        void Cpu::op0x6D() { printf("%s\n", __FUNCTION__); Adc( Abs_R() ); }
        void Cpu::op0x7D() { printf("%s\n", __FUNCTION__); Adc( AbsX_R() ); }
        void Cpu::op0x79() { printf("%s\n", __FUNCTION__); Adc( AbsY_R() ); }
        void Cpu::op0x61() { printf("%s\n", __FUNCTION__); Adc( IndX_R() ); }
        void Cpu::op0x71() { printf("%s\n", __FUNCTION__); Adc( IndY_R() ); }
        void Cpu::op0x29() { printf("%s\n", __FUNCTION__); And( Imm_R() ); }
        void Cpu::op0x25() { printf("%s\n", __FUNCTION__); And( Zpg_R() ); }
        void Cpu::op0x35() { printf("%s\n", __FUNCTION__); And( ZpgX_R() ); }
        void Cpu::op0x2D() { printf("%s\n", __FUNCTION__); And( Abs_R() ); }
        void Cpu::op0x3D() { printf("%s\n", __FUNCTION__); And( AbsX_R() ); }
        void Cpu::op0x39() { printf("%s\n", __FUNCTION__); And( AbsY_R() ); }
        void Cpu::op0x21() { printf("%s\n", __FUNCTION__); And( IndX_R() ); }
        void Cpu::op0x31() { printf("%s\n", __FUNCTION__); And( IndY_R() ); }
        void Cpu::op0x0A() { printf("%s\n", __FUNCTION__); cycles.count += cycles.clock[1]; a = Asl( a ); }
        void Cpu::op0x06() { printf("%s\n", __FUNCTION__); uint data; const uint dst = Zpg_RW( data ); StoreZpg( dst, Asl(data) ); }
        void Cpu::op0x16() { printf("%s\n", __FUNCTION__); uint data; const uint dst = ZpgX_RW( data ); StoreZpg(dst,Asl(data)); }
        void Cpu::op0x0E() { printf("%s\n", __FUNCTION__); uint data; const uint dst = Abs_RW( data ); StoreMem(dst,Asl(data)); }
        void Cpu::op0x1E() { printf("%s\n", __FUNCTION__); uint data; const uint dst = AbsX_RW( data ); StoreMem(dst,Asl(data)); }
        void Cpu::op0x90() { printf("%s\n", __FUNCTION__); Bcc(); }
        void Cpu::op0xB0() { printf("%s\n", __FUNCTION__); Bcs(); }
        void Cpu::op0xF0() { printf("%s\n", __FUNCTION__); Beq(); }
        void Cpu::op0x24() { printf("%s\n", __FUNCTION__); Bit( Zpg_R() ); }
        void Cpu::op0x2C() { printf("%s\n", __FUNCTION__); Bit( Abs_R() ); }
        void Cpu::op0x30() { printf("%s\n", __FUNCTION__); Bmi(); }
        void Cpu::op0xD0() { printf("%s\n", __FUNCTION__); Bne(); }
        void Cpu::op0x10()
        {
            printf("%s\n", __FUNCTION__);
            Bpl();
        }
        void Cpu::op0x50() { printf("%s\n", __FUNCTION__); Bvc(); }
        void Cpu::op0x70() { printf("%s\n", __FUNCTION__); Bvs(); }
        void Cpu::op0x18() { printf("%s\n", __FUNCTION__); Clc(); }
        void Cpu::op0xD8() { printf("%s\n", __FUNCTION__); Cld(); }
        void Cpu::op0x58() { printf("%s\n", __FUNCTION__); Cli(); }
        void Cpu::op0xB8() { printf("%s\n", __FUNCTION__); Clv(); }
        void Cpu::op0xC9() { printf("%s\n", __FUNCTION__); Cmp( Imm_R() ); }
        void Cpu::op0xC5() { printf("%s\n", __FUNCTION__); Cmp( Zpg_R() ); }
        void Cpu::op0xD5() { printf("%s\n", __FUNCTION__); Cmp( ZpgX_R() ); }
        void Cpu::op0xCD() { printf("%s\n", __FUNCTION__); Cmp( Abs_R() ); }
        void Cpu::op0xDD() { printf("%s\n", __FUNCTION__); Cmp( AbsX_R() ); }
        void Cpu::op0xD9() { printf("%s\n", __FUNCTION__); Cmp( AbsY_R() ); }
        void Cpu::op0xC1() { printf("%s\n", __FUNCTION__); Cmp( IndX_R() ); }
        void Cpu::op0xD1() { printf("%s\n", __FUNCTION__); Cmp( IndY_R() ); }
        void Cpu::op0xE0() { printf("%s\n", __FUNCTION__); Cpx( Imm_R() ); }
        void Cpu::op0xE4() { printf("%s\n", __FUNCTION__); Cpx( Zpg_R() ); }
        void Cpu::op0xEC() { printf("%s\n", __FUNCTION__); Cpx( Abs_R() ); }
        void Cpu::op0xC0() { printf("%s\n", __FUNCTION__); Cpy( Imm_R() ); }
        void Cpu::op0xC4() { printf("%s\n", __FUNCTION__); Cpy( Zpg_R() ); }
        void Cpu::op0xCC() { printf("%s\n", __FUNCTION__); Cpy( Abs_R() ); }
        void Cpu::op0xC6() { printf("%s\n", __FUNCTION__); uint data; const uint dst = Zpg_RW( data ); StoreZpg( dst, Dec(data) ); }
        void Cpu::op0xD6() { printf("%s\n", __FUNCTION__); uint data; const uint dst = ZpgX_RW( data ); StoreZpg(dst,Dec(data)); }
        void Cpu::op0xCE() { printf("%s\n", __FUNCTION__); uint data; const uint dst = Abs_RW( data ); StoreMem(dst,Dec(data)); }
        void Cpu::op0xDE() { printf("%s\n", __FUNCTION__); uint data; const uint dst = AbsX_RW( data ); StoreMem(dst,Dec(data)); }
        void Cpu::op0xCA() { printf("%s\n", __FUNCTION__); Dex(); }
        void Cpu::op0x88() { printf("%s\n", __FUNCTION__); Dey(); }
        void Cpu::op0x49() { printf("%s\n", __FUNCTION__); Eor( Imm_R() ); }
        void Cpu::op0x45() { printf("%s\n", __FUNCTION__); Eor( Zpg_R() ); }
        void Cpu::op0x55() { printf("%s\n", __FUNCTION__); Eor( ZpgX_R() ); }
        void Cpu::op0x4D() { printf("%s\n", __FUNCTION__); Eor( Abs_R() ); }
        void Cpu::op0x5D() { printf("%s\n", __FUNCTION__); Eor( AbsX_R() ); }
        void Cpu::op0x59() { printf("%s\n", __FUNCTION__); Eor( AbsY_R() ); }
        void Cpu::op0x41() { printf("%s\n", __FUNCTION__); Eor( IndX_R() ); }
        void Cpu::op0x51() { printf("%s\n", __FUNCTION__); Eor( IndY_R() ); }
        void Cpu::op0xE6()
        {
            printf("%s\n", __FUNCTION__);
            uint data;
            const uint dst = Zpg_RW( data );
            StoreZpg( dst, Inc(data) );
        }
        void Cpu::op0xF6() { printf("%s\n", __FUNCTION__); uint data; const uint dst = ZpgX_RW( data ); StoreZpg(dst,Inc(data)); }
        void Cpu::op0xEE() { printf("%s\n", __FUNCTION__); uint data; const uint dst = Abs_RW( data ); StoreMem(dst,Inc(data)); }
        void Cpu::op0xFE() {
            printf("%s\n", __FUNCTION__);
            
            uint data;
            const uint dst = AbsX_RW( data );
            StoreMem(dst,Inc(data));
        }
        void Cpu::op0xE8() { printf("%s\n", __FUNCTION__); Inx(); }
        void Cpu::op0xC8() { printf("%s\n", __FUNCTION__); Iny(); }
        void Cpu::op0x4C() { printf("%s\n", __FUNCTION__); JmpAbs(); }
        void Cpu::op0x6C() { printf("%s\n", __FUNCTION__); JmpInd(); }
        void Cpu::op0x20() { printf("%s\n", __FUNCTION__); Jsr(); }
        void Cpu::op0xA9() { printf("%s\n", __FUNCTION__); Lda( Imm_R() ); }
        void Cpu::op0xA5() { printf("%s\n", __FUNCTION__); Lda( Zpg_R() ); }
        void Cpu::op0xB5() { printf("%s\n", __FUNCTION__); Lda( ZpgX_R() );}
        void Cpu::op0xAD()
        {
            printf("%s\n", __FUNCTION__);
            Lda( Abs_R() );
        }
        void Cpu::op0xBD() { printf("%s\n", __FUNCTION__); Lda( AbsX_R() ); }
        void Cpu::op0xB9() { printf("%s\n", __FUNCTION__); Lda( AbsY_R() ); }
        void Cpu::op0xA1() { printf("%s\n", __FUNCTION__); Lda( IndX_R() ); }
        void Cpu::op0xB1() { printf("%s\n", __FUNCTION__); Lda( IndY_R() ); }
        void Cpu::op0xA2() { printf("%s\n", __FUNCTION__); Ldx( Imm_R() ); }
        void Cpu::op0xA6() { printf("%s\n", __FUNCTION__); Ldx( Zpg_R() ); }
        void Cpu::op0xB6() { printf("%s\n", __FUNCTION__); Ldx( ZpgY_R() ); }
        void Cpu::op0xAE() { printf("%s\n", __FUNCTION__); Ldx( Abs_R() ); }
        void Cpu::op0xBE() { printf("%s\n", __FUNCTION__); Ldx( AbsY_R() ); }
        void Cpu::op0xA0() { printf("%s\n", __FUNCTION__); Ldy( Imm_R() ); }
        void Cpu::op0xA4() { printf("%s\n", __FUNCTION__); Ldy( Zpg_R() ); }
        void Cpu::op0xB4() { printf("%s\n", __FUNCTION__); Ldy( ZpgX_R() ); }
        void Cpu::op0xAC() { printf("%s\n", __FUNCTION__); Ldy( Abs_R() ); }
        void Cpu::op0xBC() { printf("%s\n", __FUNCTION__); Ldy( AbsX_R() ); }
        void Cpu::op0x4A() { printf("%s\n", __FUNCTION__); cycles.count += cycles.clock[1]; a = Lsr( a ); }
        void Cpu::op0x46() { printf("%s\n", __FUNCTION__); uint data; const uint dst = Zpg_RW( data ); StoreZpg( dst, Lsr(data) ); }
        void Cpu::op0x56() { printf("%s\n", __FUNCTION__); uint data; const uint dst = ZpgX_RW( data ); StoreZpg(dst,Lsr(data)); }
        void Cpu::op0x4E() { printf("%s\n", __FUNCTION__); uint data; const uint dst = Abs_RW( data ); StoreMem(dst,Lsr(data)); }
        void Cpu::op0x5E() { printf("%s\n", __FUNCTION__); uint data; const uint dst = AbsX_RW( data ); StoreMem(dst,Lsr(data)); }
        void Cpu::op0x1A() { printf("%s\n", __FUNCTION__); cycles.count += cycles.clock[2 - 1]; }
        void Cpu::op0x3A() { printf("%s\n", __FUNCTION__); cycles.count += cycles.clock[2 - 1]; }
        void Cpu::op0x5A() { printf("%s\n", __FUNCTION__); cycles.count += cycles.clock[2 - 1]; }
        void Cpu::op0x7A() { printf("%s\n", __FUNCTION__); cycles.count += cycles.clock[2 - 1]; }
        void Cpu::op0xDA() { printf("%s\n", __FUNCTION__); cycles.count += cycles.clock[2 - 1]; }
        void Cpu::op0xEA() { printf("%s\n", __FUNCTION__); cycles.count += cycles.clock[2 - 1]; }
        void Cpu::op0xFA() { printf("%s\n", __FUNCTION__); cycles.count += cycles.clock[2 - 1]; }
        void Cpu::op0x09() { printf("%s\n", __FUNCTION__); Ora( Imm_R() ); }
        void Cpu::op0x05() { printf("%s\n", __FUNCTION__); Ora( Zpg_R() ); }
        void Cpu::op0x15() { printf("%s\n", __FUNCTION__); Ora( ZpgX_R() ); }
        void Cpu::op0x0D() { printf("%s\n", __FUNCTION__); Ora( Abs_R() ); }
        void Cpu::op0x1D() { printf("%s\n", __FUNCTION__); Ora( AbsX_R() ); }
        void Cpu::op0x19() { printf("%s\n", __FUNCTION__); Ora( AbsY_R() ); }
        void Cpu::op0x01() { printf("%s\n", __FUNCTION__); Ora( IndX_R() ); }
        void Cpu::op0x11() { printf("%s\n", __FUNCTION__); Ora( IndY_R() ); }
        void Cpu::op0x48() { printf("%s\n", __FUNCTION__); Pha(); }
        void Cpu::op0x08() { printf("%s\n", __FUNCTION__); Php(); }
        void Cpu::op0x68() { printf("%s\n", __FUNCTION__); Pla(); }
        void Cpu::op0x28() { printf("%s\n", __FUNCTION__); Plp(); }
        void Cpu::op0x2A() { printf("%s\n", __FUNCTION__); cycles.count += cycles.clock[1]; a = Rol( a ); }
        void Cpu::op0x26() { printf("%s\n", __FUNCTION__); uint data; const uint dst = Zpg_RW( data ); StoreZpg( dst, Rol(data) ); }
        void Cpu::op0x36() { printf("%s\n", __FUNCTION__); uint data; const uint dst = ZpgX_RW( data ); StoreZpg(dst,Rol(data)); }
        void Cpu::op0x2E() { printf("%s\n", __FUNCTION__); uint data; const uint dst = Abs_RW( data ); StoreMem(dst,Rol(data)); }
        void Cpu::op0x3E() { printf("%s\n", __FUNCTION__); uint data; const uint dst = AbsX_RW( data ); StoreMem(dst,Rol(data)); }
        void Cpu::op0x6A() { printf("%s\n", __FUNCTION__); cycles.count += cycles.clock[1]; a = Ror( a ); }
        void Cpu::op0x66() { printf("%s\n", __FUNCTION__); uint data; const uint dst = Zpg_RW( data ); StoreZpg( dst, Ror(data) ); }
        void Cpu::op0x76() { printf("%s\n", __FUNCTION__); uint data; const uint dst = ZpgX_RW( data ); StoreZpg(dst,Ror(data)); }
        void Cpu::op0x6E() { printf("%s\n", __FUNCTION__); uint data; const uint dst = Abs_RW( data ); StoreMem(dst,Ror(data)); }
        void Cpu::op0x7E() { printf("%s\n", __FUNCTION__); uint data; const uint dst = AbsX_RW( data ); StoreMem(dst,Ror(data)); }
        void Cpu::op0x40() { printf("%s\n", __FUNCTION__); Rti(); }
        void Cpu::op0x60() { printf("%s\n", __FUNCTION__); Rts(); }
        void Cpu::op0xE9() { printf("%s\n", __FUNCTION__); Sbc( Imm_R() ); }
        void Cpu::op0xEB() { printf("%s\n", __FUNCTION__); Sbc( Imm_R() ); }
        void Cpu::op0xE5() { printf("%s\n", __FUNCTION__); Sbc( Zpg_R() ); }
        void Cpu::op0xF5() { printf("%s\n", __FUNCTION__); Sbc( ZpgX_R() ); }
        void Cpu::op0xED() { printf("%s\n", __FUNCTION__); Sbc( Abs_R() ); }
        void Cpu::op0xFD() { printf("%s\n", __FUNCTION__); Sbc( AbsX_R() ); }
        void Cpu::op0xF9() { printf("%s\n", __FUNCTION__); Sbc( AbsY_R() ); }
        void Cpu::op0xE1() { printf("%s\n", __FUNCTION__); Sbc( IndX_R() ); }
        void Cpu::op0xF1() { printf("%s\n", __FUNCTION__); Sbc( IndY_R() ); }
        void Cpu::op0x38() { printf("%s\n", __FUNCTION__); Sec(); }
        void Cpu::op0xF8() { printf("%s\n", __FUNCTION__); Sed(); }
        void Cpu::op0x78() { printf("%s\n", __FUNCTION__); Sei(); }
        void Cpu::op0x85() { printf("%s\n", __FUNCTION__); const uint dst = Zpg_W(); StoreZpg( dst, Sta() ); }
        void Cpu::op0x95() { printf("%s\n", __FUNCTION__); const uint dst = ZpgX_W(); StoreZpg(dst,Sta()); }
        void Cpu::op0x8D() {
            printf("%s\n", __FUNCTION__);
            const uint targetAddress = Abs_W();
            if (0x2000 == targetAddress &&
                (a & 0x80) == 0x80)
            {
                printf("NMI enable set\n");
            }
            StoreMem(targetAddress, Sta());
        }
        void Cpu::op0x9D() { printf("%s\n", __FUNCTION__); const uint dst = AbsX_W(); StoreMem(dst,Sta()); }
        void Cpu::op0x99() { printf("%s\n", __FUNCTION__); const uint dst = AbsY_W(); StoreMem(dst,Sta()); }
        void Cpu::op0x81() { printf("%s\n", __FUNCTION__); const uint dst = IndX_W(); StoreMem(dst,Sta()); }
        void Cpu::op0x91() { printf("%s\n", __FUNCTION__); const uint dst = IndY_W(); StoreMem(dst,Sta()); }
        void Cpu::op0x86() { printf("%s\n", __FUNCTION__); const uint dst = Zpg_W(); StoreZpg( dst, Stx() ); }
        void Cpu::op0x96() { printf("%s\n", __FUNCTION__); const uint dst = ZpgY_W(); StoreZpg(dst,Stx()); }
        void Cpu::op0x8E() { printf("%s\n", __FUNCTION__); const uint dst = Abs_W(); StoreMem(dst,Stx()); }
        void Cpu::op0x84()
        {
            printf("%s\n", __FUNCTION__);
            const uint dst = Zpg_W(); // 1
            StoreZpg( dst, Sty() );
        }
        void Cpu::op0x94() { printf("%s\n", __FUNCTION__); const uint dst = ZpgX_W(); StoreZpg(dst,Sty()); }
        void Cpu::op0x8C() { printf("%s\n", __FUNCTION__); const uint dst = Abs_W(); StoreMem(dst,Sty()); }
        void Cpu::op0xAA() { printf("%s\n", __FUNCTION__); Tax(); }
        void Cpu::op0xA8() { printf("%s\n", __FUNCTION__); Tay(); }
        void Cpu::op0xBA() { printf("%s\n", __FUNCTION__); Tsx(); }
        void Cpu::op0x8A() { printf("%s\n", __FUNCTION__); Txa(); }
        void Cpu::op0x9A() { printf("%s\n", __FUNCTION__); Txs(); }
        void Cpu::op0x98() { printf("%s\n", __FUNCTION__); Tya(); }
        
        void Cpu::op0x00() { printf("%s\n", __FUNCTION__); Brk(); }
        void Cpu::op0x02() { printf("%s\n", __FUNCTION__); Jam(); }
        void Cpu::op0x12() { printf("%s\n", __FUNCTION__); Jam(); }
        void Cpu::op0x22() { printf("%s\n", __FUNCTION__); Jam(); }
        void Cpu::op0x32() { printf("%s\n", __FUNCTION__); Jam(); }
        void Cpu::op0x42() { printf("%s\n", __FUNCTION__); Jam(); }
        void Cpu::op0x52() { printf("%s\n", __FUNCTION__); Jam(); }
        void Cpu::op0x62() { printf("%s\n", __FUNCTION__); Jam(); }
        void Cpu::op0x72() { printf("%s\n", __FUNCTION__); Jam(); }
        void Cpu::op0x92() { printf("%s\n", __FUNCTION__); Jam(); }
        void Cpu::op0xB2() { printf("%s\n", __FUNCTION__); Jam(); }
        void Cpu::op0xD2() { printf("%s\n", __FUNCTION__); Jam(); }
        void Cpu::op0xF2() { printf("%s\n", __FUNCTION__); Jam(); }
        
        void Cpu::op0x0B() { printf("%s\n", __FUNCTION__); Anc( Imm_R() ); }
        void Cpu::op0x2B() { printf("%s\n", __FUNCTION__); Anc( Imm_R() ); }
        void Cpu::op0x8B() { printf("%s\n", __FUNCTION__); Ane( Imm_R() ); }
        void Cpu::op0x6B() { printf("%s\n", __FUNCTION__); Arr( Imm_R() ); }
        void Cpu::op0x4B() { printf("%s\n", __FUNCTION__); Asr( Imm_R() ); }
        void Cpu::op0xC7() { printf("%s\n", __FUNCTION__); uint data; const uint dst = Zpg_RW( data ); StoreZpg( dst, Dcp(data) ); }
        void Cpu::op0xD7() { printf("%s\n", __FUNCTION__); uint data; const uint dst = ZpgX_RW( data ); StoreZpg(dst,Dcp(data)); }
        void Cpu::op0xC3() { printf("%s\n", __FUNCTION__); uint data; const uint dst = IndX_RW( data ); StoreMem(dst,Dcp(data)); }
        void Cpu::op0xD3() { printf("%s\n", __FUNCTION__); uint data; const uint dst = IndY_RW( data ); StoreMem(dst,Dcp(data)); }
        void Cpu::op0xCF() { printf("%s\n", __FUNCTION__); uint data; const uint dst = Abs_RW( data ); StoreMem(dst,Dcp(data)); }
        void Cpu::op0xDF() { printf("%s\n", __FUNCTION__); uint data; const uint dst = AbsX_RW( data ); StoreMem(dst,Dcp(data)); }
        void Cpu::op0xDB() { printf("%s\n", __FUNCTION__); uint data; const uint dst = AbsY_RW( data ); StoreMem(dst,Dcp(data)); }
        void Cpu::op0x80() { printf("%s\n", __FUNCTION__); pc += 1; cycles.count += cycles.clock[2 - 1]; Dop(); }
        void Cpu::op0x82() { printf("%s\n", __FUNCTION__); pc += 1; cycles.count += cycles.clock[2 - 1]; Dop(); }
        void Cpu::op0x89() { printf("%s\n", __FUNCTION__); pc += 1; cycles.count += cycles.clock[2 - 1]; Dop(); }
        void Cpu::op0xC2() { printf("%s\n", __FUNCTION__); pc += 1; cycles.count += cycles.clock[2 - 1]; Dop(); }
        void Cpu::op0xE2() { printf("%s\n", __FUNCTION__); pc += 1; cycles.count += cycles.clock[2 - 1]; Dop(); }
        void Cpu::op0x04() { printf("%s\n", __FUNCTION__); pc += 1; cycles.count += cycles.clock[3 - 1]; Dop(); }
        void Cpu::op0x44() { printf("%s\n", __FUNCTION__); pc += 1; cycles.count += cycles.clock[3 - 1]; Dop(); }
        void Cpu::op0x64() { printf("%s\n", __FUNCTION__); pc += 1; cycles.count += cycles.clock[3 - 1]; Dop(); }
        void Cpu::op0x14() { printf("%s\n", __FUNCTION__); pc += 1; cycles.count += cycles.clock[4 - 1]; Dop(); }
        void Cpu::op0x34() { printf("%s\n", __FUNCTION__); pc += 1; cycles.count += cycles.clock[4 - 1]; Dop(); }
        void Cpu::op0x54() { printf("%s\n", __FUNCTION__); pc += 1; cycles.count += cycles.clock[4 - 1]; Dop(); }
        void Cpu::op0x74() { printf("%s\n", __FUNCTION__); pc += 1; cycles.count += cycles.clock[4 - 1]; Dop(); }
        void Cpu::op0xD4() { printf("%s\n", __FUNCTION__); pc += 1; cycles.count += cycles.clock[4 - 1]; Dop(); }
        void Cpu::op0xF4() { printf("%s\n", __FUNCTION__); pc += 1; cycles.count += cycles.clock[4 - 1]; Dop(); }
        void Cpu::op0xE7() { printf("%s\n", __FUNCTION__); uint data; const uint dst = Zpg_RW( data ); StoreZpg( dst, Isb(data) ); }
        void Cpu::op0xF7() { printf("%s\n", __FUNCTION__); uint data; const uint dst = ZpgX_RW( data ); StoreZpg(dst,Isb(data)); }
        void Cpu::op0xEF() { printf("%s\n", __FUNCTION__); uint data; const uint dst = Abs_RW( data ); StoreMem(dst,Isb(data)); }
        void Cpu::op0xFF() { printf("%s\n", __FUNCTION__); uint data; const uint dst = AbsX_RW( data ); StoreMem(dst,Isb(data)); }
        void Cpu::op0xFB()
        {
            printf("%s\n", __FUNCTION__);
            uint data;
            const uint dst = AbsY_RW( data );
            StoreMem(dst,Isb(data));
        }
        void Cpu::op0xE3() { printf("%s\n", __FUNCTION__); uint data; const uint dst = IndX_RW( data ); StoreMem(dst,Isb(data)); }
        void Cpu::op0xF3() { printf("%s\n", __FUNCTION__); uint data; const uint dst = IndY_RW( data ); StoreMem(dst,Isb(data)); }
        void Cpu::op0xBB() { printf("%s\n", __FUNCTION__); Las( AbsY_R() ); }
        void Cpu::op0xA7() { printf("%s\n", __FUNCTION__); Lax( Zpg_R() ); }
        void Cpu::op0xB7() { printf("%s\n", __FUNCTION__); Lax( ZpgY_R() ); }
        void Cpu::op0xAF() { printf("%s\n", __FUNCTION__); Lax( Abs_R() ); }
        void Cpu::op0xBF() { printf("%s\n", __FUNCTION__); Lax( AbsY_R() ); }
        void Cpu::op0xA3() { printf("%s\n", __FUNCTION__); Lax( IndX_R() ); }
        void Cpu::op0xB3() { printf("%s\n", __FUNCTION__); Lax( IndY_R() ); }
        void Cpu::op0xAB() { printf("%s\n", __FUNCTION__); Lxa( Imm_R() ); }
        void Cpu::op0x27() { printf("%s\n", __FUNCTION__); uint data; const uint dst = Zpg_RW( data ); StoreZpg( dst, Rla(data) ); }
        void Cpu::op0x37() { printf("%s\n", __FUNCTION__); uint data; const uint dst = ZpgX_RW( data ); StoreZpg(dst,Rla(data)); }
        void Cpu::op0x2F() { printf("%s\n", __FUNCTION__); uint data; const uint dst = Abs_RW( data ); StoreMem(dst,Rla(data)); }
        void Cpu::op0x3F() { printf("%s\n", __FUNCTION__); uint data; const uint dst = AbsX_RW( data ); StoreMem(dst,Rla(data)); }
        void Cpu::op0x3B() { printf("%s\n", __FUNCTION__); uint data; const uint dst = AbsY_RW( data ); StoreMem(dst,Rla(data)); }
        void Cpu::op0x23() { printf("%s\n", __FUNCTION__); uint data; const uint dst = IndX_RW( data ); StoreMem(dst,Rla(data)); }
        void Cpu::op0x33() { printf("%s\n", __FUNCTION__); uint data; const uint dst = IndY_RW( data ); StoreMem(dst,Rla(data)); }
        void Cpu::op0x67() { printf("%s\n", __FUNCTION__); uint data; const uint dst = Zpg_RW( data ); StoreZpg( dst, Rra(data) ); }
        void Cpu::op0x77() { printf("%s\n", __FUNCTION__); uint data; const uint dst = ZpgX_RW( data ); StoreZpg(dst,Rra(data)); }
        void Cpu::op0x6F() { printf("%s\n", __FUNCTION__); uint data; const uint dst = Abs_RW( data ); StoreMem(dst,Rra(data)); }
        void Cpu::op0x7F() { printf("%s\n", __FUNCTION__); uint data; const uint dst = AbsX_RW( data ); StoreMem(dst,Rra(data)); }
        void Cpu::op0x7B() { printf("%s\n", __FUNCTION__); uint data; const uint dst = AbsY_RW( data ); StoreMem(dst,Rra(data)); }
        void Cpu::op0x63() { printf("%s\n", __FUNCTION__); uint data; const uint dst = IndX_RW( data ); StoreMem(dst,Rra(data)); }
        void Cpu::op0x73() { printf("%s\n", __FUNCTION__); uint data; const uint dst = IndY_RW( data ); StoreMem(dst,Rra(data)); }
        void Cpu::op0x87() { printf("%s\n", __FUNCTION__); const uint dst = Zpg_W(); StoreZpg( dst, Sax() ); }
        void Cpu::op0x97() { printf("%s\n", __FUNCTION__); const uint dst = ZpgY_W(); StoreZpg(dst,Sax()); }
        void Cpu::op0x8F() { printf("%s\n", __FUNCTION__); const uint dst = Abs_W(); StoreMem(dst,Sax()); }
        void Cpu::op0x83() { printf("%s\n", __FUNCTION__); const uint dst = IndX_W(); StoreMem(dst,Sax()); }
        void Cpu::op0xCB() { printf("%s\n", __FUNCTION__); Sbx( Imm_R() ); }
        void Cpu::op0x9F() { printf("%s\n", __FUNCTION__); const uint dst = AbsY_W(); StoreMem(dst,Sha(dst)); }
        void Cpu::op0x93() { printf("%s\n", __FUNCTION__); const uint dst = IndY_W(); StoreMem(dst,Sha(dst)); }
        void Cpu::op0x9B() { printf("%s\n", __FUNCTION__); const uint dst = AbsY_W(); StoreMem(dst,Shs(dst)); }
        void Cpu::op0x9E() { printf("%s\n", __FUNCTION__); const uint dst = AbsY_W(); StoreMem(dst,Shx(dst)); }
        void Cpu::op0x9C() { printf("%s\n", __FUNCTION__); const uint dst = AbsX_W(); StoreMem(dst,Shy(dst)); }
        void Cpu::op0x07() { printf("%s\n", __FUNCTION__); uint data; const uint dst = Zpg_RW( data ); StoreZpg( dst, Slo(data) ); }
        void Cpu::op0x17() { printf("%s\n", __FUNCTION__); uint data; const uint dst = ZpgX_RW( data ); StoreZpg(dst,Slo(data)); }
        void Cpu::op0x0F() { printf("%s\n", __FUNCTION__); uint data; const uint dst = Abs_RW( data ); StoreMem(dst,Slo(data)); }
        void Cpu::op0x1F() { printf("%s\n", __FUNCTION__); uint data; const uint dst = AbsX_RW( data ); StoreMem(dst,Slo(data)); }
        void Cpu::op0x1B() { printf("%s\n", __FUNCTION__); uint data; const uint dst = AbsY_RW( data ); StoreMem(dst,Slo(data)); }
        void Cpu::op0x03() { printf("%s\n", __FUNCTION__); uint data; const uint dst = IndX_RW( data ); StoreMem(dst,Slo(data)); }
        void Cpu::op0x13() { printf("%s\n", __FUNCTION__); uint data; const uint dst = IndY_RW( data ); StoreMem(dst,Slo(data)); }
        void Cpu::op0x47() { printf("%s\n", __FUNCTION__); uint data; const uint dst = Zpg_RW( data ); StoreZpg( dst, Sre(data) ); }
        void Cpu::op0x57() { printf("%s\n", __FUNCTION__); uint data; const uint dst = ZpgX_RW( data ); StoreZpg(dst,Sre(data)); }
        void Cpu::op0x4F() { printf("%s\n", __FUNCTION__); uint data; const uint dst = Abs_RW( data ); StoreMem(dst,Sre(data)); }
        void Cpu::op0x5F() { printf("%s\n", __FUNCTION__); uint data; const uint dst = AbsX_RW( data ); StoreMem(dst,Sre(data)); }
        void Cpu::op0x5B() { printf("%s\n", __FUNCTION__); uint data; const uint dst = AbsY_RW( data ); StoreMem(dst,Sre(data)); }
        void Cpu::op0x43() { printf("%s\n", __FUNCTION__); uint data; const uint dst = IndX_RW( data ); StoreMem(dst,Sre(data)); }
        void Cpu::op0x53() { printf("%s\n", __FUNCTION__); uint data; const uint dst = IndY_RW( data ); StoreMem(dst,Sre(data)); }
        void Cpu::op0x0C() { printf("%s\n", __FUNCTION__); pc += 2; cycles.count += cycles.clock[4 - 1]; Top(); }
        void Cpu::op0x1C() { printf("%s\n", __FUNCTION__); Top( AbsX_R() ); }
        void Cpu::op0x3C() { printf("%s\n", __FUNCTION__); Top( AbsX_R() ); }
        void Cpu::op0x5C() { printf("%s\n", __FUNCTION__); Top( AbsX_R() ); }
        void Cpu::op0x7C() { printf("%s\n", __FUNCTION__); Top( AbsX_R() ); }
        void Cpu::op0xDC() { printf("%s\n", __FUNCTION__); Top( AbsX_R() ); }
        void Cpu::op0xFC() { printf("%s\n", __FUNCTION__); Top( AbsX_R() ); }
    }
}
