// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov and Max Mikhailov, 2001-2003.
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

// Copyright (c) 2004 Eugeniy Sokol. All rights reserved

#include "stdafx.h"

// --------------- the above should be a 'standard' beginning of any .cpp file

// effect class
#include "NoiseGate2Ts.h"
// effect source
#include "da_NoiseGate2Ts.cpp"

// plugin parameters
// (these are 'logical' parameters visible to the user)
typedef enum _params_id{
	HOLD_ID,
	THUP_ID,
	THDN_ID,
	ER_ID,			
	EA_ID			
};

// default plugin parameters
// -------------------------
static kxparam_t default_params[NoiseGate2Ts_PARAMS_COUNT] = {0, 0, 0, 0};

int iNoiseGate2TsPlugin::set_defaults() 
{
	set_all_params(default_params); 
	return 0;
}

// for kX Automation: param description (see kxplugin.h for details on kx_fxparam_descr)
// -------------------------------------------------------------------------------------

static kx_fxparam_descr _param_descr[NoiseGate2Ts_PARAMS_COUNT] = {
	{"HOLD",	KX_FXPARAM_USER, 0,    1000},
	{"THUP",	KX_FXPARAM_USER, 0,    1000},
	{"THDN",	KX_FXPARAM_USER, 0,    1000},
	{"ER",		KX_FXPARAM_USER, 0,    1000},
	{"EA",		KX_FXPARAM_USER, 0,    1000}
	};

int iNoiseGate2TsPlugin::describe_param(int ndx, kx_fxparam_descr *descr)
{
	memcpy(descr, &_param_descr[ndx], sizeof kx_fxparam_descr); return 0;
}

// presets
// -------

#define P (kxparam_t)

static kxparam_t presets[]=
{
 // don't include 'default' preset
 // P "preset_name", p1, p2, ... p(PARAMS_COUNT),
 // NULL

 NULL // must be here
};

#undef P

const kxparam_t *iNoiseGate2TsPlugin::get_plugin_presets()
{
 return presets;
}


/* Plugin Implemenation */

int iNoiseGate2TsPlugin::get_param(int ndx, kxparam_t *value) // ndx corresponds to 'logical' parameter id
{
	*value = _params[ndx]; 
	return 0;
}

// set_param sets 'logical' values by writing to (one or a group of) 'physical' registers

int iNoiseGate2TsPlugin::set_param(int ndx, kxparam_t value)
{
	double x;
	kxparam_t v; dword efxvalue;
	_params[ndx] = value;
	switch (ndx)
	{
	case HOLD_ID:
		x=-double(value)/1000.*80.;
		x=dBtoG(x);
		efxvalue=int(0x7fffffff*x);
		set_dsp_register(efxHOLD, efxvalue);
		break;

	case THUP_ID:
		x=-double(value)/1000.*80.;
		x=dBtoG(x);
		efxvalue=int(0x7fffffff*x);
		set_dsp_register(efxTHUP, efxvalue);
		//
		v = _params[THDN_ID];
		x=-(double(v)+double(value)-double(v)*double(value)/1000.)/1000.*80.;
		x=dBtoG(x);
		efxvalue=int(0x7fffffff*x);
		set_dsp_register(efxTHDN, efxvalue);
		
		break;

	case THDN_ID:
		v = _params[THUP_ID];
		x=-(double(v)+double(value)-double(v)*double(value)/1000.)/1000.*80.;
		x=dBtoG(x);
		efxvalue=int(0x7fffffff*x);
		set_dsp_register(efxTHDN, efxvalue);
		break;

	case ER_ID:
		x=-double(value)/1000*80;
		x=dBtoG(x);
		efxvalue=int(0x7fffffff*x);
		set_dsp_register(efxER, efxvalue);
		break;

	case EA_ID:
		x=-double(value)/1000.*80.;
		x=dBtoG(x);
		efxvalue=int(0x7fffffff*x);
		set_dsp_register(efxEA, efxvalue);
		break;
}

// we need do synchronize the GUI part, if set_param was called by the mixer or kX Automation
	if (cp) ((iNoiseGate2TsPluginDlg*) cp)->sync(ndx);

	return 0;
}

int iNoiseGate2TsPlugin::set_all_params(kxparam_t* values)
{
	for (int i = 0; i < NoiseGate2Ts_PARAMS_COUNT; i++) set_param(i, values[i]);
	return 0;
}

