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
#include "NstMapper147.hpp"

namespace Nes
{
    namespace Core
    {
        
        
        
        
        void Mapper147::SubReset(const bool hard)
        {
            for (dword i=0x4100; i <= 0xFFFF; ++i)
            {
                if ((i & 0x103) == 0x102)
                    Map( i, &Mapper147::Poke_4100 );
            }
            
            if (hard)
                Poke_4100(this,0x4102,0x00);
        }
        
        
        
        
        
        void Mapper147::Poke_4100(void* p_,Address i_,Data j_) { static_cast<Mapper147*>(p_)->Poke_M_4100(i_,j_); } inline void Mapper147::Poke_M_4100(Address,Data data)
        {
            ppu.Update();
            prg.SwapBank<SIZE_32K,0x0000>( (data >> 6 & 0x2) | (data >> 2 & 0x1) );
            chr.SwapBank<SIZE_8K,0x0000>( data >> 3 );
        }
    }
}
