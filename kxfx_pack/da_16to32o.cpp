// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov, 2002-2004.
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
// Patch name: 'do16to32'

char *do16to32_copyright="Copyright (c) Eugene Gavrilov, 2003-2004. All rights reserved";
char *do16to32_engine="kX";
char *do16to32_comment="Patents pending";
char *do16to32_created="05/09/2003";
char *do16to32_guid="3a5ee31e-9acb-4d57-b0cc-367396ce621f";

char *do16to32_name="16to32";
int do16to32_itramsize=0,do16to32_xtramsize=0;

dsp_register_info do16to32_info[]={
	{ "high",0x4000,0x7,0xffff,0x0 },
	{ "low",0x4001,0x7,0xffff,0x0 },
	{ "out",0x8000,0x8,0xffff,0x0 },
	{ "_AC1008",0x8001,0x1,0xffff,0x1008 },
	{ "_AC4000",0x8002,0x1,0xffff,0x4000 },
	{ "_ACffffc000",0x8003,0x1,0xffff,0xffffc000 },
	{ "tmp_h",0x8004,0x3,0xffff,0x0 },
	{ "tmp_l",0x8005,0x3,0xffff,0x0 },
};

dsp_code do16to32_code[]={
	{ 0x0,0x8004,0x4000,0x2040,0x2040 },
	{ 0xf,0x2057,0x2057,0x8001,0x2041 },
	{ 0x6,0x8004,0x2040,0x8002,0x8004 },
	{ 0x0,0x8005,0x2040,0x4001,0x2049 },
	{ 0x8,0x8000,0x8004,0x8003,0x8005 },
};

