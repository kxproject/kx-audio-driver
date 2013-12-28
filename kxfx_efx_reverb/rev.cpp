// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Martin Borisov,2004.
// All rights reserved
// martintiger@abv.bg

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
#include "rev.h"
// effect source
#include "rev_da.cpp"



// plugin parameters
// (these are 'logical' parameters visible to the user)
typedef enum _params_id{
   
    //knobs
	INLEV,
	ERLEV,
	REVLEV,
	DRYLEV,
	BAND,
	PRE,
	DECAY,
	DIFF,
	DAMP,
	OUTLEV,
	TAP1,
	TAP2,
	TAP3,
	TAP4,
	TAP5,
	TAP6,
	 //checkboxes
	LED_PRE_X10,
	LED_DECAY_X10,
  
	
	
};

// default plugin parameters
// -------------------------                 //in, er, rev, dry, band,pre,decay,diff,damp,out
static kxparam_t default_params[REV_PARAMS_COUNT] = {60,3,39,40,50,30,15,60,19,30,0,0,2,4,47,60,0,0};
                
int iRevPlugin::set_defaults() 
{
	set_all_params(default_params); 
	return 0;
}

// for kX Automation: param description (see kxplugin.h for details on kx_fxparam_descr)
// -------------------------------------------------------------------------------------

static kx_fxparam_descr _param_descr[REV_PARAMS_COUNT] = {
	{"Input Level", KX_FXPARAM_USER, 0,    60},
	{"ER Level", KX_FXPARAM_USER, 0,    60},
	{"Reverb Level", KX_FXPARAM_USER, 0,    60},
	{"Dry Signal Level", KX_FXPARAM_USER, 0,    60},
	{"Bandwidth", KX_FXPARAM_USER, 0,    60},
	{"Pre-delay", KX_FXPARAM_USER, 0,    60},
	{"Decay Time", KX_FXPARAM_USER, 0,    60},
	{"Diffusion", KX_FXPARAM_USER, 0,    60},
	{"Damping", KX_FXPARAM_USER, 0,    60},
	{"Output level", KX_FXPARAM_USER, 0,    60},
	{"Tap1", KX_FXPARAM_USER, 0,    60},
	{"Tap2", KX_FXPARAM_USER, 0,    60},
	{"Tap3", KX_FXPARAM_USER, 0,    60},
	{"Tap4", KX_FXPARAM_USER, 0,    60},
	{"Tap5", KX_FXPARAM_USER, 0,    60},
	{"Tap6", KX_FXPARAM_USER, 0,    60},
	{"Pre-delay x10" , KX_FXPARAM_SWITCH, 0, 1},
	{"Decay time x10" , KX_FXPARAM_SWITCH, 0, 1},
	
	

	};

int iRevPlugin::describe_param(int ndx, kx_fxparam_descr *descr)
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
 
	                 //in,er,rev,dry,bnd,pre,dcy,dif,dmp,out,taps,pre10,dcy10//in,er,rev,dry,bnd,pre,dcy,dif,dmp,out,taps,pre10,dcy10
	P "Small Room"               ,60,3,60,31,60,0,4,60,25,30,2,7,8,24,8,7,0,0,
	P "Bathroom"                   ,60,3,50,14,60,0,5,60,0,40,2,7,12,19,50,9,0,0,
	P "Medium Room - Furnished"    ,60,3,48,43,47,10,7,60,33,28,2,7,12,13,14,9,0,0,
	P "Medium Room - Empty"        ,60,3,48,43,53,10,7,60,25,28,2,7,12,19,50,9,0,0,
	P "Large Room"                 ,60,16,28,44,51,7,12,60,20,28,7,0,3,4,0,60,1,0,
	P "Empty Gymnasium"            ,60,3,39,40,50,30,15,60,19,30,0,0,2,4,47,60,0,0,
    P "Large Auditorium - Occupied",60,7,53,31,50,60,20,53,25,28,0,0,0,4,0,60,0,0,
	P "Large Auditorium - Empty"   ,60,7,53,31,60,60,20,48,13,28,7,0,3,4,0,60,0,0,
	P "Large Concert Hall"         ,60,12,58,40,53,14,26,60,21,24,7,0,3,4,0,60,1,0,
	P "Cathedral"                  ,60,10,53,38,60,7,24,55,15,24,7,0,3,4,0,60,1,0,
	P "The Coliseum"               ,60,0,53,22,38,50,30,48,13,28,0,0,0,0,0,60,0,0,
    //P "Cave"                       ,60,0,28,14,60,10,50,60,0,50,3,6,8,11,16,23,1,0,
    P "Large Hangar"  ,60,0,23,42,46,15,15,66,24,30,0,0,0,0,50,60,1,1,
    P "Echoes From Beyond"         ,60,0,45,22,50,35,25,29,20,50,0,3,0,8,0,23,1,1,
    P "Canyon"                     ,60,0,39,40,60,5,10,19,26,30,0,0,2,0,0,60,0,1,
    P "Sweeps"                     ,60,0,40,31,37,50,10,60, 46,26,0,0,0,11,32,60,1,1,
    
	NULL
};

