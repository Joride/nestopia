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
#include "NstState.hpp"
#include "api/NstApiSound.hpp"
#include "NstSoundRenderer.inl"

namespace Nes
{
    namespace Core
    {
        const dword Apu::Cycles::frameClocks[2][4] =
        {
            {
                Clocks::RP2A03_CC * 29830UL,
                Clocks::RP2A03_CC,
                Clocks::RP2A03_CC,
                Clocks::RP2A03_CC * (29830UL - 2),
            },
            {
                Clocks::RP2A07_CC * 33254UL,
                Clocks::RP2A07_CC,
                Clocks::RP2A07_CC,
                Clocks::RP2A07_CC * (33254UL - 2)
            }
        };
        
        const dword Apu::Cycles::oscillatorClocks[2][2][4] =
        {
            {
                {
                    Clocks::RP2A03_CC * (7459UL - 1),
                    Clocks::RP2A03_CC * 7456UL,
                    Clocks::RP2A03_CC * 7458UL,
                    Clocks::RP2A03_CC * 7458UL
                },
                {
                    Clocks::RP2A03_CC * 7458UL,
                    Clocks::RP2A03_CC * 7456UL,
                    Clocks::RP2A03_CC * 7458UL,
                    Clocks::RP2A03_CC * (7458UL + 7452)
                }
            },
            {
                {
                    Clocks::RP2A07_CC * (8315UL - 1),
                    Clocks::RP2A07_CC * 8314UL,
                    Clocks::RP2A07_CC * 8312UL,
                    Clocks::RP2A07_CC * 8314UL
                },
                {
                    Clocks::RP2A07_CC * 8314UL,
                    Clocks::RP2A07_CC * 8314UL,
                    Clocks::RP2A07_CC * 8312UL,
                    Clocks::RP2A07_CC * (8314UL + 8312)
                }
            }
        };
        
        const byte Apu::Channel::LengthCounter::lut[32] =
        {
            0x0A, 0xFE, 0x14, 0x02,
            0x28, 0x04, 0x50, 0x06,
            0xA0, 0x08, 0x3C, 0x0A,
            0x0E, 0x0C, 0x1A, 0x0E,
            0x0C, 0x10, 0x18, 0x12,
            0x30, 0x14, 0x60, 0x16,
            0xC0, 0x18, 0x48, 0x1A,
            0x10, 0x1C, 0x20, 0x1E
        };
        
        const word Apu::Noise::lut[2][16] =
        {
            {
                0x004, 0x008, 0x010, 0x020,
                0x040, 0x060, 0x080, 0x0A0,
                0x0CA, 0x0FE, 0x17C, 0x1FC,
                0x2FA, 0x3F8, 0x7F2, 0xFE4
            },
            {
                0x004, 0x007, 0x00E, 0x01E,
                0x03C, 0x058, 0x076, 0x094,
                0x0BC, 0x0EC, 0x162, 0x1D8,
                0x2C4, 0x3B0, 0x762, 0xEC2
            }
        };
        
        const word Apu::Dmc::lut[2][16] =
        {
            {
                0x1AC * Clocks::RP2A03_CC,
                0x17C * Clocks::RP2A03_CC,
                0x154 * Clocks::RP2A03_CC,
                0x140 * Clocks::RP2A03_CC,
                0x11E * Clocks::RP2A03_CC,
                0x0FE * Clocks::RP2A03_CC,
                0x0E2 * Clocks::RP2A03_CC,
                0x0D6 * Clocks::RP2A03_CC,
                0x0BE * Clocks::RP2A03_CC,
                0x0A0 * Clocks::RP2A03_CC,
                0x08E * Clocks::RP2A03_CC,
                0x080 * Clocks::RP2A03_CC,
                0x06A * Clocks::RP2A03_CC,
                0x054 * Clocks::RP2A03_CC,
                0x048 * Clocks::RP2A03_CC,
                0x036 * Clocks::RP2A03_CC
            },
            {
                0x18E * Clocks::RP2A07_CC,
                0x162 * Clocks::RP2A07_CC,
                0x13C * Clocks::RP2A07_CC,
                0x12A * Clocks::RP2A07_CC,
                0x114 * Clocks::RP2A07_CC,
                0x0EC * Clocks::RP2A07_CC,
                0x0D2 * Clocks::RP2A07_CC,
                0x0C6 * Clocks::RP2A07_CC,
                0x0B0 * Clocks::RP2A07_CC,
                0x094 * Clocks::RP2A07_CC,
                0x084 * Clocks::RP2A07_CC,
                0x076 * Clocks::RP2A07_CC,
                0x062 * Clocks::RP2A07_CC,
                0x04E * Clocks::RP2A07_CC,
                0x042 * Clocks::RP2A07_CC,
                0x032 * Clocks::RP2A07_CC
            }
        };
        
        
        
        
        
        Apu::Apu(Cpu& c)
        :
        cpu (c),
        extChannel (__null),
        buffer (16)
        {
            typedef char Nestopia_assertion_at_line_161[(Region::NTSC == 0 && Region::PAL == 1) ? 1 : -1];
            
            PowerOff();
            UpdateSettings();
        }
        
        void Apu::PowerOff()
        {
            Reset( false, true );
        }
        
        void Apu::Reset(bool hard)
        {
            Reset( true, hard );
        }
        
        void Apu::Reset(const bool on,const bool hard)
        {
            updater = &Apu::SyncOff;
            
            cycles.Reset( extChannel );
            synchronizer.Resync( settings.speed, cycles.region );
            
            for (uint i=0; i < 2; ++i)
                square[i].Reset();
            
            triangle.Reset();
            noise.Reset( cycles.region );
            dmc.Reset( cycles.region );
            
            dcBlocker.Reset();
            
            stream = __null;
            
            buffer.Reset( settings.bits );
            
            if (on)
            {
                cpu.Map( 0x4000 ).Set( this, &Apu::Peek_4xxx, &Apu::Poke_4000 );
                cpu.Map( 0x4001 ).Set( this, &Apu::Peek_4xxx, &Apu::Poke_4001 );
                cpu.Map( 0x4002 ).Set( this, &Apu::Peek_4xxx, &Apu::Poke_4002 );
                cpu.Map( 0x4003 ).Set( this, &Apu::Peek_4xxx, &Apu::Poke_4003 );
                cpu.Map( 0x4004 ).Set( this, &Apu::Peek_4xxx, &Apu::Poke_4000 );
                cpu.Map( 0x4005 ).Set( this, &Apu::Peek_4xxx, &Apu::Poke_4001 );
                cpu.Map( 0x4006 ).Set( this, &Apu::Peek_4xxx, &Apu::Poke_4002 );
                cpu.Map( 0x4007 ).Set( this, &Apu::Peek_4xxx, &Apu::Poke_4003 );
                cpu.Map( 0x4008 ).Set( this, &Apu::Peek_4xxx, &Apu::Poke_4008 );
                cpu.Map( 0x400A ).Set( this, &Apu::Peek_4xxx, &Apu::Poke_400A );
                cpu.Map( 0x400B ).Set( this, &Apu::Peek_4xxx, &Apu::Poke_400B );
                cpu.Map( 0x400C ).Set( this, &Apu::Peek_4xxx, &Apu::Poke_400C );
                cpu.Map( 0x400E ).Set( this, &Apu::Peek_4xxx, &Apu::Poke_400E );
                cpu.Map( 0x400F ).Set( this, &Apu::Peek_4xxx, &Apu::Poke_400F );
                cpu.Map( 0x4010 ).Set( this, &Apu::Peek_4xxx, &Apu::Poke_4010 );
                cpu.Map( 0x4011 ).Set( this, &Apu::Peek_4xxx, &Apu::Poke_4011 );
                cpu.Map( 0x4012 ).Set( this, &Apu::Peek_4xxx, &Apu::Poke_4012 );
                cpu.Map( 0x4013 ).Set( this, &Apu::Peek_4xxx, &Apu::Poke_4013 );
                cpu.Map( 0x4015 ).Set( this, &Apu::Peek_4015, &Apu::Poke_4015 );
                
                if (hard)
                    ctrl = STATUS_FRAME_IRQ_ENABLE;
                
                if (ctrl == STATUS_FRAME_IRQ_ENABLE)
                    cycles.frameIrqClock = (cycles.frameCounter / cycles.fixed) - cpu.GetClock();
                
                if (extChannel)
                    extChannel->Reset();
            }
            else
            {
                ctrl = STATUS_FRAME_IRQ_ENABLE;
            }
        }
        
        Result Apu::SetSampleRate(const dword rate)
        {
            if (settings.rate == rate)
                return RESULT_NOP;
            
            if (!rate)
                return RESULT_ERR_INVALID_PARAM;
            
            if (rate < 11025 || rate > 96000)
                return RESULT_ERR_UNSUPPORTED;
            
            settings.rate = rate;
            UpdateSettings();
            
            return RESULT_OK;
        }
        
        Result Apu::SetSampleBits(const uint bits)
        {
            if (settings.bits == bits)
                return RESULT_NOP;
            
            if (!bits)
                return RESULT_ERR_INVALID_PARAM;
            
            if (bits != 8 && bits != 16)
                return RESULT_ERR_UNSUPPORTED;
            
            settings.bits = bits;
            UpdateSettings();
            
            return RESULT_OK;
        }
        
