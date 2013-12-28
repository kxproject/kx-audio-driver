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
#ifndef _WIN32_WINNT
 #define _WIN32_WINNT 0x0400 // for WM_MOUSEWHEEL
#endif

#ifdef _ACTIVEX
#define _ATL_APARTMENT_THREADED
#define STRICT

#define CM_X_VERSION 2

#define _ATL_STATIC_REGISTRY 

//#define ATL_TRACE_LEVEL 0xfffffff  // uncomment for ATL trace messages
#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>
#include <atlctl.h>
#include <docobj.h>
#endif

#define NOTOOLBAR
#define NOUPDOWN
#define NOSTATUSBAR
#define NOMENUHELP
#define NOTRACKBAR
#define NODRAGLIST
#define NOPROGRESS
#define NOHEADER
#define NOLISTVIEW
#define NOTREEVIEW
#define NOTABCONTROL
#define NOANIMATE

#undef STRICT
#include <windows.h>
#include <commctrl.h>
#include <limits.h>
#include <tchar.h>
#include <mbctype.h>
#include <locale.h>
#undef STRICT
//#define _CODEMAX_BETA
#include "debug.h"
#include "codemax.h"
#include "cedit.h"
#include "misc.h"

#ifdef _ATL_STATIC_REGISTRY
#include <statreg.h>
#include <statreg.cpp>
#endif

#include <atlbase.h>
#include <atlimpl.cpp>
#include <atlctl.cpp>
#include <atlwin.cpp>

#pragma warning ( disable : 4100 )
#include <climits>
#include <memory>
#pragma warning ( disable : 4245 )
#include <stdexcept>
#include <xutility>
#pragma warning ( disable : 4663 4097 4189 4018 )
#include <vector>
#include <map>

using namespace std; 
