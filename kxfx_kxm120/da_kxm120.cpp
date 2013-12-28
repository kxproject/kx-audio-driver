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
// Patch name: 'kxm120'

char *kxm120_copyright="2008. russ, stylus";
// NOTE: The present DSP microcode dump is protected by the 
// license agreement bundled with the appropriate software 
// package containing this microcode,
// regardless the particular copyright notice is present in the dump.

char *kxm120_engine="kX";
char *kxm120_comment="keyboard unit with fast midi support (kxm 120 v1.0)";
char *kxm120_created="05/19/2008";
char *kxm120_guid="005b7557-4136-424e-bfc5-f1ab428c7322";

char *kxm120_name="kxm 120";
int kxm120_itramsize=0,kxm120_xtramsize=0;

dsp_register_info kxm120_info[]={
	{ "cv",0x8000,0x8,0xffff,0x0 },
	{ "gate",0x8001,0x8,0xffff,0x0 },
	{ "cv_val",0x8002,0x1,0xffff,0x0 },
	{ "gate_val",0x8003,0x1,0xffff,0x0 },
};

dsp_code kxm120_code[]={
	{ 0x0,0x8000,0x8002,0x2040,0x2040 },
	{ 0x0,0x8001,0x8003,0x2040,0x2040 },
};

#define CV_VAL_P	0x8002
#define GATE_VAL_P	0x8003
