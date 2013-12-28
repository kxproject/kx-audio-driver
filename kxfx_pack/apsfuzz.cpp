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
#include "apsfuzz.h"
// effect source
#include "da_apsfuzz.cpp"

// Plugin Parameters (this identifiers also used for gui controls)
typedef enum _params_id{
	EQ_CENTER_ID,	// 5 parameters in plugin, see below
	EQ_BANDWIDTH_ID,
	GAIN_ID,
	PRE_LP_CUTOFF_ID,
	EDGE_ID
};

/*
#define _EQ_CENTER			0x8002
#define _EQ_BANDWIDTH		0x8003
#define _GAIN				0x8004
#define _PRE_LP_CUTOFF_1	0x8005
#define _PRE_LP_CUTOFF_2	0x8006
#define _EDGE				0x8007
*/


// default plugin Parameters (used when plugin is loaded first time)
static kxparam_t apsfuzz_default_params[APSFUZZ_PARAMS_COUNT] = {3600 , 3600, -26, 8000, 20};

static kx_fxparam_descr _param_descr[APSFUZZ_PARAMS_COUNT] = {
	{"EQ Center",   KX_FXPARAM_HZ, 80, 24000},
	{"EQ Bandwidth", KX_FXPARAM_HZ, 80, 24000},
	{"Gain", KX_FXPARAM_DB, -60, 0 },
	{"Pre LP Cutoff", KX_FXPARAM_HZ, 80, 24000 },
	{"Edge", KX_FXPARAM_RATIO, 0, 100  }
	};

int iAPSFuzzPlugin::describe_param(int ndx, kx_fxparam_descr *descr)
{
 memcpy(descr, &_param_descr[ndx], sizeof kx_fxparam_descr); 
 return 0;
}


/* Plugin Implementation */

int iAPSFuzzPlugin::get_param(int ndx, kxparam_t *value) // ndx corresponds to parameter id
{
	*value = _params[ndx]; 
	return 0;
}

double gainfac0 = 1; //default gain factor GAIN
double gainfac1 = 1; //default gain factor EQ_CENTER
double gainfac2 = 1; //default gain factor EQ_BANDWIDTH
double gainfac3 = 1; //default gain factor EDGE

