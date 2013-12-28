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
// Patch name: 'fxbus'

const char *fxbus_copyright="(c) Eugene Gavrilov, 2001-2005";
const char *fxbus_engine="kX";
const char *fxbus_comment="$nobypass";
const char *fxbus_created="2002";
const char *fxbus_guid="2b8b7fa8-98b9-4f6e-81a0-400d3ba39c6f";

const char *fxbus_name="FXBus";
int fxbus_itramsize=0,fxbus_xtramsize=0;

dsp_register_info fxbus_info[]={
	{ "FXBus0",0x8000,0x8,0xffff,0x0 },
        { "FXBus1",0x8001,0x8,0xffff,0x0 },
        { "FXBus2",0x8002,0x8,0xffff,0x0 },
        { "FXBus3",0x8003,0x8,0xffff,0x0 },
        { "FXBus4",0x8004,0x8,0xffff,0x0 },
        { "FXBus5",0x8005,0x8,0xffff,0x0 },
        { "FXBus6",0x8006,0x8,0xffff,0x0 },
        { "FXBus7",0x8007,0x8,0xffff,0x0 },
        { "FXBus8",0x8008,0x8,0xffff,0x0 },
        { "FXBus9",0x8009,0x8,0xffff,0x0 },
        { "FXBus10",0x800a,0x8,0xffff,0x0 },
        { "FXBus11",0x800b,0x8,0xffff,0x0 },
        { "FXBus12",0x800c,0x8,0xffff,0x0 },
        { "FXBus13",0x800d,0x8,0xffff,0x0 },
        { "FXBus14",0x800e,0x8,0xffff,0x0 },
        { "FXBus15",0x800f,0x8,0xffff,0x0 },
	    { "FXBus16",0x8010,0x8,0xffff,0x0 },
        { "FXBus17",0x8011,0x8,0xffff,0x0 },
        { "FXBus18",0x8012,0x8,0xffff,0x0 },
        { "FXBus19",0x8013,0x8,0xffff,0x0 },
        { "FXBus20",0x8014,0x8,0xffff,0x0 },
        { "FXBus21",0x8015,0x8,0xffff,0x0 },
        { "FXBus22",0x8016,0x8,0xffff,0x0 },
        { "FXBus23",0x8017,0x8,0xffff,0x0 },
        { "FXBus24",0x8018,0x8,0xffff,0x0 },
        { "FXBus25",0x8019,0x8,0xffff,0x0 },
        { "FXBus26",0x801a,0x8,0xffff,0x0 },
        { "FXBus27",0x801b,0x8,0xffff,0x0 },
        { "FXBus28",0x801c,0x8,0xffff,0x0 },
        { "FXBus29",0x801d,0x8,0xffff,0x0 },
        { "FXBus30",0x801e,0x8,0xffff,0x0 },
        { "FXBus31",0x801f,0x8,0xffff,0x0 }
};

dsp_code fxbus_code[]={
	{ 0xff, 0xff, 0xff, 0xff, 0xff } // this is not used
};
