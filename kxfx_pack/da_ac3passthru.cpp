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
// Patch name: 'ac3passthru'

char *ac3passthru_copyright="Copyright (c) Eugene Gavrilov, 2003-2004.";
char *ac3passthru_engine="kX";
char *ac3passthru_comment="v2.0; includes x4";
char *ac3passthru_created="05/11/2003";
char *ac3passthru_guid="459fd76d-a110-438e-908f-fcd1872d5a06";

char *ac3passthru_name="ac3passthru_old";
int ac3passthru_itramsize=0,ac3passthru_xtramsize=0;

dsp_register_info ac3passthru_info[]={ 
	{ "inl",0x4000,0x7,0xffff,0x0 },
	{ "inr",0x4001,0x7,0xffff,0x0 },
	{ "outl",0x8000,0x8,0xffff,0x0 },
	{ "outr",0x8003,0x8,0xffff,0x0 },
	{ "ac1008",0x8002,0x1,0xffff,0x1008 },
	{ "tmpl",0x8000,0x3,0xffff,0x0 },
	{ "tmpr",0x8001,0x3,0xffff,0x0 },
};

dsp_code ac3passthru_code[]={
	{ MACINTS,0x8000,0x2040,0x4000,0x2044 },
	{ SKIP,0x2057,0x2057,0x8002,0x2041 },
	{ ACC3,0x8000,0x2040,0x2049,0x8000 },
	{ MACINTS,0x8001,0x2040,0x4001,0x2044 },
	{ SKIP,0x2057,0x2057,0x8002,0x2041 },
	{ ACC3,0x8001,0x2040,0x2049,0x8001 },
	{ MACS,0x8000,0x8000,0x2040,0x2040 },
	{ MACS,0x8003,0x8001,0x2040,0x2040 }
};

