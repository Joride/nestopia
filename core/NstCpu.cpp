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

#include <cstring>
#include "NstCpu.hpp"
#include "NstHook.hpp"
#include "NstState.hpp"
#include "api/NstApiUser.hpp"

#import "NESTracer/NESTracer-Bridge.h"


// use this to turn off the print statements (with them, emulation is slooooowww)
//#define printf(x, ...)

#define TraceFunctionName NESTracerCurrentOpcodeName(__FUNCTION__)

// use this to turn the printf statements to printToFile statements
//#define printf(args...) do {\
//    FILE *out = fopen("/Users/Jorrit/iOS/EmuOutputCompare/output/nestopia.txt", "a");\
//    fprintf(out, args);\
//    fclose(out);\
//    } while(0)


namespace Nes
{
    namespace Core
    {
        dword Cpu::logged = 0;
        
        void (Cpu::*const Cpu::opcodes[0x100])() =
        {
            &Cpu::op0x00, &Cpu::op0x01, &Cpu::op0x02, &Cpu::op0x03,
            &Cpu::op0x04, &Cpu::op0x05, &Cpu::op0x06, &Cpu::op0x07,
            &Cpu::op0x08, &Cpu::op0x09, &Cpu::op0x0A, &Cpu::op0x0B,
            &Cpu::op0x0C, &Cpu::op0x0D, &Cpu::op0x0E, &Cpu::op0x0F,
            &Cpu::op0x10, &Cpu::op0x11, &Cpu::op0x12, &Cpu::op0x13,
            &Cpu::op0x14, &Cpu::op0x15, &Cpu::op0x16, &Cpu::op0x17,
            &Cpu::op0x18, &Cpu::op0x19, &Cpu::op0x1A, &Cpu::op0x1B,
            &Cpu::op0x1C, &Cpu::op0x1D, &Cpu::op0x1E, &Cpu::op0x1F,
            &Cpu::op0x20, &Cpu::op0x21, &Cpu::op0x22, &Cpu::op0x23,
            &Cpu::op0x24, &Cpu::op0x25, &Cpu::op0x26, &Cpu::op0x27,
            &Cpu::op0x28, &Cpu::op0x29, &Cpu::op0x2A, &Cpu::op0x2B,
            &Cpu::op0x2C, &Cpu::op0x2D, &Cpu::op0x2E, &Cpu::op0x2F,
            &Cpu::op0x30, &Cpu::op0x31, &Cpu::op0x32, &Cpu::op0x33,
            &Cpu::op0x34, &Cpu::op0x35, &Cpu::op0x36, &Cpu::op0x37,
            &Cpu::op0x38, &Cpu::op0x39, &Cpu::op0x3A, &Cpu::op0x3B,
            &Cpu::op0x3C, &Cpu::op0x3D, &Cpu::op0x3E, &Cpu::op0x3F,
            &Cpu::op0x40, &Cpu::op0x41, &Cpu::op0x42, &Cpu::op0x43,
            &Cpu::op0x44, &Cpu::op0x45, &Cpu::op0x46, &Cpu::op0x47,
            &Cpu::op0x48, &Cpu::op0x49, &Cpu::op0x4A, &Cpu::op0x4B,
            &Cpu::op0x4C, &Cpu::op0x4D, &Cpu::op0x4E, &Cpu::op0x4F,
            &Cpu::op0x50, &Cpu::op0x51, &Cpu::op0x52, &Cpu::op0x53,
            &Cpu::op0x54, &Cpu::op0x55, &Cpu::op0x56, &Cpu::op0x57,
            &Cpu::op0x58, &Cpu::op0x59, &Cpu::op0x5A, &Cpu::op0x5B,
            &Cpu::op0x5C, &Cpu::op0x5D, &Cpu::op0x5E, &Cpu::op0x5F,
            &Cpu::op0x60, &Cpu::op0x61, &Cpu::op0x62, &Cpu::op0x63,
            &Cpu::op0x64, &Cpu::op0x65, &Cpu::op0x66, &Cpu::op0x67,
            &Cpu::op0x68, &Cpu::op0x69, &Cpu::op0x6A, &Cpu::op0x6B,
            &Cpu::op0x6C, &Cpu::op0x6D, &Cpu::op0x6E, &Cpu::op0x6F,
            &Cpu::op0x70, &Cpu::op0x71, &Cpu::op0x72, &Cpu::op0x73,
            &Cpu::op0x74, &Cpu::op0x75, &Cpu::op0x76, &Cpu::op0x77,
            &Cpu::op0x78, &Cpu::op0x79, &Cpu::op0x7A, &Cpu::op0x7B,
            &Cpu::op0x7C, &Cpu::op0x7D, &Cpu::op0x7E, &Cpu::op0x7F,
            &Cpu::op0x80, &Cpu::op0x81, &Cpu::op0x82, &Cpu::op0x83,
            &Cpu::op0x84, &Cpu::op0x85, &Cpu::op0x86, &Cpu::op0x87,
            &Cpu::op0x88, &Cpu::op0x89, &Cpu::op0x8A, &Cpu::op0x8B,
            &Cpu::op0x8C, &Cpu::op0x8D, &Cpu::op0x8E, &Cpu::op0x8F,
            &Cpu::op0x90, &Cpu::op0x91, &Cpu::op0x92, &Cpu::op0x93,
            &Cpu::op0x94, &Cpu::op0x95, &Cpu::op0x96, &Cpu::op0x97,
            &Cpu::op0x98, &Cpu::op0x99, &Cpu::op0x9A, &Cpu::op0x9B,
            &Cpu::op0x9C, &Cpu::op0x9D, &Cpu::op0x9E, &Cpu::op0x9F,
            &Cpu::op0xA0, &Cpu::op0xA1, &Cpu::op0xA2, &Cpu::op0xA3,
            &Cpu::op0xA4, &Cpu::op0xA5, &Cpu::op0xA6, &Cpu::op0xA7,
            &Cpu::op0xA8, &Cpu::op0xA9, &Cpu::op0xAA, &Cpu::op0xAB,
            &Cpu::op0xAC, &Cpu::op0xAD, &Cpu::op0xAE, &Cpu::op0xAF,
            &Cpu::op0xB0, &Cpu::op0xB1, &Cpu::op0xB2, &Cpu::op0xB3,
            &Cpu::op0xB4, &Cpu::op0xB5, &Cpu::op0xB6, &Cpu::op0xB7,
            &Cpu::op0xB8, &Cpu::op0xB9, &Cpu::op0xBA, &Cpu::op0xBB,
            &Cpu::op0xBC, &Cpu::op0xBD, &Cpu::op0xBE, &Cpu::op0xBF,
            &Cpu::op0xC0, &Cpu::op0xC1, &Cpu::op0xC2, &Cpu::op0xC3,
            &Cpu::op0xC4, &Cpu::op0xC5, &Cpu::op0xC6, &Cpu::op0xC7,
            &Cpu::op0xC8, &Cpu::op0xC9, &Cpu::op0xCA, &Cpu::op0xCB,
            &Cpu::op0xCC, &Cpu::op0xCD, &Cpu::op0xCE, &Cpu::op0xCF,
            &Cpu::op0xD0, &Cpu::op0xD1, &Cpu::op0xD2, &Cpu::op0xD3,
            &Cpu::op0xD4, &Cpu::op0xD5, &Cpu::op0xD6, &Cpu::op0xD7,
            &Cpu::op0xD8, &Cpu::op0xD9, &Cpu::op0xDA, &Cpu::op0xDB,
            &Cpu::op0xDC, &Cpu::op0xDD, &Cpu::op0xDE, &Cpu::op0xDF,
            &Cpu::op0xE0, &Cpu::op0xE1, &Cpu::op0xE2, &Cpu::op0xE3,
            &Cpu::op0xE4, &Cpu::op0xE5, &Cpu::op0xE6, &Cpu::op0xE7,
            &Cpu::op0xE8, &Cpu::op0xE9, &Cpu::op0xEA, &Cpu::op0xEB,
            &Cpu::op0xEC, &Cpu::op0xED, &Cpu::op0xEE, &Cpu::op0xEF,
            &Cpu::op0xF0, &Cpu::op0xF1, &Cpu::op0xF2, &Cpu::op0xF3,
            &Cpu::op0xF4, &Cpu::op0xF5, &Cpu::op0xF6, &Cpu::op0xF7,
            &Cpu::op0xF8, &Cpu::op0xF9, &Cpu::op0xFA, &Cpu::op0xFB,
            &Cpu::op0xFC, &Cpu::op0xFD, &Cpu::op0xFE, &Cpu::op0xFF
        };
        Cpu::Cpu()
        :
        region ( Region::NTSC ),
        apu ( *this ),
        map ( this, &Cpu::Peek_Overflow, &Cpu::Poke_Overflow )
        {
            NESTracerSetFileName("nestopia");
            Reset( false, false );
            
//            std::string a = ListValuesByAdrres();
//            printf("%s", a.c_str());
            
            cycles.SetRegion( GetRegion() );
        }
        
        
       void Cpu::PowerOff()
        {
            Reset( false, true );
        }
        
        void Cpu::Reset(bool hard)
        {
            Reset( true, hard );
        }
        
        void Cpu::Reset(const bool on,const bool hard)
        {
            if (!on || hard)
            {
                ram.Reset();
                
                a = 0x00;
                x = 0x00;
                y = 0x00;
                sp = 0xFD;
                
                flags.nz = 0U ^ 1U;
                flags.c = 0;
                flags.v = 0;
                flags.d = 0;
            }
            else
            {
                sp = (sp - 3) & 0xFF;
            }
            
            pc = RESET_VECTOR;
            flags.i = Flags::I;
            jammed = false;
            ticks = 0;
            logged = 0;
            
            cycles.count = 0;
            cycles.round = 0;
            cycles.frame = (region == Region::NTSC ? Clocks::RP2C02_HVSYNC : Clocks::RP2C07_HVSYNC);
            
            interrupt.Reset();
            hooks.Clear();
            linker.Clear();
            
            if (on)
            {
                
                // zeropage, stack and ram
                map( 0x0000, 0x07FF ).Set(&ram,
                                          &Cpu::Ram::Peek_Ram_0,
                                          &Cpu::Ram::Poke_Ram_0 );
                
                // mirros 0x0000 - 0x07FF
                map( 0x0800, 0x0FFF ).Set(&ram,
                                          &Cpu::Ram::Peek_Ram_1,
                                          &Cpu::Ram::Poke_Ram_1 );
                // mirros 0x0000 - 0x07FF
                map( 0x1000, 0x17FF ).Set(&ram,
                                          &Cpu::Ram::Peek_Ram_2,
                                          &Cpu::Ram::Poke_Ram_2 );
                // mirros 0x0000 - 0x07FF,
                map( 0x1800, 0x1FFF ).Set(&ram,
                                          &Cpu::Ram::Peek_Ram_3,
                                          &Cpu::Ram::Poke_Ram_3 );
                
                // io-registers, mirrors of io-registers, io registers,
                // expansion ROM, SRAM, PRG-ROM (lower bank), PRG-ROM (upper bank)
                map( 0x2000, 0xFFFF ).Set(this,
                                          &Cpu::Peek_Nop,
                                          &Cpu::Poke_Nop );
                
                // reset-vector
                map( 0xFFFC ).Set(this,
                                  &Cpu::Peek_Jam_1,
                                  &Cpu::Poke_Nop );
                
                map( 0xFFFD ).Set(this,
                                  &Cpu::Peek_Jam_2,
                                  &Cpu::Poke_Nop );
                
                apu.Reset( hard );
            }
            else
            {
                map( 0x0000, 0xFFFF ).Set( this, &Cpu::Peek_Nop, &Cpu::Poke_Nop );
                
                if (hard)
                    apu.PowerOff();
            }
            
            cyclesSubtracted = 0; // added by joride for debugging purposes
//            printf("AFTER RESET:");
//            printCPU();
            
//            std::string a = ListValuesByAdrres();
//            printf("%s", a.c_str());
            
        }
        
