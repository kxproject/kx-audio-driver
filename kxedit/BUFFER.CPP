#include "precomp.h"
#include "regexp.h"
#include "undo.h"
#include "buffer.h"

CBuffer::CBuffer()
{
	m_pCtrl = NULL;
	m_nLineCount = m_nLinesAllocated = 0;
	m_pLines = NULL;
	m_pfnCallback = NULL;
	m_lParam = NULL;
	m_pUndo = NULL;
	m_nUndoCount = 0;
	m_nUndosAllocated = 0;
	m_nUndoPos = 0;
	m_nUndoTrans = 0;
	m_nMaxUndo = DEF_MAXUNDO; // -1 means infinite
	m_nEditLevel = 0;
	m_nMaxLineWidthGuess = 0;
	m_nLineChunkSize = 100;
	m_bFirstUndoAction = FALSE;
	m_bUndoEnabled = TRUE;
	m_bCollapseUndo = FALSE;
	m_bSilentChange = FALSE;
	m_cbTab = DEF_TABSIZE;
	m_bReadOnly = DEF_READONLY;
	m_bWantCarriageReturn = DEF_WANTCARRIAGERETURN;
	m_bNormalizeCase = DEF_NORMALIZECASE;

	m_pLangBuff = NULL;
	ZeroMemory( m_CharIsKeyword, sizeof( m_CharIsKeyword ) );
	m_bLangIsCaseSensitive = TRUE;

	m_nKeywords =
	m_nOperators =
	m_nMultiLineComments =
	m_nStringDelims =
	m_nScopeKeywords =
	m_nTagElementNames =
	m_nTagAttributeNames =
	m_nTagEntities = 0;
	
	m_pKeywords =
	m_pKeywordsUnsorted =
	m_pOperators =
	m_pOperatorsUnsorted =
	m_pSingleLineComments =
	m_pSingleLineCommentsUnsorted =
	m_pMultiLineComments1 =
	m_pMultiLineComments1Unsorted =
	m_pMultiLineComments2 =
	m_pMultiLineComments2Unsorted =
	m_pStringDelims =
	m_pStringDelimsUnsorted =
	m_pScopeKeywords1 =
	m_pScopeKeywords1Unsorted =
	m_pScopeKeywords2 =
	m_pScopeKeywords2Unsorted =
	m_pTagElementNames =
	m_pTagElementNamesUnsorted =
	m_pTagAttributeNames =
	m_pTagAttributeNamesUnsorted =
	m_pTagEntities =
	m_pTagEntitiesUnsorted = NULL;

	InitializeCriticalSection( &m_csSyntax );
	m_hSyntaxThread = NULL;
	m_hSyntaxEvent = NULL;
	m_byteSyntaxCmd = 0;
	m_nStartParseAt = 0;
	m_nCanStopParseOn = -1;	// the end of the buffer
	m_bAllowParse = TRUE;

	m_nChangeLevel = 0;
	m_bForceModified = DEF_MODIFIED;
	m_nNotifyDelCount = 0;

	m_eState = eEdit;
}

CBuffer::~CBuffer()
{
	// must end thread before buffer is deleted
	if ( m_hSyntaxThread )
	{
		EndSyntaxParse();
		ASSERT( !m_hSyntaxThread );
		ASSERT( !m_hSyntaxEvent );
	}

	if ( m_pLines )
	{
		for ( int i = 0; i < m_nLineCount; i++ )
		{
			m_pLines[ i ].CLine::~CLine();
		}
		free( m_pLines );
	}

	if ( m_pUndo )
	{
		ClearRedoChain( TRUE );
		free( m_pUndo );
	}

	DeleteCriticalSection( &m_csSyntax );
	DeleteLanguageInfo();
}

LPCTSTR CBuffer::GetLineText( int nLine ) const
{
	ASSERT( nLine >= 0 && nLine < m_nLineCount );
	LPCTSTR psz = m_pLines[ nLine ].GetText();

	return psz ? psz : _T("");
}

int CBuffer::GetLineLength( int nLine ) const
{
	ASSERT( nLine >= 0 && nLine < m_nLineCount );
	return m_pLines[ nLine ].GetLength();
}

CLine *CBuffer::GetLine( int nLine ) const
{
	ASSERT( nLine >= 0 && nLine < m_nLineCount );
	return &m_pLines[ nLine ];
}

void CBuffer::InsertLine( int nBefore, LPCTSTR pszText, int cbText )
{
	Lock();

	ASSERT( nBefore >= 0 && nBefore <= m_nLineCount );
	ASSERT( m_nEditLevel > 0 || IsInUndoRedo() );

	if ( cbText == -1 )
	{
		cbText = _tcslen( pszText );
	}
	EnsureLineArraySize( m_nLineCount + 1 );
	CLine *pLine = &m_pLines[ nBefore ];
	
	int nCommentStyle = 0, nStringStyle = 0;
	BOOL bInComment;
	BOOL bInString;
	if ( nBefore < m_nLineCount )
	{
		bInComment = pLine->IsInComment( nCommentStyle );
		bInString = pLine->IsInString( nStringStyle );
	}
	else
	{
		bInComment = bInString = FALSE;
	}

	// make room for the new line
	memmove( m_pLines + nBefore + 1, 
	         m_pLines + nBefore,
	         ( m_nLineCount - nBefore ) * sizeof( CLine ) );
	pLine->CLine::CLine( pszText, cbText );
	m_nLineCount++;

	// if large files (> 100,000 lines) are editted, bump up the chunk size
	// to prevent expensive reallocs.
	if ( m_nLineCount > 100000 )
	{
		m_nLineChunkSize = 2000;
	}
	int nLineLen = pLine->GetLength();
	m_nMaxLineWidthGuess = max( m_nMaxLineWidthGuess, nLineLen );
	
	// default the comment and string states to that of the previous line
	pLine->SetInComment( bInComment, nCommentStyle );
	pLine->SetInString( bInString, nStringStyle );
	Notify( eLineAdded, nBefore, 0, 1 );

	if ( ShouldAddUndoActions() )
	{
		AddUndoAction( new CUndoAddRemoveLine( TRUE, pLine->GetText(), pLine->GetLength(), nBefore ) );
	}

	Unlock();

	NormalizeLineTextCase( nBefore );

	int nTemp = nBefore - 1;
	NotifySyntaxParser( max( 0, nTemp ) );
	OnChanged();
}

