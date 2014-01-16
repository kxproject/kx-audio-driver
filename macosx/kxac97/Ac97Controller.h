//
//  Ac97Controller.h
//  kX AC97 control utility
//  www.kxproject.com
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


#import <Cocoa/Cocoa.h>
#import "RoundedBox.h"

#include "interface/kxapi.h"


@interface Ac97Controller : NSViewController
{
    IBOutlet RoundedBox	*cdBox, *videoBox, *auxBox, *lineBox, *dacBox, *masterBox,
			 *adcBox, *spkBox, *micBox, *phoneBox;

    IBOutlet NSSlider *cdL, *cdR, *videoL, *videoR, *auxL, *auxR, *lineL, *lineR,
					  *dacL, *dacR, *masterL, *masterR, *adcL, *adcR,
					  *spkSlider, *micSlider, *phoneSlider;

    BOOL cdLinked, videoLinked, auxLinked, lineLinked,
		 dacLinked, masterLinked, adcLinked;

    word regMaster, regPCBeep, regPhone, regMic, regLine, regCd,
		 regVideo, regAux, regDac, regRecSel, regRec, regGp;

    iKX *kx;
}

- (void)setDevice:(int)value;

@property (readonly) iKX *kx;
@property (readonly) NSDictionary *registers;

@end
