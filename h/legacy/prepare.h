// kX Driver  / kX Driver Interface
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


// *** Prepare **
#include "legacy/clear.h"

#define begin_microcode(name)			\
    	microcode_name=name;		\
    	last_gpr=0x8000;	                \
    	last_input_gpr=0x4000;              \
    	itramsize=0;                        \
    	xtramsize=0;                        \
    	code_size=0;                        \
    	info_size=0;				\
    	dyn_code=0; dyn_info=0; 		\
    	cur_info=0;							\
    	cur_code=0;


#define end_microcode()							\
	dyn_code=NULL; dyn_info=NULL; 		\
	code_size=code_size*(int)sizeof(dsp_code);				\
	info_size=info_size*(int)sizeof(dsp_register_info);			\
	my_alloc(hw,(void **)&dyn_code,code_size);				\
	my_alloc(hw,(void **)&dyn_info,info_size);				\
	cur_info=0;							\
	cur_code=0;

#define const_gpr(a,b,a_n) int a=last_gpr++; info_size++;
#define static_gpr(a,b,a_n) int a=last_gpr++; info_size++;
#define control_gpr(a,b,c) int a=last_gpr++; info_size++;
#define vcontrol_gpr(a,b,c) int a=last_gpr++; info_size++;
#define mcontrol_gpr(a,b,c) int a=last_gpr++; info_size++;

#define add_register(a,b,c,d) int b=last_gpr++; info_size++;

#define output(a,b) int a=last_gpr++; info_size++;
#define input(a,b) int a=last_input_gpr++; info_size++;
#define alias(a,b) int a=b;

#define temp_gpr(a,a_n) int a=last_gpr++; info_size++;
#define intr_gpr(a,b,a_n) int a=last_gpr++; info_size++;

#define idelay_line(l,b) int l=itramsize; itramsize+=(b);
#define itwrite(l,a,aa,o,a_n) int a=last_gpr++; int aa=last_gpr++; info_size+=2;
#define itread(l,a,aa,o,a_n) int a=last_gpr++; int aa=last_gpr++; info_size+=2;
#define itram(l,a,aa,o,a_n) int a=last_gpr++; int aa=last_gpr++; info_size+=2;
#define irsaw(l,a,aa,o,a_n) int a=last_gpr++; int aa=last_gpr++; info_size+=2;

#define xdelay_line(l,b) int l=xtramsize; xtramsize+=(b);
#define xtwrite(l,a,aa,o,a_n) int a=last_gpr++; int aa=last_gpr++; info_size+=2;
#define xtread(l,a,aa,o,a_n) int a=last_gpr++; int aa=last_gpr++; info_size+=2;
#define xtram(l,a,aa,o,a_n) int a=last_gpr++; int aa=last_gpr++; info_size+=2;
#define xrsaw(l,a,aa,o,a_n) int a=last_gpr++; int aa=last_gpr++; info_size+=2;

#define OP(op, z, w, x, y) code_size++;

#undef kx_double
#define kx_double(a) (dword)((double)0x7fffffff*((double)a))

#undef kx_time
#define kx_time(a) (dword)((double)((a)*48000.0*2048.0))
