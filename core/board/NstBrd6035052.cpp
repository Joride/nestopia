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
#include "NstBrdMmc3.hpp"
#include "NstBrd6035052.hpp"

namespace Nes
{
    namespace Core
    {
        namespace Boards
        {
            
            
            
            
            void Unl6035052::SubReset(const bool hard)
            {
                security = 0x0;
                
                Mmc3::SubReset( hard );
                
                Map( 0x4020U, 0x7FFFU, &Unl6035052::Peek_4020, &Unl6035052::Poke_4020 );
            }
            
            void Unl6035052::SubLoad(State::Loader& state)
            {
                while (const dword chunk = state.Begin())
                {
                    if (chunk == AsciiId<'R','E','G'>::V)
                        security = state.Read8() & 0x3;
                    
                    state.End();
                }
            }
            
            void Unl6035052::SubSave(State::Saver& state) const
            {
                state.Begin( AsciiId<'R','E','G'>::V ).Write8( security ).End();
            }
            
            
            
            
            
            Data Unl6035052::Peek_4020(void* p_,Address i_) { return static_cast<Unl6035052*>(p_)->Peek_M_4020(i_); } inline Data Unl6035052::Peek_M_4020(Address)
            {
                return security;
            }
            
            void Unl6035052::Poke_4020(void* p_,Address i_,Data j_) { static_cast<Unl6035052*>(p_)->Poke_M_4020(i_,j_); } inline void Unl6035052::Poke_M_4020(Address,Data data)
            {
                security = data & 0x3;
                
                if (security == 0x1)
                    security = 0x2;
            }
        }
    }
}
