#ifndef __BUFFER_H__
#define __BUFFER_H__

#define TOKEN_LEN( x ) ( ( x )[ 0 ] & 0x3f )
#define TOKEN_ID( x ) ( ( x )[ 1 ] )
#define TOKEN_IS_ALPHA_PRE( x ) ( ( ( x )[ 0 ] & 0x80 ) != 0 )
#define TOKEN_IS_ALPHA_POST( x ) ( ( ( x )[ 0 ] & 0x40 ) != 0 )
#define TOKEN_TEXT( x ) ( x + 2 )

class CUndoAction;

class CLine
{
public:

	CLine( LPCTSTR pszLine, int cbLine, WORD wFlags = 0 );
	~CLine();

public:
	int AddChars( int nIndexBefore, LPCTSTR pszText, int cbText );
	int RemoveChars( int nIndexStart, int nChars );
	__forceinline LPCTSTR GetText() const { return m_pszText ? m_pszText : _T(""); }
	__forceinline LPTSTR GetTextForWrite() const { return ( LPTSTR ) GetText(); }
	int GetLength() const	{ return m_nLength; }
	WORD GetFlags() const	{ return m_wFlags; }
	__forceinline BOOL HasBookmark() const	{ return HAS_FLAG( m_wFlags, BOOKMARK ); }
	void SetBookmark( BOOL bOn );
	BOOL IsInComment( int &nCommentStyle ) const;
	void SetInComment(  BOOL bIn, int nCommentStyle );
	BOOL IsInString( int &nStringStyle ) const;
	void SetInString( BOOL bIn, int nStringStyle );
	BOOL IsInTag( int &nTagStyle ) const;
	void SetInTag( BOOL bIn, int nTagStyle );
	BOOL IsHighlighted() const;
	void SetHighlighted( BOOL bOn );
	__forceinline BOOL HasDivider() const	{ return HAS_FLAG( m_wFlags, DIVIDER ); }
	void SetDivider( BOOL bOn );
	__forceinline LPARAM GetItemData() const	{ return m_lParam; }
	__forceinline void SetItemData( LPARAM lParam )	{ m_lParam = lParam; }
	__forceinline DWORD GetStyle() const	{ return m_dwStyle; }
	__forceinline void SetStyle( DWORD dwStyle )	{ m_dwStyle = dwStyle; }
	__forceinline BYTE GetMarginImages() { return m_byImages; }
	__forceinline void SetMarginImages( BYTE byImages ) { m_byImages = byImages; }

	// m_wFlags bitfield values
	enum { BOOKMARK = 0x8,
	       IN_COMMENT = 0x4,
		   COMMENT_STYLE = 0x3,
		   COMMENT_SHIFT = 0,		// number of bits to shift right to get to the comment style
		   IN_STRING = 0x40,
		   STRING_STYLE = 0x30,
		   STRING_SHIFT = 4,		// number of bits to shift right to get to the string style
		   IN_TAG = 0x400,
		   TAG_STYLE = 0x300,
		   TAG_SHIFT = 8,			// number of bits to shift right to get to the tag style
		   IS_HIGHLIGHTED = 0x800,
		   DIVIDER = 0x1000,
		   BIT_MASK = 0xffff };

private:

	enum { GROWBY = 10 };

	void EnsureTextSize( int nCharsRequired );

	LPTSTR m_pszText;
	int m_nLength;
	int m_nLengthAllocated;
	LPARAM m_lParam;
	DWORD m_dwStyle;
	WORD m_wFlags;
	BYTE m_byImages;
};

typedef void BUFFERCALLBACKFN( LPARAM lParam, DWORD dwNotification, int nLine, int nChar, int nCount );
class CUndoAction;
class CEdit;

class CBuffer
{
friend class CLineParser;
public:

	// construction/destruction/initialization
	CBuffer();
	~CBuffer();
	void SetCtrl( CEdit *pCtrl ) { m_pCtrl = pCtrl; }

