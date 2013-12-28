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

#if !defined(_kDefs_H)
#define _kDefs_H

#pragma once

// methods:
#define kMETHOD_COPY	0x1
#define kMETHOD_BLEND	0x10	// alpha
#define kMETHOD_TRANS	0x20	// with keycolor
#define kMETHOD_TILED	0x40	// 'tiled' background for kDialog
#define kMETHOD_DEFAULT 0x1000  // windows-default (not skinned)
#define kMETHOD_OWNER   0x2000  // for statics (owner-draw)

// states 
#define kSTATE_CREATED		0x1
#define kSTATE_ENABLED  	0x2
#define kSTATE_OVER		0x4
#define kSTATE_SELECTED		0x8
#define kSTATE_DEFAULT		0x10
#define kSTATE_MOVING		0x20
#define kSTATE_FOCUSED		0x40
#define kSTATE_DRAGGING		0x80
#define kSTATE_CHECKED		0x100

// forward class declaration

class kCLASS_TYPE kRect;
class kCLASS_TYPE kPoint;
class kCLASS_TYPE kColor;
class kCLASS_TYPE kDraw;
class kCLASS_TYPE kWindow;
class kCLASS_TYPE kKnob;
class kCLASS_TYPE kLogo;
class kCLASS_TYPE kStatic;
class kCLASS_TYPE kCheckBox;
class kCLASS_TYPE kButton;
class kCLASS_TYPE kDialog;
class kCLASS_TYPE kSlider;

class kCLASS_TYPE kResource;
class kCLASS_TYPE kSettings;

#ifdef CDC
 #undef CDC
#endif

#ifndef _T
 #if defined(UNICODE) || defined(_UNICODE)
  #define _T(a) L ## a
 #else
  #define _T(a) a
 #endif
#endif

#include <malloc.h>

#ifndef ATLA2WHELPER

   // MFC70 compatibility

   /////////////////////////////////////////////////////////////////////////////
   // Global UNICODE<>ANSI translation helpers
   inline LPWSTR WINAPI AtlA2WHelper(LPWSTR lpw, LPCSTR lpa, int nChars, UINT acp)
   {
   	ASSERT(lpa != NULL);
   	ASSERT(lpw != NULL);
   	// verify that no illegal character present
   	// since lpw was allocated based on the size of lpa
   	// don't worry about the number of chars
   	lpw[0] = '\0';
   	MultiByteToWideChar(acp, 0, lpa, -1, lpw, nChars);
   	return lpw;
   }

   inline LPSTR WINAPI AtlW2AHelper(LPSTR lpa, LPCWSTR lpw, int nChars, UINT acp)
   {
   	ASSERT(lpw != NULL);
   	ASSERT(lpa != NULL);
   	// verify that no illegal character present
   	// since lpa was allocated based on the size of lpw
   	// don't worry about the number of chars
   	lpa[0] = '\0';
   	WideCharToMultiByte(acp, 0, lpw, -1, lpa, nChars, NULL, NULL);
   	return lpa;
   }

   inline LPWSTR WINAPI AtlA2WHelper(LPWSTR lpw, LPCSTR lpa, int nChars)
   {
   	return AtlA2WHelper(lpw, lpa, nChars, CP_ACP);
   }

   inline LPSTR WINAPI AtlW2AHelper(LPSTR lpa, LPCWSTR lpw, int nChars)
   {
   	return AtlW2AHelper(lpa, lpw, nChars, CP_ACP);
   }
   #ifdef UNICODE
   #define A2W(lpa) (\
   		((lpa) == NULL) ? NULL : (\
   			AtlA2WHelper((LPWSTR) alloca((lstrlenA(lpa)+1)*2), lpa, (lstrlenA(lpa)+1))))
   #define W2A(lpw) (\
   		((lpw) == NULL) ? NULL : (\
   			AtlW2AHelper((LPSTR) alloca((lstrlenW(lpw)+1)*2), lpw, (lstrlenW(lpw)+1)*2)))
   #else
   #define A2W(a) (a)
   #define W2A(a) (a)
   #endif

#endif
// define Platform SDK values
#if defined(WIN32) || defined(_WIN32)
    #ifndef WS_EX_LAYERED
     #define WS_EX_LAYERED 0x00080000
    #endif
    #ifndef LWA_ALPHA
     #define LWA_ALPHA 0x00000002
    #endif
    #ifndef TTS_BALLOON
     #define TTS_BALLOON 0x40
    #endif
#endif

#if defined(_MSC_VER)
 // #pragma comment(lib, "kxgui.lib")
#endif

#endif
