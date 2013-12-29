
#include "ufw.h"
#include "uaknob.h"

//.............................................................................
// uAKnob

#define KXU_AKNOB_CLASSNAME _T("UFX-Knob") // Window class name

// uAKnob Class
uAKnob::uAKnob()
{
	kxuRegisterWindowClass(KXU_AKNOB_CLASSNAME);

	//local
	UserMovesRuler = 0;
	MouseDownY = 0;
	MouseDownX = 0;
	MouseDownPos = 0;

	bmpDisabled = NULL;
}

uAKnob::~uAKnob()
{
	if (bmpDisabled) delete bmpDisabled;
}

BEGIN_MESSAGE_MAP(uAKnob, uControl)
	//{{AFX_MSG_MAP(uAKnob)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_GETDLGCODE()
	ON_WM_MOUSEWHEEL()
	ON_WM_ENABLE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

#define SB_TEXTFORMAT	(DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX)

void uAKnob::OnPaint() 
{
	CDC MemDC;
	RECT rect;
	CBitmap *pMemOld;

	CPaintDC dc(this); // device context for painting

	//if (bmpKnob.GetSafeHandle() == NULL) PreDrawKnob(&dc);

	// Get Size of Display area
    GetClientRect(&rect);

    // Create memory DCs
	MemDC.CreateCompatibleDC(&dc);

	// Draw

	if (State & UC_STATE_DISABLED)
	{
		pMemOld = (CBitmap*) MemDC.SelectObject(bmpDisabled);
		dc.BitBlt(0, 0, rect.right, rect.bottom, &MemDC, 0, 0, SRCCOPY);
	}
	else
	{
		pMemOld = (CBitmap*) MemDC.SelectObject(&bmpKnob);

		// calc frame
		int f = RangeMax - RangeMin;
		if (f) f = ((Pos - RangeMin) * ((rframes << 1) + 1)) / (f << 1);
		f += sframe; f %= nFrames;
		f *= (int) Size.right;
		
		dc.BitBlt(0, 0, rect.right, rect.bottom, &MemDC, f, 0, SRCCOPY);
	}

	MemDC.SelectObject(pMemOld);

	// Do not call CWnd::OnPaint() for painting messages
}

const int size_to_el[][2] = {{UCB_KNOB_MASK, UCB_KNOB}, {UCB_KNOB_SMALL_MASK, UCB_KNOB_SMALL}};

BOOL uAKnob::PreDrawKnob()
{
	CBitmap bmpMask;
	CBitmap bmpTemp;
	POINT  pt = {0, 0};

	CWindowDC ScreenDC(NULL); // screen
	CDC SrcDC, DstDC;
	CBitmap *pSrcDCOldBitmap, *pDstDCOldBitmap;

	SrcDC.CreateCompatibleDC(NULL);
	DstDC.CreateCompatibleDC(NULL);

	int Element = size_to_el[Style & UKS_SIZE_MASK][0];
	RECT* pRenderData = &Size;
	uCore->GetControlBitmap(&bmpMask, 0, 1, &Element, &pRenderData);

	pRenderData = &FilmSize;
	Element = size_to_el[Style & UKS_SIZE_MASK][1];
	uCore->GetControlBitmap(&bmpKnob, 0, 1, &Element, &pRenderData);

	bmpTemp.CreateCompatibleBitmap(&ScreenDC, FilmSize.right, FilmSize.bottom);
	pDstDCOldBitmap = (CBitmap*) DstDC.SelectObject(&bmpTemp);

	// Transparency
	MapWindowPoints(pParentWindow, &pt, 1);
	OffsetWindowOrgEx(DstDC.m_hDC, pt.x, pt.y, &pt);
	::SendMessage(pParentWindow->m_hWnd, WM_ERASEBKGND, (WPARAM) DstDC.m_hDC, 0);
	SetWindowOrgEx(DstDC.m_hDC, pt.x, pt.y, NULL);

	pSrcDCOldBitmap = (CBitmap*) SrcDC.SelectObject(&bmpMask);
	DstDC.BitBlt(EXPANDRECTWH(Size), &SrcDC, 0, 0, SRCPAINT);

	for (int i = Size.right; i < FilmSize.right; i += Size.right)
	{DstDC.BitBlt(i, Size.top, RECTW(Size), RECTH(Size), &DstDC, 0, 0, SRCCOPY);}

	DstDC.SelectObject(pDstDCOldBitmap);
	SrcDC.SelectObject(pSrcDCOldBitmap);

	kxuBlend((HBITMAP) bmpKnob, &FilmSize, (HBITMAP) bmpTemp, 0, 0, BLEND_MULTIPLY);

	return TRUE;
}

