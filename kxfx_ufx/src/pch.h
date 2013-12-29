
#ifndef PCH_INCLUDED
#define PCH_INCLUDED

// ............................................................................

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x501
#endif

#define _CRT_SECURE_NO_WARNINGS 1

#define WIN32_LEAN_AND_MEAN
#define _AFX_NO_RICHEDIT_SUPPORT

#undef _DEBUG

#include <afxwin.h>
#include <afxext.h>
#include <afxdtctl.h>
#include <afxcmn.h>

// ............................................................................

#endif // ~ PCH_INCLUDED
