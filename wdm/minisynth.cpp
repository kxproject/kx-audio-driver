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
#include "wdm/tbl_synth.h"

#pragma code_seg("PAGE")
NTSTATUS create_synth
(
    OUT     PUNKNOWN *  Unknown,
    IN      REFCLSID,
    IN      PUNKNOWN    UnknownOuter    OPTIONAL,
    IN      POOL_TYPE   PoolType
)
{
    PAGED_CODE();
    ASSERT(Unknown);

    STD_CREATE_BODY_(CMiniportSynth,Unknown,UnknownOuter,PoolType,PMINIPORTMIDI);
}

#pragma code_seg("PAGE")
STDMETHODIMP CMiniportSynth::GetDescription(OUT PPCFILTER_DESCRIPTOR * OutFilterDescriptor)
{
    PAGED_CODE();

    ASSERT(OutFilterDescriptor);

    if(synth_number==0)
    {
     MiniportPins[0].KsPinDescriptor.Name=&TOPO_SYNTH1_NAME;
     MiniportPins[1].KsPinDescriptor.Name=&TOPO_SYNTH1_NAME;
     MiniportNodes[0].Name=&TOPO_SYNTH1_NAME;
    }
    else
    {
      MiniportPins[0].KsPinDescriptor.Name=&TOPO_SYNTH2_NAME;
      MiniportPins[1].KsPinDescriptor.Name=&TOPO_SYNTH2_NAME;
      MiniportNodes[0].Name=&TOPO_SYNTH2_NAME;
    }
     
    *OutFilterDescriptor = &MiniportFilterDescriptor;

    return STATUS_SUCCESS;
}


#pragma code_seg("PAGE")
STDMETHODIMP CMiniportSynth::NonDelegatingQueryInterface(IN  REFIID  Interface,
                                              OUT PVOID * Object)
{
    PAGED_CODE();
    ASSERT(Object);

    if (IsEqualGUIDAligned(Interface, IID_IUnknown))
    {
        *Object = PVOID(PUNKNOWN(PMINIPORTMIDI(this)));
    }
    else if (IsEqualGUIDAligned(Interface, IID_IMiniport))
    {
        *Object = PVOID(PMINIPORT(this));
    }
    else if (IsEqualGUIDAligned(Interface, IID_IMiniportMidi))
    {
        *Object = PVOID(PMINIPORTMIDI(this));
    }
    else
    if (IsEqualGUIDAligned(Interface,IID_IPowerNotify))
    {
        *Object = PVOID(PPOWERNOTIFY(this));
    }
    else
    if (IsEqualGUIDAligned(Interface,IID_IPinCount))
    {
    	*Object=NULL; // FIXME
    }
    else
    if (IsEqualGUIDAligned(Interface,IID_IMiniportDMus))
    {
    	*Object=NULL; // FIXME
    }
    else
    {
        *Object = NULL;
        debug(DWDM,"!!! - (Synth) try to delegate to ref_iid unknown\n");
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

    }

    if (*Object)
    {
        PUNKNOWN(*Object)->AddRef();
        return STATUS_SUCCESS;
    }

    return STATUS_INVALID_PARAMETER;
}

#pragma code_seg("PAGE")
CMiniportSynth::~CMiniportSynth()
{
    PAGED_CODE();

    debug(DWDM,"[CMiniportSynth::~CMiniportSynth]\n");
    if(magic!=SYNTH_MAGIC)
    {
     debug(DWDM,"!!! Magic!=%x (%x)\n",SYNTH_MAGIC,magic);
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
        if((synth_number<MAX_SYNTH_DEVICES) && (synth_number>=0))
          ((CAdapterCommon *)AdapterCommon)->Synth[synth_number]=NULL;

        ((CAdapterCommon *)AdapterCommon)->synth_count--;

    	AdapterCommon->Release();
    	AdapterCommon=NULL;
    }
}