int uAKnob::RenderDisabled()
{
	POINT  pt = {0, 0};

	CWindowDC ScreenDC(NULL); // screen
	CDC DstDC;
	CBitmap *pDstDCOldBitmap;

	DstDC.CreateCompatibleDC(NULL);

	bmpDisabled->CreateCompatibleBitmap(&ScreenDC, Size.right, Size.bottom);

	pDstDCOldBitmap = (CBitmap*) DstDC.SelectObject(bmpDisabled);

	// Transparency
	MapWindowPoints(pParentWindow, &pt, 1);
	OffsetWindowOrgEx(DstDC.m_hDC, pt.x, pt.y, &pt);
	::SendMessage(pParentWindow->m_hWnd, WM_ERASEBKGND, (WPARAM) DstDC.m_hDC, 0);
	SetWindowOrgEx(DstDC.m_hDC, pt.x, pt.y, NULL);

	DstDC.SelectObject(pDstDCOldBitmap);

	// calc frame
	int f = RangeMax - RangeMin;
	if (f) f = ((Pos - RangeMin) * ((rframes << 1) + 1)) / (f << 1);
	f += sframe; f %= nFrames;
	f *= (int) Size.right;

	kxuBlend((HBITMAP) *bmpDisabled, &Size, (HBITMAP) bmpKnob, f, 0, BLEND_COPY, 128);

	return TRUE;
}

BOOL uAKnob::Create(DWORD dwStyle, int x, int y, CWnd* pParentWnd, UINT nID, int nMin, int nMax, int StartAngle, int RotationAngle)
{
	ASSERT(pParentWnd);
	pParentWindow = pParentWnd;
	mID = nID;
	Style = dwStyle & UCS_CONTROL_MASK;

	Pos = (nMax + nMin) / 2;
	LimitMax = nMax;
	LimitMin = nMin;
	SetRange(nMin, nMax);
	
	// Calc knob size from skin data
	uCore->GetElementData(size_to_el[Style & UKS_SIZE_MASK][0], UED_SIZERECT, sizeof RECT, &Size);
	uCore->GetElementData(size_to_el[Style & UKS_SIZE_MASK][1], UED_SIZERECT, sizeof RECT, &FilmSize);
	nFrames = FilmSize.right / Size.right;
	MouseMoveSence = (Size.right << 1) + 1;

	// calc "degrees"
	int degperframe = 360 / nFrames;
	sframe = ((StartAngle + 180) / degperframe);
	rframes = (RotationAngle / degperframe);
	
	BOOL ret = CWnd::Create(KXU_AKNOB_CLASSNAME, "", dwStyle & UCS_WINDOW_MASK,
							CRect(x, y, Size.right + x, Size.bottom + y),
							pParentWnd, nID);
	PreDrawKnob();
	if (State & UC_STATE_DISABLED) RenderDisabled();

	return ret;
}

// User Input

void uAKnob::OnEnable(BOOL bEnable)
{
	if (bEnable) 
	{
		State &= ~UC_STATE_DISABLED;
		if (bmpDisabled) delete bmpDisabled;
		bmpDisabled = NULL;
	}
	else
	{
		State |= UC_STATE_DISABLED;
		bmpDisabled = (CBitmap*) new CBitmap;
		RenderDisabled();
	}
	REDRAW;
}

UINT uAKnob::OnGetDlgCode() 
{
	return DLGC_WANTARROWS | CWnd::OnGetDlgCode();
}