void CBuffer::InsertCharsIntoLine( int nLine, int nColBefore, LPCTSTR pszText, int cbText )
{
	Lock();

	ASSERT( m_nEditLevel > 0 || IsInUndoRedo() );

	if ( cbText == -1 )
	{
		cbText = _tcslen( pszText );
	}

	CLine *pLine = GetLine( nLine );
	pLine->AddChars( nColBefore, pszText, cbText );
	int nLineLen = pLine->GetLength();
	m_nMaxLineWidthGuess = max( m_nMaxLineWidthGuess, nLineLen );
	Notify( eLineChanged, nLine, nColBefore, cbText );

	BOOL bAdd = ( cbText > 0 );
	if ( cbText && ShouldAddUndoActions() )
	{
		if ( cbText == 1 && m_bCollapseUndo )
		{
			if ( m_nUndoPos >= 2 )
			{
				CUndoAction *pFixupEnd = m_pUndo[ m_nUndoPos - 1 ];
				CUndoAction *pFixup = m_pUndo[ m_nUndoPos - 2 ];
				if ( pFixupEnd->IsEndAction() && 
				     pFixup->Append( TRUE, *pszText, nLine, nColBefore ) )
				{
					// this char was added to the previous undo record
					bAdd = FALSE;

					// fixup the end record's cursor pos
					pFixupEnd->SetCol( nColBefore + 1 );
					// let control know that buffer changed even though
					// m_nChangeLevel didn't.
					m_bSilentChange = TRUE;
				}
			}
		}

		if ( bAdd )
		{
			CUndoAction *pAction = new CUndoInsertDeleteString( TRUE, pszText, cbText, nLine, nColBefore );
			if ( m_bCollapseUndo )
			{
				pAction->EnableAppend();
			}
			AddUndoAction( pAction );
		}
	}

	Unlock();

	NotifySyntaxParser( nLine );

	if ( bAdd || !IsModified() )
	{
		OnChanged();
	}
}

void CBuffer::RemoveLines( int nStartLine, int nCount )
{
	ASSERT( m_nEditLevel > 0 || IsInUndoRedo() );
	int nTemp = m_nLineCount - nStartLine;
	nCount = min( nTemp, nCount );
	if ( !nCount )
	{
		// nothing to do
		return;
	}

	Notify( eLineRemoved, nStartLine, 0, nCount );

	Lock();

	for ( int i = nStartLine + nCount - 1; i >= nStartLine; i-- )
	{
		SetLineStyle( i, 0 );	// Set the line style to zero so counters will be updated property
		CLine *pLine = GetLine( i );
		if ( ShouldAddUndoActions() )
		{
			AddUndoAction( new CUndoAddRemoveLine( FALSE, pLine->GetText(), pLine->GetLength(), i ) );
		}
		pLine->CLine::~CLine();
	} 
	// remove the entry from the array
	memmove( m_pLines + nStartLine, m_pLines + nStartLine + nCount, ( m_nLineCount - ( nStartLine + nCount - 1 ) ) * sizeof( CLine ) );
	m_nLineCount -= nCount;

	Unlock();

	if ( GetLineCount() )
	{
		NotifySyntaxParser( nStartLine > 0 ? nStartLine - 1 : 0 );
	}

	OnChanged( nCount );
}

void CBuffer::RemoveCharsFromLine( int nLine, int nStartCol, int nChars )
{
	ASSERT( m_nEditLevel > 0 || IsInUndoRedo() );
	if ( !nChars )
	{
		// nothing to do
		return;
	}

	Lock();

	CLine *pLine = GetLine( nLine );
	if ( ShouldAddUndoActions() )
	{
		AddUndoAction( new CUndoInsertDeleteString( FALSE, pLine->GetText() + nStartCol, nChars, nLine, nStartCol ) );
	}
	pLine->RemoveChars( nStartCol, nChars );
	Notify( eLineChanged, nLine, nStartCol, nChars );

	Unlock();

	NotifySyntaxParser( nLine );

	OnChanged();
}

void CBuffer::EnsureLineArraySize( register int nLinesRequired )
{
	if ( nLinesRequired > m_nLinesAllocated )
	{
		m_nLinesAllocated = nLinesRequired + m_nLineChunkSize;
		size_t cbNewSize = ( m_nLinesAllocated ) * sizeof( CLine );
		// need to realloc
		m_pLines = ( m_pLines ? 
		           ( CLine * )realloc( m_pLines, cbNewSize ) :
				   ( CLine * )malloc( cbNewSize ) );
 	}
}

void CBuffer::FlushExtraLines()
{
	if ( m_nLineCount != m_nLinesAllocated )
	{
		ASSERT( m_nLineCount < m_nLinesAllocated );
		size_t cbNewSize = m_nLineCount * sizeof( CLine );
		// need to realloc
		m_pLines = ( m_pLines ? 
		           ( CLine * )realloc( m_pLines, cbNewSize ) :
				   ( CLine * )malloc( cbNewSize ) );
		m_nLinesAllocated = m_nLineCount;
	}
}

void CBuffer::SetNotificationCallback( BUFFERCALLBACKFN *pfn, LPARAM lParam )
{
	m_pfnCallback = pfn;
	m_lParam = lParam;
}

void CBuffer::Notify( Notification eNotification, int nLine, int nCol, int nCount ) const
{
	if ( m_pfnCallback )
	{
		m_pfnCallback( m_lParam, eNotification, nLine, nCol, nCount );
	}
}

void CBuffer::BeginEdit( int nCursorLine, int nCursorCol )
{
	Notify( eBeginEdit, -1, -1, -1 );
	ASSERT( m_nEditLevel >= 0 );
	if ( ( ++m_nEditLevel == 1 ) && !IsInUndoRedo() )
	{
		m_bFirstUndoAction = TRUE;
		m_nBeginEditLine = nCursorLine;
		m_nBeginEditCol =  nCursorCol;
		ClearRedoChain( FALSE );
	}
}

void CBuffer::EndEdit( int nCursorLine, int nCursorCol )
{
	ASSERT( m_nEditLevel >= 1 );
	if ( ( --m_nEditLevel == 0 ) && !IsInUndoRedo() )
	{
		if ( !m_bFirstUndoAction )
		{
			ASSERT( m_nUndoPos );
			CUndoStartEnd *pUndo = new CUndoStartEnd( nCursorLine,  nCursorCol );
			pUndo->FlagAsEndAction();
			AddUndoAction( pUndo );
		}
	}
	Notify( eEndEdit, -1, -1, -1 );
}

void CBuffer::Undo()
{
	m_eState = eUndo;
	BeginEdit();
	ASSERT( CanUndo() );
	CUndoAction *pAction;
	Notify( eBeginUndo, -1, -1, -1 );

	do
	{
		ASSERT( m_nUndoPos >= 0 );
		pAction = m_pUndo[ --m_nUndoPos ];
		pAction->Undo( this );
	} while ( !pAction->IsStartAction() );
	m_nUndoTrans--;
	EndEdit();
	m_eState = eEdit;
	Notify( eEndUndo, -1, -1, -1 );
}

void CBuffer::Redo()
{
	m_eState = eRedo;
	BeginEdit();
	ASSERT( CanRedo() );
	CUndoAction *pAction;
	Notify( eBeginRedo, -1, -1, -1 );
	do
	{
		pAction = m_pUndo[ m_nUndoPos++ ];
		pAction->Redo( this );
	} while ( !pAction->IsEndAction() );
	m_nUndoTrans++;
	EndEdit();
	m_eState = eEdit;
	Notify( eEndRedo, -1, -1, -1 );
}

