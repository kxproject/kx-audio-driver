// kX Driver  / kX Driver Interface
// Copyright (c) Eugene Gavrilov, 2006.
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

#ifndef KX_P17_V_H
#define KX_P17_V_H

// p17v / alice3 hardware registers

#define p17vSRCSel      0x60
 #define p17vSRCSel_AC97		1	// when set, output src48 to ac97; otherwise: 10k2 out
 #define p17vSRCSel_I2S		    4	// when set, output srcmulti to i2s
 #define p17vSRCSel_SPDIF		0x10	

#define p17vK2_AC97L    0x61    // 10k to mixer_ac97
#define p17vK2_AC97R    0x62
#define p17vP17V_AC97L  0x63    // p17v to mixer_ac97
#define p17vP17V_AC97R  0x64
#define p17vSPR_AC97L   0x65    // srp rec -> mixer_ac97
#define p17vSPR_AC97R   0x66
#define p17vAC97En      0x79
#define p17vAC97L       0x80    // ac97 master vol
#define p17vAC97R       0x81

#define p17vK2_SPDIFL    0x69   // 10k to mixer_spdif
#define p17vK2_SPDIFR    0x6a
#define p17vP17V_SPDIFL  0x6b   // p17v to mixer_spdif
#define p17vP17V_SPDIFR  0x6c
#define p17vSPR_SPDIFL   0x6d   // srp rec -> mixer_spdif
#define p17vSPR_SPDIFR   0x6e
#define p17vSPDIFEn      0x7a
#define p17vSPDIFL       0x82   // spdif master vol
#define p17vSPDIFR       0x83

#define p17vK2_I2SL    0x71     // 10k to mixer_i2s
#define p17vK2_I2SR    0x72
#define p17vP17V_I2SL  0x73     // p17v to mixer_i2s
#define p17vP17V_I2SR  0x74
#define p17vSPR_I2SL   0x75     // srp rec -> mixer_i2s
#define p17vSPR_I2SR   0x76
#define p17vI2SEn      0x7b
#define p17vI2SL       0x84     // i2s master vol
#define p17vI2SR       0x85

#define p17vAudOutEnb		0x7c

#define p17vMixerAtt			0x7D	// SRP Mixer Attenuation Select
#define p17vSRPRecordSRP		0x7E	// SRP Record channel source Select
 #define SRP_FX6067 0x0
 #define SRP_FX686f 0x1
 #define SRP_MIXADC97   0x2
 #define SRP_MIXSPDIF   0x3
 #define SRP_MIXI2S     0x4
 #define SRP_SPDIF      0x5
 #define SRP_I2S        0x7
#define p17vSoftResetSRPMixer	0x7F	// SRP and mixer soft reset

#define P17V_I2S_CHANNEL_SWAP_PHASE_INVERSE	0x88	/* I2S out mono channel swap
							 * and phase inverse */
#define P17V_SPDIF_CHANNEL_SWAP_PHASE_INVERSE	0x89	/* SPDIF out mono channel swap
							 * and phase inverse */
/* 8A: Not used */
#define P17V_SRP_P17V_ESR		0x8B	/* SRP_P17V estimated sample rate and rate lock */
#define P17V_SRP_REC_ESR		0x8C	/* SRP_REC estimated sample rate and rate lock */
#define P17V_SRP_BYPASS			0x8D	/* srps channel bypass and srps bypass */
/* 8E - 92: Not used */
#define P17V_I2S_SRC_SEL		0x93	/* I2SIN mode sel */

#define p17vSA			    0x40	// start audio

#define p17vSC          0x48    // start capture
 #define SC_RS0_SPDIF   0x0
 #define SC_RS0_I2S     0x1
 #define SC_RS0_MIXAC97 0x2
 #define SC_RS0_MIXSPDIF    0x3
 #define SC_RS0_MIXI2S      0x4
 #define SC_RS0_FXENG       0x6 // note: no 0x5?..
 #define SC_RS0_AC97        0x7
 #define SC_RS0_SRP         0x8
 #define SC_RS0_SRP441      0x9
 #define SC_RS0_SRP96       0xa
 #define SC_RS0_SRP192      0xb

#define p17vRFBA        0x49	/* Record FIFO base address */
#define p17vRFBS        0x4a	/* Record FIFO buffer size */
#define p17vRFRI        0x4b	/* Record FIFO capture index */

