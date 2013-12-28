// kX GUI
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

#if !defined(kWindow_H__52789AD8_9B95_49F3_9BAD_3EA749987D3F__INCLUDED_)
#define kWindow_H__52789AD8_9B95_49F3_9BAD_3EA749987D3F__INCLUDED_

#pragma once

/////////////////////////////////////////////////////////////////////////////
// kWindow

class kCLASS_TYPE kWindow : 
 #ifdef TEST_KGUI
  protected 
 #else
  public 
 #endif
  CWnd
{
public:
	kWindow();
	virtual ~kWindow();

        // creation
	virtual int create(const TCHAR *caption, const TCHAR *class_,
	  kRect &rect, unsigned int style,
	  unsigned int id=0, kWindow *parent_=0);

        // operators
	virtual int draw(kDraw &d); // returns 1 - draw default
	virtual int draw_nc(kDraw &d);
	virtual int erase_background(kDraw &d);
	virtual void show();
	virtual void hide();
	virtual void redraw();
	virtual void destroy();
	virtual void activate();
	virtual void resize(kPoint pt);
	virtual void move(kPoint pt);
	virtual void move(int new_x,int new_y);
	virtual void reposition(int new_x,int new_y,int new_wd,int new_ht); // new_wd/ht - relative
	virtual void reposition(kRect &r); // r.right/bottom - absolute
	virtual void size_to_content(void);
	virtual void get_rect(kRect &r_);
	virtual void get_rect_abs(kRect &r_);
	virtual void set_text(LPCTSTR s);

	virtual void set_alpha(int alpha_) { k_alpha=alpha_; };
	virtual void set_method(int method_) { k_method=method_; };
//        virtual void add_tool(const TCHAR *text,kWindow *w,unsigned int id);
        virtual void set_transpcolor(kColor t_c) { k_transparent_color=t_c; }

        // events
        virtual int on_command(int wparam,int lparam);
        virtual void on_sys_command(int wparam,int lparam);

        virtual void on_create();
        virtual void on_destroy();
        virtual void on_post_nc_destroy();

        virtual void on_mouse_move(kPoint pt,int flags);
        virtual void on_mouse_l_up(kPoint pt,int flags);
        virtual void on_mouse_l_dbl(kPoint pt,int flags);
        virtual void on_mouse_l_down(kPoint pt,int flags);
        virtual void on_mouse_r_down(kPoint pt,int flags);
        virtual void on_mouse_r_dbl(kPoint pt,int flags);
        virtual void on_mouse_r_up(kPoint pt,int flags);
        virtual void on_mouse_m_up(kPoint pt,int flags);
        virtual void on_mouse_m_down(kPoint pt,int flags);
        virtual void on_mouse_m_dbl(kPoint pt,int flags);
        virtual void on_mouse_wheel(kPoint pt, int flags, int delta);

        virtual void on_key_down(int ch, int rpt, int fl);
        virtual void on_char(int ch, int rpt, int fl);

        virtual void on_loose_focus(); // kill focus
        virtual void on_loose_capture(); // on change capture
        virtual void on_focus();

        virtual int on_hit_test(kPoint pnt);

	virtual const TCHAR *get_class_name() { return _T("kGuiWindowClass"); };
	virtual intptr_t message(int msg,int wparam,int lparam);

	int is_over() { return (k_state&kSTATE_OVER)?1:0; };
	int is_enabled() { return (k_state&kSTATE_ENABLED)?1:0; };
	int is_selected() { return (k_state&kSTATE_SELECTED)?1:0; };
	int is_focused() { return (k_state&kSTATE_FOCUSED)?1:0; };
	void set_over(int set=1) { if(set) k_state|=kSTATE_OVER; else k_state&=~kSTATE_OVER; };
	void set_enabled(int set=1) { if(set) k_state|=kSTATE_ENABLED; else k_state&=~kSTATE_ENABLED; };
	void set_selected(int set=1) { if(set) k_state|=kSTATE_SELECTED; else k_state&=~kSTATE_SELECTED; };
	void set_focused(int set=1) { if(set) k_state|=kSTATE_FOCUSED; else k_state&=~kSTATE_FOCUSED; };

	unsigned int send_message(unsigned int message, unsigned int wparam,unsigned int lparam);
	
	int get_state() { return k_state; };
	unsigned int get_id() { return k_id; };
	HWND get_wnd() { return m_hWnd; };
	kWindow *get_parent() { return k_parent; };

	kFont *get_font() { return &font; };

	void set_font(HFONT f);
	void set_font(kFont &f);

	// data
protected:
	kWindow *k_parent;

	int k_state;
	unsigned int k_id;
	// HWND k_wnd;
	kFont font;
	int k_alpha;
	int k_method;
	kColor k_transparent_color;

	#define kMAX_CLASS_NAME	64
	TCHAR k_class_name[kMAX_CLASS_NAME];

private:
// LEGACY stuff
	DECLARE_MESSAGE_MAP()

	afx_msg BOOL OnCommand(WPARAM,LPARAM);
	afx_msg void OnPaint();
        afx_msg void OnNcPaint();
        afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
        afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
        afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
        afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
        afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
        afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
        afx_msg void OnMouseMove( UINT fl, CPoint pt);
        afx_msg void OnDestroy();
        afx_msg BOOL OnMouseWheel( UINT, short, CPoint );
        afx_msg void OnKillFocus(CWnd*w);
        afx_msg void OnCaptureChanged(CWnd* pWnd);
#ifdef KX_MFC7
        afx_msg void OnDrawItem(int ctrl_id,LPDRAWITEMSTRUCT lpDrawItemStruct);
        afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
#else
        afx_msg BOOL OnDrawItem(int ctrl_id,LPDRAWITEMSTRUCT lpDrawItemStruct);
        afx_msg void OnCreate(LPCREATESTRUCT lpCreateStruct);
#endif
        afx_msg BOOL OnEraseBkgnd(CDC* pDc);
        afx_msg void PostNcDestroy();
        afx_msg UINT OnGetDlgCode();
        afx_msg void OnSetFocus(CWnd* pOldWnd);
        afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
        afx_msg void OnChar(UINT ch,UINT rep,UINT fl);
        afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
        afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
        afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
        afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
        afx_msg UINT OnNcHitTest(CPoint point);
        afx_msg void OnNcRButtonUp(UINT nFlags, CPoint point);
        LRESULT WindowProc(UINT uMsg,WPARAM wParam,LPARAM lParam);
};

/////////////////////////////////////////////////////////////////////////////

#endif
