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
#include "NstMapper185.hpp"

namespace Nes
{
    namespace Core
    {
        
        
        
        
        void Mapper185::SubReset(bool)
        {
            Map( 0x8000U, 0xFFFFU, &Mapper185::Poke_Prg );
            
            openBus = 0;
        }
        
        void Mapper185::SubSave(State::Saver& state) const
        {
            state.Begin( AsciiId<'O','P','B'>::V ).Write8( openBus ).End();
        }
        
        void Mapper185::SubLoad(State::Loader& state)
        {
            while (const dword chunk = state.Begin())
            {
                if (chunk == AsciiId<'O','P','B'>::V)
                {
                    openBus = state.Read8() & 0x1;
                    
                    if (openBus)
                        chr.SetAccessors( this, &Mapper185::Access_Chr, &Mapper185::Access_Chr );
                    else
                        chr.ResetAccessors();
                }
                
                state.End();
            }
        }
        
        
        
        
        
        __attribute__((noinline)) Data Mapper185::Access_Chr(void* p_,Address i_) { return static_cast<Mapper185*>(p_)->Access_M_Chr(i_); } inline Data Mapper185::Access_M_Chr(Address address)
        {
            return 0xFF;
        }
        
        void Mapper185::Poke_Prg(void* p_,Address i_,Data j_) { static_cast<Mapper185*>(p_)->Poke_M_Prg(i_,j_); } inline void Mapper185::Poke_M_Prg(Address,Data data)
        {
            ppu.Update();
            
            chr.SwapBank<SIZE_8K,0x0000>( data );
            
            openBus ^= 0x1;
            
            if (openBus)
                chr.SetAccessors( this, &Mapper185::Access_Chr, &Mapper185::Access_Chr );
            else
                chr.ResetAccessors();
        }
    }
}
