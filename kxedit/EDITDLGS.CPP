#include "precomp.h"
#include "editdlgs.h"
#include "resource.h"

extern HINSTANCE hInstance;

typedef struct 
{
	CEdit *pEdit;
	TCHAR szLine[ 15 ];
	int nRadix;
} GotoLineInfo;

BOOL CALLBACK GotoLineDlgProc( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	GotoLineInfo *pInfo = ( GotoLineInfo * )GetWindowLong( hWndDlg, GWL_USERDATA );

	BOOL bProcessed = FALSE;

	switch ( uMsg )
	{
		case WM_INITDIALOG:
		{
			GotoLineInfo *pInfo = ( GotoLineInfo * ) lParam;
			SetWindowLong( hWndDlg, GWL_USERDATA, lParam );
			pInfo->pEdit->RestoreDlgPos( hWndDlg, IDD_GOTO_LINE );
			// limit the text to just enough space for the maximum number allowed
			SendMessage( GetDlgItem( hWndDlg, IDC_LINENO ), EM_LIMITTEXT, _log( INT_MAX >> 1, ( ( GotoLineInfo * ) lParam )->nRadix ), 0 );
			SendMessage( hWndDlg, WM_COMMAND, MAKEWPARAM( IDC_LINENO, EN_CHANGE ), 0 );
			bProcessed = TRUE;
			break;
		}

		case WM_COMMAND:
		{
			WORD wID = LOWORD(wParam);  

			switch ( HIWORD( wParam ) )
			{
				case BN_CLICKED:
				{
					ASSERT( pInfo );
					GetDlgItemText( hWndDlg, IDC_LINENO, pInfo->szLine, ARRAY_SIZE( pInfo->szLine ) - 1 );
					pInfo->pEdit->SaveDlgPos( hWndDlg, IDD_GOTO_LINE );
					EndDialog( hWndDlg, ( wID == IDOK ) ? IDOK : IDCANCEL );
					bProcessed = TRUE;
					break;
				}

				case EN_CHANGE:
				{
					EnableWindow( GetDlgItem( hWndDlg, IDOK ), 
					              GetWindowTextLength( GetDlgItem( hWndDlg, IDC_LINENO ) ) > 0 );
					bProcessed = TRUE;
					break;
				}
			}
			break;
		}
	}

	return bProcessed;
}

BOOL PromptUserForLineNumber( CEdit *pEdit, HWND hWndParent, int &nLine )
{
	nLine = -1;
	const CM_LINENUMBERING *pln = pEdit->GetLineNumbering();
	GotoLineInfo info;
	info.pEdit = pEdit;
	if ( pln->bEnabled )
	{
		info.nRadix = ( int ) pln->dwStyle;
		if ( !info.nRadix )
			info.nRadix = CM_DECIMAL;
	}
	else
	{
		info.nRadix = CM_DECIMAL;
	}

	BOOL bOK = IDOK == pEdit->DialogBoxParam( hInstance, 
                                              IDD_GOTO_LINE, 
	                                          hWndParent,
	                                          ( DLGPROC ) GotoLineDlgProc,
	                                          ( LPARAM )&info );

	if ( bOK )
	{
		nLine = _atoi( info.szLine, info.nRadix );
		if ( pln->bEnabled )
		{
			// normalize to buffer index
			nLine -= pln->nStartAt;
		}
		else
		{
			// default to 1-based decimal
			nLine -= 1;
		}

		nLine = max( 0, nLine );
	}

	// some ActiveX containers cause the caret to go away -- put it back
	pEdit->FlashCaret();
	return bOK;
}

typedef struct 
{
	CEdit *pEdit;
	int nCount;
} RepeatInfo;

