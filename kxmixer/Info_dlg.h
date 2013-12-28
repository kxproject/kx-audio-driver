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


#if !defined(AFX_MYDIALOG_H__B294EF63_CE1F_4020_B1F8_573626C3E572__INCLUDED_)
#define AFX_MYDIALOG_H__B294EF63_CE1F_4020_B1F8_573626C3E572__INCLUDED_

#pragma once


class CInfoDialog: public CKXDialog
{
public:
	CInfoDialog(iKX *dev) { ikx_t=dev; };
        const char *get_class_name() { return "kXInfo"; };

	kButton b_copy;
	CEdit b_edit;
	kStatic label;
	kString hw_info;

	virtual void init();

	virtual int on_command(int wParam, int lParam);
};

#define IDg_COPY		(WM_USER+0x4a)


#endif // !defined(AFX_MYDIALOG_H__B294EF63_CE1F_4020_B1F8_573626C3E572__INCLUDED_)
