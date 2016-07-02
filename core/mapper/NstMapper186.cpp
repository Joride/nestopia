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

#include <cstring>
#include "../NstMapper.hpp"
#include "NstMapper186.hpp"

namespace Nes
{
    namespace Core
    {
        
        
        
        
        void Mapper186::SubReset(const bool hard)
        {
            Map( 0x4200U, 0x4201U, &Mapper186::Peek_4200 );
            Map( 0x4202U, &Mapper186::Peek_4202 );
            Map( 0x4203U, &Mapper186::Peek_4200 );
            Map( 0x4204U, 0x43FFU, &Mapper186::Peek_4204 );
            
            for (uint i=0x4200; i < 0x4400; i += 0x2)
            {
                Map( i + 0x0, &Mapper186::Poke_4200 );
                Map( i + 0x1, PRG_SWAP_16K_0 );
            }
            
            Map( 0x4400U, 0x4EFFU, &Mapper186::Peek_4400, &Mapper186::Poke_4400 );
            Map( WRK_PEEK );
            
            if (hard)
            {
                std::memset( ram, 0, sizeof(ram) );
                prg.SwapBanks<SIZE_16K,0x0000>(0,0);
            }
        }
        
        void Mapper186::SubLoad(State::Loader& state)
        {
            while (const dword chunk = state.Begin())
            {
                if (chunk == AsciiId<'R','A','M'>::V)
                    state.Uncompress( ram );
                
                state.End();
            }
        }
        
        void Mapper186::SubSave(State::Saver& state) const
        {
            state.Begin( AsciiId<'R','A','M'>::V ).Compress( ram ).End();
        }
        
        
        
        
        
        Data Mapper186::Peek_4200(void* p_,Address i_) { return static_cast<Mapper186*>(p_)->Peek_M_4200(i_); } inline Data Mapper186::Peek_M_4200(Address)
        {
            return 0x00;
        }
        
        Data Mapper186::Peek_4202(void* p_,Address i_) { return static_cast<Mapper186*>(p_)->Peek_M_4202(i_); } inline Data Mapper186::Peek_M_4202(Address)
        {
            return 0x40;
        }
        
        Data Mapper186::Peek_4204(void* p_,Address i_) { return static_cast<Mapper186*>(p_)->Peek_M_4204(i_); } inline Data Mapper186::Peek_M_4204(Address)
        {
            return 0xFF;
        }
        
        void Mapper186::Poke_4200(void* p_,Address i_,Data j_) { static_cast<Mapper186*>(p_)->Poke_M_4200(i_,j_); } inline void Mapper186::Poke_M_4200(Address,Data data)
        {
            wrk.SwapBank<SIZE_8K,0x0000>( data >> 6 );
        }
        
        Data Mapper186::Peek_4400(void* p_,Address i_) { return static_cast<Mapper186*>(p_)->Peek_M_4400(i_); } inline Data Mapper186::Peek_M_4400(Address address)
        {
            return ram[address - 0x4400];
        }
        
        void Mapper186::Poke_4400(void* p_,Address i_,Data j_) { static_cast<Mapper186*>(p_)->Poke_M_4400(i_,j_); } inline void Mapper186::Poke_M_4400(Address address,Data data)
        {
            ram[address - 0x4400] = data;
        }
    }
}
