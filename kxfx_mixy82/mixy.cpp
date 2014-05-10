// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov, LeMury 2005.
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

// Written by: townkat
////////////////////////////////////////////////////////////////
//mixy.cpp	VU Meter Example - LeMury 2005
////////////////////////////////////////////////////////////////

#include "stdafx.h"
// --------------- the above should be a 'standard' beginning of any .cpp file
// effect class
#include "mixy.h"
// effect source
#include "da_mixy.cpp"

#define P_RESET1	0	//vumeter reset button
#define P_RESET2	1
#define P_RESET3	2
#define P_RESET4	3
#define P_RESET5	4
#define P_RESET6	5
#define P_RESET7	6
#define P_RESET8	7
#define P_RESET9	8

#define MIXY_PLG_EDIT1	0x10001
#define MIXY_PLG_EDIT2	0x10002
#define MIXY_PLG_EDIT3	0x10003
#define MIXY_PLG_EDIT4	0x10004
#define MIXY_PLG_EDIT5	0x10005
#define MIXY_PLG_EDIT6	0x10006
#define MIXY_PLG_EDIT7	0x10007
#define MIXY_PLG_EDIT8	0x10008
#define MIXY_PLG_EDIT9	0x10009

char key_name[32];

// plugin parameters (these are 'logical' parameters)
typedef enum _params_id{
	VOL1_ID,//0	//fader
	VOL2_ID,//1
	VOL3_ID,//2
	VOL4_ID,//3
	VOL5_ID,//4
	VOL6_ID,//5
	VOL7_ID,//6
	VOL8_ID,//7
	VOL9_ID,//8
	VOL10_ID,//9
	VOL11_ID,//10
	VOL12_ID,//11
	VOL13_ID,//12
	VOL14_ID,//13
	VOL15_ID,//14
	VOL16_ID,//15
	VOL17_ID,//16
	VOL18_ID,//17
	MUTE1_ID,//18
	MUTE2_ID,//29
	MUTE3_ID,//10
	MUTE4_ID,//21
	MUTE5_ID,//22
	MUTE6_ID,//23
	MUTE7_ID,//24
	MUTE8_ID,//25
	MUTE9_ID,//26
	MUTE10_ID,//27
	MUTE11_ID,//28
	MUTE12_ID,//29
	MUTE13_ID,//30
	MUTE14_ID,//31
	MUTE15_ID,//32
	MUTE16_ID,//33
	MUTE17_ID,//34
	MUTE18_ID,//35
	STLINK1_ID,//36
	STLINK2_ID,//37
	STLINK3_ID,//38
	STLINK4_ID,//39
	STLINK5_ID,//40
	STLINK6_ID,//41
	STLINK7_ID,//42
	STLINK8_ID,//43
	STLINK9_ID,//44
	VURANGE1_ID,//45
	VURANGE2_ID,//46
	VURANGE3_ID,//47
	VURANGE4_ID,//48
	VURANGE5_ID,//49
	VURANGE6_ID,//50
	VURANGE7_ID,//51
	VURANGE8_ID,//52
	VURANGE9_ID,//53
	PAN1_ID,//54
	PAN2_ID,//55
	PAN3_ID,//56
	PAN4_ID,//57
	PAN5_ID,//58
	PAN6_ID,//59
	PAN7_ID,//60
	PAN8_ID,//61
	PAN9_ID,//62
	PAN10_ID,//63
	PAN11_ID,//64
	PAN12_ID,//65
	PAN13_ID,//66
	PAN14_ID,//67
	PAN15_ID,//68
	PAN16_ID,//69
	SOLO1_ID,//70
	SOLO2_ID,//71
	SOLO3_ID,//72
	SOLO4_ID,//73
	SOLO5_ID,//74
	SOLO6_ID,//75
	SOLO7_ID,//76
	SOLO8_ID,//77
	SOLO9_ID,//78
	SOLO10_ID,//79
	SOLO11_ID,//80
	SOLO12_ID,//81
	SOLO13_ID,//82
	SOLO14_ID,//83
	SOLO15_ID,//84
	SOLO16_ID,//85
	MONOMIX_ID,//86
};

// Define default params 
static kxparam_t default_params[MIXY_PARAMS_COUNT] = {0,0,0,0,0,0,0,0,0,0,
												0,0,0,0,0,0,0,0,
												0,0,0,0,0,0,0,0,0,0,
												0,0,0,0,0,0,0,0,
												1,1,1,1,1,
												1,1,1,1,
												3,3,3,3,3,
												3,3,3,3,
												-10,10,-10,10,-10,10,-10,10,
												-10,10,-10,10,-10,10,-10,10,
												0,0,0,0,0,0,0,0,
												0,0,0,0,0,0,0,0,
												0,
												};

// define kX Automation: param description
// ---------------------------------------------------------------
static kx_fxparam_descr _param_descr[MIXY_PARAMS_COUNT] = {
	{"vol1",	KX_FXPARAM_DB, -120, 12},
	{"vol2",    KX_FXPARAM_DB, -120, 12},
	{"vol3",	KX_FXPARAM_DB, -120, 12},
	{"vol4",    KX_FXPARAM_DB, -120, 12},
	{"vol5",	KX_FXPARAM_DB, -120, 12},
	{"vol6",    KX_FXPARAM_DB, -120, 12},
	{"vol7",	KX_FXPARAM_DB, -120, 12},
	{"vol8",    KX_FXPARAM_DB, -120, 12},
	{"vol9",	KX_FXPARAM_DB, -120, 12},
	{"vol10",    KX_FXPARAM_DB, -120, 12},
	{"vol11",	KX_FXPARAM_DB, -120, 12},
	{"vol12",    KX_FXPARAM_DB, -120, 12},
	{"vol13",	KX_FXPARAM_DB, -120, 12},
	{"vol14",    KX_FXPARAM_DB, -120, 12},
	{"vol15",	KX_FXPARAM_DB, -120, 12},
	{"vol16",    KX_FXPARAM_DB, -120, 12},
	{"vol17",	KX_FXPARAM_DB, -120, 12},
	{"vol18",    KX_FXPARAM_DB, -120, 12},
	{"mute1",  KX_FXPARAM_SWITCH, 0, 1},
	{"mute2",  KX_FXPARAM_SWITCH, 0, 1},
	{"mute3",  KX_FXPARAM_SWITCH, 0, 1},
	{"mute4",  KX_FXPARAM_SWITCH, 0, 1},
	{"mute5",  KX_FXPARAM_SWITCH, 0, 1},
	{"mute6",  KX_FXPARAM_SWITCH, 0, 1},
	{"mute7",  KX_FXPARAM_SWITCH, 0, 1},
	{"mute8",  KX_FXPARAM_SWITCH, 0, 1},
	{"mute9",  KX_FXPARAM_SWITCH, 0, 1},
	{"mute10",  KX_FXPARAM_SWITCH, 0, 1},
	{"mute11",  KX_FXPARAM_SWITCH, 0, 1},
	{"mute12",  KX_FXPARAM_SWITCH, 0, 1},
	{"mute13",  KX_FXPARAM_SWITCH, 0, 1},
	{"mute14",  KX_FXPARAM_SWITCH, 0, 1},
	{"mute15",  KX_FXPARAM_SWITCH, 0, 1},
	{"mute16",  KX_FXPARAM_SWITCH, 0, 1},
	{"mute17",  KX_FXPARAM_SWITCH, 0, 1},
	{"mute18",  KX_FXPARAM_SWITCH, 0, 1},
	{"stlink1", KX_FXPARAM_SWITCH, 0, 1},
	{"stlink2", KX_FXPARAM_SWITCH, 0, 1},
	{"stlink3", KX_FXPARAM_SWITCH, 0, 1},
	{"stlink4", KX_FXPARAM_SWITCH, 0, 1},
	{"stlink5", KX_FXPARAM_SWITCH, 0, 1},
	{"stlink6", KX_FXPARAM_SWITCH, 0, 1},
	{"stlink7", KX_FXPARAM_SWITCH, 0, 1},
	{"stlink8", KX_FXPARAM_SWITCH, 0, 1},
	{"stlink9", KX_FXPARAM_SWITCH, 0, 1},
	{"vurange1", KX_FXPARAM_SELECTION, 0, 7},
	{"vurange2", KX_FXPARAM_SELECTION, 0, 7},
	{"vurange3", KX_FXPARAM_SELECTION, 0, 7},
	{"vurange4", KX_FXPARAM_SELECTION, 0, 7},
	{"vurange5", KX_FXPARAM_SELECTION, 0, 7},
	{"vurange6", KX_FXPARAM_SELECTION, 0, 7},
	{"vurange7", KX_FXPARAM_SELECTION, 0, 7},
	{"vurange8", KX_FXPARAM_SELECTION, 0, 7},
	{"vurange9", KX_FXPARAM_SELECTION, 0, 7},
	{"pan1", KX_FXPARAM_LEVEL, -10, 10},
	{"pan2", KX_FXPARAM_LEVEL, -10, 10},
	{"pan3", KX_FXPARAM_LEVEL, -10, 10},
	{"pan4", KX_FXPARAM_LEVEL, -10, 10},
	{"pan5", KX_FXPARAM_LEVEL, -10, 10},
	{"pan6", KX_FXPARAM_LEVEL, -10, 10},
	{"pan7", KX_FXPARAM_LEVEL, -10, 10},
	{"pan8", KX_FXPARAM_LEVEL, -10, 10},
	{"pan9", KX_FXPARAM_LEVEL, -10, 10},
	{"pan10", KX_FXPARAM_LEVEL, -10, 10},
	{"pan11", KX_FXPARAM_LEVEL, -10, 10},
	{"pan12", KX_FXPARAM_LEVEL, -10, 10},
	{"pan13", KX_FXPARAM_LEVEL, -10, 10},
	{"pan14", KX_FXPARAM_LEVEL, -10, 10},
	{"pan15", KX_FXPARAM_LEVEL, -10, 10},
	{"pan16", KX_FXPARAM_LEVEL, -10, 10},
	{"solo1", KX_FXPARAM_SWITCH, 0, 1},
	{"solo2", KX_FXPARAM_SWITCH, 0, 1},
	{"solo3", KX_FXPARAM_SWITCH, 0, 1},
	{"solo4", KX_FXPARAM_SWITCH, 0, 1},
	{"solo5", KX_FXPARAM_SWITCH, 0, 1},
	{"solo6", KX_FXPARAM_SWITCH, 0, 1},
	{"solo7", KX_FXPARAM_SWITCH, 0, 1},
	{"solo8", KX_FXPARAM_SWITCH, 0, 1},
	{"solo9", KX_FXPARAM_SWITCH, 0, 1},
	{"solo10", KX_FXPARAM_SWITCH, 0, 1},
	{"solo11", KX_FXPARAM_SWITCH, 0, 1},
	{"solo12", KX_FXPARAM_SWITCH, 0, 1},
	{"solo13", KX_FXPARAM_SWITCH, 0, 1},
	{"solo14", KX_FXPARAM_SWITCH, 0, 1},
	{"solo15", KX_FXPARAM_SWITCH, 0, 1},
	{"solo16", KX_FXPARAM_SWITCH, 0, 1},
	{"monomix", KX_FXPARAM_SWITCH, 0, 1},
	};

