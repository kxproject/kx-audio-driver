// kX Setup
// Copyright (c) Eugene Gavrilov, 2001-2014.
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


#if !defined(AFX_STDAFX_H__31843FC6_5DFB_49B7_8C8E_C86286A050C8__INCLUDED_)
#define AFX_STDAFX_H__31843FC6_5DFB_49B7_8C8E_C86286A050C8__INCLUDED_

#pragma once

#include <process.h>

#include <afxwin.h>         
#include <afxext.h>         
#include <afxdisp.h>        
#include <afxdtctl.h>       
#include <afxcmn.h>         

#include <mmsystem.h>
#include <dsound.h>

#define _SETUPAPI_VER _WIN32_WINNT_WINXP
#include <setupapi.h>

#include <newdev.h>
#include <cfgmgr32.h>
#include <malloc.h>
#include <tchar.h>

#include <shlwapi.h>

#include "resource.h"
#include "gui/kGui.h"
#include "gui/kResource.h"
#include "vers.h"

class CSetupDialog : public kDialog
{
public:
    virtual void init();
    afx_msg void OnSetup();
    afx_msg void OnAbout();
    afx_msg void OnRemove();

    int on_command(int wp,int lp);

    kButton b_setup,b_quit,b_about,b_remove;

};
class CAboutDialog : public kDialog
{
public:
    virtual void init();

    kButton b_ok;
    kStatic info;
};

class CSetupApp : public CWinApp
{
public:
    public:
    virtual BOOL InitInstance();

};

class CLicenseDialog : public kDialog
{
public:
    virtual void init();

    kButton b_ok;
    kButton b_cancel;
    kList b_license;
};

void kx_setup(kWindow *w,int remove_only);
void reset_kx_settings(void);
int update_asio_drivers(void);

extern kFile mf;

#endif
