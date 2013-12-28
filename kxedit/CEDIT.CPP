#include "precomp.h"
#include "cedit.h"
#include "resource.h"
#ifdef _ACTIVEX
#include "editx.h"
#include "ceditx.h"
#endif

extern HINSTANCE hInstance;

HCURSOR CEdit::g_hcurHSplit = NULL;
HCURSOR CEdit::g_hcurVSplit = NULL;
HCURSOR CEdit::g_hcurHVSplit = NULL;
HCURSOR CEdit::g_hcurRecord = NULL;
HCURSOR CEdit::g_hcurLeftMargin = NULL;
CFindReplaceData CEdit::g_FindReplaceData;
HWND *CEdit::g_phWnds = NULL;
int CEdit::g_nhWndCount = 0;
HFONT CEdit::g_hFontDefault;
int CEdit::g_dwVersion = 0x00000000;

// We want g_bCopyColumn to be shared, so that instances of CodeMax will intelligently handle columns on the clipboard.
#pragma data_seg( "Shared" )
BOOL g_bCopyColumn = FALSE;
#pragma data_seg()
#pragma comment(linker, "-section:Shared,rws")

CEdit::CEdit( HWND hWnd )
{
	SetMode( eConstructing );
	ASSERT( IsWindow( hWnd ) );
	m_hWnd = hWnd;
	m_hWndDlgParent = hWnd;
	m_Buffer.SetCtrl( this );
	SetDisplayFont( g_hFontDefault, TRUE );
	m_dwStyle = 0;
	m_Buffer.SetNotificationCallback( ( BUFFERCALLBACKFN * )BufferCallback, ( LPARAM )this );
	m_bAppendNextCut = FALSE;
	m_bEatKeyMsg = FALSE;
	m_bSkipTranslate = FALSE;
	m_bOverType = DEF_OVERTYPE;
	m_bHideSel = DEF_HIDESEL;

	m_bAllowHSplit = DEF_ALLOWHSPLIT;
	m_bAllowVSplit = DEF_ALLOWVSPLIT;
	m_xPosHSplitter = DEF_POSHSPLITTER;
	m_yPosVSplitter = DEF_POSVSPLITTER;
	m_bDisplayWhitespace = DEF_DISPLAYWHITESPACE;
	m_bSmoothScroll = DEF_SMOOTHSCROLL;
	m_bLineToolTips = DEF_LINETOOLTIPS;
	m_bAllowDragDrop = DEF_ALLOWDRAGDROP;
	m_bAllowColumnSel = DEF_ALLOWCOLUMNSEL;
	m_nHourGlass = 0;
	m_hbmPaint = NULL;
	m_hdcPaint = NULL;
	m_sizeOffScreenBmp.cx = m_sizeOffScreenBmp.cy = 0;
	m_bSelBounds = DEF_SELBOUNDS;
	m_nHighlightedLine = DEF_HIGHLIGHTEDLINE;
	m_bOvertypeCaret = DEF_OVERTYPECARET;
	
	m_nRepeatCount = 1;
	m_bColorSyntax = DEF_COLORSYNTAX;
	m_bExpandTabs = DEF_EXPANDTABS;
	m_bGlobalProps = DEF_GLOBALPROPS;
	m_bOwnFont = FALSE;
	m_eIndentStyle = DEF_INDENTSTYLE;

	m_nCmdNestLevel = 0;
	
	m_chLeadByteIn = _T('\0');

	for ( int i = 0; i < ARRAY_SIZE( m_Views ); i++ )
	{
		m_Views[ i ] = NULL;
	}
	m_nViews = 0;

	m_pActiveMacro = NULL;
	m_bAbortMacro = FALSE;
	m_pMacroRecordBuffer = NULL;
	m_cbMacroRecordBuffer = 0;
	m_hDlgRecord = NULL;
	m_hbmEndRecord = NULL;

	ASSERT( CMD_PLAYMACRO1 + 1 == CMD_PLAYMACRO2 );
	ASSERT( CMD_PLAYMACRO2 + 1 == CMD_PLAYMACRO3 );
	ASSERT( CMD_PLAYMACRO3 + 1 == CMD_PLAYMACRO4 );
	ASSERT( CMD_PLAYMACRO4 + 1 == CMD_PLAYMACRO5 );
	ASSERT( CMD_PLAYMACRO5 + 1 == CMD_PLAYMACRO6 );
	ASSERT( CMD_PLAYMACRO6 + 1 == CMD_PLAYMACRO7 );
	ASSERT( CMD_PLAYMACRO7 + 1 == CMD_PLAYMACRO8 );
	ASSERT( CMD_PLAYMACRO8 + 1 == CMD_PLAYMACRO9 );
	ASSERT( CMD_PLAYMACRO9 + 1 == CMD_PLAYMACRO10 );
	ASSERT( CMD_FIRST != 0 );  // property dialog assumes this

	// default text colors
	m_Colors.crWindow			        = DEF_CLRWINDOW;
	m_Colors.crLeftMargin		        = DEF_CLRLEFTMARGIN;
	m_Colors.crBookmark			        = DEF_CLRBOOKMARK;
	m_Colors.crBookmarkBk		        = DEF_CLRBOOKMARKBK;
	m_Colors.crText				        = DEF_CLRTEXT;
	m_Colors.crTextBk			        = DEF_CLRTEXTBK;
	m_Colors.crNumber			        = DEF_CLRNUMBER;
	m_Colors.crNumberBk			        = DEF_CLRNUMBERBK;
	m_Colors.crKeyword			        = DEF_CLRKEYWORD;
	m_Colors.crKeywordBk		        = DEF_CLRKEYWORDBK;
	m_Colors.crOperator			        = DEF_CLROPERATOR;
	m_Colors.crOperatorBk		        = DEF_CLROPERATORBK;
	m_Colors.crScopeKeyword		        = DEF_CLRSCOPEKEYWORD;
	m_Colors.crScopeKeywordBk	        = DEF_CLRSCOPEKEYWORDBK;
	m_Colors.crComment			        = DEF_CLRCOMMENT;
	m_Colors.crCommentBk		        = DEF_CLRCOMMENTBK;
	m_Colors.crString			        = DEF_CLRSTRING;
	m_Colors.crStringBk			        = DEF_CLRSTRINGBK;
	m_Colors.crTagText				    = DEF_CLRTAGTEXT;
	m_Colors.crTagTextBk			    = DEF_CLRTAGTEXTBK;
    m_Colors.crTagEntity		        = DEF_CLRTAGENT;
	m_Colors.crTagEntityBk		        = DEF_CLRTAGENTBK;
    m_Colors.crTagElementName			= DEF_CLRTAGELEMNAME;
    m_Colors.crTagElementNameBk			= DEF_CLRTAGELEMNAMEBK;
    m_Colors.crTagAttributeName			= DEF_CLRTAGATTRNAME;
    m_Colors.crTagAttributeNameBk		= DEF_CLRTAGATTRNAMEBK;
    m_Colors.crLineNumber				= DEF_CLRLINENUMBER;
    m_Colors.crLineNumberBk				= DEF_CLRLINENUMBERBK;
	m_Colors.crHDividerLines			= DEF_CLRHDIVIDERLINES;
	m_Colors.crVDividerLines			= DEF_CLRVDIVIDERLINES;
	m_Colors.crHighlightedLine			= DEF_CLRHIGHLIGHTEDLINE;

	// default font styles
	m_FontStyles.byKeyword				= DEF_FSKEYWORD;
	m_FontStyles.byComment				= DEF_FSCOMMENT;
	m_FontStyles.byOperator				= DEF_FSOPERATOR;
	m_FontStyles.byScopeKeyword			= DEF_FSSCOPEKEYWORD;
	m_FontStyles.byString				= DEF_FSSTRING;
	m_FontStyles.byText					= DEF_FSTEXT;
	m_FontStyles.byNumber				= DEF_FSNUMBER;
	m_FontStyles.byTagText				= DEF_FSTAGTEXT;
	m_FontStyles.byTagEntity			= DEF_FSTAGENT;
	m_FontStyles.byTagElementName		= DEF_FSTAGELEMNAME;
	m_FontStyles.byTagAttributeName		= DEF_FSTAGATTRNAME;
	m_FontStyles.byLineNumber			= DEF_FSLINENUMBER;

	m_LineNum.bEnabled					= DEF_LINENUMENABLE;
	m_LineNum.nStartAt					= DEF_LINENUMBERSTART;
	m_LineNum.dwStyle                   = DEF_LINENUMBERSTYLE;
	m_nLineNumWidth						= DEF_LINENUMBERMINWIDTH;
	m_nLastEdittedLine					= -1;

	m_bShowLeftMargin = DEF_SHOWLEFTMARGIN;
	m_bPaintTimerSet = FALSE;
	m_pActiveView = NULL;

	m_nDelayPaint = 0;
	m_nSelTrans = 0;

	m_hImageList = NULL;

	*m_szLang = '\0';


	m_wCurrCmd = 0;

	ZeroMemory( &m_cmHotKeyPending, sizeof( m_cmHotKeyPending ) );

	// verify that all commands have an entry in the command array
	#ifdef _DEBUG
	{
	TCHAR szCmd[ max( CM_MAX_CMD_DESCRIPTION, CM_MAX_CMD_STRING ) + 1 ];
	for ( int i = CMD_FIRST; i <= CMD_LAST; i++ )
	{
		switch ( i )
		{
			case CMD_RESERVED3:
				break;
			default:
			{
				GetCommandString( ( WORD ) i, FALSE, szCmd, ARRAY_SIZE( szCmd ) );
				ASSERT( *szCmd );
				ASSERT( _tcslen( szCmd ) <= CM_MAX_CMD_STRING );
				GetCommandString( ( WORD ) i, TRUE, szCmd, ARRAY_SIZE( szCmd ) );
				ASSERT( *szCmd );
				ASSERT( _tcslen( szCmd ) <= CM_MAX_CMD_DESCRIPTION );
				break;
			}
		}
	}

	m_hWndTest = NULL;
	m_bSelfTest = FALSE;
	}
	#endif

#ifdef _ACTIVEX
	m_pEditX = NULL;
#endif

	SetMode( eIdle );
}

CEdit::~CEdit()
{
	SetMode( eDestructing );

	DeleteDisplayFonts();

	// no need to destroy m_hWnd.  CEdit will be destroyed only from within WM_NCDESTROY
	for ( int nView = 0; nView < m_nViews; nView++ )
	{
		delete m_Views[ nView ];
	}

	if ( m_pMacroRecordBuffer )
	{
		free( m_pMacroRecordBuffer );
	}

	ASSERT( !m_hDlgRecord );

#ifdef _ACTIVEX
	if ( m_pEditX )
		m_pEditX->SetEdit( NULL );
#endif

	SetMode( eIdle );
}

// FontProc is used to select a good default font for all windows
int CALLBACK FontProc( ENUMLOGFONT *pelf, NEWTEXTMETRIC *pntm, int nFontType, LOGFONT *plfOut )
{
	if ( !HAS_FLAG( pntm->tmPitchAndFamily, TMPF_FIXED_PITCH ) &&	// fixed-pitch
	     *pelf->elfFullName != _T('@') )	// not a vertical font
	{
		// found a fixed-pitch font!
		memcpy( plfOut, &pelf->elfLogFont, sizeof( LOGFONT ) );
		// keep searching if this isn't a TrueType font -- TrueType fonts look nicer,
		// but in a pinch raster fonts are ok.
		return ( nFontType != TRUETYPE_FONTTYPE );
	}
	return 1; // keep searching
}

int CEdit::g_nRegCount = 0;
TCHAR CEdit::g_UpperConv[ 256 ];
BYTE CEdit::g_CharFlags[ 256 ];

