
#ifndef __DBG_H_
#define __DBG_H_

#if !defined(NDEBUG)

#include <stdio.h>

inline void trace(const char* format, ...)
{
	enum {MAX_TRACE_STRING = 1024};
	char msg[MAX_TRACE_STRING];
	va_list arglist;
    va_start(arglist, format);
	_vsntprintf(msg, MAX_TRACE_STRING, format, arglist);
	va_end(arglist);
	OutputDebugString(msg);
}

#else // NDEBUG
#define trace
#endif 

//.............................................................................

#endif // __DBG_H_
