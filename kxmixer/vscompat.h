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


// for compatibility between VS 6.0 and XP DDK

#ifndef VS_COMPAT_H_
#define VS_COMPAT_H_

#if !defined(_W64)
 #if !defined(__midl) && (defined(_X86_) || defined(_M_IX86)) && _MSC_VER >= 1300
  #define _W64 __w64
 #else
  #define _W64
 #endif
#endif

#ifndef _UINTPTR_T_DEFINED
 #ifdef  _WIN64
  typedef unsigned __int64    uintptr_t;
 #else
  typedef _W64 unsigned int   uintptr_t;
 #endif
 #define _UINTPTR_T_DEFINED
#endif

#ifndef VK_VOLUME_MUTE
  #define VK_VOLUME_MUTE         0xAD
  #define VK_VOLUME_DOWN         0xAE
  #define VK_VOLUME_UP           0xAF
  #define VK_MEDIA_NEXT_TRACK    0xB0
  #define VK_MEDIA_PREV_TRACK    0xB1
  #define VK_MEDIA_STOP          0xB2
  #define VK_MEDIA_PLAY_PAUSE    0xB3
  #define VK_LAUNCH_MAIL         0xB4
  #define VK_LAUNCH_MEDIA_SELECT 0xB5
  #define VK_LAUNCH_APP1         0xB6
  #define VK_LAUNCH_APP2         0xB7

  #define VK_BROWSER_BACK        0xA6
  #define VK_BROWSER_FORWARD     0xA7
  #define VK_BROWSER_REFRESH     0xA8
  #define VK_BROWSER_STOP        0xA9
  #define VK_BROWSER_SEARCH      0xAA
  #define VK_BROWSER_FAVORITES   0xAB
  #define VK_BROWSER_HOME        0xAC
#endif


#if (_WIN32_WINNT <= 0x0400)

typedef struct tagMOUSEINPUT {
    LONG    dx;
    LONG    dy;
    DWORD   mouseData;
    DWORD   dwFlags;
    DWORD   time;
    ULONG_PTR dwExtraInfo;
} MOUSEINPUT, *PMOUSEINPUT, FAR* LPMOUSEINPUT;

typedef struct tagKEYBDINPUT {
    WORD    wVk;
    WORD    wScan;
    DWORD   dwFlags;
    DWORD   time;
    ULONG_PTR dwExtraInfo;
} KEYBDINPUT, *PKEYBDINPUT, FAR* LPKEYBDINPUT;

typedef struct tagHARDWAREINPUT {
    DWORD   uMsg;
    WORD    wParamL;
    WORD    wParamH;
} HARDWAREINPUT, *PHARDWAREINPUT, FAR* LPHARDWAREINPUT;

#define INPUT_MOUSE     0
#define INPUT_KEYBOARD  1
#define INPUT_HARDWARE  2

typedef struct tagINPUT {
    DWORD   type;

    union
    {
        MOUSEINPUT      mi;
        KEYBDINPUT      ki;
        HARDWAREINPUT   hi;
    };
} INPUT, *PINPUT, FAR* LPINPUT;

extern "C"

WINUSERAPI
UINT
WINAPI
SendInput(
    IN UINT    cInputs,     // number of input in the array
    IN LPINPUT pInputs,     // array of inputs
    IN int     cbSize);     // sizeof(INPUT)

#endif

#endif