// Register() is called every time CMRegisterControl() is called.
BOOL CEdit::Register()
{
	if ( ++g_nRegCount == 1 )
	{
		OleInitialize( NULL );
		// check to see if a fixed size font is installed and set it as the default font.
		// if there are no fixed-size fonts installed, then CodeMax cannot run.
		LOGFONT lfSys;
		VERIFY( GetObject( GetStockObject( SYSTEM_FIXED_FONT ), sizeof( lfSys ), &lfSys ) );
		LOGFONT lf;
		ZeroMemory( &lf, sizeof( LOGFONT ) );
		lf.lfCharSet = lfSys.lfCharSet;
		lf.lfPitchAndFamily = lfSys.lfPitchAndFamily;
		LOGFONT lfOut;
		ZeroMemory( &lfOut, sizeof( LOGFONT ) );

		HDC hDC = GetDC( NULL );
		EnumFontFamiliesEx( hDC, &lf, ( FONTENUMPROC ) FontProc, ( LPARAM ) &lfOut, 0 );
		lfOut.lfHeight = -MulDiv( 11, GetDeviceCaps( hDC, LOGPIXELSY ), 72 );
		lfOut.lfWidth = 0;
		lfOut.lfWeight = FW_NORMAL;
		NormalizeLOGFONT( lf );
		ReleaseDC( NULL, hDC );

		if ( !*lfOut.lfFaceName )
		{
			// didn't find a fixed-pitch font, so use the sys fixed font
			lfOut = lfSys;
		}

		// create the global, default font
		g_hFontDefault = CreateFontIndirect( &lfOut );
		SetDefaultHotKeys();

		for ( int i = 0; i < CM_MAX_MACROS; i++ )
		{
			g_Macros[ i ] = NULL;
		}

		CMRegisterLanguage( CMLANG_CPP, NULL );
		CMRegisterLanguage( CMLANG_BASIC, NULL );
		CMRegisterLanguage( CMLANG_JAVA, NULL );
		CMRegisterLanguage( CMLANG_PASCAL, NULL );
		CMRegisterLanguage( CMLANG_SQL, NULL );
		CMRegisterLanguage( CMLANG_HTML, NULL );
		CMRegisterLanguage( CMLANG_XML, NULL );

		// calculate all upper case conversions and bit states for each character in the Ansi charset
		for ( int ch = 0; ch < 256; ch++ )
		{
			g_UpperConv[ ch ] = ( TCHAR ) ( DWORD ) CharUpper( ( LPTSTR ) ( DWORD ) ch );
			BYTE byFlags = 0;

			if ( ISALPHA_EXPR( ( BYTE ) ch ) )
				byFlags |= CHARFLAG_ALPHA;
			if ( ISALNUM_EXPR( ( BYTE ) ch ) )
				byFlags |= CHARFLAG_ALPHANUMERIC;
			if ( ISNUMERIC_EXPR( ( BYTE ) ch ) )
				byFlags |= CHARFLAG_NUMERIC;
			if ( ISSPACE_EXPR( ( BYTE ) ch ) )
				byFlags |= CHARFLAG_SPACE;
			if ( ISSYMBOLORPUNCT_EXPR( ( BYTE ) ch ) )
				byFlags |= CHARFLAG_SYMBOLORPUNCT;
			if ( ISEOLN_EXPR( ( BYTE ) ch ) )
				byFlags |= CHARFLAG_EOLN;
			if ( ISLEADBYTE_EXPR( ( BYTE ) ch ) )
				byFlags |= CHARFLAG_LEADBYTE;
			if ( ISTRAILBYTE_EXPR( ( BYTE ) ch ) )
				byFlags |= CHARFLAG_TRAILBYTE;
						
			g_CharFlags[ ch ] = byFlags;
				
		}

		// load cursors
		ASSERT( !g_hcurHSplit );		g_hcurHSplit = LoadCursor( hInstance, MAKEINTRESOURCE( IDR_HSPLIT ) );
		ASSERT( !g_hcurVSplit );		g_hcurVSplit = LoadCursor( hInstance, MAKEINTRESOURCE( IDR_VSPLIT ) );
		ASSERT( !g_hcurHVSplit );		g_hcurHVSplit = LoadCursor( hInstance, MAKEINTRESOURCE( IDR_HVSPLIT ) );
		ASSERT( !g_hcurRecord );		g_hcurRecord = LoadCursor( hInstance, MAKEINTRESOURCE( IDR_RECORD_MACRO ) );
		ASSERT( !g_hcurLeftMargin );	g_hcurLeftMargin = LoadCursor( hInstance, MAKEINTRESOURCE( IDR_LEFT_MARGIN ) );

		WNDCLASS wc;
		wc.style = CS_DBLCLKS | CS_GLOBALCLASS | CS_VREDRAW | CS_HREDRAW;
		wc.lpfnWndProc = ( WNDPROC )EditCtrlWndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = sizeof( LPVOID );
		wc.hInstance = NULL;
		wc.hIcon = NULL;
		wc.hCursor = LoadCursor( NULL, IDC_ARROW );
		wc.hbrBackground = NULL;
		wc.lpszMenuName = NULL;
		wc.lpszClassName = CODEMAXWNDCLASS;

		RegisterClass( &wc );
	}
	return TRUE;
}

// Unregister() is called every time CMUnregisterControl() is called.
void CEdit::Unregister()
{
	if ( g_nRegCount > 0 && --g_nRegCount == 0 )
	{
		OleUninitialize();
		g_nRegCount = 0;

		if ( g_hFontDefault )
		{
			DeleteObject( g_hFontDefault );
			g_hFontDefault = NULL;
		}

		if ( g_pHotKeys )
		{
			free( g_pHotKeys );
			g_pHotKeys = NULL;
			g_nHotKeyCount = g_nHotKeyAllocCount = 0;
		}

		if ( g_pRegCmds )
		{
			free( g_pRegCmds );
			g_pRegCmds = NULL;
			g_nRegCmdCount = g_nRegCmdAllocCount = 0;
		}

		for ( int i = 0; i < CM_MAX_MACROS; i++ )
		{
			LPBYTE pMacro = g_Macros[ i ];
			if ( pMacro )
			{
				free( pMacro );
			}
		}

		if ( g_pLanguages )
		{
			for ( int i = 0; i < g_nLanguageCount; i++ )
			{
				delete g_pLanguages[ i ];
			}
			free( g_pLanguages );
			g_pLanguages = NULL;
			g_nLanguageCount = 0;
		}

		ASSERT( g_hcurHSplit );		DestroyCursor( g_hcurHSplit );		g_hcurHSplit = NULL;
		ASSERT( g_hcurVSplit );		DestroyCursor( g_hcurVSplit );		g_hcurVSplit = NULL;
		ASSERT( g_hcurHVSplit );	DestroyCursor( g_hcurHVSplit );		g_hcurHVSplit = NULL;
		ASSERT( g_hcurRecord );		DestroyCursor( g_hcurRecord );		g_hcurRecord = NULL;
		ASSERT( g_hcurLeftMargin );	DestroyCursor( g_hcurLeftMargin );	g_hcurLeftMargin = NULL;
		
		UnregisterClass( CODEMAXWNDCLASS, NULL );
	}
}

CEditView *CEdit::GetActiveView() const
{
	return m_pActiveView;
}

// SetActiveView() is called when the user clicks on a different splitter pane
void CEdit::SetActiveView( CEditView *pView )
{
	if ( pView != m_pActiveView )
	{
		CEditView *pOldView = m_pActiveView;
		m_pActiveView = pView;
		m_Selection.SetView( pView );

		if ( pOldView )
		{
			CMTRACE0( _T("CMN_VIEWCHANGE\r\n") );
			NotifyParent( CMN_VIEWCHANGE );
		}
	}
}

// InitializeWindow() is called during WM_CREATE
void CEdit::InitializeWindow( LPCREATESTRUCT lpcs )
{
	ASSERT( ::IsWindow( m_hWnd ) );

	m_dwStyle = lpcs->style;
	m_hbmPaint = NULL;
	m_hdcPaint = NULL;

	lpcs->style = ( ( lpcs->style & ~( WS_VSCROLL | WS_HSCROLL ) ) | WS_CLIPCHILDREN );
	SetWindowLong( m_hWnd, GWL_STYLE, lpcs->style );

	// can't show splitter unless adjacent scrollbar is visible
	if ( !ShowHScrollBar() )
	{
		m_bAllowHSplit = FALSE;
	}

	if ( !ShowVScrollBar() )
	{
		m_bAllowVSplit = FALSE;
	}

	// setup drop target support
   	m_DropTarget.SetCtrl( this );
	m_DropTarget.AddRef();
	CoLockObjectExternal( ( LPUNKNOWN ) &m_DropTarget, TRUE, FALSE );
	VERIFY( S_OK == RegisterDragDrop( m_hWnd, &m_DropTarget ) );

	// create the initial view
	m_nViews = 1;
	CEditView *pView = new CEditView( this, &m_Buffer );
	m_Views[ 0 ] = pView;

	// initialize the selection
	m_Selection.Initialize( this, pView );

	OnFontChanged();

	SetActiveView( pView );

	// tell Windows to recalc the client area, in case the WS_VSCROLL or WS_HSCROLL bits were passed in
	// to CreateWindow().  Since we handle the scroll styles -- not Windows, we don't want Windows to think
	// it has control of the scrollbars.
	SetWindowPos( m_hWnd, NULL, -1, -1, -1, -1, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_DRAWFRAME );
}

// ReplaceSelection() is the one-and-only place where text is added to the buffer.  Everything goes thru this
// method.
void CEdit::ReplaceSelection( LPCTSTR pszText, BOOL bExtendIfEmpty, BOOL bEnsureVisible, BOOL bSelectWhenDone )
{
	int nStartCol, nStartRow, nEndCol, nEndRow;
	m_Selection.GetNormalizedBufferSelection( nStartCol, nStartRow, nEndCol, nEndRow );

	m_Buffer.BeginEdit( nEndRow, nEndCol );
	DeleteSelection( bExtendIfEmpty, FALSE );

	ASSERT( m_Selection.IsEmpty() );

	if ( !m_Buffer.GetLineCount() )
	{
		ASSERT( nStartRow == 0 );
		m_Buffer.InsertLine( 0, _T(""), 0 );
	}

	// Extend line with spaces if neccessary
	m_Buffer.EnsureLineLength( nStartRow, nStartCol );

	register LPCTSTR psz = pszText;
	register LPCTSTR pszStartOfLine;
	int nLine = nStartRow;
	LPTSTR pszSuffix = NULL;
	int cbSuffix = 0;

	for ( ;; )
	{
		pszStartOfLine = psz;
		// tolerate \n, \r, \r\n, and \r\r\n
		while ( *psz &&
		        !( ( *psz == _T('\r') ) && ( psz[ 1 ] == _T('\n') ) ) &&
		        !( ( *psz == _T('\r') ) && ( psz[ 1 ] == _T('\r') ) && ( psz[ 2 ] == _T('\n') ) ) &&
				!( ( *psz == _T('\r') ) && ( psz[ 1 ] != _T('\r') ) && ( psz[ 1 ] != _T('\n') ) ) &&
				( *psz != _T('\n') ) )
		{
			psz++;
		}
		if ( *psz == _T('\0') )
		{
			nEndRow = nLine;
			int cbText = ( psz - pszStartOfLine );
			if ( nLine == nStartRow )
			{
				m_Buffer.InsertCharsIntoLine( nStartRow, nStartCol, pszStartOfLine, cbText );
				nEndCol = nStartCol + cbText;
			}
			else
			{
				m_Buffer.InsertLine( nLine, pszStartOfLine, cbText );
	
				nEndCol = cbText;
				if ( pszSuffix )
				{
					m_Buffer.InsertCharsIntoLine( nLine, m_Buffer.GetLineLength( nLine ), pszSuffix, cbSuffix );
				}
			}

			m_Selection.SetEmptySelection( nEndCol, nLine );
			break;
		}
		else
		{
			if ( nLine == nStartRow )
			{
				// remember the suffix portion will be placed in a new line later
				ASSERT( !pszSuffix );
				int nTemp = m_Buffer.GetLineLength( nStartRow ) - nStartCol;
				cbSuffix = max( 0, nTemp );
				if ( cbSuffix > 0 )
				{
					pszSuffix = new TCHAR[ cbSuffix + 1 ];
					_tcsncpy( pszSuffix, m_Buffer.GetLineText( nStartRow ) + nStartCol, cbSuffix );
					pszSuffix[ cbSuffix ] = _T('\0');
					m_Buffer.RemoveCharsFromLine( nStartRow, nStartCol, cbSuffix );
				}

				if ( psz > pszStartOfLine )
				{
					m_Buffer.InsertCharsIntoLine( nStartRow, nStartCol, pszStartOfLine, psz - pszStartOfLine );
				}
				nLine++;
			}
			else
			{
				m_Buffer.InsertLine( nLine++, pszStartOfLine, psz - pszStartOfLine );
			}
			
			if ( *psz == _T('\n') )
			{
				psz++;
			}
			else
			{
				psz++;									// \r or \n
				switch ( *psz )
				{
					case _T('\n'):	psz++; break;		// \r\n
					case _T('\r'):	psz += 2; break;	// \r\r\n
				}
			}
		}
		pszStartOfLine = psz;
	}

	if ( pszSuffix )
	{
		delete pszSuffix;
	}

	m_Buffer.EndEdit( nEndRow, nEndCol );
	
	if ( bSelectWhenDone )
	{
		m_Selection.SetExtendedSelection( nStartCol, nStartRow, nEndCol, nEndRow, FALSE, TRUE );
	}
	
	if ( bEnsureVisible )
	{
		m_Selection.EnsureVisible( TRUE );
	}
}

