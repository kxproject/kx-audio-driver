// kX GUI
// Copyright (c) Eugene Gavrilov, 2001-2005.
// All rights reserved

// Indicator.cpp : implementation file
//

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

#ifdef _DEBUG
 #define new DEBUG_NEW
 #undef THIS_FILE
 static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// kIndicator


kIndicator::kIndicator()
{
 ht=10;
 wd=18;
 digits=0;
 wd_gap=ht_gap=4;
 fg=0x00ff00;
 bg=0x0;
 brd=0x0;

 set_method(kMETHOD_OWNER);
}

kIndicator::~kIndicator()
{
}


BOOL kIndicator::create(const TCHAR *name,TCHAR *value_,kWindow *parent_,
	int x,int y,int n_of_digits,int wd_,int ht_,int wd_gap_,int ht_gap_)
{
	k_parent=(kWindow *)parent_;
	digits=n_of_digits;
	ht=ht_;
	wd=wd_;
        wd_gap=wd_gap_;
        ht_gap=ht_gap_;
        
	value=value_;

	kRect r;

	r.left=x;
	r.top=y;
	r.right=r.left+(wd+wd_gap)*n_of_digits;
	r.bottom=r.top+ht+ht_gap;

	BOOL ret=kWindow::create(name,_T("STATIC"), r, SS_OWNERDRAW, 0, k_parent);

	return ret;
}

int kIndicator::draw_nc(kDraw &)
{
  return 0; // draw nothing
}

