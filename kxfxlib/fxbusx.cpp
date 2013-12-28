// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov and Dmitry Gorelov, 2005-2008.
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
#include "fxbusx.h"
// effect source
#include "da_fxbusx.cpp"

int iFXBusXPlugin::request_microcode() // generally is called after init()
{
	strncpy(name, FXBusX_name, sizeof(name));

    dword can_k8_passthru=0;

    ikx->get_dword(KX_DWORD_CAN_K8_PASSTHRU,&can_k8_passthru);

    if(!can_k8_passthru)
    {
	 code = FXBusX_10k2_code;
     code_size = FXBusX_10k2_code_size;
     info_size = FXBusX_10k2_info_size;
    }
    else
    {
     code = FXBusX_10k8_code;
     code_size = FXBusX_10k8_code_size;
     info_size = FXBusX_10k8_info_size;
    }

    info = FXBusX_info;

	itramsize = 0;
	xtramsize = 0;
        
	return 0;
}

const char *iFXBusXPlugin::get_plugin_description(int id)
{
	plugin_description(FXBusX);
}
