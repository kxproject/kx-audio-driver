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
// #include "interface/dspnames.h"

// should not be placed in '.h' file
const char* KEYWORDS[]=
{
	",",
	";",
	"\n",

	op_codes[0],// "MACS",
	op_codes[1],// "MACSN",
	op_codes[2],// "MACW",
	op_codes[3],// "MACWN",
	op_codes[4],// "MACINTS",
	op_codes[5],// "MACINTW",
	op_codes[6],// "ACC3",
	op_codes[7],// "MACMV",
	op_codes[8],// "ANDXOR",
	op_codes[9],// "TSTNEG",
	op_codes[0xa],// "LIMIT",
	op_codes[0xb],// "LIMITN",
	op_codes[0xc],// "LOG",
	op_codes[0xd],// "EXP",
	op_codes[0xe],// "INTERP",
	op_codes[0xf],// "SKIP",

	"input",
	"output",
	"static",
	"control",
	"const",
	"temp",
	"idelay",
	"xdelay",

	"read",
	"add",
	"write",

	"wscl",
	"iscl",
	"oscl", 
	"intr",
	"coarse",
	
	"name",
	"copyright",
	"engine",
	"created",
	"comment",
	"guid",
	"itramsize",
	"xtramsize",

	operand_names_k1[0x57], // "ccr",
	operand_names_k1[0x5a], // "irq",
	operand_names_k1[0x56], // "accum",
	operand_names_k1[0x58], // "noise1",
	operand_names_k1[0x59], // "noise2",
	operand_names_k1[0x5b], // "dbac",

	"at",
	"=",

	"&",

	"end"
};


/***********************************************************/

int iDane::paparse(char* buf){

	token tok = { {'\0'}, -1};
	int toktype;

        // set source for the scanner
	hsrc = buf;
	bufpos = 0;
	iscomments = 0;
	cline = 1;
	empty[0] = 0;
	empty[1] = 0;

	iminitim();

	for (int i = 0; i < 2048; i++) // why not 2048? ;)
	{
		if (scnexttoken(&tok)) return 1; // scanner error
		toktype = _gettoktype(tok.chars);

		switch (toktype & K_GROUP_MASK){
		case K_RDEC_BIT:
			if (_l0_newreg(&tok, toktype)) return 1;
			break;
		case K_RSRC_BIT: 
			if (_l0_pgmrsrc(toktype)) return 1;
			break;
		case K_OPCODE_BIT: 
			if (_l0_newinstr(toktype)) return 1;
			break;
		case K_ENDEXPR: 
			break;
		case K_END: 
			return 0;
		default:
			// error: token unexpected here
			return _err(ERR_UNEXPTOKEN, tok.line, ERR_NONE, tok.chars);
		}
	}

	return _err(ERR_UNEXPTOKEN, tok.line, ERR_NONE, 0); // error: too many tokens
}