void CBuffer::AddUndoAction( CUndoAction *pAction )
{
	if ( !m_bReadOnly && ( m_nMaxUndo != 0 ) )
	{
		EnsureUndoArraySize( m_nUndoPos + 2 );
		if ( m_bFirstUndoAction )
		{
			m_bFirstUndoAction = FALSE;
			CUndoStartEnd *pUndo = new CUndoStartEnd( m_nBeginEditLine, m_nBeginEditCol );
			pUndo->FlagAsStartAction();
			m_pUndo[ m_nUndoPos++ ] = pUndo;
			VERIFY( ++m_nUndoCount == m_nUndoPos );
			m_nUndoTrans++;
			EnforceMaxUndo();
		}

		m_pUndo[ m_nUndoPos++ ] = pAction;
		VERIFY( ++m_nUndoCount == m_nUndoPos );
	}
	else
	{
		delete pAction;
	}
}

void CBuffer::ClearRedoChain( BOOL bEntire )
{
	for ( int i = bEntire ? 0 : m_nUndoPos; i < m_nUndoCount; i++ )
	{
		delete m_pUndo[ i ];
	}
	if ( bEntire )
	{
		m_nUndoCount = m_nUndoPos = m_nUndoTrans = 0;
	}
	else
	{
		m_nUndoCount = m_nUndoPos;
	}
}

void CBuffer::EnsureUndoArraySize( int nUndosRequired )
{
	if ( nUndosRequired > m_nUndosAllocated )
	{
		size_t cbNewSize = ( nUndosRequired + UNDO_GROWBY ) * sizeof( CUndoAction * );
		// need to realloc
		m_pUndo  = ( m_pUndo ? 
		           ( CUndoAction ** )realloc( m_pUndo, cbNewSize ) :
				   ( CUndoAction ** )malloc( cbNewSize ) );

		if (m_pUndo)
			m_nUndosAllocated = nUndosRequired + UNDO_GROWBY;
 	}
}

void CBuffer::EnforceMaxUndo()
{
	if ( ( m_nMaxUndo != -1 ) && ( m_nUndoTrans > m_nMaxUndo ) )
	{
		// trim from the from of the undo chain enough transactions to keep the undo chain
		// limit enforced.
		int nRemoved = 0;
		int i = 0;
		do
		{
			ASSERT( m_pUndo[ i ]->IsStartAction() );
			ASSERT( !m_pUndo[ i ]->IsEndAction() );
			BOOL bIsEndAction;
			do
			{
				CUndoAction *pAction = m_pUndo[ i ];
				bIsEndAction = pAction->IsEndAction();
				delete pAction;
				nRemoved++;
				i++;
			} while ( !bIsEndAction );
			m_nUndoTrans--;
		} while ( m_nUndoTrans > m_nUndoCount );

		ASSERT( i <= m_nUndoPos );

		// shift (but don't shrink) the undo chain, consuming the deleted undo records
		memmove( m_pUndo, m_pUndo + nRemoved, ( m_nUndoCount - nRemoved ) * sizeof( CUndoAction * ) );
		m_nUndoCount -= nRemoved;
		m_nUndoPos -= nRemoved;

	}
}

BOOL CBuffer::EnableUndo( BOOL bEnable )
{
	BOOL bOldValue = m_bUndoEnabled;
	m_bUndoEnabled = bEnable;

	if ( !m_bUndoEnabled )
	{
		ClearRedoChain( TRUE );
	}
	return bOldValue;
}

int CBuffer::ConvertBufferColToViewCol( int nRow, int nBuffCol ) const
{
	if ( nRow < m_nLineCount )
	{
		register int nViewCol = 0;
		register LPCTSTR psz = GetLineText( nRow );

		register int i = 0;
		while ( ( i < nBuffCol ) && *psz )
		{
			if ( *psz == _T('\t') )
			{
				nViewCol += ( ( nViewCol / m_cbTab ) * m_cbTab ) + m_cbTab - nViewCol;
				i++;
			}
			else
			{
				int cbChar = _tclen( psz );
				nViewCol += cbChar;
				i += cbChar;
			}
			psz = _tcsinc( psz );
		}

		i = min( i, nBuffCol );

		// also add in any space beyond the line
		return ( nViewCol + ( nBuffCol - i ) );
	}
	else
	{
		return nBuffCol;
	}
}

int CBuffer::ConvertViewColToBufferCol( int nRow, int nViewCol, BOOL bRoundToLeadByte ) const
{
	if ( nRow < m_nLineCount )
	{
		int nBuffCol = 0;
		register LPCTSTR psz = GetLineText( nRow );

		register int i = 0;

		while ( *psz )
		{
			int cbChar = bRoundToLeadByte ? _tclen( psz ) : 1;
			if ( *psz == _T('\t') )
			{
				i += ( ( i / m_cbTab ) * m_cbTab ) + m_cbTab - i;
				
			}
			else
			{
				i += cbChar;
			}

			if ( i <= nViewCol )
			{
				nBuffCol += cbChar;
			}
			else
			{
				break;
			}
			if ( bRoundToLeadByte )
				psz = _tcsinc( psz );
			else
				psz++;
		}

		// also add in any space beyond the line
		int nTemp = nViewCol - i;
		return ( nBuffCol + max( 0, nTemp ) );
	}
	else
	{
		return nViewCol;
	}
}

void CBuffer::SetTabSize( int nLen )
{
	ASSERT( nLen >= CM_MIN_TABSIZE && nLen <= CM_MAX_TABSIZE );
	m_cbTab = nLen;
}

void CBuffer::AdvanceToWordStart( int &nRow, int &nCol, BOOL bForward, BOOL bSkipInitialSpace ) const
{
	int nLen = GetLineLength( nRow );
	if ( bForward )
	{
		int cbChar;
		BOOL bSkipJustSpace = FALSE;
		if ( nCol >= nLen )
		{
			if ( nRow + 1 >= m_nLineCount )
			{
				// cannot go further
				return;
			}
			nRow++;
			nCol = 0;
			nLen = GetLineLength( nRow );
			bSkipJustSpace = TRUE;
		}

		LPCTSTR psz = GetLineText( nRow ) + nCol;

		if ( !bSkipJustSpace )
		{
			if ( is_alphanumeric( *psz ) )
			{
				do
				{
					cbChar = _tclen( psz );
					nCol += cbChar;
					psz += cbChar;
				}
				while ( cbChar && *psz && is_alphanumeric( *psz ) );
			}
			else if ( is_symbolorpunct( *psz ) )
			{
				do
				{
					cbChar = _tclen( psz );
					nCol += cbChar;
					psz += cbChar;
				}
				while ( cbChar && *psz && is_symbolorpunct( *psz ) );
			}
		}

		if ( bSkipInitialSpace && is_space( *psz ) )
		{
			do
			{
				cbChar = _tclen( psz );
				nCol += cbChar;
				psz += cbChar;
			}
			while ( cbChar && *psz && is_space( *psz ) );
		}
	}
	else
	{
		if ( nCol > nLen )
		{
			nCol = nLen;
		}
		else if ( nCol <= 0 )
		{
			if ( nRow - 1 < 0 )
			{
				// cannot go further
				return;
			}
			nRow--;
			nCol = GetLineLength( nRow );
		}

		if ( nCol == 0 )
		{
			return;
		}

		LPCTSTR pszStart = GetLineText( nRow );
		LPCTSTR psz = CharPrev( pszStart, pszStart + nCol );

		if ( bSkipInitialSpace && is_space( *psz ) )
		{
			do
			{
				nCol -= _tclen( psz );
				psz -= _tclen_prev( pszStart, psz );
			}
			while ( nCol > 0 && ( psz > pszStart ) && is_space( *psz ) );
		}

		// don't let the cursor go past the start of the buffer
		psz = max( psz, pszStart );

		if ( ( nCol > 0 ) && is_alphanumeric( *psz ) )
		{
			do
			{
				nCol -= _tclen( psz );
				psz -= _tclen_prev( pszStart, psz );
			}
			while ( nCol > 0 && is_alphanumeric( *psz ) );
		}
		else if ( ( nCol > 0 ) && is_symbolorpunct( *psz ) )
		{
			do
			{
				nCol -= _tclen( psz );
				psz -= _tclen_prev( pszStart, psz );
			}
			while ( nCol > 0 && is_symbolorpunct( *psz ) );
		}
	}
}

