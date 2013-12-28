// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov, Soeren Bovbjerg 2001-2005.
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
// Patch name: 'encode4'

char *encode4_copyright="(c) Soeren Bovbjerg, 2002-2004";
char *encode4_engine="kX";
char *encode4_comment="v0.1, beta; downmixes left+right+center+subwoofer -> stereo";
char *encode4_created="2002/04/22";
char *encode4_guid="bb5186f4-6844-4169-8e49-8333b69f0020";

char *encode4_name="encode4";
int encode4_itramsize=0,encode4_xtramsize=0;

dsp_register_info encode4_info[]={
	{ "inl",0x4000,0x7,0xffff,0x0 },
	{ "inr",0x4001,0x7,0xffff,0x0 },
	{ "inc",0x4002,0x7,0xffff,0x0 },
	{ "ins",0x4003,0x7,0xffff,0x0 },
	{ "outl",0x8000,0x8,0xffff,0x0 },
	{ "outr",0x8001,0x8,0xffff,0x0 },
	{ "Center",0x8002,0x4,0xffff,0x3fffffff },
	{ "Surround",0x8003,0x4,0xffff,0x3fffffff },
	{ "lt",0x8004,0x3,0xffff,0x0 },
	{ "rt",0x8005,0x3,0xffff,0x0 },
};

dsp_code encode4_code[]={
	{ 0x0,0x8004,0x4000,0x4002,0x8002 },
	{ 0x1,0x8000,0x8004,0x4003,0x8003 },
	{ 0x0,0x8005,0x4001,0x4002,0x8002 },
	{ 0x0,0x8001,0x8005,0x4003,0x8003 },
};

