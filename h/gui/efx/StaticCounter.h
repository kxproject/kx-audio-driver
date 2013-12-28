// EF-X Library
// Copyright (c) kX Project, 2004.
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


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CStaticCounter - CStatic derived numeric counter display
//
// Author: Jason Hattingh
// Email:  jhattingh@greystonefx.com
//
// You may freely use or modify this code provided this
// notice is included in all derived versions.
//
// This class implements a LED style counter without the need for bitmap resources
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define WM_UPDATE_STATIC (WM_USER+0x411)		// user defined WM message

#if !defined(AFX_STATICCOUNTER_H__F666A491_3847_11D3_A58E_00805FC1DE10__INCLUDED_)
#define AFX_STATICCOUNTER_H__F666A491_3847_11D3_A58E_00805FC1DE10__INCLUDED_

#include "stdafx.h"
#include "3DBar.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StaticCounter.h : header file
//

const unsigned short BARHEIGHTMIN = 6;

const DWORD STCOUNTER0    = 252;
const DWORD STCOUNTER1    = 96;
const DWORD STCOUNTER2    = 218;
const DWORD STCOUNTER3    = 242;
const DWORD STCOUNTER4    = 102;
const DWORD STCOUNTER5    = 182;
const DWORD STCOUNTER6    = 190;
const DWORD STCOUNTER7    = 224;
const DWORD STCOUNTER8    = 254;
const DWORD STCOUNTER9    = 246;
const DWORD STCOUNTER10  = 2;		// The minus sign [2]
const DWORD STCOUNTER11  = 256;	  // The "point"
const DWORD STCOUNTER12  = 257;	  // The "colon" (:)
const DWORD STCOUNTERALL  = 999;

const DWORD NOTCH1 = 128;
const DWORD NOTCH2 = 64;
const DWORD NOTCH3 = 32;
const DWORD NOTCH4 = 16;
const DWORD NOTCH5 = 8;
const DWORD NOTCH6 = 4;
const DWORD NOTCH7 = 2;		// The minus sign
const DWORD NOTCH8 = 1;		// Not used...

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CStaticDC - memory DC
//
// Author: Keith Rule, keithr@europa.com,  Copyright 1996-1997, Keith Rule
//
// You may freely use or modify this code provided this copyright is included in all derived versions.
//
// History - 10/3/97 Fixed scrolling bug.
//                   Added print support.
//				 - 14/7/99 Added optional clip rect parameter [jgh]
//

class kCLASS_TYPE CStaticDC : public CDC {
	private:
		CBitmap m_bitmap; // Offscreen bitmap
		CBitmap* m_oldBitmap; // bitmap originally found in CStaticDC
		CDC* m_pDC; // Saves CDC passed in constructor
		CRect m_rect; // Rectangle of drawing area.
		BOOL m_bMemDC; // TRUE if CDC really is a Memory DC.
	public:
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Function Header
		CStaticDC(CDC* pDC, CRect rect = CRect(0,0,0,0)) : CDC(), m_oldBitmap(NULL), m_pDC(pDC)
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		{
			ASSERT(m_pDC != NULL); // If you asserted here, you passed in a NULL CDC.
			
			m_bMemDC = !pDC->IsPrinting();
			
			if (m_bMemDC){
				// Create a Memory DC
				CreateCompatibleDC(pDC);
				if ( rect == CRect(0,0,0,0) )
					pDC->GetClipBox(&m_rect);
				else
					m_rect = rect;

				m_bitmap.CreateCompatibleBitmap(pDC, m_rect.Width(), m_rect.Height());
				m_oldBitmap = SelectObject(&m_bitmap);
				SetWindowOrg(m_rect.left, m_rect.top);
			} else {
				// Make a copy of the relevent parts of the current DC for printing
				m_bPrinting = pDC->m_bPrinting;
				m_hDC = pDC->m_hDC;
				m_hAttribDC = pDC->m_hAttribDC;
			}
		}
		
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Function Header
		~CStaticDC()
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		{
			if (m_bMemDC) {
				// Copy the offscreen bitmap onto the screen.
				m_pDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
					this, m_rect.left, m_rect.top, SRCCOPY);
				//Swap back the original bitmap.
				SelectObject(m_oldBitmap);
			} else {
				// All we need to do is replace the DC with an illegal value,
				// this keeps us from accidently deleting the handles associated with
				// the CDC that was passed to the constructor.
				m_hDC = m_hAttribDC = NULL;
			}
		}
		
		// Allow usage as a pointer
		CStaticDC* operator->() {return this;}
		
		// Allow usage as a pointer
		operator CStaticDC*() {return this;}
	};


// End CStaticDC
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Begin: class CStaticCounter
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class kCLASS_TYPE CStaticCounter : public CStatic
{
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStaticCounter)
	//}}AFX_VIRTUAL

