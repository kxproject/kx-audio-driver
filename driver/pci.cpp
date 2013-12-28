// kX Driver
// Copyright (c) Eugene Gavrilov, 2001-2014.
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


#include "kx.h"

// NOTE: pcibios_XXX functions are implemented this way only in order to 
// maintain any (future) linux compatibility
// in no event should this code be considered as 'derived' from any linux source

#define CONFIG_CMD(bus, device_fn, where)   (0x80000000 | (bus << 16) | (device_fn << 8) | (where & ~3))
KX_API(int,pcibios_read_config_dword (byte bus,
        byte device_fn, byte where, dword *value))
{
    outpd(0xCF8,CONFIG_CMD(bus,device_fn,where));
    *value = inpd(0xCFC);
    outpd(0xCF8,0);
    return 1;
}

KX_API(int,pcibios_write_config_dword (byte bus,
        byte device_fn, byte where, dword value))
{
    outpd(0xCF8,CONFIG_CMD(bus,device_fn,where));
    outpd(0xCFC,value);
    outpd(0xCF8,0);
    return 1;
}

KX_API(int, pcibios_read_config_byte(byte bus, byte device_fn,
                               byte where, byte *value))
{
    outpd(0xCF8,CONFIG_CMD(bus,device_fn,where));
    *value = (byte)inp(0xCFC + (where&3));
    outpd(0xCF8,0);
    return 1;
}

KX_API(int, pcibios_read_config_word (byte bus,
    byte device_fn, byte where, word *value))
{
    outpd(0xCF8,CONFIG_CMD(bus,device_fn,where));
    *value = (word) inpw(0xCFC + (where&2));
    outpd(0xCF8,0);
    return 1;
}

KX_API(int, pcibios_write_config_byte (byte bus, byte device_fn,
                                 byte where, byte value))
{
    outpd(0xCF8,CONFIG_CMD(bus,device_fn,where));
    outp(0xCFC + (where&3),value);
    outpd(0xCF8,0);
    return 1;
}

KX_API(int, pcibios_write_config_word (byte bus, byte device_fn,
                                 byte where, word value))
{
    outpd(0xCF8,CONFIG_CMD(bus,device_fn,where));
    outpw(0xCFC + (where&2),value);
    outpd(0xCF8,0);
    return 1;
}

