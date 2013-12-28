// Copyright (c) by Jaroslav Kysela <perex@perex.cz>,
//		     Creative Labs, Inc.
//           Eugene Gavrilov, 2008. (http://www.kxproject.com/contact.php)
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

#ifndef KX_EMU_H
#define KX_EMU_H

/************************************************************************************************/
/* EMU1010m HANA FPGA registers									*/
/************************************************************************************************/
#define EMU_HANA_DESTHI		0x00	/* 0000xxx  3 bits Link Destination */
#define EMU_HANA_DESTLO		0x01	/* 00xxxxx  5 bits */
#define EMU_HANA_SRCHI		0x02	/* 0000xxx  3 bits Link Source */
#define EMU_HANA_SRCLO		0x03	/* 00xxxxx  5 bits */
#define EMU_HANA_DOCK_PWR	0x04	/* 000000x  1 bits Audio Dock power */
#define EMU_HANA_DOCK_PWR_ON		0x01 /* Audio Dock power on */
#define EMU_HANA_WCLOCK		0x05	/* 0000xxx  3 bits Word Clock source select  */
					/* Must be written after power on to reset DLL */
					/* One is unable to detect the Audio dock without this */
#define EMU_HANA_WCLOCK_SRC_MASK	0x07
#define EMU_HANA_WCLOCK_INT_48K		0x00
#define EMU_HANA_WCLOCK_INT_44_1K	0x01
#define EMU_HANA_WCLOCK_HANA_SPDIF_IN	0x02
#define EMU_HANA_WCLOCK_HANA_ADAT_IN	0x03
#define EMU_HANA_WCLOCK_SYNC_BNCN	0x04
#define EMU_HANA_WCLOCK_2ND_HANA	0x05
#define EMU_HANA_WCLOCK_SRC_RESERVED	0x06
#define EMU_HANA_WCLOCK_OFF		0x07 /* For testing, forces fallback to DEFCLOCK */
#define EMU_HANA_WCLOCK_MULT_MASK	0x18
#define EMU_HANA_WCLOCK_1X		0x00
#define EMU_HANA_WCLOCK_2X		0x08
#define EMU_HANA_WCLOCK_4X		0x10
#define EMU_HANA_WCLOCK_MULT_RESERVED	0x18

#define EMU_HANA_DEFCLOCK	0x06	/* 000000x  1 bits Default Word Clock  */
#define EMU_HANA_DEFCLOCK_48K		0x00
#define EMU_HANA_DEFCLOCK_44_1K		0x01

#define EMU_HANA_UNMUTE		0x07	/* 000000x  1 bits Mute all audio outputs  */
#define EMU_MUTE			0x00
#define EMU_UNMUTE			0x01

#define EMU_HANA_FPGA_CONFIG	0x08	/* 00000xx  2 bits Config control of FPGAs  */
#define EMU_HANA_FPGA_CONFIG_AUDIODOCK	0x01 /* Set in order to program FPGA on Audio Dock */
#define EMU_HANA_FPGA_CONFIG_HANA	0x02 /* Set in order to program FPGA on Hana */

#define EMU_HANA_IRQ_ENABLE	0x09	/* 000xxxx  4 bits IRQ Enable  */
#define EMU_HANA_IRQ_WCLK_CHANGED	0x01
#define EMU_HANA_IRQ_ADAT		0x02
#define EMU_HANA_IRQ_DOCK		0x04
#define EMU_HANA_IRQ_DOCK_LOST		0x08

#define EMU_HANA_SPDIF_MODE	0x0a	/* 00xxxxx  5 bits SPDIF MODE  */
#define EMU_HANA_SPDIF_MODE_TX_COMSUMER	0x00
#define EMU_HANA_SPDIF_MODE_TX_PRO	0x01
#define EMU_HANA_SPDIF_MODE_TX_NOCOPY	0x02
#define EMU_HANA_SPDIF_MODE_RX_COMSUMER	0x00
#define EMU_HANA_SPDIF_MODE_RX_PRO	0x04
#define EMU_HANA_SPDIF_MODE_RX_NOCOPY	0x08
#define EMU_HANA_SPDIF_MODE_RX_INVALID	0x10

#define EMU_HANA_OPTICAL_TYPE	0x0b	/* 00000xx  2 bits ADAT or SPDIF in/out  */
#define EMU_HANA_OPTICAL_IN_SPDIF	0x00
#define EMU_HANA_OPTICAL_IN_ADAT	0x01
#define EMU_HANA_OPTICAL_OUT_SPDIF	0x00
#define EMU_HANA_OPTICAL_OUT_ADAT	0x02

#if 0
#define EMU_HANA_MIDI_IN		0x0c	/* 000000x  1 bit  Control MIDI  */
#define EMU_HANA_MIDI_IN_FROM_HAMOA	0x00 /* HAMOA MIDI in to Alice 2 MIDI B */
#define EMU_HANA_MIDI_IN_FROM_DOCK	0x01 /* Audio Dock MIDI in to Alice 2 MIDI B */
#endif

// not as in ALSA:
#define EMU_HANA_MIDI_IN		0x0c	/* yy?xx  ? bits Control MIDI  */
#define EMU_HANA_MIDI_INA_FROM_HAMOA	0x01 /* HAMOA MIDI in to Alice 2 MIDI A */
#define EMU_HANA_MIDI_INA_FROM_DOCK1	0x02 /* Audio Dock-1 MIDI in to Alice 2 MIDI A */
#define EMU_HANA_MIDI_INA_FROM_DOCK2	0x03 /* Audio Dock-2 MIDI in to Alice 2 MIDI A */
#define EMU_HANA_MIDI_INB_FROM_HAMOA	0x08 /* HAMOA MIDI in to Alice 2 MIDI B */
#define EMU_HANA_MIDI_INB_FROM_DOCK1	0x10 /* Audio Dock-1 MIDI in to Alice 2 MIDI B */
#define EMU_HANA_MIDI_INB_FROM_DOCK2	0x18 /* Audio Dock-2 MIDI in to Alice 2 MIDI B */

