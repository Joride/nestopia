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
#include "../board/NstBrdVrc4.hpp"
#include "NstMapper027.hpp"

namespace Nes
{
    namespace Core
    {
        
        
        
        
        Mapper27::Mapper27(Context& c)
        :
        Mapper (c,WRAM_DEFAULT),
        irq (c.cpu)
        {}
        
        void Mapper27::SubReset(const bool hard)
        {
            if (hard)
                prgSwap = 0;
            
            irq.Reset( hard, hard ? false : irq.Connected() );
            
            for (dword i=0x8000; i <= 0xFFFF; ++i)
            {
                switch (i & 0xF0CF)
                {
                    case 0x8000: Map( i, &Mapper27::Poke_8 ); break;
                    case 0x9000: Map( i, NMT_SWAP_VH01 ); break;
                    case 0x9002:
                    case 0x9080: Map( i, &Mapper27::Poke_9 ); break;
                    case 0xA000: Map( i, PRG_SWAP_8K_1 ); break;
                    case 0xB000: Map( i, &Mapper27::Poke_B0 ); break;
                    case 0xB001: Map( i, &Mapper27::Poke_B1 ); break;
                    case 0xB002: Map( i, &Mapper27::Poke_B2 ); break;
                    case 0xB003: Map( i, &Mapper27::Poke_B3 ); break;
                    case 0xC000: Map( i, &Mapper27::Poke_C0 ); break;
                    case 0xC001: Map( i, &Mapper27::Poke_C1 ); break;
                    case 0xC002: Map( i, &Mapper27::Poke_C2 ); break;
                    case 0xC003: Map( i, &Mapper27::Poke_C3 ); break;
                    case 0xD000: Map( i, &Mapper27::Poke_D0 ); break;
                    case 0xD001: Map( i, &Mapper27::Poke_D1 ); break;
                    case 0xD002: Map( i, &Mapper27::Poke_D2 ); break;
                    case 0xD003: Map( i, &Mapper27::Poke_D3 ); break;
                    case 0xE000: Map( i, &Mapper27::Poke_E0 ); break;
                    case 0xE001: Map( i, &Mapper27::Poke_E1 ); break;
                    case 0xE002: Map( i, &Mapper27::Poke_E2 ); break;
                    case 0xE003: Map( i, &Mapper27::Poke_E3 ); break;
                    case 0xF000: Map( i, &Mapper27::Poke_F0 ); break;
                    case 0xF001: Map( i, &Mapper27::Poke_F1 ); break;
                    case 0xF002: Map( i, &Mapper27::Poke_F2 ); break;
                    case 0xF003: Map( i, &Mapper27::Poke_F3 ); break;
                }
            }
        }
        
        void Mapper27::SubLoad(State::Loader& state)
        {
            while (const dword chunk = state.Begin())
            {
                switch (chunk)
                {
                    case AsciiId<'R','E','G'>::V:
                        
                        prgSwap = state.Read8() & 0x2;
                        break;
                        
                    case AsciiId<'I','R','Q'>::V:
                        
                        irq.LoadState( state );
                        break;
                }
                
                state.End();
            }
        }
        
        void Mapper27::SubSave(State::Saver& state) const
        {
            state.Begin( AsciiId<'R','E','G'>::V ).Write8( prgSwap ).End();
            irq.SaveState( state, AsciiId<'I','R','Q'>::V );
        }
        
        
        
        
        
        void Mapper27::Poke_8(void* p_,Address i_,Data j_) { static_cast<Mapper27*>(p_)->Poke_M_8(i_,j_); } inline void Mapper27::Poke_M_8(Address,Data data)
        {
            prg.SwapBank<SIZE_8K>( prgSwap << 13, data );
        }
        
        void Mapper27::Poke_9(void* p_,Address i_,Data j_) { static_cast<Mapper27*>(p_)->Poke_M_9(i_,j_); } inline void Mapper27::Poke_M_9(Address,Data data)
        {
            data &= 0x2;
            
            if (prgSwap != data)
            {
                prgSwap = data;
                
                prg.SwapBanks<SIZE_8K,0x0000>
                (
                 prg.GetBank<SIZE_8K,0x4000>(),
                 prg.GetBank<SIZE_8K,0x0000>()
                 );
            }
        }
        
        template<uint MASK,uint BITS,uint SHIFT>
        void Mapper27::SwapChr(const uint address,const uint data) const
        {
            ppu.Update();
            chr.SwapBank<SIZE_1K>( address, (chr.GetBank<SIZE_1K>(address) & MASK) | (data & BITS) << SHIFT );
        }
        
