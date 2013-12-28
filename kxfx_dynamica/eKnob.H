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

#define WM_UPDATE_STATIC (WM_USER+0x411)//message to update the control

class eKnob: public CStatic
{
	
public:
	eKnob();

	void SetID(UINT uID) { m_uID=uID; };//assigns an ID parameter to the control
	void SetBitmaps(HBITMAP bitmaps);//sets the bitmaps, 
	void SetPos(int nPos);
	void SetTicks(int nTicks, float nVelocity_x, float nVelocity_y);


public:
	~eKnob();

protected:
	UINT m_uID;

	//{{AFX_MSG(eKnob)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG

	  DECLARE_MESSAGE_MAP()
private:
	bool lb_down;
	int click_y, current_y, release_y, preserve_y;
	int click_x, current_x, release_x, preserve_x;
	long position;
	long rext, rint;
	int center, Ticks;
	HBITMAP bkg_knob;
	POINT mark[2];
	float Velocity_x, Velocity_y;
	
};


