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
// Patch name: 'Dither'

char *Dither_copyright="JoshuaChang, 2002-2004";
// NOTE: The present DSP microcode dump is protected by the 
// license agreement bundled with the appropriate software 
// package containing this microcode,
// regardless the particular copyright notice is present in the dump.

char *Dither_engine="Emu10kX";
char *Dither_comment="Simple Dither System for Loopback Recording & Analog Output @ 48kHz";
char *Dither_created="July 22 2004";
char *Dither_guid="23e00a6e-6646-47e7-877d-0a3beb7bc7d8";

char *Dither_name="Dither";
int Dither_itramsize=0,Dither_xtramsize=0;

dsp_register_info Dither_info[]={
	{ "in_l",0x4000,0x7,0xffff,0x0 },
	{ "in_r",0x4001,0x7,0xffff,0x0 },
	{ "out_l",0x8000,0x8,0xffff,0x0 },
	{ "out_r",0x8001,0x8,0xffff,0x0 },
	{ "level",0x8002,0x1,0xffff,0x34ad },
	{ "r004",0x8003,0x1,0xffff,0x2dae00b6 },
	{ "r005",0x8004,0x1,0xffff,0x80000000 },
	{ "r006",0x8005,0x1,0xffff,0x40000000 },
	{ "r007",0x8006,0x1,0xffff,0x2041ec2a },
	{ "r008",0x8007,0x1,0xffff,0x4e5eabd },
	{ "r009",0x8008,0x1,0xffff,0x0 },
	{ "r010",0x8009,0x1,0xffff,0x0 },
	{ "r013",0x800a,0x1,0xffff,0x1 },
	{ "bn",0x800b,0x3,0xffff,0x0 },
	{ "r014",0x800c,0x3,0xffff,0x0 },
};

dsp_code Dither_code[]={
	{ 0x4,0x800b,0x2040,0x2058,0x2042 },
	{ 0x0,0x800b,0x2040,0x800b,0x8002 },
	{ 0x0,0x800c,0x2040,0x800b,0x8003 },
	{ 0x6,0x800b,0x2056,0x8008,0x8008 },
	{ 0x0,0x8008,0x8009,0x800b,0x8006 },
	{ 0x0,0x8008,0x2056,0x800c,0x8004 },
	{ 0x0,0x8009,0x2041,0x800b,0x8007 },
	{ 0x0,0x8009,0x2056,0x800c,0x8005 },
	{ 0x4,0x800b,0x800a,0x800b,0x800a },
	{ 0x6,0x8000,0x2040,0x4000,0x800b },
	{ 0x6,0x8001,0x2040,0x4001,0x800b },
};

