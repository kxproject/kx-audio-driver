// ///////////////////////////////////////////////////////////////////////////////
// Copyright (c) LeMury, 2005.
// All rights reserved
//
// 	adc v2.1 (skinned) AC97/UDA plugin for 10k1 and 10k2.!!
//
// This plugins takes care of all Analog inputs.
// On 10k2 (Audigy 1,2 etc) it enables use of second ADC,Phillips UDA.
// Features;
// - enables use of UDA codec on 10k2
// - separate selectable Rec sources for ac97 device
//
// Remarks;
// - ac97's stereo mixer it is not capable to mute only one (left or right) channel.
// One can only attenuate or mute both at once.
// Therefore I don't see the use of 'ganged' sliders in ac97 mixer part.
//////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
// --------------- the above should be a 'standard' beginning of any .cpp file
#include "adc.h"
#include "da_adc_10k2.cpp"
#include "da_adc_10k1.cpp"



typedef enum _params_id{
	ON_GANG_ID,		//0
	SELL_ID,		//1
	SELR_ID,		//2
	GAINRECL_ID,	//3	RECGain is 0 - 15 = 0x0 - 0xF = 0dB - +22dB.
	GAINRECR_ID,	//4	RECGain is 0 - 15 = 0x0 - 0xF = 0dB - +22dB.
	GAINLINE_ID,	//5 Gains are 31 - 0 = 0x1F - 0x00 = -34.5dB - +12dB. 0x08 = 0dB
	GAINMIC_ID,		//6
	GAINCD_ID,		//7
	GAINAUX_ID,		//8
	ON_REC_ID,		//9 Checked means '1' = ON
	ON_LINE_ID,		//10
	ON_MIC_ID,		//11
	ON_CD_ID,		//12
	ON_AUX_ID,		//13
	MICBOOST_ID,	//14
	SETUDA_ID,		//16

	DSP_ID,			//17 open dsp window
	FXMENU_ID,		//18 fx pop menu

};

// default plugin parameters
// -------------------------
static int default_params[ADC_PARAMS_COUNT] = { 1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0};

// presets
// -------
#define P (kxparam_t)
static kxparam_t presets_10k1[]=
{
P "AC97 Line",								1,4,4,0,0,0,0,0,0,1,0,0,0,0,0,0,
P "AC97 CD",								1,1,1,0,0,0,0,0,0,1,0,0,0,0,0,0,
P "AC97 Aux",								1,3,3,0,0,0,0,0,0,1,0,0,0,0,0,0,
P "AC97 Mic",								1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,
P "AC97 Left Line / Right Mic",				0,4,0,0,0,0,0,0,0,1,0,0,0,0,0,0,
NULL
};

static kxparam_t presets_10k2[]=
{
P "AC97 Line",								1,4,4,0,0,0,0,0,0,1,0,0,0,0,0,0,
P "AC97 CD",								1,1,1,0,0,0,0,0,0,1,0,0,0,0,0,0,
P "AC97 Aux",								1,3,3,0,0,0,0,0,0,1,0,0,0,0,0,0,
P "AC97 Mic",								1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,
P "AC97 CD + UDA Line",						1,1,1,0,0,0,0,0,0,1,1,0,0,0,0,1,
P "AC97 Left Mic / Right Aux  + UDA Line",	0,0,3,0,0,0,0,0,0,1,1,0,0,0,0,1,
P "AC97 Left Line / Right Mic + UDA Aux",	0,4,0,0,0,0,0,0,1,1,0,0,0,1,0,1,
NULL
};
#undef P


// for kX Automation: param description (see kxplugin.h for details on kx_fxparam_descr)
// -------------------------------------------------------------------------------------
static kx_fxparam_descr _param_descr_10k2[ADC_PARAMS_COUNT] = {
	{"GangRecSource",	KX_FXPARAM_SWITCH,	 0, 1 },
	{"LeftRecSource",	KX_FXPARAM_SELECTION, 0, 7 },
	{"RightRecSource",	KX_FXPARAM_SELECTION, 0, 7 },
	{"RecGainL",		KX_FXPARAM_USER,	 0, 15 },
	{"RecGainR",		KX_FXPARAM_USER,	 0, 15 },
	{"LineGain",	   KX_FXPARAM_USER,		 0, 31 },
	{"MicGain",		   KX_FXPARAM_USER,		 0, 31 },
	{"CDGain",	       KX_FXPARAM_USER,		 0, 31 },
	{"AuxGain",		   KX_FXPARAM_USER,		 0, 31 },
	{"RecEnable",      KX_FXPARAM_SWITCH,	 0, 1 },
	{"LineEnable",	   KX_FXPARAM_SWITCH,	 0, 1 },
	{"MicEnable",	   KX_FXPARAM_SWITCH,	 0, 1 },
	{"CDEnable",	   KX_FXPARAM_SWITCH,	 0, 1 },
	{"AuxEnable",	   KX_FXPARAM_SWITCH,	 0, 1 },
	{"MicBoost",	   KX_FXPARAM_SWITCH,	 0, 1 },
	{"UDAEnable",	   KX_FXPARAM_SWITCH,	 0, 1 }
	};
