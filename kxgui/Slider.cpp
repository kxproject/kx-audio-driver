// kX GUI
// Copyright (c) Eugene Gavrilov, 2001-2005.
// All rights reserved

// MySlider.cpp : implementation file
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


kSlider::kSlider()
{ 
 k_method=kMETHOD_DEFAULT;

 background=0;
 thumb=0;
 thumb_down=0;
 disabled=0;

 is_virtual=0;
};

kSlider::~kSlider()
{
}

void kSlider::size_to_content()
{
        ASSERT(background != NULL);
        BITMAP bmInfo;
        VERIFY(GetObject(background,sizeof(bmInfo), &bmInfo) == sizeof(bmInfo));
        VERIFY(::SetWindowPos(m_hWnd,NULL, -1, -1, bmInfo.bmWidth, bmInfo.bmHeight,
           SWP_NOMOVE|SWP_NOZORDER|SWP_NOREDRAW|SWP_NOACTIVATE));
}


/////////////////////////////////////////////////////////////////////////////
// kSlider

int kSlider::create(const TCHAR *name,DWORD style, kRect& rect, kDialog* parent_, UINT nID,int method_)
{
 BOOL ret=kWindow::create(name,TRACKBAR_CLASS, rect, style,nID,parent_);

 if(ret)
 {
  k_parent=parent_;
  if(background)
   size_to_content();
  if(parent_)
  {
   parent_->add_tool(name,this,nID);

   set_font(parent_->get_font()->get_font());
  }

  BITMAP bm;
  if(thumb)
  {
   ::GetObject(thumb, sizeof(bm), &bm);
   ::SendMessage(m_hWnd,TBM_SETTHUMBLENGTH,(::GetWindowLong(m_hWnd,GWL_STYLE)&TBS_VERT)?bm.bmHeight:bm.bmWidth,0);
  }
 }
 k_method=method_;
 return ret;
}

void kSlider::set_bitmaps(HBITMAP background_,HBITMAP thumb_,HBITMAP thumb_down_,HBITMAP disabled_)
{
  background=background_;
  thumb=thumb_;
  thumb_down=thumb_down_;
  disabled=disabled_;

  if(background && (k_method==kMETHOD_DEFAULT))
   k_method=kMETHOD_TRANS;

  if(IsWindow(m_hWnd))
  {
   BITMAP bm;
   if(thumb)
   {
    ::GetObject(thumb, sizeof(bm), &bm);
    ::SendMessage(m_hWnd,TBM_SETTHUMBLENGTH,(GetWindowLong(m_hWnd,GWL_STYLE)&TBS_VERT)?bm.bmHeight:bm.bmWidth,0);
   }
  }
}


/////////////////////////////////////////////////////////////////////////////
// kSlider message handlers

int kSlider::draw_nc(kDraw &)
{
 if(!background)
   return 0; // don't draw NC
 else
   return 1;
}

int kSlider::get_slider_width()
{
      ASSERT(background != NULL);
      BITMAP bmInfo;
      VERIFY(::GetObject(background,sizeof(bmInfo), &bmInfo) == sizeof(bmInfo));
      return bmInfo.bmWidth;
}

int kSlider::get_slider_height()
{
      ASSERT(background != NULL);
      BITMAP bmInfo;
      VERIFY(::GetObject(background,sizeof(bmInfo), &bmInfo) == sizeof(bmInfo));
      return bmInfo.bmHeight;
}

