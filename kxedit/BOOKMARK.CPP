#include "precomp.h"
#include "editdlgs.h"
#include "cedit.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Bookmarks
//
//

void CEdit::BookmarkToggle()
{
	if ( m_Buffer.GetLineCount() )
	{
		int nLine = m_Selection.GetEndRow();
		m_Buffer.ToggleBookmark( nLine );
		DamageAllViews( nLine, nLine );
	}
}

void CEdit::BookmarkPrev()
{
	if ( m_Buffer.GetLineCount() )
	{
		int nLine = GetAdjacentBookmark( m_Selection.GetEndRow(), TRUE );
		if ( nLine != -1 )
		{
			GoToLine( nLine, !m_pActiveView->LineIsVisible( nLine ) );
		}
	}
}

void CEdit::BookmarkNext()
{
	if ( m_Buffer.GetLineCount() )
	{
		int nLine = GetAdjacentBookmark( m_Selection.GetEndRow(), FALSE );
		if ( nLine != -1 )
		{
			GoToLine( nLine, !m_pActiveView->LineIsVisible( nLine ) );
		}
	}
}

void CEdit::BookmarkClearAll()
{
	int nLineCount = m_Buffer.GetLineCount();
	for ( int nLine = 0; nLine < nLineCount; nLine++ )
	{
		if ( m_Buffer.HasBookmark( nLine ) )
		{
			m_Buffer.RemoveBookmark( nLine );
		}
	}

	Repaint( FALSE );
}

void CEdit::BookmarkJumpToFirst()
{
	if ( m_Buffer.GetLineCount() )
	{
		int nLine = GetAdjacentBookmark( -1, FALSE );
		if ( nLine != -1 )
		{
			GoToLine( nLine, !m_pActiveView->LineIsVisible( nLine ) );
		}
	}
}

void CEdit::BookmarkJumpToLast()
{
	int nLineCount = m_Buffer.GetLineCount();
	if ( nLineCount )
	{
		int nLine = GetAdjacentBookmark( nLineCount, TRUE );
		if ( nLine != -1 )
		{
			GoToLine( nLine, !m_pActiveView->LineIsVisible( nLine ) );
		}
	}
}

int CEdit::GetAdjacentBookmark( int nLine, BOOL bPrev ) const
{
	int nStartLine = nLine;
	int nLineCount = m_Buffer.GetLineCount();
	ASSERT( nLineCount );
	
	if ( bPrev )
	{
		for ( --nLine; nLine >= 0; nLine-- )
		{
			if ( m_Buffer.HasBookmark( nLine ) )
			{
				return nLine;
			}
		}
		for ( nLine = nLineCount - 1; nLine >= nStartLine; nLine-- )
		{
			if ( m_Buffer.HasBookmark( nLine ) )
			{
				return nLine;
			}
		}
	}
	else
	{
		for ( ++nLine; nLine < nLineCount; nLine++ )
		{
			if ( m_Buffer.HasBookmark( nLine ) )
			{
				return nLine;
			}
		}
		for ( nLine = 0; nLine <= nStartLine; nLine++ )
		{
			if ( m_Buffer.HasBookmark( nLine ) )
			{
				return nLine;
			}
		}
	}
	return -1;
}