        uint8_t* Cpu::MemoryBuffer()
        {
            uint16_t bufferSize = 0xFFFF;
            uint8_t * buffer = (uint8_t *) malloc(sizeof(uint8_t) * bufferSize);
            for (uint16_t index = 0; index < 0xFFFF; index++)
            {
                Io::Port aPort = map.ports[index];
                uint8_t value1 = aPort.valueAtIndex(index);
                uint8_t value2 = aPort.valueAtIndex(index);
                uint8_t value3 = aPort.valueAtIndex(index);
                uint8_t value4 = aPort.valueAtIndex(index);
                
//                uint8_t value1 = map.Peek8(index);
//                uint8_t value2 = map.Peek8(index);

                
                // these asserts are here to make sure, that reading
                // a values several times does not change it
                // (i.e. to make sure the reader function pointers are
                // bypassed)
                assert(value1 == value2);
                assert(value1 == value3);
                assert(value1 == value4);
                
                buffer[index] = value1;
            }
            return buffer;
        }
        
        std::string Cpu::ListValuesByAdrres()
        {
            std::string string = "Address\tValue\n";
            for (uint16_t index = 0; index < 0xFFFF; index++)
            {
//                printf("0x%04X\t0x%02X\n", index, map.Peek8(index));
                
                char buffer [16];
                sprintf(buffer, "0x%04X\t0x%02X\n", index, map.Peek8(index));
                std::string toAppend = std::string(buffer);
                string = string + toAppend;
            }
            return string;
        }
        
        void Cpu::SetRegion(const Region::Type r)
        {
            if (region != r)
            {
                region = r;
                cycles.SetRegion( r );
                interrupt.SetRegion( r );
                
                ticks = ticks / uint(r == Region::NTSC ? Clocks::RP2A07_CC : Clocks::RP2A03_CC) *
                uint(r == Region::NTSC ? Clocks::RP2A03_CC : Clocks::RP2A07_CC);
                
                apu.UpdateRegion();
            }
        }
        
        void Cpu::AddHook(const Hook& hook)
        {
            hooks.Add( hook );
        }
        
        void Cpu::RemoveHook(const Hook& hook)
        {
            hooks.Remove( hook );
        }
        
        Cycle Cpu::ClockConvert(Cycle clock,Region::Type region)
        {
            return
            (
             clock / (region == Region::NTSC ? Clocks::RP2A07_CC : Clocks::RP2A03_CC) *
             (region == Region::NTSC ? Clocks::RP2A03_CC : Clocks::RP2A07_CC)
             );
        }
        
        
        
        
        
        bool Cpu::IsOddCycle() const
        {
            return uint((ticks + cycles.count) % cycles.clock[1]);
        }
        
         uint Cpu::Cycles::NmiEdge() const
        {
            return clock[0] + clock[0] / 2;
        }
        
         uint Cpu::Cycles::IrqEdge() const
        {
            return clock[1];
        }
        
        
        
        
        
        void Cpu::SaveState(State::Saver& state,const dword cpuChunk,const dword apuChunk) const
        {
            state.Begin( cpuChunk );
            
            {
                const byte data[7] =
                {
                    pc & 0xFF,
                    pc >> 8,
                    sp,
                    a,
                    x,
                    y,
                    flags.Pack()
                };
                
                state.Begin( AsciiId<'R','E','G'>::V ).Write( data ).End();
            }
            
            state.Begin( AsciiId<'R','A','M'>::V ).Compress( ram.mem ).End();
            
            {
                const byte data[5] =
                {
                    ((interrupt.nmiClock != CYCLE_MAX) ? 0x01U : 0x00U) |
                    ((interrupt.low & IRQ_FRAME) ? 0x02U : 0x00U) |
                    ((interrupt.low & IRQ_DMC) ? 0x04U : 0x00U) |
                    ((interrupt.low & IRQ_EXT) ? 0x08U : 0x00U) |
                    (jammed ? 0x40U : 0x00U) |
                    (region == Region::PAL ? 0x80U : 0x00U),
                    cycles.count & 0xFF,
                    cycles.count >> 8,
                    (interrupt.nmiClock != CYCLE_MAX) ? interrupt.nmiClock+1 : 0,
                    (interrupt.irqClock != CYCLE_MAX) ? interrupt.irqClock+1 : 0
                };
                
                state.Begin( AsciiId<'F','R','M'>::V ).Write( data ).End();
            }
            
            state.Begin( AsciiId<'C','L','K'>::V ).Write64( ticks ).End();
            
            state.End();
            
            apu.SaveState( state, apuChunk );
        }
        
        void Cpu::LoadState(State::Loader& state,const dword cpuChunk,const dword apuChunk,const dword baseChunk)
        {
            if (baseChunk == cpuChunk)
            {
                Region::Type stateRegion = GetRegion();
                ticks = 0;
                
                while (const dword chunk = state.Begin())
                {
                    switch (chunk)
                    {
                        case AsciiId<'R','E','G'>::V:
                        {
                            State::Loader::Data<7> data( state );
                            
                            pc = data[0] | data[1] << 8;
                            sp = data[2];
                            a = data[3];
                            x = data[4];
                            y = data[5];
                            
                            flags.Unpack( data[6] );
                            break;
                        }
                            
                        case AsciiId<'R','A','M'>::V:
                            
                            state.Uncompress( ram.mem );
                            break;
                            
                        case AsciiId<'F','R','M'>::V:
                        {
                            State::Loader::Data<5> data( state );
                            
                            stateRegion = (data[0] & 0x80) ? Region::PAL : Region::NTSC;
                            
                            interrupt.nmiClock = CYCLE_MAX;
                            interrupt.irqClock = CYCLE_MAX;
                            interrupt.low = 0;
                            
                            if (data[0] & (0x2|0x4|0x8))
                            {
                                interrupt.low =
                                (
                                 ((data[0] & 0x2) ? IRQ_FRAME : 0) |
                                 ((data[0] & 0x4) ? IRQ_DMC : 0) |
                                 ((data[0] & 0x8) ? IRQ_EXT : 0)
                                 );
                                
                                if (!flags.i)
                                    interrupt.irqClock = data[4] ? data[4] - 1 : 0;
                            }
                            
                            cycles.count = data[1] | data[2] << 8;
                            
                            if (data[0] & 0x1)
                                interrupt.nmiClock = data[3] ? data[3] - 1 : cycles.NmiEdge();
                            
                            jammed = data[0] >> 6 & 0x1;
                            
                            if (jammed)
                                interrupt.Reset();
                            
                            break;
                        }
                            
                        case AsciiId<'C','L','K'>::V:
                            
                            ticks = state.Read64();
                            break;
                    }
                    
                    state.End();
                }
                
                const Region::Type actualRegion = GetRegion();
                
                if (stateRegion != actualRegion)
                {
                    cycles.SetRegion( actualRegion );
                    interrupt.SetRegion( actualRegion );
                    
                    ticks = ticks / uint(actualRegion == Region::NTSC ? Clocks::RP2A07_CC : Clocks::RP2A03_CC) *
                    uint(actualRegion == Region::NTSC ? Clocks::RP2A03_CC : Clocks::RP2A07_CC);
                }
                
                if (cycles.count >= cycles.frame)
                    cycles.count = 0;
                
                ticks -= (ticks + cycles.count) % cycles.clock[0];
            }
            else if (baseChunk == apuChunk)
            {
                apu.LoadState( state );
            }
        }
        
        void Cpu::NotifyOp(const char (&code)[4],const dword which)
        {
            if (!(logged & which))
            {
                logged |= which;
                Api::User::eventCallback( Api::User::EVENT_CPU_UNOFFICIAL_OPCODE, code );
            }
        }
        
        Cpu::Hooks::Hooks()
        : hooks(new Hook [2]), size(0), capacity(2) {}
        
        Cpu::Hooks::~Hooks()
        {
            delete [] hooks;
        }
        
        void Cpu::Hooks::Clear()
        {
            size = 0;
        }
        
        void Cpu::Hooks::Add(const Hook& hook)
        {
            for (uint i=0, n=size; i < n; ++i)
            {
                if (hooks[i] == hook)
                    return;
            }
            
            if (size == capacity)
            {
                Hook* const __restrict__ next = new Hook [capacity+1];
                ++capacity;
                
                for (uint i=0, n=size; i < n; ++i)
                    next[i] = hooks[i];
                
                delete [] hooks;
                hooks = next;
            }
            
            hooks[size++] = hook;
        }
        
        void Cpu::Hooks::Remove(const Hook& hook)
        {
            for (uint i=0, n=size; i < n; ++i)
            {
                if (hooks[i] == hook)
                {
                    while (++i < n)
                        hooks[i-1] = hooks[i];
                    
                    --size;
                    return;
                }
            }
        }
        
        
        
        
        
         uint Cpu::Hooks::Size() const
        {
            return size;
        }
        
         const Hook* Cpu::Hooks::Ptr() const
        {
            return hooks;
        }
        
        
        
        
        
        Cpu::Linker::Chain::Chain(const Port& p,uint a,uint l)
        : Port(p), address(a), level(l) {}
        
        Cpu::Linker::Linker()
        : chain(__null) {}
        
        Cpu::Linker::~Linker()
        {
            Clear();
        }
        
        void Cpu::Linker::Clear()
        {
            if (Chain* next = chain)
            {
                chain = __null;
                
                do
                {
                    Chain* tmp = next->next;
                    delete next;
                    next = tmp;
                }
                while (next);
            }
        }
        
