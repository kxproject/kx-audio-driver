// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov and Max Mikhailov, 2001-2004.
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

#include "Phaser.h"
#include "da_Phaser.cpp"

typedef enum _params_id{
	DRYLEV,
	WETLEV,
	FEEDBACK,
	XFEED,
	SWEEPRATE,
	SWEEPRANGE,
	FREQUENCY,
	STAGES
};

static kxparam_t default_params[PHASER_PARAMS_COUNT] = {14,	/*  0.4   ((val / 0.1) + 10)    */
												  16, 	/*  0.6   ((val / 0.1) + 10)    */
												  6,	/*  0.6    (val * 10)			*/
												  50,
												  4,	/*  0.4    (val * 10)			*/
												  40,	/*  5.0    (val / 0.125)		*/
												  180,
												  2}; 

int iPhaserPlugin::set_defaults() 
{
	set_all_params(default_params);	
	return 0;
}

static kx_fxparam_descr _param_descr[PHASER_PARAMS_COUNT] = {	
	{"Dry",			KX_FXPARAM_USER,		0,	  20},  /* -1.0 to  1.0 in increments of 0.1   */
	{"Wet",			KX_FXPARAM_USER,		0,	  20},  /* -1.0 to  1.0 in increments of 0.1   */
	{"Feedback",	KX_FXPARAM_LEVEL,	   -9,	   9},  /* -0.9 to  0.9 in increments of 0.1   */
	{"CrossFeed",	KX_FXPARAM_PERCENT,	    0,	 100},  /*  0.0 to  100 in increments of 5.0   */
	{"Sweep Rate",	KX_FXPARAM_USER,		1,	 100},  /*  0.1 to  250 in increments of 0.1   */
	{"Sweep Range",	KX_FXPARAM_USER,		0,	  52},  /*  0.0 to  6.5 in increments of 0.125 */
	{"Frequency",	KX_FXPARAM_USER,		50,	1500},  /*  50  to 1500 in increments of 1.0   */
	{"Stages",		KX_FXPARAM_USER,		0,	   2}   
};

int iPhaserPlugin::describe_param(int ndx, kx_fxparam_descr *descr)
{
	memcpy(descr, &_param_descr[ndx], sizeof kx_fxparam_descr); return 0;
}

#define P (kxparam_t)
static kxparam_t presets[]=
{ 
	NULL // must be here
};

#undef P

const kxparam_t *iPhaserPlugin::get_plugin_presets()
{
 return presets;
}


int iPhaserPlugin::get_param(int ndx, kxparam_t *value)
{
	*value = _params[ndx]; 
	return 0;
}


int iPhaserPlugin::set_param(int ndx, kxparam_t value)
{
	_params[ndx] = value;
	double v;
	
	switch (ndx)
	{	
		case DRYLEV:
			v = (double)value;				/* 0 to 20 */
			v = ((v * .1) - 1);		/* -1.0 to 1.0 step 0.1 */
			set_dsp_register(DRYLEV_P, _dbl_to_dspword(v));
			break;
		case WETLEV:
			v = (double)value;				/* 0 to 20 */
			v = ((v * .1) - 1);		/* -1.0 to 1.0 step 0.1 */
			set_dsp_register(WETLEV_P, _dbl_to_dspword(v));
			break;
		case FEEDBACK:		
			v = (double)value;				/* -9 to 9 */
			v = (v / 10.0f);			/* -0.9 to 0.9 step 0.1 */
			set_dsp_register(FEEDBACK_P, _dbl_to_dspword(v));
			break;
		case XFEED:		
			v = (double)value;				/* 0 to 100  */
			v = (v / 100.0f);			/* 0 to 1 step 0.1 */
			set_dsp_register(XFEED_P, _dbl_to_dspword(v));
			break;
		case SWEEPRATE:
									/* 1 to 2500 */
									/* 0.1 to 250 step 0.1 */
			UpdateParams();
			break;
		case SWEEPRANGE:
									/* 0 to 52 */
									/* 0 to 6.5 step 0.125 */
			UpdateParams();
			break;
		case FREQUENCY:
									/* 50 to 1500 */
									/* 50 to 1500 step 1 */			
			UpdateParams();
			break;
		case STAGES:
			switch (value)
			{
				case 0:
					write_instr_y(ADDR1, STAGE2_LEFT_P);
					write_instr_y(ADDR2, STAGE2_RIGHT_P);
					write_instr_y(ADDR3, STAGE2_LEFT_P);
					write_instr_y(ADDR4, STAGE2_RIGHT_P);
				break;
				case 1:
					write_instr_y(ADDR1, STAGE4_LEFT_P);
					write_instr_y(ADDR2, STAGE4_RIGHT_P);
					write_instr_y(ADDR3, STAGE4_LEFT_P);
					write_instr_y(ADDR4, STAGE4_RIGHT_P);
				break;
				case 2:
					write_instr_y(ADDR1, STAGE6_LEFT_P);
					write_instr_y(ADDR2, STAGE6_RIGHT_P);
					write_instr_y(ADDR3, STAGE6_LEFT_P);
					write_instr_y(ADDR4, STAGE6_RIGHT_P);
				break;
			}
			break;
	}	

	if (cp) ((iPhaserPluginDlg*) cp)->sync(ndx);	

	return 0;
}

int iPhaserPlugin::set_all_params(kxparam_t* values)
{
	for (int i = 0; i < PHASER_PARAMS_COUNT; i++) set_param(i, values[i]);
	return 0;
}

int iPhaserPlugin::request_microcode() 
{
	publish_microcode(Phaser);

	return 0;
}

const char *iPhaserPlugin::get_plugin_description(int id)
{
	plugin_description(Phaser);
}


