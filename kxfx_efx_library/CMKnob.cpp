// EF-X Library
// Copyright (c) Martin Borisov, 2004.
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
#include "gui/efx/CMKnob.h"



CMKnob::CMKnob()
{
     
  lb_down=false;
  //current_y=0;
  //click_y=0;
  //release_y=0;

  
}

CMKnob::~CMKnob()
{
}
////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CMKnob, CStatic)
     //{{AFX_MSG_MAP
     ON_WM_PAINT()
	 ON_WM_LBUTTONDOWN()
	 ON_WM_LBUTTONUP()
	 ON_WM_MOUSEMOVE()
     //}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////


void CMKnob::OnPaint() 
{
	CRect rect;
    GetClientRect(&rect);

	CPaintDC dc(this);//create dc for painting
    
	CBitmap bitmap;
    bitmap.Attach(bm_knob_dc);//attach current bitmap frame
	
    
	CDC dcMem;
    dcMem.CreateCompatibleDC (&dc);//create compatible memory DC
    CBitmap* pOldBitmap = dcMem.SelectObject (&bitmap);
    
	dc.BitBlt  (0, 0, 100, 100, &dcMem, 0, 0, SRCCOPY);//blit memory DC to the visible PaintDC
	dcMem.SelectObject (pOldBitmap);
	bitmap.Detach();//detach current bitmap; necessary!
}

void CMKnob::OnLButtonDown(UINT nFlags, CPoint point) 
{
   
   lb_down=true;
   click_y=point.y;//save y click position
   release_y = preserve_y;
   SetCapture();
   
   
}

void CMKnob::OnLButtonUp(UINT nFlags, CPoint point) 
{
    lb_down=false;
	//release_y=current_y;//save previous "real" knob position
    ReleaseCapture(); 
	

}

void CMKnob::OnMouseMove(UINT nFlags, CPoint point) 
{   
   
	if(lb_down){//if left button is down
	    current_y  = release_y + (int)(click_y - point.y);//calculate "real" knob position
		if (current_y < 0) current_y=0;
		if (current_y > (FRAMES - 1)) current_y=(FRAMES - 1);
		
		::PostMessage(  GetParent()->m_hWnd,  WM_UPDATE_STATIC, (WPARAM)m_uID, (LPARAM)current_y );
	}
	
}



void CMKnob::SetBitmaps(HBITMAP bitmaps[FRAMES]){
    int i;
	for(i=0;i<FRAMES;i++){
	   bm_knob[i] = bitmaps[i];
   }
}


void CMKnob::SetPos(int nPos)
{
    bm_knob_dc = bm_knob[nPos];
    preserve_y = nPos;
	RedrawWindow();
}



