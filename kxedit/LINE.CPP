#include "precomp.h"
#include "buffer.h"

CLine::CLine( LPCTSTR pszLine, int cbLine, WORD wFlags )
{
	m_pszText = NULL;
	m_nLength = 0;
	m_nLengthAllocated = 0;
	m_wFlags = wFlags;
	m_lParam = 0;
	m_dwStyle = 0;
	m_byImages = 0;
	if ( pszLine && cbLine )
	{
		AddChars( 0, pszLine, cbLine );
	}
}
CLine::~CLine()
{
	if ( m_pszText )
	{
		free( m_pszText );
	}
}

int CLine::AddChars( int nIndexBefore, LPCTSTR pszText, int cbText )
{
	ASSERT( nIndexBefore >= 0 );
	EnsureTextSize( max( m_nLength, nIndexBefore ) + cbText + 1 );

	// make room for the new chars
	memmove( m_pszText + nIndexBefore + cbText, 
	         m_pszText + nIndexBefore,
	         ( m_nLength - nIndexBefore ) * sizeof( TCHAR ) );
	// copy in the new chars
	_tcsncpy( m_pszText + nIndexBefore, pszText, cbText );
	m_nLength = m_nLength + cbText;
	m_pszText[ m_nLength ] = _T('\0');
	ASSERT( *m_pszText != ( TCHAR ) -1 );

	return m_nLength;
}

int CLine::RemoveChars( int nIndexStart, int nChars )
{
	ASSERT( m_pszText != NULL );
	ASSERT( nIndexStart >= 0 );

	// consume the removed chars
	int nMaxRemove = m_nLength - nIndexStart;
	nChars = min( nChars, nMaxRemove );

	if ( nChars )
	{
		LPTSTR pszAt = m_pszText + nIndexStart;

		// shift the text
		memmove( pszAt,
				 pszAt + nChars, 
				 ( m_nLength - nIndexStart - nChars + 1 ) * sizeof( TCHAR ) );
		m_nLength -= nChars;
	}

	return m_nLength;
}

void CLine::EnsureTextSize( int nCharsRequired )
{
	if ( nCharsRequired > m_nLengthAllocated )
	{
		m_nLengthAllocated = ( nCharsRequired + GROWBY );
		int cbNewSize = m_nLengthAllocated * sizeof( TCHAR );
		// need to realloc
		m_pszText = ( m_pszText ? 
		            ( LPTSTR )realloc( m_pszText, cbNewSize ) :
				    ( LPTSTR )malloc( cbNewSize ) );
 	}
}


void CLine::SetBookmark( BOOL bOn )
{
	if ( bOn )
		m_wFlags |= BOOKMARK;
	else
		m_wFlags &= ~BOOKMARK;
}

void CLine::SetDivider( BOOL bOn )
{
	if ( bOn )
		m_wFlags |= DIVIDER;
	else
		m_wFlags &= ~DIVIDER;
}

BOOL CLine::IsInComment( int &nCommentStyle ) const
{
	nCommentStyle = ( m_wFlags & COMMENT_STYLE ) >> COMMENT_SHIFT;
	return HAS_FLAG( m_wFlags, IN_COMMENT );

}

void CLine::SetInComment( BOOL bIn, int nCommentStyle )
{
	m_wFlags &= ~COMMENT_STYLE;
	if ( bIn )
	{
		m_wFlags |= IN_COMMENT;
		m_wFlags |= ( ( WORD ) nCommentStyle ) << COMMENT_SHIFT;
	}
	else
	{
		m_wFlags &= ~IN_COMMENT;
	}
}

BOOL CLine::IsInString( int &nStringStyle ) const
{
	nStringStyle = ( m_wFlags & STRING_STYLE ) >> STRING_SHIFT;
	return HAS_FLAG( m_wFlags, IN_STRING );

}

void CLine::SetInString( BOOL bIn, int nStringStyle )
{
	m_wFlags &= ~STRING_STYLE;
	if ( bIn )
	{
		m_wFlags |= IN_STRING;
		m_wFlags |= ( ( WORD ) nStringStyle ) << STRING_SHIFT;
	}
	else
	{
		m_wFlags &= ~IN_STRING;
	}
}

BOOL CLine::IsInTag( int &nTagStyle ) const
{
	nTagStyle = ( m_wFlags & TAG_STYLE ) >> TAG_SHIFT;
	return HAS_FLAG( m_wFlags, IN_TAG );

}

void CLine::SetInTag( BOOL bIn, int nTagStyle )
{
	m_wFlags &= ~TAG_STYLE;
	if ( bIn )
	{
		m_wFlags |= IN_TAG;
		m_wFlags |= ( ( WORD ) nTagStyle ) << TAG_SHIFT;
	}
	else
	{
		m_wFlags &= ~IN_TAG;
	}
}

BOOL CLine::IsHighlighted() const
{
	return HAS_FLAG( m_wFlags, IS_HIGHLIGHTED );

}

void CLine::SetHighlighted( BOOL bOn )
{
	if ( bOn )
	{
		m_wFlags |= IS_HIGHLIGHTED;
	}
	else
	{
		m_wFlags &= ~IS_HIGHLIGHTED;
	}
}
