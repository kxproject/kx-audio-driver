// kX Driver
// Copyright (c) Eugene Gavrilov, 2001-2014.
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
#include "hw/emu.h"

#include <stdarg.h>

#define TIMEOUT 16384

KX_API(void,kx_disable_analog(kx_hw *hw,int disable));

KX_API(void, kx_writefn0(kx_hw *hw, dword reg, dword data))
{
    unsigned long flags=0;

    if(reg & 0xff000000) 
    {
        dword mask;
        byte size, offset;

        size = (byte) ((reg >> 24) & 0x3f);
        offset = (byte) ((reg >> 16) & 0x1f);
        mask = ((1 << size) - 1) << offset;
        data = (data << offset) & mask;
        reg &= 0x7f;

        kx_lock_acquire(hw,&hw->hw_lock, &flags);
        data |= inpd(hw->port + (word)reg) & ~mask;
        outpd(hw->port + reg,data);
        kx_lock_release(hw,&hw->hw_lock, &flags);
    } 
     else 
    {
        outpd(hw->port + reg,data);
    }
}

KX_API(int, kx_writespi(kx_hw *hw, dword data))
{
    dword reset, set;
    dword reg, tmp;

    int n, result;
    if(hw->is_zsnb)
        reg = P16V_pSPI;
    else
    {
        // not supported
        return -1;
    }

    if (data > 0xffff) /* Only 16bit values allowed */
        return 1;

    tmp = kx_readp16v(hw, reg, 0);
    reset = (tmp & ~0x3ffff) | 0x20000; // Set xxx20000
    set = reset | 0x10000; // set xxx1xxxx
    kx_writep16v(hw, reg, 0, reset | data);
    tmp = kx_readp16v(hw, reg, 0); // write post
    kx_writep16v(hw, reg, 0, set | data);
    result = 1;

    // Wait for status bit to return to 0
    for (n = 0; n < 100; n++) 
    {
        // wait 10usec
        hw->cb.usleep(10);

        tmp = kx_readp16v(hw, reg, 0);
        if (!(tmp & 0x10000))
        {
            result = 0;
            break;
        }
    }
    if (result) /* Timed out */
        return -1;

    kx_writep16v(hw, reg, 0, reset | data);
    tmp = kx_readp16v(hw, reg, 0); /* Write post */
    return 0;
}

KX_API(int, kx_writei2c(kx_hw * hw,dword reg, dword value))
{
    // return 0;

    dword tmp;
    int timeout = 0;
    int status;
    int retry;

    if ((reg > 0x7f) || (value > 0x1ff)) {
        debug(DLIB, "kx_i2c_write: invalid parameters\n");
        return -1;
    }

    tmp = reg << 25 | value << 16;
    /* Not sure what this I2C channel controls. */
    /* snd_emu10k1_ptr_write(emu, P17V_I2C_0, 0, tmp); */

    /* This controls the I2C connected to the WM8775 ADC Codec */
    kx_writep16v(hw, p17vI2C_1, 0, tmp);
    tmp = kx_readp16v(hw, p17vI2C_1, 0); /* write post */

    for (retry = 0; retry < 10; retry++) {
        /* Send the data to i2c */
        //tmp = snd_emu10k1_ptr_read(emu, P17V_I2C_ADDR, 0);
        //tmp = tmp & ~(I2C_A_ADC_READ|I2C_A_ADC_LAST|I2C_A_ADC_START|I2C_A_ADC_ADD_MASK);
        tmp = 0;
        tmp = tmp | (I2C_A_ADC_LAST|I2C_A_ADC_START|I2C_A_ADC_ADD);
        kx_writep16v(hw, p17vI2C_ADDR, 0, tmp);

        /* Wait till the transaction ends */
        while (1) 
        {
            // udelay(10);
            hw->cb.usleep(10);

            status = kx_readp16v(hw, p17vI2C_ADDR, 0);
                    // snd_printk("I2C:status=0x%x\n", status);
            timeout++;
            if ((status & I2C_A_ADC_START) == 0)
                break;

            if (timeout > 1000) {

                        debug(DLIB,"kx_i2c_write:timeout with status=0x%x\n",status);
                break;
            }
        }
        //Read back and see if the transaction is successful
        if ((status & I2C_A_ADC_ABORT) == 0)
            break;
    }

    if (retry == 10) {
        debug(DLIB,"kx_i2c_write: error writing to ADC !!\n");
        return -1;
    }
    
    return 0;
}


KX_API(dword, kx_readfn0(kx_hw * hw, dword reg))
{
    dword val;
    unsigned long flags=0;

    if(reg & 0xff000000) 
    {
        dword mask;
        byte size, offset;

        size = (byte) ((reg >> 24) & 0x3f);
        offset = (byte) ((reg >> 16) & 0x1f);
        mask = ((1 << size) - 1) << offset;
        reg &= 0x7f;

        kx_lock_acquire(hw,&hw->hw_lock, &flags);
        val = inpd(hw->port + reg);
        kx_lock_release(hw,&hw->hw_lock, &flags);

        return (val & mask) >> offset;
        }
         else 
        {
        return inpd(hw->port + reg);
    }
}

KX_API(void, kx_writefn0w(kx_hw *hw, dword reg, word data))
{
        outpw(hw->port + reg,data);
}

KX_API(void, kx_writefn0b(kx_hw *hw, dword reg, byte data))
{
        outp(hw->port + reg,data);
}


KX_API(word, kx_readfn0w(kx_hw * hw, dword reg))
{
        return inpw(hw->port + reg);
}

KX_API(byte, kx_readfn0b(kx_hw * hw, dword reg))
{
        return inp(hw->port + reg);
}

KX_API(void, kx_writeptrw(kx_hw *hw, dword reg, dword channel, word data))
{
        dword regptr;
        unsigned long flags=0;

        regptr = ((reg << 16) & PTR_ADDRESS_MASK) | (channel & PTR_CHANNELNUM_MASK);

        kx_lock_acquire(hw,&hw->hw_lock, &flags);
        outpd(hw->port + PTR,regptr);
        outpw(hw->port + DATA,data);
        kx_lock_release(hw,&hw->hw_lock, &flags);
}

KX_API(word, kx_readptrw(kx_hw * hw, dword reg, dword channel))
{
        dword regptr;
        word val;
        unsigned long flags=0;

        regptr = ((reg << 16) & PTR_ADDRESS_MASK) | (channel & PTR_CHANNELNUM_MASK);
        kx_lock_acquire(hw,&hw->hw_lock, &flags);
        outpd(hw->port + PTR,regptr);
        val = inpw(hw->port + DATA);
        kx_lock_release(hw,&hw->hw_lock, &flags);

        return val;
}



KX_API(void, kx_writeptrb(kx_hw *hw, dword reg, dword channel, byte data))
{
        dword regptr;
        unsigned long flags=0;

        regptr = ((reg << 16) & PTR_ADDRESS_MASK) | (channel & PTR_CHANNELNUM_MASK);

        kx_lock_acquire(hw,&hw->hw_lock, &flags);

        outpd(hw->port + PTR,regptr);
        outp(hw->port + DATA,data);

        kx_lock_release(hw,&hw->hw_lock, &flags);
}

KX_API(byte, kx_readptrb(kx_hw * hw, dword reg, dword channel))
{
        dword regptr;
        byte val;
        unsigned long flags=0;

        regptr = ((reg << 16) & PTR_ADDRESS_MASK) | (channel & PTR_CHANNELNUM_MASK);
        kx_lock_acquire(hw,&hw->hw_lock, &flags);
        outpd(hw->port + PTR,regptr);
        val = inp(hw->port + DATA);
        kx_lock_release(hw,&hw->hw_lock, &flags);

        return val;
}


KX_API(void,kx_writeptr(kx_hw *hw, dword reg, dword channel, dword data))
{
    dword regptr;
    unsigned long flags=0;

    regptr = ((reg << 16) & PTR_ADDRESS_MASK) | (channel & PTR_CHANNELNUM_MASK);

    if(reg & 0xff000000) 
    {
        dword mask;
        byte size, offset;

        size = (byte) ((reg >> 24) & 0x3f);
        offset = (byte) ((reg >> 16) & 0x1f);
        mask = ((1 << size) - 1) << offset;
        data = (data << offset) & mask;

        kx_lock_acquire(hw,&hw->hw_lock, &flags);
        outpd(hw->port + PTR,regptr);
        data |= inpd(hw->port + DATA) & ~mask;
        outpd(hw->port + DATA,data);
        kx_lock_release(hw,&hw->hw_lock, &flags);
    } else {
        kx_lock_acquire(hw,&hw->hw_lock, &flags);
        outpd(hw->port + PTR,regptr);
        outpd(hw->port + DATA,data);
        kx_lock_release(hw,&hw->hw_lock, &flags);
    }
}

/*
#undef PAGE_SIZE
#include <kern/clock.h>

#define PROFILE(var) \
	end = mach_absolute_time(); \
	absolutetime_to_nanoseconds(end - start, &var); \
	start = end;

KX_API(void,kx_writeptr_prof(kx_hw *hw, dword reg, dword channel, dword data))
{
    dword regptr;
    unsigned long flags=0;
	
	uint64_t start, end, total, lock, ptr, val_in, val_out, unlock;

	total = start = mach_absolute_time();

    regptr = ((reg << 16) & PTR_ADDRESS_MASK) | (channel & PTR_CHANNELNUM_MASK);

	kx_lock_acquire(hw,&hw->hw_lock, &flags);
	PROFILE(lock);
	
    if(reg & 0xff000000) 
    {
        dword mask;
        byte size, offset;

        size = (byte) ((reg >> 24) & 0x3f);
        offset = (byte) ((reg >> 16) & 0x1f);
        mask = ((1 << size) - 1) << offset;
        data = (data << offset) & mask;

        outpd(hw->port + PTR,regptr);
		PROFILE(ptr);

        data |= inpd(hw->port + DATA) & ~mask;
		PROFILE(val_in);		

        outpd(hw->port + DATA,data);
		PROFILE(val_out);		
    } else {
        outpd(hw->port + PTR,regptr);
		PROFILE(ptr);

        outpd(hw->port + DATA,data);
		PROFILE(val_out);
		val_in = 0;
    }

	kx_lock_release(hw,&hw->hw_lock, &flags);
	PROFILE(unlock);
	
	absolutetime_to_nanoseconds(mach_absolute_time() - total, &total);

	debug(DLIB, "kx_writeptr_prof: total: %lu, lock: %lu, ptr: %lu, val_in: %lu, val_out: %lu, unlock: %lu\n",
		  total, lock, ptr, val_in, val_out, unlock);
}
*/

