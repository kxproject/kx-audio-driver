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


#ifndef _DANE_LANGDEF_H_
#define _DANE_LANGDEF_H_

	/* Language Keywords */    
	
	#define K_RDEC_BIT      0x00010000
	#define K_INPUT			0x00000001        
	#define K_OUTPUT		0x00000002
	#define K_STATIC		0x00000003
	#define K_CONTROL		0x00000004
	#define K_CONST			0x00000005
	#define K_TEMP			0x00000006
	#define K_IDELAY		0x00000007
	#define K_XDELAY		0x00000008
	#define K_RDEC_MASK		0x0000000f
	
	#define K_RPROP_BIT		0x00020000
	#define K_READ			0x00000010
	#define K_ADD			0x00000020
	#define K_WRITE			0x00000030
	#define K_RPROP_MASK	0x000000f0
	
	#define K_RSUB_BIT		0x00040000
	#define K_WSCL			0x00000100
	#define K_ISCL			0x00000200
	#define K_OSCL			0x00000300   
	#define K_INTR			0x00000400
	#define K_COARSE		0x00001000
	#define K_RSUB_MASK		0x0000ff00   
	#define K_XSCL_MASK		0x00000f00  
	#define K_20BIT_MASK	0x0000f000     
	
	#define K_RSRC_BIT		0x00080000      
	#define K_NAME			0x00000001
	#define K_COPYRIGHT		0x00000002
	#define K_ENGINE		0x00000003
	#define K_CREATED		0x00000004
	#define K_COMMENT		0x00000005
	#define K_GUID			0x00000006
	#define K_ITRAMSIZE		0x00000007
	#define K_XTRAMSIZE		0x00000008
	#define K_RSRC_MASK		0x0000000f
	
	#define K_OPCODE_BIT	0x00100000  
	#define K_MACS			0x00000000         
	#define K_MACSN			0x00000001
	#define K_MACW			0x00000002
	#define K_MACWN			0x00000003
	#define K_MACINTS		0x00000004
	#define K_MACINTW		0x00000005
	#define K_ACC3			0x00000006
	#define K_MACMV			0x00000007
	#define K_ANDXOR		0x00000008
	#define K_TSTNEG		0x00000009
	#define K_LIMIT			0x0000000a
	#define K_LIMITN		0x0000000b
	#define K_LOG			0x0000000c
	#define K_EXP			0x0000000d       
	#define K_INTERP		0x0000000e
	#define K_SKIP			0x0000000f
	#define K_OPCODE_MASK	0x0000000f   
	                    
	#define K_HWR_BIT		0x00200000
	#define K_CCR			0x00000001
	#define K_IRQ			0x00000002
	#define K_ACCUM			0x00000003		
	#define K_NOISE1		0x00000004
	#define K_NOISE2		0x00000005
	#define K_DBAC			0x00000006
	#define K_HWR_MASK		0x0000000f
	
	#define K_RDATA_BIT		0x01000000
	#define K_AT			0x00000001
	#define K_ASSIGN		0x00000002  
	#define K_RDATA_MASK	0x0000000f
	
	#define K_COMMA			0x10000000

	#define K_ADDRESS		0x20000000
	
	#define K_ENDEXPR		0x30000000
	
	#define K_END			0x40000000
	
	#define K_NOTKEYWORD	0x80000000       

	#define K_GROUP_MASK	0xffff0000  
    #define K_RTYPE_MASK	0xffff0000
	
	#define NUMKWS			53
	
    const int KEYWIDS[NUMKWS]=
    {     
		K_COMMA,		
		
		K_ENDEXPR,
		K_ENDEXPR,
		
		K_OPCODE_BIT | K_MACS,		      
		K_OPCODE_BIT | K_MACSN,		
		K_OPCODE_BIT | K_MACW,		
		K_OPCODE_BIT | K_MACWN,		
		K_OPCODE_BIT | K_MACINTS,	
		K_OPCODE_BIT | K_MACINTW,	
		K_OPCODE_BIT | K_ACC3,		
		K_OPCODE_BIT | K_MACMV,		
		K_OPCODE_BIT | K_ANDXOR,		
		K_OPCODE_BIT | K_TSTNEG,		
		K_OPCODE_BIT | K_LIMIT,		
		K_OPCODE_BIT | K_LIMITN,		
		K_OPCODE_BIT | K_LOG,		
		K_OPCODE_BIT | K_EXP,		    
		K_OPCODE_BIT | K_INTERP,		
		K_OPCODE_BIT | K_SKIP,

		K_RDEC_BIT | K_INPUT,		     
		K_RDEC_BIT | K_OUTPUT,		
		K_RDEC_BIT | K_STATIC,		
		K_RDEC_BIT | K_CONTROL,	
		K_RDEC_BIT | K_CONST,		
		K_RDEC_BIT | K_TEMP,		
		K_RDEC_BIT | K_IDELAY,		
		K_RDEC_BIT | K_XDELAY,			
		
		K_RPROP_BIT | K_READ,
		K_RPROP_BIT | K_ADD,
		K_RPROP_BIT | K_WRITE,	
		
		K_RSUB_BIT | K_WSCL,		
		K_RSUB_BIT | K_ISCL,		
		K_RSUB_BIT | K_OSCL,		
		K_RSUB_BIT | K_INTR,
		K_RSUB_BIT | K_COARSE,
		   
		K_RSRC_BIT | K_NAME,				
		K_RSRC_BIT | K_COPYRIGHT,
                K_RSRC_BIT | K_ENGINE,
                K_RSRC_BIT | K_CREATED,
                K_RSRC_BIT | K_COMMENT,
                K_RSRC_BIT | K_GUID,
		K_RSRC_BIT | K_ITRAMSIZE,	
		K_RSRC_BIT | K_XTRAMSIZE,			

		K_HWR_BIT | K_CCR,	
		K_HWR_BIT | K_IRQ,
		K_HWR_BIT | K_ACCUM,
		K_HWR_BIT | K_NOISE1,		
		K_HWR_BIT | K_NOISE2,		
		K_HWR_BIT | K_DBAC,		
		
		K_RDATA_BIT | K_AT,			
		K_RDATA_BIT | K_ASSIGN,		

		K_ADDRESS,
		
		K_END	
	};	

	extern const char *KEYWORDS[];

#endif	// _LANGDEF_H_
