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

// 3DBar.cpp: implementation of the C3DBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "gui/efx/3DBar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Function Header
C3DBar::C3DBar()
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	m_crChannel = RGB(0, 100, 0);
	UpdateColours();
	CreatePens();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Function Header
C3DBar::~C3DBar()
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Function Header
void C3DBar::UpdateColours()
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	// Calculates the lighter and darker colors, as well as the shadow colors.
	m_crColorLight = LightenColor(m_crChannel, 51);
	m_crColorLighter = LightenColor(m_crColorLight, 51);
	m_crColorLightest = LightenColor(m_crColorLighter, 51);
	m_crColorDark = DarkenColor(m_crChannel, 51);
	m_crColorDarker = DarkenColor(m_crColorDark, 51);
	m_crDkShadow = ::GetSysColor(COLOR_3DDKSHADOW);
	m_crLiteShadow = ::GetSysColor(COLOR_3DSHADOW);

	// Get a color halfway between COLOR_3DDKSHADOW and COLOR_3DSHADOW
	BYTE byRed3DDkShadow = GetRValue(m_crDkShadow);
	BYTE byRed3DLiteShadow = GetRValue(m_crLiteShadow);
	BYTE byGreen3DDkShadow = GetGValue(m_crDkShadow);
	BYTE byGreen3DLiteShadow = GetGValue(m_crLiteShadow);
	BYTE byBlue3DDkShadow = GetBValue(m_crDkShadow);
	BYTE byBlue3DLiteShadow = GetBValue(m_crLiteShadow);

	m_crShadow = RGB(byRed3DLiteShadow + ((byRed3DDkShadow - byRed3DLiteShadow) >> 1),
						  byGreen3DLiteShadow + ((byGreen3DDkShadow - byGreen3DLiteShadow) >> 1),
						  byBlue3DLiteShadow + ((byBlue3DDkShadow - byBlue3DLiteShadow) >> 1));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Function Header
void C3DBar::DrawHorizontal(CDC *pDC, CRect &BarRect)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	if (!BarRect.Width())	return;

	CBrush brLightest(m_crColorLightest);
	pDC->FillRect(BarRect, &brLightest);

	int nLeft = BarRect.left;
	int nTop = BarRect.top;
	int nBottom = BarRect.bottom;
	int nRight = BarRect.right;

	CPen *pOldPen = pDC->SelectObject(&m_penColorLight);
	{
		pDC->MoveTo(nLeft + 2, nBottom - 4);	
		pDC->LineTo(nRight - 2, nBottom - 4);
		pDC->MoveTo(nLeft + 2, nTop + 2);		
		pDC->LineTo(nRight - 2, nTop + 2);
		pDC->SetPixel(nLeft + 1, nBottom - 3, m_crColorLight);
		pDC->SetPixel(nLeft + 1, nTop + 1, m_crColorLight);
	}

	pDC->SelectObject(&m_penColorLighter);
	{
		pDC->MoveTo(nLeft + 2, nBottom - 5);
		pDC->LineTo(nRight - 3, nBottom - 5);
		pDC->LineTo(nRight - 3, nTop + 3);
		pDC->LineTo(nLeft + 1, nTop + 3);
		pDC->SetPixel(nLeft + 1, nBottom - 4, m_crColorLighter);
		pDC->SetPixel(nLeft + 1, nTop + 2, m_crColorLighter);
	}

	pDC->SelectObject(&m_penColor);
	{
		pDC->MoveTo(nLeft, nBottom - 1);
		pDC->LineTo(nLeft, nTop);
		pDC->LineTo(nLeft + 2, nTop);
		pDC->SetPixel(nLeft + 1, nBottom - 2, m_crChannel);
		pDC->MoveTo(nLeft + 2, nBottom - 3);
		pDC->LineTo(nRight - 2, nBottom - 3);
		pDC->MoveTo(nLeft + 2, nTop + 1);
		pDC->LineTo(nRight - 1, nTop + 1);
	}
	
	pDC->SelectObject(&m_penColorDark);
	{
		pDC->MoveTo(nLeft + 2, nBottom - 2);
		pDC->LineTo(nRight - 2, nBottom - 2);
		pDC->LineTo(nRight - 2, nTop + 1);
		pDC->MoveTo(nLeft + 2, nTop);
		pDC->LineTo(nRight, nTop);
		pDC->SetPixel(nLeft + 1, nBottom - 1, m_crColorDark);
	}

	pDC->SelectObject(&m_penColorDarker);
	{
		pDC->MoveTo(nLeft + 2, nBottom - 1);
		pDC->LineTo(nRight - 1, nBottom - 1);
		pDC->LineTo(nRight - 1, nTop);
	}

	pDC->SelectObject(&m_penShadow);
	{
		pDC->MoveTo(nRight, nTop);
 		pDC->LineTo(nRight, nBottom);
	}

	pDC->SelectObject(&m_penLiteShadow);
	{
 		pDC->MoveTo(nRight + 1, nTop);
		pDC->LineTo(nRight + 1, nBottom);
	}


	pDC->SelectObject(pOldPen);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Function Header
