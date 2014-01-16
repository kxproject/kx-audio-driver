
#include "ufw.h"
#include "ulabel.h"

/////////////////////////////////////////////////////////////////////////////
// uLabel

#define KXU_LABEL_CLASSNAME _T("UFX-Label") // Window class name

// uLabel Class
uLabel::uLabel()
{
	kxuRegisterWindowClass(KXU_LABEL_CLASSNAME);
}

uLabel::~uLabel()
{
}

BEGIN_MESSAGE_MAP(uLabel, uControl)
	//{{AFX_MSG_MAP(uLabel)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_NCHITTEST()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

#define UL_TEXTFORMAT	(DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX)

void uLabel::OnPaint() 
{
	CDC MemDC, TmpDC;
	CRect rect;
	CBitmap *pMemOldBitmap, *pTmpOld, bm;

	RECT DstRect;
	POINT  pt = {0, 0};

	char Caption[MAX_LABEL_STRING];

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

	if (Style & ULS_BOX) {
		// Draw Box
		pTmpOld = (CBitmap*) TmpDC.SelectObject(&bmpBox);
		MemDC.BitBlt(0, 0, rect.Width(), rect.Height(), &TmpDC, 0, 0, SRCCOPY);
		TmpDC.SelectObject(pTmpOld);
		int chex = uCore->GetAveCharWidth();
		RESIZERECT(DstRect, BoxBorder + chex, BoxBorder, - (BoxBorder + chex - 1), -BoxBorder);
	}
	else {
		// Transparency
		MapWindowPoints(pParentWindow, &pt, 1);
		OffsetWindowOrgEx(MemDC.m_hDC, pt.x, pt.y, &pt);
		::SendMessage(pParentWindow->m_hWnd, WM_ERASEBKGND, (WPARAM) MemDC.m_hDC, 0);
		SetWindowOrgEx(MemDC.m_hDC, pt.x, pt.y, NULL);
	}
	
	// Draw Text
	int tcount = GetWindowText(Caption, MAX_LABEL_STRING);
	MemDC.SelectObject(uCore->GetFont((Style & ULS_FONTMASK) >> ULS_FONTSHIFT));
	MemDC.SetBkMode(TRANSPARENT);
	if (Style & ULS_SHADOW)
	{
		MOVERECT(DstRect, 1, 1);
		MemDC.SetTextColor(ShadowColor);
		MemDC.DrawText(Caption, tcount, &DstRect, Style & ULS_TEXT_MASK);
		MOVERECT(DstRect, -1, -1);
	}
	MemDC.SetTextColor(CaptionColor);
	MemDC.DrawText(Caption, tcount, &DstRect, Style & ULS_TEXT_MASK);

	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &MemDC, 0, 0, SRCCOPY);
	
	MemDC.SelectObject(pMemOldBitmap);

	// Do not call CWnd::OnPaint() for painting messages
}

BOOL uLabel::Create(LPCTSTR Caption, DWORD dwStyle, int x, int y, int w, int h, CWnd* pParentWnd, UINT nID)
{
	ASSERT(pParentWnd);
	pParentWindow = pParentWnd;
	mID = nID;

	Style = (dwStyle & UCS_CONTROL_MASK) | UL_TEXTFORMAT;
	CaptionColor = uCore->GetColor(_UC_BLACK);
	ShadowColor = uCore->GetColor(_UC_DARK);

	// Render Button
	BoxBorder = 0;
	
	RECT xrect;
	SETRECT(Size, 0, 0, w, h);
	
	if (Style & ULS_BOX)
	{	
		BoxBorder = uCore->GetElementData(UCB_OUTER_SUNKEN, UED_BORDER);
		SETRECT(xrect, 0, 0, w, h);
		SCALERECT(xrect, -BoxBorder, -BoxBorder);
		int Element[2] = {UCB_OUTER_SUNKEN, UCB_BOX_WHITE};
		LPRECT pRenderData[2] = {&Size, &xrect};
		uCore->GetControlBitmap(&bmpBox, 0, 2, Element, pRenderData);
		BoxBorder += uCore->GetElementData(UCB_BOX_WHITE, UED_BORDER);
	}
	else
	{
		CaptionColor += 0x090909;
		ShadowColor = (ShadowColor + uCore->GetColor(_UC_GREY)) >> 1;
	}
	
	BOOL ret = CWnd::Create(KXU_LABEL_CLASSNAME, Caption, dwStyle & UCS_WINDOW_MASK,
							CRect(x, y, x + w, y + h),
							pParentWnd, nID);
	return ret;
}

void uLabel::SetWindowText(LPCTSTR lpszString)
{
	CWnd::SetWindowText(lpszString);
	REDRAW;
}

void uLabel::printf(LPCTSTR format, ...)
{
	char msg[MAX_LABEL_STRING];
	va_list arglist;
    va_start(arglist, format);
	/*w*/vsprintf(msg, format, arglist);
	CWnd::SetWindowText(msg);
	REDRAW;
}

void uLabel::SetColor(COLORREF color)
{
	if (color & 0x80000000) color = uCore->GetColor(~color);
	CaptionColor = color;
	REDRAW;
}

void uLabel::SetShadowColor(COLORREF color)
{
	if (color & 0x80000000) color = uCore->GetColor(~color);
	ShadowColor = color;
	REDRAW;
}

//.............................................................................
// Message Handlers
UINT uLabel::OnNcHitTest(CPoint /*point*/)
{
	return (UINT) HTTRANSPARENT;
}
