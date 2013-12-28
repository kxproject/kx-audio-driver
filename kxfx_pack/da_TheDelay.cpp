// kX DSP Generated file

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
// Patch name: 'TheDelay'

char *TheDelay_copyright="Copyright (c) 2007. Tril";
// NOTE: The present DSP microcode dump is protected by the 
// license agreement bundled with the appropriate software 
// package containing this microcode,
// regardless the particular copyright notice is present in the dump.

char *TheDelay_engine="kX";
char *TheDelay_comment="Can delay a signal from 25 to 24000 samples.";
char *TheDelay_created="02/24/2007";
char *TheDelay_guid="f83701f0-59bd-4677-8de4-2c974be925bf";

char *TheDelay_name="TheDelay";
int TheDelay_itramsize=0,TheDelay_xtramsize=24000;

dsp_register_info TheDelay_info[]={
	{ "in",0x4000,0x7,0xffff,0x0 },
	{ "out",0x8000,0x8,0xffff,0x0 },
	{ "dw",0x8001,0x4a,0xffff,0x0 },
	{ "&dw",0x8002,0xb,0xffff,0x0 },
	{ "dr",0x8003,0x2a,0xffff,0x0 },
	{ "&dr",0x8004,0xb,0xffff,0x0 },
};

dsp_code TheDelay_code[]={
	{ 0x0,0x8001,0x4000,0x2040,0x2040 },
	{ 0x0,0x8000,0x8003,0x2040,0x2040 },
};

#define DW 0x8001
#define DR 0x8003