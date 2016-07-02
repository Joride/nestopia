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
#include "NstMapper232.hpp"

namespace Nes
{
    namespace Core
    {
        
        
        
        
        Mapper232::Mapper232(Context& c)
        :
        Mapper (c,c.attribute == ATR_BF9096 || c.attribute == ATR_BF9096_ALADDIN ? PROM_MAX_256K|CROM_MAX_8K|WRAM_NONE : PROM_MAX_256K|CROM_MAX_8K|WRAM_DEFAULT),
        aladdin (c.attribute == ATR_BF9096_ALADDIN)
        {}
        
        void Mapper232::SubReset(bool)
        {
            Map( 0x8000U, 0xBFFFU, aladdin ? &Mapper232::Poke_8000_A : &Mapper232::Poke_8000 );
            Map( 0xC000U, 0xFFFFU, &Mapper232::Poke_A000 );
        }
        
        
        
        
        
        void Mapper232::SwapBasePrg(uint base)
        {
            prg.SwapBanks<SIZE_16K,0x0000>( base | (prg.GetBank<SIZE_16K,0x0000>() & 0x3), base | 0x3 );
        }
        
        void Mapper232::Poke_8000(void* p_,Address i_,Data j_) { static_cast<Mapper232*>(p_)->Poke_M_8000(i_,j_); } inline void Mapper232::Poke_M_8000(Address,Data data)
        {
            SwapBasePrg( (data & 0x8) | (data >> 2 & 0x4) );
        }
        
        void Mapper232::Poke_8000_A(void* p_,Address i_,Data j_) { static_cast<Mapper232*>(p_)->Poke_M_8000_A(i_,j_); } inline void Mapper232::Poke_M_8000_A(Address,Data data)
        {
            SwapBasePrg( data >> 1 & 0xC );
        }
        
        void Mapper232::Poke_A000(void* p_,Address i_,Data j_) { static_cast<Mapper232*>(p_)->Poke_M_A000(i_,j_); } inline void Mapper232::Poke_M_A000(Address,Data data)
        {
            prg.SwapBank<SIZE_16K,0x0000>( (prg.GetBank<SIZE_16K,0x0000>() & 0xC) | (data & 0x3) );
        }
    }
}
