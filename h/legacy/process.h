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


#include "legacy/clear.h"

#define add_register(name1,num1,type1,p1)		\
	do { strncpy(dyn_info[cur_info].name,name1,MAX_GPR_NAME);		\
	     dyn_info[cur_info].num=(word)num1;			\
	     dyn_info[cur_info].type=(byte)type1;			\
	     dyn_info[cur_info].p=(dword)p1;			\
	     dyn_info[cur_info].translated=(word)DSP_REG_NOT_TRANSLATED;	\
	     cur_info++;					\
	   } while(0);

#define const_gpr(a,b,c) add_register(c,a,GPR_CONST,b);
#define static_gpr(a,b,c) add_register(c,a,GPR_STATIC,b);
#define control_gpr(a,b,c) add_register(c,a,GPR_CONTROL,b);
#define vcontrol_gpr(a,b,c) add_register(c,a,GPR_VCONTROL,b);
#define mcontrol_gpr(a,b,c) add_register(c,a,GPR_MCONTROL,b);
#define temp_gpr(a,a_n) add_register(a_n,a,GPR_TEMP,0x0);
#define intr_gpr(a,b,a_n) add_register(a_n,a,GPR_INTR,b);

#define output(a,b) add_register(b,a,GPR_OUTPUT,0x0);
#define input(a,b) add_register(b,a,GPR_INPUT,0x0);
#define alias(a,b)

#define begin_microcode(name)

#define end_microcode()				\
	my_free(hw,code);	\
	my_free(hw,info);	

#define idelay_line(l,b) 
#define itwrite(l,a,aa,o,a_n) add_register(a_n,a,GPR_ITRAM|TRAM_WRITE,l+o); add_register(a_n"_a",aa,GPR_TRAMA,l+o); 
#define itread(l,a,aa,o,a_n) add_register(a_n,a,GPR_ITRAM|TRAM_READ,l+o); add_register(a_n"_a",aa,GPR_TRAMA,l+o); 
#define itram(l,a,aa,o,a_n) add_register(a_n,a,GPR_ITRAM,l+o); add_register(a_n"_a",aa,GPR_TRAMA,l+o); 
#define irsaw(l,a,aa,o,a_n) add_register(a_n,a,GPR_ITRAM|TRAM_WRITE|TRAM_READ,l+o); add_register(a_n"_a",aa,GPR_TRAMA,l+o); 

#define xdelay_line(l,b) 
#define xtwrite(l,a,aa,o,a_n) add_register(a_n,a,GPR_XTRAM|TRAM_WRITE,l+o); add_register(a_n"_a",aa,GPR_TRAMA,l+o); 
#define xtread(l,a,aa,o,a_n) add_register(a_n,a,GPR_XTRAM|TRAM_READ,l+o); add_register(a_n"_a",aa,GPR_TRAMA,l+o); 
#define xtram(l,a,aa,o,a_n) add_register(a_n,a,GPR_XTRAM,l+o); add_register(a_n"_a",aa,GPR_TRAMA,l+o); 
#define xrsaw(l,a,aa,o,a_n) add_register(a_n,a,GPR_XTRAM|TRAM_WRITE|TRAM_READ,l+o); add_register(a_n"_a",aa,GPR_TRAMA,l+o); 

#define OP(op1, r1, a1, x1, y1) 			\
		do {				\
			dyn_code[cur_code].op=op1;	\
			dyn_code[cur_code].r=(word)r1;	\
			dyn_code[cur_code].a=(word)a1;	\
			dyn_code[cur_code].x=(word)x1;	\
			dyn_code[cur_code].y=(word)y1;	\
			cur_code++;	\
		}while(0);
