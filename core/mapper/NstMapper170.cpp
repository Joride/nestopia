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
#include "NstMapper170.hpp"

namespace Nes
{
    namespace Core
    {
        
        
        
        
        void Mapper170::SubReset(bool)
        {
            reg = 0x00;
            
            Map( 0x6502U, &Mapper170::Poke_7000 );
            Map( 0x7000U, &Mapper170::Poke_7000 );
            Map( 0x7001U, &Mapper170::Peek_7001 );
            Map( 0x7777U, &Mapper170::Peek_7001 );
        }
        
        void Mapper170::SubLoad(State::Loader& state)
        {
            while (const dword chunk = state.Begin())
            {
                if (chunk == AsciiId<'R','E','G'>::V)
                    reg = state.Read8() & 0x80;
                
                state.End();
            }
        }
        
        void Mapper170::SubSave(State::Saver& state) const
        {
            state.Begin( AsciiId<'R','E','G'>::V ).Write8( reg ).End();
        }
        
        
        
        
        
        void Mapper170::Poke_7000(void* p_,Address i_,Data j_) { static_cast<Mapper170*>(p_)->Poke_M_7000(i_,j_); } inline void Mapper170::Poke_M_7000(Address,Data data)
        {
            reg = data << 1 & 0x80;
        }
        
        Data Mapper170::Peek_7001(void* p_,Address i_) { return static_cast<Mapper170*>(p_)->Peek_M_7001(i_); } inline Data Mapper170::Peek_M_7001(Address address)
        {
            return reg | (address >> 8 & 0x7F);
        }
    }
}
