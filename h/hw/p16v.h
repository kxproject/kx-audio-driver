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

#ifndef KX_P16_V_H
#define KX_P16_V_H

// p16v / alice3 hardware registers

#define pPTR			0x20
 #define pPTR_CHANNEL		 0x1ff
 #define pPTR_REG		     0xfff0000

#define pDATA			0x24
#define pIPR			0x28
#define pINTE			0x2c
#define pHCFG			0x34
 #define pHCFG_EA		 1	// enable audio
 #define pHCFG_RI		 0x100	// rec in phase
 #define pHCFG_PI		 0x200	// pb in phase
 #define pHCFG_DRI		 0x8	// dummy record (zero)
 #define pHCFG_RBEN		 0x4000	// enable src48->fxengine routing

#define p16vCDIF_IPR	 0x38
#define p16vCDIF_INTE	 0x3c

// p16v PTR register space
#define pREG(a)			((a)<<16)

// scatter/gather
#define p16vPTBA		0x0	// base address; 2DW aligned (000)
#define p16vPTBS		0x1	// buffer size (in bytes); 
 #define pTBSA_MASK		   0x3f0000 
#define p16vPTCA		0x2	// cur. address (in bytes)
 #define pPTCA_MASK		   0x3f

#define p16vPFBA		0x4	// base address
#define p16vPFBS		0x5	// buffer size in bytes
 #define pPFBS_MASK		   0xffff0000
#define p16vCPFA		0x6	// cur. address (in bytes)
 #define CPFA_MASK		    0xffff
#define p16vPFEA		0x7	// end address: mask: 0xffff
 #define pPFEA_SL		    0x10000	// stop at loop
#define pCPCAV			0x8
 #define pCPCAV_CFA		    0x3f0000	// Current address of playback fifo in audio samples (DW)
 #define pCPCAV_CSV		    0x3f		// Current status of cache size in audio samples

// 9..f unused

#define pRFBA			0x10
#define pRFBS			0x11	// 0xffff0000
#define pCRFA			0x12
#define pCRCAV			0x13

// P16vRecVolX
#define p16vRecVolL		0x14	// rec. path 0..3; 0xff-aligned; mask: 0x1f 
#define p16vRecVolH		0x15	// rec. path 4..7

#define p16vRecSel		0x16
 #define pRS_Fx_ChnlSel		    0x7000000 // fx engine recording (0=0x60..0x67, 1=0x68..0x6f etc.)
 #define pRS_PlayMixerInEnb 	0xf00000 // When any bit is set, playback mixer input for the corresponding path is selected
 #define pRS_PlayMixerOutEnb	0xf0000  // When any bit is set, playback mixer output for the corresponding path is selected
 // + playback / recording routings
#define p16vRecRate		0x17	// read only
 #define pRR_SR0		  0x30000 // sample rate
 #define pRR_R0V		  0x80000 // valid
 // -""- for sr1,sr2,sr3

// 0x18..0x1f - unused

#define p16vPCD			0x20	// Playback fifo cache data at location determined by CNL 

// 0x21..0x3f - reserved

#define p16vSA			    0x40	// start audio
 #define pSA_Playback		0xf	// SPAx
 #define pSA_Recording		0xf0	// SRAx
 #define pSA_RS			    0x70000	// RSx select:
 	// 0: spdif X
        // 1: i2s X
        // 2: src48 X
        // 3: srcmulti X
        // 4: cdif X
        // for channel 3: 0: ac97 ADC, 1 - src48 #3, 3: srcmulti #3

// 0x41 - reserved

// #define pDVD			0x42	// DVD Key ? actually, reserved [per ca0102 specs]

// 0x41-0x45 - reserved?

// #define pWatermark		0x46 // is it p16v-only?,..

// 0x47-0x48 - reserved

// #define pRCD			0x50     // record fifo cache data at locations determined by CNL [p16v-only?]

#define p16vSRCSel		0x60
 #define p16vSRCSel_AC97		1	// when set, output src48 to ac97; otherwise: 10k2 out
 #define p16vSRCSel_I2S		    4	// when set, output srcmulti to i2s
 #define p16vSRCSel_SPDIF		0x10	
   // when set, output srcmulti to spdif; otherwise: 10k2 out (should be disabled for 10kx ac-3 passthru)

#define p16vSRC48VolL		0x61
#define p16vSRC48VolH		0x62
#define p16vSRCMultiSPDIFL	0x63
#define p16vSRCMultiSPDIFH	0x64
#define p16vSRCMultiI2SL	0x65
#define p16vSRCMultiI2SH	0x66
#define p16vSRCSpdifL		0x67
#define p16vSRCSpdifH		0x68
#define p16vSRCI2SL		    0x69
#define p16vSRCI2SH		    0x6a
#define p16vSRCk2L			0x6b
#define p16vSRCk2H			0x6c

#define p16vSRC48AudEnb		0x6d
#define p16vSRCMultiAudEnb	0x6e
#define p16vAudOutEnb		0x6f
 #define pAudOutEnb_AC97		0x10000

// CDIF interface
/*
#define pCDIFInSel		0x70
#define pCDIFOutSource		0x71
#define pCDIFOutUserData0	0x72
#define pCDIFOutUserData1	0x72
// 0x74,0x75 unused?
// #define pCDIFInSel	0x76
#define pCDIFInStat		0x77
#define pCDIFInUserData0	0x78
#define pCDIFInUserData1	0x79
#define pCDIFInUserData2	0x7a
*/
#endif
