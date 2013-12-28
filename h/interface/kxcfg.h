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

// kxcfg.h
// -------
// kX settings flags
// -------
// do not include this file directly, use 'kxapi.h' instead


#ifndef _KX_CFGSET_H
#define _KX_CFGSET_H

        // kx_saved_flag
        #define KX_SAVED_ROUTING 		0x1
        #define KX_SAVED_AMOUNT 		0x2
        #define KX_SAVED_ASIO 			0x4
        #define KX_SAVED_BUFFERS 		0x8
        #define KX_SAVED_MIXER 			0x10
        #define KX_SAVED_AC97 			0x20
        #define KX_SAVED_SOUNDFONTS 		0x40
        #define KX_SAVED_HWPARAMS 		0x80
        #define KX_SAVED_DSP			0x100
        #define KX_SAVED_NO_VERSION		0x200
        #define KX_SAVED_NO_CARDNAME		0x400
        #define KX_SAVED_AUTOMATION		0x800
        #define KX_SAVED_ADDONS			0x1000

        #ifndef KX_SAVED_ALL
         #define KX_SAVED_ALL	(KX_SAVED_ROUTING|KX_SAVED_AMOUNT|KX_SAVED_ASIO|KX_SAVED_BUFFERS|KX_SAVED_MIXER|KX_SAVED_AC97|KX_SAVED_SOUNDFONTS|KX_SAVED_HWPARAMS|KX_SAVED_DSP|KX_SAVED_AUTOMATION|KX_SAVED_ADDONS)
        #endif

#endif
