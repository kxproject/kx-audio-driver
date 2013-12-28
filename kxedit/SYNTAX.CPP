#include "precomp.h"
#include "undo.h"
#include "misc.h"
#include "buffer.h"

BOOL CBuffer::LookupToken( LPCTSTR *ppszTokens, int nTokens, register LPCTSTR psz, int nLen, int &nTokenLen, BOOL bCheckSurrounding, TCHAR chLeading, int &nTokenID, int &nTokenOffset, BOOL bForceCaseInsensitive ) const
{
	ASSERT( ppszTokens );
	ASSERT( psz );
	ASSERT( *psz );
	ASSERT( nLen );

	// do simple binary search
	int nStart = 0;
	int nEnd = nTokens - 1;
	register int nPosFound = -1;
	TCHAR chFirst = *psz;

	BOOL bFound = FALSE;
	register LPCTSTR pszToken = NULL;
	BOOL bCaseSensitive = bForceCaseInsensitive ? FALSE : LanguageIsCaseSensitive();

	while ( nStart <= nEnd )
	{
		nPosFound = ( nStart + nEnd ) / 2;
		pszToken = ppszTokens[ nPosFound ];
		TCHAR chFound = pszToken[ 2 ];
		if ( compare_char_i( chFirst, chFound ) == 0 )
		{
			int nPosLast = nPosFound;
			// scan backwards to the first token with the same first char
			while ( nPosFound >= 0 && compare_char_i( chFirst, ppszTokens[ nPosFound ][ 2 ] ) == 0 )
			{
				nPosLast = nPosFound;
				nPosFound--;
			}
			nPosFound = nPosLast;

			// now, scan forward until a matching token is located
			while ( nPosFound <= nEnd && compare_char_i( chFirst, ppszTokens[ nPosFound ][ 2 ] ) == 0 )
			{
				pszToken = ppszTokens[ nPosFound ];
				int nTempLen = TOKEN_LEN( pszToken );
				if ( ( nTempLen <= nLen ) && 
				     ( bCaseSensitive ? ( _tcsncmp( TOKEN_TEXT( pszToken ), psz, nTempLen ) == 0 ) :
				                        ( _tcsnicmp( TOKEN_TEXT( pszToken ), psz, nTempLen ) == 0 ) ) )
				{
					if ( bCheckSurrounding )
					{
						TCHAR chTrailing = psz[ nTempLen ];
						BOOL bIsAlphaPre = TOKEN_IS_ALPHA_PRE( pszToken );		// token begins with a letter or number
						BOOL bIsAlphaPost = TOKEN_IS_ALPHA_POST( pszToken );	// token ends with a letter or number
						if ( ( !chLeading || 
						       chLeading == m_chTerminator || 
							   ( !bIsAlphaPre || !is_alphanumeric( chLeading ) ) ||	// token begins with non-alpha or ( token begins with alpha and prev char is non-alpha )
							   is_space( chLeading ) || 
							   is_eoln( chLeading ) )
							  &&
							 ( !chTrailing ||
							   chTrailing == m_chTerminator ||
							   ( !bIsAlphaPost || !is_alphanumeric( chTrailing ) ) ||	// token ends with non-alpha or ( token ends with alpha and next char is non-alpha )
							   is_space( chTrailing ) ||
							   is_eoln( chTrailing ) ) )
						{
							// found a token!
							bFound = TRUE;
							goto bail;
						}
					}
					else
					{
						bFound = TRUE;
						goto bail;
					}
				}

				// not found
				nPosFound++;
			}

			// not found
			goto bail;
		}
		else if ( compare_char_i( chFirst, chFound ) < 0 )
		{
			nEnd = nPosFound - 1;
		}
		else
		{
			nStart = nPosFound + 1;
		}
	}

	bail:
	if ( bFound )
	{
		nTokenLen = TOKEN_LEN( pszToken );
		nTokenID = TOKEN_ID( pszToken );
		nTokenOffset = nPosFound;
	}

	return bFound;
}

