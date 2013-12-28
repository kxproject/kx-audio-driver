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

#include "stdafx.h"

#ifdef _DEBUG
 #define new DEBUG_NEW
 #undef THIS_FILE
 static char THIS_FILE[] = __FILE__;
#endif


BEGIN_MESSAGE_MAP(kWindow, CWnd)
	ON_WM_PAINT()
	ON_WM_NCPAINT()
	ON_WM_ERASEBKGND()

        ON_WM_LBUTTONUP()
        ON_WM_LBUTTONDOWN()
        ON_WM_LBUTTONDBLCLK()
        ON_WM_RBUTTONUP()
        ON_WM_RBUTTONDOWN()
        ON_WM_RBUTTONDBLCLK()
        ON_WM_MBUTTONUP()
        ON_WM_MBUTTONDOWN()
        ON_WM_MBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()

	ON_WM_DRAWITEM()

	ON_WM_CREATE()
	ON_WM_DESTROY()

	ON_WM_SYSCOMMAND()

	ON_WM_CAPTURECHANGED()
	ON_WM_KILLFOCUS()
	ON_WM_NCHITTEST()
	ON_WM_NCRBUTTONUP()

        ON_WM_GETDLGCODE()

        ON_WM_CHAR()
        ON_WM_KEYDOWN()
END_MESSAGE_MAP()

kWindow::kWindow()
{
	k_parent=NULL;
	k_method=kMETHOD_DEFAULT;
	k_alpha=255;
        k_state=0;
        k_id=0;

        k_transparent_color=RGB(0xff,0xff,0xff);
}

kWindow::~kWindow()
{
}

int kWindow::draw(kDraw &)
{
 return 1; // draw default
}

void kWindow::destroy()
{
 DestroyWindow();
 // m_hWnd=NULL;
}

void kWindow::OnPaint()
{
	if(k_method==kMETHOD_DEFAULT)
	{
	 CWnd::OnPaint();
/*
	_AFX_THREAD_STATE* pThreadState = _afxThreadState.GetData();
	return DefWindowProc(pThreadState->m_lastSentMsg.message,
		pThreadState->m_lastSentMsg.wParam, pThreadState->m_lastSentMsg.lParam);
*/
	}
	else
	{
	PAINTSTRUCT ps;

        HDC pDC=::BeginPaint(this->m_hWnd,&ps);
	if(pDC==NULL)
	{
	  return;
	}

	kDraw d(pDC);

	if(draw(d)==1) // draw default
	{
         ::EndPaint(this->m_hWnd,&ps);
         CWnd::OnPaint();
        }
        else
         ::EndPaint(this->m_hWnd,&ps);

        }
}


int kWindow::draw_nc(kDraw &)
{
	return 1; // draw default
}

void kWindow::OnNcPaint()
{
	HDC hdc = ::GetDCEx(this->m_hWnd,NULL, DCX_WINDOW/*|DCX_INTERSECTRGN*/);
        // note: this call sometimes fails...

	if(hdc)
	{
    	// Paint into this DC
    	kDraw d(hdc);

    	if(draw_nc(d)==1)
    	{
    	 ::ReleaseDC(this->m_hWnd, hdc);
    	 CWnd::OnNcPaint();
    	}
    	else
    	 ::ReleaseDC(this->m_hWnd, hdc);
	}
	else
		CWnd::OnNcPaint();
}

int kWindow::erase_background(kDraw &d)
{
	// 'default'
	return CWnd::OnEraseBkgnd((CDC *)(d.get_dc()));
}

afx_msg BOOL kWindow::OnEraseBkgnd(CDC*d)
{
 kDraw dr((HDC)d); // FIXME!!
 return erase_background(dr);
}


void kWindow::show()
{
    ::SetForegroundWindow(m_hWnd);
	::ShowWindow(m_hWnd,SW_SHOW);
    ::ShowWindow(m_hWnd,SW_RESTORE);
//	redraw();
}

