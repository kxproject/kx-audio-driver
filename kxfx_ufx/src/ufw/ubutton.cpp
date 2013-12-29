
#include "ufw.h"
#include "ubutton.h"

//.............................................................................
// uButton

#define KXU_BUTTON_CLASSNAME _T("UFX-Button") // window class name

// uButton Class
uButton::uButton()
{
	kxuRegisterWindowClass(KXU_BUTTON_CLASSNAME);
	SETRECT(LmpRect, 0, 0, 0, 0);
}

uButton::~uButton()
{
}

BEGIN_MESSAGE_MAP(uButton, uControl)
	//{{AFX_MSG_MAP(uButton)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//.............................................................................

#define UB_TEXTFORMAT	(DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX)

void uButton::OnPaint() 
{
	CDC MemDC, TmpDC;
	CRect rect;
	CBitmap *pMemOld, *pTmpOld, bm, *pSrcBitmap;

	RECT DstRect;

	CString Caption;

	CPaintDC dc(this); // device context for painting

	// Draw

	// Get Size of Display area
    GetClientRect(rect);

    // Create memory DCs
    if (!TmpDC.CreateCompatibleDC(&dc)) return;
	MemDC.CreateCompatibleDC(&dc);
	bm.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
	pMemOld = (CBitmap*) MemDC.SelectObject(&bm);
    
    // Draw Button
	pSrcBitmap = &bmpNormal;
	if ((State & UC_STATE_PUSHED) | ((State & UC_STATE_CHECKED) & (!(Style & UBS_LAMP))))
	{
		pSrcBitmap = &bmpPressed;
	}

	pTmpOld = (CBitmap*) TmpDC.SelectObject(pSrcBitmap);
	MemDC.BitBlt(0, 0, rect.Width(), rect.Height(), &TmpDC, 0, 0, SRCCOPY);

	// Draw Lamp
	if (Style & UBS_LAMP)
	{
		CopyRect(&DstRect, &LmpRect);
		if (State & UC_STATE_PUSHED) {MOVERECT(DstRect, 1, 1)};
		pSrcBitmap = (State & UC_STATE_CHECKED) ? &bmpLampOn : &bmpLampOff;
		TmpDC.SelectObject(pSrcBitmap);
		MemDC.BitBlt(EXPANDRECTWH(DstRect), &TmpDC, 0, 0, SRCCOPY);
	}

	// Draw Text
	GetWindowText(Caption);
	if (!Caption.IsEmpty())
	{
		int fontindex = (Style & UBS_FONTMASK) >> UBS_FONTSHIFT;
		CopyRect(&DstRect, &Size); DstRect.left = LmpRect.right;
		DstRect.top += BoxBorder;
		DstRect.bottom -= BoxBorder; 
		// if (fontindex > 1) DstRect.bottom -= 1; // fixme, remove this - very dirty
		if (LmpRect.top) 
		{
			DstRect.left += LmpRect.top - BoxBorder;
			DstRect.right -= LmpRect.top - BoxBorder;		
		}
		else
		{
			DstRect.left += BoxBorder + uCore->GetAveCharWidth(fontindex);
			DstRect.right -= BoxBorder + uCore->GetAveCharWidth(fontindex);
		}

		MemDC.SelectObject(uCore->GetFont(fontindex));
		MemDC.SetBkMode(TRANSPARENT);
		MemDC.SetTextColor(CaptionColor);
		if ((State & UC_STATE_PUSHED) | ((State & UC_STATE_CHECKED) & (!(Style & UBS_LAMP))))
		{
			MOVERECT(DstRect, 1, 1);
		}

		MemDC.DrawText(Caption, &DstRect, (Style & UBS_TEXT_MASK) | UB_TEXTFORMAT);
	}

	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &MemDC, 0, 0, SRCCOPY);
	TmpDC.SelectObject(pTmpOld);
	MemDC.SelectObject(pMemOld);

	// Do not call CWnd::OnPaint() for painting messages
}

