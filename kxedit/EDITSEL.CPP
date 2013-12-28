#include "precomp.h"
#include "cedit.h"
#include "editview.h"
#include "editsel.h"

CSelection::CSelection()
{
	m_hWnd = NULL;
	m_pView = NULL;
	m_nStartCol = m_nStartRow = m_nEndCol = m_nEndViewColPreferred = m_nEndRow = m_nStartViewCol = m_nEndViewCol = 0;
	m_cxCaret = m_cxCaretIns = 3;
	m_cyCaret = 17;
	m_bCaretVisible = FALSE;
	m_bColumnSel = FALSE;
}

CSelection::~CSelection()
{
}

void CSelection::Initialize( CEdit *pCtrl, CEditView *pActiveView )
{
	m_pCtrl = pCtrl;
	m_hWnd = pCtrl->GetWindow();
	m_pView = pActiveView;
}

void CSelection::OnFocusChange( BOOL bSetFocus )
{
	if ( bSetFocus )
	{
		ASSERT( IsWindow( m_hWnd ) );
		ASSERT( GetFocus() == m_hWnd );
		VERIFY( CreateCaret( m_hWnd, NULL, m_cxCaret, m_cyCaret ) );
		UpdateCaretPosition();
		ShowCaret();
	}
	else
	{
		HideCaret();
		ASSERT( IsWindow( m_hWnd ) );
		DestroyCaret();
	}
}

BOOL CSelection::ShowCaret()
{
	BOOL bResult = TRUE;
	if ( !m_bCaretVisible )
	{
		bResult = ::ShowCaret( m_hWnd );
		m_bCaretVisible = bResult;
	}

	return bResult;
}

BOOL CSelection::HideCaret()
{
	BOOL bResult = TRUE;
	if ( m_bCaretVisible )
	{
		bResult = ::HideCaret( m_hWnd );
		m_bCaretVisible = FALSE;
	}

	return bResult;
}

void CSelection::SetEmptySelection( int nCol, int nRow, BOOL bEnsureVisible, BOOL bAllowDamage )
{
	int nOldRow = m_nEndRow;
	m_bColumnSel = FALSE;
	SetExtendedSelection( nCol, nRow, nCol, nRow, bEnsureVisible, bAllowDamage );

	// if user changed lines, notify the control so it can normalize the text case in the
	// line that was just left.
	if ( nOldRow != nRow )
	{
	m_pCtrl->OnChangeLineSelection();
	}
}

void CSelection::SetExtendedSelection( int nStartCol, int nStartRow, int nEndCol, int nEndRow, BOOL bEnsureVisible, BOOL bAllowDamage )
{
	ASSERT( nStartRow >= 0 );

	int nOldStartRow = m_nStartRow;
	int nOldEndRow = m_nEndRow;

	CBuffer *pBuffer = m_pCtrl->GetBuffer();
	int nMaxLine = pBuffer->GetLineCount() - 1;
	nMaxLine = max( 0, nMaxLine );
	m_nStartRow = min( nMaxLine, nStartRow );
	m_nEndRow = min( nMaxLine, nEndRow );

	m_nStartCol = nStartCol;
	
	m_nEndCol = nEndCol;
	// keep selection over text, if requested to
	if ( BoundSelection() )
		{
		BOOL bKeepEmpty = ( nStartCol == nEndCol && nStartRow == nEndRow );
		EnforceSelBounds();
		if ( bKeepEmpty )
			{
			m_nStartCol = m_nEndCol;
			}
		}


	m_nEndViewCol = pBuffer->ConvertBufferColToViewCol( m_nEndRow, m_nEndCol );
	m_nStartViewCol = pBuffer->ConvertBufferColToViewCol( m_nStartRow, m_nStartCol );
	m_nEndViewColPreferred = m_nEndViewCol;

	// damage old selection and new selection to clear it
	if ( bAllowDamage )
	{
		int nStartDamage = min( nOldStartRow, nOldEndRow );
		nStartDamage = min( nStartDamage, nStartRow );
		nStartDamage = min( nStartDamage, nEndRow );
		int nEndDamage = max( nOldStartRow, nOldEndRow );
		nEndDamage = max( nEndDamage, nStartRow );
		nEndDamage = max( nEndDamage, nEndRow );
		m_pView->DamageView( nStartDamage, nEndDamage );
	}

	if ( bEnsureVisible )
	{
		EnsureVisible( TRUE );
	}

	if ( ::IsWindow( m_hWnd ) && GetFocus() == m_hWnd )
	{
		UpdateCaretPosition();
	}
}