#pragma code_seg("PAGE")
STDMETHODIMP CMiniportSynth::DataRangeIntersection(IN  ULONG        PinId,
                                        IN  PKSDATARANGE DataRange,
                                        IN  PKSDATARANGE MatchingDataRange,
                                        IN  ULONG        OutputBufferLength,
                                        OUT PVOID        ResultantFormat OPTIONAL,
                                        OUT PULONG       ResultantFormatLength)
{
    PAGED_CODE();


    return STATUS_NOT_IMPLEMENTED;
}

#pragma code_seg("PAGE")
STDMETHODIMP CMiniportSynth::Init(
    IN      PUNKNOWN        UnknownAdapter,
    IN  PRESOURCELIST       ResourceList,
    IN  PPORTMIDI           Port_,
    OUT PSERVICEGROUP*      ServiceGroup_
)
{
    PAGED_CODE();

    debug(DWDM,"[CMiniportSynth::Init]\n");
    ASSERT(ResourceList);
    ASSERT(Port_);
    ASSERT(ServiceGroup_);

    *ServiceGroup_ = NULL;

    PowerState=PowerDeviceD0;

    magic=SYNTH_MAGIC;
    AdapterCommon=NULL;
    Port=NULL;
    Port = Port_;
    Port->AddRef();

    NTSTATUS ntStatus =
        UnknownAdapter->QueryInterface
        (
            IID_IAdapterCommon,
            (PVOID *) &AdapterCommon
        );

    *ServiceGroup_=NULL;

    if( !NT_SUCCESS(ntStatus) )
    {
        // clean up AdapterCommon
        if( AdapterCommon )
        {
            AdapterCommon->Release();
            AdapterCommon = NULL;
        }

        // release the port
        Port->Release();
        Port = NULL;
    }
    else 
    {
     hw=((CAdapterCommon *)AdapterCommon)->hw;
     synth_number=((CAdapterCommon *)AdapterCommon)->synth_count;
     if((synth_number<MAX_SYNTH_DEVICES) && (synth_number>=0))
       ((CAdapterCommon *)AdapterCommon)->Synth[synth_number]=this;
     ((CAdapterCommon *)AdapterCommon)->synth_count++;
    }

    if(!NT_SUCCESS(ntStatus))
     debug(DWDM,"!!! Init failed\n");

    return ntStatus;
}

#pragma code_seg("PAGE")
STDMETHODIMP_(void)
CMiniportSynth::Service()
{
 PAGED_CODE();
 debug(DWDM,"[CMiniportSynth::Service]\n");
}

#pragma code_seg("PAGE")
STDMETHODIMP CMiniportSynth::NewStream
(
    OUT     PMINIPORTMIDISTREAM *   Stream,
    IN      PUNKNOWN                OuterUnknown    OPTIONAL,
    IN      POOL_TYPE               PoolType,
    IN      ULONG                   PinID,
    IN      BOOLEAN                 Capture,
    IN      PKSDATAFORMAT           DataFormat,
    OUT     PSERVICEGROUP *         ServiceGroup_
)
{
    PAGED_CODE();

    debug(DWDM,"[CMiniportSynth::NewStream]\n");

    NTSTATUS ntStatus = STATUS_SUCCESS;

    if(Capture)
    {
     return STATUS_INVALID_DEVICE_REQUEST;
    }

    CMiniportSynthStream *pStream =
                  new(PoolType,'ssXk') CMiniportSynthStream(OuterUnknown);

    if (pStream)
    {
           pStream->AddRef();

           ntStatus = pStream->Init(this);

           if (NT_SUCCESS(ntStatus))
           {
                      *Stream = PMINIPORTMIDISTREAM(pStream);
                      (*Stream)->AddRef();

                      *ServiceGroup_ = NULL;
           }

           pStream->Release();
    }
    else
    {
           ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }
    		
    return ntStatus;
}

// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------

