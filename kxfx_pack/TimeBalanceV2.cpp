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
#include "TimeBalanceV2.h"
// effect source
#include "da_TimeBalanceV2.cpp"

inline kxparam_t kabs(kxparam_t v)
{
        if(v<0) return -v; else return v;
}

static void dbg(const char *format, ...);

// Used to write to file
#include <fstream>

#define BTNSAVE 0

#include <winuser.h>
#define OBM_CHECK 32760
// plugin parameters
// (these are 'logical' parameters visible to the user)
typedef enum _params_id{
    FLX_ID, // = 0
    FLY_ID,
    FLZ_ID,
    FRX_ID,
    FRY_ID,
    FRZ_ID,
    RLX_ID,
    RLY_ID,
    RLZ_ID,
    RRX_ID,
    RRY_ID,
    RRZ_ID,
    CX_ID,
    CY_ID,
    CZ_ID,
    LX_ID,
    LY_ID,
    LZ_ID,
    FLDIST_ID,
    FRDIST_ID,
    RLDIST_ID,
    RRDIST_ID,
    CDIST_ID,
    COMBOUNITS_ID,
    COMBOMODE_ID,
    COMBOREFERENCE_ID,
    TEMPERATURE_ID
};

enum _extras_id{
    LBLDISTL_FL_ID=75,
    LBLDISTL_FR_ID,
    LBLDISTL_RL_ID,
    LBLDISTL_RR_ID,
    LBLDISTL_C_ID,

    LBLDISTFL_RL_ID,
    LBLDISTFR_RR_ID,
    LBLDISTFL_FR_ID,
    LBLDISTRL_RR_ID,
    LBLDISTFL_C_ID,
    LBLDISTFR_C_ID,

    LBLHEADER_ID,

    LBLSPKFL_ID,
    LBLSPKFR_ID,
    LBLSPKRL_ID,
    LBLSPKRR_ID,
    LBLSPKC_ID,
    LBLSPKL_ID  
};


// default plugin parameters
// -------------------------
static kxparam_t default_params[TIMEBALANCE_PARAMS_COUNT] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4, 200, 2, 2, 2, 2, 2, 1, 0, 0};
int iTimeBalanceV2Plugin::set_defaults() 
{
    for (int i = 0; i < TIMEBALANCE_PARAMS_COUNT; i++) 
        _params[i] = default_params[i];

    set_all_params(default_params); 
    return 0;
}

// for kX Automation: param description (see kxplugin.h for details on kx_fxparam_descr)
// -------------------------------------------------------------------------------------

static kx_fxparam_descr _param_descr[TIMEBALANCE_PARAMS_COUNT] = {
    {0}
    };

int iTimeBalanceV2Plugin::describe_param(int ndx, kx_fxparam_descr *descr)
{
    memcpy(descr, &_param_descr[ndx], sizeof kx_fxparam_descr); return 0;
}

// presets
// -------

#define P (kxparam_t)

static kxparam_t presets[]=
{
 P "5 m by 5 m with listener in the middle", -2500, 2500, 0, 2500, 2500, 0, -2500, -2500, 0, 2500, -2500, 0, 0, 2500, 0, 0, 0, 0, 3536, 3536, 3536, 3536, 2500,  0, 1, 4, 200, 2, 2, 2, 2, 146, 1, 0, 0,
 NULL // must be here
};

#undef P

const kxparam_t *iTimeBalanceV2Plugin::get_plugin_presets()
{
 return presets;
}


/* Plugin Implemenation */

int iTimeBalanceV2Plugin::get_param(int ndx, kxparam_t *value) // ndx corresponds to 'logical' parameter id
{
    *value = _params[ndx]; 
    return 0;
}


/* Calculates several parameters based on the actual values of the 'user controled' parameters
and set them into the DANE code */
int iTimeBalanceV2Plugin::general_parameters()
{
    kxparam_t t;

    // Calculate the 5 delays
    kxparam_t DelayFL = 0;
    kxparam_t DelayFR = 0;
    kxparam_t DelayRL = 0;
    kxparam_t DelayRR = 0;
    kxparam_t DelayC  = 0;

    kxparam_t DistL_FL;
    kxparam_t DistL_FR;
    kxparam_t DistL_RL;
    kxparam_t DistL_RR;
    kxparam_t DistL_C;

    kxparam_t DistL[5];

    kxparam_t MaxDelay;

    double SoS; // Speed of Sound

    // Calculate the Speed of Sound
    SoS = sqrt(1.402 * 287.05 * (_params[TEMPERATURE_ID]/10 + 273.15 ));
    

    if (_params[TEMPERATURE_ID+6] == 0) // Basic mode
    {
        DistL_FL = _params[FLDIST_ID];
        DistL_FR = _params[FRDIST_ID];
        DistL_RL = _params[RLDIST_ID];
        DistL_RR = _params[RRDIST_ID];
        DistL_C = _params[CDIST_ID];
    }
    else if (_params[TEMPERATURE_ID+6] == 1) // Advanced mode
    {
        DistL_FL = (int)sqrt( (double)kabs(_params[FLX_ID] - _params[LX_ID])*kabs(_params[FLX_ID] - _params[LX_ID]) + kabs(_params[FLY_ID] - _params[LY_ID])*kabs(_params[FLY_ID] - _params[LY_ID]) + kabs(_params[FLZ_ID] - _params[LZ_ID])*kabs(_params[FLZ_ID] - _params[LZ_ID]) );
        DistL_FR = (int)sqrt( (double)kabs(_params[FRX_ID] - _params[LX_ID])*kabs(_params[FRX_ID] - _params[LX_ID]) + kabs(_params[FRY_ID] - _params[LY_ID])*kabs(_params[FRY_ID] - _params[LY_ID]) + kabs(_params[FRZ_ID] - _params[LZ_ID])*kabs(_params[FRZ_ID] - _params[LZ_ID]) );
        DistL_RL = (int)sqrt( (double)kabs(_params[RLX_ID] - _params[LX_ID])*kabs(_params[RLX_ID] - _params[LX_ID]) + kabs(_params[RLY_ID] - _params[LY_ID])*kabs(_params[RLY_ID] - _params[LY_ID]) + kabs(_params[RLZ_ID] - _params[LZ_ID])*kabs(_params[RLZ_ID] - _params[LZ_ID]) );
        DistL_RR = (int)sqrt( (double)kabs(_params[RRX_ID] - _params[LX_ID])*kabs(_params[RRX_ID] - _params[LX_ID]) + kabs(_params[RRY_ID] - _params[LY_ID])*kabs(_params[RRY_ID] - _params[LY_ID]) + kabs(_params[RRZ_ID] - _params[LZ_ID])*kabs(_params[RRZ_ID] - _params[LZ_ID]) );
        DistL_C = (int)sqrt( (double)kabs(_params[CX_ID] - _params[LX_ID])*kabs(_params[CX_ID] - _params[LX_ID]) + kabs(_params[CY_ID] - _params[LY_ID])*kabs(_params[CY_ID] - _params[LY_ID]) + kabs(_params[CZ_ID] - _params[LZ_ID])*kabs(_params[CZ_ID] - _params[LZ_ID]) );
    }

    DistL[0] = DistL_FL;
    DistL[1] = DistL_FR;
    DistL[2] = DistL_RL;
    DistL[3] = DistL_RR;
    DistL[4] = DistL_C;

    // Find the bigger distance using bubble sort
    // DistL[4] is the biggest
    for ( int pass = 0; pass < 4; pass++)
    {
        for ( int j = 0; j < 4; j++)
        {
            if ( DistL[j] > DistL[j + 1] )
            {
                t = DistL[j];
                DistL[j] = DistL[j + 1];
                DistL[j + 1] = t;
            } // end if
        } // end for
    }
    MaxDelay = (int)((double)DistL[4] * 48000 / SoS / 1000);

    DelayFL = (int)((double)MaxDelay - (double)DistL_FL * 48000 / SoS / 1000) + 2;
    DelayFR = (int)((double)MaxDelay - (double)DistL_FR * 48000 / SoS / 1000) + 2;
    DelayRL = (int)((double)MaxDelay - (double)DistL_RL * 48000 / SoS / 1000) + 2;
    DelayRR = (int)((double)MaxDelay - (double)DistL_RR * 48000 / SoS / 1000) + 2;
    DelayC = (int)((double)MaxDelay - (double)DistL_C * 48000 / SoS / 1000) + 2;
    
    // Save 'logical' values
    // It is necessary for the "Save to TimeBalanceV2 light" function
    _params[TEMPERATURE_ID+1] = DelayFL;
    _params[TEMPERATURE_ID+2] = DelayFR;
    _params[TEMPERATURE_ID+3] = DelayRL;
    _params[TEMPERATURE_ID+4] = DelayRR;
    _params[TEMPERATURE_ID+5] = DelayC;

    // Set the Tram
    t = 0;
    set_tram_addr(DWFL, (dword)(t));
    t = t + DelayFL;
    set_tram_addr(DRFL, (dword)(t));
    
    t = t + 1;
    set_tram_addr(DWFR, (dword)(t));
    t = t + DelayFR;
    set_tram_addr(DRFR, (dword)(t));

    t = t + 1;
    set_tram_addr(DWRL, (dword)(t));
    t = t + DelayRL;
    set_tram_addr(DRRL, (dword)(t));

    t = t + 1;
    set_tram_addr(DWRR, (dword)(t));
    t = t + DelayRR;
    set_tram_addr(DRRR, (dword)(t));

    t = t + 1;
    set_tram_addr(DWC, (dword)(t));
    t = t + DelayC;
    set_tram_addr(DRC, (dword)(t));
    
    return 0;
}

