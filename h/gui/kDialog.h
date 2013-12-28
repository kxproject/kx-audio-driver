// kX GUI
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

#if !defined(kGUI_kDialog_H)
#define kGUI_kDialog_H

#pragma once

class kCLASS_TYPE kDialog : public kWindow
{
public:
	kDialog();	// standard constructor
	~kDialog(); 

	int create(kDialog *pParent_=NULL,int style=-1);
	int create(int idd,kDialog *parent_=NULL);

        // events
        virtual void on_ok() {};
        virtual void on_cancel() {};
        virtual int on_command(int wparam,int lparam);

        virtual void on_create();
        virtual void on_destroy();

        virtual void size_to_content(void);

        // operators
        virtual void init();

        virtual int do_modal(kDialog *parent_);
        virtual int notify(kWindow *wnd,int msg,int wparam,int lparam);

	virtual int is_moving() { return (k_state&kSTATE_MOVING)?1:0; };
	virtual void set_moving(int set=1) { if(set) k_state|=kSTATE_MOVING; else k_state&=~kSTATE_MOVING; };
	virtual int is_dragging() { return (k_state&kSTATE_DRAGGING)?1:0; };
	virtual void set_dragging(int set=1);

	virtual int on_hit_test(kPoint pnt);

	virtual int set_background(HBITMAP image,ULONG transp_color_,int alpha_,int method_); // 0...100

	virtual void on_mouse_l_up(kPoint pt,int flags);
	virtual void on_mouse_l_down(kPoint pt,int flags);
	virtual void on_mouse_move(kPoint pt,int flags);

	virtual const TCHAR *get_class_name() { return _T("kGuiDialog"); };

	virtual int draw(kDraw &d);
	virtual int draw_nc(kDraw &d); // returns 1 - draw default
	virtual int erase_background(kDraw &d);

	virtual void add_tool(const TCHAR *text,kWindow *w,unsigned int id);
	virtual void set_tool_font(kFont &f);
	virtual void update_tooltip();
	virtual void create_tooltip(int flags);

	virtual int get_bitmap_width() { return k_bitmap_width; };
	virtual int get_bitmap_height() { return k_bitmap_height; };
	virtual HDC get_dialog_dc() { return mem_dc; };

private:
        int final_result;

        // internal vars:
       	int capture;
        kRect capture_pos;

        int k_bitmap_width,k_bitmap_height;

        CToolTipCtrl t_t;

        HBITMAP background_bitmap;
        BITMAP background_info;
        HBITMAP prev_bitmap;
        HDC mem_dc;
        HRGN region;
        int set_region;
};

#endif