CBuffer::LangToken CBuffer::FindToken( LPCTSTR pszBuff, int nPos, LPCTSTR pszEnd, int &nTokenLen, BOOL &bReachedEnd, int &nTokenID, int &nTokenID2, BOOL bWantRawLen, int &nTokenOffset ) const
{
	LPCTSTR psz = pszBuff + nPos;
	int nLenRemaining = pszEnd - psz;
	LangToken eToken = (_T('0') <= *psz && *psz <= _T('9')) ? CBuffer::eNumber : CBuffer::eText;
	nTokenLen = nLenRemaining ? _tclen(pszBuff + nPos) : 0;
	nTokenID = 0;
	nTokenID2 = -1;

	if ( nLenRemaining )
	{
		WORD wMap = m_CharIsKeyword[ ( BYTE ) *psz ];
		if ( wMap )
		{
			TCHAR chLeading = nPos ? pszBuff[ nPos - 1 ] : _T('\0');
	
			if ( HAS_FLAG( wMap, CHAR_KEYWORD ) && LookupToken( m_pKeywords, m_nKeywords, psz, nLenRemaining, nTokenLen, TRUE, chLeading, nTokenID, nTokenOffset, FALSE ) )
			{
				eToken = eKeyword;
				goto bail;
			}
			if ( HAS_FLAG( wMap, CHAR_TAG_ENTITY ) && LookupToken( m_pTagEntities, m_nTagEntities, psz, nLenRemaining, nTokenLen, TRUE, chLeading, nTokenID, nTokenOffset, FALSE ) )
			{
				eToken = eTagEntity;
				goto bail;
			}
			
			// literal entities (e.g. &#123;)
			if ( psz[ 0 ] == _T('&') && psz[ 1 ] == _T('#') && LanguageIsSGML() )
			{
				LPCTSTR pszTemp = psz + 2;
				BOOL bHasNumbers = FALSE;
				while ( _istdigit( *pszTemp ) )
					{
					bHasNumbers = TRUE;
					pszTemp++;
					}
				if ( bHasNumbers && *pszTemp == _T(';') )
				{
					eToken = eTagEntity;
					nTokenLen = pszTemp - psz + 1;
					nTokenID = nTokenID2 = nTokenOffset = -1;
					bReachedEnd = ( pszTemp[ 1 ] == _T('\0') );
					goto bail;
				}
			}

			if ( HAS_FLAG( wMap, CHAR_TAG_ELEMENT_NAME ) && LookupToken( m_pTagElementNames, m_nTagElementNames, psz, nLenRemaining, nTokenLen, TRUE, chLeading, nTokenID, nTokenOffset, FALSE ) )
			{
				eToken = eTagElementName;
				goto bail;
			}
			if ( HAS_FLAG( wMap, CHAR_TAG_ATTRIBUTE_NAME ) && LookupToken( m_pTagAttributeNames, m_nTagAttributeNames, psz, nLenRemaining, nTokenLen, TRUE, chLeading, nTokenID, nTokenOffset, FALSE ) )
			{
				eToken = eTagAttributeName;
				goto bail;
			}
			if ( HAS_FLAG( wMap, CHAR_STRING ) && LookupToken( m_pStringDelims, m_nStringDelims, psz, nLenRemaining, nTokenLen, FALSE, chLeading, nTokenID, nTokenOffset, FALSE ) )
			{
				eToken = eStringDelim;
				goto bail;
			}
			
			BOOL bStartToken = FALSE;
			int nTokenID2;
			if ( HAS_FLAG( wMap, CHAR_MLCOMMENT1 ) && LookupToken( m_pMultiLineComments1, m_nMultiLineComments, psz, nLenRemaining, nTokenLen, FALSE, _T('\0'), nTokenID, nTokenOffset, FALSE ) )
			{
				eToken = eMultiLineCommentStart;
				bStartToken = TRUE;
			}

			int nTokenOffset2;
			if ( HAS_FLAG( wMap, CHAR_MLCOMMENT2 ) && LookupToken( m_pMultiLineComments2, m_nMultiLineComments, psz, nLenRemaining, nTokenLen, FALSE, _T('\0'), nTokenID2, nTokenOffset2, FALSE ) )
			{
				if ( bStartToken )
				{
					eToken = eMultiLineCommentStartAndEnd;
					goto bail;
				}
				else
				{
					nTokenID = nTokenID2;
					nTokenOffset = nTokenOffset2;
					eToken = eMultiLineCommentEnd;
					goto bail;
				}
			}
			
			if ( bStartToken )
			{
				goto bail;
			}

			if ( HAS_FLAG( wMap, CHAR_SLCOMMENT ) && LookupToken( m_pSingleLineComments, m_nLineComments, psz, nLenRemaining, nTokenLen, TRUE, chLeading, nTokenID, nTokenOffset, FALSE ) )
			{	
				if ( !bWantRawLen )
				{
					nTokenLen = pszEnd - psz;
				}
				eToken = eSingleLineComment;
				goto bail;
			}

			if ( HAS_FLAG( wMap, CHAR_OPERATOR ) && LookupToken( m_pOperators, m_nOperators, psz, nLenRemaining, nTokenLen, TRUE, chLeading, nTokenID, nTokenOffset, FALSE ) )
			{
				eToken = eOperator;
				goto bail;
			}
			
			if ( *psz == m_chEscape && *psz != _T('\t') && nLenRemaining > 1 )
			{
				eToken = eEscapeSeq;
				nTokenLen = 1 + _tclen(psz+1);	// escape char + next char
				goto bail;
			}
			// scope words can exist in both scope word lists (e.g. in Basic, 'ELSE' is a scope start word as well as a scope end word)
			if ( HAS_FLAG( wMap, CHAR_SCOPE1 ) && LookupToken( m_pScopeKeywords1, m_nScopeKeywords, psz, nLenRemaining, nTokenLen, TRUE, chLeading, nTokenID, nTokenOffset, FALSE ) )
			{
				eToken = eScopeKeywordStart;
				bStartToken = TRUE;
			}
			if ( HAS_FLAG( wMap, CHAR_SCOPE2 ) && LookupToken( m_pScopeKeywords2, m_nScopeKeywords, psz, nLenRemaining, nTokenLen, TRUE, chLeading, nTokenID2, nTokenOffset2, FALSE ) )
			{
				if ( bStartToken )
				{
					// nTokenID2 has meanings
					eToken = eScopeKeywordStartAndEnd;
					goto bail;
				}
				else
				{
					nTokenID = nTokenID2;
					nTokenOffset = nTokenOffset2;
					eToken = eScopeKeywordEnd;
					goto bail;
				}
			}

			if ( bStartToken )
			{
				goto bail;
			}
		}
	}

bail:

	bReachedEnd = ( nTokenLen == nLenRemaining );
	return eToken;
}

