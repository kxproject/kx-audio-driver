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

#ifndef _KX_ADC_EFFECT_H
#define _KX_ADC_EFFECT_H

#include "ColorCtrl.h"
#include "FontCtrl.h"
#include "mySlider.cpp"
#include "myCheckBox.cpp"
#include "myMenu.cpp"
#include "mySkin.cpp"


#define ADC_PARAMS_COUNT	16	// number of user-visible parameters

declare_effect_source(adc_10k2);
void dbg(const char *format, ...);

// Plugin definition
//-------------------------------------------------------------------
class iAdcPlugin : public iKXPlugin
{
public:
	virtual int init();	// plugin initialization
	virtual int request_microcode();
	virtual const char *get_plugin_description(int id);
	virtual iKXPluginGUI *create_cp(kDialog *parent, kFile *mf);
	virtual int get_user_interface();
	virtual int get_param_count(void) {return ADC_PARAMS_COUNT;}
	virtual int describe_param(int, kx_fxparam_descr*);
	virtual int set_defaults();
	virtual int set_param(int ndx, int value);
	virtual int get_param(int ndx, int *value);
	virtual int set_all_params(int* values);
	virtual const kxparam_t *get_plugin_presets();

	int _params[ADC_PARAMS_COUNT];
	dword is_10k2, has_ac97, is_A2, is_A2ex, is_10k1;
};

// Plugin GUI definition
//---------------------------------------------------------------------------
class iAdcPluginDlg : public CKXPluginGUI
{
public:
	iAdcPlugin *plugin;

    // constructor:
	iAdcPluginDlg(iAdcPlugin *plg, kDialog *parent_, kFile *mf_);   

	// desctructor
		~iAdcPluginDlg();

	virtual void init();
	virtual iKXPlugin *get_plugin() { return plugin; };
	int	on_command(int,int);	// for skinned butons

	myMenu	sell;// left ac97 rec source
	myMenu	selr;// right ac97 rec source

	mySlider gainrecl;	
	mySlider gainrecr;
	mySlider gainline;
	mySlider gainmic;
	mySlider gaincd;
	mySlider gainaux;

	myCheckBox	on_rec;
	myCheckBox	on_line;
	myCheckBox	on_mic;
	myCheckBox	on_cd;
	myCheckBox	on_aux;

	myCheckBox on_gang;	
	myCheckBox micboost;
	myCheckBox setuda;

	CColorCtrl<CFontCtrl<CStatic> > label1;
	CColorCtrl<CFontCtrl<CStatic> > label2;
	CFont labelfont;
	CFont sliderfont;

	mySkin	skin;								// skin class
	HBITMAP hsliderback_140,hsliderthumb,		// bitmap handles
			henaon, henaoff,hgangon,hgangoff,
			hbooston,hboostoff,hudaon,hudaoff,
			harrow,harrow_o,htxtbox,
			hdsp,hdsp_o,hkfx,hkfx_o;

	myCheckBox launch_dsp;			// open dsp button
	myMenu2	fxmenu;					// launch kfx menu
	iKXManager*	ikm;				// for 'launch DSP' etc.


	// for updating UI from plugin
	virtual void sync(int what=EVERYPM_ID);
};

#endif
