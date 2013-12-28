// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov and LeMury, 2003-2006.
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

//k2lt v1.3	'Epilog Lite' Output selector without asio
//Only for 10k2 CL cards -- New labels

#include "stdafx.h"
// --------------- the above should be a 'standard' beginning of any .cpp file
// effect class
#include "k2lt.h"
// effect source
#include "da_k2lt.cpp"


typedef enum _params_id{ SEL_ID };

static char* outname[] = {"Out 1/2","Out 3/4","Out 5/6","Out 7/8",
						  "Out 9/10","Out 11/12","Out 13/14","Out 15/16"};

static kxparam_t default_params[K2LT_PARAMS_COUNT] = {0,1,2,3,4,5,6,7};

static kxparam_t old_params[K2LT_PARAMS_COUNT];	// holds old values

// for kX Automation: param description 
// ---------------------------------------------------------
static kx_fxparam_descr _param_descr[K2LT_PARAMS_COUNT] = {
	{"Out1/2",	0, 0, 126},
	{"Out3/4",	0, 0, 126},
	{"Out5/6",	0, 0, 126},
	{"Out7/8",	0, 0, 126},
	{"Out9/10",	0, 0, 126},
	{"Out11/12",0, 0, 126},
	{"Out13/14",0, 0, 126},
	{"Out15/16",0, 0, 126},
};

// set all default plugin parameters
// ----------------------------------------------------------
int iK2ltPlugin::set_defaults() 
{
	set_all_params(default_params); 
	return 0;
}

//------------------------------------------------------------------
int iK2ltPlugin::describe_param(int ndx, kx_fxparam_descr *descr)
{
	memcpy(descr, &_param_descr[ndx], sizeof kx_fxparam_descr); return 0;
}

// presets
// -------------------------------------------------
#define P (kxparam_t)

static kxparam_t presets[]=
{
P "kX standard",	 4,1,2,3,0,5,6,7,
P "Analog first",	 4,0,5,3,1,2,6,7,
P "Digital first",	 1,2,6,3,4,0,5,7,
 NULL // must be here
};
#undef P

const kxparam_t *iK2ltPlugin::get_plugin_presets()
{
 return presets;
}

/* Plugin Implemenation */
//---------------------------------------------------------------------
// ndx corresponds to 'logical' parameter id
int iK2ltPlugin::get_param(int ndx, kxparam_t *value)
{
	*value = _params[ndx]; 
	return 0;
}
//--------------------------------------------------------------------
// set_param sets 'logical' values by writing to 'physical' registers
//-------------------------------------------------------------------
int iK2ltPlugin::set_param(int ndx, kxparam_t value)
{
	_params[ndx] = value;

	off= (ndx*2);				// calc offset
	set_select(off,value);		// do selection

	// Sync (refresh) GUI
	if (cp) ((iK2ltPluginDlg*) cp)->sync(ndx);

	return 0;
}
//--------------------------------------------------
// Set selected channel output function (LN)
//--------------------------------------------------
int iK2ltPlugin::set_select(int off, kxparam_t value)
{

	if (value <=4)		// 0..4
		{ Zl=(word)(KX_OUT(value*2));
		  Zr=Zl+1;
		  Y=C_4;
		}	
		else

	if (value == 5)		// KX_OUT(11,12)
		{ Zl=(KX_OUT(0x11));
		  Zr=Zl+1;
		  Y=C_4;
		}	
		else

	if (value == 6)		// KX_OUT(28,29)
		{ Zl=(KX_OUT(0x28));
		  Zr=Zl+1;
		  Y=C_4;
		}	
		else

	if (value ==7)		// KX_OUT(0xa,0xb) WINMME Rec
		{ Zl=KX_OUT(0xA);
		  Zr=Zl+1;
		  Y=C_4; 
		}
        else

    if (value ==8)		// KX_OUT(0x30,0x31) side left/right - added in 3551
		{ Zl=KX_OUT(0x30);
		  Zr=Zl+1;
		  Y=C_4; 
		}

		write_mc(0, off, Zl, Zr, Y);
			
		return 0;
}
//---------------------------------------------------
// Write micro code (LN)
//---------------------------------------------------
int iK2ltPlugin::write_mc(int _CODE, int off, word Zl, word Zr, word Y)
{
		// write Z (destination)pair
		write_instr_r(_CODE + (off), Zl);
		write_instr_r(_CODE + (off+1), Zr);

		// write Y output multiplier pair
		write_instr_y(_CODE + (off), Y);
		write_instr_y(_CODE + (off+1), Y);

		return 0;
}
//-----------------------------------
// Set all parameters
//--------------------------------------------------
int iK2ltPlugin::set_all_params(kxparam_t* values)
{
	for (int i = 0; i < K2LT_PARAMS_COUNT; i++) set_param(i, values[i]);
	return 0;
}

