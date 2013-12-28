// Copyright 1999, 2000 Creative Labs, Inc. 
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

#ifndef KX_ecard_H_GNU
#define KX_ecard_H_GNU

// ecard.h
// the same legal/copyright stuff applies

// In A1 Silicon, these bits are in the HC register
#define HOOKN_BIT   	HCFG_GPOUT2_K1
#define HANDN_BIT   	HCFG_GPOUT1_K1
#define PULSEN_BIT  	HCFG_GPOUT0_K1

#define EC_GDI1 	HCFG_GPINPUT1_K1
#define EC_GDI0 	HCFG_GPINPUT0_K1

#define EC_NUM_CONTROL_BITS 	20

#define EC_AC3_DATA_SELN  0x0001
#define EC_EE_DATA_SEL    0x0002
#define EC_EE_CNTRL_SELN  0x0004
#define EC_EECLK          0x0008
#define EC_EECS           0x0010
#define EC_EESDO          0x0020
#define EC_TRIM_CSN		  0x0040
#define EC_TRIM_SCLK	  0x0080
#define EC_TRIM_SDATA	  0x0100
#define EC_TRIM_MUTEN	  0x0200
#define EC_ADCCAL	  0x0400
#define EC_ADCRSTN	  0x0800
#define EC_DACCAL	  0x1000
#define EC_DACMUTEN	  0x2000
#define EC_LEDN		  0x4000

#define KX_EC_SPDIF1_MASK	(0x3 << KX_EC_SPDIF1_SHIFT)
#define KX_EC_SPDIF0_MASK	(0x7 << KX_EC_SPDIF0_SHIFT)

#define EC_CURRENT_PROM_VERSION 0x01 

#define EC_EEPROM_SIZE	        0x40 // ECARD EEPROM has 64 16-bit words 

// Addresses for special values stored in to EEPROM
#define EC_PROM_VERSION_ADDR	0x20	// Address of the current prom version
#define EC_BOARDREV0_ADDR	0x21	// LSW of board rev
#define EC_BOARDREV1_ADDR 	0x22	// MSW of board rev 

#define EC_LAST_PROMFILE_ADDR	0x2f

#define EC_SERIALNUM_ADD	0x30	
// First word of serial number.  The number
// can be up to 30 characters in length
// and is stored as a NULL-terminated
// ASCII string.  Any unused bytes must be
// filled with zeros

#define EC_CHECKSUM_ADDR	0x3f    // Location at which checksum is stored

// Most of this stuff is pretty self-evident.  According to the hardware 
// dudes, we need to leave the ADCCAL bit low in order to avoid a DC 
// offset problem.  Weird.

#define EC_RAW_RUN_MODE	(EC_DACMUTEN | EC_ADCRSTN | EC_TRIM_MUTEN | EC_TRIM_CSN | EC_AC3_DATA_SELN)
// note: EC_AC3_DATA_SELN is actually undocumented...

#define EC_DEFAULT_ADC_GAIN   0xC4C4   

#define HC_EA 0x01L
// and what's this?!! FIXME

#endif
