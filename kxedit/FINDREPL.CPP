#include "precomp.h"
#include "resource.h"
#include "editdlgs.h"
#include "cedit.h"

extern HINSTANCE hInstance;


CFindReplaceData::CFindReplaceData()
{
	m_bWholeWordOnly = DEF_WHOLEWORDONLY;
	m_bRegExp = DEF_REGEXP;
	m_bCaseSensitiveSearch = DEF_CASESENSITIVESEARCH;
	m_bPreserveCase = DEF_PRESERVECASE;
	m_pszFindText = NULL;
	m_pszReplaceText = NULL;
	m_pszFindMRUList = NULL;
	m_pszReplaceMRUList = NULL;
}

CFindReplaceData::~CFindReplaceData()
{
	if ( m_pszFindText )
	{
		delete m_pszFindText;
	}
	if ( m_pszReplaceText )
	{
		delete m_pszReplaceText;
	}
	if ( m_pszFindMRUList )
	{
		free( m_pszFindMRUList );
	}
	if ( m_pszReplaceMRUList )
	{
		free( m_pszReplaceMRUList );
	}
}

void CEdit::FindReplaceSelection()
{
	if ( g_FindReplaceData.m_bPreserveCase )
	{
		///////////////////////////////////////
		//
		// Format the replace text such that it follows the same
		// case pattern of the find text:
		//
		// a) All lower
		// b) All upper
		// c) First n chars all upper
		//
		//
		HGLOBAL hMem = NULL;
		// Selection should be normalized by caller!
		ASSERT( m_Selection.GetStartRow() <= m_Selection.GetEndRow() );
		ASSERT( m_Selection.GetStartRow() < m_Selection.GetEndRow() || m_Selection.GetStartCol() < m_Selection.GetEndCol() );
		VERIFY( m_Buffer.GetText( m_Selection.GetStartRow(), m_Selection.GetStartCol(), m_Selection.GetEndRow(), m_Selection.GetEndCol(), hMem, FALSE, FALSE ) );
		ASSERT( hMem );
		LPCTSTR pszFind = ( LPCTSTR ) GlobalLock( hMem );
		BOOL bUpper = TRUE;
		BOOL bLower = TRUE;
		int nFirstUpper = 0;

		LPCTSTR psz = pszFind;
		while ( *psz )
		{
			TCHAR chUpper = g_UpperConv[ ( BYTE ) *psz ];
			bUpper &= ( chUpper == *psz );
			bLower &= ( chUpper != *psz );
			if ( bUpper )
			{
				nFirstUpper++;
			}
			psz++;
		}
		
		LPTSTR pszOut = new TCHAR[ _tcslen( g_FindReplaceData.m_pszReplaceText ) + 1 ];
		_tcscpy( pszOut, g_FindReplaceData.m_pszReplaceText );

		if ( bUpper )
		{
			CharUpper( pszOut );
		}
		else if ( bLower )
		{
			CharLower( pszOut );
		}
		else if ( nFirstUpper )
		{
			CharLower( pszOut );
			LPTSTR psz = pszOut;
			while ( nFirstUpper-- && *psz )
			{
				*psz = g_UpperConv[ ( BYTE ) *psz ];
				psz++;
			}
		}

		ReplaceSelection( pszOut, FALSE );

		delete pszOut;
		GlobalUnlock( hMem );
		GlobalFree( hMem );

	}
	else
	{
		ReplaceSelection( g_FindReplaceData.m_pszReplaceText, FALSE );
	}
}


///////////////////////////////////////////////////////////////////////////////
//
//  Search
//
//
void CEdit::FindReplace()
{
	if ( m_Buffer.GetLineCount() )
	{
		if ( !IsPlayingMacro() )
		{
			DialogBoxParam( hInstance, 
							IDD_REPLACE, 
							GetDlgParent(),
							( DLGPROC ) ReplaceDlgProc,
							( LPARAM )this );
			// some ActiveX containers cause the caret to go away -- put it back
			FlashCaret();
		}
	}
	else
	{
		NotifyParentOfCmdFailure( CMDERR_EMPTYBUF );
	}
}

