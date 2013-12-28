// kX Driver / Driver Interface
// Copyright (c) Eugene Gavrilov, 2001-2005.
// All rights reserved

// user-level debugging

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

#ifndef KX_DEBUG_H_
#define KX_DEBUG_H_

#pragma once

#ifdef KX_DEBUG

 #ifdef UNICODE
  void debug(const wchar_t *__format,...);
 #else
  void debug(const char *__format,...);
 #endif

 #if defined(_MSC_VER)
   // #define malloc(size) kx_malloc_dbg(size,__FILE__,__LINE__)
   // extern "C" void *kx_malloc_dbg(size_t size,const char *file,int line);
   #define malloc(size) _malloc_dbg(size,_NORMAL_BLOCK,__FILE__,__LINE__)
 #endif

#else

 #ifndef UNICODE
  inline void debug(const char *,...) {}
 #else
  inline void debug(const wchar_t *,...) {}
 #endif

#endif

// #define trace_here()    debug("== "__FILE__": %d\n",__LINE__)

#endif
