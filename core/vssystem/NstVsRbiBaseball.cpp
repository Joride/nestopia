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

#include "../NstCpu.hpp"
#include "../NstState.hpp"
#include "../vssystem/NstVsSystem.hpp"
#include "../vssystem/NstVsRbiBaseball.hpp"

namespace Nes
{
    namespace Core
    {
        
        
        
        
        void Cartridge::VsSystem::RbiBaseball::Reset()
        {
            cpu.Map( 0x5E00 ).Set( &RbiBaseball::Peek_5E00 );
            cpu.Map( 0x5E01 ).Set( &RbiBaseball::Peek_5E01 );
            
            counter = 0;
        }
        
        void Cartridge::VsSystem::RbiBaseball::SubSave(State::Saver& state) const
        {
            state.Begin( AsciiId<'R','B','I'>::V ).Write8( counter & 0xFF ).End();
        }
        
        void Cartridge::VsSystem::RbiBaseball::SubLoad(State::Loader& state,const dword chunk)
        {
            if (chunk == AsciiId<'R','B','I'>::V)
                counter = state.Read8();
        }
        
        
        
        
        
        Data Cartridge::VsSystem::RbiBaseball::Peek_5E00(void* p_,Address i_) { return static_cast<Cartridge::VsSystem::RbiBaseball*>(p_)->Peek_M_5E00(i_); } inline Data Cartridge::VsSystem::RbiBaseball::Peek_M_5E00(Address)
        {
            counter = 0;
            return 0x00;
        }
        
        Data Cartridge::VsSystem::RbiBaseball::Peek_5E01(void* p_,Address i_) { return static_cast<Cartridge::VsSystem::RbiBaseball*>(p_)->Peek_M_5E01(i_); } inline Data Cartridge::VsSystem::RbiBaseball::Peek_M_5E01(Address)
        {
            return (counter++ == 0x9) ? 0x6F : 0xB4;
        }
    }
}
