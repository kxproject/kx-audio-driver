// kX GUI
// Copyright (c) Eugene Gavrilov, Max Mikhailov 2002-2005.
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

#ifndef _KX_GUI_KNOB_H_
#define _KX_GUI_KNOB_H_

// kKnob

class kCLASS_TYPE kKnob : public kWindow
{
// construction
public:
	kKnob();
	virtual ~kKnob();

	unsigned int Style;
	kSize Size;

// implementation
public:
	int  GetPageStepSize();
	int  GetArrowStepSize();
	void SetPageStepSize(int StepSize);
	void SetArrowStepSize(int StepSize);
	int  get_range_min() const;
	void set_range_min(int nMin);
	int  get_range_max() const;
	void set_range_max(int nMax);
	int  get_pos() const;
	void set_pos(int nPos);
	void get_range(int& nMin, int& nMax) const;
	void set_range(int nMin, int nMax);

	BOOL create(DWORD dwStyle, const RECT& rect, kDialog* pParentWnd, UINT nID, int nMin = 0, int nMax = 100)
	{ return create(dwStyle, rect.left, rect.top, pParentWnd, nID, nMin, nMax, -150, 300); }
	BOOL create(DWORD dwStyle, long x, long y, kDialog* pParentWnd, UINT nID)
	{ return create(dwStyle, x, y, pParentWnd, nID, 0, 100, -150, 300); }
	BOOL create(DWORD dwStyle, long x, long y, kDialog* pParentWnd, UINT nID, int nMin, int nMax, int StartAngle = -150, int RotationAngle = 300);

	virtual const TCHAR *get_class_name() { return _T("kGuiKnob"); };

	virtual int draw(kDraw &);
	virtual void on_mouse_move(kPoint pt,int flags);
        virtual void on_mouse_l_up(kPoint pt,int flags);
        virtual void on_mouse_l_down(kPoint pt,int flags);
        virtual void on_mouse_wheel(kPoint pt, int flags, int delta);

        virtual void on_char(int ch, int rpt, int nFlags);
	
	afx_msg UINT OnGetDlgCode();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

// local
protected:	
	kRect FilmSize;
	int nFrames;
	int sframe;
	int rframes;

	CBitmap bmpKnob;

	BOOL UserMovesRuler;
	LONG MouseDownY;
	LONG MouseDownX;
	LONG MouseDownPos;

	int Pos;
	int RangeMax;
	int RangeMin;

	int ArrowStepSize;
	int PageStepSize;
	int MouseMoveSence;

protected:

	LRESULT NotifyParent(WPARAM NotifyCode = 0, LPARAM NotifyData = 0);
};

//.............................................................................

#endif // _KXU_AKNOB_H_
