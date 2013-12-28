// EF-X Library
// Copyright (c) Martin Borisov, 2004.
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
#include "tube.h"
// effect source
#include "tube_da.cpp"



// plugin parameters
// (these are 'logical' parameters visible to the user)
typedef enum _params_id{

	//Checkboxes
	LED_DC,
	LED_X2,
	LED_BPS1,
	LED_BPS2,
	SW_ST,
	SW_OC,
	
	//Knobs
	DRIVE,
	LEVEL_DRIVE,
	LEVEL_CLEAN,
	O_FREQ,//output filter freq
	O_WID,//Q2
	I_FREQ,//input filter freq
    I_WID,//Q1
	
	
};

// default plugin parameters
// -------------------------
static kxparam_t default_params[FUZZ_PARAMS_COUNT] = {0, 0, 0, 0, 1, 0, 42, 13, 15, 6, 18, 28, 29};

                
int ifuzzPlugin::set_defaults() 
{
	set_all_params(default_params); 
	return 0;
}

// for kX Automation: param description (see kxplugin.h for details on kx_fxparam_descr)
// -------------------------------------------------------------------------------------

static kx_fxparam_descr _param_descr[FUZZ_PARAMS_COUNT] = {
	{"DC" , KX_FXPARAM_SWITCH, 0, 1},
	{"x2" , KX_FXPARAM_SWITCH, 0, 1},
	{"Bypass BP Filter" , KX_FXPARAM_SWITCH, 0, 1},
	{"Bypass LP Filter" , KX_FXPARAM_SWITCH, 0, 1},
	{"Solid state/Tube" , KX_FXPARAM_SWITCH, 0, 1},
	{"Overdrive/Clean" , KX_FXPARAM_SWITCH, 0, 1},
	{"Drive", KX_FXPARAM_USER, 0,    60},
	{"Level_Overdrive", KX_FXPARAM_USER, 0,    60},
	{"Level_Clean", KX_FXPARAM_USER, 0,    60},
	{"BP Center Frequency", KX_FXPARAM_USER, 0,    60},
	{"BP Width", KX_FXPARAM_USER, 0,    60},
	{"LP Cutoff Frequency", KX_FXPARAM_USER, 0,    60},
	{"LP Width", KX_FXPARAM_USER, 0,    60},
	

	};

int ifuzzPlugin::describe_param(int ndx, kx_fxparam_descr *descr)
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

     P "Vintage Overdriven Tube Amp",0,0,0,0,1,0,42,13,15,6,18,28,29,
	 P "60's Fuzz",0,0,1,0,1,0,42,13,15,6,18,60,6,
	 P "Dirty Chords",0,0,0,0,1,0,24,15,15,6,3,24,40,
	 P "Heavy Drive",0,0,0,0,1,0,55,10,15,29,4,21,25,
	 P "Fuzzbox",0,0,0,1,0,0,49,12,15,16,7,30,30,
	 P "Bluesy Combo (neck pickup)",0,0,0,0,1,0,33,18,15,34,4,16,29,
	 P "Bluesy 2 (neck pickup)",0,0,0,0,1,0,33,24,15,50,4,30,21,
	 P "Fat Solo Guitar" ,1,0,0,0,0,0,50,10,15,0,0,28,19,
	 P "Solid State Crunch",0,0,0,0,0,0,4,14,15,10,9,22,23,
	 P "Heavy Metal (bridge pickup)",0,0,0,0,0,0,60,14,15,42,6,22,25,
	 P "Fat Tubes",0,0,0,0,1,0,52,11,15,7,29,28,23,
	 P "Santana",0,0,0,0,1,0,46,13,15,7,29,27,14,


	 NULL
};

#undef P

const kxparam_t *ifuzzPlugin::get_plugin_presets()
{
 return presets;
}


/* Plugin Implemenation */

int ifuzzPlugin::get_param(int ndx, kxparam_t *value) // ndx corresponds to 'logical' parameter id
{
	*value = _params[ndx]; 
	return 0;
}

// set_param sets 'logical' values by writing to (one or a group of) 'physical' registers

