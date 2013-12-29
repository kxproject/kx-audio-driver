
#include "ufw.h"
#include "uecombo.h"

/////////////////////////////////////////////////////////////////////////////
// uECombo

#define KXU_ECOMBO_CLASSNAME _T("UFX-Combo") // Window class name

uECombo::uECombo()
{
	kxuRegisterWindowClass(KXU_ECOMBO_CLASSNAME);
	ResetItems();

	// local
	bmpCaption = 0;
	Hovered = 0;

	HiddenItemsOffset = 0xffff;
}

uECombo::~uECombo()
{
	if (Menu.GetSafeHmenu()) Menu.DestroyMenu();
}

BEGIN_MESSAGE_MAP(uECombo, uControl)
	//{{AFX_MSG_MAP(uECombo)
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

#define EC_TEXTFORMAT	(DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX)

void uECombo::OnPaint() 
{ 
	CDC MemDC, TmpDC;
	CRect rect;
	CBitmap *pMemOld, *pTmpOld, bm;
	CBrush ArrowBrush;
	CRect DstRect;
	CBitmap* pSrcBitmap = &bmpFlat;
	COLORREF _ArrowColor = ArrowColor;

	CPaintDC dc(this); // device context for painting

	// Draw

	if ((State == UC_STATE_PUSHED) && !(Style & UECS_NOBUTTON)){
		pSrcBitmap = &bmpPressed;
	}

	// Get Size of Display area
    GetClientRect(rect);

    // Create memory DCs
    if (!TmpDC.CreateCompatibleDC(&dc)) return;
	MemDC.CreateCompatibleDC(&dc);
	bm.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
	pMemOld = (CBitmap*) MemDC.SelectObject(&bm);

	// Draw Item Box
	pTmpOld = (CBitmap*) TmpDC.SelectObject(pSrcBitmap);
	MemDC.BitBlt(0, 0, rect.Width(), rect.Height(), &TmpDC, 0, 0, SRCCOPY);

	// Draw Arrow
	if (!(Style & UECS_NOBUTTON)){
		CopyRect(&DstRect, &ArrowRect);
		if (State == UC_STATE_PUSHED){
			_ArrowColor = ArrowActiveColor;
			MOVERECT(DstRect, 0, 1);
		}
		if (Hovered == TRUE) _ArrowColor = ArrowActiveColor;
		_ArrowColor |= 1; // dirty fix, black SolidBrush causes strange gdi behaviour...
		ArrowBrush.CreateSolidBrush(_ArrowColor);
		MemDC.SelectObject(&ArrowBrush);
		TmpDC.SelectObject(&bmpArrow);
		MemDC.BitBlt(EXPANDRECTWH(DstRect), &TmpDC, 0, 0, 0x00B8074A);
	}

	// calc text/bitmap rect
	CopyRect(&DstRect, &Size);
	RESIZERECT(DstRect, 7, 2, 0, -3);
	if ((Style & 0xff00) != UECS_NOBUTTON) {DstRect.right -= RECTH(Size);}
	else {DstRect.right -= 7;}

	// Draw Bitmap
	if (bmpCaption)
	{
		
		DstRect.left -= 2;
		DstRect.top += 1;
		::SelectObject((HDC) TmpDC, bmpCaption);
		MemDC.BitBlt(EXPANDRECTWH(DstRect), &TmpDC, 0, 0, SRCAND);
		DstRect.left += 16;
		DstRect.top -= 1;
	}

	// Draw Text
	char Caption[MAX_ECOMBO_STRING];
	int tcount = GetWindowText(Caption, MAX_ECOMBO_STRING);
	if (tcount != 0){
		if ((State == UC_STATE_PUSHED) | (!(Style & UECS_NOITEMBOX))){
			MOVERECT(DstRect, 0, 1);
		}
		MemDC.SelectObject(uCore->GetFont());
		MemDC.SetBkMode(TRANSPARENT);
		MemDC.SetTextColor(CaptionColor);
		MemDC.DrawText(Caption, tcount, &DstRect, EC_TEXTFORMAT | (Style & 0xff));
	}

	// Screen Blt
	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &MemDC, 0, 0, SRCCOPY);
	TmpDC.SelectObject(pTmpOld);
	MemDC.SelectObject(pMemOld);
	
	// Do not call CWnd::OnPaint() for painting messages
}

