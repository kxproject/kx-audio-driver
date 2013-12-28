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
// Patch name: 'Peak'

const char *peak_copyright="Copyright (c) Eugene Gavrilov, 2002-2004. All rights reserved";
const char *peak_engine="kX";
const char *peak_comment="$nobypass";
const char *peak_created="07/14/2002; Sep 2003";
const char *peak_guid="cb8abf40-a6b2-417f-aaa5-3402466e40e8";

const char *peak_name="Peak";
int peak_itramsize=0,peak_xtramsize=0;

dsp_register_info peak_info[]={
	{ "peak_l",0x4000,0x7,0xffff,0x0 },
	{ "peak_r",0x4001,0x7,0xffff,0x0 },
	{ "result_l",0x8000,0x1,0xffff,0x0 },
	{ "result_r",0x8001,0x1,0xffff,0x0 },
	{ "resolution",0x8002,0x1,0xffff,0x1f },
	{ "tmp",0x8003,0x3,0xffff,0x0 },
};

dsp_code peak_code[]={
	{ 0xc,0x8003,0x4000,0x8002,0x2041 },
	{ 0xa,0x8000,0x8003,0x8003,0x8000 },
	{ 0xc,0x8003,0x4001,0x8002,0x2041 },
	{ 0xa,0x8001,0x8003,0x8003,0x8001 }/*,
	{ 0xa,0x8000,0x4000,0x4000,0x8000 },
	{ 0xa,0x8001,0x4001,0x4001,0x8001 }*/
};