#undef P

const kxparam_t* iRevPlugin::get_plugin_presets()
{
 return presets;
}


/* Plugin Implemenation */

int iRevPlugin::get_param(int ndx, kxparam_t *value) // ndx corresponds to 'logical' parameter id
{
	*value = _params[ndx]; 
	return 0;
}

//function for calculating the decay time based on delay lenght and desired decay time
//returns the gain (attenuation) value for each all-pass or delay unit
double iRevPlugin::calc_gain(double len_samp, double time_s)
{
	double x;
	
	x = (double)-60*((double)len_samp/48000);
    return pow(10.0,(double)(x/time_s)/25);
	
}
// set_param sets 'logical' values by writing to (one or a group of) 'physical' registers

int iRevPlugin::set_param(int ndx, kxparam_t value)
{
	_params[ndx] = value;
	kxparam_t t, t1, val;
	double x;
	kxparam_t int_v;

	switch (ndx)
	{

	case INLEV:
		x = (double)value;
	    set_dsp_register(INLEV_P, _dbl_to_dspword(x/(FRAMES-1)));
	    break;

	case ERLEV:
		x = (double)value;
	    set_dsp_register(ERLEV_P, _dbl_to_dspword(x/(FRAMES-1)));
		break;

	case REVLEV:
		x = (double)value;
	    set_dsp_register(REVLEV_P, _dbl_to_dspword(x/(FRAMES-1)));
		break;

	case DRYLEV:
		x = (double)value;
	    set_dsp_register(DRYLEV_P, _dbl_to_dspword(x/(FRAMES-1)));
		break;

	case BAND:
		x = (int)value*333;
		x = int(20 * exp((log((double)20000 / 20)/(double)(20000 - 20))*(double)(x)));
		x = cos(6.283185307179586476925286766559*(x/48000));
		x = 2 - x - sqrt(pow((x-2),2) - 1);	
		set_dsp_register(BAND_P, _dbl_to_dspword(x));
		break;

	case PRE:
		get_tram_addr(PRE_LWR_P, (dword *) &t);
		get_tram_addr(PRE_RWR_P, (dword *) &t1);

		value = (int)(value/1.7142857);
		if(value<1) value=1;

	    if(!_params[LED_PRE_X10]){//if x10 checkbox is unchecked
		    set_tram_addr(PRE_L_P, (dword)(t + value * SAMPLES_PER_MSEC));
            set_tram_addr(PRE_R_P, (dword)(t1 + value * SAMPLES_PER_MSEC));
		}
		else{
			set_tram_addr(PRE_L_P, (dword)(t + value * SAMPLES_PER_MSEC*10));
            set_tram_addr(PRE_R_P, (dword)(t1 + value * SAMPLES_PER_MSEC*10));
		}
        break;

	case DECAY:
		if(!_params[LED_DECAY_X10]) x = (double)value/10;
		else x = (double)value;
	    
		set_dsp_register(G1_P, _dbl_to_dspword(calc_gain(1227, x)));
		set_dsp_register(G2_P, _dbl_to_dspword(calc_gain(1571, x)));
		set_dsp_register(G3_P, _dbl_to_dspword(calc_gain(2344, x)));
		set_dsp_register(G4_P, _dbl_to_dspword(calc_gain(2601, x)));
		set_dsp_register(G5_P, _dbl_to_dspword(calc_gain(2405, x)));
		set_dsp_register(G6_P, _dbl_to_dspword(calc_gain(3363, x)));
		set_dsp_register(G7_P, _dbl_to_dspword(calc_gain(3231, x)));
		set_dsp_register(G8_P, _dbl_to_dspword(calc_gain(1277, x)));
		set_dsp_register(G9_P, _dbl_to_dspword(calc_gain(1757, x)));
		set_dsp_register(G10_P, _dbl_to_dspword(calc_gain(2095, x)));
		set_dsp_register(G11_P, _dbl_to_dspword(calc_gain(2910, x)));
		set_dsp_register(G12_P, _dbl_to_dspword(calc_gain(2405, x)));
		set_dsp_register(G13_P, _dbl_to_dspword(calc_gain(3005, x)));
		set_dsp_register(G14_P, _dbl_to_dspword(calc_gain(3615, x)));
		break;

	case DIFF:
		x = (double)value;
	    set_dsp_register(DIFF_P, _dbl_to_dspword(x/85));
		break;

	case DAMP:
		x = (double)value;
	    set_dsp_register(DAMP_P, _dbl_to_dspword(x/(FRAMES-1)));
		break;

	case TAP1:
		x = (double)value;
	    set_dsp_register(TAP1_P, _dbl_to_dspword(x/(FRAMES-1)));
		break;

	case TAP2:
		x = (double)value;
	    set_dsp_register(TAP2_P, _dbl_to_dspword(x/(FRAMES-1)));
		break;

	case TAP3:
		x = (double)value;
	    set_dsp_register(TAP3_P, _dbl_to_dspword(x/(FRAMES-1)));
		break;

	case TAP4:
		x = (double)value;
	    set_dsp_register(TAP4_P, _dbl_to_dspword(x/(FRAMES-1)));
		break;

	case TAP5:
		x = (double)value;
	    set_dsp_register(TAP5_P, _dbl_to_dspword(x/(FRAMES-1)));
		break;

	case TAP6:
		x = (double)value;
	    set_dsp_register(TAP6_P, _dbl_to_dspword(x/(FRAMES-1)));
		break;

    case OUTLEV://FIX THIS!!!
		x = (double)(value*5);

		int_v = (int)(x/100);
		if(int_v>2) int_v = 2;
		set_dsp_register(INT_P, int_v);

		if(x <100) set_dsp_register(FRAC_P, _dbl_to_dspword(x/100));
		else if (x>100 && x<200) set_dsp_register(FRAC_P, _dbl_to_dspword((x-100)/100));
		else if (x>200) set_dsp_register(FRAC_P, _dbl_to_dspword((x-200)/100));

		 
	    break;

	case LED_PRE_X10:
		get_tram_addr(PRE_LWR_P, (dword *) &t);
		get_tram_addr(PRE_RWR_P, (dword *) &t1);

		get_param(PRE, &val);
		val = (int)(val/1.7142857);
		if(val<1) val=1;
		
        

		if(value) {
			set_tram_addr(PRE_L_P, (dword)(t + val * SAMPLES_PER_MSEC*10));
            set_tram_addr(PRE_R_P, (dword)(t1 + val * SAMPLES_PER_MSEC*10));
		}
		else{
			set_tram_addr(PRE_L_P, (dword)(t + val * SAMPLES_PER_MSEC));
            set_tram_addr(PRE_R_P, (dword)(t1 + val * SAMPLES_PER_MSEC));
		}
		break;

	case LED_DECAY_X10:
		  
		get_param(DECAY, &int_v);
		if(value) x = (double)int_v;
		else x = (double)int_v/10;
			
		


		 set_dsp_register(G1_P, _dbl_to_dspword(calc_gain(563, x)));
		 set_dsp_register(G2_P, _dbl_to_dspword(calc_gain(701, x)));
		 set_dsp_register(G3_P, _dbl_to_dspword(calc_gain(1503, x)));
		 set_dsp_register(G4_P, _dbl_to_dspword(calc_gain(2107, x)));
		 set_dsp_register(G5_P, _dbl_to_dspword(calc_gain(4047, x)));
		 set_dsp_register(G6_P, _dbl_to_dspword(calc_gain(2723, x)));
		 set_dsp_register(G7_P, _dbl_to_dspword(calc_gain(3477, x)));
		 set_dsp_register(G8_P, _dbl_to_dspword(calc_gain(327, x)));
		 set_dsp_register(G9_P, _dbl_to_dspword(calc_gain(999, x)));
		 set_dsp_register(G10_P, _dbl_to_dspword(calc_gain(1491, x)));
		 set_dsp_register(G11_P, _dbl_to_dspword(calc_gain(2113, x)));
		 set_dsp_register(G12_P, _dbl_to_dspword(calc_gain(4017, x)));
		 set_dsp_register(G13_P, _dbl_to_dspword(calc_gain(2871, x)));
		 set_dsp_register(G14_P, _dbl_to_dspword(calc_gain(3501, x)));
		break;
		 
	    

	
	
	}

	// we need to synchronize the GUI part, if set_param was called by the mixer or kX Automation
	if (cp) ((iRevPluginDlg*) cp)->sync(ndx);

	return 0;
}