// DeleteSelection() deletes the currently selected text, or optionally the next character after the cursor
void CEdit::DeleteSelection( BOOL bExtendIfEmpty, BOOL bAllowConsumption )
{
	if ( !m_Buffer.GetLineCount() )
	{
		return;
	}


	m_Buffer.BeginEdit( m_Selection.GetEndRow(), m_Selection.GetEndCol() );

	int nStartCol, nStartRow;

	if ( m_Selection.IsColumnSel() )
	{
		///////////////////////////////////////////////////////
		// Column selection
		///////////////////////////////////////////////////////
		int nDontCare;
		m_Selection.GetNormalizedBufferSelection( nStartCol, nStartRow, nDontCare, nDontCare );
		int nViewStartCol, nViewStartRow, nViewEndCol, nViewEndRow;
		m_Selection.GetNormalizedViewSelection( nViewStartCol, nViewStartRow, nViewEndCol, nViewEndRow );
		if ( nViewEndCol < nViewStartCol )
		{
			// further normalization
			int nTemp = nViewEndCol;
			nViewEndCol = nViewStartCol;
			nViewStartCol = nTemp;
		}

		// delete the chars from each line
		for ( int nLine = nViewStartRow; nLine <= nViewEndRow; nLine++ )
		{
			int nBuffStartCol = m_Buffer.ConvertViewColToBufferCol( nLine, nViewStartCol );			
			int nBuffEndCol = m_Buffer.ConvertViewColToBufferCol( nLine, nViewEndCol );
			if ( bExtendIfEmpty && ( nViewEndCol == nViewStartCol ) )
			{
				nBuffEndCol++;
			}

			int nLineLen = m_Buffer.GetLineLength( nLine );
			if ( nBuffStartCol < nLineLen )
			{
				nBuffEndCol = min( nBuffEndCol, nLineLen );
				if ( nBuffEndCol > nBuffStartCol )
				{
					m_Buffer.RemoveCharsFromLine( nLine, nBuffStartCol, nBuffEndCol - nBuffStartCol );
				}
			}
		}
	}
	else
	{
		///////////////////////////////////////////////////////
		// Regular selection
		///////////////////////////////////////////////////////
		BOOL bEmptySel = m_Selection.IsEmpty();

		if ( bExtendIfEmpty && bEmptySel )
		{
			m_Selection.Extend( CSelection::eRight, CSelection::eChar, FALSE, TRUE, FALSE );
		}

		int nEndCol, nEndRow;
		m_Selection.GetNormalizedBufferSelection( nStartCol, nStartRow, nEndCol, nEndRow );

		if ( nStartRow == nEndRow )
		{
			int nLineLength = m_Buffer.GetLineLength( nStartRow );
			int nLastChar = ( nLineLength - 1 );
			BOOL bConsumeNextLine = ( bAllowConsumption && bEmptySel && ( nStartCol > nLastChar ) && ( ( nStartRow + 1 ) <  m_Buffer.GetLineCount() ) );
			// remove the selection
			if ( nStartCol < nLineLength )
			{
				m_Buffer.RemoveCharsFromLine( nStartRow, nStartCol, nEndCol - nStartCol );
			}

			// consume the crlf and move the next line up if neccessary
			if ( bConsumeNextLine )
			{
				if ( bEmptySel && nLineLength == 0 && nStartCol == 0 )
				{
					// delete the line rather than consume the next line so that
					// caller receives reasonable CMN_DELETELINE notifications
					// where the line number is more intuitive.
					m_Buffer.RemoveLines( nStartRow, 1 );
				}
				else
				{
					m_Buffer.EnsureLineLength( nStartRow, nStartCol );
					m_Buffer.InsertCharsIntoLine( nStartRow, nStartCol, m_Buffer.GetLineText( nStartRow + 1 ) );
					m_Buffer.RemoveLines( nStartRow + 1, 1 );
				}
			}
		}
		else
		{
			if ( nEndRow < m_Buffer.GetLineCount() ) 
			{
				if ( nStartCol == 0 )
				{
					// delete the lines the user sees rather than do some fancy shifting below
					// so that the caller receives reasonable CMN_DELETELINE notifications
					// where the line number is more intuitive.
					int nEndRowLen = m_Buffer.GetLineLength( nEndRow );
					int cbRemove = min( nEndRowLen, nEndCol );
					if ( cbRemove > 0 )
					{
						m_Buffer.RemoveCharsFromLine( nEndRow, 0, cbRemove );
					}
					m_Buffer.RemoveLines( nStartRow, nEndRow - nStartRow );
				}
				else
				{
					int cbRemove = m_Buffer.GetLineLength( nStartRow ) - nStartCol;
					if ( cbRemove > 0 )
					{
						m_Buffer.RemoveCharsFromLine( nStartRow, nStartCol, cbRemove );
					}
					if ( nEndCol < m_Buffer.GetLineLength( nEndRow ) )
					{
						m_Buffer.EnsureLineLength( nStartRow, nStartCol );
						m_Buffer.InsertCharsIntoLine( nStartRow, nStartCol, m_Buffer.GetLineText( nEndRow ) + nEndCol );
					}
					m_Buffer.RemoveLines( nStartRow + 1, nEndRow - nStartRow );
				}
			}
		}
	}

	m_Selection.SetEmptySelection( nStartCol, nStartRow );
	m_Buffer.EndEdit( nStartRow, nStartCol );
}

// ChangeCaseOfSelection() toggles upper/lower case of a paragraph selection of text
void CEdit::ChangeCaseOfSelection( BOOL bUpper )
{
	if ( m_Selection.IsEmpty() )
		return;

	int nDontCare, nEndColAtStart, nEndRowAtStart;
	m_Selection.GetBufferSelection( nDontCare, nDontCare, nEndColAtStart, nEndRowAtStart );

	m_Buffer.BeginEdit( nEndRowAtStart, nEndColAtStart );

	if ( m_Selection.IsColumnSel() )
	{
		///////////////////////////////////////////////////////
		// Column selection
		///////////////////////////////////////////////////////
		int nStartCol, nStartRow, nDontCare;
		m_Selection.GetNormalizedBufferSelection( nStartCol, nStartRow, nDontCare, nDontCare );
		int nViewStartCol, nViewStartRow, nViewEndCol, nViewEndRow;
		m_Selection.GetNormalizedViewSelection( nViewStartCol, nViewStartRow, nViewEndCol, nViewEndRow );
		if ( nViewEndCol < nViewStartCol )
		{
			// further normalization
			int nTemp = nViewEndCol;
			nViewEndCol = nViewStartCol;
			nViewStartCol = nTemp;
		}

		// preserve the selection
		CSaveSelection save( this );
		BOOL bColumnSelSave = m_Selection.IsColumnSel();
		m_Selection.EnableColumnSel( FALSE, TRUE );

		// delete the chars from each line
		for ( int nLine = nViewStartRow; nLine <= nViewEndRow; nLine++ )
		{
			int nBuffStartCol = m_Buffer.ConvertViewColToBufferCol( nLine, nViewStartCol );			
			int nBuffEndCol = m_Buffer.ConvertViewColToBufferCol( nLine, nViewEndCol );
			int nLineLen = m_Buffer.GetLineLength( nLine );
			if ( nViewEndCol != nViewStartCol &&
			     nBuffStartCol < nLineLen &&
				 nBuffStartCol != nBuffEndCol )
			{
				m_Selection.SetExtendedSelection( nBuffStartCol, nLine, nBuffEndCol, nLine, FALSE, FALSE );
				ChangeCaseOfSelection( bUpper );
			}
		}
		m_Selection.EnableColumnSel( bColumnSelSave, TRUE );
	}
	else
	{
		int nStartCol, nStartRow, nEndCol, nEndRow;
		m_Selection.GetBufferSelection( nStartCol, nStartRow, nEndCol, nEndRow );
		HGLOBAL hMem = GetSelectionText();
		if ( hMem )
		{
			LPTSTR pszText = ( LPTSTR ) GlobalLock( hMem );
			if ( pszText && *pszText )
			{
				if ( bUpper )
				{
					CharUpper( pszText );
				}
				else
				{
					CharLower( pszText );
				}
				ReplaceSelection( pszText, FALSE );
				m_Selection.SetExtendedSelection( nStartCol, nStartRow, nEndCol, nEndRow );
			}
			GlobalUnlock( hMem );
			GlobalFree( hMem );
		}
	}

	m_Buffer.EndEdit( nEndRowAtStart, nEndColAtStart );
}

// BufferCallback() is the one link the buffer has to the UI.  All communication from the buffer
// to the control goes thru this static method.
void CEdit::BufferCallback( LPARAM lParam, DWORD dwNotification, int nLine, int nChar, int nCount )
{
	CEdit *pEdit = ( CEdit * )lParam;
	ASSERT( pEdit );

	pEdit->OnBufferNotification( dwNotification, nLine, nChar, nCount );
}

void CEdit::OnBufferNotification( DWORD dwNotification, register int nLine, int nChar, int nCount )
{
	switch ( dwNotification )
	{
		case CBuffer::eBeginEdit: // an edit action is just starting
		{
			if ( !m_Buffer.IsInEditTransaction() )
			{
				// take a snapshot of the current state for comparison later
				m_stateBegin.m_nChangeLevel = m_Buffer.GetChangeLevel();
				m_stateBegin.m_bModified = m_Buffer.IsModified();
			}

			// notify all views
			for ( int i = 0; i < m_nViews; i++ )
			{
				m_Views[ i ]->OnBeginEdit();
			}
			break;
		}

		case CBuffer::eEndEdit:  // an edit action is ending
		{
			// notify all views
			for ( int i = 0; i < m_nViews; i++ )
			{
				m_Views[ i ]->OnEndEdit();
			}

			// if this is the outer edit transaction, issue notifications
			if ( !m_Buffer.IsInEditTransaction() )
			{
				if ( m_Buffer.ChangedSilently() ||
				     ( m_Buffer.GetChangeLevel() != m_stateBegin.m_nChangeLevel ) )
				{
					CMTRACE0( _T("CMN_CHANGE\r\n") );
					NotifyParent( CMN_CHANGE );
				}

				if ( m_Buffer.IsModified() != m_stateBegin.m_bModified )
				{
					CMTRACE0( _T("CMN_MODIFIEDCHANGE\r\n") );
					NotifyParent( CMN_MODIFIEDCHANGE );
				}
			}
			break;
		}

		case CBuffer::eLineChanged:  // a line in the buffer changed.  Damage the minimum number of lines
		{
			m_nLastEdittedLine = nLine;
			// fall through
		}
		case CBuffer::eLineChangedSilent:  // a line in the buffer changed silenty (i.e. The user didn't make the change)
		{
			DamageAllViews( nLine, nLine + nCount - 1 );
			break;
		}

		case CBuffer::eLineRemoved: // a line was removed.
		{
			// notify the parent window so it can clean up any per-line allocated memory
			int nNotifications = m_Buffer.GetNotifyDelCount();
			for ( int i = 0; nNotifications && i < nCount; i++ )
			{
				int nLineDel = nLine + i;
				if ( HAS_FLAG( m_Buffer.GetLineStyle( nLineDel ), CML_NOTIFY_DEL ) )
				{
					nNotifications--;
					CM_DELETELINEDATA dld;
					dld.nLine = nLineDel;
					dld.lParam = m_Buffer.GetItemData( nLineDel );
					NotifyParent( CMN_DELETELINE, ( NMHDR * ) &dld );
				}
			}

			// fixup the highlighted line index accordingly
			if (nLine <= m_nHighlightedLine)
				{
				// assume it has been deleted
				int nNewHighlightedLine = -1;
				// if not deleted, shift the index up
				if (nLine + nCount <= m_nHighlightedLine)
					nNewHighlightedLine = m_nHighlightedLine - nCount;
				
				SetHighlightedLine(nNewHighlightedLine, TRUE);
				}
			
			// fall through...
		}
		case CBuffer::eLineAdded:	// a line was added
		{
			// Damage all visible lines below the line
			// or,
			// if line numbers visible and need more horz room for numbers,
			// damage the entire view so that prev lines will be repainted
			// because of the shift.
			if ( m_LineNum.bEnabled )
			{
				// calculate the new line count -- CBuffer::GetLineCount() reports a stale value.
				int nNewLineCount = m_Buffer.GetLineCount() +
				                    ( ( dwNotification == CBuffer::eLineRemoved ) ? -nCount : 0 );
				int nLogNew = _log( nNewLineCount + m_LineNum.nStartAt - 1, m_LineNum.dwStyle );
				nLogNew = max( nLogNew, DEF_LINENUMBERMINWIDTH );
				if ( nLogNew != GetLineNumWidth() )
				{
					SetLineNumWidth( nLogNew );
					nLine = 0;
				}
			}

			if (dwNotification == CBuffer::eLineAdded)
				{
				// fixup the highlighted line index accordingly
				if (nLine <= m_nHighlightedLine)
					{
					// if not deleted, shift the index up
					if (nLine <= m_nHighlightedLine)
						SetHighlightedLine(m_nHighlightedLine + nCount, TRUE);
					}
				}

			DamageAllViews( nLine, CEditView::MAXROW );
			break;
		}

		case CBuffer::eBeginUndo:  // undo or redo was started
		case CBuffer::eBeginRedo:
		{
			break;
		}

		case CBuffer::eEndUndo:  // undo or redo ended
		case CBuffer::eEndRedo:
		{
			m_Selection.EnsureVisible( TRUE );
			break;
		}

		case CBuffer::eSetCursorPos: // undo or redo wants to restore the cursor to the previous pos
		{
			m_Selection.SetEmptySelection( nChar, nLine );
			break;
		}
		
		#ifdef _DEBUG
		default:
		{
			// need to write this handler!
			ASSERT( FALSE );
			break;
		}
		#endif
	}
}

// TranslateHotKey() converts a hotkey into a CodeMax command and executes the command.
BOOL CEdit::TranslateHotKey( WPARAM wParam )
{
	WORD wVirtKey = LOWORD( wParam );
	if ( wVirtKey == VK_SHIFT || wVirtKey == VK_CONTROL || wVirtKey == VK_MENU )
	{
		return FALSE;
	}

	BYTE byModifiers = 0;
	byModifiers |= ( ( GetKeyState( VK_CONTROL ) < 0 ) ? HOTKEYF_CONTROL : 0 );
	byModifiers |= ( ( GetKeyState( VK_SHIFT ) < 0 ) ? HOTKEYF_SHIFT : 0 );
	byModifiers |= ( ( GetKeyState( VK_MENU ) < 0 ) ? HOTKEYF_ALT : 0 );
	CM_HOTKEY cmHotKey = { byModifiers, wVirtKey, 0, 0 };

	BOOL bProcessed = FALSE;

	WORD wCmd;
	int nDontCare;

	if ( m_cmHotKeyPending.byModifiers1 || m_cmHotKeyPending.nVirtKey1 )
	{
		CM_HOTKEY cmCompositeHotKey = m_cmHotKeyPending;
		cmCompositeHotKey.byModifiers2 = cmHotKey.byModifiers1;
		cmCompositeHotKey.nVirtKey2 = cmHotKey.nVirtKey1;
		NormalizeHotKey( cmCompositeHotKey );
		if ( LookupHotKey( cmCompositeHotKey, wCmd, nDontCare ) )
		{
			bProcessed = TRUE;
		}
		else
		{
			bProcessed = LookupHotKey( cmHotKey, wCmd, nDontCare );
		}
		if ( bProcessed )
		{
			ZeroMemory( &m_cmHotKeyPending, sizeof( m_cmHotKeyPending ) );
		}
		else
		{
			m_cmHotKeyPending = cmHotKey;
		}
	}
	else
	{
		bProcessed = LookupHotKey( cmHotKey, wCmd, nDontCare );
	}


	if ( bProcessed )
	{
		ExecuteCommand( wCmd, 0, TRUE );
	}
	else
	{
		m_cmHotKeyPending = cmHotKey;		
	}

	return bProcessed;
}

