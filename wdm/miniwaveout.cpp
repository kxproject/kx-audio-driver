// kX WDM Audio Driver
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


#include "common.h"

#include "interface/guids.h"

#include "eax/eax10.h"
#include "eax/eax20.h"
#include "eax/eax30.h"
#include "eax/3dl2.h"

#include "wdm/tbl_wave.h"

#include "kx3d.cpp"

#if !defined(AMD64)
        #include "./f_asm.c"
#else
inline void __fastcall swap_bytes(register byte *buf, register int count)
{
    while(count>0)
    {
        byte tmp=buf[0];
        buf[0]=buf[1];
        buf[1]=tmp;
        buf+=2;
        count-=2;
    }
}
#endif

#define AC3_DST_FRAME_SIZE      (1536*2)    // 1536 mono samples per frame
#define AC3_SRC_FRAME_SIZE      (1536*4)    // padded: 1536 stereo samples

extern "C" int AC3_CALLTYPE __stdcall get_frame_size(unsigned int tmp,int *frame_size,int *bit_rate,int *sampling_rate);

#pragma code_seg("PAGE")
CMiniportWaveOutStream::~CMiniportWaveOutStream(void)
{
    PAGED_CODE();

    if(hw)
    {
       if(!(ac3_timer.status&TIMER_UNINSTALLED))
       {
        kx_timer_disable(hw,&ac3_timer);
        kx_timer_uninstall(hw,&ac3_timer);
       }

       if(ac3_pt_method)
       {
        kx_close_passthru(hw,this);
        ac3_pt_method=0;
       }
    }

    if(source.in_chain)
    {
     Miniport->listener.remove(&source);
    }

    if(magic!=WAVEOUTSTREAM_MAGIC)
    {
        debug(DWDM,"!!! [~CMiniportWaveOutStream]: magic(%x) != %x\n",magic,WAVEOUTSTREAM_MAGIC);
        magic=0;
        return;
    }
    magic=0;

    if(is_asio)
    {
     if(hw)
     {
        asio_close();
     }
     is_asio=0;
    }

    for(int i=0;i<PHYS_WAVE_CHANNELS;i++)
    {
     if(channels[i]!=-1)
     {
       kx_wave_close(hw,channels[i]);
       channels[i]=-1;
     }
    }

    if(ServiceGroup)
    {
        ServiceGroup->Release();
        ServiceGroup=NULL;
    }

    for(int i=0;i<PHYS_WAVE_CHANNELS;i++)
    {
       if(dma_channels[i])
       {
        dma_channels[i]->Release();
        dma_channels[i]=NULL;
       }
    }

    if(source_channel)
    {
        source_channel->Release();
        source_channel=NULL;
    }

    if(Miniport)
    {
        Miniport->Release();
        Miniport=NULL;
    }

}

#pragma code_seg()
inline dword routing_by_pos(kx_hw * hw, int pos)
{
    switch((1<<pos))
    {
         case SPEAKER_FRONT_LEFT: 
         case SPEAKER_TOP_FRONT_LEFT:
         case SPEAKER_FRONT_LEFT_OF_CENTER:
           //debug(DNONE,"setting routing [%d] to left\n",cnt);
           return hw->cb.def_routings[DEF_AC3_LEFT_ROUTING];
         case SPEAKER_FRONT_RIGHT: 
         case SPEAKER_TOP_FRONT_RIGHT:
         case SPEAKER_FRONT_RIGHT_OF_CENTER:
           //debug(DNONE,"setting routing [%d] to right\n",cnt);
           return hw->cb.def_routings[DEF_AC3_RIGHT_ROUTING];
         case SPEAKER_LOW_FREQUENCY: 
           //debug(DNONE,"setting routing [%d] to lfe\n",cnt);
           return hw->cb.def_routings[DEF_AC3_SUBWOOFER_ROUTING];
         case SPEAKER_SIDE_LEFT:
           //debug(DNONE,"setting routing [%d] to sleft\n",cnt);
           return hw->cb.def_routings[DEF_3D_LEFT_ROUTING];
         case SPEAKER_BACK_LEFT: 
         case SPEAKER_TOP_BACK_LEFT:
           //debug(DNONE,"setting routing [%d] to sleft\n",cnt);
           return hw->cb.def_routings[DEF_AC3_SLEFT_ROUTING];
         case SPEAKER_SIDE_RIGHT:
           //debug(DNONE,"setting routing [%d] to sright\n",cnt);
           return hw->cb.def_routings[DEF_3D_RIGHT_ROUTING];
         case SPEAKER_BACK_RIGHT: 
         case SPEAKER_TOP_BACK_RIGHT:
           //debug(DNONE,"setting routing [%d] to sright\n",cnt);
           return hw->cb.def_routings[DEF_AC3_SRIGHT_ROUTING];
         case SPEAKER_FRONT_CENTER:
         case SPEAKER_TOP_CENTER:
         case SPEAKER_TOP_FRONT_CENTER:
         case SPEAKER_TOP_BACK_CENTER:
           //debug(DNONE,"setting routing [%d] to center\n",cnt);
           return hw->cb.def_routings[DEF_AC3_CENTER_ROUTING];
         case SPEAKER_BACK_CENTER:
           //debug(DNONE,"setting routing [%d] to rear_center\n",cnt);
           return hw->cb.def_routings[DEF_AC3_SCENTER_ROUTING];
    } // switch

    return 0xffffffff; // not found
}

