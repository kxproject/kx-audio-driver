#include "precomp.h"
#include "editview.h"
#include "cedit.h"

extern HINSTANCE hInstance;
BOOL CEditView::m_bInit = FALSE;
TCHAR CEditView::m_szSpace[ CBuffer::MAX_TABSIZE + 1 ];

CEditView::CEditView( CEdit *pCtrl, CBuffer *pBuffer, int nLeftIndex, int nTopIndex )
{
	m_pCtrl = pCtrl;
	m_hWndView = pCtrl->GetWindow();
	m_pBuffer = pBuffer;
	SetRectEmpty( &m_rcAll );
	SetRectEmpty( &m_rcView );
	m_hWndHScroll = m_hWndVScroll = NULL;
	m_nBottomIndex = m_nRightIndex = m_nBottomIndexFullyVisible = m_nRightIndexFullyVisible = 0;
	m_nLeftIndex = nLeftIndex;
	m_nTopIndex = nTopIndex;
	m_nLineDamageStart = MAXROW;
	m_nLineDamageEnd = -1;
	m_nEditLevel = 0;
	if ( !m_bInit )
	{
		CharFill( m_szSpace, _T(' '), CBuffer::MAX_TABSIZE );
		m_bInit = TRUE;
	}
	OnFontChanged( pCtrl->GetFont( FALSE ), pCtrl->GetCharWidth() );
}

CEditView::~CEditView()
{
	if ( ::IsWindow( m_hWndHScroll ) )
	{
		::DestroyWindow( m_hWndHScroll );
		m_hWndHScroll = NULL;
	}
	if ( ::IsWindow( m_hWndVScroll ) )
	{
		::DestroyWindow( m_hWndVScroll );
		m_hWndVScroll = NULL;
	}
}

void CEditView::SetPosition( LPRECT prc, BOOL bReserveHSplitter, BOOL bReserveVSplitter )
{
	ASSERT( prc != NULL );

	m_rcAll = *prc;
	RecalcLayout( bReserveHSplitter, bReserveVSplitter );

	RecalcIndexes();
	RecalcScrollInfo();
}

void CEditView::RecalcIndexes()
{
	// recalc the new bottom index and new right index
	int cxView = ( m_rcView.right - m_rcView.left ) - GetLeftMargin( TRUE, TRUE );
	int cyView = ( m_rcView.bottom - m_rcView.top );
	m_nBottomIndex = m_nBottomIndexFullyVisible = m_nTopIndex + ( cyView / m_cyLine );
	m_nRightIndex = m_nRightIndexFullyVisible = m_nLeftIndex + ( cxView / m_cxChar );
	m_nRightIndexFullyVisible--;
	m_nBottomIndexFullyVisible--;
}

void CEditView::DrawVDividerLine( HDC hDC, const RECT &rcView, int xPos ) const
{
	HPEN hPen = CreatePen( PS_SOLID, CX_DIVIDERLINE, m_pCtrl->GetVDividerLineColor() );
	HPEN hPenOld = ( HPEN ) SelectObject( hDC, hPen );
	MoveToEx( hDC, xPos, rcView.top, NULL );
	LineTo( hDC, xPos, rcView.bottom );
	SelectObject( hDC, hPenOld );
	DeleteObject( hPen );
}

