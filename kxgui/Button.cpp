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


// Button.cpp : implementation file
//

#include "stdafx.h"

#ifdef _DEBUG
 #define new DEBUG_NEW
 #undef THIS_FILE
 static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// kButton

kButton::kButton() : kWindow()
{
	k_state=0;
	bitmap_normal=0;
	bitmap_over=0;
	bitmap_pressed=0;
	bitmap_default=0;
	bitmap_disabled=0;
}

kButton::~kButton()
{
}

void kButton::size_to_content()
{
	ASSERT(bitmap_normal != NULL);
	BITMAP bmInfo;
	VERIFY(GetObject(bitmap_normal,sizeof(bmInfo), &bmInfo) == sizeof(bmInfo));
	VERIFY(::SetWindowPos(this->m_hWnd,NULL, -1, -1, bmInfo.bmWidth, bmInfo.bmHeight,
	                      SWP_NOMOVE|SWP_NOZORDER|SWP_NOREDRAW|SWP_NOACTIVATE));
}

/*
BOOL kButton::create(const TCHAR *caption, unsigned int dwStyle,
	  RECT rect, CKXDialog* parent_, unsigned int id_)
{ 
 is_checkbox=0; 
 parent=NULL; 
 return CButton::Create(caption,dwStyle,rect,parent_,id_); 
}
*/

int kButton::set_default(int fl)
{ 
 if(fl)
  k_state|=kSTATE_DEFAULT;
 else
  k_state&=~kSTATE_DEFAULT;

 return 0;
}

BOOL kButton::create(const TCHAR *caption,kRect rect, kDialog* parent_, 
	  unsigned int id_,int method_)
{
 return create(caption,BS_OWNERDRAW,rect,parent_,id_,method_);
}

BOOL kButton::create(const TCHAR *caption,unsigned int style,kRect rect, kDialog* parent_, 
	  unsigned int id_,int method_)
{
	k_method=method_;

	if(!(k_method&kMETHOD_DEFAULT))
	 style=BS_OWNERDRAW;

	BOOL ret=kWindow::create(caption?caption:_T(""),_T("BUTTON"),rect,style,id_,parent_);

	if(k_parent && k_parent->get_font()->get_font())
	 set_font(k_parent->get_font()->get_font());

	if(k_parent && caption)
	 ((kDialog *)k_parent)->add_tool(caption,this,k_id);

	if(bitmap_normal)
	 size_to_content();

	return ret;
}



/////////////////////////////////////////////////////////////////////////////
// kButton message handlers

BOOL kButton::set_bitmaps(HBITMAP normal,HBITMAP over,HBITMAP pressed,HBITMAP default_,HBITMAP disabled)
{
	bitmap_normal=normal;
	bitmap_over=over;
	bitmap_pressed=pressed;
	bitmap_disabled=disabled;
	bitmap_default=default_;

	return 1;
}


int kButton::draw(kDraw &d)
{
	if(k_method&kMETHOD_DEFAULT)
	 return 1;

        HDC pDC=d.get_dc();

	kRect rect;
	get_rect(rect);

        // select bitmap
	HBITMAP bm;
        if(is_over())
        	bm = (HBITMAP)bitmap_over;
        else
        	bm = (HBITMAP)bitmap_normal;

        DRAWITEMSTRUCT dis;
        d.get_dis(&dis);

        int itemState=dis.itemState;

        if ((itemState & ODS_SELECTED) && (bitmap_pressed != NULL))
         		bm = (HBITMAP)bitmap_pressed;

	if(is_default())
	{
	 bm=(HBITMAP)bitmap_default;
	 if(bm==0)
	  bm=bitmap_pressed;
	}

        if((itemState & ODS_DISABLED)&&(bitmap_disabled))
         bm=(HBITMAP)bitmap_disabled;

	if(bm==0)
	{
	 debug(_T("button: Bitmap = 0 (kButton) - Severe error\n"));
//	 exit(7);
	 return 0;
	}

	if((gui_has_sfx()==0)&&(k_method&kMETHOD_TRANS)) // win95/98/98se
	{
	 gui_transparent_blit(pDC,bm,(short)rect.left,(short)rect.top,k_transparent_color);
	 return 0;
	}

	if(bitmap_normal==0)
	{
	 debug(_T("kButton::DrawItem [normal.m_hObject=0] -- Severe error\n"));
	 exit(6);
	}

	HDC pMemDC=CreateCompatibleDC(pDC);

	if(pDC==0)
	{
		debug(_T("button: BeginPaint failed -- Severe error\n"));
		exit(1);
	}

	// draw the whole button
	HBITMAP pOldBitmap = (HBITMAP)SelectObject(pMemDC,bm);
	if(pOldBitmap==0)
	{
		debug(_T("button: SelectObject failed - Severe error\n"));
		exit(9);
	}

	int alpha_=255*k_alpha/100;
	if(k_parent)
	{
		if(((kDialog *)k_parent)->is_moving())
			alpha_=255;
	}

	switch(k_method)
	{
	case kMETHOD_COPY:
	default:
		BitBlt(pDC,rect.left, rect.top, rect.width(), rect.height(),
		       pMemDC, 0, 0, SRCCOPY);
		break;

        case kMETHOD_TRANS:		
		TransparentBlt(pDC,rect.left,rect.top,
		       rect.width(),rect.height(),pMemDC,0,0,rect.width(),
		       rect.height(),k_transparent_color);
		break;
	case kMETHOD_BLEND:
		BLENDFUNCTION f;
		f.AlphaFormat=0; // AC_SRC_ALPHA;
		f.BlendFlags=0;
		f.BlendOp=AC_SRC_OVER;
		f.SourceConstantAlpha=(byte)alpha_; // 0..255 (transp->opaque)

		AlphaBlend(pDC,rect.left,rect.top,
			rect.width(),rect.height(),pMemDC,0,0,rect.width(),rect.height(),f);
		break;
	}
	// clean up
	SelectObject(pMemDC,pOldBitmap);
	DeleteDC(pMemDC);
	return 0; // draw nothing
}

