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


/*Simple rotary (knob) custom MFC control by Martin Borisov, based on CStatic.

The following code may be used or modified freely for kX driver development purposes. 
If used for other purposes please contact Martin Borisov via this e-mail address:
  martin.borisov@abv.bg

*/

#define WM_UPDATE_STATIC (WM_USER+0x411)//message to update the control
#define FRAMES 61//number of bitmap frames, should be set here

class kCLASS_TYPE CMKnob: public CStatic
{
	
public:
	CMKnob();

	void SetID(UINT uID) { m_uID=uID; };//assigns an ID parameter to the control
	void SetBitmaps(HBITMAP bitmaps[FRAMES - 1]);//sets the bitmaps, 
	//int GetPos();//not implemented yet
	void SetPos(int nPos);


public:
	~CMKnob();

protected:
	UINT m_uID;

	//{{AFX_MSG(CMKnob)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG

	  DECLARE_MESSAGE_MAP()
private:
	bool lb_down;
	int click_y, current_y, release_y, preserve_y;
	HBITMAP bm_knob[FRAMES], bm_knob_dc;

};


