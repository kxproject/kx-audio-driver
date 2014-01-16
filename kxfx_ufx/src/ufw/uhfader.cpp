
#include "ufw.h"
// #include "math.h"
#include "uhfader.h"

/////////////////////////////////////////////////////////////////////////////
// uHFader

#define KXU_HFADER_CLASSNAME _T("UFX-HFader") // Window class name

// uHFader Class
uHFader::uHFader()
{
	kxuRegisterWindowClass(KXU_HFADER_CLASSNAME);

	//local
	UserMovesRuler = 0;
	MouseDownX = 0;
	MouseDownPos = 0;
	Pos = 1;
	RangeMin = 0;
	RangeMax = 1;
}

uHFader::~uHFader()
{
}

BEGIN_MESSAGE_MAP(uHFader, uControl)
	//{{AFX_MSG_MAP(uHFader)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_GETDLGCODE()
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void uHFader::OnPaint() 
{
	CDC MemDC, TmpDC;
	CRect rect;
	CBitmap *pMemOld, *pTmpOld, bm;
	int dl = ButtonRect.left, dr = ButtonRect.right, sl = 0;

	CPaintDC dc(this); // device context for painting

	// Draw

	// Get Size of Display area
    GetClientRect(rect);

    // Create memory DCs
    if (!TmpDC.CreateCompatibleDC(&dc)) return;
	MemDC.CreateCompatibleDC(&dc);
	bm.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
	pMemOld = (CBitmap*) MemDC.SelectObject(&bm);
    
    // Draw Box
	pTmpOld = (CBitmap*) TmpDC.SelectObject(&bmpBox);
	MemDC.BitBlt(0, 0, rect.Width(), rect.Height(), &TmpDC, 0, 0, SRCCOPY);

	// Draw Button
	if (BoxBorder > dl) {sl = BoxBorder - dl; dl = BoxBorder;};
	if ((Size.right - BoxBorder) < dr) dr = (Size.right - BoxBorder);
	TmpDC.SelectObject(&bmpButton);
    MemDC.BitBlt(dl, 0, dr - dl, RECTH(ButtonSize), &TmpDC, sl, 0, SRCCOPY);

	// Draw Line RShadow
	if (LineRect.right > (ButtonRect.right - RShadowWidth))
	{
		dl = ButtonRect.right - RShadowWidth;
		dr = LineRect.right;
		if (dr > (dl + RShadowWidth)) dr = dl + RShadowWidth;

		TmpDC.SelectObject(&bmpRShadow);
		MemDC.BitBlt(dl, LineRect.top, dr - dl, RECTH(LineRect), &TmpDC, 0, 0, SRCCOPY);
	}

	// Draw Line LShadow
	if (LineRect.left < (ButtonRect.left + LShadowWidth))
	{
		sl = 0;
		dr = ButtonRect.left + LShadowWidth;
		dl = dr - LShadowWidth;
		if (dl < LineRect.left) {dl = LineRect.left; sl = LineRect.left - dl;}
		if ((dr - dl) > 0)
		{
			TmpDC.SelectObject(&bmpLShadow);
			MemDC.BitBlt(dl, LineRect.top, dr - dl, RECTH(LineRect), &TmpDC, sl, 0, SRCCOPY);
		}
	}

	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &MemDC, 0, 0, SRCCOPY);
	TmpDC.SelectObject(pTmpOld);
	MemDC.SelectObject(pMemOld);

	// Do not call CWnd::OnPaint() for painting messages
}

BOOL uHFader::Create(DWORD dwStyle, int x, int y, int w, int h, 
					 CWnd* pParentWnd, UINT nID, int nMin, int nMax)
{
	ASSERT(pParentWnd);
	pParentWindow = pParentWnd;
	mID = nID;

	UserMovesRuler = 0;

	// Correct width
	int q = uCore->GetElementData(UCB_HFADER_BUTTON, UED_SIZEHEIGHT);
	if (h < q) h = q;

	// Render LShadow
	RECT rect = CRect(0, 0, 0, 0);
	int Element[2] = {UCB_HFADER_LINE_LSHADOW};
	RECT* pRenderData[2] = {&rect};
	uCore->GetControlBitmap(&bmpLShadow, 0, 1, Element, pRenderData);
	LShadowWidth = rect.right;

	// Render RShadow
	rect = CRect(0, 0, 0, 0);
	Element[0] = UCB_HFADER_LINE_RSHADOW;
	pRenderData[0] = &rect;
	uCore->GetControlBitmap(&bmpRShadow, 0, 1, Element, pRenderData);
	RShadowWidth = rect.right;

	// Render Button
	ButtonSize = CRect(0, 0, 0, h);
	Element[0] = UCB_HFADER_BUTTON;
	pRenderData[0] = &ButtonSize;
	uCore->GetControlBitmap(&bmpButton, 0, 1, Element, pRenderData);
	CopyRect(&ButtonRect, &ButtonSize);

	// Render Box and Line
	BoxBorder = uCore->GetElementData(UCB_FADER_BOX, UED_BORDER);
	uCore->GetElementData(UCB_HFADER_LINE, UED_SIZERECT, sizeof RECT, &rect);
	int lw = w - (ButtonSize.right - (LShadowWidth + RShadowWidth)) - BoxBorder;
	rect.left = ((ButtonSize.right - (LShadowWidth + RShadowWidth) - 1) >> 1) + BoxBorder;
	rect.right = rect.left + lw;
	rect.top = ((h + 1) >> 1) - (rect.bottom >> 1);
	rect.bottom += rect.top;
	CopyRect(&LineRect, &rect); 

	Size = CRect(0, 0, w, h);
	Element[0] = UCB_FADER_BOX; Element[1] = UCB_HFADER_LINE;
	pRenderData[0] = &Size; pRenderData[1] = &rect;
	uCore->GetControlBitmap(&bmpBox, 0, 2, Element, pRenderData);

	SetRange(nMin, nMax);
	SetPos((nMax + nMin) / 2);
	
	BOOL ret = CWnd::Create(KXU_HFADER_CLASSNAME, "", dwStyle,
							CRect(x, y, Size.right + x, Size.bottom + y),
							pParentWnd, nID);
	return ret;
}