// Implementation
public:
	CStaticCounter();
	virtual ~CStaticCounter();

	void SetFormatString(CString strFormat="%.3d")	{ 
		m_strFormat = strFormat;
		if (m_strFormat.Find('f')>0) m_bFloat=true; else m_bFloat=false;
		Update(); 
	};

	void SetID(UINT uID) { m_uID=uID; };

	static CRect GetRect(UINT uID, CDialog* pDlg);

	void SetBarHeight(int nHeight=BARHEIGHTMIN) { 
		nHeight<BARHEIGHTMIN?m_nBarHeight=BARHEIGHTMIN:m_nBarHeight=nHeight;};

	void SetAllowInteraction(bool bValue=true){m_bAllowInteraction=bValue;};

	double GetPos() { return m_fPos; };//float
   
	void SetPos( int nPos, bool bSetRange=false, float fMin=0, float fMax=100 )	{
		m_bFloat=false; 
		if (bSetRange)	{
			ASSERT(fMin <= fMax);
			SetRange(fMin, fMax); 
			if ( (float)nPos<fMin ) nPos = (int)fMin;
			if ( (float)nPos>fMax ) nPos = (int)fMax;
		}
		m_fPos = (float)nPos; 
		DisplayInt( (int)m_fPos );	
	};
	void SetPos( float fPos, bool bSetRange=false, float fMin=0, float fMax=100 ){
		m_bFloat=true; 
		if (bSetRange)	{
			ASSERT(fMin <= fMax);
			SetRange(fMin, fMax); 
			if ( fPos<fMin ) fPos = fMin;
			if ( fPos>fMax ) fPos = fMax;
		}
		m_fPos = fPos; 
		DisplayFloat( m_fPos ); 
	};

	void SetBlankPadding(int nPadding){ m_nBlankPadding = nPadding;};

	void SetRange( float fMin=0, float fMax=100 ) {	
		ASSERT(fMin <= fMax); 
		if ( m_fPos<fMin ) m_fPos = fMin;
		if ( m_fPos>fMax ) m_fPos = fMax;
		m_fMin = fMin;
		m_fMax = fMax;
	};

	void SetDrawFaded(bool bState=true){	m_bDrawFadedNotches = bState;	};
	void SetDraw3DBar(bool bState=true){	m_bDraw3DBar = bState;	};

	void Display( CString strDisplay )	{ m_strDisplay = strDisplay; Invalidate(FALSE);	};
	
	void DisplayInt(int nValue, bool bSetFmtStr=false, CString strFormat="%.3d")	{ 
		m_bFloat=false;
		if (bSetFmtStr) m_strFormat = strFormat;
		m_strDisplay.Format(m_strFormat, nValue); 
		Invalidate(FALSE); 
	};
	void DisplayFloat(float fValue, bool bSetFmtStr=false, CString strFormat="%.2f")	{
		m_bFloat=true; 
		if (bSetFmtStr) m_strFormat = strFormat;	
		m_strDisplay.Format(m_strFormat, fValue); 
		Update();	
	};
	void DisplayTime( CString strFormat = "%H:%M:%S" )	{ 
		m_bDraw3DBar=false; Display( (CTime::GetCurrentTime()).Format(strFormat) ); };
	void DisplayTime(UINT uSecs, UINT uMins=0, UINT uHours=0, UINT uDays=0, CString strFormat="%D:%H:%M:%S") {	
		m_bDraw3DBar=false; Display( (CTimeSpan(uDays, uHours, uMins, uSecs)).Format(strFormat) );	};

	void SetColourFaded(COLORREF crColor ){	m_bSpecifiedFadeColour = true;	m_crDimForeground = crColor; Invalidate(FALSE); };
	void SetColours(COLORREF crFG, COLORREF crBG = 0, COLORREF crChannel=RGB(0,100,0)){ 
		m_crForeground = crFG;	
		m_crBackground = crBG; 
		m_3DBar.SetBarColour(crChannel); 
		Invalidate(FALSE);	
	};

protected:
	
	void Update()
	{
		if (::IsWindow(m_hWnd))	Display(m_strDisplay);
	}

	void CalculateMetrics();
	void Draw( CStaticDC* pDC, DWORD dwChar, int nCol);

	int m_nLastAmount;

	unsigned short m_nBarHeight;

	bool m_bDrawFadedNotches;
	bool m_bGotMetrics;
	bool m_bSpecifiedFadeColour;

	CString m_strDisplay;
	CString m_strFormat;

	CRect m_recClient;

	float m_fMin, m_fMax;

protected:
	int m_nBlankPadding;
	UINT m_uID;
	int m_nMovement;
	C3DBar m_3DBar;

	int m_nNotchWidth;
	int m_nNotchLength;
	int m_nMargin;

	COLORREF m_crBackground;
	COLORREF m_crForeground;
	COLORREF m_crDimForeground;

	float m_fPos;

	bool m_bDraw3DBar;
	bool m_bLDown, m_bRDown;
	bool m_bAllowInteraction;
	bool m_bFloat;

	// Generated message map functions
	
	//{{AFX_MSG(CStaticCounter)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATICCOUNTER_H__F666A491_3847_11D3_A58E_00805FC1DE10__INCLUDED_)
