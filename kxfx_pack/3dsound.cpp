// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov and WORM  2002-2005.
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
#include "3dsound.h"
// effect source
#include "da_soundgen.cpp"

// Parameters
typedef enum {
    	G_1,
		G_2,
		T_1,
		T_2,
		T_3,
		T_4,
};

// default plugin parameters
static kxparam_t soundgen_default_params[SOUNDGEN_PARAMS_COUNT] = {40, 30, 12, 16, 30, 10}; // valid for all instances


/* Plugin Implemenation */

int iSoundGenPlugin::get_param(int ndx, kxparam_t* value) 
{
	*value = _params[ndx];
	return 0;
}

int iSoundGenPlugin::set_param(int ndx, kxparam_t value) // calculates effect _params
{
	kxparam_t t;
	_params[ndx] = value;

	switch (ndx)
	{
	case G_1:
		set_dsp_register(R_G_1, itodspr_scl(100, value));
		break;

	case G_2:
			set_dsp_register(R_G_2, itodspr_scl(100, value));
			set_dsp_register(R_G_21, itodspr_scl(100,(-value) ));
			set_dsp_register(R_G_22, itodspr_scl(100, (1-(value*value)) ));
		break;

	case T_1:
		// reading delay start (write) address, adding predelay time to it and writing to delay read address
		// left channel predelay
		get_tram_addr(R_T_1_W, (dword *) &t);
		set_tram_addr(R_T_1_R, (dword)(t + (value+1)* SAMPLES_PER_MSEC));

		break;
	case T_2:
		// reading delay start (write) address, adding predelay time to it and writing to delay read address
		// left channel predelay
		get_tram_addr(R_T_2_W, (dword *) &t);
		set_tram_addr(R_T_2_R, (dword)(t + (value+1)* SAMPLES_PER_MSEC));

		break;
	case T_3:
		// reading delay start (write) address, adding predelay time to it and writing to delay read address
		// left channel predelay
		get_tram_addr(R_T_3_W, (dword *) &t);
		set_tram_addr(R_T_3_R, (dword)(t + (value+1)* SAMPLES_PER_MSEC));

		break;
	case T_4:
		// reading delay start (write) address, adding predelay time to it and writing to delay read address
		// left channel predelay
		get_tram_addr(R_T_4_W, (dword *) &t);
		set_tram_addr(R_T_4_R, (dword)(t + (value+1)* SAMPLES_PER_MSEC));

		break;
	default:
		return -1;
	}

	if (cp) ((iSoundGenPluginDlg*) cp)->sync(ndx);

	return 0;
}

int iSoundGenPlugin::set_defaults() 
{
	set_all_params(soundgen_default_params); 
	return 0;
}

int iSoundGenPlugin::set_all_params(kxparam_t* values)
{
	kxparam_t t;

	memcpy(_params, values, SOUNDGEN_PARAMS_COUNT*sizeof(kxparam_t));

	// G_1:
		set_dsp_register(R_G_1, itodspr_scl(100, _params[G_1]));

	// G_2:
		set_dsp_register(R_G_2, itodspr_scl(100, _params[G_2] ));
		set_dsp_register(R_G_21, itodspr_scl(100,(-(_params[G_2])) ));
		set_dsp_register(R_G_22, itodspr_scl(100, (1-( (_params[G_2])*(_params[G_2]) )) ));


	// T_1:
		get_tram_addr(R_T_1_W, (dword *) &t);
		set_tram_addr(R_T_1_R, (dword)(t + (_params[T_1]+1)* SAMPLES_PER_MSEC));
    // T_2:
		get_tram_addr(R_T_2_W, (dword *) &t);
		set_tram_addr(R_T_2_R, (dword)(t + (_params[T_2]+1)* SAMPLES_PER_MSEC));
	// T_3:
		get_tram_addr(R_T_3_W, (dword *) &t);
		set_tram_addr(R_T_3_R, (dword)(t + (_params[T_3]+1)* SAMPLES_PER_MSEC));
	// T_4:
		get_tram_addr(R_T_4_W, (dword *) &t);
		set_tram_addr(R_T_4_R, (dword)(t + (_params[T_4]+1)* SAMPLES_PER_MSEC));

	if (cp) ((iSoundGenPluginDlg*) cp)->sync(EVERYPM_ID);

	return 0;
}

int iSoundGenPlugin::request_microcode() // generally is called after init()
{
	publish_microcode(soundgen);

	// you should free code and info massives in ::close() if allocated dynamically
	return 0;
}

const char *iSoundGenPlugin::get_plugin_description(int id)
{
	plugin_description(soundgen);
}

