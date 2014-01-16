
#include "ufw.h"
#include "ucheckbox.h"

/////////////////////////////////////////////////////////////////////////////
// uCheckBox

#define KXU_CHECKBOX_CLASSNAME _T("UFX-CheckBox") // Window class name

// uCheckBox Class
uCheckBox::uCheckBox()
{
	kxuRegisterWindowClass(KXU_CHECKBOX_CLASSNAME);
}

uCheckBox::~uCheckBox()
{
}

BEGIN_MESSAGE_MAP(uCheckBox, uControl)
	//{{AFX_MSG_MAP(uCheckBox)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

#define UC_TEXTFORMAT	(DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX)

void uCheckBox::OnPaint() 
{
	CDC MemDC, TmpDC;
	CRect rect;
	CBitmap *pMemOldBitmap, *pTmpOld, bm, *pSrcBitmap;

	RECT DstRect;
	POINT pt = {0, 0};

	CPaintDC dc(this); // device context for painting

	// Draw

	// Get Size of Display area
    GetClientRect(rect);

    // Create memory DCs
    if (!TmpDC.CreateCompatibleDC(&dc)) return;
	MemDC.CreateCompatibleDC(&dc);
	bm.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
	pMemOldBitmap = (CBitmap*) MemDC.SelectObject(&bm);

	CopyRect(&DstRect, &Size);

	// Draw Transparency
		MapWindowPoints(pParentWindow, &pt, 1);
		OffsetWindowOrgEx(MemDC.m_hDC, pt.x, pt.y, &pt);
		::SendMessage(pParentWindow->m_hWnd, WM_ERASEBKGND, (WPARAM) MemDC.m_hDC, 0);
		SetWindowOrgEx(MemDC.m_hDC, pt.x, pt.y, NULL);

	// Draw Lamp
	CopyRect(&DstRect, &LmpRect);
	pSrcBitmap = (State & UC_STATE_CHECKED) ? &bmpOn : &bmpOff;
	pTmpOld = (CBitmap*) TmpDC.SelectObject(pSrcBitmap);
	MemDC.BitBlt(EXPANDRECTWH(DstRect), &TmpDC, 0, 0, SRCCOPY);
	
/*
	char Caption[MAX_LABEL_STRING];
	int tcount = GetWindowText(Caption, MAX_LABEL_STRING);
	if (tcount) 
	{
		// Draw Text
		MemDC.SelectObject(uCore->GetFont((Style & ULS_FONTMASK) >> ULS_FONTSHIFT));
		MemDC.SetBkMode(TRANSPARENT);
		MemDC.SetTextColor(CaptionColor);
		MemDC.DrawText(Caption, tcount, &DstRect, Style & ULS_TEXT_MASK);
	}
*/

	dc.BitBlt(/*EXPANDRECTWH(DstRect)*/ 0, 0, rect.Width(), rect.Height(), &MemDC, 0, 0, SRCCOPY);
	
	TmpDC.SelectObject(pTmpOld);
	MemDC.SelectObject(pMemOldBitmap);

	// Do not call CWnd::OnPaint() for painting messages
}

BOOL uCheckBox::Create(LPCTSTR Caption, DWORD dwStyle, int x, int y, int w, int h, CWnd* pParentWnd, UINT nID)
{
	ASSERT(pParentWnd);
	pParentWindow = pParentWnd;
	mID = nID;

	Style = dwStyle & UCS_CONTROL_MASK;
	State = UC_STATE_DEFAULT;
	// CaptionColor = uCore->GetColor((Style & ULS_BOX) ? _UC_BLACK : _UC_WHITE);
	// ShadowColor = uCore->GetColor(_UC_DARK);
	LampColor = uCore->GetColor(_UC_HOVER);

	// Render
	SETRECT(Size, 0, 0, w, h);

	// Render Lamp
	SetLampColor(LampColor);
	
	BOOL ret = CWnd::Create(KXU_CHECKBOX_CLASSNAME, Caption, dwStyle & UCS_WINDOW_MASK,
							CRect(x, y, x + w, y + h),
							pParentWnd, nID);
	return ret;
}

