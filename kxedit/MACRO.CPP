#include "precomp.h"
#include "cedit.h"
#include "resource.h"
#include "hkeyctrl.h"

extern HINSTANCE hInstance;
LPBYTE CEdit::g_Macros[ CM_MAX_MACROS ];

void CEdit::PlayMacro( int nMacro )
{
	if ( IsRecordingMacro() )
	{
		NotifyParentOfCmdFailure( CMDERR_FAILURE );
	}
	else if ( IsPlayingMacro() )
	{
		m_bAbortMacro = TRUE;
	}
	else
	{
		SetMode( eMacroPlayback );
		m_bAbortMacro = FALSE;

		ASSERT( nMacro >= 0 && nMacro < CM_MAX_MACROS );

		m_pActiveMacro = g_Macros[ nMacro ];
		if ( m_pActiveMacro )
		{
			CDelayRepaint delay( this );
			#ifdef _DEBUG
			int nVersion = ( int ) *( m_pActiveMacro++ );
			ASSERT( nVersion >= 0 );
			BOOL bUnicode = ( BOOL ) *( m_pActiveMacro++ );
			ASSERT( bUnicode == TRUE || bUnicode == FALSE );
			#else
			m_pActiveMacro += 2;
			#endif

			int nSize = ( int ) ( *( DWORD * )m_pActiveMacro );
			m_pActiveMacro += sizeof( DWORD );

			LPBYTE pMacroEnd = m_pActiveMacro + nSize;

			while ( m_pActiveMacro < pMacroEnd )
			{
				WORD wCmd = *( WORD * )m_pActiveMacro;
				m_pActiveMacro += sizeof( WORD );
				ExecuteCommand( wCmd, 0, FALSE );
				if ( m_bAbortMacro )
				{
					MessageBox( IDS_PLAYBACK_CANCELED, IDS_MACRO_ERROR, MB_ICONHAND | MB_OK );
					break;
				}
			}

			m_pActiveMacro = NULL;
			SafeUpdateWindow();
		}
		else
		{
			NotifyParentOfCmdFailure( CMDERR_FAILURE );
		}
		SetMode( eIdle );
	}
}

void CEdit::RecordMacro( BOOL bAbortRecording )
{
	if ( IsPlayingMacro() )
	{
		NotifyParentOfCmdFailure( CMDERR_FAILURE );
	}
	else
	{
		if ( IsRecordingMacro() )
		{
			BOOL bNotifyPropsChange = FALSE;
			if ( bAbortRecording )
			{
				m_bAbortMacro = TRUE;
			}
			
			DisplayRecordMacroDialog( FALSE );
			ASSERT( m_pActiveMacro );
			ASSERT( m_pMacroRecordBuffer );
			int cbMacro = m_pActiveMacro - m_pMacroRecordBuffer; 
			int cbCmds = cbMacro - 2 - sizeof( DWORD );
			LPBYTE pMacro = NULL;
			if ( cbCmds && !m_bAbortMacro )
			{
				int nMacro;
				CM_HOTKEY cmHotKey;
				if ( PromptUserToSaveMacro( nMacro, cmHotKey ) )
				{
					ASSERT( nMacro >= 0 && nMacro < CM_MAX_MACROS );

					// embed the macro version number and unicode state
					//
					// macro version history
					// ------------------------------------
					// 0: initial version
					// 1: CMD_BEGINEDIT and CMD_ENDEDIT no longer serialized into macro
					//
					#define CM_MACRO_VER 1
					*m_pMacroRecordBuffer = CM_MACRO_VER;
					#ifdef UNICODE
					*( m_pMacroRecordBuffer + 1 ) = TRUE;
					#else
					*( m_pMacroRecordBuffer + 1 ) = FALSE;
					#endif
					*( DWORD * )( m_pMacroRecordBuffer + 2 ) = cbCmds;
					pMacro = ( LPBYTE ) malloc( cbMacro );
					memcpy( pMacro, m_pMacroRecordBuffer, cbMacro );
					if ( g_Macros[ nMacro ] )
					{
						free( g_Macros[ nMacro ] );
					}
					g_Macros[ nMacro ] = pMacro;

					VERIFY( RegisterHotKey( cmHotKey, ( WORD ) ( CMD_PLAYMACRO1 + nMacro ) ) );

					bNotifyPropsChange = TRUE;
				}
			}
			free( m_pMacroRecordBuffer );
			m_pMacroRecordBuffer = NULL;
			m_pActiveMacro = NULL;
			SetMode( eIdle );
			
			if ( bNotifyPropsChange )
			{
				// since the hotkey assignments changed, tell the parent that props changed
				NotifyParent( CMN_PROPSCHANGE );
			}
		}
		else
		{
			ASSERT( !m_pMacroRecordBuffer );
			ASSERT( !m_hDlgRecord );
			DisplayRecordMacroDialog( TRUE );
			m_pMacroRecordBuffer = ( LPBYTE ) malloc( MACRO_GROWBY );
			m_cbMacroRecordBuffer = MACRO_GROWBY;
			m_pActiveMacro = m_pMacroRecordBuffer + 2 + sizeof( DWORD );
			SetMode( eMacroRecord );
		}
		m_bAbortMacro = FALSE;
	}
}