#define EMU_HANA_DOCK_LEDS_1	0x0d	/* 000xxxx  4 bit  Audio Dock LEDs  */
#define EMU_HANA_DOCK_LEDS_1_MIDI1	0x01	/* MIDI 1 LED on */
#define EMU_HANA_DOCK_LEDS_1_MIDI2	0x02	/* MIDI 2 LED on */
#define EMU_HANA_DOCK_LEDS_1_SMPTE_IN	0x04	/* SMPTE IN LED on */
#define EMU_HANA_DOCK_LEDS_1_SMPTE_OUT	0x08	/* SMPTE OUT LED on */

#define EMU_HANA_DOCK_LEDS_2	0x0e	/* 0xxxxxx  6 bit  Audio Dock LEDs  */
#define EMU_HANA_DOCK_LEDS_2_44K	0x01	/* 44.1 kHz LED on */
#define EMU_HANA_DOCK_LEDS_2_48K	0x02	/* 48 kHz LED on */
#define EMU_HANA_DOCK_LEDS_2_96K	0x04	/* 96 kHz LED on */
#define EMU_HANA_DOCK_LEDS_2_192K	0x08	/* 192 kHz LED on */
#define EMU_HANA_DOCK_LEDS_2_LOCK	0x10	/* LOCK LED on */
#define EMU_HANA_DOCK_LEDS_2_EXT	0x20	/* EXT LED on */

#define EMU_HANA_DOCK_LEDS_3	0x0f	/* 0xxxxxx  6 bit  Audio Dock LEDs  */
#define EMU_HANA_DOCK_LEDS_3_CLIP_A	0x01	/* Mic A Clip LED on */
#define EMU_HANA_DOCK_LEDS_3_CLIP_B	0x02	/* Mic B Clip LED on */
#define EMU_HANA_DOCK_LEDS_3_SIGNAL_A	0x04	/* Signal A Clip LED on */
#define EMU_HANA_DOCK_LEDS_3_SIGNAL_B	0x08	/* Signal B Clip LED on */
#define EMU_HANA_DOCK_LEDS_3_MANUAL_CLIP	0x10	/* Manual Clip detection */
#define EMU_HANA_DOCK_LEDS_3_MANUAL_SIGNAL	0x20	/* Manual Signal detection */

#define EMU_HANA_ADC_PADS	0x10	/* 0000xxx  3 bit  Audio Dock ADC 14dB pads */
#define EMU_HANA_DOCK_ADC_PAD1	0x01	/* 14dB Attenuation on Audio Dock ADC 1 */
#define EMU_HANA_DOCK_ADC_PAD2	0x02	/* 14dB Attenuation on Audio Dock ADC 2 */
#define EMU_HANA_DOCK_ADC_PAD3	0x04	/* 14dB Attenuation on Audio Dock ADC 3 */
#define EMU_HANA_0202_ADC_PAD1	0x08	/* 14dB Attenuation on 0202 ADC 1 */

#define EMU_HANA_DOCK_MISC	0x11	/* 0xxxxxx  6 bit  Audio Dock misc bits */
#define EMU_HANA_DOCK_DAC1_MUTE	0x01	/* DAC 1 Mute */
#define EMU_HANA_DOCK_DAC2_MUTE	0x02	/* DAC 2 Mute */
#define EMU_HANA_DOCK_DAC3_MUTE	0x04	/* DAC 3 Mute */
#define EMU_HANA_DOCK_DAC4_MUTE	0x08	/* DAC 4 Mute */
#define EMU_HANA_DOCK_PHONES_192_DAC1	0x00	/* DAC 1 Headphones source at 192kHz */
#define EMU_HANA_DOCK_PHONES_192_DAC2	0x10	/* DAC 2 Headphones source at 192kHz */
#define EMU_HANA_DOCK_PHONES_192_DAC3	0x20	/* DAC 3 Headphones source at 192kHz */
#define EMU_HANA_DOCK_PHONES_192_DAC4	0x30	/* DAC 4 Headphones source at 192kHz */

#define EMU_HANA_MIDI_OUT	0x12	/* 00xxxxx  5 bit  Source for each MIDI out port */
#define EMU_HANA_MIDI_OUT_0202	0x01 /* 0202 MIDI from Alice 2. 0 = A, 1 = B */
#define EMU_HANA_MIDI_OUT_DOCK1	0x02 /* Audio Dock MIDI1 front, from Alice 2. 0 = A, 1 = B */
#define EMU_HANA_MIDI_OUT_DOCK2	0x04 /* Audio Dock MIDI2 rear, from Alice 2. 0 = A, 1 = B */
#define EMU_HANA_MIDI_OUT_SYNC2	0x08 /* Sync card. Not the actual MIDI out jack. 0 = A, 1 = B */
#define EMU_HANA_MIDI_OUT_LOOP	0x10 /* 0 = bits (3:0) normal. 1 = MIDI loopback enabled. */

#define EMU_HANA_DAC_PADS	0x13	/* 00xxxxx  5 bit  DAC 14dB attenuation pads */
#define EMU_HANA_DOCK_DAC_PAD1	0x01	/* 14dB Attenuation on AudioDock DAC 1. Left and Right */
#define EMU_HANA_DOCK_DAC_PAD2	0x02	/* 14dB Attenuation on AudioDock DAC 2. Left and Right */
#define EMU_HANA_DOCK_DAC_PAD3	0x04	/* 14dB Attenuation on AudioDock DAC 3. Left and Right */
#define EMU_HANA_DOCK_DAC_PAD4	0x08	/* 14dB Attenuation on AudioDock DAC 4. Left and Right */
#define EMU_HANA_0202_DAC_PAD1	0x10	/* 14dB Attenuation on 0202 DAC 1. Left and Right */