void C3DBar::DeletePens()
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	if (m_penColorLight.m_hObject)		 m_penColorLight.DeleteObject();
	if (m_penColorLighter.m_hObject)	m_penColorLighter.DeleteObject();
	if (m_penColor.m_hObject)			   m_penColor.DeleteObject();
	if (m_penColorDark.m_hObject)		m_penColorDark.DeleteObject();
	if (m_penColorDarker.m_hObject)	   m_penColorDarker.DeleteObject();
	if (m_penDkShadow.m_hObject)	   m_penDkShadow.DeleteObject();
	if (m_penShadow.m_hObject)			m_penShadow.DeleteObject();
	if (m_penLiteShadow.m_hObject)		m_penLiteShadow.DeleteObject();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Function Header
void C3DBar::CreatePens()
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	DeletePens();
	m_penColorLight.CreatePen(PS_SOLID, 1, m_crColorLight);
	m_penColorLighter.CreatePen(PS_SOLID, 1, m_crColorLighter);
	m_penColor.CreatePen(PS_SOLID, 1, m_crChannel);
	m_penColorDark.CreatePen(PS_SOLID, 1, m_crColorDark);
	m_penColorDarker.CreatePen(PS_SOLID, 1, m_crColorDarker);
	m_penDkShadow.CreatePen(PS_SOLID, 1, m_crDkShadow);
	m_penShadow.CreatePen(PS_SOLID, 1, m_crShadow);
	m_penLiteShadow.CreatePen(PS_SOLID, 1, m_crLiteShadow);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Function Header
COLORREF C3DBar::LightenColor(const COLORREF crColor, BYTE byIncreaseVal)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	BYTE byRed = GetRValue(crColor);
	BYTE byGreen = GetGValue(crColor);
	BYTE byBlue = GetBValue(crColor);

	if ((byRed + byIncreaseVal) <= 255)
		byRed = BYTE(byRed + byIncreaseVal);
	if ((byGreen + byIncreaseVal)	<= 255)
		byGreen = BYTE(byGreen + byIncreaseVal);
	if ((byBlue + byIncreaseVal) <= 255)
		byBlue = BYTE(byBlue + byIncreaseVal);

	return RGB(byRed, byGreen, byBlue);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Function Header
COLORREF C3DBar::DarkenColor(const COLORREF crColor, BYTE byReduceVal)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	BYTE byRed = GetRValue(crColor);
	BYTE byGreen = GetGValue(crColor);
	BYTE byBlue = GetBValue(crColor);

	if (byRed >= byReduceVal)
		byRed = BYTE(byRed - byReduceVal);
	if (byGreen >= byReduceVal)
		byGreen = BYTE(byGreen - byReduceVal);
	if (byBlue >= byReduceVal)
		byBlue = BYTE(byBlue - byReduceVal);

	return RGB(byRed, byGreen, byBlue);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Function Header
void C3DBar::SetBarColour(COLORREF cr)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	m_crChannel = cr;
	UpdateColours();
	CreatePens();
}
