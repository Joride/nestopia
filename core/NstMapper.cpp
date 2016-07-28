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

#include <cstdio>
#include "NstLog.hpp"
#include "NstMapper.hpp"
#include "board/NstBrdMmc1.hpp"
#include "board/NstBrdMmc2.hpp"
#include "board/NstBrdMmc3.hpp"
#include "board/NstBrdMmc5.hpp"
#include "board/NstBrdVrc4.hpp"
#include "board/NstBrdVrc6.hpp"
#include "board/NstBrdVrc7.hpp"
#include "board/NstBrdN106.hpp"
#include "board/NstBrdN118.hpp"
#include "board/NstBrdFme7.hpp"
#include "board/NstBrdBandai.hpp"
#include "board/NstBrdJaleco.hpp"
#include "board/NstBrdTaitoTc.hpp"
#include "board/NstBrdTaitoX.hpp"
#include "board/NstBrdColorDreams.hpp"
#include "board/NstBrdJy.hpp"
#include "board/NstBrdFfe.hpp"
#include "board/NstBrdMmc3Waixing.hpp"
#include "board/NstBrdNina.hpp"
#include "board/NstBrdS8259.hpp"
#include "board/NstBrdSuper24In1.hpp"
#include "board/NstBrd8157.hpp"
#include "board/NstBrd8237.hpp"
#include "board/NstBrdWs.hpp"
#include "board/NstBrdDreamTech01.hpp"
#include "board/NstBrdCc21.hpp"
#include "board/NstBrdKof97.hpp"
#include "board/NstBrd64in1Nr.hpp"
#include "board/NstBrdStreetHeroes.hpp"
#include "board/NstBrdT262.hpp"
#include "board/NstBrdFk23C.hpp"
#include "board/NstBrd6035052.hpp"
#include "board/NstBrdA65AS.hpp"
#include "board/NstBrdEdu2000.hpp"
#include "board/NstBrdSubor.hpp"
#include "board/NstBrdTf1201.hpp"
#include "board/NstBrd22211.hpp"
#include "board/NstBrdGs2004.hpp"
#include "board/NstBrdAx5705.hpp"
#include "board/NstBrdT230.hpp"
#include "board/NstBrd190in1.hpp"
#include "board/NstBrdKaiser.hpp"
#include "board/NstBrdCtc65.hpp"
#include "mapper/NstMapper000.hpp"
#include "mapper/NstMapper001.hpp"
#include "mapper/NstMapper002.hpp"
#include "mapper/NstMapper003.hpp"
#include "mapper/NstMapper004.hpp"
#include "mapper/NstMapper005.hpp"
#include "mapper/NstMapper006.hpp"
#include "mapper/NstMapper007.hpp"
#include "mapper/NstMapper008.hpp"
#include "mapper/NstMapper009.hpp"
#include "mapper/NstMapper010.hpp"
#include "mapper/NstMapper011.hpp"
#include "mapper/NstMapper012.hpp"
#include "mapper/NstMapper013.hpp"
#include "mapper/NstMapper014.hpp"
#include "mapper/NstMapper015.hpp"
#include "mapper/NstMapper016.hpp"
#include "mapper/NstMapper017.hpp"
#include "mapper/NstMapper018.hpp"
#include "mapper/NstMapper019.hpp"
#include "mapper/NstMapper021.hpp"
#include "mapper/NstMapper022.hpp"
#include "mapper/NstMapper023.hpp"
#include "mapper/NstMapper024.hpp"
#include "mapper/NstMapper025.hpp"
#include "mapper/NstMapper026.hpp"
#include "mapper/NstMapper027.hpp"
#include "mapper/NstMapper032.hpp"
#include "mapper/NstMapper033.hpp"
#include "mapper/NstMapper034.hpp"
#include "mapper/NstMapper037.hpp"
#include "mapper/NstMapper038.hpp"
#include "mapper/NstMapper039.hpp"
#include "mapper/NstMapper040.hpp"
#include "mapper/NstMapper041.hpp"
#include "mapper/NstMapper042.hpp"
#include "mapper/NstMapper043.hpp"
#include "mapper/NstMapper044.hpp"
#include "mapper/NstMapper045.hpp"
#include "mapper/NstMapper046.hpp"
#include "mapper/NstMapper047.hpp"
#include "mapper/NstMapper048.hpp"
#include "mapper/NstMapper049.hpp"
#include "mapper/NstMapper050.hpp"
#include "mapper/NstMapper051.hpp"
#include "mapper/NstMapper052.hpp"
#include "mapper/NstMapper053.hpp"
#include "mapper/NstMapper054.hpp"
#include "mapper/NstMapper055.hpp"
#include "mapper/NstMapper056.hpp"
#include "mapper/NstMapper057.hpp"
#include "mapper/NstMapper058.hpp"
#include "mapper/NstMapper060.hpp"
#include "mapper/NstMapper061.hpp"
#include "mapper/NstMapper062.hpp"
#include "mapper/NstMapper063.hpp"
#include "mapper/NstMapper064.hpp"
#include "mapper/NstMapper065.hpp"
#include "mapper/NstMapper066.hpp"
#include "mapper/NstMapper067.hpp"
#include "mapper/NstMapper068.hpp"
#include "mapper/NstMapper069.hpp"
#include "mapper/NstMapper070.hpp"
#include "mapper/NstMapper071.hpp"
#include "mapper/NstMapper072.hpp"
#include "mapper/NstMapper073.hpp"
#include "mapper/NstMapper074.hpp"
#include "mapper/NstMapper075.hpp"
#include "mapper/NstMapper076.hpp"
#include "mapper/NstMapper077.hpp"
#include "mapper/NstMapper078.hpp"
#include "mapper/NstMapper079.hpp"
#include "mapper/NstMapper080.hpp"
#include "mapper/NstMapper082.hpp"
#include "mapper/NstMapper083.hpp"
#include "mapper/NstMapper085.hpp"
#include "mapper/NstMapper086.hpp"
#include "mapper/NstMapper087.hpp"
#include "mapper/NstMapper088.hpp"
#include "mapper/NstMapper089.hpp"
#include "mapper/NstMapper090.hpp"
#include "mapper/NstMapper091.hpp"
#include "mapper/NstMapper092.hpp"
#include "mapper/NstMapper093.hpp"
#include "mapper/NstMapper094.hpp"
#include "mapper/NstMapper095.hpp"
#include "mapper/NstMapper096.hpp"
#include "mapper/NstMapper097.hpp"
#include "mapper/NstMapper099.hpp"
#include "mapper/NstMapper101.hpp"
#include "mapper/NstMapper105.hpp"
#include "mapper/NstMapper107.hpp"
#include "mapper/NstMapper108.hpp"
#include "mapper/NstMapper112.hpp"
#include "mapper/NstMapper113.hpp"
#include "mapper/NstMapper114.hpp"
#include "mapper/NstMapper115.hpp"
#include "mapper/NstMapper116.hpp"
#include "mapper/NstMapper117.hpp"
#include "mapper/NstMapper118.hpp"
#include "mapper/NstMapper119.hpp"
#include "mapper/NstMapper121.hpp"
#include "mapper/NstMapper123.hpp"
#include "mapper/NstMapper132.hpp"
#include "mapper/NstMapper133.hpp"
#include "mapper/NstMapper134.hpp"
#include "mapper/NstMapper136.hpp"
#include "mapper/NstMapper137.hpp"
#include "mapper/NstMapper138.hpp"
#include "mapper/NstMapper139.hpp"
#include "mapper/NstMapper140.hpp"
#include "mapper/NstMapper141.hpp"
#include "mapper/NstMapper142.hpp"
#include "mapper/NstMapper143.hpp"
#include "mapper/NstMapper144.hpp"
#include "mapper/NstMapper145.hpp"
#include "mapper/NstMapper146.hpp"
#include "mapper/NstMapper147.hpp"
#include "mapper/NstMapper148.hpp"
#include "mapper/NstMapper149.hpp"
#include "mapper/NstMapper150.hpp"
#include "mapper/NstMapper151.hpp"
#include "mapper/NstMapper152.hpp"
#include "mapper/NstMapper153.hpp"
#include "mapper/NstMapper154.hpp"
#include "mapper/NstMapper155.hpp"
#include "mapper/NstMapper156.hpp"
#include "mapper/NstMapper157.hpp"
#include "mapper/NstMapper159.hpp"
#include "mapper/NstMapper163.hpp"
#include "mapper/NstMapper164.hpp"
#include "mapper/NstMapper165.hpp"
#include "mapper/NstMapper166.hpp"
#include "mapper/NstMapper167.hpp"
#include "mapper/NstMapper169.hpp"
#include "mapper/NstMapper170.hpp"
#include "mapper/NstMapper171.hpp"
#include "mapper/NstMapper172.hpp"
#include "mapper/NstMapper173.hpp"
#include "mapper/NstMapper178.hpp"
#include "mapper/NstMapper180.hpp"
#include "mapper/NstMapper182.hpp"
#include "mapper/NstMapper183.hpp"
#include "mapper/NstMapper184.hpp"
#include "mapper/NstMapper185.hpp"
#include "mapper/NstMapper186.hpp"
#include "mapper/NstMapper187.hpp"
#include "mapper/NstMapper188.hpp"
#include "mapper/NstMapper189.hpp"
#include "mapper/NstMapper191.hpp"
#include "mapper/NstMapper192.hpp"
#include "mapper/NstMapper193.hpp"
#include "mapper/NstMapper194.hpp"
#include "mapper/NstMapper195.hpp"
#include "mapper/NstMapper196.hpp"
#include "mapper/NstMapper197.hpp"
#include "mapper/NstMapper198.hpp"
#include "mapper/NstMapper199.hpp"
#include "mapper/NstMapper200.hpp"
#include "mapper/NstMapper201.hpp"
#include "mapper/NstMapper202.hpp"
#include "mapper/NstMapper203.hpp"
#include "mapper/NstMapper204.hpp"
#include "mapper/NstMapper205.hpp"
#include "mapper/NstMapper206.hpp"
#include "mapper/NstMapper207.hpp"
#include "mapper/NstMapper208.hpp"
#include "mapper/NstMapper209.hpp"
#include "mapper/NstMapper210.hpp"
#include "mapper/NstMapper211.hpp"
#include "mapper/NstMapper212.hpp"
#include "mapper/NstMapper213.hpp"
#include "mapper/NstMapper214.hpp"
#include "mapper/NstMapper215.hpp"
#include "mapper/NstMapper216.hpp"
#include "mapper/NstMapper217.hpp"
#include "mapper/NstMapper222.hpp"
#include "mapper/NstMapper225.hpp"
#include "mapper/NstMapper226.hpp"
#include "mapper/NstMapper227.hpp"
#include "mapper/NstMapper228.hpp"
#include "mapper/NstMapper229.hpp"
#include "mapper/NstMapper230.hpp"
#include "mapper/NstMapper231.hpp"
#include "mapper/NstMapper232.hpp"
#include "mapper/NstMapper233.hpp"
#include "mapper/NstMapper234.hpp"
#include "mapper/NstMapper235.hpp"
#include "mapper/NstMapper236.hpp"
#include "mapper/NstMapper240.hpp"
#include "mapper/NstMapper241.hpp"
#include "mapper/NstMapper242.hpp"
#include "mapper/NstMapper243.hpp"
#include "mapper/NstMapper244.hpp"
#include "mapper/NstMapper245.hpp"
#include "mapper/NstMapper246.hpp"
#include "mapper/NstMapper249.hpp"
#include "mapper/NstMapper250.hpp"
#include "mapper/NstMapper252.hpp"
#include "mapper/NstMapper254.hpp"
#include "mapper/NstMapper255.hpp"

