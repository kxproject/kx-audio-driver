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


#ifndef CALC_H_
#define CALC_H_

dword kx_sr_coeff(kx_hw *hw,dword);
dword kx_srToPitch(dword);
dword kx_samplerate_to_linearpitch(dword samplingrate);

extern byte kx_fxamount_tbl[256];

inline byte kx_fxamount(kx_hw *hw,dword k1_amount)
{
 if(hw->is_10k2)
 {
   // pre-3543:
   // return kx_fxamount_tbl_old[(k1_amount>0xff?0xff:k1_amount)];

   // 3544: handle one special case
   if(k1_amount==0xffffffff)
    return 0xff;

   return kx_fxamount_tbl[(k1_amount>0xff?0xff:k1_amount)];
 }
 else
  return (byte)k1_amount;
}

#endif
