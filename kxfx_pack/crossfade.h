// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov and Max Mikhailov, 2001-2005.
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

declare_effect_source(crossfade);

// Number of plugin parameters
#define CROSSFADE_PARAMS_COUNT	2

// Plugin definition
class iCrossfadePlugin : public iKXPlugin
{
public:
	// members of iKXPlugin
	virtual int request_microcode();
	virtual const char *get_plugin_description(int id) { plugin_description(crossfade); }
	
	virtual iKXPluginGUI *create_cp(kDialog *parent, kFile *);
	virtual int get_user_interface();

	virtual int get_param_count(void) {return CROSSFADE_PARAMS_COUNT;}
	virtual const kxparam_t *get_plugin_presets();
	virtual int describe_param(int , kx_fxparam_descr *);
	virtual int set_defaults();
	virtual int set_param(int ndx, kxparam_t value);
	virtual int get_param(int ndx, kxparam_t *value);
	virtual int set_all_params(kxparam_t* values);

	kxparam_t _params[CROSSFADE_PARAMS_COUNT]; // place to store parameters inside the plugin
};

// Plugin GUI definition
class iCrossfadePluginDlg : public CKXPluginGUI
{
public:
	iCrossfadePlugin *plugin;

    // constructor:
	iCrossfadePluginDlg(iCrossfadePlugin *plg, kDialog *parent_, kFile *mf_):CKXPluginGUI(parent_, mf_) {plugin = plg;};

	virtual void init();
	virtual iKXPlugin *get_plugin() { return plugin; };

	// declare GUI Elements

	// Controls
	kFader fdrGain,fdrCross;

	// for updating UI from plugin
	virtual void sync(int what=EVERYPM_ID);
};