int kSlider::draw(kDraw &d)
{
 if(background)
 {
    HDC pDC=d.get_dc();
    if(pDC==0)
    {
        debug(_T("slider: BeginPaint failed"));
        return 1;
    }
    kRect ori_rect;
    get_rect(ori_rect);

    kRect rect;
    rect.init(&ori_rect);

    ::SetBkMode(pDC,TRANSPARENT);
    if((gui_has_sfx()==0)&&(k_method==kMETHOD_TRANS))
    {
     gui_transparent_blit(pDC,
          (::GetWindowLong(this->m_hWnd,GWL_STYLE)&WS_DISABLED)?(disabled?disabled:background):background,
          (short)rect.left,(short)rect.top,0xffffff);
         get_thumb_rect(rect);
         gui_transparent_blit(pDC,
          is_over()?(thumb_down?thumb_down:thumb):thumb,
          (short)rect.left,(short)rect.top,0xffffff);

         return 1;
        }

    {
    HDC memDC;
    memDC=CreateCompatibleDC(pDC);
    if(memDC==0)
    {
        debug(_T("slider: create pDC failed"));
        return 1;
    }

    HGDIOBJ pOld= SelectObject(memDC,(GetWindowLong(this->m_hWnd,GWL_STYLE)&WS_DISABLED)?(disabled?disabled:background):background);
    if (pOld == NULL)
    {
        debug(_T("slider: select memdc failed"));
        DeleteDC(memDC);
        return 1;     // destructors will clean up
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
        ::BitBlt(pDC,rect.left, rect.top, rect.width(), rect.height(),
          memDC, 0, 0, SRCCOPY);
        break;
    case kMETHOD_TRANS:
        ::TransparentBlt(pDC,rect.left,rect.top,
          rect.width(),rect.height(),memDC,0,0,rect.width(),rect.height(),0xffffff);
        break;
    case kMETHOD_BLEND:
        BLENDFUNCTION f;
        f.AlphaFormat=0; // AC_SRC_ALPHA;
        f.BlendFlags=0;
        f.BlendOp=AC_SRC_OVER;
        f.SourceConstantAlpha=(byte)alpha_; // 0..255 (transp->opaque)

        ::AlphaBlend(pDC,rect.left,rect.top,
          rect.width(),rect.height(),memDC,0,0,rect.width(),rect.height(),f);
        break;
    }

    SelectObject(memDC,pOld);
        DeleteDC(memDC);
        memDC=0;
        }

        // ----
    HDC memDC2;
    memDC2=CreateCompatibleDC(pDC);
    if(memDC2==0)
    {
        debug(_T("slider: create memDC2 failed"));
        return 1;
    }
    HGDIOBJ pOld2 = SelectObject(memDC2,is_over()?(thumb_down?thumb_down:thumb):thumb);
    if (pOld2 == NULL)
    {
        debug(_T("slider: select2 failed"));
        DeleteDC(memDC2);
        return 1;     // destructors will clean up
    }
    rect.init(&ori_rect);

    int alpha_=255*k_alpha/100;
    if(k_parent)
    {
        if(((kDialog *)k_parent)->is_moving())
            alpha_=255;
    }
        get_thumb_rect(rect);

    switch(k_method)
    {
    default:
    case kMETHOD_COPY:
        ::BitBlt(pDC,rect.left, rect.top, rect.width(), rect.height(),
           memDC2, 0, 0, SRCCOPY);
        break;
    case kMETHOD_TRANS:
        ::TransparentBlt(pDC,rect.left,rect.top,
           rect.width(),rect.height(),memDC2,0,0,rect.width(),rect.height(),0xffffff);
        break;
    case kMETHOD_BLEND:
        BLENDFUNCTION f;
        f.AlphaFormat=0; // AC_SRC_ALPHA;
        f.BlendFlags=0;
        f.BlendOp=AC_SRC_OVER;
        f.SourceConstantAlpha=(byte)alpha_; // 0..255 (transp->opaque)

        ::AlphaBlend(pDC,rect.left,rect.top,
           rect.width(),rect.height(),memDC2,0,0,rect.width(),rect.height(),f);
        break;
    }
    SelectObject(memDC2,pOld2);
    DeleteDC(memDC2);
  } 
   else
    return 1;

  return 0;
}

void kSlider::on_mouse_l_down(kPoint pt,int )
{
    set_over(0);

    kRect Tr;
    get_thumb_rect(Tr);

    kPoint pp; pp.x=pt.x; pp.y=pt.y;

    if(Tr.point_in_rect(pp))
    {
        set_over(1);
                // SetCapture();
    }

    ::InvalidateRect(m_hWnd, NULL, TRUE);
}

void kSlider::on_mouse_l_up(kPoint ,int )
{
    if(is_over())
    {
//   ReleaseCapture();
     set_over(0);
    }
    ::InvalidateRect(m_hWnd, NULL, TRUE);
}

void kSlider::on_mouse_move(kPoint ,int )
{
    if(is_over()) 
     ::InvalidateRect(m_hWnd, NULL, TRUE);
}

void kSlider::on_loose_capture()
{
    if(is_over())
    {
        set_over(0);
//      ReleaseCapture();
//      debug("kxslider: capture changed\n");
        ::InvalidateRect(m_hWnd, NULL, TRUE);
    }
}


int kSlider::erase_background(kDraw &)
{
    return FALSE;
}

