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
#include "NstBrdVrc4.hpp"

namespace Nes
{
    namespace Core
    {
        namespace Boards
        {
            
            
            
            
            Vrc4::Vrc4(Context& c,const Type t,dword settings)
            :
            Mapper (c,settings),
            irq (c.cpu),
            prgSwap (0),
            type (t)
            {}
            
            void Vrc4::BaseIrq::Reset(bool)
            {
                ctrl = 0;
                count[0] = 0;
                count[1] = 0;
                latch = 0;
            }
            
            void Vrc4::SubReset(const bool hard)
            {
                if (hard)
                    prgSwap = 0;
                
                irq.Reset( hard, hard ? false : irq.Connected() );
                
                Map( 0x8000U, 0x8FFFU, &Vrc4::Poke_8 );
                Map( 0xA000U, 0xAFFFU, PRG_SWAP_8K_1 );
                
                switch (type)
                {
                    case TYPE_VRC4_352889:
                        
                        for (dword i=0x9000; i <= 0xFFFF; ++i)
                        {
                            switch ((((i >> 1) | (i >> 6)) & 0x3) | (i & 0xF000))
                            {
                                case 0x9000:
                                case 0x9001: Map( i, NMT_SWAP_VH01 ); break;
                                case 0x9002:
                                case 0x9003: Map( i, &Vrc4::Poke_9 ); break;
                                case 0xB000: Map( i, &Vrc4::Poke_B0 ); break;
                                case 0xB001: Map( i, &Vrc4::Poke_B1 ); break;
                                case 0xB002: Map( i, &Vrc4::Poke_B2 ); break;
                                case 0xB003: Map( i, &Vrc4::Poke_B3 ); break;
                                case 0xC000: Map( i, &Vrc4::Poke_C0 ); break;
                                case 0xC001: Map( i, &Vrc4::Poke_C1 ); break;
                                case 0xC002: Map( i, &Vrc4::Poke_C2 ); break;
                                case 0xC003: Map( i, &Vrc4::Poke_C3 ); break;
                                case 0xD000: Map( i, &Vrc4::Poke_D0 ); break;
                                case 0xD001: Map( i, &Vrc4::Poke_D1 ); break;
                                case 0xD002: Map( i, &Vrc4::Poke_D2 ); break;
                                case 0xD003: Map( i, &Vrc4::Poke_D3 ); break;
                                case 0xE000: Map( i, &Vrc4::Poke_E0 ); break;
                                case 0xE001: Map( i, &Vrc4::Poke_E1 ); break;
                                case 0xE002: Map( i, &Vrc4::Poke_E2 ); break;
                                case 0xE003: Map( i, &Vrc4::Poke_E3 ); break;
                                case 0xF000: Map( i, &Vrc4::Poke_F0 ); break;
                                case 0xF001: Map( i, &Vrc4::Poke_F1 ); break;
                                case 0xF002: Map( i, &Vrc4::Poke_F2 ); break;
                                case 0xF003: Map( i, &Vrc4::Poke_F3 ); break;
                            }
                        }
                        break;
                        
                    case TYPE_VRC4_352396:
                        
                        for (dword i=0x9000; i <= 0xFFFF; ++i)
                        {
                            switch ((((i >> 0) | (i >> 2) | (i >> 4) | (i >> 6)) & 0x3) | (i & 0xF000))
                            {
                                case 0x9000:
                                case 0x9001: Map( i, NMT_SWAP_VH01 ); break;
                                case 0x9002:
                                case 0x9003: Map( i, &Vrc4::Poke_9 ); break;
                                case 0xB000: Map( i, &Vrc4::Poke_B0 ); break;
                                case 0xB001: Map( i, &Vrc4::Poke_B1 ); break;
                                case 0xB002: Map( i, &Vrc4::Poke_B2 ); break;
                                case 0xB003: Map( i, &Vrc4::Poke_B3 ); break;
                                case 0xC000: Map( i, &Vrc4::Poke_C0 ); break;
                                case 0xC001: Map( i, &Vrc4::Poke_C1 ); break;
                                case 0xC002: Map( i, &Vrc4::Poke_C2 ); break;
                                case 0xC003: Map( i, &Vrc4::Poke_C3 ); break;
                                case 0xD000: Map( i, &Vrc4::Poke_D0 ); break;
                                case 0xD001: Map( i, &Vrc4::Poke_D1 ); break;
                                case 0xD002: Map( i, &Vrc4::Poke_D2 ); break;
                                case 0xD003: Map( i, &Vrc4::Poke_D3 ); break;
                                case 0xE000: Map( i, &Vrc4::Poke_E0 ); break;
                                case 0xE001: Map( i, &Vrc4::Poke_E1 ); break;
                                case 0xE002: Map( i, &Vrc4::Poke_E2 ); break;
                                case 0xE003: Map( i, &Vrc4::Poke_E3 ); break;
                                case 0xF000: Map( i, &Vrc4::Poke_F0 ); break;
                                case 0xF001: Map( i, &Vrc4::Poke_F1 ); break;
                                case 0xF002: Map( i, &Vrc4::Poke_F2 ); break;
                                case 0xF003: Map( i, &Vrc4::Poke_F3 ); break;
                            }
                        }
                        break;
                        
                    case TYPE_VRC4_351406:
                        
                        for (dword i=0x9000; i <= 0xFFFF; ++i)
                        {
                            switch ((i | (i >> 2 & 0x3)) & 0xF003)
                            {
                                case 0x9000:
                                case 0x9002: Map( i, NMT_SWAP_VH01 ); break;
                                case 0x9001:
                                case 0x9003: Map( i, &Vrc4::Poke_9 ); break;
                                case 0xB000: Map( i, &Vrc4::Poke_B0 ); break;
                                case 0xB001: Map( i, &Vrc4::Poke_B2 ); break;
                                case 0xB002: Map( i, &Vrc4::Poke_B1 ); break;
                                case 0xB003: Map( i, &Vrc4::Poke_B3 ); break;
                                case 0xC000: Map( i, &Vrc4::Poke_C0 ); break;
                                case 0xC001: Map( i, &Vrc4::Poke_C2 ); break;
                                case 0xC002: Map( i, &Vrc4::Poke_C1 ); break;
                                case 0xC003: Map( i, &Vrc4::Poke_C3 ); break;
                                case 0xD000: Map( i, &Vrc4::Poke_D0 ); break;
                                case 0xD001: Map( i, &Vrc4::Poke_D2 ); break;
                                case 0xD002: Map( i, &Vrc4::Poke_D1 ); break;
                                case 0xD003: Map( i, &Vrc4::Poke_D3 ); break;
                                case 0xE000: Map( i, &Vrc4::Poke_E0 ); break;
                                case 0xE001: Map( i, &Vrc4::Poke_E2 ); break;
                                case 0xE002: Map( i, &Vrc4::Poke_E1 ); break;
                                case 0xE003: Map( i, &Vrc4::Poke_E3 ); break;
                                case 0xF000: Map( i, &Vrc4::Poke_F0 ); break;
                                case 0xF001: Map( i, &Vrc4::Poke_F2 ); break;
                                case 0xF002: Map( i, &Vrc4::Poke_F1 ); break;
                                case 0xF003: Map( i, &Vrc4::Poke_F3 ); break;
                            }
                        }
                        break;
                }
            }
            
