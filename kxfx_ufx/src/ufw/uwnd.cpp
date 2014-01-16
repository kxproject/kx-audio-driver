
#include "ufw.h"
#include "uwnd.h"

/////////////////////////////////////////////////////////////////////////////
// uWnd

#define KXU_WND_CLASSNAME _T("UFX-Window") // Window class name

// uLabel Class
uWnd::uWnd()
{
	kxuRegisterWindowClass(KXU_WND_CLASSNAME);
	uCore = uGetCore();
	hParentWindow = 0;
}

uWnd::~uWnd()
{
}

BEGIN_MESSAGE_MAP(uWnd, CWnd)
	//{{AFX_MSG_MAP(uWnd)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL uWnd::CreateEx( DWORD dwExStyle, LPCTSTR lpszWindowName, DWORD dwStyle, 
					int x, int y, int nWidth, int nHeight, HWND hwndParent, HMENU nIDorHMenu, LPVOID lpParam)
{
	hParentWindow = hwndParent;
	
	BOOL r = CWnd::CreateEx(dwExStyle,
		KXU_WND_CLASSNAME, lpszWindowName,
		dwStyle,
		x, y, nWidth, nHeight,
		hwndParent, nIDorHMenu, lpParam);

	GetClientRect(&size);
	PrepareBckgnd();
	return r;
}

void uWnd::PrepareBckgnd()
{
	CBitmap x; 
	int Element = UCB_BACK;
	LPRECT RenderData = &size;
	uCore->GetControlBitmap(&bkgbitmap, 0, 1, &Element, &RenderData);
	RECT rect = {0, 0, 0, 0}; 
	Element = UCB_TEXTURE;
	RenderData = &rect;
	uCore->GetControlBitmap(&x, 0, 1, &Element, &RenderData);
	kxuApplyTexture((HBITMAP) bkgbitmap, &size, (HBITMAP) x, 0, 0, NULL, 128);
	Element = UCB_FRAMEBEVEL;
	RenderData = &size;
	uCore->GetControlBitmap(&x, 0, 1, &Element, &RenderData);
	kxuBlend((HBITMAP) bkgbitmap, &size, (HBITMAP) x, 0, 0, BLEND_OVERLAY, 480);
}

/////////////////////////////////////////////////////////////////////////////
// uWnd message handlers

void uWnd::OnSize(UINT nType, int cx, int cy) 
{
	if ((nType != SIZE_MINIMIZED) | (size.right != cx) | (size.right != cx))
	{
		SETRECT(size, 0, 0, cx, cy);
		PrepareBckgnd();
	}
	CWnd::OnSize(nType, cx, cy);
}

BOOL uWnd::OnEraseBkgnd(CDC* pDC) 
{
	RECT rect; pDC->GetClipBox(&rect);
	CDC MemDC;
	MemDC.CreateCompatibleDC(pDC);
	MemDC.SelectObject(&bkgbitmap);
	pDC->BitBlt(EXPANDRECTWH(rect), &MemDC, EXPANDRECTLT(rect), SRCCOPY);
	return TRUE;
}


//.............................................................................
