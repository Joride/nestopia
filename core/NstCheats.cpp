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

#include <algorithm>
#include "NstCpu.hpp"
#include "NstCheats.hpp"

namespace Nes
{
    namespace Core
    {
        
        
        
        
        Cheats::Cheats(Cpu& c)
        : cpu(c), frameLocked(false) {}
        
        Cheats::~Cheats()
        {
            ClearCodes();
        }
        
        Result Cheats::SetCode
        (
         const word address,
         const byte data,
         const byte compare,
         const bool useCompare,
         const bool activate
         )
        {
            if (address < 0x2000)
            {
                const LoCode code = {address,data,compare,useCompare};
                
                for (LoCode *it=loCodes.Begin(), *const end=loCodes.End(); ; ++it)
                {
                    if (it == end || it->address > address)
                    {
                        loCodes.Insert( it, code );
                        break;
                    }
                    else if (it->address == address)
                    {
                        if (it->data == code.data && it->useCompare == code.useCompare && (!code.useCompare || it->compare == code.compare))
                        {
                            return RESULT_NOP;
                        }
                        else
                        {
                            *it = code;
                            return RESULT_WARN_DATA_REPLACED;
                        }
                    }
                }
            }
            else
            {
                const HiCode code = {address,data,compare,useCompare,__null};
                
                HiCode* it = hiCodes.Begin();
                
                for (const HiCode* const end=hiCodes.End(); ; ++it)
                {
                    if (it == end || it->address > address)
                    {
                        it = hiCodes.Insert( it, code );
                        break;
                    }
                    else if (it->address == address)
                    {
                        if (it->data == code.data && it->useCompare == code.useCompare && (!code.useCompare || it->compare == code.compare))
                        {
                            return RESULT_NOP;
                        }
                        else
                        {
                            it->data = code.data;
                            it->compare = code.compare;
                            it->useCompare = code.useCompare;
                            return RESULT_WARN_DATA_REPLACED;
                        }
                    }
                }
                
                if (activate)
                    Map( *it );
            }
            
            return RESULT_OK;
        }
        
        Result Cheats::DeleteCode(dword index)
        {
            if (loCodes.Size() > index)
            {
                loCodes.Erase( loCodes.Begin() + index );
                return RESULT_OK;
            }
            else if (hiCodes.Size() > (index -= loCodes.Size()))
            {
                HiCode* const it = hiCodes.Begin() + index;
                cpu.Unlink( it->address, this, &Cheats::Peek_Wizard, &Cheats::Poke_Wizard );
                hiCodes.Erase( it );
                return RESULT_OK;
            }
            else
            {
                return RESULT_ERR_INVALID_PARAM;
            }
        }
        
        void Cheats::Reset()
        {
            loCodes.Defrag();
            hiCodes.Defrag();
            
            for (HiCode *it=hiCodes.Begin(), *const end=hiCodes.End(); it != end; ++it)
                Map( *it );
        }
        
        void Cheats::Map(HiCode& code)
        {
            code.port = cpu.Link( code.address, Cpu::LEVEL_HIGH, this, &Cheats::Peek_Wizard, &Cheats::Poke_Wizard );
        }
        
        void Cheats::ClearCodes()
        {
            loCodes.Destroy();
            
            for (const HiCode *it=hiCodes.Begin(), *const end=hiCodes.End(); it != end; ++it)
                cpu.Unlink( it->address, this, &Cheats::Peek_Wizard, &Cheats::Poke_Wizard );
            
            hiCodes.Destroy();
        }
        
        Result Cheats::GetCode
        (
         dword index,
         ushort* const address,
         uchar* const data,
         uchar* const compare,
         bool* const useCompare
         ) const
        {
            if (loCodes.Size() > index)
            {
                const LoCode* __restrict__ code = loCodes.Begin() + index;
                
                if (address)
                    *address = code->address;
                
                if (data)
                    *data = code->data;
                
                if (compare)
                    *compare = code->compare;
                
                if (useCompare)
                    *useCompare = code->useCompare;
            }
            else if (hiCodes.Size() > (index -= loCodes.Size()))
            {
                const HiCode* __restrict__ code = hiCodes.Begin() + index;
                
                if (address)
                    *address = code->address;
                
                if (data)
                    *data = code->data;
                
                if (compare)
                    *compare = code->compare;
                
                if (useCompare)
                    *useCompare = code->useCompare;
            }
            else
            {
                return RESULT_ERR_INVALID_PARAM;
            }
            
            return RESULT_OK;
        }
        
        
        
        
        
        void Cheats::BeginFrame(bool frameLock)
        {
            frameLocked = frameLock;
            
            if (!frameLock)
            {
                for (const LoCode* __restrict__ it=loCodes.Begin(), *const end=loCodes.End(); it != end; ++it)
                {
                    const uint address = it->address & (Cpu::RAM_SIZE-1);
                    
                    if (!it->useCompare || cpu.GetRam()[address] == it->compare)
                        cpu.GetRam()[address] = it->data;
                }
            }
        }
        
        inline bool Cheats::HiCode::operator < (const Cheats::HiCode& c) const
        {
            return address < c.address;
        }
        
        inline bool Cheats::HiCode::operator < (Address a) const
        {
            return address < a;
        }
        
        inline bool operator < (Address a,const Cheats::HiCode& c)
        {
            return a < c.address;
        }
        
        Data Cheats::Peek_Wizard(void* p_,Address i_) { return static_cast<Cheats*>(p_)->Peek_M_Wizard(i_); } inline Data Cheats::Peek_M_Wizard(Address address)
        {
            (__builtin_expect(!(!!(address >= 0x2000)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstCheats.cpp", 245, "!!(address >= 0x2000)") : (void)0);
            
            const HiCode* const __restrict__ code = std::lower_bound( hiCodes.Begin(), hiCodes.End(), address );
            
            if (!frameLocked)
            {
                if (code->useCompare)
                {
                    const uint data = code->port->Peek( address );
                    
                    if (code->compare != data)
                        return data;
                }
                
                return code->data;
            }
            else
            {
                return code->port->Peek( address );
            }
        }
        
        void Cheats::Poke_Wizard(void* p_,Address i_,Data j_) { static_cast<Cheats*>(p_)->Poke_M_Wizard(i_,j_); } inline void Cheats::Poke_M_Wizard(Address address,Data data)
        {
            (__builtin_expect(!(!!(address >= 0x2000)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstCheats.cpp", 269, "!!(address >= 0x2000)") : (void)0);
            
            return std::lower_bound( hiCodes.Begin(), hiCodes.End(), address )->port->Poke( address, data );
        }
    }
}
