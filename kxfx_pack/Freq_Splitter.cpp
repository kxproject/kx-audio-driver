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

// Freq_Splitter 
// Copyright (c) Soeren Bovbjerg 2002

// The filter is based on RBJ-AUDIO-EQ-Cookbook http://www.musicdsp.org

#include "stdafx.h"

// --------------- the above should be a 'standard' beginning of any .cpp file

// effect class
#include "Freq_Splitter.h"
// effect source
#include "da_Freq_Splitter.cpp"

// Plugin Parameters (this identifiers also used for gui controls)
typedef enum _params_id{
	FREQ_ID,
	BAND_ID
};

// default plugin Parameters (used when plugin is loaded for the first time)
static kxparam_t Freq_Splitter_default_params[Freq_Splitter_PARAMS_COUNT] = { 1000, 10};

/* Plugin Implementation */

int iFreq_SplitterPlugin::get_param(int ndx, kxparam_t *value) // ndx corresponds to parameter id
{
	*value = _params[ndx];
	return 0;
}

int iFreq_SplitterPlugin::set_param(int ndx, kxparam_t value)
{

	_params[ndx] = value;


	double omega,sn,cs,alpha,scal,dumm;
	double eqfreq,eqband;

	double ha0, ha1, ha2, hb0, hb1, hb2, la0, la1, la2, lb0, lb1, lb2; //filter coefficients


switch (ndx)
	{
	case FREQ_ID:

	eqfreq=(double)_params[FREQ_ID];
	eqband=(double)_params[BAND_ID];
	eqfreq = int(20 * exp((log(20000.0 / 20.0)/(double)(20000 - 20))*(double)(eqfreq)));

	omega = (2.*pi)*(eqfreq/48000.);

    sn    = sin(omega);
    cs    = cos(omega);

	alpha		= sn*sinh((0.693147180559945309417/2.)*(eqband/10.)*omega/sn);

	dumm = 2.;

				ha0 =   1. + alpha; //highpass part
				scal = dumm*ha0;
				hb0 =  ((1. + cs)/2.)/scal;
                hb1 =  (-1*(1. +cs))/scal;
                hb2 =  ((1. + cs)/2.)/scal;
                ha1 =  (-2.*cs)/(scal*-1.);
                ha2 =   (1.-alpha)/(scal*-1.);

				la0 =   1. + alpha; //Lowpass part
				scal = dumm*la0;
				lb0 =  ((1. - cs)/2.)/scal;
                lb1 =  (1.-cs)/scal;
                lb2 =  ((1. - cs)/2.)/scal;
                la1 =  (-2.*cs)/(scal*-1.);
                la2 =   (1.-alpha)/(scal*-1.);

		set_dsp_register(R_HB0, _dbl_to_dspword(hb0));
		set_dsp_register(R_HB1, _dbl_to_dspword(hb1));
		set_dsp_register(R_HB2, _dbl_to_dspword(hb2));
		set_dsp_register(R_HA1, _dbl_to_dspword(ha1));
		set_dsp_register(R_HA2, _dbl_to_dspword(ha2));

		set_dsp_register(R_LB0, _dbl_to_dspword(lb0));
		set_dsp_register(R_LB1, _dbl_to_dspword(lb1));
		set_dsp_register(R_LB2, _dbl_to_dspword(lb2));
		set_dsp_register(R_LA1, _dbl_to_dspword(la1));
		set_dsp_register(R_LA2, _dbl_to_dspword(la2));

	break;

	case BAND_ID:
	eqfreq=(double)_params[FREQ_ID];
	eqband=(double)_params[BAND_ID];
	eqfreq = int(20 * exp((log(20000.0 / 20.0)/(double)(20000 - 20))*(double)(eqfreq)));
	omega = (2.*pi)*(eqfreq/48000.);

    sn    = sin(omega);
    cs    = cos(omega);

	alpha		= sn*sinh((0.693147180559945309417/2.)*(eqband/10.)*omega/sn);

				dumm = 2.;

				ha0 =   1. + alpha; //highpass part
				scal = dumm*ha0;
				hb0 =  ((1. + cs)/2.)/scal;
                hb1 =  (-1*(1. +cs))/scal;
                hb2 =  ((1. + cs)/2.)/scal;
                ha1 =  (-2.*cs)/(scal*-1.);
                ha2 =   (1.-alpha)/(scal*-1.);

				la0 =   1. + alpha; //Lowpass part
				scal = dumm*la0;
				lb0 =  ((1. - cs)/2.)/scal;
                lb1 =  (1.-cs)/scal;
                lb2 =  ((1. - cs)/2.)/scal;
                la1 =  (-2.*cs)/(scal*-1.);
                la2 =   (1.-alpha)/(scal*-1.);

		set_dsp_register(R_HB0, _dbl_to_dspword(hb0));
		set_dsp_register(R_HB1, _dbl_to_dspword(hb1));
		set_dsp_register(R_HB2, _dbl_to_dspword(hb2));
		set_dsp_register(R_HA1, _dbl_to_dspword(ha1));
		set_dsp_register(R_HA2, _dbl_to_dspword(ha2));

		set_dsp_register(R_LB0, _dbl_to_dspword(lb0));
		set_dsp_register(R_LB1, _dbl_to_dspword(lb1));
		set_dsp_register(R_LB2, _dbl_to_dspword(lb2));
		set_dsp_register(R_LA1, _dbl_to_dspword(la1));
		set_dsp_register(R_LA2, _dbl_to_dspword(la2));

	break;

	}
	if (cp) ((iFreq_SplitterPluginDlg*) cp)->sync(ndx);

	return 0;
}

