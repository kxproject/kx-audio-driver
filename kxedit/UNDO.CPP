#include "precomp.h"
#include "buffer.h"
#include "undo.h"

CUndoAction::CUndoAction( BOOL bInsert )
{
	m_fFlags = 0;
	if ( bInsert )
	{
		m_fFlags |= BIT_INSERT;
	}
}

void CUndoAction::FlagAsStartAction()
{
	m_fFlags |= BIT_START;
}

void CUndoAction::FlagAsEndAction()
{
	m_fFlags |= BIT_END;
}

void CUndoAction::EnableAppend()
{
	m_fFlags |= BIT_APPEND;
}

BOOL CUndoAction::CanAppend() const
{
	return HAS_FLAG( m_fFlags, BIT_APPEND );
}

BOOL CUndoAction::IsStartAction() const
{
	return HAS_FLAG( m_fFlags, BIT_START );
}

BOOL CUndoAction::IsEndAction() const
{
	return HAS_FLAG( m_fFlags, BIT_END );
}

BOOL CUndoAction::IsInsertAction() const
{
	return HAS_FLAG( m_fFlags, BIT_INSERT );
}

BOOL CUndoAction::Append( BOOL /* bInsertChar */, TCHAR /* ch */, int /* nRow */, int /* nCol */ )
{
	return FALSE;
}

void CUndoAction::SetCol( int /* nCol */ )
{
}

CUndoInsertDeleteString::CUndoInsertDeleteString( BOOL bInsert, LPCTSTR pszText, int cbText, int nRow, int nCol )
						 : CUndoAction( bInsert )
{
	ASSERT( pszText );
	ASSERT( cbText );
	m_nCol = nCol;
	m_nRow = nRow;
	m_pszText = ( LPTSTR ) malloc( ( cbText + 1 ) * sizeof( TCHAR ) );
	_tcsncpy( m_pszText, pszText, cbText );
	m_pszText[ cbText ] = _T('\0');
}

CUndoInsertDeleteString::~CUndoInsertDeleteString()
{
	ASSERT( m_pszText );
	free( m_pszText );
}

void CUndoInsertDeleteString::Undo( CBuffer *pBuffer ) const
{
	if ( IsEndAction() )
	{
		pBuffer->BeginEdit();
	}
	if ( IsInsertAction() )
	{
		pBuffer->RemoveCharsFromLine( m_nRow, m_nCol, _tcslen( m_pszText ) );
	}
	else
	{
		pBuffer->InsertCharsIntoLine( m_nRow, m_nCol, m_pszText );
	}
	if ( IsStartAction() )
	{
		pBuffer->EndEdit();
	}
}

void CUndoInsertDeleteString::Redo( CBuffer *pBuffer ) const
{
	if ( IsStartAction() )
	{
		pBuffer->BeginEdit();
	}
	if ( IsInsertAction() )
	{
		pBuffer->InsertCharsIntoLine( m_nRow, m_nCol, m_pszText );
	}
	else
	{
		pBuffer->RemoveCharsFromLine( m_nRow, m_nCol, _tcslen( m_pszText ) );
	}
	if ( IsEndAction() )
	{
		pBuffer->EndEdit();
	}
}

BOOL CUndoInsertDeleteString::Append( BOOL bInsertChar, TCHAR ch, int nRow, int nCol )
{
	BOOL bSuccess = FALSE;
	if ( ( nRow == m_nRow ) && CanAppend() )
	{
		int cbText = _tcslen( m_pszText );
		ASSERT( cbText );

		if ( bInsertChar )
		{
			if ( ( m_nCol + cbText ) == nCol )
			{
				// now, only allow appending if ch is same 'kind' of text as what's in 
				// this record.
				BOOL bAlpha = is_alphanumeric( ch );
				BOOL bCanAppend = TRUE;
				for ( register int i = 0; i < cbText; i++ )
				{
					TCHAR chText = m_pszText[ i ];
					if ( bAlpha )
					{
						if ( !is_alphanumeric( chText ) )
						{
							bCanAppend = FALSE;
						}
					}
					else
					{
						if ( is_alphanumeric( chText ) )
						{
							bCanAppend = FALSE;
						}
					}
				}

				if ( bCanAppend )
				{
					// can append to this record
					m_pszText = ( LPTSTR ) realloc( m_pszText, ( cbText + 2 ) * sizeof( TCHAR ) );
					m_pszText[ cbText ] = ch;
					m_pszText[ cbText + 1 ] = '\0';
					bSuccess = TRUE;
				}
			}
		}
	}
	return bSuccess;
}

CUndoAddRemoveLine::CUndoAddRemoveLine( BOOL bInsert, LPCTSTR pszText, int cbText, int nRow ) :
                    CUndoAction( bInsert )
{
	if ( pszText )
	{
		ASSERT( cbText == ( int ) _tcslen( pszText ) );
		m_pszText = ( LPTSTR ) malloc( ( cbText + 1 ) * sizeof( TCHAR ) );
		_tcsncpy( m_pszText, pszText, cbText );
		m_pszText[ cbText ] = _T('\0');
	}
	else
	{
		m_pszText = NULL;
	}

	m_nRow = nRow;
}

CUndoAddRemoveLine::~CUndoAddRemoveLine()
{
	if ( m_pszText )
	{
		free( m_pszText );
	}
}

void CUndoAddRemoveLine::Undo( CBuffer *pBuffer ) const
{
	if ( IsEndAction() )
	{
		pBuffer->BeginEdit();
	}
	if ( IsInsertAction() )
	{
		pBuffer->RemoveLines( m_nRow, 1 );
	}
	else
	{
		pBuffer->InsertLine( m_nRow, m_pszText ? m_pszText : _T(""), -1 );
	}
	if ( IsStartAction() )
	{
		pBuffer->EndEdit();
	}
}

void CUndoAddRemoveLine::Redo( CBuffer *pBuffer ) const
{
	if ( IsStartAction() )
	{
		pBuffer->BeginEdit();
	}
	if ( IsInsertAction() )
	{
		pBuffer->InsertLine( m_nRow, m_pszText ? m_pszText : _T(""), -1 );
	}
	else
	{
		pBuffer->RemoveLines( m_nRow, 1 );
	}
	if ( IsEndAction() )
	{
		pBuffer->EndEdit();
	}
}

CUndoStartEnd::CUndoStartEnd( int nCursorLine, int nCursorCol ) :
                CUndoAction( FALSE )
{
	m_nRow = nCursorLine;
	m_nCol = nCursorCol;
}

void CUndoStartEnd::Undo( CBuffer *pBuffer ) const
{
	if ( IsStartAction() && m_nRow >= 0 && m_nCol >= 0 )
	{
		pBuffer->Notify( CBuffer::eSetCursorPos, m_nRow, m_nCol, -1 );
	}
}

void CUndoStartEnd::Redo( CBuffer *pBuffer ) const
{
	if ( IsEndAction() && m_nRow >= 0 && m_nCol >= 0 )
	{
		pBuffer->Notify( CBuffer::eSetCursorPos, m_nRow, m_nCol, -1 );
	}
}

void CUndoStartEnd::SetCol( int nCol )
{
	m_nCol = nCol;
}
