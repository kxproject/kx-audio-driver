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

static int tt_initial=0,tt_popup=0,tt_reshow=0;

int kDialog::create(int idd,kDialog *parent_)
{
 k_parent=parent_;

 LPCTSTR lpszTemplateName=MAKEINTRESOURCE(idd);

 HINSTANCE hInst = AfxFindResourceHandle(lpszTemplateName, RT_DIALOG);
 HRSRC hResource = ::FindResource(hInst, lpszTemplateName, RT_DIALOG);
 HGLOBAL hTemplate = ::LoadResource(hInst, hResource);
   LPCDLGTEMPLATE lpTemplate = (LPCDLGTEMPLATE)LockResource(hTemplate);
   CreateDlgIndirect(lpTemplate, parent_, hInst);
//   BOOL bResult = CreateIndirect(lpTemplate, parent, NULL, hInst);
   ::UnlockResource(hTemplate);
 ::FreeResource(hTemplate);

 init();

 return 0;
}

LPCTSTR kRegisterWndClass(UINT nClassStyle,
	HCURSOR hCursor, HBRUSH hbrBackground, HICON hIcon,const TCHAR *class_name)
{
	// Returns a temporary string name for the class
	//  Save in a kString if you want to use it for a long time
	LPTSTR lpszName = AfxGetThreadState()->m_szTempClassName;

	// generate a synthetic name for this class
	HINSTANCE hInst = AfxGetInstanceHandle();
#ifdef KX_MFC7
	if (hCursor == NULL && hbrBackground == NULL && hIcon == NULL)
		wsprintf(lpszName, _T("Afx:%x:%x:%s"), hInst, nClassStyle,class_name);
	else
		wsprintf(lpszName, _T("Afx:%x:%x:%x:%x:%x:%s"), hInst, nClassStyle,
			hCursor, hbrBackground, hIcon,class_name);
#else
	if (hCursor == NULL && hbrBackground == NULL && hIcon == NULL)
		wsprintf(lpszName, _T("Afx:%p:%x:%s"), hInst, nClassStyle,class_name);
	else
		wsprintf(lpszName, _T("Afx:%p:%x:%p:%p:%p:%s"), hInst, nClassStyle,
			hCursor, hbrBackground, hIcon,class_name);
#endif

	// see if the class already exists
	WNDCLASS wndcls;
	if (::GetClassInfo(hInst, lpszName, &wndcls))
	{
		return lpszName;
	}

	// otherwise we need to register a new class
	wndcls.style = nClassStyle;
	wndcls.lpfnWndProc = DefWindowProc;
	wndcls.cbClsExtra = wndcls.cbWndExtra = 0;
	wndcls.hInstance = hInst;
	wndcls.hIcon = hIcon;
	wndcls.hCursor = hCursor;
	wndcls.hbrBackground = hbrBackground;
	wndcls.lpszMenuName = NULL;
	wndcls.lpszClassName = lpszName;
	if (!AfxRegisterClass(&wndcls))
		AfxThrowResourceException();

	// return thread-local pointer
	return lpszName;
}



int kDialog::create(kDialog *parent_,int style)
{
 k_parent=parent_;

/* WNDCLASS wndcls;

 memset(&wndcls, 0, sizeof(WNDCLASS));   // start with NULL
 wndcls.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;

 //you can specify your own window procedure
// wndcls.lpfnWndProc = ::DefWindowProc; 
 wndcls.hInstance = AfxGetInstanceHandle();
// wndcls.hIcon = LoadIcon(IDR_MAINFRAME); // or load a different icon
 wndcls.hCursor = LoadCursor(wndcls.hInstance,IDC_ARROW);
 wndcls.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
 wndcls.lpszMenuName = NULL;

 // Specify your own class name for using FindWindow later
 wndcls.lpszClassName = get_class_name();

 // Register the new class and exit if it fails
 AfxRegisterClass(&wndcls); // allow duplicates
*/
 if(CreateEx(WS_EX_APPWINDOW|WS_EX_CONTROLPARENT,/*get_class_name()*/
    kRegisterWndClass( CS_DBLCLKS|CS_OWNDC|CS_VREDRAW|CS_HREDRAW,
    			 LoadCursor(NULL,IDC_ARROW),
		    	 (HBRUSH) ::GetSysColorBrush(COLOR_3DFACE),
		    	 // (HBRUSH)::GetStockObject(COLOR_BTNFACE),
		    	 gui_get_default_icon()?gui_get_default_icon():LoadIcon(NULL, IDI_APPLICATION),
		    	 get_class_name()
    	                ),
    _T("kX"), // window name
    (style==-1)?((WS_POPUP | WS_SYSMENU | WS_MINIMIZEBOX | WS_TABSTOP)&(~WS_MAXIMIZEBOX)):(DWORD)style,
    0,0,1,1,k_parent?k_parent->get_wnd():NULL,NULL, // menu
    NULL)==0) // lpCreateParams
 {
  debug(_T("kxgui: window creation failed [%s]\n"),get_class_name());
  return -1;
 }

 init();

 return 0;
}

