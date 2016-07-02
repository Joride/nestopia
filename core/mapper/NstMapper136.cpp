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
#include "NstMapper136.hpp"

namespace Nes
{
    namespace Core
    {
        
        
        
        
        void Mapper136::SubReset(const bool hard)
        {
            if (hard)
                reg = 0;
            
            for (uint i=0x4100; i < 0x6000; i += 0x200)
            {
                for (uint j=0x0; j < 0x100; j += 0x4)
                {
                    Map( i + j + 0x0, &Mapper136::Peek_4100 );
                    Map( i + j + 0x2, &Mapper136::Poke_4102 );
                }
            }
        }
        
        void Mapper136::SubLoad(State::Loader& state)
        {
            while (const dword chunk = state.Begin())
            {
                if (chunk == AsciiId<'R','E','G'>::V)
                    reg = state.Read8();
                
                state.End();
            }
        }
        
        void Mapper136::SubSave(State::Saver& state) const
        {
            state.Begin( AsciiId<'R','E','G'>::V ).Write8( reg ).End();
        }
        
        
        
        
        
        Data Mapper136::Peek_4100(void* p_,Address i_) { return static_cast<Mapper136*>(p_)->Peek_M_4100(i_); } inline Data Mapper136::Peek_M_4100(Address)
        {
            return reg | 0x40;
        }
        
        void Mapper136::Poke_4102(void* p_,Address i_,Data j_) { static_cast<Mapper136*>(p_)->Poke_M_4102(i_,j_); } inline void Mapper136::Poke_M_4102(Address,Data data)
        {
            ppu.Update();
            reg = (data & 0x30) | ((data+3) & 0x0F);
            chr.SwapBank<SIZE_8K,0x0000>( reg );
        }
    }
}
