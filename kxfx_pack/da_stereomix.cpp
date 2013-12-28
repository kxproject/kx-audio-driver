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
// Patch name: 'StereoMix'

char *stereomix_copyright="Copyright (c) 2002-2004, kX Project";
char *stereomix_engine="kX";
char *stereomix_comment="";
char *stereomix_created="05/05/2002";
char *stereomix_guid="32a03c8a-5c04-487e-9cc4-e8c5a25f0ef4";

char *stereomix_name="Stereo Mix";
int stereomix_itramsize=0,stereomix_xtramsize=0;

dsp_register_info stereomix_info[]={
	{ "in1L",0x4000,0x7,0xffff,0x0 },
	{ "in1R",0x4001,0x7,0xffff,0x0 },
	{ "in2L",0x4002,0x7,0xffff,0x0 },
	{ "in2R",0x4003,0x7,0xffff,0x0 },
	{ "outL",0x8000,0x8,0xffff,0x0 },
	{ "outR",0x8001,0x8,0xffff,0x0 },
	{ "In1 Level",0x8002,0x4,0xffff,0x0 },
	{ "In2 Level",0x8003,0x4,0xffff,0x0 },
	{ "tmp",0x8004,0x3,0xffff,0x0 },
};

dsp_code stereomix_code[]={
	{ 0x0,0x8004,0x2040,0x4000,0x8002 },
	{ 0x0,0x8000,0x8004,0x4002,0x8003 },
	{ 0x0,0x8004,0x2040,0x4001,0x8002 },
	{ 0x0,0x8001,0x8004,0x4003,0x8003 },
};

