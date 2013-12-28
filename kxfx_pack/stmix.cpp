// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov, Max Mikhailov 2001-2005.
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
#include "stmix.h"
// effect source
#include "da_stmix.cpp"

// Plugin Parameters (this identifiers also used for gui controls)
typedef enum _params_id{
	GAIN1_ID,
	GAIN2_ID
};

static kx_fxparam_descr _param_descr[STMIX_PARAMS_COUNT] = {
	{"Gain1", KX_FXPARAM_CB, -960, 240},
	{"Gain2", KX_FXPARAM_CB, -960, 240}
};

int iStmixPlugin::describe_param(int ndx, kx_fxparam_descr *descr)
{
	memcpy(descr, &_param_descr[ndx], sizeof kx_fxparam_descr); return 0;
}

// default plugin Parameters (used when plugin is loaded for the first time)
static kxparam_t stmix_default_params[STMIX_PARAMS_COUNT] = { 0, 0};

/* Plugin Implementation */

int iStmixPlugin::get_param(int ndx, kxparam_t *value) // ndx corresponds to parameter id
{
	*value = _params[ndx]; 
	return 0;
}

int iStmixPlugin::set_param(int ndx, kxparam_t value)
{
	_params[ndx] = value; // saving value into _params[] for later use

	double x;
	int gain1;
	double gain2;

	switch (ndx)
	{
	case GAIN1_ID:
		x = value/10.0;
		if (x>=0) 
		{
			x=dBtoG(x);
			//gain1 = x;
			gain1 = static_cast < int > ( x ); //convert to int to truncate
			gain1 = gain1 - static_cast < int > (gain1 * 0.0234375);
			gain2 = x - gain1; //get fractional part
			set_dsp_register(R_GAIN1_I, gain1); // writing value to level register with conversion from double to dsp's dword
			set_dsp_register(R_GAIN1_F, _dbl_to_dspword(gain2));
			break;
		}
		//-600 to 0
		x=dBtoG(x);
		gain1 = 0;
		gain2 = x;
		set_dsp_register(R_GAIN1_I, gain1); // writing value to level register with conversion from double to dsp's dword
		set_dsp_register(R_GAIN1_F, _dbl_to_dspword(gain2));
		break;
	case GAIN2_ID:
		x = value/10.0; //range is -600 to 600 dB
		if (x>=0) //0 to 600
		{
			x=dBtoG(x);
			//gain1 = x;
			gain1 = static_cast < int > ( x ); //convert to int to truncate
			gain1 = gain1 - static_cast < int > (gain1 * 0.0234375);
			gain2 = x - gain1; //get fractional part
			set_dsp_register(R_GAIN2_I, gain1); // writing value to level register with conversion from double to dsp's dword
			set_dsp_register(R_GAIN2_F, _dbl_to_dspword(gain2));
			break;
		}
		//-600 to 0
		x=dBtoG(x);
		gain1 = 0;
		gain2 = x;
		set_dsp_register(R_GAIN2_I, gain1); // writing value to level register with conversion from double to dsp's dword
		set_dsp_register(R_GAIN2_F, _dbl_to_dspword(gain2));
		break;
	}

	if (cp) ((iStmixPluginDlg*) cp)->sync(ndx);

	return 0;
}

int iStmixPlugin::set_defaults() 
{
	set_all_params(stmix_default_params); 
	return 0;
}

int iStmixPlugin::set_all_params(kxparam_t* values)
{
	for (int i = 0; i < STMIX_PARAMS_COUNT; i++) set_param(i, values[i]);
	return 0;
}

int iStmixPlugin::request_microcode() 
{
	publish_microcode(stmix);
	return 0;
}

iKXPluginGUI *iStmixPlugin::create_cp(kDialog *parent, kFile *mf)
{
	iStmixPluginDlg *tmp;
	tmp = new iStmixPluginDlg(this, parent, mf);
	tmp->create();
	return tmp;
}

int iStmixPlugin::get_user_interface()
{
	return IKXPLUGIN_OWNER;	// says to mixer that this plugin has its own control panel
}


void iStmixPluginDlg::init()
{
	CKXPluginGUI::init();
	// custom dialog initialization here...

	// create_dialog(Caption, Width, Height, ExtraHeight = 25)
	create_dialog(plugin_name(stmix), 300, 100);

	// create_hfader(Fader, Ctrl_ID, Label, MinVal, MaxVal, Left, Top, Width, Label_Width)
	create_hfader(fdrGain1,  GAIN1_ID,  "In1 Level (dB):" ,-960,240, 7, 12, 283, 110);
	create_hfader(fdrGain2,  GAIN2_ID,  "In2 Level (dB):" ,-960,240, 7, 48, 283, 110);
	fdrGain1.slider.set_tic_freq(120);
	fdrGain2.slider.set_tic_freq(120);

	controls_enabled = TRUE;

	sync(EVERYPM_ID);
}

void iStmixPluginDlg::sync(int ndx)
{
	char c[MAX_LABEL_STRING];
	kxparam_t v;
	switch (ndx)
	{
	case EVERYPM_ID: // update all controls
	
	case GAIN1_ID:
		v = plugin->_params[GAIN1_ID];
		if (controls_enabled) 
		{
		 fdrGain1.slider.set_pos((int)v); 
		 fdrGain1.slider.RedrawWindow();
		} // update fader value only if function called from mixer
		sprintf(c, " %.1f", v/10.);
		fdrGain1.svalue.SetWindowText(c); // show new value for fader
		if (ndx != EVERYPM_ID) break;
	case GAIN2_ID: // update all controls
		v = plugin->_params[GAIN2_ID];
		if (controls_enabled) 
		{
		 fdrGain2.slider.set_pos((int)v); 
		 fdrGain2.slider.RedrawWindow();
		} // update fader value only if function called from mixer
		sprintf(c, " %.1f", v/10.);
		fdrGain2.svalue.SetWindowText(c); // show new value for fader
		// if (ndx != EVERYPM_ID) break;
	}
}