#pragma code_seg("PAGE")
STDMETHODIMP CMiniportSynthStream::NonDelegatingQueryInterface(    REFIID  Interface,    PVOID * Object)
{
    PAGED_CODE();

    ASSERT(Object);

    if (IsEqualGUIDAligned(Interface,IID_IUnknown))
    {
        *Object = PVOID(PUNKNOWN(this));
    }
    else
    if (IsEqualGUIDAligned(Interface,IID_IMiniportMidiStream))
    {
        *Object = PVOID(PMINIPORTMIDISTREAM(this));
    }
    else
    {
        *Object = NULL;
        debug(DWDM,"!!! - (SynthStream) try to delegate to ref_iid unknown\n");
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

    }

    if (*Object)
    {
        //
        // We reference the interface for the caller.
        //
        PUNKNOWN(*Object)->AddRef();
        return STATUS_SUCCESS;
    }

    return STATUS_INVALID_PARAMETER;
}

#pragma code_seg("PAGE")
STDMETHODIMP CMiniportSynthStream::SetFormat(IN      PKSDATAFORMAT   Format)
{
    PAGED_CODE();

    ASSERT(Format);

    debug(DWDM,"!!! ignore CMiniportSynthStream::SetFormat\n");

    return STATUS_SUCCESS;
}

#pragma code_seg("PAGE")
CMiniportSynthStream::~CMiniportSynthStream(void)
{
    PAGED_CODE();

    debug(DWDM,"[~CMiniportSynthStream]\n");

    if(magic!=SYNTHSTREAM_MAGIC)
    {
     debug(DWDM,"!!! Magic!=%x (%x)\n",SYNTHSTREAM_MAGIC,magic);
     magic=0;
     return;
    }
    magic=0;

    if(midi.inited)
     kx_midi_close(&midi);
    memset(&midi,0,sizeof(midi));

    if(Miniport)
    {
        Miniport->Release();
        Miniport=NULL;
    }
}

#pragma code_seg("PAGE")
STDMETHODIMP CMiniportSynthStream::SetState( IN      KSSTATE     NewState)
{
    PAGED_CODE();
    state=NewState;

    if(state==KSSTATE_STOP || state==KSSTATE_PAUSE || state==KSSTATE_ACQUIRE)
     if(midi.inited)
      kx_midi_stop(&midi);

    return STATUS_SUCCESS;
}

#pragma code_seg("PAGE")
NTSTATUS CMiniportSynthStream::Init(IN      CMiniportSynth * pMiniport)
{
 PAGED_CODE();

 magic=SYNTHSTREAM_MAGIC;
 Miniport=pMiniport;
 Miniport->AddRef();

 hw=Miniport->hw;

 memset(&midi,0,sizeof(midi));
 if(kx_midi_init(hw,&midi,Miniport->synth_number))
 {
  debug(DWDM,"!!! midi_init failed in synthstream::Init\n");
  return STATUS_INVALID_PARAMETER;
  }
 return STATUS_SUCCESS;
}

#pragma code_seg()
STDMETHODIMP
CMiniportSynthStream::
Write
(
    IN      PVOID       BufferAddress,
    IN      ULONG       Length,
    OUT     PULONG      BytesWritten
)
{
 if(Miniport)
 {
  if(BufferAddress && Length) 
   if(midi.inited)
   {
    kx_midi_play_buffer(&midi,(byte *)BufferAddress,Length);
   }

  (*BytesWritten)=Length;
 }
 return STATUS_SUCCESS;
}

#pragma code_seg()
STDMETHODIMP
CMiniportSynthStream::
Read
(
    IN      PVOID   BufferAddress,
    IN      ULONG   Length,
    OUT     PULONG  BytesRead
)
{
    ASSERT(BufferAddress);
    ASSERT(BytesRead);

    return STATUS_INVALID_DEVICE_REQUEST;
}


#pragma code_seg()
STDMETHODIMP_(void) CMiniportSynth::PowerChangeNotify(IN      POWER_STATE     NewState)
{
    debug(DWDM,"[CMiniportSynth::PowerChangeState] - do nothing\n");
    PowerState = NewState.DeviceState;
}

