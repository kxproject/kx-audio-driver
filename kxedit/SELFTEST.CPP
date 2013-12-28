#include "precomp.h"
#include "cedit.h"
#ifdef _ACTIVEX
#include "editx.h"
#endif

#ifdef _DEBUG
extern HINSTANCE hInstance;

void CEdit::ValidateSymmetricProp( UINT uMsgSet, UINT uMsgGet, DWORD dwPropVal ) const
{
	DWORD dwPropLast = SendMessage( m_hWndTest, uMsgGet, 0, 0 );
	ASSERT( dwPropLast != dwPropVal ); // you didn't pick a good test value!
	ASSERT( SendMessage( m_hWndTest, uMsgSet, dwPropVal, 0 ) == CME_SUCCESS );
	ASSERT( SendMessage( m_hWndTest, uMsgGet, 0, 0 ) == ( LRESULT ) dwPropVal ); 
	ASSERT( SendMessage( m_hWndTest, uMsgSet, dwPropLast, 0 ) == CME_SUCCESS );
}

void CEdit::DoSelfTest()
{
	if ( m_bSelfTest )
	{
		// cancel
		m_bSelfTest = FALSE;
		InvalidateRect( m_hWnd, NULL, TRUE );
		ReleaseCapture();
		ASSERT( GetCapture() != m_hWnd );
		return;
	}
	else
	{
		m_bSelfTest = TRUE;
		SetRectEmpty( &m_rcLastPosTest );

	}

	RECT rc = { 0, 0, 100, 100 };
	
	HWND hWndStatic = CreateWindow( _T("STATIC"), 
	                           NULL, 
	                           WS_CHILD,
	                           rc.left, 
	                           rc.top, 
	                           rc.right - rc.left, 
	                           rc.bottom - rc.top,
	                           m_hWnd,
	                           NULL, 
	                           hInstance,
	                           NULL );
	ASSERT( hWndStatic );
	
	m_hWndTest = CreateWindow( CODEMAXWNDCLASS, 
	                           NULL, 
	                           WS_CHILD | WS_VSCROLL | WS_HSCROLL,
	                           rc.left, 
	                           rc.top, 
	                           rc.right - rc.left, 
	                           rc.bottom - rc.top,
	                           hWndStatic,
	                           NULL, 
	                           hInstance,
	                           NULL );

	ASSERT( m_hWndTest );

	ValidateSymmetricProp( CMM_SETTABSIZE, CMM_GETTABSIZE, 20 );
	ValidateSymmetricProp( CMM_ENABLECOLORSYNTAX, CMM_ISCOLORSYNTAXENABLED, TRUE );
	ValidateSymmetricProp( CMM_ENABLEWHITESPACEDISPLAY, CMM_ISWHITESPACEDISPLAYENABLED, TRUE );
	ValidateSymmetricProp( CMM_ENABLETABEXPAND, CMM_ISTABEXPANDENABLED, TRUE );
	ValidateSymmetricProp( CMM_ENABLESMOOTHSCROLLING, CMM_ISSMOOTHSCROLLINGENABLED, TRUE );
	ValidateSymmetricProp( CMM_SETREADONLY, CMM_ISREADONLY, TRUE );
	ValidateSymmetricProp( CMM_ENABLELINETOOLTIPS, CMM_ISLINETOOLTIPSENABLED, FALSE );
	ValidateSymmetricProp( CMM_ENABLELEFTMARGIN, CMM_ISLEFTMARGINENABLED, FALSE );
	if ( !DBCS_ACTIVE )
		ValidateSymmetricProp( CMM_ENABLECOLUMNSEL, CMM_ISCOLUMNSELENABLED, FALSE );
	ValidateSymmetricProp( CMM_ENABLEDRAGDROP, CMM_ISDRAGDROPENABLED, FALSE );
	ValidateSymmetricProp( CMM_ENABLEOVERTYPE, CMM_ISOVERTYPEENABLED, TRUE );
	ValidateSymmetricProp( CMM_ENABLECASESENSITIVE, CMM_ISCASESENSITIVEENABLED, TRUE );
	ValidateSymmetricProp( CMM_ENABLEPRESERVECASE, CMM_ISPRESERVECASEENABLED, TRUE );
	ValidateSymmetricProp( CMM_ENABLEWHOLEWORD, CMM_ISWHOLEWORDENABLED, TRUE );
	ValidateSymmetricProp( CMM_ENABLEREGEXP, CMM_ISREGEXPENABLED, TRUE );
	ValidateSymmetricProp( CMM_ENABLECRLF, CMM_ISCRLFENABLED, FALSE );
	ValidateSymmetricProp( CMM_SETAUTOINDENTMODE, CMM_GETAUTOINDENTMODE, CM_INDENT_OFF );
	ValidateSymmetricProp( CMM_SETUNDOLIMIT, CMM_GETUNDOLIMIT, 20 );
	ValidateSymmetricProp( CMM_ENABLEGLOBALPROPS, CMM_ISGLOBALPROPSENABLED, FALSE );
	ValidateSymmetricProp( CMM_ENABLESELBOUNDS, CMM_ISSELBOUNDSENABLED, TRUE );
	ValidateSymmetricProp( CMM_ENABLEHIDESEL, CMM_ISHIDESELENABLED, FALSE );
	ValidateSymmetricProp( CMM_ENABLENORMALIZECASE, CMM_ISNORMALIZECASEENABLED, TRUE );
	ValidateSymmetricProp( CMM_ENABLEOVERTYPECARET, CMM_ISOVERTYPECARETENABLED, FALSE );

	ASSERT( SendMessage( m_hWndTest, CMM_ISMODIFIED, 0, 0 ) == FALSE );
	ValidateSymmetricProp( CMM_SETMODIFIED, CMM_ISMODIFIED, TRUE );

	HFONT hFont = CreateFont( 15, 0, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, 0, 0, _T("Courier New") );
	SendMessage( m_hWndTest, WM_SETFONT, ( WPARAM ) hFont, 0 );
	ValidateSymmetricProp( CMM_SETFONTOWNERSHIP, CMM_GETFONTOWNERSHIP, TRUE );

	ASSERT( CM_AddText( m_hWndTest, _T("x") ) == CME_SUCCESS );
	CM_Undo( m_hWndTest );
	ASSERT( CM_ReplaceSel( m_hWndTest, _T("x") ) == CME_SUCCESS );
	CM_Undo( m_hWndTest );
	ASSERT( CM_ReplaceText( m_hWndTest, _T("x"), NULL ) == CME_SUCCESS );
	CM_Undo( m_hWndTest );
	ASSERT( CM_InsertText( m_hWndTest, _T("x"), NULL ) == CME_SUCCESS );
	CM_Undo( m_hWndTest );
	ASSERT( CM_InsertLine( m_hWndTest, 0, _T("x") ) == CME_SUCCESS );
	CM_Undo( m_hWndTest );
	ASSERT( CM_GetLineCount( m_hWndTest ) == 0 );

	LPCTSTR pszTestLines[] = {
	    _T("This is Line 0"),
	    _T("This is Line 1"),
	    _T("This is Line 2"),
	    _T("This is Line 3"),
	    _T("This is Line 4"),
	    _T("This is Line 5"),
	    _T("This is Line 6"),
	    _T("This is Line 7"),
	    _T("This is Line 8"),
	    _T("This is Line 9"),
	    _T("This is Line 10"),
	    _T("This is Line 11"),
	    _T("This is Line 12"),
	    _T("This is Line 13"),
	    _T("This is Line 14"),
	    _T("This is Line 15"),
	    _T("This is Line 16"),
	    _T("This is Line 17"),
	    _T("This is Line 18"),
	    _T("This is Line 19")
	};

	// test text insertion using all five methods at once
	const int nTestLines = ARRAY_SIZE( pszTestLines );
	ASSERT( nTestLines >= 5 );

	ASSERT( CM_GetTopIndex( m_hWndTest, 0 ) == 0 );
	for ( int i = 0; i < nTestLines; i++ )
	{
		switch ( i % 5 )
		{
			case 0:  // CMM_INSERTLINE
			{
				ASSERT( CM_InsertLine( m_hWndTest, i, pszTestLines[ i ] ) == CME_SUCCESS );
				ASSERT( CM_GetLineCount( m_hWndTest ) == i + 1 );
				ASSERT( CM_GetTopIndex( m_hWndTest, 0 ) == 0 );
				break;
			}
			case 1:  // CMM_INSERTTEXT
			{
				CM_POSITION cmPos;
				cmPos.nLine = i;
				cmPos.nCol = 0;
				ASSERT( CM_AddText( m_hWndTest, _T("\n") ) == CME_SUCCESS );
				ASSERT( CM_GetLineCount( m_hWndTest ) == i + 1 );
				ASSERT( CM_InsertText( m_hWndTest, pszTestLines[ i ], &cmPos ) == CME_SUCCESS );
				ASSERT( CM_GetLineCount( m_hWndTest ) == i + 1 );
				ASSERT( CM_GetTopIndex( m_hWndTest, 0 ) == 0 );
				break;
			}
			case 2:  // CMM_REPLACETEXT
			{
				CM_RANGE cmRange;
				ZeroMemory( &cmRange, sizeof( cmRange ) );
				ASSERT( CM_AddText( m_hWndTest, _T("\n") ) == CME_SUCCESS );
				ASSERT( CM_GetLineCount( m_hWndTest ) == i + 1 );
				cmRange.posStart.nLine = cmRange.posEnd.nLine = i;
				cmRange.posStart.nCol = cmRange.posEnd.nCol = 0;
				ASSERT( CM_ReplaceText( m_hWndTest, pszTestLines[ i ], &cmRange ) == CME_SUCCESS );
				ASSERT( CM_GetLineCount( m_hWndTest ) == i + 1 );
				ASSERT( CM_GetTopIndex( m_hWndTest, 0 ) == 0 );
				break;
			}
			case 3:  // CMM_REPLACESEL
			{
				CM_RANGE cmRange;
				ZeroMemory( &cmRange, sizeof( cmRange ) );
				ASSERT( CM_AddText( m_hWndTest, _T("\n") ) == CME_SUCCESS );
				ASSERT( CM_GetLineCount( m_hWndTest ) == i + 1 );
				cmRange.posStart.nLine = cmRange.posEnd.nLine = i;
				cmRange.posStart.nCol = cmRange.posEnd.nCol = 0;
				ASSERT( CM_SetSel( m_hWndTest, &cmRange, FALSE ) == CME_SUCCESS ); 
				ASSERT( CM_ReplaceSel( m_hWndTest, pszTestLines[ i ] ) == CME_SUCCESS );
				ASSERT( CM_GetLineCount( m_hWndTest ) == i + 1 );
				ASSERT( CM_GetTopIndex( m_hWndTest, 0 ) == 0 );
				break;
			}
			case 4:  // CMM_ADDTEXT
			{
				ASSERT( CM_AddText( m_hWndTest, _T("\n") ) == CME_SUCCESS );
				ASSERT( CM_GetLineCount( m_hWndTest ) == i + 1 );
				ASSERT( CM_AddText( m_hWndTest, pszTestLines[ i ] ) == CME_SUCCESS );
				ASSERT( CM_GetLineCount( m_hWndTest ) == i + 1 );
				ASSERT( CM_GetTopIndex( m_hWndTest, 0 ) == 0 );
				break;
			}
		}
	}   

	// Verify contents are what were entered
	ASSERT( CM_GetLineCount( m_hWndTest ) == nTestLines );
	ASSERT( CM_GetTopIndex( m_hWndTest, 0 ) == 0 );
	ASSERT( CM_SetTopIndex( m_hWndTest, 0, 2 ) == CME_SUCCESS );
	ASSERT( CM_GetTopIndex( m_hWndTest, 0 ) == 2 );

	// bookmark tests
	ASSERT( nTestLines >= 5 );
	ASSERT( CM_SetBookmark( m_hWndTest, 0, TRUE ) == CME_SUCCESS );
	ASSERT( CM_SetBookmark( m_hWndTest, 3, TRUE ) == CME_SUCCESS );
	ASSERT( CM_SetBookmark( m_hWndTest, 4, TRUE ) == CME_SUCCESS );
	ASSERT( CM_GetAllBookmarks( m_hWndTest, NULL ) == 3 );
	ASSERT( CM_SetBookmark( m_hWndTest, 4, FALSE ) == CME_SUCCESS );
	ASSERT( CM_GetAllBookmarks( m_hWndTest, NULL ) == 2 );
	ASSERT( CM_GetBookmark( m_hWndTest, 0 ) == TRUE );
	ASSERT( CM_GetBookmark( m_hWndTest, 1 ) == FALSE );
	ASSERT( CM_GetBookmark( m_hWndTest, 2 ) == FALSE );
	ASSERT( CM_GetBookmark( m_hWndTest, 3 ) == TRUE );
	ASSERT( CM_GetBookmark( m_hWndTest, 4 ) == FALSE );
	DWORD dwLines[] = { 1, 2, 4 };
	ASSERT( CM_SetAllBookmarks( m_hWndTest, ARRAY_SIZE( dwLines ), &dwLines ) == CME_SUCCESS );
	ASSERT( CM_GetBookmark( m_hWndTest, 0 ) == FALSE );
	ASSERT( CM_GetBookmark( m_hWndTest, 1 ) == TRUE );
	ASSERT( CM_GetBookmark( m_hWndTest, 2 ) == TRUE );
	ASSERT( CM_GetBookmark( m_hWndTest, 3 ) == FALSE );
	ASSERT( CM_GetBookmark( m_hWndTest, 4 ) == TRUE );
	DWORD dwLines2[ ARRAY_SIZE( dwLines ) ];
	ASSERT( CM_GetAllBookmarks( m_hWndTest, NULL ) == ARRAY_SIZE( dwLines ) );
	ASSERT( CM_GetAllBookmarks( m_hWndTest, &dwLines2 ) == ARRAY_SIZE( dwLines ) );
	ASSERT( memcmp( dwLines2, dwLines, ARRAY_SIZE( dwLines ) ) == 0 );

	ASSERT( CM_SetHighlightedLine( m_hWndTest, 4 ) == CME_SUCCESS );
	ASSERT( CM_GetHighlightedLine( m_hWndTest ) == 4 );
	ASSERT( CM_SetHighlightedLine( m_hWndTest, -1 ) == CME_SUCCESS );
	ASSERT( CM_GetHighlightedLine( m_hWndTest ) == -1 );

	ASSERT( CM_SetDivider( m_hWndTest, 0, TRUE ) == CME_SUCCESS );
	ASSERT( CM_SetDivider( m_hWndTest, 3, TRUE ) == CME_SUCCESS );
	ASSERT( CM_GetDivider( m_hWndTest, 0 ) == TRUE );
	ASSERT( CM_GetDivider( m_hWndTest, 1 ) == FALSE );
	ASSERT( CM_GetDivider( m_hWndTest, 3 ) == TRUE );
	ASSERT( CM_SetDivider( m_hWndTest, 3, FALSE ) == CME_SUCCESS );
	ASSERT( CM_GetDivider( m_hWndTest, 3 ) == FALSE );

	ASSERT( CM_GetMarginImages( m_hWndTest, 3 ) == 0 );
	ASSERT( CM_SetMarginImages( m_hWndTest, 3, 6 ) == CME_SUCCESS );
	ASSERT( CM_GetMarginImages( m_hWndTest, 3 ) == 6 );
	ASSERT( CM_SetMarginImages( m_hWndTest, 3, 0 ) == CME_SUCCESS );
	ASSERT( CM_GetMarginImages( m_hWndTest, 3 ) == 0 );

	for ( i = nTestLines - 1; i >= 0; i-- )
	{
		int cbLine = CM_GetLineLength( m_hWndTest, i, FALSE );
		ASSERT( cbLine == ( int ) _tcslen( pszTestLines[ i ] ) );
		LPTSTR psz = new TCHAR[ cbLine + 1 ];
		ASSERT( CM_GetLine( m_hWndTest, i, psz ) == CME_SUCCESS );
		ASSERT( !psz[ cbLine ] );
		ASSERT( !_tcscmp( psz, pszTestLines[ i ] ) );
		delete psz;
		CM_DeleteLine( m_hWndTest, i );
		ASSERT( CM_GetLineCount( m_hWndTest ) == i );
	}

	// Verify undo/redo
	ASSERT( CM_IsModified( m_hWndTest ) );
	ASSERT( CM_CanUndo( m_hWndTest ) );
	while ( CM_CanUndo( m_hWndTest ) )
	{
		CM_Undo( m_hWndTest );
		ASSERT( CM_CanRedo( m_hWndTest ) );
	}
	ASSERT( !CM_IsModified( m_hWndTest ) );
	while ( CM_CanRedo( m_hWndTest ) )
	{
		CM_Redo( m_hWndTest );
		ASSERT( CM_CanUndo( m_hWndTest ) );
	}
	ASSERT( CM_IsModified( m_hWndTest ) );
	CM_ClearUndoBuffer( m_hWndTest );
	ASSERT( !CM_CanUndo( m_hWndTest ) );
	ASSERT( !CM_CanRedo( m_hWndTest ) );

	ASSERT( CM_GetViewCount( m_hWndTest ) == 1 );
	ASSERT( CM_GetSplitterPos( m_hWndTest, TRUE ) == 0 );
	ASSERT( CM_SetSplitterPos( m_hWndTest, TRUE, 40 ) == CME_SUCCESS );
	ASSERT( CM_GetViewCount( m_hWndTest ) == 2 );
	ASSERT(CM_SetCurrentView(m_hWndTest,0) == CME_SUCCESS);
	ASSERT(CM_SetCurrentView(m_hWndTest,3) == CME_BADARGUMENT);
	ASSERT(CM_SetCurrentView(m_hWndTest,4) == CME_BADARGUMENT);
	ASSERT(CM_SetCurrentView(m_hWndTest,1) == CME_SUCCESS);
	ASSERT(CM_GetCurrentView(m_hWndTest) == 1);
	ASSERT(CM_SetCurrentView(m_hWndTest,0) == CME_SUCCESS);
	ASSERT(CM_GetSplitterPos(m_hWndTest, true) == 40);
	ASSERT(CM_SetSplitterPos(m_hWndTest, true, 0) == CME_SUCCESS);
	ASSERT(CM_GetSplitterPos(m_hWndTest, true) == 0);
	ASSERT(CM_GetViewCount(m_hWndTest) == 1);
	ASSERT(CM_SetSplitterPos(m_hWndTest, false, 0) == CME_SUCCESS);
	ASSERT(CM_GetSplitterPos(m_hWndTest, false) == 0);
	ASSERT(CM_GetViewCount(m_hWndTest) == 1);
	ASSERT(CM_GetCurrentView(m_hWndTest) == 0);

	ASSERT( CM_IsSplitterEnabled( m_hWndTest, TRUE ) );
	ASSERT( CM_EnableSplitter( m_hWndTest, TRUE, FALSE ) == CME_SUCCESS );
	ASSERT( !CM_IsSplitterEnabled( m_hWndTest, TRUE ) );
	ASSERT( CM_EnableSplitter( m_hWndTest, TRUE, TRUE ) == CME_SUCCESS );
	ASSERT( CM_IsSplitterEnabled( m_hWndTest, TRUE ) );
	ASSERT( CM_IsSplitterEnabled( m_hWndTest, FALSE ) );
	ASSERT( CM_EnableSplitter( m_hWndTest, FALSE, FALSE ) == CME_SUCCESS );
	ASSERT( !CM_IsSplitterEnabled( m_hWndTest, FALSE ) );
	ASSERT( CM_EnableSplitter( m_hWndTest, FALSE, TRUE ) == CME_SUCCESS );
	ASSERT( CM_IsSplitterEnabled( m_hWndTest, FALSE ) );

	ASSERT( CM_HasScrollBar( m_hWndTest, TRUE ) );
	ASSERT( CM_ShowScrollBar( m_hWndTest, TRUE, FALSE ) == CME_SUCCESS );
	ASSERT( !CM_HasScrollBar( m_hWndTest, TRUE ) );
	ASSERT( CM_HasScrollBar( m_hWndTest, FALSE ) );
	ASSERT( CM_ShowScrollBar( m_hWndTest, FALSE, FALSE ) == CME_SUCCESS );
	ASSERT( !CM_HasScrollBar( m_hWndTest, FALSE ) );

	ASSERT( CM_SetBorderStyle( m_hWndTest, CM_BORDER_CORRAL ) == CME_SUCCESS );
	ASSERT( CM_GetBorderStyle( m_hWndTest ) == CM_BORDER_CORRAL );
	ASSERT( CM_SetBorderStyle( m_hWndTest, CM_BORDER_NONE ) == CME_SUCCESS );
	ASSERT( CM_GetBorderStyle( m_hWndTest ) == CM_BORDER_NONE );
	ASSERT( CM_SetBorderStyle( m_hWndTest, CM_BORDER_THIN ) == CME_SUCCESS );
	ASSERT( CM_GetBorderStyle( m_hWndTest ) == CM_BORDER_THIN );

	// Verify visible line count
	ASSERT( CM_GetVisibleLineCount( m_hWndTest, 0, FALSE ) == 7 );  // Ceil(100 pixels / 15 pixels)
	ASSERT( CM_GetVisibleLineCount( m_hWndTest, 0, TRUE ) == 6 );   // Floor(100 pixels / 15 pixels)

	ASSERT( !CM_IsRecordingMacro( m_hWndTest ) );
	ASSERT( CM_ExecuteCmd( m_hWndTest, CMD_RECORDMACRO, 0 ) == CME_SUCCESS );
	ASSERT( CM_IsRecordingMacro( m_hWndTest ) );
	ASSERT( CM_ExecuteCmd( m_hWndTest, CMD_RECORDMACRO, TRUE ) == CME_SUCCESS );
	ASSERT( !CM_IsPlayingMacro( m_hWndTest ) );

	LPCTSTR pszText = _T("AB\r\nCD\r\n");
	::SendMessage( m_hWndTest, WM_SETTEXT, 0, ( LPARAM ) pszText );
	ASSERT( CM_GetTextLength( m_hWndTest, NULL, FALSE ) == ( int ) _tcslen( pszText ) );
	LPCTSTR pszText2 = _T("AB\r\nCDEF\r\n");
	ASSERT( CM_SetText( m_hWndTest, pszText2 ) == CME_SUCCESS );
	ASSERT( CM_GetTextLength( m_hWndTest, NULL, FALSE ) == ( int ) _tcslen( pszText2 ) );
	TCHAR szText[ 100 ];
	ASSERT( CM_GetText( m_hWndTest, szText, NULL ) == CME_SUCCESS );

	ASSERT( CM_GetLineLength( m_hWndTest, 0, FALSE ) == 2 );
	CM_SelectLine( m_hWndTest, 0, FALSE );
	CM_DeleteSel( m_hWndTest );
	ASSERT( CM_GetLineLength( m_hWndTest, 0, FALSE ) == 0 );
	CM_Undo( m_hWndTest );
	CM_RANGE cmRange = { { 0, 1 }, { 1, 1 }, FALSE };
	ASSERT( CM_SetSel( m_hWndTest, &cmRange, FALSE ) == CME_SUCCESS );
	CM_RANGE cmRange2;
	ASSERT( CM_GetSel( m_hWndTest, &cmRange2, FALSE ) == CME_SUCCESS );
	ASSERT( memcmp( &cmRange, &cmRange2, sizeof( CM_RANGE ) ) == 0 );
	ASSERT( CM_GetTextLength( m_hWndTest, &cmRange, FALSE ) == 4 );

	ASSERT( CM_SetItemData( m_hWndTest, 0, 0x1234567 ) == CME_SUCCESS );
	ASSERT( CM_GetItemData( m_hWndTest, 0 ) == 0x1234567 );
	ASSERT( CM_SetLineStyle( m_hWndTest, 0, 0x1234567 ) == CME_SUCCESS );
	ASSERT( CM_GetLineStyle( m_hWndTest, 0 ) == 0x1234567 );

	CM_DeleteSel( m_hWndTest );

	TCHAR szLang[ CM_MAX_LANGUAGE_NAME + 1 ];
	*szLang = 1;
	ASSERT( CM_GetLanguage( m_hWndTest, szLang ) == CME_SUCCESS );
	ASSERT( !*szLang );
	ASSERT( CM_SetLanguage( m_hWndTest, CMLANG_PASCAL ) == CME_SUCCESS );
	ASSERT( CM_GetLanguage( m_hWndTest, szLang ) == CME_SUCCESS );
	ASSERT( _tcscmp( szLang, CMLANG_PASCAL ) == 0 );

	CM_COLORS cmColors = {
		RGB( 0, 255, 0 ),    // window background color
		RGB( 0, 255, 0 ),    // left margin background color
		RGB( 0, 255, 0 ),    // bookmark foreground color
		RGB( 0, 255, 0 ),    // bookmark background color
		RGB( 0, 255, 0 ),    // plain text foreground color
		RGB( 0, 255, 0 ),    // plain text background color
		RGB( 0, 255, 0 ),    // numeric literal foreground color
		RGB( 0, 255, 0 ),    // numeric literal background color
		RGB( 0, 255, 0 ),    // keyword foreground color
		RGB( 0, 255, 0 ),    // keyword background color
		RGB( 0, 255, 0 ),    // operator foreground color
		RGB( 0, 255, 0 ),    // operator background color
		RGB( 0, 255, 0 ),    // scope keyword foreground color
		RGB( 0, 255, 0 ),    // scope keyword background color
		RGB( 0, 255, 0 ),    // comment foreground color
		RGB( 0, 255, 0 ),    // comment background color
		RGB( 0, 255, 0 ),    // string foreground color
		RGB( 0, 255, 0 )     // string background color
	};
	ASSERT( CM_SetColors( m_hWndTest, &cmColors ) == CME_SUCCESS );
	CM_COLORS cmColors2;
	ASSERT( CM_GetColors( m_hWndTest, &cmColors2 ) == CME_SUCCESS );
	ASSERT( memcmp( &cmColors, &cmColors2, sizeof( CM_COLORS ) ) == 0 );

	CM_FONTSTYLES cmFontStyles = {
		CM_FONT_NORMAL,			// plain text foreground font style
		CM_FONT_BOLD,			// numeric literal foreground font style
		CM_FONT_ITALIC,			// keyword foreground font style
		CM_FONT_BOLDITALIC,		// operator foreground font style
		CM_FONT_BOLD,			// scope keyword foreground font style
		CM_FONT_ITALIC,			// comment foreground font style
		CM_FONT_UNDERLINE,		// string foreground font style
	};
	ASSERT( CM_SetFontStyles( m_hWndTest, &cmFontStyles ) == CME_SUCCESS );
	CM_FONTSTYLES cmFontStyles2;
	ASSERT( CM_GetFontStyles( m_hWndTest, &cmFontStyles2 ) == CME_SUCCESS );
	ASSERT( memcmp( &cmFontStyles, &cmFontStyles, sizeof( CM_FONTSTYLES ) ) == 0 );

	CM_LINENUMBERING cmLineNum = { TRUE, 100, CM_OCTAL };
	ASSERT( CM_SetLineNumbering( m_hWndTest, &cmLineNum ) == CME_SUCCESS );
	CM_LINENUMBERING cmLineNum2;
	ASSERT( CM_GetLineNumbering( m_hWndTest, &cmLineNum2 ) == CME_SUCCESS );
	ASSERT( cmLineNum.bEnabled == cmLineNum2.bEnabled );
	ASSERT( cmLineNum.nStartAt == cmLineNum2.nStartAt );
	ASSERT( cmLineNum.dwStyle == cmLineNum2.dwStyle );

	TCHAR szFindRepl[ 30 ];
	ASSERT( CM_SetFindText( m_hWndTest, _T("Hello") ) == CME_SUCCESS );
	ASSERT( CM_GetFindText( m_hWndTest, szFindRepl ) == CME_SUCCESS );
	ASSERT( _tcscmp( szFindRepl, _T("Hello") ) == 0 );

	ASSERT( CM_SetReplaceText( m_hWndTest, _T("World") ) == CME_SUCCESS );
	ASSERT( CM_GetReplaceText( m_hWndTest, szFindRepl ) == CME_SUCCESS );
	ASSERT( _tcscmp( szFindRepl, _T("World") ) == 0 );

	ASSERT( CM_GetImageList( m_hWndTest ) == NULL );
	ASSERT( CM_SetImageList( m_hWndTest, 0x1234 ) == CME_SUCCESS );
	ASSERT( CM_GetImageList( m_hWndTest ) == ( HIMAGELIST ) 0x1234 );
	ASSERT( CM_SetImageList( m_hWndTest, NULL ) == CME_SUCCESS );

	// make sure typelib constants are correct
#ifdef _ACTIVEX
	ASSERT( ( int ) cmMaxFindReplText			==	CM_MAX_FINDREPL_TEXT );
	ASSERT( ( int ) cmFindReplaceMRUMax			==	CM_FIND_REPLACE_MRU_MAX );
	ASSERT( ( int ) cmFindReplaceMRUBuffSize	==	CM_FIND_REPLACE_MRU_BUFF_SIZE );
	ASSERT( ( int ) cmMaxMacros					==	CM_MAX_MACROS );
	ASSERT( ( int ) cmMaxCmdString				==	CM_MAX_CMD_STRING );
	ASSERT( ( int ) cmMaxCmdDescription			==	CM_MAX_CMD_DESCRIPTION );
	ASSERT( ( int ) cmMaxLanguageName			==	CM_MAX_LANGUAGE_NAME );
	ASSERT( ( int ) cmMaxTabSize				==	CM_MAX_TABSIZE );
	ASSERT( ( int ) cmMinTabSize				==	CM_MIN_TABSIZE );

	ASSERT( ( int ) cmCmdWordUppercase			==	CMD_WORDUPPERCASE );
	ASSERT( ( int ) cmCmdWordTranspose			==	CMD_WORDTRANSPOSE );
	ASSERT( ( int ) cmCmdWordRightExtend		==	CMD_WORDRIGHTEXTEND );
	ASSERT( ( int ) cmCmdWordRight				==	CMD_WORDRIGHT );
	ASSERT( ( int ) cmCmdWordEndRight			==	CMD_WORDENDRIGHT );
	ASSERT( ( int ) cmCmdWordEndRightExtend		==	CMD_WORDENDRIGHTEXTEND );
	ASSERT( ( int ) cmCmdWordLowerCase			==	CMD_WORDLOWERCASE );
	ASSERT( ( int ) cmCmdWordLeftExtend			==	CMD_WORDLEFTEXTEND );
	ASSERT( ( int ) cmCmdWordLeft				==	CMD_WORDLEFT );
	ASSERT( ( int ) cmCmdWordEndLeft			==	CMD_WORDENDLEFT );
	ASSERT( ( int ) cmCmdWordEndLeftExtend		==	CMD_WORDENDLEFTEXTEND );
	ASSERT( ( int ) cmCmdWordDeleteToStart		==	CMD_WORDDELETETOSTART );
	ASSERT( ( int ) cmCmdWordDeleteToEnd		==	CMD_WORDDELETETOEND );
	ASSERT( ( int ) cmCmdWordCapitalize			==	CMD_WORDCAPITALIZE );
	ASSERT( ( int ) cmCmdWindowStart			==	CMD_WINDOWSTART );
	ASSERT( ( int ) cmCmdWindowScrollUp			==	CMD_WINDOWSCROLLUP );
	ASSERT( ( int ) cmCmdWindowScrollToTop		==	CMD_WINDOWSCROLLTOTOP );
	ASSERT( ( int ) cmCmdWindowScrollToCenter	==	CMD_WINDOWSCROLLTOCENTER );
	ASSERT( ( int ) cmCmdWindowScrollToBottom	==	CMD_WINDOWSCROLLTOBOTTOM );
	ASSERT( ( int ) cmCmdWindowScrollRight		==	CMD_WINDOWSCROLLRIGHT );
	ASSERT( ( int ) cmCmdWindowScrollLeft		==	CMD_WINDOWSCROLLLEFT );
	ASSERT( ( int ) cmCmdWindowScrollDown		==	CMD_WINDOWSCROLLDOWN );
	ASSERT( ( int ) cmCmdWindowRightEdge		==	CMD_WINDOWRIGHTEDGE );
	ASSERT( ( int ) cmCmdWindowLeftEdge			==	CMD_WINDOWLEFTEDGE );
	ASSERT( ( int ) cmCmdWindowEnd				==	CMD_WINDOWEND );
	ASSERT( ( int ) cmCmdUppercaseSelection		==	CMD_UPPERCASESELECTION );
	ASSERT( ( int ) cmCmdUntabifySelection		==	CMD_UNTABIFYSELECTION );
	ASSERT( ( int ) cmCmdUnindentSelection		==	CMD_UNINDENTSELECTION );
	ASSERT( ( int ) cmCmdUndoChanges			==	CMD_UNDOCHANGES );
	ASSERT( ( int ) cmCmdUndo					==	CMD_UNDO );
	ASSERT( ( int ) cmCmdTabifySelection		==	CMD_TABIFYSELECTION );
	ASSERT( ( int ) cmCmdSentenceRight			==	CMD_SENTENCERIGHT );
	ASSERT( ( int ) cmCmdSentenceLeft			==	CMD_SENTENCELEFT );
	ASSERT( ( int ) cmCmdSentenceCut			==	CMD_SENTENCECUT );
	ASSERT( ( int ) cmCmdSelectSwapAnchor		==	CMD_SELECTSWAPANCHOR );
	ASSERT( ( int ) cmCmdSelectPara				==	CMD_SELECTPARA );
	ASSERT( ( int ) cmCmdSelectLine				==	CMD_SELECTLINE );
	ASSERT( ( int ) cmCmdSelectAll				==	CMD_SELECTALL );
	ASSERT( ( int ) cmCmdRedoChanges			==	CMD_REDOCHANGES );
	ASSERT( ( int ) cmCmdRedo					==	CMD_REDO );
	ASSERT( ( int ) cmCmdPaste					==	CMD_PASTE );
	ASSERT( ( int ) cmCmdParaUp					==	CMD_PARAUP );
	ASSERT( ( int ) cmCmdParaDown				==	CMD_PARADOWN );
	ASSERT( ( int ) cmCmdPageUpExtend			==	CMD_PAGEUPEXTEND );
	ASSERT( ( int ) cmCmdPageUp					==	CMD_PAGEUP );
	ASSERT( ( int ) cmCmdPageDownExtend			==	CMD_PAGEDOWNEXTEND );
	ASSERT( ( int ) cmCmdPageDown				==	CMD_PAGEDOWN );
	ASSERT( ( int ) cmCmdLowercaseSelection		==	CMD_LOWERCASESELECTION );
	ASSERT( ( int ) cmCmdLineUpExtend			==	CMD_LINEUPEXTEND );
	ASSERT( ( int ) cmCmdLineUp					==	CMD_LINEUP );
	ASSERT( ( int ) cmCmdLineTranspose			==	CMD_LINETRANSPOSE );
	ASSERT( ( int ) cmCmdLineStart				==	CMD_LINESTART );
	ASSERT( ( int ) cmCmdLineOpenBelow			==	CMD_LINEOPENBELOW );
	ASSERT( ( int ) cmCmdLineOpenAbove			==	CMD_LINEOPENABOVE );
	ASSERT( ( int ) cmCmdLineEndExtend			==	CMD_LINEENDEXTEND );
	ASSERT( ( int ) cmCmdLineEnd				==	CMD_LINEEND );
	ASSERT( ( int ) cmCmdLineDownExtend			==	CMD_LINEDOWNEXTEND );
	ASSERT( ( int ) cmCmdLineDown				==	CMD_LINEDOWN );
	ASSERT( ( int ) cmCmdLineDeleteToStart		==	CMD_LINEDELETETOSTART );
	ASSERT( ( int ) cmCmdLineDeleteToEnd		==	CMD_LINEDELETETOEND );
	ASSERT( ( int ) cmCmdLineDelete				==	CMD_LINEDELETE );
	ASSERT( ( int ) cmCmdLineCut				==	CMD_LINECUT );
	ASSERT( ( int ) cmCmdIndentToPrev			==	CMD_INDENTTOPREV );
	ASSERT( ( int ) cmCmdIndentSelection		==	CMD_INDENTSELECTION );
	ASSERT( ( int ) cmCmdHomeExtend				==	CMD_HOMEEXTEND );
	ASSERT( ( int ) cmCmdHome					==	CMD_HOME );
	ASSERT( ( int ) cmCmdGotoMatchBrace			==	CMD_GOTOMATCHBRACE );
	ASSERT( ( int ) cmCmdGotoIndentation		==	CMD_GOTOINDENTATION );
	ASSERT( ( int ) cmCmdGotoLine				==	CMD_GOTOLINE );
	ASSERT( ( int ) cmCmdFindReplace			==	CMD_FINDREPLACE );
	ASSERT( ( int ) cmCmdReplace				==	CMD_REPLACE );
	ASSERT( ( int ) cmCmdReplaceAllInBuffer		==	CMD_REPLACEALLINBUFFER );
	ASSERT( ( int ) cmCmdReplaceAllInSelection	==	CMD_REPLACEALLINSELECTION );
	ASSERT( ( int ) cmCmdFindPrevWord			==	CMD_FINDPREVWORD );
	ASSERT( ( int ) cmCmdFindPrev				==	CMD_FINDPREV );
	ASSERT( ( int ) cmCmdFindNextWord			==	CMD_FINDNEXTWORD );
	ASSERT( ( int ) cmCmdFindNext				==	CMD_FINDNEXT );
	ASSERT( ( int ) cmCmdFindMarkAll			==	CMD_FINDMARKALL );
	ASSERT( ( int ) cmCmdFind					==	CMD_FIND );
	ASSERT( ( int ) cmCmdSetFindText			==	CMD_SETFINDTEXT );
	ASSERT( ( int ) cmCmdSetReplaceText			==	CMD_SETREPLACETEXT );
	ASSERT( ( int ) cmCmdTogglePreserveCase		==	CMD_TOGGLEPRESERVECASE );
	ASSERT( ( int ) cmCmdToggleWholeWord		==	CMD_TOGGLEWHOLEWORD );
	ASSERT( ( int ) cmCmdToggleCaseSensitive	==	CMD_TOGGLECASESENSITIVE );
	ASSERT( ( int ) cmCmdEnd					==	CMD_END );
	ASSERT( ( int ) cmCmdToggleWhitespaceDisplay==	CMD_TOGGLEWHITESPACEDISPLAY );
	ASSERT( ( int ) cmCmdToggleOvertype			==	CMD_TOGGLEOVERTYPE );
	ASSERT( ( int ) cmCmdSetRepeatCount			==	CMD_SETREPEATCOUNT );
	ASSERT( ( int ) cmCmdDocumentStartExtend	==	CMD_DOCUMENTSTARTEXTEND );
	ASSERT( ( int ) cmCmdDocumentStart			==	CMD_DOCUMENTSTART );
	ASSERT( ( int ) cmCmdDocumentEndExtend		==	CMD_DOCUMENTENDEXTEND );
	ASSERT( ( int ) cmCmdDocumentEnd			==	CMD_DOCUMENTEND );
	ASSERT( ( int ) cmCmdDeleteHorizontalSpace	==	CMD_DELETEHORIZONTALSPACE );
	ASSERT( ( int ) cmCmdDeleteBlankLines		==	CMD_DELETEBLANKLINES );
	ASSERT( ( int ) cmCmdDeleteBack				==	CMD_DELETEBACK );
	ASSERT( ( int ) cmCmdDelete					==	CMD_DELETE );
	ASSERT( ( int ) cmCmdCutSelection			==	CMD_CUTSELECTION );
	ASSERT( ( int ) cmCmdCut					==	CMD_CUT );
	ASSERT( ( int ) cmCmdCopy					==	CMD_COPY );
	ASSERT( ( int ) cmCmdCharTranspose			==	CMD_CHARTRANSPOSE );
	ASSERT( ( int ) cmCmdCharRightExtend		==	CMD_CHARRIGHTEXTEND );
	ASSERT( ( int ) cmCmdCharRight				==	CMD_CHARRIGHT );
	ASSERT( ( int ) cmCmdCharLeftExtend			==	CMD_CHARLEFTEXTEND );
	ASSERT( ( int ) cmCmdCharLeft				==	CMD_CHARLEFT );
	ASSERT( ( int ) cmCmdBookmarkToggle			==	CMD_BOOKMARKTOGGLE );
	ASSERT( ( int ) cmCmdBookmarkPrev			==	CMD_BOOKMARKPREV );
	ASSERT( ( int ) cmCmdBookmarkNext			==	CMD_BOOKMARKNEXT );
	ASSERT( ( int ) cmCmdBookmarkClearAll		==	CMD_BOOKMARKCLEARALL );
	ASSERT( ( int ) cmCmdBookmarkJumpToFirst	==	CMD_BOOKMARKJUMPTOFIRST );
	ASSERT( ( int ) cmCmdBookmarkJumpToLast		==	CMD_BOOKMARKJUMPTOLAST );
	ASSERT( ( int ) cmCmdAppendNextCut			==	CMD_APPENDNEXTCUT );
	ASSERT( ( int ) cmCmdInsertChar				==	CMD_INSERTCHAR );
	ASSERT( ( int ) cmCmdNewLine				==	CMD_NEWLINE );
	ASSERT( ( int ) cmCmdRecordMacro			==	CMD_RECORDMACRO );
	ASSERT( ( int ) cmCmdPlayMacro1				==	CMD_PLAYMACRO1 );
	ASSERT( ( int ) cmCmdPlayMacro2				==	CMD_PLAYMACRO2 );
	ASSERT( ( int ) cmCmdPlayMacro3				==	CMD_PLAYMACRO3 );
	ASSERT( ( int ) cmCmdPlayMacro4				==	CMD_PLAYMACRO4 );
	ASSERT( ( int ) cmCmdPlayMacro5				==	CMD_PLAYMACRO5 );
	ASSERT( ( int ) cmCmdPlayMacro6				==	CMD_PLAYMACRO6 );
	ASSERT( ( int ) cmCmdPlayMacro7				==	CMD_PLAYMACRO7 );
	ASSERT( ( int ) cmCmdPlayMacro8				==	CMD_PLAYMACRO8 );
	ASSERT( ( int ) cmCmdPlayMacro9				==	CMD_PLAYMACRO9 );
	ASSERT( ( int ) cmCmdPlayMacro10			==	CMD_PLAYMACRO10 );
	ASSERT( ( int ) cmCmdProperties				==	CMD_PROPERTIES );
	ASSERT( ( int ) cmCmdBeginUndo				==	CMD_BEGINUNDO );
	ASSERT( ( int ) cmCmdEndUndo				==	CMD_ENDUNDO );
	ASSERT( ( int ) cmCmdToggleRegExp			==	CMD_TOGGLEREGEXP );
	ASSERT( ( int ) cmCmdClearSelection			==	CMD_CLEARSELECTION );
	ASSERT( ( int ) cmCmdRegExpOn				==  CMD_REGEXPON );
	ASSERT( ( int ) cmCmdRegExpOff				==  CMD_REGEXPOFF );
	ASSERT( ( int ) cmCmdWholeWordOn			==  CMD_WHOLEWORDON );
	ASSERT( ( int ) cmCmdWholeWordOff			==  CMD_WHOLEWORDOFF );
	ASSERT( ( int ) cmCmdPreserveCaseOn			==  CMD_PRESERVECASEON );
	ASSERT( ( int ) cmCmdPreserveCaseOff		==  CMD_PRESERVECASEOFF );
	ASSERT( ( int ) cmCmdCaseSensitiveOn		==  CMD_CASESENSITIVEON );
	ASSERT( ( int ) cmCmdCaseSensitiveOff		==  CMD_CASESENSITIVEOFF );
	ASSERT( ( int ) cmCmdWhitespaceDisplayOn	==  CMD_WHITESPACEDISPLAYON );
	ASSERT( ( int ) cmCmdWhitespaceDisplayOff	==  CMD_WHITESPACEDISPLAYOFF );
	ASSERT( ( int ) cmCmdOvertypeOn				==  CMD_OVERTYPEON );
	ASSERT( ( int ) cmCmdOvertypeOff			==  CMD_OVERTYPEOFF );

	ASSERT( ( int ) cmIndentOff					==	CM_INDENT_OFF );
	ASSERT( ( int ) cmIndentScope				==	CM_INDENT_SCOPE );
	ASSERT( ( int ) cmIndentPrevLine			==	CM_INDENT_PREVLINE );

	ASSERT( ( int ) cmNowhere					==	CM_NOWHERE );
	ASSERT( ( int ) cmHSplitter					==	CM_HSPLITTER );
	ASSERT( ( int ) cmVSplitter					==	CM_VSPLITTER );
	ASSERT( ( int ) cmHVSplitter				==	CM_HVSPLITTER );
	ASSERT( ( int ) cmEditSpace					==	CM_EDITSPACE );
	ASSERT( ( int ) cmHScrollBar				==	CM_HSCROLLBAR );
	ASSERT( ( int ) cmVScrollBar				==	CM_VSCROLLBAR );
	ASSERT( ( int ) cmSizeBox					==	CM_SIZEBOX );
	ASSERT( ( int ) cmLeftMargin				==	CM_LEFTMARGIN );

	ASSERT( ( int ) cmErrFailure				==  CMDERR_FAILURE );
	ASSERT( ( int ) cmErrInput					==  CMDERR_INPUT );
	ASSERT( ( int ) cmErrSelection				==  CMDERR_SELECTION );
	ASSERT( ( int ) cmErrNotFound				==  CMDERR_NOTFOUND );
	ASSERT( ( int ) cmErrEmptyBuf				==  CMDERR_EMPTYBUF );
	ASSERT( ( int ) cmErrReadOnly				==  CMDERR_READONLY );

	ASSERT( ( int ) cmLangStyleProcedural		==  CMLS_PROCEDURAL );
	ASSERT( ( int ) cmLangStyleSGML				==  CMLS_SGML );

	ASSERT( ( int ) cmBorderNone				==  CM_BORDER_NONE );
	ASSERT( ( int ) cmBorderThin				==  CM_BORDER_THIN );
	ASSERT( ( int ) cmBorderClient				==  CM_BORDER_CLIENT );
	ASSERT( ( int ) cmBorderStatic				==  CM_BORDER_STATIC );
	ASSERT( ( int ) cmBorderModal				==  CM_BORDER_MODAL );
	ASSERT( ( int ) cmBorderCorral				==  CM_BORDER_CORRAL );



	ASSERT( DestroyWindow( hWndStatic ) );
	ASSERT( DeleteObject( hFont ) );
	m_hWndTest = NULL;
#endif
}

