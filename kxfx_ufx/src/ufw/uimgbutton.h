
#ifndef _U_IMGBUTTON_H_
#define _U_IMGBUTTON_H_

//.............................................................................

// uImgButton styles

#define UIBS_2STATE	0x100

// uImgButton state frame order

/* 8 States and 8 Frames possible
	0 - N	- DEFAULT
	1 - C	- CHECKED
	2 - P	- PUSHED
	3 - CP	- CHECKED | PUSHED
	4 - F	- FOCUS
	5 - CF	- CHECKED | FOCUS
	6 - D	- DISABLED
	7 - CD  - CHECKED | DISABLED
*/

#define na 0 // not available, undefined, impossible
	
inline int STATEFRAMEORDER(int N, int C, int P, int CP, int F, int CF, int D, int CD)
{
	int ret = N;
	ret |= C << 3;
	ret |= P << 6;
	ret |= CP << 9;
	ret |= F << 12;
	ret |= CF << 15;
	ret |= D << 18;
	ret |= CD << 21;
	return ret;
}

//...................
// uImgButton window

class uImgButton : public uControl
{
public:
	uImgButton();
	virtual ~uImgButton();

public:
	int  GetCheck() const;
	void SetCheck(int check = true);

	BOOL Create(CBitmap* Image, int FrameOrder, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
	{return Create(Image, FrameOrder, dwStyle, EXPANDRECTWH(rect), pParentWnd, nID);};
	BOOL Create(CBitmap* Image, int FrameOrder, DWORD dwStyle, 
		int x, int y, int w, int h, CWnd* pParentWnd, UINT nID);
	
protected:
	int	State;
	int Order;

	CBitmap* bmpImage;

	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	DECLARE_MESSAGE_MAP()
};

#endif // _U_IMGBUTTON_H_
