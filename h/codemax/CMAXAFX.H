/////////////////////////////////////////////////////////////////////////////////
//
//  CodeMax Code Editor Control
//
//  Copyright © 2000  WinMain Software
//
//  For a complete description of all declarations below, please refer
//  to the CodeMax documentation.
//
/////////////////////////////////////////////////////////////////////////////////
#ifndef __CMAXAFX_H__
#define __CMAXAFX_H__

#ifndef __CODEMAX_H__
#include <codemax.h>
#endif

class CCodeMaxCtrl : public CWnd
{
	public:
	
	virtual BOOL CCodeMaxCtrl::Create( DWORD dwStyle, const CRect& rect, CWnd *pParentWnd, UINT nID )
			{ return CWnd::Create( CODEMAXWNDCLASS, _T(""), dwStyle, rect, pParentWnd, nID, NULL ); }

	CME_CODE SetLanguage( LPCTSTR pszName )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_SetLanguage( m_hWnd, pszName ); }
	CME_CODE GetLanguage( LPTSTR pszName ) const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_GetLanguage( m_hWnd, pszName ); }
	CME_CODE EnableColorSyntax( BOOL bEnable = TRUE )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_EnableColorSyntax( m_hWnd, bEnable ); }
	BOOL IsColorSyntaxEnabled() const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_IsColorSyntaxEnabled( m_hWnd ); }
	CME_CODE OpenFile( LPCTSTR pszFileName )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_OpenFile( m_hWnd, pszFileName ); }
	CME_CODE InsertFile( LPCTSTR pszFileName, const CM_POSITION *pPos = NULL )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_InsertFile( m_hWnd, pszFileName, pPos ); }
	CME_CODE InsertText( LPCTSTR pszText, const CM_POSITION *pPos = NULL )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_InsertText( m_hWnd, pszText, pPos ); }
	CME_CODE SetText( LPCTSTR pszText )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_SetText( m_hWnd, pszText ); }
	CME_CODE SetColors( const CM_COLORS *pColors )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_SetColors( m_hWnd, pColors ); }
	CME_CODE GetColors( CM_COLORS *pColors ) const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_GetColors( m_hWnd, pColors ); }
	CME_CODE EnableWhitespaceDisplay( BOOL bEnable = TRUE )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_EnableWhitespaceDisplay( m_hWnd, bEnable ); }
	BOOL IsWhitespaceDisplayEnabled() const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_IsWhitespaceDisplayEnabled( m_hWnd ); }
	CME_CODE EnableTabExpand( BOOL bEnable = TRUE )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_EnableTabExpand( m_hWnd, bEnable ); }
	BOOL IsTabExpandEnabled() const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_IsTabExpandEnabled( m_hWnd ); }
	CME_CODE EnableSmoothScrolling( BOOL bEnable = TRUE )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_EnableSmoothScrolling( m_hWnd, bEnable ); }
	BOOL IsSmoothScrollingEnabled() const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_IsSmoothScrollingEnabled( m_hWnd ); }
	CME_CODE SetTabSize( int nTabSize )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_SetTabSize( m_hWnd, nTabSize ); }
	int GetTabSize() const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_GetTabSize( m_hWnd ); }
	CME_CODE SetReadOnly( BOOL bReadOnly = TRUE )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_SetReadOnly( m_hWnd, bReadOnly ); }
	BOOL IsReadOnly() const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_IsReadOnly( m_hWnd ); }
	CME_CODE EnableLineToolTips( BOOL bEnable = TRUE )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_EnableLineToolTips( m_hWnd, bEnable ); }
	BOOL IsLineToolTipsEnabled() const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_IsLineToolTipsEnabled( m_hWnd ); }
	CME_CODE EnableLeftMargin( BOOL bEnable = TRUE )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_EnableLeftMargin( m_hWnd, bEnable ); }
	BOOL IsLeftMarginEnabled() const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_IsLeftMarginEnabled( m_hWnd ); }
	CME_CODE SaveFile( LPCTSTR pszFileName, BOOL bClearUndo = TRUE )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_SaveFile( m_hWnd, pszFileName, bClearUndo ); }
	CME_CODE ReplaceText( LPCTSTR pszText, const CM_RANGE *pRange = NULL )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_ReplaceText( m_hWnd, pszText, pRange ); }
	int GetTextLength( const CM_RANGE *pRange = NULL, BOOL bLogical = FALSE ) const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_GetTextLength( m_hWnd, pRange, bLogical ); }
	CME_CODE GetText( LPTSTR pszBuff, const CM_RANGE *pRange = NULL ) const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_GetText( m_hWnd, pszBuff, pRange ); }
	CME_CODE GetText( CString &strText, const CM_RANGE *pRange = NULL ) const
		{ ASSERT( ::IsWindow( m_hWnd ) );
		  CME_CODE nCode = CM_GetText( m_hWnd, strText.GetBufferSetLength( GetTextLength( pRange ) + 1 ), pRange );
		  strText.ReleaseBuffer();
		  return nCode; }
	CME_CODE GetLine( int nLine, LPTSTR pszBuff ) const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_GetLine( m_hWnd, nLine, pszBuff ); }
	CME_CODE GetLine( int nLine, CString &strLine ) const
		{ ASSERT( ::IsWindow( m_hWnd ) );
		  CME_CODE nCode = CM_GetLine( m_hWnd, nLine, strLine.GetBufferSetLength( GetLineLength( nLine ) + 1 ) );
		  strLine.ReleaseBuffer();
		  return nCode; }
	int GetLineCount() const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return (int)CM_GetLineCount( m_hWnd ); }
	int GetLineLength( int nLine, BOOL bLogical = FALSE ) const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return (int)CM_GetLineLength( m_hWnd, nLine, bLogical ); }
	CME_CODE GetWord( LPTSTR pszBuff, CM_POSITION *pPos ) const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_GetWord( m_hWnd, pszBuff, pPos ); }
	CME_CODE GetWord( CString &strWord, CM_POSITION *pPos ) const
		{ ASSERT( ::IsWindow( m_hWnd ) );
		  CME_CODE nCode = CM_GetWord( m_hWnd, strWord.GetBufferSetLength( GetWordLength( pPos ) + 1 ), pPos );
		  strWord.ReleaseBuffer();
		  return nCode; }
	int GetWordLength( CM_POSITION *pPos, BOOL bLogical = FALSE ) const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_GetWordLength( m_hWnd, pPos, bLogical ); }
	CME_CODE AddText( LPCTSTR pszText )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_AddText( m_hWnd, pszText ); }
	CME_CODE DeleteLine( int nLine )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_DeleteLine( m_hWnd, nLine ); }
	CME_CODE InsertLine( int nLine, LPCTSTR pszText )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_InsertLine( m_hWnd, nLine, pszText ); }
	CME_CODE GetSel( CM_RANGE *pRange, BOOL bNormalized = TRUE ) const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_GetSel( m_hWnd, pRange, bNormalized ); }
	CME_CODE SetSel( const CM_RANGE *pRange, BOOL bMakeVisible = TRUE )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_SetSel( m_hWnd, pRange, bMakeVisible ); }
	CME_CODE DeleteSel()
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_DeleteSel( m_hWnd ); }
	CME_CODE ReplaceSel( LPCTSTR pszText )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_ReplaceSel( m_hWnd, pszText ); }
	CME_CODE ExecuteCmd( WORD wCmd, DWORD dwCmdData = 0 )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_ExecuteCmd( m_hWnd, wCmd, dwCmdData ); }
	CME_CODE SetSplitterPos( BOOL bHorz, int nPos )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_SetSplitterPos( m_hWnd, bHorz, nPos ); }
	int GetSplitterPos( BOOL bHorz ) const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_GetSplitterPos( m_hWnd, bHorz ); }
	CME_CODE SetAutoIndentMode( int nMode )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_SetAutoIndentMode( m_hWnd, nMode ); }
	int GetAutoIndentMode() const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_GetAutoIndentMode( m_hWnd ); }
	BOOL CanCopy() const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_CanCopy( m_hWnd ); }
	BOOL CanCut() const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_CanCut( m_hWnd ); }
	BOOL CanPaste() const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_CanPaste( m_hWnd ); }
	CME_CODE Copy() const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_Copy( m_hWnd ); }
	CME_CODE Cut()
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_Cut( m_hWnd ); }
	CME_CODE Paste()
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_Paste( m_hWnd ); }
	BOOL CanUndo() const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_CanUndo( m_hWnd ); }
	BOOL CanRedo() const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_CanRedo( m_hWnd ); }
	CME_CODE Redo()
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_Redo( m_hWnd ); }
	CME_CODE Undo()
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_Undo( m_hWnd ); }
	CME_CODE ClearUndoBuffer()
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_ClearUndoBuffer( m_hWnd ); }
	CME_CODE SetUndoLimit( int nLimit )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_SetUndoLimit( m_hWnd, nLimit ); }
	int GetUndoLimit() const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_GetUndoLimit( m_hWnd ); }
	BOOL IsModified() const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_IsModified( m_hWnd ); }
	CME_CODE SetModified( BOOL bModified = TRUE )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_SetModified( m_hWnd, bModified ); }
	CME_CODE EnableOvertype( BOOL bEnable = TRUE )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_EnableOvertype( m_hWnd, bEnable ); }
	BOOL IsOvertypeEnabled() const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_IsOvertypeEnabled( m_hWnd ); }
	CME_CODE EnableCaseSensitive( BOOL bEnable = TRUE )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_EnableCaseSensitive( m_hWnd, bEnable ); }
	BOOL IsCaseSensitiveEnabled() const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_IsCaseSensitiveEnabled( m_hWnd ); }
	CME_CODE EnablePreserveCase( BOOL bEnable = TRUE )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_EnablePreserveCase( m_hWnd, bEnable ); }
	BOOL IsPreserveCaseEnabled() const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_IsPreserveCaseEnabled( m_hWnd ); }
	CME_CODE EnableWholeWord( BOOL bEnable = TRUE )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_EnableWholeWord( m_hWnd, bEnable ); }
	BOOL IsWholeWordEnabled() const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_IsWholeWordEnabled( m_hWnd ); }
	CME_CODE EnableRegExp( BOOL bEnable = TRUE )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_EnableRegExp( m_hWnd, bEnable ); }
	BOOL IsRegExpEnabled() const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_IsRegExpEnabled( m_hWnd ); }
	CME_CODE SetTopIndex( int nView, int nLine )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_SetTopIndex( m_hWnd, nView, nLine ); }
	int GetTopIndex( int nView ) const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_GetTopIndex( m_hWnd, nView ); }
	int GetVisibleLineCount( int nView, BOOL bFullyVisible = TRUE ) const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_GetVisibleLineCount( m_hWnd, nView, bFullyVisible ); }
	CME_CODE EnableCRLF( BOOL bEnable = TRUE )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_EnableCRLF( m_hWnd, bEnable ); }
	BOOL IsCRLFEnabled() const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_IsCRLFEnabled( m_hWnd ); }
	CME_CODE SetFontOwnership( BOOL bEnable )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_SetFontOwnership( m_hWnd, bEnable ); }
	BOOL GetFontOwnership() const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_GetFontOwnership( m_hWnd ); }
	int GetCurrentView() const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_GetCurrentView( m_hWnd ); }
	CME_CODE SetCurrentView(int nView) const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_SetCurrentView( m_hWnd, nView ); }
	int GetViewCount() const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_GetViewCount( m_hWnd ); }
	CME_CODE ShowScrollBar( BOOL bHorz, BOOL bShow = TRUE )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_ShowScrollBar( m_hWnd, bHorz, bShow ); }
	BOOL HasScrollBar( BOOL bHorz ) const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_HasScrollBar( m_hWnd, bHorz ); }
	CME_CODE GetSelFromPoint( int xClient, int yClient, CM_POSITION *pPos ) const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_GetSelFromPoint( m_hWnd, xClient, yClient, pPos ); }
	CME_CODE SelectLine( int nLine, BOOL bMakeVisible = TRUE )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_SelectLine( m_hWnd, nLine, bMakeVisible ); }
	int HitTest( int xClient, int yClient ) const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_HitTest( m_hWnd, xClient, yClient ); }
	CME_CODE EnableDragDrop( BOOL bEnable = TRUE )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_EnableDragDrop( m_hWnd, bEnable ); }
	BOOL IsDragDropEnabled() const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_IsDragDropEnabled( m_hWnd ); }
	CME_CODE EnableSplitter( BOOL bHorz, BOOL bEnable = TRUE )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_EnableSplitter( m_hWnd, bHorz, bEnable ); }
	BOOL IsSplitterEnabled( BOOL bHorz ) const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_IsSplitterEnabled( m_hWnd, bHorz ); }
	CME_CODE EnableColumnSel( BOOL bEnable = TRUE )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_EnableColumnSel( m_hWnd, bEnable ); }
	BOOL IsColumnSelEnabled() const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_IsColumnSelEnabled( m_hWnd ); }
	CME_CODE EnableGlobalProps( BOOL bEnable = TRUE )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_EnableGlobalProps( m_hWnd, bEnable ); }
	BOOL IsGlobalPropsEnabled() const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return (BOOL)CM_IsGlobalPropsEnabled( m_hWnd ); }
	BOOL IsRecordingMacro() const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return (BOOL)CM_IsRecordingMacro( m_hWnd ); }
	BOOL IsPlayingMacro() const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return (BOOL)CM_IsPlayingMacro( m_hWnd ); }
	CME_CODE SetDlgParent( CWnd *pWnd ) const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_SetDlgParent( m_hWnd, pWnd->m_hWnd ); }
	CME_CODE EnableSelBounds( BOOL bEnable = TRUE )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_EnableSelBounds( m_hWnd, bEnable ); }
	BOOL IsSelBoundsEnabled() const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_IsSelBoundsEnabled( m_hWnd ); }
	CME_CODE SetFontStyles( CM_FONTSTYLES *pFontStyles )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_SetFontStyles( m_hWnd, pFontStyles ); }
	CME_CODE GetFontStyles( CM_FONTSTYLES *pFontStyles ) const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_GetFontStyles( m_hWnd, pFontStyles ); }
	CME_CODE SetItemData( int nLine, LPARAM lParam )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_SetItemData( m_hWnd, nLine, lParam ); }
	LPARAM GetItemData( int nLine ) const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_GetItemData( m_hWnd, nLine ); }
	CME_CODE SetLineStyle( int nLine, DWORD dwStyle )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_SetLineStyle( m_hWnd, nLine, dwStyle ); }
	DWORD GetLineStyle( int nLine ) const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_GetLineStyle( m_hWnd, nLine ); }
	CME_CODE SetBookmark( int nLine, BOOL bEnable = TRUE )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_SetBookmark( m_hWnd, nLine, bEnable ); }
	BOOL GetBookmark( int nLine ) const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_GetBookmark( m_hWnd, nLine ); }
	CME_CODE SetAllBookmarks( int nCount, LPDWORD pdwLines )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_SetAllBookmarks( m_hWnd, nCount, pdwLines ); }
	int GetAllBookmarks( LPDWORD pdwLines ) const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_GetAllBookmarks( m_hWnd, pdwLines ); }
	CME_CODE SetLineNumbering( CM_LINENUMBERING *pNumbering )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_SetLineNumbering( m_hWnd, pNumbering ); }
	CME_CODE GetLineNumbering( CM_LINENUMBERING *pNumbering ) const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_GetLineNumbering( m_hWnd, pNumbering ); }
	CME_CODE PosFromChar( CM_POSITION *pPos, LPRECT pRect ) const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_PosFromChar( m_hWnd, pPos, pRect ); }
	CME_CODE EnableHideSel( BOOL bEnable = TRUE )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_EnableHideSel( m_hWnd, bEnable ); }
	BOOL IsHideSelEnabled() const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_IsHideSelEnabled( m_hWnd ); }
	CME_CODE SetHighlightedLine( int nLine )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_SetHighlightedLine( m_hWnd, nLine ); }
	int GetHighlightedLine() const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_GetHighlightedLine( m_hWnd ); }
	CME_CODE EnableNormalizeCase( BOOL bEnable = TRUE )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_EnableNormalizeCase( m_hWnd, bEnable ); }
	BOOL IsNormalizeCaseEnabled() const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_IsNormalizeCaseEnabled( m_hWnd ); }
	CME_CODE SetDivider( int nLine, BOOL bEnable = TRUE )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_SetDivider( m_hWnd, nLine, bEnable ); }
	BOOL GetDivider( int nLine ) const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_GetDivider( m_hWnd, nLine ); }
	CME_CODE EnableOvertypeCaret( BOOL bEnable = TRUE )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_EnableOvertypeCaret( m_hWnd, bEnable ); }
	BOOL IsOvertypeCaretEnabled() const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_IsOvertypeCaretEnabled( m_hWnd ); }
	CME_CODE SetFindText( LPCTSTR pszText )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_SetFindText( m_hWnd, pszText ); }
	CME_CODE GetFindText( LPTSTR pszText ) const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_GetFindText( m_hWnd, pszText ); }
	CME_CODE GetFindText( CString &strText ) const
		{ ASSERT( ::IsWindow( m_hWnd ) );
		  CME_CODE nCode = CM_GetFindText( m_hWnd, strText.GetBufferSetLength( CM_MAX_FINDREPL_TEXT ) );
		  strText.ReleaseBuffer();
		  return nCode; }
	CME_CODE SetReplaceText( LPCTSTR pszText )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_SetReplaceText( m_hWnd, pszText ); }
	CME_CODE GetReplaceText( LPTSTR pszText ) const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_GetReplaceText( m_hWnd, pszText ); }
	CME_CODE GetReplaceText( CString &strText ) const
		{ ASSERT( ::IsWindow( m_hWnd ) );
		  CME_CODE nCode = CM_GetReplaceText( m_hWnd, strText.GetBufferSetLength( CM_MAX_FINDREPL_TEXT ) );
		  strText.ReleaseBuffer();
		  return nCode; }
	CME_CODE SetImageList( HIMAGELIST hImageList )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_SetImageList( m_hWnd, hImageList ); }
	HIMAGELIST GetImageList() const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_GetImageList( m_hWnd ); }
	CME_CODE SetMarginImages( int nLine, BYTE byImages )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_SetMarginImages( m_hWnd, nLine, byImages ); }
	BYTE GetMarginImages( int nLine ) const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_GetMarginImages( m_hWnd, nLine ); }
	CME_CODE AboutBox() const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_AboutBox( m_hWnd ); }
	CME_CODE Print( HDC hDC, DWORD dwFlags ) const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_Print( m_hWnd, hDC, dwFlags ); }
	CME_CODE SetCaretPos( int nLine, int nCol )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_SetCaretPos( m_hWnd, nLine, nCol ); }
	int ViewColToBufferCol( int nLine, int nViewCol ) const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_ViewColToBufferCol( m_hWnd, nLine, nViewCol ); }
	int BufferColToViewCol( int nLine, int nBufferCol ) const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_BufferColToViewCol( m_hWnd, nLine, nBufferCol ); }
	CME_CODE SetBorderStyle( DWORD dwStyle )
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_SetBorderStyle( m_hWnd, dwStyle ); }
	DWORD GetBorderStyle() const
		{ ASSERT( ::IsWindow( m_hWnd ) ); return CM_GetBorderStyle( m_hWnd ); }
	}; 
#endif