BOOL CEdit::IsRecordingMacro() const
{
	return ( m_eMode == eMacroRecord );
}

BOOL CEdit::IsPlayingMacro() const
{
	return ( m_eMode == eMacroPlayback );
}

void CEdit::AddMacroData( LPBYTE pData, int cbData )
{
	ASSERT( m_pActiveMacro );
	ASSERT( m_pMacroRecordBuffer );
	ASSERT( pData );
	ASSERT( cbData > 0 );

	int cbCurr = m_pActiveMacro - m_pMacroRecordBuffer;
	int cbNew = cbCurr + cbData;

	if ( cbNew > m_cbMacroRecordBuffer )
	{
		ASSERT( cbData <= MACRO_GROWBY );
		// need to realloc
		m_pMacroRecordBuffer = ( LPBYTE ) realloc( m_pMacroRecordBuffer, m_cbMacroRecordBuffer + MACRO_GROWBY );
		m_cbMacroRecordBuffer += MACRO_GROWBY;
		m_pActiveMacro = m_pMacroRecordBuffer + cbCurr;
 	}

	memcpy( m_pActiveMacro, pData, cbData );
	m_pActiveMacro += cbData;
}

void CEdit::GetMacroData( LPBYTE pData, int cbData )
{
	ASSERT( m_pActiveMacro );
	ASSERT( pData );
	ASSERT( cbData > 0 );

	memcpy( pData, m_pActiveMacro, cbData );
	m_pActiveMacro += cbData;
}