void CBuffer::AdvanceToWordEnd( int &nRow, int &nCol, BOOL bForward, BOOL bSkipInitialSpace ) const
{
	int nLen = GetLineLength( nRow );

	if ( bForward )
	{
		int cbChar;
		if ( nCol >= nLen )
		{
			if ( nRow < m_nLineCount - 1 )
			{
				nCol = 0;
				nRow++;
				nLen = GetLineLength( nRow );
				if ( nCol >= nLen )
				{
					return;
				}
			}
			else
			{
				// end of buffer
				return;
			}
		}

		LPCTSTR psz = GetLineText( nRow ) + nCol;
		if ( bSkipInitialSpace && is_space( *psz ) )
		{
			do
			{
				cbChar = _tclen( psz );
				nCol += cbChar;
				psz += cbChar;
			}
			while ( cbChar && *psz && is_space( *psz ) );
		}

		if ( is_alphanumeric( *psz ) )
		{
			do
			{
				cbChar = _tclen( psz );
				nCol += cbChar;
				psz += cbChar;
			}
			while ( cbChar && *psz && is_alphanumeric( *psz ) );
		}
		else if ( is_symbolorpunct( *psz ) )
		{
			do
			{
				cbChar = _tclen( psz );
				nCol += cbChar;
				psz += cbChar;
			}
			while ( cbChar && *psz && is_symbolorpunct( *psz ) );
		}

		if ( nCol > nLen )
		{
			nCol = nLen;
		}
	}
	else
	{
		BOOL bSkipJustSpace = FALSE;
		if ( nCol > nLen )
		{
			nCol = nLen;
			bSkipJustSpace = TRUE;
		}
		else if ( nCol <= 0 )
		{
			if ( nRow - 1 < 0 )
			{
				// cannot go further
				return;
			}
			nRow--;
			nCol = GetLineLength( nRow );
			bSkipJustSpace = TRUE;
		}

		if ( nCol == 0 )
		{
			return;
		}

		LPCTSTR pszStart = GetLineText( nRow );
		LPCTSTR psz = pszStart + nCol;

		if ( !bSkipJustSpace )
		{
			if ( ( nCol > 0 ) && is_alphanumeric( *CharPrev( psz, pszStart ) ) )
			{
				do
				{
					nCol -= _tclen( psz );
					psz -= _tclen_prev( pszStart, psz );
				}
				while ( nCol > 0 && is_alphanumeric( *CharPrev( psz, pszStart ) ) );
			}
			else if ( ( nCol > 0 ) && is_symbolorpunct( *CharPrev( psz, pszStart ) ) )
			{
				do
				{
					nCol -= _tclen( psz );
					psz -= _tclen_prev( pszStart, psz );
				}
				while ( nCol > 0 && is_symbolorpunct( *CharPrev( psz, pszStart ) ) );
			}
		}

		if ( bSkipInitialSpace && ( nCol > 0 ) && is_space( *CharPrev( psz, pszStart ) ) )
		{
			do
			{
				nCol -= _tclen( psz );
				psz -= _tclen_prev( pszStart, psz );
			}
			while ( nCol > 0 && is_space( *CharPrev( psz, pszStart ) ) );
		}

	}
}

void CBuffer::AdvanceToSentenceStart( int &nRow, int &nCol, BOOL bForward ) const
{
	if ( bForward )
	{
		if ( nCol <= GetLineLength( nRow ) )
		{
			while ( ( nRow < m_nLineCount - 1 ) && LineIsEmpty( nRow, TRUE ) )
			{
				nRow++;
			}
		}
		nCol = 0;
		nRow++;
		while ( ( nRow < m_nLineCount - 1 ) && !LineIsEmpty( nRow, TRUE ) )
		{
			nRow++;
		}

		if ( nRow >= m_nLineCount - 1 )
		{
			nRow = m_nLineCount - 1;
			nCol = GetLineLength( nRow );
		}
	}
	else
	{
		if ( nCol == 0 )
		{
			while ( ( nRow > 0 ) && LineIsEmpty( nRow - 1, TRUE ) )
			{
				nRow--;
			}
		}
		else
		{
			nCol = 0;
		}

		while ( ( nRow > 0 ) && !LineIsEmpty( nRow - 1, TRUE ) )
		{
			nRow--;
		}

		nRow = max( 0, nRow );
	}
}

BOOL CBuffer::LineIsEmpty( int nLine, BOOL bIgnoreWhiteSpace ) const
{
	LPCTSTR psz = GetLineText( nLine );

	BOOL bEmpty = TRUE;
	if ( bIgnoreWhiteSpace )
	{
		while ( *psz )
		{
			if ( !is_space( *psz ) )
			{
				bEmpty = FALSE;
				break;
			}
			psz = _tcsinc( psz );
		}
	}
	else
	{
		bEmpty = ( *psz == _T('\0') );
	}
		
	return bEmpty;
}

void CBuffer::ToggleBookmark( int nLine ) const
{
	CLine *pLine = GetLine( nLine );
	pLine->SetBookmark( !pLine->HasBookmark() );
	Notify( eLineChanged, nLine, 0, 0 );
}

void CBuffer::RemoveBookmark( int nLine ) const
{
	if ( HasBookmark( nLine ) )
	{
		ToggleBookmark( nLine );
		Notify( eLineChanged, nLine, 0, 0 );
	}
}

