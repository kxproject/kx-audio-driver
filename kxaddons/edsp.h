// kX E-DSP Control Add-on
// Copyright (c) Eugene Gavrilov, 2008-2009.
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

// sample add-on

class EDSPDialog;

class iEDSPControlAddOn : public iKXAddOn
{
public:
 iEDSPControlAddOn() : iKXAddOn() 
 { 
  needed_menu_stage=KXADDON_MENU_TOP;
  event_mask=KXADDON_EVENT_MENU|KXADDON_EVENT_GENERIC|KXADDON_EVENT_REMOTEIR|KXADDON_EVENT_POWER;
  addon_type=KXADDON_PER_DEVICE;
 };

 int menu(int menu_stage,kMenu *mnu);
 int init(int p);
 int close();
 int save_settings(int where,kSettings &cfg);
 int load_settings(int where,kSettings &cfg);
 int reset_settings(int where);
 int event(int event,void *p1,void *p2);
 int activate();

 int init_firmware(int force=0);

 int get_addon_version() { return KXADDON_VERSION; };

 static iKXAddOn *create(void) { return (iKXAddOn *) (new iEDSPControlAddOn); };

 iKX *ikx;
 EDSPDialog *edsp_dialog;
};
