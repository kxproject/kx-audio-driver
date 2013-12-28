// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov and Max Mikhailov, 2001-2005.
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
#include "da_apscomp.cpp"

// effect class
#include "apscomp.h"

// Plugin Parameters (this identifiers also used for gui controls)
typedef enum _params_id{
	ATTACK_TIME_ID,
	RELEASE_TIME_ID,
	POST_GAIN_ID,
	THRESHOLD_ID,
	RATIO_ID,
	PREDELAY_ID
};

// default plugin Parameters (used when plugin is loaded first time)
static kxparam_t apscomp_default_params[APSCOMP_PARAMS_COUNT] = {10 , 200, 0, -20, 3, 0}; 
								//{attack, release, postgain, threshold, ratio, predelay}

static kx_fxparam_descr _param_descr[APSCOMP_PARAMS_COUNT] = {
	{"Attack Time",   KX_FXPARAM_MS, 0, 500 },
	{"Release Time", KX_FXPARAM_MS, 50, 3000 },
	{"Post Gain", KX_FXPARAM_DB, -60, 60 },
	{"Threshold", KX_FXPARAM_DB, -60,  0 },
	{"Ratio", KX_FXPARAM_PERCENT, 1, 100 },
	{"Pre-Delay", KX_FXPARAM_MS, 0, 40 }
	};

int iAPSCompPlugin::describe_param(int ndx, kx_fxparam_descr *descr)
{
 memcpy(descr, &_param_descr[ndx], sizeof kx_fxparam_descr); 
 return 0;
}

/* Plugin Implementation */

int iAPSCompPlugin::get_param(int ndx, kxparam_t *value) // ndx corresponds to parameter id
{
	*value = _params[ndx]; 
	return 0;
}

int iAPSCompPlugin::set_param(int ndx, kxparam_t value)
{
	_params[ndx] = value; // saving value into _params[] for later use

	double x;
	int gain1;
	double gain2;
	kxparam_t t;


	switch (ndx)
	{
	case ATTACK_TIME_ID:
		x = value + 0.1; //range is 0.1 to 500 ms
		x=1.0/x;
		x=0.9999992783202552+x*(-0.04795182062779634+
		x*(0.001135741853172241+x*-1.508962701294095E-05));
		set_dsp_register(_ATTACK_TIME, _dbl_to_dspword(x));
		break;
	case RELEASE_TIME_ID:
		x = (double)value; //range is 50 to 3000 ms
		x=1.0/x;
		x=0.9999999999551990+x*(-0.04797083163562625+
		x*(0.001240522577384484+x*-0.003723766234607763));
		set_dsp_register(_RELEASE_TIME, _dbl_to_dspword(x)); // writing value to level register with conversion from double to dsp's dword		
		break;
	case POST_GAIN_ID:
		x = (double)value; //range is -60 to 60 dB
		if (x>=0) //0 to 60
		{
		x=0.02346134384321507+0.9998151510352060*exp(-x/(-
		8.685689951326771));
		//gain1 = x;
		gain1 = static_cast < int > ( x ); //convert to int to truncate
		gain2 = x - gain1; //get fractional part
		set_dsp_register(_POST_GAIN_1, gain1); // writing value to level register with conversion from double to dsp's dword
		set_dsp_register(_POST_GAIN_2, _dbl_to_dspword(gain2));
		break;
		}
		x=(-7.575249529383578E-07)+0.9999997493552754*exp(-x/
		(-8.685892422025119));
		gain1 = 0;
		gain2 = x;
		set_dsp_register(_POST_GAIN_1, gain1); // writing value to level register with conversion from double to dsp's dword
		set_dsp_register(_POST_GAIN_2, _dbl_to_dspword(gain2));
		break;
	case THRESHOLD_ID:
		x = (double)value; //range is -60 to 0 dB
		x=1.000000000116500+x*0.005190512648478571;
		set_dsp_register(_THRESHOLD, _dbl_to_dspword(x)); // writing value to level register with conversion from double to dsp's dword
		break;
	case RATIO_ID:
		x = (double)value; //range is 1 to 100
		x=(-444515.6696800559+x*444516.3104954770)/
		(1.0+x*(444516.3056633568));
		set_dsp_register(_RATIO, _dbl_to_dspword(x)); // writing value to level register with conversion from double to dsp's dword
		break;
	case PREDELAY_ID:
		value = value/10;
		// reading delay start (write) address, adding predelay time to it and writing to delay read address
		// left channel predelay
		get_tram_addr(_R_PREDELAY_LW, (dword *) &t);
		set_tram_addr(_R_PREDELAY_LR, (dword)(t + 2 + value * SAMPLES_PER_MSEC));
		// right channel predelay
		get_tram_addr(_R_PREDELAY_RW, (dword *) &t);
		set_tram_addr(_R_PREDELAY_RR, (dword)(t + 2 + value * SAMPLES_PER_MSEC));
		break;
	}

	// when "control panel" (cp) for plugin is loaded, this will "synchronize" ui controls to plugin parameters
	if (cp) ((iAPSCompPluginDlg*) cp)->sync(ndx);

	return 0;
}

