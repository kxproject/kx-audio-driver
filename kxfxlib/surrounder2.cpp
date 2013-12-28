// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov and Max Mikhailov, 2001-2008.
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
#include "surrounder2.h"
// effect source
#include "da_surrounder2.cpp"

// plugin parameters
typedef enum _params_id {
	ID_SPEAKER_MODE,
	ID_SUR_MODE,
	ID_CENTER_AMOUNT,
	ID_REAR_AMOUNT,
	ID_REAR_WIDTH,
	ID_REAR_DELAY,
	ID_SUBW_MODE,
	ID_SUB_FREQ
};

// 'virtual' parameters for subwoofer/gui
#define ID_SUB_ONOFF	ID_SUB_FREQ+10
#define ID_SUB_REDIR	ID_SUB_FREQ+11

//.............................................................................

enum sub_modes {
	SBM_DEFAULT=0,
	SBM_NOSUB,
	SBM_REDIRECT,
		
	sub_modes_count
};

enum speaker_modes {
	SPM_21=0,
	SPM_41,
	SPM_51,
	SPM_61, 	// 6th is 'rear center'
	SPM_71, 	// 4&5 are 'side'; 6&7 are 'rear'
	SPM_HEADPHONES_LITE,
	SPM_HEADPHONES_HARD,
	//	SPM_81, 	// 4&5 are 'side'; 6&7 are 'rear'; 8 is 'rear center'
		
	speaker_modes_count,
};

enum sur_modes {
	SUM_OFF=0,
	SUM_COPY,
	SUM_ON,
		
	sur_modes_count,
};

// factory presets
#define P (kxparam_t)

static kxparam_t presets[] = 
{
	P"2.0 - Stereo only",			 SPM_21, SUM_OFF,  100, 100, 50, 0,   SBM_NOSUB,   2000,
	P"2.0 - Simple Mix",			 SPM_21, SUM_COPY, 100, 100, 50, 0,   SBM_NOSUB,   2000,
	P"2.0 - Surround mix",			 SPM_21, SUM_ON,   100, 100, 66, 0,   SBM_NOSUB,   2000,
	P"4.0 - Quadro",				 SPM_41, SUM_OFF,  100, 100, 50, 0,   SBM_NOSUB,   2000,
	P"4.0 - Rear=Front",			 SPM_41, SUM_COPY, 100, 100, 50, 0,   SBM_NOSUB,   2000,
	P"4.0 - Movie Mode",			 SPM_41, SUM_ON,   100, 100, 66, 120, SBM_NOSUB,   2000,
	P"5.0 - Direct Path",			 SPM_51, SUM_OFF,  100, 100, 50, 0,   SBM_NOSUB,   2000,
	P"5.0 - Rear=Front",			 SPM_51, SUM_COPY, 100, 100, 50, 0,   SBM_NOSUB,   2000,
	P"5.0 - Movie Mode",			 SPM_51, SUM_ON,   100, 100, 66, 120, SBM_NOSUB,   2000,
	P"4.1 - Quadro",				 SPM_41, SUM_OFF,  100, 100, 50, 0,   SBM_DEFAULT, 2000,
	P"4.1 - Rear=Front",			 SPM_41, SUM_COPY, 100, 100, 50, 0,   SBM_DEFAULT, 2000,
	P"4.1 - Movie Mode",			 SPM_41, SUM_ON,   100, 100, 66, 120, SBM_DEFAULT, 2000,
	P"5.1 - Direct Path",			 SPM_51, SUM_OFF,  100, 100, 50, 0,   SBM_DEFAULT, 2000,
	P"5.1 - Rear=Front",			 SPM_51, SUM_COPY, 100, 100, 50, 0,   SBM_DEFAULT, 2000,
	P"5.1 - Movie Mode",			 SPM_51, SUM_ON,   100, 100, 66, 120, SBM_DEFAULT, 2000,
	P"Headphones Lite", SPM_HEADPHONES_LITE, SUM_OFF,  100, 100, 50, 0,   SBM_NOSUB,   2000,
	P"Headphones",		SPM_HEADPHONES_HARD, SUM_OFF,  100, 100, 50, 0,   SBM_NOSUB,   2000,
	NULL,
};

