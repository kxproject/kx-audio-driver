
#ifndef _U_LABEL_H_
#define _U_LABEL_H_

//.............................................................................
// uLabel styles

#define ULS_LEFT			0x0000
#define ULS_CENTER			0x0001
#define ULS_RIGHT			0x0002

#define ULS_TEXT_MASK		0x00FF

#define ULS_BOX				0x0100
#define ULS_SHADOW			0x0200

#define ULS_HEADERFONT		0x1000
#define ULS_SMALLFONT		0x2000
#define UBS_FATSMALLFONT	0x3000
#define ULS_FONTMASK		0xF000
#define ULS_FONTSHIFT		0xC
 
#define MAX_LABEL_STRING	128

//.............................................................................
// uLabel Class

class uLabel : public uControl
{
public:
	uLabel();
	virtual ~uLabel();

public:
	void SetShadowColor(COLORREF color);
	void SetColor(COLORREF color);
	void printf(LPCTSTR format, ...);
	void SetWindowText(LPCTSTR lpszString);
	BOOL Create(LPCTSTR Caption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
	{return Create(Caption, dwStyle, EXPANDRECTWH(rect), pParentWnd, nID);}
	BOOL Create(LPCTSTR Caption, DWORD dwStyle, int x, int y, int w, int h, CWnd* pParentWnd, UINT nID);
	
protected:
	int BoxBorder;

	CBitmap bmpBox;

	COLORREF CaptionColor;
	COLORREF ShadowColor;

	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* /* pDC */) {return 1;}; // fixme, works buggy without that
	afx_msg UINT OnNcHitTest(CPoint point);
	DECLARE_MESSAGE_MAP()
};

#endif // _U_LABEL_H_

