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
#include "NstFds.hpp"
#include "NstMapper.hpp"
#include "board/NstBrdVrc6.hpp"
#include "board/NstBrdVrc7.hpp"
#include "board/NstBrdN106.hpp"
#include "board/NstBrdFme7.hpp"
#include "board/NstBrdMmc5.hpp"
#include "api/NstApiNsf.hpp"
#include "NstNsf.hpp"


namespace Nes
{
    namespace Core
    {
        
        
        
        
        class Nsf::Chips : Apu::Channel
        {
            struct Mmc5 : Boards::Mmc5::Sound
            {
                uint mul[2];
                byte exRam[SIZE_1K];
                
                explicit Mmc5(Apu& a)
                : Sound(a,false) {}
                
                void Reset();
                void ClearExRam();
                
                using Boards::Mmc5::Sound::UpdateSettings;
                using Boards::Mmc5::Sound::GetSample;
                using Boards::Mmc5::Sound::Clock;
            };
            
            struct Fds : Core::Fds::Sound
            {
                byte ram[SIZE_8K+SIZE_32K];
                
                explicit Fds(Apu& a)
                : Sound(a,false) {}
                
                void Reset();
                void SwapBank(const Prg&,uint,uint);
                
                using Core::Fds::Sound::UpdateSettings;
                using Core::Fds::Sound::GetSample;
                using Core::Fds::Sound::Clock;
            };
            
            struct N106 : Boards::N106::Sound
            {
                explicit N106(Apu& a)
                : Sound(a,false) {}
                
                using Boards::N106::Sound::Reset;
                using Boards::N106::Sound::UpdateSettings;
                using Boards::N106::Sound::GetSample;
            };
            
            struct Vrc6 : Boards::Vrc6::Sound
            {
                explicit Vrc6(Apu& a)
                : Sound(a,false) {}
                
                using Boards::Vrc6::Sound::Reset;
                using Boards::Vrc6::Sound::UpdateSettings;
                using Boards::Vrc6::Sound::GetSample;
            };
            
            struct Vrc7 : Boards::Vrc7::Sound
            {
                explicit Vrc7(Apu& a)
                : Sound(a,false) {}
                
                using Boards::Vrc7::Sound::Reset;
                using Boards::Vrc7::Sound::UpdateSettings;
                using Boards::Vrc7::Sound::GetSample;
            };
            
            struct S5B : Boards::Fme7::Sound
            {
                explicit S5B(Apu& a)
                : Sound(a,false) {}
                
                using Boards::Fme7::Sound::Reset;
                using Boards::Fme7::Sound::UpdateSettings;
                using Boards::Fme7::Sound::GetSample;
            };
            
            template<typename T>
            struct Chip : Pointer<T>
            {
                Chip(Apu& a,uint t)
                : Pointer<T>(t ? new T(a) : __null) {}
            };
            
            struct Clocks
            {
                void Reset(bool,bool);
                
                Cycle next;
                Cycle mmc5;
                Cycle fds;
            };
            
            void Reset();
            bool UpdateSettings();
            Sample GetSample();
            Cycle Clock(Cycle,Cycle,Cycle);
            
            Clocks clocks;
            
        public:
            
            Chips(uint,Apu&);
            
            Chip<Mmc5> mmc5;
            Chip<Vrc6> vrc6;
            Chip<Vrc7> vrc7;
            Chip<Fds> fds;
            Chip<S5B> s5b;
            Chip<N106> n106;
        };
        
        void Nsf::Chips::Mmc5::ClearExRam()
        {
            std::memset( exRam, 0, sizeof(exRam) );
        }
        
        void Nsf::Chips::Mmc5::Reset()
        {
            mul[0] = 0;
            mul[1] = 0;
            
            ClearExRam();
            
            Sound::Reset();
        }
        
        void Nsf::Chips::Fds::Reset()
        {
            std::memset( ram, 0, sizeof(ram) );
            
            Sound::Reset();
        }
        
        void Nsf::Chips::Fds::SwapBank(const Prg& prg,uint page,uint bank)
        {
            std::memcpy( ram + SIZE_4K * page, prg.Source().Mem(bank * SIZE_4K), SIZE_4K );
        }
        
        Nsf::Chips::Chips(const uint types,Apu& apu)
        :
        Channel ( apu ),
        mmc5 ( apu, types & Api::Nsf::CHIP_MMC5 ),
        vrc6 ( apu, types & Api::Nsf::CHIP_VRC6 ),
        vrc7 ( apu, types & Api::Nsf::CHIP_VRC7 ),
        fds ( apu, types & Api::Nsf::CHIP_FDS ),
        s5b ( apu, types & Api::Nsf::CHIP_S5B ),
        n106 ( apu, types & Api::Nsf::CHIP_N106 )
        {
            Connect( UpdateSettings() );
        }
        
        void Nsf::Chips::Clocks::Reset(bool mmc5Chip,bool fdsChip)
        {
            next = (mmc5Chip || fdsChip ? 0UL : Cpu::CYCLE_MAX);
            mmc5 = (mmc5Chip ? 0UL : Cpu::CYCLE_MAX);
            fds = (fdsChip ? 0UL : Cpu::CYCLE_MAX);
        }
        
        void Nsf::Chips::Reset()
        {
            clocks.Reset( mmc5, fds );
            
            if ( mmc5 ) mmc5->Reset();
            if ( vrc6 ) vrc6->Reset();
            if ( vrc7 ) vrc7->Reset();
            if ( fds ) fds->Reset();
            if ( s5b ) s5b->Reset();
            if ( n106 ) n106->Reset();
        }
        
        bool Nsf::Chips::UpdateSettings()
        {
            clocks.Reset( mmc5, fds );
            
            return
            (
             ( mmc5 ? mmc5->UpdateSettings() : 0U ) |
             ( vrc6 ? vrc6->UpdateSettings() : 0U ) |
             ( vrc7 ? vrc7->UpdateSettings() : 0U ) |
             ( fds ? fds->UpdateSettings() : 0U ) |
             ( s5b ? s5b->UpdateSettings() : 0U ) |
             ( n106 ? n106->UpdateSettings() : 0U )
             );
        }
        