int ifuzzPlugin::set_param(int ndx, kxparam_t value)
{
	_params[ndx] = value;
	double x;
	double omega,sn,cs,alpha,scal,dumm;
	double eqfreq,eqband;
        double a0, a1, a2, b0, b1, b2;
	kxparam_t v_id;

	switch (ndx)
	{

	case LED_DC://add dc
		if(!_params[SW_OC]){
		    if(value) set_dsp_register(DC_P, _dbl_to_dspword(0.002));
		    else set_dsp_register(DC_P, 0);
		}
		break;

	case LED_BPS1://bypass BP filter
		 
		if(value) {	
			    write_instruction(1, 0x0u,0x8019u, 0x8019u, 0x2040u, 0x2040u);
				write_instruction(2, 0x0u,0x8019u, 0x8019u, 0x2040u, 0x2040u);
				write_instruction(3, 0x0u,0x8019u, 0x8019u, 0x2040u, 0x2040u);
				write_instruction(4, 0x0u,0x8019u, 0x8019u, 0x2040u, 0x2040u);
				write_instruction(5, 0x0u,0x8019u, 0x8019u, 0x2040u, 0x2040u);
				write_instruction(6, 0x0u,0x8019u, 0x8019u, 0x2040u, 0x2040u);
				write_instruction(7, 0x0u,0x8019u, 0x8019u, 0x2040u, 0x2040u);
				
			}
			
		else {
		        write_instruction(1, 0x0u ,0x2040u, 0x2040u, 0x8009u, 0x8005u);
	            write_instruction(2, 0x7u, 0x8009u, 0x8008u, 0x8008u, 0x8004u);
	            write_instruction(3, 0x7u, 0x8008u, 0x8019u, 0x800bu, 0x8007u);
	            write_instruction(4, 0x7u, 0x800bu, 0x800au, 0x800au, 0x8006u);
	            write_instruction(5, 0x0u, 0x800au, 0x2056u, 0x8019u, 0x8003u);
	            write_instruction(6, 0x6u, 0x800au, 0x2056u, 0x800au, 0x2040u);
	            write_instruction(7, 0x0u, 0x8019u, 0x800au, 0x2040u, 0x2040u);
			}
            
		break;

	case LED_BPS2://bypass LP filter
		if(value) {
			write_instruction(17, 0x0u, 0x2040u, 0x2040u, 0x2040u, 0x2040u);
			write_instruction(24, 0x0u, 0x2040u, 0x2040u, 0x2040u, 0x2040u);
		}
		else  {
			write_instruction(17, 0x0u, 0x8019u, 0x8013u, 0x2040u, 0x2040u);
			write_instruction(24, 0x0u, 0x8019u, 0x8017u, 0x2040u, 0x2040u);
		}
		break;

	case LED_X2://clean level *2
		if(_params[SW_OC]){
		  if(value) write_instr_y(25, 0x2042u);
		  else write_instr_y(25, 0x2041u);
		}
		break;

	case SW_ST://switch solid state/tube
		if(!_params[SW_OC]){	
		if(value) {//tube
			write_instruction(8, 0xcu,0x8019u,0x8019u,0x8002u,0x2040u);
			write_instruction(9, 0x0u,0x8019u,0x8019u,0x2040u,0x2040u);
			write_instruction(10, 0x0u,0x8019u,0x8019u,0x2040u,0x2040u);
			get_param(DRIVE, &v_id);
			set_dsp_register(DRIVE_P, v_id/4);

			
		}
		else {//solid state
			write_instruction(8, 0x4u,0x8019u,0x2040u,0x8019u,0x8002u);
			write_instruction(9, 0xbu,0x8019u,0x801c,0x801c,0x8019u);
			write_instruction(10, 0xau,0x8019u,0x801d,0x801d,0x8019u);
			get_param(DRIVE, &v_id);
			set_dsp_register(DRIVE_P, v_id*10);
		}
		}
		break;

	case SW_OC://overdrive/clean channel switch
		if(value) {//clean
			write_instr_r(8, 0x2040u);
			write_instr_r(9, 0x2040u);
			write_instr_r(10, 0x2040u);
		
			if(_params[LED_X2]) write_instr_y(25, 0x2042u);//if x2 led is on
			if(_params[LED_DC]) set_dsp_register(DC_P, 0);//if dc led is on, remove dc
		

            get_param(LEVEL_CLEAN, &v_id);
			x = (double)v_id;
	        set_dsp_register(MASTER_P, _dbl_to_dspword(x/FRAMES));			
		}
		else {//overdrive
			write_instr_r(8, 0x8019u);
			write_instr_r(9, 0x8019u);
			write_instr_r(10, 0x8019u);
			
		    write_instr_y(25, 0x2041u);//return gain to 1 (from x2 led)
			if(_params[LED_DC]) set_dsp_register(DC_P, _dbl_to_dspword(0.002));//if dc led is on, add dc

		    
			if(_params[SW_ST]) {//tube
		        write_instruction(8, 0xcu,0x8019u,0x8019u,0x8002u,0x2040u);
			    write_instruction(9, 0x0u,0x8019u,0x8019u,0x2040u,0x2040u);
			    write_instruction(10, 0x0u,0x8019u,0x8019u,0x2040u,0x2040u);
			    get_param(DRIVE, &v_id);
			    set_dsp_register(DRIVE_P, v_id/4);
			}
	     	else {//solid state
			    write_instruction(8, 0x4u,0x8019u,0x2040u,0x8019u,0x8002u);
			    write_instruction(9, 0xbu,0x8019u,0x801c,0x801c,0x8019u);
			    write_instruction(10, 0xau,0x8019u,0x801d,0x801d,0x8019u);
			    get_param(DRIVE, &v_id);
			    set_dsp_register(DRIVE_P, v_id*10);
			}
		    get_param(LEVEL_DRIVE, &v_id);
			x = (double)v_id;
	        set_dsp_register(MASTER_P, _dbl_to_dspword(x/FRAMES));	
	}

		break;

    case LEVEL_DRIVE:
		if(!_params[SW_OC]){
		x = (double)value;
	    set_dsp_register(MASTER_P, _dbl_to_dspword(x/FRAMES));
		}
		break;

	case LEVEL_CLEAN:
		if(_params[SW_OC]){
		x = (double)value;
	    set_dsp_register(MASTER_P, _dbl_to_dspword(x/FRAMES));
		}
		break;

	case DRIVE:
		if(_params[SW_ST]) set_dsp_register(DRIVE_P, value/4);//tube
		else set_dsp_register(DRIVE_P, value*10);//solid state
		break;

	case I_FREQ:
		eqfreq = (double)value*(10000/(FRAMES-1));
		if (eqfreq<166) eqfreq=166;
	    
	    get_param(I_WID, &v_id);
		eqband=v_id*(2000/(FRAMES-1))/800.;
		if(eqband <= 0.05) eqband=0.05;
	    //eqfreq = int(20 * exp((log(20000 / 20)/(double)(20000 - 20))*(double)(eqfreq)));
	    omega = (2.*pi)*(eqfreq/48000.);

        sn    = sin(omega);
        cs    = cos(omega);

		alpha = sn/(2*eqband);
        dumm = 2.;
				
		a0 =   1. + alpha;
		scal = dumm*a0;
		b0 =  ((1. - cs)/2.)/scal;
        b1 =  (1.-cs)/scal;
        b2 =  ((1. - cs)/2.)/scal;
        a1 =  (-2.*cs)/(scal*-1.);
        a2 =   (1.-alpha)/(scal*-1.);

		set_dsp_register(LOW_B0, _dbl_to_dspword(b0));
		set_dsp_register(LOW_B1, _dbl_to_dspword(b1));
		set_dsp_register(LOW_B2, _dbl_to_dspword(b2));
		set_dsp_register(LOW_A1, _dbl_to_dspword(a1));
		set_dsp_register(LOW_A2, _dbl_to_dspword(a2));
		break;

	case O_FREQ:
		eqfreq=(double)value*(10000/(FRAMES-1));
		if (eqfreq<166) eqfreq=166;
	    
	    get_param(O_WID, &v_id);
		eqband=v_id*(1000/(FRAMES-1))/800.;
		if(eqband <= 0.05) eqband=0.05;
	    //eqfreq = int(20 * exp((log(20000 / 20)/(double)(20000 - 20))*(double)(eqfreq)));

        omega = (2.*pi)*(eqfreq/48000.);
        sn    = sin(omega);
        cs    = cos(omega);
        alpha = sn/(2*eqband);
	    dumm = 2.;
				
		a0 =   1. + alpha;
		scal = dumm*a0;
		b0 =  alpha/scal;
		b1 =  0;
        b2 =  (-1. * alpha)/scal;
        a1 =  (-2.*cs)/(scal*-1.);
        a2 =   (1.-alpha)/(scal*-1.);

		set_dsp_register(B_B0, _dbl_to_dspword(b0));
		set_dsp_register(B_B1, _dbl_to_dspword(b1));
		set_dsp_register(B_B2, _dbl_to_dspword(b2));
		set_dsp_register(B_A1, _dbl_to_dspword(a1));
		set_dsp_register(B_A2, _dbl_to_dspword(a2));
		break;

	case I_WID:
		get_param(I_FREQ, &v_id);
		eqfreq=(double)v_id*(10000.0/(FRAMES-1));
		if (eqfreq<166) eqfreq=166;
	    
	
		eqband=value*(2000/(FRAMES-1))/800.;
		if(eqband <= 0.05) eqband=0.05;
	    //eqfreq = int(20 * exp((log(20000 / 20)/(double)(20000 - 20))*(double)(eqfreq)));
	    omega = (2.*pi)*(eqfreq/48000.);

        sn    = sin(omega);
        cs    = cos(omega);

		alpha = sn/(2*eqband);
        dumm = 2.;
				
		a0 =   1. + alpha;
		scal = dumm*a0;
		b0 =  ((1. - cs)/2.)/scal;
        b1 =  (1.-cs)/scal;
        b2 =  ((1. - cs)/2.)/scal;
        a1 =  (-2.*cs)/(scal*-1.);
        a2 =   (1.-alpha)/(scal*-1.);

		set_dsp_register(LOW_B0, _dbl_to_dspword(b0));
		set_dsp_register(LOW_B1, _dbl_to_dspword(b1));
		set_dsp_register(LOW_B2, _dbl_to_dspword(b2));
		set_dsp_register(LOW_A1, _dbl_to_dspword(a1));
		set_dsp_register(LOW_A2, _dbl_to_dspword(a2));
		break;

	case O_WID:
		get_param(O_FREQ, &v_id);
		eqfreq=(double)v_id*(10000.0/(FRAMES-1));;
		if (eqfreq<166) eqfreq=166;
	    
	
		eqband=value*(1000/(FRAMES-1))/800.;
	    if(eqband <= 0.05) eqband=0.05;
		//eqfreq = int(20 * exp((log(20000 / 20)/(double)(20000 - 20))*(double)(eqfreq)));

        omega = (2.*pi)*(eqfreq/48000.);
        sn    = sin(omega);
        cs    = cos(omega);
        alpha = sn/(2*eqband);
	    dumm = 2.;
				
		a0 =   1. + alpha;
		scal = dumm*a0;
		b0 =  alpha/scal;
		b1 =  0;
        b2 =  (-1. * alpha)/scal;
        a1 =  (-2.*cs)/(scal*-1.);
        a2 =   (1.-alpha)/(scal*-1.);

		set_dsp_register(B_B0, _dbl_to_dspword(b0));
		set_dsp_register(B_B1, _dbl_to_dspword(b1));
		set_dsp_register(B_B2, _dbl_to_dspword(b2));
		set_dsp_register(B_A1, _dbl_to_dspword(a1));
		set_dsp_register(B_A2, _dbl_to_dspword(a2));
		break;

	
	}

	// we need to synchronize the GUI part, if set_param was called by the mixer or kX Automation
	if (cp) ((ifuzzPluginDlg*) cp)->sync(ndx);

	return 0;
}

