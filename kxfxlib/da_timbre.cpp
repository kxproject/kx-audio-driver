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

// kX DSP program data
// Simple "Timbre" (based on first-order IIRs, 3dB/octave)  
// Stereo

const char *timbre_copyright = "(c) Max Mikhailov and Eugene Gavrilov, 2002-2004";
const char *timbre_engine = "kX";
const char *timbre_comment = "v0.7, beta";
const char *timbre_created = "2002/02/15";
const char *timbre_guid = "80100004-0ADF-11D6-BFBC-D4F706E10C52";

const char *timbre_name = "Timbre";
int timbre_itramsize = 0,  timbre_xtramsize = 0;   

#ifndef KX_INTERNAL
   #define R_LB1	0x8006      
   #define R_LA1	0x8005
   #define R_HB0	0x8003
   #define R_HB1	0x8004
   #define R_HA1	0x8002

   #define I_BYPS1	0x0006
   #define I_BYPS2	0x000d
   #define R_X		0x800b
   #define R_INL	0x4000
   #define R_INR	0x4001
#endif

dsp_register_info timbre_info[] = {
	{"inl", 0x4000, 0x7, 0xffff, 0x0},
	{"inr", 0x4001, 0x7, 0xffff, 0x0},
	{"outl", 0x8000, 0x8, 0xffff, 0x0},
	{"outr", 0x8001, 0x8, 0xffff, 0x0},
	{"ha1", 0x8002, 0x4, 0xffff, 0xc5a3d1c6},
	{"hb0", 0x8003, 0x4, 0xffff, 0x1fffffff},
	{"hb1", 0x8004, 0x4, 0xffff, 0xf168f471},
	{"la1", 0x8005, 0x4, 0xffff, 0x827d1f6e},
	{"lb1", 0x8006, 0x4, 0xffff, 0x827d1f6e},
	{"lstateleft", 0x8007, 0x1, 0xffff, 0x0},
	{"lstateright", 0x8008, 0x1, 0xffff, 0x0},
	{"hstateleft", 0x8009, 0x1, 0xffff, 0x0},
	{"hstateright", 0x800a, 0x1, 0xffff, 0x0},
	{"x", 0x800b, 0x3, 0xffff, 0x0},
	{"y", 0x800c, 0x3, 0xffff, 0x0},
};

dsp_code timbre_code[] = {
	{0x0, 0x800c, 0x8007, 0x4000, 0x204f},
	{0x0, 0x8007, 0x2040, 0x4000, 0x8006},
	{0x1, 0x8007, 0x2056, 0x800c, 0x8005},
	{0x0, 0x800b, 0x8009, 0x800c, 0x8003},
	{0x0, 0x8009, 0x2046, 0x800c, 0x8004},
	{0x1, 0x8009, 0x2056, 0x800b, 0x8002},
	{0x4, 0x8000, 0x2040, 0x800b, 0x2044},
	{0x0, 0x800c, 0x8008, 0x4001, 0x204f},
	{0x0, 0x8008, 0x2040, 0x4001, 0x8006},
	{0x1, 0x8008, 0x2056, 0x800c, 0x8005},
	{0x0, 0x800b, 0x800a, 0x800c, 0x8003},
	{0x0, 0x800a, 0x2046, 0x800c, 0x8004},
	{0x1, 0x800a, 0x2056, 0x800b, 0x8002},
	{0x4, 0x8001, 0x2040, 0x800b, 0x2044},
};

