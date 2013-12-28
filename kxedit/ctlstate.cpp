// ctrlstate.cpp : Implementation of CControlState
#include "precomp.h"
#ifdef _ACTIVEX
#include "editx.h"
#include "ceditx.h"

CControlState::CControlState()
{
	m_bColorSyntax = DEF_COLORSYNTAX;
	m_bAllowHSplit = DEF_ALLOWHSPLIT;
	m_bAllowVSplit = DEF_ALLOWVSPLIT;
	m_bHScroll = DEF_HSCROLL;
	m_bVScroll = DEF_VSCROLL;
	m_bSmoothScroll = DEF_SMOOTHSCROLL;
	m_bLineToolTips = DEF_LINETOOLTIPS;
	m_bShowLeftMargin = DEF_SHOWLEFTMARGIN;
	m_bAllowColumnSel = DEF_ALLOWCOLUMNSEL;
	m_bAllowDragDrop = DEF_ALLOWDRAGDROP;
	m_bExpandTabs = DEF_EXPANDTABS;
	m_xPosHSplitter = DEF_POSHSPLITTER;
	m_yPosVSplitter = DEF_POSVSPLITTER;
	m_eIndentStyle = DEF_INDENTSTYLE;
	m_nTabSize = DEF_TABSIZE;
	m_bDisplayWhitespace = DEF_DISPLAYWHITESPACE;
	m_bWantCarriageReturn = DEF_WANTCARRIAGERETURN;
	m_bEnabled = DEF_ENABLED;
	m_bGlobalProps = DEF_GLOBALPROPS;
    m_bModified = DEF_MODIFIED;
    m_bOverType = DEF_OVERTYPE;
    m_bReadOnly = DEF_READONLY;
    m_bPreserveCase = DEF_PRESERVECASE;
    m_bCaseSensitiveSearch = DEF_CASESENSITIVESEARCH;
    m_bWholeWordOnly = DEF_WHOLEWORDONLY;
    m_bRegExp = DEF_REGEXP;
    m_nMaxUndo = DEF_MAXUNDO;
	m_bSelBounds = DEF_SELBOUNDS;
	m_bHideSel = DEF_HIDESEL;
	m_bNormalizeCase = DEF_NORMALIZECASE;
	m_bOvertypeCaret = DEF_OVERTYPECARET;
	m_nHighlightedLine = DEF_HIGHLIGHTEDLINE;
	m_dwBorderStyle = DEF_BORDERSTYLE;

	m_Colors.crWindow					= DEF_CLRWINDOW;
	m_Colors.crLeftMargin				= DEF_CLRLEFTMARGIN;
	m_Colors.crBookmark					= DEF_CLRBOOKMARK;
	m_Colors.crBookmarkBk				= DEF_CLRBOOKMARKBK;
	m_Colors.crText						= DEF_CLRTEXT;
	m_Colors.crTextBk					= DEF_CLRTEXTBK;
	m_Colors.crNumber					= DEF_CLRNUMBER;
	m_Colors.crNumberBk					= DEF_CLRNUMBERBK;
	m_Colors.crKeyword					= DEF_CLRKEYWORD;
	m_Colors.crKeywordBk				= DEF_CLRKEYWORDBK;
	m_Colors.crOperator					= DEF_CLROPERATOR;
	m_Colors.crOperatorBk				= DEF_CLROPERATORBK;
	m_Colors.crScopeKeyword				= DEF_CLRSCOPEKEYWORD;
	m_Colors.crScopeKeywordBk			= DEF_CLRSCOPEKEYWORDBK;
	m_Colors.crComment					= DEF_CLRCOMMENT;
	m_Colors.crCommentBk				= DEF_CLRCOMMENTBK;
	m_Colors.crString					= DEF_CLRSTRING;
	m_Colors.crStringBk					= DEF_CLRSTRINGBK;
	m_Colors.crTagText					= DEF_CLRTAGTEXT;
	m_Colors.crTagTextBk				= DEF_CLRTAGTEXTBK;
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
	
	ZeroMemory( &m_lf, sizeof( m_lf ) );

	m_LineNum.bEnabled					= DEF_LINENUMENABLE;
	m_LineNum.nStartAt					= DEF_LINENUMBERSTART;
	m_LineNum.dwStyle                   = DEF_LINENUMBERSTYLE;

	m_bstrText = NULL;
	m_bstrLang = NULL;
}
	
