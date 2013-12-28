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

// kX DSP/Dane Generated file

// 10kX microcode
// Patch name: 'demo'

const char *demo_copyright="Copyright (c) kX Project, 2002-2004. All rights reserved";
const char *demo_engine="kX";
const char *demo_comment="";
const char *demo_created="05/03/2002";
const char *demo_guid="ac9e749d-1c7d-4fd6-9896-9794c12b6d31";

const char *demo_name="demo";
int demo_itramsize=0,demo_xtramsize=0;

dsp_register_info demo_info[]={
	{ "in1",0x4000,0x7,0xffff,0x0 },
	{ "in2",0x4001,0x7,0xffff,0x0 },
	{ "out1",0x8000,0x8,0xffff,0x0 },
	{ "out2",0x8001,0x8,0xffff,0x0 },
	{ "vol1",0x8002,0x4,0xffff,0x0 },
	{ "vol2",0x8003,0x4,0xffff,0x0 },
};

dsp_code demo_code[]={
	{ 0x0,0x8000,0x2040,0x4000,0x8002 },
	{ 0x0,0x8001,0x2040,0x4001,0x8003 },
};

#define VOL1_P	0x8002
#define VOL2_P	0x8003
