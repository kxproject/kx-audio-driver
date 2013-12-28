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

#include "stdafx.h"

// --------------- the above should be a 'standard' beginning of any .cpp file
// effect class
#include "reverblt.h"
// effect source
#include "da_reverblt.cpp"

// Parameters
typedef enum {
	WETMIX_ID,
	ERLRBAL_ID,
	DIFFUSION_ID,
	PREDELAY_ID,
	DECAY_ID
};

// default plugin parameters
static kxparam_t reverblt_default_params[REVERBLT_PARAMS_COUNT] = {100, 40, 75, 40, 52}; // valid for all ReverbLite instances


/* Plugin Implemenation */

int iReverbLitePlugin::get_param(int ndx, kxparam_t *value) 
{
	*value = _params[ndx]; return 0;
}

int iReverbLitePlugin::set_param(int ndx, kxparam_t value) // calculates effect _params
{
	kxparam_t t;
	_params[ndx] = value;

	switch (ndx)
	{
	case WETMIX_ID:
		set_dsp_register(_R_WETMIX, itodspr_scl(100, value));
		break;

	case ERLRBAL_ID:
		set_dsp_register(_R_ERLRBAL, itodspr_scl(100, value));
		break;

	case PREDELAY_ID:
		// reading delay start (write) address, adding predelay time to it and writing to delay read address
		// left channel predelay
		get_tram_addr(_R_PREDELAY_LW, (dword *) &t);
		set_tram_addr(_R_PREDELAY_LR, (dword)(t + value * SAMPLES_PER_MSEC));
		// right channel predelay
		get_tram_addr(_R_PREDELAY_RW, (dword *) &t);
		set_tram_addr(_R_PREDELAY_RR, (dword)(t + value * SAMPLES_PER_MSEC));
		break;

	case DIFFUSION_ID:
		set_dsp_register(_R_DIFFUSION, itodspr_scl((1000 / 6), value)); // mapping parameter 0-100 to register 0.0-0.6
		// no break here since DECAY also depends on DIFFUSION

	case DECAY_ID:
		// of coarse this is approximation 
		value = _params[DECAY_ID];
		value -= _params[DIFFUSION_ID] / 10; // more diffusion - more actual decay, so substract it a bit
		set_dsp_register(_R_DECAY, itodspr_scl((9800 / 66), value)); // map 0...98 to 0.0...~0.66 (9800 / 66 = 98 / 0.66)
		break;

	default:
		return -1;
	}

	if (cp) ((iReverbLitePluginDlg*) cp)->sync(ndx);

	return 0;
}

int iReverbLitePlugin::set_defaults() 
{
	set_all_params(reverblt_default_params); 
	return 0;
}

int iReverbLitePlugin::set_all_params(kxparam_t* values)
{
	kxparam_t t;

	memcpy(_params, values, REVERBLT_PARAMS_COUNT*sizeof(kxparam_t));

	// WETMIX_ID:
		set_dsp_register(_R_WETMIX, itodspr_scl(100, _params[WETMIX_ID]));

	// ERLRBAL_ID:
		set_dsp_register(_R_ERLRBAL, itodspr_scl(100, _params[ERLRBAL_ID]));

	// PREDELAY_ID:
		get_tram_addr(_R_PREDELAY_LW, (dword *) &t);
		set_tram_addr(_R_PREDELAY_LR, (dword)(t + _params[PREDELAY_ID] * SAMPLES_PER_MSEC));
		get_tram_addr(_R_PREDELAY_RW, (dword *) &t);
		set_tram_addr(_R_PREDELAY_RR, (dword)(t + _params[PREDELAY_ID] * SAMPLES_PER_MSEC));
	// DIFFUSION_ID:
		set_dsp_register(_R_DIFFUSION, itodspr_scl((1000 / 6), _params[DIFFUSION_ID])); 

	// DECAY_ID:
		t = _params[DECAY_ID];
		t -= _params[DIFFUSION_ID] / 10; 
		set_dsp_register(_R_DECAY, itodspr_scl((9800 / 66), t)); 

	if (cp) ((iReverbLitePluginDlg*) cp)->sync(EVERYPM_ID);

	return 0;
}

int iReverbLitePlugin::request_microcode() // generally is called after init()
{
	publish_microcode(reverblt);

	// you should free code and info massives in ::close() if allocated dynamically
	return 0;
}

