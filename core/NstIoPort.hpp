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

#ifndef NST_IO_PORT_H
#define NST_IO_PORT_H

#ifdef NST_PRAGMA_ONCE
#pragma once
#endif

#include <stdio.h>

namespace Nes
{
	namespace Core
	{
		namespace Io
		{
		#ifdef NST_FASTDELEGATE

			class Port
			{
				class Component {};

				typedef Data (NST_FASTCALL Component::*Reader)(Address);
				typedef void (NST_FASTCALL Component::*Writer)(Address,Data);

				Component* component;
				Reader reader;
				Writer writer;

				NST_COMPILE_ASSERT
				(
					sizeof( Reader ) <= sizeof( Data (*)(void*,Address)      ) &&
					sizeof( Writer ) <= sizeof( Data (*)(void*,Address,Data) )
				);

			public:

				Port() {}

				template<typename T>
				Port(T* c,Data (NST_FASTCALL T::*r)(Address),void (NST_FASTCALL T::*w)(Address,Data))
				:
				component ( reinterpret_cast<Component*>(c) ),
				reader    ( reinterpret_cast<Reader>(r)    ),
				writer    ( reinterpret_cast<Writer>(w)    )
				{
					NST_COMPILE_ASSERT( sizeof(reader) == sizeof(r) && sizeof(writer) == sizeof(w) );
				}

				template<typename T>
				void Set(T* c,Data (NST_FASTCALL T::*r)(Address),void (NST_FASTCALL T::*w)(Address,Data))
				{
					NST_COMPILE_ASSERT( sizeof(reader) == sizeof(r) && sizeof(writer) == sizeof(w) );

					component = reinterpret_cast<Component*>(c);
					reader    = reinterpret_cast<Reader>(r);
					writer    = reinterpret_cast<Writer>(w);
				}

				template<typename T>
				void Set(Data (NST_FASTCALL T::*r)(Address))
				{
					NST_COMPILE_ASSERT( sizeof(reader) == sizeof(r) );

					reader = reinterpret_cast<Reader>(r);
				}

				template<typename T>
				void Set(void (NST_FASTCALL T::*w)(Address,Data))
				{
					NST_COMPILE_ASSERT( sizeof(writer) == sizeof(w) );

					writer = reinterpret_cast<Writer>(w);
				}

				template<typename T>
				void Set(Data (NST_FASTCALL T::*r)(Address),void (NST_FASTCALL T::*w)(Address,Data))
				{
					NST_COMPILE_ASSERT( sizeof(reader) == sizeof(r) && sizeof(writer) == sizeof(w) );

					reader = reinterpret_cast<Reader>(r);
					writer = reinterpret_cast<Writer>(w);
				}

				uint Peek(Address address) const
				{
					return (*component.*reader)( address );
				}

				void Poke(Address address,Data data) const
				{
					(*component.*writer)( address, data );
				}

				bool operator == (const Port& p) const
				{
					return component == p.component && reader == p.reader && writer == p.writer;
				}
			};

			#define NES_DECL_PEEK(a_) Data NST_FASTCALL Peek_##a_(Address)
			#define NES_DECL_POKE(a_) void NST_FASTCALL Poke_##a_(Address,Data)

			#define NES_PEEK(o_,a_) Data NST_FASTCALL o_::Peek_##a_(Address)
			#define NES_PEEK_A(o_,a_) Data NST_FASTCALL o_::Peek_##a_(Address address)

			#define NES_POKE(o_,a_) void NST_FASTCALL o_::Poke_##a_(Address,Data)
			#define NES_POKE_A(o_,a_) void NST_FASTCALL o_::Poke_##a_(Address address,Data)
			#define NES_POKE_D(o_,a_) void NST_FASTCALL o_::Poke_##a_(Address,Data data)
			#define NES_POKE_AD(o_,a_) void NST_FASTCALL o_::Poke_##a_(Address address,Data data)

			#define NES_DO_POKE(a_,p_,d_) Poke_##a_(p_,d_)
			#define NES_DO_PEEK(a_,p_) Peek_##a_(p_)

		#else

			class Port
			{
                
				typedef void* Component;
                
                // typedef uint (*ReaderFunctionPointer)(void*,uint); // function pointer named 'Reader', taking arguemnts void* and uint
                typedef uint (*ReaderFunctionPointer)(void*,uint);
                
                
				typedef void (*Writer)(Component,Address,Data);

				
				ReaderFunctionPointer readerFP;
				Writer writer;

			public:
                Component component;
                
                // this function added by joride: to bypass the reader-function
                // pointers and not have values change upon read
                uint8_t valueAtIndex(uint16_t index)
                {
                    uint8_t * values = (uint8_t *) component;
                    return values[index];
                }
                
