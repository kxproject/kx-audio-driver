// kX Driver Interface
// Copyright (c) Eugene Gavrilov, 2001-2004.
// All rights reserved

#include "stdafx.h"

#ifdef KX_DEBUG

void debug(wchar_t *__format,...)
{
 wchar_t my_internal_buf[2048];
 va_list ap;
 va_start(ap, __format);

 if(_vsnwprintf(my_internal_buf, 2048, __format, ap))
 {
  OutputDebugStringW(my_internal_buf);
 }
}

void debug(char *__format,...)
{
 char my_internal_buf[2048];
 va_list ap;
 va_start(ap, __format);

 if(_vsnprintf(my_internal_buf, 2048, __format, ap))
 {
  OutputDebugStringA(my_internal_buf);
 }
}

#endif
