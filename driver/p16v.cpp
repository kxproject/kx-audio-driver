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


// p16v / alice3 

#include "kx.h"

#define get_prouting(a) (p16v_table[((hw->dsp_flags&KX_DSP_SWAP_REAR)?1:0)][(a)&0x3])

static int p16v_table[2][4]=
{
 { 0, 3, 1, 2 },
 { 3, 0, 1, 2 }
};


KX_API(int,kx_p16v_init(kx_hw *hw))
{
  if(hw->is_edsp)
  {
   debug(DLIB,"p16v/p17v initialization skipped for E-DSP cards\n");
   hw->hcfg_k1|=(HCFG_I2SASRC0_K2|HCFG_I2SASRC1_K2|HCFG_I2SASRC2_K2);
   return 0;
  }

  debug(DLIB,"p16v initialization\n");

  dword tmp;

  // certain registers are invalid after CL drivers [if installed w/o reboot]:
  kx_writefn0(hw,pINTE,0);

  if(!hw->is_k8)
    kx_writefn0(hw,p16vCDIF_INTE,0);

  // Setup SRCMulti_I2S SamplingRate
  tmp = kx_readptr(hw, EHC, 0);
  tmp &= (~EHC_SRCMULTI_I2S_MASK); // clear srcmulti_i2s bits
  // 3538j change: initially, set it
  tmp |= EHC_SRCMULTI_I2S_96; // 0x400 -- questionable; now we set it to 'bypass'
  kx_writeptr(hw, EHC, 0, tmp);

  // Setup SRCSel (Enable Spdif,I2S SRCMulti; 10k2.ac97 is sent to ac97, not src48)
  // NOTE: these settings are duplicated in ac3pt.cpp:
  if(!hw->is_k8)
   kx_writep16v(hw, p16vSRCSel, 0, p16vSRCSel_SPDIF|p16vSRCSel_I2S|((hw->lack_ac97?p16vSRCSel_AC97:0))); // reg: 0x60
        //   srcmulti(i2s,spdif)->i2s,spdif [not 10k2.i2s/10k2.spdif]; 10k2.ac97->ac97
        //   for ac3 passthru: spdif should be re-routed to 10kx
  else
   kx_writep16v(hw, p17vSRCSel, 0, p17vSRCSel_SPDIF|p17vSRCSel_I2S|((hw->lack_ac97?p17vSRCSel_AC97:0))); // reg: 0x60 
        // [10kx.ac97-> ac97; mixer_spdif->spdif; mixer_i2s->i2s]
        // srp_rec_i2sin_to_10k2_sel: 0 [i2sin goes to fxengine dsp 0x48..0x4d]

  // enable backward recording of p16v (via DSP 0x50..57)
  if(!hw->is_k8)
  {
   kx_writep16v(hw, p16vSRC48AudEnb, 0, 0x00ff0000); 
   // 0x6d: p16v->fxbus50 is enabled [SRC48AudEnbp16v]; fxbus60..67->src48 is NOT [SRC48AudEnb10k2]
  }

  // select rec sources
  kx_p16v_rec_select(hw,P16V_REC_I2S|P16V_REC_0);

  // set p16v channel assignment (+ swap front & rear)
  kx_p16v_set_routing(hw,P16V_ROUTE_SPDIF|P16V_ROUTE_P_SPDIF|P16V_ROUTE_I2S|P16V_ROUTE_P_I2S);
  // that is, 0xffffffff [0x6e, SRCMULTI_ENABLE]

  if(hw->is_k8)
  {
    // kx_writep16v(hw,p17vSoftResetSRPMixer,0,0x77); // do NOT reset anything

    kx_writep16v(hw,p17vK2_AC97L,0,0xffffffff); // [10k2 -> mixer_ac97]: always muted
    kx_writep16v(hw,p17vK2_AC97R,0,0xffffffff);
    kx_writep16v(hw,p17vP17V_AC97L,0,0xffffffff); // p17v -> mixer_ac97
    kx_writep16v(hw,p17vP17V_AC97R,0,0xffffffff);
    kx_writep16v(hw,p17vSPR_AC97L,0,0xffffffff); // rec_srp -> mixer_ac97
    kx_writep16v(hw,p17vSPR_AC97R,0,0xffffffff);

    kx_writep16v(hw,p17vAC97En,0,0x0); // do not enable 10k2,p17v,rec_srp -> mixer_ac97

    kx_writep16v(hw,p17vAC97L,0,0x30303030); // master volume for ac97 outputs
    kx_writep16v(hw,p17vAC97R,0,0x30303030);

    kx_writep16v(hw,p17vK2_SPDIFL,0,0x30303030); // 10k2 -> mixer_spdif
    kx_writep16v(hw,p17vK2_SPDIFR,0,0x30303030);
    kx_writep16v(hw,p17vSPR_SPDIFL,0,0xffffffff); // rec_srp -> mixer_spdif
    kx_writep16v(hw,p17vSPR_SPDIFR,0,0xffffffff);

    kx_writep16v(hw,p17vSPDIFEn,0,0xffff0000); 
     // spdifout audio enable: a2value has a separate spdif out; needs mixer switch
     // enable 10k2->mixer_spdif, p17v->mixer_spdif

    kx_writep16v(hw,p17vSPDIFL,0,0x30303030); // master volume for spdif outputs
    kx_writep16v(hw,p17vSPDIFR,0,0x30303030);

    kx_writep16v(hw,p17vK2_I2SL,0,0x30303030); // 10k2 -> mixer_i2s
    kx_writep16v(hw,p17vK2_I2SR,0,0x30303030);
    kx_writep16v(hw,p17vSPR_I2SL,0,0xffffffff); // rec_srp -> mixer_i2s
    kx_writep16v(hw,p17vSPR_I2SR,0,0xffffffff);

    kx_writep16v(hw,p17vI2SEn,0,0xffff0000); // srcmulti input audio enable
     // enable 10k2->mixer_i2s, p17v->mixer_i2s

    kx_writep16v(hw,p17vI2SL,0,0x30303030); // master volume for i2s outputs
    kx_writep16v(hw,p17vI2SR,0,0x30303030);

    kx_writep16v(hw,p17vMixerAtt,0,0x0); // default 0dB gain for all mixers
  }
  else
  {
    // src48vols aren't affected by set_pb (since it's a loopback):
    kx_writep16v(hw,p16vSRC48VolL,0,0x30303030); // p16v->src48->10k2 DSP [0x50..0x57 DSP]
    kx_writep16v(hw,p16vSRC48VolH,0,0x30303030);
    kx_writep16v(hw,p16vSRCk2L,0,0xffffffff); // 10k2 DSP 0x60 -> src48 -> ac97 and DSP 0x50..0x57 (muted)
    kx_writep16v(hw,p16vSRCk2H,0,0xffffffff);
    kx_writep16v(hw,p16vSRCMultiSPDIFL,0,0x30303030); // srcmulti -> srcmulti mixer (10k2 path; unaltered)
    kx_writep16v(hw,p16vSRCMultiSPDIFH,0,0x30303030);
    kx_writep16v(hw,p16vSRCMultiI2SL,0,0x30303030); // srcmulti -> srcmulti mixer (10k2 path; unaltered)
    kx_writep16v(hw,p16vSRCMultiI2SH,0,0x30303030);
  }

  // set p16v playback & rec levels
  for(int i=0;i<8;i++)
   kx_p16v_set_pb_volume(hw,i,0);

  kx_p16v_set_rec_volume(hw,0);

  // audio enable: ac97, spdif [I2sOutEnb], i2s [SpdifOutEnb]-- enabled
  // srcmulti: source: fxengine (0x60/0x68), not src48 [0xXX000000]

  if(!hw->is_k8)
  {
    if(hw->lack_ac97) // note: ac97 not initialized yet...
     kx_writep16v(hw,p16vAudOutEnb,0,0xff); // spdif+i2s enabled
    else
    {
     kx_writep16v(hw,p16vAudOutEnb,0,0xff|pAudOutEnb_AC97);  // spdif+i2s+ac97 enabled
    }
  }
  else
  {
    kx_writep16v(hw,p17vAudOutEnb,0,0x000000ff|(!hw->lack_ac97?0x10000:0));
     // enable all i2s, spdif outputs (0xff)
     //  mixer_ac97 is NOT routed to dsp 0x50..0x57
     //  srp_rec IS routed to dsp 0x58..0x5f [and not mixer_ac97]
  }

  // enabled; zero-filled rec -- multichannel set-up by default + route-back
  kx_writefn0(hw, pHCFG, pHCFG_EA|/* pre-3537c pHCFG_DRI|*/pHCFG_RBEN);
   // note: alsa sets pHCFG_PI/pHCFG_PI
   // we set both on the fly [if stereo mode is activated]

  // Setup I2S ASRC Enable
  hw->hcfg_k1|=(HCFG_I2SASRC0_K2|HCFG_I2SASRC1_K2|HCFG_I2SASRC2_K2);

  // Unmute Analog now.  Set GPO6 to 1 for Apollo. 
  // This has to be done after init ALice3 I2SOut beyond 48KHz. 
  // So, sequence is important.
  hw->hcfg_k2 |= HCFG_K2_DISABLE_ANALOG; // NOTE: 'ENABLE'! -> GPO1

  if(hw->is_k8)
  {
   hw->hcfg_k2 |= HCFG_K2_UNKNOWN_20; // -> 0x60 [0x40 is set above]
   hw->hcfg_k2 &= ~0x8;

   hw->hcfg_k2 |= 0x16; // |0x60 -> 0x76
   // actually we need to set 0x2|0x10 (0x12) - enable spdifout, enable 7.1 analog
   hw->hcfg_k2 |= HCFG_K2_ENABLE_SPDIF; // 0x10

   kx_writep16v(hw,p17vSESlotL,0,0x03020100);
   kx_writep16v(hw,p17vSESlotH,0,0x07060504);

   // capture source + start bit
   kx_writep16v(hw,p17vSC,0,0x1e740);

   // 14, 15, 16 = 0
   // 40 = 0
   // 67,68 (spdif vol) = 0
   // 6b, 6c -> 0xcfcfcfcf
   // 6d: ffff3030
   // kx_writep16v(hw,,0,);
  }

  // force 0x5f - because ac97_reset isn't called for certain 10k2.2 boards
  kx_writeptr(hw, PCB, 0, PCB_10K2);

  hw->p16v_pb_chn=get_prouting(hw->p16v_routing>>8);
  hw->p16v_rec_chn=0;

  // Setup SRCMulti_I2S SamplingRate
  if(hw->ext_flags&KX_HW_P16V_FOLLOW)
  {
   tmp = kx_readptr(hw, EHC, 0);
   tmp &= (~EHC_SRCMULTI_I2S_MASK); // clear srcmulti_i2s bits
   // 3538j change: now, clear it [0x400 / EHC_SRCMULTI_I2S_96]
   kx_writeptr(hw, EHC, 0, tmp);
  }

  return 0;
}

