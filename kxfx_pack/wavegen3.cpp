// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov and eYagos, 2002-2004.
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

// Copyright (c) eYagos, 2002-2004. All rights reserved.

#include "stdafx.h"

// --------------- the above should be a 'standard' beginning of any .cpp file

// effect class
#include "wavegen3.h"
// effect source
#include "da_wavegen3.cpp"

// Plugin Parameters (this identifiers also used for gui controls)
typedef enum _params_id{
	LEVEL_ID,
	FRECUENCY_ID
};

// default plugin Parameters (used when plugin is loaded for the first time)
static kxparam_t wavegen3_default_params[WAVEGEN3_PARAMS_COUNT] = { -30, 1000 };

static kx_fxparam_descr _param_descr[WAVEGEN3_PARAMS_COUNT] = {
	{"Level",   KX_FXPARAM_DB, -1000, 0},
	{"Frequency", KX_FXPARAM_HZ, 0,20000 }
	};

int iWavegen3Plugin::describe_param(int ndx, kx_fxparam_descr *descr)
{
 memcpy(descr, &_param_descr[ndx], sizeof kx_fxparam_descr); 
 return 0;
}

/* Plugin Implementation */

int iWavegen3Plugin::get_param(int ndx, kxparam_t *value) // ndx corresponds to parameter id
{
	*value = _params[ndx]; 
	return 0;
}

int iWavegen3Plugin::set_param(int ndx, kxparam_t value)
{
	_params[ndx] = value; // saving value into _params[] for later use
	dword lev;
	double w, ws;
	
	switch (ndx){
	case LEVEL_ID:
		set_dsp_register(R_Level, (dword)(_dbl_to_dspword(cBtoG((double)value)*0.25)));
		break;

	case FRECUENCY_ID:
		w = 2.0 * pi * value / 48000.0;
		ws = value / 24000.0;
		get_dsp_register("LEVEL", &lev);
		set_dsp_register(R_Level, _dbl_to_dspword(0.0));
		set_dsp_register(R_S0, _dbl_to_dspword( (double) 1.0 ));
		set_dsp_register(R_S1, _dbl_to_dspword( (double) 1.0 ));
		set_dsp_register(R_F , _dbl_to_dspword( (double) cos(w) ));
		set_dsp_register(R_Fs, _dbl_to_dspword( ws ));
		set_dsp_register("LEVEL", lev);
		break;
	}
	

	if (cp) ((iWavegen3PluginDlg*) cp)->sync(ndx);

	return 0;
}

int iWavegen3Plugin::set_defaults() 
{
	set_all_params(wavegen3_default_params); 
	return 0;
}

int iWavegen3Plugin::set_all_params(kxparam_t* values)
{
	for (int i = 0; i < WAVEGEN3_PARAMS_COUNT; i++) set_param(i, values[i]);
	return 0;
}

int iWavegen3Plugin::request_microcode() 
{
	publish_microcode(wavegen3);
	return 0;
}

iKXPluginGUI *iWavegen3Plugin::create_cp(kDialog *parent, kFile *mf)
{
	iWavegen3PluginDlg *tmp;
	tmp = new iWavegen3PluginDlg(this, parent, mf);
	tmp->create();
	return tmp;
}

int iWavegen3Plugin::get_user_interface()
{
	return IKXPLUGIN_OWNER;	// says to mixer that this plugin has its own control panel
}


/* Plugin UI Implementation */

void iWavegen3PluginDlg::init()
{
	CKXPluginGUI::init();
	// custom dialog initialization here...

	// create_dialog(Caption, Width, Height, ExtraHeight = 25)
	create_dialog(plugin_name(wavegen3), 300, 100);

	// create_hfader(Fader, Ctrl_ID, Label, MinVal, MaxVal, Left, Top, Width, Label_Width)
	create_hfader(level		, LEVEL_ID		, "Level (dB):"		,-1000, 0   , 7, 12, 275, 110);
	create_hfader(frecuency , FRECUENCY_ID	, "Frecuency (Hz):"	,0    ,20000, 7, 48, 275, 110);

	controls_enabled = TRUE;

	sync(EVERYPM_ID);
}

void iWavegen3PluginDlg::sync(int ndx)
{
	char c[MAX_LABEL_STRING];
	kxparam_t v;
	switch (ndx)
	{
	case EVERYPM_ID: // update all controls
	case LEVEL_ID:
		v = plugin->_params[LEVEL_ID];
		if (controls_enabled) 
		{
		 level.slider.set_pos((int)v); 
		 level.slider.redraw();
		} // update fader value only if function called from mixer
		sprintf(c, " %.1f", v/10.);
		level.svalue.SetWindowText(c); // show new value for fader
		if (ndx != EVERYPM_ID) break;
	case FRECUENCY_ID: // update all controls
		v = plugin->_params[FRECUENCY_ID];
		if (controls_enabled) 
		{
		 frecuency.slider.set_pos((int)v); 
		 frecuency.slider.redraw();
		} // update fader value only if function called from mixer
		sprintf(c, "%d", (int)((float)v));
		frecuency.svalue.SetWindowText(c); // show new value for fader
		// if (ndx != EVERYPM_ID) break;
	}
}