KX_API(void,kx_writeptr_multiple(kx_hw *hw, dword channel, ...))
{
    va_list args;

    unsigned long flags=0;
        dword reg;

    va_start(args, channel);

    kx_lock_acquire(hw,&hw->hw_lock, &flags);
    while((reg = va_arg(args, dword)) != (dword)REGLIST_END) 
    {
        dword data = va_arg(args, dword);
        dword regptr = (((reg << 16) & PTR_ADDRESS_MASK)
                  | (channel & PTR_CHANNELNUM_MASK));
        outpd(hw->port + PTR,regptr);

        if(reg & 0xff000000) 
        {
            int size = (reg >> 24) & 0x3f;
                        int offset = (reg >> 16) & 0x1f;
            dword mask = ((1 << size) - 1) << offset;
            data = (data << offset) & mask;

            data |= inpd(hw->port + DATA) & ~mask;
        }
        outpd(hw->port + DATA,data);
    }
    kx_lock_release(hw,&hw->hw_lock, &flags);

    va_end(args);
}

KX_API(dword, kx_readptr(kx_hw * hw, dword reg, dword channel))
{
    dword regptr, val;
    unsigned long flags=0;

    regptr = ((reg << 16) & PTR_ADDRESS_MASK) | (channel & PTR_CHANNELNUM_MASK);

    if(reg & 0xff000000)
    {
        dword mask;
        byte size, offset;

        size = (byte) ((reg >> 24) & 0x3f);
        offset = (byte) ((reg >> 16) & 0x1f);
        mask = ((1 << size) - 1) << offset;

        kx_lock_acquire(hw,&hw->hw_lock, &flags);
        outpd(hw->port + PTR,regptr);
        val = inpd(hw->port + DATA);
        kx_lock_release(hw,&hw->hw_lock, &flags);

        return (val & mask) >> offset;
    } else {
        kx_lock_acquire(hw,&hw->hw_lock, &flags);
        outpd(hw->port + PTR,regptr);
        val = inpd(hw->port + DATA);
        kx_lock_release(hw,&hw->hw_lock, &flags);

        return val;
    }
}

KX_API(int,kx_writefpga(kx_hw *hw, dword reg, dword value))
{
    unsigned long flags;

    if (reg > 0x3f)
        return -1;

    reg += 0x40; // 0x40 upwards are registers
    if (value > 0x3f) // 0 to 0x3f are values
        return -2;

    kx_lock_acquire(hw,&hw->hw_lock, &flags);

    outpd(hw->port+HCFG_K2,reg);
    hw->cb.usleep(10);
    outpd(hw->port+HCFG_K2,reg | 0x80);  // High bit clocks the value into the fpga.
    hw->cb.usleep(10);
    outpd(hw->port+HCFG_K2,value);
    hw->cb.usleep(10);
    outpd(hw->port+HCFG_K2,value | 0x80);  // High bit clocks the value into the fpga

    kx_lock_release(hw,&hw->hw_lock, &flags);

    return 0;
}

KX_API(dword,kx_readfpga(kx_hw *hw, dword reg))
{
    unsigned long flags;

    if (reg > 0x3f)
        return (dword)-1;

    reg += 0x40; /// 0x40 upwards are registers

    kx_lock_acquire(hw,&hw->hw_lock, &flags);

    outpd(hw->port+HCFG_K2,reg);
    hw->cb.usleep(10);
    outpd(hw->port+HCFG_K2,reg | 0x80);  // High bit clocks the value into the fpga
    hw->cb.usleep(10);
    dword ret = ((inpd(hw->port+HCFG_K2) >> 8) & 0x7f);
    kx_lock_release(hw,&hw->hw_lock, &flags);

    return ret;
}

// Each Destination has one and only one Source,
// but one Source can feed any number of Destinations simultaneously.

KX_API(int,kx_fpga_link_src2dst(kx_hw *hw, dword src, dword dst))
{
    kx_writefpga(hw, 0x00, ((dst >> 8) & 0x3f) );
    kx_writefpga(hw, 0x01, (dst & 0x3f) );
    kx_writefpga(hw, 0x02, ((src >> 8) & 0x3f) );
    kx_writefpga(hw, 0x03, (src & 0x3f) );

    return 0;
}


KX_API(void, kx_wcwait(kx_hw *hw, dword wait))
{
    volatile unsigned cnt;
    dword newtime=0, curtime=0;

    curtime = kx_readfn0(hw, WC_SAMPLECOUNTER);
    for(;wait;wait--) 
    {
        for(cnt=0;cnt<TIMEOUT;cnt++)
        {
            newtime = kx_readfn0(hw, WC_SAMPLECOUNTER);
            if(newtime != curtime)
                break;
        }

        if(cnt >= TIMEOUT) // timeout
            break;

        curtime = newtime;
    }
}

void kx_init_spdif(kx_hw *hw,int mode);
void kx_init_spdif(kx_hw *hw,int mode)
{
     // init value 0x02109204:

     // Clock accuracy    = 00    (1000ppm)
     // Sample Rate       = 0010  (48kHz)
     // Audio Channel     = 0001  (Left of 2)
     // Source Number     = 0000  (Unspecified)
     // Generation Status = 1     (Original for Cat Code 12)
     // Cat Code          = 0x12  (0010010) (Digital Signal Mixer)
     // Mode              = 00    (Mode 0)
     // Emphasis          = 000   (None)
     // CP                = 1     (Copyright unasserted)
     // AN                = 0     (Digital audio)
     // P                 = 0     (Consumer)

        dword cat_code=0x00001200;

        dword emph=0x0;

        if(mode!=EHC_SPDIF_44)
         emph|=SCS_SAMPLERATE_48;

        dword bits=(SCS_CLKACCY_1000PPM | 
                    SCS_CHANNELNUM_LEFT | SCS_SOURCENUM_UNSPEC | SCS_GENERATIONSTATUS 
                    | cat_code | emph | SCS_COPYRIGHT);

    for(int i=0;i<3;i++)
     hw->spdif_states[i]=bits;

    kx_writeptr_multiple(hw, 0,
                // SPDIF0
                SCS0, bits,
                // SPDIF1
                SCS1, bits,
                // SPDIF2 & SPDIF3 (10k1)
                SCS2, bits,
                REGLIST_END);

    if(hw->is_a2) // set SCS0x
    {
     switch(mode)
     {
      case EHC_SPDIF_44:
        kx_writeptr_multiple(hw,1,
                SCS0, 0xf1+0xa,    // 3542 change: set number of bits to 24, not 'undefined'
                SCS1, 0xf1+0xa,
                SCS2, 0xf1+0xa,
                REGLIST_END);
        break;
      case EHC_SPDIF_48:
        kx_writeptr_multiple(hw,1,
                SCS0, 0xb1+0xa,
                SCS1, 0xb1+0xa,
                SCS2, 0xb1+0xa,
                REGLIST_END);
        break;
      case EHC_SPDIF_96:
        kx_writeptr_multiple(hw,1,
                SCS0, 0xa1+0xa,
                SCS1, 0xa1+0xa,
                SCS2, 0xa1+0xa,
                REGLIST_END);
        break;
     }
    }
}

KX_API(int,kx_reset_voices(kx_hw *hw))
{
    int chn;

    // init the envelope engine
    for(chn = 0; chn < KX_NUMBER_OF_VOICES; chn++) 
    {
        kx_writeptr_multiple(hw, chn,
                    DCYSUSV, 0, // according to awe specs
                    IP, 0,
                    PTAB, 0,
                    CPF, 0,
                    emuCCR, 0,
                    REGLIST_END);
            kx_wcwait(hw,2);

            kx_writeptr(hw, QKBCA, chn, 0);

            kx_writeptr_multiple(hw, chn,
                                    CD0,0,
                                    CD1,0,
                                    CD2,0,
                                    CD3,0,
                                    CD4,0,
                                    CD5,0,
                                    CD6,0,
                                    CD7,0,
                                    CD8,0,
                                    CD9,0,
                                    CDA,0,
                                    CDB,0,
                                    CDC,0,
                                    CDD,0,
                                    CDE,0,
                                    CDF,0,

                                    MAPA, 0xffffffff,
                                    MAPB, 0xffffffff,
                                    REGLIST_END);

                if(hw->is_10k2) // specific part
                {
                    kx_writeptr_multiple(hw,chn,
                        CSBA,0x0,
                        CSDC,0x0,
                        CSFE,0x0,
                        CSHG,0x0,
                        FXRT2_K2,0xbfbfbfbf,
                        FXRT1_K2,0xbfbfbfbf,
                        FXAMOUNT_K2,0x0,
                        REGLIST_END);
                }
                else
                {
                    kx_writeptr(hw,FXRT_K1,chn,0x30120000);
                }


            kx_writeptr_multiple(hw, chn,

                    VTFT, 0x0,
                    CVCF, 0x0,

                    Z1, 0,
                    Z2, 0,

                    ATKHLDM, 0,
                    DCYSUSM, 0,
                    IP, 0,
                    IFATN, 0,
                    PEFE, 0,
                    FMMOD, 0,
                    TREMFRQ, 0, // 1 Hz
                    FM2FRQ2, 0, // 1 Hz
                    TEMPENV, 0,

                    // These should be last
                    LFOVAL2, 0,
                    LFOVAL1, 0,
                    VEHA, 0,
                    VEV, 0,
                    ENVVAL, 0,
                    SCSA, 0x40,
                    SDL, 0xffffff,
                    emuCCR, 0x0,
                                    REGLIST_END);

                 kx_wcwait(hw,1);
    }

    hw->last_voice=0;

    debug(DLIB,"HW voices reset\n");

        return 0;
}

