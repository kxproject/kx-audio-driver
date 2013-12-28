#include "precomp.h"
#include "cedit.h"
#include "resource2.h"

extern HINSTANCE hInstance;
CRegCmd *CEdit::g_pRegCmds = NULL;
int CEdit::g_nRegCmdCount = 0;
int CEdit::g_nRegCmdAllocCount = 0;

WORD g_wCmds[] = 
{
	CMD_WORDUPPERCASE,
	CMD_WORDTRANSPOSE,
	CMD_WORDRIGHTEXTEND,
	CMD_WORDRIGHT,
	CMD_WORDENDRIGHT,
	CMD_WORDENDRIGHTEXTEND,
	CMD_WORDLOWERCASE,
	CMD_WORDLEFTEXTEND,
	CMD_WORDLEFT,
	CMD_WORDENDLEFT,
	CMD_WORDENDLEFTEXTEND,
	CMD_WORDDELETETOSTART,
	CMD_WORDDELETETOEND,
	CMD_WORDCAPITALIZE,
	CMD_WINDOWSTART,
	CMD_WINDOWSCROLLUP,
	CMD_WINDOWSCROLLTOTOP,
	CMD_WINDOWSCROLLTOCENTER,
	CMD_WINDOWSCROLLTOBOTTOM,
	CMD_WINDOWSCROLLRIGHT,
	CMD_WINDOWSCROLLLEFT,
	CMD_WINDOWSCROLLDOWN,
	CMD_WINDOWRIGHTEDGE,
	CMD_WINDOWLEFTEDGE,
	CMD_WINDOWEND,
	CMD_UPPERCASESELECTION,
	CMD_UNTABIFYSELECTION,
	CMD_UNINDENTSELECTION,
	CMD_UNDOCHANGES,
	CMD_UNDO,
	CMD_TABIFYSELECTION,
	CMD_SENTENCERIGHT,
	CMD_SENTENCELEFT,
	CMD_SENTENCECUT,
	CMD_SELECTSWAPANCHOR,
	CMD_SELECTPARA,
	CMD_SELECTLINE,
	CMD_SELECTALL,
	CMD_REDOCHANGES,
	CMD_REDO,
	CMD_PASTE,
	CMD_PARAUP,
	CMD_PARADOWN,
	CMD_PAGEUPEXTEND,
	CMD_PAGEUP,
	CMD_PAGEDOWNEXTEND,
	CMD_PAGEDOWN,
	CMD_LOWERCASESELECTION,
	CMD_LINEUPEXTEND,
	CMD_LINEUP,
	CMD_LINETRANSPOSE,
	CMD_LINESTART,
	CMD_LINEOPENBELOW,
	CMD_LINEOPENABOVE,
	CMD_LINEENDEXTEND,
	CMD_LINEEND,
	CMD_LINEDOWNEXTEND,
	CMD_LINEDOWN,
	CMD_LINEDELETETOSTART,
	CMD_LINEDELETETOEND,
	CMD_LINEDELETE,
	CMD_LINECUT,
	CMD_INDENTTOPREV,
	CMD_INDENTSELECTION,
	CMD_HOMEEXTEND,
	CMD_HOME,
	CMD_GOTOMATCHBRACE,
	CMD_GOTOINDENTATION,
	CMD_GOTOLINE,
	CMD_FINDREPLACE,
	CMD_REPLACE,
	CMD_REPLACEALLINBUFFER,
	CMD_REPLACEALLINSELECTION,
	CMD_FINDPREVWORD,
	CMD_FINDPREV,
	CMD_FINDNEXTWORD,
	CMD_FINDNEXT,
	CMD_FINDMARKALL,
	CMD_FIND,
	CMD_SETFINDTEXT,
	CMD_SETREPLACETEXT,
	CMD_TOGGLEWHOLEWORD,
	CMD_TOGGLEREGEXP,
	CMD_TOGGLEPRESERVECASE,
	CMD_TOGGLECASESENSITIVE,
	CMD_END,
	CMD_TOGGLEWHITESPACEDISPLAY,
	CMD_TOGGLEOVERTYPE,
	CMD_SETREPEATCOUNT,
	CMD_DOCUMENTSTARTEXTEND,
	CMD_DOCUMENTSTART,
	CMD_DOCUMENTENDEXTEND,
	CMD_DOCUMENTEND,
	CMD_DELETEHORIZONTALSPACE,
	CMD_DELETEBLANKLINES,
	CMD_DELETEBACK,
	CMD_DELETE,
	CMD_CUTSELECTION,
	CMD_CUT,
	CMD_COPY,
	CMD_CHARTRANSPOSE,
	CMD_CHARRIGHTEXTEND,
	CMD_CHARRIGHT,
	CMD_CHARLEFTEXTEND,
	CMD_CHARLEFT,
	CMD_BOOKMARKTOGGLE,
	CMD_BOOKMARKPREV,
	CMD_BOOKMARKNEXT,
	CMD_BOOKMARKCLEARALL,
	CMD_BOOKMARKJUMPTOFIRST,
	CMD_BOOKMARKJUMPTOLAST,
	CMD_APPENDNEXTCUT,
	CMD_INSERTCHAR,
	CMD_NEWLINE,
	CMD_RECORDMACRO,
	CMD_PLAYMACRO1,
	CMD_PLAYMACRO2,
	CMD_PLAYMACRO3,
	CMD_PLAYMACRO4,
	CMD_PLAYMACRO5,
	CMD_PLAYMACRO6,
	CMD_PLAYMACRO7,
	CMD_PLAYMACRO8,
	CMD_PLAYMACRO9,
	CMD_PLAYMACRO10,
	CMD_PROPERTIES,
	CMD_BEGINUNDO,
	CMD_ENDUNDO,
	CMD_CLEARSELECTION,
	CMD_REGEXPON,
	CMD_REGEXPOFF,
	CMD_WHOLEWORDON,
	CMD_WHOLEWORDOFF,
	CMD_PRESERVECASEON,
	CMD_PRESERVECASEOFF,
	CMD_CASESENSITIVEON,
	CMD_CASESENSITIVEOFF,
	CMD_WHITESPACEDISPLAYON,
	CMD_WHITESPACEDISPLAYOFF,
	CMD_OVERTYPEON,
	CMD_OVERTYPEOFF
};