int iAPSFuzzPlugin::set_param(int ndx, kxparam_t value)
{
	_params[ndx] = value; // saving value into _params[] for later use

	double x;
	double defaultgain = 0.017979653095; //default gain

	switch (ndx)
	{
	case EQ_CENTER_ID: //double param
		x = (double)value; //range is 80 to 24000 Hz
		x=-0.9999996087868674+x*(-4.889784797739696E-09+
		x*(8.572346969664452E-09+x*(-1.996903248462239E-15+
		x*(-1.182444103683059E-17+x*(-4.734665105096328E-23+
		x*(1.018330728577132E-26+x*-1.212133769172390E-31))))));
		set_dsp_register(_EQ_CENTER, _dbl_to_dspword(x));
		x = (double)value; //range is 80 to 24000 Hz
		x =1.699301032030485E-07+x*(5.182045005173811E-06+
		x*(2.764141589086379E-12+x*(-1.612896897411086E-14+
		x*(3.321583980491456E-19+x*(-3.605837395528725E-23+
		x*(4.399439430036868E-27+x*(-2.526845257646646E-31+
		x*(8.461903396116215E-36+x*(-1.595980625902211E-40+
		x*1.297843086174530E-45)))))))));
		gainfac1 = x/defaultgain;
		x = defaultgain * gainfac0 * gainfac1 * gainfac2 * gainfac3;
		set_dsp_register(_GAIN, _dbl_to_dspword(x)); 
		break;
	case EQ_BANDWIDTH_ID: //single param
		x = (double)value; //range is 80 to 24000 Hz
		x=(0.9999971480978572+x*(-7.599754766413978E-05+
		x*(-9.187287764154778E-10+x*2.561863999091350E-14)))/
		(1.0+x*(5.488738787122047E-05+x*(-2.290663425972764E-09+
		x*7.994332419413221E-17)));
		set_dsp_register(_EQ_BANDWIDTH, _dbl_to_dspword(x)); // writing value to level register with conversion from double to dsp's dword
		x = (double)value; //range is 80 to 24000 Hz
		x=(0.001755747932309962+x*(2.578414757554362E-05+
		x*(1.358925816726522E-08+x*(-1.229089567268020E-12+
		x*2.574913909356508E-17))))/
		(1.0+x*(0.002375808191869344+x*(3.116906585534323E-07+
		x*(-3.442958514009344E-11+x*(7.170108682532983E-16+
		x*1.428735923746634E-22)))));
		gainfac2 = x/defaultgain;
		x = defaultgain * gainfac0 * gainfac1 * gainfac2 * gainfac3;
		set_dsp_register(_GAIN, _dbl_to_dspword(x)); 		
		break;
	case GAIN_ID:
		x = (double)value; //range is -60 to 0
		x = (0.3587592723531544+x*(0.01273786618116914+
		x*(0.0001598921656996944+x*7.060883841260972E-07)))/
		(1.0+x*(-0.07936311462039212+x*(0.003074520589084146+
		x*(-5.809010086644333E-05+x*1.515345713846910E-06))));
		gainfac0 = x/defaultgain;
		x = defaultgain * gainfac0 * gainfac1 * gainfac2 * gainfac3;
		set_dsp_register(_GAIN, _dbl_to_dspword(x)); // writing value to level register with conversion from double to dsp's dword
		break;
	case PRE_LP_CUTOFF_ID:	//double param
		x = (double)value; //range is 80 to 24000 Hz
		x=(6.367742453517444E-07+x*(5.853557859626613E-05+
		x*(4.795995919883891E-10+x*-3.649870513640977E-14)))/
		(1.0+x*(8.141117204386345E-06+x*(1.029611172007810E-10+
		x*4.392806041489011E-15)));
		set_dsp_register(_PRE_LP_CUTOFF_1, _dbl_to_dspword(x)); // writing value to level register with conversion from double to dsp's dword
		x = (double)value; //range is 80 to 24000 Hz
		x=(0.9999999352321886+x*(-6.500392344679056E-05+
		x*(-8.254106771569408E-11+x*(8.170541729490515E-14+
		x*-1.399304674834949E-18))))/
		(1.0+x*(-6.500479981397641E-05+x*(1.631910920798897E-09+
		x*(-3.011088422595398E-14+x*(5.167040302426033E-19+
		x*-7.837140053861223E-25)))));
		set_dsp_register(_PRE_LP_CUTOFF_2, _dbl_to_dspword(x)); // writing value to level register with conversion from double to dsp's dword
		break;

	case EDGE_ID:
		x = (double)value; //range is 0 to 100
		x=7.906728850124174E-10+1.172341514867014E-10*
		pow(x,1.032998713689527);
		set_dsp_register(_EDGE, _dbl_to_dspword(x)); // writing value to level register with conversion from double to dsp's dword
		x = (double)value; //range is 0 to 100
		if (x<4) //0 to 3
			x = 0.026969956712;
		if (x>3 && x<7) //4 to 6
			x = 0.017980606304;
		if (x>6 && x<11) //7 to 10
			x = 0.031465108055;
		if (x>10 && x<14) //11 to 13
			x = 0.025172849197;
		if (x>13 && x<18) //14 to 17
			x = 0.02103730944;
		if (x>17 && x<21) //18 to 20
			x = 0.017980606304;
		if (x>20 && x<25) //21 to 24
			x = 0.033802587089;		
		if (x>24 && x<50) //21 to 49 starting at 25
		{	x = x - 21; //range 0 to 28
			x = x / 28; //range 0 to 1
			x = x * 50; //range 0 to 50
			x = x + 50; //range 50 to 100
			x=(-8.553990707862978+x*(0.05372813938725340+
			x*7.734763023753810E-05))/
			(1.0+x*(-5.198404793476452+x*0.03849617078645805));
		}
		if (x>49) //50 to 100
		{	x=(-8.553990707862978+x*(0.05372813938725340+
			x*7.734763023753810E-05))/
			(1.0+x*(-5.198404793476452+x*0.03849617078645805));
		}
		gainfac3 = x/defaultgain;
		x = defaultgain * gainfac0 * gainfac1 * gainfac2 * gainfac3;
		set_dsp_register(_GAIN, _dbl_to_dspword(x)); // writing value to level register with conversion from double to dsp's dword
		break;
	}

	// when "control panel" (cp) for plugin is loaded, this will "synchronize" ui controls to plugin parameters
	if (cp) ((iAPSFuzzPluginDlg*) cp)->sync(ndx);

	return 0;
}

int iAPSFuzzPlugin::set_defaults() 
{
	set_all_params(apsfuzz_default_params); 
	return 0;
}

