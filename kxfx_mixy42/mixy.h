// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov, LeMury 2005.
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

////////////////////////////////////////////////////////////////
//mixy.h	VU Meter Example - Lemury 2005
////////////////////////////////////////////////////////////////

#ifndef _KX_MIXY_EFFECT_H
#define _KX_MIXY_EFFECT_H

#define MIXY_PARAMS_COUNT	47	// number of user-visible parameters

// Plugin definition
//------------------------------------
class iMixyPlugin : public iKXPlugin
{
public:
	virtual int request_microcode();
	virtual const char *get_plugin_description(int id);
	virtual int get_user_interface();
	virtual iKXPluginGUI *create_cp(kDialog *parent, kFile *mf);
	

	virtual int get_param_count(void) {return MIXY_PARAMS_COUNT;}
	virtual int describe_param(int, kx_fxparam_descr*); // this function is used for kX Automation and mixer-provided GUI panels
	virtual int set_defaults();
    virtual int set_param(int ndx, kxparam_t value);
    virtual int get_param(int ndx, kxparam_t *value);
    virtual int set_all_params(kxparam_t* values);
	virtual int save_plugin_settings(kSettings &);
	virtual int load_plugin_settings(kSettings &);

	char str_param[5][256];
  
	kxparam_t _params[MIXY_PARAMS_COUNT]; // place to store parameters inside of plugin
};

// Plugin GUI definition
//-----------------------------------------------
class iMixyPluginDlg : public CKXPluginGUI
{
public:
	iMixyPlugin *plugin;

   	// constructor:
	iMixyPluginDlg(iMixyPlugin *plg, kDialog *parent_,kFile *mf_);
	virtual iKXPlugin *get_plugin() { return plugin; };

        // desctructor
        ~iMixyPluginDlg();

	virtual void init();
	virtual void on_destroy();


	UINT_PTR timer_id;
	afx_msg void OnTimer(UINT_PTR);	
	int mode; // 0 - horizontal; 1 - vertical
	int vuleft[5];
	int vutop;
	float max_l[5],max_r[5];
	int turn_on(int what);
	int on_command(int,int);

    // peak meter settings from the skin
    int pm_offset,pm_width;
    float pm_min,pm_max;

    int pm_voffset,pm_height;
    float pm_vmin[5],pm_vmax[5];


// declare GUI Elements
	kPeak *vu[5];
	kButton p_reset[5];
	kSlider fdrvol[11];
	kSlider pan[10];
	kCheckButton monomix;
	kCheckButton mute[11];
	kCheckButton solo[8];
	kCheckButton stlink[5];
	kStatic vutextl[5];			// peak hold label
	kStatic vutextr[5];
	kStatic fdrtext[10];
	CFont smallfont;
	CEdit* edit[5];
	//kStatic image;

// for updating UI from plugin
	virtual void sync(int what=EVERYPM_ID);
	void on_mouse_r_up(kPoint ,int );
	void editor_change1(void);	
	void editor_change2(void);	
	void editor_change3(void);	
	void editor_change4(void);	
	void editor_change5(void);
	//void edit_changed(UINT nID);
	DECLARE_MESSAGE_MAP()


};

declare_effect_source(mixy);

#endif