#pragma code_seg()
NTSTATUS CMiniportWaveOutStream::Open()
{
     if(ac3_pt_method==KX_AC3_PASSTHRU_XTRAM)
     {
       debug(DSTATE,"waveout: ac3 passthru via xTRAM opened\n");
       return STATUS_SUCCESS;
     }

     //debug(DBUFF,"kx wdm debug: begin ------- cut here -------------------------------\n");
     debug(DWDM,"waveout open[%x;%x]: open(freq=%d; n_channels=%d[%d]; bps=%d; type: %s%s%s)\n",
          Miniport,this,
          current_freq,
          n_channels,n_chn,
          bps,
          out_type&OUT_BUFFERED?" Buffered":" Normal",out_type&OUT_AC3?" AC3":"",
          (ac3_pt_method==KX_AC3_PASSTHRU_GENERIC)?"+AC3passthru":"");

     dword routings=DEF_WAVE01_ROUTING;

     if(ac3_pt_method==KX_AC3_PASSTHRU_GENERIC)
     {
       routings=DEF_AC3PASSTHROUGH_ROUTING;
     }
     else
     {
      switch(Miniport->wave_number)
      {
        case 1: routings=DEF_WAVE23_ROUTING; break;
        case 2: routings=DEF_WAVE45_ROUTING; break;
        case 3: routings=DEF_WAVE67_ROUTING; break;
      }
     }

     #define new_routing_h(a) (bps<=16)?(a):(((a)*2)|0x20202020)
     #define new_routing_l(a) (((a)*2+1)|0x20202020)

     if(channels[0]!=-1)
     {
      debug(DWDM,"!!! NB! stream waveout::open - already opened\n");

      for(int i=0;i<PHYS_WAVE_CHANNELS;i++)
      {
           if(channels[i]!=-1)
            kx_wave_close(hw,channels[0]);
           channels[i]=-1;
      }
      // return STATUS_UNSUCCESSFUL;
     }

       if(!(out_type&OUT_BUFFERED))
       {
            kx_voice_buffer buffer;
            buffer.size=source_buffer_size;
            buffer.physical=source_channel->PhysicalAddress().LowPart;

            if(source_channel->PhysicalAddress().HighPart || source_channel->PhysicalAddress().LowPart&0x80000000)
               debug(DWDM,"!! [CMiniportWaveOutStream::Open]: physical address out of limits (%x.%x)\n",
                    source_channel->PhysicalAddress().HighPart,
                    source_channel->PhysicalAddress().LowPart);

            buffer.addr=source_channel->SystemAddress();
            buffer.that=this;
            buffer.notify=DMA_DEFAULT_NOTIFY; // 10 ms

            //debug(DBUFF,"kx wdm debug: physical address: %x.%x; system addr: %x\n",
             //source_channel->PhysicalAddress().HighPart,
             //buffer.physical,
             //buffer.addr);

            channels[0]=kx_wave_open(hw,
                 &buffer,
                 ((n_chn==2)?VOICE_FLAGS_STEREO:0)|
                 (bps==16?VOICE_FLAGS_16BIT:0)|
                 VOICE_USAGE_PLAYBACK,current_freq,routings);

            if(channels[0]!=-1)
            {
                    hw->voicetable[channels[0]].synth_id=0;
                    if(n_chn==2)
                     hw->voicetable[channels[0]+1].synth_id=0;

                    if(routings==DEF_AC3PASSTHROUGH_ROUTING)
                    {
                        kx_wave_setup_b2b(hw,channels[0],hw->cb.def_routings[routings],1,0); // high, A
                        kx_wave_setup_b2b(hw,channels[0]+1,hw->cb.def_routings[routings],1,1); // high, B

                        hw->voicetable[channels[0]].synth_id|=VOICE_STATUS_AC3PT;
                        if(n_chn==2)
                         hw->voicetable[channels[0]+1].synth_id|=VOICE_STATUS_AC3PT;
                    }

                    return STATUS_SUCCESS;
            }
            else
            {
                return STATUS_UNSUCCESSFUL;
            }
       }
       else
       {
             NTSTATUS ntStatus=STATUS_SUCCESS;

             my_memset(&ac3_state,0,sizeof(struct ac3_state_s));

             ac3_state.debug_func=hw->cb.debug_func;

             int dest_buffer_size;

             if(out_type&OUT_AC3)
               dest_buffer_size=AC3_DST_FRAME_SIZE*cb_ac3_buffers;
             else
             {
               // assume output will be always '16 bit'
               dest_buffer_size=((source_buffer_size*8/bps)/n_chn)*2;
               // was: dest_buffer_size=source_buffer_size/n_channels;
             }

             // check for >16 bit
             if(bps>16)
             {
              if(bps!=24 && bps!=32)
              {
               debug(DWDM,"incorrect bps: %d\n",bps);
               return STATUS_UNSUCCESSFUL;
              }
             }

             for(int i=0;i<n_channels;i++)
             {
                     if(dma_channels[i])
                     {
                        dma_channels[i]->Release();
                        dma_channels[i]=NULL;
                     }

                     PHYSICAL_ADDRESS addr; addr.LowPart=0x7fffffff; addr.HighPart=0x0;
                     dma_channels[i] = CreateCDmaChannel(dest_buffer_size,addr);
                     if(dma_channels[i] == 0)
                      ntStatus=STATUS_INSUFFICIENT_RESOURCES;
                     else
                      dma_channels[i]->AddRef();

                     if(!NT_SUCCESS(ntStatus))
                      return ntStatus;

                     kx_voice_buffer buffer;
                     buffer.size=dest_buffer_size;
                     buffer.physical=dma_channels[i]->PhysicalAddress().LowPart;

                     if(dma_channels[i]->PhysicalAddress().HighPart || dma_channels[i]->PhysicalAddress().LowPart&0x80000000)
                        debug(DWDM,"!! [CMiniportWaveOutStream::Open]: physical address out of limits (%x.%x)\n",
                             dma_channels[i]->PhysicalAddress().HighPart,
                             dma_channels[i]->PhysicalAddress().LowPart);

                     buffer.addr=dma_channels[i]->SystemAddress();
                     buffer.that=this;
                     buffer.notify=DMA_DEFAULT_NOTIFY; // ms
                     
                     Silence(buffer.addr,buffer.size);

                     if(channels[i]!=-1)
                       kx_wave_close(hw,channels[i]);

                     channels[i]=kx_wave_open(hw,
                      &buffer,
                      VOICE_FLAGS_16BIT|VOICE_USAGE_PLAYBACK| (i==0?0:VOICE_OPEN_NOTIMER),
                      current_freq,
                      routings);

                     if(channels[i]==-1)
                      return STATUS_UNSUCCESSFUL;

                     hw->voicetable[channels[i]].synth_id=((out_type&OUT_AC3)?VOICE_STATUS_AC3:0) |
                                     ((out_type&OUT_BUFFERED)?VOICE_STATUS_BUFFERED:0);

                     if(bps==24 || bps==32)
                       hw->voicetable[channels[i]].synth_id|=VOICE_STATUS_24BIT;
             } // for each channel

             if(NT_SUCCESS(ntStatus))
             {
               if(out_type&OUT_AC3)
               {
                     ac3_state.left=(signed short *)(dma_channels[AC3_LEFT]->SystemAddress());
                     ac3_state.right=(signed short *)(dma_channels[AC3_RIGHT]->SystemAddress());
                     ac3_state.center=(signed short *)(dma_channels[AC3_CENTER]->SystemAddress());
                     ac3_state.sleft=(signed short *)(dma_channels[AC3_SLEFT]->SystemAddress());
                     ac3_state.sright=(signed short *)(dma_channels[AC3_SRIGHT]->SystemAddress());
                     ac3_state.subwoofer=(signed short *)(dma_channels[AC3_SUBWOOFER]->SystemAddress());

                     // INs: direct l,c,r,sl,sr,s\w
                     kx_wave_set_parameter(hw,channels[AC3_LEFT],0,KX_VOICE_ROUTINGS,
                        hw->cb.def_routings[DEF_AC3_LEFT_ROUTING]);
                     kx_wave_set_parameter(hw,channels[AC3_LEFT],1,KX_VOICE_PAN,
                         KX_MIN_VOLUME);

                     kx_wave_set_parameter(hw,channels[AC3_RIGHT],0,KX_VOICE_ROUTINGS,
                        hw->cb.def_routings[DEF_AC3_RIGHT_ROUTING]);
                     kx_wave_set_parameter(hw,channels[AC3_RIGHT],1,KX_VOICE_PAN,
                         KX_MIN_VOLUME);

                     kx_wave_set_parameter(hw,channels[AC3_CENTER],0,KX_VOICE_ROUTINGS,
                        hw->cb.def_routings[DEF_AC3_CENTER_ROUTING]);
                     kx_wave_set_parameter(hw,channels[AC3_CENTER],1,KX_VOICE_PAN,
                         KX_MIN_VOLUME);

                     kx_wave_set_parameter(hw,channels[AC3_SLEFT],0,KX_VOICE_ROUTINGS,
                        hw->cb.def_routings[DEF_AC3_SLEFT_ROUTING]);
                     kx_wave_set_parameter(hw,channels[AC3_SLEFT],1,KX_VOICE_PAN,
                         KX_MIN_VOLUME);

                     kx_wave_set_parameter(hw,channels[AC3_SRIGHT],0,KX_VOICE_ROUTINGS,
                        hw->cb.def_routings[DEF_AC3_SRIGHT_ROUTING]);
                     kx_wave_set_parameter(hw,channels[AC3_SRIGHT],1,KX_VOICE_PAN,
                         KX_MIN_VOLUME);

                     kx_wave_set_parameter(hw,channels[AC3_SUBWOOFER],0,KX_VOICE_ROUTINGS,
                        hw->cb.def_routings[DEF_AC3_SUBWOOFER_ROUTING]);
                     kx_wave_set_parameter(hw,channels[AC3_SUBWOOFER],1,KX_VOICE_PAN,
                         KX_MIN_VOLUME);

                     ac3_state.fill_buffer=ac3_fill_buffer;
                     ac3_state.that=this;

                     kx_fpu_state state;
                     hw->cb.save_fpu_state(&state);

                     ac3_init(&ac3_state);

                     hw->cb.rest_fpu_state(&state);
                     cur_ac3_buffer=-1;
                }
                else
                {
                 // buffered here
                 // set routings here

                 dword mask=0;
                 {
                  PKSDATAFORMAT_WAVEFORMATEX  WaveFormat;
                  PWAVEFORMATEXTENSIBLE WaveFormatEx;
                  WaveFormat = (PKSDATAFORMAT_WAVEFORMATEX) &DataFormat2;
                  WaveFormatEx = (PWAVEFORMATEXTENSIBLE)&WaveFormat->WaveFormatEx;
                  mask=WaveFormatEx->dwChannelMask;
                 }

//                 debug(DNONE,"open: mask: %x\n",mask);

                 // check if mask is set incorrectly...
                 int ch_tmp=0;
                 for(int j=0;j<32;j++)
                  if(mask&(1<<j)) ch_tmp++;

                 if(ch_tmp!=n_chn)
                 {
                    if(mask!=0)
                     debug(DWDM,"!! mask is [%x] nch=%d[%d] -> reset to corresponding value\n",
                       mask,n_chn,n_channels);
                    mask=0;

                    switch(n_chn)
                    {
                     default:

#if REAL_WAVE_CHANNELS>=6
                     case 5+1:
                        mask = KSAUDIO_SPEAKER_5POINT1;
                        break;
 #if REAL_WAVE_CHANNELS>=7
                     case 6+1:
                        mask = KSAUDIO_SPEAKER_5POINT1 | SPEAKER_BACK_CENTER;
                        break;
    #if REAL_WAVE_CHANNELS>=8
                     case 7+1:
                        mask= KSAUDIO_SPEAKER_7POINT1;
                        // SPEAKER_FRONT_LEFT_OF_CENTER | SPEAKER_FRONT_RIGHT_OF_CENTER
                        break;
    #endif
 #endif
#endif
                     case 2:
                        mask = KSAUDIO_SPEAKER_STEREO;
                        break;
                     case 1:
                        mask = KSAUDIO_SPEAKER_MONO;
                        break;
                     case 4:
                        mask = KSAUDIO_SPEAKER_QUAD;
                        break;
                     case 5:
                        mask = KSAUDIO_SPEAKER_SURROUND|SPEAKER_LOW_FREQUENCY;
                        break;
                     case 3:
                        mask = SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_FRONT_CENTER;
                        break;
                    }
                   // debug(DWDM,"Mask is now %x [%d/%d channels]\n",mask,n_channels,n_chn);
                 }

                 if(n_chn!=2 || mask!=KSAUDIO_SPEAKER_STEREO)
                 {
                    int cnt=0;

                    int pos=0;
                    for(pos=0;pos<32;pos++)
                    {
                     if(mask&(1<<pos))
                     {
                      dword nroutings=new_routing_h(routing_by_pos(hw,pos));

                      if(nroutings==0xffffffff)
                        continue;

                      if(hw->is_10k2 && bps>16)
                      {
                         kx_wave_setup_b2b(hw,channels[cnt],nroutings,1,0); // high, A
                      }
                      else
                      {
                         // 16-bit multichannel audio will also use original routings, new_routing_h() will simply return def_routings[] value
                         kx_wave_set_parameter(hw,channels[cnt],0,KX_VOICE_ROUTINGS,nroutings);
                         kx_wave_set_parameter(hw,channels[cnt],1,KX_VOICE_PAN,KX_MIN_VOLUME);
                      }
                      cnt++;
                     }  // if mask
                    } // for pos 0..32

                    // next channels - 'lowest' parts here:
                    // kx_wave_set_parameter(hw,channels[cnt],0,KX_VOICE_SEND_A,4); // hw: kx_fxamount(4)=0x20
                    // also, assume volume is 0x8000 by default

                    if((bps==24 || bps==32) && hw->can_passthru) // 10k2/passthru-only
                    {
                    int pos=0;
                    for(pos=0;pos<32;pos++)
                    {
                     if(mask&(1<<pos))
                     {
                      dword nroutings=new_routing_l(routing_by_pos(hw,pos));

                      if(nroutings==0xffffffff)
                        continue;

                      kx_wave_setup_b2b(hw,channels[cnt],nroutings,0,0); // low, A
                      cnt++;
                     }  // if mask
                    } // for pos 0..32
                }

                if(cnt!=n_channels)
                {
                 debug(DWDM,"!! error: nchannels!=chn in multiwave pb (voices routed: %d n_channels: %d[%d]; mask: %x)\n",
                  cnt,n_channels,n_chn,mask);
                }
                }
                else // stereo & n_chn=2 
                {
                  // reset routings
                  if(bps==16 || !hw->is_10k2 || n_channels!=4)
                  {
                    debug(DWDM,"!! error: bps=%d, 10k2=%d n_channels=%d\n",bps,hw->is_10k2,n_channels);
                  }
                  else
                  {
                    // example:
                    // routing 4/5 -> 0x28/0x2a00
                    kx_wave_setup_b2b(hw,channels[0],new_routing_h(hw->cb.def_routings[routings]),1,0); // high, A
                    kx_wave_setup_b2b(hw,channels[1],new_routing_h(hw->cb.def_routings[routings]),1,1); // high, B

                    // example:
                    // routing 4/5 -> 0x29/2b00
                    kx_wave_setup_b2b(hw,channels[2],new_routing_l(hw->cb.def_routings[routings]),0,0); // inc SEND_A; low, A
                    kx_wave_setup_b2b(hw,channels[3],(hw->cb.def_routings[routings]*2+0x100)|0x20202020,0,1); // inc SEND_B; low, B
                  }
                } // stereo & nchn=2 / nchannels=4

                cur_ac3_buffer=-1;
               } // if type & (!AC_3)
             } // success allocating?
             return ntStatus;
       } // buffered?
}