void CEdit::DoCursorSelfTest( int x, int y )
{
	SetCapture( m_hWnd );
	ASSERT( GetCapture() == m_hWnd );
	HDC hDC = GetDC( m_hWnd );
	RECT rc;
	GetClientRect( m_hWnd, &rc );
	SetBkMode( hDC, TRANSPARENT );
	rc.bottom = rc.top + m_pActiveView->GetLineHeight();
	FillRect( hDC, &rc, ( HBRUSH ) GetStockObject( BLACK_BRUSH ) );
	SetTextColor( hDC, RGB( 255, 255, 255 ) );
	TCHAR szHT[ 20 ];
	DWORD dwHitTest = CM_HitTest( m_hWnd, x, y );
	switch ( HIWORD( dwHitTest ) )
	{
		case CM_NOWHERE: _tcscpy( szHT, _T("CM_NOWHERE") ); break;
		case CM_HSPLITTER: _tcscpy( szHT, _T("CM_HSPLITTER") ); break;
		case CM_VSPLITTER: _tcscpy( szHT, _T("CM_VSPLITTER") ); break;
		case CM_HVSPLITTER: _tcscpy( szHT, _T("CM_HVSPLITTER") ); break;
		case CM_EDITSPACE: _tcscpy( szHT, _T("CM_EDITSPACE") ); break;
		case CM_HSCROLLBAR: _tcscpy( szHT, _T("CM_HSCROLLBAR") ); break;
		case CM_VSCROLLBAR: _tcscpy( szHT, _T("CM_VSCROLLBAR") ); break;
		case CM_SIZEBOX: _tcscpy( szHT, _T("CM_SIZEBOX") ); break;
		case CM_LEFTMARGIN: _tcscpy( szHT, _T("CM_LEFTMARGIN") ); break;
		default: _tcscpy( szHT, _T("ERROR!") ); break; 
	}

	int nView = LOWORD( dwHitTest );

	CM_POSITION p = { 0, 0 };
	TCHAR sz[ 2000 ];
	TCHAR szWord[ 200 ];
	*szWord = _T('\0');
	if ( CM_GetSelFromPoint( m_hWnd, x, y, &p ) == CME_SUCCESS )
	{
		ASSERT( CM_GetWord( m_hWnd, szWord, &p ) == CME_SUCCESS );

		RECT rc;
		ASSERT( CM_PosFromChar( m_hWnd, &p, &rc ) == CME_SUCCESS );
		SetROP2( hDC, R2_XORPEN );
		// erase last rect
		if ( !::IsRectEmpty( &m_rcLastPosTest ) )
			DrawFocusRect( hDC, &m_rcLastPosTest );
		// draw new rect
		m_rcLastPosTest = rc;
		DrawFocusRect( hDC, &m_rcLastPosTest );

	}
	wsprintf( sz, _T("view=%d, pos: {%d,%d}, word='%s', hittest=%s"), nView, p.nLine, p.nCol, szWord, szHT );
	TextOut( hDC, rc.left, rc.top, sz, _tcslen( sz ) );
	ReleaseDC( m_hWnd, hDC );
}
#endif