void uAKnob::OnKeyDown(UINT nChar, UINT /* nRepCnt */, UINT /* nFlags */) 
{
	switch (nChar)
	{
	case VK_DOWN:
	case VK_LEFT:
		SetPos(Pos - ArrowStepSize); 
		break;
	case VK_UP:
	case VK_RIGHT:
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

	NotifyParent(0, Pos);
	return;
}

void uAKnob::OnMouseMove(UINT /* nFlags */, CPoint point) 
{
	int delta;
	int p;

	// CWnd::OnMouseMove(nFlags, point); // drop through to default handler

	if (UserMovesRuler == TRUE) 
	{
		delta = (MouseDownY - point.y) + ((point.x - MouseDownX) >> 1);
		p = MouseDownPos + ((delta * (RangeMax - RangeMin)) / MouseMoveSence);
		//trace("delta %i, pos %i, new %i\n", delta, Pos, p);
		if (Pos == p) return;
		SetPos(p);
		if (Style & UKS_WRAP) MouseDownPos += Pos - p;
		NotifyParent(0, Pos);
	}
	return;
}

void uAKnob::OnLButtonDown(UINT /* nFlags */, CPoint point) 
{
	UserMovesRuler = TRUE;
	MouseDownY = point.y;
	MouseDownX = point.x;
	MouseDownPos = Pos;
	SetFocus();
	SetCapture();

	//CWnd::OnLButtonDown(nFlags, point); // drop through to default handler
}

BOOL uAKnob::OnMouseWheel(UINT /* nFlags */, short zDelta, CPoint /*pt*/) 
{
	SetPos(Pos + ((zDelta / WHEEL_DELTA) * PageStepSize));
	NotifyParent(0, Pos);
	return TRUE; // CWnd::OnMouseWheel(nFlags, zDelta, pt); // drop through to default handler
}

void uAKnob::OnLButtonUp(UINT /* nFlags */, CPoint /* point */) 
{
	UserMovesRuler = FALSE;
	ReleaseCapture();
	// CWnd::OnLButtonUp(nFlags, point); // drop through to default handler
}

// Public Functions

void uAKnob::SetPos(int nPos)
{
	if (nPos == Pos) return;
	Pos = nPos;
	if (Style & UKS_WRAP) 
	{
		WRAPBOUNDS(Pos, LimitMin, LimitMax)
	}
	else 
	{
		SETBOUNDS(Pos, LimitMin, LimitMax)
	}
	if (GetSafeHwnd() != NULL) REDRAW;
}

int uAKnob::GetPos() const
{
	return Pos;
}

void uAKnob::SetRange(int nMin, int nMax)
{
	RangeMin = nMin;
	RangeMax = nMax;
	SETBOUNDS(LimitMin, RangeMin, RangeMax);
	SETBOUNDS(LimitMax, RangeMin, RangeMax);
	SETBOUNDS(Pos, LimitMin, LimitMax);
	ArrowStepSize = ((nMax - nMin) < 0) ? -1 : 1;
	PageStepSize = (nMax - nMin) / 10; if (PageStepSize == 0) PageStepSize = ArrowStepSize;
	if (GetSafeHwnd() != NULL) REDRAW;
}

void uAKnob::SetLimit(int nMin, int nMax)
{
	LimitMin = nMin;
	LimitMax = nMax;
	SETBOUNDS(LimitMin, RangeMin, RangeMax);
	SETBOUNDS(LimitMax, RangeMin, RangeMax);
	SETBOUNDS(Pos, LimitMin, LimitMax);
	if (GetSafeHwnd() != NULL) REDRAW;
}

void uAKnob::GetRange(int &nMin, int &nMax) const
{
	nMin = RangeMin;
	nMax = RangeMax;
}

void uAKnob::SetRangeMax(int nMax)
{
	SetRange(RangeMin, nMax);
}

int uAKnob::GetRangeMax() const
{
	return RangeMax;
}

void uAKnob::SetRangeMin(int nMin)
{
	SetRange(nMin, RangeMax);
}

void uAKnob::SetLimitMin(int nMin)
{
	SetLimit(nMin, LimitMax);
}

int uAKnob::GetRangeMin() const
{
	return RangeMin;
}

void uAKnob::SetArrowStepSize(int StepSize)
{
	ArrowStepSize = StepSize;
}

int uAKnob::GetArrowStepSize()
{
	return ArrowStepSize;
}

void uAKnob::SetPageStepSize(int StepSize)
{
	PageStepSize = StepSize;
}

int uAKnob::GetPageStepSize()
{
	return PageStepSize;
}
