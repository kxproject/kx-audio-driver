// EF-X Library
// Copyright (c) Martin Borisov, 2004.
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

// StaticCounter.cpp : implementation file
//

#include "stdafx.h"
#include "gui/efx/StaticCounter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStaticCounter

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Function Header
CStaticCounter::CStaticCounter()
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	m_crBackground = RGB(0,0,0);
	m_crForeground = RGB(100, 255, 100);

	m_bLDown = m_bRDown = false;

	m_uID=0;
	m_nBlankPadding = 0;

	m_nBarHeight = BARHEIGHTMIN + 4;
	m_nLastAmount = 0;

	m_bAllowInteraction = true;
	m_bFloat=false;
	m_strDisplay = "0";
	m_strFormat="%.3d";
	m_bSpecifiedFadeColour = false;
	m_bDrawFadedNotches = true;
	m_bGotMetrics = false;
	m_bDraw3DBar = true;
	m_fPos = 0;
	SetRange(0,100);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Function Header
CStaticCounter::~CStaticCounter()
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
}

BEGIN_MESSAGE_MAP(CStaticCounter, CStatic)
	//{{AFX_MSG_MAP(CStaticCounter)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStaticCounter message handlers

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Function Header
void CStaticCounter::CalculateMetrics()
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	// Calculate the character metrics in proportion to the size of the control:
	int nHeight = m_recClient.bottom;

	if (m_bDraw3DBar) nHeight -= (m_nBarHeight-2);
	
	(nHeight * 0.06) < 1 ? m_nMargin = 1 : m_nMargin = (int)(nHeight * 0.06);
	(nHeight * 0.35) < 1 ? m_nNotchLength = 1 : m_nNotchLength = (int)(nHeight * 0.35);
	m_nNotchWidth = m_nMargin;
	m_bGotMetrics = true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Function Header
