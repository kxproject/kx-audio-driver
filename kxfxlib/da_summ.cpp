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
// Patch name: 'summ'

const char *summ_copyright="(c) Max Mikhailov and Eugene Gavrilov, 2001-2005";
const char *summ_engine="kX";
const char *summ_comment="";
const char *summ_created="2002";
const char *summ_guid="2B7655A7-0D05-4116-AE4F-CB950FC5961F";

const char *summ_name="Summ";
int summ_itramsize=0,summ_xtramsize=0;

dsp_register_info summ_info[]={
	{ "in0",0x4000,0x7,0xffff,0x0 },
	{ "in1",0x4001,0x7,0xffff,0x0 },
	{ "in2",0x4002,0x7,0xffff,0x0 },
	{ "out",0x8000,0x8,0xffff,0x0 },
};

dsp_code summ_code[]={
	{ 0x6,0x8000,0x4000,0x4001,0x4002 },
};