int iDane::_l0_newreg(token* tok, int toktype){

	reg* creg;

nextsym:
	switch (toktype & K_RDEC_MASK){
	case K_CONST:
		if (imnewcnst(&creg)) return 1;
		break;
	default:
		if (imnewreg(&creg)) return 1;
	}
	creg->type = toktype;

	for (int i = 0; i < 16; i++) // must never reach 16
	{
		if (scnexttoken(tok)) return 1; // scanner error
		toktype = _gettoktype(tok->chars);

		switch (toktype & K_GROUP_MASK){
		case K_NOTKEYWORD: 
			//check required props:
			if ((((creg->type & K_RDEC_MASK) == K_IDELAY)|
				((creg->type & K_RDEC_MASK) == K_XDELAY))&
				((creg->type & K_RPROP_MASK) == 0))
			return _err(ERR_MISSTANKOP, tok->line, ERR_NONE, 0);
			// return 1; // error: delay register operation (read/write) not specified
			switch (_l1_regsymbol(creg, tok)){
			case 0:
				return 0;
			case 1:
				return 1;
			case 2:
				toktype = creg->type & (int)(K_RDEC_MASK | K_RPROP_MASK | K_RSUB_MASK);
				goto nextsym;
			break;
			}
		case K_RPROP_BIT:
			toktype &= K_RPROP_MASK;
			// check if reg type allows this prop
			if (((creg->type & K_RDEC_MASK) < K_IDELAY)|
				((creg->type & K_RDEC_MASK) > K_XDELAY))
				return _err(ERR_NONE, tok->line, ERR_RWDELAYONLY, 0);
				// return 1; // error: write/read only for delays
			// check if read/write already present
			if (creg->type & K_RPROP_MASK) return _err(ERR_DUPTOKEN, tok->line, ERR_NONE, tok->chars); // return 1; // error: same type token duplication
			// assign property ro reg
			creg->type |= toktype;
			break;
		case K_RSUB_BIT:
			toktype &= K_RSUB_MASK;
			switch (toktype){
			case K_WSCL:
			case K_ISCL:
			case K_OSCL:
				if ((creg->type & K_RDEC_MASK) != K_CONTROL) return _err(ERR_NONE, tok->line, ERR_CTRLONLY, tok->chars); // return 1; // error: xscl can only be set for control register
				if (creg->type & K_XSCL_MASK) return _err(ERR_DUPTOKEN, tok->line, ERR_NONE, tok->chars); // return 1; // error: register property duplication
				if (imsetscal(creg, (toktype >> 8) - 1)) return _err(ERR_NONE, tok->line, ERR_REGALRDYSET, tok->chars); // return 1; // error: xscl already set
				creg->type |= toktype;
				break;
			case K_INTR:
				if (((creg->type & K_RDEC_MASK) != K_STATIC)| 
					(creg->type & K_20BIT_MASK)) return _err(ERR_NONE, tok->line, ERR_NOTFORCRSE, tok->chars); // return 1; // error: intr can only be set for 32bit static register
				if (creg->type & K_XSCL_MASK) return _err(ERR_DUPTOKEN, tok->line, ERR_NONE, 0); // return 1; // error: register property duplication
				creg->type |= toktype;
				if (imsetintr(creg)) return _err(ERR_DUPTOKEN, tok->line, ERR_NONE, tok->chars); // return 1; // error: intr already set
				break;
			case K_COARSE:
				if (((creg->type & K_RDEC_MASK) != K_STATIC)&
					((creg->type & K_RDEC_MASK) != K_CONTROL))
					return _err(ERR_NONE, tok->line, ERR_STATCTRLONLY, tok->chars); // return 1; // error: coarse only for static and control registers
				if (creg->type & K_XSCL_MASK) return _err(ERR_NONE, tok->line, ERR_NOTFORCRSE, tok->chars);  // return 1; // error: intr and xscl register cannot be 20bit (coarse)
				if (creg->type & K_20BIT_MASK) return _err(ERR_DUPTOKEN, tok->line, ERR_NONE, tok->chars); // return 1; // error: register property duplication
				creg->type |= toktype;
				break;
			}
			break;
		case K_ENDEXPR:
			return _err(ERR_EXPID, tok->line, ERR_NONE, 0);
			// error: no symbol assigned to register
		case K_RDEC_BIT:
			return _err(ERR_NONE, tok->line, ERR_TYPEUNEXP, tok->chars); 
			// return 1; // error: unexpected type specifier
		default:
			return _err(ERR_UNEXPTOKEN, tok->line, ERR_NONE, tok->chars); 
			// return 1; // error: unexpected token
		}
	}

	return _err(ERR_UNKP, tok->line, ERR_NONE, tok->chars);
	// return 1; // error: too many tokens per line
}

int iDane::_l0_pgmrsrc(int toktype){

	token tok;
	int size, cnvopts = _32INT;

	toktype &= K_RSRC_MASK;
	if (scnexttoken(&tok)) return 1; // scanner error

	int nexttoktype = _gettoktype(tok.chars);
	if (nexttoktype == K_ENDEXPR) return _err(ERR_EXPRSRCVAL, tok.line, ERR_NONE, tok.chars); // return 1; // error: missing resource value
	if ((unsigned)nexttoktype != K_NOTKEYWORD) return _err(ERR_EXPRSRCVAL, tok.line, ERR_NONE, tok.chars); // return 1; // error: should be resource value

	switch (toktype){
		case K_ITRAMSIZE: 
		case K_XTRAMSIZE: 
			if (_symdigorstr(tok.chars) != NKW_DIG) return _err(ERR_NOTDIGIT, tok.line, ERR_NONE, tok.chars); // return 1; // error: numeric value expected
			if (_strtoregval(&tok, &size, &cnvopts)) return 1;
			return imaddtramsize(toktype, size);
		default: // RSRC (name, engine etc.)
			if (_symdigorstr(tok.chars) != NKW_STR) return _err(ERR_STREXP, tok.line, ERR_NONE, tok.chars); // return 1; // error: string expected
			size = (dword)strlen(tok.chars);
			tok.chars[size - 1] = '\0';
			return imsetrscrstr(toktype, &tok.chars[1]);
	}
}

