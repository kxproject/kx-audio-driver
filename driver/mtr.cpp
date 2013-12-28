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

// kX Multitrack device
// --------------------
KX_API(int,kx_mtrec_start(kx_hw *hw))
{
 if(hw->mtr_buffer.dma_handle)
 {
  kx_writeptr(hw,FXIDX,0,0);
  kx_writeptr(hw,FXBA,0,hw->mtr_buffer.dma_handle);

  dword sz;
  switch(hw->mtr_buffer_size)
  {
        case 384: sz=ADCBS_BUFSIZE_384; break;
        case 448: sz=ADCBS_BUFSIZE_448; break;
        case 512: sz=ADCBS_BUFSIZE_512; break;
        case 640: sz=ADCBS_BUFSIZE_640; break;
        case 768: sz=ADCBS_BUFSIZE_768; break;
        case 896: sz=ADCBS_BUFSIZE_896; break;
        case 1024: sz=ADCBS_BUFSIZE_1024; break;
        case 1280: sz=ADCBS_BUFSIZE_1280; break;
        case 1536: sz=ADCBS_BUFSIZE_1536; break;
        case 1792: sz=ADCBS_BUFSIZE_1792; break;
        case 2048: sz=ADCBS_BUFSIZE_2048; break;
        case 2560: sz=ADCBS_BUFSIZE_2560; break;
        case 3072: sz=ADCBS_BUFSIZE_3072; break;
        case 3584: sz=ADCBS_BUFSIZE_3584; break;
        case 4096: sz=ADCBS_BUFSIZE_4096; break;
        case 5120: sz=ADCBS_BUFSIZE_5120; break;
        case 6144: sz=ADCBS_BUFSIZE_6144; break;
        case 7168: sz=ADCBS_BUFSIZE_7168; break;
        case 8192: sz=ADCBS_BUFSIZE_8192; break;
        case 10240: sz=ADCBS_BUFSIZE_10240; break;
        case 12288: sz=ADCBS_BUFSIZE_12288; break;
        case 14366: sz=ADCBS_BUFSIZE_14366; break;
        case 16384: sz=ADCBS_BUFSIZE_16384; break;
        case 20480: sz=ADCBS_BUFSIZE_20480; break;
        case 24576: sz=ADCBS_BUFSIZE_24576; break;
        case 28672: sz=ADCBS_BUFSIZE_28672; break;
        case 32768: sz=ADCBS_BUFSIZE_32768; break;
        case 40960: sz=ADCBS_BUFSIZE_40960; break;
        case 49152: sz=ADCBS_BUFSIZE_49152; break;
        case 57344: sz=ADCBS_BUFSIZE_57344; break;
        default:
        case 65536: sz=ADCBS_BUFSIZE_65536; break;
  }

  kx_writeptr(hw,FXBS,0,sz);

  // enable IRQs
  kx_irq_enable(hw,INTE_EFXBUFENABLE);

  debug(DASIO,"mtrec_start\n");
  return 0;
 }
 else
 {
  debug(DLIB,"!!! mtrec_start: invalid call\n");
  return -1;
 }
}

KX_API(int,kx_mtrec_stop(kx_hw *hw))
{
 kx_writeptr(hw,FXBS,0,0);

 // disable IRQs
 kx_irq_disable(hw,INTE_EFXBUFENABLE);

 debug(DASIO,"mtrec_stop\n");
 return 0;
}

KX_API(int,kx_mtrec_select(kx_hw *hw,dword flag,dword flag2)) // flag2-10k2 only
{
 if(hw->is_10k2)
 {
  kx_writeptr(hw,FXWCL,0,/*flag2*/0); // in general, it is ignored: FXBuses are not recorded
  kx_writeptr(hw,FXWCH,0,flag);  // temporary buses ARE recorded
 }
 else
 {
  kx_writeptr(hw,FXWC_K1,0,(flag&0xffff)<<16); // don't know
 }
 return 0;
}