iKXPluginGUI *iSoundGenPlugin::create_cp(kDialog *parent,kFile *mf_)
{
	iSoundGenPluginDlg *tmp;
	tmp = new iSoundGenPluginDlg(this, parent,mf_);
	tmp->create();
	return tmp;
}

int iSoundGenPlugin::get_user_interface()
{
	return IKXPLUGIN_OWNER;
}

static kx_fxparam_descr _param_descr[SOUNDGEN_PARAMS_COUNT] = 
{
	{"G_1-FilterFeedback", KX_FXPARAM_PERCENT, 0, 100},
	{"G_2-CrossFeedback", KX_FXPARAM_PERCENT, 0, 100},
	{"T_1-LeftPreDelay", KX_FXPARAM_MS, 0, 39},
	{"T_2-LeftDelay", KX_FXPARAM_MS, 0, 39},
	{"T_3-RightPreDelay", KX_FXPARAM_MS, 0, 39},
	{"T_4-RightDelay", KX_FXPARAM_MS, 0, 39},
};

int iSoundGenPlugin::describe_param(int ndx, kx_fxparam_descr *descr)
{
	memcpy(descr, &_param_descr[ndx], sizeof kx_fxparam_descr); return 0;
}

/* Plugin UI Implementation */

void iSoundGenPluginDlg::init()
{
	CKXPluginGUI::init();

	// custom dialog initialization here...

	// MAKEUP_DLG(Caption, Width, Height)
	create_dialog(soundgen_name, 570, 220);

	// add all GUI controls

	// SHOW_VFADER(Fader, Ctrl_ID, Label, MinVal, MaxVal, Left, Top, Width, Height)
	create_vfader(fdrT_1, T_1, "LeftPreDelay" , 0,39, 5, 47, 80, 155);
	create_vfader(fdrT_2, T_2, "LeftDelay"    , 0,39, 95, 47, 80, 155);
	create_vfader(fdrT_3, T_3, "RightPreDelay", 0,39, 185, 47, 80, 155);
	create_vfader(fdrT_4, T_4, "RightDelay"   , 0,39, 275, 47, 80, 155);

	create_vfader(fdrG_2, G_2, "FilterFeedBack" , 0, 100, 365, 12, 80, 190);
	create_vfader(fdrG_1, G_1, "CrossFeedback", 0, 100, 460, 12, 80, 190);

	create_label(label,0,"Surround sound generator for rear channels",10,12,285);

	controls_enabled = TRUE;

	sync(EVERYPM_ID); // Synchronize UI Controls with Plugin _params 
}

void iSoundGenPluginDlg::sync(int ndx)
{
	char c[MAX_LABEL_STRING];
	kxparam_t v;

	switch (ndx)
	{
	case EVERYPM_ID: // update all controls
	case G_1:
		v = plugin->_params[G_1];
		if (controls_enabled)
		{
			VSSETPOS(fdrG_1.slider, (int)v);
			fdrG_1.slider.redraw();
		}
		sprintf(c, "%i " "%%", v);
		fdrG_1.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;

	case G_2:
		v = plugin->_params[G_2];
		if (controls_enabled)
		{
			VSSETPOS(fdrG_2.slider, (int)v);
			fdrG_2.slider.redraw();
		}
		sprintf(c, "%i " "%%", v);
		fdrG_2.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;
	case T_1:
		v = plugin->_params[T_1];
		if (controls_enabled)
		{
			VSSETPOS(fdrT_1.slider, (int)v);
			fdrT_1.slider.redraw();
		}
		sprintf(c, "%i ""msec", v);
		fdrT_1.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;
	case T_2:
		v = plugin->_params[T_2];
		if (controls_enabled)
		{
			VSSETPOS(fdrT_2.slider, (int)v);
			fdrT_2.slider.redraw();
		}
		sprintf(c, "%i ""msec" , v);
		fdrT_2.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;
	case T_3:
		v = plugin->_params[T_3];
		if (controls_enabled)
		{
			VSSETPOS(fdrT_3.slider, (int)v);
			fdrT_3.slider.redraw();
		}
		sprintf(c, "%i ""msec", v);
		fdrT_3.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;
	case T_4:
		v = plugin->_params[T_4];
		if (controls_enabled)
		{
			VSSETPOS(fdrT_4.slider, (int)v);
			fdrT_4.slider.redraw();
		}
		sprintf(c, "%i ""msec", v);
		fdrT_4.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;

	
/*	case DECAY_ID:
		v = plugin->_params[DECAY_ID];
		if (controls_enabled) {fdrDecay.slider.set_pos(v); fdrDecay.slider.redraw();}
		v = v * v; // this trick make Decay Fader to look logarithmic ;)
		sprintf(c, "%i.%i", v/1000, (v/100)%10);
		fdrDecay.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;
*/
	}
}

