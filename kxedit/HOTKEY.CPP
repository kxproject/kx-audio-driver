#include "precomp.h"
#include "cedit.h"											

_defhotkeyrec CEdit::g_DefHotKeys[] =
	{
		{ CMD_WORDRIGHTEXTEND,				HOTKEYF_CONTROL | HOTKEYF_SHIFT,	VK_RIGHT	,0,0 },
		{ CMD_WORDRIGHT,					HOTKEYF_CONTROL,					VK_RIGHT	,0,0 },
		{ CMD_WORDLEFTEXTEND,				HOTKEYF_CONTROL | HOTKEYF_SHIFT,	VK_LEFT		,0,0 },
		{ CMD_WORDLEFT,						HOTKEYF_CONTROL,					VK_LEFT		,0,0 },
		{ CMD_WORDDELETETOSTART,			HOTKEYF_CONTROL,					VK_BACK		,0,0 },
		{ CMD_WORDDELETETOEND,				HOTKEYF_CONTROL,					VK_DELETE	,0,0 },
		{ CMD_WINDOWSCROLLUP,				HOTKEYF_CONTROL,					VK_DOWN		,0,0 },
		{ CMD_WINDOWSCROLLRIGHT,			HOTKEYF_CONTROL,					VK_NEXT		,0,0 },
		{ CMD_WINDOWSCROLLLEFT,				HOTKEYF_CONTROL,					VK_PRIOR	,0,0 },
		{ CMD_WINDOWSCROLLDOWN,				HOTKEYF_CONTROL,					VK_UP		,0,0 },
		{ CMD_UPPERCASESELECTION,			HOTKEYF_CONTROL | HOTKEYF_SHIFT,	_T('U')		,0,0 },
		{ CMD_UNTABIFYSELECTION,			HOTKEYF_CONTROL | HOTKEYF_SHIFT,	VK_SPACE	,0,0 },
		{ CMD_UNINDENTSELECTION,			HOTKEYF_SHIFT,						VK_TAB		,0,0 },
		{ CMD_UNDO,							HOTKEYF_CONTROL,					_T('Z')		,0,0 },
		{ CMD_UNDO,							HOTKEYF_ALT,						VK_BACK		,0,0 },
		{ CMD_TABIFYSELECTION,				HOTKEYF_CONTROL | HOTKEYF_SHIFT,	_T('T')		,0,0 },
		{ CMD_SENTENCERIGHT,				HOTKEYF_CONTROL | HOTKEYF_ALT,		VK_RIGHT	,0,0 },
		{ CMD_SENTENCELEFT,					HOTKEYF_CONTROL | HOTKEYF_ALT,		VK_LEFT		,0,0 },
		{ CMD_SENTENCECUT,					HOTKEYF_CONTROL | HOTKEYF_ALT,		_T('K')		,0,0 },
		{ CMD_SELECTSWAPANCHOR,				HOTKEYF_CONTROL | HOTKEYF_SHIFT,	_T('X')		,0,0 },
		{ CMD_SELECTLINE,					HOTKEYF_CONTROL | HOTKEYF_ALT,		VK_F8		,0,0 },
		{ CMD_REDO,							HOTKEYF_CONTROL,					_T('A')		,0,0 },
		{ CMD_PASTE,						HOTKEYF_CONTROL,					_T('V')		,0,0 },
		{ CMD_PASTE,						HOTKEYF_SHIFT,						VK_INSERT	,0,0 },
		{ CMD_PAGEUPEXTEND,					HOTKEYF_SHIFT,						VK_PRIOR	,0,0 },
		{ CMD_PAGEUP,						0,									VK_PRIOR	,0,0 },
		{ CMD_PAGEDOWNEXTEND,				HOTKEYF_SHIFT,						VK_NEXT		,0,0 },
		{ CMD_PAGEDOWN,						0,									VK_NEXT		,0,0 },
		{ CMD_LOWERCASESELECTION,			HOTKEYF_CONTROL,					_T('U')		,0,0 },
		{ CMD_LINEUPEXTEND,					HOTKEYF_SHIFT,						VK_UP		,0,0 },
		{ CMD_LINEUP,						0,									VK_UP		,0,0 },
		{ CMD_LINEOPENABOVE,				HOTKEYF_CONTROL | HOTKEYF_SHIFT,	_T('N')		,0,0 },
		{ CMD_LINEENDEXTEND,				HOTKEYF_SHIFT,						VK_END		,0,0 },
		{ CMD_LINEEND,						0,									VK_END		,0,0 },
		{ CMD_LINEDOWNEXTEND,				HOTKEYF_SHIFT,						VK_DOWN		,0,0 },
		{ CMD_LINEDOWN,						0,									VK_DOWN		,0,0 },
		{ CMD_LINECUT,						HOTKEYF_CONTROL,					_T('Y')		,0,0 },
		{ CMD_INDENTSELECTION,				0,									VK_TAB		,0,0 },
		{ CMD_HOMEEXTEND,					HOTKEYF_SHIFT,						VK_HOME		,0,0 },
		{ CMD_HOME,							0,									VK_HOME		,0,0 },
		{ CMD_GOTOMATCHBRACE,				HOTKEYF_CONTROL,					0xdd /*']'*/,0,0 },
		{ CMD_GOTOLINE,						HOTKEYF_CONTROL,					_T('G')		,0,0 },
		{ CMD_FINDPREVWORD,					HOTKEYF_CONTROL | HOTKEYF_SHIFT,	VK_F3		,0,0 },
		{ CMD_FINDPREV,						HOTKEYF_SHIFT,						VK_F3		,0,0 },
		{ CMD_FINDNEXTWORD,					HOTKEYF_CONTROL,					VK_F3		,0,0 },
		{ CMD_FINDNEXT,						0,									VK_F3		,0,0 },
		{ CMD_FIND,							HOTKEYF_ALT,						VK_F3		,0,0 },
		{ CMD_FIND,							HOTKEYF_CONTROL,					_T('F')		,0,0 },
		{ CMD_FINDREPLACE,					HOTKEYF_CONTROL | HOTKEYF_ALT,		VK_F3		,0,0 },
		{ CMD_TOGGLEWHITESPACEDISPLAY,		HOTKEYF_CONTROL | HOTKEYF_ALT,		_T('T')		,0,0 },
		{ CMD_TOGGLEOVERTYPE,				0,									VK_INSERT	,0,0 },
		{ CMD_DOCUMENTSTARTEXTEND,			HOTKEYF_CONTROL | HOTKEYF_SHIFT,	VK_HOME		,0,0 },
		{ CMD_DOCUMENTSTART,				HOTKEYF_CONTROL,					VK_HOME		,0,0 },
		{ CMD_DOCUMENTENDEXTEND,			HOTKEYF_CONTROL | HOTKEYF_SHIFT,	VK_END		,0,0 },
		{ CMD_DOCUMENTEND,					HOTKEYF_CONTROL,					VK_END		,0,0 },
		{ CMD_DELETEBACK,					0,									VK_BACK		,0,0 },
		{ CMD_DELETEBACK,					HOTKEYF_SHIFT,						VK_BACK		,0,0 },
		{ CMD_DELETE,						0,									VK_DELETE	,0,0 },
		{ CMD_CUTSELECTION,					HOTKEYF_CONTROL | HOTKEYF_ALT,		_T('W')		,0,0 },
		{ CMD_CUT,							HOTKEYF_SHIFT,						VK_DELETE	,0,0 },
		{ CMD_CUT,							HOTKEYF_CONTROL,					_T('X')		,0,0 },
		{ CMD_COPY,							HOTKEYF_CONTROL,					_T('C')		,0,0 },
		{ CMD_COPY,							HOTKEYF_CONTROL,					VK_INSERT	,0,0 },
		{ CMD_CHARRIGHTEXTEND,				HOTKEYF_SHIFT,						VK_RIGHT	,0,0 },
		{ CMD_CHARRIGHT,					0,									VK_RIGHT	,0,0 },
		{ CMD_CHARLEFTEXTEND,				HOTKEYF_SHIFT,						VK_LEFT		,0,0 },
		{ CMD_CHARLEFT,						0,									VK_LEFT		,0,0 },
		{ CMD_BOOKMARKTOGGLE,				HOTKEYF_CONTROL,					VK_F2		,0,0 },
		{ CMD_BOOKMARKPREV,					HOTKEYF_SHIFT,						VK_F2		,0,0 },
		{ CMD_BOOKMARKNEXT,					0,									VK_F2		,0,0 },
		{ CMD_RECORDMACRO,					HOTKEYF_CONTROL | HOTKEYF_SHIFT,	_T('R')		,0,0 },
		{ CMD_SETREPEATCOUNT,				HOTKEYF_CONTROL,					_T('R')		,0,0 },
		{ CMD_PROPERTIES,					HOTKEYF_ALT,						VK_RETURN 	,0,0 }
	};

