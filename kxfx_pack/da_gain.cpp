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
// Patch name: 'gain'

char *gain_copyright="Copyright (c) kX Project, 2003-2004.";
char *gain_engine="kX";
char *gain_comment="";
char *gain_created="02/14/2003";
char *gain_guid="79b68b0b-cd7e-4856-ba97-31189a6ae8e9";

char *gain_name="gain";
int gain_itramsize=0,gain_xtramsize=0;

dsp_register_info gain_info[]={
	{ "inl",0x4000,0x7,0xffff,0x0 },
	{ "inr",0x4001,0x7,0xffff,0x0 },
	{ "outl",0x8000,0x8,0xffff,0x0 },
	{ "outr",0x8001,0x8,0xffff,0x0 },
	{ "integer",0x8002,0x4,0xffff,0x4 },
	{ "fraction",0x8003,0x4,0xffff,0x13333333 },
	{ "tmp",0x8004,0x3,0xffff,0x0 },
};

dsp_code gain_code[]={
	{ 0x4,0x8004,0x2040,0x4000,0x8002 },
	{ 0x0,0x8000,0x8004,0x4000,0x8003 },
	{ 0x4,0x8004,0x2040,0x4001,0x8002 },
	{ 0x0,0x8001,0x8004,0x4001,0x8003 },
};

#define _INTEGER			0x8002
#define _FRACTION			0x8003
