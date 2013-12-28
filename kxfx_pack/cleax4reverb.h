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


#ifndef _KX_CLEAX4REVERB_H
#define _KX_CLEAX4REVERB_H

#define CLEAX4Reverb_PARAMS_COUNT	1
#define CLEAX4Reverb_MAX_PRESET		38	// 0..37

// Plugin definition
class iCLEAX4ReverbPlugin: public iKXPlugin
{
public:
 virtual const char *get_plugin_description(int id);			
 virtual int request_microcode();				

 virtual int get_param_count(void) {return CLEAX4Reverb_PARAMS_COUNT;}
 int set_defaults();

 int preset;
 virtual int set_param(int ndx,kxparam_t value); // 0...count-1
 virtual int get_param(int ndx,kxparam_t *value);
 virtual int describe_param(int ndx, kx_fxparam_descr *descr);
 virtual const kxparam_t *get_plugin_presets();
};

declare_effect_source(CLEAX4Reverb);

#endif