// ---------------- playback ----------------------

KX_API(int,kx_p16v_pb_start(kx_hw *hw))
{
 if(hw->p16v_pb_opened)
 {
   debug(DLIB,"start pb: pb_chn: %d\n",hw->p16v_pb_chn);
   kx_timer_enable(hw,&hw->p16v_pb_timer);

   // start HW
   if(!hw->is_k8)
    kx_writep16v(hw,p16vSA,0,(0x1<<hw->p16v_pb_chn)|(kx_readp16v(hw,p16vSA,0)));
   else
    ; // fixme !! p17v

   return 0;
 }
 else return -1;
}

KX_API(int,kx_p16v_pb_stop(kx_hw *hw))
{
 if(hw->p16v_pb_opened)
 {
   debug(DPV,"stop pb\n");
   kx_timer_disable(hw,&hw->p16v_pb_timer);

   // stop HW
   if(!hw->is_k8)
    kx_writep16v(hw,p16vSA,0,kx_readp16v(hw,p16vSA,0)&(~(0x1<<hw->p16v_pb_chn)));
   else
    ; // fixme !! p17v

   return 0;
 }
 else return -1;
}

KX_API(int,kx_p16v_pb_get_pos(kx_hw *hw,dword &pos))
{
 if(hw->is_edsp)
  return -1;

 if(!hw->is_k8)
  pos=kx_readp16v(hw,p16vCPFA,hw->p16v_pb_chn); // current address [bytes]
 else
  pos=0; // FIXME !! p17v 3542

// debug(DPV,"get pos [%x]\n",pos);
 return 0;
}

