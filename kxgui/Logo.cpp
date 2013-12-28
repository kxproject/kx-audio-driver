// kX GUI
// Copyright (c) Eugene Gavrilov, 2001-2005.
// All rights reserved

// Logo.cpp : implementation file
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
// kLogo


kLogo::kLogo()
{
	bitmap=0;
}

kLogo::~kLogo()
{
/*
 gui_delete_bitmap(&bitmap);
 */
}

BOOL kLogo::create(DWORD style,DWORD ext_style,const TCHAR *name,int x,int y,
	  HBITMAP bm_id, int method_,CWnd *parent)
{
	LPCTSTR tmp_class=AfxRegisterWndClass(0,
	 0, 0, 0);

        /*
        gui_delete_bitmap(&bitmap);
        */

	bitmap=bm_id;

	k_method=method_;
	BITMAP bm_info;
	GetObject(bitmap,sizeof(bm_info),&bm_info);
	
	RECT r;
	r.left=x;
	r.top=y;
	r.right=r.left+bm_info.bmWidth-1;
	r.bottom=r.top+bm_info.bmHeight-1;

	BOOL ret=CWnd::CreateEx(ext_style,
	  tmp_class, name, style, r, parent, 0);

	k_parent=NULL;

	return ret;
}

void kLogo::size_to_content()
{
		ASSERT(bitmap != NULL);
		BITMAP bmInfo;
		VERIFY(GetObject(bitmap,sizeof(bmInfo), &bmInfo) == sizeof(bmInfo));
		VERIFY(::SetWindowPos(m_hWnd,NULL, -1, -1, bmInfo.bmWidth, bmInfo.bmHeight,
		  SWP_NOMOVE|SWP_NOZORDER|SWP_NOREDRAW|SWP_NOACTIVATE));
}


int kLogo::draw_nc(kDraw &)
{
 return 0; // draw nothing
}

int kLogo::draw(kDraw &d)
{
	kRect rect;
	get_rect(rect);

	rect.bottom-=rect.top;
	rect.right-=rect.left;
	rect.left=0;
	rect.top=0;

        HDC pDC=d.get_dc();
        
	if(pDC==NULL)
	{
	  debug(_T("myLogo: BeginPaint failed"));
	  exit(10);
	}

	HDC memDC=CreateCompatibleDC(pDC);
	if(memDC==0)
	{
	  debug(_T("myLogo: CreateCompatibleDC failed"));
	  exit(11);
	}

	HGDIOBJ pOld = SelectObject(memDC,bitmap);
	if (pOld == NULL)
	{
		debug(_T("myLogo: Select object failed"));
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
		BitBlt(pDC,rect.left, rect.top, rect.width(), rect.height(),
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

 return 0; // draw nothing
}
