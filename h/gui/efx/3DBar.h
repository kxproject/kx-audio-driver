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

// 3DBar.h: interface for the C3DBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_3DBAR_H__E77033B8_CCDE_11D3_8041_00805FC1DE10__INCLUDED_)
#define AFX_3DBAR_H__E77033B8_CCDE_11D3_8041_00805FC1DE10__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"

class kCLASS_TYPE C3DBar  
{
public:
	void SetBarColour(COLORREF cr);
	void UpdateColours();
	void CreatePens();
	void DrawHorizontal(CDC* pDC, CRect& BarRect);

	C3DBar();
	virtual ~C3DBar();

protected:
	void DeletePens();
	COLORREF DarkenColor(const COLORREF crColor, BYTE byReduceVal);
	COLORREF LightenColor(const COLORREF crColor, BYTE byIncreaseVal);
	COLORREF m_crChannel;
	COLORREF m_crColorLight;
	COLORREF m_crColorLighter;
	COLORREF m_crColorLightest;
	COLORREF m_crColorDark;
	COLORREF m_crColorDarker;
	COLORREF m_crDkShadow;
	COLORREF m_crShadow;
	COLORREF m_crLiteShadow;

	CPen m_penColor;
	CPen m_penColorLight;
	CPen m_penColorLighter;
	CPen m_penColorDark;
	CPen m_penColorDarker;
	CPen m_penDkShadow;
	CPen m_penShadow;
	CPen m_penLiteShadow;
};

#endif // !defined(AFX_3DBAR_H__E77033B8_CCDE_11D3_8041_00805FC1DE10__INCLUDED_)