void CEdit::Replace()
{
	if ( m_Buffer.GetLineCount() && !m_Selection.IsEmpty() )
	{
		if ( g_FindReplaceData.m_pszFindText )
		{
			if ( g_FindReplaceData.m_pszReplaceText )
			{
				FindReplaceSelection();
			}
			else
			{
				DeleteSelection( FALSE, FALSE );
			}
		}
	}
	FindText( TRUE );
}

void CEdit::ReplaceAllInBuffer()
{
	CHourGlass wait( this );
	CDelayRepaint delay( this );
	if ( m_Buffer.GetLineCount() && g_FindReplaceData.m_pszFindText && *g_FindReplaceData.m_pszFindText )
	{
		m_Selection.MakeEmpty();
		int nStartRow = m_Selection.GetStartRow();
		int nStartCol = m_Selection.GetStartCol();
		m_Buffer.BeginEdit( nStartRow, nStartCol );
		BOOL bFoundText = FALSE;
		int nFirstRow = 0, nFirstCol = 0;
		int nCol = 0;
		int nRow = 0;
		int cbFindText = _tcslen( g_FindReplaceData.m_pszFindText );
		int cbReplaceText = _tcslen( g_FindReplaceData.m_pszReplaceText );

		for ( ;; )
		{
			BOOL bWrapped;
			int cbMatched;
			if ( m_Buffer.FindText( g_FindReplaceData.m_pszFindText, cbFindText, TRUE, g_FindReplaceData.m_bCaseSensitiveSearch, g_FindReplaceData.m_bWholeWordOnly, g_FindReplaceData.m_bRegExp, nRow, nCol, bWrapped, cbMatched ) )
			{
				if ( !bFoundText )
				{
					bFoundText = TRUE;
					nFirstRow = nRow;
					nFirstCol = nCol;
				}
				else if ( bWrapped && ( nRow >= nFirstRow ) && ( nCol >= nFirstCol ) )
				{
					// have completely wrapped around and came back to the first occurrence.
					break;
				}
				m_Selection.SetExtendedSelection( nCol, nRow, nCol + cbMatched, nRow );
				if ( g_FindReplaceData.m_pszReplaceText )
				{
					FindReplaceSelection();
					nCol += cbReplaceText;
				}
				else
				{
					DeleteSelection( FALSE, FALSE );
					nCol += cbMatched;
				}
			}
			else
			{
				break;
			}
		}
		if ( bFoundText && !m_pActiveView->LineIsVisible( nFirstRow ) )
		{
			// found occurrences of the find text.  Go to the location of the first bookmark
			GoToLine( nFirstRow, TRUE );
		}
		Repaint( FALSE );
		m_Buffer.EndEdit( nStartRow, nStartCol );
	}
}

void CEdit::ReplaceAllInSelection()
{
	CHourGlass wait( this );
	CDelayRepaint delay( this );
	if ( m_Buffer.GetLineCount() && g_FindReplaceData.m_pszFindText && *g_FindReplaceData.m_pszFindText )
	{
		int nStartCol, nStartRow, nEndCol, nEndRow;
		m_Selection.GetNormalizedBufferSelection( nStartCol, nStartRow, nEndCol, nEndRow );
		m_Buffer.BeginEdit( nStartRow, nStartCol );
		BOOL bFoundText = FALSE;
		int nFirstRow = 0, nFirstCol = 0;
		int nCol = nStartCol;
		int nRow = nStartRow;
		int cbFindText = _tcslen( g_FindReplaceData.m_pszFindText );
		int cbReplaceText = _tcslen( g_FindReplaceData.m_pszReplaceText );

		for ( ;; )
		{
			BOOL bWrapped;
			int cbMatched;
			if ( m_Buffer.FindText( g_FindReplaceData.m_pszFindText, cbFindText, TRUE, g_FindReplaceData.m_bCaseSensitiveSearch, g_FindReplaceData.m_bWholeWordOnly, g_FindReplaceData.m_bRegExp, nRow, nCol, bWrapped, cbMatched ) )
			{
				if ( !bFoundText )
				{
					bFoundText = TRUE;
					nFirstRow = nRow;
					nFirstCol = nCol;
				}
				else
				{
					BOOL bOutsideSel = ( nRow < nStartRow ) || 
					                   ( nRow == nStartRow && nCol < nStartCol ) ||
									   ( nRow > nEndRow ) ||
									   ( nRow == nEndRow && nCol >= nEndCol );
					if ( bOutsideSel || ( bWrapped && ( nRow >= nFirstRow ) && ( nCol >= nFirstCol ) ) )
					{
						// no longer in selection, or wrapped around to the start -- done!
						break;
					}
				}
				m_Selection.SetExtendedSelection( nCol, nRow, nCol + cbMatched, nRow );
				if ( g_FindReplaceData.m_pszReplaceText )
				{
					FindReplaceSelection();
					nCol += cbReplaceText;
				}
				else
				{
					DeleteSelection( FALSE, FALSE );
					nCol += cbMatched;
				}
			}
			else
			{
				break;
			}
		}
		if ( bFoundText && !m_pActiveView->LineIsVisible( nFirstRow ) )
		{
			// found occurrences of the find text.  Go to the location of the first bookmark
			GoToLine( nFirstRow, TRUE );
		}
		Repaint( FALSE );
		m_Buffer.EndEdit( nStartRow, nStartCol );
	}
}

