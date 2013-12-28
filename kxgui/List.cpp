// kX GUI
// Copyright (c) Eugene Gavrilov, 2001-2005.
// All rights reserved

// kList class
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
// kList

kList::kList() : kWindow()
{
}


BOOL kList::create(kRect &r,unsigned int style,unsigned int id_,kWindow *parent_)
{
	return kWindow::create(NULL,_T("LISTBOX"), r, style, id_, parent_);
}
/*
int kList::get_selection()
{
 return (int)::SendMessage(get_wnd(), LB_GETCURSEL, 0, 0);
}

int kList::set_selection(int id)
{
 return (int)::SendMessage(get_wnd(), LB_SETCURSEL, id, 0);
}
*/
int kList::add_string(LPCTSTR str)
{
 return (int)::SendMessage(get_wnd(), LB_ADDSTRING, 0, (LPARAM)str);
}

int kList::reset()
{
 return (int)::SendMessage(get_wnd(), LB_RESETCONTENT, 0, 0);
}

int kList::get_text(int id,LPTSTR buff)
{
 return (int)::SendMessage(get_wnd(), LB_GETTEXT, id, (LPARAM)buff);
}

int kList::get_text(int id,kString &rString)
{
	ASSERT(::IsWindow(m_hWnd));
	int ret=get_text(id, rString.GetBufferSetLength(get_text_len(id)));
	rString.ReleaseBuffer();

	return ret;
}

int kList::get_text_len(int id)
{
 return (int)::SendMessage(get_wnd(), LB_GETTEXTLEN, id, 0);
}

int kList::get_multiple_selection(int max, int *array)
{
 return (int)::SendMessage(get_wnd(), LB_GETSELITEMS, max, (LPARAM)array);
}

int kList::get_selection(void)
{
 return (int)::SendMessage(get_wnd(), LB_GETCURSEL, 0, 0);
}
