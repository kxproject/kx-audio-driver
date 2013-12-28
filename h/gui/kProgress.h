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

#if !defined(kX_PROGRESS_H__52789AD8_9B95_49F3_9BAD_3EA749987D3F__INCLUDED_)
#define kX_PROGRESS_H__52789AD8_9B95_49F3_9BAD_3EA749987D3F__INCLUDED_

#pragma once

/////////////////////////////////////////////////////////////////////////////
// kProgress

class kCLASS_TYPE kProgress : public kWindow
{
public:
	kProgress();

	BOOL create(kRect &r,unsigned int style,unsigned int id_,kWindow *parent_);
        // method_

        virtual void set_range(int min_,int max_);
        virtual int get_pos();
        virtual int set_pos(int pos);
        virtual int set_step(int step);
        virtual int set_bar_color(kColor c);
        virtual int set_bk_color(kColor c);

        virtual const TCHAR *get_class_name() { return _T("kGuiProgress"); };

private:
};

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(kX_Progress_H__52789AD8_9B95_49F3_9BAD_3EA749987D3F__INCLUDED_)

