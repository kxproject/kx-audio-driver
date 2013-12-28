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

#pragma code_seg()
NTSTATUS CMiniportWaveInStream::Open()
{
        kx_voice_buffer buffer;
        buffer.size=buffer_size;
        buffer.physical=DmaChannel->PhysicalAddress().LowPart;

        if(DmaChannel->PhysicalAddress().HighPart || DmaChannel->PhysicalAddress().LowPart&0x80000000)
            debug(DWDM,"!! [CMiniportWaveInStream::Open]: physical address out of limits (%x.%x)\n",
                    DmaChannel->PhysicalAddress().HighPart,
                    DmaChannel->PhysicalAddress().LowPart);

        buffer.addr=DmaChannel->SystemAddress();
        buffer.that=this;
        buffer.notify=DMA_DEFAULT_NOTIFY; // 10 ms

        is_rec=!kx_rec_open(hw,
             &buffer,(isStereo?VOICE_FLAGS_STEREO:0)|VOICE_FLAGS_16BIT,current_freq,rec_subdevice);
        if(is_rec)
          return STATUS_SUCCESS;
        else
        {
          return STATUS_UNSUCCESSFUL;
        }
}

#pragma code_seg()
void CMiniportWaveInStream::my_isr(void)
{
 Miniport->Port->Notify(ServiceGroup);
}

#pragma code_seg()
static inline check_rec_option(int buf)
{
/*        if(buf<=384) return 384; else 
        if(buf<=448) return 448; else 
        if(buf<=512) return 512; else 
        if(buf<=640) return 640; else 
        if(buf<=768) return 768; else 
        if(buf<=896) return 896; else 
        if(buf<=1024) return 1024; else 
        if(buf<=1280) return 1280; else 
        if(buf<=1536) return 1536; else 
        if(buf<=1792) return 1792; else 
        if(buf<=2048) return 2048; else 
        if(buf<=2560) return 2560; else 
        if(buf<=3072) return 3072; else 
        if(buf<=3584) return 3584; else 
        if(buf<=4096) return 4096; else 
        if(buf<=5120) return 5120; else 
        if(buf<=6144) return 6144; else 
        if(buf<=7168) return 7168; else 
        if(buf<=8192) return 8192; else 
        if(buf<=10240) return 10240; else
        if(buf<=12288) return 12288; else
        if(buf<=14366) return 14366; else
        if(buf<=16384) return 16384; else
        if(buf<=20480) return 20480; else
        if(buf<=24576) return 24576; else
        if(buf<=28672) return 28672; else
        if(buf<=32768) return 32768; else
        if(buf<=40960) return 40960; else
        if(buf<=49152) return 49152; else
        if(buf<=57344) return 57344; else
        if(buf<=65536) return 65536; else
         return 65536;
*/
    return (16384);
}

#pragma code_seg("PAGE")
NTSTATUS CMiniportWaveInStream::Init(
              IN CMiniportWave *Miniport_,
          IN POOL_TYPE PoolType_,
              IN ULONG Pin_,
              IN PKSDATAFORMAT DataFormat_,
              OUT PDMACHANNEL *DmaChannel_,
              OUT PSERVICEGROUP *ServiceGroup_)