int kx_hal_init(kx_hw *hw,int fast)
{
    int ret=0;

    debug(DLIB,"initializing kX HAL...\n");

        if(hw->is_cardbus)
        {
               // before anything else, enable I/O on zs notebook

               word port=hw->port+TINA_POWER_CTRL;
               dword value;

               value = inpd(port);

               debug(DLIB,"pcmcia init [initial value: %08x]\n",value);

               outpd(port,0x00d00000); // set gpo: 0xd0 (1101B)  --> creative pdf recommends 1111B
               value = inpd(port);
               outpd(port,0x00d00001); // set pll_enable
               value = inpd(port);

               // wait at least 8usec (4*2)
               hw->cb.usleep(8);
               // value = inpd(port); // wait more..
               // creative pdf recommends to poll power control [11:9]=100

               outpd(port,0x00d0005f); // set buff_res=1, clk2048: p17, 10k2, common; set gated=1
               value = inpd(port);
               outpd(port,0x00d0007f); // set audio_reset=1
               value = inpd(port);
               outpd(port,0x0090007f); // set gpo: 0x90 (1001B)
               value = inpd(port);

               // sleep for 200ms
               // FIXME NOW !!
        }

        if(hw->is_edsp)
        {
               // special sequence
               debug(DLIB,"hal: init e-dsp\n");
               outpd(hw->port+HCFG_K1,0x0005a00c);
               outpd(hw->port+HCFG_K1,0x0005a004);
               outpd(hw->port+HCFG_K1,0x0005a000);
               outpd(hw->port+HCFG_K1,0x0005a000);
               debug(DLIB,"hal: need to upload E-DSP FPGA firmware and setup routing\n");
        }

        // initial IRQ set-up
        kx_writefn0(hw,INTE,0);

        dword tmp_hcfg_k1=0;
        tmp_hcfg_k1=kx_readfn0(hw,HCFG_K1);
        tmp_hcfg_k1|=(HCFG_LOCKSOUNDCACHE | HCFG_LOCKTANKCACHE_MASK);
        tmp_hcfg_k1&=(~HCFG_AUDIOENABLE);

        // mute all
        kx_writefn0(hw, HCFG_K1, tmp_hcfg_k1); // HCFG_AUDIOENABLED is 0

        kx_disable_analog(hw,1);

    /* Disable audio and lock cache: already done in kx_init */

    /* Reset recording buffers */
    kx_writeptr_multiple(hw, 0,
                MBS, ADCBS_BUFSIZE_NONE,
                MBA, 0,
                FXBS, ADCBS_BUFSIZE_NONE,
                FXBA, 0,
                ADCBS, ADCBS_BUFSIZE_NONE,
                ADCBA, 0,
                REGLIST_END);

    if(hw->is_10k2)
     kx_writeptr(hw,SOC,0x0,SPDIF_FORMAT); // bypass is disabled; 24bit mode
                            // note: bypass method 2: via HCFG_K2
    else
     kx_writeptr(hw,SOC,0x0,0x0);

    if(hw->is_10k2)
    {
     kx_writeptr_multiple(hw,0,
        SPRA,0,
        SPRC,0,
        SPRI,0,
        REGLIST_END);
    }

    /* Disable channel interrupt */
    kx_writeptr_multiple(hw, 0,
                CLIEL, 0,
                CLIEH, 0,
                SOLL, 0,
                SOLH, 0,
                REGLIST_END);

    if(hw->is_10k2)
    {
     kx_writeptr_multiple(hw,0,
        HLIEL,0,
        HLIEH,0,
        REGLIST_END);
    }

    kx_reset_voices(hw);

    kx_init_spdif(hw,EHC_SPDIF_48);

    if(!fast)
    {
        /*
            // pre-3539: moved after ac97 initialization
            ret=kx_dsp_init(hw); // implies kx_dsp_reset(hw);
            if(ret)
            {
             debug(DERR,"!!! DSP init failed\n");
             return ret;
            }
        */
            if(!hw->standalone)
            {

                for(int pagecount = 0; pagecount < MAXPAGES; pagecount++)
                        ((dword *) hw->virtualpagetable.addr)[pagecount] = (hw->silentpage.dma_handle * 2) | pagecount;

                    /* Init page table & tank memory base register */
                    dword trbs=0;
                    switch(hw->cb.tram_size)
                    {
                            case 16*1024: trbs=TRBS_BUFFSIZE_16K; break;
                            case 32*1024: trbs=TRBS_BUFFSIZE_32K; break;
                            case 64*1024: trbs=TRBS_BUFFSIZE_64K; break;
                            case 128*1024: trbs=TRBS_BUFFSIZE_128K; break;
                            case 256*1024: trbs=TRBS_BUFFSIZE_256K; break;
                            case 512*1024: trbs=TRBS_BUFFSIZE_512K; break;
                            case 1024*1024: trbs=TRBS_BUFFSIZE_1024K; break;
                            case 2048*1024: trbs=TRBS_BUFFSIZE_2048K; break;
                            case 0: // disable TRAM
                                trbs=0x0;
                                hw->cb.tram_size=0x0; // disable TRAM
                                break;
                        default:
                            debug(DERR,"!!! Wrong tank_size (%d) specified\n",hw->cb.tram_size);
                            trbs=0x0;
                            hw->cb.tram_size=0x0; // disable TRAM
                            break;
                    }
                    kx_writeptr_multiple(hw, 0,
                                PTBA, hw->virtualpagetable.dma_handle,
                                TRBA, hw->cb.tram_size?hw->tankmem.dma_handle:0,
                                TRBS, trbs, 
                                REGLIST_END);
    
                    for(int chn = 0; chn < KX_NUMBER_OF_VOICES; chn++) 
                    {
                        kx_writeptr_multiple(hw, chn,
                                    MAPA, MAP_PTI_MASK | (hw->silentpage.dma_handle * 2),
                                    MAPB, MAP_PTI_MASK | (hw->silentpage.dma_handle * 2),
                                    REGLIST_END);
                    }
        }
    } // fast?

    if(hw->is_10k2)
    {
        // creative uses 0x8001 for 10k2; 0x8071 for 10k2-ex
                // = audio_enable+automute_k2+[HCFG_I2SASRC0_K2 -- will be set by kx_p16b_init]

        hw->hcfg_k1=HCFG_AUTOMUTE_K2 | HCFG_AUDIOENABLE | HCFG_PBE_K2;
                // HCFG_VBIT_AUTOMUTE_K2

        if(hw->is_edsp)
         hw->hcfg_k1|=(HCFG_CLOCKSYNTH_K2|HCFG_CODECFORMAT_K2|(HCFG_I2SASRC0_K2|HCFG_I2SASRC1_K2|HCFG_I2SASRC2_K2));
          // CL uses 0x1a071 or 0x2071: HCFG_I2SASRCxxx_K2 + [HCFG_CODECFORMAT_K2]
          // CL does not set (sometimes) HCFG_PBE_K2 and HCFG_AUTOMUTE_K2
          // for 44.1kHz mode: 0x8a071: HCFG_44K_K2|HCFG_AUTOMUTE_K2|HCFG_CLOCKSYNTH_K2|HCFG_I2SASRCx_K2|HCFG_AUDIOENABLE
          //    fpga wclock: 1
          //    ehc: 0x400
          // for 48.0kHz mode: w/o HCFG_44K_K2, wclock: 0

        hw->hcfg_k2=0;

        if(hw->is_a2 && !hw->is_edsp) // Audigy2
        {
           kx_p16v_init(hw);
        }
    }
    else
    {
        if( (hw->pci_subsys == 0x00201102) || (hw->pci_subsys == 0x04001102) ||
              ( (hw->pci_subsys == 0x00211102) && (hw->pci_chiprev < 6) ))
                    hw->hcfg_k1=HCFG_AUDIOENABLE  | HCFG_AUTOMUTE_K1;
        else
                hw->hcfg_k1=HCFG_AUDIOENABLE  | HCFG_AUTOMUTE_K1 | HCFG_JOYENABLE_K1;

        if(hw->is_aps)
        {
            hw->hcfg_k1|=HCFG_CODECFORMAT_I2S_K1;
        }
    }

        if(hw->is_zsnb)
        {
            debug(DLIB,"spi initialization\n");

            // init spi here
            const dword spi_dac[] = {
                        0x00ff, 0x02ff, 0x0400, 0x0520, 0x0600, 0x08ff, 0x0aff,
                                 0x0cff, 0x0eff, 0x10ff, 0x1200, 0x1400, 0x1480, 0x1800,
                        0x1aff, 0x1cff, 0x1e00, 0x0530, 0x0602, 0x0622, 0x1400,
            };

            // init spi
            int n;
            for(n=0; (size_t)n < sizeof(spi_dac)/sizeof(spi_dac[0]); n++)
                kx_writespi(hw, spi_dac[n]);

            // wolfson WM8775 ADC
            static unsigned int i2c_adc_init[][2] = {
               { 0x17, 0x00 }, /* Reset */
               { 0x07, 0x00 }, /* Timeout: TOD:0 [timeout disabled] */
               { 0x0b, 0x22 },  /* Interface control: ADCFMT:10[i2s] ADCLRP:0 ADCBCP:0 ADCWL:10[24-bit] ADCMCLK:0 ADCHPD:0 [high-pass filter enabled] */
               { 0x0c, 0x22 },  /* Master mode control: ADCRATE:010[256fs] [b5]:1 ADCOSR:0[128x over] ADCMS:0[slave]*/
               { 0x0d, 0x08 },  /* Powerdown control: PWDN:0 ADCPD:0 AINPD:0 */
               { 0x0e, 0xcf },  /* Attenuation Left  0x01 = -103dB, 0xff = 24dB: ZCLA:? LAG:cf */
               { 0x0f, 0xcf },  /* Attenuation Right 0.5dB steps: ZCRA:? RAG:cf */
               { 0x10, 0x7b },  /* ALC Control 1: LCT: 1011 [-5dB] MAXGAIN:111 [+24dB] LCSEL:x1 [stereo?]*/
               { 0x11, 0x00 },  /* ALC Control 2: HLD:0 ALCZC:0 LCEN:0 [automatic level ctrl]*/
               { 0x12, 0x32 },  /* ALC Control 3: ATK:0010 DCY:0011 */
               { 0x13, 0x00 },  /* Noise gate control: NGAT:0 NGTH:0 */
               { 0x14, 0xa6 },  /* Limiter control: MAXATTEN:0110 TRANWIN:010 */
               { 0x15, ADC_MUX_2 },  /* ADC Mixer control. Mic for Audigy 2 ZS Notebook; ZCRA:0 LRBOTH:0[rag] MUTELA,MUTERA:0 */
            };

            // init i2c
            kx_writep16v(hw, P17V_I2S_SRC_SEL, 0, 0x2020205f); 
             // set master_mode=111, i2s_select_adc=96kHz, fs_adc=25MHz, fs_dac=25MHz [last byte: 0x5f]
             // default [per spec.pdf]: master_mode=000, fs_dac=12.5@48 [last byte: 0x42]

            int size = sizeof(i2c_adc_init)/sizeof(i2c_adc_init[0]);
            for (n = 0; n < size; n++)
                kx_writei2c(hw, i2c_adc_init[n][0], i2c_adc_init[n][1]);
        }

        if(kx_readfn0(hw,INTE)!=0)
         debug(DERR,"hal init: INTE = %x [critical error!!!]\n",kx_readfn0(hw,INTE));

        // Enable Vol_Ctrl irqs
        kx_irq_enable(hw,INTE_VOLINCRENABLE | INTE_VOLDECRENABLE | INTE_MUTEENABLE );

        kx_irq_enable(hw,INTE_PCIERRORENABLE);
        kx_irq_enable(hw,INTE_FXDSPENABLE);

        if(hw->ext_flags&KX_HW_NO_SPDIF_NOTIFICATION)
        {
         kx_irq_disable(hw,INTE_GPSPDIFENABLE | INTE_CDSPDIFENABLE);
        }
        else
        {
         kx_irq_enable(hw,INTE_GPSPDIFENABLE | INTE_CDSPDIFENABLE);
        }

        // lock-ups under APIC/nForce2?..
        // FIXME NOW 3537c
//        if(hw->is_10k2)
//         kx_irq_enable(hw,INTE_K2_GPIO_ENABLE);

        // FIXME: creative enables sampleratetracker, too?..

        // creative interrupts:
        // 10k2: 11718
        // 10k2-ex: 13718
        // INTE_K2_MIDIRXENABLE|INTE_FXDSPENABLE|vol_irqs|INTE_GPSPDIFENABLE|INTE_CDSPDIFENABLE
        // 10k2-ex: +=INTE_SAMPLERATETRACKER??? FIXME

        if(hw->is_aps)
        {
          // init values already set above
          ret=kx_ecard_init(hw);
        }

        if(!hw->lack_ac97)
        {
          ret=kx_ac97_init(hw); // will set 'have_ac97' based on 'lack_ac97'
          if(hw->have_ac97)
           kx_ac97write(hw,AC97_REG_MASTER_VOL,0x8000); // mute ac97
        }
        else
          hw->have_ac97=0;

        if(ret)
           debug(DLIB,"!!! hal: AC97/ECard init FAILED\n");

        if(!fast)
        {
           if(kx_mpu_init(hw)) // no result codes: non critical; SHOULD be called (list init)
           {
              debug(DLIB,"MPU init failed: no UART-mode MPU available\n");
           }
        }
        else
        {
         for(int i=0;i<hw->have_mpu;i++)
           kx_mpu_reset(hw,i,0); // irqs off
        }

        // detect livedrives _after_ mpu_init
        if(!hw->is_zsnb && !hw->is_edsp)
        {
         detect_db(hw);
        }
        else
         strncpy(hw->db_name,"none",sizeof(hw->db_name));

        if(!fast)
        {
                ret=kx_dsp_init(hw); // implies kx_dsp_reset(hw);
                if(ret)
                {
                 debug(DERR,"!!! DSP init failed\n");
                 return ret;
                }
        }

    // finally, enable audio NOW
    // including unmute()

    if(!fast)
    {
            if(hw->is_10k2 && !hw->is_edsp)
              kx_writefn0(hw, HCFG_K2, hw->hcfg_k2);

            // read current HCFG_K1 and set new bits
            tmp_hcfg_k1=kx_readfn0(hw,HCFG_K1);

            // preserve these bits:
            tmp_hcfg_k1&=(HCFG_44K_K2|HCFG_CODECFORMAT_K2|HCFG_CODECFORMAT2_K2|HCFG_CLOCKSYNTH_K2|HCFG_SLOWRAMPRATE_K2);
            tmp_hcfg_k1|=(hw->hcfg_k1|((hw->cb.tram_size==0)?HCFG_LOCKTANKCACHE_MASK:0));

            kx_writefn0(hw, HCFG_K1, tmp_hcfg_k1);
    }

 return ret;
}

