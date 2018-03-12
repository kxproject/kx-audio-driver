//
//  Ac97Controller.m
//  kX AC97 control utility
//  https://github.com/kxproject/ (previously www.kxproject.com)
//
//  Created by Dmitry P. Gorelov on 30.07.09.
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


#import "Ac97Controller.h"
#import "Ac97Macro.h"

@implementation Ac97Controller

@synthesize kx;
@synthesize registers;

static NSArray *controls = NULL;

- (void)awakeFromNib
{
    if (!controls)
        controls = [@[
            @"masterMuted",	@"masterLeft",	@"masterRight",
            @"lineMuted",	@"lineLeft",	@"lineRight",
            @"cdMuted",		@"cdLeft",		@"cdRight",
            @"videoMuted",	@"videoLeft",	@"videoRight",
            @"auxMuted",	@"auxLeft",		@"auxRight",
            @"dacMuted",	@"dacLeft",		@"dacRight",
            @"adcMuted",	@"adcLeft",		@"adcRight",
            @"micMuted",	@"mic",			@"micAmp",
            @"phoneMuted",	@"phone",
            @"spkMuted",	@"spk",
            @"rsLeft",		@"rsRight",		@"digLB"] retain];

    registers = [@{
        @"Master":  @[@AC97_REG_MASTER_VOL,	[NSValue valueWithPointer: &regMaster]],
        @"PCBeep":  @[@AC97_REG_PC_BEEP_VOL,[NSValue valueWithPointer: &regPCBeep]],
        @"Phone":   @[@AC97_REG_PHONE_VOL,	[NSValue valueWithPointer: &regPhone]],
        @"Mic":     @[@AC97_REG_MIC_VOL,	[NSValue valueWithPointer: &regMic]],
        @"Line":    @[@AC97_REG_LINE_VOL,   [NSValue valueWithPointer: &regLine]],
        @"Cd":      @[@AC97_REG_CD_VOL,		[NSValue valueWithPointer: &regCd]],
        @"Video":   @[@AC97_REG_VIDEO_VOL,	[NSValue valueWithPointer: &regVideo]],
        @"Aux":     @[@AC97_REG_AUX_VOL,	[NSValue valueWithPointer: &regAux]],
        @"Dac":     @[@AC97_REG_PCM_VOL,	[NSValue valueWithPointer: &regDac]],
        @"RecSel":  @[@AC97_REG_REC_SELECT,	[NSValue valueWithPointer: &regRecSel]],
        @"Rec":     @[@AC97_REG_REC_GAIN,   [NSValue valueWithPointer: &regRec]],
        @"Gp":      @[@AC97_REG_GENERAL,    [NSValue valueWithPointer: &regGp]]} retain];
    
    // bind mute boxes
    //
    BIND_MUTED(cd);
    BIND_MUTED(video);
    BIND_MUTED(aux);
    BIND_MUTED(line);
    BIND_MUTED(dac);
    BIND_MUTED(master);
    BIND_MUTED(adc);
    BIND_MUTED(spk);
    BIND_MUTED(mic);
    BIND_MUTED(phone);
}

- (word)float2reg:(NSSlider *)slider :(CGFloat)value :(BOOL)inv
{
    CGFloat v = (inv) ? (CGFloat)[slider maxValue] - value : value;

	return (word)(v * ([slider numberOfTickMarks] - 1) /
			([slider maxValue] - [slider minValue]));
}

- (CGFloat)reg2float:(NSSlider *)slider :(word)reg :(word)mask :(int)shift :(BOOL)inv
{
    CGFloat v = (CGFloat)(([slider maxValue] - [slider minValue]) * ((reg >> shift) & mask) /
			([slider numberOfTickMarks] - 1));
	
    return (inv) ? (CGFloat)[slider maxValue] - v : v; 
}

- (void)set_reg:(word &)reg :(word)mask :(word)bits :(byte)regnum
{
    reg = (reg & mask) | bits;
    
    if (kx)
		kx->ac97_write(regnum, reg);
}

