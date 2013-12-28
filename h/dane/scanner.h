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


#ifndef _DANE_SCANNER_H_
#define _DANE_SCANNER_H_

	enum smtypes
	{
		non_sm,			// not a symbol, reserved
		alpha_sm,		// "A-Z", "a-z", "0-9", "_", ".", "+", "-"
		semicolon_sm,	// ";"
		newline_sm,		// linebreak, newline etc.
		address_sm,		// "&"
		comma_sm,		// ","
		assign_sm,		// "="
		string_sm,		// """
		delimiter_sm,	// space, tab
		unknown_sm,		// all others
		eof_sm			// end of file
	};

	#define MAXTOKENSIZE 256

	typedef struct {
		char chars[MAXTOKENSIZE];
		int line;
	} token;

#endif	// _SCANNER_H_
