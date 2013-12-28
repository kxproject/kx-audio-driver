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

#define AC3_CODESIZE    4

static dsp_code ac3_off[AC3_CODESIZE]=
{
 { ACC3,C_0,C_0,C_0,C_0 },
 { ACC3,C_0,C_0,C_0,C_0 },
 { ACC3,C_0,C_0,C_0,C_0 },
 { ACC3,C_0,C_0,C_0,C_0 }
};

static dsp_code ac3_on[AC3_CODESIZE]=
{
    // 10k8 gets data 'as is'
    //      highest: send: 0xe0, vol: 0x8000 -> ((data<<1)>>1) -> nop
    // 10k2 gets data >>2
    //      highest: send: 0xff, vol: 0x1000 -> ((data<<2)>>4) -> need to <<2 in DSP
    // 0x8004: = 0xffff0000,"andmask"
    { MACINTS,0x8000/*tmpl*/,	C_0,			0x0/*inputL*/,	C_4|C_10 	},	// note: C_4|C_10: actual constant is chosen below
    { ANDXOR, 0x0/*outl*/,		0x8000/*tmpl*/, 0x8004,			C_0			},
    { MACINTS,0x8001/*tmpr*/,	C_0,			0x0/*inputR*/,	C_4|C_10 	},
    { ANDXOR, 0x0/*outr*/,		0x8001/*tmpr*/,	0x8004, 		C_0     	}
};