	// access methods
	LPCTSTR PASCAL GetLineText( int nLine ) const;
	int PASCAL GetLineLength( int nLine ) const;
	__forceinline int PASCAL GetLineCount() const { return m_nLineCount; }
	__forceinline int PASCAL GetMaxLineWidthGuess() const { return m_nMaxLineWidthGuess; }
	BOOL PASCAL LineIsEmpty( int nLine, BOOL bIgnoreWhiteSpace ) const;
	__forceinline BOOL PASCAL HasBookmark( int nLine ) const	{ return GetLine( nLine )->HasBookmark(); }
	void PASCAL ToggleBookmark( int nLine ) const;
	void PASCAL RemoveBookmark( int nLine ) const;
	__forceinline void PASCAL SetBookmark(  int nLine ) { GetLine( nLine )->SetBookmark( TRUE ); }
	__forceinline BOOL PASCAL IsInComment( int nLine, int &nCommentStyle ) const { return GetLine( nLine )->IsInComment( nCommentStyle ); }
	__forceinline void PASCAL SetInComment(  int nLine, BOOL bIn, int nCommentStyle ) { GetLine( nLine )->SetInComment( bIn, nCommentStyle ); }
	__forceinline BOOL PASCAL IsInString( int nLine, int &nStringStyle ) const { return GetLine( nLine )->IsInString( nStringStyle ); }
	__forceinline void PASCAL SetInString(  int nLine, BOOL bIn, int nStringStyle ) { GetLine( nLine )->SetInString( bIn, nStringStyle ); }
	__forceinline BOOL PASCAL IsInTag( int nLine, int &nTagStyle ) const { return GetLine( nLine )->IsInTag( nTagStyle ); }
	__forceinline void PASCAL SetInTag(  int nLine, BOOL bIn, int nTagStyle ) { GetLine( nLine )->SetInTag( bIn, nTagStyle ); }
	__forceinline BOOL PASCAL IsHighlighted( int nLine ) const { return GetLine( nLine )->IsHighlighted(); }
	__forceinline void PASCAL SetHighlighted(  int nLine, BOOL bOn ) { GetLine( nLine )->SetHighlighted( bOn ); }
	void PASCAL SetReadOnly( BOOL bReadOnly );
	__forceinline BOOL PASCAL IsReadOnly() const	{ return m_bReadOnly; }
	BOOL PASCAL IsEmpty() const { 	return ( ( m_nLineCount == 0 ) || ( ( m_nLineCount == 1 ) && ( GetLineLength( 0 ) == 0 ) ) ); }
	BOOL PASCAL WantCarriageReturn() const	{ return m_bWantCarriageReturn; }
	void PASCAL EnableCarriageReturn( BOOL bWantCarriageReturn )	{ m_bWantCarriageReturn = bWantCarriageReturn; }
	__forceinline LPARAM PASCAL GetItemData( int nLine ) const				{ return GetLine( nLine )->GetItemData(); }
	__forceinline void PASCAL SetItemData( int nLine, LPARAM lParam ) const	{ GetLine( nLine )->SetItemData( lParam ); }
	DWORD PASCAL GetLineStyle( int nLine ) const				{ return GetLine( nLine )->GetStyle(); }
	void PASCAL SetLineStyle( int nLine, DWORD dwStyle );
	__forceinline BOOL PASCAL HasDivider( int nLine ) const	{ return GetLine( nLine )->HasDivider(); }
	__forceinline void PASCAL SetDivider( int nLine, BOOL bOn ) { GetLine( nLine )->SetDivider( bOn ); }
	__forceinline BYTE PASCAL GetMarginImages( int nLine ) const	{ return GetLine( nLine )->GetMarginImages(); }
	__forceinline void PASCAL SetMarginImages( int nLine, BYTE byImages ) { GetLine( nLine )->SetMarginImages( byImages ); }

	// notification support
	void PASCAL SetNotificationCallback( BUFFERCALLBACKFN *pfn, LPARAM lParam );
	__forceinline int PASCAL GetNotifyDelCount() const	{ return m_nNotifyDelCount; }

	// undo/redo support
	void PASCAL BeginEdit( int nCursorLine = -1, int nCursorCol = -1 );
	void PASCAL EndEdit( int nCursorLine = -1, int nCursorCol = -1 );
	__forceinline BOOL PASCAL IsInEditTransaction() const	{ return ( m_nEditLevel > 0 ); }
	void SetMaxUndo( int nMax );
	int GetMaxUndo() const	{ return m_nMaxUndo; }

	void PauseParser();

	// Add/Remove text
	void PASCAL InsertLine( int nBefore, LPCTSTR pszText, int cbText = -1 );
	void PASCAL InsertCharsIntoLine( int nLine, int nCol, LPCTSTR pszText, int cbText = -1 );
	void PASCAL EnsureLineLength( int nRow, int cbLen );
	
