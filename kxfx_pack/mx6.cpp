// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov, LeMury 2003-2004.
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

// Written by: LeMury
// Mixer v1.113b

#include "stdafx.h"
// --------------- the above should be a 'standard' beginning of any .cpp file
// effect class
#include "mx6.h"
#include "ColorCtrl.h"

// effect source
#include "da_mx6.cpp"


#define IDC_BTN_LVAL	300
#define IDC_BTN_RVAL	301
#define IDC_STC_VUSEL	302
#define IDC_STC_FINLVU	303
#define IDC_STC_FINRVU	304

//Microcode constants
#define y_off	0x2040u;	// is 0x0
#define y_on	0x204fu;	// is 0x7ffffffff

// plugin parameters (these are 'logical' parameters visible to the user)
typedef enum _params_id{
	VOL1_ID,	//fader
	VOL2_ID,	//fader
	VOL3_ID,	//fader
	VOL4_ID,	//fader
	VOL5_ID,	//fader
	VOL6_ID,	//fader
	S1A_ID,		//fader 
	S1B_ID,		//fader
	S2A_ID,		//fader
	S2B_ID,		//fader	10
	S3A_ID,		//fader
	S3B_ID,		//fader
	S4A_ID,		//fader
	S4B_ID,		//fader
	S5A_ID,		//fader
	S5B_ID,		//fader
	S6A_ID,		//fader
	S6B_ID,		//fader
	MUTE1_ID,	//chkbox
	MUTE2_ID,	//chkbox	20
	MUTE3_ID,	//chkbox
	MUTE4_ID,	//chkbox
	MUTE5_ID,	//chkbox
	MUTE6_ID,	//chkbox
	RS1_ID,		//chkbox
	RS2_ID,		//chkbox
	RS3_ID,		//chkbox
	RS4_ID,		//chkbox
	RS5_ID,		//chkbox
	RS6_ID,		//chkbox	30
	MS1_ID,		//chkbox
	MS2_ID,		//chkbox
	MS3_ID,		//chkbox
	MS4_ID,		//chkbox
	MS5_ID,		//chkbox
	MS6_ID,		//chkbox
	MON_ID,		//chkbox	37
	SEL_ID,
};

// default plugin parameters  (37 )
static kxparam_t default_params[MX6_PARAMS_COUNT] =
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


// define kX Automation: param description
// ---------------------------------------------------------------
static kx_fxparam_descr _param_descr[MX6_PARAMS_COUNT] = {
	{"input_level1",	1, 0, 100},
	{"input_level2",	1, 0, 100},
	{"input_level3",	1, 0, 100},
	{"input_level4",	1, 0, 100},
	{"input_level5",	1, 0, 100},
	{"input_level6",	1, 0, 100},
	{"send1>a",			1, 0, 100},
	{"send1>b",			1, 0, 100},
	{"send2>a",			1, 0, 100},
	{"send2>b",			1, 0, 100},
	{"send3>a",			1, 0, 100},
	{"send3>b",			1, 0, 100},
	{"send4>a",			1, 0, 100},
	{"send4>b",			1, 0, 100},
	{"send5>a",			1, 0, 100},
	{"send5>b",			1, 0, 100},
	{"send6>a",			1, 0, 100},
	{"send6>b",			1, 0, 100},
	{"input_enable1",   0, 0, 1},
	{"input_enable2",	0, 0, 1},
	{"input_enable3",	0, 0, 1},
	{"input_enable4",   0, 0, 1},
	{"input_enable5",	0, 0, 1},
	{"input_enable6",	0, 0, 1},
	{"rec_enable1",		0, 0, 1},
	{"rec_enable2",		0, 0, 1},
	{"rec_enable3",		0, 0, 1},
	{"rec_enable4",		0, 0, 1},
	{"rec_enable5",		0, 0, 1},
	{"rec_enable6",		0, 0, 1},
	{"main_enable1",	0, 0, 1},
	{"main_enable2",	0, 0, 1},
	{"main_enable3",	0, 0, 1},
	{"main_enable4",	0, 0, 1},
	{"main_enable5",	0, 0, 1},
	{"main_enable6",	0, 0, 1},
	{"recmon",			0, 0, 1},
	{"vu_select",		0, 0, 1}};

