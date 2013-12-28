// kX Driver  / kX Driver Interface
// Copyright (c) Eugene Gavrilov, 2001-2005.
// All rights reserved

// GPL-safe, since:
//  a. all the comments and text strings were ripped out
//  b. there's a similar version of NetBSD file available under NetBSD license
//  c. register names were already released to the public in patents & ADIP SDK (by Creative)
//  d. certain register information was released under NDA

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


#ifndef KX_8010x_H
#define KX_8010x_H

#define PTR			0x00
#define PTR_CHANNELNUM_MASK	0x0000003f
#define PTR_ADDRESS_MASK	0xffff0000	

#define DATA			0x04		

#define IPR			0x08		
#define IPR_K2_MIDITRANSBUFEMPTY 	0x10000000
#define IPR_K2_MIDIRECVBUFEMPTY 	0x08000000 
#define IPR_SAMPLERATETRACKER	0x01000000	
#define IPR_FXDSP		0x00800000	
#define IPR_FORCEINT		0x00400000	
#define IPR_PCIERROR		0x00200000	
#define IPR_VOLINCR		0x00100000	
#define IPR_VOLDECR		0x00080000	
#define IPR_MUTE		0x00040000	
#define IPR_MICBUFFULL		0x00020000	
#define IPR_MICBUFHALFFULL	0x00010000	
#define IPR_ADCBUFFULL		0x00008000	
#define IPR_ADCBUFHALFFULL	0x00004000	
#define IPR_EFXBUFFULL		0x00002000	
#define IPR_EFXBUFHALFFULL	0x00001000	
#define IPR_GPSPDIFSTATUSCHANGE	0x00000800	
#define IPR_CDROMSTATUSCHANGE	0x00000400	
#define IPR_INTERVALTIMER	0x00000200	
#define IPR_MIDITRANSBUFEMPTY	0x00000100	
#define IPR_MIDIRECVBUFEMPTY	0x00000080	
#define IPR_CHANNELLOOP		0x00000040	
#define IPR_CHANNELNUMBERMASK	0x0000003f	

#define INTE			0x0c		

#define INTE_K2_MIDITXENABLE 	0x00020000
#define INTE_K2_MIDIRXENABLE 	0x00010000 
#define INTE_SAMPLERATETRACKER	0x00002000	
						
#define INTE_FXDSPENABLE	0x00001000	
#define INTE_PCIERRORENABLE	0x00000800	
#define INTE_VOLINCRENABLE	0x00000400	
#define INTE_VOLDECRENABLE	0x00000200	
#define INTE_MUTEENABLE		0x00000100	
#define INTE_MICBUFENABLE	0x00000080	
#define INTE_ADCBUFENABLE	0x00000040	
#define INTE_EFXBUFENABLE	0x00000020	
#define INTE_GPSPDIFENABLE	0x00000010	
#define INTE_CDSPDIFENABLE	0x00000008	
#define INTE_INTERVALTIMERENB	0x00000004	
#define INTE_MIDITXENABLE	0x00000002	
#define INTE_MIDIRXENABLE	0x00000001	

#define WC			0x10		
#define WC_SAMPLECOUNTER_MASK	0x03FFFFC0	
#define WC_SAMPLECOUNTER	0x14060010
#define WC_CURRENTCHANNEL	0x0000003F	

#define HCFG_K1			0x14		
#define HCFG_CODECFORMAT_MASK_K1	0x00070000	
#define HCFG_CODECFORMAT_AC97_K1	0x00000000	
#define HCFG_CODECFORMAT_I2S_K1		0x00010000	
#define HCFG_GPINPUT0_K1		0x00004000	
#define HCFG_GPINPUT1_K1		0x00002000	
#define HCFG_GPOUTPUT_MASK_K1		0x00001c00	
#define HCFG_GPOUT2_K1			0x00001000	
#define HCFG_GPOUT1_K1			0x00000800
#define HCFG_GPOUT0_K1			0x00000400
#define HCFG_JOYENABLE_K1	      	0x00000200	
#define HCFG_PHASETRACKENABLE_K1	0x00000100	
#define HCFG_AC3ENABLE_MASK_K1		0x0x0000e0	
#define HCFG_AC3ENABLE_ZVIDEO_K1	0x00000080	
#define HCFG_AC3ENABLE_CDSPDIF_K1	0x00000040	
#define HCFG_AC3ENABLE_GPSPDIF_K1  	0x00000020      
#define HCFG_AUTOMUTE_K1		0x00000010	
#define HCFG_LOCKSOUNDCACHE	0x00000008	
						
