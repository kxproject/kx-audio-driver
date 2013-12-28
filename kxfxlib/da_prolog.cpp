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
// Patch name: 'Prolog'
#ifndef PROLOG_DESCR
#define PROLOG_DESCR

const char *prolog_name="prolog";
const char *prolog_engine="kX";
const char *prolog_comment="new version with attenuation/amplification; $nobypass";
const char *prolog_created="27 Sep 2003";
const char *prolog_copyright="(c) Eugene Gavrilov, 2001-2008";
const char *prolog_guid="631d8de5-11bc-4c3d-a0d2-f079977fd184";

#endif // PROLOG_DESCR

#ifdef PROLOG_SOURCE
// external variables: dsp_flags, is_10k2, is_aps

#if defined(_MSC_VER)
#pragma warning(disable:4189)
#endif
	// NEW: all the controls assume div4 attenuation built-in

	output(in0,"in0");
	output(in1,"in1");
	control_gpr(in0vol,0x0,"in0vol");
	control_gpr(in1vol,0x0,"in1vol");
	output(in2,"in2");
	output(in3,"in3");
	control_gpr(in2vol,0x0,"in2vol");
	control_gpr(in3vol,0x0,"in3vol");
	output(in4,"in4");
	output(in5,"in5");
	control_gpr(in4vol,0x0,"in4vol");
	control_gpr(in5vol,0x0,"in5vol");
	output(in6,"in6");
	output(in7,"in7");
	control_gpr(in6vol,0x0,"in6vol");
	control_gpr(in7vol,0x0,"in7vol");
	output(in8,"in8");
	output(in9,"in9");
	control_gpr(in8vol,0x0,"in8vol");
	control_gpr(in9vol,0x0,"in9vol");
	output(in10,"in10");
	output(in11,"in11");
	control_gpr(in10vol,0x0,"in10vol");
	control_gpr(in11vol,0x0,"in11vol");
	output(in12,"in12");
	output(in13,"in13");
	control_gpr(in12vol,0x0,"in12vol");
	control_gpr(in13vol,0x0,"in13vol");

// Code
//    int input_r;

    if(is_edsp)
     input_r=KX_E32IN(0x0);
    else
     input_r=KX_IN(0x0);

	 OP(MACS, in0 ,  C_0,  (input_r+0x0),  in0vol);
	 OP(MACS, in1 ,  C_0,  (input_r+0x1),  in1vol);
	 OP(MACS, in2 ,  C_0,  (input_r+0x2),  in2vol);
	 OP(MACS, in3 ,  C_0,  (input_r+0x3),  in3vol);
	 OP(MACS, in4 ,  C_0,  (input_r+0x4),  in4vol);
	 OP(MACS, in5 ,  C_0,  (input_r+0x5),  in5vol);
	 OP(MACS, in6 ,  C_0,  (input_r+0x6),  in6vol);
	 OP(MACS, in7 ,  C_0,  (input_r+0x7),  in7vol);
	 OP(MACS, in8 ,  C_0,  (input_r+0x8),  in8vol);
	 OP(MACS, in9 ,  C_0,  (input_r+0x9),  in9vol);
	 OP(MACS, in10 ,  C_0,  (input_r+0xa),  in10vol);
	 OP(MACS, in11 ,  C_0,  (input_r+0xb),  in11vol);
	 OP(MACS, in12 ,  C_0,  (input_r+0xc),  in12vol);
	 OP(MACS, in13 ,  C_0,  (input_r+0xd),  in13vol);

#endif // PROLOG_SOURCE
