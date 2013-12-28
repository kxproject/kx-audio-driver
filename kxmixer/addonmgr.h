// kX Mixer / kX Audio Driver
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

#if !defined(AFX_ADDON_H__B294EF63_CE1F_4020_B1F8_573626C3E572__INCLUDED_)
#define AFX_ADDON_H__B294EF63_CE1F_4020_B1F8_573626C3E572__INCLUDED_

#pragma once

typedef struct kx_addon_list_t
{
 iKXAddOn *addon;
 char guid[KX_MAX_STRING];
 int device_n;

 struct kx_addon_list_t *next;
}kx_addon_list;

class iKXAddOnManager
{
 public:
  iKXAddOnManager();
  ~iKXAddOnManager();

  void init(int stage); // stages: KXADDON_PER_DEVICE KXADDON_SYSTEM_WIDE
  void close();

  // enumerators:
  int menu(int menu_stage,kMenu *mnu); // returns 0 if something was added/modified
  int process_menu(int cmd);
  int event(int event,void *p1,void *p2);

  int notify(iKXAddOn *addon,int event,void *p1,void *p2);

  int register_addon(char *path,int gui=1);
  void unregister_addon(char *guid);
  void unregister_addons(char *path);

  int save_all_addon_settings(int where,kSettings &cfg);
  int load_all_addon_settings(int where,kSettings &cfg);
  int reset_all_addon_settings(int where);
  // where: KXADDON_PER_DEVICE KXADDON_SYSTEM_WIDE

  kx_addon_list *list;

  int launch(char *guid);
  int is_supported(char *guid);
};

#endif
