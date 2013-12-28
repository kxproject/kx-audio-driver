// kX Mixer
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


#ifndef NOTIFY_GUI_H_
#define NOTIFY_GUI_H_

class COSDIcon : public kWindow
{
public:
 void create(iKXNotifier *p_);
 void start(int icon);
private:
 virtual const TCHAR *get_class_name() { return _T("kgui_osd_icon"); };

 int draw(kDraw &d); 
 int erase_background(kDraw &) { return TRUE; };
 intptr_t message(int msg,int wparam,int lparam);

 void on_timer(int ev);
 dword last;
 int osd_on;

 #define MAX_ICONS	15
 HBITMAP images[MAX_ICONS];

 int osd_icon;

 iKXNotifier *p;
};

class COSDVolume : public kWindow
{
public:
 void create(iKXNotifier *); 
 void start(int icon,const TCHAR *str);
 void on_timer(int ev);
private:
 virtual const TCHAR *get_class_name() { return _T("kgui_osd_vol"); };

 int draw(kDraw &d); // returns 1 - draw default
 int erase_background(kDraw &) { return TRUE; };
 intptr_t message(int msg,int wparam,int lparam);

 dword last;
 int osd_on;

 kString osd_text;
 int osd_icon;

 iKXNotifier *p;
};

#endif