        void Mapper27::Poke_B0(void* p_,Address i_,Data j_) { static_cast<Mapper27*>(p_)->Poke_M_B0(i_,j_); } inline void Mapper27::Poke_M_B0(Address,Data data) { SwapChr<0xFF0,0x0F,0>( 0x0000, data ); }
        void Mapper27::Poke_B1(void* p_,Address i_,Data j_) { static_cast<Mapper27*>(p_)->Poke_M_B1(i_,j_); } inline void Mapper27::Poke_M_B1(Address,Data data) { SwapChr<0x00F,0xFF,4>( 0x0000, data ); }
        void Mapper27::Poke_B2(void* p_,Address i_,Data j_) { static_cast<Mapper27*>(p_)->Poke_M_B2(i_,j_); } inline void Mapper27::Poke_M_B2(Address,Data data) { SwapChr<0xFF0,0x0F,0>( 0x0400, data ); }
        void Mapper27::Poke_B3(void* p_,Address i_,Data j_) { static_cast<Mapper27*>(p_)->Poke_M_B3(i_,j_); } inline void Mapper27::Poke_M_B3(Address,Data data) { SwapChr<0x00F,0xFF,4>( 0x0400, data ); }
        void Mapper27::Poke_C0(void* p_,Address i_,Data j_) { static_cast<Mapper27*>(p_)->Poke_M_C0(i_,j_); } inline void Mapper27::Poke_M_C0(Address,Data data) { SwapChr<0xFF0,0x0F,0>( 0x0800, data ); }
        void Mapper27::Poke_C1(void* p_,Address i_,Data j_) { static_cast<Mapper27*>(p_)->Poke_M_C1(i_,j_); } inline void Mapper27::Poke_M_C1(Address,Data data) { SwapChr<0x00F,0xFF,4>( 0x0800, data ); }
        void Mapper27::Poke_C2(void* p_,Address i_,Data j_) { static_cast<Mapper27*>(p_)->Poke_M_C2(i_,j_); } inline void Mapper27::Poke_M_C2(Address,Data data) { SwapChr<0xFF0,0x0F,0>( 0x0C00, data ); }
        void Mapper27::Poke_C3(void* p_,Address i_,Data j_) { static_cast<Mapper27*>(p_)->Poke_M_C3(i_,j_); } inline void Mapper27::Poke_M_C3(Address,Data data) { SwapChr<0x00F,0xFF,4>( 0x0C00, data ); }
        void Mapper27::Poke_D0(void* p_,Address i_,Data j_) { static_cast<Mapper27*>(p_)->Poke_M_D0(i_,j_); } inline void Mapper27::Poke_M_D0(Address,Data data) { SwapChr<0xFF0,0x0F,0>( 0x1000, data ); }
        void Mapper27::Poke_D1(void* p_,Address i_,Data j_) { static_cast<Mapper27*>(p_)->Poke_M_D1(i_,j_); } inline void Mapper27::Poke_M_D1(Address,Data data) { SwapChr<0x00F,0xFF,4>( 0x1000, data ); }
        void Mapper27::Poke_D2(void* p_,Address i_,Data j_) { static_cast<Mapper27*>(p_)->Poke_M_D2(i_,j_); } inline void Mapper27::Poke_M_D2(Address,Data data) { SwapChr<0xFF0,0x0F,0>( 0x1400, data ); }
        void Mapper27::Poke_D3(void* p_,Address i_,Data j_) { static_cast<Mapper27*>(p_)->Poke_M_D3(i_,j_); } inline void Mapper27::Poke_M_D3(Address,Data data) { SwapChr<0x00F,0xFF,4>( 0x1400, data ); }
        void Mapper27::Poke_E0(void* p_,Address i_,Data j_) { static_cast<Mapper27*>(p_)->Poke_M_E0(i_,j_); } inline void Mapper27::Poke_M_E0(Address,Data data) { SwapChr<0xFF0,0x0F,0>( 0x1800, data ); }
        void Mapper27::Poke_E1(void* p_,Address i_,Data j_) { static_cast<Mapper27*>(p_)->Poke_M_E1(i_,j_); } inline void Mapper27::Poke_M_E1(Address,Data data) { SwapChr<0x00F,0xFF,4>( 0x1800, data ); }
        void Mapper27::Poke_E2(void* p_,Address i_,Data j_) { static_cast<Mapper27*>(p_)->Poke_M_E2(i_,j_); } inline void Mapper27::Poke_M_E2(Address,Data data) { SwapChr<0xFF0,0x0F,0>( 0x1C00, data ); }
        void Mapper27::Poke_E3(void* p_,Address i_,Data j_) { static_cast<Mapper27*>(p_)->Poke_M_E3(i_,j_); } inline void Mapper27::Poke_M_E3(Address,Data data) { SwapChr<0x00F,0xFF,4>( 0x1C00, data ); }
        
        void Mapper27::Poke_F0(void* p_,Address i_,Data j_) { static_cast<Mapper27*>(p_)->Poke_M_F0(i_,j_); } inline void Mapper27::Poke_M_F0(Address,Data data)
        {
            irq.WriteLatch0( data );
        }
        
        void Mapper27::Poke_F1(void* p_,Address i_,Data j_) { static_cast<Mapper27*>(p_)->Poke_M_F1(i_,j_); } inline void Mapper27::Poke_M_F1(Address,Data data)
        {
            irq.WriteLatch1( data );
        }
        
        void Mapper27::Poke_F2(void* p_,Address i_,Data j_) { static_cast<Mapper27*>(p_)->Poke_M_F2(i_,j_); } inline void Mapper27::Poke_M_F2(Address,Data data)
        {
            irq.Toggle( data );
        }
        
        void Mapper27::Poke_F3(void* p_,Address i_,Data j_) { static_cast<Mapper27*>(p_)->Poke_M_F3(i_,j_); } inline void Mapper27::Poke_M_F3(Address,Data)
        {
            irq.Toggle();
        }
        
        void Mapper27::Sync(Event event,Input::Controllers*)
        {
            if (event == EVENT_END_FRAME)
                irq.VSync();
        }
    }
}