BOOL CALLBACK RepeatCountDlgProc( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	RepeatInfo *pInfo = ( RepeatInfo * )GetWindowLong( hWndDlg, GWL_USERDATA );

	BOOL bProcessed = FALSE;

	switch ( uMsg )
	{
		case WM_INITDIALOG:
		{
			RepeatInfo *pInfo = ( RepeatInfo * ) lParam;
			pInfo->pEdit->RestoreDlgPos( hWndDlg, IDD_REPEAT_COUNT );
			SetWindowLong( hWndDlg, GWL_USERDATA, lParam );
			SendMessage( GetDlgItem( hWndDlg, IDC_REPEAT_COUNT ), EM_LIMITTEXT, 4, 0 );
			SetDlgItemInt( hWndDlg, IDC_REPEAT_COUNT, 10, FALSE );
			SendMessage( hWndDlg, WM_COMMAND, MAKEWPARAM( IDC_REPEAT_COUNT, EN_CHANGE ), 0 );
			bProcessed = TRUE;
			break;
		}

		case WM_COMMAND:
		{
			WORD wID = LOWORD(wParam);  

			switch ( HIWORD( wParam ) )
			{
				case BN_CLICKED:
				{
					ASSERT( pInfo );
					pInfo->nCount = GetDlgItemInt( hWndDlg, IDC_REPEAT_COUNT, NULL, FALSE );
					pInfo->pEdit->SaveDlgPos( hWndDlg, IDD_REPEAT_COUNT );
					EndDialog( hWndDlg, ( wID == IDOK ) ? IDOK : IDCANCEL );
					bProcessed = TRUE;
					break;
				}

				case EN_CHANGE:
				{
					EnableWindow( GetDlgItem( hWndDlg, IDOK ), 
					              GetWindowTextLength( GetDlgItem( hWndDlg, IDC_REPEAT_COUNT ) ) > 0 );
					bProcessed = TRUE;
					break;
				}
			}
			break;
		}
	}

	return bProcessed;
}

BOOL PromptUserForRepeatCount( CEdit *pEdit, HWND hWndParent, int &nCount )
{
	nCount = 0;
	RepeatInfo ri = { pEdit, nCount };
	BOOL bOK = IDOK == pEdit->DialogBoxParam( hInstance, 
                                              IDD_REPEAT_COUNT, 
	                                          hWndParent,
	                                          ( DLGPROC ) RepeatCountDlgProc,
	                                          ( LPARAM )&ri );

	if ( bOK )
	{
		nCount = ri.nCount;
	}

	// some ActiveX containers cause the caret to go away -- put it back
	pEdit->FlashCaret();

	return bOK;
}

void SaveMRUList( HWND hWndCombo, CFindReplaceData *pData, BOOL bFind )
{
	ASSERT( pData );

	TCHAR szText[ CM_MAX_FINDREPL_TEXT + 2 ];
	SendMessage( hWndCombo, WM_GETTEXT, ARRAY_SIZE( szText ), ( LPARAM ) szText );
	ASSERT( !bFind || *szText );
	TCHAR szFindText[ CM_MAX_FINDREPL_TEXT + 1 ];
	_tcscpy( szFindText, szText );

	int cb = 0;
	LPTSTR pszMRU = ( LPTSTR ) malloc( sizeof( TCHAR ) );
	*pszMRU = _T('\0');
	
	int nItems = SendMessage( hWndCombo, CB_GETCOUNT, 0, 0 );
	int nMRUCount = 0;
	int nItem = -1;

	do
	{
		if ( ++nMRUCount > CM_FIND_REPLACE_MRU_MAX )
		{
			break;
		}
		// avoid duplicates
		if ( nItem == -1 || _tcscmp( szText, szFindText ) != 0 )
		{
			_tcscat( szText, _T("\n") );
			cb += _tcslen( szText );
			pszMRU = ( LPTSTR )realloc( pszMRU, ( cb + 1 ) * sizeof( TCHAR ) );
			_tcscat( pszMRU, szText );
		}
		nItem++;
		SendMessage( hWndCombo, CB_GETLBTEXT, nItem, ( LPARAM ) szText );
	}
	while ( nItem < nItems );

	if ( nMRUCount )
	{
		// remove the last '\n'
		pszMRU[ cb - 1 ] = _T('\0');
	}

	if ( bFind )
	{
		if ( pData->m_pszFindMRUList )
		{
			free( pData->m_pszFindMRUList );
		}
		pData->m_pszFindMRUList = pszMRU;
	}
	else
	{
		if ( pData->m_pszReplaceMRUList )
		{
			free( pData->m_pszReplaceMRUList );
		}
		pData->m_pszReplaceMRUList = pszMRU;
	}
}

