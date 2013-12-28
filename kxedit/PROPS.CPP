#include "precomp.h"
#include "cedit.h"
#ifdef _ACTIVEX
#include "editx.h"
#include "ceditx.h"
#endif
#include "hkeyctrl.h"
#include "resource.h"

extern HINSTANCE hInstance;

#define IDX_BOOKMARK		0
#define IDX_COMMENT			1
#define IDX_HDIVIDERLINES	2
#define IDX_HIGHLIGHTEDLINE	3
#define IDX_KEYWORD			4
#define IDX_LMARGIN			5
#define IDX_LINENUMBER		6
#define IDX_NUMBER			7
#define IDX_OPERATOR		8
#define IDX_SCOPEKEYWORD	9
#define IDX_STRING			10
#define IDX_TAGELEMNAME		11
#define IDX_TAGENT			12
#define IDX_TAGATTRNAME		13
#define IDX_TEXT			14
#define IDX_WINDOW			15
#define IDX_TAGTEXT			16
#define IDX_VDIVIDERLINES	17

#define IDX_ASPECT_COUNT	18

class CPropInfo {

	public:
	CPropInfo();
	~CPropInfo();

	CEdit *m_pCtrl;

	typedef enum { eColorFont, eKeyboard, eTabs, eMisc } eProps;

	HWND m_hWndKeyboardDlg;

	//////////////////
	// Misc Props
	//
	BOOL m_bSmoothScroll : 2;
	BOOL m_bShowLeftMargin : 2;
	BOOL m_bAllowDragDrop : 2;
	BOOL m_bLineToolTips : 2;
	BOOL m_bAllowColumnSel : 2;
	BOOL m_bColorSyntax : 2;
	BOOL m_bShowHScrollBar : 2;
	BOOL m_bShowVScrollBar : 2;
	BOOL m_bAllowHSplit : 2;
	BOOL m_bAllowVSplit : 2;
	BOOL m_bSelBounds : 2;
	int m_nMaxUndo;
	CM_LINENUMBERING m_cmLineNum;

	////////////////////////
	// Color/Font Props
	//
	int m_nPosToIndexMap[ IDX_ASPECT_COUNT ];
	COLORREF m_crFore[ IDX_ASPECT_COUNT ];
	COLORREF m_crBack[ IDX_ASPECT_COUNT ];
	BYTE m_byFontStyles[ IDX_ASPECT_COUNT ];
	LOGFONT m_lf;
	HFONT m_hFont;

	////////////////////////
	// Keyboard Props
	//
	CM_HOTKEY *m_pcmAddKey;
	WORD *m_pwAddCmd;
	int m_nAddCount;
	CM_HOTKEY *m_pcmRemoveKey;
	int m_nRemoveCount;
	void AddHotKey( CM_HOTKEY &cmHotKey, WORD wCmd );
	void RemoveHotKey( CM_HOTKEY &cmHotKey );
	void ResetHotKeys();
	BOOL ShouldEatCommand() const
		{ return m_hWndKeyboardDlg ? ( SendMessage( GetDlgItem( m_hWndKeyboardDlg, IDC_HOTKEY ), HOTM_QUERYEATCOMMAND, 0, 0 ) != 0 ) : FALSE; }

	////////////////////////
	// Tab Props
	//
	CEdit::IndentStyle m_eIndentStyle;
	int m_nTabSize;
	BOOL m_bExpandTabs;
	BOOL m_bNormalizeCase;
	TCHAR m_szLang[ CM_MAX_LANGUAGE_NAME + 1 ];

	void CommitChanges( eProps eWhat );
};

CPropInfo::CPropInfo()
{
	m_pcmAddKey = NULL;
	m_pwAddCmd = NULL;
	m_pcmRemoveKey = NULL;
	m_nAddCount = m_nRemoveCount = 0;
	m_hFont = NULL;
	m_hWndKeyboardDlg = NULL;
}

CPropInfo::~CPropInfo()
{
	if ( m_pcmAddKey )
	{
		free( m_pcmAddKey );
	}
	if ( m_pwAddCmd )
	{
		free( m_pwAddCmd );
	}
	if ( m_pcmRemoveKey )
	{
		free( m_pcmRemoveKey );
	}
	if ( m_hFont )
	{
		DeleteObject( m_hFont );
	}
}

void CPropInfo::AddHotKey( CM_HOTKEY &cmHotKey, WORD wCmd )
{
	BOOL bAppend = TRUE;

	CEdit::NormalizeHotKey( cmHotKey );

	// check to see if already have 'add' record for this hotkey
	// and modify it if so
	for ( int i = 0; i < m_nAddCount; i++ )
	{
		if ( m_pcmAddKey[ i ] == cmHotKey )
		{
			bAppend = FALSE;
			m_pwAddCmd[ i ] = wCmd;
			break;
		}
	}

	// check to see if have a 'remove' record for this hotkey
	// and remove if so
	for ( int i = 0; i < m_nRemoveCount; i++ )
	{
		if ( m_pcmRemoveKey[ i ] == cmHotKey )
		{
			m_pcmRemoveKey[ i ].nVirtKey1 = 0;  // don't actually remove the element -- just wipe it
			break;
		}
	}

	if ( bAppend )
	{
		m_pcmAddKey = m_pcmAddKey ? ( CM_HOTKEY * ) realloc( m_pcmAddKey, sizeof( CM_HOTKEY ) * ( m_nAddCount + 1 ) ) :
		                      ( CM_HOTKEY * ) malloc( sizeof( CM_HOTKEY ) );
		m_pwAddCmd = m_pwAddCmd ? ( WORD * ) realloc( m_pwAddCmd, sizeof( WORD ) * ( m_nAddCount + 1 ) ) :
		                      ( WORD * ) malloc( sizeof( WORD ) );
		m_pcmAddKey[ m_nAddCount ] = cmHotKey;
		m_pwAddCmd[ m_nAddCount++ ] = wCmd;
	}
}

void CPropInfo::RemoveHotKey( CM_HOTKEY &cmHotKey )
{
	BOOL bAppend = TRUE;
	CEdit::NormalizeHotKey( cmHotKey );

	// check to see if already have a 'remove' record for this hotkey
	// and ignore this one if so
	for ( int i = 0; i < m_nRemoveCount; i++ )
	{
		if ( m_pcmRemoveKey[ i ] == cmHotKey )
		{
			bAppend = FALSE;
			break;
		}
	}

	// check to see if have an 'add' record for this hotkey
	// and remove if so
	for ( int i = 0; i < m_nAddCount; i++ )
	{
		if ( m_pcmAddKey[ i ] == cmHotKey )
		{
			m_pcmAddKey[ i ].nVirtKey1 = 0;  // don't actually add the element -- just wipe it
			m_pwAddCmd[ i ] = 0;  // don't actually add the element -- just wipe it
			break;
		}
	}

	if ( bAppend )
	{
		m_pcmRemoveKey = m_pcmRemoveKey ? ( CM_HOTKEY * ) realloc( m_pcmRemoveKey, sizeof( CM_HOTKEY ) * ( m_nRemoveCount + 1 ) ) : 
		                      ( CM_HOTKEY * ) malloc( sizeof( CM_HOTKEY ) );
		m_pcmRemoveKey[ m_nRemoveCount++ ] = cmHotKey;
	}
}

void CPropInfo::ResetHotKeys()
{
	// remove all pending changes
	if ( m_pcmAddKey )
	{
		free( m_pcmAddKey );
		m_pcmAddKey = NULL;
	}
	if ( m_pwAddCmd )
	{
		free( m_pwAddCmd );
		m_pwAddCmd = NULL;
	}
	if ( m_pcmRemoveKey )
	{
		free( m_pcmRemoveKey );
		m_pcmRemoveKey = NULL;
	}
	m_nAddCount = m_nRemoveCount = 0;

	// Now, add 'remove' records for all existing hotkeys
	for ( int i = 0; i < CEdit::g_nHotKeyCount; i++ )
	{
		CHotKey *pHotKey = CEdit::g_pHotKeys + i;
		if ( !CEdit::IsRegisteredCommand( pHotKey->wCmd ) )
			RemoveHotKey( pHotKey->cmHotKey ); 
	}

	// Now, add 'add' records for all default hotkeys
	for ( int i = 0; i < CEdit::g_nDefHotKeyCount; i++ )
	{
		_defhotkeyrec *pHotKey = &CEdit::g_DefHotKeys[ i ];
		CM_HOTKEY cmHotKey = { pHotKey->fsModifiers1, pHotKey->nVirtKey1,
		                       pHotKey->fsModifiers2, pHotKey->nVirtKey2 };
		AddHotKey( cmHotKey, pHotKey->wCmd );
	}
}

