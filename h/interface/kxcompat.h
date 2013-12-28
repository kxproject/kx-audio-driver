// kX SDK:
// kX API, kX Audio Driver Interface, kX Plugin Manager API
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

// kxcompat.h
// -----
// pre-3538 defines (for compatibility)
// -----
// this file is NOT included by default

#ifndef _KXAPI_COMPAT_H
#define _KXAPI_COMPAT_H

   #define efx_code 			dsp_code
   #define efx_register_info 		dsp_register_info
   #define efx_microcode 		dsp_microcode
   #define EFX_MICROCODE_NOT_TRANSLATED	DSP_MICROCODE_NOT_TRANSLATED
   #define EFX_REG_NOT_TRANSLATED 	DSP_REG_NOT_TRANSLATED

   #define set_efx_register 		set_dsp_register
   #define get_efx_register 		get_dsp_register

   #define VALID_Z			VALID_R
   #define VALID_W			VALID_A

#endif