KX_API(int,kx_p16v_pb_open(kx_hw *hw,kx_voice_buffer *buff,int frequency,int flag))
{
 if(hw->is_edsp)
  return -1;

 debug(DLIB,"opening p16v playback... [%x/%x %x %d; %d channels; freq: %d; routing: %x, chn: %d]\n",buff->physical,buff->addr,buff->size,buff->notify,(flag&P16V_STEREO)?2:8,frequency,hw->p16v_routing,get_prouting(hw->p16v_routing>>8));

 int bitmask=0;

 switch(frequency)
 {
  case 44100: bitmask=EHC_SRC48_44; break;
  case 48000: bitmask=EHC_SRC48_BYPASS; break;
  case 96000: bitmask=EHC_SRC48_96; break;
  case 192000: bitmask=EHC_SRC48_192; break;
  default:
    debug(DLIB,"p16v_pb_open: error: invalid freq [%d]\n",frequency);
    return -2;
 }

 if(hw->p16v_pb_opened)
 {
  debug(DLIB,"p16v pb device already in use\n");
  return -1;
 }

 hw->p16v_pb_opened=1;
 hw->p16v_pb_chn=get_prouting(hw->p16v_routing>>8);

 if(flag&P16V_STEREO) // reset routing
 {
  if(hw->is_k8)
  {
   // fixme: p17v
  }
  else
   kx_writep16v(hw, p16vRecSel, 0, 0x100e4e4); // pb: 3210; record: 0x68..6f (i2s outs); rec: 3210
 }

 my_memset(&hw->p16v_pb_timer,0,sizeof(kx_timer));
 hw->p16v_pb_timer.status=TIMER_UNINSTALLED;
 hw->p16v_pb_timer.data=buff->that;
 hw->p16v_pb_timer.timer_func=hw->cb.notify_func;

 dword hcfg=kx_readfn0(hw, pHCFG);

 if(flag&P16V_STEREO)
   kx_writefn0(hw, pHCFG, hcfg&(~pHCFG_PI));
 else
 {
   kx_writefn0(hw, pHCFG, hcfg|pHCFG_PI);
   hw->p16v_pb_chn=0;
 }

 // set-up HW buffers here
 if(!hw->is_k8)
 {
    kx_writep16v(hw,p16vCPFA,hw->p16v_pb_chn,0); // current address [bytes]
    kx_writep16v(hw,p16vPFEA,hw->p16v_pb_chn,0); // end address [bytes]
    kx_writep16v(hw,p16vPFBA,hw->p16v_pb_chn,buff->physical); // start address
 }
 else
 {
    // fixme!! p17v
 }

 kx_writep16v(hw,pCPCAV,hw->p16v_pb_chn,0); // cur. addr in samples; cur cache size in samples

 kx_p16v_pb_set_notify(hw,buff->notify);

 if(!hw->is_k8)
  kx_writep16v(hw,p16vPFBS,hw->p16v_pb_chn,(buff->size&0xffff)<<16); // buffer size
 else
  ; // FIXME p17v

 // CPFAV not programmed! FIXME (3541): Current address of playback fifo in audio samples  and Current status of cache size in audio samples

 // set p16v frequency
 kx_writeptr(hw,EHC,0,(kx_readptr(hw,EHC,0)&(~EHC_SRC48_MASK))|bitmask);

 return 0;
}

