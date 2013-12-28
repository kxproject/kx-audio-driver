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


#ifndef _DANE_BUILDERROR_H_
#define _DANE_BUILDERROR_H_

#define _DBGSTOP _asm int 3

/* Build Errors */

#define MSG_TRANSL		"Translating...\n"
#define MSG_BUILD		"Building...\n"
#define MSG_CANTTRANSL	"Can't translate.\n\n"
#define MSG_CANTBUILD	"Can't build.\n\n"
#define MSG_DONE		"Done.\n\n"

#define _ERR	0
#define _WRN	1

enum E_ERROR
{
	ERR_NONE,
	ERR_UNK,
	ERR_SRCFILE,
	ERR_DSTFILE,
	ERR_INVARGS,

	ERR_INVCHAR,
	ERR_TOOBIGSYMB,
	ERR_TOOBIGCMNT,
	ERR_TOOBIGSTR,
	ERR_UNEXPEOF,
	ERR_STRNEWLINE,

	ERR_UNEXPTOKEN,
	ERR_UNKP,
	ERR_MISSTANKOP,
	ERR_RWDELAYONLY,
	ERR_DUPTOKEN,
	ERR_CTRLONLY,
	ERR_REGALRDYSET,
	ERR_NOTFORCRSE,
	ERR_STATCTRLONLY,
	ERR_EXPRSRCVAL,
	ERR_MISSOPS,
	ERR_MISSING,
	ERR_EXPENDOFI,
	ERR_NOADDR,
	ERR_NULLCNSTONLY,
	ERR_UNDECLARED,
	ERR_EXPID,
	ERR_MISSNAME,
	ERR_REDEF,
	ERR_EXPENDOFE,
	ERR_MISSVALUE,
	ERR_NOTASGN,
	ERR_NOTAT,
	ERR_SYNTAX1,
	ERR_NOTDIGIT,
	ERR_CTRLREADONLY,
	ERR_INVDIGIT,
	ERR_UNEXPSTR,
	ERR_STREXP,
	ERR_INVDELAYOFST,
	ERR_ACCASAONLY,
	ERR_N1READONLY,
	ERR_N2READONLY,
	ERR_DBACREADONLY,
	ERR_TYPEUNEXP,
	ERR_INVHEX,

	ERR_INSTROVER,
	ERR_CONSTOVER,
	ERR_REGOVER,

	WRN_UNK,

	WRN_INTTRUNK,
	WRN_ACCASRESULT,
	WRN_REALSATUR,
};

extern const char *E_STRING[][2];

#endif // _BUILDERROR_H_	