#define P (kxparam_t)

static kxparam_t presets[]=
{
NULL
};

#undef P

const kxparam_t *iMixyPlugin::get_plugin_presets()
{
return presets;
}

// Describe parameters
//------------------------
int iMixyPlugin::describe_param(int ndx, kx_fxparam_descr *descr)
{
	memcpy(descr, &_param_descr[ndx], sizeof kx_fxparam_descr); return 0;
}


/* Plugin Implemenation */
// get_param index  (ndx corresponds to 'logical' parameter id)
//-----------------------------------------------------------------------------
int iMixyPlugin::get_param(int ndx, kxparam_t *value)
{
	*value = _params[ndx]; 
	return 0;
}

// set_param (sets 'logical' values by writing to 'physical' registers)
//-------------------------------------------------------------------
int iMixyPlugin::set_param(int ndx, kxparam_t value)
{
	_params[ndx] = value;
	kxparam_t pan1=0;
	kxparam_t pan2=0;	
	int soloenabled=0;

	if(_params[70]==1||_params[71]==1||_params[72]==1||_params[73]==1
		||_params[74]==1||_params[75]==1||_params[76]==1||_params[77]==1
		||_params[78]==1||_params[79]==1||_params[80]==1||_params[81]==1
		||_params[82]==1||_params[83]==1||_params[84]==1||_params[85]==1) soloenabled=1;
	else soloenabled=0;

	if (ndx==0||ndx==1||ndx==18||ndx==19||ndx==54||ndx==55||ndx==36
		||ndx==70||ndx==71||ndx==72||ndx==73||ndx==74||ndx==75||ndx==76||ndx==77
		||ndx==78||ndx==79||ndx==80||ndx==81||ndx==82||ndx==83||ndx==84||ndx==85)
	{				
		value=(kxparam_t)(1000000*pow(10.0f,(float)_params[VOL1_ID]/20));
		if (_params[PAN1_ID]<=0)
		{			
			pan1=value;
			pan2=(value*(10+_params[PAN1_ID])/10);
		}
		else 
		{
			pan1=(value*(10-_params[PAN1_ID])/10);
			pan2=value;
		}
		if (_params[MUTE1_ID]==1||_params[VOL1_ID]<=-120||(soloenabled==1&&_params[SOLO1_ID]==0)){ value=0;pan1=0; pan2=0;}
		set_efx_register("pan1a", itodspr_scl(4000000, pan1));
		set_efx_register("pan2a", itodspr_scl(4000000, pan2));

		value=(kxparam_t)(1000000*pow(10.0f,(float)_params[VOL2_ID]/20));		
		if (_params[PAN2_ID]<=0)
		{			
			pan1=value;
			pan2=(value*(10+_params[PAN2_ID])/10);
		}
		else 
		{
			pan1=(value*(10-_params[PAN2_ID])/10);
			pan2=value;
		}
		if (_params[MUTE2_ID]==1||_params[VOL2_ID]<=-120||(soloenabled==1&&_params[SOLO2_ID]==0)){ value=0;pan1=0;pan2=0;}
		set_efx_register("pan1b", itodspr_scl(4000000, pan1));
		set_efx_register("pan2b", itodspr_scl(4000000, pan2));
	}

		//////		
	if (ndx==2||ndx==3||ndx==20||ndx==21||ndx==56||ndx==57||ndx==38
		||ndx==70||ndx==71||ndx==72||ndx==73||ndx==74||ndx==75||ndx==76||ndx==77
		||ndx==78||ndx==79||ndx==80||ndx==81||ndx==82||ndx==83||ndx==84||ndx==85)
	{				
		value=(int)(1000000*pow(10.0f,(float)_params[VOL3_ID]/20));
		if (_params[PAN3_ID]<=0)
		{			
			pan1=value;
			pan2=(value*(10+_params[PAN3_ID])/10);
		}
		else 
		{
			pan1=(value*(10-_params[PAN3_ID])/10);
			pan2=value;
		}
		if (_params[MUTE3_ID]==1||_params[VOL3_ID]<=-120||(soloenabled==1&&_params[SOLO3_ID]==0)){ value=0;pan1=0; pan2=0;}
		set_efx_register("pan3a", itodspr_scl(4000000, pan1));
		set_efx_register("pan4a", itodspr_scl(4000000, pan2));
				value=(int)(1000000*pow(10.0f,(float)_params[VOL4_ID]/20));		
		if (_params[PAN4_ID]<=0)
		{			
			pan1=value;
			pan2=value*(10+_params[PAN4_ID])/10 ;
		}
		else 
		{
			pan1=value*(10-_params[PAN4_ID])/10 ;
			pan2=value;
		}
		if (_params[MUTE4_ID]==1||_params[VOL4_ID]<=-120||(soloenabled==1&&_params[SOLO4_ID]==0)){ value=0;pan1=0;pan2=0;}
		set_efx_register("pan3b", itodspr_scl(4000000, pan1));
		set_efx_register("pan4b", itodspr_scl(4000000, pan2));
	}

		//////		
	if (ndx==4||ndx==5||ndx==22||ndx==23||ndx==58||ndx==59||ndx==40
		||ndx==70||ndx==71||ndx==72||ndx==73||ndx==74||ndx==75||ndx==76||ndx==77
		||ndx==78||ndx==79||ndx==80||ndx==81||ndx==82||ndx==83||ndx==84||ndx==85)
	{				
		value=(int)(1000000*pow(10.0f,(float)_params[VOL5_ID]/20));
		if (_params[PAN5_ID]<=0)
		{			
			pan1=value;
			pan2=value*(10+_params[PAN5_ID])/10 ;
		}
		else 
		{
			pan1=value*(10-_params[PAN5_ID])/10 ;
			pan2=value;
		}
		if (_params[MUTE5_ID]==1||_params[VOL5_ID]<=-120||(soloenabled==1&&_params[SOLO5_ID]==0)){ value=0;pan1=0; pan2=0;}
		set_efx_register("pan5a", itodspr_scl(4000000, pan1));
		set_efx_register("pan6a", itodspr_scl(4000000, pan2));
				value=(int)(1000000*pow(10.0f,(float)_params[VOL6_ID]/20));		
		if (_params[PAN6_ID]<=0)
		{			
			pan1=value;
			pan2=value*(10+_params[PAN6_ID])/10 ;
		}
		else 
		{
			pan1=value*(10-_params[PAN6_ID])/10 ;
			pan2=value;
		}
		if (_params[MUTE6_ID]==1||_params[VOL6_ID]<=-120||(soloenabled==1&&_params[SOLO6_ID]==0)){ value=0;pan1=0;pan2=0;}
		set_efx_register("pan5b", itodspr_scl(4000000, pan1));
		set_efx_register("pan6b", itodspr_scl(4000000, pan2));
	}

		//////
	if (ndx==6||ndx==7||ndx==24||ndx==25||ndx==60||ndx==61||ndx==42
		||ndx==70||ndx==71||ndx==72||ndx==73||ndx==74||ndx==75||ndx==76||ndx==77
		||ndx==78||ndx==79||ndx==80||ndx==81||ndx==82||ndx==83||ndx==84||ndx==85)
	{				
		value=(int)(1000000*pow(10.0f,(float)_params[VOL7_ID]/20));
		if (_params[PAN7_ID]<=0)
		{			
			pan1=value;
			pan2=value*(10+_params[PAN7_ID])/10 ;
		}
		else 
		{
			pan1=value*(10-_params[PAN7_ID])/10 ;
			pan2=value;
		}
		if (_params[MUTE7_ID]==1||_params[VOL7_ID]<=-120||(soloenabled==1&&_params[SOLO7_ID]==0)){ value=0;pan1=0; pan2=0;}
		set_efx_register("pan7a", itodspr_scl(4000000, pan1));
		set_efx_register("pan8a", itodspr_scl(4000000, pan2));
				value=(int)(1000000*pow(10.0f,(float)_params[VOL8_ID]/20));		
		if (_params[PAN8_ID]<=0)
		{			
			pan1=value;
			pan2=value*(10+_params[PAN8_ID])/10 ;
		}
		else 
		{
			pan1=value*(10-_params[PAN8_ID])/10 ;
			pan2=value;
		}
		if (_params[MUTE8_ID]==1||_params[VOL8_ID]<=-120||(soloenabled==1&&_params[SOLO8_ID]==0)){ value=0;pan1=0;pan2=0;}
		set_efx_register("pan7b", itodspr_scl(4000000, pan1));
		set_efx_register("pan8b", itodspr_scl(4000000, pan2));
	}
	
//////
	if (ndx==8||ndx==9||ndx==26||ndx==27||ndx==62||ndx==63||ndx==44
		||ndx==70||ndx==71||ndx==72||ndx==73||ndx==74||ndx==75||ndx==76||ndx==77
		||ndx==78||ndx==79||ndx==80||ndx==81||ndx==82||ndx==83||ndx==84||ndx==85)
	{				
		value=(int)(1000000*pow(10.0f,(float)_params[VOL9_ID]/20));
		if (_params[PAN9_ID]<=0)
		{			
			pan1=value;
			pan2=value*(10+_params[PAN9_ID])/10 ;
		}
		else 
		{
			pan1=value*(10-_params[PAN9_ID])/10 ;
			pan2=value;
		}
		if (_params[MUTE9_ID]==1||_params[VOL9_ID]<=-120||(soloenabled==1&&_params[SOLO9_ID]==0)){ value=0;pan1=0; pan2=0;}
		set_efx_register("pan9a", itodspr_scl(4000000, pan1));
		set_efx_register("pan10a", itodspr_scl(4000000, pan2));
				value=(int)(1000000*pow(10.0f,(float)_params[VOL10_ID]/20));		
		if (_params[PAN10_ID]<=0)
		{			
			pan1=value;
			pan2=value*(10+_params[PAN10_ID])/10 ;
		}
		else 
		{
			pan1=value*(10-_params[PAN10_ID])/10 ;
			pan2=value;
		}
		if (_params[MUTE10_ID]==1||_params[VOL10_ID]<=-120||(soloenabled==1&&_params[SOLO10_ID]==0)){ value=0;pan1=0;pan2=0;}
		set_efx_register("pan9b", itodspr_scl(4000000, pan1));
		set_efx_register("pan10b", itodspr_scl(4000000, pan2));
	}

/////////////////
	if (ndx==10||ndx==11||ndx==28||ndx==29||ndx==64||ndx==65||ndx==46
		||ndx==70||ndx==71||ndx==72||ndx==73||ndx==74||ndx==75||ndx==76||ndx==77
		||ndx==78||ndx==79||ndx==80||ndx==81||ndx==82||ndx==83||ndx==84||ndx==85)
	{				
		value=(int)(1000000*pow(10.0f,(float)_params[VOL11_ID]/20));
		if (_params[PAN11_ID]<=0)
		{			
			pan1=value;
			pan2=value*(10+_params[PAN11_ID])/10 ;
		}
		else 
		{
			pan1=value*(10-_params[PAN11_ID])/10 ;
			pan2=value;
		}
		if (_params[MUTE11_ID]==1||_params[VOL11_ID]<=-120||(soloenabled==1&&_params[SOLO11_ID]==0)){ value=0;pan1=0; pan2=0;}
		set_efx_register("pan11a", itodspr_scl(4000000, pan1));
		set_efx_register("pan12a", itodspr_scl(4000000, pan2));
				value=(int)(1000000*pow(10.0f,(float)_params[VOL12_ID]/20));		
		if (_params[PAN12_ID]<=0)
		{			
			pan1=value;
			pan2=value*(10+_params[PAN12_ID])/10 ;
		}
		else 
		{
			pan1=value*(10-_params[PAN12_ID])/10 ;
			pan2=value;
		}
		if (_params[MUTE12_ID]==1||_params[VOL12_ID]<=-120||(soloenabled==1&&_params[SOLO12_ID]==0)){ value=0;pan1=0;pan2=0;}
		set_efx_register("pan11b", itodspr_scl(4000000, pan1));
		set_efx_register("pan12b", itodspr_scl(4000000, pan2));
	}
	
//////////////////////	
	if (ndx==12||ndx==13||ndx==30||ndx==31||ndx==66||ndx==67||ndx==48
		||ndx==70||ndx==71||ndx==72||ndx==73||ndx==74||ndx==75||ndx==76||ndx==77
		||ndx==78||ndx==79||ndx==80||ndx==81||ndx==82||ndx==83||ndx==84||ndx==85)
	{				
		value=(int)(1000000*pow(10.0f,(float)_params[VOL13_ID]/20));
		if (_params[PAN13_ID]<=0)
		{			
			pan1=value;
			pan2=value*(10+_params[PAN13_ID])/10 ;
		}
		else 
		{
			pan1=value*(10-_params[PAN13_ID])/10 ;
			pan2=value;
		}
		if (_params[MUTE13_ID]==1||_params[VOL13_ID]<=-120||(soloenabled==1&&_params[SOLO13_ID]==0)){ value=0;pan1=0; pan2=0;}
		set_efx_register("pan13a", itodspr_scl(4000000, pan1));
		set_efx_register("pan14a", itodspr_scl(4000000, pan2));
				value=(int)(1000000*pow(10.0f,(float)_params[VOL14_ID]/20));		
		if (_params[PAN14_ID]<=0)
		{			
			pan1=value;
			pan2=value*(10+_params[PAN14_ID])/10 ;
		}
		else 
		{
			pan1=value*(10-_params[PAN14_ID])/10 ;
			pan2=value;
		}
		if (_params[MUTE14_ID]==1||_params[VOL14_ID]<=-120||(soloenabled==1&&_params[SOLO14_ID]==0)){ value=0;pan1=0;pan2=0;}
		set_efx_register("pan13b", itodspr_scl(4000000, pan1));
		set_efx_register("pan14b", itodspr_scl(4000000, pan2));
	}


/////////////////////////////////
	if (ndx==14||ndx==15||ndx==32||ndx==33||ndx==68||ndx==69||ndx==50
		||ndx==70||ndx==71||ndx==72||ndx==73||ndx==74||ndx==75||ndx==76||ndx==77
		||ndx==78||ndx==79||ndx==80||ndx==81||ndx==82||ndx==83||ndx==84||ndx==85)
	{				
		value=(int)(1000000*pow(10.0f,(float)_params[VOL15_ID]/20));
		if (_params[PAN15_ID]<=0)
		{			
			pan1=value;
			pan2=value*(10+_params[PAN15_ID])/10 ;
		}
		else 
		{
			pan1=value*(10-_params[PAN15_ID])/10 ;
			pan2=value;
		}
		if (_params[MUTE15_ID]==1||_params[VOL15_ID]<=-120||(soloenabled==1&&_params[SOLO15_ID]==0)){ value=0;pan1=0; pan2=0;}
		set_efx_register("pan15a", itodspr_scl(4000000, pan1));
		set_efx_register("pan16a", itodspr_scl(4000000, pan2));
				value=(int)(1000000*pow(10.0f,(float)_params[VOL16_ID]/20));		
		if (_params[PAN16_ID]<=0)
		{			
			pan1=value;
			pan2=value*(10+_params[PAN16_ID])/10 ;
		}
		else 
		{
			pan1=value*(10-_params[PAN16_ID])/10 ;
			pan2=value;
		}
		if (_params[MUTE16_ID]==1||_params[VOL16_ID]<=-120||(soloenabled==1&&_params[SOLO16_ID]==0)){ value=0;pan1=0;pan2=0;}
		set_efx_register("pan15b", itodspr_scl(4000000, pan1));
		set_efx_register("pan16b", itodspr_scl(4000000, pan2));
	}

//////////////////////Master
	if (ndx==16||ndx==17||ndx==34||ndx==35||ndx==44||ndx==86)		

	{		
		value=(int)(1000000*pow(10.0f,(float)_params[VOL17_ID]/20));
		if (_params[86]==0)
		{			
			pan1=value;
			pan2=0 ;
		}
		else 
		{
			pan1=value/2 ;
			pan2=value/2;
		}
		if (_params[MUTE17_ID]==1||_params[VOL17_ID]<=-120){ value=0;pan1=0;pan2=0;}	
		set_efx_register("pan17a", itodspr_scl(4000000, pan1));
		set_efx_register("pan18a", itodspr_scl(4000000, pan2));

		value=(int)(1000000*pow(10.0f,(float)_params[VOL18_ID]/20));		
		
		if (_params[86]==0)
		{			
			pan1=0;
			pan2=value ;
		}
		else 
		{
			pan1=value/2 ;
			pan2=value/2;
		}
		if (_params[MUTE18_ID]==1||_params[VOL18_ID]<=-120){ value=0;pan1=0;pan2=0;}
		set_efx_register("pan17b", itodspr_scl(4000000, pan1));
		set_efx_register("pan18b", itodspr_scl(4000000, pan2));
	}


	// sync the GUI part, if set_param was called by the mixer or kX Automation
	if (cp) ((iMixyPluginDlg*) cp)->sync(ndx);

	return 0;
}