BOOL CBuffer::GetText( int nStartRow, int nStartCol, int nEndRow, int nEndCol, HGLOBAL &hMem, BOOL bCopyLineIfEmpty, BOOL bForceCR ) const
{
	ASSERT( nStartRow <= nEndRow );

	if ( !m_nLineCount )
	{
		return FALSE;
	}

	int nTemp = GetLineLength( nEndRow );
	nEndCol = min( nTemp, nEndCol );
	nTemp = GetLineLength( nStartRow );
	nStartCol = min( nTemp, nStartCol );

	///////////////////////////////////////////
	//
	// Calculate the required memory
	//
	//

	// reserve room for the '\n' carriage returns, plus the null terminator
	int cbText = nEndRow - nStartRow + 1;
	BOOL bWantCR = m_bWantCarriageReturn || bForceCR;
	for ( int nRow = nStartRow; nRow <= nEndRow; nRow++ )
	{
		int nLen = GetLineLength( nRow ) + ( int ) bWantCR;
		int nFirstPos = ( nRow == nStartRow ? min( nStartCol, nLen ) : 0 );
		nFirstPos = max( 0, nFirstPos );
		int nLastPos = ( nRow == nEndRow ? min( nEndCol, nLen ) : nLen );
		nLastPos = max( 0, nLastPos );

		cbText += ( nLastPos - nFirstPos );
	}

	ASSERT( cbText > 0 );

	BOOL bAppendCR = FALSE;
	if ( bCopyLineIfEmpty && ( cbText == 1 ) )
	{
		// empty selection (just the null terminator counted above)
		ASSERT( nStartRow == nEndRow );
		cbText = GetLineLength( nStartRow ) + 1 + bWantCR + 1;	// line text + CR + LF + NULL
		nStartCol = 0;
		nEndCol = cbText;
		bAppendCR = TRUE;
	}

	///////////////////////////////////////////
	//
	// Copy the text
	//
	//
	hMem = GlobalAlloc( GHND | GMEM_DDESHARE, cbText * sizeof( TCHAR ) );
	
	if ( hMem )
	{
		LPTSTR psz = ( LPTSTR )GlobalLock( hMem );
#ifdef _DEBUG
		LPCTSTR pszStart = psz;
#endif

		for ( int nRow = nStartRow; nRow <= nEndRow; nRow++ )
		{
			int nLen = GetLineLength( nRow );
			int nFirstPos = ( nRow == nStartRow ? min( nStartCol, nLen ) : 0 );
			nFirstPos = max( 0, nFirstPos );
			int nLastPos = ( nRow == nEndRow ? min( nEndCol, nLen ) : nLen );
			nLastPos = max( 0, nLastPos );
			
			if ( nLastPos > nFirstPos )
			{
				int cbLine = ( nLastPos - nFirstPos );
				_tcsncpy( psz, GetLineText( nRow ) + nFirstPos, cbLine );
				psz += cbLine;
			}

			if ( nRow < nEndRow || bAppendCR )
			{
				if ( bWantCR )
				{
					*psz++ = _T('\r');
				}
				*psz++ = _T('\n');
			}
		}
		
		*psz = _T('\0');

		ASSERT( _tcslen( pszStart ) + 1 == ( size_t )cbText );
		GlobalUnlock( hMem );
	}

	return ( hMem != NULL );
}

BOOL CBuffer::GetText( HANDLE hFile ) const
{
	ASSERT( hFile );
	char szEOLN[ 3 ];
	if ( m_bWantCarriageReturn )
	{
		szEOLN[ 0 ] = _T('\r');
		szEOLN[ 1 ] = _T('\n');
		szEOLN[ 2 ] = _T('\0');
	}
	else
	{
		szEOLN[ 0 ] = _T('\n');
		szEOLN[ 1 ] = _T('\0');
	}
	int cbEOLN = m_bWantCarriageReturn ? 2 : 1;
	BOOL bSuccess = TRUE;
	for ( int nRow = 0; bSuccess && nRow < m_nLineCount; nRow++ )
	{
		int nLen = GetLineLength( nRow );

#ifdef _UNICODE
		LPSTR pszLine = new char[ nLen + 1 ];
		WideCharToMultiByte( CP_ACP, 0, GetLineText( nRow ), nLen + 1, pszLine, nLen + 1, NULL, NULL );
		DWORD dwWritten;
		bSuccess = WriteFile( hFile,
		                      pszLine,
				              nLen,
				              &dwWritten,
				              NULL );
		delete pszLine;
#else
		DWORD dwWritten;
		bSuccess = WriteFile( hFile,
		                      GetLineText( nRow ),
				              nLen,
				              &dwWritten,
				              NULL );
#endif
		if ( bSuccess && ( nRow < m_nLineCount - 1 ) )
		{
			bSuccess = WriteFile( hFile,
								  szEOLN,
								  cbEOLN,
								  &dwWritten,
								  NULL );
		}
	}

	return bSuccess;
}

void CBuffer::GetText( LPTSTR pszBuff ) const
{
	ASSERT( pszBuff );
	LPTSTR psz = pszBuff;

	for ( int nRow = 0; nRow < m_nLineCount; nRow++ )
	{
		_tcscpy( psz, GetLineText( nRow ) );
		psz += GetLineLength( nRow );

		if ( nRow < m_nLineCount - 1 )
		{
			if ( m_bWantCarriageReturn )
			{
				*psz++ = _T('\r');
			}
			*psz++ = _T('\n');
		}
	}
	*psz = _T('\0');
}
 
void CBuffer::EnsureLineLength( int nRow, int cbLen )
{
	int cbLine = GetLineLength( nRow );
	if ( cbLen > cbLine )
	{
		int cbFill = ( cbLen - cbLine );
		LPTSTR pszFill = new TCHAR[ cbFill + 1 ];
		CharFill( pszFill, _T(' '), cbFill );
		InsertCharsIntoLine( nRow, cbLine, pszFill,  cbFill );
		delete pszFill;
	}
}

/* static */ int CBuffer::CharCount( LPCTSTR psz, TCHAR ch )
{
	ASSERT( psz );
	int cb = 0;
	LPCTSTR p = psz;
	while ( *p )
	{
		if ( *p++ == ch )
		{
			cb++;
		}
	}

	return cb;
}

