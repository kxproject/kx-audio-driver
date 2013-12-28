// kX GUI
// Copyright (c) Eugene Gavrilov, Max Mikhailov 2002-2005.
// All rights reserved

/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

#include "stdafx.h"

#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//.............................................................................

#define UCS_WINDOW_MASK		0xffff0000
#define UCS_CONTROL_MASK	0x0000ffff

//.............................................................................
// notify codes
/*
#define UC_NOTIFY_CODE_MASK	0xffff0000
#define UC_NOTIFY_ID_MASK	0x0000ffff

#define UCN_DEFAULT		0x00000000
#define UCN_SETFOCUS	0x00030000

//.............................................................................

#define UC_STATE_DEFAULT	0
#define UC_STATE_CHECKED	1
#define UC_CHECK_MASK		(UC_STATE_DEFAULT | UC_STATE_CHECKED)
#define UC_STATE_PUSHED		4
#define UC_STATE_FOCUS		8
#define UC_STATE_HOVER		UC_STATE_FOCUS

//.............................................................................

#define KXU_CONTROL_CLASSNAME _T("kXUniformCtrl")

#define WM_UCONTROL_MESSAGE	(WM_USER + 33)
 */

#define REDRAW RedrawWindow(NULL, NULL, RDW_UPDATENOW|RDW_INVALIDATE|RDW_NOERASE);


//.............................................................................
// utility

#define SETRECT(rect, l, t, r, b) (rect).left = (l); (rect).top = (t); (rect).right = (r); (rect).bottom = (b);

#define EXPANDRECT(r) (r).left, (r).top, (r).right , (r).bottom
#define EXPANDRECTWH(r) (r).left, (r).top, (r).right - (r).left, (r).bottom - (r).top

#define SETBOUNDS(v, nmin, nmax)	\
	if ((nmax - nmin) < 0) {	\
		if (v < nmax) {				\
			v = nmax;				\
		}							\
		else if (v > nmin){			\
			v = nmin;				\
		}							\
	}								\
	else {							\
		if (v > nmax) {				\
			v = nmax;				\
		}							\
		else if (v < nmin){			\
			v = nmin;				\
		}							\
	}
// end of SETBOUNDS



//.............................................................................


//.............................................................................
// uAKnob

#define KXU_AKNOB_CLASSNAME _T("kXUCtrlAKnob") // в принципе отдельный класс совсем не нужно регистрировать

/*
LRESULT CKXControl::NotifyParent(WPARAM NotifyCode, LPARAM NotifyData)
{
	return pParentWindow->SendMessage(WM_CKXControl_MESSAGE, mID | NotifyCode, NotifyData);
}

//.............................................................................
// CKXControl message handlers

void CKXControl::OnSetFocus(CWnd* pOldWnd) 
{
	CWnd::OnSetFocus(pOldWnd);
	NotifyParent(UCN_SETFOCUS);
}
*/

// uAKnob Class
kKnob::kKnob()
{
	Style = 0;

	gui_register_window_class(KXU_AKNOB_CLASSNAME);

	//local
	UserMovesRuler = 0;
	MouseDownY = 0;
	MouseDownX = 0;
	MouseDownPos = 0;
}

kKnob::~kKnob()
{
}

//.............................................................................