#if defined(_MSC_VER)
#pragma code_seg()
// this is required, since we call send_message() here
#endif
KX_API(int,kx_set_passthru(kx_hw *hw,void *instance,int onoff))
{
 if(hw->ac3_pt_state.method)
 {
  if(hw->ac3_pt_state.instance!=instance)
  {
   debug(DERR,"kx_set_passthru: invalid instance (%08x %08x; %d)\n",
    instance,hw->ac3_pt_state.instance,hw->ac3_pt_state.method);
   return -1;
  }
 }
 else
 {
   debug(DERR,"kx_set_passthru: invalid sequence (%08x %08x; %d)\n",
    instance,hw->ac3_pt_state.instance,hw->ac3_pt_state.method);
   return -2;
 }

 struct list *item;

 unsigned long flags=0;
 kx_lock_acquire(hw,&hw->dsp_lock, &flags);

 dword i;

 int have_non_epilog=0;
 int once=0;

 // find & notify only
 for_each_list_entry(item, &hw->microcodes) 
 {
       dsp_microcode *m;
       m = list_item(item, dsp_microcode, list);
       if(!m)
        continue;

       int found=0;

       if(
           strcmp(m->guid,"459fd76d-a110-438e-908f-fcd1872d5a06")==0   || // passthru
           strcmp(m->guid,"9812dce7-5738-48aa-b700-927caec84053")==0   || // passthru_old
           strcmp(m->guid,"64824522-f847-4bca-ac45-7a58c321d4e3")==0      // passthru_x
         )
       {
         have_non_epilog=1;
         found=1;
       }

       if(!once && 
          ((strcmp(m->guid,"ceffc302-ea28-44df-873f-d3df1ba31736")==0) ||  // epilog X
          found))
       {
         once=1;

         switch(onoff)
         {
          case 0: // restore 'original' state
           if(hw->pt_spdif) // it may be 0 if ac3 pt is disabled/re-enabled on the fly (3551 change)
           {
            if(hw->spdif_states[hw->pt_spdif-1]!=0xffffffff)
             kx_writeptr(hw,SCS0 + hw->pt_spdif-1, 0, hw->spdif_states[hw->pt_spdif-1]);
           }

           hw->cb.send_message(hw->cb.call_with,KX_SYSEX_SIZE,KX_SYSEX_AC3PASSTHRU_OFF);
           break;

         case 1:
           // some updates here...
           hw->cb.send_message(hw->cb.call_with,KX_SYSEX_SIZE,KX_SYSEX_AC3PASSTHRU_ON);

           if(hw->pt_spdif) // it may be 0 (3551 change)
           {
            if(hw->spdif_states[hw->pt_spdif-1]!=0xffffffff)
             kx_writeptr(hw,SCS0 + hw->pt_spdif-1, 0, hw->spdif_states[hw->pt_spdif-1]);

            hw->spdif_states[hw->pt_spdif-1] = kx_readptr(hw, SCS0 + hw->pt_spdif-1, 0);
            kx_writeptr(hw,SCS0 + hw->pt_spdif-1, 0, hw->spdif_states[hw->pt_spdif-1] | SCS_NOTAUDIODATA);
           }
           break;
         }
         // break;
         // process all...
       }
 }

 // now, perform method-dependent initialization

 switch(hw->ac3_pt_state.method)
 {
  case KX_AC3_PASSTHRU_GENERIC:

              // find & patch
              if(!have_non_epilog)
              {
                for_each_list_entry(item, &hw->microcodes) 
                {
                    dsp_microcode *m;
                    m = list_item(item, dsp_microcode, list);
                    if(!m)
                     continue;

                    if(strcmp(m->guid,"ceffc302-ea28-44df-873f-d3df1ba31736")==0)  // patch epilog only
                    {
                      switch(onoff)
                      {
                       case 0: // restore 'original' state
                        memcpy(&m->code[m->code_size/sizeof(dsp_code)-AC3_CODESIZE],ac3_off,sizeof(ac3_off));
                        for(i=(dword)(m->code_size/sizeof(dsp_code)-AC3_CODESIZE);i<m->code_size/sizeof(dsp_code);i++)
                        {
                           upload_instruction(hw,m,i);
                        }
                        break;
                      case 1:
                        // some updates here...

                        word tmp;
                        switch(hw->pt_spdif)
                        {
                         case 0:
                         default:
                          debug(DERR,"!! internal error in set_passthru\n");
                          kx_lock_release(hw,&hw->dsp_lock,&flags);
                          return -2;
                         case 1: tmp=0x2302; break;
                         case 2: tmp=0x2304; break;
                         case 3: tmp=0x2306; break;
                        }

                        int to_swap=0;
                        if((hw->is_a2 || (hw->is_a2==0 && hw->cb.subsys==0x00521102)) && !hw->is_k8)
                        {
                         // any audigy2-based board + sb0160 [a1]
                         // FIXME !! need to check if is_k8 are affected, too
                         to_swap=1;
                        }

                        if(hw->ext_flags&KX_HW_FORCE_SPDIF_SWAP)
                        	to_swap=1-to_swap;

                        // inputs:
                        ac3_on[0].x=(word)KX_GET_SEND_A(hw->cb.def_routings[DEF_AC3PASSTHROUGH_ROUTING]);
                        ac3_on[2].x=(word)KX_GET_SEND_B(hw->cb.def_routings[DEF_AC3PASSTHROUGH_ROUTING]);

                        // outputs:
                        if(to_swap)
                        {
                                ac3_on[1].r=tmp+1;
                                ac3_on[3].r=tmp;
                        }
                        else
                        {
                                // pre-3538e; no-swap
                                ac3_on[1].r=tmp;
                                ac3_on[3].r=tmp+1;
                        }

                        // multiplier:
                        if(hw->can_k8_passthru)
                        {
                            ac3_on[0].y=C_4; ac3_on[2].y=C_4;
                        }
                        else
                        {
                            ac3_on[0].y=C_10; ac3_on[2].y=C_10;
                        }

                        debug(DLIB,"AC-3 passthru mode, swap set to %d\n",to_swap);

                        memcpy(&m->code[m->code_size/sizeof(dsp_code)-AC3_CODESIZE],ac3_on,sizeof(ac3_on));

                        for(i=(dword)(m->code_size/sizeof(dsp_code)-AC3_CODESIZE);i<m->code_size/sizeof(dsp_code);i++)
                        {
                           upload_instruction(hw,m,i);
                        }
                        break;
                      }
                      // patch only -one- effect
                      break;
                    }
              }
              } // have non-epilog

              // for 'ac3passthru' plugin no initialization is required
              break;
   case KX_AC3_PASSTHRU_XTRAM:
        {
            // turn on/off irq generation
            int ok=0;
                        for_each_list_entry(item, &hw->microcodes) 
                        {
                              dsp_microcode *m;
                              m = list_item(item, dsp_microcode, list);
                              if(!m)
                               continue;
                              if(m==hw->ac3_pt_state.m)
                              {
                                ok=1;

                                for(int ii=0;ii<m->code_size;ii++)
                                {
                                 if(m->code[ii].r==0x205a) // 'IRQ'
                                 {
                                  if(onoff)
                                   m->code[ii].a=0x204e;
                                  else
                                   m->code[ii].a=0x2040;

                                  upload_instruction(hw,m,ii);

                                  debug(DLIB,"set_passthru: dynamic microcode update @ offset [%d]\n",ii);
                                  break;
                                 }
                                }
                                // FIXME: set outputs to 0/value, too!!!
                              }
                        }
                        if(ok==0)
                         debug(DERR,"set_passthru: AC-3 passthru microcode was unloaded!\n");
        }
        break;
   default:
    debug(DERR,"set_passthru: invalid internal state! [%d]\n",hw->ac3_pt_state.method);
    break;
 }

 kx_lock_release(hw,&hw->dsp_lock,&flags);

 /*
  // original code found in p16v.cpp:
  // Setup SRCSel (Enable Spdif,I2S SRCMulti; 10k2.ac97 is sent to ac97, not src48)
  if(!hw->is_k8)
   kx_writep16v(hw, p16vSRCSel, 0, p16vSRCSel_SPDIF|p16vSRCSel_I2S|((hw->lack_ac97?p16vSRCSel_AC97:0))); // reg: 0x60
        //   srcmulti(i2s,spdif)->i2s,spdif [not 10k2.i2s/10k2.spdif]; 10k2.ac97->ac97
        //   for ac3 passthru: spdif should be re-routed to 10kx
  else
   kx_writep16v(hw, p17vSRCSel, 0, p17vSRCSel_SPDIF|p17vSRCSel_I2S|((hw->lack_ac97?p17vSRCSel_AC97:0))); // reg: 0x60 
        // [10kx.ac97-> ac97; mixer_spdif->spdif; mixer_i2s->i2s]
        // srp_rec_i2sin_to_10k2_sel: 0 [i2sin goes to fxengine dsp 0x48..0x4d]
 */

 // p16v should be re-routed (spdif issue)
 if(hw->is_a2)
 {
  if(!hw->is_k8)
  {
     if(onoff)
      kx_writep16v(hw, p16vSRCSel, 0, p16vSRCSel_I2S|((hw->lack_ac97?p17vSRCSel_AC97:0))); // 0x60
     else
      kx_writep16v(hw, p16vSRCSel, 0, p16vSRCSel_SPDIF|p16vSRCSel_I2S|((hw->lack_ac97?p17vSRCSel_AC97:0))); // 0x60
  }
  else
  {
     // note: AC97 is never enabled

     if(onoff)
      kx_writep16v(hw, p17vSRCSel, 0, p17vSRCSel_I2S|((hw->lack_ac97?p17vSRCSel_AC97:0))); // 0x60
     else
      kx_writep16v(hw, p17vSRCSel, 0, p17vSRCSel_SPDIF|p17vSRCSel_I2S|((hw->lack_ac97?p17vSRCSel_AC97:0))); // 0x60
  }
 }
 
 return 0;
}