// set_param sets 'logical' values by writing to (one or a group of) 'physical' registers

int iTimeBalanceV2Plugin::set_param(int ndx, kxparam_t value)
{
    //kxparam_t t;
    _params[ndx] = value;


    switch (ndx)
    {
    case FLX_ID:
        general_parameters();
        
        break;
    
    case FLY_ID:
        general_parameters();
        
        break;
    
    case FLZ_ID:
        general_parameters();
        
        break;

    case FRX_ID:
        general_parameters();       

        break;
    
    case FRY_ID:
        general_parameters();       

        break;

    case FRZ_ID:
        general_parameters();
        
        break;
    
    case RLX_ID:
        general_parameters();       

        break;
    
    case RLY_ID:
        general_parameters();       

        break;

    case RLZ_ID:
        general_parameters();
        
        break;
    
    case RRX_ID:
        general_parameters();       

        break;
    
    case RRY_ID:
        general_parameters();
        
        break;

    case RRZ_ID:
        general_parameters();
        
        break;
    
    case CX_ID:
        general_parameters();       

        break;
    
    case CY_ID:
        general_parameters();
        
        break;

    case CZ_ID:
        general_parameters();
        
        break;
    
    case LX_ID:
        general_parameters();       

        break;
    
    case LY_ID:
        general_parameters();
        
        break;

    case LZ_ID:
        general_parameters();
        
        break;
    
    case FLDIST_ID:
        general_parameters();
        
        break;

    case FRDIST_ID:
        general_parameters();
        
        break;

    case RLDIST_ID:
        general_parameters();
        
        break;

    case RRDIST_ID:
        general_parameters();
        
        break;

    case CDIST_ID:
        general_parameters();
        
        break;

    case COMBOUNITS_ID:
        general_parameters();
        
        break;
    
    case COMBOMODE_ID:
        
        if (cp) ((iTimeBalanceV2PluginDlg*) cp)->message_display();

        break;
        
    case COMBOREFERENCE_ID:
        
        switch (_params[ndx])
        {
        case 0: // FL
            _params[33] = 2500;
            _params[34] = -2500;
            break;

        case 1: // FR
            _params[33] = -2500;
            _params[34] = -2500;
            break;

        case 2: // RL
            _params[33] = 2500;
            _params[34] = 2500;
            break;

        case 3: // RR
            _params[33] = -2500;
            _params[34] = 2500;
            break;

        case 4: // C
            _params[33] = 0;
            _params[34] = 0;
            break;
        }

        break;

    case TEMPERATURE_ID:
        general_parameters();
        
        break;
    
    }
    
    // we need do synchronize the GUI part, if set_param was called by the mixer or kX Automation
    if (cp) ((iTimeBalanceV2PluginDlg*) cp)->sync(ndx);

    return 0;
}

int iTimeBalanceV2Plugin::set_all_params(kxparam_t* values)
{
    for (int i = 0; i < TIMEBALANCE_PARAMS_COUNT; i++) set_param(i, values[i]);
    return 0;
}

int iTimeBalanceV2Plugin::request_microcode() 
{
    publish_microcode(TimeBalanceV2);
    return 0;
}

const char *iTimeBalanceV2Plugin::get_plugin_description(int id)
{
    plugin_description(TimeBalanceV2);
}


iKXPluginGUI *iTimeBalanceV2Plugin::create_cp(kDialog *parent, kFile *mf)
{
    iTimeBalanceV2PluginDlg *tmp;
    tmp = new iTimeBalanceV2PluginDlg(this, parent,mf);
    tmp->create();
    return tmp;
}


int iTimeBalanceV2Plugin::get_user_interface()
{
    return IKXPLUGIN_OWNER; // means 'custom' GUI panel
}


/* Plugin UI Implementation */

