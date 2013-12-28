// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov and Max Mikhailov, 2001-2005.
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

#ifndef _KX_KXM120_EFFECT_H
#define _KX_KXM120_EFFECT_H

#pragma comment (lib, "Winmm")
#include "Mmsystem.h"
         
#define KXM120_PARAMS_COUNT	4

// Plugin definition
class iKXM120Plugin : public iKXPlugin
{     
public:
	virtual int request_microcode();
	virtual const char *get_plugin_description(int id);
	virtual iKXPluginGUI *create_cp(kDialog *parent, kFile *mf);	
	
	virtual int get_user_interface();
	
	virtual int get_param_count(void) {return KXM120_PARAMS_COUNT;}
	virtual int set_defaults();
	virtual int set_param(int ndx, kxparam_t value);
	virtual int get_param(int ndx, kxparam_t *value);
	virtual int set_all_params(kxparam_t* values);
	virtual int event(int event);
	
	kxparam_t _params[KXM120_PARAMS_COUNT];
        int nChannel;

	BOOL m_bMidiInOpen;
	HMIDIIN m_hMidiIn;
	virtual void OnMidiIn(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwParam1, DWORD_PTR dwParam2);


};

// Plugin GUI definition
class iKXM120PluginDlg : public CKXPluginGUI
{
public:
	iKXM120Plugin *plugin;

    	// constructor:
	iKXM120PluginDlg(iKXM120Plugin *plg, kDialog *parent_, kFile *mf_) : CKXPluginGUI(parent_,mf_) {plugin = plg; };

	virtual void init();
	virtual iKXPlugin *get_plugin() { return plugin; };

	// declare GUI Elements

	// Controls
	kCombo midiin_combo;
	kCombo midichan_combo;
	kStatic midiin_label;
        kStatic midichan_label;

	// for updating UI from plugin
	virtual void sync(int what=EVERYPM_ID);
};
declare_effect_source(kxm120);

#endif
