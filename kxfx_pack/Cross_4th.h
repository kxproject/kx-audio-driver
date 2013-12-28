// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov, Max Mikhailov and Dmitry Kapustin, 2001-2005.
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

#ifndef _KX_Cross_4_EFFECT_H
#define _KX_Cross_4_EFFECT_H

#define CROSS_4_PARAMS_COUNT	1	// number of user-visible parameters

// Plugin definition
class iCross_4_Plugin : public iKXPlugin
{
public:
	virtual int request_microcode();
	virtual const char *get_plugin_description(int id);
	
	virtual iKXPluginGUI *create_cp(kDialog *parent, kFile *mf);
	virtual int get_user_interface();

	virtual int get_param_count(void) {return CROSS_4_PARAMS_COUNT;}
	virtual int describe_param(int, kx_fxparam_descr*); // this function is used for kX Automation and mixer-provided GUI panels
	virtual int set_defaults();
        virtual int set_param(int ndx, kxparam_t value);
        virtual int get_param(int ndx, kxparam_t *value);
        virtual int set_all_params(kxparam_t* values);

        //const virtual kxparam_t *get_plugin_presets();

	kxparam_t _params[CROSS_4_PARAMS_COUNT]; // place to store parameters inside of plugin
};

// Plugin GUI definition
class iCross_4_PluginDlg : public CKXPluginGUI
{
public:
	iCross_4_Plugin *plugin;

    	// constructor:
	iCross_4_PluginDlg(iCross_4_Plugin *plg, kDialog *parent_, kFile *mf_) : CKXPluginGUI(parent_,mf_) {plugin = plg; };

	virtual void init();
	virtual iKXPlugin *get_plugin() { return plugin; };

	// declare GUI Elements

	// Controls
	kSlider	freq_1;
	kStatic	label;
	
	// for updating UI from plugin
	virtual void sync(int what=EVERYPM_ID);
};

declare_effect_source(Crossover_4);

#endif
