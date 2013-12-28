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
// Patch name: 'decimator'

char *decimator_copyright="Copyright (c) David Descheneau, 2003.";
char *decimator_engine="kX";
char *decimator_comment="";
char *decimator_created="02/11/2003";
char *decimator_guid="46c99605-edc7-4443-a2b2-f9d451c19483";

char *decimator_name="Stereo Decimator";
int decimator_itramsize=0,decimator_xtramsize=0;

dsp_register_info decimator_info[]={
	{ "lin",0x4000,0x7,0xffff,0x0 },
	{ "rin",0x4001,0x7,0xffff,0x0 },
	{ "lout",0x8000,0x8,0xffff,0x0 },
	{ "rout",0x8001,0x8,0xffff,0x0 },
	{ "mask",0x8002,0x4,0xffff,0xffffffff },
	{ "sr",0x8003,0x4,0xffff,0x7fffffff },
	{ "count",0x8004,0x1,0xffff,0x80000000 },
	{ "holdl",0x8005,0x1,0xffff,0x0 },
	{ "holdr",0x8006,0x1,0xffff,0x0 },
};

dsp_code decimator_code[]={
	{ 0x6,0x8004,0x8004,0x8003,0x2040 },
	{ 0xf,0x2040,0x2057,0x2044,0x2043 },
	{ 0x8,0x8005,0x4000,0x8002,0x2040 },
	{ 0x8,0x8006,0x4001,0x8002,0x2040 },
	{ 0x6,0x8004,0x8004,0x204e,0x2040 },
	{ 0x0,0x8000,0x8005,0x2040,0x2040 },
	{ 0x0,0x8001,0x8006,0x2040,0x2040 },
};

#define _BITS			0x8002
#define _SRATE			0x8003
