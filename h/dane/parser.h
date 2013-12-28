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


#ifndef _DANE_PARSER_H_
#define _DANE_PARSER_H_

#define NKW_SYM 0 // symbol
#define NKW_DIG 1 // digit
#define NKW_STR 2 // string


// for decision tables
#define __OK	0x00	// valid
#define __NO	0x01	// invalid

const int VALIDOP_DT[][4][2] = { // VALIDOP_DT[x][y][1] is error number
//	_OP_R,				       _OP_A,			 _OP_X,			         _OP_Y     
	{{__OK, ERR_NONE},	       {__OK, ERR_NONE}, {__OK, ERR_NONE},       {__OK, ERR_NONE}},       // K_CCR	
	{{__OK, ERR_NONE},	       {__OK, ERR_NONE}, {__OK, ERR_NONE},       {__OK, ERR_NONE}},       // K_IRQ
	{{__NO, WRN_ACCASRESULT},  {__OK, ERR_NONE}, {__NO, ERR_ACCASAONLY}, {__NO, ERR_ACCASAONLY}}, // K_ACCUM		
	{{__NO, ERR_N1READONLY},   {__OK, ERR_NONE}, {__OK, ERR_NONE},       {__OK, ERR_NONE}},       // K_NOISE1
	{{__NO, ERR_N2READONLY},   {__OK, ERR_NONE}, {__OK, ERR_NONE},       {__OK, ERR_NONE}},       // K_NOISE2		
	{{__NO, ERR_DBACREADONLY}, {__OK, ERR_NONE}, {__OK, ERR_NONE},       {__OK, ERR_NONE}}        // K_DBAC
};

const int OPVALFMT_DT[][4]=
{//  R,		 A,		  X,		Y
	{_32INT, _32UNK,  _32REAL,	_32REAL}, // macs
	{_32INT, _32UNK,  _32REAL,	_32REAL}, // macsn
	{_32INT, _32UNK,  _32REAL,	_32REAL}, // macw
	{_32INT, _32UNK,  _32REAL,	_32REAL}, // macwn
	{_32INT, _32UNK,  _32UNK,	_32INT},  // macints
	{_32INT, _32UNK,  _32UNK,	_32INT},  // macintw
	{_32INT, _32REAL, _32REAL,	_32REAL}, // acc3
	{_32INT, _32UNK,  _32REAL,	_32REAL}, // macmv
	{_32INT, _32INT,  _32INT,	_32INT},  // andxor
	{_32INT, _32REAL, _32REAL,	_32REAL}, // tstneg
	{_32INT, _32REAL, _32REAL,	_32REAL}, // limit
	{_32INT, _32REAL, _32REAL,	_32REAL}, // limitn
	{_32INT, _32UNK,  _32INT,	_32UNK},  // log
	{_32INT, _32UNK,  _32INT,	_32UNK},  // exp
	{_32INT, _32UNK,  _32REAL,	_32UNK},  // interp
	{_32INT, _32UNK,  _32UNK,	_32INT},  // skip
};

#endif // _PARSER_H_
