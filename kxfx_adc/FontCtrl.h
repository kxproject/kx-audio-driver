#if !defined(AFX_FONTCTRL_H__CA4DE73C_CDC9_11D3_B261_00104BB13A66__INCLUDED_)
#define AFX_FONTCTRL_H__CA4DE73C_CDC9_11D3_B261_00104BB13A66__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "TemplDef.h" // message map extensions for templates

///////////////////////////////////////////////////////////////////////////////
// class CFontCtrl
//
// Author: Yury Goltsman
// email:   ygprg@go.to
// page:    http://go.to/ygprg
// Copyright © 2000, Yury Goltsman
//
// This code provided "AS IS," without any kind of warranty.
// You may freely use or modify this code provided this
// Copyright is included in all derived versions.
//
// version : 1.1
// height added, 
// derived template CFontCtrlEx with initial style/height added
// derived templates CBoldCtrl, CItalicCtrl, CUnderlineCtrl, CStrikeoutCtrl added
// message map macro added
//
// version : 1.0
//

enum
{
	FC_FONT_BOLD      = 0x01,
	FC_FONT_ITALIC    = 0x02,
	FC_FONT_UNDERLINE = 0x04,
	FC_FONT_STRIKEOUT	= 0x08
};

///////////////////////////////////////////////////////////////////////////////
// internal class CFontCtrl

template<class BASE_TYPE = CWnd>
class CFontCtrl : public BASE_TYPE
{
public:
	CFontCtrl(){m_fAdd = m_fRemove = m_nHeight = 0;}
	virtual ~CFontCtrl(){}

public:
	void ChangeFontStyle(int fAdd, int fRemove = 0, BOOL fRedraw = TRUE);
	void ChangeFontHeight(int nHeight, BOOL fRedraw = TRUE);
	void SetFont(CFont* pFont, BOOL bRedraw = TRUE);
	void SetFont(LOGFONT& lf, BOOL bRedraw = TRUE);

protected:
	void RecreateFont(BOOL fRedraw = FALSE);
	CFont m_Font;
	int m_fAdd;
	int m_fRemove;
	int m_nHeight;
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFontCtrl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CFontCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_TEMPLATE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

BEGIN_TEMPLATE_MESSAGE_MAP(class BASE_TYPE, CFontCtrl<BASE_TYPE>, BASE_TYPE)
	//{{AFX_MSG_MAP(CFontCtrl)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_TEMPLATE_MESSAGE_MAP()

template<class BASE_TYPE>
void CFontCtrl<BASE_TYPE>::RecreateFont(BOOL fRedraw)
{
	if(!m_fAdd && !m_fRemove && !m_nHeight)
	{
		if(m_Font.m_hObject)
			BASE_TYPE::SetFont(&m_Font, fRedraw); // in case font was preset - set to control	
		return; // nothing to do
	}
	LOGFONT lf;
	if(!m_Font.m_hObject)
	{
		CFont* pFont = GetFont();
		if(!pFont)
			return; // nothing to do
		pFont->GetLogFont(&lf);
  }
	else
	{
	  m_Font.GetLogFont(&lf);
		m_Font.DeleteObject();
	}
	
	if(m_fAdd & m_fRemove)
		TRACE0("CFontCtrl: Warning! You try to add and remove the same styles.\n");
	
	if(m_fAdd & FC_FONT_BOLD)
		lf.lfWeight = FW_BOLD;
	if(m_fAdd & FC_FONT_ITALIC)
		lf.lfItalic = TRUE;
	if(m_fAdd & FC_FONT_UNDERLINE)
		lf.lfUnderline = TRUE;
	if(m_fAdd & FC_FONT_STRIKEOUT)
		lf.lfStrikeOut = TRUE;
	
	if(m_fRemove & FC_FONT_BOLD)
		lf.lfWeight = FW_NORMAL;
	if(m_fRemove & FC_FONT_ITALIC)
		lf.lfItalic = FALSE;
	if(m_fRemove & FC_FONT_UNDERLINE)
		lf.lfUnderline = FALSE;
	if(m_fRemove & FC_FONT_STRIKEOUT)
		lf.lfStrikeOut = FALSE;
	
	if(m_nHeight)
		lf.lfHeight = m_nHeight;

	m_Font.CreateFontIndirect(&lf);
	BASE_TYPE::SetFont(&m_Font, fRedraw);	
}

template<class BASE_TYPE>
void CFontCtrl<BASE_TYPE>::ChangeFontStyle(int fAdd, int fRemove, BOOL fRedraw)
{
	ASSERT(fAdd || fRemove);
	m_fAdd = fAdd;
	m_fRemove = fRemove;
	if(::IsWindow(m_hWnd))
		RecreateFont(fRedraw);
}

template<class BASE_TYPE>
void CFontCtrl<BASE_TYPE>::ChangeFontHeight(int nHeight, BOOL fRedraw)
{
	ASSERT(nHeight);
	m_nHeight = nHeight;
	if(::IsWindow(m_hWnd))
		RecreateFont(fRedraw);
}

template<class BASE_TYPE>
void CFontCtrl<BASE_TYPE>::SetFont(CFont* pFont, BOOL fRedraw)
{
	ASSERT(pFont);
	LOGFONT lf;
	pFont->GetLogFont(&lf);
	m_Font.CreateFontIndirect(&lf);
	if(::IsWindow(m_hWnd))
		RecreateFont(fRedraw);
}

template<class BASE_TYPE>
void CFontCtrl<BASE_TYPE>::SetFont(LOGFONT& lf, BOOL fRedraw)
{
	m_Font.CreateFontIndirect(&lf);
	if(::IsWindow(m_hWnd))
		RecreateFont(fRedraw);
}

template<class BASE_TYPE>
void CFontCtrl<BASE_TYPE>::PreSubclassWindow() 
{
	// create and set font
	RecreateFont();
	BASE_TYPE::PreSubclassWindow();
}

template<class BASE_TYPE>
int CFontCtrl<BASE_TYPE>::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (BASE_TYPE::OnCreate(lpCreateStruct) == -1)
		return -1;
	// create and set font
	RecreateFont();
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// class CFontCtrl

template<class BASE_TYPE, int InitialStyle = 0, int InitialHeight = 0>
class CFontCtrlEx : public CFontCtrl<BASE_TYPE>
{
public:
	CFontCtrlEx(){m_fAdd = InitialStyle; m_nHeight = InitialHeight;}
};

/////////////////////////////////////////////////////////////////////////////
// some derived classes 

template<class BASE_TYPE> class CBoldCtrl : public CFontCtrlEx<BASE_TYPE, FC_FONT_BOLD>{};
template<class BASE_TYPE> class CItalicCtrl : public CFontCtrlEx<BASE_TYPE, FC_FONT_ITALIC>{};
template<class BASE_TYPE> class CUnderlineCtrl : public CFontCtrlEx<BASE_TYPE, FC_FONT_UNDERLINE>{};
template<class BASE_TYPE> class CStrikeoutCtrl : public CFontCtrlEx<BASE_TYPE, FC_FONT_STRIKEOUT>{};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FONTCTRL_H__CA4DE73C_CDC9_11D3_B261_00104BB13A66__INCLUDED_)

