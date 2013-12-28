#ifndef __EDITDLGS_H__
#define __EDITDLGS_H__

BOOL PromptUserForLineNumber( CEdit *pEdit, HWND hWndParent, int &nLine );
BOOL CALLBACK FindDlgProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK ReplaceDlgProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
BOOL PromptUserForRepeatCount( CEdit *pEdit, HWND hWndParent, int &nCount );

#endif

