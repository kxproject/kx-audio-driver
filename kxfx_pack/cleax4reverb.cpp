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
#include "cleax4reverb.h"
// effect source
#include "da_cleax4reverb.cpp"

declare_plugin_members(CLEAX4Reverb,iCLEAX4Reverb);

// MIDI control implementation
static kx_fxparam_descr _param_descr[CLEAX4Reverb_PARAMS_COUNT] = {
	{"Preset", KX_FXPARAM_SELECTION, 0, CLEAX4Reverb_MAX_PRESET},
};

int iCLEAX4ReverbPlugin::describe_param(int ndx, kx_fxparam_descr *descr)
{
	memcpy(descr, &_param_descr[ndx], sizeof kx_fxparam_descr); return 0;
}

int iCLEAX4ReverbPlugin::set_defaults() 
{
	return set_param(0,0);
}

#define P (kxparam_t)

static kxparam_t presets[CLEAX4Reverb_MAX_PRESET*2+1]=
{
 // don't include 'default' preset
 P "Standard",     0,
 P "Alley",     1,
 P "Arena",     2,
 P "Auditorium",     3,
 P "Ball Park",     4,
 P "Bathroom",     5,
 P "Carpeted Hallway",     6,
 P "Cathedral",     7,
 P "Cave",     8,
 P "Cavern",     9,
 P "Chapel",     10,
 P "Church",     11,
 P "City",     12,
 P "Concert Hall",     13,
 P "Dizzy",     14,
 P "Drugged",     15,
 P "Forest",     16,
 P "Garage",     17,
 P "Generic",     18,
 P "Hallway",     19,
 P "Hangar",     20,
 P "Jazz Room",     21,
 P "Living Room",     22,
 P "Mountains",     23,
 P "Opera House",     24,
 P "Padded Cell",     25,
 P "Parking Lot",     26,
 P "Plain",     27,
 P "Psychotic",     28,
 P "Quarry",     29,
 P "Room",     30,
 P "Sewer Pipe",     31,
 P "Small Room",     32,
 P "Stone Corridor",     33,
 P "Stone Room",     34,
 P "Studio",     35,
 P "Theatre",     36,
 P "Underwater",     37,
 NULL  // must be here
};
#undef P

const kxparam_t *iCLEAX4ReverbPlugin::get_plugin_presets()
{
 return presets;
}


// Plugin Implementation

int iCLEAX4ReverbPlugin::get_param(int ndx, kxparam_t *value) // ndx corresponds to parameter id
{
	*value = preset;
	return 0;
}

int iCLEAX4ReverbPlugin::set_param(int ndx, kxparam_t value)
{
	if(ndx==0)
	{
	 if(value>=0 && value<CLEAX4Reverb_MAX_PRESET)
	 {
	  preset=(int)value;
          // dynamic update here...

          if(info)
          {
           for(int j=0;j<2;j++)
           for(int i=0;i<((CLEAX4STATIC_SIZE<CLEAX4DYNAMIC_SIZE)?CLEAX4DYNAMIC_SIZE:CLEAX4STATIC_SIZE);i++)
           {
            if(i<CLEAX4DYNAMIC_SIZE)
                set_dsp_register(CLEAX4Reverb_dregister_presets[preset][i].num,CLEAX4Reverb_dregister_presets[preset][i].p);
            if(i<CLEAX4STATIC_SIZE)
                set_dsp_register(CLEAX4Reverb_register_presets[preset][i].num,CLEAX4Reverb_register_presets[preset][i].p);
           }
           return 0;           
          }
         }
         else return -2;
	}
	return -1;
}
