
#include "ufw.h"
// #include "math.h"
#include "uvfader.h"

/////////////////////////////////////////////////////////////////////////////
// uVFader

#define KXU_VFADER_CLASSNAME _T("UFX-VFader") // Window class name

// uVFader Class
uVFader::uVFader()
{
	kxuRegisterWindowClass(KXU_VFADER_CLASSNAME);

	//local
	UserMovesRuler = 0;
	MouseDownY = 0;
	MouseDownPos = 0;
	Pos = 1;
	RangeMin = 0;
	RangeMax = 1;
}

uVFader::~uVFader()
{
}

BEGIN_MESSAGE_MAP(uVFader, uControl)
	//{{AFX_MSG_MAP(uVFader)
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

void uVFader::OnPaint() 
{
	CDC MemDC, TmpDC;
	CRect rect;
	CBitmap *pMemOld, *pTmpOld, bm;
	int dt = ButtonRect.top, db = ButtonRect.bottom, st = 0;

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
	// MemDC.FillSolidRect(&rect, 0xffffff);

	// Draw Button
	if (BoxBorder > dt) {st = BoxBorder - dt; dt = BoxBorder;};
	if ((Size.bottom - BoxBorder) < db) db = (Size.bottom - BoxBorder);
	TmpDC.SelectObject(&bmpButton);
    MemDC.BitBlt(0, dt, RECTW(ButtonSize), db - dt, &TmpDC, 0, st, SRCCOPY);

	if (!(Style & UFS_SWITCH))
	{
		// Draw Line BShadow
		if (LineRect.bottom > (ButtonRect.bottom - BShadowHeight))
		{
			dt = ButtonRect.bottom - BShadowHeight;
			db = LineRect.bottom;
			if (db > (dt + BShadowHeight)) db = dt + BShadowHeight;

			TmpDC.SelectObject(&bmpBShadow);
			MemDC.BitBlt(LineRect.left, dt, RECTW(LineRect), db - dt, &TmpDC, 0, 0, SRCCOPY);
		}

		// Draw Line TShadow
		if (LineRect.top < (ButtonRect.top + TShadowHeight))
		{
			st = 0;
			db = ButtonRect.top + TShadowHeight;
			dt = db - TShadowHeight;
			if (dt < LineRect.top) {dt = LineRect.top; st = LineRect.top - dt;}
			if ((db - dt) > 0)
			{
				TmpDC.SelectObject(&bmpTShadow);
				MemDC.BitBlt(LineRect.left, dt, RECTW(LineRect), db - dt, &TmpDC, 0, st, SRCCOPY);
			}
		}
	}

	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &MemDC, 0, 0, SRCCOPY);
	TmpDC.SelectObject(pTmpOld);
	MemDC.SelectObject(pMemOld);

	// Do not call CWnd::OnPaint() for painting messages
}

BOOL uVFader::Create(DWORD dwStyle, int x, int y, int w, int h, 
					 CWnd* pParentWnd, UINT nID, int nMin, int nMax)
{
	ASSERT(pParentWnd);
	pParentWindow = pParentWnd;
	mID = nID;
	int n;

	Style = dwStyle & UCS_CONTROL_MASK;

	UserMovesRuler = 0;

	// Correct width
	int q = BoxBorder = uCore->GetElementData(UCB_FADER_BOX, UED_SIZEWIDTH);
	if (w < q) w = q;

	RECT rect;
	int Element[2];
	RECT* pRenderData[2];

	// Render TShadow
	rect = CRect(0, 0, 0, 0);
	Element[0] = UCB_VFADER_LINE_TSHADOW;
	pRenderData[0] = &rect;
	uCore->GetControlBitmap(&bmpTShadow, 0, 1, Element, pRenderData);
	TShadowHeight = rect.bottom;

	// Render BShadow
	rect = CRect(0, 0, 0, 0);
	Element[0] = UCB_VFADER_LINE_BSHADOW;
	pRenderData[0] = &rect;
	uCore->GetControlBitmap(&bmpBShadow, 0, 1, Element, pRenderData);
	BShadowHeight = rect.bottom;

	// Render Button
	ButtonSize = CRect(0, 0, w, 0);
	Element[0] = (Style & UFS_SWITCH) ? UCB_VFADER_SWITCH : UCB_VFADER_BUTTON;
	pRenderData[0] = &ButtonSize;
	uCore->GetControlBitmap(&bmpButton, 0, 1, Element, pRenderData);
	CopyRect(&ButtonRect, &ButtonSize);

	// Render Box and Line
	BoxBorder = uCore->GetElementData(UCB_FADER_BOX, UED_BORDER);
	uCore->GetElementData(UCB_VFADER_LINE, UED_SIZERECT, sizeof RECT, &rect);
	int lh = h - (ButtonSize.bottom - (TShadowHeight + BShadowHeight)) - BoxBorder;
	rect.top = ((ButtonSize.bottom - (TShadowHeight + BShadowHeight) - 1) >> 1) + BoxBorder;
	rect.bottom = rect.top + lh;
	rect.left = ((w + 1) >> 1) - (rect.right >> 1);
	rect.right += rect.left;
	CopyRect(&LineRect, &rect); 

	Size = CRect(0, 0, w, h);
	Element[0] = UCB_FADER_BOX; 
	Element[1] = UCB_VFADER_LINE;
	pRenderData[0] = &Size; pRenderData[1] = &rect;
	n = (Style & UFS_SWITCH) ? 1 : 2;
	uCore->GetControlBitmap(&bmpBox, 0, n, Element, pRenderData);

	SetRange(nMin, nMax);
	SetPos((nMax + nMin) / 2);
	
	BOOL ret = CWnd::Create(KXU_VFADER_CLASSNAME, "", dwStyle,
							CRect(x, y, Size.right + x, Size.bottom + y),
							pParentWnd, nID);
	return ret;
}

