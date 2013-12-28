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
#include "amp.h"
// effect source
#include "da_amp.cpp"

// Plugin Parameters (this identifiers also used for gui controls)
typedef enum _params_id{
	LOCK_ID,
	LEFT_LEVEL_ID,
	RIGHT_LEVEL_ID
};

// default plugin Parameters (used when plugin is loaded for the first time)
static kxparam_t amp_default_params[AMP_PARAMS_COUNT] = { 1, 0, 0};

/* Plugin Implementation */

static kx_fxparam_descr _param_descr[AMP_PARAMS_COUNT] = {
	{"Lock",   KX_FXPARAM_SWITCH, 0, 1 },
	{"Left Level", KX_FXPARAM_CB, -600, 600 },
	{"Right Level", KX_FXPARAM_CB, -600, 600  }
	};

int iAmpPlugin::describe_param(int ndx, kx_fxparam_descr *descr)
{
 memcpy(descr, &_param_descr[ndx], sizeof kx_fxparam_descr); 
 return 0;
}


int iAmpPlugin::get_param(int ndx, kxparam_t *value) // ndx corresponds to parameter id
{
	*value = _params[ndx]; 
	return 0;
}

int iAmpPlugin::set_param(int ndx, kxparam_t value)
{
	_params[ndx] = value; // saving value into _params[] for later use

	if(ndx==LOCK_ID)
	 return 0;

	double x;
	int gain1;
	double gain2;

	switch (ndx)
	{
	case LEFT_LEVEL_ID:
		x = value/10.0; //range is -600 to 600 dB
		if (x>=0) //0 to 600
		{
			x=dBtoG(x);
			//gain1 = x;
			gain1 = static_cast < int > ( x ); //convert to int to truncate
			gain1 = gain1 - static_cast < int > (gain1 * 0.0234375);
			gain2 = x - gain1; //get fractional part
			set_dsp_register(R_GAINL_I, gain1); // writing value to level register with conversion from double to dsp's dword
			set_dsp_register(R_GAINL_F, _dbl_to_dspword(gain2));
		}
		else
		{
			//-600 to 0
			x=dBtoG(x);
			gain1 = 0;
			gain2 = x;
			set_dsp_register(R_GAINL_I, gain1); // writing value to level register with conversion from double to dsp's dword
			set_dsp_register(R_GAINL_F, _dbl_to_dspword(gain2));
		}
		if(!_params[LOCK_ID])
		 break;
                // else - fall thru
	case RIGHT_LEVEL_ID:
		x = value/10.0; //range is -600 to 600 dB
		if (x>=0) //0 to 600
		{
			x=dBtoG(x);
			//gain1 = x;
			gain1 = static_cast < int > ( x ); //convert to int to truncate
			gain1 = gain1 - static_cast < int > (gain1 * 0.0234375);
			gain2 = x - gain1; //get fractional part
			set_dsp_register(R_GAINR_I, gain1); // writing value to level register with conversion from double to dsp's dword
			set_dsp_register(R_GAINR_F, _dbl_to_dspword(gain2));
		}
		else
		{
			//-600 to 0
			x=dBtoG(x);
			gain1 = 0;
			gain2 = x;
			set_dsp_register(R_GAINR_I, gain1); // writing value to level register with conversion from double to dsp's dword
			set_dsp_register(R_GAINR_F, _dbl_to_dspword(gain2));
	        }
	}

	if (cp)
	{
	  ((iAmpPluginDlg*) cp)->sync(ndx);

          if((((iAmpPluginDlg*) cp)->controls_enabled==0)&&_params[LOCK_ID]) // set the 'pair', too
          {
            ((iAmpPluginDlg*) cp)->controls_enabled=1;
            if(ndx==LEFT_LEVEL_ID)
             set_param(RIGHT_LEVEL_ID,value);
            else
             set_param(LEFT_LEVEL_ID,value);
            ((iAmpPluginDlg*) cp)->controls_enabled=0;
          }
        }

	return 0;
}

int iAmpPlugin::set_defaults() 
{
	set_all_params(amp_default_params); 
	return 0;
}

int iAmpPlugin::set_all_params(kxparam_t* values)
{
	for (int i = 0; i < AMP_PARAMS_COUNT; i++) set_param(i, values[i]);
	return 0;
}

int iAmpPlugin::request_microcode() 
{
	publish_microcode(amp);
	return 0;
}

iKXPluginGUI *iAmpPlugin::create_cp(kDialog *parent, kFile *mf)
{
	iAmpPluginDlg *tmp;
	tmp = new iAmpPluginDlg(this, parent, mf);
	tmp->create();
	return tmp;
}

int iAmpPlugin::get_user_interface()
{
	return IKXPLUGIN_OWNER;	// says to mixer that this plugin has its own control panel
}


void iAmpPluginDlg::init()
{
	CKXPluginGUI::init();
	// custom dialog initialization here...

	// create_dialog(Caption, Width, Height, ExtraHeight = 25)
	create_dialog(plugin_name(amp), 300, 120);

	// create_hfader(Fader, Ctrl_ID, Label, MinVal, MaxVal, Left, Top, Width, Label_Width)
	create_hfader(left,  LEFT_LEVEL_ID,  "Left (dB):" ,-600,600, 7, 12, 310, 110);
	create_hfader(right, RIGHT_LEVEL_ID, "Right (dB):",-600,600, 7, 48, 310, 110);
	left.slider.set_tic_freq(120);
	right.slider.set_tic_freq(120);

	// SHOW_CHECKBOX(CheckBox, Ctrl_ID, Caption, Left, Top, Width)
	create_checkbox(chkLock, LOCK_ID, "Lock", 30, 94, 80);

	controls_enabled = TRUE;

	sync(EVERYPM_ID);
}

void iAmpPluginDlg::sync(int ndx)
{
	char c[MAX_LABEL_STRING];
	kxparam_t v;
	switch (ndx)
	{
	case EVERYPM_ID: // update all controls
	
	case LOCK_ID:
		if (controls_enabled) chkLock.set_check((int)plugin->_params[LOCK_ID]);
		if (ndx != EVERYPM_ID) break;

	case RIGHT_LEVEL_ID:
		v = plugin->_params[RIGHT_LEVEL_ID];
		if (controls_enabled) 
		{
		 right.slider.set_pos((int)v); 
		 right.slider.redraw();
		} // update fader value only if function called from mixer
		sprintf(c, " %.1f", v/10.);
		right.svalue.SetWindowText(c); // show new value for fader
		if (ndx != EVERYPM_ID) break;
	case LEFT_LEVEL_ID: // update all controls
		v = plugin->_params[LEFT_LEVEL_ID];
		if (controls_enabled) 
		{
		 left.slider.set_pos((int)v); 
		 left.slider.redraw();
		} // update fader value only if function called from mixer
		sprintf(c, " %.1f", v/10.);
		left.svalue.SetWindowText(c); // show new value for fader
		// if (ndx != EVERYPM_ID) break;
	}
}