int kButton::erase_background(kDraw &)
{
	return TRUE; // skip to be transparent
}


void kButton::on_mouse_move(kPoint point,int )
{
	kRect r;
	get_rect(r);

	if(point.x>=0 && point.y>=0 && 
		point.x<=r.width() && point.y<=r.height())
	{
		if(!is_over())
		{
			::SetCapture(m_hWnd);
			set_over(1);
			::InvalidateRect(m_hWnd, NULL, TRUE);
		}
	}
	else
	{
		if(is_over())
		{
			::ReleaseCapture();
			set_over(0);
			::InvalidateRect(m_hWnd, NULL, TRUE);
		}
	}
}

void kButton::on_loose_capture() 
{
	if (is_over())
	{
		set_over(0);
		::ReleaseCapture();
		::InvalidateRect(m_hWnd, NULL, TRUE);
	}
}

void kButton::on_loose_focus()
{
	set_over(0);
	::InvalidateRect(m_hWnd, NULL, TRUE);
}


void kButton::on_mouse_l_up(kPoint pt,int )
{
	kRect rr;
	get_rect(rr);
	kPoint point;
	point.x=pt.x; point.y=pt.y;
	if(rr.point_in_rect(point))
	{
		set_over(0);
		::InvalidateRect(m_hWnd, NULL, TRUE);
	}
}

void kCheckButton::set_check(int onoff)
{
 if(IsWindow(m_hWnd)&&(k_method&kMETHOD_DEFAULT))
 {
  ::SendMessage(m_hWnd, BM_SETCHECK, onoff, 0);
 }
 if(onoff)
  k_state|=kSTATE_CHECKED;
 else
  k_state&=(~kSTATE_CHECKED);
}

int kCheckButton::get_check()
{
 if(IsWindow(m_hWnd)&&(k_method&kMETHOD_DEFAULT))
  return (int)::SendMessage(m_hWnd, BM_GETCHECK, 0, 0);
 else
  return (k_state&kSTATE_CHECKED)?1:0;
}

kCheckButton::kCheckButton() : kButton()
{
	bitmap_checked=0;
	bitmap_unchecked=0;
	bitmap_checked_over=0;
	bitmap_unchecked_over=0;
	bitmap_checked_disabled=0;
	bitmap_unchecked_disabled=0;
}

BOOL kCheckButton::create(const TCHAR *caption,kRect rect, kDialog* parent_, 
	  unsigned int id_,int method_,int curstate)
{
 BOOL ret=kButton::create(caption,BS_AUTOCHECKBOX,rect,parent_,id_,method_);
 set_check(curstate);
 return ret;
}

BOOL kCheckButton::set_bitmaps(HBITMAP unchecked,HBITMAP unchecked_over,HBITMAP checked,HBITMAP checked_over,
	 HBITMAP unchecked_disabled,HBITMAP checked_disabled)
{
 bitmap_unchecked=unchecked;
 bitmap_unchecked_over=unchecked_over;
 bitmap_unchecked_disabled=unchecked_disabled;
 bitmap_checked=checked;
 bitmap_checked_over=checked_over;
 bitmap_checked_disabled=checked_disabled;

 if(bitmap_unchecked_over==0)
  bitmap_unchecked_over=bitmap_unchecked;
 if(bitmap_checked_over==0)
  bitmap_checked_over=bitmap_checked;

 if(get_check())
  kButton::set_bitmaps(bitmap_checked,bitmap_checked_over,bitmap_checked,
                              0,bitmap_checked_disabled);
 else
  kButton::set_bitmaps(bitmap_unchecked,bitmap_unchecked_over,bitmap_unchecked,
                              0,bitmap_unchecked_disabled);
 return 1;
}

int kCheckButton::draw(kDraw &d)
{
 if(get_check())
  kButton::set_bitmaps(bitmap_checked,bitmap_checked_over,bitmap_checked,
                              0,bitmap_checked_disabled);
 else
  kButton::set_bitmaps(bitmap_unchecked,bitmap_unchecked_over,bitmap_unchecked,
                              0,bitmap_unchecked_disabled);
 return kButton::draw(d);
}
