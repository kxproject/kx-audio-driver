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


#import "RoundedBox.h"


@implementation RoundedBox

static NSImage *muteOn, *muteOff;

+ (void)initialize
{
    muteOn = [NSImage imageNamed:@"mute-on.tif"];
    muteOff = [NSImage imageNamed:@"mute-off.tif"];
}

- (void)setMuted:(BOOL)value
{
    muted = value;

    [self setBorderColor:(muted ? [NSColor lightGrayColor] : [NSColor grayColor])];
    [self setNeedsDisplay:YES];
}

- (void)mouseDown:(NSEvent *)theEvent
{
    if (NSPointInRect([self convertPoint:[theEvent locationInWindow]
                                fromView:nil],
                      clickableArea))
        [self setMuted:!muted];
}

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];

    if (self)
        [self setDefaults];

    return self;
}

- (void)dealloc
{
    [borderColor release];
    [titleColor release];
    [backgroundColor release];
    [titleAttrs release];

    [super dealloc];
}

- (void)setDefaults
{
    borderWidth = 2.0f;
    [self setBorderColor:(muted ? [NSColor lightGrayColor] : [NSColor grayColor])];
    [self setTitleColor:[NSColor whiteColor]];
    [self setBackgroundColor:[NSColor colorWithCalibratedWhite:0.9f alpha:1.0f]];
    [self setTitleFont:[NSFont boldSystemFontOfSize:[NSFont systemFontSizeForControlSize:NSSmallControlSize]]];
    
    // Set up text attributes for drawing
    NSMutableParagraphStyle *paragraphStyle;
    paragraphStyle = [[NSMutableParagraphStyle alloc] init];
    [paragraphStyle setParagraphStyle:[NSParagraphStyle defaultParagraphStyle]];
    [paragraphStyle setAlignment:NSLeftTextAlignment];
    [paragraphStyle setLineBreakMode:NSLineBreakByTruncatingTail];
    
    titleAttrs = [[NSMutableDictionary dictionaryWithObjectsAndKeys:
        [self titleFont], NSFontAttributeName,
        titleColor, NSForegroundColorAttributeName,
        [paragraphStyle autorelease], NSParagraphStyleAttributeName,
        nil] retain];
}

- (void)awakeFromNib
{
    [self setDefaults];
}

