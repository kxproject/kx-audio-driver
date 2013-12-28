// kX Driver
// Copyright (c) Eugene Gavrilov and Max Mikhailov, 2001-2014.
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


#include "kx.h"

int kx_ecard_close(kx_hw *hw)
{
 // do we need some stuff here?..
 return 0;
}

static void ecard_write(kx_hw *, dword);

// Set the gain of the ECARD's CS3310 Trim/gain controller.  The
// trim value consists of a 16bit value which is composed of two
// 8 bit gain/trim values, one for the left channel and one for the
// right channel.  The following table maps from the Gain/Attenuation
// value in decibels into the corresponding bit pattern for a single
// channel.

KX_API(void,kx_ecard_set_adc_gain(kx_hw *hw, dword gain))
{
    int bit;
    hw->ecard.adc_gain = gain;

    // Enable writing to the TRIM registers
    ecard_write(hw, hw->ecard.control_bits & ~EC_TRIM_CSN);

    // Do it again to insure that we meet hold time requirements
    ecard_write(hw, hw->ecard.control_bits & ~EC_TRIM_CSN);

    for(bit=(1L<<15);bit;bit>>=1)  // do not re-program spdif bits?
    {
        dword value = hw->ecard.control_bits & ~(EC_TRIM_CSN|EC_TRIM_SDATA);

        if(gain & bit)
              value |= EC_TRIM_SDATA;

        // Clock the bit
        ecard_write(hw, value);
        ecard_write(hw, value | EC_TRIM_SCLK);
        ecard_write(hw, value);
    }

    ecard_write(hw, hw->ecard.control_bits);
}                                      

KX_API(int,kx_ecard_set_routing(kx_hw *hw, dword routing))
{
 if(kx_ecard_spdif_select(hw,KX_EC_SPDIF0_SHIFT,(routing&0xff)))
  return -1;
 if(kx_ecard_spdif_select(hw,KX_EC_SPDIF1_SHIFT,(routing>>16)&0xffff))
  return -2;

 return 0;
}

// Assigns one of the APS SPDIFs to EMU10K1 digital inputs    
KX_API(int,kx_ecard_spdif_select(kx_hw *hw, dword target, dword source))
{
    if(source&(~0x7))
    {
     debug(DLIB,"!!! ecard spdif select: invalid argument [%x; %x]\n",target,source);
     return -2;
    }

    switch(target) 
    {  
        case KX_EC_SPDIF1_SHIFT:
            if(source==KX_EC_SOURCE_CDROM) 
              return -1; // impossible?
            hw->ecard.spdif1 = source; 
            hw->ecard.control_bits&=(~KX_EC_SPDIF1_MASK);
            break;   
        case KX_EC_SPDIF0_SHIFT:
            hw->ecard.spdif0 = source; 
            hw->ecard.control_bits&=(~KX_EC_SPDIF0_MASK);
            break;   
        default:
            return -3;
    }
    
    hw->ecard.control_bits = EC_RAW_RUN_MODE | 
                             (hw->ecard.spdif1 << KX_EC_SPDIF1_SHIFT) |
                 (hw->ecard.spdif0 << KX_EC_SPDIF0_SHIFT);
     
    ecard_write(hw, hw->ecard.control_bits);
    
    return 0;
} 
 

// Clock bits into the Ecard's control latch.  The Ecard uses a
// control latch will is loaded bit-serially by toggling the Modem control
// lines from function 2 on the E8010.  This function hides these details
// and presents the illusion that we are actually writing to a distinct
// register.

static void ecard_write(kx_hw *hw, dword value)
{
    int count;
    dword data, hcvalue;
    unsigned long flags=0;

    kx_lock_acquire(hw,&hw->hw_lock, &flags);

    hcvalue = inpd(hw->port + HCFG_K1) & ~(HOOKN_BIT|HANDN_BIT|PULSEN_BIT);

    outpd(hw->port + HCFG_K1, hcvalue);

    for(count = 0 ; count < EC_NUM_CONTROL_BITS; count++) 
    {
        // Set up the value 
        data = ((value & 0x1) ? PULSEN_BIT : 0);
        value >>= 1;

        outpd(hw->port + HCFG_K1, hcvalue | data);

        // Clock the shift register
        outpd(hw->port + HCFG_K1, hcvalue | data | HANDN_BIT);
        outpd(hw->port + HCFG_K1, hcvalue | data);
    }

    // Latch the bits
    outpd(hw->port + HCFG_K1, hcvalue | HOOKN_BIT);
    outpd(hw->port + HCFG_K1, hcvalue);

    kx_lock_release(hw,&hw->hw_lock, &flags);
}

int kx_ecard_init(kx_hw *hw)
{
    dword hcvalue;

    debug(DLIB,"ecard init\n");

    // Step 0: Set the codec type in the hardware control register 
    // and enable audio output
    hcvalue = kx_readfn0(hw, HCFG_K1);
    kx_writefn0(hw, HCFG_K1, hcvalue | HCFG_AUDIOENABLE | HCFG_CODECFORMAT_I2S_K1);

    // Step 1: Turn off the led and deassert TRIM_CS
    ecard_write(hw, EC_ADCCAL | EC_LEDN | EC_TRIM_CSN);

    // Step 2: Calibrate the ADC and DAC
    ecard_write(hw, EC_DACCAL | EC_LEDN | EC_TRIM_CSN);

    // Step 3: Wait for awhile
    kx_wcwait(hw, 48000);

    // Step 4: Switch off the DAC and ADC calibration.  Note
    // That ADC_CAL is actually an inverted signal, so we assert
    // it here to stop calibration.
    ecard_write(hw, EC_ADCCAL | EC_LEDN | EC_TRIM_CSN);

    // Step 4: Switch into run mode
    ecard_write(hw, hw->ecard.control_bits);

    // Step 5: Set the analog input gain
    kx_ecard_set_adc_gain(hw, hw->ecard.adc_gain);

        // SPDIF sources are set automatically according to control_bits

    return 0;
}