// Set all params to defaults
//-------------------------------------------
int iMixyPlugin::set_defaults() 
{
	set_all_params(default_params); 
	str_param[0][0]=0; 
	for (int i=0; i<9; i++)	 
	{	 		
		if (i<8) sprintf(str_param[i], "Ch %i/%i" , 2*i+1, 2*i+2);else sprintf(str_param[i], "Master" );
	}
	return 0;
}

// Set all params during runtime
//--------------------------------------------------
int iMixyPlugin::set_all_params(kxparam_t* values)
{
	for (int i = 0; i < MIXY_PARAMS_COUNT; i++) set_param(i, values[i]);
	return 0;
}

// publish DSP Microcode
//----------------------------------------------------
int iMixyPlugin::request_microcode() 
{
	publish_microcode(mixy);

	return 0;
}

// get plugins descriotion
const char *iMixyPlugin::get_plugin_description(int id)
{
	plugin_description(mixy);
}

// Create own Control panel (cp)
//--------------------------------------------
iKXPluginGUI *iMixyPlugin::create_cp(kDialog *parent, kFile *mf)
{
	iMixyPluginDlg *tmp;
	tmp = new iMixyPluginDlg(this, parent,mf);
	tmp->create();
	return tmp;
}
int iMixyPlugin::get_user_interface()
{
	return IKXPLUGIN_OWNER; // means 'custom' GUI panel
}


