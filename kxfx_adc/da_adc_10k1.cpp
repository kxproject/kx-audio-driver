// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov and Max Mikhailov, 2001-2004.
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

// kX DSP Generated file
// 10kX microcode
// Patch name: 'adc'

char *adc_10k1_copyright="Copyright (c) LeMury, 2004-2005. All rights reserved.";
char *adc_10k1_engine="kX";
char *adc_10k1_comment="AC97 Analog Inputs (10k2 UDA enabled) v2.0 $nobypass";
char *adc_10k1_created="03/01/2005";

char *adc_10k1_guid="9a35a629-3467-4fbe-bab9-99fa62fc2a73";	// unique guid
// use next guid to let AC97 overwrite k2lt
// char *adc_10k1_guid="f02fc662-a8b0-45fa-bde5-459c23549314";

char *adc_10k1_name="AC97";
int adc_10k1_itramsize=0,adc_10k1_xtramsize=0;


dsp_register_info adc_10k1_info[]={
	{ "AC97_Left",0x8000,0x8,0xffff,0x0 },
	{ "AC97_Right",0x8001,0x8,0xffff,0x0 },
};

dsp_code adc_10k1_code[]={
	{ 0x0,0x8000,0x2040,KX_IN(0x0),C_20000000 },	// ac97 left
	{ 0x0,0x8001,0x2040,KX_IN(0x1),C_20000000 },	// ac97 right
};

