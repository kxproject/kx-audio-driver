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
#include "da_gain.cpp"
// effect class
#include "gain.h"

#define GAIN_RESOLUTION		4
#define GAIN_UNIT		0x20000000 // 0x80000000U/GAIN_RESOLUTION

// Plugin Parameters (this identifiers also used for gui controls)
typedef enum _params_id{
	GAIN_ID
};

// default plugin Parameters (used when plugin is loaded first time)
static kxparam_t gain_default_params[GAIN_PARAMS_COUNT] = { GAIN_UNIT }; 
							//{bits, samplerate}

static kx_fxparam_descr _param_descr[GAIN_PARAMS_COUNT] = {
	{"Gain",   KX_FXPARAM_LEVEL, 1, 0x7fffffff }
	};

int iGainPlugin::describe_param(int ndx, kx_fxparam_descr *descr)
{
 memcpy(descr, &_param_descr[ndx], sizeof kx_fxparam_descr); 
 return 0;
}


/* Plugin Implementation */

int iGainPlugin::get_param(int ndx, kxparam_t *value) // ndx corresponds to parameter id
{
	*value = _params[ndx]; 
	return 0;
}

int iGainPlugin::set_param(int ndx, kxparam_t value)
{
	//kxparam_t t;
	_params[ndx] = value; // saving value into _params[] for later use


	switch (ndx)
	{
	case GAIN_ID:
		set_dsp_register(_INTEGER, (dword)(value/GAIN_UNIT));
		set_dsp_register(_FRACTION, (dword)((value&(GAIN_UNIT-1))*GAIN_RESOLUTION));
	break;
	}

	// when "control panel" (cp) for plugin is loaded, this will "synchronize" ui controls to plugin parameters
	if (cp) ((iGainPluginDlg*) cp)->sync(ndx);

	return 0;
}

int iGainPlugin::set_defaults() 
{
	set_all_params(gain_default_params); 
	return 0;
}

int iGainPlugin::set_all_params(kxparam_t* values)
{
	for (int i = 0; i < GAIN_PARAMS_COUNT; i++) set_param(i, values[i]);
	/* you can perform parameters calculations right here (as i did in Reverb/Chorus),
	or just call set_param for every parameter (like above) */

	return 0;
}

int iGainPlugin::request_microcode() 
{
	publish_microcode(gain);
	return 0;
}

iKXPluginGUI *iGainPlugin::create_cp(kDialog *parent, kFile *mf)
{
	iGainPluginDlg *tmp;
	tmp = new iGainPluginDlg(this, parent, mf);
	tmp->create();
	return tmp;
}

int iGainPlugin::get_user_interface()
{
	return IKXPLUGIN_OWNER;	// says to mixer that this plugin has its own control panel
}


/* Plugin UI Implementation */

void iGainPluginDlg::init()
{
	CKXPluginGUI::init();

	// custom dialog initialization here...

	// create_dialog(Caption, Width, Height, ExtraHeight)
	create_dialog(plugin_name(gain), 380, 45); //w=275

	// create_hfader(Fader, Ctrl_ID, Label, MinVal, MaxVal, Left, Top, Width, Label_Width)
	create_hfader(fdrGain, GAIN_ID, "Gain:",1,0x7fffffff, 5, 10, 350, 50,12);

	controls_enabled = TRUE;

	sync(EVERYPM_ID);
}

void iGainPluginDlg::sync(int ndx)
{
	char c[MAX_LABEL_STRING];
	kxparam_t v;
	switch (ndx)
	{
	case EVERYPM_ID: // update all controls
	case GAIN_ID:
		v = plugin->_params[GAIN_ID];
		if (controls_enabled) {fdrGain.slider.set_pos((int)v); fdrGain.slider.redraw();} // update fader value only if function called from mixer

		float t=(float)(v/GAIN_UNIT)+((float)(v&(GAIN_UNIT-1))*(float)GAIN_RESOLUTION/(float)0x7fffffff);

		sprintf(c, "%+2.2fdB",20.0f*(float)log10(t));
		fdrGain.svalue.SetWindowText(c); // show new value for fader
		if (ndx != EVERYPM_ID) break;
  }
}
