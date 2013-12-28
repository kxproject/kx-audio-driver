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


#if !defined(AFX_KXROUTER_H__8CF38337_77A4_4F06_8A5F_BB91831DFD59__INCLUDED_)
#define AFX_KXROUTER_H__8CF38337_77A4_4F06_8A5F_BB91831DFD59__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CRouterDlg dialog

class CRouterDlg;
extern CRouterDlg *router_window;

class CRouterDlg : public CKXDialog
{
public:
	const char *get_class_name() { return "kXRouter"; };

        int select_device(int dev_=-1);

        CTreeCtrl tree;

        #define MAX_TREE_ITEMS	5
        HTREEITEM tree_items[MAX_TREE_ITEMS];

        virtual int on_command(int wParam, int lParam);

	virtual void init();
	virtual void on_destroy();
        afx_msg void OnTreeNotify( NMHDR * pNotifyStruct, LRESULT * result );
        afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

        int get_flag() { return KX_SAVED_ROUTING|KX_SAVED_AMOUNT; };

        void empty_controls();

        // router/asio controls:
        #define MAX_BUSES 8
        kSlider fx_amounts[MAX_BUSES];
        #define IDg_ASIO_FXAMOUNT	(WM_USER+0x20)
        kTextButton fx_routings[MAX_BUSES];

        int cur_item; // (ndx for get/set_routing())
        
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_KXROUTER_H__8CF38337_77A4_4F06_8A5F_BB91831DFD59__INCLUDED_)
