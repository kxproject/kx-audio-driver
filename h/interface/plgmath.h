// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov, eYagos and Max Mikhailov, 2001-2005.
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

// this file is automatically included by 'stdafx.h' in kxfxlib sources

#ifndef _PLGMATH_H
#define _PLGMATH_H

#ifdef M_PI
 #define pi	M_PI
#else
 #define pi	3.141592653589793
#endif

#define dbk	19.9315685693241740 // used for dB to G conversion instead of classic 20
#define FS	48000 
#define FN	24000
#define SAMPLES_PER_MSEC	48
#define SAMPLES_PER_SEC		FS

inline dword itodspr_scl(int scl, kxparam_t v)
{
 return (dword)((double) (v) * ((double)0x7fffffff / ((double)scl)));
}

#ifndef KX_DBL_TO_DSPWORD
#define KX_DBL_TO_DSPWORD
    inline dword _dbl_to_dspword(double x)
    {
      double y = (x * 2147483647.5) - 0.25;
      y += 6755399441055744.0;
      return ((dword*) &y)[0];
    }
#endif

inline double dBtoG(double x) { return exp(0.1151292546498 * x); }
inline double cBtoG(double x) { return exp(0.01151292546498 * x); }



// LEGACY: 
// #define dBtoG(value) (pow(10., ((double) value) / dbk))
// #define cBtoG(value) (pow(10., ((double) value) / (dbk * 10.)))
// #define _dbl_to_dspword(value) (dword) ((double) (value) * (double) MAX_32BIT_VALUE)




//****************************************************************
//--- LOG & EXP intructions -------------------------
// Calculates the result of the DANE intructions: 
//		LOG  a,value,res,0x0
//		LOG  a,value,res,0x0
// (based on as10k1-manual description of these two instructions)
//****************************************************************

 #define	CN231	2147483647
 #define	LN2		0.69314718055994530942

 inline	double log_DANE(double value, int res)
 {
	// Function for calculatinf 
	int	  long_res, shifts, exp, s; 
	double	x;
	unsigned int val;
	
	long_res = static_cast <int> (log((float)res)/LN2 + 1.); 
	s = static_cast <int> (fabs(value)/value);
	val = static_cast <int> (fabs(value) * CN231); 
	shifts = 32 - static_cast <int> (log((float)val)/LN2 + 1.); 
	val = val << shifts; 
	exp = res  - shifts;
	if (exp >= 0) {val = val << 1;}
	exp = exp + 1;
	shifts = (long_res + 1);
	if (exp < 0 ) {shifts = shifts - exp; exp=0;};
	val = val >> shifts;
	exp = exp << (32 - shifts);
	val = exp + val; 
	x = (double) (val) / CN231 * s; 
	return x;
 }

 inline	double exp_DANE(double value, int res)
 {
	int exp;
	// size of res (in bits)  
	int long_res = static_cast <int> (log((float)res)/LN2 + 1.); 
	// Sign
	int	s = static_cast <int> (fabs(value)/value);  
	// Integer format
	unsigned int val = static_cast <int> (fabs(value) * CN231); 
	// calculates msb
	int msb = 32 - static_cast <int> (log((float)val)/LN2 + 1.); 
	
	if (msb<=long_res)  // imlicit msb
	{
		exp = val >> (31 - long_res);
		val = val << (long_res + 1);
		val = val >> (long_res + 1);
		val = val << (long_res + 1);
		val = val >> 1;
		val = val + CN231 - 1;
		val = val >> (res + 1 - exp);
	}
	else
	{
		val = val << (long_res + 1);
		val = val << (msb - long_res - 1);
		val = val >> (msb + res - long_res);
	}
	
	double x = (double) (val) / CN231 * s; 

	return x;
 }

 #undef	CN231	
 #undef LN2	
#endif
//****************************************************************
//****************************************************************
