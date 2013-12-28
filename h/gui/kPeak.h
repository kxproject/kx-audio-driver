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

#if !defined(kGUI_PEAK_H__F975EE5D_96E1_4505_BBE6_9F3813EA7D30__INCLUDED_)
#define kGUI_PEAK_H__F975EE5D_96E1_4505_BBE6_9F3813EA7D30__INCLUDED_

#pragma once

/////////////////////////////////////////////////////////////////////////////
// kPeak

class kCLASS_TYPE kPeak : public kWindow
{
public:
	kPeak();
	virtual ~kPeak();

	virtual void set_bitmaps(HBITMAP b1, HBITMAP b2);
	virtual BOOL create(const TCHAR *name,int x,int y, kDialog* pParentWnd,int mode);
        // mode: 0 - horizontal; 1-vertical

	virtual void size_to_content();
	virtual int draw(kDraw &d);
	virtual int draw_nc(kDraw &d); // returns 1 - draw default

	virtual const TCHAR *get_class_name() { return _T("kGuiPeak"); };

	virtual void set_value(int val_l,int val_r,int clip_l,int clip_r) { separator_l=val_l; separator_r=val_r; clipping_l=clip_l; clipping_r=clip_r; };

	virtual int get_peak_width() { return level_width; };
	virtual int get_peak_height() { return level_height; };

private:
        // internal vars
	HBITMAP level;
	HBITMAP level_high;
	HBITMAP level2;
	HBITMAP level_high2;
	HDC level_dc;
	HDC level_high_dc;
	int level_width,level_height;
	HBITMAP prev_level,prev_level_high;

	int mode;

	int separator_l,separator_r;
	int clipping_l,clipping_r;

	virtual int destroy_bitmaps();
};

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_STATIC_H__F975EE5D_96E1_4505_BBE6_9F3813EA7D30__INCLUDED_)
