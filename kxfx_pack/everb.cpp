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

// Everb
// (c) Hanz Petrov, 2002-2004

#include "stdafx.h"

// --------------- the above should be a 'standard' beginning of any .cpp file

// effect class
#include "everb.h"
// effect source
#include "da_everb.cpp"

// Plugin Parameters (this identifiers also used for gui controls)
typedef enum _params_id{
	GAIN_ID,
	LEVEL_ID,
	DECAY_ID,
	DIFFUSION_ID,
	EARLY_REFL_ID,
	LATE_DELAY_ID,
	HIGH_FREQ_ID,
	LOW_FREQ_ID,
};

// default plugin Parameters (used when plugin is loaded first time)
static kxparam_t everb_default_params[EVERB_PARAMS_COUNT] = {0, 50, 8, 100, 50, 50, 0, 0}; 
							//{gain, level, decay_time, diffusion, early_refl, late_delay, high_freq, low_freq}

static kx_fxparam_descr _param_descr[EVERB_PARAMS_COUNT] = {
	{"Gain",   KX_FXPARAM_LEVEL, -1, 1},
	{"Level", KX_FXPARAM_PERCENT, 0, 100},
	{"Decay Time", KX_FXPARAM_MS, 1,30 },
	{"Diffusion", KX_FXPARAM_PERCENT, 0, 100 },
	{"Early Refl", KX_FXPARAM_PERCENT,0 ,100  },
	{"Late Delay", KX_FXPARAM_MS, 1, 350 },
	{"High Freq", KX_FXPARAM_USER, -10, 3 },
	{"Low Freq", KX_FXPARAM_USER, -10, 3 }
	};

int iEverbPlugin::describe_param(int ndx, kx_fxparam_descr *descr)
{
 memcpy(descr, &_param_descr[ndx], sizeof kx_fxparam_descr); 
 return 0;
}


/* Plugin Implementation */

int iEverbPlugin::get_param(int ndx, kxparam_t *value) // ndx corresponds to parameter id
{
	*value = _params[ndx]; 
	return 0;
}

