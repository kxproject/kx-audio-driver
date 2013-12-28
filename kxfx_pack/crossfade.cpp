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

#include "stdafx.h"

// --------------- the above should be a 'standard' beginning of any .cpp file

// effect source
#include "da_crossfade.cpp"
// effect class
#include "crossfade.h"

inline kxparam_t kabs(kxparam_t v)
{
        if(v<0) return -v; else return v;
}

// Plugin Parameters (this identifiers also used for gui controls)
typedef enum _params_id{
	CROSS_ID,
	GAIN_ID
};

#define P (kxparam_t)

static kxparam_t presets[]=
{
	NULL
};

#undef P

const kxparam_t *iCrossfadePlugin::get_plugin_presets()
{
 return presets;
}


static kx_fxparam_descr _param_descr[CROSSFADE_PARAMS_COUNT] = {
	{"Crossfade",      KX_FXPARAM_PERCENT, -100, 100},
	{"Gain",           KX_FXPARAM_DB, 0, 100}
};

int iCrossfadePlugin::describe_param(int ndx, kx_fxparam_descr *descr)
{
	memcpy(descr, &_param_descr[ndx], sizeof kx_fxparam_descr); return 0;
}


/* Plugin Implementation */

int iCrossfadePlugin::get_param(int ndx, kxparam_t *value) // ndx corresponds to parameter id
{
	*value = _params[ndx]; 
	return 0;
}

int iCrossfadePlugin::set_param(int ndx, kxparam_t value)
{
	//kxparam_t t;
	_params[ndx] = value; // saving value into _params[] for later use

	double k[2];
	int i;

	switch (ndx)
	{
		case CROSS_ID:
		case GAIN_ID:
			i = (_params[CROSS_ID] > 0) ? 1 : 0;
			k[i] = _params[GAIN_ID] * .01;
			k[1 - i] = (.0001 * _params[GAIN_ID]) * (100L - kabs(_params[CROSS_ID]));
			set_dsp_register(R_K1, _dbl_to_dspword(k[0]));
			set_dsp_register(R_K2, _dbl_to_dspword(k[1]));
			break;
	}

	// when "control panel" (cp) for plugin is loaded, this will "synchronize" ui controls to plugin parameters
	if (cp) ((iCrossfadePluginDlg*) cp)->sync(ndx);

	return 0;
}

// default plugin Parameters (used when plugin is loaded first time)
static kxparam_t crossfade_default_params[CROSSFADE_PARAMS_COUNT] = { 0, 100 }; 

int iCrossfadePlugin::set_defaults() 
{
	set_all_params(crossfade_default_params); 
	return 0;
}

int iCrossfadePlugin::set_all_params(kxparam_t* values)
{
	for (int i = 0; i < CROSSFADE_PARAMS_COUNT; i++) set_param(i, values[i]);
	/* you can perform parameters calculations right here (as i did in Reverb/Chorus),
	or just call set_param for every parameter (like above) */

	return 0;
}

int iCrossfadePlugin::request_microcode() 
{
	publish_microcode(crossfade);
	return 0;
}

iKXPluginGUI *iCrossfadePlugin::create_cp(kDialog *parent, kFile *mf)
{
	iCrossfadePluginDlg *tmp;
	tmp = new iCrossfadePluginDlg(this, parent, mf);
	tmp->create();
	return tmp;
}

int iCrossfadePlugin::get_user_interface()
{
	return IKXPLUGIN_OWNER;	// says to mixer that this plugin has its own control panel
}


/* Plugin UI Implementation */

void iCrossfadePluginDlg::init()
{
	CKXPluginGUI::init();

	// custom dialog initialization here...

	// create_dialog(Caption, Width, Height, ExtraHeight)
	create_dialog(plugin_name(crossfade), 380, 90);

	// create_hfader(Fader, Ctrl_ID, Label, MinVal, MaxVal, Left, Top, Width, Label_Width)
	create_hfader(fdrCross, CROSS_ID, "Cross Mix (%)",-100,100, 15, 10, 350, 40,7);
	create_hfader(fdrGain, GAIN_ID, "Gain (dB)", 0, 100, 15, 50, 350, 40,7);

	controls_enabled = TRUE;

	sync(EVERYPM_ID);
}

void iCrossfadePluginDlg::sync(int ndx)
{
	char c[MAX_LABEL_STRING];
	kxparam_t v;

	switch (ndx)
	{
	case EVERYPM_ID:
	case CROSS_ID:
		v=plugin->_params[CROSS_ID];
		if(controls_enabled) fdrCross.slider.set_pos((int)v);

		// well, i don't know how to show this... another var. is "75/25"...
		if (v < 0) sprintf(c,"<%i", (int)kabs(v));
		else if (v > 0) sprintf(c,"%i>", kabs((int)v));
		else sprintf(c,"<0>");

		fdrCross.svalue.SetWindowText(c);
                
                if(ndx!=EVERYPM_ID)
		 break;
	case GAIN_ID:
		v=plugin->_params[GAIN_ID];
		if(controls_enabled) fdrGain.slider.set_pos((int)v);

		if (v) sprintf(c,"%.1f", 20. * log10(.01 * v));
		else sprintf(c,"-Inf.");

		fdrGain.svalue.SetWindowText(c);

                if(ndx!=EVERYPM_ID)
                 break;
  }
}
