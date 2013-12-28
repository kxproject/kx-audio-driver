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


#include "stdafx.h"
#include "dane/dane.h"

int iDane::iminitim(){

    memset(&ObjRsrc,0,sizeof(ObjRsrc));

    ObjRsrc.ITankSize = 0;
    ObjRsrc.XTankSize = 0;
	ObjRsrc.RegistersCount = 0;
	ObjRsrc.ConstantsCount = 0;
	ObjRsrc.InstructionsCount = 0;
	ObjRsrc.Wscl = -1;
	ObjRsrc.Iscl = -1;
	ObjRsrc.Oscl = -1;
	ObjRsrc.Intr = -1;

	memset(RegTable, 0, sizeof(reg) * MAXNUMREGS);
	memset(ConstTable, 0, sizeof(reg) * (MAXNUMREGS));
	memset(CodeTable, 0, sizeof(iminstr) * MAXNUMINSTRS);

	return 0;
}

int iDane::imsetrscrstr(int id, const char* chars){

	char* rsrc = (char*) &ObjRsrc;
	strcpy(&(rsrc[(id - K_NAME) << 7]), chars); // 7 depends on KX_MAX_STRING

	return 0;
}

int iDane::imaddtramsize(int id, int size){

	int* tramsize = &ObjRsrc.ITankSize;
	tramsize[id - K_ITRAMSIZE] += size;

	return 0;
}

int iDane::imnewreg(reg** ppreg){

	if (ObjRsrc.RegistersCount >= MAXNUMREGS) return 1; // error!!!

	*ppreg = &RegTable[ObjRsrc.RegistersCount];
	(*ppreg)->tindex = ObjRsrc.RegistersCount;

	ObjRsrc.RegistersCount++;

	return 0;
}

int iDane::imnewcnst(reg** ppreg){

	if (ObjRsrc.ConstantsCount >= MAXNUMCNSTS) return 1; // error!!!

	*ppreg = &ConstTable[ObjRsrc.ConstantsCount];
	(*ppreg)->tindex = ObjRsrc.ConstantsCount | RT_CONST;

	ObjRsrc.ConstantsCount++;

	return 0;
}

int iDane::imnewinstr(iminstr** ppiminstr){

	if (ObjRsrc.InstructionsCount >= MAXNUMINSTRS) return 1; // error!!!

	*ppiminstr = &CodeTable[ObjRsrc.InstructionsCount];

	ObjRsrc.InstructionsCount++;

	return 0;
}

int iDane::imsetscal(reg* preg, int pos){

	int* xscl = (&ObjRsrc.Wscl) + pos; // get ptr to Xscl[pos] (Wscl, Iscl, and Oscl depending on pos)

	if (*xscl > 0) return 1;

	*xscl = preg->tindex;
	return 0;
}

int iDane::imsetintr(reg* ppreg){

	if (ObjRsrc.Intr >= 0) return 1;

	ObjRsrc.Intr = ppreg->tindex;
	return 0;
}

int iDane::imregbyname(const char* symbol){

	for (int i = 0; i < ObjRsrc.ConstantsCount; i++){
		if (!stricmp(symbol, ConstTable[i].symbol)) return i | RT_CONST;
	}

	for (int i = 0; i < ObjRsrc.RegistersCount; i++){
		if (!stricmp(symbol, RegTable[i].symbol)) return i;
	}
	
	return -1;
}

int iDane::imregbyid(int id, reg** ppreg){

	if (id & RT_CONST) {
		*ppreg = &ConstTable[id & RT_ID_MASK];
	}
	else {
		*ppreg = &RegTable[id];
	}
	return 0;
}

int iDane::imcnstbyvalue(int value){

	for (int i = 0; i < ObjRsrc.ConstantsCount; i++){
		if (ConstTable[i].tag == value) return i | RT_CONST;
	}

	// if const isn't found let's auto create const with requested value
	reg* preg;

	if (imnewcnst(&preg)) return -1; // out of constants
	preg->type = K_RDEC_BIT | K_CONST | K_RDATA_BIT;
	preg->tag = value;
	// strcpy(preg->symbol, "_AC");

        char tmp_str[KX_MAX_STRING]; sprintf(tmp_str,"_AC%d",value);
        strncpy(preg->symbol,tmp_str,MAX_GPR_NAME-1);
	
	return preg->tindex;
}

int iDane::imtanksize(int id){

	return (&ObjRsrc.ITankSize)[id - K_IDELAY];
}

int iDane::gfixconsts(){

	for (int i = 0; i < ObjRsrc.ConstantsCount; i++){
		ConstTable[i].id = _constidbyvalue(ConstTable[i].tag);
		if (ConstTable[i].id < 0){
			// constant does not exist in hardware, making static register
			ConstTable[i].type = K_RDEC_BIT | K_STATIC | K_RDATA_BIT;
		}
	}
	return 0;
}

int iDane::_constid(int id){
	/*if (_fmt == G1_RFX1)*/ return RFX2TO1_HWCONSTS[id];
	// return (id | RFX2_HWCONSTS_BASE);
};

int iDane::_constidbyvalue(int value){

	#define CNSTFUZZYCOEF	0x4000

	int cnstrange = value / CNSTFUZZYCOEF;

	/* really dirty fixes - remove later */
	if (value >= (signed) 0x7ffe0000l) cnstrange = 0;
	if (value <= (signed) 0x8001ffff) cnstrange = 0;

	for (int i = 0; i < RFX1_NUMHWCONSTS; i++){
		if ((RFX2_CNSTVALS[i] <= (value + cnstrange)) & (RFX2_CNSTVALS[i] >= (value - cnstrange))) return _constid(i);
	}
	return -1;
}
