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
#include "NstMapper188.hpp"
#include "../api/NstApiInput.hpp"

namespace Nes
{
    namespace Core
    {
        
        
        
        
        void Mapper188::SubReset(const bool hard)
        {
            if (hard)
                prg.SwapBank<SIZE_16K,0x4000>(7);
            
            Map( 0x6000U, 0x7FFFU, &Mapper188::Peek_Mic );
            Map( 0x8000U, 0xFFFFU, &Mapper188::Poke_Prg );
        }
        
        
        
        
        
        void Mapper188::Poke_Prg(void* p_,Address i_,Data j_) { static_cast<Mapper188*>(p_)->Poke_M_Prg(i_,j_); } inline void Mapper188::Poke_M_Prg(Address,Data data)
        {
            prg.SwapBank<SIZE_16K,0x0000>
            (
             data ? (data & 0x7) | (~data >> 1 & 0x8) : (prg.Source().Size() >> 18) + 0x7
             );
        }
        
        Data Mapper188::Peek_Mic(void* p_,Address i_) { return static_cast<Mapper188*>(p_)->Peek_M_Mic(i_); } inline Data Mapper188::Peek_M_Mic(Address)
        {
            return mic;
        }
        
        void Mapper188::Sync(Event event,Input::Controllers* controllers)
        {
            if (event == EVENT_BEGIN_FRAME)
            {
                if (controllers)
                {
                    Input::Controllers::KaraokeStudio::callback( controllers->karaokeStudio );
                    mic = controllers->karaokeStudio.buttons & 0x7 ^ 0x3;
                }
                else
                {
                    mic = 0x3;
                }
            }
        }
    }
}