void kSlider::get_thumb_rect(kRect& r)
{
/*
    BITMAP bm;

    GetObject(thumb, sizeof(bm), &bm);

    int min_p,max_p,pos;
    GetRange(min_p,max_p);
    pos=GetPos();

    CRect my_rect;
    GetClientRect(&my_rect);

//  CSliderCtrl::GetThumbRect(r);

    if((GetStyle() & TBS_VERT) == TBS_VERT)
    {
        pos=pos*(my_rect.height()-bm.bmHeight)/(max_p-min_p);

        r.left   = 0;
                r.right  = r.left + bm.bmWidth;

        r.top    = pos;
        r.bottom = r.top + bm.bmHeight;
    }
    else
    {
        pos=pos*(my_rect.width()-bm.bmWidth)/(max_p-min_p);

        r.top = 0;
        r.bottom = r.top + bm.bmHeight;

        r.left = pos;
        r.right = r.left + bm.bmWidth;
    }
}
*/
    BITMAP bm;

    RECT rr;
    ::SendMessage(get_wnd(), TBM_GETTHUMBRECT, 0, (LPARAM)&rr);
    r.x1=rr.left;
    r.y1=rr.top;
    r.x2=rr.right;
    r.y2=rr.bottom;

    if(thumb)
    {
        GetObject(thumb, sizeof(bm), &bm);
    }
    else
    {
        bm.bmWidth=rr.right-rr.left;
        bm.bmHeight=rr.top-rr.bottom;
    }

    if ((GetStyle() & TBS_VERT) == TBS_VERT) 
    {
        kRect mR;
        get_rect(mR);
        int o    = /*mR.width() - */(r.height() / 2);

        r.left   = 0;
        r.top    = (r.top + o) - (bm.bmHeight / 2);
        r.right  = r.left + bm.bmWidth;
        if(r.top < 0) r.top = 0;
        r.bottom = r.top + bm.bmHeight;
    }
    else
    {
        int o    = r.width() / 2;
        r.top = 0;
        r.bottom = r.top + bm.bmHeight;
        r.left = (r.left + o) - (bm.bmWidth / 2);
        r.right = r.left + bm.bmWidth;
    }
}

void kSlider::set_range(int min_,int max_,int redraw)
{
 // pre-3551:
 // this seems to cause problems (Windows allocates too much memory)
 // ::SendMessage(get_wnd(), TBM_SETRANGEMIN, redraw, min_);
 // ::SendMessage(get_wnd(), TBM_SETRANGEMAX, redraw, max_);

 if(min_>32767 || max_>32767 || min_<-32767 || max_<-32767)
 {
  if(min_==0 && max_==0x7fffffff) // dsp register
  {
    min_=0;
    max_=32767;
    is_virtual=1;
  }
  else
  {
    static err=1;
    if(err)
    {
        ::MessageBox(NULL,"ERROR: slider ranges are specified incorrectly\n You need to update your plugin code to fix this\n","Error",MB_ICONSTOP);
        err=0;
    }
  }
 }
 else
  is_virtual=0;

 ::SendMessage(get_wnd(), TBM_SETRANGE, redraw, MAKELPARAM(min_, max_));
}

void kSlider::set_tic_freq(int fr_)
{
 if(is_virtual)
  fr_=8192;

 ::SendMessage(get_wnd(), TBM_SETTICFREQ, fr_, 0L); 
}

void kSlider::set_pos(int pos_)
{
 if(is_virtual) // adjust pos
    pos_=pos_/65536;

 ::SendMessage(get_wnd(), TBM_SETPOS, TRUE, pos_);
}

int kSlider::get_range_max()
{
 if(is_virtual) return 0x7fffffff;

 return (int) ::SendMessage(m_hWnd, TBM_GETRANGEMAX, 0, 0l);
}

int kSlider::get_pos()
{
 if(!is_virtual)
    return (int) ::SendMessage(m_hWnd, TBM_GETPOS, 0, 0l);
 else
 {
    int pos=(int) (::SendMessage(m_hWnd, TBM_GETPOS, 0, 0l));
    if(pos==32767)
        return 0x7fffffff;
    
    return pos*65536;
 }
}

int kSlider::get_range_min()
{
 if(is_virtual) return 0x0;

 return (int) ::SendMessage(m_hWnd, TBM_GETRANGEMIN, 0, 0l);
}


int kSlider::set_page_size(int s)
{
 return (int) ::SendMessage(m_hWnd, TBM_SETPAGESIZE, 0, s);
}