BOOL CBuffer::SetLanguage( CM_LANGUAGE *pLang )
{
	BOOL bSuccess = FALSE;

	if ( !pLang )
	{
		// delete old language info
		DeleteLanguageInfo();
		return TRUE;
	}

	int nKeywords, nOperators, nStringDelims, nSingleLineComments, nMultiLineComments, nMultiLineComments2,
		nScopeKeywords, nScopeKeywords2, nTagElementNames, nTagAttributeNames, nTagEntities;

	struct _LangInfo { int *pnTokensIn; int *pnTokensOut; LPCTSTR *ppszTokensIn; LPCTSTR **ppszTokensOut; LPCTSTR **ppszTokensOutUnsorted; WORD wType; } LangInfo[] =
		{ { &nKeywords, &m_nKeywords, &pLang->pszKeywords, &m_pKeywords, &m_pKeywordsUnsorted, CHAR_KEYWORD },
		  { &nOperators, &m_nOperators, &pLang->pszOperators, &m_pOperators, &m_pOperatorsUnsorted, CHAR_OPERATOR },
		  { &nStringDelims, &m_nStringDelims, &pLang->pszStringDelims, &m_pStringDelims, &m_pStringDelimsUnsorted, CHAR_STRING },
		  { &nSingleLineComments, &m_nLineComments, &pLang->pszSingleLineComments, &m_pSingleLineComments, &m_pSingleLineCommentsUnsorted, CHAR_SLCOMMENT },
		  { &nMultiLineComments, &m_nMultiLineComments, &pLang->pszMultiLineComments1, &m_pMultiLineComments1, &m_pMultiLineComments1Unsorted, CHAR_MLCOMMENT1 },
		  { &nMultiLineComments2, &m_nMultiLineComments, &pLang->pszMultiLineComments2, &m_pMultiLineComments2, &m_pMultiLineComments2Unsorted, CHAR_MLCOMMENT2 },
		  { &nScopeKeywords, &m_nScopeKeywords, &pLang->pszScopeKeywords1, &m_pScopeKeywords1, &m_pScopeKeywords1Unsorted, CHAR_SCOPE1 },
		  { &nScopeKeywords2, &m_nScopeKeywords, &pLang->pszScopeKeywords2, &m_pScopeKeywords2, &m_pScopeKeywords2Unsorted, CHAR_SCOPE2 },
		  { &nTagEntities, &m_nTagEntities, &pLang->pszTagEntities, &m_pTagEntities, &m_pTagEntitiesUnsorted, CHAR_TAG_ENTITY },
		  { &nTagElementNames, &m_nTagElementNames, &pLang->pszTagElementNames, &m_pTagElementNames, &m_pTagElementNamesUnsorted, CHAR_TAG_ELEMENT_NAME },
		  { &nTagAttributeNames, &m_nTagAttributeNames, &pLang->pszTagAttributeNames, &m_pTagAttributeNames, &m_pTagAttributeNamesUnsorted, CHAR_TAG_ATTRIBUTE_NAME } };

	int cbLangBuff = 0;

	// calculate token counts and total buffer size requirement
	for ( int i = 0; i < ARRAY_SIZE( LangInfo ); i++ )
	{
	_LangInfo *pInfo = LangInfo + i;
	*pInfo->pnTokensIn = *pInfo->ppszTokensIn ? ( CharCount( *pInfo->ppszTokensIn, _T('\n') ) + 1 ) : 0;
	if ( *pInfo->ppszTokensIn )
		cbLangBuff += ( _tcslen( *pInfo->ppszTokensIn ) + 2 * *pInfo->pnTokensIn + 1 );
	}

	if ( !cbLangBuff ||
	     nMultiLineComments != nMultiLineComments2 ||
	     nScopeKeywords != nScopeKeywords2 )
	{
		// for each start comment token, there must be an end token.
		// same for scope words
		return FALSE;
	}

	// delete old language info
	DeleteLanguageInfo();

	// allocate storage for each list of tokens
	for ( int i = 0; i < ARRAY_SIZE( LangInfo ); i++ )
	{
	_LangInfo *pInfo = LangInfo + i;
	*pInfo->ppszTokensOut = *pInfo->pnTokensIn ? new LPCTSTR[ *pInfo->pnTokensIn ] : NULL;
	*pInfo->ppszTokensOutUnsorted = *pInfo->pnTokensIn ? new LPCTSTR[ *pInfo->pnTokensIn ] : NULL;
	}

	// parse the language structure passed in by the caller and calculate some per-token info
	m_pLangBuff = new TCHAR[ cbLangBuff ];
	BOOL bCaseSensitive = pLang->bIsCaseSensitive == TRUE;
	LPTSTR pszBuff = m_pLangBuff;

	ZeroMemory( m_pLangBuff, cbLangBuff * sizeof( TCHAR ) );
	ZeroMemory( m_CharIsKeyword, sizeof( m_CharIsKeyword ) );

	for ( int i = 0; i < ARRAY_SIZE( LangInfo ); i++ )
	{
		_LangInfo *pInfo = LangInfo + i;
		int nKeywords = *pInfo->pnTokensIn;
		if ( nKeywords )
		{
			WORD wKeywordType = pInfo->wType;
			LPCTSTR psz = *pInfo->ppszTokensIn;
			LPTSTR pszTokenStart = pszBuff;
			pszBuff += 2;
			int nToken = 0;
			BOOL bContinue = TRUE;
			while ( bContinue )
			{
				if ( *psz == _T('\n') || !*psz )
				{
					int nTokenLen = pszBuff - pszTokenStart - 2;
					BOOL bIsAlphaPre = nTokenLen ? is_alphanumeric( psz[ -nTokenLen ] ) : TRUE;
					BOOL bIsAlphaPost = nTokenLen ? is_alphanumeric( psz[ -1 ] ) : TRUE;
					*pszTokenStart = ( BYTE ) ( nTokenLen | ( bIsAlphaPre << 7 ) | ( bIsAlphaPost << 6 ) );
					pszTokenStart[ 1 ] = ( BYTE ) nToken;
					(*pInfo->ppszTokensOut)[ nToken ] = pszTokenStart;
					(*pInfo->ppszTokensOutUnsorted)[ nToken++ ] = pszTokenStart;
					TCHAR sz[ 5 ];
					int cbChar = _tclen( pszTokenStart + 2 );
					_tcsncpy( sz, pszTokenStart + 2, cbChar );
					sz[ cbChar ] = _T('\0');
					CharUpper( sz );
					m_CharIsKeyword[ ( BYTE ) *sz ] |= wKeywordType;
					CharLower( sz );
					m_CharIsKeyword[ ( BYTE ) *sz ] |= wKeywordType;

					if ( *psz == _T('\n') )
					{
						psz = _tcsinc( psz );
						pszTokenStart =	pszBuff;
						pszBuff += 2;
					}
					else
					{
						bContinue = FALSE;
					}
				}
				else
				{
					*pszBuff++ = *psz++;
				}
			}
		}
	}

	ASSERT( pszBuff - m_pLangBuff <= cbLangBuff );

	m_CharIsKeyword[ ( BYTE ) ( DWORD ) CharUpper( ( LPTSTR ) ( DWORD ) pLang->chEscape ) ] |= CHAR_ESCAPESEQ;
	m_CharIsKeyword[ ( BYTE ) ( DWORD ) CharLower( ( LPTSTR ) ( DWORD ) pLang->chEscape ) ] |= CHAR_ESCAPESEQ;
	
	// remember new info
	m_bLangIsCaseSensitive = pLang->bIsCaseSensitive == TRUE;

	m_chEscape = pLang->chEscape;
	m_chTerminator = pLang->chTerminator;
	m_dwLangStyle = pLang->dwStyle;

	// now, sort the separate lists
	for ( int i = 0; i < ARRAY_SIZE( LangInfo ); i++ )
	{
		_LangInfo *pInfo = LangInfo + i;
		qsort( *pInfo->ppszTokensOut, *pInfo->pnTokensIn, sizeof( LPCTSTR ), TokenSortProc );
		*pInfo->pnTokensOut = *pInfo->pnTokensIn;
	}

	// start the language parser
	StartSyntaxParse();
	// and tell it to start at the top of the file
	if ( m_nLineCount )
	{
		NotifySyntaxParser( 0 );
	}

	return TRUE;
}