// Describe parameters
int iMx6Plugin::describe_param(int ndx, kx_fxparam_descr *descr)
{
	memcpy(descr, &_param_descr[ndx], sizeof kx_fxparam_descr); return 0;
}


//////////////////////////////////////////////////////////////////////////////
/* Plugin Implemenation */
//---------------------------------------------------------------------------------------
int iMx6Plugin::get_param(int ndx, kxparam_t *value) // ndx corresponds to 'logical' parameter id
{
	*value = _params[ndx]; 
	return 0;
}
// set_param sets 'logical' values by writing to 'physical' registers
//-------------------------------------------------------------------
int iMx6Plugin::set_param(int ndx, kxparam_t value)
{
	_params[ndx] = value;
	switch (ndx) {

	case VOL1_ID:	//IN1
	case MUTE1_ID:
		value = _params[VOL1_ID];
		if ((_params[MUTE1_ID])==0) value = 0;	/* Invert Mute function */
		set_dsp_register(_VOL1, itodspr_scl(100, value));
		break;

	case VOL2_ID:	//IN2
	case MUTE2_ID:
		value = _params[VOL2_ID];
		if ((_params[MUTE2_ID])==0) value = 0;
		set_dsp_register(_VOL2, itodspr_scl(100, value));
		break;
	case VOL3_ID:	//IN3
	case MUTE3_ID:
		value = _params[VOL3_ID];
		if ((_params[MUTE3_ID])==0) value = 0;
		set_dsp_register(_VOL3, itodspr_scl(100, value));
		break;
	case VOL4_ID:	//IN4
	case MUTE4_ID:
		value = _params[VOL4_ID];
		if ((_params[MUTE4_ID])==0) value = 0;
		set_dsp_register(_VOL4, itodspr_scl(100, value));
		break;
	case VOL5_ID:	//IN5
	case MUTE5_ID:
		value = _params[VOL5_ID];
		if ((_params[MUTE5_ID])==0) value = 0;
		set_dsp_register(_VOL5, itodspr_scl(100, value));
		break;
	case VOL6_ID:	//IN6
	case MUTE6_ID:
		value = _params[VOL6_ID];
		if ((_params[MUTE6_ID])==0) value = 0;
		set_dsp_register(_VOL6, itodspr_scl(100, value));
		break;
//-----------

	case S1A_ID:	//SEND 1A 
		set_dsp_register(_S1A, itodspr_scl(100, value));
		break;
	case S1B_ID:	//SEND 1B 
		set_dsp_register(_S1B, itodspr_scl(100, value));
		break;
	case S2A_ID:	//SEND 2A
		set_dsp_register(_S2A, itodspr_scl(100, value));
		break;
	case S2B_ID:	//SEND 2B
		set_dsp_register(_S2B, itodspr_scl(100, value));
		break;
	case S3A_ID:	//SEND 3A 
		set_dsp_register(_S3A, itodspr_scl(100, value));
		break;
	case S3B_ID:	//SEND 3B
		set_dsp_register(_S3B, itodspr_scl(100, value));
		break;
	case S4A_ID:	//SEND 4A
		set_dsp_register(_S4A, itodspr_scl(100, value));
		break;
	case S4B_ID:	//SEND 4B
		set_dsp_register(_S4B, itodspr_scl(100, value));
		break;
	case S5A_ID:	//SEND 5A 
		set_dsp_register(_S5A, itodspr_scl(100, value));
		break;
	case S5B_ID:	//SEND 5B
		set_dsp_register(_S5B, itodspr_scl(100, value));
		break;
	case S6A_ID:	//SEND 6A
		set_dsp_register(_S6A, itodspr_scl(100, value));
		break;
	case S6B_ID:	//SEND 6B
		set_dsp_register(_S6B, itodspr_scl(100, value));
		break;
//-------------
	word Y;	//0x2040u is 0x0, 0x204fu is 0x7ffffffff

	case MS1_ID:	//MAIN enable sw 1
		Y = y_off
		if (_params[MS1_ID]) Y = y_on;
		write_instr_y(_L_INSTR + 1, Y);
		write_instr_y(_R_INSTR + 1, Y);
		break;
	case MS2_ID:	//MAIN enable sw 2
		Y = y_off
		if (_params[MS2_ID]) Y = y_on;
		write_instr_y(_L_INSTR + 6, Y);
		write_instr_y(_R_INSTR + 6, Y);
		break;
	case MS3_ID:	//MAIN enable sw 3
		Y = y_off
		if (_params[MS3_ID]) Y = y_on;
		write_instr_y(_L_INSTR + 11, Y);
		write_instr_y(_R_INSTR + 11, Y);
		break;
	case MS4_ID:	//MAIN enable sw 4
		Y = y_off
		if (_params[MS4_ID]) Y = y_on;
		write_instr_y(_L_INSTR + 16, Y);
		write_instr_y(_R_INSTR + 16, Y);
		break;
	case MS5_ID:	//MAIN enable sw 5
		Y = y_off
		if (_params[MS5_ID]) Y = y_on;
		write_instr_y(_L_INSTR + 21, Y);
		write_instr_y(_R_INSTR + 21, Y);
		break;
	case MS6_ID:	//MAIN enable sw 6
		Y = y_off
		if (_params[MS6_ID]) Y = y_on;
		write_instr_y(_L_INSTR + 26, Y);
		write_instr_y(_R_INSTR + 26, Y);
		break;
//-----------------

	case RS1_ID:	//REC IN SW 1
		Y = y_off
		if (_params[RS1_ID]) Y = y_on;
		write_instr_y(_L_INSTR + 2, Y);
		write_instr_y(_R_INSTR + 2, Y);
		break;
	case RS2_ID:	//REC IN SW 2
		Y = y_off
		if (_params[RS2_ID]) Y = y_on;
		write_instr_y(_L_INSTR + 7, Y);
		write_instr_y(_R_INSTR + 7, Y);
		break;
	case RS3_ID:	//REC IN SW 3
		Y = y_off
		if (_params[RS3_ID]) Y = y_on;
		write_instr_y(_L_INSTR + 12, Y);
		write_instr_y(_R_INSTR + 12, Y);
		break;
	case RS4_ID:	//REC IN SW 4
		Y = y_off
		if (_params[RS4_ID]) Y = y_on;
		write_instr_y(_L_INSTR + 17, Y);
		write_instr_y(_R_INSTR + 17, Y);
		break;
	case RS5_ID:	//REC IN SW 5
		Y = y_off
		if (_params[RS5_ID]) Y = y_on;
		write_instr_y(_L_INSTR + 22, Y);
		write_instr_y(_R_INSTR + 22, Y);
		break;
	case RS6_ID:	//REC IN SW 6
		Y = y_off
		if (_params[RS6_ID]) Y = y_on;
		write_instr_y(_L_INSTR + 27, Y);
		write_instr_y(_R_INSTR + 27, Y);
		break;

	case MON_ID:	//RECMON SW
		Y = y_off
		if (_params[MON_ID]) Y = y_on;
		write_instr_x(_L_INSTR + 30, Y);
		write_instr_x(_R_INSTR + 30, Y);
		write_instr_x(_L_INSTR + 31, Y);
		write_instr_x(_R_INSTR + 31, Y);
		break;

	case SEL_ID:	//VU SEL SW
		if (_params[SEL_ID]) {
		write_instr_a(_L_INSTR + 32, 0x801Eu );
		write_instr_a(_R_INSTR + 32, 0x801Eu);}
		else {
		write_instr_a(_L_INSTR + 32, 0x801Fu);
		write_instr_a(_R_INSTR + 32, 0x801Fu);}
		break;
	}

// we need do synchronize the GUI part, if set_param was called by the mixer or kX Automation
	if (cp) ((iMx6PluginDlg*) cp)->sync(ndx);

	return 0;
}
//-------------------------------------------
// Set all params to defaults