BOOL CALLBACK RecordMacroDlgProc( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	CEdit *pCtrl = ( CEdit * )GetWindowLong( hWndDlg, GWL_USERDATA );

	BOOL bProcessed = FALSE;
	static BOOL bNT3x = FALSE;

	switch ( uMsg )
	{
		case WM_INITDIALOG:
		{
			SetWindowLong( hWndDlg, GWL_USERDATA, lParam );
			CEdit *pCtrl = ( CEdit * )lParam;
			ASSERT( !pCtrl->m_hbmEndRecord );
			pCtrl->m_hbmEndRecord = LoadBitmap( hInstance, MAKEINTRESOURCE( IDB_END_MACRO_RECORD ) );
			ASSERT( pCtrl->m_hbmEndRecord );
			HWND hWndOK;
			OSVERSIONINFO osver;
			osver.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
			VERIFY( GetVersionEx( &osver ) );
			
			// NT3.x does not support bitmap buttons -- switch to the text version
			if ( osver.dwPlatformId == VER_PLATFORM_WIN32_NT && osver.dwMajorVersion == 3 )
			{
				// NT 3.x
				hWndOK = GetDlgItem( hWndDlg, IDOK2 );
				ShowWindow( GetDlgItem( hWndDlg, IDOK ), SW_HIDE );
				bNT3x = TRUE;
			}
			else
			{
				// not NT3.x -- everything will work
				hWndOK = GetDlgItem( hWndDlg, IDOK );
				ShowWindow( GetDlgItem( hWndDlg, IDOK2 ), SW_HIDE );
				SendMessage( hWndOK, BM_SETIMAGE, IMAGE_BITMAP, ( LPARAM ) pCtrl->m_hbmEndRecord );
			}

			// setup the tool tip control
			HWND hwndToolTip = CreateWindow( TOOLTIPS_CLASS, 
			                                 NULL, 
											 WS_POPUP,
			                                 CW_USEDEFAULT, 
											 CW_USEDEFAULT, 
											 CW_USEDEFAULT, 
											 CW_USEDEFAULT,
			                                 hWndDlg,
											 NULL, 
											 hInstance,
											 NULL );

			ASSERT( IsWindow( hwndToolTip ) );
			TOOLINFO info = { sizeof( TOOLINFO ), TTF_SUBCLASS | TTF_IDISHWND, hWndDlg, ( UINT ) hWndOK, { 0, 0, 0, 0 }, hInstance, ( LPTSTR ) LoadStringPtr( IDS_END_RECORDING ) };
			VERIFY( SendMessage( hwndToolTip, TTM_ADDTOOL, 0, ( LPARAM ) &info ) );

			RECT rcDlg;
			GetWindowRect( hWndDlg, &rcDlg );
			RECT rcEdit;
			GetWindowRect( pCtrl->m_hWnd, &rcEdit );
			SetWindowPos( hWndDlg,
			              NULL,
						  rcEdit.right - ( rcDlg.right - rcDlg.left ) - 10, 
						  rcEdit.top + 10, 
						  -1, 
						  -1, 
						  SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE );
			SetFocus( pCtrl->m_hWnd );
			return FALSE;
		}

		case WM_DESTROY:
		{
			ASSERT( pCtrl->m_hbmEndRecord );
			if ( !bNT3x )
			{
				SendMessage( GetDlgItem( hWndDlg, IDOK2 ), BM_SETIMAGE, IMAGE_BITMAP, NULL );
			}
			VERIFY( DeleteObject( pCtrl->m_hbmEndRecord ) );
			pCtrl->m_hbmEndRecord = NULL;
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
					ASSERT( pCtrl );
					if ( wID == IDOK || wID == IDOK2 )
					{
						// end the recording process
						pCtrl->RecordMacro();
						bProcessed = TRUE;
					}
					break;
				}
			}
			break;
		}
	}

	return bProcessed;
}

void CEdit::DisplayRecordMacroDialog( BOOL bShow )
{
	if ( bShow )
	{
		m_hDlgRecord = CreateDialogParam( hInstance,
		                                  MAKEINTRESOURCE( IDD_RECORD_MACRO ),
		                                  m_hWnd,
		                                  ( DLGPROC ) RecordMacroDlgProc,
		                                  ( LPARAM )this );
	}
	else
	{
		ASSERT( IsWindow( m_hDlgRecord ) );
		DestroyWindow( m_hDlgRecord );
		m_hDlgRecord = NULL;		
	}
}

typedef struct
{
	CEdit *pCtrl;
	int nMacro;
	CM_HOTKEY cmHotKey;
} SaveMacroInfo;

