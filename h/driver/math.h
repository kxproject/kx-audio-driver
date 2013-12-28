// kX Driver
// Copyright (c) kX Project, 2001-2005.
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


#ifndef KXMATH_H_
#define KXMATH_H_

#if !defined(__APPLE__)	&& !defined(__MACH__)

#include <math.h>

// these are generally inlines:

inline double kx_log10(register double _x_)
{
        return log10(_x_);
}

inline double kx_pow2(register double _y_)
{
        return pow(2.0,_y_);
}

inline double kx_pow10(register double y)
{
	return pow(10.0,y);
}

inline double kx_sqrt(register double y)
{
	return sqrt(y);
}

#else

double kx_log10(register double x);
double kx_pow2(register double);
double kx_pow10(register double);
double kx_sqrt(register double);

#endif

// long _p_acos(float c)
// approximation of arccos (see _pacos.gif: blue - real acos, red - this function)
// returns the angle in deegrees (between 0° and 180°)
inline int kx_arccos(float c)
{                   
    float x;
    int ret;

    x = c * c * c; x = x * x * x; // x = c^9
    ret = 90 - (int)(59.f * c + 31.f * x);

    return ret;
}

#endif	// KXMATH_H_
