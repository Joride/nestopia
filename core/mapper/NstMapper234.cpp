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
#include "NstMapper234.hpp"

namespace Nes
{
    namespace Core
    {
        
        
        
        
        void Mapper234::SubReset(const bool hard)
        {
            Map( 0xFF80U, 0xFF9FU, &Mapper234::Peek_FF80, &Mapper234::Poke_FF80 );
            Map( 0xFFE8U, 0xFFF7U, &Mapper234::Peek_FFE8, &Mapper234::Poke_FFE8 );
            
            if (hard)
            {
                regs[0] = 0;
                regs[1] = 0;
                
                Update();
            }
        }
        
        void Mapper234::SubLoad(State::Loader& state)
        {
            while (const dword chunk = state.Begin())
            {
                if (chunk == AsciiId<'R','E','G'>::V)
                {
                    State::Loader::Data<2> data( state );
                    
                    regs[0] = data[0];
                    regs[1] = data[1];
                }
                
                state.End();
            }
        }
        
        void Mapper234::SubSave(State::Saver& state) const
        {
            state.Begin( AsciiId<'R','E','G'>::V ).Write16( regs[0] | uint(regs[1]) << 8 ).End();
        }
        
        
        
        
        
        void Mapper234::Update()
        {
            prg.SwapBank<SIZE_32K,0x0000>( (regs[0] & 0xE) | (regs[regs[0] >> 6 & 0x1] & 0x1) );
            chr.SwapBank<SIZE_8K,0x0000>( (regs[0] << 2 & (regs[0] >> 4 & 0x4 ^ 0x3C)) | (regs[1] >> 4 & (regs[0] >> 4 & 0x4 | 0x3)) );
        }
        
        void Mapper234::Poke_FF80(void* p_,Address i_,Data j_) { static_cast<Mapper234*>(p_)->Poke_M_FF80(i_,j_); } inline void Mapper234::Poke_M_FF80(Address,Data data)
        {
            if (!(regs[0] & 0x3F))
            {
                regs[0] = data;
                ppu.SetMirroring( (data & 0x80) ? Ppu::NMT_HORIZONTAL : Ppu::NMT_VERTICAL );
                Update();
            }
        }
        
        Data Mapper234::Peek_FF80(void* p_,Address i_) { return static_cast<Mapper234*>(p_)->Peek_M_FF80(i_); } inline Data Mapper234::Peek_M_FF80(Address address)
        {
            const uint data = prg[3][address - 0xE000];
            Poke_FF80(this,address,data);
            return data;
        }
        
        void Mapper234::Poke_FFE8(void* p_,Address i_,Data j_) { static_cast<Mapper234*>(p_)->Poke_M_FFE8(i_,j_); } inline void Mapper234::Poke_M_FFE8(Address,Data data)
        {
            regs[1] = data;
            ppu.Update();
            Update();
        }
        
        Data Mapper234::Peek_FFE8(void* p_,Address i_) { return static_cast<Mapper234*>(p_)->Peek_M_FFE8(i_); } inline Data Mapper234::Peek_M_FFE8(Address address)
        {
            const uint data = prg[3][address - 0xE000];
            Poke_FFE8(this,address,data);
            return data;
        }
    }
}
