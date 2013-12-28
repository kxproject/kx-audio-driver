// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov and Max Mikhailov, 2001-2004.
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


#include "OScopeCtrl.h"
//.#include "HistogramCtrl.h"

#ifndef _KX_OSC_EFFECT_H
#define _KX_OSC_EFFECT_H

#define OSC_PARAMS_COUNT	3	// number of user-visible parameters

// Plugin definition
class iOscPlugin : public iKXPlugin
{
public:
	virtual int request_microcode();
	virtual const char *get_plugin_description(int id);
	
	virtual iKXPluginGUI *create_cp(kDialog *parent, kFile *mf);
	virtual int get_user_interface();

	virtual int get_param_count(void) {return OSC_PARAMS_COUNT;}
	virtual int describe_param(int, kx_fxparam_descr*); // this function is used for kX Automation and mixer-provided GUI panels
	virtual int set_defaults();
        
        virtual int get_param(int ndx, kxparam_t *value);
		virtual int set_all_params(kxparam_t* values);
		int set_param(int ndx, kxparam_t value);
        

        virtual const kxparam_t *get_plugin_presets();

	kxparam_t _params[OSC_PARAMS_COUNT]; // place to store parameters inside the plugin
};

// Plugin GUI definition
class iOscPluginDlg : public CKXPluginGUI
{
public:
	iOscPlugin *plugin;

    	// constructor:
	//iOscPluginDlg(iOscPlugin *plg, kDialog *parent_, kFile *mf_) : CKXPluginGUI(parent_,mf_) {plugin = plg; };

	iOscPluginDlg(iOscPlugin *plg, kDialog *parent_,kFile *mf_);
	~iOscPluginDlg();
	
	virtual void init();
	virtual iKXPlugin *get_plugin() { return plugin; };

	// declare GUI Elements

	virtual void on_destroy();
	
	
	UINT_PTR timer_id;
	
   dword s1, s0,tmp;

	afx_msg void OnTimer(UINT_PTR);
	int turn_on(int what, int res);
	

	
	// Controls
	
	kCheckButton res, turn;
	
	kFader fader;
	

	
	
	COScopeCtrl scope;
	

	

	// for updating UI from plugin
	virtual void sync(int what=EVERYPM_ID);

	DECLARE_MESSAGE_MAP()
};



declare_effect_source(osc);

#endif