        Nsf::Nsf(Context& context)
        :
        Image (SOUND),
        cpu (context.cpu),
        apu (context.apu),
        chips (__null),
        tuneMode (Api::Nsf::TUNE_MODE_NTSC)
        {
            Stream::In stream( context.stream );
            
            uint version;
            
            {
                byte data[5+1+2+6];
                stream.Read( data );
                
                if
                    (
                     data[0] != Ascii<'N'>::V ||
                     data[1] != Ascii<'E'>::V ||
                     data[2] != Ascii<'S'>::V ||
                     data[3] != Ascii<'M'>::V ||
                     data[4] != 0x1A
                     )
                    throw RESULT_ERR_INVALID_FILE;
                
                if (!data[6] || data[9] < 0x60 || data[11] < 0x60 || data[13] < 0x60)
                    throw RESULT_ERR_CORRUPT_FILE;
                
                songs.count = data[6];
                songs.start = data[7] >= 1 && data[7] <= data[6] ? data[7] - 1 : 0;
                
                addressing.load = data[8] | uint( data[9] ) << 8;
                addressing.init = data[10] | uint( data[11] ) << 8;
                addressing.play = data[12] | uint( data[13] ) << 8;
                
                version = data[5];
            }
            
            stream.Read( songs.info.name, 32 );
            stream.Read( songs.info.artist, 32 );
            stream.Read( songs.info.maker, 32 );
            
            songs.info.name[31] = '\0';
            songs.info.artist[31] = '\0';
            songs.info.maker[31] = '\0';
            
            speed.ntsc = stream.Read16();
            stream.Read( banks );
            
            addressing.bankSwitched = 0 !=
            (
             uint( banks[0] ) |
             uint( banks[1] ) |
             uint( banks[2] ) |
             uint( banks[3] ) |
             uint( banks[4] ) |
             uint( banks[5] ) |
             uint( banks[6] ) |
             uint( banks[7] )
             );
            
            speed.pal = stream.Read16();
            songs.current = songs.start;
            
            switch (stream.Read8() & 0x3)
            {
                case 0x0: tuneMode = Api::Nsf::TUNE_MODE_NTSC; break;
                case 0x1: tuneMode = Api::Nsf::TUNE_MODE_PAL; break;
                default: tuneMode = Api::Nsf::TUNE_MODE_BOTH; break;
            }
            
            uint types = stream.Read8();
            
            if (!(types & Api::Nsf::CHIP_FDS) && addressing.load < 0x8000)
                throw RESULT_ERR_CORRUPT_FILE;
            
            dword length = 0;
            
            while (length < SIZE_4096K && stream.SafeRead8() <= 0xFF)
                ++length;
            
            if (length <= HEADER_RESERVED_LENGTH)
                throw RESULT_ERR_CORRUPT_FILE;
            
            length -= HEADER_RESERVED_LENGTH;
            stream.Seek( -idword(length) );
            
            {
                const uint offset = addressing.load & 0xFFFU;
                
                prg.Source().Set( offset + length, true, false ).Fill( JAM );
                stream.Read( prg.Source().Mem() + offset, length );
            }
            
            if (types & Api::Nsf::CHIP_ALL)
                chips = new Chips (types,apu);
            
            if (Log::Available())
            {
                Log log;
                
                log << "Nsf: version " << version;
                
                if (*songs.info.name)
                    log << "\n" "Nsf: name: " << songs.info.name;
                
                if (*songs.info.artist)
                    log << "\n" "Nsf: artist: " << songs.info.artist;
                
                if (*songs.info.maker)
                    log << "\n" "Nsf: maker: " << songs.info.maker;
                
                log << "\n" "Nsf: starting song "
                << (songs.start+1U)
                << " of "
                << songs.count
                <<
                (
                 tuneMode == Api::Nsf::TUNE_MODE_NTSC ? "\n" "Nsf: NTSC mode" :
                 tuneMode == Api::Nsf::TUNE_MODE_PAL ? "\n" "Nsf: PAL mode" :
                 "\n" "Nsf: PAL/NTSC mode"
                 )
                << "\n" "Nsf: "
                << (length / SIZE_1K)
                << (addressing.bankSwitched ? "k bank-switched " : "k flat ")
                << ((types & Api::Nsf::CHIP_FDS) ? "PRG-RAM" : "PRG-ROM")
                << "\n" "Nsf: load address - " << Log::Hex( 16, addressing.load )
                << "\n" "Nsf: init address - " << Log::Hex( 16, addressing.init )
                << "\n" "Nsf: play address - " << Log::Hex( 16, addressing.play )
                << "\n";
                
                if (types & Api::Nsf::CHIP_ALL)
                {
                    if ( chips->mmc5 ) log << "Nsf: MMC5 sound chip present" "\n";
                    if ( chips->vrc6 ) log << "Nsf: VRC6 sound chip present" "\n";
                    if ( chips->vrc7 ) log << "Nsf: VRC7 sound chip present" "\n";
                    if ( chips->fds ) log << "Nsf: FDS sound chip present" "\n";
                    if ( chips->s5b ) log << "Nsf: Sunsoft5B sound chip present" "\n";
                    if ( chips->n106 ) log << "Nsf: N106 sound chip present" "\n";
                }
            }
        }
        
        Nsf::~Nsf()
        {
            delete chips;
        }
        
        void Nsf::SetRegion(const Region::Type region)
        {
            routine.nmi = Routine::NMI;
            cpu.SetFrameCycles( region == Region::NTSC ? Clocks::RP2C02_HVSYNC : Clocks::RP2C07_HVSYNC );
        }
        
        Region::Type Nsf::GetRegion() const
        {
            return tuneMode == Api::Nsf::TUNE_MODE_PAL ? Region::PAL : Region::NTSC;
        }
        
        uint Nsf::GetChips() const
        {
            uint types = 0;
            
            if (chips)
            {
                if ( chips->vrc6 ) types |= Api::Nsf::CHIP_VRC6;
                if ( chips->vrc7 ) types |= Api::Nsf::CHIP_VRC7;
                if ( chips->fds ) types |= Api::Nsf::CHIP_FDS;
                if ( chips->mmc5 ) types |= Api::Nsf::CHIP_MMC5;
                if ( chips->n106 ) types |= Api::Nsf::CHIP_N106;
                if ( chips->s5b ) types |= Api::Nsf::CHIP_S5B;
            }
            
            return types;
        }
        
