// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov and Max Mikhailov, 2001-2005.
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
// Patch name: 'downmix'

char *downmix_copyright="Copyright (c) 2002.04.22 Mathias Wagner";
char *downmix_engine="kX";
char *downmix_comment="Simple 5.1 -> 4 Quadro mixer";
char *downmix_created="04/22/2002";
char *downmix_guid="fa2bb22a-1d9b-4ace-9e82-eca900b120ef";

char *downmix_name="downmix";
int downmix_itramsize=0,downmix_xtramsize=0;

dsp_register_info downmix_info[]={
	{ "in_fl",0x4000,0x7,0xffff,0x0 },
	{ "in_fr",0x4001,0x7,0xffff,0x0 },
	{ "in_rl",0x4002,0x7,0xffff,0x0 },
	{ "in_rr",0x4003,0x7,0xffff,0x0 },
	{ "in_c",0x4004,0x7,0xffff,0x0 },
	{ "in_sw",0x4005,0x7,0xffff,0x0 },
	{ "out_fl",0x8000,0x8,0xffff,0x0 },
	{ "out_fr",0x8001,0x8,0xffff,0x0 },
	{ "out_rl",0x8002,0x8,0xffff,0x0 },
	{ "out_rr",0x8003,0x8,0xffff,0x0 },
	{ "Center",0x8004,0x4,0xffff,0x59999998 },
	{ "Subwoofer",0x8005,0x4,0xffff,0x3fffffff },
	{ "tmp_b",0x8006,0x3,0xffff,0x0 },
	{ "tmp_c",0x8007,0x3,0xffff,0x0 },
};

dsp_code downmix_code[]={
	{ 0x0,0x8007,0x2040,0x4004,0x8004 },
	{ 0x0,0x8006,0x2040,0x4005,0x8005 },
	{ 0x6,0x8000,0x4000,0x8007,0x8006 },
	{ 0x6,0x8001,0x4001,0x8007,0x8006 },
	{ 0x6,0x8002,0x4002,0x2040,0x8006 },
	{ 0x6,0x8003,0x4003,0x2040,0x8006 },
};

