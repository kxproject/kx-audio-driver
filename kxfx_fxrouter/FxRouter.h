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

#ifndef _KX_FXROUTER_EFFECT_H
#define _KX_FXROUTER_EFFECT_H

#include "SwapIndicator.h"
#include "LCD7x5DM.h"

#define FXROUTER_PARAMS_COUNT	14

// Plugin definition
class iFxRouterPlugin : public iKXPlugin
{
public:
	virtual int request_microcode();
	virtual const char *get_plugin_description(int id);
	
	virtual iKXPluginGUI *create_cp(kDialog *parent, kFile *mf);
	virtual int get_user_interface();

	virtual int get_param_count(void) {return FXROUTER_PARAMS_COUNT;}
	virtual int describe_param(int, kx_fxparam_descr*);
	virtual int set_defaults();
	virtual int set_param(int ndx, kxparam_t value);
	virtual int get_param(int ndx, kxparam_t *value);
	virtual int set_all_params(kxparam_t* values);
	virtual const kxparam_t *get_plugin_presets();
	
	virtual int init();
	virtual int save_plugin_settings(kSettings &);
	virtual int load_plugin_settings(kSettings &);
	void write_microcode();


	CString strDefault[FXROUTER_PARAMS_COUNT/2];
	CString strCurrent[FXROUTER_PARAMS_COUNT/2];

	kxparam_t _params[FXROUTER_PARAMS_COUNT];
};

// Plugin GUI definition
class iFxRouterPluginDlg : public CKXPluginGUI
{
public:
	iFxRouterPlugin *plugin;

	kFile FxRouter_skin;
	kFile *kxmixer_skin;

    iFxRouterPluginDlg(iFxRouterPlugin *plg, kDialog *parent_, kFile *mf_) : CKXPluginGUI(parent_,mf_) {plugin = plg; kxmixer_skin=mf_;};

	virtual void init();
	virtual iKXPlugin *get_plugin() { return plugin; };

	kCheckButton btnBypass[7];	
	CSwapIndicator si[7];
	CLCD7x5DM lcd[7];

	virtual void sync(int what=EVERYPM_ID);	
	int	on_command(int,int); 

	afx_msg void OnSwap( UINT id, NMHDR * pnmhdr, LRESULT * pResult );
	afx_msg void OnChange( UINT id, NMHDR * pnmhdr, LRESULT * pResult );
	afx_msg void OnContextMenu( CWnd* pWnd, CPoint pos);

	DECLARE_MESSAGE_MAP()
};

declare_effect_source(FxRouter);

#endif