int iEverbPlugin::set_param(int ndx, kxparam_t value)
{
	kxparam_t t;
	_params[ndx] = value; // saving value into _params[] for later use

	double x;
	double y;

	switch (ndx)
	{
	case GAIN_ID:	//ok for everb
		value=value+2;//range is -1 to 1 (where 2 is zero gain)
		set_dsp_register(_GAIN, (dword)value);
		break;
	case LEVEL_ID:	//ok everb
		x = (double)value; //range is 0 to 100%
		x=x/100.0;
		set_dsp_register(_LEVEL, _dbl_to_dspword(x)); // writing value to level register with conversion from double to dsp's dword		
		break;
	case DECAY_ID:	//ok everb
		x = (double)value;	//range is 1.5 to 30
		if (x<1.5)	//round off if below 1.5 (1.5 is working minimum)
		{
		x=1.5;	
		}
		x=(0.06112158043010938+x*(0.05839571946898384+
		x*(-0.06251223415316797+x*-0.002455634475547180)))/
		(1.0+x*(-0.7203877574056393+x*(-0.1428834838592317+
		x*-0.0009789250744797674)));
		set_dsp_register(_DECAY, _dbl_to_dspword(x));
		break;
	case DIFFUSION_ID:	//ok everb
		x = (double)value; //range is -60 to 0 dB
		x=-1.701573478439511E-10+x*0.006180339998905423;
		set_dsp_register(_DIFFUSION, _dbl_to_dspword(x)); // writing value to level register with conversion from double to dsp's dword
		break;
	case EARLY_REFL_ID:	//fixme - need more points
		x = (double)value; //range is 1 to 100
		//x=.000122069847;
		//x=-1.903181818182193E-10+x*2.441404977272728E-06;
		x=x/100;
		set_dsp_register(_EARLY_REFL, _dbl_to_dspword(x)); // writing value to level register with conversion from double to dsp's dword		
		break;
	case LATE_DELAY_ID:	//1 to 350 default is 50ms [offset by 5 samples]
		//value = value/10;
		// reading delay start (write) address, adding predelay time to it and writing to delay read address
		// left channel predelay
		get_tram_addr(_R_LATE_DELAY_LW, (dword *) &t);
		set_tram_addr(_R_LATE_DELAY_LR, (dword)(t + 5 + value * SAMPLES_PER_MSEC));
		// right channel predelay
		get_tram_addr(_R_LATE_DELAY_RW, (dword *) &t);
		set_tram_addr(_R_LATE_DELAY_RR, (dword)(t + 5 + value * SAMPLES_PER_MSEC));
		break;
  case HIGH_FREQ_ID:	//ok
		x = (double)value; //range is -10 to 3
		x=(-0.2500000002898438)+0.2280027098305318*exp(-x/(-4.342944817911659));
		set_dsp_register(_HIGH_FREQ_1, _dbl_to_dspword(x)); // writing value to level register with conversion from double to dsp's dword
		x=x+0.5;
		set_dsp_register(_HIGH_FREQ_2, _dbl_to_dspword(x)); // writing value to level register with conversion from double to dsp's dword
		break;
	case LOW_FREQ_ID:	//~fixme
		x = (double)value; //range is -10 to 3
		y=(-2.255169311636232E-06+x*(0.02821080952305102+
		x*(-0.009934131478222556+x*(0.008243423114987984+
		x*-0.0006937230047626648))))/
		(1.0+x*(-0.3659674415704222+x*(0.3284272427418508+
		x*(-0.03560791957057196+x*0.002315101477348684))));
		set_dsp_register(_LOW_FREQ_1, _dbl_to_dspword(y)); // writing value to level register with conversion from double to dsp's dword
		if (x<=0) //-10 to 0
		{
		x=y+0.50;
		set_dsp_register(_LOW_FREQ_2, _dbl_to_dspword(x)); // writing value to level register with conversion from double to dsp's dword
		break;
		}  // 0 to three is different!
		//x=0.4994177093724000+x*-0.01550764130209999;
		x=0.4999985916771500+x*(-0.01725028821634997+
		x*0.0005808823047499939);
		set_dsp_register(_LOW_FREQ_2, _dbl_to_dspword(x)); // writing value to level register with conversion from double to dsp's dword
		break;

	}

	// when "control panel" (cp) for plugin is loaded, this will "synchronize" ui controls to plugin parameters
	if (cp) ((iEverbPluginDlg*) cp)->sync(ndx);

	return 0;
}

int iEverbPlugin::set_defaults() 
{
	set_all_params(everb_default_params); 
	return 0;
}

int iEverbPlugin::set_all_params(kxparam_t* values)
{
	for (int i = 0; i < EVERB_PARAMS_COUNT; i++) set_param(i, values[i]);
	/* you can perform parameters calculations right here (as i did in Reverb/Chorus),
	or just call set_param for every parameter (like above) */

	return 0;
}

int iEverbPlugin::request_microcode() 
{
	publish_microcode(everb);
	return 0;
}

iKXPluginGUI *iEverbPlugin::create_cp(kDialog *parent, kFile *mf)
{
	iEverbPluginDlg *tmp;
	tmp = new iEverbPluginDlg(this, parent, mf);
	tmp->create();
	return tmp;
}

int iEverbPlugin::get_user_interface()
{
	return IKXPLUGIN_OWNER;	// says to mixer that this plugin has its own control panel
}


/* Plugin UI Implementation */

