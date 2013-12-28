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
#include "demo.h"
// effect source
#include "da_demo.cpp"

// plugin parameters
// (these are 'logical' parameters visible to the user)
typedef enum _params_id{
    VOL1_ID,
    VOL2_ID,
    CHECKBOX_ID, // these two are just examples
    COMBO_ID
};

// default plugin parameters
// -------------------------
static int default_params[DEMO_PARAMS_COUNT] = {0, 0, 0, 0};

int iDemoPlugin::set_defaults() 
{
    set_all_params(default_params); 
    return 0;
}

// for kX Automation: param description (see kxplugin.h for details on kx_fxparam_descr)
// -------------------------------------------------------------------------------------

static kx_fxparam_descr _param_descr[DEMO_PARAMS_COUNT] = {
    {"Level1", KX_FXPARAM_LEVEL, 0,    0x7fffffff},
    {"Level2", KX_FXPARAM_USER, -240, 0 },
    {"Mute",   KX_FXPARAM_SWITCH, 0,    1},
    {"Choice", KX_FXPARAM_USER, 0,    4 }
    };

int iDemoPlugin::describe_param(int ndx, kx_fxparam_descr *descr)
{
    memcpy(descr, &_param_descr[ndx], sizeof kx_fxparam_descr); return 0;
}

// presets
// -------

#define P (kxparam_t)

static kxparam_t presets[]=
{
 // don't include 'default' preset
 // P "preset_name", p1, p2, ... p(PARAMS_COUNT),
 // NULL
 P "-6 dB Right", 0,-60,0,0,
 P "-6 dB Left", 0x3fffffff,0,0,0,
 NULL // must be here
};

#undef P

const kxparam_t *iDemoPlugin::get_plugin_presets()
{
 return (kxparam_t *)presets;
}


/* Plugin Implemenation */

int iDemoPlugin::get_param(int ndx, kxparam_t *value) // ndx corresponds to 'logical' parameter id
{
    *value = _params[ndx]; 
    return 0;
}

// set_param sets 'logical' values by writing to (one or a group of) 'physical' registers

int iDemoPlugin::set_param(int ndx, kxparam_t value)
{
    _params[ndx] = value;

    switch (ndx)
    {
    case VOL1_ID:
        // vol1 parameter directly corresponds to the vol1 DSP register [0x8002]
        set_dsp_register(VOL1_P, value);
        break;

    case VOL2_ID:
        // vol2 requires special 'calculations'. an example ;)
        set_dsp_register(VOL2_P, (dword)cBtoG(value));
        break;

    case CHECKBOX_ID:
        // here we can do something. for instance, dynamically apply microcode changes
                // or anything else...
                // in the example we simply output a 'debug' string
                // 'value' is set to 1 if the checkbox is set
                debug("kX Demo: a checkbox was toggled [%d]\n",value);
        break;
    case COMBO_ID:
        // here we can change a set of parameters depending on the plugin logic
                // 'value' represents the selected item (0-first, 1-second etc..)
                debug("kX Demo: the user selected item %d\n",value);
        break;
    }

        // we need do synchronize the GUI part, if set_param was called by the mixer or kX Automation
    if (cp) ((iDemoPluginDlg*) cp)->sync(ndx);

    return 0;
}

int iDemoPlugin::set_all_params(kxparam_t* values)
{
    for (int i = 0; i < DEMO_PARAMS_COUNT; i++) set_param(i, values[i]);
    return 0;
}

int iDemoPlugin::request_microcode() 
{
    publish_microcode(demo);

    return 0;
}

const char *iDemoPlugin::get_plugin_description(int id)
{
    plugin_description(demo);
}


iKXPluginGUI *iDemoPlugin::create_cp(kDialog *parent, kFile *mf)
{
    iDemoPluginDlg *tmp;
    tmp = new iDemoPluginDlg(this, parent,mf);
    tmp->create();
    return tmp;
}

int iDemoPlugin::get_user_interface()
{
    return IKXPLUGIN_OWNER; // means 'custom' GUI panel
}


/* Plugin UI Implementation */

void iDemoPluginDlg::init()
{
    CKXPluginGUI::init(); // this is necessary

    // custom dialog initialization here...
    // (title, Width, Height)
    create_dialog(plugin_name(demo), 150, 250);

    // vertical fader (Fader, Ctrl_ID, Label, MinVal, MaxVal, Left, Top, Width, Height)
    create_vfader(level_2, VOL2_ID, "Right", -240, 0,   15, 35+30+10, 50, 170);

        // 'slider' is the same, as 'fader', but doesn't contain 'automatic' label and value parts
    create_hslider(level_1, VOL1_ID, "Left", 0x0, 0x7fffffff, 55, 35, 170, 30);

        // you can add any label; the ID should be >PARAM_COUNT
        create_label(label,DEMO_PARAMS_COUNT+1,"Label",55,15,100);

    // (CheckBox, Ctrl_ID, Caption, Left, Top, Width)
    create_checkbox(checkbox, CHECKBOX_ID, "Mute", 100, 100, 50);

        // (kCombo &combo,UINT ctrl_id,const char *label,int x,int y,int wd,int n_items,int dropped_width)
    create_combo(combo,COMBO_ID,"Sample",100,140,100,5,80);

    combo.add_string("Selection 1");
    combo.add_string("Selection 2");
    combo.add_string("Selection 3");
    combo.add_string("Selection 4");
    combo.add_string("Selection 5");

    controls_enabled = 1; // enable updating GUI when the parameters are changed

    sync(EVERYPM_ID); // update all 'GUI' elements to the preset plugin parameters
}

void iDemoPluginDlg::sync(int ndx)
{
    char c[MAX_LABEL_STRING];
    kxparam_t v;

        // this function should 'synchronize' any labels with the parameters

    switch (ndx)
    { 
    case EVERYPM_ID:
    case VOL1_ID:
        if(controls_enabled) { level_1.set_pos(plugin->_params[VOL1_ID]); }
        if (ndx != EVERYPM_ID) break;
                // fallthru

    case VOL2_ID:
        // here we sync the 'value' of the parameter
        v = plugin->_params[VOL2_ID];
        if (controls_enabled) { VSSETPOS(level_2.slider, v); level_2.slider.redraw(); }
        sprintf(c, "%.1fdB", v/10.);
        level_2.svalue.SetWindowText(c);
        if (ndx != EVERYPM_ID) break;
        // fallthru

    case CHECKBOX_ID:
        if(controls_enabled) checkbox.set_check(plugin->_params[CHECKBOX_ID]);
        if (ndx != EVERYPM_ID) break;
                // fallthru

    case COMBO_ID:
        if(controls_enabled) combo.set_selection(plugin->_params[COMBO_ID]);
        if (ndx != EVERYPM_ID) break;
        // fallthru
    }
}