namespace Nes
{
    namespace Core
    {
# 285 "/Users/Jorrit/iOS/nestopia/core/NstMapper.cpp"
        cstring const Mapper::boards[256+NUM_EXT_MAPPERS] =
        {
            "NROM",
            "SxROM",
            "UxROM",
            "CNROM",
            "TxROM / HxROM / DxROM / TENGEN 8000xx",
            "ExROM",
            "FFE F4xxx(128+128)/F3xxx(128+64)",
            "AxROM",
            "FFE F3xxx",
            "PxROM",
            "FxROM",
            "COLOR DREAMS",
            "BTL REX DBZ5",
            "CPROM",
            "SOMERI TEAM SL-1632",
            "BMC CONTRA 100-IN-1",
            "BANDAI FCG-1/FCG-2/LZ93D50 +X24C02",
            "FFE F8xxx/SMxxxx",
            "JALECO SS8806",
            "NAMCOT 106/163",
            "",
            "KONAMI 352889 VRC4",
            "KONAMI 35xxxx VRC2",
            "KONAMI 350926 VRC2 / 352396 VRC4",
            "KONAMI 35xxxx VRC6",
            "KONAMI 351406 VRC4",
            "KONAMI 351949 VRC6",
            "VRC4 +EXT.CHR",
            "",
            "",
            "",
            "",
            "IREM G-101",
            "TAITO TC0190/TC0350",
            "BNROM / AVE NINA-001",
            "",
            "",
            "ZZ",
            "BIT CORP 74HC161/138",
            "BMC STUDY & GAME 32-IN-1",
            "BTL SMB2",
            "CALTRON 6-IN-1",
            "BTL AI SENSHI NICOL / MARIO BABY",
            "BTL SMB2",
            "BMC SUPER BIG 7-IN-1",
            "BMC SUPER/HERO X-IN-1",
            "RUMBLE-STATION 15-IN-1",
            "QJ",
            "TAITO TC190V",
            "BMC SUPER HiK 4-IN-1",
            "BTL SMB2 (LF-36)",
            "BMC BALL GAMES 11-IN-1",
            "BMC MARIO PARTY 7-IN-1",
            "BMC SUPERVISION 16-IN-1",
            "BMC NOVELDIAMOND 9999999-IN-1",
            "BTL MARIO1-MALEE2",
            "KS-202",
            "BMC GK-54/GK-L01A/GK-L02A",
            "GK-192",
            "",
            "BMC VT5201",
            "BMC 20-IN-1",
            "BMC SUPER 700-IN-1",
            "BMC CH-001",
            "TENGEN 800032",
            "IREM H-3001",
            "GNROM / MHROM",
            "SUNSOFT3",
            "SUNSOFT4",
            "JLROM / JSROM / SUNSOFT5",
            "BANDAI 74HC161/32",
            "CAMERICA BF9093/BF9097",
            "JALECO JF-17/26/28",
            "KONAMI 350685 VRC3",
            "WAIXING MMC3 +XRAM.4K +CRAM.2K",
            "JALECO SS8805 / KONAMI VRC1",
            "NAMCOT 108 +74LS32",
            "IREM",
            "IREM 74HC161/32",
            "NINA-03 / NINA-06 / SA-009",
            "TAITO X1-005",
            "",
            "TAITO X1-017",
            "BMC DBZ 4-IN-1 / CONY",
            "",
            "KONAMI VRC7",
            "JALECO 74HC174/174/139 JF-13",
            "JALECO/KONAMI 74HC139/74",
            "NAMCOT 108/118",
            "SUNSOFT2",
            "J.Y.COMPANY +EXT.MIRR.OFF",
            "BTL SF3/MKR/SM&S2",
            "JALECO JF-19/21",
            "SUNSOFT1/SUNSOFT2 UNROM",
            "CAPCOM 74HC161/32",
            "NAMCOT 118 +A1.D5.MIRR",
            "BANDAI 74HC161/32",
            "IREM 74HC161/32",
            "",
            "VS.SYSTEM +D2.CHR/PRG",
            "",
            "JALECO 74HC139/74 JF-10",
            "",
            "",
            "",
            "EVENT",
            "",
            "MAGICSERIES",
            "WHIRLWIND MANU. 2706",
            "",
            "",
            "",
            "ASDER (NTDEC)",
            "NINA-03/NINA-06 +MIRR",
            "BTL THE LION KING",
            "KASING",
            "SOMERI TEAM SL-12",
            "FUTUREMEDIA",
            "TLSROM / TKSROM / TENGEN 800037",
            "TQROM",
            "",
            "SHIN-SHIN PANDA PRINCE",
            "",
            "H2288",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "TXC 22211",
            "SACHEN SA72008",
            "BMC FAMILY 4646B",
            "",
            "SACHEN SA-002 3011",
            "SACHEN S8259D",
            "SACHEN S8259C",
            "SACHEN S8259B",
            "JALECO 74HC174/139 JF-06/11/12",
            "SACHEN S8259A",
            "KS-7032",
            "SACHEN TCA01",
            "AGCI 50282",
            "SACHEN SA72007",
            "SACHEN SA0161M",
            "SACHEN TCU01",
            "SACHEN SA0037",
            "SACHEN SA0036",
            "SACHEN S74LS374N",
            "KONAMI VS.SYSTEM",
            "BANDAI 74HC161/161/32",
            "BANDAI +WRAM",
            "NAMCOT 118 +A0.D6.MIRR",
            "MMC1A",
            "DAOU 306",
            "BANDAI DATACH JOINT SYSTEM",
            "",
            "BANDAI LZ93D50 +X24C01",
            "",
            "",
            "",
            "NANJING",
            "WAIXING / MARS PRODUCTION",
            "WAIXING SHENGHUO HUIZHANG 2",
            "SUBOR (Russian)",
            "SUBOR (Chinese)",
            "",
            "N625092",
            "FUJIYA NROM +SECURITY",
            "KAISER KS7058",
            "IDEA-TEK 22211",
            "NEI-HU (IDEA-TEK) 22211",
            "",
            "",
            "",
            "",
            "SAN GUO ZHONG LIE ZHUAN",
            "",
            "UNROM M5",
            "",
            "HOSENKAN ELECTRONICS",
            "BTL SHUI GUAN PIPE",
            "SUNSOFT1 CNROM",
            "CNROM +CHR.DISABLE",
            "SBX",
            "BTL SFZ297/KOF96/S3DB6",
            "BANDAI KARAOKE STUDIO",
            "YOKOSOFT / TXC",
            "",
            "WAIXING MMC3 +XRAM.4K +CRAM.2K",
            "WAIXING MMC3 +XRAM.4K +CRAM.4K",
            "MEGA SOFT (NTDEC)",
            "WAIXING MMC3 +XRAM.4K +CRAM.2K (alt)",
            "WAIXING MMC3 +XRAM.4K +CRAM.4K (alt)",
            "MMC3 +A0/A2",
            "SUPER FIGHTER III",
            "WAIXING MMC3 +XRAM.4K",
            "WAIXING MMC3 +XRAM.4K +CRAM.8K",
            "BMC 1200/36-IN-1",
            "BMC 21/8-IN-1",
            "BMC 150-IN-1",
            "BMC 35-IN-1",
            "BMC 64-IN-1",
            "BMC 15/3-IN-1",
            "DE1ROM",
            "TAITO X1-005 +MIRR",
            "GOUDER 37017",
            "J.Y.COMPANY +EXT.MIRR.CTRL",
            "NAMCOT",
            "J.Y.COMPANY +EXT.MIRR.ON",
            "BMC SUPER HIK 300-IN-1",
            "BMC 9999999-IN-1",
            "BMC SUPER GUN 20-IN-1",
            "BMC SUPER 308 3-IN-1 / M-E3",
            "RCM GS-2015",
            "BMC SPC009",
            "",
            "",
            "",
            "",
            "BTL DRAGON NINJA",
            "",
            "",
            "BMC 58/64/72-IN-1",
            "BMC 76-IN-1 / SUPER 42-IN-1",
            "BMC 1200-IN-1",
            "ACTION 52",
            "BMC 31-IN-1",
            "BMC 22-IN-1",
            "BMC 20-IN-1",
            "CAMERICA BF9096",
            "BMC 20/22-IN-1",
            "AVE D-1012",
            "BMC GOLDEN GAME 150/260-IN-1",
            "BMC 800/70-IN-1",
            "",
            "",
            "",
            "SUPERTONE / C&E",
            "MXMDHTWO / TXC",
            "WAIXING ZHAN SHI",
            "SACHEN 74LS374N",
            "C&E DECATHLON",
            "WAIXING MMC3 +EX.PRG",
            "C&E PHONE SERM BERM",
            "",
            "",
            "WAIXING MMC3 +EX.PRG/CHR",
            "NITRA MMC3",
            "",
            "WAIXING SAN GUO ZHI",
            "",
            "BTL PIKACHU Y2K",
            "BMC 110/115-IN-1",
            "BMC SUPER 24-IN-1",
            "BMC 8157",
            "8237",
            "WS",
            "DREAMTECH-01",
            "CC21",
            "KOF97",
            "BMC 64-IN-1 NO REPEAT",
            "SHERO",
            "BMC T-262",
            "BMC FK23C",
            "603-5052",
            "A65AS",
            "EDU2000",
            "TF1201",
            "GS-2004",
            "AX5705",
            "T-230",
            "190IN1",
            "CTC-65",
        };
        
