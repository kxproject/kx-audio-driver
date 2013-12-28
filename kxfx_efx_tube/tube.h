// EF-X Library
// Copyright (c) Martin Borisov, 2004.
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

#ifndef _KX_fuzz_EFFECT_H
#define _KX_fuzz_EFFECT_H

#include "gui/efx/StaticCounter.h"
#include "gui/efx/CMKnob.h"




#define FUZZ_PARAMS_COUNT	13	// number of user-visible parameters

// Plugin definition
class ifuzzPlugin : public iKXPlugin
{
public:
	virtual int request_microcode();
	virtual const char *get_plugin_description(int id);
	
	virtual iKXPluginGUI *create_cp(kDialog *parent, kFile *mf);
	virtual int get_user_interface();

	virtual int get_param_count(void) {return FUZZ_PARAMS_COUNT;}
	virtual int describe_param(int, kx_fxparam_descr*); // this function is used for kX Automation and mixer-provided GUI panels
	virtual int set_defaults();
        virtual int set_param(int ndx, kxparam_t value);
        virtual int get_param(int ndx, kxparam_t *value);
        virtual int set_all_params(kxparam_t* values);

        virtual const kxparam_t *get_plugin_presets();

	kxparam_t _params[FUZZ_PARAMS_COUNT]; // place to store parameters inside of plugin
};

// Plugin GUI definition
class ifuzzPluginDlg : public CKXPluginGUI
{
public:
	ifuzzPlugin *plugin;

	kFile efx_skin;
	kFile *kxmixer_skin;

    	// constructor:
	ifuzzPluginDlg(ifuzzPlugin *plg, kDialog *parent_, kFile *mf_);

	virtual void init();
	virtual iKXPlugin *get_plugin() { return plugin; };
    

	afx_msg LRESULT OnUpdateStatic(WPARAM wParam, LPARAM lParam);
	int	on_command(int,int); 
	
	// Controls
    CStaticCounter count_drive, count_level_drive, count_level_clean, count_i_freq,  count_o_freq;
	CMKnob knob_drive, knob_level_drive, knob_level_clean, knob_tone, knob_polish,
		knob_i_width, knob_o_width;
	kCheckButton led_clean, led_norm, led_bps1, led_bps2, sw_st, sw_oc;
	
	/*for bitmap loading loop*/////////////////////////
	HBITMAP bitmaps[FRAMES];                         //
	int i;                                           //
	char filename[7];//for 7 char filenames          //    
	///////////////////////////////////////////////////
	
	
  

	// for updating UI from plugin
	virtual void sync(int what=EVERYPM_ID);

	DECLARE_MESSAGE_MAP()
};

declare_effect_source(fuzz);

#endif