void CPropInfo::CommitChanges( eProps eWhat )
{
	for ( int nWnd = 0; nWnd < CEdit::g_nhWndCount; nWnd++ )
	{
		HWND hWnd = m_pCtrl->m_bGlobalProps ? CEdit::g_phWnds[ nWnd ] : m_pCtrl->m_hWnd;
		ASSERT( IsWindow( hWnd ) );
		CEdit *pCtrl = ( CEdit * ) GetWindowLong( hWnd, 0 );
		ASSERT( pCtrl );

		pCtrl->Repaint( FALSE );

		if ( eWhat == eMisc )
		{
			if ( m_bSmoothScroll != pCtrl->m_bSmoothScroll )
			{
				SendMessage( hWnd, CMM_ENABLESMOOTHSCROLLING, m_bSmoothScroll, 0 );
			}
			if ( m_bShowLeftMargin != pCtrl->m_bShowLeftMargin )
			{
				SendMessage( hWnd, CMM_ENABLELEFTMARGIN, m_bShowLeftMargin, 0 );
			}
			if ( m_bAllowDragDrop != pCtrl->m_bAllowDragDrop )
			{
				SendMessage( hWnd, CMM_ENABLEDRAGDROP, m_bAllowDragDrop, 0 );
			}
			if ( m_bLineToolTips != pCtrl->m_bLineToolTips )
			{
				SendMessage( hWnd, CMM_ENABLELINETOOLTIPS, m_bLineToolTips, 0 );
			}
			if ( m_bAllowColumnSel != pCtrl->m_bAllowColumnSel )
			{
				SendMessage( hWnd, CMM_ENABLECOLUMNSEL, m_bAllowColumnSel, 0 );
			}
			if ( m_bColorSyntax != pCtrl->m_bColorSyntax )
			{
				SendMessage( hWnd, CMM_ENABLECOLORSYNTAX, m_bColorSyntax, 0 );
			}
			if ( m_bShowHScrollBar != pCtrl->ShowHScrollBar() )
			{
				SendMessage( hWnd, CMM_SHOWSCROLLBAR, TRUE, m_bShowHScrollBar );
			}
			if ( m_bShowVScrollBar != pCtrl->ShowVScrollBar() )
			{
				SendMessage( hWnd, CMM_SHOWSCROLLBAR, FALSE, m_bShowVScrollBar );
			}
			if ( m_bAllowHSplit != pCtrl->m_bAllowHSplit )
			{
				SendMessage( hWnd, CMM_ENABLESPLITTER, TRUE, m_bAllowHSplit );
			}
			if ( m_bAllowVSplit != pCtrl->m_bAllowVSplit )
			{
				SendMessage( hWnd, CMM_ENABLESPLITTER, FALSE, m_bAllowVSplit );
			}
			if ( m_bSelBounds != pCtrl->BoundSelection() )
			{
				SendMessage( hWnd, CMM_ENABLESELBOUNDS, m_bSelBounds, 0 );
			}
			if ( m_nMaxUndo != pCtrl->m_Buffer.GetMaxUndo() )
			{
				SendMessage( hWnd, CMM_SETUNDOLIMIT, m_nMaxUndo, 0 );
			}
			SendMessage( hWnd, CMM_SETLINENUMBERING, 0, ( LPARAM ) &m_cmLineNum );
		}
		else if ( eWhat == eColorFont )
		{
			CM_COLORS Colors;
			ZeroMemory( &Colors, sizeof( Colors ) );
			Colors.crWindow = m_crFore[ IDX_WINDOW ];
			Colors.crLeftMargin = m_crFore[ IDX_LMARGIN ];
			Colors.crBookmark = m_crFore[ IDX_BOOKMARK ];
			Colors.crBookmarkBk = m_crBack[ IDX_BOOKMARK ];
			Colors.crText = m_crFore[ IDX_TEXT ];
			Colors.crTextBk = m_crBack[ IDX_TEXT ];
			Colors.crNumber = m_crFore[ IDX_NUMBER ];
			Colors.crNumberBk = m_crBack[ IDX_NUMBER ];
			Colors.crKeyword = m_crFore[ IDX_KEYWORD ];
			Colors.crKeywordBk = m_crBack[ IDX_KEYWORD ];
			Colors.crOperator = m_crFore[ IDX_OPERATOR ];
			Colors.crOperatorBk = m_crBack[ IDX_OPERATOR ];
			Colors.crScopeKeyword = m_crFore[ IDX_SCOPEKEYWORD ];
			Colors.crScopeKeywordBk = m_crBack[ IDX_SCOPEKEYWORD ];
			Colors.crComment = m_crFore[ IDX_COMMENT ];
			Colors.crCommentBk = m_crBack[ IDX_COMMENT ];
			Colors.crString = m_crFore[ IDX_STRING ];
			Colors.crStringBk = m_crBack[ IDX_STRING ];
			Colors.crTagText = m_crFore[ IDX_TAGTEXT ];
			Colors.crTagTextBk = m_crBack[ IDX_TAGTEXT ];
			Colors.crTagEntity = m_crFore[ IDX_TAGENT ];
			Colors.crTagEntityBk = m_crBack[ IDX_TAGENT ];
			Colors.crTagElementName = m_crFore[ IDX_TAGELEMNAME ];
			Colors.crTagElementNameBk = m_crBack[ IDX_TAGELEMNAME ];
			Colors.crTagAttributeName = m_crFore[ IDX_TAGATTRNAME ];
			Colors.crTagAttributeNameBk = m_crBack[ IDX_TAGATTRNAME ];
			Colors.crLineNumber = m_crFore[ IDX_LINENUMBER ];
			Colors.crLineNumberBk = m_crBack[ IDX_LINENUMBER ];
			Colors.crHDividerLines = m_crFore[ IDX_HDIVIDERLINES ];
			Colors.crVDividerLines = m_crFore[ IDX_VDIVIDERLINES ];
			Colors.crHighlightedLine = m_crFore[ IDX_HIGHLIGHTEDLINE ];
			SendMessage( hWnd, CMM_SETCOLORS, 0, ( LPARAM ) &Colors );

			SendMessage( hWnd, WM_SETFONT, ( WPARAM ) CreateFontIndirect( &m_lf ), MAKELPARAM( TRUE, 0 ) );
			pCtrl->m_bOwnFont = TRUE;

			CM_FONTSTYLES FontStyles;
			ZeroMemory( &FontStyles, sizeof( FontStyles ) );
			FontStyles.byText = m_byFontStyles[ IDX_TEXT ];
			FontStyles.byNumber = m_byFontStyles[ IDX_NUMBER ];
			FontStyles.byKeyword = m_byFontStyles[ IDX_KEYWORD ];
			FontStyles.byOperator = m_byFontStyles[ IDX_OPERATOR ];
			FontStyles.byScopeKeyword = m_byFontStyles[ IDX_SCOPEKEYWORD ];
			FontStyles.byComment = m_byFontStyles[ IDX_COMMENT ];
			FontStyles.byString = m_byFontStyles[ IDX_STRING ];
			FontStyles.byTagText = m_byFontStyles[ IDX_TAGTEXT ];
			FontStyles.byTagEntity = m_byFontStyles[ IDX_TAGENT ];
			FontStyles.byTagElementName = m_byFontStyles[ IDX_TAGELEMNAME ];
			FontStyles.byTagAttributeName = m_byFontStyles[ IDX_TAGATTRNAME ];
			FontStyles.byLineNumber = m_byFontStyles[ IDX_LINENUMBER ];

			SendMessage( hWnd, CMM_SETFONTSTYLES, 0, ( LPARAM ) &FontStyles );
		}
		else if ( eWhat == eKeyboard )
		{
			if ( m_pcmAddKey )
			{
				for ( int i = 0; i < m_nAddCount; i++ )
				{
					CM_HOTKEY cmHotKey = m_pcmAddKey[ i ];
					if ( cmHotKey.nVirtKey1 )
					{
						CEdit::RegisterHotKey( cmHotKey, m_pwAddCmd[ i ] );
					}
				}
				free( m_pcmAddKey );
				free( m_pwAddCmd );
				m_pcmAddKey = NULL;
				m_pwAddCmd = NULL;
				m_nAddCount = 0;
			}

			if ( m_pcmRemoveKey )
			{
				for ( int i = 0; i < m_nRemoveCount; i++ )
				{
					CM_HOTKEY cmHotKey = m_pcmRemoveKey[ i ];
					if ( cmHotKey.nVirtKey1 )
					{
						CEdit::UnregisterHotKey( cmHotKey );
					}
				}
				free( m_pcmRemoveKey );
				m_pcmRemoveKey = NULL;
				m_nRemoveCount = 0;
			}
		}
		else if ( eWhat == eTabs )
		{
			if ( m_eIndentStyle != pCtrl->m_eIndentStyle )
			{
				SendMessage( hWnd, CMM_SETAUTOINDENTMODE, m_eIndentStyle, 0 );
			}

			if ( m_nTabSize != pCtrl->m_Buffer.GetTabSize() )
			{
				SendMessage( hWnd, CMM_SETTABSIZE, m_nTabSize, 0 );
			}

			if ( m_bExpandTabs != pCtrl->m_bExpandTabs )
			{
				SendMessage( hWnd, CMM_ENABLETABEXPAND, m_bExpandTabs, 0 );
			}

			if ( m_bNormalizeCase != pCtrl->m_Buffer.NormalizeCase() )
			{
				SendMessage( hWnd, CMM_ENABLENORMALIZECASE, m_bNormalizeCase, 0 );
			}

			// language setting only applies to the current window
			if ( ( pCtrl == m_pCtrl ) && ( _tcsicmp( m_szLang, pCtrl->m_szLang ) != 0 ) )
			{
				SendMessage( hWnd, CMM_SETLANGUAGE, 0, ( LPARAM ) m_szLang );
			}
		}

		// tell the parent that props changed
		pCtrl->NotifyParent( CMN_PROPSCHANGE );

		if ( !m_pCtrl->m_bGlobalProps )
		{
			break;
		}
	}
	#ifdef _ACTIVEX
	CEditX::ResetAccelTable();
	#endif
}

void OnPropChanged( HWND hWndPropPage )
{
	SendMessage( GetParent( hWndPropPage ), PSM_CHANGED, ( WPARAM ) hWndPropPage, 0 );
}

#define BIT_NODEFAULT			0x1
#define BIT_NOBKCOLOR			0x2
#define BIT_NOFONTSTYLE			0x4

static int g_nLastPage = 0;

#define UM_UPDATE_FONT			WM_USER + 100
#define UM_ITEM_CHANGED			WM_USER + 101

// the following two vars are thread-safe because 
// there can only be one UI thread per process
HHOOK g_hhook = NULL;
HWND g_hWndHKeyCtrl = NULL;

// HookProc intercepts Ctrl+PgUp, Ctrl+PgDown, and Alt+Tab keystrokes from the
// Keyboard properties tab and forwards them to the hotkey control if the hotkey
// control has focus.  If this didn't happen, the user would not be able to 
// assign those hotkeys via the UI.
LRESULT CALLBACK HookProc( int code, WPARAM wParam, LPARAM lParam )
{
	if ( code >= 0 )
	{
		MSG *pMSG = ( MSG * ) lParam;
		switch ( pMSG->message )
		{
			case WM_KEYDOWN:
			case WM_KEYUP:
			{
				BOOL bControl = ( GetKeyState( VK_CONTROL ) < 0 );
				if ( GetFocus() == g_hWndHKeyCtrl )
				{
					BOOL bReroute = FALSE;
					switch ( pMSG->wParam )
					{
						case VK_PRIOR:
						case VK_NEXT:
						case VK_TAB:
							{
							bReroute = bControl;
							break;
							}
					}
					if ( bReroute )
					{
						SendMessage( g_hWndHKeyCtrl, pMSG->message, pMSG->wParam, pMSG->lParam );
						pMSG->message = WM_NULL;
						return 0;					
					}
				}
				break;
			}
		}
	}

	return CallNextHookEx( g_hhook, code, wParam, lParam );
}