        Result Apu::SetVolume(const uint channels,const uint volume)
        {
            if (volume > 100)
                return RESULT_ERR_INVALID_PARAM;
            
            bool updated = false;
            
            for (uint i=0; i < MAX_CHANNELS; ++i)
            {
                if (channels & (1U << i))
                {
                    if (settings.volumes[i] != volume)
                    {
                        settings.volumes[i] = volume;
                        updated = true;
                    }
                }
            }
            
            if (!updated)
                return RESULT_NOP;
            
            UpdateSettings();
            
            return RESULT_OK;
        }
        
        uint Apu::GetVolume(const uint channel) const
        {
            for (uint i=0; i < MAX_CHANNELS; ++i)
            {
                if (channel & (1U << i))
                    return settings.volumes[i];
            }
            
            return 0;
        }
        
        Result Apu::SetSpeed(const uint speed)
        {
            if (settings.speed == speed)
                return RESULT_NOP;
            
            if ((speed > 0 && speed < 30) || speed > 240)
                return RESULT_ERR_UNSUPPORTED;
            
            settings.speed = speed;
            UpdateSettings();
            
            return RESULT_OK;
        }
        
        void Apu::SetAutoTranspose(const bool transpose)
        {
            if (settings.transpose != transpose)
            {
                settings.transpose = transpose;
                UpdateSettings();
            }
        }
        
        void Apu::EnableStereo(const bool enable)
        {
            if (settings.stereo != enable)
            {
                settings.stereo = enable;
                UpdateSettings();
            }
        }
        
        void Apu::UpdateRegion()
        {
            UpdateSettings( cpu.GetRegion() );
        }
        
        void Apu::UpdateSettings()
        {
            UpdateSettings( cycles.region );
        }
        
        void Apu::UpdateSettings(const Region::Type region)
        {
            cycles.Update( settings.rate, settings.speed, region );
            synchronizer.Reset( settings.speed, region, settings.rate );
            dcBlocker.Reset();
            buffer.Reset( settings.bits );
            
            Cycle rate; uint fixed;
            CalculateOscillatorClock( rate, fixed );
            
            square[0].UpdateSettings ( rate, fixed, (settings.volumes[ Channel::APU_SQUARE1 ] * uint(Channel::OUTPUT_MUL) + Channel::DEFAULT_VOLUME/2) / Channel::DEFAULT_VOLUME );
            square[1].UpdateSettings ( rate, fixed, (settings.volumes[ Channel::APU_SQUARE2 ] * uint(Channel::OUTPUT_MUL) + Channel::DEFAULT_VOLUME/2) / Channel::DEFAULT_VOLUME );
            triangle.UpdateSettings ( rate, fixed, (settings.volumes[ Channel::APU_TRIANGLE ] * uint(Channel::OUTPUT_MUL) + Channel::DEFAULT_VOLUME/2) / Channel::DEFAULT_VOLUME );
            noise.UpdateSettings ( rate, fixed, (settings.volumes[ Channel::APU_NOISE ] * uint(Channel::OUTPUT_MUL) + Channel::DEFAULT_VOLUME/2) / Channel::DEFAULT_VOLUME, region );
            dmc.UpdateSettings ( cycles.dmcClock, (settings.volumes[ Channel::APU_DPCM ] * uint(Channel::OUTPUT_MUL) + Channel::DEFAULT_VOLUME/2) / Channel::DEFAULT_VOLUME, region );
            
            UpdateVolumes();
        }
        
        void Apu::UpdateVolumes()
        {
            settings.audible = (extChannel && extChannel->UpdateSettings()) ||
            (
             uint(settings.volumes[ Channel::APU_SQUARE1 ]) |
             uint(settings.volumes[ Channel::APU_SQUARE2 ]) |
             uint(settings.volumes[ Channel::APU_TRIANGLE ]) |
             uint(settings.volumes[ Channel::APU_NOISE ]) |
             uint(settings.volumes[ Channel::APU_DPCM ])
             );
        }
        
        void Apu::Resync(const dword rate)
        {
            cycles.Update( rate, settings.speed, cycles.region );
            ClearBuffers( false );
        }
        
        void Apu::CalculateOscillatorClock(Cycle& rate,uint& fixed) const
        {
            static const dword clocks[2][3] =
            {
                { Clocks::RP2C02_FPS, Clocks::NTSC_CLK, Clocks::RP2A03_CC * Clocks::NTSC_DIV },
                { Clocks::RP2C07_FPS, Clocks::PAL_CLK, Clocks::RP2A07_CC * Clocks::PAL_DIV }
            };
            
            dword sampleRate = settings.rate;
            
            if (settings.transpose && settings.speed)
                sampleRate = sampleRate * clocks[cycles.region][0] / settings.speed;
            
            uint multiplier = 0;
            const dword masterRate = clocks[cycles.region][1];
            
            while (++multiplier < 0x1000 && qword(masterRate) * (multiplier+1) / sampleRate <= 0x7FFFF && qword(masterRate) * multiplier % sampleRate);
            
            rate = qword(masterRate) * multiplier / sampleRate;
            fixed = clocks[cycles.region][2] * multiplier;
        }
        
        void Apu::SaveState(State::Saver& state,const dword baseChunk) const
        {
            state.Begin( baseChunk );
            
            {
                Cycle clock = cycles.frameCounter / cycles.fixed;
                
                ((void)0);
                
                if (clock > cpu.GetCycles())
                    clock = (clock - cpu.GetCycles()) / cpu.GetClock();
                else
                    clock = 0;
                
                ((void)0);
                
                const byte data[4] =
                {
                    ctrl,
                    clock & 0xFF,
                    clock >> 8,
                    cycles.frameDivider
                };
                
                state.Begin( AsciiId<'F','R','M'>::V ).Write( data ).End();
            }
            
            if (cycles.frameIrqClock != Cpu::CYCLE_MAX)
            {
                Cycle clock = cycles.frameIrqClock;
                
                ((void)0);
                
                if (clock > cpu.GetCycles())
                    clock = (clock - cpu.GetCycles()) / cpu.GetClock();
                else
                    clock = 0;
                
                ((void)0);
                
                const byte data[3] =
                {
                    clock & 0xFF,
                    clock >> 8,
                    cycles.frameIrqRepeat % 3
                };
                
                state.Begin( AsciiId<'I','R','Q'>::V ).Write( data ).End();
            }
            
            if (cycles.extCounter != Cpu::CYCLE_MAX)
            {
                Cycle clock = cycles.extCounter / cycles.fixed;
                
                ((void)0);
                
                if (clock > cpu.GetCycles())
                {
                    clock = (clock - cpu.GetCycles()) / cpu.GetClock();
                    ((void)0);
                }
                else
                {
                    clock = 0;
                }
                
                state.Begin( AsciiId<'E','X','T'>::V ).Write16( clock ).End();
            }
            
            square[0].SaveState( state, AsciiId<'S','Q','0'>::V );
            square[1].SaveState( state, AsciiId<'S','Q','1'>::V );
            triangle.SaveState( state, AsciiId<'T','R','I'>::V );
            noise.SaveState( state, AsciiId<'N','O','I'>::V );
            dmc.SaveState( state, AsciiId<'D','M','C'>::V, cpu, cycles.dmcClock );
            
            state.End();
        }
        
        void Apu::LoadState(State::Loader& state)
        {
            cycles.frameIrqClock = Cpu::CYCLE_MAX;
            cycles.frameIrqRepeat = 0;
            
            while (const dword chunk = state.Begin())
            {
                switch (chunk)
                {
                    case AsciiId<'F','R','M'>::V:
                    {
                        State::Loader::Data<4> data( state );
                        
                        ctrl = data[0] & STATUS_BITS;
                        
                        cycles.rateCounter = cycles.fixed * cpu.GetCycles();
                        
                        cycles.frameCounter = cycles.fixed *
                        (
                         cpu.GetCycles() + (data[1] | data[2] << 8) * cpu.GetClock()
                         );
                        
                        cycles.frameDivider = data[3] & 0x3;
                        break;
                    }
                        
                    case AsciiId<'I','R','Q'>::V:
                    {
                        State::Loader::Data<3> data( state );
                        
                        cycles.frameIrqClock = cpu.GetCycles() +
                        (
                         (data[0] | data[1] << 8) * cpu.GetClock()
                         );
                        
                        cycles.frameIrqRepeat = (data[2] & 0x3) % 3;
                        break;
                    }
                        
                    case AsciiId<'E','X','T'>::V:
                        
                        ((void)0);
                        
                        if (cycles.extCounter != Cpu::CYCLE_MAX)
                        {
                            cycles.extCounter = cycles.fixed *
                            (
                             cpu.GetCycles() + state.Read16() * cpu.GetClock()
                             );
                        }
                        break;
                        
                    case AsciiId<'S','Q','0'>::V:
                        
                        square[0].LoadState( state );
                        break;
                        
                    case AsciiId<'S','Q','1'>::V:
                        
                        square[1].LoadState( state );
                        break;
                        
                    case AsciiId<'T','R','I'>::V:
                        
                        triangle.LoadState( state );
                        break;
                        
                    case AsciiId<'N','O','I'>::V:
                        
                        noise.LoadState( state, cycles.region );
                        break;
                        
                    case AsciiId<'D','M','C'>::V:
                        
                        dmc.LoadState( state, cpu, cycles.region, cycles.dmcClock );
                        break;
                }
                
                state.End();
            }
            
            if (ctrl != STATUS_FRAME_IRQ_ENABLE)
            {
                cycles.frameIrqClock = Cpu::CYCLE_MAX;
                cycles.frameIrqRepeat = 0;
            }
            else if (cycles.frameIrqClock == Cpu::CYCLE_MAX)
            {
                cycles.frameIrqClock = (cycles.frameCounter / cycles.fixed) + (3 - cycles.frameDivider) * (Cycles::frameClocks[cycles.region][0] / 4);
                cycles.frameIrqRepeat = 0;
            }
        }
        
        
        
        
        
