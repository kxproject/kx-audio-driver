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

#if !defined(kX_BUTTON_H__52789AD8_9B95_49F3_9BAD_3EA749987D3F__INCLUDED_)
#define kX_BUTTON_H__52789AD8_9B95_49F3_9BAD_3EA749987D3F__INCLUDED_

#pragma once

/////////////////////////////////////////////////////////////////////////////
// kButton

class kCLASS_TYPE kButton : public kWindow
{
public:
	kButton();
	~kButton();

	BOOL create(const TCHAR *caption,kRect rect, kDialog* parent_, 
	  unsigned int id_,int method_);
	BOOL create(const TCHAR *caption,unsigned int style,kRect rect, kDialog* parent_, 
	  unsigned int id_,int method_);

	BOOL set_bitmaps(HBITMAP normal,HBITMAP over,HBITMAP pressed,HBITMAP default_=0,
	 HBITMAP disabled=0);

	virtual int set_default(int flag_);
        virtual int is_default() { return (k_state&kSTATE_DEFAULT)?1:0; };

        virtual void size_to_content(void);

        virtual int draw(kDraw &d);
        virtual int erase_background(kDraw &d);

        virtual void on_mouse_move(kPoint pt,int flags);
        virtual void on_mouse_l_up(kPoint pt,int flags);
        virtual void on_loose_focus(); // kill focus
        virtual void on_loose_capture(); // on change capture

        virtual const TCHAR *get_class_name() { return _T("kGuiButton"); };


private:
	HBITMAP bitmap_over;
	HBITMAP bitmap_normal;
	HBITMAP bitmap_default;
	HBITMAP bitmap_pressed;
	HBITMAP bitmap_disabled;

	int mode;
};

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(kX_BUTTON_H__52789AD8_9B95_49F3_9BAD_3EA749987D3F__INCLUDED_)

