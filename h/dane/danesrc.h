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


#ifndef _DANE_SRC_H
#define _DANE_SRC_H

class iKXDaneSource : public iKXPlugin
{
public:
 virtual int init(const char *fname,iKX *ikx_);
 virtual int close();
 virtual const char *get_plugin_description(int id);
 virtual int request_microcode() { return 0; };

 char name_[KX_MAX_STRING];
 char guid[KX_MAX_STRING];
 char copyright[KX_MAX_STRING];
 char engine[KX_MAX_STRING];
 char created[KX_MAX_STRING];
 char comment[KX_MAX_STRING];
};

#endif