static kxparam_t presets_a2[] = 
{
	P"2.0 - Stereo only",			 SPM_21, SUM_OFF,  100, 100, 50, 0,   SBM_NOSUB,   2000,
	P"2.0 - Simple Mix",			 SPM_21, SUM_COPY, 100, 100, 50, 0,   SBM_NOSUB,   2000,
	P"2.0 - Surround mix",			 SPM_21, SUM_ON,   100, 100, 66, 0,   SBM_NOSUB,   2000,
	P"4.0 - Quadro",				 SPM_41, SUM_OFF,  100, 100, 50, 0,   SBM_NOSUB,   2000,
	P"4.0 - Rear=Front",			 SPM_41, SUM_COPY, 100, 100, 50, 0,   SBM_NOSUB,   2000,
	P"4.0 - Movie Mode",			 SPM_41, SUM_ON,   100, 100, 66, 120, SBM_NOSUB,   2000,
	P"5.0 - Direct Path",			 SPM_51, SUM_OFF,  100, 100, 50, 0,   SBM_NOSUB,   2000,
	P"5.0 - Rear=Front",			 SPM_51, SUM_COPY, 100, 100, 50, 0,   SBM_NOSUB,   2000,
	P"5.0 - Movie Mode",			 SPM_51, SUM_ON,   100, 100, 66, 120, SBM_NOSUB,   2000,
	P"4.1 - Quadro",				 SPM_41, SUM_OFF,  100, 100, 50, 0,   SBM_DEFAULT, 2000,
	P"4.1 - Rear=Front",			 SPM_41, SUM_COPY, 100, 100, 50, 0,   SBM_DEFAULT, 2000,
	P"4.1 - Movie Mode",			 SPM_41, SUM_ON,   100, 100, 66, 120, SBM_DEFAULT, 2000,
	P"5.1 - Direct Path",			 SPM_51, SUM_OFF,  100, 100, 50, 0,   SBM_DEFAULT, 2000,
	P"5.1 - Rear=Front",			 SPM_51, SUM_COPY, 100, 100, 50, 0,   SBM_DEFAULT, 2000,
	P"5.1 - Movie Mode",			 SPM_51, SUM_ON,   100, 100, 66, 120, SBM_DEFAULT, 2000,
	P"6.1 - Direct Path",			 SPM_61, SUM_OFF,  100, 100, 50, 0,   SBM_DEFAULT, 2000,
	P"6.1 - Rear=Front",			 SPM_61, SUM_COPY, 100, 100, 50, 0,   SBM_DEFAULT, 2000,
	P"6.1 - Movie Mode",			 SPM_61, SUM_ON,   100, 100, 66, 120, SBM_DEFAULT, 2000,
	P"7.1 - Direct Path",			 SPM_71, SUM_OFF,  100, 100, 50, 0,   SBM_DEFAULT, 2000,
	P"7.1 - Rear=Front",			 SPM_71, SUM_COPY, 100, 100, 50, 0,   SBM_DEFAULT, 2000,
	P"7.1 - Movie Mode",			 SPM_71, SUM_ON,   100, 100, 66, 120, SBM_DEFAULT, 2000,
	P"Headphones Lite", SPM_HEADPHONES_LITE, SUM_OFF,  100, 100, 50, 0,   SBM_NOSUB,   2000,
	P"Headphones",		SPM_HEADPHONES_HARD, SUM_OFF,  100, 100, 50, 0,   SBM_NOSUB,   2000,
	NULL,
};

#undef P

const kxparam_t *iSurrounder2Plugin::get_plugin_presets()
{
	if (is_a2)
		return presets_a2;
	else
		return presets;
}