void LoadMRUList( HWND hWndCombo, CFindReplaceData *pData, BOOL bFind )
{
	ASSERT( pData );

	SendMessage( hWndCombo, CB_RESETCONTENT, 0, 0 );

	LPCTSTR psz = bFind ? pData->m_pszFindMRUList : pData->m_pszReplaceMRUList;

	if ( psz && *psz )
	{
		BOOL bDone = FALSE;
		LPCTSTR pszItemStart = psz;
		for ( int i = 0; !bDone && i < CM_FIND_REPLACE_MRU_MAX; i++ )
		{
			for ( ;; )
			{
				if ( *psz == _T('\n') || !*psz )
				{
					int cbItem = psz - pszItemStart;
					if ( ( cbItem >= 0 ) && ( cbItem <= CM_MAX_FINDREPL_TEXT ) )
					{
						TCHAR szTemp[ CM_MAX_FINDREPL_TEXT + 1 ];
						*szTemp = _T('\0');
						_tcsncpy( szTemp, pszItemStart, cbItem );
						szTemp[ cbItem ] = _T('\0');
						SendMessage( hWndCombo, CB_ADDSTRING, 0, ( LPARAM ) szTemp );
					}
					if ( !*psz )
					{
						bDone = TRUE;
						break;
					}
					pszItemStart = psz + 1;
				}

				psz++;
			}
		}
		if ( !bFind )
		{
			// select the first item in the list if this is the Replace MRU list
			SendMessage( hWndCombo, CB_SETCURSEL, 0, 0 );
		}
	}
}

#define UM_UPDATEFINDREPLACETEXT		WM_USER + 100
#define UM_ENABLEBUTTONS				WM_USER + 101
#define	UM_SAVEMRULISTS					WM_USER + 102

