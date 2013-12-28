// kX SDK:
// kX Audio Driver / kX Audio Driver Plugin Interface
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

// note: do not #include this file unless you really wish to get these text strings

#ifndef _DSP_NAMES_H
#define _DSP_NAMES_H 

const char op_codes[TOTAL_OPCODES+1][9]=
  {
    "macs",
    "macsn",
    "macw",
    "macwn",

    "macints",
    "macintw",
    
    "acc3",
    "macmv",
    "andxor",
    "tstneg",
    "limit",
    "limitn",
    "log",
    "exp",
    "interp",
    "skip",
    "NotAnOp"
  };
const char * op_helps[TOTAL_OPCODES]=
{
   "0x0 (MACS) : R = A + (X * Y >> 31)   ; saturation",
   "0x1 (MACSN) : R = A + (-X * Y >> 31)  ; saturation",
   "0x2 (MACW) : R = A + (X * Y >> 31)   ; wraparound",
   "0x3 (MACWN) : R = A + (-X * Y >> 31)  ; wraparound",
   "0x4 (MACINTS) : R = A + X * Y  ; saturation",
   "0x5 (MACINTW) : R = A + X * Y  ; wraparound (31-bit)",
   "0x6 (ACC3) : R = A + X + Y  ; saturation",
   "0x7 (MACMV) : R = A, acc += X * Y >> 31",
   "0x8 (ANDXOR) : R = (A & X) ^ Y",
   "0x9 (TSTNEG) : R = (A >= Y) ? X : ~X",
   "0xa (LIMIT) : R = (A >= Y) ? X : Y",
   "0xb (LIMITN): R = (A < Y) ? X : Y",
   "0xc (LOG) : ...",
   "0xd (EXP) : ...",
   "0xe (INTERP) : R = A + (X * (Y - A) >> 31)  ; saturation",
   "0xf (SKIP) : R,CCR,CC_TEST,COUNT  "
};
const char *operand_names_k1[0x5e]=
{
 "KX_FX(0x0)"	
,"KX_FX(0x1)"	
,"KX_FX(0x2)"	
,"KX_FX(0x3)"	
,"KX_FX(0x4)"  	
,"KX_FX(0x5)"	
,"KX_FX(0x6)"	
,"KX_FX(0x7)"	
,"KX_FX(0x8)"	
,"KX_FX(0x9)"  	
,"KX_FX(0xA)"	
,"KX_FX(0xB)"	
,"KX_FX(0xC)"	
,"KX_FX(0xD)"	
,"KX_FX(0xE)" 	
,"KX_FX(0xF)"  	

// Inputs
,"KX_IN(0)"
,"KX_IN(1)"

,"KX_IN(2)"	
,"KX_IN(3)"   	

,"KX_IN(4)"	
,"KX_IN(5)"	

,"KX_IN(6)"	
,"KX_IN(7)"	

,"KX_IN(8)"	
,"KX_IN(9)"	

,"KX_IN(0xa)"	
,"KX_IN(0xb)"	

,"KX_IN(0xc)"	
,"KX_IN(0xd)"	

,"KX_IN(0xe)"	
,"KX_IN(0xf)"	

// Outputs
,"KX_OUT(0)"
,"KX_OUT(1)"	

,"KX_OUT(2)"
,"KX_OUT(3)"

,"KX_OUT(4)"	
,"KX_OUT(5)"	

,"KX_OUT(6)"	
,"KX_OUT(7)"	

,"KX_OUT(8)"		
,"KX_OUT(9)"		

,"KX_OUT(0xa)" 
,"KX_OUT(0xb)"	
,"KX_OUT(0xc)"	
,"KX_OUT(0xd)"	
,"KX_OUT(0xe)"	
,"KX_OUT(0xf)"	

,"KX_FX2(0)"
,"KX_OUT(0x11)"	// shared for sb060 etc. (5.1 10k1)
,"KX_OUT(0x12)"
,"KX_FX2(3)"
,"KX_FX2(4)"
,"KX_FX2(5)"
,"KX_FX2(6)"
,"KX_FX2(7)"
,"KX_FX2(8)"
,"KX_FX2(9)"
,"KX_FX2(0xa)"
,"KX_FX2(0xb)"
,"KX_FX2(0xc)"
,"KX_FX2(0xd)"
,"KX_FX2(0xe)"
,"KX_FX2(0xf)"

// 0x40
,"0x0"    // 00000000
,"0x1"    // 00000001
,"0x2"    // 00000002
,"0x3"    // 00000003
,"0x4"    // 00000004
,"0x8"    // 00000008
,"0x10"   // 00000010
,"0x20"   // 00000020
,"0x100"  // 00000100
,"0x10000"		 // 00010000
,"0x80000"		 // 00080000 -- note: 10k2: different (0x800!)
,"0x10000000"	 // 10000000
,"0x20000000"	 // 20000000
,"0x40000000"	 // 40000000
,"0x80000000"	 // 80000000
,"0x7fffffff"	 // 7fffffff
// 0x50
,"0xffffffff"	 // ffffffff
,"0xfffffffe"	 // fffffffe
,"0xc0000000"	 // c0000000
,"0x4f1bbcdc"	 // 4f1bbcdc
,"0x5a7ef9db"	 // 5a7ef9db
,"0x00100000"	 // 00100000 (?)

// 0x56
,"accum"	 // accum, accumulator
,"ccr"	 // ccr, condition code register
,"noise1"   // noise source
,"noise2"	 // noise source
,"irq"   // irqreg, set the msb to generate a fx interrupt
,"dbac"
,"unknown5c??"
,"unknown5d??"
};

