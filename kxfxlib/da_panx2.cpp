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

// kX DSP Generated file

// 10kX microcode
// Patch name: 'panx2'

const char *panx2_copyright="Copyright (c) 2002-2004. kX Project";
const char *panx2_engine="kX";
const char *panx2_comment="";
const char *panx2_created="05/12/2002";
const char *panx2_guid="aa395f80-6e18-4acf-855f-74d8cfb2f406";

const char *panx2_name="Pan x2";
int panx2_itramsize=0,panx2_xtramsize=0;

dsp_register_info panx2_info[]={
	{ "in",0x4000,0x7,0xffff,0x0 },
	{ "outl",0x8000,0x8,0xffff,0x0 },
	{ "outr",0x8001,0x8,0xffff,0x0 },
	{ "Pan",0x8002,0x4,0xffff,0x3fffffff },
	{ "tmp",0x8003,0x3,0xffff,0x0 },
};

dsp_code panx2_code[]={
	{ 0x4,0x8003,0x2040,0x4000,0x2042 },
	{ 0x0,0x8001,0x2040,0x8003,0x8002 },
	{ 0x1,0x8000,0x8003,0x8003,0x8002 },
};

