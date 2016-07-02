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
#include "NstMapper140.hpp"

namespace Nes
{
    namespace Core
    {
        
        
        
        
        void Mapper140::SubReset(const bool hard)
        {
            Map( 0x6000U, 0x7FFFU, &Mapper140::Poke_6000 );
            
            if (hard)
                Poke_6000(this,0x6000,0x00);
        }
        
        
        
        
        
        void Mapper140::Poke_6000(void* p_,Address i_,Data j_) { static_cast<Mapper140*>(p_)->Poke_M_6000(i_,j_); } inline void Mapper140::Poke_M_6000(Address,Data data)
        {
            ppu.Update();
            chr.SwapBank<SIZE_8K,0x0000>( data );
            prg.SwapBank<SIZE_32K,0x0000>( data >> 4 );
        }
    }
}
