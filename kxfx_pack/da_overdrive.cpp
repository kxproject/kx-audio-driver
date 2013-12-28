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
// Patch name: 'Overdrive'

char *overdrive_copyright="(c) Cybercurve, 2002";
char *overdrive_engine="kX";
char *overdrive_comment="simple overdrive";
char *overdrive_created="Jan 15 2002";
char *overdrive_guid="3753C6BA-FA55-4e79-A35C-A3B9F1EF3D45";

char *overdrive_name="Overdrive";
int overdrive_itramsize=0,overdrive_xtramsize=0;

dsp_register_info overdrive_info[]={
	{ "in",0x4000,0x7,0xffff,0x0 },
	{ "out",0x8000,0x8,0xffff,0x0 },
	{ "gain1",0x8001,0x4,0xffff,0x80000000 },
	{ "gain2",0x8002,0x4,0xffff,0x10000 },
	{ "gain",0x8003,0x3,0xffff,0x0 },
};

dsp_code overdrive_code[]={
	{ 0xe,0x8003,0x8001,0x4000,0x8002 },
	{ 0x0,0x8000,0x2040,0x4000,0x8003 },
};

