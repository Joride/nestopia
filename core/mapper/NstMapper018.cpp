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

#include "../NstMapper.hpp"
#include "../NstClock.hpp"
#include "../NstSoundPlayer.hpp"
#include "NstMapper018.hpp"

namespace Nes
{
    namespace Core
    {
        
        
        
        
        Sound::Player* Mapper18::DetectSound(uint attribute,Apu& apu)
        {
            switch (attribute)
            {
                case ATR_SAMPLES_TNDO:
                    
                    return Sound::Player::Create
                    (
                     apu,
                     Sound::Loader::TERAO_NO_DOSUKOI_OOZUMOU,
                     Sound::Loader::TERAO_NO_DOSUKOI_OOZUMOU_SAMPLES
                     );
                    
                case ATR_SAMPLES_MP90KH:
                case ATR_SAMPLES_MPSH:
                case ATR_SAMPLES_SMPY:
                    
                    return Sound::Player::Create
                    (
                     apu,
                     Sound::Loader::MOERO_PRO_YAKYUU_88,
                     Sound::Loader::MOERO_PRO_YAKYUU_88_SAMPLES
                     );
            }
            
            return __null;
        }
        
        void Mapper18::Irq::Reset(const bool hard)
        {
            if (hard)
            {
                mask = 0xFFFF;
                count = 0;
                latch = 0;
            }
        }
        
        Mapper18::Mapper18(Context& c)
        :
        Mapper (c,CROM_MAX_256K|NMT_HORIZONTAL),
        irq (c.cpu),
        sound (DetectSound(c.attribute,c.apu))
        {}
        
        Mapper18::~Mapper18()
        {
            Sound::Player::Destroy( sound );
        }
        
        void Mapper18::SubReset(const bool hard)
        {
            if (hard)
                wrk.Source().SetSecurity( false, false );
            
            reg = 0;
            irq.Reset( hard, hard ? false : irq.Connected() );
            
            Map( WRK_SAFE_PEEK_POKE );
            
            for (uint i=0x0000; i < 0x1000; i += 0x4)
            {
                Map( 0x8000 + i, &Mapper18::Poke_8000 );
                Map( 0x8001 + i, &Mapper18::Poke_8001 );
                Map( 0x8002 + i, &Mapper18::Poke_8002 );
                Map( 0x8003 + i, &Mapper18::Poke_8003 );
                Map( 0x9000 + i, &Mapper18::Poke_9000 );
                Map( 0x9001 + i, &Mapper18::Poke_9001 );
                Map( 0x9002 + i, &Mapper18::Poke_9002 );
                Map( 0x9003 + i, &Mapper18::Poke_9003 );
                Map( 0xA000 + i, &Mapper18::Poke_A000 );
                Map( 0xA001 + i, &Mapper18::Poke_A001 );
                Map( 0xA002 + i, &Mapper18::Poke_A002 );
                Map( 0xA003 + i, &Mapper18::Poke_A003 );
                Map( 0xB000 + i, &Mapper18::Poke_B000 );
                Map( 0xB001 + i, &Mapper18::Poke_B001 );
                Map( 0xB002 + i, &Mapper18::Poke_B002 );
                Map( 0xB003 + i, &Mapper18::Poke_B003 );
                Map( 0xC000 + i, &Mapper18::Poke_C000 );
                Map( 0xC001 + i, &Mapper18::Poke_C001 );
                Map( 0xC002 + i, &Mapper18::Poke_C002 );
                Map( 0xC003 + i, &Mapper18::Poke_C003 );
                Map( 0xD000 + i, &Mapper18::Poke_D000 );
                Map( 0xD001 + i, &Mapper18::Poke_D001 );
                Map( 0xD002 + i, &Mapper18::Poke_D002 );
                Map( 0xD003 + i, &Mapper18::Poke_D003 );
                Map( 0xE000 + i, &Mapper18::Poke_E000 );
                Map( 0xE001 + i, &Mapper18::Poke_E001 );
                Map( 0xE002 + i, &Mapper18::Poke_E002 );
                Map( 0xE003 + i, &Mapper18::Poke_E003 );
                Map( 0xF000 + i, &Mapper18::Poke_F000 );
                Map( 0xF001 + i, &Mapper18::Poke_F001 );
                Map( 0xF002 + i, NMT_SWAP_HV01 );
                
                if (sound)
                    Map( 0xF003 + i, &Mapper18::Poke_F003 );
            }
        }
        
