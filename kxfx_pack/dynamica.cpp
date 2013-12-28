// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov and Max Mikhailov, 2001-2005.
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

// Dynamica
// (c) eYagos, 2003-2004. All rights reserved

#include "stdafx.h"

// --------------- the above should be a 'standard' beginning of any .cpp file

// effect class
#include "dynamica.h"
// effect source
#include "da_dynamica.cpp"

// taken from plgmath_o.h

 #define	LN10	2.302585093
 #define	L1	484.0
 #define	L2	516.0
 #define	scale	1000
 #define	limit   -1000
 #define	LT	1000.0

 //Convert from time-slider value to time value
 inline double time_to_val(double value)
 {
	//double x = pow (10.0,(value/(scale/5.0))-1.0);  //lineal value
	double x = exp(LN10*(5.*value-1.*scale)/scale);
	return x;

        // legacy: #define time_to_val(value)	(exp (-0.06241108904 / (double) (value)));
 }
 //Convert from dB to exp-dB in DANE
 inline	double dB_to_exp(double value)
 {
	 double x = (0.935718067+0.045115616*log(exp(log(10.0)/20.0 * (value) )));
	 return x;
 }
 //Convert from ratio-slider value to ratio value
 inline double ratio_to_val (double value)
 {

	double x = 0.0;
	if (value<=L1)
	{
		x = value / L1;
		//x = pow (100.0, x) / pow (100.0, 10.0 / L1);
		x = exp(2.*LN10*(x*L1-10.)/L1);
		x = ((int) ((x + 0.05)*10)) / 10.0; 
	}
	else if (value>=L2) 
	{
		x = (LT - value) / L1;
		//x = pow (100.0, x) / pow (100.0, 10.0 / L1);
		x = exp(2.*LN10*(x*L1-10.)/L1);
		x = ((int) ((x + 0.05)*10)) / 10.0;
	}
	else
	{
		x = 10000.0;
	}
	return x;
 }


// plugin parameters
// (these are 'logical' parameters visible to the user)
typedef enum _params_id{
	OutputGain_ID, 
	GainAttack_ID,
	GainRelease_ID,
	LevelAttack_ID,
	LevelRelease_ID,
	Thres1_ID,
	Thres2_ID,
	Mode1_ID,
	Ratio1_ID,
	Mode2_ID,
	Ratio2_ID,
	Mode3_ID,
	Ratio3_ID,
	LookAhead_ID,
	SideChain_ID,
	KT_ID
};

#define	AGC_ID	DYNAMICA_PARAMS_COUNT    // last one, 'hidden'

// default plugin parameters
// -------------------------
static kxparam_t default_params[DYNAMICA_PARAMS_COUNT] = {0	,100,2000	,5	,3000	,-200	,-999	,0	,30	,0	,10	,0	,10	,30	,0	, 0};

int iDynamicaPlugin::set_defaults() 
{
	set_all_params(default_params); 
	_params[AGC_ID] = 0;
	return 0;
}

// for kX Automation: param description (see kxplugin.h for details on kx_fxparam_descr)
// -------------------------------------------------------------------------------------

static kx_fxparam_descr _param_descr[DYNAMICA_PARAMS_COUNT] = {
	{"Output Gain",				KX_FXPARAM_DB,  -999, 999	},
	{"Gain Attack",				KX_FXPARAM_MS,     1, 51000	},
	{"Gain Release",			KX_FXPARAM_MS,   100, 20000	},
	{"Level Attack",			KX_FXPARAM_MS,     1, 500	},
	{"Level Release",			KX_FXPARAM_MS,   100, 20000	},
	{"Threshold 1",				KX_FXPARAM_DB,  -999, 0		},
	{"Threshold 2",				KX_FXPARAM_DB,  -999, 0		},
	{"Compress/Expand 1",		KX_FXPARAM_USER,   0, 1		},
	{"Ratio 1",					KX_FXPARAM_USER,  10, 990	},
	{"Compress/Expand 2",		KX_FXPARAM_USER,   0, 1		},
	{"Ratio 2",					KX_FXPARAM_USER,  10, 990	},
	{"Compress/Expand 3",		KX_FXPARAM_USER,   0, 1		},
	{"Ratio 3",					KX_FXPARAM_USER,  10, 990	},
	{"Look Ahead Time",			KX_FXPARAM_MS,	   0, 40	},
	{"Side Chain",				KX_FXPARAM_USER,   0, 1		},
	{"Knee Type",				KX_FXPARAM_USER,   0, 2		}
	};

