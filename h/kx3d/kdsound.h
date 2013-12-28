// kX 3-D Audio Technology and Environmental Audio
// Copyright (c) kX Project, 2001-2005.
// written by Eugene Gavrilov, 2002-2005.
// All rights reserved.

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


#ifndef KX3D_DSOUND_H_
#define KX3D_DSOUND_H_

#pragma once

// we cannot use 'dsound.h' - re-define all the constants and structs here...
#ifndef DS3D_DEFAULTCONEANGLE
#define DS3D_DEFAULTCONEANGLE		360
#define DS3D_DEFAULTCONEOUTSIDEVOLUME	0
#define DS3D_DEFAULTMAXDISTANCE		1000000000.0f
#define DS3D_DEFAULTMINDISTANCE		1.0f
#define DS3DMODE_NORMAL             	0x00000000
#define DS3DMODE_HEADRELATIVE       	0x00000001
#define DS3DMODE_DISABLE            	0x00000002
#define DSBVOLUME_MIN               	-10000
#define DSBVOLUME_MAX               	0
#define DS3D_MINCONEANGLE           	0
#define DS3D_MAXCONEANGLE           	360

 #if !defined(_KSMEDIA_)
 typedef float FLOAT;

 typedef struct _DS3DVECTOR {
    union {
        FLOAT x;
        FLOAT dvX;
    };
    union {
        FLOAT y;
        FLOAT dvY;
    };
    union {
        FLOAT z;
        FLOAT dvZ;
    };
 } DS3DVECTOR, *PDS3DVECTOR;
 #endif
#endif

#ifndef FLT_MIN
 #define FLT_MIN         1.175494351e-38F        /* min positive value */
 #define FLT_MAX         3.402823466e+38F        /* max value */
#endif

#endif
