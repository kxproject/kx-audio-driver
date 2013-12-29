
#ifndef _U_HFADER_H_
#define _U_HFADER_H_

/////////////////////////////////////////////////////////////////////////////
// uHFader window

class uHFader : public uControl
{
public:
	uHFader();
	virtual ~uHFader();

public:
	int  GetPageStepSize();
	int  GetArrowStepSize();
	void SetPageStepSize(int StepSize);
	void SetArrowStepSize(int StepSize);
	int	 GetRangeMin() const;
	void SetRangeMin(int nMin);
	int  GetRangeMax() const;
	void SetRangeMax(int nMax);
	int  GetPos() const;
	void SetPos(int nPos);
	void GetRange(int& nMin, int& nMax) const;
	void SetRange(int nMin, int nMax);
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, int nMin = 0, int nMax = 100)
	{return Create(dwStyle, EXPANDRECTWH(rect), pParentWnd, nID, nMin, nMax);}
	BOOL Create(DWORD dwStyle, int x, int y, int w, int h, CWnd* pParentWnd, UINT nID,
				int nMin = 0, int nMax = 100);

protected:	
	// RECT Size;
	RECT ButtonSize;

    BOOL UserMovesRuler;
	LONG MouseDownX;
	LONG MouseDownPos;

	CBitmap bmpBox;
	CBitmap bmpButton;
	CBitmap bmpRShadow;
	CBitmap bmpLShadow;

	RECT ButtonRect;
	RECT LineRect;

	LONG BoxBorder;
	LONG RShadowWidth;
	LONG LShadowWidth;

	int Pos;
	int RangeMax;
	int RangeMin;
	
	int ArrowStepSize;
	int PageStepSize;

protected:

	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	DECLARE_MESSAGE_MAP()
};

#endif // _U_HFADER_H_
