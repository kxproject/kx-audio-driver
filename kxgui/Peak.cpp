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


// Peak.cpp : implementation file
//

#include "stdafx.h"

#ifdef _DEBUG
 #define new DEBUG_NEW
 #undef THIS_FILE
 static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// kPeak

kPeak::kPeak()
{
          level=NULL;
          level_high=NULL;
          level_dc=NULL;
          level_high_dc=NULL;
          level2=NULL;
          level_high2=NULL;

          prev_level=NULL;
          prev_level_high=NULL;

          level_width=0;
          level_height=0;

          separator_l=0;
          separator_r=0;

          clipping_l=0;
          clipping_r=0;

          mode=0;
}

kPeak::~kPeak()
{
	destroy_bitmaps();
}

int kPeak::destroy_bitmaps()
{
         	if(level_dc)
         	{
         	 if(prev_level)
         	 {
         	   SelectObject(level_dc,prev_level);
         	   prev_level=0;
         	 }
         	 ::ReleaseDC(this->m_hWnd,level_dc); // FIXME: DC might be already removed
         	 level_dc=NULL;
         	}
         	if(level_high_dc)
         	{
         	 if(prev_level_high)
         	 {
         	   SelectObject(level_high_dc,prev_level_high);
         	   prev_level_high=0;
         	 }
         	 ::ReleaseDC(this->m_hWnd,level_high_dc); // FIXME: DC might be already removed
         	 level_high_dc=NULL;
         	}
         	if(level2)
         	{
         	 DeleteObject(level2);
         	 level2=NULL;
         	}
         	if(level_high2)
         	{
         	 DeleteObject(level_high2);
         	 level_high2=NULL;
         	}

         	return 0;
}

void kPeak::size_to_content()
{
		ASSERT(level != NULL);
		BITMAP bmInfo;
		VERIFY(GetObject(level,sizeof(bmInfo), &bmInfo) == sizeof(bmInfo));
		VERIFY(::SetWindowPos(m_hWnd,NULL, -1, -1, bmInfo.bmWidth, bmInfo.bmHeight,
		  SWP_NOMOVE|SWP_NOZORDER|SWP_NOREDRAW|SWP_NOACTIVATE));
};

void kPeak::set_bitmaps(HBITMAP b1,HBITMAP b2)
{
	destroy_bitmaps();

 	level=b1;
 	level_high=b2;
 	if(level)
 	{
 	 // create level dc here...
 	 BITMAP bmInfo;
 	 GetObject(level,sizeof(bmInfo), &bmInfo);
 	 level_width=bmInfo.bmWidth;
 	 level_height=bmInfo.bmHeight;
 	 level_dc=CreateCompatibleDC(NULL);
 	 prev_level=(HBITMAP)SelectObject(level_dc,level);
 	 if(prev_level==0)
 	 {
 	  // already used: duplicate bitmap:
 	  level2=(HBITMAP)CopyImage(level,IMAGE_BITMAP,0,0,0);
 	  prev_level=(HBITMAP)SelectObject(level_dc,level2);
 	  level=level2;
 	 }
 	}
 	if(level_high)
 	{
 	 level_high_dc=CreateCompatibleDC(NULL);
 	 prev_level_high=(HBITMAP)SelectObject(level_high_dc,level_high);
 	 if(prev_level_high==0)
 	 {
 	  // already used: duplicate bitmap:
 	  level_high2=(HBITMAP)CopyImage(level_high,IMAGE_BITMAP,0,0,0);
 	  prev_level_high=(HBITMAP)SelectObject(level_high_dc,level_high2);
 	  level_high=level_high2;
 	 }
 	}
}

BOOL kPeak::create(const TCHAR *text,int x,int y,kDialog *pParentWnd,int mode_)
{
	kRect r;

	mode=mode_;

	r.left=x; r.top=y; r.right=x+5; r.bottom=y+5;
	BOOL ret=kWindow::create(text,_T("STATIC"), r, SS_OWNERDRAW, 0, pParentWnd);

	size_to_content();

	k_parent=pParentWnd;

	k_method=kMETHOD_OWNER;

	if(pParentWnd)
	 pParentWnd->add_tool(text,this,0);

	return ret;
}

