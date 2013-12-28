// Copyright (c) Martin Borisov, 2004.
// All rights reserved
// martintiger@abv.bg

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


#ifndef _KX_REV_EFFECT_H
#define _KX_REV_EFFECT_H

#include "gui/efx/StaticCounter.h"
#include "gui/efx/CMKnob.h"


#define REV_PARAMS_COUNT	18	// number of user-visible parameters

// Plugin definition
class iRevPlugin : public iKXPlugin
{
public:
	virtual int request_microcode();
	virtual const char *get_plugin_description(int id);
	
	virtual iKXPluginGUI *create_cp(kDialog *parent, kFile *mf);
	virtual int get_user_interface();

	virtual int get_param_count(void) {return REV_PARAMS_COUNT;}
	virtual int describe_param(int, kx_fxparam_descr*); // this function is used for kX Automation and mixer-provided GUI panels
	virtual int set_defaults();
    virtual int set_param(int ndx, kxparam_t value);
    virtual int get_param(int ndx, kxparam_t *value);
    virtual int set_all_params(kxparam_t* values);

    virtual const kxparam_t *get_plugin_presets();
           
	//used for decay time calculation
	virtual double calc_gain(double len_samp, double time_s);

	kxparam_t _params[REV_PARAMS_COUNT]; // place to store parameters inside of plugin
};

// Plugin GUI definition
class iRevPluginDlg : public CKXPluginGUI
{
public:
	iRevPlugin *plugin;
    
	kFile efx_skin;
	kFile *kxmixer_skin;
	
	iRevPluginDlg(iRevPlugin *plg, kDialog *parent_,kFile *mf_);

	virtual iKXPlugin *get_plugin() { return plugin; };
    
	//iRevPluginDlg(iRevPlugin *plg, kDialog *parent_, kFile *mf_) : CKXPluginGUI(parent_,mf_) {plugin = plg; };
        ~iRevPluginDlg();
	virtual void init();
	virtual void on_destroy();
	
	// VU meter
	UINT_PTR timer_id;
	float max_l,max_r;
	
	afx_msg void OnTimer(UINT_PTR);
	int turn_on(int what);
    
        // For the knob class and skinned checkboxes
	afx_msg LRESULT OnUpdateStatic(WPARAM wParam, LPARAM lParam);
	int	on_command(int,int); 
	
	// Controls
        CStaticCounter count_inlev, count_erlev, count_revlev, count_drylev, count_band,
		count_pre, count_decay, count_outlev;
	
	CMKnob knob_inlev, knob_erlev, knob_revlev, knob_drylev, knob_band, knob_pre,
		knob_decay, knob_diff, knob_damp, knob_outlev, knob_tap1, knob_tap2,
		knob_tap3, knob_tap4, knob_tap5, knob_tap6;
	
	kCheckButton led_pre_x10, led_decay_x10;
	kPeak vumeter;
	
	/*for bitmap loading loop*/////////////////////////
	HBITMAP bitmaps[FRAMES];                         //
	int i;                                           //
	char filename[7];                                //    
	///////////////////////////////////////////////////

	// for updating UI from plugin
	virtual void sync(int what=EVERYPM_ID);

	DECLARE_MESSAGE_MAP()
};

declare_effect_source(rev);

#endif
