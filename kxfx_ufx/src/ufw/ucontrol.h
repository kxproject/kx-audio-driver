
#ifndef _U_CONTROL_H_
#define _U_CONTROL_H_

class kxuCore;

//.............................................................................

#define UCS_WINDOW_MASK		0xffff0000
#define UCS_CONTROL_MASK	0x0000ffff

//.............................................................................
// notify codes
#define UC_NOTIFY_CODE_MASK	0xffff0000
#define UC_NOTIFY_ID_MASK	0x0000ffff

#define UCN_DEFAULT		0x00000000
#define UCN_SETFOCUS	0x00030000
#define UCN_SETRESET	0x00200000

//.............................................................................

#define UC_STATE_DEFAULT	0x0
#define UC_STATE_CHECKED	0x1
#define UC_CHECK_MASK		(UC_STATE_DEFAULT | UC_STATE_CHECKED)
#define UC_STATE_PUSHED		0x4
#define UC_STATE_FOCUS		0x8
#define UC_STATE_HOVER		UC_STATE_FOCUS
#define UC_STATE_DISABLED	0x10

//.............................................................................

#define KXU_CONTROL_CLASSNAME _T("UFX-Ctrl")

#define WM_UCONTROL_MESSAGE	(WM_USER + 33)

#define REDRAW RedrawWindow(NULL, NULL, RDW_UPDATENOW|RDW_INVALIDATE|RDW_NOERASE);

//.............................................................................
// uControl window

class uControl : public CWnd
{
public:
	uControl();
	virtual ~uControl();

protected:
	kxuCore* uCore;

	CWnd* pParentWindow;
	UINT  mID;	

	DWORD Style;
	DWORD State;
	RECT  Size;

	LPCTSTR tip;
	int focus;

public:
	void SetToolTipString(LPCTSTR Tip) {tip = Tip;};
	LPCTSTR GetToolTipString() {return tip;};

    const RECT& size() {return Size;}

protected:
	LRESULT NotifyParent(WPARAM NotifyCode = 0, LPARAM NotifyData = 0);

	afx_msg void OnSetFocus(CWnd* pOldWnd);
	DECLARE_MESSAGE_MAP()
};

//.............................................................................

#ifndef EXPANDRECT
#define EXPANDRECT(r) (r).left, (r).top, (r).right , (r).bottom
#endif

#ifndef EXPANDRECTWH
#define EXPANDRECTWH(r) (r).left, (r).top, (r).right - (r).left, (r).bottom - (r).top
#endif

//.............................................................................

#endif // _U_BUTTON_H_