void CBuffer::GetNextToken( register LangToken &eToken, int &nTokenLen, LPCTSTR psz, int &nPos, LPCTSTR pszEnd, BOOL bIsInComment, BOOL bIsInString, BOOL bIsInTag, BOOL &bHasTab, int &nTokenID, int &nTokenID2, BOOL &bMoreComing, int &nTokenOffset ) const
{
	ASSERT( psz + nPos < pszEnd );	// already at the end of the buffer
	ASSERT( psz );
	nTokenLen = 0;
	register int nTempLen;
	bHasTab = FALSE;
	BOOL bReachedEnd;
	eToken = FindToken( psz, nPos, pszEnd, nTempLen, bReachedEnd, nTokenID, nTokenID2, FALSE, nTokenOffset );
	if ( !bIsInTag && LanguageIsSGML() && ( eToken != eScopeKeywordStart &&
	                                        eToken != eTagEntity &&
	                                        eToken != eMultiLineCommentStart &&
	                                        eToken != eMultiLineCommentEnd &&
	                                        eToken != eMultiLineCommentStartAndEnd ) )
	{
		eToken = CBuffer::eText;
		goto build_text_token;
	}
	else
		switch ( eToken )
		{
			case eMultiLineCommentEnd:
			{
				// if not in a comment, don't treat this as a comment end token
				if ( !bIsInComment )
				{
					eToken = CBuffer::eText;
					nTokenLen = 1;
					nPos++;
					bReachedEnd = ( ( psz + nPos ) >= pszEnd );
					break;
				}
				// else fall through...
			}
			case eKeyword:
			case eOperator:
			case eEscapeSeq:
			case eMultiLineCommentStart:
			case eMultiLineCommentStartAndEnd:
			case eScopeKeywordStart:
			case eScopeKeywordEnd:
			case eScopeKeywordStartAndEnd:
			case eTagEntity:
			case eTagElementName:
			case eTagAttributeName:
			{
				nTokenLen = nTempLen;
				nPos += nTokenLen;
				break;
			}

			case eText:
			{
				if ( bIsInTag && LanguageIsSGML() )
					eToken = eTagText;

build_text_token:

				nTokenLen = 0;
				BOOL bAtEndOfLine;
				do
				{
					bHasTab |= ( psz[ nPos ] == _T('\t') );
					nTokenLen += nTempLen;
					nPos += nTempLen;
					bAtEndOfLine = bReachedEnd;
				}
				while ( !bAtEndOfLine && ( FindToken( psz, nPos, pszEnd, nTempLen, bReachedEnd, nTokenID, nTokenID2, FALSE, nTokenOffset ) == eText ) );
				nTokenOffset = 0;
				bReachedEnd = bAtEndOfLine;
				break;
			}

			case eNumber:
			{
				nTokenLen = 0;
				bHasTab = FALSE;
				int nStartPos = nPos;
				LPCTSTR pszNum = psz + nPos;
				LPCTSTR pszStart = pszNum;
				BOOL bStartsWithPeriod = *pszNum == _T('.');
				if ( bStartsWithPeriod )
					goto parse_after_decimal_point;

				while ( is_numeric( *pszNum ) )
					pszNum++;

				if ( *pszNum == _T('.') )
					pszNum++;
				else
					goto end_number_parse;
	
				parse_after_decimal_point:

				while ( is_numeric( *pszNum ) )
					pszNum++;


				end_number_parse:

				if ( bStartsWithPeriod && pszNum == pszStart )
					{
					// just a single period -- not a number -- treat as text
					goto build_text_token;
					}
				else
					{
					nPos += ( pszNum - pszStart );
					nTokenLen += ( pszNum - pszStart );
					}
				nTokenOffset = 0;
				bReachedEnd = ( *pszNum == _T('\0') );

				// make sure numbers are not surrounded by alpha chars.
				// if so, then this token is really just text			
				eToken = ( ( !nStartPos || !is_alpha( psz[ nStartPos - 1 ] ) ) &&
						   !is_alpha( psz[ nPos ] ) ) ? CBuffer::eNumber : ( bIsInTag ? CBuffer::eTagText : CBuffer::eText );
				break;
			}

			case eSingleLineComment:
			{
				// if in a comment or string, don't treat this as a single-line comment
				if ( bIsInComment || bIsInString )
				{
					eToken = CBuffer::eText;
					nTokenLen = 1;
					nPos++;
					bReachedEnd = ( ( psz + nPos ) >= pszEnd );
					break;
				}

				nTokenLen = nTempLen;
				nPos += nTokenLen;
				LPCTSTR pszTemp = psz;
				while ( !bHasTab && pszTemp < pszEnd )
				{
					bHasTab |= ( *pszTemp == _T('\t') );
					pszTemp++;
				}
				bReachedEnd = TRUE;
				break;
			}
			case eStringDelim:
			{
				nTokenLen = 0;
				// if in a comment, treat the string delim as plain text
				if ( bIsInComment )
				{
					eToken = eText;
				}
				nTokenLen += nTempLen;
				nPos += nTempLen;
				break;
			}
		}

	bMoreComing = !bReachedEnd;
}

