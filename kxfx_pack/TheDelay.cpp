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
#include "TheDelay.h"
// effect source
#include "da_TheDelay.cpp"


// plugin parameters
// (these are 'logical' parameters visible to the user)
typedef enum _params_id{
    DT_ID,
};


// default plugin parameters
// -------------------------
static kxparam_t default_params[THEDELAY_PARAMS_COUNT] = {25};
int iTheDelayPlugin::set_defaults() 
{
    set_all_params(default_params); 
    return 0;
}

// for kX Automation: param description (see kxplugin.h for details on kx_fxparam_descr)
// -------------------------------------------------------------------------------------

static kx_fxparam_descr _param_descr[THEDELAY_PARAMS_COUNT] = {
    {"DelayTime", KX_FXPARAM_LEVEL, 0x19,    0x5DC0}
    };

int iTheDelayPlugin::describe_param(int ndx, kx_fxparam_descr *descr)
{
    memcpy(descr, &_param_descr[ndx], sizeof kx_fxparam_descr); return 0;
}

// presets
// -------

#define P (kxparam_t)

static kxparam_t presets[]=
{
 NULL // must be here
};

#undef P

const kxparam_t *iTheDelayPlugin::get_plugin_presets()
{
 return presets;
}


/* Plugin Implemenation */

int iTheDelayPlugin::get_param(int ndx, kxparam_t *value) // ndx corresponds to 'logical' parameter id
{
    *value = _params[ndx]; 
    return 0;
}

// set_param sets 'logical' values by writing to (one or a group of) 'physical' registers

int iTheDelayPlugin::set_param(int ndx, kxparam_t value)
{
    kxparam_t t;
    _params[ndx] = value;

    switch (ndx)
    {
    case DT_ID:
        get_tram_addr(DW, (dword *) &t);
        set_tram_addr(DR, (dword)(t + value));
        
    break;
    }

    // we need do synchronize the GUI part, if set_param was called by the mixer or kX Automation
    if (cp) ((iTheDelayPluginDlg*) cp)->sync(ndx);

    return 0;
}

int iTheDelayPlugin::set_all_params(kxparam_t* values)
{
    for (int i = 0; i < THEDELAY_PARAMS_COUNT; i++) set_param(i, values[i]);
    return 0;
}

int iTheDelayPlugin::request_microcode() 
{
    publish_microcode(TheDelay);

    return 0;
}

const char *iTheDelayPlugin::get_plugin_description(int id)
{
    plugin_description(TheDelay);
}


iKXPluginGUI *iTheDelayPlugin::create_cp(kDialog *parent, kFile *mf)
{
    iTheDelayPluginDlg *tmp;
    tmp = new iTheDelayPluginDlg(this, parent,mf);
    tmp->create();
    return tmp;
}

int iTheDelayPlugin::get_user_interface()
{
    return IKXPLUGIN_OWNER; // means 'custom' GUI panel
}


/* Plugin UI Implementation */

void iTheDelayPluginDlg::init()
{
    CKXPluginGUI::init(); // this is necessary
    
    
    // dialog initialization
    create_dialog(plugin_name(TheDelay), 500, 80);

    // slider
    create_hslider(dt_value, DT_ID, "Left", 25, 24000, 6, 10, 480, 30);
    
    
    // labels
    create_label(lblsamples,THEDELAY_PARAMS_COUNT+1,"The Delay is : samples",10,40,480);
    create_label(lblms,THEDELAY_PARAMS_COUNT+2,"The Delay is : ms",10,60,480);


    controls_enabled = 1; // enable updating GUI when the parameters are changed

    sync(EVERYPM_ID); // update all 'GUI' elements to the preset plugin parameters
}

void iTheDelayPluginDlg::sync(int ndx)
{
    char s1[MAX_LABEL_STRING];
    char s2[MAX_LABEL_STRING];

    // this function should 'synchronize' any labels with the parameters
    switch (ndx)
    { 
    case EVERYPM_ID:
    case DT_ID:
        if(controls_enabled) { dt_value.set_pos((int)plugin->_params[DT_ID]); }

        sprintf(s1,"The Delay is : %d samples",(int)plugin->_params[DT_ID]);
        sprintf(s2,"The Delay is : %.10f miliseconds",plugin->_params[DT_ID]/48.0f);
        
        lblsamples.SetWindowText(s1);       
        lblms.SetWindowText(s2);        
                    
        if (ndx != EVERYPM_ID) break;
                // fallthru
    }
}

