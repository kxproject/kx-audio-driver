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
#include "da_decimator.cpp"
// effect class
#include "decimator.h"

// Plugin Parameters (this identifiers also used for gui controls)
typedef enum _params_id{
	BITS_ID,
	SRATE_ID,
};

// default plugin Parameters (used when plugin is loaded first time)
static kxparam_t decimator_default_params[DECIMATOR_PARAMS_COUNT] = {32, 48000}; 
							//{bits, samplerate}

static kx_fxparam_descr _param_descr[DECIMATOR_PARAMS_COUNT] = {
	{"Bits",   KX_FXPARAM_USER, 1, 32},
	{"Sample Rate", KX_FXPARAM_HZ, 1, 48000}
	};

int iDecimatorPlugin::describe_param(int ndx, kx_fxparam_descr *descr)
{
 memcpy(descr, &_param_descr[ndx], sizeof kx_fxparam_descr); 
 return 0;
}


/* Plugin Implementation */

int iDecimatorPlugin::get_param(int ndx, kxparam_t *value) // ndx corresponds to parameter id
{
	*value = _params[ndx]; 
	return 0;
}

int iDecimatorPlugin::set_param(int ndx, kxparam_t value)
{
	//kxparam_t t;
	_params[ndx] = value; // saving value into _params[] for later use


	switch (ndx)
	{
	case BITS_ID:
		
		// the bit decimation is done using bitwise & with input samples.
		set_dsp_register(_BITS, (0xFFFFFFFF << (32-value)));

	break;
	case SRATE_ID:

		// the dsp code treats sample rate as a fractional which is a
		// fraction of max (normal) sample rate
		set_dsp_register(_SRATE, (_dbl_to_dspword(value/48000.0)));
	break;
	}

	// when "control panel" (cp) for plugin is loaded, this will "synchronize" ui controls to plugin parameters
	if (cp) ((iDecimatorPluginDlg*) cp)->sync(ndx);

	return 0;
}

int iDecimatorPlugin::set_defaults() 
{
	set_all_params(decimator_default_params); 
	return 0;
}

int iDecimatorPlugin::set_all_params(kxparam_t* values)
{
	for (int i = 0; i < DECIMATOR_PARAMS_COUNT; i++) set_param(i, values[i]);
	/* you can perform parameters calculations right here (as i did in Reverb/Chorus),
	or just call set_param for every parameter (like above) */

	return 0;
}

int iDecimatorPlugin::request_microcode() 
{
	publish_microcode(decimator);
	return 0;
}

iKXPluginGUI* iDecimatorPlugin::create_cp(kDialog *parent, kFile *mf)
{
	iDecimatorPluginDlg *tmp;
	tmp = new iDecimatorPluginDlg(this, parent, mf);
	tmp->create();
	return tmp;
}

int iDecimatorPlugin::get_user_interface()
{
	return IKXPLUGIN_OWNER;	// says to mixer that this plugin has its own control panel
}


/* Plugin UI Implementation */

void iDecimatorPluginDlg::init()
{
	CKXPluginGUI::init();

	// custom dialog initialization here...

	// create_dialog(Caption, Width, Height, ExtraHeight)
	create_dialog(plugin_name(decimator), 330, 95); //w=275

	// create_hfader(Fader, Ctrl_ID, Label, MinVal, MaxVal, Left, Top, Width, Label_Width)
	create_hfader(fdrBits, BITS_ID, "Bits:", 1, 32, 7, 12, 315, 110); 
	create_hfader(fdrSrate, SRATE_ID, "Samplerate:",1,48000, 7, 48, 315, 110);

	controls_enabled = TRUE;

	sync(EVERYPM_ID);
}

void iDecimatorPluginDlg::sync(int ndx)
{
	char c[MAX_LABEL_STRING];
	kxparam_t v;
	switch (ndx)
	{
	case EVERYPM_ID: // update all controls
	case BITS_ID:
		v = plugin->_params[BITS_ID];
		if (controls_enabled) {fdrBits.slider.set_pos((int)v); fdrBits.slider.redraw();} // update fader value only if function called from mixer
		sprintf(c, "%i", v);
		fdrBits.svalue.SetWindowText(c); // show new value for fader
		if (ndx != EVERYPM_ID) break;
	case SRATE_ID:
		v = plugin->_params[SRATE_ID];
		if (controls_enabled) {fdrSrate.slider.set_pos((int)v); fdrSrate.slider.redraw();} // update fader value only if function called from mixer
		sprintf(c, "%i", v);
		fdrSrate.svalue.SetWindowText(c); // show new value for fader
		if (ndx != EVERYPM_ID) break;
  }
}