BOOL kDialog::on_command(int wparam,int lparam)
{
 int ret=0;

 if(wparam==IDOK)
  { on_ok(); final_result=IDOK; PostMessage(WM_CLOSE); ret=1; }
 if(wparam==IDCANCEL)
  { 
   on_cancel();
   PostMessage(WM_CLOSE); 
   ret=1; 
  }

 return ret?ret:kWindow::on_command(wparam,lparam);
}

int kDialog::do_modal(kDialog *parent_)
{
 if(get_wnd()==0)
 {
  create(parent_);
  CenterWindow();
 }

 show();

 BOOL bEnableParent = FALSE;
 HWND prnt=::GetParent(m_hWnd);
 if(prnt && prnt != ::GetDesktopWindow() && ::IsWindowEnabled(prnt))
 {
  	::EnableWindow(prnt, FALSE);
  	bEnableParent = TRUE;
 }

 final_result=IDCANCEL;
 MSG msg;
 while(GetMessage(&msg, NULL, 0U, 0U))
 {
 	TranslateMessage(&msg);
 	DispatchMessage(&msg);

 	if(msg.message==WM_CLOSE)
 	{
 	 break;
 	}
 	if(msg.message==WM_COMMAND)
 	{
 	 if(msg.wParam==IDOK)
 	 { on_ok(); final_result=IDOK; break; }
 	 if(msg.wParam==IDCANCEL)
 	 { on_cancel(); break; }
 	}
 	if(msg.message==WM_QUIT)
 	{
 	 if(prnt)
 	  ::PostMessage(prnt,WM_QUIT,0,0);
 	 break;
 	}
 }

 if(bEnableParent)
  	::EnableWindow(prnt, TRUE);
// if(prnt != NULL && ::GetActiveWindow() == m_hWnd)
//  	::SetActiveWindow(prnt);
 if(k_parent)
  k_parent->activate();

 return final_result;
}

kDialog::kDialog()
{ 
 capture=0; 
 k_bitmap_width=-1;
 k_bitmap_height=-1;

 background_bitmap=0;
 prev_bitmap=(HBITMAP)-1;
 mem_dc=0;
 k_transparent_color=RGB(0,0,0);
 region=0;
 set_region=0;
}

kDialog::~kDialog()
{
 if(is_moving()) 
 { 
  ReleaseCapture(); 
  set_moving(0); 
 } 
 if((prev_bitmap!=(HBITMAP)-1)&&(mem_dc))
 {
  SelectObject(mem_dc,prev_bitmap);
  prev_bitmap=(HBITMAP)-1;
 }

 /*
 gui_delete_bitmap(&background_bitmap);
 */

 if(region)
 {
  DeleteObject(region);
  region=0;
 }
 if(mem_dc)
 {
  DeleteDC(mem_dc);
  mem_dc=0;
 }
}

void kDialog::on_create()
{
 kWindow::on_create();

 if((k_method&kMETHOD_TRANS) && background_bitmap/*&&(has_sfx==0)*/)
 {
  HRGN region=gui_bitmap_region(background_bitmap,k_transparent_color);
  SetWindowRgn(region,TRUE);
 }
 if(background_bitmap)
 {
	GetObject(background_bitmap,sizeof(background_info),&background_info);
	mem_dc=CreateCompatibleDC(NULL);
	prev_bitmap=(HBITMAP)SelectObject(mem_dc,background_bitmap);
 }

 set_moving(0);
 set_dragging(0);
}

void kDialog::on_destroy()
{
 kWindow::on_destroy();
 if((prev_bitmap!=(HBITMAP)-1)&&(mem_dc))
 {
  SelectObject(mem_dc,prev_bitmap);
  prev_bitmap=(HBITMAP)-1;
 }

 /*
 gui_delete_bitmap(&background_bitmap);
 */

 if(region)
 {
  DeleteObject(region);
  region=0;
 }
 if(mem_dc)
 {
  DeleteDC(mem_dc);
  mem_dc=0;
 }
}