static kx_fxparam_descr _param_descr_10k1[ADC_PARAMS_COUNT] = {
	{"GangRecSource",  KX_FXPARAM_SWITCH,	 0, 1 },
	{"LeftRecSource", KX_FXPARAM_SELECTION, 0, 7 },
	{"RightRecSource",KX_FXPARAM_SELECTION, 0, 7 },
	{"RecGainL",		KX_FXPARAM_USER,	 0, 15 },
	{"RecGainR",		KX_FXPARAM_USER,	 0, 15 },
	{"LineGain",	   KX_FXPARAM_USER,		 0, 31 },
	{"MicGain",		   KX_FXPARAM_USER,		 0, 31 },
	{"CDGain",	       KX_FXPARAM_USER,		 0, 31 },
	{"AuxGain",		   KX_FXPARAM_USER,		 0, 31 },
	{"RecEnable",      KX_FXPARAM_SWITCH,	 0, 1 },
	{"LineEnable",	   KX_FXPARAM_SWITCH,	 0, 1 },
	{"MicEnable",	   KX_FXPARAM_SWITCH,	 0, 1 },
	{"CDEnable",	   KX_FXPARAM_SWITCH,	 0, 1 },
	{"AuxEnable",	   KX_FXPARAM_SWITCH,	 0, 1 },
	{"MicBoost",	   KX_FXPARAM_SWITCH,	 0, 1 }
	};

//---------------------------------------------------------------------------------------
int iAdcPlugin::set_defaults() 
{
	set_all_params(default_params); 
	return 0;
}
//-----------------------------------------------------------------------
const kxparam_t *iAdcPlugin::get_plugin_presets()
{
	if(is_10k1)						// for 10k1 cards
		return presets_10k1;	
	else							// for 10k2 cards
		return presets_10k2;
}
//-----------------------------------------------------------------------
int iAdcPlugin::describe_param(int ndx, kx_fxparam_descr *descr)
{
	if(is_10k1)
	{
		memcpy(descr, &_param_descr_10k1[ndx], sizeof kx_fxparam_descr); return 0;
	}
	else
	{
		memcpy(descr, &_param_descr_10k2[ndx], sizeof kx_fxparam_descr); return 0;
	}
}
//--------------------------------------------------------------------------
int iAdcPlugin::get_param(int ndx, int *value) // ndx corresponds to 'logical' parameter id
{
	*value = _params[ndx]; 
	return 0;
}