int iMx6Plugin::set_defaults() 
{
	set_all_params(default_params); 
	return 0;
}

//--------------------------------------------------
int iMx6Plugin::set_all_params(kxparam_t* values)
{
	for (int i = 0; i < MX6_PARAMS_COUNT; i++) set_param(i, values[i]);
	return 0;
}
// Get DSP Microcode
//----------------------------------------------------
int iMx6Plugin::request_microcode() 
{
	publish_microcode(mx6);

	return 0;
}
const char *iMx6Plugin::get_plugin_description(int id)
{
	plugin_description(mx6);
}

//--------------------------------------------
// Create own Control panel (cp)
iKXPluginGUI *iMx6Plugin::create_cp(kDialog *parent, kFile *mf)
{
	iMx6PluginDlg *tmp;
	tmp = new iMx6PluginDlg(this, parent,mf);
	tmp->create();
	return tmp;
}
int iMx6Plugin::get_user_interface()
{
	return IKXPLUGIN_OWNER; // means 'custom' GUI panel
}

//////////////////////////////////////////////////////////////////
// VU Peak UI Implementation 
//----------------------------------------------------------------
BEGIN_MESSAGE_MAP(iMx6PluginDlg, CKXPluginGUI)
	ON_WM_TIMER()
	ON_BN_CLICKED (IDC_BTN_LVAL, OnClick)
	ON_BN_CLICKED (IDC_BTN_RVAL, OnClick)
