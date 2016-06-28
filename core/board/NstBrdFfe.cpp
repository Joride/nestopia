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
#include "NstBrdFfe.hpp"

namespace Nes
{
    namespace Core
    {
        namespace Boards
        {
            
            
            
            
            Ffe::Ffe(Context& c,const Type t)
            :
            Mapper (c,t == F4_XXX ? CRAM_32K : WRAM_8K),
            irq (t == F3_XXX ? __null : new ClockUnits::M2<Irq>(c.cpu,t == F8_XXX_1 ? 0xFFFF : 0xCFFF)),
            type (t)
            {
            }
            
            Ffe::~Ffe()
            {
                delete irq;
            }
            
            void Ffe::Irq::Reset(const bool hard)
            {
                if (hard)
                {
                    count = 0;
                    enabled = false;
                }
            }
            
            void Ffe::SubReset(const bool hard)
            {
                if (hard)
                    mode = 0;
                
                Map( 0x42FEU, &Ffe::Poke_42FE );
                Map( 0x42FFU, &Ffe::Poke_42FF );
                
                if (irq)
                {
                    irq->Reset( hard, hard || irq->Connected() );
                    
                    Map( 0x4501U, &Ffe::Poke_4501 );
                    Map( 0x4502U, &Ffe::Poke_4502 );
                    Map( 0x4503U, &Ffe::Poke_4503 );
                }
                
                switch (type)
                {
                    case F3_XXX:
                        
                        Map( 0x8000U, 0xFFFFU, &Ffe::Poke_Prg_F3 );
                        
                        if (hard)
                            prg.SwapBank<SIZE_32K,0x0000>(0);
                        
                        break;
                        
                    case F4_XXX:
                        
                        Map( 0x8000U, 0xFFFFU, &Ffe::Poke_Prg_F4 );
                        
                        if (hard)
                            prg.SwapBank<SIZE_16K,0x4000>(7);
                        
                        break;
                        
                    case F8_XXX_0:
                    case F8_XXX_1:
                        
                        Map( 0x4504U, PRG_SWAP_8K_0 );
                        Map( 0x4505U, PRG_SWAP_8K_1 );
                        Map( 0x4506U, PRG_SWAP_8K_2 );
                        Map( 0x4507U, PRG_SWAP_8K_3 );
                        Map( 0x4510U, CHR_SWAP_1K_0 );
                        Map( 0x4511U, CHR_SWAP_1K_1 );
                        Map( 0x4512U, CHR_SWAP_1K_2 );
                        Map( 0x4513U, CHR_SWAP_1K_3 );
                        Map( 0x4514U, CHR_SWAP_1K_4 );
                        Map( 0x4515U, CHR_SWAP_1K_5 );
                        Map( 0x4516U, CHR_SWAP_1K_6 );
                        Map( 0x4517U, CHR_SWAP_1K_7 );
                        
                        break;
                }
            }
            
            void Ffe::BaseLoad(State::Loader& state,const dword baseChunk)
            {
                ((void)0);
                
                if (baseChunk == AsciiId<'F','F','E'>::V)
                {
                    while (const dword chunk = state.Begin())
                    {
                        switch (chunk)
                        {
                            case AsciiId<'R','E','G'>::V:
                                
                                ((void)0);
                                
                                if (type == F4_XXX)
                                    mode = state.Read8() & 0x1;
                                
                                break;
                                
                            case AsciiId<'I','R','Q'>::V:
                                
                                ((void)0);
                                
                                if (irq)
                                {
                                    State::Loader::Data<3> data( state );
                                    
                                    irq->unit.enabled = data[0] & 0x1;
                                    irq->unit.count = data[1] | data[2] << 8;
                                }
                                break;
                        }
                        
                        state.End();
                    }
                }
            }
            
            void Ffe::BaseSave(State::Saver& state) const
            {
                state.Begin( AsciiId<'F','F','E'>::V );
                
                if (type == F4_XXX)
                    state.Begin( AsciiId<'R','E','G'>::V ).Write8( mode ).End();
                
                if (irq)
                {
                    const byte data[3] =
                    {
                        irq->unit.enabled != false,
                        irq->unit.count & 0xFF,
                        irq->unit.count >> 8
                    };
                    
                    state.Begin( AsciiId<'I','R','Q'>::V ).Write( data ).End();
                }
                
                state.End();
            }
            
            
            
            
            
            bool Ffe::Irq::Clock()
            {
                if (enabled && count++ == clock)
                {
                    count = 0;
                    enabled = false;
                    return true;
                }
                
                return false;
            }
            
            void Ffe::Poke_42FE(void* p_,Address i_,Data j_) { static_cast<Ffe*>(p_)->Poke_M_42FE(i_,j_); } inline void Ffe::Poke_M_42FE(Address,Data data)
            {
                mode = data >> 7 ^ 0x1;
                ppu.SetMirroring( (data & 0x10) ? Ppu::NMT_ONE : Ppu::NMT_ZERO );
            }
            
            void Ffe::Poke_42FF(void* p_,Address i_,Data j_) { static_cast<Ffe*>(p_)->Poke_M_42FF(i_,j_); } inline void Ffe::Poke_M_42FF(Address,Data data)
            {
                ppu.SetMirroring( (data & 0x10) ? Ppu::NMT_HORIZONTAL : Ppu::NMT_VERTICAL );
            }
            
            void Ffe::Poke_4501(void* p_,Address i_,Data j_) { static_cast<Ffe*>(p_)->Poke_M_4501(i_,j_); } inline void Ffe::Poke_M_4501(Address,Data data)
            {
                irq->Update();
                irq->unit.enabled = data & 0x1;
                irq->ClearIRQ();
            }
            
            void Ffe::Poke_4502(void* p_,Address i_,Data j_) { static_cast<Ffe*>(p_)->Poke_M_4502(i_,j_); } inline void Ffe::Poke_M_4502(Address,Data data)
            {
                irq->Update();
                irq->unit.count = (irq->unit.count & 0xFF00) | data;
            }
            
            void Ffe::Poke_4503(void* p_,Address i_,Data j_) { static_cast<Ffe*>(p_)->Poke_M_4503(i_,j_); } inline void Ffe::Poke_M_4503(Address,Data data)
            {
                irq->Update();
                irq->unit.count = (irq->unit.count & 0x00FF) | (data << 8);
                irq->unit.enabled = true;
                irq->ClearIRQ();
            }
            
            void Ffe::Poke_Prg_F3(void* p_,Address i_,Data j_) { static_cast<Ffe*>(p_)->Poke_M_Prg_F3(i_,j_); } inline void Ffe::Poke_M_Prg_F3(Address,Data data)
            {
                ppu.Update();
                prg.SwapBank<SIZE_16K,0x0000>( data >> 3 );
                chr.SwapBank<SIZE_8K,0x0000>( data & 0x7 );
            }
            
            void Ffe::Poke_Prg_F4(void* p_,Address i_,Data j_) { static_cast<Ffe*>(p_)->Poke_M_Prg_F4(i_,j_); } inline void Ffe::Poke_M_Prg_F4(Address,Data data)
            {
                ppu.Update();
                
                if (mode || chr.Source(0).Writable())
                {
                    prg.SwapBank<SIZE_16K,0x0000>( data >> 2 );
                    data &= 0x3;
                }
                
                chr.Source( mode ).SwapBank<SIZE_8K,0x0000>( data );
            }
            
            void Ffe::Sync(Event event,Input::Controllers*)
            {
                if (event == EVENT_END_FRAME)
                {
                    if (irq)
                        irq->VSync();
                }
            }
        }
    }
}