void CSelection::SetSelectionFromPoint( CEditView *pView, int x, int y, BOOL bEmpty, BOOL bAllowLineSel )
{
	ASSERT( bEmpty || ( pView == m_pView ) );

	if ( m_pView != pView )
	{
		CEditView *pLastView = m_pView;
		m_pView = pView;
		if ( !IsEmpty() )
		{
			// switching views -- erase the selection in the other view
			pLastView->DamageView( min( m_nEndRow, m_nStartRow ), max( m_nEndRow, m_nStartRow ) );
		}
	}

	int nCol, nRow;
	RECT rcChar;
	m_pView->GetCharPosFromPoint( x, y, nCol, nRow, &rcChar );

	RECT rcView;
	m_pView->GetViewRect( &rcView );
	CBuffer *pBuffer = m_pCtrl->GetBuffer();
	int nLineCount = pBuffer->GetLineCount();

	if ( !bEmpty && bAllowLineSel && ( x > rcView.left && x < ( rcView.left + m_pView->GetLeftMargin( TRUE ) ) ) )
	{
		// line selecting
		nCol = 0;
		if ( nRow < m_nStartRow )
		{
			m_nStartCol = CEditView::MAXCOL;
		}
		else
		{
			m_nStartCol = 0;
			nRow++;
		}
		nRow = min( nRow, nLineCount - 1 );
		nRow = max( 0, nRow );
		// selecting the last line should just go to the end of the line since
		// there is no line below nRow.
		if ( nLineCount && ( nRow == nLineCount - 1 ) )
		{
			nCol = pBuffer->GetLineLength( nRow );
		}
	}
	else
	{
		nRow = min( nRow, nLineCount - 1 );
		nRow = max( 0, nRow );
		nCol = pBuffer->ConvertViewColToBufferCol( nRow, nCol );
	}

	// since the column might have changed above, let's refetch the
	// char rect.
	m_pView->GetCharBoundingRect( nCol, nRow, &rcChar );

	if ( !IsRectEmpty( &rcChar ) && ( x > ( ( rcChar.left + rcChar.right ) / 2 ) ) )
	{
		// cursor is closer to the next char
		nCol += pBuffer->GetCharSize( nRow, nCol );
	}

	if ( bEmpty )
	{
		SetEmptySelection( nCol, nRow );
	}
	else
	{
		if ( nCol != m_nEndCol || nRow != m_nEndRow )
			ExtendTo( nCol, nRow );
	}
}

void CSelection::ResetCaret( HFONT hFont )
{
	HDC hDC = GetDC( NULL );
	ASSERT( hDC );
	ASSERT( hFont );
	HFONT hOldFont = ( HFONT ) SelectObject( hDC, hFont );
	TEXTMETRIC tm;
	VERIFY( GetTextMetrics( hDC, &tm ) );
	SelectObject( hDC, hOldFont );
	ReleaseDC( NULL, hDC );

	// caret height is the font height
	m_cyCaret = tm.tmExternalLeading + tm.tmHeight;

	// insert caret width is the font height / 6
	m_cxCaretIns = m_cyCaret / 6;
	m_cxCaretIns = max( m_cxCaretIns, 2 );

	if ( m_pCtrl->OvertypeCaret() && m_pCtrl->InOvertypeMode() )
	{
		m_cxCaret = tm.tmAveCharWidth;
	}
	else
	{
		m_cxCaret = m_cxCaretIns;
	}

	// recreate the caret in the right dimensions
	if ( ::GetFocus() == m_hWnd )
	{
		VERIFY( DestroyCaret() );
		VERIFY( CreateCaret( m_hWnd, NULL, m_cxCaret, m_cyCaret ) );
		::ShowCaret( m_hWnd );
	}
}

void CSelection::OnFontChanged( HFONT hFont )
{
	ResetCaret( hFont );
}

