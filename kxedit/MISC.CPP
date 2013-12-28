#include "precomp.h"
#include "resource.h"
#include "cedit.h"

extern HINSTANCE hInstance;
LRESULT PASCAL LineToolTipWndProc(HWND hWnd, register UINT uMsg, register WPARAM wParam, register LPARAM lParam);
#define LINETOOLTIP_CLASS _T("LineToolTip")

BOOL operator==(const CM_HOTKEY &cmHK1, const CM_HOTKEY &cmHK2)
{
	return ( cmp_hotkeys( &cmHK1, &cmHK2 ) == 0 );
}

int cmp_hotkeys( const CM_HOTKEY *p1, const CM_HOTKEY *p2 )
{
	register int nResult = 0;
	nResult = p1->byModifiers1 - p2->byModifiers1;
	if ( !nResult )
	{
		nResult = p1->nVirtKey1 - p2->nVirtKey1;
		if ( !nResult )
		{
			nResult = p1->byModifiers2 - p2->byModifiers2;
			if ( !nResult )
			{
				nResult = p1->nVirtKey2 - p2->nVirtKey2;
			}
		}
	}
	return nResult;
}

BOOL RegisterLineToolTip()
{
	WNDCLASS wc;
    wc.style = 0;
    wc.lpfnWndProc = ( WNDPROC )LineToolTipWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof( LPVOID );
    wc.hInstance = hInstance;
    wc.hIcon = NULL;
    wc.hCursor = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = LINETOOLTIP_CLASS;

	
	return ( RegisterClass( &wc ) != 0 );
}

BOOL UnregisterLineToolTip()
{
	return UnregisterClass( LINETOOLTIP_CLASS, NULL );
}

static HWND g_hWndToolTip = NULL;

void StartLineToolTip( CEditView *pEditView )
{
	if ( g_hWndToolTip )
	{
		// refresh the text
		InvalidateRect( g_hWndToolTip, NULL, FALSE );
		UpdateWindow( g_hWndToolTip );
	}
	else
	{
		HDC hDC = GetDC( NULL );
		int cyFont = MulDiv( 8, GetDeviceCaps( hDC, LOGPIXELSY ), 72 ) + 4;
		SIZE size;
		VERIFY( GetTextExtentPoint32( hDC, _T("XXXXXXXXXX"), 10, &size ) );
		ReleaseDC( NULL, hDC );

		RECT rc;
		GetWindowRect( pEditView->GetVScrollBar(), &rc );
		rc.right = rc.left - 5;
		rc.left -= size.cx;
		rc.top = ( rc.top + rc.bottom - cyFont ) / 2;
		rc.bottom = rc.top + cyFont;

		g_hWndToolTip = CreateWindow( LINETOOLTIP_CLASS, 
									  NULL, 
									  WS_POPUP | WS_BORDER,
									  rc.left, 
									  rc.top, 
									  rc.right - rc.left, 
									  rc.bottom - rc.top,
									  pEditView->GetWindow(),
									  NULL, 
									  hInstance,
									  NULL );
		SetWindowLong( g_hWndToolTip, 0, ( DWORD ) pEditView );
		SetWindowPos( g_hWndToolTip, NULL, -1, -1, -1, -1, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW );
	}
}

void EndLineToolTip()
{
	if ( g_hWndToolTip )
	{
		VERIFY( DestroyWindow( g_hWndToolTip ) );
		g_hWndToolTip = NULL;
	}
}


