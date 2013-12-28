// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov, 2001-2005.
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

// kX DSP Generated file

// 10kX microcode
// Patch name: 'overdrive2'

char *overdrive2_copyright="by eYagos";
char *overdrive2_engine="kX";
char *overdrive2_comment="simple overdrive";
char *overdrive2_created="March 25 2002";
char *overdrive2_guid="3753C6BA-FA53-4e79-A35C-A3B9F1EF3D45";

char *overdrive2_name="Overdrive2";
int overdrive2_itramsize=0,overdrive2_xtramsize=0;

dsp_register_info overdrive2_info[]={
	{ "in",0x4000,0x7,0xffff,0x0 },
	{ "out",0x8000,0x8,0xffff,0x0 },
	{ "gain1",0x8001,0x4,0xffff,0x7fffffff },
	{ "gain2",0x8002,0x4,0xffff,0xffff0000 },
	{ "C_0",0x8003,0x1,0xffff,0x0 },
	{ "C_1",0x8004,0x1,0xffff,0x7fffffff },
	{ "gain",0x8005,0x3,0xffff,0x0 },
	{ "sign",0x8006,0x3,0xffff,0x0 },
	{ "in_abs",0x8007,0x3,0xffff,0x0 },
};

dsp_code overdrive2_code[]={
	{ 0x9,0x8006,0x4000,0x8004,0x8003 },
	{ 0x0,0x8007,0x8003,0x4000,0x8006 },
	{ 0xe,0x8005,0x8001,0x8007,0x8002 },
	{ 0x0,0x8000,0x2040,0x4000,0x8005 },
};