int kx_hal_close(kx_hw *hw)
{
    if(!hw->standalone)
    {
         kx_writefn0(hw,INTE, 0);
         kx_writefn0(hw,HCFG_K1, HCFG_LOCKSOUNDCACHE | HCFG_LOCKTANKCACHE_MASK);

         kx_disable_analog(hw,1);

         kx_writeptr(hw,PTBA, 0, 0);
         kx_writeptr(hw,TRBA, 0, 0);

         kx_mpu_close(hw);

         if(hw->is_aps)
          kx_ecard_close(hw);

         if(hw->have_ac97)
           kx_ac97_close(hw);

         int ch;

         for(ch = 0; ch < KX_NUMBER_OF_VOICES; ch++)
            kx_writeptr(hw, DCYSUSV, ch, 0);

         for(ch = 0; ch < KX_NUMBER_OF_VOICES; ch++) 
         {
            kx_writeptr_multiple(hw, ch,
                        VTFT, 0,
                        CVCF, 0,
                        PTAB, 0,
                        CPF, 0,
                        REGLIST_END);
         }

         kx_writeptr_multiple(hw, 0,
                                 PTBA, 0,

                    // recording buffers
                    MBS, ADCBS_BUFSIZE_NONE,
                    MBA, 0,
                    FXBS, ADCBS_BUFSIZE_NONE,
                    FXBA, 0,
                    ADCBS, ADCBS_BUFSIZE_NONE,
                    ADCBA, 0,
                    TRBS, 0,
                    TRBA, 0,

                    // voice interrupt
                    CLIEL, 0,
                    CLIEH, 0,
                    SOLL, 0,
                    SOLH, 0,
                    REGLIST_END);

         kx_dsp_stop(hw);

         if(hw->is_10k2)
         {
              kx_writeptr(hw,FXWCL, 0, 0);
              kx_writeptr(hw,FXWCH, 0, 0);

              // mute
              kx_disable_analog(hw,1);
         }
         else
         {
              kx_writeptr(hw,FXWC_K1, 0, 0);
         }
    }

    kx_dsp_close(hw);

 return 0;
}