BOOL uECombo::Create(LPCTSTR Caption, DWORD dwStyle, int x, int y, int w, int h, CWnd* pParentWnd, UINT nID)
{
	ASSERT(pParentWnd);
	pParentWindow = pParentWnd;
	mID = nID;

	Hovered = FALSE;
	Style = dwStyle & 0xffffu;
	State = UC_STATE_DEFAULT;
	ArrowColor = uCore->GetColor(_UC_BLACK);
	ArrowActiveColor = uCore->GetColor(_UC_HOVER);
	Size = CRect(0, 0, w, h);
	CaptionColor = 0;

	// Render Combo
	int nRenderItems = 3;
	int border = uCore->GetElementData(UCB_OUTER_SUNKEN, UED_BORDER);
	int   Elements[3] = {UCB_OUTER_SUNKEN, UCB_BOX_WHITE, UCB_BUTTON_RAISED};
	RECT xrect; CopyRect(&xrect, &Size);
	SCALERECT(xrect, -border, -border);
	border = uCore->GetElementData(UCB_BOX_WHITE, UED_BORDER);
	RECT* pRenderData[3] = {&Size, &xrect, &BtnRect};

	switch (Style & 0xff00)
	{
	case UECS_NOBUTTON:
		nRenderItems = 2;
		break;
	case UECS_NOITEMBOX:
		BtnRect = CRect(border, border, w - border, h - border);
		break;
	default:
		BtnRect = CRect(xrect.right - h + (border << 1), border, w - border, h - border);
		break;
	}

	uCore->GetControlBitmap(&bmpFlat, 0, nRenderItems, Elements, pRenderData);
	if (nRenderItems != 2){
		Elements[2] = UCB_BUTTON_SUNKEN;
		uCore->GetControlBitmap(&bmpPressed, 0, nRenderItems, Elements, pRenderData);
	}
	// Render Arrow
	Elements[0] = UCB_ARROW_SOUTH;
	SETRECT(ArrowRect, 0, 0, 0, 0); pRenderData[0] = &ArrowRect; 
	uCore->GetControlBitmap(&bmpArrow, 0, 1, Elements, pRenderData);

	// Calc ArrowRect
	xrect.left = xrect.right - (xrect.bottom - xrect.top);
	kxuCenterRectToAnotherOne(&ArrowRect, &xrect);	

	// Create Window
	BOOL ret = CWnd::Create(KXU_ECOMBO_CLASSNAME, Caption, dwStyle & UCS_WINDOW_MASK,
							CRect(x, y, x + w, y + h), pParentWnd, nID);
	return ret;
}

// User Input

void uECombo::OnMouseMove(UINT /* nFlags */, CPoint point) 
{
	BOOL PrevHovered = Hovered;
	if (State != UECS_NOBUTTON){
		Hovered = PtInRect(&BtnRect, point);
		if (Hovered != PrevHovered)
		{
			if (Hovered == TRUE){
				SetCapture();
			}
			else {
				ReleaseCapture();
			}
			REDRAW;
		}
	}
	
	// CWnd::OnMouseMove(nFlags, point);    State = UC_STATE_NORMAL; REDRAW;
}

void uECombo::OnLButtonDown(UINT /*  nFlags */, CPoint /* point */) 
{
	RECT rect;

	if (Style & UECS_NOBUTTON) {
		SwitchSelItem(-1);
	}
	else {
		if (1) /*(PtInRect(&BtnRect, point))*/ {
			State = UC_STATE_PUSHED; Hovered = FALSE; REDRAW;
			GetWindowRect(&rect);

			NotifyParent(UCBN_DROPDOWN);

			Menu.TrackPopupMenu(TPM_RIGHTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, rect.right, rect.bottom, this, NULL);
			State = UC_STATE_DEFAULT; REDRAW;
		}
	}

	// CWnd::OnLButtonDown(nFlags, point);
}

void uECombo::OnRButtonDown(UINT /* nFlags */, CPoint /* point */) 
{	
	if (Style & UECS_NOBUTTON) SwitchSelItem(1);
}

BOOL uECombo::SwitchSelItem(int step)
{
	int i, NewPos;
	int MenuItemsCount = Menu.GetMenuItemCount();
	UINT SelItemPos = Menu.GetMenuItemID(SelectedItem);
	if (SelItemPos != SelectedItem){
		for (i = 0; i < MenuItemsCount; i++){
			SelItemPos = Menu.GetMenuItemID(i);
			if (SelItemPos == SelectedItem){
				SelItemPos = i;
				break;
			}
		}
	}
	NewPos = SelItemPos + step;
	if (NewPos < 0) NewPos = MenuItemsCount + NewPos;
	else if (NewPos >= MenuItemsCount) NewPos = NewPos - MenuItemsCount;
	return SelectItem(Menu.GetMenuItemID(NewPos));
}

