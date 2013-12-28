// kX E-DSP Control utility
// Copyright (c) Eugene Gavrilov, 2008-2014.
// www.kxproject.com
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

#ifndef _EDSPLIB_H_
#define _EDSPLIB_H_

#include "emu.h"

int is_fpga_programmed(iKX *ikx);
int is_dock_connected(iKX *ikx);
int is_dock_online(iKX *ikx);
int upload_card_firmware(iKX *ikx);
int upload_dock_firmware(iKX *ikx);
void load_defaults(iKX *ikx);
void print_status(iKX *ikx);

#endif