//----------------------------------------------------
int iK2ltPlugin::request_microcode() 
{
		is_aps=0;
		is_10k2=0;
		is_51=0;

// get card info
        ikx->get_dword(KX_DWORD_IS_APS,&is_aps);
        ikx->get_dword(KX_DWORD_IS_51,&is_51);
        ikx->get_dword(KX_DWORD_IS_10K2,&is_10k2);
//		ikx->get_dword(KX_DWORD_CAN_PASSTHRU,&can_passthru);

	publish_microcode(k2lt);

	return 0;
}
//--------------------------------------------------------
const char *iK2ltPlugin::get_plugin_description(int id)
{
	plugin_description(k2lt);
}
iKXPluginGUI *iK2ltPlugin::create_cp(kDialog *parent, kFile *mf)
{
	iK2ltPluginDlg *tmp;
	tmp = new iK2ltPluginDlg(this, parent,mf);
	tmp->create();
	return tmp;
}
int iK2ltPlugin::get_user_interface()
{
	return IKXPLUGIN_OWNER; // means 'custom' GUI panel
}

/* Plugin GUI Implementation */
//----------------------------------------------------------------------
void iK2ltPluginDlg::init()
{
	CKXPluginGUI::init(); // this is necessary
	
	// display warning msg if no 10k2
	if (! plugin->is_10k2){
		create_dialog(plugin_name(k2lt), 400, 50);
		create_label(card,K2LT_PARAMS_COUNT+1,"No 10k2 card detected...Please use k1lt or Epilog..!",10,20,300);
		goto gui_end;}

# define top 40		//base top position
# define lft 70		//base left position

	// create dialog        (title, Width, Height)
	create_dialog(plugin_name(k2lt), 255, 355,-1,KXFX_NO_RESIZE);

	// add  card type label
    create_label(card,K2LT_PARAMS_COUNT+1, "type",10,10,150);
	
	// add text labels
	for ( i = 0; i < K2LT_PARAMS_COUNT; i++) {
	create_label(lab[i],K2LT_PARAMS_COUNT+1,(outname[i]),10,(top+3+(i*40)), 60);}

	// create combo boxes and get label names 
	char c[KX_MAX_STRING];
	for ( i = 0; i < K2LT_PARAMS_COUNT; i++) {
  //                    ctrl_id,  label,	  left, top, wd, n_items, drop_width)
	create_combo(sel[i],SEL_ID+i,(outname[i]), lft,(top+(i*40)),  165,  7,   160);

	plugin->ikx->get_description(KX_HW_NAME, KX_OUT_I2S0,c);	//0,1
	sel[i].add_string(c);
	plugin->ikx->get_description(KX_HW_NAME, KX_OUT_SPDIF0,c);	//2,3
	sel[i].add_string(c);
	plugin->ikx->get_description(KX_HW_NAME, KX_OUT_SPDIF1,c);	//4,5
	sel[i].add_string(c);
	plugin->ikx->get_description(KX_HW_NAME, KX_OUT_SPDIF2,c);	//6,7
	sel[i].add_string(c);
	plugin->ikx->get_description(KX_HW_NAME, KX_OUT_I2S3,c);	//8,9
	sel[i].add_string(c);
	plugin->ikx->get_description(KX_HW_NAME, KX_OUT_I2S1,c);	//11,12
	sel[i].add_string(c);
	plugin->ikx->get_description(KX_HW_NAME, KX_OUT_SPDIF3,c);	//28,29 (10k2 only)
	sel[i].add_string(c);
	plugin->ikx->get_description(KX_HW_NAME, KX_OUT_REC,c);		//WMME REC
	sel[i].add_string(c); 
    plugin->ikx->get_description(KX_HW_NAME, KX_OUT_I2S2,c);    // side left/right - added in 3551
	sel[i].add_string(c); 
    }

	controls_enabled = 1; // enable updating GUI when the parameters are changed

	// get string info card type
  	char name[KX_MAX_STRING];
    plugin->ikx->get_string(KX_STR_CARD_NAME, name);
	card.SetWindowText(name);

	// update all 'GUI' elements  -- sync all (LN)
	for ( i = 0; i < K2LT_PARAMS_COUNT; i++)  sync(i);

gui_end:;
}
//-----------------------------------------------------------------------
// Sync any labels with the paramaters, update(refdraw) all GUI controls
//-----------------------------------------------------------------------
void iK2ltPluginDlg::sync(int ndx)
{
	
	kxparam_t v;

        kxparam_t old;

	old = old_params[ndx];		// get old params value
	v = plugin->_params[ndx];	// get params new value
	
		// swap combo that have the same selection

	// find equal
	for( j=0; j < K2LT_PARAMS_COUNT; j++)
			{
			if ( (plugin->_params[j]) == (plugin->_params[ndx]) ){
					
				if (j==ndx) goto excl_sel; //exclude own current id

					// j is found to be equal. swap destination;				

					(plugin->_params[j]) = old; // set found's param to old val
					sel[j].set_selection((int)old);  // update found's gui
					plugin->set_param(j, old); // change found's code now
					excl_sel:;}
				}

	sel[ndx].set_selection((int)v); // update id's gui
	old_params[ndx] = v;	   // update id's old params value
}
//----------------------------------------------------------------------------