int iNoiseGate2TsPlugin::request_microcode() 
{
	publish_microcode(NoiseGate2Ts);

	return 0;
}

const char *iNoiseGate2TsPlugin::get_plugin_description(int id)
{
	plugin_description(NoiseGate2Ts);
}


iKXPluginGUI *iNoiseGate2TsPlugin::create_cp(kDialog *parent, kFile *mf)
{
	iNoiseGate2TsPluginDlg *tmp;
	tmp = new iNoiseGate2TsPluginDlg(this, parent,mf);
	tmp->create();
	return tmp;
}

int iNoiseGate2TsPlugin::get_user_interface()
{
	return IKXPLUGIN_OWNER; // means 'custom' GUI panel
}


/* Plugin UI Implementation */

void iNoiseGate2TsPluginDlg::init()
{
	CKXPluginGUI::init(); // this is necessary

	// custom dialog initialization here...
	// (title, Width, Height)
	create_dialog(plugin_name(NoiseGate2Ts), 200,164,-1,KXFX_NO_RESIZE);
		
    create_label(THRS,NoiseGate2Ts_PARAMS_COUNT+1,"Threshold",   14,136,60);
    create_label(ENV,NoiseGate2Ts_PARAMS_COUNT+1,"Envelope",		106,136,60);
// vertical fader (Fader, Ctrl_ID, Label, MinVal, MaxVal, Left, Top, Width, Height)
	create_vfader(THUP, THUP_ID, "ON", 0, 1000,     12,  12, 20, 120);
	create_vfader(THDN, THDN_ID, "OFF", 0, 1000,    42,  12, 35, 120);
	create_vfader(EA,   EA_ID,   "A",   0, 1000,    92, 12, 20, 120);
	create_vfader(HOLD, HOLD_ID, "H", 0, 1000,     122,  12, 20, 120);
	create_vfader(ER,   ER_ID,   "R",   0, 1000,   152, 12, 20, 120);

	controls_enabled = 1; // enable updating GUI when the parameters are changed

	sync(EVERYPM_ID); // update all 'GUI' elements to the preset plugin parameters
}

void iNoiseGate2TsPluginDlg::sync(int ndx)
{
	char c[MAX_LABEL_STRING];
	kxparam_t v, nv;
	double x;
	        // this function should 'synchronize' any labels with the parameters
	switch (ndx)
	{ 
	case EVERYPM_ID:
	case HOLD_ID:
		v = plugin->_params[HOLD_ID];
		x=-double(v)/1000*80;
		nv=int(x); 
		if(controls_enabled) {VSSETPOS(HOLD.slider,(int)v); HOLD.slider.redraw();}
		sprintf(c, "%d", nv);
		HOLD.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;
                // fallthru
	case THUP_ID:
		v = plugin->_params[THUP_ID];
		x=-double(v)/1000.0*80.0;
		nv=int(x); 
		if(controls_enabled) {VSSETPOS(THUP.slider,(int)v); THUP.slider.redraw();}
		sprintf(c, "%d", nv);
		THUP.svalue.SetWindowText(c);
		//
		nv = plugin->_params[THDN_ID];
		x=-(double(v)+double(nv)-double(v)*double(nv)/1000.0)/1000.0*80.0;
		nv=int(x); 
		sprintf(c, "%d", nv);
		THDN.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;
                // fallthru
	case THDN_ID:
		v = plugin->_params[THDN_ID];
		nv= plugin->_params[THUP_ID];
		x=-(double(v)+double(nv)-double(v)*double(nv)/1000)/1000*80;
		nv=int(x); 
		if(controls_enabled) {VSSETPOS(THDN.slider,(int)v); THDN.slider.redraw();}
		sprintf(c, "%d", nv);
		THDN.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;
                // fallthru
	case ER_ID:
		v = plugin->_params[ER_ID];
		x=-double(v)/1000.0*80.0;
		nv=int(x); 
		if(controls_enabled) {VSSETPOS(ER.slider,(int)v); ER.slider.redraw();}
		sprintf(c, "%d", nv);
		ER.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;
                // fallthru
	case EA_ID:
		v = plugin->_params[EA_ID];
		x=-double(v)/1000.0*80.0;
		nv=int(x); 
		if(controls_enabled) {VSSETPOS(EA.slider,(int)v); EA.slider.redraw();}
		sprintf(c, "%d", nv);
		EA.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;
                // fallthru
	}
}
