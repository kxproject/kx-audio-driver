
#include "ufw.h"
#include "umeter.h"

/////////////////////////////////////////////////////////////////////////////
// uMeter

#define KXU_METER_CLASSNAME _T("UFX-Meter") // Window class name

// uMeter Class
uMeter::uMeter()
{
	kxuRegisterWindowClass(KXU_METER_CLASSNAME);
	SETRECT(meterrect[0], 0, 0, 0, 0);
	SETRECT(meterrect[1], 0, 0, 0, 0);
	bkgbitmap = NULL;
	bmp_meter = NULL;
	value[0] = 0; value[1] = 0;
	tail[0] = 0; tail[1] = 0;
	clip[0] = 0; clip[1] = 0;
	strcpy(label[0], "-Inf. "); strcpy(label[1], "-Inf. ");
	htimer = 0;
}

uMeter::~uMeter()
{
}

BEGIN_MESSAGE_MAP(uMeter, uControl)
	//{{AFX_MSG_MAP(uMeter)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

#define UL_TEXTFORMAT	(DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX)

void uMeter::OnPaint() 
{
	CDC MemDC, TmpDC;
	CRect rect;
	CBitmap *pMemOldBitmap, *pTmpOld, bm;

	CPaintDC dc(this); // device context for painting

	// Draw

	// Get Size of Display area
    GetClientRect(rect);

    // Create memory DCs
    if (!TmpDC.CreateCompatibleDC(&dc)) return;
	MemDC.CreateCompatibleDC(&dc);
	bm.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
	pMemOldBitmap = (CBitmap*) MemDC.SelectObject(&bm);

	// Draw Back
	pTmpOld = (CBitmap*) TmpDC.SelectObject(bkgbitmap);
	MemDC.BitBlt(0, 0, rect.Width(), rect.Height(), &TmpDC, 0, 0, SRCCOPY);

	// Labels
	RECT xrect;
	MemDC.SetBkMode(TRANSPARENT);
	SETRECT(xrect, meterrect[0].left - 3, meterrect[0].top - 11, meterrect[0].right, meterrect[0].top - 2);
	MemDC.SelectObject(uCore->GetFont(UF_SMALLLABEL));
	if (clip[0] == 0) { MemDC.SetTextColor(0);}
	else {MemDC.SetTextColor(0xE0E0E0); MemDC.FillSolidRect(&xrect, 0x0000ff);}
	MemDC.DrawText(label[0], -1, &xrect, UL_TEXTFORMAT | ULS_RIGHT);
	xrect.left = meterrect[1].left - 3; xrect.right = meterrect[1].right;
	if (clip[1] == 0) {MemDC.SetTextColor(0);}
	else {MemDC.SetTextColor(0xE0E0E0); MemDC.FillSolidRect(&xrect, 0x0000ff);}
	MemDC.DrawText(label[1], -1, &xrect, UL_TEXTFORMAT | ULS_RIGHT);

	// Meters
	if (bmp_meter)
	{
		TmpDC.SelectObject(bmp_meter);
		MemDC.BitBlt(meterrect[0].left, meterrect[0].bottom - value[0], meterrect[0].right, 
			meterrect[0].bottom, &TmpDC, 0, RECTH(meterrect[0]) - value[0], SRCCOPY);
		MemDC.BitBlt(meterrect[1].left, meterrect[1].bottom - value[1], meterrect[1].right, 
			meterrect[1].bottom, &TmpDC, 0, RECTH(meterrect[1]) - value[1], SRCCOPY);
		if (tail[0] > 1)
		{
			MemDC.BitBlt(meterrect[0].left, meterrect[0].bottom - tail[0], meterrect[0].right, 
			4, &TmpDC, 0, RECTH(meterrect[0]) - tail[0], SRCCOPY);
		}
		if (tail[1] > 1)
		{
			MemDC.BitBlt(meterrect[1].left, meterrect[1].bottom - tail[1], meterrect[1].right, 
			4, &TmpDC, 0, RECTH(meterrect[1]) - tail[1], SRCCOPY);
		}
	}

	TmpDC.SelectObject(pTmpOld);

	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &MemDC, 0, 0, SRCCOPY);
	
	MemDC.SelectObject(pMemOldBitmap);

	// trace("%i, %i\n", rect.Width(), rect.Height());

	// Do not call CWnd::OnPaint() for painting messages
}

BOOL uMeter::Create(DWORD dwStyle, const CBitmap* bkgbitmap, int x, int y, CWnd* pParentWnd, UINT nID)
{
	ASSERT(pParentWnd);
	pParentWindow = pParentWnd;
	mID = nID;

	Style = (dwStyle & UCS_CONTROL_MASK);

	uMeter::bkgbitmap = (CBitmap*) bkgbitmap;
	kxuGetBitmapSize((HBITMAP) *bkgbitmap, &Size);
	
	BOOL ret = CWnd::Create(KXU_METER_CLASSNAME, NULL, dwStyle & UCS_WINDOW_MASK,
							CRect(x, y, x + Size.right, y + Size.bottom),
							pParentWnd, nID);
	return ret;
}

int uMeter::SetMeterBitmap(const CBitmap* bitmap, int x, int y, int offset)
{
	bmp_meter = (CBitmap*) bitmap;
	kxuGetBitmapSize((HBITMAP) *bitmap, &meterrect[0]);
	MOVERECT(meterrect[0], x, y);
	CopyRect(&meterrect[1], &meterrect[0]); 
	MOVERECT(meterrect[1], offset + RECTW(meterrect[0]), 0);
	return 0;
}

void uMeter::SetLabel(unsigned int id, const char* l, int c)
{
	clip[id] = c;
	strcpy(label[id], l);
}

void uMeter::SetValue(int v1, int v2, int t1, int t2)
{
	value[0] = v1 + 1;
	value[1] = v2 + 1;
	tail[0] = t1 + 1;
	tail[1] = t2 + 1;
	// RECT rect; GetUpdateRect(&rect);
	RedrawWindow(NULL, NULL, RDW_UPDATENOW|RDW_NOERASE|RDW_INVALIDATE);
}

void uMeter::OnTimer(UINT /* nIDEvent */)
{
	// trace(".\n");
	NotifyParent();
}

void uMeter::OnLButtonDown(UINT nFlags, CPoint point) 
{
	RECT xrect;
	SETRECT(xrect, meterrect[0].left - 3, meterrect[0].top - 11, meterrect[0].right, meterrect[0].top - 2);
	if (PtInRect(&xrect, point)) NotifyParent(UCN_SETRESET, 0);
	else
	{
		xrect.left = meterrect[1].left - 3; xrect.right = meterrect[1].right;
		if (PtInRect(&xrect, point)) NotifyParent(UCN_SETRESET, 1);
	}
	CWnd::OnLButtonDown(nFlags, point);
	return;	
}

void  uMeter::start()
{
	htimer = SetTimer(1, 50, NULL);
}

void  uMeter::stop()
{
	if (htimer) ::KillTimer(m_hWnd, htimer);
}

