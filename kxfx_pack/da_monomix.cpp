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
// Patch name: 'monomix'

char *monomix_copyright="Copyright (c) 2002-2004, kX Project";
char *monomix_engine="kX";
char *monomix_comment="";
char *monomix_created="05/10/2002";
char *monomix_guid="2b53ce82-8b75-43c3-9a8e-07cb812bce06";

char *monomix_name="Mono Mix";
int monomix_itramsize=0,monomix_xtramsize=0;

dsp_register_info monomix_info[]={
	{ "inl",0x4000,0x7,0xffff,0x0 },
	{ "inr",0x4001,0x7,0xffff,0x0 },
	{ "out",0x8000,0x8,0xffff,0x0 },
	{ "left",0x8001,0x4,0xffff,0x0 },
	{ "right",0x8002,0x4,0xffff,0x0 },
	{ "tmp",0x8003,0x3,0xffff,0x0 },
};

dsp_code monomix_code[]={
	{ 0x0,0x8003,0x2040,0x4000,0x8001 },
	{ 0x0,0x8000,0x8003,0x4001,0x8002 },
};