            void Vrc4::BaseLoad(State::Loader& state,const dword baseChunk)
            {
                ((void)0);
                
                if (baseChunk == AsciiId<'V','R','4'>::V)
                {
                    while (const dword chunk = state.Begin())
                    {
                        switch (chunk)
                        {
                            case AsciiId<'R','E','G'>::V:
                                
                                prgSwap = state.Read8() & 0x2;
                                break;
                                
                            case AsciiId<'I','R','Q'>::V:
                                
                                irq.LoadState( state );
                                break;
                        }
                        
                        state.End();
                    }
                }
            }
            
            void Vrc4::BaseSave(State::Saver& state) const
            {
                state.Begin( AsciiId<'V','R','4'>::V );
                
                state.Begin( AsciiId<'R','E','G'>::V ).Write8( prgSwap ).End();
                irq.SaveState( state, AsciiId<'I','R','Q'>::V );
                
                state.End();
            }
            
            void Vrc4::Irq::LoadState(State::Loader& state)
            {
                State::Loader::Data<5> data( state );
                
                unit.ctrl = data[0] & (BaseIrq::ENABLE_1|BaseIrq::NO_PPU_SYNC);
                Connect( data[0] & BaseIrq::ENABLE_0 );
                unit.latch = data[1];
                unit.count[0] = ((340) < (data[2] | data[3] << 8) ? (340) : (data[2] | data[3] << 8));
                unit.count[1] = data[4];
            }
            