void CEdit::GetCommandString( WORD wCmd, BOOL bDescription, LPTSTR psz, int cb )
{
	*psz = _T('\0');

	if ( wCmd >= CMD_USER_BASE )
	{
		// registered command
		for ( int i = 0; i < g_nRegCmdCount; i++ )
		{
			CRegCmd *pCmd = g_pRegCmds + i;
			if ( pCmd->wCmd == wCmd )
			{
				_tcsncpy( psz, bDescription ? pCmd->szDesc : pCmd->szName, cb );
				cb = max( 1, cb );
				psz[ cb - 1 ] = _T('\0');
				break;
			}
		}
	}
	else
	{
		// intrinsic command
		for ( int i = 0; i < ARRAY_SIZE( g_wCmds ); i++ )
		{
			if ( g_wCmds[ i ] == wCmd )
			{
				LoadString( hInstance, wCmd + ( 1 - bDescription ) * CMD_TO_STRING_OFFSET, psz, cb - 1 );
				psz[ cb - 1 ] = _T('\0');
			}
		}
	}
}

BOOL CEdit::RegisterCommand( WORD wCmd, LPCTSTR pszName, LPCTSTR pszDesc )
{
	// avoid duplicate entries
	if ( IsRegisteredCommand( wCmd ) )
		return FALSE;

	int nInsertAt = g_nRegCmdCount;

	if ( !g_pRegCmds )
	{
		g_pRegCmds = ( CRegCmd * ) malloc( REGCMD_BLOCKSIZE * sizeof( CRegCmd ) );
		if ( !g_pRegCmds )
		{
			return FALSE;
		}
		g_nRegCmdAllocCount = REGCMD_BLOCKSIZE;
	}

	if ( g_nRegCmdCount + 1 > g_nRegCmdAllocCount )
	{
		g_nRegCmdAllocCount += REGCMD_BLOCKSIZE;
		CRegCmd *pNew = ( CRegCmd * )realloc( g_pRegCmds, sizeof( CRegCmd ) * g_nRegCmdAllocCount );
		if ( pNew )
		{
			g_pRegCmds = pNew;
		}
		else
		{
			return FALSE;
		}
	}

	if ( g_nRegCmdCount )
	{
		memmove( g_pRegCmds + nInsertAt + 1, g_pRegCmds + nInsertAt, sizeof( CRegCmd ) * ( g_nRegCmdCount - nInsertAt ) );
	}
	g_nRegCmdCount++;

	g_pRegCmds[ nInsertAt ].wCmd = wCmd;
	_tcsncpy( g_pRegCmds[ nInsertAt ].szName, pszName, CM_MAX_CMD_STRING - 1 );
	_tcsncpy( g_pRegCmds[ nInsertAt ].szDesc, pszDesc, CM_MAX_CMD_DESCRIPTION - 1 );

	return TRUE;
}

BOOL CEdit::UnregisterCommand( WORD wCmd )
{
	int nFoundAt = -1;
	if ( g_pRegCmds )
	{
		for ( int i = 0; i < g_nRegCmdCount; i++ )
		{
			if ( g_pRegCmds[ i ].wCmd == wCmd )
			{
				nFoundAt = i;
				break;
			}
		}
	}

	if ( nFoundAt == -1 )
		return FALSE;	// command not found!

	// shrink the array
	memmove( g_pRegCmds + nFoundAt, 
		     g_pRegCmds + nFoundAt + 1, 
		     ( g_nRegCmdCount - nFoundAt - 1 ) * sizeof( CRegCmd ) );
	g_nRegCmdCount--;
	ASSERT( g_nRegCmdCount >= 0 );

	return TRUE;
}

BOOL CEdit::IsRegisteredCommand( WORD wCmd )
{
	if ( g_pRegCmds )
	{
		for ( int i = 0; i < g_nRegCmdCount; i++ )
		{
			if ( g_pRegCmds[ i ].wCmd == wCmd )
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}
