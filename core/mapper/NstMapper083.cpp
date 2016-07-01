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
#include "../NstClock.hpp"
#include "../NstDipSwitches.hpp"
#include "NstMapper083.hpp"

namespace Nes
{
    namespace Core
    {
        
        
        
        
        class Mapper83::CartSwitches : public DipSwitches
        {
            uint region;
            
        public:
            
            CartSwitches()
            : region(0) {}
            
            void SetRegion(uint value)
            {
                region = value ? 1 : 0;
            }
            
            uint GetRegion() const
            {
                return region ? 0xFF : 0x00;
            }
            
        private:
            
            uint GetValue(uint) const
            {
                return region;
            }
            
            void SetValue(uint,uint value)
            {
                region = value;
            }
            
            uint NumDips() const
            {
                return 1;
            }
            
            uint NumValues(uint) const
            {
                return 2;
            }
            
            cstring GetDipName(uint) const
            {
                return "Region";
            }
            
            cstring GetValueName(uint,uint i) const
            {
                return i ? "Asia" : "US";
            }
        };
        
        void Mapper83::Irq::Reset(const bool hard)
        {
            if (hard)
            {
                enabled = false;
                count = 0;
                step = 1;
            }
        }
        
        Mapper83::Mapper83(Context& c)
        :
        Mapper (c,WRAM_DEFAULT|NMT_VERTICAL),
        irq (c.cpu),
        cartSwitches (c.attribute == ATR_REGION_SELECT ? new CartSwitches : __null)
        {}
        
        Mapper83::~Mapper83()
        {
            delete cartSwitches;
        }
        
        Mapper83::Device Mapper83::QueryDevice(DeviceType type)
        {
            if (type == DEVICE_DIP_SWITCHES)
                return cartSwitches;
            else
                return Mapper::QueryDevice( type );
        }
        
        void Mapper83::SubReset(const bool hard)
        {
            irq.Reset( hard, true );
            
            if (hard)
            {
                regs.ctrl = 0;
                
                for (uint i=0; i < 5; ++i)
                    regs.prg[i] = 0;
                
                regs.pr8 = 0;
            }
            
            UpdatePrg();
            
            Map( 0x5000U, &Mapper83::Peek_5000 );
            Map( 0x5100U, 0x51FF, &Mapper83::Peek_5100, &Mapper83::Poke_5100 );
            
            if (!wrk.Source().Writable())
                Map( 0x6000U, 0x7FFFU, &Mapper83::Peek_6000 );
            
            for (uint i=0x8000; i < 0x9000; i += 0x400)
            {
                Map( i+0x000, i+0x0FF, &Mapper83::Poke_8000 );
                Map( i+0x100, i+0x1FF, &Mapper83::Poke_8100 );
                
                for (uint j=i+0x00, n=i+0x100; j < n; j += 0x02)
                {
                    Map( j+0x200, &Mapper83::Poke_8200 );
                    Map( j+0x201, &Mapper83::Poke_8201 );
                }
                
                for (uint j=i+0x00, n=i+0x100; j < n; j += 0x20)
                {
                    Map( j+0x300, j+0x30F, &Mapper83::Poke_8300 );
                    
                    if (chr.Source().Size() == SIZE_512K)
                    {
                        Map( j+0x310, j+0x311, &Mapper83::Poke_8310_1 );
                        Map( j+0x316, j+0x317, &Mapper83::Poke_8310_1 );
                    }
                    else
                    {
                        Map( j+0x310, j+0x317, &Mapper83::Poke_8310_0 );
                    }
                }
            }
            
            Map( 0xB000U, &Mapper83::Poke_8000 );
            Map( 0xB0FFU, &Mapper83::Poke_8000 );
            Map( 0xB100U, &Mapper83::Poke_8000 );
        }
        
        void Mapper83::SubLoad(State::Loader& state)
        {
            while (const dword chunk = state.Begin())
            {
                switch (chunk)
                {
                    case AsciiId<'R','E','G'>::V:
                        
                        regs.ctrl = state.Read8();
                        state.Read( regs.prg );
                        break;
                        
                    case AsciiId<'P','R','8'>::V:
                        
                        regs.pr8 = state.Read8();
                        break;
                        
                    case AsciiId<'I','R','Q'>::V:
                    {
                        State::Loader::Data<3> data( state );
                        
                        irq.unit.enabled = data[0] & 0x1;
                        irq.unit.step = (data[0] & 0x2) ? ~0U : 1U;
                        irq.unit.count = data[1] | data[2] << 8;
                        
                        break;
                    }
                        
                    case AsciiId<'L','A','N'>::V:
                        
                        ((void)0);
                        
                        if (cartSwitches)
                            cartSwitches->SetRegion( state.Read8() & 0x1 );
                        
                        break;
                }
                
                state.End();
            }
        }
        
        void Mapper83::SubSave(State::Saver& state) const
        {
            {
                const byte data[1+5] =
                {
                    regs.ctrl,
                    regs.prg[0],
                    regs.prg[1],
                    regs.prg[2],
                    regs.prg[3],
                    regs.prg[4]
                };
                
                state.Begin( AsciiId<'R','E','G'>::V ).Write( data ).End();
            }
            
            state.Begin( AsciiId<'P','R','8'>::V ).Write8( regs.pr8 ).End();
            
            {
                const byte data[3] =
                {
                    (irq.unit.enabled ? 0x1U : 0x0U) |
                    (irq.unit.step == 1 ? 0x0U : 0x2U),
                    irq.unit.count & 0xFF,
                    irq.unit.count >> 8
                };
                
                state.Begin( AsciiId<'I','R','Q'>::V ).Write( data ).End();
            }
            
            if (cartSwitches)
                state.Begin( AsciiId<'L','A','N'>::V ).Write8( cartSwitches->GetRegion() ? 0x1 : 0x0 ).End();
        }
        
        
        
        
        
