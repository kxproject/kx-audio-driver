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

// effect class
#include "autowah.h"
// effect source
#include "da_autowah.cpp"

// Plugin Parameters (this identifiers also used for gui controls)
typedef enum _params_id{
	BASE_ID,	// 5 parameters in plugin, see below
	BAND_ID,
	ATTACK_ID,
	RELEASE_ID,
	RANGE_ID,
};
/*
#define _BASE_CENT_FREQ	0x8003
#define _BANDWIDTH		0x8004
#define _ATT_TIME		0x8005
#define _REL_TIME		0x8006
#define _RANGE			0x8007
*/


// default plugin Parameters (used when plugin is loaded first time)
static kxparam_t autowah_default_params[AUTOWAH_PARAMS_COUNT] = {120, 120, 0, 60, 50};

static kx_fxparam_descr _param_descr[AUTOWAH_PARAMS_COUNT] = {
	{"Base Center Freq",   KX_FXPARAM_HZ, 80,2400 },
	{"Bandwidth", KX_FXPARAM_HZ, 1, 800 },
	{"Attack Time", KX_FXPARAM_MS, 0, 500},
	{"Release Time", KX_FXPARAM_MS, 10, 1000 },
	{"Range", KX_FXPARAM_PERCENT, 0, 100 }
	};

int iAutoWahPlugin::describe_param(int ndx, kx_fxparam_descr *descr)
{
 memcpy(descr, &_param_descr[ndx], sizeof kx_fxparam_descr); 
 return 0;
}


/* Plugin Implementation */

int iAutoWahPlugin::get_param(int ndx, kxparam_t *value) // ndx corresponds to parameter id
{
	*value = _params[ndx]; 
	return 0;
}

int iAutoWahPlugin::set_param(int ndx, kxparam_t value)
{
	_params[ndx] = value; // saving value into _params[] for later use

	double v; 

	switch (ndx)
	{
	case BASE_ID:
		v = (double)value - 80.0; //range is 80 to 2400 non-linear
		v = (v/2320);
		v = pow(v,2);
		v = v * 0.048888;
		v = v - 0.999945;
		set_dsp_register(_BASE_CENT_FREQ, _dbl_to_dspword(v));
		break;
	case BAND_ID:
		v = (double)value - 1.0; //range is 1 to 800 ~linear
		v = (v/799.0);
		v = v * 0.099464;
		v = 0.999868 - v;
		set_dsp_register(_BANDWIDTH, _dbl_to_dspword(v)); // writing value to level register with conversion from double to dsp's dword
		break;
	case ATTACK_ID:
		v = (double)value; //range is 0 to 500 inv-non-linear
		v = (v/500.0);
		v = sqrt(sqrt(sqrt(sqrt(v))));
		v = v * 0.999904;
		set_dsp_register(_ATT_TIME, _dbl_to_dspword(v)); // writing value to level register with conversion from double to dsp's dword
		break;
	case RELEASE_ID:
		v = (double)value - 10.0; //range is 10 to 1000 non-linear
		v = (v/990.0);
		v = sqrt(sqrt(sqrt(sqrt(v))));
		v = v * 0.004738;
		v = 0.995214 + v;
		set_dsp_register(_REL_TIME, _dbl_to_dspword(v)); // writing value to level register with conversion from double to dsp's dword
		break;
	case RANGE_ID:
		set_dsp_register(_RANGE, itodspr_scl(100, value)); // 0...100 -> 0.0...0.999
		break;

	}

	// when "control panel" (cp) for plugin is loaded, this will "synchronize" ui controls to plugin parameters
	if (cp) ((iAutoWahPluginDlg*) cp)->sync(ndx);

	return 0;
}

int iAutoWahPlugin::set_defaults() 
{
	set_all_params(autowah_default_params); 
	return 0;
}

int iAutoWahPlugin::set_all_params(kxparam_t* values)
{
	for (int i = 0; i < AUTOWAH_PARAMS_COUNT; i++) set_param(i, values[i]);

	return 0;
}

