// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov, eYagos and Max Mikhailov, 2001-2004.
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

//--- Parameter Conversions -------------------------

 // Calculates the result of the DANE intructions: 
 //		log  a,value,res,0x0
 //		exp  a,value,res,0x0
 //- value has to be in fractional format: 
 //     value[fractional] = 10 ^ (value[dB] / 20.) 
 //		value[fractional] = DSPWORD / (2^31 - 1)
 //- results are valid for res=3..31)(there is an error fo res<3).

 #define	CN231	2147483647
 #define	LN2	0.6931471806
 #define	LN10	2.302585093
 
 #define	LT	1000.0
 #define	L1	484.0
 #define	L2	516.0
 #define	scale	1000
 #define	limit   -1000

 inline	double log_DANE(double value, int res)
 {
	// size of res (in bits)
	int long_res = static_cast <int> (log(res)/LN2 + 1.); 
	// Sign
	int	s = static_cast <int> (fabs(value)/value);  
	// Integer format
	unsigned int val = static_cast <int> (fabs(value) * CN231); 
	// calculates the MSB
	int msb = 32 - static_cast <int> (log(val)/LN2 + 1.); 
	// Shift 1 
	val = val << msb; 
	int exp = res  - msb;
	// Shift 2
	if (exp >= 0) //implicit MSB
	{
		val = val << 1; 
		exp = exp + 1;
	}
	else
	{
		val = val >> (-1 - exp);
		exp = 0;
	}
	// Shift 3  -> mantisa
	val = val >> (long_res + 1); 
	//Shift 4  -> exponent
	exp = exp << (31 - long_res); 
	// result in integer format
	val = val + exp; 
	// result in fractional format
	double x = (double) (val) / CN231 * s; 

	return x;
 }
 inline	double log_DANE_INTERP(double value, int res)
 {
	// size of res (in bits)
	int long_res = static_cast <int> (log(res)/LN2 + 1.); 
	// Sign
	int	s = static_cast <int> (fabs(value)/value);  
	// Integer format
	unsigned int val = static_cast <int> (fabs(value) * CN231); 
	// calculates the MSB
	int msb = 32 - static_cast <int> (log(val)/LN2 + 1.); 
	// Shift 1 
	val = val << msb; 
	int exp = res  - msb;
	// Shift 2
	if (exp >= 0) //implicit MSB
	{
		val = val << 1; 
		exp = exp + 1;
	}
	else
	{
		val = val >> (-1 - exp);
		exp = 0;
	}
	// Shift 3  -> mantisa
	val = val >> (long_res + 1); 
	//Shift 4  -> exponent
	exp = exp << (31 - long_res); 
	// result in integer format
	unsigned int result = val + exp; 
	// result in fractional format
	double x = (double) (result) / CN231 * s; 
	double mant = (double) (val) / CN231 * s; 

	x = x - 2583.519600*(mant-.5326320540e-2)*(mant-.2365656064e-1)*(mant*mant-.9454410654e-1*mant+.5425726184e-2);
	

	return x;
 }
 inline	double exp_DANE(double value, int res)
 {
	int exp;
	// size of res (in bits)  
	int long_res = static_cast <int> (log(res)/LN2 + 1.); 
	// Sign
	int	s = static_cast <int> (fabs(value)/value);  
	// Integer format
	unsigned int val = static_cast <int> (fabs(value) * CN231); 
	// calculates msb
	int msb = 32 - static_cast <int> (log(val)/LN2 + 1.); 
	
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
//___________
//___________
 //Convert from dB to exp-dB in DANE
 inline	double dB_to_exp(double value)
 {
	 double x = (0.935718067+0.045115616*log(exp(log(10.0)/20.0 * (value) )));
	 return x;
 }

 //Convert from ratio-slider value to ratio value
 inline double ratio_to_val (double value)
 {

	double x = 0.0;
	if (value<=L1)
	{
		x = value / L1;
		//x = pow (100.0, x) / pow (100.0, 10.0 / L1);
		x = exp(2.*LN10*(x*L1-10.)/L1);
		x = ((int) ((x + 0.05)*10)) / 10.0; 
	}
	else if (value>=L2) 
	{
		x = (LT - value) / L1;
		//x = pow (100.0, x) / pow (100.0, 10.0 / L1);
		x = exp(2.*LN10*(x*L1-10.)/L1);
		x = ((int) ((x + 0.05)*10)) / 10.0;
	}
	else
	{
		x = 10000.0;
	}
	return x;
 }

 //Convert from dB-slider value to dB value
 inline double dB_to_val (int value)
 {
	double	x = value / 10.0;
	if (value<limit)	x = -120.0;
	return x;
 }
 //Convert from time-slider value to time value
 inline double time_to_val(double value)
 {
	//double x = pow (10.0,(value/(scale/5.0))-1.0);  //lineal value
	double x = exp(LN10*(5.*value-1.*scale)/scale);
	return x;

        // legacy: #define time_to_val(value)	(exp (-0.06241108904 / (double) (value)));
 }
 //Convert form time-lineal value to time-exp value
 inline double time_to_exp(double value)
 {
	//double x = exp (-0.06241108904 / value); //exp value
	double x = 1.0 - 1.0 / (value/1000.0 * 48000.0);
	return x;
 }

 #undef	CN231	
 #undef LN2	
 #undef LN10	
 
 #undef	LT	
 #undef	L1	
 #undef L2	
 #undef scale	
 #undef limit   

#endif
