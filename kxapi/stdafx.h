// kX API
// Copyright (c) Eugene Gavrilov, 2001-2009.
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

#if !defined(AFX_STDAFX_H__11223344_EC7B_4C58_8989_2C23F9DDDDFD__INCLUDED_)
#define AFX_STDAFX_H__11223344_EC7B_4C58_8989_2C23F9DDDDFD__INCLUDED_

#pragma once

#if defined(__APPLE__)
	#include <unistd.h>
#endif

#if defined(_MSC_VER)
  #include <afxtempl.h>
  #include <afxwin.h>         
  #include <afxext.h>         
  #include <afxdisp.h>        
  #include <afxdtctl.h>		
  #include <afxcmn.h>			

  #include <mmsystem.h>
  #include <setupapi.h>
  #include <dsound.h>
  #include <ks.h>
  #include <ksmedia.h>
  #include <winioctl.h>
#endif

#if defined(__GNUC__)
 #include "../h/debug.h"
 // there seems to be a name conflict with debug.h somewhere...
#else
 #include "debug.h"
#endif

#include "interface/kxapi.h"
#if defined(_MSC_VER)
	#include "interface/guids.h"
#endif
#include "interface/kx_ioctl.h"

#include <stdio.h>

#if defined(_MSC_VER)
	#include <malloc.h>
	#include <direct.h>
	#include <io.h>
#endif

#include <errno.h>

#ifndef TARGET_STANDALONE
 // #define dane_alloc(size) LocalAlloc(LMEM_FIXED|LMEM_ZEROINIT,size)
 // #define dane_free(block) LocalFree(block)
#else
 #define dane_alloc(size) malloc(size)
 #define dane_free(block) free(block)
 // your includes
 #include "i386.h"
 #include "dsp.h"
#endif

#include <cstring>
#if !defined(__GNUC__)
    // some weird issues with this
    #include <cmath>
#endif
#include <cfloat>
#include <stdio.h>
#include <cstdlib>

#endif  // AFX_STDAFX_H__11223344_EC7B_4C58_8989_2C23F9DDDDFD__INCLUDED_