void CStaticCounter::OnPaint() 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	GetClientRect(&m_recClient);
	CPaintDC dc(this);
	CStaticDC memDC(&dc, m_recClient);
	CStaticDC* pDC = &memDC;

	pDC->FillSolidRect(&m_recClient, m_crBackground);

	if (!m_bGotMetrics)		CalculateMetrics();

	int nColPos = 0;

	CString strFormatted = m_strDisplay;

	/////////////////////////////////////////////////////////////////////////////////////////////
	// BLANK PADDING
		if ( (m_nBlankPadding>0) && (m_strDisplay.GetLength()<m_nBlankPadding) )
		{
			int nCurLength = m_strDisplay.GetLength();
			int nDiff = m_nBlankPadding-nCurLength;
			CString strBlank("                                        ");
			strFormatted = strBlank.Left(nDiff) + m_strDisplay;
		}
	// END BLANK PADDING
	/////////////////////////////////////////////////////////////////////////////////////////////


	for (int nCount = 0; nCount<strFormatted.GetLength(); nCount++)
	{
		// Calculate the position of the next character:

		if ( nCount > 0 )
		{
			if (strFormatted[nCount] == ':')	
				nColPos+= m_nNotchLength+m_nMargin;
			else if (strFormatted[nCount-1] == ':' )	
				nColPos+= m_nNotchLength+1;
			else
				nColPos += m_nNotchLength + (m_nMargin*4);
		}

		// First 'lay down' the faded notches:
		if (m_bDrawFadedNotches && strFormatted[nCount] != ':' )	
			Draw( pDC, STCOUNTERALL, nColPos );

		if		  ( strFormatted[nCount] == '0' ) Draw( pDC, STCOUNTER0, nColPos );
		else if ( strFormatted[nCount] == '1' )	Draw( pDC, STCOUNTER1, nColPos );
		else if ( strFormatted[nCount] == '2' )	Draw( pDC, STCOUNTER2, nColPos );
		else if ( strFormatted[nCount] == '3' )	Draw( pDC, STCOUNTER3, nColPos );
		else if ( strFormatted[nCount] == '4' )	Draw( pDC, STCOUNTER4, nColPos );
		else if ( strFormatted[nCount] == '5' )	Draw( pDC, STCOUNTER5, nColPos );
		else if ( strFormatted[nCount] == '6' )	Draw( pDC, STCOUNTER6, nColPos );
		else if ( strFormatted[nCount] == '7' )	Draw( pDC, STCOUNTER7, nColPos );
		else if ( strFormatted[nCount] == '8' )	Draw( pDC, STCOUNTER8, nColPos );
		else if ( strFormatted[nCount] == '9' )	Draw( pDC, STCOUNTER9, nColPos );
		else if ( strFormatted[nCount] == '-' )	Draw( pDC, STCOUNTER10, nColPos );
		else if ( strFormatted[nCount] == '.' )	Draw( pDC, STCOUNTER11, nColPos );
		else if ( strFormatted[nCount] == ':' )	Draw( pDC, STCOUNTER12, nColPos );
	}

	if (m_bDraw3DBar)		
	{
		// the reason I multiply by a hundred is to improve the visuals of ranges between 0 and 1
		float fRange = (float)abs( (int)( (m_fMax*100) - (m_fMin*100) ) );
		float fWidth = ((float)m_recClient.right/fRange) * // Split the width up into fRange number of 'portions' ...
							( (m_fPos*100)-(m_fMin*100) ); // .. and multiply by current value

		CRect BarBgRect( CPoint(m_recClient.left, m_recClient.bottom-m_nBarHeight), CSize((int)m_recClient.right, m_nBarHeight) );
		CBrush brBG(::GetSysColor(COLOR_BTNFACE));
		pDC->FillRect(BarBgRect, &brBG);
		pDC->Draw3dRect(	&BarBgRect, ::GetSysColor(COLOR_BTNSHADOW), ::GetSysColor(COLOR_BTNHILIGHT) );

		CRect BarRect(  CPoint(m_recClient.left, m_recClient.bottom-m_nBarHeight), CSize((int)fWidth, m_nBarHeight) );
		m_3DBar.DrawHorizontal( pDC, BarRect );
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Function Header
void CStaticCounter::Draw(CStaticDC* pDC, DWORD dwChar, int nCol)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	COLORREF crNotchColor = m_crForeground;
	
	if (dwChar == STCOUNTERALL && !m_bSpecifiedFadeColour)	// The colour used will be a dim version of normal foreground
	{
		int r = GetRValue(m_crForeground)/3;
		int g = GetGValue(m_crForeground)/3;
		int b = GetBValue(m_crForeground)/3;
		crNotchColor = RGB(r,g,b);
	}
	else if (dwChar == STCOUNTERALL && m_bSpecifiedFadeColour)
		crNotchColor = m_crDimForeground;

	// Create the Pen accordingly
	CPen pen(PS_SOLID | PS_ENDCAP_ROUND, m_nNotchWidth, crNotchColor);
	CPen* pOldPen=pDC->SelectObject(&pen);


	if ( (dwChar & NOTCH1) || dwChar == STCOUNTERALL)	{	// should I draw the first bar in the display?
		pDC->MoveTo( nCol + m_nMargin*2, m_nMargin );
		pDC->LineTo( nCol + m_nNotchLength, m_nMargin );
	}

	if ( dwChar & NOTCH2 || dwChar == STCOUNTERALL)	{	// should I draw the 2nd bar in the display? [minus sign]
		pDC->MoveTo(nCol + m_nNotchLength + m_nMargin, m_nMargin*2);
		pDC->LineTo(nCol + m_nNotchLength + m_nMargin, m_nNotchLength + (m_nMargin*2) );
	}

	if ( dwChar & NOTCH3 || dwChar == STCOUNTERALL)	{	// should I draw the 3rd bar in the display?
		pDC->MoveTo(nCol + m_nNotchLength + m_nMargin, m_nNotchLength + (m_nMargin*4) );
		pDC->LineTo(nCol + m_nNotchLength + m_nMargin, m_nNotchLength*2 + (m_nMargin*3) );
	}

	if ( dwChar & NOTCH4 || dwChar == STCOUNTERALL)	{	// should I draw the 4th bar in the display?
		pDC->MoveTo( nCol + m_nMargin*2, m_nNotchLength*2 + (m_nMargin*4) );
		pDC->LineTo( nCol + m_nNotchLength, m_nNotchLength*2 + (m_nMargin*4) );
	}

	if ( dwChar & NOTCH5 || dwChar == STCOUNTERALL)	{	// should I draw the 5th bar in the display?
		pDC->MoveTo(nCol + m_nMargin, m_nNotchLength + (m_nMargin*4) );
		pDC->LineTo(nCol + m_nMargin, m_nNotchLength*2 + (m_nMargin*3) );
	}

	if ( dwChar & NOTCH6 || dwChar == STCOUNTERALL)	{	// should I draw the 6th bar in the display?
		pDC->MoveTo(nCol + m_nMargin, m_nMargin*2);
		pDC->LineTo(nCol + m_nMargin, m_nNotchLength + (m_nMargin*2) );
	}

	if ( dwChar & NOTCH7 || dwChar == STCOUNTERALL)	{	// should I draw the 7th bar in the display?
		pDC->MoveTo(nCol + m_nMargin*2, m_nNotchLength + (m_nMargin*3) );
		pDC->LineTo(nCol + m_nMargin + m_nNotchLength - m_nMargin, m_nNotchLength + (m_nMargin*3) );
	}

	if ( dwChar == STCOUNTER11 )	{	// should I draw the point?
		pDC->MoveTo( nCol + m_nMargin*2, m_nNotchLength*2 + (m_nMargin*4) );
		pDC->LineTo( nCol + (m_nNotchLength/2), m_nNotchLength*2 + (m_nMargin*4) );
	}

	if ( dwChar == STCOUNTER12 )	{	// should I draw the colon?
		// Upper dot:
		pDC->MoveTo( nCol + m_nMargin*2+(m_nMargin*2), m_nNotchLength );
		pDC->LineTo( nCol + (m_nNotchLength/2)+(m_nMargin*2), m_nNotchLength );

		// Lower dot:
		pDC->MoveTo( nCol + m_nMargin*2+(m_nMargin*2), m_nNotchLength*2 + (m_nMargin) );
		pDC->LineTo( nCol + (m_nNotchLength/2)+(m_nMargin*2), m_nNotchLength*2 + (m_nMargin) );
	}

	pDC->SelectObject(pOldPen);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Function Header
CRect CStaticCounter::GetRect(UINT uID, CDialog *pDlg)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// I wrote this static function here to save me re-writing it for every dialog app that uses this class (about time!)
{
	CWnd* pWnd = pDlg->GetDlgItem(uID);
	ASSERT(pWnd);

	CRect rect;
	pWnd->GetWindowRect( &rect );
	pDlg->ScreenToClient( &rect );

	return rect;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Function Header
void CStaticCounter::OnRButtonDown(UINT nFlags, CPoint point) 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	SetCapture();
	m_bRDown = true;
	m_nMovement = point.x;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Function Header
void CStaticCounter::OnRButtonUp(UINT nFlags, CPoint point) 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	ReleaseCapture();
	
	if (m_nLastAmount==0 && m_bAllowInteraction)	// ALLOW UNIT CHANGES WHEN IT'S HARD TO DRAG FOR SMALL NUMBERS
	{
		m_fPos+=(m_bFloat?0.01f:1);	// INCREMENT

		if (m_fPos<m_fMin) m_fPos = m_fMin;
		if (m_fPos>m_fMax) m_fPos = m_fMax;
		if (m_bFloat)
			DisplayFloat(m_fPos);
		else
			DisplayInt((int)m_fPos);
	}

	#ifdef WM_UPDATE_STATIC
		//#define WM_UPDATE_STATIC (WM_USER+0x411)		// user defined WM message
		::PostMessage(  GetParent()->m_hWnd,  WM_UPDATE_STATIC, (WPARAM)m_uID, (LPARAM)m_fPos );
	#endif

	m_nLastAmount = 0;
	m_bRDown = false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Function Header
void CStaticCounter::OnLButtonDown(UINT nFlags, CPoint point) 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	m_bLDown = true;
	m_nMovement = point.x;
	SetCapture();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Function Header
void CStaticCounter::OnLButtonUp(UINT nFlags, CPoint point) 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	ReleaseCapture();
	
	if (m_nLastAmount==0 && m_bAllowInteraction)	// ALLOW UNIT CHANGES WHEN IT'S HARD TO DRAG FOR SMALL NUMBERS
	{
		m_fPos-=(m_bFloat?0.01f:1);	// DECREMENT

		if (m_fPos<m_fMin) m_fPos = m_fMin;
		if (m_fPos>m_fMax) m_fPos = m_fMax;
		if (m_bFloat)
			DisplayFloat(m_fPos);
		else
			DisplayInt((int)m_fPos);
	}

	#ifdef WM_UPDATE_STATIC
		//#define WM_UPDATE_STATIC (WM_USER+0x411)		// user defined WM message
		::PostMessage(  GetParent()->m_hWnd,  WM_UPDATE_STATIC, (WPARAM)m_uID, (LPARAM)m_fPos );
	#endif

	m_nLastAmount = 0;
	m_bLDown = false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Function Header
void CStaticCounter::OnMouseMove(UINT nFlags, CPoint point) 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	if (! m_bLDown && !m_bRDown)	return;		// If neither mouse buttons are down, don't come here thinking this is an off-license, ok!
	if (! m_bAllowInteraction)	return;					// "Sorry, I'm grounded and my dad says I'm not allowed visitors"

	m_nLastAmount = point.x-m_nMovement;
	m_nMovement = point.x;

	if (m_bLDown)
		m_fPos+=(float)m_nLastAmount;
	else
		m_fPos+=(float)((float)m_nLastAmount/(float)100);

	if (m_fPos<m_fMin) m_fPos = m_fMin;
	if (m_fPos>m_fMax) m_fPos = m_fMax;

	if (m_bFloat)
		DisplayFloat(m_fPos);
	else
		DisplayInt((int)m_fPos);

	#ifdef WM_UPDATE_STATIC
		//#define WM_UPDATE_STATIC (WM_USER+0x411)		// user defined WM message
		::PostMessage(  GetParent()->m_hWnd,  WM_UPDATE_STATIC, (WPARAM)m_uID, (LPARAM)m_fPos );
	#endif
}