int kKnob::draw(kDraw &d)
{
	CDC MemDC, TmpDC;
	RECT rect;
	CBitmap *pMemOld, *pTmpOld, bm;
	POINT pt = {0, 0};

	CDC dc;
	dc.Attach(d.get_dc());

	// Get Size of Display area
    GetClientRect(&rect);

    // Create memory DCs
    if (!TmpDC.CreateCompatibleDC(&dc)) 
     return 0;

	MemDC.CreateCompatibleDC(&dc);
	bm.CreateCompatibleBitmap(&dc, rect.right, rect.bottom);
	pMemOld = (CBitmap*) MemDC.SelectObject(&bm);

	// Select SrcBitmap
	pTmpOld = (CBitmap*) TmpDC.SelectObject(&bmpKnob);

	// draw parent
	::MapWindowPoints(m_hWnd,get_parent()->get_wnd(), &pt, 1);
	OffsetWindowOrgEx(MemDC.m_hDC, pt.x, pt.y, &pt);
	::SendMessage(get_parent()->get_wnd(), WM_ERASEBKGND, (WPARAM) MemDC.m_hDC, 0);
	SetWindowOrgEx(MemDC.m_hDC, pt.x, pt.y, NULL);

	// draw mask
	MemDC.BitBlt(0, 0, rect.right, rect.bottom, &TmpDC, FilmSize.right, 0, SRCPAINT);

	// draw knob
	
	// calc frame
	int f = RangeMax - RangeMin;
	if (f) f = (((Pos - RangeMin) * (rframes)) / f);
	f += sframe; f %= nFrames;
	f *= (int) Size.cx;

	MemDC.BitBlt(0, 0, rect.right, rect.bottom, &TmpDC, f, 0, SRCAND);
	
	// Screen Blt
	dc.BitBlt(0, 0, rect.right, rect.bottom, &MemDC, 0, 0, SRCCOPY);
	TmpDC.SelectObject(pTmpOld);
	MemDC.SelectObject(pMemOld);

	// Do not call CWnd::OnPaint() for painting messages
	return 0;
}

/*
BOOL kKnob::PreDrawKnob()
{
	CBitmap bmpMask;
	CBitmap bmpTemp;
	POINT pt = {0, 0};

	CWindowDC ScreenDC(NULL); // screen
	CDC SrcDC, DstDC;
	CBitmap *pSrcDCOldBitmap, *pDstDCOldBitmap;

	SrcDC.CreateCompatibleDC(NULL);
	DstDC.CreateCompatibleDC(NULL);

	int Element = UCB_KNOB_MASK;
	RECT* pRenderData = &Size;
	uCore->GetControlBitmap(&bmpMask, 0, 1, &Element, &pRenderData);

	pRenderData = &FilmSize;
	Element = UCB_KNOB;
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

	DstDC.SelectObject(pSrcDCOldBitmap);
	SrcDC.SelectObject(pDstDCOldBitmap);

	kxuBlend((HBITMAP) bmpKnob, &FilmSize, (HBITMAP) bmpTemp, 0, 0, BLEND_MULTIPLY);

	return TRUE;
}
*/

BOOL kKnob::create(DWORD dwStyle, long x, long y, kDialog* pParentWnd, UINT nID, int nMin, int nMax, int StartAngle, int RotationAngle)
{
	ASSERT(pParentWnd);

	k_id=nID;
	k_parent=pParentWnd;

	Style = dwStyle & UCS_CONTROL_MASK;

	Pos = (nMax + nMin) / 2;
	set_range(nMin, nMax);

	// Calc knob size from skin data

	/*
	uCore->GetElementData(UCB_KNOB_MASK, UED_SIZERECT, sizeof RECT, &Size);
	uCore->GetElementData(UCB_KNOB, UED_SIZERECT, sizeof RECT, &FilmSize);
	*/

	#define KNOB_BMP_WIDTH	1152	// по-идее, кроме текущего среднего размера, еще будут маленький и большой
	#define KNOB_WIDTH	48			// (в смысле я отрендерю кнобы других размеров)
	#define KNOB_HEIGHT	48			// так что оставь лазейку

	Size.init(KNOB_WIDTH, KNOB_HEIGHT);
	FilmSize.init(0, 0, KNOB_BMP_WIDTH, KNOB_HEIGHT); // KNOB_BMP_WIDTH не включает маску, тебе удобней её в отдельный файл вынести

	nFrames = FilmSize.right / Size.cx;
	MouseMoveSence = (Size.cx << 1) + 1;

	// load bitmap, у меня оно сразу отрисовывается из маски, кноба и фона (и в Paint один единственный Blt без всяких там transparency и т.д.)
	#define IDB_KNOB 129
	bmpKnob.LoadBitmap(IDB_KNOB);

	// calc "degrees"
	int degperframe = 360 / nFrames;
	sframe = ((StartAngle + 180) / degperframe);
	rframes = (RotationAngle / degperframe);

	kRect rr(x, y, Size.cx + x, Size.cy + y);
	
	BOOL ret = kWindow::create(_T(""), KXU_AKNOB_CLASSNAME, 
	         rr,
	         dwStyle & UCS_WINDOW_MASK,
	         nID, pParentWnd);
	return ret;
}