KX_API(int,kx_p16v_pb_close(kx_hw *hw))
{
 if(hw->p16v_pb_opened)
 {
  debug(DPV,"closing pb...\n");

  kx_p16v_pb_stop(hw);
  kx_p16v_pb_set_notify(hw,0);

  hw->p16v_pb_opened=0;

  // set p16v frequency
  kx_writeptr(hw,EHC,0,kx_readptr(hw,EHC,0)&(~EHC_SRC48_MASK));

  // re-set routing
  kx_p16v_set_routing(hw,hw->p16v_routing);

  return 0;
 }
 else return -1;
}

KX_API(int,kx_p16v_pb_set_freq(kx_hw *hw,int freq))
{
 if(hw->is_edsp)
  return -1;

 int bitmask;
 switch(freq)
 {
  case 44100: bitmask=EHC_SRC48_44; break;
  case 48000: bitmask=EHC_SRC48_BYPASS; break;
  case 96000: bitmask=EHC_SRC48_96; break;
  case 192000: bitmask=EHC_SRC48_192; break;
  default:
    debug(DLIB,"p16v_pb_set_freq: error: invalid freq [%d]\n",freq);
    return -2;
 }
  // set p16v frequency
  kx_writeptr(hw,EHC,0,(kx_readptr(hw,EHC,0)&(~EHC_SRC48_MASK))|bitmask);

  return 0;
}