// parameter descriptions
const kx_fxparam_descr param_descr[SURROUNDER2_PARAMS_COUNT] = 
{
	{"Speaker Mode",	KX_FXPARAM_USER, 0, speaker_modes_count - 1},
	{"Surround Mode",	KX_FXPARAM_USER, 0, sur_modes_count - 1},
	{"VCenter Amount",	KX_FXPARAM_USER, 0, 150},
	{"VRear Amount",	KX_FXPARAM_USER, 0, 100},
	{"VRear Width", 	KX_FXPARAM_USER, 0, 100},
	{"VRear Delay", 	KX_FXPARAM_USER, 0, 200},
	{"VSub Mode",		KX_FXPARAM_USER, 0, sub_modes_count - 1},
	{"VSub Split Freq", KX_FXPARAM_HZ, 100, 3200},
};

int iSurrounder2Plugin::describe_param(int id, kx_fxparam_descr *descr)
{
	memcpy(descr, &param_descr[id], sizeof kx_fxparam_descr); 
	return 0;
}

//.............................................................................

iSurrounder2Plugin::iSurrounder2Plugin()
{
    // ikx is not yet available, cannot get these:
    is_a2=0;
    is_51=0;

    // use preset #2(3); offset=1 due to 'preset_name' item
    memcpy(_params,&presets_a2[(SURROUNDER2_PARAMS_COUNT+1)*2+1], sizeof kxparam_t * SURROUNDER2_PARAMS_COUNT);
}

int iSurrounder2Plugin::init()
{
	// check for a2/a2zs:
	is_a2=0;
	ikx->get_dword(KX_DWORD_IS_A2,(dword *)&is_a2); 
    is_51=0;
    ikx->get_dword(KX_DWORD_IS_51,(dword *)&is_51); 

    if(is_a2)
	 memcpy(_params,&presets_a2[(SURROUNDER2_PARAMS_COUNT+1)*18+1], sizeof kxparam_t * SURROUNDER2_PARAMS_COUNT);  // 7.1 direct path
    else
    {
     if(is_51)
       memcpy(_params,&presets[(SURROUNDER2_PARAMS_COUNT+1)*12+1], sizeof kxparam_t * SURROUNDER2_PARAMS_COUNT); // 5.1 direct path
      else
       memcpy(_params,&presets[(SURROUNDER2_PARAMS_COUNT+1)*3+1], sizeof kxparam_t * SURROUNDER2_PARAMS_COUNT);  // QUADRO
    }
	
	return iKXPlugin::init();
}

int iSurrounder2Plugin::get_param(int id, kxparam_t *value)
{
	*value = _params[id]; 
	return 0;
}

int iSurrounder2Plugin::set_all_params(kxparam_t* values)
{
	for (int i = 0; i < SURROUNDER2_PARAMS_COUNT; i++) 
		set_param(i, values[i]);
	
	return 0;
}

int iSurrounder2Plugin::request_microcode()
{
	publish_microcode(surrounder2);
	
	if(!is_a2)
	{	
		code_size-=4 * sizeof(dsp_code);
		info_size-=2 * sizeof(dsp_register_info);
	}
	
	return 0;
}

int iSurrounder2Plugin::set_defaults() 
{
    if(is_a2)
	 set_all_params(&presets_a2[(SURROUNDER2_PARAMS_COUNT+1)*18+1]);  // 7.1 direct path
    else
    {
     if(is_51)
       set_all_params(&presets[(SURROUNDER2_PARAMS_COUNT+1)*12+1]); // 5.1 direct path
      else
       set_all_params(&presets[(SURROUNDER2_PARAMS_COUNT+1)*3+1]);  // QUADRO
    }
	return 0;
}

#include "surrounder2_dsp_tags.h"

