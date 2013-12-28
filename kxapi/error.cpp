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

const char* E_STRING[][2]=
{
	{"\0", "\0"},
	{"A000:", " unknown error"},
	{"A001:", " error opening source file"},
	{"A002:", " error opening destination file"},
	{"A003:", " invalid arguments"},

	{"S100:", " unknown character"},
	{"S101:", " identifier cannot be more then 64 characters length"},
	{"S102:", " comments cannot be more then 256 characters length"},
	{"S103:", " string cannot be more then 256 characters length"},
	{"S104:", " unexpected end of file"},
	{"S105:", " new line in string"},

	{"P200:", " unexpected token"},
	{"P201:", " unknown error"},
	{"P202:", " missing delay register operation (read/write/add)"},
	{"P203:", " property can be set for delay line only register"},
	{"P204:", " duplicated token"},
	{"P205:", " property can be set for control only register"},
	{"P206:", " register already set"},
	{"P207:", " property cannot be set for coarse register"},
	{"P208:", " property can be only set for control or static register"},
	{"P209:", " resource value expected"},
	{"P210:", " not enough operands"},
	{"P211:", " missing"},
	{"P212:", " end of instruction expected"},
	{"P213:", " address operator can be use with delay line register only"},
	{"P214:", " constants (except null) cannot be used as R operand"},
	{"P215:", " undeclared identifier"},
	{"P216:", " identifier expected"},
	{"P217:", " no variable is declared"},
	{"P218:", " - identifier redefinition"},
	{"P219:", " end of expression expected"},
	{"P220:", " missing register value or address"},
	{"P221:", " value should be assigned with '=' operand"},
	{"P222:", " address should be assigned with 'at' operand"},
	{"P223:", " syntax error -"},
	{"P224:", " numeric value expected"},
	{"P225:", " control register is read-only"},
	{"P226:", " - unknown number format"},
	{"P228:", " unexpected string"},
	{"P229:", " string expected"},
	{"P230:", " - offset of delay access exceeds bounds of declared tramsize"},
	{"P231:", " accumulator can be used as A operand only"},
	{"P232:", " noise1 register is read-only"},
	{"P233:", " noise2 register is read-only"},
	{"P234:", " dbac register is read-only"},
	{"P235:", " - type specifier unexpected"},
	{"P236:", " - no suffix expected for hex number"},

	{"I400:", " unknown error"},
	{"I401:", " unknown error"},
	{"I402:", " unknown error"},

	{"A500:", " unknown warning"},

	{"P700:", " - number was trunkated to integer"},
	{"P701:", " usage of accumulator as R operand is a bad style - use 0 instead"},
	{"P702:", " - number is out of fractional bounds, limited to 1/-1"},
};


void iDane::_msg(const char* msg){
#ifdef TARGET_STANDALONE
	printf(msg);
#else
	if(err)
	 (*err)+=msg;
#endif
}

int iDane::_err(int errpref, int errline, int errsuf, const char* extrainfo){
#ifdef TARGET_STANDALONE
	if (errline >= 0) printf("(%i): ", errline);
	printf(((errpref | errsuf) < WRN_UNK) ? "error " : "warning ");
	printf(E_STRING[errpref][0]);
	printf(E_STRING[errsuf][0]);
	printf(E_STRING[errpref][1]); 
	if (extrainfo != 0) printf(" '%s'", extrainfo);
	printf(E_STRING[errsuf][1]); 
	printf("\n");
#else
	kString s,tmp;
	if (errline >= 0) { tmp.Format("(%d): ", errline); s+=tmp; }
	s+=(((errpref | errsuf) < WRN_UNK) ? "error " : "warning ");
	s+=E_STRING[errpref][0];
	s+=E_STRING[errsuf][0];
	s+=E_STRING[errpref][1]; 
	if (extrainfo != 0) { tmp.Format(" '%s'", extrainfo); s+=tmp; }
	s+=E_STRING[errsuf][1];
	s+="\n";

	if(err)
	 (*err)+=(const char *)s;
	else
	 fprintf(stderr,"%s",(const char *)s);
#endif
	return ((errpref | errsuf) < WRN_UNK) ? 1 : 0;
	
}
