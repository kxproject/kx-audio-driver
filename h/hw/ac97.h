// kX Driver / kX Driver Interface
// Copyright (c) Eugene Gavrilov, 2001-2009.
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

#ifndef AC97_H_
#define AC97_H_

// reference: AC97 Spec [Rev. 2.1] (Intel)

#define AC97_REG_RESET              0x00

#define AC97_REG_MASTER_VOL         0x02    // line control
#define AC97_REG_SURROUND_DAC_VOL   0x04    // in Live! 5.1: Center/Subwoofer -> routing somewhere?
#define AC97_REG_MONO_VOL           0x06    // play control; TAD OUT
// #define AC97_REG_MASTER_TONE        0x08    // bass(4)+treble(4)
#define AC97_REG_PC_BEEP_VOL        0x0A
#define AC97_REG_PHONE_VOL          0x0C	// TAD IN
#define AC97_REG_MIC_VOL            0x0E
#define AC97_REG_LINE_VOL           0x10
#define AC97_REG_CD_VOL             0x12
#define AC97_REG_VIDEO_VOL          0x14	// FIXME: is changed in crea'beta7 on master change
#define AC97_REG_AUX_VOL            0x16
#define AC97_REG_PCM_VOL            0x18

#define AC97_REG_REC_SELECT         0x1A
#define AC97_REG_REC_GAIN           0x1C	// analog gain
#define AC97_REG_REC_GAIN_MIC       0x1E
#define AC97_REG_GENERAL            0x20
#define AC97_REG_3D_CTRL            0x22
// #define AC97_REG_MODEM_RATE         0x24   // Reserved in 2.1?
#define AC97_REG_PWR_DWN_CTRL       0x26
#define AC97_REG_PWR_DWN_STAT       0x26

#define AC97_REG_EXTENDED_ID		0x28	/* Extended Audio ID */
#define AC97_REG_EXTENDED_STATUS	0x2a	/* Extended Audio Status */
#define AC97_PCM_FRONT_DAC_RATE 	0x2c	/* PCM Front DAC Rate */
//#define AC97_REG_PCM_SURR_DAC_RATE	0x2e	/* PCM Surround DAC Rate */
//#define AC97_REG_PCM_LFE_DAC_RATE	0x30	/* PCM LFE DAC Rate */
//#define AC97_REG_PCM_LR_DAC_RATE	0x32	/* PCM LR DAC Rate */
//#define AC97_REG_PCM_MIC_ADC_RATE	0x34	/* PCM MIC ADC Rate */
#define AC97_REG_CENTER_LFE_MASTER	0x36	/* Center + LFE Master Volume */
#define AC97_REG_SURROUND_MASTER	0x38	/* Surround (Rear) or Center/Subwoofer Master Volume */

#define AC97_REG_MULTICHANNEL_SELECT	0x74	// Vendor-specific: ST AC9708
#define AC97_SIGMATEL_ANALOG	0x6c	/* Analog Special */
#define AC97_SIGMATEL_DAC2INVERT 0x6e
#define AC97_SIGMATEL_BIAS1	0x70
#define AC97_SIGMATEL_BIAS2	0x72
#define AC97_SIGMATEL_CIC1	0x76
#define AC97_SIGMATEL_CIC2	0x78

#define AC97_REG_VENDOR_ID1         0x7C
#define AC97_REG_VENDOR_ID2         0x7E

enum ac97_recording_source {
	AC97_REC_MIC=0,
	AC97_REC_CD,
	AC97_REC_VIDEO,	// 10k1/10k2: unknown usage...
	AC97_REC_AUX,
	AC97_REC_LINE,
	AC97_REC_STEREO, 
	AC97_REC_MONO,	      
	AC97_REC_PHONE	      
};

#endif