DWORD PASCAL SyntaxParserThread( CBuffer *pBuffer )
{
	ASSERT( pBuffer );
	pBuffer->SyntaxParserThread();
	return 0;
}

void CBuffer::StartSyntaxParse()
{
	m_nStartParseAt = 0;
	m_nCanStopParseOn = -1;	// the end of the buffer
	ASSERT( !m_hSyntaxEvent );
	m_hSyntaxEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
	ASSERT( m_hSyntaxEvent );

	ASSERT( !m_hSyntaxThread );
	DWORD dwID;
	m_hSyntaxThread = CreateThread( NULL, 0, ( LPTHREAD_START_ROUTINE ) ::SyntaxParserThread, this, 0, &dwID );
	ASSERT( m_hSyntaxThread );
}

void CBuffer::EndSyntaxParse()
{
	if ( m_hSyntaxThread )
	{
		Lock();
		m_byteSyntaxCmd |= SHUTDOWN;
		Unlock();
		ASSERT( m_hSyntaxEvent );
		SetEvent( m_hSyntaxEvent );
		VERIFY( WaitForSingleObject( m_hSyntaxThread, 3000 ) == WAIT_OBJECT_0 );
		CloseHandle( m_hSyntaxThread );
		m_hSyntaxThread = NULL;
	}

	if ( m_hSyntaxEvent )
	{
		CloseHandle( m_hSyntaxEvent );
		m_hSyntaxEvent = NULL;
	}
}

