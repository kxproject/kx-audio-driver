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

//k1lt.h

#ifndef _KX_K1LT_EFFECT_H
#define _KX_K1LT_EFFECT_H

#define K1LT_PARAMS_COUNT	8	// number of user-visible parameters

// Plugin definition
class iK1ltPlugin : public iKXPlugin
{
public:
	virtual int request_microcode();
	virtual const char *get_plugin_description(int id);
	
	virtual iKXPluginGUI *create_cp(kDialog *parent, kFile *mf);
	virtual int get_user_interface();

	virtual int get_param_count(void) {return K1LT_PARAMS_COUNT;}
	virtual int describe_param(int, kx_fxparam_descr*); // this function is used for kX Automation and mixer-provided GUI panels
	virtual int set_defaults();
    virtual int set_param(int ndx, kxparam_t value);
    virtual int get_param(int ndx, kxparam_t *value);
    virtual int set_all_params(kxparam_t* values);
	virtual const kxparam_t *get_plugin_presets();

	virtual int write_mc(int _CODE, int off, word Zl, word Zr, word Y, int inp_type); // my function LN
	virtual int set_select(int off, kxparam_t value);	// my function LN
	virtual void get_hwpm();	// gets hardware params from driver LN


	kxparam_t _params[K1LT_PARAMS_COUNT]; // place to store parameters inside the plugin

	// next are some variables for Micro code change
	word Zl;		// Left destination
	word Zr;		// Right destination
	word Y;			// Output amp factor
	int off;		// offset
	int inp_type;	// selects macints or macs(n)
	
	dword is_aps;
	dword is_10k2;
	dword is_51;
	dword is_doo;
//	dword dsp_flags;
	
};

// Plugin GUI definition
class iK1ltPluginDlg : public CKXPluginGUI
{
public:
	iK1ltPlugin *plugin;

    	// constructor:
	iK1ltPluginDlg(iK1ltPlugin *plg, kDialog *parent_, kFile *mf_)
		: CKXPluginGUI(parent_,mf_) {plugin = plg; };

	virtual void init();
	virtual iKXPlugin *get_plugin() { return plugin; };
	int i;
	int j;

	// declare GUI Elements
	kCombo sel[K1LT_PARAMS_COUNT];	// combo
	kStatic lab[K1LT_PARAMS_COUNT];	// labels	
	kStatic	card;					// card type label
	kCheckButton setdoo;			// toggle doo mode

	// for updating UI from plugin

	virtual void sync(int what=EVERYPM_ID);
};

declare_effect_source(k1lt);

#endif
