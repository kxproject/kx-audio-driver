// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov, Max Mikhailov 2001-2005.
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
#include "stchorus.h"
// effect source
#include "da_stchorus.cpp"

inline kxparam_t kabs(kxparam_t v)
{
        if(v<0) return -v; else return v;
}

// _params
typedef enum {
	LEVEL_ID,	
	INWIDTH_ID,	
	OUTWIDTH_ID,	
	FEEDBACK_ID,	
	DELAY_ID,	
	MODDEPTH_ID,	
	MODPHASE_ID,	
	MODFREQ_ID,	
	FEEDINVERT_ID
};

// default plugin _params
static kxparam_t stchorus_default_params[STCHORUS_PARAMS_COUNT] = {80, 0, 50, 16, 60, 20, -120, 150, 0}; //temporarely

/* Plugin Implemenation */

int iStereoChorusBPlugin::get_param(int ndx, kxparam_t *value) 
{
	*value = _params[ndx]; return 0;
}

int iStereoChorusBPlugin::set_param(int ndx, kxparam_t value) // calculates effect _params
{
	_params[ndx] = value;

	switch (ndx)
	{
	case LEVEL_ID:
		set_dsp_register(_R_LEVEL, itodspr_scl(100, value));
		break;

	case INWIDTH_ID:
		set_dsp_register(_R_INWIDTH, itodspr_scl(100, kabs(value)));
		write_instr_opcode(_IN_PHASE_INSTR, (word)((value>0)?MACS1:MACS));
		write_instr_opcode(_IN_PHASE_INSTR + 1, (word)((value>0)?MACS1:MACS));
		break;

	case OUTWIDTH_ID:
		set_dsp_register(_R_OUTWIDTH, itodspr_scl(100, 50 - value));
		break;

	case FEEDBACK_ID:
	case FEEDINVERT_ID:
		value = _params[FEEDBACK_ID];
		if (_params[FEEDINVERT_ID]) value = - value;
		set_dsp_register(_R_FEEDBACK, itodspr_scl(100, value));
		break;

	case DELAY_ID:
		set_dsp_register(_R_DELAY, (dword)((value * SAMPLES_PER_MSEC / 10 + 2) << 0xc));
		// or less precise: set_dsp_register(_R_DELAY, value * 0x4CCD + 0x2000);
		break;

	case MODDEPTH_ID:
		set_dsp_register(_R_MODDEPTH, itodspr_scl(200, value)); // 0...100 -> 0.0...0.5
		break;

	case MODPHASE_ID:
		set_dsp_register(_R_MODPHASE, itodspr_scl(180, value));
		break;

	case MODFREQ_ID:
		set_dsp_register(_R_MODFREQ, itodspr_scl(100 * (SAMPLES_PER_SEC / 2), value));
		break;
	}

	if (cp) ((iStereoChorusBPluginDlg*) cp)->sync(ndx);

	return 0;
}

int iStereoChorusBPlugin::set_defaults() 
{
	set_all_params(stchorus_default_params); return 0;
}

int iStereoChorusBPlugin::set_all_params(kxparam_t* values)
{
	kxparam_t value;

	memcpy(_params, values, STCHORUS_PARAMS_COUNT*sizeof(dword));
	
	// LEVEL_ID:
		set_dsp_register(_R_LEVEL, itodspr_scl(100, _params[LEVEL_ID]));

	// INWIDTH_ID:
		value = (int)_params[INWIDTH_ID];
		set_dsp_register(_R_INWIDTH, itodspr_scl(100, kabs(value)));
		write_instr_opcode(_IN_PHASE_INSTR, (word)((value>0)?MACS1:MACS));
		write_instr_opcode(_IN_PHASE_INSTR + 1, (word)((value>0)?MACS1:MACS));

	// OUTWIDTH_ID:
		set_dsp_register(_R_OUTWIDTH, itodspr_scl(100, 50 - _params[OUTWIDTH_ID]));

	// FEEDBACK_ID:
	// FEEDINVERT_ID:
		value = (int)_params[FEEDBACK_ID];
		if (_params[FEEDINVERT_ID]) value = - value;
		set_dsp_register(_R_FEEDBACK, itodspr_scl(100, value));

	// DELAY_ID:
		set_dsp_register(_R_DELAY, (dword)((_params[DELAY_ID] * SAMPLES_PER_MSEC / 10 + 2) << 0xc));

	// MODDEPTH_ID:
		set_dsp_register(_R_MODDEPTH, itodspr_scl(200, _params[MODDEPTH_ID]));

	// MODPHASE_ID:
		set_dsp_register(_R_MODPHASE, itodspr_scl(180, _params[MODPHASE_ID]));

	// MODFREQ_ID:
		set_dsp_register(_R_MODFREQ, itodspr_scl(100 * (SAMPLES_PER_SEC / 2), _params[MODFREQ_ID]));

	if (cp) ((iStereoChorusBPluginDlg*) cp)->sync(EVERYPM_ID);

	return 0;
}

int iStereoChorusBPlugin::request_microcode() // generally is called after init()
{
	publish_microcode(stchorus);

	// you should free code and info massives in ::close() if allocated dynamically
	return 0;
}

const char *iStereoChorusBPlugin::get_plugin_description(int id)
{
	plugin_description(stchorus);
}

iKXPluginGUI *iStereoChorusBPlugin::create_cp(kDialog *parent,kFile *mf)
{
	iStereoChorusBPluginDlg *tmp;
	tmp = new iStereoChorusBPluginDlg(this, parent, mf);
	tmp->create();
	return tmp;
}

int iStereoChorusBPlugin::get_user_interface()
{
	return IKXPLUGIN_OWNER;
}

