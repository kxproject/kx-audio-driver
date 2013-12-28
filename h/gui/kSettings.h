// kX GUI / XML Settings API
// Copyright (c) Eugene Gavrilov, 2002-2005.
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

#if !defined(kGUI_SETTINGS_H__52789AD8_9B95_49F3_9BAD_3EA749987D3F__INCLUDED_)
#define kGUI_SETTINGS_H__52789AD8_9B95_49F3_9BAD_3EA749987D3F__INCLUDED_

// flag
#define SETTINGS_FROMFILE	0x0
#define SETTINGS_GUI		0x10		// ask for filename / kx_saved_flag
#define SETTINGS_AUTO		0x100		// use kxdefault.kx; don't ask anything
#define SETTINGS_MAY_NOT_EXIST 	0x200		// ignore any errors

#include "interface/kxcfg.h"

#ifndef _KX_NO_KGUI_CLASSES

typedef unsigned long dword;

/////////////////////////////////////////////////////////////////////////////
// kSettings

class kCLASS_TYPE kSettings
{
public:
	kSettings(const TCHAR *card_name_=NULL,const TCHAR *cfg_file_=NULL,unsigned int kx_saved_flag_=KX_SAVED_ALL);
	~kSettings();

	void set_flag(unsigned int fl) { flag=fl; };

        // dwords
	void write(const TCHAR *section,TCHAR *key,dword value,int subkey=0);
        int read(const TCHAR *section,TCHAR *key,dword *value,int subkey=0);
        // strings
        void write(const TCHAR *section,const TCHAR *key,TCHAR *value);
        int read(const TCHAR *section,const TCHAR *key,TCHAR *value,int max_value_size);

        // bypasses 'card_name' translation
        // dwords
        void write_abs(const TCHAR *section,const TCHAR *key,dword value,int subkey=0);
        int read_abs(const TCHAR *section,const TCHAR *key,dword *value,int subkey=0);
        // strings
        void write_abs(const TCHAR *section,const TCHAR *key,const TCHAR *value);
        int read_abs(const TCHAR *section,const TCHAR *key,TCHAR *value,int max_value_size);
        // binaries
        void write_bin_abs(const TCHAR *section,const TCHAR *key,void *mem,int size);
        int read_bin_abs(const TCHAR *section,const TCHAR *key,void *mem,int *size);

        int enum_abs(int ndx,const TCHAR *section,TCHAR *key,int max_keyname_size,TCHAR *value,int max_value_size);
        int delete_key_abs(const TCHAR *section,const TCHAR *key,int complete=0);
        int delete_key(const TCHAR *section, const TCHAR *key,int complete=0);

        int delete_value(const TCHAR *section, const TCHAR *value);
        int delete_value_abs(const TCHAR *section, const TCHAR *value);

private:
	#ifndef KX_MAX_STRING
	 #define KX_MAX_STRING 128
	#endif

	TCHAR card_name[KX_MAX_STRING];
	kString cfg_file;
	unsigned int flag;

	HKEY hkey;
	int mode;
	#define kSETTINGS_REGISTRY	1
	#define kSETTINGS_INI		2

	void translate_section(const TCHAR *section,kString *ret);
};

#endif

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_SETTINGS_H__52789AD8_9B95_49F3_9BAD_3EA749987D3F__INCLUDED_)
