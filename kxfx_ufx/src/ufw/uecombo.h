
#ifndef _U_ECOMBO_H_
#define _U_ECOMBO_H_

/***************************** Global Functions ****************************/
   
// ECombo Styles
#define UECS_LEFT			0x0000
#define UECS_CENTER			0x0001
#define UECS_RIGHT			0x0002

#define UECS_NOBUTTON		0x0004
#define UECS_NOITEMBOX		0x0008
#define UECS_FIXEDCAPTION	0x0010
#define UECS_NOHOVER		0x0020

#define UECS_HIDDENNOTIFIES 0x0800

#define UECS_DROPDOWN		UECS_LEFT
#define UECS_BUTTON			(UECS_NOITEMBOX | UECS_FIXEDCAPTION)
#define UECS_SWITCHBOX		(UECS_NOBUTTON | UECS_RIGHT)

#define MAX_ECOMBO_STRING 63

#define UECAI_INSERT_FIRST	0x10000

// notify codes
#define UCBN_SELCHANGE	UCN_DEFAULT
#define UCBN_DROPDOWN	0x00070000

/////////////////////////////////////////////////////////////////////////////
// uECombo window

class uECombo : public uControl
{
public:
	uECombo();
	virtual ~uECombo();

public:
	BOOL SelectItem(UINT nIndex);
	void SetCaptionColor(COLORREF color);
	int  GetItem(UINT nItem, LPTSTR pItem, int nMaxCount);
	BOOL SetItem(UINT nItem, LPCTSTR pItem);
	BOOL GetItemDisabled(UINT nItem);
	void SetItemDisabled(UINT nItem, BOOL nDisable = TRUE);
	void SetWindowText(LPCTSTR lpszString);
	void SetArrowActiveColor(COLORREF color);
	void SetArrowColor(COLORREF color);
	UINT GetSelItem();
	void ResetItems();
	BOOL DeleteItem(UINT nIndex);
	UINT GetCount();
	BOOL AddItems(UINT nItems, LPCTSTR pItems[]);
	UINT AddItem(LPCTSTR Item, UINT Flags = 0) {return AddItem(Item, NULL, Flags);};
	UINT AddItem(LPCTSTR Item, const CBitmap* bitmap, UINT Flags = 0);
	void __setitemsoffset(UINT nIndex);
	void __sethiddenitemsoffset(UINT nIndex);
	BOOL Create(LPCTSTR Caption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
	{return Create(Caption, dwStyle, EXPANDRECTWH(rect), pParentWnd, nID);}
	BOOL Create(LPCTSTR Caption, DWORD dwStyle,  int x, int y, int w, int h, CWnd* pParentWnd, UINT nID);
	
protected:
	
	UINT  ItemsOffset;
	UINT  HiddenItemsOffset;
	UINT  ItemsCount;
	UINT  RemovedItems;
	UINT  SelectedItem;

	RECT	BtnRect;
	RECT	ArrowRect;
	CBitmap bmpFlat;
	CBitmap bmpPressed;
	CBitmap bmpArrow;
	HBITMAP bmpCaption;

	BOOL	Hovered;

	COLORREF CaptionColor;
	COLORREF ArrowColor;
	COLORREF ArrowActiveColor;

	CMenu	Menu;

protected:
	BOOL SwitchSelItem(int step);

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

#endif // _U_ECOMBO_H_
