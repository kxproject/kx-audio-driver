// kX Driver
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


#ifndef TIMER_H_
#define TIMER_H_

void kx_timer_install(kx_hw *, struct kx_timer *, dword);
void kx_timer_uninstall(kx_hw *, struct kx_timer *);
void kx_timer_enable(kx_hw *, struct kx_timer *);
void kx_timer_disable(kx_hw *, struct kx_timer *);

// bit-field
#define TIMER_INSTALLED 		0x01
#define TIMER_UNINSTALLED 		0x02
#define TIMER_ACTIVE			0x80

#define TIMER_STOPPED 			0xffffffff	// a kind of delay

#endif 
