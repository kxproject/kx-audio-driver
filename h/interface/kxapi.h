// kX SDK:
// kX API, kX Audio Driver Interface, kX Plugin Manager API, kX Manager API, kX Add-on API
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

// kxapi.h
// -------
// the main and only file to be included in the project

#ifndef _KX_SDK_H
#define _KX_SDK_H

#include "i386.h"
// #include "hw/8010.h"
#include "hw/ac97.h"
#include "interface/ikx.h"
#include "interface/dsp.h"
#include "interface/soundfont.h"

// Interface classes
#ifdef WIN32
	// kxapi.dll-based classes
	#ifdef KX_INTERFACE
	 #define KX_CLASS_TYPE __declspec(dllexport)
	#else
	 #define KX_CLASS_TYPE __declspec(dllimport)
	#endif

	// kX Manager-based classes
	#ifdef KXMANAGER_INTERFACE
	 #define KXMANAGER_CLASS_TYPE __declspec(dllexport)
	#else
	 #define KXMANAGER_CLASS_TYPE __declspec(dllimport)
	#endif
#elif defined(__APPLE__)
	#define KX_CLASS_TYPE
	#define KXMANAGER_CLASS_TYPE

	#ifndef MAX_PATH
		#define MAX_PATH	1024
	#endif
#else
    #error unknown architecture
#endif

#ifndef KX_DEF_CALLBACK
  #if defined(WIN32)
    #define KX_DEF_CALLBACK __cdecl
  #else
    #define KX_DEF_CALLBACK
  #endif
#endif

#ifndef KX_DEF_EXPORT
  #if defined(WIN32)
    #define KX_DEF_EXPORT  __declspec(dllexport)
  #else
    #define KX_DEF_EXPORT
  #endif
#endif

// Forward declarations...
#ifndef _AFX
 class CString;
 class CPoint;
#endif

class KX_CLASS_TYPE iKXPlugin;
class KX_CLASS_TYPE iKXDSPWindow;
class KX_CLASS_TYPE iKXPluginGUI;

class KXMANAGER_CLASS_TYPE iKXAddOn;
class KXMANAGER_CLASS_TYPE iKXPluginManager;
class KXMANAGER_CLASS_TYPE iKXMidiParser;
class KXMANAGER_CLASS_TYPE iKXManager;

// kGUI forward declaration
class kFile;
class kDialog;
class kWindow;
class kSlider;
class kDraw;
class kPoint;
class kRect;
class kSize;
class kMenu;

// MFC compatibility
#ifndef kString
	#if defined(WIN32)
		#define kString CString
	#elif defined(__APPLE__)
		#include "interface/kstring.h"
	#endif
#endif

#include "interface/kxi.h"
#include "interface/kxcfg.h"

// Plugin Architecture

#include "interface/kxplugin.h"
#include "interface/kxpm.h"
#include "interface/kxm.h"
#include "interface/kxaddon.h"

// #include "interface/kxplugingui.h" // is included by stdafx.h in kxfxlib project

#endif