/* 0x14 - 0x1f Unused R/W registers */
#define EMU_HANA_IRQ_STATUS	0x20	/* 000xxxx  4 bits IRQ Status  */
#if 0  /* Already defined for reg 0x09 IRQ_ENABLE */
#define EMU_HANA_IRQ_WCLK_CHANGED	0x01
#define EMU_HANA_IRQ_ADAT		0x02
#define EMU_HANA_IRQ_DOCK		0x04
#define EMU_HANA_IRQ_DOCK_LOST		0x08
#endif

#define EMU_HANA_OPTION_CARDS	0x21	/* 000xxxx  4 bits Presence of option cards */
#define EMU_HANA_OPTION_HAMOA	0x01	/* HAMOA card present */
#define EMU_HANA_OPTION_SYNC	0x02	/* Sync card present */
#define EMU_HANA_OPTION_DOCK_ONLINE	0x04	/* Audio Dock online and FPGA configured */
#define EMU_HANA_OPTION_DOCK_OFFLINE	0x08	/* Audio Dock online and FPGA not configured */

#define EMU_HANA_ID		0x22	/* 1010101  7 bits ID byte & 0x7f = 0x55 */

#define EMU_HANA_MAJOR_REV	0x23	/* 0000xxx  3 bit  Hana FPGA Major rev */
#define EMU_HANA_MINOR_REV	0x24	/* 0000xxx  3 bit  Hana FPGA Minor rev */

#define EMU_DOCK_MAJOR_REV	0x25	/* 0000xxx  3 bit  Audio Dock FPGA Major rev */
#define EMU_DOCK_MINOR_REV	0x26	/* 0000xxx  3 bit  Audio Dock FPGA Minor rev */

#define EMU_DOCK_BOARD_ID	0x27	/* 00000xx  2 bits Audio Dock ID pins */
#define EMU_DOCK_BOARD_ID0	0x00	/* ID bit 0 */
#define EMU_DOCK_BOARD_ID1	0x03	/* ID bit 1 */

#define EMU_HANA_WC_SPDIF_HI	0x28	/* 0xxxxxx  6 bit  SPDIF IN Word clock, upper 6 bits */
#define EMU_HANA_WC_SPDIF_LO	0x29	/* 0xxxxxx  6 bit  SPDIF IN Word clock, lower 6 bits */

#define EMU_HANA_WC_ADAT_HI	0x2a	/* 0xxxxxx  6 bit  ADAT IN Word clock, upper 6 bits */
#define EMU_HANA_WC_ADAT_LO	0x2b	/* 0xxxxxx  6 bit  ADAT IN Word clock, lower 6 bits */

#define EMU_HANA_WC_BNC_LO	0x2c	/* 0xxxxxx  6 bit  BNC IN Word clock, lower 6 bits */
#define EMU_HANA_WC_BNC_HI	0x2d	/* 0xxxxxx  6 bit  BNC IN Word clock, upper 6 bits */

#define EMU_HANA2_WC_SPDIF_HI	0x2e	/* 0xxxxxx  6 bit  HANA2 SPDIF IN Word clock, upper 6 bits */
#define EMU_HANA2_WC_SPDIF_LO	0x2f	/* 0xxxxxx  6 bit  HANA2 SPDIF IN Word clock, lower 6 bits */
/* 0x30 - 0x3f Unused Read only registers */

