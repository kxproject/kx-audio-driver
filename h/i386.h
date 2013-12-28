// kX Driver / kX Driver Interface
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

#ifndef I386_H_
#define I386_H_

// Basic kX-related type definitions
// these are architecture-dependent:

#if defined(__GNUC__)
typedef unsigned int dword;
#else
typedef unsigned long dword;
#endif

typedef unsigned short word;
typedef unsigned char byte;

#if defined(__GNUC__)
    typedef long long __int64;
#endif

#ifdef KX_INTERNAL
 #ifndef CONIO_USAGE

#ifdef _MSC_VER
// MS Visual C specific code

#if !defined(AMD64)

        #pragma warning(disable:4035)
        inline byte inp(dword __port)
        {
        	__asm {
        		mov edx,__port
        		in al,dx
        	};
        }

        #pragma warning(disable:4035)
        inline word inpw(dword __port)
        {
        	__asm {
        		mov edx,__port
        		in ax,dx
        	};
        }
        #pragma warning(disable:4035)
        inline dword inpd(dword __port)
        {
        	__asm {
        		mov edx,__port
        		in eax,dx
        	};
        }

        inline void outp(dword __port, byte value)
        {
        	__asm {
        		mov edx,__port
        		mov al,value
        		out dx,al
        	};
        }

        inline void outpw(dword __port,word value)
        {
        	__asm {
        		mov edx,__port
        		mov ax,value
        		out dx,ax
        	};
        }

        inline void outpd(dword __port, dword value)
        {
        	__asm {
        		mov edx,__port
        		mov eax,value
        		out dx,eax
        	};
        }
#else // AMD64?

typedef unsigned char UCHAR;
typedef unsigned long ULONG;
typedef unsigned short USHORT;
#if !defined(VOID)
 typedef void VOID;
#endif
#define IN

extern "C" {
byte __inbyte (word Port);
word __inword (word Port);
dword __indword (word Port);
void __outbyte (word Port,byte Data);
void __outword (word Port,word Data);
void __outdword (word Port,dword Data);
};
#pragma intrinsic(__inbyte)
#pragma intrinsic(__inword)
#pragma intrinsic(__indword)
#pragma intrinsic(__outbyte)
#pragma intrinsic(__outword)
#pragma intrinsic(__outdword)

        inline byte inp(dword __port)
        {
                return __inbyte((word)__port);
        }

        inline word inpw(dword __port)
        {
                return __inword((word)__port);
        }
        inline dword inpd(dword __port)
        {
                return __indword((word)__port);
        }

        inline void outp(dword __port, byte value)
        {
                __outbyte((word)__port, value);
        }

        inline void outpw(dword __port,word value)
        {
                __outword((word)__port, value);
        }

        inline void outpd(dword __port, dword value)
        {
                __outdword((word)__port, value);
        }
#endif

#elif defined(__GNUC__) // _MSC_VER?

extern __inline__ dword inpd(word port)
{
	unsigned int datum;
	__asm__ volatile("inl %w1, %0" : "=a" (datum) : "Nd" (port));
	return(datum);
}

extern __inline__ word inpw(word port)
{
	unsigned short datum;
	__asm__ volatile("inw %w1, %w0" : "=a" (datum) : "Nd" (port));
	return(datum);
}

extern __inline__ byte inp(word port)
{
	unsigned char datum;
	__asm__ volatile("inb %w1, %b0" : "=a" (datum) : "Nd" (port));
	return(datum);
}

extern __inline__ void outpd(word port,
							 unsigned int value)
{
	// note: used to be 'dword', but this conflicts with x64 build in 10.6/10.7
    __asm__ volatile("outl %0, %w1" : : "a" (value), "Nd" (port));
}

extern __inline__ void outpw(word port,
							 word value)
{
    __asm__ volatile("outw %w0, %w1" : : "a" (value), "Nd" (port));
}

extern __inline__ void outp(word port,
                            byte value)
{
    __asm__ volatile("outb %b0, %w1" : : "a" (value), "Nd" (port));
}

#endif // __GNUC__
#endif // CONIO_USAGE
#endif // KX_INTERNAL

#ifndef PAGE_SIZE
 #define PAGE_SIZE 	4096
#endif

#endif
