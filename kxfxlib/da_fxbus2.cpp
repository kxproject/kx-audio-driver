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
// Patch name: 'fxbus2'

const char *fxbus2_copyright="(c) Eugene Gavrilov, 2001-2005";
const char *fxbus2_engine="kX";
const char *fxbus2_comment="$nobypass";
const char *fxbus2_created="2003";
const char *fxbus2_guid="131f1059-f384-4403-abd0-16ef6025bb9b";

const char *fxbus2_name="FXBus2";
int fxbus2_itramsize=0,fxbus2_xtramsize=0;

dsp_register_info fxbus2_info[]={
        { "FXBus32",0x8000,0x8,0xffff,0x0 },
        { "FXBus33",0x8001,0x8,0xffff,0x0 },
        { "FXBus34",0x8002,0x8,0xffff,0x0 },
        { "FXBus35",0x8003,0x8,0xffff,0x0 },
        { "FXBus36",0x8004,0x8,0xffff,0x0 },
        { "FXBus37",0x8005,0x8,0xffff,0x0 },
        { "FXBus38",0x8006,0x8,0xffff,0x0 },
        { "FXBus39",0x8007,0x8,0xffff,0x0 },
        { "FXBus40",0x8008,0x8,0xffff,0x0 },
        { "FXBus41",0x8009,0x8,0xffff,0x0 },
        { "FXBus42",0x800a,0x8,0xffff,0x0 },
        { "FXBus43",0x800b,0x8,0xffff,0x0 },
        { "FXBus44",0x800c,0x8,0xffff,0x0 },
        { "FXBus45",0x800d,0x8,0xffff,0x0 },
	    { "FXBus46",0x800e,0x8,0xffff,0x0 },
        { "FXBus47",0x800f,0x8,0xffff,0x0 },
        { "FXBus48",0x8010,0x8,0xffff,0x0 },
        { "FXBus49",0x8011,0x8,0xffff,0x0 },
        { "FXBus50",0x8012,0x8,0xffff,0x0 },
        { "FXBus51",0x8013,0x8,0xffff,0x0 },
        { "FXBus52",0x8014,0x8,0xffff,0x0 },
        { "FXBus53",0x8015,0x8,0xffff,0x0 },
        { "FXBus54",0x8016,0x8,0xffff,0x0 },
        { "FXBus55",0x8017,0x8,0xffff,0x0 },
        { "FXBus56",0x8018,0x8,0xffff,0x0 },
        { "FXBus57",0x8019,0x8,0xffff,0x0 },
        { "FXBus58",0x801a,0x8,0xffff,0x0 },
        { "FXBus59",0x801b,0x8,0xffff,0x0 },
        { "FXBus60",0x801c,0x8,0xffff,0x0 },
        { "FXBus61",0x801d,0x8,0xffff,0x0 },
        { "FXBus62",0x801e,0x8,0xffff,0x0 },
        { "FXBus63",0x801f,0x8,0xffff,0x0 },
};

dsp_code fxbus2_code[]={
	{ 0xff } // this is not used
};