int CEdit::g_nDefHotKeyCount = ARRAY_SIZE( CEdit::g_DefHotKeys );

void CEdit::SetDefaultHotKeys()
{
	if ( g_pHotKeys )
	{
		delete [] g_pHotKeys;
		g_pHotKeys = NULL;
		g_nHotKeyCount = g_nHotKeyAllocCount = 0;
	}

	for ( int i = 0; i < ARRAY_SIZE( g_DefHotKeys ); i++ )
	{
		_defhotkeyrec *pHotKey = &g_DefHotKeys[ i ];
		#ifdef _DEBUG
		int nOldCount = g_nHotKeyCount;
		#endif
		CM_HOTKEY cmHotKey = { pHotKey->fsModifiers1, pHotKey->nVirtKey1,
		                      pHotKey->fsModifiers2, pHotKey->nVirtKey2 };
		VERIFY( RegisterHotKey( cmHotKey, pHotKey->wCmd ) );
		#ifdef _DEBUG
		ASSERT( nOldCount + 1 == g_nHotKeyCount );
		#endif
	}
	#ifdef _DEBUG
	for ( int x = 0; x < g_nHotKeyCount - 1; x++ )
	{
		ASSERT( cmp_hotkeys( &g_pHotKeys[ x ].cmHotKey, &g_pHotKeys[ x + 1 ].cmHotKey ) < 0 ); 
	}
	#endif
}

