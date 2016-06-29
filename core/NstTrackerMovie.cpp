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

#include <new>
#include "NstMachine.hpp"
#include "NstState.hpp"
#include "NstTrackerMovie.hpp"
#include "NstZlib.hpp"
#include "api/NstApiMovie.hpp"
#include "api/NstApiUser.hpp"

namespace Nes
{
    namespace Core
    {
        
        
        
        
        class Tracker::Movie::Player
        {
        public:
            
            ~Player();
            
            void Relink();
            
        private:
            
            static dword Validate(State::Loader&,const Cpu&,dword,bool);
            
            enum
            {
                MAX_BUFFER_MASK = 0xFFFFFF,
                OPEN_BUS = 0x40
            };
            
            struct Buffer : Vector<byte>
            {
                dword pos;
                
                Buffer()
                : pos(0) {}
            };
            
            struct Loader : State::Loader
            {
                explicit Loader(StdStream s)
                : State::Loader(s,false) {}
                
                bool operator == (StdStream s) const
                {
                    return stream == s;
                }
            };
            
            inline Data Peek_M_Port(Address); static Data Peek_Port(void*,Address);
            inline void Poke_M_Port(Address,Data); static void Poke_Port(void*,Address,Data);
            
            const Io::Port* ports[2];
            dword frame;
            Buffer buffers[2];
            Loader state;
            Cpu& cpu;
            
        public:
            
            static dword Validate(StdStream stream,const Cpu& cpu,dword prgCrc)
            {
                Loader state( stream );
                return Validate( state, cpu, prgCrc, true );
            }
            
            Player(StdStream stream,Cpu& c,const dword prgCrc)
            : frame(0), state(stream), cpu(c)
            {
                Validate( state, cpu, prgCrc, false );
                Relink();
            }
            
            bool operator == (StdStream stream) const
            {
                return state == stream;
            }
            
            void Stop()
            {
                state.End();
            }
            
