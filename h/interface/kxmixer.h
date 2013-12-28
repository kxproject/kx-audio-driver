// kX SDK:
// kX API, kX Audio Driver Interface, kX Plugin Manager API
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

// kxmixer.h
// -----
// kX Mixer System Tray messages
// -----
// do not include this file directly, use 'kxapi.h' instead


#ifndef _KX_MIXER_H
#define _KX_MIXER_H

// Windows messages for the kX Manager System Tray window:

 #define KX_DEF_WINDOW		"KXDefWindow"
 #define KX_DEF_WINDOW_CLASS	"KXDefClass"

 #define WM_KXMIXER_TWEAK_VSTI		WM_USER+0x100
 // wParam: device_id lParam: reserved
 #define WM_KXMIXER_MESSAGE		WM_USER+0x103
 // wParam: device_id lParam: midi message
 #define WM_KXMIXER_SAVE_SETTINGS	WM_USER+0x101
 // wParam: device_id lParam: {high: flags; low: random number}
 #define WM_KXMIXER_REST_SETTINGS	WM_USER+0x102
 // wParam: device_id lParam: {high: flags; low: random number}
 #define WM_KXMIXER_TWEAK_ROUTER	WM_USER+0x104
 // wParam: device_id lParam: reserved

 #define WM_KXMIXER_DISPLAY_MENU        (WM_USER)
  // just displays the systray menu
  // wParam/lParam: accoding to Explorer SysTray notifications
 #define WM_KXMIXER_PROCESS_MENU	(WM_USER+1)
  // lParam: perform particular command -- interface is undocumented and is subject to change
  // the only thing that is guaranteed to work:
  //  lParam: 2 -- open kX Mixer window
  //  lParam: 9 -- open kX Automation [wParam: device ID (0..max_dev-1)]
  // (used mainly for kX VSTi)
 #define WM_KXMIXER_PROCESS_CMDLINE	(WM_USER+2)
  // full command line should be in clipboard as OEM Text
  // (this is subject to change)

 #define WM_KXMIXER_TWEAK		(WM_USER+3)
 // internally-used routine for tweaking plugins
 // wParam: pointer to iKXPlugin
 // lParam: pointer to iKXPluginManager

#endif
