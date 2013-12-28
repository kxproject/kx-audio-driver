// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov, Max Mikhailov, and Hanz, 2002-2004.
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
// Patch name: 'b2b'

char *b2bv2_copyright="Copyright (c) 2002-2004, Eugene Gavrilov. kX Project. All rights reserved";
char *b2bv2_engine="kX";
char *b2bv2_comment="v3.0; pre-x4 mode";
char *b2bv2_created="06/26/2003";
char *b2bv2_guid="e4c6b77f-ee7a-4df0-92ea-cac787dd223d";

char *b2bv2_name="b2b_old";
int b2bv2_itramsize=0,b2bv2_xtramsize=0;

dsp_register_info b2bv2_info[]={
	{ "inl",0x4000,0x7,0xffff,0x0 },
	{ "inr",0x4001,0x7,0xffff,0x0 },
	{ "outl",0x8000,0x8,0xffff,0x0 },
	{ "outr",0x8001,0x8,0xffff,0x0 },
	{ "_AC1008",0x8003,0x1,0xffff,0x1008 },
	{ "_AC4000",0x8004,0x1,0xffff,0x4000 },
	{ "tmp",0x8005,0x3,0xffff,0x0 },
};

dsp_code b2bv2_code[]={
	{ 0x0,0x8005,0x4000,0x2040,0x2040 },
	{ 0xf,0x2057,0x2057,0x8003,0x2041 },
	{ 0x6,0x8005,0x2040,0x8004,0x8005 },
	{ 0x6,0x8000,0x8005,0x2040,0x2040 },

	{ 0x0,0x8005,0x4001,0x2040,0x2040 },
	{ 0xf,0x2057,0x2057,0x8003,0x2041 },
	{ 0x6,0x8005,0x2040,0x8004,0x8005 },
	{ 0x6,0x8001,0x8005,0x2040,0x2040 },
};