{
    PAGED_CODE();

    ServiceGroup=NULL;
    Miniport = NULL;
    DmaChannel=NULL;
    State=KSSTATE_STOP;
    magic=WAVEINSTREAM_MAGIC;
    current_freq=-1;
    isStereo=-1;
    rec_subdevice=0;

    Pin = Pin_;
    PoolType = PoolType_;

    Miniport = Miniport_;
    Miniport->AddRef();
    hw=Miniport->hw;

    NTSTATUS ntStatus=SetFormat(DataFormat_);

    if(!NT_SUCCESS(ntStatus))
    {
     goto fail;
    }

    ntStatus = PcNewServiceGroup(&ServiceGroup,NULL);
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

    if( NT_SUCCESS(ntStatus) )
    {
        if(current_freq<=0)
         current_freq=hw->card_frequency;

        // buffer_size=check_rec_option(current_freq*2*(isStereo?2:1)/50);
        buffer_size=hw->cb.rec_buffer;
        // 16bit, stereo, /100ms = 10ms, *2 - double buffering
        // FIXME

        /*
        // pre-3549

        ntStatus = Miniport->Port->NewMasterDmaChannel
          (&DmaChannel,NULL,NULL,buffer_size,TRUE,FALSE,(DMA_WIDTH)-1,(DMA_SPEED)-1);
        if(NT_SUCCESS(ntStatus))
        {
            // re-allocate buffer for >2Gb memory size

            DmaChannel->FreeBuffer();
            PHYSICAL_ADDRESS addr; addr.LowPart=0x7fffffff; addr.HighPart=0x0;
            ntStatus = DmaChannel->AllocateBuffer(buffer_size,&addr);

            if(NT_SUCCESS(ntStatus))
            {
                    debug(DWDM,"[CMiniportWaveInStream::Init]: allocated buffer @%p, phys: %x:%x\n",
                        DmaChannel->SystemAddress(),
                        DmaChannel->PhysicalAddress().HighPart,
                        DmaChannel->PhysicalAddress().LowPart);

                    DmaChannel->AddRef();
            }
            else
             debug(DWDM,"!!! [CMiniportWaveInStream::Init] :: failed to re-allocate buffer [%x]\n",ntStatus);

        } else debug(DWDM,"!!! [CMiniportWaveInStream::Init] :: failed to create NewMasterDmaChannel [%x]\n",ntStatus);
        */
        PHYSICAL_ADDRESS addr; addr.LowPart=0x7fffffff; addr.HighPart=0x0;
        DmaChannel = CreateCDmaChannel(buffer_size,addr);
        if(DmaChannel ==0)
         ntStatus=STATUS_INSUFFICIENT_RESOURCES;
        else
         DmaChannel->AddRef();
    }

    if(DmaChannel)
    {
     *DmaChannel_=DmaChannel;
     }
    else
    {
     *DmaChannel_=NULL;
     debug(DWDM,"!!! DmaChannel creation failed\n");
    }

    if(NT_SUCCESS(ntStatus))
    {
        if(!NT_SUCCESS(Open()))
        {
         debug(DWDM,"!!! [CMiniportWaveInStream::Init] : Failed to open audio\n");
         ntStatus=STATUS_UNSUCCESSFUL;
        }
        else
        {
            Silence(DmaChannel->SystemAddress(),buffer_size);
        }
    }

fail:
     
    if( !NT_SUCCESS(ntStatus) )
    {
        if(DmaChannel)
        {
            DmaChannel->Release();
            DmaChannel=NULL;    
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
int CMiniportWaveInStream::Parse()
{
 PAGED_CODE();
 PWAVEFORMATEXTENSIBLE WaveFormatEx;

  // Fill the structure
  if (IsEqualGUIDAligned(DataFormat2.DataFormat.Specifier,KSDATAFORMAT_SPECIFIER_DSOUND))
  {
      PKSDATAFORMAT_DSOUND    DSoundFormat;
      KSDSOUND_BUFFERDESC     *BufferDesc;

      DSoundFormat = (PKSDATAFORMAT_DSOUND) &DataFormat2;
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

   current_freq = WaveFormatEx->Format.nSamplesPerSec;

   isStereo = (WaveFormatEx->Format.nChannels==2)?1:0;

   if(WaveFormatEx->Format.wBitsPerSample!=16) // support only 16bit recording
   // FIXME 3551 there is support for 24/32 as well
   {
    debug(DWDM,"!!! recording: only 16 bit supported in hardware\n");
    return STATUS_INVALID_PARAMETER;
   }

   if(WaveFormatEx->Format.nChannels>2)
   {
    debug(DWDM,"!!! recording: %d channels not supported\n",WaveFormatEx->Format.nChannels);
    return STATUS_INVALID_PARAMETER;
   }

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
         debug(DWDM,"!!! wavein:Parse(): invalid subformat for WaveExtensible\n");
         return STATUS_INVALID_PARAMETER;
        }

        if(WaveFormatEx->Samples.wValidBitsPerSample!=16 && WaveFormatEx->Samples.wValidBitsPerSample!=0)
        {
         debug(DWDM,"!! wavein: valid bits per sample!=16 [%d]\n",WaveFormatEx->Samples.wValidBitsPerSample);
         return STATUS_INVALID_PARAMETER;
        }
    }
    else
    {
        debug(DWDM,"!! cbSize=0 for Extensible format\n");
        return STATUS_INVALID_PARAMETER;
    }
   }

   rec_subdevice=0;

   if(hw->spdif_recording)
     rec_subdevice=1;

   if(current_freq>48000 && rec_subdevice==0) // quick hack; pre p16v ;)
   {
    debug(DLIB,"wavein: parse: %d freq is supported in direct spdif rec. mode only\n",
     current_freq);
    return STATUS_INVALID_PARAMETER;
   }

   switch(WaveFormatEx->Format.wFormatTag)
   {
        case WAVE_FORMAT_UNKNOWN:
        case WAVE_FORMAT_PCM:
             break;

        case WAVE_FORMAT_DOLBY_AC3_SPDIF:
        case WAVE_FORMAT_RAW_SPORT:
        case WAVE_FORMAT_ESST_AC3:
            if(current_freq!=48000 || !isStereo)
            {
             debug(DWDM,"!!! ac-3 spdif rec - sample rate %d stere0: %d\n",current_freq,isStereo);
             return STATUS_INVALID_PARAMETER;
            }
            if(hw->card_frequency!=48000)
            {
             debug(DWDM,"!!! ac-3 spdif rec - sample rate %d, but card freq is %d\n",current_freq,isStereo,hw->card_frequency);
             return STATUS_INVALID_PARAMETER;
            }
            debug(DWDM,"kx rec: spdif recording on\n");
            rec_subdevice=1;
            break;
    default:
        debug(DWDM,"!!! Unknown wFormatTag %x\n",WaveFormatEx->Format.wFormatTag);
        break;
   }

   return STATUS_SUCCESS;
}

#pragma code_seg("PAGE")
STDMETHODIMP CMiniportWaveInStream::SetFormat(IN      PKSDATAFORMAT   DataFormat_)
{
    PAGED_CODE();

    ASSERT(DataFormat_);

    int oldRate=current_freq;
    int wasStereo=isStereo;


    if(DataFormat_->FormatSize>sizeof(DataFormat2))
    {
        debug(DWDM,"!!! Not enough memory for DataFormat (%d)\n",DataFormat_->FormatSize);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlCopyMemory(&DataFormat2,DataFormat_,DataFormat_->FormatSize);

    NTSTATUS ntStatus=Parse();

    if(NT_SUCCESS(ntStatus))
    {
        // changed
        if(is_rec) // change if already opened
        {
          debug(DWDM,"!!! Trying to change bitness / rate for REC\n");
          ntStatus=STATUS_INVALID_PARAMETER;
        }
    }
    return ntStatus;
}

#pragma code_seg("PAGE")
STDMETHODIMP_(ULONG) CMiniportWaveInStream::SetNotificationFreq(
    IN      ULONG   Interval,
    OUT     PULONG  FramingSize)
{
    PAGED_CODE();

    if(Interval!=DMA_DEFAULT_NOTIFY)
    {
     debug(DWDM,"!!! SetNotificationFreq !=10 ( %d )\n",Interval);
    }

    *FramingSize =
         (1 << (isStereo + 1 /* 16bit */)) *
         (current_freq * Interval / 1000);
 
    kx_rec_set_notify(hw,rec_subdevice,Interval);

    return Interval;
}

#pragma code_seg("PAGE")
STDMETHODIMP CMiniportWaveInStream::SetState(IN      KSSTATE     NewState)
{
    PAGED_CODE();

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
                 if(is_rec)
                  kx_rec_stop(hw,rec_subdevice);
                 }
                break;
            case KSSTATE_RUN:
                if(State!=KSSTATE_RUN)
                {
                 if(is_rec)
                  kx_rec_start(hw,rec_subdevice);
                }
                break;
            case KSSTATE_PAUSE:
                if(State!=KSSTATE_PAUSE)
                {
                 if(is_rec)
                  kx_rec_stop(hw,rec_subdevice);
                }
                break;
            default:
                debug(DWDM,"!!! [CMiniportWaveInStream::SetState] :: -- Unknown state %d\n",NewState);
                break;
    }
    State=NewState;

    return STATUS_SUCCESS;
}