LRESULT PASCAL LineToolTipWndProc(HWND hWnd, register UINT uMsg, register WPARAM wParam, register LPARAM lParam)
{
	switch ( uMsg )
	{
		case WM_ERASEBKGND:
		{
			return 0;
		}
		
		case WM_SETFOCUS:
		{
			// don't want the focus
			SetFocus( ( HWND ) wParam );
			return 0;
		}
		case WM_PAINT:
		{
			RECT rc;
			GetClientRect( hWnd, &rc );

			PAINTSTRUCT ps;
			BeginPaint( hWnd, &ps );

			// draw the line tool tip
			CEditView *pView = (CEditView *)GetWindowLong(hWnd, 0);
			HBRUSH hbr = CreateSolidBrush( GetSysColor( COLOR_INFOBK ) );
			FillRect( ps.hdc, &rc, hbr );
			DeleteObject( hbr );
			SetBkMode( ps.hdc, TRANSPARENT );
			LOGFONT lf;
			VERIFY( GetObject( GetStockObject( DEFAULT_GUI_FONT ), sizeof( lf ), &lf ) );
			lf.lfHeight = -MulDiv( 8, GetDeviceCaps( ps.hdc, LOGPIXELSY ), 72 );
			lf.lfWidth = 0;
			HFONT hFont = CreateFontIndirect( &lf );
			HFONT hOldFont = ( HFONT ) SelectObject( ps.hdc, hFont );
			TCHAR szTip[ 30 ];
			_tcscpy( szTip, LoadStringPtr( IDS_LINE_TIP_PREFIX ) );
			_itot( pView->GetTopIndex() + 1, szTip + _tcslen(szTip), 10 );
			SetTextColor( ps.hdc, GetSysColor( COLOR_INFOTEXT ) );
			DrawText( ps.hdc, szTip, -1, &rc, DT_SINGLELINE | DT_VCENTER | DT_CENTER );
			SelectObject( ps.hdc, hOldFont );
			DeleteObject( hFont );
			EndPaint( hWnd, &ps );
			return 0;
		}
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

CSaveSelection::CSaveSelection( CEdit *pCtrl )
{
	Initialize( pCtrl, TRUE );
}

CSaveSelection::CSaveSelection( CEdit *pCtrl, BOOL bAllowDamage )
{
	Initialize( pCtrl, bAllowDamage );
}

void CSaveSelection::Initialize( CEdit *pCtrl, BOOL bAllowDamage )
{
	ASSERT( pCtrl );
	m_pCtrl = pCtrl;
	CSelection *pSel = m_pCtrl->GetSelection();
	pSel->GetBufferSelection( m_nStartCol, m_nStartRow, m_nEndCol, m_nEndRow );
	m_bColumnSel = pSel->IsColumnSel();
	m_bAllowDamage = bAllowDamage;
}

CSaveSelection::~CSaveSelection()
{
	CSelection *pSel = m_pCtrl->GetSelection();
	pSel->EnableColumnSel( m_bColumnSel );
	pSel->SetExtendedSelection( m_nStartCol, m_nStartRow, m_nEndCol, m_nEndRow, FALSE, m_bAllowDamage );
}

CHourGlass::CHourGlass( CEdit *pCtrl )
{
	ASSERT( pCtrl );
	m_pCtrl = pCtrl;
	ASSERT( m_pCtrl->m_nHourGlass >= 0 );
	if ( ++m_pCtrl->m_nHourGlass == 1 )
	{
		// refresh the cursor
		SetCursor( LoadCursor( NULL, IDC_WAIT ) );
	}
}

CHourGlass::~CHourGlass()
{
	ASSERT( m_pCtrl->m_nHourGlass > 0 );
	if ( --m_pCtrl->m_nHourGlass == 0 )
	{
		POINT pt;
		// refresh the cursor
		GetCursorPos( &pt );
		SetCursorPos( pt.x, pt.y );
	}
}

CDelayRepaint::CDelayRepaint( CEdit *pCtrl )
{
	ASSERT( pCtrl );
	m_pCtrl = pCtrl;
	ASSERT( m_pCtrl->m_nDelayPaint >= 0 );
	if ( ++m_pCtrl->m_nDelayPaint == 1 )
	{
		m_pCtrl->m_Selection.HideCaret();
		SetRectEmpty( &m_pCtrl->m_rcDelayPaint );
	}
}

CDelayRepaint::~CDelayRepaint()
{
	ASSERT( m_pCtrl->m_nDelayPaint > 0 );
	if ( --m_pCtrl->m_nDelayPaint == 0 )
	{
		if ( !IsRectEmpty( &m_pCtrl->m_rcDelayPaint ) )
		{
			InvalidateRect( m_pCtrl->GetWindow(), &m_pCtrl->m_rcDelayPaint, FALSE );
			UpdateWindow( m_pCtrl->GetWindow() );
		}
		m_pCtrl->m_Selection.ShowCaret();
		m_pCtrl->m_Selection.UpdateCaretPosition();
	}
}

CEditTransaction::CEditTransaction( CEdit *pCtrl )
{
	ASSERT( pCtrl );
	m_pCtrl = pCtrl;
	CSelection *pSel = m_pCtrl->GetSelection();
	m_pCtrl->GetBuffer()->BeginEdit( pSel->GetEndRow(), pSel->GetEndCol() );
}

CEditTransaction::~CEditTransaction()
{
	ASSERT( m_pCtrl->GetBuffer()->IsInEditTransaction() );
	CSelection *pSel = m_pCtrl->GetSelection();
	m_pCtrl->GetBuffer()->EndEdit( pSel->GetEndRow(), pSel->GetEndCol() );
}

CSelTransaction::CSelTransaction( CEdit *pCtrl )
{
	ASSERT( pCtrl );
	m_pCtrl = pCtrl;
	ASSERT( m_pCtrl->m_nSelTrans >= 0 );
	if (++m_pCtrl->m_nSelTrans == 1)
		m_pCtrl->SaveViewSelState();
}

CSelTransaction::~CSelTransaction()
{
	ASSERT( m_pCtrl->m_nSelTrans > 0 );
	if ( --m_pCtrl->m_nSelTrans == 0 )
	{
		m_pCtrl->ProcessViewSelNotifications();
	}
}

CNoHideSel::CNoHideSel( CEdit *pCtrl )
{
	ASSERT( pCtrl );
	m_pCtrl = pCtrl;
	m_bOldState = m_pCtrl->m_bHideSel;
	m_pCtrl->m_bHideSel = FALSE;
}

CNoHideSel::~CNoHideSel()
{
	m_pCtrl->m_bHideSel = m_bOldState;
}

BOOL is_space( register TCHAR ch )
{
	#ifdef _UNICODE
	if ( ( UINT ) ch > 255 )
		return ISSPACE_EXPR( ch );	// use expensive version for less used chars
	else
	#endif
		return HAS_FLAG( CEdit::g_CharFlags[ ( BYTE ) ch ], CEdit::CHARFLAG_SPACE );
}

BOOL is_alphanumeric( register TCHAR ch )
{
	#ifdef _UNICODE
	if ( ( UINT ) ch > 255 )
		return ISALNUM_EXPR( ch );	// use expensive version for less used chars
	else
	#endif
		return HAS_FLAG( CEdit::g_CharFlags[ ( BYTE ) ch ], CEdit::CHARFLAG_ALPHANUMERIC );
}

BOOL is_alpha( register TCHAR ch )
{
	#ifdef _UNICODE
	if ( ( UINT ) ch > 255 )
		return ISALPHA_EXPR( ch );	// use expensive version for less used chars
	else
	#endif
		return HAS_FLAG( CEdit::g_CharFlags[ ( BYTE ) ch ], CEdit::CHARFLAG_ALPHA );
}

BOOL is_numeric( register TCHAR ch )
{
	#ifdef _UNICODE
	if ( ( UINT ) ch > 255 )
		return ISNUMERIC_EXPR( ch );	// use expensive version for less used chars
	else
	#endif
		return HAS_FLAG( CEdit::g_CharFlags[ ( BYTE ) ch ], CEdit::CHARFLAG_NUMERIC );
}

BOOL is_symbolorpunct( register TCHAR ch )
{
	#ifdef _UNICODE
	if ( ( UINT ) ch > 255 )
		return ISSYMBOLORPUNCT_EXPR( ch );	// use expensive version for less used chars
	else
	#endif
		return HAS_FLAG( CEdit::g_CharFlags[ ( BYTE ) ch ], CEdit::CHARFLAG_SYMBOLORPUNCT );
}

BOOL is_eoln( TCHAR ch )
{
	#ifdef _UNICODE
	if ( ( UINT ) ch > 255 )
		return ISEOLN_EXPR( ch );	// use expensive version for less used chars
	else
	#endif
		return HAS_FLAG( CEdit::g_CharFlags[ ( BYTE ) ch ], CEdit::CHARFLAG_EOLN );
}

#ifndef _UNICODE
BOOL is_leadbyte( TCHAR ch )
{
	return HAS_FLAG( CEdit::g_CharFlags[ ( BYTE ) ch ], CEdit::CHARFLAG_LEADBYTE );
}

BOOL is_trailbyte( TCHAR ch )
{
	return HAS_FLAG( CEdit::g_CharFlags[ ( BYTE ) ch ], CEdit::CHARFLAG_TRAILBYTE );
}

// logical char counter (useful in DBCS environment)
int log_strlen( LPCTSTR psz )
{
	int nChars = 0;
	while ( *psz )
	{
		nChars++;
		psz = _tcsinc( psz );
	}

	return nChars;
}

#endif

int compare_char_i( register TCHAR ch1, register TCHAR ch2 )
{
	// a quick map is used instead of the slow CharUpper API call
	register TCHAR chA = CEdit::g_UpperConv[ ( BYTE ) ch1 ];
	register TCHAR chB = CEdit::g_UpperConv[ ( BYTE ) ch2 ];

	return ( chA == chB ) ? 0 : ( ( chA < chB ) ? -1 : +1 );
}

BOOL equal_char_i( register TCHAR ch1, register TCHAR ch2 )
{
	// a quick map is used instead of the slow CharUpper API call
	return ( CEdit::g_UpperConv[ ( BYTE ) ch1 ] == CEdit::g_UpperConv[ ( BYTE ) ch2 ] );
}

void CharFill( LPTSTR pszBuff, TCHAR ch, int nCount )
{
	#ifdef _UNICODE
	for ( register int i = 0; i < nCount; i++ )
	{
		*pszBuff++ = ch;
	}
	#else
	memset( pszBuff, ch, nCount );
	#endif
}

// quick-n-dirty log
int _log( int nVal, int nRadix )
{
	int nLog = 0;
	int nTemp = nVal;
	for ( ;; )
	{
		nLog++;
		if ( nTemp < nRadix )
			break;
		nTemp = nTemp / nRadix;
	}

	return nLog;
}

int _atoi( LPCTSTR pszLine, int nRadix )
{
	LPCTSTR psz = pszLine;
	// skip leading spaces
	while ( *psz && is_space( *psz ) )
		psz++;
	LPCTSTR pszStart = psz;
	if ( nRadix == CM_HEXADECIMAL )
	{
		// tolerate 0x at the start of the number
		while ( *psz && *psz != _T('x') && *psz != _T('X') )
			psz++;

		if ( *psz )	// if found an 'x' -- start after this char
			pszStart = psz + 1;
	}

	int nResult = 0;
	psz = pszStart;
	while ( *psz )
	{
		TCHAR chDigit = CEdit::g_UpperConv[ *psz ];
		int nDigit = INT_MAX;
		if ( chDigit >= _T('0') && chDigit <= _T('9') )
			nDigit = chDigit - _T('0');
		else if ( chDigit >= _T('A') && chDigit <= _T('F') )
			nDigit = chDigit - _T('A') + 10;

		if ( nDigit > ( nRadix - 1 ) )
		{
			// digit is not valid for the numbering system
			nResult = 0;
			break;
		}
		else
		{
			// shift over one magnitude and add new digit
			nResult *= nRadix;
			nResult += nDigit;
		}
		psz++;
	}

	return nResult;
}

LPCTSTR LoadStringPtr( UINT unID )
{
	#define NUM_BUFS 3
	#define BUF_SIZE 256
	static TCHAR szBuf[ NUM_BUFS ][ BUF_SIZE + 1 ];
	static int nBuf = 0;
	LPTSTR psz = szBuf[ nBuf++ ];
	nBuf = nBuf % NUM_BUFS;
	VERIFY( LoadString( hInstance, unID, psz, BUF_SIZE ) );
	psz[ BUF_SIZE ] = _T('\0');
	return psz;
}

// NormalizeLOGFONT() will ensure that a LOGFONT struct is
// normalized such that the lfWidth member is not zero.  Creating
// such a font in a DBCS environment will result in the fixed-pitch
// characteristics to not be honored by ExtTextOut.  If we use a
// non-zero value, then the font will truly be fixed-pitch.
void NormalizeLOGFONT( LOGFONT &lf )
{
	// bail if already normalized
	if ( lf.lfWidth != 0 )
		return;

	HDC hDC = GetDC( NULL );
	BOOL bSuccess = FALSE;
	if ( hDC )
	{
		HFONT hFont = CreateFontIndirect( &lf );
		if ( hFont )
		{
			HFONT hFontOld = ( HFONT ) SelectObject( hDC, hFont );
			TEXTMETRIC tm;
			if ( GetTextMetrics( hDC, &tm ) )
			{
				lf.lfWidth = tm.tmAveCharWidth;
				bSuccess = TRUE;
			}
			SelectObject( hDC, hFontOld );
			DeleteObject( hFont );
		}
		ReleaseDC( NULL, hDC );
	}

	if ( !bSuccess )
	{
		// something went wrong -- just make the 
		// width half the height.  Generally speaking,
		// this should look reasonable for most fonts.
		lf.lfWidth = lf.lfHeight / 2;
	}
}

#ifdef _CODEMAX_BETA
void DrawBetaBackground( HDC hDC, const RECT &rc, COLORREF crWindow )
{
	TCHAR szWatermark[] = _T("CodeMax 2.0 Beta");

	HFONT hFont = CreateFont( MulDiv( 12, GetDeviceCaps( hDC, LOGPIXELSY ), 72 ), 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, _T("Arial") );
	HFONT hOldFont = ( HFONT ) SelectObject( hDC, hFont );
	SIZE size;
	VERIFY( GetTextExtentPoint32( hDC, szWatermark, _tcslen( szWatermark ), &size ) );
	int x = rc.right - size.cx - 10;
	int y = rc.bottom - size.cy - 10;
	x = max( x, rc.left );
	y = max( y, rc.top );
	SetTextColor( hDC, crWindow == RGB( 192, 192, 192 ) ? RGB( 128, 128, 128 ) : RGB( 192, 192, 192 ) );
	SetBkMode( hDC, TRANSPARENT );
	TextOut( hDC, x, y, szWatermark, _tcslen( szWatermark ) );
	SelectObject( hDC, hOldFont );
	DeleteObject( hFont );
}
#endif

#ifdef _ACTIVEX
LPTSTR BSTR2TSTR( BSTR str )
{
	int nLen = SysStringLen( str );
#ifdef _UNICODE
	LPTSTR psz = new TCHAR[ nLen + 1 ];
	ASSERT( psz );
	memcpy( psz, bstr, nLen * sizeof( TCHAR ) );
	psz[ nLen ] = _T('\0');
#else
	int nBytes = WideCharToMultiByte( CP_ACP, 0, str, nLen, NULL, NULL, NULL,NULL );
	LPSTR psz = new TCHAR[ nBytes + 1 ];
	ASSERT( psz );
	WideCharToMultiByte( CP_ACP, 0, str, nLen, psz, nBytes, NULL, NULL );
	psz[ nBytes ] = _T('\0');
#endif
	return psz;
}

#endif

#ifdef _DEBUG
// this function exists only in the debug build and exists solely to catch compile errors in the CM_xxx macros
void MacroVerify()
{
	HWND hWnd = NULL;
	{
		LPCTSTR pszName = 0; 
		CM_SetLanguage( hWnd, pszName );
	}
	{
		LPCTSTR pszName = 0; 
		CM_GetLanguage( hWnd, pszName );
	}
	{
		BOOL bEnable = 0; 
		CM_EnableColorSyntax( hWnd, bEnable );
	}
	{
		CM_IsColorSyntaxEnabled( hWnd );
	}
	{
		LPCTSTR pszFileName = 0;
		CM_OpenFile( hWnd, pszFileName );
	}
	{
		LPCTSTR pszFileName = 0;
		CM_POSITION *pPos = 0;
		CM_InsertFile( hWnd, pszFileName, pPos );
	}
	{
		LPCTSTR pszText = 0;
		CM_POSITION *pPos = 0;
		CM_InsertText( hWnd, pszText, pPos );
	}
	{
		LPCTSTR pszText = 0;
		CM_SetText( hWnd, pszText );
	}
	{
		CM_COLORS *pColors = 0;
		CM_SetColors( hWnd, pColors );
	}
	{
		CM_COLORS *pColors = 0;
		CM_GetColors( hWnd, pColors );
	}
	{
		BOOL bEnable = 0;
		CM_EnableWhitespaceDisplay( hWnd, bEnable );
	}
	{
		CM_IsWhitespaceDisplayEnabled( hWnd );
	}
	{
		BOOL bEnable = 0;
		CM_EnableTabExpand( hWnd, bEnable );
	}
	{
		CM_IsTabExpandEnabled( hWnd );
	}
	{
		BOOL bEnable = 0;
		CM_EnableSmoothScrolling( hWnd, bEnable );
	}
	{
		CM_IsSmoothScrollingEnabled( hWnd );
	}
	{
		int nTabSize = 0;
		CM_SetTabSize( hWnd, nTabSize );
	}
	{
		CM_GetTabSize( hWnd );
	}
	{
		BOOL bReadOnly = 0;
		CM_SetReadOnly( hWnd, bReadOnly );
	}
	{
		CM_IsReadOnly( hWnd );
	}
	{
		BOOL bEnable = 0;
		CM_EnableLineToolTips( hWnd, bEnable );
	}
	{
		CM_IsLineToolTipsEnabled( hWnd );
	}
	{
		BOOL bEnable = 0;
		CM_EnableLeftMargin( hWnd, bEnable );
	}
	{
		CM_IsLeftMarginEnabled( hWnd );
	}
	{
		BOOL bClearUndo = 0;
		LPCTSTR pszFileName = 0;
		CM_SaveFile( hWnd, bClearUndo, pszFileName );
	}
	{
		LPCTSTR pszText = 0;
		CM_RANGE *pRange = 0;
		CM_ReplaceText( hWnd, pszText, pRange );
	}
	{
		CM_RANGE *pRange = 0;
		BOOL bLogical = FALSE;
		CM_GetTextLength( hWnd, pRange, bLogical );
	}
	{
		LPCTSTR pszBuff = 0;
		CM_RANGE *pRange = 0;
		CM_GetText( hWnd, pszBuff, pRange );
	}
	{
		int nLine = 0;
		LPCTSTR pszBuff = 0;
		CM_GetLine( hWnd, nLine, pszBuff );
	}
	{
		int nLine = 0;
		BOOL bLogical = FALSE;
		CM_GetLineLength( hWnd, nLine, bLogical );
	}
	{
		LPCTSTR pszBuff = 0;
		CM_POSITION *pPos = 0;
		CM_GetWord( hWnd, pszBuff, pPos );
	}
	{
		CM_POSITION *pPos = 0;
		BOOL bLogical = FALSE;
		CM_GetWordLength( hWnd, pPos, bLogical );
	}
	{
		LPCTSTR pszText = 0;
		CM_AddText( hWnd, pszText );
	}
	{
		int nLine = 0;
		CM_DeleteLine( hWnd, nLine );
	}
	{
		int nLine = 0;
		LPCTSTR pszText = 0;
		CM_InsertLine( hWnd, nLine, pszText );
	}
	{
		CM_RANGE *pRange = 0;
		BOOL bNormalized = 0;
		CM_GetSel( hWnd, pRange, bNormalized );
	}
	{
		CM_RANGE *pRange = 0;
		BOOL bMakeVisible = 0;
		CM_SetSel( hWnd, pRange, bMakeVisible );
	}
	{
		CM_DeleteSel( hWnd );
	}
	{
		LPCTSTR pszText = 0;
		CM_ReplaceSel( hWnd, pszText );
	}
	{
		WORD wCmd = 0;
		DWORD dwCmdData = 0;
		CM_ExecuteCmd( hWnd, wCmd, dwCmdData );
	}
	{
		BOOL bHorz = 0;
		int nPos = 0;
		CM_SetSplitterPos( hWnd, bHorz, nPos );
	}
	{
		BOOL bHorz = 0;
		CM_GetSplitterPos( hWnd, bHorz );
	}
	{
		int nMode = 0;
		CM_SetAutoIndentMode( hWnd, nMode );
	}
	{
		CM_GetAutoIndentMode( hWnd );
	}
	{
		CM_CanUndo( hWnd );
	}
	{
		CM_CanRedo( hWnd );
	}
	{
		CM_Redo( hWnd );
	}
	{
		CM_Undo( hWnd );
	}
	{
		CM_ClearUndoBuffer( hWnd );
	}
	{
		int nLimit = 0;
		CM_SetUndoLimit( hWnd, nLimit );
	}
	{
		CM_GetUndoLimit( hWnd );
	}
	{
		CM_IsModified( hWnd );
	}
	{
		BOOL bModified = 0;
		CM_SetModified( hWnd, bModified );
	}
	{
		BOOL bEnable = 0;
		CM_EnableOvertype( hWnd, bEnable );
	}
	{
		CM_IsOvertypeEnabled( hWnd );
	}
	{
		BOOL bEnable = 0;
		CM_EnableCaseSensitive( hWnd, bEnable );
	}
	{
		CM_IsCaseSensitiveEnabled( hWnd );
	}
	{
		BOOL bEnable = 0;
		CM_EnablePreserveCase( hWnd, bEnable );
	}
	{
		CM_IsPreserveCaseEnabled( hWnd );
	}
	{
		BOOL bEnable = 0;
		CM_EnableWholeWord( hWnd, bEnable );
	}
	{
		CM_IsWholeWordEnabled( hWnd );
	}
	{
		BOOL bEnable = 0;
		CM_EnableRegExp( hWnd, bEnable );
	}
	{
		CM_IsRegExpEnabled( hWnd );
	}
	{
		int nView = 0;
		int nLine = 0;
		CM_SetTopIndex( hWnd, nView, nLine );
	}
	{
		int nView = 0;
		CM_GetTopIndex( hWnd, nView );
	}
	{
		int nView = 0;
		BOOL bFullyVisible = 0;
		CM_GetVisibleLineCount( hWnd, nView, bFullyVisible );
	}
	{
		BOOL bEnable = 0;
		CM_EnableCRLF( hWnd, bEnable );
	}
	{
		CM_IsCRLFEnabled( hWnd );
	}
	{
		CM_GetCurrentView( hWnd );
	}
	{
		int nView = 0;
		CM_SetCurrentView( hWnd, nView );
	}
	{
		CM_GetViewCount( hWnd );
	}
	{
		BOOL bHorz = 0;
		BOOL bShow = 0;
		CM_ShowScrollBar( hWnd, bHorz, bShow );
	}
	{
		BOOL bHorz = 0;
		CM_HasScrollBar( hWnd, bHorz );
	}
	{
		int xClient = 0;
		int yClient = 0;
		CM_RANGE *pRange = 0;
		CM_GetSelFromPoint( hWnd, xClient, yClient, pRange );
	}
	{
		int nLine = 0;
		BOOL bMakeVisible = 0;
		CM_SelectLine( hWnd, nLine, bMakeVisible );
	}
	{
		int xClient = 0;
		int yClient = 0;
		CM_HitTest( hWnd, xClient, yClient );
	}
	{
		BOOL bEnable = 0;
		CM_EnableDragDrop( hWnd, bEnable );
	}
	{
		CM_IsDragDropEnabled( hWnd );
	}
	{
		BOOL bHorz = 0;
		BOOL bEnable = 0;
		CM_EnableSplitter( hWnd, bHorz, bEnable );
	}
	{
		BOOL bHorz = 0;
		CM_IsSplitterEnabled( hWnd, bHorz );
	}
	{
		BOOL bEnable = 0;
		CM_EnableColumnSel( hWnd, bEnable );
	}
	{
		CM_IsColumnSelEnabled( hWnd );
	}
	{
		BOOL bEnable = 0;
		CM_EnableGlobalProps( hWnd, bEnable );
	}
	{
		CM_IsGlobalPropsEnabled( hWnd );
	}
	{
		CM_IsRecordingMacro( hWnd );
	}
	{
		CM_IsPlayingMacro( hWnd );
	}
	{
		HWND hWndParent = 0;
		CM_SetDlgParent( hWnd, hWndParent );
	}
	{
		BOOL bEnable = 0;
		CM_EnableSelBounds( hWnd, bEnable );
	}
	{
		CM_IsSelBoundsEnabled( hWnd );
	}
	{
		CM_FONTSTYLES *pFontStyles = NULL;
		CM_SetFontStyles( hWnd, pFontStyles );
	}
	{
		CM_FONTSTYLES *pFontStyles = NULL;
		CM_GetFontStyles( hWnd, pFontStyles );
	}
	{
		LPARAM lParam = 0;
		int nLine = 0;
		CM_SetItemData( hWnd, nLine, lParam );
	}
	{
		int nLine = 0;
		CM_GetItemData( hWnd, nLine );
	}
	{
		int nLine = 0;
		DWORD dwStyle = 0;
		CM_SetLineStyle( hWnd, nLine, dwStyle );
	}
	{
		int nLine = 0;
		CM_GetLineStyle( hWnd, nLine );
	}
	{
		int nLine = 0;
		BOOL bEnable = 0;
		CM_SetBookmark( hWnd, nLine, bEnable );
	}
	{
		int nLine = 0;
		CM_GetBookmark( hWnd, nLine );
	}
	{
		int nCount = 0;
		LPDWORD pdwLines = NULL;
		CM_SetAllBookmarks( hWnd, nCount, pdwLines );
	}
	{
		LPDWORD pdwLines = NULL;
		CM_GetAllBookmarks( hWnd, pdwLines );
	}
	{
		CM_LINENUMBERING *pNumbering = NULL;
		CM_SetLineNumbering( hWnd, pNumbering );
	}
	{
		CM_LINENUMBERING *pNumbering = NULL;
		CM_GetLineNumbering( hWnd, pNumbering );
	}
	{
		CM_POSITION *pPos = NULL;
		LPRECT pRect = NULL;
		CM_PosFromChar( hWnd, pPos, pRect );
	}
	{
		BOOL bEnable = 0;
		CM_EnableHideSel( hWnd, bEnable );
	}
	{
		CM_IsHideSelEnabled( hWnd );
	}
	{
		int nLine = 0;
		CM_SetHighlightedLine( hWnd, nLine );
	}
	{
		CM_GetHighlightedLine( hWnd );
	}
	{
		BOOL bEnable = 0;
		CM_EnableNormalizeCase( hWnd, bEnable );
	}
	{
		CM_IsNormalizeCaseEnabled( hWnd );
	}
	{
		int nLine = 0;
		BOOL bEnable = 0;
		CM_SetDivider( hWnd, nLine, bEnable );
	}
	{
		int nLine = 0;
		CM_GetDivider( hWnd, nLine );
	}
	{
		BOOL bEnable = 0;
		CM_EnableOvertypeCaret( hWnd, bEnable );
	}
	{
		CM_IsOvertypeCaretEnabled( hWnd );
	}
	{
		LPCTSTR pszText = NULL;
		CM_SetFindText( hWnd, pszText );
	}
	{
		LPCTSTR pszText = NULL;
		CM_GetFindText( hWnd, pszText );
	}
	{
		LPCTSTR pszText = NULL;
		CM_SetReplaceText( hWnd, pszText );
	}
	{
		LPCTSTR pszText = NULL;
		CM_GetReplaceText( hWnd, pszText );
	}
	{
		HIMAGELIST hImageList = NULL;
		CM_SetImageList( hWnd, hImageList );
	}
	{
		CM_GetImageList( hWnd );
	}
	{
		int nLine = 0;
		BYTE byImages = 0;
		CM_SetMarginImages( hWnd, nLine, byImages );
	}
	{
		int nLine = 0;
		CM_GetMarginImages( hWnd, nLine );
	}
	{
		CM_AboutBox( hWnd );
	}
	{
		HDC hDC = NULL;
		WORD wFlags = 0;
		CM_Print( hWnd, hDC, wFlags );
	}
	{
		int nLine = 0;
		int nCol = 0;
		CM_SetCaretPos( hWnd, nLine, nCol );
	}
	{
		int nLine = 0;
		int nCol = 0;
		CM_ViewColToBufferCol( hWnd, nLine, nCol );
	}
	{
		int nLine = 0;
		int nCol = 0;
		CM_BufferColToViewCol( hWnd, nLine, nCol );
	}
}
#endif

#pragma pack(push, 1)

typedef struct
{
	WORD dlgVer;
	WORD signature;
	DWORD helpID;
	DWORD exStyle;
	DWORD style;
	WORD cDlgItems;
	short x;
	short y;
	short cx;
	short cy;
} DLGTEMPLATEEX;

typedef struct
{
	DWORD helpID;
	DWORD exStyle;
	DWORD style;
	short x;
	short y;
	short cx;
	short cy;
	DWORD id;
} DLGITEMTEMPLATEEX;

#pragma pack(pop)


/////////////////////////////////////////////////////////////////////////////
// CDialogTemplate

class CDialogTemplate
{
// Constructors
public:
	CDialogTemplate(const DLGTEMPLATE* pTemplate = NULL);

// Attributes
	BOOL HasFont() const;
	BOOL SetFont(LPCTSTR lpFaceName, WORD nFontSize);
	BOOL SetSystemFont(WORD nFontSize = 0);

// Operations
	BOOL Load(LPCTSTR lpDialogTemplateID);
	HGLOBAL Detach();

// Implementation
public:
	~CDialogTemplate();

	HGLOBAL m_hTemplate;
	DWORD m_dwTemplateSize;
	BOOL m_bSystemFont;

protected:
	static BYTE* __cdecl GetFontSizeField(const DLGTEMPLATE* pTemplate);
	static UINT __cdecl GetTemplateSize(const DLGTEMPLATE* pTemplate);
	BOOL SetTemplate(const DLGTEMPLATE* pTemplate, UINT cb);
};

inline BOOL IsDialogEx(const DLGTEMPLATE* pTemplate)
{
	return ((DLGTEMPLATEEX*)pTemplate)->signature == 0xFFFF;
}
inline BOOL HasFont(const DLGTEMPLATE* pTemplate)
{
	return (DS_SETFONT &
		(IsDialogEx(pTemplate) ? ((DLGTEMPLATEEX*)pTemplate)->style :
		pTemplate->style));
}

inline int FontAttrSize(BOOL bDialogEx)
{
	return sizeof(WORD) * (bDialogEx ? 3 : 1);
}
CDialogTemplate::CDialogTemplate(const DLGTEMPLATE* pTemplate)
{
	if (pTemplate == NULL)
	{
		m_hTemplate = NULL;
		m_dwTemplateSize = 0;
		m_bSystemFont = FALSE;
	}
	else
	{
		SetTemplate(pTemplate, GetTemplateSize(pTemplate));
	}
}

BOOL CDialogTemplate::SetTemplate(const DLGTEMPLATE* pTemplate, UINT cb)
{
	m_dwTemplateSize = cb;

	if ((m_hTemplate = GlobalAlloc(GPTR, m_dwTemplateSize + LF_FACESIZE * 2)) == NULL)
		return FALSE;
	DLGTEMPLATE* pNew = (DLGTEMPLATE*)GlobalLock(m_hTemplate);
	memcpy((BYTE*)pNew, pTemplate, (size_t)m_dwTemplateSize);

	m_bSystemFont = (::HasFont(pNew) == 0);

	GlobalUnlock(m_hTemplate);
	return TRUE;
}

CDialogTemplate::~CDialogTemplate()
{
	if (m_hTemplate != NULL)
		GlobalFree(m_hTemplate);
}

BOOL CDialogTemplate::Load(LPCTSTR lpDialogTemplateID)
{
	if (hInstance == NULL)
		return FALSE;
	HRSRC hRsrc = FindResource(hInstance, lpDialogTemplateID, RT_DIALOG);
	if (hRsrc == NULL)
		return FALSE;
	HGLOBAL hTemplate = LoadResource(hInstance, hRsrc);
	DLGTEMPLATE* pTemplate = (DLGTEMPLATE*)LockResource(hTemplate);
	SetTemplate(pTemplate, (UINT)SizeofResource(hInstance, hRsrc));
	UnlockResource(hTemplate);
	FreeResource(hTemplate);
	return TRUE;
}

HGLOBAL CDialogTemplate::Detach()
{
	HGLOBAL hTmp = m_hTemplate;
	m_hTemplate = NULL;
	return hTmp;
}

BOOL CDialogTemplate::HasFont() const
{
	DLGTEMPLATE* pTemplate = (DLGTEMPLATE*)GlobalLock(m_hTemplate);
	BOOL bHasFont = ::HasFont(pTemplate);
	GlobalUnlock(m_hTemplate);
	return bHasFont;
}

__forceinline WCHAR* _SkipString(WCHAR* p)
{
	while (*p++);
	return p;
}

BYTE* __cdecl CDialogTemplate::GetFontSizeField(const DLGTEMPLATE* pTemplate)
{
	BOOL bDialogEx = IsDialogEx(pTemplate);
	WORD* pw;

	if (bDialogEx)
		pw = (WORD*)((DLGTEMPLATEEX*)pTemplate + 1);
	else
		pw = (WORD*)(pTemplate + 1);

	if (*pw == (WORD)-1)        // Skip menu name string or ordinal
		pw += 2; // WORDs
	else
		while(*pw++);

	if (*pw == (WORD)-1)        // Skip class name string or ordinal
		pw += 2; // WORDs
	else
		while(*pw++);

	while (*pw++);          // Skip caption string

	return (BYTE*)pw;
}

UINT __cdecl CDialogTemplate::GetTemplateSize(const DLGTEMPLATE* pTemplate)
{
	BOOL bDialogEx = IsDialogEx(pTemplate);
	BYTE* pb = GetFontSizeField(pTemplate);

	if (::HasFont(pTemplate))
	{
		// Skip font size and name
		pb += FontAttrSize(bDialogEx);  // Skip font size, weight, (italic, charset)
		pb += 2 * (wcslen((WCHAR*)pb) + 1);
	}

	WORD nCtrl = bDialogEx ? (WORD)((DLGTEMPLATEEX*)pTemplate)->cDlgItems :
		(WORD)pTemplate->cdit;

	while (nCtrl > 0)
	{
		pb = (BYTE*)(((DWORD)pb + 3) & ~3); // DWORD align

		pb += (bDialogEx ? sizeof(DLGITEMTEMPLATEEX) : sizeof(DLGITEMTEMPLATE));

		if (*(WORD*)pb == (WORD)-1)     // Skip class name string or ordinal
			pb += 2 * sizeof(WORD);
		else
			pb = (BYTE*)_SkipString((WCHAR*)pb);

		if (*(WORD*)pb == (WORD)-1)     // Skip text string or ordinal
			pb += 2 * sizeof(WORD);
		else
			pb = (BYTE*)_SkipString((WCHAR*)pb);

		WORD cbExtra = *(WORD*)pb;      // Skip extra data
		pb += sizeof(WORD) + cbExtra;
		--nCtrl;
	}

	return pb - (BYTE*)pTemplate;
}

BOOL CDialogTemplate::SetFont(LPCTSTR lpFaceName, WORD nFontSize)
{
	ASSERT(m_hTemplate != NULL);

	if (m_dwTemplateSize == 0)
		return FALSE;

	DLGTEMPLATE* pTemplate = (DLGTEMPLATE*)GlobalLock(m_hTemplate);

	BOOL bDialogEx = IsDialogEx(pTemplate);
	BOOL bHasFont = ::HasFont(pTemplate);
	int cbFontAttr = FontAttrSize(bDialogEx);

	if (bDialogEx)
		((DLGTEMPLATEEX*)pTemplate)->style |= DS_SETFONT;
	else
		pTemplate->style |= DS_SETFONT;

#ifdef _UNICODE
	int cbNew = cbFontAttr + ((_tcslen(lpFaceName) + 1) * sizeof(TCHAR));
	BYTE* pbNew = (BYTE*)lpFaceName;
#else
	WCHAR wszFaceName [LF_FACESIZE];
	int cbNew = cbFontAttr + 2 * MultiByteToWideChar(CP_ACP, 0, lpFaceName, -1, wszFaceName, LF_FACESIZE);
	BYTE* pbNew = (BYTE*)wszFaceName;
#endif

	BYTE* pb = GetFontSizeField(pTemplate);
	int cbOld = bHasFont ? cbFontAttr + 2 * (wcslen((WCHAR*)(pb + cbFontAttr)) + 1) : 0;

	BYTE* pOldControls = (BYTE*)(((DWORD)pb + cbOld + 3) & ~3);
	BYTE* pNewControls = (BYTE*)(((DWORD)pb + cbNew + 3) & ~3);

	WORD nCtrl = bDialogEx ? (WORD)((DLGTEMPLATEEX*)pTemplate)->cDlgItems :
		(WORD)pTemplate->cdit;

	if (cbNew != cbOld && nCtrl > 0)
		memmove(pNewControls, pOldControls, (size_t)(m_dwTemplateSize - (pOldControls - (BYTE*)pTemplate)));

	*(WORD*)pb = nFontSize;
	memmove(pb + cbFontAttr, pbNew, cbNew - cbFontAttr);

	m_dwTemplateSize += (pNewControls - pOldControls);

	GlobalUnlock(m_hTemplate);
	m_bSystemFont = FALSE;
	return TRUE;
}

BOOL CDialogTemplate::SetSystemFont(WORD wSize)
{
	LPCTSTR pszFace = _T("System");
	WORD wDefSize = 10;
	HFONT hFont = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
	if (hFont == NULL)
		hFont = (HFONT)::GetStockObject(SYSTEM_FONT);
	LOGFONT lf;
	if (hFont != NULL)
	{
		if (::GetObject(hFont, sizeof(LOGFONT), &lf) != 0)
		{
			pszFace = lf.lfFaceName;
			HDC hDC = ::GetDC(NULL);
			if (lf.lfHeight < 0)
				lf.lfHeight = -lf.lfHeight;
			wDefSize = (WORD)MulDiv(lf.lfHeight, 72, GetDeviceCaps(hDC, LOGPIXELSY));
			::ReleaseDC(NULL, hDC);
		}
	}

	if (wSize == 0)
		wSize = wDefSize;

	return SetFont(pszFace, wSize);
}

// LoadDialogTemplate() returns an HGLOBAL to a DLGTEMPLATE structure 
// given the template's name.
// The HGLOBAL that is returned must be unlocked and freed using 
// UnlockAndFreeDialogTemplate().
// Use GlobalLock (not LockResource) to obtain a pointer to the memory.
// The primary purpose of loading the template is to change the font
// if necessary.
HGLOBAL LoadDialogTemplate( LPCTSTR lpszTemplateName )
{
	// Get lpDialogTemplate from lpszTemplateName
	LPCDLGTEMPLATE lpDialogTemplate = NULL;
	HRSRC hResource = ::FindResource(hInstance, lpszTemplateName, RT_DIALOG);
	HGLOBAL hDialogTemplate = LoadResource(hInstance, hResource);
	if (hDialogTemplate != NULL)
	{
		lpDialogTemplate = (LPCDLGTEMPLATE)LockResource(hDialogTemplate);
	}
	ASSERT(lpDialogTemplate != NULL);

	// set the system GUI font as the dialog font
	CDialogTemplate dlgTemp(lpDialogTemplate);
	dlgTemp.SetSystemFont(0);
	return dlgTemp.Detach();
}

void UnlockAndFreeDialogTemplate( HGLOBAL hTemplate )
{
	if (hTemplate != NULL)
	{
		GlobalUnlock(hTemplate);
		GlobalFree(hTemplate);
	}
}