        void Nsf::Reset(bool)
        {
            cpu.Map( 0x38EC ).Set( this, &Nsf::Peek_38EC, &Nsf::Poke_Nop );
            cpu.Map( 0x38ED ).Set( this, &Nsf::Peek_38ED, &Nsf::Poke_Nop );
            cpu.Map( 0x38EE ).Set( this, &Nsf::Peek_38EE, &Nsf::Poke_Nop );
            cpu.Map( 0x38EF ).Set( this, &Nsf::Peek_38EF, &Nsf::Poke_Nop );
            cpu.Map( 0x38F0 ).Set( this, &Nsf::Peek_38F0, &Nsf::Poke_Nop );
            cpu.Map( 0x38F1 ).Set( this, &Nsf::Peek_38F1, &Nsf::Poke_Nop );
            cpu.Map( 0x38F2 ).Set( this, &Nsf::Peek_38F2, &Nsf::Poke_Nop );
            cpu.Map( 0x38F3 ).Set( this, &Nsf::Peek_38F3, &Nsf::Poke_Nop );
            cpu.Map( 0x38F4 ).Set( this, &Nsf::Peek_38F4, &Nsf::Poke_Nop );
            cpu.Map( 0x38F5 ).Set( this, &Nsf::Peek_38F5, &Nsf::Poke_Nop );
            cpu.Map( 0x38F6 ).Set( this, &Nsf::Peek_38F6, &Nsf::Poke_Nop );
            cpu.Map( 0x38F7 ).Set( this, &Nsf::Peek_38F7, &Nsf::Poke_Nop );
            cpu.Map( 0x38F8 ).Set( this, &Nsf::Peek_38F8, &Nsf::Poke_Nop );
            cpu.Map( 0x38F9 ).Set( this, &Nsf::Peek_38F9, &Nsf::Poke_Nop );
            cpu.Map( 0x38FA ).Set( this, &Nsf::Peek_38FA, &Nsf::Poke_Nop );
            cpu.Map( 0x38FB ).Set( this, &Nsf::Peek_38FB, &Nsf::Poke_Nop );
            cpu.Map( 0x38FC ).Set( this, &Nsf::Peek_38FC, &Nsf::Poke_Nop );
            cpu.Map( 0x38FD ).Set( this, &Nsf::Peek_38FD, &Nsf::Poke_Nop );
            cpu.Map( 0x38FE ).Set( this, &Nsf::Peek_38FE, &Nsf::Poke_Nop );
            cpu.Map( 0x38FF ).Set( this, &Nsf::Peek_38FF, &Nsf::Poke_Nop );
            
            cpu.Map( 0x4017 ).Set( this, &Nsf::Peek_Nop, &Nsf::Poke_4017 );
            
            const bool fds = chips && chips->fds;
            
            if (addressing.bankSwitched)
            {
                if (fds)
                {
                    cpu.Map( 0x5FF6 ).Set( this, &Nsf::Peek_Nop, &Nsf::Poke_Fds_5FF6 );
                    cpu.Map( 0x5FF7 ).Set( this, &Nsf::Peek_Nop, &Nsf::Poke_Fds_5FF7 );
                }
                
                cpu.Map( 0x5FF8 ).Set( this, &Nsf::Peek_Nop, fds ? &Nsf::Poke_Fds_5FF8 : &Nsf::Poke_5FF8 );
                cpu.Map( 0x5FF9 ).Set( this, &Nsf::Peek_Nop, fds ? &Nsf::Poke_Fds_5FF9 : &Nsf::Poke_5FF9 );
                cpu.Map( 0x5FFA ).Set( this, &Nsf::Peek_Nop, fds ? &Nsf::Poke_Fds_5FFA : &Nsf::Poke_5FFA );
                cpu.Map( 0x5FFB ).Set( this, &Nsf::Peek_Nop, fds ? &Nsf::Poke_Fds_5FFB : &Nsf::Poke_5FFB );
                cpu.Map( 0x5FFC ).Set( this, &Nsf::Peek_Nop, fds ? &Nsf::Poke_Fds_5FFC : &Nsf::Poke_5FFC );
                cpu.Map( 0x5FFD ).Set( this, &Nsf::Peek_Nop, fds ? &Nsf::Poke_Fds_5FFD : &Nsf::Poke_5FFD );
                cpu.Map( 0x5FFE ).Set( this, &Nsf::Peek_Nop, fds ? &Nsf::Poke_Fds_5FFE : &Nsf::Poke_5FFE );
                cpu.Map( 0x5FFF ).Set( this, &Nsf::Peek_Nop, fds ? &Nsf::Poke_Fds_5FFF : &Nsf::Poke_5FFF );
            }
            else if (!fds)
            {
                for (dword i=0x8000, j=0; i < 0x10000; j += (i >= (addressing.load & 0xF000U)), i += 0x1000)
                    prg.SwapBank<SIZE_4K>( i-0x8000, j );
            }
            
            if (fds)
            {
                cpu.Map( 0x4040, 0x407F ).Set( this, &Nsf::Peek_Fds_4040, &Nsf::Poke_Fds_4040 );
                cpu.Map( 0x4080 ).Set( this, &Nsf::Peek_Nop, &Nsf::Poke_Fds_4080 );
                cpu.Map( 0x4082 ).Set( this, &Nsf::Peek_Nop, &Nsf::Poke_Fds_4082 );
                cpu.Map( 0x4083 ).Set( this, &Nsf::Peek_Nop, &Nsf::Poke_Fds_4083 );
                cpu.Map( 0x4084 ).Set( this, &Nsf::Peek_Nop, &Nsf::Poke_Fds_4084 );
                cpu.Map( 0x4085 ).Set( this, &Nsf::Peek_Nop, &Nsf::Poke_Fds_4085 );
                cpu.Map( 0x4086 ).Set( this, &Nsf::Peek_Nop, &Nsf::Poke_Fds_4086 );
                cpu.Map( 0x4087 ).Set( this, &Nsf::Peek_Nop, &Nsf::Poke_Fds_4087 );
                cpu.Map( 0x4088 ).Set( this, &Nsf::Peek_Nop, &Nsf::Poke_Fds_4088 );
                cpu.Map( 0x4089 ).Set( this, &Nsf::Peek_Nop, &Nsf::Poke_Fds_4089 );
                cpu.Map( 0x408A ).Set( this, &Nsf::Peek_Nop, &Nsf::Poke_Fds_408A );
                cpu.Map( 0x4090 ).Set( this, &Nsf::Peek_Fds_4090, &Nsf::Poke_Nop );
                cpu.Map( 0x4092 ).Set( this, &Nsf::Peek_Fds_4092, &Nsf::Poke_Nop );
                cpu.Map( 0x6000, 0xFFFF ).Set( this, &Nsf::Peek_Fds_Ram, &Nsf::Poke_Fds_Ram );
            }
            else
            {
                cpu.Map( 0x6000, 0x7FFF ).Set( this, &Nsf::Peek_Wrk, &Nsf::Poke_Wrk );
                cpu.Map( 0x8000, 0x8FFF ).Set( this, &Nsf::Peek_Prg_8, &Nsf::Poke_Nop );
                cpu.Map( 0x9000, 0x9FFF ).Set( this, &Nsf::Peek_Prg_9, &Nsf::Poke_Nop );
                cpu.Map( 0xA000, 0xAFFF ).Set( this, &Nsf::Peek_Prg_A, &Nsf::Poke_Nop );
                cpu.Map( 0xB000, 0xBFFF ).Set( this, &Nsf::Peek_Prg_B, &Nsf::Poke_Nop );
                cpu.Map( 0xC000, 0xCFFF ).Set( this, &Nsf::Peek_Prg_C, &Nsf::Poke_Nop );
                cpu.Map( 0xD000, 0xDFFF ).Set( this, &Nsf::Peek_Prg_D, &Nsf::Poke_Nop );
                cpu.Map( 0xE000, 0xEFFF ).Set( this, &Nsf::Peek_Prg_E, &Nsf::Poke_Nop );
                cpu.Map( 0xF000, 0xFFFF ).Set( this, &Nsf::Peek_Prg_F, &Nsf::Poke_Nop );
            }
            
            if (chips)
            {
                if (chips->mmc5)
                {
                    cpu.Map( 0x5000 ).Set( this, &Nsf::Peek_Nop, &Nsf::Poke_Mmc5_5000 );
                    cpu.Map( 0x5002 ).Set( this, &Nsf::Peek_Nop, &Nsf::Poke_Mmc5_5002 );
                    cpu.Map( 0x5003 ).Set( this, &Nsf::Peek_Nop, &Nsf::Poke_Mmc5_5003 );
                    cpu.Map( 0x5004 ).Set( this, &Nsf::Peek_Nop, &Nsf::Poke_Mmc5_5004 );
                    cpu.Map( 0x5006 ).Set( this, &Nsf::Peek_Nop, &Nsf::Poke_Mmc5_5006 );
                    cpu.Map( 0x5007 ).Set( this, &Nsf::Peek_Nop, &Nsf::Poke_Mmc5_5007 );
                    cpu.Map( 0x5010 ).Set( this, &Nsf::Peek_Nop, &Nsf::Poke_Mmc5_5010 );
                    cpu.Map( 0x5011 ).Set( this, &Nsf::Peek_Nop, &Nsf::Poke_Mmc5_5011 );
                    cpu.Map( 0x5015 ).Set( this, &Nsf::Peek_Mmc5_5015, &Nsf::Poke_Mmc5_5015 );
                    cpu.Map( 0x5205 ).Set( this, &Nsf::Peek_Mmc5_5205, &Nsf::Poke_Mmc5_5205 );
                    cpu.Map( 0x5206 ).Set( this, &Nsf::Peek_Mmc5_5206, &Nsf::Poke_Mmc5_5206 );
                    cpu.Map( 0x5C00, 0x5FF5 ).Set( this, &Nsf::Peek_Mmc5_5C00, &Nsf::Poke_Mmc5_5C00 );
                }
                
                if (chips->vrc6)
                {
                    cpu.Map( 0x9000 ).Set( &Nsf::Poke_Vrc6_9000 );
                    cpu.Map( 0x9001 ).Set( &Nsf::Poke_Vrc6_9001 );
                    cpu.Map( 0x9002 ).Set( &Nsf::Poke_Vrc6_9002 );
                    cpu.Map( 0xA000 ).Set( &Nsf::Poke_Vrc6_A000 );
                    cpu.Map( 0xA001 ).Set( &Nsf::Poke_Vrc6_A001 );
                    cpu.Map( 0xA002 ).Set( &Nsf::Poke_Vrc6_A002 );
                    cpu.Map( 0xB000 ).Set( &Nsf::Poke_Vrc6_B000 );
                    cpu.Map( 0xB001 ).Set( &Nsf::Poke_Vrc6_B001 );
                    cpu.Map( 0xB002 ).Set( &Nsf::Poke_Vrc6_B002 );
                }
                
                if (chips->vrc7)
                {
                    cpu.Map( 0x9010 ).Set( &Nsf::Poke_Vrc7_9010 );
                    cpu.Map( 0x9030 ).Set( &Nsf::Poke_Vrc7_9030 );
                }
                
                if (chips->n106)
                {
                    cpu.Map( 0x4800 ).Set( this, &Nsf::Peek_N106_48, &Nsf::Poke_N106_48 );
                    cpu.Map( 0xF800 ).Set( &Nsf::Poke_N106_F8 );
                }
                
                if (chips->s5b)
                {
                    cpu.Map( 0xC000 ).Set( &Nsf::Poke_S5B_C );
                    cpu.Map( 0xE000 ).Set( &Nsf::Poke_S5B_E );
                }
            }
            
            cpu.Map( 0xFFFA ).Set( &Nsf::Peek_FFFA );
            cpu.Map( 0xFFFB ).Set( &Nsf::Peek_FFFB );
            cpu.Map( 0xFFFC ).Set( &Nsf::Peek_FFFC );
            cpu.Map( 0xFFFD ).Set( &Nsf::Peek_FFFD );
            
            routine.reset = Routine::RESET;
            
            SetRegion( cpu.GetRegion() );
        }
        