KX_API(int,kx_p16v_pb_set_notify(kx_hw *hw,int interval))
{
 debug(DPV,"set pb notify to %d\n",interval);
 if(hw->p16v_pb_opened)
 {
    kx_timer_uninstall(hw,&hw->p16v_pb_timer);

    if(interval!=0)
    {
       __int64 delay;

       if(interval>0)
       {
        delay=hw->card_frequency*interval/1000; // it is in ms
       }
       else
       {
        delay=-interval; // it is in samples
       }
       kx_timer_install(hw,&hw->p16v_pb_timer,(dword)delay);
    }
    return 0;
 } else return -1;

 return 0;
}

// ---------------- recording ----------------------

KX_API(int,kx_p16v_rec_start(kx_hw *hw))
{
 if(hw->p16v_rec_opened)
 {
   debug(DPV,"start rec\n");
   kx_timer_enable(hw,&hw->p16v_rec_timer);

   // start HW
   if(!hw->is_k8)
    kx_writep16v(hw,p16vSA,0,(0x100<<hw->p16v_rec_chn)|(kx_readp16v(hw,p16vSA,0)));
   else
    ; // fixme p17v

   return 0;
 }
 else return -1;
}

KX_API(int,kx_p16v_rec_stop(kx_hw *hw))
{
 if(hw->p16v_rec_opened)
 {
   debug(DPV,"stop rec\n");
   kx_timer_disable(hw,&hw->p16v_rec_timer);

   // stop HW
   if(!hw->is_k8)
    kx_writep16v(hw,p16vSA,0,kx_readp16v(hw,p16vSA,0)&(~(0x100<<hw->p16v_rec_chn)));
   else
    ; // fixme p17v

   return 0;
 }
 else return -1;
}

KX_API(int,kx_p16v_rec_get_pos(kx_hw *hw,dword &pos))
{
 pos=kx_readp16v(hw,pCRFA,hw->p16v_rec_chn); // current address [bytes]
// debug(DPV,"get rec pos [%x]\n",pos);
 return 0;
}

