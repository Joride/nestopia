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
#include "NstBrdWs.hpp"

namespace Nes
{
    namespace Core
    {
        namespace Boards
        {
            
            
            
            
            void Ws::SubReset(const bool hard)
            {
                reg = 0x00;
                
                for (uint i=0x6000; i < 0x7000; i += 0x2)
                {
                    Map( i + 0x0, &Ws::Poke_6000 );
                    Map( i + 0x1, &Ws::Poke_6001 );
                }
                
                if (hard)
                    Poke_6000(this,0x6000,0x00);
            }
            
            void Ws::SubLoad(State::Loader& state)
            {
                while (const dword chunk = state.Begin())
                {
                    if (chunk == AsciiId<'R','E','G'>::V)
                        reg = state.Read8() & 0x20;
                    
                    state.End();
                }
            }
            
            void Ws::SubSave(State::Saver& state) const
            {
                state.Begin( AsciiId<'R','E','G'>::V ).Write8( reg ).End();
            }
            
            
            
            
            
            void Ws::Poke_6000(void* p_,Address i_,Data j_) { static_cast<Ws*>(p_)->Poke_M_6000(i_,j_); } inline void Ws::Poke_M_6000(Address,Data data)
            {
                if (!reg)
                {
                    reg = data & 0x20;
                    prg.SwapBanks<SIZE_16K,0x0000>( data & ~(~data >> 3 & 0x1), data | (~data >> 3 & 0x1) );
                    ppu.SetMirroring( (data & 0x10) ? Ppu::NMT_HORIZONTAL : Ppu::NMT_VERTICAL );
                }
            }
            
            void Ws::Poke_6001(void* p_,Address i_,Data j_) { static_cast<Ws*>(p_)->Poke_M_6001(i_,j_); } inline void Ws::Poke_M_6001(Address,Data data)
            {
                if (!reg)
                {
                    ppu.Update();
                    chr.SwapBank<SIZE_8K,0x0000>( data );
                }
            }
        }
    }
}