void CSelection::UpdateCaretPosition() const
{
	ASSERT( m_pView );

	RECT rcView;
	m_pView->GetViewRect( &rcView );

	if ( GetFocus() == m_hWnd && !m_pCtrl->DelayPaint() )
	{
		int yCaret = rcView.top + ( ( m_nEndRow - m_pView->GetTopIndex() ) * m_pView->GetLineHeight() );
		int cxLeftMargin = m_pView->GetLeftMargin( TRUE, TRUE );
		int xCaret = rcView.left + cxLeftMargin + ( ( m_nEndViewCol - m_pView->GetLeftIndex() ) * m_pView->GetCharWidth() );
		if ( m_pCtrl->ShowHScrollBar() && ( ( ( yCaret + m_cyCaret ) > rcView.bottom ) ||
		                                    ( yCaret < rcView.top ) ) )
		{
			// don't let the caret draw over the scrollbars
			yCaret = -m_cyCaret - 1;
		}

		if ( ( xCaret < rcView.left + cxLeftMargin ) || 
		     ( m_pCtrl->ShowVScrollBar() && ( ( ( xCaret + m_cxCaret ) > rcView.right ) ||
		                                      ( xCaret < rcView.left ) ) ) )
		{
			// don't let the caret draw over the scrollbars
			xCaret = -m_cxCaret - 1;
		}

		POINT pt;
		GetCaretPos( &pt );
		if ( pt.x != xCaret || pt.y != yCaret )
		{
			VERIFY( SetCaretPos( xCaret, yCaret ) );
		}
	}
}

void CSelection::GetCaretRect( CEditView *pView, int nBuffCol, int nRow, RECT &rcCaret )
{
	ASSERT( pView );

	RECT rcView;
	pView->GetRect( &rcView );

	CBuffer *pBuffer = m_pCtrl->GetBuffer();
	int nLastLine = pBuffer->GetLineCount() - 1;
	int nViewCol = nBuffCol;
	nRow = min( nLastLine, nRow );
	if ( nRow < 0 )
	{
		nRow = 0;
	}
	else
	{
		nViewCol = pBuffer->ConvertBufferColToViewCol( nRow, nBuffCol );
	}

	rcCaret.left = rcView.left + pView->GetLeftMargin( TRUE, TRUE ) + ( ( nViewCol - pView->GetLeftIndex() ) * pView->GetCharWidth() );
	rcCaret.right = rcCaret.left + m_cxCaretIns;
	rcCaret.top = rcView.top + ( ( nRow - pView->GetTopIndex() ) * pView->GetLineHeight() );
	rcCaret.bottom = rcCaret.top + m_cyCaret;
}