KX_API(int,kx_p16v_rec_open(kx_hw *hw,kx_voice_buffer *buff,int frequency,int flag))
{
 if(hw->is_edsp)
  return -1;

 debug(DLIB,"opening p16v rec... [%x/%x %x %d; %d channels; %dHz]\n",buff->physical,buff->addr,buff->size,buff->notify,(flag&P16V_STEREO)?2:8,frequency);

 if(hw->p16v_rec_opened)
 {
  debug(DLIB,"p16v rec device already in use\n");
  return -1;
 }

 int bitmask;
 switch(frequency)
 {
  case 44100: bitmask=EHC_SRC48_44; break;
  case 48000: bitmask=EHC_SRC48_BYPASS; break;
  case 96000: bitmask=EHC_SRC48_96; break;
  case 192000: bitmask=EHC_SRC48_192; break;
  default:
    debug(DLIB,"p16v_rec_open: error: invalid freq [%d]\n",frequency);
    return -2;
 }

 hw->p16v_rec_opened=1;

 my_memset(&hw->p16v_rec_timer,0,sizeof(kx_timer));
 hw->p16v_rec_timer.status=TIMER_UNINSTALLED;
 hw->p16v_rec_timer.data=buff->that;
 hw->p16v_rec_timer.timer_func=hw->cb.notify_func;

 dword hcfg=kx_readfn0(hw, pHCFG);

 if(flag&P16V_STEREO)
 {
   // pre-3537c: hw->p16v_rec_chn should be set by set_rec_source
   // 3537d: bad idea :)
   hw->p16v_rec_chn=(hw->p16v_rec_select&0x30)>>4;

   kx_writefn0(hw, pHCFG, hcfg&(~pHCFG_RI));
 }
 else
 {
   kx_writefn0(hw, pHCFG, hcfg|pHCFG_RI);
   hw->p16v_rec_chn=0;
 }

 // set-up HW buffers here
 kx_writep16v(hw,pCRFA,hw->p16v_rec_chn,0); // current address [bytes]
 // kx_writep16v(hw,pXX,0,0); // end address [bytes]
 kx_writep16v(hw,pCRCAV,hw->p16v_rec_chn,0); // cur. addr in samples; cur cache size in samples
 kx_writep16v(hw,pRFBA,hw->p16v_rec_chn,buff->physical); // start address

 kx_p16v_rec_set_notify(hw,buff->notify);

 kx_writep16v(hw,pRFBS,hw->p16v_rec_chn,(buff->size&0xffff)<<16); // buffer size

 // 3538j: dynamic p16v i2s in/out programming (bypass->96kHz)
 if(hw->ext_flags&KX_HW_P16V_FOLLOW)
 {
  kx_writeptr(hw,EHC,0,(kx_readptr(hw,EHC,0)|EHC_SRCMULTI_I2S_96));
 }

 // CRFAV not programmed! FIXME (3541): Current address of playback fifo in audio samples  and Current status of cache size in audio samples

 return 0;
}

KX_API(int,kx_p16v_rec_close(kx_hw *hw))
{
 if(hw->p16v_rec_opened)
 {
  debug(DPV,"closing rec...\n");

  kx_p16v_rec_stop(hw);
  kx_p16v_rec_set_notify(hw,0);

  // 3538j: dynamic p16v i2s in/out programming (bypass->96kHz)
  if(hw->ext_flags&KX_HW_P16V_FOLLOW)
  {
   kx_writeptr(hw,EHC,0,(kx_readptr(hw,EHC,0)&(~EHC_SRCMULTI_I2S_96)));
  }

  hw->p16v_rec_opened=0;

  return 0;
 }
 else return -1;
}

KX_API(int,kx_p16v_rec_set_notify(kx_hw *hw,int interval))
{
 debug(DPV,"set rec notify to %d\n",interval);
 if(hw->p16v_rec_opened)
 {
    kx_timer_uninstall(hw,&hw->p16v_rec_timer);

    if(interval!=0)
    {
       __int64 delay;

       if(interval>0)
       {
        delay=hw->card_frequency*interval/1000; // it is in ms
       }
       else
       {
        delay=-interval; // it is in samples
       }
       kx_timer_install(hw,&hw->p16v_rec_timer,(dword)delay);
    }
    return 0;
 } else return -1;

 return 0;
}

// ---------------------- routing / volume stuff