static kx_fxparam_descr _param_descr[STCHORUS_PARAMS_COUNT] = {
	{"Level", KX_FXPARAM_LEVEL, 0, 100},
	{"InWidth", KX_FXPARAM_PERCENT, -50, 50},
	{"OutWidth", KX_FXPARAM_PERCENT, -50, 50},
	{"Feedback", KX_FXPARAM_PERCENT, 0, 99},
	{"Delay", KX_FXPARAM_MS, 0, 200},
	{"Mod Depth", KX_FXPARAM_PERCENT, 0, 100},
	{"Phase", KX_FXPARAM_ANGLE, -180, 180},
	{"Mod Rate", KX_FXPARAM_HZ, 0, 1000},
	{"Invert Feedback", KX_FXPARAM_SWITCH, 0, 1}};

int iStereoChorusBPlugin::describe_param(int ndx, kx_fxparam_descr *descr)
{
	memcpy(descr, &_param_descr[ndx], sizeof kx_fxparam_descr); return 0;
}


/* Plugin UI Implementation */

void iStereoChorusBPluginDlg::init()
{
	CKXPluginGUI::init();
	// custom dialog initialization here...

	// MAKEUP_DLG(Caption, Width, Height)
	create_dialog(stchorus_name, 535, 200);

	// add all GUI controls

	// SHOW_VFADER(Fader, Ctrl_ID, Label, MinVal, MaxVal, Left, Top, Width, Height)
	create_vfader(fdrLevel, LEVEL_ID, "Level", 0, 100,             5, 12, 55, 170);
	create_vfader(fdrInWidth, INWIDTH_ID, "InWidth", -50, 50,      65, 12, 55, 170);
	create_vfader(fdrOutWidth, OUTWIDTH_ID, "OutWidth", -50, 50,   125, 12, 55, 170);
	create_vfader(fdrFeedback, FEEDBACK_ID, "Feedback", 0, 99,     185, 12, 55, 170);

	// SHOW_HFADER(Fader, Ctrl_ID, Label, MinVal, MaxVal, Left, Top, Width, LabelWidth)
	create_hfader(fdrDelay, DELAY_ID, "Delay (ms)", 0, 200,          245, 47, 280, 80);
	create_hfader(fdrModDepth, MODDEPTH_ID, "Mod Depth (%)", 0, 100, 245, 82, 280, 80);
	create_hfader(fdrModPhase, MODPHASE_ID, "Phase (°)", -180, 180,  245, 117, 280, 80);
	create_hfader(fdrModFreq, MODFREQ_ID, "Mod Rate (Hz)",  0, 1000, 245, 152, 280, 80);

	// SHOW_CHECKBOX(CheckBox, Ctrl_ID, Caption, Left, Top, Width)
	create_checkbox(chkFeedInvert, FEEDINVERT_ID, "Invert Feedback", 255, 20, 120);

	controls_enabled = TRUE;

	sync(EVERYPM_ID); // Synchronize UI Controls with Plugin _params 

        redraw();
}

void iStereoChorusBPluginDlg::sync(int ndx)
{
	char c[MAX_LABEL_STRING];
	kxparam_t v;

	switch (ndx)
	{
	case EVERYPM_ID: // update all controls
	case LEVEL_ID:
		v = plugin->_params[LEVEL_ID];
		if (controls_enabled) {VSSETPOS(fdrLevel.slider, (int)v); fdrLevel.slider.redraw();}
		sprintf(c, "%i " "%%", v);
		fdrLevel.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;

	case INWIDTH_ID:
		v = plugin->_params[INWIDTH_ID];
		if (controls_enabled) {VSSETPOS(fdrInWidth.slider, (int)v); fdrInWidth.slider.redraw();}
		sprintf(c, "%i " "%%", v);
		fdrInWidth.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;

	case OUTWIDTH_ID:
		v = plugin->_params[OUTWIDTH_ID];
		if (controls_enabled) {VSSETPOS(fdrOutWidth.slider, (int)v); fdrOutWidth.slider.redraw();}
		sprintf(c, "%i " "%%", v);
		fdrOutWidth.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;
		
	case FEEDINVERT_ID:
		v = plugin->_params[FEEDINVERT_ID];
		if (controls_enabled) {chkFeedInvert.set_check((int)v); chkFeedInvert.redraw();}
		if (ndx != EVERYPM_ID) break;

	case FEEDBACK_ID:
		v = plugin->_params[FEEDBACK_ID];
		if (controls_enabled) {VSSETPOS(fdrFeedback.slider, (int)v); fdrFeedback.slider.redraw();}
		sprintf(c, "%i " "%%", v);
		fdrFeedback.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;

	case DELAY_ID:
		v = plugin->_params[DELAY_ID];
		if (controls_enabled) {fdrDelay.slider.set_pos((int)v); fdrDelay.slider.redraw();}
		sprintf(c, "%i.%i", v/10, v%10);
		fdrDelay.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;

	case MODPHASE_ID:
		v = plugin->_params[MODPHASE_ID];
		if (controls_enabled) {fdrModPhase.slider.set_pos((int)v); fdrModPhase.slider.redraw();}
		sprintf(c, "%i", v);
		fdrModPhase.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;

	case MODFREQ_ID:
		v = plugin->_params[MODFREQ_ID];
		if (controls_enabled) {fdrModFreq.slider.set_pos((int)v); fdrModFreq.slider.redraw();}
		sprintf(c, "%i.%02i", v/100, v%100);
		fdrModFreq.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;

	case MODDEPTH_ID:
		v = plugin->_params[MODDEPTH_ID];
		if (controls_enabled) {fdrModDepth.slider.set_pos((int)v); fdrModDepth.slider.redraw();}
		sprintf(c, "%i", v);
		fdrModDepth.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;
	}
}