KX_API(int,kx_get_hw_parameter(kx_hw *hw,int id,dword *value))
{
 if(!value)
  return -2;
 switch(id)
 {
    case KX_HW_DOO:
        if(hw->is_aps || hw->is_edsp)
         return -1;

            // Get DOO flag
            if(!hw->is_10k2)
            {
                    if(hw->is_51)
                    {
                        dword hcfg;
                        hcfg=kx_readfn0(hw,HCFG_K1);
                        if(hcfg&HCFG_GPOUT2_K1)
                            *value=1;
                        else
                            *value=0;
                    }
                    else
                    {
                        word val;
                        val=kx_ac97read(hw,AC97_REG_PCM_VOL);
                        if(val&0x8000) // muted
                            *value=1;
                        else
                            *value=0;
                    }
                }
                else // 10k2:
                {
                    if(hw->is_a4) // for a4 value:
                    {
                     dword hcfg2=kx_readfn0(hw,HCFG_K2);
                     if(hcfg2&HCFG_K2_A4_LINE_DOO)
                      return 0;
                     else
                      return 1;
                    }

                    if(hw->is_a2) // a2(ex) doesn't need 'doo' mode
                     return -1;

                    dword hcfg2;
                    hcfg2=kx_readfn0(hw,HCFG_K2);
                    if(hcfg2&HCFG_K2_ENABLE_DIGITAL)
                     *value=1;
                    else
                         *value=0;
                }
        break;
    case KX_HW_AC3_PASSTHROUGH:
        if(!hw->can_passthru /*|| hw->is_edsp*/)
        {
         return -1; 
        }
        {
                 *value=hw->pt_spdif;
                 return 0;
                }
        break;
    case KX_HW_SWAP_FRONT_REAR:
            if(hw->is_aps || hw->is_edsp)
             return -1;

        if(hw->dsp_flags&KX_DSP_SWAP_REAR)
         *value=1;
        else
         *value=0;
        break;
    case KX_HW_ROUTE_PH_TO_CSW:
        if(hw->is_aps || hw->is_zsnb || hw->is_edsp)
         return -1;

        if(hw->dsp_flags&KX_DSP_ROUTE_PH_TO_CSW)
         *value=1;
        else
         *value=0;
        break;
    case KX_HW_SPDIF_FREQ:
        if(hw->is_edsp)
         return -1;

        if(hw->is_10k2)
        {
            // FIXME NOW: for 10k2.2: do we really need to set these?..
                dword val=kx_readptr(hw,EHC,0);
                
                if(val&EHC_SPDIF_96) *value=2;
                 else
                if(val&EHC_SPDIF_44) *value=0;
                 else
                 {
                  if(hw->is_a2 && !hw->is_k8) // audigy2, but not audigy4/a2value
                  {
                   // read HCFG bit: 'ex' models usually perform a different resampling
                           dword ori=kx_readfn0(hw,HCFG_K2);

                           // a2 can have val=0, but hcfg is |=0x80
                           // seems to be true for a2 plat. ex

                           // a4/a2value are not affected by this bits
                           if(ori&HCFG_K2_UNKNOWN_80) // a2, !k4
                     *value=0;
                   else
                     *value=1;
                  }
                  else
                   *value=1; // a1, k4; 44.1 otherwise [for audigy1, too]
                 }
            } else return -1;
        break;
    case KX_HW_SPDIF_BYPASS:
        {
         if(hw->is_aps || hw->is_zsnb || hw->is_edsp)
          return -1;

         // fixme: different bypass methods...
         dword val1=0,val2=0;
         if(hw->is_10k2)
         {
          val1=kx_readfn0(hw,HCFG_K2);
          val1=(val1&HCFG_K2_BYPASS)?1:0;
         }

         {
          val2=kx_readptr(hw,SOC,0);
              val2=val2&(SP0_BYPASS_MASK|SP1_BYPASS_MASK);
             }
             if(val1 || val2)
              *value=1;
             else
              *value=0;
        }
        break;
    case KX_HW_ECARD_ROUTING:
        {
         if(hw->is_aps)
         {
          *value=((hw->ecard.spdif0)&0xffff)+((hw->ecard.spdif1&0xffff)<<16);
         }
          else return -1;
        }
        break;
    case KX_HW_ECARD_ADC_GAIN:
        {
         if(hw->is_aps)
         {
          *value=hw->ecard.adc_gain&0xffff;
         }
         else
          return -1;
        }
        break;
    case KX_HW_SPDIF_DECODE:
        if(hw->is_10k2 && !hw->is_edsp)
         *value=(hw->spdif_decode)?1:0;
        else
         return -1;
        break;
    case KX_HW_SPDIF_RECORDING:
        if(hw->is_10k2 && !hw->is_edsp)
         *value=hw->spdif_recording&0x7;
        else
         return -1;
        break;
    case KX_HW_SYNTH_COMPATIBILITY:
        *value=hw->synth_compat;
        break;
    case KX_HW_COMPAT:
        *value=hw->ext_flags;
        break;
    case KX_HW_P16V_PB_ROUTING:
        if(hw->is_a2 && !hw->is_edsp)
        {
         *value=hw->p16v_routing;
        } else return -1;
        break;
    case KX_HW_P16V_REC_ROUTING:
        if(hw->is_a2 && !hw->is_k8)
        {
         *value=hw->p16v_rec_select;
        } else return -1;
        break;
    case KX_HW_A2ZSNB_SOURCE:
        {
          if(!hw->is_zsnb)
           return -1;

          dword fn0=kx_readfn0(hw,HCFG_K2);
          // dword p16v_p17vSC=kx_readp16v(hw,p17vSC,0);
          // dword p16v_p17vSRPRecordSRP=kx_readp16v(hw,p17vSRPRecordSRP,0);

          *value=KX_ZSNB_LINEIN;

          if(!(fn0&HCFG_K2_LINE_MIC_SELECT))
           *value=KX_ZSNB_LINEIN;

          if(fn0&HCFG_K2_LINE_MIC_SELECT)
           *value=KX_ZSNB_MICIN;
        }
        break;
    case KX_HW_K2_AC97:
        if(hw->is_10k2 && !hw->is_edsp)
        {
         int t1=0;

                 if((kx_readfn0(hw,HCFG_K2)&HCFG_K2_AC97_DISCONNECT) && (!hw->is_a2)) // 10k2.1 only
                  t1=1;

                 dword t2=0;
                 
                 if(hw->have_ac97)
                 {
                  t2=kx_ac97read(hw, AC97_REG_MASTER_VOL);
                  if(t2&0x8000) // muted
                   t2=0;
                  else
                   t2=1;
                 }

                 *value=(t2|t1)?1:0;

        } else return -1;
        break;
    case KX_HW_DRUM_CHANNEL:
        *value=hw->drum_channel;
        break;
    default:
        *value=0;
        return -1;
 }
 return 0;
}

int update_hw_parameters(kx_hw *hw);
int update_hw_parameters(kx_hw *hw)
{
  // update S/PDIF notification dynamically here...
  if(hw->ext_flags&KX_HW_NO_SPDIF_NOTIFICATION)
  {
   kx_irq_disable(hw,INTE_GPSPDIFENABLE | INTE_CDSPDIFENABLE);
  }
  else
  {
   kx_irq_enable(hw,INTE_GPSPDIFENABLE | INTE_CDSPDIFENABLE);
  }

  // update KX_HW_P16V_FOLLOW behaviour
  if(hw->ext_flags&KX_HW_P16V_FOLLOW)
  {
   dword tmp = kx_readptr(hw, EHC, 0);
   tmp &= (~EHC_SRCMULTI_I2S_MASK); // clear srcmulti_i2s bits
   // 3538j change: now, clear it [0x400 / EHC_SRCMULTI_I2S_96]
   kx_writeptr(hw, EHC, 0, tmp);
  }
  else
  {
   dword tmp = kx_readptr(hw, EHC, 0);
   tmp |= EHC_SRCMULTI_I2S_96;
   kx_writeptr(hw, EHC, 0, tmp);
  }

  // sb22x
 if(hw->is_51)
 {
  if(!hw->is_10k2)
  {
   int need_hack=0;

   if((dword)(((dword)hw->ac97_id1 << 16) | (dword)hw->ac97_id2)==0x83847658)
   {
    if( (hw->pci_subsys==0x80661102) || (hw->pci_subsys==0x100a1102) )
     need_hack=1;
   }

   if(hw->ext_flags&KX_HW_SB22x)
    need_hack=1-need_hack;
   
   if(need_hack)
   {
    kx_writeptr(hw, PCB, 0, PCB_CNTR|PCB_LFE|PCB_10K2); // sb22x with ac97 codec instead of i2s rear
    kx_ac97write(hw,AC97_REG_CENTER_LFE_MASTER,kx_ac97read(hw,AC97_REG_MASTER_VOL)); // enable/disable center+lfe -> rear
    hw->is_bad_sb22x=1;
   }
   else
   {
    kx_writeptr(hw, PCB, 0, PCB_CNTR|PCB_LFE);
    hw->is_bad_sb22x=0;
   }
  }
 }

 return 0;
}