#define HCFG_LOCKTANKCACHE_MASK	0x00000004	
						
#define HCFG_MUTEBUTTONENABLE	0x00000002	
						
#define HCFG_AUDIOENABLE	0x00000001	

#define HCFG_K2			0x18
#define HCFG_K2_GPINPUT_MASK	0x0000ff00
#define HCFG_K2_GPOUTPUT_MASK	0x000000ff
#define HCFG_K2_DISABLE_ANALOG	0x40
#define HCFG_K2_ENABLE_DIGITAL	0x04
 #define HCFG_K2_LINE_MIC_SELECT   HCFG_K2_ENABLE_DIGITAL   // a2zsnb switch: line/mic switch
 #define HCFG_K2_ENABLE_SPDIF   0x10
#define HCFG_K2_A4_LINE_DOO     0x08    // a4 value switch: line in / doo
#define HCFG_K2_BYPASS		0x02			
#define HCFG_K2_UNKNOWN_20	0x20
#define HCFG_K2_UNKNOWN_80	0x80			
#define HCFG_K2_AC97_DISCONNECT	0x80
#define HCFG_K2_MULTIPURPOSE_JACK	0x2000		

/* Audigy 2 Value:
 GPIO:
 0x400: Front analog
 0x1000: Read analog
 0x2000: Center/LFE

 0x60: analog front enabled ?

 def. value: 0x61
*/
							
#define HCFG_K2_DIGITAL_JACK	0x1000			
#define HCFG_K2_FRONT_JACK	0x4000
#define HCFG_K2_REAR_JACK	0x8000
#define HCFG_K2_PHONES_JACK	0x0100		

// Audigy2 ZS Notebook:

#define HCFG_ZSNB_OPT_IN	0x0100     // set for line-in or optical-in
#define HCFG_ZSNB_LINE_IN	0x0300     // set for optical-in
#define HCFG_ZSNB_OPT_OUT	0x0400     // set for optical-out or headphones-out
#define HCFG_ZSNB_PHONES	0x0C00     // set for headphones-out
#define HCFG_ZSNB_SPEAKERS	0x1000

#define MUDATA_K1		0x18		

#define MUCMD_K1		0x19		
#define MUCMD_RESET		0xff		
#define MUCMD_ENTERUARTMODE	0x3f		
						
#define MUSTAT_K1		MUCMD_K1	
#define MUSTAT_IRDYN		0x80		
#define MUSTAT_ORDYN		0x40		

#define TIMER			0x1a		
						
#define TIMER_RATE_MASK		0x000003ff	
						
#define AC97DATA		0x1c		

#define AC97ADDRESS		0x1e		
#define AC97ADDRESS_READY	0x80		
#define AC97ADDRESS_ADDRESS	0x7f		

#define CPF			0x00		// Current Pitch and Fraction
#define CPF_CURRENTPITCH_MASK	0xffff0000	
#define CPF_CURRENTPITCH	0x10100000
#define CPF_STEREO_MASK		0x00008000	
#define CPF_STOP_MASK		0x00004000	
#define CPF_FRACADDRESS_MASK	0x00003fff	

#define PTAB			0x01		// Pitch Target and Sends A and B Targets 
#define PTAB_PITCHTARGET_MASK	0xffff0000	
#define PTAB_PITCHTARGET	0x10100001
#define PTAB_FXSENDAMOUNT_A_MASK 0x0000ff00	
#define PTAB_FXSENDAMOUNT_A	0x08080001
#define PTAB_FXSENDAMOUNT_B_MASK 0x000000ff	
#define PTAB_FXSENDAMOUNT_B	0x08000001
#define PTAB_SENDAB		0x10000001

#define CVCF			0x02		// Current Volume and Current Filter Cutoff 
#define CVCF_CURRENTVOL_MASK	0xffff0000	
#define CVCF_CURRENTVOL		0x10100002
#define CVCF_CURRENTFILTER_MASK	0x0000ffff	
#define CVCF_CURRENTFILTER	0x10000002