        void Mapper83::UpdatePrg()
        {
            if (regs.ctrl & 0x10U)
            {
                prg.SwapBanks<SIZE_8K,0x0000>( regs.prg[0], regs.prg[1] );
                prg.SwapBank<SIZE_8K,0x4000>( regs.prg[2] );
            }
            else
            {
                prg.SwapBank<SIZE_16K,0x0000>( regs.prg[4] & 0x3FU );
                prg.SwapBank<SIZE_16K,0x4000>( (regs.prg[4] & 0x30U) | 0x0F );
            }
        }
        
        Data Mapper83::Peek_5000(void* p_,Address i_) { return static_cast<Mapper83*>(p_)->Peek_M_5000(i_); } inline Data Mapper83::Peek_M_5000(Address)
        {
            return cartSwitches ? cartSwitches->GetRegion() : 0xFF;
        }
        
        Data Mapper83::Peek_5100(void* p_,Address i_) { return static_cast<Mapper83*>(p_)->Peek_M_5100(i_); } inline Data Mapper83::Peek_M_5100(Address)
        {
            return regs.pr8;
        }
        
        void Mapper83::Poke_5100(void* p_,Address i_,Data j_) { static_cast<Mapper83*>(p_)->Poke_M_5100(i_,j_); } inline void Mapper83::Poke_M_5100(Address,Data data)
        {
            regs.pr8 = data;
        }
        
        Data Mapper83::Peek_6000(void* p_,Address i_) { return static_cast<Mapper83*>(p_)->Peek_M_6000(i_); } inline Data Mapper83::Peek_M_6000(Address address)
        {
            ((void)0);
            
            if (regs.ctrl & 0x20U)
            {
                const uint bank = (regs.ctrl & 0x10U) ? 0x1F : regs.prg[3];
                return *prg.Source().Mem( (bank << 13) | (address & 0x1FFF) );
            }
            else
            {
                return address >> 8;
            }
        }
        
        void Mapper83::Poke_8000(void* p_,Address i_,Data j_) { static_cast<Mapper83*>(p_)->Poke_M_8000(i_,j_); } inline void Mapper83::Poke_M_8000(Address,Data data)
        {
            if (regs.prg[4] != data)
            {
                regs.prg[4] = data;
                UpdatePrg();
            }
        }
        
        void Mapper83::Poke_8100(void* p_,Address i_,Data j_) { static_cast<Mapper83*>(p_)->Poke_M_8100(i_,j_); } inline void Mapper83::Poke_M_8100(Address,Data data)
        {
            const uint diff = data ^ regs.ctrl;
            regs.ctrl = data;
            
            if (diff & 0x10)
                UpdatePrg();
            
            if (diff & 0xC0)
            {
                irq.Update();
                irq.unit.step = (data & 0x40) ? ~0U : 1U;
            }
            
            if (diff & 0x03)
                SetMirroringVH01( data );
        }
        
        void Mapper83::Poke_8200(void* p_,Address i_,Data j_) { static_cast<Mapper83*>(p_)->Poke_M_8200(i_,j_); } inline void Mapper83::Poke_M_8200(Address,Data data)
        {
            irq.Update();
            irq.unit.count = (irq.unit.count & 0xFF00) | data;
            irq.ClearIRQ();
        }
        
        void Mapper83::Poke_8201(void* p_,Address i_,Data j_) { static_cast<Mapper83*>(p_)->Poke_M_8201(i_,j_); } inline void Mapper83::Poke_M_8201(Address,Data data)
        {
            irq.Update();
            irq.unit.count = (irq.unit.count & 0x00FF) | (data << 8);
            irq.unit.enabled = regs.ctrl & 0x80U;
            irq.ClearIRQ();
        }
        
        void Mapper83::Poke_8310_0(void* p_,Address i_,Data j_) { static_cast<Mapper83*>(p_)->Poke_M_8310_0(i_,j_); } inline void Mapper83::Poke_M_8310_0(Address address,Data data)
        {
            ppu.Update();
            chr.SwapBank<SIZE_1K>( (address & 0x7) << 10, (regs.prg[4] << 4 & 0x300U) | data );
        }
        
        void Mapper83::Poke_8310_1(void* p_,Address i_,Data j_) { static_cast<Mapper83*>(p_)->Poke_M_8310_1(i_,j_); } inline void Mapper83::Poke_M_8310_1(Address address,Data data)
        {
            ppu.Update();
            chr.SwapBank<SIZE_2K>( (address & 0x3) << 11, data );
        }
        
        void Mapper83::Poke_8300(void* p_,Address i_,Data j_) { static_cast<Mapper83*>(p_)->Poke_M_8300(i_,j_); } inline void Mapper83::Poke_M_8300(Address address,Data data)
        {
            data &= 0x1F;
            
            if (regs.prg[address & 0x3] != data)
            {
                regs.prg[address & 0x3] = data;
                UpdatePrg();
            }
        }
        
        bool Mapper83::Irq::Clock()
        {
            if (enabled && count)
            {
                count = (count + step) & 0xFFFF;
                
                if (!count)
                {
                    enabled = false;
                    return true;
                }
            }
            
            return false;
        }
        
        void Mapper83::Sync(Event event,Input::Controllers*)
        {
            if (event == EVENT_END_FRAME)
                irq.VSync();
        }
    }
}