int kIndicator::draw(kDraw &d)
{
	kRect rect;
	get_rect(rect);

        HDC dc=d.get_dc();
        
	if(dc==NULL)
	{
	  return 0;
	}
        // set color
        HPEN c_pb;
        HPEN prev_pb;

        c_pb=CreatePen(PS_SOLID,0,brd);
        prev_pb=(HPEN)SelectObject(dc,c_pb);

	HBRUSH c_b;
	HBRUSH prev_b;
	c_b=CreateSolidBrush(bg);
	prev_b=(HBRUSH)SelectObject(dc,c_b);
	Rectangle(dc,0,0,rect.right,rect.bottom);

        SelectObject(dc,prev_pb);
        DeleteObject(c_pb);

        // set color
        HPEN c_p;
        HPEN prev_p;

        c_p=CreatePen(PS_SOLID,0,fg);
        prev_p=(HPEN)SelectObject(dc,c_p);

	//TCHAR *p=(TCHAR *)(LPCTSTR)value;
	TCHAR *p=((TCHAR *)(LPCTSTR)value)+strlen((LPCTSTR)value)-1;
	if(p)
	{
	 int pos;
	 // pos=0;
	 pos=digits-1;

	 while(*p)
	 {
	  switch(*p)
	  {
	  	case '0':
	  		MoveToEx(dc,pos*(wd+wd_gap)+wd_gap,ht_gap/2+1,NULL);
	  		LineTo(dc,pos*(wd+wd_gap)+wd_gap,ht_gap/2+ht-1);
	  		LineTo(dc,(pos+1)*(wd+wd_gap)-1,ht_gap/2+ht-1);
	  		LineTo(dc,(pos+1)*(wd+wd_gap)-1,ht_gap/2+1);
	  		LineTo(dc,pos*(wd+wd_gap)+wd_gap,ht_gap/2+1);
	  		break;
	  	case '1':
	  		MoveToEx(dc,(pos+1)*(wd+wd_gap)-1,ht_gap/2+ht-1,NULL);
	  		LineTo(dc,(pos+1)*(wd+wd_gap)-1,ht_gap/2+1);
	  		break;
	  	case '2':
	  		MoveToEx(dc,pos*(wd+wd_gap)+wd_gap,ht_gap/2+1,NULL);
	  		LineTo(dc,(pos+1)*(wd+wd_gap)-1,ht_gap/2+1);
	  		LineTo(dc,(pos+1)*(wd+wd_gap)-1,ht_gap/2+ht/2);
	  		LineTo(dc,pos*(wd+wd_gap)+wd_gap,ht_gap/2+ht/2);
	  		LineTo(dc,pos*(wd+wd_gap)+wd_gap,ht_gap/2+ht-1);
	  		LineTo(dc,(pos+1)*(wd+wd_gap)-1,ht_gap/2+ht-1);
	  		break;
	  	case '3':
	  		MoveToEx(dc,pos*(wd+wd_gap)+wd_gap,ht_gap/2+1,NULL);
	  		LineTo(dc,(pos+1)*(wd+wd_gap)-1,ht_gap/2+1);
	  		LineTo(dc,(pos+1)*(wd+wd_gap)-1,ht_gap/2+ht-1);
	  		LineTo(dc,pos*(wd+wd_gap)+wd_gap,ht_gap/2+ht-1);
	  		MoveToEx(dc,(pos+1)*(wd+wd_gap)-1,ht_gap/2+ht/2,NULL);
	  		LineTo(dc,pos*(wd+wd_gap)+wd_gap,ht_gap/2+ht/2);
	  		break;
	  	case '4':
	  		MoveToEx(dc,pos*(wd+wd_gap)+wd_gap,ht_gap/2+1,NULL);
	  		LineTo(dc,pos*(wd+wd_gap)+wd_gap,ht_gap/2+ht/2);
	  		LineTo(dc,(pos+1)*(wd+wd_gap)-1,ht_gap/2+ht/2);
	  		LineTo(dc,(pos+1)*(wd+wd_gap)-1,ht_gap/2+1);
	  		LineTo(dc,(pos+1)*(wd+wd_gap)-1,ht_gap/2+ht-1);
	  		break;
	  	case '5':
	  		MoveToEx(dc,(pos+1)*(wd+wd_gap)-1,ht_gap/2+1,NULL);
	  		LineTo(dc,pos*(wd+wd_gap)+wd_gap,ht_gap/2+1);
	  		LineTo(dc,pos*(wd+wd_gap)+wd_gap,ht_gap/2+ht/2);
	  		LineTo(dc,(pos+1)*(wd+wd_gap)-1,ht_gap/2+ht/2);
	  		LineTo(dc,(pos+1)*(wd+wd_gap)-1,ht_gap/2+ht-1);
	  		LineTo(dc,pos*(wd+wd_gap)+wd_gap,ht_gap/2+ht-1);
	  		break;
	  	case '6':
	  		MoveToEx(dc,pos*(wd+wd_gap)+wd_gap,ht_gap/2+1,NULL);
	  		LineTo(dc,(pos+1)*(wd+wd_gap)-1,ht_gap/2+1);
	  		LineTo(dc,pos*(wd+wd_gap)+wd_gap,ht_gap/2+1);
	  		LineTo(dc,pos*(wd+wd_gap)+wd_gap,ht_gap/2+ht-1);
	  		LineTo(dc,(pos+1)*(wd+wd_gap)-1,ht_gap/2+ht-1);
	  		LineTo(dc,(pos+1)*(wd+wd_gap)-1,ht_gap/2+ht/2);
	  		LineTo(dc,pos*(wd+wd_gap)+wd_gap,ht_gap/2+ht/2);
	  		break;
	  	case '7':
	  		MoveToEx(dc,(pos+1)*(wd+wd_gap)-1,ht_gap/2+ht-1,NULL);
	  		LineTo(dc,(pos+1)*(wd+wd_gap)-1,ht_gap/2+1);
	  		LineTo(dc,pos*(wd+wd_gap)+wd_gap,ht_gap/2+1);
	  		break;
	  	case '8':
	  		MoveToEx(dc,pos*(wd+wd_gap)+wd_gap,ht_gap/2+1,NULL);
	  		LineTo(dc,pos*(wd+wd_gap)+wd_gap,ht_gap/2+ht-1);
	  		LineTo(dc,(pos+1)*(wd+wd_gap)-1,ht_gap/2+ht-1);
	  		LineTo(dc,(pos+1)*(wd+wd_gap)-1,ht_gap/2+1);
	  		LineTo(dc,pos*(wd+wd_gap)+wd_gap,ht_gap/2+1);
	  		LineTo(dc,pos*(wd+wd_gap)+wd_gap,ht_gap/2+ht/2);
	  		LineTo(dc,(pos+1)*(wd+wd_gap)-1,ht_gap/2+ht/2);
	  		break;
	  	case '9':
	  		MoveToEx(dc,pos*(wd+wd_gap)+wd_gap,ht_gap/2+ht-1,NULL);
	  		LineTo(dc,(pos+1)*(wd+wd_gap)-1,ht_gap/2+ht-1);
	  		LineTo(dc,(pos+1)*(wd+wd_gap)-1,ht_gap/2+1);
	  		LineTo(dc,pos*(wd+wd_gap)+wd_gap,ht_gap/2+1);
	  		LineTo(dc,pos*(wd+wd_gap)+wd_gap,ht_gap/2+ht/2);
	  		LineTo(dc,(pos+1)*(wd+wd_gap)-1,ht_gap/2+ht/2);
	  		break;
	  	case 'd':
	  		MoveToEx(dc,(pos+1)*(wd+wd_gap)-1,ht_gap/2+ht/5,NULL);
	  		LineTo(dc,(pos+1)*(wd+wd_gap)-1,ht_gap/2+ht-1);
	  		LineTo(dc,pos*(wd+wd_gap)+wd_gap+wd/3,ht_gap/2+ht-1);
	  		LineTo(dc,pos*(wd+wd_gap)+wd_gap+wd/3,ht_gap/2+ht/2);
	  		LineTo(dc,(pos+1)*(wd+wd_gap)-1,ht_gap/2+ht/2);
	  		break;
	  	case 'B':
	  		MoveToEx(dc,pos*(wd+wd_gap)+wd_gap,ht_gap/2+ht/5,NULL);
	  		LineTo(dc,pos*(wd+wd_gap)+wd_gap,ht_gap/2+ht-1);
	  		LineTo(dc,pos*(wd+wd_gap)+wd_gap+wd*2/3,ht_gap/2+ht-1);
	  		LineTo(dc,pos*(wd+wd_gap)+wd_gap+wd*2/3,ht/2+ht_gap/2);

	  		LineTo(dc,pos*(wd+wd_gap)+wd_gap,ht/2+ht_gap/2);
	  		LineTo(dc,pos*(wd+wd_gap)+wd_gap+wd/2,ht/2+ht_gap/2);

	  		LineTo(dc,pos*(wd+wd_gap)+wd_gap+wd/2,ht/5+ht_gap/2);
                        LineTo(dc,pos*(wd+wd_gap)+wd_gap,ht_gap/2+ht/5);
	  		break;
	  	case '.':
	  		MoveToEx(dc,pos*(wd+wd_gap)+wd_gap-2,ht_gap/2+ht-1,NULL);
	  		LineTo(dc,pos*(wd+wd_gap)+wd_gap-3,ht_gap/2+ht-1);
	  		// pos--;
	  		pos++;
	  		break;
	  	case '-':
	  		MoveToEx(dc,pos*(wd+wd_gap)+wd_gap,ht_gap/2+ht/2,NULL);
	  		LineTo(dc,(pos+1)*(wd+wd_gap)-1,ht_gap/2+ht/2);
	  		break;
                        /*
	  	case '+':
	  		MoveToEx(dc,pos*(wd+wd_gap)+wd_gap,ht_gap/2+ht/2,NULL);
	  		LineTo(dc,(pos+1)*(wd+wd_gap)-1,ht_gap/2+ht/2);

	  		MoveToEx(dc,(pos+1)*(wd+wd_gap)/2,ht_gap/2+ht-ht/6,NULL);
	  		LineTo(dc,(pos+1)*(wd+wd_gap)/2,ht_gap/2+ht/6);
	  		break;
                        */
	  	case 'i':
	  		MoveToEx(dc,(pos*2+1)*(wd+wd_gap)/2+wd_gap,ht_gap/2+ht/2,NULL);
	  		LineTo(dc,(pos*2+1)*(wd+wd_gap)/2+wd_gap,ht_gap/2+ht);
	  		break;
	  	case 'n':
	  		MoveToEx(dc,pos*(wd+wd_gap)+wd_gap,ht_gap/2+ht-1,NULL);
	  		LineTo(dc,pos*(wd+wd_gap)+wd_gap,ht_gap/2+ht/2);
	  		LineTo(dc,(pos+1)*(wd+wd_gap)-1,ht_gap/2+ht/2);
	  		LineTo(dc,(pos+1)*(wd+wd_gap)-1,ht_gap/2+ht);
	  		break;
	  	case 'f':
	  		MoveToEx(dc,pos*(wd+wd_gap)+wd_gap,ht_gap/2+ht-1,NULL);
	  		LineTo(dc,pos*(wd+wd_gap)+wd_gap,ht_gap/2+ht*6/10);
	  		LineTo(dc,(pos+1)*(wd+wd_gap)-1,ht_gap/2+ht*6/10);

	  		MoveToEx(dc,pos*(wd+wd_gap)+wd_gap,ht_gap/2+ht*6/10,NULL);
	  		LineTo(dc,pos*(wd+wd_gap)+wd_gap,ht_gap/2+ht*3/10);
	  		LineTo(dc,(pos+1)*(wd+wd_gap)-1,ht_gap/2+ht*3/10);
	  		break;
	  	case '~':
	  		MoveToEx(dc,pos*(wd+wd_gap)+wd_gap,ht_gap/2+ht*6/10,NULL);
	  		LineTo(dc,(pos+1)*(wd+wd_gap)-1,ht_gap/2+ht*6/10);
	  		break;
	  	default:
	  	case ' ':
	  		break;
	  }
	  //p++;
	  //pos++;
	  //if(pos>digits)
	  // break;
	  p--;
	  if(p==(((TCHAR *)(LPCTSTR)value)-1))
	   break;
	  pos--;
	  if(pos<0)
	   break;
	 }
	}

        SelectObject(dc,prev_p);
        DeleteObject(c_p);
        SelectObject(dc,prev_b);
        DeleteObject(c_b);

         return 0; // draw nothing
}
