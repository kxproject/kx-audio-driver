// kX Mixer
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


#if !defined(_AFX_STDAFX_H_)
#define _AFX_STDAFX_H_

#pragma once

#include <afxwin.h>         
#include <afxext.h>         
#include <afxdisp.h>        
#include <afxdtctl.h>		
#include <afxcmn.h>			

#include <mmsystem.h>
#include <setupapi.h>
#include <ks.h>
#include <ksmedia.h>
#include <winioctl.h>
#include <direct.h>
// legacy: #include <pbt.h>

#include <dsound.h>
#include <locale.h>

#include <htmlhelp.h>
#include <process.h>

#include <shlwapi.h>
#include <malloc.h>

#include "resource.h"
#include "debug.h"
#include "vscompat.h"

class CControlApp : public CWinApp
{
public:
	public:
	virtual BOOL InitInstance();
    virtual BOOL ExitInstance();
    virtual void OnHelp();

	DECLARE_MESSAGE_MAP()
};

#endif 
