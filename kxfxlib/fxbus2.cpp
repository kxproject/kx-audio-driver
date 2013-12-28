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
#include "fxbus2.h"
// effect source
#include "da_fxbus2.cpp"


int iFXBus2Plugin::request_microcode() // generally is called after init()
{
	if(ikx->get_dsp())
	{
	strncpy(name, fxbus2_name, sizeof(name));

	info = fxbus2_info;
	code = fxbus2_code;

	info_size = sizeof(fxbus2_info);

	code_size = 0;
	itramsize = 0;
	xtramsize = 0;
        
	return 0;
	}
	else return -1;
}

const char *iFXBus2Plugin::get_plugin_description(int id)
{
	plugin_description(fxbus2);
}

