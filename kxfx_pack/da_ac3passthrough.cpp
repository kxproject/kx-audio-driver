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
// Patch name: 'ac3passthrough'

char *ac3passthrough_copyright="Copyright (c) 2002-2004, kX Project.";
char *ac3passthrough_engine="kX";
char *ac3passthrough_comment="v2.0; includes x4";
char *ac3passthrough_created="10/08/2002";
char *ac3passthrough_guid="9812dce7-5738-48aa-b700-927caec84053";

char *ac3passthrough_name="ac3passthrough";
int ac3passthrough_itramsize=0,ac3passthrough_xtramsize=0;

dsp_register_info ac3passthrough_info[]={
	{ "inl",0x4000,0x7,0xffff,0x0 },
	{ "inr",0x4001,0x7,0xffff,0x0 },
	{ "outl",0x8000,0x8,0xffff,0x0 },
	{ "outr",0x8003,0x8,0xffff,0x0 },
	{ "_AC4000",0x8001,0x1,0xffff,0x4000 },
	{ "t",0x8002,0x3,0xffff,0x0 },
};

dsp_code ac3passthrough_code[]={
	{ 0x0,0x8002,0x8001,0x4000,0x204f },
	{ 0xa,0x8002,0x8001,0x4000,0x8002 },
	{ 0x4,0x8000,0x2040,0x8002,0x2044 },

	{ 0x0,0x8002,0x8001,0x4001,0x204f },
	{ 0xa,0x8002,0x8001,0x4001,0x8002 },
	{ 0x4,0x8003,0x2040,0x8002,0x2044 }
};
