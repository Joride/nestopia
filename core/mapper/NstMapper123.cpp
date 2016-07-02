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
#include "NstMapper123.hpp"

namespace Nes
{
    namespace Core
    {
        
        
        
        
        void Mapper123::SubReset(const bool hard)
        {
            if (hard)
            {
                exRegs[0] = 0;
                exRegs[1] = 0;
            }
            
            Mmc3::SubReset( hard );
            
            Map( 0x5000U, 0x57FFU, &Mapper123::Peek_5000 );
            Map( 0x5800U, 0x5FFFU, &Mapper123::Peek_5000, &Mapper123::Poke_5800 );
            
            for (uint i=0x8000; i < 0x9000; i += 0x2)
                Map( i, &Mapper123::Poke_8000 );
        }
        
        void Mapper123::SubLoad(State::Loader& state)
        {
            while (const dword chunk = state.Begin())
            {
                if (chunk == AsciiId<'R','E','G'>::V)
                {
                    State::Loader::Data<2> data( state );
                    
                    exRegs[0] = data[0];
                    exRegs[1] = data[1];
                }
                
                state.End();
            }
        }
        
        void Mapper123::SubSave(State::Saver& state) const
        {
            const byte data[2] =
            {
                exRegs[0],
                exRegs[1]
            };
            
            state.Begin( AsciiId<'R','E','G'>::V ).Write( data ).End();
        }
        
        
        
        
        
        void Mapper123::UpdatePrg()
        {
            if (!(exRegs[0] & 0x40))
                Mmc3::UpdatePrg();
        }
        
        Data Mapper123::Peek_5000(void* p_,Address i_) { return static_cast<Mapper123*>(p_)->Peek_M_5000(i_); } inline Data Mapper123::Peek_M_5000(Address address)
        {
            return (address >> 8 & 0xFE) | (((~address & 0x1) & (address >> 8 & 0x1)) ^ 0x1);
        }
        
        void Mapper123::Poke_5800(void* p_,Address i_,Data j_) { static_cast<Mapper123*>(p_)->Poke_M_5800(i_,j_); } inline void Mapper123::Poke_M_5800(Address address,Data data)
        {
            address &= 0x1;
            
            if (exRegs[address] != data)
            {
                exRegs[address] = data;
                
                if (exRegs[0] & 0x40)
                {
                    data = (exRegs[0] & 0x5) | (exRegs[0] >> 2 & 0x2) | (exRegs[0] >> 2 & 0x8);
                    address = exRegs[0] >> 1 & 0x1;
                    
                    prg.SwapBanks<SIZE_16K,0x0000>( data & ~address, data | address );
                }
                else
                {
                    Mmc3::UpdatePrg();
                }
            }
        }
        
        void Mapper123::Poke_8000(void* p_,Address i_,Data j_) { static_cast<Mapper123*>(p_)->Poke_M_8000(i_,j_); } inline void Mapper123::Poke_M_8000(Address address,Data data)
        {
            static const byte security[8] = {0,3,1,5,6,7,2,4};
            Mmc3::Poke_8000(this,address,(data & 0xC0) | security[data & 0x07]);
        }
    }
}
