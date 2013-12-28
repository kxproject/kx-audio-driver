// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov and Max Mikhailov, 2001-2006.
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
#include "MoreBass.h"
// effect source
#include "da_MoreBass.cpp"

static void dbg(const char *format, ...);

// plugin parameters
// (these are 'logical' parameters visible to the user)
typedef enum _params_id{
	VOLPASSTHRU_ID=0,
	FREQ_ID,
	BAND_ID,
	GAINBASS_ID,
	VOLBASS_ID
};

enum _extras_id{
	LBLPASSTHRU_ID=5,
	LBLBASS_ID
};

// default plugin parameters
// -------------------------
static kxparam_t default_params[MOREBASS_PARAMS_COUNT] = {1000, 4660, 100, 1, 1000};

int iMoreBassPlugin::set_defaults() 
{
	set_all_params(default_params); 
	return 0;
}

// for kX Automation: param description (see kxplugin.h for details on kx_fxparam_descr)
// -------------------------------------------------------------------------------------

static kx_fxparam_descr _param_descr[MOREBASS_PARAMS_COUNT] = {
	{"Passthru Vol",	KX_FXPARAM_USER,    0, 1000 },
	{"Frequency",		KX_FXPARAM_HZ,     20, 6670 },
	{"Width (Q)",		KX_FXPARAM_PERCENT, 1, 2000 },
	{"Bass Gain",		KX_FXPARAM_USER,    1, 10   },
	{"Bass Vol",		KX_FXPARAM_USER,    0, 1000 }
	};

int iMoreBassPlugin::describe_param(int ndx, kx_fxparam_descr *descr)
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
 //P "-6 dB Right", 0,-60,0,0,
 //P "-6 dB Left", 0x3fffffff,0,0,0,
 NULL // must be here
};

#undef P

const kxparam_t *iMoreBassPlugin::get_plugin_presets()
{
 return presets;
}


/* Plugin Implemenation */

int iMoreBassPlugin::get_param(int ndx, kxparam_t *value) // ndx corresponds to 'logical' parameter id
{
	*value = _params[ndx]; 
	return 0;
}

// set_param sets 'logical' values by writing to (one or a group of) 'physical' registers

int iMoreBassPlugin::set_param(int ndx, kxparam_t value)
{
	_params[ndx] = value;

	double omega,sn,cs,alpha,scal,dumm;
	double eqfreq,eqband;

	double a0, a1, a2, b0, b1, b2; //filter coefficients


	switch (ndx)
	{
	case VOLPASSTHRU_ID:
		set_dsp_register(VOLPASSTHRU, (dword)((dword)value*(0x7fffffff/1000)));
	break;

	case FREQ_ID:

		eqfreq=(double)_params[FREQ_ID];
		eqband=_params[BAND_ID]/100.;
		eqfreq = int(20 * exp((log(20000.0 / 20.0)/(double)(20000 - 20))*(double)(eqfreq)));
		omega = (2.*pi)*(eqfreq/48000.);

		sn    = sin(omega);
		cs    = cos(omega);


		alpha = sn/(2*eqband);

		dumm = 2.;
		a0 =   1. + alpha;
		scal = dumm*a0;
		b0 =  ((1. - cs)/2.)/scal;
        b1 =  (1.-cs)/scal;
        b2 =  ((1. - cs)/2.)/scal;
        a1 =  (-2.*cs)/(scal*-1.);
        a2 =   (1.-alpha)/(scal*-1.);

		set_dsp_register(R_B0, _dbl_to_dspword(b0));
		set_dsp_register(R_B1, _dbl_to_dspword(b1));
		set_dsp_register(R_B2, _dbl_to_dspword(b2));
		set_dsp_register(R_A1, _dbl_to_dspword(a1));
		set_dsp_register(R_A2, _dbl_to_dspword(a2));

	break;

	case BAND_ID:
		eqfreq=(double)_params[FREQ_ID];
		eqband=_params[BAND_ID]/100.;
		eqfreq = int(20 * exp((log(20000.0 / 20.0)/(double)(20000 - 20))*(double)(eqfreq)));
		omega = (2.*pi)*(eqfreq/48000.);

		sn    = sin(omega);
		cs    = cos(omega);


		alpha = sn/(2*eqband);
		dumm = 2.;
		a0 =   1. + alpha;
		scal = dumm*a0;
		b0 =  ((1. - cs)/2.)/scal;
        b1 =  (1.-cs)/scal;
        b2 =  ((1. - cs)/2.)/scal;
        a1 =  (-2.*cs)/(scal*-1.);
        a2 =   (1.-alpha)/(scal*-1.);

		set_dsp_register(R_B0, _dbl_to_dspword(b0));
		set_dsp_register(R_B1, _dbl_to_dspword(b1));
		set_dsp_register(R_B2, _dbl_to_dspword(b2));
		set_dsp_register(R_A1, _dbl_to_dspword(a1));
		set_dsp_register(R_A2, _dbl_to_dspword(a2));

	break;
	
	case GAINBASS_ID:
		set_dsp_register(GAINBASS, (dword)value);
	break;

	case VOLBASS_ID:
		set_dsp_register(VOLBASS, (dword)(value*(0x7fffffff/1000)));
	break;
	}

    // we need do synchronize the GUI part, if set_param was called by the mixer or kX Automation
	if (cp) ((iMoreBassPluginDlg*) cp)->sync(ndx);

	return 0;
}

