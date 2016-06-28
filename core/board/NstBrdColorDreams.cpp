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
#include "NstBrdColorDreams.hpp"

namespace Nes
{
    namespace Core
    {
        namespace Boards
        {
            
            
            
            
            ColorDreams::ColorDreams(Context& c,Type t)
            :
            Mapper (c,PROM_MAX_128K|CROM_MAX_128K|WRAM_DEFAULT),
            type (t)
            {}
            
            void ColorDreams::SubReset(const bool hard)
            {
                Map( 0x8000U + (type == TYPE_AGCI50282), 0xFFFFU, &ColorDreams::Poke_Prg );
                
                if (hard)
                    Poke_Prg(this,0x8001,0x00);
            }
            
            
            
            
            
            void ColorDreams::Poke_Prg(void* p_,Address i_,Data j_) { static_cast<ColorDreams*>(p_)->Poke_M_Prg(i_,j_); } inline void ColorDreams::Poke_M_Prg(Address,Data data)
            {
                ppu.Update();
                prg.SwapBank<SIZE_32K,0x0000>( data );
                chr.SwapBank<SIZE_8K,0x0000>( data >> 4 );
            }
        }
    }
}
