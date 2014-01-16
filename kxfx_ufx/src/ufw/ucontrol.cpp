
#include "ufw.h"
#include "ucontrol.h"

/////////////////////////////////////////////////////////////////////////////
// uControl

uControl::uControl()
{
	uCore = uGetCore();
	pParentWindow = NULL;
	mID = 0;	
	Style = 0;
	State = 0;
	tip = NULL;
	focus = 0;
}

uControl::~uControl()
{
}

LRESULT uControl::NotifyParent(WPARAM NotifyCode, LPARAM NotifyData)
{
	return pParentWindow->SendMessage(WM_UCONTROL_MESSAGE, mID | NotifyCode, NotifyData);
}

BEGIN_MESSAGE_MAP(uControl, CWnd)
	//{{AFX_MSG_MAP(uControl)
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// uControl message handlers

void uControl::OnSetFocus(CWnd* pOldWnd) 
{
	CWnd::OnSetFocus(pOldWnd);
	NotifyParent(UCN_SETFOCUS);
}
