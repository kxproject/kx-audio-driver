#if !defined(AFX_SWAPINDICATOR_H__E52C60B6_C75A_44FB_B7C1_507EF66BAA8D__INCLUDED_)
#define AFX_SWAPINDICATOR_H__E52C60B6_C75A_44FB_B7C1_507EF66BAA8D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SwapIndicator.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSwapIndicator window

#define SWAPINDICATOR_CLASSNAME    _T("SwapIndicator")
#define SI_SWAP		WM_USER + 1

typedef struct tagSIHDR {
    NMHDR hdr;   
    UINT idDropSource;  
	UINT idDropTarget;
	CWnd * pWndSource;
	CWnd * pWndTarget;
} SIHDR;

class CSwapIndicator : public CWnd
{	
	DECLARE_DYNAMIC(CSwapIndicator)

// Construction
public:
	CSwapIndicator();
	virtual ~CSwapIndicator();
	
	UINT m_nCtrlID;
	CWnd* m_pParentWnd;
	BOOL Create(const RECT& rect, CWnd* pParentWnd, UINT nID = 0xFFFF, UINT nContextID = 0xFFFF);
	BOOL Create(CPoint topLeft, CWnd* pParentWnd, UINT nID = 0xFFFF, UINT nContextID = 0xFFFF);
	void SetBitmaps(HBITMAP arrBitmaps[3]);	

private:
	
	BOOL m_bIsDragging;	
	HBITMAP m_bmpCurrent;
	HBITMAP m_bmpNormal;
	HBITMAP m_bmpDrag;
	HBITMAP m_bmpDragOver;
	UINT m_nContextID;
	HCURSOR m_Cursor;
	HCURSOR m_DragCursor;
	CSwapIndicator * m_pDropWnd;	

	void OnDrop(CSwapIndicator* pDropSource, CSwapIndicator* pDropTarget);
	BOOL RegisterWindowClass();
	HBITMAP SetBitmap(HBITMAP hBitmap);

	// Generated message map functions
protected:	
	//{{AFX_MSG(CSwapIndicator)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();	
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SWAPINDICATOR_H__E52C60B6_C75A_44FB_B7C1_507EF66BAA8D__INCLUDED_)