        const Io::Port* Cpu::Linker::Add(const Address address,const uint level,const Io::Port& port,IoMap& map)
        {
            (__builtin_expect(!(!!(level)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstCpu.cpp", 503, "!!(level)") : (void)0);
            
            Chain* const entry = new Chain( port, address, level );
            
            for (Chain *it=chain, *prev=__null; it; prev=it, it=it->next)
            {
                if (it->address == address)
                {
                    (__builtin_expect(!(!!(it->next && it->next->address == address)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstCpu.cpp", 511, "!!(it->next && it->next->address == address)") : (void)0);
                    
                    if (level > it->level)
                    {
                        entry->next = it;
                        
                        if (prev)
                            prev->next = entry;
                        else
                            chain = entry;
                        
                        map(address) = port;
                        
                        return it;
                    }
                    else for (;;)
                    {
                        it = it->next;
                        
                        (__builtin_expect(!(!!(level != it->level)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstCpu.cpp", 530, "!!(level != it->level)") : (void)0);
                        
                        if (level > it->level)
                        {
                            const Chain tmp( *it );
                            *it = *entry;
                            it->next = entry;
                            *entry = tmp;
                            
                            return entry;
                        }
                    }
                }
            }
            
            entry->next = new Chain( map[address], address );
            entry->next->next = __null;
            
            map(address) = port;
            
            if (Chain* it = chain)
            {
                while (it->next)
                    it = it->next;
                
                it->next = entry;
            }
            else
            {
                chain = entry;
            }
            
            return entry->next;
        }
        
        void Cpu::Linker::Remove(const Address address,const Io::Port& port,IoMap& map)
        {
            for (Chain *it=chain, *prev=__null; it; prev=it, it=it->next)
            {
                if (it->address == address && port == *it)
                {
                    const Chain* const next = it->next;
                    
                    (__builtin_expect(!(!!(it->level && next && next->address == address)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstCpu.cpp", 573, "!!(it->level && next && next->address == address)") : (void)0);
                    
                    *it = *next;
                    delete next;
                    
                    if (map(address) == port)
                        map(address) = *it;
                    
                    if (it->level == 0)
                    {
                        if (prev == __null)
                        {
                            it = it->next;
                            delete chain;
                            chain = it;
                        }
                        else if (prev->address != address)
                        {
                            prev->next = it->next;
                            delete it;
                        }
                    }
                    
                    break;
                }
            }
        }
        
        void Cpu::Cycles::SetRegion(Region::Type region)
        {
            count = ClockConvert( count, region );
            
            for (uint i=0; i < 8; ++i)
                clock[i] = (i+1) * (region == Region::NTSC ? Clocks::RP2A03_CC : Clocks::RP2A07_CC);
        }
        
        void Cpu::Cycles::NextRound(const Cycle next)
        {
            if (round > next)
                round = next;
        }
        
        uint Cpu::Flags::Pack() const
        {
            return
            (
             ((nz | nz >> 1) & N) |
             ((nz & 0xFF) ? 0 : Z) |
             c |
             (v ? V : 0) |
             i |
             d |
             R
             );
        }
        
        void Cpu::Flags::Unpack(const uint f)
        {
            nz = (~f & Z) | ((f & N) << 1);
            c = f & C;
            v = f & V;
            i = f & I;
            d = f & D;
        }
        
        
        
        
        
        void Cpu::Interrupt::Reset()
        {
            nmiClock = CYCLE_MAX;
            irqClock = CYCLE_MAX;
            low = 0;
        }
        
        void Cpu::Interrupt::SetRegion(Region::Type region)
        {
            if (nmiClock != CYCLE_MAX)
                nmiClock = ClockConvert( nmiClock, region );
            
            if (irqClock != CYCLE_MAX)
                irqClock = ClockConvert( nmiClock, region );
        }
        
        
        
        
        
         uint Cpu::Interrupt::Clock(const Cycle cycle)
        {
            if (cycle >= nmiClock)
            {
                ((void)0);
                
                nmiClock = CYCLE_MAX;
                irqClock = CYCLE_MAX;
                return NMI_VECTOR;
            }
            else if (cycle >= irqClock)
            {
                irqClock = CYCLE_MAX;
                return IRQ_VECTOR;
            }
            else
            {
                return 0;
            }
        }
        
         void Cpu::Interrupt::EndFrame(const Cycle frameCycles)
        {
            if (nmiClock != CYCLE_MAX)
            {
                ((void)0);
                nmiClock -= frameCycles;
            }
            
            if (irqClock != CYCLE_MAX)
                irqClock = irqClock > frameCycles ? irqClock - frameCycles : 0;
        }
        
        
        
        
        
        template<typename T,typename U>
        Cpu::IoMap::IoMap(Cpu* cpu,T peek,U poke)
        : Io::Map<SIZE_64K>( cpu, peek, poke )
        {
            
        }
        
        
         uint Cpu::IoMap::Peek8(const uint address) const
        {

            return ports[address].Peek( address );
        }
        
         uint Cpu::IoMap::Peek16(const uint address) const
        {
//            uint peekedAddress = ports[address].Peek( address );
//            uint peekedAddressPlusOne = ports[address + 1].Peek( address + 1 );
            
            Io::Port firstPort = ports[address];
            Io::Port secondPort = ports[address + 1];
            
            uint loByte = firstPort.Peek(address);
            uint hiByte = secondPort.Peek(address + 1);
            
            uint value = loByte | hiByte << 8;
            
            
            return ports[address].Peek( address ) | ports[address + 1].Peek( address + 1 ) << 8;
        }
        
         void Cpu::IoMap::Poke8(const uint address,const uint data) const
        {
            Io::Port destPort = ports[address];
            destPort.Poke( address, data );
//            ports[address].Poke( address, data );
        }
        
        
        void Cpu::Ram::Reset()
        {
            std::memset( mem + 0x000, 0xFF, 0x3F0 );
            std::memset( mem + 0x3F0, 0x00, 0x010 );
            std::memset( mem + 0x400, 0xFF, 0x1F0 );
            std::memset( mem + 0x5F0, 0x00, 0x010 );
            std::memset( mem + 0x600, 0xFF, 0x200 );
            
            mem[0x08] = 0xF7;
            mem[0x09] = 0xEF;
            mem[0x0A] = 0xDF;
            mem[0x0F] = 0xBF;
        }
        
        Data Cpu::Ram::Peek_Ram_0(void* p_,Address i_)
        {
            
            return static_cast<Cpu::Ram*>(p_)->Peek_M_Ram_0(i_);
        }
        
        Data Cpu::Ram::Peek_M_Ram_0(Address address)
        {
            printf("YAY:%s\n", __FUNCTION__);
            return mem[address - 0x0000];
        }
        
        Data Cpu::Ram::Peek_Ram_1(void* p_,Address i_)
        {
            printf("YAY:%s\n", __FUNCTION__);
            return static_cast<Cpu::Ram*>(p_)->Peek_M_Ram_1(i_);
        }
        
        Data Cpu::Ram::Peek_M_Ram_1(Address address)
        {
            printf("YAY:%s\n", __FUNCTION__);
            return mem[address - 0x0800];
        }
        
        Data Cpu::Ram::Peek_Ram_2(void* p_,Address i_)
        {
            printf("YAY:%s\n", __FUNCTION__);
            return static_cast<Cpu::Ram*>(p_)->Peek_M_Ram_2(i_);
        }
        Data Cpu::Ram::Peek_M_Ram_2(Address address)
        {
            printf("YAY:%s\n", __FUNCTION__);
            return mem[address - 0x1000];
        }
        
        Data Cpu::Ram::Peek_Ram_3(void* p_,Address i_)
        {
            printf("YAY:%s\n", __FUNCTION__);
            return static_cast<Cpu::Ram*>(p_)->Peek_M_Ram_3(i_);
        }
        
        Data Cpu::Ram::Peek_M_Ram_3(Address address)
        {
            printf("YAY:%s\n", __FUNCTION__);
            return mem[address - 0x1800];
        }
        
        void Cpu::Ram::Poke_Ram_0(void* p_,Address i_,Data j_)
        {
            static_cast<Cpu::Ram*>(p_)->Poke_M_Ram_0(i_,j_);
        }
        
        void Cpu::Ram::Poke_M_Ram_0(Address address,Data data)
        {
            mem[address - 0x0000] = data;
        }
        
        void Cpu::Ram::Poke_Ram_1(void* p_,Address i_,Data j_)
        {
            static_cast<Cpu::Ram*>(p_)->Poke_M_Ram_1(i_,j_);
        }
        
        void Cpu::Ram::Poke_M_Ram_1(Address address,Data data)
        {
            mem[address - 0x0800] = data;
        }
        
        void Cpu::Ram::Poke_Ram_2(void* p_,Address i_,Data j_)
        {
            static_cast<Cpu::Ram*>(p_)->Poke_M_Ram_2(i_,j_);
        }
        
        void Cpu::Ram::Poke_M_Ram_2(Address address,Data data)
        {
            mem[address - 0x1000] = data;
        }
        
        void Cpu::Ram::Poke_Ram_3(void* p_,Address i_,Data j_)
        {
            static_cast<Cpu::Ram*>(p_)->Poke_M_Ram_3(i_,j_);
        }
        
        void Cpu::Ram::Poke_M_Ram_3(Address address,Data data)
        {
            mem[address - 0x1800] = data;
        }
        
        Data Cpu::Peek_Nop(void* p_,Address i_)
        {
            printf("YAY:%s\n", __FUNCTION__);
            return static_cast<Cpu*>(p_)->Peek_M_Nop(i_);
        }
        
        Data Cpu::Peek_M_Nop(Address address)
        {
            printf("YAY:%s\n", __FUNCTION__);
            return address >> 8;
        }
        
        void Cpu::Poke_Nop(void* p_,Address i_,Data j_)
        {
            printf("YAY:%s\n", __FUNCTION__);
            static_cast<Cpu*>(p_)->Poke_M_Nop(i_,j_);
        }
        
        void Cpu::Poke_M_Nop(Address,Data)
        {
            printf("YAY:%s\n", __FUNCTION__);
        }
        
        Data Cpu::Peek_Overflow(void* p_,Address i_)
        {
            printf("YAY:%s\n", __FUNCTION__);
            return static_cast<Cpu*>(p_)->Peek_M_Overflow(i_);
        }
        
        Data Cpu::Peek_M_Overflow(Address address)
        {
            printf("YAY:%s\n", __FUNCTION__);
            pc &= 0xFFFF;
            return ram.mem[address & 0x7FF];
        }
        
        void Cpu::Poke_Overflow(void* p_,Address i_,Data j_)
        {
            static_cast<Cpu*>(p_)->Poke_M_Overflow(i_,j_);
            
        }
         void Cpu::Poke_M_Overflow(Address address,Data data)
        {
            pc &= 0xFFFF;
            ram.mem[address & 0x7FF] = data;
        }
        
        Data Cpu::Peek_Jam_1(void* p_,Address i_)
        {
            return static_cast<Cpu*>(p_)->Peek_M_Jam_1(i_);
        }
        
        Data Cpu::Peek_M_Jam_1(Address)
        {
            pc = (pc - 1) & 0xFFFF;
            return 0xFC;
        }
        
        Data Cpu::Peek_Jam_2(void* p_,Address i_)
        {
            return static_cast<Cpu*>(p_)->Peek_M_Jam_2(i_);
        }
        
        Data Cpu::Peek_M_Jam_2(Address)
        {
            return 0xFF;
        }
        
        uint Cpu::FetchZpg16(const uint address) const
        {
            return ram.mem[address & 0xFF] | uint(ram.mem[(address+1) & 0xFF]) << 8;
        }
        
        uint Cpu::FetchPc8()
        {
            const uint data = map.Peek8( pc );
            ++pc;
            return data;
        }
        
        uint Cpu::FetchPc16()
        {
            const uint data = map.Peek16( pc );
            pc += 2;
            return data;
        }
        
#pragma mark - Addressing
        uint Cpu::Imm_R()
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeImmediate);
            NESTracerSetOpcodeArgumentsAddress(pc);
            
            const uint data = FetchPc8();
            cycles.count += cycles.clock[1];
            
            NESTracerSetOpcodeArguments( (uint8_t[]) {data}, 1, 0);
            return data;
        }
        
        uint Cpu::Abs_R()
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeAbsolute);
            uint data = FetchPc16();
            cycles.count += cycles.clock[2];
            uint address = data;
            
//            if (address == 0x2002)
//            { printf("reading 0x2002"); }
            
            // figuring out how to start up the PPU:
            // what data is returned here causes the CPU.NZ flag to be set
            data = map.Peek8( data );
            
            NESTracerSetOpcodeArguments( (uint8_t[]) {data}, 1, 0);
            NESTracerSetOpcodeArgumentsAddress(address);
            
            cycles.count += cycles.clock[0];
            
//            printf("%s  address: 0x%04X\tvalue: 0x%02X\n", __FUNCTION__, address, data);
            
            if (data == 0x90)
            {
                // to reset the printed count in printCPU() to zero, to sync
                // up with Nestament
                // (cyclesSubtracted + cycles.count - (7 * 12)) / 12
                cyclesSubtracted = -((int64_t)cycles.count) + 84;
                
                printf("going out of the loop\n");
            }
            
            return data;
        }
        
        uint Cpu::Abs_RW(uint& data)
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeAbsolute);
            const uint address = FetchPc16();
            cycles.count += cycles.clock[2];
            
            data = map.Peek8( address );
            cycles.count += cycles.clock[0];
            
            map.Poke8( address, data );
            cycles.count += cycles.clock[0];
            
            NESTracerSetOpcodeArgumentsAddress(address);
            return address;
        }
        
         uint Cpu::Abs_W()
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeAbsolute);
            const uint address = FetchPc16();
            cycles.count += cycles.clock[2];
            
            NESTracerSetOpcodeArgumentsAddress(address);
            
            return address;
        }
        
         uint Cpu::Zpg_R()
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeZeropage);
            const uint address = FetchPc8();
            cycles.count += cycles.clock[2];
            
            uint16_t addressFromRam = ram.mem[address];
            NESTracerSetOpcodeArgumentsAddress(addressFromRam);
            return addressFromRam;
        }
        
         uint Cpu::Zpg_RW(uint& data)
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeZeropage);
            const uint address = FetchPc8();
            cycles.count += cycles.clock[4];
            data = ram.mem[address];
            
            NESTracerSetOpcodeArgumentsAddress(address);
            return address;
        }
        
