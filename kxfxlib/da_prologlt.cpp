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
// Patch name: 'prologlt'

const char *prologlt_copyright="Copyright (c) Eugene Gavrilov, 2003-2004.";
const char *prologlt_engine="kX";
const char *prologlt_comment="prolog lite - no resources used";
const char *prologlt_created="05/11/2003";
const char *prologlt_guid="06f1854e-8e8f-465f-8d9c-966cfcc20dc7";

const char *prologlt_name="prologlt";
int prologlt_itramsize=0,prologlt_xtramsize=0;

dsp_register_info prologlt_info[]={
	{ "ac97_l",0x8000,0x8,0xffff,0x0 },
	{ "ac97_r",0x8001,0x8,0xffff,0x0 },
	{ "spdifA_l",0x8002,0x8,0xffff,0x0 },
	{ "spdifA_r",0x8003,0x8,0xffff,0x0 },
	{ "spdifC_l",0x8004,0x8,0xffff,0x0 },
	{ "spdifC_r",0x8005,0x8,0xffff,0x0 },
	{ "spdifB_l",0x8006,0x8,0xffff,0x0 },
	{ "spdifB_r",0x8007,0x8,0xffff,0x0 },
	{ "i2s0_l",0x8008,0x8,0xffff,0x0 },
	{ "i2s0_r",0x8009,0x8,0xffff,0x0 },
	{ "i2s1_l",0x800a,0x8,0xffff,0x0 },
	{ "i2s1_r",0x800b,0x8,0xffff,0x0 },
	{ "i2s2_l",0x800c,0x8,0xffff,0x0 },
	{ "i2s2_r",0x800d,0x8,0xffff,0x0 },
	{ "ac97_mic",0x800e,0x8,0xffff,0x0 },
};

dsp_code prologlt_code[]={ { 0xff } };

