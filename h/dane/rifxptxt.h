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


#ifndef _DANE_RIFXPTXT_H_
#define _DANE_RIFXPTXT_H_

#include "kxidsp.h"

#pragma pack(1)

/* Structures */

typedef struct {
	int id;
	int size;
	int tag;
} RFXCHUNK, *pRFXCHUNK;

typedef struct {
	char  OP;
	short A;
	short X;
	short Y;
	short R;
} RFXINSTR, *pRFXINSTR;

typedef struct {
	char  OP;
	short (*OPS)[4];
} RFXINSTRB, *pRFXINSTRB;

typedef struct {
	short Reg;
	short Cell;
} RFXOUTP, *pRFXOUTP;  

typedef short RFXINP, *pRFXINP; 

typedef struct {
    short Reg;
    int  Value;
} RFXGPRS, *pRFXGPRS;

typedef struct {
	short Reg;	// register id with flags
	int  Offset;
	int  Size;
	short Cell;
} RFXTRAM, *pRFXTRAM;

typedef struct {
	short Instructions;
	short Inputs; 
	short Outputs;
	short Statics;
	short ITramAccess;
	short XTramAccess;
	short Temps;
	int  ITramSize;
	int  XTramSize;
	int  Reserved;	// table lookups ?
} RFXPGMRSRC, *pRFXPGMRSRC;

#pragma pack()

/* Constants */

#define RFX_RIFX 'XFIR'
#define RFX_PTXT 'TXTP'
#define RFX_LIST 'TSIL'
#define RFX_INFO 'OFNI'
#define RFX_ICOP 'POCI'
#define RFX_ICRD 'DRCI'
#define RFX_ckid 'dikc'
#define RFX_rsrc 'crsr'
#define RFX_gpri 'irpg'
#define RFX_gprs 'srpg'
#define RFX_tram 'mart'
#define RFX_tabl 'lbat'
#define RFX_intr 'rtni'
#define RFX_xtra 'artx'
#define RFX_patc 'ctap'
#define RFX_inp_ ' pni'
#define RFX_outp 'ptuo'
#define RFX_scal 'lacs'
#define RFX_iscl 'lcsi'
#define RFX_oscl 'lcso'
#define RFX_wscl 'lcsw'
#define RFX_code 'edoc'

#define RFX_EMU8010			"EMU8010"  
#define RFX_RCHIP			"RCHIP"	
#define RFX_EMU10K1			"EMU10K1"

#define RFX_REG_BASE		0x8000
#define RFX2_SPREGS_BASE	0x2200
#define RFX2_HWCONSTS_BASE	0x2000
#define RFX_REG_MASK		0x01ff

#define RFX_TRAM_WRITE		0x10

#define _RFX_TRAM_WRITE		0x1000


const ENGINERSRC RfxRsrc[2] = {
	{0x100, 0x100, 0x80, 0x20, 0x2000, 0x100000},
	{0x100, 0x100, 0x80, 0x20, 0x2000, 0x100000}
};

#endif // _RIFXPTXT_H_