int iDynamicaPlugin::describe_param(int ndx, kx_fxparam_descr *descr)
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
 P	"2:1, -20, Fast Drums"								,60,240,1000,5,3000,-200,-999,0,83,0,10,0,10,30,0,0,
 P	"3:1 Expander <10dB"								,60,10,5000,5,3000,-100,-388,0,10,1,125,0,297,30,0,0,
 P	"3:1, -18, +3dB, Drums"								,30,310,750,5,3000,-180,-999,0,125,0,10,0,10,30,0,0,
 P	"4:1, -24, Very Fast Attack"						,60,10,5000,5,3000,-240,-999,0,156,0,10,0,10,30,0,0,
 P	"De-Esser High S (with SideChain at 5500-14000)"	,0,10,4000,10,3000,-300,-999,0,106,0,10,0,10,20,1,0,
 P	"De-Esser Hard   (with SideChain at 4000-12000)"	,0,10,4000,10,3000,-350,-999,0,125,0,10,0,10,20,1,0,
 P	"De-Esser Light  (with SideChain at 4000-12000)"	,0,10,4000,10,3000,-240,-999,0,53,0,10,0,10,20,1,0,
 P	"De-Esser Medium (with SideChain at 4000-12000)"	,0,10,4000,10,3000,-300,-999,0,83,0,10,0,10,20,1,0,
 P	"Drum Machne Limiter"								,140,100,2500,5,3000,-180,-999,0,189,1,38,0,10,30,0,0,
 P	"Electric Guitar Gate"								,120,10,2500,5,3000,-240,-457,0,189,1,59,1,292,30,0,0,
 P	"Heavy Master Limit, -18 Thres"						,80,290,540,5,3000,-180,-999,0,494,0,10,0,10,30,0,0,
 P	"Limit Hard -12dB"									,0,50,150,5,3000,-120,-999,0,494,0,10,0,10,30,0,0,
 P	"Limit Hard -3dB"									,0,50,150,5,3000,-30,-999,0,494,0,10,0,10,30,0,0,
 P	"Limit Hard -6dB"									,0,50,150,5,3000,-60,-999,0,494,0,10,0,10,30,0,0,
 P	"Limit Hard -9dB"									,0,50,150,5,3000,-90,-999,0,494,0,10,0,10,30,0,0,
 P	"Limit Soft -12dB, with Boost"						,80,50,150,5,3000,-120,-999,0,494,0,10,0,10,30,0,2,
 P	"Limit Soft -18dB, with Boost"						,120,50,150,5,3000,-180,-999,0,494,0,10,0,10,30,0,2,
 P	"Limit Soft -24dB, with Boost"						,160,50,150,5,3000,-240,-999,0,494,0,10,0,10,30,0,2,
 P	"Limit Soft -6dB, with Boost"						,40,50,150,5,3000,-60,-999,0,494,0,10,0,10,30,0,2,
 P	"Noise Gate @ 10dB"									,40,10,5000,5,3000,-100,-196,0,10,1,240,0,318,30,0,0,
 P	"Noise Gate @ 20dB"									,40,10,5000,5,3000,-200,-308,0,10,1,216,0,318,30,0,0,
 P	"Noisy Hot Guitar"									,120,20,7500,5,5000,-200,-290,0,187,1,59,1,287,40,0,0,
 P	"Power Drums"										,120,300,150,5,3000,-180,-999,0,494,0,10,0,10,30,0,2,
 P	"Radio Limit, fast release, with Boost"				,180,500,10,5,3000,-300,-999,0,325,0,10,0,10,30,0,0,
 P	"Compander"											,3,1,400,5,3000,-90,-400,0,150,0,20,1,83,30,0,0,
 P	"Sharp Attack Bass"									,-10,150,540,10,3000,-180,-999,0,10,0,125,0,10,30,0,0,
 P	"Thump Bass Comp"									,80,200,500,5,3000,-200,-999,0,106,0,10,0,10,30,0,0,
 P	"Uber-Punch"										,130,500,3000,5,500,-200,-999,0,923,1,38,0,10,30,0,2,
 P	"Vocal Comp, -24, +12dB, Fast"						,120,10,2500,5,3000,-240,-999,0,189,1,38,0,10,30,0,0,
 P	"Vocal Comp, 8:1, -24, Fast Attack"					,80,10,500,5,3000,-240,-999,0,229,0,10,0,10,30,0,0,
 P	"Vocal Limit, -26 Thres, +16dB"						,160,10,10000,5,3000,-260,-999,0,494,0,10,0,10,30,0,0,
 NULL // must be here
};

#undef P

const kxparam_t *iDynamicaPlugin::get_plugin_presets()
{
 return presets;
}


/* Plugin Implemenation */

int iDynamicaPlugin::get_param(int ndx, kxparam_t *value) // ndx corresponds to 'logical' parameter id
{
	*value = _params[ndx]; 
	return 0;
}

// set_param sets 'logical' values by writing to (one or a group of) 'physical' registers

