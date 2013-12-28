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
#include "eq10a.h"
// effect source
#include "da_eq10a.cpp"

// plugin parameters
typedef enum _params_id {
	LEVEL_ID,
	BAND_ID
};

static char* bandname[] = {"31", "62", "125", "250", "500", "1K", "2K", "4K", "8K", "16K"};

// default plugin parameters
static kxparam_t default_params[EQ10A_PARAMS_COUNT] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

static kx_fxparam_descr _param_descr[EQ10A_PARAMS_COUNT] = {
	{"Input Level",  KX_FXPARAM_CB, -240, 0},
	{" 31 Hz Band",  KX_FXPARAM_CB, -120, 120},
	{" 62 Hz Band",  KX_FXPARAM_CB, -120, 120},
	{"125 Hz Band",  KX_FXPARAM_CB, -120, 120},
	{"250 Hz Band",  KX_FXPARAM_CB, -120, 120},
	{"500 Hz Band",  KX_FXPARAM_CB, -120, 120},
	{" 1 kHz Band",  KX_FXPARAM_CB, -120, 120},
	{" 2 kHz Band",  KX_FXPARAM_CB, -120, 120},
	{" 4 kHz Band",  KX_FXPARAM_CB, -120, 120},
	{" 8 kHz Band",  KX_FXPARAM_CB, -120, 120},
	{"16 kHz Band",  KX_FXPARAM_CB, -120, 120}};

int iEq10aPlugin::describe_param(int ndx, kx_fxparam_descr *descr)
{
	memcpy(descr, &_param_descr[ndx], sizeof kx_fxparam_descr); return 0;
}

#define P (kxparam_t)
static kxparam_t presets[]=
{
 // don't include 'default' preset
 P "Rock",     0,60,30,20,10,0,0,10,20,30,60,
 P "Pop",      0,10,20,30,40,60,60,40,30,20,10,
 P "Jazz",     0,20,30,30,20,10,00,-10,-20,-20,-10,
 P "Classic",  0,-10,-20,-20,-10,00,10,20,30,30,20,
 P "Techno",   0,30,60,30,00,-60,-60,00,30,60,30,
 P "Disco",    0,-60,30,30,-30,0,-15,-30,-30,0,30,
 P "Heavy",    0,0,30,30,0,-32,-60,-28,30,60,30,
 P "Radio",    0,-120,-120,-120,-120,30,60,30,-120,-120,-120,
 NULL  // must be here
};
#undef P

const kxparam_t *iEq10aPlugin::get_plugin_presets()
{
 return presets;
}

/* Plugin Implemenation */

int iEq10aPlugin::get_param(int ndx, kxparam_t *value)
{
	*value = _params[ndx]; 
	return 0;
}

int iEq10aPlugin::set_param(int ndx, kxparam_t value)
{
	_params[ndx] = value;

	switch (ndx){
	case LEVEL_ID:
		set_dsp_register(R_S, _dbl_to_dspword(cBtoG((int)value)));
		break;

	default:
		set_dsp_register((word)(R_GX + R_GSTEP * (ndx - BAND_ID)), _dbl_to_dspword(0.25 * cBtoG((int)_params[ndx]) - 0.25));
		break;
	}

	if (cp) ((iEq10aPluginDlg*) cp)->sync(ndx);

	return 0;
}

int iEq10aPlugin::set_defaults() 
{
	set_all_params(default_params); 
	return 0;
}

int iEq10aPlugin::set_all_params(kxparam_t* values)
{
	for (int i = 0; i < EQ10A_PARAMS_COUNT; i++) set_param(i, values[i]);
	return 0;
}

int iEq10aPlugin::request_microcode() 
{
	publish_microcode(eq10a);

	return 0;
}

iKXPluginGUI *iEq10aPlugin::create_cp(kDialog *parent, kFile *mf)
{
	iEq10aPluginDlg *tmp;
	tmp = new iEq10aPluginDlg(this, parent, mf);
	tmp->create();
	return tmp;
}

int iEq10aPlugin::get_user_interface()
{
	return IKXPLUGIN_OWNER;
}


/* Plugin UI Implementation */

// identifiers for extra control
typedef enum _extra_ctrl {
	LGAIN_ID = EQ10A_PARAMS_COUNT,
	LFREQ_ID
};

void iEq10aPluginDlg::init()
{
	CKXPluginGUI::init();
	// custom dialog initialization here...

	// MAKEUP_DLG(Caption, Width, Height)
	create_dialog(plugin_name(eq10a), 542, 220);

	for (int i = 0; i < EQ10A_BANDS_COUNT; i++)
	{
		// create_hfader(Fader, Ctrl_ID, Label, MinVal, MaxVal, Left, Top, Width, Height)
		create_vfader(fdrBand[i], BAND_ID + i, bandname[i], -120, 120, (116 + (i * 40)), 12, 50, 170);
		fdrBand[i].slider.set_page_size(10);
	}

	create_vfader(fdrLevel, LEVEL_ID, " Input", -240, 0, 4, 12, 60, 170);
	fdrLevel.slider.set_page_size(10);

	// SHOW_CHECKBOX(CheckBox, Ctrl_ID, Caption, Left, Top, Width)

	// SHOW_LABEL(Label, Ctrl_ID, Caption, Left, Top, Width)
	create_label(lblGain, LGAIN_ID, "Gain (dB):", 65, 12, 60);
	create_label(lblFreq, LFREQ_ID, "Freq (Hz):", 65, 167, 60);

	controls_enabled = TRUE;

	for (int i = 0; i < EQ10A_PARAMS_COUNT; i++)
	  sync(i);
}

void iEq10aPluginDlg::sync(int ndx)
{
	char c[MAX_LABEL_STRING];
	kxparam_t v;

	switch (ndx)
	{
	case EVERYPM_ID: // update all controls
	case LEVEL_ID:
		v = plugin->_params[LEVEL_ID];
		if (controls_enabled) {VSSETPOS(fdrLevel.slider, (int)v); fdrLevel.slider.redraw();}
		sprintf(c, " %.1fdB", v/10.);
		fdrLevel.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;		
	default:
		if(ndx>=0)
		{
			v = plugin->_params[ndx]; ndx = ndx - BAND_ID;
			if (controls_enabled) {VSSETPOS(fdrBand[ndx].slider, (int)v); fdrBand[ndx].slider.redraw();}
			sprintf(c, "%.1f", v/10.);
			fdrBand[ndx].svalue.SetWindowText(c);
			break;
	       }
	       else
	       {
	       	for(int i=0;i<EQ10A_BANDS_COUNT;i++)
	       	{
			v = plugin->_params[i+BAND_ID];
			if (controls_enabled) {VSSETPOS(fdrBand[i].slider, (int)v); fdrBand[i].slider.redraw();}
			sprintf(c, "%.1f", v/10.);
			fdrBand[i].svalue.SetWindowText(c);
		}
	       }
	}
}
