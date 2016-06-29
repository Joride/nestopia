////////////////////////////////////////////////////////////////////////////////////////
//
// Nestopia - NES/Famicom emulator written in C++
//
// Copyright (C) 2003-2007 Martin Freij
// Copyright (C) 2003 MaxSt ( maxst@hiend3d.com )
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

#include "NstCore.hpp"

#include "NstVideoRenderer.hpp"
#include "NstVideoFilterHqX.hpp"

namespace Nes
{
 namespace Core
 {
  namespace Video
  {
   void Renderer::FilterHqX::Blit(const Input& input,const Output& output,uint phase)
   {
    (*this.*path)( input, output, phase );
   }

   template<dword R,dword G,dword B>
   dword Renderer::FilterHqX::Interpolate1(dword c1,dword c2)
   {
    return ((((c1 & G)*3 + (c2 & G)) & (G << 2)) + (((c1 & (R|B))*3 + (c2 & (R|B))) & ((R|B) << 2))) >> 2;
   }

   template<>
   inline dword Renderer::FilterHqX::Interpolate1<0xFF0000,0x00FF00,0x0000FF>(dword c1,dword c2)
   {
    return (c1 * 3 + c2) >> 2;
   }

   template<dword R,dword G,dword B>
   dword Renderer::FilterHqX::Interpolate2(dword c1,dword c2,dword c3)
   {
    return ((((c1 & G)*2 + (c2 & G) + (c3 & G)) & (G << 2)) + (((c1 & (R|B))*2 + (c2 & (R|B)) + (c3 & (R|B))) & ((R|B) << 2))) >> 2;
   }

   template<>
   inline dword Renderer::FilterHqX::Interpolate2<0xFF0000,0x00FF00,0x0000FF>(dword c1,dword c2,dword c3)
   {
    return (c1 * 2 + c2 + c3) >> 2;
   }

   template<dword R,dword G,dword B>
   dword Renderer::FilterHqX::Interpolate3(dword c1,dword c2)
   {
    return ((((c1 & G)*7 + (c2 & G)) & (G << 3)) + (((c1 & (R|B))*7 + (c2 & (R|B))) & ((R|B) << 3))) >> 3;
   }

   template<dword R,dword G,dword B>
   dword Renderer::FilterHqX::Interpolate4(dword c1,dword c2,dword c3)
   {
    return ((((c1 & G)*2 + ((c2 & G) + (c3 & G))*7) & (G << 4)) + (((c1 & (R|B))*2 + ((c2 & (R|B)) + (c3 & (R|B)))*7) & ((R|B) << 4))) >> 4;
   }

   template<dword R,dword G,dword B>
   dword Renderer::FilterHqX::Interpolate5(dword c1,dword c2)
   {
    return (((c1 & G) + (c2 & G) & (G << 1)) + ((c1 & (R|B)) + (c2 & (R|B)) & ((R|B) << 1))) >> 1;
   }

   template<>
   inline dword Renderer::FilterHqX::Interpolate5<0xFF0000,0x00FF00,0x0000FF>(dword c1,dword c2)
   {
    return (c1 + c2) >> 1;
   }

   template<dword R,dword G,dword B>
   dword Renderer::FilterHqX::Interpolate6(dword c1,dword c2,dword c3)
   {
    return ((((c1 & G)*5 + (c2 & G)*2 + (c3 & G)) & (G << 3)) + (((c1 & (R|B))*5 + (c2 & (R|B))*2 + (c3 & (R|B))) & ((R|B) << 3))) >> 3;
   }

   template<dword R,dword G,dword B>
   dword Renderer::FilterHqX::Interpolate7(dword c1,dword c2,dword c3)
   {
    return ((((c1 & G)*6 + (c2 & G) + (c3 & G)) & (G << 3)) + (((c1 & (R|B))*6 + (c2 & (R|B)) + (c3 & (R|B))) & ((R|B) << 3))) >> 3;
   }

   template<dword R,dword G,dword B>
   dword Renderer::FilterHqX::Interpolate8(dword c1,dword c2)
   {
    return ((((c1 & G)*5 + (c2 & G)*3) & (G << 3)) + (((c1 & (R|B))*5 + (c2 & (R|B))*3) & ((R|B) << 3))) >> 3;
   }

   template<dword R,dword G,dword B>
   dword Renderer::FilterHqX::Interpolate9(dword c1,dword c2,dword c3)
   {
    return ((((c1 & G)*2 + ((c2 & G) + (c3 & G))*3 ) & (G << 3)) + (((c1 & (R|B))*2 + ((c2 & (R|B)) + (c3 & (R|B)))*3 ) & ((R|B) << 3))) >> 3;
   }

   template<dword R,dword G,dword B>
   dword Renderer::FilterHqX::Interpolate10(dword c1,dword c2,dword c3)
   {
    return ((((c1 & G)*14 + (c2 & G) + (c3 & G)) & (G << 4)) + (((c1 & (R|B))*14 + (c2 & (R|B)) + (c3 & (R|B))) & ((R|B) << 4))) >> 4;
   }

   inline dword Renderer::FilterHqX::Diff(uint w1,uint w2) const
   {
    return (lut.yuv[w1] - lut.yuv[w2] + Lut::YUV_OFFSET) & Lut::YUV_MASK;
   }

   template<typename T>
   struct Renderer::FilterHqX::Buffer
   {
    uint w[10];
    dword c[10];

    inline void Convert(const Lut& lut)
    {
     for (uint k=0; k < 9; ++k)
      c[k] = lut.rgb[w[k]];
    }
   };

   template<>
   struct Renderer::FilterHqX::Buffer<word>
   {
    union
    {
     uint w[10];
     dword c[10];
    };

    void Convert(const Lut&)
    {
    }
   };

   template<typename T,dword R,dword G,dword B>
   void Renderer::FilterHqX::Blit2x(const Input& input,const Output& output,uint) const
   {
    const byte* __restrict__ src = reinterpret_cast<const byte*>(input.pixels);
    const long pitch = output.pitch + output.pitch - (WIDTH*2 * sizeof(T));

    T* __restrict__ dst[2] =
    {
     static_cast<T*>(output.pixels) - 2,
     reinterpret_cast<T*>(static_cast<byte*>(output.pixels) + output.pitch) - 2
    };

    for (uint y=HEIGHT; y; --y)
    {
     const uint lines[2] =
     {
      y < HEIGHT ? WIDTH * sizeof(Input::Pixel) : 0,
      y > 1 ? WIDTH * sizeof(Input::Pixel) : 0
     };

     Buffer<T> b;

     b.w[2] = (b.w[1] = input.palette[*reinterpret_cast<const Input::Pixel*>(src - lines[0])]);
     b.w[5] = (b.w[4] = input.palette[*reinterpret_cast<const Input::Pixel*>(src)]);
     b.w[8] = (b.w[7] = input.palette[*reinterpret_cast<const Input::Pixel*>(src + lines[1])]);

     for (uint x=WIDTH; x; )
     {
      src += sizeof(Input::Pixel);
      dst[0] += 2;
      dst[1] += 2;

      b.w[0] = b.w[1];
      b.w[1] = b.w[2];
      b.w[3] = b.w[4];
      b.w[4] = b.w[5];
      b.w[6] = b.w[7];
      b.w[7] = b.w[8];

      if (--x)
      {
       b.w[2] = input.palette[*reinterpret_cast<const Input::Pixel*>(src - lines[0])];
       b.w[5] = input.palette[*reinterpret_cast<const Input::Pixel*>(src)];
       b.w[8] = input.palette[*reinterpret_cast<const Input::Pixel*>(src + lines[1])];
      }

      b.Convert( lut );

      const uint yuv5 = lut.yuv[b.w[4]];


# 1 "/Users/Jorrit/iOS/nestopia/core/NstVideoFilterHq2x.inl" 1
# 26 "/Users/Jorrit/iOS/nestopia/core/NstVideoFilterHq2x.inl"
switch
(
 (b.w[4] != b.w[0] && ((yuv5 - lut.yuv[b.w[0]]) & Lut::YUV_MASK) ? 0x01U : 0x0U) |
 (b.w[4] != b.w[1] && ((yuv5 - lut.yuv[b.w[1]]) & Lut::YUV_MASK) ? 0x02U : 0x0U) |
 (b.w[4] != b.w[2] && ((yuv5 - lut.yuv[b.w[2]]) & Lut::YUV_MASK) ? 0x04U : 0x0U) |
 (b.w[4] != b.w[3] && ((yuv5 - lut.yuv[b.w[3]]) & Lut::YUV_MASK) ? 0x08U : 0x0U) |
 (b.w[4] != b.w[5] && ((yuv5 - lut.yuv[b.w[5]]) & Lut::YUV_MASK) ? 0x10U : 0x0U) |
 (b.w[4] != b.w[6] && ((yuv5 - lut.yuv[b.w[6]]) & Lut::YUV_MASK) ? 0x20U : 0x0U) |
 (b.w[4] != b.w[7] && ((yuv5 - lut.yuv[b.w[7]]) & Lut::YUV_MASK) ? 0x40U : 0x0U) |
 (b.w[4] != b.w[8] && ((yuv5 - lut.yuv[b.w[8]]) & Lut::YUV_MASK) ? 0x80U : 0x0U)
)
# 85 "/Users/Jorrit/iOS/nestopia/core/NstVideoFilterHq2x.inl"
{
 case 0:
 case 1:
 case 4:
 case 32:
 case 128:
 case 5:
 case 132:
 case 160:
 case 33:
 case 129:
 case 36:
 case 133:
 case 164:
 case 161:
 case 37:
 case 165:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );
  break;

 case 2:
 case 34:
 case 130:
 case 162:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[0], b.c[3] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[2], b.c[5] );
  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );
  break;

 case 16:
 case 17:
 case 48:
 case 49:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[2], b.c[1] );
  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[8], b.c[7] );
  break;

 case 64:
 case 65:
 case 68:
 case 69:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[6], b.c[3] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[8], b.c[5] );
  break;