// TranslateChar() is like TranslateHotKey(), except that all characters always are inserted raw with
// the CMD_NEWLINE or CMD_INSERTCHAR commands, and are not modifiable by the user.
BOOL CEdit::TranslateChar( TCHAR ch )
{
	if ( ch == _T('\n') )
	{
		ExecuteCommand( CMD_NEWLINE, 0, TRUE );
	}
	else
	{
		// make an attempt to map to a font that supports the current keyboard
		// locale.
		RemapAnsiFontToKeyboardLocale();
		ExecuteCommand( CMD_INSERTCHAR, ( DWORD ) ch, FALSE );
	}

	return TRUE;
}

// ExecuteCommand() executes a CodeMax command.  This method is called during user typing, macro record and playback, or 
// by the CMM_EXECUTECMD message.
void CEdit::ExecuteCommand( WORD wCmd, DWORD dwData, BOOL bApplyDefaultParam, BOOL bForcePersistInMacro )
{
	if ( ShowHourGlass() )
	{
		// cannot do anything
		return;
	}

	// save the current command context
	WORD wSaveCmd = m_wCurrCmd;
	m_wCurrCmd = wCmd;

	if ( IsRecordingMacro() )
	{
		// macros cannot call other macros (gotta prevent infinite looping!)
		switch ( wCmd )
		{
			default:
				// don't record the cmd id if called from within another cmd proc.  This
				// is essential because the calling cmd proc has effectively hardcoded
				// this information by virtual of it's own code.
				if ( m_nCmdNestLevel == 0 || bForcePersistInMacro )
					AddMacroData( ( LPBYTE) &wCmd, sizeof( wCmd ) );
				break;
			case CMD_RECORDMACRO:
			case CMD_PLAYMACRO1:case CMD_PLAYMACRO2:case CMD_PLAYMACRO3:case CMD_PLAYMACRO4:case CMD_PLAYMACRO5:
			case CMD_PLAYMACRO6:case CMD_PLAYMACRO7:case CMD_PLAYMACRO8:case CMD_PLAYMACRO9:case CMD_PLAYMACRO10:
				break;
		}
	}

	// keep track of the nesting level of commands so macros can serialize data correcty
	m_nCmdNestLevel++;

	// Repeat the command the requested number of times
	int nRepeatCount = m_nRepeatCount;
	m_nRepeatCount = 1;
	for ( int i = 0; i < nRepeatCount; i++ )
	{
		if ( m_bAbortMacro && nRepeatCount > 1 )
		{
			m_bAbortMacro = FALSE;
			break;
		}
		CSelTransaction trans( this );

		if ( wCmd >= CMD_USER_BASE )
		{
			if ( IsRegisteredCommand( wCmd ) )
			{
				// This is a registered command -- notify the parent to perform the 
				// custom action.
				ExecuteRegisteredCommand( wCmd );
			}
		}
		else
		{
			switch ( wCmd )
			{
				case CMD_WORDRIGHTEXTEND:				WordRightExtend(); break;
				case CMD_WORDRIGHT:						WordRight(); break;
				case CMD_WORDENDRIGHT:					WordEndRight(); break;
				case CMD_WORDENDRIGHTEXTEND:			WordEndRightExtend(); break;
				case CMD_WORDLEFT:						WordLeft(); break;
				case CMD_WORDENDLEFT:					WordEndLeft(); break;
				case CMD_WORDENDLEFTEXTEND:				WordEndLeftExtend(); break;
				case CMD_WORDLEFTEXTEND:				WordLeftExtend(); break;
				case CMD_WINDOWSTART:					WindowStart(); break;
				case CMD_WINDOWSCROLLUP:				WindowScrollUp(); break;
				case CMD_WINDOWSCROLLTOTOP:				WindowScrollToTop(); break;
				case CMD_WINDOWSCROLLTOCENTER:			WindowScrollToCenter(); break;
				case CMD_WINDOWSCROLLTOBOTTOM:			WindowScrollToBottom(); break;
				case CMD_WINDOWSCROLLRIGHT:				WindowScrollRight(); break;
				case CMD_WINDOWSCROLLLEFT:				WindowScrollLeft(); break;
				case CMD_WINDOWSCROLLDOWN:				WindowScrollDown(); break;
				case CMD_WINDOWRIGHTEDGE:				WindowRightEdge(); break;
				case CMD_WINDOWLEFTEDGE:				WindowLeftEdge(); break;
				case CMD_WINDOWEND:						WindowEnd(); break;
				case CMD_SENTENCERIGHT:					SentenceRight(); break;
				case CMD_SENTENCELEFT:					SentenceLeft(); break;
				case CMD_SENTENCECUT:					SentenceCut(); break;
				case CMD_SELECTSWAPANCHOR:				SelectSwapAnchor(); break;
				case CMD_SELECTPARA:					SelectPara(); break;
				case CMD_SELECTLINE:					SelectLine(); break;
				case CMD_SELECTALL:						SelectAll(); break;
				case CMD_PARAUP:						ParaUp(); break;
				case CMD_PARADOWN:						ParaDown(); break;
				case CMD_PAGEUPEXTEND:					PageUpExtend(); break;
				case CMD_PAGEUP:						PageUp(); break;
				case CMD_PAGEDOWNEXTEND:				PageDownExtend(); break;
				case CMD_PAGEDOWN:						PageDown(); break;
				case CMD_LINEUPEXTEND:					LineUpExtend(); break;
				case CMD_LINEUP:						LineUp(); break;
				case CMD_LINESTART:						LineStart(); break;
				case CMD_LINEENDEXTEND:					LineEndExtend(); break;
				case CMD_LINEEND:						LineEnd(); break;
				case CMD_LINEDOWNEXTEND:				LineDownExtend(); break;
				case CMD_LINEDOWN:						LineDown(); break;
				case CMD_HOMEEXTEND:					HomeExtend(); break;
				case CMD_HOME:							Home(); break;
				case CMD_GOTOMATCHBRACE:				GoToMatchBrace(); break;
				case CMD_GOTOINDENTATION:				GoToIndentation(); break;
				case CMD_GOTOLINE:						GoToLine( bApplyDefaultParam ? -1 : ( int ) dwData ); break;
				case CMD_FINDPREVWORD:					FindPrevWord(); break;
				case CMD_FINDPREV:						FindPrev(); break;
				case CMD_FINDNEXTWORD:					FindNextWord(); break;
				case CMD_FINDNEXT:						FindNext(); break;
				case CMD_FIND:							Find(); break;
				case CMD_FINDMARKALL:					FindMarkAll(); break;
				case CMD_SETFINDTEXT:					SetFindText( ( LPCTSTR ) dwData ); break;
				case CMD_END:							End(); break;
				case CMD_SETREPEATCOUNT:			    SetRepeatCount( bApplyDefaultParam ? 0 : ( int ) dwData ); break;
				case CMD_TOGGLEWHITESPACEDISPLAY:	    ToggleWhitespaceDisplay(); break;
				case CMD_TOGGLEWHOLEWORD:				ToggleWholeWord(); break;
				case CMD_TOGGLEPRESERVECASE:			TogglePreserveCase(); break;
				case CMD_TOGGLECASESENSITIVE:		    ToggleCaseSensitive(); break;
				case CMD_TOGGLEREGEXP:				    ToggleRegExp(); break;
				case CMD_DOCUMENTSTARTEXTEND:			DocumentStartExtend(); break;
				case CMD_DOCUMENTSTART:					DocumentStart(); break;
				case CMD_DOCUMENTENDEXTEND:				DocumentEndExtend(); break;
				case CMD_DOCUMENTEND:					DocumentEnd(); break;
				case CMD_COPY:							Copy(); break;
				case CMD_CHARRIGHTEXTEND:				CharRightExtend(); break;
				case CMD_CHARRIGHT:						CharRight(); break;
				case CMD_CHARLEFTEXTEND:				CharLeftExtend(); break;
				case CMD_CHARLEFT:						CharLeft(); break;
				case CMD_BOOKMARKTOGGLE:				BookmarkToggle(); break;
				case CMD_BOOKMARKPREV:					BookmarkPrev(); break;
				case CMD_BOOKMARKNEXT:					BookmarkNext(); break;
				case CMD_BOOKMARKCLEARALL:				BookmarkClearAll(); break;
				case CMD_BOOKMARKJUMPTOFIRST:			BookmarkJumpToFirst(); break;
				case CMD_BOOKMARKJUMPTOLAST:			BookmarkJumpToLast(); break;
				case CMD_PROPERTIES:					Properties(); break;
				case CMD_BEGINUNDO:						BeginUndo(); break;
				case CMD_ENDUNDO:						EndUndo(); break;
				case CMD_CLEARSELECTION:				ClearSelection(); break;
				case CMD_REGEXPON:						RegExpOn(); break;
				case CMD_REGEXPOFF:						RegExpOff(); break;
				case CMD_WHOLEWORDON:					WholeWordOn(); break;
				case CMD_WHOLEWORDOFF:					WholeWordOff(); break;
				case CMD_PRESERVECASEON:				PreserveCaseOn(); break;
				case CMD_PRESERVECASEOFF:				PreserveCaseOff(); break;
				case CMD_CASESENSITIVEON:				CaseSensitiveOn(); break;
				case CMD_CASESENSITIVEOFF:				CaseSensitiveOff(); break;
				case CMD_WHITESPACEDISPLAYON:			WhitespaceDisplayOn(); break;
				case CMD_WHITESPACEDISPLAYOFF:			WhitespaceDisplayOff(); break;
				case CMD_OVERTYPEON:					OvertypeOn(); break;
				case CMD_OVERTYPEOFF:					OvertypeOff(); break;
				#ifdef _DEBUG
				case CMD_SELFTEST:						DoSelfTest(); break;
				#else
				case CMD_SELFTEST:						break;
				#endif

				default:
				{
					// disallow commands that would violate the read-only mode
					if ( !IsEnabled() || m_Buffer.IsReadOnly() )
					{
						// Notify the user
						if ( IsPlayingMacro() || IsRecordingMacro() )
						{
							m_bAbortMacro = TRUE;
						}
						else
						{
							NotifyParentOfCmdFailure( CMDERR_READONLY );
						}
						i = nRepeatCount; // break out of the for-loop
					}
					else
					{
						switch ( wCmd )
						{
							case CMD_WORDUPPERCASE:					WordUpperCase(); break;
							case CMD_WORDTRANSPOSE:					WordTranspose(); break;
							case CMD_WORDLOWERCASE:					WordLowerCase(); break;
							case CMD_WORDDELETETOSTART:				WordDeleteToStart(); break;
							case CMD_WORDDELETETOEND:				WordDeleteToEnd(); break;
							case CMD_WORDCAPITALIZE:				WordCapitalize(); break;
							case CMD_UPPERCASESELECTION:			UpperCaseSelection(); break;
							case CMD_UNTABIFYSELECTION:				UntabifySelection(); break;
							case CMD_UNINDENTSELECTION:				UnindentSelection(); break;
							case CMD_UNDOCHANGES:					UndoChanges(); break;
							case CMD_UNDO:							Undo(); break;
							case CMD_TABIFYSELECTION:				TabifySelection(); break;
							case CMD_REDOCHANGES:					RedoChanges(); break;
							case CMD_REDO:							Redo(); break;
							case CMD_PASTE:							Paste(); break;
							case CMD_LOWERCASESELECTION:			LowerCaseSelection(); break;
							case CMD_LINETRANSPOSE:					LineTranspose(); break;
							case CMD_LINEOPENBELOW:					LineOpenBelow(); break;
							case CMD_LINEOPENABOVE:					LineOpenAbove(); break;
							case CMD_LINEDELETETOSTART:				LineDeleteToStart(); break;
							case CMD_LINEDELETETOEND:				LineDeleteToEnd(); break;
							case CMD_LINEDELETE:					LineDelete(); break;
							case CMD_LINECUT:						LineCut(); break;
							case CMD_INDENTTOPREV:					IndentToPrev(); break;
							case CMD_INDENTSELECTION:				IndentSelection(); break;
							case CMD_FINDREPLACE:					FindReplace(); break;
							case CMD_REPLACE:						Replace(); break;
							case CMD_REPLACEALLINSELECTION:			ReplaceAllInSelection(); break;
							case CMD_REPLACEALLINBUFFER:			ReplaceAllInBuffer(); break;
							case CMD_SETREPLACETEXT:				SetReplaceText( ( LPCTSTR ) dwData ); break;
							case CMD_TOGGLEOVERTYPE:			    ToggleOvertype(); break;
							case CMD_DELETEHORIZONTALSPACE:			DeleteHorizontalSpace(); break;
							case CMD_DELETEBLANKLINES:				DeleteBlankLines(); break;
							case CMD_DELETEBACK:					DeleteBack(); break;
							case CMD_DELETE:						Delete(); break;
							case CMD_CUTSELECTION:					CutSelection(); break;
							case CMD_CUT:							Cut( FALSE ); break;
							case CMD_CHARTRANSPOSE:					CharTranspose(); break;
							case CMD_APPENDNEXTCUT:					AppendNextCut(); break;
							case CMD_INSERTCHAR:					InsertChar( ( TCHAR ) dwData ); break;
							case CMD_NEWLINE:						NewLine(); break;
							case CMD_RECORDMACRO:					RecordMacro( ( BOOL ) dwData ); break;
							case CMD_PLAYMACRO1:					PlayMacro( 0 ); break;
							case CMD_PLAYMACRO2:					PlayMacro( 1 ); break;
							case CMD_PLAYMACRO3:					PlayMacro( 2 ); break;
							case CMD_PLAYMACRO4:					PlayMacro( 3 ); break;
							case CMD_PLAYMACRO5:					PlayMacro( 4 ); break;
							case CMD_PLAYMACRO6:					PlayMacro( 5 ); break;
							case CMD_PLAYMACRO7:					PlayMacro( 6 ); break;
							case CMD_PLAYMACRO8:					PlayMacro( 7 ); break;
							case CMD_PLAYMACRO9:					PlayMacro( 8 ); break;
							case CMD_PLAYMACRO10:					PlayMacro( 9 ); break;
							#if 0
							default:								ASSERT( FALSE ); break;
							#endif
						}
					}
				}
			}
		}
	}

	// un-pop the nesting level
	m_nCmdNestLevel--;

	// restore the last current command (if nested)
	m_wCurrCmd = wSaveCmd;
}