//.............................................................................
// User Input

void kKnob::on_char(int nChar, int /* nRepCnt */, int /* nFlags */) 
{
	switch (nChar)
	{
	case VK_DOWN:
		set_pos(Pos - ArrowStepSize); 
		break;
	case VK_UP:
		set_pos(Pos + ArrowStepSize); 
		break;
	case VK_NEXT:
		set_pos(Pos - PageStepSize); 
		break;
	case VK_PRIOR:
		set_pos(Pos + PageStepSize); 
		break;
	default:
		// CWnd::OnKeyDown(nChar, nRepCnt, nFlags); // drop through to default handler
		return;
	}

	NotifyParent();
	return;
}

void kKnob::on_mouse_move(kPoint point,int )
{
	int delta;
	int p;

	// CWnd::OnMouseMove(nFlags, point); // drop through to default handler

	if (UserMovesRuler == TRUE) 
	{
		delta = (MouseDownY - point.y) + ((point.x - MouseDownX) >> 1);
		p = MouseDownPos + ((delta * (RangeMax - RangeMin)) / MouseMoveSence);
		NotifyParent(); 
		set_pos(p);
	}
	return;
}

void kKnob::on_mouse_l_down(kPoint pt,int )
{
	UserMovesRuler = TRUE;
	MouseDownY = pt.y;
	MouseDownX = pt.x;
	MouseDownPos = Pos;
	SetFocus();
	SetCapture();

	//CWnd::OnLButtonDown(nFlags, point); // drop through to default handler
}

void kKnob::on_mouse_wheel(kPoint , int , int delta)
{
	set_pos(Pos + ((delta / WHEEL_DELTA) * PageStepSize));
	NotifyParent();
}

void kKnob::on_mouse_l_up(kPoint ,int )
{
	UserMovesRuler = FALSE;
	ReleaseCapture();
	// CWnd::OnLButtonUp(nFlags, point); // drop through to default handler
}

// Public Functions

void kKnob::set_pos(int nPos)
{
	if (nPos == Pos) return;
	Pos = nPos;
	// важное отличие от MFC, здесь RangeMax может спокойно быть меньше чем RangeMin, см. диалог
	SETBOUNDS(Pos, RangeMin, RangeMax);
	if (GetSafeHwnd() != NULL) REDRAW;
}

int kKnob::get_pos() const
{
	return Pos;
}

void kKnob::set_range(int nMin, int nMax)
{
	RangeMin = nMin;
	RangeMax = nMax;
	SETBOUNDS(Pos, RangeMin, RangeMax);
	ArrowStepSize = ((nMax - nMin) < 0) ? -1 : 1;
	PageStepSize = (nMax - nMin) / 10; if (PageStepSize == 0) PageStepSize = ArrowStepSize;
	if (GetSafeHwnd() != NULL) REDRAW;
}

void kKnob::get_range(int &nMin, int &nMax) const
{
	nMin = RangeMin;
	nMax = RangeMax;
}

void kKnob::set_range_max(int nMax)
{
	set_range(RangeMin, nMax);
}

int kKnob::get_range_max() const
{
	return RangeMax;
}

void kKnob::set_range_min(int nMin)
{
	set_range(nMin, RangeMax);
}

int kKnob::get_range_min() const
{
	return RangeMin;
}

void kKnob::SetArrowStepSize(int StepSize)
{
	ArrowStepSize = StepSize;
}

int kKnob::GetArrowStepSize()
{
	return ArrowStepSize;
}

void kKnob::SetPageStepSize(int StepSize)
{
	PageStepSize = StepSize;
}

int kKnob::GetPageStepSize()
{
	return PageStepSize;
}

//.............................................................................
// у меня все контролы шлют один универсальный WM (uControlChanged)
// при любом изменении (SCROLL, FOCUS etc. etc.)
// здесь я тебе как в MFC cделал, но только один единственный упрощенный ON_SCROLL
LRESULT kKnob::NotifyParent(WPARAM /* NotifyCode */, LPARAM /* NotifyData*/)
{
	return get_parent()->send_message(WM_VSCROLL, (UINT)MAKEWPARAM(0, Pos), (UINT)(LPARAM) m_hWnd);
}