void CBuffer::DeleteLanguageInfo()
{
	if ( HasLanguage() )
	{
		EndSyntaxParse();
	}

	ZeroMemory( m_CharIsKeyword, sizeof( m_CharIsKeyword ) );
	delete m_pLangBuff;
	m_pLangBuff = NULL;

	struct _Info { LPCTSTR **ppszKeywords; LPCTSTR **ppszKeywordsUnsorted; int *pnKeywords; } Info[] = { 
		{ &m_pKeywords, &m_pKeywordsUnsorted, &m_nKeywords },
		{ &m_pOperators, &m_pOperatorsUnsorted, &m_nOperators },
		{ &m_pSingleLineComments, &m_pSingleLineCommentsUnsorted, &m_nLineComments },
		{ &m_pMultiLineComments1, &m_pMultiLineComments1Unsorted, &m_nMultiLineComments },
		{ &m_pMultiLineComments2, &m_pMultiLineComments2Unsorted, &m_nMultiLineComments },
		{ &m_pScopeKeywords1, &m_pScopeKeywords1Unsorted, &m_nScopeKeywords },
		{ &m_pScopeKeywords2, &m_pScopeKeywords2Unsorted, &m_nScopeKeywords },
		{ &m_pStringDelims, &m_pStringDelimsUnsorted, &m_nStringDelims },
		{ &m_pTagElementNames, &m_pTagElementNamesUnsorted, &m_nTagElementNames },
		{ &m_pTagAttributeNames, &m_pTagAttributeNamesUnsorted, &m_nTagAttributeNames },
		{ &m_pTagEntities, &m_pTagEntitiesUnsorted, &m_nTagEntities } };

	for ( int i = 0; i < ARRAY_SIZE( Info ); i++ )
	{
		_Info *pInfo = Info + i;
		delete *pInfo->ppszKeywords;
		delete *pInfo->ppszKeywordsUnsorted;
		*pInfo->ppszKeywords = NULL;
		*pInfo->ppszKeywordsUnsorted = NULL;
		*pInfo->pnKeywords = 0;
	}
}

/* static */ int CBuffer::TokenSortProc( const void *pVal1, const void *pVal2 )
{
	LPCTSTR psz1 = *( LPCTSTR * )pVal1;
	ASSERT( psz1 );
	ASSERT( *psz1 );
	LPCTSTR psz2 = *( LPCTSTR * )pVal2;
	ASSERT( psz2 );
	ASSERT( *psz2 );

	// first sort ascending by first char, then descending by length
	int nCompare = compare_char_i( psz1[ 2 ], psz2[ 2 ] );

	if ( nCompare < 0 )
	{
		return -1;
	}
	else if ( nCompare > 0 )
	{
		return +1;
	}
	else
	{
		if ( *psz1 < *psz2 )
		{
			return +1;
		}
		else if ( *psz1 > *psz2 )
		{
			return -1;
		}
		else
		{
			return 0;
		}
	}
}

void CBuffer::SetReadOnly( BOOL bReadOnly )
{
	if ( bReadOnly != m_bReadOnly )
	{
		m_bReadOnly = bReadOnly;
		if ( m_bReadOnly )
		{
			// no pointing storing undoable transactions if undo is not possible
			ClearRedoChain( TRUE );
		}
	}
}

BOOL CBuffer::FindText( LPCTSTR pszText, int cbText, BOOL bForward, BOOL bCaseSensitive, BOOL bWholeWordOnly, BOOL bRegExp, register int &nLine, register int &nChar, BOOL &bWrapped, int &cbMatched ) const
{
	BOOL bFound = FALSE;
	bWrapped = FALSE;
	if ( m_nLineCount )
	{
		int nStartLine = nLine;
		int nStartChar = nChar;

		int cbFullLine = GetLineLength( nLine );
		int cbLine = ( bRegExp ? cbFullLine :
		                         ( cbFullLine - cbText + 1 ) );
		nChar = min( cbLine, nChar );
		LPCTSTR pszStart = GetLineText( nLine );
		LPCTSTR psz = pszStart + nChar;
		nStartChar = min( nStartChar, cbLine );

		// set up the regular expression state objects
		CSimpleFA *pFABQ = NULL;
		CRegExp *pRE = NULL;
		CFiniteAutomaton *pFA = NULL;

		if ( bRegExp )
		{
			pFABQ = new CSimpleFA();
			pRE = new CRegExp( *pFABQ );
			pRE->ConvertToFiniteAutomaton( pszText );
			pFA = new CFiniteAutomaton( *pFABQ );
		}

		BOOL bContinue = TRUE;

		if ( !bForward )
		{
			// skip to the previous char -- we don't want the current location to be considered
			goto notfound;
		}
		
		while ( bContinue )
		{
			if ( bRegExp )
			{
				if ( RegExpStrCmp( *pFA, psz, cbMatched ) )
				{
					bFound = TRUE;
				}
			}
			else if ( bCaseSensitive )
			{
				if ( ( *psz == *pszText ) && 
					 ( _tcsncmp( pszText, psz, cbText ) == 0 ) )
				{
					cbMatched = cbText;
					bFound = TRUE;
				}
			}
			else
			{
				if ( equal_char_i( *psz, *pszText ) &&
					 ( _tcsnicmp( pszText, psz, cbText ) == 0 ) )
				{
					cbMatched = cbText;
					bFound = TRUE;
				}
			}

			if ( bFound )
			{
				// enforce whole word only option
				if ( bWholeWordOnly && !( ( nChar == 0 || 
					                        !is_alphanumeric( *CharPrev( pszStart, psz ) ) ) &&
					                      ( ( nChar + cbMatched >= cbFullLine ) || 
											!is_alphanumeric( *( psz + cbMatched ) ) ) ))
				{
					bFound = FALSE;
					bContinue = TRUE;
				}
				else
				{
					bContinue = FALSE;
				}
			}

			if ( !bFound )
			{
				notfound:

				if ( bForward )
				{
					int cbChar = _tclen( psz );
					nChar += cbChar;
					psz += cbChar;
					if ( nChar > cbLine )
					{
						nChar = 0;
						if ( ++nLine >= m_nLineCount )
						{
							nLine = 0;
							bWrapped = TRUE;
							bContinue = !m_pCtrl->NotifyParentOfFindWrapped( TRUE );
						}
						if ( bContinue )
							{
							psz = pszStart = GetLineText( nLine );
							cbFullLine = GetLineLength( nLine );
							cbLine = ( bRegExp ? cbFullLine :
												 ( cbFullLine - cbText + 1 ) );
							}
					}

					if ( bWrapped && ( ( nLine > nStartLine ) || 
					                   ( nLine == nStartLine && nChar >= nStartChar ) ) )
					{
						bContinue = FALSE;
					}
				}
				else
				{
					psz = _tcsdec( pszStart, psz );
					if ( !psz )	// past start of line
						nChar = -1;
					else
						nChar -= _tclen( psz );
					if ( nChar < 0 )
					{
						if ( --nLine < 0 )
						{
							nLine = m_nLineCount - 1;
							bWrapped = TRUE;
							bContinue = !m_pCtrl->NotifyParentOfFindWrapped( FALSE );
						}
						if ( bContinue )
							{
							cbFullLine = GetLineLength( nLine );
							cbLine = ( bRegExp ? cbFullLine :
												( cbFullLine - cbText + 1 ) );
							nChar = cbLine;
							pszStart = GetLineText( nLine );
							psz = pszStart + nChar;
							}
					}

					if ( bWrapped && ( ( nLine < nStartLine ) || 
					                   ( nLine == nStartLine && nChar <= nStartChar ) ) )
					{
						bContinue = FALSE;
					}
				}
			}
		}

	// cleanup the regular expression stuff
	delete pFABQ;
	delete pRE;
	delete pFA;
	}

	return bFound;
}