void kWindow::hide()
{
	::ShowWindow(m_hWnd,SW_HIDE);
}

void kWindow::redraw()
{
	::RedrawWindow(m_hWnd,NULL,NULL,RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
}

void kWindow::resize(kPoint pt)
{
	SetWindowPos(NULL, -1, -1, pt.x, pt.y,
	  SWP_NOMOVE|SWP_NOZORDER|SWP_NOREDRAW|SWP_NOACTIVATE);
}

void kWindow::move(kPoint pt)
{
	SetWindowPos(NULL, pt.x, pt.y, -1, -1,
	  SWP_NOSIZE|SWP_NOZORDER|SWP_NOREDRAW|SWP_NOACTIVATE);
}

void kWindow::move(int new_x,int new_y)
{
	SetWindowPos(NULL, new_x, new_y, -1, -1,
	  SWP_NOSIZE|SWP_NOZORDER|SWP_NOREDRAW|SWP_NOACTIVATE);
}

void kWindow::reposition(int new_x,int new_y,int new_wd,int new_ht)
{
	SetWindowPos(NULL, new_x, new_y, new_x+new_wd, new_y+new_ht,
	  SWP_NOZORDER|SWP_NOREDRAW|SWP_NOACTIVATE);
}

void kWindow::reposition(kRect &r)
{
	SetWindowPos(NULL, r.x1, r.y1, r.x2-r.x1, r.y2-r.y1,
	  SWP_NOZORDER|SWP_NOREDRAW|SWP_NOACTIVATE);
}

afx_msg void kWindow::OnMouseMove( UINT fl, CPoint pt)
{
 kPoint p(pt.x,pt.y);
 on_mouse_move(p,fl);
 if(IsWindow(m_hWnd))
  CWnd::OnMouseMove(fl,pt);
}

void kWindow::size_to_content(void)
{
}
/*
void kWindow::add_tool(const TCHAR *,kWindow *,unsigned int )
{
}
*/

        // events
int kWindow::on_command(int ,int )
{
 return 0;
}

void kWindow::on_create()
{
}

void kWindow::on_destroy()
{
}

#ifdef KX_MFC7
afx_msg int kWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
 on_create();
 return CWnd::OnCreate(lpCreateStruct);
}
#else
afx_msg void kWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
 on_create();
 CWnd::OnCreate(lpCreateStruct);
}
#endif

afx_msg void kWindow::OnDestroy()
{
 on_destroy();
 CWnd::OnDestroy();
}


void kWindow::on_mouse_move(kPoint ,int )
{
}

void kWindow::on_mouse_l_up(kPoint ,int ) 
{
}

void kWindow::on_mouse_r_up(kPoint ,int ) 
{
}

void kWindow::on_mouse_m_up(kPoint ,int ) 
{
}

void kWindow::on_mouse_l_down(kPoint ,int ) 
{
}

void kWindow::on_mouse_r_down(kPoint ,int ) 
{
}

void kWindow::on_mouse_m_down(kPoint ,int ) 
{
}


afx_msg void kWindow::OnRButtonDown(UINT nFlags, CPoint point)
{
 kPoint p(point.x,point.y);

 on_mouse_r_down(p,(int)nFlags);

 if(IsWindow(m_hWnd))
  CWnd::OnRButtonDown(nFlags,point);
}

afx_msg void kWindow::OnRButtonUp(UINT nFlags, CPoint point)
{
 kPoint p(point.x,point.y);

 on_mouse_r_up(p,(int)nFlags);

 if(IsWindow(m_hWnd))
   CWnd::OnRButtonUp(nFlags,point);
}

afx_msg void kWindow::OnLButtonUp(UINT nFlags, CPoint point)
{
 kPoint p(point.x,point.y);

 on_mouse_l_up(p,(int)nFlags);

 if(IsWindow(m_hWnd))
   CWnd::OnLButtonUp(nFlags,point);
}

