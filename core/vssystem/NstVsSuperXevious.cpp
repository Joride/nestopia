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
#include "../vssystem/NstVsSystem.hpp"
#include "../vssystem/NstVsSuperXevious.hpp"

namespace Nes
{
    namespace Core
    {
        
        
        
        
        void Cartridge::VsSystem::SuperXevious::Reset()
        {
            cpu.Map( 0x54FF ).Set( &SuperXevious::Peek_54FF );
            cpu.Map( 0x5567 ).Set( &SuperXevious::Peek_5567 );
            cpu.Map( 0x5678 ).Set( &SuperXevious::Peek_5678 );
            cpu.Map( 0x578F ).Set( &SuperXevious::Peek_578F );
            
            protection = 0;
        }
        
        
        
        
        
        Data Cartridge::VsSystem::SuperXevious::Peek_54FF(void* p_,Address i_) { return static_cast<Cartridge::VsSystem::SuperXevious*>(p_)->Peek_M_54FF(i_); } inline Data Cartridge::VsSystem::SuperXevious::Peek_M_54FF(Address) { return 0x05; }
        Data Cartridge::VsSystem::SuperXevious::Peek_5567(void* p_,Address i_) { return static_cast<Cartridge::VsSystem::SuperXevious*>(p_)->Peek_M_5567(i_); } inline Data Cartridge::VsSystem::SuperXevious::Peek_M_5567(Address) { return (protection ^= 0x1) ? 0x37 : 0x3E; }
        Data Cartridge::VsSystem::SuperXevious::Peek_5678(void* p_,Address i_) { return static_cast<Cartridge::VsSystem::SuperXevious*>(p_)->Peek_M_5678(i_); } inline Data Cartridge::VsSystem::SuperXevious::Peek_M_5678(Address) { return protection ? 0x00 : 0x01; }
        Data Cartridge::VsSystem::SuperXevious::Peek_578F(void* p_,Address i_) { return static_cast<Cartridge::VsSystem::SuperXevious*>(p_)->Peek_M_578F(i_); } inline Data Cartridge::VsSystem::SuperXevious::Peek_M_578F(Address) { return protection ? 0xD1 : 0x89; }
    }
}
