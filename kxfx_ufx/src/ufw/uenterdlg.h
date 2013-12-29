#ifndef _U_ENTERDIALOG_H_
#define _U_ENTERDIALOG_H_

// #include "kxucore.h"

int AskForString(CWnd* pParent, LPCSTR Caption, LPSTR pDstText, LPCSTR pSrcText = "", int Reserved = 0);

/////////////////////////////////////////////////////////////////////////////
// uEnterDialog dialog

class uEnterDialog : public CDialog
{
public:
	uEnterDialog(CWnd* pParent);   // standard constructor

protected:
	kxuCore* uCore;
	CButton ok;
	CButton cancel;
	CEdit	edit;

public:
	const char*	caption;
	char*	text;

protected:
	virtual BOOL OnInitDialog();
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
    virtual BOOL DestroyWindow();
	DECLARE_MESSAGE_MAP()
};

#endif // _U_PLUGINCONTAINER_H_
