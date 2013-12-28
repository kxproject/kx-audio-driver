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

#if !defined(kX_MENU_H__52789AD8_9B95_49F3_9BAD_3EA749987D3F__INCLUDED_)
#define kX_MENU_H__52789AD8_9B95_49F3_9BAD_3EA749987D3F__INCLUDED_

#pragma once

/////////////////////////////////////////////////////////////////////////////
// kList

class kCLASS_TYPE kMenu
{
public:
	kMenu();
	~kMenu();

	int create();	// creates a pop-up menu
	int destroy();
	int track(int flag,int x,int y,kWindow *wnd);
	int track(int flag,int x,int y,uintptr_t hwnd);

	int add(TCHAR *str,int id,int flag=0);
	int add(TCHAR *str,kMenu *menu,int flag=0);
	int add(kString,int id,int flag=0);
	int add(kString,kMenu *menu,int flag=0);
	int separator();

private:
	HMENU mnu;
};

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(kX_MENU_H__52789AD8_9B95_49F3_9BAD_3EA749987D3F__INCLUDED_)