void CSelection::Extend( Direction eDirection, Amount eAmount, BOOL bScrollIfNeccessary, BOOL bDamage, BOOL bAllowPastEndOfLine )
{
	ASSERT( !( ( eDirection == eUp || eDirection == eDown ) && eAmount == eWord ) );
	ASSERT( !( ( eDirection == eUp || eDirection == eDown ) && eAmount == eSentence ) );

	CBuffer *pBuffer = m_pCtrl->GetBuffer();
	int nLineCount = pBuffer->GetLineCount();
	BOOL bEnforceSelBounds = BoundSelection();
	int nSaveEndRow = m_nEndRow;
	int nSaveEndCol = m_nEndCol;
	BOOL bUsePreferredCol = FALSE;

	if ( nLineCount )
	{
		int nOldEndRow = m_nEndRow;
		int nOldStartRow = m_nStartRow;
		LPCTSTR pszEndLineStart = pBuffer->GetLineText( m_nEndRow );
		int nEndLineLen = pBuffer->GetLineLength( m_nEndRow );

		BOOL bStartRowChanged = FALSE;

		switch ( eDirection )
		{
			case eUp:
				{
					switch ( eAmount )
					{
						case eChar:
						{
							m_nEndRow--;
							bUsePreferredCol = TRUE;
							break;
						}
						case ePage:
						{
							m_nEndRow -= ( m_pView->GetBottomIndex( FALSE ) - m_pView->GetTopIndex() );
							break;
						}
						case eSmartAll:
						case eAll:
						{
							m_nEndRow = 0;
							break;
						}
					}
					break;
				}
			case eDown:
				{
					switch ( eAmount )
					{
						case eChar:
						{
							m_nEndRow++;
							bUsePreferredCol = TRUE;
							break;
						}
						case ePage:
						{
							int nTemp = m_nEndRow + ( m_pView->GetBottomIndex( FALSE ) - m_pView->GetTopIndex() );
							m_nEndRow = min( nLineCount, nTemp );
							break;
						}
						case eAll:
						case eSmartAll:
						{
							m_nEndRow = nLineCount - 1;
							break;
						}
					}
					break;
				}
			case eLeft:
				{
					switch ( eAmount )
					{
						case eChar:
						{
							if ( m_nEndCol == 0 || m_nEndCol > nEndLineLen )
								m_nEndCol--;
							else
								m_nEndCol -= _tclen_prev( pszEndLineStart, pszEndLineStart + m_nEndCol );
							if ( m_nEndCol < 0 )
							{
								if ( bAllowPastEndOfLine && m_nEndRow > 0 )
								{
									m_nEndRow--;
									m_nEndCol = pBuffer->GetLineLength( m_nEndRow );
									bEnforceSelBounds = FALSE;	// already enforced by previous statement!
								}
								else
								{
									m_nEndCol = 0;
								}
							}
							break;
						}
						case ePage:
						{
							m_nEndCol -= ( m_pView->GetRightIndex( FALSE ) - m_pView->GetLeftIndex() );
							break;
						}
						case eAll:
						{
							m_nEndCol = 0;
							break;
						}
						case eSmartAll:
						{
							LPCTSTR pszLine = pBuffer->GetLineText( m_nEndRow );
							int nFirstNonSpace = 0;
							while ( *pszLine && ( *pszLine == _T(' ') || *pszLine == _T('\t') ) )
							{
								nFirstNonSpace++;
								pszLine = _tcsinc( pszLine );
							}

							// jump between absolute left and 'textual' left
							m_nEndCol = ( m_nEndCol == nFirstNonSpace ? 0 : nFirstNonSpace );
							break;
						}
						case eWord:
						{
							pBuffer->AdvanceToWordStart( m_nEndRow, m_nEndCol, FALSE, TRUE );
							break;
						}
						case eWordEnd:
						{
							pBuffer->AdvanceToWordEnd( m_nEndRow, m_nEndCol, FALSE, TRUE );
							break;
						}
						case eSentence:
						{
							pBuffer->AdvanceToSentenceStart( m_nEndRow, m_nEndCol, FALSE );
							break;
						}
					}
					break;
				}
			case eRight:
				{
					switch ( eAmount )
					{
						case eChar:
						{
							if ( m_nEndCol >= nEndLineLen )
								m_nEndCol++;
							else
								m_nEndCol += _tclen( pszEndLineStart + m_nEndCol );
							break;
						}
						case ePage:
						{
							m_nEndCol += ( m_pView->GetRightIndex( FALSE ) - m_pView->GetLeftIndex() );
							break;
						}
						case eAll:
						{
							m_nEndCol = pBuffer->GetLineLength( m_nEndRow );
							break;
						}
						case eSmartAll:
						{
							LPCTSTR pszStart = pBuffer->GetLineText( m_nEndRow );
							int nLastChar = pBuffer->GetLineLength( m_nEndRow );
							int nFirstNonSpace = nLastChar;
							LPCTSTR pszEnd = pszStart + nFirstNonSpace - 1;
							while ( ( pszEnd >= pszStart ) && ( *pszEnd == _T(' ') || *pszEnd == _T('\t') ) )
							{
								nFirstNonSpace--;
								pszEnd = _tcsdec( pszStart, pszEnd );
							}

							// jump between absolute right and 'textual' right
							m_nEndCol = ( m_nEndCol <= nFirstNonSpace ? nLastChar : nFirstNonSpace );
							break;
						}
						case eWord:
						{
							pBuffer->AdvanceToWordStart( m_nEndRow, m_nEndCol, TRUE, TRUE );
							break;
						}
						case eWordEnd:
						{
							pBuffer->AdvanceToWordEnd( m_nEndRow, m_nEndCol, TRUE, TRUE );
							break;
						}
						case eSentence:
						{
							pBuffer->AdvanceToSentenceStart( m_nEndRow, m_nEndCol, TRUE );
							break;
						}
					}
					break;
				}
			case eOutward:
				{
					switch ( eAmount )
					{
						case eWord:
						{
							m_nStartCol = m_nEndCol;
							int nLineLen = pBuffer->GetLineLength( m_nEndRow );
							if ( m_nStartCol <= nLineLen )
							{
								if ( m_nStartCol )
								{
									pBuffer->AdvanceToWordStart( m_nEndRow, m_nStartCol, FALSE, FALSE );
								}
								m_nEndCol = m_nStartCol;
								if ( m_nStartCol < nLineLen )
								{
									pBuffer->AdvanceToWordEnd( m_nEndRow, m_nEndCol, TRUE, FALSE );
								}
							}
							break;
						}
						case eSentence:
						{
							m_nStartRow = m_nEndRow;
							m_nStartCol = 0;
							pBuffer->AdvanceToSentenceStart( m_nStartRow, m_nStartCol, FALSE );
							m_nEndCol = m_nStartCol;
							m_nEndRow = m_nStartRow;
							pBuffer->AdvanceToSentenceStart( m_nEndRow, m_nEndCol, TRUE );
							bStartRowChanged = ( m_nStartRow != nOldStartRow );
							break;
						}
					}
					break;
				}
		}

		int nTemp = nLineCount - 1;
		m_nEndRow = min( m_nEndRow, nTemp );
		m_nEndRow = max( 0, m_nEndRow );

		m_nEndCol = ( nLineCount == 0 ) ? 0 : max( 0, m_nEndCol );
		BOOL bEndViewColUpToDate = FALSE;
		
		// keep cursor within the line's bounds if requested to
		if ( bEnforceSelBounds )
		{
			// special case: if moving left one char and beyond the end of the line,
			// do the fixup now or else the one-char move will be nullified by
			// EnforceSelBounds()
			if ( nLineCount && eDirection == eLeft && eAmount == eChar )
			{
				int nEndRowLen = pBuffer->GetLineLength( m_nEndRow );
			    if ( m_nEndCol >= nEndRowLen )
				{
					m_nEndCol = nEndRowLen - 1;
					m_nEndCol = max( 0, m_nEndCol );
				}
			}

			if ( bUsePreferredCol && nSaveEndRow != m_nEndRow )
				m_nEndCol = pBuffer->ConvertViewColToBufferCol( m_nEndRow, m_nEndViewColPreferred );

			BOOL bFixup = EnforceSelBounds();

			// if we didn't have to fix-up the selection, remember this new col position
			// as the preferred position.
			if ( !bFixup )
			{
				if ( bUsePreferredCol && nSaveEndRow != m_nEndRow )
					{
					// moved vertically -- need to translate view col from one row to another
					int nBuffCol = pBuffer->ConvertViewColToBufferCol( m_nEndRow, m_nEndViewColPreferred );
					m_nEndViewCol = pBuffer->ConvertBufferColToViewCol( m_nEndRow, nBuffCol );
					m_nEndCol = pBuffer->ConvertViewColToBufferCol( m_nEndRow, m_nEndViewCol );
					}
				else if ( nSaveEndCol != m_nEndCol )
					{
					m_nEndViewCol = pBuffer->ConvertBufferColToViewCol( m_nEndRow, m_nEndCol );
					m_nEndViewColPreferred = m_nEndViewCol;
					}
				bEndViewColUpToDate = TRUE;
			}
		}

		// since m_nEndCol may have changed, we need to recalc the view position and re-snap m_nEndCol to the current row
		if ( !bEndViewColUpToDate )
			{
			m_nEndViewCol = pBuffer->ConvertBufferColToViewCol( m_nEndRow, m_nEndCol );
			m_nEndCol = pBuffer->ConvertViewColToBufferCol( m_nEndRow, m_nEndViewCol );
			}

		if ( eDirection == eOutward )
		{
			m_nStartViewCol = pBuffer->ConvertBufferColToViewCol( m_nStartRow, m_nStartCol );
		}

		if ( bDamage )
		{
			int nDamageStart = min( nOldEndRow, m_nEndRow );
			int nDamageEnd = max( nOldEndRow, m_nEndRow );
			if ( m_bColumnSel )
				{
				nDamageStart = min( nDamageStart, nOldStartRow );
				nDamageStart = min( nDamageStart, m_nStartRow );
				nDamageEnd = max( nDamageEnd, nOldStartRow );
				nDamageEnd = max( nDamageEnd, m_nStartRow );
				}
			if ( bStartRowChanged )
			{
				nDamageStart = min( nDamageStart, nOldStartRow );
				nDamageStart = min( nDamageStart, m_nStartRow );
				nDamageEnd = max( nDamageEnd, nOldEndRow );
				nDamageEnd = max( nDamageEnd, m_nEndRow );
			}
				
			m_pView->DamageView( nDamageStart, nDamageEnd );
		}

		// if user changed lines, notify the control so it can normalize the text case in the
		// line that was just left.
		if ( eDirection == eUp || eDirection == eDown )
		{
			m_pCtrl->OnChangeLineSelection();
		}
	}
	else
	{
		m_nEndCol = m_nEndRow = m_nEndViewCol = m_nStartViewCol = m_nStartCol = m_nStartRow = 0;
	}
	if ( bScrollIfNeccessary )
	{
		EnsureVisible( TRUE );
	}
}

