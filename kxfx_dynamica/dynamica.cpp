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

// Dynamica
// (c) eYagos, 2004-2005. All rights reserved       

#include "stdafx.h" 

// --------------- the above should be a 'standard' beginning of any .cpp file


#include "dynamica.h" // effect class
#include "da_dynamica.cpp" // effect source  

typedef enum _params_id{
	//Controls
	INPUTGAIN_ID=0,
	OUTPUTGAIN_ID,
	GAINATTACK_ID,
	GAINRELEASE_ID,
	LEVELATTACK_ID,
	LEVELRELEASE_ID,
	THRESHOLD1_ID,
	THRESHOLD2_ID,
	MODE1_ID,
	RATIO1_ID,
	MODE2_ID,
	RATIO2_ID, 
	MODE3_ID,
	RATIO3_ID,
	LOOKAHEAD_ID,
	SIDECHAIN_ID,
	KNEETYPE_ID
};

enum _extras_id{
	CLIPP_IN_VAL_ID=20,
	CLIPP_OUT_VAL_ID,

	SIDECHAIN_N_ID,
	SIDECHAIN_S_ID,

	KTHARD_ID,
	KTSOFT_ID,
	KTSOFTER_ID,

	RESETIG_ID,
	RESETOG_ID,
	
	LBL_HEADER_ID,
	LBL_TH1_ID,
	LBL_TH2_ID,
	LBL_RATIO1_ID,
	LBL_RATIO2_ID,
	LBL_RATIO3_ID,
	LBL_ATTACK_ID, 
	LBL_RELEASE_ID,
	LBL_ATTACK_L_ID, 
	LBL_RELEASE_L_ID,
	LBL_OUTPUTGAIN_ID,
	LBL_INPUTGAIN_ID,
	LBL_LOOKAHEAD_ID,
	LBL_DYNAMICGAIN_ID,

	GRAPH_ID

};


/* default plugin parameters */
// =====================================================================================
static kxparam_t default_params[DYNAMICA_PARAMS_COUNT] = {
	0,0,476,600,140,600,-200,-1000,0,10,0,10,0,10,30,0,0};
	
/*
Input Gain
Output Gain
Attack
Release
Level Attack
Level Release
Threshold 1
Threshold 2 
Mode 1
Ratio 1
Mode 2
Ratio 2
Mode 3
Ratio 3
Look Ahead
Side Chain
Knee Type
*/

int iDynamicaPlugin::set_defaults()  
{
	set_all_params(default_params); 
	return 0;
}