BOOL uECombo::OnCommand(WPARAM wParam, LPARAM /* lParam */) 
{
	BOOL ret = TRUE;
	if ((wParam >= ItemsOffset) && (wParam < HiddenItemsOffset))
	{
		SelectItem((UINT) wParam - ItemsOffset);
	}
	WPARAM nd = 0;
	if (Style & UECS_HIDDENNOTIFIES) nd = wParam;
	NotifyParent(UCBN_SELCHANGE, nd);

	State = UC_STATE_DEFAULT; REDRAW;

	// ret = CWnd::OnCommand(wParam, lParam);
	return ret;
}

// Internal Usage Functions

void uECombo::__setitemsoffset(UINT nIndex)
{
	if (!(Style & UECS_NOBUTTON)) ItemsOffset = nIndex;
}

void uECombo::__sethiddenitemsoffset(UINT nIndex)
{
	HiddenItemsOffset = nIndex;
}

// Public Functions

UINT uECombo::AddItem(LPCTSTR Item, const CBitmap* bitmap, UINT Flags)
{
	if (Flags & UECAI_INSERT_FIRST) Menu.InsertMenu(ItemsOffset, Flags & 0xffff, ItemsCount, Item);
	else Menu.AppendMenu(Flags & 0xffff, ItemsCount, Item);
	if (bitmap) Menu.SetMenuItemBitmaps(ItemsCount, MF_BYPOSITION, bitmap, NULL);
	
	return ItemsCount++;
}

BOOL uECombo::AddItems(UINT nItems, LPCTSTR pItems[])
{
	for (UINT i = 0; i < nItems; i++){
		AddItem(pItems[i], NULL, MF_STRING);
	}
	return TRUE;
}

UINT uECombo::GetCount()
{
	return ItemsCount - RemovedItems - ItemsOffset;
}

BOOL uECombo::DeleteItem(UINT nIndex)
{
	nIndex += ItemsOffset;
	BOOL ret = Menu.RemoveMenu(nIndex, MF_BYCOMMAND);
	if (ret == TRUE) RemovedItems++;
	return ret;
}

void uECombo::ResetItems()
{
	if (Menu.GetSafeHmenu()) Menu.DestroyMenu();
	Menu.CreatePopupMenu(); 
	ItemsCount = 0; 
	RemovedItems = 0;
	SelectedItem = 0;
	ItemsOffset = 0;
}

BOOL uECombo::SelectItem(UINT nIndex)
{
	BOOL ret;
	char Caption[MAX_ECOMBO_STRING];
	MENUITEMINFO mi = {sizeof MENUITEMINFO, MIIM_TYPE | MIIM_CHECKMARKS,}; 
	mi.dwTypeData = Caption;
	mi.cch = MAX_ECOMBO_STRING;
	nIndex += ItemsOffset;
	ret = Menu.GetMenuItemInfo(nIndex, &mi);
	if (!ret) return FALSE;
	bmpCaption = mi.hbmpUnchecked;
	SelectedItem = nIndex;
	if (!(Style & UECS_FIXEDCAPTION)){
		SetWindowText(Caption);
	}
	return TRUE;
}

UINT uECombo::GetSelItem()
{
	return SelectedItem - ItemsOffset;
}

void uECombo::SetArrowColor(COLORREF color)
{
	ArrowColor = color;
	if (GetSafeHwnd() != NULL) REDRAW;
}

void uECombo::SetArrowActiveColor(COLORREF color)
{
	ArrowActiveColor = color;
}

void uECombo::SetCaptionColor(COLORREF color)
{
	CaptionColor = color;
}

void uECombo::SetWindowText(LPCTSTR lpszString)
{
	if (GetSafeHwnd() != NULL){ 
		CWnd::SetWindowText(lpszString);
		REDRAW;
	}
}

void uECombo::SetItemDisabled(UINT nItem, BOOL nDisable)
{
	nItem += ItemsOffset;
	Menu.EnableMenuItem(nItem, nDisable | MF_BYCOMMAND);   
}

BOOL uECombo::GetItemDisabled(UINT nItem)
{
	nItem += ItemsOffset;
	return (Menu.GetMenuState(nItem, MF_BYCOMMAND)) & MF_GRAYED;
}

BOOL uECombo::SetItem(UINT nItem, LPCTSTR pItem)
{
	nItem += ItemsOffset;
	return Menu.ModifyMenu(nItem, MF_BYCOMMAND, nItem, pItem);
}

int uECombo::GetItem(UINT nItem, LPTSTR pItem, int nMaxCount)
{
	nItem += ItemsOffset;
	return Menu.GetMenuString(nItem, pItem, nMaxCount, MF_BYCOMMAND);
}