void iEverbPluginDlg::init()
{
	CKXPluginGUI::init();

	// custom dialog initialization here...

	// create_dialog(Caption, Width, Height, ExtraHeight)
	create_dialog(plugin_name(everb), 300, 303); //w=275

	// create_hfader(Fader, Ctrl_ID, Label, MinVal, MaxVal, Left, Top, Width, Label_Width)
	create_hfader(fdrGain, GAIN_ID, "Gain:",-1,1, 7, 12, 283, 110); // fader must be declared with plugin dialog class
	create_hfader(fdrLevel, LEVEL_ID, "Level (%):",0,100, 7, 48, 283, 110);
	create_hfader(fdrDecay, DECAY_ID, "Decay Time (ms):",1,30, 7, 84, 283, 110);
	create_hfader(fdrDiffusion, DIFFUSION_ID, "Diffusion (%):", 0, 100, 7, 120, 283, 110);	
	create_hfader(fdrEarlyRefl, EARLY_REFL_ID, "Early Refl. (%):", 0, 100, 7, 156, 283, 110);
	create_hfader(fdrLateDelay, LATE_DELAY_ID, "Late Delay (ms)", 1, 350, 7, 192, 283, 110);
	create_hfader(fdrHighFreq, HIGH_FREQ_ID, "High Freq", -10, 3, 7, 228, 283, 110);
	create_hfader(fdrLowFreq, LOW_FREQ_ID, "Low Freq", -10, 3, 7, 264, 283, 110);

	controls_enabled = TRUE;

	sync(EVERYPM_ID);
}

void iEverbPluginDlg::sync(int ndx)
{
	char c[MAX_LABEL_STRING];
	kxparam_t v;
	switch (ndx)
	{
	case EVERYPM_ID: // update all controls
	case GAIN_ID:
		v = plugin->_params[GAIN_ID];
		if (controls_enabled) {fdrGain.slider.set_pos((int)v); fdrGain.slider.redraw();} // update fader value only if function called from mixer
		sprintf(c, "%i", v);
		if (v==1)
		sprintf(c, "boost");
		if (v==-1)
		sprintf(c, "cut");
		fdrGain.svalue.SetWindowText(c); // show new value for fader
		if (ndx != EVERYPM_ID) break;
	case LEVEL_ID:
		v = plugin->_params[LEVEL_ID];
		if (controls_enabled) {fdrLevel.slider.set_pos((int)v); fdrLevel.slider.redraw();} // update fader value only if function called from mixer
		sprintf(c, "%i", v);
		fdrLevel.svalue.SetWindowText(c); // show new value for fader
		if (ndx != EVERYPM_ID) break;
	case DECAY_ID:
		v = plugin->_params[DECAY_ID];
		if (controls_enabled) {fdrDecay.slider.set_pos((int)v); fdrDecay.slider.redraw();} // update fader value only if function called from mixer
		sprintf(c, "%i", v);
		fdrDecay.svalue.SetWindowText(c); // show new value for fader
		if (ndx != EVERYPM_ID) break;
	case DIFFUSION_ID:
		v = plugin->_params[DIFFUSION_ID];
		if (controls_enabled) {fdrDiffusion.slider.set_pos((int)v); fdrDiffusion.slider.redraw();} // update fader value only if function called from mixer
		sprintf(c, "%i", v);
		fdrDiffusion.svalue.SetWindowText(c); // show new value for fader
		if (ndx != EVERYPM_ID) break;
	case EARLY_REFL_ID:
		v = plugin->_params[EARLY_REFL_ID];
		if (controls_enabled) {fdrEarlyRefl.slider.set_pos((int)v); fdrEarlyRefl.slider.redraw();} // update fader value only if function called from mixer
		sprintf(c, "%i", v);
		fdrEarlyRefl.svalue.SetWindowText(c); // show new value for fader
		if (ndx != EVERYPM_ID) break;		
	case LATE_DELAY_ID:
		v = plugin->_params[LATE_DELAY_ID];
		if (controls_enabled) {fdrLateDelay.slider.set_pos((int)v); fdrLateDelay.slider.redraw();}
		sprintf(c, "%i", v);
		fdrLateDelay.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;
	case HIGH_FREQ_ID:
		v = plugin->_params[HIGH_FREQ_ID];
		if (controls_enabled) {fdrHighFreq.slider.set_pos((int)v); fdrHighFreq.slider.redraw();}
		sprintf(c, "%i", v);
		fdrHighFreq.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;
	case LOW_FREQ_ID:
		v = plugin->_params[LOW_FREQ_ID];
		if (controls_enabled) {fdrLowFreq.slider.set_pos((int)v); fdrLowFreq.slider.redraw();}
		sprintf(c, "%i", v);
		fdrLowFreq.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;
  }
}