KX_API(int,kx_init_passthru(kx_hw *hw,void *instance,int *method))
{
 int ret=0;

 if(hw->ac3_pt_state.method==0)
 {
  // choose the best method
  if(hw->is_10k2)
   hw->ac3_pt_state.method=KX_AC3_PASSTHRU_GENERIC;
  else
   hw->ac3_pt_state.method=KX_AC3_PASSTHRU_XTRAM;

  // for debugging purposes:
  if(hw->ext_flags&KX_HW_FORCE_XTRAM_PT) // force XTRAM method
   hw->ac3_pt_state.method=KX_AC3_PASSTHRU_XTRAM;
  
  if(method)
   *method=hw->ac3_pt_state.method;

  switch(hw->ac3_pt_state.method)
  {
    case KX_AC3_PASSTHRU_XTRAM:
          {
           // perform additional initialization here...
           // we may set ret to -1 and fail initialization!

           // search for 'ac3passthru_x'
           dsp_microcode *m=0;

           struct list *item;
           unsigned long flags=0;
           kx_lock_acquire(hw,&hw->dsp_lock, &flags);

           ret=-1;
           for_each_list_entry(item, &hw->microcodes) 
           {
              m = list_item(item, dsp_microcode, list);
              if(!m)
                  continue;
              if(strcmp(m->guid,"64824522-f847-4bca-ac45-7a58c321d4e3")==0)
              {
                 hw->ac3_pt_state.pgm_id=m->pgm;

                 // found:
                 dsp_register_info *bz=find_dsp_register_in_m(hw,m,"buf_size");
                 if(bz)
                 {
                   hw->ac3_pt_state.buf_size=bz->p+1;

                   bz=find_dsp_register_in_m(hw,m,"dbac_value");
                   if(bz)
                   {
                    hw->ac3_pt_state.dbac_reg=bz->num;
                    hw->ac3_pt_state.m=m;

                    bz=find_dsp_register_in_m(hw,m,"counter");

                    if(bz)
                    {
                         hw->ac3_pt_state.counter_reg=bz->num;

                         // microcode is oK
                         debug(DLIB,"ac3passthru_x ok: pgm_id: %d buf_size: %d(+) dbac_reg: %x counter: %x\n",
                           hw->ac3_pt_state.pgm_id,
                           hw->ac3_pt_state.buf_size,
                           hw->ac3_pt_state.dbac_reg,
                           hw->ac3_pt_state.counter_reg);
                         ret=0;

                         break;
                    }
                    else
                    {
                     debug(DLIB,"ac3passthru_x microcode invalid [no 'counter']\n");
                     ret=-5;
                    }
                   }
                   else
                   {
                    debug(DLIB,"ac3passthru_x microcode invalid [no 'dbac_value']\n");
                    ret=-4;
                   }
                 }
                 else
                 {
                  debug(DLIB,"ac3passthru_x microcode invalid [no 'buf_size']\n");                  
                  ret=-3;
                 }
              }
           }

           if(ret)
           {
            debug(DLIB,"ac3passthru_x microcode not loaded. trying epilog...\n");
            // FIXME
            debug(DLIB,"epilog is not ac-3 xtram - capable\n");

            ret=-1;
           }

           kx_lock_release(hw,&hw->dsp_lock,&flags);
          }
          break;
    case KX_AC3_PASSTHRU_GENERIC:
          {
           // nothing
          }
          break;
  }

  if(ret==0)
  {
    hw->ac3_pt_state.prev_spdif_freq=0xffffffff;
    
    kx_get_hw_parameter(hw,KX_HW_SPDIF_FREQ,&hw->ac3_pt_state.prev_spdif_freq);
    kx_set_hw_parameter(hw,KX_HW_SPDIF_FREQ,1); // 48000

    // 'instance' should be set -after- spdif freq
    hw->ac3_pt_state.instance=instance;
  }
  else
   hw->ac3_pt_state.method=0;

  return ret;
 }

 debug(DWDM,"ac-3 passthru is already activated\n");
 return -1;
}


