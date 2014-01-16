
#include "ufw.h"
#include "uenterdlg.h"

//.............................................................................

#define BTN_WIDTH	75
#define BTN_HEIGHT	23
#define EDIT_HEIGHT	20
#define SPACE1		12
#define SPACE2		8


const DLGTEMPLATE dt = {DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU, 
				0, 0, 0, 0, 180, 50};

//.............................................................................

int AskForString(CWnd* pParent, LPCSTR Caption, LPSTR pDstText, LPCSTR pSrcText, int /* Reserved */)
{
	strcpy(pDstText, pSrcText);
	uEnterDialog edlg(pParent);
	edlg.caption = Caption;
	edlg.text = pDstText;
	INT_PTR rv = edlg.DoModal();
	ASSERT(rv != -1);
	return int(rv);
}

//.............................................................................
// uEnterDialog dialog


uEnterDialog::uEnterDialog(CWnd* pParent)
{
	CDialog::CDialog();
	VERIFY(CDialog::InitModalIndirect(&dt, pParent));

	uCore = uGetCore();
	//{{AFX_DATA_INIT(uEnterDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

BEGIN_MESSAGE_MAP(uEnterDialog, CDialog)
	//{{AFX_MSG_MAP(uEnterDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//.............................................................................
// uEnterDialog message handlers

BOOL uEnterDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetWindowText(caption);

	RECT rect;
	GetClientRect(&rect);

	CFont* font = uCore->GetFont();

	rect.right = rect.right - SPACE1;
	rect.bottom = rect.bottom - SPACE1;
	rect.left = rect.right - BTN_WIDTH;
	rect.top = rect.bottom - BTN_HEIGHT;
	cancel.Create("&Cancel", BS_PUSHBUTTON | WS_VISIBLE, rect, this, 101);
	cancel.SetFont(font);

	rect.right = rect.left - SPACE2;
	rect.left = rect.right - BTN_WIDTH;
	ok.Create("&Ok", BS_DEFPUSHBUTTON | WS_VISIBLE, rect, this, 100);
	ok.SetFont(font);

	GetClientRect(&rect);

	rect.left = SPACE1;
	rect.top = SPACE1;
	rect.right = rect.right - SPACE1;
	rect.bottom = rect.top + EDIT_HEIGHT;
	edit.CreateEx(WS_EX_CLIENTEDGE, WC_EDIT, text, ES_LEFT | ES_AUTOHSCROLL | 
		ES_NOHIDESEL | WS_VISIBLE | WS_CHILD, rect, this, 102);
	edit.SetFont(font);
	edit.SetSel(0, -1);
	edit.SetFocus();

	return FALSE; 
}

BOOL uEnterDialog::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	switch (wParam & 0xffff)
	{
	case 100:
		EndDialog(IDOK);
		return 1;
	case 101:
		EndDialog(IDCANCEL);
		return 1;
	}
	
	return CDialog::OnCommand(wParam, lParam);
}

BOOL uEnterDialog::DestroyWindow() 
{
	edit.GetWindowText(text, 256);

	return CDialog::DestroyWindow();
}