//--------------------------------------------------------------
BEGIN_MESSAGE_MAP(iMixyPluginDlg, CKXPluginGUI)
ON_CONTROL(EN_UPDATE,MIXY_PLG_EDIT1,editor_change1)
ON_CONTROL(EN_UPDATE,MIXY_PLG_EDIT2,editor_change2)
ON_CONTROL(EN_UPDATE,MIXY_PLG_EDIT3,editor_change3)
ON_CONTROL(EN_UPDATE,MIXY_PLG_EDIT4,editor_change4)
ON_CONTROL(EN_UPDATE,MIXY_PLG_EDIT5,editor_change5)
ON_CONTROL(EN_UPDATE,MIXY_PLG_EDIT6,editor_change6)
ON_CONTROL(EN_UPDATE,MIXY_PLG_EDIT7,editor_change7)
ON_CONTROL(EN_UPDATE,MIXY_PLG_EDIT8,editor_change8)
ON_CONTROL(EN_UPDATE,MIXY_PLG_EDIT9,editor_change9)
//ON_CONTROL_RANGE(EN_UPDATE, MIXY_PLG_EDIT1, MIXY_PLG_EDIT5, edit_changed)
ON_WM_TIMER()
END_MESSAGE_MAP()

iMixyPluginDlg::iMixyPluginDlg(iMixyPlugin *plg, kDialog *parent_,kFile *mf_) 
 : CKXPluginGUI(parent_,mf_) 
{	  
	plugin=plg;          
	timer_id=(UINT_PTR)-1;
    mode=1;
	for (int i=0; i<9; i++)
	{
		max_l[i]=-120;
		max_r[i]=-120;
		vu[i]=NULL;
	}

          // defaults: found in the skin file, too
          
	pm_offset=2;          
	pm_width=270;          
	pm_min=-120.4f;          
	pm_max=0.0f;
    pm_voffset=2;          
	pm_height=271;

    for (int i=0; i<9; i++)
	{	
		switch(plugin->_params[i+45])	
		{
	 
		case 0 :	 
			pm_vmin[i]=-24.0f;     
			pm_vmax[i]=0.0f;	 
			break;	 
		case 1 :	 
			pm_vmin[i]=-60.0f;     
			pm_vmax[i]=0.0f;	 
			break;	 
		case 2 :	 
			pm_vmin[i]=-90.0f;     
			pm_vmax[i]=0.0f;	 
			break;	 
		case 3 :	 
			pm_vmin[i]=-120.0f;     
			pm_vmax[i]=0.0f;	 
			break;	 
		case 4 :	 
			pm_vmin[i]=-36.0f;     
			pm_vmax[i]=12.0f;	 
			break;	 
		case 5 :	 
			pm_vmin[i]=-72.0f;     
			pm_vmax[i]=12.0f;	 
			break;	 
		case 6 :	 
			pm_vmin[i]=-102.0f;     
			pm_vmax[i]=12.0f;	 
			break;	 
		case 7 :	 
			pm_vmin[i]=-132.0f;     
			pm_vmax[i]=12.0f;	 
			break;	
		}	
	}
}

