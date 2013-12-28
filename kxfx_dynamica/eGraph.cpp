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

#include "stdafx.h"
#include "eGraph.h"

eGraph::eGraph()
{
     
}

eGraph::~eGraph()
{
}
////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(eGraph, CStatic)
     //{{AFX_MSG_MAP
     ON_WM_PAINT()
	 ON_WM_LBUTTONDOWN()
	 ON_WM_LBUTTONUP()
	 ON_WM_MOUSEMOVE()
     //}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////


void eGraph::OnPaint() 
{

	CRect rect;
    GetClientRect(&rect);

    CPaintDC dc(this);//create dc for painting 
    
	CBitmap bitmap;
    bitmap.Attach(bkg_graph);//attach current bitmap frame
	
	CDC dcMem;
    dcMem.CreateCompatibleDC (&dc);//create compatible memory DC
	CBitmap* pOldBitmap = dcMem.SelectObject (&bitmap);
    
	dc.BitBlt (0, 0, 116, 116, &dcMem, 0, 0, SRCCOPY);//blit memory DC to the visible PaintDC
	dcMem.SelectObject (pOldBitmap);
	
	
		dc.MoveTo (0, 0);
		
		// Draw line
		LOGBRUSH logBrush;
		logBrush.lbStyle = BS_SOLID;
		logBrush.lbColor = col_graph_Line;
		CPen borde(PS_SOLID|PS_GEOMETRIC|PS_ENDCAP_ROUND, 1, &logBrush);
		dc.SelectObject	(borde);
		dc.Polyline	(pt,width_graph); 

		// Draw marks of Th1 nad Th2 
		CPen borde2	(PS_SOLID, 1,col_graph_Mark);
		dc.SelectObject	(borde2);
		dc.MoveTo (0, 0); 
		dc.Polyline (pt_mark_th1,5); 
		dc.MoveTo (0, 0); 
		dc.Polyline (pt_mark_th2,5);

		// Draw border
		CPen borde3 (PS_SOLID, 1,col_graph_Border);
		dc.SelectObject	(borde3);
		dc.Polyline (pt_border,5); 

	bitmap.Detach();//detach current bitmap; necessary!
}


void eGraph::SetBitmaps(HBITMAP bitmaps){

	bkg_graph = bitmaps;
}


