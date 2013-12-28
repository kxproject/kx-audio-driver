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


#if !defined(AFX_KXDSPDLG_H__8CF38337_77A4_4F06_8A5F_BB91831DFD59__INCLUDED_)
#define AFX_KXDSPDLG_H__8CF38337_77A4_4F06_8A5F_BB91831DFD59__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CKxdspDlg dialog

class CKxCode;

class CKxdspDlg;
extern CKxdspDlg *dsp_window;

class CKxdspDlg : public CKXDialog
{
public:
	const char *get_class_name() { return "kXDSP"; };

        int select_device(int dev_=-1);

        dword dsp_conn_size,dsp_wnd_size;

        #define MAX_PATCHES 100
        CKxCode *patches[MAX_PATCHES];

        CKxdspDlg() { for(int i=0;i<MAX_PATCHES;i++) patches[i]=NULL; };

        int get_flag() { return KX_SAVED_DSP; };

        void create_status();
        void resize_dsp_window();

        kStatic status,topline;

	int redraw_window(void);
	void invalidate_patches();

	int find_pgm_gpr(kPoint *,CKxCode **pgm,int *reg,char *tooltip=NULL);

	virtual void init();

	void on_mouse_r_up(kPoint pt,int flags);
	int draw(kDraw &d);
	void on_destroy();

        afx_msg BOOL OnToolTipNotify( UINT id, NMHDR * pNMHDR, LRESULT * pResult );

        virtual int on_command(int wParam, int lParam);

	DECLARE_MESSAGE_MAP()
};

#define IDg_REDRAW_KXDSP	(WM_USER+0x4a)


#endif // !defined(AFX_KXDSPDLG_H__8CF38337_77A4_4F06_8A5F_BB91831DFD59__INCLUDED_)
