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


// p16v / alice3 
// (NOTE: covered by Creative NDA, no distribution allowed)

#include "common.h"

#include "interface/guids.h"

#include "wdm/miniwave_p16v.h"
#include "wdm/tbl_wave_p16v.h"

#pragma code_seg("PAGE")
NTSTATUS create_waveHQ
(
    OUT     PUNKNOWN *  Unknown,
    IN      REFCLSID,
    IN      PUNKNOWN    UnknownOuter    OPTIONAL,
    IN      POOL_TYPE   PoolType
)
{
    PAGED_CODE();

    ASSERT(Unknown);

    STD_CREATE_BODY_(CMiniportWaveHQ,Unknown,UnknownOuter,PoolType,PMINIPORTWAVECYCLIC);
}

#pragma code_seg("PAGE")
STDMETHODIMP CMiniportWaveHQ::NonDelegatingQueryInterface(IN      REFIID  Interface,
								 OUT     PVOID * Object)
{
    PAGED_CODE();

    ASSERT(Object);

    if (IsEqualGUIDAligned(Interface,IID_IUnknown))
    {
        *Object = PVOID(PUNKNOWN(PMINIPORTWAVECYCLIC(this)));
    }
    else
    if (IsEqualGUIDAligned(Interface,IID_IMiniport))
    {
        *Object = PVOID(PMINIPORT(this));
    }
    else
    if (IsEqualGUIDAligned(Interface,IID_IMiniportWaveCyclic))
    {
        *Object = PVOID(PMINIPORTWAVECYCLIC(this));
    }
    else
    if (IsEqualGUIDAligned(Interface,IID_IPowerNotify))
    {
        *Object = PVOID(PPOWERNOTIFY(this));
    }
    else
    if (IsEqualGUIDAligned(Interface,IID_IPinCount))
    {
    	*Object=PVOID(PPINCOUNT(this));
    }
    else
    if (IsEqualGUIDAligned(Interface,IID_IMiniportDMus))
    {
    	*Object=NULL; // FIXME
    }
    else
    {
    	debug(DWDM,"!!! - (WaveHQ) try to delegate to ref_iid unknown\n");
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

    if (*Object)
    {
        PUNKNOWN(*Object)->AddRef();
        return STATUS_SUCCESS;
    }

    return STATUS_INVALID_PARAMETER;
}

#pragma code_seg("PAGE")
CMiniportWaveHQ::~CMiniportWaveHQ(void)
{
    PAGED_CODE();

    debug(DWDM,"[CMiniportWaveHQ::~CMiniportWaveHQ]\n");
    if(magic!=WAVEHQ_MAGIC)
    {
    	debug(DWDM," !!! [~CMiniportWaveHQ]: magic (%x) != %x\n",magic,WAVEHQ_MAGIC);
    	magic=0;
    	return;
    }
    magic=0;

    if (Port)
    {
        Port->Release();
        Port = NULL;
    }
    if (AdapterCommon)
    {
    	 ((CAdapterCommon *)AdapterCommon)->WaveHQ=0;
         AdapterCommon->Release();
         AdapterCommon = NULL;
    }
}

#pragma code_seg("PAGE")
STDMETHODIMP CMiniportWaveHQ::Init(IN      PUNKNOWN        UnknownAdapter,
				    IN      PRESOURCELIST   ResourceList,
                                    IN      PPORTWAVECYCLIC Port_)
{
    PAGED_CODE();

    ASSERT(UnknownAdapter);
    ASSERT(ResourceList);
    ASSERT(Port_);

    debug(DWDM,"[CMiniportWaveHQ::init]\n");

    magic=WAVEHQ_MAGIC;

    PowerState=PowerDeviceD0;

    AdapterCommon=NULL;
    Port=NULL;

    pb_stream=0;

    //
    // AddRef() is required because we are keeping this pointer.
    //
    Port = Port_;
    Port->AddRef();

    //
    // We want the IAdapterCommon interface on the adapter common object,
    // which is given to us as a IUnknown.  The QueryInterface call gives us
    // an AddRefed pointer to the interface we want.
    //
    NTSTATUS ntStatus =
        UnknownAdapter->QueryInterface
        (
            IID_IAdapterCommon,
            (PVOID *) &AdapterCommon
        );

    if( !NT_SUCCESS(ntStatus) )
    {
        // clean up AdapterCommon
        if( AdapterCommon )
        {
            AdapterCommon->Release();
            AdapterCommon = NULL;
        }

        // release the port
        if(Port)
        {
         Port->Release();
         Port = NULL;
        }
    } else 
    {
     ((CAdapterCommon*)AdapterCommon)->WaveHQ=this;
     hw=((CAdapterCommon *)AdapterCommon)->hw;
    }

    if(!NT_SUCCESS(ntStatus))
     debug(DWDM,"!!! WaveHQ Init failed\n");

    return ntStatus;
}

#pragma code_seg("PAGE")
STDMETHODIMP CMiniportWaveHQ::GetDescription(OUT     PPCFILTER_DESCRIPTOR *  OutFilterDescriptor)
{
    PAGED_CODE();

    ASSERT(OutFilterDescriptor);

    // limit format support to 24(32)/96, 8(2) channels
    if(((CAdapterCommon *)AdapterCommon)->is_vista)
    {
      MiniportPins[0].KsPinDescriptor.DataRangesCount=2;
      MiniportPins[2].KsPinDescriptor.DataRangesCount=2;

      // re-enable playback in Windows 7 (it does not support 'true' 32-bit data at all)
      PinDataRangesStreamRecording[0].MinimumBitsPerSample=24;
      PinDataRangesStreamPlayback[0].MinimumBitsPerSample=24;
      PinDataRangesStreamRecording[1].MinimumBitsPerSample=24;
      PinDataRangesStreamPlayback[1].MinimumBitsPerSample=24;
    }

    *OutFilterDescriptor = &MiniportFilterDescriptor;

    return STATUS_SUCCESS;
}

#pragma code_seg("PAGE")
STDMETHODIMP CMiniportWaveHQ::DataRangeIntersection
(
    IN      ULONG           PinId,
    IN      PKSDATARANGE    ClientDataRange,
    IN      PKSDATARANGE    MyDataRange,
    IN      ULONG           OutputBufferLength,
    OUT     PVOID           ResultantFormat,
    OUT     PULONG          ResultantFormatLength
)
{
   PAGED_CODE();

    if (!IsEqualGUIDAligned(ClientDataRange->MajorFormat,
           KSDATAFORMAT_TYPE_AUDIO) && 
        !IsEqualGUIDAligned(ClientDataRange->MajorFormat, 
           KSDATAFORMAT_TYPE_WILDCARD))
    {
    	debug(DWDM,"!!! -- DataIntersection:: it was not AUDIO and not *\n");
        return STATUS_NO_MATCH;
    }

    // Major format is: audio or *

    if(!IsEqualGUIDAligned(ClientDataRange->SubFormat,
        KSDATAFORMAT_SUBTYPE_PCM))
    {
    	debug(DWDM,"!!! -- DataIntersectionHQ (not PCM, not *) unknown subformat:"
    	"%x.%x.%x-%x-%x-%x-%x-%x-%x-%x-%x\n",
    	  ClientDataRange->SubFormat.Data1,
          ClientDataRange->SubFormat.Data2,
          ClientDataRange->SubFormat.Data3,
          ClientDataRange->SubFormat.Data4[0],
          ClientDataRange->SubFormat.Data4[1],
          ClientDataRange->SubFormat.Data4[2],
          ClientDataRange->SubFormat.Data4[3],
          ClientDataRange->SubFormat.Data4[4],
          ClientDataRange->SubFormat.Data4[5],
          ClientDataRange->SubFormat.Data4[6],
          ClientDataRange->SubFormat.Data4[7]);

          return STATUS_NO_MATCH;
    }

            	   int sz=0;

            	   // should be PCM
    		   if(IsEqualGUIDAligned(ClientDataRange->Specifier,
    	    		KSDATAFORMAT_SPECIFIER_WAVEFORMATEX))
    	    	   {
    	    	    sz=sizeof(WAVEFORMATEXTENSIBLE);
    	    	   }
    	    	   else
   		    if(IsEqualGUIDAligned(ClientDataRange->Specifier,
    	    		KSDATAFORMAT_SPECIFIER_DSOUND))

    	    	    {
    	    	     sz=sizeof(WAVEFORMATEXTENSIBLE)+2*sizeof(ULONG);
    	    	     debug(DWDM,"!!! btw, DataRangeIntersection: specifier is DSound!\n");
    	    	    }
    	    	    else
    	    	    {
    	    	     debug(DWDM,"!!! DataRangeIntersection: unknown specifier!\n");
    	    	     return STATUS_NO_MATCH;
    	    	    }


                   if (!OutputBufferLength) 
                   {
                       *ResultantFormatLength = sizeof(KSDATAFORMAT) + sz;
                       return STATUS_BUFFER_OVERFLOW;
                   } 
                   
                   if (OutputBufferLength < (sizeof(KSDATAFORMAT) + sz)) 
                   {
                       debug(DWDM,"!! buffer was < than needed\n");
                       return STATUS_BUFFER_TOO_SMALL;
                   }

                   // Fill in the structure the datarange structure.
                   //
                   RtlCopyMemory(ResultantFormat, MyDataRange, sizeof(KSDATAFORMAT));

                   // Modify the size of the data format structure to fit the WAVEFORMATEXTENSIBLE
                   // structure.
                   //
                   ((PKSDATAFORMAT)ResultantFormat)->FormatSize =
                       sizeof(KSDATAFORMAT) + sz;

                   // Append the WAVEFORMATEXTENSIBLE structure
                   //
                   PWAVEFORMATEXTENSIBLE pWfxExt = 
                       (PWAVEFORMATEXTENSIBLE)((PKSDATAFORMAT)ResultantFormat + 1);

   		    if(IsEqualGUIDAligned(ClientDataRange->Specifier,
    	    		KSDATAFORMAT_SPECIFIER_DSOUND))
    	    	    {	
			((PKSDATAFORMAT_DSOUND)ResultantFormat)->BufferDesc.Flags = 0 ;
			((PKSDATAFORMAT_DSOUND)ResultantFormat)->BufferDesc.Control = 0 ;

			((PKSDATAFORMAT_DSOUND)ResultantFormat)->DataFormat.Specifier = 
          			KSDATAFORMAT_SPECIFIER_DSOUND;

			pWfxExt=(PWAVEFORMATEXTENSIBLE) &((PKSDATAFORMAT_DSOUND)ResultantFormat)->BufferDesc.WaveFormatEx;
    	    	    }	

#define my_min(a,b) ((a)<(b)?(a):(b))

		   debug(DNONE,"HQ Intersection: client: %dch/%dHz/%dBps vs my: %dch/%dHz/%dBps\n",
		     (DWORD)((PKSDATARANGE_AUDIO) ClientDataRange)->MaximumChannels,
		     (DWORD)((PKSDATARANGE_AUDIO) ClientDataRange)->MaximumSampleFrequency,
		     (DWORD)((PKSDATARANGE_AUDIO) ClientDataRange)->MaximumBitsPerSample,

		     (DWORD)((PKSDATARANGE_AUDIO) MyDataRange)->MaximumChannels,
		     (DWORD)((PKSDATARANGE_AUDIO) MyDataRange)->MaximumSampleFrequency,
		     (DWORD)((PKSDATARANGE_AUDIO) MyDataRange)->MaximumBitsPerSample);

                   pWfxExt->Format.nChannels = (WORD)((PKSDATARANGE_AUDIO) MyDataRange)->MaximumChannels;

                   pWfxExt->Format.nSamplesPerSec = ((PKSDATARANGE_AUDIO) MyDataRange)->MaximumSampleFrequency;

                   pWfxExt->Format.wBitsPerSample = 32;
                   pWfxExt->Samples.wValidBitsPerSample = 24; // (WORD)((PKSDATARANGE_AUDIO) MyDataRange)->MaximumBitsPerSample;

                   pWfxExt->Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;

                   pWfxExt->Format.nBlockAlign = 
                       (pWfxExt->Format.wBitsPerSample * pWfxExt->Format.nChannels) / 8;
                   pWfxExt->Format.nAvgBytesPerSec = 
                       pWfxExt->Format.nSamplesPerSec * pWfxExt->Format.nBlockAlign;
                   pWfxExt->Format.cbSize = 22;

                   pWfxExt->SubFormat = KSDATAFORMAT_SUBTYPE_PCM;

                   // This should be set to wave port's channel config
                    switch(pWfxExt->Format.nChannels)
                    {
                     default:
                     case 5+1:
                        pWfxExt->dwChannelMask = KSAUDIO_SPEAKER_5POINT1;
                        break;
                     case 6+1:
                        pWfxExt->dwChannelMask = KSAUDIO_SPEAKER_5POINT1 | SPEAKER_BACK_CENTER;
                        break;
                     case 7+1:
                     	pWfxExt->dwChannelMask = KSAUDIO_SPEAKER_7POINT1;
                        // SPEAKER_FRONT_LEFT_OF_CENTER | SPEAKER_FRONT_RIGHT_OF_CENTER
                     	break;
                     case 2:
                        pWfxExt->dwChannelMask = KSAUDIO_SPEAKER_STEREO;
                        break;
                     case 1:
                        pWfxExt->dwChannelMask = KSAUDIO_SPEAKER_MONO;
                        break;
                     case 4:
                        pWfxExt->dwChannelMask = KSAUDIO_SPEAKER_QUAD;
                        break;
                     case 3:
                        pWfxExt->dwChannelMask = SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_FRONT_CENTER;
                        break;
                     case 5:
                        pWfxExt->dwChannelMask = KSAUDIO_SPEAKER_SURROUND|SPEAKER_LOW_FREQUENCY;
                        break;
                    }

                   // Now overwrite also the sample size in the ksdataformat structure.
                   // per XP ddk: this field is ignored
                   ((PKSDATAFORMAT)ResultantFormat)->SampleSize = pWfxExt->Format.nBlockAlign;
                   
                   // That we will return.
                   //
                   *ResultantFormatLength = sizeof(KSDATAFORMAT) + sizeof(WAVEFORMATEXTENSIBLE);

                   return STATUS_SUCCESS;
}

#pragma code_seg("PAGE")
STDMETHODIMP CMiniportWaveHQ::NewStream(OUT     PMINIPORTWAVECYCLICSTREAM * OutStream_,
					   IN      PUNKNOWN                    OuterUnknown_,
                                           IN      POOL_TYPE                   PoolType_,
                                           IN      ULONG                       Pin_,
                                           IN      BOOLEAN                     Capture_,
                                           IN      PKSDATAFORMAT               DataFormat_,
                                           OUT     PDMACHANNEL *               DmaChannel_,
                                           OUT     PSERVICEGROUP *             ServiceGroup_)
{
    PAGED_CODE();

    ASSERT(OutStream_);
    ASSERT(DataFormat_);
    ASSERT(DmaChannel_);
    ASSERT(ServiceGroup_);

    NTSTATUS ntStatus = STATUS_SUCCESS;

    CMiniportWaveStreamHQ *stream;

    if(Capture_)
    {
         if(hw->p16v_rec_opened)
         {
         	debug(DERR,"p16v rec already opened\n");
         	return STATUS_INSUFFICIENT_RESOURCES;
         }

    	 // wavein
         stream = new (PoolType_, 'QHXk')
                             CMiniportWaveStreamHQ(OuterUnknown_);
    }
    else
    {
        if(hw->p16v_pb_opened)
        {
        	debug(DERR,"p16v playback already opened\n");
        	return STATUS_INSUFFICIENT_RESOURCES;
        }

    	// waveout
    	stream = new (PoolType_, 'QHXk')
    			     CMiniportWaveStreamHQ(OuterUnknown_);
    }
    if(stream)
    {
        stream->AddRef();
        ntStatus = stream->Init(this,PoolType_,Pin_,DataFormat_,DmaChannel_,ServiceGroup_,Capture_);
    }
    else
    {
      ntStatus=STATUS_INSUFFICIENT_RESOURCES;
    }

    if(NT_SUCCESS(ntStatus))
    {
    	*OutStream_ = PMINIPORTWAVECYCLICSTREAM(stream);
    	(*OutStream_)->AddRef();
    	stream->Release();
    }
     else 
    { 
        if(stream)
        {
          stream->Release();
          stream=NULL;
        }
        *OutStream_=NULL; 
        *DmaChannel_=NULL;
        *ServiceGroup_=NULL;
        return ntStatus; 
    }
    return STATUS_SUCCESS;
}

#pragma code_seg("PAGE")
NTSTATUS CMiniportWaveHQ::PropertyCpuResources(IN PPCPROPERTY_REQUEST PropertyRequest)
{
 PAGED_CODE();

    ASSERT(PropertyRequest);

    NTSTATUS ntStatus = STATUS_INVALID_DEVICE_REQUEST;

        if(PropertyRequest->Verb & KSPROPERTY_TYPE_GET)
        {
            if(PropertyRequest->ValueSize >= sizeof(LONG))
            {
                *(PLONG(PropertyRequest->Value)) = KSAUDIO_CPU_RESOURCES_NOT_HOST_CPU;
                PropertyRequest->ValueSize = sizeof(LONG);
                ntStatus = STATUS_SUCCESS;
                debug(DPROP,"WaveHQ:cpu: get (node=%d)\n",PropertyRequest->Node);
            } else
            {
            	debug(DWDM,"!!! -- waveHQ::cpuresources: buffer too small\n");
                ntStatus = STATUS_BUFFER_TOO_SMALL;
            }
        } else if(PropertyRequest->Verb & KSPROPERTY_TYPE_BASICSUPPORT)
        {
            if(PropertyRequest->ValueSize >= (sizeof(KSPROPERTY_DESCRIPTION)))
            {
                // if return buffer can hold a KSPROPERTY_DESCRIPTION, return it
                PKSPROPERTY_DESCRIPTION PropDesc = PKSPROPERTY_DESCRIPTION(PropertyRequest->Value);

                PropDesc->AccessFlags       = KSPROPERTY_TYPE_BASICSUPPORT |
                                              KSPROPERTY_TYPE_GET;
                PropDesc->DescriptionSize   = sizeof(KSPROPERTY_DESCRIPTION);
                PropDesc->PropTypeSet.Set   = KSPROPTYPESETID_General;
                PropDesc->PropTypeSet.Id    = VT_I4;
                PropDesc->PropTypeSet.Flags = 0;
                PropDesc->MembersListCount  = 0;
                PropDesc->Reserved          = 0;

                // set the return value size
                PropertyRequest->ValueSize = sizeof(KSPROPERTY_DESCRIPTION);
                ntStatus = STATUS_SUCCESS;
                debug(DPROP,"WaveHQ:cpu: basic support 1\n");
            } else if(PropertyRequest->ValueSize >= sizeof(ULONG))
            {
                // if return buffer can hold a ULONG, return the access flags
                PULONG AccessFlags = PULONG(PropertyRequest->Value);
        
                *AccessFlags = KSPROPERTY_TYPE_BASICSUPPORT |
                               KSPROPERTY_TYPE_GET |
                               KSPROPERTY_TYPE_SET;
        
                // set the return value size
                PropertyRequest->ValueSize = sizeof(ULONG);
                ntStatus = STATUS_SUCCESS;                    
                debug(DPROP,"WaveHQ:cpu: basic support 2\n");
            }
        } else debug(DWDM,"!!! Unknown verb in wave::CpuResources\n");

    if(!NT_SUCCESS(ntStatus))
     debug(DWDM,"!!! Error CpuResources Property op\n");

    return ntStatus;
}

#pragma code_seg("PAGE")
NTSTATUS CMiniportWaveHQ::PropertyVolume(IN PPCPROPERTY_REQUEST PropertyRequest)
{
    PAGED_CODE();

    ASSERT(PropertyRequest);

    CMiniportWaveStreamHQ *that =
        (CMiniportWaveStreamHQ *)(PMINIPORTWAVECYCLIC)PropertyRequest->MinorTarget;

    if(that)
    {
     if(that->magic==WAVEHQSTREAM_MAGIC)
     {
      return CMiniportWaveStreamHQ::PropertyVolume(PropertyRequest);
     }
     else
     {
      debug(DWDM,"!!! MiniportHQ::propertyVolume: magic!=waveoutstream_magic\n");
     }
    }
    else
    {
     return CMiniportWaveStreamHQ::PropertyVolume(PropertyRequest);;
    }
    return STATUS_INVALID_PARAMETER;
}

#pragma code_seg("PAGE")
NTSTATUS CMiniportWaveHQ::PropertySamplingRate(IN PPCPROPERTY_REQUEST PropertyRequest)
{
 PAGED_CODE();
 ASSERT(PropertyRequest);
 return CMiniportWaveStreamHQ::PropertySamplingRate(PropertyRequest);
}

#pragma code_seg()
STDMETHODIMP_(void) CMiniportWaveHQ::PowerChangeNotify(IN      POWER_STATE     NewState)
{
    debug(DWDM,"[CMiniportWaveHQ::PowerChangeNotify] - do nothing\n");
    PowerState = NewState.DeviceState;
}

// --------------------------------------------------------------------------------
#pragma code_seg()
void CMiniportWaveStreamHQ::my_isr(void)
{
 Miniport->Port->Notify(ServiceGroup);
}

#pragma code_seg("PAGE")
NTSTATUS CMiniportWaveStreamHQ::Init(
    		  IN CMiniportWaveHQ *Miniport_,
		  IN POOL_TYPE PoolType_,
    		  IN ULONG Pin_,
    		  IN PKSDATAFORMAT DataFormat_,
    		  OUT PDMACHANNEL *DmaChannel_,
    		  OUT PSERVICEGROUP *ServiceGroup_,
    		  IN int Capture_)
{
    PAGED_CODE();

    debug(DSTATE,"hq: new stream --  Init()\n");

    ServiceGroup=NULL;
    Miniport = NULL;
    State=KSSTATE_STOP;
    magic=WAVEHQSTREAM_MAGIC;
    source_channel=NULL;
    capture=Capture_;

    nchannels=2;
    cur_freq=96000;
    bps=32;

    memset(vol1,0,sizeof(vol1));
    memset(vol2,0,sizeof(vol2));

    opened=0;

    Pin = Pin_;
    PoolType = PoolType_;

    Miniport = Miniport_;
    Miniport->AddRef();

    hw=Miniport->hw;

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

    source_buffer_size=61440; // ((cur_freq*bps*nchannels)/8)/10; // 10ms buffer
    				// creative uses: 65472

    if(NT_SUCCESS(ntStatus))
    {
        // pre-3549:
    	/* ntStatus = Miniport->Port->NewMasterDmaChannel
    	      (&source_channel,NULL,NULL,source_buffer_size,TRUE,FALSE,(DMA_WIDTH)-1,(DMA_SPEED)-1);
        if(NT_SUCCESS(ntStatus))
        {
            // re-allocate buffer for >2Gb memory size

            source_channel->FreeBuffer();

            PHYSICAL_ADDRESS addr; addr.LowPart=0x7fffffff; addr.HighPart=0x0;
            ntStatus = source_channel->AllocateBuffer(source_buffer_size,&addr);

            if(NT_SUCCESS(ntStatus))
            {
                    debug(DWDM,"[CMiniportWaveStreamHQ::Init]: allocated buffer @%p, phys: %x:%x\n",
                        source_channel->SystemAddress(),
                        source_channel->PhysicalAddress().HighPart,
                        source_channel->PhysicalAddress().LowPart);
                    source_channel->AddRef();
            }
            else
             debug(DWDM,"!!! [CMiniportWaveStreamHQ::Init] :: failed to re-allocate buffer [%x]\n",ntStatus);

        } else debug(DWDM,"!!! [CMiniportWaveStreamHQ::Init] :: failed to create NewMasterDmaChannel [%x]\n",ntStatus);
        */

        // 3549 and later: use our own CDmaChannel class

        PHYSICAL_ADDRESS addr; addr.LowPart=0x7fffffff; addr.HighPart=0x0;
        source_channel = CreateCDmaChannel(source_buffer_size,addr);
        if(source_channel==0)
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
         	 debug(DWDM,"!!! [CMiniportWaveOutStreamHQ::Init] : Failed to open audio\n");
         	 ntStatus=STATUS_UNSUCCESSFUL;
         	}
         	else
         	{
         	    Silence(source_channel->SystemAddress(),source_buffer_size);
         	    if(!capture)
         	     Miniport->pb_stream=this;
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
STDMETHODIMP_(void) CMiniportWaveHQ::PinCount
(
    IN      ULONG   PinId,
    IN  OUT PULONG  FilterNecessary,
    IN  OUT PULONG  FilterCurrent,
    IN  OUT PULONG  FilterPossible,
    IN  OUT PULONG  GlobalCurrent,
    IN  OUT PULONG  GlobalPossible
)
{
    PAGED_CODE();

    if(FilterPossible) *FilterPossible=1;
    if(GlobalPossible) *GlobalPossible=1;
}

#pragma code_seg("PAGE")
STDMETHODIMP_(NTSTATUS) CMiniportWaveStreamHQ::SetContentId(IN ULONG ContentId,IN PCDRMRIGHTS DrmRights)
{
 PAGED_CODE();
 return STATUS_SUCCESS;
}

#pragma code_seg("PAGE")
CMiniportWaveStreamHQ::~CMiniportWaveStreamHQ(void)
{
    PAGED_CODE();

    if(opened && hw)
    {
     opened=0;
     if(capture) kx_p16v_rec_close(hw); else kx_p16v_pb_close(hw);

     if(!capture)
      Miniport->pb_stream=0;
    }

    if(magic!=WAVEHQSTREAM_MAGIC)
    {
    	debug(DWDM,"!!! [~CMiniportWaveStreamHQ]: magic(%x) != %x\n",magic,WAVEHQSTREAM_MAGIC);
    	magic=0;
    	return;
    }
    magic=0;

    if(ServiceGroup)
    {
        ServiceGroup->Release();
        ServiceGroup=NULL;
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

#pragma code_seg("PAGE")
int CMiniportWaveStreamHQ::Parse()
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
      
      debug(DWDM,"HQ -- [prased] using KSDATAFORMAT_DSOUND format\n");
      
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
  	debug(DWDM,"!!! HQ -- [parsed] UNKNOWN FORMAT SPECIFIER (not WaveFormatEx)\n");
  	return STATUS_INVALID_PARAMETER;
  }

  switch(WaveFormatEx->Format.nSamplesPerSec)
  {
//   case 44100: // mode not supported (due to p16v SRC issue?) FIXME
   case 48000:
   case 96000:
   case 192000:
    // ok
    break;
   default:
     debug(DWDM,"!!! HQ -- but format freq is not OK [%d/%d(%d)/%d]\n",
        WaveFormatEx->Format.nChannels,
        WaveFormatEx->Format.wBitsPerSample,
        WaveFormatEx->Samples.wValidBitsPerSample,
        WaveFormatEx->Format.nSamplesPerSec);
     return STATUS_UNSUCCESSFUL;
  }

  cur_freq=WaveFormatEx->Format.nSamplesPerSec;

  if(WaveFormatEx->Format.nChannels!=2 && WaveFormatEx->Format.nChannels!=8)
  {
   debug(DWDM,"waveoutHQ: bad channel number [%d] - stereo/8chn only! [%d/%d; %d; %d]\n",WaveFormatEx->Format.nChannels,
     WaveFormatEx->Format.wBitsPerSample,WaveFormatEx->Samples.wValidBitsPerSample,
     cur_freq,
     WaveFormatEx->Format.nChannels);
   return STATUS_INVALID_PARAMETER;
  }

  nchannels=WaveFormatEx->Format.nChannels;

  if(WaveFormatEx->Format.wBitsPerSample!=32)
  {
        debug(DWDM,"!! HQ: non-32 bit per sample not supported [%d/%d] - fr:%d ch:%d\n",WaveFormatEx->Format.wBitsPerSample,WaveFormatEx->Samples.wValidBitsPerSample,cur_freq,WaveFormatEx->Format.nChannels);
        return STATUS_INVALID_PARAMETER;
  }
  if(WaveFormatEx->Samples.wValidBitsPerSample!=24)
  {
    if(WaveFormatEx->Samples.wValidBitsPerSample!=32)
    {
        // last check: if it is not WaveExtensible, wValidBits can be 0
        if(WaveFormatEx->Samples.wValidBitsPerSample!=0)
        {
         debug(DWDM,"!! HQ: non-24 bit per sample not supported [%d/%d] - fr:%d ch:%d\n",WaveFormatEx->Format.wBitsPerSample,WaveFormatEx->Samples.wValidBitsPerSample,cur_freq,WaveFormatEx->Format.nChannels);
         return STATUS_INVALID_PARAMETER;
        }
         else debug(DWDM,"!! HQ: note: using 32bps, ValidBitsPerSample set to 0\n");
    }
    else
     debug(DWDM,"!! HQ: note: using 32/32, not 24/32...\n");
  }

  bps=WaveFormatEx->Format.wBitsPerSample;

  debug(DWDM,"HQ: format parsed: %d/%d/%d [%s]\n",nchannels,bps,cur_freq,capture?"rec":"pb");

   return STATUS_SUCCESS;
}

#pragma code_seg("PAGE")
STDMETHODIMP CMiniportWaveStreamHQ::SetFormat(IN PKSDATAFORMAT DataFormat_)
{
    PAGED_CODE();

    ASSERT(DataFormat_);

    if(DataFormat_->FormatSize>sizeof(DataFormat2))
    {
    	debug(DWDM,"!!! HQ: Not enough memory for DataFormat (%d)\n",DataFormat_->FormatSize);
    	return STATUS_INSUFFICIENT_RESOURCES;
    }

    memset(&DataFormat2,0,sizeof(DataFormat2));
    if(DataFormat_->FormatSize==sizeof(KSDATAFORMAT))
    {
     debug(DWDM,"!!! note: setFormatHQ: format size invalid\n");
    }
    RtlCopyMemory(&DataFormat2,DataFormat_,DataFormat_->FormatSize);

    NTSTATUS ret=Parse();
    if(NT_SUCCESS(ret))
    {
     // dynamic format change if necessary
     if(opened)
     {
      if(!capture)
      {
       debug(DWDM,"dynamic p16v freq changed [%d]\n",cur_freq);
       kx_p16v_pb_set_freq(hw,cur_freq);
      }
      else
      {
       debug(DWDM,"cannot change rec. format on the fly! ERROR!\n");
       return STATUS_UNSUCCESSFUL;
      }
     }
    }

    return ret;
}

#pragma code_seg("PAGE")
STDMETHODIMP_(ULONG) CMiniportWaveStreamHQ::SetNotificationFreq(
    IN      ULONG   Interval,
    OUT     PULONG  FramingSize)
{
    PAGED_CODE();

    if(Interval!=DMA_DEFAULT_NOTIFY)
    {
     debug(DWDM,"!!! SetNotificationFreq !=10 ( %d )\n",Interval);
    }

    *FramingSize =
         bps * nchannels * 
         (cur_freq * Interval / 1000) / 8;

    if(opened)
    {     
      if(capture)
       kx_p16v_rec_set_notify(hw,Interval);
      else
       kx_p16v_pb_set_notify(hw,Interval);
    }

    return Interval;
}

#pragma code_seg("PAGE")
STDMETHODIMP CMiniportWaveStreamHQ::SetState(IN KSSTATE NewState)
{
    PAGED_CODE();

    int i=0;

    if (NewState == KSSTATE_ACQUIRE)
    {
        State = KSSTATE_PAUSE;
    	return STATUS_SUCCESS;
    }

    debug(DSTATE,"HQ New State [cur=%d] (%d)\n",State,NewState);

    switch(NewState)
    {
        	case KSSTATE_STOP:
        		if(State==KSSTATE_RUN)
        		{
        		   if(capture) kx_p16v_rec_stop(hw); else kx_p16v_pb_stop(hw);
                           // should we 'rewind' the hardware position to zero?..
                           //   kx_wave_set_position(hw,channels[i],0);
                        }
        		break;
        	case KSSTATE_RUN:
        		if(State!=KSSTATE_RUN)
        		{
        		   if(capture) kx_p16v_rec_start(hw); else kx_p16v_pb_start(hw);
        		}
        		break;
        	case KSSTATE_PAUSE:
        		if(State!=KSSTATE_PAUSE)
        		{
        		   if(capture) kx_p16v_rec_stop(hw); else kx_p16v_pb_stop(hw);
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
STDMETHODIMP_(void) CMiniportWaveStreamHQ::Silence(IN      PVOID   Buffer,    IN      ULONG   ByteCount)
{
  RtlFillMemory(Buffer,ByteCount, 0);
}


#pragma code_seg()
STDMETHODIMP CMiniportWaveStreamHQ::GetPosition(OUT     PULONG  Position)
{
    ASSERT(Position);

    dword current_position;

    if(capture)
     kx_p16v_rec_get_pos(hw,current_position);
    else
     kx_p16v_pb_get_pos(hw,current_position);

    (*Position)=current_position;

   return STATUS_SUCCESS;
}

#pragma code_seg()
STDMETHODIMP CMiniportWaveStreamHQ::NormalizePhysicalPosition(IN OUT PLONGLONG PhysicalPosition)
{                           
    debug(DWDM,"!!! [NormalizeHQ]\n");
    *PhysicalPosition =
            (_100NS_UNITS_PER_SECOND / 
                bps * nchannels * (*PhysicalPosition) / 8) /
                    cur_freq;
    return STATUS_SUCCESS;
}

#pragma code_seg("PAGE")
STDMETHODIMP CMiniportWaveStreamHQ::NonDelegatingQueryInterface(IN      REFIID  Interface,
							    OUT     PVOID * Object)
{
    PAGED_CODE();

    ASSERT(Object);

    if (IsEqualGUIDAligned(Interface,IID_IUnknown))
    {
        *Object = PVOID(PUNKNOWN(PMINIPORTWAVECYCLICSTREAM(this)));
    }
/*    else if (IsEqualGUIDAligned(Interface, IID_IDrmAudioStream))
    {
        *Object = (PVOID)(PDRMAUDIOSTREAM)this;
    }
*/
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
NTSTATUS CMiniportWaveStreamHQ::PropertyVolume(IN PPCPROPERTY_REQUEST PropertyRequest)
{
    PAGED_CODE();

    CMiniportWaveHQ *that =
        (CMiniportWaveHQ *)(PMINIPORTWAVECYCLIC)PropertyRequest->MajorTarget;

    if(!that)
    {
     debug(DWDM,"!!! WaveHQ majortarget=0! :: vol\n");
     return STATUS_INVALID_PARAMETER;
    }

    if(that->magic!=WAVEHQ_MAGIC)
    {
     debug(DWDM,"!!! Bad wavehq magic!\n");
     return STATUS_INVALID_PARAMETER;
    }

    PUNKNOWN pu=PropertyRequest->MinorTarget;
    CMiniportWaveStreamHQ *that2=(CMiniportWaveStreamHQ *)PMINIPORTWAVECYCLICSTREAM(pu);

    if(that2!=NULL)
    {
     if(that2->magic!=WAVEHQSTREAM_MAGIC)
     {
      debug(DWDM,"!!! Bad wavestream_hq magic x2\n");
      return STATUS_INVALID_PARAMETER;
     }
    }
    else
    {
     PropertyRequest->Verb&=KSPROPERTY_TYPE_BASICSUPPORT;
     if(!PropertyRequest->Verb)
     {
      // debug(DWDM,"!!! No minorTarget in wave::vol!\n");
      // FIXME 3551: Windows 7 uses this property
      return STATUS_INVALID_PARAMETER;
     }
    }

    NTSTATUS        ntStatus = STATUS_INVALID_PARAMETER;
    int i=0;

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

                if(channel>=8|| channel<0)
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
                                  	break;
                                  case WAVE_VOLUME2:
                                      	*Level=that2->vol2[channel];
                                  	break;
                                  default:
                                    debug(DWDM,"!!! WaveHQ::Level get unknown %d\n",PropertyRequest->Node);
                                    break;
                                }
                                PropertyRequest->ValueSize = sizeof(LONG);
                                ntStatus = STATUS_SUCCESS;
                    }
                } else debug(DWDM,"!!! NB getHQ more than one channel!\n");
            } else debug(DWDM,"!!! Buffer too small in waveHQ::vollevel\n");

        } else if(PropertyRequest->Verb & KSPROPERTY_TYPE_SET)
        {
            LONG channel_s=-1,channel_e=-1;

            // get the instance channel parameter
            if(PropertyRequest->InstanceSize >= sizeof(LONG))
            {
                channel_s = *(PLONG(PropertyRequest->Instance));

                if(channel_s==-1)
                 { channel_s=0; channel_e=8-1; }
                else
                 { channel_e=channel_s; }

                if(channel_s>=8 || channel_s<0 ||
                   channel_e>=8 || channel_e<0)
                 { debug(DWDM,"!!! WaveLevelHQ:: Channel is %d\n",channel_s); return ntStatus; }

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

                                  	if(channel_s<2 && (channel_s==channel_e))
                                  	{
                                  	 st=2;
                                         channel_e=MAX_TOPOLOGY_CHANNELS-1;
                                         if(channel_e>=that2->nchannels)
                                          channel_e=that2->nchannels-1;
                                  	}
                                  	else
                                  	 st=1;

                                  	for(i=channel_s;i<=channel_e;i+=st)
                                  	{
                                  	  that2->vol1[i]=*Level;
                                  	  if((that2->vol1[i]<KX_MIN_VOLUME) && (that2->vol1[i]!=0x80000000U) && (that2->vol1[i]!=0))
                                  	   that2->vol1[i]=KX_MIN_VOLUME;

                                  	  debug(DERR,"set waveHQ Vol1 to %d channel=%d logic\n",(*Level),i);
                                  	}
                                  	 for(i=channel_s;i<=channel_e;i+=st)
                                  	 {
                                  	  debug(DPROP,"set waveHQ Vol1 to %d channel=%d physically\n",(*Level),i);
                                  	  if((int)(((CAdapterCommon *)(that->AdapterCommon))->Topology->mixer_data[5][i&1])) // 'mute'
                                  	   kx_p16v_set_pb_volume(that2->hw,i,0x80000000);
                                  	  else
                                  	   kx_p16v_set_pb_volume(that2->hw,i,(int)(*Level)+(int)(((CAdapterCommon *)(that->AdapterCommon))->Topology->mixer_data[4][i&1]));
                                         }
                                  	break;
                                  case WAVE_VOLUME2:
                                  	 for(i=channel_s;i<=channel_e;i++)
                                  	 {
                                          that2->vol2[i]=*Level;
                                          debug(DPROP,"set waveHQ Vol2 to %d channel=%d logic\n",(*Level),i);
                                         }
                                         // FIXME: no hardware support for vol2
                                  	break;
                                  default:
                                    debug(DWDM,"!!! set unknown nodeHQ=%d\n",PropertyRequest->Node);
                                    break;
                            }
                            ntStatus = STATUS_SUCCESS;
                    } else debug(DWDM,"!!! Not vollevel in waveHQ::vollevel\n");
                } else debug(DWDM,"!!! Set more than one channel Hq\n");
            } else debug(DWDM,"!!! wrong bufsize in HQ::vollevel\n");

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
                    } else debug(DWDM,"!!! Unknown request of HQBasicSupport\n");
                    break;
                default:
                  debug(DWDM,"!!! basic support unknown %d HQ\n",PropertyRequest->Node);
                  break;
            }
        } else debug(DWDM,"!!! Unknown verb in HQPropertyLevel\n");
    } else debug(DWDM,"!!! Node is -1 in waveHQ::vollevel\n");

    if(!NT_SUCCESS(ntStatus))
      debug(DWDM,"!!! NOT OK in WaveHQ::PropertyVolume\n");

    return ntStatus;
}