CControlState::~CControlState()
{
	FreeStrings();
}

void CControlState::FreeStrings()
{
	if ( m_bstrText )
	{
		SysFreeString( m_bstrText );
		m_bstrText = NULL;
	}
	if ( m_bstrLang )
	{
		SysFreeString( m_bstrLang );
		m_bstrLang = NULL;
	}
}

void CControlState::Load( CEditX *pCtrl )
{
	if ( !pCtrl )
		return;
	ASSERT( IsWindow( pCtrl->m_hWnd ) );
	CEdit *pEdit = pCtrl->m_pEdit;
	if ( !pEdit )
		return;

	CBuffer *pBuff = pEdit->GetBuffer();
	
	m_bColorSyntax = pEdit->m_bColorSyntax;
	m_bAllowHSplit = pEdit->m_bAllowHSplit;
	m_bAllowVSplit = pEdit->m_bAllowVSplit;
	m_bHScroll = pEdit->ShowHScrollBar();
	m_bVScroll = pEdit->ShowVScrollBar();
	m_bSmoothScroll = pEdit->m_bSmoothScroll;
	m_bLineToolTips = pEdit->m_bLineToolTips;
	m_bShowLeftMargin = pEdit->m_bShowLeftMargin;
	m_bAllowColumnSel = pEdit->m_bAllowColumnSel;
	m_bAllowDragDrop = pEdit->m_bAllowDragDrop;
	m_bExpandTabs = pEdit->m_bExpandTabs;
	m_xPosHSplitter = pEdit->m_xPosHSplitter;
	m_yPosVSplitter = pEdit->m_yPosVSplitter;
	m_eIndentStyle = pEdit->m_eIndentStyle;
	m_nTabSize = pBuff->GetTabSize();
	m_bDisplayWhitespace = pEdit->m_bDisplayWhitespace;
	m_bWantCarriageReturn = pBuff->WantCarriageReturn();
	m_bEnabled = IsWindowEnabled( pEdit->m_hWnd );
	m_bGlobalProps = pEdit->m_bGlobalProps;
    m_bModified = pBuff->IsModified();
    m_bOverType = pEdit->m_bOverType;
    m_bReadOnly = pBuff->IsReadOnly();
    m_bPreserveCase = pEdit->g_FindReplaceData.m_bPreserveCase;
    m_bCaseSensitiveSearch = pEdit->g_FindReplaceData.m_bCaseSensitiveSearch;
    m_bWholeWordOnly = pEdit->g_FindReplaceData.m_bWholeWordOnly;
    m_bRegExp = pEdit->g_FindReplaceData.m_bRegExp;
    m_nMaxUndo = pBuff->GetMaxUndo();
	m_Colors = pEdit->m_Colors;
	m_FontStyles = pEdit->m_FontStyles;
	m_bSelBounds = pEdit->m_bSelBounds;
	m_bHideSel = pEdit->m_bHideSel;
	m_bNormalizeCase = pEdit->m_Buffer.NormalizeCase();
	m_LineNum = pEdit->m_LineNum;
	m_bOvertypeCaret = pEdit->m_bOvertypeCaret;
	m_nHighlightedLine = pEdit->m_nHighlightedLine;
	m_dwBorderStyle = CM_GetBorderStyle( pCtrl->m_hWnd );

	HFONT hFont = ( HFONT ) SendMessage( pCtrl->m_hWnd, WM_GETFONT, 0, 0 );
	int cbSize = GetObject( hFont, sizeof( LOGFONT ), NULL );
	VERIFY( GetObject( hFont, cbSize, &m_lf ) );
   	pCtrl->get_Text( &m_bstrText );
   	pCtrl->get_Language( &m_bstrLang );
}

