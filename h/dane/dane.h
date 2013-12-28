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


#ifndef _DANE_H_
#define _DANE_H_

#include "danestd.h"
#include "error.h"
#include "imobj.h"
#include "scanner.h"
#include "langdef.h"
#include "rifxptxt.h"
#include "parser.h"
#include "error.h"

class iDane
{
	public:
		// scanner data:
		char *hsrc;
		int bufpos;
		int iscomments, cline;
		char empty[2];
		
		// gendic.cpp - related
		int _vir_id;
		int* dbginfo;
		dsp_code *_code;
		int _code_size;
		dsp_register_info *_info;
		int _info_size; 
		
		iminstr CodeTable[MAXNUMINSTRS];
		reg RegTable[MAXNUMREGS];
		reg ConstTable[MAXNUMREGS];
		IMPGMRSRC ObjRsrc;
		
		// declare all the functions
		int _makeinputs();
		int _makeoutputs();
		int _makestatics();
		int _maketemps();
		int _makedelays();
		int _makecode();
		
		int d3build(char *name,dsp_code **code,int *code_size,
					 dsp_register_info **info,int *info_size,int *itramsize,int *xtramsize,
					 char *copyright,char *engine,char *created,char *comment,char *guid);
		
		// scanner
		int scnexttoken(token* ntoken);
		int _getsymtype(char symbol);
		
		// imobj
		int _constid(int id);
		int _constidbyvalue(int value);
		int iminitim();
		
		int imnewreg(reg** ppreg);
		int imnewcnst(reg** ppreg);
		
		int imnewinstr(iminstr** ppinstr);
		
		int imsetscal(reg* preg, int pos);
		int imsetintr(reg* preg);
		
		int imregbyname(const char* symbol);
		int imregbyid(int id, reg** ppreg);
		int imcnstbyvalue(int value);
		
		int imsetrscrstr(int id, const char* chars);
		int imaddtramsize(int id, int size);
		int imtanksize(int id);
		
		int gfixconsts();
		int gfixdwrites();
		
		
		// parser
		int paparse(char* hsrcfile);
		int _gettoktype(char* ktokchars);
		
		int _l0_newreg(token* tok, int toktype);
		int _l0_pgmrsrc(int toktype);
		int _l0_newinstr(int toktype);
		
		int _l1_regsymbol(reg* creg, token* tok);
		int _l1_ioperand(iminstr* ins, int ifld);
		
		int _l2_asgnvalue(reg* creg, int toktype, int line);
		
		int _strtoregval(token* tok, int* value, int* flags);
		int _symdigorstr(char* ktokchars);
		int _isvaliduop(int rid, int ifld, int line);
		
		void _msg(const char* msg);
		int _err(int errpref, int errline, int errsuf, const char* extrainfo);
		
#ifndef TARGET_STANDALONE
		kString *err;
#endif
};

#endif