BOOL CALLBACK PageColorFontDlgProc( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	static BOOL bInitialized = FALSE;
	
	if ( !bInitialized && uMsg != WM_INITDIALOG )
		return FALSE;

	DWORD dwData = GetWindowLong( hWndDlg, GWL_USERDATA );
	CPropInfo *pInfo = dwData ? ( ( CPropInfo * ) ( ( PROPSHEETPAGE * ) dwData )->lParam ) : NULL;

	switch ( uMsg )
	{
		case WM_INITDIALOG:
		{
			SetWindowLong( hWndDlg, GWL_USERDATA, lParam );
			bInitialized = TRUE;
			CPropInfo *pInfo = ( ( CPropInfo * ) ( ( PROPSHEETPAGE * ) lParam )->lParam );
			// restore propdialog position
			HWND hWndParent = GetParent( hWndDlg );
			if ( !IsWindowVisible( hWndParent ) )
				pInfo->m_pCtrl->RestoreDlgPos( hWndParent, IDD_WINPROPS );

			HWND hWndList = GetDlgItem( hWndDlg, IDC_COLOR_ITEM );

			// fill the Items list
			struct {
				UINT unText;
				int nIdx;
				DWORD dwFlags;
			} Items[] =
			  {
				{ IDS_PROP_BOOKMARK, IDX_BOOKMARK, BIT_NOFONTSTYLE },
				{ IDS_PROP_COMMENT, IDX_COMMENT, BIT_NODEFAULT },
				{ IDS_PROP_HDIVIDERLINES, IDX_HDIVIDERLINES, BIT_NOBKCOLOR | BIT_NOFONTSTYLE },
				{ IDS_PROP_VDIVIDERLINES, IDX_VDIVIDERLINES, BIT_NOBKCOLOR | BIT_NOFONTSTYLE },
				{ IDS_PROP_HIGHLIGHTEDLINE, IDX_HIGHLIGHTEDLINE, BIT_NODEFAULT | BIT_NOBKCOLOR | BIT_NOFONTSTYLE },
				{ IDS_PROP_KEYWORD, IDX_KEYWORD, BIT_NODEFAULT },
				{ IDS_PROP_LMARGIN, IDX_LMARGIN, BIT_NOBKCOLOR | BIT_NOFONTSTYLE },
				{ IDS_PROP_LINENUMBER, IDX_LINENUMBER, BIT_NODEFAULT },
				{ IDS_PROP_NUMBER, IDX_NUMBER, BIT_NODEFAULT },
				{ IDS_PROP_OPERATOR, IDX_OPERATOR, BIT_NODEFAULT },
				{ IDS_PROP_SCOPEKEYWORD, IDX_SCOPEKEYWORD, BIT_NODEFAULT },
				{ IDS_PROP_STRING, IDX_STRING, BIT_NODEFAULT },
				{ IDS_PROP_TAGELEMNAME, IDX_TAGELEMNAME, BIT_NODEFAULT },
				{ IDS_PROP_TAGENT, IDX_TAGENT, BIT_NODEFAULT },
				{ IDS_PROP_TAGATTRNAME, IDX_TAGATTRNAME, BIT_NODEFAULT },
				{ IDS_PROP_TAGTEXT, IDX_TAGTEXT, BIT_NODEFAULT },
				{ IDS_PROP_TEXT, IDX_TEXT, BIT_NODEFAULT },
				{ IDS_PROP_WINDOW, IDX_WINDOW, BIT_NOBKCOLOR | BIT_NOFONTSTYLE } };

			ASSERT( ARRAY_SIZE( Items ) == IDX_ASPECT_COUNT );	// every ui aspect should be configurable

			for ( int i = 0; i < ARRAY_SIZE( Items ); i++ )
			{
				int nIndex = SendMessage( hWndList, LB_ADDSTRING, 0, ( LPARAM ) LoadStringPtr( Items[ i ].unText ) );
				pInfo->m_nPosToIndexMap[ nIndex ] = Items[ i ].nIdx;	// need to update the mapping from combo item to arrays in pInfo
				VERIFY( SendMessage( hWndList, LB_SETITEMDATA, nIndex, Items[ i ].dwFlags ) != LB_ERR );
			}

			SendMessage( hWndList, LB_SETCURSEL, 0, 0 );

			// fill the color combos
			COLORREF Colors[] = {
				RGB( 0, 0, 0 ),				// Black
				RGB( 128, 0, 0 ),			// Maroon
				RGB( 0, 128, 0 ),			// Green
				RGB( 128, 128, 0 ),			// Olive
				RGB( 0, 0, 128 ),			// Navy
				RGB( 128, 0, 128 ),			// Purple
				RGB( 0, 128, 128 ),			// Teal
				RGB( 128, 128, 128 ),		// Gray
				RGB( 192, 192, 192 ),		// Silver
				RGB( 255, 0, 0 ),			// Red
				RGB( 0, 255, 0 ),			// Lime
				RGB( 255, 255, 0 ),			// Yellow
				RGB( 0, 0, 255 ),			// Blue
				RGB( 255, 0, 255 ),			// Fuschia
				RGB( 0, 255, 255 ),			// Aqua
				RGB( 255, 255, 255 ) };		// White

			
			HWND hWndBack = GetDlgItem( hWndDlg, IDC_BACK_COMBO );
			HWND hWndFore = GetDlgItem( hWndDlg, IDC_FORE_COMBO );
			SendMessage( hWndBack, CB_ADDSTRING, 0, CLR_INVALID );
			for ( int i = 0; i < ARRAY_SIZE( Colors ); i++ )
			{
				SendMessage( hWndBack, CB_ADDSTRING, 0, Colors[ i ] );
				SendMessage( hWndFore, CB_ADDSTRING, 0, Colors[ i ] );
			}

			CEdit *pCtrl = pInfo->m_pCtrl;
			pInfo->m_crFore[ IDX_WINDOW ] = pCtrl->m_Colors.crWindow;
			pInfo->m_crFore[ IDX_LMARGIN ] = pCtrl->m_Colors.crLeftMargin;
			pInfo->m_crFore[ IDX_BOOKMARK ] = pCtrl->m_Colors.crBookmark;
			pInfo->m_crBack[ IDX_BOOKMARK ] = pCtrl->m_Colors.crBookmarkBk;
			pInfo->m_crFore[ IDX_TEXT ] = pCtrl->m_Colors.crText;
			pInfo->m_crBack[ IDX_TEXT ] = pCtrl->m_Colors.crTextBk;
			pInfo->m_crFore[ IDX_NUMBER ] = pCtrl->m_Colors.crNumber;
			pInfo->m_crBack[ IDX_NUMBER ] = pCtrl->m_Colors.crNumberBk;
			pInfo->m_crFore[ IDX_KEYWORD ] = pCtrl->m_Colors.crKeyword;
			pInfo->m_crBack[ IDX_KEYWORD ] = pCtrl->m_Colors.crKeywordBk;
			pInfo->m_crFore[ IDX_OPERATOR ] = pCtrl->m_Colors.crOperator;
			pInfo->m_crBack[ IDX_OPERATOR ] = pCtrl->m_Colors.crOperatorBk;
			pInfo->m_crFore[ IDX_SCOPEKEYWORD ] = pCtrl->m_Colors.crScopeKeyword;
			pInfo->m_crBack[ IDX_SCOPEKEYWORD ] = pCtrl->m_Colors.crScopeKeywordBk;
			pInfo->m_crFore[ IDX_COMMENT ] = pCtrl->m_Colors.crComment;
			pInfo->m_crBack[ IDX_COMMENT ] = pCtrl->m_Colors.crCommentBk;
			pInfo->m_crFore[ IDX_STRING ] = pCtrl->m_Colors.crString;
			pInfo->m_crBack[ IDX_STRING ] = pCtrl->m_Colors.crStringBk;
			pInfo->m_crFore[ IDX_TAGTEXT ] = pCtrl->m_Colors.crTagText;
			pInfo->m_crBack[ IDX_TAGTEXT ] = pCtrl->m_Colors.crTagTextBk;
			pInfo->m_crFore[ IDX_TAGENT ] = pCtrl->m_Colors.crTagEntity;
			pInfo->m_crBack[ IDX_TAGENT ] = pCtrl->m_Colors.crTagEntityBk;
			pInfo->m_crFore[ IDX_TAGELEMNAME ] = pCtrl->m_Colors.crTagElementName;
			pInfo->m_crBack[ IDX_TAGELEMNAME ] = pCtrl->m_Colors.crTagElementNameBk;
			pInfo->m_crFore[ IDX_TAGATTRNAME ] = pCtrl->m_Colors.crTagAttributeName;
			pInfo->m_crBack[ IDX_TAGATTRNAME ] = pCtrl->m_Colors.crTagAttributeNameBk;
			pInfo->m_crFore[ IDX_LINENUMBER ] = pCtrl->m_Colors.crLineNumber;
			pInfo->m_crBack[ IDX_LINENUMBER ] = pCtrl->m_Colors.crLineNumberBk;
			pInfo->m_crFore[ IDX_HDIVIDERLINES ] = pCtrl->m_Colors.crHDividerLines;
			pInfo->m_crFore[ IDX_VDIVIDERLINES ] = pCtrl->m_Colors.crVDividerLines;
			pInfo->m_crFore[ IDX_HIGHLIGHTEDLINE ] = pCtrl->m_Colors.crHighlightedLine;

			// fill the font style combo
			struct {
				UINT unName;
				BYTE byVal;
			} FontStyles[] =
			{
				IDS_FONT_NORMAL,			CM_FONT_NORMAL,
				IDS_FONT_BOLD,				CM_FONT_BOLD,
				IDS_FONT_ITALIC,			CM_FONT_ITALIC,
				IDS_FONT_BOLDITALIC,		CM_FONT_BOLDITALIC,
				IDS_FONT_UNDERLINE,			CM_FONT_UNDERLINE
			};

			
			HWND hWndFontStyle = GetDlgItem( hWndDlg, IDC_FONT_STYLE );
			for ( int i = 0; i < ARRAY_SIZE( FontStyles ); i++ )
			{
				int nItem = SendMessage( hWndFontStyle, CB_ADDSTRING, 0, ( LPARAM ) LoadStringPtr( FontStyles[ i ].unName ) );
				SendMessage( hWndFontStyle, CB_SETITEMDATA, nItem, FontStyles[ i ].byVal );
			}

			pInfo->m_byFontStyles[ IDX_TEXT ] = pCtrl->m_FontStyles.byText;
			pInfo->m_byFontStyles[ IDX_NUMBER ] = pCtrl->m_FontStyles.byNumber;
			pInfo->m_byFontStyles[ IDX_KEYWORD ] = pCtrl->m_FontStyles.byKeyword;
			pInfo->m_byFontStyles[ IDX_OPERATOR ] = pCtrl->m_FontStyles.byOperator;
			pInfo->m_byFontStyles[ IDX_SCOPEKEYWORD ] = pCtrl->m_FontStyles.byScopeKeyword;
			pInfo->m_byFontStyles[ IDX_COMMENT ] = pCtrl->m_FontStyles.byComment;
			pInfo->m_byFontStyles[ IDX_STRING ] = pCtrl->m_FontStyles.byString;
			pInfo->m_byFontStyles[ IDX_TAGTEXT ] = pCtrl->m_FontStyles.byTagText;
			pInfo->m_byFontStyles[ IDX_TAGENT ] = pCtrl->m_FontStyles.byTagEntity;
			pInfo->m_byFontStyles[ IDX_TAGELEMNAME ] = pCtrl->m_FontStyles.byTagElementName;
			pInfo->m_byFontStyles[ IDX_TAGATTRNAME ] = pCtrl->m_FontStyles.byTagAttributeName;
			pInfo->m_byFontStyles[ IDX_LINENUMBER ] = pCtrl->m_FontStyles.byLineNumber;

			int nSize = GetObject( pCtrl->GetFont( FALSE ), sizeof( LOGFONT ), NULL );
			VERIFY( GetObject( pCtrl->GetFont( FALSE ), nSize, &pInfo->m_lf ) );
			pInfo->m_hFont = CreateFontIndirect( &pInfo->m_lf );
			SendMessage( hWndDlg, UM_UPDATE_FONT, 0, 0 );
			SendMessage( hWndDlg, UM_ITEM_CHANGED, 0, 0 );

			return TRUE;
		}

		case WM_DESTROY:
		{
			// save dlg pos for next time
			pInfo->m_pCtrl->SaveDlgPos( GetParent( hWndDlg ), IDD_WINPROPS );

			// reset for next time
			bInitialized = FALSE;
			return FALSE;
		}

		case WM_LBUTTONDBLCLK:
		{
			// a back-door to the about box for tech support issues
			CEdit *pCtrl = pInfo->m_pCtrl;
			HWND hWndSave = pCtrl->m_hWndDlgParent;
			pCtrl->m_hWndDlgParent = hWndDlg;
			pInfo->m_pCtrl->AboutBox();
			pCtrl->m_hWndDlgParent = hWndSave;
			return FALSE;
		}

		case UM_UPDATE_FONT:
		{
			SendMessage( GetDlgItem( hWndDlg, IDC_PREVIEW ), WM_SETFONT, ( WPARAM )pInfo->m_hFont, MAKELPARAM( TRUE, 0 ) );
			break;
		}

		case UM_ITEM_CHANGED:
		{
			HWND hWndList = GetDlgItem( hWndDlg, IDC_COLOR_ITEM );
			int nCurSel = SendMessage( hWndList, LB_GETCURSEL, 0, 0 );
			ASSERT( nCurSel != LB_ERR );
			int nAspect = pInfo->m_nPosToIndexMap[ nCurSel ];
			DWORD dwData = SendMessage( hWndList, LB_GETITEMDATA, nCurSel, 0 );

			HWND hWndBack = GetDlgItem( hWndDlg, IDC_BACK_COMBO );
			HWND hWndFore = GetDlgItem( hWndDlg, IDC_FORE_COMBO );
			HWND hWndFontStyle = GetDlgItem( hWndDlg, IDC_FONT_STYLE );

			// Add/Remove 'Default' item to color combos if necessary
			if ( HAS_FLAG( dwData, BIT_NODEFAULT ) )
			{
				if ( SendMessage( hWndFore, CB_GETITEMDATA, 0, 0 ) == CLR_INVALID )
				{
					SendMessage( hWndFore, CB_DELETESTRING, 0, 0 );
				}
			}
			else
			{
				if ( SendMessage( hWndFore, CB_GETITEMDATA, 0, 0 ) != CLR_INVALID )
				{
					SendMessage( hWndFore, CB_INSERTSTRING, 0, CLR_INVALID );
				}
			}

			// show/hide the background color combo as needed
			BOOL bHasBKColor = !HAS_FLAG( dwData, BIT_NOBKCOLOR );
			int nCmdShow =  bHasBKColor ? SW_SHOW : SW_HIDE;
			ShowWindow( hWndBack, nCmdShow );
			ShowWindow( GetDlgItem( hWndDlg, IDC_BACK_LABEL ), nCmdShow );

			// pick the back color
			if ( bHasBKColor )
			{
				int nCBCount = SendMessage( hWndBack, CB_GETCOUNT, 0, 0 );
				for ( int i = 0; i < nCBCount; i++ )
				{
					if ( ( COLORREF ) SendMessage( hWndBack, CB_GETITEMDATA, i, 0 ) == pInfo->m_crBack[ nAspect ] )
					{
						SendMessage( hWndBack, CB_SETCURSEL, i, 0 );
						break;
					}
				}
			}

			// pick the fore color
			int nCBCount = SendMessage( hWndFore, CB_GETCOUNT, 0, 0 );
			for ( int i = 0; i < nCBCount; i++ )
			{
				if ( ( COLORREF ) SendMessage( hWndFore, CB_GETITEMDATA, i, 0 ) == pInfo->m_crFore[ nAspect ] )
				{
					SendMessage( hWndFore, CB_SETCURSEL, i, 0 );
					break;
				}
			}
			ASSERT( i < nCBCount );

			// show/hide the font style combo as needed
			BOOL bHasStyle = !HAS_FLAG( dwData, BIT_NOFONTSTYLE );
			nCmdShow =  ( bHasStyle && !DBCS_ACTIVE ) ? SW_SHOW : SW_HIDE;
			ShowWindow( hWndFontStyle, nCmdShow );
			ShowWindow( GetDlgItem( hWndDlg, IDC_FONT_STYLE_LABEL ), nCmdShow );

			// pick the font style
			if ( bHasStyle )
			{
				int nCBCount = SendMessage( hWndFontStyle, CB_GETCOUNT, 0, 0 );
				for ( int i = 0; i < nCBCount; i++ )
				{
					if ( ( BYTE ) SendMessage( hWndFontStyle, CB_GETITEMDATA, i, 0 ) == pInfo->m_byFontStyles[ nAspect ] )
					{
						SendMessage( hWndFontStyle, CB_SETCURSEL, i, 0 );
						break;
					}
				}
				ASSERT( i < nCBCount );
			}

			return TRUE;
		}

		case WM_DRAWITEM:
		{
			DRAWITEMSTRUCT *pds = ( DRAWITEMSTRUCT * ) lParam;

			HDC hDC = pds->hDC;

			RECT rc = pds->rcItem;
			HBRUSH hBrush = CreateSolidBrush( GetSysColor( COLOR_WINDOW ) );
			FillRect( hDC, &rc, hBrush );
			DeleteObject( hBrush );

			RECT rcColor = rc;
			InflateRect( &rcColor, -4, -3 );
			COLORREF crItem = ( COLORREF )SendMessage( pds->hwndItem, CB_GETITEMDATA, pds->itemID, 0 );
			BOOL bDefault = ( crItem == CLR_INVALID );
			if ( bDefault )
			{
				crItem = GetSysColor( COLOR_WINDOW );
			}

			hBrush = CreateSolidBrush( crItem );
			HBRUSH hbrOld = ( HBRUSH ) SelectObject( hDC, hBrush );
			HPEN hPen = ( HPEN ) CreatePen( PS_SOLID, 1, GetSysColor( COLOR_WINDOWFRAME ) );
			HBRUSH hpenOld = ( HBRUSH ) SelectObject( hDC, hPen );
			Rectangle( hDC, rcColor.left, rcColor.top, rcColor.right, rcColor.bottom );
			SelectObject( hDC, hpenOld );
			DeleteObject( hPen );
			SelectObject( hDC, hbrOld );
			DeleteObject( hBrush );

			if ( bDefault )
			{						
				SetTextColor( hDC, GetSysColor( COLOR_WINDOWTEXT ) );
				SetBkMode( hDC, TRANSPARENT );
				DrawText( hDC, LoadStringPtr( IDS_DEFAULT ), -1, &rc, DT_VCENTER | DT_CENTER | DT_SINGLELINE );
			}

			if ( HAS_FLAG( pds->itemState, ODS_FOCUS ) )
			{
				DrawFocusRect( hDC, &rc );
			}
			return TRUE;
		}

		case WM_COMMAND:
		{
			WORD wID = LOWORD( wParam );  
			WORD wCmd = HIWORD( wParam );

			switch ( wID )
			{
				case IDC_COLOR_ITEM:
				{
					if ( wCmd == LBN_SELCHANGE )
					{
						SendMessage( hWndDlg, UM_ITEM_CHANGED, 0, 0 );
						return FALSE;
					}
					break;
				}

				case IDC_FORE_COMBO:
				case IDC_BACK_COMBO:
				{
					if ( wCmd == CBN_SELCHANGE )
					{
						BOOL bFore = ( wID == IDC_FORE_COMBO );

						HWND hWndList = GetDlgItem( hWndDlg, IDC_COLOR_ITEM );
						int nCurItem = SendMessage( hWndList, LB_GETCURSEL, 0, 0 );
						ASSERT( nCurItem != LB_ERR );
						int nAspect = pInfo->m_nPosToIndexMap[ nCurItem ];

						HWND hWndCombo = GetDlgItem( hWndDlg, bFore ? IDC_FORE_COMBO : IDC_BACK_COMBO );
						int nCurSel = SendMessage( hWndCombo, CB_GETCURSEL, 0, 0 );
						ASSERT( nCurSel != CB_ERR );
						COLORREF cr = SendMessage( hWndCombo, CB_GETITEMDATA, nCurSel, 0 );
						if ( bFore )
						{
							pInfo->m_crFore[ nAspect ] = cr;
						}
						else
						{
							pInfo->m_crBack[ nAspect ] = cr;
						}

						OnPropChanged( hWndDlg );
						return FALSE;
					}
					break;
				}

				case IDC_FONT_STYLE:
				{
					if ( wCmd == CBN_SELCHANGE )
					{
						HWND hWndList = GetDlgItem( hWndDlg, IDC_COLOR_ITEM );
						int nCurItem = SendMessage( hWndList, LB_GETCURSEL, 0, 0 );
						ASSERT( nCurItem != LB_ERR );

						HWND hWndCombo = GetDlgItem( hWndDlg, IDC_FONT_STYLE );
						int nCurSel = SendMessage( hWndCombo, CB_GETCURSEL, 0, 0 );
						ASSERT( nCurSel != CB_ERR );
						BYTE by = ( BYTE ) SendMessage( hWndCombo, CB_GETITEMDATA, nCurSel, 0 );
						pInfo->m_byFontStyles[ pInfo->m_nPosToIndexMap[ nCurItem ] ] = by;

						OnPropChanged( hWndDlg );
						return FALSE;
					}
					break;
				}

				case IDC_FONT:
				{
					if ( wCmd == BN_CLICKED )
					{
						CHOOSEFONT cf = { sizeof( CHOOSEFONT ), GetParent( hWndDlg ), NULL, &pInfo->m_lf,
						                  0,
										  CF_INITTOLOGFONTSTRUCT | CF_FIXEDPITCHONLY | CF_SCREENFONTS | CF_NOVERTFONTS | CF_SELECTSCRIPT,
										  0, 0, 0, 0, 0, 0, 0, 0, 0 };

						if ( ChooseFont( &cf ) )
						{
							// delete the old font
							DeleteObject( pInfo->m_hFont );
							// create the new font
							NormalizeLOGFONT( pInfo->m_lf );
							pInfo->m_hFont = CreateFontIndirect( &pInfo->m_lf );
							SendMessage( hWndDlg, UM_UPDATE_FONT, 0, 0 );
							OnPropChanged( hWndDlg );
						}
						return FALSE;
					}
					break;
				}
			}
			return TRUE;
		}

		case WM_NOTIFY:
		{
			switch ( ( ( NMHDR FAR * ) lParam )->code ) 
			{
				case PSN_SETACTIVE:
				{
					SetWindowLong( hWndDlg, DWL_MSGRESULT, PSNRET_NOERROR );
					g_nLastPage = 0;
					break;
				}
				case PSN_APPLY:
				{
					if ( pInfo->ShouldEatCommand() )
					{
						OnPropChanged( hWndDlg );
					}
					else
					{
						pInfo->CommitChanges( CPropInfo::eColorFont );
					}
					SetWindowLong( hWndDlg, DWL_MSGRESULT, PSNRET_NOERROR );
					break;
				}
			}
		}
	}

	return FALSE;
}