void CControlState::Save( CEditX *pCtrl )
{
	if ( !pCtrl )
		return;
	BOOL bSaveState = pCtrl->m_bRequiresSave;

	HWND hWnd = pCtrl->m_hWnd;
	ASSERT( IsWindow( hWnd ) );
   	pCtrl->put_Text( m_bstrText );
	CM_EnableColorSyntax( hWnd, m_bColorSyntax );
	CM_EnableSplitter( hWnd, TRUE, m_bAllowHSplit );
	CM_EnableSplitter( hWnd, FALSE, m_bAllowVSplit );
	CM_ShowScrollBar( hWnd, TRUE, m_bHScroll );
	CM_ShowScrollBar( hWnd, FALSE, m_bVScroll );
	CM_EnableSmoothScrolling( hWnd, m_bSmoothScroll );
	CM_EnableLineToolTips( hWnd, m_bLineToolTips );
	CM_EnableLeftMargin( hWnd, m_bShowLeftMargin );
	CM_EnableColumnSel( hWnd, m_bAllowColumnSel );
	CM_EnableDragDrop( hWnd, m_bAllowDragDrop );
	CM_EnableTabExpand( hWnd, m_bExpandTabs );
	CM_SetSplitterPos( hWnd, TRUE, m_xPosHSplitter );
	CM_SetSplitterPos( hWnd, FALSE, m_yPosVSplitter );
	CM_SetAutoIndentMode( hWnd, m_eIndentStyle );
	CM_SetTabSize( hWnd, m_nTabSize );
	CM_EnableWhitespaceDisplay( hWnd, m_bDisplayWhitespace );
	CM_EnableCRLF( hWnd, m_bWantCarriageReturn );
	EnableWindow( hWnd, m_bEnabled );
	CM_EnableGlobalProps( hWnd, m_bGlobalProps );
    CM_SetModified( hWnd, m_bModified );
    CM_EnableOvertype( hWnd, m_bOverType );
    CM_SetReadOnly( hWnd, m_bReadOnly );
    CM_EnablePreserveCase( hWnd, m_bPreserveCase );
    CM_EnableCaseSensitive( hWnd, m_bCaseSensitiveSearch );
    CM_EnableWholeWord( hWnd, m_bWholeWordOnly );
    CM_EnableRegExp( hWnd, m_bRegExp );
    CM_SetUndoLimit( hWnd, m_nMaxUndo );
	CM_SetColors( hWnd, &m_Colors );
	CM_EnableSelBounds( hWnd, m_bSelBounds );
	CM_EnableHideSel( hWnd, m_bHideSel );
	CM_EnableNormalizeCase( hWnd, m_bNormalizeCase );
	CM_EnableOvertypeCaret( hWnd, m_bOvertypeCaret );
	SendMessage( hWnd, WM_SETFONT, ( WPARAM ) CreateFontIndirect( &m_lf ), 0 );
	CM_SetFontOwnership( hWnd, TRUE );
	CM_SetFontStyles( hWnd, &m_FontStyles );
	CM_SetLineNumbering( hWnd, &m_LineNum );
   	pCtrl->put_Language( m_bstrLang );
	CM_SetHighlightedLine( hWnd, m_nHighlightedLine );
	CM_SetBorderStyle( hWnd, m_dwBorderStyle );
	pCtrl->m_bRequiresSave = bSaveState;
}