#pragma warning(disable:4701)
void CEditView::Draw( HDC hDC, LPRECT prcClipBox, CSelection *pSel ) const
{
	// prcClipBox is control-relative, not view-relative
	int nRowStart = ( max( m_rcView.top, prcClipBox->top ) - m_rcView.top ) / m_cyLine;
	int ySnap = m_rcView.top + nRowStart * m_cyLine;
	nRowStart += m_nTopIndex;
	int nRowEnd = m_nTopIndex + ( min( prcClipBox->bottom, m_rcView.bottom ) - m_rcView.top ) / m_cyLine;

//#define _DRAWCHARGRID
#ifdef _DRAWCHARGRID
	{
	RECT rc;
	if ( IntersectRect( &rc, prcClipBox, &m_rcView ) )
	{
		rc.left = GetLeftMargin( TRUE, TRUE );
		HPEN hPen = CreatePen( PS_SOLID, 0, RGB( 192, 192, 192 ) );
		HPEN hPenOld = ( HPEN ) SelectObject( hDC, hPen );
		int xStart = m_rcView.left + GetLeftMargin( TRUE, TRUE ) +
					 ( ( prcClipBox->left - m_rcView.left ) / m_cxChar ) * m_cxChar;
		int xEnd = min( prcClipBox->right, m_rcView.right );
		int yStart = ySnap;
		int yEnd = min( prcClipBox->bottom, m_rcView.bottom );
		for ( int y = yStart; y <= yEnd; y += m_cyLine )
		{
			MoveToEx( hDC, rc.left, y, NULL );
			LineTo( hDC, rc.right, y );
		}
		for ( int x = xStart; x <= xEnd; x += m_cxChar )
		{
			MoveToEx( hDC, x, rc.top, NULL );
			LineTo( hDC, x, rc.bottom );
		}
		SelectObject( hDC, hPenOld );
		DeleteObject( hPen );
	}
	}
#endif

	int nLastLine = m_pBuffer->GetLineCount() - 1;
	nLastLine = min( nLastLine, nRowEnd );
	int nSelStartCol, nSelStartRow, nSelEndCol, nSelEndRow;
	BOOL bColumnSel = FALSE;
	int cxCaret = 0;
	if ( pSel )
	{
		pSel->GetNormalizedViewSelection( nSelStartCol, nSelStartRow, nSelEndCol, nSelEndRow );
		bColumnSel = pSel->IsColumnSel();
		cxCaret = pSel->GetCaretWidth();
	}

	int cxLeftMargin1 = GetLeftMargin( FALSE );
	COLORREF crVDividerLines = m_pCtrl->GetVDividerLineColor();

	// draw the left margin in a different color
	if ( cxLeftMargin1 )
	{
		COLORREF crMargin = m_pCtrl->GetLeftMarginColor();
		if ( crMargin != CLR_INVALID )
		{
			RECT rc = m_rcView;
			rc.right = rc.left + cxLeftMargin1;
			SetBkColor( hDC, crMargin );
			ExtTextOut( hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL );
		}
	}

	// draw the line numbers in a different color
	int cxLeftMargin2 = GetLeftMargin( TRUE );
	if ( cxLeftMargin2 != cxLeftMargin1 )
	{
		COLORREF crLineNumbers = m_pCtrl->GetLineNumberBackColor();
		if ( crLineNumbers != CLR_INVALID )
		{
			RECT rc = m_rcView;
			rc.left = m_rcView.left + cxLeftMargin1;
			rc.right = m_rcView.left + cxLeftMargin2;
			SetBkColor( hDC, crLineNumbers );
			ExtTextOut( hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL );
		}
	}

	// draw the divider lines
	if ( crVDividerLines != CLR_INVALID )
	{
		if ( cxLeftMargin1 )
			DrawVDividerLine( hDC, m_rcView, m_rcView.left + cxLeftMargin1 );
		if ( cxLeftMargin1 != cxLeftMargin2 )
			DrawVDividerLine( hDC, m_rcView, m_rcView.left + cxLeftMargin2 );
	}

	int cxLeftMargin3 = GetLeftMargin( TRUE, TRUE );

	// draw the stuff in the left margin in one pass, rather than with each
	// line.  This is much faster.
	int y = ySnap;
	int xLineNumber = m_rcView.left + cxLeftMargin1;
	int nLineNumRadix;
	BOOL bLineNumbers;

	// if line numbering, select the font once, or else things slow down a bit
	// if done for each line	
	if ( ( bLineNumbers = m_pCtrl->LineNumberingEnabled( nLineNumRadix ) ) == TRUE )
	{
		SetTextCharacterExtra( hDC, 0 );
		SelectObject( hDC, m_pCtrl->GetLineNumberFont() );
		SetTextColor( hDC, m_pCtrl->GetLineNumberForeColor() );
		::SetBkMode( hDC, TRANSPARENT );
	}

	int cyDescentShiftNormal = m_pCtrl->m_font->cyDescentShift;
    int nLine;
	for ( nLine = nRowStart; nLine <= nLastLine; nLine++ )
	{
		// draw the bookmark and images if present
		if ( cxLeftMargin1 > 0 )
		{
			BOOL bHasBookmark = m_pBuffer->HasBookmark( nLine );
			BYTE byImages = m_pBuffer->GetMarginImages( nLine );
			if ( bHasBookmark || byImages )
			{
				DrawLeftMarginImages( hDC, bHasBookmark, byImages, m_rcView.left, y );
			}
		}
		// Draw the line number if asked to
		if ( bLineNumbers )
		{
			DrawLineNumber( nLine, nLineNumRadix, hDC, xLineNumber, y - cyDescentShiftNormal );
		}
		y += m_cyLine;
	}

	int x = m_rcView.left + cxLeftMargin3;
	int xDividerStart = m_rcView.left + cxLeftMargin2;

	// draw the text
	for ( nLine = nRowStart, y = ySnap; nLine <= nLastLine; nLine++, y += m_cyLine )
		DrawLine( nLine, hDC, x, y, xDividerStart );

	// draw the selection as a separate pass, because the text will overlap slightly
	// (for tighter display), and the invert code would end up double-inverting the
	// overlapping regions, resulting in normal display.
	if ( pSel )
	{
		for ( nLine = nRowStart, y = ySnap; nLine <= nLastLine; nLine++, y += m_cyLine )
		{
			if ( ( nLine >= nSelStartRow ) && ( nLine <= nSelEndRow ) )
			{
				RECT rcInvert;
				rcInvert.top = y;
				rcInvert.bottom = y + m_cyLine;
				if ( bColumnSel )
				{
					int xSel = nSelStartCol - m_nLeftIndex;
					xSel = max( 0, xSel );
					rcInvert.left = m_rcView.left + xSel * m_cxChar + cxLeftMargin3;
					xSel = nSelEndCol - m_nLeftIndex;
					xSel = max( 0, xSel );
					rcInvert.right = m_rcView.left + xSel * m_cxChar + cxLeftMargin3;
					if ( rcInvert.left == rcInvert.right )
					{
						// Is a column selection and is 'empty' -- make selection appear as a
						// giant insertion cursor
						rcInvert.right = rcInvert.left + cxCaret;
					}
				}
				else
				{
					int xSel = ( nLine == nSelStartRow ) ? nSelStartCol - m_nLeftIndex : 0;
					xSel = max( 0, xSel );
					rcInvert.left = m_rcView.left + xSel * m_cxChar + cxLeftMargin3;
					rcInvert.left = max( rcInvert.left, x );
					xSel = ( nLine == nSelEndRow ) ? ( nSelEndCol - m_nLeftIndex ) : ( m_nRightIndex - m_nLeftIndex + 1 );
					rcInvert.right = m_rcView.left + xSel * m_cxChar + cxLeftMargin3;
					rcInvert.right = max( rcInvert.right, x );
				}

				int xMaxLeft = m_rcView.left + cxLeftMargin2;
				rcInvert.left = max( rcInvert.left, xMaxLeft );
				rcInvert.right = max( rcInvert.right, xMaxLeft );
				InvertRect( hDC, &rcInvert );
			}
		}
	}

	if ( m_pCtrl->ShowHScrollBar() && m_pCtrl->ShowVScrollBar() )
	{
		// Draw the size box
		HBRUSH hbr = CreateSolidBrush( GetSysColor( COLOR_BTNFACE ) );
		RECT rcBox = m_rcAll;
		rcBox.left = rcBox.right - GetSystemMetrics( SM_CXVSCROLL );
		rcBox.top = rcBox.bottom - GetSystemMetrics( SM_CYHSCROLL );
		FillRect( hDC, &rcBox, hbr );
		DeleteObject( hbr );

	}
}

