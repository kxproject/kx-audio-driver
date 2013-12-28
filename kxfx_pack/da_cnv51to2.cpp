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
// Patch name: 'cnv51to20'

char *cnv51to20_copyright="(c) kX Project, 2002-2004.";
char *cnv51to20_engine="kX";
char *cnv51to20_comment="5.1 to Stereo Encoder";
char *cnv51to20_created="09/30/2002";
char *cnv51to20_guid="e38441c0-d40c-11d6-8f89-a22ffa804d06";

char *cnv51to20_name="cnv51to20";
int cnv51to20_itramsize=0,cnv51to20_xtramsize=0;

dsp_register_info cnv51to20_info[]={
	{ "in_front_l",0x4000,0x7,0xffff,0x0 },
	{ "in_front_r",0x4001,0x7,0xffff,0x0 },
	{ "in_rear_l",0x4002,0x7,0xffff,0x0 },
	{ "in_rear_r",0x4003,0x7,0xffff,0x0 },
	{ "in_center",0x4004,0x7,0xffff,0x0 },
	{ "in_sub",0x4005,0x7,0xffff,0x0 },
	{ "out_l",0x8000,0x8,0xffff,0x0 },
	{ "out_r",0x8001,0x8,0xffff,0x0 },
	{ "front",0x8002,0x4,0xffff,0x7fffffff },
	{ "center",0x8003,0x4,0xffff,0x5a827999 },
	{ "rear",0x8004,0x4,0xffff,0x5a827999 },
	{ "a",0x8005,0x3,0xffff,0x0 },
	{ "b",0x8006,0x3,0xffff,0x0 },
	{ "c",0x8007,0x3,0xffff,0x0 },
};

dsp_code cnv51to20_code[]={
	{ 0x0,0x8007,0x4005,0x4004,0x8003 },
	{ 0x1,0x8005,0x4002,0x4003,0x204d },
	{ 0x1,0x8006,0x4003,0x4002,0x204d },
	{ 0x0,0x8000,0x8007,0x4000,0x8002 },
	{ 0x0,0x8001,0x8007,0x4001,0x8002 },
	{ 0x1,0x8000,0x8000,0x8005,0x8004 },
	{ 0x1,0x8001,0x8001,0x8006,0x8004 },
};