/* for kX Automation: param description (see kxplugin.h for details on kx_fxparam_descr) */
// =====================================================================================
static kx_fxparam_descr _param_descr[DYNAMICA_PARAMS_COUNT] = {
	{"Input Gain",				KX_FXPARAM_DB,  -600, 0		},
	{"Output Gain",				KX_FXPARAM_DB,  -600, 600	},
	{"Gain Attack",				KX_FXPARAM_MS,     0, 1000	},
	{"Gain Release",			KX_FXPARAM_MS,	   0, 1000	},
	{"Level Attack",			KX_FXPARAM_MS,     0, 1000	},
	{"Level Release",			KX_FXPARAM_MS,     0, 1000	},
	{"Threshold 1",				KX_FXPARAM_DB, -1000, 0		},
	{"Threshold 2",				KX_FXPARAM_DB, -1000, 0		},
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

/* presets */ 
// =====================================================================================
#define P (kxparam_t)

static kxparam_t presets[]=
{
 // don't include 'default' preset
 // P "preset_name", p1, p2, ... p(PARAMS_COUNT),
 // NULL
	P "2:1, -20, Fast Drums"						,0,40,476,600,140,600,-200,-1000,0,83,0,10,0,10,30,0,0,
	P "3:1 Expander <10dB"							,0,0,200,740,140,600,-100,-388,0,10,1,125,0,297,30,0,0,
	P "3:1, -18, +3dB, Drums"						,0,30,498,575,140,600,-180,-1000,0,125,0,10,0,10,30,0,0,
	P "4:1, -24, Very Fast Attack"					,0,60,200,740,140,600,-240,-1000,0,156,0,10,0,10,30,0,0,
	P "Classic SoftKnee"							,0,30,400,679,140,600,-100,-270,0,180,0,106,1,29,30,0,2,
	P "De-Esser (High S) (S.Chain BP 5.5-14kHz)"	,0,0,200,720,200,600,-300,-1000,0,106,0,10,0,10,20,1,0,
	P "De-Esser Hard (S.Chain BP 4-12kHz)"			,0,0,200,720,200,600,-350,-1000,0,125,0,10,0,10,20,1,0,
	P "De-Esser Light (S.Chain BP 4-12kHz)"			,0,0,200,720,200,600,-240,-1000,0,53,0,10,0,10,20,1,0,
	P "De-Esser Medium (S.Chain BP 4-12kHz)"		,0,0,200,720,200,600,-300,-1000,0,83,0,10,0,10,20,1,0,
	P "Drum Machine Limiter"						,0,140,400,680,140,600,-180,-1000,0,189,1,38,0,10,30,0,0,
	P "Electric Guitar Gate"						,0,120,200,680,140,600,-240,-457,0,189,1,59,1,292,30,0,0,
	P "Heavy Master Limit, -18 Thres"				,0,80,492,546,140,600,-180,-1000,0,494,0,10,0,10,30,0,0,
	P "Limit Hard -6dB"								,0,0,340,435,140,600,-60,-1000,0,494,0,10,0,10,30,0,0,
	P "Limit Soft -12dB, with Boost"				,0,80,340,435,140,600,-120,-1000,0,494,0,10,0,10,30,0,2,
	P "Limit Soft -24dB, with Boost"				,0,160,340,435,140,600,-240,-1000,0,494,0,10,0,10,30,0,2,
	P "Limit Soft -6dB, with Boost"					,0,40,340,435,140,600,-60,-1000,0,494,0,10,0,10,30,0,2,
	P "Noise Gate @ 10dB"							,0,40,200,740,140,600,-100,-196,0,10,1,240,0,318,30,0,0,
	P "Noise Gate @ 20dB"							,0,40,200,740,140,600,-200,-308,0,10,1,216,0,318,30,0,0,
	P "Noisy Hot Guitar"							,0,120,260,775,140,660,-200,-290,0,187,1,59,1,287,40,0,0,
	P "Power Drums"									,0,120,495,435,140,600,-180,-1000,0,494,0,10,0,10,30,0,2,
	P "Radio Limit, fast release, with Boost"		,0,180,540,200,140,600,-300,-1000,0,325,0,10,0,10,30,0,0,
	P "RealAudio® Compander"						,0,3,0,520,140,695,-90,-400,0,150,0,20,1,83,30,0,0,
	P "Sharp Attack Bass"							,0,-10,435,546,200,600,-180,-1000,0,10,0,125,0,10,30,0,0,
	P "Thump Bass Comp"								,0,40,460,540,140,600,-200,-1000,0,106,0,10,0,10,30,0,0,
	P "Uber-Punch 1"								,0,70,540,695,140,400,-200,-1000,0,923,1,38,0,10,30,0,2,
	P "Vocal Comp, -24, +12dB, Fast"				,0,120,200,680,140,600,-240,-1000,0,189,1,38,0,10,30,0,0,
	P "Vocal Comp, 8:1, -24, Fast Attack"			,0,80,200,540,140,600,-240,-1000,0,229,0,10,0,10,30,0,0,
	P "Vocal Limit, -26 Thres, +16dB"				,0,160,200,800,140,600,-260,-1000,0,494,0,10,0,10,30,0,0,
	P "Normalizer"									,0,400,400,295,140,1000,-450,-510,0,449,0,10,1,173,30,0,2,
	P "Normalizer v2"								,0,-23,744,200,61,1095,-30,-640,0,391,0,391,1,156,40,0,1,
 NULL // must be here
};

#undef P

const kxparam_t *iDynamicaPlugin::get_plugin_presets()
{
 return presets;
}


/************************/
/* Plugin Implemenation */
/************************/

int iDynamicaPlugin::get_param(int ndx, kxparam_t *value) // ndx corresponds to 'logical' parameter id
{
	*value = _params[ndx]; 
	return 0;
}

/* Various functions for the plugin calculations */
// =====================================================================================
double iDynamicaPlugin::Log_Dane_Interp(double value, int res)
 {
	int		CN231=2147483647, long_res, shifts, exp; 
	double	LN2=.69314718055994530942, x, m;
	unsigned int val;
	
	long_res = static_cast <int> (log((float)res)/LN2 + 1.); 
	val = static_cast <int> (fabs(value) * CN231); 
	shifts = 32 - static_cast <int> (log((float)val)/LN2 + 1.); 
	val = val << shifts; 
	exp = res  - shifts;
	if (exp >= 0) {val = val << 1;}
	exp = exp + 1;
	shifts = (long_res + 1);
	if (exp < 0 ) {shifts = shifts - exp; exp=0;};
	val = val >> shifts;
	m = (double) (val) * 32 / CN231;
	exp = exp << (32 - shifts);
	val = exp + val; 
	x = (double) (val) / CN231; 
	x=x+.529792754208057e-2*m*(m-1.0001381175098221129)*(m-2.6332719299470813950);
	return x;
 }

double iDynamicaPlugin::Exp_Dane_Interp(double value)
 {
	return exp((value - 0.99821340874364500) / 0.04508325259950360);
 }
/* Converts from time-slider value to time value */
double iDynamicaPlugin::time_to_val(kxparam_t value)
 {
	double LN10=2.302585093, x;
	int	scale=1000;
	x = exp( LN10 * ( 5. * value - 1. * scale) / scale);
	return x;
 }
/* Converts form time-lineal value to time-exp value */
double iDynamicaPlugin::time_to_exp(double value)
 {
	double x;
	//x = 1.0 - 1.0 / (value / 1000 * SAMPLES_PER_SEC);
	x = exp (-1.0 / (SAMPLES_PER_SEC / 1000 * value));
	return x;
 }
/* Converts from ratio-slider value to ratio value */
double iDynamicaPlugin::ratio_to_val(kxparam_t value)
 {

	double x; 
	double LN10=2.302585093;
	double LT=1000.0;
	double L1=484.0;
	double L2=516.0; 
	if (value<=L1)
	{
		x = value / L1;
		x = exp(2.*LN10*(x*L1-10.)/L1);
		x = ((int) ((x + 0.05)*10)) / 10.0; 
	}
	else if (value>=L2) 
	{
		x = (LT - value) / L1;
		x = exp(2.*LN10*(x*L1-10.)/L1);
		x = -((int) ((x + 0.05)*10)) / 10.0;
	}
	else
	{
		x = 10000.0;
	}
	return x;
 }
/* Converts from dB-slider value to dB value */
double iDynamicaPlugin::dB_to_val (kxparam_t value)
 {
	double x;
	x = value / 10.0;
	if (value<-999) x = -160;
	return x;
 }
/* Calculates several parameters based on the actual values of the 'user controled' parameters
and set them into the DANE code */
int iDynamicaPlugin::general_prameters()
{
	double	th1, th2, rat1, rat2, rat3, v_medio1, v_medio2, a, GO;
	int		a1;

	th1	= dB_to_val(_params[THRESHOLD1_ID]);  
	th1 = Log_Dane_Interp(dBtoG(th1)*0.25,31);
	th2	= dB_to_val(_params[THRESHOLD2_ID]);
	th2 = Log_Dane_Interp(dBtoG(th2)*0.25,31);
	rat1 = ratio_to_val(_params[RATIO1_ID]);
	rat2 = ratio_to_val(_params[RATIO2_ID]);
	rat3 = ratio_to_val(_params[RATIO3_ID]);
	if (_params[MODE1_ID]==0) rat1 = 1. / rat1;
	if (_params[MODE2_ID]==0) rat2 = 1. / rat2;
	if (_params[MODE3_ID]==0) rat3 = 1. / rat3;
	
	//----Soft knee calculations
	a = 0.025;
	switch (_params[KNEETYPE_ID])
		{
		case 0:
			v_medio1 = 0.0;
			v_medio2 = 0.0;
			a1 = static_cast < int > ( 1/a );
			break;
		case 1:
			v_medio1 = (rat2 - rat1) / 4.* a;
			v_medio2 = (rat3 - rat2) / 4.* a;
			a1 = static_cast < int > ( 1/a );
			break;
		case 2:
			v_medio1 = (rat2 - rat1) / 4.* (a * 2.);
			v_medio2 = (rat3 - rat2) / 4.* (a * 2.);
			a1 = static_cast < int > ( 1/(a * 2.)) ;
			break;
		}
	set_dsp_register(R_soft_long, a1);
	set_dsp_register(R_v_medio1, _dbl_to_dspword(v_medio1));
	set_dsp_register(R_v_medio2, _dbl_to_dspword(v_medio2));
	
	//---- Offset calculation
	GO = th1-th1*rat2+th2*rat2;
	set_dsp_register(R_Offset, _dbl_to_dspword(GO));
	
	return 0;
}

 

/* set_param sets 'logical' values by writing to (one or a group of) 'physical' registers */
// =====================================================================================
int iDynamicaPlugin::set_param(int ndx, kxparam_t value)
{
	double x, v_F;
	int	v_I;
	kxparam_t t;

	_params[ndx] = value;

	switch (ndx)
	{
	case INPUTGAIN_ID:
		// Input Gain DSP parameters 
		x = dBtoG(value / 10.);
		v_I = static_cast < int > ( x ); //convert to int to truncate
		v_F = x - v_I; //get fractional part
		 
		set_dsp_register(R_iGAIN_I, v_I); // writing value to level register with conversion from double to dsp's dword
		set_dsp_register(R_iGAIN_F, _dbl_to_dspword(v_F));
		break;

	case OUTPUTGAIN_ID:
		// MakeUp Gain DSP parameters 
		x = dBtoG(value / 10.);
		v_I = static_cast < int > ( x ); //convert to int to truncate
		v_F = x - v_I; //get fractional part
		 
		set_dsp_register(R_GAIN_I, v_I); // writing value to level register with conversion from double to dsp's dword
		set_dsp_register(R_GAIN_F, _dbl_to_dspword(v_F));
		break;

	case GAINATTACK_ID:
		// Attack time for gain processor DSP parameters
		x = time_to_val (value);//calculates the time value from slider position
		x = time_to_exp (x);//value to be passed to DANE
		set_dsp_register(R_Attack, _dbl_to_dspword(x));
		//set_dsp_register(R_Attack, _dbl_to_dspword(0.0));
		break;

	case GAINRELEASE_ID:
		// Release time for gain processor DSP parameters
		x = time_to_val (value);//calculates the time value from slider position
		x = time_to_exp (x);//value to be passed to DANE
		set_dsp_register(R_Release, _dbl_to_dspword(x));
		//set_dsp_register(R_Release, _dbl_to_dspword(0.0));
		break;

	case LEVELATTACK_ID:
		// Attack time for level detector DSP parameters
		x = time_to_val (value);//calculates the time value from slider position
		x = time_to_exp (x);//value to be passed to DANE
		set_dsp_register(R_AttackL, _dbl_to_dspword(x));
		//set_dsp_register(R_AttackL, _dbl_to_dspword(0.0));
		break;

	case LEVELRELEASE_ID:
		// Release time for level detector DSP parameters
		x = time_to_val (value);//calculates the time value from slider position
		x = time_to_exp (x);//value to be passed to DANE
		set_dsp_register(R_ReleaseL, _dbl_to_dspword(x));
		//set_dsp_register(R_ReleaseL, _dbl_to_dspword(0.0));
		break;

	case THRESHOLD1_ID:
		// Threshold 1 DSP parameters
		x = dB_to_val(value);//calculates the threshold value from slider position
		x = Log_Dane_Interp((double) dBtoG(x) * 0.25, (int) 31);//value to be passed to DANE
		set_dsp_register(R_Thres1, _dbl_to_dspword(x));
		general_prameters();
		break;

	case THRESHOLD2_ID:
		// Threshold 2 DSP parameters
		x = dB_to_val(value);//calculates the threshold value from slider position
		x = Log_Dane_Interp((double) dBtoG(x) * 0.25, (int) 31);//value to be passed to DANE
		set_dsp_register(R_Thres2, _dbl_to_dspword(x));
		general_prameters();
		break;

	case MODE1_ID:
		// Mode of Ratio 1 (compress or expand) DSP parameters
		set_param(RATIO1_ID, _params[RATIO1_ID]);
		general_prameters();
		break;

	case RATIO1_ID:
		// Ratio 1 DSP parameters
		x = ratio_to_val (value);//calculates the ratio value from slider position
		if (_params[MODE1_ID]==0)
		{
			v_I = 0; //convert to int to truncate
			v_F = 1/x; //get fractional part
		}
		else 
		{
			v_I = static_cast < int > ( x ); //convert to int to truncate
			v_F = x - v_I; //get fractional part
		}
		set_dsp_register(R_Rat1_i, v_I); // writing value to level register with conversion from double to dsp's dword
		set_dsp_register(R_Rat1_f, _dbl_to_dspword(v_F));
		general_prameters();
		break;

	case MODE2_ID:
		// Mode of Ratio 2 (compress or expand) DSP parameters
		set_param(RATIO2_ID, _params[RATIO2_ID]);
		general_prameters();
		break;

	case RATIO2_ID:
		// Ratio 2 DSP parameters
		x = ratio_to_val (value);//calculates the ratio value from slider position
		if (_params[MODE2_ID]==0)
		{
			v_I = 0; //convert to int to truncate
			v_F = 1/x; //get fractional part
		}
		else 
		{
			v_I = static_cast < int > ( x ); //convert to int to truncate
			v_F = x - v_I; //get fractional part
		}
		set_dsp_register(R_Rat2_i, v_I); // writing value to level register with conversion from double to dsp's dword
		set_dsp_register(R_Rat2_f, _dbl_to_dspword(v_F));
		general_prameters();
		break;

	case MODE3_ID:
		// Mode of Ratio 3 (compress or expand) DSP parameters
		set_param(RATIO3_ID, _params[RATIO3_ID]);
		general_prameters();
		break;

	case RATIO3_ID:
		// Ratio 3 DSP parameters
		x = ratio_to_val (value);//calculates the ratio value from slider position
		if (_params[MODE3_ID]==0)
		{
			v_I = 0; //convert to int to truncate
			v_F = 1/x; //get fractional part
		}
		else 
		{
			v_I = static_cast < int > ( x ); //convert to int to truncate
			v_F = x - v_I; //get fractional part
		}
		set_dsp_register(R_Rat3_i, v_I); // writing value to level register with conversion from double to dsp's dword
		set_dsp_register(R_Rat3_f, _dbl_to_dspword(v_F));
		general_prameters();
		break;

	case KNEETYPE_ID:
		// Knee Type DSP parameters
		set_param(RATIO3_ID, _params[RATIO3_ID]);
		general_prameters();
		break;

	case LOOKAHEAD_ID:
		// Look ahead time DSP parameters
		x = value / 10.0;  // range is 0.0 to 4.0
		// reading delay start (write) address, adding predelay time to it and writing to delay read address
		// left channel predelay
		get_tram_addr(R_delW1, (dword *) &t);
		set_tram_addr(R_delR1, (dword)(t + 2 + x * SAMPLES_PER_MSEC));
		// right channel predelay
		get_tram_addr(R_delW2, (dword *) &t);
		set_tram_addr(R_delR2, (dword)(t + 2 + x * SAMPLES_PER_MSEC));
		break;
	
	case SIDECHAIN_ID:
		// Side chain DSP parameters
		// if off write normal "read inputs" instructions
		if (!value)
		{
			write_instruction(0, 0x0u, R_tmp1, 0x2040u,0x4000u, 0x204f);
			write_instruction(1, 0x0u, R_tmp2, 0x2040u,0x4001u, 0x204f);
			write_instruction(2, 0x0u, R_delW1, R_tmp1,0x2040u,0x2040u);
			write_instruction(3, 0x0u, R_delW2, R_tmp2,0x2040u,0x2040u);
		}
		// if on write side chain "read inputs" instructions
		else
		{
			write_instruction(0, 0x0u, R_tmp1, 0x2040u,0x4002u, 0x204f);
			write_instruction(1, 0x0u, R_tmp2, 0x2040u,0x4003u, 0x204f);
			write_instruction(2, 0x0u, R_delW1, 0x4000u,0x2040u,0x2040u);
			write_instruction(3, 0x0u, R_delW2, 0x4001u,0x2040u,0x2040u);
		}
		break;
 
	}

    // we need do synchronize the GUI part, if set_param was called by the mixer or kX Automation
	if (cp) ((iDynamicaPluginDlg*) cp)->sync(ndx);

	return 0;
}

int iDynamicaPlugin::set_all_params(kxparam_t* values)
{
	for (int i = 0; i < DYNAMICA_PARAMS_COUNT; i++) set_param(i, values[i]);
	set_dsp_register(R_mon_IN_L,0x00000000);
	set_dsp_register(R_mon_IN_R,0x00000000);
	// Output
	set_dsp_register(R_mon_OUT_L,0x00000000);
	set_dsp_register(R_mon_OUT_R,0x00000000);
	general_prameters();

	return 0;
}

/* Plugin Generals */
// =====================================================================================
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



/* Timer */
// =====================================================================================
BEGIN_MESSAGE_MAP(iDynamicaPluginDlg, CKXPluginGUI)
	ON_BN_CLICKED (CLIPP_IN_VAL_ID, OnClick_IN)
	ON_BN_CLICKED (CLIPP_OUT_VAL_ID, OnClick_OUT)
        ON_MESSAGE(WM_UPDATE_STATIC, OnUpdateStatic)  
	ON_WM_TIMER()
END_MESSAGE_MAP()


iDynamicaPluginDlg::iDynamicaPluginDlg(iDynamicaPlugin *plg, kDialog *parent_,kFile *mf_) 
 : CKXPluginGUI(parent_,mf_) 
{ 
		plugin=plg;
		// kxmixer_skin=mf_; // save kxmixer skin pointer
		
		timer_id=(UINT_PTR)-1;

		// Input - Output monitors numbers
		VU_px_0dB = 13; // 0dB postion 
		VU_sca_pxdB = 2.33333f; // pixeles/dB 
		// Level monitor numbers
		LEV_px_0dB = 116; // 0dB postion 
		LEV_sca_pxdB = 1.16f; // pixeles/dB 
		// Dynamic Gain monitor numbers
		GAIN_px_0dB = 58; // 0dB postion 
		GAIN_max_dB = 48.; // Max scale
		// Dynamic Gain label counter 
		steps_gain = 0;
		max_steps_gain = 2;
		
		// Clipping values
		Clipp_in = -160.0;
		Clipp_out = -160.0;

		//VU positions
		pos_in_L = 0;
		pos_in_R = 0;
		pos_out_L = 0;
		pos_out_R = 0;
		pos_level = 0;
		pos_gain = 0;
}

iDynamicaPluginDlg::~iDynamicaPluginDlg()
{ 
         	turn_on(0); 
}

int iDynamicaPluginDlg::turn_on(int what) 
{
 if((what==1) && (timer_id==(UINT_PTR)-1))
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
// =====================================================================================
void iDynamicaPluginDlg::init()
{
 
	char tmp[MAX_LABEL_STRING];
	
	CKXPluginGUI::init(); // this is necessary 
	// init() should be called -after- skin initialization 
 
	//--------------------------------------------------------------------------------------
	// custom dialog initialization here...  
	// (title, Width, Height) (Width= image_width - 3 px) (Height= image_height - 49 px)
	create_dialog(plugin_name(dynamica), 534, 245);  
	grpbox.hide();//hide useless GUI elements       
	
	// creat font for vu text    
	vufont.CreatePointFont(70,"Tahoma");
	vufont_gain.CreatePointFont(81,"Tahoma"); 
	vufont_clipp.CreatePointFont(70,"Arial");

	//Colors 
	color_txt_values = RGB(165,175,186);
	color_bkg_values = RGB(12,37,69); 
	color_txt_dynamicgain = RGB(174,174,174);
	color_txt_clipp = RGB(102,102,106);  
	color_txt1_clipp = RGB(33,33,34);  
	color_txt2_clipp = RGB(203,0,0);   
	
	// Header
	w_label.hide();//remove original header  
	headerfont.CreatePointFont(80,"Tahoma");  
	lbl_Header.Create("    ",WS_CHILD | WS_VISIBLE | SS_LEFT ,CRect(CPoint(6,4),CSize(400,14)), this, LBL_HEADER_ID);
	lbl_Header.SetBkColor(CLR_NONE);
	lbl_Header.SetTextColor(RGB(198,198,198));
	lbl_Header.SetFont(&headerfont,NULL); 
	sprintf(tmp, " Dynamics Processor - %s",  get_plugin()->ikx->get_device_name());
	lbl_Header.SetWindowText(tmp);

	// Knobs 
	k_OutputGain.SetBitmaps(mf.load_image("k_og.jpg"));
	k_OutputGain.Create("",WS_CHILD| WS_VISIBLE|SS_NOTIFY, CRect(CPoint(454,155),CSize(26,26)), this);
    k_OutputGain.SetID(OUTPUTGAIN_ID);
	k_OutputGain.SetTicks(1200,1,2);
	
	k_LevelRelease.SetBitmaps(mf.load_image("k_lr.jpg"));
	k_LevelRelease.Create("",WS_CHILD| WS_VISIBLE|SS_NOTIFY, CRect(CPoint(403,155),CSize(26,26)), this);
    k_LevelRelease.SetID(LEVELRELEASE_ID);
	k_LevelRelease.SetTicks(1120,1,4);
	
	k_LevelAttack.SetBitmaps(mf.load_image("k_la.jpg"));
	k_LevelAttack.Create("",WS_CHILD| WS_VISIBLE|SS_NOTIFY, CRect(CPoint(352,155),CSize(26,26)), this);
    k_LevelAttack.SetID(LEVELATTACK_ID);
	k_LevelAttack.SetTicks(1000,1,4);

	k_Ratio3.SetBitmaps(mf.load_image("k_r3.jpg"));
	k_Ratio3.Create("",WS_CHILD| WS_VISIBLE|SS_NOTIFY, CRect(CPoint(153,155),CSize(26,26)), this);
    k_Ratio3.SetID(RATIO3_ID);
	k_Ratio3.SetTicks(980,1,4);

	k_Threshold2.SetBitmaps(mf.load_image("k_th2.jpg"));
	k_Threshold2.Create("",WS_CHILD| WS_VISIBLE|SS_NOTIFY, CRect(CPoint(102,155),CSize(26,26)), this);
    k_Threshold2.SetID(THRESHOLD2_ID);
	k_Threshold2.SetTicks(1000,1,4);

	k_InputGain.SetBitmaps(mf.load_image("k_ig.jpg"));
	k_InputGain.Create("",WS_CHILD| WS_VISIBLE|SS_NOTIFY, CRect(CPoint(51,155),CSize(26,26)), this);
    k_InputGain.SetID(INPUTGAIN_ID);
	k_InputGain.SetTicks(720,1,4);

	k_Threshold1.SetBitmaps(mf.load_image("k_th1.jpg"));
	k_Threshold1.Create("",WS_CHILD| WS_VISIBLE|SS_NOTIFY, CRect(CPoint(51,61),CSize(26,26)), this);
    k_Threshold1.SetID(THRESHOLD1_ID);
	k_Threshold1.SetTicks(1000,1,4);

	k_Ratio1.SetBitmaps(mf.load_image("k_r1.jpg"));
	k_Ratio1.Create("",WS_CHILD| WS_VISIBLE|SS_NOTIFY, CRect(CPoint(102,61),CSize(26,26)), this);
    k_Ratio1.SetID(RATIO1_ID);
	k_Ratio1.SetTicks(980,1,4);

	k_Ratio2.SetBitmaps(mf.load_image("k_r2.jpg"));
	k_Ratio2.Create("",WS_CHILD| WS_VISIBLE|SS_NOTIFY, CRect(CPoint(153,61),CSize(26,26)), this);
    k_Ratio2.SetID(RATIO2_ID);
	k_Ratio2.SetTicks(980,1,4);

	k_GainAttack.SetBitmaps(mf.load_image("k_ga.jpg"));
	k_GainAttack.Create("",WS_CHILD| WS_VISIBLE|SS_NOTIFY, CRect(CPoint(352,61),CSize(26,26)), this);
    k_GainAttack.SetID(GAINATTACK_ID);
	k_GainAttack.SetTicks(1000,1,4); 

	k_GainRelease.SetBitmaps(mf.load_image("k_gr.jpg"));
	k_GainRelease.Create("",WS_CHILD| WS_VISIBLE|SS_NOTIFY, CRect(CPoint(403,61),CSize(26,26)), this);
    k_GainRelease.SetID(GAINRELEASE_ID);
	k_GainRelease.SetTicks(1000,1,4);

	k_LookAhead.SetBitmaps(mf.load_image("k_loa.jpg"));
	k_LookAhead.Create("",WS_CHILD| WS_VISIBLE|SS_NOTIFY, CRect(CPoint(454,61),CSize(26,26)), this);
    k_LookAhead.SetID(LOOKAHEAD_ID);
	k_LookAhead.SetTicks(40,0.3f,1);

	// Graph
    graph.SetBitmaps(mf.load_image("rejilla.jpg"));
	graph.Create("",WS_CHILD| WS_VISIBLE|SS_NOTIFY, CRect(CPoint(208,47),CSize(116,116)), this);
	graph.SetID(GRAPH_ID);

	// Buttons
	c_Mode1.set_bitmaps( 
		mf.load_image("MR1_0.jpg"),
		mf.load_image("MR1_1.jpg"),
		mf.load_image("MR1_2.jpg"),
		mf.load_image("MR1_3.jpg"));
	create_checkbox(c_Mode1, MODE1_ID, "Compress/Expand Above Threshold 1", 93, 18, 32); 
	c_Mode1.set_method(kMETHOD_TRANS);

	c_Mode2.set_bitmaps( 
		mf.load_image("MR2_0.jpg"),
		mf.load_image("MR2_1.jpg"),
		mf.load_image("MR2_2.jpg"),
		mf.load_image("MR2_3.jpg"));
	create_checkbox(c_Mode2, MODE2_ID, "Compress/Expand Bellow Threshold 1", 144, 18, 32); 
	c_Mode2.set_method(kMETHOD_TRANS);
	
	c_Mode3.set_bitmaps( 
		mf.load_image("MR3_0.jpg"),
		mf.load_image("MR3_1.jpg"),
		mf.load_image("MR3_2.jpg"),
		mf.load_image("MR3_3.jpg"));
	create_checkbox(c_Mode3, MODE3_ID, "Compress/Expand Bellow Threshold 2", 144, 112, 32); 
	c_Mode3.set_method(kMETHOD_TRANS);
		
	c_KTHard.set_bitmaps( 
		mf.load_image("led1_0.jpg"),
		mf.load_image("led1_1.jpg"),
		mf.load_image("led1_2.jpg"),
		mf.load_image("led1_3.jpg"));
	create_checkbox(c_KTHard, KTHARD_ID, "Hard Knee", 200, 171, 29); 
	c_KTHard.set_method(kMETHOD_TRANS);

	c_KTSoft.set_bitmaps( 
		mf.load_image("led1_0.jpg"),
		mf.load_image("led1_1.jpg"),
		mf.load_image("led1_2.jpg"),
		mf.load_image("led1_3.jpg")); 
	create_checkbox(c_KTSoft, KTSOFT_ID, "Soft Knee", 210, 171, 25); 
	c_KTSoft.set_method(kMETHOD_TRANS);

	c_KTSofter.set_bitmaps( 
		mf.load_image("led1_0.jpg"),
		mf.load_image("led1_1.jpg"),
		mf.load_image("led1_2.jpg"),
		mf.load_image("led1_3.jpg"));
	create_checkbox(c_KTSofter, KTSOFTER_ID, "Softer Knee", 220, 171, 34); 
	c_KTSofter.set_method(kMETHOD_TRANS);

	sta_hard.create("",201,201, mf.load_image("kH.jpg"),this,1);
	sta_soft.create("",201,201, mf.load_image("kS.jpg"),this,1);
	sta_softer.create("",201,201, mf.load_image("kST.jpg"),this,1);

	c_SideChain_N.set_bitmaps( 
		mf.load_image("led2_0.jpg"),
		mf.load_image("led2_1.jpg"),
		mf.load_image("led2_2.jpg"),
		mf.load_image("led2_3.jpg"));
	create_checkbox(c_SideChain_N, SIDECHAIN_N_ID, "Normal Input Mode", 5, 188, 20); 
	c_SideChain_N.set_method(kMETHOD_TRANS);

	c_SideChain_S.set_bitmaps( 
		mf.load_image("led2_0.jpg"),
		mf.load_image("led2_1.jpg"),
		mf.load_image("led2_2.jpg"),
		mf.load_image("led2_3.jpg"));
	create_checkbox(c_SideChain_S, SIDECHAIN_S_ID, "SideChain Input Mode", 17, 188, 20); 
	c_SideChain_S.set_method(kMETHOD_TRANS);
	
	sta_normal.create("",4,198, mf.load_image("in_normal.jpg"),this,1);
	sta_side.create("",4,198, mf.load_image("in_side.jpg"),this,1);


	c_Reset_IG.set_bitmaps( 
		mf.load_image("reset_ig_0.jpg"),
		mf.load_image("reset_ig_1.jpg"),
		mf.load_image("reset_ig_0.jpg"));
	create_button(c_Reset_IG, RESETIG_ID, "Set 0dB", 46,112, 17); 
	c_Reset_IG.set_method(kMETHOD_TRANS);

	c_Reset_OG.set_bitmaps( 
		mf.load_image("reset_og_0.jpg"),
		mf.load_image("reset_og_1.jpg"),
		mf.load_image("reset_og_0.jpg"));
	create_button(c_Reset_OG, RESETOG_ID, "Set 0dB", 449,112, 17); 
	c_Reset_OG.set_method(kMETHOD_TRANS);

	// Monitors
	m_in.set_bitmaps( 
		mf.load_image("m1_low.jpg"),
		mf.load_image("m1_high.jpg"));
	m_in.create("Input", 9, 47,this,1); 
	m_in.show();
	m_in.set_value(0, 0, 0, 0);

	m_out.set_bitmaps( 
		mf.load_image("m1_low.jpg"),
		mf.load_image("m1_high.jpg"));
	m_out.create("Output", 503, 47,this,1);   
	m_out.show();
	m_out.set_value(0, 0, 0, 0);

	m_gain_lev.set_bitmaps( 
		mf.load_image("m2_low.jpg"),
		mf.load_image("m2_high.jpg"));
	m_gain_lev.create("Input Level - Dynamic Gain", 208, 171,this,0); 
	m_gain_lev.show();
	m_gain_lev.set_value(0, 0, 0, 0);

	//Labels
	lbl_Th1.Create("    ",WS_CHILD | WS_VISIBLE | SS_CENTER ,CRect(CPoint(45,103),CSize(37,9)), this, LBL_TH1_ID);
	lbl_Th1.SetBkColor(color_bkg_values);
	lbl_Th1.SetTextColor(color_txt_values);
	lbl_Th1.SetFont(&vufont,NULL);  

	lbl_Th2.Create("    ",WS_CHILD | WS_VISIBLE | SS_CENTER ,CRect(CPoint(96,196),CSize(37,9)), this, LBL_TH2_ID);
	lbl_Th2.SetBkColor(color_bkg_values);
	lbl_Th2.SetTextColor(color_txt_values);
	lbl_Th2.SetFont(&vufont,NULL);  

	lbl_Ratio1.Create("    ",WS_CHILD | WS_VISIBLE | SS_CENTER ,CRect(CPoint(96,103),CSize(37,9)), this, LBL_RATIO1_ID);
	lbl_Ratio1.SetBkColor(color_bkg_values);
	lbl_Ratio1.SetTextColor(color_txt_values);
	lbl_Ratio1.SetFont(&vufont,NULL);

	lbl_Ratio2.Create("    ",WS_CHILD | WS_VISIBLE | SS_CENTER ,CRect(CPoint(147,103),CSize(37,9)), this, LBL_RATIO2_ID);
	lbl_Ratio2.SetBkColor(color_bkg_values);
	lbl_Ratio2.SetTextColor(color_txt_values);
	lbl_Ratio2.SetFont(&vufont,NULL);

	lbl_Ratio3.Create("    ",WS_CHILD | WS_VISIBLE | SS_CENTER ,CRect(CPoint(147,196),CSize(37,9)), this, LBL_RATIO3_ID);
	lbl_Ratio3.SetBkColor(color_bkg_values);
	lbl_Ratio3.SetTextColor(color_txt_values);
	lbl_Ratio3.SetFont(&vufont,NULL);

	lbl_Attack.Create("    ",WS_CHILD | WS_VISIBLE | SS_CENTER ,CRect(CPoint(346,103),CSize(37,9)), this, LBL_ATTACK_ID);
	lbl_Attack.SetBkColor(color_bkg_values);
	lbl_Attack.SetTextColor(color_txt_values);
	lbl_Attack.SetFont(&vufont,NULL);
	
	lbl_Release.Create("    ",WS_CHILD | WS_VISIBLE | SS_CENTER ,CRect(CPoint(397,103),CSize(37,9)), this, LBL_RELEASE_ID);
	lbl_Release.SetBkColor(color_bkg_values);
	lbl_Release.SetTextColor(color_txt_values);
	lbl_Release.SetFont(&vufont,NULL);

	lbl_Lookahead.Create("    ",WS_CHILD | WS_VISIBLE | SS_CENTER ,CRect(CPoint(448,103),CSize(37,9)), this, LBL_LOOKAHEAD_ID);
	lbl_Lookahead.SetBkColor(color_bkg_values);
	lbl_Lookahead.SetTextColor(color_txt_values);
	lbl_Lookahead.SetFont(&vufont,NULL);

	lbl_InputGain.Create("    ",WS_CHILD | WS_VISIBLE | SS_CENTER ,CRect(CPoint(45,196),CSize(37,9)), this, LBL_INPUTGAIN_ID);
	lbl_InputGain.SetBkColor(color_bkg_values);
	lbl_InputGain.SetTextColor(color_txt_values);
	lbl_InputGain.SetFont(&vufont,NULL);

	lbl_Attack_L.Create("    ",WS_CHILD | WS_VISIBLE | SS_CENTER ,CRect(CPoint(346,196),CSize(37,9)), this, LBL_ATTACK_L_ID);
	lbl_Attack_L.SetBkColor(color_bkg_values);
	lbl_Attack_L.SetTextColor(color_txt_values);
	lbl_Attack_L.SetFont(&vufont,NULL);
	
	lbl_Release_L.Create("    ",WS_CHILD | WS_VISIBLE | SS_CENTER ,CRect(CPoint(397,196),CSize(37,9)), this, LBL_RELEASE_L_ID);
	lbl_Release_L.SetBkColor(color_bkg_values);
	lbl_Release_L.SetTextColor(color_txt_values); 
	lbl_Release_L.SetFont(&vufont,NULL);

	lbl_OutputGain.Create("    ",WS_CHILD | WS_VISIBLE | SS_CENTER ,CRect(CPoint(448,196),CSize(37,9)), this, LBL_OUTPUTGAIN_ID);
	lbl_OutputGain.SetBkColor(color_bkg_values);
	lbl_OutputGain.SetTextColor(color_txt_values); 
	lbl_OutputGain.SetFont(&vufont,NULL);

	lbl_DynamicGain.Create("    ",WS_CHILD | WS_VISIBLE | SS_CENTER ,CRect(CPoint(289,195),CSize(39,10)), this, LBL_DYNAMICGAIN_ID);
	lbl_DynamicGain.SetTextColor(color_txt_dynamicgain);
	lbl_DynamicGain.SetFont(&vufont_gain,NULL);
	lbl_DynamicGain.SetBkPattern(mf.load_image("bkg_gain.jpg")); 
	lbl_DynamicGain.SetWindowText("-inf");
		
	lbl_Clipp_In_Val.Create("    ",WS_CHILD | WS_VISIBLE | SS_NOTIFY | SS_CENTER ,CRect(CPoint(8,186),CSize(22,9)), this, CLIPP_IN_VAL_ID);
	lbl_Clipp_In_Val.SetTextColor(color_txt_clipp);
	lbl_Clipp_In_Val.SetFont(&vufont_clipp,NULL); 
	lbl_Clipp_In_Val.SetBkPattern(mf.load_image("bkg_clippin.jpg"));  
	lbl_Clipp_In_Val.SetWindowText("-inf");

	lbl_Clipp_Out_Val.Create("    ",WS_CHILD | WS_VISIBLE | SS_NOTIFY | SS_CENTER ,CRect(CPoint(502,186),CSize(22,9)), this, CLIPP_OUT_VAL_ID);
	lbl_Clipp_Out_Val.SetTextColor(color_txt_clipp);
	lbl_Clipp_Out_Val.SetFont(&vufont_clipp,NULL);
	lbl_Clipp_Out_Val.SetBkPattern(mf.load_image("bkg_clippout.jpg")); 
	lbl_Clipp_Out_Val.SetWindowText("-inf");

	
	turn_on(1); 
	
	controls_enabled = 1; // enable updating GUI when the parameters are changed

	sync(EVERYPM_ID); // update all 'GUI' elements to the preset plugin parameters
}

void iDynamicaPluginDlg::sync(int ndx)
{
	char c[MAX_LABEL_STRING]; 
	kxparam_t v;
	double x;
	
    // this function should 'synchronize' any labels with the parameters
	switch (ndx)
	{ 
	case EVERYPM_ID: 
		
	case THRESHOLD1_ID:
		v = plugin->_params[THRESHOLD1_ID];
		if (v < plugin->_params[THRESHOLD2_ID]) {
			plugin->set_param(THRESHOLD2_ID, v);
		}
		if (controls_enabled) k_Threshold1.SetPos((int) (v+1000));
		if (v<-999)	{sprintf(c, "-inf");} else {sprintf(c, "%.1f", v/10.);}
		lbl_Th1.SetWindowText(c);
		SetGraphPoints();
		destroyClipp(1);
		if (ndx != EVERYPM_ID) break;

	case THRESHOLD2_ID:
		v = plugin->_params[THRESHOLD2_ID]; 
		if (v > plugin->_params[THRESHOLD1_ID]) {
			plugin->set_param(THRESHOLD1_ID, v);
		}
		if (controls_enabled) k_Threshold2.SetPos((int) (v+1000));
		if (v<-999)	{sprintf(c, "-inf");} else {sprintf(c, "%.1f", v/10.);}
		lbl_Th2.SetWindowText(c);
		SetGraphPoints();
		destroyClipp(1);
		if (ndx != EVERYPM_ID) break;

	case RATIO1_ID:
		v = plugin->_params[RATIO1_ID];
		if (controls_enabled) k_Ratio1.SetPos((int) (v-10));
		x = plugin->ratio_to_val(v);
		if (x > 100.0) {sprintf(c, "%s","inf");} else {sprintf(c, "%.1f", x);};
		lbl_Ratio1.SetWindowText(c);
		SetGraphPoints();
		destroyClipp(1);
		if (ndx != EVERYPM_ID) break;

	case RATIO2_ID:
		v = plugin->_params[RATIO2_ID];
		if (controls_enabled) k_Ratio2.SetPos((int) (v-10));
		x = plugin->ratio_to_val(v);
		if (x > 100.0) {sprintf(c, "%s","inf");} else {sprintf(c, "%.1f", x);};
		lbl_Ratio2.SetWindowText(c);
		SetGraphPoints();
		destroyClipp(1);
		if (ndx != EVERYPM_ID) break;

	case RATIO3_ID:
		v = plugin->_params[RATIO3_ID];
		if (controls_enabled) k_Ratio3.SetPos((int) (v-10));
		x = plugin->ratio_to_val(v);
		if (x > 100.0) {sprintf(c, "%s","inf");} else {sprintf(c, "%.1f", x);};
		lbl_Ratio3.SetWindowText(c);
		SetGraphPoints();
		destroyClipp(1);
		if (ndx != EVERYPM_ID) break; 

	case MODE1_ID:
		v = plugin->_params[MODE1_ID];
		if (controls_enabled) {c_Mode1.set_check((int)v);c_Mode1.hide();c_Mode1.show();};
		SetGraphPoints();
		destroyClipp(1);
		if (ndx != EVERYPM_ID) break;

	case MODE2_ID:
		v = plugin->_params[MODE2_ID];
		if (controls_enabled) {c_Mode2.set_check((int)v);c_Mode2.hide();c_Mode2.show();};
		SetGraphPoints();
		destroyClipp(1);
		if (ndx != EVERYPM_ID) break;

	case MODE3_ID:
		v = plugin->_params[MODE3_ID];
		if (controls_enabled) {c_Mode3.set_check((int)v);c_Mode3.redraw();};
		SetGraphPoints();
		destroyClipp(1);
		if (ndx != EVERYPM_ID) break; 

	case GAINATTACK_ID:
		v = plugin->_params[GAINATTACK_ID];
		if (controls_enabled) k_GainAttack.SetPos((int) v);
		x = plugin->time_to_val(v);
		if (x>=10.0) {sprintf(c, "%.0f", x);} else {sprintf(c, "%.1f", x);};
		lbl_Attack.SetWindowText(c);
		destroyClipp(1);
		if (ndx != EVERYPM_ID) break;
	
	case GAINRELEASE_ID:
		v = plugin->_params[GAINRELEASE_ID];
		if (controls_enabled) k_GainRelease.SetPos((int) v);
		x = plugin->time_to_val(v);
		if (x>=10.0) {sprintf(c, "%.0f", x);} else {sprintf(c, "%.1f", x);};
		lbl_Release.SetWindowText(c); 
		destroyClipp(1);
		if (ndx != EVERYPM_ID) break;

	case LEVELATTACK_ID:
		v = plugin->_params[LEVELATTACK_ID];
		if (controls_enabled) k_LevelAttack.SetPos((int) v);
		x = plugin->time_to_val(v);
		if (x>=10.0) {sprintf(c, "%.0f", x);} else {sprintf(c, "%.1f", x);};
		lbl_Attack_L.SetWindowText(c);
		destroyClipp(1);
		if (ndx != EVERYPM_ID) break; 
	
	case LEVELRELEASE_ID: 
		v = plugin->_params[LEVELRELEASE_ID];
		if (controls_enabled) k_LevelRelease.SetPos((int) v);		
		x = plugin->time_to_val(v);
		if (x>=10.0) {sprintf(c, "%.0f", x);} else {sprintf(c, "%.1f", x);};
		lbl_Release_L.SetWindowText(c); 
		destroyClipp(1);
		if (ndx != EVERYPM_ID) break;
		
	case OUTPUTGAIN_ID:
		v = plugin->_params[OUTPUTGAIN_ID];
		if(controls_enabled) k_OutputGain.SetPos((int) (600+v));  
		x = v/10.;
		if (x<=0.0) {sprintf(c, "%.1f", x);} else {sprintf(c, "+%.1f", x);};
		lbl_OutputGain.SetWindowText(c);
		SetGraphPoints();
		destroyClipp(1);
		if (ndx != EVERYPM_ID) break;	
		
	case INPUTGAIN_ID:
		v = plugin->_params[INPUTGAIN_ID];
		if(controls_enabled) k_InputGain.SetPos((int) (360+v));  
		x = v/10.;
		if (x<=0.0) {sprintf(c, "%.1f", x);} else {sprintf(c, "+%.1f", x);};
		lbl_InputGain.SetWindowText(c);
		destroyClipp(1);
		if (ndx != EVERYPM_ID) break;	

	case LOOKAHEAD_ID:
		v = plugin->_params[LOOKAHEAD_ID];
		if(controls_enabled) k_LookAhead.SetPos((int) v);  
		sprintf(c, "%.1f", v/10.);
		lbl_Lookahead.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;	

	case KNEETYPE_ID:
		v = plugin->_params[KNEETYPE_ID];
		if (controls_enabled) {
			switch (v)
			{
				case 0:
					c_KTHard.set_check(1);
					c_KTSoft.set_check(0);
					c_KTSofter.set_check(0);
					c_KTHard.redraw();
					c_KTSoft.redraw();
					c_KTSofter.redraw();
					sta_hard.show();
					sta_soft.hide();
					sta_softer.hide();
					break;
				case 1:
					c_KTHard.set_check(0);
					c_KTSoft.set_check(1);
					c_KTSofter.set_check(0);
					c_KTHard.redraw();
					c_KTSoft.redraw();
					c_KTSofter.redraw();
					sta_hard.hide();
					sta_soft.show();
					sta_softer.hide();
					break;
				case 2:
					c_KTHard.set_check(0);
					c_KTSoft.set_check(0);
					c_KTSofter.set_check(1);
					c_KTHard.redraw();
					c_KTSoft.redraw();
					c_KTSofter.redraw();
					sta_hard.hide();
					sta_soft.hide();
					sta_softer.show();
					break;
			}
		}
		SetGraphPoints();
		destroyClipp(1);
		if (ndx != EVERYPM_ID) break;	
		
	case SIDECHAIN_ID:
		v = plugin->_params[SIDECHAIN_ID];
		if (controls_enabled) {
			if (v) {
				c_SideChain_S.set_check(1);
				c_SideChain_S.redraw();
				c_SideChain_N.set_check(0);
				c_SideChain_N.redraw(); 
				sta_normal.hide();
				sta_side.show();
			} else {
				c_SideChain_S.set_check(0);
				c_SideChain_S.redraw();
				c_SideChain_N.set_check(1);
				c_SideChain_N.redraw(); 
				sta_normal.show();
				sta_side.hide();
			}
		}
		destroyClipp(1);
		destroyClipp(0);
		if (ndx != EVERYPM_ID) break;
	}

}

// Knobs 
LRESULT iDynamicaPluginDlg::OnUpdateStatic(WPARAM wParam, LPARAM lParam)
{
    
    switch(wParam)
	{
		case OUTPUTGAIN_ID:
			plugin->set_param(OUTPUTGAIN_ID, (int) (lParam-600)); 
 			break; 

		case LEVELRELEASE_ID:
			plugin->set_param(LEVELRELEASE_ID, (int) lParam); 
			break; 

		case LEVELATTACK_ID:
			plugin->set_param(LEVELATTACK_ID, (int) lParam ); 
			break; 
		
		case RATIO3_ID:
			plugin->set_param(RATIO3_ID, (int) (lParam+10)); 
			break;

		case THRESHOLD2_ID:
			plugin->set_param(THRESHOLD2_ID, (int) (lParam-1000)); 
			break;
		
		case INPUTGAIN_ID:
			plugin->set_param(INPUTGAIN_ID, (int) (lParam-360)); 
			break;			

		case THRESHOLD1_ID:
			plugin->set_param(THRESHOLD1_ID, (int) (lParam-1000)); 
			break;
		
		case RATIO1_ID:
			plugin->set_param(RATIO1_ID, (int) (lParam+10)); 
			break;

		case RATIO2_ID:
			plugin->set_param(RATIO2_ID, (int) (lParam+10)); 
			break;
		
		case GAINATTACK_ID:
			plugin->set_param(GAINATTACK_ID, (int) lParam); 
			break;

		case GAINRELEASE_ID:
			plugin->set_param(GAINRELEASE_ID, (int) lParam); 
			break;
		
		case LOOKAHEAD_ID:
			plugin->set_param(LOOKAHEAD_ID, (int) lParam); 
			break;
	}
	return 1;
}

int iDynamicaPluginDlg::on_command(int lp,int rp)
{
	int	i;

	switch(lp-kCONTROL_BASE)
	{
	case KTHARD_ID:
		i = c_KTHard.get_check();
		if (i==0) {
			plugin->set_param(KNEETYPE_ID, 0);
		}
		return 1;
		break;
	
	case KTSOFT_ID:
		i = c_KTSoft.get_check();
		if (i==0) {
			plugin->set_param(KNEETYPE_ID, 1);
		}
		return 1;
		break;
	
	case KTSOFTER_ID: 
		i = c_KTSofter.get_check();
		if (i==0) {
			plugin->set_param(KNEETYPE_ID, 2);
		}
		return 1;
		break;

	case MODE1_ID:
		i = c_Mode1.get_check();
		c_Mode1.set_check(!i);
		plugin->set_param(MODE1_ID, !i);
		return 1;
		break;

	case MODE2_ID:
		i = c_Mode2.get_check();
		c_Mode2.set_check(!i);
		plugin->set_param(MODE2_ID, !i);
		return 1;
		break;

	case MODE3_ID:
		i = c_Mode3.get_check();
		c_Mode3.set_check(!i);
		plugin->set_param(MODE3_ID, !i);
		return 1;
		break;

		
	case SIDECHAIN_N_ID:
		i = c_SideChain_N.get_check();
		if (!i) {
			plugin->set_param(SIDECHAIN_ID, 0);
		}
		return 1;
		break;

	case SIDECHAIN_S_ID:
		i = c_SideChain_S.get_check();
		if (!i) {
			plugin->set_param(SIDECHAIN_ID, 1);
		}
		return 1;
		break;

	case RESETIG_ID:
		plugin->set_param(INPUTGAIN_ID, 0);
		return 1;
		break;

	case RESETOG_ID:
		plugin->set_param(OUTPUTGAIN_ID, 0);
		return 1;
		break;

	}
	return CKXPluginGUI::on_command(lp,rp);
}

void iDynamicaPluginDlg::OnTimer(UINT_PTR)	
{
	dword	reg_in_L, reg_in_R, reg_out_L, reg_out_R, reg_level, reg_gain ;
	float	x1, x2;
	int		pos1,pos2;
	char	c[10];

	/*== DSP REGISTERS READ - SET 0 ====================================*/
	// Input
	plugin->get_dsp_register(R_mon_IN_L,&reg_in_L);
	plugin->set_dsp_register(R_mon_IN_L,0x00000000);
	plugin->get_dsp_register(R_mon_IN_R,&reg_in_R);
	plugin->set_dsp_register(R_mon_IN_R,0x00000000);
	// Output
	plugin->get_dsp_register(R_mon_OUT_L,&reg_out_L);
	plugin->set_dsp_register(R_mon_OUT_L,0x00000000);
	plugin->get_dsp_register(R_mon_OUT_R,&reg_out_R);
	plugin->set_dsp_register(R_mon_OUT_R,0x00000000);
	// Level
	plugin->get_dsp_register(R_LEVEL,&reg_level);
	// Gain
	plugin->get_dsp_register(R_GAINDB,&reg_gain);

	
	/*== INPUT VU ======================================================*/
	x1 = (float) (8.68588963800 * log((double) reg_in_L + 1) - 174.597397471);// in fractional format & log scale
	x2 = (float) (8.68588963800 * log((double) reg_in_R + 1) - 174.597397471);
	pos1 = (int) (VU_px_0dB - x1 * VU_sca_pxdB);// VUmeter position
	pos2 = (int) (VU_px_0dB - x2 * VU_sca_pxdB);
	if ((pos1 != pos_in_L) || (pos2 != pos_in_R)) // if position change
	{
		m_in.set_value(pos1, pos2, 0, 0); 
		m_in.redraw();	
		pos_in_L = pos1;
		pos_in_R = pos2;
	}
	
	/*== INPUT CLIPP ===================================================*/
	if (x2 > x1)  x1 = x2;
	if (x1 > Clipp_in) 
	{
		if (x1<0.1) 
		{
			lbl_Clipp_In_Val.SetTextColor(color_txt_clipp);
			if (x1 > -0.1) {
				sprintf(c, " 0.0");
			} else {
				if (x1 < -99.9)
				{
					sprintf(c, "-inf");
				} else {
					sprintf(c, "%03.1f", x1);
				}
			}

		} else {
			lbl_Clipp_In_Val.SetTextColor(color_txt1_clipp);
			sprintf(c, "+%03.1f", x1);
		}
		lbl_Clipp_In_Val.SetWindowText(c);
		Clipp_in = x1;
		
	}

	/*== OUTPUT VU =====================================================*/
	x1 = (float) (8.68588963800 * log((double) reg_out_L + 1) - 174.597397471);// in fractional format & log scale
	x2 = (float) (8.68588963800 * log((double) reg_out_R + 1) - 174.597397471);
	pos1 = (int) (VU_px_0dB - x1 * VU_sca_pxdB);// VUmeter position
	pos2 = (int) (VU_px_0dB - x2 * VU_sca_pxdB);
	if ((pos1 != pos_out_L) || (pos2 != pos_out_R)) // if position change
	{
		m_out.set_value(pos1, pos2, 0, 0); 
		m_out.redraw();	
		pos_out_L = pos1;
		pos_out_R = pos2;
	}
	
	/*== OUTPUT CLIPP ==================================================*/
	if (x2 > x1)  x1 = x2;
	if (x1 > Clipp_out) 
	{
		if (x1<0.1) 
		{
			lbl_Clipp_Out_Val.SetTextColor(color_txt_clipp);
			if (x1 > -0.1) {
				sprintf(c, " 0.0");
			} else {
				if (x1 < -99.9)
				{
					sprintf(c, "-inf");
				} else {
					sprintf(c, "%03.1f", x1);
				}
			}

		} else {
			if (x1>=12.0) {
				lbl_Clipp_Out_Val.SetTextColor(color_txt2_clipp); 
			} else {
				lbl_Clipp_Out_Val.SetTextColor(color_txt1_clipp); 
			}
			sprintf(c, "+%03.1f", x1);
		}
		lbl_Clipp_Out_Val.SetWindowText(c);
		Clipp_out = x1;
	}

	/*== GAIN INDICATOR =====================================================*/
	x2 = (float) ((long) (reg_gain) / 11154303.32); 
	steps_gain = steps_gain + 1;
	if (steps_gain > max_steps_gain-1) 
	{
		if (x2<0.1) 
		{
			if (x2 > -0.1) {
				sprintf(c, " 0.0");
			} else {
				sprintf(c, "%03.1f", x2);
			}
		} else {
			sprintf(c, "+%03.1f", x2);
		}
		lbl_DynamicGain.SetWindowText(c);
		steps_gain = 0;
	}
	
	
	/*== LEVEL-GAIN VU ======================================================*/
	x1 = (float) (8.685889638 * log(plugin->Exp_Dane_Interp((long)(reg_level) / 2147483647.)*4.));
	if (x2 != 0.) x2 = (float) (log(fabs((double) x2) + 1.0) * (double) x2 / fabs((double) x2));
	pos1 = (int) (LEV_px_0dB + x1 * LEV_sca_pxdB );
	pos2 = (int) (GAIN_px_0dB + (x2 * GAIN_px_0dB / log(GAIN_max_dB + 1.) + 1.));
	if ((pos1 != pos_level) || (pos2 != pos_gain)) // if position change
	{
		m_gain_lev.set_value(pos1, pos2, 0, 0);
		m_gain_lev.redraw();
		pos_level = pos1;
		pos_gain = pos2;
	}

}

void iDynamicaPluginDlg::on_destroy()
{ 
 turn_on(0);
}

void iDynamicaPluginDlg::SetGraphPoints()
{
	// Parameters
	double	gain= plugin->_params[OUTPUTGAIN_ID] / 10.0;
	double	th1	= plugin->dB_to_val(plugin->_params[THRESHOLD1_ID]);
	double	th2	= plugin->dB_to_val(plugin->_params[THRESHOLD2_ID]);
	kxparam_t	m1	= plugin->_params[MODE1_ID];
	kxparam_t	m2	= plugin->_params[MODE2_ID];
	kxparam_t	m3	= plugin->_params[MODE3_ID];
	double	ra1	= plugin->ratio_to_val (plugin->_params[RATIO1_ID]); 
	double	ra2	= plugin->ratio_to_val (plugin->_params[RATIO2_ID]);
	double	ra3	= plugin->ratio_to_val (plugin->_params[RATIO3_ID]);
	kxparam_t      kt	= plugin->_params[KNEETYPE_ID];
	if (m1==0) ra1 = 1. / ra1;
	if (m2==0) ra2 = 1. / ra2;
	if (m3==0) ra3 = 1. / ra3;

	graph.SetPoints(gain, th1, th2, ra1, ra2, ra3, (int)kt);
		
}


/* Set the clipps indicators to off */
// =========================================================================
int iDynamicaPluginDlg::destroyClipp(int ndx)
{
	switch (ndx)
	{
		case 0:
			Clipp_in = -160.0;
			break;
		
		case 1:
			Clipp_out = -160.0;
			break;
	}
	return 1;

}

//------------------------------------------
// Reset 'clip peak hold' on mouse click
//--------------------------------------------
void iDynamicaPluginDlg::OnClick_IN()
{
	Clipp_in=-160;
}
void iDynamicaPluginDlg::OnClick_OUT()
{
	Clipp_out=-160;
}
//----------------------------

