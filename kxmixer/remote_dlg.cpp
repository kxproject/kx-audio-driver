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


#include "stdinc.h"

#include "remote_dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////

void CRemoteDlg::init()
{
	generic_init("remote");

        select_device();
}       	

int CRemoteDlg::select_device(int dev_)
{
	CKXDialog::select_device(dev_);

        device_init("remote");

        SetWindowText(mf.get_profile("lang","remote.name")+" - "+ikx_t->get_device_name());

        return 0;
}

void CRemoteDlg::on_destroy()
{
	if(card_name.m_hWnd) card_name.DestroyWindow();
	CKXDialog::on_destroy();
}