//.............................................................................
// Message Handlers

void uCheckBox::OnLButtonDown(UINT /* nFlags */, CPoint /* point */) 
{
	SetCapture();
	State ^= UC_STATE_CHECKED;

	// SetFocus();
	REDRAW;
	NotifyParent(0, State & UC_STATE_CHECKED);

	// CWnd::OnLButtonDown(nFlags, point); // drop through to default handler
}

void uCheckBox::OnLButtonUp(UINT /* nFlags */, CPoint /* point */) 
{
	ReleaseCapture();

	REDRAW;
	
	// CWnd::OnLButtonUp(nFlags, point); // drop through to default handler
}

//.............................................................................
// Public Functions

void uCheckBox::SetLampColor(COLORREF color)
{
	LampColor = color;
	if (LampColor & 0x80000000) LampColor = uCore->GetColor(~LampColor);

	
	RECT _LmpRect = {0, 0, 0, 0};
	uCore->GetElementData(UCB_LAMP_ON, UED_SIZERECT, sizeof LmpRect, &LmpRect);
	CopyRect(&_LmpRect, &LmpRect);
	int Border = uCore->GetElementData(UCB_OUTER_SUNKEN, UED_BORDER);
	RESIZERECT(LmpRect, 0, 0, Border << 1, Border << 1);
	MOVERECT(_LmpRect, Border, Border);

	CBitmap bmpColorSrc;
	CWindowDC ScreenDC(NULL); // screen
	CDC TmpDC; TmpDC.CreateCompatibleDC(NULL);
	CBitmap *pOldBmp;

	// Get LampOn
	LPRECT pRenderData[2] = {&LmpRect, &_LmpRect};
	int Element[2] = {UCB_OUTER_SUNKEN, UCB_LAMP_ON};
	uCore->GetControlBitmap(&bmpOn, 0, 2, Element, pRenderData);

	// Create DC with selected color
	bmpColorSrc.CreateCompatibleBitmap(&ScreenDC, _LmpRect.right, _LmpRect.bottom);
	pOldBmp = (CBitmap*) TmpDC.SelectObject(&bmpColorSrc);
	TmpDC.FillSolidRect(0, 0, _LmpRect.right, _LmpRect.bottom, LampColor);
	TmpDC.SelectObject(pOldBmp);

	// Get LampOff
	Element[1] = UCB_LAMP_OFF;
	uCore->GetControlBitmap(&bmpOff, 0, 2, Element, pRenderData);
	
	// Blend Lamps
	Border = uCore->GetElementData(UCB_LAMP_ON, UED_BORDER);
	SCALERECT(_LmpRect, -Border, -Border);
	kxuBlend((HBITMAP) bmpOn, &_LmpRect, (HBITMAP) bmpColorSrc, 0, 0, BLEND_MULTIPLY);
	kxuBlend((HBITMAP) bmpOff, &_LmpRect, (HBITMAP) bmpColorSrc, 0, 0, BLEND_MULTIPLY);

	// Calc LampRect
	int t = (RECTH(Size) - RECTH(LmpRect)) >> 1;
	MOVERECT(LmpRect, 0, t);
}

int uCheckBox::GetCheck() const
{
	return State & UC_CHECK_MASK;
}

void uCheckBox::SetCheck(int check)
{
	State &= ~UC_STATE_CHECKED; 
	State |= check & UC_STATE_CHECKED; 
	REDRAW;
}

/*
void uCheckBox::SetWindowText(LPCTSTR lpszString)
{
	CWnd::SetWindowText(lpszString);
	REDRAW;
}


void uCheckBox::SetColor(COLORREF color)
{
	if (color & 0x80000000) color = uCore->GetColor(~color);
	CaptionColor = color;
	REDRAW;
}

void uCheckBox::SetShadowColor(COLORREF color)
{
	if (color & 0x80000000) color = uCore->GetColor(~color);
	ShadowColor = color;
	REDRAW;
}
*/