        bool Nsf::PowerOff()
        {
            StopSong();
            return true;
        }
        
        Result Nsf::SelectSong(const uint song)
        {
            if (song < songs.count)
            {
                if (songs.current != song)
                {
                    songs.current = song;
                    
                    if (routine.playing)
                    {
                        routine.nmi = Routine::NMI;
                        apu.ClearBuffers();
                    }
                    
                    Api::Nsf::eventCallback( Api::Nsf::EVENT_SELECT_SONG );
                    
                    return RESULT_OK;
                }
                
                return RESULT_NOP;
            }
            
            return RESULT_ERR_INVALID_PARAM;
        }
        
        Result Nsf::PlaySong()
        {
            if (!routine.playing)
            {
                routine.nmi = Routine::NMI;
                routine.playing = true;
                
                Api::Nsf::eventCallback( Api::Nsf::EVENT_PLAY_SONG );
                
                return RESULT_OK;
            }
            
            return RESULT_NOP;
        }
        
        Result Nsf::StopSong()
        {
            if (routine.playing)
            {
                routine.playing = false;
                routine.nmi = Routine::NMI;
                apu.ClearBuffers();
                
                Api::Nsf::eventCallback( Api::Nsf::EVENT_STOP_SONG );
                
                return RESULT_OK;
            }
            
            return RESULT_NOP;
        }
        
        void Nsf::InitSong()
        {
            std::memset( wrk, 0x00, SIZE_8K );
            
            if (chips && chips->mmc5)
                chips->mmc5->ClearExRam();
            
            const bool fds = chips && chips->fds;
            
            if (addressing.bankSwitched)
            {
                if (fds)
                {
                    for (uint i=0; i < 2; ++i)
                        cpu.Poke( 0x5FF6+i, banks[6+i] );
                }
                
                for (uint i=0; i < 8; ++i)
                    cpu.Poke( 0x5FF8+i, banks[i] );
            }
            else if (fds)
            {
                for (dword i=0x6000, j=0; i < 0x10000; j += (i >= (addressing.load & 0xF000U)), i += 0x1000)
                    std::memcpy( chips->fds->ram + (i-0x6000), prg.Source().Mem(j * 0x1000), 0x1000 );
            }
            
            if (fds)
            {
                cpu.Poke( 0x4089, 0x80 );
                cpu.Poke( 0x408A, 0xE8 );
            }
            
            apu.ClearBuffers();
            std::memset( cpu.GetRam(), 0x00, Cpu::RAM_SIZE );
            
            for (uint i=0x4000; i <= 0x4013; ++i)
                cpu.Poke( i, 0x00 );
            
            cpu.Poke( 0x4015, 0x0F );
            cpu.Poke( 0x4017, 0xC0 );
        }
        
        
        
        
        