	void PASCAL RemoveLines( int nStart, int nCount );
	void PASCAL RemoveCharsFromLine( int nLine, int nStartCol, int nChars );

	int PASCAL ConvertBufferColToViewCol( int nRow, int nBuffCol ) const;
	int PASCAL ConvertViewColToBufferCol( int nRow, int nViewCol, BOOL bRoundToLeadByte = TRUE ) const;
	int PASCAL GetCharSize( int nLine, int nCol ) const;

	void PASCAL ClearRedoChain( BOOL bEntire );

	int PASCAL GetTabSize() const	{ return m_cbTab; }
	void PASCAL SetTabSize( int nLen );

	void PASCAL AdvanceToWordStart( int &nRow, int &nCol, BOOL bForward, BOOL bSkipInitialSpace ) const;
	void PASCAL AdvanceToWordEnd( int &nRow, int &nCol, BOOL bForward, BOOL bSkipInitialSpace ) const;

	void PASCAL AdvanceToSentenceStart( int &nRow, int &nCol, BOOL bForward ) const;
	void PASCAL AdvanceToSentenceEnd( int &nRow, int &nCol ) const;

	BOOL PASCAL GetText( int nStartRow, int nStartCol, int nEndRow, int nEndCol, HGLOBAL &hMem, BOOL bCopyLineIfEmpty, BOOL bForceCR = FALSE ) const;
	BOOL PASCAL GetText( HANDLE hFile ) const;
	void PASCAL GetText( LPTSTR pszBuff ) const;
	void NormalizeLineTextCase( int nLine );
	BOOL NormalizeCase() const { return m_bNormalizeCase; }
	__forceinline void EnableNormalizeCase( BOOL bEnable ) { m_bNormalizeCase = bEnable; }

	// misc
	BOOL PASCAL SetLanguage( CM_LANGUAGE *pLang );
	static int PASCAL CharCount( LPCTSTR psz, TCHAR ch );

	typedef enum { eKeyword =						0x0001,
	               eOperator =						0x0002,
	               eStringDelim =					0x0003,
	               eSingleLineComment =				0x0004,
	               eMultiLineCommentStart =			0x0104,
	               eMultiLineCommentEnd =			0x0204,
	               eMultiLineCommentStartAndEnd =	0x0304,
	               eScopeKeywordStart =				0x0105,
	               eScopeKeywordEnd =				0x0205,
	               eScopeKeywordStartAndEnd =		0x0305,
	               eText =							0x0006,
	               eTagText =						0x0007,
	               eNumber =						0x0008,
				   eEscapeSeq =						0x0009,
				   eTagElementName =				0x0010,
				   eTagAttributeName =				0x0020,
				   eTagEntity =						0x0030,
	               eColorMask =						0x00ff } LangToken;

	// the color syntax highlighting parser methods
	void GetTokenText( LangToken eToken, int nTokenID, LPTSTR pszDest ) const;
	void PASCAL GetNextToken( LangToken &eToken, int &nTokenLen, LPCTSTR psz, int &nPos, LPCTSTR pszEnd, BOOL bIsInComment, BOOL bIsInString, BOOL bIsInTag, BOOL &bHasTab, int &nTokenID, int &nTokenID2, BOOL &bMoreComing, int &nTokenOffset ) const;
	void PASCAL StartSyntaxParse();
	void PASCAL EndSyntaxParse();
	void PASCAL AllowParse( BOOL bAllow );

	void PASCAL Lock();
	void PASCAL Unlock();
	void PASCAL WaitForLine( int nLine );

	__forceinline BOOL PASCAL HasLanguage() const { return m_pLangBuff != NULL; }
	__forceinline BOOL PASCAL LanguageIsSGML() const { return HAS_FLAG( m_dwLangStyle, CMLS_SGML ); }
	__forceinline BOOL PASCAL LanguageIsCaseSensitive() const { return ( m_bLangIsCaseSensitive && !m_bNormalizeCase ); }
	
	//BOOL PASCAL ScopeWord1MatchesScopeWord2( int nTokenIDScope1, int nTokenIDScope2 ) const;

	BOOL FindText( LPCTSTR pszText, int cbText, BOOL bForward, BOOL bCaseSensitive, BOOL bWholeWordOnly, BOOL bRegExp, register int &nLine, register int &nChar, BOOL &bWrapped, int &cbMatched ) const;
	BOOL FindChar( TCHAR ch, TCHAR chPair, BOOL bForward, int &nLine, int &nChar ) const;

private:

	void PASCAL DeleteLanguageInfo();
	static int _cdecl TokenSortProc( const void *psz1, const void *psz2 );
	BOOL PASCAL LookupToken( LPCTSTR *ppszTokens, int nTokens, LPCTSTR psz, int nLen, int &nTokenLen, BOOL bCheckSurrounding, TCHAR chLeading, int &nTokenID, int &nTokenOffset, BOOL bForceCaseInsensitive ) const;
	LangToken PASCAL FindToken( LPCTSTR pszBuff, int nPos, LPCTSTR pszEnd, int &nTokenLen, BOOL &bReachedEnd, int &nTokenID, int &nTokenID2, BOOL bWantRawLen, int &nTokenOffset ) const;

	friend DWORD PASCAL SyntaxParserThread( CBuffer *pBuffer );
	void PASCAL SyntaxParserThread();
	void PASCAL NotifySyntaxParser( int nChangedLine );

	enum { SHUTDOWN = 0x1 };

public:
	
	BOOL IsModified() const	{ return ( m_bForceModified || ( m_nChangeLevel != 0 ) ); }
	void SetModified( BOOL bModified );
	int GetChangeLevel() const	{ return m_nChangeLevel; }

private:
	void OnChanged( int nCount = 1 );

public:
	void PASCAL EnsureLineArraySize( int nLinesRequired );
	void FlushExtraLines();

private:

	CLine * PASCAL GetLine( int nLine ) const;

	void PASCAL AddUndoAction( CUndoAction *pAction );
	void PASCAL EnsureUndoArraySize( int nUndosRequired );

public:
	enum { MAX_TABSIZE = 30 };
	typedef enum { eBeginEdit, eEndEdit, eLineChanged, eLineChangedSilent, eLineAdded, eLineRemoved, eBeginUndo, eBeginRedo, eEndUndo, eEndRedo, eSetCursorPos } Notification;

	BOOL PASCAL IsInUndoRedo() const	{ return ( IsInUndo() || IsInRedo() ); }
	BOOL PASCAL IsInUndo() const		{ return ( m_eState == eUndo ); }
	BOOL PASCAL IsInRedo() const		{ return ( m_eState == eRedo ); }
	void PASCAL Undo();
	void PASCAL Redo();
	BOOL PASCAL CanUndo() const			{ return ( m_nUndoPos != 0 ); }
	BOOL PASCAL CanRedo() const			{ return ( m_nUndoPos < m_nUndoCount ); }

	BOOL EnableUndo( BOOL bEnable );
	__forceinline void EnableCollapseUndo( BOOL bEnable )	{ m_bCollapseUndo = bEnable; }
	BOOL ChangedSilently( BOOL bReset = TRUE );

private:

	void PASCAL EnforceMaxUndo();
	__forceinline BOOL PASCAL ShouldAddUndoActions()	{ return ( m_bUndoEnabled && !IsInUndoRedo() ); }

	enum { eUndo, eRedo, eEdit } m_eState;

private:

	friend class CUndoStartEnd;
	void Notify( Notification eNotification, int nLine, int nCol, int nCount ) const;

	enum { UNDO_GROWBY = 20 };


	enum { CHAR_KEYWORD = 0x1, CHAR_OPERATOR = 0x2, CHAR_SLCOMMENT = 0x4,
	       CHAR_MLCOMMENT1 = 0x8, CHAR_MLCOMMENT2 = 0x10, CHAR_STRING = 0x20,
		   CHAR_SCOPE1 = 0x40, CHAR_SCOPE2 = 0x80, CHAR_ESCAPESEQ = 0x100,
		   CHAR_TAG_ENTITY = 0x200, CHAR_TAG_ELEMENT_NAME = 0x400,
		   CHAR_TAG_ATTRIBUTE_NAME = 0x800,  CHAR_TAG_ATTRIBUTE_VALUE = 0x1000 };