#define VTFT			0x03		// Volume Target and Filter Cutoff Target 
#define VTFT_VOLUMETARGET_MASK	0xffff0000	
#define VTFT_FILTERTARGET_MASK	0x0000ffff	
#define VTFT_VOLUME		0x10100003

#define Z2			0x04		// Filter Delay Memory 2 
#define Z1			0x05		// Filter Delay Memory 1

#define SCSA			0x06		// Send C Target and Start Address 
#define SCSA_FXSENDAMOUNT_C_MASK 0xff000000	
#define SCSA_FXSENDAMOUNT_C	0x08180006
#define SCSA_LOOPSTARTADDR_MASK	0x00ffffff	
#define SCSA_LOOPSTARTADDR	0x18000006

#define SDL			0x07		// Send D Target and Loop Address 
#define SDL_FXSENDAMOUNT_D_MASK	0xff000000	
#define SDL_FXSENDAMOUNT_D	0x08180007
#define SDL_LOOPENDADDR_MASK	0x00ffffff	
#define SDL_LOOPENDADDR		0x18000007

#define QKBCA			0x08		// Filter Q, ROM, Byte, Current Address 
#define QKBCA_RESONANCEMASK	0xf0000000	
#define QKBCA_INTERPROMMASK	0x0e000000	
#define QKBCA_INTERPROM_0	0x00000000	
#define QKBCA_INTERPROM_1	0x02000000	
#define QKBCA_INTERPROM_2	0x04000000	
#define QKBCA_INTERPROM_3	0x06000000	
#define QKBCA_INTERPROM_4	0x08000000	
#define QKBCA_INTERPROM_5	0x0a000000	
#define QKBCA_INTERPROM_6	0x0c000000	
#define QKBCA_INTERPROM_7	0x0e000000	
#define QKBCA_8BITSELECT	0x01000000	
#define QKBCA_CURRADDR_MASK	0x00ffffff	
#define QKBCA_CURRADDR		0x18000008
#define QKBCA_INTERPROM		0x04190008
#define QKBCA_RESONANCEQ	0x041c0008

#define emuCCR			0x09		// Cache Control
#undef CCR
#define CCR_CACHEINVALIDSIZE	0x07190009
#define CCR_CACHEINVALIDSIZE_MASK	0xfe000000	
#define CCR_CACHELOOPFLAG	0x01000000	
#define CCR_INTERLEAVEDSAMPLES	0x00800000	
#define CCR_WORDSIZEDSAMPLES	0x00400000	
#define CCR_READADDRESS		0x06100009
#define CCR_READADDRESS_MASK	0x003f0000	
#define CCR_LOOPINVALSIZE	0x0000fe00	
#define CCR_LOOPFLAG		0x00000100	
#define CCR_CACHELOOPADDRHI	0x000000ff	

#define CLP			0x0a		// Cache Loop
						
#define CLP_CACHELOOPADDR	0x0000ffff	

#define FXRT_K1			0x0b		
						
#define FXRT_CHANNELA		0x000f0000	
#define FXRT_CHANNELB		0x00f00000	
#define FXRT_CHANNELC		0x0f000000	
#define FXRT_CHANNELD		0xf0000000	

#define MAPA			0x0c		// Cache Map A

#define MAPB			0x0d		// Cache Map B

#define MAP_PTE_MASK		0xffffe000	
#define MAP_PTI_MASK		0x00001fff	

#define VEV			0x10		// Volume Envelope Value
#define VEV_MASK		0x0000ffff	

#define VEHA 			0x11		// Volume Envelope Hold and Attack Times 
#define VEHA_PHASE0		0x00008000	
#define VEHA_HOLDTIME_MASK	0x00007f00	
#define VEHA_ATTACKTIME_MASK	0x0000007f	

#define DCYSUSV 		0x12		
#define DCYSUSV_PHASE1_MASK	0x00008000	
#define DCYSUSV_SUSTAINLEVEL_MASK 0x00007f00	
#define DCYSUSV_CHANNELENABLE_MASK 0x00000080	
						
#define DCYSUSV_DECAYTIME_MASK	0x0000007f	
						
#define LFOVAL1 		0x13		
#define LFOVAL_MASK		0x0000ffff	

