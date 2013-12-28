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


#ifndef _DANE_IMOBJ_H_
#define _DANE_IMOBJ_H_

typedef struct {
	// should have the same size
	char  Name[KX_MAX_STRING];
        char  Copyright[KX_MAX_STRING];
	char  Engine[KX_MAX_STRING];
        char  Created[KX_MAX_STRING];
	char  Comment[KX_MAX_STRING];
	char  Guid[KX_MAX_STRING];

    int  ITankSize;
    int  XTankSize;

	int  RegistersCount;
	int  ConstantsCount;
	int  InstructionsCount;

	int  Wscl;
	int  Iscl;
	int  Oscl;

	int  Intr;

} IMPGMRSRC;

typedef struct {
	char symbol[32];
	int tindex;
	int type;
	int id;
	int tag;
} reg;

typedef struct {
	int opcode;
	int itag;
	short ops[4];
	short tags[4];
} iminstr;

#define MAXNUMREGS		0x300
#define MAXNUMCNSTS		0x100
#define MAXNUMINSTRS		0x400

#define RT_ID_MASK		0x0fff
#define RT_CONST		0x2000
#define RT_HWR			0x4000

#define _OPR			0x00
#define _OPA			0x01
#define _OPX			0x02
#define _OPY			0x03
#define NUMOPS			0x04

#define OPTAG_ADDR		0x0001	

#endif // _IMOBJ_H_
