//
//  Ac97Macro.h
//  kX AC97 control utility
//  www.kxproject.com
//
//  Created by Dmitry P. Gorelov on 27.08.09.
//  Copyright Dmitry Gorelov, 2009. All rights reserved.
//

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


// This macro implements accessor methods for simple
// binary flag mapped to specific hardware register
//
#define BINARY_SWITCH(name, Name, reg, mask, regnum) \
- (BOOL)name\
{\
    return (reg & mask) ? YES : NO;\
}\
- (void)set##Name:(BOOL)value\
{\
    [self set_reg:reg :(word)~mask :value ? mask :0 :regnum];\
}

// Macro for mute control. Mute bit is always MSB for all registers
//
#define IMPLEMENT_MUTED(name, Name, reg, regnum) \
BINARY_SWITCH(name##Muted, Name##Muted, reg, 0x8000, regnum)

// Macro for single (not stereo) volume register with
// mute control
//
#define IMPLEMENT_SINGLE(name, Name, reg, mask, shift, regnum) \
IMPLEMENT_MUTED(name, Name, reg, regnum)\
- (CGFloat)name\
{\
	return [self reg2float:name##Slider :reg :mask :shift :YES];\
}\
- (void)set##Name:(float)value\
{\
    [self set_reg:reg :0xffe0 :[self float2reg:name##Slider :value :YES] << shift :regnum];\
}

#define LINKED_RS

// Macro for pair of (linkable) volume registers with mute
// control. Link state has no hardware backend, so
// it is stored in <name>Linked variable of type 'BOOL'
//
#define IMPLEMENT_PAIR(name, Name, reg, mask, inv, regnum) \
IMPLEMENT_MUTED(name, Name, reg, regnum)\
- (CGFloat)name##Left\
{\
    return [self reg2float:name##L :reg :mask :8 :inv];\
}\
- (CGFloat)name##Right\
{\
    return [self reg2float:name##R :reg :mask :0 :inv];\
}\
- (void)set##Name##Left:(CGFloat)value\
{\
    if (name##Linked)\
    {\
		[self willChangeValueForKey:@#name "Right"];\
		word pos = [self float2reg:name##L :value :inv];\
		[self set_reg:reg :(word)(((~mask) & 0xff) | ((~mask) << 8)) :pos | (pos << 8) :regnum];\
		[self didChangeValueForKey:@#name "Right"];\
    }\
    else\
		[self set_reg:reg :(word)(((~mask) << 8) | 0xff) :[self float2reg:name##L :value :inv] << 8 :regnum];\
}\
- (void)set##Name##Right:(CGFloat)value\
{\
    if (name##Linked)\
    {\
		[self willChangeValueForKey:@#name "Left"];\
		word pos = [self float2reg:name##R :value :inv];\
		[self set_reg:reg :(word)(((~mask) & 0xff) | ((~mask) << 8)) :pos | (pos << 8) :regnum];\
		[self didChangeValueForKey:@#name "Left"];\
    }\
    else\
		[self set_reg:reg :(word)~mask :[self float2reg:name##R :value :inv] :regnum];\
}\
- (void)set##Name##Linked:(BOOL)value\
{\
    if(value)\
    {\
	if ([name##L floatValue] != [name##R floatValue])\
	    [self set##Name##Right:[name##L floatValue]];\
	    LINKED_RS;\
    }\
\
    name##Linked=value;\
}

// Macro for establishing bi-directional binding with mute value of
// RoundedBox control
//
#define BIND_MUTED(name) \
[self bind:@#name "Muted" toObject:name##Box withKeyPath:@"muted" options:nil];\
[name##Box bind:@"muted" toObject:self withKeyPath:@#name "Muted" options:nil]