BOOL CALLBACK SaveMacroDlgProc( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	SaveMacroInfo *pInfo = ( SaveMacroInfo * )GetWindowLong( hWndDlg, GWL_USERDATA );

	BOOL bProcessed = FALSE;

	switch ( uMsg )
	{
		case WM_INITDIALOG:
		{
			( ( SaveMacroInfo * ) lParam )->pCtrl->RestoreDlgPos( hWndDlg, IDD_SAVE_MACRO );
			SetWindowLong( hWndDlg, GWL_USERDATA, lParam );
			HWND hWndCombo = GetDlgItem( hWndDlg, IDC_MACROS );
			ASSERT( hWndCombo );
			HWND hWndHotKey = GetDlgItem( hWndDlg, IDC_HOTKEY );
			ASSERT( hWndHotKey );

			TCHAR szMacro[ 150 ];
			TCHAR szNum[ 5 ];
			TCHAR szHotKey[ 100 ];
			int nSel = -1;
			for ( int i = 0; i < CM_MAX_MACROS; i++ )
			{
				_itot( 1 + i, szNum, 10 );
				_tcscpy( szMacro, szNum );
				_tcscat( szMacro, _T(" : ") );
				if ( pInfo->pCtrl->g_Macros[ i ] )
				{
					int nHotKeys = CEdit::FindHotKeysForCommand( ( WORD ) ( CMD_PLAYMACRO1 + i ), NULL );
					if ( nHotKeys )
					{
						CM_HOTKEY *pHotKeys = new CM_HOTKEY[ nHotKeys ];
						VERIFY( nHotKeys == CEdit::FindHotKeysForCommand( ( WORD ) ( CMD_PLAYMACRO1 + i ), pHotKeys ) );
						GetHotKeyString( *pHotKeys, szHotKey );
						delete [] pHotKeys;
						_tcscat( szMacro, szHotKey );

					}
					else
					{
						goto unassigned;
					}
				}
				else
				{
					unassigned:
					_tcscat( szMacro, LoadStringPtr( IDS_UNASSIGNED ) );
					if ( nSel == -1 )
					{
						nSel = i;
					}
				}
				SendMessage( hWndCombo, CB_ADDSTRING, 0, ( LPARAM )szMacro );
			}
			nSel = ( ( nSel == -1 ) ? 0 : nSel );
			SendMessage( hWndCombo, CB_SETCURSEL, nSel, 0 );
			SendMessage( hWndHotKey, HOTM_SETHOTKEY, 0, 0 );
			SendMessage( hWndDlg, WM_COMMAND, MAKEWPARAM( IDC_HOTKEY, EN_CHANGE ), 0 );

			return TRUE;
		}

		case WM_COMMAND:
		{
			WORD wID = LOWORD(wParam);  

			switch ( HIWORD( wParam ) )
			{
				case BN_CLICKED:
				{
					if ( !SendMessage( GetDlgItem( hWndDlg, IDC_HOTKEY ), HOTM_QUERYEATCOMMAND, 0, 0 ) )
					{
						ASSERT( pInfo );
						switch ( wID )
						{
							case IDOK:
							{
								pInfo->nMacro = SendMessage( GetDlgItem( hWndDlg, IDC_MACROS ), CB_GETCURSEL, 0, 0 );
								ASSERT( pInfo->nMacro != CB_ERR );
								SendMessage( GetDlgItem( hWndDlg, IDC_HOTKEY ), HOTM_GETHOTKEY, 0, ( LPARAM ) &pInfo->cmHotKey );
								BOOL bBadHotKey = FALSE;
								int nHotKeys = CEdit::FindHotKeysForCommand( CMD_RECORDMACRO, NULL );
								if ( nHotKeys )
								{
									CM_HOTKEY *pHotKeys = new CM_HOTKEY[ nHotKeys ];
									VERIFY( nHotKeys == CEdit::FindHotKeysForCommand( CMD_RECORDMACRO, pHotKeys ) );
									for ( int i = 0; i < nHotKeys; i++ )
									{
										if ( pHotKeys[ i ] == pInfo->cmHotKey )
										{
											pInfo->pCtrl->MessageBox( hWndDlg, LoadStringPtr( IDS_ERR_HOTKEY_RESERVED ), LoadStringPtr( IDS_ERROR ), MB_OK | MB_ICONHAND );
											bBadHotKey = TRUE;
											break;
										}
									}
									delete [] pHotKeys;

								}
								if ( !bBadHotKey )
								{
									// If reassigning a keystroke that once was assigned to a macro,
									// ask the user if can delete the macro.
									int nDontCare;
									WORD wCmd;
									if ( pInfo->pCtrl->LookupHotKey( pInfo->cmHotKey, wCmd, nDontCare ) &&
									     wCmd >= CMD_PLAYMACRO1 &&
										 ( wCmd < ( CMD_PLAYMACRO1 + CM_MAX_MACROS ) ) &&
										 ( ( wCmd - CMD_PLAYMACRO1 ) != pInfo->nMacro ) &&
										 pInfo->pCtrl->g_Macros[ pInfo->nMacro ] )
									{
										if ( pInfo->pCtrl->MessageBox( hWndDlg, LoadStringPtr( IDS_DELETE_DUPE_MACRO ), LoadStringPtr( IDS_SAVE_MACRO ), MB_YESNO | MB_ICONQUESTION ) == IDYES )
										{
											pInfo->pCtrl->SetMacro( wCmd - CMD_PLAYMACRO1, NULL );
										}
									}

									pInfo->pCtrl->SaveDlgPos( hWndDlg, IDD_SAVE_MACRO );
									EndDialog( hWndDlg, IDOK );
								}
								bProcessed = TRUE;
								break;
							}
							
							case IDCANCEL:
							{
								pInfo->pCtrl->SaveDlgPos( hWndDlg, IDD_SAVE_MACRO );
								EndDialog( hWndDlg, IDCANCEL );
								bProcessed = TRUE;
								break;
							}
						}
					}
					break;
				}
				case EN_CHANGE:
				{
					if ( wID == IDC_HOTKEY )
					{
						CM_HOTKEY cmHotKey;
						SendMessage( GetDlgItem( hWndDlg, IDC_HOTKEY ), HOTM_GETHOTKEY, 0, ( LPARAM ) &cmHotKey );
						EnableWindow( GetDlgItem( hWndDlg, IDOK ), cmHotKey.nVirtKey1 != 0 );

						TCHAR szHotKey[ 250 ];
						_tcscpy( szHotKey, LoadStringPtr( IDS_CURRENTLY_ASSIGNED_TO ) );
						int nDontCare;
						WORD wCmd;
						if ( pInfo->pCtrl->LookupHotKey( cmHotKey, wCmd, nDontCare ) )
						{
							TCHAR szCmd[ 50 ];
							szCmd[ 0 ] = _T('\'');
							szCmd[ 1 ] = _T('\0');
							CEdit::GetCommandString( wCmd, FALSE, szCmd + 1, ARRAY_SIZE( szCmd ) - 1 );
							_tcscat( szHotKey, szCmd );
							_tcscat( szHotKey, _T("'") );
						}
						else
						{
							_tcscat( szHotKey, LoadStringPtr( IDS_UNASSIGNED ) );
						}

						SetDlgItemText(	hWndDlg, IDC_HOTKEY_ASSIGNMENT, szHotKey );
						return 0;
					}
					break;
				}
			}
			break;
		}
	}

	return bProcessed;
}

