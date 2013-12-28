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
// Patch name: 'p16v'

const char *p16v_copyright="Copyright (c) Eugene Gavrilov, 2003-2004.";
const char *p16v_engine="kX";
const char *p16v_comment="p16v outputs; for audigy2 only";
const char *p16v_created="09/02/2003";
const char *p16v_guid="85E09152-D489-4dee-8D60-6A509DC70901";

const char *p16v_name="p16v";
int p16v_itramsize=0,p16v_xtramsize=0;

dsp_register_info p16v_info[]={
	{ "in1",0x8000,0x8,0xffff,0x0 },
	{ "in2",0x8001,0x8,0xffff,0x0 },
	{ "in3",0x8002,0x8,0xffff,0x0 },
	{ "in4",0x8003,0x8,0xffff,0x0 },
	{ "in5",0x8004,0x8,0xffff,0x0 },
	{ "in6",0x8005,0x8,0xffff,0x0 },
	{ "in7",0x8006,0x8,0xffff,0x0 },
	{ "in8",0x8007,0x8,0xffff,0x0 }
};

dsp_code p16v_code[]={
	{ 0x0,0x8000,0x2040,KX_E32IN(0),0x204c },
	{ 0x0,0x8001,0x2040,KX_E32IN(1),0x204c },
	{ 0x0,0x8002,0x2040,KX_E32IN(2),0x204c },
	{ 0x0,0x8003,0x2040,KX_E32IN(3),0x204c },
	{ 0x0,0x8004,0x2040,KX_E32IN(4),0x204c },
	{ 0x0,0x8005,0x2040,KX_E32IN(5),0x204c },
	{ 0x0,0x8006,0x2040,KX_E32IN(6),0x204c },
	{ 0x0,0x8007,0x2040,KX_E32IN(7),0x204c },
};

