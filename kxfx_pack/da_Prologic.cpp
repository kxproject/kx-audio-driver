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
// Patch name: 'Prologic'

char *Prologic_copyright="Copyright (c) 2004.";
// NOTE: The present DSP microcode dump is protected by the 
// license agreement bundled with the appropriate software 
// package containing this microcode,
// regardless the particular copyright notice is present in the dump.

char *Prologic_engine="kX";
char *Prologic_comment="Dolby Prologic encoder";
char *Prologic_created="12/17/2007";
char *Prologic_guid="c3cbd3ac-b3bc-4022-b18d-5b157b305838";

char *Prologic_name="Dolby Prologic";
int Prologic_itramsize=0,Prologic_xtramsize=0;

dsp_register_info Prologic_info[]={
	{ "L",0x4000,0x7,0xffff,0x0 },
	{ "R",0x4001,0x7,0xffff,0x0 },
	{ "Ls",0x4002,0x7,0xffff,0x0 },
	{ "Rs",0x4003,0x7,0xffff,0x0 },
	{ "C",0x4004,0x7,0xffff,0x0 },
	{ "Lt",0x8000,0x8,0xffff,0x0 },
	{ "Rt",0x8001,0x8,0xffff,0x0 },
	{ "tL",0x8002,0x3,0xffff,0x0 },
	{ "tr",0x8003,0x3,0xffff,0x0 },
};

dsp_code Prologic_code[]={
	{ 0x0,0x8002,0x4000,0x4004,0x2054 },
	{ 0x1,0x8002,0x8002,0x4002,0x2054 },
	{ 0x1,0x8002,0x8002,0x4003,0x2054 },
	{ 0x0,0x8003,0x4001,0x4004,0x2054 },
	{ 0x0,0x8003,0x8003,0x4002,0x2054 },
	{ 0x0,0x8003,0x8003,0x4003,0x2054 },
	{ 0x0,0x8000,0x8002,0x2040,0x2040 },
	{ 0x0,0x8001,0x8003,0x2040,0x2040 },
};