IMPLEMENT_PAIR(cd,		Cd,		regCd,	    0x1f, YES, AC97_REG_CD_VOL)
IMPLEMENT_PAIR(video,	Video,	regVideo,   0x1f, YES, AC97_REG_VIDEO_VOL)
IMPLEMENT_PAIR(aux,		Aux,	regAux,	    0x1f, YES, AC97_REG_AUX_VOL)
IMPLEMENT_PAIR(line,	Line,	regLine,    0x1f, YES, AC97_REG_LINE_VOL)
IMPLEMENT_PAIR(dac,		Dac,	regDac,	    0x1f, YES, AC97_REG_PCM_VOL)
IMPLEMENT_PAIR(master,	Master,	regMaster,  0x3f, YES, AC97_REG_MASTER_VOL)

IMPLEMENT_SINGLE(spk,	Spk,	regPCBeep,  0x0f, 1, AC97_REG_PC_BEEP_VOL)
IMPLEMENT_SINGLE(mic,	Mic,	regMic,	    0x1f, 0, AC97_REG_MIC_VOL)
IMPLEMENT_SINGLE(phone,	Phone,	regPhone,   0x1f, 0, AC97_REG_PHONE_VOL)

BINARY_SWITCH(micAmp,	MicAmp,	regMic,	0x40, AC97_REG_MIC_VOL)
BINARY_SWITCH(digLB,	DigLB,	regGp,	0x80, AC97_REG_GENERAL)

// Record source accessors
//
- (int) rsLeft { return (regRecSel >> 8) & 0x07; }
- (int) rsRight { return regRecSel & 0x07; }

- (void)setRsLeft:(int)value
{
    if (adcLinked)
    {
		[self willChangeValueForKey:@"rsRight"];
		[self set_reg:regRecSel :0xf8f8 :value | (value << 8) :AC97_REG_REC_SELECT];
		[self didChangeValueForKey:@"rsRight"];
    }
    else
		[self set_reg:regRecSel :0xf8ff :value << 8 :AC97_REG_REC_SELECT];
}

- (void)setRsRight:(int)value
{
    if (adcLinked)
    {
		[self willChangeValueForKey:@"rsLeft"];
		[self set_reg:regRecSel :0xf8f8 :value | (value << 8) :AC97_REG_REC_SELECT];
		[self didChangeValueForKey:@"rsLeft"];
    }
    else
		[self set_reg:regRecSel :0xfff8 :value :AC97_REG_REC_SELECT];
}

#undef LINKED_RS
#define LINKED_RS \
if ([self rsLeft] != [self rsRight])\
[self setRsRight:[self rsLeft]]

IMPLEMENT_PAIR(adc, Adc, regRec, 0x0f, NO, AC97_REG_REC_GAIN)

- (void)setDevice:(int)value
{
    if (!kx || (kx->get_device_num() != value))
    {
		if (kx)
			delete kx;
		
		if (!(kx = iKX::create(value)))
			NSLog(@"iKX::create(%d) returned null", value);
		else
		{
			// synchronize with hardware and update UI
			//
			for (NSString *key in controls)
				[self willChangeValueForKey:key];

			for (NSArray *val in [registers allValues])
                kx->ac97_read([val[0] unsignedCharValue], (word*)[val[1] pointerValue]);
			
			for (NSString *key in controls)
				[self didChangeValueForKey:key];
		}
    }
	
    // update linked state
    //
    if ([cdL     floatValue] == [cdR     floatValue]) [self setCdLinked:     YES];
    if ([videoL	 floatValue] == [videoR  floatValue]) [self setVideoLinked:  YES];
    if ([auxL	 floatValue] == [auxR    floatValue]) [self setAuxLinked:    YES];
    if ([lineL	 floatValue] == [lineR   floatValue]) [self setLineLinked:   YES];
    if ([dacL	 floatValue] == [dacR    floatValue]) [self setDacLinked:    YES];
    if ([masterL floatValue] == [masterR floatValue]) [self setMasterLinked: YES];
	
    if (([adcL floatValue] == [adcR floatValue]) && ([self rsLeft] == [self rsRight]))
		[self setAdcLinked: YES];
}

- (void)dealloc
{
    [super dealloc];
	
    if (kx)
		delete kx;
}

@end
