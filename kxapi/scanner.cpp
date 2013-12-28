// kX Driver Interface / Dane Assembler
// Copyright (c) Max Mikhailov, Eugene Gavrilov, 2001-2005.
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
#include "dane/dane.h"

#define _eof(a) a[bufpos] == '\0'
#define _read(a, b, c) *b = a[bufpos]; bufpos++
#define _lseek(a, b, c) bufpos += b 

int iDane::scnexttoken(token* ntoken){

	int symtype, posintoken = 0;
	char& cchr = empty[0];
	ntoken->line = cline;

	// unexpected end of file (before 'end'!)
	if (_eof(hsrc)) return _err(ERR_UNEXPEOF, ntoken->line, ERR_NONE, 0);
	

	if (iscomments == 1)
	{
		for (int i = 0; i < 256; i++)
		{
			_read(hsrc, &cchr, 1);
			symtype = _getsymtype(cchr);

			if (symtype == newline_sm)
			{
				ntoken->line = cline;
				iscomments = 0;
				ntoken->chars[0] = cchr;
				ntoken->chars[0 + 1] = '\0';
				cline++;
				return 0;
			}
	
		}
		return _err(ERR_TOOBIGCMNT, ntoken->line, ERR_NONE, 0);
		// return 1; // error: too long comments

	}

nextsym:

	_read(hsrc, &cchr, 1);
	symtype = _getsymtype(cchr);

	switch (symtype)
	{

	case alpha_sm:
		ntoken->chars[0] = cchr;
		ntoken->line = cline;

		while (posintoken < MAXTOKENSIZE)
		{
			posintoken++;

			if (_eof(hsrc)){
				ntoken->chars[posintoken] = '\0';
				return 0;
			};

			_read(hsrc, &cchr, 1);
			symtype = _getsymtype(cchr);

			switch (symtype)
			{
			case alpha_sm:
				ntoken->chars[posintoken] = cchr;
				break;

			case newline_sm:
			case semicolon_sm:
			case assign_sm:
			case address_sm:
			case delimiter_sm:
			case comma_sm:
				ntoken->chars[posintoken] = '\0';
				_lseek(hsrc, -1, 1);
				return 0;
				break;

			case unknown_sm:
				return _err(ERR_INVCHAR, ntoken->line, ERR_NONE, &cchr);
				// return 1; // error: invalid char

			}

		}
		return _err(ERR_TOOBIGSYMB, ntoken->line, ERR_NONE, 0);
		// return 1; // error: indentifier cannot be more then 32 characters long

	case string_sm:
		ntoken->chars[0] = cchr;
		ntoken->line = cline;

		while (posintoken < MAXTOKENSIZE)
		{
			posintoken++;

			if (_eof(hsrc)){
				ntoken->chars[posintoken] = '\0';
				return 0;
			};

			_read(hsrc, &cchr, 1);
			symtype = _getsymtype(cchr);

			switch (symtype)
			{
			case string_sm:
				ntoken->chars[posintoken] = cchr;
				ntoken->chars[posintoken + 1] = '\0';
				return 0;

			case newline_sm:
				return _err(ERR_STRNEWLINE, ntoken->line, ERR_NONE, 0);
				// return 1; // error: new line in string

			//case unknown_sm:
				//return _err(ERR_INVCHAR, ntoken->line, ERR_NONE, &cchr);
				// return 1; // error: invalid char

			default:
				ntoken->chars[posintoken] = cchr;
			}

		}
		return _err(ERR_TOOBIGSTR, ntoken->line, ERR_NONE, 0);
		// return 1; // error: string cannot be more then 256 characters long


	case semicolon_sm:
		iscomments = 1;
	case address_sm:
	case assign_sm:
	case comma_sm:
		ntoken->line = cline;
		ntoken->chars[0] = cchr;
		ntoken->chars[1] = '\0';
		return 0;

	case newline_sm:
		ntoken->line = cline;
		ntoken->chars[0] = cchr;
		ntoken->chars[1] = '\0';
		cline++;
		return 0;

	case delimiter_sm:
		goto nextsym;

	case unknown_sm:
		return _err(ERR_INVCHAR, ntoken->line, ERR_NONE, &cchr);
		// return 1; // error: invalid char
		
	}

	return 1;

}


int iDane::_getsymtype(char symbol){

	if ((symbol >= 'A' && symbol <= 'Z') || 
		(symbol >= 'a' && symbol <= 'z') ||
		(symbol >= '0' && symbol <= '9') ||
		(symbol == '_') || 
		(symbol == '.') || 
		(symbol == '+') ||
		(symbol == '-'))
    return alpha_sm;

	switch (symbol)
	{
	case ';':
		return semicolon_sm;

	case '&':
		return address_sm;

	case ',':
		return comma_sm;

	case '=':
		return assign_sm;

	case '"':
		return string_sm;

	case ' ':
	case '\t':
	case '\r':
		return delimiter_sm;

	case '\n':
		return newline_sm;

	default:
		return unknown_sm;
	}
}