KX_API(int,kx_set_hw_parameter(kx_hw *hw,int id,dword value))
{
 switch(id)
 {
    case KX_HW_DOO:
          if(hw->is_aps || hw->is_edsp)
           return -1;

      if(!hw->is_10k2) // 10k1:
      {
        if(value)   // set doo
        {
                kx_ac97write(hw,AC97_REG_PCM_VOL,0x9f1f);
                kx_ac97write(hw,AC97_REG_SURROUND_MASTER,0x8080); // FIXME: crea's 8000
                kx_ac97write(hw,AC97_REG_MASTER_VOL,0x9f1f);
                kx_ac97write(hw,AC97_REG_REC_SELECT,0x0505); // StereoMix

                kx_ac97write(hw,AC97_REG_CENTER_LFE_MASTER,0x9f1f); // disable center+lfe -> rear

                // UnMute all analog inputs
                for(int i=AC97_REG_PC_BEEP_VOL;i<=AC97_REG_AUX_VOL;i+=2)
                {
                    word val=kx_ac97read(hw,(byte)i)&0x7fff;
                    kx_ac97write(hw,(byte)i,val);
                }
                if(hw->is_51)
                {
                        dword hcfg;
                        hcfg=kx_readfn0(hw,HCFG_K1);

                    kx_writefn0(hw,HCFG_K1,hcfg|HCFG_GPOUT2_K1);
                }
        }
        else        // set not-doo
        {
                kx_ac97write(hw,AC97_REG_PCM_VOL,0x0808); // 0dB
                kx_ac97write(hw,AC97_REG_SURROUND_MASTER,0x0);
                kx_ac97write(hw,AC97_REG_MASTER_VOL,0x0);
                kx_ac97write(hw,AC97_REG_REC_SELECT,0x404); // LineIn

                kx_ac97write(hw,AC97_REG_CENTER_LFE_MASTER,0x0); // enable center+lfe -> rear

                // Mute all analog inputs
                for(int i=AC97_REG_PC_BEEP_VOL;i<=AC97_REG_AUX_VOL;i+=2)
                {
                    byte reg=(byte)i;
                    word val=(word)0x8000|kx_ac97read(hw,(byte)i);
                    kx_ac97write(hw,reg,val);
                }
                if(hw->is_51)
                {
                        dword hcfg;
                        hcfg=kx_readfn0(hw,HCFG_K1);

                    // set HCFG&=~HCFG_GPOUT2;
                    kx_writefn0(hw,HCFG_K1,hcfg&~HCFG_GPOUT2_K1);
                }
        }
     }
      else  // 10k2:
     {
                if(hw->is_a4) // for a4 value:
                {
                 dword hcfg2=kx_readfn0(hw,HCFG_K2);

                 if(value)
                  hcfg2&=(~HCFG_K2_A4_LINE_DOO);
                 else
                  hcfg2|=HCFG_K2_A4_LINE_DOO;

                 kx_writefn0(hw,HCFG_K2,hcfg2);

                 break;
                }

                if(hw->is_a2) // a2(ex) doesn't need 'doo' mode
                 return -1;

                dword t_flag=HCFG_K2_ENABLE_DIGITAL;

                t_flag|=HCFG_K2_DISABLE_ANALOG; // for a1 only
                 
                if(value)
                {
                    kx_writefn0(hw,HCFG_K2,kx_readfn0(hw,HCFG_K2)|(t_flag));
                }
                else
                {
                    kx_writefn0(hw,HCFG_K2,kx_readfn0(hw,HCFG_K2)&(~(t_flag)));
                }
     }

     kx_dsp_reload_epilog(hw);

     break;
    case KX_HW_SWAP_FRONT_REAR:
        if(hw->is_aps || hw->is_edsp)
         return -1;

        if(value)
         hw->dsp_flags|=KX_DSP_SWAP_REAR;
        else
         hw->dsp_flags&=(~KX_DSP_SWAP_REAR);
        kx_dsp_reload_epilog(hw);

        if(hw->is_a2)
         kx_p16v_set_routing(hw,hw->p16v_routing);

        break;
    case KX_HW_ROUTE_PH_TO_CSW:
        if(hw->is_aps || hw->is_edsp)
         return -1;

        if(value)
         hw->dsp_flags|=KX_DSP_ROUTE_PH_TO_CSW;
        else
         hw->dsp_flags&=(~KX_DSP_ROUTE_PH_TO_CSW);
        kx_dsp_reload_epilog(hw);
        break;
    case KX_HW_SPDIF_FREQ:
        if(hw->is_edsp)
         return -1;

        if(hw->is_10k2)
        {
         if(hw->ac3_pt_state.instance)
         {
          debug(DLIB,"! cannot set dsp freq when ac3passthru is active\n");
          return -1;
         }
                dword spdif_freq=0;

                dword ori=kx_readfn0(hw,HCFG_K2);
                ori&=~(HCFG_K2_UNKNOWN_20|HCFG_K2_UNKNOWN_80);

                switch(value)
                {
                      case 0: // 44100
                         if(!hw->is_a2 || hw->is_k8) // audigy1 or audigy2value/a4
                         {
                          spdif_freq=EHC_SPDIF_44;

                          if(!hw->is_k8)
                           kx_writefn0(hw,HCFG_K2,ori); // a1 only
                         }
                         else
                         {
                          // a2, a2ex
                          spdif_freq=0x0;
                          kx_writefn0(hw,HCFG_K2,ori|HCFG_K2_UNKNOWN_80); // works for a2ex only
                         }
                             break;
                      case 1:
                    spdif_freq=0x0;
                    if(!hw->is_k8)
                         kx_writefn0(hw,HCFG_K2,ori);
                    break;
                  case 2:   
                        spdif_freq=EHC_SPDIF_96;
                        if(!hw->is_k8)
                         kx_writefn0(hw,HCFG_K2,ori|HCFG_K2_UNKNOWN_20);
                        break;
                  default:
                         debug(DLIB,"! Incorrect SPDIF Freq param (%d)\n",value);
                         return -1;
                }
                // FIXME NOW: for 10k2.2: do we really need to set these?..
                kx_writeptr(hw,EHC,0,
                   (kx_readptr(hw,EHC,0)&0xffffff3f)|spdif_freq);

                kx_init_spdif(hw,
                  (value==0)?EHC_SPDIF_44:
                  ((value==1)?EHC_SPDIF_48:EHC_SPDIF_96)); // re-set spdif parameters
            }
             else 
            {
             debug(DLIB,"!! spdif freq is for 10k2 only\n");
             return -1;
            }
        break;
    case KX_HW_SPDIF_BYPASS:
        {
         if(hw->is_aps || hw->is_zsnb || hw->is_edsp)
          return -1;

          if(hw->is_10k2)
          {
                dword val=kx_readfn0(hw,HCFG_K2);
                val&=(~HCFG_K2_BYPASS);
                if(value)
                 val|=HCFG_K2_BYPASS;

                kx_writefn0(hw,HCFG_K2,val);
          }

          {
            dword val=kx_readptr(hw,SOC,0);
            val&=~(SP0_BYPASS_MASK|SP1_BYPASS_MASK);
            kx_writeptr(hw,SOC,0,val|(value&(SP0_BYPASS_MASK|SP1_BYPASS_MASK)));
          } 
        }
        break;
    case KX_HW_AC3_PASSTHROUGH:
        if(!hw->can_passthru /*|| hw->is_edsp*/ || hw->card_frequency!=48000)
        {
         return -1; 
        }
        else
        {
          value&=0x3;
              hw->pt_spdif=value;
              if(value)
              {
                // force 48000 freq
                // it is now set dynamically
                        // if(hw->is_10k2)
                        //  kx_set_hw_parameter(hw,KX_HW_SPDIF_FREQ,1);
                    hw->dsp_flags|=KX_AC3_PASSTHROUGH;
                  }
                  else
                    hw->dsp_flags&=(~KX_AC3_PASSTHROUGH);
                }
        return 0;
        break;
    case KX_HW_ECARD_ROUTING:
        {
         if(hw->is_aps)
         {
          return kx_ecard_set_routing(hw, value);
         }
          else return -1;
        }
        break;
    case KX_HW_ECARD_ADC_GAIN:
        {
         if(hw->is_aps)
         {
          kx_ecard_set_adc_gain(hw, value);
         }
         else
          return -1;
        }
        break;
    case KX_HW_SPDIF_DECODE:
        if(hw->is_10k2 && !hw->is_edsp)
        {
         hw->spdif_decode=value?1:0;

         // note: this may -not- work with a2zs notebook is_cardbus

         if(value)
          kx_writefn0(hw,HCFG_K1,kx_readfn0(hw,HCFG_K1)|HCFG_VBIT_AUTOMUTE_K2);
         else
          kx_writefn0(hw,HCFG_K1,kx_readfn0(hw,HCFG_K1)&(~HCFG_VBIT_AUTOMUTE_K2));
        } else return -1;
        break;
    case KX_HW_SPDIF_RECORDING:
        if(hw->is_10k2 && !hw->is_edsp)
        {
         hw->spdif_recording=value&0x7;

        } else return -1;
        break;
    case KX_HW_SYNTH_COMPATIBILITY:
        hw->synth_compat=value;
        break;
    case KX_HW_COMPAT:
        hw->ext_flags=value;
        update_hw_parameters(hw);
        break;
    case KX_HW_P16V_PB_ROUTING:
        if(hw->is_a2)
        {
          kx_p16v_set_routing(hw,value); // will set hw->p16v_routing
        } else return -1;
        break;
    case KX_HW_P16V_REC_ROUTING:
        if(hw->is_a2)
        {
          kx_p16v_rec_select(hw,value);
        } else return -1;
        break;
    case KX_HW_A2ZSNB_SOURCE:
        {
         if(hw->is_zsnb)
         switch(value)
         {
          case KX_ZSNB_LINEIN:
                kx_writefn0(hw,HCFG_K2,(kx_readfn0(hw,HCFG_K2)&(~HCFG_K2_LINE_MIC_SELECT))); // 0x4
                kx_writei2c(hw, ADC_MUX, ADC_MUX_3); // line-in
                break;
          default:
          case KX_ZSNB_MICIN:
                kx_writefn0(hw,HCFG_K2,(kx_readfn0(hw,HCFG_K2)|HCFG_K2_LINE_MIC_SELECT)); // 0x4
                kx_writei2c(hw, ADC_MUX, ADC_MUX_2); // mic
                break;
         }
         else
          return -1;
        }
        break;
    case KX_HW_K2_AC97:
        if(hw->is_10k2 && !hw->is_edsp)
        {
          if(!hw->is_a2) // 10k2.0 only: disconnect ac97 from the front output
          {
           dword prev=kx_readfn0(hw,HCFG_K2);
           if(value)
            prev|=HCFG_K2_AC97_DISCONNECT;
           else
            prev&=(~HCFG_K2_AC97_DISCONNECT);
           kx_writefn0(hw,HCFG_K2,prev);
          }

          if(hw->have_ac97)
          {
                   word p=kx_ac97read(hw, AC97_REG_MASTER_VOL);
                   if(value)
                    p=0x0; // unmute
                   else
                    p|=0x8000; // mute
                   
                   kx_ac97write(hw,AC97_REG_MASTER_VOL,p);
                  }

        } else return -1;
        break;
    case KX_HW_DRUM_CHANNEL:
        hw->drum_channel=value;
        break;
    default:
        return -1;
 }
 return 0;
}

