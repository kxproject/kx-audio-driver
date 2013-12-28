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


#if !defined(AFX_KXP16VROUTER_H__8CF38337_77A4_4F06_8A5F_BB91831DFD59__INCLUDED_)
#define AFX_KXP16VROUTER_H__8CF38337_77A4_4F06_8A5F_BB91831DFD59__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CRouterDlg dialog

class CP16VRouterDlg;
extern CP16VRouterDlg *p16v_router_window;

class CP16VRouterDlg : public CKXDialog
{
public:
	const char *get_class_name() { return "kXP16VRouter"; };

        int select_device(int dev_=-1);

        virtual int on_command(int wParam, int lParam);

	virtual void init();
	virtual void on_destroy();

	kCheckButton b_router[4];
	kCombo p16v_pb;

	void OnChangeP16vPlayback();

        int get_flag() { return KX_SAVED_AMOUNT|KX_SAVED_HWPARAMS; };

        DECLARE_MESSAGE_MAP()
};

#define IDg_P16V_PLAYBACK	(WM_USER+0x47)
#define P16V_ROUTER_R1		(WM_USER+0x48)

#endif // !defined(AFX_KXP16VROUTER_H__8CF38337_77A4_4F06_8A5F_BB91831DFD59__INCLUDED_)
