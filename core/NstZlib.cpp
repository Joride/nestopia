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

#include "NstAssert.hpp"
#include "NstZlib.hpp"

#ifndef NST_NO_ZLIB

 #ifdef NST_WIN32

  #if NST_MSVC >= 800
  #define ZEXPORT __stdcall
  #else
  #define ZLIB_WINAPI
  #endif

 #endif

// #include "../zlib/zlib.h"

#endif

namespace Nes
{
    namespace Core
    {
        namespace Zlib
        {
            ulong Compress(const byte*,ulong,byte*,ulong,Compression)
            {
                return 0;
            }
            
            ulong Uncompress(const byte*,ulong,byte*,ulong)
            {
                return 0;
            }
            
            
        }
    }
}
