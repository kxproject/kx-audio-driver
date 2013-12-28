// kX Driver Interface
// Copyright (c) Eugene Gavrilov, 2001-2005.
// All rights reserved

/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

#include "stdafx.h"

#ifdef KX_DEBUG

void debug(const wchar_t *__format,...)
{
 wchar_t my_internal_buf[2048];
 va_list ap;
 va_start(ap, __format);

 if(_vsnwprintf(my_internal_buf, 2048, __format, ap))
 {
  OutputDebugStringW(my_internal_buf);
 }
}

void debug(const char *__format,...)
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
