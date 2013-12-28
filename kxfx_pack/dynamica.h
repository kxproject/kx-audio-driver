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

// Dynamica
// (c) eYagos, 2003-2004. All rights reserved

#ifndef _KX_DYNAMICA_EFFECT_H
#define _KX_DYNAMICA_EFFECT_H

#define DYNAMICA_PARAMS_COUNT	16	// number of user-visible parameters

// Plugin definition
class iDynamicaPlugin : public iKXPlugin
{
public:
	virtual int request_microcode();
	virtual const char *get_plugin_description(int id);
	
	virtual iKXPluginGUI *create_cp(kDialog *parent, kFile *mf);
	virtual int get_user_interface();

	virtual int get_param_count(void) {return DYNAMICA_PARAMS_COUNT;}
	virtual int describe_param(int, kx_fxparam_descr*); // this function is used for kX Automation and mixer-provided GUI panels
	virtual int set_defaults();
        virtual int set_param(int ndx, kxparam_t value);
        virtual int get_param(int ndx, kxparam_t *value);
        virtual int set_all_params(kxparam_t* values);

        virtual const kxparam_t *get_plugin_presets();

	kxparam_t _params[DYNAMICA_PARAMS_COUNT+1]; // place to store parameters inside the plugin
                                                // last one stores AGC_ID
};

// Plugin GUI definition
class iDynamicaPluginDlg : public CKXPluginGUI
{
public:
	iDynamicaPlugin *plugin;

    // constructor:
	//iDynamicaPluginDlg(iDynamicaPlugin *plg, kDialog *parent_, kFile *mf_) : CKXPluginGUI(parent_,mf_) {plugin = plg; };
	iDynamicaPluginDlg(iDynamicaPlugin *plg, kDialog *parent_, kFile *mf_);
	virtual iKXPlugin *get_plugin() { return plugin; };

		// desctructor
		~iDynamicaPluginDlg();

	virtual void init();
	virtual void on_destroy();

	UINT_PTR	timer_id;
	float	max_l,max_r;
    int		pm_offset,pm_width;
    float	pm_min,pm_max;
	
	afx_msg	void OnTimer(UINT_PTR);
	int			turn_on(int what);
	

	// declare GUI Elements
	// Controls
	//Labels
	kStatic lblTh1;
	kStatic lblTh2;
	kStatic lblAb1;
	kStatic lblBe1;
	kStatic lblBe2;
	kStatic lblRa1;
	kStatic lblRa2;
	kStatic lblRa3;
	kStatic lblAtt;
	kStatic lblRel;
	kStatic lblAttV;
	kStatic lblRelV;
	kStatic lblAttL;
	kStatic lblRelL;
	kStatic lblAttLV;
	kStatic lblRelLV;
	kStatic lblLA;
	kStatic lblOG;
	kStatic lblKT;
	kStatic lblAGC;
	//Sliders
	kSlider fdrTh1;
	kSlider fdrTh2;
	kSlider fdrRa1;
	kSlider fdrRa2;
	kSlider fdrRa3;
	kSlider fdrAtt;
	kSlider fdrRel;
	kSlider fdrAttL;
	kSlider fdrRelL;
	kSlider fdrLA;
	kSlider fdrOG;
	//Combos
	kCombo cmbMo1;
	kCombo cmbMo2;
	kCombo cmbMo3;
	kCombo cmbKT;
	kCombo cmbAGC;
	//CheckButtos
	kCheckButton chkSC;
	kCheckButton chkAGC;
	//Monitor
	kPeak monitor;

	// for updating UI from plugin
	virtual void sync(int what=EVERYPM_ID);

	DECLARE_MESSAGE_MAP()
};

declare_effect_source(dynamica);

#endif

//--------------------------------------------------