/************************************************************************************************/
/* EMU1010m HANA Destinations									*/
/************************************************************************************************/
/* Hana, original 1010,1212,1820 using Alice2
 * Destiniations for SRATEX = 1X rates: 44.1 kHz or 48 kHz
 * 0x00, 0x00-0x0f: 16 EMU32 channels to Alice2
 * 0x01, 0x10-0x1f: 32 Elink channels to Audio Dock
 * 0x01, 0x00: Dock DAC 1 Left
 * 0x01, 0x04: Dock DAC 1 Right
 * 0x01, 0x08: Dock DAC 2 Left
 * 0x01, 0x0c: Dock DAC 2 Right
 * 0x01, 0x10: Dock DAC 3 Left
 * 0x01, 0x12: PHONES Left
 * 0x01, 0x14: Dock DAC 3 Right
 * 0x01, 0x16: PHONES Right
 * 0x01, 0x18: Dock DAC 4 Left
 * 0x01, 0x1a: S/PDIF Left
 * 0x01, 0x1c: Dock DAC 4 Right
 * 0x01, 0x1e: S/PDIF Right
 * 0x02, 0x00: Hana S/PDIF Left
 * 0x02, 0x01: Hana S/PDIF Right
 * 0x03, 0x00: Hanoa DAC Left
 * 0x03, 0x01: Hanoa DAC Right
 * 0x04, 0x00-0x07: Hana ADAT
 * 0x05, 0x00: I2S0 Left to Alice2
 * 0x05, 0x01: I2S0 Right to Alice2
 * 0x06, 0x00: I2S0 Left to Alice2
 * 0x06, 0x01: I2S0 Right to Alice2
 * 0x07, 0x00: I2S0 Left to Alice2
 * 0x07, 0x01: I2S0 Right to Alice2
 *
 * Hana2 never released, but used Tina
 * Not needed.
 *
 * Hana3, rev2 1010,1212,1616 using Tina
 * Destinations for SRATEX = 1X rates: 44.1 kHz or 48 kHz
 * 0x00, 0x00-0x0f: 16 EMU32A channels to Tina
 * 0x01, 0x10-0x1f: 32 EDI channels to Micro Dock
 * 0x01, 0x00: Dock DAC 1 Left
 * 0x01, 0x04: Dock DAC 1 Right
 * 0x01, 0x08: Dock DAC 2 Left
 * 0x01, 0x0c: Dock DAC 2 Right
 * 0x01, 0x10: Dock DAC 3 Left
 * 0x01, 0x12: Dock S/PDIF Left
 * 0x01, 0x14: Dock DAC 3 Right
 * 0x01, 0x16: Dock S/PDIF Right
 * 0x01, 0x18-0x1f: Dock ADAT 0-7
 * 0x02, 0x00: Hana3 S/PDIF Left
 * 0x02, 0x01: Hana3 S/PDIF Right
 * 0x03, 0x00: Hanoa DAC Left
 * 0x03, 0x01: Hanoa DAC Right
 * 0x04, 0x00-0x07: Hana3 ADAT 0-7
 * 0x05, 0x00-0x0f: 16 EMU32B channels to Tina
 * 0x06-0x07: Not used
 *
 * HanaLite, rev1 0404 using Alice2
 * Destiniations for SRATEX = 1X rates: 44.1 kHz or 48 kHz
 * 0x00, 0x00-0x0f: 16 EMU32 channels to Alice2
 * 0x01: Not used
 * 0x02, 0x00: S/PDIF Left
 * 0x02, 0x01: S/PDIF Right
 * 0x03, 0x00: DAC Left
 * 0x03, 0x01: DAC Right
 * 0x04-0x07: Not used
 *
 * HanaLiteLite, rev2 0404 using Alice2
 * Destiniations for SRATEX = 1X rates: 44.1 kHz or 48 kHz
 * 0x00, 0x00-0x0f: 16 EMU32 channels to Alice2
 * 0x01: Not used
 * 0x02, 0x00: S/PDIF Left
 * 0x02, 0x01: S/PDIF Right
 * 0x03, 0x00: DAC Left
 * 0x03, 0x01: DAC Right
 * 0x04-0x07: Not used
 *
 * Mana, Cardbus 1616 using Tina2
 * Destinations for SRATEX = 1X rates: 44.1 kHz or 48 kHz
 * 0x00, 0x00-0x0f: 16 EMU32A channels to Tina2
 * 0x01, 0x10-0x1f: 32 EDI channels to Micro Dock
 * 0x01, 0x00: Dock DAC 1 Left
 * 0x01, 0x04: Dock DAC 1 Right
 * 0x01, 0x08: Dock DAC 2 Left
 * 0x01, 0x0c: Dock DAC 2 Right
 * 0x01, 0x10: Dock DAC 3 Left
 * 0x01, 0x12: Dock S/PDIF Left
 * 0x01, 0x14: Dock DAC 3 Right
 * 0x01, 0x16: Dock S/PDIF Right
 * 0x01, 0x18-0x1f: Dock ADAT 0-7
 * 0x02: Not used
 * 0x03, 0x00: Mana DAC Left
 * 0x03, 0x01: Mana DAC Right
 * 0x04, 0x00-0x0f: 16 EMU32B channels to Tina2
 * 0x05-0x07: Not used
 *
 *
 */
/* 32-bit destinations of signal in the Hana FPGA. Destinations are either
 * physical outputs of Hana, or outputs going to Alice2 (audigy) for capture
 * - 16 x EMU_DST_ALICE2_EMU32_X.
 */
/* EMU32 = 32-bit serial channel between Alice2 (audigy) and Hana (FPGA) */
/* EMU_DST_ALICE2_EMU32_X - data channels from Hana to Alice2 used for capture.
 * Which data is fed into a EMU_DST_ALICE2_EMU32_X channel in Hana depends on
 * setup of mixer control for each destination - see emumixer.c -
 * snd_emu1010_output_enum_ctls[], snd_emu1010_input_enum_ctls[]
 */
