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

#if !defined(kX_CHECK_BUTTON_H__52789AD8_9B95_49F3_9BAD_3EA749987D3F__INCLUDED_)
#define kX_CHECK_BUTTON_H__52789AD8_9B95_49F3_9BAD_3EA749987D3F__INCLUDED_

#pragma once

/////////////////////////////////////////////////////////////////////////////
// kCheckButton

class kCLASS_TYPE kCheckButton : public kButton
{
public:
	kCheckButton();

	BOOL create(const TCHAR *caption,kRect rect, kDialog* parent_, 
	  unsigned int id_,int method_,int cur_state);

	BOOL set_bitmaps(HBITMAP unchecked,HBITMAP unchecked_over,HBITMAP checked,HBITMAP checked_over,
	 HBITMAP unchecked_disabled=0,HBITMAP checked_disabled=0);

        virtual void set_check(int onoff);
        virtual int get_check();

        virtual int draw(kDraw &d);

        virtual const TCHAR *get_class_name() { return _T("kGuiCheckButton"); };

private:
	HBITMAP bitmap_unchecked;
	HBITMAP bitmap_unchecked_over;
	HBITMAP bitmap_checked;
	HBITMAP bitmap_checked_over;
	HBITMAP bitmap_unchecked_disabled;
	HBITMAP bitmap_checked_disabled;
};

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(kX_CHECK_BUTTON_H__52789AD8_9B95_49F3_9BAD_3EA749987D3F__INCLUDED_)