void iTimeBalanceV2PluginDlg::init()
{
    char tmp[MAX_LABEL_STRING];

    load_skin((int)plugin->_params[COMBOMODE_ID]);

    CKXPluginGUI::init(); // this is necessary
    // init() should be called -after- skin initialization 
    
    // dialog initialization
    // (title, Width, Height)

    create_dialog(plugin_name(TimeBalanceV2), 1000, 600);
    grpbox.hide();//hide useless GUI elements
    
    // create font  
    
    defaultfont.CreatePointFont(100,"Arial");

    //Colors 

    color_bkg_values = RGB(160,160,160); 
    color_bkg_values2 = RGB(212,208,200);
    
    color_txt_values = RGB(0,0,0);
    color_txt_listener = RGB(0,0,255);
    color_txt_speakers = RGB(255,255,0);


    // Header

    w_label.hide();//remove original header   
    lblHeader.Create("    ",WS_CHILD | WS_VISIBLE | SS_LEFT ,CRect(CPoint(6,4),CSize(400,16)), this, LBLHEADER_ID);
    lblHeader.SetBkColor(RGB(49,93,165));  
    lblHeader.SetTextColor(RGB(255,255,255));
    lblHeader.SetFont(&defaultfont,NULL); 
    sprintf(tmp, "TimeBalanceV2 - %s",  get_plugin()->ikx->get_device_name());
    lblHeader.SetWindowText(tmp);

    //Labels

    lblDistL_FL.Create("    ",WS_CHILD | WS_VISIBLE | SS_CENTER ,CRect(CPoint(245,317),CSize(75,16)), this, LBLDISTL_FL_ID);
    lblDistL_FL.SetBkColor(color_bkg_values);
    lblDistL_FL.SetTextColor(color_txt_listener);
    lblDistL_FL.SetFont(&defaultfont,NULL);  

    lblDistL_FR.Create("    ",WS_CHILD | WS_VISIBLE | SS_CENTER ,CRect(CPoint(470,317),CSize(75,16)), this, LBLDISTL_FR_ID);
    lblDistL_FR.SetBkColor(color_bkg_values);
    lblDistL_FR.SetTextColor(color_txt_listener);
    lblDistL_FR.SetFont(&defaultfont,NULL);  

    lblDistL_RL.Create("    ",WS_CHILD | WS_VISIBLE | SS_CENTER ,CRect(CPoint(245,471),CSize(75,16)), this, LBLDISTL_RL_ID);
    lblDistL_RL.SetBkColor(color_bkg_values);
    lblDistL_RL.SetTextColor(color_txt_listener);
    lblDistL_RL.SetFont(&defaultfont,NULL);  

    lblDistL_RR.Create("    ",WS_CHILD | WS_VISIBLE | SS_CENTER ,CRect(CPoint(470,471),CSize(75,16)), this, LBLDISTL_RR_ID);
    lblDistL_RR.SetBkColor(color_bkg_values);
    lblDistL_RR.SetTextColor(color_txt_listener);
    lblDistL_RR.SetFont(&defaultfont,NULL);  

    lblDistL_C.Create("    ",WS_CHILD | WS_VISIBLE | SS_CENTER ,CRect(CPoint(357,317),CSize(75,16)), this, LBLDISTL_C_ID);
    lblDistL_C.SetBkColor(color_bkg_values);
    lblDistL_C.SetTextColor(color_txt_listener);
    lblDistL_C.SetFont(&defaultfont,NULL);  

    // slider
    // vertical fader (Fader, Ctrl_ID, Label, MinVal, MaxVal, Left, Top, Width, Height)
    // 'slider' is the same, as 'fader', but doesn't contain 'automatic' label and value parts

    create_hslider(FLx, FLX_ID, "Front Left x", -2500, 2500, 10, 167, 256, 16);
    create_hslider(FLy, FLY_ID, "Front Left y", -2500, 2500, 10, 207, 256, 16);
    create_hslider(FLz, FLZ_ID, "Front Left z", -1000, 1000, 10, 247, 256, 16);

    create_hslider(FRx, FRX_ID, "Front Right x", -2500, 2500, 524, 167, 256, 16);
    create_hslider(FRy, FRY_ID, "Front Right y", -2500, 2500, 524, 207, 256, 16);
    create_hslider(FRz, FRZ_ID, "Front Right z", -1000, 1000, 524, 247, 256, 16);

    create_hslider(RLx, RLX_ID, "Rear Left x", -2500, 2500, 10, 470, 256, 16);
    create_hslider(RLy, RLY_ID, "Rear Left y", -2500, 2500, 10, 510, 256, 16);
    create_hslider(RLz, RLZ_ID, "Rear Left z", -1000, 1000, 10, 550, 256, 16);

    create_hslider(RRx, RRX_ID, "Rear Right x", -2500, 2500, 524, 470, 256, 16);
    create_hslider(RRy, RRY_ID, "Rear Right y", -2500, 2500, 524, 510, 256, 16);
    create_hslider(RRz, RRZ_ID, "Rear Right z", -1000, 1000, 524, 550, 256, 16);

    create_hslider(Cx, CX_ID, "Center x", -2500, 2500, 267, 15, 256, 16);
    create_hslider(Cy, CY_ID, "Center y", -2500, 2500, 267, 55, 256, 16);
    create_hslider(Cz, CZ_ID, "Center z", -1000, 1000, 267, 95, 256, 16);

    create_hslider(Lx, LX_ID, "Listener x", -2500, 2500, 267, 319, 256, 16);
    create_hslider(Ly, LY_ID, "Listener y", -2500, 2500, 267, 359, 256, 16);
    create_hslider(Lz, LZ_ID, "Listener z", -1000, 1000, 267, 399, 256, 16);
    
    // Basic Sliders

    create_hslider(FLDist, FLDIST_ID, "Distance betweeen front left speaker and listener", 0x0, 0xE5A, 344, 100, 256, 16);
    create_hslider(FRDist, FRDIST_ID, "Distance betweeen front right speaker and listener", 0x0, 0xE5A, 344, 140, 256, 16);
    create_hslider(RLDist, RLDIST_ID, "Distance betweeen rear left speaker and listener", 0x0, 0xE5A, 344, 180, 256, 16);
    create_hslider(RRDist, RRDIST_ID, "Distance betweeen rear right speaker and listener", 0x0, 0xE5A, 344, 220, 256, 16);
    create_hslider(CDist, CDIST_ID, "Distance betweeen center speaker and listener", 0x0, 0xE5A, 344, 260, 256, 16);

    create_hslider(Temperature, TEMPERATURE_ID, "Air Temperature", 0x64, 0x1F4, 605, 17, 175, 16);

    create_button(btnSave,BTNSAVE, "Save to \"TimeBalanceV2 light\"",10,17,180);
    
    // labels

    create_label(lblFLx,TIMEBALANCE_PARAMS_COUNT+3,"x = 0.000 m",95,187,85);
    create_label(lblFLy,TIMEBALANCE_PARAMS_COUNT+4,"y = 0.000 m",95,227,85);
    create_label(lblFLz,TIMEBALANCE_PARAMS_COUNT+5,"y = 0.000 m",95,267,85);

    create_label(lblFRx,TIMEBALANCE_PARAMS_COUNT+6,"x = 0.000 m",609,187,85);
    create_label(lblFRy,TIMEBALANCE_PARAMS_COUNT+7,"y = 0.000 m",609,227,85);
    create_label(lblFRz,TIMEBALANCE_PARAMS_COUNT+8,"y = 0.000 m",609,267,85);

    create_label(lblRLx,TIMEBALANCE_PARAMS_COUNT+9,"x = 0.000 m",95,490,85);
    create_label(lblRLy,TIMEBALANCE_PARAMS_COUNT+10,"y = 0.000 m",95,530,85);
    create_label(lblRLz,TIMEBALANCE_PARAMS_COUNT+11,"y = 0.000 m",95,570,85);

    create_label(lblRRx,TIMEBALANCE_PARAMS_COUNT+12,"x = 0.000 m",609,490,85);
    create_label(lblRRy,TIMEBALANCE_PARAMS_COUNT+13,"y = 0.000 m",609,530,85);
    create_label(lblRRz,TIMEBALANCE_PARAMS_COUNT+14,"y = 0.000 m",609,570,85);

    create_label(lblCx,TIMEBALANCE_PARAMS_COUNT+15,"x = 0.000 m",352,35,85);
    create_label(lblCy,TIMEBALANCE_PARAMS_COUNT+16,"y = 0.000 m",352,75,85);
    create_label(lblCz,TIMEBALANCE_PARAMS_COUNT+17,"y = 0.000 m",352,115,85);

    create_label(lblLx,TIMEBALANCE_PARAMS_COUNT+18,"x = 0.000 m",352,339,85);
    create_label(lblLy,TIMEBALANCE_PARAMS_COUNT+19,"y = 0.000 m",352,379,85);
    create_label(lblLz,TIMEBALANCE_PARAMS_COUNT+20,"y = 0.000 m",352,419,85);

    // Basic Mode labels

    create_label(lblDist,TIMEBALANCE_PARAMS_COUNT+21,"Distance",449,70,53);
    create_label(lblFL,TIMEBALANCE_PARAMS_COUNT+22,"Front Left : 0.000 m",197,100,140);
    create_label(lblFR,TIMEBALANCE_PARAMS_COUNT+23,"Front Right : 0.000 m",197,140,140);
    create_label(lblRL,TIMEBALANCE_PARAMS_COUNT+24,"Rear Left : 0.000 m",197,180,140);
    create_label(lblRR,TIMEBALANCE_PARAMS_COUNT+25,"Rear Right : 0.000 m",197,220,140);
    create_label(lblC,TIMEBALANCE_PARAMS_COUNT+26,"Center : 0.000 m",197,260,140);

    lblDistFL_RL.Create("    ",WS_CHILD | WS_VISIBLE | SS_CENTER ,CRect(CPoint(95,385),CSize(75,16)), this, LBLDISTFL_RL_ID);
    lblDistFL_RL.SetBkColor(color_bkg_values);
    lblDistFL_RL.SetTextColor(color_txt_speakers);
    lblDistFL_RL.SetFont(&defaultfont,NULL);  

    lblDistFR_RR.Create("    ",WS_CHILD | WS_VISIBLE | SS_CENTER ,CRect(CPoint(609,385),CSize(75,16)), this, LBLDISTFR_RR_ID);
    lblDistFR_RR.SetBkColor(color_bkg_values);
    lblDistFR_RR.SetTextColor(color_txt_speakers);
    lblDistFR_RR.SetFont(&defaultfont,NULL);  

    lblDistFL_FR.Create("    ",WS_CHILD | WS_VISIBLE | SS_CENTER ,CRect(CPoint(357,235),CSize(75,16)), this, LBLDISTFL_FR_ID);
    lblDistFL_FR.SetBkColor(color_bkg_values);
    lblDistFL_FR.SetTextColor(color_txt_speakers);
    lblDistFL_FR.SetFont(&defaultfont,NULL);  

    lblDistRL_RR.Create("    ",WS_CHILD | WS_VISIBLE | SS_CENTER ,CRect(CPoint(357,535),CSize(75,16)), this, LBLDISTRL_RR_ID);
    lblDistRL_RR.SetBkColor(color_bkg_values);
    lblDistRL_RR.SetTextColor(color_txt_speakers);
    lblDistRL_RR.SetFont(&defaultfont,NULL);  

    lblDistFL_C.Create("    ",WS_CHILD | WS_VISIBLE | SS_CENTER ,CRect(CPoint(180,167),CSize(75,16)), this, LBLDISTFL_C_ID);
    lblDistFL_C.SetBkColor(color_bkg_values);
    lblDistFL_C.SetTextColor(color_txt_speakers);
    lblDistFL_C.SetFont(&defaultfont,NULL);  

    lblDistFR_C.Create("    ",WS_CHILD | WS_VISIBLE | SS_CENTER ,CRect(CPoint(547,167),CSize(75,16)), this, LBLDISTFR_C_ID);
    lblDistFR_C.SetBkColor(color_bkg_values);
    lblDistFR_C.SetTextColor(color_txt_speakers);
    lblDistFR_C.SetFont(&defaultfont,NULL);  

    create_label(lblTemperature,TIMEBALANCE_PARAMS_COUNT+27,"20 °C",670,37,60);

    lblSpkFL.Create("FL",WS_CHILD | WS_VISIBLE | SS_CENTER ,CRect(CPoint(890,300),CSize(20,16)), this, LBLSPKFL_ID);
    lblSpkFL.SetBkColor(CLR_NONE);
    lblSpkFL.SetTextColor(color_txt_listener);
    lblSpkFL.SetFont(&defaultfont,NULL);  

    lblSpkFR.Create("FR",WS_CHILD | WS_VISIBLE | SS_CENTER ,CRect(CPoint(890,300),CSize(20,16)), this, LBLSPKFR_ID);
    lblSpkFR.SetBkColor(CLR_NONE);
    lblSpkFR.SetTextColor(color_txt_listener);
    lblSpkFR.SetFont(&defaultfont,NULL);  

    lblSpkRL.Create("RL",WS_CHILD | WS_VISIBLE | SS_CENTER ,CRect(CPoint(890,300),CSize(20,16)), this, LBLSPKRL_ID);
    lblSpkRL.SetBkColor(CLR_NONE);
    lblSpkRL.SetTextColor(color_txt_listener);
    lblSpkRL.SetFont(&defaultfont,NULL);  
    
    lblSpkRR.Create("RR",WS_CHILD | WS_VISIBLE | SS_CENTER ,CRect(CPoint(890,300),CSize(20,16)), this, LBLSPKRR_ID);
    lblSpkRR.SetBkColor(CLR_NONE);
    lblSpkRR.SetTextColor(color_txt_listener);
    lblSpkRR.SetFont(&defaultfont,NULL);  

    lblSpkC.Create("C",WS_CHILD | WS_VISIBLE | SS_CENTER ,CRect(CPoint(890,300),CSize(20,16)), this, LBLSPKC_ID);
    lblSpkC.SetBkColor(CLR_NONE);
    lblSpkC.SetTextColor(color_txt_listener);
    lblSpkC.SetFont(&defaultfont,NULL);  

    lblSpkL.Create("L",WS_CHILD | WS_VISIBLE | SS_CENTER ,CRect(CPoint(890,300),CSize(20,16)), this, LBLSPKL_ID);
    
    lblSpkL.SetBkColor(CLR_NONE);
    lblSpkL.SetTextColor(color_txt_listener);
    lblSpkL.SetFont(&defaultfont,NULL);  

    create_combo(ComboMode,COMBOMODE_ID,"Mode", 10, 40, 80, 2, 80);

    ComboMode.add_string("Basic");
    ComboMode.add_string("Advanced");

    ComboMode.set_selection((int)plugin->_params[COMBOMODE_ID]);
    plugin->_params[TEMPERATURE_ID+6] = ComboMode.get_selection();
    
    create_combo(ComboUnits,COMBOUNITS_ID,"Units", 10, 70, 70, 2, 70);
    ComboUnits.add_string("Metric");
    ComboUnits.add_string("Imperial");  
    
    create_combo(ComboReference,COMBOREFERENCE_ID,"Reference of x and y (where x = 0 and y = 0)", 10, 100, 60, 5, 60);
    ComboReference.add_string("FL");
    ComboReference.add_string("FR");
    ComboReference.add_string("RL");
    ComboReference.add_string("RR");
    ComboReference.add_string("C");
    
    change_mode();

    controls_enabled = 1; // enable updating GUI when the parameters are changed

    sync(EVERYPM_ID); // update all 'GUI' elements to the preset plugin parameters
}