#pragma code_seg()    
STDMETHODIMP_(void) CMiniportWaveInStream::Silence(IN      PVOID   Buffer,    IN      ULONG   ByteCount)
{
   RtlFillMemory(Buffer,ByteCount,0); 
}


#pragma code_seg()
#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
STDMETHODIMP CMiniportWaveInStream::GetPosition(OUT     PULONG  Position)
{
    ASSERT(Position);

    int current_position=0;

    kx_rec_get_position(hw,
        &current_position,rec_subdevice);

    (*Position)=current_position;

   return STATUS_SUCCESS;
}

#pragma code_seg()
STDMETHODIMP CMiniportWaveInStream::NormalizePhysicalPosition(IN OUT PLONGLONG PhysicalPosition)
{                           
    debug(DWDM,"!!! [Normalize]\n");
    *PhysicalPosition =
            (_100NS_UNITS_PER_SECOND / 
                (1 << (isStereo + 1 /* 16 bit */)) * (*PhysicalPosition)) / 
                    current_freq;
    return STATUS_SUCCESS;
}

#pragma code_seg("PAGE")
STDMETHODIMP CMiniportWaveInStream::NonDelegatingQueryInterface(IN      REFIID  Interface,
                                OUT     PVOID * Object)
{
    PAGED_CODE();

    ASSERT(Object);

    if (IsEqualGUIDAligned(Interface,IID_IUnknown))
    {
        *Object = PVOID(PUNKNOWN(PMINIPORTWAVECYCLICSTREAM(this)));
    }
    else 
    if (IsEqualGUIDAligned(Interface, IID_IDrmAudioStream))
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
    debug(DWDM,"!!! - (WaveStreamIn) try to delegate to ref_iid unknown\n");
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
CMiniportWaveInStream::~CMiniportWaveInStream()
{
    PAGED_CODE();

    if(magic!=WAVEINSTREAM_MAGIC)
    {
        debug(DWDM,"!!! [~CMiniportWaveInStream]: magic(%x) != %x\n",magic,WAVEINSTREAM_MAGIC);
        magic=0;
        return;
    }
    magic=0;

    if(is_rec)
    {
        kx_rec_close(hw,rec_subdevice);
        is_rec=0;
    }

    if(ServiceGroup)
    {
        ServiceGroup->Release();
        ServiceGroup=NULL;
    }

    if(DmaChannel)
    {
        DmaChannel->Release();
        DmaChannel=NULL;
    }

    if(Miniport)
    {
        Miniport->Release();
        Miniport=NULL;
    }

}
