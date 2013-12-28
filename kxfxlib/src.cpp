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

// src.cpp	-- Source Select (Prolog like but with AC97 and FXbus)
// Src v1.3c	fixed: -ct4832 and other non-5.1 cards (APS device specific) included
// Scr v1.4 	3536 New labels
// src v2.0 	re-written
// src v2.4	added support for presets


#include "stdafx.h"
// --------------- the above should be a 'standard' beginning of any .cpp file
// effect class
#include "src.h"
// effect source
#include "da_src.cpp"

// translate KX_IN -> KX_HW... input
static int src_translation_table[14]=
             { KX_IN_AC97,KX_IN_AC97,KX_IN_SPDIFA,KX_IN_SPDIFA,KX_IN_SPDIFC,KX_IN_SPDIFC,
               KX_IN_SPDIFB,KX_IN_SPDIFB,KX_IN_I2S0,KX_IN_I2S0,KX_IN_I2S1,KX_IN_I2S1,
               KX_IN_I2S2,KX_IN_I2S2 };

typedef enum _params_id{ SEL_ID	};

static kxparam_t default_params[SRC_PARAMS_COUNT] = { 0 };	// ac97.mic is default

// set all default plugin parameters 
// ------------------------------------------------------------
int iSrcPlugin::set_defaults() 
{
	set_all_params(default_params); 
	return 0;
}

// kX Automation: param description 
// ----------------------------------------------------------
static kx_fxparam_descr _param_descr[SRC_PARAMS_COUNT] = {
	{"Source", KX_FXPARAM_USER, 0, 126 },
};

//--------------------------------------------
int iSrcPlugin::describe_param(int ndx, kx_fxparam_descr *descr)
{
	memcpy(descr, &_param_descr[ndx], sizeof kx_fxparam_descr); 
	return 0;
}

/* Plugin Implemenation */
//---------------------------------------------------------------------------------------
// ndx corresponds to 'logical' parameter id
int iSrcPlugin::get_param(int ndx, kxparam_t *value)
{
	*value = _params[ndx]; 
	return 0;
}

// set_param sets 'logical' values by writing to 'physical' registers
//-------------------------------------------------------------------
int iSrcPlugin::set_param(int ndx, kxparam_t value)
{
	_params[ndx] = value;

	switch (ndx) 
	{
	case EVERYPM_ID:
		ndx=SEL_ID;
                // fall thru

	case SEL_ID:
		value = _params[SEL_ID];

		if(value>=0 && value<=MAX_SRC_INPUTS && inputs[value].kx_in!=0)
		{
		 if(inputs[value].ac_97!=-1) // need to re-program ac97 codec, too
		 {
	 		ikx->ac97_write(AC97_REG_REC_SELECT,(word)inputs[value].ac_97);
	 	 }	

		 write_mc(inputs[value].kx_in,inputs[value].mult);
		}
		break;
	}

// we need do synchronize the GUI part,
// if set_param was called by the mixer or kX Automation
	if (cp) ((iSrcPluginDlg*) cp)->sync(ndx);

	return 0;
}
//---------------------------------------------------
// Write Micro code "on the fly" LN
//---------------------------------------------------
int iSrcPlugin::write_mc(word X, word Y)
{
		// write source
		write_instr_x(_EL_INSTR + 0, X );
		write_instr_x(_EL_INSTR + 1,(X+0x1) );

		// write amp factor
		write_instr_y(_EL_INSTR + 0, Y );
		write_instr_y(_EL_INSTR + 1, Y );
		return 0;
}
//------------------------------------------------------
// Set defaults
//--------------------------------------------------
int iSrcPlugin::set_all_params(kxparam_t* values)
{
	for (int i = 0; i < SRC_PARAMS_COUNT; i++) 
	  set_param(i, values[i]);
	return 0;
}
// Get DSP Microcode gegevens
//----------------------------------------------------

const kxparam_t *iSrcPlugin::get_plugin_presets()
{
 return presets;
}

int iSrcPlugin::init()
{
	// this should be done HERE (before anything else)

	dword is_10k2=0,has_ac97=0; // is_aps=0,,is_51=0,is_a2ex=0,;

	ikx->get_dword(KX_DWORD_IS_10K2,&is_10k2);
	ikx->get_dword(KX_DWORD_AC97_PRESENT,&has_ac97);

	memset(inputs,0,sizeof(inputs));

        // initialize src_input structure
        int n=0,i=0;

        // ac97 inputs:
        if(has_ac97)
        {
         for(;n<6;n++)
         {
          inputs[n].kx_in=KX_IN(0x0);
          inputs[n].mult=C_20000000;
          inputs[n].ac_97=n+(n<<8);

           char *t=0;
           switch(n) // low byte
           {
            case 0: t="mic"; break;
            case 1: t="cd"; break;
            case 2: t="video"; break;
            case 3: t="aux"; break;
            case 4: t="linein"; break;
            case 5: t="stereomix"; break;
            case 6: t="monomix"; break;
            default: // internal error
            	break;
           }
           if(t)
            strcpy(inputs[n].name,t);
         }
        }
        else
        {
          inputs[n].kx_in=KX_IN(0x0);
          inputs[n].mult=C_20000000;
          inputs[n].ac_97=-1;
          ikx->get_description(KX_HW_NAME,KX_IN_AC97,inputs[n].name);
          n++;
        }

        // all the rest kx_ins
        for(;i<6;i++)
        {
          inputs[n].kx_in=KX_IN((i+1)*2);
          inputs[n].mult=C_20000000;
          inputs[n].ac_97=-1;
          ikx->get_description(KX_HW_NAME,src_translation_table[(i+1)*2],inputs[n].name);
          n++;
        }

        // add FXBUSses:
        for(i=0;i<(is_10k2?32:16);i+=2)
        {
          inputs[n].kx_in=KX_FX(i);
          inputs[n].mult=C_7fffffff;
          inputs[n].ac_97=-1;
          sprintf(inputs[n].name,"FXBus %d/%d",i,i+1);
          n++;
        }

        inputs[n].kx_in=0; // last item

        // initialize presets
        for(i=0;i<n;i++)
        {
         presets[i*2]=(kxparam_t)inputs[i].name;
         presets[i*2+1]=i;
        }
        presets[i*2]=0;

	return iKXPlugin::init();
}