int ifuzzPlugin::set_all_params(kxparam_t* values)
{
	for (int i = 0; i < FUZZ_PARAMS_COUNT; i++) set_param(i, values[i]);
	return 0;
}

int ifuzzPlugin::request_microcode() 
{
	publish_microcode(fuzz);

	return 0;
}

const char *ifuzzPlugin::get_plugin_description(int id)
{
	plugin_description(fuzz);
}


iKXPluginGUI *ifuzzPlugin::create_cp(kDialog *parent, kFile *mf)
{
	ifuzzPluginDlg *tmp;
	tmp = new ifuzzPluginDlg(this, parent,mf);
	tmp->create();
	return tmp;
}

int ifuzzPlugin::get_user_interface()
{
	return IKXPLUGIN_OWNER; // means 'custom' GUI panel
}


BEGIN_MESSAGE_MAP(ifuzzPluginDlg, CKXPluginGUI)
     ON_MESSAGE(WM_UPDATE_STATIC, OnUpdateStatic)
END_MESSAGE_MAP()


ifuzzPluginDlg::ifuzzPluginDlg(ifuzzPlugin *plg, kDialog *parent_, kFile *mf_) : CKXPluginGUI(parent_,mf_)
{
          plugin = plg;
          kxmixer_skin=mf_; // save kxmixer skin pointer
}


