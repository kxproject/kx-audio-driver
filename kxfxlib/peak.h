// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov, 2002-2004.
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

#ifndef _KX_PEAK_H
#define _KX_PEAK_H

// Plugin definition
class iPeakPlugin : public iKXPlugin
{
public:
	virtual int request_microcode();
	virtual const char *get_plugin_description(int id);
	virtual int get_user_interface();

	virtual iKXPluginGUI *create_cp(kDialog *parent,kFile *mf);
};

// Plugin GUI definition
class iPeakPluginDlg : public CKXPluginGUI
{
public:
	iPeakPlugin *plugin;

    	// constructor:
	iPeakPluginDlg(iPeakPlugin *plg, kDialog *parent_,kFile *mf_);
	virtual iKXPlugin *get_plugin() { return plugin; };

        // desctructor
        ~iPeakPluginDlg();

	virtual void init();

	UINT_PTR timer_id;
	kPeak *peak;

	int mode; // 0 - horizontal; 1 - vertical
	float max_l,max_r;

        // peak meter settings from the skin
        int pm_offset,pm_width;
        float pm_min,pm_max;

        int pm_voffset,pm_height;
        float pm_vmin,pm_vmax;

	virtual void on_destroy();

	kButton p_reset;

	afx_msg void OnTimer(UINT_PTR);

	// declare GUI Elements
	int turn_on(int what);

	int on_command(int,int);
	void on_mouse_r_up(kPoint ,int );
	
	DECLARE_MESSAGE_MAP()
};

declare_effect_source(peak);

#endif // _KX_PEAK_LT_H