afx_msg void kWindow::OnLButtonDown(UINT nFlags, CPoint point)
{
 kPoint p(point.x,point.y);

 on_mouse_l_down(p,(int)nFlags);

 if(IsWindow(m_hWnd))
   CWnd::OnLButtonDown(nFlags,point);
}

afx_msg void kWindow::OnMButtonUp(UINT nFlags, CPoint point)
{
 kPoint p(point.x,point.y);

 on_mouse_m_up(p,(int)nFlags);

 if(IsWindow(m_hWnd))
   CWnd::OnMButtonUp(nFlags,point);
}

afx_msg void kWindow::OnMButtonDown(UINT nFlags, CPoint point)
{
 kPoint p(point.x,point.y);

 on_mouse_m_down(p,(int)nFlags);

 if(IsWindow(m_hWnd))
   CWnd::OnMButtonDown(nFlags,point);
}

void kWindow::on_mouse_l_dbl(kPoint ,int )
{
}
void kWindow::on_mouse_r_dbl(kPoint ,int )
{
}
void kWindow::on_mouse_m_dbl(kPoint ,int )
{
}

extern int gui_show_content;

void kWindow::OnNcRButtonUp(UINT nFlags, CPoint point)
{
 if(gui_show_content || (k_state&kSTATE_DRAGGING))
 {
  CWnd::OnNcRButtonUp(nFlags,point);
 }
 else
 {
  // since hit_test will return HTCAPTION, app will never get RButtonUp message
  OnRButtonUp(nFlags,point);
 }
}

void kWindow::OnLButtonDblClk(UINT nFlags, CPoint point)
{
 kPoint p(point.x,point.y);

 on_mouse_l_dbl(p,(int)nFlags);

 CWnd::OnLButtonDblClk(nFlags,point);
}

void kWindow::OnRButtonDblClk(UINT nFlags, CPoint point)
{
 kPoint p(point.x,point.y);

 on_mouse_r_dbl(p,(int)nFlags);

 CWnd::OnRButtonDblClk(nFlags,point);
}

void kWindow::OnMButtonDblClk(UINT nFlags, CPoint point)
{
 kPoint p(point.x,point.y);

 on_mouse_m_dbl(p,(int)nFlags);

 CWnd::OnMButtonDblClk(nFlags,point);
}

void kWindow::on_loose_focus() 
{
} // kill focus

afx_msg void kWindow::OnKillFocus(CWnd*w)
{
 on_loose_focus();
 CWnd::OnKillFocus(w);
}

void kWindow::on_loose_capture() 
{
} // on change capture

afx_msg void kWindow::OnCaptureChanged(CWnd*w)
{
 on_loose_capture();
 CWnd::OnCaptureChanged(w);
}

kFont::kFont() { flag=0; };
kFont::~kFont() { if(flag && font) { DeleteObject(font); font=0; flag=0; }; };


#ifdef KX_MFC7
void kWindow::OnDrawItem(int ctrl,LPDRAWITEMSTRUCT dis)
{
 kDraw *d=new kDraw(this);
 d->set_dis(dis);

 if(draw(*d)==1)
 {
  delete d;
  return CWnd::OnDrawItem(ctrl,dis);
 }
 delete d;
}
#else
BOOL kWindow::OnDrawItem(int ctrl,LPDRAWITEMSTRUCT dis)
{
 kDraw *d=new kDraw(this);
 d->set_dis(dis);

 if(draw(*d)==1)
 {
  delete d;
  CWnd::OnDrawItem(ctrl,dis);
  return TRUE;
 }
 delete d;

 return TRUE;
}
#endif

kDraw::kDraw(kWindow *from_)
{
 dc=BeginPaint(from_->get_wnd(),&ps);
 if(dc)
 {
  created=1;
  from=from_;
 }
}