#define ENVVAL			0x14		
#define ENVVAL_MASK		0x0000ffff	

#define ATKHLDM			0x15		
#define ATKHLDM_PHASE0		0x00008000	
#define ATKHLDM_HOLDTIME	0x00007f00	
#define ATKHLDM_ATTACKTIME	0x0000007f	

#define DCYSUSM			0x16		
#define DCYSUSM_PHASE1_MASK	0x00008000	
#define DCYSUSM_SUSTAINLEVEL_MASK 0x00007f00	
#define DCYSUSM_DECAYTIME_MASK	0x0000007f	

#define LFOVAL2 		0x17		
#define LFOVAL2_MASK		0x0000ffff	

#define IP			0x18		
#define IP_MASK			0x0000ffff	
						
#define IP_UNITY		0x0000e000	

#define IFATN			0x19		
#define IFATN_FILTERCUTOFF_MASK	0x0000ff00	
						
#define IFATN_FILTERCUTOFF	0x08080019
#define IFATN_ATTENUATION_MASK	0x000000ff	
#define IFATN_ATTENUATION	0x08000019

#define PEFE			0x1a		
#define PEFE_PITCHAMOUNT_MASK	0x0000ff00	
						
#define PEFE_PITCHAMOUNT	0x0808001a
#define PEFE_FILTERAMOUNT_MASK	0x000000ff	
						
#define PEFE_FILTERAMOUNT	0x0800001a
#define FMMOD			0x1b		
#define FMMOD_MODVIBRATO	0x0000ff00	
						
#define FMMOD_MOFILTER		0x000000ff	
						
#define TREMFRQ 		0x1c		
#define TREMFRQ_DEPTH		0x0000ff00	
						
#define TREMFRQ_FREQ		0x000000ff	

#define FM2FRQ2 		0x1d		
#define FM2FRQ2_DEPTH		0x0000ff00	
						
#define FM2FRQ2_FREQUENCY	0x000000ff	

#define TEMPENV 		0x1e		
#define TEMPENV_MASK		0x0000ffff	
						
#define CD0			0x20		
#define CD1			0x21		
#define CD2			0x22		
#define CD3			0x23		
#define CD4			0x24		
#define CD5			0x25		
#define CD6			0x26		
#define CD7			0x27		
#define CD8			0x28		
#define CD9			0x29		
#define CDA			0x2a		
#define CDB			0x2b		
#define CDC			0x2c		
#define CDD			0x2d		
#define CDE			0x2e		
#define CDF			0x2f		

#define PTBA			0x40		// Page Table Base Address 
#define PTB_MASK		0xfffff000	

#define TRBA			0x41		// Tank Ram Base Address 
#define TRBA_MASK		0xfffff000	

#define ADCSR			0x42		// ADC Sample Rate and Control 
#define ADCSR_RCHANENABLE_K1	0x00000010	
#define ADCSR_LCHANENABLE_K1	0x00000008	
						
#define ADCSR_RCHANENABLE_K2	0x00000020	
#define ADCSR_LCHANENABLE_K2	0x00000010	
						
#define ADCSR_SAMPLERATE_MASK_K1 0x00000007	
#define ADCSR_SAMPLERATE_MASK_K2 0x0000000f 	
#define ADCSR_SAMPLERATE_48	0x00000000	
#define ADCSR_SAMPLERATE_44	0x00000001	
#define ADCSR_SAMPLERATE_32	0x00000002	
#define ADCSR_SAMPLERATE_24	0x00000003	
#define ADCSR_SAMPLERATE_22	0x00000004	
#define ADCSR_SAMPLERATE_16	0x00000005	
#define ADCSR_SAMPLERATE_11_K1	0x00000006	
#define ADCSR_SAMPLERATE_8_K1	0x00000007	
#define ADCSR_SAMPLERATE_12_K2	0x00000006	
#define ADCSR_SAMPLERATE_11_K2	0x00000007	
#define ADCSR_SAMPLERATE_8_K2	0x00000008	

#define FXWC_K1			0x43		
						