void CBuffer::NotifySyntaxParser( int nChangedLine )
{
	ASSERT( nChangedLine < GetLineCount() );
	if ( HasLanguage() )
	{
		ASSERT( m_hSyntaxThread );
		Lock();
		m_nStartParseAt = min( m_nStartParseAt, nChangedLine );
		if ( m_nCanStopParseOn != -1 )
		{
			m_nCanStopParseOn = max( m_nCanStopParseOn, nChangedLine );
		}
		Unlock();

		if ( m_bAllowParse )
		{
			ASSERT( m_hSyntaxEvent );
			SetEvent( m_hSyntaxEvent );
		}
	}
}

void CBuffer::Lock()
{
	EnterCriticalSection( &m_csSyntax );
}

void CBuffer::Unlock()
{
	LeaveCriticalSection( &m_csSyntax );
}

void CBuffer::SyntaxParserThread()
{
	CLineParser Parser( this );
	for( ;; )
	{
		ASSERT( m_hSyntaxEvent );

		BOOL bContinue = TRUE;

		while ( bContinue )
		{
			if ( HAS_FLAG( m_byteSyntaxCmd, SHUTDOWN ) )
			{
				// Buffer requested that this thread end
				m_byteSyntaxCmd = 0;
				return;
			}
			
			Lock();
			int nLineCount = GetLineCount();
			if ( nLineCount && ( m_nStartParseAt + 1 < nLineCount ) )
			{
				int nMinEndLine = ( ( m_nCanStopParseOn == -1 || m_nCanStopParseOn >= nLineCount ) ? nLineCount - 1 : m_nCanStopParseOn );
				ASSERT( nMinEndLine <= nLineCount );

				// parse the line and determine if line (nCurrLine+1) starts in a comment
				if ( m_nStartParseAt == 0 )
				{
					SetInComment( 0, FALSE, 0 );
					SetInString( 0, FALSE, 0 );
					SetInTag( 0, FALSE, 0 );
				}
				
				Parser.SetLine( m_nStartParseAt );

				if ( !LineIsEmpty( m_nStartParseAt, FALSE ) )
				{
					do
					{
						Parser.AcceptToken();
					}
					while ( Parser.MoreComing() );
				}

				m_nStartParseAt++;
				int nTemp = 0;
				BOOL bMismatchedCommentState = ( Parser.m_bInComment != IsInComment( m_nStartParseAt, nTemp ) ) || 
				                               ( Parser.m_nCommentStyle != nTemp );  
				BOOL bMismatchedStringState = ( Parser.m_bInString != IsInString( m_nStartParseAt, nTemp ) ) || 
				                              ( Parser.m_nStringStyle != nTemp );  
				BOOL bMismatchedTagState = ( Parser.m_bInTag != IsInTag( m_nStartParseAt, nTemp ) ) || 
				                           ( Parser.m_nTagStyle != nTemp );  
							  
				bContinue = ( bMismatchedCommentState ||
				              bMismatchedStringState ||
							  bMismatchedTagState ||
							  ( m_nStartParseAt <= nMinEndLine ) );

				if ( bMismatchedCommentState || bMismatchedStringState || bMismatchedTagState )
				{
					SetInComment( m_nStartParseAt, Parser.m_bInComment, Parser.m_nCommentStyle );
					SetInString( m_nStartParseAt, Parser.m_bInString, Parser.m_nStringStyle );
					SetInTag( m_nStartParseAt, Parser.m_bInTag, Parser.m_nTagStyle );
					Notify( eLineChangedSilent, m_nStartParseAt, 0, 1 );
				}
				if ( !bContinue )
				{
					// reset the starting point
					m_nStartParseAt = nLineCount - 1;
				}
				ASSERT( !bContinue || m_nStartParseAt < nLineCount );
			}
			else
			{
				bContinue = FALSE;
			}
			Unlock();
		}

		Lock();
		m_nCanStopParseOn = 0; // everthing is clean
		Unlock();

		WaitForSingleObject( m_hSyntaxEvent, INFINITE );
	}
}

