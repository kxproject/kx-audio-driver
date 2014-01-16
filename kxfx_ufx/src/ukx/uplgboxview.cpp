
#include "ukx.h"
// #include "uplgboxview.h"

/////////////////////////////////////////////////////////////////////////////
// uPluginBoxView

uPluginBoxView::uPluginBoxView()
{
	SetRect(&size, 0, 0, 32, 32);
	state = 0;
}

uPluginBoxView::~uPluginBoxView()
{

}

BEGIN_MESSAGE_MAP(uPluginBoxView, CWnd)
	//{{AFX_MSG_MAP(uPluginBoxView)
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_UCONTROL_MESSAGE, ucontrol_changed)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////

int uPluginBoxView::open()
{
	return true;
}

void uPluginBoxView::close()
{

}

int uPluginBoxView::redraw()
{
    return 0;
}

BOOL uPluginBoxView::OnEraseBkgnd(CDC* pDC) 
{
	RECT rect; pDC->GetClipBox(&rect);
	CDC MemDC;
	MemDC.CreateCompatibleDC(pDC);
	MemDC.SelectObject(&container->bitmap);
    POINT p = {0, 0};
    ::MapWindowPoints(m_hWnd, container->that->get_wnd(), &p, 1);
	pDC->BitBlt(EXPANDRECTWH(rect), &MemDC, rect.left + p.x, rect.top + p.y, SRCCOPY);
    // trace("%i, %i, %i, %i\n", rect.left, rect.top, rect.right, rect.bottom, p.x, p.y);
	return TRUE;
}

void uPluginBoxView::PostNcDestroy() 
{
	plugin->boxview = NULL;
	plugin = NULL;
	delete this;
	CWnd::PostNcDestroy();
}
