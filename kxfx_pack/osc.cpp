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

// effect class
#include "osc.h"
// effect source
#include "da_osc.cpp"

// plugin parameters
// (these are 'logical' parameters visible to the user)
typedef enum _params_id{
	RES_ID,
	TURN_ID,
	FADER_ID
	

};

// default plugin parameters
// -------------------------
static kxparam_t default_params[OSC_PARAMS_COUNT] = {0, 0, 20};

int iOscPlugin::set_defaults() 
{
	set_all_params(default_params); 
	return 0;
}

// for kX Automation: param description (see kxplugin.h for details on kx_fxparam_descr)
// -------------------------------------------------------------------------------------

static kx_fxparam_descr _param_descr[OSC_PARAMS_COUNT] = {
	{"Resolution",   KX_FXPARAM_SWITCH, 0,    1},
	{"Stop",   KX_FXPARAM_SWITCH, 0,    1}

	};

int iOscPlugin::describe_param(int ndx, kx_fxparam_descr *descr)
{
	memcpy(descr, &_param_descr[ndx], sizeof kx_fxparam_descr); return 0;
}

// presets
// -------

#define P (kxparam_t)

static kxparam_t presets[]=
{
NULL 
};

#undef P

const kxparam_t *iOscPlugin::get_plugin_presets()
{
 return presets;
}


/* Plugin Implemenation */

int iOscPlugin::get_param(int ndx, kxparam_t *value) // ndx corresponds to 'logical' parameter id
{
	*value = _params[ndx]; 
	return 0;
}


int iOscPlugin::set_param(int ndx, kxparam_t value)
{
	_params[ndx] = value;


        // we need do synchronize the GUI part, if set_param was called by the mixer or kX Automation
	if (cp) ((iOscPluginDlg*) cp)->sync(ndx);

	return 0;
}
int iOscPlugin::set_all_params(kxparam_t* values)
{
	for (int i = 0; i < OSC_PARAMS_COUNT; i++) set_param(i, values[i]);
	return 0;
}


int iOscPlugin::request_microcode() 
{
	publish_microcode(osc);

	return 0;
}

const char *iOscPlugin::get_plugin_description(int id)
{
	plugin_description(osc);
}


iKXPluginGUI *iOscPlugin::create_cp(kDialog *parent, kFile *mf)
{
	iOscPluginDlg *tmp;
	tmp = new iOscPluginDlg(this, parent,mf);
	tmp->create();
	return tmp;
}

int iOscPlugin::get_user_interface()
{
	return IKXPLUGIN_OWNER; // means 'custom' GUI panel
}

BEGIN_MESSAGE_MAP(iOscPluginDlg, CKXPluginGUI)
     ON_WM_TIMER()
END_MESSAGE_MAP()

iOscPluginDlg::iOscPluginDlg(iOscPlugin *plg, kDialog *parent_,kFile *mf_) 
 : CKXPluginGUI(parent_,mf_) 
{ 
	  plugin=plg;
      timer_id=(UINT)-1;
}

iOscPluginDlg::~iOscPluginDlg()
{
		
         	turn_on(0, 0);
}

int iOscPluginDlg::turn_on(int what, int res)
{
 if((what==1) && (timer_id==-1))
 {
  timer_id=SetTimer(4321+plugin->pgm_id,res,NULL);
  						// FIXME: timer ID should be unique
  						// perhaps, we will need a better code 
  						// to handle this
  if(timer_id==0)
   timer_id=(UINT_PTR)-1;
 }
 else
 {
	if(timer_id!=(UINT_PTR)-1)
	{
	 KillTimer(timer_id);
	 timer_id=(UINT_PTR)-1;
	}
 }
 return 0;
}



/* Plugin UI Implementation */

void iOscPluginDlg::init()
{
	CKXPluginGUI::init(); // this is necessary

	// custom dialog initialization here...
	// (title, Width, Height)
	create_dialog("osc", 480, 200, -1 , KXFX_NO_RESIZE);
	grpbox.destroy();
	b_reset.destroy();
	b_mute.destroy();
	b_bypass.destroy();
	preset.destroy();
    

	// (CheckBox, Ctrl_ID, Caption, Left, Top, Width)
	//create_checkbox(res, RES_ID, "Resolution", 10, 206, 100);
	//create_checkbox(turn, TURN_ID, "Stop", 110, 206, 100);

	// vertical fader (Fader, Ctrl_ID, Label, MinVal, MaxVal, Left, Top, Width, Height)
	create_hfader(fader, FADER_ID, "Res", 1, 200,   20, 202, 400, 30);
	

  CRect rect(2,25,480,220);//300,100
    
	
  scope.Create(WS_VISIBLE | WS_CHILD, rect, this) ; 
  scope.SetRange(-1.0, 1.0, 1) ;
  scope.SetYUnits("") ;
  scope.SetXUnits("") ;
  scope.SetBackgroundColor(RGB(0, 0, 64)) ;
  scope.SetGridColor(RGB(192, 192, 255)) ;
  scope.SetPlotColor(RGB(255, 255, 255)) ;




    
    turn_on(1, (int)plugin->_params[FADER_ID]);
    
	controls_enabled = 1; // enable updating GUI when the parameters are changed

	sync(EVERYPM_ID); // update all 'GUI' elements to the preset plugin parameters
}



void iOscPluginDlg::sync(int ndx)
{
	char c[MAX_LABEL_STRING];
	kxparam_t v;

        // this function should 'synchronize' any labels with the parameters

	switch (ndx)
	{ 
	case EVERYPM_ID:
    /*
	case RES_ID:
		v = plugin->_params[RES_ID];
		if(controls_enabled) res.set_check(plugin->_params[RES_ID]);
		
		if(plugin->_params[TURN_ID]==0) {
			if(v==0) {turn_on(0, 0); turn_on(1, 20);}
			else {turn_on(0, 0); turn_on(1, 100);}
		}
		if (ndx != EVERYPM_ID) 
	    break;

	case TURN_ID:
		v = plugin->_params[TURN_ID];
		if(controls_enabled) turn.set_check(plugin->_params[TURN_ID]);
		
			if(v==0) {
				if(plugin->_params[RES_ID]==0) {turn_on(0, 0); turn_on(1, 20);}
				else {turn_on(0, 0); turn_on(1, 100);}
			}
		    else {turn_on(0, 0);}
			
			if (ndx != EVERYPM_ID) 
		    break;
	*/

	case FADER_ID:
		// here we sync the 'value' of the parameter
		v = plugin->_params[FADER_ID];
		if (controls_enabled) { 
			VSSETPOS(fader.slider, (int)v); 
			fader.slider.redraw();
			
		
		}
		sprintf(c, "%ims", v);
		fader.svalue.SetWindowText(c);
		turn_on(0, 0); turn_on(1, (int)v);
		if (ndx != EVERYPM_ID) break;
		// fallthru
	}
}



void iOscPluginDlg::OnTimer(UINT_PTR)	
{
 
	
	dword value;
    plugin->get_dsp_register("in1",&value);
	
	
	double x = (double)(long)(value-0.25)/2147483647.5f;
	x *= 4;
    scope.AppendPoint(x);
	
	
}

void iOscPluginDlg::on_destroy()
{
 turn_on(0, 0);
}