int iTimeBalanceV2PluginDlg::load_skin(int skin_number)
{
    switch (skin_number)
    {
    case 0: // Basic Skin
        {
                TCHAR tmp_str[MAX_PATH];
                efx_skin.get_full_skin_path("kxTimeBalanceV2B.kxs",tmp_str);
        efx_skin.set_skin(tmp_str);
                mf.attach_skin(&efx_skin);
                efx_skin.attach_skin(kxmixer_skin);
                efx_skin.set_attach_priority(1);
        }

        break;

    case 1: // Advanced Skin
        {   
                TCHAR tmp_str[MAX_PATH];
                efx_skin.get_full_skin_path("kxTimeBalanceV2A.kxs",tmp_str);
        efx_skin.set_skin(tmp_str);
                mf.attach_skin(&efx_skin);
                efx_skin.attach_skin(kxmixer_skin);
                efx_skin.set_attach_priority(1);
        }
        break;  
    }

    return 0;
}

int iTimeBalanceV2PluginDlg::message_display()
{
    char tmp[MAX_LABEL_STRING];

    if (plugin->_params[COMBOMODE_ID] == plugin->_params[TEMPERATURE_ID+6] )
    {   
        sprintf(tmp, "TimeBalanceV2 - %s",  get_plugin()->ikx->get_device_name());
        lblHeader.SetWindowText(tmp);
    }
    else if (plugin->_params[COMBOMODE_ID] != plugin->_params[TEMPERATURE_ID+6])
    {
        sprintf(tmp, "TimeBalanceV2 - %s - Close and Reopen Window",  get_plugin()->ikx->get_device_name());
        lblHeader.SetWindowText(tmp);

        plugin->notify(IKX_CLOSE_AND_REOPEN);
    }

    return 0;
}

int iTimeBalanceV2PluginDlg::change_mode()
{
    switch (plugin->_params[TEMPERATURE_ID+6])
    {
    case 0: // Change to Basic
        // Hide Advanced controls 
            
        FLx.hide();
        FLy.hide();
        FLz.hide();
        FRx.hide();
        FRy.hide();
        FRz.hide();
        RLx.hide();
        RLy.hide();
        RLz.hide();
        RRx.hide();
        RRy.hide();
        RRz.hide();
        Cx.hide();
        Cy.hide();
        Cz.hide();
        Lx.hide();
        Ly.hide();
        Lz.hide();
        
        // Hide Advanced labels

        // CColorCtrl labels
        lblDistL_FL.ShowWindow(SW_HIDE);
        lblDistL_FR.ShowWindow(SW_HIDE);
        lblDistL_RL.ShowWindow(SW_HIDE);
        lblDistL_RR.ShowWindow(SW_HIDE);
        lblDistL_C.ShowWindow(SW_HIDE);
        lblDistFL_RL.ShowWindow(SW_HIDE);
        lblDistFR_RR.ShowWindow(SW_HIDE);
        lblDistFL_FR.ShowWindow(SW_HIDE);
        lblDistRL_RR.ShowWindow(SW_HIDE);
        lblDistFL_C.ShowWindow(SW_HIDE);
        lblDistFR_C.ShowWindow(SW_HIDE);
            
        // kStatic labels
        lblFLx.hide();
        lblFLy.hide();
        lblFLz.hide();
        lblFRx.hide();
        lblFRy.hide();
        lblFRz.hide();
        lblRLx.hide();
        lblRLy.hide();
        lblRLz.hide();
        lblRRx.hide();
        lblRRy.hide();
        lblRRz.hide();
        lblCx.hide();
        lblCy.hide();
        lblCz.hide();
        lblLx.hide();
        lblLy.hide();
        lblLz.hide();

        // Hide Advanced Combo Box
        ComboReference.hide();
            
        // Show Basic controls
        FLDist.show();
        FRDist.show();
        RLDist.show();
        RRDist.show();
        CDist.show();


        // Show Basic labels
        lblDist.show();
        lblFL.show();
        lblFR.show();
        lblRL.show();
        lblRR.show();
        lblC.show();

        break;


    case 1: // Change to Advanced
        // Hide Basic controls 
        FLDist.hide();
        FRDist.hide();
        RLDist.hide();
        RRDist.hide();
        CDist.hide();
                    
        // Hide Basic labels
        lblDist.hide();
        lblFL.hide();
        lblFR.hide();
        lblRL.hide();
        lblRR.hide();
        lblC.hide();

        // Show Advanced controls

        FLx.show();
        FLy.show();
        FLz.show();
        FRx.show();
        FRy.show();
        FRz.show();
        RLx.show();
        RLy.show();
        RLz.show();
        RRx.show();
        RRy.show();
        RRz.show();
        Cx.show();
        Cy.show();
        Cz.show();
        Lx.show();
        Ly.show();
        Lz.show();

        // Show Advanced labels

        // CColorCtrl labels
        lblDistL_FL.ShowWindow(SW_SHOW);
        lblDistL_FR.ShowWindow(SW_SHOW);
        lblDistL_RL.ShowWindow(SW_SHOW);
        lblDistL_RR.ShowWindow(SW_SHOW);
        lblDistL_C.ShowWindow(SW_SHOW);
        lblDistFL_RL.ShowWindow(SW_SHOW);
        lblDistFR_RR.ShowWindow(SW_SHOW);
        lblDistFL_FR.ShowWindow(SW_SHOW);
        lblDistRL_RR.ShowWindow(SW_SHOW);
        lblDistFL_C.ShowWindow(SW_SHOW);
        lblDistFR_C.ShowWindow(SW_SHOW);
            
        // kStatic labels
        lblFLx.show();
        lblFLy.show();
        lblFLz.show();
        lblFRx.show();
        lblFRy.show();
        lblFRz.show();
        lblRLx.show();
        lblRLy.show();
        lblRLz.show();
        lblRRx.show();
        lblRRy.show();
        lblRRz.show();
        lblCx.show();
        lblCy.show();
        lblCz.show();
        lblLx.show();
        lblLy.show();
        lblLz.show();

        // Show Advanced Combo Box
        ComboReference.show();

        break;

    }

    return 0;
}