#pragma code_seg()
#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
VOID dpc_ac3_func(
    IN PKDPC Dpc,
    IN PVOID DeferredContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    )
{
 CMiniportWaveOutStream *that=(CMiniportWaveOutStream *)SystemArgument1;

 if(that)
 {
  if(that->magic==WAVEOUTSTREAM_MAGIC)
  {
     int pos=0;
     kx_wave_get_position(that->hw,that->channels[0],&pos); // pos is in bytes

     kx_hw *hw=that->hw;

     int buf_pos=pos/AC3_DST_FRAME_SIZE;
     buf_pos-=2;
     if(buf_pos<0)
       buf_pos+=(int)that->cb_ac3_buffers;

     if(that->cur_ac3_buffer==-1 || buf_pos==that->cur_ac3_buffer)
         {
           if(that->out_type&OUT_AC3)
           {
              if(that->cur_ac3_buffer==-1)
               that->cur_ac3_buffer=0;

                  that->cur_ac3_buffer++;
                  if(that->cur_ac3_buffer>=(int)that->cb_ac3_buffers)
                    that->cur_ac3_buffer=0;

                  dword offset=AC3_DST_FRAME_SIZE*that->cur_ac3_buffer;

                  that->ac3_state.left=(signed short *)((uintptr_t)that->dma_channels[AC3_LEFT]->SystemAddress()+offset);
                  that->ac3_state.right=(signed short *)((uintptr_t)that->dma_channels[AC3_RIGHT]->SystemAddress()+offset);
                  that->ac3_state.center=(signed short *)((uintptr_t)that->dma_channels[AC3_CENTER]->SystemAddress()+offset);
                  that->ac3_state.sleft=(signed short *)((uintptr_t)that->dma_channels[AC3_SLEFT]->SystemAddress()+offset);
                  that->ac3_state.sright=(signed short *)((uintptr_t)that->dma_channels[AC3_SRIGHT]->SystemAddress()+offset);
                  that->ac3_state.subwoofer=(signed short *)((uintptr_t)that->dma_channels[AC3_SUBWOOFER]->SystemAddress()+offset);

                  kx_fpu_state state;
                  hw->cb.save_fpu_state(&state);

                  __try
                  {

                  if(ac3_decode_frame(&that->ac3_state))
                  {
                   debug(DWDM,"ac-3 sync lost - zero the frame\n");

                   memset(&that->ac3_state.left[0],0,AC3_DST_FRAME_SIZE);
                   memset(&that->ac3_state.right[0],0,AC3_DST_FRAME_SIZE);
                   memset(&that->ac3_state.center[0],0,AC3_DST_FRAME_SIZE);
                   memset(&that->ac3_state.sleft[0],0,AC3_DST_FRAME_SIZE);
                   memset(&that->ac3_state.sright[0],0,AC3_DST_FRAME_SIZE);
                   memset(&that->ac3_state.subwoofer[0],0,AC3_DST_FRAME_SIZE);
                  }

                  } __except ( EXCEPTION_EXECUTE_HANDLER )
                  {
                   debug(DWDM,"ac-3 exception! [decoding]\n");
                  }

                  hw->cb.rest_fpu_state(&state);
           }
         }  
  } else debug(DWDM,"!!! DPC call with BAD MAGIC [%x]\n",that->magic);
 } else debug(DWDM,"!!! DPC call with NO that\n");
}

#pragma code_seg()
#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
void CMiniportWaveOutStream::my_isr(void)
{
 if(channels[0]>=0)
 {
    if(hw->voicetable[channels[0]].param.interpolate)
    {
     kx_set_fx_amount(hw,hw->voicetable[channels[0]].param.interpolate,channels[0],0);
    }
    if(hw->voicetable[channels[0]].usage&VOICE_FLAGS_STEREO)
    {
     if(hw->voicetable[channels[0]+1].param.interpolate)
     {
      kx_set_fx_amount(hw,hw->voicetable[channels[0]+1].param.interpolate,channels[0]+1,0);
     }
    }
 }

 Miniport->Port->Notify(ServiceGroup);
 if(out_type&OUT_AC3)
 {
   KeInsertQueueDpc(&dpc_ac3,this,NULL);
   // seems to be right since irq dpc handles a spinlock...
   // dpc_ac3_func(0,0,this,0);
 }
 else
 {
  if(out_type&OUT_BUFFERED)
  {
   // split buffers here
   int pos=0;
   kx_wave_get_position(hw,channels[0],&pos); /*pos/=2;*/
   pos=pos/(((source_buffer_size*8/bps)/n_chn)); // since original position is in bytes, and data is actually 16-bit
   // 3545b
   // was: pos/((source_buffer_size/n_channels)/2);

   if(
      (cur_ac3_buffer==-1) ||
      (cur_ac3_buffer==pos)
     )
   {
     if(cur_ac3_buffer==-1)
      cur_ac3_buffer=0;

     cur_ac3_buffer=1-cur_ac3_buffer;

     switch(bps)
     {
      case 16:
             {
                  word *outputs_[PHYS_WAVE_CHANNELS+1];
                  int cnt=0;
                  int shift=((source_buffer_size/n_chn))>>2;
                  for(int i=0;i<PHYS_WAVE_CHANNELS;i++)
                  {
                   if(channels[i]>=0)
                   {
                    outputs_[cnt]=(word *)hw->voicetable[channels[i]].buffer.addr;
                    if(outputs_[cnt])
                    {
                     if(cur_ac3_buffer)
                       outputs_[cnt]+=shift; // /2; /2 because of 'word'
                     cnt++;
                    }
                   }
                  }

                  outputs_[cnt]=0;

                  word *input=(word *)source_channel->SystemAddress();

                  if(cur_ac3_buffer)
                    input+=shift*cnt; 

                  word **outputs=outputs_;
                  while(*outputs) 
                  {
                   register int i;
                   register word *out;

                   out = *outputs;
                   for(i = shift; --i >= 0; )
                    *(out + i) = *(input + i*cnt);
                   outputs++;
                   input++;
                  }
             }
             break;
      case 24:
             {
                  int shift=((source_buffer_size/n_chn)/6);

                  word *outputs_high_[REAL_WAVE_CHANNELS+1];
                  int cnt_high=0;

                  for(int i=0;i<n_chn;i++)
                  {
                   if(channels[i]>=0)
                   {
                    outputs_high_[cnt_high]=(word *)hw->voicetable[channels[i]].buffer.addr;
                    if(outputs_high_[cnt_high])
                    {
                     if(cur_ac3_buffer)
                       outputs_high_[cnt_high]+=shift;
                       // was: ((source_buffer_size/n_channels))>>2; 
                       // /2; /2 because of 'word'
                     cnt_high++;
                    }
                   }
                  }

                  word *outputs_low_[REAL_WAVE_CHANNELS+1];
                  int cnt_low=0;

                  for(int i=n_chn;i<n_channels;i++)
                  {
                   if(channels[i]>=0)
                   {
                    outputs_low_[cnt_low]=(word *)hw->voicetable[channels[i]].buffer.addr;
                    if(outputs_low_[cnt_low])
                    {
                     if(cur_ac3_buffer)
                       outputs_low_[cnt_low]+=shift;
                       // was: ((source_buffer_size/n_channels))>>2; 
                       // /2; /2 because of 'word'
                     cnt_low++;
                    }
                   }
                  }

                  outputs_high_[cnt_high]=0;
                  outputs_low_[cnt_low]=0;

                  if(cnt_high!=cnt_low)
                  {
                   debug(DWDM,"kx.sys: internal error 1223\n");
                   return;
                  }

                  byte *input=(byte *)source_channel->SystemAddress();

                  if(cur_ac3_buffer)
                    input+=(source_buffer_size>>1); 

                  word **outputs_low=outputs_low_;
                  word **outputs_high=outputs_high_;

                  int mul1=cnt_low*3;

                  while(*outputs_low)
                  {
                   register int i;
                   register word *outl, *outh;

                   outl = *outputs_low;
                   outh = *outputs_high;

                   for(i = shift; --i >= 0; )
                   {
                    dword data=*(dword *)(input + i*mul1);
                    *(outl + i) = (word)((data&0xff)<<8);
                    *(outh + i) = (word)((data&0xffff00)>>8);
                   }

                   outputs_low++;
                   outputs_high++;

                   input+=3;
                  }
             }
             break;
      case 32:
             {
                  int shift=((source_buffer_size/n_chn)/8);

                  word *outputs_high_[REAL_WAVE_CHANNELS+1];
                  int cnt_high=0;

                  for(int i=0;i<n_chn;i++)
                  {
                   if(channels[i]>=0)
                   {
                    outputs_high_[cnt_high]=(word *)hw->voicetable[channels[i]].buffer.addr;
                    if(outputs_high_[cnt_high])
                    {
                     if(cur_ac3_buffer)
                       outputs_high_[cnt_high]+=shift;
                       // was: ((source_buffer_size/n_channels))>>2; 
                       // /2; /2 because of 'word'
                     cnt_high++;
                    }
                   }
                  }

                  word *outputs_low_[REAL_WAVE_CHANNELS+1];
                  int cnt_low=0;

                  for(int i=n_chn;i<n_channels;i++)
                  {
                   if(channels[i]>=0)
                   {
                    outputs_low_[cnt_low]=(word *)hw->voicetable[channels[i]].buffer.addr;
                    if(outputs_low_[cnt_low])
                    {
                     if(cur_ac3_buffer)
                       outputs_low_[cnt_low]+=shift;
                       // was: ((source_buffer_size/n_channels))>>2; 
                       // /2; /2 because of 'word'
                     cnt_low++;
                    }
                   }
                  }

                  outputs_high_[cnt_high]=0;
                  outputs_low_[cnt_low]=0;

                  if(cnt_high!=cnt_low)
                  {
                   debug(DWDM,"kx.sys: internal error 1223\n");
                   return;
                  }

                  dword *input=(dword *)source_channel->SystemAddress();

                  if(cur_ac3_buffer)
                    input+=(source_buffer_size>>(1+2)); // since 'input' is dword

                  word **outputs_low=outputs_low_;
                  word **outputs_high=outputs_high_;

                  while(*outputs_low)
                  {
                   register int i;
                   register word *outl, *outh;

                   outl = *outputs_low;
                   outh = *outputs_high;

                   for(i=0;i<shift;i++)
                   {
                    *(outl + i) = (word)(input[i*cnt_low]&0xffff);
                    *(outh + i) = (word)(input[i*cnt_low]>>16);
                   }

                   outputs_low++;
                   outputs_high++;

                   input++;
                  }
             }
             break;
      default:
      case 8:
             break;
     } // switch bps
   } // need to swap?
  } // buffered
 } // not ac-3?
}

#pragma code_seg()
#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
void CMiniportWaveOutStream::ac3_fill_buffer(byte **start,byte **end,struct ac3_state_s *state,int size)
{
    uint_32 bytes_read;
    CMiniportWaveOutStream *that=(CMiniportWaveOutStream *)state->that;

    if(that)
    {
     if(that->magic!=WAVEOUTSTREAM_MAGIC)
     {
      debug(DWDM,"!!! Bad Magic in AC3 fill_buffer\n");
      return;
     }
    } else { debug(DWDM,"!!! Empty state in ac3 fill_buffer\n"); return; }

        int nb=that->cur_ac3_buffer;
        if(nb<0)
         nb=0;         

        *start = (byte *)((uintptr_t)that->source_channel->SystemAddress()+(uintptr_t)nb*AC3_SRC_FRAME_SIZE);

        byte *p=*start;
        int i=0;

ONCE_AGAIN:

        int found=0;
        int iec_size=0;

        // check per IEC 61937 [spdif packing]
        // and search for the 0x0b 0x77 bytes:
        
        for(;i<AC3_SRC_FRAME_SIZE-16;i++,p++)
        {
         if(*(dword *)p==0x4e1ff872) // IEC sync bytes
         {
          if((*(word *)(p+4)==0x0001))
          {
             iec_size=(int)(*(word *)(p+6));
             p+=8;
             i+=8;

             if((p[0]==0x77) && (p[1]==0x0b)) // found
             {
              *start=p;
              found=1;
              break;
             } else debug(DWDM,"ac3: IEC sync ok, AC-3 magic failed [%x]\n",*(word *)p);
          } else debug(DWDM,"ac3: IEC sync ok, subformat failed [%x]\n",*(word *)(p+4));
         }
        }

        if(found)
        {
            unsigned int t;

            t=*(p+5)+((*(p+2))<<8)+((*(p+3))<<16);

            int frame_size=0,bit_rate=0,sampling_rate=-1;

            if(get_frame_size(t,&frame_size,&bit_rate,&sampling_rate)==0)
            {
             debug(DWDM,"ac3: frame: %d bitrate: %d samplerate: %d; size: %d x:%d, iec_size=%d\n",
               frame_size,bit_rate,sampling_rate,size,frame_size*2,iec_size);

             if(size==0)
              size=frame_size*2;
             else
              if(size!=frame_size*2)
              {
               debug(DWDM,"ac3: frame size has changed? [%d %d %x]\n",size,frame_size*2,t);
              }
            }
            else // invalid frame
            {
             debug(DWDM,"ac3: invalid subframe - skip [%x]\n",t);
             goto ONCE_AGAIN;
            }

            // re-check 'size': can be invalid...
            if(nb*AC3_SRC_FRAME_SIZE+i+size > that->cb_ac3_buffers*AC3_SRC_FRAME_SIZE)
             size=that->cb_ac3_buffers*AC3_SRC_FRAME_SIZE-nb*AC3_SRC_FRAME_SIZE-i;

            *end=(byte *)((uintptr_t)*start + size);

            swap_bytes(*start,size);

            // final check: sampling_rate
            if(that->current_freq!=sampling_rate && sampling_rate!=-1)
            {
             debug(DWDM,"ac3: dynamic sampling rate change [%d->%d]\n",that->current_freq,sampling_rate);
             that->current_freq=sampling_rate;
             for(i=0;i<that->n_channels;i++)
             {
              if(that->channels[i]!=-1)
               kx_wave_set_parameter(that->hw,that->channels[i],0,KX_VOICE_PITCH,that->current_freq);           
             }
            }
        } // found?
        else
        {
          // not found?..
          // start already set
          *end=(byte *)((uintptr_t)*start + 4);
        }
}


