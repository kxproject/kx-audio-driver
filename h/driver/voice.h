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


#ifndef _VOICE_H_
#define _VOICE_H_

void kx_setup_playback(kx_hw *hw,int num);
void kx_voice_stop(kx_hw *hw,int num);
void kx_voice_stop_multiple(kx_hw *hw,dword low,dword high);
void kx_voice_start(kx_hw *hw,int num);
void kx_voice_start_multiple(kx_hw *hw,dword low,dword high);
int kx_voice_alloc(kx_hw *hw, int usage);
void kx_voice_free(kx_hw *hw, int num);
void kx_voice_init_cache(kx_hw *hw,int i);

#endif
