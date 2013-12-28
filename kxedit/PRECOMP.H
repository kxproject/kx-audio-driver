// UNICODE and _UNICODE go hand in hand
#ifdef _UNICODE
#ifndef UNICODE
#define UNICODE
#endif
#endif
#ifdef UNICODE
#ifndef _UNICODE
#define _UNICODE
#endif
#endif

#pragma warning ( disable : 4514 4201 4702 4552 4553 4710 )
#ifdef _UNICODE
// disable the 'conditional expression is constant' warning caused by the
// DBCS code that is auto-compiled out.
#pragma warning ( disable : 4127 )
#endif
// #define _WIN32_WINNT 0x0400 // for WM_MOUSEWHEEL

#include <windows.h>
#include <winuser.h>
#include <commctrl.h>
#include <limits.h>
#include <tchar.h>
#include <mbctype.h>
#include <locale.h>

#include <comdef.h>
#include <commdlg.h>
#include <shellapi.h>

//#define _CODEMAX_BETA
#include "debug.h"
#include "codemax.h"
#include "cedit.h"
#include "misc.h"

#include <atlbase.h>
#include <atlimpl.cpp>
#include <atlctl.cpp>
#include <atlwin.cpp>


#ifdef _ATL_STATIC_REGISTRY
#include <statreg.h>
#include <statreg.cpp>
#endif

#pragma warning ( disable : 4100 )
#include <climits>
#include <memory>
#pragma warning ( disable : 4245 )
#include <stdexcept>
#include <xutility>
#pragma warning ( disable : 4663 4097 4189 4018 )
#include <vector>
#include <map>

#if !defined(GWL_USERDATA)
 #define GWL_USERDATA        (-21)
 #define GWL_HINSTANCE       (-6)
 #define DWL_MSGRESULT   0
#endif

using namespace std; 
