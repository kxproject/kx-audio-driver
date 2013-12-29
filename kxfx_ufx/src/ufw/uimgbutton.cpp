
#include "ufw.h"
#include "uimgbutton.h"

//.............................................................................
// uImgButton

#define KXU_IMGBUTTON_CLASSNAME _T("UFX-ImgButton") // window class name

// uImgButton Class
uImgButton::uImgButton()
{
	kxuRegisterWindowClass(KXU_IMGBUTTON_CLASSNAME);
}

uImgButton::~uImgButton()
{
}

BEGIN_MESSAGE_MAP(uImgButton, uControl)
	//{{AFX_MSG_MAP(uImgButton)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//.............................................................................

const int dt_state_to_frame[] = 
{
	0, 1, na, na, 2, 3, na, na,
	4, 5, na, na, 2, 5, na, na,
	6, 7, na, na, 6, 7, na, na,
	6, 7, na, na, 6, 7, na, na
};

#define STATETOFRAME(f, s) (((f) >> (dt_state_to_frame[(s)] * 3)) & 7)

void uImgButton::OnPaint() 
{
	CDC MemDC;
	CRect rect;
	CBitmap *pMemOld, bm;

	CPaintDC dc(this); // device context for painting

	// Draw

	// Get Size of Display area
    GetClientRect(rect);

    // Create memory DCs
    if (!MemDC.CreateCompatibleDC(&dc)) return;
	pMemOld = (CBitmap*) MemDC.SelectObject(bmpImage);

	int i = STATETOFRAME(Order, State);
	i *= Size.right;
	
	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &MemDC, i, 0, SRCCOPY);
	
	MemDC.SelectObject(pMemOld);

	// Do not call CWnd::OnPaint() for painting messages
}

BOOL uImgButton::Create(CBitmap* Image, int FrameOrder, DWORD dwStyle, 
					 int x, int y, int w, int h, CWnd* pParentWnd, UINT nID)
{
	ASSERT(pParentWnd);
	pParentWindow = pParentWnd;
	mID = nID;
	SETRECT(Size, 0, 0, w, h);

	Style = dwStyle & UCS_CONTROL_MASK;
	State = UC_STATE_DEFAULT;
	Order = FrameOrder;
	bmpImage = Image;

	BOOL ret = CWnd::Create(KXU_IMGBUTTON_CLASSNAME, NULL, dwStyle,
							CRect(x, y, x + w, y + h),
							pParentWnd, nID);
	return ret;
}

//.............................................................................

void uImgButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	int PrevState = State & UC_STATE_HOVER;
	State = (State & ~UC_STATE_HOVER) | (UC_STATE_HOVER & -PtInRect(&Size, point));
	State = (State & ~UC_STATE_PUSHED) | (UC_STATE_PUSHED &  -((int) nFlags & MK_LBUTTON));
	if ((State & UC_STATE_HOVER) != PrevState)
	{
		if (State & UC_STATE_HOVER){
			SetCapture();
		}
		else {
			State &= ~UC_STATE_PUSHED;
			ReleaseCapture();
		}
		REDRAW;
	}
	
	// CWnd::OnMouseMove(nFlags, point);  
}

void uImgButton::OnLButtonDown(UINT /* nFlags */, CPoint /* point */) 
{
	SetCapture();
	State |= UC_STATE_PUSHED; 

	SetFocus();
	REDRAW;
	
	// CWnd::OnLButtonDown(nFlags, point); // drop through to default handler
}

void uImgButton::OnLButtonUp(UINT /* nFlags */, CPoint /* point */) 
{
	if (Style & (UIBS_2STATE)) State ^= UC_STATE_CHECKED;
	if (!(State & UC_STATE_HOVER)) ReleaseCapture(); 
	State &= ~UC_STATE_PUSHED;

	REDRAW;
	NotifyParent();
	
	// CWnd::OnLButtonUp(nFlags, point); // drop through to default handler
}

void uImgButton::OnKillFocus(CWnd* /* pNewWnd */)
{
	State &= ~UC_STATE_HOVER;
}

//.............................................................................
// Public Functions

int uImgButton::GetCheck() const
{
	return State & UC_CHECK_MASK;
}

void uImgButton::SetCheck(int check)
{
	State &= ~UC_STATE_CHECKED; 
	State |= check & UC_STATE_CHECKED; 
	REDRAW;
}