BOOL CALLBACK FindDlgProc( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	CEdit *pCtrl = ( CEdit * )GetWindowLong( hWndDlg, GWL_USERDATA );

	BOOL bProcessed = FALSE;

	switch ( uMsg )
	{
		case WM_INITDIALOG:
		{
			( ( CEdit * ) lParam )->RestoreDlgPos( hWndDlg, IDD_FIND );
			SetWindowLong( hWndDlg, GWL_USERDATA, lParam );
			pCtrl = ( CEdit * ) lParam;
			HWND hWndCombo = GetDlgItem( hWndDlg, IDC_FIND_TEXT );
			SendMessage( hWndCombo, CB_LIMITTEXT, CM_MAX_FINDREPL_TEXT, 0 );
			CheckDlgButton( hWndDlg, IDC_WHOLE_WORD, CEdit::g_FindReplaceData.m_bWholeWordOnly );
			CheckDlgButton( hWndDlg, IDC_MATCH_CASE, CEdit::g_FindReplaceData.m_bCaseSensitiveSearch );
			CheckDlgButton( hWndDlg, IDC_REG_EXP, CEdit::g_FindReplaceData.m_bRegExp );
			CheckDlgButton( hWndDlg, IDC_DOWN, TRUE );
			LoadMRUList( hWndCombo, &CEdit::g_FindReplaceData, TRUE );

			HGLOBAL hMem = NULL;
			if ( ( pCtrl->m_Selection.GetStartRow() == pCtrl->m_Selection.GetEndRow() ) &&
			     !pCtrl->m_Selection.IsEmpty() )
			{
				hMem = pCtrl->GetSelectionText();
			}
			else if ( !pCtrl->m_Buffer.LineIsEmpty( pCtrl->m_Selection.GetEndRow(), TRUE ) )
			{
				CSaveSelection save( pCtrl );
				// if nothing selected or more than one line is selected, pick the word
				pCtrl->m_Selection.MakeEmpty( FALSE );
				pCtrl->m_Selection.Extend( CSelection::eOutward, CSelection::eWord, FALSE, FALSE, FALSE );
				hMem = pCtrl->GetSelectionText();
			}

			if ( hMem )
			{
				LPTSTR pszText = ( LPTSTR ) GlobalLock( hMem );
				if ( pszText && *pszText )
				{
					SendMessage( hWndCombo, WM_SETTEXT, 0, ( LPARAM ) pszText );
				}
				GlobalUnlock( hMem ); GlobalFree( hMem );
			}

			SendMessage( hWndDlg, WM_COMMAND, MAKEWPARAM( IDC_FIND_TEXT, CBN_EDITCHANGE ), 0 );
			bProcessed = TRUE;
			break;
		}

		case UM_UPDATEFINDREPLACETEXT:
		{
			TCHAR sz[ CM_MAX_FINDREPL_TEXT + 1 ];
			sz[ CM_MAX_FINDREPL_TEXT ] = _T('\0');
			VERIFY( SendMessage( GetDlgItem( hWndDlg, IDC_FIND_TEXT ), WM_GETTEXT, CM_MAX_FINDREPL_TEXT, ( LPARAM ) sz ) != 0 );
			if ( !CEdit::g_FindReplaceData.m_pszFindText || _tcscmp( CEdit::g_FindReplaceData.m_pszFindText, sz ) != 0 )
			{
				pCtrl->ExecuteCommand( CMD_SETFINDTEXT, ( DWORD ) sz, FALSE, TRUE );
			}
			break;
		}

		case WM_COMMAND:
		{
			WORD wID = LOWORD( wParam );
			WORD wCmd = HIWORD( wParam );

			switch ( wCmd )
			{
				case BN_CLICKED:
				{
					ASSERT( pCtrl );
					switch ( wID )
					{
						case IDOK:
							SendMessage( hWndDlg, UM_UPDATEFINDREPLACETEXT, 0, 0 );
							if ( IsDlgButtonChecked( hWndDlg, IDC_DOWN ) )
							{
								pCtrl->ExecuteCommand( CMD_FINDNEXT, 0, FALSE, TRUE );
							}
							else
							{
								pCtrl->ExecuteCommand( CMD_FINDPREV, 0, FALSE, TRUE );
							}
							SaveMRUList( GetDlgItem( hWndDlg, IDC_FIND_TEXT ), &CEdit::g_FindReplaceData, TRUE );
							pCtrl->SaveDlgPos( hWndDlg, IDD_FIND );
							EndDialog( hWndDlg, IDOK );
							break;
						case IDC_MARK_ALL:
							SendMessage( hWndDlg, UM_UPDATEFINDREPLACETEXT, 0, 0 );
							pCtrl->ExecuteCommand( CMD_FINDMARKALL, 0, FALSE, TRUE );
							SaveMRUList( GetDlgItem( hWndDlg, IDC_FIND_TEXT ), &CEdit::g_FindReplaceData, TRUE );
							EndDialog( hWndDlg, IDC_MARK_ALL );
							break;
						case IDC_WHOLE_WORD:
							pCtrl->ExecuteCommand( CMD_TOGGLEWHOLEWORD, 0, FALSE, TRUE );
							break;
						case IDC_REG_EXP:
							pCtrl->ExecuteCommand( CMD_TOGGLEREGEXP, 0, FALSE, TRUE );
							break;
						case IDC_MATCH_CASE:
							pCtrl->ExecuteCommand( CMD_TOGGLECASESENSITIVE, 0, FALSE, TRUE );
							break;
						case IDCANCEL:
							pCtrl->SaveDlgPos( hWndDlg, IDD_FIND );
							EndDialog( hWndDlg, IDCANCEL );
							break;
					}
					bProcessed = TRUE;
					break;
				}

				case CBN_SELCHANGE:
				case CBN_EDITCHANGE:
				{
					HWND hWndCombo = GetDlgItem( hWndDlg, IDC_FIND_TEXT );
					BOOL bEnable = ( ( wCmd == CBN_EDITCHANGE ) && ( GetWindowTextLength( hWndCombo ) > 0 ) ) ||
					               ( ( wCmd == CBN_SELCHANGE ) && ( SendMessage( hWndCombo, CB_GETCURSEL, 0, 0 ) != CB_ERR ) );
					EnableWindow( GetDlgItem( hWndDlg, IDOK ), bEnable );
					EnableWindow( GetDlgItem( hWndDlg, IDC_MARK_ALL ), bEnable );
					bProcessed = TRUE;
					break;
				}
			}
			break;
		}
	}

	return bProcessed;
}

