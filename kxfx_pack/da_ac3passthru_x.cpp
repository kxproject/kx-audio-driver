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
// Patch name: 'ac3passthru_x'

char *ac3passthru_x_copyright="Copyright (c) Eugene Gavrilov and Max Mikhailov, 2004. All rights reserved";
char *ac3passthru_x_engine="kX";
char *ac3passthru_x_comment="";
char *ac3passthru_x_created="01/06/2004";
char *ac3passthru_x_guid="64824522-f847-4bca-ac45-7a58c321d4e3";

char *ac3passthru_x_name="ac3passthru_x";
int ac3passthru_x_itramsize=0,ac3passthru_x_xtramsize=6144;

dsp_register_info ac3passthru_x_info[]={
	{ "out_l",0x8000,0x8,0xffff,0x0 },
	{ "out_r",0x8001,0x8,0xffff,0x0 },
	{ "counter",0x8002,0x1,0xffff,0x0 },
	{ "dbac_value",0x8003,0x1,0xffff,0x0 },
	{ "buf_size",0x8004,0x1,0xffff,0x5ff },
	{ "_ACfffff000",0x8008,0x1,0xffff,0xfffff000 },
	{ "_AC7",0x8009,0x1,0xffff,0x7 },
	{ "_ACffff0000",0x800a,0x1,0xffff,0xffff0000 },
	{ "t",0x800b,0x3,0xffff,0x0 },
	{ "buf_l",0x800c,0x2a,0xffff,0x0 },
	{ "&buf_l",0x800d,0xb,0xffff,0x0 },
	{ "buf_r",0x800e,0x2a,0xffff,0xc00 },
	{ "&buf_r",0x800f,0xb,0xffff,0xc00 },
};

dsp_code ac3passthru_x_code[]={
	{ 0x1,0x8002,0x8002,0x204e,0x2041 },
	{ 0xb,0x8002,0x8004,0x2040,0x8002 },
	{ 0xf,0x2057,0x2057,0x2048,0x2042 },
	{ 0x0,0x8003,0x205b,0x800d,0x2055 },
	{ 0x0,0x205a,0x2040,0x2040,0x2040 }, // turns on/off 0x204e
	{ 0x8,0x800b,0x800c,0x8008,0x2040 },
	{ 0xc,0x800b,0x800b,0x8009,0x2040 },
	{ 0x8,0x8000,0x800b,0x800a,0x2040 },
	{ 0x8,0x800b,0x800e,0x8008,0x2040 },
	{ 0xc,0x800b,0x800b,0x8009,0x2040 },
	{ 0x8,0x8001,0x800b,0x800a,0x2040 },
};

