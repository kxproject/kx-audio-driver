// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov, Max Mikhailov, and Hanz, 2002-2004.
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

// (c) Hanz Petrov, 2002-2004

#include "stdafx.h"

// --------------- the above should be a 'standard' beginning of any .cpp file

// effect class
#include "flanger.h"
// effect source
#include "da_flanger.cpp"

// Plugin Parameters (this identifiers also used for gui controls)
typedef enum _params_id{
	WET_LEV_ID,	// 4 parameters in plugin, see below
	FEEDBACK_ID,
	LFO_DEPTH_ID,
	LFO_FREQ_ID,
};

// default plugin Parameters (used when plugin is loaded first time)
static kxparam_t flanger_default_params[FLANGER_PARAMS_COUNT] = {50 , -50, 100, 25};

static kx_fxparam_descr _param_descr[FLANGER_PARAMS_COUNT] = {
	{"Wet Level",   KX_FXPARAM_PERCENT, 0,100 },
	{"Feedback", KX_FXPARAM_PERCENT, -99,99 },
	{"LFO Depth", KX_FXPARAM_PERCENT, 0,100 },
	{"LFO Frequency", KX_FXPARAM_HZ, 0,1000  }
	};

int iFlangerPlugin::describe_param(int ndx, kx_fxparam_descr *descr)
{
 memcpy(descr, &_param_descr[ndx], sizeof kx_fxparam_descr); 
 return 0;
}


/* Plugin Implementation */

int iFlangerPlugin::get_param(int ndx, kxparam_t *value) // ndx corresponds to parameter id
{
	*value = _params[ndx]; 
	return 0;
}

int iFlangerPlugin::set_param(int ndx, kxparam_t value)
{
	_params[ndx] = value; // saving value into _params[] for later use

	double v; 

	switch (ndx)
	{
	case WET_LEV_ID:
		set_dsp_register(_WET_LEVEL, itodspr_scl(100, value)); // 0...100 -> 0.0...0.999
		break;
	case FEEDBACK_ID:
		v = (double)value ; //range is -99 to 99 linear
		v = (v/99.0);
		v = v * 0.98999;
		set_dsp_register(_FEEDBACK, _dbl_to_dspword(v)); // writing value to level register with conversion from double to dsp's dword
		break;
	case LFO_DEPTH_ID:
		v = (double)value; //range is 0 to 100 linear
		v = (v/100.0);
		v = v * 0.000092;
		set_dsp_register(_LFO_DEPTH, _dbl_to_dspword(v)); // writing value to level register with conversion from double to dsp's dword
		break;
	case LFO_FREQ_ID:
		v = (double)value; //range is 0 to 1000 linear
		v = (v/1000.0);
		v = v * 0.001308;
		set_dsp_register(_LFO_FREQ, _dbl_to_dspword(v)); // writing value to level register with conversion from double to dsp's dword
		break;
	}

	// when "control panel" (cp) for plugin is loaded, this will "synchronize" ui controls to plugin parameters
	if (cp) ((iFlangerPluginDlg*) cp)->sync(ndx);

	return 0;
}

int iFlangerPlugin::set_defaults() 
{
	set_all_params(flanger_default_params); 
	return 0;
}

int iFlangerPlugin::set_all_params(kxparam_t* values)
{
	for (int i = 0; i < FLANGER_PARAMS_COUNT; i++) set_param(i, values[i]);
	return 0;
}

int iFlangerPlugin::request_microcode() 
{
	publish_microcode(flanger);
	return 0;
}

iKXPluginGUI *iFlangerPlugin::create_cp(kDialog *parent, kFile *mf)
{
	iFlangerPluginDlg *tmp;
	tmp = new iFlangerPluginDlg(this, parent, mf);
	tmp->create();
	return tmp;
}

int iFlangerPlugin::get_user_interface()
{
	return IKXPLUGIN_OWNER;	// says to mixer that this plugin has its own control panel
}


/* Plugin UI Implementation */

void iFlangerPluginDlg::init()
{
	CKXPluginGUI::init();
	// custom dialog initialization here...

	// create_dialog(Caption, Width, Height, ExtraHeight = 0)
	create_dialog(plugin_name(flanger), 300, 160); //w=275

	// create_hfader(Fader, Ctrl_ID, Label, MinVal, MaxVal, Left, Top, Width, Label_Width)
	create_hfader(fdrWetLev, WET_LEV_ID, "Wet Level (%):",0,100, 7, 12, 283, 110); // fader must be declared with plugin dialog class
	create_hfader(fdrFeedback, FEEDBACK_ID, "Feedback (%):",-99,99, 7, 48, 283, 110);
	create_hfader(fdrLFODepth, LFO_DEPTH_ID, "LFO Depth (%):",0,100, 7, 84, 283, 110);
	create_hfader(fdrLFOFreq, LFO_FREQ_ID, "LFO Frequency (Hz):", 0, 1000, 7, 120, 283, 110);	
	
	controls_enabled = TRUE;

	sync(EVERYPM_ID);
}

void iFlangerPluginDlg::sync(int ndx)
{
	char c[MAX_LABEL_STRING];
	kxparam_t v;
	switch (ndx)
	{
	case EVERYPM_ID: // update all controls
	case WET_LEV_ID:
		v = plugin->_params[WET_LEV_ID];
		if (controls_enabled) {fdrWetLev.slider.set_pos((int)v); fdrWetLev.slider.redraw();} // update fader value only if function called from mixer
		sprintf(c, "%i", v);
		fdrWetLev.svalue.SetWindowText(c); // show new value for fader
		if (ndx != EVERYPM_ID) break;
	case FEEDBACK_ID:
		v = plugin->_params[FEEDBACK_ID];
		if (controls_enabled) {fdrFeedback.slider.set_pos((int)v); fdrFeedback.slider.redraw();} // update fader value only if function called from mixer
		sprintf(c, "%i", v);
		fdrFeedback.svalue.SetWindowText(c); // show new value for fader
		if (ndx != EVERYPM_ID) break;
	case LFO_DEPTH_ID:
		v = plugin->_params[LFO_DEPTH_ID];
		if (controls_enabled) {fdrLFODepth.slider.set_pos((int)v); fdrLFODepth.slider.redraw();} // update fader value only if function called from mixer
		sprintf(c, "%i", v);
		fdrLFODepth.svalue.SetWindowText(c); // show new value for fader
		if (ndx != EVERYPM_ID) break;
	case LFO_FREQ_ID:
		v = plugin->_params[LFO_FREQ_ID];
		if (controls_enabled) {fdrLFOFreq.slider.set_pos((int)v); fdrLFOFreq.slider.redraw();} // update fader value only if function called from mixer
		sprintf(c, "%i.%i", v/100, v%100);
		fdrLFOFreq.svalue.SetWindowText(c); // show new value for fader
		if (ndx != EVERYPM_ID) break;
  }
}