void CEdit::ExecuteRegisteredCommand( WORD wCmd ) const
{
	CM_REGISTEREDCMDDATA rcd;
	rcd.wCmd = wCmd;
	NotifyParent( CMN_REGISTEREDCMD, ( NMHDR * ) &rcd );
}

// CopySelection() places text on the clipboard
void CEdit::CopySelection( BOOL bCopyLineIfEmpty, BOOL bAppend ) const
{
	HGLOBAL hMem = NULL;

	if ( m_Selection.IsColumnSel() )
	{
		ASSERT( !m_Selection.IsEmpty() );
		int nStartCol, nStartRow, nEndCol, nEndRow;
		m_Selection.GetNormalizedColumnSelection( nStartCol, nStartRow, nEndCol, nEndRow );
		if ( nStartCol != nEndCol )
		{
			// calculate the space needed

			// count the null terminator and line break chars now
			int cb = 1 + ( nEndRow - nStartRow ) * 2;
			for ( int nLine = nStartRow; nLine <= nEndRow; nLine++ )
			{
				int nBuffStartCol = m_Buffer.ConvertViewColToBufferCol( nLine, nStartCol );			
				int nBuffEndCol = m_Buffer.ConvertViewColToBufferCol( nLine, nEndCol );
				cb += nBuffEndCol - nBuffStartCol;
							
			}

			if ( cb > 1 )
			{
				hMem = GlobalAlloc( GHND | GMEM_DDESHARE, cb );
				LPTSTR psz = ( LPTSTR ) GlobalLock( hMem );
				LPTSTR pszOut = psz;

				// Copy the text
				for ( int nLine = nStartRow; nLine <= nEndRow; nLine++ )
				{
					int nBuffStartCol = m_Buffer.ConvertViewColToBufferCol( nLine, nStartCol );			
					int nBuffEndCol = m_Buffer.ConvertViewColToBufferCol( nLine, nEndCol );
					int nLineLen = m_Buffer.GetLineLength( nLine );
					int nSpaces = nBuffEndCol - nBuffStartCol;
					if ( nBuffStartCol < nLineLen )
					{
						int nBuffChars = min( nLineLen, nBuffEndCol ) - nBuffStartCol;
						_tcsncpy( pszOut, m_Buffer.GetLineText( nLine ) + nBuffStartCol, nBuffChars );
						pszOut += nBuffChars;
						nSpaces -= nBuffChars;
					}

					if ( nSpaces )
					{
						CharFill( pszOut, _T(' '), nSpaces );
						pszOut += nSpaces;
					}

					if ( nLine < nEndRow )
					{
						// clipboard text must always contain CRLF's -- not just LF's.
						*pszOut++ = _T('\r');
						*pszOut++ = _T('\n');
					}
				}

				*pszOut = '\0';				
				ASSERT( ( int ) _tcslen( psz ) == ( cb - 1 ) );
				GlobalUnlock( hMem );
				// remember that we just copied a column so that paste can paste it back as
				// a column
				g_bCopyColumn = TRUE;
			}
		}
	}
	else
	{
		int nStartCol, nStartRow, nEndCol, nEndRow;
		m_Selection.GetNormalizedBufferSelection( nStartCol, nStartRow, nEndCol, nEndRow );

		if ( m_Buffer.GetText( nStartRow, nStartCol, nEndRow, nEndCol, hMem, bCopyLineIfEmpty, TRUE ) )
		{
			if ( bAppend )
			{
				HGLOBAL hMemClip = GetClipboardText();
				if ( hMemClip )
				{
					HGLOBAL hMemConcat = GlobalAlloc( GHND | GMEM_DDESHARE, GlobalSize( hMem ) + GlobalSize( hMemClip ) );
					if ( hMemConcat )
					{
						LPTSTR psz = ( LPTSTR ) GlobalLock( hMemConcat );
						*psz = 0;
						_tcscat( psz, ( LPCTSTR )GlobalLock( hMemClip ) );
						_tcscat( psz, ( LPCTSTR )GlobalLock( hMem ) );
						GlobalUnlock( hMem );
						GlobalFree( hMem );
						GlobalUnlock( hMemConcat );
						hMem = hMemConcat;
					}
				}
			}
			// remember that we did not just copy a column so that paste can paste 
			// it back as regular block text
			g_bCopyColumn = FALSE;
		}
	}

	if ( hMem )
	{
		VERIFY( SetClipboardText( hMem ) );
	}
}

HGLOBAL CEdit::GetClipboardText() const
{
	HGLOBAL hMem = NULL;
	if ( IsClipboardFormatAvailable( CLIP_TEXT ) && OpenClipboard( m_hWnd ) )
	{
		hMem = GetClipboardData( CLIP_TEXT );
		CloseClipboard();
	}

	return hMem;
}

BOOL CEdit::SetClipboardText( HGLOBAL hMem ) const
{
	BOOL bSuccess = FALSE;
	if ( OpenClipboard( m_hWnd ) )
	{
		EmptyClipboard();
		if ( hMem )
		{
			bSuccess = ( hMem == SetClipboardData( CLIP_TEXT, hMem ) );
		}
		else
		{
			bSuccess = TRUE;
		}
		CloseClipboard();
	}

	return bSuccess;
}

// HitTest() determines what CodeMax region is under a client point
CEdit::HitTestCode CEdit::HitTest( int xPos, int yPos, int &nView ) const
{
	HitTestCode eCode = eNowhere;

	BOOL bFound = FALSE;
	nView = -1;

#ifdef _ACTIVEX
	// show arrow cursor if in design mode
	if ( m_pEditX && m_pEditX->IsInDesignMode() )
		return eCode;
#endif

	RECT rc;
	ASSERT( IsWindow( m_hWnd ) );
	GetClientRect( m_hWnd, &rc );
	POINT pt = { xPos, yPos };

	if ( PtInRect( &rc, pt ) )
	{
		if ( m_bAllowHSplit && ( xPos >= m_xPosHSplitter && xPos < m_xPosHSplitter + CXY_SPLITTER ) )
		{
			if ( m_xPosHSplitter || ( ( m_xPosHSplitter == 0 ) && ( yPos >= ( rc.bottom - GetSystemMetrics( SM_CYHSCROLL ) ) ) ) )
			{
				eCode = eHSplitter;
				bFound = TRUE;
			}
		}
		
		if ( m_bAllowVSplit && ( yPos >= m_yPosVSplitter && yPos < m_yPosVSplitter + CXY_SPLITTER ) )
		{
			if ( m_yPosVSplitter || ( ( m_yPosVSplitter == 0 ) && ( xPos >= ( rc.right - GetSystemMetrics( SM_CXVSCROLL ) ) ) ) )
			{
				eCode = ( ( eCode == eHSplitter ) ? eHVSplitter : eVSplitter );
				bFound = TRUE;
			}
		}

		if ( !bFound )
		{
			for ( int i = 0; i < m_nViews; i++ )
			{
				RECT rcView;
				m_Views[ i ] ->GetRect( &rcView );
				if ( PtInRect( &rcView, pt ) )
				{
					nView = i;
					eCode = eEditSpace;
					bFound = TRUE;
					int cyHScroll = GetSystemMetrics( SM_CYHSCROLL );
					if ( ShowVScrollBar() && ( xPos >= rcView.right - cyHScroll ) )
					{
						eCode = eVScrollBar;
					}
					int cxVScroll = GetSystemMetrics( SM_CXVSCROLL );
					if ( ShowHScrollBar() && ( yPos >= rcView.bottom - cxVScroll ) )
					{
						eCode = ( ( eCode == eVScrollBar ) ? eSizeBox : eHScrollBar );
					}
					else if ( ShowLeftMargin() && ( xPos < rcView.left + CM_CXLEFTMARGIN ) )
					{
						eCode = eLeftMargin;
					}
				}
			}
		}
	}

	return eCode;
}

// PtInSelection() determines if a client point is over the selection
BOOL CEdit::PtInSelection( int xPos, int yPos, BOOL bViewsMustBeSame ) const
{
	BOOL bOverSel = FALSE;

	int nView;
	if ( !m_Selection.IsEmpty() && ( HitTest( xPos, yPos, nView ) == eEditSpace ) )
	{
		CEditView *pView = m_Views[ nView ];
		if ( !bViewsMustBeSame || ( m_Selection.GetView() == pView ) )
		{
			int nCol, nRow;
			RECT rcChar;
			pView->GetCharPosFromPoint( xPos, yPos, nCol, nRow, &rcChar );
			//nCol = m_Buffer.ConvertViewColToBufferCol( nRow, nCol );
			int nStartRow, nStartCol, nEndRow, nEndCol;
			m_Selection.GetNormalizedViewSelection( nStartCol, nStartRow, nEndCol, nEndRow );
			if ( nRow >= nStartRow && nRow <= nEndRow )
			{
				if ( m_Selection.IsColumnSel() )
				{
					bOverSel = ( ( nCol >= nStartCol ) &&
				                 ( nCol < nEndCol ) ) ||
					           ( ( nCol >= nEndCol ) &&
				                 ( nCol < nStartCol ) );
				}
				else
				{
					bOverSel = !( nRow == nStartRow && nCol < nStartCol ) &&
				               !( nRow == nEndRow && nCol >= nEndCol );
				}
			}
		}
	}

	return bOverSel;
}

// CancelMode() bails out of whatever mode the user is in, switching back to standard editing mode.
void CEdit::CancelMode()
{
	if ( IsSplitting() )
	{
		EndSplitting( 0, 0, FALSE );
	}
	else if ( IsSelecting() || IsLineSelecting() )
	{
		EndSelecting();
	}
	else if ( !IsRecordingMacro() )
	{
		m_eMode = eIdle;
	}

	if ( GetCapture() == m_hWnd )
		{
		VERIFY( ReleaseCapture() );
		ASSERT( GetCapture() != m_hWnd );
		}

	// flash the caret if it should be shown.  This fixes problems with
	// some containers where the caret cannot be shown while in certain
	// states.
	FlashCaret();
}

void CEdit::DamageAllViews( int nFirstLine, int nLastLine ) const
{
	for ( int i = 0; i < m_nViews; i++ )
	{
		m_Views[ i ]->DamageView( nFirstLine, nLastLine );
	}
}

COLORREF CEdit::GetTokenColor( CBuffer::LangToken eToken, BOOL bTextColor ) const
{
	switch ( eToken )
	{
		case CBuffer::eKeyword: return bTextColor ? m_Colors.crKeyword : m_Colors.crKeywordBk;
		case CBuffer::eOperator: return bTextColor ? m_Colors.crOperator : m_Colors.crOperatorBk;
		case CBuffer::eScopeKeywordStart:
		case CBuffer::eScopeKeywordEnd:
		case CBuffer::eScopeKeywordStartAndEnd: return bTextColor ? m_Colors.crScopeKeyword : m_Colors.crScopeKeywordBk;
		case CBuffer::eSingleLineComment:
		case CBuffer::eMultiLineCommentStart:
		case CBuffer::eMultiLineCommentEnd:
		case CBuffer::eMultiLineCommentStartAndEnd: return bTextColor ? m_Colors.crComment : m_Colors.crCommentBk;
		case CBuffer::eStringDelim: return bTextColor ? m_Colors.crString : m_Colors.crStringBk;
		case CBuffer::eEscapeSeq:
		case CBuffer::eText: return bTextColor ? m_Colors.crText : m_Colors.crTextBk;
		case CBuffer::eTagText: return bTextColor ? m_Colors.crTagText : m_Colors.crTagTextBk;
		case CBuffer::eNumber: return bTextColor ? m_Colors.crNumber : m_Colors.crNumberBk;
		case CBuffer::eTagEntity: return bTextColor ? m_Colors.crTagEntity : m_Colors.crTagEntityBk;
		case CBuffer::eTagElementName: return bTextColor ? m_Colors.crTagElementName : m_Colors.crTagElementNameBk;
		case CBuffer::eTagAttributeName: return bTextColor ? m_Colors.crTagAttributeName : m_Colors.crTagAttributeNameBk;
	}

	ASSERT( FALSE );
	return CLR_INVALID;
}

