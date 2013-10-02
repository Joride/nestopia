/*
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
*/

#ifdef __INTEL_COMPILER
#pragma warning( disable : 981 1572 1599 )
#elif defined(_MSC_VER)
#pragma warning( disable : 4127 )
#endif

#if defined(_MSC_VER) && _MSC_VER >= 1200
#pragma optimize("s", on)
#endif

#define NES_NTSC_NO_BLITTERS
#include "../nes_ntsc/nes_ntsc.inl"

#if defined(_MSC_VER) && _MSC_VER >= 1200
#pragma optimize("", on)
#endif