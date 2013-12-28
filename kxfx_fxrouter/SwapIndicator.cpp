// SwapIndicator.cpp : implementation file
//

#include "stdafx.h"
#include "SwapIndicator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSwapIndicator

IMPLEMENT_DYNAMIC(CSwapIndicator, CWnd)

BEGIN_MESSAGE_MAP(CSwapIndicator, CWnd)
	//{{AFX_MSG_MAP(CSwapIndicator)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEACTIVATE()
	ON_WM_MOUSEMOVE()	
	ON_WM_PAINT()	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CSwapIndicator::Create(const RECT &rect, CWnd *pParentWnd, UINT nID, UINT nContextID)
{
	m_pParentWnd = pParentWnd;
	m_nCtrlID = nID;
	m_nContextID = nContextID;		
	
	DWORD dwStyle = WS_CHILD | WS_VISIBLE;
	return CWnd::Create(SWAPINDICATOR_CLASSNAME, _T(""), dwStyle, rect, pParentWnd, nID);
}

BOOL CSwapIndicator::Create(CPoint topLeft, CWnd *pParentWnd, UINT nID, UINT nContextID)
{
	// size is determined from the bitmap, so bitmap must be valid to use this creation method
	ASSERT(m_bmpNormal != NULL);

	m_pParentWnd = pParentWnd;
	m_nCtrlID = nID;
	m_nContextID = nContextID;	
	BITMAP bm;	
	CBitmap bmp;
	bmp.Attach(m_bmpNormal);
	bmp.GetBitmap(&bm);	
	RECT rect;
	rect.left = topLeft.x;
	rect.top = topLeft.y;
	rect.right = rect.left + bm.bmWidth;
	rect.bottom = rect.top + bm.bmHeight;
	bmp.Detach();

	DWORD dwStyle = WS_CHILD | WS_VISIBLE;	
	int iResult = CWnd::Create(SWAPINDICATOR_CLASSNAME, _T(""), dwStyle, rect, pParentWnd, nID);
	SetBitmap(m_bmpNormal);
	return iResult;
}

CSwapIndicator::CSwapIndicator()
{
	
	m_bIsDragging	= FALSE;
	m_bmpCurrent	= NULL;
	m_bmpNormal		= NULL;
	m_bmpDrag		= NULL;
	m_bmpDragOver	= NULL;	
	m_nContextID	= NULL;
	m_nCtrlID		= NULL;
	m_Cursor		= NULL;
	m_pParentWnd	= NULL;
	m_pDropWnd		= NULL;

	BYTE ANDmaskCursor[] = 
	{
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF,	0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF,	0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF,	0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF,	0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF,	0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF,	0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF,	0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF,	0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF,	0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF,	0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF,	0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF,	0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF,	0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF,	0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF,	0xFF, 0xFF, 0xFF, 0xFF
	};

	BYTE XORmaskCursor[] = 
	{
		0x00, 0x00, 0x00, 0x00,	0x00, 0x00, 0x00, 0x00,
		0x00, 0x01, 0x00, 0x00,	0x00, 0x00, 0x00, 0x00,	
		0x00, 0x01, 0x00, 0x00,	0x00, 0x01, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,	0x00, 0x01, 0x00, 0x00,
		0x00, 0x01, 0x00, 0x00,	0x00, 0x00, 0x00, 0x00,
		0x00, 0x01, 0x00, 0x00,	0x00, 0x01, 0x00, 0x00,
		0x00, 0x08, 0x20, 0x00,	0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,	0x2D, 0xB1, 0x1B, 0x68,
		0x00, 0x00, 0x00, 0x00,	0x00, 0x00, 0x00, 0x00,
		0x00, 0x08, 0x20, 0x00,	0x00, 0x01, 0x00, 0x00,
		0x00, 0x01, 0x00, 0x00,	0x00, 0x00, 0x00, 0x00,
		0x00, 0x01, 0x00, 0x00,	0x00, 0x01, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,	0x00, 0x01, 0x00, 0x00,
		0x00, 0x01, 0x00, 0x00,	0x00, 0x00, 0x00, 0x00,
		0x00, 0x01, 0x00, 0x00,	0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,	0x00, 0x00, 0x00, 0x00
	};

	m_DragCursor	= CreateCursor( AfxGetApp()->m_hInstance, 15, 15, 32, 32,
									ANDmaskCursor, XORmaskCursor );

	RegisterWindowClass();
}

CSwapIndicator::~CSwapIndicator()
{
	DestroyCursor(m_DragCursor);
}