HFONT CEdit::GetTokenFont( CBuffer::LangToken eToken, long &cxExtraSpacing, long &cyDescentShift, FONT_DATA *pFonts ) const
{
	int nFont = CM_FONT_NORMAL;
	// under DBCS, the fancy font support is disabled because of inter-character spacing issues.
	if ( !DBCS_ACTIVE )
	{
		switch ( eToken )
		{
			case CBuffer::eKeyword:							nFont = m_FontStyles.byKeyword; break;
			case CBuffer::eSingleLineComment:
			case CBuffer::eMultiLineCommentStart:
			case CBuffer::eMultiLineCommentEnd:
			case CBuffer::eMultiLineCommentStartAndEnd:		nFont = m_FontStyles.byComment; break;
			case CBuffer::eOperator:						nFont = m_FontStyles.byOperator; break;
			case CBuffer::eScopeKeywordStart:
			case CBuffer::eScopeKeywordEnd:
			case CBuffer::eScopeKeywordStartAndEnd:			nFont = m_FontStyles.byScopeKeyword; break;
			case CBuffer::eStringDelim:						nFont = m_FontStyles.byString; break;
			case CBuffer::eEscapeSeq:
			case CBuffer::eText:							nFont = m_FontStyles.byText; break;
			case CBuffer::eTagText:							nFont = m_FontStyles.byTagText; break;
			case CBuffer::eNumber:							nFont = m_FontStyles.byNumber; break;
			case CBuffer::eTagEntity:						nFont = m_FontStyles.byTagEntity; break;
			case CBuffer::eTagElementName:					nFont = m_FontStyles.byTagElementName; break;
			case CBuffer::eTagAttributeName:				nFont = m_FontStyles.byTagAttributeName; break;
			default:										nFont = CM_FONT_NORMAL; break;
		}
	}

	const FONT_DATA *pFont = pFonts + nFont;
	cxExtraSpacing = pFont->cxExtraSpacing;
	cyDescentShift = pFont->cyDescentShift;
	return pFont->hFont;
}

COLORREF CEdit::GetWindowColor( BOOL bSafe ) const
{
	return ( bSafe && m_Colors.crWindow == CLR_INVALID ) ? GetSysColor( COLOR_WINDOW ) : m_Colors.crWindow;
}

COLORREF CEdit::GetBookmarkColor() const
{
	return m_Colors.crBookmark == CLR_INVALID ? GetSysColor( COLOR_HIGHLIGHT ) : m_Colors.crBookmark;
}

COLORREF CEdit::GetBookmarkBkColor() const
{
	return m_Colors.crBookmarkBk == CLR_INVALID ? GetSysColor( COLOR_HIGHLIGHT ) : m_Colors.crBookmarkBk;
}

void CEdit::Repaint( BOOL bUpdateNow )
{
	// Update without causing the scrollbars to flash
	RECT rcClient;
	GetClientRect( m_hWnd, &rcClient );
	if ( ShowVScrollBar() )
	{
		rcClient.right -= GetSystemMetrics( SM_CXVSCROLL );
	}
	if ( ShowHScrollBar() )
	{
		rcClient.bottom -= GetSystemMetrics( SM_CYHSCROLL );
	}
	InvalidateRect( m_hWnd, &rcClient, FALSE );
	if ( bUpdateNow )
	{
		SafeUpdateWindow();
	}
}

// SafeUpdateWindow() prevents unneccessary repaints.  To ensure efficiency, never call UpdateWindow() directly -- call this.
void CEdit::SafeUpdateWindow() const
{
	// don't cause an immediate repaint if in a first-time transaction, or else just let the WM_PAINT queue up
	if ( !m_Buffer.IsInEditTransaction() || m_Buffer.IsInUndoRedo() )
	{
		UpdateWindow( m_hWnd );
	}
}

// LineIsValid() determines if a zero-based line number is represented in the buffer.
BOOL CEdit::LineIsValid( int nLine, BOOL bAllowConcatenation ) const
{
	int nLineCount = m_Buffer.GetLineCount();
	return ( nLine >= 0 && ( nLine < ( nLineCount + bAllowConcatenation ) ) );
}

// RangeIsValid() determines if a range is not outside the current buffer bounds.
BOOL CEdit::RangeIsValid( CM_RANGE *pRange ) const
{
	ASSERT( pRange );
	int nLineCount = m_Buffer.GetLineCount();
	return ( pRange->posStart.nCol >= 0 &&
	         pRange->posEnd.nCol >= 0 &&
	         pRange->posStart.nLine >= 0 &&
	         pRange->posStart.nLine < nLineCount &&
	         pRange->posEnd.nLine >= 0 &&
	         pRange->posEnd.nLine < nLineCount );
}

// GetSelectionText() returns a HGLOBAL to the text selected.  Caller owns the memory and is responsible for it's cleanup.
HGLOBAL CEdit::GetSelectionText() const
{
	HGLOBAL hMem = NULL;

	if ( !m_Selection.IsEmpty() )
	{
		int nBuffStartCol, nBuffStartRow, nBuffEndCol, nBuffEndRow;
		m_Selection.GetNormalizedBufferSelection( nBuffStartCol, nBuffStartRow, nBuffEndCol, nBuffEndRow );
		m_Buffer.GetText( nBuffStartRow, nBuffStartCol, nBuffEndRow, nBuffEndCol, hMem, TRUE );
	}

	return hMem;
}

// NotifyParent() sends the requested WM_NOTIFY notification to the parent window.
LRESULT CEdit::NotifyParent( UINT unNotification, LPNMHDR pnmhdr ) const
{
#ifdef _ACTIVEX
	// go thru proper dispatch interface
	return ( LRESULT ) m_pEditX ? !m_pEditX->FireControlEvent( unNotification, pnmhdr ) : 0;
#else
	// do WM_NOTIFY processing
	UINT unCtrlID = GetDlgCtrlID( m_hWnd );
	NMHDR hdr = { m_hWnd, unCtrlID, unNotification };
	if ( pnmhdr )
	{
		// caller supplied NMHDR; fill in the standard stuff here
		*pnmhdr = hdr;
	}
	else
	{
		// simple notification -- no extra data required
		pnmhdr = &hdr;
	}
	HWND hWndParent = GetParent( m_hWnd );
	ASSERT( IsWindow( hWndParent ) );
	return SendMessage( hWndParent, WM_NOTIFY, ( WPARAM ) unCtrlID, ( LPARAM ) pnmhdr );
#endif
}

LRESULT CEdit::NotifyParentOfCmdFailure( DWORD dwErr ) const
{
	CM_CMDFAILUREDATA hdr;
	hdr.wCmd = m_wCurrCmd;
	hdr.dwErr = dwErr;

	LRESULT lResult = NotifyParent( CMN_CMDFAILURE, ( LPNMHDR ) &hdr );

	if ( !lResult )
	{
		// parent wants default error feedback (beep)
		MessageBeep( 0 );
	}

	// TRUE if parent handled notification and wants no default handling
	return lResult;
}

LRESULT CEdit::NotifyParentOfKeyMsg( UINT unNotification, WPARAM wParam ) const
{
	CM_KEYDATA hdr;
	hdr.nKeyCode = ( int ) wParam;
	hdr.nKeyModifier = 0;
	hdr.nKeyModifier |= ( ( GetKeyState( VK_CONTROL ) < 0 ) ? CM_KEY_CTRL : 0 );
	hdr.nKeyModifier |= ( ( GetKeyState( VK_SHIFT ) < 0 ) ? CM_KEY_SHIFT : 0 );
	hdr.nKeyModifier |= ( ( GetKeyState( VK_MENU ) < 0 ) ? CM_KEY_ALT : 0 );

	return NotifyParent( unNotification, ( LPNMHDR ) &hdr );
}

LRESULT CEdit::NotifyParentOfMouseMsg( UINT unNotification, WPARAM wParam, LPARAM lParam ) const
{
	CM_MOUSEDATA hdr;
	hdr.pt.x = ( short ) LOWORD( lParam );
	hdr.pt.y = ( short ) HIWORD( lParam );
	hdr.nButton = 0;
	if ( HAS_FLAG( wParam, MK_LBUTTON ) )
		hdr.nButton |= CM_BTN_LEFT;
	if ( HAS_FLAG( wParam, MK_RBUTTON ) )
		hdr.nButton |= CM_BTN_RIGHT;
	if ( HAS_FLAG( wParam, MK_MBUTTON ) )
		hdr.nButton |= CM_BTN_MIDDLE;
	hdr.nKeyModifier = 0;
	hdr.nKeyModifier |= ( ( GetKeyState( VK_CONTROL ) < 0 ) ? CM_KEY_CTRL : 0 );
	hdr.nKeyModifier |= ( ( GetKeyState( VK_SHIFT ) < 0 ) ? CM_KEY_SHIFT : 0 );
	hdr.nKeyModifier |= ( ( GetKeyState( VK_MENU ) < 0 ) ? CM_KEY_ALT : 0 );

	return NotifyParent( unNotification, ( LPNMHDR ) &hdr );
}

LRESULT CEdit::NotifyParentOfFindWrapped( BOOL bForward ) const
{
	CM_FINDWRAPPEDDATA hdr;
	hdr.wCmd = m_wCurrCmd;
	hdr.bForward = bForward;

	return NotifyParent( CMN_FINDWRAPPED, ( LPNMHDR ) &hdr );
}

int CEdit::MessageBox( UINT unText, UINT unCaption, UINT unType ) const
{
	CNoHideSel disable( ( CEdit * ) this );  // don't honor hidesel behavior here
	TCHAR szText[ 256 ];
	VERIFY( LoadString( hInstance, unText, szText, ARRAY_SIZE( szText ) ) );
	TCHAR szCaption[ 256 ];
	VERIFY( LoadString( hInstance, unCaption, szCaption, ARRAY_SIZE( szCaption ) ) );
	return ::MessageBox( m_hWnd, szText, szCaption, unType );
}

int CEdit::MessageBox( HWND hWndParent, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType ) const
{
	CNoHideSel disable( ( CEdit * ) this );  // don't honor hidesel behavior here
	return ::MessageBox( hWndParent, lpText, lpCaption, uType );
}
 
int CEdit::DialogBoxParam( HINSTANCE hInstance, UINT unDlg, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam ) const
{
	CNoHideSel disable( ( CEdit * ) this );  // don't honor hidesel behavior here
	return ::DialogBoxParam( hInstance, MAKEINTRESOURCE( unDlg ), hWndParent, lpDialogFunc, dwInitParam );
}

void CEdit::ShowRightClickMenu( int x, int y )
{
	CancelMode();

	BOOL bHasSelection = !m_Selection.IsEmpty();
	BOOL bCanEdit = !m_Buffer.IsReadOnly();
	BOOL bColumnSel = m_Selection.IsColumnSel();
	BOOL bBufferNonEmpty = !m_Buffer.IsEmpty();
	struct {
		UINT unCmd;
		UINT unCmdStr;
		BOOL bEnabled;
		BOOL bChecked;
	} Menu[] = {
		{ CMD_CUT, IDS_MENU_CUT, bHasSelection, FALSE },
		{ CMD_COPY, IDS_MENU_COPY, bHasSelection, FALSE },
		{ CMD_PASTE, IDS_MENU_PASTE, IsClipboardFormatAvailable( CLIP_TEXT ) && bCanEdit, FALSE },
		{ ( UINT ) -1, NULL, FALSE, FALSE },
		{ CMD_FIND, IDS_MENU_FIND, bBufferNonEmpty, FALSE },
		{ CMD_FINDREPLACE, IDS_MENU_FINDREPLACE, bCanEdit && bBufferNonEmpty, FALSE },
		{ ( UINT ) -1, NULL, FALSE, FALSE },
		{ CMD_TOGGLEWHITESPACEDISPLAY, IDS_MENU_TOGGLEWHITESPACEDISPLAY, TRUE, m_bDisplayWhitespace },
		{ CMD_UPPERCASESELECTION, IDS_MENU_UPPERCASESELECTION, bHasSelection && !bColumnSel, FALSE },
		{ CMD_LOWERCASESELECTION, IDS_MENU_LOWERCASESELECTION, bHasSelection && !bColumnSel, FALSE },
		{ ( UINT ) -1, NULL, FALSE, FALSE },
		{ CMD_UNDO, IDS_MENU_UNDO, m_Buffer.CanUndo(), FALSE },
		{ CMD_REDO, IDS_MENU_REDO, m_Buffer.CanRedo(), FALSE },
		{ ( UINT ) -1, NULL, FALSE, FALSE },
		{ CMD_PROPERTIES, IDS_MENU_PROPERTIES, TRUE, FALSE },
		#ifdef _DEBUG
		{ CMD_SELFTEST, IDS_MENU_SELFTEST, TRUE, m_bSelfTest }
		#endif
	};

	HMENU hMenu = CreatePopupMenu();
	for ( int i = 0; i < ARRAY_SIZE( Menu ); i++ )
	{
		UINT unCmd = Menu[ i ].unCmd;
		if ( unCmd == ( UINT ) -1 )
		{
			InsertMenu( hMenu, 0xffffffff, MF_BYPOSITION | MF_SEPARATOR, 0, NULL );
		}
		else
		{
			
			InsertMenu( hMenu, 
			            0xffffffff, 
						MF_BYPOSITION | MF_STRING | ( Menu[ i ].bEnabled ? MF_ENABLED : MF_GRAYED ) | ( Menu[ i ].bChecked ? MF_CHECKED : MF_UNCHECKED ),
						unCmd, 
						LoadStringPtr( Menu[ i ].unCmdStr ) );
		}
	}
	
	// temporarily disable selection hiding -- the popup window does not qualify as taking the focus away
	CNoHideSel disable( this );  // don't honor hidesel behavior here
	UINT unCmd = TrackPopupMenu( hMenu, 
	                             TPM_LEFTALIGN | TPM_TOPALIGN | TPM_NONOTIFY | TPM_RETURNCMD | TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
					             x,
					             y,
					             0,
					             m_hWnd,
					             NULL );

	if ( unCmd )
	{
		ExecuteCommand( ( WORD ) unCmd, 0, TRUE );
	}
}