CHotKey *CEdit::g_pHotKeys = NULL;
int CEdit::g_nHotKeyCount = 0;
int CEdit::g_nHotKeyAllocCount = 0;

int CEdit::FindHotKeysForCommand( WORD wCmd, CM_HOTKEY *pHotKeys )
{
	int nCount = 0;
	for ( int i = 0; i < g_nHotKeyCount; i++ )
	{
		if ( g_pHotKeys[ i ].wCmd == wCmd )
		{
			if ( pHotKeys )
			{
				*pHotKeys = g_pHotKeys[ i ].cmHotKey;
				pHotKeys++;
			}			
			nCount++;
		}
	}

	return nCount;
}

BOOL CEdit::LookupHotKey( CM_HOTKEY &cmHotKey, WORD &wCmd, int &nPosFound )
{
	BOOL bFound = FALSE;
	nPosFound = 0;
	wCmd = 0;
	NormalizeHotKey( cmHotKey );

	if ( g_pHotKeys )
	{
		// perform a binary search

		int nStart = 0;
		int nEnd = g_nHotKeyCount - 1;

		while ( nStart <= nEnd )
		{
			nPosFound = ( nStart + nEnd ) / 2;
			ASSERT( nPosFound < g_nHotKeyCount );
			ASSERT( nPosFound < g_nHotKeyAllocCount );
			CHotKey *pKeyFound = &g_pHotKeys[ nPosFound ];
			CM_HOTKEY &cmKeyFound = pKeyFound->cmHotKey;

			int nCmp = cmp_hotkeys( &cmHotKey, &cmKeyFound );
			if ( nCmp == 0 )
			{
				wCmd = pKeyFound->wCmd;
				bFound = TRUE;
				break;
			}
			else if ( nCmp < 0 )
			{
				nEnd = nPosFound - 1;
			}
			else if ( nCmp > 0 )
			{
				nStart = nPosFound + 1;
			}
		}

		ASSERT( nPosFound < g_nHotKeyCount );
		if ( !bFound && ( cmp_hotkeys( &g_pHotKeys[ nPosFound ].cmHotKey, &cmHotKey ) < 0 ) )
		{
			nPosFound++;
		}

	}

	return bFound;
}

BOOL CEdit::RegisterHotKey( CM_HOTKEY &cmHotKey, WORD wCmd )
{
	WORD wDontCare;
	int nInsertAt;
	NormalizeHotKey( cmHotKey );

	if ( !LookupHotKey( cmHotKey, wDontCare, nInsertAt ) )
	{
		// not a reassignment -- will need to make room
		if ( !g_pHotKeys )
		{
			g_pHotKeys = ( CHotKey * ) malloc( HOTKEY_BLOCKSIZE * sizeof( CHotKey ) );
			if ( !g_pHotKeys )
			{
				return FALSE;
			}
			g_nHotKeyAllocCount = HOTKEY_BLOCKSIZE;
		}

		if ( g_nHotKeyCount + 1 > g_nHotKeyAllocCount )
		{
			g_nHotKeyAllocCount += HOTKEY_BLOCKSIZE;
			CHotKey *pNew = ( CHotKey * )realloc( g_pHotKeys, sizeof( CHotKey ) * g_nHotKeyAllocCount );
			if ( pNew )
			{
				g_pHotKeys = pNew;
			}
			else
			{
				return FALSE;
			}
		}

		if ( g_nHotKeyCount )
		{
			memmove( g_pHotKeys + nInsertAt + 1, g_pHotKeys + nInsertAt, sizeof( CHotKey ) * ( g_nHotKeyCount - nInsertAt ) );
		}
		g_nHotKeyCount++;
	}
	g_pHotKeys[ nInsertAt ].cmHotKey = cmHotKey;
	g_pHotKeys[ nInsertAt ].wCmd = wCmd;

	return TRUE;
}