        void Mapper18::SubLoad(State::Loader& state)
        {
            if (sound)
                sound->Stop();
            
            while (const dword chunk = state.Begin())
            {
                if (chunk == AsciiId<'I','R','Q'>::V)
                {
                    State::Loader::Data<5> data( state );
                    
                    irq.Connect( data[0] & 0x1 );
                    
                    if (data[0] & 0x8) irq.unit.mask = 0x000F;
                    else if (data[0] & 0x4) irq.unit.mask = 0x00FF;
                    else if (data[0] & 0x2) irq.unit.mask = 0x0FFF;
                    else irq.unit.mask = 0xFFFF;
                    
                    irq.unit.latch = data[1] | data[2] << 8;
                    irq.unit.count = data[3] | data[4] << 8;
                }
                else if (chunk == AsciiId<'R','E','G'>::V)
                {
                    ((void)0);
                    reg = state.Read8();
                }
                
                state.End();
            }
        }
        
        void Mapper18::SubSave(State::Saver& state) const
        {
            const byte data[5] =
            {
                (irq.Connected() ? 0x1U : 0x0U) |
                (
                 irq.unit.mask == 0x000F ? 0x8U :
                 irq.unit.mask == 0x00FF ? 0x4U :
                 irq.unit.mask == 0x0FFF ? 0x2U :
                 0x0U
                 ),
                irq.unit.latch & 0xFF,
                irq.unit.latch >> 8,
                irq.unit.count & 0xFF,
                irq.unit.count >> 8
            };
            
            state.Begin( AsciiId<'I','R','Q'>::V ).Write( data ).End();
            
            if (sound)
                state.Begin( AsciiId<'R','E','G'>::V ).Write8( reg ).End();
        }
        
        
        
        
        
        template<uint MASK,uint SHIFT>
        void Mapper18::SwapPrg(const uint address,const uint data)
        {
            prg.SwapBank<SIZE_8K>( address, (prg.GetBank<SIZE_8K>(address) & MASK) | (data & 0xF) << SHIFT );
        }
        
        void Mapper18::Poke_8000(void* p_,Address i_,Data j_) { static_cast<Mapper18*>(p_)->Poke_M_8000(i_,j_); } inline void Mapper18::Poke_M_8000(Address,Data data) { SwapPrg<0xF0,0>( 0x0000, data ); }
        void Mapper18::Poke_8001(void* p_,Address i_,Data j_) { static_cast<Mapper18*>(p_)->Poke_M_8001(i_,j_); } inline void Mapper18::Poke_M_8001(Address,Data data) { SwapPrg<0x0F,4>( 0x0000, data ); }
        void Mapper18::Poke_8002(void* p_,Address i_,Data j_) { static_cast<Mapper18*>(p_)->Poke_M_8002(i_,j_); } inline void Mapper18::Poke_M_8002(Address,Data data) { SwapPrg<0xF0,0>( 0x2000, data ); }
        void Mapper18::Poke_8003(void* p_,Address i_,Data j_) { static_cast<Mapper18*>(p_)->Poke_M_8003(i_,j_); } inline void Mapper18::Poke_M_8003(Address,Data data) { SwapPrg<0x0F,4>( 0x2000, data ); }
        void Mapper18::Poke_9000(void* p_,Address i_,Data j_) { static_cast<Mapper18*>(p_)->Poke_M_9000(i_,j_); } inline void Mapper18::Poke_M_9000(Address,Data data) { SwapPrg<0xF0,0>( 0x4000, data ); }
        void Mapper18::Poke_9001(void* p_,Address i_,Data j_) { static_cast<Mapper18*>(p_)->Poke_M_9001(i_,j_); } inline void Mapper18::Poke_M_9001(Address,Data data) { SwapPrg<0x0F,4>( 0x4000, data ); }
        
        void Mapper18::Poke_9002(void* p_,Address i_,Data j_) { static_cast<Mapper18*>(p_)->Poke_M_9002(i_,j_); } inline void Mapper18::Poke_M_9002(Address,Data data)
        {
            ((void)0);
            wrk.Source().SetSecurity( data & 0x1, data & 0x2 );
        }
        
