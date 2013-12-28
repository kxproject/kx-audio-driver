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


// Commons
#include "stdafx.h"

// #include "interface/guids.h"
#include "vers.h"

// kX API and stuff
#include "interface/kxapi.h"

// menu-related

#define MENU_TRAYCFG_BASE   2000
#define MENU_TRAYCFG_BASE2  1900
#define MENU_ADDON_BASE     1100
#define MENU_DEVID_BASE     1000
#define MENU_PLUGIN_BASE    500
#define MENU_SKINS_BASE     100
#define MENU_MRU_BASE       200

KXMANAGER_CLASS_TYPE void translate_keyboard(UINT ch,UINT rep,UINT fl);

int change_device(int new_dev,CWnd *parent=NULL); // new_dev==-1 - GUI select; =-2 - from registry
KXMANAGER_CLASS_TYPE int launch_menu();
KXMANAGER_CLASS_TYPE int process_menu(int ret);

class kFile;
class kFont;
class iKX;

// Utility functions:

// save/restore folder name
void save_cwd(const char *id);
void restore_cwd(const char *id);
void save_lru(char *f);

int destroy_decoder(int dev);
int create_decoder(int dev);

void tray_icon(int cmd);

extern kFile mf;

void quit_kxmixer(int force_restart);

void set_font(kWindow *that,kFont &font,const char *section);

extern iKXManager *manager;

int update_asio_drivers(void);
int get_mixer_folder(TCHAR *folder);

int get_mixer_page_by_guid(const char *guid);
char *get_mixer_page_guid(int n);

#define IDg_ONTOP       (0x10)

// main button messages
#define IDg_MINI        (WM_USER+0x1)
#define IDg_CARDNAME        (WM_USER+0x2)
#define IDg_INFO        (WM_USER+0x3)
#define IDg_SAVE_SETTINGS   (WM_USER+0x4)
#define IDg_LOAD_SETTINGS   (WM_USER+0x5)
#define IDg_QUIT        (IDCANCEL)

// GUI
#include "gui/kGui.h"
#include "gui/kxdialog.h"

// settings
#include "settings.h"

#undef CDC