// set_param sets 'logical' values by writing to 'physical' registers
//------------------------------------------------------------------------------------------
int iAdcPlugin::set_param(int ndx, int value)
{
	_params[ndx] = value;

	word l,r,set;

	switch (ndx) 
	{
	case SELL_ID:	// Left AC97 Rec source selection
		if(_params[ON_GANG_ID]) 	_params[SELR_ID] = value;
		l = (word) _params[SELL_ID];
		r = (word) _params[SELR_ID];
		set = (l<<8) + r;
		ikx->ac97_write(AC97_REG_REC_SELECT, set);
		dbg("set= %x", set);
		break;

	case SELR_ID:	// Right AC97 Rec source selection
		if(_params[ON_GANG_ID]) 	_params[SELL_ID] = value;
		l = (word) _params[SELL_ID];
		r = (word) _params[SELR_ID];
		set = (l<<8) + r;
		ikx->ac97_write(AC97_REG_REC_SELECT, set);
		dbg("set= %x", set);
		break;

	case GAINRECL_ID:
		if(_params[ON_GANG_ID]) 	_params[GAINRECR_ID] = value;
		l = _params[ON_REC_ID] == 0?1:0;	// invert add mute value
		l = l<<15;
		l = l + (value<<8);					// add left rec gain value
		l = l +	_params[GAINRECR_ID];		// add right rec gain value
		ikx->ac97_write(AC97_REG_REC_GAIN, l);
		break;
	case GAINRECR_ID:
		if(_params[ON_GANG_ID]) 	_params[GAINRECL_ID] = value;
		l = _params[ON_REC_ID] == 0?1:0;	// invert add mute value
		l = l<<15;
		l = l + (_params[GAINRECL_ID]<<8);	// add left rec gain value
		l = l + value;						// add right rec gain value
		ikx->ac97_write(AC97_REG_REC_GAIN, l);
		break;
	case ON_REC_ID:
		l = value==0?1:0;					// invert  mute value
		l = l<<15;
		l = l + (_params[GAINRECL_ID]<<8);	// add left rec gain value
		l = l + _params[GAINRECR_ID];		// add right rec gain value
		ikx->ac97_write(AC97_REG_REC_GAIN, l);
		break;


	case GAINLINE_ID:
		//-34.5dB - +12dB - 31 step 0x08 = 0dB (-23, 8,)
		value = 31 - (value+23); // convert value to 31-0
		l = _params[ON_LINE_ID] == 0?1:0;	// invert add mute value
		l = l<<15;
		l = l + (value<<8);					// add left line gain value
		set = l+ value;						// add right line gain value
		ikx->ac97_write(AC97_REG_LINE_VOL, set);
		break;
	case ON_LINE_ID:
		l = value==0?1:0;					// invert  mute value
		l = l<<15;
		l = l+ ((31 -(_params[GAINLINE_ID] + 23))<<8);	// add left line gain value
		set = l + (31 -(_params[GAINLINE_ID] + 23));	// add right line gain value
		ikx->ac97_write(AC97_REG_LINE_VOL, set);
		break;

	case GAINCD_ID:
		//-34.5dB - +12dB - 31 step 0x08 = 0dB (-23, 8,)
		value = 31 - (value+23); // convert value to 31-0
		l = _params[ON_CD_ID] == 0?1:0;	// invert add mute value
		l = l<<15;
		l = l + (value<<8);					// add left cd gain value
		set = l+ value;						// add right cd gain value
		ikx->ac97_write(AC97_REG_CD_VOL, set);
		break;
	case ON_CD_ID:
		l = value==0?1:0;					// invert  mute value
		l = l<<15;
		l = l+ ((31 -(_params[GAINCD_ID] + 23))<<8);	// add left cd gain value
		set = l + (31 -(_params[GAINCD_ID] + 23));		// add right cd gain value
		ikx->ac97_write(AC97_REG_CD_VOL, set);
		break;


	case GAINAUX_ID:
		//-34.5dB - +12dB - 31 step 0x08 = 0dB (-23, 8,)
		value = 31 - (value+23); // convert value to 31-0
		l = _params[ON_AUX_ID] == 0?1:0;	// invert add mute value
		l = l<<15;
		l = l + (value<<8);					// add left value
		set = l+ value;						// add right value
		ikx->ac97_write(AC97_REG_AUX_VOL, set);
		break;
	case ON_AUX_ID:
		l = value==0?1:0;					// invert  mute value
		l = l<<15;
		l = l+ ((31 -(_params[GAINAUX_ID] + 23))<<8);	// add left gain value
		set = l + (31 -(_params[GAINAUX_ID] + 23));		// add right gain value
		ikx->ac97_write(AC97_REG_AUX_VOL, set);
		break;

	case GAINMIC_ID:
		value = 31 - (value+23);			// convert value to 31-0
		l = _params[ON_MIC_ID] == 0?1:0;	// invert add mute value
		l = l<<15;
		l = l + value;						// add Mic gain value
		set = l+ (_params[MICBOOST_ID]<<6);	// add Mic Boost value
		ikx->ac97_write(AC97_REG_MIC_VOL, set);
		break;
	case ON_MIC_ID:
		l = value==0?1:0;					// invert  mute value
		l = l<<15;
		l = l+ (31 -(_params[GAINMIC_ID] + 23));	// add Mic gain value
		set = l + (_params[MICBOOST_ID]<<6);		// add Mic Boost value
		ikx->ac97_write(AC97_REG_MIC_VOL, set);
		break;
	case MICBOOST_ID:
		l = _params[ON_MIC_ID] == 0?1:0;	// invert add Mic mute value
		l = l<<15;
		l = l+ (31 -(_params[GAINMIC_ID] + 23));	// add Mic gain value
		set = l + (value<<6);						// add Mic Boost value
		ikx->ac97_write(AC97_REG_MIC_VOL, set);
		break;

	case SETUDA_ID:		// Enable UDA (AC97.2) only for adg 1/2
		if(is_10k1)	return 0;
		value = _params[SETUDA_ID];
		if(value)
		{
			if(!is_A2 && !is_A2ex)
				ikx->set_hw_parameter(19, 1);			// Only for Audigy 1
			ikx->ac97_write(AC97_REG_MASTER_VOL, 0);	// set master to 'On' = 0dB
		}
		else
		{
			if(!is_A2 && !is_A2ex)
				ikx->set_hw_parameter(19, 0);			// Only for Audigy 1
			ikx->ac97_write(AC97_REG_MASTER_VOL,0x8000);// set master to mute (default)
		}
		break;

	default:	
		return 0;
	}

	// we need do synchronize the GUI part,
	if (cp) ((iAdcPluginDlg*) cp)->sync(ndx);
	return 0;
}

