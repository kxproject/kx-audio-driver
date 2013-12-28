// kX Driver / kX Driver Interface / kX Driver Add-On Library
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

#include "stdafx.h"

#include "sample.h"

int iSampleAddOn::menu(int menu_stage,kMenu *mnu)
{
 switch(menu_stage)
 {
  case KXADDON_MENU_TOP:
  	// mnu->add("- sample top menu",menu_base+1);
  	break;
  case KXADDON_MENU_SETTINGS:
  	// mnu->add("- sample settings item",menu_base+2);
  	break;
  case KXADDON_MENU_ADDONS:
  	// mnu->add("- sample addons item",menu_base+3);
  	break;
  case KXADDON_MENU_BOTTOM:
  	// mnu->add("- sample bottom item",menu_base+4);
  	break;
  case KXADDON_MENU_INTERNET:
  	// mnu->add("- sample internet item",menu_base+5);
  	break;
  default:
  	return 0; // mnu not changed
 }
 return -1; // option added
}

int iSampleAddOn::init(int p)
{
 if(check_versions())
  return -1;

 return 0;
}

int iSampleAddOn::close()
{
 return 0;
}

int iSampleAddOn::save_settings(int where,kSettings &cfg)
{
 return 0;
}

int iSampleAddOn::load_settings(int where,kSettings &cfg)
{
 return 0;
}

int iSampleAddOn::reset_settings(int where)
{
 return 0;
}


int iSampleAddOn::event(int event,void *p1,void *p2)
{
 switch(event)
 {
  case KXADDON_EVENT_MENU:
  	{
  	 intptr_t item=(intptr_t)p1; // menu_base-based
  	 item-=menu_base;

  	 switch(item)
  	 {
  	  case 1:
  	  case 2:
  	  case 3:
  	  case 4:
  	  case 5:
  	  	MessageBox(NULL,"Hello from Add-On","Info",MB_OK);
  	  	return 0; // message processed
  	 }
  	}
  	break;
  case KXADDON_EVENT_REMOTEIR:
  	{
  	 // MessageBox(NULL,"Remote IR","Add-On",MB_OK);
  	 return -1; // message processed
  	}
  	break;
 }

 return -1;
}