void CSelection::ExtendTo( int nCol, int nRow )
{
	CBuffer *pBuffer = m_pCtrl->GetBuffer();

	int nOldRow = m_nEndRow;

	int nTemp = pBuffer->GetLineCount() - 1;
	nTemp = min( nRow, nTemp );
	m_nEndRow = max( 0, nTemp );

	int nMinRow = min( nOldRow, m_nEndRow );
	int nMaxRow = max( nOldRow, m_nEndRow );
	
	int nOldViewCol = m_nEndViewCol;
	m_nEndCol = max( 0, nCol );

	// keep selection over text is requested to
	if ( BoundSelection() )
	{
		EnforceSelBounds();
	}

	m_nStartViewCol = pBuffer->ConvertBufferColToViewCol( m_nStartRow, m_nStartCol );
	m_nEndViewCol = pBuffer->ConvertBufferColToViewCol( m_nEndRow, m_nEndCol );

	if ( m_bColumnSel && ( m_nEndViewCol != nOldViewCol ) )
	{
		// column sel width changed
		nMinRow = min( nMinRow, m_nStartRow );
		nMaxRow = max( nMaxRow, m_nStartRow );
	}

	// if user changed lines, notify the control so it can normalize the text case in the
	// line that was just left.
	if ( nOldRow != nRow )
	{
		m_pCtrl->OnChangeLineSelection();
	}

	m_pView->DamageView( nMinRow, nMaxRow );
	UpdateCaretPosition();
}

