// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov, 2001-2004.
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

// Based on:
/*--------------------------------------------------------------------------------------*/
/*Simple rotary (knob) custom MFC control by Martin Borisov, based on CStatic.*/
/*--------------------------------------------------------------------------------------*/

// Dynamica
// (c) eYagos, 2004-2005. All rights reserved



#include "stdafx.h"
#include "eKnob.h"


eKnob::eKnob()
{
     
  lb_down=false;
  //current_y=0;
  //click_y=0;
  //release_y=0;
  
}

eKnob::~eKnob()
{
}
////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(eKnob, CStatic)
     //{{AFX_MSG_MAP
     ON_WM_PAINT()
	 ON_WM_LBUTTONDOWN()
	 ON_WM_LBUTTONUP()
	 ON_WM_MOUSEMOVE()
     //}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////


void eKnob::OnPaint() 
{
	CRect rect;
    GetClientRect(&rect);

    center = (rect.right - rect.left) / 2;
	rext = center - 3;
	rint = center - 5;

	mark[0].x = static_cast < int > (center + rint * cos(4.08407 - position * 5.02655 / (Ticks)));
	mark[0].y = static_cast < int > (center - rint * sin(4.08407 - position * 5.02655 / (Ticks)));
	mark[1].x = static_cast < int > (center + rext * cos(4.08407 - position * 5.02655 / (Ticks)));
	mark[1].y = static_cast < int > (center - rext * sin(4.08407 - position * 5.02655 / (Ticks)));



	CPaintDC dc(this);//create dc for painting 
    
	CBitmap bitmap;
    bitmap.Attach(bkg_knob);//attach current bitmap frame
	
	CDC dcMem;
    dcMem.CreateCompatibleDC (&dc);//create compatible memory DC
	CBitmap* pOldBitmap = dcMem.SelectObject (&bitmap);
    
	dc.BitBlt (0, 0, 26, 26, &dcMem, 0, 0, SRCCOPY);//blit memory DC to the visible PaintDC
	dcMem.SelectObject (pOldBitmap);
	
	LOGBRUSH logBrush;
	logBrush.lbStyle = BS_SOLID;
	logBrush.lbColor = RGB(154,164,173);
	CPen borde(PS_DOT|PS_GEOMETRIC|PS_ENDCAP_ROUND, 3, &logBrush);
	
	dc.SelectObject (borde); 
	dc.Polyline (mark, 2);
	
	bitmap.Detach();//detach current bitmap; necessary!
}

void eKnob::OnLButtonDown(UINT nFlags, CPoint point) 
{
   lb_down=true;
   click_y=point.y;//save y click position
   click_x=point.x;
   release_y = preserve_y;
   release_x = preserve_x;
   SetCapture();
}

void eKnob::OnLButtonUp(UINT nFlags, CPoint point) 
{
    lb_down=false;
	//release_y=current_y;//save previous "real" knob position
    ReleaseCapture(); 
}

void eKnob::OnMouseMove(UINT nFlags, CPoint point) 
{   
   
	if(lb_down){//if left button is down
	    current_y  = release_y + (int)((click_y - point.y)*Velocity_y) - (int)((click_x - point.x)*(Velocity_x));//calculate "real" knob position
		if (current_y < 0) current_y=0;
		if (current_y > (Ticks)) current_y=(Ticks);
		
		::PostMessage(  GetParent()->m_hWnd, WM_UPDATE_STATIC, (WPARAM)m_uID, (LPARAM)current_y );
	}
	
}



void eKnob::SetBitmaps(HBITMAP bitmaps){

	bkg_knob = bitmaps;
}


void eKnob::SetPos(int nPos)
{

    preserve_y = nPos;
	position = nPos;
	RedrawWindow();
}


void eKnob::SetTicks(int nTicks, float nVelocity_x, float nVelocity_y)
{
    Ticks = nTicks;
	Velocity_x = nVelocity_x;
	Velocity_y = nVelocity_y;
}