void iTimeBalanceV2PluginDlg::sync(int ndx)
{
    ComboUnits.set_selection((int)plugin->_params[COMBOUNITS_ID]);
    ComboReference.set_selection((int)plugin->_params[COMBOREFERENCE_ID]);

    char s1[MAX_LABEL_STRING];

    switch (plugin->_params[COMBOMODE_ID])
    {
    case 0: // Basic

        if (controls_enabled) 
        { 
            FLDist.set_pos((int)plugin->_params[FLDIST_ID]);
            FRDist.set_pos((int)plugin->_params[FRDIST_ID]);
            RLDist.set_pos((int)plugin->_params[RLDIST_ID]);
            RRDist.set_pos((int)plugin->_params[RRDIST_ID]);
            CDist.set_pos((int)plugin->_params[CDIST_ID]);
            
            Temperature.set_pos((int)plugin->_params[TEMPERATURE_ID]);
        }

        switch (plugin->_params[COMBOUNITS_ID])
        {
        case 0: // Metric   
            
            sprintf(s1,"Front Left :  %.3f m",(float)(plugin->_params[FLDIST_ID])/1000);
            lblFL.SetWindowText(s1);

            sprintf(s1,"Front Right : %.3f m",(float)(plugin->_params[FRDIST_ID])/1000);
            lblFR.SetWindowText(s1);

            sprintf(s1,"Rear Left :   %.3f m",(float)(plugin->_params[RLDIST_ID])/1000);
            lblRL.SetWindowText(s1);

            sprintf(s1,"Rear Right :  %.3f m",(float)(plugin->_params[RRDIST_ID])/1000);
            lblRR.SetWindowText(s1);

            sprintf(s1,"Center :      %.3f m",(float)(plugin->_params[CDIST_ID])/1000);
            lblC.SetWindowText(s1);


            sprintf(s1,"%.1f °C",(float)(plugin->_params[TEMPERATURE_ID])/10);
            lblTemperature.SetWindowText(s1);   

            break;
            
        case 1: // Imperial

            sprintf(s1,"Front Left :  %.3f in",(float)(plugin->_params[FLDIST_ID])*0.03937007874015747788);
            lblFL.SetWindowText(s1);

            sprintf(s1,"Front Right : %.3f in",(float)(plugin->_params[FRDIST_ID])*0.03937007874015747788);
            lblFR.SetWindowText(s1);

            sprintf(s1,"Rear Left :   %.3f in",(float)(plugin->_params[RLDIST_ID])*0.03937007874015747788);
            lblRL.SetWindowText(s1);

            sprintf(s1,"Rear Right :  %.3f in",(float)(plugin->_params[RRDIST_ID])*0.03937007874015747788);
            lblRR.SetWindowText(s1);

            sprintf(s1,"Center :      %.3f in",(float)(plugin->_params[CDIST_ID])*0.03937007874015747788);
            lblC.SetWindowText(s1);


            sprintf(s1,"%.2f °F",1.8*(float)(plugin->_params[TEMPERATURE_ID])/10+32);
            lblTemperature.SetWindowText(s1);

            break;
        
        break;
        }

    case 1: // Advanced
        {
        if (controls_enabled) 
        { 
            FLx.set_pos((int)plugin->_params[FLX_ID]);
            FLy.set_pos((int)plugin->_params[FLY_ID]); 
            FLz.set_pos((int)plugin->_params[FLZ_ID]); 

            FRx.set_pos((int)plugin->_params[FRX_ID]);
            FRy.set_pos((int)plugin->_params[FRY_ID]);
            FRz.set_pos((int)plugin->_params[FRZ_ID]);
            
            RLx.set_pos((int)plugin->_params[RLX_ID]);
            RLy.set_pos((int)plugin->_params[RLY_ID]);
            RLz.set_pos((int)plugin->_params[RLZ_ID]);
            
            RRx.set_pos((int)plugin->_params[RRX_ID]);
            RRy.set_pos((int)plugin->_params[RRY_ID]);
            RRz.set_pos((int)plugin->_params[RRZ_ID]);
            
            Cx.set_pos((int)plugin->_params[CX_ID]);
            Cy.set_pos((int)plugin->_params[CY_ID]);
            Cz.set_pos((int)plugin->_params[CZ_ID]);
            
            Lx.set_pos((int)plugin->_params[LX_ID]);
            Ly.set_pos((int)plugin->_params[LY_ID]);
            Lz.set_pos((int)plugin->_params[LZ_ID]);
            
            Temperature.set_pos((int)plugin->_params[TEMPERATURE_ID]);
        }

            // Draw Speakers Positions
            int xoffset = 890;
            int yoffset = 300;
            lblSpkFL.MoveWindow(xoffset + (int)plugin->_params[FLX_ID]/30, yoffset - (int)plugin->_params[FLY_ID]/30, 20, 16, TRUE);
            lblSpkFR.MoveWindow(xoffset + (int)plugin->_params[FRX_ID]/30, yoffset - (int)plugin->_params[FRY_ID]/30, 20, 16, TRUE);
            lblSpkRL.MoveWindow(xoffset + (int)plugin->_params[RLX_ID]/30, yoffset - (int)plugin->_params[RLY_ID]/30, 20, 16, TRUE);
            lblSpkRR.MoveWindow(xoffset + (int)plugin->_params[RRX_ID]/30, yoffset - (int)plugin->_params[RRY_ID]/30, 20, 16, TRUE);
            lblSpkC.MoveWindow(xoffset + (int)plugin->_params[CX_ID]/30, yoffset - (int)plugin->_params[CY_ID]/30, 20, 16, TRUE);
            lblSpkL.MoveWindow(xoffset + (int)plugin->_params[LX_ID]/30, yoffset - (int)plugin->_params[LY_ID]/30, 20, 16, TRUE);

            lblSpkFL.Invalidate();
            lblSpkFR.Invalidate();
            lblSpkRL.Invalidate();
            lblSpkRR.Invalidate();
            lblSpkC.Invalidate();
            lblSpkL.Invalidate();

        switch (plugin->_params[COMBOUNITS_ID])
        {
        case 0: // Metric   

            // Labels of sliders
            sprintf(s1,"x = %.3f m",(float)(plugin->_params[FLX_ID]+plugin->_params[33])/1000);
            lblFLx.SetWindowText(s1);   
        
            sprintf(s1,"y = %.3f m",(float)(plugin->_params[FLY_ID]+plugin->_params[34])/1000);
            lblFLy.SetWindowText(s1);
            
            sprintf(s1,"z = %.3f m",(float)(plugin->_params[FLZ_ID])/1000);
            lblFLz.SetWindowText(s1);   
            

            sprintf(s1,"x = %.3f m",(float)(plugin->_params[FRX_ID]+plugin->_params[33])/1000);
            lblFRx.SetWindowText(s1);
            
            sprintf(s1,"y = %.3f m",(float)(plugin->_params[FRY_ID]+plugin->_params[34])/1000);
            lblFRy.SetWindowText(s1);

            sprintf(s1,"z = %.3f m",(float)(plugin->_params[FRZ_ID])/1000);
            lblFRz.SetWindowText(s1);
            

            sprintf(s1,"x = %.3f m",(float)(plugin->_params[RLX_ID]+plugin->_params[33])/1000);
            lblRLx.SetWindowText(s1);   
            
            sprintf(s1,"y = %.3f m",(float)(plugin->_params[RLY_ID]+plugin->_params[34])/1000);
            lblRLy.SetWindowText(s1);       
            
            sprintf(s1,"z = %.3f m",(float)(plugin->_params[RLZ_ID])/1000);
            lblRLz.SetWindowText(s1);


            sprintf(s1,"x = %.3f m",(float)(plugin->_params[RRX_ID]+plugin->_params[33])/1000);
            lblRRx.SetWindowText(s1);   
            
            sprintf(s1,"y = %.3f m",(float)(plugin->_params[RRY_ID]+plugin->_params[34])/1000);
            lblRRy.SetWindowText(s1);   

            sprintf(s1,"z = %.3f m",(float)(plugin->_params[RRZ_ID])/1000);
            lblRRz.SetWindowText(s1);   
            

            sprintf(s1,"x = %.3f m",(float)(plugin->_params[CX_ID]+plugin->_params[33])/1000);
            lblCx.SetWindowText(s1);

            sprintf(s1,"y = %.3f m",(float)(plugin->_params[CY_ID]+plugin->_params[34])/1000);
            lblCy.SetWindowText(s1);    

            sprintf(s1,"z = %.3f m",(float)(plugin->_params[CZ_ID])/1000);
            lblCz.SetWindowText(s1);
            
            
            sprintf(s1,"x = %.3f m",(float)(plugin->_params[LX_ID]+plugin->_params[33])/1000);
            lblLx.SetWindowText(s1);

            sprintf(s1,"y = %.3f m",(float)(plugin->_params[LY_ID]+plugin->_params[34])/1000);
            lblLy.SetWindowText(s1);    

            sprintf(s1,"z = %.3f m",(float)(plugin->_params[LZ_ID])/1000);
            lblLz.SetWindowText(s1);

            
            sprintf(s1,"%.1f °C",(float)(plugin->_params[TEMPERATURE_ID])/10);
            lblTemperature.SetWindowText(s1);   

            // Calculate and display the distances between the speakers
            sprintf(s1,"%.3f m",(float)(sqrt( (double)kabs(plugin->_params[FLX_ID] - plugin->_params[RLX_ID])*kabs(plugin->_params[FLX_ID] - plugin->_params[RLX_ID]) + kabs(plugin->_params[FLY_ID] - plugin->_params[RLY_ID])*kabs(plugin->_params[FLY_ID] - plugin->_params[RLY_ID]) + kabs(plugin->_params[FLZ_ID] - plugin->_params[RLZ_ID])*kabs(plugin->_params[FLZ_ID] - plugin->_params[RLZ_ID]) ))/1000);
            lblDistFL_RL.SetWindowText(s1);
            sprintf(s1,"%.3f m",(float)(sqrt( (double)kabs(plugin->_params[FRX_ID] - plugin->_params[RRX_ID])*kabs(plugin->_params[FRX_ID] - plugin->_params[RRX_ID]) + kabs(plugin->_params[FRY_ID] - plugin->_params[RRY_ID])*kabs(plugin->_params[FRY_ID] - plugin->_params[RRY_ID]) + kabs(plugin->_params[FRZ_ID] - plugin->_params[RRZ_ID])*kabs(plugin->_params[FRZ_ID] - plugin->_params[RRZ_ID]) ))/1000);
            lblDistFR_RR.SetWindowText(s1);
            sprintf(s1,"%.3f m",(float)(sqrt( (double)kabs(plugin->_params[FLX_ID] - plugin->_params[FRX_ID])*kabs(plugin->_params[FLX_ID] - plugin->_params[FRX_ID]) + kabs(plugin->_params[FLY_ID] - plugin->_params[FRY_ID])*kabs(plugin->_params[FLY_ID] - plugin->_params[FRY_ID]) + kabs(plugin->_params[FLZ_ID] - plugin->_params[FRZ_ID])*kabs(plugin->_params[FLZ_ID] - plugin->_params[FRZ_ID]) ))/1000);
            lblDistFL_FR.SetWindowText(s1);
            sprintf(s1,"%.3f m",(float)(sqrt( (double)kabs(plugin->_params[RLX_ID] - plugin->_params[RRX_ID])*kabs(plugin->_params[RLX_ID] - plugin->_params[RRX_ID]) + kabs(plugin->_params[RLY_ID] - plugin->_params[RRY_ID])*kabs(plugin->_params[RLY_ID] - plugin->_params[RRY_ID]) + kabs(plugin->_params[RLZ_ID] - plugin->_params[RRZ_ID])*kabs(plugin->_params[RLZ_ID] - plugin->_params[RRZ_ID]) ))/1000);
            lblDistRL_RR.SetWindowText(s1);
            sprintf(s1,"%.3f m",(float)(sqrt( (double)kabs(plugin->_params[FLX_ID] - plugin->_params[CX_ID])*kabs(plugin->_params[FLX_ID] - plugin->_params[CX_ID]) + kabs(plugin->_params[FLY_ID] - plugin->_params[CY_ID])*kabs(plugin->_params[FLY_ID] - plugin->_params[CY_ID]) + kabs(plugin->_params[FLZ_ID] - plugin->_params[CZ_ID])*kabs(plugin->_params[FLZ_ID] - plugin->_params[CZ_ID]) ))/1000);
            lblDistFL_C.SetWindowText(s1);
            sprintf(s1,"%.3f m",(float)(sqrt( (double)kabs(plugin->_params[FRX_ID] - plugin->_params[CX_ID])*kabs(plugin->_params[FRX_ID] - plugin->_params[CX_ID]) + kabs(plugin->_params[FRY_ID] - plugin->_params[CY_ID])*kabs(plugin->_params[FRY_ID] - plugin->_params[CY_ID]) + kabs(plugin->_params[FRZ_ID] - plugin->_params[CZ_ID])*kabs(plugin->_params[FRZ_ID] - plugin->_params[CZ_ID]) ))/1000);
            lblDistFR_C.SetWindowText(s1);

            // Calculate and display the distances between the listener and the speakers
            sprintf(s1,"%.3f m",(float)(sqrt( (double)kabs(plugin->_params[FLX_ID] - plugin->_params[LX_ID])*kabs(plugin->_params[FLX_ID] - plugin->_params[LX_ID]) + kabs(plugin->_params[FLY_ID] - plugin->_params[LY_ID])*kabs(plugin->_params[FLY_ID] - plugin->_params[LY_ID]) + kabs(plugin->_params[FLZ_ID] - plugin->_params[LZ_ID])*kabs(plugin->_params[FLZ_ID] - plugin->_params[LZ_ID]) ))/1000);
            lblDistL_FL.SetWindowText(s1);
            sprintf(s1,"%.3f m",(float)(sqrt( (double)kabs(plugin->_params[FRX_ID] - plugin->_params[LX_ID])*kabs(plugin->_params[FRX_ID] - plugin->_params[LX_ID]) + kabs(plugin->_params[FRY_ID] - plugin->_params[LY_ID])*kabs(plugin->_params[FRY_ID] - plugin->_params[LY_ID]) + kabs(plugin->_params[FRZ_ID] - plugin->_params[LZ_ID])*kabs(plugin->_params[FRZ_ID] - plugin->_params[LZ_ID]) ))/1000);
            lblDistL_FR.SetWindowText(s1);
            sprintf(s1,"%.3f m",(float)(sqrt( (double)kabs(plugin->_params[RLX_ID] - plugin->_params[LX_ID])*kabs(plugin->_params[RLX_ID] - plugin->_params[LX_ID]) + kabs(plugin->_params[RLY_ID] - plugin->_params[LY_ID])*kabs(plugin->_params[RLY_ID] - plugin->_params[LY_ID]) + kabs(plugin->_params[RLZ_ID] - plugin->_params[LZ_ID])*kabs(plugin->_params[RLZ_ID] - plugin->_params[LZ_ID]) ))/1000);
            lblDistL_RL.SetWindowText(s1);
            sprintf(s1,"%.3f m",(float)(sqrt( (double)kabs(plugin->_params[RRX_ID] - plugin->_params[LX_ID])*kabs(plugin->_params[RRX_ID] - plugin->_params[LX_ID]) + kabs(plugin->_params[RRY_ID] - plugin->_params[LY_ID])*kabs(plugin->_params[RRY_ID] - plugin->_params[LY_ID]) + kabs(plugin->_params[RRZ_ID] - plugin->_params[LZ_ID])*kabs(plugin->_params[RRZ_ID] - plugin->_params[LZ_ID]) ))/1000);
            lblDistL_RR.SetWindowText(s1);
            sprintf(s1,"%.3f m",(float)(sqrt( (double)kabs(plugin->_params[CX_ID] - plugin->_params[LX_ID])*kabs(plugin->_params[CX_ID] - plugin->_params[LX_ID]) + kabs(plugin->_params[CY_ID] - plugin->_params[LY_ID])*kabs(plugin->_params[CY_ID] - plugin->_params[LY_ID]) + kabs(plugin->_params[CZ_ID] - plugin->_params[LZ_ID])*kabs(plugin->_params[CZ_ID] - plugin->_params[LZ_ID]) ))/1000);
            lblDistL_C.SetWindowText(s1);

            break;

        case 1: // Imperial
    
            // Labels of sliders
            sprintf(s1,"x = %.3f in",(float)(plugin->_params[FLX_ID]+plugin->_params[33])*0.03937007874015747788);
            lblFLx.SetWindowText(s1);       

            sprintf(s1,"y = %.3f in",(float)(plugin->_params[FLY_ID]+plugin->_params[34])*0.03937007874015747788);
            lblFLy.SetWindowText(s1);       

            sprintf(s1,"z = %.3f in",(float)(plugin->_params[FLZ_ID])*0.03937007874015747788);
            lblFLz.SetWindowText(s1);

            
            sprintf(s1,"x = %.3f in",(float)(plugin->_params[FRX_ID]+plugin->_params[33])*0.03937007874015747788);
            lblFRx.SetWindowText(s1);       

            sprintf(s1,"y = %.3f in",(float)(plugin->_params[FRY_ID]+plugin->_params[34])*0.03937007874015747788);
            lblFRy.SetWindowText(s1);       

            sprintf(s1,"z = %.3f in",(float)(plugin->_params[FRZ_ID])*0.03937007874015747788);
            lblFRz.SetWindowText(s1);       

            
            sprintf(s1,"x = %.3f in",(float)(plugin->_params[RLX_ID]+plugin->_params[33])*0.03937007874015747788);
            lblRLx.SetWindowText(s1);       
    
            sprintf(s1,"y = %.3f in",(float)(plugin->_params[RLY_ID]+plugin->_params[34])*0.03937007874015747788);
            lblRLy.SetWindowText(s1);       

            sprintf(s1,"z = %.3f in",(float)(plugin->_params[RLZ_ID])*0.03937007874015747788);
            lblRLz.SetWindowText(s1);       

            
            sprintf(s1,"x = %.3f in",(float)(plugin->_params[RRX_ID]+plugin->_params[33])*0.03937007874015747788);
            lblRRx.SetWindowText(s1);       

            sprintf(s1,"y = %.3f in",(float)(plugin->_params[RRY_ID]+plugin->_params[34])*0.03937007874015747788);
            lblRRy.SetWindowText(s1);       

            sprintf(s1,"z = %.3f in",(float)(plugin->_params[RRZ_ID])*0.03937007874015747788);
            lblRRz.SetWindowText(s1);       

            
            sprintf(s1,"x = %.3f in",(float)(plugin->_params[CX_ID]+plugin->_params[33])*0.03937007874015747788);
            lblCx.SetWindowText(s1);        

            sprintf(s1,"y = %.3f in",(float)(plugin->_params[CY_ID]+plugin->_params[34])*0.03937007874015747788);
            lblCy.SetWindowText(s1);    

            sprintf(s1,"z = %.3f in",(float)(plugin->_params[CZ_ID])*0.03937007874015747788);
            lblCz.SetWindowText(s1);

            
            sprintf(s1,"x = %.3f in",(float)(plugin->_params[LX_ID]+plugin->_params[33])*0.03937007874015747788);
            lblLx.SetWindowText(s1);        

            sprintf(s1,"y = %.3f in",(float)(plugin->_params[LY_ID]+plugin->_params[34])*0.03937007874015747788);
            lblLy.SetWindowText(s1);    

            sprintf(s1,"z = %.3f in",(float)(plugin->_params[LZ_ID])*0.03937007874015747788);
            lblLz.SetWindowText(s1);    
        
            
            sprintf(s1,"%.2f °F",1.8*(float)(plugin->_params[TEMPERATURE_ID])/10+32);
            lblTemperature.SetWindowText(s1);   

            // Calculate and display the distances between the speakers
            sprintf(s1,"%.3f in",(float)(sqrt( (double)kabs(plugin->_params[FLX_ID] - plugin->_params[RLX_ID])*kabs(plugin->_params[FLX_ID] - plugin->_params[RLX_ID]) + kabs(plugin->_params[FLY_ID] - plugin->_params[RLY_ID])*kabs(plugin->_params[FLY_ID] - plugin->_params[RLY_ID]) + kabs(plugin->_params[FLZ_ID] - plugin->_params[RLZ_ID])*kabs(plugin->_params[FLZ_ID] - plugin->_params[RLZ_ID]) ))*0.03937007874015747788);
            lblDistFL_RL.SetWindowText(s1);
            sprintf(s1,"%.3f in",(float)(sqrt( (double)kabs(plugin->_params[FRX_ID] - plugin->_params[RRX_ID])*kabs(plugin->_params[FRX_ID] - plugin->_params[RRX_ID]) + kabs(plugin->_params[FRY_ID] - plugin->_params[RRY_ID])*kabs(plugin->_params[FRY_ID] - plugin->_params[RRY_ID]) + kabs(plugin->_params[FRZ_ID] - plugin->_params[RRZ_ID])*kabs(plugin->_params[FRZ_ID] - plugin->_params[RRZ_ID]) ))*0.03937007874015747788);
            lblDistFR_RR.SetWindowText(s1);
            sprintf(s1,"%.3f in",(float)(sqrt( (double)kabs(plugin->_params[FLX_ID] - plugin->_params[FRX_ID])*kabs(plugin->_params[FLX_ID] - plugin->_params[FRX_ID]) + kabs(plugin->_params[FLY_ID] - plugin->_params[FRY_ID])*kabs(plugin->_params[FLY_ID] - plugin->_params[FRY_ID]) + kabs(plugin->_params[FLZ_ID] - plugin->_params[FRZ_ID])*kabs(plugin->_params[FLZ_ID] - plugin->_params[FRZ_ID]) ))*0.03937007874015747788);
            lblDistFL_FR.SetWindowText(s1);
            sprintf(s1,"%.3f in",(float)(sqrt( (double)kabs(plugin->_params[RLX_ID] - plugin->_params[RRX_ID])*kabs(plugin->_params[RLX_ID] - plugin->_params[RRX_ID]) + kabs(plugin->_params[RLY_ID] - plugin->_params[RRY_ID])*kabs(plugin->_params[RLY_ID] - plugin->_params[RRY_ID]) + kabs(plugin->_params[RLZ_ID] - plugin->_params[RRZ_ID])*kabs(plugin->_params[RLZ_ID] - plugin->_params[RRZ_ID]) ))*0.03937007874015747788);
            lblDistRL_RR.SetWindowText(s1);
            sprintf(s1,"%.3f in",(float)(sqrt( (double)kabs(plugin->_params[FLX_ID] - plugin->_params[CX_ID])*kabs(plugin->_params[FLX_ID] - plugin->_params[CX_ID]) + kabs(plugin->_params[FLY_ID] - plugin->_params[CY_ID])*kabs(plugin->_params[FLY_ID] - plugin->_params[CY_ID]) + kabs(plugin->_params[FLZ_ID] - plugin->_params[CZ_ID])*kabs(plugin->_params[FLZ_ID] - plugin->_params[CZ_ID]) ))*0.03937007874015747788);
            lblDistFL_C.SetWindowText(s1);
            sprintf(s1,"%.3f in",(float)(sqrt( (double)kabs(plugin->_params[FRX_ID] - plugin->_params[CX_ID])*kabs(plugin->_params[FRX_ID] - plugin->_params[CX_ID]) + kabs(plugin->_params[FRY_ID] - plugin->_params[CY_ID])*kabs(plugin->_params[FRY_ID] - plugin->_params[CY_ID]) + kabs(plugin->_params[FRZ_ID] - plugin->_params[CZ_ID])*kabs(plugin->_params[FRZ_ID] - plugin->_params[CZ_ID]) ))*0.03937007874015747788);
            lblDistFR_C.SetWindowText(s1);

            // Calculate and display the distances between the listener and the speakers
            sprintf(s1,"%.3f in",(float)(sqrt( (double)kabs(plugin->_params[FLX_ID] - plugin->_params[LX_ID])*kabs(plugin->_params[FLX_ID] - plugin->_params[LX_ID]) + kabs(plugin->_params[FLY_ID] - plugin->_params[LY_ID])*kabs(plugin->_params[FLY_ID] - plugin->_params[LY_ID]) + kabs(plugin->_params[FLZ_ID] - plugin->_params[LZ_ID])*kabs(plugin->_params[FLZ_ID] - plugin->_params[LZ_ID]) ))*0.03937007874015747788);
            lblDistL_FL.SetWindowText(s1);
            sprintf(s1,"%.3f in",(float)(sqrt( (double)kabs(plugin->_params[FRX_ID] - plugin->_params[LX_ID])*kabs(plugin->_params[FRX_ID] - plugin->_params[LX_ID]) + kabs(plugin->_params[FRY_ID] - plugin->_params[LY_ID])*kabs(plugin->_params[FRY_ID] - plugin->_params[LY_ID]) + kabs(plugin->_params[FRZ_ID] - plugin->_params[LZ_ID])*kabs(plugin->_params[FRZ_ID] - plugin->_params[LZ_ID]) ))*0.03937007874015747788);
            lblDistL_FR.SetWindowText(s1);
            sprintf(s1,"%.3f in",(float)(sqrt( (double)kabs(plugin->_params[RLX_ID] - plugin->_params[LX_ID])*kabs(plugin->_params[RLX_ID] - plugin->_params[LX_ID]) + kabs(plugin->_params[RLY_ID] - plugin->_params[LY_ID])*kabs(plugin->_params[RLY_ID] - plugin->_params[LY_ID]) + kabs(plugin->_params[RLZ_ID] - plugin->_params[LZ_ID])*kabs(plugin->_params[RLZ_ID] - plugin->_params[LZ_ID]) ))*0.03937007874015747788);
            lblDistL_RL.SetWindowText(s1);
            sprintf(s1,"%.3f in",(float)(sqrt( (double)kabs(plugin->_params[RRX_ID] - plugin->_params[LX_ID])*kabs(plugin->_params[RRX_ID] - plugin->_params[LX_ID]) + kabs(plugin->_params[RRY_ID] - plugin->_params[LY_ID])*kabs(plugin->_params[RRY_ID] - plugin->_params[LY_ID]) + kabs(plugin->_params[RRZ_ID] - plugin->_params[LZ_ID])*kabs(plugin->_params[RRZ_ID] - plugin->_params[LZ_ID]) ))*0.03937007874015747788);
            lblDistL_RR.SetWindowText(s1);
            sprintf(s1,"%.3f in",(float)(sqrt( (double)kabs(plugin->_params[CX_ID] - plugin->_params[LX_ID])*kabs(plugin->_params[CX_ID] - plugin->_params[LX_ID]) + kabs(plugin->_params[CY_ID] - plugin->_params[LY_ID])*kabs(plugin->_params[CY_ID] - plugin->_params[LY_ID]) + kabs(plugin->_params[CZ_ID] - plugin->_params[LZ_ID])*kabs(plugin->_params[CZ_ID] - plugin->_params[LZ_ID]) ))*0.03937007874015747788);
            lblDistL_C.SetWindowText(s1);              

            break;
        }
        }

        break;
    
    }

}


