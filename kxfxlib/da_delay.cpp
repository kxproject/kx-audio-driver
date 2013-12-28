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
// Patch name: 'Delay'

const char *delay_name="Delay Old";
int delay_itramsize=0,delay_xtramsize=14400;

const char *delay_copyright="(c) Eugene Gavrilov, 2001-2005";
const char *delay_comment="";
const char *delay_created="2002";
const char *delay_engine="kX";
const char *delay_guid="D54D398D-A8DF-4ec0-8C64-E706E0A1C466";

dsp_register_info delay_info[]={
	{ "out",0x8000,0x8,0x488,0x0 },
	{ "in",0x4000,0x7,0x2040,0x0 },
	{ "level",0x8001,0x4,0x489,0x7fffffff },
	{ "feedback",0x8002,0x4,0x48a,0x40000000 },
	{ "delay",0x8003,0x4,0x48b,0x1c20000 },
	{ "wrt",0x8004,0x4a,0x2db,0x0 },
	{ "&wrt",0x8005,0xb,0x3db,0x0 },
	{ "rd",0x8006,0x2a,0x2dc,0x0 },
	{ "&rd",0x8007,0xb,0x3dc,0x0 },
};

dsp_code delay_code[]={
	{ 0x6,0x8007,0x8003,0x8005,0x2040 },
	{ 0x0,0x8000,0x4000,0x8001,0x8006 },
	{ 0x0,0x8004,0x4000,0x8006,0x8002 },
};

