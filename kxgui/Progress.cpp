// kX GUI
// Copyright (c) Eugene Gavrilov, 2001-2005.
// All rights reserved

// kProgress class
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
// kProgress

kProgress::kProgress() : kWindow()
{
}


BOOL kProgress::create(kRect &r,unsigned int style,unsigned int id_,kWindow *parent_)
{
	// fixme: verify 'common controls'
	return kWindow::create(NULL,PROGRESS_CLASS, r, style, id_, parent_);
}

void kProgress::set_range(int min_,int max_)
{
 ::SendMessage(m_hWnd, PBM_SETRANGE, 0, MAKELPARAM(min_, max_));
}

int kProgress::get_pos()
{
 return (int) ::SendMessage(m_hWnd, PBM_GETPOS, 0, 0);
}

int kProgress::set_pos(int pos)
{
 return (int) ::SendMessage(m_hWnd, PBM_SETPOS, pos, 0);
}

int kProgress::set_step(int step)
{
 return (int) ::SendMessage(m_hWnd, PBM_SETSTEP, step, 0L);
}

int kProgress::set_bar_color(kColor c)
{
 return (int) ::SendMessage(get_wnd(),PBM_SETBARCOLOR, 0x0, (unsigned int)c);
}

int kProgress::set_bk_color(kColor c)
{
 return (int) ::SendMessage(get_wnd(),PBM_SETBKCOLOR, 0x0, (unsigned int)c);
}