// UpdateAutoIndentStack() is a helper method called by IndentLineToScope().  It will 
// parse a line of code, looking for scope words.  Once a scope word is found, it is placed
// on the stack, and the stack is then collapsed (if possible).  Once the stack shrinks to
// size 0, the start scope word has been located.
BOOL CEdit::UpdateAutoIndentStack( int nLine, DWORD *pStack, int nStackCeil, int &nStackPos, BOOL bSkipInitialStartTokens, BOOL &bHasLeadingText, BOOL &bHasTrailingStartTokens, BOOL &bException )
{
	bException = FALSE;
	BOOL bUpdatedStack = FALSE;
	DWORD ScopeDataLR[ 100 ];	// must parse text from left to right; this array will later
	                            // be pushed onto the main stack (pStack) in reverse order.
	int nScopeDataLRPos = 0;

	bHasTrailingStartTokens = FALSE;

	ASSERT( m_Buffer.HasLanguage() );
	m_Buffer.WaitForLine( nLine );

	int nLen = m_Buffer.GetLineLength( nLine );
	if ( nLen > CEditView::MAXCOL )
	{
		// don't bother parsing a huge line!
		bException = TRUE;
	}

	CLineParser Parser( &m_Buffer, nLine, nLen );

	LPCTSTR psz = m_Buffer.GetLineText( nLine );
	while ( *psz && is_space( *psz ) )
	{
		psz++;
	}
	BOOL bFirst = TRUE;
	bHasLeadingText = FALSE;
	if ( *psz != '\0' )
	{
		Parser.AdvanceTo( psz );
		do
		{
			Parser.AcceptToken();

			if ( !Parser.m_bInComment && !Parser.m_bInString )
			{
				switch ( Parser.m_eToken )
				{
					case CBuffer::eScopeKeywordStart:
					case CBuffer::eScopeKeywordEnd:
					case CBuffer::eScopeKeywordStartAndEnd:
					{
						ScopeDataLR[ nScopeDataLRPos++ ] = MAKELPARAM( Parser.m_eToken, MAKEWORD( Parser.m_nTokenID, Parser.m_nTokenID2 ) );
						break;
					}
				}

				if ( nScopeDataLRPos >= ARRAY_SIZE( ScopeDataLR ) )
				{
					bException = TRUE;
					break;
				}
			}

			if ( bFirst )
			{
				bFirst = FALSE;
				bHasLeadingText = ( nScopeDataLRPos == 0 );
			}
		} while ( Parser.MoreComing() );	
 	}

	if ( !bException )
	{
		// now that we've built a list of scope tokens for this line, 
		// we must add them to the scope stack, collapsing the stack
		// whenever possible.  When the stack reaches size 0, then 
		// we have arrived at the start of the outer scope block and
		// can quit.
		BOOL bEncounteredEndToken = FALSE;

		for ( int i = nScopeDataLRPos - 1; !bException && i >= 0; i-- )
		{
			DWORD dwScopeData = ScopeDataLR[ i ];
			CBuffer::LangToken eToken = ( CBuffer::LangToken ) LOWORD( dwScopeData );
			WORD wTokenIDs = HIWORD( dwScopeData );
			int nTokenID1 = LOBYTE( wTokenIDs );
			int nTokenID2 = HIBYTE( wTokenIDs );

			bHasTrailingStartTokens = ( !bEncounteredEndToken && HAS_FLAG( eToken, CBuffer::eScopeKeywordStart ) );
			// caller wants the initial scope word start tokens to be ignored
			if ( bSkipInitialStartTokens && ( !bEncounteredEndToken && eToken == CBuffer::eScopeKeywordStart ) )
			{
				continue;
			}

			DWORD dwPush = MAKELPARAM( eToken, MAKEWORD( nTokenID1, nTokenID2 ) );
			BOOL bPush = TRUE;
			
			switch ( eToken )
			{
				case CBuffer::eScopeKeywordStartAndEnd:
				{
					// attempt to collapse the stack
					if ( nStackPos > 0 )
					{
						do
						{
							DWORD dwPrev = pStack[ nStackPos - 1 ];
							nStackPos--;
							bUpdatedStack = TRUE;
							WORD wLo = LOWORD( dwPrev );
							if ( wLo == CBuffer::eScopeKeywordEnd ||
							     wLo == CBuffer::eScopeKeywordStartAndEnd )
							{
								break;
							}
						} while ( nStackPos > 0 );
					}

					// since we are at a 'middle' token (both a start and an end token),
					// push this onto the stack so that it will be popped by the next start
					// token
					bPush = TRUE;
					break;
				}
				case CBuffer::eScopeKeywordStart:
				{
					// attempt to collapse the stack
					if ( nStackPos > 0 )
					{
						do
						{
							DWORD dwPrev = pStack[ nStackPos - 1 ];
							nStackPos--;
							bUpdatedStack = TRUE;
							WORD wLo = LOWORD( dwPrev );
							if ( wLo == CBuffer::eScopeKeywordEnd ||
							     wLo == CBuffer::eScopeKeywordStartAndEnd )
							{
								break;
							}
						} while ( nStackPos > 0 );
					}
					else
					{
						// there is a scoping syntax error.  Underflow the stack to signify this to the caller.
						nStackPos = -1;
					}
					bPush = FALSE;
					break;
				}
				case CBuffer::eScopeKeywordEnd:
				{
					bEncounteredEndToken = TRUE;
					break;
				}
			}

			if ( bPush )
			{
				bUpdatedStack = TRUE;
				if ( nStackPos <= nStackCeil )
				{
					pStack[ nStackPos++ ] = dwPush;
				}
				else
				{
					// don't blow the stack.
					bException = TRUE;
				}
			}
		}
	}
	return bUpdatedStack;
}

// CopyLeadingIndentation() copies the indentation of one line to another
int CEdit::CopyLeadingIndentation( int nLineSrc, int nLineDest, BOOL bIndentExtra, BOOL bAllowRemoveLeadingIndentation )
{
	ASSERT( m_Buffer.IsInEditTransaction() );
	LPCTSTR pszLine = m_Buffer.GetLineText( nLineSrc );
	LPCTSTR psz = pszLine;
	while ( *psz && is_space( *psz ) )
	{
		psz++;
	}

	int cbChars = 0;
	if ( bAllowRemoveLeadingIndentation )
	{
		RemoveLeadingIndentation( nLineDest );
	}
	if ( bIndentExtra || ( psz > pszLine ) )
	{
		// copy the indentation of the previous line
		cbChars = psz - pszLine;

		LPTSTR pszTab = NULL;
		if ( bIndentExtra )
		{
			if (  m_bExpandTabs )
			{
				// expand this tab now
				int nViewCol = m_Buffer.ConvertBufferColToViewCol( nLineSrc, cbChars );
				int cbTabSize = m_Buffer.GetTabSize();
				int nSpaces = -( nViewCol - ( ( ( nViewCol + cbTabSize ) / cbTabSize ) * cbTabSize ) );
				pszTab = new TCHAR[ nSpaces + 1 ];
				CharFill( pszTab, _T(' '), nSpaces );
				pszTab[ nSpaces ] = _T('\0');
				cbChars += nSpaces;
			}
			else
			{
				pszTab = new TCHAR[ 2 ];
				pszTab[ 0 ] = _T('\t');
				pszTab[ 1 ] = _T('\0');
				cbChars++;
			}
		}

		LPTSTR pszIndent = new TCHAR[ cbChars + 1 ];
		int cbIndent = psz - pszLine;
		_tcsncpy( pszIndent, pszLine, cbIndent );
		pszIndent[ cbIndent ] = _T('\0');
		if ( pszTab )
		{
			_tcscpy( pszIndent + cbIndent, pszTab );
			delete pszTab;
		}
		m_Buffer.InsertCharsIntoLine( nLineDest, 0, pszIndent, cbChars );
		delete pszIndent;
	}
	
	return cbChars;
}

void CEdit::RemoveLeadingIndentation( int nLine )
{
	ASSERT( m_Buffer.IsInEditTransaction() );
	LPCTSTR pszLine = m_Buffer.GetLineText( nLine );
	LPCTSTR psz = pszLine;
	while ( *psz && is_space( *psz ) )
	{
		psz++;
	}

	if ( psz > pszLine )
	{
		m_Buffer.RemoveCharsFromLine( nLine, 0, psz - pszLine );
	}
}

// IndentLineToScope() auto-indents a line to the correct scoping level defined by the
// context of the lines above nLine and the current language
int CEdit::IndentLineToScope( int nLine, BOOL &bScopeStart )
{
	BOOL bException = FALSE;
	int nCopyLine = -1;
	if ( m_Buffer.HasLanguage() )
	{
		int nStopLine = nLine - SCOPE_RADIUS;
		nStopLine = max( 0, nStopLine );
		BOOL bHasLeadingText = FALSE;
		
		// Stack is used to locate the start of the scope block.  Each DWORD value
		// stores a HIWORD == Token Type (eScopeKeywordStart, eScopeKeywordEnd, or eScopeKeywordStartAndEnd)
		// and a LOWORD == the token ID
		DWORD Stack[ 1000 ];
		int nStackTop = 0;

		BOOL bFirst = TRUE;
		int nCurrLine = nLine;
		while ( !bException && nCurrLine >= nStopLine )
		{
			BOOL bLeadsWithText, bHasTrailingStartTokens;
			BOOL bUpdatedStack = UpdateAutoIndentStack( nCurrLine, Stack, ARRAY_SIZE( Stack ) - 1, nStackTop, bFirst, bLeadsWithText, bHasTrailingStartTokens, bException );
			if ( bFirst )
			{
				bScopeStart = bHasTrailingStartTokens;
				bHasLeadingText = bLeadsWithText;
			}

			// if stack was fully collapsed (or almost collapsed, but the one token left was a start-and-end token which is
			// valid)
			if ( nStackTop == 0 || ( !bFirst && bUpdatedStack && nStackTop == 1 && LOWORD( Stack[ 0 ] ) == CBuffer::eScopeKeywordStartAndEnd ) )
			{
				// traversed back to the line where the scope began
				nCopyLine = ( nCurrLine == nLine ) ? -1 : nCurrLine;
				break;
			}
			else if ( nStackTop < 0 || bException )
			{
				nCopyLine = -1; // failsafe: copy prev line (caller knows -1 means this)
				break;
			}

			bFirst = FALSE;
			nCurrLine--;
		}
		
		
		if ( !bHasLeadingText && nCopyLine != -1 )
		{
			CopyLeadingIndentation( nCopyLine, nLine, FALSE, TRUE );
		}
	}

	return ( bException ? -1 : nCopyLine );
}

// flash the caret if it should be shown.  This fixes problems with
// some containers where the caret cannot be shown while in certain
// states.
void CEdit::FlashCaret() const
{
	if ( GetFocus() == m_hWnd )
	{	
		SetFocus( NULL );
		SetFocus( m_hWnd );
	}
}

void CEdit::DeleteOffScreenBitmap()
{
	if ( m_hbmPaint )
	{
		DeleteObject( m_hbmPaint );
		m_sizeOffScreenBmp.cx = m_sizeOffScreenBmp.cy = 0;
		m_hbmPaint = NULL;
	}
}
void CEdit::CreateOffScreenBitmap( int cx, int cy )
{
	// create a new bitmap only if the cached one isn't big enough
	if ( cx > m_sizeOffScreenBmp.cx ||
	     cy > m_sizeOffScreenBmp.cy )
	{
		// clear the last one.
		DeleteOffScreenBitmap();

		HDC hdcScreen = GetDC( NULL );
		m_hbmPaint = CreateCompatibleBitmap( hdcScreen, cx, cy );
		ReleaseDC( NULL, hdcScreen );
		if ( m_hbmPaint )
		{
			m_sizeOffScreenBmp.cx = cx;
			m_sizeOffScreenBmp.cy = cy;
		}
	}
}

void CEdit::DeleteDisplayFonts()
{
	if ( m_bOwnFont && m_hFont != g_hFontDefault )
	{
		DeleteObject( m_hFont );
	}

	for ( int i = 0; i < NUM_FONT_VARIATIONS; i++ )
	{
		if ( m_hFont != m_font[ i ].hFont )
			DeleteObject( m_font[ i ].hFont );
	}
}

void CEdit::SetDisplayFont( HFONT hFont, BOOL bFirstTime )
{
	if ( !bFirstTime )
	{
		DeleteDisplayFonts();
	}

	ZeroMemory( m_font, sizeof( m_font ) );
	m_bOwnFont = FALSE;
	m_hFont = hFont;

	int nSize = GetObject( hFont, sizeof( LOGFONT ), NULL );
	LOGFONT lf;
	ASSERT( nSize != 0 );
	VERIFY( GetObject( hFont, nSize, &lf ) );
	NormalizeLOGFONT( lf );

	GenerateFontVariations( NULL, lf, m_font, m_cxChar );
}