void eGraph::SetPoints(double gain, double th1, double th2, double ra1, double ra2, double ra3, int kt)
{
	// POINT CALCULATIONS
	//_____________________________________________________________
	int		i;
	long	xint, yint;
	double	x, y;
	double	v_medio1;
	double	v_medio2;
	double	diff;
	
	double	a	= 0.025 * 200.0;

	switch (kt)
		{
		case 0:
			v_medio1 = 0.0;
			v_medio2 = 0.0;
			break;
		case 1:
			v_medio1 = (ra2-ra1)/4.0*a;
			v_medio2 = (ra3-ra2)/4.0*a;
			break;
		case 2:
			a = a*2.0;
			v_medio1 = (ra2-ra1)/4.0*a;
			v_medio2 = (ra3-ra2)/4.0*a;
			break;
		}
	double	y1 = th1 + gain;
	//double	y0 = y1-th1*ra1; 
	double	y2 = y1-(th1-th2)*ra2;
	// Line points
	for (i = 0; i <= width_graph; i++) 
	{
		x = i*(-range)/(width_graph-1)-(-range);
		if ((range<=x) && (x<th2))
		{
			y=y2-(th2-x)*ra3;
		}
		else if((th2<=x) && (x<th1))
		{
			y=y1-(th1-x)*ra2;
		}
		else 
		{
			y=y1-(th1-x)*ra1;
		}
		//soft knee
		//diff1
		diff = fabs(x-th1)/a-1.0;
		if (diff>0.0)
		{
			diff=0.0;
		}
		else
		{
			diff = diff*diff;
		}
		y = y-v_medio1*diff;
		//diff2
		diff = fabs(x-th2)/a-1.0;
		if (diff>0.0)
		{
			diff=0.0;
		}
		else
		{
			diff = diff*diff;
		}
		y = y-v_medio2*diff; 
		//Scale
		x = (( x * (width_graph-1)/(-range)) + (width_graph-1));
		y = ((-y * (width_graph-1)/(-range)));
		xint = static_cast < int > (x);
		yint = static_cast < int > (y);
		if ( (x - static_cast < int > (x)) > 0.5)  xint = xint + 1;
		if ( (y - static_cast < int > (y)) > 0.5)  yint = yint + 1;

		
		//Limits
		if (yint<0) yint=0; 
		if (yint>(width_graph-1)) yint=(width_graph-1);
		if (xint<0)	xint=0; 
		//Offset
		pt[i].x = xint;
		pt[i].y = yint;
	}

	// Border points
	pt_border[0].x = (long) -1;
	pt_border[0].y = (long) -1;
	pt_border[1].x = (long) (width_graph);
	pt_border[1].y = (long) -1;
	pt_border[2].x = (long) (width_graph);
	pt_border[2].y = (long) (width_graph);
	pt_border[3].x = (long) -1; 
	pt_border[3].y = (long) (width_graph);
	pt_border[4].x = (long) -1;
	pt_border[4].y = (long) -1;

	pt_mark_th1[0].x = (long)(( th1 * (width_graph)/(-range)) + (width_graph));
	pt_mark_th1[0].y = (long)(width_graph-1)-4;
	pt_mark_th1[1].x = (long)(( th1 * (width_graph)/(-range)) + (width_graph));
	pt_mark_th1[1].y = (long)(width_graph-1);
	pt_mark_th1[2].x = pt_mark_th1[1].x - 1;
	pt_mark_th1[2].y = pt_mark_th1[1].y - 1;
	pt_mark_th1[3].x = pt_mark_th1[2].x+2;
	pt_mark_th1[3].y = pt_mark_th1[2].y;
	pt_mark_th1[4].x = pt_mark_th1[1].x;
	pt_mark_th1[4].y = pt_mark_th1[1].y;

	pt_mark_th2[0].x = (long)(( th2 * (width_graph)/(-range)) + (width_graph));
	pt_mark_th2[0].y = (long)(width_graph-1)-4;
	pt_mark_th2[1].x = (long)(( th2 * (width_graph)/(-range)) + (width_graph));
	pt_mark_th2[1].y = (long)(width_graph-1);
	pt_mark_th2[2].x = pt_mark_th2[1].x - 1;
	pt_mark_th2[2].y = pt_mark_th2[1].y - 1;
	pt_mark_th2[3].x = pt_mark_th2[2].x+2;
	pt_mark_th2[3].y = pt_mark_th2[2].y;
	pt_mark_th2[4].x = pt_mark_th2[1].x;
	pt_mark_th2[4].y = pt_mark_th2[1].y;

	for (i=0; i<5; i++)
	{
		if (pt_mark_th1[i].x > width_graph-1) pt_mark_th1[i].x = width_graph-1;
		if (pt_mark_th1[i].x < -1 ) pt_mark_th1[i].x = -1;
		if (pt_mark_th2[i].x > width_graph-1) pt_mark_th2[i].x = width_graph-1;
		if (pt_mark_th2[i].x < -1 ) pt_mark_th2[i].x = -1;
		//if (pt_mark_th1[i].y > width_graph) pt_mark_th1[i].y = width_graph;
		//if (pt_mark_th1[i].y < -1 ) pt_mark_th1[i].y = -1;
		//if (pt_mark_th2[i].y > width_graph) pt_mark_th2[i].y = width_graph;
		//if (pt_mark_th2[i].y < -1 ) pt_mark_th2[i].y = -1;

	}

	RedrawWindow();

}