void CEdit::FindPrevWord()
{
	FindSelectedWord( FALSE );
}

void CEdit::FindPrev()
{
	FindText( FALSE );
}

void CEdit::FindNextWord()
{
	FindSelectedWord( TRUE );
}

void CEdit::FindSelectedWord( BOOL bForward )
{
	if ( m_Selection.IsEmpty() )
	{
		m_Selection.Extend( CSelection::eOutward, CSelection::eWord, FALSE, FALSE, FALSE );
	}

	if ( !m_Selection.IsEmpty() )
	{
		HGLOBAL hMem = GetSelectionText();
		if ( hMem )
		{
			LPTSTR pszText = ( LPTSTR ) GlobalLock( hMem );
			if ( pszText && *pszText )
			{
				SetFindText( pszText );
				FindText( bForward );
			}
			GlobalUnlock( hMem );
			GlobalFree( hMem );
		}
	}
	else
	{
		NotifyParentOfCmdFailure( CMDERR_EMPTYBUF );
	}

}

void CEdit::FindNext()
{
	FindText( TRUE );
}

void CEdit::FindText( BOOL bForward )
{
	if ( g_FindReplaceData.m_pszFindText )
	{
		int nRow = bForward ? m_Selection.GetEndRow() : m_Selection.GetStartRow();
		int nRowSave = nRow;
		int nCol = bForward ? m_Selection.GetEndCol() : m_Selection.GetStartCol();
		int cbWord = _tcslen( g_FindReplaceData.m_pszFindText );

		BOOL bWrapped;
		int cbMatched;
		if ( m_Buffer.FindText( g_FindReplaceData.m_pszFindText, cbWord, bForward, g_FindReplaceData.m_bCaseSensitiveSearch, g_FindReplaceData.m_bWholeWordOnly, g_FindReplaceData.m_bRegExp, nRow, nCol, bWrapped, cbMatched ) )
		{
			if ( nRow != nRowSave && !m_pActiveView->LineIsVisible( nRow ) )
			{
				GoToLine( nRow, TRUE );
			}
			m_Selection.SetExtendedSelection( nCol, nRow, nCol + cbMatched, nRow, FALSE );
			m_Selection.EnsureVisible( TRUE, TRUE ); // center the selection
		}
		else
		{
			NotifyParentOfCmdFailure( CMDERR_NOTFOUND );
			m_Selection.MakeEmpty( TRUE, bForward );
			if ( IsPlayingMacro() )
			{
				m_bAbortMacro = TRUE;
			}
		}
	}
	else
	{
		m_Selection.EnsureVisible( TRUE, TRUE ); // center the selection
	}
}

void CEdit::Find()
{
	if ( m_Buffer.GetLineCount() )
	{
		if ( !IsPlayingMacro() )
		{
			DialogBoxParam( hInstance, 
							IDD_FIND, 
							GetDlgParent(),
							( DLGPROC ) FindDlgProc,
							( LPARAM )this );
			// some ActiveX containers cause the caret to go away -- put it back
			FlashCaret();
		}
	}
	else
	{
		NotifyParentOfCmdFailure( CMDERR_EMPTYBUF );
	}
}

