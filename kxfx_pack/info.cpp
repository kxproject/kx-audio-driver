// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov, 2002-2004.
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

// (c) Eugene Gavrilov, 2004

#include "stdafx.h"

// --------------- the above should be a 'standard' beginning of any .cpp file

// effect class
#include "info.h"
// effect source
#include "da_info.cpp"

/* Plugin Implementation */

int iInfoPlugin::save_plugin_settings(kSettings &cfg)
{
 if(pgm_id>0)
 {
  char section[128];
  sprintf(section,"pgm_%d",pgm_id);

  cfg.write(section,"info_text",str_param);
 }
 return 0;
}

int iInfoPlugin::load_plugin_settings(kSettings &cfg)
{
 if(pgm_id>0)
 {
  char section[128];
  sprintf(section,"pgm_%d",pgm_id);

  cfg.read(section,"info_text",str_param,sizeof(str_param));
 }
 else
  str_param[0]=0;

 return 0;
}

int iInfoPlugin::request_microcode() 
{
	publish_microcode(info);
	info_size=0;
	return 0;
}

#define INFO_PLG_EDIT	0x10001

BEGIN_MESSAGE_MAP(iInfoPluginDlg, CKXPluginGUI)
 ON_CONTROL(EN_UPDATE,INFO_PLG_EDIT,editor_change)
END_MESSAGE_MAP()

iKXPluginGUI *iInfoPlugin::create_cp(kDialog *parent, kFile *mf)
{
	iInfoPluginDlg *tmp;
	tmp = new iInfoPluginDlg(this, parent, mf);
	tmp->create();
	return tmp;
}

int iInfoPlugin::get_user_interface()
{
	return IKXPLUGIN_OWNER;	// says to mixer that this plugin has its own control panel
}


/* Plugin UI Implementation */

void iInfoPluginDlg::init()
{
	CKXPluginGUI::init();

	// custom dialog initialization here...
	create_dialog(plugin_name(info), 320, 100);

	b_mute.hide();
	b_bypass.hide();
	b_reset.hide();
	preset.hide();

	CRect r(10,34,350,115);

        text.Create(ES_MULTILINE | ES_AUTOHSCROLL,r,this,INFO_PLG_EDIT);
        text.SetWindowText(plugin->str_param);
        text.ShowWindow(SW_SHOW);

	controls_enabled = TRUE;
}

void iInfoPluginDlg::editor_change(void)
{
  kString tmp;
  text.GetWindowText(tmp);
  strncpy(plugin->str_param,tmp,sizeof(plugin->str_param));
}