				Port()
                {
                    
//                    printf("Port()\n");
                    
                }

				Port(Component c,ReaderFunctionPointer r,Writer w)
				:
				component ( c ),
				readerFP    ( r ),
				writer    ( w )
				{
                    printf("Port(c,r,w)\n");
                }

                void Set(void* c,ReaderFunctionPointer r,Writer w)
				{
//                    printf("Port.set(c, r, w)\n");
                    component = c;
                    readerFP    = r;
                    writer    = w;
				}

				void Set(ReaderFunctionPointer r)
				{
                    printf("Port.set(r)\n");
                    readerFP = r;
				}

				void Set(Writer w)
				{
                    printf("Port.set(w)\n");
                    writer = w;
				}

				void Set(ReaderFunctionPointer r,Writer w)
				{
                    printf("Port.set(r,w)\n");
					readerFP = r;
					writer = w;
                    
				}

				Data Peek(Address address) const
				{
//                    printf("START: readerFP( component, address )\n");
                    // Nes::Core::Mapper::Peek_M_Prg_E(component, address);
                    Data value = readerFP( component, address );
//                    printf("END: readerFP( component, address )\n");
                    return value;
				}

				void Poke(Address address,Data data) const
				{
					writer( component, address, data );
				}

				bool operator == (const Port& p) const
				{
					return component == p.component && readerFP == p.readerFP && writer == p.writer;
				}
			};

			#define NES_DECL_PEEK(a_)                                                        \
                                                                                             \
				NST_SINGLE_CALL Data NST_FASTCALL Peek_M_##a_(Address);                      \
				static Data NST_REGCALL Peek_##a_(void*,Address)

			#define NES_DECL_POKE(a_)                                                        \
                                                                                             \
				NST_SINGLE_CALL void NST_FASTCALL Poke_M_##a_(Address,Data);                 \
				static void NST_REGCALL Poke_##a_(void*,Address,Data)

			#define NES_PEEK(o_,a_)                                                          \
                                                                                             \
				Data NST_REGCALL o_::Peek_##a_(void* p_,Address i_)                          \
				{                                                                            \
					return static_cast<o_*>(p_)->Peek_M_##a_(i_);                            \
				}                                                                            \
                                                                                             \
				NST_SINGLE_CALL Data NST_FASTCALL o_::Peek_M_##a_(Address)

			#define NES_PEEK_A(o_,a_)                                                        \
                                                                                             \
				Data NST_REGCALL o_::Peek_##a_(void* p_,Address i_)                          \
				{                                                                            \
					return static_cast<o_*>(p_)->Peek_M_##a_(i_);                            \
				}                                                                            \
                                                                                             \
				NST_SINGLE_CALL Data NST_FASTCALL o_::Peek_M_##a_(Address address)

			#define NES_POKE(o_,a_)                                                          \
                                                                                             \
				void NST_REGCALL o_::Poke_##a_(void* p_,Address i_,Data j_)                  \
				{                                                                            \
					static_cast<o_*>(p_)->Poke_M_##a_(i_,j_);                                \
				}                                                                            \
                                                                                             \
				NST_SINGLE_CALL void NST_FASTCALL o_::Poke_M_##a_(Address,Data)

			#define NES_POKE_A(o_,a_)                                                        \
                                                                                             \
				void NST_REGCALL o_::Poke_##a_(void* p_,Address i_,Data j_)                  \
				{                                                                            \
					static_cast<o_*>(p_)->Poke_M_##a_(i_,j_);                                \
				}                                                                            \
                                                                                             \
				NST_SINGLE_CALL void NST_FASTCALL o_::Poke_M_##a_(Address address,Data)

			#define NES_POKE_D(o_,a_)                                                        \
                                                                                             \
				void NST_REGCALL o_::Poke_##a_(void* p_,Address i_,Data j_)                  \
				{                                                                            \
					static_cast<o_*>(p_)->Poke_M_##a_(i_,j_);                                \
				}                                                                            \
                                                                                             \
				NST_SINGLE_CALL void NST_FASTCALL o_::Poke_M_##a_(Address,Data data)

			#define NES_POKE_AD(o_,a_)                                                       \
                                                                                             \
				void NST_REGCALL o_::Poke_##a_(void* p_,Address i_,Data j_)                  \
				{                                                                            \
					static_cast<o_*>(p_)->Poke_M_##a_(i_,j_);                                \
				}                                                                            \
                                                                                             \
				NST_SINGLE_CALL void NST_FASTCALL o_::Poke_M_##a_(Address address,Data data)

			#define NES_DO_POKE(a_,p_,d_) Poke_##a_(this,p_,d_)
			#define NES_DO_PEEK(a_,p_)    Peek_##a_(this,p_)

		#endif
		}
	}
}

#endif