int iAutoWahPlugin::request_microcode() 
{
	publish_microcode(autowah);
	return 0;
}

iKXPluginGUI *iAutoWahPlugin::create_cp(kDialog *parent, kFile *mf)
{
	iAutoWahPluginDlg *tmp;
	tmp = new iAutoWahPluginDlg(this, parent, mf);
	tmp->create();
	return tmp;
}

int iAutoWahPlugin::get_user_interface()
{
	return IKXPLUGIN_OWNER;	// says to mixer that this plugin has its own control panel
}


/* Plugin UI Implementation */

void iAutoWahPluginDlg::init()
{
	CKXPluginGUI::init();
	// custom dialog initialization here...

	// create_dialog(Caption, Width, Height, ExtraHeight = 0)
	create_dialog(plugin_name(autowah), 300, 195); //w=275

	// create_hfader(Fader, Ctrl_ID, Label, MinVal, MaxVal, Left, Top, Width, Label_Width)
	create_hfader(fdrBase, BASE_ID, "Base Center Freq.(Hz):",80,2400, 7, 12, 283, 110); // fader must be declared with plugin dialog class
	create_hfader(fdrBand, BAND_ID, "Bandwidth (Hz):",1,800, 7, 48, 283, 110);
	create_hfader(fdrAttack, ATTACK_ID, "Attack Time (msec):",0,500, 7, 84, 283, 110);
	create_hfader(fdrRelease, RELEASE_ID, "Release Time (msec):", 10, 1000, 7, 120, 283, 110);
	create_hfader(fdrRange, RANGE_ID, "Range (%):",0,100, 7, 156, 283, 110);	
	
	controls_enabled = TRUE;

	sync(EVERYPM_ID);
}

void iAutoWahPluginDlg::sync(int ndx)
{
	char c[MAX_LABEL_STRING];
	kxparam_t v;
	switch (ndx)
	{
	case EVERYPM_ID: // update all controls
	case BASE_ID:
		v = plugin->_params[BASE_ID];
		if (controls_enabled) {fdrBase.slider.set_pos((int)v); fdrBase.slider.redraw();} // update fader value only if function called from mixer
		sprintf(c, "%i", v);
		fdrBase.svalue.SetWindowText(c); // show new value for fader
		if (ndx != EVERYPM_ID) break;
	case BAND_ID:
		v = plugin->_params[BAND_ID];
		if (controls_enabled) {fdrBand.slider.set_pos((int)v); fdrBand.slider.redraw();} // update fader value only if function called from mixer
		sprintf(c, "%i", v);
		fdrBand.svalue.SetWindowText(c); // show new value for fader
		if (ndx != EVERYPM_ID) break;
	case ATTACK_ID:
		v = plugin->_params[ATTACK_ID];
		if (controls_enabled) {fdrAttack.slider.set_pos((int)v); fdrAttack.slider.redraw();} // update fader value only if function called from mixer
		sprintf(c, "%i", v);
		fdrAttack.svalue.SetWindowText(c); // show new value for fader
		if (ndx != EVERYPM_ID) break;
	case RELEASE_ID:
		v = plugin->_params[RELEASE_ID];
		if (controls_enabled) {fdrRelease.slider.set_pos((int)v); fdrRelease.slider.redraw();} // update fader value only if function called from mixer
		sprintf(c, "%i", v);
		fdrRelease.svalue.SetWindowText(c); // show new value for fader
		if (ndx != EVERYPM_ID) break;
	case RANGE_ID:
		v = plugin->_params[RANGE_ID];
		if (controls_enabled) {fdrRange.slider.set_pos((int)v); fdrRange.slider.redraw();} // update fader value only if function called from mixer
		sprintf(c, "%i", v);
		fdrRange.svalue.SetWindowText(c); // show new value for fader
		if (ndx != EVERYPM_ID) break;
	}
}

