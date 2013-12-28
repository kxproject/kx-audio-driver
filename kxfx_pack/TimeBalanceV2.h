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

#ifndef _KX_TIMEBALANCE_EFFECT_H
#define _KX_TIMEBALANCE_EFFECT_H

//#include "gui/efx/StaticCounter.h"
//#include "gui/efx/CMKnob.h"

#include "ColorCtrl.h"
#include "FontCtrl.h" 

#define TIMEBALANCE_PARAMS_COUNT	(27+5+1+2)	// number of user-visible parameters + non visible
												// 5 : Time Delays
												// 1 : Mode
												// 2 : Reference x, Reference y

// Plugin definition
class iTimeBalanceV2Plugin : public iKXPlugin
{
public:
	virtual int request_microcode();
	virtual const char *get_plugin_description(int id);
	
	virtual iKXPluginGUI *create_cp(kDialog *parent, kFile *mf);
	virtual int get_user_interface();

	virtual int get_param_count(void) {return TIMEBALANCE_PARAMS_COUNT;}
	virtual int describe_param(int, kx_fxparam_descr*); // this function is used for kX Automation and mixer-provided GUI panels
	virtual int set_defaults();
    virtual int set_param(int ndx, kxparam_t value);
    virtual int get_param(int ndx, kxparam_t *value);
    virtual int set_all_params(kxparam_t* values);

    virtual const kxparam_t *get_plugin_presets();

	int general_parameters();

	kxparam_t _params[TIMEBALANCE_PARAMS_COUNT]; // place to store parameters inside the plugin

};

// Plugin GUI definition
class iTimeBalanceV2PluginDlg : public CKXPluginGUI
{
public:
	iTimeBalanceV2Plugin *plugin;

    // constructor:
	iTimeBalanceV2PluginDlg(iTimeBalanceV2Plugin *plg, kDialog *parent_, kFile *mf_) : CKXPluginGUI(parent_,mf_) 
	{
		plugin = plg; 
		kxmixer_skin=mf_; // save kxmixer skin pointer
	};

	virtual void init();
	virtual iKXPlugin *get_plugin() { return plugin; };

	int change_mode();
	int load_skin(int skin_number);
	int message_display();

	// MESSAGES
	afx_msg LRESULT OnUpdateStatic(WPARAM wParam, LPARAM lParam);

	// declare GUI Elements

	// Files
	kFile efx_skin;
	kFile *kxmixer_skin;

	// Fonts
	CFont defaultfont; 

	// Colors
	kColor color_bkg_values,
		color_bkg_values2,
		color_txt_values,
		color_txt_listener, 
		color_txt_speakers;

	// Labels
	CColorCtrl<CFontCtrl<CStatic> > lblDistL_FL, lblDistL_FR,
		lblDistL_RL, lblDistL_RR, lblDistL_C,
		lblDistFL_RL, lblDistFR_RR, lblDistFL_FR,
		lblDistRL_RR, lblDistFL_C, lblDistFR_C,
		lblHeader,
		lblSpkFL,
		lblSpkFR,
		lblSpkRL,
		lblSpkRR,
		lblSpkC,
		lblSpkL;

	// Controls
	
	//Advanced Mode

	kSlider FLx;
	kSlider FLy;
	kSlider FLz;
	
	kSlider FRx;
	kSlider FRy;
	kSlider FRz;
	
	kSlider RLx;
	kSlider RLy;
	kSlider RLz;

	kSlider RRx;
	kSlider RRy;
	kSlider RRz;

	kSlider Cx;
	kSlider Cy;
	kSlider Cz;

	kSlider Lx;
	kSlider Ly;
	kSlider Lz;

	// Basic Mode

	kSlider FLDist;
	kSlider FRDist;
	kSlider RLDist;
	kSlider RRDist;
	kSlider CDist;

	// Both Modes

	kCombo ComboUnits;
	kCombo ComboMode;
	kCombo ComboReference;

	kSlider Temperature;

	kButton btnSave;

	// Labels static

	// Advanced Mode

	kStatic lblFLx;
	kStatic lblFLy;
	kStatic lblFLz;
	
	kStatic lblFRx;
	kStatic lblFRy;
	kStatic lblFRz;

	kStatic lblRLx;
	kStatic lblRLy;
	kStatic lblRLz;

	kStatic lblRRx;
	kStatic lblRRy;
	kStatic lblRRz;

	kStatic lblCx;
	kStatic lblCy;
	kStatic lblCz;

	kStatic lblLx;
	kStatic lblLy;
	kStatic lblLz;

	// Basic Mode

	kStatic lblDist;
	kStatic lblFL;
	kStatic lblFR;
	kStatic lblRL;
	kStatic lblRR;
	kStatic lblC;

	// Both Modes

	kStatic lblTemperature;

	// for updating UI from plugin
	virtual void sync(int what=EVERYPM_ID);

	int on_command(int,int); //added
};

declare_effect_source(TimeBalanceV2);

#endif
