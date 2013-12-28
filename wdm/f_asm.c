// kX Driver
// Copyright (c) Eugene Gavrilov, 2001-2014.
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


/************************************
  REVISION LOG ENTRY
  File: f_asm.c
  Revision By: S.L.Kommarynsky (slkom@sao.ru)
  Revised on 15.04.2002 4:03:57
  Comments: CPU >= 486
 ************************************/

//работает при любых count > 0
//при нечетных count просто не изменяет последний byte

#undef PTR

inline void __fastcall swap_bytes(register byte *buf, register int count)
{	
	__asm	{
; buf = ecx
; count = edx

		mov	edi, edx
		sar	edx, 3
		and	edi, 6
		test	edi, edi
		jz	SHORT NEXT
		sal	edx, 3
		mov	ebx, ecx
		shr	edi, 1
		add	ebx, edx
NEXT1:
		movzx	eax, WORD PTR[ebx]
		bswap 	eax
		ror	eax, 16
		mov	WORD PTR[ebx], ax
		add	ebx, 2
		dec	edi
		jnz	SHORT NEXT1

		test	edx, edx
		jz	SHORT BYE
		sar	edx, 3
NEXT:
		mov	eax, DWORD PTR[ecx]
		mov	ebx, DWORD PTR[ecx+4]
		bswap	eax
		bswap 	ebx
		add	ecx, 8
		ror	eax, 16
		ror	ebx, 16
		mov	DWORD PTR[ecx-8], eax
		mov	DWORD PTR[ecx-4], ebx
		dec	edx
		jne	SHORT NEXT
BYE:
	}
}