        void Apu::SyncOn(const Cycle target)
        {
            (__builtin_expect(!(!!((stream && settings.audible) && (cycles.rate && cycles.fixed) && (cycles.extCounter == Cpu::CYCLE_MAX))), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstApu.cpp", 584, "!!((stream && settings.audible) && (cycles.rate && cycles.fixed) && (cycles.extCounter == Cpu::CYCLE_MAX))") : (void)0);
            
            if (cycles.rateCounter < target)
            {
                Cycle rateCounter = cycles.rateCounter;
                const Cycle rate = cycles.rate;
                
                do
                {
                    buffer << GetSample();
                    
                    if (cycles.frameCounter <= rateCounter)
                        ClockFrameCounter();
                    
                    rateCounter += rate;
                }
                while (rateCounter < target);
                
                cycles.rateCounter = rateCounter;
            }
            
            if (cycles.frameCounter < target)
            {
                ClockFrameCounter();
                (__builtin_expect(!(!!(cycles.frameCounter >= target)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstApu.cpp", 608, "!!(cycles.frameCounter >= target)") : (void)0);
            }
        }
        
        void Apu::SyncOnExt(const Cycle target)
        {
            (__builtin_expect(!(!!((stream && settings.audible) && (cycles.rate && cycles.fixed) && extChannel)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstApu.cpp", 614, "!!((stream && settings.audible) && (cycles.rate && cycles.fixed) && extChannel)") : (void)0);
            
            Cycle extCounter = cycles.extCounter;
            
            if (cycles.rateCounter < target)
            {
                Cycle rateCounter = cycles.rateCounter;
                
                do
                {
                    buffer << GetSample();
                    
                    if (extCounter <= rateCounter)
                        extCounter = extChannel->Clock( extCounter, cycles.fixed, rateCounter );
                    
                    if (cycles.frameCounter <= rateCounter)
                        ClockFrameCounter();
                    
                    rateCounter += cycles.rate;
                }
                while (rateCounter < target);
                
                cycles.rateCounter = rateCounter;
            }
            
            if (extCounter <= target)
            {
                cycles.extCounter = extChannel->Clock( extCounter, cycles.fixed, target );
                (__builtin_expect(!(!!(cycles.extCounter > target)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstApu.cpp", 642, "!!(cycles.extCounter > target)") : (void)0);
            }
            else
            {
                cycles.extCounter = extCounter;
            }
            
            if (cycles.frameCounter < target)
            {
                ClockFrameCounter();
                (__builtin_expect(!(!!(cycles.frameCounter >= target)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstApu.cpp", 652, "!!(cycles.frameCounter >= target)") : (void)0);
            }
        }
        
        void Apu::SyncOff(const Cycle target)
        {
            (__builtin_expect(!(!!(!(stream && settings.audible) && cycles.fixed)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstApu.cpp", 658, "!!(!(stream && settings.audible) && cycles.fixed)") : (void)0);
            
            cycles.rateCounter = target;
            
            while (cycles.frameCounter < target)
                ClockFrameCounter();
            
            (__builtin_expect(!(!!(cycles.extCounter == Cpu::CYCLE_MAX || extChannel)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstApu.cpp", 665, "!!(cycles.extCounter == Cpu::CYCLE_MAX || extChannel)") : (void)0);
            
            if (cycles.extCounter <= target)
            {
                cycles.extCounter = extChannel->Clock( cycles.extCounter, cycles.fixed, target );
                (__builtin_expect(!(!!(cycles.extCounter > target)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstApu.cpp", 670, "!!(cycles.extCounter > target)") : (void)0);
            }
        }
        
        void Apu::BeginFrame(Sound::Output* output)
        {
            stream = output;
            updater = (output && settings.audible ? (cycles.extCounter == Cpu::CYCLE_MAX ? &Apu::SyncOn : &Apu::SyncOnExt) : &Apu::SyncOff);
        }
        
        inline void Apu::Update(const Cycle target)
        {
            (__builtin_expect(!(!!(cycles.fixed)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstApu.cpp", 682, "!!(cycles.fixed)") : (void)0);
            (*this.*updater)( target );
        }
        
        inline void Apu::Update()
        {
            Update( cpu.GetCycles() * cycles.fixed );
        }
        
        inline void Apu::UpdateLatency()
        {
            Update( (cpu.GetCycles() + 1) * cycles.fixed );
        }
        
        inline Cycle Apu::UpdateDelta()
        {
            const Cycle elapsed = cpu.GetCycles() * cycles.fixed;
            const Cycle delta = cycles.frameCounter - elapsed;
            Update( elapsed + 1 );
            return delta;
        }
        
        template<typename T,bool STEREO>
        void Apu::FlushSound()
        {
            (__builtin_expect(!(!!((stream && settings.audible) && (cycles.rate && cycles.fixed))), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstApu.cpp", 707, "!!((stream && settings.audible) && (cycles.rate && cycles.fixed))") : (void)0);
            
            for (uint i=0; i < 2; ++i)
            {
                if (stream->length[i] && stream->samples[i])
                {
                    Sound::Buffer::Block block( stream->length[i] );
                    buffer >> block;
                    
                    Sound::Buffer::Renderer<T,STEREO> output( stream->samples[i], stream->length[i], buffer.history );
                    
                    if (output << block)
                    {
                        const Cycle target = cpu.GetCycles() * cycles.fixed;
                        
                        if (cycles.rateCounter < target)
                        {
                            Cycle rateCounter = cycles.rateCounter;
                            
                            do
                            {
                                output << GetSample();
                                
                                if (cycles.frameCounter <= rateCounter)
                                    ClockFrameCounter();
                                
                                if (cycles.extCounter <= rateCounter)
                                    cycles.extCounter = extChannel->Clock( cycles.extCounter, cycles.fixed, rateCounter );
                                
                                rateCounter += cycles.rate;
                            }
                            while (rateCounter < target && output);
                            
                            cycles.rateCounter = rateCounter;
                        }
                        
                        if (output)
                        {
                            if (cycles.frameCounter < target)
                                ClockFrameCounter();
                            
                            if (cycles.extCounter <= target)
                                cycles.extCounter = extChannel->Clock( cycles.extCounter, cycles.fixed, target );
                            
                            do
                            {
                                output << GetSample();
                            }
                            while (output);
                        }
                    }
                }
            }
        }
        
        void Apu::EndFrame()
        {
            Clock( cpu.GetCycles() );
            
            (__builtin_expect(!(!!((stream && settings.audible) == (updater != &Apu::SyncOff))), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstApu.cpp", 766, "!!((stream && settings.audible) == (updater != &Apu::SyncOff))") : (void)0);
            
            if (updater != &Apu::SyncOff)
            {
                dword streamed = 0;
                
                if (Sound::Output::lockCallback( *stream ))
                {
                    streamed = stream->length[0] + stream->length[1];
                    
                    if (settings.bits == 16)
                    {
                        if (!settings.stereo)
                            FlushSound<iword,false>();
                        else
                            FlushSound<iword,true>();
                    }
                    else
                    {
                        if (!settings.stereo)
                            FlushSound<byte,false>();
                        else
                            FlushSound<byte,true>();
                    }
                    
                    Sound::Output::unlockCallback( *stream );
                }
                
                if (const dword rate = synchronizer.Clock( streamed, cycles.region, settings.rate ))
                    Resync( rate );
            }
            
            Update();
            
            Cycle frame = cpu.GetFrameCycles();
            
            (__builtin_expect(!(!!(cycles.dmcClock >= frame && cycles.frameIrqClock >= frame)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstApu.cpp", 806, "!!( cycles.dmcClock >= frame && cycles.frameIrqClock >= frame)") : (void)0);
            
            
            
            
            
            cycles.dmcClock -= frame;
            
            if (cycles.frameIrqClock != Cpu::CYCLE_MAX)
                cycles.frameIrqClock -= frame;
            
            frame *= cycles.fixed;
            
            (__builtin_expect(!(!!(cycles.rateCounter >= frame && cycles.frameCounter >= frame && cycles.extCounter >= frame)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstApu.cpp", 820, "!!( cycles.rateCounter >= frame && cycles.frameCounter >= frame && cycles.extCounter >= frame)") : (void)0);
            
            
            
            
            
            
            cycles.rateCounter -= frame;
            cycles.frameCounter -= frame;
            
            if (cycles.extCounter != Cpu::CYCLE_MAX)
                cycles.extCounter -= frame;
        }
        
        
        
        
        
        Apu::Settings::Settings()
        : rate(44100), bits(16), speed(0), transpose(false), stereo(false), audible(true)
        {
            for (uint i=0; i < MAX_CHANNELS; ++i)
                volumes[i] = Channel::DEFAULT_VOLUME;
        }
        
        Apu::Cycles::Cycles()
        : fixed(1), rate(1), region(Region::NTSC) {}
        
        void Apu::Cycles::Reset(const bool extChannel)
        {
            rateCounter = 0;
            frameDivider = 0;
            frameIrqClock = Cpu::CYCLE_MAX;
            frameIrqRepeat = 0;
            dmcClock = Dmc::GetResetFrequency( region );
            frameCounter = frameClocks[region][0] * fixed;
            extCounter = (extChannel ? 0UL : Cpu::CYCLE_MAX);
        }
        
        void Apu::Cycles::Update(dword sampleRate,const uint speed,const Region::Type r)
        {
            frameCounter /= fixed;
            rateCounter /= fixed;
            
            if (extCounter != Cpu::CYCLE_MAX)
                extCounter /= fixed;
            
            if (region != r)
            {
                region = r;
                
                frameCounter = Cpu::ClockConvert( frameCounter, r );
                rateCounter = Cpu::ClockConvert( rateCounter, r );
                
                if (extCounter != Cpu::CYCLE_MAX)
                    extCounter = Cpu::ClockConvert( extCounter, r );
                
                if (frameIrqClock != Cpu::CYCLE_MAX)
                    frameIrqClock = Cpu::ClockConvert( frameIrqClock, r );
            }
            
            static const dword clocks[2][3] =
            {
                { Clocks::RP2C02_FPS, Clocks::NTSC_CLK, Clocks::NTSC_DIV },
                { Clocks::RP2C07_FPS, Clocks::PAL_CLK, Clocks::PAL_DIV }
            };
            
            if (speed)
                sampleRate = sampleRate * clocks[region][0] / speed;
            
            uint multiplier = 0;
            const dword masterRate = clocks[region][1];
            
            while (++multiplier < 512 && qword(masterRate) * multiplier % sampleRate);
            
            rate = qword(masterRate) * multiplier / sampleRate;
            fixed = clocks[region][2] * multiplier;
            
            frameCounter *= fixed;
            rateCounter *= fixed;
            
            if (extCounter != Cpu::CYCLE_MAX)
                extCounter *= fixed;
        }
        
        Apu::Synchronizer::Synchronizer()
        : rate(0) {}
        
        void Apu::Synchronizer::Resync(uint speed,Region::Type region)
        {
            duty = 0;
            streamed = 0;
            
            if (speed == 0 || speed == uint(region == Region::NTSC ? Clocks::RP2C02_FPS : Clocks::RP2C07_FPS))
                sync = 4;
            else
                sync = 0;
        }
        
        void Apu::Synchronizer::Reset(uint speed,Region::Type region,dword sampleRate)
        {
            rate = sampleRate;
            Resync( speed, region );
        }
        
        
        
        
        
        inline dword Apu::Synchronizer::Clock
        (
         const dword output,
         const Region::Type region,
         const dword sampleRate
         )
        {
            if (sync)
            {
                if (duty >= 60*4)
                    streamed += output;
                
                if (duty < 60*12)
                {
                    duty++;
                }
                else
                {
                    duty = 60*4;
                    
                    dword actualRate = streamed / (60*8) * (region == Region::NTSC ? Clocks::RP2C02_FPS : Clocks::RP2C07_FPS);
                    const dword limit = sampleRate / 21;
                    
                    if (actualRate <= sampleRate-limit)
                    {
                        actualRate = sampleRate-limit;
                        sync--;
                    }
                    else if (actualRate >= sampleRate+limit)
                    {
                        actualRate = sampleRate+limit;
                        sync--;
                    }
                    else
                    {
                        sync = (sync > 2 ? sync - 2 : 0);
                    }
                    
                    actualRate = actualRate * 9999 / 10000;
                    streamed = 0;
                    
                    if (rate != actualRate)
                    {
                        rate = actualRate;
                        return actualRate;
                    }
                }
            }
            
            return 0;
        }
        
        
        
        
        
        Apu::Channel::LengthCounter::LengthCounter()
        {
            Reset();
        }
        
        void Apu::Channel::LengthCounter::Reset()
        {
            enabled = 0;
            count = 0;
        }
        
        void Apu::Channel::LengthCounter::LoadState(State::Loader& state)
        {
            const uint data = state.Read8();
            enabled = (data == 0xFF ? 0U : ~0U);
            count = data & enabled;
        }
        
        void Apu::Channel::LengthCounter::SaveState(State::Saver& state,const dword chunk) const
        {
            ((void)0);
            state.Begin( chunk ).Write8( enabled ? count : 0xFF ).End();
        }
        
        Apu::Channel::Envelope::Envelope()
        : outputVolume(OUTPUT_MUL)
        {
            Reset();
        }
        
        void Apu::Channel::Envelope::Reset()
        {
            output = 0;
            regs[0] = 0x0;
            regs[1] = 0x10;
            count = 0;
            reset = false;
        }
        
        void Apu::Channel::Envelope::SetOutputVolume(uint v)
        {
            outputVolume = v;
            UpdateOutput();
        }
        
        void Apu::Channel::Envelope::SaveState(State::Saver& state,const dword chunk) const
        {
            const byte data[3] =
            {
                count,
                regs[0] | (reset ? 0x80U : 0x00U),
                regs[1]
            };
            
            state.Begin( chunk ).Write( data ).End();
        }
        
        void Apu::Channel::Envelope::LoadState(State::Loader& state)
        {
            State::Loader::Data<3> data( state );
            
            count = data[0] & 0x0F;
            reset = data[1] >> 7;
            regs[0] = data[1] & 0x0F;
            regs[1] = data[2];
            
            UpdateOutput();
        }
        
        
        
        
        
        void Apu::Channel::Envelope::UpdateOutput()
        {
            output = (regs[regs[1] >> 4 & 1U] & 0xFUL) * outputVolume;
        }
        
        void Apu::Channel::Envelope::Clock()
        {
            if (!reset)
            {
                if (count)
                {
                    count--;
                    return;
                }
                
                if (regs[0] | (regs[1] & 0x20U))
                    regs[0] = (regs[0] - 1U) & 0xF;
            }
            else
            {
                reset = false;
                regs[0] = 0xF;
            }
            
            count = regs[1] & 0x0FU;
            UpdateOutput();
        }
        
        void Apu::Channel::Envelope::Write(const uint data)
        {
            regs[1] = data;
            UpdateOutput();
        }
        
        
        
        
        
        Apu::Channel::DcBlocker::DcBlocker()
        {
            Reset();
        }
        
        void Apu::Channel::DcBlocker::Reset()
        {
            acc = 0;
            prev = 0;
            next = 0;
        }
        
        
        
        
        
        Apu::Channel::Sample Apu::Channel::DcBlocker::Apply(Sample sample)
        {
            acc -= prev;
            prev = signed_shl(sample,15);
            acc += prev - next * POLE;
            next = signed_shr(acc,15);
            return next;
        }
        
        
        
        
        
        Apu::Channel::Channel(Apu& a)
        : apu(a) {}
        
        Apu::Channel::~Channel()
        {
            if (apu.extChannel == this)
            {
                apu.extChannel = __null;
                apu.UpdateVolumes();
            }
        }
        
        void Apu::Channel::Connect(bool audible)
        {
            (__builtin_expect(!(!!(apu.extChannel == __null)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstApu.cpp", 1134, "!!(apu.extChannel == __null)") : (void)0);
            
            if (audible)
                apu.settings.audible = true;
            else
                apu.UpdateVolumes();
            
            apu.extChannel = this;
        }
        
        void Apu::Channel::GetOscillatorClock(Cycle& rate,uint& fixed) const
        {
            apu.CalculateOscillatorClock( rate, fixed );
        }
        
        uint Apu::Channel::GetVolume(uint channel) const
        {
            (__builtin_expect(!(!!(channel < MAX_CHANNELS)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstApu.cpp", 1151, "!!(channel < MAX_CHANNELS)") : (void)0);
            return apu.settings.volumes[channel];
        }
        
        
        
        
        
        Region::Type Apu::Channel::GetRegion() const
        {
            return apu.cycles.region;
        }
        
        dword Apu::Channel::GetSampleRate() const
        {
            return apu.settings.rate;
        }
        
        void Apu::Channel::Update() const
        {
            apu.Update();
        }
        
        Cycle Apu::Channel::Clock(Cycle,Cycle,Cycle)
        {
            return Cpu::CYCLE_MAX;
        }
        
        
        
        
        
        Apu::Oscillator::Oscillator()
        : rate(1), fixed(1) {}
        
        void Apu::Oscillator::Reset()
        {
            active = false;
            timer = RESET_CYCLES * fixed;
            frequency = fixed;
            amp = 0;
        }
        
        inline void Apu::Oscillator::ClearAmp()
        {
            amp = 0;
        }
        
        void Apu::Oscillator::UpdateSettings(dword r,uint f)
        {
            (__builtin_expect(!(!!(r && f)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstApu.cpp", 1201, "!!(r && f)") : (void)0);
            
            frequency = frequency / fixed * f;
            timer = timer / fixed * f;
            fixed = f;
            rate = r;
        }
        
        void Apu::Square::Reset()
        {
            Oscillator::Reset();
            
            frequency = fixed * 2;
            step = 0;
            duty = 0;
            
            envelope.Reset();
            lengthCounter.Reset();
            
            validFrequency = false;
            
            sweepRate = 0;
            sweepCount = 1;
            sweepReload = false;
            sweepIncrease = ~0U;
            sweepShift = 0;
            
            waveLength = 0;
        }
        
        
        
        
        
        inline bool Apu::Square::CanOutput() const
        {
            return lengthCounter.GetCount() && envelope.Volume() && validFrequency;
        }
        
        
        
        
        
        void Apu::Square::UpdateSettings(dword r,uint f,uint v)
        {
            Oscillator::UpdateSettings( r, f );
            envelope.SetOutputVolume( v );
            active = CanOutput();
        }
        
        void Apu::Square::SaveState(State::Saver& state,const dword chunk) const
        {
            state.Begin( chunk );
            
            {
                byte data[4];
                
                data[0] = waveLength & 0xFFU;
                data[1] = (waveLength >> 8) | (duty ? duty << (2+3) : 2U << 3);
                data[2] = (sweepCount - 1U) << 4;
                
                if (sweepRate)
                    data[2] |= 0x08U | (sweepRate - 1);
                
                if (sweepReload)
                    data[2] |= 0x80U;
                
                data[3] = sweepShift;
                
                if (!sweepIncrease)
                    data[3] |= 0x08U;
                
                state.Begin( AsciiId<'R','E','G'>::V ).Write( data ).End();
            }
            
            lengthCounter.SaveState( state, AsciiId<'L','E','N'>::V );
            envelope.SaveState( state, AsciiId<'E','N','V'>::V );
            
            state.End();
        }
        
        void Apu::Square::LoadState(State::Loader& state)
        {
            while (const dword chunk = state.Begin())
            {
                switch (chunk)
                {
                    case AsciiId<'R','E','G'>::V:
                    {
                        State::Loader::Data<4> data( state );
                        
                        waveLength = data[0] | (data[1] << 8 & 0x0700);
                        
                        
                        switch (data[1] >> 3 & 0xF)
                        {
                            case 4: duty = 1; break;
                            case 8: duty = 2; break;
                            case 12: duty = 3; break;
                            default: duty = 0; break;
                        }
                        
                        if (data[2] & 0x08)
                            sweepRate = (data[2] & 0x07) + 1;
                        else
                            sweepRate = 0;
                        
                        sweepCount = (data[2] >> 4 & 0x07) + 1;
                        sweepReload = data[2] >> 7;
                        sweepShift = data[3] & 0x07;
                        sweepIncrease = (data[3] & 0x08) ? 0U : ~0U;
                        break;
                    }
                        
                    case AsciiId<'L','E','N'>::V:
                        
                        lengthCounter.LoadState( state );
                        break;
                        
                    case AsciiId<'E','N','V'>::V:
                        
                        envelope.LoadState( state );
                        break;
                }
                
                state.End();
            }
            
            step = 0;
            timer = 0;
            UpdateFrequency();
        }
        
        
        
        
        
        inline void Apu::Square::Disable(const uint disable)
        {
            active &= lengthCounter.Disable( disable );
        }
        
        void Apu::Square::UpdateFrequency()
        {
            if (waveLength >= MIN_FRQ && waveLength + (sweepIncrease & waveLength >> sweepShift) <= MAX_FRQ)
            {
                frequency = (waveLength + 1UL) * 2 * fixed;
                validFrequency = true;
                active = lengthCounter.GetCount() && envelope.Volume();
            }
            else
            {
                validFrequency = false;
                active = false;
            }
        }
        
        inline void Apu::Square::WriteReg0(const uint data)
        {
            envelope.Write( data );
            duty = data >> REG0_DUTY_SHIFT;
            active = CanOutput();
        }
        
        inline void Apu::Square::WriteReg1(const uint data)
        {
            sweepIncrease = (data & REG1_SWEEP_DECREASE) ? 0U : ~0U;
            sweepShift = data & REG1_SWEEP_SHIFT;
            sweepRate = 0;
            
            if ((data & (REG1_SWEEP_ENABLED|REG1_SWEEP_SHIFT)) > REG1_SWEEP_ENABLED)
            {
                sweepRate = ((data & REG1_SWEEP_RATE) >> REG1_SWEEP_RATE_SHIFT) + 1;
                sweepReload = true;
            }
            
            UpdateFrequency();
        }
        
        inline void Apu::Square::WriteReg2(const uint data)
        {
            waveLength = (waveLength & uint(REG3_WAVELENGTH_HIGH)) | (data & REG3_WAVELENGTH_LOW);
            
            UpdateFrequency();
        }
        
        inline void Apu::Square::WriteReg3(const uint data,const Cycle frameCounterDelta)
        {
            step = 0;
            
            envelope.ResetClock();
            lengthCounter.Write( data, frameCounterDelta );
            
            waveLength = (data << 8 & REG3_WAVELENGTH_HIGH) | (waveLength & uint(REG3_WAVELENGTH_LOW));
            
            UpdateFrequency();
        }
        
        inline void Apu::Square::ClockEnvelope()
        {
            envelope.Clock();
            active = CanOutput();
        }
        
        inline void Apu::Square::ClockSweep(const uint complement)
        {
            if (!envelope.Looping() && lengthCounter.Clock())
                active = false;
            
            if (sweepRate && !--sweepCount)
            {
                sweepCount = sweepRate;
                
                if (waveLength >= MIN_FRQ)
                {
                    const uint shifted = waveLength >> sweepShift;
                    
                    if (!sweepIncrease)
                    {
                        waveLength += complement - shifted;
                        UpdateFrequency();
                    }
                    else if (waveLength + shifted <= MAX_FRQ)
                    {
                        waveLength += shifted;
                        UpdateFrequency();
                    }
                }
            }
            
            if (sweepReload)
            {
                sweepReload = false;
                sweepCount = sweepRate;
            }
        }
        
        inline uint Apu::Square::GetLengthCounter() const
        {
            return lengthCounter.GetCount();
        }
        
        dword Apu::Square::GetSample()
        {
            ((void)0);
            
            dword sum = timer;
            timer -= idword(rate);
            
            if (active)
            {
                static const byte forms[4][8] =
                {
                    {0x1F,0x00,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F},
                    {0x1F,0x00,0x00,0x1F,0x1F,0x1F,0x1F,0x1F},
                    {0x1F,0x00,0x00,0x00,0x00,0x1F,0x1F,0x1F},
                    {0x00,0x1F,0x1F,0x00,0x00,0x00,0x00,0x00}
                };
                
                const byte* const __restrict__ form = forms[duty];
                
                if (timer >= 0)
                {
                    amp = dword(envelope.Volume()) >> form[step];
                }
                else
                {
                    sum >>= form[step];
                    
                    do
                    {
                        sum += ((dword(-timer)) < (frequency) ? (dword(-timer)) : (frequency)) >> form[step = (step + 1) & 0x7];
                        timer += idword(frequency);
                    }
                    while (timer < 0);
                    
                    ((void)0);
                    amp = (sum * envelope.Volume() + rate/2) / rate;
                }
            }
            else
            {
                if (timer < 0)
                {
                    const uint count = (dword(-timer) + frequency - 1) / frequency;
                    step = (step + count) & 0x7;
                    timer += idword(count * frequency);
                }
                
                if (amp < Channel::OUTPUT_DECAY)
                {
                    return 0;
                }
                else
                {
                    amp -= Channel::OUTPUT_DECAY;
                }
            }
            
            return amp;
        }
        
        
        
        
        
        Apu::Triangle::Triangle()
        : outputVolume(0) {}
        
        void Apu::Triangle::Reset()
        {
            Oscillator::Reset();
            
            step = 0x7;
            status = STATUS_COUNTING;
            waveLength = 0;
            linearCtrl = 0;
            linearCounter = 0;
            
            lengthCounter.Reset();
        }
        
        
        
        
        
        inline bool Apu::Triangle::CanOutput() const
        {
            return lengthCounter.GetCount() && linearCounter && waveLength >= MIN_FRQ && outputVolume;
        }
        
        
        
        
        
        void Apu::Triangle::UpdateSettings(dword r,uint f,uint v)
        {
            Oscillator::UpdateSettings( r, f );
            outputVolume = v;
            active = CanOutput();
        }
        
        void Apu::Triangle::SaveState(State::Saver& state,const dword chunk) const
        {
            state.Begin( chunk );
            
            {
                const byte data[4] =
                {
                    waveLength & 0xFFU,
                    waveLength >> 8,
                    linearCounter | (uint(status) << 7),
                    linearCtrl
                };
                
                state.Begin( AsciiId<'R','E','G'>::V ).Write( data ).End();
            }
            
            lengthCounter.SaveState( state, AsciiId<'L','E','N'>::V );
            
            state.End();
        }
        
        void Apu::Triangle::LoadState(State::Loader& state)
        {
            while (const dword chunk = state.Begin())
            {
                switch (chunk)
                {
                    case AsciiId<'R','E','G'>::V:
                    {
                        State::Loader::Data<4> data( state );
                        
                        waveLength = data[0] | (data[1] << 8 & 0x0700);
                        linearCounter = data[2] & 0x7F;
                        status = static_cast<Status>(data[2] >> 7);
                        linearCtrl = data[3];
                        
                        frequency = (waveLength + 1UL) * fixed;
                        break;
                    }
                        
                    case AsciiId<'L','E','N'>::V:
                        
                        lengthCounter.LoadState( state );
                        break;
                }
                
                state.End();
            }
            
            timer = 0;
            step = 0;
            active = CanOutput();
        }
        
        
        
        
        
        inline void Apu::Triangle::Disable(const uint disable)
        {
            active &= lengthCounter.Disable( disable );
        }
        
        inline void Apu::Triangle::WriteReg0(const uint data)
        {
            linearCtrl = data;
        }
        
        inline void Apu::Triangle::WriteReg2(const uint data)
        {
            waveLength = (waveLength & uint(REG3_WAVE_LENGTH_HIGH)) | (data & REG2_WAVE_LENGTH_LOW);
            frequency = (waveLength + 1UL) * fixed;
            
            active = CanOutput();
        }
        
        inline void Apu::Triangle::WriteReg3(const uint data,const Cycle frameCounterDelta)
        {
            waveLength = (data << 8 & REG3_WAVE_LENGTH_HIGH) | (waveLength & uint(REG2_WAVE_LENGTH_LOW));
            frequency = (waveLength + 1UL) * fixed;
            
            status = STATUS_RELOAD;
            lengthCounter.Write( data, frameCounterDelta );
            
            active = CanOutput();
        }
        
        inline void Apu::Triangle::ClockLinearCounter()
        {
            if (status == STATUS_COUNTING)
            {
                if (linearCounter && !--linearCounter)
                    active = false;
            }
            else
            {
                if (!(linearCtrl & uint(REG0_LINEAR_COUNTER_START)))
                    status = STATUS_COUNTING;
                
                linearCounter = linearCtrl & uint(REG0_LINEAR_COUNTER_LOAD);
                active = CanOutput();
            }
        }
        
        inline void Apu::Triangle::ClockLengthCounter()
        {
            if (!(linearCtrl & uint(REG0_LINEAR_COUNTER_START)) && lengthCounter.Clock())
                active = false;
        }
        
        inline dword Apu::Triangle::GetSample()
        {
            ((void)0);
            
            if (active)
            {
                static const byte pyramid[32] =
                {
                    0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,
                    0x8,0x9,0xA,0xB,0xC,0xD,0xE,0xF,
                    0xF,0xE,0xD,0xC,0xB,0xA,0x9,0x8,
                    0x7,0x6,0x5,0x4,0x3,0x2,0x1,0x0
                };
                
                dword sum = timer;
                timer -= idword(rate);
                
                if (timer >= 0)
                {
                    amp = pyramid[step] * outputVolume * 3;
                }
                else
                {
                    sum *= pyramid[step];
                    
                    do
                    {
                        sum += ((dword(-timer)) < (frequency) ? (dword(-timer)) : (frequency)) * pyramid[step = (step + 1) & 0x1F];
                        timer += idword(frequency);
                    }
                    while (timer < 0);
                    
                    ((void)0);
                    amp = (sum * outputVolume + rate/2) / rate * 3;
                }
            }
            else if (amp < Channel::OUTPUT_DECAY)
            {
                return 0;
            }
            else
            {
                amp -= Channel::OUTPUT_DECAY;
                step &= STEP_CHECK;
            }
            
            return amp;
        }
        
        inline uint Apu::Triangle::GetLengthCounter() const
        {
            return lengthCounter.GetCount();
        }
        
        
        
        
        
        void Apu::Noise::Reset(const Region::Type region)
        {
            Oscillator::Reset();
            
            frequency = lut[region][0] * dword(fixed);
            
            bits = 1;
            shifter = 13;
            
            envelope.Reset();
            lengthCounter.Reset();
        }
        
        uint Apu::Noise::GetFrequencyIndex() const
        {
            for (uint v=frequency/fixed, i=0; i < 16; ++i)
            {
                if (v == lut[0][i] || v == lut[1][i])
                    return i;
            }
            
            return 0;
        }
        
        
        
        
        
        inline bool Apu::Noise::CanOutput() const
        {
            return lengthCounter.GetCount() && envelope.Volume();
        }
        
        
        
        
        
        void Apu::Noise::UpdateSettings(dword r,uint f,uint v,Region::Type region)
        {
            Oscillator::UpdateSettings( r, f );
            
            const dword newFrequency = lut[region][GetFrequencyIndex()] * dword(fixed);
            
            if (frequency != newFrequency)
            {
                frequency = newFrequency;
                timer = 0;
            }
            
            envelope.SetOutputVolume( v );
            active = CanOutput();
        }
        
        void Apu::Noise::SaveState(State::Saver& state,const dword chunk) const
        {
            state.Begin( chunk );
            
            state.Begin( AsciiId<'R','E','G'>::V ).Write8( (shifter == 8 ? 0x10 : 0x00) | GetFrequencyIndex() ).End();
            lengthCounter.SaveState( state, AsciiId<'L','E','N'>::V );
            envelope.SaveState( state, AsciiId<'E','N','V'>::V );
            
            state.End();
        }
        
        void Apu::Noise::LoadState(State::Loader& state,const Region::Type region)
        {
            while (const dword chunk = state.Begin())
            {
                switch (chunk)
                {
                    case AsciiId<'R','E','G'>::V:
                    {
                        const uint data = state.Read8();
                        
                        frequency = lut[region][data & 0x0F] * dword(fixed);
                        shifter = (data & 0x10) ? 8 : 13;
                        break;
                    }
                        
                    case AsciiId<'L','E','N'>::V:
                        
                        lengthCounter.LoadState( state );
                        break;
                        
                    case AsciiId<'E','N','V'>::V:
                        
                        envelope.LoadState( state );
                        break;
                }
                
                state.End();
            }
            
            timer = 0;
            bits = 1;
            active = CanOutput();
        }
        
        
        
        
        
        inline void Apu::Noise::Disable(const uint disable)
        {
            active &= lengthCounter.Disable( disable );
        }
        
        inline void Apu::Noise::WriteReg0(const uint data)
        {
            envelope.Write( data );
            active = CanOutput();
        }
        
        inline void Apu::Noise::WriteReg2(const uint data,const Region::Type region)
        {
            frequency = lut[region][data & REG2_FREQUENCY] * dword(fixed);
            shifter = (data & REG2_93BIT_MODE) ? 8 : 13;
        }
        
        inline void Apu::Noise::WriteReg3(const uint data,const Cycle frameCounterDelta)
        {
            envelope.ResetClock();
            lengthCounter.Write( data, frameCounterDelta );
            
            active = CanOutput();
        }
        
        inline void Apu::Noise::ClockEnvelope()
        {
            envelope.Clock();
            active = CanOutput();
        }
        
        inline void Apu::Noise::ClockLengthCounter()
        {
            if (!envelope.Looping() && lengthCounter.Clock())
                active = false;
        }
        
        inline dword Apu::Noise::GetSample()
        {
            ((void)0);
            
            dword sum = timer;
            timer -= idword(rate);
            
            if (active)
            {
                if (timer >= 0)
                {
                    if (!(bits & 0x4000))
                        return envelope.Volume() * 2;
                }
                else
                {
                    if (bits & 0x4000)
                        sum = 0;
                    
                    do
                    {
                        bits = (bits << 1) | ((bits >> 14 ^ bits >> shifter) & 0x1);
                        
                        if (!(bits & 0x4000))
                            sum += ((dword(-timer)) < (frequency) ? (dword(-timer)) : (frequency));
                        
                        timer += idword(frequency);
                    }
                    while (timer < 0);
                    
                    ((void)0);
                    return (sum * envelope.Volume() + rate/2) / rate * 2;
                }
            }
            else while (timer < 0)
            {
                bits = (bits << 1) | ((bits >> 14 ^ bits >> shifter) & 0x1);
                timer += idword(frequency);
            }
            
            return 0;
        }
        
        inline uint Apu::Noise::GetLengthCounter() const
        {
            return lengthCounter.GetCount();
        }
        
        
        
        
        
        Apu::Dmc::Dmc()
        : outputVolume(0)
        {
            frequency = GetResetFrequency( Region::NTSC );
        }
        
        void Apu::Dmc::Reset(const Region::Type region)
        {
            curSample = 0;
            linSample = 0;
            frequency = GetResetFrequency( region );
            regs.ctrl = 0;
            regs.lengthCounter = 1;
            regs.address = 0xC000;
            out.active = false;
            out.shifter = 0;
            out.dac = 0;
            out.buffer = 0x00;
            dma.lengthCounter = 0;
            dma.buffered = false;
            dma.address = 0xC000;
            dma.buffer = 0x00;
        }
        
        Cycle Apu::Dmc::GetResetFrequency(Region::Type region)
        {
            return lut[region][0];
        }
        
        void Apu::Dmc::UpdateSettings(Cycle& dmcClock,const uint newVolume,const Region::Type region)
        {
            if (frequency != lut[region][regs.ctrl & REG0_FREQUENCY])
            {
                frequency = lut[region][regs.ctrl & REG0_FREQUENCY];
                dmcClock = Cpu::ClockConvert( dmcClock, region );
            }
            
            if (outputVolume)
                linSample /= outputVolume;
            
            if (outputVolume)
                curSample /= outputVolume;
            
            linSample *= newVolume;
            curSample *= newVolume;
            outputVolume = newVolume;
            
            if (!newVolume)
                out.active = false;
        }
        
        inline void Apu::Dmc::ClearAmp()
        {
            curSample = 0;
            linSample = 0;
        }
        
        void Apu::Dmc::SaveState(State::Saver& state,const dword chunk,const Cpu& cpu,const Cycle dmcMcClock) const
        {
            ((void)0);
            
            dword dmcClock = dmcMcClock;
            
            if (dmcClock > cpu.GetCycles())
                dmcClock = (dmcClock - cpu.GetCycles()) / cpu.GetClock(1);
            else
                dmcClock = 0;
            
            ((void)0);
            
            byte data[12] =
            {
                dmcClock & 0xFF,
                dmcClock >> 8,
                (
                 ( ( regs.ctrl & REG0_FREQUENCY ) ) |
                 ( ( regs.ctrl & REG0_LOOP ) ? 0x10U : 0U ) |
                 ( ( regs.ctrl & REG0_IRQ_ENABLE ) ? 0x20U : 0U ) |
                 ( ( dma.lengthCounter ) ? 0x40U : 0U )
                 ),
                (regs.address - 0xC000U) >> 6,
                (regs.lengthCounter - 1U) >> 4,
                (dma.address >> 0 & 0xFFU),
                (dma.address >> 8 & 0x7FU) | (dma.buffered ? 0x80 : 0x00),
                dma.lengthCounter ? (dma.lengthCounter - 1U) >> 4 : 0,
                dma.buffer,
                7 - out.shifter,
                out.buffer,
                out.dac
            };
            
            state.Begin( chunk ).Begin( AsciiId<'R','E','G'>::V ).Write( data ).End().End();
        }
        
        void Apu::Dmc::LoadState(State::Loader& state,const Cpu& cpu,const Region::Type region,Cycle& dmcClock)
        {
            while (const dword chunk = state.Begin())
            {
                switch (chunk)
                {
                    case AsciiId<'R','E','G'>::V:
                    {
                        State::Loader::Data<12> data( state );
                        
                        dmcClock = cpu.GetCycles() + ((data[0] | data[1] << 8) * cpu.GetClock());
                        
                        regs.ctrl =
                        (
                         ( ( data[2] & 0x10 ) ? REG0_LOOP : 0U ) |
                         ( ( data[2] & 0x20 ) ? REG0_IRQ_ENABLE : 0U ) |
                         ( ( data[2] & REG0_FREQUENCY ) )
                         );
                        
                        frequency = lut[region][regs.ctrl & REG0_FREQUENCY];
                        regs.address = 0xC000 | (data[3] << 6);
                        regs.lengthCounter = (data[4] << 4) + 1;
                        dma.address = 0x8000 | data[5] | (data[6] << 8 & 0x7F00);
                        dma.buffered = data[6] >> 7;
                        dma.lengthCounter = (data[2] & 0x40) ? (data[7] << 4) + 1 : 0;
                        dma.buffer = data[8];
                        out.shifter = 7 - (data[9] & 0x7);
                        out.buffer = data[10];
                        out.dac = data[11] & 0x7F;
                        
                        curSample = out.dac * outputVolume;
                        linSample = curSample;
                        out.active = dma.buffered && outputVolume;
                        break;
                    }
                }
                
                state.End();
            }
        }
        
        
        
        
        
        inline void Apu::Dmc::Disable(const uint disable,Cpu& cpu)
        {
            cpu.ClearIRQ( Cpu::IRQ_DMC );
            
            if (disable)
            {
                dma.lengthCounter = 0;
            }
            else if (!dma.lengthCounter)
            {
                dma.lengthCounter = regs.lengthCounter;
                dma.address = regs.address;
                
                if (!dma.buffered)
                    DoDMA( cpu );
            }
        }
        
        inline dword Apu::Dmc::GetSample()
        {
            if (curSample != linSample)
            {
                const uint step = outputVolume * INP_STEP;
                
                if (curSample + step - linSample <= step*2)
                {
                    linSample = curSample;
                }
                else if (curSample > linSample)
                {
                    linSample += step;
                }
                else
                {
                    linSample -= step;
                }
            }
            
            return linSample;
        }
        
        void Apu::Dmc::DoDMA(Cpu& cpu)
        {
            ((void)0);
            
            dma.buffer = cpu.Peek( dma.address );
            cpu.StealCycles( cpu.GetClock(DMA_CYCLES) );
            dma.address = 0x8000 | ((dma.address + 1U) & 0x7FFF);
            dma.buffered = true;
            
            ((void)0);
            
            if (--dma.lengthCounter)
                return;
            
            
            
            if (regs.ctrl & REG0_LOOP)
            {
                
                dma.address = regs.address;
                dma.lengthCounter = regs.lengthCounter;
            }
            else if (regs.ctrl & REG0_IRQ_ENABLE)
            {
                cpu.DoIRQ( Cpu::IRQ_DMC );
            }
        }
        
        inline bool Apu::Dmc::WriteReg0(const uint data,const Region::Type region)
        {
            regs.ctrl = data;
            frequency = lut[region][data & REG0_FREQUENCY];
            return data & REG0_IRQ_ENABLE;
        }
        
        inline void Apu::Dmc::WriteReg1(const uint data)
        {
            out.dac = data & 0x7F;
            curSample = out.dac * outputVolume;
        }
        
        inline void Apu::Dmc::WriteReg2(const uint data)
        {
            regs.address = 0xC000 | (data << 6);
        }
        
        inline void Apu::Dmc::WriteReg3(const uint data)
        {
            regs.lengthCounter = (data << 4) + 1;
        }
        
        inline bool Apu::Dmc::ClockDAC()
        {
            if (out.active)
            {
                const uint next = out.dac + ((out.buffer & 0x1U) << 2) - 2;
                out.buffer >>= 1;
                
                if (next <= 0x7F && next != out.dac)
                {
                    out.dac = next;
                    return true;
                }
            }
            
            return false;
        }
        
        inline void Apu::Dmc::Update()
        {
            curSample = out.dac * outputVolume;
        }
        
        inline Cycle Apu::Dmc::ClockDMA(Cpu& cpu)
        {
            if (out.shifter)
            {
                out.shifter--;
            }
            else
            {
                out.shifter = 7;
                out.active = dma.buffered;
                
                if (out.active)
                {
                    
                    
                    
                    out.active = outputVolume;
                    dma.buffered = false;
                    out.buffer = dma.buffer;
                    
                    if (dma.lengthCounter)
                        DoDMA( cpu );
                }
            }
            
            return frequency;
        }
        
        inline uint Apu::Dmc::GetLengthCounter() const
        {
            return dma.lengthCounter;
        }
        
        
        
        
        
        void Apu::ClearBuffers()
        {
            ClearBuffers( true );
        }
        
        __attribute__((noinline)) void Apu::ClearBuffers(bool resync)
        {
            if (resync)
                synchronizer.Resync( settings.speed, cycles.region );
            
            square[0].ClearAmp();
            square[1].ClearAmp();
            triangle.ClearAmp();
            noise.ClearAmp();
            dmc.ClearAmp();
            
            dcBlocker.Reset();
            
            buffer.Reset( settings.bits, false );
        }
        
        
        
        
        
        void Apu::ClockOscillators(const bool twoClocks)
        {
            for (uint i=0; i < 2; ++i)
                square[i].ClockEnvelope();
            
            triangle.ClockLinearCounter();
            noise.ClockEnvelope();
            
            if (twoClocks)
            {
                for (uint i=0; i < 2; ++i)
                    square[i].ClockSweep( i-1 );
                
                triangle.ClockLengthCounter();
                noise.ClockLengthCounter();
            }
        }
        
        void Apu::ClockDmc(const Cycle target)
        {
            (__builtin_expect(!(!!(cycles.dmcClock <= target)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstApu.cpp", 2237, "!!(cycles.dmcClock <= target)") : (void)0);
            
            do
            {
                if (dmc.ClockDAC())
                {
                    Update( cycles.dmcClock * cycles.fixed );
                    dmc.Update();
                }
                
                cycles.dmcClock += dmc.ClockDMA( cpu );
            }
            while (cycles.dmcClock <= target);
        }
        
        __attribute__((noinline)) void Apu::ClockFrameCounter()
        {
            typedef char Nestopia_assertion_at_line_2254[(STATUS_SEQUENCE_5_STEP == 0x80) ? 1 : -1];
            ((void)0);
            
            ClockOscillators( cycles.frameDivider & 0x1U );
            
            cycles.frameDivider = (cycles.frameDivider + 1) & 0x3U;
            cycles.frameCounter += Cycles::oscillatorClocks[cycles.region][ctrl >> 7][cycles.frameDivider] * cycles.fixed;
        }
        
        __attribute__((noinline)) void Apu::ClockFrameIRQ(const Cycle target)
        {
            ((void)0);
            
            cpu.DoIRQ( Cpu::IRQ_FRAME, cycles.frameIrqClock );
            
            Cycle clock = cycles.frameIrqClock;
            uint repeat = cycles.frameIrqRepeat;
            
            do
            {
                clock += Cycles::frameClocks[cycles.region][1 + repeat++ % 3];
            }
            while (clock <= target);
            
            cycles.frameIrqClock = clock;
            cycles.frameIrqRepeat = repeat;
        }
        
        __attribute__((noinline)) Apu::Channel::Sample Apu::GetSample()
        {
            dword dac[2];
            
            return Clamp<Channel::OUTPUT_MIN,Channel::OUTPUT_MAX>
            (
             dcBlocker.Apply
             (
              (0 != (dac[0] = square[0].GetSample() + square[1].GetSample()) ? NLN_SQ_0 / (NLN_SQ_1 / dac[0] + NLN_SQ_2) : 0) +
              (0 != (dac[1] = triangle.GetSample() + noise.GetSample() + dmc.GetSample()) ? NLN_TND_0 / (NLN_TND_1 / dac[1] + NLN_TND_2) : 0)
              ) + (extChannel ? extChannel->GetSample() : 0)
             );
        }
        
        void Apu::Poke_4000(void* p_,Address i_,Data j_) { static_cast<Apu*>(p_)->Poke_M_4000(i_,j_); } inline void Apu::Poke_M_4000(Address address,Data data)
        {
            UpdateLatency();
            square[address >> 2 & 0x1].WriteReg0( data );
        }
        
        void Apu::Poke_4001(void* p_,Address i_,Data j_) { static_cast<Apu*>(p_)->Poke_M_4001(i_,j_); } inline void Apu::Poke_M_4001(Address address,Data data)
        {
            Update();
            square[address >> 2 & 0x1].WriteReg1( data );
        }
        
        void Apu::Poke_4002(void* p_,Address i_,Data j_) { static_cast<Apu*>(p_)->Poke_M_4002(i_,j_); } inline void Apu::Poke_M_4002(Address address,Data data)
        {
            Update();
            square[address >> 2 & 0x1].WriteReg2( data );
        }
        
        void Apu::Poke_4003(void* p_,Address i_,Data j_) { static_cast<Apu*>(p_)->Poke_M_4003(i_,j_); } inline void Apu::Poke_M_4003(Address address,Data data)
        {
            square[address >> 2 & 0x1].WriteReg3( data, UpdateDelta() );
        }
        
        void Apu::Poke_4008(void* p_,Address i_,Data j_) { static_cast<Apu*>(p_)->Poke_M_4008(i_,j_); } inline void Apu::Poke_M_4008(Address,Data data)
        {
            Update();
            triangle.WriteReg0( data );
        }
        
        void Apu::Poke_400A(void* p_,Address i_,Data j_) { static_cast<Apu*>(p_)->Poke_M_400A(i_,j_); } inline void Apu::Poke_M_400A(Address,Data data)
        {
            Update();
            triangle.WriteReg2( data );
        }
        
        void Apu::Poke_400B(void* p_,Address i_,Data j_) { static_cast<Apu*>(p_)->Poke_M_400B(i_,j_); } inline void Apu::Poke_M_400B(Address,Data data)
        {
            triangle.WriteReg3( data, UpdateDelta() );
        }
        
        void Apu::Poke_400C(void* p_,Address i_,Data j_) { static_cast<Apu*>(p_)->Poke_M_400C(i_,j_); } inline void Apu::Poke_M_400C(Address,Data data)
        {
            UpdateLatency();
            noise.WriteReg0( data );
        }
        
        void Apu::Poke_400E(void* p_,Address i_,Data j_) { static_cast<Apu*>(p_)->Poke_M_400E(i_,j_); } inline void Apu::Poke_M_400E(Address,Data data)
        {
            Update();
            noise.WriteReg2( data, cycles.region );
        }
        
        void Apu::Poke_400F(void* p_,Address i_,Data j_) { static_cast<Apu*>(p_)->Poke_M_400F(i_,j_); } inline void Apu::Poke_M_400F(Address,Data data)
        {
            noise.WriteReg3( data, UpdateDelta() );
        }
        
        void Apu::Poke_4010(void* p_,Address i_,Data j_) { static_cast<Apu*>(p_)->Poke_M_4010(i_,j_); } inline void Apu::Poke_M_4010(Address,Data data)
        {
            if (!dmc.WriteReg0( data, cycles.region ))
                cpu.ClearIRQ( Cpu::IRQ_DMC );
        }
        
        void Apu::Poke_4011(void* p_,Address i_,Data j_) { static_cast<Apu*>(p_)->Poke_M_4011(i_,j_); } inline void Apu::Poke_M_4011(Address,Data data)
        {
            Update();
            dmc.WriteReg1( data );
        }
        
        void Apu::Poke_4012(void* p_,Address i_,Data j_) { static_cast<Apu*>(p_)->Poke_M_4012(i_,j_); } inline void Apu::Poke_M_4012(Address,Data data)
        {
            dmc.WriteReg2( data );
        }
        
        void Apu::Poke_4013(void* p_,Address i_,Data j_) { static_cast<Apu*>(p_)->Poke_M_4013(i_,j_); } inline void Apu::Poke_M_4013(Address,Data data)
        {
            dmc.WriteReg3( data );
        }
        
        void Apu::Poke_4015(void* p_,Address i_,Data j_) { static_cast<Apu*>(p_)->Poke_M_4015(i_,j_); } inline void Apu::Poke_M_4015(Address,Data data)
        {
            Update();
            
            data = ~data;
            
            square[0].Disable ( data >> 0 & 0x1 );
            square[1].Disable ( data >> 1 & 0x1 );
            triangle.Disable ( data >> 2 & 0x1 );
            noise.Disable ( data >> 3 & 0x1 );
            dmc.Disable ( data & 0x10, cpu );
        }
        
        Data Apu::Peek_4015(void* p_,Address i_) { return static_cast<Apu*>(p_)->Peek_M_4015(i_); } inline Data Apu::Peek_M_4015(Address)
        {
            typedef char Nestopia_assertion_at_line_2390[(Cpu::IRQ_FRAME == 0x40 && Cpu::IRQ_DMC == 0x80) ? 1 : -1];
            
            Cycle elapsed = cpu.GetCycles();
            
            if (cycles.frameIrqClock <= elapsed)
                ClockFrameIRQ( elapsed );
            
            elapsed *= cycles.fixed;
            
            if (cycles.frameCounter < elapsed)
                Update( elapsed );
            
            const uint data = cpu.GetIRQ();
            cpu.ClearIRQ( Cpu::IRQ_FRAME );
            
            return (data & (Cpu::IRQ_FRAME|Cpu::IRQ_DMC)) |
            (
             ( square[0].GetLengthCounter() ? 0x01U : 0x00U ) |
             ( square[1].GetLengthCounter() ? 0x02U : 0x00U ) |
             ( triangle.GetLengthCounter() ? 0x04U : 0x00U ) |
             ( noise.GetLengthCounter() ? 0x08U : 0x00U ) |
             ( dmc.GetLengthCounter() ? 0x10U : 0x00U )
             );
        }
        
        void Apu::WriteFrameCtrl(uint data)
        {
            Cycle next = cpu.GetCycles();
            
            if (cpu.IsOddCycle())
                next += cpu.GetClock();
            
            Update( next * cycles.fixed );
            
            if (cycles.frameIrqClock <= next)
                ClockFrameIRQ( next );
            
            next += cpu.GetClock();
            
            data &= STATUS_BITS;
            
            cycles.frameCounter = (next + Cycles::oscillatorClocks[cycles.region][data >> 7][0]) * cycles.fixed;
            cycles.frameDivider = 0;
            cycles.frameIrqRepeat = 0;
            
            ctrl = data;
            
            if (data)
            {
                cycles.frameIrqClock = Cpu::CYCLE_MAX;
                
                if (data & STATUS_NO_FRAME_IRQ)
                    cpu.ClearIRQ( Cpu::IRQ_FRAME );
                
                if (data & STATUS_SEQUENCE_5_STEP)
                    ClockOscillators( true );
            }
            else
            {
                cycles.frameIrqClock = next + Cycles::frameClocks[cycles.region][0];
            }
        }
        
        Data Apu::Peek_4xxx(void* p_,Address i_) { return static_cast<Apu*>(p_)->Peek_M_4xxx(i_); } inline Data Apu::Peek_M_4xxx(Address)
        {
            return 0x40;
        }
    }
}
