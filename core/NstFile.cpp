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
#include "NstVector.hpp"
#include "NstZlib.hpp"
#include "NstFile.hpp"
#include "api/NstApiUser.hpp"

namespace Nes
{
    namespace Core
    {
        
        
        
        
        class File::Buffer
        {
            dword uncompressed;
            Vector<byte> data;
            
        public:
            
            void Load(const byte* __restrict__,dword);
            bool Save(Type,const byte* __restrict__,dword) const;
            
            Buffer()
            : uncompressed(0) {}
        };
        
        File::File()
        : buffer(*new Buffer)
        {
            typedef char Nestopia_assertion_at_line_67[(LOAD_BATTERY - Api::User::FILE_LOAD_BATTERY == 0 && SAVE_BATTERY - Api::User::FILE_SAVE_BATTERY == 0 && SAVE_FDS - Api::User::FILE_SAVE_FDS == 0 && LOAD_EEPROM - Api::User::FILE_LOAD_EEPROM == 0 && SAVE_EEPROM - Api::User::FILE_SAVE_EEPROM == 0 && LOAD_TAPE - Api::User::FILE_LOAD_TAPE == 0 && SAVE_TAPE - Api::User::FILE_SAVE_TAPE == 0 && LOAD_TURBOFILE - Api::User::FILE_LOAD_TURBOFILE == 0 && SAVE_TURBOFILE - Api::User::FILE_SAVE_TURBOFILE == 0) ? 1 : -1];
# 68 "/Users/Jorrit/iOS/nestopia/core/NstFile.cpp"
        }
        
        File::~File()
        {
            delete &buffer;
        }
        
        void File::Load(const byte* const __restrict__ data,const dword size) const
        {
            (__builtin_expect(!(!!(data && size)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstFile.cpp", 77, "!!(data && size)") : (void)0);
            
            buffer.Load( data, size );
        }
        
        bool File::Load(const Type type,byte* const __restrict__ data,const dword size) const
        {
            (__builtin_expect(!(!!(data && size)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstFile.cpp", 84, "!!(data && size)") : (void)0);
            
            bool matched = true;
            
            {
                Api::User::FileData file;
                Api::User::fileIoCallback( static_cast<Api::User::File>(type), file );
                
                if (Api::User::FileData::size_type filesize = file.size())
                {
                    if (filesize != size)
                    {
                        matched = false;
                        
                        if (filesize > size)
                            filesize = size;
                    }
                    
                    std::memcpy( data, &file.front(), filesize );
                }
            }
            
            buffer.Load( data, size );
            
            return matched;
        }
        
        bool File::Load(const Type type,Vector<byte>& vector,const dword maxsize) const
        {
            (__builtin_expect(!(!!(maxsize)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstFile.cpp", 113, "!!(maxsize)") : (void)0);
            
            bool matched = true;
            
            {
                Api::User::FileData file;
                Api::User::fileIoCallback( static_cast<Api::User::File>(type), file );
                
                if (Api::User::FileData::size_type filesize = file.size())
                {
                    if (filesize > maxsize)
                    {
                        filesize = maxsize;
                        matched = false;
                    }
                    
                    vector.Resize( filesize );
                    std::memcpy( vector.Begin(), &file.front(), filesize );
                }
                else
                {
                    vector.Destroy();
                    return true;
                }
            }
            
            buffer.Load( vector.Begin(), vector.Size() );
            
            return matched;
        }
        
        void File::Save(const Type type,const byte* const __restrict__ data,const dword size,const bool cache) const
        {
            if (buffer.Save( type, data, size ))
                buffer.Load( data, cache ? size : 0 );
        }
        
        void File::Buffer::Load(const byte* const __restrict__ input,dword size)
        {
            try
            {
                uncompressed = 0;
                data.Resize( size );
                
                if (size)
                {
                    (__builtin_expect(!(!!(input)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstFile.cpp", 159, "!!(input)") : (void)0);
                    
                    size = Zlib::Compress( input, size, data.Begin(), size, Zlib::BEST_COMPRESSION );
                    
                    if (size-1 < data.Size()-1)
                    {
                        uncompressed = data.Size();
                        data.SetTo( size );
                    }
                    else
                    {
                        std::memcpy( data.Begin(), input, data.Size() );
                    }
                }
                
                data.Defrag();
            }
            catch (...)
            {
                data.Destroy();
            }
        }
        
        bool File::Buffer::Save(const Type type,const byte* const __restrict__ input,const dword size) const
        {
            (__builtin_expect(!(!!(input && size)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstFile.cpp", 184, "!!(input && size)") : (void)0);
            
            Api::User::FileData file( size );
            
            bool dirty = (uncompressed ? uncompressed : data.Size()) != size;
            
            if (!dirty)
            {
                dirty = uncompressed && Zlib::Uncompress( data.Begin(), data.Size(), &file.front(), size ) != size;
                
                if (!dirty)
                {
                    const byte* const __restrict__ old = (uncompressed ? &file.front() : data.Begin());
                    dword i = 0;
                    
                    do
                    {
                        if (old[i] != input[i])
                        {
                            dirty = true;
                            break;
                        }
                    }
                    while (++i < size);
                }
            }
            
            if (dirty)
            {
                std::memcpy( &file.front(), input, size );
                Api::User::fileIoCallback( static_cast<Api::User::File>(type), file );
            }
            
            return dirty;
        }
        
        
        
        
    }
}
