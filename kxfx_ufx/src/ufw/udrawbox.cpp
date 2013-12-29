
#include "ufw.h"
#include "udrawbox.h"

/////////////////////////////////////////////////////////////////////////////
// uDrawBox

#define KXU_DRAWBOX_CLASSNAME _T("UFX-Drawbox") // Window class name

// uDrawBox Class
uDrawBox::uDrawBox()
{
	kxuRegisterWindowClass(KXU_DRAWBOX_CLASSNAME);
	rgn = 0;
	npoints = 0;
	pts_shift = 0;
}

uDrawBox::~uDrawBox()
{
	if (rgn) DeleteObject(rgn);
}

BEGIN_MESSAGE_MAP(uDrawBox, uControl)
	//{{AFX_MSG_MAP(uDrawBox)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_NCHITTEST()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

#define UL_TEXTFORMAT	(DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX)

void uDrawBox::OnPaint() 
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

	// Draw Box
	pTmpOld = (CBitmap*) TmpDC.SelectObject(&bmp_predraw);
	MemDC.BitBlt(0, 0, rect.Width(), rect.Height(), &TmpDC, 0, 0, SRCCOPY);

	TmpDC.SelectObject(pTmpOld);

	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &MemDC, 0, 0, SRCCOPY);
	
	MemDC.SelectObject(pMemOldBitmap);

	// Do not call CWnd::OnPaint() for painting messages
}

BOOL uDrawBox::Create(DWORD dwStyle, const CBitmap* bkgbitmap, int x, int y, int w, int h, CWnd* pParentWnd, UINT nID)
{
	ASSERT(pParentWnd);
	pParentWindow = pParentWnd;
	mID = nID;

	Style = (dwStyle & UCS_CONTROL_MASK);

	// Render Box
	BoxBorder = 0;
	
	// RECT xrect;
	SETRECT(Size, 0, 0, w, h);
	CopyRect(&shaperect, &Size);

	// CopyRect(&drawrect, &Size);
	uDrawBox::bkgbitmap = (CBitmap*) bkgbitmap;
	
	BOOL ret = CWnd::Create(KXU_DRAWBOX_CLASSNAME, NULL, dwStyle & UCS_WINDOW_MASK,
							CRect(x, y, x + w, y + h),
							pParentWnd, nID);
	return ret;
}

int uDrawBox::SetShapeFore(const CBitmap* bitmap, int x, int y, int w, int h)
{
	bmp_shapefore = (CBitmap*) bitmap;
	SETRECT(shaperect, x, y, x + w, y + h);
	return 0;
}

int uDrawBox::PrepareMask()
{
	CDC SrcDC;
	CDC SrcDC2;
	CDC DstDC;
	DstDC.CreateCompatibleDC(NULL);
	SrcDC.CreateCompatibleDC(NULL);
	SrcDC2.CreateCompatibleDC(NULL);
	CBitmap* pSrc2OldBitmap = (CBitmap*) SrcDC2.SelectObject(bmp_shapefore);

	CBitmap mask;

	if (bmp_predraw.m_hObject == NULL)
	{
		bmp_predraw.CreateCompatibleBitmap(&SrcDC2, RECTW(Size), RECTH(Size));
	}

	CBitmap* pDstOldBitmap = (CBitmap*) DstDC.SelectObject(&bmp_predraw);

	CBitmap* pSrcOldBitmap = (CBitmap*) SrcDC.SelectObject(bkgbitmap);
	RECT bkgbmpsize; kxuGetBitmapSize((HBITMAP) *bkgbitmap, &bkgbmpsize);
	int r = RECTW(bkgbmpsize) - (Size.right - shaperect.right);
	DstDC.FillSolidRect(0, 0, RECTW(Size), RECTH(Size), 0x0000ff);
	DstDC.BitBlt(0, 0, shaperect.left, RECTH(Size), &SrcDC, 0, 0, SRCCOPY);
	DstDC.BitBlt(shaperect.left, 0, RECTW(Size) - shaperect.left, RECTH(Size), &SrcDC, shaperect.left + pts_shift, 0, SRCCOPY);
	DstDC.BitBlt(shaperect.right, 0, shaperect.right + r, RECTH(Size), &SrcDC, r, 0, SRCCOPY);

	mask.CreateCompatibleBitmap(&SrcDC, shaperect.right, shaperect.bottom);
	SrcDC.SelectObject(&mask);
	SrcDC.FillSolidRect(0, 0, RECTW(shaperect), RECTH(shaperect), 0xffffff);

	int y = RECTH(shaperect) >> 1;
	int x;

	for (int i = pts_shift; i < npoints; i++)
	{
		x = pts[i].x - pts_shift;
		MoveToEx((HDC) SrcDC, x, y, NULL);
		LineTo((HDC) SrcDC, x, pts[i].y + y);	
	}

	DstDC.BitBlt(EXPANDRECTWH(shaperect), &SrcDC, 0, 0, SRCAND);
	SrcDC.BitBlt(0, 0, RECTW(shaperect), RECTH(shaperect), &SrcDC2, pts_shift, 0, SRCERASE);
	DstDC.BitBlt(EXPANDRECTWH(shaperect), &SrcDC, 0, 0, SRCPAINT);

	BITMAP bi;
	bmp_predraw.GetBitmap(&bi);

	SrcDC2.SelectObject(pSrc2OldBitmap);
	SrcDC.SelectObject(pSrcOldBitmap);
	DstDC.SelectObject(pDstOldBitmap);

	return 0;
}

int uDrawBox::SetRegionPoints(LPPOINT pts, int npoints, int pts_shift)
{
	uDrawBox::pts = pts;
	uDrawBox::npoints = npoints;
	uDrawBox::pts_shift = pts_shift;
	/*if (rgn) DeleteObject(rgn);
	rgn = CreatePolygonRgn(pts, npoints, ALTERNATE);*/
	PrepareMask();
	REDRAW;
	return true;
}