void CSwapIndicator::OnDrop(CSwapIndicator* pDropSource, CSwapIndicator* pDropTarget)
{
	// Notify parent window	
	// *** Add ON_NOTIFY_RANGE for message SI_SWAP to parent class ***	
	SIHDR sihdr;
	sihdr.hdr.code = SI_SWAP;
	sihdr.hdr.hwndFrom = m_hWnd;
	sihdr.hdr.idFrom = m_nCtrlID;
	sihdr.idDropSource = m_nCtrlID;
	sihdr.idDropTarget = m_pDropWnd->m_nCtrlID;
	sihdr.pWndSource = this;
	sihdr.pWndTarget = pDropTarget;
	m_pParentWnd->SendMessage(WM_NOTIFY, (WPARAM) (int) m_nCtrlID, (LPARAM) (LPNMHDR) &sihdr);	
}

BOOL CSwapIndicator::OnEraseBkgnd(CDC* pDC) 
{
	if (m_bmpCurrent != NULL)
        return TRUE;
	
	return CWnd::OnEraseBkgnd(pDC);
}

void CSwapIndicator::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_bIsDragging = TRUE;
	SetBitmap(m_bmpDrag);	
	m_Cursor = ::SetCursor(m_DragCursor);
	SetCapture();
	
	CWnd::OnLButtonDown(nFlags, point);
}

void CSwapIndicator::OnLButtonUp(UINT nFlags, CPoint point) 
{
	ReleaseCapture();
	m_bIsDragging = FALSE;
	::SetCursor(m_Cursor);	
	SetBitmap(m_bmpNormal);
	if (m_pDropWnd != NULL)
	{
		m_pDropWnd->SetBitmap(m_bmpNormal);	
		OnDrop(this, m_pDropWnd);
		m_pDropWnd = NULL;
	}
	
	CWnd::OnLButtonUp(nFlags, point);
}

int CSwapIndicator::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message) 
{
	SetFocus();	
	return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

void CSwapIndicator::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_bIsDragging)
	{
		CPoint pt(point);
		ClientToScreen(&pt);
		CWnd * pFoundWnd = WindowFromPoint(pt);
		ASSERT(pFoundWnd != NULL);

		if ( (m_pDropWnd != NULL) && (pFoundWnd != m_pDropWnd) )
		{				
				m_pDropWnd->SetBitmap(m_bmpNormal);
				m_pDropWnd = NULL;				
		}
		else
		{
			if ( (pFoundWnd->IsKindOf(RUNTIME_CLASS(CSwapIndicator))) && (pFoundWnd != this) )
			{
				if (pFoundWnd->GetParent() == m_pParentWnd) // only interact with windows with same parent
				{
					CSwapIndicator* pDropWnd = reinterpret_cast<CSwapIndicator*>(pFoundWnd);
					if (pDropWnd->m_nContextID == m_nContextID) // only interact with specified instances of window
					{
						m_pDropWnd = pDropWnd;
						m_pDropWnd->SetBitmap(m_bmpDragOver);	
					}
				}
			}		
		}
	}
	
	CWnd::OnMouseMove(nFlags, point);
}

void CSwapIndicator::OnPaint() 
{	
    if (m_bmpCurrent != NULL)
    {
		CPaintDC dc(this); 

        CDC dcMem;
        dcMem.CreateCompatibleDC(&dc);        

        CRect rect(dc.m_ps.rcPaint);

        CBitmap bitmap;
		bitmap.Attach(m_bmpCurrent);                
        CBitmap* pOldBitmap = (CBitmap*) dcMem.SelectObject(&bitmap);

        dc.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &dcMem, rect.left, rect.top, SRCCOPY);

        dcMem.SelectObject(pOldBitmap);      

		bitmap.Detach();
		dcMem.DeleteDC();
    }		
}

BOOL CSwapIndicator::RegisterWindowClass()
{
	WNDCLASS wndclass;
    HINSTANCE hInst = AfxGetInstanceHandle();

    if (!(::GetClassInfo(hInst, SWAPINDICATOR_CLASSNAME, &wndclass)))
    {
        wndclass.style            = CS_DBLCLKS | CS_PARENTDC;
        wndclass.lpfnWndProc      = ::DefWindowProc;
        wndclass.cbClsExtra       = wndclass.cbWndExtra = 0;
        wndclass.hInstance        = hInst;
        wndclass.hIcon            = NULL;
        wndclass.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
        wndclass.hbrBackground    = (HBRUSH) (COLOR_3DFACE + 1);
        wndclass.lpszMenuName     = NULL;
        wndclass.lpszClassName    = SWAPINDICATOR_CLASSNAME;

        if (!AfxRegisterClass(&wndclass))
        {
            AfxThrowResourceException();
            return FALSE;
        }
    }

    return TRUE;
}

HBITMAP CSwapIndicator::SetBitmap(HBITMAP hBitmap)
{
	HBITMAP bmpLast = m_bmpCurrent;
	m_bmpCurrent = hBitmap;
	if (m_hWnd != NULL)
		Invalidate();	
	return bmpLast;
}

void CSwapIndicator::SetBitmaps(HBITMAP arrBitmaps[])
{
	m_bmpNormal		= arrBitmaps[0];
	m_bmpDrag		= arrBitmaps[1];
	m_bmpDragOver	= arrBitmaps[2];
}
