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
#include "NstMapper082.hpp"

namespace Nes
{
    namespace Core
    {
        
        
        
        
        void Mapper82::SubReset(const bool hard)
        {
            if (hard)
                swapOffset = 0x0000;
            
            Map( 0x7EF0U, &Mapper82::Poke_7EF0 );
            Map( 0x7EF1U, &Mapper82::Poke_7EF1 );
            Map( 0x7EF2U, &Mapper82::Poke_7EF2 );
            Map( 0x7EF3U, &Mapper82::Poke_7EF3 );
            Map( 0x7EF4U, &Mapper82::Poke_7EF4 );
            Map( 0x7EF5U, &Mapper82::Poke_7EF5 );
            Map( 0x7EF6U, &Mapper82::Poke_7EF6 );
            Map( 0x7EFAU, &Mapper82::Poke_7EFA );
            Map( 0x7EFBU, &Mapper82::Poke_7EFB );
            Map( 0x7EFCU, &Mapper82::Poke_7EFC );
        }
        
        void Mapper82::SubLoad(State::Loader& state)
        {
            while (const dword chunk = state.Begin())
            {
                if (chunk == AsciiId<'R','E','G'>::V)
                    swapOffset = (state.Read8() & 0x2) << 11;
                
                state.End();
            }
        }
        
        void Mapper82::SubSave(State::Saver& state) const
        {
            state.Begin( AsciiId<'R','E','G'>::V ).Write8( swapOffset >> 11 ).End();
        }
        
        
        
        
        
        void Mapper82::Poke_7EF0(void* p_,Address i_,Data j_) { static_cast<Mapper82*>(p_)->Poke_M_7EF0(i_,j_); } inline void Mapper82::Poke_M_7EF0(Address,Data data)
        {
            ppu.Update();
            chr.SwapBank<SIZE_2K>( swapOffset | 0x0000, data >> 1 );
        }
        
        void Mapper82::Poke_7EF1(void* p_,Address i_,Data j_) { static_cast<Mapper82*>(p_)->Poke_M_7EF1(i_,j_); } inline void Mapper82::Poke_M_7EF1(Address,Data data)
        {
            ppu.Update();
            chr.SwapBank<SIZE_2K>( swapOffset | 0x0800, data >> 1 );
        }
        
        void Mapper82::Poke_7EF2(void* p_,Address i_,Data j_) { static_cast<Mapper82*>(p_)->Poke_M_7EF2(i_,j_); } inline void Mapper82::Poke_M_7EF2(Address,Data data)
        {
            ppu.Update();
            chr.SwapBank<SIZE_1K>( swapOffset ^ 0x1000, data );
        }
        
        void Mapper82::Poke_7EF3(void* p_,Address i_,Data j_) { static_cast<Mapper82*>(p_)->Poke_M_7EF3(i_,j_); } inline void Mapper82::Poke_M_7EF3(Address,Data data)
        {
            ppu.Update();
            chr.SwapBank<SIZE_1K>( swapOffset ^ 0x1400, data );
        }
        
        void Mapper82::Poke_7EF4(void* p_,Address i_,Data j_) { static_cast<Mapper82*>(p_)->Poke_M_7EF4(i_,j_); } inline void Mapper82::Poke_M_7EF4(Address,Data data)
        {
            ppu.Update();
            chr.SwapBank<SIZE_1K>( swapOffset ^ 0x1800, data );
        }
        
        void Mapper82::Poke_7EF5(void* p_,Address i_,Data j_) { static_cast<Mapper82*>(p_)->Poke_M_7EF5(i_,j_); } inline void Mapper82::Poke_M_7EF5(Address,Data data)
        {
            ppu.Update();
            chr.SwapBank<SIZE_1K>( swapOffset ^ 0x1C00, data );
        }
        
        void Mapper82::Poke_7EF6(void* p_,Address i_,Data j_) { static_cast<Mapper82*>(p_)->Poke_M_7EF6(i_,j_); } inline void Mapper82::Poke_M_7EF6(Address,Data data)
        {
            swapOffset = (data & 0x2) << 11;
            ppu.SetMirroring( (data & 0x1) ? Ppu::NMT_VERTICAL : Ppu::NMT_HORIZONTAL );
        }
        
        void Mapper82::Poke_7EFA(void* p_,Address i_,Data j_) { static_cast<Mapper82*>(p_)->Poke_M_7EFA(i_,j_); } inline void Mapper82::Poke_M_7EFA(Address,Data data) { prg.SwapBank<SIZE_8K,0x0000>(data >> 2); }
        void Mapper82::Poke_7EFB(void* p_,Address i_,Data j_) { static_cast<Mapper82*>(p_)->Poke_M_7EFB(i_,j_); } inline void Mapper82::Poke_M_7EFB(Address,Data data) { prg.SwapBank<SIZE_8K,0x2000>(data >> 2); }
        void Mapper82::Poke_7EFC(void* p_,Address i_,Data j_) { static_cast<Mapper82*>(p_)->Poke_M_7EFC(i_,j_); } inline void Mapper82::Poke_M_7EFC(Address,Data data) { prg.SwapBank<SIZE_8K,0x4000>(data >> 2); }
    }
}