int iDynamicaPlugin::set_param(int ndx, kxparam_t value)
{
	
	// Variables
	double	x, gain2, clip_f, rat, a, v_medio1, v_medio2;
	int		gain1, rati, a1;
	int		m1,m2,m3;
	double	th1,th2,rat1,rat2,rat3,GO;
	dword	clip;
	kxparam_t t;

	// Param
	_params[ndx] = value;

	
	// Switch
	switch (ndx)
	{
	//-- OutputGain --------------------------
	case OutputGain_ID:
		x = value / 10.0; //range is -99.9 to 99.9 dB
		x = dBtoG(x);
		gain1 = static_cast < int > ( x ); //convert to int to truncate
		gain2 = x - gain1; //get fractional part
		
		get_dsp_register("gainCLIP",&clip);
		clip_f =((float)(long)(clip)) / (float) (pow (2.0f,31)-1.0);
		if (clip_f=1.0)
		{
			set_dsp_register(R_GAIN_I, gain1); // writing value to level register with conversion from double to dsp's dword
			set_dsp_register(R_GAIN_F, _dbl_to_dspword(gain2));
		}
		set_dsp_register("gainCLIP",_dbl_to_dspword(1.0));
		
		break;
	
	//-- GainAttack --------------------------
	case GainAttack_ID:
		x = value /10.0 ; //range is 0.1 to 500 ms
		x = time_to_val (x);
		set_dsp_register(R_Attack, _dbl_to_dspword(x));

		break;
	
	//-- GainRelease --------------------------
	case GainRelease_ID:
		x = value / 10.0; //range is 40 to 3000 ms
		x = time_to_val (x);
		set_dsp_register(R_Release, _dbl_to_dspword(x));

		break;
	
	//-- LevelAttack --------------------------
	case LevelAttack_ID:
		x = value / 10.0; //range is 0.1 to 500 ms
		x = time_to_val (x);
		set_dsp_register(R_AttackL, _dbl_to_dspword(x));

		break;
	
	//-- LevelRelease --------------------------
	case LevelRelease_ID:
		x = value / 10.0; //range is 40 to 3000 ms
		x = time_to_val (x);
		set_dsp_register(R_ReleaseL, _dbl_to_dspword(x));

		break;
	
	//-- Thres1 --------------------------
	case Thres1_ID:
		x = value / 10.0;	//range is -99.9 to 0 dB
		x = dB_to_exp(x);
		set_dsp_register(R_Thres1, _dbl_to_dspword(x));
	
		break;
	
	//-- Thres2 --------------------------
	case Thres2_ID:
		x = value / 10.0;	//range is -99.9 to 0 dB
		x = dB_to_exp(x);
		set_dsp_register(R_Thres2, _dbl_to_dspword(x));
		
		break;
	
	//-- Mode1 --------------------------
	case Mode1_ID:
		set_param(Ratio1_ID, _params[Ratio1_ID]);

		break;
	
	//-- Mode2 --------------------------
	case Mode2_ID:
		set_param(Ratio2_ID, _params[Ratio2_ID]);

		break;
	
	//-- Mode3 --------------------------
	case Mode3_ID:
		set_param(Ratio3_ID, _params[Ratio3_ID]);

		break;
	
	//-- Ratio1 --------------------------
	case Ratio1_ID:
		x = ratio_to_val ((double)value);

		if (value > 516.0)
		{
			set_dsp_register(R_Rat1s, _dbl_to_dspword(-0.5));
		}
		else
		{
			set_dsp_register(R_Rat1s, _dbl_to_dspword( 0.5));
		}
		
		if ((_params[Mode1_ID]==0) || (_params[Mode1_ID]==2) )
		{
			rati = static_cast < int > ( 0.0 ); //convert to int to truncate
			rat  = 1/x; //get fractional part
			set_dsp_register(R_Rat1i, rati); // writing value to level register with conversion from double to dsp's dword
			set_dsp_register(R_Rat1 , _dbl_to_dspword(rat));
		}
		else 
		{
			rati = static_cast < int > ( x ); //convert to int to truncate
			rat  = x - rati; //get fractional part
			set_dsp_register(R_Rat1i, rati); // writing value to level register with conversion from double to dsp's dword
			set_dsp_register(R_Rat1 , _dbl_to_dspword(rat));
		}

		break;

	//-- Ratio2 --------------------------
	case Ratio2_ID:
		x = ratio_to_val ((double)value);

		if (value > 516.0)
		{
			set_dsp_register(R_Rat2s, _dbl_to_dspword(-0.5));
		}
		else
		{
			set_dsp_register(R_Rat2s, _dbl_to_dspword( 0.5));
		}

		if ((_params[Mode2_ID]==0) || (_params[Mode2_ID]==2) )
		{
			rati = static_cast < int > ( 0.0 ); //convert to int to truncate
			rat  = 1/x; //get fractional part
			set_dsp_register(R_Rat2i, rati); // writing value to level register with conversion from double to dsp's dword
			set_dsp_register(R_Rat2 , _dbl_to_dspword(rat));
		}
		else
		{
			rati = static_cast < int > ( x ); //convert to int to truncate
			rat  = x - rati; //get fractional part
			set_dsp_register(R_Rat2i, rati); // writing value to level register with conversion from double to dsp's dword
			set_dsp_register(R_Rat2 , _dbl_to_dspword(rat));
		}

		break;

	//-- Ratio3 --------------------------
	case Ratio3_ID:
		x = ratio_to_val ((double)value);
		
		if (value > 516.0)
		{
			set_dsp_register(R_Rat3s, _dbl_to_dspword(-0.5));
		}
		else
		{
			set_dsp_register(R_Rat3s, _dbl_to_dspword( 0.5));
		}

		if ((_params[Mode3_ID]==0) || (_params[Mode3_ID]==2) )
		{
			rati = static_cast < int > ( 0.0 ); //convert to int to truncate
			rat  = 1/x; //get fractional part
			set_dsp_register(R_Rat3i, rati); // writing value to level register with conversion from double to dsp's dword
			set_dsp_register(R_Rat3 , _dbl_to_dspword(rat));
		}
		else //       if (_params[Mode3_ID]==1)
		{
			rati = static_cast < int > ( x ); //convert to int to truncate
			rat  = x - rati; //get fractional part
			set_dsp_register(R_Rat3i, rati); // writing value to level register with conversion from double to dsp's dword
			set_dsp_register(R_Rat3 , _dbl_to_dspword(rat));
		}
		//else
		//{
		//	_params[Mode3_ID]=0;
		//	set_param(Mode3_ID, _params[Mode3_ID]);
		//}

		break;

	//-- LookAhead --------------------------
	case LookAhead_ID:
		x = value / 10.0;  // range is 0.0 to 7.7
		// reading delay start (write) address, adding predelay time to it and writing to delay read address
		// left channel predelay
		get_tram_addr(R_delW1, (dword *) &t);
		set_tram_addr(R_delR1, (dword)(t + 2 + x * SAMPLES_PER_MSEC));
		// right channel predelay
		get_tram_addr(R_delW2, (dword *) &t);
		set_tram_addr(R_delR2, (dword)(t + 2 + x * SAMPLES_PER_MSEC));

		break;

	//-- SideChain --------------------------
	case SideChain_ID:

		// if off write normal "read input" instructions
		if (!value)
		{
			write_instruction(0, 0x0u , R_tmp1	,0x4000u	,0x2040u,0x2040u);
			write_instruction(1, 0x0u , R_tmp2	,0x4001u	,0x2040u,0x2040u);
			write_instruction(2, 0x0u , R_delW1	,R_tmp1		,0x2040u,0x2040u);
			write_instruction(3, 0x0u , R_delW2	,R_tmp2		,0x2040u,0x2040u);
	
		}
		// if on write "read side chain" instructions
		else
		{
			write_instruction(0, 0x0u , R_tmp1	,0x4002u	,0x2040u,0x2040u);
			write_instruction(1, 0x0u , R_tmp2	,0x4003u	,0x2040u,0x2040u);
			write_instruction(2, 0x0u , R_delW1	,0x4000u	,0x2040u,0x2040u);
			write_instruction(3, 0x0u , R_delW2	,0x4001u	,0x2040u,0x2040u);
		}
		break;

	//-- AGC --------------------------
	case AGC_ID: 
		
		if	(value)
		{
			write_instruction(I_CLIP ,0x1u ,R_gainCLIP	,R_gainCLIP ,R_gainCLIP ,R_incCLIP);
		}
		else
		{
			write_instruction(I_CLIP ,0x1u ,R_gainCLIP	,R_gainCLIP ,R_gainCLIP ,0x2040u);
		}
		break;
	
	//-- default --------------------------
	default:
		th1		=_params[Thres1_ID] / 10.0;
		th2		=_params[Thres2_ID] / 10.0;
		m1		=(int)_params[Mode1_ID];
		m2		=(int)_params[Mode2_ID];
		m3		=(int)_params[Mode3_ID];
		rat1	= ratio_to_val ((double)_params[Ratio1_ID]);
		rat2	= ratio_to_val ((double)_params[Ratio2_ID]);
		rat3	= ratio_to_val ((double)_params[Ratio3_ID]);
		if (m1==0) rat1 = 1. / rat1;
		if (m2==0) rat2 = 1. / rat2;
		if (m3==0) rat3 = 1. / rat3;

		//---- Cálculo del soft knee
		a=0.025;
		v_medio1 = (rat2 - rat1) / 4.* a;
		v_medio2 = (rat3 - rat2) / 4.* a;
		a1 = static_cast < int > ( 1/a );
		switch (_params[KT_ID])
		{
		case 0:
			set_dsp_register(R_soft_long, a1);
			set_dsp_register(R_v_medio1, _dbl_to_dspword(0.));
			set_dsp_register(R_v_medio2, _dbl_to_dspword(0.));
			break;
		case 1:
			set_dsp_register(R_soft_long, a1);
			set_dsp_register(R_v_medio1, _dbl_to_dspword(v_medio1));
			set_dsp_register(R_v_medio2, _dbl_to_dspword(v_medio2));
			break;
		case 2:
			set_dsp_register(R_soft_long, a1/2);
			set_dsp_register(R_v_medio1, _dbl_to_dspword(v_medio1*2.));
			set_dsp_register(R_v_medio2, _dbl_to_dspword(v_medio2*2.));
			break;
		}

		//---- Offset 
		GO = (th1-th2)*(1.0 - rat2);
		GO = 0.045115616*log(exp(log(10.)/20.0 * GO));
		GO = 0.935718067 + 0.045115616 * log(exp(log(10.)/20.0 * th2 )) + GO;
		set_dsp_register(R_Offset, _dbl_to_dspword(GO));
		
		break;


	}

        // we need do synchronize the GUI part, if set_param was called by the mixer or kX Automation
	if (cp) ((iDynamicaPluginDlg*) cp)->sync(ndx);

	return 0;
}

