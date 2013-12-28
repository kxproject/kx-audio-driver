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

// 10kX microcode
// Patch name: 'crossfade'

char *crossfade_copyright="Copyright (c) kX Project, 2003-2004. All rights reserved";
char *crossfade_engine="kX";
char *crossfade_comment="";
char *crossfade_created="03/14/2003";
char *crossfade_guid="2f0ef3c1-87cd-4469-9d05-caa78687c703";

char *crossfade_name="crossfade";
int crossfade_itramsize=0,crossfade_xtramsize=0;

dsp_register_info crossfade_info[]={
	{ "i1L",0x4000,0x7,0xffff,0x0 },
	{ "i1R",0x4001,0x7,0xffff,0x0 },
	{ "i2L",0x4002,0x7,0xffff,0x0 },
	{ "i2R",0x4003,0x7,0xffff,0x0 },
	{ "L",0x8000,0x8,0xffff,0x0 },
	{ "R",0x8001,0x8,0xffff,0x0 },
	{ "k1",0x8002,0x1,0xffff,0x7fffffff },
	{ "k2",0x8003,0x1,0xffff,0x7fffffff },
};

dsp_code crossfade_code[]={
	{ 0x0,0x8000,0x2040,0x4000,0x8002 },
	{ 0x0,0x8000,0x8000,0x4002,0x8003 },
	{ 0x0,0x8001,0x2040,0x4001,0x8002 },
	{ 0x0,0x8001,0x8001,0x4003,0x8003 },
};

#define R_K1	0x8002u
#define R_K2	0x8003u

