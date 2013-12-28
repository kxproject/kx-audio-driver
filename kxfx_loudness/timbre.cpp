// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov and Max Mikhailov, 2001-2005.
// Updated by Russ, 2008.
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

#include "stdafx.h"

// --------------- the above should be a 'standard' beginning of any .cpp file

// effect class
#include "timbre.h"
// effect source
#include "da_timbre.cpp"

inline kxparam_t kabs(kxparam_t v)
{
        if(v<0) return -v; else return v;
}

// plugin parameters
typedef enum _params_id{
	BASS_ID,
	TREBLE_ID,
	VOL_ID,
	SETVOL_ID,
};

#define BF	150
#define TF	6800

// default plugin parameters
static kxparam_t default_params[TIMBRE_PARAMS_COUNT] = {0, 0,0,-60};


static kx_fxparam_descr _param_descr[TIMBRE_PARAMS_COUNT] = {
	{"Bass",   KX_FXPARAM_CB, -120, 120},
	{"Treble", KX_FXPARAM_CB, -120, 120},
	{"Vol",  KX_FXPARAM_CB, -120, 0 },
	{"SetVol",  KX_FXPARAM_CB, -120, -1 }};

int iTimbrePlugin::describe_param(int ndx, kx_fxparam_descr *descr)
{
	memcpy(descr, &_param_descr[ndx], sizeof kx_fxparam_descr); return 0;
}

#define P (kxparam_t)

static kxparam_t presets[]=
{
 // don't include 'default' preset

 NULL // must be here
};

#undef P

const kxparam_t *iTimbrePlugin::get_plugin_presets()
{
 return presets;
}


/* Plugin Implemenation */

int iTimbrePlugin::get_param(int ndx, kxparam_t *value) // ndx corresponds to parameter id
{
	*value = _params[ndx]; 
	return 0;
}

int iTimbrePlugin::set_param(int ndx, kxparam_t value)
{
	_params[ndx] = value;

	double N, F1, F2, B0, B1, A1; 

	switch (ndx) /* !!! Not yet optimized !!! */
	{
	case BASS_ID:
		F1 = BF;
		F2 = BF * pow(10., kabs(value*_params[VOL_ID]/_params[SETVOL_ID]) / 200.0);
		if (value*_params[VOL_ID]/_params[SETVOL_ID] < 0) {F1 = F2; F2 = BF;};
		B1 = -exp((-pi * F2) / FN);
		A1 = -exp((-pi * F1) / FN);

		set_dsp_register(R_LB1, _dbl_to_dspword(B1));
		set_dsp_register(R_LA1, _dbl_to_dspword(A1));
		break;

	case TREBLE_ID:
		F1 = TF;
		F2 = TF / pow(10., kabs(value*_params[VOL_ID]/_params[SETVOL_ID]) / 200.0);
		if (value*_params[VOL_ID]/_params[SETVOL_ID] < 0) {F1 = F2; F2 = TF;};
		B0 = -1.;
		B1 = exp((-pi * F2) / FN);
		A1 = -exp((-pi * F1) / FN);
		N = (1. + A1) / ((-1. + B1) * 4.);
		B0 = B0 * N; B1 = B1 * N;

		set_dsp_register(R_HB0, _dbl_to_dspword(B0));
		set_dsp_register(R_HB1, _dbl_to_dspword(B1));
		set_dsp_register(R_HA1, _dbl_to_dspword(A1));
		break;
	case VOL_ID:
		set_dsp_register(PVOL, itodspr_scl(1000000, (int)(1000000*pow(10.0f,(float)_params[VOL_ID]/20))));

		F1 = BF;
		F2 = BF * pow(10., kabs(_params[BASS_ID]*_params[VOL_ID]/_params[SETVOL_ID]) / 200.0);
		if (_params[BASS_ID]*_params[VOL_ID]/_params[SETVOL_ID] < 0) {F1 = F2; F2 = BF;};
		B1 = -exp((-pi * F2) / FN);
		A1 = -exp((-pi * F1) / FN);

		set_dsp_register(R_LB1, _dbl_to_dspword(B1));
		set_dsp_register(R_LA1, _dbl_to_dspword(A1));

				F1 = TF;
		F2 = TF / pow(10., kabs(_params[TREBLE_ID]*_params[VOL_ID]/_params[SETVOL_ID]) / 200.0);
		if (_params[TREBLE_ID]*_params[VOL_ID]/_params[SETVOL_ID] < 0) {F1 = F2; F2 = TF;};
		B0 = -1.;
		B1 = exp((-pi * F2) / FN);
		A1 = -exp((-pi * F1) / FN);
		N = (1. + A1) / ((-1. + B1) * 4.);
		B0 = B0 * N; B1 = B1 * N;

		set_dsp_register(R_HB0, _dbl_to_dspword(B0));
		set_dsp_register(R_HB1, _dbl_to_dspword(B1));
		set_dsp_register(R_HA1, _dbl_to_dspword(A1));
		break;
	case SETVOL_ID:

		F1 = BF;
		F2 = BF * pow(10., kabs(_params[BASS_ID]*_params[VOL_ID]/_params[SETVOL_ID]) / 200.0);
		if (_params[BASS_ID]*_params[VOL_ID]/_params[SETVOL_ID] < 0) {F1 = F2; F2 = BF;};
		B1 = -exp((-pi * F2) / FN);
		A1 = -exp((-pi * F1) / FN);

		set_dsp_register(R_LB1, _dbl_to_dspword(B1));
		set_dsp_register(R_LA1, _dbl_to_dspword(A1));

				F1 = TF;
		F2 = TF / pow(10., kabs(_params[TREBLE_ID]*_params[VOL_ID]/_params[SETVOL_ID]) / 200.0);
		if (_params[TREBLE_ID]*_params[VOL_ID]/_params[SETVOL_ID] < 0) {F1 = F2; F2 = TF;};
		B0 = -1.;
		B1 = exp((-pi * F2) / FN);
		A1 = -exp((-pi * F1) / FN);
		N = (1. + A1) / ((-1. + B1) * 4.);
		B0 = B0 * N; B1 = B1 * N;

		set_dsp_register(R_HB0, _dbl_to_dspword(B0));
		set_dsp_register(R_HB1, _dbl_to_dspword(B1));
		set_dsp_register(R_HA1, _dbl_to_dspword(A1));
		break;

	}

	if (cp) ((iTimbrePluginDlg*) cp)->sync(ndx);

	return 0;
}

