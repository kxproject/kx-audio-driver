// kX Driver Interface
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

#include "stdafx.h"
#include "vers.h"

#pragma warning(disable:4100)

// DirectKX API
int iKX::audio_close(int num)
{
 return -1;
}

int iKX::audio_stop(int num)
{
 return -1;
}

int iKX::audio_start(int num)
{
 return -1;
}

int iKX::audio_get_position(int num,int *pos)
{
 return -1;
}

int iKX::audio_set_parameter(int num, int ch,int param,dword value)
{
 return -1;
}

int iKX::audio_get_parameter(int num, int ch,int param,dword *value)
{
 return -1;
}

int iKX::audio_open(void *buffer, size_t bufsize,int usage,int sampling_rate)
{
 return -1;
}