	HANDLE m_hSyntaxThread;
	HANDLE m_hSyntaxEvent;
	CRITICAL_SECTION m_csSyntax;
	LPTSTR m_pLangBuff;
	LPCTSTR *m_pKeywords;
	LPCTSTR *m_pKeywordsUnsorted;
	LPCTSTR *m_pOperators;
	LPCTSTR *m_pOperatorsUnsorted;
	LPCTSTR *m_pSingleLineComments;
	LPCTSTR *m_pSingleLineCommentsUnsorted;
	LPCTSTR *m_pMultiLineComments1;
	LPCTSTR *m_pMultiLineComments1Unsorted;
	LPCTSTR *m_pMultiLineComments2;
	LPCTSTR *m_pMultiLineComments2Unsorted;
	LPCTSTR *m_pScopeKeywords1;
	LPCTSTR *m_pScopeKeywords1Unsorted;
	LPCTSTR *m_pScopeKeywords2;
	LPCTSTR *m_pScopeKeywords2Unsorted;
	LPCTSTR *m_pStringDelims;
	LPCTSTR *m_pStringDelimsUnsorted;
	LPCTSTR *m_pTagElementNames;
	LPCTSTR *m_pTagElementNamesUnsorted;
	LPCTSTR *m_pTagAttributeNames;
	LPCTSTR *m_pTagAttributeNamesUnsorted;
	LPCTSTR *m_pTagEntities;
	LPCTSTR *m_pTagEntitiesUnsorted;

	DWORD m_dwLangStyle;
	BYTE m_byteSyntaxCmd;
	WORD m_CharIsKeyword[ 256 ];
	BOOL m_bAllowParse : 2;
	BOOL m_bReadOnly : 2;
	BOOL m_bWantCarriageReturn : 2;
	BOOL m_bLangIsCaseSensitive : 2;	// Warning: use LanguageIsCaseSensitive() to get real state!
	BOOL m_bForceModified : 2;
	BOOL m_bFirstUndoAction : 2;
	BOOL m_bUndoEnabled : 2;
	BOOL m_bCollapseUndo : 2;
	BOOL m_bSilentChange : 2;
	BOOL m_bNormalizeCase : 2;
	TCHAR m_chEscape;
	TCHAR m_chTerminator;
	int m_nChangeLevel;
	int m_nStartParseAt;
	int m_nCanStopParseOn;
	int m_nBeginEditCol;
	int m_nStringDelims;
	int m_nEditLevel : 5;
	int m_cbTab : 8;
	int m_nKeywords;
	int m_nOperators;
	int m_nLineComments;
	int m_nMultiLineComments;
	int m_nScopeKeywords;
	int m_nTagElementNames;
	int m_nTagAttributeNames;
	int m_nTagEntities;
	int m_nBeginEditLine;
	int m_nLineCount;		// number of used entries in m_pLines
	int m_nLinesAllocated;	// number of valid entries in m_pLines
	int m_nUndoCount;		// number of used entries in m_pUndo
	int m_nUndoTrans;		// number of undo transactions
	int m_nUndosAllocated;	// number of valid entries in m_pUndo
	int m_nUndoPos;
	int m_nMaxUndo;
	int m_nMaxLineWidthGuess; // used for optimizing HScrollBar
	int m_nLineChunkSize;
	int m_nNotifyDelCount;	// number of lines that have the CML_NOTIFY_DEL style
	CLine *m_pLines;		// the master buffer
	CUndoAction **m_pUndo;	// the master undo buffer
	BUFFERCALLBACKFN *m_pfnCallback;
	LPARAM m_lParam;
	CEdit *m_pCtrl;
};

class CLineParser
{
	public:
	
	CLineParser( CBuffer *pBuffer, int nLine, int nStopParseAt = -1 );
	CLineParser( CBuffer *pBuffer );
	void SetLine( int nLine, int nStopParseAt = -1 );

	void AcceptToken();
	__forceinline BOOL MoreComing() const { return m_bMoreComing; }
	__forceinline void AdvanceTo( LPCTSTR psz ) { m_psz = psz; }

	CBuffer *m_pBuffer;
	int m_nLine;
	int m_nCommentStyle;
	int m_nStringStyle;
	int m_nTagStyle;
	int m_nTokenID;
	int m_nTokenID2;
	int m_nTokenOffset;
	int m_nPos;
	int m_nPosBefore;
	int m_nTokenLen;
	BOOL m_bWasInComment;
	BOOL m_bWasInString;
	BOOL m_bInComment;
	BOOL m_bInString;
	BOOL m_bInTag;
	BOOL m_bHasTab;
	BOOL m_bMoreComing;
	BOOL m_bIsCommentEndToken;
	BOOL m_bIsStringEndToken;
	CBuffer::LangToken m_eToken;
	LPCTSTR m_psz;
	LPCTSTR m_pszStopParseAt;
};

#endif