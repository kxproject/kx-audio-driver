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

#if !defined(_kBase_H)
#define _kBase_H

#pragma once

class kCLASS_TYPE kRect
{
public:

 union
 {
  struct
  {
   int x1,y1,x2,y2;
  };
  struct
  {
   int left, top, right, bottom;
  };
 };

 kRect(int p1,int p2,int p3,int p4) { x1=p1; y1=p2; x2=p3; y2=p4; normalyze(); };
 kRect() { x1=y1=x2=y2=0; };
 kRect(int a) { x1=y1=x2=y2=a; };
 
 int width() { return x2-x1; };
 int height() { return y2-y1; };
 void normalyze() { int tmp; if(x1>x2) { tmp=x1; x1=x2; x2=tmp; }; if(y1>y2) { tmp=y1; y1=y2; y2=tmp; }; };
 int point_in_rect(const kPoint &pt);
 void init(kRect *r) { x1=r->x1; y1=r->y1; x2=r->x2; y2=r->y2; };
 void init(int p1,int p2,int p3,int p4) { x1=p1; y1=p2; x2=p3; y2=p4; normalyze(); };

 operator LPRECT() { return (LPRECT)&x1; };
};

class kCLASS_TYPE kSize
{
public:
 struct 
 {
 int cx,cy;
 };

 kSize(int a,int b) { cx=a; cy=b; };
 kSize() { cx=cy=0; };

 void init(int a,int b) { cx=a; cy=b; };
};

class kCLASS_TYPE kPoint
{
public:
 int x,y;

 kPoint(int _x,int _y) { x=_x; y=_y; };
 kPoint() { x=y=0; };

 operator LPPOINT() { return (LPPOINT)&x; };
};

class kCLASS_TYPE kDraw
{
public:
 kDraw(HDC dc_) { dc=dc_; from=0; created=0; memset(&dis,0,sizeof(dis)); memset(&ps,0,sizeof(ps)); };
 kDraw(kWindow *);
 ~kDraw();
 HDC get_dc() { return dc; };
 void set_dis(LPDRAWITEMSTRUCT d) { memcpy(&dis,d,sizeof(DRAWITEMSTRUCT)); };
 void get_dis(LPDRAWITEMSTRUCT d) { memcpy(d,&dis,sizeof(DRAWITEMSTRUCT)); };
 void set_ps(LPPAINTSTRUCT p) { memcpy(&ps,p,sizeof(PAINTSTRUCT)); };
 void get_ps(LPPAINTSTRUCT p) { memcpy(p,&ps,sizeof(PAINTSTRUCT)); };

private:
 HDC dc;
 PAINTSTRUCT ps;
 DRAWITEMSTRUCT dis;

 int created;
 kWindow *from;
};

class kCLASS_TYPE kColor
{
public:
 unsigned int color;

 kColor(int r,int g,int b) { color=(r&0xff)+((b&0xff)<<8)+((g&0xff)<<16); };
 kColor(unsigned int c_) { color=c_; };
 kColor() { color=0; };

 unsigned int operator =(unsigned int val) { color=val; return val; };
 operator COLORREF() { return (COLORREF)color; }
};

class kCLASS_TYPE kFont
{
public:
 kFont();
 ~kFont();

 void set_font(kWindow *w,HFONT fnt,int flag_=0);
 void set_font(kWindow *w,kFont &f,int flag_=0); 
 HFONT get_font() { return font; };

 operator HFONT() { return font; };
 operator CFont *() { return CFont::FromHandle(font); };

private:
 HFONT font;
 int flag;
};

#ifndef kString
 #define kString CString
#endif

#endif