int iTimbrePlugin::set_defaults() 
{
	set_all_params(default_params); 
	return 0;
}

int iTimbrePlugin::set_all_params(kxparam_t* values)
{
	for (int i = 0; i < TIMBRE_PARAMS_COUNT; i++) set_param(i, values[i]);
	return 0;
}

int iTimbrePlugin::request_microcode() 
{
	publish_microcode(timbre);

	return 0;
}

const char *iTimbrePlugin::get_plugin_description(int id)
{
 plugin_description(timbre);
}


iKXPluginGUI *iTimbrePlugin::create_cp(kDialog *parent, kFile *mf)
{
	iTimbrePluginDlg *tmp;
	tmp = new iTimbrePluginDlg(this, parent,mf);
	tmp->create();
	return tmp;
}

int iTimbrePlugin::get_user_interface()
{
	return IKXPLUGIN_OWNER;
}


/* Plugin UI Implementation */

void iTimbrePluginDlg::init()
{
	CKXPluginGUI::init();

	// custom dialog initialization here...
	// MAKEUP_DLG(Caption, Width, Height)
	create_dialog(plugin_name(timbre), 240, 220,-1,KXFX_NO_RESIZE);

	// SHOW_VFADER(Fader, Ctrl_ID, Label, MinVal, MaxVal, Left, Top, Width, Height)
	
	create_vfader(fdrSetVol, SETVOL_ID, "Eq 100%", -120, -1, 5, 12, 60, 170);
	create_vfader(fdrBass, BASS_ID, " Bass"      , -120, 120, 60, 12, 50, 170);
	create_vfader(fdrTreble, TREBLE_ID, " Treble", -120, 120, 105, 12, 50, 170);
	create_vfader(fdrVol, VOL_ID, " Vol"         , -120, 0, 150, 12, 60, 170);
	
	fdrBass.slider.set_page_size(10);
	fdrTreble.slider.set_page_size(10);	
	fdrVol.slider.set_page_size(10);	
	fdrSetVol.slider.set_page_size(10);

	// SHOW_CHECKBOX(CheckBox, Ctrl_ID, Caption, Left, Top, Width)

	controls_enabled = TRUE;

	for (int i = 0; i < TIMBRE_PARAMS_COUNT; i++)
	 sync(i);

        redraw();
}

void iTimbrePluginDlg::sync(int ndx)
{
	char c[MAX_LABEL_STRING];
	kxparam_t v;

	switch (ndx)
	{ 
	case EVERYPM_ID:
	case BASS_ID:
		v = plugin->_params[BASS_ID];
		if (controls_enabled) {VSSETPOS(fdrBass.slider, (int)v); fdrBass.slider.redraw();}
		sprintf(c, "%.1fdB", v/10.);
		fdrBass.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;

	case TREBLE_ID:
		v = plugin->_params[TREBLE_ID];
		if (controls_enabled) {VSSETPOS(fdrTreble.slider, (int)v); fdrTreble.slider.redraw();}
		sprintf(c, "%.1fdB", v/10.);
		fdrTreble.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;
	case VOL_ID:
		v = plugin->_params[VOL_ID];
		if (controls_enabled) {VSSETPOS(fdrVol.slider, (int)v); fdrVol.slider.redraw();}
		sprintf(c, "%.1fdB", v/1.);
		fdrVol.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;
	case SETVOL_ID:
		v = plugin->_params[SETVOL_ID];
		if (controls_enabled) {VSSETPOS(fdrSetVol.slider, (int)v); fdrSetVol.slider.redraw();}
		sprintf(c, "%.1fdB", v/1.);
		fdrSetVol.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;
	}
}
