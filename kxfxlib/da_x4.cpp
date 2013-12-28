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
// Patch name: 'x4'

const char *x4_copyright="Copyright (c) 2003-2004, kX Project";
const char *x4_engine="kX";
const char *x4_comment="";
const char *x4_created="04/16/2003";
const char *x4_guid="f0edcc1f-040b-4a94-990b-19dfb753a248";

const char *x4_name="x4";
int x4_itramsize=0,x4_xtramsize=0;

dsp_register_info x4_info[]={
	{ "inl",0x4000,0x7,0xffff,0x0 },
	{ "inr",0x4001,0x7,0xffff,0x0 },
	{ "outl",0x8000,0x8,0xffff,0x0 },
	{ "outr",0x8001,0x8,0xffff,0x0 },
};

dsp_code x4_code[]={
	{ 0x4,0x8000,0x2040,0x4000,0x2044 },
	{ 0x4,0x8001,0x2040,0x4001,0x2044 },
};

