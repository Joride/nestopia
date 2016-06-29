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

#include "NstState.hpp"
#include "NstZlib.hpp"

namespace Nes
{
    namespace Core
    {
        namespace State
        {
            enum Compression
            {
                NO_COMPRESSION,
                ZLIB_COMPRESSION
            };
            
            
            
            
            
            Saver::Saver(StdStream p,bool c,bool i,dword append)
            : stream(p), chunks(CHUNK_RESERVE), useCompression(c), internal(i)
            {
                typedef char Nestopia_assertion_at_line_47[(CHUNK_RESERVE >= 2) ? 1 : -1];
                
                chunks.SetTo(1);
                chunks.Front() = 0;
                
                if (append)
                {
                    chunks.SetTo(2);
                    chunks[1] = append;
                    stream.Seek( 4 + 4 + append );
                }
            }
            
            Saver::~Saver()
            {
                ((void)0);
            }
            
            
            
            
            
            Saver& Saver::Begin(dword chunk)
            {
                stream.Write32( chunk );
                stream.Write32( 0 );
                chunks.Append( 0 );
                
                return *this;
            }
            
            Saver& Saver::End()
            {
                (__builtin_expect(!(!!(chunks.Size() > 1)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstState.cpp", 80, "!!(chunks.Size() > 1)") : (void)0);
                
                const dword written = chunks.Pop();
                chunks.Back() += 4 + 4 + written;
                
                stream.Seek( -idword(written + 4) );
                stream.Write32( written );
                stream.Seek( written );
                
                return *this;
            }
            
            Saver& Saver::Write8(uint data)
            {
                chunks.Back() += 1;
                stream.Write8( data );
                return *this;
            }
            
            Saver& Saver::Write16(uint data)
            {
                chunks.Back() += 2;
                stream.Write16( data );
                return *this;
            }
            
            Saver& Saver::Write32(dword data)
            {
                chunks.Back() += 4;
                stream.Write32( data );
                return *this;
            }
            
            Saver& Saver::Write64(qword data)
            {
                chunks.Back() += 8;
                stream.Write64( data );
                return *this;
            }
            
            Saver& Saver::Write(const byte* data,dword length)
            {
                chunks.Back() += length;
                stream.Write( data, length );
                return *this;
            }
            
            Saver& Saver::Compress(const byte* const data,const dword length)
            {
                ((void)0);
                
                if (Zlib::AVAILABLE && useCompression && length > 1)
                {
                    Vector<byte> buffer( length - 1 );
                    
                    if (const dword compressed = Zlib::Compress( data, length, buffer.Begin(), buffer.Size(), Zlib::BEST_COMPRESSION ))
                    {
                        chunks.Back() += 1 + compressed;
                        stream.Write8( ZLIB_COMPRESSION );
                        stream.Write( buffer.Begin(), compressed );
                        return *this;
                    }
                }
                
                chunks.Back() += 1 + length;
                stream.Write8( NO_COMPRESSION );
                stream.Write( data, length );
                
                return *this;
            }
            
            
            
            
            
            Loader::Loader(StdStream p,bool c)
            : stream(p), chunks(CHUNK_RESERVE), checkCrc(c)
            {
                chunks.SetTo(0);
            }
            
            Loader::~Loader()
            {
                ((void)0);
            }
            
            
            
            
            
            dword Loader::Begin()
            {
                if (chunks.Size() && !chunks.Back())
                    return 0;
                
                const dword chunk = stream.Read32();
                const dword length = stream.Read32();
                
                if (chunks.Size())
                {
                    if (chunks.Back() >= 4+4+length)
                        chunks.Back() -= 4+4+length;
                    else
                        throw RESULT_ERR_CORRUPT_FILE;
                }
                
                chunks.Append( length );
                
                return chunk;
            }
            
            dword Loader::Length() const
            {
                return chunks.Size() ? chunks.Back() : 0;
            }
            
            dword Loader::Check()
            {
                return chunks.Size() && !chunks.Back() ? 0 : stream.Peek32();
            }
            
            void Loader::End()
            {
                if (const dword remaining = chunks.Pop())
                {
                    ((void)0);
                    stream.Seek( remaining );
                }
            }
            
            void Loader::End(dword rollBack)
            {
                if (const idword back = -idword(rollBack+4+4) + idword(chunks.Pop()))
                    stream.Seek( back );
            }
            
            void Loader::CheckRead(dword length)
            {
                if (chunks.Back() >= length)
                    chunks.Back() -= length;
                else
                    throw RESULT_ERR_CORRUPT_FILE;
            }
            
            uint Loader::Read8()
            {
                CheckRead( 1 );
                return stream.Read8();
            }
            
            uint Loader::Read16()
            {
                CheckRead( 2 );
                return stream.Read16();
            }
            
            dword Loader::Read32()
            {
                CheckRead( 4 );
                return stream.Read32();
            }
            
            qword Loader::Read64()
            {
                CheckRead( 8 );
                return stream.Read64();
            }
            
            void Loader::Read(byte* const data,const dword length)
            {
                CheckRead( length );
                stream.Read( data, length );
            }
            
            void Loader::Uncompress(byte* const data,const dword length)
            {
                ((void)0);
                
                switch (Read8())
                {
                    case NO_COMPRESSION:
                        
                        Read( data, length );
                        break;
                        
                    case ZLIB_COMPRESSION:
                        
                        if (!Zlib::AVAILABLE)
                        {
                            throw RESULT_ERR_UNSUPPORTED;
                        }
                        else if (chunks.Back())
                        {
                            Vector<byte> buffer( chunks.Back() );
                            Read( buffer.Begin(), buffer.Size() );
                            
                            if (Zlib::Uncompress( buffer.Begin(), buffer.Size(), data, length ))
                                break;
                        }
                        
                    default:
                        
                        throw RESULT_ERR_CORRUPT_FILE;
                }
            }
        }
    }
}
