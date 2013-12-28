// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov and Max Mikhailov, 2001-2005.
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
// Patch name: 'epiloglt_k1'

const char *epiloglt_k1_copyright="Copyright (c) Eugene Gavrilov, 2003-2004.";
const char *epiloglt_k1_engine="kX";
const char *epiloglt_k1_comment="epilog lite - no resources used";
const char *epiloglt_k1_created="05/11/2003";
const char *epiloglt_k1_guid="85e97848-0004-4006-a500-5a6a03b1bf09";

const char *epiloglt_k1_name="epiloglt_k1";
int epiloglt_k1_itramsize=0,epiloglt_k1_xtramsize=0;

dsp_register_info epiloglt_k1_info[]={
	{ "ac97_l",0x4000,0x7,0xffff,0x0 },
	{ "ac97_r",0x4001,0x7,0xffff,0x0 },
	{ "spdif0_l",0x4002,0x7,0xffff,0x0 },
	{ "spdif0_r",0x4003,0x7,0xffff,0x0 },
	{ "spdif1_l",0x4004,0x7,0xffff,0x0 },
	{ "spdif1_r",0x4005,0x7,0xffff,0x0 },
	{ "spdif2_l",0x4006,0x7,0xffff,0x0 },
	{ "spdif2_r",0x4007,0x7,0xffff,0x0 },
	{ "spdif3_l",0x4008,0x7,0xffff,0x0 },
	{ "spdif3_r",0x4009,0x7,0xffff,0x0 },
	{ "recl",0x400a,0x7,0xffff,0x0 },
	{ "recr",0x400b,0x7,0xffff,0x0 },
	{ "rec_mic",0x400c,0x7,0xffff,0x0 },
	{ "out_13",0x400d,0x7,0xffff,0x0 },
	{ "out_14",0x400e,0x7,0xffff,0x0 },
	{ "out_15",0x400f,0x7,0xffff,0x0 },
	{ "asio0",0x4010,0x7,0xffff,0x0 },
	{ "asio1_cnt",0x4011,0x7,0xffff,0x0 },
	{ "asio2_lfe",0x4012,0x7,0xffff,0x0 },
	{ "asio3",0x4013,0x7,0xffff,0x0 },
	{ "asio4",0x4014,0x7,0xffff,0x0 },
	{ "asio5",0x4015,0x7,0xffff,0x0 },
	{ "asio6",0x4016,0x7,0xffff,0x0 },
	{ "asio7",0x4017,0x7,0xffff,0x0 },
	{ "asio8",0x4018,0x7,0xffff,0x0 },
	{ "asio9",0x4019,0x7,0xffff,0x0 },
	{ "asio10",0x401a,0x7,0xffff,0x0 },
	{ "asio11",0x401b,0x7,0xffff,0x0 },
	{ "asio12",0x401c,0x7,0xffff,0x0 },
	{ "asio13",0x401d,0x7,0xffff,0x0 },
	{ "asio14",0x401e,0x7,0xffff,0x0 },
	{ "asio15",0x401f,0x7,0xffff,0x0 },
};

dsp_code epiloglt_k1_code[]={ { 0xff } };