BOOL CALLBACK ReplaceDlgProc( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	CEdit *pCtrl = ( CEdit * )GetWindowLong( hWndDlg, GWL_USERDATA );

	BOOL bProcessed = FALSE;

	switch ( uMsg )
	{
		case WM_INITDIALOG:
		{
			( ( CEdit * ) lParam )->RestoreDlgPos( hWndDlg, IDD_REPLACE );
			SetWindowLong( hWndDlg, GWL_USERDATA, lParam );
			pCtrl = ( CEdit * ) lParam;
			HWND hWndFindCombo = GetDlgItem( hWndDlg, IDC_FIND_TEXT );
			HWND hWndReplaceCombo = GetDlgItem( hWndDlg, IDC_REPLACE_TEXT );
			SendMessage( hWndFindCombo, CB_LIMITTEXT, CM_MAX_FINDREPL_TEXT, 0 );
			SendMessage( hWndReplaceCombo, CB_LIMITTEXT, CM_MAX_FINDREPL_TEXT, 0 );
			CheckDlgButton( hWndDlg, IDC_PRESERVE_CASE, CEdit::g_FindReplaceData.m_bPreserveCase );
			CheckDlgButton( hWndDlg, IDC_WHOLE_WORD, CEdit::g_FindReplaceData.m_bWholeWordOnly );
			CheckDlgButton( hWndDlg, IDC_REG_EXP, CEdit::g_FindReplaceData.m_bRegExp );
			CheckDlgButton( hWndDlg, IDC_MATCH_CASE, CEdit::g_FindReplaceData.m_bCaseSensitiveSearch );
			BOOL bEmptySel = pCtrl->m_Selection.IsEmpty();
			EnableWindow( GetDlgItem( hWndDlg, IDC_SELECTION ), !bEmptySel );
			LoadMRUList( hWndFindCombo, &CEdit::g_FindReplaceData, TRUE );
			LoadMRUList( hWndReplaceCombo, &CEdit::g_FindReplaceData, FALSE );

			HGLOBAL hMem = NULL;
			BOOL bReplaceInSel = FALSE;
			if ( ( pCtrl->m_Selection.GetStartRow() == pCtrl->m_Selection.GetEndRow() ) &&
			     !bEmptySel )
			{
				bReplaceInSel = FALSE;
				hMem = pCtrl->GetSelectionText();
			}
			else
			{
				bReplaceInSel = !bEmptySel;
				if ( !pCtrl->m_Buffer.LineIsEmpty( pCtrl->m_Selection.GetEndRow(), TRUE ) )
				{
					CSaveSelection save( pCtrl );
					// if nothing selected or more than one line is selected, pick the word
					pCtrl->m_Selection.MakeEmpty( FALSE );
					pCtrl->m_Selection.Extend( CSelection::eOutward, CSelection::eWord, FALSE, FALSE, FALSE );
					hMem = pCtrl->GetSelectionText();
				}
			}

			if ( hMem )
			{
				LPTSTR pszText = ( LPTSTR ) GlobalLock( hMem );
				if ( pszText && *pszText )
				{
					SendMessage( hWndFindCombo, WM_SETTEXT, 0, ( LPARAM ) pszText );
				}
				GlobalUnlock( hMem ); GlobalFree( hMem );
			}

			CheckDlgButton( hWndDlg, IDC_SELECTION, bReplaceInSel );
			CheckDlgButton( hWndDlg, IDC_BUFFER, !bReplaceInSel );
			SendMessage( hWndDlg, WM_COMMAND, MAKEWPARAM( IDC_FIND_TEXT, CBN_EDITCHANGE ), 0 );
			bProcessed = TRUE;
			break;
		}

		case UM_UPDATEFINDREPLACETEXT:
		{
			TCHAR sz[ CM_MAX_FINDREPL_TEXT + 1 ];
			sz[ CM_MAX_FINDREPL_TEXT ] = _T('\0');
			VERIFY( SendMessage( GetDlgItem( hWndDlg, IDC_FIND_TEXT ), WM_GETTEXT, CM_MAX_FINDREPL_TEXT, ( LPARAM ) sz ) != 0 );
			if ( !CEdit::g_FindReplaceData.m_pszFindText || _tcscmp( CEdit::g_FindReplaceData.m_pszFindText, sz ) != 0 )
			{
				pCtrl->ExecuteCommand( CMD_SETFINDTEXT, ( DWORD ) sz, FALSE, TRUE );
			}

			SendMessage( GetDlgItem( hWndDlg, IDC_REPLACE_TEXT ), WM_GETTEXT, CM_MAX_FINDREPL_TEXT, ( LPARAM ) sz );
			if ( !CEdit::g_FindReplaceData.m_pszReplaceText || _tcscmp( CEdit::g_FindReplaceData.m_pszReplaceText, sz ) != 0 )
			{
				pCtrl->ExecuteCommand( CMD_SETREPLACETEXT, ( DWORD ) sz, FALSE, TRUE );
			}
			break;
		}

		case UM_ENABLEBUTTONS:
		{
			HWND hWndCombo = GetDlgItem( hWndDlg, IDC_FIND_TEXT );
			BOOL bEnable = ( ( wParam == CBN_SELCHANGE || wParam == BN_CLICKED || wParam == CBN_EDITCHANGE ) && ( GetWindowTextLength( hWndCombo ) > 0 ) );
			BOOL bSelection = IsDlgButtonChecked( hWndDlg, IDC_SELECTION );
			EnableWindow( GetDlgItem( hWndDlg, IDOK ), bEnable && !bSelection );
			EnableWindow( GetDlgItem( hWndDlg, IDC_REPLACE ), bEnable && !bSelection );
			EnableWindow( GetDlgItem( hWndDlg, IDC_REPLACE_ALL ), bEnable );
			break;
		}

		case UM_SAVEMRULISTS:
		{
			SaveMRUList( GetDlgItem( hWndDlg, IDC_FIND_TEXT ), &CEdit::g_FindReplaceData, TRUE );
			SaveMRUList( GetDlgItem( hWndDlg, IDC_REPLACE_TEXT ), &CEdit::g_FindReplaceData, FALSE );
			break;
		}

		case WM_COMMAND:
		{
			WORD wID = LOWORD(wParam);  
			WORD wCmd = HIWORD( wParam );

			switch ( wCmd )
			{
				case BN_CLICKED:
				{
					ASSERT( pCtrl );
					switch ( wID )
					{
						case IDOK:
							SendMessage( hWndDlg, UM_UPDATEFINDREPLACETEXT, 0, 0 );
							SendMessage( hWndDlg, UM_SAVEMRULISTS, 0, 0 );
							pCtrl->FindNext();
							// make sure SELCHANGE notifications are emitted now,
							// rather than later once the dialog is dismissed.
							pCtrl->ProcessViewSelNotifications();
							break;
						case IDC_REPLACE:
							SendMessage( hWndDlg, UM_UPDATEFINDREPLACETEXT, 0, 0 );
							pCtrl->ExecuteCommand( CMD_REPLACE, 0, FALSE, TRUE );
							// make sure SELCHANGE notifications are emitted now,
							// rather than later once the dialog is dismissed.
							pCtrl->ProcessViewSelNotifications();
							SendMessage( hWndDlg, UM_SAVEMRULISTS, 0, 0 );
							break;
						case IDC_REPLACE_ALL:
							SendMessage( hWndDlg, UM_UPDATEFINDREPLACETEXT, 0, 0 );
							if ( IsDlgButtonChecked( hWndDlg, IDC_BUFFER ) )
							{
								pCtrl->ExecuteCommand( CMD_REPLACEALLINBUFFER, 0, FALSE, TRUE );
							}
							else
							{
								pCtrl->ExecuteCommand( CMD_REPLACEALLINSELECTION, 0, FALSE, TRUE );
							}
							SendMessage( hWndDlg, UM_SAVEMRULISTS, 0, 0 );
							EndDialog( hWndDlg, IDC_REPLACE_ALL );
							break;
						case IDC_WHOLE_WORD:
							pCtrl->ExecuteCommand( CMD_TOGGLEWHOLEWORD, 0, FALSE, TRUE );
							break;
						case IDC_REG_EXP:
							pCtrl->ExecuteCommand( CMD_TOGGLEREGEXP, 0, FALSE, TRUE );
							break;
						case IDC_PRESERVE_CASE:
							pCtrl->ExecuteCommand( CMD_TOGGLEPRESERVECASE, 0, FALSE, TRUE );
							break;
						case IDC_MATCH_CASE:
							pCtrl->ExecuteCommand( CMD_TOGGLECASESENSITIVE, 0, FALSE, TRUE );
							break;
						case IDCANCEL:
							pCtrl->SaveDlgPos( hWndDlg, IDD_REPLACE );
							EndDialog( hWndDlg, IDCANCEL );
							break;
						case IDC_SELECTION:
						case IDC_BUFFER:
							SendMessage( hWndDlg, UM_ENABLEBUTTONS, wCmd, 0 );
							break;
					}
					bProcessed = TRUE;
					break;
				}

				case CBN_SELCHANGE:
				case CBN_EDITCHANGE:
				{
					SendMessage( hWndDlg, UM_ENABLEBUTTONS, wCmd, 0 );
					bProcessed = TRUE;
					break;
 
 				}
			}
			break;
		}
	}

	return bProcessed;
}