- (void)drawRect:(NSRect)rect
{
    // Construct rounded rect path
    NSRect boxRect = [self bounds];
    NSRect bgRect = boxRect;
    bgRect = NSInsetRect(boxRect, borderWidth / 2.0f, borderWidth / 2.0f);
    bgRect = NSIntegralRect(bgRect);
    bgRect.origin.x += 0.5f;
    bgRect.origin.y += 0.5f;
    CGFloat minX = NSMinX(bgRect);
    CGFloat midX = NSMidX(bgRect);
    CGFloat maxX = NSMaxX(bgRect);
    CGFloat minY = NSMinY(bgRect);
    CGFloat midY = NSMidY(bgRect);
    CGFloat maxY = NSMaxY(bgRect);
    CGFloat radius = 4.0f;
    NSBezierPath *bgPath = [NSBezierPath bezierPath];
    
    // Bottom edge and bottom-right curve
    [bgPath moveToPoint:NSMakePoint(midX, minY)];
    [bgPath appendBezierPathWithArcFromPoint:NSMakePoint(maxX, minY) 
                                     toPoint:NSMakePoint(maxX, midY) 
                                      radius:radius];
    
    // Right edge and top-right curve
    [bgPath appendBezierPathWithArcFromPoint:NSMakePoint(maxX, maxY) 
                                     toPoint:NSMakePoint(midX, maxY) 
                                      radius:radius];
    
    // Top edge and top-left curve
    [bgPath appendBezierPathWithArcFromPoint:NSMakePoint(minX, maxY) 
                                     toPoint:NSMakePoint(minX, midY) 
                                      radius:radius];
    
    // Left edge and bottom-left curve
    [bgPath appendBezierPathWithArcFromPoint:NSMakePoint(minX, minY) 
                                     toPoint:NSMakePoint(midX, minY) 
                                      radius:radius];
    [bgPath closePath];
    
    // Create drawing rectangle for title    
    CGFloat titleHInset = borderWidth + 4.0f;
    CGFloat titleVInset = borderWidth;
    NSSize titleSize = [[self title] sizeWithAttributes:titleAttrs];
    NSRect titleRect = NSMakeRect(boxRect.origin.x + titleHInset, 
                                  boxRect.origin.y + boxRect.size.height - titleSize.height - (titleVInset * 2.0f), 
                                  titleSize.width + borderWidth, 
                                  titleSize.height);
    titleRect.size.width = MIN(titleRect.size.width, boxRect.size.width - (2.0f * titleHInset));
    
    // Draw title background

    [borderColor set];
    [[self titlePathWithinRect:bgRect
                  cornerRadius:radius
                     titleRect:titleRect] fill];
    
    
    // Draw rounded rect around entire box

    if (borderWidth > 0.0) {
        [bgPath setLineWidth:borderWidth];
        [bgPath stroke];
    }

    // Draw title text
    [[self title] drawInRect:titleRect withAttributes:titleAttrs];
    
    // Draw mute checkbox
    NSImage *box = muted ? muteOn : muteOff;
    titleSize = [box size];
    titleRect = NSMakeRect(bgRect.origin.x + bgRect.size.width - titleSize.width - 5,
			   boxRect.origin.y + boxRect.size.height - titleSize.height - 5,
			   titleSize.width, titleSize.height);

    [box drawInRect:titleRect
           fromRect:NSZeroRect
          operation:NSCompositeSourceOver
           fraction:1.0f];
}


- (NSBezierPath *)titlePathWithinRect:(NSRect)rect cornerRadius:(CGFloat)radius titleRect:(NSRect)titleRect
{
    // Construct rounded rect path
    NSRect bgRect = rect;
    CGFloat minX = NSMinX(bgRect);
    CGFloat maxY = NSMaxY(bgRect);
    CGFloat minY = NSMinY(titleRect) - (maxY - (titleRect.origin.y + titleRect.size.height));

    clickableArea = NSMakeRect(minX, minY, bgRect.size.width, maxY - minY);

    NSBezierPath *path = [NSBezierPath bezierPath];
    
    [path moveToPoint:NSMakePoint(minX, minY)];
    
    // Draw full titlebar, since we're either set to always do so, or we don't have room for a short one.
    [path lineToPoint:NSMakePoint(NSMaxX(bgRect), minY)];
    [path appendBezierPathWithArcFromPoint:NSMakePoint(NSMaxX(bgRect), maxY) 
				   toPoint:NSMakePoint(NSMaxX(bgRect) - (bgRect.size.width / 2.0f), maxY) 
				    radius:radius];
    
    [path appendBezierPathWithArcFromPoint:NSMakePoint(minX, maxY) 
                                   toPoint:NSMakePoint(minX, minY) 
                                    radius:radius];
    [path closePath];
    return path;
}

- (void)setTitle:(NSString *)newTitle
{
    [super setTitle:newTitle];
    [self setNeedsDisplay:YES];
}

- (void)setBorderColor:(NSColor *)newBorderColor
{
    [newBorderColor retain];
    [borderColor release];
    borderColor = newBorderColor;
    [self setNeedsDisplay:YES];
}

- (void)setTitleColor:(NSColor *)newTitleColor
{
    [newTitleColor retain];
    [titleColor release];
    titleColor = newTitleColor;
    
    [titleAttrs setObject:titleColor forKey:NSForegroundColorAttributeName];
    
    [self setNeedsDisplay:YES];
}

- (void)setBackgroundColor:(NSColor *)newBackgroundColor
{
    [newBackgroundColor retain];
    [backgroundColor release];
    backgroundColor = newBackgroundColor;
}

@end