int iRevPlugin::set_all_params(kxparam_t* values)
{
	for (int i = 0; i < REV_PARAMS_COUNT; i++) set_param(i, values[i]);
	return 0;
}

int iRevPlugin::request_microcode() 
{
	publish_microcode(rev);

	return 0;
}

const char *iRevPlugin::get_plugin_description(int id)
{
	plugin_description(rev);
}


iKXPluginGUI *iRevPlugin::create_cp(kDialog *parent, kFile *mf)
{
	iRevPluginDlg *tmp;
	tmp = new iRevPluginDlg(this, parent,mf);
	tmp->create();
	return tmp;
}

int iRevPlugin::get_user_interface()
{
	return IKXPLUGIN_OWNER; // means 'custom' GUI panel
}


BEGIN_MESSAGE_MAP(iRevPluginDlg, CKXPluginGUI)
     ON_MESSAGE(WM_UPDATE_STATIC, OnUpdateStatic)
     ON_WM_TIMER()
END_MESSAGE_MAP()

iRevPluginDlg::iRevPluginDlg(iRevPlugin *plg, kDialog *parent_,kFile *mf_) 
 : CKXPluginGUI(parent_,mf_) 
{ 
	  plugin=plg;

	  kxmixer_skin=mf_; // save kxmixer skin pointer

          timer_id=(UINT_PTR)-1;
		  
          max_l=-60;
          max_r=-60;

}


