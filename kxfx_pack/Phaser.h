// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov and Max Mikhailov, 2001-2004.
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

#ifndef _KX_PHASER_EFFECT_H
#define _KX_PHASER_EFFECT_H

#define PHASER_PARAMS_COUNT	8

// Plugin definition
class iPhaserPlugin : public iKXPlugin
{
public:
	virtual int request_microcode();
	virtual const char *get_plugin_description(int id);
	
	virtual iKXPluginGUI *create_cp(kDialog *parent, kFile *mf);
	virtual int get_user_interface();

	virtual int get_param_count(void) {return PHASER_PARAMS_COUNT;}
	virtual int describe_param(int, kx_fxparam_descr*);
	virtual int set_defaults();
    virtual int set_param(int ndx, kxparam_t value);
    virtual int get_param(int ndx, kxparam_t *value);
    virtual int set_all_params(kxparam_t* values);

    virtual const kxparam_t *get_plugin_presets();

	kxparam_t _params[PHASER_PARAMS_COUNT];
	void UpdateParams(void);
};


// Plugin GUI definition
class iPhaserPluginDlg : public CKXPluginGUI
{
public:
	iPhaserPlugin *plugin;

	iPhaserPluginDlg(iPhaserPlugin *plg, kDialog *parent_, kFile *mf_) : CKXPluginGUI(parent_,mf_) {plugin = plg; };

	virtual void init();
	virtual iKXPlugin *get_plugin() { return plugin; };

	kFader dry;
	kFader wet;
	kFader feedback;
	kFader xfeed;
	kFader sweeprate;
	kFader sweeprange;
	kFader frequency;
	kCombo stages;
	kStatic label;	
	
	virtual void sync(int what=EVERYPM_ID);
};

declare_effect_source(Phaser);

#endif
