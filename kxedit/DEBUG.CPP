#include "precomp.h"

#ifdef _DEBUG
BOOL _CODEMAX_ASSERT( BOOL bCond, LPCSTR pszFile, int nLine )
{
	if ( !bCond )
	{
		char szMsg[ 200 ];
		wsprintfA( szMsg, "Assertion Failure: %s, Line %d", pszFile, nLine );
		int nResult = MessageBoxA( NULL, szMsg, "CodeMax", MB_ABORTRETRYIGNORE | MB_ICONEXCLAMATION );
		if ( nResult == IDABORT )
		{
			exit( 0 );
		}
		else if ( nResult == IDRETRY )
		{
			return FALSE;
		}
	}

	return TRUE;
}
#endif
