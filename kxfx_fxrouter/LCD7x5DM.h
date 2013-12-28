#if !defined(AFX_LCD7X5DM_H__D2ECA1FF_A6AF_49F4_B275_C6410D27C342__INCLUDED_)
#define AFX_LCD7X5DM_H__D2ECA1FF_A6AF_49F4_B275_C6410D27C342__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LCD7x5DM.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLCD7x5DM window

#define LCD7X5DM_CLASSNAME    _T("LCD7x5DM")
#define LD_CHANGE		WM_USER + 1

class CLCD7x5DM : public CWnd
{
	DECLARE_DYNAMIC(CLCD7x5DM)

public:
	CLCD7x5DM();
	virtual ~CLCD7x5DM();
	BOOL Create(LPCTSTR lpszText, CPoint topLeft, CWnd *pParentWnd, UINT nID=0xFF, int nChars=1, BOOL bBackLight=FALSE); 
	BOOL GetBackLight();
	void SetBackLight(BOOL bBackLight = FALSE);
	void SetWindowText(LPCTSTR lpszString);
	
private:
	CBitmap m_bmpCaret;
	BOOL m_bInsert;	
	BOOL m_bBackLight;
	CString m_strUndo;
	CString m_strText;		
	CPoint m_nMaxCaretPos;
	CPoint m_nMinCaretPos;
	CPoint m_nCaretPos;
	CPoint m_ptCharOneStart;
	int m_nChars;
	int m_nCharPadding;
	int m_nCharHeight;
	int m_nCharWidth;
	int m_nCharPos;
	int m_nCtrlID;
	CWnd * m_pParentWnd;

	BOOL RegisterWindowClass();
	void DrawText(TCHAR charText,  CDC * pDC, CRect &rect);
	int CharFromPos(CPoint point);
	int PosFromChar(int nChar);

protected:			
	void OnTextChanged();
	//{{AFX_MSG(CLCD7x5DM)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnPaint();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);	
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LCD7X5DM_H__D2ECA1FF_A6AF_49F4_B275_C6410D27C342__INCLUDED_)