#pragma code_seg()
void update_hq_volume(CMiniportWaveHQ *a)
{
  if(a)
    if(a->pb_stream)
     a->pb_stream->update_volume();
}

#pragma code_seg()
int CMiniportWaveStreamHQ::update_volume()
{
 for(int i=0;i<MAX_TOPOLOGY_CHANNELS;i++)
 {
   int p2=(int)vol1[i]+(int)(((CAdapterCommon *)(Miniport->AdapterCommon))->Topology->mixer_data[4][i&1]);
   if((int)(((CAdapterCommon *)(Miniport->AdapterCommon))->Topology->mixer_data[5][i&1]))
    p2=0x80000000;

   kx_p16v_set_pb_volume(hw,i,p2);
   /*
   debug(DWDM,"[%d dB %d dB %d dB]\n",
    (vol1[i]/0x10000),
    ((int)(((CAdapterCommon *)(Miniport->AdapterCommon))->Topology->mixer_data[4][i&1]))/0x10000,
    p2/0x10000);
   */
 }
 return 0;
}

#pragma code_seg("PAGE")
NTSTATUS CMiniportWaveStreamHQ::PropertySamplingRate(IN PPCPROPERTY_REQUEST PropertyRequest)
{
 PAGED_CODE();

    CMiniportWaveHQ *that =
        (CMiniportWaveHQ *)(PMINIPORTWAVECYCLIC) PropertyRequest->MajorTarget;

    if(!that)
    {
     debug(DWDM,"!!! WaveHQ majortarget=0! :: sample_rate\n");
     return STATUS_INVALID_PARAMETER;
    }

    if(that->magic!=WAVEHQ_MAGIC)
    {
     debug(DWDM,"!!! Bad waveHQ magic!\n");
     return STATUS_INVALID_PARAMETER;
    }

    PUNKNOWN pu=PropertyRequest->MinorTarget;
    CMiniportWaveStreamHQ *that2=(CMiniportWaveStreamHQ *)PMINIPORTWAVECYCLICSTREAM(pu);

    if(that2!=NULL)
    {
     if(that2->magic!=WAVEHQSTREAM_MAGIC)
     {
      debug(DWDM,"!!! Bad waveHQstream magic x2\n");
      return STATUS_INVALID_PARAMETER;
     }
    }
    else
    {
     if(PropertyRequest->Verb != KSPROPERTY_TYPE_BASICSUPPORT &&
       PropertyRequest->PropertyItem->Id != KSPROPERTY_AUDIO_QUALITY)
     {
      debug(DWDM,"!!! No minorTarget! in waveHQ::sampl_rate[%x];%x\n",PropertyRequest->Node,
       PropertyRequest->PropertyItem->Id);
      return STATUS_INVALID_PARAMETER;
     }
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
                    		*(PUINT)PropertyRequest->Value=(UINT)that2->cur_freq;

                                PropertyRequest->ValueSize = sizeof(LONG);
                                ntStatus = STATUS_SUCCESS;
                                break;
                        case KSPROPERTY_AUDIO_DYNAMIC_SAMPLING_RATE:
                        	*(PLONG)PropertyRequest->Value=FALSE;
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
                        	debug(DWDM,"!!! Unknown GET in waveHQ::SamplingRate\n");
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
                    	         switch(*(int *)PropertyRequest->Value)
                    	         {
//                    	           case 44100: // mode not supported due to p16v src issue? fixme
                    	           case 48000:
                    	           case 96000:
                    	           case 192000:
                    	           	// ok: change freq. dynamically
                    	           	that2->cur_freq=*(int *)PropertyRequest->Value;
                                        kx_p16v_pb_set_freq(that->hw,that2->cur_freq);
                                        debug(DWDM,"dynamic p16v freq changed [%d]\n",that2->cur_freq);
                    	           	break;
                    	           default:
                                     ntStatus=STATUS_UNSUCCESSFUL;
                                     debug(DWDM,"!! cannot set HQ Freq: (%d)\n",*(int *)PropertyRequest->Value);
                                     break;
                                 }
                                 break;
                    	case KSPROPERTY_AUDIO_DYNAMIC_SAMPLING_RATE:
                    		debug(DPROP," -- set audio_dynamic_sampling_rate HQ %d\n",*(PLONG)PropertyRequest->Value);
                                // well... this just returns 'true' (supported)
                                ntStatus = STATUS_SUCCESS;
                                break;
                    	case KSPROPERTY_AUDIO_QUALITY:
                    		debug(DPROP," -- set audio_qualityHQ\n");
                                ntStatus = STATUS_SUCCESS;
                                if((ULONG)*(PLONG)PropertyRequest->Value!=KSAUDIO_QUALITY_ADVANCED)
                                  debug(DWDM,"!!! Trying to set HQ AudioQuality = %d\n",*(PLONG)PropertyRequest->Value);
                                break;
                        default:
                        	debug(DWDM,"!!! unknown set in waveHQ::SamplingRate\n");
                        	break;
                    }
                } else  debug(DWDM,"!!! value size in HQ::samplerate\n");
        } else debug(DWDM,"!!! Unknown verb in waveHQ::SamplingRate\n");
    } else debug(DWDM,"!!! Node=-1\n");

    if(!NT_SUCCESS(ntStatus))
      debug(DWDM,"!!! NOT OK in WaveHQ::SamplingRate\n");

    return ntStatus;
}