int iSurrounder2Plugin::set_param(int id, kxparam_t value)
{
	if(id==ID_SUB_REDIR)
	{
		id=ID_SUBW_MODE;
		if(value)
			value=SBM_REDIRECT;
		else 
			value=SBM_DEFAULT;
	}
	
	if(id==ID_SUB_ONOFF)
	{
		id=ID_SUBW_MODE; 
		if(value) 
			value=SBM_DEFAULT; 
		else 
			value=SBM_NOSUB;
	}
	
	_params[id] = value;
	
	double W, T, A, Y, B; 
	int i;
	long k, m;
	word y;
	
	switch(id)
	{
		case ID_SPEAKER_MODE:
		{
			// mute here
			// ikx->mute();
			
			if(!is_a2)
			{
				if(value==SPM_61 || value==SPM_71)
					value += 2;
			}

			
			// FIXME
			if(value==SPM_HEADPHONES_HARD || value==SPM_HEADPHONES_LITE)
			{
				MessageBox(NULL,"Headphones presets are not currently supported","kX Mixer",MB_OK|MB_ICONSTOP); 			
				value=SPM_21;
				_params[ID_SPEAKER_MODE]	 = SPM_21;
				_params[ID_SUR_MODE]		 = SUM_OFF;
				_params[ID_CENTER_AMOUNT]	 = 100;
				_params[ID_REAR_AMOUNT] 	 = 100;
				_params[ID_REAR_WIDTH]		 = 50;
				_params[ID_REAR_DELAY]		 = 0;
				_params[ID_SUBW_MODE]		 = SBM_NOSUB;
				_params[ID_SUB_FREQ]		 = 2000;				
			}
			
			#define _write_code(offset, ops) write_instruction(offset, ops[0], ops[1], ops[2], ops[3], ops[4])
			for (i = 0; i < (is_a2?SURROUNDER2_CODE_SIZE:(SURROUNDER2_CODE_SIZE-4)); i++) {_write_code(i, sc_code_sp_mode[value][i]);}
			// well, it's not good that outputs are unmuted before "reloading" is complete, clicks, clicks
			
			set_param(ID_SUR_MODE, _params[ID_SUR_MODE]);
			// and unmute here (if not already)
			// ikx->unmute();
			
			// set speakers
			dword old=0;
			if(ikx->get_hw_parameter(KX_HW_8PS,&old)==0)
			{
				// update speaker configuration
				if(value==SPM_61 || value==SPM_71 || value==SPM_HEADPHONES_LITE || value==SPM_HEADPHONES_HARD)
					ikx->set_hw_parameter(KX_HW_8PS,old|KX_HW_8PS_ON);
				else
					ikx->set_hw_parameter(KX_HW_8PS,old&(~KX_HW_8PS_ON));
			}
		}
		break;
		
		case ID_SUR_MODE:
			if(_params[ID_SPEAKER_MODE]==SPM_HEADPHONES_HARD ||
				_params[ID_SPEAKER_MODE]==SPM_HEADPHONES_LITE)
				break;
		
			set_param(ID_CENTER_AMOUNT, _params[ID_CENTER_AMOUNT]);
			set_param(ID_REAR_AMOUNT, _params[ID_REAR_AMOUNT]);
			// updated by above: set_param(ID_REAR_WIDTH, params[ID_REAR_WIDTH]);
			set_param(ID_REAR_DELAY, _params[ID_REAR_DELAY]);
			set_param(ID_SUBW_MODE, _params[ID_SUBW_MODE]);
			break;
		
		case ID_CENTER_AMOUNT:
			if(_params[ID_SPEAKER_MODE]==SPM_HEADPHONES_HARD ||
				_params[ID_SPEAKER_MODE]==SPM_HEADPHONES_LITE)
				break;
		
			switch(_params[ID_SUR_MODE])
			{
				case SUM_OFF:	A = 0.; break;
				case SUM_COPY:	A = .5; break;
				case SUM_ON:	A = _params[ID_CENTER_AMOUNT] * .005; break;
				default:		return 0;
			}
			
			set_dsp_register(R_CK, _dbl_to_dspword(A));
			break;
		
		case ID_REAR_AMOUNT:
		case ID_REAR_WIDTH:
			if(_params[ID_SPEAKER_MODE]==SPM_HEADPHONES_HARD ||
				_params[ID_SPEAKER_MODE]==SPM_HEADPHONES_LITE)
				break;
				
			switch(_params[ID_SUR_MODE])
			{
				case SUM_OFF:	A = 0.; B = 0.; break;
				case SUM_COPY:	A = 1.; B = 0.; break;
				case SUM_ON:	
					A = (_params[ID_REAR_AMOUNT] * .01);
					B = A * (_params[ID_REAR_WIDTH] * .01);
					break;
				default:		return 0;
			}
			
			set_dsp_register(R_SK1, _dbl_to_dspword(A));
			set_dsp_register(R_SK2, _dbl_to_dspword(B));
			break;
			
		case ID_REAR_DELAY:
			if(_params[ID_SPEAKER_MODE]==SPM_HEADPHONES_HARD ||
				_params[ID_SPEAKER_MODE]==SPM_HEADPHONES_LITE)
				break;
				
			switch(_params[ID_SUR_MODE])
			{
				case SUM_OFF:
				case SUM_COPY:	m = 2; // well, could be better
				case SUM_ON:	m = 2 + (long) (.0001 * FS * _params[ID_REAR_DELAY]); break;
				default:		return 0;
			}
				
			get_tram_addr(R_DWL, (dword*) &k);
			set_tram_addr(R_DRL, k + m);
			get_tram_addr(R_DWR, (dword*) &k);
			set_tram_addr(R_DRR, k + m);
			break;		
					
		case ID_SUB_FREQ:
			if(_params[ID_SPEAKER_MODE]==SPM_HEADPHONES_HARD ||
				_params[ID_SPEAKER_MODE]==SPM_HEADPHONES_LITE)
				break;
					
			#define LP_SLOPE sqrt(2.)
			W = 2. * pi * ((value * .1) / FS);
			T = (LP_SLOPE / 2.) * sin(W);
			B = (1. - T) / (1. + T);
			Y = (1 + B) * cos(W);
			A = 1 + B - Y;
			B = - B;
			Y = Y - 1;
					
			set_dsp_register(R_WFA, _dbl_to_dspword(A));
			set_dsp_register(R_WFY, _dbl_to_dspword(Y));
			set_dsp_register(R_WFB, _dbl_to_dspword(B));
			break;	
			
		case ID_SUBW_MODE:
			if(_params[ID_SPEAKER_MODE]==SPM_HEADPHONES_HARD ||
				_params[ID_SPEAKER_MODE]==SPM_HEADPHONES_LITE)
				break;
				
			if(value<0) value=0;
			if(value>=sub_modes_count) value=sub_modes_count-1;
					
			y = (word) ((_params[ID_SUR_MODE] == SUM_OFF) ? 0x2040u : 0x204du);
			surr2_sub_mode_code[SBM_NOSUB][0][2] = y;
			surr2_sub_mode_code[SBM_NOSUB][1][2] = y;
			write_instruction(0, 0, 0, 0, surr2_sub_mode_code[value][0][1], surr2_sub_mode_code[value][0][2],VALID_X|VALID_Y);	
			write_instruction(1, 0, 0, 0, surr2_sub_mode_code[value][1][1], surr2_sub_mode_code[value][1][2],VALID_X|VALID_Y);
			write_instruction(2, 0, 0, surr2_sub_mode_code[value][2][0], surr2_sub_mode_code[value][2][1], surr2_sub_mode_code[value][2][2],VALID_X|VALID_Y|VALID_A);
			break;
					
		default:
			return 0;
	}
		
	// send parameter change notification to the host
	if (cp) ((iSurrounder2PluginDlg*) cp)->sync(id);
		
	return 0;
}

