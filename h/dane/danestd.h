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


#ifndef _DANESTD_H_
#define _DANESTD_H_

#ifndef MAXFRACT
#define MAXFRACT	0x7fffffff

#define _fract2double(value) ((double) value / MAXFRACT)
#define _double2fract(value) ((int) (MAXFRACT * (double) value)) - ((value < 0) ? 1 : 0)

/* 
 * Also following variant is working sometimes (but not always!) 
 * -1 -> 0x80000000 vs. 0x80000001 problem :(
 #define _double2fract(value) ((int) (MAXFRACT * ((value < 0) ? (float) value : (double) value)))
 */

#if defined(__APPLE__)
	#define stricmp(a,b) strcasecmp(a,b)
	#define _copysign(x,y) copysign(x,y)
#endif

#endif /* MAXFRACT */


#define KXSAMPLERATE	48000.
#define _5BITS			4096.

#define _32UNK	0
#define _32INT	1
#define _32REAL	2

#define _SUF__INT	"i"
#define _SUF_SEC	"s"
#define _SUF_MSEC	"ms"
#define _SUF_FSMP	"fs"	
#define _SUF_DBL	"db"

#define _HEXSUF	-2	// no suffix with hex allowed
#define _BADDGT	-1	// unknown number format
#define __INT	1	// force integer
#define _SEC	2	// second
#define _MSEC	4	// 1/1000 of second
#define _FSMP	8	// fractional sample (e.g 0x800 == 0.5fs)
#define _DBL	16	// decibel (e.g 0.5 == -6db)
#define _SATUR	32	// saturation
#define _TRUNC	64	// truncation

int _strtodspword(char* string, int* value, int* type = NULL);

#endif /* _DANESTD_H_ */