 case 8:
 case 12:
 case 136:
 case 140:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[0], b.c[1] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[6], b.c[7] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );
  break;

 case 3:
 case 35:
 case 131:
 case 163:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[2], b.c[5] );
  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );
  break;

 case 6:
 case 38:
 case 134:
 case 166:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[0], b.c[3] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );
  break;

 case 20:
 case 21:
 case 52:
 case 53:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[8], b.c[7] );
  break;

 case 144:
 case 145:
 case 176:
 case 177:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[2], b.c[1] );
  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  break;

 case 192:
 case 193:
 case 196:
 case 197:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[6], b.c[3] );
  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  break;

 case 96:
 case 97:
 case 100:
 case 101:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[8], b.c[5] );
  break;

 case 40:
 case 44:
 case 168:
 case 172:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[0], b.c[1] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );
  break;

 case 9:
 case 13:
 case 137:
 case 141:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[6], b.c[7] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );
  break;

 case 18:
 case 50:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[0], b.c[3] );

  if (Diff( b.w[1], b.w[5] ))
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  else
   dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[8], b.c[7] );
  break;

 case 80:
 case 81:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[2], b.c[1] );
  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[6], b.c[3] );

  if (Diff( b.w[5], b.w[7] ))
   dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  else
   dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 72:
 case 76:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[0], b.c[1] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  if (Diff( b.w[7], b.w[3] ))
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  else
   dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );

  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[8], b.c[5] );
  break;

 case 10:
 case 138:

  if (Diff( b.w[3], b.w[1] ))
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  else
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );

  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[2], b.c[5] );
  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[6], b.c[7] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );
  break;

 case 66:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[0], b.c[3] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[2], b.c[5] );
  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[6], b.c[3] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[8], b.c[5] );
  break;

 case 24:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[0], b.c[1] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[2], b.c[1] );
  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[6], b.c[7] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[8], b.c[7] );
  break;

 case 7:
 case 39:
 case 135:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );
  break;

 case 148:
 case 149:
 case 180:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  break;

 case 224:
 case 228:
 case 225:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  break;

 case 41:
 case 169:
 case 45:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );
  break;

 case 22:
 case 54:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[0], b.c[3] );

  if (Diff( b.w[1], b.w[5] ))
   dst[0][1] = b.c[4];
  else
   dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[8], b.c[7] );
  break;

 case 208:
 case 209:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[2], b.c[1] );
  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[6], b.c[3] );

  if (Diff( b.w[5], b.w[7] ))
   dst[1][1] = b.c[4];
  else
   dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 104:
 case 108:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[0], b.c[1] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  if (Diff( b.w[7], b.w[3] ))
   dst[1][0] = b.c[4];
  else
   dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );

  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[8], b.c[5] );
  break;

 case 11:
 case 139:

  if (Diff( b.w[3], b.w[1] ))
   dst[0][0] = b.c[4];
  else
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );

  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[2], b.c[5] );
  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[6], b.c[7] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );
  break;

 case 19:
 case 51:

  if (Diff( b.w[1], b.w[5] ))
  {
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  }
  else
  {
   dst[0][0] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[3] );
   dst[0][1] = Interpolate9<R,G,B>( b.c[4], b.c[1], b.c[5] );
  }

  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[8], b.c[7] );
  break;

 case 146:
 case 178:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[0], b.c[3] );

  if (Diff( b.w[1], b.w[5] ))
  {
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
   dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  }
  else
  {
   dst[0][1] = Interpolate9<R,G,B>( b.c[4], b.c[1], b.c[5] );
   dst[1][1] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[7] );
  }

  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  break;

 case 84:
 case 85:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );

  if (Diff( b.w[5], b.w[7] ))
  {
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  }
  else
  {
   dst[0][1] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[1] );
   dst[1][1] = Interpolate9<R,G,B>( b.c[4], b.c[5], b.c[7] );
  }

  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[6], b.c[3] );
  break;

 case 112:
 case 113:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[2], b.c[1] );

  if (Diff( b.w[5], b.w[7] ))
  {
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
   dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  }
  else
  {
   dst[1][0] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[3] );
   dst[1][1] = Interpolate9<R,G,B>( b.c[4], b.c[5], b.c[7] );
  }
  break;

 case 200:
 case 204:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[0], b.c[1] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  if (Diff( b.w[7], b.w[3] ))
  {
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
   dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  }
  else
  {
   dst[1][0] = Interpolate9<R,G,B>( b.c[4], b.c[7], b.c[3] );
   dst[1][1] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[5] );
  }
  break;

 case 73:
 case 77:

  if (Diff( b.w[7], b.w[3] ))
  {
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  }
  else
  {
   dst[0][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[1] );
   dst[1][0] = Interpolate9<R,G,B>( b.c[4], b.c[7], b.c[3] );
  }

  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[8], b.c[5] );
  break;

 case 42:
 case 170:

  if (Diff( b.w[3], b.w[1] ))
  {
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  }
  else
  {
   dst[0][0] = Interpolate9<R,G,B>( b.c[4], b.c[3], b.c[1] );
   dst[1][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[7] );
  }

  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[2], b.c[5] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );
  break;

 case 14:
 case 142:

  if (Diff( b.w[3], b.w[1] ))
  {
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  }
  else
  {
   dst[0][0] = Interpolate9<R,G,B>( b.c[4], b.c[3], b.c[1] );
   dst[0][1] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[5] );
  }

  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[6], b.c[7] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );
  break;

 case 67:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[2], b.c[5] );
  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[6], b.c[3] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[8], b.c[5] );
  break;

 case 70:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[0], b.c[3] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[6], b.c[3] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[8], b.c[5] );
  break;

 case 28:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[0], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[6], b.c[7] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[8], b.c[7] );
  break;

 case 152:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[0], b.c[1] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[2], b.c[1] );
  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[6], b.c[7] );
  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  break;

 case 194:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[0], b.c[3] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[2], b.c[5] );
  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[6], b.c[3] );
  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  break;

 case 98:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[0], b.c[3] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[2], b.c[5] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[8], b.c[5] );
  break;

 case 56:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[0], b.c[1] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[2], b.c[1] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[8], b.c[7] );
  break;

 case 25:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[2], b.c[1] );
  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[6], b.c[7] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[8], b.c[7] );
  break;

 case 26:
 case 31:

  if (Diff( b.w[3], b.w[1] ))
   dst[0][0] = b.c[4];
  else
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );

  if (Diff( b.w[1], b.w[5] ))
   dst[0][1] = b.c[4];
  else
   dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[6], b.c[7] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[8], b.c[7] );
  break;

 case 82:
 case 214:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[0], b.c[3] );

  if (Diff( b.w[1], b.w[5] ))
   dst[0][1] = b.c[4];
  else
   dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[6], b.c[3] );

  if (Diff( b.w[5], b.w[7] ))
   dst[1][1] = b.c[4];
  else
   dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 88:
 case 248:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[0], b.c[1] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[2], b.c[1] );

  if (Diff( b.w[7], b.w[3] ))
   dst[1][0] = b.c[4];
  else
   dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );

  if (Diff( b.w[5], b.w[7] ))
   dst[1][1] = b.c[4];
  else
   dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 74:
 case 107:

  if (Diff( b.w[3], b.w[1] ))
   dst[0][0] = b.c[4];
  else
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );

  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[2], b.c[5] );

  if (Diff( b.w[7], b.w[3] ))
   dst[1][0] = b.c[4];
  else
   dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );

  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[8], b.c[5] );
  break;

 case 27:

  if (Diff( b.w[3], b.w[1] ))
   dst[0][0] = b.c[4];
  else
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );

  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[6], b.c[7] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[8], b.c[7] );
  break;

 case 86:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[0], b.c[3] );

  if (Diff( b.w[1], b.w[5] ))
   dst[0][1] = b.c[4];
  else
   dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[6], b.c[3] );
  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 216:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[0], b.c[1] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[2], b.c[1] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );

  if (Diff( b.w[5], b.w[7] ))
   dst[1][1] = b.c[4];
  else
   dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 106:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[2], b.c[5] );

  if (Diff( b.w[7], b.w[3] ))
   dst[1][0] = b.c[4];
  else
   dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );

  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[8], b.c[5] );
  break;

 case 30:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );

  if (Diff( b.w[1], b.w[5] ))
   dst[0][1] = b.c[4];
  else
   dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[6], b.c[7] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[8], b.c[7] );
  break;

 case 210:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[0], b.c[3] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[6], b.c[3] );

  if (Diff( b.w[5], b.w[7] ))
   dst[1][1] = b.c[4];
  else
   dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 120:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[0], b.c[1] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[2], b.c[1] );

  if (Diff( b.w[7], b.w[3] ))
   dst[1][0] = b.c[4];
  else
   dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );

  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 75:

  if (Diff( b.w[3], b.w[1] ))
   dst[0][0] = b.c[4];
  else
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );

  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[2], b.c[5] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[8], b.c[5] );
  break;

 case 29:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[6], b.c[7] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[8], b.c[7] );
  break;

 case 198:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[0], b.c[3] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[6], b.c[3] );
  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  break;

 case 184:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[0], b.c[1] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[2], b.c[1] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  break;

 case 99:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[2], b.c[5] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[8], b.c[5] );
  break;

 case 57:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[2], b.c[1] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[8], b.c[7] );
  break;

 case 71:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[6], b.c[3] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[8], b.c[5] );
  break;

 case 156:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[0], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[6], b.c[7] );
  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  break;

 case 226:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[0], b.c[3] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[2], b.c[5] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  break;

 case 60:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[0], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[8], b.c[7] );
  break;

 case 195:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[2], b.c[5] );
  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[6], b.c[3] );
  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  break;

 case 102:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[0], b.c[3] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[8], b.c[5] );
  break;

 case 153:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[2], b.c[1] );
  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[6], b.c[7] );
  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  break;

 case 58:

  if (Diff( b.w[3], b.w[1] ))
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  else
   dst[0][0] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[1] );

  if (Diff( b.w[1], b.w[5] ))
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  else
   dst[0][1] = Interpolate7<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[8], b.c[7] );
  break;

 case 83:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );

  if (Diff( b.w[1], b.w[5] ))
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  else
   dst[0][1] = Interpolate7<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[6], b.c[3] );

  if (Diff( b.w[5], b.w[7] ))
   dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  else
   dst[1][1] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 92:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[0], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );

  if (Diff( b.w[7], b.w[3] ))
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  else
   dst[1][0] = Interpolate7<R,G,B>( b.c[4], b.c[7], b.c[3] );

  if (Diff( b.w[5], b.w[7] ))
   dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  else
   dst[1][1] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 202:

  if (Diff( b.w[3], b.w[1] ))
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  else
   dst[0][0] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[1] );

  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[2], b.c[5] );

  if (Diff( b.w[7], b.w[3] ))
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  else
   dst[1][0] = Interpolate7<R,G,B>( b.c[4], b.c[7], b.c[3] );

  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  break;

 case 78:

  if (Diff( b.w[3], b.w[1] ))
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  else
   dst[0][0] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[1] );

  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[5] );

  if (Diff( b.w[7], b.w[3] ))
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  else
   dst[1][0] = Interpolate7<R,G,B>( b.c[4], b.c[7], b.c[3] );

  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[8], b.c[5] );
  break;

 case 154:

  if (Diff( b.w[3], b.w[1] ))
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  else
   dst[0][0] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[1] );

  if (Diff( b.w[1], b.w[5] ))
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  else
   dst[0][1] = Interpolate7<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[6], b.c[7] );
  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  break;

 case 114:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[0], b.c[3] );

  if (Diff( b.w[1], b.w[5] ))
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  else
   dst[0][1] = Interpolate7<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );

  if (Diff( b.w[5], b.w[7] ))
   dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  else
   dst[1][1] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 89:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[2], b.c[1] );

  if (Diff( b.w[7], b.w[3] ))
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  else
   dst[1][0] = Interpolate7<R,G,B>( b.c[4], b.c[7], b.c[3] );

  if (Diff( b.w[5], b.w[7] ))
   dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  else
   dst[1][1] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 90:

  if (Diff( b.w[3], b.w[1] ))
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  else
   dst[0][0] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[1] );

  if (Diff( b.w[1], b.w[5] ))
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  else
   dst[0][1] = Interpolate7<R,G,B>( b.c[4], b.c[1], b.c[5] );

  if (Diff( b.w[7], b.w[3] ))
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  else
   dst[1][0] = Interpolate7<R,G,B>( b.c[4], b.c[7], b.c[3] );

  if (Diff( b.w[5], b.w[7] ))
   dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  else
   dst[1][1] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 55:
 case 23:

  if (Diff( b.w[1], b.w[5] ))
  {
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
   dst[0][1] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[3] );
   dst[0][1] = Interpolate9<R,G,B>( b.c[4], b.c[1], b.c[5] );
  }

  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[8], b.c[7] );
  break;

 case 182:
 case 150:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[0], b.c[3] );

  if (Diff( b.w[1], b.w[5] ))
  {
   dst[0][1] = b.c[4];
   dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  }
  else
  {
   dst[0][1] = Interpolate9<R,G,B>( b.c[4], b.c[1], b.c[5] );
   dst[1][1] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[7] );
  }

  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  break;

 case 213:
 case 212:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );

  if (Diff( b.w[5], b.w[7] ))
  {
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[1][1] = b.c[4];
  }
  else
  {
   dst[0][1] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[1] );
   dst[1][1] = Interpolate9<R,G,B>( b.c[4], b.c[5], b.c[7] );
  }

  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[6], b.c[3] );
  break;

 case 241:
 case 240:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[2], b.c[1] );

  if (Diff( b.w[5], b.w[7] ))
  {
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
   dst[1][1] = b.c[4];
  }
  else
  {
   dst[1][0] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[3] );
   dst[1][1] = Interpolate9<R,G,B>( b.c[4], b.c[5], b.c[7] );
  }
  break;

 case 236:
 case 232:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[0], b.c[1] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  if (Diff( b.w[7], b.w[3] ))
  {
   dst[1][0] = b.c[4];
   dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  }
  else
  {
   dst[1][0] = Interpolate9<R,G,B>( b.c[4], b.c[7], b.c[3] );
   dst[1][1] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[5] );
  }
  break;

 case 109:
 case 105:

  if (Diff( b.w[7], b.w[3] ))
  {
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[1][0] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[1] );
   dst[1][0] = Interpolate9<R,G,B>( b.c[4], b.c[7], b.c[3] );
  }

  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[8], b.c[5] );
  break;

 case 171:
 case 43:

  if (Diff( b.w[3], b.w[1] ))
  {
   dst[0][0] = b.c[4];
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  }
  else
  {
   dst[0][0] = Interpolate9<R,G,B>( b.c[4], b.c[3], b.c[1] );
   dst[1][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[7] );
  }

  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[2], b.c[5] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );
  break;

 case 143:
 case 15:

  if (Diff( b.w[3], b.w[1] ))
  {
   dst[0][0] = b.c[4];
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  }
  else
  {
   dst[0][0] = Interpolate9<R,G,B>( b.c[4], b.c[3], b.c[1] );
   dst[0][1] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[5] );
  }

  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[6], b.c[7] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );
  break;

 case 124:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[0], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );

  if (Diff( b.w[7], b.w[3] ))
   dst[1][0] = b.c[4];
  else
   dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );

  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 203:

  if (Diff( b.w[3], b.w[1] ))
   dst[0][0] = b.c[4];
  else
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );

  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[2], b.c[5] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  break;

 case 62:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );

  if (Diff( b.w[1], b.w[5] ))
   dst[0][1] = b.c[4];
  else
   dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[8], b.c[7] );
  break;

 case 211:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[6], b.c[3] );

  if (Diff( b.w[5], b.w[7] ))
   dst[1][1] = b.c[4];
  else
   dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 118:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[0], b.c[3] );

  if (Diff( b.w[1], b.w[5] ))
   dst[0][1] = b.c[4];
  else
   dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 217:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[2], b.c[1] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );

  if (Diff( b.w[5], b.w[7] ))
   dst[1][1] = b.c[4];
  else
   dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 110:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[5] );

  if (Diff( b.w[7], b.w[3] ))
   dst[1][0] = b.c[4];
  else
   dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );

  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[8], b.c[5] );
  break;

 case 155:

  if (Diff( b.w[3], b.w[1] ))
   dst[0][0] = b.c[4];
  else
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );

  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[6], b.c[7] );
  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  break;

 case 188:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[0], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  break;

 case 185:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[2], b.c[1] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  break;

 case 61:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[8], b.c[7] );
  break;

 case 157:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[6], b.c[7] );
  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  break;

 case 103:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[8], b.c[5] );
  break;

 case 227:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[2], b.c[5] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  break;

 case 230:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[0], b.c[3] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  break;

 case 199:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[6], b.c[3] );
  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  break;

 case 220:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[0], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );

  if (Diff( b.w[7], b.w[3] ))
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  else
   dst[1][0] = Interpolate7<R,G,B>( b.c[4], b.c[7], b.c[3] );

  if (Diff( b.w[5], b.w[7] ))
   dst[1][1] = b.c[4];
  else
   dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 158:

  if (Diff( b.w[3], b.w[1] ))
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  else
   dst[0][0] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[1] );

  if (Diff( b.w[1], b.w[5] ))
   dst[0][1] = b.c[4];
  else
   dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[6], b.c[7] );
  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  break;

 case 234:

  if (Diff( b.w[3], b.w[1] ))
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  else
   dst[0][0] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[1] );

  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[2], b.c[5] );

  if (Diff( b.w[7], b.w[3] ))
   dst[1][0] = b.c[4];
  else
   dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );

  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  break;

 case 242:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[0], b.c[3] );

  if (Diff( b.w[1], b.w[5] ))
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  else
   dst[0][1] = Interpolate7<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );

  if (Diff( b.w[5], b.w[7] ))
   dst[1][1] = b.c[4];
  else
   dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 59:

  if (Diff( b.w[3], b.w[1] ))
   dst[0][0] = b.c[4];
  else
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );

  if (Diff( b.w[1], b.w[5] ))
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  else
   dst[0][1] = Interpolate7<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[8], b.c[7] );
  break;

 case 121:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[2], b.c[1] );

  if (Diff( b.w[7], b.w[3] ))
   dst[1][0] = b.c[4];
  else
   dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );

  if (Diff( b.w[5], b.w[7]))
   dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  else
   dst[1][1] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 87:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );

  if (Diff( b.w[1], b.w[5] ))
   dst[0][1] = b.c[4];
  else
   dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[6], b.c[3] );

  if (Diff( b.w[5], b.w[7] ))
   dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  else
   dst[1][1] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 79:

  if (Diff( b.w[3], b.w[1] ))
   dst[0][0] = b.c[4];
  else
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );

  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[5] );

  if (Diff( b.w[7], b.w[3] ))
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  else
   dst[1][0] = Interpolate7<R,G,B>( b.c[4], b.c[7], b.c[3] );

  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[8], b.c[5] );
  break;

 case 122:

  if (Diff( b.w[3], b.w[1] ))
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  else
   dst[0][0] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[1] );

  if (Diff( b.w[1], b.w[5] ))
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  else
   dst[0][1] = Interpolate7<R,G,B>( b.c[4], b.c[1], b.c[5] );

  if (Diff( b.w[7], b.w[3] ))
   dst[1][0] = b.c[4];
  else
   dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );

  if (Diff( b.w[5], b.w[7] ))
   dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  else
   dst[1][1] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 94:

  if (Diff( b.w[3], b.w[1] ))
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  else
   dst[0][0] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[1] );

  if (Diff( b.w[1], b.w[5] ))
   dst[0][1] = b.c[4];
  else
   dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  if (Diff( b.w[7], b.w[3] ))
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  else
   dst[1][0] = Interpolate7<R,G,B>( b.c[4], b.c[7], b.c[3] );

  if (Diff( b.w[5], b.w[7] ))
   dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  else
   dst[1][1] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 218:

  if (Diff( b.w[3], b.w[1] ))
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  else
   dst[0][0] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[1] );

  if (Diff( b.w[1], b.w[5] ))
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  else
   dst[0][1] = Interpolate7<R,G,B>( b.c[4], b.c[1], b.c[5] );

  if (Diff( b.w[7], b.w[3] ))
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  else
   dst[1][0] = Interpolate7<R,G,B>( b.c[4], b.c[7], b.c[3] );

  if (Diff( b.w[5], b.w[7] ))
   dst[1][1] = b.c[4];
  else
   dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 91:

  if (Diff( b.w[3], b.w[1] ))
   dst[0][0] = b.c[4];
  else
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );

  if (Diff( b.w[1], b.w[5] ))
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  else
   dst[0][1] = Interpolate7<R,G,B>( b.c[4], b.c[1], b.c[5] );

  if (Diff( b.w[7], b.w[3] ))
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  else
   dst[1][0] = Interpolate7<R,G,B>( b.c[4], b.c[7], b.c[3] );

  if (Diff( b.w[5], b.w[7] ))
   dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  else
   dst[1][1] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 229:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  break;

 case 167:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );
  break;

 case 173:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );
  break;

 case 181:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  break;

 case 186:

  if (Diff( b.w[3], b.w[1] ))
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  else
   dst[0][0] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[1] );

  if (Diff( b.w[1], b.w[5] ))
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  else
   dst[0][1] = Interpolate7<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  break;

 case 115:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );

  if (Diff( b.w[1], b.w[5] ))
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  else
   dst[0][1] = Interpolate7<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );

  if (Diff( b.w[5], b.w[7] ))
   dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  else
   dst[1][1] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 93:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );

  if (Diff( b.w[7], b.w[3] ))
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  else
   dst[1][0] = Interpolate7<R,G,B>( b.c[4], b.c[7], b.c[3] );

  if (Diff( b.w[5], b.w[7] ))
   dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  else
   dst[1][1] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 206:

  if (Diff( b.w[3], b.w[1] ))
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  else
   dst[0][0] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[1] );

  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[5] );

  if (Diff( b.w[7], b.w[3] ))
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  else
   dst[1][0] = Interpolate7<R,G,B>( b.c[4], b.c[7], b.c[3] );

  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  break;

 case 205:
 case 201:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  if (Diff( b.w[7], b.w[3] ))
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  else
   dst[1][0] = Interpolate7<R,G,B>( b.c[4], b.c[7], b.c[3] );

  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  break;

 case 174:
 case 46:

  if (Diff( b.w[3], b.w[1] ))
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  else
   dst[0][0] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[1] );

  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );
  break;

 case 179:
 case 147:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );

  if (Diff( b.w[1], b.w[5] ))
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  else
   dst[0][1] = Interpolate7<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  break;

 case 117:
 case 116:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );

  if (Diff( b.w[5], b.w[7] ))
   dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  else
   dst[1][1] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 189:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  break;

 case 231:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  break;

 case 126:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );

  if (Diff( b.w[1], b.w[5] ))
   dst[0][1] = b.c[4];
  else
   dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  if (Diff( b.w[7], b.w[3] ))
   dst[1][0] = b.c[4];
  else
   dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );

  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 219:

  if (Diff( b.w[3], b.w[1] ))
   dst[0][0] = b.c[4];
  else
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );

  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );

  if (Diff( b.w[5], b.w[7] ))
   dst[1][1] = b.c[4];
  else
   dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 125:

  if (Diff( b.w[7], b.w[3] ))
  {
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[1][0] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[1] );
   dst[1][0] = Interpolate9<R,G,B>( b.c[4], b.c[7], b.c[3] );
  }

  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 221:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );

  if (Diff( b.w[5], b.w[7] ))
  {
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[1][1] = b.c[4];
  }
  else
  {
   dst[0][1] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[1] );
   dst[1][1] = Interpolate9<R,G,B>( b.c[4], b.c[5], b.c[7] );
  }

  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  break;

 case 207:

  if (Diff( b.w[3], b.w[1] ))
  {
   dst[0][0] = b.c[4];
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  }
  else
  {
   dst[0][0] = Interpolate9<R,G,B>( b.c[4], b.c[3], b.c[1] );
   dst[0][1] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[5] );
  }

  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  break;

 case 238:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[5] );

  if (Diff( b.w[7], b.w[3] ))
  {
   dst[1][0] = b.c[4];
   dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  }
  else
  {
   dst[1][0] = Interpolate9<R,G,B>( b.c[4], b.c[7], b.c[3] );
   dst[1][1] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[5] );
  }
  break;

 case 190:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );

  if (Diff( b.w[1], b.w[5] ))
  {
   dst[0][1] = b.c[4];
   dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  }
  else
  {
   dst[0][1] = Interpolate9<R,G,B>( b.c[4], b.c[1], b.c[5] );
   dst[1][1] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[7] );
  }

  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  break;

 case 187:

  if (Diff( b.w[3], b.w[1] ))
  {
   dst[0][0] = b.c[4];
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  }
  else
  {
   dst[0][0] = Interpolate9<R,G,B>( b.c[4], b.c[3], b.c[1] );
   dst[1][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[7] );
  }

  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  break;

 case 243:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[2] );

  if (Diff( b.w[5], b.w[7] ))
  {
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
   dst[1][1] = b.c[4];
  }
  else
  {
   dst[1][0] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[3] );
   dst[1][1] = Interpolate9<R,G,B>( b.c[4], b.c[5], b.c[7] );
  }
  break;

 case 119:

  if (Diff( b.w[1], b.w[5] ))
  {
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
   dst[0][1] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[3] );
   dst[0][1] = Interpolate9<R,G,B>( b.c[4], b.c[1], b.c[5] );
  }

  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 237:
 case 233:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  if (Diff( b.w[7], b.w[3] ))
   dst[1][0] = b.c[4];
  else
   dst[1][0] = Interpolate10<R,G,B>( b.c[4], b.c[7], b.c[3] );

  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  break;

 case 175:
 case 47:

  if (Diff( b.w[3], b.w[1] ))
   dst[0][0] = b.c[4];
  else
   dst[0][0] = Interpolate10<R,G,B>( b.c[4], b.c[3], b.c[1] );

  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );
  break;

 case 183:
 case 151:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );

  if (Diff( b.w[1], b.w[5] ))
   dst[0][1] = b.c[4];
  else
   dst[0][1] = Interpolate10<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  break;

 case 245:
 case 244:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );

  if (Diff( b.w[5], b.w[7] ))
   dst[1][1] = b.c[4];
  else
   dst[1][1] = Interpolate10<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 250:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[2] );

  if (Diff( b.w[7], b.w[3] ))
   dst[1][0] = b.c[4];
  else
   dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );

  if (Diff( b.w[5], b.w[7] ))
   dst[1][1] = b.c[4];
  else
   dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 123:

  if (Diff( b.w[3], b.w[1] ))
   dst[0][0] = b.c[4];
  else
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );

  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[2] );

  if (Diff( b.w[7], b.w[3] ))
   dst[1][0] = b.c[4];
  else
   dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );

  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 95:

  if (Diff( b.w[3], b.w[1] ))
   dst[0][0] = b.c[4];
  else
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );

  if (Diff( b.w[1], b.w[5] ))
   dst[0][1] = b.c[4];
  else
   dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 222:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );

  if (Diff( b.w[1], b.w[5] ))
   dst[0][1] = b.c[4];
  else
   dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );

  if (Diff( b.w[5], b.w[7] ))
   dst[1][1] = b.c[4];
  else
   dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 252:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[0], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );

  if (Diff( b.w[7], b.w[3] ))
   dst[1][0] = b.c[4];
  else
   dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );

  if (Diff( b.w[5], b.w[7] ))
   dst[1][1] = b.c[4];
  else
   dst[1][1] = Interpolate10<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 249:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[2], b.c[1] );

  if (Diff( b.w[7], b.w[3] ))
   dst[1][0] = b.c[4];
  else
   dst[1][0] = Interpolate10<R,G,B>( b.c[4], b.c[7], b.c[3] );

  if (Diff( b.w[5], b.w[7] ))
   dst[1][1] = b.c[4];
  else
   dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 235:

  if (Diff( b.w[3], b.w[1] ))
   dst[0][0] = b.c[4];
  else
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );

  dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[2], b.c[5] );

  if (Diff( b.w[7], b.w[3] ))
   dst[1][0] = b.c[4];
  else
   dst[1][0] = Interpolate10<R,G,B>( b.c[4], b.c[7], b.c[3] );

  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  break;

 case 111:

  if (Diff( b.w[3], b.w[1] ))
   dst[0][0] = b.c[4];
  else
   dst[0][0] = Interpolate10<R,G,B>( b.c[4], b.c[3], b.c[1] );

  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[5] );

  if (Diff( b.w[7], b.w[3] ))
   dst[1][0] = b.c[4];
  else
   dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );

  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[8], b.c[5] );
  break;

 case 63:

  if (Diff( b.w[3], b.w[1] ))
   dst[0][0] = b.c[4];
  else
   dst[0][0] = Interpolate10<R,G,B>( b.c[4], b.c[3], b.c[1] );

  if (Diff( b.w[1], b.w[5] ))
   dst[0][1] = b.c[4];
  else
   dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[8], b.c[7] );
  break;

 case 159:

  if (Diff( b.w[3], b.w[1] ))
   dst[0][0] = b.c[4];
  else
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );

  if (Diff( b.w[1], b.w[5] ))
   dst[0][1] = b.c[4];
  else
   dst[0][1] = Interpolate10<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[6], b.c[7] );
  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  break;

 case 215:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );

  if (Diff( b.w[1], b.w[5] ))
   dst[0][1] = b.c[4];
  else
   dst[0][1] = Interpolate10<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[6], b.c[3] );

  if (Diff( b.w[5], b.w[7] ))
   dst[1][1] = b.c[4];
  else
   dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 246:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[0], b.c[3] );

  if (Diff( b.w[1], b.w[5] ))
   dst[0][1] = b.c[4];
  else
   dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );

  if (Diff( b.w[5], b.w[7] ))
   dst[1][1] = b.c[4];
  else
   dst[1][1] = Interpolate10<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 254:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );

  if (Diff( b.w[1], b.w[5] ))
   dst[0][1] = b.c[4];
  else
   dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  if (Diff( b.w[7], b.w[3] ))
   dst[1][0] = b.c[4];
  else
   dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );

  if (Diff( b.w[5], b.w[7] ))
   dst[1][1] = b.c[4];
  else
   dst[1][1] = Interpolate10<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 253:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );

  if (Diff( b.w[7], b.w[3] ))
   dst[1][0] = b.c[4];
  else
   dst[1][0] = Interpolate10<R,G,B>( b.c[4], b.c[7], b.c[3] );

  if (Diff( b.w[5], b.w[7] ))
   dst[1][1] = b.c[4];
  else
   dst[1][1] = Interpolate10<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 251:

  if (Diff( b.w[3], b.w[1] ))
   dst[0][0] = b.c[4];
  else
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );

  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[2] );

  if (Diff( b.w[7], b.w[3] ))
   dst[1][0] = b.c[4];
  else
   dst[1][0] = Interpolate10<R,G,B>( b.c[4], b.c[7], b.c[3] );

  if (Diff( b.w[5], b.w[7] ))
   dst[1][1] = b.c[4];
  else
   dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 239:

  if (Diff( b.w[3], b.w[1] ))
   dst[0][0] = b.c[4];
  else
   dst[0][0] = Interpolate10<R,G,B>( b.c[4], b.c[3], b.c[1] );

  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[5] );

  if (Diff( b.w[7], b.w[3] ))
   dst[1][0] = b.c[4];
  else
   dst[1][0] = Interpolate10<R,G,B>( b.c[4], b.c[7], b.c[3] );

  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  break;

 case 127:

  if (Diff( b.w[3], b.w[1] ))
   dst[0][0] = b.c[4];
  else
   dst[0][0] = Interpolate10<R,G,B>( b.c[4], b.c[3], b.c[1] );

  if (Diff( b.w[1], b.w[5] ))
   dst[0][1] = b.c[4];
  else
   dst[0][1] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  if (Diff( b.w[7], b.w[3] ))
   dst[1][0] = b.c[4];
  else
   dst[1][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );

  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 191:

  if (Diff( b.w[3], b.w[1] ))
   dst[0][0] = b.c[4];
  else
   dst[0][0] = Interpolate10<R,G,B>( b.c[4], b.c[3], b.c[1] );

  if (Diff( b.w[1], b.w[5] ))
   dst[0][1] = b.c[4];
  else
   dst[0][1] = Interpolate10<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[1][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  break;

 case 223:

  if (Diff( b.w[3], b.w[1] ))
   dst[0][0] = b.c[4];
  else
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );

  if (Diff( b.w[1], b.w[5] ))
   dst[0][1] = b.c[4];
  else
   dst[0][1] = Interpolate10<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );

  if (Diff( b.w[5], b.w[7] ))
   dst[1][1] = b.c[4];
  else
   dst[1][1] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 247:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );

  if (Diff( b.w[1], b.w[5] ))
   dst[0][1] = b.c[4];
  else
   dst[0][1] = Interpolate10<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );

  if (Diff( b.w[5], b.w[7] ))
   dst[1][1] = b.c[4];
  else
   dst[1][1] = Interpolate10<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 255:

  if (Diff( b.w[3], b.w[1] ))
   dst[0][0] = b.c[4];
  else
   dst[0][0] = Interpolate10<R,G,B>( b.c[4], b.c[3], b.c[1] );

  if (Diff( b.w[1], b.w[5] ))
   dst[0][1] = b.c[4];
  else
   dst[0][1] = Interpolate10<R,G,B>( b.c[4], b.c[1], b.c[5] );

  if (Diff( b.w[7], b.w[3] ))
   dst[1][0] = b.c[4];
  else
   dst[1][0] = Interpolate10<R,G,B>( b.c[4], b.c[7], b.c[3] );

  if (Diff( b.w[5], b.w[7] ))
   dst[1][1] = b.c[4];
  else
   dst[1][1] = Interpolate10<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;


}
# 203 "/Users/Jorrit/iOS/nestopia/core/NstVideoFilterHqX.cpp" 2

     }

     dst[0] = reinterpret_cast<T*>(reinterpret_cast<byte*>(dst[0]) + pitch);
     dst[1] = reinterpret_cast<T*>(reinterpret_cast<byte*>(dst[1]) + pitch);
    }
   }

   template<typename T,dword R,dword G,dword B>
   void Renderer::FilterHqX::Blit3x(const Input& input,const Output& output,uint) const
   {
    const byte* __restrict__ src = reinterpret_cast<const byte*>(input.pixels);
    const long pitch = (output.pitch * 2) + output.pitch - (WIDTH*3 * sizeof(T));

    T* __restrict__ dst[3] =
    {
     static_cast<T*>(output.pixels) - 3,
     reinterpret_cast<T*>(static_cast<byte*>(output.pixels) + output.pitch) - 3,
     reinterpret_cast<T*>(static_cast<byte*>(output.pixels) + output.pitch * 2) - 3
    };

    for (uint y=HEIGHT; y; --y)
    {
     const uint lines[2] =
     {
      y < HEIGHT ? WIDTH * sizeof(Input::Pixel) : 0,
      y > 1 ? WIDTH * sizeof(Input::Pixel) : 0
     };

     Buffer<T> b;

     b.w[2] = (b.w[1] = input.palette[*reinterpret_cast<const Input::Pixel*>(src - lines[0])]);
     b.w[5] = (b.w[4] = input.palette[*reinterpret_cast<const Input::Pixel*>(src)]);
     b.w[8] = (b.w[7] = input.palette[*reinterpret_cast<const Input::Pixel*>(src + lines[1])]);

     for (uint x=WIDTH; x; )
     {
      src += sizeof(Input::Pixel);
      dst[0] += 3;
      dst[1] += 3;
      dst[2] += 3;

      b.w[0] = b.w[1];
      b.w[1] = b.w[2];
      b.w[3] = b.w[4];
      b.w[4] = b.w[5];
      b.w[6] = b.w[7];
      b.w[7] = b.w[8];

      if (--x)
      {
       b.w[2] = input.palette[*reinterpret_cast<const Input::Pixel*>(src - lines[0])];
       b.w[5] = input.palette[*reinterpret_cast<const Input::Pixel*>(src)];
       b.w[8] = input.palette[*reinterpret_cast<const Input::Pixel*>(src + lines[1])];
      }

      b.Convert( lut );

      const uint yuv5 = lut.yuv[b.w[4]];


# 1 "/Users/Jorrit/iOS/nestopia/core/NstVideoFilterHq3x.inl" 1
# 26 "/Users/Jorrit/iOS/nestopia/core/NstVideoFilterHq3x.inl"
switch
(
 (b.w[4] != b.w[0] && ((yuv5 - lut.yuv[b.w[0]]) & Lut::YUV_MASK) ? 0x01U : 0x0U) |
 (b.w[4] != b.w[1] && ((yuv5 - lut.yuv[b.w[1]]) & Lut::YUV_MASK) ? 0x02U : 0x0U) |
 (b.w[4] != b.w[2] && ((yuv5 - lut.yuv[b.w[2]]) & Lut::YUV_MASK) ? 0x04U : 0x0U) |
 (b.w[4] != b.w[3] && ((yuv5 - lut.yuv[b.w[3]]) & Lut::YUV_MASK) ? 0x08U : 0x0U) |
 (b.w[4] != b.w[5] && ((yuv5 - lut.yuv[b.w[5]]) & Lut::YUV_MASK) ? 0x10U : 0x0U) |
 (b.w[4] != b.w[6] && ((yuv5 - lut.yuv[b.w[6]]) & Lut::YUV_MASK) ? 0x20U : 0x0U) |
 (b.w[4] != b.w[7] && ((yuv5 - lut.yuv[b.w[7]]) & Lut::YUV_MASK) ? 0x40U : 0x0U) |
 (b.w[4] != b.w[8] && ((yuv5 - lut.yuv[b.w[8]]) & Lut::YUV_MASK) ? 0x80U : 0x0U)
)
# 82 "/Users/Jorrit/iOS/nestopia/core/NstVideoFilterHq3x.inl"
{
 case 0:
 case 1:
 case 4:
 case 32:
 case 128:
 case 5:
 case 132:
 case 160:
 case 33:
 case 129:
 case 36:
 case 133:
 case 164:
 case 161:
 case 37:
 case 165:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );
  break;

 case 2:
 case 34:
 case 130:
 case 162:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = b.c[4];
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );
  break;

 case 16:
 case 17:
 case 48:
 case 49:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 64:
 case 65:
 case 68:
 case 69:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 8:
 case 12:
 case 136:
 case 140:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );
  break;

 case 3:
 case 35:
 case 131:
 case 163:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = b.c[4];
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );
  break;

 case 6:
 case 38:
 case 134:
 case 166:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = b.c[4];
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );
  break;

 case 20:
 case 21:
 case 52:
 case 53:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 144:
 case 145:
 case 176:
 case 177:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  break;

 case 192:
 case 193:
 case 196:
 case 197:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  break;

 case 96:
 case 97:
 case 100:
 case 101:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 40:
 case 44:
 case 168:
 case 172:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );
  break;

 case 9:
 case 13:
 case 137:
 case 141:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );
  break;

 case 18:
 case 50:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );

  if (Diff(b.w[1], b.w[5]))
  {
   dst[0][1] = b.c[4];
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
   dst[1][2] = b.c[4];
  }
  else
  {
   dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
   dst[0][2] = Interpolate4<R,G,B>( b.c[4], b.c[1], b.c[5] );
   dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  }

  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[2][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 80:
 case 81:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );

  if (Diff(b.w[5], b.w[7]))
  {
   dst[1][2] = b.c[4];
   dst[2][1] = b.c[4];
   dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  }
  else
  {
   dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
   dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
   dst[2][2] = Interpolate4<R,G,B>( b.c[4], b.c[5], b.c[7] );
  }
  break;

 case 72:
 case 76:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );

  if (Diff(b.w[7], b.w[3]))
  {
   dst[1][0] = b.c[4];
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
   dst[2][1] = b.c[4];
  }
  else
  {
   dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
   dst[2][0] = Interpolate4<R,G,B>( b.c[4], b.c[7], b.c[3] );
   dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  }

  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 10:
 case 138:

  if (Diff(b.w[3], b.w[1]))
  {
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
   dst[0][1] = b.c[4];
   dst[1][0] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate4<R,G,B>( b.c[4], b.c[3], b.c[1] );
   dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
   dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  }

  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );
  break;

 case 66:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = b.c[4];
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 24:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 7:
 case 39:
 case 135:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = b.c[4];
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );
  break;

 case 148:
 case 149:
 case 180:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  break;

 case 224:
 case 228:
 case 225:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  break;

 case 41:
 case 169:
 case 45:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );
  break;

 case 22:
 case 54:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );

  if (Diff(b.w[1], b.w[5]))
  {
   dst[0][1] = b.c[4];
   dst[0][2] = b.c[4];
   dst[1][2] = b.c[4];
  }
  else
  {
   dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
   dst[0][2] = Interpolate4<R,G,B>( b.c[4], b.c[1], b.c[5] );
   dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  }

  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[2][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 208:
 case 209:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );

  if (Diff(b.w[5], b.w[7]))
  {
   dst[1][2] = b.c[4];
   dst[2][1] = b.c[4];
   dst[2][2] = b.c[4];
  }
  else
  {
   dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
   dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
   dst[2][2] = Interpolate4<R,G,B>( b.c[4], b.c[5], b.c[7] );
  }
  break;

 case 104:
 case 108:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );

  if (Diff(b.w[7], b.w[3]))
  {
   dst[1][0] = b.c[4];
   dst[2][0] = b.c[4];
   dst[2][1] = b.c[4];
  }
  else
  {
   dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
   dst[2][0] = Interpolate4<R,G,B>( b.c[4], b.c[7], b.c[3] );
   dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  }

  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 11:
 case 139:

  if (Diff(b.w[3], b.w[1]))
  {
   dst[0][0] = b.c[4];
   dst[0][1] = b.c[4];
   dst[1][0] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate4<R,G,B>( b.c[4], b.c[3], b.c[1] );
   dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
   dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  }

  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );
  break;

 case 19:
 case 51:

  if (Diff(b.w[1], b.w[5]))
  {
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
   dst[0][1] = b.c[4];
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
   dst[1][2] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );
   dst[0][1] = Interpolate1<R,G,B>( b.c[1], b.c[4] );
   dst[0][2] = Interpolate5<R,G,B>( b.c[1], b.c[5] );
   dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  }

  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[2][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 146:
 case 178:

  if (Diff(b.w[1], b.w[5]))
  {
   dst[0][1] = b.c[4];
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
   dst[1][2] = b.c[4];
   dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  }
  else
  {
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[0][2] = Interpolate5<R,G,B>( b.c[1], b.c[5] );
   dst[1][2] = Interpolate1<R,G,B>( b.c[5], b.c[4] );
   dst[2][2] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );
  }

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[2][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  break;

 case 84:
 case 85:

  if (Diff(b.w[5], b.w[7]))
  {
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[1][2] = b.c[4];
   dst[2][1] = b.c[4];
   dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  }
  else
  {
   dst[0][2] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
   dst[1][2] = Interpolate1<R,G,B>( b.c[5], b.c[4] );
   dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
   dst[2][2] = Interpolate5<R,G,B>( b.c[5], b.c[7] );
  }

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  break;

 case 112:
 case 113:

  if (Diff(b.w[5], b.w[7]))
  {
   dst[1][2] = b.c[4];
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
   dst[2][1] = b.c[4];
   dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  }
  else
  {
   dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
   dst[2][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
   dst[2][1] = Interpolate1<R,G,B>( b.c[7], b.c[4] );
   dst[2][2] = Interpolate5<R,G,B>( b.c[5], b.c[7] );
  }

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  break;

 case 200:
 case 204:

  if (Diff(b.w[7], b.w[3]))
  {
   dst[1][0] = b.c[4];
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
   dst[2][1] = b.c[4];
   dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  }
  else
  {
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
   dst[2][0] = Interpolate5<R,G,B>( b.c[7], b.c[3]);
   dst[2][1] = Interpolate1<R,G,B>( b.c[7], b.c[4] );
   dst[2][2] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );
  }

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  break;

 case 73:
 case 77:

  if (Diff(b.w[7], b.w[3]))
  {
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[1][0] = b.c[4];
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
   dst[2][1] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );
   dst[1][0] = Interpolate1<R,G,B>( b.c[3], b.c[4] );
   dst[2][0] = Interpolate5<R,G,B>( b.c[7], b.c[3]);
   dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  }

  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 42:
 case 170:

  if (Diff(b.w[3], b.w[1]))
  {
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
   dst[0][1] = b.c[4];
   dst[1][0] = b.c[4];
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  }
  else
  {
   dst[0][0] = Interpolate5<R,G,B>( b.c[3], b.c[1] );
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[1][0] = Interpolate1<R,G,B>( b.c[3], b.c[4] );
   dst[2][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  }

  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );
  break;

 case 14:
 case 142:

  if (Diff(b.w[3], b.w[1]))
  {
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
   dst[0][1] = b.c[4];
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
   dst[1][0] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate5<R,G,B>( b.c[3], b.c[1] );
   dst[0][1] = Interpolate1<R,G,B>( b.c[1], b.c[4] );
   dst[0][2] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  }

  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );
  break;

 case 67:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = b.c[4];
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 70:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = b.c[4];
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 28:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 152:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  break;

 case 194:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = b.c[4];
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  break;

 case 98:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = b.c[4];
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 56:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 25:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 26:
 case 31:

  if (Diff(b.w[3], b.w[1]))
  {
   dst[0][0] = b.c[4];
   dst[1][0] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate4<R,G,B>( b.c[4], b.c[3], b.c[1] );
   dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  }

  dst[0][1] = b.c[4];

  if (Diff(b.w[1], b.w[5]))
  {
   dst[0][2] = b.c[4];
   dst[1][2] = b.c[4];
  }
  else
  {
   dst[0][2] = Interpolate4<R,G,B>( b.c[4], b.c[1], b.c[5] );
   dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  }

  dst[1][1] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 82:
 case 214:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );

  if (Diff(b.w[1], b.w[5]))
  {
   dst[0][1] = b.c[4];
   dst[0][2] = b.c[4];
  }
  else
  {
   dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
   dst[0][2] = Interpolate4<R,G,B>( b.c[4], b.c[1], b.c[5] );
  }

  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );

  if (Diff(b.w[5], b.w[7]))
  {
   dst[2][1] = b.c[4];
   dst[2][2] = b.c[4];
  }
  else
  {
   dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
   dst[2][2] = Interpolate4<R,G,B>( b.c[4], b.c[5], b.c[7] );
  }
  break;

 case 88:
 case 248:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][1] = b.c[4];

  if (Diff(b.w[7], b.w[3]))
  {
   dst[1][0] = b.c[4];
   dst[2][0] = b.c[4];
  }
  else
  {
   dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
   dst[2][0] = Interpolate4<R,G,B>( b.c[4], b.c[7], b.c[3] );
  }

  dst[2][1] = b.c[4];

  if (Diff(b.w[5], b.w[7]))
  {
   dst[1][2] = b.c[4];
   dst[2][2] = b.c[4];
  }
  else
  {
   dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
   dst[2][2] = Interpolate4<R,G,B>( b.c[4], b.c[5], b.c[7] );
  }
  break;

 case 74:
 case 107:

  if (Diff(b.w[3], b.w[1]))
  {
   dst[0][0] = b.c[4];
   dst[0][1] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate4<R,G,B>( b.c[4], b.c[3], b.c[1] );
   dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  }

  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );

  if (Diff(b.w[7], b.w[3]))
  {
   dst[2][0] = b.c[4];
   dst[2][1] = b.c[4];
  }
  else
  {
   dst[2][0] = Interpolate4<R,G,B>( b.c[4], b.c[7], b.c[3] );
   dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  }

  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 27:

  if (Diff(b.w[3], b.w[1]))
  {
   dst[0][0] = b.c[4];
   dst[0][1] = b.c[4];
   dst[1][0] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate4<R,G,B>( b.c[4], b.c[3], b.c[1] );
   dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
   dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  }

  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 86:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );

  if (Diff(b.w[1], b.w[5]))
  {
   dst[0][1] = b.c[4];
   dst[0][2] = b.c[4];
   dst[1][2] = b.c[4];
  }
  else
  {
   dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
   dst[0][2] = Interpolate4<R,G,B>( b.c[4], b.c[1], b.c[5] );
   dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  }

  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 216:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );

  if (Diff(b.w[5], b.w[7]))
  {
   dst[1][2] = b.c[4];
   dst[2][1] = b.c[4];
   dst[2][2] = b.c[4];
  }
  else
  {
   dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
   dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
   dst[2][2] = Interpolate4<R,G,B>( b.c[4], b.c[5], b.c[7] );
  }
  break;

 case 106:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = b.c[4];
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );

  if (Diff(b.w[7], b.w[3]))
  {
   dst[1][0] = b.c[4];
   dst[2][0] = b.c[4];
   dst[2][1] = b.c[4];
  }
  else
  {
   dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
   dst[2][0] = Interpolate4<R,G,B>( b.c[4], b.c[7], b.c[3] );
   dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  }

  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 30:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );

  if (Diff(b.w[1], b.w[5]))
  {
   dst[0][1] = b.c[4];
   dst[0][2] = b.c[4];
   dst[1][2] = b.c[4];
  }
  else
  {
   dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
   dst[0][2] = Interpolate4<R,G,B>( b.c[4], b.c[1], b.c[5] );
   dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  }

  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 210:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = b.c[4];
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );

  if (Diff(b.w[5], b.w[7]))
  {
   dst[1][2] = b.c[4];
   dst[2][1] = b.c[4];
   dst[2][2] = b.c[4];
  }
  else
  {
   dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
   dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
   dst[2][2] = Interpolate4<R,G,B>( b.c[4], b.c[5], b.c[7] );
  }
  break;

 case 120:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];

  if (Diff(b.w[7], b.w[3]))
  {
   dst[1][0] = b.c[4];
   dst[2][0] = b.c[4];
   dst[2][1] = b.c[4];
  }
  else
  {
   dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
   dst[2][0] = Interpolate4<R,G,B>( b.c[4], b.c[7], b.c[3] );
   dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  }

  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 75:

  if (Diff(b.w[3], b.w[1]))
  {
   dst[0][0] = b.c[4];
   dst[0][1] = b.c[4];
   dst[1][0] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate4<R,G,B>( b.c[4], b.c[3], b.c[1] );
   dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
   dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  }

  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 29:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 198:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = b.c[4];
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  break;

 case 184:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  break;

 case 99:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = b.c[4];
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 57:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 71:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = b.c[4];
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 156:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  break;

 case 226:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = b.c[4];
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  break;

 case 60:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 195:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = b.c[4];
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  break;

 case 102:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = b.c[4];
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 153:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  break;

 case 58:

  if (Diff(b.w[3], b.w[1]))
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  else
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );

  dst[0][1] = b.c[4];

  if (Diff(b.w[1], b.w[5]))
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  else
   dst[0][2] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 83:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = b.c[4];

  if (Diff(b.w[1], b.w[5]))
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  else
   dst[0][2] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = b.c[4];

  if (Diff(b.w[5], b.w[7]))
   dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  else
   dst[2][2] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 92:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];

  if (Diff(b.w[7], b.w[3]))
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  else
   dst[2][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );

  dst[2][1] = b.c[4];

  if (Diff(b.w[5], b.w[7]))
   dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  else
   dst[2][2] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 202:

  if (Diff(b.w[3], b.w[1]))
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  else
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );

  dst[0][1] = b.c[4];
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );

  if (Diff(b.w[7], b.w[3]))
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  else
   dst[2][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );

  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  break;

 case 78:

  if (Diff(b.w[3], b.w[1]))
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  else
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );

  dst[0][1] = b.c[4];
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );

  if (Diff(b.w[7], b.w[3]))
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  else
   dst[2][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );

  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 154:

  if (Diff(b.w[3], b.w[1]))
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  else
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );

  dst[0][1] = b.c[4];

  if (Diff(b.w[1], b.w[5]))
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  else
   dst[0][2] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  break;

 case 114:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = b.c[4];

  if (Diff(b.w[1], b.w[5]))
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  else
   dst[0][2] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[2][1] = b.c[4];

  if (Diff(b.w[5], b.w[7]))
   dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  else
   dst[2][2] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 89:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];

  if (Diff(b.w[7], b.w[3]))
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  else
   dst[2][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );

  dst[2][1] = b.c[4];

  if (Diff(b.w[5], b.w[7]))
   dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  else
   dst[2][2] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 90:

  if (Diff(b.w[3], b.w[1]))
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  else
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );

  dst[0][1] = b.c[4];

  if (Diff(b.w[1], b.w[5]))
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  else
   dst[0][2] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];

  if (Diff(b.w[7], b.w[3]))
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  else
   dst[2][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );

  dst[2][1] = b.c[4];

  if (Diff(b.w[5], b.w[7]))
   dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  else
   dst[2][2] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 55:
 case 23:

  if (Diff(b.w[1], b.w[5]))
  {
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
   dst[0][1] = b.c[4];
   dst[0][2] = b.c[4];
   dst[1][2] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );
   dst[0][1] = Interpolate1<R,G,B>( b.c[1], b.c[4] );
   dst[0][2] = Interpolate5<R,G,B>( b.c[1], b.c[5] );
   dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  }

  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[2][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 182:
 case 150:

  if (Diff(b.w[1], b.w[5]))
  {
   dst[0][1] = b.c[4];
   dst[0][2] = b.c[4];
   dst[1][2] = b.c[4];
   dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  }
  else
  {
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[0][2] = Interpolate5<R,G,B>( b.c[1], b.c[5] );
   dst[1][2] = Interpolate1<R,G,B>( b.c[5], b.c[4] );
   dst[2][2] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );
  }

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[2][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  break;

 case 213:
 case 212:

  if (Diff(b.w[5], b.w[7]))
  {
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[1][2] = b.c[4];
   dst[2][1] = b.c[4];
   dst[2][2] = b.c[4];
  }
  else
  {
   dst[0][2] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
   dst[1][2] = Interpolate1<R,G,B>( b.c[5], b.c[4] );
   dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
   dst[2][2] = Interpolate5<R,G,B>( b.c[5], b.c[7] );
  }

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  break;

 case 241:
 case 240:

  if (Diff(b.w[5], b.w[7]))
  {
   dst[1][2] = b.c[4];
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
   dst[2][1] = b.c[4];
   dst[2][2] = b.c[4];
  }
  else
  {
   dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
   dst[2][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
   dst[2][1] = Interpolate1<R,G,B>( b.c[7], b.c[4] );
   dst[2][2] = Interpolate5<R,G,B>( b.c[5], b.c[7] );
  }

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  break;

 case 236:
 case 232:

  if (Diff(b.w[7], b.w[3]))
  {
   dst[1][0] = b.c[4];
   dst[2][0] = b.c[4];
   dst[2][1] = b.c[4];
   dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  }
  else
  {
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
   dst[2][0] = Interpolate5<R,G,B>( b.c[7], b.c[3]);
   dst[2][1] = Interpolate1<R,G,B>( b.c[7], b.c[4] );
   dst[2][2] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );
  }

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  break;

 case 109:
 case 105:

  if (Diff(b.w[7], b.w[3]))
  {
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[1][0] = b.c[4];
   dst[2][0] = b.c[4];
   dst[2][1] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );
   dst[1][0] = Interpolate1<R,G,B>( b.c[3], b.c[4] );
   dst[2][0] = Interpolate5<R,G,B>( b.c[7], b.c[3]);
   dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  }

  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 171:
 case 43:

  if (Diff(b.w[3], b.w[1]))
  {
   dst[0][0] = b.c[4];
   dst[0][1] = b.c[4];
   dst[1][0] = b.c[4];
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  }
  else
  {
   dst[0][0] = Interpolate5<R,G,B>( b.c[3], b.c[1] );
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[1][0] = Interpolate1<R,G,B>( b.c[3], b.c[4] );
   dst[2][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  }

  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );
  break;

 case 143:
 case 15:

  if (Diff(b.w[3], b.w[1]))
  {
   dst[0][0] = b.c[4];
   dst[0][1] = b.c[4];
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
   dst[1][0] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate5<R,G,B>( b.c[3], b.c[1] );
   dst[0][1] = Interpolate1<R,G,B>( b.c[1], b.c[4] );
   dst[0][2] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  }

  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );
  break;

 case 124:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];

  if (Diff(b.w[7], b.w[3]))
  {
   dst[1][0] = b.c[4];
   dst[2][0] = b.c[4];
   dst[2][1] = b.c[4];
  }
  else
  {
   dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
   dst[2][0] = Interpolate4<R,G,B>( b.c[4], b.c[7], b.c[3] );
   dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  }

  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 203:

  if (Diff(b.w[3], b.w[1]))
  {
   dst[0][0] = b.c[4];
   dst[0][1] = b.c[4];
   dst[1][0] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate4<R,G,B>( b.c[4], b.c[3], b.c[1] );
   dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
   dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  }

  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  break;

 case 62:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );

  if (Diff(b.w[1], b.w[5]))
  {
   dst[0][1] = b.c[4];
   dst[0][2] = b.c[4];
   dst[1][2] = b.c[4];
  }
  else
  {
   dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
   dst[0][2] = Interpolate4<R,G,B>( b.c[4], b.c[1], b.c[5] );
   dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  }

  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 211:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = b.c[4];
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );

  if (Diff(b.w[5], b.w[7]))
  {
   dst[1][2] = b.c[4];
   dst[2][1] = b.c[4];
   dst[2][2] = b.c[4];
  }
  else
  {
   dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
   dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
   dst[2][2] = Interpolate4<R,G,B>( b.c[4], b.c[5], b.c[7] );
  }
  break;

 case 118:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );

  if (Diff(b.w[1], b.w[5]))
  {
   dst[0][1] = b.c[4];
   dst[0][2] = b.c[4];
   dst[1][2] = b.c[4];
  }
  else
  {
   dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
   dst[0][2] = Interpolate4<R,G,B>( b.c[4], b.c[1], b.c[5] );
   dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  }

  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 217:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );

  if (Diff(b.w[5], b.w[7]))
  {
   dst[1][2] = b.c[4];
   dst[2][1] = b.c[4];
   dst[2][2] = b.c[4];
  }
  else
  {
   dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
   dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
   dst[2][2] = Interpolate4<R,G,B>( b.c[4], b.c[5], b.c[7] );
  }
  break;

 case 110:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = b.c[4];
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );

  if (Diff(b.w[7], b.w[3]))
  {
   dst[1][0] = b.c[4];
   dst[2][0] = b.c[4];
   dst[2][1] = b.c[4];
  }
  else
  {
   dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
   dst[2][0] = Interpolate4<R,G,B>( b.c[4], b.c[7], b.c[3] );
   dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  }

  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 155:

  if (Diff(b.w[3], b.w[1]))
  {
   dst[0][0] = b.c[4];
   dst[0][1] = b.c[4];
   dst[1][0] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate4<R,G,B>( b.c[4], b.c[3], b.c[1] );
   dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
   dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  }

  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  break;

 case 188:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  break;

 case 185:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  break;

 case 61:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 157:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  break;

 case 103:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = b.c[4];
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 227:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = b.c[4];
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  break;

 case 230:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = b.c[4];
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  break;

 case 199:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = b.c[4];
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  break;

 case 220:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];

  if (Diff(b.w[7], b.w[3]))
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  else
   dst[2][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );

  if (Diff(b.w[5], b.w[7]))
  {
   dst[1][2] = b.c[4];
   dst[2][1] = b.c[4];
   dst[2][2] = b.c[4];
  }
  else
  {
   dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
   dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
   dst[2][2] = Interpolate4<R,G,B>( b.c[4], b.c[5], b.c[7] );
  }
  break;

 case 158:

  if (Diff(b.w[3], b.w[1]))
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  else
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );

  if (Diff(b.w[1], b.w[5]))
  {
   dst[0][1] = b.c[4];
   dst[0][2] = b.c[4];
   dst[1][2] = b.c[4];
  }
  else
  {
   dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
   dst[0][2] = Interpolate4<R,G,B>( b.c[4], b.c[1], b.c[5] );
   dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  }

  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  break;

 case 234:

  if (Diff(b.w[3], b.w[1]))
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  else
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );

  dst[0][1] = b.c[4];
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );

  if (Diff(b.w[7], b.w[3]))
  {
   dst[1][0] = b.c[4];
   dst[2][0] = b.c[4];
   dst[2][1] = b.c[4];
  }
  else
  {
   dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
   dst[2][0] = Interpolate4<R,G,B>( b.c[4], b.c[7], b.c[3] );
   dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  }

  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  break;

 case 242:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = b.c[4];

  if (Diff(b.w[1], b.w[5]))
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  else
   dst[0][2] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );

  if (Diff(b.w[5], b.w[7]))
  {
   dst[1][2] = b.c[4];
   dst[2][1] = b.c[4];
   dst[2][2] = b.c[4];
  }
  else
  {
   dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
   dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
   dst[2][2] = Interpolate4<R,G,B>( b.c[4], b.c[5], b.c[7] );
  }
  break;

 case 59:

  if (Diff(b.w[3], b.w[1]))
  {
   dst[0][0] = b.c[4];
   dst[0][1] = b.c[4];
   dst[1][0] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate4<R,G,B>( b.c[4], b.c[3], b.c[1] );
   dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
   dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  }

  if (Diff(b.w[1], b.w[5]))
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  else
   dst[0][2] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 121:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];

  if (Diff(b.w[7], b.w[3]))
  {
   dst[1][0] = b.c[4];
   dst[2][0] = b.c[4];
   dst[2][1] = b.c[4];
  }
  else
  {
   dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
   dst[2][0] = Interpolate4<R,G,B>( b.c[4], b.c[7], b.c[3] );
   dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  }

  if (Diff(b.w[5], b.w[7]))
   dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  else
   dst[2][2] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 87:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );

  if (Diff(b.w[1], b.w[5]))
  {
   dst[0][1] = b.c[4];
   dst[0][2] = b.c[4];
   dst[1][2] = b.c[4];
  }
  else
  {
   dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
   dst[0][2] = Interpolate4<R,G,B>( b.c[4], b.c[1], b.c[5] );
   dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  }

  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = b.c[4];

  if (Diff(b.w[5], b.w[7]))
   dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  else
   dst[2][2] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 79:

  if (Diff(b.w[3], b.w[1]))
  {
   dst[0][0] = b.c[4];
   dst[0][1] = b.c[4];
   dst[1][0] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate4<R,G,B>( b.c[4], b.c[3], b.c[1] );
   dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
   dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  }

  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );

  if (Diff(b.w[7], b.w[3]))
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  else
   dst[2][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );

  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 122:

  if (Diff(b.w[3], b.w[1]))
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  else
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );

  dst[0][1] = b.c[4];

  if (Diff(b.w[1], b.w[5]))
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  else
   dst[0][2] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];

  if (Diff(b.w[7], b.w[3]))
  {
   dst[1][0] = b.c[4];
   dst[2][0] = b.c[4];
   dst[2][1] = b.c[4];
  }
  else
  {
   dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
   dst[2][0] = Interpolate4<R,G,B>( b.c[4], b.c[7], b.c[3] );
   dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  }

  if (Diff(b.w[5], b.w[7]))
   dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  else
   dst[2][2] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 94:

  if (Diff(b.w[3], b.w[1]))
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  else
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );

  if (Diff(b.w[1], b.w[5]))
  {
   dst[0][1] = b.c[4];
   dst[0][2] = b.c[4];
   dst[1][2] = b.c[4];
  }
  else
  {
   dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
   dst[0][2] = Interpolate4<R,G,B>( b.c[4], b.c[1], b.c[5] );
   dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  }

  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];

  if (Diff(b.w[7], b.w[3]))
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  else
   dst[2][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );

  dst[2][1] = b.c[4];

  if (Diff(b.w[5], b.w[7]))
   dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  else
   dst[2][2] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 218:

  if (Diff(b.w[3], b.w[1]))
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  else
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );

  dst[0][1] = b.c[4];

  if (Diff(b.w[1], b.w[5]))
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  else
   dst[0][2] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];

  if (Diff(b.w[7], b.w[3]))
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  else
   dst[2][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );

  if (Diff(b.w[5], b.w[7]))
  {
   dst[1][2] = b.c[4];
   dst[2][1] = b.c[4];
   dst[2][2] = b.c[4];
  }
  else
  {
   dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
   dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
   dst[2][2] = Interpolate4<R,G,B>( b.c[4], b.c[5], b.c[7] );
  }
  break;

 case 91:

  if (Diff(b.w[3], b.w[1]))
  {
   dst[0][0] = b.c[4];
   dst[0][1] = b.c[4];
   dst[1][0] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate4<R,G,B>( b.c[4], b.c[3], b.c[1] );
   dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
   dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  }

  if (Diff(b.w[1], b.w[5]))
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  else
   dst[0][2] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];

  if (Diff(b.w[7], b.w[3]))
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  else
   dst[2][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );

  dst[2][1] = b.c[4];

  if (Diff(b.w[5], b.w[7]))
   dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  else
   dst[2][2] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 229:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  break;

 case 167:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = b.c[4];
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );
  break;

 case 173:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );
  break;

 case 181:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  break;

 case 186:

  if (Diff(b.w[3], b.w[1]))
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  else
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );

  dst[0][1] = b.c[4];

  if (Diff(b.w[1], b.w[5]))
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  else
   dst[0][2] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  break;

 case 115:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = b.c[4];

  if (Diff(b.w[1], b.w[5]))
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  else
   dst[0][2] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[2][1] = b.c[4];

  if (Diff(b.w[5], b.w[7]))
   dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  else
   dst[2][2] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 93:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];

  if (Diff(b.w[7], b.w[3]))
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  else
   dst[2][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );

  dst[2][1] = b.c[4];

  if (Diff(b.w[5], b.w[7]))
   dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  else
   dst[2][2] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 206:

  if (Diff(b.w[3], b.w[1]))
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  else
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );

  dst[0][1] = b.c[4];
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );

  if (Diff(b.w[7], b.w[3]))
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  else
   dst[2][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );

  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  break;

 case 205:
 case 201:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );

  if (Diff(b.w[7], b.w[3]))
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  else
   dst[2][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );

  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  break;

 case 174:
 case 46:

  if (Diff(b.w[3], b.w[1]))
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  else
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );

  dst[0][1] = b.c[4];
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );
  break;

 case 179:
 case 147:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = b.c[4];

  if (Diff(b.w[1], b.w[5]))
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  else
   dst[0][2] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  break;

 case 117:
 case 116:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[2][1] = b.c[4];

  if (Diff(b.w[5], b.w[7]))
   dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  else
   dst[2][2] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 189:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  break;

 case 231:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = b.c[4];
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  break;

 case 126:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );

  if (Diff(b.w[1], b.w[5]))
  {
   dst[0][1] = b.c[4];
   dst[0][2] = b.c[4];
   dst[1][2] = b.c[4];
  }
  else
  {
   dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
   dst[0][2] = Interpolate4<R,G,B>( b.c[4], b.c[1], b.c[5] );
   dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  }

  dst[1][1] = b.c[4];

  if (Diff(b.w[7], b.w[3]))
  {
   dst[1][0] = b.c[4];
   dst[2][0] = b.c[4];
   dst[2][1] = b.c[4];
  }
  else
  {
   dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
   dst[2][0] = Interpolate4<R,G,B>( b.c[4], b.c[7], b.c[3] );
   dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  }

  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 219:

  if (Diff(b.w[3], b.w[1]))
  {
   dst[0][0] = b.c[4];
   dst[0][1] = b.c[4];
   dst[1][0] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate4<R,G,B>( b.c[4], b.c[3], b.c[1] );
   dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
   dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  }

  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][1] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );

  if (Diff(b.w[5], b.w[7]))
  {
   dst[1][2] = b.c[4];
   dst[2][1] = b.c[4];
   dst[2][2] = b.c[4];
  }
  else
  {
   dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
   dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
   dst[2][2] = Interpolate4<R,G,B>( b.c[4], b.c[5], b.c[7] );
  }
  break;

 case 125:

  if (Diff(b.w[7], b.w[3]))
  {
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[1][0] = b.c[4];
   dst[2][0] = b.c[4];
   dst[2][1] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );
   dst[1][0] = Interpolate1<R,G,B>( b.c[3], b.c[4] );
   dst[2][0] = Interpolate5<R,G,B>( b.c[7], b.c[3]);
   dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  }

  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 221:

  if (Diff(b.w[5], b.w[7]))
  {
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[1][2] = b.c[4];
   dst[2][1] = b.c[4];
   dst[2][2] = b.c[4];
  }
  else
  {
   dst[0][2] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
   dst[1][2] = Interpolate1<R,G,B>( b.c[5], b.c[4] );
   dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
   dst[2][2] = Interpolate5<R,G,B>( b.c[5], b.c[7] );
  }

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  break;

 case 207:

  if (Diff(b.w[3], b.w[1]))
  {
   dst[0][0] = b.c[4];
   dst[0][1] = b.c[4];
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
   dst[1][0] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate5<R,G,B>( b.c[3], b.c[1] );
   dst[0][1] = Interpolate1<R,G,B>( b.c[1], b.c[4] );
   dst[0][2] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  }

  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  break;

 case 238:

  if (Diff(b.w[7], b.w[3]))
  {
   dst[1][0] = b.c[4];
   dst[2][0] = b.c[4];
   dst[2][1] = b.c[4];
   dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  }
  else
  {
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
   dst[2][0] = Interpolate5<R,G,B>( b.c[7], b.c[3]);
   dst[2][1] = Interpolate1<R,G,B>( b.c[7], b.c[4] );
   dst[2][2] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );
  }

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = b.c[4];
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  break;

 case 190:

  if (Diff(b.w[1], b.w[5]))
  {
   dst[0][1] = b.c[4];
   dst[0][2] = b.c[4];
   dst[1][2] = b.c[4];
   dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  }
  else
  {
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[0][2] = Interpolate5<R,G,B>( b.c[1], b.c[5] );
   dst[1][2] = Interpolate1<R,G,B>( b.c[5], b.c[4] );
   dst[2][2] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );
  }

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  break;

 case 187:

  if (Diff(b.w[3], b.w[1]))
  {
   dst[0][0] = b.c[4];
   dst[0][1] = b.c[4];
   dst[1][0] = b.c[4];
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  }
  else
  {
   dst[0][0] = Interpolate5<R,G,B>( b.c[3], b.c[1] );
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[1][0] = Interpolate1<R,G,B>( b.c[3], b.c[4] );
   dst[2][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  }

  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  break;

 case 243:

  if (Diff(b.w[5], b.w[7]))
  {
   dst[1][2] = b.c[4];
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
   dst[2][1] = b.c[4];
   dst[2][2] = b.c[4];
  }
  else
  {
   dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
   dst[2][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
   dst[2][1] = Interpolate1<R,G,B>( b.c[7], b.c[4] );
   dst[2][2] = Interpolate5<R,G,B>( b.c[5], b.c[7] );
  }

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = b.c[4];
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  break;

 case 119:

  if (Diff(b.w[1], b.w[5]))
  {
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
   dst[0][1] = b.c[4];
   dst[0][2] = b.c[4];
   dst[1][2] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );
   dst[0][1] = Interpolate1<R,G,B>( b.c[1], b.c[4] );
   dst[0][2] = Interpolate5<R,G,B>( b.c[1], b.c[5] );
   dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  }

  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 237:
 case 233:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );

  if (Diff(b.w[7], b.w[3]))
   dst[2][0] = b.c[4];
  else
   dst[2][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );

  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  break;

 case 175:
 case 47:

  if (Diff(b.w[3], b.w[1]))
   dst[0][0] = b.c[4];
  else
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );

  dst[0][1] = b.c[4];
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );
  break;

 case 183:
 case 151:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = b.c[4];

  if (Diff(b.w[1], b.w[5]))
   dst[0][2] = b.c[4];
  else
   dst[0][2] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  break;

 case 245:
 case 244:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[2][1] = b.c[4];

  if (Diff(b.w[5], b.w[7]))
   dst[2][2] = b.c[4];
  else
   dst[2][2] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 250:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = b.c[4];
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][1] = b.c[4];

  if (Diff(b.w[7], b.w[3]))
  {
   dst[1][0] = b.c[4];
   dst[2][0] = b.c[4];
  }
  else
  {
   dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
   dst[2][0] = Interpolate4<R,G,B>( b.c[4], b.c[7], b.c[3] );
  }

  dst[2][1] = b.c[4];

  if (Diff(b.w[5], b.w[7]))
  {
   dst[1][2] = b.c[4];
   dst[2][2] = b.c[4];
  }
  else
  {
   dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
   dst[2][2] = Interpolate4<R,G,B>( b.c[4], b.c[5], b.c[7] );
  }
  break;

 case 123:

  if (Diff(b.w[3], b.w[1]))
  {
   dst[0][0] = b.c[4];
   dst[0][1] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate4<R,G,B>( b.c[4], b.c[3], b.c[1] );
   dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  }

  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];

  if (Diff(b.w[7], b.w[3]))
  {
   dst[2][0] = b.c[4];
   dst[2][1] = b.c[4];
  }
  else
  {
   dst[2][0] = Interpolate4<R,G,B>( b.c[4], b.c[7], b.c[3] );
   dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  }

  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 95:

  if (Diff(b.w[3], b.w[1]))
  {
   dst[0][0] = b.c[4];
   dst[1][0] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate4<R,G,B>( b.c[4], b.c[3], b.c[1] );
   dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  }

  dst[0][1] = b.c[4];

  if (Diff(b.w[1], b.w[5]))
  {
   dst[0][2] = b.c[4];
   dst[1][2] = b.c[4];
  }
  else
  {
   dst[0][2] = Interpolate4<R,G,B>( b.c[4], b.c[1], b.c[5] );
   dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  }

  dst[1][1] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 222:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );

  if (Diff(b.w[1], b.w[5]))
  {
   dst[0][1] = b.c[4];
   dst[0][2] = b.c[4];
  }
  else
  {
   dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
   dst[0][2] = Interpolate4<R,G,B>( b.c[4], b.c[1], b.c[5] );
  }

  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );

  if (Diff(b.w[5], b.w[7]))
  {
   dst[2][1] = b.c[4];
   dst[2][2] = b.c[4];
  }
  else
  {
   dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
   dst[2][2] = Interpolate4<R,G,B>( b.c[4], b.c[5], b.c[7] );
  }
  break;

 case 252:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];

  if (Diff(b.w[7], b.w[3]))
  {
   dst[1][0] = b.c[4];
   dst[2][0] = b.c[4];
  }
  else
  {
   dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
   dst[2][0] = Interpolate4<R,G,B>( b.c[4], b.c[7], b.c[3] );
  }

  dst[2][1] = b.c[4];

  if (Diff(b.w[5], b.w[7]))
   dst[2][2] = b.c[4];
  else
   dst[2][2] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 249:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];

  if (Diff(b.w[7], b.w[3]))
   dst[2][0] = b.c[4];
  else
   dst[2][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );

  dst[2][1] = b.c[4];

  if (Diff(b.w[5], b.w[7]))
  {
   dst[1][2] = b.c[4];
   dst[2][2] = b.c[4];
  }
  else
  {
   dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
   dst[2][2] = Interpolate4<R,G,B>( b.c[4], b.c[5], b.c[7] );
  }
  break;

 case 235:

  if (Diff(b.w[3], b.w[1]))
  {
   dst[0][0] = b.c[4];
   dst[0][1] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate4<R,G,B>( b.c[4], b.c[3], b.c[1] );
   dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  }

  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );

  if (Diff(b.w[7], b.w[3]))
   dst[2][0] = b.c[4];
  else
   dst[2][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );

  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  break;

 case 111:

  if (Diff(b.w[3], b.w[1]))
   dst[0][0] = b.c[4];
  else
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );

  dst[0][1] = b.c[4];
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );

  if (Diff(b.w[7], b.w[3]))
  {
   dst[2][0] = b.c[4];
   dst[2][1] = b.c[4];
  }
  else
  {
   dst[2][0] = Interpolate4<R,G,B>( b.c[4], b.c[7], b.c[3] );
   dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  }

  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 63:

  if (Diff(b.w[3], b.w[1]))
   dst[0][0] = b.c[4];
  else
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );

  dst[0][1] = b.c[4];

  if (Diff(b.w[1], b.w[5]))
  {
   dst[0][2] = b.c[4];
   dst[1][2] = b.c[4];
  }
  else
  {
   dst[0][2] = Interpolate4<R,G,B>( b.c[4], b.c[1], b.c[5] );
   dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  }

  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 159:

  if (Diff(b.w[3], b.w[1]))
  {
   dst[0][0] = b.c[4];
   dst[1][0] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate4<R,G,B>( b.c[4], b.c[3], b.c[1] );
   dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  }

  dst[0][1] = b.c[4];

  if (Diff(b.w[1], b.w[5]))
   dst[0][2] = b.c[4];
  else
   dst[0][2] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  break;

 case 215:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = b.c[4];

  if (Diff(b.w[1], b.w[5]))
   dst[0][2] = b.c[4];
  else
   dst[0][2] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );

  if (Diff(b.w[5], b.w[7]))
  {
   dst[2][1] = b.c[4];
   dst[2][2] = b.c[4];
  }
  else
  {
   dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
   dst[2][2] = Interpolate4<R,G,B>( b.c[4], b.c[5], b.c[7] );
  }
  break;

 case 246:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );

  if (Diff(b.w[1], b.w[5]))
  {
   dst[0][1] = b.c[4];
   dst[0][2] = b.c[4];
  }
  else
  {
   dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
   dst[0][2] = Interpolate4<R,G,B>( b.c[4], b.c[1], b.c[5] );
  }

  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[2][1] = b.c[4];

  if (Diff(b.w[5], b.w[7]))
   dst[2][2] = b.c[4];
  else
   dst[2][2] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 254:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );

  if (Diff(b.w[1], b.w[5]))
  {
   dst[0][1] = b.c[4];
   dst[0][2] = b.c[4];
  }
  else
  {
   dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
   dst[0][2] = Interpolate4<R,G,B>( b.c[4], b.c[1], b.c[5] );
  }

  dst[1][1] = b.c[4];

  if (Diff(b.w[7], b.w[3]))
  {
   dst[1][0] = b.c[4];
   dst[2][0] = b.c[4];
  }
  else
  {
   dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
   dst[2][0] = Interpolate4<R,G,B>( b.c[4], b.c[7], b.c[3] );
  }

  if (Diff(b.w[5], b.w[7]))
  {
   dst[1][2] = b.c[4];
   dst[2][1] = b.c[4];
   dst[2][2] = b.c[4];
  }
  else
  {
   dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
   dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
   dst[2][2] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );
  }
  break;

 case 253:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];

  if (Diff(b.w[7], b.w[3]))
   dst[2][0] = b.c[4];
  else
   dst[2][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );

  dst[2][1] = b.c[4];

  if (Diff(b.w[5], b.w[7]))
   dst[2][2] = b.c[4];
  else
   dst[2][2] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 251:

  if (Diff(b.w[3], b.w[1]))
  {
   dst[0][0] = b.c[4];
   dst[0][1] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate4<R,G,B>( b.c[4], b.c[3], b.c[1] );
   dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  }

  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[1][1] = b.c[4];

  if (Diff(b.w[7], b.w[3]))
  {
   dst[1][0] = b.c[4];
   dst[2][0] = b.c[4];
   dst[2][1] = b.c[4];
  }
  else
  {
   dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
   dst[2][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
   dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  }

  if (Diff(b.w[5], b.w[7]))
  {
   dst[1][2] = b.c[4];
   dst[2][2] = b.c[4];
  }
  else
  {
   dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
   dst[2][2] = Interpolate4<R,G,B>( b.c[4], b.c[5], b.c[7] );
  }
  break;

 case 239:

  if (Diff(b.w[3], b.w[1]))
   dst[0][0] = b.c[4];
  else
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );

  dst[0][1] = b.c[4];
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );

  if (Diff(b.w[7], b.w[3]))
   dst[2][0] = b.c[4];
  else
   dst[2][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );

  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  break;

 case 127:

  if (Diff(b.w[3], b.w[1]))
  {
   dst[0][0] = b.c[4];
   dst[0][1] = b.c[4];
   dst[1][0] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );
   dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
   dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  }

  if (Diff(b.w[1], b.w[5]))
  {
   dst[0][2] = b.c[4];
   dst[1][2] = b.c[4];
  }
  else
  {
   dst[0][2] = Interpolate4<R,G,B>( b.c[4], b.c[1], b.c[5] );
   dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  }

  dst[1][1] = b.c[4];

  if (Diff(b.w[7], b.w[3]))
  {
   dst[2][0] = b.c[4];
   dst[2][1] = b.c[4];
  }
  else
  {
   dst[2][0] = Interpolate4<R,G,B>( b.c[4], b.c[7], b.c[3] );
   dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  }

  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  break;

 case 191:

  if (Diff(b.w[3], b.w[1]))
   dst[0][0] = b.c[4];
  else
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );

  dst[0][1] = b.c[4];

  if (Diff(b.w[1], b.w[5]))
   dst[0][2] = b.c[4];
  else
   dst[0][2] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  break;

 case 223:

  if (Diff(b.w[3], b.w[1]))
  {
   dst[0][0] = b.c[4];
   dst[1][0] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate4<R,G,B>( b.c[4], b.c[3], b.c[1] );
   dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  }

  if (Diff(b.w[1], b.w[5]))
  {
   dst[0][1] = b.c[4];
   dst[0][2] = b.c[4];
   dst[1][2] = b.c[4];
  }
  else
  {
   dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
   dst[0][2] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
   dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  }

  dst[1][1] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );

  if (Diff(b.w[5], b.w[7]))
  {
   dst[2][1] = b.c[4];
   dst[2][2] = b.c[4];
  }
  else
  {
   dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
   dst[2][2] = Interpolate4<R,G,B>( b.c[4], b.c[5], b.c[7] );
  }
  break;

 case 247:

  dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = b.c[4];

  if (Diff(b.w[1], b.w[5]))
   dst[0][2] = b.c[4];
  else
   dst[0][2] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  dst[2][1] = b.c[4];

  if (Diff(b.w[5], b.w[7]))
   dst[2][2] = b.c[4];
  else
   dst[2][2] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );

  break;

 case 255:

  if (Diff(b.w[3], b.w[1]))
   dst[0][0] = b.c[4];
  else
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[3], b.c[1] );

  dst[0][1] = b.c[4];

  if (Diff(b.w[1], b.w[5]))
   dst[0][2] = b.c[4];
  else
   dst[0][2] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];

  if (Diff(b.w[7], b.w[3]))
   dst[2][0] = b.c[4];
  else
   dst[2][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );

  dst[2][1] = b.c[4];

  if (Diff(b.w[5], b.w[7]))
   dst[2][2] = b.c[4];
  else
   dst[2][2] = Interpolate2<R,G,B>( b.c[4], b.c[5], b.c[7] );
  break;


}
# 263 "/Users/Jorrit/iOS/nestopia/core/NstVideoFilterHqX.cpp" 2

     }

     dst[0] = reinterpret_cast<T*>(reinterpret_cast<byte*>(dst[0]) + pitch);
     dst[1] = reinterpret_cast<T*>(reinterpret_cast<byte*>(dst[1]) + pitch);
     dst[2] = reinterpret_cast<T*>(reinterpret_cast<byte*>(dst[2]) + pitch);
    }
   }

   template<typename T,dword R,dword G,dword B>
   void Renderer::FilterHqX::Blit4x(const Input& input,const Output& output,uint) const
   {
    const byte* __restrict__ src = reinterpret_cast<const byte*>(input.pixels);
    const long pitch = (output.pitch * 3) + output.pitch - (WIDTH*4 * sizeof(T));

    T* __restrict__ dst[4] =
    {
     static_cast<T*>(output.pixels) - 4,
     reinterpret_cast<T*>(static_cast<byte*>(output.pixels) + output.pitch) - 4,
     reinterpret_cast<T*>(static_cast<byte*>(output.pixels) + output.pitch * 2) - 4,
     reinterpret_cast<T*>(static_cast<byte*>(output.pixels) + output.pitch * 3) - 4
    };

    for (uint y=HEIGHT; y; --y)
    {
     const uint lines[2] =
     {
      y < HEIGHT ? WIDTH * sizeof(Input::Pixel) : 0,
      y > 1 ? WIDTH * sizeof(Input::Pixel) : 0
     };

     Buffer<T> b;

     b.w[2] = (b.w[1] = input.palette[*reinterpret_cast<const Input::Pixel*>(src - lines[0])]);
     b.w[5] = (b.w[4] = input.palette[*reinterpret_cast<const Input::Pixel*>(src)]);
     b.w[8] = (b.w[7] = input.palette[*reinterpret_cast<const Input::Pixel*>(src + lines[1])]);

     for (uint x=WIDTH; x; )
     {
      src += sizeof(Input::Pixel);
      dst[0] += 4;
      dst[1] += 4;
      dst[2] += 4;
      dst[3] += 4;

      b.w[0] = b.w[1];
      b.w[1] = b.w[2];
      b.w[3] = b.w[4];
      b.w[4] = b.w[5];
      b.w[6] = b.w[7];
      b.w[7] = b.w[8];

      if (--x)
      {
       b.w[2] = input.palette[*reinterpret_cast<const Input::Pixel*>(src - lines[0])];
       b.w[5] = input.palette[*reinterpret_cast<const Input::Pixel*>(src)];
       b.w[8] = input.palette[*reinterpret_cast<const Input::Pixel*>(src + lines[1])];
      }

      b.Convert( lut );

      const uint yuv5 = lut.yuv[b.w[4]];


# 1 "/Users/Jorrit/iOS/nestopia/core/NstVideoFilterHq4x.inl" 1
# 26 "/Users/Jorrit/iOS/nestopia/core/NstVideoFilterHq4x.inl"
switch
(
 (b.w[4] != b.w[0] && ((yuv5 - lut.yuv[b.w[0]]) & Lut::YUV_MASK) ? 0x01U : 0x0U) |
 (b.w[4] != b.w[1] && ((yuv5 - lut.yuv[b.w[1]]) & Lut::YUV_MASK) ? 0x02U : 0x0U) |
 (b.w[4] != b.w[2] && ((yuv5 - lut.yuv[b.w[2]]) & Lut::YUV_MASK) ? 0x04U : 0x0U) |
 (b.w[4] != b.w[3] && ((yuv5 - lut.yuv[b.w[3]]) & Lut::YUV_MASK) ? 0x08U : 0x0U) |
 (b.w[4] != b.w[5] && ((yuv5 - lut.yuv[b.w[5]]) & Lut::YUV_MASK) ? 0x10U : 0x0U) |
 (b.w[4] != b.w[6] && ((yuv5 - lut.yuv[b.w[6]]) & Lut::YUV_MASK) ? 0x20U : 0x0U) |
 (b.w[4] != b.w[7] && ((yuv5 - lut.yuv[b.w[7]]) & Lut::YUV_MASK) ? 0x40U : 0x0U) |
 (b.w[4] != b.w[8] && ((yuv5 - lut.yuv[b.w[8]]) & Lut::YUV_MASK) ? 0x80U : 0x0U)
)
# 177 "/Users/Jorrit/iOS/nestopia/core/NstVideoFilterHq4x.inl"
{
 case 0:
 case 1:
 case 4:
 case 32:
 case 128:
 case 5:
 case 132:
 case 160:
 case 33:
 case 129:
 case 36:
 case 133:
 case 164:
 case 161:
 case 37:
 case 165:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[3] );
  dst[0][1] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[3] );
  dst[0][2] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[0][3] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[1][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[1][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[1] );
  dst[1][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[1] );
  dst[2][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[7] );
  dst[2][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[7] );
  dst[2][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[7]);
  dst[2][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[7] );
  dst[3][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[3][1] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[3][2] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[5] );
  dst[3][3] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[5] );
  break;

 case 2:
 case 34:
 case 130:
 case 162:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[2] );
  dst[2][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[7] );
  dst[2][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[7] );
  dst[2][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[7]);
  dst[2][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[7] );
  dst[3][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[3][1] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[3][2] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[5] );
  dst[3][3] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[5] );
  break;

 case 16:
 case 17:
 case 48:
 case 49:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[3] );
  dst[0][1] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[3] );
  dst[0][2] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[1][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[2][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[7] );
  dst[2][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[7] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
  dst[3][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[3][1] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[3][2] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 64:
 case 65:
 case 68:
 case 69:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[3] );
  dst[0][1] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[3] );
  dst[0][2] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[0][3] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[1][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[1][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[1] );
  dst[1][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[1] );
  dst[2][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[8] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 8:
 case 12:
 case 136:
 case 140:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[0] );
  dst[0][2] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[0][3] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[1][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[1] );
  dst[1][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[1] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[2][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[7]);
  dst[2][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[7] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[6] );
  dst[3][2] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[5] );
  dst[3][3] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[5] );
  break;

 case 3:
 case 35:
 case 131:
 case 163:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[2] );
  dst[2][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[7] );
  dst[2][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[7] );
  dst[2][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[7]);
  dst[2][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[7] );
  dst[3][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[3][1] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[3][2] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[5] );
  dst[3][3] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[5] );
  break;

 case 6:
 case 38:
 case 134:
 case 166:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[1][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[1][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[7] );
  dst[2][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[7] );
  dst[2][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[7]);
  dst[2][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[7] );
  dst[3][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[3][1] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[3][2] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[5] );
  dst[3][3] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[5] );
  break;

 case 20:
 case 21:
 case 52:
 case 53:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[3] );
  dst[0][1] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[3] );
  dst[0][2] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[1][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][3] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[2][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[7] );
  dst[2][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[7] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
  dst[3][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[3][1] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[3][2] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 144:
 case 145:
 case 176:
 case 177:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[3] );
  dst[0][1] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[3] );
  dst[0][2] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[1][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[2][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[7] );
  dst[2][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[7] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][3] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[3][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[3][1] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[3][2] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  break;

 case 192:
 case 193:
 case 196:
 case 197:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[3] );
  dst[0][1] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[3] );
  dst[0][2] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[0][3] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[1][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[1][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[1] );
  dst[1][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[1] );
  dst[2][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[2][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[3][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  break;

 case 96:
 case 97:
 case 100:
 case 101:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[3] );
  dst[0][1] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[3] );
  dst[0][2] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[0][3] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[1][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[1][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[1] );
  dst[1][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[1] );
  dst[2][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[8] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[3][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 40:
 case 44:
 case 168:
 case 172:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[0] );
  dst[0][2] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[0][3] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[1][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[1] );
  dst[1][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[1] );
  dst[2][0] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[7]);
  dst[2][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[7] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][1] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][2] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[5] );
  dst[3][3] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[5] );
  break;

 case 9:
 case 13:
 case 137:
 case 141:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[0][3] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[1] );
  dst[1][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[1] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[2][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[7]);
  dst[2][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[7] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[6] );
  dst[3][2] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[5] );
  dst[3][3] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[5] );
  break;

 case 18:
 case 50:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );

  if (Diff( b.w[1], b.w[5] ))
  {
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
   dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
   dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
   dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  }
  else
  {
   dst[0][2] = Interpolate5<R,G,B>( b.c[1], b.c[4] );
   dst[0][3] = Interpolate5<R,G,B>( b.c[1], b.c[5] );
   dst[1][2] = b.c[4];
   dst[1][3] = Interpolate5<R,G,B>( b.c[5], b.c[4] );
  }

  dst[1][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[2][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[7] );
  dst[2][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[7] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
  dst[3][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[3][1] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[3][2] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 80:
 case 81:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[3] );
  dst[0][1] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[3] );
  dst[0][2] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[1][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[2][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );

  if (Diff( b.w[5], b.w[7] ))
  {
   dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
   dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
   dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
   dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  }
  else
  {
   dst[2][2] = b.c[4];
   dst[2][3] = Interpolate5<R,G,B>( b.c[5], b.c[4] );
   dst[3][2] = Interpolate5<R,G,B>( b.c[7], b.c[4] );
   dst[3][3] = Interpolate5<R,G,B>( b.c[7], b.c[5] );
  }

  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  break;

 case 72:
 case 76:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[0] );
  dst[0][2] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[0][3] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[1][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[1] );
  dst[1][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[1] );

  if (Diff( b.w[7], b.w[3] ))
  {
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
   dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
   dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
   dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  }
  else
  {
   dst[2][0] = Interpolate5<R,G,B>( b.c[3], b.c[4] );
   dst[2][1] = b.c[4];
   dst[3][0] = Interpolate5<R,G,B>( b.c[7], b.c[3] );
   dst[3][1] = Interpolate5<R,G,B>( b.c[7], b.c[4] );
  }

  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[8] );
  dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 10:
 case 138:

  if (Diff( b.w[3], b.w[1] ))
  {
   dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
   dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  }
  else
  {
   dst[0][0] = Interpolate5<R,G,B>( b.c[1], b.c[3] );
   dst[0][1] = Interpolate5<R,G,B>( b.c[1], b.c[4] );
   dst[1][0] = Interpolate5<R,G,B>( b.c[3], b.c[4] );
   dst[1][1] = b.c[4];
  }

  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[2] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[2][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[7]);
  dst[2][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[7] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[6] );
  dst[3][2] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[5] );
  dst[3][3] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[5] );
  break;

 case 66:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[2] );
  dst[2][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[8] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 24:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[0] );
  dst[0][2] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[6] );
  dst[3][2] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 7:
 case 39:
 case 135:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[0][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[1][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[1][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[7] );
  dst[2][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[7] );
  dst[2][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[7]);
  dst[2][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[7] );
  dst[3][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[3][1] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[3][2] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[5] );
  dst[3][3] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[5] );
  break;

 case 148:
 case 149:
 case 180:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[3] );
  dst[0][1] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[3] );
  dst[0][2] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[1][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][3] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[2][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[7] );
  dst[2][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[7] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][3] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[3][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[3][1] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[3][2] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  break;

 case 224:
 case 228:
 case 225:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[3] );
  dst[0][1] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[3] );
  dst[0][2] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[0][3] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[1][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[1][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[1] );
  dst[1][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[1] );
  dst[2][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[2][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[3][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[3][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  break;

 case 41:
 case 169:
 case 45:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[0][3] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[1] );
  dst[1][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[1] );
  dst[2][0] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[7]);
  dst[2][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[7] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][1] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][2] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[5] );
  dst[3][3] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[5] );
  break;

 case 22:
 case 54:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );

  if (Diff( b.w[1], b.w[5] ))
  {
   dst[0][2] = b.c[4];
   dst[0][3] = b.c[4];
   dst[1][3] = b.c[4];
  }
  else
  {
   dst[0][2] = Interpolate5<R,G,B>( b.c[1], b.c[4] );
   dst[0][3] = Interpolate5<R,G,B>( b.c[1], b.c[5] );
   dst[1][3] = Interpolate5<R,G,B>( b.c[5], b.c[4] );
  }

  dst[1][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[7] );
  dst[2][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[7] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
  dst[3][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[3][1] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[3][2] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 208:
 case 209:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[3] );
  dst[0][1] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[3] );
  dst[0][2] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[1][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[2][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[2][2] = b.c[4];

  if (Diff( b.w[5], b.w[7] ))
  {
   dst[2][3] = b.c[4];
   dst[3][2] = b.c[4];
   dst[3][3] = b.c[4];
  }
  else
  {
   dst[2][3] = Interpolate5<R,G,B>( b.c[5], b.c[4] );
   dst[3][2] = Interpolate5<R,G,B>( b.c[7], b.c[4] );
   dst[3][3] = Interpolate5<R,G,B>( b.c[7], b.c[5] );
  }

  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  break;

 case 104:
 case 108:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[0] );
  dst[0][2] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[0][3] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[1][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[1] );
  dst[1][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[1] );

  if (Diff( b.w[7], b.w[3] ))
  {
   dst[2][0] = b.c[4];
   dst[3][0] = b.c[4];
   dst[3][1] = b.c[4];
  }
  else
  {
   dst[2][0] = Interpolate5<R,G,B>( b.c[3], b.c[4] );
   dst[3][0] = Interpolate5<R,G,B>( b.c[7], b.c[3] );
   dst[3][1] = Interpolate5<R,G,B>( b.c[7], b.c[4] );
  }

  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[8] );
  dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 11:
 case 139:

  if (Diff( b.w[3], b.w[1] ))
  {
   dst[0][0] = b.c[4];
   dst[0][1] = b.c[4];
   dst[1][0] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate5<R,G,B>( b.c[1], b.c[3] );
   dst[0][1] = Interpolate5<R,G,B>( b.c[1], b.c[4] );
   dst[1][0] = Interpolate5<R,G,B>( b.c[3], b.c[4] );
  }

  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[2] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[2][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[7]);
  dst[2][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[7] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[6] );
  dst[3][2] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[5] );
  dst[3][3] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[5] );
  break;

 case 19:
 case 51:

  if (Diff( b.w[1], b.w[5] ))
  {
   dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
   dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
   dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
   dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
   dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  }
  else
  {
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[0][1] = Interpolate1<R,G,B>( b.c[1], b.c[4] );
   dst[0][2] = Interpolate8<R,G,B>( b.c[1], b.c[5] );
   dst[0][3] = Interpolate5<R,G,B>( b.c[1], b.c[5] );
   dst[1][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[1] );
   dst[1][3] = Interpolate2<R,G,B>( b.c[5], b.c[4], b.c[1] );
  }

  dst[1][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[2][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[7] );
  dst[2][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[7] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
  dst[3][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[3][1] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[3][2] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 146:
 case 178:
 {
  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );

  if (Diff( b.w[1], b.w[5] ))
  {
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
   dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
   dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
   dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
   dst[2][3] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
   dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  }
  else
  {
   dst[0][2] = Interpolate2<R,G,B>( b.c[1], b.c[4], b.c[5] );
   dst[0][3] = Interpolate5<R,G,B>( b.c[1], b.c[5] );
   dst[1][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[1] );
   dst[1][3] = Interpolate8<R,G,B>( b.c[5], b.c[1] );
   dst[2][3] = Interpolate1<R,G,B>( b.c[5], b.c[4]);
   dst[3][3] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  }

  dst[1][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[2][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[7] );
  dst[2][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[7] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[3][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[3][1] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[3][2] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  break;
 }
 case 84:
 case 85:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[3] );
  dst[0][1] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[3] );
  dst[0][2] = Interpolate8<R,G,B>( b.c[4], b.c[1] );

  if (Diff( b.w[5], b.w[7] ))
  {
   dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
   dst[1][3] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
   dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
   dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
   dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
   dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  }
  else
  {
   dst[0][3] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
   dst[1][3] = Interpolate1<R,G,B>( b.c[5], b.c[4] );
   dst[2][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[7]);
   dst[2][3] = Interpolate8<R,G,B>( b.c[5], b.c[7] );
   dst[3][2] = Interpolate2<R,G,B>( b.c[7], b.c[4], b.c[5] );
   dst[3][3] = Interpolate5<R,G,B>( b.c[7], b.c[5] );
  }

  dst[1][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[1][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[2][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  break;

 case 112:
 case 113:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[3] );
  dst[0][1] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[3] );
  dst[0][2] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[1][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[2][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );

  if (Diff( b.w[5], b.w[7] ))
  {
   dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
   dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
   dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
   dst[3][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
   dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
   dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  }
  else
  {
   dst[2][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[7]);
   dst[2][3] = Interpolate2<R,G,B>( b.c[5], b.c[4], b.c[7] );
   dst[3][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
   dst[3][1] = Interpolate1<R,G,B>( b.c[7], b.c[4] );
   dst[3][2] = Interpolate8<R,G,B>( b.c[7], b.c[5] );
   dst[3][3] = Interpolate5<R,G,B>( b.c[7], b.c[5] );
  }
  break;

 case 200:
 case 204:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[0] );
  dst[0][2] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[0][3] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[1][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[1] );
  dst[1][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[1] );

  if (Diff( b.w[7], b.w[3] ))
  {
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
   dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
   dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
   dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
   dst[3][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
   dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  }
  else
  {
   dst[2][0] = Interpolate2<R,G,B>( b.c[3], b.c[4], b.c[7] );
   dst[2][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[7] );
   dst[3][0] = Interpolate5<R,G,B>( b.c[7], b.c[3] );
   dst[3][1] = Interpolate8<R,G,B>( b.c[7], b.c[3] );
   dst[3][2] = Interpolate1<R,G,B>( b.c[7], b.c[4] );
   dst[3][3] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  }

  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[2][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  break;

 case 73:
 case 77:

  if (Diff( b.w[7], b.w[3] ))
  {
   dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
   dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
   dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
   dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
   dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  }
  else
  {
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
   dst[1][0] = Interpolate1<R,G,B>( b.c[3], b.c[4] );
   dst[2][0] = Interpolate8<R,G,B>( b.c[3], b.c[7] );
   dst[2][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[7] );
   dst[3][0] = Interpolate5<R,G,B>( b.c[7], b.c[3] );
   dst[3][1] = Interpolate2<R,G,B>( b.c[7], b.c[4], b.c[3] );
  }

  dst[0][1] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[0][3] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[1] );
  dst[1][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[1] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[8] );
  dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 42:
 case 170:

  if (Diff( b.w[3], b.w[1] ))
  {
   dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
   dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
   dst[2][0] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
   dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  }
  else
  {
   dst[0][0] = Interpolate5<R,G,B>( b.c[1], b.c[3] );
   dst[0][1] = Interpolate2<R,G,B>( b.c[1], b.c[4], b.c[3] );
   dst[1][0] = Interpolate8<R,G,B>( b.c[3], b.c[1] );
   dst[1][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[1] );
   dst[2][0] = Interpolate1<R,G,B>( b.c[3], b.c[4] );
   dst[3][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  }

  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[2] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[7]);
  dst[2][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[7] );
  dst[3][1] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][2] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[5] );
  dst[3][3] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[5] );
  break;

 case 14:
 case 142:

  if (Diff( b.w[3], b.w[1] ))
  {
   dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
   dst[0][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
   dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
   dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  }
  else
  {
   dst[0][0] = Interpolate5<R,G,B>( b.c[1], b.c[3] );
   dst[0][1] = Interpolate8<R,G,B>( b.c[1], b.c[3] );
   dst[0][2] = Interpolate1<R,G,B>( b.c[1], b.c[4] );
   dst[0][3] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[1][0] = Interpolate2<R,G,B>( b.c[3], b.c[4], b.c[1]);
   dst[1][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[1] );
  }

  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[1][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[2][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[7]);
  dst[2][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[7] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[6] );
  dst[3][2] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[5] );
  dst[3][3] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[5] );
  break;

 case 67:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[2] );
  dst[2][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[8] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 70:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[1][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[1][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[8] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 28:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[0] );
  dst[0][2] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][3] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[6] );
  dst[3][2] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 152:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[0] );
  dst[0][2] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][3] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[6] );
  dst[3][2] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  break;

 case 194:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[2] );
  dst[2][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[2][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[3][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  break;

 case 98:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[2] );
  dst[2][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[8] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[3][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 56:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[0] );
  dst[0][2] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[2][0] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][1] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][2] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 25:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[6] );
  dst[3][2] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 26:
 case 31:

  if (Diff( b.w[3], b.w[1] ))
  {
   dst[0][0] = b.c[4];
   dst[0][1] = b.c[4];
   dst[1][0] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate5<R,G,B>( b.c[1], b.c[3] );
   dst[0][1] = Interpolate5<R,G,B>( b.c[1], b.c[4] );
   dst[1][0] = Interpolate5<R,G,B>( b.c[3], b.c[4] );
  }

  if (Diff( b.w[1], b.w[5] ))
  {
   dst[0][2] = b.c[4];
   dst[0][3] = b.c[4];
   dst[1][3] = b.c[4];
  }
  else
  {
   dst[0][2] = Interpolate5<R,G,B>( b.c[1], b.c[4] );
   dst[0][3] = Interpolate5<R,G,B>( b.c[1], b.c[5] );
   dst[1][3] = Interpolate5<R,G,B>( b.c[5], b.c[4] );
  }

  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[6] );
  dst[3][2] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 82:
 case 214:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );

  if (Diff( b.w[1], b.w[5] ))
  {
   dst[0][2] = b.c[4];
   dst[0][3] = b.c[4];
   dst[1][3] = b.c[4];
  }
  else
  {
   dst[0][2] = Interpolate5<R,G,B>( b.c[1], b.c[4] );
   dst[0][3] = Interpolate5<R,G,B>( b.c[1], b.c[5] );
   dst[1][3] = Interpolate5<R,G,B>( b.c[5], b.c[4] );
  }

  dst[1][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[2][2] = b.c[4];

  if (Diff( b.w[5], b.w[7] ))
  {
   dst[2][3] = b.c[4];
   dst[3][2] = b.c[4];
   dst[3][3] = b.c[4];
  }
  else
  {
   dst[2][3] = Interpolate5<R,G,B>( b.c[5], b.c[4] );
   dst[3][2] = Interpolate5<R,G,B>( b.c[7], b.c[4] );
   dst[3][3] = Interpolate5<R,G,B>( b.c[7], b.c[5] );
  }

  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  break;

 case 88:
 case 248:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[0] );
  dst[0][2] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );

  if (Diff( b.w[7], b.w[3] ))
  {
   dst[2][0] = b.c[4];
   dst[3][0] = b.c[4];
   dst[3][1] = b.c[4];
  }
  else
  {
   dst[2][0] = Interpolate5<R,G,B>( b.c[3], b.c[4] );
   dst[3][0] = Interpolate5<R,G,B>( b.c[7], b.c[3] );
   dst[3][1] = Interpolate5<R,G,B>( b.c[7], b.c[4] );
  }

  dst[2][1] = b.c[4];
  dst[2][2] = b.c[4];

  if (Diff( b.w[5], b.w[7] ))
  {
   dst[2][3] = b.c[4];
   dst[3][2] = b.c[4];
   dst[3][3] = b.c[4];
  }
  else
  {
   dst[2][3] = Interpolate5<R,G,B>( b.c[5], b.c[4] );
   dst[3][2] = Interpolate5<R,G,B>( b.c[7], b.c[4] );
   dst[3][3] = Interpolate5<R,G,B>( b.c[7], b.c[5] );
  }
  break;

 case 74:
 case 107:

  if (Diff( b.w[3], b.w[1] ))
  {
   dst[0][0] = b.c[4];
   dst[0][1] = b.c[4];
   dst[1][0] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate5<R,G,B>( b.c[1], b.c[3] );
   dst[0][1] = Interpolate5<R,G,B>( b.c[1], b.c[4] );
   dst[1][0] = Interpolate5<R,G,B>( b.c[3], b.c[4] );
  }

  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[2] );

  if (Diff( b.w[7], b.w[3] ))
  {
   dst[2][0] = b.c[4];
   dst[3][0] = b.c[4];
   dst[3][1] = b.c[4];
  }
  else
  {
   dst[2][0] = Interpolate5<R,G,B>( b.c[3], b.c[4] );
   dst[3][0] = Interpolate5<R,G,B>( b.c[7], b.c[3] );
   dst[3][1] = Interpolate5<R,G,B>( b.c[7], b.c[4] );
  }

  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[8] );
  dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 27:

  if (Diff( b.w[3], b.w[1] ))
  {
   dst[0][0] = b.c[4];
   dst[0][1] = b.c[4];
   dst[1][0] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate5<R,G,B>( b.c[1], b.c[3] );
   dst[0][1] = Interpolate5<R,G,B>( b.c[1], b.c[4] );
   dst[1][0] = Interpolate5<R,G,B>( b.c[3], b.c[4] );
  }

  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[6] );
  dst[3][2] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 86:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );

  if (Diff( b.w[1], b.w[5] ))
  {
   dst[0][2] = b.c[4];
   dst[0][3] = b.c[4];
   dst[1][3] = b.c[4];
  }
  else
  {
   dst[0][2] = Interpolate5<R,G,B>( b.c[1], b.c[4] );
   dst[0][3] = Interpolate5<R,G,B>( b.c[1], b.c[5] );
   dst[1][3] = Interpolate5<R,G,B>( b.c[5], b.c[4] );
  }

  dst[1][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 216:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[0] );
  dst[0][2] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[2][2] = b.c[4];

  if (Diff( b.w[5], b.w[7] ))
  {
   dst[2][3] = b.c[4];
   dst[3][2] = b.c[4];
   dst[3][3] = b.c[4];
  }
  else
  {
   dst[2][3] = Interpolate5<R,G,B>( b.c[5], b.c[4] );
   dst[3][2] = Interpolate5<R,G,B>( b.c[7], b.c[4] );
   dst[3][3] = Interpolate5<R,G,B>( b.c[7], b.c[5] );
  }

  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  break;

 case 106:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[2] );

  if (Diff( b.w[7], b.w[3] ))
  {
   dst[2][0] = b.c[4];
   dst[3][0] = b.c[4];
   dst[3][1] = b.c[4];
  }
  else
  {
   dst[2][0] = Interpolate5<R,G,B>( b.c[3], b.c[4] );
   dst[3][0] = Interpolate5<R,G,B>( b.c[7], b.c[3] );
   dst[3][1] = Interpolate5<R,G,B>( b.c[7], b.c[4] );
  }

  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[8] );
  dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 30:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );

  if (Diff( b.w[1], b.w[5] ))
  {
   dst[0][2] = b.c[4];
   dst[0][3] = b.c[4];
   dst[1][3] = b.c[4];
  }
  else
  {
   dst[0][2] = Interpolate5<R,G,B>( b.c[1], b.c[4] );
   dst[0][3] = Interpolate5<R,G,B>( b.c[1], b.c[5] );
   dst[1][3] = Interpolate5<R,G,B>( b.c[5], b.c[4] );
  }

  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[6] );
  dst[3][2] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 210:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[2][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[2][2] = b.c[4];

  if (Diff( b.w[5], b.w[7] ))
  {
   dst[2][3] = b.c[4];
   dst[3][2] = b.c[4];
   dst[3][3] = b.c[4];
  }
  else
  {
   dst[2][3] = Interpolate5<R,G,B>( b.c[5], b.c[4] );
   dst[3][2] = Interpolate5<R,G,B>( b.c[7], b.c[4] );
   dst[3][3] = Interpolate5<R,G,B>( b.c[7], b.c[5] );
  }

  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  break;

 case 120:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[0] );
  dst[0][2] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );

  if (Diff( b.w[7], b.w[3] ))
  {
   dst[2][0] = b.c[4];
   dst[3][0] = b.c[4];
   dst[3][1] = b.c[4];
  }
  else
  {
   dst[2][0] = Interpolate5<R,G,B>( b.c[3], b.c[4] );
   dst[3][0] = Interpolate5<R,G,B>( b.c[7], b.c[3] );
   dst[3][1] = Interpolate5<R,G,B>( b.c[7], b.c[4] );
  }

  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
  dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 75:

  if (Diff( b.w[3], b.w[1] ))
  {
   dst[0][0] = b.c[4];
   dst[0][1] = b.c[4];
   dst[1][0] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate5<R,G,B>( b.c[1], b.c[3] );
   dst[0][1] = Interpolate5<R,G,B>( b.c[1], b.c[4] );
   dst[1][0] = Interpolate5<R,G,B>( b.c[3], b.c[4] );
  }

  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[2] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[8] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 29:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][3] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[6] );
  dst[3][2] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 198:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[1][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[1][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[2][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[3][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  break;

 case 184:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[0] );
  dst[0][2] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[2][0] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][3] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][1] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][2] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  break;

 case 99:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[2] );
  dst[2][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[8] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[3][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 57:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[2][0] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][1] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][2] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 71:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[0][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[1][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[1][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[8] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 156:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[0] );
  dst[0][2] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][3] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][3] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[6] );
  dst[3][2] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  break;

 case 226:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[2] );
  dst[2][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[2][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[3][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[3][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  break;

 case 60:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[0] );
  dst[0][2] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][3] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[2][0] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][1] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][2] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 195:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[2] );
  dst[2][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[2][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[3][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  break;

 case 102:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[1][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[1][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[8] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[3][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 153:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][3] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[6] );
  dst[3][2] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  break;

 case 58:

  if (Diff( b.w[3], b.w[1] ))
  {
   dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
   dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  }
  else
  {
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[3] );
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
   dst[1][1] = b.c[4];
  }

  if (Diff( b.w[1], b.w[5] ))
  {
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
   dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
   dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
   dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  }
  else
  {
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[0][3] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
   dst[1][2] = b.c[4];
   dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  }

  dst[2][0] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][1] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][2] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 83:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );

  if (Diff( b.w[1], b.w[5] ))
  {
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
   dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
   dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
   dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  }
  else
  {
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[0][3] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
   dst[1][2] = b.c[4];
   dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  }

  dst[1][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[2][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );

  if (Diff( b.w[5], b.w[7] ))
  {
   dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
   dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
   dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
   dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  }
  else
  {
   dst[2][2] = b.c[4];
   dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[5]);
   dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
   dst[3][3] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[5] );
  }

  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  break;

 case 92:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[0] );
  dst[0][2] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][3] = Interpolate3<R,G,B>( b.c[4], b.c[1] );

  if (Diff( b.w[7], b.w[3] ))
  {
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
   dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
   dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
   dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  }
  else
  {
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
   dst[2][1] = b.c[4];
   dst[3][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
   dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  }

  if (Diff( b.w[5], b.w[7] ))
  {
   dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
   dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
   dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
   dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  }
  else
  {
   dst[2][2] = b.c[4];
   dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[5]);
   dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
   dst[3][3] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[5] );
  }
  break;

 case 202:

  if (Diff( b.w[3], b.w[1] ))
  {
   dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
   dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  }
  else
  {
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[3] );
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
   dst[1][1] = b.c[4];
  }

  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[2] );

  if (Diff( b.w[7], b.w[3] ))
  {
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
   dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
   dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
   dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  }
  else
  {
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
   dst[2][1] = b.c[4];
   dst[3][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
   dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  }

  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[2][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[3][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  break;

 case 78:

  if (Diff( b.w[3], b.w[1] ))
  {
   dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
    dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
    dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
    dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  }
  else
  {
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[3] );
    dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
    dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
    dst[1][1] = b.c[4];
  }

  dst[0][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[1][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );

  if (Diff( b.w[7], b.w[3] ))
  {
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
   dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
   dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
   dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  }
  else
  {
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
   dst[2][1] = b.c[4];
   dst[3][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
   dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  }

  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[8] );
  dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 154:

  if (Diff( b.w[3], b.w[1] ))
  {
   dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
   dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  }
  else
  {
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[3] );
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
   dst[1][1] = b.c[4];
  }

  if (Diff( b.w[1], b.w[5] ))
  {
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
   dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
   dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
   dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  }
  else
  {
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[0][3] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
   dst[1][2] = b.c[4];
   dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  }

  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][3] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[6] );
  dst[3][2] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  break;

 case 114:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );

  if (Diff( b.w[1], b.w[5] ))
  {
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
   dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
   dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
   dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  }
  else
  {
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[0][3] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
   dst[1][2] = b.c[4];
   dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  }

  dst[1][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[2][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );

  if (Diff( b.w[5], b.w[7] ))
  {
   dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
   dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
   dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
   dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  }
  else
  {
   dst[2][2] = b.c[4];
   dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[5]);
   dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
   dst[3][3] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[5] );
  }

  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[3][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  break;

 case 89:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );

  if (Diff( b.w[7], b.w[3] ))
  {
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
   dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
   dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
   dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  }
  else
  {
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
   dst[2][1] = b.c[4];
   dst[3][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
   dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  }

  if (Diff( b.w[5], b.w[7] ))
  {
   dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
   dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
   dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
   dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  }
  else
  {
   dst[2][2] = b.c[4];
   dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[5]);
   dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
   dst[3][3] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[5] );
  }
  break;

 case 90:

  if (Diff( b.w[3], b.w[1] ))
  {
   dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
   dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  }
  else
  {
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[3] );
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
   dst[1][1] = b.c[4];
  }

  if (Diff( b.w[1], b.w[5] ))
  {
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
   dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
   dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
   dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  }
  else
  {
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[0][3] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
   dst[1][2] = b.c[4];
   dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  }

  if (Diff( b.w[7], b.w[3] ))
  {
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
   dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
   dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
   dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  }
  else
  {
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
   dst[2][1] = b.c[4];
   dst[3][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
   dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  }

  if (Diff( b.w[5], b.w[7] ))
  {
   dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
   dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
   dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
   dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  }
  else
  {
   dst[2][2] = b.c[4];
   dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[5]);
   dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
   dst[3][3] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[5] );
  }
  break;

 case 55:
 case 23:

  if (Diff( b.w[1], b.w[5] ))
  {
   dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
   dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
   dst[0][2] = b.c[4];
   dst[0][3] = b.c[4];
   dst[1][2] = b.c[4];
   dst[1][3] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[0][1] = Interpolate1<R,G,B>( b.c[1], b.c[4] );
   dst[0][2] = Interpolate8<R,G,B>( b.c[1], b.c[5] );
   dst[0][3] = Interpolate5<R,G,B>( b.c[1], b.c[5] );
   dst[1][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[1] );
   dst[1][3] = Interpolate2<R,G,B>( b.c[5], b.c[4], b.c[1] );
  }

  dst[1][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[2][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[7] );
  dst[2][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[7] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
  dst[3][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[3][1] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[3][2] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 182:
 case 150:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );

  if (Diff( b.w[1], b.w[5] ))
  {
   dst[0][2] = b.c[4];
   dst[0][3] = b.c[4];
   dst[1][2] = b.c[4];
   dst[1][3] = b.c[4];
   dst[2][3] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
   dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  }
  else
  {
   dst[0][2] = Interpolate2<R,G,B>( b.c[1], b.c[4], b.c[5] );
   dst[0][3] = Interpolate5<R,G,B>( b.c[1], b.c[5] );
   dst[1][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[1] );
   dst[1][3] = Interpolate8<R,G,B>( b.c[5], b.c[1] );
   dst[2][3] = Interpolate1<R,G,B>( b.c[5], b.c[4]);
   dst[3][3] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  }

  dst[1][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[2][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[7] );
  dst[2][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[7] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[3][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[3][1] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[3][2] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  break;

 case 213:
 case 212:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[3] );
  dst[0][1] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[3] );
  dst[0][2] = Interpolate8<R,G,B>( b.c[4], b.c[1] );

  if (Diff( b.w[5], b.w[7] ))
  {
   dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
   dst[1][3] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
   dst[2][2] = b.c[4];
   dst[2][3] = b.c[4];
   dst[3][2] = b.c[4];
   dst[3][3] = b.c[4];
  }
  else
  {
   dst[0][3] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
   dst[1][3] = Interpolate1<R,G,B>( b.c[5], b.c[4] );
   dst[2][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[7]);
   dst[2][3] = Interpolate8<R,G,B>( b.c[5], b.c[7] );
   dst[3][2] = Interpolate2<R,G,B>( b.c[7], b.c[4], b.c[5] );
   dst[3][3] = Interpolate5<R,G,B>( b.c[7], b.c[5] );
  }

  dst[1][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[1][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[2][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  break;

 case 241:
 case 240:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[3] );
  dst[0][1] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[3] );
  dst[0][2] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[1][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[2][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );

  if (Diff( b.w[5], b.w[7] ))
  {
   dst[2][2] = b.c[4];
   dst[2][3] = b.c[4];
   dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
   dst[3][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
   dst[3][2] = b.c[4];
   dst[3][3] = b.c[4];
  }
  else
  {
   dst[2][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[7]);
   dst[2][3] = Interpolate2<R,G,B>( b.c[5], b.c[4], b.c[7] );
   dst[3][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
   dst[3][1] = Interpolate1<R,G,B>( b.c[7], b.c[4] );
   dst[3][2] = Interpolate8<R,G,B>( b.c[7], b.c[5] );
   dst[3][3] = Interpolate5<R,G,B>( b.c[7], b.c[5] );
  }
  break;

 case 236:
 case 232:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[0] );
  dst[0][2] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[0][3] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[1][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[1] );
  dst[1][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[1] );

  if (Diff( b.w[7], b.w[3] ))
  {
   dst[2][0] = b.c[4];
   dst[2][1] = b.c[4];
   dst[3][0] = b.c[4];
   dst[3][1] = b.c[4];
   dst[3][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
   dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  }
  else
  {
   dst[2][0] = Interpolate2<R,G,B>( b.c[3], b.c[4], b.c[7] );
   dst[2][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[7] );
   dst[3][0] = Interpolate5<R,G,B>( b.c[7], b.c[3] );
   dst[3][1] = Interpolate8<R,G,B>( b.c[7], b.c[3] );
   dst[3][2] = Interpolate1<R,G,B>( b.c[7], b.c[4] );
   dst[3][3] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  }

  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[2][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  break;

 case 109:
 case 105:

  if (Diff( b.w[7], b.w[3] ))
  {
   dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
   dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
   dst[2][0] = b.c[4];
   dst[2][1] = b.c[4];
   dst[3][0] = b.c[4];
   dst[3][1] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
   dst[1][0] = Interpolate1<R,G,B>( b.c[3], b.c[4] );
   dst[2][0] = Interpolate8<R,G,B>( b.c[3], b.c[7] );
   dst[2][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[7] );
   dst[3][0] = Interpolate5<R,G,B>( b.c[7], b.c[3] );
   dst[3][1] = Interpolate2<R,G,B>( b.c[7], b.c[4], b.c[3] );
  }

  dst[0][1] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[0][3] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[1] );
  dst[1][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[1] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[8] );
  dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 171:
 case 43:

  if (Diff( b.w[3], b.w[1] ))
  {
   dst[0][0] = b.c[4];
   dst[0][1] = b.c[4];
   dst[1][0] = b.c[4];
   dst[1][1] = b.c[4];
   dst[2][0] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
   dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  }
  else
  {
   dst[0][0] = Interpolate5<R,G,B>( b.c[1], b.c[3] );
   dst[0][1] = Interpolate2<R,G,B>( b.c[1], b.c[4], b.c[3] );
   dst[1][0] = Interpolate8<R,G,B>( b.c[3], b.c[1] );
   dst[1][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[1] );
   dst[2][0] = Interpolate1<R,G,B>( b.c[3], b.c[4] );
   dst[3][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  }

  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[2] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[7]);
  dst[2][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[7] );
  dst[3][1] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][2] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[5] );
  dst[3][3] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[5] );
  break;

 case 143:
 case 15:

  if (Diff( b.w[3], b.w[1] ))
  {
   dst[0][0] = b.c[4];
   dst[0][1] = b.c[4];
   dst[0][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
   dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
   dst[1][0] = b.c[4];
   dst[1][1] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate5<R,G,B>( b.c[1], b.c[3] );
   dst[0][1] = Interpolate8<R,G,B>( b.c[1], b.c[3] );
   dst[0][2] = Interpolate1<R,G,B>( b.c[1], b.c[4] );
   dst[0][3] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[1][0] = Interpolate2<R,G,B>( b.c[3], b.c[4], b.c[1]);
   dst[1][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[1] );
  }

  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[1][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[2][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[7]);
  dst[2][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[7] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[6] );
  dst[3][2] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[5] );
  dst[3][3] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[5] );
  break;

 case 124:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[0] );
  dst[0][2] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][3] = Interpolate3<R,G,B>( b.c[4], b.c[1] );

  if (Diff( b.w[7], b.w[3] ))
  {
   dst[2][0] = b.c[4];
   dst[3][0] = b.c[4];
   dst[3][1] = b.c[4];
  }
  else
  {
   dst[2][0] = Interpolate5<R,G,B>( b.c[3], b.c[4] );
   dst[3][0] = Interpolate5<R,G,B>( b.c[7], b.c[3] );
   dst[3][1] = Interpolate5<R,G,B>( b.c[7], b.c[4] );
  }

  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
  dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 203:

  if (Diff( b.w[3], b.w[1] ))
  {
   dst[0][0] = b.c[4];
   dst[0][1] = b.c[4];
   dst[1][0] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate5<R,G,B>( b.c[1], b.c[3] );
   dst[0][1] = Interpolate5<R,G,B>( b.c[1], b.c[4] );
   dst[1][0] = Interpolate5<R,G,B>( b.c[3], b.c[4] );
  }

  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[2] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[2][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[3][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  break;

 case 62:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );

  if (Diff( b.w[1], b.w[5] ))
  {
   dst[0][2] = b.c[4];
   dst[0][3] = b.c[4];
   dst[1][3] = b.c[4];
  }
  else
  {
   dst[0][2] = Interpolate5<R,G,B>( b.c[1], b.c[4] );
   dst[0][3] = Interpolate5<R,G,B>( b.c[1], b.c[5] );
   dst[1][3] = Interpolate5<R,G,B>( b.c[5], b.c[4] );
  }

  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][1] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][2] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 211:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[2][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[2][2] = b.c[4];

  if (Diff( b.w[5], b.w[7] ))
  {
   dst[2][3] = b.c[4];
   dst[3][2] = b.c[4];
   dst[3][3] = b.c[4];
  }
  else
  {
   dst[2][3] = Interpolate5<R,G,B>( b.c[5], b.c[4] );
   dst[3][2] = Interpolate5<R,G,B>( b.c[7], b.c[4] );
   dst[3][3] = Interpolate5<R,G,B>( b.c[7], b.c[5] );
  }

  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  break;

 case 118:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );

  if (Diff( b.w[1], b.w[5] ))
  {
   dst[0][2] = b.c[4];
   dst[0][3] = b.c[4];
   dst[1][3] = b.c[4];
  }
  else
  {
   dst[0][2] = Interpolate5<R,G,B>( b.c[1], b.c[4] );
   dst[0][3] = Interpolate5<R,G,B>( b.c[1], b.c[5] );
   dst[1][3] = Interpolate5<R,G,B>( b.c[5], b.c[4] );
  }

  dst[1][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[3][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 217:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[2][2] = b.c[4];

  if (Diff( b.w[5], b.w[7] ))
  {
   dst[2][3] = b.c[4];
   dst[3][2] = b.c[4];
   dst[3][3] = b.c[4];
  }
  else
  {
   dst[2][3] = Interpolate5<R,G,B>( b.c[5], b.c[4] );
   dst[3][2] = Interpolate5<R,G,B>( b.c[7], b.c[4] );
   dst[3][3] = Interpolate5<R,G,B>( b.c[7], b.c[5] );
  }

  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  break;

 case 110:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[1][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );

  if (Diff( b.w[7], b.w[3] ))
  {
   dst[2][0] = b.c[4];
   dst[3][0] = b.c[4];
   dst[3][1] = b.c[4];
  }
  else
  {
   dst[2][0] = Interpolate5<R,G,B>( b.c[3], b.c[4] );
   dst[3][0] = Interpolate5<R,G,B>( b.c[7], b.c[3] );
   dst[3][1] = Interpolate5<R,G,B>( b.c[7], b.c[4] );
  }

  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[8] );
  dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 155:

  if (Diff( b.w[3], b.w[1] ))
  {
   dst[0][0] = b.c[4];
   dst[0][1] = b.c[4];
   dst[1][0] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate5<R,G,B>( b.c[1], b.c[3] );
   dst[0][1] = Interpolate5<R,G,B>( b.c[1], b.c[4] );
   dst[1][0] = Interpolate5<R,G,B>( b.c[3], b.c[4] );
  }

  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][3] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[6] );
  dst[3][2] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  break;

 case 188:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[0] );
  dst[0][2] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][3] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[2][0] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][3] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][1] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][2] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  break;

 case 185:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[2][0] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][3] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][1] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][2] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  break;

 case 61:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][3] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[2][0] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][1] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][2] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 157:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][3] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][3] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[6] );
  dst[3][2] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  break;

 case 103:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[0][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[1][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[1][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[8] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[3][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 227:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[2] );
  dst[2][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[2][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[3][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[3][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  break;

 case 230:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[1][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[1][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[2][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[3][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[3][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  break;

 case 199:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[0][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[1][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[1][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[2][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[3][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  break;

 case 220:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[0] );
  dst[0][2] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][3] = Interpolate3<R,G,B>( b.c[4], b.c[1] );

  if (Diff( b.w[7], b.w[3] ))
  {
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
   dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
   dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
   dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  }
  else
  {
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
   dst[2][1] = b.c[4];
   dst[3][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
   dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  }

  dst[2][2] = b.c[4];

  if (Diff( b.w[5], b.w[7] ))
  {
   dst[2][3] = b.c[4];
   dst[3][2] = b.c[4];
   dst[3][3] = b.c[4];
  }
  else
  {
   dst[2][3] = Interpolate5<R,G,B>( b.c[5], b.c[4] );
   dst[3][2] = Interpolate5<R,G,B>( b.c[7], b.c[4] );
   dst[3][3] = Interpolate5<R,G,B>( b.c[7], b.c[5] );
  }
  break;

 case 158:

  if (Diff( b.w[3], b.w[1] ))
  {
   dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
   dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  }
  else
  {
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[3] );
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
   dst[1][1] = b.c[4];
  }

  if (Diff( b.w[1], b.w[5] ))
  {
   dst[0][2] = b.c[4];
   dst[0][3] = b.c[4];
   dst[1][3] = b.c[4];
  }
  else
  {
   dst[0][2] = Interpolate5<R,G,B>( b.c[1], b.c[4] );
   dst[0][3] = Interpolate5<R,G,B>( b.c[1], b.c[5] );
   dst[1][3] = Interpolate5<R,G,B>( b.c[5], b.c[4] );
  }

  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][3] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[6] );
  dst[3][2] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  break;

 case 234:

  if (Diff( b.w[3], b.w[1] ))
  {
   dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
   dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  }
  else
  {
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[3] );
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
   dst[1][1] = b.c[4];
  }

  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[2] );

  if (Diff( b.w[7], b.w[3] ))
  {
   dst[2][0] = b.c[4];
   dst[3][0] = b.c[4];
   dst[3][1] = b.c[4];
  }
  else
  {
   dst[2][0] = Interpolate5<R,G,B>( b.c[3], b.c[4] );
   dst[3][0] = Interpolate5<R,G,B>( b.c[7], b.c[3] );
   dst[3][1] = Interpolate5<R,G,B>( b.c[7], b.c[4] );
  }

  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[2][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[3][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  break;

 case 242:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );

  if (Diff( b.w[1], b.w[5] ))
  {
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
   dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
   dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
   dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  }
  else
  {
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[0][3] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
   dst[1][2] = b.c[4];
   dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  }

  dst[1][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[2][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[2][2] = b.c[4];

  if (Diff( b.w[5], b.w[7] ))
  {
   dst[2][3] = b.c[4];
   dst[3][2] = b.c[4];
   dst[3][3] = b.c[4];
  }
  else
  {
   dst[2][3] = Interpolate5<R,G,B>( b.c[5], b.c[4] );
   dst[3][2] = Interpolate5<R,G,B>( b.c[7], b.c[4] );
   dst[3][3] = Interpolate5<R,G,B>( b.c[7], b.c[5] );
  }

  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[3][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  break;

 case 59:

  if (Diff( b.w[3], b.w[1] ))
  {
   dst[0][0] = b.c[4];
   dst[0][1] = b.c[4];
   dst[1][0] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate5<R,G,B>( b.c[1], b.c[3] );
   dst[0][1] = Interpolate5<R,G,B>( b.c[1], b.c[4] );
   dst[1][0] = Interpolate5<R,G,B>( b.c[3], b.c[4] );
  }

  if (Diff( b.w[1], b.w[5] ))
  {
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
   dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
   dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
   dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  }
  else
  {
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[0][3] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
   dst[1][2] = b.c[4];
   dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  }

  dst[1][1] = b.c[4];
  dst[2][0] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][1] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][2] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 121:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );

  if (Diff( b.w[7], b.w[3] ))
  {
   dst[2][0] = b.c[4];
   dst[3][0] = b.c[4];
   dst[3][1] = b.c[4];
  }
  else
  {
   dst[2][0] = Interpolate5<R,G,B>( b.c[3], b.c[4] );
   dst[3][0] = Interpolate5<R,G,B>( b.c[7], b.c[3] );
   dst[3][1] = Interpolate5<R,G,B>( b.c[7], b.c[4] );
  }

  dst[2][1] = b.c[4];

  if (Diff( b.w[5], b.w[7] ))
  {
   dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
   dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
   dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
   dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  }
  else
  {
   dst[2][2] = b.c[4];
   dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[5]);
   dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
   dst[3][3] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[5] );
  }
  break;

 case 87:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );

  if (Diff( b.w[1], b.w[5] ))
  {
   dst[0][2] = b.c[4];
   dst[0][3] = b.c[4];
   dst[1][3] = b.c[4];
  }
  else
  {
   dst[0][2] = Interpolate5<R,G,B>( b.c[1], b.c[4] );
   dst[0][3] = Interpolate5<R,G,B>( b.c[1], b.c[5] );
   dst[1][3] = Interpolate5<R,G,B>( b.c[5], b.c[4] );
  }

  dst[1][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );

  if (Diff( b.w[5], b.w[7] ))
  {
   dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
   dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
   dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
   dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  }
  else
  {
   dst[2][2] = b.c[4];
   dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[5]);
   dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
   dst[3][3] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[5] );
  }

  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  break;

 case 79:

  if (Diff( b.w[3], b.w[1] ))
  {
   dst[0][0] = b.c[4];
   dst[0][1] = b.c[4];
   dst[1][0] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate5<R,G,B>( b.c[1], b.c[3] );
   dst[0][1] = Interpolate5<R,G,B>( b.c[1], b.c[4] );
   dst[1][0] = Interpolate5<R,G,B>( b.c[3], b.c[4] );
  }

  dst[0][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[1][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );

  if (Diff( b.w[7], b.w[3] ))
  {
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
   dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
   dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
   dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  }
  else
  {
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
   dst[2][1] = b.c[4];
   dst[3][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
   dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  }

  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[8] );
  dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 122:

  if (Diff( b.w[3], b.w[1] ))
  {
   dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
   dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  }
  else
  {
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[3] );
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
   dst[1][1] = b.c[4];
  }

  if (Diff( b.w[1], b.w[5] ))
  {
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
   dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
   dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
   dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  }
  else
  {
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[0][3] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
   dst[1][2] = b.c[4];
   dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  }

  if (Diff( b.w[7], b.w[3] ))
  {
   dst[2][0] = b.c[4];
   dst[3][0] = b.c[4];
   dst[3][1] = b.c[4];
  }
  else
  {
   dst[2][0] = Interpolate5<R,G,B>( b.c[3], b.c[4] );
   dst[3][0] = Interpolate5<R,G,B>( b.c[7], b.c[3] );
   dst[3][1] = Interpolate5<R,G,B>( b.c[7], b.c[4] );
  }

  dst[2][1] = b.c[4];

  if (Diff( b.w[5], b.w[7] ))
  {
   dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
   dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
   dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
   dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  }
  else
  {
   dst[2][2] = b.c[4];
   dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[5]);
   dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
   dst[3][3] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[5] );
  }
  break;

 case 94:

  if (Diff( b.w[3], b.w[1] ))
  {
   dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
   dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  }
  else
  {
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[3] );
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
   dst[1][1] = b.c[4];
  }

  if (Diff( b.w[1], b.w[5] ))
  {
   dst[0][2] = b.c[4];
   dst[0][3] = b.c[4];
   dst[1][3] = b.c[4];
  }
  else
  {
   dst[0][2] = Interpolate5<R,G,B>( b.c[1], b.c[4] );
   dst[0][3] = Interpolate5<R,G,B>( b.c[1], b.c[5] );
   dst[1][3] = Interpolate5<R,G,B>( b.c[5], b.c[4] );
  }

  dst[1][2] = b.c[4];

  if (Diff( b.w[7], b.w[3] ))
  {
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
   dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
   dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
   dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  }
  else
  {
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
   dst[2][1] = b.c[4];
   dst[3][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
   dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  }

  if (Diff( b.w[5], b.w[7] ))
  {
   dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
   dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
   dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
   dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  }
  else
  {
   dst[2][2] = b.c[4];
   dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[5]);
   dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
   dst[3][3] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[5] );
  }
  break;

 case 218:

  if (Diff( b.w[3], b.w[1] ))
  {
   dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
   dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  }
  else
  {
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[3] );
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
   dst[1][1] = b.c[4];
  }

  if (Diff( b.w[1], b.w[5] ))
  {
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
   dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
   dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
   dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  }
  else
  {
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[0][3] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
   dst[1][2] = b.c[4];
   dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  }

  if (Diff( b.w[7], b.w[3] ))
  {
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
   dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
   dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
   dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  }
  else
  {
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
   dst[2][1] = b.c[4];
   dst[3][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
   dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  }

  dst[2][2] = b.c[4];

  if (Diff( b.w[5], b.w[7] ))
  {
   dst[2][3] = b.c[4];
   dst[3][2] = b.c[4];
   dst[3][3] = b.c[4];
  }
  else
  {
   dst[2][3] = Interpolate5<R,G,B>( b.c[5], b.c[4] );
   dst[3][2] = Interpolate5<R,G,B>( b.c[7], b.c[4] );
   dst[3][3] = Interpolate5<R,G,B>( b.c[7], b.c[5] );
  }
  break;

 case 91:

  if (Diff( b.w[3], b.w[1] ))
  {
   dst[0][0] = b.c[4];
   dst[0][1] = b.c[4];
   dst[1][0] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate5<R,G,B>( b.c[1], b.c[3] );
   dst[0][1] = Interpolate5<R,G,B>( b.c[1], b.c[4] );
   dst[1][0] = Interpolate5<R,G,B>( b.c[3], b.c[4] );
  }

  if (Diff( b.w[1], b.w[5] ))
  {
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
   dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
   dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
   dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  }
  else
  {
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[0][3] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
   dst[1][2] = b.c[4];
   dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  }

  dst[1][1] = b.c[4];

  if (Diff( b.w[7], b.w[3] ))
  {
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
   dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
   dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
   dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  }
  else
  {
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
   dst[2][1] = b.c[4];
   dst[3][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
   dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  }

  if (Diff( b.w[5], b.w[7] ))
  {
   dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
   dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
   dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
   dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  }
  else
  {
   dst[2][2] = b.c[4];
   dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[5]);
   dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
   dst[3][3] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[5] );
  }
  break;

 case 229:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[3] );
  dst[0][1] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[3] );
  dst[0][2] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[0][3] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[1][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[1][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[1] );
  dst[1][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[1] );
  dst[2][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[2][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[3][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[3][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  break;

 case 167:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[0][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[1][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[1][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[7] );
  dst[2][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[7] );
  dst[2][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[7]);
  dst[2][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[7] );
  dst[3][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[3][1] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[3][2] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[5] );
  dst[3][3] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[5] );
  break;

 case 173:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[0][3] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[1] );
  dst[1][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[1] );
  dst[2][0] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[7]);
  dst[2][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[7] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][1] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][2] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[5] );
  dst[3][3] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[5] );
  break;

 case 181:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[3] );
  dst[0][1] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[3] );
  dst[0][2] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[1][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][3] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[2][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[7] );
  dst[2][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[7] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][3] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[3][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[3][1] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[3][2] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  break;

 case 186:

  if (Diff( b.w[3], b.w[1] ))
  {
   dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
   dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  }
  else
  {
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[3] );
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
   dst[1][1] = b.c[4];
  }

  if (Diff( b.w[1], b.w[5] ))
  {
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
   dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
   dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
   dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  }
  else
  {
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[0][3] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
   dst[1][2] = b.c[4];
   dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  }

  dst[2][0] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][3] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][1] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][2] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  break;

 case 115:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );

  if (Diff( b.w[1], b.w[5] ))
  {
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
   dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
   dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
   dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  }
  else
  {
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[0][3] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
   dst[1][2] = b.c[4];
   dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  }

  dst[1][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[2][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );

  if (Diff( b.w[5], b.w[7] ))
  {
   dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
   dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
   dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
   dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  }
  else
  {
   dst[2][2] = b.c[4];
   dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[5]);
   dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
   dst[3][3] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[5] );
  }

  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[3][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  break;

 case 93:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][3] = Interpolate3<R,G,B>( b.c[4], b.c[1] );

  if (Diff( b.w[7], b.w[3] ))
  {
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
   dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
   dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
   dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  }
  else
  {
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
   dst[2][1] = b.c[4];
   dst[3][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
   dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  }

  if (Diff( b.w[5], b.w[7] ))
  {
   dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
   dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
   dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
   dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  }
  else
  {
   dst[2][2] = b.c[4];
   dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[5]);
   dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
   dst[3][3] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[5] );
  }
  break;

 case 206:

  if (Diff( b.w[3], b.w[1] ))
  {
   dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
   dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  }
  else
  {
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[3] );
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
   dst[1][1] = b.c[4];
  }

  dst[0][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[1][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );

  if (Diff( b.w[7], b.w[3] ))
  {
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
   dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
   dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
   dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  }
  else
  {
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
   dst[2][1] = b.c[4];
   dst[3][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
   dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  }

  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[2][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[3][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  break;

 case 205:
 case 201:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[0][3] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[1] );
  dst[1][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[1] );

  if (Diff( b.w[7], b.w[3] ))
  {
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
   dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
   dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
   dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  }
  else
  {
   dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
   dst[2][1] = b.c[4];
   dst[3][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
   dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  }

  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[2][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[3][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  break;

 case 174:
 case 46:

  if (Diff( b.w[3], b.w[1] ))
  {
   dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
   dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  }
  else
  {
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[3] );
   dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
   dst[1][1] = b.c[4];
  }

  dst[0][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[1][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[7]);
  dst[2][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[7] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][1] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][2] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[5] );
  dst[3][3] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[5] );
  break;

 case 179:
 case 147:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );

  if (Diff( b.w[1], b.w[5] ))
  {
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
   dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
   dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
   dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  }
  else
  {
   dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[0][3] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
   dst[1][2] = b.c[4];
   dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  }

  dst[1][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[2][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[7] );
  dst[2][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[7] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][3] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[3][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[3][1] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[3][2] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  break;

 case 117:
 case 116:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[3] );
  dst[0][1] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[3] );
  dst[0][2] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[1][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][3] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[2][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );

  if (Diff( b.w[5], b.w[7] ))
  {
   dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
   dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
   dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
   dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  }
  else
  {
   dst[2][2] = b.c[4];
   dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[5]);
   dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
   dst[3][3] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[5] );
  }

  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[3][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  break;

 case 189:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][3] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[2][0] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][3] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][1] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][2] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  break;

 case 231:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[0][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[1][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[1][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[2][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[3][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[3][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  break;

 case 126:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );

  if (Diff( b.w[1], b.w[5] ))
  {
   dst[0][2] = b.c[4];
   dst[0][3] = b.c[4];
   dst[1][3] = b.c[4];
  }
  else
  {
   dst[0][2] = Interpolate5<R,G,B>( b.c[1], b.c[4] );
   dst[0][3] = Interpolate5<R,G,B>( b.c[1], b.c[5] );
   dst[1][3] = Interpolate5<R,G,B>( b.c[5], b.c[4] );
  }

  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[1][2] = b.c[4];

  if (Diff( b.w[7], b.w[3] ))
  {
   dst[2][0] = b.c[4];
   dst[3][0] = b.c[4];
   dst[3][1] = b.c[4];
  }
  else
  {
   dst[2][0] = Interpolate5<R,G,B>( b.c[3], b.c[4] );
   dst[3][0] = Interpolate5<R,G,B>( b.c[7], b.c[3] );
   dst[3][1] = Interpolate5<R,G,B>( b.c[7], b.c[4] );
  }

  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
  dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 219:

  if (Diff( b.w[3], b.w[1] ))
  {
   dst[0][0] = b.c[4];
   dst[0][1] = b.c[4];
   dst[1][0] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate5<R,G,B>( b.c[1], b.c[3] );
   dst[0][1] = Interpolate5<R,G,B>( b.c[1], b.c[4] );
   dst[1][0] = Interpolate5<R,G,B>( b.c[3], b.c[4] );
  }

  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[2][2] = b.c[4];

  if (Diff( b.w[5], b.w[7] ))
  {
   dst[2][3] = b.c[4];
   dst[3][2] = b.c[4];
   dst[3][3] = b.c[4];
  }
  else
  {
   dst[2][3] = Interpolate5<R,G,B>( b.c[5], b.c[4] );
   dst[3][2] = Interpolate5<R,G,B>( b.c[7], b.c[4] );
   dst[3][3] = Interpolate5<R,G,B>( b.c[7], b.c[5] );
  }

  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  break;

 case 125:

  if (Diff( b.w[7], b.w[3] ))
  {
   dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
   dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
   dst[2][0] = b.c[4];
   dst[2][1] = b.c[4];
   dst[3][0] = b.c[4];
   dst[3][1] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
   dst[1][0] = Interpolate1<R,G,B>( b.c[3], b.c[4] );
   dst[2][0] = Interpolate8<R,G,B>( b.c[3], b.c[7] );
   dst[2][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[7] );
   dst[3][0] = Interpolate5<R,G,B>( b.c[7], b.c[3] );
   dst[3][1] = Interpolate2<R,G,B>( b.c[7], b.c[4], b.c[3] );
  }

  dst[0][1] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][3] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
  dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 221:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate8<R,G,B>( b.c[4], b.c[1] );

  if (Diff( b.w[5], b.w[7] ))
  {
   dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
   dst[1][3] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
   dst[2][2] = b.c[4];
   dst[2][3] = b.c[4];
   dst[3][2] = b.c[4];
   dst[3][3] = b.c[4];
  }
  else
  {
   dst[0][3] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
   dst[1][3] = Interpolate1<R,G,B>( b.c[5], b.c[4] );
   dst[2][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[7]);
   dst[2][3] = Interpolate8<R,G,B>( b.c[5], b.c[7] );
   dst[3][2] = Interpolate2<R,G,B>( b.c[7], b.c[4], b.c[5] );
   dst[3][3] = Interpolate5<R,G,B>( b.c[7], b.c[5] );
  }

  dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  break;

 case 207:

  if (Diff( b.w[3], b.w[1] ))
  {
   dst[0][0] = b.c[4];
   dst[0][1] = b.c[4];
   dst[0][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
   dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
   dst[1][0] = b.c[4];
   dst[1][1] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate5<R,G,B>( b.c[1], b.c[3] );
   dst[0][1] = Interpolate8<R,G,B>( b.c[1], b.c[3] );
   dst[0][2] = Interpolate1<R,G,B>( b.c[1], b.c[4] );
   dst[0][3] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[1][0] = Interpolate2<R,G,B>( b.c[3], b.c[4], b.c[1]);
   dst[1][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[1] );
  }

  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[1][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[2][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[3][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  break;

 case 238:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[1][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );

  if (Diff( b.w[7], b.w[3] ))
  {
   dst[2][0] = b.c[4];
   dst[2][1] = b.c[4];
   dst[3][0] = b.c[4];
   dst[3][1] = b.c[4];
   dst[3][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
   dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  }
  else
  {
   dst[2][0] = Interpolate2<R,G,B>( b.c[3], b.c[4], b.c[7] );
   dst[2][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[7] );
   dst[3][0] = Interpolate5<R,G,B>( b.c[7], b.c[3] );
   dst[3][1] = Interpolate8<R,G,B>( b.c[7], b.c[3] );
   dst[3][2] = Interpolate1<R,G,B>( b.c[7], b.c[4] );
   dst[3][3] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
  }

  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[2][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  break;

 case 190:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );

  if (Diff( b.w[1], b.w[5] ))
  {
   dst[0][2] = b.c[4];
   dst[0][3] = b.c[4];
   dst[1][2] = b.c[4];
   dst[1][3] = b.c[4];
   dst[2][3] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
   dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  }
  else
  {
   dst[0][2] = Interpolate2<R,G,B>( b.c[1], b.c[4], b.c[5] );
   dst[0][3] = Interpolate5<R,G,B>( b.c[1], b.c[5] );
   dst[1][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[1] );
   dst[1][3] = Interpolate8<R,G,B>( b.c[5], b.c[1] );
   dst[2][3] = Interpolate1<R,G,B>( b.c[5], b.c[4]);
   dst[3][3] = Interpolate1<R,G,B>( b.c[4], b.c[5] );
  }

  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[2][0] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][1] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][2] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  break;

 case 187:

  if (Diff( b.w[3], b.w[1] ))
  {
   dst[0][0] = b.c[4];
   dst[0][1] = b.c[4];
   dst[1][0] = b.c[4];
   dst[1][1] = b.c[4];
   dst[2][0] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
   dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  }
  else
  {
   dst[0][0] = Interpolate5<R,G,B>( b.c[1], b.c[3] );
   dst[0][1] = Interpolate2<R,G,B>( b.c[1], b.c[4], b.c[3] );
   dst[1][0] = Interpolate8<R,G,B>( b.c[3], b.c[1] );
   dst[1][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[1] );
   dst[2][0] = Interpolate1<R,G,B>( b.c[3], b.c[4] );
   dst[3][0] = Interpolate1<R,G,B>( b.c[4], b.c[3] );
  }

  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][3] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[3][1] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][2] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  break;

 case 243:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[2][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );

  if (Diff( b.w[5], b.w[7] ))
  {
   dst[2][2] = b.c[4];
   dst[2][3] = b.c[4];
   dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
   dst[3][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
   dst[3][2] = b.c[4];
   dst[3][3] = b.c[4];
  }
  else
  {
   dst[2][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[7]);
   dst[2][3] = Interpolate2<R,G,B>( b.c[5], b.c[4], b.c[7] );
   dst[3][0] = Interpolate1<R,G,B>( b.c[4], b.c[7] );
   dst[3][1] = Interpolate1<R,G,B>( b.c[7], b.c[4] );
   dst[3][2] = Interpolate8<R,G,B>( b.c[7], b.c[5] );
   dst[3][3] = Interpolate5<R,G,B>( b.c[7], b.c[5] );
  }
  break;

 case 119:

  if (Diff( b.w[1], b.w[5] ))
  {
   dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
   dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
   dst[0][2] = b.c[4];
   dst[0][3] = b.c[4];
   dst[1][2] = b.c[4];
   dst[1][3] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate1<R,G,B>( b.c[4], b.c[1] );
   dst[0][1] = Interpolate1<R,G,B>( b.c[1], b.c[4] );
   dst[0][2] = Interpolate8<R,G,B>( b.c[1], b.c[5] );
   dst[0][3] = Interpolate5<R,G,B>( b.c[1], b.c[5] );
   dst[1][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[1] );
   dst[1][3] = Interpolate2<R,G,B>( b.c[5], b.c[4], b.c[1] );
  }

  dst[1][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[2][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[3][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 237:
 case 233:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[0][3] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );
  dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[1] );
  dst[1][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[1] );
  dst[2][0] = b.c[4];
  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[2][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );

  if (Diff( b.w[7], b.w[3] ))
   dst[3][0] = b.c[4];
  else
   dst[3][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );

  dst[3][1] = b.c[4];
  dst[3][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  break;

 case 175:
 case 47:

  if (Diff( b.w[3], b.w[1] ))
   dst[0][0] = b.c[4];
  else
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[3] );

  dst[0][1] = b.c[4];
  dst[0][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[1][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate7<R,G,B>( b.c[4], b.c[5], b.c[7]);
  dst[2][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[7] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][1] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][2] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[5] );
  dst[3][3] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[5] );
  break;

 case 183:
 case 151:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[0][2] = b.c[4];

  if (Diff( b.w[1], b.w[5] ))
   dst[0][3] = b.c[4];
  else
   dst[0][3] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[1][2] = b.c[4];
  dst[1][3] = b.c[4];
  dst[2][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[7] );
  dst[2][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[7] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][3] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[3][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[3][1] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[3] );
  dst[3][2] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  break;

 case 245:
 case 244:

  dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[3] );
  dst[0][1] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[3] );
  dst[0][2] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[1][1] = Interpolate7<R,G,B>( b.c[4], b.c[3], b.c[1] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][3] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[2][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[2][2] = b.c[4];
  dst[2][3] = b.c[4];
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[3][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[3][2] = b.c[4];

  if (Diff( b.w[5], b.w[7] ))
   dst[3][3] = b.c[4];
  else
   dst[3][3] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[5] );
  break;

 case 250:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );

  if (Diff( b.w[7], b.w[3] ))
  {
   dst[2][0] = b.c[4];
   dst[3][0] = b.c[4];
   dst[3][1] = b.c[4];
  }
  else
  {
   dst[2][0] = Interpolate5<R,G,B>( b.c[3], b.c[4] );
   dst[3][0] = Interpolate5<R,G,B>( b.c[7], b.c[3] );
   dst[3][1] = Interpolate5<R,G,B>( b.c[7], b.c[4] );
  }

  dst[2][1] = b.c[4];
  dst[2][2] = b.c[4];

  if (Diff( b.w[5], b.w[7] ))
  {
   dst[2][3] = b.c[4];
   dst[3][2] = b.c[4];
   dst[3][3] = b.c[4];
  }
  else
  {
   dst[2][3] = Interpolate5<R,G,B>( b.c[5], b.c[4] );
   dst[3][2] = Interpolate5<R,G,B>( b.c[7], b.c[4] );
   dst[3][3] = Interpolate5<R,G,B>( b.c[7], b.c[5] );
  }
  break;

 case 123:

  if (Diff( b.w[3], b.w[1] ))
  {
   dst[0][0] = b.c[4];
   dst[0][1] = b.c[4];
   dst[1][0] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate5<R,G,B>( b.c[1], b.c[3] );
   dst[0][1] = Interpolate5<R,G,B>( b.c[1], b.c[4] );
   dst[1][0] = Interpolate5<R,G,B>( b.c[3], b.c[4] );
  }

  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );

  if (Diff( b.w[7], b.w[3] ))
  {
   dst[2][0] = b.c[4];
   dst[3][0] = b.c[4];
   dst[3][1] = b.c[4];
  }
  else
  {
   dst[2][0] = Interpolate5<R,G,B>( b.c[3], b.c[4] );
   dst[3][0] = Interpolate5<R,G,B>( b.c[7], b.c[3] );
   dst[3][1] = Interpolate5<R,G,B>( b.c[7], b.c[4] );
  }

  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
  dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 95:

  if (Diff( b.w[3], b.w[1] ))
  {
   dst[0][0] = b.c[4];
   dst[0][1] = b.c[4];
   dst[1][0] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate5<R,G,B>( b.c[1], b.c[3] );
   dst[0][1] = Interpolate5<R,G,B>( b.c[1], b.c[4] );
   dst[1][0] = Interpolate5<R,G,B>( b.c[3], b.c[4] );
  }

  if (Diff( b.w[1], b.w[5] ))
  {
   dst[0][2] = b.c[4];
   dst[0][3] = b.c[4];
   dst[1][3] = b.c[4];
  }
  else
  {
   dst[0][2] = Interpolate5<R,G,B>( b.c[1], b.c[4] );
   dst[0][3] = Interpolate5<R,G,B>( b.c[1], b.c[5] );
   dst[1][3] = Interpolate5<R,G,B>( b.c[5], b.c[4] );
  }

  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 222:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );

  if (Diff( b.w[1], b.w[5] ))
  {
   dst[0][2] = b.c[4];
   dst[0][3] = b.c[4];
   dst[1][3] = b.c[4];
  }
  else
  {
   dst[0][2] = Interpolate5<R,G,B>( b.c[1], b.c[4] );
   dst[0][3] = Interpolate5<R,G,B>( b.c[1], b.c[5] );
   dst[1][3] = Interpolate5<R,G,B>( b.c[5], b.c[4] );
  }

  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[2][2] = b.c[4];

  if (Diff( b.w[5], b.w[7] ))
  {
   dst[2][3] = b.c[4];
   dst[3][2] = b.c[4];
   dst[3][3] = b.c[4];
  }
  else
  {
   dst[2][3] = Interpolate5<R,G,B>( b.c[5], b.c[4] );
   dst[3][2] = Interpolate5<R,G,B>( b.c[7], b.c[4] );
   dst[3][3] = Interpolate5<R,G,B>( b.c[7], b.c[5] );
  }

  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  break;

 case 252:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[0] );
  dst[0][2] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][3] = Interpolate3<R,G,B>( b.c[4], b.c[1] );

  if (Diff( b.w[7], b.w[3] ))
  {
   dst[2][0] = b.c[4];
   dst[3][0] = b.c[4];
   dst[3][1] = b.c[4];
  }
  else
  {
   dst[2][0] = Interpolate5<R,G,B>( b.c[3], b.c[4] );
   dst[3][0] = Interpolate5<R,G,B>( b.c[7], b.c[3] );
   dst[3][1] = Interpolate5<R,G,B>( b.c[7], b.c[4] );
  }

  dst[2][1] = b.c[4];
  dst[2][2] = b.c[4];
  dst[2][3] = b.c[4];
  dst[3][2] = b.c[4];

  if (Diff( b.w[5], b.w[7] ))
   dst[3][3] = b.c[4];
  else
   dst[3][3] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[5] );

  break;

 case 249:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate6<R,G,B>( b.c[4], b.c[1], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[2][0] = b.c[4];
  dst[2][1] = b.c[4];
  dst[2][2] = b.c[4];

  if (Diff( b.w[5], b.w[7] ))
  {
   dst[2][3] = b.c[4];
   dst[3][2] = b.c[4];
   dst[3][3] = b.c[4];
  }
  else
  {
   dst[2][3] = Interpolate5<R,G,B>( b.c[5], b.c[4] );
   dst[3][2] = Interpolate5<R,G,B>( b.c[7], b.c[4] );
   dst[3][3] = Interpolate5<R,G,B>( b.c[7], b.c[5] );
  }

  if (Diff( b.w[7], b.w[3] ))
   dst[3][0] = b.c[4];
  else
   dst[3][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );

  dst[3][1] = b.c[4];
  break;

 case 235:

  if (Diff( b.w[3], b.w[1] ))
  {
   dst[0][0] = b.c[4];
   dst[0][1] = b.c[4];
   dst[1][0] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate5<R,G,B>( b.c[1], b.c[3] );
   dst[0][1] = Interpolate5<R,G,B>( b.c[1], b.c[4] );
   dst[1][0] = Interpolate5<R,G,B>( b.c[3], b.c[4] );
  }

  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[2] );
  dst[2][0] = b.c[4];
  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[2][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );

  if (Diff( b.w[7], b.w[3] ))
   dst[3][0] = b.c[4];
  else
   dst[3][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );

  dst[3][1] = b.c[4];
  dst[3][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  break;

 case 111:

  if (Diff( b.w[3], b.w[1] ))
   dst[0][0] = b.c[4];
  else
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[3] );

  dst[0][1] = b.c[4];
  dst[0][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[1][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );

  if (Diff( b.w[7], b.w[3] ))
  {
   dst[2][0] = b.c[4];
   dst[3][0] = b.c[4];
   dst[3][1] = b.c[4];
  }
  else
  {
   dst[2][0] = Interpolate5<R,G,B>( b.c[3], b.c[4] );
   dst[3][0] = Interpolate5<R,G,B>( b.c[7], b.c[3] );
   dst[3][1] = Interpolate5<R,G,B>( b.c[7], b.c[4] );
  }

  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate6<R,G,B>( b.c[4], b.c[5], b.c[8] );
  dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 63:

  if (Diff( b.w[3], b.w[1] ))
   dst[0][0] = b.c[4];
  else
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[3] );

  dst[0][1] = b.c[4];

  if (Diff( b.w[1], b.w[5] ))
  {
   dst[0][2] = b.c[4];
   dst[0][3] = b.c[4];
   dst[1][3] = b.c[4];
  }
  else
  {
   dst[0][2] = Interpolate5<R,G,B>( b.c[1], b.c[4] );
   dst[0][3] = Interpolate5<R,G,B>( b.c[1], b.c[5] );
   dst[1][3] = Interpolate5<R,G,B>( b.c[5], b.c[4] );
  }

  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][1] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][2] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 159:

  if (Diff( b.w[3], b.w[1] ))
  {
   dst[0][0] = b.c[4];
   dst[0][1] = b.c[4];
   dst[1][0] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate5<R,G,B>( b.c[1], b.c[3] );
   dst[0][1] = Interpolate5<R,G,B>( b.c[1], b.c[4] );
   dst[1][0] = Interpolate5<R,G,B>( b.c[3], b.c[4] );
  }

  dst[0][2] = b.c[4];

  if (Diff( b.w[1], b.w[5] ))
   dst[0][3] = b.c[4];
  else
   dst[0][3] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[1][3] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][3] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate6<R,G,B>( b.c[4], b.c[7], b.c[6] );
  dst[3][2] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  break;

 case 215:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[0][2] = b.c[4];

  if (Diff( b.w[1], b.w[5] ))
   dst[0][3] = b.c[4];
  else
   dst[0][3] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[1][2] = b.c[4];
  dst[1][3] = b.c[4];
  dst[2][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[2][2] = b.c[4];

  if (Diff( b.w[5], b.w[7] ))
  {
   dst[2][3] = b.c[4];
   dst[3][2] = b.c[4];
   dst[3][3] = b.c[4];
  }
  else
  {
   dst[2][3] = Interpolate5<R,G,B>( b.c[5], b.c[4] );
   dst[3][2] = Interpolate5<R,G,B>( b.c[7], b.c[4] );
   dst[3][3] = Interpolate5<R,G,B>( b.c[7], b.c[5] );
  }

  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  break;

 case 246:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );

  if (Diff( b.w[1], b.w[5] ))
  {
   dst[0][2] = b.c[4];
   dst[0][3] = b.c[4];
   dst[1][3] = b.c[4];
  }
  else
  {
   dst[0][2] = Interpolate5<R,G,B>( b.c[1], b.c[4] );
   dst[0][3] = Interpolate5<R,G,B>( b.c[1], b.c[5] );
   dst[1][3] = Interpolate5<R,G,B>( b.c[5], b.c[4] );
  }

  dst[1][0] = Interpolate6<R,G,B>( b.c[4], b.c[3], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[1][2] = b.c[4];
  dst[2][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[2][2] = b.c[4];
  dst[2][3] = b.c[4];
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[3][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[3][2] = b.c[4];

  if (Diff( b.w[5], b.w[7] ))
   dst[3][3] = b.c[4];
  else
   dst[3][3] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[5] );

  break;

 case 254:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[0] );
  dst[0][1] = Interpolate1<R,G,B>( b.c[4], b.c[0] );

  if (Diff( b.w[1], b.w[5] ))
  {
   dst[0][2] = b.c[4];
   dst[0][3] = b.c[4];
   dst[1][3] = b.c[4];
  }
  else
  {
   dst[0][2] = Interpolate5<R,G,B>( b.c[1], b.c[4] );
   dst[0][3] = Interpolate5<R,G,B>( b.c[1], b.c[5] );
   dst[1][3] = Interpolate5<R,G,B>( b.c[5], b.c[4] );
  }

  dst[1][0] = Interpolate1<R,G,B>( b.c[4], b.c[0] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[0] );
  dst[1][2] = b.c[4];

  if (Diff( b.w[7], b.w[3] ))
  {
   dst[2][0] = b.c[4];
   dst[3][0] = b.c[4];
   dst[3][1] = b.c[4];
  }
  else
  {
   dst[2][0] = Interpolate5<R,G,B>( b.c[3], b.c[4] );
   dst[3][0] = Interpolate5<R,G,B>( b.c[7], b.c[3] );
   dst[3][1] = Interpolate5<R,G,B>( b.c[7], b.c[4] );
  }

  dst[2][1] = b.c[4];
  dst[2][2] = b.c[4];
  dst[2][3] = b.c[4];
  dst[3][2] = b.c[4];

  if (Diff( b.w[5], b.w[7] ))
   dst[3][3] = b.c[4];
  else
   dst[3][3] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[5] );

  break;

 case 253:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][1] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][2] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[1] );
  dst[1][0] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[1][3] = Interpolate3<R,G,B>( b.c[4], b.c[1] );
  dst[2][0] = b.c[4];
  dst[2][1] = b.c[4];
  dst[2][2] = b.c[4];
  dst[2][3] = b.c[4];

  if (Diff( b.w[7], b.w[3] ))
   dst[3][0] = b.c[4];
  else
   dst[3][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );

  dst[3][1] = b.c[4];
  dst[3][2] = b.c[4];

  if (Diff( b.w[5], b.w[7] ))
   dst[3][3] = b.c[4];
  else
   dst[3][3] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[5] );

  break;

 case 251:

  if (Diff( b.w[3], b.w[1] ))
  {
   dst[0][0] = b.c[4];
   dst[0][1] = b.c[4];
   dst[1][0] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate5<R,G,B>( b.c[1], b.c[3] );
   dst[0][1] = Interpolate5<R,G,B>( b.c[1], b.c[4] );
   dst[1][0] = Interpolate5<R,G,B>( b.c[3], b.c[4] );
  }

  dst[0][2] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[2] );
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[2] );
  dst[1][3] = Interpolate1<R,G,B>( b.c[4], b.c[2] );
  dst[2][0] = b.c[4];
  dst[2][1] = b.c[4];
  dst[2][2] = b.c[4];

  if (Diff( b.w[5], b.w[7] ))
  {
   dst[2][3] = b.c[4];
   dst[3][2] = b.c[4];
   dst[3][3] = b.c[4];
  }
  else
  {
   dst[2][3] = Interpolate5<R,G,B>( b.c[5], b.c[4] );
   dst[3][2] = Interpolate5<R,G,B>( b.c[7], b.c[4] );
   dst[3][3] = Interpolate5<R,G,B>( b.c[7], b.c[5] );
  }

  if (Diff( b.w[7], b.w[3] ))
   dst[3][0] = b.c[4];
  else
   dst[3][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );

  dst[3][1] = b.c[4];
  break;

 case 239:

  if (Diff( b.w[3], b.w[1] ))
   dst[0][0] = b.c[4];
  else
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[3] );

  dst[0][1] = b.c[4];
  dst[0][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[0][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[1][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  dst[2][0] = b.c[4];
  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[2][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );

  if (Diff( b.w[7], b.w[3] ))
   dst[3][0] = b.c[4];
  else
   dst[3][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );

  dst[3][1] = b.c[4];
  dst[3][2] = Interpolate3<R,G,B>( b.c[4], b.c[5] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[5] );
  break;

 case 127:

  if (Diff( b.w[3], b.w[1] ))
   dst[0][0] = b.c[4];
  else
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[3] );

  dst[0][1] = b.c[4];

  if (Diff( b.w[1], b.w[5] ))
  {
   dst[0][2] = b.c[4];
   dst[0][3] = b.c[4];
   dst[1][3] = b.c[4];
  }
  else
  {
   dst[0][2] = Interpolate5<R,G,B>( b.c[1], b.c[4] );
   dst[0][3] = Interpolate5<R,G,B>( b.c[1], b.c[5] );
   dst[1][3] = Interpolate5<R,G,B>( b.c[5], b.c[4] );
  }

  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];

  if (Diff( b.w[7], b.w[3] ))
  {
   dst[2][0] = b.c[4];
   dst[3][0] = b.c[4];
   dst[3][1] = b.c[4];
  }
  else
  {
   dst[2][0] = Interpolate5<R,G,B>( b.c[3], b.c[4] );
   dst[3][0] = Interpolate5<R,G,B>( b.c[7], b.c[3] );
   dst[3][1] = Interpolate5<R,G,B>( b.c[7], b.c[4] );
  }

  dst[2][1] = b.c[4];
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[8] );
  dst[2][3] = Interpolate1<R,G,B>( b.c[4], b.c[8]);
  dst[3][2] = Interpolate1<R,G,B>( b.c[4], b.c[8] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[8] );
  break;

 case 191:

  if (Diff( b.w[3], b.w[1] ))
   dst[0][0] = b.c[4];
  else
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[3] );

  dst[0][1] = b.c[4];
  dst[0][2] = b.c[4];

  if (Diff( b.w[1], b.w[5] ))
   dst[0][3] = b.c[4];
  else
   dst[0][3] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[1][3] = b.c[4];
  dst[2][0] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][2] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[2][3] = Interpolate3<R,G,B>( b.c[4], b.c[7] );
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][1] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][2] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  dst[3][3] = Interpolate8<R,G,B>( b.c[4], b.c[7] );
  break;

 case 223:

  if (Diff( b.w[3], b.w[1] ))
  {
   dst[0][0] = b.c[4];
   dst[0][1] = b.c[4];
   dst[1][0] = b.c[4];
  }
  else
  {
   dst[0][0] = Interpolate5<R,G,B>( b.c[1], b.c[3] );
   dst[0][1] = Interpolate5<R,G,B>( b.c[1], b.c[4] );
   dst[1][0] = Interpolate5<R,G,B>( b.c[3], b.c[4] );
  }

  dst[0][2] = b.c[4];

  if (Diff( b.w[1], b.w[5] ))
   dst[0][3] = b.c[4];
  else
   dst[0][3] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[1][3] = b.c[4];
  dst[2][0] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[6] );
  dst[2][2] = b.c[4];

  if (Diff( b.w[5], b.w[7] ))
  {
   dst[2][3] = b.c[4];
   dst[3][2] = b.c[4];
   dst[3][3] = b.c[4];
  }
  else
  {
   dst[2][3] = Interpolate5<R,G,B>( b.c[5], b.c[4] );
   dst[3][2] = Interpolate5<R,G,B>( b.c[7], b.c[4] );
   dst[3][3] = Interpolate5<R,G,B>( b.c[7], b.c[5] );
  }

  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[6] );
  dst[3][1] = Interpolate1<R,G,B>( b.c[4], b.c[6] );
  break;

 case 247:

  dst[0][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[0][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[0][2] = b.c[4];

  if (Diff( b.w[1], b.w[5] ))
   dst[0][3] = b.c[4];
  else
   dst[0][3] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[1][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[1][2] = b.c[4];
  dst[1][3] = b.c[4];
  dst[2][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[2][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[2][2] = b.c[4];
  dst[2][3] = b.c[4];
  dst[3][0] = Interpolate8<R,G,B>( b.c[4], b.c[3] );
  dst[3][1] = Interpolate3<R,G,B>( b.c[4], b.c[3] );
  dst[3][2] = b.c[4];

  if (Diff( b.w[5], b.w[7] ))
   dst[3][3] = b.c[4];
  else
   dst[3][3] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[5] );

  break;

 case 255:

  if (Diff( b.w[3], b.w[1] ))
   dst[0][0] = b.c[4];
  else
   dst[0][0] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[3] );

  dst[0][1] = b.c[4];
  dst[0][2] = b.c[4];

  if (Diff( b.w[1], b.w[5] ))
   dst[0][3] = b.c[4];
  else
   dst[0][3] = Interpolate2<R,G,B>( b.c[4], b.c[1], b.c[5] );

  dst[1][0] = b.c[4];
  dst[1][1] = b.c[4];
  dst[1][2] = b.c[4];
  dst[1][3] = b.c[4];
  dst[2][0] = b.c[4];
  dst[2][1] = b.c[4];
  dst[2][2] = b.c[4];
  dst[2][3] = b.c[4];

  if (Diff( b.w[7], b.w[3] ))
   dst[3][0] = b.c[4];
  else
   dst[3][0] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[3] );

  dst[3][1] = b.c[4];
  dst[3][2] = b.c[4];

  if (Diff( b.w[5], b.w[7] ))
   dst[3][3] = b.c[4];
  else
   dst[3][3] = Interpolate2<R,G,B>( b.c[4], b.c[7], b.c[5] );

  break;


}
# 326 "/Users/Jorrit/iOS/nestopia/core/NstVideoFilterHqX.cpp" 2

     }

     dst[0] = reinterpret_cast<T*>(reinterpret_cast<byte*>(dst[0]) + pitch);
     dst[1] = reinterpret_cast<T*>(reinterpret_cast<byte*>(dst[1]) + pitch);
     dst[2] = reinterpret_cast<T*>(reinterpret_cast<byte*>(dst[2]) + pitch);
     dst[3] = reinterpret_cast<T*>(reinterpret_cast<byte*>(dst[3]) + pitch);
    }
   }





   Renderer::FilterHqX::Lut::Lut(const bool bpp32,const dword (&left)[3],dword* tmp)
   : rgb(tmp = (bpp32 ? new dword [0x10000] : __null))
   {
    const uint shifts[3] =
    {
     bpp32 ? 11 : left[0],
     bpp32 ? 5 : left[1],
     bpp32 ? 0 : left[2]
    };

    for (uint i=0; i < 32; ++i)
    {
     for (uint j=0; j < 64; ++j)
     {
      for (uint k=0; k < 32; ++k)
      {
       uint r = i << 3;
       uint g = j << 2;
       uint b = k << 3;

       dword y = ((r + g + b) >> 2) & 0xFF;
       dword u = (128 + ((r - b) >> 2)) & 0xFF;
       dword v = (128 + ((2*g - r - b) >> 3)) & 0xFF;

       yuv[(i << shifts[0]) | (j << shifts[1]) | (k << shifts[2])] = (y << 16) | (u << 8) | (v << 0);
      }
     }
    }

    if (bpp32)
    {
     for (dword i=0; i < 0x10000; ++i)
      tmp[i] = ((i & 0xF800) << 8) | ((i & 0x07E0) << 5) | ((i & 0x001F) << 3);
    }
   }

   Renderer::FilterHqX::Lut::~Lut()
   {
    delete [] rgb;
   }

   Renderer::FilterHqX::Path Renderer::FilterHqX::GetPath(const RenderState& state)
   {
    if (state.filter == RenderState::FILTER_HQ2X)
    {
     if (state.bits.count == 32)
     {
      return &FilterHqX::Blit2x<dword,0xFF0000,0x00FF00,0x0000FF>;
     }
     else if (state.bits.mask.g == 0x07E0)
     {
      return &FilterHqX::Blit2x<word,0xF800,0x07E0,0x001F>;
     }
     else
     {
      return &FilterHqX::Blit2x<word,0x7C00,0x03E0,0x001F>;
     }
    }
    else if (state.filter == RenderState::FILTER_HQ3X)
    {
     if (state.bits.count == 32)
     {
      return &FilterHqX::Blit3x<dword,0xFF0000,0x00FF00,0x0000FF>;
     }
     else if (state.bits.mask.g == 0x07E0)
     {
      return &FilterHqX::Blit3x<word,0xF800,0x07E0,0x001F>;
     }
     else
     {
      return &FilterHqX::Blit3x<word,0x7C00,0x03E0,0x001F>;
     }
    }
    else
    {
     if (state.bits.count == 32)
     {
      return &FilterHqX::Blit4x<dword,0xFF0000,0x00FF00,0x0000FF>;
     }
     else if (state.bits.mask.g == 0x07E0)
     {
      return &FilterHqX::Blit4x<word,0xF800,0x07E0,0x001F>;
     }
     else
     {
      return &FilterHqX::Blit4x<word,0x7C00,0x03E0,0x001F>;
     }
    }
   }

   Renderer::FilterHqX::FilterHqX(const RenderState& state)
   :
   Filter (state),
   path (GetPath(state)),
   lut (state.bits.count == 32,format.left)
   {
   }

   bool Renderer::FilterHqX::Check(const RenderState& state)
   {
    return (state.scanlines == 0) &&
    (
     (state.bits.count == 16 && state.bits.mask.b == 0x001F && ((state.bits.mask.g == 0x07E0 && state.bits.mask.r == 0xF800) || (state.bits.mask.g == 0x03E0 && state.bits.mask.r == 0x7C00))) ||
     (state.bits.count == 32 && state.bits.mask.r == 0xFF0000 && state.bits.mask.g == 0x00FF00 && state.bits.mask.b == 0x0000FF)
    )
    &&
    (
     (state.filter == RenderState::FILTER_HQ2X && state.width == WIDTH*2 && state.height == HEIGHT*2) ||
     (state.filter == RenderState::FILTER_HQ3X && state.width == WIDTH*3 && state.height == HEIGHT*3) ||
     (state.filter == RenderState::FILTER_HQ4X && state.width == WIDTH*4 && state.height == HEIGHT*4)
    );
   }

   void Renderer::FilterHqX::Transform(const byte (&src)[PALETTE][3],Input::Palette& dst) const
   {
    uint rgb[2][3];

    if (bpp == 16)
    {
     rgb[0][0] = format.right[0];
     rgb[0][1] = format.right[1];
     rgb[0][2] = format.right[2];
     rgb[1][0] = format.left[0];
     rgb[1][1] = format.left[1];
     rgb[1][2] = format.left[2];
    }
    else
    {
     rgb[0][0] = 3;
     rgb[0][1] = 2;
     rgb[0][2] = 3;
     rgb[1][0] = 11;
     rgb[1][1] = 5;
     rgb[1][2] = 0;
    }

    for (uint i=0; i < PALETTE; ++i)
    {
     dst[i] =
     (
      (dword(src[i][0]) >> rgb[0][0] << rgb[1][0]) |
      (dword(src[i][1]) >> rgb[0][1] << rgb[1][1]) |
      (dword(src[i][2]) >> rgb[0][2] << rgb[1][2])
     );
    }
   }




  }
 }
}