//.............................................................................

iKXPluginGUI *iSurrounder2Plugin::create_cp(kDialog *parent, kFile *mf)
{
	iSurrounder2PluginDlg *tmp;
	tmp = new iSurrounder2PluginDlg(this, parent, mf);
	tmp->create();
	return tmp;
}

int iSurrounder2Plugin::get_user_interface()
{
	return IKXPLUGIN_OWNER;
}


//.............................................................................
// Plugin View Implementation
//.............................................................................

void iSurrounder2PluginDlg::init()
{
	CKXPluginGUI::init();
	// custom dialog initialization here...
	
	// MAKEUP_DLG(Caption, Width, Height)
	create_dialog(plugin_name(surrounder2), 300, 370);
	
	// create controls
	int i;
	
	const char* const speaker_modes_names[speaker_modes_count] = {"2.1", "4.1", "5.1", "6.1", "7.1", "HPh Lt","HPh" };
	
	create_label(lblFoo[4],SURROUNDER2_PARAMS_COUNT + 4,"Speaker Mode",20,15,100);
	// control,id,name,x,y,wd,n_items,dropped_width
	create_combo(cmbSpkMode,ID_SPEAKER_MODE,"Speaker Mode",20+190,15,70,3,70);
	
	for (i = 0; i < speaker_modes_count; i++)
	{
		if((i!=3 && i!=4) || (plugin->is_a2))
			cmbSpkMode.add_string(speaker_modes_names[i]);
	}
	
	// (kStatic &label,UINT ctrl_id,const char *caption,int left,int top,int width)
	
	create_label(lblFoo[5],SURROUNDER2_PARAMS_COUNT + 5,"Surround",20,15+30,100);
	
	const char* const sur_modes_names[sur_modes_count] = {"Off", "Copy", "On"};
	
	create_combo(cmbSurMode,ID_SUR_MODE,"Surround Mode",20+190,15+30,70,3,70);
	
	for (i = 0; i < sur_modes_count; i++) 
		cmbSurMode.add_string(sur_modes_names[i]);
	
	// labels:
	// (kStatic &label,UINT ctrl_id,const char *caption,int left,int top,int width)
	// lblFoo[0].Create("", WS_VISIBLE | ULS_BOX, size.right - 162 - 20, 64 - 24, 88, 19, this, SURROUNDER2_PARAMS_COUNT + 0);
	create_label(lblFoo[0],SURROUNDER2_PARAMS_COUNT + 0,"",20,15+60,100);
	// slider, id, min, max, x, y, wd, ht
	create_hslider(knbSurAmount[0], ID_CENTER_AMOUNT, "", 0, 150, 20+120, 15+60, 200, 30);
	
	// lblFoo[1].Create("", WS_VISIBLE | ULS_BOX, size.right - 78 - 30, 36 + 48, 92, 19, this, SURROUNDER2_PARAMS_COUNT + 1);
	create_label(lblFoo[1],SURROUNDER2_PARAMS_COUNT + 1,"",20, 15+100, 100);
	create_hslider(knbSurAmount[1], ID_REAR_AMOUNT, "", 0, 100, 20+120, 15+100, 200, 30);
	
	// lblFoo[2].Create("", WS_VISIBLE | ULS_BOX, size.right - 162 - 20, 144 - 24, 88, 19, this, SURROUNDER2_PARAMS_COUNT + 2);
	create_label(lblFoo[2],SURROUNDER2_PARAMS_COUNT + 2,"",20, 15+140, 100);
	create_hslider(knbSurAmount[2], ID_REAR_WIDTH, "", 0, 100, 20+120, 15+140, 200, 30);
	
	// lblFoo[3].Create("", WS_VISIBLE | ULS_BOX, size.right - 78 - 30, 116 + 48, 92, 19, this, SURROUNDER2_PARAMS_COUNT + 3);
	create_label(lblFoo[3],SURROUNDER2_PARAMS_COUNT + 3,"",20, 15+180, 100);
	create_hslider(knbRearD, ID_REAR_DELAY, "", 0, 200, 20+120, 15+180, 200, 30);
	
	create_label(lblFoo[8],SURROUNDER2_PARAMS_COUNT + 8,"Use Subwoofer output",20,15+220,200);
	create_checkbox(chkUseSub,ID_SUB_ONOFF,"",20+210,15+220+5,20);
	
	// (kCheckButton &checkbox,UINT ctrl_id,const char *caption,int left,int top,int width)
	//lblFoo[6].Create("Bass Redirection", WS_VISIBLE | UBS_FATSMALLFONT, 16 + 16, size.bottom - 80 - 50, 96, 19, this, SURROUNDER2_PARAMS_COUNT + 6);
	
	create_checkbox(chkSubMove,ID_SUB_REDIR,"",20+210,15+260+5,20);
	create_label(lblFoo[6],SURROUNDER2_PARAMS_COUNT + 6,"Bass Redirection",20,15+260,100);
	
	//lblFoo[7].Create("", WS_VISIBLE | ULS_BOX, 62, size.bottom - 80 - 28 - 1, 96, 19, this, SURROUNDER2_PARAMS_COUNT + 7);
	create_label(lblFoo[7],SURROUNDER2_PARAMS_COUNT + 7,"",20,15+300,100);
	// (kFader &fader,UINT ctrl_id,const char *label,int minval,int maxval,int left,int top,int width,int height)
	create_hslider(fdrSubFreq,ID_SUB_FREQ,"Frequency",100,3200,20+120,15+300,200,30);
	
	controls_enabled = TRUE;
	
	for (i = 0; i < SURROUNDER2_PARAMS_COUNT; i++)
		sync(i);
}

