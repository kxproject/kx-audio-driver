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

#include "stdafx.h"
#include "test_addon.h"

// --------------- the above should be a 'standard' beginning of any add-on .cpp file

// Plugin list
typedef struct
{
 char *name;
 char *guid;
 iKXAddOn * (*create_addon)(void);
}kxaddon_list_t;

kxaddon_list_t addon_list[]=
{
 { "Test_Add-on", "bea42d0e-5073-4219-88cd-1687b69a8055", itest_addonAddOn::create }
};

// the only exported function
// --------------------------

declare_publish_addons
{
 switch(command)
 {
  case KXADDON_GET_COUNT:
  	{
         *ret=sizeof(addon_list)/sizeof(kxaddon_list_t);
  	}
  	return 0;
  case KXADDON_GET_VERSION:
  	*ret=KXADDON_VERSION;
  	return 0;
  case KXADDON_GET_LIB_VER:
  	*ret=KX_VERSION_DWORD;
  	return 0;
  case KXADDON_GET_NAME:
  	if(param<sizeof(addon_list)/sizeof(kxaddon_list_t))
  	{
  	 strncpy((char *)ret,addon_list[param].name,KX_MAX_STRING);
  	 return 0;
  	}
  	break;
  case KXADDON_GET_GUID:
  	if(param<sizeof(addon_list)/sizeof(kxaddon_list_t))
  	{
  	 strncpy((char *)ret,addon_list[param].guid,KX_MAX_STRING);
  	 return 0;
  	}
  	break;	
  case KXADDON_INSTANTIATE:
       if(param<sizeof(addon_list)/sizeof(kxaddon_list_t))
       {
        // to avoid numbering stuff
        *ret=(uintptr_t)addon_list[param].create_addon();
        return (*ret==0)?-4:0;
       }
       break;
  case KXADDON_INSTANTIATE_BY_GUID:
       {
       char *guid=(char *)param;

         for(int i=0;i<sizeof(addon_list)/sizeof(kxaddon_list_t);i++)
         {
          if(stricmp(addon_list[i].guid,guid)==0)
          {
           *ret=(uintptr_t)addon_list[i].create_addon();
           return (*ret==0)?-3:0;
          }
         }
       }
       break;
 }	
 *ret=0;
 return -1;
}