            void Vrc4::Irq::SaveState(State::Saver& state,const dword chunk) const
            {
                const byte data[5] =
                {
                    unit.ctrl | (Connected() ? BaseIrq::ENABLE_0 : 0),
                    unit.latch,
                    unit.count[0] & 0xFF,
                    unit.count[0] >> 8,
                    unit.count[1]
                };
                
                state.Begin( chunk ).Write( data ).End();
            }
            
            
            
            
            
            void Vrc4::Poke_8(void* p_,Address i_,Data j_) { static_cast<Vrc4*>(p_)->Poke_M_8(i_,j_); } inline void Vrc4::Poke_M_8(Address,Data data)
            {
                prg.SwapBank<SIZE_8K>( prgSwap << 13, data );
            }
            
            void Vrc4::Poke_9(void* p_,Address i_,Data j_) { static_cast<Vrc4*>(p_)->Poke_M_9(i_,j_); } inline void Vrc4::Poke_M_9(Address,Data data)
            {
                data &= 0x2;
                
                if (prgSwap != data)
                {
                    prgSwap = data;
                    
                    prg.SwapBanks<SIZE_8K,0x0000>
                    (
                     prg.GetBank<SIZE_8K,0x4000>(),
                     prg.GetBank<SIZE_8K,0x0000>()
                     );
                }
            }
            
            template<uint OFFSET>
            void Vrc4::SwapChr(const uint address,const uint data) const
            {
                ppu.Update();
                chr.SwapBank<SIZE_1K>( address, (chr.GetBank<SIZE_1K>(address) & (0xF0U >> OFFSET)) | ((data & 0xF) << OFFSET) );
            }
            
            void Vrc4::Poke_B0(void* p_,Address i_,Data j_) { static_cast<Vrc4*>(p_)->Poke_M_B0(i_,j_); } inline void Vrc4::Poke_M_B0(Address,Data data) { SwapChr<0>( 0x0000, data ); }
            void Vrc4::Poke_B1(void* p_,Address i_,Data j_) { static_cast<Vrc4*>(p_)->Poke_M_B1(i_,j_); } inline void Vrc4::Poke_M_B1(Address,Data data) { SwapChr<4>( 0x0000, data ); }
            void Vrc4::Poke_B2(void* p_,Address i_,Data j_) { static_cast<Vrc4*>(p_)->Poke_M_B2(i_,j_); } inline void Vrc4::Poke_M_B2(Address,Data data) { SwapChr<0>( 0x0400, data ); }
            void Vrc4::Poke_B3(void* p_,Address i_,Data j_) { static_cast<Vrc4*>(p_)->Poke_M_B3(i_,j_); } inline void Vrc4::Poke_M_B3(Address,Data data) { SwapChr<4>( 0x0400, data ); }
            void Vrc4::Poke_C0(void* p_,Address i_,Data j_) { static_cast<Vrc4*>(p_)->Poke_M_C0(i_,j_); } inline void Vrc4::Poke_M_C0(Address,Data data) { SwapChr<0>( 0x0800, data ); }
            void Vrc4::Poke_C1(void* p_,Address i_,Data j_) { static_cast<Vrc4*>(p_)->Poke_M_C1(i_,j_); } inline void Vrc4::Poke_M_C1(Address,Data data) { SwapChr<4>( 0x0800, data ); }
            void Vrc4::Poke_C2(void* p_,Address i_,Data j_) { static_cast<Vrc4*>(p_)->Poke_M_C2(i_,j_); } inline void Vrc4::Poke_M_C2(Address,Data data) { SwapChr<0>( 0x0C00, data ); }
            void Vrc4::Poke_C3(void* p_,Address i_,Data j_) { static_cast<Vrc4*>(p_)->Poke_M_C3(i_,j_); } inline void Vrc4::Poke_M_C3(Address,Data data) { SwapChr<4>( 0x0C00, data ); }
            void Vrc4::Poke_D0(void* p_,Address i_,Data j_) { static_cast<Vrc4*>(p_)->Poke_M_D0(i_,j_); } inline void Vrc4::Poke_M_D0(Address,Data data) { SwapChr<0>( 0x1000, data ); }
            void Vrc4::Poke_D1(void* p_,Address i_,Data j_) { static_cast<Vrc4*>(p_)->Poke_M_D1(i_,j_); } inline void Vrc4::Poke_M_D1(Address,Data data) { SwapChr<4>( 0x1000, data ); }
            void Vrc4::Poke_D2(void* p_,Address i_,Data j_) { static_cast<Vrc4*>(p_)->Poke_M_D2(i_,j_); } inline void Vrc4::Poke_M_D2(Address,Data data) { SwapChr<0>( 0x1400, data ); }
            void Vrc4::Poke_D3(void* p_,Address i_,Data j_) { static_cast<Vrc4*>(p_)->Poke_M_D3(i_,j_); } inline void Vrc4::Poke_M_D3(Address,Data data) { SwapChr<4>( 0x1400, data ); }
            void Vrc4::Poke_E0(void* p_,Address i_,Data j_) { static_cast<Vrc4*>(p_)->Poke_M_E0(i_,j_); } inline void Vrc4::Poke_M_E0(Address,Data data) { SwapChr<0>( 0x1800, data ); }
            void Vrc4::Poke_E1(void* p_,Address i_,Data j_) { static_cast<Vrc4*>(p_)->Poke_M_E1(i_,j_); } inline void Vrc4::Poke_M_E1(Address,Data data) { SwapChr<4>( 0x1800, data ); }
            void Vrc4::Poke_E2(void* p_,Address i_,Data j_) { static_cast<Vrc4*>(p_)->Poke_M_E2(i_,j_); } inline void Vrc4::Poke_M_E2(Address,Data data) { SwapChr<0>( 0x1C00, data ); }
            void Vrc4::Poke_E3(void* p_,Address i_,Data j_) { static_cast<Vrc4*>(p_)->Poke_M_E3(i_,j_); } inline void Vrc4::Poke_M_E3(Address,Data data) { SwapChr<4>( 0x1C00, data ); }
            