int iDane::_l0_newinstr(int toktype){

	iminstr* ins;
	int i = _OPR;
	token tok;

	if (imnewinstr(&ins)) return _err(ERR_INSTROVER, -1, ERR_NONE, 0); // not enough instr

	ins->opcode = toktype & K_OPCODE_MASK;

	if (_l1_ioperand(ins, i)) return 1;

	for (i = _OPA; i < NUMOPS; i++){

		if (scnexttoken(&tok)) return 1; // scanner error
		toktype = _gettoktype(tok.chars);

		switch (toktype & K_GROUP_MASK){
		case K_COMMA:
			break;
		case K_ENDEXPR: 
			return _err(ERR_MISSOPS, tok.line, ERR_NONE, 0);
			// return 1; // error: not enough operands
		default:
			return _err(ERR_MISSING, tok.line, ERR_NONE, ",");
			// return 1; // error: missing ","
		}
		
		if (_l1_ioperand(ins, i)) return 1;

	}

	if (scnexttoken(&tok)) return 1; // scanner error
	toktype = _gettoktype(tok.chars);

	if ((toktype & K_GROUP_MASK) != K_ENDEXPR) return _err(ERR_EXPENDOFI, tok.line, ERR_NONE, 0); // return 1; // error: end of instruction expected

	return 0;
}

int iDane::_l1_ioperand(iminstr* ins, int ifld){

	token tok;
	int rval, rid, opts = OPVALFMT_DT[ins->opcode][ifld];

addr:

	if (scnexttoken(&tok)) return 1; // scanner error
	int toktype = _gettoktype(tok.chars);

	switch (toktype & K_GROUP_MASK){
	case K_NOTKEYWORD:
		switch (_symdigorstr(tok.chars)){
		case NKW_DIG:
			if (ins->tags[ifld] & OPTAG_ADDR) return _err(ERR_NOADDR, tok.line, ERR_NONE, 0); // return 1; // error: address ('&') operator can be used only with delay registers
			if (_strtoregval(&tok, &rval, &opts)) return 1;
			if ((ifld == _OPR) & (rval != 0)) return _err(ERR_NULLCNSTONLY, tok.line, ERR_NONE, 0); // return 1; // error: only null constant can be used as result operand
			rid = imcnstbyvalue(rval);
			if (rid < 0) return _err(ERR_CONSTOVER, tok.line, ERR_NONE, 0); // return 1; // out of constants
			ins->ops[ifld] = (short) rid;
			break;
		case NKW_SYM:
			rid = imregbyname(tok.chars);
			if (rid < 0) return _err(ERR_UNDECLARED, tok.line, ERR_NONE, tok.chars); // return 1; // error: undeclared identifier
			if (_isvaliduop(rid, ifld, tok.line)) return 1;
			ins->ops[ifld] = (short) rid;
			break;
		case NKW_STR:
			return _err(ERR_UNEXPSTR, tok.line, ERR_NONE, NULL);
			// return 1; // error: unexpected string
		}
		
		break;
	case K_HWR_BIT:
		toktype &= K_HWR_MASK;
		if (VALIDOP_DT[toktype - K_CCR][ifld][0]) return _err(VALIDOP_DT[toktype - K_CCR][ifld][1], tok.line, ERR_NONE, NULL);; // error: depends on HVALIDOP_DT error
		ins->ops[ifld] = (short)(toktype | RT_HWR);
		break;
	case K_ADDRESS:
		ins->tags[ifld] = (short)(ins->tags[ifld] | OPTAG_ADDR);
		goto addr;
	case K_ENDEXPR:
		return _err(ERR_MISSOPS, tok.line, ERR_NONE, 0);
	default:
		return _err(ERR_EXPID, tok.line, ERR_NONE, 0); // return 1; // error: identifier expected
	}

	return 0;
}

int iDane::_l1_regsymbol(reg* creg, token* tok){

	if (_symdigorstr(tok->chars)) return _err(ERR_MISSNAME, tok->line, ERR_NONE, 0); // return 1; // error: missing variable name
	if (imregbyname(tok->chars) >= 0) return _err(ERR_NONE, tok->line, ERR_REDEF, tok->chars); // return 1; // error: symbol redifinition
	strncpy(creg->symbol, tok->chars, sizeof(creg->symbol));

	if (scnexttoken(tok)) return 1; // scanner error
	int toktype = _gettoktype(tok->chars);

	switch (creg->type & K_RDEC_MASK){
		case K_INPUT: 
		case K_OUTPUT: 
		case K_TEMP: 
			if (toktype == K_ENDEXPR) return 0;
			if (toktype == K_COMMA) return 2;
			return _err(ERR_EXPENDOFE, tok->line, ERR_NONE, 0); // return 1; // error: end of expression expected
		case K_STATIC:
		case K_CONTROL:
			if (toktype == K_ENDEXPR) return 0;
			if (toktype == K_COMMA) return 2;
		case K_CONST:
		case K_IDELAY:
		case K_XDELAY:
			if (toktype & K_RDATA_BIT) return _l2_asgnvalue(creg, toktype, tok->line);
			return _err(ERR_MISSVALUE, tok->line, ERR_NONE, 0); // return 1; // error: value assigning expected	
	}

	return 0;

	}