int iAPSFuzzPlugin::set_all_params(kxparam_t* values)
{
	for (int i = 0; i < APSFUZZ_PARAMS_COUNT; i++) set_param(i, values[i]);

	return 0;
}

int iAPSFuzzPlugin::request_microcode() 
{
	publish_microcode(apsfuzz);
	return 0;
}

iKXPluginGUI *iAPSFuzzPlugin::create_cp(kDialog *parent, kFile *mf)
{
	iAPSFuzzPluginDlg *tmp;
	tmp = new iAPSFuzzPluginDlg(this, parent, mf);
	tmp->create();
	return tmp;
}

int iAPSFuzzPlugin::get_user_interface()
{
	return IKXPLUGIN_OWNER;	// says to mixer that this plugin has its own control panel
}


/* Plugin UI Implementation */

void iAPSFuzzPluginDlg::init()
{
	CKXPluginGUI::init();
	// custom dialog initialization here...

	// create_dialog(Caption, Width, Height, ExtraHeight = 0)
	create_dialog(plugin_name(apsfuzz), 300, 195); //w=275

	// create_hfader(Fader, Ctrl_ID, Label, MinVal, MaxVal, Left, Top, Width, Label_Width)
	create_hfader(fdrEQCenter, EQ_CENTER_ID, "EQ Center (Hz):",80,24000, 7, 12, 283, 110); // fader must be declared with plugin dialog class
	create_hfader(fdrEQBandwidth, EQ_BANDWIDTH_ID, "EQ Bandwidth (Hz):",80,24000, 7, 48, 283, 110);
	create_hfader(fdrGain, GAIN_ID, "Gain (dB):",-60,0, 7, 84, 283, 110);
	create_hfader(fdrPreLPCutoff, PRE_LP_CUTOFF_ID, "Pre LP Cutoff (Hz):", 80, 24000, 7, 120, 283, 110);
	create_hfader(fdrEdge, EDGE_ID, "Edge:", 0, 100, 7, 156, 283, 110);

	controls_enabled = TRUE;

	sync(EVERYPM_ID);
}

void iAPSFuzzPluginDlg::sync(int ndx)
{
	char c[MAX_LABEL_STRING];
	kxparam_t v;
	switch (ndx)
	{
	case EVERYPM_ID: // update all controls
	case EQ_CENTER_ID:
		v = plugin->_params[EQ_CENTER_ID];
		if (controls_enabled) {fdrEQCenter.slider.set_pos((int)v); fdrEQCenter.slider.redraw();} // update fader value only if function called from mixer
		sprintf(c, "%i", v);
		fdrEQCenter.svalue.SetWindowText(c); // show new value for fader
		if (ndx != EVERYPM_ID) break;
	case EQ_BANDWIDTH_ID:
		v = plugin->_params[EQ_BANDWIDTH_ID];
		if (controls_enabled) {fdrEQBandwidth.slider.set_pos((int)v); fdrEQBandwidth.slider.redraw();} // update fader value only if function called from mixer
		sprintf(c, "%i", v);
		fdrEQBandwidth.svalue.SetWindowText(c); // show new value for fader
		if (ndx != EVERYPM_ID) break;
	case GAIN_ID:
		v = plugin->_params[GAIN_ID];
		if (controls_enabled) {fdrGain.slider.set_pos((int)v); fdrGain.slider.redraw();} // update fader value only if function called from mixer
		sprintf(c, "%i", v);
		fdrGain.svalue.SetWindowText(c); // show new value for fader
		if (ndx != EVERYPM_ID) break;
	case PRE_LP_CUTOFF_ID:
		v = plugin->_params[PRE_LP_CUTOFF_ID];
		if (controls_enabled) {fdrPreLPCutoff.slider.set_pos((int)v); fdrPreLPCutoff.slider.redraw();} // update fader value only if function called from mixer
		sprintf(c, "%i", v);
		fdrPreLPCutoff.svalue.SetWindowText(c); // show new value for fader
		if (ndx != EVERYPM_ID) break;
	case EDGE_ID:
		v = plugin->_params[EDGE_ID];
		if (controls_enabled) {fdrEdge.slider.set_pos((int)v); fdrEdge.slider.redraw();} // update fader value only if function called from mixer
		sprintf(c, "%i", v);
		fdrEdge.svalue.SetWindowText(c); // show new value for fader
		if (ndx != EVERYPM_ID) break;
  }
}
