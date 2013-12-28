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

#if !defined(kX_COMBO_H__52789AD8_9B95_49F3_9BAD_3EA749987D3F__INCLUDED_)
#define kX_COMBO_H__52789AD8_9B95_49F3_9BAD_3EA749987D3F__INCLUDED_

#pragma once

/////////////////////////////////////////////////////////////////////////////
// kCombo

class kCLASS_TYPE kCombo : public kWindow
{
public:
	kCombo();

	int get_selection();
	int set_selection(int);
	int set_dropped_width(int);
	int add_string(LPCTSTR);

	BOOL create(kRect &r,unsigned int style,unsigned int id_,kWindow *parent_);
	// int method_

        virtual const TCHAR *get_class_name() { return _T("kGuiCombo"); };

private:
};

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(kX_COMBO_H__52789AD8_9B95_49F3_9BAD_3EA749987D3F__INCLUDED_)

