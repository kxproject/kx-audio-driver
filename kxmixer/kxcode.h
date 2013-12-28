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


#if !defined(AFX_KXCODE_H__8CF38337_77A4_4F06_8A5F_BB91831DFD59__INCLUDED_)
#define AFX_KXCODE_H__8CF38337_77A4_4F06_8A5F_BB91831DFD59__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CKxdspDlg;

class CKxCode : public kDialog
{
public:
	CKxCode();
	~CKxCode();

	iKXPlugin *plugin;

	CKxdspDlg *parent;
	int capture,is_moving;
	kRect capture_pos;
	kButton b_minimax;

        // microcode info
        int flag;
	dsp_code *code;
	int code_size;
	dsp_register_info *info;
	int info_size;
	int itramsize,xtramsize;
	char name[KX_MAX_STRING];
	int offset;
	char m_copyright[KX_MAX_STRING];
	char m_comment[KX_MAX_STRING];
	char m_engine[KX_MAX_STRING];
	char m_created[KX_MAX_STRING];
	char m_guid[KX_MAX_STRING];

	kString def_tooltip;
	char *set_tooltip();

	int gpr_to_connect;

	BOOL create(int x,int y,int pgm_id,CKxdspDlg *parent);

	int refresh(void);

	int pgm_id;

	dword cc_link_progress,cc_progress_text,cc_progress_bk;

        virtual void on_mouse_move(kPoint pt,int flags);
        virtual void on_mouse_l_up(kPoint pt,int flags);
        virtual void on_mouse_l_dbl(kPoint pt,int flags);
        virtual void on_mouse_l_down(kPoint pt,int flags);
        virtual void on_mouse_r_up(kPoint pt,int flags);
        virtual int draw(kDraw &d);

        void do_paint(HDC dc,int x=0,int y=0);

        virtual int on_command(int wParam, int lParam);

        virtual void on_destroy();
};

#define IDg_MINIMAX	(WM_USER+0x4a)

#endif // !defined(AFX_KXCODE_H__8CF38337_77A4_4F06_8A5F_BB91831DFD59__INCLUDED_)
