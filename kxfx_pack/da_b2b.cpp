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

/*
input in
output out
temp t
 
---
; before x4            
macs t, 0x4000, in, 1 
limit out, 0x4000, in, t
 
----
; after x4            
macs t, 0x10000, in, 1 
limit out, 0x10000, in, t
*/

// kX DSP Generated file

// 10kX microcode
// Patch name: 'b2b'

char *b2b_copyright="Copyright (c) 2002-2004. kX Project. All rights reserved";
char *b2b_engine="kX";
char *b2b_comment="pre-x4 mode; v2.1";
char *b2b_created="26/06/2003";
char *b2b_guid="e4c6b77f-ee7a-4df0-92ea-cac787dd223c";

char *b2b_name="b2b";
int b2b_itramsize=0,b2b_xtramsize=0;

dsp_register_info b2b_info[]={
	{ "inl",0x4000,0x7,0xffff,0x0 },
	{ "outl",0x8000,0x8,0xffff,0x0 },
	{ "inr",0x4001,0x7,0xffff,0x0 },
	{ "outr",0x8003,0x8,0xffff,0x0 },
	{ "_AC4000",0x8001,0x1,0xffff,0x4000 },
	{ "t",0x8002,0x3,0xffff,0x0 },
};

dsp_code b2b_code[]={
	{ 0x0,0x8002,0x8001,0x4000,0x204f },
	{ 0xa,0x8000,0x8001,0x4000,0x8002 },
	{ 0x0,0x8002,0x8001,0x4001,0x204f },
	{ 0xa,0x8003,0x8001,0x4001,0x8002 },
};

