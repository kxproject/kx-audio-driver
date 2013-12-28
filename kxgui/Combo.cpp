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


// kCombo class
//

#include "stdafx.h"

#ifdef _DEBUG
 #define new DEBUG_NEW
 #undef THIS_FILE
 static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// kCombo

kCombo::kCombo() : kWindow()
{
}


BOOL kCombo::create(kRect &r,unsigned int style,unsigned int id_,kWindow *parent_)
{
    return kWindow::create(NULL,_T("COMBOBOX"), r, style, id_, parent_);
}

int kCombo::get_selection()
{
 return (int)::SendMessage(get_wnd(), CB_GETCURSEL, 0, 0);
}

int kCombo::set_selection(int id)
{
 return (int)::SendMessage(get_wnd(), CB_SETCURSEL, id, 0);
}

int kCombo::set_dropped_width(int wd)
{
 return (int)::SendMessage(get_wnd(), CB_SETDROPPEDWIDTH, wd, 0);
}

int kCombo::add_string(LPCTSTR str)
{
 // Windows 7 needs a workaround
 // string might be stored in code segment, or in read-only memory
 // need to make a copy
 // 3551 change

 CString s=str;

 return (int)::SendMessage(get_wnd(), CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)s);
}