void CBuffer::WaitForLine( int nLine )
{
	ASSERT( nLine >= 0 && nLine < GetLineCount() );
	if ( m_bAllowParse && HasLanguage() )
	{
		// background thread is running.  Wait until parser has passed nLine
		int nParserAt;
		BOOL bMustSleep;
		do
		{
			Lock();
			nParserAt = m_nStartParseAt;
			Unlock();
			bMustSleep = ( nLine >= nParserAt && ( nLine < GetLineCount() - 1 ) );
			// relinquish control to another thread since we are waiting for the line to become available
			if ( bMustSleep )
			{
				Sleep( 0 );
			}
		} while ( bMustSleep );
	}
}

void CBuffer::AllowParse( BOOL bAllow )
{
	m_bAllowParse = bAllow;
	if ( m_bAllowParse && m_nLineCount )
	{
		NotifySyntaxParser( 0 );
	}
}

#if 0 // currently not used
// ScopeWord1MatchesScopeWord2() determines if a specific scope ending word (e.g. 'ELSE') can be
// matched to a specific scope starting word (e.g. 'IF', or 'ELSIF').  This function simply does
// an O(n) lookup into the m_pScopeKeywords.  This function does not need to be that fast, since
// it is not a part of the parsing logic that is called during painting.  This function is called
// during auto-indenting.
BOOL CBuffer::ScopeWord1MatchesScopeWord2( int nTokenIDScope1, int nTokenIDScope2 ) const
{
	// do the obvious up-front check and try to bail now.
	if ( nTokenIDScope1 == nTokenIDScope2 )
	{
		return TRUE;
	}
	ASSERT( m_pScopeKeywords2 );
	ASSERT( m_nScopeKeywords );

	LPCTSTR pszScope1a, pszScope2a;
	LPCTSTR pszScope1b, pszScope2b;
	pszScope1a = pszScope1b = pszScope2a = pszScope2b = NULL;
	// Find both IDs in the scope1 list
	for ( register int i = 0; ( !pszScope1a || !pszScope1b ) && i < m_nScopeKeywords; i++ )
	{
		LPCTSTR pszScopeWord = m_pScopeKeywords1[ i ];
		int nTokenID = TOKEN_ID( pszScopeWord );
		if ( nTokenID == nTokenIDScope1 )
		{
			pszScope1a = pszScopeWord;
		}
		else if ( nTokenID == nTokenIDScope2 )
		{
			pszScope1b = pszScopeWord;
		}
	}

	ASSERT( pszScope1a );
	ASSERT( pszScope1b );

	// Find both IDs in the scope2 list
	for ( i = 0; ( !pszScope2a || !pszScope2b ) && i < m_nScopeKeywords; i++ )
	{
		LPCTSTR pszScopeWord = m_pScopeKeywords2[ i ];
		int nTokenID = TOKEN_ID( pszScopeWord );
		if ( nTokenID == nTokenIDScope1 )
		{
			pszScope2a = pszScopeWord;
		}
		else if ( nTokenID == nTokenIDScope2 )
		{
			pszScope2b = pszScopeWord;
		}
	}

	ASSERT( pszScope2a );
	ASSERT( pszScope2b );

	// now, compare the two tokens for equality
	int nLen1a = TOKEN_LEN( pszScope1a );
	int nLen1b = TOKEN_LEN( pszScope1b );
	int nLen2a = TOKEN_LEN( pszScope2a );
	int nLen2b = TOKEN_LEN( pszScope2b );
	BOOL bIsMatch = FALSE;
	if ( nLen1a == nLen1b )
	{
		if ( LanguageIsCaseSensitive() )
		{
			bIsMatch = ( compare_char_i( pszScope1a[ 2 ], pszScope1b[ 2 ] ) == 0 &&
			             _tcsnicmp( pszScope1a + 2, pszScope1b + 2, nLen1a ) == 0 );
		}
		else
		{
			bIsMatch = ( pszScope1a[ 2 ] == pszScope1b[ 2 ] &&
			             _tcsncmp( pszScope1a + 2, pszScope1b + 2, nLen1a ) == 0 );
		}
	}
	
	if ( !bIsMatch )
	{
		// keep checking
		if ( nLen2a == nLen2b )
		{
			if ( LanguageIsCaseSensitive() )
			{
				bIsMatch = ( compare_char_i( pszScope2a[ 2 ], pszScope2b[ 2 ] ) == 0 &&
							 _tcsnicmp( pszScope2a + 2, pszScope2b + 2, nLen2a ) == 0 );
			}
			else
			{
				bIsMatch = ( pszScope2a[ 2 ] == pszScope2b[ 2 ] &&
							 _tcsncmp( pszScope2a + 2, pszScope2b + 2, nLen2a ) == 0 );
			}
		}
	}

	return bIsMatch;
}
#endif

