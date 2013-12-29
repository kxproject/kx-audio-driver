
#ifndef _U_CHECKBOX_H_
#define _U_CHECKBOX_H_

//.............................................................................
// uCheckBox styles

#define UCS_HEADERFONT		0x1000
#define UCS_SMALLFONT		0x2000
#define UCS_FATSMALLFONT	0x3000
#define UCS_FONTMASK		0xF000
#define UCS_FONTSHIFT		0xC
 
#define MAX_LABEL_STRING	128

//.............................................................................
// uCheckBox Class

class uCheckBox : public uControl
{
public:
	uCheckBox();
	virtual ~uCheckBox();

public:
	// void SetShadowColor(COLORREF color);
	// void SetColor(COLORREF color);
	// void SetWindowText(LPCTSTR lpszString);
	void uCheckBox::SetLampColor(COLORREF color);
	int  GetCheck() const;
	void SetCheck(int check = true);
	BOOL Create(LPCTSTR Caption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
	{return Create(Caption, dwStyle, EXPANDRECTWH(rect), pParentWnd, nID);}
	BOOL Create(LPCTSTR Caption, DWORD dwStyle, int x, int y, int w, int h, CWnd* pParentWnd, UINT nID);
	
protected:

	CBitmap bmpOff;
	CBitmap bmpOn;

	RECT LmpRect;

	COLORREF LampColor;
	// COLORREF CaptionColor;
	// COLORREF ShadowColor;

	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* /* pDC */) {return 1;}; // fixme, works buggy without that
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#endif // _U_CHECKBOX_H_