        void Nsf::BeginFrame()
        {
            routine.jmp = (routine.playing ? 0xFA : 0xFD);
            
            if (routine.nmi)
                cpu.DoNMI(0);
        }
        
        Cycle Nsf::Chips::Clock(Cycle rateCycles,Cycle rateClock,const Cycle targetCycles)
        {
            if (clocks.next != Cpu::CYCLE_MAX)
            {
                (__builtin_expect(!(!!((mmc5 || fds) && (clocks.mmc5 != Cpu::CYCLE_MAX || clocks.fds != Cpu::CYCLE_MAX))), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstNsf.cpp", 664, "!!((mmc5 || fds) && (clocks.mmc5 != Cpu::CYCLE_MAX || clocks.fds != Cpu::CYCLE_MAX))") : (void)0);
                
                if (clocks.mmc5 == clocks.next)
                    clocks.mmc5 = mmc5->Clock( rateCycles, rateClock, targetCycles ) - rateCycles;
                
                if (clocks.fds == clocks.next)
                    clocks.fds = fds->Clock( rateCycles, rateClock, targetCycles ) - rateCycles;
                
                clocks.next = ((clocks.mmc5) < (clocks.fds) ? (clocks.mmc5) : (clocks.fds));
                
                rateCycles += clocks.next;
                
                return rateCycles;
            }
            else
            {
                (__builtin_expect(!(!!(!mmc5 && !fds)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstNsf.cpp", 680, "!!(!mmc5 && !fds)") : (void)0);
                
                return Channel::Clock( rateCycles, rateClock, targetCycles );
            }
        }
        
        Nsf::Chips::Sample Nsf::Chips::GetSample()
        {
            return
            (
             (mmc5 ? mmc5->GetSample() : 0) +
             (vrc6 ? vrc6->GetSample() : 0) +
             (vrc7 ? vrc7->GetSample() : 0) +
             (fds ? fds->GetSample() : 0) +
             (s5b ? s5b->GetSample() : 0) +
             (n106 ? n106->GetSample() : 0)
             );
        }
        
        inline uint Nsf::FetchLast(uint offset) const
        {
            (__builtin_expect(!(!!(offset <= 0xFFF)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstNsf.cpp", 701, "!!(offset <= 0xFFF)") : (void)0);
            return offset[chips && chips->fds ? chips->fds->ram + (sizeof(array(chips->fds->ram))-SIZE_4K) : prg[7]];
        }
        
        Data Nsf::Peek_FFFA(void* p_,Address i_) { return static_cast<Nsf*>(p_)->Peek_M_FFFA(i_); } inline Data Nsf::Peek_M_FFFA(Address)
        {
            return routine.nmi ? routine.nmi &= Routine::NMI_B, routine.playing ? 0xEC : 0xFD : FetchLast(0xFFA);
        }
        
        Data Nsf::Peek_FFFB(void* p_,Address i_) { return static_cast<Nsf*>(p_)->Peek_M_FFFB(i_); } inline Data Nsf::Peek_M_FFFB(Address)
        {
            return routine.nmi ? routine.nmi &= Routine::NMI_A, 0x38 : FetchLast(0xFFB);
        }
        
        Data Nsf::Peek_FFFC(void* p_,Address i_) { return static_cast<Nsf*>(p_)->Peek_M_FFFC(i_); } inline Data Nsf::Peek_M_FFFC(Address)
        {
            return routine.reset ? routine.reset &= Routine::RESET_B, 0xFD : FetchLast(0xFFC);
        }
        
        Data Nsf::Peek_FFFD(void* p_,Address i_) { return static_cast<Nsf*>(p_)->Peek_M_FFFD(i_); } inline Data Nsf::Peek_M_FFFD(Address)
        {
            return routine.reset ? routine.reset &= Routine::RESET_A, 0x38 : FetchLast(0xFFD);
        }
        
        Data Nsf::Peek_38EC(void* p_,Address i_) { return static_cast<Nsf*>(p_)->Peek_M_38EC(i_); } inline Data Nsf::Peek_M_38EC(Address)
        {
            ((void)0);
            
            InitSong();
            return LDA;
        }
        
        Data Nsf::Peek_38ED(void* p_,Address i_) { return static_cast<Nsf*>(p_)->Peek_M_38ED(i_); } inline Data Nsf::Peek_M_38ED(Address)
        {
            ((void)0);
            return songs.current;
        }
        
        Data Nsf::Peek_38EE(void* p_,Address i_) { return static_cast<Nsf*>(p_)->Peek_M_38EE(i_); } inline Data Nsf::Peek_M_38EE(Address)
        {
            ((void)0);
            return LDX;
        }
        
        Data Nsf::Peek_38EF(void* p_,Address i_) { return static_cast<Nsf*>(p_)->Peek_M_38EF(i_); } inline Data Nsf::Peek_M_38EF(Address)
        {
            ((void)0);
            return 0xFC;
        }
        
        Data Nsf::Peek_38F0(void* p_,Address i_) { return static_cast<Nsf*>(p_)->Peek_M_38F0(i_); } inline Data Nsf::Peek_M_38F0(Address)
        {
            ((void)0);
            return TXS;
        }
        
        Data Nsf::Peek_38F1(void* p_,Address i_) { return static_cast<Nsf*>(p_)->Peek_M_38F1(i_); } inline Data Nsf::Peek_M_38F1(Address)
        {
            ((void)0);
            return LDX;
        }
        
        Data Nsf::Peek_38F2(void* p_,Address i_) { return static_cast<Nsf*>(p_)->Peek_M_38F2(i_); } inline Data Nsf::Peek_M_38F2(Address)
        {
            ((void)0);
            return cpu.GetRegion() == Region::PAL;
        }
        
        Data Nsf::Peek_38F3(void* p_,Address i_) { return static_cast<Nsf*>(p_)->Peek_M_38F3(i_); } inline Data Nsf::Peek_M_38F3(Address)
        {
            ((void)0);
            return JSR;
        }
        
        Data Nsf::Peek_38F4(void* p_,Address i_) { return static_cast<Nsf*>(p_)->Peek_M_38F4(i_); } inline Data Nsf::Peek_M_38F4(Address)
        {
            ((void)0);
            return addressing.init & 0xFFU;
        }
        
        Data Nsf::Peek_38F5(void* p_,Address i_) { return static_cast<Nsf*>(p_)->Peek_M_38F5(i_); } inline Data Nsf::Peek_M_38F5(Address)
        {
            ((void)0);
            return addressing.init >> 8;
        }
        
        Data Nsf::Peek_38F6(void* p_,Address i_) { return static_cast<Nsf*>(p_)->Peek_M_38F6(i_); } inline Data Nsf::Peek_M_38F6(Address)
        {
            ((void)0);
            return SEI;
        }
        
        Data Nsf::Peek_38F7(void* p_,Address i_) { return static_cast<Nsf*>(p_)->Peek_M_38F7(i_); } inline Data Nsf::Peek_M_38F7(Address)
        {
            ((void)0);
            routine.jmp = 0xFD;
            return JMP;
        }
        
        Data Nsf::Peek_38F8(void* p_,Address i_) { return static_cast<Nsf*>(p_)->Peek_M_38F8(i_); } inline Data Nsf::Peek_M_38F8(Address)
        {
            ((void)0);
            return 0xFD;
        }
        
        Data Nsf::Peek_38F9(void* p_,Address i_) { return static_cast<Nsf*>(p_)->Peek_M_38F9(i_); } inline Data Nsf::Peek_M_38F9(Address)
        {
            ((void)0);
            return 0x38;
        }
        
        Data Nsf::Peek_38FA(void* p_,Address i_) { return static_cast<Nsf*>(p_)->Peek_M_38FA(i_); } inline Data Nsf::Peek_M_38FA(Address)
        {
            ((void)0);
            routine.jmp = 0xFD;
            return JSR;
        }
        
        Data Nsf::Peek_38FB(void* p_,Address i_) { return static_cast<Nsf*>(p_)->Peek_M_38FB(i_); } inline Data Nsf::Peek_M_38FB(Address)
        {
            ((void)0);
            return addressing.play & 0xFFU;
        }
        
        Data Nsf::Peek_38FC(void* p_,Address i_) { return static_cast<Nsf*>(p_)->Peek_M_38FC(i_); } inline Data Nsf::Peek_M_38FC(Address)
        {
            ((void)0);
            return addressing.play >> 8;
        }
        
        Data Nsf::Peek_38FD(void* p_,Address i_) { return static_cast<Nsf*>(p_)->Peek_M_38FD(i_); } inline Data Nsf::Peek_M_38FD(Address)
        {
            return JMP;
        }
        
        Data Nsf::Peek_38FE(void* p_,Address i_) { return static_cast<Nsf*>(p_)->Peek_M_38FE(i_); } inline Data Nsf::Peek_M_38FE(Address)
        {
            return routine.jmp;
        }
        
        Data Nsf::Peek_38FF(void* p_,Address i_) { return static_cast<Nsf*>(p_)->Peek_M_38FF(i_); } inline Data Nsf::Peek_M_38FF(Address)
        {
            return 0x38;
        }
        
        void Nsf::Poke_4017(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_4017(i_,j_); } inline void Nsf::Poke_M_4017(Address,Data data)
        {
            apu.WriteFrameCtrl( data );
        }
        
        Data Nsf::Peek_Nop(void* p_,Address i_) { return static_cast<Nsf*>(p_)->Peek_M_Nop(i_); } inline Data Nsf::Peek_M_Nop(Address address) { return address >> 8; }
        void Nsf::Poke_Nop(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Nop(i_,j_); } inline void Nsf::Poke_M_Nop(Address,Data) {}
        
        Data Nsf::Peek_Prg_8(void* p_,Address i_) { return static_cast<Nsf*>(p_)->Peek_M_Prg_8(i_); } inline Data Nsf::Peek_M_Prg_8(Address address) { return prg[0][address - 0x8000]; }
        Data Nsf::Peek_Prg_9(void* p_,Address i_) { return static_cast<Nsf*>(p_)->Peek_M_Prg_9(i_); } inline Data Nsf::Peek_M_Prg_9(Address address) { return prg[1][address - 0x9000]; }
        Data Nsf::Peek_Prg_A(void* p_,Address i_) { return static_cast<Nsf*>(p_)->Peek_M_Prg_A(i_); } inline Data Nsf::Peek_M_Prg_A(Address address) { return prg[2][address - 0xA000]; }
        Data Nsf::Peek_Prg_B(void* p_,Address i_) { return static_cast<Nsf*>(p_)->Peek_M_Prg_B(i_); } inline Data Nsf::Peek_M_Prg_B(Address address) { return prg[3][address - 0xB000]; }
        Data Nsf::Peek_Prg_C(void* p_,Address i_) { return static_cast<Nsf*>(p_)->Peek_M_Prg_C(i_); } inline Data Nsf::Peek_M_Prg_C(Address address) { return prg[4][address - 0xC000]; }
        Data Nsf::Peek_Prg_D(void* p_,Address i_) { return static_cast<Nsf*>(p_)->Peek_M_Prg_D(i_); } inline Data Nsf::Peek_M_Prg_D(Address address) { return prg[5][address - 0xD000]; }
        Data Nsf::Peek_Prg_E(void* p_,Address i_)
        {
            return static_cast<Nsf*>(p_)->Peek_M_Prg_E(i_);
        }
        inline Data Nsf::Peek_M_Prg_E(Address address)
        {
            return prg[6][address - 0xE000];
        }
        Data Nsf::Peek_Prg_F(void* p_,Address i_) { return static_cast<Nsf*>(p_)->Peek_M_Prg_F(i_); } inline Data Nsf::Peek_M_Prg_F(Address address) { return prg[7][address - 0xF000]; }
        
        Data Nsf::Peek_Wrk(void* p_,Address i_) { return static_cast<Nsf*>(p_)->Peek_M_Wrk(i_); } inline Data Nsf::Peek_M_Wrk(Address address) { return wrk[address - 0x6000]; }
        void Nsf::Poke_Wrk(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Wrk(i_,j_); } inline void Nsf::Poke_M_Wrk(Address address,Data data) { wrk[address - 0x6000] = data; }
        
        void Nsf::Poke_5FF8(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_5FF8(i_,j_); } inline void Nsf::Poke_M_5FF8(Address,Data data) { prg.SwapBank<SIZE_4K,0x0000>( data ); }
        void Nsf::Poke_5FF9(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_5FF9(i_,j_); } inline void Nsf::Poke_M_5FF9(Address,Data data) { prg.SwapBank<SIZE_4K,0x1000>( data ); }
        void Nsf::Poke_5FFA(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_5FFA(i_,j_); } inline void Nsf::Poke_M_5FFA(Address,Data data) { prg.SwapBank<SIZE_4K,0x2000>( data ); }
        void Nsf::Poke_5FFB(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_5FFB(i_,j_); } inline void Nsf::Poke_M_5FFB(Address,Data data) { prg.SwapBank<SIZE_4K,0x3000>( data ); }
        void Nsf::Poke_5FFC(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_5FFC(i_,j_); } inline void Nsf::Poke_M_5FFC(Address,Data data) { prg.SwapBank<SIZE_4K,0x4000>( data ); }
        void Nsf::Poke_5FFD(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_5FFD(i_,j_); } inline void Nsf::Poke_M_5FFD(Address,Data data) { prg.SwapBank<SIZE_4K,0x5000>( data ); }
        void Nsf::Poke_5FFE(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_5FFE(i_,j_); } inline void Nsf::Poke_M_5FFE(Address,Data data) { prg.SwapBank<SIZE_4K,0x6000>( data ); }
        void Nsf::Poke_5FFF(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_5FFF(i_,j_); } inline void Nsf::Poke_M_5FFF(Address,Data data) { prg.SwapBank<SIZE_4K,0x7000>( data ); }
        
        Data Nsf::Peek_Fds_4040(void* p_,Address i_) { return static_cast<Nsf*>(p_)->Peek_M_Fds_4040(i_); } inline Data Nsf::Peek_M_Fds_4040(Address address) { return chips->fds->ReadWaveData( address ); }
        void Nsf::Poke_Fds_4040(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Fds_4040(i_,j_); } inline void Nsf::Poke_M_Fds_4040(Address address,Data data) { chips->fds->WriteWaveData( address, data ); }
        void Nsf::Poke_Fds_4080(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Fds_4080(i_,j_); } inline void Nsf::Poke_M_Fds_4080(Address,Data data) { chips->fds->WriteReg0( data ); }
        void Nsf::Poke_Fds_4082(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Fds_4082(i_,j_); } inline void Nsf::Poke_M_Fds_4082(Address,Data data) { chips->fds->WriteReg1( data ); }
        void Nsf::Poke_Fds_4083(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Fds_4083(i_,j_); } inline void Nsf::Poke_M_Fds_4083(Address,Data data) { chips->fds->WriteReg2( data ); }
        void Nsf::Poke_Fds_4084(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Fds_4084(i_,j_); } inline void Nsf::Poke_M_Fds_4084(Address,Data data) { chips->fds->WriteReg3( data ); }
        void Nsf::Poke_Fds_4085(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Fds_4085(i_,j_); } inline void Nsf::Poke_M_Fds_4085(Address,Data data) { chips->fds->WriteReg4( data ); }
        void Nsf::Poke_Fds_4086(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Fds_4086(i_,j_); } inline void Nsf::Poke_M_Fds_4086(Address,Data data) { chips->fds->WriteReg5( data ); }
        void Nsf::Poke_Fds_4087(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Fds_4087(i_,j_); } inline void Nsf::Poke_M_Fds_4087(Address,Data data) { chips->fds->WriteReg6( data ); }
        void Nsf::Poke_Fds_4088(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Fds_4088(i_,j_); } inline void Nsf::Poke_M_Fds_4088(Address,Data data) { chips->fds->WriteReg7( data ); }
        void Nsf::Poke_Fds_4089(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Fds_4089(i_,j_); } inline void Nsf::Poke_M_Fds_4089(Address,Data data) { chips->fds->WriteReg8( data ); }
        void Nsf::Poke_Fds_408A(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Fds_408A(i_,j_); } inline void Nsf::Poke_M_Fds_408A(Address,Data data) { chips->fds->WriteReg9( data ); }
        Data Nsf::Peek_Fds_4090(void* p_,Address i_) { return static_cast<Nsf*>(p_)->Peek_M_Fds_4090(i_); } inline Data Nsf::Peek_M_Fds_4090(Address) { return chips->fds->ReadVolumeGain(); }
        Data Nsf::Peek_Fds_4092(void* p_,Address i_) { return static_cast<Nsf*>(p_)->Peek_M_Fds_4092(i_); } inline Data Nsf::Peek_M_Fds_4092(Address) { return chips->fds->ReadSweepGain(); }
        void Nsf::Poke_Fds_5FF6(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Fds_5FF6(i_,j_); } inline void Nsf::Poke_M_Fds_5FF6(Address,Data data) { chips->fds->SwapBank( prg, 0, data ); }
        void Nsf::Poke_Fds_5FF7(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Fds_5FF7(i_,j_); } inline void Nsf::Poke_M_Fds_5FF7(Address,Data data) { chips->fds->SwapBank( prg, 1, data ); }
        void Nsf::Poke_Fds_5FF8(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Fds_5FF8(i_,j_); } inline void Nsf::Poke_M_Fds_5FF8(Address,Data data) { chips->fds->SwapBank( prg, 2, data ); }
        void Nsf::Poke_Fds_5FF9(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Fds_5FF9(i_,j_); } inline void Nsf::Poke_M_Fds_5FF9(Address,Data data) { chips->fds->SwapBank( prg, 3, data ); }
        void Nsf::Poke_Fds_5FFA(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Fds_5FFA(i_,j_); } inline void Nsf::Poke_M_Fds_5FFA(Address,Data data) { chips->fds->SwapBank( prg, 4, data ); }
        void Nsf::Poke_Fds_5FFB(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Fds_5FFB(i_,j_); } inline void Nsf::Poke_M_Fds_5FFB(Address,Data data) { chips->fds->SwapBank( prg, 5, data ); }
        void Nsf::Poke_Fds_5FFC(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Fds_5FFC(i_,j_); } inline void Nsf::Poke_M_Fds_5FFC(Address,Data data) { chips->fds->SwapBank( prg, 6, data ); }
        void Nsf::Poke_Fds_5FFD(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Fds_5FFD(i_,j_); } inline void Nsf::Poke_M_Fds_5FFD(Address,Data data) { chips->fds->SwapBank( prg, 7, data ); }
        void Nsf::Poke_Fds_5FFE(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Fds_5FFE(i_,j_); } inline void Nsf::Poke_M_Fds_5FFE(Address,Data data) { chips->fds->SwapBank( prg, 8, data ); }
        void Nsf::Poke_Fds_5FFF(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Fds_5FFF(i_,j_); } inline void Nsf::Poke_M_Fds_5FFF(Address,Data data) { chips->fds->SwapBank( prg, 9, data ); }
        Data Nsf::Peek_Fds_Ram(void* p_,Address i_) { return static_cast<Nsf*>(p_)->Peek_M_Fds_Ram(i_); } inline Data Nsf::Peek_M_Fds_Ram(Address address) { return chips->fds->ram[address - 0x6000]; }
        void Nsf::Poke_Fds_Ram(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Fds_Ram(i_,j_); } inline void Nsf::Poke_M_Fds_Ram(Address address,Data data) { chips->fds->ram[address - 0x6000] = data; }
        
        void Nsf::Poke_Mmc5_5000(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Mmc5_5000(i_,j_); } inline void Nsf::Poke_M_Mmc5_5000(Address,Data data) { chips->mmc5->WriteSquareReg0( 0, data ); }
        void Nsf::Poke_Mmc5_5002(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Mmc5_5002(i_,j_); } inline void Nsf::Poke_M_Mmc5_5002(Address,Data data) { chips->mmc5->WriteSquareReg1( 0, data ); }
        void Nsf::Poke_Mmc5_5003(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Mmc5_5003(i_,j_); } inline void Nsf::Poke_M_Mmc5_5003(Address,Data data) { chips->mmc5->WriteSquareReg2( 0, data ); }
        void Nsf::Poke_Mmc5_5004(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Mmc5_5004(i_,j_); } inline void Nsf::Poke_M_Mmc5_5004(Address,Data data) { chips->mmc5->WriteSquareReg0( 1, data ); }
        void Nsf::Poke_Mmc5_5006(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Mmc5_5006(i_,j_); } inline void Nsf::Poke_M_Mmc5_5006(Address,Data data) { chips->mmc5->WriteSquareReg1( 1, data ); }
        void Nsf::Poke_Mmc5_5007(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Mmc5_5007(i_,j_); } inline void Nsf::Poke_M_Mmc5_5007(Address,Data data) { chips->mmc5->WriteSquareReg2( 1, data ); }
        void Nsf::Poke_Mmc5_5010(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Mmc5_5010(i_,j_); } inline void Nsf::Poke_M_Mmc5_5010(Address,Data data) { chips->mmc5->WritePcmReg0( data ); }
        void Nsf::Poke_Mmc5_5011(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Mmc5_5011(i_,j_); } inline void Nsf::Poke_M_Mmc5_5011(Address,Data data) { chips->mmc5->WritePcmReg1( data ); }
        void Nsf::Poke_Mmc5_5015(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Mmc5_5015(i_,j_); } inline void Nsf::Poke_M_Mmc5_5015(Address,Data data) { chips->mmc5->WriteCtrl( data ); }
        Data Nsf::Peek_Mmc5_5015(void* p_,Address i_) { return static_cast<Nsf*>(p_)->Peek_M_Mmc5_5015(i_); } inline Data Nsf::Peek_M_Mmc5_5015(Address) { return chips->mmc5->ReadCtrl(); }
        Data Nsf::Peek_Mmc5_5205(void* p_,Address i_) { return static_cast<Nsf*>(p_)->Peek_M_Mmc5_5205(i_); } inline Data Nsf::Peek_M_Mmc5_5205(Address) { return (chips->mmc5->mul[0] * chips->mmc5->mul[1]) >> 0 & 0xFF; }
        Data Nsf::Peek_Mmc5_5206(void* p_,Address i_) { return static_cast<Nsf*>(p_)->Peek_M_Mmc5_5206(i_); } inline Data Nsf::Peek_M_Mmc5_5206(Address) { return (chips->mmc5->mul[0] * chips->mmc5->mul[1]) >> 8 & 0xFF; }
        void Nsf::Poke_Mmc5_5205(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Mmc5_5205(i_,j_); } inline void Nsf::Poke_M_Mmc5_5205(Address,Data data) { chips->mmc5->mul[0] = data; }
        void Nsf::Poke_Mmc5_5206(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Mmc5_5206(i_,j_); } inline void Nsf::Poke_M_Mmc5_5206(Address,Data data) { chips->mmc5->mul[1] = data; }
        Data Nsf::Peek_Mmc5_5C00(void* p_,Address i_) { return static_cast<Nsf*>(p_)->Peek_M_Mmc5_5C00(i_); } inline Data Nsf::Peek_M_Mmc5_5C00(Address address) { return chips->mmc5->exRam[address - 0x5C00]; }
        void Nsf::Poke_Mmc5_5C00(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Mmc5_5C00(i_,j_); } inline void Nsf::Poke_M_Mmc5_5C00(Address address,Data data) { chips->mmc5->exRam[address - 0x5C00] = data; }
        
        void Nsf::Poke_Vrc6_9000(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Vrc6_9000(i_,j_); } inline void Nsf::Poke_M_Vrc6_9000(Address,Data data) { chips->vrc6->WriteSquareReg0( 0, data ); }
        void Nsf::Poke_Vrc6_9001(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Vrc6_9001(i_,j_); } inline void Nsf::Poke_M_Vrc6_9001(Address,Data data) { chips->vrc6->WriteSquareReg1( 0, data ); }
        void Nsf::Poke_Vrc6_9002(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Vrc6_9002(i_,j_); } inline void Nsf::Poke_M_Vrc6_9002(Address,Data data) { chips->vrc6->WriteSquareReg2( 0, data ); }
        void Nsf::Poke_Vrc6_A000(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Vrc6_A000(i_,j_); } inline void Nsf::Poke_M_Vrc6_A000(Address,Data data) { chips->vrc6->WriteSquareReg0( 1, data ); }
        void Nsf::Poke_Vrc6_A001(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Vrc6_A001(i_,j_); } inline void Nsf::Poke_M_Vrc6_A001(Address,Data data) { chips->vrc6->WriteSquareReg1( 1, data ); }
        void Nsf::Poke_Vrc6_A002(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Vrc6_A002(i_,j_); } inline void Nsf::Poke_M_Vrc6_A002(Address,Data data) { chips->vrc6->WriteSquareReg2( 1, data ); }
        void Nsf::Poke_Vrc6_B000(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Vrc6_B000(i_,j_); } inline void Nsf::Poke_M_Vrc6_B000(Address,Data data) { chips->vrc6->WriteSawReg0( data ); }
        void Nsf::Poke_Vrc6_B001(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Vrc6_B001(i_,j_); } inline void Nsf::Poke_M_Vrc6_B001(Address,Data data) { chips->vrc6->WriteSawReg1( data ); }
        void Nsf::Poke_Vrc6_B002(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Vrc6_B002(i_,j_); } inline void Nsf::Poke_M_Vrc6_B002(Address,Data data) { chips->vrc6->WriteSawReg2( data ); }
        
        void Nsf::Poke_Vrc7_9010(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Vrc7_9010(i_,j_); } inline void Nsf::Poke_M_Vrc7_9010(Address,Data data) { chips->vrc7->SelectReg( data ); }
        void Nsf::Poke_Vrc7_9030(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_Vrc7_9030(i_,j_); } inline void Nsf::Poke_M_Vrc7_9030(Address,Data data) { chips->vrc7->WriteReg( data ); }
        
        void Nsf::Poke_S5B_C(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_S5B_C(i_,j_); } inline void Nsf::Poke_M_S5B_C(Address,Data data) { chips->s5b->SelectReg( data ); }
        void Nsf::Poke_S5B_E(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_S5B_E(i_,j_); } inline void Nsf::Poke_M_S5B_E(Address,Data data) { chips->s5b->WriteReg( data ); }
        
        Data Nsf::Peek_N106_48(void* p_,Address i_) { return static_cast<Nsf*>(p_)->Peek_M_N106_48(i_); } inline Data Nsf::Peek_M_N106_48(Address) { return chips->n106->ReadData(); }
        void Nsf::Poke_N106_48(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_N106_48(i_,j_); } inline void Nsf::Poke_M_N106_48(Address,Data data) { chips->n106->WriteData( data ); }
        void Nsf::Poke_N106_F8(void* p_,Address i_,Data j_) { static_cast<Nsf*>(p_)->Poke_M_N106_F8(i_,j_); } inline void Nsf::Poke_M_N106_F8(Address,Data data) { chips->n106->WriteAddress( data ); }
    }
}
