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
#include "../board/NstBrdMmc3.hpp"
#include "NstMapper189.hpp"

namespace Nes
{
    namespace Core
    {
        
        
        
        
        void Mapper189::SubReset(const bool hard)
        {
            Mmc3::SubReset( hard );
            
            Map( 0x4120U, 0x7FFFU, &Mapper189::Poke_4120 );
            
            if (hard)
                prg.SwapBank<SIZE_32K,0x0000>(0);
        }
        
        
        
        
        
        void Mapper189::Poke_4120(void* p_,Address i_,Data j_) { static_cast<Mapper189*>(p_)->Poke_M_4120(i_,j_); } inline void Mapper189::Poke_M_4120(Address,Data data)
        {
            prg.SwapBank<SIZE_32K,0x0000>( (data >> 4) | data );
        }
        
        void Mapper189::UpdatePrg()
        {
            
        }
    }
}