void CEditView::DrawLeftMarginImages( HDC hDC, BOOL bBookmark, BYTE byImages, int x, int y ) const
{
	HIMAGELIST hil = m_pCtrl->GetImageList();
	if ( hil )
	{
		int cx, cy;
		if ( !ImageList_GetIconSize( hil, &cx, &cy ) )
			return;
		x = x + ( ( CM_CXLEFTMARGIN - cx ) / 2 );
		y = y + ( ( m_cyLine - cy ) / 2 );

		// draw the bookmark if enabled
		if ( bBookmark )
		{
			ImageList_Draw( hil, 0, hDC, x, y, ILD_TRANSPARENT );
		}

		// draw all custom margin images enabled by the caller
		if ( byImages )
		{
			for ( int i = 1; byImages && i <= 7; i++ )
			{
				BYTE byMask = ( BYTE ) ( 1 << i );
				if ( byImages & byMask )
				{
					byImages &= ~byMask;	// remove so we may shortcircuit from loop if done
					ImageList_Draw( hil, i, hDC, x, y, ILD_TRANSPARENT );
				}
			}
		}
	}
	else if ( bBookmark )
	{
		// no imagelist, so let's just draw the bookmark by hand as usual
		HBRUSH hbr = CreateSolidBrush( m_pCtrl->GetBookmarkColor() );
		HPEN hpen = CreatePen( PS_SOLID, 1, m_pCtrl->GetBookmarkBkColor() );
		HBRUSH hbrOld = ( HBRUSH ) SelectObject( hDC, hbr );
		HPEN hpenOld = ( HPEN ) SelectObject( hDC, hpen );

		POINT pt[ 3 ];
		pt[ 0 ].x = x + CM_CXLEFTMARGIN - 4;
		pt[ 0 ].y = y + m_cyLine / 2;
		pt[ 1 ].x = x + CM_CXLEFTMARGIN / 4;
		pt[ 1 ].y = pt[ 0 ].y - 6;
		pt[ 2 ].x = pt[ 1 ].x;
		pt[ 2 ].y = pt[ 0 ].y + 6;

		Polygon( hDC, pt, ARRAY_SIZE( pt ) );

		SelectObject( hDC, hbrOld );
		DeleteObject( hbr );
		SelectObject( hDC, hpenOld );
		DeleteObject( hpen );
	}
}

void CEditView::DrawLineNumber( int nLine, int nRadix, HDC hDC, int x, int y ) const
{
	static TCHAR g_szDigits[] = _T("0123456789ABCDEF");
	TCHAR szNum[ 30 ];
	nLine += m_pCtrl->GetLineNumberStart();
	int nLog = _log( nLine, nRadix );

	int nLineNumWidth = m_pCtrl->GetLineNumWidth();
	int nStartAt = nLineNumWidth - nLog;
	CharFill( szNum, _T('0'),  nStartAt );
	_itot( nLine, szNum + nStartAt, nRadix );
	::ExtTextOut( hDC, x + ( LINEWIDTH_EXTRA * m_cxChar ) / 2, y, 0, NULL, szNum, nLineNumWidth, NULL );
}