/* Plugin UI Implementation */

void ifuzzPluginDlg::init()
{
        // skin status:
        // ------------------------
        // mf=this.kxl [if exists]
        // mf.next=[attached]=kX Mixer's mf
        // mf.priority=1; this.kxl is accessed first

        // process kxefx.kxs now
        TCHAR tmp_str[MAX_PATH];
        efx_skin.get_full_skin_path(_T("kxefx.kxs"),tmp_str);
        efx_skin.set_skin(tmp_str);
        mf.attach_skin(&efx_skin);
        efx_skin.attach_skin(kxmixer_skin);
        efx_skin.set_attach_priority(1);

        // skin status:
        // -------------------------
        // [1] mf=this.kxl
        // [2] mf.next=efx_skin
        // [3] efx_skin=kxefx.kxs
        // [4] efx_skin.next=kxmixer skin
        // [1,2,3,4] -- access order

        CKXPluginGUI::init(); // this is necessary
        // init() should be called -after- skin initialization

//-----------------------------------------------------------------------------------------------
// custom dialog initialization here...
// (title, Width, Height)
create_dialog(plugin_name(fuzz), 250, 120);
grpbox.hide();//hide useless GUI elements
w_label.hide();

    /*LCDs*/
    
    count_drive.Create("",WS_VISIBLE|SS_NOTIFY,CRect(CPoint(62,86),CSize(29,18)), this);
	//count_drive.SetID(COUNT_DRIVE);
	count_drive.SetBlankPadding(3);//празни полета от ляво
	count_drive.SetColours(RGB(0,220,0), RGB(0,0,0), RGB(0,0,0));//цифри, блат, слайдер
	count_drive.SetAllowInteraction(false);//не може да се настройва
	count_drive.SetDraw3DBar(false);//без слайдър
	count_drive.DisplayInt(true);

	count_level_drive.Create("",WS_VISIBLE|SS_NOTIFY,CRect(CPoint(187,86),CSize(29,18)), this);
	//count_level_drive.SetID(COUNT_LEVEL_DRIVE);
	count_level_drive.SetBlankPadding(3);//празни полета от ляво
	count_level_drive.SetColours(RGB(0,220,0), RGB(0,0,0), RGB(0,0,0));//цифри, блат, слайдер
	count_level_drive.SetAllowInteraction(false);//не може да се настройва
	count_level_drive.SetDraw3DBar(false);//без слайдър
	count_level_drive.DisplayInt(true);

	count_level_clean.Create("",WS_VISIBLE|SS_NOTIFY,CRect(CPoint(62,149),CSize(29,18)), this);
	//count_level_clean.SetID(COUNT_LEVEL_CLEAN);
	count_level_clean.SetBlankPadding(3);//празни полета от ляво
	count_level_clean.SetColours(RGB(0,220,0), RGB(0,0,0), RGB(0,0,0));//цифри, блат, слайдер
	count_level_clean.SetAllowInteraction(false);//не може да се настройва
    count_level_clean.SetDraw3DBar(false);//без слайдър
	count_level_clean.DisplayInt(true);

	count_o_freq.Create("",WS_VISIBLE|SS_NOTIFY,CRect(CPoint(281,92),CSize(32,14)), this);
	//count_o_freq.SetID(COUNT_O_FREQ);
	count_o_freq.SetBlankPadding(4);//празни полета от ляво
	count_o_freq.SetColours(RGB(0,220,0), RGB(0,0,0), RGB(0,0,0));//цифри, блат, слайдер
	count_o_freq.SetAllowInteraction(false);//не може да се настройва
	count_o_freq.SetDraw3DBar(false);//без слайдър
	count_o_freq.DisplayInt(true);
	
	count_i_freq.Create("",WS_VISIBLE|SS_NOTIFY,CRect(CPoint(381,92),CSize(32,14)), this);
	//count_i_freq.SetID(COUNT_I_FREQ);
	count_i_freq.SetBlankPadding(4);//празни полета от ляво
	count_i_freq.SetColours(RGB(0,220,0), RGB(0,0,0), RGB(0,0,0));//цифри, блат, слайдер
	count_i_freq.SetAllowInteraction(false);//не може да се настройва
	count_i_freq.SetDraw3DBar(false);//без слайдър
	count_i_freq.DisplayInt(true);

	/*count_i_width.Create("",WS_VISIBLE|SS_NOTIFY,CRect(CPoint(252,145),CSize(29,18)), this);
	count_i_width.SetID(COUNT_I_WID);
	//count_i_width.SetFormatString("%.f2");
	count_i_width.SetBlankPadding(3);//празни полета от ляво
	count_i_width.SetColours(RGB(0,220,0), RGB(0,0,0), RGB(0,0,0));//цифри, блат, слайдер
	count_i_width.SetAllowInteraction(false);//не може да се настройва
	count_i_width.SetDraw3DBar(false);//без слайдър
	count_i_width.DisplayInt(true);

	count_o_width.Create("",WS_VISIBLE|SS_NOTIFY,CRect(CPoint(153,145),CSize(29,18)), this);
	count_o_width.SetID(COUNT_O_WID);
	//count_o_width.SetFormatString("%.f2");
	count_o_width.SetBlankPadding(3);//празни полета от ляво
	count_o_width.SetColours(RGB(0,220,0), RGB(0,0,0), RGB(0,0,0));//цифри, блат, слайдер
	count_o_width.SetAllowInteraction(false);//не може да се настройва
	count_o_width.SetDraw3DBar(false);//без слайдър
	count_o_width.DisplayInt(true);*/
	

	
	
	/*for loading the bitmap frames for the knobs*///valid for filenames kx.bmp (e.g. k1.bmp
	for(i=0;i<FRAMES;i++){                        //k2.bmp, k3.bmp etc.) 
	   sprintf(filename, "k%d.bmp", i+1);         //
       bitmaps[i] = mf.load_image(filename);      //
	}                                             //
   /*end*///////////////////////////////////////////

   
	/*Knobs*/

	knob_drive.SetBitmaps(bitmaps);
	knob_drive.Create("",WS_CHILD| WS_VISIBLE|SS_NOTIFY, CRect(CPoint(13,76),CSize(40,40)), this);
    knob_drive.SetID(DRIVE);
	knob_drive.SetPos(plugin->_params[DRIVE]);
	
	knob_level_drive.SetBitmaps(bitmaps);
	knob_level_drive.Create("",WS_CHILD| WS_VISIBLE|SS_NOTIFY, CRect(CPoint(138,76),CSize(40,40)), this);
    knob_level_drive.SetID(LEVEL_DRIVE);
	knob_level_drive.SetPos(plugin->_params[LEVEL_DRIVE]);

	knob_level_clean.SetBitmaps(bitmaps);
	knob_level_clean.Create("",WS_CHILD| WS_VISIBLE|SS_NOTIFY, CRect(CPoint(13,139),CSize(40,40)), this);
    knob_level_clean.SetID(LEVEL_CLEAN);
	knob_level_clean.SetPos(plugin->_params[LEVEL_CLEAN]);
	
    knob_tone.SetBitmaps(bitmaps);
	knob_tone.Create("",WS_CHILD| WS_VISIBLE|SS_NOTIFY, CRect(CPoint(333,80),CSize(40,40)), this);
    knob_tone.SetID(I_FREQ);
	knob_tone.SetPos(plugin->_params[I_FREQ]);

	knob_i_width.SetBitmaps(bitmaps);
	knob_i_width.Create("",WS_CHILD| WS_VISIBLE|SS_NOTIFY, CRect(CPoint(355,133),CSize(40,40)), this);
    knob_i_width.SetID(I_WID);
	knob_i_width.SetPos(plugin->_params[I_WID]);
	
    knob_polish.SetBitmaps(bitmaps);
	knob_polish.Create("",WS_CHILD| WS_VISIBLE|SS_NOTIFY, CRect(CPoint(235,80),CSize(40,40)), this);
    knob_polish.SetID(O_FREQ);
	knob_polish.SetPos(plugin->_params[O_FREQ]);

	knob_o_width.SetBitmaps(bitmaps);
	knob_o_width.Create("",WS_CHILD| WS_VISIBLE|SS_NOTIFY, CRect(CPoint(256,133),CSize(40,40)), this);
    knob_o_width.SetID(O_WID);
	knob_o_width.SetPos(plugin->_params[O_WID]);

    
    /*Checkboxes*/
	
	led_clean.set_bitmaps(
		mf.load_image("led_off.bmp"),
		mf.load_image("led_off.bmp"),
		mf.load_image("led_on.bmp"),
		mf.load_image("led_on.bmp"));
	create_checkbox	(led_clean, LED_DC, ""	, 73, 47, 150); 
	led_clean.set_method(kMETHOD_TRANS);

	led_norm.set_bitmaps(
		mf.load_image("led_off.bmp"),
		mf.load_image("led_off.bmp"),
		mf.load_image("led_on.bmp"),
		mf.load_image("led_on.bmp"));
	create_checkbox	(led_norm, LED_X2, ""	, 73, 110, 150); 
	led_norm.set_method(kMETHOD_TRANS);

	led_bps1.set_bitmaps(
		mf.load_image("led_off.bmp"),
		mf.load_image("led_off.bmp"),
		mf.load_image("led_on_red.bmp"),
		mf.load_image("led_on_red.bmp"));
	create_checkbox	(led_bps1, LED_BPS1, ""	, 238, 109, 150); 
	led_bps1.set_method(kMETHOD_TRANS);

	led_bps2.set_bitmaps(
		mf.load_image("led_off.bmp"),
		mf.load_image("led_off.bmp"),
		mf.load_image("led_on_red.bmp"),
		mf.load_image("led_on_red.bmp"));
	create_checkbox	(led_bps2, LED_BPS2, ""	, 337, 109, 150); 
	led_bps2.set_method(kMETHOD_TRANS);

	sw_st.set_bitmaps(
		mf.load_image("switch_up.bmp"),
		mf.load_image("switch_up.bmp"),
		mf.load_image("switch_down.bmp"),
		mf.load_image("switch_down.bmp"));
	create_checkbox	(sw_st, SW_ST, ""	, 106, 57, 150); 
	sw_st.set_method(kMETHOD_TRANS);

	sw_oc.set_bitmaps(
		mf.load_image("switch_up.bmp"),
		mf.load_image("switch_up.bmp"),
		mf.load_image("switch_down.bmp"),
		mf.load_image("switch_down.bmp"));
	create_checkbox	(sw_oc, SW_OC, ""	, 151, 118, 150); 
	sw_oc.set_method(kMETHOD_TRANS);
	

	
	controls_enabled = 1; // enable updating GUI when the parameters are changed

	sync(EVERYPM_ID); // update all 'GUI' elements to the preset plugin parameters
}