void CSelection::Move( Direction eDirection, Amount eAmount, BOOL bScrollIfNeccessary )
{
	if ( !IsEmpty() )
	{
		Damage();
	}

	EnableColumnSel( FALSE );

	Extend( eDirection, eAmount, FALSE, FALSE, FALSE );

	ASSERT( !( ( eDirection == eUp || eDirection == eDown ) && eAmount == eWord ) );

	m_nStartCol = m_nEndCol;
	m_nStartViewCol = m_nEndViewCol;
	m_nStartRow = m_nEndRow;

	if ( bScrollIfNeccessary )
	{
		EnsureVisible( TRUE );
	}
}

void CSelection::EnsureVisible( BOOL bScrollIfNeccessary, BOOL bCanCenter, BOOL bPreserveSelIfPossible )
{
	ASSERT( m_pView );

	int nTopIndex = m_pView->GetTopIndex();
	int nBottomIndex = m_pView->GetBottomIndex( TRUE );
	int nLeftIndex = m_pView->GetLeftIndex();
	int nRightIndex = m_pView->GetRightIndex( TRUE );

	if ( bScrollIfNeccessary )
	{
		int nViewWidth = nRightIndex - nLeftIndex;
		BOOL bNeedToScroll = FALSE;
		if ( bCanCenter && ( m_nStartRow == m_nEndRow ) && !IsEmpty() )
		{
			// entire selection is on one line -- if can fit within entire view, do so.
			int nSelWidth = m_nEndViewCol - m_nStartViewCol;
			if ( nSelWidth < 0 )
			{
				nSelWidth = -nSelWidth;
			}
			if ( nSelWidth < nViewWidth )
			{
				nLeftIndex = ( m_nEndViewCol < nViewWidth ) ? 0 : m_nStartViewCol - ( nViewWidth - nSelWidth ) / 2;
				bNeedToScroll = TRUE;
			}
		}
		if ( !bNeedToScroll )
		{
			if ( m_nEndViewCol < nLeftIndex )
			{
				// try to scroll all the way back to col 0
				nLeftIndex = ( m_nEndViewCol < nViewWidth ) ? 0 : m_nEndViewCol;
				bNeedToScroll = TRUE;
			}
			else if ( m_nEndViewCol > nRightIndex )
			{
				int nTemp = m_nEndViewCol - MulDiv( nViewWidth, 3, 4 );
				nLeftIndex = max( 0, nTemp );
				bNeedToScroll = TRUE;
			}
		}
		if ( m_nEndRow < nTopIndex )
		{
			nTopIndex = m_nEndRow;
			bNeedToScroll = TRUE;
		}
		else if ( m_nEndRow > nBottomIndex )
		{
			int nTemp = m_nEndRow - ( nBottomIndex - nTopIndex );
			nTopIndex = max( 0, nTemp );
			bNeedToScroll = TRUE;
		}

		if ( bNeedToScroll )
		{
			// if one-char increment, do smooth scrolling
			int nLeft = m_pView->GetLeftIndex();
			int nTop = m_pView->GetTopIndex();
			BOOL bSmooth = ( ( nLeftIndex == nLeft && ( nTop - nTopIndex ) * ( nTop - nTopIndex ) == 1 ) ||
			                 ( nTopIndex == nTop && ( nLeft - nLeftIndex ) * ( nLeft - nLeftIndex ) == 1 ) );
			m_pView->ScrollTo( nLeftIndex, nTopIndex, bSmooth );
		}
	}
	else
	{
		BOOL bWasEmpty = IsEmpty();
		BOOL bSelChanged = FALSE;
		if ( m_nEndViewCol < nLeftIndex )
		{
			m_nEndViewCol = nLeftIndex;
			bSelChanged = TRUE;
		}
		else if ( m_nEndViewCol > nRightIndex )
		{
			m_nEndViewCol = nRightIndex;
			bSelChanged = TRUE;
		}
		if ( m_nEndRow < nTopIndex )
		{
			m_nEndRow = nTopIndex;
			bSelChanged = TRUE;
		}
		else if ( m_nEndRow > nBottomIndex )
		{
			m_nEndRow = nBottomIndex;
			bSelChanged = TRUE;
		}
		
		// update the buffer position, regardless of the selection bounding setting.
		m_nEndCol = m_pCtrl->GetBuffer()->ConvertViewColToBufferCol( m_nEndRow, m_nEndViewCol );

		if ( bSelChanged || !bPreserveSelIfPossible )
			MakeEmpty( !bWasEmpty ); 
	}

	UpdateCaretPosition();
}