#define UM_UPDATERADIOS WM_USER + 100

BOOL CALLBACK PageTabsDlgProc( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	static BOOL bInitialized = FALSE;
	
	if ( !bInitialized && uMsg != WM_INITDIALOG )
		return FALSE;

	DWORD dwData = GetWindowLong( hWndDlg, GWL_USERDATA );
	CPropInfo *pInfo = dwData ? ( ( CPropInfo * ) ( ( PROPSHEETPAGE * ) dwData )->lParam ) : NULL;

	switch ( uMsg )
	{
		case WM_INITDIALOG:
		{
			SetWindowLong( hWndDlg, GWL_USERDATA, lParam );
			bInitialized = TRUE;
			CPropInfo *pInfo = ( ( CPropInfo * ) ( ( PROPSHEETPAGE * ) lParam )->lParam );
			CEdit *pCtrl = pInfo->m_pCtrl;
			// restore propdialog position
			HWND hWndParent = GetParent( hWndDlg );
			if ( !IsWindowVisible( hWndParent ) )
				pInfo->m_pCtrl->RestoreDlgPos( hWndParent, IDD_WINPROPS );
			CheckDlgButton( hWndDlg, IDC_OFF, pCtrl->m_eIndentStyle == CEdit::eIndentOff );
			CheckDlgButton( hWndDlg, IDC_SCOPE, pCtrl->m_eIndentStyle == CEdit::eIndentScope );
			CheckDlgButton( hWndDlg, IDC_COPY_PREV, pCtrl->m_eIndentStyle == CEdit::eIndentPrev );
			SetDlgItemInt( hWndDlg, IDC_TAB_SIZE, pCtrl->m_Buffer.GetTabSize(), FALSE );
			CheckDlgButton( hWndDlg, IDC_EXPAND_TABS, pCtrl->m_bExpandTabs );
			CheckDlgButton( hWndDlg, IDC_NORM_CASE, pCtrl->m_Buffer.NormalizeCase() );

			HWND hWndLang = GetDlgItem( hWndDlg, IDC_LANGUAGE );
			SendMessage( hWndLang, CB_ADDSTRING, 0, ( LPARAM ) LoadStringPtr( IDS_NONE ) );
			for ( int i = 0; i < CEdit::g_nLanguageCount; i++ )
			{
				SendMessage( hWndLang, CB_ADDSTRING, 0, ( LPARAM ) CEdit::g_pLanguages[ i ]->m_szName );
			}

			int nSel = 0;
			if ( *pCtrl->m_szLang )
			{
				nSel = SendMessage( hWndLang, CB_FINDSTRINGEXACT, 1, ( LPARAM ) pCtrl->m_szLang );
				ASSERT( nSel != CB_ERR );
			}
			SendMessage( hWndLang, CB_SETCURSEL, nSel, 0 );
			SendMessage( GetDlgItem( hWndDlg, IDC_TAB_SIZE ), EM_LIMITTEXT, 4, 0 );
			SendMessage( hWndDlg, UM_UPDATERADIOS, 0, 0 );

			return TRUE;
		}

		case WM_DESTROY:
		{
			// save dlg pos for next time
			pInfo->m_pCtrl->SaveDlgPos( GetParent( hWndDlg ), IDD_WINPROPS );

			// reset for next time
			bInitialized = FALSE;
			return FALSE;
		}

		case UM_UPDATERADIOS:
		{
			// if no language selected, then disallow AutoIndent based on language scoping
			BOOL bNoLang = SendMessage( GetDlgItem( hWndDlg, IDC_LANGUAGE ), CB_GETCURSEL, 0, 0 ) == 0;
			if ( bNoLang &&
			     IsDlgButtonChecked( hWndDlg, IDC_SCOPE ) )
			{
				CheckDlgButton( hWndDlg, IDC_SCOPE, FALSE );
				CheckDlgButton( hWndDlg, IDC_COPY_PREV, TRUE );
			}

			EnableWindow( GetDlgItem( hWndDlg, IDC_SCOPE ), !bNoLang );
			return FALSE;
		}

		case WM_COMMAND:
		{
			WORD wID = LOWORD( wParam );  
			WORD wCmd = HIWORD( wParam );  

			switch ( wID )
			{
				case IDC_LANGUAGE:
				{
					if ( wCmd == CBN_SELCHANGE )
					{
						OnPropChanged( hWndDlg );
						SendMessage( hWndDlg, UM_UPDATERADIOS, 0, 0 );
						return FALSE;
					}
					break;
				}
				case IDC_OFF:
				case IDC_SCOPE:
				case IDC_COPY_PREV:
				case IDC_EXPAND_TABS:
				case IDC_NORM_CASE:
				{
					if ( wCmd == BN_CLICKED )
					{
						OnPropChanged( hWndDlg );
						return FALSE;
					}
					break;
				}

				case IDC_TAB_SIZE:
				{
					if ( wCmd == EN_CHANGE )
					{
						OnPropChanged( hWndDlg );
						return FALSE;
					}
					break;
				}
			}

			return TRUE;
		}

		case WM_NOTIFY:
		{
			switch ( ( ( NMHDR FAR * ) lParam )->code ) 
			{
				case PSN_SETACTIVE:
				{
					SetWindowLong( hWndDlg, DWL_MSGRESULT, PSNRET_NOERROR );
					g_nLastPage = 1;
					break;
				}
				case PSN_APPLY:
				{
					if ( pInfo->ShouldEatCommand() )
					{
						OnPropChanged( hWndDlg );
					}
					else
					{
						CEdit::IndentStyle eIndent = CEdit::eIndentPrev;
						if ( IsDlgButtonChecked( hWndDlg, IDC_OFF ) )
						{
							eIndent = CEdit::eIndentOff;
						}
						else if ( IsDlgButtonChecked( hWndDlg, IDC_SCOPE ) )
						{
							eIndent = CEdit::eIndentScope;
						}
						pInfo->m_eIndentStyle = eIndent;

						pInfo->m_nTabSize = GetDlgItemInt( hWndDlg, IDC_TAB_SIZE, NULL, FALSE );
						pInfo->m_nTabSize = max( CM_MIN_TABSIZE, pInfo->m_nTabSize );	// keep reasonable
						pInfo->m_nTabSize = min( CM_MAX_TABSIZE, pInfo->m_nTabSize );	// keep reasonable
						pInfo->m_bExpandTabs = IsDlgButtonChecked( hWndDlg, IDC_EXPAND_TABS );
						pInfo->m_bNormalizeCase = IsDlgButtonChecked( hWndDlg, IDC_NORM_CASE );

						HWND hWndLang = GetDlgItem( hWndDlg, IDC_LANGUAGE );
						int nSel = SendMessage( hWndLang, CB_GETCURSEL, 0, 0 );
						if ( nSel )
						{
							SendMessage( hWndLang, CB_GETLBTEXT, nSel, ( LPARAM ) pInfo->m_szLang );
						}
						else
						{
							*pInfo->m_szLang = '\0';
						}

						pInfo->CommitChanges( CPropInfo::eTabs );
					}
					SetWindowLong( hWndDlg, DWL_MSGRESULT, PSNRET_NOERROR );
					break;
				}
			}
		}
	}

	return FALSE;
}

