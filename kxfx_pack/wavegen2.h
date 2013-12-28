// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov, Hanz Petrov and Max Mikhailov, 2001-2005.
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

// Copyright (c) eYagos, 2002. All rights reserved.

declare_effect_source(wavegen2);

// Number of plugin parameters
#define WAVEGEN2_PARAMS_COUNT	2

// Plugin definition
class iWavegen2Plugin : public iKXPlugin
{
public:
	// members of iKXPlugin
	virtual int request_microcode();
	virtual const char *get_plugin_description(int id) { plugin_description(wavegen2); };
	
	virtual iKXPluginGUI *create_cp(kDialog *parent, kFile *);
	virtual int get_user_interface();

	virtual int get_param_count(void) {return WAVEGEN2_PARAMS_COUNT;}
	virtual int describe_param(int , kx_fxparam_descr *); // this function is used for kX Automation and mixer-provided GUI panels
	virtual int set_defaults();
	virtual int set_param(int ndx, kxparam_t value);
	virtual int get_param(int ndx, kxparam_t *value);
	virtual int set_all_params(kxparam_t* values);

	kxparam_t _params[WAVEGEN2_PARAMS_COUNT]; // place to store parameters inside the plugin
};

// Plugin GUI definition
class iWavegen2PluginDlg : public CKXPluginGUI
{
public:
	iWavegen2Plugin *plugin;
	virtual iKXPlugin *get_plugin() { return plugin; };

	// constructor:
	iWavegen2PluginDlg(iWavegen2Plugin *plg, kDialog *parent_, kFile *mf_):CKXPluginGUI(parent_, mf_) {plugin = plg;};
	virtual void init();

	// declare GUI Elements

	// Controls
	kFader level,frecuency;
	
	// for updating UI from plugin
	virtual void sync(int what=EVERYPM_ID);

};