#pragma code_seg()
int CMiniportWaveStreamHQ::Open()
{
 debug(DPV,"HQ::Open()\n");

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

    NTSTATUS ntStatus = source_channel->AllocateBuffer(source_buffer_size,&a);
    if(!NT_SUCCESS(ntStatus))
    {
       debug(DWDM,"[CMiniportWaveOutStreamHQ::Open] :: failed to allocate buffer [%x] size=%x\n",ntStatus,source_buffer_size);
       return ntStatus;
    }
 }

 kx_voice_buffer buffer;
 buffer.size=source_buffer_size;
 buffer.physical=source_channel->PhysicalAddress().LowPart;

 if(source_channel->PhysicalAddress().HighPart || source_channel->PhysicalAddress().LowPart&0x80000000)
    debug(DWDM,"!! [CMiniportWaveStreamHQ::Open]: physical address out of limits (%x.%x)\n",
         source_channel->PhysicalAddress().HighPart,
         source_channel->PhysicalAddress().LowPart);

 buffer.addr=source_channel->SystemAddress();
 buffer.that=this;
 buffer.notify=DMA_DEFAULT_NOTIFY; // 10 ms

 if(capture)
 {
   if(kx_p16v_rec_open(hw,&buffer,cur_freq,(nchannels==8)?P16V_MULTICHANNEL:P16V_STEREO)==0)
   {
    opened=1;
    return STATUS_SUCCESS;
   }
 }
 else
 {
   if(kx_p16v_pb_open(hw,&buffer,cur_freq,(nchannels==8)?P16V_MULTICHANNEL:P16V_STEREO)==0)
   {
    opened=1;

    update_volume();
    return STATUS_SUCCESS;
   }
 }
   opened=0;

 return STATUS_INVALID_PARAMETER;
}