void CEdit::FindMarkAll()
{
	CHourGlass wait( this );
	m_Selection.MakeEmpty();
	if ( m_Buffer.GetLineCount() )
	{
		if ( g_FindReplaceData.m_pszFindText && *g_FindReplaceData.m_pszFindText )
		{
			BOOL bFirstBookmark = TRUE;
			int nFirstRow = 0, nFirstCol = 0;
			int nCol = 0;
			int nRow = 0;
			int cbFindText = _tcslen( g_FindReplaceData.m_pszFindText );

			for ( ;; )
			{
				BOOL bWrapped;
				int cbMatched;
				if ( m_Buffer.FindText( g_FindReplaceData.m_pszFindText, cbFindText, TRUE, g_FindReplaceData.m_bCaseSensitiveSearch, g_FindReplaceData.m_bWholeWordOnly, g_FindReplaceData.m_bRegExp, nRow, nCol, bWrapped, cbMatched ) )
				{
					if ( bFirstBookmark )
					{
						bFirstBookmark = FALSE;
						nFirstRow = nRow;
						nFirstCol = nCol;
					}
					else if ( bWrapped && ( nRow >= nFirstRow ) && ( nCol >= nFirstCol ) )
					{
						// have completely wrapped around and came back to the first occurrence.
						break;
					}

					m_Buffer.SetBookmark( nRow );
					nCol += cbMatched;
				}
				else
				{
					NotifyParentOfCmdFailure( CMDERR_NOTFOUND );
					break;
				}
			}
			if ( !bFirstBookmark && !m_pActiveView->LineIsVisible( nFirstRow ) )
			{
				// found occurrences of the find text.  Go to the location of the first bookmark
				GoToLine( nFirstRow, TRUE );
			}
			Repaint( FALSE );
		}
		else
		{
			NotifyParentOfCmdFailure( CMDERR_INPUT );
		}
	}
	else
	{
		NotifyParentOfCmdFailure( CMDERR_EMPTYBUF );
	}
}

void CEdit::ToggleWholeWord()
{
	BOOL bState;
	if ( IsPlayingMacro() )
	{
		GetMacroData( ( LPBYTE ) &bState, sizeof( bState ) );
	}
	else
	{
		bState = !g_FindReplaceData.m_bWholeWordOnly;
		if ( IsRecordingMacro() )
		{
			AddMacroData( ( LPBYTE ) &bState, sizeof( bState ) );
		}
	}
	g_FindReplaceData.m_bWholeWordOnly = bState;
}

void CEdit::WholeWordOn()
{
	g_FindReplaceData.m_bWholeWordOnly = TRUE;
}

void CEdit::WholeWordOff()
{
	g_FindReplaceData.m_bWholeWordOnly = FALSE;
}

void CEdit::ToggleRegExp()
{
	BOOL bState;
	if ( IsPlayingMacro() )
	{
		GetMacroData( ( LPBYTE ) &bState, sizeof( bState ) );
	}
	else
	{
		bState = !g_FindReplaceData.m_bRegExp;
		if ( IsRecordingMacro() )
		{
			AddMacroData( ( LPBYTE ) &bState, sizeof( bState ) );
		}
	}
	g_FindReplaceData.m_bRegExp = bState;
}

void CEdit::RegExpOn()
{
	g_FindReplaceData.m_bRegExp = TRUE;
}

void CEdit::RegExpOff()
{
	g_FindReplaceData.m_bRegExp = FALSE;
}

void CEdit::TogglePreserveCase()
{
	BOOL bState;
	if ( IsPlayingMacro() )
	{
		GetMacroData( ( LPBYTE ) &bState, sizeof( bState ) );
	}
	else
	{
		bState = !g_FindReplaceData.m_bPreserveCase;
		if ( IsRecordingMacro() )
		{
			AddMacroData( ( LPBYTE ) &bState, sizeof( bState ) );
		}
	}
	g_FindReplaceData.m_bPreserveCase = bState;
}

void CEdit::PreserveCaseOn()
{
	g_FindReplaceData.m_bPreserveCase = TRUE;
}