void CEdit::GenerateFontVariations( HDC hDC, const LOGFONT &lfBase, FONT_DATA *m_pFonts, int &cxChar )
{
	LOGFONT lf = lfBase;
	//
	// Normal
	//
	lf.lfWeight = FW_NORMAL;
	m_pFonts[ CM_FONT_NORMAL ].hFont = CreateFontIndirect( &lf );

	//
	// Bold
	//
	lf.lfWeight = FW_BOLD;
	m_pFonts[ CM_FONT_BOLD ].hFont = CreateFontIndirect( &lf );

	//
	// Bold + Italic
	//
	lf.lfWeight = FW_BOLD;
	lf.lfItalic = TRUE;
	m_pFonts[ CM_FONT_BOLDITALIC ].hFont = CreateFontIndirect( &lf );

	//
	// Italic
	//
	lf.lfWeight = FW_NORMAL;
	lf.lfItalic = TRUE;
	m_pFonts[ CM_FONT_ITALIC ].hFont = CreateFontIndirect( &lf );

	//
	// Underline
	//
	lf.lfWeight = FW_NORMAL;
	lf.lfItalic = FALSE;
	lf.lfUnderline = TRUE;
	m_pFonts[ CM_FONT_UNDERLINE ].hFont = CreateFontIndirect( &lf );

	BOOL bReleaseDC = FALSE;
	if ( !hDC )
	{
		hDC = GetDC( NULL );
		bReleaseDC = TRUE;
	}

	// calculate the inter-character spacing and vertical alignment
	// so all font variations will yield the same-sized characters
	// and baselines will match when rendered later
	cxChar = 0;
	int nMaxDescent = 0;
	for ( int i = 0; i < NUM_FONT_VARIATIONS; i++ )
	{
		FONT_DATA *pFont = m_pFonts + i;
		HFONT hFontOld = ( HFONT ) SelectObject( hDC, pFont->hFont );
		TEXTMETRIC tm;
		VERIFY( GetTextMetrics( hDC, &tm ) );
		SelectObject( hDC, hFontOld );
		pFont->cyFont = tm.tmHeight;
		pFont->cxExtraSpacing = tm.tmAveCharWidth;
		pFont->cyDescentShift = tm.tmDescent;
		cxChar = max( cxChar, tm.tmAveCharWidth );
		nMaxDescent = max( nMaxDescent, tm.tmDescent );
	}

	if ( bReleaseDC )
		ReleaseDC( NULL, hDC );

	// under DBCS, the fancy font support is disabled because of inter-character spacing issues.
	if ( DBCS_ACTIVE )
	{
		cxChar = m_pFonts->cxExtraSpacing;
	}
	
	// pass 2: fix up the widths to account for the extra space required and
	// fixup the descent shifts so baselines line up
	for ( int i = 0; i < NUM_FONT_VARIATIONS; i++ )
	{
		FONT_DATA *pFont = m_pFonts + i;
		pFont->cxExtraSpacing = cxChar - pFont->cxExtraSpacing;
		pFont->cyDescentShift = nMaxDescent - pFont->cyDescentShift;
	}

	// under DBCS, the fancy font support is disabled because of inter-character spacing issues.
	if ( DBCS_ACTIVE )
	{
		m_pFonts->cxExtraSpacing = 0;
	}
}

void CEdit::DamageSelection( BOOL bUpdateNow ) const
{
	if ( !m_Selection.IsEmpty() )
	{
		int nStartRow, nEndRow, nDontCare;
		m_Selection.GetNormalizedBufferSelection( nDontCare, nStartRow, nDontCare, nEndRow );
		m_pActiveView->DamageView( nStartRow, nEndRow );
		if ( bUpdateNow )
		{
			SafeUpdateWindow();
		}
	}
}

void CEdit::OnChangeLineSelection()
{
	if ( m_nLastEdittedLine != -1 )
	{
		m_Buffer.NormalizeLineTextCase( m_nLastEdittedLine );
		m_nLastEdittedLine = -1;
	}
}

void CEdit::GuessCRLFStateFromText( LPCTSTR pszText )
{
	ASSERT( pszText );
	// look for a LF -- most files should have one of these
	LPCTSTR pszEOLN = _tcschr( pszText, _T('\n') );
	if ( pszEOLN )
	{
		// ok, we can hazard a guess
		if ( --pszEOLN >= pszText )
		{
			// look for \r\n and sync up the buffer state with what we find
			m_Buffer.EnableCarriageReturn( *pszEOLN == _T('\r') );
		}
	}

}

void CEdit::OnFontChanged()
{
	m_Selection.OnFontChanged( m_font->hFont );
	for ( int i = 0; i < m_nViews; i++ )
	{
		m_Views[ i ]->OnFontChanged( m_font->hFont, m_cxChar );
	}
}

TCHAR g_szFaceName[ LF_FACESIZE ];

// FontProc is used to select a good default font for all windows
int CALLBACK FontProc2( ENUMLOGFONT *pelf, NEWTEXTMETRIC * /*pntm*/, int /*nFontType*/, LOGFONT *plfOut )
{
	if ( _tcscmp( pelf->elfLogFont.lfFaceName, g_szFaceName ) == 0 ) // check if facename is supported
	{
		*plfOut = pelf->elfLogFont;
		return 0;
	}
	return 1; // keep searching
}

void CEdit::RemapAnsiFontToKeyboardLocale()
	{
	LOGFONT lf;
	if ( !GetObject( m_font->hFont, sizeof( lf ), &lf ) )
		return;

	WORD wKBLocale = LOWORD( ( DWORD ) GetKeyboardLayout( 0 ) );

	BYTE byCharSetNew = lf.lfCharSet;

	switch ( wKBLocale )
	{
		case LCID_AFRIKAANS:
		case LCID_DANISH:
		case LCID_GERMAN:
		case LCID_SWISSGERMAN:
		case LCID_AMERICAN:
		case LCID_BRITISH:
		case LCID_AUSTRALIAN:
		case LCID_SPANISH:
		case LCID_FINNISH:
		case LCID_FRENCH:
		case LCID_FRENCHCANADIAN:
		case LCID_ITALIAN:
		case LCID_DUTCH:
		case LCID_DUTCHPREFERRED:
		case LCID_NORSKBOKMAL:
		case LCID_NORSKNYNORSK:
		case LCID_PORTBRAZIL:
		case LCID_PORTIBERIAN:
		case LCID_SWEDISH:
		case LCID_CATALAN:
		case LCID_BASQUE:
		case LCID_ICELANDIC:
		case LCID_MALAYSIAN:
			byCharSetNew = ANSI_CHARSET;
			break;
		case LCID_ESTONIAN:
		case LCID_LATVIAN:
		case LCID_LITHUANIAN:
			byCharSetNew = BALTIC_CHARSET;
			break;
		case LCID_URDU:
		case LCID_FARSI:
		case LCID_ARABIC:
			byCharSetNew = ARABIC_CHARSET;
			break;
		case LCID_HEBREW:
		case LCID_YIDDISH:
			byCharSetNew = HEBREW_CHARSET;
			break;
		case LCID_THAI:
			byCharSetNew = THAI_CHARSET;
			break;
		case LCID_RUSSIAN:
		case LCID_BYELORUSSIAN:
		case LCID_BULGARIAN:
		case LCID_SERBIANCYRILLIC:
		case LCID_UKRAINIAN:
		case LCID_AZERICYRILLIC:
		case LCID_KAZAKH:
		case LCID_TATAR:
		case LCID_UZBEKCYRILLIC:
		case LCID_KYRGYZ:
		case LCID_TAJIK:
		case LCID_TURKMEN:
		case LCID_ARMENIAN:
		case LCID_GEORGIAN:
		case LCID_MACEDONIAN:
			byCharSetNew = RUSSIAN_CHARSET;
			break;
		case LCID_AZERILATIN:
		case LCID_UZBEKLATIN:
		case LCID_TURKISH:
			byCharSetNew = TURKISH_CHARSET;
			break;
		case LCID_CZECH:
		case LCID_HUNGARIAN:
		case LCID_POLISH:
		case LCID_CROATIAN:
		case LCID_ROMANIAN:
		case LCID_SERBIANLATIN:
		case LCID_SLOVAK:
		case LCID_SLOVENIAN:
			byCharSetNew = EASTEUROPE_CHARSET;
			break;
		case LCID_GREEK:
			byCharSetNew = GREEK_CHARSET;
			break;
		case LCID_JAPANESE:
			byCharSetNew = SHIFTJIS_CHARSET;
			break;
		case LCID_KOREAN:
			byCharSetNew = HANGEUL_CHARSET;
			break;
		case LCID_CHINA:
			byCharSetNew = GB2312_CHARSET;
			break;
		case LCID_TAIWAN:
			byCharSetNew = CHINESEBIG5_CHARSET;
			break;
		case LCID_VIETNAMESE:
			byCharSetNew = VIETNAMESE_CHARSET;
			break;
	}

	// if the charset has changed to something other than ANSI (which all fonts
	// can already render), then look for a newer version of the font that can
	// handle the new charset.
	if ( byCharSetNew != lf.lfCharSet &&
	     byCharSetNew != ANSI_CHARSET &&
		 byCharSetNew != DEFAULT_CHARSET )
	{
		// the charset has changed.  Switch the current font over to the new font
		// if the desired font is installed on the machine.
		_tcscpy( g_szFaceName, lf.lfFaceName );

		LOGFONT lf2;
		ZeroMemory( &lf2, sizeof( lf2 ) );
		lf2.lfCharSet = byCharSetNew;
		lf2.lfPitchAndFamily = lf.lfPitchAndFamily;
		LOGFONT lfNew;
		ZeroMemory( &lfNew, sizeof( lfNew ) );

		HDC hDC = GetDC( NULL );
		EnumFontFamiliesEx( hDC, &lf2, ( FONTENUMPROC ) FontProc2, ( LPARAM ) &lfNew, 0 );

		if ( !*lfNew.lfFaceName )
			return;

		lfNew.lfHeight = lf.lfHeight;
		lfNew.lfWidth = lf.lfWidth;
		HFONT hFontNew = CreateFontIndirect( &lfNew );

		if ( hFontNew )
		{
			OnSetFont( ( WPARAM ) hFontNew, 0 );
			// control should take ownership of the font
			m_bOwnFont = TRUE;
		}
	}
}

void CEdit::EndSelecting()
{
	KillTimer( m_hWnd, TIMER_DRAG_SELECT );
	ReleaseCapture();
	ASSERT( GetCapture() != m_hWnd );
	if ( m_Selection.IsEmpty() )
	{
		// selection is empty -- make sure column selection is off
		m_Selection.EnableColumnSel( FALSE );
	}
	SetMode( eIdle );
}

void CEdit::SaveViewSelState()
{
	CViewSelState *pState = &m_stateBegin;
	ASSERT(pState);
	pState->m_nLeftIndex = m_pActiveView->GetLeftIndex();
	pState->m_nTopIndex = m_pActiveView->GetTopIndex();
	m_Selection.GetBufferSelection( pState->m_nStartCol, 
			  					    pState->m_nStartRow,
									pState->m_nEndCol, 
									pState->m_nEndRow );
}

void CEdit::ProcessViewSelNotifications()
{
	BOOL bDirty = FALSE;
	// issue selection and scroll notifications if anything changed
	CViewSelState *pState = &m_stateBegin;
	if ( m_pActiveView->GetLeftIndex() != pState->m_nLeftIndex )
	{
		CMTRACE0( _T("CMN_HSCROLL\r\n") );
		NotifyParent( CMN_HSCROLL );
		bDirty = TRUE;
	}

	if ( m_pActiveView->GetTopIndex() != pState->m_nTopIndex )
	{
		CMTRACE0( _T("CMN_VSCROLL\r\n") );
		NotifyParent( CMN_VSCROLL );
		bDirty = TRUE;
	}

	int nStartCol, nStartRow, nEndCol, nEndRow;
	m_Selection.GetBufferSelection( nStartCol, nStartRow, nEndCol, nEndRow );
	if ( ( nStartCol != pState->m_nStartCol ) ||
		 ( nStartRow != pState->m_nStartRow ) ||
		 ( nEndCol != pState->m_nEndCol ) ||
		 ( nEndRow != pState->m_nEndRow ) )
	{
		CMTRACE0( _T("CMN_SELCHANGE\r\n") );
		NotifyParent( CMN_SELCHANGE );
		bDirty = TRUE;
	}
	
	if (bDirty)
		SaveViewSelState();
}

void CEdit::SetHighlightedLine(int nLine, BOOL bRaw)
	{
	if (bRaw)
		m_nHighlightedLine = nLine;
	else
		{
		// turn off previous (if any) highlighting
		if ( m_nHighlightedLine != -1 )
		{
			m_Buffer.SetHighlighted( m_nHighlightedLine, FALSE );
			m_pActiveView->DamageView( m_nHighlightedLine, m_nHighlightedLine ); 
		}

		m_nHighlightedLine = nLine;

		// turn off new highlighting (if any)
		if ( m_nHighlightedLine != -1 )
		{
			m_Buffer.SetHighlighted( m_nHighlightedLine, TRUE );
			m_pActiveView->DamageView( m_nHighlightedLine, m_nHighlightedLine ); 
		}
	}
}
