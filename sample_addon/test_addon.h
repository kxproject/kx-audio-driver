// Sample Add-On
// Copyright (c) Eugene Gavrilov, 2001-2014.
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

// test_addon add-on

// --------------------------------------------------------
void dbgmsg(const TCHAR *format, ...);
// --------------------------------------------------------

class itest_addonAddOn : public iKXAddOn
{
public:
	itest_addonAddOn() : iKXAddOn()
	{
		//  generate guid to differentiate between addon instances (for debugging purposes)		
		iKX ikx;
		ikx.init(-1);
		ikx.generate_guid(&strDebugID[0]);
		ikx.close();

		needed_menu_stage=KXADDON_MENU_ADDONS;
		
		event_mask=KXADDON_EVENT_GENERIC|KXADDON_EVENT_MENU/*|KXADDON_EVENT_REMOTEIR|KXADDON_EVENT_WINMSG*/;
		
		addon_type=KXADDON_SYSTEM_WIDE;
		//addon_type=KXADDON_PER_DEVICE;
		
		dbgmsg("addon: [ctor] [Type: %s]                         [%s]", addon_type == KXADDON_PER_DEVICE ? _T(" Per Device") : _T("System Wide"), strDebugID);
	};
	
	virtual ~itest_addonAddOn()
	{		
		dbgmsg("addon: [dtor]                                             [%s]", strDebugID);				
	};


	TCHAR strDebugID[KX_MAX_STRING];  // used only in debug output (guid string)	
	
	int close();
	static iKXAddOn *create(void) { return (iKXAddOn *) (new itest_addonAddOn); };
	int event(int event,void *p1,void *p2);
	int get_addon_version() { return KXADDON_VERSION; };
	int init(int p);	
	int load_settings(int where,kSettings &cfg);	
	int menu(int menu_stage,kMenu *mnu);
	int reset_settings(int where);
	int save_settings(int where,kSettings &cfg);	
};