KX_API(int,kx_p16v_set_rec_volume(kx_hw *hw,dword vol)) // vol is in vol units
{
/*
 // -33..12 dB step: 1.5dB 
 // mute: 1f 
 // zero: 0x8

 // this is invalid (but per specs :)
 // fixme: re-check

 int zero=0x8;
 dword val;

 if((int)vol<(int)(-33*0x10000))
  val=0x1f;
 else
 {
  if((int)vol/0x10000>12)
   vol=12*0x10000;

  val=zero-((int)vol/0x18000); // /0x10000*(1.5dB)
 }
*/
 int zero=0x30;
 dword val;

 if((int)vol<(int)(-51*0x10000))
  val=0xff;
 else
 {
  if((int)vol/0x10000>12)
   vol=12*0x10000;
  val=zero-((int)vol/0x4000); // /0x10000*4
 }

 // 0x14,0x15
 if(hw->is_k8)
 {
  // fixme p17v: invalid volume; should be: -33dB..12dB (step 1.5dB); bypass=0x8, mute=0x1f
  kx_writep16v(hw,p17vRecVolL,0,val|(val<<8)|(val<<16)|(val<<24));
  kx_writep16v(hw,p17vRecVolH,0,val|(val<<8)|(val<<16)|(val<<24));
 }
 else
 {
  kx_writep16v(hw,p16vRecVolL,0,val|(val<<8)|(val<<16)|(val<<24));
  kx_writep16v(hw,p16vRecVolH,0,val|(val<<8)|(val<<16)|(val<<24));
 }

 return 0;
}

KX_API(int,kx_p16v_rec_select(kx_hw *hw,int source))
{
 if(hw->is_edsp)
  return -1;

 hw->p16v_rec_select=source;

 source&=3;

 if(source==P16V_REC_WUH) 
   source=3;

 // high bit is actual 'chn'
 // 3537c:
 // 3537d: bad idea
 // hw->p16v_rec_chn=(hw->p16v_rec_select&0x30)>>4;

 // low bit is 0: spdif 1: i2s 3 (was '2'): 'wuh' [srcmulti]
 // 000=spdif channel 0 ; 001=i2s channel 0 ; 010=SRC48 channel 0; 011=SRCMulti channel 0; 100=CDIF channel 0

 dword data=(source<<16)|(source<<20)|(source<<24)|(4<<28); // force srcmulti_i2s (source==3)?3:0)
 // always select i2s as 4th input (FIXME NOW... really?...)

 if(!hw->is_k8)
  kx_writep16v(hw,p16vSA,0,(kx_readp16v(hw,p16vSA,0)&0xffff)|data);
 else
 {
  ; // fixme p17v
  // kx_writep16v(hw,p17vSRPRecordSRP,0,0x7); ??
 }
 
 return 0;
}