#define TRBS			0x44		// Tank Ram Buffer Size 
#define TRBS_MASK		0x00000007	
#define TRBS_BUFFSIZE_16K	0x00000000
#define TRBS_BUFFSIZE_32K	0x00000001
#define TRBS_BUFFSIZE_64K	0x00000002
#define TRBS_BUFFSIZE_128K	0x00000003
#define TRBS_BUFFSIZE_256K	0x00000004
#define TRBS_BUFFSIZE_512K	0x00000005
#define TRBS_BUFFSIZE_1024K	0x00000006
#define TRBS_BUFFSIZE_2048K	0x00000007

#define MBA			0x45		
#define MBA_MASK		0xfffff000	

#define ADCBA			0x46		
#define ADCBA_MASK		0xfffff000	

#define FXBA			0x47		
#define FXBA_MASK		0xfffff000	

#define MBS			0x49		

#define ADCBS			0x4a		

#define FXBS			0x4b		

#define ADCBS_BUFSIZE_NONE	0x00000000
#define ADCBS_BUFSIZE_384	0x00000001
#define ADCBS_BUFSIZE_448	0x00000002
#define ADCBS_BUFSIZE_512	0x00000003
#define ADCBS_BUFSIZE_640	0x00000004
#define ADCBS_BUFSIZE_768	0x00000005
#define ADCBS_BUFSIZE_896	0x00000006
#define ADCBS_BUFSIZE_1024	0x00000007
#define ADCBS_BUFSIZE_1280	0x00000008
#define ADCBS_BUFSIZE_1536	0x00000009
#define ADCBS_BUFSIZE_1792	0x0000000a
#define ADCBS_BUFSIZE_2048	0x0000000b
#define ADCBS_BUFSIZE_2560	0x0000000c
#define ADCBS_BUFSIZE_3072	0x0000000d
#define ADCBS_BUFSIZE_3584	0x0000000e
#define ADCBS_BUFSIZE_4096	0x0000000f
#define ADCBS_BUFSIZE_5120	0x00000010
#define ADCBS_BUFSIZE_6144	0x00000011
#define ADCBS_BUFSIZE_7168	0x00000012
#define ADCBS_BUFSIZE_8192	0x00000013
#define ADCBS_BUFSIZE_10240	0x00000014
#define ADCBS_BUFSIZE_12288	0x00000015
#define ADCBS_BUFSIZE_14366	0x00000016
#define ADCBS_BUFSIZE_16384	0x00000017
#define ADCBS_BUFSIZE_20480	0x00000018
#define ADCBS_BUFSIZE_24576	0x00000019
#define ADCBS_BUFSIZE_28672	0x0000001a
#define ADCBS_BUFSIZE_32768	0x0000001b
#define ADCBS_BUFSIZE_40960	0x0000001c
#define ADCBS_BUFSIZE_49152	0x0000001d
#define ADCBS_BUFSIZE_57344	0x0000001e
#define ADCBS_BUFSIZE_65536	0x0000001f

#define CDCS			0x50		

#define GPSCS			0x51		

#define DBG_10K1		0x52		

#define DBG_10K1_ZC                  0x80000000      
#define DBG_10K1_SATURATION_OCCURED  0x02000000      
#define DBG_10K1_SATURATION_ADDR     0x01ff0000      
#define DBG_10K1_SINGLE_STEP         0x00008000      
#define DBG_10K1_STEP                0x00004000      
#define DBG_10K1_CONDITION_CODE      0x00003e00      
#define DBG_10K1_SINGLE_STEP_ADDR    0x000001ff      

#define DBG_10K2		0x53		
#define DBG_10K2_SINGLE_STEP		0x00020000
#define DBG_10K2_STEP			0x00010000
#define DBG_10K2_ZC			0x40000000
#define DBG_10K2_STEP_ADDR		0x000003ff
#define DBG_10K2_CONDITION_CODE		0x0000f800
#define DBG_10K2_SATURATION_OCCURED	0x20000000
#define DBG_10K2_SATURATION_ADDR	0x0ffc0000

#define SCS0			0x54		// SPDIF Output 0 Channel Status 

#define SCS1			0x55		// SPDIF Output 1 Channel Status 

#define SCS2			0x56		// SPDIF Output 2 Channel Status 

