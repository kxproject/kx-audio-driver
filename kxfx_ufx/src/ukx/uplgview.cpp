
#include "ukx.h"
#include "uplgview.h"

/////////////////////////////////////////////////////////////////////////////
// uPluginView

uPluginView::uPluginView()
{
	SetRect(&size, 0, 0, 82, 82);
}

uPluginView::~uPluginView()
{
}

BEGIN_MESSAGE_MAP(uPluginView, CWnd)
	//{{AFX_MSG_MAP(uPluginView)
	ON_WM_ERASEBKGND()
	// ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTip)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_MESSAGE(WM_UCONTROL_MESSAGE, ucontrol_changed)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// uPluginView message handlers

int uPluginView::open()
{
	return true;
}

void uPluginView::close()
{
}

BOOL uPluginView::OnEraseBkgnd(CDC* pDC) 
{
	/* since this function can be requested by one of the
	   child controls we must be sure that background
	   is already present */
	if (pParentContainer->bkgbitmap.GetSafeHandle() == NULL) pParentContainer->BigInit();

	RECT rect; pDC->GetClipBox(&rect);
	CDC MemDC;
	MemDC.CreateCompatibleDC(pDC);
	MemDC.SelectObject(&bkgbitmap);
	pDC->BitBlt(EXPANDRECTWH(rect), &MemDC, EXPANDRECTLT(rect), SRCCOPY);
	return TRUE;
}

//.............................................................................

int uPluginView::set_title(const char* title)
{
	pParentContainer->title = title;
	return 0;
}

int uPluginView::set_property(int id, int adata, int bdata)
{
	switch(id)
	{
	case UPVP_STYLE:
		pParentContainer->flags |= adata;
		break;

	case UPVP_TEXTURE:
		pParentContainer->texture_id = adata;
		if (adata == NULL) bdata = 0;
		pParentContainer->texture_depth = bdata;
		pParentContainer->flags |= UPC_FLAGS_CUSTOMTEXTURE;
		break;
	
	case UPVP_ICON:
		pParentContainer->icon_id = bdata;
		pParentContainer->flags |= UPC_FLAGS_CUSTOMICON;
		break;

	default:
		return -1;

	}
	return 0;
}

int uPluginView::set_bypass(int id)
{
	pParentContainer->bypass_id = id;
	pParentContainer->flags |= UPC_FLAGS_BYPASS;
	return 0;
}

int uPluginView::set_mute(int id)
{
	pParentContainer->mute_id = id;
	pParentContainer->flags |= UPC_FLAGS_MUTE;
	return 0;
}

//.............................................................................
