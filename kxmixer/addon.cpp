// kX Audio Driver API / kX Audio Driver Plugin Interface / kX Add-on Interface
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

#include "stdinc.h"
#include "addonmgr.h"

// manager-provided

iKXAddOn::iKXAddOn()	// should not call any m-> functions
{
 needed_menu_stage=0; 
 event_mask=0; // KXADDON_EVENT_MENU|KXADDON_EVENT_GENERIC
 menu_base=0;
}

iKXAddOn::~iKXAddOn()
{
 kx_manager=0;
}

int iKXAddOn::get_manager_version()
{
 return KXMANAGER_VERSION;
}

int iKXAddOn::check_versions(int mgr,int pm,int p)
{
	if(mgr!=KXMANAGER_VERSION && mgr!=0)
	 return -1;
	if(pm!=KXPLUGINMANAGER_VERSION && pm!=0)
	 return -2;
	if(p!=KXPLUGIN_VERSION && p!=0)
	 return -3;

	return 0;
}

int iKXAddOn::notify(int event,void *p1,void *p2) // provided by kX Manager
{
 if(kx_manager && kx_manager->addon_mgr)
  return kx_manager->addon_mgr->notify(this,event,p1,p2);
 else
  return -1;
}

int iKXAddOn::activate()
{
 return 0; // Ok
}