#define SCS_CLKACCYMASK	0x30000000				
#define SCS_CLKACCY_1000PPM	0x00000000	
#define SCS_CLKACCY_50PPM	0x10000000	
#define SCS_CLKACCY_VARIABLE	0x20000000	
#define SCS_SAMPLERATEMASK	0x0f000000	
#define SCS_SAMPLERATE_44	0x00000000	
#define SCS_SAMPLERATE_48	0x02000000	
#define SCS_SAMPLERATE_32	0x03000000	
#define SCS_CHANNELNUMMASK	0x00f00000	
#define SCS_CHANNELNUM_UNSPEC	0x00000000	
#define SCS_CHANNELNUM_LEFT	0x00100000	
#define SCS_CHANNELNUM_RIGHT	0x00200000	
#define SCS_SOURCENUMMASK	0x000f0000	
#define SCS_SOURCENUM_UNSPEC	0x00000000	
#define SCS_GENERATIONSTATUS	0x00008000	
#define SCS_CATEGORYCODEMASK	0x00007f00	
#define SCS_MODEMASK		0x000000c0	
#define SCS_EMPHASISMASK	0x00000038	
#define SCS_EMPHASIS_NONE	0x00000000	
#define SCS_EMPHASIS_50_15	0x00000008	
#define SCS_COPYRIGHT		0x00000004	
#define SCS_NOTAUDIODATA	0x00000002	
#define SCS_PROFESSIONAL	0x00000001	

#define CLIEL			0x58		

#define CLIEH			0x59		

#define CLIPL			0x5a		

#define CLIPH			0x5b		

#define SOLL			0x5c		

#define SOLH			0x5d		

#define SOC			0x5e		// S/PDIF Output Control [bypass]
#define SP0_BYPASS_MASK		0x00000003	
#define SP1_BYPASS_MASK		0x0000000c	
#define SPDIF_FORMAT		0x000f00	

#define PCB			0x5f		// PCB Revision / AC97 SLOT
#define PCB_CNTR		0x10		
#define PCB_LFE			0x20		
#define PCB_10K2		0x3

#define CDSRCS			0x60		

#define GPSRCS			0x61		

#define ZVSRCS			0x62		
						
#define SRCS_VALIDAUDIO		0x04000000	
#define SRCS_SPDIFLOCKED	0x02000000	
#define SRCS_RATELOCKED		0x01000000	
#define SRCS_ESTSAMPLERATE	0x0007ffff	

#define MIDX_K1               	0x63            
#define ADCIDX_K2		0x63
#define MIDX_MASK             	0x0000ffff                              
#define MIDX_IDX		0x10000063

#define MIDX_K2               	0x64

#define ADCIDX_K1		0x64		
#define ADCIDX_MASK		0x0000ffff				
#define ADCIDX_IDX		0x10000064

#define FXIDX			0x65		
#define FXIDX_MASK		0x0000ffff				
#define FXIDX_IDX		0x10000065

#define MPUDATA_10K2		0x70
#define MPUSTAT_10K2		0x71
#define MPUCMD_10K2		MPUSTAT_10K2

#define MPUDATA2_10K2		0x72
#define MPUSTAT2_10K2		0x73
#define MPUCMD2_10K2		MPUSTAT_10K2

#define FXWCL			0x74		// Effects Engine Output Write Low Channels (0 - 31) 
#define FXWCH			0x75		// Effects Engine Output Write High Channels (32 - 63) 

#define EHC			0x76

#define EHC_ASYNC_BYPASS	0x80000000
#define EHC_P16V_PB_RATE	0x30000	
#define EHC_SRC48_BYPASS	0x0
#define EHC_SRC48_192		0x2000
#define EHC_SRC48_96		0x4000
#define EHC_SRC48_44		0x8000
#define EHC_SRC48_MUTED		0xC000
#define EHC_SRC48_MASK		0xE000

#define EHC_SPDIF_44		0x80
#define EHC_SPDIF_96		0x40
#define EHC_P17V_SPDIF_192  0x20    // p17v only?..
#define EHC_SPDIF_48		0x00
#define EHC_SRCMULTI_SPDIF_BYPASS	0x0
#define EHC_SRCMULTI_SPDIF_MASK		0xe0
#define EHC_SRCMULTI_SPDIF_MUTED	0xc0