BOOL CALLBACK AboutDlgProc( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	BOOL bProcessed = FALSE;
	static HFONT hFontTitle = NULL;
	static HFONT hFontURL = NULL;
	CEdit *pCtrl = ( CEdit * )GetWindowLong( hWndDlg, GWL_USERDATA );

	switch ( uMsg )
	{
		case WM_INITDIALOG:
		{
			( ( CEdit * ) lParam )->RestoreDlgPos( hWndDlg, IDD_ABOUT );
			SetWindowLong( hWndDlg, GWL_USERDATA, lParam );
			RECT rcTitle;
			// create a nice big font for the 'CodeMax' title
			HWND hWndTitle = GetDlgItem( hWndDlg, IDC_ABOUT_TITLE );
			GetClientRect( hWndTitle, &rcTitle );
			LOGFONT lf;
			VERIFY( GetObject( ( HFONT ) SendMessage( hWndDlg, WM_GETFONT, 0, 0 ), sizeof( lf ), &lf ) );
			lf.lfHeight = rcTitle.bottom;
			lf.lfWidth = 0;
			lf.lfWeight = FW_BOLD;
			hFontTitle = CreateFontIndirect( &lf );
			SendMessage( hWndTitle, WM_SETFONT, ( WPARAM ) hFontTitle, 0 );

			// change the URL font to underline
			HWND hWndURL = GetDlgItem( hWndDlg, IDC_ABOUT_URL );
			VERIFY( GetObject( ( HFONT ) SendMessage( hWndURL, WM_GETFONT, 0, 0 ), sizeof( lf ), &lf ) );
			lf.lfUnderline = TRUE;
			hFontURL = CreateFontIndirect( &lf );
			SendMessage( hWndURL, WM_SETFONT, ( WPARAM ) hFontURL, 0 );

			// get the version info string from the VERSIONINFO resource and show it to the user
			TCHAR szVer[ 100 ];
			TCHAR szDLL[ 100 ];
			TCHAR szNum[ 20 ] = _T("UNKNOWN");
			HWND hWndVer = GetDlgItem( hWndDlg, IDC_ABOUT_VERSION );
			GetWindowText( hWndVer, szVer, ARRAY_SIZE( szVer ) );
			if ( GetModuleFileName( hInstance, szDLL, ARRAY_SIZE( szDLL ) ) )
			{
				DWORD dwHandle;
				DWORD dwSize = GetFileVersionInfoSize( szDLL, &dwHandle );
				LPVOID pVerInfo = new BYTE[ dwSize + 1 ];	// +1 in case dwSize is zero
				if ( dwSize )
				{
					VERIFY( GetFileVersionInfo( szDLL, dwHandle, dwSize, pVerInfo ) );
					LPTSTR pszVer;
					UINT unVerSize;
					if ( VerQueryValue( pVerInfo, _T("\\StringFileInfo\\040904b0\\FileVersion"), (LPVOID*)&pszVer, &unVerSize ) )
					{
						_tcscpy( szNum, pszVer );
					}
				}
				delete pVerInfo;
			}
			_tcscat( szVer, szNum );
			SetWindowText( hWndVer, szVer );
			bProcessed = TRUE;
			break;
		}

		case WM_DESTROY:
		{
			DeleteObject( hFontTitle );
			DeleteObject( hFontURL );
			break;
		}

		case WM_COMMAND:
		{
			switch ( wParam )
			{
				case MAKEWPARAM( IDOK, BN_CLICKED ):
				case MAKEWPARAM( IDCANCEL, BN_CLICKED ):
				{
					pCtrl->SaveDlgPos( hWndDlg, IDD_ABOUT );
					EndDialog( hWndDlg, IDOK );
					bProcessed = TRUE;
					break;
				}

				case MAKEWPARAM( IDC_ABOUT_URL, STN_CLICKED ):
				{
					// clicked on the URL -- go to the WinMain website
					ShellExecute( NULL, NULL, _T("http://www.winmain.com"), NULL, NULL, SW_SHOW );
					return 0;
				}
			}
			break;
		}

		case WM_CTLCOLORSTATIC:
		{
			// set the URL text to blue
			if ( ( HWND ) lParam == GetDlgItem( hWndDlg, IDC_ABOUT_URL ) )
			{
				SetTextColor( ( HDC ) wParam, RGB( 0, 0, 255 ) );
				SetBkMode( ( HDC ) wParam, TRANSPARENT );
				return ( BOOL ) GetSysColorBrush( COLOR_BTNFACE );
			}
			break;
		}

		case WM_SETCURSOR:
		{
			if ( ( HWND ) wParam == GetDlgItem( hWndDlg, IDC_ABOUT_URL ) )
			{
				SetCursor( LoadCursor( hInstance, MAKEINTRESOURCE( IDC_URLHAND ) ) );
				bProcessed = TRUE;
				SetWindowLong( hWndDlg, DWL_MSGRESULT, 1 );
				break;
			}
		}
	}

	return bProcessed;
}

