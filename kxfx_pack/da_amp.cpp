// EF-X Library
// Copyright (c) Martin Borisov, 2004.
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
// Patch name: 'amp'

char *amp_copyright="by eYagos - Copyright (c) 2003-2004.";
char *amp_engine="kX";
char *amp_comment="Amplifies the signal without looses ";
char *amp_created="02/14/2003";
char *amp_guid="d7e58d27-eb7f-4cde-84fc-a4a238fc1025";

char *amp_name="gainHQ";
int amp_itramsize=0,amp_xtramsize=0;

#define R_GAINL_I	0x8002
#define R_GAINL_F	0x8003
#define R_GAINR_I	0x8004
#define R_GAINR_F	0x8005

dsp_register_info amp_info[]={
	{ "inl",0x4000,0x7,0xffff,0x0 },
	{ "inr",0x4001,0x7,0xffff,0x0 },
	{ "outl",0x8000,0x8,0xffff,0x0 },
	{ "outr",0x8001,0x8,0xffff,0x0 },
	{ "gainl_i",0x8002,0x4,0xffff,0x4 },
	{ "gainl_f",0x8003,0x4,0xffff,0x13333333 },
	{ "gainr_i",0x8004,0x4,0xffff,0x4 },
	{ "gainr_f",0x8005,0x4,0xffff,0x13333333 },
	{ "tmp",0x8006,0x3,0xffff,0x0 },
};

dsp_code amp_code[]={
	{ 0x4,0x8006,0x2040,0x4000,0x8002 },
	{ 0x0,0x8000,0x8006,0x4000,0x8003 },
	{ 0x4,0x8006,0x2040,0x4001,0x8004 },
	{ 0x0,0x8001,0x8006,0x4001,0x8005 },
};
