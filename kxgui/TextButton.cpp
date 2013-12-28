// kX GUI
// Copyright (c) Eugene Gavrilov, 2001-2005.
// All rights reserved

// kTextButton class
//

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

#ifdef _DEBUG
 #define new DEBUG_NEW
 #undef THIS_FILE
 static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// kTextButton

kTextButton::kTextButton() : kButton()
{
	// set_over(0);

	text=_TEXT("");
	bk=0xffffff;
	fg=0x0;
	text_bk=0x0;
	text_fg=0xffffff;
	bk_o=0xffffff;
	fg_o=0x0;
	text_bk_o=0x0;
	text_fg_o=0xffffff;
	text_align=kTEXT_DEFAULT;
}


BOOL kTextButton::create(const TCHAR *lpszCaption,
	  kRect rect, kDialog* parent_, int nID,
	  kColor bk_,kColor fg_,kColor text_bk_,kColor text_fg_,
          kColor bk_o_,kColor fg_o_,kColor text_bk_o_,kColor text_fg_o_)
{
	BOOL ret=kWindow::create(lpszCaption?lpszCaption:_T(""),_T("BUTTON"),rect,BS_OWNERDRAW,nID,parent_);
	set_method(kMETHOD_OWNER);
	k_parent=parent_;

	bk=bk_; fg=fg_;
	bk_o=bk_o_; fg_o=fg_o_;
	text_bk=text_bk_; text_fg=text_fg_;
	text_bk_o=text_bk_o_; text_fg_o=text_fg_o_;

	text=lpszCaption;

	if(k_parent && lpszCaption)
	 parent_->add_tool(lpszCaption,this,nID);

	if(parent_ && parent_->get_font()->get_font())
	 set_font(parent_->get_font()->get_font());

	return ret;
}


int kTextButton::draw_nc(kDraw &)
{
 return 1;
}

int kTextButton::draw(kDraw &d)
{
        HDC pDC=d.get_dc();

	kRect rect;
	get_rect(rect);

	kColor bk_d,fg_d,text_bk_d,text_fg_d;
	if(is_over())
	 { bk_d=bk_o; fg_d=fg_o; text_bk_d=text_bk_o; text_fg_d=text_fg_o; }
	else
	 { bk_d=bk; fg_d=fg; text_bk_d=text_bk; text_fg_d=text_fg; }

	SetBkColor(pDC,text_bk_d);
	SetTextColor(pDC,text_fg_d);
        HBRUSH b=CreateSolidBrush(bk_d);
        HBRUSH prev_b=(HBRUSH)SelectObject(pDC,b);
        HPEN p=CreatePen(PS_SOLID,0,fg_d);
        HPEN prev_p=(HPEN)SelectObject(pDC,p);

        kRect c_r;
        get_rect(c_r);
        Rectangle(pDC,c_r.left,c_r.top,c_r.right,c_r.bottom);

        TCHAR tmp_str[256];
        GetWindowText(tmp_str,sizeof(tmp_str));

        HFONT prev_f=(HFONT)SelectObject(pDC,(HFONT)font);

        switch(text_align)
        {
         case kTEXT_DEFAULT:
         	ExtTextOut(pDC,1,1,ETO_OPAQUE,NULL,tmp_str,(UINT)_tcslen(tmp_str),NULL);
         	break;
         case kTEXT_CENTER:
         	SetTextAlign(pDC,TA_CENTER);
         	ExtTextOut(pDC,c_r.width()/2,1,ETO_OPAQUE,NULL,tmp_str,(UINT)_tcslen(tmp_str),NULL);
         	break;
         case kTEXT_RIGHT:
         	SetTextAlign(pDC,TA_RIGHT);
         	ExtTextOut(pDC,c_r.width(),1,ETO_OPAQUE,NULL,tmp_str,(UINT)_tcslen(tmp_str),NULL);
         	break;
        }

        SelectObject(pDC,prev_f);
        SelectObject(pDC,prev_b);
        SelectObject(pDC,prev_p);
        DeleteObject(b);
        DeleteObject(p);

        return 0;
}