const char *iReverbLitePlugin::get_plugin_description(int id)
{
 plugin_description(reverblt);
}

iKXPluginGUI *iReverbLitePlugin::create_cp(kDialog *parent,kFile *mf)
{
	iReverbLitePluginDlg *tmp;
	tmp = new iReverbLitePluginDlg(this, parent,mf);
	tmp->create();
	return tmp;
}

int iReverbLitePlugin::get_user_interface()
{
	return IKXPLUGIN_OWNER;
}

static kx_fxparam_descr _param_descr[REVERBLT_PARAMS_COUNT] = {
	{"Level", KX_FXPARAM_LEVEL, 0, 100},
	{"ER/LR Balance", KX_FXPARAM_PERCENT, 0, 100},
	{"Diffusion", KX_FXPARAM_PERCENT, 0, 100},
	{"PreDelay", KX_FXPARAM_MS, 0, 100},
	{"Decay", KX_FXPARAM_S, 0, 97}};

int iReverbLitePlugin::describe_param(int ndx, kx_fxparam_descr *descr)
{
	memcpy(descr, &_param_descr[ndx], sizeof kx_fxparam_descr); return 0;
}

/* Plugin UI Implementation */

void iReverbLitePluginDlg::init()
{
	CKXPluginGUI::init();

	// custom dialog initialization here...
	// MAKEUP_DLG(Caption, Width, Height)
	create_dialog(reverblt_name, 470, 200);

	// add all GUI controls

	// SHOW_VFADER(Fader, Ctrl_ID, Label, MinVal, MaxVal, Left, Top, Width, Height)
	create_vfader(fdrWetMix, WETMIX_ID, "Level",           0, 100, 5, 12, 55, 170);
	create_vfader(fdrERLRBal, ERLRBAL_ID, "ER/LR Balance", 0, 100, 65, 12, 55, 170);
	create_vfader(fdrDiffusion, DIFFUSION_ID, "Diffusion", 0, 100, 125, 12, 55, 170);

	// SHOW_HFADER(Fader, Ctrl_ID, Label, MinVal, MaxVal, Left, Top, Width, LabelWidth)
	create_hfader(fdrPreDelay, PREDELAY_ID, "PreDelay (ms)", 25, 350, 185, 27, 260, 70);
	create_hfader(fdrDecay, DECAY_ID, "Decay (sec)", 10, 97, 185, 62, 260, 70);

	controls_enabled = TRUE;

	sync(EVERYPM_ID); // Synchronize UI Controls with Plugin _params

	redraw();
}

void iReverbLitePluginDlg::sync(int ndx)
{
	char c[MAX_LABEL_STRING];
	kxparam_t v;

	switch (ndx)
	{
	case EVERYPM_ID: // update all controls
	case WETMIX_ID:
		v = plugin->_params[WETMIX_ID];
		if (controls_enabled) {VSSETPOS(fdrWetMix.slider, (int)v); fdrWetMix.slider.redraw();}
		sprintf(c, "%i " "%%", v);
		fdrWetMix.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;

	case ERLRBAL_ID:
		v = plugin->_params[ERLRBAL_ID];
		if (controls_enabled) {VSSETPOS(fdrERLRBal.slider, (int)v); fdrERLRBal.slider.redraw();}
		sprintf(c, "%i " "%%", v);
		fdrERLRBal.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;

	case PREDELAY_ID:
		v = plugin->_params[PREDELAY_ID];
		if (controls_enabled) {fdrPreDelay.slider.set_pos((int)v); fdrPreDelay.slider.redraw();}
		sprintf(c, "%i", v);
		fdrPreDelay.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;

	case DIFFUSION_ID:
		v = plugin->_params[DIFFUSION_ID];
		if (controls_enabled) {VSSETPOS(fdrDiffusion.slider, (int)v); fdrDiffusion.slider.redraw();} 
		sprintf(c, "%i " "%%", v);
		fdrDiffusion.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;

	case DECAY_ID:
		v = plugin->_params[DECAY_ID];
		if (controls_enabled) {fdrDecay.slider.set_pos((int)v); fdrDecay.slider.redraw();}
		v = v * v; // this trick make Decay Fader to look logarithmic ;)
		sprintf(c, "%i.%i", v/1000, (v/100)%10);
		fdrDecay.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;
	}
}