KX_API(int,kx_set_power_state(kx_hw *hw,int state))
{
 if(state!=hw->power_state)
 {
  switch(state)
  {
   case KX_POWER_NORMAL:
    debug(DLIB,"Power: resuming after standby/suspend...\n");
    if(hw->power_state!=KX_POWER_NORMAL)
    {
     // disable audio as in kx_init():

     kx_hal_init(hw,1); // fast init

     kx_state_restore(hw); // will restore HCFG1,2 and INTE, too

        dword hi=0,lo=0;
        int first=-1;
         // resume any playback
         for(int i = 0; i < KX_NUMBER_OF_VOICES; i++)
         {
           if( (voice_usage(hw->voicetable[i].usage)==VOICE_USAGE_PLAYBACK) ||
               (voice_usage(hw->voicetable[i].usage)==VOICE_USAGE_ASIO))
           {
            if(hw->voicetable[i].state==VOICE_STATE_STARTED) // actually it is currently STOPPED
            {
                if(first==-1)
                    first=i;
                
                if(i<32)
                    lo|=(1<<i);
                else
                    hi|=(1<<(i-32));
                
                hw->voicetable[i].state=VOICE_STATE_STOPPED;
            }
           }
         }
        
        kx_wave_start_multiple(hw, first, lo, hi);
    }
        hw->power_state=KX_POWER_NORMAL;
        break;
   case KX_POWER_SUSPEND:
    if(hw->power_state!=KX_POWER_NORMAL)
    {
     debug(DLIB,"Power: suspend state, but already sleeping - ignored\n");
         hw->power_state=state;
     break;
    }
   case KX_POWER_SLEEP:
    {
        debug(DLIB,"Power: going to sleep...\n");

        hw->power_state=state;

        // stop any playback
        int i;
        for(i = 0; i < KX_NUMBER_OF_VOICES; i++)
        {
          if( (voice_usage(hw->voicetable[i].usage)==VOICE_USAGE_RELEASING) ||
              (voice_usage(hw->voicetable[i].usage)==VOICE_USAGE_MIDI))
          {
           kx_synth_term(hw,i);
          }
           else
          if( (voice_usage(hw->voicetable[i].usage)==VOICE_USAGE_PLAYBACK) ||
              (voice_usage(hw->voicetable[i].usage)==VOICE_USAGE_ASIO))
          {
           if(hw->voicetable[i].state==VOICE_STATE_STARTED)
           {
            kx_wave_stop(hw,i);
            hw->voicetable[i].state=VOICE_STATE_STARTED;
           }
          }
        }

        // start/stop voice may affect timer...
        // save INTE, too
        kx_state_save(hw);

        kx_writeptr(hw,CLIEL,0,0);
        kx_writeptr(hw,CLIEH,0,0);
        if(hw->is_10k2)
        {
          kx_writeptr(hw,HLIEL,0,0);
          kx_writeptr(hw,HLIEH,0,0);
        }
        kx_writefn0(hw,INTE,0);
        kx_writefn0w(hw,TIMER,0x3ff);
        kx_irq_disable(hw,INTE_INTERVALTIMERENB); // just to be sure...
        
        int ch;
        for(ch = 0; ch < KX_NUMBER_OF_VOICES; ch++)
            kx_writeptr(hw, DCYSUSV, ch, 0);
        
        for(ch = 0; ch < KX_NUMBER_OF_VOICES; ch++) 
        {
            kx_writeptr_multiple(hw, ch,
                                 VTFT, 0,
                                 CVCF, 0,
                                 PTAB, 0,
                                 CPF, 0,
                                 IP, 0,
                                 IFATN, 0xffff,
                                 REGLIST_END);
        }
        
        kx_dsp_stop(hw);

        // clear microcode
        for(i = 0; i < hw->microcode_size; i++)
        {
            hwOP(i,ACC3,C_0-0x2000+(hw->is_10k2?0x80:0),
                        C_0-0x2000+(hw->is_10k2?0x80:0),
                        C_0-0x2000+(hw->is_10k2?0x80:0),
                        C_0-0x2000+(hw->is_10k2?0x80:0));
        }

        // clear GPRs, TRAMs...
        for(i = 0; i < 256; i++)
        {
            kx_writeptr(hw, E10K1_GPR_BASE+i,0,0);
            kx_writeptr(hw, TANKMEMADDRREGBASE+i,0,0);
        }

        if(hw->is_10k2)
         for(i=0; i<512; i++)
          kx_writeptr(hw, E10K2_GPR_BASE+i, 0, 0);

        kx_writeptr(hw, PTBA, 0, 0);
        kx_writeptr(hw, TRBA, 0, 0);

        kx_writefn0(hw, HCFG_K1, HCFG_LOCKSOUNDCACHE | HCFG_LOCKTANKCACHE_MASK);

        kx_disable_analog(hw,1);
        }
        break;
   default:
    return -2;
  }
 } else return -1; // same state
 return 0;
}

KX_API(int,kx_mute(kx_hw *hw))
{
 kx_writefn0(hw,HCFG_K1,kx_readfn0(hw,HCFG_K1)&(~HCFG_AUDIOENABLE));
 return 0;
}


KX_API(int,kx_unmute(kx_hw *hw))
{
 kx_writefn0(hw,HCFG_K1,kx_readfn0(hw,HCFG_K1)|HCFG_AUDIOENABLE);
 return 0;
}


#if defined(_MSC_VER)
#pragma code_seg()
// this is required, since we call send_message() here
#endif
void system_timer_func(void *data,int /*what*/);
void system_timer_func(void *data,int /*what*/)
{
 // 'what' is LLA_NOTIFY_SYSTEM or LLA_NOTIFY_TIMER

 kx_hw *hw=(kx_hw *)data;

 if(hw->is_edsp) // ignore GPIO
 {
  byte card=(byte)kx_readfpga(hw,EMU_HANA_OPTION_CARDS);
  if(hw->gp_ins!=card)
  {
    debug(DLIB,"kx: fpga card status changed\n");

    hw->cb.send_message(hw->cb.call_with,KX_SYSEX_SIZE,KX_SYSEX_EDSP_CFG);
  }
  hw->gp_ins=card;
  return;
 }

 byte tmp=kx_get_gp_inputs(hw);

 if(hw->is_a2ex)
 {
   // bit 8 bit is unstable...
   tmp&=(~0x8);
   hw->gp_ins&=(~0x8);
 }

 if(hw->gp_ins!=tmp)
 {
  debug(DLIB,"GPIO msg [%x o:%x]\n",tmp,hw->gp_ins);

  char msg[KX_SYSEX_SIZE];
  memcpy(msg,KX_SYSEX_GPIO,KX_SYSEX_SIZE);
  msg[KX_SYSEX_GPIO_POS1]=tmp&0x7f;
  msg[KX_SYSEX_GPIO_POS2]=(tmp&0x80)?1:0;

  hw->cb.send_message(hw->cb.call_with,KX_SYSEX_SIZE,msg);
  hw->gp_ins=tmp;
 }

 // only if there's MIDI activity...
 // FIXME NOW: bad idea at the moment [celine] 3537rc1
 /*
 if(hw->midi[0] || hw->midi[1])
  kx_steal_voices(hw,VOICE_USAGE_MIDI,KX_STEAL_MAINTENANCE);
 */
}

KX_API(int,kx_get_spdif_i2s_status(kx_hw *hw,kx_spdif_i2s_status *st))
{
 st->spdif.channel_status_a=kx_readptr(hw,CDCS,0);
 st->spdif.srt_status_a=kx_readptr(hw,CDSRCS,0);
 st->spdif.channel_status_b=kx_readptr(hw,GPSCS,0);
 st->spdif.srt_status_b=kx_readptr(hw,GPSRCS,0);

 if(hw->is_10k2)
 {
   st->spdif.channel_status_c=kx_readptr(hw,GP2SCS_10K2,0);
 }
 else
   st->spdif.channel_status_c=0;

 st->spdif.srt_status_c=kx_readptr(hw,ZVSRCS,0);

 if(hw->is_a2)
 {
  st->spdif.channel_status_a_x=kx_readptr(hw,CDCS,1);
  st->spdif.channel_status_b_x=kx_readptr(hw,GPSCS,1);
  st->spdif.channel_status_c_x=kx_readptr(hw,GP2SCS_10K2,1);
 }

 st->i2s.srt_status_0=kx_readptr(hw,SRT3,0);
 st->i2s.srt_status_1=kx_readptr(hw,SRT4,0);
 st->i2s.srt_status_2=kx_readptr(hw,SRT5,0);

 st->spdif.scs0=kx_readptr(hw,SCS0,0);
 st->spdif.scs1=kx_readptr(hw,SCS1,0);
 st->spdif.scs2=kx_readptr(hw,SCS2,0);

 if(hw->is_a2)
 {
  st->spdif.scs0x=kx_readptr(hw,SCS0,0x1);
  st->spdif.scs1x=kx_readptr(hw,SCS1,0x1);
  st->spdif.scs2x=kx_readptr(hw,SCS2,0x1);

  if(hw->is_k8)
   st->p16v=0; // fixme: p17v
  else
   st->p16v=kx_readp16v(hw,p16vRecRate,0);
 }
 else
 {
  st->spdif.scs0x=0x0;
  st->spdif.scs1x=0x0;
  st->spdif.scs2x=0x0;
 }

 dword d;
 if(kx_get_hw_parameter(hw,KX_HW_SPDIF_FREQ,&d)==0)
  st->spdif.spo_sr=(int)d;
 else
  st->spdif.spo_sr=1; // 48000

 return 0;
}


// x(n+1)=x(n)+S*(T-x(n))
#define acerca(n,c,t) { \
    delta=(( ((t)<<4) - ((c)<<4) )>>1); \
    if(delta>sense) delta=sense;    \
    if(delta<-sense) delta=-sense;  \
        (n)=( ((c)<<4) + delta )>>4; }

