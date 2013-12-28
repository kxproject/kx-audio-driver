// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov, 2001-2006.
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

#if !defined(_KXFXLIB_STDAFX_H_)
#define _KXFXLIB_STDAFX_H_

#ifndef WINVER				// Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0501		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target Windows 2000 or later.
#endif						

#ifndef _WIN32_IE			// Allow use of features specific to IE 4.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target IE 5.0 or later.
#endif

#pragma once

// Windows / MFC headers

#include <afxtempl.h>
#include <afxwin.h>         
#include <afxext.h>         
#include <afxdisp.h>        
#include <afxdtctl.h>		
#include <afxcmn.h>			


#include <commctrl.h>			

// ANSI C headers

#include <stdio.h>
#include <malloc.h>
#include <math.h>

#if defined(_MFC_VER) && (_MFC_VER >= 0x0700)
#define KX_MFC7
#endif

// kX SDK headers:

#include "debug.h"
#include "vers.h"

#include "interface/kxapi.h"
#include "gui/kGui.h"
#include "interface/kxplugingui.h"

#include "fxlib.h"
#include "interface/plgmath.h"

// NOTE: FOR PRE-3538 plugins ONLY
// all the plugin authors are encouraged to update their code accordingly!
#if KX_VERSION_DWORD>=0x51003538
 #include "interface/kxcompat.h"
#endif

#endif 