#define EHC_SRCMULTI_I2S_BYPASS		0x0
#define EHC_SRCMULTI_I2S_192		0x200
#define EHC_SRCMULTI_I2S_96		    0x400
#define EHC_SRCMULTI_I2S_44		    0x800
#define EHC_SRCMULTI_I2S_MUTED		0xc00
#define EHC_SRCMULTI_I2S_MASK		0xe00

// additional bits for p17v:
#define EHC_P17V_TVM        0x1     // tank virtual memory mode [p17v only]
#define EHC_P17V_SEL0       0x30000 // 00: 48, 01: 44.1, 10: 96, 11: 192
#define EHC_P17V_SEL1       0xc0000
#define EHC_P17V_SEL2       0x300000
#define EHC_P17V_SEL3       0xc00000

#define FXRT2_K2		0x7c		
#define FXRT1_K2		0x7e		

#define FXAMOUNT_K2		0x7d		

#define FXSENDAMOUNT_H		0x0818007d
#define FXSENDAMOUNT_G		0x0810007d
#define FXSENDAMOUNT_F		0x0808007d
#define FXSENDAMOUNT_E		0x0800007d

#define E10K1_GPR_BASE		0x100		    	
#define E10K2_GPR_BASE		0x400

#define TANKMEMDATAREGBASE	0x200		
#define TANKMEMDATAREG_MASK	0x000fffff	

#define TANKMEMADDRREGBASE	0x300		
#define TANKMEMADDRREG_ADDR_MASK_K1 0x000fffff	

#define TRAM_CLEAR_K1	0x00800000	
#define TRAM_ALIGN_K1	0x00400000	
#define TRAM_WRITE_K1	0x00200000	
#define TRAM_READ_K1	0x00100000	

#define HCFG_I2SASRC0_K2	0x0010
#define HCFG_I2SASRC1_K2	0x0020
#define HCFG_I2SASRC2_K2	0x0040
#define HCFG_I2SPT0_K2		0x0100
#define HCFG_I2SPT1_K2		0x0200
#define HCFG_I2SPT2_K2		0x0400
#define HCFG_SLOWRAMPRATE_K2	0x1000 	
#define HCFG_CLOCKSYNTH_K2	0x2000 	
#define HCFG_VBIT_AUTOMUTE_K2	0x4000  
#define HCFG_AUTOMUTE_K2	0x8000 
#define HCFG_CODECFORMAT_K2	0x10000 
#define HCFG_CODECFORMAT2_K2	0x20000 
#define HCFG_XMM_K2		0x40000 
#define HCFG_44K_K2		0x80000 
#define HCFG_PBE_K2		0x100000

#define IPR_P16V_ENABLE		0x80000000 
#define IPR_WATERMARK_REACHED	0x40000000
#define IPR_GPIO_CHANGE		0x20000000
#define IPR_SPDIFBUFHALFFUL_K2	0x04000000
#define IPR_SPDIFBUFFUL_K2	0x02000000

// INTE
#define INTE_SPDIF_BUFFUL_ENABLE_K2 		0x00008000 
#define INTE_SPDIF_HALFBUFFULL_ENABLE_K2	0x00004000 
#define INTE_K2_GPIO_ENABLE 			0x00040000


#define TBLSZ	0x43	// effects tank internal table size

#define HWM	0x48	

#define CSBA	0x4c	
#define CSDC	0x4d
#define CSFE	0x4e
#define CSHG	0x4f

#define GP2SCS_10K2		0x52	

#define SPSX_WL		1	

#define SPRI	0x6a	
#define SPRA	0x6b	
#define SPRC	0x6c	

#define SRT3	0x77	
#define SRT4	0x78
#define SRT5	0x79

#define SRT_ESTSAMPLERATE	0x1fffff	
#define SRT_RATELOCKED		0x1000000

#define TTB			0x6e		// Tank Table Base 
#define TDOF			0x6f		// Tank Delay Offset
#define TTDA			0x7a		// Tank Table DMA Address 
#define TTDD			0x7b		// Tank Table DMA Data 

#define HLIEL	0x66	// Half Loop Interrupt Enable (low) 
#define HLIEH	0x67	// Half Loop Interrupt Enable (high) 
#define HLIPL	0x68	// Half Loop Interrupt Pending (low) 
#define HLIPH	0x69	// Half Loop Interrupt Pending (high) 

#endif
