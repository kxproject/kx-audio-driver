//
//  RoundedBox.m
//  RoundedBox
//
//  Created by Matt Gemmell on 01/11/2005.
//  Copyright 2006 Matt Gemmell. http://mattgemmell.com/
//
//  Permission to use this code:
//
//  Feel free to use this code in your software, either as-is or 
//  in a modified form. Either way, please include a credit in 
//  your software's "About" box or similar, mentioning at least 
//  my name (Matt Gemmell). A link to my site would be nice too.
//
//  Permission to redistribute this code:
//
//  You can redistribute this code, as long as you keep these 
//  comments. You can also redistribute modified versions of the 
//  code, as long as you add comments to say that you've made 
//  modifications (keeping these original comments too).
//
//  If you do use or redistribute this code, an email would be 
//  appreciated, just to let me know that people are finding my 
//  code useful. You can reach me at matt.gemmell@gmail.com

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

@interface RoundedBox : NSBox
{
    CGFloat borderWidth;
    NSColor *borderColor;
    NSColor *titleColor;
    NSColor *backgroundColor;
    NSMutableDictionary *titleAttrs;

    NSRect clickableArea;

    BOOL muted;
}

- (void)setMuted:(BOOL)value;

- (void)setDefaults;
- (NSBezierPath *)titlePathWithinRect:(NSRect)rect cornerRadius:(CGFloat)radius titleRect:(NSRect)titleRect;
- (void)setTitleColor:(NSColor *)newTitleColor;
- (void)setBackgroundColor:(NSColor *)newBackgroundColor;

@end