void CEdit::PreserveCaseOff()
{
	g_FindReplaceData.m_bPreserveCase = FALSE;
}

void CEdit::ToggleCaseSensitive()
{
	BOOL bState;
	if ( IsPlayingMacro() )
	{
		GetMacroData( ( LPBYTE ) &bState, sizeof( bState ) );
	}
	else
	{
		bState = !g_FindReplaceData.m_bCaseSensitiveSearch;
		if ( IsRecordingMacro() )
		{
			AddMacroData( ( LPBYTE ) &bState, sizeof( bState ) );
		}
	}
	g_FindReplaceData.m_bCaseSensitiveSearch = bState;
}

void CEdit::CaseSensitiveOn()
{
	g_FindReplaceData.m_bCaseSensitiveSearch = TRUE;
}

void CEdit::CaseSensitiveOff()
{
	g_FindReplaceData.m_bCaseSensitiveSearch = FALSE;
}

void CEdit::SetFindText( LPCTSTR psz )
{
	TCHAR szText[ CM_MAX_FINDREPL_TEXT + 1 ];
	if ( IsRecordingMacro() )
	{
		WORD wLen = ( WORD ) ( psz ? _tcslen( psz ) : 0 );
		AddMacroData( ( LPBYTE ) &wLen, sizeof( wLen ) );
		if ( wLen )
		{
			AddMacroData( ( LPBYTE ) psz, wLen * sizeof( TCHAR ) );
		}
	}
	else if ( IsPlayingMacro() )
	{
		WORD wLen;
		GetMacroData( ( LPBYTE ) &wLen, sizeof( wLen ) );
		if ( wLen )
		{
			GetMacroData( ( LPBYTE ) szText, wLen  * sizeof( TCHAR ) );
			szText[ wLen ] = _T('\0');
			psz = szText;
		}
		else
		{
			psz = NULL;
		}
	}

	if ( g_FindReplaceData.m_pszFindText )
	{
		delete g_FindReplaceData.m_pszFindText;
	}

	if ( psz )
	{
		int cbText = _tcslen( psz );
		cbText = min( cbText, CM_MAX_FINDREPL_TEXT );
		g_FindReplaceData.m_pszFindText = new TCHAR[ cbText + 1 ];
		g_FindReplaceData.m_pszFindText[ cbText ] = _T('\0');
		_tcsncpy( g_FindReplaceData.m_pszFindText, psz, cbText );
	}
	else
	{
		g_FindReplaceData.m_pszFindText = NULL;
	}
}

void CEdit::SetReplaceText( LPCTSTR psz )
{
	TCHAR szText[ CM_MAX_FINDREPL_TEXT + 1 ];
	if ( IsRecordingMacro() )
	{
		WORD wLen = ( WORD ) ( psz ? _tcslen( psz ) : 0 );
		AddMacroData( ( LPBYTE ) &wLen, sizeof( wLen ) );
		if ( wLen )
		{
			AddMacroData( ( LPBYTE ) psz, wLen  * sizeof( TCHAR ) );
		}
	}
	else if ( IsPlayingMacro() )
	{
		WORD wLen;
		GetMacroData( ( LPBYTE ) &wLen, sizeof( wLen ) );
		if ( wLen )
		{
			GetMacroData( ( LPBYTE ) szText, wLen * sizeof( TCHAR ) );
			szText[ wLen ] = _T('\0');
			psz = szText;
		}
		else
		{
			psz = NULL;
		}
	}

	if ( g_FindReplaceData.m_pszReplaceText )
	{
		delete g_FindReplaceData.m_pszReplaceText;
	}

	if ( psz )
	{
		int cbText = _tcslen( psz );
		cbText = min( cbText, CM_MAX_FINDREPL_TEXT );
		g_FindReplaceData.m_pszReplaceText = new TCHAR[ cbText + 1 ];
		g_FindReplaceData.m_pszReplaceText[ cbText ] = _T('\0');
		_tcsncpy( g_FindReplaceData.m_pszReplaceText, psz, cbText );
	}
	else
	{
		g_FindReplaceData.m_pszReplaceText = NULL;
	}
}