void ifuzzPluginDlg::sync(int ndx)
{

	kxparam_t v;
	

    // this function should 'synchronize' any labels with the parameters

	switch (ndx)
	{ 
	case EVERYPM_ID:
	case DRIVE:
		v=plugin->_params[DRIVE];
		if(controls_enabled) {
			knob_drive.SetPos((int)v); 
			count_drive.SetPos((int)(v*1.66666667));
		}
		if(ndx!=EVERYPM_ID) break;

	case LEVEL_DRIVE:
		v=plugin->_params[LEVEL_DRIVE];
		if(controls_enabled) {
			knob_level_drive.SetPos((int)v); 
			count_level_drive.SetPos((int)(v*1.66666667));
		}
		if(ndx!=EVERYPM_ID) break;

	case LEVEL_CLEAN:
		v=plugin->_params[LEVEL_CLEAN];
		if(controls_enabled) {
			knob_level_clean.SetPos((int)v);
			count_level_clean.SetPos((int)(v*1.66666667));
		}
		if(ndx!=EVERYPM_ID) break;

	case I_FREQ:
		v=plugin->_params[I_FREQ];
		if(controls_enabled) {
			knob_tone.SetPos((int)v); 
			count_i_freq.SetPos((int)(v*(10000/(FRAMES-1))));
		}
		if(ndx!=EVERYPM_ID) break;

	case O_FREQ:
		v=plugin->_params[O_FREQ];
		if(controls_enabled) {
			knob_polish.SetPos((int)v); 
			count_o_freq.SetPos((int)(v*(10000/(FRAMES-1))));
		}
		if(ndx!=EVERYPM_ID) break;

	case I_WID:
		v=plugin->_params[I_WID];
		if(controls_enabled) knob_i_width.SetPos((int)v); 
		if(ndx!=EVERYPM_ID) break;

	case O_WID:
		v=plugin->_params[O_WID];
		if(controls_enabled)  knob_o_width.SetPos((int)v); 
		if(ndx!=EVERYPM_ID) break;

    case LED_DC:
		v=plugin->_params[LED_DC];
		if (controls_enabled) {led_clean.set_check(v);led_clean.redraw();}
		if (ndx!=EVERYPM_ID) break;

	case LED_X2:
		v=plugin->_params[LED_X2];
		if (controls_enabled) {led_norm.set_check(v);led_norm.redraw();}
		if (ndx!=EVERYPM_ID) break;

	case LED_BPS1:
		v=plugin->_params[LED_BPS1];
		if (controls_enabled) {led_bps1.set_check(v);led_bps1.redraw();}
		if (ndx!=EVERYPM_ID) break;

	case LED_BPS2:
		v=plugin->_params[LED_BPS2];
		if (controls_enabled) {led_bps2.set_check(v);led_bps2.redraw();}
		if (ndx!=EVERYPM_ID) break;

	case SW_ST:
		v=plugin->_params[SW_ST];
		if (controls_enabled) {sw_st.set_check(v);sw_st.redraw();}
		if (ndx!=EVERYPM_ID) break;

	case SW_OC:
		v=plugin->_params[SW_OC];
		if (controls_enabled) {sw_oc.set_check(v);sw_oc.redraw();}
		if (ndx!=EVERYPM_ID) break;


	
	}
}