#pragma code_seg("PAGE")
NTSTATUS CMiniportWaveOutStream::Init(
              IN CMiniportWave *Miniport_,
          IN POOL_TYPE PoolType_,
              IN ULONG Pin_,
              IN PKSDATAFORMAT DataFormat_,
              OUT PDMACHANNEL *DmaChannel_,
              OUT PSERVICEGROUP *ServiceGroup_)

{
    PAGED_CODE();

    memset(&ac3_timer,0,sizeof(kx_timer));
    ac3_timer.status=TIMER_UNINSTALLED;
    ac3_timer.timer_func=ac3_timer_func;
    ac3_timer.data=this;

    out_type=0;
    ServiceGroup=NULL;
    Miniport = NULL;
    source_channel=NULL;
    is_asio=0;
    State=KSSTATE_STOP;
    magic=WAVEOUTSTREAM_MAGIC;
    current_freq=-1;
    current_freq_multiplier=DOPPLER_MULT;
    n_channels=-1;
    n_chn=-1;
    bps=-1;
    cur_ac3_buffer=-1;
    source_buffer_size=0;
    ac3_pt_method=0;
    kx3d_attn=0;
    ac3_pt_pos=0;

    for(int i=0;i<PHYS_WAVE_CHANNELS;i++)
    {
     channels[i]=-1;
     dma_channels[i]=NULL;
    }

    for(int i=0;i<MAX_TOPOLOGY_CHANNELS;i++)
    { vol1[i]=0; vol2[i]=0; }

    Pin = Pin_;
    PoolType = PoolType_;

    Miniport = Miniport_;
    Miniport->AddRef();

    source.listener=&Miniport->listener;

    hw=Miniport->hw;
    asio_hw=hw;
    asio_magic=ASIO_MAGIC;
    that_=this;
    cb_ac3_buffers=hw->cb.ac3_buffers;
    cb_pb_buffers=hw->cb.pb_buffer;

    asio_notification_user=0;
    asio_notification_mdl=0;

    asio_latency=0; // in samples
    asio_bps=0;

    NTSTATUS ntStatus = PcNewServiceGroup(&ServiceGroup,NULL);
    if(ServiceGroup)
    {
     *ServiceGroup_=ServiceGroup;
     ServiceGroup->AddRef();
    }
    else
    {
     *ServiceGroup_=NULL;
     debug(DWDM,"!!! ServiceGroup creation failed\n");
    }

    if(NT_SUCCESS(ntStatus))
    {
        /*
        // pre-3549:
        ntStatus = Miniport->Port->NewMasterDmaChannel
          (&source_channel,NULL,NULL,cb_pb_buffers*2*REAL_WAVE_CHANNELS*2,TRUE,FALSE,(DMA_WIDTH)-1,(DMA_SPEED)-1);
        if(NT_SUCCESS(ntStatus))
        {
            // re-allocate buffer for >2Gb memory size

            source_channel->FreeBuffer();
            PHYSICAL_ADDRESS addr; addr.LowPart=0x7fffffff; addr.HighPart=0x0;

            ntStatus = source_channel->AllocateBuffer(cb_pb_buffers*2*REAL_WAVE_CHANNELS*2,&addr);

            if(NT_SUCCESS(ntStatus))
            {
                    source_channel->AddRef();

                    debug(DWDM,"[CMiniportWaveOutStream::Init]: allocated buffer @%p, phys: %x:%x\n",
                        source_channel->SystemAddress(),
                        source_channel->PhysicalAddress().HighPart,
                        source_channel->PhysicalAddress().LowPart);

            }
            else
             debug(DWDM,"!!! [CMiniportWaveOutStream::Init] :: failed to re-allocate buffer [%x]\n",ntStatus);

        } else debug(DWDM,"!!! [CMiniportWaveOutStream::Init] :: failed to create NewMasterDmaChannel [%x]\n",ntStatus);
        */
        PHYSICAL_ADDRESS addr; addr.LowPart=0x7fffffff; addr.HighPart=0x0;
        source_channel = CreateCDmaChannel(cb_pb_buffers*2*REAL_WAVE_CHANNELS*2,addr);
        if(source_channel == 0)
         ntStatus=STATUS_INSUFFICIENT_RESOURCES;
        else
         source_channel->AddRef();
    }

    if(source_channel)
    {
     *DmaChannel_=source_channel;
     }
    else
    {
     *DmaChannel_=NULL;
     debug(DWDM,"!!! DmaChannel creation failed\n");
    }

    if(NT_SUCCESS(ntStatus))
    {
        ntStatus=SetFormat(DataFormat_);

        if(NT_SUCCESS(ntStatus))
        {
            ntStatus=Open();

            if(!NT_SUCCESS(ntStatus))
            {
             debug(DWDM,"!!! [CMiniportWaveOutStream::Init] : Failed to open audio\n");
             ntStatus=STATUS_UNSUCCESSFUL;
            }
            else
            {
                Silence(source_channel->SystemAddress(),source_buffer_size);
            }
        }
    }
     
    if( !NT_SUCCESS(ntStatus) )
    {
        if(source_channel)
        {
            source_channel->Release();
            source_channel=NULL;    
        }
        if(ServiceGroup)
        {
            ServiceGroup->Release();
            ServiceGroup=NULL;
        }
        if(Miniport)
        {
            Miniport->Release();
            Miniport=NULL;
        }
    }
    return ntStatus;
}

#pragma code_seg("PAGE")
int CMiniportWaveOutStream::Parse()
{
 PAGED_CODE();
 PWAVEFORMATEXTENSIBLE WaveFormatEx;

  // Fill the structure
  if (IsEqualGUIDAligned(DataFormat2.DataFormat.Specifier,KSDATAFORMAT_SPECIFIER_DSOUND))
  {
      PKSDATAFORMAT_DSOUND    DSoundFormat;
      KSDSOUND_BUFFERDESC     *BufferDesc;

      DSoundFormat = (PKSDATAFORMAT_DSOUND)&DataFormat2;
      BufferDesc = (KSDSOUND_BUFFERDESC *)&DataFormat2.BufferDesc;
      // bufferdesc contains, moreover, Flags & Control
      // Flags: primary/static/lochardware/locsoftware
      // Controls: 3d/pan/freq/vol/position
      // FIXME later
      
      debug(DWDM," -- [prased] using KSDATAFORMAT_DSOUND format\n");
      
      WaveFormatEx = (PWAVEFORMATEXTENSIBLE)&DSoundFormat->BufferDesc.WaveFormatEx;
  } 
  else
  if (IsEqualGUIDAligned(DataFormat2.DataFormat.Specifier,KSDATAFORMAT_SPECIFIER_WAVEFORMATEX))
  {
      PKSDATAFORMAT_WAVEFORMATEX  WaveFormat;

      WaveFormat = (PKSDATAFORMAT_WAVEFORMATEX) &DataFormat2;
      
      WaveFormatEx = (PWAVEFORMATEXTENSIBLE)&WaveFormat->WaveFormatEx;
  }
  else
  {
    debug(DWDM,"!!! -- [parsed] UNKNOWN FORMAT SPECIFIER (not WaveFormatEx)\n");
    return STATUS_INVALID_PARAMETER;
  }

  if(WaveFormatEx->Format.nSamplesPerSec>KX_MAX_RATE ||
     WaveFormatEx->Format.nSamplesPerSec<KX_MIN_RATE)
     {
       debug(DWDM,"!!! -- but format freq is not OK\n");

       if(WaveFormatEx->Format.nSamplesPerSec) 
         debug(DWDM,"! -- it was: nSamplePerSec: %d\n",WaveFormatEx->Format.nSamplesPerSec);
       if(WaveFormatEx->Format.nChannels)
         debug(DWDM,"! --         nChannels: %d\n",WaveFormatEx->Format.nChannels);
       if(WaveFormatEx->Format.wBitsPerSample)
         debug(DWDM,"! --         wBitsPerSample: %d\n",WaveFormatEx->Format.wBitsPerSample);

      return STATUS_INVALID_PARAMETER;
     }

  out_type=0;

  current_freq=WaveFormatEx->Format.nSamplesPerSec;
  n_chn=WaveFormatEx->Format.nChannels;
  n_channels=n_chn;

  if(n_chn>REAL_WAVE_CHANNELS)
  {
   debug(DWDM,"waveout: too many channels [%d]\n",n_chn);
   return STATUS_INVALID_PARAMETER;
  }

  if((WaveFormatEx->Format.wBitsPerSample!=16) &&
      (WaveFormatEx->Format.wBitsPerSample!=8))
  {
       if(WaveFormatEx->Format.wBitsPerSample!=24 && WaveFormatEx->Format.wBitsPerSample!=32)
       {
        debug(DWDM,"!! non-24/32 bit per sample not supported [%d]\n",bps);
        return STATUS_INVALID_PARAMETER;
       }

       if(current_freq!=hw->card_frequency)
       {
        debug(DWDM,"!! 24/32 BPS supports only %d freq [%d]\n",hw->card_frequency,current_freq);
        return STATUS_INVALID_PARAMETER;
       }

       if(!hw->can_passthru || !hw->is_10k2)
       {
        debug(DWDM,"!! 24/32 BPS supported for passthru-enabled 10k2 cards only\n");
        return STATUS_INVALID_PARAMETER;
       }

       out_type|=OUT_BUFFERED;
       n_channels*=2;
  }

   if(n_channels>2)
   {
    out_type|=OUT_BUFFERED;
   }

   bps = WaveFormatEx->Format.wBitsPerSample;

   dword chn_mask=-1;

   if(WaveFormatEx->Format.wFormatTag==WAVE_FORMAT_EXTENSIBLE)
   {
    if(WaveFormatEx->Format.cbSize!=0)
    {
                // new data: wValidBitsPerSample, channelmask, subformat
                
                GUID *t=&WaveFormatEx->SubFormat;
                chn_mask=WaveFormatEx->dwChannelMask;

        if(IS_VALID_WAVEFORMATEX_GUID(t)) 
        {
         
         WaveFormatEx->Format.wFormatTag=EXTRACT_WAVEFORMATEX_ID(t);
        }
        else
        {
         debug(DWDM,"!!! waveout:Parse(): invalid subformat for WaveExtensible\n");
         return STATUS_INVALID_PARAMETER;
        }

        if(WaveFormatEx->Samples.wValidBitsPerSample!=bps && WaveFormatEx->Samples.wValidBitsPerSample!=0)
        {
         if(WaveFormatEx->Samples.wValidBitsPerSample==24 && bps==32) // ok
         {
           // ok
         }
         else
         {
          debug(DWDM,"!! waveout: valid bits per sample!=%d [%d]\n",bps,WaveFormatEx->Samples.wValidBitsPerSample);
          return STATUS_INVALID_PARAMETER;
         }
        }
    }
    else
    {
        debug(DWDM,"!! cbSize=0 for Extensible format\n");
        return STATUS_INVALID_PARAMETER;
    }
   }

   switch(WaveFormatEx->Format.wFormatTag)
   {
        case WAVE_FORMAT_PCM:
            break;

        case WAVE_FORMAT_DOLBY_AC3_SPDIF:
        case WAVE_FORMAT_RAW_SPORT:
        case WAVE_FORMAT_ESST_AC3:
            // debug(DWDM,"!!! waveformatex: ac3\n");
            if(
//                    (WaveFormatEx->Format.cbSize == 0) &&
                    (WaveFormatEx->Format.nChannels == 2) &&
                    (WaveFormatEx->Format.wBitsPerSample == 16) &&
                    (WaveFormatEx->Format.nSamplesPerSec == 48000)
                  )
                {
                   if(WaveFormatEx->Format.cbSize!=0)
                   {
                    if(chn_mask!=KSAUDIO_SPEAKER_STEREO)
                     debug(DWDM,"!! note: ac-3 + channelmask: %x\n",chn_mask);
                   }

                   if(hw->card_frequency!=48000)
                   {
                     debug(DWDM,"!! note: ac-3 cannot be enabled, because master clock is not 48kHz\n");
                     return STATUS_INVALID_PARAMETER;
                   }

                   if(!(hw->dsp_flags&KX_AC3_PASSTHROUGH))
                   {
                        out_type=OUT_AC3|OUT_BUFFERED;
                        bps=16;
                        current_freq=48000;
                        n_channels=6;
                        n_chn=2;
                   }
                   else
                   {
                    if(hw->ac3_pt_state.instance!=this) // format already set!
                    {
                        // auto-detect available passthru method
                        if(kx_init_passthru(hw,this,&ac3_pt_method)!=0)
                        {
                            // failed
                            debug(DWDM,"! failed to initialize ac-3 passthru - aborting...\n");
                            ac3_pt_method=0;
                            return STATUS_INVALID_PARAMETER;
                        }

                        if(ac3_pt_method==KX_AC3_PASSTHRU_XTRAM)
                        {
                         if(hw->ac3_pt_state.instance==this)
                         {
                            // initialize callback
                            hw->ac3_pt_state.callback=ac3_pt_handler;
                         }  
                          else
                          {
                           debug(DERR,"internal ac-3 pt init error [bad instance!]\n");
                           return STATUS_INVALID_PARAMETER;
                          }
                        }
                     }
                      else debug(DWDM,"note: ac-3 passthru already initialized for this device\n");
                   }
                }
                else
                {
                    debug(DWDM,"!!! waveformatex ac3 INVALID: nch:%d wbi:%d nsamp:%d cbsize:%d\n",
                     WaveFormatEx->Format.nChannels,
                     WaveFormatEx->Format.wBitsPerSample,
                     WaveFormatEx->Format.nSamplesPerSec,
                     WaveFormatEx->Format.cbSize);
                    return STATUS_INVALID_PARAMETER;
                }

            break;
    default:
    case WAVE_FORMAT_UNKNOWN:
        debug(DWDM,"!!! Unknown wFormatTag / SubFormatGUID %x\n",WaveFormatEx->Format.wFormatTag);
        return STATUS_INVALID_PARAMETER;
   }

   return STATUS_SUCCESS;
}

