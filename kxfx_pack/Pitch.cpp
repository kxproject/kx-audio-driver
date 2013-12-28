// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov and Hanz Petrov, 2003-2004.
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
#include "pitch.h"
// effect source
#include "da_pitch.cpp"

inline kxparam_t kabs(kxparam_t v)
{
        if(v<0) return -v; else return v;
}


// plugin parameters
// (these are 'logical' parameters visible to the user)
typedef enum _params_id{
	LEVEL_ID, //0 to 100
	PITCH_ID, //-1250 to 1250
};

// default plugin parameters
// -------------------------
static kxparam_t default_params[PITCH_PARAMS_COUNT] = {100, 0}; //{0, 100, 0}

int iPitchPlugin::set_defaults() 
{
	set_all_params(default_params); 
	return 0;
}

// for kX Automation: param description (see kxplugin.h for details on kx_fxparam_descr)
// -------------------------------------------------------------------------------------

static kx_fxparam_descr _param_descr[PITCH_PARAMS_COUNT] = {
	{"Level", KX_FXPARAM_LEVEL, 0, 100 },
	{"Pitch",   KX_FXPARAM_CENTS, -1250, 1250}
	};

int iPitchPlugin::describe_param(int ndx, kx_fxparam_descr *descr)
{
	memcpy(descr, &_param_descr[ndx], sizeof kx_fxparam_descr); return 0;
}

// presets
// -------

#define P (kxparam_t)

static kxparam_t presets[]=
{
 // don't include 'default' preset
 // P "preset name", p1, p2, p3, ...
 NULL // must be here
};

#undef P

const kxparam_t *iPitchPlugin::get_plugin_presets()
{
 return presets;
}


/* Plugin Implemenation */

int iPitchPlugin::get_param(int ndx, kxparam_t *value) // ndx corresponds to 'logical' parameter id
{
	*value = _params[ndx]; 
	return 0;
}

// set_param sets 'logical' values by writing to (one or a group of) 'physical' registers

int iPitchPlugin::set_param(int ndx, kxparam_t value)
{
	_params[ndx] = value;
	double x;
	double y;
	switch (ndx)
	{
	case LEVEL_ID:
		x = (double)value; //range is 0 to 100%
		x=x/100.0;
		set_dsp_register(_LEVEL, _dbl_to_dspword(x));
		break;

	case PITCH_ID:
		// fixme !! perform truncation..
		x = (double)value; //range is -12.50 to 12.50
		x=x/100.0;
		y = x;

		x=(-3.372957086043240E-09+x*-8.859580403504260E-05)/
		(1.0+x*(-0.02887715356902830+x*0.0002749755844979194));
		set_dsp_register(_PITCH, _dbl_to_dspword(x));
		
		y=-4.002216357671908E-07+y*(-5.641437909059427E-05+
		y*(-1.621408704872448E-06+y*(-3.204726841065972E-08+
		y*-4.932649619664321E-10)));
		set_dsp_register(_PITCH2, _dbl_to_dspword(y));

		break;
	}

        // we need do synchronize the GUI part, if set_param was called by the mixer or kX Automation
	if (cp) ((iPitchPluginDlg*) cp)->sync(ndx);

	return 0;
}

int iPitchPlugin::set_all_params(kxparam_t* values)
{
	for (int i = 0; i < PITCH_PARAMS_COUNT; i++) set_param(i, values[i]);
	return 0;
}

int iPitchPlugin::request_microcode() 
{
	publish_microcode(pitch);

	return 0;
}

const char *iPitchPlugin::get_plugin_description(int id)
{
	plugin_description(pitch);
}


iKXPluginGUI *iPitchPlugin::create_cp(kDialog *parent, kFile *mf)
{
	iPitchPluginDlg *tmp;
	tmp = new iPitchPluginDlg(this, parent,mf);
	tmp->create();
	return tmp;
}

int iPitchPlugin::get_user_interface()
{
	return IKXPLUGIN_OWNER; // means 'custom' GUI panel
}


/* Plugin UI Implementation */

void iPitchPluginDlg::init()
{
	CKXPluginGUI::init(); // this is necessary

	// custom dialog initialization here...
	// (title, Width, Height)
	create_dialog(plugin_name(pitch), 350, 85);

	// create_hfader(Fader, Ctrl_ID, Label, MinVal, MaxVal, Left, Top, Width, Label_Width)
	
	create_hfader(fdrLevel, LEVEL_ID, "Level (%):",0,100, 7, 12, 283, 110);
	create_hfader(fdrPitch, PITCH_ID, "Pitch (semitones):",-1250,1250, 7, 48, 283, 110);

	controls_enabled = 1; // enable updating GUI when the parameters are changed

	sync(EVERYPM_ID); // update all 'GUI' elements to the preset plugin parameters
}

void iPitchPluginDlg::sync(int ndx)
{
	char c[MAX_LABEL_STRING];
	kxparam_t v;

        // this function should 'synchronize' any labels with the parameters

	switch (ndx)
	{ 
	case EVERYPM_ID:

	case LEVEL_ID:
		v = plugin->_params[LEVEL_ID];
		if (controls_enabled) {fdrLevel.slider.set_pos((int)v); fdrLevel.slider.redraw();} // update fader value only if function called from mixer
		sprintf(c, "%d", v);
		fdrLevel.svalue.SetWindowText(c); // show new value for fader
		if (ndx != EVERYPM_ID) break;
	case PITCH_ID:
		v = plugin->_params[PITCH_ID];
		if (controls_enabled) {fdrPitch.slider.set_pos((int)v); fdrPitch.slider.redraw();} // update fader value only if function called from mixer
		sprintf(c, "%d.%d", ((v>0)?1:(-1))*(int)kabs(v/100), (int)kabs(v%100));
		fdrPitch.svalue.SetWindowText(c); // show new value for fader
		if (ndx != EVERYPM_ID) break;
		// fallthru
	}
}