kDraw::~kDraw()
{
 if(created)
 {
  created=0;
  EndPaint(from->get_wnd(),&ps);
 }
}

void kWindow::set_font(HFONT f)
{
 font.set_font(this,f);
}

void kWindow::set_font(kFont &f)
{
 font.set_font(this,f);
}

void kWindow::on_mouse_wheel(kPoint , int , int )
{
}

void kWindow::on_key_down(int , int , int )
{
}

void kWindow::on_char(int , int , int )
{
}

BOOL kWindow::OnMouseWheel( UINT fl, short del, CPoint pt)
{
 kPoint point(pt.x,pt.y);

 on_mouse_wheel(point,fl,del);

 return CWnd::OnMouseWheel(fl,del,pt);
}

void kWindow::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
 on_key_down(nChar,nRepCnt,nFlags);
 CWnd::OnKeyDown(nChar,nRepCnt,nFlags);
}

void kWindow::OnChar(UINT ch,UINT rep,UINT fl)
{
 on_char(ch,rep,fl);
 CWnd::OnChar(ch,rep,fl);
}

UINT kWindow::OnGetDlgCode() 
{
	return DLGC_WANTARROWS | CWnd::OnGetDlgCode();
}

void kWindow::on_focus()
{
}

void kWindow::OnSetFocus(CWnd* )
{
 set_focused();
 on_focus();
}

int kWindow::create(const TCHAR *caption, const TCHAR *class_,
	  kRect &rect, unsigned int style,
	  unsigned int id_, kWindow *parent_)
{
 k_parent=parent_;
 k_id=id_;
 _tcsncpy(k_class_name,class_,sizeof(k_class_name));

 RECT rr;
 rr.left=rect.x1; rr.top=rect.y1; rr.right=rect.x2; rr.bottom=rect.y2;
 return CWnd::Create(class_,caption,style,rr,parent_,k_id);
}

afx_msg BOOL kWindow::OnCommand(WPARAM w,LPARAM l)
{
 if(on_command((int)w,(int)l)==0)
  return CWnd::OnCommand((WPARAM)w,(LPARAM)l);
 else
  return 1;
}

void kWindow::on_post_nc_destroy()
{
}

void kWindow::PostNcDestroy()
{
 on_post_nc_destroy();
 CWnd::PostNcDestroy();
}

void kWindow::OnSysCommand(UINT id, LPARAM lp)
{
 on_sys_command((int)id,(int)lp);
}

void kWindow::on_sys_command(int wparam,int lparam)
{
 CWnd::OnSysCommand(wparam,lparam);
}

void kWindow::get_rect(kRect &r_)
{
 ::GetClientRect(m_hWnd,(LPRECT)&r_.x1);
}

void kWindow::get_rect_abs(kRect &r_)
{
 ::GetWindowRect(m_hWnd,(LPRECT)&r_.x1);
}

unsigned int kWindow::send_message(unsigned int message, unsigned int wparam,unsigned int lparam)
{
 return (unsigned int)SendMessage((UINT)message,(WPARAM)wparam,(LPARAM)lparam);
}

void kWindow::activate()
{
 ::SetActiveWindow(m_hWnd);
}

void kWindow::set_text(LPCTSTR s)
{
 ::SetWindowText(m_hWnd,s);
}

LRESULT kWindow::WindowProc(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
 return message((int)uMsg,(int)wParam,(int)lParam);
}

intptr_t kWindow::message(int msg,int wp,int lp) 
{ 
 return (int)CWnd::WindowProc(msg,wp,lp);
}

int kWindow::on_hit_test(kPoint pnt_)
{
 CPoint pnt(pnt_.x,pnt_.y);

 return CWnd::OnNcHitTest(pnt);
}

UINT kWindow::OnNcHitTest(CPoint point)
{
 kPoint pnt(point.x,point.y);

 return (UINT) on_hit_test(pnt);
}
