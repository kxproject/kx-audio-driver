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


#if !defined(AFX_MYSPECTR_H__F975EE5D_96E1_4505_BBE6_9F3813EA7D30__INCLUDED_)
#define AFX_MYSPECTR_H__F975EE5D_96E1_4505_BBE6_9F3813EA7D30__INCLUDED_

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CKXSpectrum

class CMainDialog;
class CKXSpectrum : public kWindow
{
// Construction
public:
	CKXSpectrum();
    ~CKXSpectrum();

	BOOL create(char *name,int x,int y, int wd, int ht, CMainDialog* pParentWnd);

	UINT_PTR timer_id;

#if 0
	#define MAX_PEAK_METERS 10
	kDialog *meters[MAX_PEAK_METERS];
#endif

    int on;
    void turn_on(int onoff);

    #define SPECTRUM_VIEW_LAST 1
#if 0
	int view;
	void set_view(int m_) { /*view=m_;*/view=0; };
	int get_view() { return view; };
#endif

    void set_view(int m_) { /* nop */; };
	int get_view() { return 0; };

    // voice meter
	#define MAX_SPECTRUM_PREV	50
	int prev[MAX_SPECTRUM_PREV];
	int last;
	int step_size,step_gap;

	CMainDialog *parent;

	virtual void on_destroy();

	int draw(kDraw &);

    afx_msg void OnTimer(UINT_PTR);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_MYSPECTR_H__F975EE5D_96E1_4505_BBE6_9F3813EA7D30__INCLUDED_)
