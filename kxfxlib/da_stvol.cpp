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
// Patch name: 'st vol'

const char *stvol_copyright="(c) Eugene Gavrilov, 2001-2005";
const char *stvol_engine="kX";
const char *stvol_comment="";
const char *stvol_created="2002";
const char *stvol_guid="a539e24a-4f08-46b6-9d86-70b81401ebcf";

const char *stvol_name="St Vol";
int stvol_itramsize=0,stvol_xtramsize=0;

dsp_register_info stvol_info[]={
	{ "inl",0x4000,0x7,0xffff,0x0 },
	{ "outl",0x8000,0x8,0xffff,0x0 },
	{ "inr",0x4001,0x7,0xffff,0x0 },
	{ "outr",0x8001,0x8,0xffff,0x0 },
	{ "vol",0x8002,0x4,0xffff,0x7fffffff },
};

dsp_code stvol_code[]={
	{ 0x0,0x8000,0x2040,0x4000,0x8002 },
	{ 0x0,0x8001,0x2040,0x4001,0x8002 }
};