        void Mapper18::Poke_9003(void* p_,Address i_,Data j_) { static_cast<Mapper18*>(p_)->Poke_M_9003(i_,j_); } inline void Mapper18::Poke_M_9003(Address,Data)
        {
            ((void)0);
        }
        
        template<uint MASK,uint SHIFT>
        void Mapper18::SwapChr(const uint address,const uint data) const
        {
            ppu.Update();
            chr.SwapBank<SIZE_1K>( address, (chr.GetBank<SIZE_1K>(address) & MASK) | (data & 0xF) << SHIFT );
        }
        
        void Mapper18::Poke_A000(void* p_,Address i_,Data j_) { static_cast<Mapper18*>(p_)->Poke_M_A000(i_,j_); } inline void Mapper18::Poke_M_A000(Address,Data data) { SwapChr<0xF0,0>( 0x0000, data ); }
        void Mapper18::Poke_A001(void* p_,Address i_,Data j_) { static_cast<Mapper18*>(p_)->Poke_M_A001(i_,j_); } inline void Mapper18::Poke_M_A001(Address,Data data) { SwapChr<0x0F,4>( 0x0000, data ); }
        void Mapper18::Poke_A002(void* p_,Address i_,Data j_) { static_cast<Mapper18*>(p_)->Poke_M_A002(i_,j_); } inline void Mapper18::Poke_M_A002(Address,Data data) { SwapChr<0xF0,0>( 0x0400, data ); }
        void Mapper18::Poke_A003(void* p_,Address i_,Data j_) { static_cast<Mapper18*>(p_)->Poke_M_A003(i_,j_); } inline void Mapper18::Poke_M_A003(Address,Data data) { SwapChr<0x0F,4>( 0x0400, data ); }
        void Mapper18::Poke_B000(void* p_,Address i_,Data j_) { static_cast<Mapper18*>(p_)->Poke_M_B000(i_,j_); } inline void Mapper18::Poke_M_B000(Address,Data data) { SwapChr<0xF0,0>( 0x0800, data ); }
        void Mapper18::Poke_B001(void* p_,Address i_,Data j_) { static_cast<Mapper18*>(p_)->Poke_M_B001(i_,j_); } inline void Mapper18::Poke_M_B001(Address,Data data) { SwapChr<0x0F,4>( 0x0800, data ); }
        void Mapper18::Poke_B002(void* p_,Address i_,Data j_) { static_cast<Mapper18*>(p_)->Poke_M_B002(i_,j_); } inline void Mapper18::Poke_M_B002(Address,Data data) { SwapChr<0xF0,0>( 0x0C00, data ); }
        void Mapper18::Poke_B003(void* p_,Address i_,Data j_) { static_cast<Mapper18*>(p_)->Poke_M_B003(i_,j_); } inline void Mapper18::Poke_M_B003(Address,Data data) { SwapChr<0x0F,4>( 0x0C00, data ); }
        void Mapper18::Poke_C000(void* p_,Address i_,Data j_) { static_cast<Mapper18*>(p_)->Poke_M_C000(i_,j_); } inline void Mapper18::Poke_M_C000(Address,Data data) { SwapChr<0xF0,0>( 0x1000, data ); }
        void Mapper18::Poke_C001(void* p_,Address i_,Data j_) { static_cast<Mapper18*>(p_)->Poke_M_C001(i_,j_); } inline void Mapper18::Poke_M_C001(Address,Data data) { SwapChr<0x0F,4>( 0x1000, data ); }
        void Mapper18::Poke_C002(void* p_,Address i_,Data j_) { static_cast<Mapper18*>(p_)->Poke_M_C002(i_,j_); } inline void Mapper18::Poke_M_C002(Address,Data data) { SwapChr<0xF0,0>( 0x1400, data ); }
        void Mapper18::Poke_C003(void* p_,Address i_,Data j_) { static_cast<Mapper18*>(p_)->Poke_M_C003(i_,j_); } inline void Mapper18::Poke_M_C003(Address,Data data) { SwapChr<0x0F,4>( 0x1400, data ); }
        void Mapper18::Poke_D000(void* p_,Address i_,Data j_) { static_cast<Mapper18*>(p_)->Poke_M_D000(i_,j_); } inline void Mapper18::Poke_M_D000(Address,Data data) { SwapChr<0xF0,0>( 0x1800, data ); }
        void Mapper18::Poke_D001(void* p_,Address i_,Data j_) { static_cast<Mapper18*>(p_)->Poke_M_D001(i_,j_); } inline void Mapper18::Poke_M_D001(Address,Data data) { SwapChr<0x0F,4>( 0x1800, data ); }
        void Mapper18::Poke_D002(void* p_,Address i_,Data j_) { static_cast<Mapper18*>(p_)->Poke_M_D002(i_,j_); } inline void Mapper18::Poke_M_D002(Address,Data data) { SwapChr<0xF0,0>( 0x1C00, data ); }
        void Mapper18::Poke_D003(void* p_,Address i_,Data j_) { static_cast<Mapper18*>(p_)->Poke_M_D003(i_,j_); } inline void Mapper18::Poke_M_D003(Address,Data data) { SwapChr<0x0F,4>( 0x1C00, data ); }
        
