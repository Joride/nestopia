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
#include "NstMapper250.hpp"

namespace Nes
{
    namespace Core
    {
        
        
        
        
        void Mapper250::SubReset(const bool hard)
        {
            Mmc3::SubReset( hard );
            
            Map( 0x8000U, 0xFFFFU, &Mapper250::Poke_Prg );
        }
        
        
        
        
        
        void Mapper250::Poke_Prg(void* p_,Address i_,Data j_) { static_cast<Mapper250*>(p_)->Poke_M_Prg(i_,j_); } inline void Mapper250::Poke_M_Prg(Address address,Data)
        {
            const uint data = address & 0xFF;
            address = (address & 0xE000) | (address >> 10 & 0x1);
            
            switch (address)
            {
                case 0x8000: Mmc3::Poke_8000(this,address,data); break;
                case 0x8001: Mmc3::Poke_8001(this,address,data); break;
                case 0xA000: SetMirroringHV( data ); break;
                case 0xA001: Mmc3::Poke_A001(this,address,data); break;
                case 0xC000: Mmc3::Poke_C000(this,address,data); break;
                case 0xC001: Mmc3::Poke_C001(this,address,data); break;
                case 0xE000: Mmc3::Poke_E000(this,address,data); break;
                case 0xE001: Mmc3::Poke_E001(this,address,data); break;
            }
        }
    }
}