KX_API(int,kx_close_passthru(kx_hw *hw,void *instance))
{
 if(hw->ac3_pt_state.method)
 {
  if(hw->ac3_pt_state.instance!=instance)
  {
   debug(DERR,"kx_close_passthru: invalid instance (%08x %08x; %d)\n",
    instance,hw->ac3_pt_state.instance,hw->ac3_pt_state.method);
   return -4;
  }

  switch(hw->ac3_pt_state.method)
  {
   case KX_AC3_PASSTHRU_XTRAM:
           // unload dynamically uploaded effect here
           // ...
           break;
   case KX_AC3_PASSTHRU_GENERIC:
       // nothing to do
       break;
   default:
    debug(DERR,"invalid ac-3 pt state [%d] while closing pt\n",hw->ac3_pt_state.method);
  }

  // to make 'set_hw_parameter' work
  hw->ac3_pt_state.instance=0;

  if(hw->ac3_pt_state.prev_spdif_freq!=0xffffffff)
   kx_set_hw_parameter(hw,KX_HW_SPDIF_FREQ,hw->ac3_pt_state.prev_spdif_freq);

  my_memset(&hw->ac3_pt_state,0,sizeof(kx_ac3_passthru));
  return 0;
 }

 debug(DWDM,"ac-3 passthru was not initialized!\n");
 return -3;
}