END_MESSAGE_MAP()

iMx6PluginDlg::iMx6PluginDlg(iMx6Plugin *plg, kDialog *parent_,kFile *mf_) 
 : CKXPluginGUI(parent_,mf_) 
{ 
	  plugin=plg;
          timer_id=(UINT_PTR)-1;
          max_l=-120;
          max_r=-120;
}

iMx6PluginDlg::~iMx6PluginDlg()
{
         	turn_on(0);
}

int iMx6PluginDlg::turn_on(int what)
{
 if((what==1) && (timer_id==-1))
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

// Plugin GUI Implementation (init)
//----------------------------------------------------------------------
void iMx6PluginDlg::init()
{
	CKXPluginGUI::init();

	// custom dialog box    (title, Width, Height)
	create_dialog(plugin_name(mx6), 485, 463);

// load custom attached bitmaps here
	vumeter.set_bitmaps(
		 mf.load_image("fxlib/vulev.bmp"),
		 mf.load_image("fxlib/vuhigh.bmp"));

// creat font for vu text
	vufont.CreatePointFont(100,"Tahoma");

	// SHOW_VFADER(Fader, Ctrl_ID, Label, MinVal,MaxVal,Left, Top, Width, Height)
	create_vfader(fdrvol1,	VOL1_ID, "IN1",  0, 100,	6,	  280,	50,	170);
	create_vfader(fdrvol2,	VOL2_ID, "IN2",  0, 100,	65,	  280,	50,	170);
	create_vfader(fdrvol3,	VOL3_ID, "IN3",  0, 100,	125,  280,	50,	170);
	create_vfader(fdrvol4,	VOL4_ID, "IN4",  0, 100,	185,  280,	50,	170);
	create_vfader(fdrvol5,	VOL5_ID, "IN5",  0, 100,	245,  280,	50,	170);
	create_vfader(fdrvol6,	VOL6_ID, "IN6",  0, 100,	305,  280,	50,	170);

	// SEND FADERS
	create_vfader(fdrs1a,	S1A_ID,	 "S1",   0, 100,	6,	  175,	50,	85);
	create_vfader(fdrs1b,	S1B_ID,	 "S2",   0, 100,	6,	  80,	50,	85);
	create_vfader(fdrs2a,	S2A_ID,	 "S1",   0, 100,	65,	  175,	50,	85);
	create_vfader(fdrs2b,	S2B_ID,	 "S2",   0, 100,	65,	  80,	50,	85);
	create_vfader(fdrs3a,	S3A_ID,	 "S1",   0, 100,	125,  175,	50,	85);
	create_vfader(fdrs3b,	S3B_ID,	 "S2",   0, 100,	125,  80,	50,	85);
	create_vfader(fdrs4a,	S4A_ID,	 "S1",   0, 100,	185,  175,	50,	85);
	create_vfader(fdrs4b,	S4B_ID,	 "S2",   0, 100,	185,  80,	50,	85);
	create_vfader(fdrs5a,	S5A_ID,	 "S1",   0, 100,	245,  175,	50,	85);
	create_vfader(fdrs5b,	S5B_ID,	 "S2",   0, 100,	245,  80,	50,	85);
	create_vfader(fdrs6a,	S6A_ID,	 "S1",   0, 100,	305,  175,	50,	85);
	create_vfader(fdrs6b,	S6B_ID,	 "S2",   0, 100,	305,  80,	50,	85);

	// Enable BOXES           ID,    Name, Left,Top, Width)
	create_checkbox(chkmute1, MUTE1_ID, "E", 20, 60, 30);
	create_checkbox(chkmute2, MUTE2_ID, "E", 79, 60, 30);
	create_checkbox(chkmute3, MUTE3_ID, "E", 138,60, 30);
	create_checkbox(chkmute4, MUTE4_ID, "E", 197,60, 30);
	create_checkbox(chkmute5, MUTE5_ID, "E", 256,60, 30);
	create_checkbox(chkmute6, MUTE6_ID, "E", 315,60, 30);

	// RECORD BUS BOXES
	create_checkbox(chkrec1,  RS1_ID,	"R", 20,  20, 30);
	create_checkbox(chkrec2,  RS2_ID,	"R", 79,  20, 30);
	create_checkbox(chkrec3,  RS3_ID,	"R", 138, 20, 30);
	create_checkbox(chkrec4,  RS4_ID,	"R", 197, 20, 30);
	create_checkbox(chkrec5,  RS5_ID,	"R", 256, 20, 30);
	create_checkbox(chkrec6,  RS6_ID,	"R", 315, 20, 30);
	// MAIN BUS BOXES
	create_checkbox(chkms1,  MS1_ID,	"M", 20,  40, 30);
	create_checkbox(chkms2,  MS2_ID,	"M", 79,  40, 30);
	create_checkbox(chkms3,  MS3_ID,	"M", 138, 40, 30);
	create_checkbox(chkms4,  MS4_ID,	"M", 197, 40, 30);
	create_checkbox(chkms5,  MS5_ID,	"M", 256, 40, 30);
	create_checkbox(chkms6,  MS6_ID,	"M", 315, 40, 30);
	// RECMON AND VU SWITCH
	create_checkbox(chkmon, MON_ID, "RECMON", 374, 20, 90);
	create_checkbox(chksel, SEL_ID, "Vu=",     374, 40, 45);

	// create CStatic Vu assignment
	vusel.Create("    ",WS_CHILD | WS_VISIBLE | SS_CENTER,
		CRect(CPoint(420,62),CSize(40,18)), this, IDC_STC_VUSEL);


	// create CStatic final Left Vu numeric
	finlvu.Create("    ",WS_CHILD | WS_BORDER | WS_VISIBLE | SS_CENTER,
		CRect(CPoint(380,109),CSize(37,18)), this, IDC_STC_FINLVU);
	// draw CStatic final Right Vu numeric
	finrvu.Create("    ",WS_CHILD | WS_BORDER |WS_VISIBLE | SS_CENTER,
		CRect(CPoint(425,109),CSize(37,18)), this, IDC_STC_FINRVU);

	finlvu.SetBkColor(RGB(200,200,200));
	finrvu.SetBkColor(RGB(200,200,200));

	// show CStatic Left peak hold							Event
	lval.Create("    ",WS_CHILD | WS_BORDER | WS_VISIBLE | SS_NOTIFY | SS_CENTER,
		CRect(CPoint(380,130),CSize(37,18)), this, IDC_BTN_LVAL);
	// show CStatic Right peak hold							Event
	rval.Create("    ",WS_CHILD | WS_BORDER | WS_VISIBLE | SS_NOTIFY | SS_CENTER,
		CRect(CPoint(425,130),CSize(37,18)), this, IDC_BTN_RVAL);

	// set vu text font
	vusel.SetFont(&vufont,NULL);
	finlvu.SetFont(&vufont,NULL);
	finrvu.SetFont(&vufont,NULL);
	lval.SetFont(&vufont,NULL);
	rval.SetFont(&vufont,NULL);

	// show VU (bitmap) meter left, top  
	vumeter.create(mx6_name, 375, 150,this,1);
	vumeter.show();

	turn_on(1);
	redraw();

	controls_enabled = 1; // enable updating GUI when the parameters are changed
	sync(EVERYPM_ID); // update all 'GUI' elements to the preset plugin parameters
}

// Sync any labels with the paramaters, update all controls
//----------------------------------------------------------
void iMx6PluginDlg::sync(int ndx)
{
	char c[MAX_LABEL_STRING];
	kxparam_t v;       
	switch (ndx) // this function should 'synchronize' any labels with the parameters
	{ 
	case EVERYPM_ID: // update all controls

	case VOL1_ID:
		v = plugin->_params[VOL1_ID];
		if (controls_enabled) {VSSETPOS(fdrvol1.slider, (int)v); fdrvol1.slider.redraw();}
		sprintf(c, "%i" , v);
		fdrvol1.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;
	case VOL2_ID:
		v = plugin->_params[VOL2_ID];
		if (controls_enabled) {VSSETPOS(fdrvol2.slider, (int)v); fdrvol2.slider.redraw();}
		sprintf(c, "%i" , v);
		fdrvol2.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;	
	case VOL3_ID:
		v = plugin->_params[VOL3_ID];
		if (controls_enabled) {VSSETPOS(fdrvol3.slider, (int)v); fdrvol3.slider.redraw();}
		sprintf(c, "%i" , v);
		fdrvol3.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;
	case VOL4_ID:
		v = plugin->_params[VOL4_ID];
		if (controls_enabled) {VSSETPOS(fdrvol4.slider, (int)v); fdrvol4.slider.redraw();}
		sprintf(c, "%i" , v);
		fdrvol4.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;	
	case VOL5_ID:
		v = plugin->_params[VOL5_ID];
		if (controls_enabled) {VSSETPOS(fdrvol5.slider, (int)v); fdrvol5.slider.redraw();}
		sprintf(c, "%i" , v);
		fdrvol5.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;
	case VOL6_ID:
		v = plugin->_params[VOL6_ID];
		if (controls_enabled) {VSSETPOS(fdrvol6.slider, (int)v); fdrvol6.slider.redraw();}
		sprintf(c, "%i" , v);
		fdrvol6.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;	
//-------------

	case S1A_ID:
		v = plugin->_params[S1A_ID];
		if (controls_enabled) {VSSETPOS(fdrs1a.slider, (int)v); fdrs1a.slider.redraw();}
		sprintf(c, "%i" , v);
		fdrs1a.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;
	case S1B_ID:
		v = plugin->_params[S1B_ID];
		if (controls_enabled) {VSSETPOS(fdrs1b.slider, (int)v); fdrs1b.slider.redraw();}
		sprintf(c, "%i" , v);
		fdrs1b.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;
	case S2A_ID:
		v = plugin->_params[S2A_ID];
		if (controls_enabled) {VSSETPOS(fdrs2a.slider, (int)v); fdrs2a.slider.redraw();}
		sprintf(c, "%i" , v);
		fdrs2a.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;
	case S2B_ID:
		v = plugin->_params[S2B_ID];
		if (controls_enabled) {VSSETPOS(fdrs2b.slider, (int)v); fdrs2b.slider.redraw();}
		sprintf(c, "%i" , v);
		fdrs2b.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;
	case S3A_ID:
		v = plugin->_params[S3A_ID];
		if (controls_enabled) {VSSETPOS(fdrs3a.slider, (int)v); fdrs3a.slider.redraw();}
		sprintf(c, "%i" , v);
		fdrs3a.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;
	case S3B_ID:
		v = plugin->_params[S3B_ID];
		if (controls_enabled) {VSSETPOS(fdrs3b.slider, (int)v); fdrs3b.slider.redraw();}
		sprintf(c, "%i" , v);
		fdrs3b.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;
	case S4A_ID:
		v = plugin->_params[S4A_ID];
		if (controls_enabled) {VSSETPOS(fdrs4a.slider, (int)v); fdrs4a.slider.redraw();}
		sprintf(c, "%i" , v);
		fdrs4a.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;
	case S4B_ID:
		v = plugin->_params[S4B_ID];
		if (controls_enabled) {VSSETPOS(fdrs4b.slider, (int)v); fdrs4b.slider.redraw();}
		sprintf(c, "%i" , v);
		fdrs4b.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;
	case S5A_ID:
		v = plugin->_params[S5A_ID];
		if (controls_enabled) {VSSETPOS(fdrs5a.slider, (int)v); fdrs5a.slider.redraw();}
		sprintf(c, "%i" , v);
		fdrs5a.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;
	case S5B_ID:
		v = plugin->_params[S5B_ID];
		if (controls_enabled) {VSSETPOS(fdrs5b.slider, (int)v); fdrs5b.slider.redraw();}
		sprintf(c, "%i" , v);
		fdrs5b.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;
	case S6A_ID:
		v = plugin->_params[S6A_ID];
		if (controls_enabled) {VSSETPOS(fdrs6a.slider, (int)v); fdrs6a.slider.redraw();}
		sprintf(c, "%i" , v);
		fdrs6a.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;
	case S6B_ID:
		v = plugin->_params[S6B_ID];
		if (controls_enabled) {VSSETPOS(fdrs6b.slider, (int)v); fdrs6b.slider.redraw();}
		sprintf(c, "%i" , v);
		fdrs6b.svalue.SetWindowText(c);
		if (ndx != EVERYPM_ID) break;
//-----------

	case MUTE1_ID:
		v = plugin->_params[MUTE1_ID];
		if (controls_enabled) {chkmute1.set_check((int)v); chkmute1.redraw();}
		if (ndx != EVERYPM_ID) break;
	case MUTE2_ID:
		v = plugin->_params[MUTE2_ID];
		if (controls_enabled) {chkmute2.set_check((int)v); chkmute2.redraw();}
		if (ndx != EVERYPM_ID) break;
	case MUTE3_ID:
		v = plugin->_params[MUTE3_ID];
		if (controls_enabled) {chkmute3.set_check((int)v); chkmute3.redraw();}
		if (ndx != EVERYPM_ID) break;
	case MUTE4_ID:
		v = plugin->_params[MUTE4_ID];
		if (controls_enabled) {chkmute4.set_check((int)v); chkmute4.redraw();}
		if (ndx != EVERYPM_ID) break;
	case MUTE5_ID:
		v = plugin->_params[MUTE5_ID];
		if (controls_enabled) {chkmute5.set_check((int)v); chkmute5.redraw();}
		if (ndx != EVERYPM_ID) break;
	case MUTE6_ID:
		v = plugin->_params[MUTE6_ID];
		if (controls_enabled) {chkmute6.set_check((int)v); chkmute6.redraw();}
		if (ndx != EVERYPM_ID) break;
//------------

	case RS1_ID:
		v = plugin->_params[RS1_ID];
		if (controls_enabled) {chkrec1.set_check((int)v); chkrec1.redraw();}
		if (ndx != EVERYPM_ID) break;
	case RS2_ID:
		v = plugin->_params[RS2_ID];
		if (controls_enabled) {chkrec2.set_check((int)v); chkrec2.redraw();}
		if (ndx != EVERYPM_ID) break;
	case RS3_ID:
		v = plugin->_params[RS3_ID];
		if (controls_enabled) {chkrec3.set_check((int)v); chkrec3.redraw();}
		if (ndx != EVERYPM_ID) break;
	case RS4_ID:
		v = plugin->_params[RS4_ID];
		if (controls_enabled) {chkrec4.set_check((int)v); chkrec4.redraw();}
		if (ndx != EVERYPM_ID) break;
	case RS5_ID:
		v = plugin->_params[RS5_ID];
		if (controls_enabled) {chkrec5.set_check((int)v); chkrec5.redraw();}
		if (ndx != EVERYPM_ID) break;
	case RS6_ID:
		v = plugin->_params[RS6_ID];
		if (controls_enabled) {chkrec6.set_check((int)v); chkrec6.redraw();}
		if (ndx != EVERYPM_ID) break;
//--------
	case MS1_ID:
		v = plugin->_params[MS1_ID];
		if (controls_enabled) {chkms1.set_check((int)v); chkms1.redraw();}
		if (ndx != EVERYPM_ID) break;
	case MS2_ID:
		v = plugin->_params[MS2_ID];
		if (controls_enabled) {chkms2.set_check((int)v); chkms2.redraw();}
		if (ndx != EVERYPM_ID) break;
	case MS3_ID:
		v = plugin->_params[MS3_ID];
		if (controls_enabled) {chkms3.set_check((int)v); chkms3.redraw();}
		if (ndx != EVERYPM_ID) break;
	case MS4_ID:
		v = plugin->_params[MS4_ID];
		if (controls_enabled) {chkms4.set_check((int)v); chkms4.redraw();}
		if (ndx != EVERYPM_ID) break;
	case MS5_ID:
		v = plugin->_params[MS5_ID];
		if (controls_enabled) {chkms5.set_check((int)v); chkms5.redraw();}
		if (ndx != EVERYPM_ID) break;
	case MS6_ID:
		v = plugin->_params[MS6_ID];
		if (controls_enabled) {chkms6.set_check((int)v); chkms6.redraw();}
		if (ndx != EVERYPM_ID) break;

//---------
	case MON_ID:
		v = plugin->_params[MON_ID];
		if (controls_enabled) {chkmon.set_check((int)v); chkmon.redraw();}
		if (ndx != EVERYPM_ID) break;

	case SEL_ID:	// vu select
		v = plugin->_params[SEL_ID];
		if (v) {vusel.SetWindowText("MAIN");}
		else {vusel.SetWindowText("REC");}
		if (controls_enabled) {chksel.set_check((int)v); chksel.redraw();}
		if (ndx != EVERYPM_ID) break;

	}
}

// VU onTimer based Recalc and Redraw/Refresh 
///////////////////////////////////////////////////////////////////////////
void iMx6PluginDlg::OnTimer(UINT_PTR)
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

//--- VU numeric text output ----------

        char fin1[12];	
        char fin2[12];	

       if(left_f!=-120)
         sprintf(fin1,"%03.1f",left_f);
        else
         sprintf(fin1,"-Inf");

       if(right_f!=-120)
         sprintf(fin2,"%03.1f",right_f);
        else
         sprintf(fin2,"-Inf");

        sprintf(fin1,"%s",fin1);
		sprintf(fin2,"%s",fin2);

	// update vu text data static labels
		finlvu.SetWindowText(fin1);
		finrvu.SetWindowText(fin2);

//--- VU text 'Peak Hold' output --------

        char tmp1[12];

        if(max_l!=-120)
			if(max_l > 0.1){
				sprintf(tmp1,"%03.1f",max_l);
				lval.SetBkColor(RGB(255,0,0));}
			else {
				sprintf(tmp1,"%03.1f",max_l);
				lval.SetBkColor(RGB(200,200,200));}
			else{
			sprintf(tmp1,"-Inf");
			lval.SetBkColor(RGB(200,200,200));}

lval.SetWindowText(tmp1); 	// update left peak hold static label


        if(max_r!=-120)
			if(max_r > 0.1){
				sprintf(tmp1,"%03.1f",max_r);
				rval.SetBkColor(RGB(255,0,0));}
			else {
				sprintf(tmp1,"%03.1f",max_r);
				rval.SetBkColor(RGB(200,200,200));}
			else{
			sprintf(tmp1,"-Inf");
			rval.SetBkColor(RGB(200,200,200));}

rval.SetWindowText(tmp1); 	// update right peak hold static label

       }
}

//---------------------------------------------
void iMx6PluginDlg::on_destroy()
{
 turn_on(0);
}
//---------------------------------------------
int iMx6PluginDlg::on_command(int lp,int rp)
{
 return CKXPluginGUI::on_command(lp,rp);
}

//------------------------------------------
// Reset 'clip peak hold' on mouse click
//--------------------------------------------
void iMx6PluginDlg::OnClick()
{
	max_l=-120;
	max_r=-120;
}
//----------------------------