#define UM_UPDATEDESCRIPTION	WM_USER + 100
#define UM_UPDATEBUTTONS		WM_USER + 101
#define UM_CLEARHOTKEYS			WM_USER + 102
#define UM_UPDATEHOTKEYS		WM_USER + 103
#define UM_UPDATEASSIGNMENT		WM_USER + 104
#define UM_SETHOOK				WM_USER + 105

#pragma warning (disable:4189)
BOOL CALLBACK PageKeyboardDlgProc( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	static BOOL bInitialized = FALSE;
	
	if ( !bInitialized && uMsg != WM_INITDIALOG )
		return FALSE;

	DWORD dwData = GetWindowLong( hWndDlg, GWL_USERDATA );
	CPropInfo *pInfo = dwData ? ( ( CPropInfo * ) ( ( PROPSHEETPAGE * ) dwData )->lParam ) : NULL;

	switch ( uMsg )
	{
		case WM_INITDIALOG:
		{
			SetWindowLong( hWndDlg, GWL_USERDATA, lParam );
			bInitialized = TRUE;
			CPropInfo *pInfo = ( ( CPropInfo * ) ( ( PROPSHEETPAGE * ) lParam )->lParam );
			CEdit *pCtrl = pInfo->m_pCtrl;
			// restore propdialog position
			HWND hWndParent = GetParent( hWndDlg );
			if ( !IsWindowVisible( hWndParent ) )
				pInfo->m_pCtrl->RestoreDlgPos( hWndParent, IDD_WINPROPS );
			pInfo->m_hWndKeyboardDlg = hWndDlg;
			HWND hWndCmds = GetDlgItem( hWndDlg, IDC_COMMANDS );

			TCHAR szCmd[ max( CM_MAX_CMD_DESCRIPTION, CM_MAX_CMD_STRING ) + 1 ];
			for ( int i = CMD_FIRST; i <= CMD_LAST; i++ )
			{
				pCtrl->GetCommandString( ( WORD ) i, FALSE, szCmd, ARRAY_SIZE( szCmd ) );
				if ( *szCmd )
				{
					int nSel = SendMessage( hWndCmds, LB_ADDSTRING, 0, ( LPARAM ) szCmd );
					SendMessage( hWndCmds, LB_SETITEMDATA, nSel, i );
				}
			}
			
			for ( int i = 0; i < pCtrl->g_nRegCmdCount; i++ )
			{
				WORD wCmd = pCtrl->g_pRegCmds[ i ].wCmd;
				pCtrl->GetCommandString( wCmd, FALSE, szCmd, ARRAY_SIZE( szCmd ) );
				int nSel = SendMessage( hWndCmds, LB_ADDSTRING, 0, ( LPARAM ) szCmd );
				SendMessage( hWndCmds, LB_SETITEMDATA, nSel, wCmd );
			}

			g_hWndHKeyCtrl = GetDlgItem( hWndDlg, IDC_HOTKEY );
			SendMessage( hWndDlg, UM_UPDATEBUTTONS, 0, 0 );
			SendMessage( hWndDlg, UM_UPDATEHOTKEYS, 0, 0 );

			// wait for the queue to simmer down and install a hook
			// that is called before the system hook.
			PostMessage( hWndDlg, UM_SETHOOK, 0, 0 );

			return TRUE;
		}

		case WM_DESTROY:
		{
			// save dlg pos for next time
			pInfo->m_pCtrl->SaveDlgPos( GetParent( hWndDlg ), IDD_WINPROPS );

			// reset for next time
			bInitialized = FALSE;

			if ( g_hhook )
			{
				UnhookWindowsHookEx( g_hhook );
				g_hhook = NULL;
			}
			return FALSE;
		}

		case UM_SETHOOK:
		{
			g_hhook = SetWindowsHookEx( WH_GETMESSAGE, 
			                            ( HOOKPROC ) HookProc, 
					 				    hInstance, 
									    NULL );
			return FALSE;
		}

		case UM_UPDATEDESCRIPTION:
		{
			HWND hWndCmds = GetDlgItem( hWndDlg, IDC_COMMANDS );
			int nSel = SendMessage( hWndCmds, LB_GETCURSEL, 0, 0 );
			
			TCHAR szDesc[ CM_MAX_CMD_DESCRIPTION ];
			if ( nSel == LB_ERR )
			{
				*szDesc = '\0';
			}
			else
			{
				DWORD dwData = SendMessage( hWndCmds, LB_GETITEMDATA, nSel, 0 );
				CEdit::GetCommandString( ( WORD ) dwData, TRUE, szDesc, ARRAY_SIZE( szDesc ) );
			}

			SetDlgItemText( hWndDlg, IDC_DESCRIPTION, szDesc );

			return FALSE;
		}

		case UM_UPDATEBUTTONS:
		{
			BOOL bCommandSelected = ( SendMessage( GetDlgItem( hWndDlg, IDC_COMMANDS ), LB_GETCURSEL, 0, 0 ) != LB_ERR );
			CM_HOTKEY cmHotKey;
			HWND hWndHotKey = GetDlgItem( hWndDlg, IDC_HOTKEY );
			SendMessage( hWndHotKey, HOTM_GETHOTKEY, 0, ( LPARAM ) &cmHotKey );
			EnableWindow( GetDlgItem( hWndDlg, IDC_ASSIGN ), cmHotKey.nVirtKey1 &&
			                                                 bCommandSelected );
			EnableWindow( GetDlgItem( hWndDlg, IDC_REMOVE ), SendMessage( GetDlgItem( hWndDlg, IDC_HOTKEYS ), LB_GETCURSEL, 0, 0 ) != LB_ERR );
			EnableWindow( hWndHotKey, bCommandSelected );
			return FALSE;
		}

		case UM_UPDATEASSIGNMENT:
		{
			TCHAR szHotKey[ 250 ];
			CM_HOTKEY cmHotKey;
			SendMessage( GetDlgItem( hWndDlg, IDC_HOTKEY ), HOTM_GETHOTKEY, 0, ( LPARAM ) &cmHotKey );
			CEdit::NormalizeHotKey( cmHotKey );

			if ( cmHotKey.nVirtKey1 )
			{
				_tcscpy( szHotKey, LoadStringPtr( IDS_CURRENTLY_ASSIGNED_TO_CR ) );

				WORD wCmd = 0;
				for ( int i = 0; i < pInfo->m_nAddCount; i++ )
				{
					if ( pInfo->m_pcmAddKey[ i ] == cmHotKey )
					{
						wCmd = pInfo->m_pwAddCmd[ i ];
						break;
					}
				}

				BOOL bRemoved = FALSE;
				for ( int i = 0; i < pInfo->m_nRemoveCount; i++ )
				{
					if ( pInfo->m_pcmRemoveKey[ i ] == cmHotKey )
					{
						bRemoved = TRUE;
						break;
					}
				}

				if ( !bRemoved && !wCmd )
				{
					int nDontCare;
					pInfo->m_pCtrl->LookupHotKey( cmHotKey, wCmd, nDontCare );
				}

				if ( wCmd )
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
			}
			else
			{
				*szHotKey = '\0';
			}

			SetDlgItemText(	hWndDlg, IDC_ASSIGNED_TO, szHotKey );
			return FALSE;
		}

		case UM_CLEARHOTKEYS:
		{
			HWND hWndKeys = GetDlgItem( hWndDlg, IDC_HOTKEYS );
			int nCount = SendMessage( hWndKeys, LB_GETCOUNT, 0, 0 );
			for ( int i = 0; i < nCount; i++ )
			{
				CM_HOTKEY *pHotKey = ( CM_HOTKEY * ) SendMessage( hWndKeys, LB_GETITEMDATA, i, 0 );
				ASSERT( pHotKey );
				delete pHotKey;
			}
			SendMessage( hWndKeys, LB_RESETCONTENT, 0, 0 );
			break;
		}

		case UM_UPDATEHOTKEYS:
		{
			// stuff the listbox with all hotkeys that are associated 
			// with the currently selected command.
			HWND hWndCmds = GetDlgItem( hWndDlg, IDC_COMMANDS );
			int nSel = SendMessage( hWndCmds, LB_GETCURSEL, 0, 0 );
			HWND hWndKeys = GetDlgItem( hWndDlg, IDC_HOTKEYS );
			// free up and clear all hotkey data stored in the listbox
			SendMessage( hWndDlg, UM_CLEARHOTKEYS, 0, 0 );
			
			TCHAR szHotKey[ 100 ];
			if ( nSel != LB_ERR )
			{
				WORD wCmd = ( WORD ) SendMessage( hWndCmds, LB_GETITEMDATA, nSel, 0 );
				CEdit *pCtrl = pInfo->m_pCtrl;
				int nHotKeys = pCtrl->g_nHotKeyCount;
				for ( int i = 0; i < nHotKeys; i++ )
				{
					if ( pCtrl->g_pHotKeys[ i ].wCmd == wCmd )
					{
						CM_HOTKEY &cmHotKey = pCtrl->g_pHotKeys[ i ].cmHotKey;
						// make sure this hotkey isn't about to be removed
						BOOL bRemoved = FALSE;
						for ( int j = 0; j < pInfo->m_nRemoveCount; j++ )
						{
							if ( pInfo->m_pcmRemoveKey[ j ] == cmHotKey )
							{
								bRemoved = TRUE;
								break;
							}
						}

						// make sure this hotkey wasn't reassigned to another cmd
						if ( !bRemoved )
						{
							for ( int j = 0; j < pInfo->m_nAddCount; j++ )
							{
								if ( pInfo->m_pcmAddKey[ j ] == cmHotKey && pInfo->m_pwAddCmd[ j ] != wCmd )
								{
									bRemoved = TRUE;
									break;
								}
							}
						}

						if ( !bRemoved )
						{
							// Add the hotkey to the listbox
							GetHotKeyString( cmHotKey, szHotKey );
							int nSel = SendMessage( hWndKeys, LB_ADDSTRING, 0, ( LPARAM ) szHotKey );
							CM_HOTKEY *pHotKey = new CM_HOTKEY;
							*pHotKey = cmHotKey;
							SendMessage( hWndKeys, LB_SETITEMDATA, nSel, ( LPARAM ) pHotKey );
						}
					}
				}

				nHotKeys = pInfo->m_nAddCount;
				for ( int i = 0; i < nHotKeys; i++ )
				{
					if ( pInfo->m_pwAddCmd[ i ] == wCmd )
					{
						// Add the hotkey to the listbox if not already in the listbox
						CM_HOTKEY &cmHotKey = pInfo->m_pcmAddKey[ i ];

						BOOL bAlreadyAdded = FALSE;
						int nLBCount = SendMessage( hWndKeys, LB_GETCOUNT, 0, 0 );
						for ( int j = 0; !bAlreadyAdded && j < nLBCount; j++ )
						{
							CM_HOTKEY *pHotKey2;
							pHotKey2 = ( CM_HOTKEY * ) SendMessage( hWndKeys, LB_GETITEMDATA, j, 0 );
							ASSERT( pHotKey2 );
							bAlreadyAdded = ( cmHotKey == *pHotKey2 );
						}

						if ( !bAlreadyAdded )
						{
							GetHotKeyString( cmHotKey, szHotKey );
							int nSel = SendMessage( hWndKeys, LB_ADDSTRING, 0, ( LPARAM ) szHotKey );
							CM_HOTKEY *pHotKey = new CM_HOTKEY;
							*pHotKey = cmHotKey;
							SendMessage( hWndKeys, LB_SETITEMDATA, nSel, ( LPARAM ) pHotKey );
						}
					}
				}
			}

			return 0;
		}

		case WM_COMMAND:
		{
			BOOL bEatCommand = SendMessage( GetDlgItem( hWndDlg, IDC_HOTKEY ), HOTM_QUERYEATCOMMAND, 0, 0 );
			WORD wID = LOWORD( wParam );
			WORD wCmd = HIWORD( wParam );

			switch ( wID )
			{
				case IDC_COMMANDS:
				{
					if ( wCmd == LBN_SELCHANGE )
					{
						SendMessage( hWndDlg, UM_UPDATEDESCRIPTION, 0, 0 );
						SendMessage( hWndDlg, UM_UPDATEHOTKEYS, 0, 0 );
						SendMessage( hWndDlg, UM_UPDATEBUTTONS, 0, 0 );
						return 0;
					}
					break;
				}

				case IDC_HOTKEYS:
				{
					if ( wCmd == LBN_SELCHANGE )
					{
						SendMessage( hWndDlg, UM_UPDATEBUTTONS, 0, 0 );
						return 0;
					}
					break;
				}

				case IDC_HOTKEY:
				{
					if ( wCmd == EN_CHANGE )
					{
						SendMessage( hWndDlg, UM_UPDATEBUTTONS, 0, 0 );
						SendMessage( hWndDlg, UM_UPDATEASSIGNMENT, 0, 0 );
						return 0;
					}
					break;
				}

				case IDC_ASSIGN:
				{
					if ( wCmd == BN_CLICKED && !bEatCommand  )
					{
						HWND hWndCmds = GetDlgItem( hWndDlg, IDC_COMMANDS );
						HWND hWndHotKey = GetDlgItem( hWndDlg, IDC_HOTKEY );
						CM_HOTKEY cmHotKey;
						SendMessage( hWndHotKey, HOTM_GETHOTKEY, 0, ( LPARAM ) &cmHotKey );
						pInfo->AddHotKey( cmHotKey,
										  ( WORD ) SendMessage( hWndCmds, LB_GETITEMDATA, SendMessage( hWndCmds, LB_GETCURSEL, 0, 0 ), 0 ) );
						SendMessage( hWndDlg, UM_UPDATEHOTKEYS, 0, 0 );
						SendMessage( hWndHotKey, HOTM_SETHOTKEY, 0, 0 );
						SendMessage( hWndDlg, UM_UPDATEASSIGNMENT, 0, 0 );
						OnPropChanged( hWndDlg );
						return 0;
					}
					break;
				}

				case IDC_REMOVE:
				{
					if ( wCmd == BN_CLICKED && !bEatCommand )
					{
						HWND hWndHotKeys = GetDlgItem( hWndDlg, IDC_HOTKEYS );
						CM_HOTKEY *pHotKey = ( CM_HOTKEY * ) SendMessage( hWndHotKeys, LB_GETITEMDATA, SendMessage( hWndHotKeys, LB_GETCURSEL, 0, 0 ), 0 );
						pInfo->RemoveHotKey( *pHotKey );
						SendMessage( hWndDlg, UM_UPDATEHOTKEYS, 0, 0 );
						SendMessage( hWndDlg, UM_UPDATEASSIGNMENT, 0, 0 );
						SendMessage( hWndDlg, UM_UPDATEBUTTONS, 0, 0 );
						OnPropChanged( hWndDlg );
						return 0;
					}
					break;
				}

				case IDC_RESET:
				{
					if ( wCmd == BN_CLICKED && !bEatCommand )
					{
						pInfo->ResetHotKeys();
						SendMessage( hWndDlg, UM_UPDATEHOTKEYS, 0, 0 );
						SendMessage( hWndDlg, UM_UPDATEASSIGNMENT, 0, 0 );
						SendMessage( hWndDlg, UM_UPDATEBUTTONS, 0, 0 );
						OnPropChanged( hWndDlg );
						return 0;
					}
					break;
				}
			}
			return TRUE;
		}

		case WM_NOTIFY:
		{
			switch ( ( ( NMHDR FAR * ) lParam )->code ) 
			{
				case PSN_SETACTIVE:
				{
					SetWindowLong( hWndDlg, DWL_MSGRESULT, PSNRET_NOERROR );
					g_nLastPage = 2;
					break;
				}
				case PSN_APPLY:
				{
					if ( pInfo->ShouldEatCommand() )
					{
						OnPropChanged( hWndDlg );
					}
					else
					{
						pInfo->CommitChanges( CPropInfo::eKeyboard );
					}
					SetWindowLong( hWndDlg, DWL_MSGRESULT, PSNRET_NOERROR );
					break;
				}
			}
		}
	}

	return FALSE;
}