iMixyPluginDlg::~iMixyPluginDlg()
{	
	for (int i=0;i<9;i++)	
	{			
		if(vu[i])		
		{		 
			vu[i]->destroy();		 
			delete vu[i];		 
			vu[i]=0;		
		}
         	turn_on(0);	
	} 
}

// On Vu dialog
int iMixyPluginDlg::turn_on(int what)
{ 
	if((what==1) && (timer_id==(UINT_PTR)-1)) 
	{  
		timer_id=SetTimer(4321+plugin->pgm_id,50,NULL);  
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

// Plugin GUI init
//----------------------------------------------------------------------
void iMixyPluginDlg::init()
{
	
	CKXPluginGUI::init();
	smallfont.CreatePointFont(70,"Tahoma"); // creates a small font	
	// custom dialog window  (title, Width, Height)
	create_dialog(plugin_name(mixy), 980, 460);

	for(int i=1; i<10; i++)
	{
		vu[-1+i]=new kPeak;

	// load  bitmaps here 
		switch(plugin->_params[44+i])	
		{	 
		case 0 :	 
			vu[-1+i]->set_bitmaps(mf.load_image(("my0-24.jpg")),mf.load_image(("my0-24h.jpg")));	 
			break;	 
		case 1 :	 
			vu[-1+i]->set_bitmaps(mf.load_image(("my0-60.jpg")),mf.load_image(("my0-60h.jpg")));	 
			break;	 
		case 2 :	 
			vu[-1+i]->set_bitmaps(mf.load_image(("my0-90.jpg")),mf.load_image(("my0-90h.jpg")));	 
			break;	 
		case 3 :	 
			vu[-1+i]->set_bitmaps(mf.load_image(("my0-120.jpg")),mf.load_image(("my0-120h.jpg")));	 
			break;	 
		case 4 :	 
			vu[-1+i]->set_bitmaps(mf.load_image(("my12-24.jpg")),mf.load_image(("my12-24h.jpg")));	 
			break;	 
		case 5 :	 
			vu[-1+i]->set_bitmaps(mf.load_image(("my12-60.jpg")),mf.load_image(("my12-60h.jpg")));	 
			break;	 
		case 6 :	 
			vu[-1+i]->set_bitmaps(mf.load_image(("my12-90.jpg")),mf.load_image(("my12-90h.jpg")));	 
			break;	 
		case 7 :	 
			vu[-1+i]->set_bitmaps(mf.load_image(("my12-120.jpg")),mf.load_image(("my12-120h.jpg")));	 
			break;	
		}	
	}
	char d[7];
	vutop=75;
	for (int i=1; i<10; i++)
	{
		// show VU meter name,left, top,    mode  		
		vuleft[i-1]=50+105*(i-1); 	
		vu[-1+i]->create(mixy_name,  vuleft[i-1],  vutop, this, 1);	
		vu[-1+i]->show();

		// show VU labels; the ID should be >PARAM_COUNT  left, top, width	
		create_label(vutextl[i-1],MIXY_PARAMS_COUNT+1,"      ",vuleft[i-1]-35,vutop-40,50);	
		vutextl[i-1].ModifyStyle(SS_LEFT,SS_RIGHT,0);
		vutextl[i-1].SetFont(&smallfont,NULL);
		create_label(vutextr[i-1],MIXY_PARAMS_COUNT+1,"      ",vuleft[i-1]+20,vutop-40,50);
		vutextr[i-1].SetFont(&smallfont,NULL);

		// SHOW_VFADER(Fader, Ctrl_ID, Label, MinVal,MaxVal,Left, Top, Width, Height)	//	
		if (i<9) sprintf(d, "Ch%i" , 2*i-1); else sprintf(d, "M1" );
		create_vslider(fdrvol[2*i-1],	2*i-2, d,  -120, 12,	vuleft[i-1]-31,	vutop-31,	25, 290);
		fdrvol[2*i-1].ModifyStyle( TBS_AUTOTICKS,TBS_NOTICKS,0);

		if (i<9) sprintf(d, "Ch%i" , 2*i); else sprintf(d, "M2" );	
		create_vslider(fdrvol[2*i],	2*i-1, d,  -120, 12,	vuleft[i-1]+40,	vutop-30,	25, 290);
		fdrvol[2*i].ModifyStyle( TBS_AUTOTICKS|TBS_RIGHT,TBS_NOTICKS|TBS_LEFT,0);
		// show fdr labels; the ID should be >PARAM_COUNT  left, top, width	
		create_label(fdrtext[2*i-2],MIXY_PARAMS_COUNT+1,"      ",vuleft[i-1]-27,vutop+vu[-1+i]->get_peak_height()-10,50);	
		fdrtext[2*i-2].SetFont(&smallfont,NULL);

		create_label(fdrtext[2*i-1],MIXY_PARAMS_COUNT+1,"      ",vuleft[i-1]+10,vutop+vu[-1+i]->get_peak_height()-10,50);	
		fdrtext[2*i-1].ModifyStyle(SS_LEFT,SS_RIGHT,0);
		fdrtext[2*i-1].SetFont(&smallfont,NULL);

		// SHOW pan sliders(Fader, Ctrl_ID, Label, MinVal,MaxVal,Left, Top, Width, Height)	//	
		if (i<9)
		{ 
			sprintf(d, "Pan %i" , 2*i-1); 
			create_hslider(pan[2*i-2],	2*i+52, d,  -10, 10,	vuleft[i-1]-35,	vutop+vu[-1+i]->get_peak_height()+10,	50, 20);
			pan[2*i-2].ModifyStyle( TBS_AUTOTICKS,TBS_NOTICKS,0);			
			sprintf(d, "Pan %i" , 2*i); 
			create_hslider(pan[2*i-1],	2*i+53, d,  -10, 10,	vuleft[i-1]+15,	vutop+vu[-1+i]->get_peak_height()+10,	50, 20);
			pan[2*i-1].ModifyStyle( TBS_AUTOTICKS,TBS_NOTICKS,0);
			
			create_checkbox(solo[2*i-2], 2*i+68, "Solo", vuleft[i-1]-30, vutop+vu[-1+i]->get_peak_height()+30, 50);			
			solo[2*i-2].SetFont(&smallfont,NULL);
			create_checkbox(solo[2*i-1], 2*i+69, "Solo", vuleft[i-1]+20, vutop+vu[-1+i]->get_peak_height()+30, 50);
			solo[2*i-1].SetFont(&smallfont,NULL);
		}

		//(CheckBox, Ctrl_ID, Caption, Left, Top, Width)	
		create_checkbox(mute[2*i-1], 2*i+16, "Mute", vuleft[i-1]-30, vutop+vu[-1+i]->get_peak_height()+50, 50);
		mute[2*i-1].SetFont(&smallfont,NULL);
		create_checkbox(mute[2*i], 2*i+17, "Mute", vuleft[i-1]+20, vutop+vu[-1+i]->get_peak_height()+50, 50);
		mute[2*i].SetFont(&smallfont,NULL);
		create_checkbox(stlink[i-1], i+35, "Stereo Link", vuleft[i-1]-20, vutop+vu[-1+i]->get_peak_height()+70, 80);
		stlink[i-1].SetFont(&smallfont,NULL);

			// edit text boxes
		edit[i-1] = new CEdit;
		edit[i-1]->Create(WS_CHILD|WS_VISIBLE|WS_BORDER|SS_CENTER,CRect(vuleft[i-1]-20, vutop+390, vuleft[i-1]+61, vutop+405), this, 0x10001+i-1);		
		edit[i-1]->SetFont(&smallfont,NULL);
		//edit[i-1]->SetWindowText(plugin->str_param[i-1]);
		//sprintf(data[i-1],plugin->str_param);
	}

		create_checkbox(monomix, 86, "Mono Mix", vuleft[8]-30, vutop+vu[4]->get_peak_height()+30, 80);
		monomix.SetFont(&smallfont,NULL);

	// show reset button left, top, width
	create_button(p_reset[0],P_RESET1,	"reset", vuleft[0]-5, vutop-58,vu[0]->get_peak_width());	
	create_button(p_reset[1],P_RESET2,	"reset", vuleft[1]-5, vutop-58,vu[0]->get_peak_width());
	create_button(p_reset[2],P_RESET3,	"reset", vuleft[2]-5, vutop-58,vu[0]->get_peak_width());
	create_button(p_reset[3],P_RESET4,	"reset", vuleft[3]-5, vutop-58,vu[0]->get_peak_width());
	create_button(p_reset[4],P_RESET5,	"reset", vuleft[4]-5, vutop-58,vu[0]->get_peak_width());
	create_button(p_reset[5],P_RESET6,	"reset", vuleft[5]-5, vutop-58,vu[0]->get_peak_width());	
	create_button(p_reset[6],P_RESET7,	"reset", vuleft[6]-5, vutop-58,vu[0]->get_peak_width());
	create_button(p_reset[7],P_RESET8,	"reset", vuleft[7]-5, vutop-58,vu[0]->get_peak_width());
	create_button(p_reset[8],P_RESET9,	"reset", vuleft[8]-5, vutop-58,vu[0]->get_peak_width());
	for (int i=0;i<9;i++)
	{
		p_reset[i].SetFont(&smallfont,NULL);
	}
	//image.create("",0,0, mf.load_image("my12-120.jpg"), this, kMETHOD_TRANS);
	//image.show();

	turn_on(1);
	redraw();
	controls_enabled = 1; // enable updating GUI when the parameters are changed
	sync(EVERYPM_ID); // update all 'GUI' elements to the preset plugin parameters
}



// Sync any labels with the paramaters, update all controls
//----------------------------------------------------------
void iMixyPluginDlg::sync(int ndx)
{
	char c[MAX_LABEL_STRING];
	kxparam_t v; 
	int i;

	if (ndx==EVERYPM_ID) // update all controls when plugin is opened 
	{
	
		for (i=0; i<18; i=i+2)
		{			
			v = plugin->_params[i];
			if (controls_enabled) {VSSETPOS(fdrvol[i+1], (int)v); fdrvol[i+1].redraw();}		
			sprintf(c, "%idB" , v);	
			fdrtext[i].SetWindowText(c);

			v = plugin->_params[i+1];		
			if (controls_enabled) {VSSETPOS(fdrvol[i+2], (int)v); fdrvol[i+2].redraw();}		
			sprintf(c, "%idB" , v);		
			fdrtext[i+1].SetWindowText(c);		
			
			if (controls_enabled) mute[i+1].set_check((int)plugin->_params[i+18]);		

			if (controls_enabled) mute[i+2].set_check((int)plugin->_params[i+19]);			
			
			stlink[i/2].set_check((int)plugin->_params[i/2+36]);	
			
			edit[i/2]->SetWindowText(plugin->str_param[i/2]);
			
			if (i<16)
			{			
				v = -1*(plugin->_params[54+i]);				
				if (controls_enabled) {VSSETPOS(pan[i], (int)v); pan[i].redraw();}
			
				v = -1*(plugin->_params[55+i]);				
				if (controls_enabled) {VSSETPOS(pan[i+1], (int)v); pan[i+1].redraw();}						
			
				if (controls_enabled) solo[i].set_check((int)plugin->_params[i+70]);
			
				if (controls_enabled) solo[i+1].set_check((int)plugin->_params[i+71]);			
			}
		}
		if (controls_enabled) monomix.set_check((int)plugin->_params[86]);
	}

	//sync on runtime
	for (i=0; i<18; i=i+2)
	{
		if (ndx==i)
		{
			v = plugin->_params[i];		
			if (controls_enabled) {VSSETPOS(fdrvol[i+1], (int)v); fdrvol[i+1].redraw();}		
			if (v>-120) sprintf(c, "%idB" , v);else	sprintf(c, "-Inf" , v);
			fdrtext[i].SetWindowText(c);
			if (plugin->_params[i/2+36]==1)
			{
				if ((plugin->_params[i])!=(plugin->_params[i+1]))
				{
					VSSETPOS(fdrvol[i+2], (int)v);
					fdrvol[i+2].redraw();
					plugin->_params[i+1] = v; //set left to right value
					plugin->set_param(i+1,v);
				}
			}							
			if (ndx != EVERYPM_ID) break;
		}
		if (ndx==i+1)
		{
			v = plugin->_params[i+1];		
			if (controls_enabled) {VSSETPOS(fdrvol[i+2], (int)v); fdrvol[i+2].redraw();}		
			if (v>-120) sprintf(c, "%idB" , v);else	sprintf(c, "-Inf" , v);	
			fdrtext[i+1].SetWindowText(c);
			if (plugin->_params[i/2+36]==1)		
			{			
				if ((plugin->_params[i])!=(plugin->_params[i+1]))			
				{				
					VSSETPOS(fdrvol[i+1], (int)v);			
					fdrvol[i+1].redraw();				
					plugin->_params[i] = v; //set right to left value				
					plugin->set_param(i,(int)v);	
				}		
			}
			if (ndx != EVERYPM_ID) break;
		}
		if (ndx==i+18)
		{	
			if (controls_enabled) mute[i+1].set_check((int)plugin->_params[i+18]);
			if (plugin->_params[i/2+36]==1 && plugin->_params[i+18]!=plugin->_params[i+19])
			{	
				mute[i+2].set_check((int)plugin->_params[i+18]);
				plugin->_params[i+19] = plugin->_params[i+18]; //set left to right value
				plugin->set_param(i+19,plugin->_params[i+18]);
			}
			if (ndx != EVERYPM_ID) break;
		}		
		if (ndx==i+19)
		{
			if (controls_enabled) mute[i+2].set_check((int)plugin->_params[i+19]);
			if (plugin->_params[i/2+36]==1 && plugin->_params[i+18]!=plugin->_params[i+19])	
			{
				mute[i+1].set_check((int)plugin->_params[i+19]);	
				plugin->_params[i+18] = plugin->_params[i+19]; //set left to right value
				plugin->set_param(i+18,plugin->_params[i+19]);
			}
			if (ndx != EVERYPM_ID) break;
		}		
		
		if (ndx==i+36)
		{
			stlink[i/2].set_check((int)plugin->_params[i/2+36]);			
			if (ndx != EVERYPM_ID) break;
		}
		
		if (i<16)			
		{		
			if (ndx==i+54)		
			{
				v = (-1)*plugin->_params[i+54];				
				if (controls_enabled) {VSSETPOS(pan[i], (int)v); pan[i].redraw();}			
				if (ndx != EVERYPM_ID) break;		
			}		
			if (ndx==i+55)				
			{
				v = (-1)*plugin->_params[i+55];				
				if (controls_enabled) {VSSETPOS(pan[i+1], (int)v); pan[i+1].redraw();}			
				if (ndx != EVERYPM_ID) break;		
			}		
			if (ndx==i+70)				
			{
				if (controls_enabled) solo[i].set_check((int)plugin->_params[i+70]);			
				if (plugin->_params[i/2+36]==1 && plugin->_params[i+70]!=plugin->_params[i+71])			
				{	
					solo[i+1].set_check((int)plugin->_params[i+70]);				
					plugin->_params[i+71] = plugin->_params[i+70]; //set left to right value				
					plugin->set_param(i+71,plugin->_params[i+70]);			
				}
				if (ndx != EVERYPM_ID) break;				
			}								
			if (ndx==i+71)				
			{
				if (controls_enabled) solo[i+1].set_check((int)plugin->_params[i+71]);			
				if (plugin->_params[i/2+36]==1 && plugin->_params[i+70]!=plugin->_params[i+71])			
				{	
					solo[i].set_check((int)plugin->_params[i+71]);				
					plugin->_params[i+70] = plugin->_params[i+71]; //set left to right value				
					plugin->set_param(i+70,plugin->_params[i+71]);			
				}
				if (ndx != EVERYPM_ID) break;				
			}			
		}		
	}	

	if (ndx==86)
		{
			if (controls_enabled) monomix.set_check((int)plugin->_params[86]);
			if (plugin->_params[86]==1)
			{
				stlink[8].set_check(1);
				plugin->_params[44] = 1;		
				plugin->set_param(44,1);
				stlink[8].EnableWindow(FALSE);
			}
			else stlink[8].EnableWindow(TRUE);
		}
}

// VU onTimer Recalc 
//--------------------------------------------------------------------------------------------------
void iMixyPluginDlg::OnTimer(UINT_PTR)
{        
	dword ch1,ch2,ch3,ch4,ch5,ch6,ch7,ch8,ch9,ch10,ch11,ch12,ch13,ch14,ch15,ch16,ch17,ch18;        
	plugin->get_efx_register("result_1",&ch1);        
	plugin->get_efx_register("result_2",&ch2);        
	plugin->set_efx_register("result_1",0);        
	plugin->set_efx_register("result_2",0);        
	plugin->get_efx_register("result_3",&ch3);        
	plugin->get_efx_register("result_4",&ch4);        
	plugin->set_efx_register("result_3",0);        
	plugin->set_efx_register("result_4",0);        
	plugin->get_efx_register("result_5",&ch5);        
	plugin->get_efx_register("result_6",&ch6);        
	plugin->set_efx_register("result_5",0);        
	plugin->set_efx_register("result_6",0);        
	plugin->get_efx_register("result_7",&ch7);        
	plugin->get_efx_register("result_8",&ch8);        
	plugin->set_efx_register("result_7",0);        
	plugin->set_efx_register("result_8",0);        
	plugin->get_efx_register("result_9",&ch9);        
	plugin->get_efx_register("result_10",&ch10);        
	plugin->set_efx_register("result_9",0);        
	plugin->set_efx_register("result_10",0);      
	plugin->get_efx_register("result_11",&ch11);        
	plugin->get_efx_register("result_12",&ch12);        
	plugin->set_efx_register("result_11",0);        
	plugin->set_efx_register("result_12",0);        
	plugin->get_efx_register("result_13",&ch13);        
	plugin->get_efx_register("result_14",&ch14);        
	plugin->set_efx_register("result_13",0);        
	plugin->set_efx_register("result_14",0);        
	plugin->get_efx_register("result_15",&ch15);        
	plugin->get_efx_register("result_16",&ch16);        
	plugin->set_efx_register("result_15",0);        
	plugin->set_efx_register("result_16",0);        
	plugin->get_efx_register("result_17",&ch17);        
	plugin->get_efx_register("result_18",&ch18);        
	plugin->set_efx_register("result_17",0);        
	plugin->set_efx_register("result_18",0); 
        
#define UNITY_GAIN_COEFF 0x78000000L // level is 0.25        
#define MAGIC_SHIFT    24		
	float left_f[9], right_f[9];        
	char tmp1[32];        
	char tmp2[32];
	CString tmp;
		
	left_f[0] = ((float)(long)(ch1-0x78000000L)/16777216.0f+(float)(long)(ch1-0x78000000L)/33554432.0f);        
	right_f[0] = ((float)(long)(ch2-0x78000000L)/16777216.0f+(float)(long)(ch2-0x78000000L)/33554432.0f);		
	left_f[1] = ((float)(long)(ch3-0x78000000L)/16777216.0f+(float)(long)(ch3-0x78000000L)/33554432.0f);        
	right_f[1] = ((float)(long)(ch4-0x78000000L)/16777216.0f+(float)(long)(ch4-0x78000000L)/33554432.0f);		
	left_f[2] = ((float)(long)(ch5-0x78000000L)/16777216.0f+(float)(long)(ch5-0x78000000L)/33554432.0f);        
	right_f[2] = ((float)(long)(ch6-0x78000000L)/16777216.0f+(float)(long)(ch6-0x78000000L)/33554432.0f);		
	left_f[3] = ((float)(long)(ch7-0x78000000L)/16777216.0f+(float)(long)(ch7-0x78000000L)/33554432.0f);        
	right_f[3] = ((float)(long)(ch8-0x78000000L)/16777216.0f+(float)(long)(ch8-0x78000000L)/33554432.0f);		
	left_f[4] = ((float)(long)(ch9-0x78000000L)/16777216.0f+(float)(long)(ch9-0x78000000L)/33554432.0f);        
	right_f[4] = ((float)(long)(ch10-0x78000000L)/16777216.0f+(float)(long)(ch10-0x78000000L)/33554432.0f);
	left_f[5] = ((float)(long)(ch11-0x78000000L)/16777216.0f+(float)(long)(ch11-0x78000000L)/33554432.0f);        
	right_f[5] = ((float)(long)(ch12-0x78000000L)/16777216.0f+(float)(long)(ch12-0x78000000L)/33554432.0f);		
	left_f[6] = ((float)(long)(ch13-0x78000000L)/16777216.0f+(float)(long)(ch13-0x78000000L)/33554432.0f);        
	right_f[6] = ((float)(long)(ch14-0x78000000L)/16777216.0f+(float)(long)(ch14-0x78000000L)/33554432.0f);		
	left_f[7] = ((float)(long)(ch15-0x78000000L)/16777216.0f+(float)(long)(ch15-0x78000000L)/33554432.0f);        
	right_f[7] = ((float)(long)(ch16-0x78000000L)/16777216.0f+(float)(long)(ch16-0x78000000L)/33554432.0f);		
	left_f[8] = ((float)(long)(ch17-0x78000000L)/16777216.0f+(float)(long)(ch17-0x78000000L)/33554432.0f);        
	right_f[8] = ((float)(long)(ch18-0x78000000L)/16777216.0f+(float)(long)(ch18-0x78000000L)/33554432.0f);
		
	for (int i=0; i<9; i++)		
	{        
		if(left_f[i]<-120.0) left_f[i]=-120.0; // cannot use pm_min, since '-120.0dB' is used elsewhere
        if(right_f[i]<-120.0) right_f[i]=-120.0;

        if(left_f[i]>pm_vmax[i]) left_f[i]=pm_vmax[i];
        if(right_f[i]>pm_vmax[i]) right_f[i]=pm_vmax[i];

        if(left_f[i]>max_l[i]) max_l[i]=left_f[i];        
		if(right_f[i]>max_r[i]) max_r[i]=right_f[i];

		// We'll just do horizontal here	//

		// VU text output
		// VU text output
		vutextl[i].GetWindowText(tmp);
        if(max_l[i]!=-120) sprintf(tmp1,"%03.1fdB",max_l[i]);        
		else sprintf(tmp1,"-Inf "); 
		if(tmp!=tmp1) vutextl[i].SetWindowText(tmp1);

       	vutextr[i].GetWindowText(tmp);
		if(max_r[i]!=-120) sprintf(tmp2,"%03.1fdB",max_r[i]);        
		else sprintf(tmp2,"-Inf ");
		if(tmp!=tmp2) vutextr[i].SetWindowText(tmp2);

		int separator_l=pm_voffset+(int) ((float)pm_height*((left_f[i]-pm_vmax[i]))/pm_vmin[i]);
        int separator_r=pm_voffset+(int) ((float)pm_height*((right_f[i]-pm_vmax[i]))/pm_vmin[i]);
        
		vu[i]->set_value(separator_l,separator_r,0,0);
        vu[i]->redraw();		
	}
}

//---------------------------------------------
void iMixyPluginDlg::on_destroy()
{
	turn_on(0);
}

//----------------------------------------

int iMixyPlugin::save_plugin_settings(kSettings &cfg)
{

 if(pgm_id>0)
 {
  char key_name[32];
  char kname[10];
  sprintf(key_name,"pgm_%d",pgm_id);

  for (int i=0; i<9; i++)	 
	{	 		
		sprintf(kname,"chnames%d", i);		
		cfg.write(key_name, kname, str_param[i]);
	}
 }
	
return 0;
}


int iMixyPlugin::load_plugin_settings(kSettings &cfg)
{
	if(pgm_id>0)
	{			
		char key_name[32];		
		char kname[10];
		sprintf(key_name,"pgm_%d",pgm_id);		  
		for (int i=0; i<9; i++)	 			
		{		
			sprintf(kname,"chnames%d", i);		
			cfg.read(key_name, kname, str_param[i], sizeof(str_param[i]));		
		}		
	}	
	else  
		str_param[0][0]=0;
	return 0;
}

void iMixyPluginDlg::editor_change1(void)
{  
	CString tmp;
	edit[0]->GetWindowText(tmp);
	strncpy(plugin->str_param[0],tmp,sizeof(plugin->str_param[0]));

}
void iMixyPluginDlg::editor_change2(void)
{  
	CString tmp;
	edit[1]->GetWindowText(tmp);
	strncpy(plugin->str_param[1],tmp,sizeof(plugin->str_param[1]));
}
void iMixyPluginDlg::editor_change3(void)
{  
	CString tmp;
	edit[2]->GetWindowText(tmp);
	strncpy(plugin->str_param[2],tmp,sizeof(plugin->str_param[2]));
}
void iMixyPluginDlg::editor_change4(void)
{  
	CString tmp;
	edit[3]->GetWindowText(tmp);
	strncpy(plugin->str_param[3],tmp,sizeof(plugin->str_param[3]));
}
void iMixyPluginDlg::editor_change5(void)
{  
	CString tmp;
	edit[4]->GetWindowText(tmp);
	strncpy(plugin->str_param[4],tmp,sizeof(plugin->str_param[4]));

}
void iMixyPluginDlg::editor_change6(void)
{  
	CString tmp;
	edit[5]->GetWindowText(tmp);
	strncpy(plugin->str_param[5],tmp,sizeof(plugin->str_param[5]));
}
void iMixyPluginDlg::editor_change7(void)
{  
	CString tmp;
	edit[6]->GetWindowText(tmp);
	strncpy(plugin->str_param[6],tmp,sizeof(plugin->str_param[6]));
}
void iMixyPluginDlg::editor_change8(void)
{  
	CString tmp;
	edit[7]->GetWindowText(tmp);
	strncpy(plugin->str_param[7],tmp,sizeof(plugin->str_param[7]));
}
void iMixyPluginDlg::editor_change9(void)
{  
	CString tmp;
	edit[8]->GetWindowText(tmp);
	strncpy(plugin->str_param[8],tmp,sizeof(plugin->str_param[8]));

}

int iMixyPluginDlg::on_command(int lp,int rp)
{ 
	if(lp-kCONTROL_BASE==P_RESET1) 
	{ 		 
		max_l[0]=-120;		 
		max_r[0]=-120;  	 
		return 1; 
	} 
	if(lp-kCONTROL_BASE==P_RESET2) 
	{
		max_l[1]=-120;	
		max_r[1]=-120;  
		return 1; 
	} 
	if(lp-kCONTROL_BASE==P_RESET3) 
	{
		max_l[2]=-120;	
		max_r[2]=-120;  
		return 1; 
	} 
	if(lp-kCONTROL_BASE==P_RESET4) 
	{
		max_l[3]=-120;	
		max_r[3]=-120;  
		return 1; 
	} 
	if(lp-kCONTROL_BASE==P_RESET5) 
	{
		max_l[4]=-120;	
		max_r[4]=-120;  
		return 1; 
	} 
	if(lp-kCONTROL_BASE==P_RESET6) 
	{ 		 
		max_l[5]=-120;		 
		max_r[5]=-120;  	 
		return 1; 
	} 
	if(lp-kCONTROL_BASE==P_RESET7) 
	{
		max_l[6]=-120;	
		max_r[6]=-120;  
		return 1; 
	} 
	if(lp-kCONTROL_BASE==P_RESET8) 
	{
		max_l[7]=-120;	
		max_r[7]=-120;  
		return 1; 
	} 
	if(lp-kCONTROL_BASE==P_RESET9) 
	{
		max_l[8]=-120;	
		max_r[8]=-120;  
		return 1; 
	} 
	return CKXPluginGUI::on_command(lp,rp);
}
//-----------------------------------------------------


void iMixyPluginDlg::on_mouse_r_up(kPoint pt,int fl)
{
	kxparam_t old_vurange[9];
	for (int i=1; i<10; i++)
	{
		if (pt.x>vuleft[i-1] && pt.x<vuleft[i-1]+vu[-1+i]->get_peak_width()&&pt.y >vutop && pt.y<vutop+vu[-1+i]->get_peak_height())	
		{				
			kMenu menu;	
			menu.create();	
			menu.add("-24 to 0 dB",100);	
			menu.add("-60 to 0 dB",101);	
			menu.add("-90 to 0 dB",102);	
			menu.add("-120 to 0 dB",103);	
			menu.add("-24 to +12 dB",104);	
			menu.add("-60 to +12 dB",105);	
			menu.add("-90 to +12 dB",106);	
			menu.add("-120 to +12 dB",107);
			
			POINT pp; 	
			pp.x=pt.x; pp.y=pt.y;	
			ClientToScreen(&pp); 		
			int ret=menu.track(TPM_LEFTBUTTON|TPM_RIGHTBUTTON|TPM_RETURNCMD,pp.x,pp.y,this);					
			old_vurange[i-1]=plugin->_params[44+i];	
			switch(ret)	
			{		
			case 100: plugin->_params[44+i]=0; break;		
			case 101: plugin->_params[44+i]=1; break;		
			case 102: plugin->_params[44+i]=2; break;		
			case 103: plugin->_params[44+i]=3; break;		
			case 104: plugin->_params[44+i]=4; break;		
			case 105: plugin->_params[44+i]=5; break;		
			case 106: plugin->_params[44+i]=6; break;		
			case 107: plugin->_params[44+i]=7; break;	
			}			
	
			if(plugin->_params[i+44]!=old_vurange[i-1])	
			{ 		
				hide();		
				switch(plugin->_params[44+i])		
				{				
				case 0 :			
					vu[-1+i]->set_bitmaps(mf.load_image(("my0-24.jpg")),mf.load_image(("my0-24h.jpg")));			
					pm_vmin[i-1]=-24.0f;			
					pm_vmax[i-1]=0.0f;			
					break;		
				case 1 :			
					vu[-1+i]->set_bitmaps(mf.load_image(("my0-60.jpg")),mf.load_image(("my0-60h.jpg")));			
					pm_vmin[i-1]=-60.0f;			
					pm_vmax[i-1]=0.0f;			
					break;		
				case 2 :			
					vu[-1+i]->set_bitmaps(mf.load_image(("my0-90.jpg")),mf.load_image(("my0-90h.jpg")));			
					pm_vmin[i-1]=-90.0f;			
					pm_vmax[i-1]=0.0f;			
					break;		
				case 3 :			
					vu[-1+i]->set_bitmaps(mf.load_image(("my0-120.jpg")),mf.load_image(("my0-120h.jpg")));			
					pm_vmin[i-1]=-120.0f;			
					pm_vmax[i-1]=0.0f;			
					break;		 
				case 4 :			
					vu[-1+i]->set_bitmaps(mf.load_image(("my12-24.jpg")),mf.load_image(("my12-24h.jpg")));			
					pm_vmin[i-1]=-36.0f;			
					pm_vmax[i-1]=12.0f;			
					break;		
				case 5 :			
					vu[-1+i]->set_bitmaps(mf.load_image(("my12-60.jpg")),mf.load_image(("my12-60h.jpg")));			
					pm_vmin[i-1]=-72.0f;			
					pm_vmax[i-1]=12.0f;			
					break;		
				case 6 :			
					vu[-1+i]->set_bitmaps(mf.load_image(("my12-90.jpg")),mf.load_image(("my12-90h.jpg")));			
					pm_vmin[i-1]=-102.0f;			
					pm_vmax[i-1]=12.0f;			
					break;		 
				case 7 :			
					vu[-1+i]->set_bitmaps(mf.load_image(("my12-120.jpg")),mf.load_image(("my12-120h.jpg")));			
					pm_vmin[i-1]=-132.0f;			
					pm_vmax[i-1]=12.0f;			
					break;					
				}
			 
				//remove 'group' control:		
				grpbox.hide();		
				show();	
			} 
		}
	} 
}
	