        void Mapper18::Poke_E000(void* p_,Address i_,Data j_) { static_cast<Mapper18*>(p_)->Poke_M_E000(i_,j_); } inline void Mapper18::Poke_M_E000(Address,Data data)
        {
            irq.Update();
            irq.unit.latch = (irq.unit.latch & 0xFFF0) | (data & 0xF) << 0;
        }
        
        void Mapper18::Poke_E001(void* p_,Address i_,Data j_) { static_cast<Mapper18*>(p_)->Poke_M_E001(i_,j_); } inline void Mapper18::Poke_M_E001(Address,Data data)
        {
            irq.Update();
            irq.unit.latch = (irq.unit.latch & 0xFF0F) | (data & 0xF) << 4;
        }
        
        void Mapper18::Poke_E002(void* p_,Address i_,Data j_) { static_cast<Mapper18*>(p_)->Poke_M_E002(i_,j_); } inline void Mapper18::Poke_M_E002(Address,Data data)
        {
            irq.Update();
            irq.unit.latch = (irq.unit.latch & 0xF0FF) | (data & 0xF) << 8;
        }
        
        void Mapper18::Poke_E003(void* p_,Address i_,Data j_) { static_cast<Mapper18*>(p_)->Poke_M_E003(i_,j_); } inline void Mapper18::Poke_M_E003(Address,Data data)
        {
            irq.Update();
            irq.unit.latch = (irq.unit.latch & 0x0FFF) | (data & 0xF) << 12;
        }
        
        void Mapper18::Poke_F000(void* p_,Address i_,Data j_) { static_cast<Mapper18*>(p_)->Poke_M_F000(i_,j_); } inline void Mapper18::Poke_M_F000(Address,Data)
        {
            irq.Update();
            irq.unit.count = irq.unit.latch;
            irq.ClearIRQ();
        }
        
        void Mapper18::Poke_F001(void* p_,Address i_,Data j_) { static_cast<Mapper18*>(p_)->Poke_M_F001(i_,j_); } inline void Mapper18::Poke_M_F001(Address,Data data)
        {
            irq.Update();
            
            if (data & 0x8) irq.unit.mask = 0x000F;
            else if (data & 0x4) irq.unit.mask = 0x00FF;
            else if (data & 0x2) irq.unit.mask = 0x0FFF;
            else irq.unit.mask = 0xFFFF;
            
            irq.Connect( data & 0x1 );
            irq.ClearIRQ();
        }
        
        void Mapper18::Poke_F003(void* p_,Address i_,Data j_) { static_cast<Mapper18*>(p_)->Poke_M_F003(i_,j_); } inline void Mapper18::Poke_M_F003(Address,Data data)
        {
            (__builtin_expect(!(!!(sound)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/mapper/NstMapper018.cpp", 289, "!!(sound)") : (void)0);
            
            uint tmp = reg;
            reg = data;
            
            if ((data & 0x2) < (tmp & 0x2) && (data & 0x1D) == (tmp & 0x1D))
                sound->Play( data >> 2 & 0x1F );
        }
        
        bool Mapper18::Irq::Clock()
        {
            return (count & mask) && !(--count & mask);
        }
        
        void Mapper18::Sync(Event event,Input::Controllers*)
        {
            if (event == EVENT_END_FRAME)
                irq.VSync();
        }
    }
}