#define EMU_DST_ALICE2_EMU32_0	0x000f	/* 16 EMU32 channels to Alice2 +0 to +0xf */
#define EMU_DST_ALICE2_EMU32_1	0x0000	/* 16 EMU32 channels to Alice2 +0 to +0xf */
#define EMU_DST_ALICE2_EMU32_2	0x0001	/* 16 EMU32 channels to Alice2 +0 to +0xf */
#define EMU_DST_ALICE2_EMU32_3	0x0002	/* 16 EMU32 channels to Alice2 +0 to +0xf */
#define EMU_DST_ALICE2_EMU32_4	0x0003	/* 16 EMU32 channels to Alice2 +0 to +0xf */
#define EMU_DST_ALICE2_EMU32_5	0x0004	/* 16 EMU32 channels to Alice2 +0 to +0xf */
#define EMU_DST_ALICE2_EMU32_6	0x0005	/* 16 EMU32 channels to Alice2 +0 to +0xf */
#define EMU_DST_ALICE2_EMU32_7	0x0006	/* 16 EMU32 channels to Alice2 +0 to +0xf */
#define EMU_DST_ALICE2_EMU32_8	0x0007	/* 16 EMU32 channels to Alice2 +0 to +0xf */
#define EMU_DST_ALICE2_EMU32_9	0x0008	/* 16 EMU32 channels to Alice2 +0 to +0xf */
#define EMU_DST_ALICE2_EMU32_A	0x0009	/* 16 EMU32 channels to Alice2 +0 to +0xf */
#define EMU_DST_ALICE2_EMU32_B	0x000a	/* 16 EMU32 channels to Alice2 +0 to +0xf */
#define EMU_DST_ALICE2_EMU32_C	0x000b	/* 16 EMU32 channels to Alice2 +0 to +0xf */
#define EMU_DST_ALICE2_EMU32_D	0x000c	/* 16 EMU32 channels to Alice2 +0 to +0xf */
#define EMU_DST_ALICE2_EMU32_E	0x000d	/* 16 EMU32 channels to Alice2 +0 to +0xf */
#define EMU_DST_ALICE2_EMU32_F	0x000e	/* 16 EMU32 channels to Alice2 +0 to +0xf */
#define EMU_DST_DOCK_DAC1_LEFT1	0x0100	/* Audio Dock DAC1 Left, 1st or 48kHz only */
#define EMU_DST_DOCK_DAC1_LEFT2	0x0101	/* Audio Dock DAC1 Left, 2nd or 96kHz */
#define EMU_DST_DOCK_DAC1_LEFT3	0x0102	/* Audio Dock DAC1 Left, 3rd or 192kHz */
#define EMU_DST_DOCK_DAC1_LEFT4	0x0103	/* Audio Dock DAC1 Left, 4th or 192kHz */
#define EMU_DST_DOCK_DAC1_RIGHT1	0x0104	/* Audio Dock DAC1 Right, 1st or 48kHz only */
#define EMU_DST_DOCK_DAC1_RIGHT2	0x0105	/* Audio Dock DAC1 Right, 2nd or 96kHz */
#define EMU_DST_DOCK_DAC1_RIGHT3	0x0106	/* Audio Dock DAC1 Right, 3rd or 192kHz */
#define EMU_DST_DOCK_DAC1_RIGHT4	0x0107	/* Audio Dock DAC1 Right, 4th or 192kHz */
#define EMU_DST_DOCK_DAC2_LEFT1	0x0108	/* Audio Dock DAC2 Left, 1st or 48kHz only */
#define EMU_DST_DOCK_DAC2_LEFT2	0x0109	/* Audio Dock DAC2 Left, 2nd or 96kHz */
#define EMU_DST_DOCK_DAC2_LEFT3	0x010a	/* Audio Dock DAC2 Left, 3rd or 192kHz */
#define EMU_DST_DOCK_DAC2_LEFT4	0x010b	/* Audio Dock DAC2 Left, 4th or 192kHz */
#define EMU_DST_DOCK_DAC2_RIGHT1	0x010c	/* Audio Dock DAC2 Right, 1st or 48kHz only */
#define EMU_DST_DOCK_DAC2_RIGHT2	0x010d	/* Audio Dock DAC2 Right, 2nd or 96kHz */
#define EMU_DST_DOCK_DAC2_RIGHT3	0x010e	/* Audio Dock DAC2 Right, 3rd or 192kHz */
#define EMU_DST_DOCK_DAC2_RIGHT4	0x010f	/* Audio Dock DAC2 Right, 4th or 192kHz */
#define EMU_DST_DOCK_DAC3_LEFT1	0x0110	/* Audio Dock DAC1 Left, 1st or 48kHz only */
#define EMU_DST_DOCK_DAC3_LEFT2	0x0111	/* Audio Dock DAC1 Left, 2nd or 96kHz */
#define EMU_DST_DOCK_DAC3_LEFT3	0x0112	/* Audio Dock DAC1 Left, 3rd or 192kHz */
#define EMU_DST_DOCK_DAC3_LEFT4	0x0113	/* Audio Dock DAC1 Left, 4th or 192kHz */
#define EMU_DST_DOCK_PHONES_LEFT1	0x0112	/* Audio Dock PHONES Left, 1st or 48kHz only */
#define EMU_DST_DOCK_PHONES_LEFT2	0x0113	/* Audio Dock PHONES Left, 2nd or 96kHz */
#define EMU_DST_DOCK_DAC3_RIGHT1	0x0114	/* Audio Dock DAC1 Right, 1st or 48kHz only */
#define EMU_DST_DOCK_DAC3_RIGHT2	0x0115	/* Audio Dock DAC1 Right, 2nd or 96kHz */
#define EMU_DST_DOCK_DAC3_RIGHT3	0x0116	/* Audio Dock DAC1 Right, 3rd or 192kHz */
#define EMU_DST_DOCK_DAC3_RIGHT4	0x0117	/* Audio Dock DAC1 Right, 4th or 192kHz */
#define EMU_DST_DOCK_PHONES_RIGHT1	0x0116	/* Audio Dock PHONES Right, 1st or 48kHz only */
#define EMU_DST_DOCK_PHONES_RIGHT2	0x0117	/* Audio Dock PHONES Right, 2nd or 96kHz */
#define EMU_DST_DOCK_DAC4_LEFT1	0x0118	/* Audio Dock DAC2 Left, 1st or 48kHz only */
#define EMU_DST_DOCK_DAC4_LEFT2	0x0119	/* Audio Dock DAC2 Left, 2nd or 96kHz */
#define EMU_DST_DOCK_DAC4_LEFT3	0x011a	/* Audio Dock DAC2 Left, 3rd or 192kHz */
#define EMU_DST_DOCK_DAC4_LEFT4	0x011b	/* Audio Dock DAC2 Left, 4th or 192kHz */
#define EMU_DST_DOCK_SPDIF_LEFT1	0x011a	/* Audio Dock SPDIF Left, 1st or 48kHz only */
#define EMU_DST_DOCK_SPDIF_LEFT2	0x011b	/* Audio Dock SPDIF Left, 2nd or 96kHz */
#define EMU_DST_DOCK_DAC4_RIGHT1	0x011c	/* Audio Dock DAC2 Right, 1st or 48kHz only */
#define EMU_DST_DOCK_DAC4_RIGHT2	0x011d	/* Audio Dock DAC2 Right, 2nd or 96kHz */
#define EMU_DST_DOCK_DAC4_RIGHT3	0x011e	/* Audio Dock DAC2 Right, 3rd or 192kHz */
#define EMU_DST_DOCK_DAC4_RIGHT4	0x011f	/* Audio Dock DAC2 Right, 4th or 192kHz */
#define EMU_DST_DOCK_SPDIF_RIGHT1	0x011e	/* Audio Dock SPDIF Right, 1st or 48kHz only */
#define EMU_DST_DOCK_SPDIF_RIGHT2	0x011f	/* Audio Dock SPDIF Right, 2nd or 96kHz */
#define EMU_DST_HANA_SPDIF_LEFT1	0x0200	/* Hana SPDIF Left, 1st or 48kHz only */
#define EMU_DST_HANA_SPDIF_LEFT2	0x0202	/* Hana SPDIF Left, 2nd or 96kHz */
#define EMU_DST_HANA_SPDIF_RIGHT1	0x0201	/* Hana SPDIF Right, 1st or 48kHz only */
#define EMU_DST_HANA_SPDIF_RIGHT2	0x0203	/* Hana SPDIF Right, 2nd or 96kHz */
#define EMU_DST_HAMOA_DAC_LEFT1	0x0300	/* Hamoa DAC Left, 1st or 48kHz only */
#define EMU_DST_HAMOA_DAC_LEFT2	0x0302	/* Hamoa DAC Left, 2nd or 96kHz */
#define EMU_DST_HAMOA_DAC_LEFT3	0x0304	/* Hamoa DAC Left, 3rd or 192kHz */
#define EMU_DST_HAMOA_DAC_LEFT4	0x0306	/* Hamoa DAC Left, 4th or 192kHz */
#define EMU_DST_HAMOA_DAC_RIGHT1	0x0301	/* Hamoa DAC Right, 1st or 48kHz only */
#define EMU_DST_HAMOA_DAC_RIGHT2	0x0303	/* Hamoa DAC Right, 2nd or 96kHz */
#define EMU_DST_HAMOA_DAC_RIGHT3	0x0305	/* Hamoa DAC Right, 3rd or 192kHz */
#define EMU_DST_HAMOA_DAC_RIGHT4	0x0307	/* Hamoa DAC Right, 4th or 192kHz */
#define EMU_DST_HANA_ADAT	0x0400	/* Hana ADAT 8 channel out +0 to +7 */
#define EMU_DST_ALICE_I2S0_LEFT		0x0500	/* Alice2 I2S0 Left */
#define EMU_DST_ALICE_I2S0_RIGHT	0x0501	/* Alice2 I2S0 Right */
#define EMU_DST_ALICE_I2S1_LEFT		0x0600	/* Alice2 I2S1 Left */
#define EMU_DST_ALICE_I2S1_RIGHT	0x0601	/* Alice2 I2S1 Right */
#define EMU_DST_ALICE_I2S2_LEFT		0x0700	/* Alice2 I2S2 Left */
#define EMU_DST_ALICE_I2S2_RIGHT	0x0701	/* Alice2 I2S2 Right */