#pragma code_seg("PAGE")
STDMETHODIMP CMiniportWaveOutStream::SetFormat(IN PKSDATAFORMAT DataFormat_)
{
    PAGED_CODE();

    ASSERT(DataFormat_);

    if(out_type&OUT_AC3)
    {
        debug(DWDM,"!!! Dynamic AC3 format change!\n");
        return STATUS_UNSUCCESSFUL;
    }

    if(DataFormat_->FormatSize>sizeof(DataFormat2))
    {
        debug(DWDM,"!!! Not enough memory for DataFormat (%d)\n",DataFormat_->FormatSize);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    int oldRate=current_freq;
    int old_bps=bps;
    int was_nchannels=n_channels;
    int was_nchn=n_chn;

    memset(&DataFormat2,0,sizeof(DataFormat2));
    if(DataFormat_->FormatSize==sizeof(KSDATAFORMAT))
    {
     debug(DWDM,"!!! note: setFormat: format size invalid\n");
    }
    RtlCopyMemory(&DataFormat2,DataFormat_,DataFormat_->FormatSize);

    NTSTATUS ntStatus=Parse();

    if(NT_SUCCESS(ntStatus))
    {
        if(out_type&OUT_AC3)
            source_buffer_size=AC3_SRC_FRAME_SIZE*(int)cb_ac3_buffers;
        else
        {
            source_buffer_size=cb_pb_buffers*n_chn;

            if(bps>16)
             source_buffer_size*=2;
        }

        if(source_channel)
        {
           if(source_channel->AllocatedBufferSize()>=(ULONG)source_buffer_size)
           {
            source_channel->SetBufferSize(source_buffer_size);
           }
           else
           {
              source_channel->FreeBuffer();

              PHYSICAL_ADDRESS a;
              a.LowPart=0x7fffffff; // 2Gb limit
              a.HighPart=0x0;

              ntStatus = source_channel->AllocateBuffer(source_buffer_size,&a);
              if(!NT_SUCCESS(ntStatus))
              {
                 debug(DWDM,"!!! [CMiniportWaveOutStream::Init] :: failed to allocate buffer [%x] size=%x\n",ntStatus,source_buffer_size);
                 return STATUS_UNSUCCESSFUL;
              }
           }
        }

        if((old_bps!=bps) || (was_nchannels!=n_channels) || (was_nchn!=n_chn))
        {
            // changed
            if(channels[0]!=-1) // and already opened
            {
                debug(DWDM,"!!! mono/stereo 8/16 changed: Trying to re-open...\n");
                        Open();
                oldRate=current_freq; // to skip the next 'if'
                if(channels[0]==-1)
                 ntStatus=STATUS_INVALID_PARAMETER;
            }
                // else - ignore: first call
        }
        if((oldRate!=current_freq) && (oldRate!=-1)) // and not first call
        {
            if(channels[0]!=-1 && ac3_pt_method!=KX_AC3_PASSTHRU_XTRAM) // and already opened
            {
                // dynamic change
                // FIXME NOW: set_parameter_multiple()

                debug(DSTATE," -- Dynamic rate change to %d (%d)\n",current_freq,oldRate);
                for(int i=0;i<PHYS_WAVE_CHANNELS;i++)
                {
                 if(channels[i]!=-1)
                   kx_wave_set_parameter(hw,channels[i],0,KX_VOICE_PITCH,current_freq);
                         if(!(out_type&OUT_BUFFERED))
                  break;
                }
            }
        }
    }
    return ntStatus;
}

#pragma code_seg("PAGE")
STDMETHODIMP_(ULONG) CMiniportWaveOutStream::SetNotificationFreq(
    IN      ULONG   Interval,
    OUT     PULONG  FramingSize)
{
    PAGED_CODE();

    if(Interval!=DMA_DEFAULT_NOTIFY)
    {
     debug(DWDM,"!!! SetNotificationFreq !=10 ( %d )\n",Interval);
    }

    *FramingSize =
         bps * n_chn * 
         (current_freq * Interval / 1000) / 8;
 
    if(channels[0]>=0)
          kx_wave_set_notify(hw,channels[0],Interval);

    if(ac3_pt_method==KX_AC3_PASSTHRU_XTRAM) // we need to set freq here...
    {
     debug(DWDM,"setting ac3 pt timer [%d] for xtram...\n",Interval);
     kx_timer_install(hw,&ac3_timer,(dword)(hw->card_frequency*Interval/1000));
    }

    return Interval;
}

#if defined(_MSC_VER)
#pragma code_seg()
// this is required, since we call send_message() here, and it needs buffer to be in non-paged memory
#endif
STDMETHODIMP CMiniportWaveOutStream::SetState(IN KSSTATE NewState)
{
    int i=0;

    if (NewState == KSSTATE_ACQUIRE)
    {
        State = KSSTATE_PAUSE;
        return STATUS_SUCCESS;
    }

    debug(DSTATE,"New State [cur=%d] (%d)\n",State,NewState);

    switch(NewState)
    {
            case KSSTATE_STOP:
                if(State==KSSTATE_RUN)
                {
                 if(channels[0]>=0)
                 {
                         dword low=0,high=0;

                         for(i=0;i<PHYS_WAVE_CHANNELS;i++)
                                  if(channels[i]!=-1)
                                  {
                                   if(channels[i]>=32)
                                    high|=(1<<(channels[i]-32));
                                   else
                                    low|=(1<<channels[i]);

                                   if(!(out_type&OUT_BUFFERED))
                            break;
                                  }

                                  if(i==0)
                                   kx_wave_stop(hw,channels[0]);
                                  else
                                   kx_wave_stop_multiple(hw,channels[0],low,high);
                          }

                          if(ac3_pt_method==KX_AC3_PASSTHRU_XTRAM)
                           kx_timer_disable(hw,&ac3_timer);

                          if(ac3_pt_method)
                           kx_set_passthru(hw,this,0);

                          if(out_type&OUT_AC3)
                           hw->cb.send_message(hw->cb.call_with,KX_SYSEX_SIZE,KX_SYSEX_AC3DECODE_OFF);
                         }
                          // FIXME:
                          // should we 'rewind' the hardware position to zero?..
                          for(i=0;i<PHYS_WAVE_CHANNELS;i++)
                          {
                            if(channels[i]!=-1)
                              kx_wave_set_position(hw,channels[i],0);
                          }
                          if(out_type&OUT_BUFFERED)
                          {
                            cur_ac3_buffer=-1;
                          }
                break;
            case KSSTATE_RUN:
                if(State!=KSSTATE_RUN)
                {
                         if(ac3_pt_method)
                            kx_set_passthru(hw,this,1);

                         if(out_type&OUT_AC3)
                            hw->cb.send_message(hw->cb.call_with,KX_SYSEX_SIZE,KX_SYSEX_AC3DECODE_ON);

                         if(channels[0]>=0)
                         {
                             dword low=0,high=0;

                             for(i=0;i<PHYS_WAVE_CHANNELS;i++)
                             {
                              if(channels[i]!=-1) 
                              {
                               if(channels[i]>=32)
                                high|=(1<<(channels[i]-32));
                               else
                                low|=(1<<channels[i]);

                               if(!(out_type&OUT_BUFFERED))
                                break;
                              }
                             }

                             if(i==0)
                              kx_wave_start(hw,channels[0]);
                             else
                              kx_wave_start_multiple(hw,channels[0],low,high);
                         }
                         
                         if(ac3_pt_method==KX_AC3_PASSTHRU_XTRAM)
                          kx_timer_enable(hw,&ac3_timer);
                }
                break;
            case KSSTATE_PAUSE:
                if(State!=KSSTATE_PAUSE)
                {
                 if(channels[0]>=0)
                 {
                         dword low=0,high=0;

                         for(i=0;i<PHYS_WAVE_CHANNELS;i++)
                                  if(channels[i]!=-1)
                                  {
                                   if(channels[i]>=32)
                                    high|=(1<<(channels[i]-32));
                                   else
                                    low|=(1<<channels[i]);

                                   if(!(out_type&OUT_BUFFERED))
                            break;
                                  }

                                  if(i==0)
                                   kx_wave_stop(hw,channels[0]);
                                  else
                                   kx_wave_stop_multiple(hw,channels[0],low,high);
                           }

                           if(ac3_pt_method==KX_AC3_PASSTHRU_XTRAM)
                            kx_timer_disable(hw,&ac3_timer);

                          if(ac3_pt_method)
                           kx_set_passthru(hw,this,0);

                          if(out_type&OUT_AC3)
                           hw->cb.send_message(hw->cb.call_with,KX_SYSEX_SIZE,KX_SYSEX_AC3DECODE_OFF);
                }
                break;
            default:
                debug(DWDM,"!!! [CMiniportWaveOutStream::SetState] :: -- Unknown state %d\n",NewState);
                break;
    }
    State=NewState;

    return STATUS_SUCCESS;
}

#pragma code_seg()    
STDMETHODIMP_(void) CMiniportWaveOutStream::Silence(IN      PVOID   Buffer,    IN      ULONG   ByteCount)
{
  RtlFillMemory(Buffer,ByteCount, (bps==8)? 0x80 : 0);
}


#pragma code_seg()
#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
STDMETHODIMP CMiniportWaveOutStream::GetPosition(OUT     PULONG  Position)
{
    ASSERT(Position);

    int current_position=0;

    if(channels[0]!=-1)
    {
      kx_wave_get_position(hw,channels[0],&current_position);

      // current_position is in 'bytes'
      if(out_type&OUT_BUFFERED)
      {
        // correct according to 'virtual' channels
        current_position=current_position*n_chn;
        // correct according to bps
        current_position=(current_position*bps/8)/2; // /2, since original pos is in bytes, need samples multiplied by bps
      }
    }
    if(ac3_pt_method==KX_AC3_PASSTHRU_XTRAM)
    {
      current_position=ac3_pt_pos;
    }

    (*Position)=current_position;

   return STATUS_SUCCESS;
}

#pragma code_seg()
STDMETHODIMP CMiniportWaveOutStream::NormalizePhysicalPosition(IN OUT PLONGLONG PhysicalPosition)
{                           
    debug(DWDM,"!!! [Normalize]\n");
    *PhysicalPosition =
            (_100NS_UNITS_PER_SECOND / 
                bps * n_chn * (*PhysicalPosition) / 8) /
                    current_freq;
    return STATUS_SUCCESS;
}

#pragma code_seg("PAGE")
STDMETHODIMP CMiniportWaveOutStream::NonDelegatingQueryInterface(IN      REFIID  Interface,
                                OUT     PVOID * Object)
{
    PAGED_CODE();

    ASSERT(Object);

    if (IsEqualGUIDAligned(Interface,IID_IUnknown))
    {
        *Object = PVOID(PUNKNOWN(PMINIPORTWAVECYCLICSTREAM(this)));
    }
    else if (IsEqualGUIDAligned(Interface, IID_IDrmAudioStream))
    {
        *Object = (PVOID)(PDRMAUDIOSTREAM)this;
    }
    else
    if (IsEqualGUIDAligned(Interface,IID_IMiniportWaveCyclicStream))
    {
        *Object = PVOID(PMINIPORTWAVECYCLICSTREAM(this));
    }
    else
    {
    debug(DWDM,"!!! - (WaveOutStream) try to delegate to ref_iid unknown\n");
        debug(DWDM,"!!! -- refid:"
        "%x.%x.%x-%x-%x-%x-%x-%x-%x-%x-%x\n",
          Interface.Data1,
          Interface.Data2,
          Interface.Data3,
          Interface.Data4[0],
          Interface.Data4[1],
          Interface.Data4[2],
          Interface.Data4[3],
          Interface.Data4[4],
          Interface.Data4[5],
          Interface.Data4[6],
          Interface.Data4[7]);

        *Object = NULL;
    }

    debug(DWDM,"[CMiniportWaveStream::NonDelegatingQueryInterface]\n");

    if (*Object)
    {
        PUNKNOWN(*Object)->AddRef();
        return STATUS_SUCCESS;
    }

    return STATUS_INVALID_PARAMETER;
}

#pragma code_seg("PAGE")
NTSTATUS CMiniportWaveOutStream::PropertyVolume(IN PPCPROPERTY_REQUEST PropertyRequest)
{
    PAGED_CODE();

    CMiniportWave *that =
        (CMiniportWave *)(PMINIPORTWAVECYCLIC)PropertyRequest->MajorTarget;

    if(!that)
    {
     debug(DWDM,"!!! Wave majortarget=0! :: vol\n");
     return STATUS_INVALID_PARAMETER;
    }

    if(that->magic!=WAVE_MAGIC)
    {
     debug(DWDM,"!!! Bad wave magic!\n");
     return STATUS_INVALID_PARAMETER;
    }

    PUNKNOWN pu=PropertyRequest->MinorTarget;
    CMiniportWaveOutStream *that2=(CMiniportWaveOutStream *)PMINIPORTWAVECYCLICSTREAM(pu);

    if(that2!=NULL)
    {
     if(that2->magic!=WAVEOUTSTREAM_MAGIC)
     {
      debug(DWDM,"!!! Bad wavestream magic x2\n");
      return STATUS_INVALID_PARAMETER;
     }
    }
    else
    {
     PropertyRequest->Verb&=KSPROPERTY_TYPE_BASICSUPPORT;
     if(!PropertyRequest->Verb)
     {
      // debug(DWDM,"!!! No minorTarget in wave::vol!\n");
      // 3551 FIXME
      // Windows 7 uses this property!
      return STATUS_INVALID_PARAMETER;
     }
    }

    NTSTATUS        ntStatus = STATUS_INVALID_PARAMETER;
    int i=0;

    if(that2 && (that2->ac3_pt_method))
    {
     debug(DWDM,"!!! vol for ac3 passthru!\n");
     return STATUS_INVALID_PARAMETER;
    }

    // validate node
    if(PropertyRequest->Node != ULONG(-1))
    {
        if(PropertyRequest->Verb & KSPROPERTY_TYPE_GET)
        {
            LONG channel=-1;
            // get the instance channel parameter
            if(PropertyRequest->InstanceSize >= sizeof(LONG))
            {
                channel = *(PLONG(PropertyRequest->Instance));

                if(channel==-1)
                 { channel=0; debug(DWDM,"!!! Note: get_vol and chn# is -1 -> set to '0'\n"); }

                if(channel>=MAX_TOPOLOGY_CHANNELS || channel<0)
                 { debug(DWDM,"!!! WaveLevel:: Channel is %d\n",channel); return ntStatus; }

                // validate and get the output parameter
                if (PropertyRequest->ValueSize == sizeof(LONG))
                {
                    PLONG Level = (PLONG)PropertyRequest->Value;

                    // check if volume property request
                    if(PropertyRequest->PropertyItem->Id == KSPROPERTY_AUDIO_VOLUMELEVEL)
                    {
                                switch(PropertyRequest->Node)
                                {
                                  case WAVE_VOLUME1:
                                    *Level=that2->vol1[channel];
                                    debug(DPROP,"Get wave Vol1 to %d channel=%d\n",(*Level),channel);
                                    break;
                                  case WAVE_VOLUME2:
                                        *Level=that2->vol2[channel];
                                        debug(DPROP,"Get wave Vol2 to %d channel=%d\n",(*Level),channel);
                                    break;
                                  default:
                                    debug(DWDM,"!!! Wave::Level get unknown %d\n",PropertyRequest->Node);
                                    break;
                                }
                                PropertyRequest->ValueSize = sizeof(LONG);
                                ntStatus = STATUS_SUCCESS;
                    }
                } else debug(DWDM,"!!! NB get more than one channel!\n");
            } else debug(DWDM,"!!! Buffer too small in wave::vollevel\n");

        } else if(PropertyRequest->Verb & KSPROPERTY_TYPE_SET)
        {
            LONG channel_s=-1,channel_e=-1;

            // get the instance channel parameter
            if(PropertyRequest->InstanceSize >= sizeof(LONG))
            {
                channel_s = *(PLONG(PropertyRequest->Instance));

                if(channel_s==-1)
                 { channel_s=0; channel_e=MAX_TOPOLOGY_CHANNELS-1; }
                else
                 { channel_e=channel_s; }

                if(channel_s>=MAX_TOPOLOGY_CHANNELS || channel_s<0 ||
                   channel_e>=MAX_TOPOLOGY_CHANNELS || channel_e<0)
                 { debug(DWDM,"!!! WaveLevel:: Channel is %d\n",channel_s); return ntStatus; }

                //debug(DWDM,"-- vol/pan chn: %x %x %x\n",
                //  *(PLONG(PropertyRequest->Instance)),channel_s,channel_e);

                // validate and get the input parameter
                if (PropertyRequest->ValueSize == sizeof(LONG))
                {
                    PLONG Level = (PLONG)PropertyRequest->Value;

                    if(PropertyRequest->PropertyItem->Id == KSPROPERTY_AUDIO_VOLUMELEVEL)
                    {
                        // switch on the node id
                            switch(PropertyRequest->Node)
                            {
                                  case WAVE_VOLUME1:
                                    int st;

                                    if((!(that->hw->ext_flags&KX_HW_WAVE_MULTICHANNEL_COMPAT)) && channel_s<2 && (channel_s==channel_e))
                                    {
                                     st=2;
                                         channel_e=MAX_TOPOLOGY_CHANNELS-1;
                                         if(channel_e>=that2->n_channels)
                                          channel_e=that2->n_channels-1;
                                    }
                                    else
                                     st=1;

                                    for(i=channel_s;i<=channel_e;i+=st)
                                    {
                                      that2->vol1[i]=*Level;
                                      if((that2->vol1[i]<KX_MIN_VOLUME) && (that2->vol1[i]!=0x80000000U) && (that2->vol1[i]!=0))
                                       that2->vol1[i]=KX_MIN_VOLUME;

                                      debug(DPROP,"set wave Vol1 to %d channel=%d logic\n",(*Level),i);
                                    }
//                                      if(!(that2->out_type&OUT_AC3)) // simple stereo, mono or multichannel
                                    {
                                     for(i=channel_s;i<=channel_e;i+=st)
                                     {
                                      debug(DPROP,"set wave Vol1 to %d channel=%d physically\n",(*Level),i);

                                      if(that2->out_type&OUT_BUFFERED)
                                           kx_wave_set_parameter(that->hw,that2->channels[i],0,KX_VOICE_VOLUME,(dword)((int)that2->vol1[i]+that2->kx3d_attn));
                                          else
                                          {
                                           kx_wave_set_parameter(that->hw,that2->channels[0],i,KX_VOICE_VOLUME,(dword)((int)that2->vol1[i]+that2->kx3d_attn));
                                           if(i==1) 
                                             break;
                                          }
                                         }
                                    }
//                                      else
//                                      {
//                                        debug(DWDM,"cannot set volume for ac-3 streams\n");
//                                      }
                                    break;
                                  case WAVE_VOLUME2:
                                     for(i=channel_s;i<=channel_e;i++)
                                     {
                                          that2->vol2[i]=*Level;
                                          debug(DPROP,"set wave Vol2 to %d channel=%d logic\n",(*Level),i);
                                         }

                                        if(!(that2->out_type&OUT_BUFFERED))
                                    {
                                     for(i=channel_s;i<=((channel_e<=1)?channel_e:2);i++)
                                     {
                                       kx_wave_set_parameter(that->hw,that2->channels[0],i,KX_VOICE_PAN,that2->vol2[i]);
                                       debug(DPROP,"set wave Vol2 to %d channel=%d phys\n",(*Level),i);
                                     }
                                    } 
                                        else
                                        {
                                          debug(DWDM,"!! set wave:Vol2 [buffered/ac3] to %d channel=%d:%d NOT SUPPORTED\n",(*Level),channel_s,channel_e);
                                          return STATUS_INVALID_PARAMETER;
                                        }
                                    break;
                                  default:
                                    debug(DWDM,"!!! set unknown node=%d\n",PropertyRequest->Node);
                                    break;
                            }
                            ntStatus = STATUS_SUCCESS;
                    } else debug(DWDM,"!!! Not vollevel in wave::vollevel\n");
                } else debug(DWDM,"!!! Set more than one channel\n");
            } else debug(DWDM,"!!! wrong bufsize in ::vollevel\n");

        } else if(PropertyRequest->Verb & KSPROPERTY_TYPE_BASICSUPPORT)
        {
            // service basic support request
            switch(PropertyRequest->Node)
            {
                case WAVE_VOLUME1:
                case WAVE_VOLUME2:
                    // debug(DWDM,"WaveBasic support call for %d\n",PropertyRequest->Node);
                    if(PropertyRequest->PropertyItem->Id == KSPROPERTY_AUDIO_VOLUMELEVEL)
                    {
                        ntStatus = BasicSupportHandler(PropertyRequest);
                    } else debug(DWDM,"!!! Unknown request of BasicSupport\n");
                    break;
                default:
                  debug(DWDM,"!!! basic support unknown %d\n",PropertyRequest->Node);
                  break;
            }
        } else debug(DWDM,"!!! Unknown verb in PropertyLevel\n");
    } else debug(DWDM,"!!! Node is -1 in wave::vollevel\n");

    if(!NT_SUCCESS(ntStatus))
      debug(DWDM,"!!! NOT OK in Wave::PropertyVolume\n");

    return ntStatus;
}

#pragma code_seg("PAGE")
NTSTATUS CMiniportWaveOutStream::PropertySamplingRate(IN PPCPROPERTY_REQUEST PropertyRequest)
{
    PAGED_CODE();

    CMiniportWave *that =
        (CMiniportWave *)(PMINIPORTWAVECYCLIC) PropertyRequest->MajorTarget;

    if(!that)
    {
     debug(DWDM,"!!! Wave majortarget=0! :: sample_rate\n");
     return STATUS_INVALID_PARAMETER;
    }

    if(that->magic!=WAVE_MAGIC)
    {
     debug(DWDM,"!!! Bad wave magic!\n");
     return STATUS_INVALID_PARAMETER;
    }

    PUNKNOWN pu=PropertyRequest->MinorTarget;
    CMiniportWaveOutStream *that2=(CMiniportWaveOutStream *)PMINIPORTWAVECYCLICSTREAM(pu);

    if(that2!=NULL)
    {
     if(that2->magic!=WAVEOUTSTREAM_MAGIC)
     {
      debug(DWDM,"!!! Bad wavestream magic x2\n");
      return STATUS_INVALID_PARAMETER;
     }
    }
    else
    {
     if(PropertyRequest->Verb != KSPROPERTY_TYPE_BASICSUPPORT &&
       PropertyRequest->PropertyItem->Id != KSPROPERTY_AUDIO_QUALITY)
     {
      debug(DWDM,"!!! No minorTarget! in wave::sampl_rate[%x];%x\n",PropertyRequest->Node,
       PropertyRequest->PropertyItem->Id);
      return STATUS_INVALID_PARAMETER;
     }
    }

    if((that2->out_type&OUT_AC3) || (that2->ac3_pt_method))
    {
        debug(DWDM,"!!! AC-3 Dynamic Sampling rate not applicable!\n"); 
        return STATUS_INVALID_PARAMETER;
    }

    NTSTATUS ntStatus = STATUS_INVALID_PARAMETER;

    // validate node
    if(PropertyRequest->Node != ULONG(-1))
    {
        if(PropertyRequest->Verb & KSPROPERTY_TYPE_GET)
        {
                // validate and get the output parameter
                if (PropertyRequest->ValueSize >= sizeof(LONG))
                {
                    // check if volume property request
                    switch(PropertyRequest->PropertyItem->Id)
                    {
                        case KSPROPERTY_AUDIO_SAMPLING_RATE:
                            // FIXME: multy support
                            kx_wave_get_parameter(that2->hw,
                                                      that2->channels[0],
                                                      0,KX_VOICE_PITCH,
                                                      (PULONG)PropertyRequest->Value);
                                PropertyRequest->ValueSize = sizeof(LONG);
                                ntStatus = STATUS_SUCCESS;
                                break;
                        case KSPROPERTY_AUDIO_DYNAMIC_SAMPLING_RATE:
                            debug(DPROP," -- get audio_dynamic_sampling_rate\n");
                            *(PLONG)PropertyRequest->Value=TRUE;
                PropertyRequest->ValueSize = sizeof(LONG);
                                ntStatus = STATUS_SUCCESS;
                                break;                          
                        case KSPROPERTY_AUDIO_QUALITY:
                            debug(DPROP," -- get audio_quality\n");
                            *(PLONG)PropertyRequest->Value=KSAUDIO_QUALITY_ADVANCED;
                PropertyRequest->ValueSize = sizeof(LONG);
                                ntStatus = STATUS_SUCCESS;
                                break;
                        default:
                            debug(DWDM,"!!! Unknown GET in wave::SamplingRate\n");
                            break;
                    }
                } else debug(DWDM,"!!! value size in property SampleRate\n");
        } else if(PropertyRequest->Verb & KSPROPERTY_TYPE_SET)
        {
                // validate and get the input parameter
                if (PropertyRequest->ValueSize == sizeof(LONG))
                {
                    switch(PropertyRequest->PropertyItem->Id)
                    {
                        case KSPROPERTY_AUDIO_SAMPLING_RATE:
                                if(!(that2->out_type&OUT_AC3))
                                {
                                 if(!(that2->out_type&OUT_BUFFERED))
                                 {
                                        ntStatus = STATUS_SUCCESS;
                                        debug(DPROP," -- set audio_sampling_rate to %d\n",
                                            *(PULONG)PropertyRequest->Value);
                                        that2->current_freq=*(int *)PropertyRequest->Value;
                                    kx_wave_set_parameter(that2->hw,
                                                          that2->channels[0],
                                                          0,KX_VOICE_PITCH,
                                                          that2->current_freq*that2->current_freq_multiplier/DOPPLER_MULT);
                                 }
                                 else
                                 {
                                     ntStatus=STATUS_SUCCESS;
                                     that2->current_freq=*(int *)PropertyRequest->Value;

                                     for(int ch=0;ch<that2->n_channels;ch++)
                                     {
                                      kx_wave_set_parameter(that2->hw,
                                                     that2->channels[ch],
                                                     0,KX_VOICE_PITCH,
                                                     that2->current_freq*that2->current_freq_multiplier/DOPPLER_MULT);
                                     }
                                 }
                                }
                                 else debug(DWDM,"cannot change AC-3 stream sampling rate [to %d]\n",*(PULONG)PropertyRequest->Value);
                                break;
                        case KSPROPERTY_AUDIO_DYNAMIC_SAMPLING_RATE:
                            debug(DPROP," -- set audio_dynamic_sampling_rate %d\n",*(PLONG)PropertyRequest->Value);
                                ntStatus = STATUS_SUCCESS;
                                break;
                        case KSPROPERTY_AUDIO_QUALITY:
                            debug(DPROP," -- set audio_quality\n");
                                ntStatus = STATUS_SUCCESS;
                                if((ULONG)*(PLONG)PropertyRequest->Value!=KSAUDIO_QUALITY_ADVANCED)
                                  debug(DWDM,"!!! Trying to set AudioQuality = %d\n",*(PLONG)PropertyRequest->Value);
                                break;
                        default:
                            debug(DWDM,"!!! unknown set in wave::SamplingRate\n");
                            break;
                    }
                } else  debug(DWDM,"!!! value size in ::samplerate\n");
        } else debug(DWDM,"!!! Unknown verb in wave::SamplingRate\n");
    } else debug(DWDM,"!!! Node=-1\n");

    if(!NT_SUCCESS(ntStatus))
      debug(DWDM,"!!! NOT OK in Wave::SamplingRate\n");

    return ntStatus;
}

#include "wave3d.cpp"
#include "wave_eax.cpp"

int eax10to30_trans_l[EAX10_NUM_STANDARD_PROPERTIES]=
{
 DSPROPERTY_EAX10_ALL+EAX10_OFFSET,
 DSPROPERTY_EAX30LISTENER_ENVIRONMENT,
 DSPROPERTY_EAX10_VOLUME+EAX10_OFFSET,
 DSPROPERTY_EAX30LISTENER_DECAYTIME,
 DSPROPERTY_EAX10_DAMPING+EAX10_OFFSET
};
  
#pragma code_seg("PAGE")
NTSTATUS CMiniportWaveOutStream::PropertyEAX1L(IN      PPCPROPERTY_REQUEST PropertyRequest)
{
 PAGED_CODE();

 if((unsigned int)PropertyRequest->PropertyItem->Id<EAX10_NUM_STANDARD_PROPERTIES)
 {
  int id=eax10to30_trans_l[PropertyRequest->PropertyItem->Id];
  return PropertyEAL(id,PropertyRequest); 
 }
 else
 {
   debug(DWDM,"!!! invalid EAX 1.0 listener property -- for node: %d; prop: %d verb: %x\n",PropertyRequest->Node,
     PropertyRequest->PropertyItem->Id,PropertyRequest->Verb);
   return  STATUS_NOT_IMPLEMENTED;
 }
}

#pragma code_seg("PAGE")
NTSTATUS CMiniportWaveOutStream::PropertyEAX1B(IN      PPCPROPERTY_REQUEST PropertyRequest)
{
 PAGED_CODE();

 int id;
 switch(PropertyRequest->PropertyItem->Id)
 {
  case DSPROPERTY_EAX10BUFFER_ALL:
   id=DSPROPERTY_EAX10BUFFER_ALL+EAX10_OFFSET;
   break;
  case DSPROPERTY_EAX10BUFFER_REVERBMIX:
   id=DSPROPERTY_EAX10BUFFER_REVERBMIX+EAX10_OFFSET;
   break;
  default:
   debug(DWDM,"!!! invalid EAX 1.0 source property -- for node: %d; prop: %d verb: %x\n",PropertyRequest->Node,
         PropertyRequest->PropertyItem->Id,PropertyRequest->Verb);
   return  STATUS_NOT_IMPLEMENTED;
 }
 return PropertyEAB(id,PropertyRequest); 
}

int eax20to30_trans_l[DSPROPERTY_EAX20LISTENER_FLAGS+1]=
{
    DSPROPERTY_EAX30LISTENER_NONE,
    DSPROPERTY_EAX20LISTENER_ALLPARAMETERS+EAX20_OFFSET,
    DSPROPERTY_EAX30LISTENER_ROOM,
    DSPROPERTY_EAX30LISTENER_ROOMHF,
    DSPROPERTY_EAX30LISTENER_ROOMROLLOFFFACTOR,
    DSPROPERTY_EAX30LISTENER_DECAYTIME,
    DSPROPERTY_EAX30LISTENER_DECAYHFRATIO,
    DSPROPERTY_EAX30LISTENER_REFLECTIONS,
    DSPROPERTY_EAX30LISTENER_REFLECTIONSDELAY,
    DSPROPERTY_EAX30LISTENER_REVERB,
    DSPROPERTY_EAX30LISTENER_REVERBDELAY,
    DSPROPERTY_EAX30LISTENER_ENVIRONMENT,
    DSPROPERTY_EAX30LISTENER_ENVIRONMENTSIZE,
    DSPROPERTY_EAX30LISTENER_ENVIRONMENTDIFFUSION,
    DSPROPERTY_EAX30LISTENER_AIRABSORPTIONHF,
    DSPROPERTY_EAX20LISTENER_FLAGS+EAX20_OFFSET
};

#pragma code_seg("PAGE")
NTSTATUS CMiniportWaveOutStream::PropertyEAX2L(IN      PPCPROPERTY_REQUEST PropertyRequest)
{
 PAGED_CODE();

 if((unsigned int)PropertyRequest->PropertyItem->Id<=DSPROPERTY_EAX20LISTENER_FLAGS)
 {
  int id=eax20to30_trans_l[PropertyRequest->PropertyItem->Id];
  return PropertyEAL(id,PropertyRequest);
 }
 else
 {
   debug(DWDM,"!!! invalid EAX 2.0 listener property -- for node: %d; prop: %d verb: %x\n",PropertyRequest->Node,
     PropertyRequest->PropertyItem->Id,PropertyRequest->Verb);
   return  STATUS_NOT_IMPLEMENTED;
 }
}

#pragma code_seg("PAGE")
NTSTATUS CMiniportWaveOutStream::PropertyEAX3L(IN      PPCPROPERTY_REQUEST PropertyRequest)
{
 PAGED_CODE();
 return PropertyEAL(PropertyRequest->PropertyItem->Id,PropertyRequest);
}

int i3dltoeax30_trans_l[DSPROPERTY_I3DL2LISTENER_HFREFERENCE+1]=
{
DSPROPERTY_I3DL2LISTENER_ALL+I3D_OFFSET,
DSPROPERTY_EAX30LISTENER_ROOM,
DSPROPERTY_EAX30LISTENER_ROOMHF,
DSPROPERTY_EAX30LISTENER_ROOMROLLOFFFACTOR,
DSPROPERTY_EAX30LISTENER_DECAYTIME,
DSPROPERTY_EAX30LISTENER_DECAYHFRATIO,
DSPROPERTY_EAX30LISTENER_REFLECTIONS,
DSPROPERTY_EAX30LISTENER_REFLECTIONSDELAY,
DSPROPERTY_EAX30LISTENER_REVERB,
DSPROPERTY_EAX30LISTENER_REVERBDELAY,
DSPROPERTY_I3DL2LISTENER_DIFFUSION+I3D_OFFSET,
DSPROPERTY_I3DL2LISTENER_DENSITY+I3D_OFFSET,
DSPROPERTY_EAX30LISTENER_HFREFERENCE
};

#pragma code_seg("PAGE")
NTSTATUS CMiniportWaveOutStream::PropertyI3DL2L(IN      PPCPROPERTY_REQUEST PropertyRequest)
{
 PAGED_CODE();
 if((unsigned int)PropertyRequest->PropertyItem->Id<=DSPROPERTY_I3DL2LISTENER_HFREFERENCE)
 {
  int id=i3dltoeax30_trans_l[PropertyRequest->PropertyItem->Id];
  return PropertyEAL(id,PropertyRequest);
 }
 else
 {
   debug(DWDM,"!!! invalid I3DL2L listener property -- for node: %d; prop: %d verb: %x\n",PropertyRequest->Node,
     PropertyRequest->PropertyItem->Id,PropertyRequest->Verb);
   return  STATUS_NOT_IMPLEMENTED;
 }
}

int eax20toeax30_trans_b[DSPROPERTY_EAX20BUFFER_FLAGS+1]=
{
    DSPROPERTY_EAX30BUFFER_NONE,
    DSPROPERTY_EAX20BUFFER_ALLPARAMETERS+EAX20_OFFSET,
    DSPROPERTY_EAX30BUFFER_DIRECT,
    DSPROPERTY_EAX30BUFFER_DIRECTHF,
    DSPROPERTY_EAX30BUFFER_ROOM,
    DSPROPERTY_EAX30BUFFER_ROOMHF, 
    DSPROPERTY_EAX30BUFFER_ROOMROLLOFFFACTOR,
    DSPROPERTY_EAX30BUFFER_OBSTRUCTION,
    DSPROPERTY_EAX30BUFFER_OBSTRUCTIONLFRATIO,
    DSPROPERTY_EAX30BUFFER_OCCLUSION, 
    DSPROPERTY_EAX30BUFFER_OCCLUSIONLFRATIO,
    DSPROPERTY_EAX30BUFFER_OCCLUSIONROOMRATIO,
    DSPROPERTY_EAX30BUFFER_OUTSIDEVOLUMEHF,
    DSPROPERTY_EAX30BUFFER_AIRABSORPTIONFACTOR,
    DSPROPERTY_EAX30BUFFER_FLAGS
};

#pragma code_seg("PAGE")
NTSTATUS CMiniportWaveOutStream::PropertyEAX2B(IN      PPCPROPERTY_REQUEST PropertyRequest)
{
 PAGED_CODE();

 if((unsigned int)PropertyRequest->PropertyItem->Id<=DSPROPERTY_EAX20BUFFER_FLAGS)
 {
  int id=eax20toeax30_trans_b[PropertyRequest->PropertyItem->Id];
  return PropertyEAB(id,PropertyRequest);
 }
 else
 {
   debug(DWDM,"!!! invalid EAX2.0 buffer property -- for node: %d; prop: %d verb: %x\n",PropertyRequest->Node,
     PropertyRequest->PropertyItem->Id,PropertyRequest->Verb);
   return  STATUS_NOT_IMPLEMENTED;
 }
}

#pragma code_seg("PAGE")
NTSTATUS CMiniportWaveOutStream::PropertyEAX3B(IN      PPCPROPERTY_REQUEST PropertyRequest)
{
 PAGED_CODE();
 return PropertyEAB(PropertyRequest->PropertyItem->Id,PropertyRequest);
}

int i3dltoeax30_trans_b[DSPROPERTY_I3DL2BUFFER_OCCLUSIONLFRATIO+1]=
{
DSPROPERTY_I3DL2BUFFER_ALL+I3D_OFFSET,
DSPROPERTY_EAX30BUFFER_OBSTRUCTIONPARAMETERS,
DSPROPERTY_I3DL2BUFFER_OCCLUSIONALL+I3D_OFFSET,
DSPROPERTY_EAX30BUFFER_DIRECT,
DSPROPERTY_EAX30BUFFER_DIRECTHF,
DSPROPERTY_EAX30BUFFER_ROOM,
DSPROPERTY_EAX30BUFFER_ROOMHF,
DSPROPERTY_EAX30BUFFER_ROOMROLLOFFFACTOR,
DSPROPERTY_EAX30BUFFER_OBSTRUCTION,
DSPROPERTY_EAX30BUFFER_OBSTRUCTIONLFRATIO,
DSPROPERTY_EAX30BUFFER_OCCLUSION,
DSPROPERTY_EAX30BUFFER_OCCLUSIONLFRATIO
};

#pragma code_seg("PAGE")
NTSTATUS CMiniportWaveOutStream::PropertyI3DL2B(IN      PPCPROPERTY_REQUEST PropertyRequest)
{
 PAGED_CODE();
 if((unsigned int)PropertyRequest->PropertyItem->Id<=DSPROPERTY_I3DL2BUFFER_OCCLUSIONLFRATIO)
 {
  int id=i3dltoeax30_trans_b[PropertyRequest->PropertyItem->Id];
  return PropertyEAL(id,PropertyRequest);
 }
 else
 {
   debug(DWDM,"!!! invalid I3DL2L buffer property -- for node: %d; prop: %d verb: %x\n",PropertyRequest->Node,
     PropertyRequest->PropertyItem->Id,PropertyRequest->Verb);
   return  STATUS_NOT_IMPLEMENTED;
 }
}

#pragma code_seg()
void CMiniportWaveOutStream::ac3_timer_func(void *data,int /*what*/)
{
 CMiniportWaveOutStream *that=(CMiniportWaveOutStream *)data; 

 that->my_isr();
}

#pragma code_seg()
#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
void CMiniportWaveOutStream::ac3_pt_handler(void *t)
{
 CMiniportWaveOutStream *that=(CMiniportWaveOutStream *)t;

 // read dbac
 dword dbac=0;

 kx_get_dsp_register(that->hw,that->hw->ac3_pt_state.pgm_id,that->hw->ac3_pt_state.dbac_reg,&dbac);

 /*
 dword cntr=0;
 kx_get_dsp_register(hw,hw->ac3_pt_state.pgm_id,hw->ac3_pt_state.counter_reg,&cntr);
 current_position=(cur_ac3_buffer*hw->ac3_pt_state.buf_size+cntr);
 debug(DWDM,"cntr: %x pos: %x final: %x\n",
  cntr,current_position,current_position*source_buffer_size/(hw->ac3_pt_state.buf_size*2));
 // current_position is 0..buf_size*2
 current_position=current_position*source_buffer_size/(hw->ac3_pt_state.buf_size*2);
 */

 if(that->cur_ac3_buffer==-1) // first call
 {
  that->cur_ac3_buffer=0;
  that->ac3_pt_pos=0;
 }
 else
 {
  that->cur_ac3_buffer=1-that->cur_ac3_buffer;
  that->ac3_pt_pos+=that->hw->ac3_pt_state.buf_size*2*2;
 }

 if(that->ac3_pt_pos>=that->source_buffer_size)
  that->ac3_pt_pos-=that->source_buffer_size;

 debug(DWDM,"[kxdsp irq: dbac: %x : %d; pos: %x]\n",dbac,that->cur_ac3_buffer,that->ac3_pt_pos);

 // write data:
 //  dbac+buf_size << left
 //  dbac+buf_size*3 << right
 //  [size: buf_size (*2 total)]
}
