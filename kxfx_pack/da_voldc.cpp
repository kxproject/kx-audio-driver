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
// Patch name: 'vol'

char *voldc_copyright="(c) Max Mikhailov and Eugene Gavrilov, 2001-2005";
char *voldc_engine="kX";
char *voldc_comment="";
char *voldc_created="2001";
char *voldc_guid="F924CD0E-893F-4175-89CF-3591E2C80BBD";

char *voldc_name="Vol+DC";
int voldc_itramsize=0,voldc_xtramsize=0;

dsp_register_info voldc_info[]={
	{ "in",0x4000,0x7,0xffff,0x0 },
	{ "out",0x8000,0x8,0xffff,0x0 },
	{ "offset",0x8001,0x4,0xffff,0x0 },
	{ "vol",0x8002,0x4,0xffff,0x0 },
};

dsp_code voldc_code[]={
	{ 0x0,0x8000,0x8001,0x4000,0x8002 },
};
