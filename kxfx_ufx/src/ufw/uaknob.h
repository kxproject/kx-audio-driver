#ifndef _U_AKNOB_H_
#define _U_AKNOB_H_

//.............................................................................
// uLabel styles

#define UKS_DEFAULT			0x0000
#define UKS_SMALL			0x0001
#define UKS_SIZE_MASK		0x0003

#define UKS_WRAP			0x0010

/////////////////////////////////////////////////////////////////////////////
// uAKnob window

class uAKnob : public uControl
{
public:
	uAKnob();
	virtual ~uAKnob();

public:
    BOOL PreDrawKnob();
	int GetPageStepSize();
	int GetArrowStepSize();
	void SetPageStepSize(int StepSize);
	void SetArrowStepSize(int StepSize);
	int	 GetRangeMin() const;
	void SetRangeMin(int nMin);
	void SetLimitMin(int nMin);
	int  GetRangeMax() const;
	void SetRangeMax(int nMax);
	void SetLimitMax(int nMin);
	int  GetPos() const;
	void SetPos(int nPos);
	void GetRange(int& nMin, int& nMax) const;
	void SetRange(int nMin, int nMax);
	void SetLimit(int nMin, int nMax);
	void SetLimit() {LimitMin = RangeMin; LimitMax = RangeMax;};
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, int nMin = 0, int nMax = 100)
	{return Create(dwStyle, rect.left, rect.top, pParentWnd, nID, nMin, nMax, -150, 300);}
	BOOL Create(DWORD dwStyle, int x, int y, CWnd* pParentWnd, UINT nID)
	{return Create(dwStyle, x, y, pParentWnd, nID, 0, 100, -150, 300);}
	BOOL Create(DWORD dwStyle, int x, int y, CWnd* pParentWnd, UINT nID, int nMin, int nMax, int StartAngle = -150, int RotationAngle = 300);
	
protected:	
	RECT FilmSize;
	int nFrames;
	int sframe;
	int rframes;

	CBitmap bmpKnob;
	CBitmap* bmpDisabled;

	BOOL UserMovesRuler;
	LONG MouseDownY;
	LONG MouseDownX;
	LONG MouseDownPos;

	int Pos;
	int RangeMax;
	int RangeMin;
	int LimitMax;
	int LimitMin;

	int ArrowStepSize;
	int PageStepSize;
	int MouseMoveSence;

	COLORREF CaptionColor;

protected:
	int uAKnob::RenderDisabled();

	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnEnable(BOOL bEnable = TRUE);
	DECLARE_MESSAGE_MAP()
};

#endif // _U_AKNOB_H_