void CSelection::GetBufferSelection( int &nStartCol, int &nStartRow, int &nEndCol, int &nEndRow ) const
{
	nStartCol = m_nStartCol;
	nStartRow = m_nStartRow;
	nEndCol = m_nEndCol;
	nEndRow = m_nEndRow;
}

void CSelection::GetViewSelection( int &nStartCol, int &nStartRow, int &nEndCol, int &nEndRow ) const
{
	nStartCol = m_nStartViewCol;
	nStartRow = m_nStartRow;
	nEndCol = m_nEndViewCol;
	nEndRow = m_nEndRow;
}

void CSelection::GetNormalizedBufferSelection( int &nStartCol, int &nStartRow, int &nEndCol, int &nEndRow ) const
{
	if ( m_nStartRow == m_nEndRow )
	{
		nStartRow = nEndRow = m_nStartRow;
		if ( m_nStartCol <= m_nEndCol )
		{
			nStartCol = m_nStartCol;
			nEndCol = m_nEndCol;
		}
		else
		{
			// normalize
			nStartCol = m_nEndCol;
			nEndCol = m_nStartCol;
		}
	}
	else if ( m_nEndRow < m_nStartRow )
	{
		// normalize
		nStartCol = m_nEndCol;
		nEndCol = m_nStartCol;
		nStartRow = m_nEndRow;
		nEndRow = m_nStartRow;
	}
	else
	{
		// already normalized
		nStartCol = m_nStartCol;
		nEndCol = m_nEndCol;
		nStartRow = m_nStartRow;
		nEndRow = m_nEndRow;
	}
}