BOOL CEdit::PromptUserToSaveMacro( int &nMacro, CM_HOTKEY &cmHotKey )
{
	CNoHideSel disable( this );  // don't honor hidesel behavior here
	SaveMacroInfo info;
	info.pCtrl = this;
	BOOL bOK = IDOK == DialogBoxParam( hInstance, 
                                       IDD_SAVE_MACRO, 
	                                   GetDlgParent(),
	                                   ( DLGPROC ) SaveMacroDlgProc,
	                                   ( LPARAM )&info );

	if ( bOK )
	{
		nMacro = info.nMacro;
		cmHotKey = info.cmHotKey;
	}

	// some ActiveX containers cause the caret to go away -- put it back
	FlashCaret();

	return bOK;
}

int CEdit::GetMacro( int nMacro, LPBYTE pMacroBuff )
{
	int cbMacro = 0;
	ASSERT( nMacro < CM_MAX_MACROS )
	{
		LPBYTE pMacro = g_Macros[ nMacro ];

		if ( pMacro )
		{
			cbMacro = MACRO_SIZE( pMacro );
			if ( pMacroBuff )
			{
				memcpy( ( LPBYTE ) pMacroBuff, pMacro, cbMacro );
			}
		}
	}

	return cbMacro;
}

int CEdit::SetMacro( int nMacro, const LPBYTE pMacroBuff )
{
	int cbMacro = 0;
	if ( nMacro < CM_MAX_MACROS )
	{
		LPBYTE pMacro = g_Macros[ nMacro ];
		if ( pMacro )
		{
			free( pMacro );
			pMacro = NULL;
			g_Macros[ nMacro ] = NULL;
		}

		if ( pMacroBuff )
		{
			// only accept macros that have the same unicode setting as this control
			#ifdef UNICODE
			if ( pMacroBuff[ 1 ] == 1 )	// macro is unicode-encoded
			#else
			if ( pMacroBuff[ 1 ] == 0 )	// macro is ansi-encoded
			#endif
			{
				cbMacro = MACRO_SIZE( pMacroBuff );
				LPBYTE pMacroCopy = ( LPBYTE ) malloc( cbMacro );
				memcpy( pMacroCopy, pMacroBuff, cbMacro );
				g_Macros[ nMacro ] = pMacroCopy;
			}
		}
	}

	return cbMacro;
}
