// kX SDK:
// kX API, kX Audio Driver Interface, kX Plugin Manager API, kX Manager API, kX Add-on API
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

// kxaddon.h
// -----
// kX Add-on API
// -----
// do not include this file directly, use 'kxapi.h' instead


#ifndef _iKX_ADD_ON_INTERFACE__H_
#define _iKX_ADD_ON_INTERFACE__H_

class KXMANAGER_CLASS_TYPE iKXAddOn
{
public:
#if defined(WIN32)	
	HINSTANCE instance; // DLL instance; freed automatically by iKX::delete_object
#endif
	
	iKXAddOn();	// should not call any m-> functions
                        // should fill-in addon_type,neede_menu_stage & event_mask fields
	virtual ~iKXAddOn();

    // these functions are provided by kX Manager:
    int get_manager_version();
    int check_versions(int mgr=KXMANAGER_VERSION,int pm=KXPLUGINMANAGER_VERSION,int p=KXPLUGIN_VERSION);
     // returns 0 if add-on is compatible
     // provide '0' instead of the particular version # if you don't use the corresponding API
    int notify(int event,void *p1,void *p2); // provided by kX Manager

    // these should be provided by the Add-On
    virtual int get_addon_version()=0;
    virtual int init(int p=-1)=0; // p=device_num for 'per-device' add-ons
    virtual int close()=0;
    virtual int menu(int menu_stage,kMenu *mnu)=0; // returns 0 if something was added/modified
    virtual int event(int event,void *p1,void *p2)=0;
     // should return '0' if event was processed; <0 otherwise
    virtual int activate(); // launch guid-based add-on; returns 0 if o.k. or !=0 to keep enumerating add-ons
                            // need to set KXADDON_EVENT_ACTIVATE to receive this event

    virtual int save_settings(int where,kSettings &cfg)=0;
    virtual int load_settings(int where,kSettings &cfg)=0;
    virtual int reset_settings(int where)=0;
    // where: KXADDON_PER_DEVICE or KXADDON_SYSTEM_WIDE

    int needed_menu_stage;	// constructor / init() should fill this with KXADDON_MENU_... flags
    int event_mask;		// constructor / init() should fill this with KXADDON_EVENT_... masks
    int addon_type;		// should be filled-in by iKXAddOn() constructor, -before- init()
    int menu_base;		// is filled-in by the manager. should be used by menu()
    			        // add-ons can have up to MAX_ADDON_MENU_ITEMS unique menu items
    #define MAX_ADDON_MENU_ITEMS	10

    iKXManager *kx_manager;
};

#define KXADDON_VERSION		0x2

// add-on menu stages:
#define KXADDON_MENU_TOP	0x1
#define KXADDON_MENU_SETTINGS	0x2
#define KXADDON_MENU_ADDONS	0x4
#define KXADDON_MENU_INTERNET	0x8
#define KXADDON_MENU_BOTTOM	0x10

// add-on event masks
#define KXADDON_EVENT_GENERIC	0x1	// 'generic' events
#define KXADDON_EVENT_MENU	0x2
#define KXADDON_EVENT_REMOTEIR	0x4
#define KXADDON_EVENT_WINMSG	0x8
#define KXADDON_EVENT_ACTIVATE  0x10
#define KXADDON_EVENT_POWER     0x20

// add-on 'generic' events:
#define IKX_DSP_CHANGED		2	// same notification, as iKXPlugin one (::notify(KX_DSP_CHANGED))
#define IKX_DEVICE_CHANGED  3
#define IKX_EDSP_CONFIG_CHANGED 4
// add-on 'menu' events:
//  p1: (int) menu ID [menu_base-based]
//  p2: undefined

// add-on 'remote_ir' events:
//  p1: (int) message
//  p2: undefined
//  should return '0' if message was processed

// add-on 'winmsg' events:
// p1: (int) msg
// p2: kx_win_message structure:
// should return '0' if message was processed

// add-on 'power' events:
// p1:
#define KXADDON_EVENT_POWER_SLEEP   1
#define KXADDON_EVENT_POWER_RESUME  2

#if defined(WIN32)
	typedef struct kx_win_message_t
		{
			WPARAM wparam;
			LPARAM lparam;
			LRESULT ret;
		}kx_win_message;
#endif

// add-on types:
#define KXADDON_PER_DEVICE	1
#define KXADDON_SYSTEM_WIDE	2

// kX AddOn publisher (should be exported by plugin DLL)
typedef int (KX_DEF_CALLBACK *kxaddon_publish_t)(int,uintptr_t,uintptr_t *);
#define declare_publish_addons extern "C" int KX_DEF_EXPORT KX_DEF_CALLBACK publish_addons(int command,uintptr_t param,uintptr_t *ret)

// defined commands; param is 0 or plugin id (0..get_count()-1)
#define KXADDON_GET_COUNT	0
#define KXADDON_INSTANTIATE	1
#define KXADDON_GET_NAME	2
#define KXADDON_GET_GUID	3
#define KXADDON_GET_VERSION	4
#define KXADDON_GET_LIB_VER	5	// for 'main' kX add-on package ONLY
#define KXADDON_INSTANTIATE_BY_GUID	6

#endif