int iFreq_SplitterPlugin::set_defaults()
{
	set_all_params(Freq_Splitter_default_params);
	return 0;
}

int iFreq_SplitterPlugin::set_all_params(kxparam_t* values)
{
	for (int i = 0; i < Freq_Splitter_PARAMS_COUNT; i++) set_param(i, values[i]);
	return 0;
}

int iFreq_SplitterPlugin::request_microcode()
{
	publish_microcode(Freq_Splitter);
	return 0;
}

iKXPluginGUI *iFreq_SplitterPlugin::create_cp(kDialog *parent, kFile *mf)
{
	iFreq_SplitterPluginDlg *tmp;
	tmp = new iFreq_SplitterPluginDlg(this, parent, mf);
	tmp->create();
	return tmp;
}

int iFreq_SplitterPlugin::get_user_interface()
{
	return IKXPLUGIN_OWNER;	// says to mixer that this plugin has its own control panel
}

// MIDI control implementation
static kx_fxparam_descr _param_descr[Freq_Splitter_PARAMS_COUNT] = {
	{"Frequency", KX_FXPARAM_HZ, 20, 19980},
	{"Width (Oct)", KX_FXPARAM_RATIO, 1, 100}};

int iFreq_SplitterPlugin::describe_param(int ndx, kx_fxparam_descr *descr)
{
	memcpy(descr, &_param_descr[ndx], sizeof kx_fxparam_descr); return 0;
}

// MIDI control implementation

/* Plugin UI Implementation */

void iFreq_SplitterPluginDlg::init()
{
	CKXPluginGUI::init();
	// custom dialog initialization here...

	// create_dialog(Caption, Width, Height, ExtraHeight = 25)
	create_dialog(plugin_name(Freq_Splitter), 400, 100);

	// create_hfader(Fader, Ctrl_ID, Label, MinVal, MaxVal, Left, Top, Width, Label_Width)
	create_hfader(freq, FREQ_ID, "Freq.(Hz):",20,19980, 7, 12, 368, 70);
	create_hfader(band, BAND_ID, "Width (Oct):",1, 100, 7, 48, 368, 70);

	controls_enabled = TRUE;

	sync(EVERYPM_ID);
}

void iFreq_SplitterPluginDlg::sync(int ndx)
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
		sprintf(c, "%.1f", v/10.);

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
	}
}