iKXPluginGUI *iPhaserPlugin::create_cp(kDialog *parent, kFile *mf)
{
	iPhaserPluginDlg *tmp;
	tmp = new iPhaserPluginDlg(this, parent,mf);
	tmp->create();
	return tmp;
}

int iPhaserPlugin::get_user_interface()
{
	return IKXPLUGIN_OWNER;
}


void iPhaserPluginDlg::init()
{
	CKXPluginGUI::init();

	create_dialog(plugin_name(Phaser), 680, 250);	
	create_vfader(dry, DRYLEV, "Dry", 0, 20, 15, 15, 90, 170);
	dry.slider.set_tic_freq(2);
	create_vfader(wet, WETLEV, "Wet", 0, 20, 105, 15, 90, 170);
	wet.slider.set_tic_freq(2);
	create_vfader(feedback, FEEDBACK, "Feedback", -9, 9, 195, 15, 90, 170);
	feedback.slider.set_tic_freq(2);
	create_vfader(xfeed, XFEED, "CrossFeed", 0, 100, 285, 15, 90, 170);
	xfeed.slider.set_tic_freq(10);
	create_vfader(sweeprate, SWEEPRATE, "SweepRate", 1, 100,	375, 15, 90, 170);
	sweeprate.slider.set_tic_freq(10);
	create_vfader(sweeprange, SWEEPRANGE, "SweepRange", 0, 52, 465, 15, 90, 170);
	sweeprange.slider.set_tic_freq(4);
	create_vfader(frequency, FREQUENCY, "Frequency", 50, 1500, 555, 15, 90, 170);
	create_label(label,PHASER_PARAMS_COUNT+1,"Number of Stages:", 20, 204, 105);
	create_combo(stages,STAGES,"Stages",135,200,35,5,80);	
	stages.add_string("2");
	stages.add_string("4");
	stages.add_string("6");
	stages.set_selection(2);	

	controls_enabled = 1;

	sync(EVERYPM_ID);
}

void iPhaserPluginDlg::sync(int ndx)
{
	char c[MAX_LABEL_STRING];
	kxparam_t v;

	switch (ndx)
	{ 
	case EVERYPM_ID:
	case DRYLEV:
		v = plugin->_params[DRYLEV];
		if (controls_enabled) { VSSETPOS(dry.slider, (int)v); dry.slider.redraw(); }
		sprintf(c, "%.1f", ((v * .1) - 1));
		dry.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;		

		case WETLEV:
		v = plugin->_params[WETLEV];
		if (controls_enabled) { VSSETPOS(wet.slider, (int)v); wet.slider.redraw(); }
		sprintf(c, "%.1f", ((v * .1) - 1));
		wet.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;

		case FEEDBACK:
		v = plugin->_params[FEEDBACK];
		if (controls_enabled) { VSSETPOS(feedback.slider, (int)v); feedback.slider.redraw(); }
		sprintf(c, "%.1f", ( v / 10.));
		feedback.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;

		case XFEED:
		v = plugin->_params[XFEED];
		if (controls_enabled) { VSSETPOS(xfeed.slider, (int)v); xfeed.slider.redraw(); }
		sprintf(c, "%d%%", v);
		xfeed.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;

		case SWEEPRATE:
		v = plugin->_params[SWEEPRATE];
		if (controls_enabled) { VSSETPOS(sweeprate.slider, (int)v); sweeprate.slider.redraw(); }
		sprintf(c, "%.1f Hz", v / 10.);
		sweeprate.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;

		case SWEEPRANGE:
		v = plugin->_params[SWEEPRANGE];
		if (controls_enabled) { VSSETPOS(sweeprange.slider, (int)v); sweeprange.slider.redraw(); }
		sprintf(c, "%.3f Octaves", v * .125);
		sweeprange.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;

		case FREQUENCY:
		v = plugin->_params[FREQUENCY];
		if (controls_enabled) { VSSETPOS(frequency.slider, (int)v); frequency.slider.redraw(); }
		sprintf(c, "%d Hz", v);
		frequency.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;

		case STAGES:
		if(controls_enabled) stages.set_selection((int)plugin->_params[STAGES]);
		if (ndx != EVERYPM_ID) break;
	}
}

void iPhaserPlugin::UpdateParams(void)
{	
	int SamplingRate = 48000;
	double Frequency;
	double SweepRate;
	double SweepRange;
	double MinSweep;
	double MaxSweep;
	double SweepVal;
	double Rate;
	double SweepFactor;
	double Range;	
	
	Frequency = (double)_params[FREQUENCY]; 
	SweepRate = (double)_params[SWEEPRATE] / 10;
	SweepRange = (double)_params[SWEEPRANGE] * .125;

	MinSweep = pi * Frequency / SamplingRate;
	MinSweep = MinSweep / 10;
	set_dsp_register(SW_MIN_P, _dbl_to_dspword(MinSweep));	

	Range = pow(2.0, SweepRange);

	MaxSweep = pi * Frequency * Range / SamplingRate;
	MaxSweep = MaxSweep / 10;
	set_dsp_register(SW_MAX_P, _dbl_to_dspword(MaxSweep));

	SweepVal = MinSweep;
	set_dsp_register(SW_VAL_P, _dbl_to_dspword(SweepVal));

	Rate = pow(Range, 2.0 * SweepRate / SamplingRate);
	Rate = Rate / 10;

	SweepFactor = Rate;
	set_dsp_register(SW_FAC_P, _dbl_to_dspword(SweepFactor));

	set_dsp_register(SW_UP_P, _dbl_to_dspword(Rate));  	
	
	Rate = (.1f / Rate) / 10;	
	set_dsp_register(SW_DOWN_P, _dbl_to_dspword(Rate)); 	
}