// debug messages
static void dbg(const char *format, ...)
{
    char buf[4096];
    char *p = buf;
    va_list args;
    va_start(args, format);
    p += _vsnprintf(p, sizeof buf - 1, format, args);
    va_end(args);
    while ( p > buf && isspace(p[-1]) )
    *--p = '\0';
    *p++ = '\r';
    *p++ = '\n';
    *p = '\0';
    OutputDebugString(buf);
}


int iTimeBalanceV2PluginDlg::on_command(int lp,int rp)
{
 if(lp-kCONTROL_BASE==BTNSAVE)
 {
    //=============Read the registry============
    HKEY MyKey;
    DWORD VarType = REG_DWORD; //the value's a REG_DWORD type
    DWORD PathSize = _MAX_PATH + 1;
    char  *Path = (char *)malloc(PathSize);

    RegOpenKeyEx(HKEY_CURRENT_USER,"Software\\kX\\Plugins", 0, KEY_READ, &MyKey);
    RegQueryValueEx(MyKey,TimeBalanceV2_guid,NULL,&VarType,(unsigned char *)Path,&PathSize);

    RegCloseKey (MyKey);

    // find the dot in the pathname  
    char* dot = strrchr( (const char *)Path, '\\' ); 
    if (dot != NULL) 
    { 
        strcpy( dot, "\\TimeBalanceV2_light.da" ); // change the name to TimeBalanceV2.ini 
    }

    FILE *f;
    f=fopen(Path,"wt");

    if ( !f ) 
        dbg("TimeBalanceV2: TimeBalanceV2_light.da file could not be created");
        
    if ( f )
    {
        dbg("TimeBalanceV2: TimeBalanceV2_light.da file was created");

        fprintf(f, "; New microcode\n");
        fprintf(f, "name \"TimeBalanceV2 light\"\n");
        fprintf(f, "copyright \"Copyright (c) 2007. Tril\" \n");
        fprintf(f, "created \"02/23/2007\" \n");
        fprintf(f, "engine \"kX\" \n");
        fprintf(f, "comment \"Ajusts delays to five speakers\" \n");
        fprintf(f, "guid \"89d947b6-cc2d-4442-98b0-11dad50bbb6f\" \n");
        fprintf(f, "; -- generated GUID\n\n");

        fprintf(f, "itramsize %d\n",0 + plugin->_params[TEMPERATURE_ID+1] + 1 + plugin->_params[COMBOUNITS_ID+2] + 1 + plugin->_params[TEMPERATURE_ID+3] + 1 + plugin->_params[TEMPERATURE_ID+4] + 1 + plugin->_params[TEMPERATURE_ID+5]);


        fprintf(f, "input inFL, inFR, inRL, inRR, inC;\n");
        fprintf(f, "output outFL, outFR, outRL, outRR, outC; \n");

        fprintf(f, "; FL \n");
        fprintf(f, "idelay write dwFL at 0x0; delay write Front Left \n");
        fprintf(f, "idelay read drFL at 0x%x; delay read Front Left\n",plugin->_params[TEMPERATURE_ID+1]);
        fprintf(f, "; FR \n");
        fprintf(f, "idelay write dwFR at 0x%x; delay write Front Right\n",plugin->_params[TEMPERATURE_ID+1] + 1);
        fprintf(f, "idelay read drFR at 0x%x; delay read Front Right\n",plugin->_params[TEMPERATURE_ID+1] + 1 + plugin->_params[TEMPERATURE_ID+2]);
        fprintf(f, "; RL \n");
        fprintf(f, "idelay write dwRL at 0x%x ; delay write Rear Left\n",plugin->_params[TEMPERATURE_ID+1] + 1 + plugin->_params[TEMPERATURE_ID+2] + 1);
        fprintf(f, "idelay read drRL at 0x%x ; delay read Rear Left\n",plugin->_params[TEMPERATURE_ID+1] + 1 + plugin->_params[TEMPERATURE_ID+2] + 1 + plugin->_params[TEMPERATURE_ID+3]);
        fprintf(f, "; RR \n");
        fprintf(f, "idelay write dwRR at 0x%x ; delay write Rear Right\n", plugin->_params[TEMPERATURE_ID+1] + 1 + plugin->_params[TEMPERATURE_ID+2] + 1 + plugin->_params[TEMPERATURE_ID+3] + 1);
        fprintf(f, "idelay read drRR at 0x%x ; delay read Rear Right\n",plugin->_params[TEMPERATURE_ID+1] + 1 + plugin->_params[TEMPERATURE_ID+2] + 1 + plugin->_params[TEMPERATURE_ID+3] + 1 + plugin->_params[TEMPERATURE_ID+4]);
        fprintf(f, "; C \n");
        fprintf(f, "idelay write dwC at 0x%x ; delay write Center Left\n",plugin->_params[TEMPERATURE_ID+1] + 1 + plugin->_params[TEMPERATURE_ID+2] + 1 + plugin->_params[TEMPERATURE_ID+3] + 1 + plugin->_params[TEMPERATURE_ID+4] + 1);
        fprintf(f, "idelay read drC at 0x%x; ; delay read Center Left\n",plugin->_params[TEMPERATURE_ID+1] + 1 + plugin->_params[TEMPERATURE_ID+2] + 1 + plugin->_params[TEMPERATURE_ID+3] + 1 + plugin->_params[TEMPERATURE_ID+4] + 1 + plugin->_params[TEMPERATURE_ID+5]);


                                              
        fprintf(f, "; code \n");
        fprintf(f, "; FL \n");
        fprintf(f, "macs    dwFL, inFL, 0, 0; \n");
        fprintf(f, "macs    outFL,  drFL, 0, 0; \n");
        fprintf(f, "; FR \n");
        fprintf(f, "macs    dwFR, inFR, 0, 0; \n");
        fprintf(f, "macs    outFR,  drFR, 0, 0; \n");
        fprintf(f, "; RL \n");
        fprintf(f, "macs    dwRL, inRL, 0, 0; \n");
        fprintf(f, "macs    outRL,  drRL, 0, 0; \n");
        fprintf(f, "; RR \n");
        fprintf(f, "macs    dwRR, inRR, 0, 0; \n");
        fprintf(f, "macs    outRR,  drRR, 0, 0; \n");
        fprintf(f, "; C \n");
        fprintf(f, "macs    dwC, inC, 0, 0; \n");
        fprintf(f, "macs    outC,  drC, 0, 0;\n\n");
          

        fprintf(f, "end\n\n");

        fclose(f);

        // ask windows shell to open the file 
         ShellExecute( NULL, "register", (const char *)Path, NULL, NULL, SW_SHOWNORMAL ); 

    }
  return 1;
 }
 return CKXPluginGUI::on_command(lp,rp);
}
