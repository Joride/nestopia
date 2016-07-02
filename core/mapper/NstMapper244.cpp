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
#include "NstMapper244.hpp"

namespace Nes
{
    namespace Core
    {
        
        
        
        
        void Mapper244::SubReset(const bool hard)
        {
            Map( 0x8065U, 0x80A4U, &Mapper244::Poke_8065 );
            Map( 0x80A5U, 0x80E4U, &Mapper244::Poke_80A5 );
            
            if (hard)
                Poke_8065(this,0x8065,0x00);
        }
        
        
        
        
        
        void Mapper244::Poke_8065(void* p_,Address i_,Data j_) { static_cast<Mapper244*>(p_)->Poke_M_8065(i_,j_); } inline void Mapper244::Poke_M_8065(Address address,Data)
        {
            prg.SwapBank<SIZE_32K,0x0000>( (address - 0x8065) & 0x3 );
        }
        
        void Mapper244::Poke_80A5(void* p_,Address i_,Data j_) { static_cast<Mapper244*>(p_)->Poke_M_80A5(i_,j_); } inline void Mapper244::Poke_M_80A5(Address address,Data)
        {
            ppu.Update();
            chr.SwapBank<SIZE_8K,0x0000>( (address - 0x80A5) & 0x7 );
        }
    }
}
