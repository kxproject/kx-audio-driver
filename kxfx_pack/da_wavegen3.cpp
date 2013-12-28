// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov and eYagos, 2003-2004.
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
// Patch name: 'wavegen3'

char *wavegen3_copyright="by eYagos - Copyright (c) 2003-2004. All rights reserved";
char *wavegen3_engine="kX";
char *wavegen3_comment="Wave Generator 3.0: Sine, Square, Triangle, Sawtooth and White Noise. Not intended for making music";
char *wavegen3_created="07/23/2003";
char *wavegen3_guid="e6580857-eb69-45a6-bb78-9ecf4f74eeb3";

char *wavegen3_name="Wave Generator 3.0";
int wavegen3_itramsize=0,wavegen3_xtramsize=0;

#define		R_Level	0x8005
#define		R_F		0x8006
#define		R_Fs	0x8007
#define		R_S0	0x8008
#define		R_S1	0x8009

dsp_register_info wavegen3_info[]={
	{ "Sinus",0x8000,0x8,0xffff,0x0 },
	{ "Sawtooth",0x8001,0x8,0xffff,0x0 },
	{ "Triangular",0x8002,0x8,0xffff,0x0 },
	{ "Square",0x8003,0x8,0xffff,0x0 },
	{ "Blank_Noise",0x8004,0x8,0xffff,0x0 },
	{ "LEVEL",0x8005,0x1,0xffff,0x1fffffff },
	{ "f",0x8006,0x1,0xffff,0x7ee7aa4a },
	{ "fs",0x8007,0x1,0xffff,0x5555555 },
	{ "s0",0x8008,0x1,0xffff,0xef4aeaf0 },
	{ "s1",0x8009,0x1,0xffff,0x0 },
	{ "saw",0x800a,0x3,0xffff,0x0 },
	{ "tmp",0x800b,0x3,0xffff,0x0 },
};

dsp_code wavegen3_code[]={
	{ 0x0,0x800b,0x2040,0x8006,0x8009 },
	{ 0x1,0x800b,0x800b,0x8008,0x204d },
	{ 0x4,0x800b,0x2040,0x800b,0x2042 },
	{ 0x0,0x8008,0x8009,0x2040,0x2040 },
	{ 0x0,0x8009,0x800b,0x2040,0x2040 },
	{ 0x0,0x8000,0x2040,0x800b,0x8005 },
	{ 0x3,0x800a,0x800a,0x8007,0x204f },
	{ 0x0,0x8001,0x2040,0x800a,0x8005 },
	{ 0x9,0x800b,0x800a,0x800a,0x2040 },
	{ 0x1,0x800b,0x800b,0x204d,0x204f },
	{ 0x4,0x800b,0x2040,0x800b,0x2042 },
	{ 0x0,0x8002,0x2040,0x800b,0x8005 },
	{ 0x9,0x800b,0x800a,0x204f,0x2040 },
	{ 0x0,0x8003,0x2040,0x800b,0x8005 },
	{ 0x4,0x800b,0x2040,0x2058,0x2042 },
	{ 0x0,0x8004,0x2040,0x800b,0x8005 },
};