void CSelection::GetNormalizedViewSelection( int &nStartViewCol, int &nStartRow, int &nEndViewCol, int &nEndRow ) const
{
	if ( m_nStartRow == m_nEndRow )
	{
		nStartRow = nEndRow = m_nStartRow;
		if ( m_nStartViewCol <= m_nEndViewCol )
		{
			nStartViewCol = m_nStartViewCol;
			nEndViewCol = m_nEndViewCol;
		}
		else
		{
			// normalize
			nStartViewCol = m_nEndViewCol;
			nEndViewCol = m_nStartViewCol;
		}
	}
	else if ( m_nEndRow < m_nStartRow )
	{
		// normalize
		nStartViewCol = m_nEndViewCol;
		nEndViewCol = m_nStartViewCol;
		nStartRow = m_nEndRow;
		nEndRow = m_nStartRow;
	}
	else
	{
		// already normalized
		nStartViewCol = m_nStartViewCol;
		nEndViewCol = m_nEndViewCol;
		nStartRow = m_nStartRow;
		nEndRow = m_nEndRow;
	}
}

void CSelection::GetNormalizedColumnSelection( int &nStartViewCol, int &nStartRow, int &nEndViewCol, int &nEndRow ) const
{
	ASSERT( IsColumnSel() );
	GetNormalizedViewSelection( nStartViewCol, nStartRow, nEndViewCol, nEndRow );
	if ( nEndViewCol < nStartViewCol )
	{
		// further normalization
		int nTemp = nEndViewCol;
		nEndViewCol = nStartViewCol;
		nStartViewCol = nTemp;
	}
}

void CSelection::MakeEmpty( BOOL bDamage, BOOL bAtEnd )
{
	if ( !IsEmpty() )
	{
		EnableColumnSel( FALSE );
		if ( bDamage )
		{
			Damage();
		}

		if ( bAtEnd )
		{
			m_nStartCol = m_nEndCol;
			m_nStartViewCol = m_nEndViewCol;
			m_nStartRow = m_nEndRow;
		}
		else
		{
			m_nEndCol = m_nStartCol;
			m_nEndViewCol = m_nStartViewCol;
			m_nEndRow = m_nStartRow;
		}
		UpdateCaretPosition();
	}
}

void CSelection::Damage() const
{
	if ( !IsEmpty() )
	{
		m_pView->DamageView( min( m_nStartRow, m_nEndRow ), max( m_nStartRow, m_nEndRow ) );
	}
}

void CSelection::SetView( CEditView *pView, BOOL bSilent )
{
	if ( m_pView != pView )
	{
		CBuffer *pBuffer = m_pCtrl->GetBuffer();
		int nRow = pView->GetTopIndex();
		int nCol = pBuffer->ConvertViewColToBufferCol( nRow, pView->GetLeftIndex() );
		nCol = pBuffer->ConvertBufferColToViewCol( nRow, nCol );

		if ( !bSilent )
		{
			SetEmptySelection( nCol, nRow );
		}
		m_pView = pView;
		UpdateCaretPosition();
	}
}

void CSelection::SelectWord()
{
	MakeEmpty( FALSE );
	Extend( CSelection::eOutward, CSelection::eWord, FALSE, FALSE, FALSE );
}

BOOL CSelection::EnforceSelBounds()
{
	ASSERT( BoundSelection() );	// don't call this unless it's neccessary
	CBuffer *pBuffer = m_pCtrl->GetBuffer();

	BOOL bFixup = FALSE;

	if ( m_nEndRow < pBuffer->GetLineCount() )
	{
		int nLastChar = pBuffer->GetLineLength( m_nEndRow );
		if ( m_nEndCol > nLastChar )
		{
			m_nEndCol = nLastChar;
			bFixup = TRUE;
		}
	}
	else
	{
		m_nEndCol = 0;
		bFixup = TRUE;
	}

	return bFixup;
}

BOOL CSelection::BoundSelection() const
{
	return !m_bColumnSel && m_pCtrl->BoundSelection();
}

void CSelection::EnableColumnSel( BOOL bEnable, BOOL bRaw )
{
	m_bColumnSel = bEnable && ( bRaw || m_pCtrl->ColumnSelAllowed() );
}
