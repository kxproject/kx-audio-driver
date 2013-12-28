#include "precomp.h"
#include "editdlgs.h"
#include "cedit.h"

extern BOOL g_bCopyColumn;

///////////////////////////////////////////////////////////////////////////////
//
//  Upper/Lower Case
//
//

void CEdit::WordUpperCase()
{	
	CSaveSelection save( this );
	m_Selection.SelectWord();
	UpperCaseSelection();
}

void CEdit::WordLowerCase()
{
	CSaveSelection save( this );
	m_Selection.SelectWord();
	LowerCaseSelection();
}

void CEdit::UpperCaseSelection()
{
	ChangeCaseOfSelection( TRUE );
}

void CEdit::LowerCaseSelection()
{
	ChangeCaseOfSelection( FALSE );
}

void CEdit::WordCapitalize()
{
	CDelayRepaint delay( this );
	WordLowerCase();
	CSaveSelection save( this );
	m_Selection.Move( CSelection::eRight, CSelection::eChar, FALSE );
	m_Selection.Move( CSelection::eLeft, CSelection::eWord, FALSE );
	m_Selection.Extend( CSelection::eRight, CSelection::eChar, FALSE, FALSE, FALSE );
	ChangeCaseOfSelection( TRUE );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Text transposing
//
//

void CEdit::LineTranspose()
{
	CEditTransaction trans( this );
	int nLine = m_Selection.GetEndRow();
	m_Selection.MakeEmpty();
	if ( nLine > 0 )
	{
		m_Buffer.InsertLine( nLine - 1, m_Buffer.GetLineText( nLine ), m_Buffer.GetLineLength( nLine ) );
		m_Buffer.RemoveLines( nLine + 1, 1 );
	}
}

void CEdit::CharTranspose()
{
	if ( !m_Buffer.IsEmpty() )
	{
		CEditTransaction trans( this );
		CSelTransaction seltrans( this );

		m_Selection.MakeEmpty();
		int nLine = m_Selection.GetEndRow();
		int nChar = m_Selection.GetEndCol();
		int cbLine = m_Buffer.GetLineLength( nLine );
		if ( nChar > 0 && nChar < cbLine )
		{
			LPCTSTR pszLine = m_Buffer.GetLineText( nLine );
			LPCTSTR pszChar = pszLine + nChar;
			int cbChar = _tclen( pszChar );
			TCHAR szSave[10];
			ZeroMemory( szSave, ARRAY_SIZE( szSave ) );
			memcpy( szSave, pszChar, cbChar * sizeof( TCHAR ) );
			m_Buffer.RemoveCharsFromLine( nLine, nChar, cbChar );
			int nInsertAt = CharPrev( pszLine, pszChar ) - pszLine;
			m_Buffer.InsertCharsIntoLine( nLine, nInsertAt, szSave, cbChar );
			// fixup caret pos if moved (swapped SBCS char with DBCS char)
			m_Selection.SetEmptySelection( nInsertAt + cbChar, nLine, FALSE, FALSE );
		}
	}
}

void CEdit::WordTranspose()
{
	CEditTransaction trans( this );
	if ( !m_Buffer.IsEmpty() )
	{
		m_Selection.MakeEmpty();
		m_Selection.SelectWord();
		int nStartRow, nStartCol, nEndRow, nEndCol;
		m_Selection.GetBufferSelection( nStartCol, nStartRow, nEndCol, nEndRow );
		if ( !m_Selection.IsEmpty() )
		{
			HGLOBAL hMemWord1;
			if ( nStartCol > 0 )
			{
				int nWord1StartCol = nStartCol;
				int nWord1EndCol = nEndCol;
				m_Buffer.GetText( nStartRow, nStartCol, nEndRow, nEndCol, hMemWord1, FALSE );
				if ( hMemWord1 )
				{
					m_Selection.SetEmptySelection( nStartCol, nStartRow );
					m_Selection.Move( CSelection::eLeft, CSelection::eWord, TRUE );
					m_Selection.SelectWord();
					
					LPCTSTR pszWord1 = ( LPCTSTR ) GlobalLock( hMemWord1 );
					HGLOBAL hMemWord2;
					m_Selection.GetBufferSelection( nStartCol, nStartRow, nEndCol, nEndRow );
					m_Buffer.GetText( nStartRow, nStartCol, nEndRow, nEndCol, hMemWord2, FALSE );
					if ( hMemWord2 )
					{
						DeleteSelection( FALSE, FALSE );
						LPCTSTR pszWord2 = ( LPCTSTR ) GlobalLock( hMemWord2 );
						int cbWord2 = _tcslen( pszWord2 );
						m_Selection.SetEmptySelection( nWord1StartCol - cbWord2, nEndRow );
						ReplaceSelection( pszWord2, FALSE );
						m_Selection.SetExtendedSelection( nWord1StartCol, nEndRow, nWord1EndCol, nEndRow );
						DeleteSelection( FALSE, FALSE );
						m_Selection.SetEmptySelection( nStartCol, nEndRow );
						ReplaceSelection( pszWord1, FALSE );
						m_Selection.SetEmptySelection( nWord1StartCol - cbWord2 + _tcslen( pszWord1 ), nEndRow );
						GlobalUnlock( hMemWord2 );
						GlobalFree( hMemWord2 );

					}
					GlobalUnlock( hMemWord1 );
					GlobalFree( hMemWord2 );
				}
			}
			else
			{
				m_Selection.SetEmptySelection( nStartCol, nStartRow );
			}
		}
		else
		{
			m_Selection.SetEmptySelection( nStartCol, nStartRow );
		}
	}	
}


///////////////////////////////////////////////////////////////////////////////
//
//  Movement
//
//

void CEdit::WordLeft()
{
	m_Selection.Move( CSelection::eLeft, CSelection::eWord, TRUE );
}

void CEdit::WordEndLeft()
{
	m_Selection.Move( CSelection::eLeft, CSelection::eWordEnd, TRUE );
}

void CEdit::WordRight()
{
	m_Selection.Move( CSelection::eRight, CSelection::eWord, TRUE );
}

void CEdit::WordEndRight()
{
	m_Selection.Move( CSelection::eRight, CSelection::eWordEnd, TRUE );
}

void CEdit::WindowStart()
{
	m_Selection.Move( CSelection::eUp, CSelection::eWindow, TRUE );
}

void CEdit::WindowRightEdge()
{
	m_Selection.Move( CSelection::eRight, CSelection::eWindow, TRUE );
}

void CEdit::WindowLeftEdge()
{
	m_Selection.Move( CSelection::eLeft, CSelection::eWindow, TRUE );
}

void CEdit::WindowEnd()
{
	m_Selection.Move( CSelection::eDown, CSelection::eWindow, TRUE );
}

void CEdit::SentenceRight()
{
	m_Selection.Move( CSelection::eRight, CSelection::eSentence, TRUE );
}

void CEdit::SentenceLeft()
{
	m_Selection.Move( CSelection::eLeft, CSelection::eSentence, TRUE );
}

void CEdit::ParaUp()
{
	SentenceLeft();
}

void CEdit::ParaDown()
{
	SentenceRight();
}

void CEdit::PageUp()
{
	int nTopOffset = m_Selection.GetEndRow() - m_pActiveView->GetTopIndex();
	m_Selection.Move( CSelection::eUp, CSelection::ePage, FALSE );
	int nRow = m_Selection.GetEndRow();
	int nTopIndex = nRow - nTopOffset;
	if ( nTopIndex < 0 )
	{
		nTopIndex = 0;
	}
	m_pActiveView->ScrollTo( m_pActiveView->GetLeftIndex(), nTopIndex );
	m_Selection.EnsureVisible( TRUE );
}

void CEdit::PageDown()
{
	int nTopOffset = m_Selection.GetEndRow() - m_pActiveView->GetTopIndex();
	m_Selection.Move( CSelection::eDown, CSelection::ePage, FALSE );
	int nRow = m_Selection.GetEndRow();
	// don't scroll unless we must
	if ( nRow > m_pActiveView->GetBottomIndex( TRUE ) )
	{
		int nTopIndex = nRow - nTopOffset;
		if ( nTopIndex < 0 )
		{
			nTopIndex = 0;
		}
		m_pActiveView->ScrollTo( m_pActiveView->GetLeftIndex(), nTopIndex );
	}
	m_Selection.EnsureVisible( TRUE );
}

void CEdit::LineUp()
{
	if ( m_Selection.IsEmpty() )
	{
		m_Selection.Move( CSelection::eUp, CSelection::eChar, TRUE );
	}
	else
	{
		int nStartCol, nStartRow, nEndCol, nEndRow;
		m_Selection.GetNormalizedBufferSelection( nStartCol, nStartRow, nEndCol, nEndRow );
		nStartRow--;
		m_Selection.SetEmptySelection( nStartCol, max( 0, nStartRow ) );
		m_Selection.EnsureVisible( TRUE );
	}
}

void CEdit::LineStart()
{
	m_Selection.Move( CSelection::eLeft, CSelection::eSmartAll, TRUE );
}

void CEdit::LineEnd()
{
	m_Selection.Move( CSelection::eRight, CSelection::eSmartAll, TRUE );
}

void CEdit::LineDown()
{
	if ( m_Selection.IsEmpty() )
	{
		m_Selection.Move( CSelection::eDown, CSelection::eChar, TRUE );
	}
	else
	{
		int nStartCol, nStartRow, nEndCol, nEndRow;
		m_Selection.GetNormalizedBufferSelection( nStartCol, nStartRow, nEndCol, nEndRow );
		nEndRow++;
		int nLastLine = m_Buffer.GetLineCount() - 1;
		m_Selection.SetEmptySelection( nEndCol, min( nLastLine, nEndRow ) );
		m_Selection.EnsureVisible( TRUE );
	}
}

void CEdit::Home()
{
	m_Selection.Move( CSelection::eLeft, CSelection::eSmartAll, TRUE );
}

void CEdit::End()
{
	m_Selection.Move( CSelection::eRight, CSelection::eSmartAll, TRUE );
}

void CEdit::DocumentStart()
{
	m_Selection.Move( CSelection::eUp, CSelection::eAll, FALSE );
	m_Selection.Move( CSelection::eLeft, CSelection::eAll, TRUE );
}

void CEdit::DocumentEnd()
{
	CHourGlass wait( this );
	m_Selection.Move( CSelection::eDown, CSelection::eAll, FALSE );
	m_Selection.Move( CSelection::eRight, CSelection::eAll, TRUE );
}

void CEdit::CharRight()
{
	if ( m_Selection.IsEmpty() )
	{
		int nRow = m_Selection.GetEndRow();
		if ( BoundSelection() &&
		     m_Buffer.GetLineCount() &&
			 m_Selection.GetEndCol() >= m_Buffer.GetLineLength( nRow ) )
			{
			// wrap to the start of the next line
			if ( nRow < m_Buffer.GetLineCount() - 1 )
				m_Selection.SetEmptySelection( 0, nRow + 1, TRUE );
			}
		else
			m_Selection.Move( CSelection::eRight, CSelection::eChar, TRUE );
	}
	else
	{
		int nStartCol, nStartRow, nEndCol, nEndRow;
		m_Selection.GetNormalizedBufferSelection( nStartCol, nStartRow, nEndCol, nEndRow );
		m_Selection.SetEmptySelection( nEndCol, nEndRow );
	}
}

void CEdit::CharLeft()
{
	if ( m_Selection.IsEmpty() )
	{
		if ( BoundSelection() && m_Selection.GetEndCol() == 0 )
			{
			// wrap to the end of the previous line
			int nRow = m_Selection.GetEndRow();
			if ( nRow )
				m_Selection.SetEmptySelection( m_Buffer.GetLineLength( nRow - 1 ), nRow - 1, TRUE );
			}
		else
			m_Selection.Move( CSelection::eLeft, CSelection::eChar, TRUE );
	}
	else
	{
		int nStartCol, nStartRow, nEndCol, nEndRow;
		m_Selection.GetNormalizedBufferSelection( nStartCol, nStartRow, nEndCol, nEndRow );
		m_Selection.SetEmptySelection( nStartCol, nStartRow );
	}
}

void CEdit::GoToLine( int nLine )
{
	if ( IsPlayingMacro() )
	{
		DWORD dwLine;
		GetMacroData( ( LPBYTE ) &dwLine, sizeof( dwLine ) );
		nLine = dwLine;
	}
	else
	{
		// ask user where to go
		if ( !LineIsValid( nLine ) &&
			 PromptUserForLineNumber( this, GetDlgParent(), nLine ) )
		{
			if ( IsRecordingMacro() )
			{
				DWORD dwLine = nLine;
				AddMacroData( ( LPBYTE ) &dwLine, sizeof( dwLine ) );
			}
		}
	}

	if ( LineIsValid( nLine ) )
	{
		// make the requested line visible, centered vertically in the active view
		ASSERT( m_pActiveView );
		GoToLine( nLine, TRUE );
	}
}

void CEdit::GoToLine( int nLine, BOOL bVCenter )
{
	int nTemp = m_Buffer.GetLineCount() - 1;
	nLine = min( nTemp, nLine );

	m_Selection.SetEmptySelection( 0, nLine );

	if ( bVCenter )
	{
		int nTopRow = nLine - ( m_pActiveView->GetBottomIndex( TRUE ) - m_pActiveView->GetTopIndex() ) / 2;
		nTopRow = max( 0, nTopRow );
		m_pActiveView->ScrollTo( 0, nTopRow );
		m_Selection.UpdateCaretPosition();
	}
	else
	{
		m_Selection.EnsureVisible( TRUE );
	}
}

void CEdit::GoToIndentation()
{
	if ( !m_Buffer.IsEmpty() )
	{
		int nLine = m_Selection.GetEndRow();
		LPCTSTR pszLine = m_Buffer.GetLineText( nLine );
		LPCTSTR psz = pszLine;
		while ( *psz && is_space( *psz ) )
		{
			psz++;
		}

		m_Selection.SetEmptySelection( psz - pszLine, nLine, TRUE );
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//  Extended Selection
//
//

void CEdit::WordRightExtend()
{
	m_Selection.Extend( CSelection::eRight, CSelection::eWord, TRUE, TRUE, FALSE );
}

void CEdit::WordEndRightExtend()
{
	m_Selection.Extend( CSelection::eRight, CSelection::eWordEnd, TRUE, TRUE, FALSE );
}

void CEdit::WordLeftExtend()
{
	m_Selection.Extend( CSelection::eLeft, CSelection::eWord, TRUE, TRUE, FALSE );
}

void CEdit::WordEndLeftExtend()
{
	m_Selection.Extend( CSelection::eLeft, CSelection::eWordEnd, TRUE, TRUE, FALSE );
}

void CEdit::PageUpExtend()
{
	int nTopOffset = m_Selection.GetEndRow() - m_pActiveView->GetTopIndex();
	m_Selection.Extend( CSelection::eUp, CSelection::ePage, FALSE, TRUE, FALSE );
	int nRow = m_Selection.GetEndRow();
	int nTopIndex = nRow - nTopOffset;
	if ( nTopIndex < 0 )
	{
		nTopIndex = 0;
	}
	m_pActiveView->ScrollTo( m_pActiveView->GetLeftIndex(), nTopIndex );
	m_Selection.EnsureVisible( TRUE );
 }

void CEdit::PageDownExtend()
{
	int nTopOffset = m_Selection.GetEndRow() - m_pActiveView->GetTopIndex();
	m_Selection.Extend( CSelection::eDown, CSelection::ePage, FALSE, TRUE, FALSE );
	int nRow = m_Selection.GetEndRow();
	// don't scroll unless we must
	if ( nRow > m_pActiveView->GetBottomIndex( TRUE ) )
	{
		int nTopIndex = nRow - nTopOffset;
		if ( nTopIndex < 0 )
		{
			nTopIndex = 0;
		}
		m_pActiveView->ScrollTo( m_pActiveView->GetLeftIndex(), nTopIndex );
	}
	m_Selection.EnsureVisible( TRUE );
}

void CEdit::LineUpExtend()
{
	m_Selection.Extend( CSelection::eUp, CSelection::eChar, TRUE, TRUE, FALSE );
}

void CEdit::LineEndExtend()
{
	m_Selection.Extend( CSelection::eRight, CSelection::eSmartAll, TRUE, TRUE, FALSE );
}

void CEdit::LineDownExtend()
{
	m_Selection.Extend( CSelection::eDown, CSelection::eChar, TRUE, TRUE, FALSE );
}

void CEdit::HomeExtend()
{
	m_Selection.Extend( CSelection::eLeft, CSelection::eSmartAll, TRUE, TRUE, FALSE );
}

void CEdit::DocumentStartExtend()
{
	m_Selection.Extend( CSelection::eUp, CSelection::eSmartAll, FALSE, TRUE, FALSE );
	m_Selection.Extend( CSelection::eLeft, CSelection::eSmartAll, TRUE, TRUE, FALSE );
}

void CEdit::DocumentEndExtend()
{
	m_Selection.Extend( CSelection::eDown, CSelection::eSmartAll, FALSE, TRUE, FALSE );
	m_Selection.Extend( CSelection::eRight, CSelection::eSmartAll, TRUE, TRUE, FALSE );
}

void CEdit::CharRightExtend()
{
	m_Selection.Extend( CSelection::eRight, CSelection::eChar, TRUE, TRUE, FALSE );
}

void CEdit::CharLeftExtend()
{
	m_Selection.Extend( CSelection::eLeft, CSelection::eChar, TRUE, TRUE, FALSE );
}

void CEdit::SelectSwapAnchor()
{
	int nStartCol, nStartRow, nEndCol, nEndRow;
	m_Selection.GetBufferSelection( nStartCol, nStartRow, nEndCol, nEndRow );
	m_Selection.SetExtendedSelection( nEndCol, nEndRow, nStartCol, nStartRow );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Text Deletion
//
//

void CEdit::WordDeleteToStart()
{
	m_Buffer.BeginEdit( m_Selection.GetEndRow(), m_Selection.GetEndCol() );
	m_Selection.Extend( CSelection::eLeft, CSelection::eWord, FALSE, FALSE, FALSE );
	DeleteSelection( FALSE, FALSE );
	m_Buffer.EndEdit( m_Selection.GetEndRow(), m_Selection.GetEndCol() );
	m_Selection.EnsureVisible( TRUE );
}

void CEdit::WordDeleteToEnd()
{
	m_Buffer.BeginEdit( m_Selection.GetEndRow(), m_Selection.GetEndCol() );
	m_Selection.Extend( CSelection::eRight, CSelection::eWordEnd, FALSE, FALSE, FALSE );
	DeleteSelection( FALSE, FALSE );
	m_Buffer.EndEdit( m_Selection.GetEndRow(), m_Selection.GetEndCol() );
	m_Selection.EnsureVisible( TRUE );
}

void CEdit::LineDeleteToStart()
{
	m_Selection.Extend( CSelection::eLeft, CSelection::eSmartAll, FALSE, FALSE, FALSE );
	DeleteSelection( FALSE, FALSE );
}

void CEdit::LineDeleteToEnd()
{
	m_Selection.Extend( CSelection::eRight, CSelection::eSmartAll, FALSE, FALSE, FALSE );
	DeleteSelection( FALSE, FALSE );
}

void CEdit::LineDelete( BOOL bCopy )
{
	int nLineCount = m_Buffer.GetLineCount();
	if ( nLineCount )
	{
		int nLine = m_Selection.GetEndRow();
		if ( nLine >= nLineCount - 1 )
		{
			m_Selection.SetExtendedSelection( 0, nLine, m_Buffer.GetLineLength( nLine ), nLine, FALSE );
		}
		else
		{
			m_Selection.SetExtendedSelection( 0, nLine, 0, nLine + 1, FALSE );
		}
		if ( bCopy )
			{
			Copy( FALSE,  FALSE );
			}
		m_Buffer.BeginEdit( nLine, m_Selection.GetEndCol() );
		m_Buffer.RemoveLines( nLine, 1 );
		m_Selection.SetEmptySelection( 0, nLine );
		m_Buffer.EndEdit( nLine, 0 );
	}
}

void CEdit::DeleteHorizontalSpace()
{
	if ( !m_Buffer.IsEmpty() )
	{
		int nLine = m_Selection.GetEndRow();
		int nStartCol = m_Selection.GetEndCol();
		LPCTSTR pszLine = m_Buffer.GetLineText( nLine );
		int nLineLen = m_Buffer.GetLineLength( nLine );
		if ( ( nStartCol && is_space( pszLine[ nStartCol - 1 ] ) ) ||
		     ( ( nStartCol < nLineLen ) && is_space( pszLine[ nStartCol ] ) ) )
		{
			int nEndCol = nStartCol;
			while ( nStartCol > 0 && is_space( pszLine[ nStartCol - 1 ] ) )
			{
				nStartCol--;
			}
			while ( nEndCol < nLineLen && is_space( pszLine[ nEndCol ] ) )
			{
				nEndCol++;
			}
			if ( nEndCol != nStartCol )
			{
				m_Selection.SetExtendedSelection( nStartCol, nLine, nEndCol, nLine );
				DeleteSelection( FALSE, FALSE );
			}
		}
	}
}

void CEdit::DeleteBlankLines()
{
	int nLine = m_Selection.GetEndRow();
	if ( !m_Buffer.IsEmpty() )
	{
		int nRemoveLines = 0;
		int nTemp = nLine;
		while ( nTemp >= 0 && m_Buffer.LineIsEmpty( nTemp, TRUE ) )
		{
			nLine = nTemp;
			nTemp--;
			nRemoveLines++;
		}
		int nStartLine = nLine;
		nLine += nRemoveLines;

		int nLineCount = m_Buffer.GetLineCount();
		while ( nLine < nLineCount && m_Buffer.LineIsEmpty( nLine, TRUE ) )
		{
			nLine++;
			nRemoveLines++;
		}

		if ( nRemoveLines )
		{
			CEditTransaction trans( this );
			m_Buffer.RemoveLines( nStartLine, nRemoveLines );
			m_Selection.SetEmptySelection( 0, nStartLine, TRUE );
		}
	}
}

void CEdit::DeleteBack()
{
	if ( m_Selection.IsColumnSel() )
	{
		///////////////////////////////////////////////////////
		// Special multi-line char deletion mode
		///////////////////////////////////////////////////////
		CDelayRepaint delay( this );
		CHourGlass wait( this );
		m_Buffer.BeginEdit( m_Selection.GetEndRow(), m_Selection.GetEndCol() );

		int nStartCol, nStartRow, nEndCol, nEndRow;
		m_Selection.GetNormalizedColumnSelection( nStartCol, nStartRow, nEndCol, nEndRow );

		int nStartBuffCol = 0, nEndBuffCol = 0;
		BOOL bDeleted = FALSE;

		if ( nStartCol == nEndCol )
		{
			if ( nStartCol )
			{
				nStartBuffCol = m_Buffer.ConvertViewColToBufferCol( nStartRow, nStartCol ) - 1;
				nEndBuffCol = m_Buffer.ConvertViewColToBufferCol( nEndRow, nStartCol ) - 1;
				m_Selection.SetExtendedSelection( nStartBuffCol, nStartRow, nEndBuffCol, nEndRow, FALSE );
				DeleteSelection( TRUE, FALSE );
				bDeleted = TRUE;
			}
		}
		else
		{
			DeleteSelection( FALSE, FALSE );
			nStartBuffCol = m_Buffer.ConvertViewColToBufferCol( nStartRow, nStartCol );
			nEndBuffCol = m_Buffer.ConvertViewColToBufferCol( nEndRow, nStartCol );
			bDeleted = TRUE;
		}

		if ( bDeleted )
		{
			m_Selection.EnableColumnSel( TRUE );
			m_Selection.SetExtendedSelection( nStartBuffCol, nStartRow, nEndBuffCol, nEndRow, TRUE );
		}

		m_Buffer.EndEdit( m_Selection.GetEndRow(), m_Selection.GetEndCol() );
	}
	else
	{
		if ( m_Selection.IsEmpty() )
		{
			int nStartRow = m_Selection.GetStartRow();
			int nStartCol = m_Selection.GetStartCol();
			if ( nStartRow || nStartCol )
			{
				m_Buffer.BeginEdit( nStartRow, nStartCol );
				m_Selection.Extend( CSelection::eLeft, CSelection::eChar, FALSE, FALSE, TRUE );
				DeleteSelection( TRUE, TRUE );
				m_Buffer.EndEdit( m_Selection.GetStartRow(), m_Selection.GetStartCol() );
			}
		}
		else
		{
			DeleteSelection( FALSE, FALSE );
		}
	}
	m_Selection.EnsureVisible( TRUE );
}

void CEdit::Delete()
{
	if ( m_Selection.IsColumnSel() )
	{
		///////////////////////////////////////////////////////
		// Special multi-line char deletion mode
		///////////////////////////////////////////////////////
		CDelayRepaint delay( this );
		CHourGlass wait( this );
		m_Buffer.BeginEdit( m_Selection.GetEndRow(), m_Selection.GetEndCol() );

		int nStartCol, nStartRow, nEndCol, nEndRow;
		m_Selection.GetNormalizedColumnSelection( nStartCol, nStartRow, nEndCol, nEndRow );

		DeleteSelection( TRUE, TRUE );

		m_Selection.EnableColumnSel( TRUE );
		m_Selection.SetExtendedSelection( m_Buffer.ConvertViewColToBufferCol( nStartRow, nStartCol ), nStartRow,
		                                  m_Buffer.ConvertViewColToBufferCol( nEndRow, nStartCol ), nEndRow,
										  TRUE );

		m_Buffer.EndEdit( m_Selection.GetEndRow(), m_Selection.GetEndCol() );
	}
	else
	{
		DeleteSelection( TRUE, TRUE );
	}

	m_Selection.EnsureVisible( TRUE );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Scrolling
//
//

void CEdit::WindowScrollToTop()
{
	int nCurrLine = m_Selection.GetEndRow();
	BOOL bSmooth = ( nCurrLine > m_pActiveView->GetTopIndex() && nCurrLine <= m_pActiveView->GetBottomIndex( TRUE ) );
	m_pActiveView->ScrollTo( m_pActiveView->GetLeftIndex(), m_Selection.GetEndRow(), bSmooth );
	m_Selection.EnsureVisible( TRUE, FALSE, TRUE );
}

void CEdit::WindowScrollToCenter()
{
	if ( !m_Buffer.IsEmpty() )
	{
		GoToLine( m_Selection.GetEndRow() + 1 );  // GoToLine() is one-based
		m_Selection.EnsureVisible( TRUE, FALSE, TRUE );
	}
}

void CEdit::WindowScrollToBottom()
{
	int nVisLines = m_pActiveView->GetBottomIndex( TRUE ) - m_pActiveView->GetTopIndex();
	int nCurrLine = m_Selection.GetEndRow();
	if ( nCurrLine >= nVisLines )
	{
		BOOL bSmooth = ( nCurrLine > m_pActiveView->GetTopIndex() && nCurrLine <= m_pActiveView->GetBottomIndex( TRUE ) );
		m_pActiveView->ScrollTo( m_pActiveView->GetLeftIndex(), nCurrLine - nVisLines, bSmooth );
		m_Selection.EnsureVisible( TRUE, FALSE, TRUE );
	}
}

void CEdit::WindowScrollUp()
{
	GetActiveView()->Scroll( 0, +1 );
	m_Selection.EnsureVisible( FALSE, FALSE, TRUE );
}

void CEdit::WindowScrollDown()
{
	GetActiveView()->Scroll( 0, -1 );
	m_Selection.EnsureVisible( FALSE, FALSE, TRUE );
}

void CEdit::WindowScrollLeft()
{
	m_pActiveView->Scroll( -1, 0 );
	m_Selection.EnsureVisible( FALSE, FALSE, TRUE );
}

void CEdit::WindowScrollRight()
{
	m_pActiveView->Scroll( +1, 0 );
	m_Selection.EnsureVisible( FALSE, FALSE, TRUE );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Tab/space conversion
//
//

void CEdit::UntabifySelection()
{
	if ( !m_Selection.IsEmpty() )
	{
		CEditTransaction trans( this );
		int nStartCol, nEndCol, nStartRow, nEndRow;
		m_Selection.GetNormalizedBufferSelection( nStartCol, nStartRow, nEndCol, nEndRow );
		LPTSTR pszBuff = new TCHAR[ CEditView::MAXCOL ];
		int nLastColOut = CEditView::MAXCOL - 1;
		int cbTabSize = m_Buffer.GetTabSize();
		for ( int nLine = nStartRow; nLine <= nEndRow; nLine++ )
		{
			BOOL bLineChanged = FALSE;
			int nColIn = ( nLine == nStartRow ? nStartCol : 0 );
			int nViewCol = m_Buffer.ConvertBufferColToViewCol( nLine, nColIn );
			int nColOut = nColIn;
			LPCTSTR pszLine = m_Buffer.GetLineText( nLine );
			if ( nColIn )
			{
				_tcsncpy( pszBuff, pszLine, nColIn );
			}
			int cbLine = m_Buffer.GetLineLength( nLine );
			int nLastColIn = ( nLine == nEndRow ? nEndCol : cbLine );
			LPCTSTR pszIn = pszLine + nColIn;
			LPTSTR pszOut = pszBuff + nColOut;
			while ( nColOut < nLastColOut )
			{
				if ( !*pszIn )
				{
					*pszOut = _T('\0');
					break;
				}
				if ( ( nColIn < nLastColIn ) && ( *pszIn == _T('\t') ) )
				{
					int nOldViewCol = nViewCol;
					nViewCol = ( ( ( nViewCol + cbTabSize ) / cbTabSize ) * cbTabSize );
					int cbFill = nViewCol - nOldViewCol;
					ASSERT( cbFill > 0 );
					bLineChanged = TRUE;
					while ( cbFill )
					{
						*pszOut++ = _T(' ');
						nColOut++;
						cbFill--;
					}
					pszIn++;
					nColIn++;
				}
				else
				{
					nColIn++;
					nColOut++;
					nViewCol++;
					*pszOut++ = *pszIn++;
				}
			}

			if ( bLineChanged )
			{
				m_Buffer.RemoveCharsFromLine( nLine, 0, cbLine );
				m_Buffer.InsertCharsIntoLine( nLine, 0, pszBuff, -1 );
			}
		}
		delete pszBuff;
		m_Selection.SetEmptySelection( nStartCol, nStartRow, TRUE );
	}
}

void CEdit::TabifySelection()
{
	if ( !m_Selection.IsEmpty() )
	{
		CEditTransaction trans( this );
		int nStartCol, nEndCol, nStartRow, nEndRow;
		m_Selection.GetNormalizedBufferSelection( nStartCol, nStartRow, nEndCol, nEndRow );
		LPTSTR pszBuff = new TCHAR[ CEditView::MAXCOL ];
		int cbTabSize = m_Buffer.GetTabSize();
		int nLastColOut = CEditView::MAXCOL - 1;
		for ( int nLine = nStartRow; nLine <= nEndRow; nLine++ )
		{
			BOOL bLineChanged = FALSE;
			int nColIn = ( nLine == nStartRow ? nStartCol : 0 );
			int nViewCol = m_Buffer.ConvertBufferColToViewCol( nLine, nColIn );
			int nColOut = nColIn;
			LPCTSTR pszLine = m_Buffer.GetLineText( nLine );
			if ( nColIn )
			{
				_tcsncpy( pszBuff, pszLine, nColIn );
			}
			int cbLine = m_Buffer.GetLineLength( nLine );
			int nLastColIn = ( nLine == nEndRow ? nEndCol : cbLine );
			LPCTSTR pszIn = pszLine + nColIn;
			LPTSTR pszOut = pszBuff + nColOut;
			while ( nColOut < nLastColOut )
			{
				if ( !*pszIn )
				{
					*pszOut = _T('\0');
					break;
				}
				if ( nColIn < nLastColIn )
				{
					if ( *pszIn == _T(' ') )
					{
						int nSpaces = 1;
						while ( pszIn[ nSpaces ] == _T(' ') && 
						        ( ( nColIn + nSpaces ) < nLastColIn ) )
						{
							nSpaces++;
						}

						// 
						if ( nSpaces < cbTabSize && pszIn[ nSpaces ] == '\t' )
						{
							int nViewColNext = nViewCol + nSpaces;
							nViewColNext = ( ( ( nViewColNext + cbTabSize ) / cbTabSize ) * cbTabSize );

							nColIn += nSpaces;
							pszIn += nSpaces;
							if ( nViewColNext - nViewCol > cbTabSize )
							{
								bLineChanged = TRUE;
								nViewCol = ( ( ( nViewCol + cbTabSize ) / cbTabSize ) * cbTabSize );
								nColOut++;
								*pszOut++ = _T('\t');
							}
							continue;
						}

						if ( nSpaces >= cbTabSize )
						{
							int nTemp = nViewCol + nSpaces;
							int nClosestTab = ( ( nTemp / cbTabSize ) * cbTabSize );
							int cbTab = ( nClosestTab - nViewCol ) / cbTabSize + ( nViewCol % cbTabSize == 0 ? 0 : 1 );
							int cbFill = nTemp - nClosestTab;

							if ( cbTab )
							{
								bLineChanged = TRUE;
								while ( cbTab )
								{
									*pszOut++ = _T('\t');
									nColOut++;
									cbTab--;
								}
							}
							if ( cbFill )
							{
								bLineChanged = TRUE;
								while ( cbFill )
								{
									*pszOut++ = _T(' ');
									nColOut++;
									cbFill--;
								}
							}
							pszIn += nSpaces;
							nColIn += nSpaces;
							nViewCol = nTemp;
						}
						else
						{
							pszIn += nSpaces;
							nColIn += nSpaces;
							nColOut += nSpaces;
							nViewCol += nSpaces;
							while ( nSpaces )
							{
								*pszOut++ = _T(' ');
								nSpaces--;
							}
						}
					}
					else
					{
						if ( *pszIn == _T('\t') )
						{
							nViewCol = ( ( ( nViewCol + cbTabSize ) / cbTabSize ) * cbTabSize );
							nColIn++;
							nColOut++;
							*pszOut++ = *pszIn++;
						}
						else
						{
							goto skip;
						}
					}
				}
				else
				{
					skip:

					nColIn++;
					nColOut++;
					nViewCol++;
					*pszOut++ = *pszIn++;
				}
			}

			if ( bLineChanged )
			{
				m_Buffer.RemoveCharsFromLine( nLine, 0, cbLine );
				m_Buffer.InsertCharsIntoLine( nLine, 0, pszBuff, -1 );
			}
		}
		delete pszBuff;
		m_Selection.SetEmptySelection( nStartCol, nStartRow, TRUE );
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//  Indentation
//
//

void CEdit::UnindentSelection()
{
	// unindent with a column selection is a nop.
	if ( m_Selection.IsColumnSel() )
	{
		NotifyParentOfCmdFailure( CMDERR_SELECTION );
		return;
	}

	int nStartCol, nStartRow, nEndCol, nEndRow;
	m_Selection.GetBufferSelection( nStartCol, nStartRow, nEndCol, nEndRow );
	int cbTab = m_Buffer.GetTabSize();

	if ( nStartRow != nEndRow )
	{
		// selection extends over multiple rows.  Remove one leading tab at the start of each line
		CDelayRepaint delay( this );
		int nOrigEndRow = nEndRow;
		int nOrigEndCol = nEndCol;
		m_Buffer.BeginEdit( nOrigEndRow, nOrigEndCol );

		BOOL bStartIsAtEnd = FALSE;
		if ( nStartCol )
		{
			// Include entire row
			nStartCol = 0;
			if ( nStartRow > nEndRow )
			{
				nStartCol = m_Buffer.GetLineLength( nStartRow );
				bStartIsAtEnd = TRUE;
			}
		}

		int nFirstRow = min( nEndRow, nStartRow );
		int nLineCount = m_Buffer.GetLineCount();

		BOOL bEndIsAtEnd = FALSE;
		if ( nEndCol )
		{
			// Include entire row
			nEndCol = 0;
			if ( nEndRow > nStartRow )
			{
				if ( ++nEndRow >= nLineCount )
				{
					nEndRow--;
					nEndCol = m_Buffer.GetLineLength( nEndRow );
					bEndIsAtEnd = TRUE;
				}
			}
		}
		if ( nStartCol )
		{
			// Include entire row
			nStartCol = ( nStartRow < nEndRow ) ? 0 : m_Buffer.GetLineLength( nStartRow );
		}

		int nLastRow = max( nEndRow, nStartRow );
		if ( ( nStartRow < nEndRow ) && ( nEndCol == 0 ) ||
		     ( nEndRow < nStartRow ) && ( nStartCol == 0 ) )
		{
			// if the last line in the selection has no chars selected, then skip the line
			nLastRow--;
		}

		// remove the tabs
		for ( int i = nFirstRow; i <= nLastRow; i++ )
		{
			LPCTSTR pszLine = m_Buffer.GetLineText( i );
			int nChars = 0;
			if ( *pszLine == _T('\t') )
			{
				nChars = 1;
			}
			else if ( *pszLine == _T(' ') )
			{
				ASSERT( cbTab > 1 );
				do
				{
					nChars++;
					pszLine++;
				} while ( ( nChars < cbTab ) && ( *pszLine == _T(' ') ) );
			}
			if ( nChars )
			{
				m_Selection.SetExtendedSelection( 0, i, nChars, i );
				DeleteSelection( FALSE, FALSE );
			}
		}

		if ( bStartIsAtEnd )
		{
			nStartCol = m_Buffer.GetLineLength( nStartRow );
		}
		if ( bEndIsAtEnd )
		{
			nEndCol = m_Buffer.GetLineLength( nEndRow );
		}
		m_Selection.SetExtendedSelection( nStartCol, nStartRow, nEndCol, nEndRow );

		m_Buffer.EndEdit( nOrigEndRow, nOrigEndCol );
	}
	else
	{
		// move backward one tab
		int nEndRow = m_Selection.GetEndRow();
		int nEndCol = m_Selection.GetEndViewCol();
		if ( nEndCol > 0 )
		{
			nEndCol = ( ( ( nEndCol - 1 ) / cbTab ) * cbTab );
			nEndCol = m_Buffer.ConvertViewColToBufferCol( nEndRow, nEndCol );
		}
		m_Selection.SetEmptySelection( nEndCol, nEndRow, TRUE );
	}
}

void CEdit::IndentSelection()
{
	int nStartCol, nStartRow, nEndCol, nEndRow;
	m_Selection.GetBufferSelection( nStartCol, nStartRow, nEndCol, nEndRow );

	if ( m_Selection.IsColumnSel() || ( nStartRow == nEndRow ) )
	{
		// just replace (if extended sel) or insert a tab at the current location
		InsertCharLowLevel( _T("\t") );
	}
	else
	{
		// selection extends over multiple rows.  Insert a tab at the start of each line
		CDelayRepaint delay( this );
		int nOrigEndRow = nEndRow;
		int nOrigEndCol = nEndCol;
		m_Buffer.BeginEdit( nOrigEndRow, nOrigEndCol );

		BOOL bStartIsAtEnd = FALSE;
		if ( nStartCol )
		{
			// Include entire row
			nStartCol = 0;
			if ( nStartRow > nEndRow )
			{
				nStartCol = m_Buffer.GetLineLength( nStartRow );
				bStartIsAtEnd = TRUE;
			}
		}

		int nFirstRow = min( nEndRow, nStartRow );
		int nLineCount = m_Buffer.GetLineCount();

		BOOL bEndIsAtEnd = FALSE;
		if ( nEndCol )
		{
			// Include entire row
			nEndCol = 0;
			if ( nEndRow > nStartRow )
			{
				if ( ++nEndRow >= nLineCount )
				{
					nEndRow--;
					nEndCol = m_Buffer.GetLineLength( nEndRow );
					bEndIsAtEnd = TRUE;
				}
			}
		}

		int nLastRow = max( nEndRow, nStartRow );
		if ( ( nStartRow < nEndRow ) && ( nEndCol == 0 ) ||
		     ( nEndRow < nStartRow ) && ( nStartCol == 0 ) )
		{
			// if the last line in the selection has no chars selected, then skip the line
			nLastRow--;
		}

		// insert the tabs
		if ( m_bExpandTabs )
		{
			int cbTabSize = m_Buffer.GetTabSize();
			LPTSTR pszSpaces = new TCHAR[ cbTabSize + 1 ];
			CharFill( pszSpaces, _T(' '), cbTabSize );
			pszSpaces[ cbTabSize ] = _T('\0');
			for ( int i = nFirstRow; i <= nLastRow; i++ )
			{
				m_Selection.SetEmptySelection( 0, i );
				ReplaceSelection( pszSpaces, FALSE );
			}
			delete pszSpaces;
		}
		else
		{
			for ( int i = nFirstRow; i <= nLastRow; i++ )
			{
				m_Selection.SetEmptySelection( 0, i );
				ReplaceSelection( _T("\t"), FALSE );
			}
		}

		if ( bStartIsAtEnd )
		{
			nStartCol = m_Buffer.GetLineLength( nStartRow );
		}
		if ( bEndIsAtEnd )
		{
			nEndCol = m_Buffer.GetLineLength( nEndRow );
		}
		m_Selection.SetExtendedSelection( nStartCol, nStartRow, nEndCol, nEndRow );

		m_Buffer.EndEdit( nOrigEndRow, nOrigEndCol );
	}
}

void CEdit::IndentToPrev()
{
	int nRow2 = m_Selection.GetEndRow();
	if ( nRow2 > 0 )
	{
		CEditTransaction trans( this );
		int nRow1 = nRow2 - 1;
		int nViewCol2 = m_Selection.GetEndViewCol();
		int nViewCol1 = nViewCol2;
		int nBuffCol1 = m_Buffer.ConvertViewColToBufferCol( nRow1, nViewCol1 );
		int nBuffCol2 = m_Selection.GetEndCol();
		int nLen2 = m_Buffer.GetLineLength( nRow2 );
		nBuffCol2 = min( nBuffCol2, nLen2 );
		int nTemp = nBuffCol1;
		BOOL bAddTab = FALSE;
		int nLen1 = m_Buffer.GetLineLength( nRow1 );
		// scan ahead to the next tab location
		if ( nBuffCol1 < nLen1 )
		{
			LPCTSTR psz1 = m_Buffer.GetLineText( nRow1 ) + nBuffCol1;
			while ( *psz1 && !is_space( *psz1 ) )
			{
				psz1++;
				nBuffCol1++;
			}
			while ( *psz1 && is_space( *psz1 ) )
			{
				psz1++;
				nBuffCol1++;
			}
		}
		if ( nBuffCol1 >= nLen1 )
		{
			bAddTab = TRUE;
			nBuffCol1 = nTemp;
		}
		nViewCol2 = nViewCol1 = m_Buffer.ConvertBufferColToViewCol( nRow1, nBuffCol1 );
		int cbTabSize = m_Buffer.GetTabSize();
		if ( bAddTab )
		{
			nViewCol2 = ( ( ( nViewCol2 + cbTabSize ) / cbTabSize ) * cbTabSize );
		}

		// scan backwards to the start of the spacing preceding the cursor
		int nDelStartCol = nBuffCol2;
		if ( nBuffCol2 <= nLen2 )
		{
			LPCTSTR psz2 = m_Buffer.GetLineText( nRow2 ) + nDelStartCol;
			while ( nDelStartCol > 0 && is_space( *( psz2 - 1 ) ) )
			{
				nDelStartCol--;
				psz2--;
			}
		}

		// remove the old indentation
		if ( nDelStartCol != nBuffCol2 )
		{
			m_Buffer.RemoveCharsFromLine( nRow2, nDelStartCol, nBuffCol2 - nDelStartCol );
			nBuffCol2 = nDelStartCol;
		}

		// insert the new indentation
		int nViewCol2Start = m_Buffer.ConvertBufferColToViewCol( nRow2, nBuffCol2 );
		int nSpaces = nViewCol2 - nViewCol2Start;
		LPTSTR pszInsert = new TCHAR[ nSpaces + 1 ];
		LPTSTR pszOut = pszInsert;
		if ( nSpaces >= cbTabSize )
		{
			int nTemp = nViewCol2Start + nSpaces;
			int nClosestTab = ( ( nTemp / cbTabSize ) * cbTabSize );
			int cbTab = ( nClosestTab - nViewCol2Start ) / cbTabSize + ( nViewCol2Start % cbTabSize == 0 ? 0 : 1 );
			int cbFill = nTemp - nClosestTab;

			if ( cbTab )
			{
				while ( cbTab )
				{
					*pszOut++ = _T('\t');
					cbTab--;
				}
			}
			if ( cbFill )
			{
				while ( cbFill )
				{
					*pszOut++ = _T(' ');
					cbFill--;
				}
			}
		}
		else
		{
			while ( nSpaces )
			{
				*pszOut++ = _T(' ');
				nSpaces--;
			}
		}
		*pszOut = _T('\0');
		int cbInsert = _tcslen( pszInsert );
		m_Buffer.InsertCharsIntoLine( nRow2, nBuffCol2, pszInsert, cbInsert );
		m_Selection.SetEmptySelection( nBuffCol2 + cbInsert, nRow2, TRUE );
		delete pszInsert;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//  Undo/Redo
//
//

void CEdit::UndoChanges()
{
	CSaveSelection save( this );
	Undo();
}

void CEdit::Undo()
{
	if ( m_Buffer.CanUndo() )
	{
		CHourGlass wait( this );
		m_Buffer.Undo();
	}
	else
	{
		NotifyParentOfCmdFailure( CMDERR_EMPTYBUF );
	}
}

void CEdit::RedoChanges()
{
	CSaveSelection save( this );
	Redo();
}

void CEdit::Redo()
{
	if ( m_Buffer.CanRedo() )
	{
		CHourGlass wait( this );
		m_Buffer.Redo();
	}
	else
	{
		NotifyParentOfCmdFailure( CMDERR_EMPTYBUF );
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//  Clipboard
//
//

void CEdit::SentenceCut()
{
	m_Selection.Extend( CSelection::eOutward, CSelection::eSentence, FALSE, FALSE, FALSE );
	Cut( FALSE );
}

void CEdit::Paste()
{
	HGLOBAL hMem = GetClipboardText();
	if ( hMem )
	{
		LPCTSTR psz = ( LPCTSTR ) GlobalLock( hMem );
		ASSERT( psz );
		if ( psz && *psz )
		{
			if ( g_bCopyColumn )
			{
				// text probably was copied as a column, so paste it back as a column
				CDelayRepaint delay( this );
				CHourGlass wait( this );

				int nCol, nRow, nEndCol, nEndRow;
				m_Selection.GetNormalizedBufferSelection( nCol, nRow, nEndCol, nEndRow );

				m_Buffer.BeginEdit( nRow, nCol );

				DeleteSelection( FALSE, FALSE );

				m_Selection.SetEmptySelection( nCol, nRow );
				int nViewCol = m_Selection.GetStartViewCol();
				LPCTSTR pszLineStart = psz;
				for ( ;; )
				{
					if ( *psz == _T('\0') ||
						 *psz == _T('\r') && psz[1] == _T('\n') ||
						 *psz == _T('\n') )
					{
						// make sure line exists
						if ( nRow == m_Buffer.GetLineCount() )
						{
							m_Buffer.InsertLine( nRow, _T(""), 0 );
						}

						int nLineLen = m_Buffer.GetLineLength( nRow );
						int nBuffCol = m_Buffer.ConvertViewColToBufferCol( nRow, nViewCol );
						if ( nBuffCol > nLineLen )
						{
							// pad with spaces up to insertion point
							int cbPad = nBuffCol - nLineLen;
							LPTSTR pszFill = new TCHAR[ cbPad ];
							CharFill( pszFill, _T(' '), cbPad );
							m_Buffer.InsertCharsIntoLine( nRow, nLineLen, pszFill, cbPad );
							delete pszFill;
						}

						// paste the clipboard data for this line
						m_Buffer.InsertCharsIntoLine( nRow, nBuffCol, pszLineStart, psz - pszLineStart );

						if ( *psz == _T('\0') )
						{
							break;
						}
						else
						{
							nRow++;
							psz += ( *psz == _T('\r') ? 2 : 1 );
							pszLineStart = psz;
						}
					}
					else
					{
						psz++;
					}
				}

				m_Buffer.EndEdit( nRow, nCol );
			}
			else
			{
				// text was not copied as a column, so paste it back as normal block text
				ReplaceSelection( psz, FALSE );
			}
		}
		GlobalUnlock( hMem );
	}
}

void CEdit::LineCut()
{
	LineDelete( TRUE );
}

void CEdit::CutSelection()
{
	Cut( FALSE );
}

BOOL CEdit::CanCut( BOOL bCopyLineIfEmpty ) const
{
	return ( bCopyLineIfEmpty ||
	         ( !m_Buffer.IsReadOnly() && !m_Selection.IsEmpty() ) );
}

void CEdit::Cut( BOOL bCopyLineIfEmpty )
{
	if ( CanCut( bCopyLineIfEmpty ) )
	{
		Copy( bCopyLineIfEmpty, m_bAppendNextCut && !m_Selection.IsColumnSel() );
		m_bAppendNextCut = FALSE;
		DeleteSelection( FALSE, TRUE );
	}
	else
		NotifyParentOfCmdFailure( m_Buffer.IsReadOnly() ? CMDERR_READONLY : CMDERR_SELECTION );  // some kind of error
}

void CEdit::Copy( BOOL bCopyLineIfEmpty, BOOL bAppend )
{
	CopySelection( bCopyLineIfEmpty, bAppend );
}

void CEdit::AppendNextCut()
{
	m_bAppendNextCut = TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Select
//
//

void CEdit::SelectPara()
{
	m_Selection.Extend( CSelection::eOutward, CSelection::eParagraph, TRUE, TRUE, FALSE );
}

void CEdit::SelectLine()
{
	m_Selection.Move( CSelection::eLeft, CSelection::eSmartAll, FALSE );
	m_Selection.Extend( CSelection::eRight, CSelection::eSmartAll, TRUE, TRUE, FALSE );
}

void CEdit::SelectAll()
{
	DocumentStart();
	DocumentEndExtend();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Insertion
//
//

void CEdit::LineOpenBelow()
{
	CEditTransaction trans( this );
	if ( !m_Buffer.GetLineCount() )
	{
		m_Buffer.InsertLine( 0, _T(""), 0 );
	}
	m_Buffer.InsertLine( m_Selection.GetEndRow() + 1, _T(""), 0 );
}

void CEdit::LineOpenAbove()
{
	CEditTransaction trans( this );
	int nLine = m_Selection.GetEndRow();
	m_Buffer.InsertLine( nLine, _T(""), 0 );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Brackets & Braces
//
//
void CEdit::GoToMatchBrace()
{
	DWORD dwError = CMDERR_EMPTYBUF;
	if ( !m_Buffer.IsEmpty() )
	{
		int nRow, nCol, nDontCare;
		m_Selection.GetNormalizedBufferSelection( nCol, nRow, nDontCare, nDontCare );
		if ( LineIsValid( nRow ) )
		{
			if ( nCol <= m_Buffer.GetLineLength( nRow ) )
			{
				TCHAR chFind = _T('\0');
				TCHAR chFindPair = _T('\0');
				BOOL bForward = TRUE;
				for ( int i = 0; !chFind && i < 2; i++ )
				{
					LPCTSTR pszLine = m_Buffer.GetLineText( nRow );
					LPCTSTR pszChar = pszLine + nCol;
					switch ( *pszChar )
					{
						case _T('['):	chFindPair = _T('['); chFind = _T(']'); break;
						case _T('{'):	chFindPair = _T('{'); chFind = _T('}'); break;
						case _T('('):	chFindPair = _T('('); chFind = _T(')'); break;
						case _T(']'):	chFindPair = _T(']'); chFind = _T('['); bForward = FALSE; break;
						case _T('}'):	chFindPair = _T('}'); chFind = _T('{'); bForward = FALSE; break;
						case _T(')'):	chFindPair = _T(')'); chFind = _T('('); bForward = FALSE; break;
					}

					if ( !chFind )
					{
						if ( nCol <= 0 )
							break;
						else
							nCol -= _tclen_prev( pszLine, pszChar );
					}
				}

				if ( chFind )
				{
					if ( m_Buffer.FindChar( chFind, chFindPair, bForward, nRow, nCol ) )
					{
						m_Selection.SetExtendedSelection( nCol, nRow, nCol + 1, nRow, TRUE );
						dwError = 0;
					}
					else
					{
						dwError = CMDERR_NOTFOUND;
					}
				}
				else
				{
					dwError = CMDERR_INPUT;
				}

			}
		}
		else
		{
			dwError = CMDERR_SELECTION;
		}
	}

	if ( dwError )
	{
		// some kind of error
		NotifyParentOfCmdFailure( dwError );
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//  Display Options
//
//

void CEdit::ToggleWhitespaceDisplay()
{
	BOOL bState;
	if ( IsPlayingMacro() )
	{
		GetMacroData( ( LPBYTE ) &bState, sizeof( bState ) );
	}
	else
	{
		bState = !m_bDisplayWhitespace;
		if ( IsRecordingMacro() )
		{
			AddMacroData( ( LPBYTE ) &bState, sizeof( bState ) );
		}
	}
	m_bDisplayWhitespace = bState;
	Repaint( FALSE );
}

void CEdit::ToggleOvertype()
{
	BOOL bState;
	
	if ( IsPlayingMacro() )
	{
		GetMacroData( ( LPBYTE ) &bState, sizeof( bState ) );
	}
	else
	{
		bState = !m_bOverType;
		if ( IsRecordingMacro() )
		{
			AddMacroData( ( LPBYTE ) &bState, sizeof( bState ) );
		}
	}
	m_bOverType = bState;
	// make sure the correct caret (fat or thin) is set
	m_Selection.ResetCaret( m_font->hFont );

	NotifyParent( CMN_OVERTYPECHANGE );
}

void CEdit::SetRepeatCount( int nCount )
{
	if ( !IsPlayingMacro() && !IsRecordingMacro() && ( m_nRepeatCount == 1 ) )
	{
		// ask user for repeat count
		if ( !nCount &&
			 PromptUserForRepeatCount( this, GetDlgParent(), nCount ) )
		{
			m_nRepeatCount = nCount;
		}
	}
	else
	{
		NotifyParentOfCmdFailure( CMDERR_FAILURE );
	}
}

/////////////////////////////////////////////
//
// Text Insertion
//
//
void CEdit::InsertChar( TCHAR ch )
{
	if ( !IsPlayingMacro() && ch == _T('\0') )
	{
		// user probably assigned a hotkey to this command -- not a useful hotkey!
		return;
	}

	// ch is '\0' when playing a macro
	ASSERT( IsPlayingMacro() || ch != _T('\0') );
	if ( IsPlayingMacro() )
	{
		GetMacroData( ( LPBYTE ) &ch, sizeof( ch ) );
	}
	else if ( IsRecordingMacro() )
	{
		AddMacroData( ( LPBYTE ) &ch, sizeof( ch ) );
	}

	TCHAR szChar[ 3 ] = { _T('\0'), _T('\0'), _T('\0') };

	if ( !m_chLeadByteIn && is_leadbyte( ch ) )
	{
		// remember the lead byte for later (hopefully soon!) when we
		// receive the trail byte.
		m_chLeadByteIn = ch;
		return;
	}
	else if ( m_chLeadByteIn && is_trailbyte( ch ) )
	{
		// got the trailing byte.  package up a DBCS char and add it into the buffer.
		szChar[ 0 ] = m_chLeadByteIn;
		szChar[ 1 ] = ch;
	}
	else
	{
		szChar[ 0 ] = ch;
	}

	// reset the lead byte history for next time
	m_chLeadByteIn = _T('\0');

	InsertCharLowLevel( szChar );
}

void CEdit::InsertCharLowLevel( LPCTSTR pszChar )
{
	ASSERT( pszChar );

	int nChars = 1;
	LPTSTR pszSpaces = NULL;
	BOOL bTabChar = ( *pszChar == _T('\t') );
	// if expanding a tab, we want to insert the correct number of spaces.
	if ( bTabChar )
	{
		// expand this tab now
		int nViewCol = m_Selection.GetStartViewCol();
		int cbTabSize = m_Buffer.GetTabSize();
		nChars = -( nViewCol - ( ( ( nViewCol + cbTabSize ) / cbTabSize ) * cbTabSize ) );
		if (  m_bExpandTabs )
		{
			pszSpaces = new TCHAR[ nChars + 1 ];
			CharFill( pszSpaces, _T(' '), nChars );
			pszSpaces[ nChars ] = _T('\0');
		}
	}

	if ( m_Selection.IsColumnSel() )
	{
		///////////////////////////////////////////////////////
		// Special multi-line char insertion mode
		///////////////////////////////////////////////////////
		CDelayRepaint delay( this );
		CHourGlass wait( this );

		// turn off sel bounds for a moment so no text will be forced to the end of the line.
		BOOL bColumnSel = BoundSelection();
		SetBoundSelection( FALSE );

		m_Buffer.BeginEdit( m_Selection.GetEndRow(), m_Selection.GetEndCol() );

		int nStartCol, nStartRow, nEndCol, nEndRow;
		m_Selection.GetNormalizedColumnSelection( nStartCol, nStartRow, nEndCol, nEndRow );

		// if typing chars (and not indenting), first delete any selection.
		if ( !bTabChar )
			DeleteSelection( m_bOverType, FALSE );

		// insert the char into each line
		for ( int nLine = nStartRow; nLine <= nEndRow; nLine++ )
		{
			int nBuffStartCol = m_Buffer.ConvertViewColToBufferCol( nLine, nStartCol );			
			m_Selection.SetEmptySelection( nBuffStartCol, nLine, FALSE );
			ReplaceSelection( pszSpaces ? pszSpaces : pszChar, FALSE );
		}
		m_Selection.EnableColumnSel( TRUE );

		// if typing chars (and not indenting), make the selection empty (column-wise).
		if ( !bTabChar )
			nEndCol = nStartCol;

		m_Selection.SetExtendedSelection( m_Buffer.ConvertViewColToBufferCol( nStartRow, nStartCol + nChars ), nStartRow,
		                              m_Buffer.ConvertViewColToBufferCol( nEndRow, nEndCol + nChars ), nEndRow,
									  true );

		m_Buffer.EndEdit( m_Selection.GetEndRow(), m_Selection.GetEndCol() );

		SetBoundSelection( bColumnSel );
	}
	else
	{
		m_Buffer.EnableCollapseUndo( TRUE );
		ReplaceSelection( pszSpaces ? pszSpaces : pszChar, m_bOverType );
		SafeUpdateWindow();
		m_Buffer.EnableCollapseUndo( FALSE );
	}
	delete pszSpaces;
}

void CEdit::NewLine()
{
	int nNextLine = m_Selection.GetStartRow() + 1;
	int nCol = 0;

	if ( !m_bOverType || ( nNextLine >= m_Buffer.GetLineCount() ) )
	{
		CDelayRepaint delay( this ); // this prevents cursor from jumping around while autoindent occurs
		int nStartCol, nStartRow, nEndCol, nEndRow;
		m_Selection.GetNormalizedBufferSelection( nStartCol, nStartRow, nEndCol, nEndRow );
		nCol = m_Selection.GetEndCol();

		m_Buffer.BeginEdit( nEndRow, nEndCol );

		m_Buffer.NormalizeLineTextCase( m_Selection.GetEndRow() );

		TCHAR szChar[ 2 ] = { _T('\n'), _T('\0') };
		ReplaceSelection( szChar, FALSE );

		// if user pressed enter at the very start of the line, then don't do any smart indentation
		if ( nCol > 0 )
		{
			if ( m_eIndentStyle == eIndentOff )
			{
				nCol = 0;
			}
			else if ( m_eIndentStyle == eIndentPrev || m_Buffer.LanguageIsSGML() )
			{
				nCol = CopyLeadingIndentation( nNextLine - 1, nNextLine, FALSE, FALSE );
			}
			else
			{
				ASSERT( m_eIndentStyle == eIndentScope || m_Buffer.LanguageIsSGML() );
				BOOL bScopeStart;
				int nMatchingLine = IndentLineToScope( nNextLine - 1, bScopeStart );
				BOOL bCopyPrev = ( nMatchingLine == -1 );
				nCol = CopyLeadingIndentation( bCopyPrev ? nNextLine - 1 : nMatchingLine, nNextLine, bScopeStart, !bCopyPrev );
			}
		}
		m_Buffer.EndEdit( nNextLine, nCol );
	}
	m_Selection.SetEmptySelection( nCol, nNextLine, TRUE );
}

void CEdit::ClearSelection()
{
	// clear any extended selection
	if ( !m_Selection.IsEmpty() )
	{
		m_Selection.SetEmptySelection( m_Selection.GetEndCol(), m_Selection.GetEndRow() );
	}
}

void CEdit::WhitespaceDisplayOn()
{
	m_bDisplayWhitespace = TRUE;
	Repaint( FALSE );
}

void CEdit::WhitespaceDisplayOff()
{
	m_bDisplayWhitespace = FALSE;
	Repaint( FALSE );
}

void CEdit::OvertypeOn()
{
	m_bOverType = TRUE;
	// make sure the correct caret is set
	m_Selection.ResetCaret( m_font->hFont );

	NotifyParent( CMN_OVERTYPECHANGE );
}

void CEdit::OvertypeOff()
{
	m_bOverType = FALSE;
	// make sure the correct caret is set
	m_Selection.ResetCaret( m_font->hFont );

	NotifyParent( CMN_OVERTYPECHANGE );
}

void CEdit::BeginUndo()
{
	m_Buffer.BeginEdit( m_Selection.GetEndRow(), m_Selection.GetEndCol() );
}

void CEdit::EndUndo()
{
	m_Buffer.EndEdit( m_Selection.GetEndRow(), m_Selection.GetEndCol() );
}
