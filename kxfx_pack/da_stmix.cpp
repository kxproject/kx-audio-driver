// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov, Rémy TROTIN 2001-2005.
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
// Patch name: 'stmix'

char *stmix_copyright="by eYagos, Copyright (c) 2003-2004.";
char *stmix_engine="kX";
char *stmix_comment="Mixes two stereo channels";
char *stmix_created="02/14/2003";
char *stmix_guid="34131ae6-3e2d-416f-acd9-7d1bf4e77860";

char *stmix_name="Stereo Mix + Gain";
int stmix_itramsize=0,stmix_xtramsize=0;

#define R_GAIN1_I	0x8002
#define R_GAIN2_I	0x8003
#define R_GAIN1_F	0x8004
#define R_GAIN2_F	0x8005

dsp_register_info stmix_info[]={
	{ "in1L",0x4000,0x7,0xffff,0x0 },
	{ "in1R",0x4001,0x7,0xffff,0x0 },
	{ "in2L",0x4002,0x7,0xffff,0x0 },
	{ "in2R",0x4003,0x7,0xffff,0x0 },
	{ "outL",0x8000,0x8,0xffff,0x0 },
	{ "outR",0x8001,0x8,0xffff,0x0 },
	{ "gain1_I",0x8002,0x4,0xffff,0x0 },
	{ "gain2_I",0x8003,0x4,0xffff,0x0 },
	{ "gain1_F",0x8004,0x4,0xffff,0x0 },
	{ "gain2_F",0x8005,0x4,0xffff,0x0 },
	{ "tmp",0x8006,0x3,0xffff,0x0 },
};

dsp_code stmix_code[]={
	{ 0x4,0x8006,0x2040,0x4000,0x8002 },
	{ 0x0,0x8006,0x8006,0x4000,0x8004 },
	{ 0x4,0x8006,0x8006,0x4002,0x8003 },
	{ 0x0,0x8000,0x8006,0x4002,0x8005 },
	{ 0x4,0x8006,0x2040,0x4001,0x8002 },
	{ 0x0,0x8006,0x8006,0x4001,0x8004 },
	{ 0x4,0x8006,0x8006,0x4003,0x8003 },
	{ 0x0,0x8001,0x8006,0x4003,0x8005 },
};