void local_itoa( int n, LPTSTR psz, int nRadix )
{
	LPTSTR pszStartAt = psz;
	if ( nRadix == CM_HEXADECIMAL )
	{
		_tcscpy( psz, _T("0x") );
		pszStartAt += 2;
	}
	_itot( n, pszStartAt, nRadix );
}

#define UM_UPDATEEDIT				WM_USER + 100
#define UM_UPDATESPLITTERCHECKS		WM_USER + 101
#define UM_GETLINENUMRADIX			WM_USER + 102

BOOL CALLBACK PageMiscDlgProc( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	static BOOL bInitialized = FALSE;
	
	if ( !bInitialized && uMsg != WM_INITDIALOG )
		return FALSE;

	DWORD dwData = GetWindowLong( hWndDlg, GWL_USERDATA );
	CPropInfo *pInfo = dwData ? ( ( CPropInfo * ) ( ( PROPSHEETPAGE * ) dwData )->lParam ) : NULL;

	static BOOL nLocked = 0;

	switch ( uMsg )
	{
		case WM_INITDIALOG:
		{
			SetWindowLong( hWndDlg, GWL_USERDATA, lParam );
			bInitialized = TRUE;
			CPropInfo *pInfo = ( ( CPropInfo * ) ( ( PROPSHEETPAGE * ) lParam )->lParam );
			CEdit *pCtrl = pInfo->m_pCtrl;
			// restore propdialog position
			HWND hWndParent = GetParent( hWndDlg );
			if ( !IsWindowVisible( hWndParent ) )
				pCtrl->RestoreDlgPos( hWndParent, IDD_WINPROPS );
				
			CheckDlgButton( hWndDlg, IDC_SMOOTH_SCROLL, pCtrl->m_bSmoothScroll );
			CheckDlgButton( hWndDlg, IDC_LEFT_MARGIN, pCtrl->m_bShowLeftMargin );
			CheckDlgButton( hWndDlg, IDC_DRAG_DROP, pCtrl->m_bAllowDragDrop );
			CheckDlgButton( hWndDlg, IDC_LINE_TOOLTIP, pCtrl->m_bLineToolTips );
			CheckDlgButton( hWndDlg, IDC_COLUMN_SEL, pCtrl->m_bAllowColumnSel );
			CheckDlgButton( hWndDlg, IDC_COLOR_SYNTAX, pCtrl->m_bColorSyntax );
			CheckDlgButton( hWndDlg, IDC_H_SCROLL, pCtrl->ShowHScrollBar() );
			CheckDlgButton( hWndDlg, IDC_V_SCROLL, pCtrl->ShowVScrollBar() );
			CheckDlgButton( hWndDlg, IDC_SEL_BOUNDS, pCtrl->BoundSelection() );
			CheckDlgButton( hWndDlg, IDC_H_SPLIT, pCtrl->m_bAllowHSplit );
			CheckDlgButton( hWndDlg, IDC_V_SPLIT, pCtrl->m_bAllowVSplit );
			int nMaxUndo = pCtrl->m_Buffer.GetMaxUndo();
			CheckDlgButton( hWndDlg, IDC_UNLIMITED, nMaxUndo == -1 );
			CheckDlgButton( hWndDlg, IDC_LIMITED, nMaxUndo != -1 );
			SendMessage( GetDlgItem( hWndDlg, IDC_MAX_UNDO ), EM_LIMITTEXT, 4, 0 );
			SendMessage( hWndDlg, UM_UPDATEEDIT, 0, 0 );
			SendMessage( hWndDlg, UM_UPDATESPLITTERCHECKS, 0, 0 );

			// display the 'start at' value in the correct radix
			pInfo->m_cmLineNum = pCtrl->m_LineNum;
			TCHAR szStartAt[ 50 ];
			local_itoa( pCtrl->m_LineNum.nStartAt, szStartAt, ( int ) pCtrl->m_LineNum.dwStyle );
			nLocked++;
			SetDlgItemText( hWndDlg, IDC_NUMSTART, szStartAt );
			nLocked--;

			struct _Style {
				UINT unStyle;
				DWORD dwStyle;
			} Styles[] = { { IDS_NONE, 0 },
			               { IDS_BINARY, 2 },
			               { IDS_OCTAL, 8 },
			               { IDS_DECIMAL, 10 },
			               { IDS_HEX, 16 } };

			HWND hWndCombo = GetDlgItem( hWndDlg, IDC_NUMSTYLE );
			BOOL bFound = FALSE;
			DWORD dwCurStyle = pCtrl->m_LineNum.dwStyle;
			if ( !pCtrl->LineNumberingEnabled() )
				dwCurStyle = 0;
			for ( int i = 0; i < ARRAY_SIZE( Styles ); i++ )
			{
				_Style *pStyle = Styles + i;
				int nItem = SendMessage( hWndCombo, CB_ADDSTRING, 0, ( LPARAM ) LoadStringPtr( pStyle->unStyle ) );
				SendMessage( hWndCombo, CB_SETITEMDATA, nItem, pStyle->dwStyle );
				if ( dwCurStyle == pStyle->dwStyle )
				{
					// current selection -- set it now
					SendMessage( hWndCombo, CB_SETCURSEL, nItem, 0 );
					bFound = TRUE;
				}
			}

			if ( !bFound )
			{
				// a custom line number style was chosen -- preserve this as 'custom'
				int nItem = SendMessage( hWndCombo, CB_ADDSTRING, 0, ( LPARAM ) LoadStringPtr( IDS_CUSTOM ) );
				SendMessage( hWndCombo, CB_SETITEMDATA, nItem, pCtrl->m_LineNum.dwStyle );
				SendMessage( hWndCombo, CB_SETCURSEL, nItem, 0 );
			}
			

			return TRUE;
			break;
		}

		case WM_DESTROY:
		{
			// save dlg pos for next time
			pInfo->m_pCtrl->SaveDlgPos( GetParent( hWndDlg ), IDD_WINPROPS );

			// reset for next time
			bInitialized = FALSE;
			return FALSE;
		}

		case UM_UPDATEEDIT:
		{
			BOOL bEnable;
			nLocked++;
			if ( IsDlgButtonChecked( hWndDlg, IDC_UNLIMITED ) )
			{
				SetDlgItemText( hWndDlg, IDC_MAX_UNDO, _T("") );
				bEnable = FALSE;
			}
			else
			{
				int nMaxUndo = pInfo->m_pCtrl->m_Buffer.GetMaxUndo();
				if ( nMaxUndo == -1 )
				{
					nMaxUndo = 10;
				}
				SetDlgItemInt( hWndDlg, IDC_MAX_UNDO, nMaxUndo, TRUE );
				bEnable = TRUE;
			}
			nLocked--;
			EnableWindow( GetDlgItem( hWndDlg, IDC_MAX_UNDO ), bEnable );
			break;
		}

		case UM_UPDATESPLITTERCHECKS:
		{
			// user cannot show the splitters unless the scrollbars are also shown
			BOOL bShowScroll = IsDlgButtonChecked( hWndDlg, IDC_H_SCROLL );
			if ( !bShowScroll )
			{
				CheckDlgButton( hWndDlg, IDC_H_SPLIT, FALSE );
			}
			EnableWindow( GetDlgItem( hWndDlg, IDC_H_SPLIT ), bShowScroll );

			bShowScroll = IsDlgButtonChecked( hWndDlg, IDC_V_SCROLL );
			if ( !bShowScroll )
			{
				CheckDlgButton( hWndDlg, IDC_V_SPLIT, FALSE );
			}
			EnableWindow( GetDlgItem( hWndDlg, IDC_V_SPLIT ), bShowScroll );
			break;
		}

		case UM_GETLINENUMRADIX:
		{
			HWND hWndCombo = GetDlgItem( hWndDlg, IDC_NUMSTYLE );
			int nCurSel = SendMessage( hWndCombo, CB_GETCURSEL, 0, 0 );
			ASSERT( nCurSel != CB_ERR );
			int nStyle = SendMessage( hWndCombo, CB_GETITEMDATA, nCurSel, 0 );
			pInfo->m_cmLineNum.bEnabled = ( nStyle != 0 && nStyle != CB_ERR );
			pInfo->m_cmLineNum.dwStyle = nStyle;
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
					if ( wID == IDC_H_SCROLL || wID == IDC_V_SCROLL )
					{
						SendMessage( hWndDlg, UM_UPDATESPLITTERCHECKS, 0, 0 );
					}
					// fall through...
				}
				case CBN_SELCHANGE:
				case EN_CHANGE:
				{
					if ( !nLocked )
					{
						if ( wCmd == CBN_SELCHANGE && wID == IDC_NUMSTYLE )
						{
							// convert the 'start at' value from the old radix to the
							// new radix.

							// save old info
							CM_LINENUMBERING lniOld = pInfo->m_cmLineNum;
							// cache current info
							SendMessage( hWndDlg, UM_GETLINENUMRADIX, 0, 0 );

							if ( !lniOld.bEnabled || !pInfo->m_cmLineNum.bEnabled )
							{
								SetDlgItemText( hWndDlg, IDC_NUMSTART, _T("0") );
							}
							else
							{
								// display the 'start at' value in the correct radix
								TCHAR szStartAt[ 50 ];
								GetDlgItemText( hWndDlg, IDC_NUMSTART, szStartAt, ARRAY_SIZE( szStartAt ) - 1 );
								int nStartAt = _atoi( szStartAt, ( int ) lniOld.dwStyle );
								local_itoa( nStartAt, szStartAt, ( int ) pInfo->m_cmLineNum.dwStyle );
								SetDlgItemText( hWndDlg, IDC_NUMSTART, szStartAt );
							}
						}

						OnPropChanged( hWndDlg );
						if ( wID == IDC_UNLIMITED || wID == IDC_LIMITED )
						{
							SendMessage( hWndDlg, UM_UPDATEEDIT, 0, 0 );
						}
					}
					return FALSE;
				}
			}
			return TRUE;
		}

		case WM_NOTIFY:
		{
			switch ( ( ( NMHDR FAR * ) lParam )->code ) 
			{
				case PSN_SETACTIVE:
				{
					SetWindowLong( hWndDlg, DWL_MSGRESULT, PSNRET_NOERROR );
					g_nLastPage = 3;
					break;
				}
				case PSN_APPLY:
				{
					if ( pInfo->ShouldEatCommand() )
					{
						OnPropChanged( hWndDlg );
					}
					else
					{
						pInfo->m_bSmoothScroll = IsDlgButtonChecked( hWndDlg, IDC_SMOOTH_SCROLL );
						pInfo->m_bShowLeftMargin = IsDlgButtonChecked( hWndDlg, IDC_LEFT_MARGIN );
						pInfo->m_bAllowDragDrop = IsDlgButtonChecked( hWndDlg, IDC_DRAG_DROP );
						pInfo->m_bLineToolTips = IsDlgButtonChecked( hWndDlg, IDC_LINE_TOOLTIP );
						pInfo->m_bAllowColumnSel = IsDlgButtonChecked( hWndDlg, IDC_COLUMN_SEL );
						pInfo->m_bColorSyntax = IsDlgButtonChecked( hWndDlg, IDC_COLOR_SYNTAX );
						pInfo->m_bShowHScrollBar = IsDlgButtonChecked( hWndDlg, IDC_H_SCROLL );
						pInfo->m_bShowVScrollBar = IsDlgButtonChecked( hWndDlg, IDC_V_SCROLL );
						pInfo->m_bSelBounds = IsDlgButtonChecked( hWndDlg, IDC_SEL_BOUNDS );
						pInfo->m_bAllowHSplit = IsDlgButtonChecked( hWndDlg, IDC_H_SPLIT );
						pInfo->m_bAllowVSplit = IsDlgButtonChecked( hWndDlg, IDC_V_SPLIT );
						pInfo->m_nMaxUndo = IsDlgButtonChecked( hWndDlg, IDC_UNLIMITED ) ? -1 :
											GetDlgItemInt( hWndDlg, IDC_MAX_UNDO, NULL, FALSE );
						SendMessage( hWndDlg, UM_GETLINENUMRADIX, 0, 0 );

						TCHAR szStartAt[ 50 ];
						GetDlgItemText( hWndDlg, IDC_NUMSTART, szStartAt, ARRAY_SIZE( szStartAt ) - 1 );
						pInfo->m_cmLineNum.nStartAt = _atoi( szStartAt, ( int ) pInfo->m_cmLineNum.dwStyle );

						pInfo->CommitChanges( CPropInfo::eMisc );
					}
					SetWindowLong( hWndDlg, DWL_MSGRESULT, PSNRET_NOERROR );
					break;
				}
			}
		}
	}

	return FALSE;
}