int iSrcPlugin::request_microcode() 
{
	publish_microcode(src);

	return 0;
}
//----------------------------------------------------
const char *iSrcPlugin::get_plugin_description(int id)
{
	plugin_description(src);
}

iKXPluginGUI *iSrcPlugin::create_cp(kDialog *parent, kFile *mf)
{
	iSrcPluginDlg *tmp;
	tmp = new iSrcPluginDlg(this, parent,mf);
	tmp->create();
	return tmp;
}

int iSrcPlugin::get_user_interface()
{
	return IKXPLUGIN_OWNER; // means 'custom' GUI panel
}

/* Plugin GUI Implementation */
//-------------------------------------------------------
void iSrcPluginDlg::init()
{
	CKXPluginGUI::init(); // this is necessary

    int i;
    for(i=0;i<MAX_SRC_INPUTS;i++)
         if(plugin->inputs[i].kx_in==0)
          break;
	
	int nr_items=i; 

	// custom dialog box here...
	//                    (title, Width, Height)
	create_dialog(plugin_name(src), 180, 80,-1,KXFX_NO_RESIZE);

	// add  card type label
    	create_label(card,SRC_PARAMS_COUNT+1,"card name",10,10,135);

        // create combo + labels

        create_combo(sel,SEL_ID,"Source", 10, 40,  155,  nr_items,  155);

        char lbl[KX_MAX_STRING+10];
        
        for(i=0;i<nr_items;i++)
        {
         if(plugin->inputs[i].kx_in>=KX_FX(0)) // fxbus
         {
          sel.add_string(plugin->inputs[i].name);
         }
         else
          if(plugin->inputs[i].ac_97!=-1) // ac97 input
          {
           char *t=NULL;

           // mf is already available: find localized labels
           switch(plugin->inputs[i].ac_97&0xff) // low byte
           {
            case 0: t="mic"; break;
            case 1: t="cd"; break;
            case 2: t="video"; break;
            case 3: t="aux"; break;
            case 4: t="linein"; break;
            case 5: t="stereomix"; break;
            case 6: t="monomix"; break;
            default: // internal error
            	break;
           }

           if(t)
           {
            strcpy(lbl,"AC97 ");
            mf.get_profile("ac97",t,lbl+5,sizeof(lbl)-5); // +5 = strlen("AC97 ");
            sel.add_string(lbl);
           }
          }
          else
           if(plugin->inputs[i].kx_in>=KX_IN(0)) // kx_in (regular); not ac97 (above)
           {
            plugin->ikx->get_description(KX_HW_NAME,src_translation_table[plugin->inputs[i].kx_in-KX_IN(0)],lbl);
            sel.add_string(lbl);
           }
           // else { INTERNAL CODE ERROR! };
        }

	// display card type
  	char tmp_name[KX_MAX_STRING];
	plugin->ikx->get_string(KX_STR_CARD_NAME, tmp_name);
	card.SetWindowText(tmp_name);

	controls_enabled = 1; // enable updating GUI when the parameters are changed
	sync(EVERYPM_ID); // update all 'GUI' elements to the preset plugin parameters
}

// Sync any labels with the paramaters, update(refdraw) all GUI controls
//----------------------------------------------------------
void iSrcPluginDlg::sync(int ndx)
{	
	kxparam_t v;

	switch (ndx)
	{ 
	case EVERYPM_ID: // update all controls
	case SEL_ID:
        	// check if this Scr has ac97 selected (0..6)
        	v = plugin->_params[SEL_ID];

        	if(v<=6 && plugin->inputs[v].ac_97!=-1) // FIXME: do we really need this?..
        	{
        		// get currently selected ac97 source reg 0x1A
        		word val;

        		plugin->ikx->ac97_read(AC97_REG_REC_SELECT, &val);

        		val=val >>8;	//shift right
        		if(val != v) 
        		  plugin->_params[SEL_ID] = val;

        		if(controls_enabled)
        		 sel.set_selection(val);
        	}
        	else 
        	{	
        		if(controls_enabled) 
        		 sel.set_selection((int)plugin->_params[SEL_ID]);
        	}
	}
}

