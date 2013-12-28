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
// Patch name: 'xor'

char *xor_copyright="Copyright (c) Eugene Gavrilov, 2001-2005.";
char *xor_engine="kX";
char *xor_comment="";
char *xor_created="01/14/2002";
char *xor_guid="cb09c9c9-725a-4338-945d-7596b137dfee";

char *xor_name="xor";
int xor_itramsize=0,xor_xtramsize=0;

dsp_register_info xor_info[]={
	{ "in",0x4000,0x7,0xffff,0x0 },
	{ "out",0x8000,0x8,0xffff,0x0 },
	{ "mask",0x8001,0x1,0xffff,0xffff0000 },
};

dsp_code xor_code[]={
	{ 0x8,0x8000,0x4000,0x8001,0x2040 },
};

