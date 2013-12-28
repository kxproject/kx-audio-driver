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

//k1lt v1.3	"Epilog Lite" Output selector without ASIO
//Only for 10k1 based CL cards!
//Change log:
//10k1 Rear out phase inverted
//DOO switch added -- (phase correction if is_doo)
//New ikx->get_description Labeling added


#include "stdafx.h"
// --------------- the above should be a 'standard' beginning of any .cpp file
// effect class
#include "k1lt.h"
// effect source
#include "da_k1lt.cpp"


typedef enum _params_id{ DOO_ID, SEL_ID };


static char* outname[] = {"Out 1/2","Out 3/4","Out 5/6","Out 7/8",
							"Out 9/10","Out 11/12","Out 13/14"};

static kxparam_t default_params[K1LT_PARAMS_COUNT] = {0,0,1,2,3,4,5,6};

static kxparam_t old_params[K1LT_PARAMS_COUNT];	// holds old selection values for "swapping"

// for kX Automation: param description 
// --------------------------------------------------------
static kx_fxparam_descr _param_descr[K1LT_PARAMS_COUNT] = {
	{"DOO",	0, 0, 1},
	{"Out1/2",	0, 0, 126},
	{"Out3/4",	0, 0, 126},
	{"Out5/6",	0, 0, 126},
	{"Out7/8",	0, 0, 126},
	{"Out9/10",	0, 0, 126},
	{"Out11/12",0, 0, 126},
	{"Out13/14",0, 0, 126}
};

// set all default plugin parameters
// ------------------------------------------------------------
int iK1ltPlugin::set_defaults() 
{
	set_all_params(default_params); 
	return 0;
}

//------------------------------------------------------------------
int iK1ltPlugin::describe_param(int ndx, kx_fxparam_descr *descr)
{
	memcpy(descr, &_param_descr[ndx], sizeof kx_fxparam_descr); return 0;
}

// presets
// -------------------------------------------------
#define P (kxparam_t)

static kxparam_t presets[]=
{
P "kX Standard",	0,4,1,2,3,0,5,6,
P "Analog first",   0,4,0,5,3,1,2,6,
P "Digital first",	1,1,2,3,4,0,5,6,
 NULL // must be here
};
#undef P

const kxparam_t *iK1ltPlugin::get_plugin_presets()
{
 return presets;
}

/* Plugin Implemenation */
//---------------------------------------------------------------------
int iK1ltPlugin::get_param(int ndx, kxparam_t *value)
{
	*value = _params[ndx]; 
	return 0;
}
//--------------------------------------------------------------------
// set_param sets 'logical' values by writing to 'physical' registers
//-------------------------------------------------------------------
int iK1ltPlugin::set_param(int ndx, kxparam_t value)
{
	_params[ndx] = value;	// store new value in parameter array

	if ( (ndx == DOO_ID) &&! (is_10k2) ){
		// set hw DOO:
		ikx->set_hw_parameter(KX_HW_DOO,(dword)value);
		goto done_setp;}

	off= ((ndx-1)*2);			// calc offset
	set_select(off,value);		// select

done_setp:
	// Sync (refresh) GUI
	if (cp) ((iK1ltPluginDlg*) cp)->sync(ndx);
	return 0;
}
//--------------------------------------------------
// Set selected channel output function (LN)
//--------------------------------------------------
int iK1ltPlugin::set_select(int off, kxparam_t value)
{
	inp_type=1; //set inp_type to default MACINTS

	if (value <4)		// 0..3 analog front to Headphones
		{ Zl=(word)(KX_OUT(value*2));
		  Zr=Zl+1;
		  Y=C_4;
		goto done;}	

	if (value==4)		// analog Rear/spdif-3
		{ Zl= _TMPL;	// select tmp grps
		  Zr= _TMPR;
		  Y=C_7fffffff;		// amp is 0x7fffffff
		inp_type=0;
		goto done;}	

	if (value ==5)		// 5 is analog center/lfe
		{ Zl=(KX_OUT(0x11));
		  Zr=Zl+1;
		  Y=C_4;	
		goto done;}	

	if (value >=6)		// 6 is ADC Rec
		{ Zl=KX_OUT(0xA);
		  Zr=Zl+1;
		  Y=C_4;	

done:
		write_mc(0, off, Zl, Zr, Y, inp_type);}
			
		return 0;
}
//---------------------------------------------------
// Change micro code "on the fly" (LN)
//---------------------------------------------------
int iK1ltPlugin::write_mc(int _CODE, int off, word Zl, word Zr, word Y, int inp_type)
{
		// write Z (destination)pair
		write_instr_r(_CODE + (off), Zl);
		write_instr_r(_CODE + (off+1), Zr);

		// write Y multiplier pair
		write_instr_y(_CODE + (off), Y);
		write_instr_y(_CODE + (off+1), Y);

if (! is_10k2) ikx->get_hw_parameter(KX_HW_DOO,&is_doo); // update to current DOO

		if ( (is_doo)||(is_10k2) ){
		// write opcode pair. select MACS if DOO
		write_instr_opcode(_CODE + off,   (word) ((inp_type<1)?MACS:MACINTS) );
		write_instr_opcode(_CODE +(off+1),(word) ((inp_type<1)?MACS:MACINTS) );}

		else {
		// write opcode pair (select MACSN if analog
		write_instr_opcode(_CODE + off,   (word) ((inp_type<1)?MACS1:MACINTS) );
		write_instr_opcode(_CODE +(off+1),(word) ((inp_type<1)?MACS1:MACINTS) ); }

		return 0;
}
//--------------------------------------------------
// get hardware parameters from driver (LN)
//--------------------------------------------------
void iK1ltPlugin::get_hwpm() 	
{
	is_aps=0;
	is_10k2=0;
	is_51=0;
	is_doo=0;

	ikx->get_dword(KX_DWORD_IS_APS,&is_aps);
	ikx->get_dword(KX_DWORD_IS_10K2,&is_10k2);
	ikx->get_dword(KX_DWORD_IS_51,&is_51);

// get current DOO status
	if (! is_10k2) {		
        ikx->get_hw_parameter(KX_HW_DOO,&is_doo);
		_params[DOO_ID] = is_doo;			// init doo preset
		default_params[DOO_ID] = is_doo;	// init doo default
					}
}