//-----------------------------------------------------
int iAdcPlugin::set_all_params(int* values)
{
	for (int i = 0; i < ADC_PARAMS_COUNT; i++) set_param(i, values[i]);
	return 0;
}

//---------------------------------------------------------
int iAdcPlugin::init()
{

#define AC97REGMUTE		0x9F1F	// Mutes 5 bit Gain registers

	is_10k2	=0;
	has_ac97=0;
	is_A2	=0;
	is_A2ex	=0;
	is_10k1 =0;

	ikx->get_dword(KX_DWORD_IS_10K2,		&is_10k2);
	ikx->get_dword(KX_DWORD_IS_A2,			&is_A2);	// audigy 2
	ikx->get_dword(KX_DWORD_IS_A2EX,		&is_A2ex);	// audigy 2 plat ex
	ikx->get_dword(KX_DWORD_AC97_PRESENT,	&has_ac97);

	if(!is_10k2 && has_ac97) is_10k1 = 1;	// set flag

	if(has_ac97)
	{
		//ikx->ac97_reset();				// Is same a next?
		ikx->ac97_write(AC97_REG_RESET,	0); // Is this a good idea?

		// Turn off some rarely used stuff ..???
		ikx->ac97_write(AC97_REG_VIDEO_VOL,	AC97REGMUTE);
		ikx->ac97_write(AC97_REG_PCM_VOL,			0x8808);
		ikx->ac97_write(AC97_REG_SURROUND_DAC_VOL,	0x8808);		
	}


	return iKXPlugin::init();
}

//------------------------------------------------------------
int iAdcPlugin::request_microcode() 
{
	if(is_10k2 && has_ac97)
	{
		publish_microcode(adc_10k2);
		return 0;
	}
	else if(is_10k1)
	{
		publish_microcode(adc_10k1);
		return 0;
	}
	else
	{
		//  Error message box
		MessageBox(0,"ADC Plugin: No AC97 device or no compatible card detected!\n", "kX Mixer",MB_ICONHAND );
		return 1;
	}

	return 0;
}
//-------------------------------------------------------------
const char *iAdcPlugin::get_plugin_description(int id)
{
	if(!is_10k1) { plugin_description(adc_10k2); }
	else		 { plugin_description(adc_10k1); }
}


iKXPluginGUI *iAdcPlugin::create_cp(kDialog *parent, kFile *mf)
{
	iAdcPluginDlg *tmp;
	tmp = new iAdcPluginDlg(this, parent,mf);
	tmp->create();
	return tmp;
}

int iAdcPlugin::get_user_interface()
{
	return IKXPLUGIN_OWNER; // means 'custom' GUI panel
}
///////////////////////////////////////////////////////////////////////////
// Plugin dialog contructor
iAdcPluginDlg::iAdcPluginDlg(iAdcPlugin *plg, kDialog *parent_,kFile *mf_) 
 : CKXPluginGUI(parent_,mf_) 
{
	//dbg("gui construct");
	plugin=plg;
	ikm=0;
	ikm = new 	iKXManager;	

}
//---------------------------------------------------------
// Dialog destructor
iAdcPluginDlg::~iAdcPluginDlg()
{
	dbg("gui destruct");

	DeleteObject(hsliderback_140);
	DeleteObject(hsliderthumb);
	DeleteObject(henaon);
	DeleteObject(henaoff);
	DeleteObject(hgangon);
	DeleteObject(hgangoff);

	DeleteObject(hudaon);
	DeleteObject(hudaoff);
	DeleteObject(hbooston);
	DeleteObject(hboostoff);
	
	DeleteObject(harrow);
	DeleteObject(harrow_o);
	DeleteObject(htxtbox);

	DeleteObject(hdsp);
	DeleteObject(hdsp_o);
	DeleteObject(hkfx);
	DeleteObject(hkfx_o);

	if(ikm)	{delete ikm; ikm=0;}

}

