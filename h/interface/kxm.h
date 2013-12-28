// kX SDK:
// kX API, kX Audio Driver Interface, kX Plugin Manager API, kX Manager API
// Copyright (c) Eugene Gavrilov, 2001-2005.
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

// kxm.h
// -----
// kX Manager API
// (draft. interface is under development. don't use it)
// -----
// do not include this file directly, use 'kxapi.h' instead


#ifndef _iKX_MANAGER_INTERFACE__H_
#define _iKX_MANAGER_INTERFACE__H_

struct pluginparam;

class iKXNotifier;
class iKXAddOnManager;

class KXMANAGER_CLASS_TYPE iKXMidiParser
{
public:
	iKXMidiParser(iKXNotifier *);
	~iKXMidiParser();

	struct pluginparam* get_param(iKXPlugin* plugin, int ndx);

	void parse_cc(dword event);

	int event(int pgm_id, iKXPlugin *plg, int event);
        // int event: IKX_... [kxapi.h]

    	int save_settings(kSettings &cfg);
    	int restore_settings(kSettings &cfg); 
	
	struct pluginparam* add_param();
	void remove_param(struct pluginparam* param);
	
	void reset();

	#define MAX_PARAMS_COUNT	512	// an estimation [10k2-based]...
        struct pluginparam *ParserMap;
	struct pluginparam *ActiveParam; // active parameters list
	struct pluginparam *EmptyParam; // empty parameters list
private:
	iKXNotifier *notifier;
};

// Master class for controlling kX-compatible device/managers

class KXMANAGER_CLASS_TYPE iKXManager
{
public:
	iKXManager();
	~iKXManager();

        // methods

	int init(char *cmdline);
	void close(int force_restart=0);

    int launch(char *guid); // generic launch: either addon, plugin or built-in mixer applet
    int is_supported(char *guid); // returns 1 if yes

    void launch(int what,int device_=-1,uintptr_t p1=0xffffffff,uintptr_t p2=0xffffffff);
        #define LAUNCH_INFO	1
        #define LAUNCH_DSP	2
        #define LAUNCH_SPTEST	3
        #define LAUNCH_EDITOR	4	// p1: pgm_id (or '-1'); p2: (char *) external_file_name
        #define LAUNCH_ROUTER	5	// p1: where
        #define LAUNCH_SETTINGS	6
        #define LAUNCH_REMOTE	7
        #define LAUNCH_MIDI	8
        #define LAUNCH_MIXER	9	// p1: page #
        #define LAUNCH_ASIO	10
        #define LAUNCH_NOTIFY	11
        #define LAUNCH_SYNTH_CFG	12
        #define LAUNCH_IO_STATUS	13
        #define LAUNCH_COMPAT_CFG	14
        #define LAUNCH_HELP		15
        #define LAUNCH_P16V_ROUTER	16

    iKX *get_ikx(int device_=-1);
    iKXPluginManager *get_pm(int device_=-1);
    iKXNotifier *get_notifier(int device_=-1);
    iKXMidiParser *get_parser(int device_=-1);

    kFile *get_default_skin(void);

	int save_settings(int flag,kString *fname_=NULL,dword kx_saved_flag=KX_SAVED_ALL);
        int restore_settings(int flag,kString *fname_=NULL,dword kx_saved_flag=KX_SAVED_ALL);
	void reset_settings();

	int change_device(int new_device,kWindow *parent=NULL);
        // devices: 
        //          -1 -- current_device [re-select]
        //          -2 -- select via gui popup menu ('parent' will be the owner of the menu)
        //          -3 -- get from the registry [on start-up only]
        int get_current_device();

        void close_all_windows(); // closes all launched applet windows

        int create_devices_menu(kMenu *menu,int cur_dev=-1); // should already be created()
        	// similar to change_device(-2,this);
                // if cur_dev!=-1, mark the device 'cur_dev' with a checkbox

        int get_version(); // returns KXMANAGER_VERSION

	iKXAddOnManager *addon_mgr;

private:
	#define MAX_KX_DEVICES	12
	iKX *ikx_devices[MAX_KX_DEVICES];
	iKXPluginManager *plugin_managers[MAX_KX_DEVICES];
	iKXNotifier *notifiers[MAX_KX_DEVICES];
	iKXMidiParser *midi_parsers[MAX_KX_DEVICES];
	int current_device;
};

#define KXMANAGER_VERSION	0x1	// should be changed when fundamental modifications occur

#include "interface/kxmixer.h"

#endif
