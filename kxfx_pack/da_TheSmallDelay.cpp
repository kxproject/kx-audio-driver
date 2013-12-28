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
// Patch name: 'TheSmallDelay'

char *TheSmallDelay_copyright="Copyright (c) 2007. Tril";
// NOTE: The present DSP microcode dump is protected by the 
// license agreement bundled with the appropriate software 
// package containing this microcode,
// regardless the particular copyright notice is present in the dump.

char *TheSmallDelay_engine="kX";
char *TheSmallDelay_comment="Can delay a signal from 0.02ms to 25 ms.";
char *TheSmallDelay_created="02/24/2007";
char *TheSmallDelay_guid="1e8ca177-74eb-4842-901c-7d93265836e9";

char *TheSmallDelay_name="TheSmallDelay";
int TheSmallDelay_itramsize=1200,TheSmallDelay_xtramsize=0;

dsp_register_info TheSmallDelay_info[]={
	{ "in",0x4000,0x7,0xffff,0x0 },
	{ "out",0x8000,0x8,0xffff,0x0 },
	{ "dw",0x8001,0x49,0xffff,0x0 },
	{ "&dw",0x8002,0xb,0xffff,0x0 },
	{ "dr",0x8003,0x29,0xffff,0x0 },
	{ "&dr",0x8004,0xb,0xffff,0x0 },
};

dsp_code TheSmallDelay_code[]={
	{ 0x0,0x8001,0x4000,0x2040,0x2040 },
	{ 0x0,0x8000,0x8003,0x2040,0x2040 },
};

#define DW 0x8001
#define DR 0x8003