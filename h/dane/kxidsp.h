// kX Driver Interface / Dane Assembler
// Copyright (c) Max Mikhailov, Eugene Gavrilov, 2001-2005.
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


#ifndef _DANE_KXIDSP_H_
#define _DANE_KXIDSP_H_

#define RFX1_NUMHWCONSTS	38

typedef struct {
	short Instructions;
	short GPRegisters;
	short ITankPairs;
	short XTankPairs;
	long  ITankSize;
	long  XTankSize;
} ENGINERSRC;

const long RFX2_CNSTVALS[] = // KX Hardware Constants Universal ID Search Table
{ 
	0x0,
	0x1,
	0x2,
	0x3,
	0x4,
	0x0, 
	0x0, 
	0x0,
	0x8, 
	0x0,		
	0x10,
	0x00,			
	0x20,
	0x000,			
	0x100,
	0x000, 
	0x0000, 
	0x0000,	
	0x10000,
	0x080000,
	0x0100000,
	0x00000000, 
	0x00000000, 
	0x00000000, 
	0x00000000,
	0x10000000,
	0x20000000,
	0x00000000,				
	0x40000000,
	0x00000000,				
	0x4f1bbcdc,
	0x5a7ef9db,
	0x7fffffff,
	0x80000000,
	0x00000000,		
	0xc0000000,
	0xfffffffe,
	0xffffffff,
};


const short RFX2TO1_HWCONSTS[]= 
{ 
	0x40,
	0x41,
	0x42,
	0x43,
	0x44,
	0x40, 0x40, 0x40,		// consts 2005-2007 are not implemented (?), so we use 0x40 instead
	0x45, 
	0x40,					// const 2009 is not implemented (?), so we use 0x40 instead
	0x46,
	0x40,					// const 200b is not implemented (?), so we use 0x40 instead
	0x47,
	0x40,					// const 200d is not implemented (?), so we use 0x40 instead
	0x48,
	0x40, 0x40, 0x40,		// consts 200f-2011 are not implemented (?), so we use 0x40 instead
	0x49,
	0x4a,
	0x55,
	0x40, 0x40, 0x40, 0x40,	// consts 2015-2018 are not implemented (?), so we use 0x40 instead
	0x4b,
	0x4c,
	0x40,					// const 201b is not implemented (?), so we use 0x40 instead
	0x4d,
	0x40,					// const 201d is not implemented (?), so we use 0x40 instead
	0x53,
	0x54,
	0x4f,
	0x4e,
	0x40,					// const 2022 is not implemented (?), so we use 0x40 instead
	0x52,
	0x51,
	0x50,
};

const short RFX2TO1_SPREGS[]=
{
	0x57,		// CCR
	0x5A,		// IRQ
	0x56,		// ACCUM
	0x58,		// NOISE1
	0x59,		// NOISE2
	0x5B,		// DBAC
};

#endif // _KXIDSP_H_
