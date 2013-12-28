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
// Patch name: 'booblegum'

char *booblegum_copyright="Copyright c Max Mikhailov, 2004.";
char *booblegum_engine="kX";
char *booblegum_comment="";
char *booblegum_created="April'2004";
char *booblegum_guid="4016dea0-8e53-11d8-bad1-c15710887410";

char *booblegum_name="Booble Gum";
int booblegum_itramsize=0,booblegum_xtramsize=0;

dsp_register_info booblegum_info[]={
	{ "l",0x4000,0x7,0xffff,0x0 },
	{ "h",0x8000,0x8,0xffff,0x0 },
	{ "e",0x8001,0x5,0xffff,0xccccccc },
	{ "c",0x8002,0x3,0xffff,0x0 },
};

dsp_code booblegum_code[]={
	{ 0xb,0x8002,0x2055,0x2055,0x4000 },
	{ 0xe,0x8000,0x8000,0x8001,0x8002 },
};