//////////////////////////////////////////////////////////////////////////////////////////////
/* Plugin UI Implementation */
//--------------------------------------------------------------------------------------------
void iAdcPluginDlg::init()
{
	CKXPluginGUI::init(); // this is necessary
	HMODULE hmodule = GetModuleHandle("ADC.KXL"); 	// Get handle to this module for bitmap loading

	// Create the dialog
	int width = 330;
	int hight = 235;
	create_dialog("AC97", width, hight, -1, KXFX_NO_RESIZE);

	//Set default (x,y) offset to (0,0) for standard 'kX objects'
	align_x =0;
	align_y =0;

	//set some fonts
	sliderfont.CreatePointFont(70,"Tahoma");

	// load all shared bitmaps
	hsliderback_140	=LoadBitmap(hmodule, MAKEINTRESOURCE(IDB_SLD110) );
	hsliderthumb	=LoadBitmap(hmodule, MAKEINTRESOURCE(IDB_THMB21) );
	henaon			=LoadBitmap(hmodule, MAKEINTRESOURCE(IDB_ENAON) );
	henaoff			=LoadBitmap(hmodule, MAKEINTRESOURCE(IDB_ENAOFF) );
	hgangon			=LoadBitmap(hmodule, MAKEINTRESOURCE(IDB_GANG_ON) );
	hgangoff		=LoadBitmap(hmodule, MAKEINTRESOURCE(IDB_GANG_OFF) );
	hbooston		=LoadBitmap(hmodule, MAKEINTRESOURCE(IDB_BOOST_ON) );
	hboostoff		=LoadBitmap(hmodule, MAKEINTRESOURCE(IDB_BOOST_OFF) );
	hudaon			=LoadBitmap(hmodule, MAKEINTRESOURCE(IDB_UDA_ON) );
	hudaoff			=LoadBitmap(hmodule, MAKEINTRESOURCE(IDB_UDA_OFF) );

	harrow			=LoadBitmap(hmodule, MAKEINTRESOURCE(IDB_ARROW_DOWN) );
	harrow_o		=LoadBitmap(hmodule, MAKEINTRESOURCE(IDB_ARROW_DOWN_O) );
	htxtbox			=LoadBitmap(hmodule, MAKEINTRESOURCE(IDB_TXTBOX) );
	
	hdsp			=LoadBitmap(hmodule, MAKEINTRESOURCE(IDB_DSP) );
	hdsp_o			=LoadBitmap(hmodule, MAKEINTRESOURCE(IDB_DSP_O) );
	hkfx			=LoadBitmap(hmodule, MAKEINTRESOURCE(IDB_KFX) );
	hkfx_o			=LoadBitmap(hmodule, MAKEINTRESOURCE(IDB_KFX_O) );

	// ******** Add plugin's controls *****************

#define top		50		// sliders top
#define slh		122		// sliders h
#define rsh		112		// real slider hight

		//add 'launch DSP' button
	launch_dsp.setbitmaps(hdsp,NULL,hdsp_o);
	launch_dsp.create(this,DSP_ID,"Open DSP", width-66,2,20);

	// add FX Pop Up Menu
	fxmenu.setbitmaps(hkfx,NULL,hkfx_o);
	fxmenu.create(this,FXMENU_ID,"Launch fX PlugIn",&sliderfont, width-86, 2);

	// Recording Gain L&R                             Min,Max, Left,Top, W, H)
	gainrecl.setbitmaps(hsliderback_140,hsliderthumb);
	gainrecl.create(this,GAINRECL_ID,"Left Rec Gain","RecL", &sliderfont,0,15, 20,top,21,rsh,30,6);
	gainrecl.sval.SetBkColor(RGB(123,0,0));	

	gainrecr.setbitmaps(hsliderback_140,hsliderthumb);
	gainrecr.create(this,GAINRECR_ID,"Right Rec Gain","RecR", &sliderfont,0,15, 75,top,21,rsh,30,6);
	gainrecr.sval.SetBkColor(RGB(123,0,0));

	on_rec.setbitmaps(henaoff,henaon);
	on_rec.create(this,ON_REC_ID,"Enable",					  75,top+slh, 30);

	on_gang.setbitmaps(hgangoff,hgangon);
	on_gang.create(this,ON_GANG_ID,"Gang Rec Source",		  20,top+slh, 30);

	// add Pop Up Menu
	sell.setbitmaps(harrow,harrow_o,htxtbox	);
	//                                                   items,left, top,        w
	sell.create(this,SELL_ID,"Left Rec Source",&sliderfont,7,   10,  top+slh+15, 32);
	sell.txtbox.SetTextColor(RGB(190,190,190));
	sell.set_itemtext(0,"Mic");
	sell.set_itemtext(1,"CD");
	sell.set_itemtext(2,"Vid");
	sell.set_itemtext(3,"Aux");
	sell.set_itemtext(4,"Line");
	sell.set_itemtext(5,"StMix");
	sell.set_itemtext(6,"MoMix");

	// add Pop Up Menu
	selr.setbitmaps(harrow,harrow_o,htxtbox	);
	//                                                   items,left, top,        w
	selr.create(this,SELR_ID,"Right Rec Source",&sliderfont,7,  60,  top+slh+15, 32);
	selr.txtbox.SetTextColor(RGB(190,190,190));
	selr.set_itemtext(0,"Mic");
	selr.set_itemtext(1,"CD");
	selr.set_itemtext(2,"Vid");
	selr.set_itemtext(3,"Aux");
	selr.set_itemtext(4,"Line");
	selr.set_itemtext(5,"StMix");
	selr.set_itemtext(6,"MoMix");

#define x1	140		// where mix section starts
#define s1	45		// space between faders

	skin.addBoxline(x1-20);		// add separation line

	gainline.setbitmaps(hsliderback_140,hsliderthumb);
	gainline.create(this,GAINLINE_ID,"Line In Gain","Line", &sliderfont,-23,8, x1,top,21,rsh,30,6);
	gainline.slider.set_page_size(10);
	gainline.sval.SetBkColor(RGB(123,0,0));

	on_line.setbitmaps(henaoff,henaon);
	on_line.create(this,ON_LINE_ID,"Enable LineIn", x1, top+slh, 30);

	gaincd.setbitmaps(hsliderback_140,hsliderthumb);
	gaincd.create(this,GAINCD_ID,"CD Gain","CD", &sliderfont,-23,8, x1+s1,top,21,rsh,30,6);
	gaincd.slider.set_page_size(10);
	gaincd.sval.SetBkColor(RGB(123,0,0));

	on_cd.setbitmaps(henaoff,henaon);
	on_cd.create(this,ON_CD_ID,"Enable CD", x1+s1, top+slh, 30);

	gainaux.setbitmaps(hsliderback_140,hsliderthumb);
	gainaux.create(this,GAINAUX_ID,"Aux Gain","Aux", &sliderfont,-23,8, x1+2*s1,top,21,rsh,30,6);
	gainaux.slider.set_page_size(10);
	gainaux.sval.SetBkColor(RGB(123,0,0));

	on_aux.setbitmaps(henaoff,henaon);
	on_aux.create(this,ON_AUX_ID,"Enable Aux", x1+2*s1, top+slh, 30);

	gainmic.setbitmaps(hsliderback_140,hsliderthumb);
	gainmic.create(this,GAINMIC_ID,"Mic Gain","Mic", &sliderfont,-23,8, x1+3*s1,top,21,rsh,30,6);
	gainmic.slider.set_page_size(10);
	gainmic.sval.SetBkColor(RGB(123,0,0));

	on_mic.setbitmaps(henaoff,henaon);
	on_mic.create(this,ON_MIC_ID,"Enable Mic", x1+3*s1, top+slh, 30);

	micboost.setbitmaps(hboostoff,hbooston);
	micboost.create(this,MICBOOST_ID,"+20dB Mic boost", x1+3*s1, top+slh+15, 30);

	if(!plugin->is_10k1)
	{
		setuda.setbitmaps(hudaoff,hudaon);
		setuda.create(this,SETUDA_ID,"Enable UDA codec", x1, top+slh+15, 30);
	}

	labelfont.CreatePointFont(80,"Tahoma");
	label1.Create("",WS_CHILD|WS_VISIBLE|SS_CENTER,
					CRect(CPoint(10,25),CSize(100,14)), this, NULL);
	label1.SetFont(&labelfont,NULL);
	label1.ChangeFontStyle(FC_FONT_BOLD);
	label1.SetBkColor(CLR_NONE);
	label1.SetWindowText("AC97 Source");

	label2.Create("",WS_CHILD|WS_VISIBLE|SS_CENTER,
					CRect(CPoint(x1+30,25),CSize(100,14)), this, NULL);
	label2.SetFont(&labelfont,NULL);
	label2.ChangeFontStyle(FC_FONT_BOLD);
	label2.SetBkColor(CLR_NONE);
	label2.SetWindowText("AC97 Mixer");



	// Get some info strings
	char devname[MAX_LABEL_STRING];
	plugin->ikx->get_string(KX_STR_CARD_NAME, devname);

	// Skin this plug!
	skin.skin_frame(this,this,width,hight,hmodule,"AC97",devname);	
	// set background          transp_clr
	set_background( skin.backmap, 0xff,0, kMETHOD_TRANS);


	controls_enabled = 1; // enable updating GUI when the parameters are changed
	sync(EVERYPM_ID); // update all 'GUI' elements to the preset plugin parameters
}
//---------------------------------------------------------------------------------
void iAdcPluginDlg::sync(int ndx)
{
	int v;
	char c[MAX_LABEL_STRING];

	switch (ndx)
	{ 
		case EVERYPM_ID: // update all controls

		case SETUDA_ID:
			if(!plugin->is_10k1)
			{
				v = plugin->_params[SETUDA_ID];
				if(controls_enabled)  { setuda.chk.set_check(v); setuda.chk.redraw();}
			}
			if (ndx != EVERYPM_ID) break;

		case ON_GANG_ID:
			v = plugin->_params[ON_GANG_ID];
			if(controls_enabled)  { on_gang.chk.set_check(v); on_gang.chk.redraw();}
			if (ndx != EVERYPM_ID) break;

		case SELL_ID:
			v = plugin->_params[SELL_ID];
			if(controls_enabled) sell.set_value(v);
			if(plugin->_params[ON_GANG_ID])
			{
				if(controls_enabled) selr.set_value(v);
			}
			if (ndx != EVERYPM_ID) break;

		case SELR_ID:
			v = plugin->_params[SELR_ID];
			if(controls_enabled) selr.set_value(v);
			if(plugin->_params[ON_GANG_ID])
			{
				if(controls_enabled) sell.set_value(v);
			}
			if (ndx != EVERYPM_ID) break;

		// ------ Rec Gain Faders and switches ---------
		case GAINRECL_ID:
			v = plugin->_params[GAINRECL_ID];
			sprintf(c, "%.1fdB", v * 1.5);
			gainrecl.sval.SetWindowText(c);
			VSSETPOS(gainrecl.slider, v);
			gainrecl.slider.redraw();
			if(plugin->_params[ON_GANG_ID])
			{
				//plugin->_params[GAINRECR_ID]=v;
				gainrecr.sval.SetWindowText(c);
				VSSETPOS(gainrecr.slider, v);
				gainrecr.slider.redraw();
			}
			if (ndx != EVERYPM_ID) break;
		case GAINRECR_ID:
			v = plugin->_params[GAINRECR_ID];
			sprintf(c, "%.1fdB", v * 1.5);
			gainrecr.sval.SetWindowText(c);
			VSSETPOS(gainrecr.slider, v);
			gainrecr.slider.redraw();
			if(plugin->_params[ON_GANG_ID])
			{
				//plugin->_params[GAINRECL_ID]=v;
				gainrecl.sval.SetWindowText(c);
				VSSETPOS(gainrecl.slider, v);
				gainrecl.slider.redraw();
			}
			if (ndx != EVERYPM_ID) break;
		case ON_REC_ID:
			v = plugin->_params[ON_REC_ID];
			if(controls_enabled)	{ on_rec.chk.set_check(v); on_rec.chk.redraw();}
			if (ndx != EVERYPM_ID) break;
		// -----------------------------------------
		
		case GAINLINE_ID:
			v = plugin->_params[GAINLINE_ID];
			if (controls_enabled)	VSSETPOS(gainline.slider, v);
			gainline.slider.redraw();			
			sprintf(c, "%.1fdB", v * 1.5);
			gainline.sval.SetWindowText(c);
			if (ndx != EVERYPM_ID) break;
			// fallthru
		case ON_LINE_ID:
			v = plugin->_params[ON_LINE_ID];
			if(controls_enabled)  { on_line.chk.set_check(v); on_line.chk.redraw();}
			if (ndx != EVERYPM_ID) break;


		case GAINCD_ID:
			v = plugin->_params[GAINCD_ID];
			if (controls_enabled)	VSSETPOS(gaincd.slider, v);
			gaincd.slider.redraw();
			sprintf(c, "%.1fdB", v * 1.5);
			gaincd.sval.SetWindowText(c);
			if (ndx != EVERYPM_ID) break;
			// fallthru
		case ON_CD_ID:
			v = plugin->_params[ON_CD_ID];
			if(controls_enabled)  { on_cd.chk.set_check(v); on_cd.chk.redraw();}
			if (ndx != EVERYPM_ID) break;


		case GAINAUX_ID:
			v = plugin->_params[GAINAUX_ID];
			if (controls_enabled)	VSSETPOS(gainaux.slider, v);
			gainaux.slider.redraw();
			sprintf(c, "%.1fdB", v * 1.5);
			gainaux.sval.SetWindowText(c);
			if (ndx != EVERYPM_ID) break;
			// fallthru
		case ON_AUX_ID:
			v = plugin->_params[ON_AUX_ID];
			if(controls_enabled)  { on_aux.chk.set_check(v); on_aux.chk.redraw();}
			if (ndx != EVERYPM_ID) break;


		case GAINMIC_ID:
			v = plugin->_params[GAINMIC_ID];
			if (controls_enabled)	VSSETPOS(gainmic.slider, v);
			gainmic.slider.redraw();
			sprintf(c, "%.1fdB", v * 1.5 + (20.0 * plugin->_params[MICBOOST_ID]) );
			gainmic.sval.SetWindowText(c);
			if (ndx != EVERYPM_ID) break;
			// fallthru
		case ON_MIC_ID:
			v = plugin->_params[ON_MIC_ID];
			if(controls_enabled)  { on_mic.chk.set_check(v); on_mic.chk.redraw(); }
			if (ndx != EVERYPM_ID) break;
		case MICBOOST_ID:
			v = plugin->_params[MICBOOST_ID];
			if(controls_enabled)  { micboost.chk.set_check(v); micboost.chk.redraw(); }
			v = plugin->_params[GAINMIC_ID];
			if (controls_enabled)
			{
				VSSETPOS(gainmic.slider, v);
				gainmic.slider.redraw();
			}
			sprintf(c, "%.1fdB", v * 1.5 + (20.0 * plugin->_params[MICBOOST_ID]) );
			gainmic.sval.SetWindowText(c);
			if (ndx != EVERYPM_ID) break;
	}
}
//------------------------------------------
int iAdcPluginDlg::on_command(int lp,int rp)
{
	int id = lp-kCONTROL_BASE;
	int state;
	
	if(id==SELL_ID)	// POPUPMENU
	{
		int val = sell.track(this); // track menu
		if(val == -1) return 1;
		plugin->set_param( id, val); // store new param value
		return 1;
	}
	if(id==SELR_ID)	// POPUPMENU
	{
		int val = selr.track(this); // track menu
		if(val == -1) return 1;
		plugin->set_param( id, val); // store new param value
		return 1;
	}
	if(id==ON_GANG_ID)
	{
		state = on_gang.chk.get_check(); //	get current state
		on_gang.chk.set_check(!state);	 // toggle state
		plugin->set_param( id, !state);
		return 1;
	}
	if(id==ON_REC_ID)
	{
		state = on_rec.chk.get_check(); //	get current state
		on_rec.chk.set_check(!state);	 // toggle state
		plugin->set_param( id, !state);
		return 1;
	}
	if(id==ON_LINE_ID)
	{
		state = on_line.chk.get_check(); //	get current state
		on_line.chk.set_check(!state);	 // toggle state
		plugin->set_param( id, !state);
		return 1;
	}
	if(id==ON_CD_ID)
	{
		state = on_cd.chk.get_check(); //	get current state
		on_cd.chk.set_check(!state);	 // toggle state
		plugin->set_param( id, !state);
		return 1;
	}
	if(id==ON_AUX_ID)
	{
		state = on_aux.chk.get_check(); //	get current state
		on_aux.chk.set_check(!state);	 // toggle state
		plugin->set_param( id, !state);
		return 1;
	}
	if(id==ON_MIC_ID)
	{
		state = on_mic.chk.get_check(); //	get current state
		on_mic.chk.set_check(!state);	 // toggle state
		plugin->set_param( id, !state);
		return 1;
	}
	if(id==MICBOOST_ID)
	{
		state = micboost.chk.get_check(); //	get current state
		micboost.chk.set_check(!state);	 // toggle state
		plugin->set_param( id, !state);
		return 1;
	}
	if(id==SETUDA_ID)
	{
		state = setuda.chk.get_check(); //	get current state
		setuda.chk.set_check(!state);	 // toggle state
		plugin->set_param( id, !state);
		return 1;
	}
	if(id==DSP_ID)	// open DSP Window via ikm
	{
		//launch(int what,int device_=-1,dword p1=0xffffffff,dword p2=0xffffffff);
		if(ikm)	ikm->launch(LAUNCH_DSP);
		return 1;
	}
	if(id==FXMENU_ID)	// FX POPUPMENU
	{
		// create and populate fx menu list
		fxmenu.create();
		for(int i=0;i<MAX_PGM_NUMBER;i++)
		{
			dsp_microcode mc;
			if(plugin->ikx->enum_microcode(i,&mc)==0)
			{
				char tmp[64];
				sprintf(tmp, "[%d] %s", i, mc.name);			
				fxmenu.add_item(i, tmp);
			}
		}
		int val = fxmenu.track(this);	// track menu
		fxmenu.destroy();				// delete menu
		if(val == -1) return 1;		
		plugin->pm->tweak_plugin(val);	// open selected plugin's tweak gui
		return 1;
	}


return CKXPluginGUI::on_command(lp,rp);
}
//---------------------------------------------------------------------------------------
void dbg(const char *format, ...)
{
	char	buf[4096];
	char	*p = buf;
	va_list	args;

	va_start(args, format);
	p += _vsnprintf(p, sizeof buf - 1, format, args);
	va_end(args);

	while ( p > buf  &&  isspace(p[-1]) )
			*--p = '\0';

	*p++ = '\r';
	*p++ = '\n';
	*p   = '\0';

	OutputDebugString(buf);
}
//----------------------------------------------------------