// User Input


UINT uHFader::OnGetDlgCode() 
{
	return DLGC_WANTARROWS | CWnd::OnGetDlgCode();
}

void uHFader::OnKeyDown(UINT nChar, UINT /*nRepCnt*/, UINT /*nFlags*/) 
{
	switch (nChar)
	{
	case VK_UP:
	case VK_RIGHT:
		SetPos(Pos + ArrowStepSize); 
		break;
	case VK_DOWN:
	case VK_LEFT:
		SetPos(Pos - ArrowStepSize); 
		break;
	case VK_NEXT:
		SetPos(Pos - PageStepSize); 
		break;
	case VK_PRIOR:
		SetPos(Pos + PageStepSize); 
		break;
	default:
		// CWnd::OnKeyDown(nChar, nRepCnt, nFlags); // drop through to default handler
		return;
	}

	NotifyParent(0, Pos);
	return;
}

void uHFader::OnMouseMove(UINT /*nFlags*/, CPoint point) 
{
	int xdelta;
	int p;

	// CWnd::OnMouseMove(nFlags, point); // drop through to default handler

	if (UserMovesRuler == TRUE) 
	{
		xdelta = MouseDownX - point.x;
		p = MouseDownPos - ((xdelta * (RangeMax - RangeMin)) / (RECTW(LineRect)));
		if (Pos == p) return;
		SetPos(p);
		NotifyParent(0, Pos);
	}

	return;
}

BOOL uHFader::OnMouseWheel(UINT /*nFlags*/, short zDelta, CPoint /*pt*/) 
{
	SetPos(Pos + ((zDelta / WHEEL_DELTA) * PageStepSize));
	NotifyParent(0, Pos);
	return TRUE; // CWnd::OnMouseWheel(nFlags, zDelta, pt); // drop through to default handler
}

void uHFader::OnLButtonDown(UINT /*nFlags*/, CPoint point) 
{
	if (PtInRect(&ButtonRect, point)){
		UserMovesRuler = TRUE;
		MouseDownX = point.x;
		MouseDownPos = Pos;
		SetCapture();
	}
	else {
		SetPos(Pos - ((point.x < ButtonRect.right) ? +PageStepSize : -PageStepSize));
		NotifyParent(0, Pos);
	}
	SetFocus();

	// CWnd::OnLButtonDown(nFlags, point); // drop through to default handler
}

void uHFader::OnLButtonUp(UINT /*nFlags*/, CPoint /*point*/) 
{
	UserMovesRuler = FALSE;
	ReleaseCapture();
	// CWnd::OnLButtonUp(nFlags, point); // drop through to default handler
}

// Public Functions

void uHFader::SetPos(int nPos)
{
	if (nPos == Pos) return;
	Pos = nPos;
	SETBOUNDS(Pos, RangeMin, RangeMax);

	// calculate ruler top position
	int r = (RangeMax - RangeMin);

	ButtonRect.left = ((RECTW(LineRect)) * (Pos - RangeMin));
	if (r) 	ButtonRect.left /= r; 

	ButtonRect.right = ButtonRect.left + ButtonSize.right;

	if (GetSafeHwnd() != NULL) REDRAW;
}

int uHFader::GetPos() const
{
	return Pos;
}

void uHFader::SetRange(int nMin, int nMax)
{
	RangeMin = nMin;
	RangeMax = nMax;
	SETBOUNDS(Pos, RangeMin, RangeMax);
	ArrowStepSize = ((nMax - nMin) < 0) ? -1 : 1;
	PageStepSize = (nMax - nMin) / 10; if (PageStepSize == 0) PageStepSize = ArrowStepSize;
	if (GetSafeHwnd() != NULL) REDRAW;
}

void uHFader::GetRange(int &nMin, int &nMax) const
{
	nMin = RangeMin;
	nMax = RangeMax;
}

void uHFader::SetRangeMax(int nMax)
{
	SetRange(RangeMin, nMax);
}

int uHFader::GetRangeMax() const
{
	return RangeMax;
}

void uHFader::SetRangeMin(int nMin)
{
	SetRange(nMin, RangeMax);
}

int uHFader::GetRangeMin() const
{
	return RangeMin;
}

void uHFader::SetArrowStepSize(int StepSize)
{
	ArrowStepSize = StepSize;
}

int uHFader::GetArrowStepSize()
{
	return ArrowStepSize;
}

void uHFader::SetPageStepSize(int StepSize)
{
	PageStepSize = StepSize;
}

int uHFader::GetPageStepSize()
{
	return PageStepSize;
}
