// kX GUI
// Copyright (c) Eugene Gavrilov, 2001-2005.
// All rights reserved

// Static.cpp : implementation file
//

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


/////////////////////////////////////////////////////////////////////////////
// kStatic

kStatic::kStatic()
{
	bitmap=0;
	bk=0x0;
	fg=0xffffff;
}

kStatic::~kStatic()
{
}

void kStatic::size_to_content()
{
		ASSERT(bitmap != NULL);
		BITMAP bmInfo;
		VERIFY(GetObject(bitmap,sizeof(bmInfo), &bmInfo) == sizeof(bmInfo));
		VERIFY(::SetWindowPos(m_hWnd,NULL, -1, -1, bmInfo.bmWidth, bmInfo.bmHeight,
		  SWP_NOMOVE|SWP_NOZORDER|SWP_NOREDRAW|SWP_NOACTIVATE));
};

BOOL kStatic::create(const TCHAR *text,int x,int y,int wd,int ht,kDialog *pParentWnd,kColor fg_,kColor bk_)
{
	kRect r(x,y,x+wd,y+ht);
	BOOL ret=kWindow::create(text, _T("STATIC"), r, SS_OWNERDRAW, 0, pParentWnd);
	k_parent=pParentWnd;

	k_method=kMETHOD_OWNER;

	if(pParentWnd)
	 pParentWnd->add_tool(text,this,0);

	bk=bk_;
	fg=fg_;

	return ret;
}


BOOL kStatic::create(const TCHAR *name,int x,int y, HBITMAP id, kDialog* pParentWnd,int method_)
{
	bitmap=id;

	k_method=method_;
	BITMAP bm_info;
	::GetObject(bitmap,sizeof(bm_info),&bm_info);
	
	kRect r(x,y,x+bm_info.bmWidth-1,y+bm_info.bmHeight-1);

	BOOL ret=kWindow::create(name, _T("STATIC"), r, SS_OWNERDRAW, 0, pParentWnd);
	k_parent=pParentWnd;

	if(k_parent && k_parent->get_font()->get_font())
	  ::SendMessage(this->get_wnd(), WM_SETFONT, (WPARAM)k_parent->get_font()->get_font(), 1);

	if(k_parent)
	 ((kDialog *)k_parent)->add_tool(name,this,0);

	return ret;
}

BOOL kStatic::create(const TCHAR *label,DWORD style, kRect &r, kDialog *parent, UINT id)
{
 k_method=kMETHOD_DEFAULT;

 return kWindow::create(label,_T("STATIC"),r,style,id,parent);
}


int kStatic::draw_nc(kDraw &)
{
 if(k_method==kMETHOD_DEFAULT)
  return 1;
 else
  return 0; // draw nothing
}

int kStatic::draw(kDraw &dd)
{
	kRect rect;
	get_rect(rect);

        HDC pDC=dd.get_dc();
	if(pDC==NULL)
	{
	  debug(_T("kStatic: BeginPaint failed"));
	  exit(10);
	}

	if(k_method==kMETHOD_OWNER) // owner draw static text
	{
           ::SetTextColor(pDC,fg);
           ::SetBkColor(pDC,bk);

           HPEN p=CreatePen(PS_SOLID,0,fg);
           HPEN prev_p=(HPEN)SelectObject(pDC,p);

           HBRUSH b=CreateSolidBrush(bk);
           HBRUSH prev_b=(HBRUSH)SelectObject(pDC,b);
           
           HFONT prev_f=0;
           if(k_parent->get_font()->get_font())
            prev_f=(HFONT)SelectObject(pDC,k_parent->get_font()->get_font());
           RECT c_r;
           GetClientRect(&c_r);
           Rectangle(pDC,c_r.left,c_r.top,c_r.right,c_r.bottom);
           TCHAR tmp_str[256];
           ::GetWindowText(m_hWnd,tmp_str,sizeof(tmp_str));
           ExtTextOut(pDC,1,1,ETO_OPAQUE,NULL,tmp_str,(UINT)_tcslen(tmp_str),NULL);

           SelectObject(pDC,prev_b);
           SelectObject(pDC,prev_p);
           if(prev_f)
            SelectObject(pDC,prev_f);

           DeleteObject(b);
           DeleteObject(p);
	}
	else
	 if(k_method==kMETHOD_DEFAULT)
	 {
	  return 1; // draw default
	 }
	else
	{
        	if(/*(parent->has_sfx==0)&&*/(k_method==kMETHOD_TRANS)) // win95/98/98se
        	{
        	 gui_transparent_blit(pDC,bitmap,(short)rect.left,(short)rect.top,k_transparent_color);
                 return 0;
        	}


        	HDC memDC=CreateCompatibleDC(pDC);
        	if(memDC==0)
        	{
        	  debug(_T("kStatic: CreateCompatibleDC failed"));
        	  exit(11);
        	}

        	HGDIOBJ pOld = SelectObject(memDC,bitmap);
        	if (pOld == NULL)
        	{
        		debug(_T("kStatic: Select object failed"));
        		DeleteDC(memDC);
        		return 0;     // destructors will clean up
        	}
        	SetBkMode(pDC,TRANSPARENT);
        	SetBkMode(memDC,TRANSPARENT);

        	int alpha_=255*k_alpha/100;
        	if(k_parent)
        	{
        		if(((kDialog *)k_parent)->is_moving())
        			alpha_=255;
        	}
        	
        	switch(k_method)
        	{
        	default:
        	case kMETHOD_COPY:
        		::BitBlt(pDC,rect.left, rect.top, rect.width(), rect.height(),
        			memDC, 0, 0, SRCCOPY);
        		break;
        	case kMETHOD_BLEND:
        		{
        		BLENDFUNCTION f;
        		f.AlphaFormat=0;
        		f.BlendFlags=0;
        		f.BlendOp=AC_SRC_OVER;
        		f.SourceConstantAlpha=(byte)alpha_; // 0..255 (transp->opaque)
        		
        		::AlphaBlend(pDC,rect.left,rect.top,
        		  rect.width(),rect.height(),memDC,0,0,rect.width(),rect.height(),f);
        		}
        		break;
        	case kMETHOD_TRANS:	
        		{
        		::TransparentBlt(pDC,rect.left,rect.top,
        		   rect.width(),rect.height(),memDC,0,0,rect.width(),rect.height(),0xffffff);
        		}
        		break;
        	}
        	if(pOld)
        	 SelectObject(memDC,pOld);
		DeleteDC(memDC);
	} // method!=-1

	 return 0; // draw nothing
}
