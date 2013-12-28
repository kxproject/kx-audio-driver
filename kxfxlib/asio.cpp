// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov and LeMury, 2003-2005.
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

//asio.cpp -- ASIO recording inputs

#include "stdafx.h"
// --------------- the above should be a 'standard' beginning of any .cpp file
// effect class
#include "asio.h"

// effect source
#include "da_asio.cpp"
#include "da_asio51.cpp"

// Get DSP Microcode
//----------------------------------------------------
int iAsioPlugin::request_microcode() 
{

	is_51=0;
	is_10k2=0;
	
	// check card type
	ikx->get_dword(KX_DWORD_IS_10K2,&is_10k2);
	ikx->get_dword(KX_DWORD_IS_51,&is_51);

	// well, not elegant, but it works:)

	if ( (is_51)&&!(is_10k2) ) {
			publish_microcode(asio51);}

		else {publish_microcode(asio);}
			
	return 0;
}
//--------------------------------------------------
const char *iAsioPlugin::get_plugin_description(int id)
{
	plugin_description(asio);

}
//-------------------------------------