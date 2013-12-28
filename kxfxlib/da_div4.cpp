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
// Patch name: 'div4'

const char *div4_copyright="Copyright (c) 2003-2004, kX Project";
const char *div4_engine="kX";
const char *div4_comment="";
const char *div4_created="04/16/2003";
const char *div4_guid="66cf84ff-c63a-49c6-9850-21766df54699";

const char *div4_name="div4";
int div4_itramsize=0,div4_xtramsize=0;

dsp_register_info div4_info[]={
	{ "inl",0x4000,0x7,0xffff,0x0 },
	{ "inr",0x4001,0x7,0xffff,0x0 },
	{ "outl",0x8000,0x8,0xffff,0x0 },
	{ "outr",0x8001,0x8,0xffff,0x0 },
};

dsp_code div4_code[]={
	{ 0x0,0x8000,0x2040,0x4000,0x204c },
	{ 0x0,0x8001,0x2040,0x4001,0x204c },
};

