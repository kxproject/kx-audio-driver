#include "precomp.h"
#include <time.h>
#include <math.h>
#include "cedit.h"
#include "resource.h"

extern HINSTANCE hInstance;

typedef struct {
	CEdit *pEdit;
	LPBOOL pbAbort;
	} AbortInfo;

// Abort dialog proc for the 'Cancel Printing' dialog
BOOL CALLBACK AbortDlgProc( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	AbortInfo *pInfo = ( AbortInfo * )GetWindowLong( hWndDlg, GWL_USERDATA );
	switch ( uMsg )
	{
		case WM_INITDIALOG:
		{
			ASSERT( lParam );
			( ( AbortInfo * ) lParam )->pEdit->RestoreDlgPos( hWndDlg, IDD_ABORTPRINT );
			SetWindowLong( hWndDlg, GWL_USERDATA, lParam );
			return TRUE;
		}

		case WM_COMMAND:
		{
			if ( LOWORD( wParam ) == IDCANCEL && HIWORD( wParam ) == BN_CLICKED )
			{
				*pInfo->pbAbort = TRUE;
			}
			break;
		}
	}

	return 0;
}

BOOL CEdit::Print( HDC hDC, DWORD dwFlags ) const
{
	BOOL bResult = TRUE;
	BOOL bPrintSel = HAS_FLAG( dwFlags, CM_PRINT_SELECTION );
	HGLOBAL hDevMode = NULL;
	HGLOBAL hDevNames = NULL;

	if ( !HAS_FLAG( dwFlags, CM_PRINT_HDC ) )
	{
		PRINTDLG pd = {
			sizeof( PRINTDLG ),
			m_hWnd,
			NULL, NULL, NULL,
			PD_RETURNDC |
			( HAS_FLAG( dwFlags, CM_PRINT_DEFAULTPRN ) ? PD_RETURNDEFAULT : 0 ) |
			( ( bPrintSel || !m_Selection.IsEmpty() ) ? PD_SELECTION : 0 ),
			0, 0, 0, 0, 0,
			hInstance,
			0,
			NULL, NULL, NULL, NULL, NULL, NULL
		};

		CNoHideSel disable( ( CEdit * ) this );  // don't honor hidesel behavior here

		if ( !PrintDlg( &pd ) )
		{
			bResult = FALSE;
		}
		else
		{
			hDC = pd.hDC;
			hDevMode = pd.hDevMode;
			hDevNames = pd.hDevNames;
			bPrintSel = HAS_FLAG( pd.Flags, PD_SELECTION );
		}
	}

	if ( !hDC )
		bResult = FALSE;
	
	if ( !bResult )
		goto cleanup;

	{
	// at this point, we have a printer DC and can start printing

	TCHAR szTitle[ _MAX_PATH + 1 ];
	_tcscpy( szTitle, LoadStringPtr( IDS_PRINT_JOB_TITLE ) );
	DOCINFO di = {
		sizeof( DOCINFO ),
		szTitle,
		NULL,
		NULL,
		0
	};

	// use an 11 point font
	int cyLine = MulDiv( 11, GetDeviceCaps( hDC, LOGPIXELSY ), 72 );
	LOGFONT lf;
	VERIFY ( GetObject( m_font->hFont, sizeof( lf ), &lf ) );
	lf.lfHeight = -cyLine;
	lf.lfWidth = 0;

	// generate the variations of the base font
	FONT_DATA Fonts[ NUM_FONT_VARIATIONS ];
	int cxChar;
	GenerateFontVariations( hDC, lf, Fonts, cxChar );
	
	// if not using rich fonts, there is no point adding extra space to compensate
	// for fonts (bold, italics, etc.) that aren't going to be used.
	if ( !HAS_FLAG( dwFlags, CM_PRINT_RICHFONTS ) )
	{
		cxChar -= Fonts->cxExtraSpacing;
	}

 	// if caller wants a double thin border, make sure the correct style is set
	if ( HAS_FLAG( dwFlags, CM_PRINT_BORDERDOUBLE ) && !HAS_FLAG( dwFlags, CM_PRINT_BORDERTHICK ) )
		dwFlags |= CM_PRINT_BORDERTHIN;

	// if printer does not support color, ignore what the caller asked for
	if ( GetDeviceCaps( hDC, NUMCOLORS ) <= 2 )
		dwFlags &= ~CM_PRINT_COLOR;

	int nStartLine, nStartCol, nEndLine, nEndCol;
	if ( bPrintSel )
		m_Selection.GetNormalizedBufferSelection( nStartCol, nStartLine, nEndCol, nEndLine );
	else
		{
		nStartCol = nStartLine = nEndCol = 0;
		nEndLine = m_Buffer.GetLineCount() - 1;
		if ( nEndLine >= 0 )
			nEndCol = m_Buffer.GetLineLength( nEndLine );
		}

	// show an abort dialog if printing more than 10 pages
	BOOL bAbort = FALSE;
	HWND hWndAbort = NULL;
	int nLineCount = nEndLine - nStartLine + 1;
	int cyPaper = GetDeviceCaps( hDC, VERTRES );
	AbortInfo ai = { ( CEdit * ) this, &bAbort };
	if ( ( ( nLineCount * cyLine ) / cyPaper ) > 10 )
	{
		hWndAbort = CreateDialogParam( hInstance, MAKEINTRESOURCE( IDD_ABORTPRINT ), GetDlgParent(), ( DLGPROC ) AbortDlgProc, ( LPARAM ) &ai );
		ShowWindow( hWndAbort, SW_SHOW );
	}

	VERIFY( StartDoc( hDC, &di ) > 0 );

	int nLine = nStartLine;

	// cache the datetime text now so it won't change
	TCHAR szDateTime[ 256 ] = {0};
	SYSTEMTIME time;
	GetSystemTime(&time);
	GetDateFormat(GetSystemDefaultLCID(), 0, &time, NULL, szDateTime, ARRAY_SIZE(szDateTime));
	_tcscat( szDateTime, _T("  ") );
	int cchDateTime = _tcslen( szDateTime );
	GetTimeFormat(GetSystemDefaultLCID(), 0, &time, NULL, szDateTime + cchDateTime, ARRAY_SIZE(szDateTime) - cchDateTime);

	// select in a font now and restore the previous later so that
	// the printer functions don't have to select/deselect all the time.
	HFONT hFontLast = ( HFONT ) SelectObject( hDC, GetStockObject( SYSTEM_FONT ) );
	BOOL bContinue = TRUE;
	int nPage = 0;
	while ( bContinue && !bAbort )
	{
		VERIFY( StartPage( hDC ) > 0 );
		RECT rcText;
		PrintPageBorder( hDC, dwFlags, Fonts, rcText, ++nPage, nLineCount, szDateTime, cyLine );
		int cxLine = rcText.right - rcText.left;
		int cy = rcText.top;
		while ( cy + cyLine < rcText.bottom )
		{
			if ( nLine > nEndLine )
			{
				// end of page
				bContinue = FALSE;
				break;
			}
			else
			{
				// print a line
				int nLineStartCol = 0, nLineEndCol = m_Buffer.GetLineLength( nLine );
				if ( nLine == nStartLine )
					nLineStartCol = nStartCol;
				if ( nLine == nEndLine )
					nLineEndCol = nEndCol;
				PrintOneLine( hDC, dwFlags, nLine, rcText.left, cy, Fonts, cxChar, cxLine, nLineStartCol, nLineEndCol );
				cy += cyLine;
				nLine++;
			}
		}
		VERIFY( EndPage( hDC ) > 0 );

		// flush the message queue for the abort dlg so it can process the Cancel button
		MSG msg;
		while ( PeekMessage( &msg, hWndAbort, NULL, NULL, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}
	SelectObject( hDC, hFontLast );
	
	if ( bAbort )
	{
		VERIFY( AbortDoc( hDC ) > 0 );
	}
	else
	{
		VERIFY( EndDoc( hDC ) > 0 );
	}

	for ( int i = 0; i < NUM_FONT_VARIATIONS; i++ )
		DeleteObject( Fonts[ i ].hFont );

	if ( hWndAbort )
		DestroyWindow( hWndAbort );
	}
cleanup:

	if ( hDC )
		DeleteDC( hDC );
	if ( hDevMode )
		GlobalFree( hDevMode );
	if ( hDevNames )
		GlobalFree( hDevNames );

	// always return TRUE -- hitting the cancel button is not really
	// an error.
	return TRUE;
}

void CEdit::PrintPageBorder( HDC hDC, DWORD dwFlags, FONT_DATA *pFonts, RECT &rcText, int nPage, int nLineCount, LPCTSTR pszDateTime, int cyLine ) const
{
	// calc the various margin area aspect sizes
	BOOL bBorderThick = HAS_FLAG( dwFlags, CM_PRINT_BORDERTHICK );
	BOOL bBorderThin = HAS_FLAG( dwFlags, CM_PRINT_BORDERTHIN );
	BOOL bBorderDouble = HAS_FLAG( dwFlags, CM_PRINT_BORDERDOUBLE );
	BOOL bDateTime = HAS_FLAG( dwFlags, CM_PRINT_DATETIME );

	// start with the printable area of the paper
	SetRect( &rcText,
	         GetDeviceCaps( hDC, PHYSICALOFFSETX ) + 1,
	         GetDeviceCaps( hDC, PHYSICALOFFSETY ) + 1,
	         GetDeviceCaps( hDC, HORZRES ) - GetDeviceCaps( hDC, PHYSICALOFFSETX ) - 1,
	         GetDeviceCaps( hDC, VERTRES ) - GetDeviceCaps( hDC, PHYSICALOFFSETY ) - 1 );

	int cxInch = GetDeviceCaps( hDC, LOGPIXELSX );
	int cyInch = GetDeviceCaps( hDC, LOGPIXELSY );


	// print the date and time at the top right of the page
	if ( bDateTime )
	{
		SelectObject( hDC, pFonts->hFont );
		SIZE size;
		int nLen = _tcslen( pszDateTime );
		GetTextExtentPoint32( hDC, pszDateTime, nLen, &size );
		SetTextColor( hDC, RGB( 0, 0, 0 ) );
		SetBkMode( hDC, TRANSPARENT );
		ExtTextOut( hDC, rcText.right - size.cx, rcText.top, 0, NULL, pszDateTime, nLen, NULL );
		rcText.top += ( int ) ( 1.5 * cyLine );
	}

	int yDateTime = rcText.bottom - cyLine;
	int xDateTimeRight = rcText.right;

	if ( bDateTime )
	{
		rcText.bottom -= ( int ) ( 1.5 * cyLine );
	}

	if ( bBorderThin || bBorderThick )
	{
		int cxyPen = bBorderThin ? 1 : ( cxInch / 48 );
		HPEN hPen = CreatePen( PS_SOLID, cxyPen, RGB( 0, 0, 0 ) ); 
		HPEN hPenOld = ( HPEN ) SelectObject( hDC, hPen );
		InflateRect( &rcText, -cxyPen, -cxyPen );
		Rectangle( hDC, rcText.left, rcText.top, rcText.right, rcText.bottom );
		if ( bBorderDouble )
		{
			InflateRect( &rcText, -cxInch / 24, -cyInch / 24 );
			Rectangle( hDC, rcText.left, rcText.top, rcText.right, rcText.bottom );
		}
		SelectObject( hDC, hPenOld );
		DeleteObject( hPen );

		InflateRect( &rcText, -cxInch / 16, -cyInch / 16 );
	}

	// print the page num at the top right of the page
	if ( HAS_FLAG( dwFlags, CM_PRINT_PAGENUMS ) )
	{
		SelectObject( hDC, pFonts->hFont );
		TCHAR szPage[ 256 ];
		int cyText = rcText.bottom - rcText.top;
		nLineCount = max( 1, nLineCount );	// don't let the page count go to zero!
		wsprintf( szPage, LoadStringPtr( IDS_PAGENUM_OF ), nPage, ( int ) ceil( ( float ) nLineCount / ( ( float ) cyText / ( float ) cyLine ) ) );
		SIZE size;
		int nLen = _tcslen( szPage );
		GetTextExtentPoint32( hDC, szPage, nLen, &size );
		SetTextColor( hDC, RGB( 0, 0, 0 ) );
		SetBkMode( hDC, TRANSPARENT );
		ExtTextOut( hDC, xDateTimeRight - size.cx, yDateTime, 0, NULL, szPage, nLen, NULL );
	}
}

void CEdit::PrintOneLine( HDC hDC, DWORD dwFlags, int nLine, int x, int y, FONT_DATA *pFonts, int cxChar, int cxLine, int nStartCol, int nEndCol ) const
{
	LPCTSTR pszCompleteLine = m_Buffer.GetLineText( nLine );
	if ( !*pszCompleteLine )
		return;
	LPCTSTR pszStart = pszCompleteLine + nStartCol;
	LPCTSTR pszEnd = pszCompleteLine + nEndCol;

	// expand out the line so that tabs are converted to spaces
	int cbTab = m_Buffer.GetTabSize();
	LPCTSTR psz = pszStart;
	TCHAR szLine[ 1000 ];
	CharFill( szLine, _T(' '), ARRAY_SIZE( szLine ) );
	LPTSTR pszBuff = szLine;
	int nViewCol = 0;
	while ( *psz && psz < pszEnd )
	{
		if ( *psz == _T('\t') )
		{
			int nSpaces = ( ( nViewCol / cbTab ) * cbTab ) + cbTab - nViewCol;
			pszBuff += nSpaces;
			nViewCol += nSpaces;
		}
		else
		{
			*pszBuff++ = *psz;
			nViewCol++;
		}
		psz++;
	}

	CLineParser Parser( ( CBuffer * ) &m_Buffer, nLine );
	int nBuffCol = Parser.m_nPos = nStartCol;
	int xStart = x;
	int nViewColLeft = m_Buffer.ConvertBufferColToViewCol( nLine, nStartCol );
	int nViewColStart = nViewColLeft;
	BOOL bPastLine = FALSE;
	while ( Parser.MoreComing() && !bPastLine )
	{
		Parser.AcceptToken();
		nBuffCol += Parser.m_nTokenLen;
		if ( nBuffCol >= nEndCol )
		{
			nBuffCol = nEndCol;
			bPastLine = TRUE;
		}
		int nViewColEnd = m_Buffer.ConvertBufferColToViewCol( nLine, nBuffCol );
		x = xStart + ( nViewColStart - nViewColLeft ) * cxChar;

		CBuffer::LangToken eToken = Parser.m_eToken;		
		if ( Parser.m_bWasInComment || Parser.m_bIsCommentEndToken )
			eToken = CBuffer::eMultiLineCommentStart;
		else if ( Parser.m_bWasInString || Parser.m_bIsStringEndToken )
			eToken = CBuffer::eStringDelim;

		long cxExtraSpacing, cyDescentShift;
		HFONT hFont;
		
		// pick a nice font if caller wants rich fonts
		if ( HAS_FLAG( dwFlags, CM_PRINT_RICHFONTS ) )
		{
			hFont = GetTokenFont( eToken, cxExtraSpacing, cyDescentShift, pFonts );
			SetTextCharacterExtra( hDC, cxExtraSpacing );
		}
		else
		{
			hFont = pFonts->hFont;
			cyDescentShift = 0;
		}
		SelectObject( hDC, hFont );

		// if printing in color, configure the printer for this token
		if ( HAS_FLAG( dwFlags, CM_PRINT_COLOR ) )
		{
			SetTextColor( hDC, GetTokenColor( eToken, TRUE ) );
			COLORREF crBk = GetTokenColor( eToken, FALSE );
			if ( crBk != CLR_INVALID )
			{
				SetBkColor( hDC, crBk );
				SetBkMode( hDC, OPAQUE );
			}
			else
				SetBkMode( hDC, TRANSPARENT );
		}

		// don't draw past the right edge of the line
		if ( ( nViewColEnd - nViewColLeft ) * cxChar > cxLine )
		{
			nViewColEnd = nViewColLeft + ( cxLine / cxChar );
			bPastLine = TRUE;
		}
		// output one token
		ExtTextOut( hDC, x, y + cyDescentShift, 0, NULL, szLine + ( nViewColStart - nViewColLeft ), nViewColEnd - nViewColStart, NULL );
		// remember the start of the next token
		nViewColStart = nViewColEnd;
	}
}
