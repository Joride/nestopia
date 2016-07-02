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
#include "NstMapper208.hpp"

namespace Nes
{
    namespace Core
    {
        
        
        
        
        void Mapper208::SubReset(const bool hard)
        {
            if (hard)
            {
                regs.select = 0;
                
                for (uint i=0; i < 4; ++i)
                    regs.buffer[i] = 0;
            }
            
            Mmc3::SubReset( hard );
            
            Map( 0x4800U, 0x4FFFU, &Mapper208::Poke_4800 );
            Map( 0x5000U, 0x57FFU, &Mapper208::Poke_5000 );
            Map( 0x5800U, 0x5FFFU, &Mapper208::Peek_5800, &Mapper208::Poke_5800 );
        }
        
        void Mapper208::SubLoad(State::Loader& state)
        {
            while (const dword chunk = state.Begin())
            {
                if (chunk == AsciiId<'R','E','G'>::V)
                {
                    state.Read( regs.buffer );
                    regs.select = state.Read8();
                }
                
                state.End();
            }
        }
        
        void Mapper208::SubSave(State::Saver& state) const
        {
            state.Begin( AsciiId<'R','E','G'>::V ).Write( regs.buffer ).Write8( regs.select ).End();
        }
        
        
        
        
        
        void Mapper208::UpdatePrg()
        {
            
        }
        
        void Mapper208::Poke_4800(void* p_,Address i_,Data j_) { static_cast<Mapper208*>(p_)->Poke_M_4800(i_,j_); } inline void Mapper208::Poke_M_4800(Address,Data data)
        {
            prg.SwapBank<SIZE_32K,0x0000>( (data & 0x1) | (data >> 3 & 0x2) );
        }
        
        void Mapper208::Poke_5000(void* p_,Address i_,Data j_) { static_cast<Mapper208*>(p_)->Poke_M_5000(i_,j_); } inline void Mapper208::Poke_M_5000(Address,Data data)
        {
            regs.select = data;
        }
        
        void Mapper208::Poke_5800(void* p_,Address i_,Data j_) { static_cast<Mapper208*>(p_)->Poke_M_5800(i_,j_); } inline void Mapper208::Poke_M_5800(Address address,Data data)
        {
            static const byte lut[256] =
            {
                0x59,0x59,0x59,0x59,0x59,0x59,0x59,0x59,0x59,0x49,0x19,0x09,0x59,0x49,0x19,0x09,
                0x59,0x59,0x59,0x59,0x59,0x59,0x59,0x59,0x51,0x41,0x11,0x01,0x51,0x41,0x11,0x01,
                0x59,0x59,0x59,0x59,0x59,0x59,0x59,0x59,0x59,0x49,0x19,0x09,0x59,0x49,0x19,0x09,
                0x59,0x59,0x59,0x59,0x59,0x59,0x59,0x59,0x51,0x41,0x11,0x01,0x51,0x41,0x11,0x01,
                0x00,0x10,0x40,0x50,0x00,0x10,0x40,0x50,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x08,0x18,0x48,0x58,0x08,0x18,0x48,0x58,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x10,0x40,0x50,0x00,0x10,0x40,0x50,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x08,0x18,0x48,0x58,0x08,0x18,0x48,0x58,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x59,0x59,0x59,0x59,0x59,0x59,0x59,0x59,0x58,0x48,0x18,0x08,0x58,0x48,0x18,0x08,
                0x59,0x59,0x59,0x59,0x59,0x59,0x59,0x59,0x50,0x40,0x10,0x00,0x50,0x40,0x10,0x00,
                0x59,0x59,0x59,0x59,0x59,0x59,0x59,0x59,0x58,0x48,0x18,0x08,0x58,0x48,0x18,0x08,
                0x59,0x59,0x59,0x59,0x59,0x59,0x59,0x59,0x50,0x40,0x10,0x00,0x50,0x40,0x10,0x00,
                0x01,0x11,0x41,0x51,0x01,0x11,0x41,0x51,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x09,0x19,0x49,0x59,0x09,0x19,0x49,0x59,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x01,0x11,0x41,0x51,0x01,0x11,0x41,0x51,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x09,0x19,0x49,0x59,0x09,0x19,0x49,0x59,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
            };
            
            regs.buffer[address & 0x3] = data ^ lut[regs.select];
        }
        
        Data Mapper208::Peek_5800(void* p_,Address i_) { return static_cast<Mapper208*>(p_)->Peek_M_5800(i_); } inline Data Mapper208::Peek_M_5800(Address address)
        {
            return regs.buffer[address & 0x3];
        }
    }
}
