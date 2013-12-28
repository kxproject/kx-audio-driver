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

#ifndef _KX_GUI_H
#define _KX_GUI_H

#ifdef kGUI_INTERNAL
 #define kCLASS_TYPE __declspec(dllexport)
#else
 #define kCLASS_TYPE __declspec(dllimport)
#endif

#if !defined(KX_MFC7) && defined(_MFC_VER) && (_MFC_VER >= 0x0700)
 #define KX_MFC7
#endif

#include "gui/kDefs.h"
#include "gui/kBase.h"

#include "gui/kWindow.h"
#include "gui/kStatic.h"
#include "gui/kButton.h"
#include "gui/kCheckButton.h"
#include "gui/kTextButton.h"
#include "gui/kKnob.h"
#include "gui/kIndicator.h"
#include "gui/kSlider.h"
#include "gui/kDialog.h"
#include "gui/kLogo.h"
#include "gui/kPeak.h"
#include "gui/kList.h"
#include "gui/kCombo.h"
#include "gui/kProgress.h"
#include "gui/kMenu.h"

#include "gui/kResource.h"
#include "gui/kSettings.h"

// non-class functions:

int kCLASS_TYPE gui_get_version();		// implies 'init_environment()'
int kCLASS_TYPE gui_init_environtment();
int kCLASS_TYPE gui_has_sfx();			// implies 'init_environment()'

int kCLASS_TYPE gui_create_button(kDialog *parent,kButton *b,UINT id,TCHAR *param,kFile &mf);

void kCLASS_TYPE gui_set_default_icon(HICON ic);

HICON kCLASS_TYPE gui_get_default_icon();

void kCLASS_TYPE gui_transparent_blit(HDC hdc, HBITMAP hBitmap, short xStart,
                           short yStart, COLORREF cTransparentColor);
HRGN kCLASS_TYPE gui_bitmap_region(HBITMAP hBitmap,COLORREF cTransparentColor=RGB(0,0,0),BOOL bIsTransparent=TRUE);

extern BOOL (WINAPI *_set_layered_window_attributes)(HWND hwnd,COLORREF crKey,BYTE bAlpha,DWORD dwFlags);


int kCLASS_TYPE gui_create_point_font(kFont &f,kFile &mf,const TCHAR *section,const TCHAR *key,const TCHAR *default_params,HDC dc);

int kCLASS_TYPE gui_set_background(kDialog *d,kFile *mf,const TCHAR *param,unsigned int &transp_color,unsigned int &alpha,unsigned int &cc_method,const TCHAR *def_bk);

int kCLASS_TYPE gui_set_tooltip_timings(int initial,int popup,int reshow);

int kCLASS_TYPE gui_set_show_content(int set);
int kCLASS_TYPE gui_get_show_content(void);

int kCLASS_TYPE gui_register_window_class(TCHAR *lpszClassName);

int kCLASS_TYPE gui_set_tooltip_type(int type); // 1-balloon 0-standard

LPCTSTR kCLASS_TYPE kRegisterWndClass(UINT nClassStyle,
	HCURSOR hCursor, HBRUSH hbrBackground, HICON hIcon,const TCHAR *class_name);

#endif
