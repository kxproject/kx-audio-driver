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

// Dynamica
// (c) eYagos, 2004-2005. All rights reserved

#ifndef _KX_DYNAMICA_EFFECT_H
#define _KX_DYNAMICA_EFFECT_H

#include "ColorCtrl.h"
#include "FontCtrl.h" 
#include "eKnob.h"
#include "eGraph.h" 

#define DYNAMICA_PARAMS_COUNT	17	// number of user-visible parameters

// plugin parameters
// (these are 'logical' parameters visible to the user)

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

	// Several functions
	double Log_Dane_Interp(double value, int res);
	double Exp_Dane_Interp(double value);
	double time_to_val(kxparam_t value);
	double ratio_to_val (kxparam_t value);
	double dB_to_val (kxparam_t value);
	double time_to_exp(double value);
	int general_prameters();

	kxparam_t _params[DYNAMICA_PARAMS_COUNT]; // place to store parameters inside the plugin

};

// Plugin GUI definition
class iDynamicaPluginDlg : public CKXPluginGUI
{
public:
	iDynamicaPlugin *plugin;

    // constructor:
	iDynamicaPluginDlg(iDynamicaPlugin *plg, kDialog *parent_, kFile *mf_);
	virtual iKXPlugin *get_plugin() { return plugin; };
		
		// desctructor
		~iDynamicaPluginDlg();
	
	virtual void init();
	virtual void on_destroy();
	
	// NUMBERS 
	UINT_PTR timer_id;
	float Clipp_in, Clipp_out; // Clipp values
	int	steps_gain, max_steps_gain; // Dynamic gain counter
	int VU_px_0dB, LEV_px_0dB, GAIN_px_0dB; //VU numbers 
	float VU_sca_pxdB, LEV_sca_pxdB, GAIN_max_dB; //VU numbers 
	int pos_in_L, pos_in_R, pos_out_L, pos_out_R, pos_level, pos_gain; //VU positions



	// MESSAGES
	afx_msg void OnTimer(UINT_PTR);
	afx_msg void OnClick_IN(); 
	afx_msg void OnClick_OUT(); 
	afx_msg LRESULT OnUpdateStatic(WPARAM wParam, LPARAM lParam);
	int turn_on(int what);	
	int	on_command(int,int); 

	// Several functions
	int destroyClipp(int ndx);
	void SetGraphPoints();

	// declare GUI Elements
	// Files
	// kFile efx_skin;
	// kFile *kxmixer_skin;
	
	// knobs
	eKnob k_OutputGain, k_InputGain,
		k_LevelRelease, k_LevelAttack,
		k_GainAttack, k_GainRelease,
		k_Threshold2, k_Threshold1,
		k_Ratio3, k_Ratio2, k_Ratio1, 
		k_LookAhead;
		
	//Graph
	eGraph graph;

	// Fonts
	CFont vufont, vufont_gain, vufont_clipp, headerfont; 	
	
	// Colors
	kColor color_txt_values, color_bkg_values,
		color_txt_dynamicgain, color_bkg_dynamicgain,
		color_txt_clipp, color_txt1_clipp, color_txt2_clipp;
	
	// Labels
	CColorCtrl<CFontCtrl<CStatic> > lbl_Th1, lbl_Th2,
		lbl_Ratio1, lbl_Ratio2, lbl_Ratio3, 
		lbl_Attack, lbl_Release,
		lbl_Attack_L, lbl_Release_L,
		lbl_OutputGain, lbl_InputGain, 
		lbl_Lookahead,
		lbl_DynamicGain,
		lbl_Clipp_In_Val, lbl_Clipp_Out_Val,
		lbl_Header;
	
	// CheckButtons
	kCheckButton c_KTHard, c_KTSoft, c_KTSofter,
		c_Mode1, c_Mode2, c_Mode3,
		c_SideChain_N, c_SideChain_S;

	// Buttons
	kButton c_Reset_IG, c_Reset_OG;
	
	// Monitors
	kPeak m_in, m_out, m_gain_lev;  
	
	// Statics
	kStatic sta_hard, sta_soft, sta_softer, sta_normal, sta_side;

		
	// for updating UI from plugin
	virtual void sync(int what=EVERYPM_ID);

	DECLARE_MESSAGE_MAP()
};

declare_effect_source(dynamica);

#endif