void CEditView::DrawLine( int nLine, HDC hDC, int x, int y, int xDividerStart ) const
{
	int nLineLen = m_pBuffer->GetLineLength( nLine );
	BOOL bUseColor = ( m_pCtrl->UseColorSyntax() && ( nLineLen <= MAXCOL ) );

	int nViewCol = m_nLeftIndex;
	int nBuffColStart = m_pBuffer->ConvertViewColToBufferCol( nLine, nViewCol, FALSE );

	int xLeft = x;

	// highlight this line if appropriate
	BOOL bHighlight = m_pBuffer->IsHighlighted( nLine );
	if ( bHighlight )
	{
		// fill out the rest of the line with the highlight
		RECT rc = m_rcView;
		rc.left = x;
		rc.top = y;
		rc.bottom = y + m_cyLine;
		SetBkColor( hDC, m_pCtrl->GetHighlightedLineColor() );
		ExtTextOut( hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL );
	}

	BOOL bMBCS = DBCS_ACTIVE;

#ifdef _UNICODE
	int nRightIndex = m_nRightIndex;
#else
	int nRightIndex = m_nRightIndex + bMBCS * 2;	// always paint two more TCHARs so multibyte chars aren't truncated
#endif

	if ( nBuffColStart < nLineLen )	// else nothing to draw
	{
		// wait for nLine to become available (in case of background syntax parsing)
		m_pBuffer->WaitForLine( nLine );

		LPCTSTR pszLineStart = m_pBuffer->GetLineText( nLine );
		register LPCTSTR psz = pszLineStart + nBuffColStart;
		LPCTSTR pszStart = psz;
		
		//////////////////////////////////////////////////////////////////////////
		// Step 1: Expand tabs
		//
		BOOL bViewWhitespace = m_pCtrl->DisplayWhitespace();
		TCHAR szLine[ 1000 ];  CharFill( szLine, _T(' '), ARRAY_SIZE( szLine ) );
		int cbTab = m_pBuffer->GetTabSize();
		LPTSTR pszBuff = szLine;
		TCHAR chTab, chSpace;
		if ( bMBCS )
			{ chTab = _T('^'); chSpace = _T('`'); }
		else
			{ chTab = _T('»'); chSpace = _T('·'); }

		while ( *psz && ( nViewCol <= nRightIndex ) )
		{
			if ( *psz == _T('\t') )
			{
				int nSpaces = ( ( nViewCol / cbTab ) * cbTab ) + cbTab - nViewCol;
				if ( nSpaces )
				{
					if ( bViewWhitespace )
					{
						if ( psz != pszStart || m_nLeftIndex == m_pBuffer->ConvertBufferColToViewCol( nLine, nBuffColStart ) )
						{
							*pszBuff = chTab;
						}
					}
					pszBuff += nSpaces;
					nViewCol += nSpaces;
				}
			}
			else
			{
				if ( bViewWhitespace && *psz == _T(' ') )
				{
					*pszBuff++ = chSpace;
				}
				else
				{
					*pszBuff++ = *psz;
				}
				nViewCol++;
			}
			psz++;
		}
		//////////////////////////////////////////////////////////////////////////
		// Step 2: Parse the line and assign colors to everything
		//
		DWORD clrLine[ 100 ]; // hiword = token, loword = token offset in szLine
		int nColors = 0;

		if ( bUseColor )
		{
			psz = pszLineStart;
			int nParseLen = m_pBuffer->ConvertViewColToBufferCol( nLine, nRightIndex ) + 20; // +20 is a reasonable max token length
			nParseLen = min( nParseLen, nLineLen );
			nViewCol = 0;
			int nBuffCol = 0;
			int nFirstColorViewCol = m_nLeftIndex;
			BOOL bColorIsVisible = nViewCol >= nFirstColorViewCol;

			CLineParser Parser( m_pBuffer, nLine, nParseLen );

			// pick up where the last line left off -- in a multi-line comment block (or not)
			clrLine[ nColors++ ] = MAKELPARAM( 0, ( WORD ) ( Parser.m_bInComment ? CBuffer::eMultiLineCommentStart : 
			                                               ( Parser.m_bInString ?  CBuffer::eStringDelim :
			                                               ( Parser.m_bInTag ?     CBuffer::eTagText :
														                           CBuffer::eText ) ) ) );

			while ( Parser.MoreComing() )
			{
				Parser.AcceptToken();

				int nTokenStartCol = nViewCol;
				nBuffCol += Parser.m_nTokenLen;
				if ( Parser.m_bHasTab )
				{
					ASSERT( Parser.m_eToken == CBuffer::eText || Parser.m_eToken == CBuffer::eTagText || Parser.m_eToken == CBuffer::eStringDelim || Parser.m_eToken == CBuffer::eSingleLineComment );
					nViewCol = m_pBuffer->ConvertBufferColToViewCol( nLine, nBuffCol );
				}
				else
				{
					nViewCol += Parser.m_nTokenLen;
				}
				if ( !bColorIsVisible )
				{
					// Assume this token (unless in a comment) will be the first token to cross the left edge 
					clrLine[ 0 ] = MAKELPARAM( 0, ( WORD )( Parser.m_bInComment ? CBuffer::eMultiLineCommentStart : 
																		 ( Parser.m_bInString ? CBuffer::eStringDelim : Parser.m_eToken ) ) );

					if ( nViewCol > nFirstColorViewCol )
					{
						//////////////////////////
						// token is now in view
						//
						bColorIsVisible = TRUE;
						nTokenStartCol = nFirstColorViewCol;
					}
				}

				// record the token position
				if ( nTokenStartCol > nRightIndex )
				{
					break;
				}

				if ( bColorIsVisible &&
				     !Parser.m_bWasInComment && !Parser.m_bIsCommentEndToken &&
					 !Parser.m_bWasInString && !Parser.m_bIsStringEndToken )
				{
					clrLine[ nColors++ ] = MAKELPARAM( nTokenStartCol - m_nLeftIndex, ( WORD ) Parser.m_eToken );
				}

				clrLine[ nColors ] = MAKELPARAM( ( nViewCol > m_nLeftIndex ? nViewCol : nRightIndex ) - m_nLeftIndex, ( WORD ) Parser.m_eToken );

				// don't blow past the local array and corrupt the stack!
				if ( nColors >= ARRAY_SIZE( clrLine ) - 1 )
				{
					nColors = 0;
					goto no_color;
				}

			}
		}
		else
		{
			no_color:

			clrLine[ nColors++ ] = MAKELPARAM( 0, ( WORD ) CBuffer::eText );
			// draw text only as far as is necessary.  We don't want to paint extra characters that aren't
			// in the buffer.  If underline font used on plain text, the underline will extend to the edge of the window.
			// we don't want that.
			int nViewColEnd = m_pBuffer->ConvertBufferColToViewCol( nLine, nLineLen );
			nViewColEnd = min( nRightIndex, nViewColEnd );
			nViewColEnd = max( m_nLeftIndex, nViewColEnd );

			clrLine[ nColors ] = MAKELPARAM( nViewColEnd - m_nLeftIndex, ( WORD ) CBuffer::eText );
		}

		//////////////////////////////////////////////////////////////////////////
		// Step 3: Output the line
		//
		ASSERT( nColors );
		BOOL bFirstToken = TRUE;

		for ( int i = 0; i < nColors; i++ )
		{
			DWORD dwColorInfo = clrLine[ i ];
			CBuffer::LangToken eToken = ( CBuffer::LangToken ) HIWORD( dwColorInfo );
			SetTextColor( hDC, m_pCtrl->GetTokenColor( eToken, TRUE ) );
			COLORREF crBk = bHighlight ? m_pCtrl->GetHighlightedLineColor() : m_pCtrl->GetTokenColor( eToken, FALSE );
			if ( crBk != CLR_INVALID )
			{
				SetBkColor( hDC, crBk );
				SetBkMode( hDC, OPAQUE );
			}
			else
			{
				SetBkMode( hDC, TRANSPARENT );
			}

			long cxExtraSpacing, cyDescentShift;
			SelectObject( hDC, m_pCtrl->GetTokenFont( eToken, cxExtraSpacing, cyDescentShift, m_pCtrl->m_font ) );
			SetTextCharacterExtra( hDC, cxExtraSpacing );

			int nTokenStart = LOWORD( dwColorInfo );
			int nTokenNext = LOWORD( clrLine[ i + 1 ] );
			int cbToken = nTokenNext - nTokenStart;
			if ( cbToken )
			{
				#ifndef _UNICODE
				// The first visible token on the left of the line might be cutoff right
				// in the middle of a multi-byte char.  We need to account for this by not
				// rendering the character (just leave whitespace).
				if ( bFirstToken && _ismbstrail( ( const unsigned char * ) pszLineStart, ( const unsigned char * ) pszStart ) )
				{
					// scan backwards to the lead byte
					LPCTSTR pszLead = pszStart;
					while ( _ismbstrail( ( const unsigned char * ) pszLineStart, ( const unsigned char * ) --pszLead ) )
						;
					int cbChar = pszStart - pszLead;
					nTokenStart += cbChar;
					cbToken -= cbChar;
					x += ( cbChar * m_cxChar );
				}
				#endif
				bFirstToken = FALSE;
				ExtTextOut( hDC, x, y - cyDescentShift, 0, NULL, szLine + nTokenStart, cbToken, NULL );
				x += ( cbToken * m_cxChar );
			}
			// don't worry about deselecting the font -- it will be cleaned up by any
			// calling method (as an optimization)
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// Step 4: give the parent window a crack at painting, too.
	//
	DWORD dwStyle = m_pBuffer->GetLineStyle( nLine );
	if ( HAS_FLAG( dwStyle, CML_OWNERDRAW ) )
	{
		CM_DRAWLINEDATA dld;
		dld.hDC = hDC;
		dld.rcLine.left = xLeft;
		dld.rcLine.top = y;
		dld.rcLine.right = m_rcView.right;
		dld.rcLine.bottom = y + m_cyLine;
		dld.nLine = nLine;
		dld.nLeftCol = m_nLeftIndex; 
		dld.nRightCol = m_nRightIndex; 
		dld.lParam = m_pBuffer->GetItemData( nLine );
		dld.dwStyle = dwStyle;
		m_pCtrl->NotifyParent( CMN_DRAWLINE, ( NMHDR * ) &dld );
	}

	// Draw divider line underneath the line if appropriate
	if ( m_pBuffer->HasDivider( nLine ) )
	{
		COLORREF crDividerLine = m_pCtrl->GetHDividerLineColor();
		COLORREF crWindow = m_pCtrl->GetWindowColor( TRUE );
		// if line will blend in with the background, make it visible (opposite of window color).
		if ( crDividerLine == CLR_INVALID || crDividerLine == crWindow )
			crDividerLine = ( ~crWindow & 0x00ffffff );

		HPEN hPen = CreatePen( PS_SOLID, CY_DIVIDERLINE, crDividerLine );
		HPEN hPenOld = ( HPEN ) SelectObject( hDC, hPen );
		int yLine = y + m_cyLine - 1;
		MoveToEx( hDC, xDividerStart, yLine, NULL );
		LineTo( hDC, m_rcView.right, yLine );
		SelectObject( hDC, hPenOld );
		DeleteObject( hPen );
	}
}


BOOL CEditView::OnHScroll( HWND hWndScrollBar, UINT nSBCode, int nPos )
{
	UNUSED( nSBCode );
	UNUSED( nPos );

	if ( m_hWndHScroll == hWndScrollBar )
	{
		SCROLLINFO si;
		si.cbSize = sizeof( SCROLLINFO );
		si.fMask = SIF_POS;
		SendMessage( hWndScrollBar, SBM_GETSCROLLINFO, TRUE, ( LPARAM )&si );
		int nOldPos = si.nPos;
		int nTemp;
		switch ( nSBCode )
		{
			case SB_LINELEFT:
			case SB_LEFT:
				nTemp = nOldPos - 2;
				nPos = max( 0, nTemp );
				break;
			case SB_LINERIGHT:
			case SB_RIGHT:
				nPos = nOldPos + 2;
				break;
			case SB_PAGELEFT:
				nTemp = nOldPos - 60;
				nPos = max( 0, nTemp );
				break;
			case SB_PAGERIGHT:
				nPos = nOldPos + 60;
				break;
			case SB_THUMBTRACK:
				break;
			default:
			case SB_THUMBPOSITION:
				break;
			case SB_ENDSCROLL:
				nPos = nOldPos;
				// some ActiveX containers cause the caret to go away -- put it back
				m_pCtrl->FlashCaret();
				break;
		}
		
		if ( nPos != nOldPos )
		{
			SCROLLINFO si;
			si.cbSize = sizeof( SCROLLINFO );
			si.fMask = SIF_POS;
			si.nPos = nPos;
			SendMessage( hWndScrollBar, SBM_SETSCROLLINFO, TRUE, ( LPARAM )&si );
			ScrollTo( nPos, m_nTopIndex );
		}
		
		return TRUE;
	}

	return FALSE;
}

BOOL CEditView::OnVScroll( HWND hWndScrollBar, UINT nSBCode, int nPos )
{
	if ( m_hWndVScroll == hWndScrollBar )
	{
		BOOL bSmoothScroll = FALSE;
		BOOL bUpdateToolTip = FALSE;
		BOOL bScroll = TRUE;
		SCROLLINFO si;
		si.cbSize = sizeof( SCROLLINFO );
		si.fMask = SIF_POS;
		SendMessage( hWndScrollBar, SBM_GETSCROLLINFO, TRUE, ( LPARAM )&si );
		int nOldPos = si.nPos;

		switch ( nSBCode )
		{
			case SB_TOP:
				bUpdateToolTip = TRUE; // fall through
			case SB_LINEUP:
			{
				int nTemp = nOldPos - 1;
				nPos = max( 0, nTemp );
				bSmoothScroll = TRUE;
				break;
			}
			case SB_BOTTOM:
				bUpdateToolTip = TRUE; // fall through
			case SB_LINEDOWN:
				nPos = nOldPos + 1;
				bSmoothScroll = TRUE;
				break;
			case SB_PAGEUP:
			{
				int nTemp = nOldPos - ( m_nBottomIndex - m_nTopIndex );
				nPos = max( 0, nTemp );
				break;
			}
			case SB_PAGEDOWN:
				nPos = nOldPos + ( m_nBottomIndex - m_nTopIndex );
				break;
			case SB_THUMBTRACK:
			{
				bUpdateToolTip = TRUE; // fall through
				break;
			}
			default:
			case SB_THUMBPOSITION:
				break;
			case SB_ENDSCROLL:
				EndLineToolTip();
				bScroll = FALSE;
				// some ActiveX containers cause the caret to go away -- put it back
				m_pCtrl->FlashCaret();
				break;
		}

		// special case: even if equal, let nPos == 0 case thru because I hacked the
		// scrollbars in ::RecalcScrollBars() to make the thumb occupy the entire space.
		if ( bScroll && 
		     ( !nPos || nPos != nOldPos ) )
		{
			int nMaxLine = m_pBuffer->GetLineCount() - 1;
			nMaxLine = max( nMaxLine, 0 );
			nPos = min( nMaxLine, nPos );
			SCROLLINFO si;
			si.cbSize = sizeof( SCROLLINFO );
			si.fMask = SIF_POS;
			si.nPos = nPos;
			SendMessage( hWndScrollBar, SBM_SETSCROLLINFO, TRUE, ( LPARAM )&si );
			ScrollTo( m_nLeftIndex, nPos, bSmoothScroll );
		}

		if ( m_pCtrl->ShowLineToolTips() && bUpdateToolTip )
		{
			StartLineToolTip( this );
		}

		return TRUE;
	}

	return FALSE;
}

void CEditView::ScrollTo( int nCol, int nRow, BOOL bSmooth )
{
	if ( nCol != m_nLeftIndex )
	{
		int cxShift = ( m_nLeftIndex - nCol ) * m_cxChar;
		SetRightIndex( nCol + m_nRightIndex - m_nLeftIndex );
		m_nLeftIndex = nCol;

		if ( !m_pCtrl->DelayPaint() )
		{
			RECT rcClip = m_rcView;
			rcClip.left += GetLeftMargin( TRUE, TRUE );
			if ( bSmooth && m_pCtrl->SmoothScrolling() )
			{
				int nSign = ( cxShift < 0 ) ? -1 : +1;
				int nScrollBy = cxShift / 6;
				if ( !nScrollBy )
				{
					// small font used!
					nScrollBy = 1;
				}

				int cx = 0;
				do
				{
					if ( nSign * ( nScrollBy + cx ) > nSign * cxShift )
					{
						nScrollBy = cxShift - cx;
					}
					cx += nScrollBy;
					RECT rcInval;
					VERIFY( ScrollWindowEx( m_hWndView, nScrollBy, 0, NULL, &rcClip, NULL, &rcInval, 0 ) );
					// factor in a bit of extra space in case a DBCS char is only half-painted
					InflateRect( &rcInval, m_cxChar, 0 );
					InvalidateRect( m_hWndView, &rcInval, FALSE );
				} while ( nSign * cx < nSign * cxShift );

				ASSERT( cx == cxShift );
			}
			else
			{
				RECT rcInval;
				VERIFY( ScrollWindowEx( m_hWndView, cxShift, 0, NULL, &rcClip, NULL, &rcInval, 0 ) );
				// factor in a bit of extra space in case a DBCS char is only half-painted
				InflateRect( &rcInval, m_cxChar, 0 );
				InvalidateRect( m_hWndView, &rcInval, FALSE );
			}
			m_pCtrl->SafeUpdateWindow();
		}
		else
		{
			m_pCtrl->Repaint( FALSE );
		}

		if ( m_hWndHScroll )
		{
			SCROLLINFO si;
			si.cbSize = sizeof( SCROLLINFO );
			si.fMask = SIF_POS;
			si.nPos = m_nLeftIndex;
			SendMessage( m_hWndHScroll, SBM_SETSCROLLINFO, TRUE, ( LPARAM )&si );
		}
	}

	if ( nRow != m_nTopIndex )
	{
		int cyShift = ( m_nTopIndex - nRow ) * m_cyLine;
		SetBottomIndex( nRow + m_nBottomIndex - m_nTopIndex );
		m_nTopIndex = nRow;
		if ( !m_pCtrl->DelayPaint() )
		{
			if ( bSmooth && m_pCtrl->SmoothScrolling() )
			{
				int nSign = ( cyShift < 0 ) ? -1 : +1;
				int nScrollBy = cyShift / 6;
				if ( !nScrollBy )
				{
					// small font used!
					nScrollBy = 1;
				}

				int cy = 0;
				do
				{
					if ( nSign * ( nScrollBy + cy ) > nSign * cyShift )
					{
						nScrollBy = cyShift - cy;
					}
					cy += nScrollBy;
					VERIFY( ScrollWindowEx( m_hWndView, 0, nScrollBy, NULL, &m_rcView, NULL, NULL, SW_INVALIDATE ) );
				} while ( nSign * cy < nSign * cyShift );

				ASSERT( cy == cyShift );
			}
			else
			{
				VERIFY( ScrollWindowEx( m_hWndView, 0, cyShift, NULL, &m_rcView, NULL, NULL, SW_INVALIDATE ) );
			}
			m_pCtrl->SafeUpdateWindow();
		}
		else
		{
			m_pCtrl->Repaint( FALSE );
		}

		if ( m_hWndVScroll )
		{
			SCROLLINFO si;
			si.cbSize = sizeof( SCROLLINFO );
			si.fMask = SIF_POS;
			si.nPos = m_nTopIndex;
			SendMessage( m_hWndVScroll, SBM_SETSCROLLINFO, TRUE, ( LPARAM )&si );
		}
	}
}

void CEditView::GetRect( LPRECT prc ) const
{
	ASSERT( prc != NULL );
	CopyRect( prc, &m_rcAll );
}

void CEditView::GetViewRect( LPRECT prcView ) const
{
	ASSERT( prcView != NULL );
	CopyRect( prcView, &m_rcView );
}

void CEditView::OnFontChanged( HFONT hFont, int cxChar )
{
	HDC hDC = GetDC( NULL );
	ASSERT( hDC );
	ASSERT( hFont );
	HFONT hOldFont = ( HFONT ) SelectObject( hDC, hFont );
	TEXTMETRIC tm;
	VERIFY( GetTextMetrics( hDC, &tm ) );
	SelectObject( hDC, hOldFont );
	ReleaseDC( NULL, hDC );

	m_cyLine = tm.tmHeight;
	m_cyLinePlusOverlap = tm.tmExternalLeading + tm.tmHeight + 1;
	m_cyLinePlusOverlap = max( m_cyLinePlusOverlap, m_cyLine );
	m_cxChar = cxChar;
	RecalcIndexes();
}

void CEditView::GetCharPosFromPoint( int x, int y, int &nCol, int &nRow, LPRECT prcChar ) const
{
	// x and y are view-relative, not control-relative!
	ASSERT( this );
	ASSERT( prcChar );

	int cxLeftMargin = GetLeftMargin( TRUE, TRUE );
	nCol = m_nLeftIndex + ( x - m_rcAll.left - cxLeftMargin ) / m_cxChar;
	nRow = m_nTopIndex  + ( y - m_rcAll.top )  / m_cyLine;
	nRow = max( 0, nRow );
	nCol = max( 0, nCol );

	BOOL bIsTab = FALSE;

	int nBuffCol = nCol;
	if ( nRow < m_pBuffer->GetLineCount() )
	{
		nBuffCol = m_pBuffer->ConvertViewColToBufferCol( nRow, nCol );
		if ( nBuffCol < m_pBuffer->GetLineLength( nRow ) )
		{
			// snap to first view col
			nCol = m_pBuffer->ConvertBufferColToViewCol( nRow, nBuffCol );
		}
	}

	prcChar->left = m_rcAll.left + cxLeftMargin + ( nCol - m_nLeftIndex ) * m_cxChar;
	prcChar->top =  m_rcAll.top  + ( nRow - m_nTopIndex )  * m_cyLine;
	prcChar->bottom = prcChar->top + m_cyLine;

	if ( bIsTab )
	{
		int cbTabSize = m_pBuffer->GetTabSize();
		prcChar->right = prcChar->left + m_cxChar * ( ( ( ( nCol + cbTabSize ) / cbTabSize ) * cbTabSize ) - nCol );
	}
	else
	{
		prcChar->right = prcChar->left + m_cxChar * m_pBuffer->GetCharSize( nRow, nBuffCol );
	}
}

void CEditView::GetCharBoundingRect( int nCol, int nRow, LPRECT prcChar ) const
{
	int nViewCol = m_pBuffer->ConvertBufferColToViewCol( nRow, nCol );
	int cyLine = GetLineHeight();
	prcChar->top = m_rcAll.top + ( nRow - GetTopIndex() ) * cyLine;
	prcChar->bottom = prcChar->top + cyLine;
	int cxChar = GetCharWidth();
	prcChar->left = m_rcAll.left + GetLeftMargin( TRUE, TRUE ) + ( nViewCol - GetLeftIndex() ) * cxChar;
	// for tabs, we want to expand the rect to represent the entire expanded width, not just one char's worth
	int cbChar = m_pBuffer->GetCharSize( nRow, nCol );
	prcChar->right = prcChar->left + cxChar * ( m_pBuffer->ConvertBufferColToViewCol( nRow, nCol + cbChar ) - nViewCol );
}

void CEditView::RecalcLayout( BOOL bReserveHSplitter, BOOL bReserveVSplitter )
{
	m_rcView.left = m_rcAll.left;
	m_rcView.top = m_rcAll.top;
	int cxAll = ( m_rcAll.right - m_rcAll.left );
	int cyAll = ( m_rcAll.bottom - m_rcAll.top );

	BOOL bShowHorz = m_pCtrl->ShowHScrollBar();
	BOOL bShowVert = m_pCtrl->ShowVScrollBar();

	int cyHScrollBar = GetSystemMetrics( SM_CYHSCROLL );
	int cxVScrollBar = GetSystemMetrics( SM_CXVSCROLL );
	if ( bShowHorz )
	{
		if ( !m_hWndHScroll )
		{
   			m_hWndHScroll = CreateWindow( _T("SCROLLBAR"), _T(""), WS_CHILD | SBS_HORZ, 0, 0, 10, 10, 
			                              m_hWndView, NULL, ( HINSTANCE ) GetWindowLong( m_hWndView, GWL_HINSTANCE ), NULL );
			ASSERT( m_hWndHScroll );
		}
		m_rcView.bottom = m_rcAll.bottom - cyHScrollBar;
		RECT rcCtrl;
		GetWindowRect( m_hWndHScroll, &rcCtrl );
		ScreenToClient( m_hWndView, ( LPPOINT )&rcCtrl );
		ScreenToClient( m_hWndView, ( LPPOINT )&rcCtrl.right );
		int cxScrollBar = cxAll - ( bShowVert ? cxVScrollBar : 0 ) - ( bReserveHSplitter ? CEdit::CXY_SPLITTER : 0 );
		SetWindowPos( m_hWndHScroll, NULL, m_rcAll.left + ( bReserveHSplitter ? CEdit::CXY_SPLITTER : 0 ), m_rcView.bottom, cxScrollBar, cyHScrollBar, SWP_NOZORDER | ( ( cyAll > cyHScrollBar ) ? SWP_SHOWWINDOW : SWP_HIDEWINDOW ) );
	}
	else
	{
		if ( m_hWndHScroll )
		{
			ASSERT( IsWindow( m_hWndHScroll ) );
			DestroyWindow( m_hWndHScroll );
			m_hWndHScroll = NULL;
		}
		m_rcView.bottom = m_rcAll.bottom;
	}

	if ( bShowVert )
	{
		if ( !m_hWndVScroll )
		{
   			m_hWndVScroll = CreateWindow( _T("SCROLLBAR"), _T(""), WS_CHILD | SBS_VERT, 0, 0, 10, 10, 
			                              m_hWndView, NULL, ( HINSTANCE ) GetWindowLong( m_hWndView, GWL_HINSTANCE ), NULL );
			ASSERT( m_hWndVScroll );
		}

		m_rcView.right = m_rcAll.right - cxVScrollBar;
		RECT rcCtrl;
		GetWindowRect( m_hWndVScroll, &rcCtrl );
		ScreenToClient( m_hWndView, ( LPPOINT )&rcCtrl );
		ScreenToClient( m_hWndView, ( LPPOINT )&rcCtrl.right );
		int cyScrollBar = cyAll - ( bShowHorz ? cyHScrollBar : 0 ) - ( bReserveVSplitter ? CEdit::CXY_SPLITTER : 0 );
		SetWindowPos( m_hWndVScroll, NULL, m_rcView.right, m_rcAll.top + ( bReserveVSplitter ? CEdit::CXY_SPLITTER : 0 ), cxVScrollBar, cyScrollBar, SWP_NOZORDER | ( ( cxAll > cxVScrollBar ) ? SWP_SHOWWINDOW : SWP_HIDEWINDOW ) );
	}
	else
	{
		if ( m_hWndVScroll )
		{
			ASSERT( IsWindow( m_hWndVScroll ) );
			DestroyWindow( m_hWndVScroll );
			m_hWndVScroll = NULL;
		}
		m_rcView.right = m_rcAll.right;
	}

	RecalcScrollInfo();
}

void CEditView::RecalcScrollInfo() const
{
	if ( m_hWndHScroll )
	{
		ASSERT( IsWindow( m_hWndHScroll ) );
		SCROLLINFO si;
		si.cbSize = sizeof( SCROLLINFO );
		si.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;
		si.nPage = m_nRightIndex - m_nLeftIndex;
		si.nMin = 0;
		si.nMax = m_pBuffer->GetMaxLineWidthGuess() + 200;
		si.nPos = m_nLeftIndex;
		SendMessage( m_hWndHScroll, SBM_SETSCROLLINFO, TRUE, ( LPARAM )&si );
	}

	if ( m_hWndVScroll )
	{
		ASSERT( IsWindow( m_hWndVScroll ) );
		SCROLLINFO si;
		si.cbSize = sizeof( SCROLLINFO );
		si.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;
		int nViewHeight = m_nBottomIndex - m_nTopIndex;
		int nLineCount = m_pBuffer->GetLineCount();
		si.nPage = nViewHeight;
		si.nMin = 0;
		si.nMax = nLineCount + nViewHeight - 1;
		si.nPos = m_nTopIndex;
		SendMessage( m_hWndVScroll, SBM_SETSCROLLINFO, TRUE, ( LPARAM )&si );
	}
}

void CEditView::Scroll( int nNumCols, int nNumRows )
{
	int nCol = m_nLeftIndex + nNumCols;
	int nMaxLineLengthGuess = m_pBuffer->GetMaxLineWidthGuess();
	nCol = min( nMaxLineLengthGuess, nCol );
	nCol = max( 0, nCol );

	int nRow = m_nTopIndex + nNumRows;
	int nLastLine = m_pBuffer->GetLineCount() - 1;
	nRow = min( nRow, nLastLine );
	nRow = max( 0, nRow );

	ScrollTo( nCol, nRow, ( nNumCols * nNumCols == 1 && !nNumRows ) || ( nNumRows * nNumRows == 1 && !nNumCols ) );
}

void CEditView::SetRightIndex( int nPos )
{
	int nOffset = m_nRightIndex - m_nRightIndexFullyVisible;
	m_nRightIndex = m_nRightIndexFullyVisible = nPos;
	m_nRightIndexFullyVisible -= nOffset;
}

void CEditView::SetBottomIndex( int nPos )
{
	int nOffset = m_nBottomIndex - m_nBottomIndexFullyVisible;
	m_nBottomIndex = m_nBottomIndexFullyVisible = nPos;
	m_nBottomIndexFullyVisible -= nOffset;
}

void CEditView::OnEndEdit()
{
	if ( --m_nEditLevel == 0 )
	{
		// repaint any lines affected by the edits.
		// damage one line up from the start, because in the case of extended
		// selection where the topmost line selected is partially selected after
		// the end of the line (meaning no text in that line changed), a remnant
		// will be left behind since no buffer notifications were sent for that
		// line.

		// if color syntax is used, then must repaint all lines below the first, since a comment block spanning multiple rows may have
		// implicitly invalidated extra rows
		RecalcScrollInfo();
		RepaintView( ( m_nLineDamageStart > 0 ) ? ( m_nLineDamageStart - 1 ) : m_nLineDamageStart, m_nLineDamageEnd );

		// reset the damage
		ResetDamage();
	}
}

void CEditView::DamageView( int nLineStart, int nLineEnd )
{
	// collect the minimum damage
	m_nLineDamageStart = min( nLineStart, m_nLineDamageStart );
	m_nLineDamageEnd = max( nLineEnd, m_nLineDamageEnd );

	if ( m_nEditLevel == 0 )
	{
		RepaintView( m_nLineDamageStart, m_nLineDamageEnd );
		ResetDamage();
	}
}

void CEditView::RepaintView( int nLineStart, int nLineEnd ) const
{
	RECT rcDamage;
	rcDamage.left = m_rcView.left;
	rcDamage.right = m_rcView.right;
	rcDamage.top = m_rcView.top + ( nLineStart - m_nTopIndex ) * m_cyLine;
	nLineEnd = min( m_nBottomIndex, nLineEnd );
	rcDamage.bottom = rcDamage.top + ( nLineEnd - nLineStart + 1 ) * m_cyLine + ( m_cyLinePlusOverlap - m_cyLine );

	RECT rcIntersect;
	if ( IntersectRect( &rcIntersect, &rcDamage, &m_rcView ) )
	{
		InvalidateRect( m_hWndView, &rcIntersect, FALSE );
	}
}

void CEditView::ResetDamage()
{
	m_nLineDamageStart = MAXROW;
	m_nLineDamageEnd = -1;
}

BOOL CEditView::ScrollIfNearBorder( int xPos, int yPos, BOOL bHideDragCaretOnScroll )
{
	int xShift = 0, yShift = 0;
	int xLeft = m_rcView.left + GetLeftMargin( TRUE, TRUE );
	if ( xPos >= xLeft && ( xPos - xLeft <= CXY_DRAG_SCROLL ) )
	{
		xShift = -CX_DRAG_SCROLL_INCREMENT_SMALL;
	}
	else if ( xPos < xLeft )
	{
		xShift = -CX_DRAG_SCROLL_INCREMENT_LARGE;
	}
	else if ( xPos <= m_rcView.right && ( m_rcView.right - xPos <= CXY_DRAG_SCROLL ) )
	{
		xShift = +CX_DRAG_SCROLL_INCREMENT_SMALL;
	}
	else if ( xPos > m_rcView.right )
	{
		xShift = +CX_DRAG_SCROLL_INCREMENT_LARGE;
	}
	if ( yPos >= m_rcView.top && ( yPos - m_rcView.top <= CXY_DRAG_SCROLL ) )
	{
		yShift = -CY_DRAG_SCROLL_INCREMENT_SMALL;
	}
	else if ( yPos < m_rcView.top )
	{
		yShift = -CY_DRAG_SCROLL_INCREMENT_LARGE;
	}
	else if ( yPos <= m_rcView.bottom && ( m_rcView.bottom - yPos <= CXY_DRAG_SCROLL ) )
	{
		yShift = +CY_DRAG_SCROLL_INCREMENT_SMALL;
	}
	else if ( yPos > m_rcView.bottom )
	{
		yShift = +CY_DRAG_SCROLL_INCREMENT_LARGE;
	}

	BOOL bScrolled = FALSE;
	if ( xShift || yShift )
	{
		if ( bHideDragCaretOnScroll )
		{
			m_pCtrl->DrawDragCaret( TRUE );
		}
		Scroll( xShift, yShift );
		bScrolled = TRUE;
	}

	return bScrolled;
}

int CEditView::GetLeftMargin( BOOL bIncludeLineNums, BOOL bIncludeTextMargin ) const
{
	int nWidth = ( m_pCtrl->ShowLeftMargin() ? CM_CXLEFTMARGIN : 0 );
	if ( bIncludeLineNums && m_pCtrl->LineNumberingEnabled() )
		nWidth += ( m_pCtrl->GetLineNumWidth() + LINEWIDTH_EXTRA ) * m_cxChar;
	if ( bIncludeTextMargin )
		nWidth += m_cxChar / 2;
	return nWidth;
}

BOOL CEditView::LineIsVisible( int nLine ) const
{
	return ( nLine >= m_nTopIndex && nLine <= m_nBottomIndexFullyVisible );
}