BOOL uButton::Create(LPCTSTR Caption, DWORD dwStyle, 
					 int x, int y, int w, int h, CWnd* pParentWnd, UINT nID)
{
	ASSERT(pParentWnd);
	pParentWindow = pParentWnd;
	mID = nID;
	LPCTSTR caption = Caption;
	Size = CRect(0, 0, w, h);

	Style = dwStyle & UCS_CONTROL_MASK;
	State = UC_STATE_DEFAULT;
	CaptionColor = uCore->GetColor(_UC_BLACK);
	LampColor = uCore->GetColor(_UC_HOVER);

	// Render Button

    RECT _LmpRect = {0, 0, 0, 0};
	int nItems = (Style & UBS_LAMP) ? 3 : 2;

	int ElementDefault[3] = {0, 0, UCB_OUTER_SUNKEN};
	int ElementPUSHED[3] = {0, 0, UCB_OUTER_SUNKEN};

	switch (Style & (UBS_FLAT))
	{
	case UBS_FLAT:
		ElementDefault[0] = UCB_OUTER_SUNKEN;
		ElementPUSHED[0] = UCB_OUTER_SUNKEN;
		ElementDefault[1] = UCB_BUTTON_RAISED;
		ElementPUSHED[1] = UCB_BUTTON_SUNKEN;
		break;
	default:
		ElementDefault[0] = UCB_OUTER_RAISED;
		ElementPUSHED[0] = UCB_OUTER_SUNKEN;
		ElementDefault[1] = UCB_BUTTON_RAISED;
		ElementPUSHED[1] = UCB_BUTTON_SUNKEN;
	}

	BoxBorder = uCore->GetElementData(ElementDefault[0], UED_BORDER);
	RECT BtnRect; CopyRect(&BtnRect, &Size);
	SCALERECT(BtnRect, -BoxBorder, -BoxBorder);
	LPRECT pRenderData[3] = {&Size, &BtnRect, &_LmpRect};
	
	// Render Lamp
	if (nItems == 3)
	{
		uCore->GetElementData(UCB_LAMP_ON, UED_SIZERECT, sizeof LmpRect, &LmpRect);
		kxuCenterRectToAnotherOne(&LmpRect, &BtnRect);
		int q = LmpRect.top - LmpRect.left;
		MOVERECT(LmpRect, q, 0);
		CopyRect(&_LmpRect, &LmpRect);
		SCALERECT(_LmpRect, BoxBorder, BoxBorder);
		SetLampColor(LampColor);
	}

	BoxBorder += uCore->GetElementData(ElementDefault[1], UED_BORDER);

	// Render Rised Button
	uCore->GetControlBitmap(&bmpNormal, 0, nItems, ElementDefault, pRenderData);

	// Render Sunken Button
	MOVERECT(_LmpRect, 1, 1);
	uCore->GetControlBitmap(&bmpPressed, 0, nItems, ElementPUSHED, pRenderData);

	BOOL ret = CWnd::Create(KXU_BUTTON_CLASSNAME, caption, dwStyle,
							CRect(x, y, x + w, y + h),
							pParentWnd, nID);
	return ret;
}

//.............................................................................

void uButton::OnLButtonDown(UINT /* nFlags */, CPoint /* point */) 
{
	SetCapture();
	State |= UC_STATE_PUSHED;
	if (Style & (UBS_2STATE | UBS_LAMP)) State ^= UC_STATE_CHECKED; // fixme, or placing this in OnLButtonUp would be better style?

	// SetFocus();
	REDRAW;

	// CWnd::OnLButtonDown(nFlags, point); // drop through to default handler
}

void uButton::OnLButtonUp(UINT /* nFlags */, CPoint /* point */) 
{
	ReleaseCapture();
	State &= ~UC_STATE_PUSHED;

	REDRAW;
	NotifyParent();

	// CWnd::OnLButtonUp(nFlags, point); // drop through to default handler
}

//.............................................................................
// Public Functions

void uButton::SetLampColor(COLORREF color)
{
	LampColor = color;
	if (LampColor & 0x80000000) LampColor = uCore->GetColor(~LampColor);

	
	if (!(Style & UBS_LAMP)) return;

	CBitmap bmpColorSrc;
	CWindowDC ScreenDC(NULL); // screen
	CDC TmpDC; TmpDC.CreateCompatibleDC(NULL);
	CBitmap *pOldBmp;

	// Get LampOn
	RECT _LmpRect = {0, 0, 0, 0};
	LPRECT pRenderData = &_LmpRect;
	int Element = UCB_LAMP_ON;
	uCore->GetControlBitmap(&bmpLampOn, 0, 1, &Element, &pRenderData);

	// Create DC with selected color
	bmpColorSrc.CreateCompatibleBitmap(&ScreenDC, _LmpRect.right, _LmpRect.bottom);
	pOldBmp = (CBitmap*) TmpDC.SelectObject(&bmpColorSrc);
	TmpDC.FillSolidRect(&_LmpRect, LampColor);
	TmpDC.SelectObject(pOldBmp);

	// Blend LampOn
	int Border = uCore->GetElementData(UCB_LAMP_ON, UED_BORDER);
	SCALERECT(_LmpRect, -Border, -Border);
	kxuBlend((HBITMAP) bmpLampOn, &_LmpRect, (HBITMAP) bmpColorSrc, 0, 0, BLEND_MULTIPLY);

	// Get LampOff
	SETRECT(_LmpRect, 0, 0, 0, 0);
	Element = UCB_LAMP_OFF;
	uCore->GetControlBitmap(&bmpLampOff, 0, 1, &Element, &pRenderData);
	
	// Blend LampOff
	Border = uCore->GetElementData(UCB_LAMP_OFF, UED_BORDER);
	SCALERECT(_LmpRect, -Border, -Border);
	kxuBlend((HBITMAP) bmpLampOff, &_LmpRect, (HBITMAP) bmpColorSrc, 0, 0, BLEND_MULTIPLY);
}

void uButton::SetWindowText(LPCTSTR lpszString)
{
	CWnd::SetWindowText(lpszString);
	REDRAW;
}

int uButton::GetCheck() const
{
	return State & UC_CHECK_MASK;
}

void uButton::SetCheck(int check)
{
	State &= ~UC_STATE_CHECKED; 
	State |= check & UC_STATE_CHECKED; 
	REDRAW;
}