int kDialog::set_background(HBITMAP id,ULONG transparent_color_,int alpha_,int method_)
{
 if((prev_bitmap!=(HBITMAP)-1)&&(mem_dc))
 {
  SelectObject(mem_dc,prev_bitmap);
  prev_bitmap=(HBITMAP)-1;
 }
 /*
 gui_delete_bitmap(&background_bitmap);
 */

 if(id==0) // invalid bitmap
 {
  debug("kxgui: kDialog: bitmap=0\n");
  k_method=kMETHOD_DEFAULT;
  return -1;
 }

 if(region)
 {
  DeleteObject(region);
  region=0;
 }
 if(mem_dc)
 {
  DeleteDC(mem_dc);
  mem_dc=0;
 }
 k_method=method_;
 k_transparent_color=transparent_color_;
 k_alpha=alpha_;
 background_bitmap=id;

 if(k_method&kMETHOD_TILED)
  k_method&=(~kMETHOD_TRANS);

 if((k_method&kMETHOD_TRANS)/*&&(has_sfx==0)*/)
 {
  HRGN region=gui_bitmap_region(background_bitmap,k_transparent_color);
  SetWindowRgn(region,TRUE);
 }
 if((_set_layered_window_attributes)&&(!(k_method&kMETHOD_COPY))&&(!(k_method&kMETHOD_DEFAULT)))
 {
 	// Set WS_EX_LAYERED on this window 
 	SetWindowLong(this->m_hWnd, GWL_EXSTYLE,
        GetWindowLong(this->m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
 	// Make this window xx% alpha
 	int flag=0;
 	if(k_method&kMETHOD_BLEND)
 	 flag|=LWA_ALPHA;
        /*
 	if(method&kMETHOD_TRANS)
 	 flag|=LWA_COLORKEY;
        */
 	_set_layered_window_attributes(this->m_hWnd, k_transparent_color, (byte)((255*k_alpha)/100), flag);
 }
 GetObject(background_bitmap,sizeof(background_info),&background_info);
 mem_dc=CreateCompatibleDC(NULL);
 prev_bitmap=(HBITMAP)SelectObject(mem_dc,background_bitmap);

 k_bitmap_width=background_info.bmWidth;
 k_bitmap_height=background_info.bmHeight;

 return 0;
}

void kDialog::size_to_content(void)
{
	ASSERT(background_bitmap != NULL);
	BITMAP bmInfo;
	VERIFY(GetObject(background_bitmap,sizeof(bmInfo), &bmInfo) == sizeof(bmInfo));
	VERIFY(SetWindowPos(NULL, -1, -1, bmInfo.bmWidth, bmInfo.bmHeight,
	SWP_NOMOVE|SWP_NOZORDER|SWP_NOREDRAW|SWP_NOACTIVATE));
}

void kDialog::on_mouse_l_down(kPoint pt,int flags)
{
	if(!capture && !is_moving() && is_dragging())
	{
		::SetCapture(m_hWnd);
		capture=1;
		set_moving(1);
		capture_pos.left=pt.x;
		capture_pos.top=pt.y;
	}

	kWindow::on_mouse_l_down(pt,flags);
}


void kDialog::on_mouse_l_up(kPoint pt,int flags) 
{
	if(capture && is_moving())
	{
		::ReleaseCapture();
		capture=0;
		set_moving(0);
		if((gui_has_sfx()==0)&&(k_method&kMETHOD_BLEND))
		{
		  hide();
		  show();
		}
	}
	
	kWindow::on_mouse_l_up(pt,flags);
}

void kDialog::on_mouse_move(kPoint pt,int flags)
{
	if(capture && is_moving())
	{
		kRect r;
		get_rect_abs(r);
		int dx=pt.x-capture_pos.left;
		int dy=pt.y-capture_pos.top;
		r.left+=dx;
		r.right+=dx;
		r.top+=dy;
		r.bottom+=dy;
		::MoveWindow(m_hWnd,r.left,r.top,r.right-r.left,r.bottom-r.top,TRUE);
		set_moving(1);
	}

	kWindow::on_mouse_move(pt,flags);
}

int kDialog::erase_background(kDraw &d)
{
	if(background_bitmap)
	 return TRUE;
	else
	{
/*        	 HBRUSH c_b;
                 HBRUSH prev_b;
                 RECT rect;
                 GetWindowRect(&rect);
                 //rect.right-=rect.left; rect.bottom-=rect.right;

        	 c_b=CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
        	 prev_b=(HBRUSH)pDC->SelectObject(c_b);
        	 pDC->Rectangle(0,0,rect.right,rect.bottom);
                 pDC->SelectObject(prev_b);
        	 DeleteObject(c_b);
	 return 1; // erased
//
*/         return kWindow::erase_background(d);
	}
}


int kDialog::draw_nc(kDraw &)
{
	if(!background_bitmap)
	  return 1; // draw default
	else
          return 0; // draw nothing
}

int kDialog::draw(kDraw &d)
{
	if(background_bitmap)
	{
         HDC pDC=d.get_dc();

#if 0
         if(((!has_sfx) && (method&kMETHOD_BLEND) && (!is_moving)))
         {
         		BLENDFUNCTION f;
         		f.AlphaFormat=0; // AC_SRC_ALPHA;
         		f.BlendFlags=0;
         		f.BlendOp=AC_SRC_OVER;
         		f.SourceConstantAlpha=(byte)(255*alpha/100); // 0..255 (transp->opaque)

         		AlphaBlend(pDC,
                         	0, 0,
                 	        background_info.bmWidth,
                 	        background_info.bmHeight,
                 	        mem_dc,
                 	        0, 0,
                  	        background_info.bmWidth,
                  	        background_info.bmHeight,
         			f);
          }
          else
#endif

          {
              if(!(k_method&kMETHOD_TILED))
              {
         		BitBlt(pDC,
                          	0, 0,
                  	        get_bitmap_width(),
                  	        get_bitmap_height(),
                  	        mem_dc,
                  	        0, 0,
         			SRCCOPY);
              }
              else
              {
              		kRect rr;
              		get_rect(rr);

              		for(int x=0;x<rr.right;x+=get_bitmap_width())
              		 for(int y=0;y<rr.bottom;y+=get_bitmap_height())
              		 {
                                BitBlt(pDC,
                                	x, y,
                                        get_bitmap_width(),
                                        get_bitmap_height(),
                                        mem_dc,
                                        0, 0,
                                       SRCCOPY);
              		 }
              }
         }
	}
	else
	{
		 return 1; // draw default
	}
	return 0; // draw nothing
}

void kDialog::create_tooltip(int flags)
{
	if(t_t.m_hWnd==0)
	{
          t_t.Create(this,flags);
          if(tt_initial!=0)
          {
           t_t.SetDelayTime(TTDT_AUTOPOP,tt_popup);
           t_t.SetDelayTime(TTDT_INITIAL,tt_initial);
           t_t.SetDelayTime(TTDT_RESHOW,tt_reshow);
          }
        }
}

extern int gui_dll_versions[3];
extern int gui_balloon_tooltip;
extern int gui_show_content;

void kDialog::init()
{
	create_tooltip(/*TTS_ALWAYSTIP|*/((gui_dll_versions[2]>=0x50050)&&(gui_balloon_tooltip))?TTS_BALLOON:0);

        //SetCursor(LoadCursor( AfxGetInstanceHandle(),IDC_ARROW));
}

void kDialog::add_tool(const TCHAR *text,kWindow *w,UINT)
{
	TOOLINFO ti;

	memset(&ti, 0, sizeof(TOOLINFO));
	ti.cbSize = sizeof(TOOLINFO);

	HWND hwnd = w->get_wnd();

	ti.hwnd = this->get_wnd();
	ti.uFlags = TTF_IDISHWND|TTF_TRANSPARENT|TTF_SUBCLASS/*|TTF_PARSELINKS*//*|TTF_TRACK*/;
	ti.uId = (UINT_PTR)hwnd;

	ti.lpszText = (LPTSTR)text; // LPSTR_TEXTCALLBACK

	::SendMessage(this->t_t, TTM_ADDTOOL, 0, (LPARAM)&ti);
}

int kDialog::notify(kWindow * /*wnd*/,int /*msg*/,int /*wparam*/,int /*lparam*/)
{
 return 0;
}

void kDialog::set_tool_font(kFont &f)
{
 if(t_t.m_hWnd)
   t_t.SetFont(f);
}

void kDialog::update_tooltip()
{
 if(t_t.m_hWnd)
  t_t.Update();
}

int kCLASS_TYPE gui_set_tooltip_timings(int initial,int popup,int reshow)
{
 tt_initial=initial;
 tt_popup=popup;
 tt_reshow=reshow;

 return 0;
}

void kDialog::set_dragging(int set)
{
 if(set) 
   k_state|=kSTATE_DRAGGING; 
 else 
   k_state&=~kSTATE_DRAGGING; 
}

int kDialog::on_hit_test(kPoint pnt)
{
 if(gui_show_content || (k_state&kSTATE_DRAGGING))
  return kWindow::on_hit_test(pnt);
 else
  return HTCAPTION;
}
