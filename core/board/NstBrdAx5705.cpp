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
#include "NstBrdAx5705.hpp"

namespace Nes
{
    namespace Core
    {
        namespace Boards
        {
            
            
            
            
            void Ax5705::SubReset(bool)
            {
                for (uint i=0x0000; i < 0x1000; i += 0x10)
                {
                    Map( i + 0x8000, &Ax5705::Poke_8000 );
                    Map( i + 0x8008, NMT_SWAP_HV );
                    Map( i + 0xA000, &Ax5705::Poke_8000 );
                    Map( i + 0xA008, &Ax5705::Poke_A008 );
                    Map( i + 0xA009, &Ax5705::Poke_A009 );
                    Map( i + 0xA00A, &Ax5705::Poke_A00A );
                    Map( i + 0xA00B, &Ax5705::Poke_A00B );
                    Map( i + 0xC000, &Ax5705::Poke_C000 );
                    Map( i + 0xC001, &Ax5705::Poke_C001 );
                    Map( i + 0xC002, &Ax5705::Poke_C002 );
                    Map( i + 0xC003, &Ax5705::Poke_C003 );
                    Map( i + 0xC008, &Ax5705::Poke_C008 );
                    Map( i + 0xC009, &Ax5705::Poke_C009 );
                    Map( i + 0xC00A, &Ax5705::Poke_C00A );
                    Map( i + 0xC00B, &Ax5705::Poke_C00B );
                    Map( i + 0xE000, &Ax5705::Poke_E000 );
                    Map( i + 0xE001, &Ax5705::Poke_E001 );
                    Map( i + 0xE002, &Ax5705::Poke_E002 );
                    Map( i + 0xE003, &Ax5705::Poke_E003 );
                }
            }
            
            
            
            
            
            void Ax5705::Poke_8000(void* p_,Address i_,Data j_) { static_cast<Ax5705*>(p_)->Poke_M_8000(i_,j_); } inline void Ax5705::Poke_M_8000(Address address,Data data)
            {
                prg.SwapBank<SIZE_8K>( address & 0x2000, (data << 2 & 0x8) | (data & 0x5) | (data >> 2 & 0x2) );
            }
            
            void Ax5705::SwapChr(uint address,uint data) const
            {
                ppu.Update();
                chr.SwapBank<SIZE_1K>( address, data );
            }
            
            void Ax5705::SwapChrLo(uint address,uint data) const
            {
                SwapChr( address, (data & 0x0F) | (chr.GetBank<SIZE_1K>(address) & 0xF0) );
            }
            
            void Ax5705::SwapChrHi(uint address,uint data) const
            {
                SwapChr( address, (chr.GetBank<SIZE_1K>(address) & 0x0F) | (data << 3 & 0x20) | (data << 5 & 0x40) | (data << 4 & 0x90) );
            }
            
            void Ax5705::Poke_A008(void* p_,Address i_,Data j_) { static_cast<Ax5705*>(p_)->Poke_M_A008(i_,j_); } inline void Ax5705::Poke_M_A008(Address,Data data) { SwapChrLo( 0x0000, data ); }
            void Ax5705::Poke_A009(void* p_,Address i_,Data j_) { static_cast<Ax5705*>(p_)->Poke_M_A009(i_,j_); } inline void Ax5705::Poke_M_A009(Address,Data data) { SwapChrHi( 0x0000, data ); }
            void Ax5705::Poke_A00A(void* p_,Address i_,Data j_) { static_cast<Ax5705*>(p_)->Poke_M_A00A(i_,j_); } inline void Ax5705::Poke_M_A00A(Address,Data data) { SwapChrLo( 0x0400, data ); }
            void Ax5705::Poke_A00B(void* p_,Address i_,Data j_) { static_cast<Ax5705*>(p_)->Poke_M_A00B(i_,j_); } inline void Ax5705::Poke_M_A00B(Address,Data data) { SwapChrHi( 0x0400, data ); }
            void Ax5705::Poke_C000(void* p_,Address i_,Data j_) { static_cast<Ax5705*>(p_)->Poke_M_C000(i_,j_); } inline void Ax5705::Poke_M_C000(Address,Data data) { SwapChrLo( 0x0800, data ); }
            void Ax5705::Poke_C001(void* p_,Address i_,Data j_) { static_cast<Ax5705*>(p_)->Poke_M_C001(i_,j_); } inline void Ax5705::Poke_M_C001(Address,Data data) { SwapChrHi( 0x0800, data ); }
            void Ax5705::Poke_C002(void* p_,Address i_,Data j_) { static_cast<Ax5705*>(p_)->Poke_M_C002(i_,j_); } inline void Ax5705::Poke_M_C002(Address,Data data) { SwapChrLo( 0x0C00, data ); }
            void Ax5705::Poke_C003(void* p_,Address i_,Data j_) { static_cast<Ax5705*>(p_)->Poke_M_C003(i_,j_); } inline void Ax5705::Poke_M_C003(Address,Data data) { SwapChrHi( 0x0C00, data ); }
            void Ax5705::Poke_C008(void* p_,Address i_,Data j_) { static_cast<Ax5705*>(p_)->Poke_M_C008(i_,j_); } inline void Ax5705::Poke_M_C008(Address,Data data) { SwapChrLo( 0x1000, data ); }
            void Ax5705::Poke_C009(void* p_,Address i_,Data j_) { static_cast<Ax5705*>(p_)->Poke_M_C009(i_,j_); } inline void Ax5705::Poke_M_C009(Address,Data data) { SwapChrHi( 0x1000, data ); }
            void Ax5705::Poke_C00A(void* p_,Address i_,Data j_) { static_cast<Ax5705*>(p_)->Poke_M_C00A(i_,j_); } inline void Ax5705::Poke_M_C00A(Address,Data data) { SwapChrLo( 0x1400, data ); }
            void Ax5705::Poke_C00B(void* p_,Address i_,Data j_) { static_cast<Ax5705*>(p_)->Poke_M_C00B(i_,j_); } inline void Ax5705::Poke_M_C00B(Address,Data data) { SwapChrHi( 0x1400, data ); }
            void Ax5705::Poke_E000(void* p_,Address i_,Data j_) { static_cast<Ax5705*>(p_)->Poke_M_E000(i_,j_); } inline void Ax5705::Poke_M_E000(Address,Data data) { SwapChrLo( 0x1800, data ); }
            void Ax5705::Poke_E001(void* p_,Address i_,Data j_) { static_cast<Ax5705*>(p_)->Poke_M_E001(i_,j_); } inline void Ax5705::Poke_M_E001(Address,Data data) { SwapChrHi( 0x1800, data ); }
            void Ax5705::Poke_E002(void* p_,Address i_,Data j_) { static_cast<Ax5705*>(p_)->Poke_M_E002(i_,j_); } inline void Ax5705::Poke_M_E002(Address,Data data) { SwapChrLo( 0x1C00, data ); }
            void Ax5705::Poke_E003(void* p_,Address i_,Data j_) { static_cast<Ax5705*>(p_)->Poke_M_E003(i_,j_); } inline void Ax5705::Poke_M_E003(Address,Data data) { SwapChrHi( 0x1C00, data ); }
        }
    }
}