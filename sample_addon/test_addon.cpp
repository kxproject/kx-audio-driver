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

#include "StdAfx.h"
#include "test_addon.h"

// --------------------------------------------------------
void dbgmsg(const TCHAR *format, ...)
{
	TCHAR buffer[4096], *pbuffer = buffer;
	va_list	args;
	
	va_start(args, format);
	pbuffer += _vsntprintf(pbuffer, sizeof(buffer) - 1, format, args);
	va_end(args);
	
	while ( pbuffer > buffer  &&  isspace(pbuffer[-1]) )
		*--pbuffer = '\0';
	
	*pbuffer++ = '\r';
	*pbuffer++ = '\n';
	*pbuffer   = '\0';
	
	OutputDebugString(buffer);
}	
// --------------------------------------------------------


int itest_addonAddOn::close()
{
	dbgmsg("addon: [close]                                            [%s]", strDebugID);
	return 0;
}

int itest_addonAddOn::event(int event,void *p1,void *p2)
{
	switch(event)
	{		
		case KXADDON_EVENT_GENERIC:
		{			
			dbgmsg("addon: [event - generic] [p1: 0x%p]                 [%s]", p1, strDebugID);			
			// IKX_DSP_CHANGED
			// IKX_DEVICE_CHANGED
		}
		break;
	
		case KXADDON_EVENT_MENU:
		{
			dbgmsg("addon: [event - menu] [p1: %02d]                          [%s]", (int)(uintptr_t)p1, strDebugID);
			//  p1: (int) menu ID [menu_base-based]
			//  p2: undefined

			int item = (int)(uintptr_t)p1;
			item -= menu_base;			
			
			switch(item)
			{
				case 1:					
					break;				
				return 0; // message processed
			}			
		}
		break;

		/*
		case KXADDON_EVENT_REMOTEIR:
		{			
			dbgmsg("addon: [event - RemoteIR] [p1: 0x%08X]                [%s]",(int)p1, strDebugID);		
			//  p1: (int) message
			//  p2: undefined
		}
		break;
		*/

		/*
		case KXADDON_EVENT_WINMSG:
		{
			dbgmsg("addon: [event - winmsg] [p1: 0x%08X] [p2: 0x%08X] [%s]",(int)p1, (int)p2,  strDebugID);
			// p1: (int) msg
			// p2: kx_win_message structure:
		}
		break;
		*/
	}
	return -1;
}

int itest_addonAddOn::init(int p)
{		
	if(check_versions())
		return -1;	

	// the following causes kx mixer to crash (works ok in save_settings() or later)
	
	int nDevices = 0;	
	for (nDevices=0; nDevices<MAX_KX_DEVICES; nDevices++)
	{
		if (kx_manager->get_ikx(nDevices) == (iKX*)NULL)		
			break;				
	}		
	dbgmsg("addon: [init] [p: %03d] [NumDevices: %02d]                   [%s]", p, nDevices, strDebugID);
	
	/*
	iKX ikx;
	//int nDevices = 0;
	while (ikx.init(nDevices) == 0)
	{		
		ikx.close();		
		nDevices++;		
	}
	dbgmsg("addon: [init] [p: %03d] [NumDevices: %02d]                   [%s]", p, nDevices, strDebugID);
	*/
	return 0;
}

int itest_addonAddOn::load_settings(int where, kSettings &cfg)
{
	dbgmsg("addon: [load_settings]                                    [%s]", strDebugID);
	return 0;
}

int itest_addonAddOn::menu(int menu_stage,kMenu *mnu)
{	
	switch(menu_stage)
	{
		case KXADDON_MENU_ADDONS:
			mnu->add(_T("Test Addon"),menu_base+1);
			break;		
		default:
			return 0; // mnu not changed		
	}
	return -1; // option added
}

int itest_addonAddOn::reset_settings(int where)
{
	dbgmsg("addon: [reset_settings]                                   [%s]", strDebugID);
	return 0;
}

int itest_addonAddOn::save_settings(int where,kSettings &cfg)
{	
	dbgmsg("addon: [save_settings]                                    [%s]", strDebugID);
	return 0;
}