        Mapper* Mapper::Create(Context& context)
        {
            switch (context.id)
            {
                case 0: return new Mapper0 ( context );
                case 1: return new Mapper1 ( context );
                case 2: return new Mapper2 ( context );
                case 3: return new Mapper3 ( context );
                case 4: return new Mapper4 ( context );
                case 5: return new Mapper5 ( context );
                case 6: return new Mapper6 ( context );
                case 7: return new Mapper7 ( context );
                case 8: return new Mapper8 ( context );
                case 9: return new Mapper9 ( context );
                case 10: return new Mapper10 ( context );
                case 11: return new Mapper11 ( context );
                case 12: return new Mapper12 ( context );
                case 13: return new Mapper13 ( context );
                case 14: return new Mapper14 ( context );
                case 15: return new Mapper15 ( context );
                case 16: return new Mapper16 ( context );
                case 17: return new Mapper17 ( context );
                case 18: return new Mapper18 ( context );
                case 19: return new Mapper19 ( context );
                case 21: return new Mapper21 ( context );
                case 22: return new Mapper22 ( context );
                case 23: return new Mapper23 ( context );
                case 24: return new Mapper24 ( context );
                case 25: return new Mapper25 ( context );
                case 26: return new Mapper26 ( context );
                case 27: return new Mapper27 ( context );
                case 32: return new Mapper32 ( context );
                case 33: return new Mapper33 ( context );
                case 34: return new Mapper34 ( context );
                case 37: return new Mapper37 ( context );
                case 38: return new Mapper38 ( context );
                case 39: return new Mapper39 ( context );
                case 40: return new Mapper40 ( context );
                case 41: return new Mapper41 ( context );
                case 42: return new Mapper42 ( context );
                case 43: return new Mapper43 ( context );
                case 44: return new Mapper44 ( context );
                case 45: return new Mapper45 ( context );
                case 46: return new Mapper46 ( context );
                case 47: return new Mapper47 ( context );
                case 48: return new Mapper48 ( context );
                case 49: return new Mapper49 ( context );
                case 50: return new Mapper50 ( context );
                case 51: return new Mapper51 ( context );
                case 52: return new Mapper52 ( context );
                case 53: return new Mapper53 ( context );
                case 54: return new Mapper54 ( context );
                case 55: return new Mapper55 ( context );
                case 56: return new Mapper56 ( context );
                case 57: return new Mapper57 ( context );
                case 58: return new Mapper58 ( context );
                case 60: return new Mapper60 ( context );
                case 61: return new Mapper61 ( context );
                case 62: return new Mapper62 ( context );
                case 63: return new Mapper63 ( context );
                case 64: return new Mapper64 ( context );
                case 65: return new Mapper65 ( context );
                case 66: return new Mapper66 ( context );
                case 67: return new Mapper67 ( context );
                case 68: return new Mapper68 ( context );
                case 69: return new Mapper69 ( context );
                case 70: return new Mapper70 ( context );
                case 71: return new Mapper71 ( context );
                case 72: return new Mapper72 ( context );
                case 73: return new Mapper73 ( context );
                case 74: return new Mapper74 ( context );
                case 75: return new Mapper75 ( context );
                case 76: return new Mapper76 ( context );
                case 77: return new Mapper77 ( context );
                case 78: return new Mapper78 ( context );
                case 79: return new Mapper79 ( context );
                case 80: return new Mapper80 ( context );
                case 82: return new Mapper82 ( context );
                case 83: return new Mapper83 ( context );
                case 85: return new Mapper85 ( context );
                case 86: return new Mapper86 ( context );
                case 87: return new Mapper87 ( context );
                case 88: return new Mapper88 ( context );
                case 89: return new Mapper89 ( context );
                case 90: return new Mapper90 ( context );
                case 91: return new Mapper91 ( context );
                case 92: return new Mapper92 ( context );
                case 93: return new Mapper93 ( context );
                case 94: return new Mapper94 ( context );
                case 95: return new Mapper95 ( context );
                case 96: return new Mapper96 ( context );
                case 97: return new Mapper97 ( context );
                case 99: return new Mapper99 ( context );
                case 101: return new Mapper101 ( context );
                case 105: return new Mapper105 ( context );
                case 107: return new Mapper107 ( context );
                case 108: return new Mapper108 ( context );
                case 112: return new Mapper112 ( context );
                case 113: return new Mapper113 ( context );
                case 114: return new Mapper114 ( context );
                case 115: return new Mapper115 ( context );
                case 116: return new Mapper116 ( context );
                case 117: return new Mapper117 ( context );
                case 118: return new Mapper118 ( context );
                case 119: return new Mapper119 ( context );
                case 121: return new Mapper121 ( context );
                case 123: return new Mapper123 ( context );
                case 132: return new Mapper132 ( context );
                case 133: return new Mapper133 ( context );
                case 134: return new Mapper134 ( context );
                case 136: return new Mapper136 ( context );
                case 137: return new Mapper137 ( context );
                case 138: return new Mapper138 ( context );
                case 139: return new Mapper139 ( context );
                case 140: return new Mapper140 ( context );
                case 141: return new Mapper141 ( context );
                case 142: return new Mapper142 ( context );
                case 143: return new Mapper143 ( context );
                case 144: return new Mapper144 ( context );
                case 145: return new Mapper145 ( context );
                case 146: return new Mapper146 ( context );
                case 147: return new Mapper147 ( context );
                case 148: return new Mapper148 ( context );
                case 149: return new Mapper149 ( context );
                case 150: return new Mapper150 ( context );
                case 151: return new Mapper151 ( context );
                case 152: return new Mapper152 ( context );
                case 153: return new Mapper153 ( context );
                case 154: return new Mapper154 ( context );
                case 155: return new Mapper155 ( context );
                case 156: return new Mapper156 ( context );
                case 157: return new Mapper157 ( context );
                case 159: return new Mapper159 ( context );
                case 163: return new Mapper163 ( context );
                case 164: return new Mapper164 ( context );
                case 165: return new Mapper165 ( context );
                case 166: return new Mapper166 ( context );
                case 167: return new Mapper167 ( context );
                case 169: return new Mapper169 ( context );
                case 170: return new Mapper170 ( context );
                case 171: return new Mapper171 ( context );
                case 172: return new Mapper172 ( context );
                case 173: return new Mapper173 ( context );
                case 178: return new Mapper178 ( context );
                case 180: return new Mapper180 ( context );
                case 182: return new Mapper182 ( context );
                case 183: return new Mapper183 ( context );
                case 184: return new Mapper184 ( context );
                case 185: return new Mapper185 ( context );
                case 186: return new Mapper186 ( context );
                case 187: return new Mapper187 ( context );
                case 188: return new Mapper188 ( context );
                case 189: return new Mapper189 ( context );
                case 191: return new Mapper191 ( context );
                case 192: return new Mapper192 ( context );
                case 193: return new Mapper193 ( context );
                case 194: return new Mapper194 ( context );
                case 195: return new Mapper195 ( context );
                case 196: return new Mapper196 ( context );
                case 197: return new Mapper197 ( context );
                case 198: return new Mapper198 ( context );
                case 199: return new Mapper199 ( context );
                case 200: return new Mapper200 ( context );
                case 201: return new Mapper201 ( context );
                case 202: return new Mapper202 ( context );
                case 203: return new Mapper203 ( context );
                case 204: return new Mapper204 ( context );
                case 205: return new Mapper205 ( context );
                case 206: return new Mapper206 ( context );
                case 207: return new Mapper207 ( context );
                case 208: return new Mapper208 ( context );
                case 209: return new Mapper209 ( context );
                case 210: return new Mapper210 ( context );
                case 211: return new Mapper211 ( context );
                case 212: return new Mapper212 ( context );
                case 213: return new Mapper213 ( context );
                case 214: return new Mapper214 ( context );
                case 215: return new Mapper215 ( context );
                case 216: return new Mapper216 ( context );
                case 217: return new Mapper217 ( context );
                case 222: return new Mapper222 ( context );
                case 225: return new Mapper225 ( context );
                case 226: return new Mapper226 ( context );
                case 227: return new Mapper227 ( context );
                case 228: return new Mapper228 ( context );
                case 229: return new Mapper229 ( context );
                case 230: return new Mapper230 ( context );
                case 231: return new Mapper231 ( context );
                case 232: return new Mapper232 ( context );
                case 233: return new Mapper233 ( context );
                case 234: return new Mapper234 ( context );
                case 235: return new Mapper235 ( context );
                case 236: return new Mapper236 ( context );
                case 240: return new Mapper240 ( context );
                case 241: return new Mapper241 ( context );
                case 242: return new Mapper242 ( context );
                case 243: return new Mapper243 ( context );
                case 244: return new Mapper244 ( context );
                case 245: return new Mapper245 ( context );
                case 246: return new Mapper246 ( context );
                case 249: return new Mapper249 ( context );
                case 250: return new Mapper250 ( context );
                case 252: return new Mapper252 ( context );
                case 254: return new Mapper254 ( context );
                case 255: return new Mapper255 ( context );
                    
                case EXT_SUPER24IN1: return new Boards::Super24In1 ( context );
                case EXT_8157: return new Boards::Unl8157 ( context );
                case EXT_8237: return new Boards::Unl8237 ( context );
                case EXT_WS: return new Boards::Ws ( context );
                case EXT_DREAMTECH01: return new Boards::DreamTech01 ( context );
                case EXT_CC21: return new Boards::Cc21 ( context );
                case EXT_KOF97: return new Boards::Kof97 ( context );
                case EXT_64IN1NR: return new Boards::Bmc64In1Nr ( context );
                case EXT_STREETHEROES: return new Boards::StreetHeroes ( context );
                case EXT_T262: return new Boards::T262 ( context );
                case EXT_FK23C: return new Boards::Fk23C ( context );
                case EXT_6035052: return new Boards::Unl6035052 ( context );
                case EXT_A65AS: return new Boards::A65AS ( context );
                case EXT_EDU2000: return new Boards::Edu2000 ( context );
                case EXT_TF1201: return new Boards::Tf1201 ( context );
                case EXT_GS2004: return new Boards::Gs2004 ( context );
                case EXT_AX5705: return new Boards::Ax5705 ( context );
                case EXT_T230: return new Boards::T230 ( context );
                case EXT_190IN1: return new Boards::Bmc190in1 ( context );
                case EXT_CTC65: return new Boards::Ctc65 ( context );
                    
                default: throw RESULT_ERR_UNSUPPORTED_MAPPER;
            }
        }
        
