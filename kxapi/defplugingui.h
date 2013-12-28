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


#ifndef _iKX_PLUGINGUI_H
#define _iKX_PLUGINGUI_H

// ----------------------------------------
// default plugin GUI for microcode effects
// ----------------------------------------

#define MAX_FADERS	128

// Plugin GUI definition
class KX_CLASS_TYPE iPluginDlg : public CKXPluginGUI
{
public:
	iKXPlugin *plugin;

    	// constructor:
	iPluginDlg(iKXPlugin *plg, kDialog *parent_, kFile *mf_):CKXPluginGUI(parent_, mf_) {plugin = plg;};
	virtual void init();
	virtual iKXPlugin *get_plugin() { return plugin; };

	const char *get_class_name() { return "kXFXDefTweaker"; }; // don't change

	// declare GUI Elements

	// Controls
	kFader faders[MAX_FADERS];

    	// GUI-related
        // events:
	virtual void sync(int what=EVERYPM_ID);
};


#endif // _KX_PLUGINGUI_H