const char *operand_names_k2[0xee]=
{
 "KX_FX(0x0)"
,"KX_FX(0x1)"
,"KX_FX(0x2)"
,"KX_FX(0x3)"
,"KX_FX(0x4)"
,"KX_FX(0x5)"
,"KX_FX(0x6)"
,"KX_FX(0x7)"
,"KX_FX(0x8)"
,"KX_FX(0x9)"
,"KX_FX(0xA)"
,"KX_FX(0xB)"
,"KX_FX(0xC)"
,"KX_FX(0xD)"
,"KX_FX(0xE)"
,"KX_FX(0xF)"
,"KX_FX(0x10)"
,"KX_FX(0x11)"
,"KX_FX(0x12)"
,"KX_FX(0x13)"
,"KX_FX(0x14)"
,"KX_FX(0x15)"
,"KX_FX(0x16)"
,"KX_FX(0x17)"
,"KX_FX(0x18)"
,"KX_FX(0x19)"
,"KX_FX(0x1A)"
,"KX_FX(0x1B)"
,"KX_FX(0x1C)"
,"KX_FX(0x1D)"
,"KX_FX(0x1E)"
,"KX_FX(0x1F)"
,"KX_FX(0x20)"
,"KX_FX(0x21)"
,"KX_FX(0x22)"
,"KX_FX(0x23)"
,"KX_FX(0x24)"
,"KX_FX(0x25)"
,"KX_FX(0x26)"
,"KX_FX(0x27)"
,"KX_FX(0x28)"
,"KX_FX(0x29)"
,"KX_FX(0x2A)"
,"KX_FX(0x2B)"
,"KX_FX(0x2C)"
,"KX_FX(0x2D)"
,"KX_FX(0x2E)"
,"KX_FX(0x2F)"
,"KX_FX(0x30)"
,"KX_FX(0x31)"
,"KX_FX(0x32)"
,"KX_FX(0x33)"
,"KX_FX(0x34)"
,"KX_FX(0x35)"
,"KX_FX(0x36)"
,"KX_FX(0x37)"
,"KX_FX(0x38)"
,"KX_FX(0x39)"
,"KX_FX(0x3A)"
,"KX_FX(0x3B)"
,"KX_FX(0x3C)"
,"KX_FX(0x3D)"
,"KX_FX(0x3E)"
,"KX_FX(0x3F)"

// Inputs
// Inputs
,"KX_IN(0)"	
,"KX_IN(1)"	

,"KX_IN(2)"
,"KX_IN(3)"

,"KX_IN(6)"	// note: swapped with '4/5' (compared to 10k1)
,"KX_IN(7)"

,"KX_IN(4)"
,"KX_IN(5)"

,"KX_IN(8)"	
,"KX_IN(9)"	

,"KX_IN(0xa)"	
,"KX_IN(0xb)"	

,"KX_IN(0xc)"	
,"KX_IN(0xd)"	

,"KX_IN(0xe)"
,"KX_IN(0xf)"	

// E-mu32 inputs / p16v inputs for a2-based boards
,"KX_E32IN(0x0)"
,"KX_E32IN(0x1)"
,"KX_E32IN(0x2)"
,"KX_E32IN(0x3)"
,"KX_E32IN(0x4)"
,"KX_E32IN(0x5)"
,"KX_E32IN(0x6)"
,"KX_E32IN(0x7)"
,"KX_E32IN(0x8)"
,"KX_E32IN(0x9)"
,"KX_E32IN(0xA)"
,"KX_E32IN(0xB)"
,"KX_E32IN(0xC)"
,"KX_E32IN(0xD)"
,"KX_E32IN(0xE)"
,"KX_E32IN(0xF)"

// Outputs
,"KX_OUT(2)"	// 0x60,61 - spdif0 - spdif out
,"KX_OUT(3)"	

,"KX_OUT(4)"	// 0x62,63 - spdif1 - dig. center/lfe
,"KX_OUT(5)"	

,"KX_OUT(6)"	// 0x64,65 - spdif2 - external headphones
,"KX_OUT(7)"	

,"KX_OUT(0x28)"	// 0x66,67 - spdif3 - dig. rear
,"KX_OUT(0x29)"	

,"KX_OUT(0)"    // 0x68,69 - i2s0 - analog front
,"KX_OUT(1)"		

,"KX_OUT(0x11)" // 0x6a,6b - i2s1 - center/lfe
,"KX_OUT(0x12)"
,"KX_OUT(0x30)" // 0x6c,6d - i2s2 - side left/right
,"KX_OUT(0x31)"
,"KX_OUT(8)"    // 0x6e,6f - i2s3 - analog rear
,"KX_OUT(9)"

,"KX_OUT(0x20)"
,"KX_OUT(0x21)"
,"KX_OUT(0x72)"
,"KX_OUT(0x73)"
,"KX_OUT(0x74)"
,"KX_OUT(0x75)"

,"KX_OUT(0xa)" 
,"KX_OUT(0xb)"	
,"KX_OUT(0xc)"	

,"UNKNOWN_?79"
,"UNKNOWN_?7A"
,"UNKNOWN_?7B"
,"UNKNOWN_?7C"
,"UNKNOWN_?7D"
,"UNKNOWN_?7E"
,"UNKNOWN_?7F"

,"KX_FX2(0x0)"
,"KX_FX2(0x1)"
,"KX_FX2(0x2)"
,"KX_FX2(0x3)"
,"KX_FX2(0x4)"
,"KX_FX2(0x5)"
,"KX_FX2(0x6)"
,"KX_FX2(0x7)"
,"KX_FX2(0x8)"
,"KX_FX2(0x9)"
,"KX_FX2(0xA)"
,"KX_FX2(0xB)"
,"KX_FX2(0xC)"
,"KX_FX2(0xD)"
,"KX_FX2(0xE)"
,"KX_FX2(0xF)"

,"KX_FX2(0x10)"
,"KX_FX2(0x11)"
,"KX_FX2(0x12)"
,"KX_FX2(0x13)"
,"KX_FX2(0x14)"
,"KX_FX2(0x15)"
,"KX_FX2(0x16)"
,"KX_FX2(0x17)"
,"KX_FX2(0x18)"
,"KX_FX2(0x19)"
,"KX_FX2(0x1A)"
,"KX_FX2(0x1B)"
,"KX_FX2(0x1C)"
,"KX_FX2(0x1D)"
,"KX_FX2(0x1E)"
,"KX_FX2(0x1F)"

// 0xa0
,"KX_E32OUT(0x10)"
,"KX_E32OUT(0x11)"
,"KX_E32OUT(0x12)"
,"KX_E32OUT(0x13)"
,"KX_E32OUT(0x14)"
,"KX_E32OUT(0x15)"
,"KX_E32OUT(0x16)"
,"KX_E32OUT(0x17)"
,"KX_E32OUT(0x18)"
,"KX_E32OUT(0x19)"
,"KX_E32OUT(0x1A)"
,"KX_E32OUT(0x1B)"
,"KX_E32OUT(0x1C)"
,"KX_E32OUT(0x1D)"
,"KX_E32OUT(0x1E)"
,"KX_E32OUT(0x1F)"

// 0xb0
,"KX_E32OUT(0x0)"
,"KX_E32OUT(0x1)"
,"KX_E32OUT(0x2)"
,"KX_E32OUT(0x3)"
,"KX_E32OUT(0x4)"
,"KX_E32OUT(0x5)"
,"KX_E32OUT(0x6)"
,"KX_E32OUT(0x7)"
,"KX_E32OUT(0x8)"
,"KX_E32OUT(0x9)"
,"KX_E32OUT(0xA)"
,"KX_E32OUT(0xB)"
,"KX_E32OUT(0xC)"
,"KX_E32OUT(0xD)"
,"KX_E32OUT(0xE)"
,"KX_E32OUT(0xF)"

// 0xc0
,"0x0"    // 00000000
,"0x1"    // 00000001
,"0x2"    // 00000002
,"0x3"    // 00000003
,"0x4"    // 00000004
,"0x8"    // 00000008
,"0x10"   // 00000010
,"0x20"   // 00000020
,"0x100"  // 00000100
,"0x10000"		 // 00010000
,"0x800"		 // 000800		// NOTE: THE ONLY DIFFERENT CONSTANT!
,"0x10000000"	 // 10000000
,"0x20000000"	 // 20000000
,"0x40000000"	 // 40000000
,"0x80000000"	 // 80000000
,"0x7fffffff"	 // 7fffffff
// 0xd0
,"0xffffffff"	 // ffffffff
,"0xfffffffe"	 // fffffffe
,"0xc0000000"	 // c0000000
,"0x4f1bbcdc"	 // 4f1bbcdc
,"0x5a7ef9db"	 // 5a7ef9db
,"0x00100000"	 // 00100000 (?)

// 0xd6      
,"accum"	 // accum, accumulator
,"ccr"	 // ccr, condition code register
,"noise1"   // noise source
,"noise2"	 // noise source
,"irq"   // irqreg, set the msb to generate a fx interrupt
,"dbac"
,"unknown_dc??"
,"unknown_dd??"

,"unknown_de??"
,"unknown_df??"
,"unknown_e0??"
,"unknown_e1??"
,"unknown_e2??"
,"unknown_e3??"
,"unknown_e4??"
,"unknown_e5??"
,"unknown_e6??"
,"unknown_e7??"
,"unknown_e8??"
,"unknown_e9??"
,"unknown_ea??"
,"0x1f"
,"unknown_ec??"
,"unknown_ed??"
};

#endif
