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

#if !defined(kGUI_TEXTBUTTON_H__52789AD8_9B95_49F3_9BAD_3EA749987D3F__INCLUDED_)
#define kGUI_TEXTBUTTON_H__52789AD8_9B95_49F3_9BAD_3EA749987D3F__INCLUDED_

#pragma once

/////////////////////////////////////////////////////////////////////////////
// kButton

class kDialog;

class kCLASS_TYPE kTextButton : public kButton
{
public:
	kTextButton();

	BOOL create(const TCHAR * lpszCaption,
	  kRect rect_, kDialog *parent_, int id_,
	  kColor bk_=0xb0b000,kColor fg_=0xffff00,kColor text_bk_=0xb0b000,kColor text_fg_=0xffffff,
          kColor bk_o_=0xb0b000,kColor fg_o_=0xffff00,kColor text_bk_o_=0xb0b000,kColor text_fg_o_=0xffffff);

	virtual int draw(kDraw &d); 
	virtual int draw_nc(kDraw &d); // returns 1 - draw default

	virtual const TCHAR *get_class_name() { return _T("kGuiTextButton"); };

	void set_align(int a) { text_align=a; };
	virtual void set_text(kString text_) { kWindow::set_text((LPCTSTR)text_); text=text_; };

	void set_colors(kColor bk_=0xb0b000,kColor fg_=0xffff00,kColor text_bk_=0xb0b000,kColor text_fg_=0xffffff,
          kColor bk_o_=0xb0b000,kColor fg_o_=0xffff00,kColor text_bk_o_=0xb0b000,kColor text_fg_o_=0xffffff)
        {
         bk=bk_; fg=fg_; text_bk=text_bk_; text_fg=text_fg_; bk_o=bk_o_;
         fg_o=fg_o_; text_bk_o=text_bk_o_; text_fg_o=text_fg_o_; 
        };

private:
	kString text;
	kColor bk,fg,text_bk,text_fg,bk_o,fg_o,text_bk_o,text_fg_o;

	int text_align;
	#define kTEXT_DEFAULT	0	// left,top
	#define kTEXT_CENTER	1	// center,top
	#define kTEXT_RIGHT	2	// right,top
};

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(kGUI_TEXTBUTTON_H__52789AD8_9B95_49F3_9BAD_3EA749987D3F__INCLUDED_)
