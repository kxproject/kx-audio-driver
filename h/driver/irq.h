// kX Driver
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


#ifndef IRQ_H_
#define IRQ_H_

#define IRQ_PCIBUSERROR         IPR_PCIERROR
#define IRQ_MIXERBUTTON         (IPR_VOLINCR | IPR_VOLDECR | IPR_MUTE)
#define IRQ_VOICE               (IPR_CHANNELLOOP | IPR_CHANNELNUMBERMASK)
// no longer necessary:
// #define IRQ_RECORD              (IPR_ADCBUFFULL | IPR_ADCBUFHALFFULL | IPR_MICBUFFULL | IPR_MICBUFHALFFULL | IPR_EFXBUFFULL | IPR_EFXBUFHALFFULL | IPR_SPDIFBUFHALFFUL_K2 | IPR_SPDIFBUFFUL_K2)
#define IRQ_MPUOUT              IPR_MIDITRANSBUFEMPTY
#define IRQ_MPUOUT2		        IPR_K2_MIDITRANSBUFEMPTY
#define IRQ_MPUIN               IPR_MIDIRECVBUFEMPTY
#define IRQ_MPUIN2              IPR_K2_MIDIRECVBUFEMPTY
#define IRQ_TIMER               IPR_INTERVALTIMER
#define IRQ_SPDIF               (IPR_GPSPDIFSTATUSCHANGE | IPR_CDROMSTATUSCHANGE)
#define IRQ_DSP                 IPR_FXDSP
#define IRQ_GPIO		        (IPR_GPIO_CHANGE)

#ifdef KX_INTERNAL

  void kx_timer_irq_handler(kx_hw *);
  void kx_dsp_irq_handler(kx_hw *);
  void kx_mute_irq_handler(kx_hw *);
  void kx_volincr_irq_handler(kx_hw *);
  void kx_voldecr_irq_handler(kx_hw *);

  #define KX_IRQ_MPUIN			IRQ_MPUIN
  #define KX_IRQ_MPUIN2			IRQ_MPUIN2

// these are not necessary
//  #define KX_IRQ_MPUOUT			IRQ_MPUOUT
//  #define KX_IRQ_MPUOUT2		    IRQ_MPUOUT2
//  #define KX_IRQ_RECORD			IRQ_RECORD
//  #define KX_IRQ_VOICE			IRQ_VOICE
//  #define KX_IRQ_VOLUMES		    IRQ_MIXERBUTTON
//  #define KX_IRQ_VOLUMES_MUTE		IPR_MUTE
//  #define KX_IRQ_VOLUMES_VOLINCR	IPR_VOLINCR
//  #define KX_IRQ_VOLUMES_VOLDECR	IPR_VOLDECR

  #define KX_IRQ_NONE			0x0
#endif

#endif