//--------------------------------------------------
// Set all parameters
//--------------------------------------------------
int iK1ltPlugin::set_all_params(kxparam_t* values)
{
	for (int i = 0; i < K1LT_PARAMS_COUNT; i++) set_param(i, values[i]);
	return 0;
}

//----------------------------------------------------
int iK1ltPlugin::request_microcode() 
{
	publish_microcode(k1lt);

	get_hwpm();	// get hw params

	return 0;
}
//--------------------------------------------------------
const char *iK1ltPlugin::get_plugin_description(int id)
{
	plugin_description(k1lt);
}
iKXPluginGUI *iK1ltPlugin::create_cp(kDialog *parent, kFile *mf)
{
	iK1ltPluginDlg *tmp;
	tmp = new iK1ltPluginDlg(this, parent,mf);
	tmp->create();
	return tmp;
}

int iK1ltPlugin::get_user_interface()
{

	return IKXPLUGIN_OWNER; // means 'custom' GUI panel
}

/* Plugin GUI Implementation */
//---------------------------------------------------------
void iK1ltPluginDlg::init()
{
	CKXPluginGUI::init(); // this is necessary

	plugin->get_hwpm();	//update to current hw params (doo might been changed by mixer)

	// display warning msg gui if this is a 10k2 card!
	if (plugin->is_10k2){
		create_dialog(plugin_name(k1lt), 400, 50);
		create_label(card,K1LT_PARAMS_COUNT+1,"10k2 card detected...Please use k2lt or Epilog..!",10,20,350);
		goto gui_end;}

# define top 40		//base top position
# define lft 70		//base left position

	// create dialog        (title, Width, Height)
	create_dialog(plugin_name(k1lt), 255, 320,-1,KXFX_NO_RESIZE);

	// add  card type label
    create_label(card,K1LT_PARAMS_COUNT+1, "type",10,10,150);

	// (create DOO	      Ctrl_ID, Caption, Left, Top, Width)
	create_checkbox(setdoo, DOO_ID, "DOO", 160, 13, 50);

	// add text labels
	for ( i = 0; i < (K1LT_PARAMS_COUNT - 1); i++) {
	create_label(lab[i],K1LT_PARAMS_COUNT+1,(outname[i]),10,(top+3+(i*40)), 60);}


	// create combo boxes and get label names 
	char c[KX_MAX_STRING];

	for ( i = 0; i < (K1LT_PARAMS_COUNT - 1); i++) {
  //               ctrl_id,label,		       left,top,       wd, n_items, drop_width)
	create_combo(sel[i],SEL_ID+i,(outname[i]), lft,(top+(i*40)),165,  7,   160);

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
	plugin->ikx->get_description(KX_HW_NAME, KX_OUT_REC,c);		//WMME REC
	sel[i].add_string(c); }


	controls_enabled = 1; // enable updating GUI when the parameters are changed

	// get string info card type
  	char name[KX_MAX_STRING];
    plugin->ikx->get_string(KX_STR_CARD_NAME, name);
	card.SetWindowText(name);

	// update all 'GUI' elements to the current plugin parameters
	for ( i = 0; i < K1LT_PARAMS_COUNT; i++)  sync(i); //(sync all LN)

gui_end:;
	
}
//-----------------------------------------------------------------------
// Sync any labels with the paramaters, update(refdraw) all GUI controls
//----------------------------------------------------------------------
void iK1ltPluginDlg::sync(int ndx) 	// ndx bevat parameter id nr
{
	
	if ( (ndx == DOO_ID) &&!(plugin->is_10k2) ) {
		if ( (plugin->_params[ndx]) != (old_params[ndx]) ){
				old_params[ndx] = (plugin->_params[ndx]);
				plugin->set_all_params(plugin->_params); }

		if(controls_enabled) setdoo.set_check((int)plugin->_params[DOO_ID]);
		goto gui_done;}
	
	else{

	kxparam_t v;

    	kxparam_t old;

	old = old_params[ndx];		// get old params value
	v = plugin->_params[ndx];	// get params new value
	
	// swap combo that have the same selection

	// find equal
	for( j=1; j < (K1LT_PARAMS_COUNT); j++)
			{
		if ( (plugin->_params[j]) == (plugin->_params[ndx]) ){
					
			if (j==ndx) goto excl_sel; //exclude own current id

					// j is found to be equal. swap destination;				

					(plugin->_params[j]) = old;	// set found's param to old val
					sel[j-1].set_selection((int)old);// update found's gui
					plugin->set_param(j, old);	// change found's code now

				excl_sel:; }
				}

	sel[ndx-1].set_selection((int)v); // update id's gui
	old_params[ndx] = v;	   // update id's old params value
	}

		
gui_done:;
}
//----------------------------------------------------------------------------

