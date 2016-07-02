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
#include "NstMapper143.hpp"

namespace Nes
{
    namespace Core
    {
        
        
        
        
        void Mapper143::SubReset(bool)
        {
            for (uint i=0x4100; i < 0x6000; i += 0x200)
                Map( i + 0x00, i + 0xFF, &Mapper143::Peek_4100 );
        }
        
        
        
        
        
        Data Mapper143::Peek_4100(void* p_,Address i_) { return static_cast<Mapper143*>(p_)->Peek_M_4100(i_); } inline Data Mapper143::Peek_M_4100(Address address)
        {
            return (~address & 0x3F) | 0x40;
        }
    }
}