         uint Cpu::Zpg_W()
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeZeropage);
            const uint address = FetchPc8();
            cycles.count += cycles.clock[2];

            
            NESTracerSetOpcodeArgumentsAddress(address);
            return address;
        }
        
         uint Cpu::ZpgReg_R(uint indexed)
        {
//            NESTracerSetCurrentAddressingMode(<#mode#>);
            indexed = (indexed + FetchPc8()) & 0xFF;
            cycles.count += cycles.clock[3];
            return ram.mem[indexed];
        }
        
         uint Cpu::ZpgReg_RW(uint& data,uint indexed)
        {
//            NESTracerSetCurrentAddressingMode(<#mode#>);
            indexed = (indexed + FetchPc8()) & 0xFF;
            cycles.count += cycles.clock[5];
            data = ram.mem[indexed];
            return indexed;
        }
        
         uint Cpu::ZpgReg_W(uint indexed)
        {
//            NESTracerSetCurrentAddressingMode(<#mode#>);
            indexed = (indexed + FetchPc8()) & 0xFF;
            cycles.count += cycles.clock[3];
            return indexed;
        }
         uint Cpu::ZpgX_R()
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeZeroPageIndexedX);
            return ZpgReg_R( x );
        }
         uint Cpu::ZpgX_RW(uint& data)
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeZeroPageIndexedX);
            return ZpgReg_RW( data, x );
        }
         uint Cpu::ZpgX_W()
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeZeroPageIndexedX);

            return ZpgReg_W( x );
            
        }
         uint Cpu::ZpgY_R()
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeZeroPageIndexedY);
            
            return ZpgReg_R( y );
            
        }
         uint Cpu::ZpgY_RW(uint& data)
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeZeroPageIndexedY);

            return ZpgReg_RW( data, y );
            
        }
         uint Cpu::ZpgY_W()
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeZeroPageIndexedY);

            return ZpgReg_W( y );
        }
        
        uint Cpu::AbsReg_R(uint indexed)
        {
//            NESTracerSetCurrentAddressingMode(<#mode#>);

            uint data = pc;
            indexed += map.Peek8( data );
            data = (map.Peek8( data + 1 ) << 8) + indexed;
            cycles.count += cycles.clock[2];
            
            if (indexed & 0x100)
            {
                map.Peek8( data - 0x100 );
                cycles.count += cycles.clock[0];
            }
            
            data = map.Peek8( data );
            pc += 2;
            cycles.count += cycles.clock[0];
            
            return data;
        }
        
        uint Cpu::AbsReg_RW(uint& data,uint indexed)
        {
//            NESTracerSetCurrentAddressingMode(<#mode#>);
            uint address = pc;
            indexed += map.Peek8( address );
            address = (map.Peek8( address + 1 ) << 8) + indexed;
            
            map.Peek8( address - (indexed & 0x100) );
            pc += 2;
            cycles.count += cycles.clock[3];
            
            data = map.Peek8( address );
            cycles.count += cycles.clock[0];
            
            map.Poke8( address, data );
            cycles.count += cycles.clock[0];
            
            return address;
        }
        
         uint Cpu::AbsReg_W(uint indexed)
        {
//            NESTracerSetCurrentAddressingMode(<#mode#>);

            uint address = pc;
            indexed += map.Peek8( address );
            address = (map.Peek8( address + 1 ) << 8) + indexed;
            
            map.Peek8( address - (indexed & 0x100) );
            pc += 2;
            cycles.count += cycles.clock[3];
            
            return address;
        }
        
         uint Cpu::AbsX_R()
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeAbsoluteIndexedX);

            return AbsReg_R( x );
            
        }
         uint Cpu::AbsY_R()
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeAbsoluteIndexedY);

            return AbsReg_R( y );
            
        }
         uint Cpu::AbsX_RW(uint& data)
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeAbsoluteIndexedX);

            return AbsReg_RW( data, x );
        }
         uint Cpu::AbsY_RW(uint& data)
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeAbsoluteIndexedY);

            return AbsReg_RW( data, y );
            
        }
         uint Cpu::AbsX_W()
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeAbsoluteIndexedX);

            return AbsReg_W( x );
        }
         uint Cpu::AbsY_W()
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeAbsoluteIndexedY);

            return AbsReg_W( y );
        }
        
        uint Cpu::IndX_R()
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeIndexedIndirect);

            uint data = FetchPc8() + x;
            cycles.count += cycles.clock[4];
            data = FetchZpg16( data );
            
            data = map.Peek8( data );
            cycles.count += cycles.clock[0];
            
            return data;
        }
        
         uint Cpu::IndX_RW(uint& data)
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeIndexedIndirect);

            uint address = FetchPc8() + x;
            cycles.count += cycles.clock[4];
            address = FetchZpg16( address );
            
            data = map.Peek8( address );
            cycles.count += cycles.clock[0];
            
            map.Poke8( address, data );
            cycles.count += cycles.clock[0];
            
            return address;
        }
        
         uint Cpu::IndX_W()
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeIndexedIndirect);

            const uint address = FetchPc8() + x;
            cycles.count += cycles.clock[4];
            return FetchZpg16( address );
        }
        
        uint Cpu::IndY_R()
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeIndirectIndexed);

            uint data = FetchPc8();
            cycles.count += cycles.clock[3];
            
            const uint indexed = ram.mem[data] + y;
            data = (uint(ram.mem[(data + 1) & 0xFF]) << 8) + indexed;
            
            if (indexed & 0x100)
            {
                map.Peek8( data - 0x100 );
                cycles.count += cycles.clock[0];
            }
            
            data = map.Peek8( data );
            cycles.count += cycles.clock[0];
            
            return data;
        }
        
         uint Cpu::IndY_RW(uint& data)
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeIndirectIndexed);

            uint address = FetchPc8();
            cycles.count += cycles.clock[4];
            
            const uint indexed = ram.mem[address] + y;
            address = (uint(ram.mem[(address + 1) & 0xFF]) << 8) + indexed;
            map.Peek8( address - (indexed & 0x100) );
            
            data = map.Peek8( address );
            cycles.count += cycles.clock[0];
            
            map.Poke8( address, data );
            cycles.count += cycles.clock[0];
            
            return address;
        }
        
         uint Cpu::IndY_W()
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeIndirectIndexed);
            uint address = FetchPc8(); // 0
            cycles.count += cycles.clock[4];
            
            const uint indexed = ram.mem[address] + y;
            uint highByteOfTargetAddress = uint(ram.mem[(address + 1) & 0xFF]); // 7 (0x07)
            uint highByteOfAddress = highByteOfTargetAddress << 8; // 1792 (0x0700)
            uint finalAddress = highByteOfAddress | indexed; // 1792 (0x0700)
            
            address = (uint(ram.mem[(address + 1) & 0xFF]) << 8) + indexed; // 1792 (0x0700)
            
            map.Peek8( address - (indexed & 0x100) );
            
            return address;
        }
        
        
        
