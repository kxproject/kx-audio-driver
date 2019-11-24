//
//  Ac97App.mm
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


#import "Ac97App.h"
#import "Ac97Controller.h"

@implementation Ac97App

//#define TESTING

- (void)awakeFromNib
{
    nItems = 0;
    ac97 = [[Ac97Controller alloc] initWithNibName:@"Ac97" bundle:nil];
    
    for (int i = 0; i < MAX_KX_DEVICES; i++)
    {
		iKX *kx = iKX::create(i);
		
		if (!kx)
			break;
		
		dword d;

		if (kx->get_dword(KX_DWORD_AC97_PRESENT, &d) || !d)
		{
			delete kx;
			continue;
		}

		NSTabViewItem *item = [[NSTabViewItem alloc] initWithIdentifier:[NSNumber numberWithInt:i]];

		[item setLabel:[NSString stringWithCString:kx->get_device_name() encoding:NSASCIIStringEncoding]];
		[item setView:[ac97 view]];

		[self loadState:kx];	

		delete kx;

		[tabView addTabViewItem:item];
		[item release];
        nItems++;
    }
	
    if (![tabView numberOfTabViewItems])
    {
#ifdef TESTING
		NSTabViewItem *item = [[NSTabViewItem alloc] initWithIdentifier:[NSNumber numberWithInt:-1]];
		
		[item setLabel:@"Testing"];
		[item setView:[ac97 view]];
		[tabView addTabViewItem:item];
		[item release];
#else
		NSRunCriticalAlertPanel(@"Error", @"No kX AC97 devices found!", @"Close", nil, nil);
		[NSApp terminate:nil];
#endif
    }
}

- (void)tabView:(NSTabView *)tabView didSelectTabViewItem:(NSTabViewItem *)item
{
    if (ac97)
		[ac97 setDevice:[[item identifier] intValue]];
}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender
{
//    for (NSTabViewItem *item in [tabView tabViewItems])
//    {
//        int device = [[item identifier] intValue];
//
//        if ([ac97 kx] && ([ac97 kx]->get_device_num() == device))
//            [self saveState:[ac97 kx]];
//        else
//        {
//            iKX *kx = iKX::create(device);
//            if (kx)
//            {
//                [self saveState:kx];
//                delete kx;
//            }
//        }
//    }
    for (Byte i = 0; i < nItems; i++)
    {
        if ([ac97 kx] && ([ac97 kx]->get_device_num() == i))
            [self saveState:[ac97 kx]];
        else
        {
            iKX *kx = iKX::create(i);
            if (kx)
            {
                [self saveState:kx];
                delete kx;
            }
        }
    }
	
    return NSTerminateNow;
}

-(BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication
{
    return YES;
}

- (void)dealloc
{
    [ac97 release];
    [super dealloc];
}

- (void)loadState:(iKX *)kx
{
    NSDictionary *card = [[NSUserDefaults standardUserDefaults] dictionaryForKey:[NSString stringWithCString:kx->get_device_name() encoding:NSASCIIStringEncoding]];
	
    if (!card)
		return;
    
    NSDictionary *regs = [ac97 registers];

    for (NSString *key in [regs allKeys])
        kx->ac97_write([regs[key][0] unsignedCharValue], [[card objectForKey:key] intValue]);
}

- (void)saveState:(iKX *)kx
{
    NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
    NSDictionary *regs = [ac97 registers];
    NSMutableDictionary *card = [NSMutableDictionary dictionaryWithCapacity:[regs count]];

    for (NSString *key in [regs allKeys])
    {
        word val;

        kx->ac97_read([regs[key][0] unsignedCharValue], &val);

        [card setValue:[NSNumber numberWithInt:val] forKey:key];
    }

    [prefs setObject:card forKey:[NSString stringWithCString:kx->get_device_name() encoding:NSASCIIStringEncoding]];
    [prefs synchronize];
}

@end