/*for handling of knob and lcd control messages*/
LRESULT ifuzzPluginDlg::OnUpdateStatic(WPARAM wParam, LPARAM lParam)
{
    
     //int v_id;
	
	 switch(wParam)
	{
	
	case DRIVE:
	    plugin->set_param(DRIVE, lParam);
		break;

	case LEVEL_DRIVE:
	    plugin->set_param(LEVEL_DRIVE, lParam);
		break;

	case LEVEL_CLEAN:
	    plugin->set_param(LEVEL_CLEAN, lParam);
		break;

	case I_FREQ:
		if(lParam<1)lParam=1;
        plugin->set_param(I_FREQ, lParam);
		break;

	case O_FREQ:
	    if(lParam<1)lParam=1;
		plugin->set_param(O_FREQ, lParam);
		break;

	case I_WID:
		plugin->set_param(I_WID, lParam);
		break;
	
	case O_WID:
		plugin->set_param(O_WID, lParam);
	    break;
	}
	return 1;
}


int ifuzzPluginDlg::on_command(int lp,int rp)
{
    int check;
	
	switch(lp-kCONTROL_BASE)
	{

	case LED_DC:
		check = led_clean.get_check();
		led_clean.set_check(!check);
		plugin->set_param(LED_DC, !check);
		return 1;
		break;

	case LED_X2:
		check = led_norm.get_check();
		led_norm.set_check(!check);
		plugin->set_param(LED_X2, !check);
		return 1;
		break;

	case LED_BPS1:
		check = led_bps1.get_check();
		led_bps1.set_check(!check);
		plugin->set_param(LED_BPS1, !check);
		return 1;
		break;

	case LED_BPS2:
		check = led_bps2.get_check();
		led_bps2.set_check(!check);
		plugin->set_param(LED_BPS2, !check);
        return 1;
		break;

	case SW_ST:
		check = sw_st.get_check();
		sw_st.set_check(!check);
	    plugin->set_param(SW_ST, !check);
        return 1;
		break;

	
	case SW_OC:
		check = sw_oc.get_check();
		sw_oc.set_check(!check);
		plugin->set_param(SW_OC, !check);
        return 1;
		break;
	}

	return CKXPluginGUI::on_command(lp,rp);
}