iRevPluginDlg::~iRevPluginDlg()
{
		
         	turn_on(0);
}


int iRevPluginDlg::turn_on(int what)
{
 if((what==1) && (timer_id==(UINT_PTR)-1))
 {
  timer_id=SetTimer(4321+plugin->pgm_id,70,NULL);
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

void iRevPluginDlg::init()
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
create_dialog(plugin_name(rev), 250, 120);
grpbox.hide();//hide useless GUI elements
w_label.hide();

    
    /*LCDs*/
    
    count_inlev.Create("",WS_VISIBLE|SS_NOTIFY,CRect(CPoint(65,96),CSize(29,18)), this);
	//count_inlev.SetID(COUNT_INLEV);
	count_inlev.SetBlankPadding(3);//празни полета от ляво
	count_inlev.SetColours(RGB(255,255,255), RGB(0,0,100), RGB(0,0,150));//цифри, блат, слайдер
	count_inlev.SetColourFaded(RGB(98,98,135));
	count_inlev.SetAllowInteraction(false);//не може да се настройва
	count_inlev.SetDraw3DBar(false);//без слайдър
	count_inlev.DisplayInt(true);

	count_erlev.Create("",WS_VISIBLE|SS_NOTIFY,CRect(CPoint(316,96),CSize(29,18)), this);
	//count_erlev.SetID(COUNT_ERLEV);
	count_erlev.SetBlankPadding(3);//празни полета от ляво
	count_erlev.SetColours(RGB(255,255,255), RGB(100,80,0), RGB(0,0,150));//цифри, блат, слайдер
	count_erlev.SetColourFaded(RGB(135,126,98));
	count_erlev.SetAllowInteraction(false);//не може да се настройва
	count_erlev.SetDraw3DBar(false);//без слайдър
	count_erlev.DisplayInt(true);

	count_revlev.Create("",WS_VISIBLE|SS_NOTIFY,CRect(CPoint(407,96),CSize(29,18)), this);
	//count_revlev.SetID(COUNT_REVLEV);
	count_revlev.SetBlankPadding(3);//празни полета от ляво
	count_revlev.SetColours(RGB(255,255,255), RGB(0,0,0), RGB(0,0,150));//цифри, блат, слайдер
	count_revlev.SetColourFaded(RGB(98,98,135));
	count_revlev.SetAllowInteraction(false);//не може да се настройва
    count_revlev.SetDraw3DBar(false);//без слайдър
	count_revlev.DisplayInt(true);

	count_drylev.Create("",WS_VISIBLE|SS_NOTIFY,CRect(CPoint(496,96),CSize(29,18)), this);
	//count_drylev.SetID(COUNT_DRYLEV);
	count_drylev.SetBlankPadding(3);//празни полета от ляво
	count_drylev.SetColours(RGB(255,255,255), RGB(100,0,0), RGB(0,0,150));//цифри, блат, слайдер
	count_drylev.SetColourFaded(RGB(135,98,98));
	count_drylev.SetAllowInteraction(false);//не може да се настройва
	count_drylev.SetDraw3DBar(false);//без слайдър
	count_drylev.DisplayInt(true);
	
	count_band.Create("",WS_VISIBLE|SS_NOTIFY,CRect(CPoint(167,97),CSize(44,15)), this);
	//count_band.SetID(COUNT_BAND);
	count_band.SetBlankPadding(5);//празни полета от ляво
	count_band.SetColours(RGB(255,255,255), RGB(0,0,100), RGB(0,0,150));//цифри, блат, слайдер
	count_band.SetColourFaded(RGB(98,98,135));
	count_band.SetAllowInteraction(false);//не може да се настройва
	count_band.SetDraw3DBar(false);//без слайдър
	count_band.DisplayInt(true);

	count_pre.Create("",WS_VISIBLE|SS_NOTIFY,CRect(CPoint(65,176),CSize(29,18)), this);
	//count_pre.SetID(COUNT_PRE);
	count_pre.SetBlankPadding(3);//празни полета от ляво
	count_pre.SetColours(RGB(255,255,255), RGB(0,0,100), RGB(0,0,0));//цифри, блат, слайдер
	count_pre.SetColourFaded(RGB(98,98,135));
	count_pre.SetAllowInteraction(false);//не може да се настройва
	count_pre.SetDraw3DBar(false);//без слайдър
	count_pre.DisplayInt(true);

	count_decay.Create("",WS_VISIBLE|SS_NOTIFY,CRect(CPoint(169,177),CSize(44,15)), this);
	//count_decay.SetID(COUNT_DECAY);
	count_decay.SetBlankPadding(5);//празни полета от ляво
	count_decay.SetColours(RGB(255,255,255), RGB(0,0,100), RGB(0,0,0));//цифри, блат, слайдер
	count_decay.SetColourFaded(RGB(98,98,135));
	count_decay.SetAllowInteraction(false);//не може да се настройва
	count_decay.SetDraw3DBar(false);//без слайдър
	count_decay.DisplayInt(true);

	count_outlev.Create("",WS_VISIBLE|SS_NOTIFY,CRect(CPoint(387,256),CSize(29,18)), this);
	//count_outlev.SetID(COUNT_OUTLEV);
	count_outlev.SetBlankPadding(3);//празни полета от ляво
	count_outlev.SetColours(RGB(255,255,255), RGB(0,0,100), RGB(0,0,0));//цифри, блат, слайдер
	count_outlev.SetColourFaded(RGB(98,98,135));
	count_outlev.SetAllowInteraction(false);//не може да се настройва
	count_outlev.SetDraw3DBar(false);//без слайдър
	count_outlev.DisplayInt(true);

	
	/*for loading the bitmap frames for the knobs*///valid for filenames kx.bmp (e.g. k1.bmp
	for(i=0;i<FRAMES;i++){                        //k2.bmp, k3.bmp etc.) 
	   sprintf(filename, "k%d.bmp", i+1);         //
       bitmaps[i] = mf.load_image(filename);      //
	}                                             //
   /*end*///////////////////////////////////////////

   /*Knobs*/

	knob_inlev.SetBitmaps(bitmaps);
	knob_inlev.Create("",WS_CHILD| WS_VISIBLE|SS_NOTIFY, CRect(CPoint(15,85),CSize(40,40)), this);
    knob_inlev.SetID(INLEV);
	knob_inlev.SetPos(plugin->_params[INLEV]);
	
	knob_erlev.SetBitmaps(bitmaps);
	knob_erlev.Create("",WS_CHILD| WS_VISIBLE|SS_NOTIFY, CRect(CPoint(265,85),CSize(40,40)), this);
    knob_erlev.SetID(ERLEV);
	knob_erlev.SetPos(plugin->_params[ERLEV]);

	knob_revlev.SetBitmaps(bitmaps);
	knob_revlev.Create("",WS_CHILD| WS_VISIBLE|SS_NOTIFY, CRect(CPoint(355,85),CSize(40,40)), this);
    knob_revlev.SetID(REVLEV);
	knob_revlev.SetPos(plugin->_params[REVLEV]);
	
    knob_drylev.SetBitmaps(bitmaps);
	knob_drylev.Create("",WS_CHILD| WS_VISIBLE|SS_NOTIFY, CRect(CPoint(445,85),CSize(40,40)), this);
    knob_drylev.SetID(DRYLEV);
    knob_drylev.SetPos(plugin->_params[DRYLEV]);

	knob_band.SetBitmaps(bitmaps);
	knob_band.Create("",WS_CHILD| WS_VISIBLE|SS_NOTIFY, CRect(CPoint(116,85),CSize(40,40)), this);
    knob_band.SetID(BAND);
	knob_band.SetPos(plugin->_params[BAND]);
	
    knob_pre.SetBitmaps(bitmaps);
	knob_pre.Create("",WS_CHILD| WS_VISIBLE|SS_NOTIFY, CRect(CPoint(15,165),CSize(40,40)), this);
    knob_pre.SetID(PRE);
	knob_pre.SetPos(plugin->_params[PRE]);

	knob_decay.SetBitmaps(bitmaps);
	knob_decay.Create("",WS_CHILD| WS_VISIBLE|SS_NOTIFY, CRect(CPoint(116,165),CSize(40,40)), this);
	knob_decay.SetID(DECAY);
	knob_decay.SetPos(plugin->_params[DECAY]);
	
    knob_diff.SetBitmaps(bitmaps);
	knob_diff.Create("",WS_CHILD| WS_VISIBLE|SS_NOTIFY, CRect(CPoint(362,165),CSize(40,40)), this);
    knob_diff.SetID(DIFF);
	knob_diff.SetPos(plugin->_params[DIFF]);

	knob_damp.SetBitmaps(bitmaps);
	knob_damp.Create("",WS_CHILD| WS_VISIBLE|SS_NOTIFY, CRect(CPoint(287,165),CSize(40,40)), this);
    knob_damp.SetID(DAMP);
	knob_damp.SetPos(plugin->_params[DAMP]);

    knob_outlev.SetBitmaps(bitmaps);
	knob_outlev.Create("",WS_CHILD| WS_VISIBLE|SS_NOTIFY, CRect(CPoint(335,245),CSize(40,40)), this);
    knob_outlev.SetID(OUTLEV);
	knob_outlev.SetPos(plugin->_params[OUTLEV]);

	/////////////////////////////////////////////////////////////

	knob_tap1.SetBitmaps(bitmaps);
	knob_tap1.Create("",WS_CHILD| WS_VISIBLE|SS_NOTIFY, CRect(CPoint(17,255),CSize(40,40)), this);
    knob_tap1.SetID(TAP1);
	knob_tap1.SetPos(plugin->_params[TAP1]);

	knob_tap2.SetBitmaps(bitmaps);
	knob_tap2.Create("",WS_CHILD| WS_VISIBLE|SS_NOTIFY, CRect(CPoint(62,255),CSize(40,40)), this);
    knob_tap2.SetID(TAP2);
	knob_tap2.SetPos(plugin->_params[TAP2]);

	knob_tap3.SetBitmaps(bitmaps);
	knob_tap3.Create("",WS_CHILD| WS_VISIBLE|SS_NOTIFY, CRect(CPoint(107,255),CSize(40,40)), this);
    knob_tap3.SetID(TAP3);
	knob_tap3.SetPos(plugin->_params[TAP3]);

	knob_tap4.SetBitmaps(bitmaps);
	knob_tap4.Create("",WS_CHILD| WS_VISIBLE|SS_NOTIFY, CRect(CPoint(152,255),CSize(40,40)), this);
    knob_tap4.SetID(TAP4);
	knob_tap4.SetPos(plugin->_params[TAP4]);

	knob_tap5.SetBitmaps(bitmaps);
	knob_tap5.Create("",WS_CHILD| WS_VISIBLE|SS_NOTIFY, CRect(CPoint(197,255),CSize(40,40)), this);
    knob_tap5.SetID(TAP5);
	knob_tap5.SetPos(plugin->_params[TAP5]);

	knob_tap6.SetBitmaps(bitmaps);
	knob_tap6.Create("",WS_CHILD| WS_VISIBLE|SS_NOTIFY, CRect(CPoint(242,255),CSize(40,40)), this);
    knob_tap6.SetID(TAP6);
	knob_tap6.SetPos(plugin->_params[TAP6]);


    /*Checkboxes*/
	
	led_pre_x10.set_bitmaps(
		mf.load_image("led_off.bmp"),
		mf.load_image("led_off.bmp"),
		mf.load_image("led_on.bmp"),
		mf.load_image("led_on.bmp"));
	create_checkbox	(led_pre_x10, LED_PRE_X10, "", 56, 183, 150); 
	led_pre_x10.set_method(kMETHOD_TRANS);

	led_decay_x10.set_bitmaps(
		mf.load_image("led_off.bmp"),
		mf.load_image("led_off.bmp"),
		mf.load_image("led_on.bmp"),
		mf.load_image("led_on.bmp"));
	create_checkbox	(led_decay_x10, LED_DECAY_X10, "", 170, 183, 150); 
	led_decay_x10.set_method(kMETHOD_TRANS);

	
	/*VU meter*/
	
	vumeter.set_bitmaps(
		 mf.load_image("vu_off.bmp"),
		 mf.load_image("vu_on.bmp"));
	vumeter.create(rev_name, 484, 161,this,1);
	vumeter.show();

	turn_on(1);
	redraw();
	

	controls_enabled = 1; // enable updating GUI when the parameters are changed

	sync(EVERYPM_ID); // update all 'GUI' elements to the preset plugin parameters
}

void iRevPluginDlg::sync(int ndx)
{

	kxparam_t v;
	

    // this function should 'synchronize' any labels with the parameters

	switch (ndx)
	{ 
	case EVERYPM_ID:
	case INLEV:
		v=plugin->_params[INLEV];
		if(controls_enabled) {
			knob_inlev.SetPos((int)v); 
			count_inlev.SetPos((int)(v*1.66666667));
		}
		if(ndx!=EVERYPM_ID) break;

	case ERLEV:
		v=plugin->_params[ERLEV];
		if(controls_enabled){
			knob_erlev.SetPos((int)v); 
			count_erlev.SetPos((int)(v*1.66666667));
		}
		if(ndx!=EVERYPM_ID) break;

	case REVLEV:
		v=plugin->_params[REVLEV];
		if(controls_enabled){
			knob_revlev.SetPos((int)v); 
			count_revlev.SetPos((int)(v*1.66666667));
		}
		if(ndx!=EVERYPM_ID) break;

	case DRYLEV:
		v=plugin->_params[DRYLEV];
		if(controls_enabled){
			knob_drylev.SetPos((int)v);
			count_drylev.SetPos((int)(v*1.66666667));
		}
		if(ndx!=EVERYPM_ID) break;

	case BAND:
		v=plugin->_params[BAND];
		if(controls_enabled){  
			knob_band.SetPos((int)v);
			count_band.SetPos((int)(20 * exp((log((double)(20000 / 20)/(double)(20000 - 20))*(double)(v*333)))));
		}
		if(ndx!=EVERYPM_ID) break;

	case PRE:
		v=plugin->_params[PRE];
		if(controls_enabled){
			knob_pre.SetPos((int)v);
			if(!plugin->_params[LED_PRE_X10]) count_pre.SetPos((int)(v/1.7142857));
		    else count_pre.SetPos((int)(10*(v/1.7142857)));
		}
		if(ndx!=EVERYPM_ID) break;

	case DECAY:
		v=plugin->_params[DECAY];
		if(controls_enabled){
			knob_decay.SetPos((int)v);
			if(!plugin->_params[LED_DECAY_X10]) count_decay.SetPos((int)(v*100));
		    else count_decay.SetPos((int)(v*1000));
		}
		if(ndx!=EVERYPM_ID) break;

	case DIFF:
		v=plugin->_params[DIFF];
		if(controls_enabled)  knob_diff.SetPos((int)v); 
		if(ndx!=EVERYPM_ID) break;

	case DAMP:
		v=plugin->_params[DAMP];
		if(controls_enabled)  knob_damp.SetPos((int)v); 
		if(ndx!=EVERYPM_ID) break;

	case OUTLEV:
		v=plugin->_params[OUTLEV];
		if(controls_enabled){
			knob_outlev.SetPos((int)v); 
			count_outlev.SetPos((int)(v*5));
		}
		if(ndx!=EVERYPM_ID) break;

	case TAP1:
		v=plugin->_params[TAP1];
		if(controls_enabled)  knob_tap1.SetPos((int)v); 
		if(ndx!=EVERYPM_ID) break;

	case TAP2:
		v=plugin->_params[TAP2];
		if(controls_enabled)  knob_tap2.SetPos((int)v); 
		if(ndx!=EVERYPM_ID) break;

	case TAP3:
		v=plugin->_params[TAP3];
		if(controls_enabled)  knob_tap3.SetPos((int)v); 
		if(ndx!=EVERYPM_ID) break;

	case TAP4:
		v=plugin->_params[TAP4];
		if(controls_enabled)  knob_tap4.SetPos((int)v); 
		if(ndx!=EVERYPM_ID) break;

	case TAP5:
		v=plugin->_params[TAP5];
		if(controls_enabled)  knob_tap5.SetPos((int)v); 
		if(ndx!=EVERYPM_ID) break;

	case TAP6:
		v=plugin->_params[TAP6];
		if(controls_enabled)  knob_tap6.SetPos((int)v); 
		if(ndx!=EVERYPM_ID) break;
	


    case LED_PRE_X10:
		v=plugin->_params[LED_PRE_X10];
		if (controls_enabled) {
			led_pre_x10.set_check(v);
			led_pre_x10.redraw();

			if(v) count_pre.SetPos((int)(5.8333333*plugin->_params[PRE]));
		    else count_pre.SetPos((int)(plugin->_params[PRE]/1.7142857));
		}
		if (ndx!=EVERYPM_ID) break;

	case LED_DECAY_X10:
		v=plugin->_params[LED_DECAY_X10];
		if (controls_enabled) {
			led_decay_x10.set_check(v);
			led_decay_x10.redraw();

		    if(v) count_decay.SetPos((int)(plugin->_params[DECAY]*1000));
		    else count_decay.SetPos((int)(plugin->_params[DECAY]*100));
		}
		if (ndx!=EVERYPM_ID) break;

	}
}

/*for handling of knob and lcd control messages*/
LRESULT iRevPluginDlg::OnUpdateStatic(WPARAM wParam, LPARAM lParam)
{
    
    
	
	 switch(wParam)
	{
	
	case INLEV:
		plugin->set_param(INLEV, lParam);
		break;

	case ERLEV:
		plugin->set_param(ERLEV, lParam);
		break;

	case REVLEV:
	    plugin->set_param(REVLEV, lParam);
		break;

	case DRYLEV:
	    plugin->set_param(DRYLEV, lParam);
		break;

	case BAND:
	    if(lParam<1)lParam=1;
		plugin->set_param(BAND, lParam);
		break;

	case PRE:
	    if(lParam<1)lParam=1;
		plugin->set_param(PRE, lParam);
        break;
	
	case DECAY:
	    plugin->set_param(DECAY, lParam);
        break;

	case DIFF:
	    plugin->set_param(DIFF, lParam);
		break;

	case DAMP:
	    plugin->set_param(DAMP, lParam);
		break;

	case OUTLEV:
	    plugin->set_param(OUTLEV, lParam);
		break;

	case TAP1:
	    plugin->set_param(TAP1, lParam);
		break;

	case TAP2:
	    plugin->set_param(TAP2, lParam);
		break;

	case TAP3:
	    plugin->set_param(TAP3, lParam);
		break;

	case TAP4:
	    plugin->set_param(TAP4, lParam);
		break;

	case TAP5:
	    plugin->set_param(TAP5, lParam);
		break;

	case TAP6:
	    plugin->set_param(TAP6, lParam);
		break;
	 }
	return 1;
}


int iRevPluginDlg::on_command(int lp,int rp)
{
    int check;
	
	switch(lp-kCONTROL_BASE)
	{

	case LED_PRE_X10:
		check = led_pre_x10.get_check();
		led_pre_x10.set_check(!check);

		plugin->set_param(LED_PRE_X10, !check);
		return 1;
		break;

	case LED_DECAY_X10:
		check = led_decay_x10.get_check();
		led_decay_x10.set_check(!check);

		plugin->set_param(LED_DECAY_X10, !check);
		return 1;
		break;

	
	}

	return CKXPluginGUI::on_command(lp,rp);
}


void iRevPluginDlg::OnTimer(UINT_PTR)
{
        dword left,right;
        plugin->get_dsp_register("result_l",&left);	//get adres vu levels from dsp
        plugin->get_dsp_register("result_r",&right);
        plugin->set_dsp_register("result_l",0);		//reset dsp result gpr
        plugin->set_dsp_register("result_r",0);

#define U_GAIN 0x78000000L // level is 0.25

float left_f  = ((float)(long)(left-U_GAIN)/16777216.0f + (float)(long)(left-U_GAIN)/33554432.0f);
float right_f = ((float)(long)(right-U_GAIN)/16777216.0f + (float)(long)(right-U_GAIN)/33554432.0f);

        if(left_f<-120.0) left_f=-120.0;	// set minimum left at -120
        if(right_f<-120.0) right_f=-120.0;	// set minimum right at -120

        if(left_f>6.0) left_f=6.0;			// set maximum left to +6
        if(right_f>6.0) right_f=6.0;		// set maximum right to +6

        if(left_f>max_l)	
         max_l=left_f;		// update peak hold left
        if(right_f>max_r)	
         max_r=right_f;		// update peak hold right
       {

// calculate 'seperators' for Peak bitmap display
int separator_l=(int)((double)vumeter.get_peak_height()*((double)-left_f+10.0)/74.0); //was 120
int separator_r=(int)((double)vumeter.get_peak_height()*((double)-right_f+10.0)/74.0); //was 120
        
		vumeter.set_value(separator_l,separator_r,0,0);
        vumeter.redraw();	// Redraw graphics VU meter

	   }

}

void iRevPluginDlg::on_destroy()
{
 turn_on(0);
}
