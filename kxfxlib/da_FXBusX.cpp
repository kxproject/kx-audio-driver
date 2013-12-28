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

const char* FXBusX_guid = "d25a7874-7c00-47ca-8ad3-1b13106bde91";
const char* FXBusX_name = "FXBusX";
const char* FXBusX_copyright = "Copyright (c) Dmitry Gorelov and Eugene Gavrilov, 2005-2008. All rights reserved";
const char* FXBusX_engine="kX";
const char* FXBusX_comment="FXBus supporting 24+ bit depths for Emu10K2; $nobypass";
const char* FXBusX_created="21/07/2008";

int FXBusX_itramsize=0,FXBusX_xtramsize=0;

dsp_register_info FXBusX_info[]={
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

        // these 16 should be translated by kernel to hardware KX_FX()
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
        { "FXBus30",0x801e,0x8,0xffff,0x0 },    // note: this is usually needed for AC-3 passthru
        { "FXBus31",0x801f,0x8,0xffff,0x0 },    // note: this is usually needed for AC-3 passthru

        { "mask", 0x8020, 1, 0xffff, 0x3FFF },	// static
        { "tmp2", 0x8022, 3, 0xffff, 0x0},

        // NOTE: optional for is_k8:
        { "tmp1", 0x8021, 3, 0xffff, 0x0},
};

int FXBusX_10k2_info_size=sizeof(FXBusX_info);
int FXBusX_10k8_info_size=sizeof(FXBusX_info)-sizeof(dsp_register_info);

#define rep16(c) \
	c(0),  c(1),  c(2),  c(3),  c(4),  c(5),  c(6),  c(7), \
	c(8),  c(9),  c(10), c(11), c(12), c(13), c(14), c(15)

// ------
// 10k2:
//  highest: send: 0xff, vol: 0x1000 -> ((data<<2)>>4) -> need to <<2 in DSP
//  lowest:  send: 0xc0, vol: 0x8000 -> ((data>>1)>>1)    -> need to >>14 in DSP
// 10k2.5 [10k8]
//  highest: send: 0xe0, vol: 0x8000 -> ((data<<1)>>1) -> nop
//  lowest:  send: 0xc0, vol: 0x8000 -> ((data)>>1)    -> need to >>15 in DSP


//   macints	tmp1,	0,	in_h,	4
//   macs tmp2,   0,    inl,      C_10000
//	 andxor	tmp1,	tmp2,	mask,	tmp1
//	 acc3	out,	tmp1,	in16,	0

#define combine_10k2(r) \
    { 4, 0x8021,     0x2040, KX_FX((r)*2+32),   0x2044 }, \
    { 0, 0x8022,     0x2040, KX_FX((r)*2+32+1), 0x2049 }, \
    { 8, 0x8021,     0x8022, 0x8020,          0x8021 }, \
    { 6, 0x8000+(r), 0x8021, KX_FX(r),        0x2040 }

dsp_code FXBusX_10k2_code[]={
      rep16(combine_10k2)
};

//   macs tmp2,   0,    inl,      C_10000
//	 andxor	tmp2,	tmp2,	mask,	inh
//	 acc3	out,	tmp2,	in16,	0

#define combine_10k8(r) \
    { 0, 0x8022,     0x2040, KX_FX((r)*2+32+1), 0x2049 }, \
    { 8, 0x8022,     0x8022, 0x8020,          KX_FX((r)*2+32) }, \
    { 6, 0x8000+(r), 0x8022, KX_FX(r),        0x2040 }

dsp_code FXBusX_10k8_code[]={
      rep16(combine_10k8)
};


int FXBusX_10k2_code_size=sizeof(FXBusX_10k2_code);
int FXBusX_10k8_code_size=sizeof(FXBusX_10k8_code);
