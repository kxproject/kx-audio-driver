// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov and LeMury, 2003-2004.
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
// Patch name: 'src'

const char *src_copyright="Copyright (c) LeMury and Eugene Gavrilov, 2003-2004. All rights reserved.";
const char *src_engine="kX";
const char *src_comment="Source Selector v2.0 $nobypass";
const char *src_created="11/06/2003";
const char *src_guid="6e302861-0df9-4b21-b91a-6631220da97d";

const char *src_name="Src";
int src_itramsize=0,src_xtramsize=0;

#define _EL_INSTR	//offset = 0

dsp_register_info src_info[]={
	{ "Left",0x8000,0x8,0xffff,0x0 },
	{ "Right",0x8001,0x8,0xffff,0x0 },
};

dsp_code src_code[]={
	{ 0x0,0x8000,0x2040,0x2040,0x204f }, 
	{ 0x0,0x8001,0x2040,0x2040,0x204f },
};

