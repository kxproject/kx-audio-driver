// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov, 2001-2004.
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

// Dynamica
// (c) eYagos, 2004-2005. All rights reserved

#define WM_UPDATE_STATIC (WM_USER+0x411)//message to update the control

// Graph numbers
#define width_graph 116 
#define	range -99.9
#define	col_graph_Line RGB(164,174,183)
#define	col_graph_Mark RGB(164,174,183)
#define	col_graph_Border RGB(60,71,74)

class eGraph: public CStatic
{
	
public:
	eGraph();

	void SetID(UINT uID) { m_uID=uID; };//assigns an ID parameter to the control
	void SetBitmaps(HBITMAP bitmaps);//sets the bitmaps, 
	//int GetPos();//not implemented yet
	void SetPoints(double gain, double th1, double th2, double ra1, double ra2, double ra3, int kt);
	
public:
	~eGraph();

protected:
	UINT m_uID;
	//{{AFX_MSG(eGraph)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	HBITMAP bkg_graph;
	POINT pt[width_graph+1];
	POINT pt_border[5];
	POINT pt_mark_th1[5];
	POINT pt_mark_th2[5];
};