            void Vrc4::Irq::WriteLatch0(const uint data)
            {
                Update();
                unit.latch = (unit.latch & 0xF0) | (data << 0 & 0x0F);
            }
            
            void Vrc4::Irq::WriteLatch1(const uint data)
            {
                Update();
                unit.latch = (unit.latch & 0x0F) | (data << 4 & 0xF0);
            }
            
            void Vrc4::Irq::Toggle(const uint data)
            {
                Update();
                unit.ctrl = data & (BaseIrq::ENABLE_1|BaseIrq::NO_PPU_SYNC);
                
                if (Connect( data & BaseIrq::ENABLE_0 ))
                {
                    unit.count[0] = 0;
                    unit.count[1] = unit.latch;
                }
                
                ClearIRQ();
            }
            
            void Vrc4::Irq::Toggle()
            {
                Update();
                Connect( unit.ctrl & BaseIrq::ENABLE_1 );
                ClearIRQ();
            }
            
            void Vrc4::Poke_F0(void* p_,Address i_,Data j_) { static_cast<Vrc4*>(p_)->Poke_M_F0(i_,j_); } inline void Vrc4::Poke_M_F0(Address,Data data)
            {
                irq.WriteLatch0( data );
            }
            
            void Vrc4::Poke_F1(void* p_,Address i_,Data j_) { static_cast<Vrc4*>(p_)->Poke_M_F1(i_,j_); } inline void Vrc4::Poke_M_F1(Address,Data data)
            {
                irq.WriteLatch1( data );
            }
            
            void Vrc4::Poke_F2(void* p_,Address i_,Data j_) { static_cast<Vrc4*>(p_)->Poke_M_F2(i_,j_); } inline void Vrc4::Poke_M_F2(Address,Data data)
            {
                irq.Toggle( data );
            }
            
            void Vrc4::Poke_F3(void* p_,Address i_,Data j_) { static_cast<Vrc4*>(p_)->Poke_M_F3(i_,j_); } inline void Vrc4::Poke_M_F3(Address,Data)
            {
                irq.Toggle();
            }
            
            bool Vrc4::BaseIrq::Clock()
            {
                if (!(ctrl & NO_PPU_SYNC))
                {
                    if (count[0] < 341-3)
                    {
                        count[0] += 3;
                        return false;
                    }
                    
                    count[0] -= 341-3;
                }
                
                if (count[1] != 0xFF)
                {
                    count[1]++;
                    return false;
                }
                
                count[1] = latch;
                
                return true;
            }
            
            void Vrc4::Sync(Event event,Input::Controllers*)
            {
                if (event == EVENT_END_FRAME)
                    irq.VSync();
            }
        }
    }
}