int iDane::_symdigorstr(char* ktokchars){

	if ((ktokchars[0] >= '0' && ktokchars[0] <= '9') ||
		(ktokchars[0] == '.') || 
		(ktokchars[0] == '-'))
    return NKW_DIG; // digit
	if (ktokchars[0] == '"') return NKW_STR; // string
	return NKW_SYM; // symbol
}

int iDane::_l2_asgnvalue(reg* creg, int toktype, int line){

	int cnvopts;

	switch (creg->type & K_RDEC_MASK){
		case K_CONST:
		case K_STATIC:
		case K_CONTROL:
			if ((toktype & K_RDATA_MASK) != K_ASSIGN) return _err(ERR_NOTASGN, line, ERR_NONE, 0); // return 1; // error: value should be assigned with "="
			cnvopts = _32UNK;
			break;
		case K_IDELAY:
		case K_XDELAY:
			if ((toktype & K_RDATA_MASK) != K_AT) return _err(ERR_NOTAT, line, ERR_NONE, 0); // return 1; // error: address of delay register should be assigned with "at"
			cnvopts = _32INT;
			break;
	}

	token tok;
	if (scnexttoken(&tok)) return 1; // scanner error
	toktype = _gettoktype(tok.chars);

	if ((unsigned)toktype != K_NOTKEYWORD) return _err(ERR_SYNTAX1, tok.line, ERR_NONE, tok.chars); // return 1; // error: value expected
	if (_symdigorstr(tok.chars) != NKW_DIG) return _err(ERR_NOTDIGIT, tok.line, ERR_NONE, 0); // return 1; // error: numeric value expected

	if (_strtoregval(&tok, &creg->tag, &cnvopts)) return 1;
	creg->type |= K_RDATA_BIT;

	if (((creg->type & K_RDEC_MASK) > K_IDELAY)&
		((creg->tag > imtanksize(creg->type & K_RDEC_MASK))|
		(creg->tag < 0)))
		return _err(ERR_NONE, tok.line, ERR_INVDELAYOFST, creg->symbol); // return 1; // error: numeric value expected


	if (scnexttoken(&tok)) return 1; // scanner error
	toktype = _gettoktype(tok.chars);

	if (toktype == K_ENDEXPR) return 0;
	if (toktype == K_COMMA) return 2;
	return _err(ERR_EXPENDOFE, tok.line, ERR_NONE, 0); // return 1; // error: end of expression expected

}


int iDane::_gettoktype(char* ktokchars){

	for (int i = 0; i < NUMKWS; i++){
		if (!stricmp(ktokchars, KEYWORDS[i])) return KEYWIDS[i];
	}
	return K_NOTKEYWORD;
}

int iDane::_isvaliduop(int rid, int ifld, int line){

	if (ifld != 0) return 0;
	
	reg* creg;
	imregbyid(rid, &creg);

	switch (creg->type & K_RDEC_MASK){
	case K_CONST:
		if (creg->tag != 0) return _err(ERR_NULLCNSTONLY, line, ERR_NONE, 0); // return 1; // error: only null constant can be used as result operand
	case K_CONTROL:
		return _err(ERR_CTRLREADONLY, line, ERR_NONE, 0); // return 1; // error: control register can be used as result operand
	}

	return 0;
}

int iDane::_strtoregval(token* tok, int* value, int* flags){

	// actually this is wrapper for error printing

	int r = _strtodspword(tok->chars, value, flags);

	switch (r){
		case _HEXSUF:
			_err(ERR_NONE, tok->line, ERR_INVHEX, tok->chars);
			break;
		case _BADDGT:
			return _err(ERR_NONE, tok->line, ERR_INVDIGIT, tok->chars);
			break;
		default:
		r &= (_SATUR | _TRUNC);
	}

	switch (r){
	case _SATUR:
		_err(ERR_NONE, tok->line, WRN_REALSATUR, tok->chars);
		break;
	case _TRUNC:
		_err(ERR_NONE, tok->line, WRN_INTTRUNK, tok->chars);
		break;
	}
	
	return 0;
}
