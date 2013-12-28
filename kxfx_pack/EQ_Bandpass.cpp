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

// EQ_Bandpass
// (c) Soeren Bovbjerg, 2002-2004
// The filter is based on RBJ-AUDIO-EQ-Cookbook http://www.musicdsp.org

#include "stdafx.h"

// --------------- the above should be a 'standard' beginning of any .cpp file

// effect class
#include "EQ_Bandpass.h"
// effect source
#include "da_EQ_Bandpass.cpp"

// Plugin Parameters (this identifiers also used for gui controls)
typedef enum _params_id{
	FREQ_ID,
	BAND_ID
};

// default plugin Parameters (used when plugin is loaded for the first time)
static kxparam_t EQ_Bandpass_default_params[EQ_Bandpass_PARAMS_COUNT] = { 10000, 120};

// MIDI control implementation
static kx_fxparam_descr _param_descr[EQ_Bandpass_PARAMS_COUNT] = {
	{"Frequency", KX_FXPARAM_HZ, 20, 19980},
	{"Width (Q)", KX_FXPARAM_PERCENT, 5, 2000}};

int iEQ_BandpassPlugin::describe_param(int ndx, kx_fxparam_descr *descr)
{
	memcpy(descr, &_param_descr[ndx], sizeof kx_fxparam_descr); return 0;
}

// Plugin Implementation

int iEQ_BandpassPlugin::get_param(int ndx, kxparam_t *value) // ndx corresponds to parameter id
{
	*value = _params[ndx];
	return 0;
}

int iEQ_BandpassPlugin::set_param(int ndx, kxparam_t value)
{

	_params[ndx] = value;


	double omega,sn,cs,alpha,scal,dumm;
	double eqfreq,eqband;

	double a0, a1, a2, b0, b1, b2; //filter coefficients



switch (ndx)
	{
	case FREQ_ID:

	eqfreq=(double)_params[FREQ_ID];
	eqband=_params[BAND_ID]/100.;
	eqfreq = int(20 * exp((log(20000.0 / 20.0)/(double)(20000 - 20))*(double)(eqfreq)));

/*		if ((24000./(eqfreq*2.))<=(eqband/10.))
		eqband=(240000./(eqfreq*2.));*/

	omega = (2.*pi)*(eqfreq/48000.);
    sn    = sin(omega);
    cs    = cos(omega);
 //	alpha		= sn*sinh((0.693147180559945309417/2.)*(eqband/10.)*omega/sn);
	alpha = sn/(2*eqband);
	dumm = 2.;
				a0 =   1. + alpha;
				scal = dumm*a0;
				b0 =  alpha/scal;
				b1 =  0;
                b2 =  (-1. * alpha)/scal;
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

	/**	if ((24000./(eqfreq*2.))<=(eqband/10.))
		eqband=(240000./(eqfreq*2.));*/

	omega = (2.*pi)*(eqfreq/48000.);

    sn    = sin(omega);
    cs    = cos(omega);

//	alpha		= sn*sinh((0.693147180559945309417/2.)*(eqband/10.)*omega/sn);
	alpha = sn/(2*eqband);
	dumm = 2.;
				a0 =   1. + alpha;
				scal = dumm*a0;
				b0 =  alpha/scal;
				b1 =  0;
                b2 =  (-1. * alpha)/scal;
                a1 =  (-2.*cs)/(scal*-1.);
                a2 =   (1.-alpha)/(scal*-1.);
		set_dsp_register(R_B0, _dbl_to_dspword(b0));
		set_dsp_register(R_B1, _dbl_to_dspword(b1));
		set_dsp_register(R_B2, _dbl_to_dspword(b2));
		set_dsp_register(R_A1, _dbl_to_dspword(a1));
		set_dsp_register(R_A2, _dbl_to_dspword(a2));

	break;

	}
	if (cp) ((iEQ_BandpassPluginDlg*) cp)->sync(ndx);

	return 0;
}

int iEQ_BandpassPlugin::set_defaults()
{
	set_all_params(EQ_Bandpass_default_params);
	return 0;
}

int iEQ_BandpassPlugin::set_all_params(kxparam_t* values)
{
	for (int i = 0; i < EQ_Bandpass_PARAMS_COUNT; i++) set_param(i, values[i]);
	return 0;
}

int iEQ_BandpassPlugin::request_microcode()
{
	publish_microcode(EQ_Bandpass);
	return 0;
}

iKXPluginGUI *iEQ_BandpassPlugin::create_cp(kDialog *parent, kFile *mf)
{
	iEQ_BandpassPluginDlg *tmp;
	tmp = new iEQ_BandpassPluginDlg(this, parent, mf);
	tmp->create();
	return tmp;
}

int iEQ_BandpassPlugin::get_user_interface()
{
	return IKXPLUGIN_OWNER;	// says to mixer that this plugin has its own control panel
}


/* Plugin UI Implementation */

void iEQ_BandpassPluginDlg::init()
{
	CKXPluginGUI::init();
	// custom dialog initialization here...

	// create_dialog(Caption, Width, Height, ExtraHeight = 25)
	create_dialog(plugin_name(EQ_Bandpass), 380, 100);

	// create_hfader(Fader, Ctrl_ID, Label, MinVal, MaxVal, Left, Top, Width, Label_Width)
	create_hfader(freq, FREQ_ID, "Freq.(Hz):",20,19980, 7, 12, 363, 65);
	create_hfader(band, BAND_ID, "Width (Q):",5, 2000, 7, 48, 363, 65);

	controls_enabled = TRUE;

	sync(EVERYPM_ID);
}

void iEQ_BandpassPluginDlg::sync(int ndx)
{
	char c[MAX_LABEL_STRING];
	kxparam_t v;
	switch (ndx)
	{
	case EVERYPM_ID: // update all controls
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
	//	sprintf(c, "%d%", (int)((float)v));
		freq.svalue.SetWindowText(c); // show new value for fader
		if (ndx != EVERYPM_ID) break;
	}
}
