// kX Mixer
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


#include "stdinc.h"

HWND systray=0;
extern LRESULT CALLBACK WndProc( HWND  hwnd,	// handle of window
    UINT  uMsg,	// message identifier
    WPARAM  wParam,	// first message parameter
    LPARAM  lParam 	// second message parameter
   );


void tray_icon(int cmd)
{
 int mute=0;
 HICON ic;

 // get current status
 if(manager && manager->get_ikx())
 {
  manager->get_ikx()->mixer(KX_PROP_GET,KX_MIXER_MASTER_MUTE,0,&mute);
 }

 // debug("kxmixer: update icon: %d\n",mute);

 // register with the Explorer:
 NOTIFYICONDATA nid;
 ic=(HICON)LoadImage(GetModuleHandle(NULL),MAKEINTRESOURCE(mute?IDR_ICON_MUTED:IDR_ICON),IMAGE_ICON,16,16,0);
 nid.cbSize=sizeof(nid);
 nid.hWnd=systray;
 nid.uID=1;
 nid.uFlags=NIF_ICON|NIF_MESSAGE|NIF_TIP;
 nid.hIcon=ic;
 strcpy(nid.szTip,mf.get_profile("menu","systray"));
 nid.uCallbackMessage=(WM_KXMIXER_DISPLAY_MENU);

 Shell_NotifyIcon(cmd,&nid);

 if(ic)
  DestroyIcon(ic);
}

int create_systray_window()
{
	  WNDCLASS wc;

          HICON def_icon=AfxGetApp()->LoadIcon(IDR_ICON);
          gui_set_default_icon(def_icon);

          wc.style = 0;
          wc.lpfnWndProc = (WNDPROC) WndProc; 
          wc.cbClsExtra = 0; 
          wc.cbWndExtra = 0; 
          wc.hInstance = (HINSTANCE)GetModuleHandle(NULL); 
          wc.hIcon = def_icon;
          wc.hCursor = ::LoadCursor(NULL,IDC_ARROW);
          wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1); 
          wc.lpszMenuName =  NULL; 
          wc.lpszClassName = KX_DEF_WINDOW_CLASS;

          if(!RegisterClass(&wc)) 
          {
          	debug("kxmixer: Error registering windows class!\n");
          	return -1;
          }

          systray = CreateWindow(
                KX_DEF_WINDOW_CLASS,
                KX_DEF_WINDOW,  // linked with 'kxsetup.exe'  & Enum...
                WS_OVERLAPPEDWINDOW,
                0,0, 10, 10,
          		  (HWND) NULL,
          		  (HMENU) NULL,
          		  (HINSTANCE)GetModuleHandle(NULL),
          		  (LPVOID) NULL);

          if(systray==NULL)
          {
          	debug("kxmixer: Error creating main kX mixer window\n");
          	return -2;
          }

          return 0;
}
