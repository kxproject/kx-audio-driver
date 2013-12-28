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


#include "stdafx.h"

kMenu::kMenu()
{
 mnu=0;
}


kMenu::~kMenu()
{
 if(mnu)
  destroy();
}


int kMenu::create()	// creates a pop-up menu
{
 mnu=CreatePopupMenu();

 return mnu?0:-1;
}


int kMenu::destroy()
{
 if(mnu)
 {
  DestroyMenu(mnu);
  mnu=0;
 }

 return 0;
}


int kMenu::track(int flag,int x,int y,kWindow *wnd)
{
 return track(flag,x,y,(uintptr_t)wnd->get_wnd());
}


int kMenu::track(int flag,int x,int y,uintptr_t hwnd)
{
 return TrackPopupMenuEx(mnu,flag,x,y,(HWND)hwnd,NULL);
}

int kMenu::add(TCHAR *str,int id,int flag)
{
 if(mnu)
 {
  AppendMenu(mnu,MFT_STRING|flag,id,str);
  if(id==0)
   debug("kx gui: menu invalid [%s]\n",str);
  return 0;
 }
 else return -2;
}


int kMenu::add(TCHAR *str,kMenu *menu,int flag)
{
 if(mnu)
 {
  AppendMenu(mnu,MFT_STRING|MF_POPUP|flag,(UINT_PTR)menu->mnu,str);
  if((UINT_PTR)menu->mnu==0)
   debug("kx gui: menu invalid [%s]\n",str);
  return 0;
 }
 else return -2;
}

int kMenu::separator()
{
 if(mnu)
 {
  AppendMenu(mnu,MFT_SEPARATOR,0,"");
  return 0;
 }
 else return -2;
}

int kMenu::add(kString s,int id,int flag)
{
 return add((TCHAR *)(LPCTSTR)s,id,flag);
}

int kMenu::add(kString s,kMenu *menu,int flag)
{
 return add((TCHAR *)(LPCTSTR)s,menu,flag);
}

