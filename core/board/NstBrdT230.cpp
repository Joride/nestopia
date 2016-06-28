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
#include "NstBrdVrc4.hpp"
#include "NstBrdT230.hpp"

namespace Nes
{
    namespace Core
    {
        namespace Boards
        {
            
            
            
            
            void T230::SubReset(const bool hard)
            {
                Vrc4::SubReset( hard );
                
                Map( 0x8000U, 0x8FFFU, NOP_POKE );
                Map( 0xA000U, 0xAFFFU, &T230::Poke_A000 );
            }
            
            
            
            
            
            void T230::Poke_A000(void* p_,Address i_,Data j_) { static_cast<T230*>(p_)->Poke_M_A000(i_,j_); } inline void T230::Poke_M_A000(Address,Data data)
            {
                data = data << 1 & 0x1F;
                prg.SwapBank<SIZE_8K>( prgSwap << 13, data );
                prg.SwapBank<SIZE_8K,0x2000>( data | 0x1 );
            }
        }
    }
}
