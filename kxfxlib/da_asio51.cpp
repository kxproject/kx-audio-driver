// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov and LeMury, 2003-2005.
// All rights reserved

/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

// 10kX microcode
// Patch name: 'asio'

// This mirco code is published if a 10k1 5.1 card is detected.
// It deals with the re-mapping problem/conflict with Center/LFE output.
// All ASIO inpus are re-mapped to correspond correctly with ASIO Apps.

const char *asio51_copyright="Copyright (c) LeMury, 2003-2005. All rights reserved.";
const char *asio51_engine="kX";
const char *asio51_comment="ASIO Recording Inputs v1.1; $nobypass";
const char *asio51_created="11/06/2003";
const char *asio51_guid="49b4ecb9-e43e-4789-9ee5-c68a970237e3";

const char *asio51_name="Asio";
int asio51_itramsize=0,asio51_xtramsize=0;


#define _CODE		//offset

dsp_register_info asio51_info[]={
	{ "asio0",0x4000,0x7,0xffff,0x0 },
	{ "asio1",0x4001,0x7,0xffff,0x0 },
	{ "asio2",0x4002,0x7,0xffff,0x0 },
	// asio3/4 is mapped to by Center/Lfe
	{ "asio5",0x4003,0x7,0xffff,0x0 },
	{ "asio6",0x4004,0x7,0xffff,0x0 },
	{ "asio7",0x4005,0x7,0xffff,0x0 },
	{ "asio8",0x4006,0x7,0xffff,0x0 },
	{ "asio9",0x4007,0x7,0xffff,0x0 },
	{ "asio10",0x4008,0x7,0xffff,0x0 },
	{ "asio11",0x4009,0x7,0xffff,0x0 },
	{ "asio12",0x400a,0x7,0xffff,0x0 },
	{ "asio13",0x400b,0x7,0xffff,0x0 },
	{ "asio14",0x400c,0x7,0xffff,0x0 },
	{ "asio15",0x400d,0x7,0xffff,0x0 },
};


dsp_code asio51_code[]={
	{ 0x4,0x250e,0x2040,0x4000,0x2044 }, //asio0
	{ 0x4,0x250f,0x2040,0x4001,0x2044 },
	{ 0x4,0x2500,0x2040,0x4002,0x2044 },
	{ 0x4,0x2503,0x2040,0x4003,0x2044 },
	{ 0x4,0x2504,0x2040,0x4004,0x2044 },
	{ 0x4,0x2505,0x2040,0x4005,0x2044 },
	{ 0x4,0x2506,0x2040,0x4006,0x2044 },
	{ 0x4,0x2507,0x2040,0x4007,0x2044 },
	{ 0x4,0x2508,0x2040,0x4008,0x2044 },
	{ 0x4,0x2509,0x2040,0x4009,0x2044 },
	{ 0x4,0x250a,0x2040,0x400a,0x2044 },
	{ 0x4,0x250b,0x2040,0x400b,0x2044 },
	{ 0x4,0x250c,0x2040,0x400c,0x2044 },
	{ 0x4,0x250d,0x2040,0x400d,0x2044 }, //asio15
};

/*ASIO mapping On 10k1 5.1 SBLive cards:
--------------------------------------------
Cubase SX	Epilog		KX_NR(nr)
--------------------------------------------
IN1		asio14		KX_FX2(0xe)
IN2		asio15		KX_FX2(0xf)
IN3		asio0		KX_FX2(0x0)	
~IN4		Center		KX_OUT(0x11)	// mapped to by Center
~IN5		LFE			KX_OUT(0x12)	// mapped to by LFE
IN6		asio3		KX_FX2(0x3)
IN7		asio4		KX_FX2(0x4)
IN8		asio5		KX_FX2(0x5)
IN9		asio6		KX_FX2(0x6)
IN10	asio7		KX_FX2(0x7)
IN11	asio8		KX_FX2(0x8)
IN12	asio9		KX_FX2(0x9)
IN13	asio10		KX_FX2(0xa)
IN14	asio11		KX_FX2(0xb)
IN15	asio12		KX_FX2(0xc)
IN16	asio13		KX_FX2(0xd)
*/