BOOL CEdit::UnregisterHotKey( CM_HOTKEY &cmHotKey )
{
	WORD wDontCare;
	int nInsertAt;

	#ifdef _DEBUG
	for ( int x = 0; x < g_nHotKeyCount - 1; x++ )
	{
		ASSERT( cmp_hotkeys( &g_pHotKeys[ x ].cmHotKey, &g_pHotKeys[ x + 1 ].cmHotKey ) < 0 ); 
	}
	#endif

	if ( LookupHotKey( cmHotKey, wDontCare, nInsertAt ) )
	{
		// shrink the array
		memmove( g_pHotKeys + nInsertAt, 
		         g_pHotKeys + nInsertAt + 1, 
		         ( g_nHotKeyCount - nInsertAt - 1 ) * sizeof( CHotKey ) );
		g_nHotKeyCount--;
		ASSERT( g_nHotKeyCount >= 0 );

	#ifdef _DEBUG
	for ( x = 0; x < g_nHotKeyCount - 1; x++ )
	{
		ASSERT( cmp_hotkeys( &g_pHotKeys[ x ].cmHotKey, &g_pHotKeys[ x + 1 ].cmHotKey ) < 0 ); 
	}
	#endif

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

int CEdit::GetHotKeys( LPBYTE pBuff )
{
	// buff is version stamp, hotkey count, and hotkeys
	int cbBuff = sizeof( BYTE ) + sizeof( DWORD ) + CEdit::g_nHotKeyCount * ( sizeof( CM_HOTKEY ) + sizeof( WORD ) );

	if ( pBuff )
	{
		*pBuff++ = 1;	// version
		*( DWORD * ) pBuff = CEdit::g_nHotKeyCount;
		pBuff += sizeof( DWORD );
		for ( int i = 0; i < CEdit::g_nHotKeyCount; i++ )
		{
			CHotKey *pHotKey = &CEdit::g_pHotKeys[ i ];
			*( WORD * ) pBuff = pHotKey->wCmd;
			pBuff += sizeof( WORD );
			*( CM_HOTKEY * ) pBuff = pHotKey->cmHotKey;
			pBuff += sizeof( CM_HOTKEY );
		}
	}

	return cbBuff;
}

BOOL CEdit::SetHotKeys( const LPBYTE pHotKeys )
{
	LPBYTE pBuff = ( LPBYTE ) pHotKeys;
	if ( g_pHotKeys )
	{
		free( g_pHotKeys );
		g_pHotKeys = NULL;
		g_nHotKeyCount = g_nHotKeyAllocCount = 0;
	}

	BYTE byVer = *pBuff;
	pBuff++;
	int nCount = ( int ) ( *( DWORD * ) pBuff ); // count
	pBuff += sizeof( DWORD );

	if ( nCount )
	{
		if ( byVer == 0 )	 // old version -- array of DWORD dwHotKeys
		{
			for ( int i = 0; i < nCount; i++ )
			{
				WORD wCmd = *( WORD * )pBuff;
				pBuff += sizeof( WORD );
				DWORD dwHotKey = *( DWORD * )pBuff;
				// convert to new format
				CM_HOTKEY cmHotKey = { ( BYTE  ) ( ( dwHotKey & 0x0000FF00 ) >> 8 ),
				                       ( TCHAR ) (   dwHotKey & 0x000000FF ),
				                       ( BYTE  ) ( ( dwHotKey & 0xFF000000 ) >> 24 ),
				                       ( TCHAR ) ( ( dwHotKey & 0x00FF0000 ) >> 16 ) };
				pBuff += sizeof( DWORD );
				RegisterHotKey( cmHotKey, wCmd );
			}
		}
		else if ( byVer == 1 ) // new version - CM_HOTKEY array
		{
			
			for ( int i = 0; i < nCount; i++ )
			{
				WORD wCmd = *( WORD * )pBuff;
				pBuff += sizeof( WORD );
				CM_HOTKEY cmHotKey = *( CM_HOTKEY * )pBuff;
				pBuff += sizeof( CM_HOTKEY );
				RegisterHotKey( cmHotKey, wCmd );
			}
		}
	}

	return TRUE;
}

void CEdit::NormalizeHotKey( CM_HOTKEY &cmHotKey )
{
	// extended keys are treated the same as non-extended
	cmHotKey.byModifiers1 &= ~HOTKEYF_EXT;
	cmHotKey.byModifiers2 &= ~HOTKEYF_EXT;

	// if both hotkeys modifiers are identical in a multi-part hotkey, then strip out the second
	// set of modifiers.  For examples, [Ctrl+Q, Ctrl+F] is the same as [Ctrl+Q, F].
	if ( cmHotKey.byModifiers2 && cmHotKey.byModifiers2 == cmHotKey.byModifiers1 )
	{
		cmHotKey.byModifiers2 = 0;
	}
}

