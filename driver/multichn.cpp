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

KX_API(int,kx_allocate_multichannel(kx_hw *hw,int bps,int rate,int flag,kx_voice_buffer *buffer,int routing))
{
   int i=-1;

   if(bps==16)
   {
         i=kx_wave_open(hw, buffer,
           VOICE_USAGE_ASIO|VOICE_FLAGS_MONO|VOICE_FLAGS_16BIT|
           flag, 
           rate,routing);

         debug(DLIB,"asio: allocating voice %d; latency: %d -- 16-bit\n",
          i,buffer->notify);

         if(i>=0) // ok
         {
               if(hw->is_10k2)
               {
                 kx_wave_setup_b2b(hw,i,hw->cb.def_routings[routing],1,0); // high, A
               }
               else
               {
                 // 10k1:
                 kx_wave_set_parameter(hw,i,0,KX_VOICE_SEND_A,0xff);
                 kx_wave_set_parameter(hw,i,0,KX_VOICE_SEND_B,0x0);
                 kx_wave_set_parameter(hw,i,0,KX_VOICE_SEND_C,0x0);
                 kx_wave_set_parameter(hw,i,0,KX_VOICE_SEND_D,0x0);
               }

               debug(DLIB,"asio: alloc_output(chn=%d; hw=%d)\n",routing-DEF_ASIO_ROUTING,i); // hack [chn #]
         }
         else
         {
         	debug(DLIB,"!! kx_allocate_multichannel failed (ret=%d)\n",i);
         }
   }
   else
    if(bps==32)
    {
         i=kx_wave_open(hw, buffer,
           VOICE_USAGE_ASIO|VOICE_FLAGS_STEREO|VOICE_FLAGS_16BIT|
           flag, 
           rate,routing);

         debug(DLIB,"asio: allocating voices %d,%d; latency: %d -- 32-bit\n",
          i,i+1,buffer->notify);

         if(i>=0) // ok
         {
               // [i] - lowest
               // [i+1] - highest

               // LOWEST
               kx_wave_setup_b2b(hw,i,hw->cb.def_routings[routing],0,1); // low, B
               // HIGHEST part:
               kx_wave_setup_b2b(hw,i+1,hw->cb.def_routings[routing],1,0); // high, A
               
               debug(DLIB,"asio: alloc_output(chn=%d; hw=%d)\n",routing-DEF_ASIO_ROUTING,i); // hack for chn #
         }
         else
         {
         	debug(DLIB,"!! kx_allocate_multichannel failed (ret=%d)\n",i);
         }
    }
     else
     {
      debug(DWDM,"!! kx_allocate_multichannel failed: invalid bps {%d}\n",bps);
      i=-1;
     }

     return i;
}