/* Additional destinations for 1616(M)/Microdock */
/* Microdock S/PDIF OUT Left, 1st or 48kHz only */
#define EMU_DST_MDOCK_SPDIF_LEFT1	0x0112
/* Microdock S/PDIF OUT Left, 2nd or 96kHz */
#define EMU_DST_MDOCK_SPDIF_LEFT2	0x0113
/* Microdock S/PDIF OUT Right, 1st or 48kHz only */
#define EMU_DST_MDOCK_SPDIF_RIGHT1	0x0116
/* Microdock S/PDIF OUT Right, 2nd or 96kHz  */
#define EMU_DST_MDOCK_SPDIF_RIGHT2	0x0117
/* Microdock S/PDIF ADAT 8 channel out +8 to +f */
#define EMU_DST_MDOCK_ADAT		0x0118

/* Headphone jack on 1010 cardbus? 44.1/48kHz only? */
#define EMU_DST_MANA_DAC_LEFT		0x0300
/* Headphone jack on 1010 cardbus? 44.1/48kHz only? */
#define EMU_DST_MANA_DAC_RIGHT		0x0301

/************************************************************************************************/
/* EMU1010m HANA Sources									*/
/************************************************************************************************/
/* Hana, original 1010,1212,1820 using Alice2
 * Sources SRATEX = 1X rates: 44.1 kHz or 48 kHz
 * 0x00,0x00-0x1f: Silence
 * 0x01, 0x10-0x1f: 32 Elink channels from Audio Dock
 * 0x01, 0x00: Dock Mic A
 * 0x01, 0x04: Dock Mic B
 * 0x01, 0x08: Dock ADC 1 Left
 * 0x01, 0x0c: Dock ADC 1 Right
 * 0x01, 0x10: Dock ADC 2 Left
 * 0x01, 0x14: Dock ADC 2 Right
 * 0x01, 0x18: Dock ADC 3 Left
 * 0x01, 0x1c: Dock ADC 3 Right
 * 0x02, 0x00: Hana ADC Left
 * 0x02, 0x01: Hana ADC Right
 * 0x03, 0x00-0x0f: 16 inputs from Alice2 Emu32A output
 * 0x03, 0x10-0x1f: 16 inputs from Alice2 Emu32B output
 * 0x04, 0x00-0x07: Hana ADAT
 * 0x05, 0x00: Hana S/PDIF Left
 * 0x05, 0x01: Hana S/PDIF Right
 * 0x06-0x07: Not used
 *
 * Hana2 never released, but used Tina
 * Not needed.
 *
 * Hana3, rev2 1010,1212,1616 using Tina
 * Sources SRATEX = 1X rates: 44.1 kHz or 48 kHz
 * 0x00,0x00-0x1f: Silence
 * 0x01, 0x10-0x1f: 32 Elink channels from Audio Dock
 * 0x01, 0x00: Dock Mic A
 * 0x01, 0x04: Dock Mic B
 * 0x01, 0x08: Dock ADC 1 Left
 * 0x01, 0x0c: Dock ADC 1 Right
 * 0x01, 0x10: Dock ADC 2 Left
 * 0x01, 0x12: Dock S/PDIF Left
 * 0x01, 0x14: Dock ADC 2 Right
 * 0x01, 0x16: Dock S/PDIF Right
 * 0x01, 0x18-0x1f: Dock ADAT 0-7
 * 0x01, 0x18: Dock ADC 3 Left
 * 0x01, 0x1c: Dock ADC 3 Right
 * 0x02, 0x00: Hanoa ADC Left
 * 0x02, 0x01: Hanoa ADC Right
 * 0x03, 0x00-0x0f: 16 inputs from Tina Emu32A output
 * 0x03, 0x10-0x1f: 16 inputs from Tina Emu32B output
 * 0x04, 0x00-0x07: Hana3 ADAT
 * 0x05, 0x00: Hana3 S/PDIF Left
 * 0x05, 0x01: Hana3 S/PDIF Right
 * 0x06-0x07: Not used
 *
 * HanaLite, rev1 0404 using Alice2
 * Sources SRATEX = 1X rates: 44.1 kHz or 48 kHz
 * 0x00,0x00-0x1f: Silence
 * 0x01: Not used
 * 0x02, 0x00: ADC Left
 * 0x02, 0x01: ADC Right
 * 0x03, 0x00-0x0f: 16 inputs from Alice2 Emu32A output
 * 0x03, 0x10-0x1f: 16 inputs from Alice2 Emu32B output
 * 0x04: Not used
 * 0x05, 0x00: S/PDIF Left
 * 0x05, 0x01: S/PDIF Right
 * 0x06-0x07: Not used
 *
 * HanaLiteLite, rev2 0404 using Alice2
 * Sources SRATEX = 1X rates: 44.1 kHz or 48 kHz
 * 0x00,0x00-0x1f: Silence
 * 0x01: Not used
 * 0x02, 0x00: ADC Left
 * 0x02, 0x01: ADC Right
 * 0x03, 0x00-0x0f: 16 inputs from Alice2 Emu32A output
 * 0x03, 0x10-0x1f: 16 inputs from Alice2 Emu32B output
 * 0x04: Not used
 * 0x05, 0x00: S/PDIF Left
 * 0x05, 0x01: S/PDIF Right
 * 0x06-0x07: Not used
 *
 * Mana, Cardbus 1616 using Tina2
 * Sources SRATEX = 1X rates: 44.1 kHz or 48 kHz
 * 0x00,0x00-0x1f: Silence
 * 0x01, 0x10-0x1f: 32 Elink channels from Audio Dock
 * 0x01, 0x00: Dock Mic A
 * 0x01, 0x04: Dock Mic B
 * 0x01, 0x08: Dock ADC 1 Left
 * 0x01, 0x0c: Dock ADC 1 Right
 * 0x01, 0x10: Dock ADC 2 Left
 * 0x01, 0x12: Dock S/PDIF Left
 * 0x01, 0x14: Dock ADC 2 Right
 * 0x01, 0x16: Dock S/PDIF Right
 * 0x01, 0x18-0x1f: Dock ADAT 0-7
 * 0x01, 0x18: Dock ADC 3 Left
 * 0x01, 0x1c: Dock ADC 3 Right
 * 0x02: Not used
 * 0x03, 0x00-0x0f: 16 inputs from Tina Emu32A output
 * 0x03, 0x10-0x1f: 16 inputs from Tina Emu32B output
 * 0x04-0x07: Not used
 *
 */