        void Mapper::Destroy(Mapper* mapper)
        {
            delete mapper;
        }
        
        Mapper::Mapper(Context& context,const dword settings)
        :
        cpu (context.cpu),
        ppu (context.ppu),
        chr (context.ppu.GetChrMem()),
        nmt (context.ppu.GetNmtMem()),
        id (context.id),
        mirroring
        (
         (settings & NMT_SETTINGS) == NMT_VERTICAL ? Ppu::NMT_VERTICAL :
         (settings & NMT_SETTINGS) == NMT_HORIZONTAL ? Ppu::NMT_HORIZONTAL :
         (settings & NMT_SETTINGS) == NMT_ZERO ? Ppu::NMT_ZERO :
         (settings & NMT_SETTINGS) == NMT_FOURSCREEN ? Ppu::NMT_FOURSCREEN :
         context.mirroring
         )
        {
            dword size = context.prg.Size();
            
            switch (settings & PROM_SETTINGS)
            {
                case PROM_MAX_16K: size = SIZE_16K; break;
                case PROM_MAX_32K: size = SIZE_32K; break;
                case PROM_MAX_64K: size = SIZE_64K; break;
                case PROM_MAX_128K: size = SIZE_128K; break;
                case PROM_MAX_256K: size = SIZE_256K; break;
                case PROM_MAX_512K: size = SIZE_512K; break;
            }
            
            if (context.prg.Size() > size)
            {
                ((void)0);
                context.prg.Set( size );
            }
            else
            {
                context.prg.Mirror( SIZE_16K );
            }
            
            prg.Source(0).Set( context.prg.Mem(), context.prg.Size(), true, false );
            
            cstring chrRomDiscarded = __null;
            
            size = context.chr.Size();
            
            if (size)
            {
                switch (settings & CROM_SETTINGS)
                {
                    case CROM_NONE: size = 0; break;
                    case CROM_MAX_8K: size = SIZE_8K; break;
                    case CROM_MAX_16K: size = SIZE_16K; break;
                    case CROM_MAX_32K: size = SIZE_32K; break;
                    case CROM_MAX_64K: size = SIZE_64K; break;
                    case CROM_MAX_128K: size = SIZE_128K; break;
                    case CROM_MAX_256K: size = SIZE_256K; break;
                    case CROM_MAX_512K: size = SIZE_512K; break;
                }
                
                if (context.chr.Size() > size)
                {
                    ((void)0);
                    
                    if (!size)
                        chrRomDiscarded = "warning, CHR-ROM discarded!" "\n";
                    
                    context.chr.Set( size );
                }
                else
                {
                    context.chr.Mirror( SIZE_8K );
                }
            }
            
            switch (settings & CRAM_SETTINGS)
            {
                case CRAM_1K: size = SIZE_1K; break;
                case CRAM_2K: size = SIZE_2K; break;
                case CRAM_4K: size = SIZE_4K; break;
                case CRAM_8K: size = SIZE_8K; break;
                case CRAM_16K: size = SIZE_16K; break;
                case CRAM_32K: size = SIZE_32K; break;
                default: size = 0; break;
            }
            
            if (context.chr.Size())
            {
                chr.Source(0).Set( context.chr.Mem(), context.chr.Size(), true, false );
                chr.Source(1).Set( context.chr.Mem(), context.chr.Size(), true, false );
            }
            else if (size < SIZE_8K)
            {
                size = SIZE_8K;
            }
            
            context.chrRam = size;
            
            if (context.chrRam)
            {
                chr.Source( context.chr.Size() != 0 ).Set( context.chrRam, true, true );
                
                if (context.chr.Empty())
                    chr.Source(1).Set( chr.Source().Mem(), chr.Source().Size(), true, true );
            }
            
            if (mirroring == Ppu::NMT_FOURSCREEN)
                nmt.Source().Set( SIZE_4K, true, true );
            
            nmt.Source(1).Set( chr.Source().Mem(), chr.Source().Size(), true, context.chr.Size() == 0 );
            
            context.wrkAuto = false;
            
            switch (settings & WRAM_SETTINGS)
            {
                case WRAM_NONE: size = 0; break;
                case WRAM_1K: size = SIZE_1K; break;
                case WRAM_2K: size = SIZE_2K; break;
                case WRAM_4K: size = SIZE_4K; break;
                case WRAM_8K: size = SIZE_8K; break;
                case WRAM_16K: size = SIZE_16K; break;
                case WRAM_32K: size = SIZE_32K; break;
                case WRAM_40K: size = SIZE_40K; break;
                case WRAM_64K: size = SIZE_32K; break;
                    
                case WRAM_DEFAULT:
                    
                    size = context.wrk.Size() ? ((context.wrkBacked) < (SIZE_8K) ? (SIZE_8K) : (context.wrkBacked)) : 0;
                    break;
                    
                default:
                    
                    size = ((context.wrkBacked) < (SIZE_8K) ? (SIZE_8K) : (context.wrkBacked));
                    context.wrkAuto = context.wrk.Empty();
                    break;
            }
            
            context.wrk.Set( size );
            
            wrk.Source(0).Set
            (
             context.wrk.Size() ? context.wrk.Mem() : context.prg.Mem(),
             context.wrk.Size() ? context.wrk.Size() : context.prg.Size(),
             true,
             context.wrk.Size() != 0
             );
            
            wrk.Source(1).Set( prg.Source().Mem(), prg.Source().Size(), true, false );
            prg.Source(1).Set( wrk.Source().Mem(), wrk.Source().Size(), true, context.wrk.Size() != 0 );
            
            if (Log::Available())
            {
                Log log;
                
                char buffer[16];
                cstring title = buffer;
                
                if (id <= 255)
                    std::sprintf( buffer, "Mapper %u: ", uint(id) );
                else
                    title = "Mapper: ";
                
                if (*GetBoard( id ))
                    log << title << GetBoard( id ) << "\n";
                
                log << title << (prg.Source().Size() / SIZE_1K) << "k PRG-ROM" "\n"
                << title << (chr.Source().Size() / SIZE_1K) << (context.chr.Size() ? "k CHR-ROM" "\n" : "k CHR-RAM" "\n");
                
                if (chrRomDiscarded)
                    log << title << chrRomDiscarded;
                
                if (chr.Source(1).Internal())
                    log << title << (chr.Source(1).Size() / SIZE_1K) << "k CHR-RAM" "\n";
                
                if (context.wrk.Size())
                    log << title << (context.wrk.Size() / SIZE_1K) << (context.wrkAuto ? "k auto W-RAM" "\n" : "k W-RAM" "\n");
            }
        }
        