            bool Execute(Machine& emulator,EmuLoadState loadState)
            {
                (__builtin_expect(!(!!(loadState)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstTrackerMovie.cpp", 114, "!!(loadState)") : (void)0);
                
                if (buffers[0].pos > buffers[0].Size() || buffers[1].pos > buffers[1].Size())
                    throw RESULT_ERR_CORRUPT_FILE;
                
                if (frame)
                {
                    --frame;
                }
                else for (;;)
                {
                    ((void)0);
                    
                    const dword chunk = state.Begin();
                    
                    if (chunk == AsciiId<'K','E','Y'>::V)
                    {
                        for (uint i=0; i < 2; ++i)
                        {
                            buffers[i].pos = 0;
                            buffers[i].Clear();
                        }
                        
                        while (const dword subChunk = state.Begin())
                        {
                            switch (subChunk)
                            {
                                case AsciiId<'S','A','V'>::V:
                                    
                                    (emulator.*loadState)( state, false );
                                    break;
                                    
                                case AsciiId<'P','T','0'>::V:
                                case AsciiId<'P','T','1'>::V:
                                {
                                    const uint i = (subChunk == AsciiId<'P','T','1'>::V);
                                    
                                    buffers[i].Resize( state.Read32() & MAX_BUFFER_MASK );
                                    state.Uncompress( buffers[i].Begin(), buffers[i].Size() );
                                    break;
                                }
                                    
                                case AsciiId<'L','E','N'>::V:
                                    
                                    frame = state.Read32();
                                    ((void)0);
                                    break;
                            }
                            
                            state.End();
                        }
                        
                        state.End();
                        break;
                    }
                    else if (chunk)
                    {
                        state.End();
                    }
                    else
                    {
                        return false;
                    }
                }
                
                return true;
            }
        };
        
        Tracker::Movie::Player::~Player()
        {
            for (uint i=0; i < 2; ++i)
                cpu.Unlink( 0x4016 + i, this, &Player::Peek_Port, &Player::Poke_Port );
        }
        
        dword Tracker::Movie::Player::Validate(State::Loader& state,const Cpu& cpu,const dword prgCrc,const bool end)
        {
            if (state.Begin() != (AsciiId<'N','S','V'>::V | 0x1AUL << 24))
                throw RESULT_ERR_INVALID_FILE;
            
            const dword length = state.Length();
            
            Region::Type region = Region::NTSC;
            dword crc = 0;
            
            while (const dword chunk = state.Check())
            {
                if (chunk == AsciiId<'P','A','L'>::V)
                {
                    state.Begin();
                    region = Region::PAL;
                    state.End();
                }
                else if (chunk == AsciiId<'C','R','C'>::V)
                {
                    state.Begin();
                    crc = state.Read32();
                    state.End();
                }
                else if (chunk & 0xFFFFFF00)
                {
                    break;
                }
                else
                {
                    throw RESULT_ERR_UNSUPPORTED_FILE_VERSION;
                }
            }
            
            if (end)
                state.End( length );
            
            if (region != cpu.GetRegion())
                throw RESULT_ERR_WRONG_MODE;
            
            if (crc && prgCrc && crc != prgCrc && Api::User::questionCallback( Api::User::QUESTION_NSV_PRG_CRC_FAIL_CONTINUE ) == Api::User::ANSWER_NO)
                throw RESULT_ERR_INVALID_CRC;
            
            return length;
        }
        
        void Tracker::Movie::Player::Relink()
        {
            for (uint i=0; i < 2; ++i)
                ports[i] = cpu.Link( 0x4016 + i, Cpu::LEVEL_HIGHEST, this, &Player::Peek_Port, &Player::Poke_Port );
        }
        
        
        
        
        
        Data Tracker::Movie::Player::Peek_Port(void* p_,Address i_) { return static_cast<Tracker::Movie::Player*>(p_)->Peek_M_Port(i_); } inline Data Tracker::Movie::Player::Peek_M_Port(Address address)
        {
            address &= 0x1;
            const uint pos = buffers[address].pos++;
            
            if (pos < buffers[address].Size())
            {
                return buffers[address][pos];
            }
            else
            {
                ((void)0);
                return OPEN_BUS;
            }
        }
        
        void Tracker::Movie::Player::Poke_Port(void* p_,Address i_,Data j_) { static_cast<Tracker::Movie::Player*>(p_)->Poke_M_Port(i_,j_); } inline void Tracker::Movie::Player::Poke_M_Port(Address address,Data data)
        {
            ports[address & 0x1]->Poke( address, data );
        }
        
        
        
        
        
        class Tracker::Movie::Recorder
        {
        public:
            
            ~Recorder();
            
            void Relink();
            
        private:
            
            void BeginKey(Machine&,EmuSaveState);
            void EndKey();
            
            enum
            {
                BAD_FRAME = dword(~0UL),
                MAX_BUFFER_BLOCK = SIZE_1K * 8192UL
            };
            
            typedef Vector<byte> Buffer;
            
            struct Saver : State::Saver
            {
                Saver(StdStream s,dword a)
                : State::Saver(s,true,true,a) {}
                
                bool operator == (StdStream s) const
                {
                    return stream == s;
                }
            };
            
            inline Data Peek_M_Port(Address); static Data Peek_Port(void*,Address);
            inline void Poke_M_Port(Address,Data); static void Poke_Port(void*,Address,Data);
            
            const Io::Port* ports[2];
            ibool resync;
            dword frame;
            Buffer buffers[2];
            Saver state;
            Cpu& cpu;
            
        public:
            
            Recorder(StdStream stream,Cpu& c,const dword prgCrc,const bool append)
            : resync(true), frame(0), state(stream,append ? Player::Validate(stream,c,prgCrc) : 0), cpu(c)
            {
                if (!append)
                {
                    state.Begin( AsciiId<'N','S','V'>::V | 0x1AUL << 24 );
                    
                    if (cpu.GetRegion() == Region::PAL)
                        state.Begin( AsciiId<'P','A','L'>::V ).End();
                    
                    if (prgCrc)
                        state.Begin( AsciiId<'C','R','C'>::V ).Write32( prgCrc ).End();
                }
                
                Relink();
            }
            
            bool operator == (StdStream stream) const
            {
                return state == stream;
            }
            
            void Resync()
            {
                resync = true;
            }
            
            void Stop()
            {
                EndKey();
                
                state.End();
            }
            
            void Execute(Machine& machine,EmuSaveState saveState)
            {
                (__builtin_expect(!(!!(saveState)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstTrackerMovie.cpp", 350, "!!(saveState)") : (void)0);
                
                if (frame == BAD_FRAME)
                    throw RESULT_ERR_OUT_OF_MEMORY;
                
                if (resync || buffers[0].Size() >= MAX_BUFFER_BLOCK || buffers[1].Size() >= MAX_BUFFER_BLOCK)
                {
                    EndKey();
                    BeginKey( machine, saveState );
                }
                
                ++frame;
            }
        };
        
        Tracker::Movie::Recorder::~Recorder()
        {
            for (uint i=0; i < 2; ++i)
                cpu.Unlink( 0x4016 + i, this, &Recorder::Peek_Port, &Recorder::Poke_Port );
        }
        
        void Tracker::Movie::Recorder::Relink()
        {
            for (uint i=0; i < 2; ++i)
                ports[i] = cpu.Link( 0x4016 + i, Cpu::LEVEL_HIGHEST, this, &Recorder::Peek_Port, &Recorder::Poke_Port );
        }
        
        void Tracker::Movie::Recorder::BeginKey(Machine& machine,EmuSaveState saveState)
        {
            state.Begin( AsciiId<'K','E','Y'>::V );
            
            if (resync)
            {
                resync = false;
                
                state.Begin( AsciiId<'S','A','V'>::V );
                (machine.*saveState)( state );
                state.End();
            }
        }
        
        void Tracker::Movie::Recorder::EndKey()
        {
            if (frame == BAD_FRAME)
                throw RESULT_ERR_OUT_OF_MEMORY;
            
            if (frame)
            {
                state.Begin( AsciiId<'L','E','N'>::V ).Write32( frame-1 ).End();
                frame = 0;
                
                for (uint i=0; i < 2; ++i)
                {
                    if (buffers[i].Size())
                    {
                        state.Begin( AsciiId<'P','T','0'>::R(0,0,i) ).Write32( buffers[i].Size() ).Compress( buffers[i].Begin(), buffers[i].Size() ).End();
                        buffers[i].Clear();
                    }
                }
                
                state.End();
            }
        }
        
        
        
        
        
        Data Tracker::Movie::Recorder::Peek_Port(void* p_,Address i_) { return static_cast<Tracker::Movie::Recorder*>(p_)->Peek_M_Port(i_); } inline Data Tracker::Movie::Recorder::Peek_M_Port(Address address)
        {
            const uint data = ports[address & 0x1]->Peek( address );
            
            if (frame != BAD_FRAME)
            {
                try
                {
                    buffers[address & 0x1].Append( data );
                }
                catch (...)
                {
                    frame = BAD_FRAME;
                }
            }
            
            return data;
        }
        
        void Tracker::Movie::Recorder::Poke_Port(void* p_,Address i_,Data j_) { static_cast<Tracker::Movie::Recorder*>(p_)->Poke_M_Port(i_,j_); } inline void Tracker::Movie::Recorder::Poke_M_Port(Address address,Data data)
        {
            ports[address & 0x1]->Poke( address, data );
        }
        
        
        
        
        
        Tracker::Movie::Movie(Machine& e,EmuLoadState l,EmuSaveState s,Cpu& c,dword crc)
        :
        player (__null),
        recorder (__null),
        emulator (e),
        saveState (s),
        loadState (l),
        cpu (c),
        prgCrc (crc)
        {
        }
        
        Tracker::Movie::~Movie()
        {
            Stop();
        }
        
        bool Tracker::Movie::Record(StdStream const stream,const bool append)
        {
            (__builtin_expect(!(!!(stream)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstTrackerMovie.cpp", 465, "!!(stream)") : (void)0);
            
            if (!Zlib::AVAILABLE)
                throw RESULT_ERR_UNSUPPORTED;
            
            if (player)
                throw RESULT_ERR_NOT_READY;
            
            if (recorder && *recorder == stream)
                return false;
            
            Stop();
            
            recorder = new Recorder( stream, cpu, prgCrc, append );
            
            Api::Movie::eventCallback( Api::Movie::EVENT_RECORDING );
            
            return true;
        }
        
        bool Tracker::Movie::Play(StdStream const stream)
        {
            (__builtin_expect(!(!!(stream)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstTrackerMovie.cpp", 487, "!!(stream)") : (void)0);
            
            if (!Zlib::AVAILABLE)
                throw RESULT_ERR_UNSUPPORTED;
            
            if (recorder)
                throw RESULT_ERR_NOT_READY;
            
            if (player && *player == stream)
                return false;
            
            Stop();
            
            player = new Player( stream, cpu, prgCrc );
            
            Api::Movie::eventCallback( Api::Movie::EVENT_PLAYING );
            
            return true;
        }
        
        void Tracker::Movie::Stop()
        {
            Stop( RESULT_OK );
        }
        
        bool Tracker::Movie::Stop(Result result)
        {
            if (recorder || player)
            {
                if (((result) >= Nes::RESULT_OK))
                {
                    try
                    {
                        if (recorder)
                            recorder->Stop();
                        else
                            player->Stop();
                    }
                    catch (Result r)
                    {
                        result = r;
                    }
                    catch (const std::bad_alloc&)
                    {
                        result = RESULT_ERR_OUT_OF_MEMORY;
                    }
                    catch (...)
                    {
                        result = RESULT_ERR_GENERIC;
                    }
                }
                
                if (recorder)
                {
                    delete recorder;
                    recorder = __null;
                    
                    Api::Movie::eventCallback( Api::Movie::EVENT_RECORDING_STOPPED, result );
                }
                else
                {
                    delete player;
                    player = __null;
                    
                    Api::Movie::eventCallback( Api::Movie::EVENT_PLAYING_STOPPED, result );
                    
                    if (((result) < Nes::RESULT_OK))
                        return false;
                }
            }
            
            return true;
        }
        
        void Tracker::Movie::Reset()
        {
            if (recorder)
            {
                recorder->Relink();
            }
            else if (player)
            {
                player->Relink();
            }
            
            Resync();
        }
        
        void Tracker::Movie::Resync()
        {
            if (recorder)
                recorder->Resync();
        }
        
        
        
        
        
        bool Tracker::Movie::Execute()
        {
            Result result = RESULT_OK;
            
            try
            {
                if (recorder)
                {
                    recorder->Execute( emulator, saveState );
                    return true;
                }
                else if (player && player->Execute( emulator, loadState ))
                {
                    return true;
                }
            }
            catch (Result r)
            {
                result = r;
            }
            catch (const std::bad_alloc&)
            {
                result = RESULT_ERR_OUT_OF_MEMORY;
            }
            catch (...)
            {
                result = RESULT_ERR_GENERIC;
            }
            
            if (!Stop( result ))
                throw result;
            
            return false;
        }
    }
}