const char* const sur_knob_names[] = {"VCenterA", "VRearA", "VRearW", "VRearD"};

void iSurrounder2PluginDlg::sync(int id)
{
	kxparam_t value=0;
	
	if((id==ID_SUB_ONOFF)||(id==ID_SUB_REDIR))
		id=ID_SUBW_MODE;
	
	if(id!=EVERYPM_ID)
		value=plugin->_params[id];
	
	int i;
	
	switch(id)
	{
		case ID_SPEAKER_MODE:
			if (controls_enabled)
			{
				cmbSpkMode.set_selection((int)value);
				if(value==SPM_HEADPHONES_HARD || value==SPM_HEADPHONES_LITE)
				{
					cmbSurMode.hide();
					lblFoo[5].hide();
				}
				else
				{
					cmbSurMode.show();
					lblFoo[5].show();
				}
			}
			break;
			
		case ID_SUR_MODE:
			if (controls_enabled)
				cmbSurMode.set_selection((int)value);
			
			value = (value == SUM_ON) ? 1 : 0;
			for(i = 0; i < 3; i++)
			{
				if(value)
				{
					knbSurAmount[i].show();
					lblFoo[i].show();
				}	
				else
				{
					knbSurAmount[i].hide();
					lblFoo[i].hide();
				}
			}
			if(cmbSpkMode.get_selection() == SPM_21) 
				value = 0;
			
			if(value)
			{
				knbRearD.show();
				lblFoo[3].show();
			}
			else
			{
				knbRearD.hide();
				lblFoo[3].hide();
			}
			break;
			
		case ID_CENTER_AMOUNT:
		case ID_REAR_AMOUNT:
		case ID_REAR_WIDTH:
		{
			i = id - ID_CENTER_AMOUNT;
			if (controls_enabled)
				knbSurAmount[i].set_pos((int)value);
				
			char tmp[127];
			sprintf(tmp,"%s: %.2f", sur_knob_names[i], value * .01);
			lblFoo[i].SetWindowText(tmp);
		}
		break;
			
		case ID_REAR_DELAY:
		{
			if (controls_enabled)
				knbRearD.set_pos((int)value);
				
			char tmp[127];
			sprintf(tmp,"%s: %.1fms", sur_knob_names[3], value * .1);
			lblFoo[3].SetWindowText(tmp);
		}
		break;
			
		case ID_SUBW_MODE:
		{
			//			if (controls_enabled)
			switch(value)
			{
				case SBM_NOSUB:
					chkUseSub.set_check(0);
					fdrSubFreq.hide();
					lblFoo[7].hide();
					chkSubMove.hide();
					lblFoo[6].hide();
					break;
				case SBM_DEFAULT:
					chkUseSub.set_check(1);
					chkSubMove.show();
					chkSubMove.set_check(0);
					lblFoo[6].show();
					fdrSubFreq.hide();
					lblFoo[7].hide();
					break;
				case SBM_REDIRECT:
					chkUseSub.set_check(1);
					chkSubMove.set_check(1);
					chkSubMove.show();
					fdrSubFreq.show();
					lblFoo[7].show();
					lblFoo[6].show();
					break;
			}
		}
		break;
			
		case ID_SUB_FREQ:
		{
			if (controls_enabled)
				fdrSubFreq.set_pos((int)value);
				
			char tmp[127];
			sprintf(tmp,"Split Freq: %.fHz", value * .1);
			lblFoo[7].SetWindowText(tmp);
		}
		break;
	}
}