        Mapper::~Mapper()
        {
            for (uint i=0; i < Chr::NUM_SOURCES; ++i)
                chr.Source(i).Remove();
            
            for (uint i=0; i < Nmt::NUM_SOURCES; ++i)
                nmt.Source(i).Remove();
        }
        
        void Mapper::Reset(const bool hard)
        {
            cpu.Map( 0x4018, 0x5FFF ).Set( this, &Mapper::Peek_Nop, &Mapper::Poke_Nop );
            
            if (wrk.Size() >= SIZE_8K)
                cpu.Map( 0x6000, 0x7FFF ).Set( this, &Mapper::Peek_Wrk_6, &Mapper::Poke_Wrk_6 );
            else
                cpu.Map( 0x6000, 0x7FFF ).Set( this, &Mapper::Peek_Nop, &Mapper::Poke_Nop );
            
            cpu.Map( 0x8000, 0x9FFF ).Set( this, &Mapper::Peek_Prg_8, &Mapper::Poke_Nop );
            cpu.Map( 0xA000, 0xBFFF ).Set( this, &Mapper::Peek_Prg_A, &Mapper::Poke_Nop );
            cpu.Map( 0xC000, 0xDFFF ).Set( this, &Mapper::Peek_Prg_C, &Mapper::Poke_Nop );
            cpu.Map( 0xE000, 0xFFFF ).Set( this, &Mapper::Peek_Prg_E, &Mapper::Poke_Nop );
            
            if (hard)
            {
                (__builtin_expect(!(!!(id < sizeof(array(boards)))), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/NstMapper.cpp", 1003, "!!(id < sizeof(array(boards)))") : (void)0);
                
                if (chr.Source(0).Internal() || chr.Source(1).Internal())
                    chr.Source( !chr.Source(0).Internal() ).Fill( 0x00 );
                
                prg.SwapBanks<SIZE_16K,0x0000>(0U,~0U);
                chr.SwapBank<SIZE_8K,0x0000>(0);
                wrk.SwapBank<SIZE_8K,0x0000>(0);
                ppu.SetMirroring( static_cast<Ppu::Mirroring>(mirroring) );
            }
            
            SubReset( hard );
        }
        
        void Mapper::Map(WrkMapping mapping) const
        {
            switch (mapping)
            {
                case WRK_PEEK: Map( 0x6000U, 0x7FFFU, &Mapper::Peek_Wrk_6 ); break;
                case WRK_POKE: Map( 0x6000U, 0x7FFFU, &Mapper::Poke_Wrk_6 ); break;
                case WRK_PEEK_POKE: Map( 0x6000U, 0x7FFFU, &Mapper::Peek_Wrk_6, &Mapper::Poke_Wrk_6 ); break;
                case WRK_SAFE_POKE: Map( 0x6000U, 0x7FFFU, &Mapper::Poke_Wrk_Safe_6 ); break;
                case WRK_SAFE_PEEK: Map( 0x6000U, 0x7FFFU, &Mapper::Peek_Wrk_Safe_6 ); break;
                case WRK_SAFE_PEEK_POKE: Map( 0x6000U, 0x7FFFU, &Mapper::Peek_Wrk_Safe_6, &Mapper::Poke_Wrk_Safe_6 ); break;
            }
        }
        
        dword Mapper::GetStateName() const
        {
            if (id <= 255)
            {
                return AsciiId<'0','0','0'>::R( id / 100U, id % 100U / 10, id % 10U );
            }
            else
            {
                typedef char Nestopia_assertion_at_line_1060[(EXT_SUPER24IN1 == 256 && EXT_8157 == 257 && EXT_8237 == 258 && EXT_WS == 259 && EXT_DREAMTECH01 == 260 && EXT_CC21 == 261 && EXT_KOF97 == 262 && EXT_64IN1NR == 263 && EXT_STREETHEROES == 264 && EXT_T262 == 265 && EXT_FK23C == 266 && EXT_6035052 == 267 && EXT_A65AS == 268 && EXT_EDU2000 == 269 && EXT_TF1201 == 270 && EXT_GS2004 == 271 && EXT_AX5705 == 272 && EXT_T230 == 273 && EXT_190IN1 == 274 && EXT_CTC65 == 275) ? 1 : -1];
# 1062 "/Users/Jorrit/iOS/nestopia/core/NstMapper.cpp"
                static const dword chunks[] =
                {
                    AsciiId<'S','2','4'>::V,
                    AsciiId<'8','1','5'>::V,
                    AsciiId<'8','2','3'>::V,
                    AsciiId<'W','S','4'>::V,
                    AsciiId<'D','0','1'>::V,
                    AsciiId<'C','2','1'>::V,
                    AsciiId<'K','O','F'>::V,
                    AsciiId<'6','4','N'>::V,
                    AsciiId<'S','H','R'>::V,
                    AsciiId<'T','2','6'>::V,
                    AsciiId<'F','K','2'>::V,
                    AsciiId<'6','0','3'>::V,
                    AsciiId<'A','6','5'>::V,
                    AsciiId<'E','D','U'>::V,
                    AsciiId<'T','F','1'>::V,
                    AsciiId<'G','S','2'>::V,
                    AsciiId<'A','X','5'>::V,
                    AsciiId<'T','2','3'>::V,
                    AsciiId<'1','9','0'>::V,
                    AsciiId<'C','T','C'>::V
                };
                
                typedef char Nestopia_assertion_at_line_1086[(NUM_EXT_MAPPERS == sizeof(array(chunks))) ? 1 : -1];
                
                return chunks[id - 256];
            }
        }
        
        void Mapper::SaveState(State::Saver& state,const dword baseChunk) const
        {
            state.Begin( baseChunk );
            
            prg.SaveState( state, AsciiId<'P','R','G'>::V, 0x0 );
            chr.SaveState( state, AsciiId<'C','H','R'>::V, chr.Source(0).Internal() ? 0x1 : chr.Source(1).Internal() ? 0x2 : 0x0 );
            nmt.SaveState( state, AsciiId<'N','M','T'>::V, nmt.Source(0).Internal() ? 0x1 : nmt.Source(1).Internal() ? 0x2 : 0x0 );
            wrk.SaveState( state, AsciiId<'W','R','K'>::V, wrk.Available() ? 0x1 : 0x0 );
            
            BaseSave( state );
            
            state.Begin( GetStateName() );
            SubSave( state );
            state.End();
            
            state.End();
        }
        
        void Mapper::LoadState(State::Loader& state)
        {
            const dword name = GetStateName();
            
            while (const dword chunk = state.Begin())
            {
                if (chunk == name)
                {
                    SubLoad( state );
                }
                else switch (chunk)
                {
                    case AsciiId<'P','R','G'>::V:
                        
                        prg.LoadState( state, 0x0 );
                        break;
                        
                    case AsciiId<'C','H','R'>::V:
                        
                        chr.LoadState( state, chr.Source(0).Internal() ? 0x1 : chr.Source(1).Internal() ? 0x2 : 0x0 );
                        break;
                        
                    case AsciiId<'N','M','T'>::V:
                        
                        nmt.LoadState( state, nmt.Source(0).Internal() ? 0x1 : nmt.Source(1).Internal() ? 0x2 : 0x0 );
                        break;
                        
                    case AsciiId<'W','R','K'>::V:
                        
                        wrk.LoadState( state, wrk.Available() ? 0x1 : 0x0 );
                        break;
                        
                    default:
                        
                        BaseLoad( state, chunk );
                        break;
                }
                
                state.End();
            }
        }
        
        cstring Mapper::GetBoard(uint i)
        {
            return i < sizeof(array(boards)) ? boards[i] : "";
        }
        
        
        
        
        
        Data Mapper::Peek_Prg_8(void* p_,Address i_) { return static_cast<Mapper*>(p_)->Peek_M_Prg_8(i_); } inline Data Mapper::Peek_M_Prg_8(Address address) { return prg[0][address - 0x8000]; }
        Data Mapper::Peek_Prg_A(void* p_,Address i_) { return static_cast<Mapper*>(p_)->Peek_M_Prg_A(i_); } inline Data Mapper::Peek_M_Prg_A(Address address) { return prg[1][address - 0xA000]; }
        Data Mapper::Peek_Prg_C(void* p_,Address i_) { return static_cast<Mapper*>(p_)->Peek_M_Prg_C(i_); } inline Data Mapper::Peek_M_Prg_C(Address address) { return prg[2][address - 0xC000]; }
        Data Mapper::Peek_Prg_E(void* p_,Address i_) { return static_cast<Mapper*>(p_)->Peek_M_Prg_E(i_); } inline Data Mapper::Peek_M_Prg_E(Address address) { return prg[3][address - 0xE000]; }
        
        void Mapper::Poke_Prg_8k_0(void* p_,Address i_,Data j_) { static_cast<Mapper*>(p_)->Poke_M_Prg_8k_0(i_,j_); } inline void Mapper::Poke_M_Prg_8k_0(Address,Data data) { prg.SwapBank<SIZE_8K,0x0000>( data ); }
        void Mapper::Poke_Prg_8k_1(void* p_,Address i_,Data j_) { static_cast<Mapper*>(p_)->Poke_M_Prg_8k_1(i_,j_); } inline void Mapper::Poke_M_Prg_8k_1(Address,Data data) { prg.SwapBank<SIZE_8K,0x2000>( data ); }
        void Mapper::Poke_Prg_8k_2(void* p_,Address i_,Data j_) { static_cast<Mapper*>(p_)->Poke_M_Prg_8k_2(i_,j_); } inline void Mapper::Poke_M_Prg_8k_2(Address,Data data) { prg.SwapBank<SIZE_8K,0x4000>( data ); }
        void Mapper::Poke_Prg_8k_3(void* p_,Address i_,Data j_) { static_cast<Mapper*>(p_)->Poke_M_Prg_8k_3(i_,j_); } inline void Mapper::Poke_M_Prg_8k_3(Address,Data data) { prg.SwapBank<SIZE_8K,0x6000>( data ); }
        void Mapper::Poke_Prg_16k_0(void* p_,Address i_,Data j_) { static_cast<Mapper*>(p_)->Poke_M_Prg_16k_0(i_,j_); } inline void Mapper::Poke_M_Prg_16k_0(Address,Data data) { prg.SwapBank<SIZE_16K,0x0000>( data ); }
        void Mapper::Poke_Prg_16k_1(void* p_,Address i_,Data j_) { static_cast<Mapper*>(p_)->Poke_M_Prg_16k_1(i_,j_); } inline void Mapper::Poke_M_Prg_16k_1(Address,Data data) { prg.SwapBank<SIZE_16K,0x4000>( data ); }
        void Mapper::Poke_Prg_32k(void* p_,Address i_,Data j_) { static_cast<Mapper*>(p_)->Poke_M_Prg_32k(i_,j_); } inline void Mapper::Poke_M_Prg_32k(Address,Data data) { prg.SwapBank<SIZE_32K,0x0000>( data ); }
        
        void Mapper::Poke_Chr_1k_0(void* p_,Address i_,Data j_) { static_cast<Mapper*>(p_)->Poke_M_Chr_1k_0(i_,j_); } inline void Mapper::Poke_M_Chr_1k_0(Address,Data data) { ppu.Update(); chr.SwapBank<SIZE_1K,0x0000>( data ); }
        void Mapper::Poke_Chr_1k_1(void* p_,Address i_,Data j_) { static_cast<Mapper*>(p_)->Poke_M_Chr_1k_1(i_,j_); } inline void Mapper::Poke_M_Chr_1k_1(Address,Data data) { ppu.Update(); chr.SwapBank<SIZE_1K,0x0400>( data ); }
        void Mapper::Poke_Chr_1k_2(void* p_,Address i_,Data j_) { static_cast<Mapper*>(p_)->Poke_M_Chr_1k_2(i_,j_); } inline void Mapper::Poke_M_Chr_1k_2(Address,Data data) { ppu.Update(); chr.SwapBank<SIZE_1K,0x0800>( data ); }
        void Mapper::Poke_Chr_1k_3(void* p_,Address i_,Data j_) { static_cast<Mapper*>(p_)->Poke_M_Chr_1k_3(i_,j_); } inline void Mapper::Poke_M_Chr_1k_3(Address,Data data) { ppu.Update(); chr.SwapBank<SIZE_1K,0x0C00>( data ); }
        void Mapper::Poke_Chr_1k_4(void* p_,Address i_,Data j_) { static_cast<Mapper*>(p_)->Poke_M_Chr_1k_4(i_,j_); } inline void Mapper::Poke_M_Chr_1k_4(Address,Data data) { ppu.Update(); chr.SwapBank<SIZE_1K,0x1000>( data ); }
        void Mapper::Poke_Chr_1k_5(void* p_,Address i_,Data j_) { static_cast<Mapper*>(p_)->Poke_M_Chr_1k_5(i_,j_); } inline void Mapper::Poke_M_Chr_1k_5(Address,Data data) { ppu.Update(); chr.SwapBank<SIZE_1K,0x1400>( data ); }
        void Mapper::Poke_Chr_1k_6(void* p_,Address i_,Data j_) { static_cast<Mapper*>(p_)->Poke_M_Chr_1k_6(i_,j_); } inline void Mapper::Poke_M_Chr_1k_6(Address,Data data) { ppu.Update(); chr.SwapBank<SIZE_1K,0x1800>( data ); }
        void Mapper::Poke_Chr_1k_7(void* p_,Address i_,Data j_) { static_cast<Mapper*>(p_)->Poke_M_Chr_1k_7(i_,j_); } inline void Mapper::Poke_M_Chr_1k_7(Address,Data data) { ppu.Update(); chr.SwapBank<SIZE_1K,0x1C00>( data ); }
        void Mapper::Poke_Chr_2k_0(void* p_,Address i_,Data j_) { static_cast<Mapper*>(p_)->Poke_M_Chr_2k_0(i_,j_); } inline void Mapper::Poke_M_Chr_2k_0(Address,Data data) { ppu.Update(); chr.SwapBank<SIZE_2K,0x0000>( data ); }
        void Mapper::Poke_Chr_2k_1(void* p_,Address i_,Data j_) { static_cast<Mapper*>(p_)->Poke_M_Chr_2k_1(i_,j_); } inline void Mapper::Poke_M_Chr_2k_1(Address,Data data) { ppu.Update(); chr.SwapBank<SIZE_2K,0x0800>( data ); }
        void Mapper::Poke_Chr_2k_2(void* p_,Address i_,Data j_) { static_cast<Mapper*>(p_)->Poke_M_Chr_2k_2(i_,j_); } inline void Mapper::Poke_M_Chr_2k_2(Address,Data data) { ppu.Update(); chr.SwapBank<SIZE_2K,0x1000>( data ); }
        void Mapper::Poke_Chr_2k_3(void* p_,Address i_,Data j_) { static_cast<Mapper*>(p_)->Poke_M_Chr_2k_3(i_,j_); } inline void Mapper::Poke_M_Chr_2k_3(Address,Data data) { ppu.Update(); chr.SwapBank<SIZE_2K,0x1800>( data ); }
        void Mapper::Poke_Chr_4k_0(void* p_,Address i_,Data j_) { static_cast<Mapper*>(p_)->Poke_M_Chr_4k_0(i_,j_); } inline void Mapper::Poke_M_Chr_4k_0(Address,Data data) { ppu.Update(); chr.SwapBank<SIZE_4K,0x0000>( data ); }
        void Mapper::Poke_Chr_4k_1(void* p_,Address i_,Data j_) { static_cast<Mapper*>(p_)->Poke_M_Chr_4k_1(i_,j_); } inline void Mapper::Poke_M_Chr_4k_1(Address,Data data) { ppu.Update(); chr.SwapBank<SIZE_4K,0x1000>( data ); }
        void Mapper::Poke_Chr_8k(void* p_,Address i_,Data j_) { static_cast<Mapper*>(p_)->Poke_M_Chr_8k(i_,j_); } inline void Mapper::Poke_M_Chr_8k(Address,Data data) { ppu.Update(); chr.SwapBank<SIZE_8K,0x0000>( data ); }
        
        void Mapper::Poke_Wrk_6(void* p_,Address i_,Data j_)
        {
            printf("%s", __FUNCTION__);
            static_cast<Mapper*>(p_)->Poke_M_Wrk_6(i_,j_);
        }
        
        inline void Mapper::Poke_M_Wrk_6(Address address,Data data)
        {
            printf("%s", __FUNCTION__);
            wrk[0][address - 0x6000] = data;
        }
        
        Data Mapper::Peek_Wrk_6(void* p_,Address i_) { return static_cast<Mapper*>(p_)->Peek_M_Wrk_6(i_); } inline Data Mapper::Peek_M_Wrk_6(Address address)
        {
            ((void)0);
            return wrk[0][address - 0x6000];
        }
        
        void Mapper::Poke_Wrk_Safe_6(void* p_,Address i_,Data j_) { static_cast<Mapper*>(p_)->Poke_M_Wrk_Safe_6(i_,j_); } inline void Mapper::Poke_M_Wrk_Safe_6(Address address,Data data)
        {
            ((void)0);
            
            if (wrk.Writable(0))
                wrk[0][address - 0x6000] = data;
        }
        
        Data Mapper::Peek_Wrk_Safe_6(void* p_,Address i_) { return static_cast<Mapper*>(p_)->Peek_M_Wrk_Safe_6(i_); } inline Data Mapper::Peek_M_Wrk_Safe_6(Address address)
        {
            ((void)0);
            return wrk.Readable(0) ? wrk[0][address - 0x6000] : (address >> 8);
        }
        
        void Mapper::Poke_Nmt_Hv(void* p_,Address i_,Data j_) { static_cast<Mapper*>(p_)->Poke_M_Nmt_Hv(i_,j_); } inline void Mapper::Poke_M_Nmt_Hv(Address,Data data)
        {
            typedef char Nestopia_assertion_at_line_1218[(Ppu::NMT_HORIZONTAL == 0 && Ppu::NMT_VERTICAL == 1) ? 1 : -1];
            ((void)0);
            
            ppu.SetMirroring( static_cast<Ppu::Mirroring>(~data & 0x1) );
        }
        
        void Mapper::Poke_Nmt_Vh(void* p_,Address i_,Data j_) { static_cast<Mapper*>(p_)->Poke_M_Nmt_Vh(i_,j_); } inline void Mapper::Poke_M_Nmt_Vh(Address,Data data)
        {
            typedef char Nestopia_assertion_at_line_1226[(Ppu::NMT_HORIZONTAL == 0 && Ppu::NMT_VERTICAL == 1) ? 1 : -1];
            ((void)0);
            
            ppu.SetMirroring( static_cast<Ppu::Mirroring>(data & 0x1) );
        }
        
        void Mapper::Poke_Nmt_Vh01(void* p_,Address i_,Data j_) { static_cast<Mapper*>(p_)->Poke_M_Nmt_Vh01(i_,j_); } inline void Mapper::Poke_M_Nmt_Vh01(Address,Data data)
        {
            static const byte lut[4][4] =
            {
                {0,1,0,1},
                {0,0,1,1},
                {0,0,0,0},
                {1,1,1,1}
            };
            
            ppu.SetMirroring( lut[data & 0x3] );
        }
        
        void Mapper::Poke_Nmt_Hv01(void* p_,Address i_,Data j_) { static_cast<Mapper*>(p_)->Poke_M_Nmt_Hv01(i_,j_); } inline void Mapper::Poke_M_Nmt_Hv01(Address,Data data)
        {
            static const byte lut[4][4] =
            {
                {0,0,1,1},
                {0,1,0,1},
                {0,0,0,0},
                {1,1,1,1}
            };
            
            ppu.SetMirroring( lut[data & 0x3] );
        }
        
        void Mapper::Poke_Nop(void* p_,Address i_,Data j_) { static_cast<Mapper*>(p_)->Poke_M_Nop(i_,j_); } inline void Mapper::Poke_M_Nop(Address,Data)
        {
            ((void)0);
        }
        
        Data Mapper::Peek_Nop(void* p_,Address i_) { return static_cast<Mapper*>(p_)->Peek_M_Nop(i_); } inline Data Mapper::Peek_M_Nop(Address address)
        {
            ((void)0);
            return address >> 8;
        }
    }
}
