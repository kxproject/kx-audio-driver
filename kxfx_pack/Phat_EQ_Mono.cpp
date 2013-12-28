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

// Phat_EQ_Mono 
// Copyright (c) Soeren Bovbjerg 2002

// The filter is based on RBJ-AUDIO-EQ-Cookbook http://www.musicdsp.org

#include "stdafx.h"

// --------------- the above should be a 'standard' beginning of any .cpp file
// effect class
#include "Phat_EQ_Mono.h"
// effect source
#include "da_Phat_EQ_Mono.cpp"

// Plugin Parameters (this identifiers also used for gui controls)
typedef enum _params_id{
	FREQ_ID,
	BAND_ID,
	GAIN_ID,
	TYPE_ID
};

// default plugin Parameters (used when plugin is loaded for the first time)
static kxparam_t Phat_EQ_Mono_default_params[Phat_EQ_Mono_PARAMS_COUNT] = { 10000, 120, 707, 0};

/* Plugin Implementation */

int iPhat_EQ_MonoPlugin::get_param(int ndx, kxparam_t *value) // ndx corresponds to parameter id
{
	*value = _params[ndx];
	return 0;
}

int iPhat_EQ_MonoPlugin::set_param(int ndx, kxparam_t value)
{

	_params[ndx] = value;


	double omega,sn,cs,alpha,scal,dumm;
	double eqgain,eqband;
	int eqfreq;

	double a0, a1, a2, b0, b1, b2; //filter coefficients
	a0=a1=a2=b0=b1=b2=0.0;


switch (ndx)
	{

	case TYPE_ID:
		eqfreq=(int)_params[FREQ_ID];
		set_param(FREQ_ID, eqfreq);
	break;

	case GAIN_ID:
		eqgain=(double)(_params[GAIN_ID]);

		set_dsp_register(R_VOL, _dbl_to_dspword(eqgain/1000.));
	break;

	case FREQ_ID:

	eqfreq=(int)_params[FREQ_ID];
	eqband=_params[BAND_ID]/100.;

	eqfreq = int(20 * exp((log(20000.0 / 20.0)/(double)(20000 - 20))*(double)(eqfreq)));
	omega = (2.*pi)*(eqfreq/48000.);

    sn    = sin(omega);
    cs    = cos(omega);


	alpha = sn/(2*eqband);
	dumm = 2.;

		if	(_params[TYPE_ID]==0)
		{
				a0 =   1. + alpha;//LP
				scal = dumm*a0;
				b0 =  ((1. - cs)/2.)/scal;
                b1 =  (1.-cs)/scal;
                b2 =  ((1. - cs)/2.)/scal;
                a1 =  (-2.*cs)/(scal*-1.);
                a2 =   (1.-alpha)/(scal*-1.);
		}

		if	(_params[TYPE_ID]==3)
		{
				a0 =   1. + alpha;//HP
				scal = dumm*a0;
				b0 =  ((1. + cs)/2.)/scal;
                b1 =  (-1*(1. +cs))/scal;
                b2 =  ((1. + cs)/2.)/scal;
                a1 =  (-2.*cs)/(scal*-1.);
                a2 =   (1.-alpha)/(scal*-1.);
		}

		if	(_params[TYPE_ID]==1)
		{
				a0 =   1. + alpha;//BP
				scal = dumm*a0;
				b0 =  alpha/scal;
				b1 =  0;
                b2 =  (-1. * alpha)/scal;
                a1 =  (-2.*cs)/(scal*-1.);
		        a2 =   (1.-alpha)/(scal*-1.);
		}


		if	(_params[TYPE_ID]==2)
		{
				a0 =   1. + alpha;//Notch
				scal = dumm*a0;
				b0 =  1/scal;
				b1 =  (-2.*cs)/scal;
                b2 =  1/scal;
				a1 =  (-2.*cs)/(scal*-1.);
                a2 =   (1.-alpha)/(scal*-1.);
		}

		set_dsp_register(R_B0, _dbl_to_dspword(b0));
		set_dsp_register(R_B1, _dbl_to_dspword(b1));
		set_dsp_register(R_B2, _dbl_to_dspword(b2));
		set_dsp_register(R_A1, _dbl_to_dspword(a1));
		set_dsp_register(R_A2, _dbl_to_dspword(a2));

	break;

	case BAND_ID:
	eqfreq=(int)_params[FREQ_ID];
	eqband=_params[BAND_ID]/100.;

	eqfreq = int(20 * exp((log(20000.0 / 20.0)/(double)(20000 - 20))*(double)(eqfreq)));

	omega = (2.*pi)*(eqfreq/48000.);

    sn    = sin(omega);
    cs    = cos(omega);


	alpha = sn/(2*eqband);
dumm = 2.;

		if	(_params[TYPE_ID]==0)
		{
				a0 =   1. + alpha;//LP
				scal = dumm*a0;
				b0 =  ((1. - cs)/2.)/scal;
                b1 =  (1.-cs)/scal;
                b2 =  ((1. - cs)/2.)/scal;
                a1 =  (-2.*cs)/(scal*-1.);
                a2 =   (1.-alpha)/(scal*-1.);
		}

		if	(_params[TYPE_ID]==3)
		{
				a0 =   1. + alpha;//HP
				scal = dumm*a0;
				b0 =  ((1. + cs)/2.)/scal;
                b1 =  (-1*(1. +cs))/scal;
                b2 =  ((1. + cs)/2.)/scal;
                a1 =  (-2.*cs)/(scal*-1.);
                a2 =   (1.-alpha)/(scal*-1.);
		}

		if	(_params[TYPE_ID]==1)
		{
				a0 =   1. + alpha;//BP
				scal = dumm*a0;
				b0 =  alpha/scal;
				b1 =  0;
                b2 =  (-1. * alpha)/scal;
                a1 =  (-2.*cs)/(scal*-1.);
		        a2 =   (1.-alpha)/(scal*-1.);
		}


		if	(_params[TYPE_ID]==2)
		{
				a0 =   1. + alpha;//Notch
				scal = dumm*a0;
				b0 =  1/scal;
				b1 =  (-2.*cs)/scal;
                b2 =  1/scal;
				a1 =  (-2.*cs)/(scal*-1.);
                a2 =   (1.-alpha)/(scal*-1.);
		}

		set_dsp_register(R_B0, _dbl_to_dspword(b0));
		set_dsp_register(R_B1, _dbl_to_dspword(b1));
		set_dsp_register(R_B2, _dbl_to_dspword(b2));
		set_dsp_register(R_A1, _dbl_to_dspword(a1));
		set_dsp_register(R_A2, _dbl_to_dspword(a2));

	break;

	}
	if (cp) ((iPhat_EQ_MonoPluginDlg*) cp)->sync(ndx);

	return 0;
}