void CBuffer::OnChanged( int nCount )
{
	if ( IsInUndo() )
	{
		m_nChangeLevel -= nCount;
	}
	else
	{
		m_nChangeLevel += nCount;
	}
}

void CBuffer::SetModified( BOOL bModified )
{
	if ( IsModified() != bModified )
	{
		m_bForceModified = bModified;
		if ( !m_bForceModified )
		{
			m_nChangeLevel = 0;
		}
	}
}

BOOL CBuffer::FindChar( TCHAR ch, TCHAR chPair, BOOL bForward, int &nLine, int &nChar ) const
{
	BOOL bFound = FALSE;
	if ( !IsEmpty() )
	{
		int nNestLevel = 0;
		LPCTSTR pszLine = GetLineText( nLine );
		int cbLine = GetLineLength( nLine );
		BOOL bFirstChar = TRUE;
		for ( ;; )
		{
			TCHAR chFound = pszLine[ nChar ];
			if ( chFound == ch )
			{
				if ( --nNestLevel < 0 )
				{
					bFound = TRUE;
					break;
				}
			}
			else if ( !bFirstChar && ( chFound == chPair ) )
			{
				nNestLevel++;
			}

			bFirstChar = FALSE;			
			if ( bForward )
			{
				nChar += _tclen( pszLine + nChar );
				if ( nChar >= cbLine )
				{
					nChar = 0;
					if ( ++nLine >= m_nLineCount )
					{
						// not found
						break;
					}
					else
					{
						cbLine = GetLineLength( nLine );
						pszLine = GetLineText( nLine );
					}
				}
			}
			else
			{
				if ( nChar <= 0 )
				{
					if ( --nLine < 0 )
					{
						// not found
						break;
					}
					else
					{
						cbLine = GetLineLength( nLine );
						pszLine = GetLineText( nLine );
						nChar = cbLine - _tclen_prev( pszLine, pszLine + cbLine );
					}
				}
				else
					nChar -= _tclen_prev( pszLine, pszLine + nChar );
			}
		}

	}
	return bFound;
}

void CBuffer::SetMaxUndo( int nMax )
{
	ClearRedoChain( TRUE );
	m_nMaxUndo = nMax;
}

BOOL CBuffer::ChangedSilently( BOOL bReset )
{
	BOOL bVal = m_bSilentChange;
	if ( bReset )
	{
		m_bSilentChange = FALSE;
	}
	return bVal;
}

void CBuffer::SetLineStyle( int nLine, DWORD dwStyle )
{
	CLine *pLine = GetLine( nLine );
	BOOL bNotifyDel = HAS_FLAG( dwStyle, CML_NOTIFY_DEL );
	if ( HAS_FLAG( pLine->GetStyle(), CML_NOTIFY_DEL ) ^ bNotifyDel )
	{
		// the caller changed CML_NOTIFY_DEL styles -- update the counter
		m_nNotifyDelCount += ( bNotifyDel ? 1 : -1 );
	}
	pLine->SetStyle( dwStyle );
}

void CBuffer::GetTokenText( LangToken eToken, int nTokenID, LPTSTR pszDest ) const
{
	// bail if token is not statically defined -- since we can't lookup the
	// token in a table, we can't do much.
	if (nTokenID == -1)
		return;

	LPCTSTR *ppszTokens = NULL;

	switch ( eToken )
	{
		case eKeyword:						ppszTokens = m_pKeywords;				break;
		case eOperator:						ppszTokens = m_pOperators;				break;
		case eStringDelim:					ppszTokens = m_pStringDelims;			break;
		case eSingleLineComment:			ppszTokens = m_pSingleLineComments;		break;
		case eMultiLineCommentStart:
		case eMultiLineCommentStartAndEnd:	ppszTokens = m_pMultiLineComments1;		break;
		case eMultiLineCommentEnd:			ppszTokens = m_pMultiLineComments2;		break;
		case eScopeKeywordStart:
		case eScopeKeywordStartAndEnd:		ppszTokens = m_pScopeKeywords1;			break;
		case eScopeKeywordEnd:				ppszTokens = m_pScopeKeywords2;			break;
		case eTagElementName:				ppszTokens = m_pTagElementNames;		break;
		case eTagAttributeName:				ppszTokens = m_pTagAttributeNames;		break;
		case eTagEntity:					ppszTokens = m_pTagEntities;			break;
		#ifdef _DEBUG
		case eText:
		case eTagText:
		case eNumber:
		case eEscapeSeq:
		default:	ASSERT( FALSE ); return;
		#endif
	}

	LPCTSTR pszToken = ppszTokens[ nTokenID ];
	if ( pszDest )
		_tcsncpy( pszDest, TOKEN_TEXT( pszToken ), TOKEN_LEN( pszToken ) );
}

void CBuffer::NormalizeLineTextCase( int nLine )
{
	Lock();

	if ( m_bNormalizeCase && ( nLine >= 0 ) && ( nLine < GetLineCount() ) && GetLineLength( nLine ) )
	{
		LPTSTR pszLine = GetLine( nLine )->GetTextForWrite();
		BOOL bChanged = FALSE;

		CLineParser Parser( this, nLine );
		while ( Parser.MoreComing() )
		{
			Parser.AcceptToken();
			switch ( Parser.m_eToken )
			{
				case eKeyword:
				case eOperator:
				case eSingleLineComment:
				case eStringDelim:
				case eMultiLineCommentStart:
				case eMultiLineCommentStartAndEnd:
				case eMultiLineCommentEnd:
				case eScopeKeywordStart:
				case eScopeKeywordStartAndEnd:
				case eScopeKeywordEnd:
				case eTagElementName:
				case eTagAttributeName:
				case eTagEntity:
				{
					if ( ( !Parser.m_bInComment && !Parser.m_bInString ) ||
					     ( Parser.m_bInComment && !Parser.m_bWasInComment ) ||
					     ( Parser.m_bInString && !Parser.m_bWasInString ) )
					{
						GetTokenText( Parser.m_eToken, Parser.m_nTokenOffset, pszLine + Parser.m_nPosBefore );
						bChanged = TRUE;
					}
					break;
				}

				case eText:
				case eTagText:
				case eNumber:
				case eEscapeSeq:
				{
					// just leave the text as-is
					break;
				}

				#ifdef _DEBUG
				default:	ASSERT( FALSE ); return;
				#endif
			}
		}
		if ( bChanged )
			Notify( eLineChangedSilent, nLine, 0, 1 );
	}

	Unlock();
}

int CBuffer::GetCharSize( int nLine, int nCol ) const
{
	if ( nLine >= 0 && nLine < m_nLineCount )
	{
		CLine *pLine = GetLine( nLine );
		ASSERT( pLine );
		if ( nCol >= 0 && nCol < pLine->GetLength() )
		{
		return _tclen( pLine->GetText() + nCol );
		}
	}

	return 1;
}
