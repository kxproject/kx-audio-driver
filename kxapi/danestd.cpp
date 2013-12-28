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

#include <math.h>

int _strtodspword(char* s, int* v, int* t){

	double dval, _x;
	char* p = s + 1;
	char* eos = s + strlen(s);
	int type = _32UNK;
	int extra=0;

	if (t == NULL) t = &type; 

	if ((p[0] == 'x') | (p[0] == 'X')){
		*v = (int)strtoul(s, &p, 0);
		if (eos - p) return _HEXSUF;
		*t = _32INT;
		return 0;
	}

	dval = strtod(s, &p);

	if ((eos - p) == 0){
		extra = 0;}
	else if(stricmp(p, _SUF__INT) == 0){
		extra = __INT;
		*t = _32INT;}
	else if(stricmp(p, _SUF_SEC) == 0){
		dval *= KXSAMPLERATE;
		extra = _SEC;
		*t = _32INT;}
	else if(stricmp(p, _SUF_MSEC) == 0){
		dval *= KXSAMPLERATE / 1000.;
		extra = _MSEC;
		*t = _32INT;}
	else if(stricmp(p, _SUF_DBL) == 0){
		/* 
		 * Note: Following expression converts decibel value to 
		 * "musical" linear value rather then to "classic math" one.
		 * In "musical" domain -6dB is equal to 0.5, while 
		 * in "classic math" it is equal to ~0.501187
		 * (however -20db is equal to 0.1 in both domains :)
		 */
		dval = pow(10.0, (double)dval * ((fabs(dval) < 20) ? .050171665943997 : .05));
		extra = _DBL;}
	else if(stricmp(p, _SUF_FSMP) == 0){
		dval *= _5BITS;
		extra = _FSMP;
		*t = _32INT;
	}
	

	if (*t == _32UNK) *t = (fabs(dval) <= 1.) ? _32REAL : _32INT;

	if (*t == _32REAL){
		//limits (saturates) double to -1/1 
		if (fabs(dval) > 1.) {
			dval =_copysign(1., dval);
			extra |= _SATUR;
		}
		*v = _double2fract(dval);
		return extra;
	}

	// converts double to *nearest* integer
	// e.g. 0.49999 -> 0 while 0.5 -> 1
	dval = modf(dval + _copysign(.5, dval), &_x);
	*v = (int) _x;
	if (dval != 0.5) extra |= _TRUNC;
	return extra;
}