int iPhat_EQ_MonoPlugin::set_defaults()
{
	set_all_params(Phat_EQ_Mono_default_params);
	return 0;
}

int iPhat_EQ_MonoPlugin::set_all_params(kxparam_t* values)
{
	for (int i = 0; i < Phat_EQ_Mono_PARAMS_COUNT; i++) set_param(i, values[i]);
	return 0;
}

int iPhat_EQ_MonoPlugin::request_microcode()
{
	publish_microcode(Phat_EQ_Mono);
	return 0;
}

iKXPluginGUI *iPhat_EQ_MonoPlugin::create_cp(kDialog *parent, kFile *mf)
{
	iPhat_EQ_MonoPluginDlg *tmp;
	tmp = new iPhat_EQ_MonoPluginDlg(this, parent, mf);
	tmp->create();
	return tmp;
}

int iPhat_EQ_MonoPlugin::get_user_interface()
{
	return IKXPLUGIN_OWNER;	// says to mixer that this plugin has its own control panel
}

// MIDI control implementation
static kx_fxparam_descr _param_descr[Phat_EQ_Mono_PARAMS_COUNT] = {
	{"Frequency", KX_FXPARAM_HZ, 20, 19980},
	{"Width (Q)", KX_FXPARAM_RATIO, 5, 2000},
	{"Input Gain (dB)", KX_FXPARAM_USER, 1, 1000},
	{"Type (LP/BP/NO/HP)", KX_FXPARAM_USER, 0, 3}};

int iPhat_EQ_MonoPlugin::describe_param(int ndx, kx_fxparam_descr *descr)
{
	memcpy(descr, &_param_descr[ndx], sizeof kx_fxparam_descr); return 0;
}

// MIDI control implementation

/* Plugin UI Implementation */

void iPhat_EQ_MonoPluginDlg::init()
{
	CKXPluginGUI::init();
	// custom dialog initialization here...

	// create_dialog(Caption, Width, Height, ExtraHeight = 25)
	create_dialog(plugin_name(Phat_EQ_Mono), 420, 164);

	// create_hfader(Fader, Ctrl_ID, Label, MinVal, MaxVal, Left, Top, Width, Label_Width)
	create_hfader(freq, FREQ_ID, "Freq.(Hz):   ",20,19980, 7, 12, 393, 75);
	create_hfader(band, BAND_ID, "Width (Q):",5, 2000, 7, 48, 393, 75);
	create_hfader(gain, GAIN_ID, "In.Gain (dB):",1, 1000, 7, 84, 393, 75); //SB
	create_hfader(type, TYPE_ID, "Type (LP/BP/NO/HP):",0, 3, 7, 120, 250, 120); //SB

	controls_enabled = TRUE;

	sync(EVERYPM_ID);
}

void iPhat_EQ_MonoPluginDlg::sync(int ndx)
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
		sprintf(c, "%.2f", v/50.);

		band.svalue.SetWindowText(c); // show new value for fader
		if (ndx != EVERYPM_ID) break;
	case FREQ_ID: // update all controls
		v = plugin->_params[FREQ_ID];
		if (controls_enabled)
		{
		 freq.slider.set_pos((int)v);
		 freq.slider.redraw();
		} // update fader value only if function called from mixer
		sprintf(c, "%d", (int(20 * exp((log(20000.0 / 20.0)/(double)(20000 - 20))*(double)(v)))));
		freq.svalue.SetWindowText(c); // show new value for fader
		if (ndx != EVERYPM_ID) break;
	case GAIN_ID: // update all controls

		v = plugin->_params[GAIN_ID];
		if (controls_enabled)
		{
		 gain.slider.set_pos((int)v);
		 gain.slider.redraw();
		} // update fader value only if function called from mixer
		sprintf(c, "%.2f", 8.6858896381*log(v/1000.));
		gain.svalue.SetWindowText(c); // show new value for fader
		if (ndx != EVERYPM_ID) break;
	case TYPE_ID: // update all controls
		v = plugin->_params[TYPE_ID];
		if (controls_enabled)
		{
		 type.slider.set_pos((int)v);
		 type.slider.redraw();
		} // update fader value only if function called from mixer

		if (v==0)
		{
			sprintf (c, " LP");
			type.svalue.SetWindowText(c);
		}
		if (v==1)
		{
			sprintf (c, " BP");
			type.svalue.SetWindowText(c);
		}
		if (v==2)
		{
			sprintf (c, " NO");
			type.svalue.SetWindowText(c);
		}
		if (v==3)
		{
			sprintf (c, " HP");
			type.svalue.SetWindowText(c);
		}
			// show new value for fader
		if (ndx != EVERYPM_ID) break;
	}
}