KX_API(int,kx_p16v_set_pb_volume(kx_hw *hw,int chn,dword vol))
{
 if(hw->is_edsp)
  return -1;

 dword shift=0,mask=0,reg_off=0;

 if(chn==-1)
 {
  debug(DERR,"p16v set vol: LEGACY CODE\n");
  return -1;
 }

 vol=(dword)((int)vol+(int)hw->p16v_volumes[chn]);

 if(hw->dsp_flags&KX_DSP_SWAP_REAR)
 {
  switch(chn)
  {
   case 0: chn=2; break;
   case 1: chn=3; break;
   case 2: chn=0; break;
   case 3: chn=1; break;
  }
 }

 switch(chn)
 {
  case 0: mask=0xff; shift=0; reg_off=0; break;
  case 1: mask=0xff00; shift=8; reg_off=0; break;
  case 2: mask=0xff0000; shift=16; reg_off=0; break;
  case 3: mask=0xff000000; shift=24; reg_off=0; break;
  case 4: mask=0xff; shift=0; reg_off=1; break;
  case 5: mask=0xff00; shift=8; reg_off=1; break;
  case 6: mask=0xff0000; shift=16; reg_off=1; break;
  case 7: mask=0xff000000; shift=24; reg_off=1; break;
  case -1: break;
  default:
   debug(DLIB,"invalid p16v channel [%d]\n",chn);
   return -5;
 }

 // FF9C0000h  -100 dB  * 0x10000

 int zero=0x30;
 dword val;

 if((int)vol<(int)(-51*0x10000))
  val=0xff;
 else
 {
  if((int)vol/0x10000>12)
   vol=12*0x10000;
  val=zero-((int)vol/0x4000); // /0x10000*4
 }


 if(hw->is_k8)
 {
   // FIXME NOW !! p17v
    kx_writep16v(hw,p17vP17V_SPDIFL,0,0x30303030); // p17v -> mixer_spdif
    kx_writep16v(hw,p17vP17V_SPDIFR,0,0x30303030);
    kx_writep16v(hw,p17vP17V_I2SL,0,0x30303030); // p17v -> mixer_i2s
    kx_writep16v(hw,p17vP17V_I2SR,0,0x30303030);
 }
 else
 {
    // this is actually: p16vSRCSpdifL, p16vSRCSpdifH, p16vSRCI2SL, p16vSRCI2SH
    for(int rr=p16vSRCSpdifL;rr<=p16vSRCI2SL;rr+=2)
    {
//    if(chn!=-1)
     kx_writep16v(hw,rr+reg_off,0,(kx_readp16v(hw,rr+reg_off,0)&(~mask))|(val<<shift));
//    else
//    {
//     kx_writep16v(hw,rr,0,val|(val<<8)|(val<<16)|(val<<24));
//     kx_writep16v(hw,rr+1,0,val|(val<<8)|(val<<16)|(val<<24));
//    }
    }
 }

 return 0; 
}

KX_API(int,kx_p16v_set_routing(kx_hw *hw,int routing))
{
  hw->p16v_routing=routing;

  if(hw->is_edsp)
   return -1;

  if(hw->is_k8)
  {
   debug(DLIB,"kx_p16v_set_routing: not supported for k4/dev8\n");
   // FIXME !!

   // re-map outputs (to allow h/ph + rear_center)
   if(hw->dsp_flags&KX_DSP_SWAP_REAR)
       kx_writep16v(hw, p17vPbChnSelect, 0, 0x76543210); // fixme ? check this?
   else
       kx_writep16v(hw, p17vPbChnSelect, 0, 0x76543210);
  }
  else
  {
     // re-map outputs (to allow h/ph + rear_center)
     if(hw->dsp_flags&KX_DSP_SWAP_REAR)
       kx_writep16v(hw, p16vRecSel, 0, 0x10036e4); // [reg 0x16: was 0xe4e4]; pb: 0312; record: 0x68..6f (i2s outs); rec: 3210
     else
       kx_writep16v(hw, p16vRecSel, 0, 0x100b4e4); // [0x16: was 0xe4e4]; pb: 2310; record: 0x68..6f (i2s outs); rec: 3210

     // Creative: record from 0x7000000 -> 0x98..0x9f

     // Setup SRCMulti Input Audio Enable
     dword val=0;
     if(routing&P16V_ROUTE_SPDIF)
      val|=0xff000000;
     if(routing&P16V_ROUTE_P_SPDIF)
      val|=0xff0000;
     if(routing&P16V_ROUTE_I2S)
      val|=0xff00;
     if(routing&P16V_ROUTE_P_I2S)
      val|=0xff;

     kx_writep16v(hw, p16vSRCMultiAudEnb, 0, val); // // 0x6e was: 0xff00ff00
  }
  return 0;
}

int kx_get_p16v_volume(kx_hw *hw,int chn,dword *vol)
{
 if(chn>=0 && chn <=7 && vol)
 {
  *vol=hw->p16v_volumes[chn];
  return 0;
 }
 else
  return -1;
}

int kx_set_p16v_volume(kx_hw *hw,int chn,dword vol)
{
 if(hw->is_edsp)
  return -1;

 if(chn>=0 && chn <=7)
 {
  hw->p16v_volumes[chn]=vol;
  // FIXME 3538 add realtime volume update function!!
  return 0;
 }
 else
  return -1;
}
