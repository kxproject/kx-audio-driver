#define UNUSED(x) x

#ifdef _DEBUG
	#define VERIFY( x ) ASSERT( ( x ) != 0 )
	#define ASSERT( x ) if ( !_CODEMAX_ASSERT( ( x ) != 0, __FILE__, __LINE__ ) ) _asm { int 3 };

	BOOL _CODEMAX_ASSERT( BOOL bCond, LPCSTR pszFile, int nLine );
#else
	#define ASSERT( x )
	#define VERIFY( x ) x
#endif

#ifdef _CMTRACE 
	#define CMTRACE0( x ) OutputDebugString( x )
#else
	#define CMTRACE0( x )
#endif
