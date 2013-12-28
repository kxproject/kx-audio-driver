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


#ifndef PCI_H_
#define PCI_H_

// PCI Configuration space registers
// (note: linux-style naming convention for compatibility)
#define PCI_DEVID       0x0000
#define PCI_DEVCTRL     0x0004
#define PCI_CLASS       0x0008
#define PCI_HEADER      0x000C
#define PCI_SUBSYSID_R  0x002C
#define PCI_INTCTRL     0x003C
#define PCI_SUBSYSID_W  0x004C
#define PCI_REVISION_ID 0x0008

KX_API(int,pcibios_read_config_dword (byte bus,byte device_fn, byte where, dword *value));
KX_API(int, pcibios_write_config_dword (byte bus,byte device_fn, byte where, dword value));
KX_API(int, pcibios_read_config_byte(byte bus, byte device_fn,byte where, byte *value));
KX_API(int, pcibios_read_config_word (byte bus,byte device_fn, byte where, word *value));
KX_API(int, pcibios_write_config_byte (byte bus, byte device_fn,byte where, byte value));
KX_API(int, pcibios_write_config_word (byte bus, byte device_fn,byte where, word value));

#endif
