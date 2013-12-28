// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov, Max Mikhailov and Dmitry Kapustin, 2001-2005.
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
#include "Cross_2nd.h"
// effect source
#include "da_Crossover_2nd.cpp"

// plugin parameters
// (these are 'logical' parameters visible to the user)
typedef enum _params_id{
	FREQ_ID
};

#define __LowerFreq_Cr2 20
#define __UpperFreq_Cr2 20000

// default plugin parameters
// -------------------------
static kxparam_t default_params[CROSS_2_PARAMS_COUNT] = {1000};

int iCross_2_Plugin::set_defaults() 
{
	set_all_params(default_params); 
	return 0;
}

// for kX Automation: param description (see kxplugin.h for details on kx_fxparam_descr)
// -------------------------------------------------------------------------------------

static kx_fxparam_descr _param_descr[CROSS_2_PARAMS_COUNT] = {
	{"Freq", KX_FXPARAM_HZ, __LowerFreq_Cr2, __UpperFreq_Cr2}
	};

int iCross_2_Plugin::describe_param(int ndx, kx_fxparam_descr *descr)
{
	memcpy(descr, &_param_descr[ndx], sizeof kx_fxparam_descr); return 0;
}
/*
const kxparam_t *iCrossPlugin::get_plugin_presets()
{
 return NULL;
}
*/

/* Plugin Implemenation */

int iCross_2_Plugin::get_param(int ndx, kxparam_t *value) // ndx corresponds to 'logical' parameter id
{
	*value = _params[ndx]; 
	return 0;
}

// set_param sets 'logical' values by writing to (one or a group of) 'physical' registers
int iCross_2_Plugin::set_param(int ndx, kxparam_t value)
{	_params[ndx] = value;
	switch (ndx)
	{
	case FREQ_ID:
		double F, t, k; 
		F = int(__LowerFreq_Cr2 * exp((log((double)__UpperFreq_Cr2 / __LowerFreq_Cr2)/(double)(__UpperFreq_Cr2 - __LowerFreq_Cr2))*(double)(value)));
		t = tan(pi * F / FS);
		k = (1 - t) / (1 + t); 
		set_efx_register("k", _dbl_to_dspword(k)); // writing value to level register with conversion from double to dsp's dword
		break; 
	}
        // we need do synchronize the GUI part, if set_param was called by the mixer or kX Automation
	if (cp) ((iCross_2_PluginDlg*) cp)->sync(ndx);
	return 0;
}

int iCross_2_Plugin::set_all_params(kxparam_t* values)
{
	for (int i = 0; i < CROSS_2_PARAMS_COUNT; i++) set_param(i, values[i]);
	return 0;
}

int iCross_2_Plugin::request_microcode() 
{
	publish_microcode(Crossover_2);
	return 0;
}

const char *iCross_2_Plugin::get_plugin_description(int id)
{
	plugin_description(Crossover_2);
}


iKXPluginGUI *iCross_2_Plugin::create_cp(kDialog *parent, kFile *mf)
{
	iCross_2_PluginDlg *tmp;
	tmp = new iCross_2_PluginDlg(this, parent,mf);
	tmp->create();
	return tmp;
}

int iCross_2_Plugin::get_user_interface()
{
	return IKXPLUGIN_OWNER; // means 'custom' GUI panel
}


/* Plugin UI Implementation */

void iCross_2_PluginDlg::init()
{	CKXPluginGUI::init(); // this is necessary
	// custom dialog initialization here...
	// (title, Width, Height)
	create_dialog(plugin_name(Crossover_2), 350, 100);
    // you can add any label; the ID should be >PARAM_COUNT
    create_label(label,CROSS_2_PARAMS_COUNT+1,"Crossover freqency: 600 Hz",10,15,320);
	// create_hfader(	Fader,	Ctrl_ID,	Label,			MinVal,		MaxVal,	Left,	Top,	Width,	Label_Width) 
	create_hslider(		freq_1,	FREQ_ID,	"Frequency",	__LowerFreq_Cr2,			__UpperFreq_Cr2,	10,		35,		340,	30);
	controls_enabled = TRUE; // enable updating GUI when the parameters are changed
	sync(EVERYPM_ID); // update all 'GUI' elements to the preset plugin parameters
}

void iCross_2_PluginDlg::sync(int ndx)
{	char c[MAX_LABEL_STRING];
	kxparam_t v;
	switch (ndx)
	{ 
	case EVERYPM_ID:
	case FREQ_ID:
	//	 here we sync the 'value' of the parameter
		v = plugin->_params[FREQ_ID];
		if (controls_enabled) 
			{	freq_1.set_pos((int)plugin->_params[FREQ_ID]); 
				freq_1.redraw(); 
			}
		sprintf(c, "%s%i%s", "Crossover frequency: ", int(__LowerFreq_Cr2 * exp((log((double)__UpperFreq_Cr2 / __LowerFreq_Cr2)/(double)(__UpperFreq_Cr2 - __LowerFreq_Cr2))*(double)(v))), " Hz");
		label.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;
		// fallthru
	}
}
