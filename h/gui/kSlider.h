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

#if !defined(kGUI_SLIDER_H__F975EE5D_96E1_4505_BBE6_9F3813EA7D30__INCLUDED_)
#define kGUI_SLIDER_H__F975EE5D_96E1_4505_BBE6_9F3813EA7D30__INCLUDED_

#pragma once

/////////////////////////////////////////////////////////////////////////////
// kStatic

class kDialog;

class kCLASS_TYPE kSlider : public kWindow
{
public:
    kSlider();
    ~kSlider();

        int create(const TCHAR *name,DWORD dwStyle, kRect& rect, kDialog* pParentWnd, UINT nID,int method_=kMETHOD_DEFAULT);

        void set_bitmaps(HBITMAP back,HBITMAP thumb,HBITMAP thumb_down=0,HBITMAP disabled=0);
    virtual int draw(kDraw &d);
    virtual int draw_nc(kDraw &d); // returns 1 - draw default
    virtual int erase_background(kDraw &d);
        virtual void size_to_content();

    int get_slider_width();
    int get_slider_height();

    virtual const TCHAR *get_class_name() { return _T("kGuiSlider"); };

        virtual void on_mouse_move(kPoint pt,int flags);
        virtual void on_mouse_l_up(kPoint pt,int flags);
        virtual void on_mouse_l_down(kPoint pt,int flags);
        virtual void on_loose_capture(); // on change capture

        // actions:
        virtual void set_range(int min_,int max_,int redraw=0);
        virtual void set_tic_freq(int fr_);
        virtual void set_pos(int pos_);
        virtual int get_range_max();
        virtual int get_pos();
        virtual int get_range_min();
        virtual int set_page_size(int s);

        // internal vars
private:
        HBITMAP background;
        HBITMAP thumb;
        HBITMAP thumb_down;
        HBITMAP disabled;

        void get_thumb_rect(kRect & rect);

        int is_virtual; // special case: handle 0x0..0x7fffffff slider as 0..32767
};

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_SLIDER_H__F975EE5D_96E1_4505_BBE6_9F3813EA7D30__INCLUDED_)
