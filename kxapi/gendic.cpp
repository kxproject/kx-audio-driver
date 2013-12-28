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

#if defined(WIN32)
    #define dane_alloc(a) LocalAlloc(LMEM_FIXED|LMEM_ZEROINIT,a)
    #define dane_free(a)  LocalFree(a)
#elif defined(__APPLE__)
    #define dane_alloc(a) malloc(a)
    #define dane_free(a)  free(a)
#endif

int assemble_dane(char *buf,kString *err,char *name,dsp_code **code,int *code_size,
				  dsp_register_info **info,int *info_size,int *itramsize,int *xtramsize,
				  char *copyright,char *engine,char *created,char *comment,char *guid);

int assemble_dane(char *buf,kString *err,char *name,dsp_code **code,int *code_size,
  dsp_register_info **info,int *info_size,int *itramsize,int *xtramsize,
  char *copyright,char *engine,char *created,char *comment,char *guid)
{
 iDane *dane=new iDane;

 if(dane==0)
  return -11;

 dane->err=err;

 int ret=dane->paparse(buf);
 if(ret==0) // ok
 {
   ret=dane->d3build(name,code,code_size,info,info_size,itramsize,xtramsize,copyright,
   	engine,created,comment,guid);
 }

 delete dane;
 return ret;
}

int iDane::d3build(char *name,dsp_code **code,int *code_size,
  dsp_register_info **info,int *info_size,int *itramsize,int *xtramsize,
  char *copyright,char *engine,char *created,char *comment,char *guid){

	gfixconsts(); // splits constants in ImObj into Hardware and GPR (static) ones

	// free them with caller!
	_code_size = ObjRsrc.InstructionsCount;
	_code = (dsp_code*) dane_alloc(_code_size * sizeof(dsp_code));

	// !!! for INFO let's allocate twice more 
	// since ONE ObjRegister can produce TWO tram registers!!!
	_info_size = ObjRsrc.RegistersCount * 2 + ObjRsrc.ConstantsCount; 
	_info = (dsp_register_info*) dane_alloc(_info_size * sizeof(dsp_register_info));
	dbginfo = ((int*) _info) - 8;
	_info_size = 0; // will be filled later with real size

	_vir_id = 0;
	
	_makeinputs();
	_makeoutputs();
	_makestatics();
	_maketemps();
	_makedelays();
	_makecode();

	/*******/
	*code = _code;
	*code_size = _code_size;
	*info = _info;
	*info_size = _info_size;

	*itramsize = ObjRsrc.ITankSize;
	*xtramsize = ObjRsrc.XTankSize;

	*itramsize = ObjRsrc.ITankSize;
	*xtramsize = ObjRsrc.XTankSize;

        strncpy(name,ObjRsrc.Name,KX_MAX_STRING);
        strncpy(copyright,ObjRsrc.Copyright,KX_MAX_STRING);
        strncpy(engine,ObjRsrc.Engine,KX_MAX_STRING);
        strncpy(created,ObjRsrc.Created,KX_MAX_STRING);
        strncpy(guid,ObjRsrc.Guid,KX_MAX_STRING);
        strncpy(comment,ObjRsrc.Comment,KX_MAX_STRING);

        // code_size and info_size are in bytes, not items!
	*code_size*=(int)sizeof(dsp_code);
	*info_size*=(int)sizeof(dsp_register_info);

	return 0;
}


int iDane::_makeinputs(){

	for (int i = 0; i < ObjRsrc.RegistersCount; i++){
		if ((RegTable[i].type & K_RDEC_MASK) == K_INPUT){
			strncpy(_info[_info_size].name, RegTable[i].symbol, MAX_GPR_NAME); // dane supports 32 chars names so some may be duplicated 
			_info[_info_size].num = (unsigned short) (0x4000 + _info_size);
			_info[_info_size].type = GPR_INPUT;
			_info[_info_size].translated = DSP_REG_NOT_TRANSLATED;
			_info[_info_size].p = 0;

			RegTable[i].id = _info[_info_size].num;

			_info_size++;
		}
	}

	return 0;
}

int iDane::_makeoutputs(){

	for (int i = 0; i < ObjRsrc.RegistersCount; i++){
		if ((RegTable[i].type & K_RDEC_MASK) == K_OUTPUT){
			strncpy(_info[_info_size].name, RegTable[i].symbol, MAX_GPR_NAME);
			_info[_info_size].num = (unsigned short) (0x8000 + _vir_id);
			_info[_info_size].type = GPR_OUTPUT;
			_info[_info_size].translated = DSP_REG_NOT_TRANSLATED;
			_info[_info_size].p = 0;

			RegTable[i].id = _info[_info_size].num;

			_vir_id++;
			_info_size++;
		}
	}

	return 0;
}

