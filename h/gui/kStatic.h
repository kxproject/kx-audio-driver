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

#if !defined(kGUI_STATIC_H__F975EE5D_96E1_4505_BBE6_9F3813EA7D30__INCLUDED_)
#define kGUI_STATIC_H__F975EE5D_96E1_4505_BBE6_9F3813EA7D30__INCLUDED_

#pragma once

/////////////////////////////////////////////////////////////////////////////
// kStatic

class kCLASS_TYPE kStatic : public kWindow
{
public:
	kStatic();
	virtual ~kStatic();

	virtual BOOL create(const TCHAR *name,int x,int y, HBITMAP bm_id, kDialog* pParentWnd,int method_);
	virtual BOOL create(const TCHAR *text,int x,int y,int wd,int ht,kDialog *pParentWnd,kColor fg_,kColor bk_);
	virtual BOOL create(const TCHAR *label,DWORD style, kRect &r, kDialog *parent, UINT id=0);
	
	virtual void size_to_content();
	virtual int draw(kDraw &d);
	virtual int draw_nc(kDraw &d); // returns 1 - draw default

	virtual const TCHAR *get_class_name() { return _T("kGuiStatic"); };

private:
        // internal vars
	HBITMAP bitmap;

        // for text statics
        kColor bk,fg;
};

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_STATIC_H__F975EE5D_96E1_4505_BBE6_9F3813EA7D30__INCLUDED_)