#define p17vRecVolL		0x4c	// rec. path 0..3; 0xff-aligned; mask: 0x1f 
#define p17vRecVolH		0x4d	// rec. path 4..7

// ----------------------------

#define p17vPbChnSelect 0x18    // p17v playback channel select
#define p17vSESlotL     0x19    // sound engine slot select low
#define p17vSESlotH     0x1a    // sound engine slot select high

#define P16V_pSPI       0x3c

#define p17vI2C_ADDR	0x3d	// I2C Address
#define p17vI2C_0		0x3e	// I2C Data
#define p17vI2C_1		0x3f	// I2C Data

/* I2C values */
#define I2C_A_ADC_ADD_MASK	0x000000fe	/*The address is a 7 bit address */
#define I2C_A_ADC_RW_MASK	0x00000001	/*bit mask for R/W */
#define I2C_A_ADC_TRANS_MASK	0x00000010  	/*Bit mask for I2c address DAC value  */
#define I2C_A_ADC_ABORT_MASK	0x00000020	/*Bit mask for I2C transaction abort flag */
#define I2C_A_ADC_LAST_MASK	0x00000040	/*Bit mask for Last word transaction */
#define I2C_A_ADC_BYTE_MASK	0x00000080	/*Bit mask for Byte Mode */

#define I2C_A_ADC_ADD		0x00000034	/*This is the Device address for ADC  */
#define I2C_A_ADC_READ		0x00000001	/*To perform a read operation */
#define I2C_A_ADC_START		0x00000100	/*Start I2C transaction */
#define I2C_A_ADC_ABORT		0x00000200	/*I2C transaction abort */
#define I2C_A_ADC_LAST		0x00000400	/*I2C last transaction */
#define I2C_A_ADC_BYTE		0x00000800	/*I2C one byte mode */

#define I2C_D_ADC_REG_MASK	0xfe000000  	/*ADC address register */ 
#define I2C_D_ADC_DAT_MASK	0x01ff0000  	/*ADC data register */

#define ADC_TIMEOUT		0x00000007	/*ADC Timeout Clock Disable */
#define ADC_IFC_CTRL		0x0000000b	/*ADC Interface Control */
#define ADC_MASTER		0x0000000c	/*ADC Master Mode Control */
#define ADC_POWER		0x0000000d	/*ADC PowerDown Control */
#define ADC_ATTEN_ADCL		0x0000000e	/*ADC Attenuation ADCL */
#define ADC_ATTEN_ADCR		0x0000000f	/*ADC Attenuation ADCR */
#define ADC_ALC_CTRL1		0x00000010	/*ADC ALC Control 1 */
#define ADC_ALC_CTRL2		0x00000011	/*ADC ALC Control 2 */
#define ADC_ALC_CTRL3		0x00000012	/*ADC ALC Control 3 */
#define ADC_NOISE_CTRL		0x00000013	/*ADC Noise Gate Control */
#define ADC_LIMIT_CTRL		0x00000014	/*ADC Limiter Control */
#define ADC_MUX			0x00000015  	/*ADC Mux offset */
#if 0
/* FIXME: Not tested yet. */
#define ADC_GAIN_MASK		0x000000ff	//Mask for ADC Gain
#define ADC_ZERODB		0x000000cf	//Value to set ADC to 0dB
#define ADC_MUTE_MASK		0x000000c0	//Mask for ADC mute
#define ADC_MUTE		0x000000c0	//Value to mute ADC
#define ADC_OSR			0x00000008	//Mask for ADC oversample rate select
#define ADC_TIMEOUT_DISABLE	0x00000008	//Value and mask to disable Timeout clock
#define ADC_HPF_DISABLE		0x00000100	//Value and mask to disable High pass filter
#define ADC_TRANWIN_MASK	0x00000070	//Mask for Length of Transient Window
#endif

#define ADC_MUX_MASK		0x0000000f	//Mask for ADC Mux
#define ADC_MUX_0		0x00000001	//Value to select Unknown at ADC Mux (Not used): AIN1L,AIN1R
#define ADC_MUX_1		0x00000002	//Value to select Unknown at ADC Mux (Not used): AIN2L,AIN2R
#define ADC_MUX_2		0x00000004	//Value to select Mic at ADC Mux, AIN3L,AIN3R
#define ADC_MUX_3		0x00000008	//Value to select Line-In at ADC Mux, AIN4L, AIN4R


#endif