void CEdit::AboutBox()
{
	DialogBoxParam( hInstance, 
	                IDD_ABOUT, 
	                GetDlgParent(),
	                ( DLGPROC ) AboutDlgProc,
					( LPARAM ) this );
	// some ActiveX containers cause the caret to go away -- put it back
	FlashCaret();
}

void KeepDlgRectFullyOnScreen( RECT &rcDlg )
{
	// force the dialog on-screen (primary monitor)
	RECT rcScreen;
	SystemParametersInfo( SPI_GETWORKAREA, 0, &rcScreen, 0 );

	int cxShift = 0, cyShift = 0;
	if ( rcDlg.left < rcScreen.left )
		cxShift = rcScreen.left - rcDlg.left;
	else if ( rcDlg.right > rcScreen.right )
		cxShift = rcScreen.right - rcDlg.right;
	if ( rcDlg.top < rcScreen.top )
		cyShift = rcScreen.top - rcDlg.top;
	else if ( rcDlg.bottom > rcScreen.bottom )
		cyShift = rcScreen.bottom - rcDlg.bottom;
	OffsetRect( &rcDlg, cxShift, cyShift );
}

// a simple UINT-to-POINT map for dialogs to save their last position.
typedef std::map<UINT, POINT> UINTPointMap;
UINTPointMap g_DlgPosMap;