int kPeak::draw_nc(kDraw &)
{
  return 0; // draw nothing
}

int kPeak::draw(kDraw &dd)
{
	CRect rect;
	GetWindowRect((LPRECT)rect);

	HDC dc_=dd.get_dc();

	rect.bottom-=rect.top;
	rect.right-=rect.left;
	rect.left=0;
	rect.top=0;

	HDC dc=CreateCompatibleDC(dc_);
	HBITMAP memBM = CreateCompatibleBitmap(dc_,rect.right,rect.bottom);
	HBITMAP prevBM = (HBITMAP)SelectObject(dc,memBM);


       	  if((level==NULL)||(level_high==NULL))
       	   ;

       	  else
       	  {
                                           BitBlt(dc,
                                           	0, 0,
                                           	    level_width,
                                           	    level_height,
                                           	    level_dc,
                                           	    0, 0,
                                           SRCCOPY);
if(mode==0)
{
                                           BitBlt(dc,
                                            	0, 0,
                                                    separator_l,
                                                    level_height/2,
                                                    level_high_dc,
                                                    0, 0,
                                           SRCCOPY);

                                           BitBlt(dc,
                                            	0, level_height/2,
                                                    separator_r,
                                                    level_height/2,
                                                    level_high_dc,
                                                    0, level_height/2,
                                           SRCCOPY);

                                           if(clipping_l)
                                           { 
                                        	  HBRUSH c_b;
                                        	  HPEN c_p;
                                                  HPEN prev_p;
                                                  HBRUSH prev_b;

                                        	  c_b=CreateSolidBrush(0xff);
                                                  c_p=CreatePen(PS_SOLID,0,0xff);
                                        	  prev_b=(HBRUSH)SelectObject(dc,c_b);
                                                  prev_p=(HPEN)SelectObject(dc,c_p);
                                        	  Rectangle(dc,level_width-7,level_height/4-5,
                                        	               level_width-2,level_height/4+10);
                                		  SelectObject(dc,prev_p);
                                                  SelectObject(dc,prev_b);
                                        	  DeleteObject(c_b);
                                                  DeleteObject(c_p);
                                           }
                                           if(clipping_r)
                                           { 
                                        	  HBRUSH c_b;
                                        	  HPEN c_p;
                                                  HPEN prev_p;
                                                  HBRUSH prev_b;

                                        	  c_b=CreateSolidBrush(0xff);
                                                  c_p=CreatePen(PS_SOLID,0,0xff);
                                        	  prev_b=(HBRUSH)SelectObject(dc,c_b);
                                                  prev_p=(HPEN)SelectObject(dc,c_p);
                                        	  Rectangle(dc,level_width-7,level_height/2+level_height/4-5,
                                        	               level_width-2,level_height/2+level_height/4+10);
                                		  SelectObject(dc,prev_p);
                                                  SelectObject(dc,prev_b);
                                        	  DeleteObject(c_b);
                                                  DeleteObject(c_p);
                                           }
} // mode ==0
else // mode==1 (!=0)
{
                                           BitBlt(dc,
                                            	0, separator_l,
                                                    level_width/2,
                                                    level_height-separator_l,
                                                    level_high_dc,
                                                    0, separator_l,
                                           SRCCOPY);

                                           BitBlt(dc,
                                            	level_width/2+1,separator_r,
                                                    level_width/2+1,
                                                    level_height-separator_r,
                                                    level_high_dc,
                                                    level_width/2+1,separator_r,
                                           SRCCOPY);

}
        }

        BitBlt(dc_,
              	0, 0,
      	        rect.right-rect.left,
      	        rect.bottom-rect.top,
      	        dc,
      	        0, 0,
     		SRCCOPY);
     	SelectObject(dc,prevBM);
     	DeleteObject(memBM);
        DeleteDC(dc);

        return 0;
}