#include <pshpack1.h>
typedef struct DLGTEMPLATEEX
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
} DLGTEMPLATEEX, *LPDLGTEMPLATEEX;
#include <poppack.h>

int CALLBACK PropSheetProc( HWND /*hWndSheet*/, UINT unMsg, LPARAM lParam )
{
    switch ( unMsg )
	{
		case PSCB_PRECREATE:
		{
			// Remove the DS_CONTEXTHELP style from the dialog template
			if (((LPDLGTEMPLATEEX)lParam)->signature ==  0xFFFF)
				((LPDLGTEMPLATEEX)lParam)->style &= ~DS_CONTEXTHELP;
			else 
				((LPDLGTEMPLATE)lParam)->style &= ~DS_CONTEXTHELP;
			return TRUE;
		}
	}
    return TRUE;
}

void CEdit::Properties()
{
	if ( IsPlayingMacro() )
	{
		// in the middle of a macro playback -- don't pop up the properties dialog now!
		return;
	}

	// the tabsize may change -- make sure the caret is updated
	CSaveSelection save( this );

	if ( !NotifyParent( CMN_SHOWPROPS ) )
	{
		// parent did not handle the properties dialog
		CPropInfo Info;
		Info.m_pCtrl = this;

		HGLOBAL hColorFontTemplate	= LoadDialogTemplate( MAKEINTRESOURCE( IDD_PROPPAGE_COLORFONT ) );
		HGLOBAL hTabsTemplate		= LoadDialogTemplate( MAKEINTRESOURCE( IDD_PROPPAGE_TABS ) );
		HGLOBAL hKeyboardTemplate	= LoadDialogTemplate( MAKEINTRESOURCE( IDD_PROPPAGE_KEYBOARD ) );
		HGLOBAL hMiscTemplate		= LoadDialogTemplate( MAKEINTRESOURCE( IDD_PROPPAGE_MISC ) );

		PROPSHEETPAGE Pages[] = { { sizeof( PROPSHEETPAGE ),
									PSP_USEICONID | PSP_RTLREADING | PSP_DLGINDIRECT,
									hInstance,
									( LPCTSTR ) GlobalLock( hColorFontTemplate ),
									NULL,
									NULL,
									( DLGPROC ) PageColorFontDlgProc,
									( LPARAM ) &Info,
									NULL,
									NULL },
								  { sizeof( PROPSHEETPAGE ),
									PSP_USEICONID | PSP_RTLREADING | PSP_DLGINDIRECT,
									hInstance,
									( LPCTSTR ) GlobalLock( hTabsTemplate ),
									NULL,
									NULL,
									( DLGPROC ) PageTabsDlgProc,
									( LPARAM ) &Info,
									NULL,
									NULL },
								  { sizeof( PROPSHEETPAGE ),
									PSP_USEICONID | PSP_RTLREADING | PSP_DLGINDIRECT,
									hInstance,
									( LPCTSTR ) GlobalLock( hKeyboardTemplate ),
									NULL,
									NULL,
									( DLGPROC ) PageKeyboardDlgProc,
									( LPARAM ) &Info,
									NULL,
									NULL },
								  { sizeof( PROPSHEETPAGE ),
									PSP_USEICONID | PSP_RTLREADING | PSP_DLGINDIRECT,
									hInstance,
									( LPCTSTR ) GlobalLock( hMiscTemplate ),
									NULL,
									NULL,
									( DLGPROC ) PageMiscDlgProc,
									( LPARAM ) &Info,
									NULL,
									NULL } };

		PROPSHEETHEADER Sheet = { sizeof( PROPSHEETHEADER ), 
								  PSH_PROPSHEETPAGE | PSH_RTLREADING | PSH_USECALLBACK,
								  m_hWndDlgParent,
								  hInstance,
								  NULL,
								  LoadStringPtr( IDS_WIN_PROPS ),
								  ARRAY_SIZE( Pages ),
								  g_nLastPage,
								  Pages,
								  ( PFNPROPSHEETCALLBACK ) PropSheetProc };

		CNoHideSel disable( this );  // don't honor hidesel behavior here
		PropertySheet( &Sheet );

		UnlockAndFreeDialogTemplate( hColorFontTemplate );
		UnlockAndFreeDialogTemplate( hTabsTemplate );
		UnlockAndFreeDialogTemplate( hKeyboardTemplate );
		UnlockAndFreeDialogTemplate( hMiscTemplate );

		// some ActiveX containers cause the caret to go away -- put it back
		FlashCaret();
	}
}