KX_API(int,kx_set_fx_amount(kx_hw *hw,int where,int num,int chn_))
{
 int perform=1;
 if( (voice_usage(hw->voicetable[num+chn_].usage)==VOICE_USAGE_MIDI) ||
     (voice_usage(hw->voicetable[num+chn_].usage)==VOICE_USAGE_RELEASING) ||
     (hw->ext_flags&KX_HW_WAVE_DONT_SMOOTH))
     perform=0;

 kx_voice *voice=&hw->voicetable[num+chn_];

 dword reg;
 dword data;
 dword cur;
 int delta;

 int sense=(hw->is_10k2?(10<<4):(4<<4));

 if(chn_!=0 && where!=3)
 {
  debug(DERR,"error with kx_set_fx_amount [%d %d]\n",where,chn_);
 }

   if(where&3) // sendA + sendB
   {
          reg=PTAB_SENDAB;

          if(perform)
          {
              int new_sa,new_sb;
              int cur_sa,cur_sb;

              cur=kx_readptr(hw,reg,num+chn_);

              cur_sa=(cur>>8)&0xff;
              cur_sb=cur&0xff;

              acerca(new_sa,cur_sa,kx_fxamount(hw,voice->param.send_a));
              acerca(new_sb,cur_sb,kx_fxamount(hw,voice->param.send_b));

              if(new_sa!=cur_sa || new_sb!=cur_sb)
               voice->param.interpolate|=where;
              else
               voice->param.interpolate&=(~where);

              data=(( new_sa << 8) | new_sb);
          }
          else
          {
           data=(( voice->param.send_a << 8) | voice->param.send_b);
          }

          kx_writeptr(hw,reg,num+chn_,data);
          // debug(DLIB,"[fxbus]: (chn: %d/%d) %d [%x] -> %x [%d]\n",num,chn_,where,reg,data,voice->param.interpolate);
          where&=(~3);
   }
   if(where&1) // sendA
   {
      reg=PTAB_FXSENDAMOUNT_A;

      if(perform)
      {
            cur=kx_readptr(hw,reg,num+chn_);

            acerca(data,cur,kx_fxamount(hw,voice->param.send_a));

            if(data!=cur)
             voice->param.interpolate|=where;
            else
             voice->param.interpolate&=(~where);
          }
           else data=voice->param.send_a;

          kx_writeptr(hw,reg,num+chn_,data);
          // debug(DLIB,"[fxbus]: (chn: %d/%d) %d [%x] -> %x [%d]\n",num,chn_,where,reg,data,voice->param.interpolate);
          where&=(~1);
   }
   if(where&2) // sendB
   {
      reg=PTAB_FXSENDAMOUNT_B;

      if(perform)
      {
             cur=kx_readptr(hw,reg,num+chn_);

             acerca(data,cur,kx_fxamount(hw,voice->param.send_b));

             if(data!=cur)
              voice->param.interpolate|=where;
             else
              voice->param.interpolate&=(~where);
          }
           else data=voice->param.send_b;

          kx_writeptr(hw,reg,num+chn_,data);
          // debug(DLIB,"[fxbus]: (chn: %d/%d) %d [%x] -> %x [%d]\n",num,chn_,where,reg,data,voice->param.interpolate);
          where&=(~2);
    }
    if(where&4) // sendC
    {
      reg=SCSA_FXSENDAMOUNT_C;

      if(perform)
      {
          cur=kx_readptr(hw,reg,num+chn_);

          acerca(data,cur,kx_fxamount(hw,voice->param.send_c));

          if(data!=cur)
           voice->param.interpolate|=where;
          else
           voice->param.interpolate&=(~where);
          }
           else data=voice->param.send_c;

          kx_writeptr(hw,reg,num+chn_,data);
          // debug(DLIB,"[fxbus]: (chn: %d/%d) %d [%x] -> %x [%d]\n",num,chn_,where,reg,data,voice->param.interpolate);
          where&=(~4);
    }
    if(where&8) // sendD
    {
      reg=SDL_FXSENDAMOUNT_D;

      if(perform)
      {
          cur=kx_readptr(hw,reg,num+chn_);

          acerca(data,cur,kx_fxamount(hw,voice->param.send_d));

          if(data!=cur)
           voice->param.interpolate|=where;
          else
           voice->param.interpolate&=(~where);
          } else data=voice->param.send_d;

          kx_writeptr(hw,reg,num+chn_,data);
          // debug(DLIB,"[fxbus]: (chn: %d/%d) %d [%x] -> %x [%d]\n",num,chn_,where,reg,data,voice->param.interpolate);
          where&=(~8);
    }
    if(where&0x10) // send E
    {
      reg=FXSENDAMOUNT_E;

      if(perform)
      {
          cur=kx_readptr(hw,reg,num+chn_);

          acerca(data,cur,kx_fxamount(hw,voice->param.send_e));

          if(data!=cur)
           voice->param.interpolate|=where;
          else
           voice->param.interpolate&=(~where);
          } else data=voice->param.send_e;

          kx_writeptr(hw,reg,num+chn_,data);
          // debug(DLIB,"[fxbus]: (chn: %d/%d) %d [%x] -> %x [%d]\n",num,chn_,where,reg,data,voice->param.interpolate);
          where&=(~0x10);
    }
    if(where&0x20) // send F
    {
      reg=FXSENDAMOUNT_F;

      if(perform)
      {
          cur=kx_readptr(hw,reg,num+chn_);

          acerca(data,cur,kx_fxamount(hw,voice->param.send_f));

          if(data!=cur)
           voice->param.interpolate|=where;
          else
           voice->param.interpolate&=(~where);
          } else data=voice->param.send_f;

          kx_writeptr(hw,reg,num+chn_,data);
          // debug(DLIB,"[fxbus]: (chn: %d/%d) %d [%x] -> %x [%d]\n",num,chn_,where,reg,data,voice->param.interpolate);
          where&=(~0x20);
    }
    if(where&0x40) // send G
    {
      reg=FXSENDAMOUNT_G;

      if(perform)
      {
          cur=kx_readptr(hw,reg,num+chn_);

          acerca(data,cur,kx_fxamount(hw,voice->param.send_g));

          if(data!=cur)
           voice->param.interpolate|=where;
          else
           voice->param.interpolate&=(~where);
          } else data=voice->param.send_g;

          kx_writeptr(hw,reg,num+chn_,data);
          // debug(DLIB,"[fxbus]: (chn: %d/%d) %d [%x] -> %x [%d]\n",num,chn_,where,reg,data,voice->param.interpolate);
          where&=(~0x40);
    }
    if(where&0x80) // send H
    {
      reg=FXSENDAMOUNT_H;

      if(perform)
      {
          cur=kx_readptr(hw,reg,num+chn_);

          acerca(data,cur,kx_fxamount(hw,voice->param.send_h));

          if(data!=cur)
           voice->param.interpolate|=where;
          else
           voice->param.interpolate&=(~where);
          } else data=voice->param.send_h;

          kx_writeptr(hw,reg,num+chn_,data);
          // debug(DLIB,"[fxbus]: (chn: %d/%d) %d [%x] -> %x [%d]\n",num,chn_,where,reg,data,voice->param.interpolate);
          where&=(~0x80);
   }
   if(where)
   {
    debug(DERR,"internal error in set_fx_amount (%d; %d; %d)\n",where,num,chn_);
    return -1;
   }
 return 0;
}

KX_API(void, kx_writep16v(kx_hw *hw, dword reg, dword chn, dword data))
{
    unsigned long flags;
        kx_lock_acquire(hw,&hw->hw_lock, &flags);
        outpd(hw->port + pPTR,pREG(reg)|chn);
        outpd(hw->port + pDATA,data);
        kx_lock_release(hw,&hw->hw_lock, &flags);
}

KX_API(dword, kx_readp16v(kx_hw * hw, dword reg,dword chn))
{
    unsigned long flags;
        kx_lock_acquire(hw,&hw->hw_lock, &flags);
        outpd(hw->port + pPTR,pREG(reg)|chn);
        dword val = inpd(hw->port + pDATA);
        kx_lock_release(hw,&hw->hw_lock, &flags);

        return val;
}

KX_API(void,kx_disable_analog(kx_hw *hw,int disable))
{
  if(disable)
  {
        if(hw->is_10k2)
        {
         dword tmp_hcfg_k2=0;
         tmp_hcfg_k2=kx_readfn0(hw,HCFG_K2);

         if(hw->is_a2)
           tmp_hcfg_k2&=(~HCFG_K2_DISABLE_ANALOG);
         else
           tmp_hcfg_k2|=HCFG_K2_DISABLE_ANALOG;

          kx_writefn0(hw, HCFG_K2, tmp_hcfg_k2);
        }
  }
  else
  {
        debug(DERR,"kx_disable_analog: should never happen!\n");
  }
}


KX_API(int,kx_upload_fpga_firmware(kx_hw *hw,byte *data,int size))
{
   unsigned long flags;
   kx_lock_acquire(hw,&hw->hw_lock, &flags);

   dword tmp;

   outpd(hw->port+HCFG_K2, 0x00); // Set PGMN low for 1uS
   tmp = inpd(hw->port+HCFG_K2);
   hw->cb.usleep(100);
   outpd(hw->port+HCFG_K2, 0x80); // Leave bit 7 set during netlist setup
   tmp = inpd(hw->port+HCFG_K2);
   hw->cb.usleep(100); // Allow FPGA memory to clean

   while(size--) 
   {
     byte value=*data; data++;

     for(int i = 0; i < 8; i++) 
     {
        byte reg = 0x80;
        if(value & 0x1)
           reg|=0x20;

        value = value >> 1;   
        outpd(hw->port+HCFG_K2, reg);
        tmp = inpd(hw->port+HCFG_K2);
        outpd(hw->port+HCFG_K2, reg | 0x40);
        tmp = inpd(hw->port+HCFG_K2);
     }
   }

   // After programming, set GPIO bit 4 high again
   outpd(hw->port+HCFG_K2, 0x10);
   tmp = inpd(hw->port+HCFG_K2);

   kx_lock_release(hw,&hw->hw_lock, &flags);

   return 0;
}

KX_API(int,kx_set_clock(kx_hw *hw,dword freq))
{
    dword val=kx_readfn0(hw,HCFG_K1);

    if(freq==44100)
    {
        // disable 48000-only settings FIXME
        kx_set_hw_parameter(hw,KX_HW_AC3_PASSTHROUGH,0);

        if(hw->is_edsp)
        {
         kx_writefpga(hw,EMU_HANA_UNMUTE,EMU_MUTE);
         kx_writefpga(hw,EMU_HANA_DEFCLOCK,EMU_HANA_DEFCLOCK_44_1K);
         kx_writefpga(hw,EMU_HANA_WCLOCK,EMU_HANA_WCLOCK_INT_44_1K | EMU_HANA_WCLOCK_1X);
         kx_writefpga(hw,EMU_HANA_DOCK_LEDS_2,EMU_HANA_DOCK_LEDS_2_44K | EMU_HANA_DOCK_LEDS_2_LOCK);
         hw->cb.usleep(20*1000);
         kx_writefpga(hw,EMU_HANA_UNMUTE,EMU_UNMUTE);
        }

        val|=HCFG_44K_K2;
        kx_writefn0(hw,HCFG_K1,val);

        hw->card_frequency=freq;

        debug(DLIB,"kx_set_clock: set to %d\n",freq);

        return 0;
    }
    else
     if(freq==48000)
     {
         val&=(~HCFG_44K_K2);
         kx_writefn0(hw,HCFG_K1,val);

         if(hw->is_edsp)
         {
          kx_writefpga(hw,EMU_HANA_UNMUTE,EMU_MUTE);
          kx_writefpga(hw,EMU_HANA_DEFCLOCK,EMU_HANA_DEFCLOCK_48K);
          kx_writefpga(hw,EMU_HANA_WCLOCK,EMU_HANA_WCLOCK_INT_48K | EMU_HANA_WCLOCK_1X);
          kx_writefpga(hw,EMU_HANA_DOCK_LEDS_2,EMU_HANA_DOCK_LEDS_2_48K | EMU_HANA_DOCK_LEDS_2_LOCK );
          hw->cb.usleep(20*1000);
          kx_writefpga(hw,EMU_HANA_UNMUTE,EMU_UNMUTE);
         }

         hw->card_frequency=freq;

         debug(DLIB,"kx_set_clock: set to %d\n",freq);

         return 0;
     }
     else
      return -1;
}
