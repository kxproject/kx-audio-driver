// kX Driver
// Copyright (c) Eugene Gavrilov, 2001-2005.
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


#ifndef DEFS_H_
#define DEFS_H_

#if defined(i386) || defined(I386) || defined(IX86) || defined(__I386__) || defined(_IX86) || defined(_M_IX86) || defined(AMD64) || defined(__x86_64__) || defined(__i386__)
 #include "i386.h"
#else
 #error "Unknown processor architecture"
#endif

#if defined(_MSC_VER)

   #if !defined(_W64)
    #if !defined(__midl) && (defined(_X86_) || defined(_M_IX86)) && _MSC_VER >= 1300
     #define _W64 __w64
    #else
     #define _W64
    #endif
   #endif

   #ifndef _INTPTR_T_DEFINED
    #ifdef  _WIN64
     typedef __int64             intptr_t;
    #else
     typedef _W64 int            intptr_t;
    #endif
    #define _INTPTR_T_DEFINED
   #endif

#elif defined(__GNUC__)

 #define __int64  long long

 typedef long			__darwin_intptr_t;
 #ifndef _INTPTR_T
 #define _INTPTR_T
 typedef __darwin_intptr_t	intptr_t;
 #endif

#else
 #error "Unknown compiler"
#endif

#ifndef NULL
 #define NULL 0
#endif

#endif