CLineParser::CLineParser( CBuffer *pBuffer, int nLine, int nStopParseAt )
{
	m_pBuffer = pBuffer;
	SetLine( nLine, nStopParseAt );
}

CLineParser::CLineParser( CBuffer *pBuffer )
{
	m_pBuffer = pBuffer;
}

void CLineParser::SetLine( int nLine, int nStopParseAt )
{
	m_nLine = nLine;
	if ( nLine )
	{
		m_bInComment = m_pBuffer->IsInComment( m_nLine, m_nCommentStyle );
		m_bInString = m_pBuffer->IsInString( m_nLine, m_nStringStyle );
		m_bInTag = m_pBuffer->IsInTag( m_nLine, m_nTagStyle );
	}
	else
	{
		m_bInComment = FALSE;
		m_bInString = FALSE;
		m_bInTag = FALSE;
		m_nCommentStyle = m_nStringStyle = m_nTagStyle = -1;
	}
	m_bMoreComing = TRUE;
	m_psz = m_pBuffer->GetLineText( m_nLine );
	m_nPosBefore = m_nPos = 0;
	if ( nStopParseAt == -1 )
		nStopParseAt = m_pBuffer->GetLineLength( m_nLine );
	m_pszStopParseAt = m_psz + nStopParseAt;
	m_nTokenOffset = 0;
}

