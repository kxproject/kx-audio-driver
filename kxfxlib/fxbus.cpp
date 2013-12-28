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

// effect class
#include "fxbus.h"
// effect source
#include "da_fxbus.cpp"

int iFXBusPlugin::request_microcode() // generally is called after init()
{
	strncpy(name, fxbus_name, sizeof(name));

	info = fxbus_info;
	code = fxbus_code;

	dword is_10k2=0;
        ikx->get_dword(KX_DWORD_IS_10K2,&is_10k2);
        if(is_10k2)
	  info_size = sizeof(fxbus_info);
	else
	  info_size = sizeof(fxbus_info)/2;

	code_size = 0;
	itramsize = 0;
	xtramsize = 0;
        
	return 0;
}

const char *iFXBusPlugin::get_plugin_description(int id)
{
	plugin_description(fxbus);
}

