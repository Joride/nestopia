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
#include "NstVideoRenderer.hpp"
#include "NstVideoFilterNtsc.hpp"
#include "NstFpuPrecision.hpp"

namespace Nes
{
    namespace Core
    {
        namespace Video
        {
            void Renderer::FilterNtsc::Blit(const Input& input,const Output& output,uint phase)
            {
                (*this.*path)( input, output, phase );
            }
            
            template<typename Pixel,uint BITS>
            void Renderer::FilterNtsc::BlitType(const Input& input,const Output& output,uint phase) const
            {
                enum
                {
                    BPP = BITS,
                    R_MASK = BPP == 32 ? 0xFF0000 : BPP == 16 ? 0xF800 : 0x7C00,
                    G_MASK = BPP == 32 ? 0x00FF00 : BPP == 16 ? 0x07E0 : 0x03E0,
                    B_MASK = BPP == 32 ? 0x0000FF : BPP == 16 ? 0x001F : 0x001F,
                    RB_MASK = R_MASK|B_MASK,
                    S_SHIFT = BPP == 32 ? 8 : 5
                };
                
                (__builtin_expect(!(!!(phase < 3)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstVideoFilterNtsc.cpp", 54, "!!(phase < 3)") : (void)0);
                
                Pixel buffer[NTSC_WIDTH+1];
                
                const Input::Pixel* __restrict__ src = input.pixels;
                Pixel* __restrict__ dst = static_cast<Pixel*>(output.pixels);
                const long pad = output.pitch - NTSC_WIDTH * sizeof(Pixel);
                
                phase &= lut.noFieldMerging;
                
                for (uint y=HEIGHT; y; --y)
                {
                    char const* const ktable = (char const*) (&lut)->table [0] + phase * (nes_ntsc_burst_size * sizeof (nes_ntsc_rgb_t)); unsigned const nes_ntsc_pixel0_ = (lut.black); nes_ntsc_rgb_t const* kernel0 = (nes_ntsc_rgb_t const*) (ktable + (nes_ntsc_pixel0_) * (nes_ntsc_entry_size * sizeof (nes_ntsc_rgb_t))); unsigned const nes_ntsc_pixel1_ = (lut.black); nes_ntsc_rgb_t const* kernel1 = (nes_ntsc_rgb_t const*) (ktable + (nes_ntsc_pixel1_) * (nes_ntsc_entry_size * sizeof (nes_ntsc_rgb_t))); unsigned const nes_ntsc_pixel2_ = (*src++); nes_ntsc_rgb_t const* kernel2 = (nes_ntsc_rgb_t const*) (ktable + (nes_ntsc_pixel2_) * (nes_ntsc_entry_size * sizeof (nes_ntsc_rgb_t))); nes_ntsc_rgb_t const* kernelx0; nes_ntsc_rgb_t const* kernelx1 = kernel0; nes_ntsc_rgb_t const* kernelx2 = kernel0;
                    
                    Pixel* __restrict__ cache = buffer;
                    
                    for (const Input::Pixel* const end=src+(NTSC_WIDTH/7*3-3); src != end; src += 3, dst += 7, cache += 7)
                    {
                        { unsigned color_; kernelx0 = kernel0; kernel0 = (color_ = (src[0]), (nes_ntsc_rgb_t const*) (ktable + (color_) * (nes_ntsc_entry_size * sizeof (nes_ntsc_rgb_t))));};
                        { nes_ntsc_rgb_t raw_ = kernel0 [0 ] + kernel1 [(0 +12)%7+14] + kernel2 [(0 +10)%7+28] + kernelx0 [(0 +7)%14] + kernelx1 [(0 + 5)%7+21] + kernelx2 [(0 + 3)%7+35]; { nes_ntsc_rgb_t sub = (raw_) >> (9-(0)) & (((1L << 21) | (1 << 11) | (1 << 1)) * 3 / 2); nes_ntsc_rgb_t clamp = (((1L << 21) | (1 << 11) | (1 << 1)) * 0x101) - sub; raw_ |= clamp; clamp -= sub; raw_ &= clamp;}; { if ( BPP == 16 ) cache[0] = (raw_>>(13-0)& 0xF800)|(raw_>>(8-0)&0x07E0)|(raw_>>(4-0)&0x001F); if ( BPP == 24 || BPP == 32 ) cache[0] = (raw_>>(5-0)&0xFF0000)|(raw_>>(3-0)&0xFF00)|(raw_>>(1-0)&0xFF); if ( BPP == 15 ) cache[0] = (raw_>>(14-0)& 0x7C00)|(raw_>>(9-0)&0x03E0)|(raw_>>(4-0)&0x001F); if ( BPP == 0 ) cache[0] = raw_ << 0;};}; dst[0] = cache[0];
                        { nes_ntsc_rgb_t raw_ = kernel0 [1 ] + kernel1 [(1 +12)%7+14] + kernel2 [(1 +10)%7+28] + kernelx0 [(1 +7)%14] + kernelx1 [(1 + 5)%7+21] + kernelx2 [(1 + 3)%7+35]; { nes_ntsc_rgb_t sub = (raw_) >> (9-(0)) & (((1L << 21) | (1 << 11) | (1 << 1)) * 3 / 2); nes_ntsc_rgb_t clamp = (((1L << 21) | (1 << 11) | (1 << 1)) * 0x101) - sub; raw_ |= clamp; clamp -= sub; raw_ &= clamp;}; { if ( BPP == 16 ) cache[1] = (raw_>>(13-0)& 0xF800)|(raw_>>(8-0)&0x07E0)|(raw_>>(4-0)&0x001F); if ( BPP == 24 || BPP == 32 ) cache[1] = (raw_>>(5-0)&0xFF0000)|(raw_>>(3-0)&0xFF00)|(raw_>>(1-0)&0xFF); if ( BPP == 15 ) cache[1] = (raw_>>(14-0)& 0x7C00)|(raw_>>(9-0)&0x03E0)|(raw_>>(4-0)&0x001F); if ( BPP == 0 ) cache[1] = raw_ << 0;};}; dst[1] = cache[1];
                        
                        { unsigned color_; kernelx1 = kernel1; kernel1 = (color_ = (src[1]), (nes_ntsc_rgb_t const*) (ktable + (color_) * (nes_ntsc_entry_size * sizeof (nes_ntsc_rgb_t))));};
                        { nes_ntsc_rgb_t raw_ = kernel0 [2 ] + kernel1 [(2 +12)%7+14] + kernel2 [(2 +10)%7+28] + kernelx0 [(2 +7)%14] + kernelx1 [(2 + 5)%7+21] + kernelx2 [(2 + 3)%7+35]; { nes_ntsc_rgb_t sub = (raw_) >> (9-(0)) & (((1L << 21) | (1 << 11) | (1 << 1)) * 3 / 2); nes_ntsc_rgb_t clamp = (((1L << 21) | (1 << 11) | (1 << 1)) * 0x101) - sub; raw_ |= clamp; clamp -= sub; raw_ &= clamp;}; { if ( BPP == 16 ) cache[2] = (raw_>>(13-0)& 0xF800)|(raw_>>(8-0)&0x07E0)|(raw_>>(4-0)&0x001F); if ( BPP == 24 || BPP == 32 ) cache[2] = (raw_>>(5-0)&0xFF0000)|(raw_>>(3-0)&0xFF00)|(raw_>>(1-0)&0xFF); if ( BPP == 15 ) cache[2] = (raw_>>(14-0)& 0x7C00)|(raw_>>(9-0)&0x03E0)|(raw_>>(4-0)&0x001F); if ( BPP == 0 ) cache[2] = raw_ << 0;};}; dst[2] = cache[2];
                        { nes_ntsc_rgb_t raw_ = kernel0 [3 ] + kernel1 [(3 +12)%7+14] + kernel2 [(3 +10)%7+28] + kernelx0 [(3 +7)%14] + kernelx1 [(3 + 5)%7+21] + kernelx2 [(3 + 3)%7+35]; { nes_ntsc_rgb_t sub = (raw_) >> (9-(0)) & (((1L << 21) | (1 << 11) | (1 << 1)) * 3 / 2); nes_ntsc_rgb_t clamp = (((1L << 21) | (1 << 11) | (1 << 1)) * 0x101) - sub; raw_ |= clamp; clamp -= sub; raw_ &= clamp;}; { if ( BPP == 16 ) cache[3] = (raw_>>(13-0)& 0xF800)|(raw_>>(8-0)&0x07E0)|(raw_>>(4-0)&0x001F); if ( BPP == 24 || BPP == 32 ) cache[3] = (raw_>>(5-0)&0xFF0000)|(raw_>>(3-0)&0xFF00)|(raw_>>(1-0)&0xFF); if ( BPP == 15 ) cache[3] = (raw_>>(14-0)& 0x7C00)|(raw_>>(9-0)&0x03E0)|(raw_>>(4-0)&0x001F); if ( BPP == 0 ) cache[3] = raw_ << 0;};}; dst[3] = cache[3];
                        
                        { unsigned color_; kernelx2 = kernel2; kernel2 = (color_ = (src[2]), (nes_ntsc_rgb_t const*) (ktable + (color_) * (nes_ntsc_entry_size * sizeof (nes_ntsc_rgb_t))));};
                        { nes_ntsc_rgb_t raw_ = kernel0 [4 ] + kernel1 [(4 +12)%7+14] + kernel2 [(4 +10)%7+28] + kernelx0 [(4 +7)%14] + kernelx1 [(4 + 5)%7+21] + kernelx2 [(4 + 3)%7+35]; { nes_ntsc_rgb_t sub = (raw_) >> (9-(0)) & (((1L << 21) | (1 << 11) | (1 << 1)) * 3 / 2); nes_ntsc_rgb_t clamp = (((1L << 21) | (1 << 11) | (1 << 1)) * 0x101) - sub; raw_ |= clamp; clamp -= sub; raw_ &= clamp;}; { if ( BPP == 16 ) cache[4] = (raw_>>(13-0)& 0xF800)|(raw_>>(8-0)&0x07E0)|(raw_>>(4-0)&0x001F); if ( BPP == 24 || BPP == 32 ) cache[4] = (raw_>>(5-0)&0xFF0000)|(raw_>>(3-0)&0xFF00)|(raw_>>(1-0)&0xFF); if ( BPP == 15 ) cache[4] = (raw_>>(14-0)& 0x7C00)|(raw_>>(9-0)&0x03E0)|(raw_>>(4-0)&0x001F); if ( BPP == 0 ) cache[4] = raw_ << 0;};}; dst[4] = cache[4];
                        { nes_ntsc_rgb_t raw_ = kernel0 [5 ] + kernel1 [(5 +12)%7+14] + kernel2 [(5 +10)%7+28] + kernelx0 [(5 +7)%14] + kernelx1 [(5 + 5)%7+21] + kernelx2 [(5 + 3)%7+35]; { nes_ntsc_rgb_t sub = (raw_) >> (9-(0)) & (((1L << 21) | (1 << 11) | (1 << 1)) * 3 / 2); nes_ntsc_rgb_t clamp = (((1L << 21) | (1 << 11) | (1 << 1)) * 0x101) - sub; raw_ |= clamp; clamp -= sub; raw_ &= clamp;}; { if ( BPP == 16 ) cache[5] = (raw_>>(13-0)& 0xF800)|(raw_>>(8-0)&0x07E0)|(raw_>>(4-0)&0x001F); if ( BPP == 24 || BPP == 32 ) cache[5] = (raw_>>(5-0)&0xFF0000)|(raw_>>(3-0)&0xFF00)|(raw_>>(1-0)&0xFF); if ( BPP == 15 ) cache[5] = (raw_>>(14-0)& 0x7C00)|(raw_>>(9-0)&0x03E0)|(raw_>>(4-0)&0x001F); if ( BPP == 0 ) cache[5] = raw_ << 0;};}; dst[5] = cache[5];
                        { nes_ntsc_rgb_t raw_ = kernel0 [6 ] + kernel1 [(6 +12)%7+14] + kernel2 [(6 +10)%7+28] + kernelx0 [(6 +7)%14] + kernelx1 [(6 + 5)%7+21] + kernelx2 [(6 + 3)%7+35]; { nes_ntsc_rgb_t sub = (raw_) >> (9-(0)) & (((1L << 21) | (1 << 11) | (1 << 1)) * 3 / 2); nes_ntsc_rgb_t clamp = (((1L << 21) | (1 << 11) | (1 << 1)) * 0x101) - sub; raw_ |= clamp; clamp -= sub; raw_ &= clamp;}; { if ( BPP == 16 ) cache[6] = (raw_>>(13-0)& 0xF800)|(raw_>>(8-0)&0x07E0)|(raw_>>(4-0)&0x001F); if ( BPP == 24 || BPP == 32 ) cache[6] = (raw_>>(5-0)&0xFF0000)|(raw_>>(3-0)&0xFF00)|(raw_>>(1-0)&0xFF); if ( BPP == 15 ) cache[6] = (raw_>>(14-0)& 0x7C00)|(raw_>>(9-0)&0x03E0)|(raw_>>(4-0)&0x001F); if ( BPP == 0 ) cache[6] = raw_ << 0;};}; dst[6] = cache[6];
                    }
                    
                    { unsigned color_; kernelx0 = kernel0; kernel0 = (color_ = (lut.black), (nes_ntsc_rgb_t const*) (ktable + (color_) * (nes_ntsc_entry_size * sizeof (nes_ntsc_rgb_t))));};
                    { nes_ntsc_rgb_t raw_ = kernel0 [0 ] + kernel1 [(0 +12)%7+14] + kernel2 [(0 +10)%7+28] + kernelx0 [(0 +7)%14] + kernelx1 [(0 + 5)%7+21] + kernelx2 [(0 + 3)%7+35]; { nes_ntsc_rgb_t sub = (raw_) >> (9-(0)) & (((1L << 21) | (1 << 11) | (1 << 1)) * 3 / 2); nes_ntsc_rgb_t clamp = (((1L << 21) | (1 << 11) | (1 << 1)) * 0x101) - sub; raw_ |= clamp; clamp -= sub; raw_ &= clamp;}; { if ( BPP == 16 ) cache[0] = (raw_>>(13-0)& 0xF800)|(raw_>>(8-0)&0x07E0)|(raw_>>(4-0)&0x001F); if ( BPP == 24 || BPP == 32 ) cache[0] = (raw_>>(5-0)&0xFF0000)|(raw_>>(3-0)&0xFF00)|(raw_>>(1-0)&0xFF); if ( BPP == 15 ) cache[0] = (raw_>>(14-0)& 0x7C00)|(raw_>>(9-0)&0x03E0)|(raw_>>(4-0)&0x001F); if ( BPP == 0 ) cache[0] = raw_ << 0;};}; dst[0] = cache[0];
                    { nes_ntsc_rgb_t raw_ = kernel0 [1 ] + kernel1 [(1 +12)%7+14] + kernel2 [(1 +10)%7+28] + kernelx0 [(1 +7)%14] + kernelx1 [(1 + 5)%7+21] + kernelx2 [(1 + 3)%7+35]; { nes_ntsc_rgb_t sub = (raw_) >> (9-(0)) & (((1L << 21) | (1 << 11) | (1 << 1)) * 3 / 2); nes_ntsc_rgb_t clamp = (((1L << 21) | (1 << 11) | (1 << 1)) * 0x101) - sub; raw_ |= clamp; clamp -= sub; raw_ &= clamp;}; { if ( BPP == 16 ) cache[1] = (raw_>>(13-0)& 0xF800)|(raw_>>(8-0)&0x07E0)|(raw_>>(4-0)&0x001F); if ( BPP == 24 || BPP == 32 ) cache[1] = (raw_>>(5-0)&0xFF0000)|(raw_>>(3-0)&0xFF00)|(raw_>>(1-0)&0xFF); if ( BPP == 15 ) cache[1] = (raw_>>(14-0)& 0x7C00)|(raw_>>(9-0)&0x03E0)|(raw_>>(4-0)&0x001F); if ( BPP == 0 ) cache[1] = raw_ << 0;};}; dst[1] = cache[1];
                    
                    { unsigned color_; kernelx1 = kernel1; kernel1 = (color_ = (lut.black), (nes_ntsc_rgb_t const*) (ktable + (color_) * (nes_ntsc_entry_size * sizeof (nes_ntsc_rgb_t))));};
                    { nes_ntsc_rgb_t raw_ = kernel0 [2 ] + kernel1 [(2 +12)%7+14] + kernel2 [(2 +10)%7+28] + kernelx0 [(2 +7)%14] + kernelx1 [(2 + 5)%7+21] + kernelx2 [(2 + 3)%7+35]; { nes_ntsc_rgb_t sub = (raw_) >> (9-(0)) & (((1L << 21) | (1 << 11) | (1 << 1)) * 3 / 2); nes_ntsc_rgb_t clamp = (((1L << 21) | (1 << 11) | (1 << 1)) * 0x101) - sub; raw_ |= clamp; clamp -= sub; raw_ &= clamp;}; { if ( BPP == 16 ) cache[2] = (raw_>>(13-0)& 0xF800)|(raw_>>(8-0)&0x07E0)|(raw_>>(4-0)&0x001F); if ( BPP == 24 || BPP == 32 ) cache[2] = (raw_>>(5-0)&0xFF0000)|(raw_>>(3-0)&0xFF00)|(raw_>>(1-0)&0xFF); if ( BPP == 15 ) cache[2] = (raw_>>(14-0)& 0x7C00)|(raw_>>(9-0)&0x03E0)|(raw_>>(4-0)&0x001F); if ( BPP == 0 ) cache[2] = raw_ << 0;};}; dst[2] = cache[2];
                    { nes_ntsc_rgb_t raw_ = kernel0 [3 ] + kernel1 [(3 +12)%7+14] + kernel2 [(3 +10)%7+28] + kernelx0 [(3 +7)%14] + kernelx1 [(3 + 5)%7+21] + kernelx2 [(3 + 3)%7+35]; { nes_ntsc_rgb_t sub = (raw_) >> (9-(0)) & (((1L << 21) | (1 << 11) | (1 << 1)) * 3 / 2); nes_ntsc_rgb_t clamp = (((1L << 21) | (1 << 11) | (1 << 1)) * 0x101) - sub; raw_ |= clamp; clamp -= sub; raw_ &= clamp;}; { if ( BPP == 16 ) cache[3] = (raw_>>(13-0)& 0xF800)|(raw_>>(8-0)&0x07E0)|(raw_>>(4-0)&0x001F); if ( BPP == 24 || BPP == 32 ) cache[3] = (raw_>>(5-0)&0xFF0000)|(raw_>>(3-0)&0xFF00)|(raw_>>(1-0)&0xFF); if ( BPP == 15 ) cache[3] = (raw_>>(14-0)& 0x7C00)|(raw_>>(9-0)&0x03E0)|(raw_>>(4-0)&0x001F); if ( BPP == 0 ) cache[3] = raw_ << 0;};}; dst[3] = cache[3];
                    
                    { unsigned color_; kernelx2 = kernel2; kernel2 = (color_ = (lut.black), (nes_ntsc_rgb_t const*) (ktable + (color_) * (nes_ntsc_entry_size * sizeof (nes_ntsc_rgb_t))));};
                    { nes_ntsc_rgb_t raw_ = kernel0 [4 ] + kernel1 [(4 +12)%7+14] + kernel2 [(4 +10)%7+28] + kernelx0 [(4 +7)%14] + kernelx1 [(4 + 5)%7+21] + kernelx2 [(4 + 3)%7+35]; { nes_ntsc_rgb_t sub = (raw_) >> (9-(0)) & (((1L << 21) | (1 << 11) | (1 << 1)) * 3 / 2); nes_ntsc_rgb_t clamp = (((1L << 21) | (1 << 11) | (1 << 1)) * 0x101) - sub; raw_ |= clamp; clamp -= sub; raw_ &= clamp;}; { if ( BPP == 16 ) cache[4] = (raw_>>(13-0)& 0xF800)|(raw_>>(8-0)&0x07E0)|(raw_>>(4-0)&0x001F); if ( BPP == 24 || BPP == 32 ) cache[4] = (raw_>>(5-0)&0xFF0000)|(raw_>>(3-0)&0xFF00)|(raw_>>(1-0)&0xFF); if ( BPP == 15 ) cache[4] = (raw_>>(14-0)& 0x7C00)|(raw_>>(9-0)&0x03E0)|(raw_>>(4-0)&0x001F); if ( BPP == 0 ) cache[4] = raw_ << 0;};}; dst[4] = cache[4];
                    { nes_ntsc_rgb_t raw_ = kernel0 [5 ] + kernel1 [(5 +12)%7+14] + kernel2 [(5 +10)%7+28] + kernelx0 [(5 +7)%14] + kernelx1 [(5 + 5)%7+21] + kernelx2 [(5 + 3)%7+35]; { nes_ntsc_rgb_t sub = (raw_) >> (9-(0)) & (((1L << 21) | (1 << 11) | (1 << 1)) * 3 / 2); nes_ntsc_rgb_t clamp = (((1L << 21) | (1 << 11) | (1 << 1)) * 0x101) - sub; raw_ |= clamp; clamp -= sub; raw_ &= clamp;}; { if ( BPP == 16 ) cache[5] = (raw_>>(13-0)& 0xF800)|(raw_>>(8-0)&0x07E0)|(raw_>>(4-0)&0x001F); if ( BPP == 24 || BPP == 32 ) cache[5] = (raw_>>(5-0)&0xFF0000)|(raw_>>(3-0)&0xFF00)|(raw_>>(1-0)&0xFF); if ( BPP == 15 ) cache[5] = (raw_>>(14-0)& 0x7C00)|(raw_>>(9-0)&0x03E0)|(raw_>>(4-0)&0x001F); if ( BPP == 0 ) cache[5] = raw_ << 0;};}; dst[5] = cache[5];
                    { nes_ntsc_rgb_t raw_ = kernel0 [6 ] + kernel1 [(6 +12)%7+14] + kernel2 [(6 +10)%7+28] + kernelx0 [(6 +7)%14] + kernelx1 [(6 + 5)%7+21] + kernelx2 [(6 + 3)%7+35]; { nes_ntsc_rgb_t sub = (raw_) >> (9-(0)) & (((1L << 21) | (1 << 11) | (1 << 1)) * 3 / 2); nes_ntsc_rgb_t clamp = (((1L << 21) | (1 << 11) | (1 << 1)) * 0x101) - sub; raw_ |= clamp; clamp -= sub; raw_ &= clamp;}; { if ( BPP == 16 ) cache[6] = (raw_>>(13-0)& 0xF800)|(raw_>>(8-0)&0x07E0)|(raw_>>(4-0)&0x001F); if ( BPP == 24 || BPP == 32 ) cache[6] = (raw_>>(5-0)&0xFF0000)|(raw_>>(3-0)&0xFF00)|(raw_>>(1-0)&0xFF); if ( BPP == 15 ) cache[6] = (raw_>>(14-0)& 0x7C00)|(raw_>>(9-0)&0x03E0)|(raw_>>(4-0)&0x001F); if ( BPP == 0 ) cache[6] = raw_ << 0;};}; dst[6] = cache[6];
                    
                    dst = reinterpret_cast<Pixel*>(reinterpret_cast<byte*>(dst) + pad + 7 * sizeof(Pixel));
                    cache = buffer;
                    
                    for (dword reg=buffer[0], x=NTSC_WIDTH, scale=scanlines; x; --x)
                    {
                        const dword prefetched = *(++cache);
                        *dst++ = (scale * (reg & dword(G_MASK)) >> S_SHIFT & G_MASK) | (scale * (reg & dword(RB_MASK)) >> S_SHIFT & RB_MASK);
                        reg = prefetched;
                    }
                    
                    dst = reinterpret_cast<Pixel*>(reinterpret_cast<byte*>(dst) + pad);
                    phase = (phase + 1) % 3;
                }
            }
            
            
            
            
            
            inline uint Renderer::FilterNtsc::Lut::GetBlack(const byte (&p)[PALETTE][3])
            {
                uint index = DEF_BLACK;
                
                for (uint i=0, intensity = 0xFF * 100; i < 64; ++i)
                {
                    const uint v = p[i][0] * 30 + p[i][1] * 59 + p[i][2] * 11;
                    
                    if (intensity > v)
                    {
                        intensity = v;
                        index = i;
                    }
                }
                
                return index;
            }
            
            Renderer::FilterNtsc::Lut::Lut
            (
             const byte (&palette)[PALETTE][3],
             const schar sharpness,
             const schar resolution,
             const schar bleed,
             const schar artifacts,
             const schar fringing,
             const bool fieldMerging
             )
            :
            noFieldMerging (fieldMerging ? 0U : ~0U),
            black (GetBlack(palette))
            {
                FpuPrecision precision;
                
                nes_ntsc_setup_t setup;
                
                setup.hue = 0;
                setup.saturation = 0;
                setup.contrast = 0;
                setup.brightness = 0;
                setup.sharpness = sharpness / 100.0;
                setup.gamma = 0;
                setup.resolution = resolution / 100.0;
                setup.artifacts = artifacts / 100.0;
                setup.fringing = fringing / 100.0;
                setup.bleed = bleed / 100.0;
                setup.merge_fields = fieldMerging;
                setup.decoder_matrix = __null;
                setup.palette_out = __null;
                setup.palette = *palette;
                setup.base_palette = __null;
                
                ::nes_ntsc_init( this, &setup );
            }
            
            bool Renderer::FilterNtsc::Check(const RenderState& state)
            {
                return (state.width == NTSC_WIDTH && state.height == NTSC_HEIGHT && state.scanlines <= 100) &&
                (
                 (state.bits.count == 16 && state.bits.mask.b == 0x001F && ((state.bits.mask.g == 0x07E0 && state.bits.mask.r == 0xF800) || (state.bits.mask.g == 0x03E0 && state.bits.mask.r == 0x7C00))) ||
                 (state.bits.count == 32 && state.bits.mask.r == 0xFF0000 && state.bits.mask.g == 0x00FF00 && state.bits.mask.b == 0x0000FF)
                 );
            }
            
            Renderer::FilterNtsc::Path Renderer::FilterNtsc::GetPath(const RenderState& state)
            {
                if (state.bits.count == 32)
                {
                    return &FilterNtsc::BlitType<dword,32>;
                }
                else if (state.bits.mask.g == 0x07E0)
                {
                    return &FilterNtsc::BlitType<word,16>;
                }
                else
                {
                    return &FilterNtsc::BlitType<word,15>;
                }
            }
            
            Renderer::FilterNtsc::FilterNtsc
            (
             const RenderState& state,
             const byte (&palette)[PALETTE][3],
             schar sharpness,
             schar resolution,
             schar bleed,
             schar artifacts,
             schar fringing,
             bool fieldMerging
             )
            :
            Filter (state),
            path (GetPath(state)),
            lut (palette,sharpness,resolution,bleed,artifacts,fringing,fieldMerging),
            scanlines ((100-state.scanlines) * (state.bits.count == 32 ? 256 : 32) / 100)
            {
            }
            
            
            
            
        }
    }
}