int iDane::_makestatics(){

	for (int i = 0; i < ObjRsrc.RegistersCount; i++){
		if (((RegTable[i].type & K_RDEC_MASK) == K_STATIC)|
		((RegTable[i].type & K_RDEC_MASK) == K_CONTROL)){
			strncpy(_info[_info_size].name, RegTable[i].symbol, MAX_GPR_NAME);
			_info[_info_size].num = (unsigned short) (0x8000 + _vir_id);

			int twin_reg=0;
			switch (RegTable[i].type & (K_RDEC_MASK | K_COARSE)){
			case K_STATIC:
				_info[_info_size].type = GPR_STATIC;
				break;
			case K_CONTROL:
				_info[_info_size].type = GPR_CONTROL;
				break;
			case K_STATIC | K_COARSE:
			case K_CONTROL | K_COARSE:
				_info[_info_size].type = GPR_ITRAM;
				twin_reg=1; // tram needs two registers
				break;
			}
			_info[_info_size].translated = DSP_REG_NOT_TRANSLATED;
			_info[_info_size].p = RegTable[i].tag;

			RegTable[i].id = _info[_info_size].num;

			_vir_id++;
			_info_size++;

			if(twin_reg)
			{
        			// address
        			sprintf(_info[_info_size].name, "&%.11s", RegTable[i].symbol); // 11 is MAX_GPR_NAME-1

        			_info[_info_size].num = (unsigned short) (0x8000 + _vir_id);
        			_info[_info_size].type = GPR_TRAMA;
        			_info[_info_size].translated = DSP_REG_NOT_TRANSLATED;
        			_info[_info_size].p = RegTable[i].tag;

        			_vir_id++;
        			_info_size++;
			}
		}
	}

	for (int i = 0; i < ObjRsrc.ConstantsCount; i++){
		if ((ConstTable[i].type & K_RDEC_MASK) == K_STATIC){
			strncpy(_info[_info_size].name, ConstTable[i].symbol, MAX_GPR_NAME);
			_info[_info_size].num = (unsigned short) (0x8000 + _vir_id);
			_info[_info_size].type = GPR_CONST;
			_info[_info_size].translated = DSP_REG_NOT_TRANSLATED;
			_info[_info_size].p = ConstTable[i].tag;

			ConstTable[i].id = _info[_info_size].num;

			_vir_id++;
			_info_size++;
		}
	}

	return 0;
}

int iDane::_maketemps(){

	for (int i = 0; i < ObjRsrc.RegistersCount; i++){
		if ((RegTable[i].type & K_RDEC_MASK) == K_TEMP){
			strncpy(_info[_info_size].name, RegTable[i].symbol, MAX_GPR_NAME);
			_info[_info_size].num = (unsigned short) (0x8000 + _vir_id);
			_info[_info_size].type = GPR_TEMP;
			_info[_info_size].translated = DSP_REG_NOT_TRANSLATED;
			_info[_info_size].p = 0;

			RegTable[i].id = _info[_info_size].num;

			_vir_id++;
			_info_size++;
		}
	}

	return 0;
}

int iDane::_makedelays(){

	// !!! I and X registers are not sorted

	for (int i = 0; i < ObjRsrc.RegistersCount; i ++){
		if ((RegTable[i].type & K_RDEC_MASK) >= K_IDELAY){
			//data
			strncpy(_info[_info_size].name, RegTable[i].symbol, MAX_GPR_NAME);
			_info[_info_size].num = (unsigned short) (0x8000 + _vir_id);
			_info[_info_size].type = (byte)((RegTable[i].type & K_RDEC_MASK) + 2); // K_*DELAY -> GPR_*TRAM
			_info[_info_size].type |= (RegTable[i].type & K_RPROP_MASK) + 0x10; // Quick fix: no RSAW!
			_info[_info_size].translated = DSP_REG_NOT_TRANSLATED;
			_info[_info_size].p = RegTable[i].tag; // is this required? -- yes; it is required

			RegTable[i].id = _info[_info_size].num;

			_vir_id++;
			_info_size++;

			// address
			sprintf(_info[_info_size].name, "&%.11s", RegTable[i].symbol); // 11 is MAX_GPR_NAME-1

			_info[_info_size].num = (unsigned short) (0x8000 + _vir_id);
			_info[_info_size].type = GPR_TRAMA;
			_info[_info_size].translated = DSP_REG_NOT_TRANSLATED;
			_info[_info_size].p = RegTable[i].tag;

			_vir_id++;
			_info_size++;
		}
	}

	return 0;
}


int iDane::_makecode(){

	int i, j;
	reg *preg;

	for (i = 0; i < ObjRsrc.InstructionsCount; i++){
		_code[i].op = (char) CodeTable[i].opcode;
		for (j = 0; j < NUMOPS; j++){
			switch (CodeTable[i].ops[j] & 0xf000){
			case RT_HWR:
				((word *) &_code[i].r)[j] = (word)(RFX2TO1_SPREGS[(CodeTable[i].ops[j] & RT_ID_MASK) - 1] + KX_CONST);
				break;
			case RT_CONST:
				preg = &ConstTable[CodeTable[i].ops[j] & RT_ID_MASK];
				if ((preg->type & K_RDEC_MASK) == K_STATIC) goto notconst;
				((word *) &_code[i].r)[j] = (unsigned short) (KX_CONST + preg->id); // logical id? - :)))
				break;
			default:
				preg = &RegTable[CodeTable[i].ops[j]];
			notconst:
				((word *) &_code[i].r)[j] = (unsigned short) preg->id;
				if (CodeTable[i].tags[j]) ((unsigned short*) &_code[i].r)[j]++; // if by address
			}
		}
	}
	

	return 0;
}

