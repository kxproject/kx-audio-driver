// kX Interface
// Copyright (c) Eugene Gavrilov, 2008-2009.
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

kString::kString(const char *str)
{
	realloc(strlen(str));
	strcpy(buff,str);
}

kString::kString()
{
	buff=NULL;
	buff_size=0;
}

kString::~kString()
{
	if(buff)
	{
		free(buff);
		buff=0;
		buff_size=0;
	}
}

void kString::realloc(size_t new_size)
{
	size_t size=0;
	
	if(buff)
	{
		size=strlen(buff);
		if(size>new_size)
			return;
	}
	
	size+=new_size;
	size=((size/256)+1)*256;
	
	buff=(char *)::realloc(buff,size);
	buff_size=size;
}

void kString::Format(const char *fmt,...)
{
	char tmp_str[8192];
	va_list args;
	va_start(args,fmt);
	vsprintf(tmp_str,fmt,args);
	va_end(args);
	
	realloc((int)strlen(tmp_str));
	strcpy(buff,tmp_str);
}

kString& kString::operator+=(const char *to_add)
{
	if(to_add && *to_add)
	{
		size_t final_size=0;
	
		if(buff)
			final_size=strlen(buff);
		final_size+=strlen(to_add);
	
		realloc(final_size);
		strcat(buff,to_add);
	
		return *this;
	}
	
	return *this;
}

kString& kString::operator+=(kString &t)
{
	(*this)+=(const char *)t;
	
	return (*this);
}

kString::operator const char *()
{
	return buff;
}
