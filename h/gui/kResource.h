// kX Resource Control
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

#if !defined(kGUI_FILE_H__52789AD8_9B95_49F3_9BAD_3EA749987D3F__INCLUDED_)
#define kGUI_FILE_H__52789AD8_9B95_49F3_9BAD_3EA749987D3F__INCLUDED_

#pragma once

// three internal structures/classes
class CArchFile;
struct _kxgui_ini_sections;
struct _image_cache_t;

class kCLASS_TYPE kFile
{
public:
	kFile();
	~kFile();
	int init(); // opens default skin; don't call if you plan to call set_skin afterwards
	int close();

	int set_skin(const TCHAR *fname); // sets new skin
        void set_language(LANGID lang);
        LANGID get_language();

	void *load_data(const TCHAR *fname,size_t *size,int *error); // caller should call free(); returns 0 if failed
        HBITMAP load_image(const TCHAR *fname); 			

        int get_profile(const TCHAR *section,const TCHAR *key,TCHAR *buff,int bufsize);
        kString get_profile(const TCHAR *section,const TCHAR *key);
        int get_profile(const TCHAR *section,const TCHAR *key,const TCHAR *default_,kString &ret_string);

        void attach_skin(kFile *n);
        void detach_skin();
        void set_attach_priority(int n) { priority=n&1; }; // if n=0 (default) 'attached' is searched first

        kString get_skin_name();
        kString get_skin_guid();
        kString get_skin_file();

        int get_full_skin_path(const TCHAR *file,TCHAR *out);

private:
	int load_inis();
        kFile *next;
        LANGID current_language;

        kString skin_file;
	kString skin_guid;
	kString skin_name;

	struct _kxgui_ini_sections *sections;
	int priority;
	
	class CArchFile *af; // zip/rar handler if zip/rar are used

	void *_load_from_arch(const TCHAR *fname,size_t *size,int *error);
        void *_load_from_file(const TCHAR *fname,size_t *size,int *error);

	// image cache
	void add_image(const TCHAR *fname,HBITMAP bm);
	void flush_cache(void);
	int find_image(const TCHAR *fname,HBITMAP *bm);

        struct _image_cache_t *cur_image_cache;
};

#endif // !defined(kGUI_FILE_H__52789AD8_9B95_49F3_9BAD_3EA749987D3F__INCLUDED_)