int iAPSCompPlugin::set_defaults() 
{
	set_all_params(apscomp_default_params); 
	return 0;
}

int iAPSCompPlugin::set_all_params(kxparam_t* values)
{
	for (int i = 0; i < APSCOMP_PARAMS_COUNT; i++) set_param(i, values[i]);
	/* you can perform parameters calculations right here (as i did in Reverb/Chorus),
	or just call set_param for every parameter (like above) */

	return 0;
}

int iAPSCompPlugin::request_microcode() 
{
	publish_microcode(apscomp);
	return 0;
}

iKXPluginGUI *iAPSCompPlugin::create_cp(kDialog *parent, kFile *mf)
{
	iAPSCompPluginDlg *tmp;
	tmp = new iAPSCompPluginDlg(this, parent, mf);
	tmp->create();
	return tmp;
}

int iAPSCompPlugin::get_user_interface()
{
	return IKXPLUGIN_OWNER;	// says to mixer that this plugin has its own control panel
}


/* Plugin UI Implementation */


void iAPSCompPluginDlg::init()
{
	CKXPluginGUI::init();
	// custom dialog initialization here...

	// create_dialog(Caption, Width, Height, ExtraHeight)
	create_dialog(plugin_name(apscomp), 300, 231); //w=275

	// create_hfader(Fader, Ctrl_ID, Label, MinVal, MaxVal, Left, Top, Width, Label_Width)
	create_hfader(fdrAttackTime, ATTACK_TIME_ID, "Attack Time (ms):",0,500, 7, 12, 283, 110); // fader must be declared with plugin dialog class
	create_hfader(fdrReleaseTime, RELEASE_TIME_ID, "Release Time (ms):",50,3000, 7, 48, 283, 110);
	create_hfader(fdrThreshold, POST_GAIN_ID, "Ouput Gain (dB):",-60,60, 7, 84, 283, 110);
	create_hfader(fdrRatio, THRESHOLD_ID, "Threshold (dB):", -60, 0, 7, 120, 283, 110);	
	create_hfader(fdrPostGain, RATIO_ID, "Ratio:", 1, 100, 7, 156, 283, 110);
	create_hfader(fdrPreDelay, PREDELAY_ID, "PreDelay (ms)", 0, 40, 7, 192, 283, 110);

	controls_enabled = TRUE;

	sync(EVERYPM_ID);
}

void iAPSCompPluginDlg::sync(int ndx)
{
	char c[MAX_LABEL_STRING];
	kxparam_t v;
	switch (ndx)
	{
	case EVERYPM_ID: // update all controls
	case ATTACK_TIME_ID:
		v = plugin->_params[ATTACK_TIME_ID];
		if (controls_enabled) {fdrAttackTime.slider.set_pos((int)v); fdrAttackTime.slider.redraw();} // update fader value only if function called from mixer
		sprintf(c, "%i", v);
		fdrAttackTime.svalue.SetWindowText(c); // show new value for fader
		if (ndx != EVERYPM_ID) break;
	case RELEASE_TIME_ID:
		v = plugin->_params[RELEASE_TIME_ID];
		if (controls_enabled) {fdrReleaseTime.slider.set_pos((int)v); fdrReleaseTime.slider.redraw();} // update fader value only if function called from mixer
		sprintf(c, "%i", v);
		fdrReleaseTime.svalue.SetWindowText(c); // show new value for fader
		if (ndx != EVERYPM_ID) break;
	case POST_GAIN_ID:
		v = plugin->_params[POST_GAIN_ID];
		if (controls_enabled) {fdrThreshold.slider.set_pos((int)v); fdrThreshold.slider.redraw();} // update fader value only if function called from mixer
		sprintf(c, "%i", v);
		fdrThreshold.svalue.SetWindowText(c); // show new value for fader
		if (ndx != EVERYPM_ID) break;
	case THRESHOLD_ID:
		v = plugin->_params[THRESHOLD_ID];
		if (controls_enabled) {fdrRatio.slider.set_pos((int)v); fdrRatio.slider.redraw();} // update fader value only if function called from mixer
		sprintf(c, "%i", v);
		fdrRatio.svalue.SetWindowText(c); // show new value for fader
		if (ndx != EVERYPM_ID) break;
	case RATIO_ID:
		v = plugin->_params[RATIO_ID];
		if (controls_enabled) {fdrPostGain.slider.set_pos((int)v); fdrPostGain.slider.redraw();} // update fader value only if function called from mixer
		sprintf(c, "%i", v);
		fdrPostGain.svalue.SetWindowText(c); // show new value for fader
		if (ndx != EVERYPM_ID) break;		
	case PREDELAY_ID:
		v = plugin->_params[PREDELAY_ID];
		if (controls_enabled) {fdrPreDelay.slider.set_pos((int)v); fdrPreDelay.slider.redraw();}
		sprintf(c, "%i.%i", v/10, v%10);;
		fdrPreDelay.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;
  }
}