// ScopeTokensMatch() determines if two scope tokens match (pair up).
BOOL ScopeTokensMatch( int nStartToken, int nEndToken, LPCTSTR *ppszTokens )
{
	// quick test up front
	if ( nStartToken == nEndToken )
		return TRUE;

	// otherwise, if the text of the two tokens is identical, then we have
	// similar end scope tokens that can pass for each other.
	LPCTSTR pszToken1 = ppszTokens[ nStartToken ];
	LPCTSTR pszToken2 = ppszTokens[ nEndToken ];

	int nLenToken1 = TOKEN_LEN( pszToken1 );
	if ( pszToken1 && pszToken2 && nLenToken1 == TOKEN_LEN( pszToken2 ) )
		return ( _tcsncmp( TOKEN_TEXT( pszToken1 ), TOKEN_TEXT( pszToken2 ), nLenToken1 ) == 0 );

	return FALSE;
}

void CLineParser::AcceptToken()
{
	m_nPosBefore = m_nPos;
	m_pBuffer->GetNextToken( m_eToken, m_nTokenLen, m_psz, m_nPos, m_pszStopParseAt, m_bInComment, m_bInString, m_bInTag, m_bHasTab, m_nTokenID, m_nTokenID2, m_bMoreComing, m_nTokenOffset );

	m_bIsCommentEndToken = ( !m_bInString && m_bInComment && ( m_eToken == CBuffer::eMultiLineCommentEnd || m_eToken == CBuffer::eMultiLineCommentStartAndEnd ) && ScopeTokensMatch( m_nCommentStyle, m_nTokenID, m_pBuffer->m_pMultiLineComments2Unsorted ) );
	m_bIsStringEndToken = ( !m_bInComment && m_bInString && ( m_eToken == CBuffer::eStringDelim ) && ScopeTokensMatch( m_nStringStyle, m_nTokenID, m_pBuffer->m_pStringDelimsUnsorted ) );
	BOOL bIsTagEndToken = ( m_bInTag && ( m_eToken == CBuffer::eScopeKeywordEnd ) && ScopeTokensMatch( m_nTagStyle, m_nTokenID, m_pBuffer->m_pScopeKeywords2Unsorted ) );

	m_bWasInComment = m_bInComment;
	m_bWasInString = m_bInString;

	if ( m_bIsCommentEndToken )
	{
		if ( m_bInComment )
		{
			m_bInComment = FALSE;
		}
	}
	else if ( m_eToken == CBuffer::eMultiLineCommentStart )
	{
		if ( !m_bInComment && !m_bInString )
		{
			m_bInComment = TRUE;
			m_nCommentStyle = m_nTokenID;
		}
	}
	else if ( m_eToken == CBuffer::eMultiLineCommentStartAndEnd && !m_bInString )
	{
		m_bInComment = !m_bInComment;
		m_nCommentStyle = m_nTokenID;
	}
	else if ( m_bIsStringEndToken && !m_bInComment )
	{
		m_bInString = FALSE;
	}
	else if ( m_eToken == CBuffer::eStringDelim && !m_bInComment )
	{
		if ( !m_bInString )
		{
			m_bInString = TRUE;
			m_nStringStyle = m_nTokenID;
		}
	}
	else if ( bIsTagEndToken )
	{
		if ( !m_bInComment && !m_bInString )
		{
			m_bInTag = FALSE;
		}
	}
	else if ( m_eToken == CBuffer::eScopeKeywordStart )
	{
		if ( !m_bInTag && !m_bInString && !m_bInComment )
		{
			m_bInTag = m_pBuffer->LanguageIsSGML();
			m_nTagStyle = m_nTokenID;
		}
	}
}
