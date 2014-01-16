
#ifndef _U_BUTTON_H_
#define _U_BUTTON_H_

//.............................................................................
   
// uButton Styles
#define UBS_LEFT			0x0000
#define UBS_CENTER			0x0001
#define UBS_RIGHT			0x0002

#define UBS_TEXT_MASK		0x00FF

#define UBS_2STATE			0x0100
#define UBS_LAMP			0x0200
#define UBS_FLAT			0x0400
#define UBS_LIGHT_SURFACE	0x0800 // ~undocumented?

#define UBS_HEADERFONT		0x1000
#define UBS_SMALLFONT		0x2000
#define UBS_FATSMALLFONT	0x3000
#define UBS_FONTMASK		0xf000
#define UBS_FONTSHIFT		0xc

//...................
// uButton window

class uButton : public uControl
{
public:
	uButton();
	virtual ~uButton();

public:
	void SetLampColor(COLORREF color);
	void SetWindowText(LPCTSTR lpszString);
	int  GetCheck() const;
	void SetCheck(int check = true);

	BOOL Create(LPCTSTR Caption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
	{return Create(Caption, dwStyle, EXPANDRECTWH(rect), pParentWnd, nID);};
	BOOL Create(LPCTSTR Caption, DWORD dwStyle, int x, int y, int w, int h, CWnd* pParentWnd, UINT nID);
	
protected:

	RECT LmpRect;
	int BoxBorder;

	CBitmap bmpNormal;
	CBitmap bmpPressed;
	CBitmap bmpLampOn;
	CBitmap bmpLampOff;
	
	COLORREF LampColor;
	COLORREF CaptionColor; 
	
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#endif // _U_BUTTON_H_
