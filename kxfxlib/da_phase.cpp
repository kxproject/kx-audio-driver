// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov, 2002-2004.
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
// Patch name: 'Phase'

const char *phase_copyright="Copyright (c) Eugene Gavrilov, 2002-2004.";
const char *phase_engine="kX";
const char *phase_comment="";
const char *phase_created="04/10/2002";
const char *phase_guid="5d13c9e0-6dd5-4c87-a624-e19a03587af3";

const char *phase_name="Phase";
int phase_itramsize=0,phase_xtramsize=0;

dsp_register_info phase_info[]={
	{ "inl",0x4000,0x7,0xffff,0x0 },
	{ "inr",0x4001,0x7,0xffff,0x0 },
	{ "outl",0x8000,0x8,0xffff,0x0 },
	{ "outr",0x8001,0x8,0xffff,0x0 },
};

dsp_code phase_code[]={
	{ 0x1,0x8000,0x2040,0x4000,0x204f },
	{ 0x1,0x8001,0x2040,0x4001,0x204f },
};

