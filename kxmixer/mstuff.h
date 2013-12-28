// kX Mixer / FX Control - MIDI Router Interface
// kX VSTi Interface
// INTERNAL SPECIFICATION
// Copyright (c) Eugene Gavrilov, Max Mikhailov, 2002-2005.
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


#ifndef _MSTUFF_H_
#define _MSTUFF_H_

#define CHANNEL_MASK			0x0000000f
#define EVENT_MASK			0x000000f0
#define INDEX_MASK			0x00007f00
#define VALUE_MASK			0x007f0000

#define NOTE_OFF         	0x80
#define NOTE_ON          	0x90
#define CONTROLCHANGE    	0xb0
#define PITCHBEND      		0xe0
#define KEY_AFTERTOUCH		0xa0
#define PROGRAM_CHANGE     	0xc0

#define getm_channel(msg) ((dword) ((msg) & CHANNEL_MASK))
#define getm_event(msg)   ((dword) ((msg) & EVENT_MASK))
#define getm_index(msg)  ((dword) (((msg) & INDEX_MASK) >> 8))
#define getm_value(msg)  ((dword) (((msg) & VALUE_MASK) >> 16))

#define setm_channel(msg, value) (msg = (dword) (((msg) & (~CHANNEL_MASK)) | (value)))
#define setm_event(msg, value)  (msg = (dword) (((msg) & (~EVENT_MASK)) | (value)))
#define setm_index(msg, value)  (msg = (dword) (((msg) & (~INDEX_MASK)) | ((value) << 8)))
#define setm_value(msg, value)  (msg = (dword) (((msg) & (~VALUE_MASK)) | ((value) << 16)))

#define MASK_7BITS	0x7f

#define FLAG_MSG_MASK					0x1c
#define FLAG_MSG_CHANNEL				0x04
#define FLAG_MSG_CHANNELEVENT			0x08
#define FLAG_MSG_CHANNELEVENTINDEX		0x10

#define getm_msg_to_compare(msg, flags) ((msg) & ((1 << ((flags) & FLAG_MSG_MASK)) - 1))

#define FLAG_MASK_MSB_VALUE_EXTRACT		0x01			
#define FLAG_MASK_LSB_VALUE_EXTRACT		0x02

#define FLAG_VALUE_VALUE				0x03 // both msb and lsb are in value bits of msg (note velocity, controller value etc.)
#define FLAG_VALUE_INDEX				0x00 // both are in index (note key etc.)
#define FLAG_VALUE_VALUEINDEX			0x01 // msb in value, lsb in index (like in pitchbend)
#define FLAG_VALUE_INDEXVALUE			0x02 // msb in index, lsb in value

#define getm_msb_value(msg, flags) (((msg) >> (8 << ((flags) & FLAG_MASK_MSB_VALUE_EXTRACT))) & MASK_7BITS)
#define getm_lsb_value(msg, flags) (((msg) >> (8 << (((flags) & FLAG_MASK_LSB_VALUE_EXTRACT) >> 1))) & MASK_7BITS)

#define FLAG_FUNCTION_MASK				0xf00
#define FLAG_FUNCTION_DEFAULT			0x000
#define FLAG_FUNCTION_RAW_DATA			0x100	// reserved (send raw midi msg to plugin)
#define FLAG_FUNCTION_BUTTON			0x200	// on -> max, off -> min
#define FLAG_FUNCTION_SWITCH			0x300	// first on -> max, next on -> min etc.

#define FLAG_CONVERT_MASK				0xf000	// reserved, will be used if we decide to use nonlinear convertions

#define FLAG_STATE_MASK					0x10000		// used when function is FLAG_FUNCTION_SWITCH
#define FLAG_STATE_RESERVED				0x7fff0000	// reserved, will be used if we decide to use 14 bit control pairs	

#define FLAGS_CONTROLCHANGE		(FLAG_MSG_CHANNELEVENTINDEX | FLAG_VALUE_VALUE)
#define FLAGS_PITCHBEND			(FLAG_MSG_CHANNELEVENT | FLAG_VALUE_VALUEINDEX)	
#define FLAGS_NOTE_VELOCITY		(FLAG_MSG_CHANNELEVENTINDEX | FLAG_VALUE_VALUE)
#define FLAGS_NOTE_KEY			(FLAG_MSG_CHANNELEVENT | FLAG_VALUE_INDEX)
#define FLAGS_NOTE_ON_TYPE1		(FLAG_MSG_CHANNELEVENTINDEX | FLAG_FUNCTION_BUTTON)
#define FLAGS_NOTE_ON_TYPE2		(FLAG_MSG_CHANNELEVENTINDEX | FLAG_FUNCTION_SWITCH)	
#define FLAGS_RAW			(FLAG_MSG_CHANNEL | FLAG_FUNCTION_RAW_DATA)

#define MSG_DISABLE		0x80000000	// reserved, temporary disable (but not remove) parameter
			
//.......................................................................................
// ~linear interpolate between two values (min, max)
// using 14 bit unsigned integer value
// where hv - high 7 bits of that value, lv - low 7 bits
// ! to interpolate with 7 bit value, pass it to both hv and lv (NOT hv = value; lv = 0;)
//
#pragma warning(disable: 4035)
_inline kxparam_t _interp_by14bv(kxparam_t min, kxparam_t max, kxparam_t hv, kxparam_t lv)
{
//again:
#if !defined(AMD64)
	_asm
	{	
		mov ecx, hv					
                shl ecx, 7                                                         
		mov eax, 0x2000 
                cmp eax, ecx                                                                   
                adc ecx, lv                                 
                shl	ecx, 2                         

		mov eax, max
		imul ecx
		mov lv, eax
		mov hv, edx
		
		neg ecx 
		add ecx, 0x10000
		mov eax, min
		imul ecx
		
		add eax, lv
		adc edx, hv

		cmp edx, -1
		adc eax, 0x7fff
		adc edx, 0

		shl edx, 16
		shr eax, 16
		add eax, edx
	}
//goto again;
#else
    return (kxparam_t) (min + ((((__int64) max - min) 
        * ((hv << 7) + (hv >> 6) + lv)) >> 14));
#endif
}
#pragma warning(default: 4035)

#endif // _MSTUFF_H_