int iMoreBassPlugin::set_all_params(kxparam_t* values)
{
	for (int i = 0; i < MOREBASS_PARAMS_COUNT; i++) set_param(i, values[i]);
	return 0;
}

int iMoreBassPlugin::request_microcode() 
{
	publish_microcode(MoreBass);

	return 0;
}

const char *iMoreBassPlugin::get_plugin_description(int id)
{
	plugin_description(MoreBass);
}


iKXPluginGUI *iMoreBassPlugin::create_cp(kDialog *parent, kFile *mf)
{
	iMoreBassPluginDlg *tmp;
	tmp = new iMoreBassPluginDlg(this, parent,mf);
	tmp->create();
	return tmp;
}

int iMoreBassPlugin::get_user_interface()
{
	return IKXPLUGIN_OWNER; // means 'custom' GUI panel
}


/* Plugin UI Implementation */

void iMoreBassPluginDlg::init()
{
	CKXPluginGUI::init(); // this is necessary

	// custom dialog initialization here...
	// (title, Width, Height)
	create_dialog(plugin_name(MoreBass), 425, 250);

	// Passthru
	create_label(lblpassthru,LBLPASSTHRU_ID,"Passthrough",10,20,100);
	
	create_hfader(volpassthru, VOLPASSTHRU_ID, "Volume:", 0x0, 1000, 10, 40, 400, 60);

	// Bass
	create_label(lblbass,LBLBASS_ID,"Bass",10,80,100);
	
	create_hfader(freq, FREQ_ID, "Freq.(Hz):",20,6670, 10, 110, 400, 60);
	create_hfader(band, BAND_ID, "Width (Q):",1, 2000, 10, 140, 400, 60);
	
	create_hfader(gainbass, GAINBASS_ID, "Gain:", 1, 10, 10, 170, 400, 60);
	create_hfader(volbass, VOLBASS_ID, "Volume:", 0x0, 1000, 10, 200, 400, 60);

	
	controls_enabled = 1; // enable updating GUI when the parameters are changed

	sync(EVERYPM_ID); // update all 'GUI' elements to the preset plugin parameters
}

void iMoreBassPluginDlg::sync(int ndx)
{
	char c[MAX_LABEL_STRING];
	kxparam_t v;
	switch (ndx)
	{
	case EVERYPM_ID: // update all controls
	case VOLPASSTHRU_ID:
		v = plugin->_params[VOLPASSTHRU_ID];
		if (controls_enabled)
		{
		 volpassthru.slider.set_pos((int)v);
		 volpassthru.slider.redraw();
		} // update fader value only if function called from mixer
		sprintf(c, "%.0f %%", (float)v/10);
		volpassthru.svalue.SetWindowText(c); // show new value for fader
		if (ndx != EVERYPM_ID) break;
	case BAND_ID: //Q
		v = plugin->_params[BAND_ID];
		if (controls_enabled)
		{
		 band.slider.set_pos((int)v);
		 band.slider.redraw();
		} // update fader value only if function called from mixer
		sprintf(c, "%.2f", v/100.);
		band.svalue.SetWindowText(c); // show new value for fader
		if (ndx != EVERYPM_ID) break;
	case FREQ_ID: // update all controls
		v = plugin->_params[FREQ_ID];
		if (controls_enabled)
		{
		 freq.slider.set_pos((int)v);
		 freq.slider.redraw();
		} // update fader value only if function called from mixer
		sprintf(c, "%d%", (int(20 * exp((log(20000.0 / 20.0)/(double)(20000 - 20))*(double)(v)))));
		freq.svalue.SetWindowText(c); // show new value for fader
		if (ndx != EVERYPM_ID) break;
	case GAINBASS_ID:
		v = plugin->_params[GAINBASS_ID];
		if (controls_enabled)
		{
		 gainbass.slider.set_pos((int)v);
		 gainbass.slider.redraw();
		} // update fader value only if function called from mixer
		sprintf(c, "%d", v);
		gainbass.svalue.SetWindowText(c); // show new value for fader
		if (ndx != EVERYPM_ID) break;
	case VOLBASS_ID:
		v = plugin->_params[VOLBASS_ID];
		if (controls_enabled)
		{
		 volbass.slider.set_pos((int)v);
		 volbass.slider.redraw();
		} // update fader value only if function called from mixer
		sprintf(c, "%.0f %%", (float)v/10);
		volbass.svalue.SetWindowText(c); // show new value for fader
		if (ndx != EVERYPM_ID) break;
	}

}

// debug messages
static void dbg(const char *format, ...)
{
	char buf[4096];
	char *p = buf;
	va_list args;
	va_start(args, format);
	p += _vsnprintf(p, sizeof buf - 1, format, args);
	va_end(args);
	while ( p > buf && isspace(p[-1]) )
	*--p = '\0';
	*p++ = '\r';
	*p++ = '\n';
	*p = '\0';
	OutputDebugString(buf);
}
