// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov and LeMury, 2003-2004.
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

//src.h

#ifndef _KX_SRC_EFFECT_H
#define _KX_SRC_EFFECT_H

#define SRC_PARAMS_COUNT	1	// number of user-visible parameters

#define MAX_SRC_INPUTS		(6+7+32)	// 6 ac97 + 7 kx_in + 32 fxbusses (10k2)

// a list of inputs with description
typedef struct
{
 word kx_in;	// =0 for the last entry
 word mult;	// multiplier
 int ac_97;	// if not ac97, set it to '-1'
 char name[KX_MAX_STRING];
}src_input;

// Plugin definition
class iSrcPlugin : public iKXPlugin
{
public:
	virtual int init();	// plugin initialization
	virtual int request_microcode();
	virtual const char *get_plugin_description(int id);	
	virtual iKXPluginGUI *create_cp(kDialog *parent, kFile *mf);
	virtual int get_user_interface();
	virtual int get_param_count(void) {return SRC_PARAMS_COUNT;}
	virtual int describe_param(int, kx_fxparam_descr*); // this function is used for kX Automation and mixer-provided GUI panels
	virtual int set_defaults();
    virtual int set_param(int ndx, kxparam_t value);
    virtual int get_param(int ndx, kxparam_t *value);
    virtual int set_all_params(kxparam_t* values);
	kxparam_t _params[SRC_PARAMS_COUNT]; // place to store parameters inside the plugin
	
	virtual int write_mc(word X, word Y);	// write micro code

	virtual const kxparam_t *get_plugin_presets();

	src_input inputs[MAX_SRC_INPUTS];
	kxparam_t presets[MAX_SRC_INPUTS*2+1];
};

// Plugin GUI definition
class iSrcPluginDlg : public CKXPluginGUI
{
public:
	iSrcPlugin *plugin;

    	// constructor:
	iSrcPluginDlg(iSrcPlugin *plg, kDialog *parent_, kFile *mf_)
		: CKXPluginGUI(parent_,mf_) { plugin=plg; }

	virtual void init();
	virtual iKXPlugin *get_plugin() { return plugin; };
	
	// gui elements
	kStatic	card;		// displays card type
	kCombo sel;			// select source

	// for updating UI from plugin
	virtual void sync(int what=EVERYPM_ID);
	
};

declare_effect_source(src);

#endif
