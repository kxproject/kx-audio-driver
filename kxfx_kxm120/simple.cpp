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

#include "stdafx.h"

// --------------- the above should be a 'standard' beginning of any .cpp file

// all effect classes:
#include "kxm120.h"

kxplugin_list_t plugin_list[]=
{
 { kxm120_name, kxm120_guid },
};

// the only exported function
// --------------------------

declare_publish_plugins
{
 switch(command)
 {
  case KXPLUGIN_GET_COUNT:
  	{
         *ret=sizeof(plugin_list)/sizeof(kxplugin_list_t);
  	}
  	return 0;
  case KXPLUGIN_GET_VERSION:
  	*ret=KXPLUGIN_VERSION;
  	return 0;
  case KXPLUGIN_GET_FXLIB_VER:
  	*ret=KX_VERSION_DWORD;
  	return 0;
  case KXPLUGIN_GET_NAME:
  	if(param<sizeof(plugin_list)/sizeof(kxplugin_list_t))
  	{
  	 strncpy((char *)ret,plugin_list[param].name,KX_MAX_STRING);
  	 return 0;
  	}
  	break;
  case KXPLUGIN_GET_GUID:
  	if(param<sizeof(plugin_list)/sizeof(kxplugin_list_t))
  	{
  	 strncpy((char *)ret,plugin_list[param].guid,KX_MAX_STRING);
  	 return 0;
  	}
  	break;	
  case KXPLUGIN_INSTANTIATE:
       if(param<sizeof(plugin_list)/sizeof(kxplugin_list_t))
       {

        // to avoid numbering stuff
         instantiate_plugin(kxm120,iKXM120Plugin);
       }
       break;
 }
 *ret=0;
 return -1;
}