/* 32-bit sources of signal in the Hana FPGA. The sources are routed to
 * destinations using mixer control for each destination - see emumixer.c
 * Sources are either physical inputs of FPGA,
 * or outputs from Alice (audigy) - 16 x EMU_SRC_ALICE_EMU32A +
 * 16 x EMU_SRC_ALICE_EMU32B
 */
#define EMU_SRC_SILENCE		0x0000	/* Silence */
#define EMU_SRC_DOCK_MIC_A1	0x0100	/* Audio Dock Mic A, 1st or 48kHz only */
#define EMU_SRC_DOCK_MIC_A2	0x0101	/* Audio Dock Mic A, 2nd or 96kHz */
#define EMU_SRC_DOCK_MIC_A3	0x0102	/* Audio Dock Mic A, 3rd or 192kHz */
#define EMU_SRC_DOCK_MIC_A4	0x0103	/* Audio Dock Mic A, 4th or 192kHz */
#define EMU_SRC_DOCK_MIC_B1	0x0104	/* Audio Dock Mic B, 1st or 48kHz only */
#define EMU_SRC_DOCK_MIC_B2	0x0105	/* Audio Dock Mic B, 2nd or 96kHz */
#define EMU_SRC_DOCK_MIC_B3	0x0106	/* Audio Dock Mic B, 3rd or 192kHz */
#define EMU_SRC_DOCK_MIC_B4	0x0107	/* Audio Dock Mic B, 4th or 192kHz */
#define EMU_SRC_DOCK_ADC1_LEFT1	0x0108	/* Audio Dock ADC1 Left, 1st or 48kHz only */
#define EMU_SRC_DOCK_ADC1_LEFT2	0x0109	/* Audio Dock ADC1 Left, 2nd or 96kHz */
#define EMU_SRC_DOCK_ADC1_LEFT3	0x010a	/* Audio Dock ADC1 Left, 3rd or 192kHz */
#define EMU_SRC_DOCK_ADC1_LEFT4	0x010b	/* Audio Dock ADC1 Left, 4th or 192kHz */
#define EMU_SRC_DOCK_ADC1_RIGHT1	0x010c	/* Audio Dock ADC1 Right, 1st or 48kHz only */
#define EMU_SRC_DOCK_ADC1_RIGHT2	0x010d	/* Audio Dock ADC1 Right, 2nd or 96kHz */
#define EMU_SRC_DOCK_ADC1_RIGHT3	0x010e	/* Audio Dock ADC1 Right, 3rd or 192kHz */
#define EMU_SRC_DOCK_ADC1_RIGHT4	0x010f	/* Audio Dock ADC1 Right, 4th or 192kHz */
#define EMU_SRC_DOCK_ADC2_LEFT1	0x0110	/* Audio Dock ADC2 Left, 1st or 48kHz only */
#define EMU_SRC_DOCK_ADC2_LEFT2	0x0111	/* Audio Dock ADC2 Left, 2nd or 96kHz */
#define EMU_SRC_DOCK_ADC2_LEFT3	0x0112	/* Audio Dock ADC2 Left, 3rd or 192kHz */
#define EMU_SRC_DOCK_ADC2_LEFT4	0x0113	/* Audio Dock ADC2 Left, 4th or 192kHz */
#define EMU_SRC_DOCK_ADC2_RIGHT1	0x0114	/* Audio Dock ADC2 Right, 1st or 48kHz only */
#define EMU_SRC_DOCK_ADC2_RIGHT2	0x0115	/* Audio Dock ADC2 Right, 2nd or 96kHz */
#define EMU_SRC_DOCK_ADC2_RIGHT3	0x0116	/* Audio Dock ADC2 Right, 3rd or 192kHz */
#define EMU_SRC_DOCK_ADC2_RIGHT4	0x0117	/* Audio Dock ADC2 Right, 4th or 192kHz */
#define EMU_SRC_DOCK_ADC3_LEFT1	0x0118	/* Audio Dock ADC3 Left, 1st or 48kHz only */
#define EMU_SRC_DOCK_ADC3_LEFT2	0x0119	/* Audio Dock ADC3 Left, 2nd or 96kHz */
#define EMU_SRC_DOCK_ADC3_LEFT3	0x011a	/* Audio Dock ADC3 Left, 3rd or 192kHz */
#define EMU_SRC_DOCK_ADC3_LEFT4	0x011b	/* Audio Dock ADC3 Left, 4th or 192kHz */
#define EMU_SRC_DOCK_ADC3_RIGHT1	0x011c	/* Audio Dock ADC3 Right, 1st or 48kHz only */
#define EMU_SRC_DOCK_ADC3_RIGHT2	0x011d	/* Audio Dock ADC3 Right, 2nd or 96kHz */
#define EMU_SRC_DOCK_ADC3_RIGHT3	0x011e	/* Audio Dock ADC3 Right, 3rd or 192kHz */
#define EMU_SRC_DOCK_ADC3_RIGHT4	0x011f	/* Audio Dock ADC3 Right, 4th or 192kHz */
#define EMU_SRC_HAMOA_ADC_LEFT1	0x0200	/* Hamoa ADC Left, 1st or 48kHz only */
#define EMU_SRC_HAMOA_ADC_LEFT2	0x0202	/* Hamoa ADC Left, 2nd or 96kHz */
#define EMU_SRC_HAMOA_ADC_LEFT3	0x0204	/* Hamoa ADC Left, 3rd or 192kHz */
#define EMU_SRC_HAMOA_ADC_LEFT4	0x0206	/* Hamoa ADC Left, 4th or 192kHz */
#define EMU_SRC_HAMOA_ADC_RIGHT1	0x0201	/* Hamoa ADC Right, 1st or 48kHz only */
#define EMU_SRC_HAMOA_ADC_RIGHT2	0x0203	/* Hamoa ADC Right, 2nd or 96kHz */
#define EMU_SRC_HAMOA_ADC_RIGHT3	0x0205	/* Hamoa ADC Right, 3rd or 192kHz */
#define EMU_SRC_HAMOA_ADC_RIGHT4	0x0207	/* Hamoa ADC Right, 4th or 192kHz */
#define EMU_SRC_ALICE_EMU32A		0x0300	/* Alice2 EMU32a 16 outputs. +0 to +0xf */
#define EMU_SRC_ALICE_EMU32B		0x0310	/* Alice2 EMU32b 16 outputs. +0 to +0xf */
#define EMU_SRC_HANA_ADAT	0x0400	/* Hana ADAT 8 channel in +0 to +7 */
#define EMU_SRC_HANA_SPDIF_LEFT1	0x0500	/* Hana SPDIF Left, 1st or 48kHz only */
#define EMU_SRC_HANA_SPDIF_LEFT2	0x0502	/* Hana SPDIF Left, 2nd or 96kHz */
#define EMU_SRC_HANA_SPDIF_RIGHT1	0x0501	/* Hana SPDIF Right, 1st or 48kHz only */
#define EMU_SRC_HANA_SPDIF_RIGHT2	0x0503	/* Hana SPDIF Right, 2nd or 96kHz */

/* Additional inputs for 1616(M)/Microdock */
/* Microdock S/PDIF Left, 1st or 48kHz only */
#define EMU_SRC_MDOCK_SPDIF_LEFT1	0x0112
/* Microdock S/PDIF Left, 2nd or 96kHz */
#define EMU_SRC_MDOCK_SPDIF_LEFT2	0x0113
/* Microdock S/PDIF Right, 1st or 48kHz only */
#define EMU_SRC_MDOCK_SPDIF_RIGHT1	0x0116
/* Microdock S/PDIF Right, 2nd or 96kHz */
#define EMU_SRC_MDOCK_SPDIF_RIGHT2	0x0117
/* Microdock ADAT 8 channel in +8 to +f */
#define EMU_SRC_MDOCK_ADAT		0x0118

/* 0x600 and 0x700 no used */

// EM8960 registers [as configured by Creative Labs]
// reg 14-1f:[0] [1f] [0] [1f] [8] [3] [0] [1f] [0] [1f] [1f] [1f] 
// reg 30-3f:[1f] [1f] [1f] [1f] [1f] [1f] [1f] [1f] [1] [7] [1f] [1f] [1f] [1f] [1f] 

#endif