#define READDATA( var ) if ( FAILED( hr = pStm->Read( &var, sizeof( var ), NULL ) ) ) goto bail;
HRESULT CControlState::Load( LPSTREAM pStm )
{
	HRESULT hr = S_OK;
	CComVariant var;

	DWORD dwVer;
	READDATA( dwVer );

	// version 1 and 2 are pre-2.0 and are not backward compatible.  Ignore this data.
	if ( dwVer < 3 )
	{
		return S_FALSE;
	}
	
	READDATA( m_bColorSyntax );
	READDATA( m_bAllowHSplit );
	READDATA( m_bAllowVSplit );
	READDATA( m_bHScroll );
	READDATA( m_bVScroll );
	READDATA( m_bSmoothScroll );
	READDATA( m_bLineToolTips );
	READDATA( m_bShowLeftMargin );
	READDATA( m_bAllowColumnSel );
	READDATA( m_bAllowDragDrop );
	READDATA( m_bExpandTabs );
	READDATA( m_xPosHSplitter );
	READDATA( m_yPosVSplitter );
	READDATA( m_eIndentStyle );
	READDATA( m_nTabSize );
	READDATA( m_Colors );
	READDATA( m_lf );

	// dealloc old strings
	FreeStrings();

	if ( SUCCEEDED( hr = var.ReadFromStream( pStm ) ) )
	{
    	m_bstrText = var.bstrVal;
    	var.bstrVal = NULL;
    	var.vt = VT_EMPTY;
	}
	else
		goto bail;

	if ( SUCCEEDED( hr = var.ReadFromStream( pStm ) ) )
	{
    	m_bstrLang = var.bstrVal;
    	var.bstrVal = NULL;
    	var.vt = VT_EMPTY;
	}
	else
		goto bail;
	
	READDATA( m_bDisplayWhitespace );
	READDATA( m_bWantCarriageReturn );
	READDATA( m_bEnabled );
	READDATA( m_bGlobalProps );
	READDATA( m_bModified );
	READDATA( m_bOverType );
	READDATA( m_bReadOnly );
	READDATA( m_bPreserveCase );
	READDATA( m_bCaseSensitiveSearch );
	READDATA( m_bWholeWordOnly );
	READDATA( m_nMaxUndo );
	READDATA( m_bSelBounds );
	READDATA( m_bRegExp );
	READDATA( m_FontStyles );
	READDATA( m_LineNum );
	READDATA( m_bHideSel );
	READDATA( m_bNormalizeCase );
	READDATA( m_bOvertypeCaret );
	READDATA( m_nHighlightedLine );

	if ( dwVer < 4 )
		goto bail;

	READDATA( m_dwBorderStyle );

	bail:
	
	return hr;
}

#define WRITEDATA( var ) if ( FAILED( hr = pStm->Write( &var, sizeof( var ), NULL ) ) ) goto bail;
HRESULT CControlState::Save( CEditX *pCtrl, LPSTREAM pStm )
{
	HRESULT hr = S_OK;
	DWORD dwVer = 4;
	WRITEDATA( dwVer );
	WRITEDATA( m_bColorSyntax );
	WRITEDATA( m_bAllowHSplit );
	WRITEDATA( m_bAllowVSplit );
	WRITEDATA( m_bHScroll );
	WRITEDATA( m_bVScroll );
	WRITEDATA( m_bSmoothScroll );
	WRITEDATA( m_bLineToolTips );
	WRITEDATA( m_bShowLeftMargin );
	WRITEDATA( m_bAllowColumnSel );
	WRITEDATA( m_bAllowDragDrop );
	WRITEDATA( m_bExpandTabs );
	WRITEDATA( m_xPosHSplitter );
	WRITEDATA( m_yPosVSplitter );
	WRITEDATA( m_eIndentStyle );
	WRITEDATA( m_nTabSize );
	WRITEDATA( m_Colors );
	WRITEDATA( m_lf );

	if ( SUCCEEDED( hr ) )
	{
		CComVariant var( m_bstrText );
		hr = var.WriteToStream( pStm );
	}
	else
		goto bail;

	if ( SUCCEEDED( hr ) )
	{
		CComVariant var( m_bstrLang );
		hr = var.WriteToStream( pStm );
	}
	else
		goto bail;
	
	WRITEDATA( m_bDisplayWhitespace );
	WRITEDATA( m_bWantCarriageReturn );
	WRITEDATA( m_bEnabled );
	WRITEDATA( m_bGlobalProps );
    WRITEDATA( m_bModified );
    WRITEDATA( m_bOverType );
    WRITEDATA( m_bReadOnly );
    WRITEDATA( m_bPreserveCase );
    WRITEDATA( m_bCaseSensitiveSearch );
    WRITEDATA( m_bWholeWordOnly );
    WRITEDATA( m_nMaxUndo );
    WRITEDATA( m_bSelBounds );
	WRITEDATA( m_bRegExp );
	WRITEDATA( m_FontStyles );
	WRITEDATA( m_LineNum );
	WRITEDATA( m_bHideSel );
	WRITEDATA( m_bNormalizeCase );
	WRITEDATA( m_bOvertypeCaret );
	WRITEDATA( m_nHighlightedLine );
	WRITEDATA( m_dwBorderStyle );

	pCtrl->m_bRequiresSave = FALSE;

	bail:
	
	return hr;
}

#endif