int iDynamicaPlugin::set_all_params(kxparam_t* values)
{
	for (int i = 0; i < DYNAMICA_PARAMS_COUNT; i++) set_param(i, values[i]);
	return 0;
}

int iDynamicaPlugin::request_microcode() 
{
	publish_microcode(dynamica);

	return 0;
}

const char *iDynamicaPlugin::get_plugin_description(int id)
{
	plugin_description(dynamica);
}


iKXPluginGUI *iDynamicaPlugin::create_cp(kDialog *parent, kFile *mf)
{
	iDynamicaPluginDlg *tmp;
	tmp = new iDynamicaPluginDlg(this, parent,mf);
	tmp->create();
	return tmp;
}

int iDynamicaPlugin::get_user_interface()
{
	return IKXPLUGIN_OWNER; // means 'custom' GUI panel
}


// TIMER
//----------------------------------------------------------------
BEGIN_MESSAGE_MAP(iDynamicaPluginDlg, CKXPluginGUI)
	ON_WM_TIMER()
END_MESSAGE_MAP()

iDynamicaPluginDlg::iDynamicaPluginDlg(iDynamicaPlugin *plg, kDialog *parent_,kFile *mf_) 
 : CKXPluginGUI(parent_,mf_) 
{ 
		plugin=plg;
		timer_id=(UINT)-1;
		max_l=-120;
		max_r=-120;
		pm_offset=77;
		pm_width=308;
		pm_min=-84.0f;
		pm_max=6.0f;
}