// User Input

UINT uVFader::OnGetDlgCode() 
{
	return DLGC_WANTARROWS | CWnd::OnGetDlgCode();
}

void uVFader::OnKeyDown(UINT nChar, UINT /*nRepCnt*/, UINT /*nFlags*/) 
{
	switch (nChar)
	{
	case VK_DOWN:
		SetPos(Pos - ArrowStepSize); 
		break;
	case VK_UP:
		SetPos(Pos + ArrowStepSize); 
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

	NotifyParent();
	return;
}

void uVFader::OnMouseMove(UINT /*nFlags*/, CPoint point) 
{
	int ydelta;
	int p;

	// CWnd::OnMouseMove(nFlags, point); // drop through to default handler

	if (UserMovesRuler == TRUE) 
	{
		ydelta = MouseDownY - point.y;
		p = MouseDownPos + ((ydelta * (RangeMax - RangeMin)) / (RECTH(LineRect)));
		if (Pos == p) return;
		SetPos(p);
		NotifyParent();
	}

	return;
}

BOOL uVFader::OnMouseWheel(UINT /*nFlags*/, short zDelta, CPoint /*pt*/) 
{
	SetPos(Pos + ((zDelta / WHEEL_DELTA) * PageStepSize));
	NotifyParent();
	return TRUE; // CWnd::OnMouseWheel(nFlags, zDelta, pt); // drop through to default handler
}

void uVFader::OnLButtonDown(UINT /*nFlags*/, CPoint point) 
{
	RECT rect; CopyRect(&rect, &ButtonRect);
	rect.top += TShadowHeight - 2;
	rect.bottom -= BShadowHeight - 2;
	if (PtInRect(&rect, point)){
		UserMovesRuler = TRUE;
		MouseDownY = point.y;
		MouseDownPos = Pos;
		SetCapture();
	}
	else {
		SetPos(Pos + ((point.y < rect.bottom) ? +PageStepSize : -PageStepSize));
		NotifyParent();
	}
	SetFocus();

	// CWnd::OnLButtonDown(nFlags, point); // drop through to default handler
}

void uVFader::OnLButtonUp(UINT /*nFlags*/, CPoint /*point*/) 
{
	UserMovesRuler = FALSE;
	ReleaseCapture();
	// CWnd::OnLButtonUp(nFlags, point); // drop through to default handler
}

// Public Functions

void uVFader::SetPos(int nPos)
{
	if (nPos == Pos) return;
	Pos = nPos;
	SETBOUNDS(Pos, RangeMin, RangeMax);

	// calculate ruler top position
	int r = (RangeMax - RangeMin);

	ButtonRect.top = ((RECTH(LineRect)) * (RangeMax - Pos));
	if (r) 	ButtonRect.top /= r; 

	ButtonRect.bottom = ButtonRect.top + ButtonSize.bottom;

	if (GetSafeHwnd() != NULL) REDRAW;
}

int uVFader::GetPos() const
{
	return Pos;
}

void uVFader::SetRange(int nMin, int nMax)
{
	RangeMin = nMin;
	RangeMax = nMax;
	SETBOUNDS(Pos, RangeMin, RangeMax);
	ArrowStepSize = ((nMax - nMin) < 0) ? -1 : 1;
	PageStepSize = (nMax - nMin) / 10; if (PageStepSize == 0) PageStepSize = ArrowStepSize;
	if (GetSafeHwnd() != NULL) REDRAW;
}

void uVFader::GetRange(int &nMin, int &nMax) const
{
	nMin = RangeMin;
	nMax = RangeMax;
}

void uVFader::SetRangeMax(int nMax)
{
	SetRange(RangeMin, nMax);
}

int uVFader::GetRangeMax() const
{
	return RangeMax;
}

void uVFader::SetRangeMin(int nMin)
{
	SetRange(nMin, RangeMax);
}

int uVFader::GetRangeMin() const
{
	return RangeMin;
}

void uVFader::SetArrowStepSize(int StepSize)
{
	ArrowStepSize = StepSize;
}

int uVFader::GetArrowStepSize()
{
	return ArrowStepSize;
}

void uVFader::SetPageStepSize(int StepSize)
{
	PageStepSize = StepSize;
}

int uVFader::GetPageStepSize()
{
	return PageStepSize;
}
