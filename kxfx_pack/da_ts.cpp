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
// Patch name: 'TS Effect'

char *ts_copyright="(c) Max Mikhailov and Eugene Gavrilov, 2001-2005";
char *ts_engine="kX";
char *ts_comment="version 0.2";
char *ts_created="2001";
char *ts_guid="33E8D755-AC3D-4bcc-9681-7DE25B989DD9";

char *ts_name="TubeSound";
int ts_itramsize=0,ts_xtramsize=0;

dsp_register_info ts_info[]={
	{ "inl",0x4000,0x7,0xffff,0x0 },
	{ "inr",0x4001,0x7,0xffff,0x0 },
	{ "outl",0x8000,0x8,0xffff,0x0 },
	{ "outr",0x8001,0x8,0xffff,0x0 },
	{ "amount",0x8002,0x4,0xffff,0x7fffffff },
	{ "dcl",0x8003,0x1,0xffff,0x0 },
	{ "dcr",0x8004,0x1,0xffff,0x0 },
	{ "_AC_0x75c28",0x8005,0x1,0xffff,0x75c28f5b },
	{ "_AC_0x68db8",0x8006,0x1,0xffff,0x68db8 },
	{ "t",0x8007,0x3,0xffff,0x0 },
	{ "s",0x8008,0x3,0xffff,0x0 },
};

dsp_code ts_code[]={
	{ 0x0,0x8007,0x2040,0x4000,0x8005 },
	{ 0x1,0x8008,0x2040,0x8007,0x8007 },
	{ 0x1,0x8008,0x8007,0x8008,0x8002 },
	{ 0xe,0x8003,0x8003,0x8006,0x2040 },
	{ 0x0,0x8003,0x8003,0x8008,0x8006 },
	{ 0x1,0x8000,0x8008,0x8003,0x204f },
	{ 0x0,0x8007,0x2040,0x4001,0x8005 },
	{ 0x1,0x8008,0x2040,0x8007,0x8007 },
	{ 0x1,0x8008,0x8007,0x8008,0x8002 },
	{ 0xe,0x8004,0x8004,0x8006,0x2040 },
	{ 0x0,0x8004,0x8004,0x8008,0x8006 },
	{ 0x1,0x8001,0x8008,0x8004,0x204f },
};