#pragma mark -
        
        template<bool STATE>
         void Cpu::Branch(uint tmp)
        {
            if ((!!tmp) == STATE)
            {
                pc = ((tmp=pc+1) + sign_extend_8(uint(map.Peek8( pc )))) & 0xFFFF;
                cycles.count += cycles.clock[2 + ((tmp^pc) >> 8 & 1)];
            }
            else
            {
                ++pc;
                cycles.count += cycles.clock[1];

            }
        }
        
        
        
        
        
         void Cpu::StoreMem(const uint address,const uint data)
        {
            map.Poke8( address, data );
            cycles.count += cycles.clock[0];
        }
        
         void Cpu::StoreZpg(const uint address,const uint data)
        {
            ram.mem[address] = data;
        }
        
        
        
        
        
         void Cpu::Push8(const uint data)
        {
            const uint p = sp;
            sp = (sp - 1) & 0xFF;
            
            ram.mem[0x100+p] = data;
        }
        
         void Cpu::Push16(const uint data) // data = 51140
        {   // sp = 255
            const uint p0 = sp; // 255
            const uint p1 = (p0 - 1) & 0xFF; //254
            sp = (p1 - 1) & 0xFF; // 253
            
            ram.mem[0x100+p1] = data & 0xFF; // [510] = 196
            ram.mem[0x100+p0] = data >> 8; // [511] = 199
        }
        
         uint Cpu::Pull8()
        {
            sp = (sp + 1) & 0xFF;
            
            return ram.mem[0x100+sp];
        }
        
         uint Cpu::Pull16()
        {
            // sp = 240
            const uint p0 = (sp + 1) & 0xFF; // (240 + 1) & 0xFF = 241
            const uint p1 = (p0 + 1) & 0xFF; // (p0 + 1) & 0xFF = 242
            sp = p1;
            
            
            uint returnValue = ram.mem[0x100+p0] | uint(ram.mem[0x100+p1]) << 8; // 226 | 61440 (240)
            return returnValue; // 61666
        }
        
        void Cpu::Lda(const uint data)
        {
            TraceFunctionName;

            
            a = data;
            flags.nz = data;
        }
         void Cpu::Ldx(const uint data)
        {
            TraceFunctionName;
            x = data;
            flags.nz = data;
            
        }
         void Cpu::Ldy(const uint data)
        {
            TraceFunctionName;

            y = data;
            flags.nz = data;
        }
        
        
        
        
        
         uint Cpu::Sta() const
        {
            TraceFunctionName;

            return a;
        }
         uint Cpu::Stx() const { TraceFunctionName; return x; }
         uint Cpu::Sty() const
        {
            TraceFunctionName;

            return y;
        }
        
        
        
        
        
         void Cpu::Tax()
        {
            TraceFunctionName;

            cycles.count += cycles.clock[1];
            x = a;
            flags.nz = a;
        }
        
         void Cpu::Tay()
        {
            TraceFunctionName;

            cycles.count += cycles.clock[1];
            y = a;
            flags.nz = a;
        }
        
         void Cpu::Txa()
        {
            TraceFunctionName;

            cycles.count += cycles.clock[1];
            a = x;
            flags.nz = x;
        }
        
         void Cpu::Tya()
        {
            TraceFunctionName;

            cycles.count += cycles.clock[1];
            a = y;
            flags.nz = y;
        }
        
        
        
        
        
         void Cpu::JmpAbs()
        {
            TraceFunctionName;

            pc = map.Peek16( pc );
            cycles.count += cycles.clock[JMP_ABS_CYCLES-1];
        }
        
         void Cpu::JmpInd()
        {
            TraceFunctionName;

            const uint pos = map.Peek16( pc );
            pc = map.Peek8( pos ) | (map.Peek8( (pos & 0xFF00) | ((pos + 1) & 0x00FF) ) << 8);
            
            cycles.count += cycles.clock[JMP_IND_CYCLES-1];
        }
        
         void Cpu::Jsr()
        {
            TraceFunctionName;
            // pc = 51139
            Push16( pc + 1 ); // c7c3 + 1
            pc = map.Peek16( pc ); // pc = 51139 -> 51175
            cycles.count += cycles.clock[JSR_CYCLES-1];
        }
        
         void Cpu::Rts()
        {
            TraceFunctionName;
            
            /*
             const uint p0 = (sp + 1) & 0xFF;
             const uint p1 = (p0 + 1) & 0xFF;
             sp = p1;
             
             pc = ram.mem[0x100+p0] | uint(ram.mem[0x100+p1]) << 8;
             pc += 1
             */
            
            pc = Pull16() + 1;
            cycles.count += cycles.clock[RTS_CYCLES-1];
        }
        
         void Cpu::Rti()
        {
            TraceFunctionName;
            cycles.count += cycles.clock[RTI_CYCLES-1];
            
            {
                const uint packed = Pull8();
                pc = Pull16();
                flags.Unpack( packed );
            }
            
            if (!interrupt.low || flags.i)
            {
                interrupt.irqClock = CYCLE_MAX;
            }
            else
            {
                interrupt.irqClock = 0;
                cycles.round = 0;
            }
        }
        
         void Cpu::Bne()
        {
            TraceFunctionName;

            Branch< true >( flags.nz & 0xFF );
        }
         void Cpu::Beq()
        {
            TraceFunctionName;

            Branch< false >( flags.nz & 0xFF );
        }
         void Cpu::Bmi()
        {
            TraceFunctionName;

            Branch< true >( flags.nz & 0x180 );
        }
         void Cpu::Bpl()
        {
            TraceFunctionName;

            Branch< false >( flags.nz & 0x180 );
        }
         void Cpu::Bcs() { TraceFunctionName;  Branch< true >( flags.c ); }
         void Cpu::Bcc() { TraceFunctionName;  Branch< false >( flags.c ); }
         void Cpu::Bvs()
        {
            TraceFunctionName;
            Branch< true >( flags.v );
        }
         void Cpu::Bvc() { TraceFunctionName; Branch< false >( flags.v ); }
        
        
        
        
        
         void Cpu::Adc(const uint data)
        {
            TraceFunctionName;
            
            const uint tmp = a + data + flags.c;
            flags.v = ~(a ^ data) & (a ^ tmp) & 0x80;
            a = tmp & 0xFF;
            flags.nz = a;
            flags.c = tmp >> 8 & 0x1;
        }
        
         void Cpu::Sbc(const uint data)
        {
            TraceFunctionName;

            Adc( data ^ 0xFF );
        }
        
        
        
        
        
         void Cpu::And(const uint data)
        {
            TraceFunctionName;

            a &= data;
            flags.nz = a;
        }
        
         void Cpu::Ora(const uint data)
        {
            TraceFunctionName;

            a |= data;
            flags.nz = a;
        }
        
         void Cpu::Eor(const uint data)
        {
            TraceFunctionName;

            a ^= data;
            flags.nz = a;
        }
        
         void Cpu::Bit(const uint data)
        {
            TraceFunctionName;

            flags.nz = ((data & a) != 0) | ((data & Flags::N) << 1);
            flags.v = data & Flags::V;
        }
        
         void Cpu::Cmp(uint data)
        {
            TraceFunctionName;

            data = a - data;
            flags.nz = data & 0xFF;
            flags.c = ~data >> 8 & 0x1;
        }
        
         void Cpu::Cpx(uint data)
        {
            TraceFunctionName;
            data = x - data;
            flags.nz = data & 0xFF;
            flags.c = ~data >> 8 & 0x1;
        }
        
         void Cpu::Cpy(uint data)
        {
            TraceFunctionName;

            data = y - data;
            flags.nz = data & 0xFF;
            flags.c = ~data >> 8 & 0x1;
        }
        
        
         uint Cpu::Asl(const uint data)
        {
            TraceFunctionName;

            flags.c = data >> 7;
            flags.nz = data << 1 & 0xFF;
            return flags.nz;
        }
        
         uint Cpu::Lsr(const uint data)
        {
            TraceFunctionName;
            flags.c = data & 0x01;
            flags.nz = data >> 1;
            return flags.nz;
        }
        
         uint Cpu::Rol(const uint data)
        {
            TraceFunctionName;

            flags.nz = (data << 1 & 0xFF) | flags.c;
            flags.c = data >> 7;
            
            return flags.nz;
        }
        
         uint Cpu::Ror(const uint data)
        {
            TraceFunctionName;

            
            flags.nz = (data >> 1) | (flags.c << 7);
            flags.c = data & 0x01;
            
            return flags.nz;
        }
        
        
         uint Cpu::Dec(const uint data)
        {
            TraceFunctionName;
            flags.nz = (data - 1) & 0xFF;
            return flags.nz;
        }
        
         uint Cpu::Inc(const uint data)
        {
            TraceFunctionName;

            flags.nz = (data + 1) & 0xFF;
            return flags.nz;
        }
        
         void Cpu::Dex()
        {
            TraceFunctionName;

            cycles.count += cycles.clock[1];
            x = (x - 1) & 0xFF;
            flags.nz = x;
        }
        
         void Cpu::Dey()
        {
            TraceFunctionName;
            cycles.count += cycles.clock[1];
            y = (y - 1) & 0xFF;
            flags.nz = y;
        }
        
         void Cpu::Inx()
        {
            TraceFunctionName;
            cycles.count += cycles.clock[1];
            x = (x + 1) & 0xFF;
            flags.nz = x;
        }
        
         void Cpu::Iny()
        {
            TraceFunctionName;
            cycles.count += cycles.clock[1];
            y = (y + 1) & 0xFF;
            flags.nz = y;
        }
        
        
        
        
        
        
         void Cpu::Clc()
        {
            TraceFunctionName;
            cycles.count += cycles.clock[1];
            flags.c = 0;
        }
        
         void Cpu::Sec()
        {
            TraceFunctionName;

            cycles.count += cycles.clock[1];
            flags.c = Flags::C;
        }
        
         void Cpu::Cld()
        {
            TraceFunctionName;
            cycles.count += cycles.clock[1];
            flags.d = 0;
        }
        
         void Cpu::Sed()
        {
            TraceFunctionName;
            cycles.count += cycles.clock[1];
            flags.d = Flags::D;
        }
        
         void Cpu::Clv()
        {
            TraceFunctionName;
            cycles.count += cycles.clock[1];
            flags.v = 0;
        }
        
         void Cpu::Sei()
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeImplied);
            TraceFunctionName;
            cycles.count += cycles.clock[1];
            
            if (!flags.i)
            {
                flags.i = Flags::I;
                interrupt.irqClock = CYCLE_MAX;
                
                if (interrupt.low)
                    DoISR( IRQ_VECTOR );
            }
        }
        
         void Cpu::Cli()
        {
            TraceFunctionName;
            
            cycles.count += cycles.clock[1];
            
            if (flags.i)
            {
                flags.i = 0;
                
                ((void)0);
                
                
                
                if (interrupt.low)
                {
                    interrupt.irqClock = cycles.count + 1;
                    cycles.NextRound( interrupt.irqClock );
                }
            }
        }
        
        
        
        
        
         void Cpu::Pha()
        {
            TraceFunctionName;
            
            cycles.count += cycles.clock[PHA_CYCLES-1];
            Push8( a );
        }
        
         void Cpu::Php()
        {
           TraceFunctionName;
            
            cycles.count += cycles.clock[PHP_CYCLES-1];
            Push8( flags.Pack() | Flags::B );
        }
        
         void Cpu::Pla()
        {
            
            TraceFunctionName;
            
            cycles.count += cycles.clock[PLA_CYCLES-1];
            a = Pull8();
            flags.nz = a;
        }
        
         void Cpu::Plp()
        {
            TraceFunctionName;
            
            cycles.count += cycles.clock[PLP_CYCLES-1];
            
            const uint i = flags.i;
            flags.Unpack( Pull8() );
            
            
            
            if (interrupt.low)
            {
                if (i > flags.i)
                {
                    interrupt.irqClock = cycles.count + 1;
                    cycles.NextRound( interrupt.irqClock );
                }
                else if (i < flags.i)
                {
                    interrupt.irqClock = CYCLE_MAX;
                    DoISR( IRQ_VECTOR );
                }
            }
        }
        
         void Cpu::Tsx()
        {
            TraceFunctionName;
            
            cycles.count += cycles.clock[1];
            x = sp;
            flags.nz = sp;
        }
        
         void Cpu::Txs()
        {
            TraceFunctionName;
            
            cycles.count += cycles.clock[1];
            sp = x;
        }
        
        __attribute__((no)) void Cpu::Anc(const uint data)
        {
            TraceFunctionName;
            
            a &= data;
            flags.nz = a;
            flags.c = flags.nz >> 7;
            NotifyOp("ANC",1UL << 0);
        }
        
         void Cpu::Ane(const uint data)
        {
            TraceFunctionName;
            
            a = (a | 0xEE) & x & data;
            flags.nz = a;
            NotifyOp("ANE",1UL << 1);
        }
        
         void Cpu::Arr(const uint data)
        {
            TraceFunctionName;
            
            a = ((data & a) >> 1) | (flags.c << 7);
            flags.nz = a;
            flags.c = a >> 6 & 0x1;
            flags.v = (a >> 6 ^ a >> 5) & 0x1;
            NotifyOp("ARR",1UL << 2);
        }
        
         void Cpu::Asr(const uint data)
        {
            TraceFunctionName;
            
            flags.c = data & a & 0x1;
            a = (data & a) >> 1;
            flags.nz = a;
            NotifyOp("ASR",1UL << 3);
        }
        
        __attribute__((no)) uint Cpu::Dcp(uint data)
        {
            TraceFunctionName;
            
            data = (data - 1) & 0xFF;
            Cmp( data );
            NotifyOp("DCP",1UL << 4);
            return data;
        }
        
        __attribute__((no)) uint Cpu::Isb(uint data)
        {
            TraceFunctionName;
            
            data = (data + 1) & 0xFF;
            Sbc( data );
            NotifyOp("ISB",1UL << 5);
            return data;
        }
        
         void Cpu::Las(const uint data)
        {
            TraceFunctionName;
            
            sp &= data;
            x = sp;
            a = sp;
            flags.nz = sp;
            NotifyOp("LAS",1UL << 6);
        }
        
        __attribute__((no)) void Cpu::Lax(const uint data)
        {
            TraceFunctionName;
            
            a = data;
            x = data;
            flags.nz = data;
            NotifyOp("LAX",1UL << 7);
        }
        
         void Cpu::Lxa(const uint data)
        {
            TraceFunctionName;
            
            a &= data;
            x = a;
            flags.nz = a;
            NotifyOp("LXA",1UL << 8);
        }
        
        __attribute__((no)) uint Cpu::Rla(uint data)
        {
            TraceFunctionName;
            
            const uint carry = flags.c;
            flags.c = data >> 7;
            data = (data << 1 & 0xFF) | carry;
            a &= data;
            flags.nz = a;
            NotifyOp("RLA",1UL << 9);
            return data;
        }
        
        __attribute__((no)) uint Cpu::Rra(uint data)
        {
            TraceFunctionName;
            
            const uint carry = flags.c << 7;
            flags.c = data & 0x01;
            data = (data >> 1) | carry;
            Adc( data );
            NotifyOp("RRA",1UL << 10);
            return data;
        }
        
        __attribute__((no)) uint Cpu::Sax()
        {
            TraceFunctionName;
            
            const uint data = a & x;
            NotifyOp("SAX",1UL << 11);
            return data;
        }
        
         void Cpu::Sbx(uint data)
        {
            TraceFunctionName;
            
            data = (a & x) - data;
            flags.c = (data <= 0xFF);
            x = data & 0xFF;
            flags.nz = x;
            NotifyOp("SBX",1UL << 12);
        }
        
        __attribute__((no)) uint Cpu::Sha(uint address)
        {
            TraceFunctionName;
            
            address = a & x & ((address >> 8) + 1);
            NotifyOp("SHA",1UL << 13);
            return address;
        }
        
         uint Cpu::Shs(uint address)
        {
            TraceFunctionName;
            
            sp = a & x;
            address = sp & ((address >> 8) + 1);
            NotifyOp("SHS",1UL << 14);
            return address;
        }
        
         uint Cpu::Shx(uint address)
        {
            TraceFunctionName;
            
            address = x & ((address >> 8) + 1);
            NotifyOp("SHX",1UL << 15);
            return address;
        }
        
         uint Cpu::Shy(uint address)
        {
            TraceFunctionName;
            
            address = y & ((address >> 8) + 1);
            NotifyOp("SHY",1UL << 16);
            return address;
        }
        
        __attribute__((no)) uint Cpu::Slo(uint data)
        {
            TraceFunctionName;
            
            flags.c = data >> 7;
            data = data << 1 & 0xFF;
            a |= data;
            flags.nz = a;
            NotifyOp("SLO",1UL << 17);
            return data;
        }
        
        __attribute__((no)) uint Cpu::Sre(uint data)
        {
            TraceFunctionName;
            
            flags.c = data & 0x01;
            data >>= 1;
            a ^= data;
            flags.nz = a;
            NotifyOp("SRE",1UL << 18);
            return data;
        }
        
        void Cpu::Dop()
        {
            TraceFunctionName;
            
            NotifyOp("DOP",1UL << 19);
        }
        
        void Cpu::Top(uint=0)
        {
            TraceFunctionName;
            
            NotifyOp("TOP",1UL << 20);
        }
        
        
        
        
        
         void Cpu::Brk()
        {
            TraceFunctionName;
            
            Push16( pc + 1 );
            Push8( flags.Pack() | Flags::B );
            flags.i = Flags::I;
            
            const uint vector = interrupt.Clock( cycles.count + cycles.clock[2] );
            
            pc = map.Peek16( vector ? vector : IRQ_VECTOR );
            cycles.count += cycles.clock[BRK_CYCLES-1];
        }
        
        __attribute__((no)) void Cpu::Jam()
        {
            TraceFunctionName;
            
            
            pc = (pc - 1) & 0xFFFF;
            cycles.count += cycles.clock[1];
            
            if (!jammed)
            {
                jammed = true;
                interrupt.Reset();
                ((void)0);
                Api::User::eventCallback( Api::User::EVENT_CPU_JAM );
            }
        }
        
        
        
        
        
        void Cpu::DoISR(const uint vector)
        {
            
            if (!jammed)
            {
                Push16( pc );
                Push8( flags.Pack() );
                flags.i = Flags::I;
                pc = map.Peek16( vector );
                cycles.count += cycles.clock[INT_CYCLES-1];
            }
        }
        
        void Cpu::DoIRQ(const IrqLine line,const Cycle cycle)
        {
            
            interrupt.low |= line;
            
            if (!flags.i && interrupt.irqClock == CYCLE_MAX)
            {
                interrupt.irqClock = cycle + cycles.IrqEdge();
                cycles.NextRound( interrupt.irqClock );
            }
        }
        
        void Cpu::DoNMI(const Cycle cycle)
        {
            
            if (interrupt.nmiClock == CYCLE_MAX)
            {
                interrupt.nmiClock = cycle + cycles.NmiEdge();
                cycles.NextRound( interrupt.nmiClock );
            }
        }
        
        
        
        
        
        void Cpu::Boot()
        {
            
            cycles.count = cycles.clock[RESET_CYCLES-1];
            cycles.round = 0;
            
            uint value = map.Peek16( RESET_VECTOR );
            
            pc = value;
        }
        
        void Cpu::ExecuteFrame(Sound::Output* sound)
        {
            
            
            apu.BeginFrame( sound );
            
            Clock();
            
            switch (hooks.Size())
            {
                case 0: Run0(); break;
                case 1: Run1(); break;
                default: Run2(); break;
            }
            uint value = map.Peek8(0x2000);
//            printf("\n>>0x2000:\t0x%02X\n", value);
            if (value == 0x90)
            {
                
                printf("--SET--\n");
            }
        }
        
        void Cpu::EndFrame()
        {
            
            apu.EndFrame();
                        
            ticks += cycles.frame;
            cycles.count -= cycles.frame;
            cyclesSubtracted += cycles.frame;
            interrupt.EndFrame( cycles.frame );
        }
        
        void Cpu::Clock()
        {
            Cycle clock = apu.Clock( cycles.count );
            
            if (const uint vector = interrupt.Clock( cycles.count ))
                DoISR( vector );
            
            if (clock > interrupt.irqClock)
                clock = interrupt.irqClock;
            
            if (clock > interrupt.nmiClock)
                clock = interrupt.nmiClock;
            
            if (clock > cycles.frame)
                clock = cycles.frame;
            
            cycles.round = clock;
        }
        
        void Cpu::printCPU()
        {
            printf("\nCPU-cc:\t%ld\nsp:\t0x%02X\npc:\t0x%04X\nA:\t0x%02X\nX:\t0x%02X\nY:\t0x%02X\n",(int64_t)(cyclesSubtracted + cycles.count - (7 * 12)) / 12 ,sp, pc, a, x, y);
            printf("N	V	-	B	D	I	Z	C\n%s	%s	-	%s	%s	%s	%s	%s\n",
                   ((flags.nz & Flags::N) == Flags::N) ? "1" : "0",
                   ((flags.v & Flags::V) == Flags::V) ? "1" : "0",
                   ((flags.B & Flags::B) == Flags::B) ? "1" : "0",
                   ((flags.D & Flags::D) == Flags::D) ? "1" : "0",
                   ((flags.I & Flags::I) == Flags::I) ? "1" : "0",
                   ((flags.nz & Flags::Z) == Flags::Z) ? "1" : "0",
                   ((flags.C & Flags::C) == Flags::C) ? "1" : "0");
        }
        
        void Cpu::Run0()
        {
            do
            {
                do
                {
                    uint opcodeAtPC = FetchPc8();
                    
                    NESTracerStartUpcomingCycle
                    ((int64_t)(cyclesSubtracted + cycles.count - (7 * 12)) / 12,
                     pc,
                     opcodeAtPC,
                     a,
                     x,
                     y,
                     flags.Pack(),
                     sp);

                    (*this.*opcodes[opcodeAtPC])();
                    
                    NESTracerEndCycle();
                    
                }
                while (cycles.count < cycles.round);
                
                Clock();
            }
            while (cycles.count < cycles.frame);
        }
        
        void Cpu::Run1()
        {
            const Hook hook( *hooks.Ptr() );
            
            do
            {
                do
                {
                    (*this.*opcodes[FetchPc8()])();
                    hook.Execute();
                }
                while (cycles.count < cycles.round);
                
                Clock();
            }
            while (cycles.count < cycles.frame);
        }
        
        void Cpu::Run2()
        {
            
            const Hook* const first = hooks.Ptr();
            const Hook* const last = first + (hooks.Size() - 1);
            
            do
            {
                do
                {
                    (*this.*opcodes[FetchPc8()])();
                    
                    const Hook* __restrict__ hook = first;
                    
                    hook->Execute();

                    
                    do
                    {
                        
                        (++hook)->Execute();
                    } while (hook != last);
                    
                } while (cycles.count < cycles.round);
                
                Clock();
                
            } while (cycles.count < cycles.frame);
        }
        
        uint Cpu::Peek(const uint address) const
        {
            return map.Peek8( address );
        }
        
        void Cpu::Poke(const uint address,const uint data) const
        {
            return map.Poke8( address, data );
        }
        
        void Cpu::op0x69() {  Adc( Imm_R() ); }
        void Cpu::op0x65() {  Adc( Zpg_R() ); }
        void Cpu::op0x75() {  Adc( ZpgX_R() ); }
        void Cpu::op0x6D() {  Adc( Abs_R() ); }
        void Cpu::op0x7D() {  Adc( AbsX_R() ); }
        void Cpu::op0x79() {  Adc( AbsY_R() ); }
        void Cpu::op0x61() {  Adc( IndX_R() ); }
        void Cpu::op0x71() {  Adc( IndY_R() ); }
        void Cpu::op0x29() {  And( Imm_R() ); }
        void Cpu::op0x25() {  And( Zpg_R() ); }
        void Cpu::op0x35() {  And( ZpgX_R() ); }
        void Cpu::op0x2D() {  And( Abs_R() ); }
        void Cpu::op0x3D() {  And( AbsX_R() ); }
        void Cpu::op0x39() {  And( AbsY_R() ); }
        void Cpu::op0x21() {  And( IndX_R() ); }
        void Cpu::op0x31() {  And( IndY_R() ); }
        void Cpu::op0x0A() {  cycles.count += cycles.clock[1]; a = Asl( a ); }
        void Cpu::op0x06() {  uint data; const uint dst = Zpg_RW( data ); StoreZpg( dst, Asl(data) ); }
        void Cpu::op0x16() {  uint data; const uint dst = ZpgX_RW( data ); StoreZpg(dst,Asl(data)); }
        void Cpu::op0x0E() {  uint data; const uint dst = Abs_RW( data ); StoreMem(dst,Asl(data)); }
        void Cpu::op0x1E() {  uint data; const uint dst = AbsX_RW( data ); StoreMem(dst,Asl(data)); }
        void Cpu::op0x90() {  Bcc(); }
        void Cpu::op0xB0()
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeRelative);
            
            Bcs();
        }
        void Cpu::op0xF0()
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeRelative);
            
            Beq();
            
        }
        void Cpu::op0x24() {  Bit( Zpg_R() ); }
        void Cpu::op0x2C() {  Bit( Abs_R() ); }
        void Cpu::op0x30()
        {
            
            Bmi();
            
        }
        void Cpu::op0xD0()
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeRelative);
            
            Bne();
            
        }
        void Cpu::op0x10()
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeRelative);
            
            Bpl();
        }
        void Cpu::op0x50() {  Bvc(); }
        void Cpu::op0x70() {  Bvs(); }
        void Cpu::op0x18()
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeImplied);
            
            Clc();
        }
        void Cpu::op0xD8()
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeImplied);
            
            Cld();
        }
        void Cpu::op0x58() {  Cli(); }
        void Cpu::op0xB8() {  Clv(); }
        void Cpu::op0xC9() {  Cmp( Imm_R() ); }
        void Cpu::op0xC5() {  Cmp( Zpg_R() ); }
        void Cpu::op0xD5() {  Cmp( ZpgX_R() ); }
        void Cpu::op0xCD() {  Cmp( Abs_R() ); }
        void Cpu::op0xDD() {  Cmp( AbsX_R() ); }
        void Cpu::op0xD9() {  Cmp( AbsY_R() ); }
        void Cpu::op0xC1() {  Cmp( IndX_R() ); }
        void Cpu::op0xD1() {  Cmp( IndY_R() ); }
        void Cpu::op0xE0() {  Cpx( Imm_R() ); }
        void Cpu::op0xE4() {  Cpx( Zpg_R() ); }
        void Cpu::op0xEC() {  Cpx( Abs_R() ); }
        void Cpu::op0xC0() {  Cpy( Imm_R() ); }
        void Cpu::op0xC4() {  Cpy( Zpg_R() ); }
        void Cpu::op0xCC() {  Cpy( Abs_R() ); }
        void Cpu::op0xC6() {  uint data; const uint dst = Zpg_RW( data ); StoreZpg( dst, Dec(data) ); }
        void Cpu::op0xD6() {  uint data; const uint dst = ZpgX_RW( data ); StoreZpg(dst,Dec(data)); }
        void Cpu::op0xCE() {  uint data; const uint dst = Abs_RW( data ); StoreMem(dst,Dec(data)); }
        void Cpu::op0xDE() {  uint data; const uint dst = AbsX_RW( data ); StoreMem(dst,Dec(data)); }
        void Cpu::op0xCA()
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeImplied);
            NESTracerNESAddressingModeImplied;
            
            Dex();
            
        }
        void Cpu::op0x88()
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeImplied);
            
            Dey();
            
        }
        void Cpu::op0x49() {  Eor( Imm_R() ); }
        void Cpu::op0x45() {  Eor( Zpg_R() ); }
        void Cpu::op0x55() {  Eor( ZpgX_R() ); }
        void Cpu::op0x4D() {  Eor( Abs_R() ); }
        void Cpu::op0x5D() {  Eor( AbsX_R() ); }
        void Cpu::op0x59() {  Eor( AbsY_R() ); }
        void Cpu::op0x41() {  Eor( IndX_R() ); }
        void Cpu::op0x51() {  Eor( IndY_R() ); }
        void Cpu::op0xE6() {  uint data; const uint dst = Zpg_RW( data ); StoreZpg( dst, Inc(data) ); }
        void Cpu::op0xF6() {  uint data; const uint dst = ZpgX_RW( data ); StoreZpg(dst,Inc(data)); }
        void Cpu::op0xEE() {  uint data; const uint dst = Abs_RW( data ); StoreMem(dst,Inc(data)); }
        void Cpu::op0xFE() {
            
            
            uint data;
            const uint dst = AbsX_RW( data );
            StoreMem(dst,Inc(data));
        }
        void Cpu::op0xE8()
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeImplied);
            
            Inx();
        }
        void Cpu::op0xC8()
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeImplied);
            
            Iny();
        }
        void Cpu::op0x4C()
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeAbsolute);
            
            JmpAbs();
        }
        void Cpu::op0x6C() {  JmpInd(); }
        void Cpu::op0x20()
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeAbsolute);
            
            Jsr();
            
        }
        void Cpu::op0xA9()
        {
            Lda( Imm_R() );
        }
        void Cpu::op0xA5() {  Lda( Zpg_R() ); }
        void Cpu::op0xB5() {  Lda( ZpgX_R() );}
        void Cpu::op0xAD()
        {
            
            Lda( Abs_R() );
        }
        void Cpu::op0xBD() {  Lda( AbsX_R() ); }
        void Cpu::op0xB9() {  Lda( AbsY_R() ); }
        void Cpu::op0xA1() {  Lda( IndX_R() ); }
        void Cpu::op0xB1()
        {
            
            Lda( IndY_R() );
        }
        void Cpu::op0xA2()
        {
            
            Ldx( Imm_R() );
        }
        void Cpu::op0xA6() {  Ldx( Zpg_R() ); }
        void Cpu::op0xB6() {  Ldx( ZpgY_R() ); }
        void Cpu::op0xAE() {  Ldx( Abs_R() ); }
        void Cpu::op0xBE() {  Ldx( AbsY_R() ); }
        void Cpu::op0xA0() {  Ldy( Imm_R() ); }
        void Cpu::op0xA4() {  Ldy( Zpg_R() ); }
        void Cpu::op0xB4() {  Ldy( ZpgX_R() ); }
        void Cpu::op0xAC() {  Ldy( Abs_R() ); }
        void Cpu::op0xBC() {  Ldy( AbsX_R() ); }
        void Cpu::op0x4A()
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeAccumulator);
            
            cycles.count += cycles.clock[1];
            a = Lsr( a );
        }
        void Cpu::op0x46() {  uint data; const uint dst = Zpg_RW( data ); StoreZpg( dst, Lsr(data) ); }
        void Cpu::op0x56() {  uint data; const uint dst = ZpgX_RW( data ); StoreZpg(dst,Lsr(data)); }
        void Cpu::op0x4E() {  uint data; const uint dst = Abs_RW( data ); StoreMem(dst,Lsr(data)); }
        void Cpu::op0x5E() {  uint data; const uint dst = AbsX_RW( data ); StoreMem(dst,Lsr(data)); }
        void Cpu::op0x1A() {  cycles.count += cycles.clock[2 - 1]; }
        void Cpu::op0x3A() {  cycles.count += cycles.clock[2 - 1]; }
        void Cpu::op0x5A() {  cycles.count += cycles.clock[2 - 1]; }
        void Cpu::op0x7A() {  cycles.count += cycles.clock[2 - 1]; }
        void Cpu::op0xDA() {  cycles.count += cycles.clock[2 - 1]; }
        void Cpu::op0xEA() {  cycles.count += cycles.clock[2 - 1]; }
        void Cpu::op0xFA() {  cycles.count += cycles.clock[2 - 1]; }
        void Cpu::op0x09() {  Ora( Imm_R() ); }
        void Cpu::op0x05() {  Ora( Zpg_R() ); }
        void Cpu::op0x15() {  Ora( ZpgX_R() ); }
        void Cpu::op0x0D() {  Ora( Abs_R() ); }
        void Cpu::op0x1D() {  Ora( AbsX_R() ); }
        void Cpu::op0x19() {  Ora( AbsY_R() ); }
        void Cpu::op0x01() {  Ora( IndX_R() ); }
        void Cpu::op0x11() {  Ora( IndY_R() ); }
        void Cpu::op0x48()
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeImplied);
            
            Pha();
        }
        void Cpu::op0x08() {  Php(); }
        void Cpu::op0x68()
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeImplied);
            
            Pla();
        }
        void Cpu::op0x28() {  Plp(); }
        void Cpu::op0x2A() {  cycles.count += cycles.clock[1]; a = Rol( a ); }
        void Cpu::op0x26() {  uint data; const uint dst = Zpg_RW( data ); StoreZpg( dst, Rol(data) ); }
        void Cpu::op0x36() {  uint data; const uint dst = ZpgX_RW( data ); StoreZpg(dst,Rol(data)); }
        void Cpu::op0x2E() {  uint data; const uint dst = Abs_RW( data ); StoreMem(dst,Rol(data)); }
        void Cpu::op0x3E() {  uint data; const uint dst = AbsX_RW( data ); StoreMem(dst,Rol(data)); }
        void Cpu::op0x6A() {  cycles.count += cycles.clock[1]; a = Ror( a ); }
        void Cpu::op0x66() {  uint data; const uint dst = Zpg_RW( data ); StoreZpg( dst, Ror(data) ); }
        void Cpu::op0x76() {  uint data; const uint dst = ZpgX_RW( data ); StoreZpg(dst,Ror(data)); }
        void Cpu::op0x6E() {  uint data; const uint dst = Abs_RW( data ); StoreMem(dst,Ror(data)); }
        void Cpu::op0x7E() {  uint data; const uint dst = AbsX_RW( data ); StoreMem(dst,Ror(data)); }
        void Cpu::op0x40()
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeImplied);
            
            Rti();
        }
        void Cpu::op0x60()
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeImplied);
            
            Rts();
        }
        void Cpu::op0xE9() {  Sbc( Imm_R() ); }
        void Cpu::op0xEB() {  Sbc( Imm_R() ); }
        void Cpu::op0xE5() {  Sbc( Zpg_R() ); }
        void Cpu::op0xF5() {  Sbc( ZpgX_R() ); }
        void Cpu::op0xED() {  Sbc( Abs_R() ); }
        void Cpu::op0xFD() {  Sbc( AbsX_R() ); }
        void Cpu::op0xF9() {  Sbc( AbsY_R() ); }
        void Cpu::op0xE1() {  Sbc( IndX_R() ); }
        void Cpu::op0xF1() {  Sbc( IndY_R() ); }
        void Cpu::op0x38() {  Sec(); }
        void Cpu::op0xF8() {  Sed(); }
        void Cpu::op0x78()
        {
            
            Sei();
        }
        void Cpu::op0x85() {  const uint dst = Zpg_W(); StoreZpg( dst, Sta() ); }
        void Cpu::op0x95() {  const uint dst = ZpgX_W(); StoreZpg(dst,Sta()); }
        void Cpu::op0x8D() {
            
            const uint dst = Abs_W();
            StoreMem(dst,Sta());
        }
        void Cpu::op0x9D() {  const uint dst = AbsX_W(); StoreMem(dst,Sta()); }
        void Cpu::op0x99() {  const uint dst = AbsY_W(); StoreMem(dst,Sta()); }
        void Cpu::op0x81() {  const uint dst = IndX_W(); StoreMem(dst,Sta()); }
        void Cpu::op0x91() {  const uint dst = IndY_W(); StoreMem(dst,Sta()); }
        void Cpu::op0x86() {  const uint dst = Zpg_W(); StoreZpg( dst, Stx() ); }
        void Cpu::op0x96() {  const uint dst = ZpgY_W(); StoreZpg(dst,Stx()); }
        void Cpu::op0x8E() {  const uint dst = Abs_W(); StoreMem(dst,Stx()); }
        void Cpu::op0x84()
        {
            
            const uint dst = Zpg_W(); // 1
            StoreZpg( dst, Sty() );
        }
        void Cpu::op0x94() {  const uint dst = ZpgX_W(); StoreZpg(dst,Sty()); }
        void Cpu::op0x8C() {  const uint dst = Abs_W(); StoreMem(dst,Sty()); }
        void Cpu::op0xAA()
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeImplied);
            
            Tax();
        }
        void Cpu::op0xA8()
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeImplied);
            
            Tay();
        }
        void Cpu::op0xBA() {  Tsx(); }
        void Cpu::op0x8A()
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeImplied);
            
            Txa();
        }
        void Cpu::op0x9A()
        {
            NESTracerSetCurrentAddressingMode(NESTracerNESAddressingModeImplied);
            
            Txs();
            
        }
        void Cpu::op0x98() {  Tya(); }
        
        void Cpu::op0x00() {  Brk(); }
        void Cpu::op0x02() {  Jam(); }
        void Cpu::op0x12() {  Jam(); }
        void Cpu::op0x22() {  Jam(); }
        void Cpu::op0x32() {  Jam(); }
        void Cpu::op0x42() {  Jam(); }
        void Cpu::op0x52() {  Jam(); }
        void Cpu::op0x62() {  Jam(); }
        void Cpu::op0x72() {  Jam(); }
        void Cpu::op0x92() {  Jam(); }
        void Cpu::op0xB2() {  Jam(); }
        void Cpu::op0xD2() {  Jam(); }
        void Cpu::op0xF2() {  Jam(); }
        
        void Cpu::op0x0B() {  Anc( Imm_R() ); }
        void Cpu::op0x2B() {  Anc( Imm_R() ); }
        void Cpu::op0x8B() {  Ane( Imm_R() ); }
        void Cpu::op0x6B() {  Arr( Imm_R() ); }
        void Cpu::op0x4B() {  Asr( Imm_R() ); }
        void Cpu::op0xC7() {  uint data; const uint dst = Zpg_RW( data ); StoreZpg( dst, Dcp(data) ); }
        void Cpu::op0xD7() {  uint data; const uint dst = ZpgX_RW( data ); StoreZpg(dst,Dcp(data)); }
        void Cpu::op0xC3() {  uint data; const uint dst = IndX_RW( data ); StoreMem(dst,Dcp(data)); }
        void Cpu::op0xD3() {  uint data; const uint dst = IndY_RW( data ); StoreMem(dst,Dcp(data)); }
        void Cpu::op0xCF() {  uint data; const uint dst = Abs_RW( data ); StoreMem(dst,Dcp(data)); }
        void Cpu::op0xDF() {  uint data; const uint dst = AbsX_RW( data ); StoreMem(dst,Dcp(data)); }
        void Cpu::op0xDB() {  uint data; const uint dst = AbsY_RW( data ); StoreMem(dst,Dcp(data)); }
        void Cpu::op0x80() {  pc += 1; cycles.count += cycles.clock[2 - 1]; Dop(); }
        void Cpu::op0x82() {  pc += 1; cycles.count += cycles.clock[2 - 1]; Dop(); }
        void Cpu::op0x89() {  pc += 1; cycles.count += cycles.clock[2 - 1]; Dop(); }
        void Cpu::op0xC2() {  pc += 1; cycles.count += cycles.clock[2 - 1]; Dop(); }
        void Cpu::op0xE2() {  pc += 1; cycles.count += cycles.clock[2 - 1]; Dop(); }
        void Cpu::op0x04() {  pc += 1; cycles.count += cycles.clock[3 - 1]; Dop(); }
        void Cpu::op0x44() {  pc += 1; cycles.count += cycles.clock[3 - 1]; Dop(); }
        void Cpu::op0x64() {  pc += 1; cycles.count += cycles.clock[3 - 1]; Dop(); }
        void Cpu::op0x14() {  pc += 1; cycles.count += cycles.clock[4 - 1]; Dop(); }
        void Cpu::op0x34() {  pc += 1; cycles.count += cycles.clock[4 - 1]; Dop(); }
        void Cpu::op0x54() {  pc += 1; cycles.count += cycles.clock[4 - 1]; Dop(); }
        void Cpu::op0x74() {  pc += 1; cycles.count += cycles.clock[4 - 1]; Dop(); }
        void Cpu::op0xD4() {  pc += 1; cycles.count += cycles.clock[4 - 1]; Dop(); }
        void Cpu::op0xF4() {  pc += 1; cycles.count += cycles.clock[4 - 1]; Dop(); }
        void Cpu::op0xE7() {  uint data; const uint dst = Zpg_RW( data ); StoreZpg( dst, Isb(data) ); }
        void Cpu::op0xF7() {  uint data; const uint dst = ZpgX_RW( data ); StoreZpg(dst,Isb(data)); }
        void Cpu::op0xEF() {  uint data; const uint dst = Abs_RW( data ); StoreMem(dst,Isb(data)); }
        void Cpu::op0xFF()
        {
            
            uint data;
            const uint dst = AbsX_RW( data );
            StoreMem(dst,Isb(data));
        }
        void Cpu::op0xFB() {  uint data; const uint dst = AbsY_RW( data ); StoreMem(dst,Isb(data)); }
        void Cpu::op0xE3() {  uint data; const uint dst = IndX_RW( data ); StoreMem(dst,Isb(data)); }
        void Cpu::op0xF3() {  uint data; const uint dst = IndY_RW( data ); StoreMem(dst,Isb(data)); }
        void Cpu::op0xBB() {  Las( AbsY_R() ); }
        void Cpu::op0xA7() {  Lax( Zpg_R() ); }
        void Cpu::op0xB7() {  Lax( ZpgY_R() ); }
        void Cpu::op0xAF() {  Lax( Abs_R() ); }
        void Cpu::op0xBF() {  Lax( AbsY_R() ); }
        void Cpu::op0xA3() {  Lax( IndX_R() ); }
        void Cpu::op0xB3() {  Lax( IndY_R() ); }
        void Cpu::op0xAB() {  Lxa( Imm_R() ); }
        void Cpu::op0x27() {  uint data; const uint dst = Zpg_RW( data ); StoreZpg( dst, Rla(data) ); }
        void Cpu::op0x37() {  uint data; const uint dst = ZpgX_RW( data ); StoreZpg(dst,Rla(data)); }
        void Cpu::op0x2F() {  uint data; const uint dst = Abs_RW( data ); StoreMem(dst,Rla(data)); }
        void Cpu::op0x3F() {  uint data; const uint dst = AbsX_RW( data ); StoreMem(dst,Rla(data)); }
        void Cpu::op0x3B() {  uint data; const uint dst = AbsY_RW( data ); StoreMem(dst,Rla(data)); }
        void Cpu::op0x23() {  uint data; const uint dst = IndX_RW( data ); StoreMem(dst,Rla(data)); }
        void Cpu::op0x33() {  uint data; const uint dst = IndY_RW( data ); StoreMem(dst,Rla(data)); }
        void Cpu::op0x67() {  uint data; const uint dst = Zpg_RW( data ); StoreZpg( dst, Rra(data) ); }
        void Cpu::op0x77() {  uint data; const uint dst = ZpgX_RW( data ); StoreZpg(dst,Rra(data)); }
        void Cpu::op0x6F() {  uint data; const uint dst = Abs_RW( data ); StoreMem(dst,Rra(data)); }
        void Cpu::op0x7F() {  uint data; const uint dst = AbsX_RW( data ); StoreMem(dst,Rra(data)); }
        void Cpu::op0x7B() {  uint data; const uint dst = AbsY_RW( data ); StoreMem(dst,Rra(data)); }
        void Cpu::op0x63() {  uint data; const uint dst = IndX_RW( data ); StoreMem(dst,Rra(data)); }
        void Cpu::op0x73() {  uint data; const uint dst = IndY_RW( data ); StoreMem(dst,Rra(data)); }
        void Cpu::op0x87() {  const uint dst = Zpg_W(); StoreZpg( dst, Sax() ); }
        void Cpu::op0x97() {  const uint dst = ZpgY_W(); StoreZpg(dst,Sax()); }
        void Cpu::op0x8F() {  const uint dst = Abs_W(); StoreMem(dst,Sax()); }
        void Cpu::op0x83() {  const uint dst = IndX_W(); StoreMem(dst,Sax()); }
        void Cpu::op0xCB() {  Sbx( Imm_R() ); }
        void Cpu::op0x9F() {  const uint dst = AbsY_W(); StoreMem(dst,Sha(dst)); }
        void Cpu::op0x93() {  const uint dst = IndY_W(); StoreMem(dst,Sha(dst)); }
        void Cpu::op0x9B() {  const uint dst = AbsY_W(); StoreMem(dst,Shs(dst)); }
        void Cpu::op0x9E() {  const uint dst = AbsY_W(); StoreMem(dst,Shx(dst)); }
        void Cpu::op0x9C() {  const uint dst = AbsX_W(); StoreMem(dst,Shy(dst)); }
        void Cpu::op0x07() {  uint data; const uint dst = Zpg_RW( data ); StoreZpg( dst, Slo(data) ); }
        void Cpu::op0x17() {  uint data; const uint dst = ZpgX_RW( data ); StoreZpg(dst,Slo(data)); }
        void Cpu::op0x0F() {  uint data; const uint dst = Abs_RW( data ); StoreMem(dst,Slo(data)); }
        void Cpu::op0x1F() {  uint data; const uint dst = AbsX_RW( data ); StoreMem(dst,Slo(data)); }
        void Cpu::op0x1B() {  uint data; const uint dst = AbsY_RW( data ); StoreMem(dst,Slo(data)); }
        void Cpu::op0x03() {  uint data; const uint dst = IndX_RW( data ); StoreMem(dst,Slo(data)); }
        void Cpu::op0x13() {  uint data; const uint dst = IndY_RW( data ); StoreMem(dst,Slo(data)); }
        void Cpu::op0x47() {  uint data; const uint dst = Zpg_RW( data ); StoreZpg( dst, Sre(data) ); }
        void Cpu::op0x57() {  uint data; const uint dst = ZpgX_RW( data ); StoreZpg(dst,Sre(data)); }
        void Cpu::op0x4F() {  uint data; const uint dst = Abs_RW( data ); StoreMem(dst,Sre(data)); }
        void Cpu::op0x5F() {  uint data; const uint dst = AbsX_RW( data ); StoreMem(dst,Sre(data)); }
        void Cpu::op0x5B() {  uint data; const uint dst = AbsY_RW( data ); StoreMem(dst,Sre(data)); }
        void Cpu::op0x43() {  uint data; const uint dst = IndX_RW( data ); StoreMem(dst,Sre(data)); }
        void Cpu::op0x53() {  uint data; const uint dst = IndY_RW( data ); StoreMem(dst,Sre(data)); }
        void Cpu::op0x0C() {  pc += 2; cycles.count += cycles.clock[4 - 1]; Top(); }
        void Cpu::op0x1C() {  Top( AbsX_R() ); }
        void Cpu::op0x3C() {  Top( AbsX_R() ); }
        void Cpu::op0x5C() {  Top( AbsX_R() ); }
        void Cpu::op0x7C() {  Top( AbsX_R() ); }
        void Cpu::op0xDC() {  Top( AbsX_R() ); }
        void Cpu::op0xFC() {  Top( AbsX_R() ); }
    }
}
