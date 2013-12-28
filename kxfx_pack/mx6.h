// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov, LeMury 2003-2004.
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

// Written by: LeMury
// Mixer v1.113a

//--------------------------------
#ifndef _KX_MX6_EFFECT_H
#define _KX_MX6_EFFECT_H

#include "ColorCtrl.h"
#include "FontCtrl.h"

#define MX6_PARAMS_COUNT	38	// number of user-visible parameters

// Plugin definition
class iMx6Plugin : public iKXPlugin
{
public:
	virtual int request_microcode();
	virtual const char *get_plugin_description(int id);
	virtual int get_user_interface();
	virtual iKXPluginGUI *create_cp(kDialog *parent, kFile *mf);
	virtual int get_param_count(void) {return MX6_PARAMS_COUNT;}
	virtual int describe_param(int, kx_fxparam_descr*); // this function is used for kX Automation and mixer-provided GUI panels
	virtual int set_defaults();
    virtual int set_param(int ndx, kxparam_t value);
    virtual int get_param(int ndx, kxparam_t *value);
    virtual int set_all_params(kxparam_t* values);

  
	kxparam_t _params[MX6_PARAMS_COUNT]; // place to store parameters inside the plugin
};

// Plugin GUI definition
class iMx6PluginDlg : public CKXPluginGUI
{
public:
	iMx6Plugin *plugin;

   	// constructor:
	iMx6PluginDlg(iMx6Plugin *plg, kDialog *parent_,kFile *mf_);
	virtual iKXPlugin *get_plugin() { return plugin; };

        // desctructor
        ~iMx6PluginDlg();

	virtual void init();
	virtual void on_destroy();
	

// VU meter
	UINT_PTR timer_id;
	float max_l,max_r;
	afx_msg void OnTimer(UINT_PTR);
	afx_msg void OnClick();		//handels peak reset on l_mouse click
	int turn_on(int what);
	int on_command(int,int);
	CFont vufont;				// vu text font

// declare GUI Elements
	kPeak vumeter;	// peak vu meter
	kFader fdrvol1;	// 6 Volumes
	kFader fdrvol2;
	kFader fdrvol3;
	kFader fdrvol4;
	kFader fdrvol5;
	kFader fdrvol6;
	kFader fdrs1a;	// 6x2 Sends
	kFader fdrs1b;
	kFader fdrs2a;
	kFader fdrs2b;
	kFader fdrs3a;
	kFader fdrs3b;
	kFader fdrs4a;
	kFader fdrs4b;
	kFader fdrs5a;
	kFader fdrs5b;
	kFader fdrs6a;
	kFader fdrs6b;
	kCheckButton chkrec1;	// 6 Rec Switches
	kCheckButton chkrec2;
	kCheckButton chkrec3;
	kCheckButton chkrec4;
	kCheckButton chkrec5;
	kCheckButton chkrec6;	
	kCheckButton chkms1;	// 6 Main sw
	kCheckButton chkms2;
	kCheckButton chkms3;
	kCheckButton chkms4;
	kCheckButton chkms5;
	kCheckButton chkms6;
	kCheckButton chkmon;	// 1 x RecMon
	kCheckButton chksel;	// 1 x VU select sw

	kCheckButton chkmute1;	// 6 Enable switches
	kCheckButton chkmute2;
	kCheckButton chkmute3;
	kCheckButton chkmute4;
	kCheckButton chkmute5;
	kCheckButton chkmute6;

	CColorCtrl<CFontCtrl<CStatic> > finlvu;	// vu data text out
	CColorCtrl<CFontCtrl<CStatic> > finrvu;	// vu data text out
	CColorCtrl<CFontCtrl<CStatic> > lval;	// peak hold left static ctrl
	CColorCtrl<CFontCtrl<CStatic> > rval;	// peak hold right static ctrl
	CColorCtrl<CFontCtrl<CStatic> > vusel;	// vu select text out


// for updating UI from plugin
	virtual void sync(int what=EVERYPM_ID);

	DECLARE_MESSAGE_MAP()
};

declare_effect_source(mx6);

#endif