iDynamicaPluginDlg::~iDynamicaPluginDlg()
{
         	turn_on(0);
}

int iDynamicaPluginDlg::turn_on(int what)
{
 if((what==1) && (timer_id==-1))
 {
  timer_id=SetTimer(4321+plugin->pgm_id,70,NULL); // FIXME: timer ID should be unique
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
// identifiers for extra controls
typedef enum _extra_ctrl {
	LTh1_ID,
	LAbove1_ID,
	LBellow1_ID,
	LTh2_ID,
	LBellow2_ID,
	LRat1_ID,
	LRat2_ID,
	LRat3_ID,
	LAtt_ID,
	LRel_ID,
	LAttL_ID,
	LRelL_ID,
	LAttV_ID,
	LRelV_ID,
	LAttLV_ID,
	LRelLV_ID,
	LLA_ID,
	LOG_ID,
	LKT_ID,
	LAGC_ID
};

void iDynamicaPluginDlg::init()
{
	CKXPluginGUI::init(); // this is necessary

	// custom dialog initialization here...
	// (title, Width, Height)
	create_dialog	(plugin_name(dynamica)	, 431	, 545); 

// create controls

// hslider	(Slider	,ID,Label	,minval,maxval,left,top,width,height)
// vslider	(Slider ,ID,Label	,minval,maxval,left,top,width,height)
// label	(Label	,ID,Caption, Left, Top, Width)
// combo	(Combo	,ID,Caption, Left, Top, Width, n_items	,drop_width)
// button   (Button ,ID,Caption, Left, Top, Width)

	int offset_x = 20;	
	int offset_y = 10;
	int y_th1	= 19  + offset_y;
	int	y_rat1	= 55  + offset_y;
	int	y_rat2	= 86  + offset_y;
	int y_th2	= 130 + offset_y;
	int	y_rat3	= 166 + offset_y;
	int	y_knee	= 212 + offset_y;
	int	y_side	= 212 + offset_y;
	int	y_attL	= 258 + offset_y;
	int	y_relL	= 288 + offset_y;
	int	y_att	= 324 + offset_y;		
	int	y_rel	= 354 + offset_y;
	int	y_look	= 390 + offset_y;
	int	y_outg	= 426 + offset_y;
	int	y_agr	= 450 + offset_y;
	int	y_mon	= 500 + offset_y;
	int	x_mon	= offset_x-5;

	const char* const Modes_names[3]	= {"Compress", "Expand", "Flat"};
	const char* const Knee_Types[3]		= {"Hard", "Soft", "Softer"};
	const char* const On_Off[2]			= {"Off", "Auto"};
	
	int i;
	
	//Threshold 1
	create_label	(lblTh1, LTh1_ID	, ""									, offset_x			, y_th1		, 140);
	create_hslider	(fdrTh1, Thres1_ID	,"1-st Threshold value"	, -999	, 0		, 153+offset_x		, y_th1-4	, 239	, 30);

	//Threshold 2
	create_label	(lblTh2, LTh2_ID	, ""									, offset_x			, y_th2		, 140);
	create_hslider	(fdrTh2, Thres2_ID	,"2-nd Threshold value",-999,0			, 153+offset_x		, y_th2-4	, 239	, 30);
	
	//Mode 1
	create_label	(lblAb1, LAbove1_ID	, ""									, offset_x+89		, y_rat1	, 120);
	create_combo	(cmbMo1, Mode1_ID, "Mode when input avobe 1st Thres"		, offset_x			, y_rat1-4	, 80	, 3	,70);
	for (i = 0; i < 3; i++) 
	  cmbMo1.add_string(Modes_names[i]);
	
	//Ratio 1
	create_label	(lblRa1, LRat1_ID	, ""									, 213+offset_x		, y_rat1	, 70);
	create_hslider	(fdrRa1, Ratio1_ID	, "Ratio"	, 10, 990 					, 275+offset_x		, y_rat1-4	, 117	, 30);
	
	//Mode 2
	create_label	(lblBe1, LBellow1_ID, ""									, offset_x+89		, y_rat2	, 120);
	create_combo	(cmbMo2, Mode2_ID, "Mode when input bellow 1st Thres"		, offset_x			, y_rat2-4	, 80	, 3	,70);
	for (i = 0; i < 3; i++) 
	  cmbMo2.add_string(Modes_names[i]);
	
	//Ratio 2
	create_label	(lblRa2, LRat2_ID	, ""									, 213+offset_x		, y_rat2	, 70);
	create_hslider	(fdrRa2, Ratio2_ID	, "Ratio"	, 10, 990					, 275+offset_x		, y_rat2-4	, 117	, 30);
	
	//Mode 3
	create_label	(lblBe2, LBellow2_ID, ""									, offset_x+89		, y_rat3	, 120);
	create_combo	(cmbMo3, Mode3_ID,"Mode when input bellow 2nd Thres"		, offset_x			, y_rat3-4	, 80	, 3	,70);
	for (i = 0; i < 3; i++) 
	  cmbMo3.add_string(Modes_names[i]);
	
	//Ratio 3	
	create_label	(lblRa3, LRat3_ID	, ""									, 213+offset_x		, y_rat3	, 70);
	create_hslider	(fdrRa3, Ratio3_ID	, "Ratio"	, 10, 990					, 275+offset_x		, y_rat3-4	, 117	, 30);

	//Knee Type
	create_label	(lblKT, LKT_ID		, "Curve Knee Type"						, offset_x+89		, y_knee	, 140);
	create_combo	(cmbKT, KT_ID		, "Knee Type"							, offset_x			, y_knee-4	, 80	, 3	,70);
	for (i = 0; i < 3; i++) 
	  cmbKT.add_string(Knee_Types[i]);

	//SideChain
	create_checkbox	(chkSC, SideChain_ID, "Toggle Side Chain Input"				, 230+offset_x		, y_side	, 150); 
	
	//Attack Level
	create_label	(lblAttL, LAttL_ID		, "Level Attack:"					, offset_x			, y_attL	, 150);
	create_label	(lblAttLV, LAttLV_ID	, ""								, offset_x+90		, y_attL	, 60);
	create_hslider	(fdrAttL, LevelAttack_ID,"Attack Time for level detector",1,500		, 153+offset_x	 	, y_attL-4	, 239	, 30);

	//Release Level
	create_label	(lblRelL, LRelL_ID		, "Level Release:"					, offset_x			, y_relL	, 150);
	create_label	(lblRelLV, LRelLV_ID	, ""								, offset_x+90		, y_relL	, 60);
	create_hslider	(fdrRelL, LevelRelease_ID,"Release Time for level detector",100,5000, 153+offset_x		, y_relL-4	, 239	, 30);
	
	//Attack Gain
	create_label	(lblAtt, LAtt_ID		, "Attack:"							, offset_x			, y_att		, 150);
	create_label	(lblAttV, LAttV_ID		, ""								, offset_x+90		, y_att		, 60);
	create_hslider	(fdrAtt, GainAttack_ID	, "Attack Time",1,5000				, 153+offset_x		, y_att-4	, 239	, 30);

	//Release Gain
	create_label	(lblRel, LRel_ID		, "Release:"						, offset_x			, y_rel		, 150);
	create_label	(lblRelV, LRelV_ID		, ""								, offset_x+90		, y_rel		, 60);
	create_hslider	(fdrRel, GainRelease_ID	, "Release Time",100,20000			, 153+offset_x		, y_rel-4	, 239	, 30);

	//Output Gain
	create_label	(lblOG, LOG_ID			, ""								, offset_x			, y_outg	, 140);
	create_hslider	(fdrOG, OutputGain_ID	, "Output Gain",-999,999			, 153+offset_x		, y_outg-4	, 239	, 30);

	//AGR
	create_checkbox	(chkAGC, AGC_ID			, "A.G.R."							, offset_x+10			, y_agr	, 80); 
	
	//Look Ahead Time
	create_label	(lblLA, LLA_ID			, ""								, offset_x			, y_look	, 140);
	create_hslider	(fdrLA, LookAhead_ID	, "Look Ahead Time",0,40			, 153+offset_x		, y_look-4	, 239	, 30);

	

	//Monitors
/*
	monitor.set_bitmaps(
	 mf.load_image(mf.get_profile("spectrum","mon_low")),
	 mf.load_image(mf.get_profile("spectrum","mon_high")));
*/
	// note: either include a custom 'kxskin.ini' file and refer to it
        // via 'mf.get_profile'
        // or use system-wide skin file and use direct file names

	monitor.set_bitmaps(
	 mf.load_image("fxlib/monitor_low.bmp"),
	 mf.load_image("fxlib/monitor_high.bmp"));

	
	// show monitors meter left, top  
	monitor.create(dynamica_name, x_mon, y_mon,this,0); 
	monitor.show();

	turn_on(1);
	redraw();
	
	controls_enabled = 1; // enable updating GUI when the parameters are changed

	sync(EVERYPM_ID); // update all 'GUI' elements to the preset plugin parameters
}

void iDynamicaPluginDlg::sync(int ndx)
{
	kxparam_t v;
	double x;

    // this function should 'synchronize' any labels with the parameters

	switch (ndx)
	{ 
	case EVERYPM_ID:
	//Threshold 1
	case Thres1_ID:	
		v  = plugin->_params[Thres1_ID];
		if (plugin->_params[Thres2_ID]>v) 
		{
			plugin->set_param(Thres2_ID, v);
			fdrTh2.set_pos((int)v);
		}
		
		if (controls_enabled) 
			fdrTh1.set_pos((int)v);
		
		{
		 char tmp[127];
		 sprintf(tmp, "%s %.1fdB","Threshold 1:    ", v/10.);
		 lblTh1.SetWindowText(tmp);
		 //ratio 1
		 sprintf(tmp, "%s %.1fdB","Ratio avobe  ", v/10.);
		 lblAb1.SetWindowText(tmp);
		 //ratio 2
		 sprintf(tmp, "%s %.1fdB","Ratio bellow ", v/10.);
		 lblBe1.SetWindowText(tmp);
		}
		if (ndx != EVERYPM_ID) break;

	//Threshold 2
	case Thres2_ID:	
		v = plugin->_params[Thres2_ID];

		if (controls_enabled) 
			fdrTh2.set_pos((int)v);
		
		if (v>plugin->_params[Thres1_ID])
		{
			v = plugin->_params[Thres1_ID];
			fdrTh2.set_pos((int)v);
			plugin->set_param(Thres2_ID, v);
		}
		
		{
		 char tmp[127];
		 sprintf(tmp, "%s %.1fdB","Threshold 2:    ", v/10.);
		 lblTh2.SetWindowText(tmp);
		 //ratio 3
		 sprintf(tmp, "%s %.1fdB","Ratio bellow ", v/10.);
		 lblBe2.SetWindowText(tmp);
		}
		if (ndx != EVERYPM_ID) break;

		
	//Mode 1
	case Mode1_ID:
		v = plugin->_params[Mode1_ID];
		if (v==2)
		{
			plugin->_params[Ratio1_ID]=10;
			plugin->_params[Mode1_ID]=0;
			plugin->set_param(Mode1_ID, 0);
			v=0;
		}
		if (controls_enabled)
		 cmbMo1.set_selection((int)v);
		if (ndx != EVERYPM_ID) break;
	//Ratio 1
	case Ratio1_ID:	
		v = plugin->_params[Ratio1_ID];

		if (controls_enabled) 
			fdrRa1.set_pos((int)v);
		{
		  char tmp[127];
		  
		  x = ratio_to_val((double)v); 
		  if (v > 516.0) x = -x;
		  if (x == 1000.0)
		  {
			sprintf(tmp, "%s",":     inf");
		  }
		  else
		  {
			  sprintf(tmp, "%s %.1f",":   ", x);
		  }
		  
		  lblRa1.SetWindowText(tmp);
		}
		if (ndx != EVERYPM_ID) break;
	
	
	//Mode 2
	case Mode2_ID:
		v = plugin->_params[Mode2_ID];
		if (v==2)
		{
			plugin->_params[Ratio2_ID]=10;
			plugin->_params[Mode2_ID]=0;
			plugin->set_param(Mode2_ID, 0);
			v=0;
		}
		if (controls_enabled)
		 cmbMo2.set_selection((int)v);
		if (ndx != EVERYPM_ID) break;	

	//Ratio 2
	case Ratio2_ID:	
		v = plugin->_params[Ratio2_ID];
		if (controls_enabled) 
			fdrRa2.set_pos((int)v);

		{
		  char tmp[127];
		  
		  x = ratio_to_val((double)v);
		  if (v > 516.0) x = -x;
		  if (x == 1000.0)
		  {
			  sprintf(tmp, "%s",":     inf");
		  }
		  else
		  {
			  sprintf(tmp, "%s %.1f",":   ", x);
		  }
		  
		  lblRa2.SetWindowText(tmp);
		}
		if (ndx != EVERYPM_ID) break;

	//Mode 3
	case Mode3_ID:
		v = plugin->_params[Mode3_ID];
		if (v==2)
		{
			plugin->_params[Ratio3_ID]=10;
			plugin->_params[Mode3_ID]=0;
			plugin->_params[Thres2_ID]=-999;
			plugin->set_param(Thres2_ID, -999);
			plugin->set_param(Mode3_ID, 0); 
			v=0;
		}
		if (controls_enabled)
		 cmbMo3.set_selection((int)v);
		if (ndx != EVERYPM_ID) break;	
	
	//Ratio 3
	case Ratio3_ID:	
		v = plugin->_params[Ratio3_ID];
		if (controls_enabled) 
			fdrRa3.set_pos((int)v);

		{
		  char tmp[127];
		  
		  x = ratio_to_val((double)v);
		  if (v > 516.0) x = -x;
		  if (x == 1000.0)
		  {
			  sprintf(tmp, "%s",":     inf");
		  }
		  else
		  {
			  sprintf(tmp, "%s %.1f",":   ", x);
		  }

		  lblRa3.SetWindowText(tmp);
		}
		if (ndx != EVERYPM_ID) break;

	//Knee Type
	case KT_ID:
		v = plugin->_params[KT_ID];
		if (controls_enabled)
		 cmbKT.set_selection((int)v);
		if (ndx != EVERYPM_ID) break;	

	//Attack Gain
	case GainAttack_ID:
		v = plugin->_params[GainAttack_ID];
		if (controls_enabled)
			fdrAtt.set_pos((int)v);

		{
		 char tmp[127];
		 sprintf(tmp, "%.1f ms",v/10.0f);
		 lblAttV.SetWindowText(tmp);
		 
		}
		if (ndx != EVERYPM_ID) break;
	
	//Release Gain
	case GainRelease_ID:
		v = plugin->_params[GainRelease_ID];
		if (controls_enabled)
			fdrRel.set_pos((int)v);

		{
		 char tmp[127];
		 sprintf(tmp, "%.1f ms",v/10.);
		 lblRelV.SetWindowText(tmp);
		 
		}
		if (ndx != EVERYPM_ID) break;

	//Attack Level
	case LevelAttack_ID:
		v = plugin->_params[LevelAttack_ID];
		if (controls_enabled)
			fdrAttL.set_pos((int)v);

		{
		 char tmp[127];
		 sprintf(tmp, "%.1f ms", v/10.);
		 lblAttLV.SetWindowText(tmp);
		 
		}
		if (ndx != EVERYPM_ID) break;
	
	//Release Gain
	case LevelRelease_ID:
		v = plugin->_params[LevelRelease_ID];
		if (controls_enabled)
			fdrRelL.set_pos((int)v);

		{
		 char tmp[127];
		 sprintf(tmp, "%.1f ms",v/10.);
		 lblRelLV.SetWindowText(tmp);
		 
		}
		if (ndx != EVERYPM_ID) break;

	//Look Ahead Time
	case LookAhead_ID:
		v = plugin->_params[LookAhead_ID];
		if (controls_enabled)
			fdrLA.set_pos((int)v);

		{
		 char tmp[127];
		 sprintf(tmp, "%s %.1f ms","LookAhead:     ", v/10.);
		 lblLA.SetWindowText(tmp);
		 
		}
		if (ndx != EVERYPM_ID) break;

	//Output Gain
	case OutputGain_ID:	
		v  = plugin->_params[OutputGain_ID];
				
		if (controls_enabled) 
			fdrOG.set_pos((int)v);
		
		{
		 char tmp[127];
		 sprintf(tmp, "%s %.1fdB","Output Gain:    ", v/10.);
		 lblOG.SetWindowText(tmp);
		 
		}
		if (ndx != EVERYPM_ID) break;
	
	//Side Cahin 
	case SideChain_ID:
		v = plugin->_params[SideChain_ID];
		if(controls_enabled) 
			chkSC.set_check((int)v);
		if (ndx != EVERYPM_ID) break;

	//AGC
	case AGC_ID:
		v  = plugin->_params[AGC_ID];
		if(controls_enabled)  
			chkAGC.set_check((int)v);
		if (ndx != EVERYPM_ID) break;

	}
}

// TIMER Recalcs and Redraw/Refresh 
///////////////////////////////////////////////////////////////////////////
void iDynamicaPluginDlg::OnTimer(UINT_PTR)	
{
        // recalc
        dword	level;
		dword	gain; 
		dword	clip;
	
		
		//----- Level
		plugin->get_dsp_register("mon_LEV",&level);
		plugin->set_dsp_register("mon_LEV",0x00000000);

		//float level_f = ((float)(long)(level-0x78000000L)/16777216.0f+(float)(long)(level-0x78000000L)/33554432.0f);
		//double separator_l=pm_offset+pm_width-(int) ((float)pm_width*((level_f-pm_max))/pm_min);

		double level_f =((float)(long)(level)) / (float) (pow (2.0f,31)-1.0);
		if (level_f<4.6e-10) level_f = 4.6e-10;
		level_f = 20.0 * log (level_f*4.0) / log (10.0);
		double separator_l=pm_offset + pm_width + int ((level_f-pm_max) * pm_width / (-pm_min));
        
		
		//----- Gain
		plugin->get_dsp_register("GaindB",&gain);

		double gain_f =((float)(long)(gain)) / (float) (pow (2.0f,31)-1.0);
		gain_f = exp(gain_f/0.045115616 );
		gain_f = 20.0 * log (gain_f) / log (10.0);
		int	offset = -1;
		if (gain_f < 0.0) offset = -2;
		
		double separator_r= pm_offset + pm_width / 2.0 + gain_f * pm_width / (84) + offset;

		
		//----- Actualiza imagenes
		monitor.set_value((int) separator_l, (int) separator_r,0,0);
        monitor.redraw(); 
				
		
		//----- Clipping
		plugin->get_dsp_register("gainCLIP",&clip);
		
		double clip_f =((float)(long)(clip)) / (float) (pow (2.0f,31)-1.0);
		if (clip_f<4.6e-10) clip_f = 4.6e-10;
		clip_f = 20.0 * log (clip_f); 

		
		if (clip_f < 0.0) 
		{
			plugin->_params[OutputGain_ID]=plugin->_params[OutputGain_ID] + (int) ((double) clip_f );
			plugin->set_param(OutputGain_ID, plugin->_params[OutputGain_ID]);
			
		}
}

//---------------------------------------------
void iDynamicaPluginDlg::on_destroy()
{
 turn_on(0);
}