void CEdit::RestoreDlgPos( HWND hWndDlg, UINT unDlg )
{
	RECT rcEdit;
	GetWindowRect( m_hWnd, &rcEdit );
	RECT rcDlg;
	GetWindowRect( hWndDlg, &rcDlg );

	UINTPointMap::iterator i = g_DlgPosMap.find( unDlg );
	if ( i != g_DlgPosMap.end() )
	{
		// restore last position
		POINT ptPos = i->second;
		SetRect( &rcDlg,
		         ptPos.x,
		         ptPos.y,
		         ptPos.x + rcDlg.right - rcDlg.left,
		         ptPos.y + rcDlg.bottom - rcDlg.top );
	}
	else
	{
		// center the dialog over editor
		OffsetRect( &rcDlg,
					( rcEdit.left + rcEdit.right - rcDlg.left - rcDlg.right ) / 2,
					( rcEdit.top + rcEdit.bottom - rcDlg.top - rcDlg.bottom ) / 2 );
	}

	KeepDlgRectFullyOnScreen( rcDlg );
	SetWindowPos( hWndDlg, NULL, rcDlg.left, rcDlg.top, -1, -1, SWP_NOZORDER | SWP_NOSIZE );
}

void CEdit::SaveDlgPos( HWND hWndDlg, UINT unDlg )
{
	RECT rcDlg;
	GetWindowRect( hWndDlg, &rcDlg );
	RECT rcEdit;
	GetWindowRect( m_hWnd, &rcEdit );
	RECT rcCenter = rcDlg;
	// center the dialog over editor
	OffsetRect( &rcCenter,
				( rcEdit.left + rcEdit.right - rcCenter.left - rcCenter.right ) / 2,
				( rcEdit.top + rcEdit.bottom - rcCenter.top - rcCenter.bottom ) / 2 );
	KeepDlgRectFullyOnScreen( rcCenter );

	// if the dialog position changed (not in center anymore),
	// remember where it's at and 
	if ( rcDlg.left != rcCenter.left ||
	     rcDlg.top != rcCenter.top ||
	     rcDlg.right != rcCenter.right ||
	     rcDlg.bottom != rcCenter.bottom )
	{
		POINT ptDlg = { rcDlg.left, rcDlg.top };
		g_DlgPosMap[ unDlg ] = ptDlg;
	}
}
