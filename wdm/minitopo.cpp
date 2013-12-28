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

#include "wdm/tbl_topo.h"

static inline unsigned axtoi(char *p)
{
 unsigned v=0;
 while(isxdigit(*p))
 {
   v=v<<4;
   toupper(*p);
   v=v|((*p>='A'?(*p-'A'+10):(*p-'0'))&0xf);
   p++;
 }
 return v;
}

static inline int atoi(const char *s)
{
  int n=0;
  while(*s>='0' && *s<='9')
  {
    n=n*10+(*s-'0');
    s++;
  }
  return(n);
}



#pragma code_seg("PAGE")
NTSTATUS create_topology
(
    OUT     PUNKNOWN *  Unknown,
    IN      REFCLSID,
    IN      PUNKNOWN    UnknownOuter    OPTIONAL,
    IN      POOL_TYPE   PoolType
)
{
    PAGED_CODE();

    ASSERT(Unknown);

    STD_CREATE_BODY_(CMiniportTopology,Unknown,UnknownOuter,PoolType,PMINIPORTTOPOLOGY);
}

#pragma code_seg("PAGE")
STDMETHODIMP CMiniportTopology::NonDelegatingQueryInterface(IN      REFIID  Interface,OUT     PVOID * Object)
{
    PAGED_CODE();

    ASSERT(Object);


    if (IsEqualGUIDAligned(Interface,IID_IUnknown))
    {
        *Object = PVOID(PUNKNOWN(PMINIPORTTOPOLOGY(this)));
    }
    else
    if (IsEqualGUIDAligned(Interface,IID_IMiniport))
    {
        *Object = PVOID(PMINIPORT(this));
    }
    else
    if (IsEqualGUIDAligned(Interface,IID_IMiniportTopology))
    {
        *Object = PVOID(PMINIPORTTOPOLOGY(this));
    }
    else
    if (IsEqualGUIDAligned(Interface,IID_IMiniportTopology))
    {
        *Object = PVOID(PMINIPORTTOPOLOGY(this));
    }
    else
    if (IsEqualGUIDAligned(Interface,IID_IPowerNotify))
    {
        *Object = PVOID(PPOWERNOTIFY(this));
    }
    else
#if 0
	3551
    if (IsEqualGUIDAligned(Interface,IID_IPinName))
    {
        *Object = PVOID(PIPINNAME(this));
    }
    else
#endif
    if (IsEqualGUIDAligned(Interface,IID_IPinCount))
    {
        *Object=NULL; // FIXME
    }
    else
    {
        debug(DWDM,"!!! - (Topology) try to delegate to refid: "
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
        //
        // We reference the interface for the caller.
        //
        PUNKNOWN(*Object)->AddRef();
        return STATUS_SUCCESS;
    }

    return STATUS_INVALID_PARAMETER;
}

#pragma code_seg("PAGE")
CMiniportTopology::~CMiniportTopology(void)
{
    PAGED_CODE();

    if(magic!=TOPOLOGY_MAGIC)
    {
        debug(DWDM," !!! [~CMiniportTopology]: magic (%x) != %x\n",magic,TOPOLOGY_MAGIC);
        magic=0;
        return;
    }

    debug(DWDM,"[CMiniportTopology::~CMiniportTopology]\n");

    if (AdapterCommon)
    {
        ((CAdapterCommon*)AdapterCommon)->Topology=NULL;
        AdapterCommon->Release();
        AdapterCommon=NULL;
    }
    if(Port)
    {   
        Port->Release();
        Port = NULL;
    }
    if (PortEvents)
    {
        PortEvents->Release();
        PortEvents = NULL;
    }
    magic=0;
}

#pragma code_seg("PAGE")
STDMETHODIMP CMiniportTopology::Init(IN      PUNKNOWN        UnknownAdapter,
                     IN      PRESOURCELIST   ResourceList,
                     IN      PPORTTOPOLOGY   Port_)
{
    PAGED_CODE();

    ASSERT(UnknownAdapter);
    ASSERT(ResourceList);
    ASSERT(Port_);

    PowerState=PowerDeviceD0;

    debug(DWDM,"[CMiniportTopology::Init]\n");

    PortEvents=NULL;
    AdapterCommon=NULL;
    magic=TOPOLOGY_MAGIC;

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
    }
    if( NT_SUCCESS(ntStatus))
    {
        Port = Port_;
        Port->AddRef();

        ((CAdapterCommon*)AdapterCommon)->Topology=this;

        dword device=0xffffffff,subsys=0xffffffff;
        byte chip_rev=0xff,bus=0xff,dev=0xff,func=0xff;
        byte bus_=0xff,dev_=0xff,func_=0xff;

        ULONG b;
        ULONG ret_b;
        if(NT_SUCCESS(Port->GetDeviceProperty(DevicePropertyAddress,sizeof(b),&b,&ret_b)))
        {
         dev_=(byte)(b>>16)&0xff;
         func_=(byte)b&0xff;
        } else debug(DWDM,"!!! port->GetDeviceProperty(Address) failed [normal under 98se]\n");

        if(NT_SUCCESS(Port->GetDeviceProperty(DevicePropertyBusNumber,sizeof(b),&b,&ret_b)))
        {
         bus_=(byte)b&0xff;
        } else debug(DWDM,"!!! port->GetDeviceProperty(Bus#) failed\n");

        WCHAR info[256];
        char info_[256];
        char *t;
        int i;
        if(NT_SUCCESS(Port->GetDeviceProperty(DevicePropertyLocationInformation,sizeof(info),info,&ret_b)))
        {
          for(i=0;i<256;i++)
           info_[i]=(byte)info[i];
          // debug(DWDM,"port->GetDeviceProperty(Location): %S\n",info);

          t=strstr(info_,"bus");
          if(t)
            bus=(byte)atoi(t+4);
          t=strstr(info_,"device");
          if(t)
            dev=(byte)atoi(t+7);
          t=strstr(info_,"function");
          if(t)
            func=(byte)atoi(t+9);
        }

        if( (bus!=bus_ || bus==0xff) ||
            (dev!=dev_ || dev==0xff) ||
            (func!=func_ || func_==0xff) )
            {
             bus=0xff;
            }

        if(NT_SUCCESS(Port->GetDeviceProperty(DevicePropertyHardwareID,sizeof(info),info,&ret_b)))
        {
          for(i=0;i<256;i++)
           info_[i]=(byte)info[i];

          memcpy(((CAdapterCommon*)AdapterCommon)->device_name,info,sizeof(((CAdapterCommon*)AdapterCommon)->device_name));

          // debug(DWDM,"port->GetDeviceProperty(HW ID) ok: %S\n",info);

          t=strstr(info_,"VEN_");
          if(t)
          {
           device=axtoi(t+4);
          }

          t=strstr(info_,"DEV_");
          if(t)
          {
           device|=(axtoi(t+4)&0xffff)<<16;
          }

          t=strstr(info_,"SUBSYS_");
          if(t)
          {
           subsys=axtoi(t+7);
          }

          t=strstr(info_,"REV_");
          if(t)
          {
           chip_rev=(byte)axtoi(t+4);
          }
        } else debug(DWDM,"!!! Fatal: Port->GetDeviceProperty(Dev.HwID) failed!\n");

        if(device==0xffffffff || subsys==0xffffffff || chip_rev==0xff)
        {
         debug(DWDM,"Error getting PCI Dev/Subsys/Rev params... will try to autodetect...\n");
         bus=0xff;
        }

        debug(DWDM,"[Hardware PCI ID: dev:%x subsys:%x chip_rev:%x located @ bus: %x dev: %x func: %x]\n",
         device,subsys,chip_rev,bus,dev,func);

        if(((CAdapterCommon*)AdapterCommon)->my_init(device,subsys,chip_rev,bus,dev,func))
           ntStatus=STATUS_UNSUCCESSFUL;
        hw=((CAdapterCommon *)AdapterCommon)->hw;
    }

    if( NT_SUCCESS(ntStatus))
    {
        // Get the port event interface.
        //
        Port->QueryInterface (IID_IPortEvents, (PVOID *)&PortEvents);
    }

    if( NT_SUCCESS(ntStatus))
    {
        ((CAdapterCommon*)AdapterCommon)->Topology=this;
    }

    if(!NT_SUCCESS(ntStatus))
     debug(DWDM,"!!! Init failed\n");

    memset(mixer_data,0,sizeof(mixer_data));

    for(int i=0;i<MAX_TOPOLOGY_CHANNELS;i++)
    {
     mixer_data[TOPO_MICIN_MUTE][i]=1;
     mixer_data[TOPO_LINEIN_MUTE][i]=1;
     mixer_data[TOPO_LINEIN_VOLUME][i]=0x80000000;
     mixer_data[TOPO_MICIN_VOLUME][i]=0x80000000;
    }
    return ntStatus;
}

#pragma code_seg("PAGE")
STDMETHODIMP CMiniportTopology::GetDescription(OUT PPCFILTER_DESCRIPTOR *  OutFilterDescriptor)
{
    PAGED_CODE();

    ASSERT(OutFilterDescriptor);

    // support for Vista/Windows7 and XP
    // if unspecified, Vista/Windows7 will rename the devices
    if(((CAdapterCommon *)AdapterCommon)->is_vista)
     MiniportPins[TOPO_MASTER_MIXER_DEST].KsPinDescriptor.Category=&KSNODETYPE_LINE_CONNECTOR;
    else
     MiniportPins[TOPO_MASTER_MIXER_DEST].KsPinDescriptor.Category=&KSNODETYPE_SPEAKER;

    *OutFilterDescriptor = &MiniportFilterDescriptor;
    return STATUS_SUCCESS;
}


#pragma code_seg("PAGE")
STDMETHODIMP CMiniportTopology::DataRangeIntersection
    (   IN      ULONG           PinId
    ,   IN      PKSDATARANGE    DataRange
    ,   IN      PKSDATARANGE    MatchingDataRange
    ,   IN      ULONG           OutputBufferLength
    ,   OUT     PVOID           ResultantFormat     OPTIONAL
    ,   OUT     PULONG          ResultantFormatLength
    )
{
    PAGED_CODE();
    debug(DWDM,"!!! topo::intersection (not impl)\n");
    return STATUS_NOT_IMPLEMENTED;
}

/*****************************************************************************
 * BasicSupportHandler()
 *****************************************************************************
 * Assists in BASICSUPPORT accesses on level properties
 */
#pragma code_seg("PAGE")
NTSTATUS BasicSupportHandler(IN PPCPROPERTY_REQUEST   PropertyRequest)
{
    ASSERT(PropertyRequest);

    PAGED_CODE();

    NTSTATUS ntStatus = STATUS_INVALID_DEVICE_REQUEST;

    if(PropertyRequest->ValueSize >= (sizeof(KSPROPERTY_DESCRIPTION)))
    {
        // if return buffer can hold a KSPROPERTY_DESCRIPTION, return it
        PKSPROPERTY_DESCRIPTION PropDesc = PKSPROPERTY_DESCRIPTION(PropertyRequest->Value);

        PropDesc->AccessFlags = KSPROPERTY_TYPE_BASICSUPPORT |
                                KSPROPERTY_TYPE_GET |
                                KSPROPERTY_TYPE_SET;
        PropDesc->DescriptionSize   = sizeof(KSPROPERTY_DESCRIPTION) +
                                      sizeof(KSPROPERTY_MEMBERSHEADER) +
                                      sizeof(KSPROPERTY_STEPPING_LONG);
        PropDesc->PropTypeSet.Set   = KSPROPTYPESETID_General;
        PropDesc->PropTypeSet.Id    = VT_I4;
        PropDesc->PropTypeSet.Flags = 0;
        PropDesc->MembersListCount  = 1;
        PropDesc->Reserved          = 0;

        // if return buffer cn also hold a range description, return it too
        if(PropertyRequest->ValueSize >= (sizeof(KSPROPERTY_DESCRIPTION) +
                                      sizeof(KSPROPERTY_MEMBERSHEADER) +
                                      sizeof(KSPROPERTY_STEPPING_LONG)))
        {
            // fill in the members header
            PKSPROPERTY_MEMBERSHEADER Members = PKSPROPERTY_MEMBERSHEADER(PropDesc + 1);

            Members->MembersFlags   = KSPROPERTY_MEMBER_STEPPEDRANGES;
            Members->MembersSize    = sizeof(KSPROPERTY_STEPPING_LONG);
            Members->MembersCount   = 1;
            Members->Flags          = 0;

            // fill in the stepped range
            PKSPROPERTY_STEPPING_LONG Range = PKSPROPERTY_STEPPING_LONG(Members + 1);

            switch(PropertyRequest->Node)
            {
                default:
                    debug(DWDM,"!!! Unknown Node in basicsupport [%d]\n",PropertyRequest->Node);
                case TOPO_MASTER_VOLUME:
                //case WAVE_VOLUME1:
                //case WAVE_VOLUME2:
                case TOPO_RECIN_VOLUME:
                case TOPO_WAVEOUT01_VOLUME:
                #ifdef USE_MULTY_WAVE
                case TOPO_WAVEOUT23_VOLUME:
                case TOPO_WAVEOUT45_VOLUME:
                case TOPO_WAVEOUT67_VOLUME:
                #endif
                case TOPO_SYNTH1_VOLUME:
                case TOPO_LINEIN_VOLUME:
                case TOPO_MICIN_VOLUME:
                //case TOPO_SYNTH2_VOLUME:
                    Range->Bounds.SignedMaximum = KX_MAX_VOLUME;
                    Range->Bounds.SignedMinimum = KX_MIN_VOLUME;
                    Range->SteppingDelta        = KX_VOLUME_STEP;
                    break;
            }
            Range->Reserved         = 0;

            // set the return value size
            PropertyRequest->ValueSize = sizeof(KSPROPERTY_DESCRIPTION) +
                                         sizeof(KSPROPERTY_MEMBERSHEADER) +
                                         sizeof(KSPROPERTY_STEPPING_LONG);
        } 
         else
        {
            // set the return value size
            PropertyRequest->ValueSize = sizeof(KSPROPERTY_DESCRIPTION);
        }
        ntStatus = STATUS_SUCCESS;

    } 
     else if(PropertyRequest->ValueSize >= sizeof(ULONG))
    {
        // if return buffer can hold a ULONG, return the access flags
        PULONG AccessFlags = PULONG(PropertyRequest->Value);

        *AccessFlags = KSPROPERTY_TYPE_BASICSUPPORT |
                       KSPROPERTY_TYPE_GET |
                       KSPROPERTY_TYPE_SET;

        // set the return value size
        PropertyRequest->ValueSize = sizeof(ULONG);
        ntStatus = STATUS_SUCCESS;
    }

    return ntStatus;
}


#pragma code_seg("PAGE")
NTSTATUS CMiniportTopology::PropertyLevel(IN PPCPROPERTY_REQUEST PropertyRequest)
{
    PAGED_CODE();

    ASSERT(PropertyRequest);

    CMiniportTopology *that =
        (CMiniportTopology *) (PMINIPORTTOPOLOGY)PropertyRequest->MajorTarget;

    if(!that)
    {
     debug(DWDM,"!!! Topology majortarget=0!\n");
     return STATUS_INVALID_PARAMETER;
    }

    if(that->magic!=TOPOLOGY_MAGIC)
    {
     debug(DWDM,"!!! Bad topology magic!\n");
     return STATUS_INVALID_PARAMETER;
    }

    NTSTATUS        ntStatus = STATUS_INVALID_PARAMETER;
    LONG            channel;

    // validate node
    if(PropertyRequest->Node != ULONG(-1))
    {
        if(PropertyRequest->Verb & KSPROPERTY_TYPE_GET)
        {
            // get the instance channel parameter
            if(PropertyRequest->InstanceSize >= sizeof(LONG))
            {
                channel = *(PLONG(PropertyRequest->Instance));
                if(channel>=MAX_TOPOLOGY_CHANNELS)
                 { debug(DWDM,"!!! Channel is %d\n",channel); return ntStatus; }
                if(channel==-1)
                 { debug(DWDM,"!!! Channel = -1\n"); channel=0; }

                // validate and get the output parameter
                if (PropertyRequest->ValueSize >= sizeof(LONG))
                {
                    PLONG Level = (PLONG)PropertyRequest->Value;

                    // check if volume property request
                    if(PropertyRequest->PropertyItem->Id == KSPROPERTY_AUDIO_VOLUMELEVEL)
                    {
                                switch(PropertyRequest->Node)
                                {
                                  case TOPO_MASTER_VOLUME:
                                    *Level=that->mixer_data[TOPO_MASTER_VOLUME][channel];
                                    debug(DPROP,"Get MasterVolume to %d channel=%d\n",(*Level),channel);
                                    break;
                                  case TOPO_WAVEOUT01_VOLUME:
                                  #ifdef USE_MULTY_WAVE
                                  case TOPO_WAVEOUT23_VOLUME:
                                  case TOPO_WAVEOUT45_VOLUME:
                                  case TOPO_WAVEOUT67_VOLUME:
                                  #endif
                                    *Level=that->mixer_data[PropertyRequest->Node][channel];
                                    // FIXME 3551 Windows 7 calls this:
                                    // debug(DWDM,"!!! Get WaveOutVolume to %d channel=%d\n",(*Level),channel);
                                    break;
                                  case TOPO_SYNTH1_VOLUME:
                                    *Level=that->mixer_data[TOPO_SYNTH1_VOLUME][channel];
                                    debug(DPROP,"Get SynthVolume to %d channel=%d\n",(*Level),channel);
                                    break;
                                        /*
                                  case TOPO_SYNTH2_VOLUME:
                                    *(PLONG)PropertyRequest->Value=that->mixer_data[TOPO_SYNTH2_VOLUME][channel];
                                    debug(DPROP,"Get SynthVolume to %d channel=%d\n",(*Level),channel);
                                    break;
                                        */
                          case TOPO_RECIN_VOLUME:
                            *Level=that->mixer_data[TOPO_RECIN_VOLUME][channel];
                            debug(DPROP,"Get RecIn volume to %d channel=%d\n",(*Level),channel);
                            break;
                          case TOPO_LINEIN_VOLUME:
                            *Level=that->mixer_data[TOPO_LINEIN_VOLUME][channel];
                            debug(DPROP,"Get LineIn volume to %d channel=%d\n",(*Level),channel);
                            break;
                          case TOPO_MICIN_VOLUME:
                            *Level=that->mixer_data[TOPO_MICIN_VOLUME][channel];
                            debug(DPROP,"Get MicIn volume to %d channel=%d\n",(*Level),channel);
                            break;
                                  default:
                                    debug(DWDM,"!!! get unknown node=%d\n",PropertyRequest->Node);
                                    break;
                                }
                                PropertyRequest->ValueSize = sizeof(LONG);
                                ntStatus = STATUS_SUCCESS;
                    } else debug(DWDM,"!!! id!=audio in topo::vol\n");
                }  else debug(DWDM,"!!! bad value size in topo::vol\n");
            }  else debug(DWDM,"!!! bad inst size in topo::vol\n");

        } else if(PropertyRequest->Verb & KSPROPERTY_TYPE_SET)
        {
            // get the instance channel parameter
            if(PropertyRequest->InstanceSize >= sizeof(LONG))
            {
                channel = *(PLONG(PropertyRequest->Instance));
                if(channel>=MAX_TOPOLOGY_CHANNELS)
                 { debug(DWDM,"!!! Channel is %d\n",channel); return ntStatus; }
                if(channel==-1)
                 { debug(DWDM,"!!! Channel = -1\n"); channel=0; }

                // validate and get the input parameter
                if (PropertyRequest->ValueSize == sizeof(LONG))
                {
                    PLONG Level = (PLONG)PropertyRequest->Value;

                    if(PropertyRequest->PropertyItem->Id == KSPROPERTY_AUDIO_VOLUMELEVEL)
                    {
                        // switch on the node id
                            switch(PropertyRequest->Node)
                            {
                                  case TOPO_MASTER_VOLUME:
                                    that->mixer_data[TOPO_MASTER_VOLUME][channel]=*Level;
                                    debug(DPROP,"set MasterVolume to %d channel=%d\n",(*Level),channel);
                                    dword val;
                                    val=*Level;
                                    if(that->mixer_data[TOPO_MASTER_MUTE][channel])
                                     val=KX_MIN_VOLUME;

                                    if(channel<=1 && that->hw->cb.mixer_controls[MIXER_MASTER].pgm_name[0])
                                     kx_set_volume(that->hw,that->hw->cb.mixer_controls[MIXER_MASTER].pgm_name,
                                       (channel==0)?that->hw->cb.mixer_controls[MIXER_MASTER].reg_left:
                                      that->hw->cb.mixer_controls[MIXER_MASTER].reg_right,val,
                                      that->hw->cb.mixer_controls[MIXER_MASTER].max_vol);

                                    if(that->hw->is_a2)
                                    {
                                     extern void update_hq_volume(CMiniportWaveHQ *);
                                     update_hq_volume(((CAdapterCommon *)(that->AdapterCommon))->WaveHQ);
                                    }
                                    break;
                                  case TOPO_WAVEOUT01_VOLUME:
                                  #ifdef USE_MULTY_WAVE
                                  case TOPO_WAVEOUT23_VOLUME:
                                  case TOPO_WAVEOUT45_VOLUME:
                                  case TOPO_WAVEOUT67_VOLUME:
                                  #endif
                                    {
                                    that->mixer_data[PropertyRequest->Node][channel]=*Level;
                                    debug(DPROP,"set WaveOutVolume to %d channel=%d\n",(*Level),channel);
                                    dword val;
                                    val=*Level;
                                    if(that->mixer_data[TOPO_WAVEOUT01_MUTE][channel])
                                     val=KX_MIN_VOLUME;

                                    if(channel<=1 && that->hw->cb.mixer_controls[MIXER_WAVE].pgm_name[0])
                                     kx_set_volume(that->hw,that->hw->cb.mixer_controls[MIXER_WAVE].pgm_name,
                                     (channel==0)?that->hw->cb.mixer_controls[MIXER_WAVE].reg_left:
                                      that->hw->cb.mixer_controls[MIXER_WAVE].reg_right,val,
                                      that->hw->cb.mixer_controls[MIXER_WAVE].max_vol);
                                    }
                                    break;
                                  case TOPO_SYNTH1_VOLUME:
                                    {
                                    that->mixer_data[TOPO_SYNTH1_VOLUME][channel]=*Level;
                                    debug(DPROP,"set SynthVolume to %d channel=%d\n",(*Level),channel);

                                    dword val;
                                    val=*Level;
                                    if(that->mixer_data[TOPO_SYNTH1_MUTE][channel])
                                     val=KX_MIN_VOLUME;

                                    if(channel<=1)
                                    {
                                     if(that->hw->cb.mixer_controls[MIXER_SYNTH].pgm_name[0]==0)
                                         kx_midi_set_volume(that->hw,channel,val);
                                     else
                                         kx_set_volume(that->hw,that->hw->cb.mixer_controls[MIXER_SYNTH].pgm_name,
                                           (channel==0)?that->hw->cb.mixer_controls[MIXER_SYNTH].reg_left:
                                           that->hw->cb.mixer_controls[MIXER_SYNTH].reg_right,
                                           val,
                                           that->hw->cb.mixer_controls[MIXER_SYNTH].max_vol);
                                    }
                                    }
                                    break;
                          case TOPO_RECIN_VOLUME:
                            {
                            that->mixer_data[TOPO_RECIN_VOLUME][channel]=*Level;
                            debug(DPROP,"set RecIn volume to %d channel=%d\n",(*Level),channel);

                            dword val;
                            val=*Level;
                            if(that->mixer_data[TOPO_RECIN_MUTE][channel])
                             val=KX_MIN_VOLUME;

                            if(channel<=1 && that->hw->cb.mixer_controls[MIXER_REC].pgm_name[0])
                                         kx_set_volume(that->hw,that->hw->cb.mixer_controls[MIXER_REC].pgm_name,
                                           (channel==0)?that->hw->cb.mixer_controls[MIXER_REC].reg_left:
                                           that->hw->cb.mixer_controls[MIXER_REC].reg_right,val,
                                           that->hw->cb.mixer_controls[MIXER_REC].max_vol);

                                        if(that->hw->is_a2 && channel==0)
                                         kx_p16v_set_rec_volume(that->hw,val);
                            }
                            break;
                          case TOPO_LINEIN_VOLUME:
                            {
                            that->mixer_data[TOPO_LINEIN_VOLUME][channel]=*Level;
                            debug(DPROP,"set LineIn volume to %d channel=%d\n",(*Level),channel);

                            dword val;
                            val=*Level;
                            if(that->mixer_data[TOPO_LINEIN_MUTE][channel])
                             val=KX_MIN_VOLUME;

                            if(channel<=1 && that->hw->cb.mixer_controls[MIXER_KX0].pgm_name[0])
                             kx_set_volume(that->hw,
                                 that->hw->cb.mixer_controls[MIXER_KX0].pgm_name,
                                           (channel==0)?that->hw->cb.mixer_controls[MIXER_KX0].reg_left:
                                           that->hw->cb.mixer_controls[MIXER_KX0].reg_right,val,
                                           that->hw->cb.mixer_controls[MIXER_KX0].max_vol);
                            }
                            break;
                          case TOPO_MICIN_VOLUME:
                            {
                            that->mixer_data[TOPO_MICIN_VOLUME][channel]=*Level;
                            debug(DPROP,"set MicIn volume to %d channel=%d\n",(*Level),channel);

                            dword val;
                            val=*Level;
                            if(that->mixer_data[TOPO_MICIN_MUTE][channel])
                             val=KX_MIN_VOLUME;

                            if(channel<=1 && that->hw->cb.mixer_controls[MIXER_KX1].pgm_name[0])
                              kx_set_volume(that->hw,
                                that->hw->cb.mixer_controls[MIXER_KX1].pgm_name,
                                          (channel==0)?that->hw->cb.mixer_controls[MIXER_KX1].reg_left:
                                          that->hw->cb.mixer_controls[MIXER_KX1].reg_right,val,
                                          that->hw->cb.mixer_controls[MIXER_KX1].max_vol);
                            }
                            break;
                                  default:
                                    debug(DWDM,"!!! set unknown node=%d",PropertyRequest->Node);
                                    break;
                            }
                            ntStatus = STATUS_SUCCESS;
                    }  else debug(DWDM,"!!! bad id in topo::vol\n");
                }  else debug(DWDM,"!!! bad value size in topo::vol\n");
            }  else debug(DWDM,"!!! bad inst size in topo::vol\n");

        } else if(PropertyRequest->Verb & KSPROPERTY_TYPE_BASICSUPPORT)
        {
            // service basic support request
            switch(PropertyRequest->Node)
            {
                case TOPO_MASTER_VOLUME:
                case TOPO_RECIN_VOLUME:
                case TOPO_WAVEOUT01_VOLUME:
                #ifdef USE_MULTY_WAVE
                case TOPO_WAVEOUT23_VOLUME:
                case TOPO_WAVEOUT45_VOLUME:
                case TOPO_WAVEOUT67_VOLUME:
                #endif
                case TOPO_SYNTH1_VOLUME:
                case TOPO_LINEIN_VOLUME:
                case TOPO_MICIN_VOLUME:
                //case TOPO_SYNTH2_VOLUME:
                    debug(DPROP,"Basic support call for %d\n",PropertyRequest->Node);
                    if(PropertyRequest->PropertyItem->Id == KSPROPERTY_AUDIO_VOLUMELEVEL)
                    {
                        ntStatus = BasicSupportHandler(PropertyRequest);
                    } else debug(DWDM,"!!! Unknown request of BasicSupport\n");
                    break;
                default:
                  debug(DWDM,"!!! basic support unknown node %d\n",PropertyRequest->Node);
                  break;
            }
        } else debug(DWDM,"!!! Unknown verb in PropertyLevel\n");
    }  else debug(DWDM,"!!! node=-1 in topo::vol\n");

    if(!NT_SUCCESS(ntStatus))
      debug(DWDM,"!!! NOT OK in Topology::PropertyLevel\n");

    return ntStatus;
}

#pragma code_seg("PAGE")
// FIXME: never called
NTSTATUS CMiniportTopology::PropertyCpuResources(IN PPCPROPERTY_REQUEST   PropertyRequest)
{
    PAGED_CODE();

    ASSERT(PropertyRequest);

    debug(DWDM,"[CMiniportTopology::PropertyCpuResources]\n");

    NTSTATUS ntStatus = STATUS_INVALID_DEVICE_REQUEST;

        if(PropertyRequest->Verb & KSPROPERTY_TYPE_GET)
        {
            if(PropertyRequest->ValueSize >= sizeof(LONG))
            {
                *(PLONG(PropertyRequest->Value)) = KSAUDIO_CPU_RESOURCES_NOT_HOST_CPU;
                PropertyRequest->ValueSize = sizeof(LONG);
                debug(DPROP,"Topo:cpu: get\n");
                ntStatus = STATUS_SUCCESS;
            } else
            {
                debug(DWDM,"!!! cpu_res: buf too small\n");
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
                debug(DPROP,"Topo:cpu: basic support 1\n");
                ntStatus = STATUS_SUCCESS;
            } else if(PropertyRequest->ValueSize >= sizeof(ULONG))
            {
                // if return buffer can hold a ULONG, return the access flags
                PULONG AccessFlags = PULONG(PropertyRequest->Value);
        
                *AccessFlags = KSPROPERTY_TYPE_BASICSUPPORT |
                               KSPROPERTY_TYPE_GET |
                               KSPROPERTY_TYPE_SET;
        
                // set the return value size
                PropertyRequest->ValueSize = sizeof(ULONG);
                debug(DPROP,"Topo:cpu: basic support 2\n");
                ntStatus = STATUS_SUCCESS;                    
            }
        } else debug(DWDM,"!!! unkn verb in topo::cpures\n");

    if(!NT_SUCCESS(ntStatus))
     debug(DWDM,"!!! Error CpuResources Property op\n");

    return ntStatus;
}

#pragma code_seg("PAGE")
NTSTATUS CMiniportTopology::PropertyOnOff(IN PPCPROPERTY_REQUEST PropertyRequest)
{
    PAGED_CODE();

    ASSERT(PropertyRequest);

    CMiniportTopology *that =
        (CMiniportTopology *)(IMiniportTopology *)PropertyRequest->MajorTarget;

    if(!that)
    {
     debug(DWDM,"!!! Topology majortarget=0! (in mute)\n");
     return STATUS_INVALID_PARAMETER;
    }

    if(that->magic!=TOPOLOGY_MAGIC)
    {
     debug(DWDM,"!!! Bad topology magic!\n");
     return STATUS_INVALID_PARAMETER;
    }


    NTSTATUS        ntStatus = STATUS_INVALID_PARAMETER;
    ULONG           count;
    LONG            channel;

    // validate node
    if(PropertyRequest->Node != ULONG(-1))
    {
        if(PropertyRequest->Verb & KSPROPERTY_TYPE_GET)
        {
            // get the instance channel parameter
            if(PropertyRequest->InstanceSize >= sizeof(LONG))
            {
                channel = *(PLONG(PropertyRequest->Instance));
                if(channel>=MAX_TOPOLOGY_CHANNELS)
                 { debug(DWDM,"!!! Channel is %d\n",channel); return ntStatus; }
                if(channel==-1)
                 { debug(DWDM,"!!! Channel = -1\n"); channel=0; }

                // validate and get the output parameter
                if (PropertyRequest->ValueSize >= sizeof(LONG))
                {
                    PLONG Level = (PLONG)PropertyRequest->Value;

                    // check if volume property request
                    if(PropertyRequest->PropertyItem->Id == KSPROPERTY_AUDIO_MUTE)
                    {
                            switch(PropertyRequest->Node)
                                {
                                  case TOPO_LINEIN_MUTE:
                                    *Level=that->mixer_data[TOPO_LINEIN_MUTE][channel];
                                    debug(DPROP,"Get LineIN MUTE to %d channel=%d\n",(*Level),channel);
                                    break;
                                  case TOPO_MICIN_MUTE:
                                    *Level=that->mixer_data[TOPO_MICIN_MUTE][channel];
                                    debug(DPROP,"Get MicIN MUTE to %d channel=%d\n",(*Level),channel);
                                    break;
                                  case TOPO_MASTER_MUTE:
                                        *Level=that->mixer_data[TOPO_MASTER_MUTE][channel];
                                    debug(DPROP,"Get Master MUTE to %d channel=%d\n",(*Level),channel);
                                    break;
                                  case TOPO_RECIN_MUTE:
                                        *Level=that->mixer_data[TOPO_RECIN_MUTE][channel];
                                    debug(DPROP,"Get Rec MUTE to %d channel=%d\n",(*Level),channel);
                                    break;
                                  case TOPO_WAVEOUT01_MUTE:
                                  #ifdef USE_MULTY_WAVE
                                  case TOPO_WAVEOUT23_MUTE:
                                  case TOPO_WAVEOUT45_MUTE:
                                  case TOPO_WAVEOUT67_MUTE:
                                  #endif
                                        *Level=that->mixer_data[PropertyRequest->Node][channel];
                                        // 3551; Windows 7 calls this FIXME
                                         // debug(DWDM,"!! Get WaveOut MUTE to %d channel=%d\n",(*Level),channel);
                                    break;
                                  case TOPO_SYNTH1_MUTE:
                                        *Level=that->mixer_data[TOPO_SYNTH1_MUTE][channel];
                                    debug(DPROP,"Get Synth MUTE to %d channel=%d\n",(*Level),channel);
                                    break;
                                        /*
                                  case TOPO_SYNTH2_MUTE:
                                        *(PLONG)PropertyRequest->Value=that->mixer_data[TOPO_SYNTH2_MUTE][channel];
                                    debug(DPROP,"Get Synth2 MUTE to %d channel=%d\n",(*Level),channel);
                                    break;
                                        */
                                  default:
                                    debug(DWDM,"!!! get MUTE unknown node=%d\n",PropertyRequest->Node);
                                    break;
                                }
                                PropertyRequest->ValueSize = sizeof(LONG);
                                ntStatus = STATUS_SUCCESS;
                    } else debug(DWDM,"!!! id!=audio in topo::mute\n");
                }  else debug(DWDM,"!!! bad value size in topo::mute\n");
            }  else debug(DWDM,"!!! bad inst size in topo::mute\n");

        } else if(PropertyRequest->Verb & KSPROPERTY_TYPE_SET)
        {
            // get the instance channel parameter
            if(PropertyRequest->InstanceSize >= sizeof(LONG))
            {
                channel = *(PLONG(PropertyRequest->Instance));

                // validate and get the input parameter
                if (PropertyRequest->ValueSize == sizeof(LONG))
                {
                    PLONG Level = (PLONG)PropertyRequest->Value;

                    // check if volume property request
                    if(PropertyRequest->PropertyItem->Id == KSPROPERTY_AUDIO_MUTE)
                    {
                            switch(PropertyRequest->Node)
                                {
                                  case TOPO_WAVEOUT01_MUTE:
                                  #ifdef USE_MULTY_WAVE
                                  case TOPO_WAVEOUT23_MUTE:
                                  case TOPO_WAVEOUT45_MUTE:
                                  case TOPO_WAVEOUT67_MUTE:
                                  #endif
                                      debug(DPROP,"set WaveOut MUTE to %d channel=%d\n",(*Level),channel);
                                      that->mixer_data[PropertyRequest->Node][channel]=*(PLONG)PropertyRequest->Value;

                                      if(channel<=1 && that->hw->cb.mixer_controls[MIXER_WAVE].pgm_name[0])
                                      {
                                       if(*Level)
                                          kx_set_volume(that->hw,
                                             that->hw->cb.mixer_controls[MIXER_WAVE].pgm_name,
                                                       (channel==0)?that->hw->cb.mixer_controls[MIXER_WAVE].reg_left:
                                                       that->hw->cb.mixer_controls[MIXER_WAVE].reg_right,0x80000000);
                                       else
                                          kx_set_volume(that->hw,
                                             that->hw->cb.mixer_controls[MIXER_WAVE].pgm_name,
                                                       (channel==0)?that->hw->cb.mixer_controls[MIXER_WAVE].reg_left:
                                                       that->hw->cb.mixer_controls[MIXER_WAVE].reg_right,
                                                       that->mixer_data[TOPO_WAVEOUT01_VOLUME][channel],
                                                       that->hw->cb.mixer_controls[MIXER_WAVE].max_vol);
                                      }
                                    break;
                                  case TOPO_LINEIN_MUTE:
                                        debug(DPROP,"set LineIn MUTE to %d channel=%d\n",(*Level),channel);
                                    that->mixer_data[TOPO_LINEIN_MUTE][channel]=*Level;

                                      if(channel<=1 && that->hw->cb.mixer_controls[MIXER_KX0].pgm_name[0])
                                      {
                                       if(*Level)
                                          kx_set_volume(that->hw,
                                             that->hw->cb.mixer_controls[MIXER_KX0].pgm_name,
                                                       (channel==0)?that->hw->cb.mixer_controls[MIXER_KX0].reg_left:
                                                       that->hw->cb.mixer_controls[MIXER_KX0].reg_right,0x80000000);
                                       else
                                          kx_set_volume(that->hw,
                                             that->hw->cb.mixer_controls[MIXER_KX0].pgm_name,
                                                       (channel==0)?that->hw->cb.mixer_controls[MIXER_KX0].reg_left:
                                                       that->hw->cb.mixer_controls[MIXER_KX0].reg_right,
                                                       that->mixer_data[TOPO_LINEIN_VOLUME][channel],
                                                       that->hw->cb.mixer_controls[MIXER_KX0].max_vol);
                                      }

                                    break;
                                  case TOPO_MICIN_MUTE:
                                        debug(DPROP,"set MicIn MUTE to %d channel=%d\n",(*Level),channel);
                                    that->mixer_data[TOPO_MICIN_MUTE][channel]=*Level;

                                      if(channel<=1 && that->hw->cb.mixer_controls[MIXER_KX1].pgm_name[0])
                                      {
                                       if(*Level)
                                          kx_set_volume(that->hw,
                                             that->hw->cb.mixer_controls[MIXER_KX1].pgm_name,
                                                       (channel==0)?that->hw->cb.mixer_controls[MIXER_KX1].reg_left:
                                                       that->hw->cb.mixer_controls[MIXER_KX1].reg_right,0x80000000);
                                       else
                                          kx_set_volume(that->hw,
                                             that->hw->cb.mixer_controls[MIXER_KX1].pgm_name,
                                                       (channel==0)?that->hw->cb.mixer_controls[MIXER_KX1].reg_left:
                                                       that->hw->cb.mixer_controls[MIXER_KX1].reg_right,
                                                       that->mixer_data[TOPO_MICIN_VOLUME][channel],
                                                       that->hw->cb.mixer_controls[MIXER_KX1].max_vol);
                                      }

                                    break;
                                  case TOPO_MASTER_MUTE:
                                    debug(DPROP,"set Master MUTE to %d channel=%d\n",(*Level),channel);
                                        that->mixer_data[TOPO_MASTER_MUTE][channel]=*Level;

                                      if(channel<=1 && that->hw->cb.mixer_controls[MIXER_MASTER].pgm_name[0])
                                      {
                                       if(*Level)
                                          kx_set_volume(that->hw,
                                             that->hw->cb.mixer_controls[MIXER_MASTER].pgm_name,
                                                       (channel==0)?that->hw->cb.mixer_controls[MIXER_MASTER].reg_left:
                                                       that->hw->cb.mixer_controls[MIXER_MASTER].reg_right,0x80000000);
                                       else
                                          kx_set_volume(that->hw,
                                             that->hw->cb.mixer_controls[MIXER_MASTER].pgm_name,
                                                       (channel==0)?that->hw->cb.mixer_controls[MIXER_MASTER].reg_left:
                                                       that->hw->cb.mixer_controls[MIXER_MASTER].reg_right,
                                                       that->mixer_data[TOPO_MASTER_VOLUME][channel],
                                                       that->hw->cb.mixer_controls[MIXER_MASTER].max_vol);
                                      }

                                    if(that->hw->is_a2)
                                    {
                                     extern void update_hq_volume(CMiniportWaveHQ *);
                                     update_hq_volume(((CAdapterCommon *)(that->AdapterCommon))->WaveHQ);
                                    }
                                    break;
                                  case TOPO_RECIN_MUTE:
                                    debug(DPROP,"set Rec MUTE to %d channel=%d\n",(*Level),channel);
                                        that->mixer_data[TOPO_RECIN_MUTE][channel]=*Level;

                                      if(channel<=1 && that->hw->cb.mixer_controls[MIXER_REC].pgm_name[0])
                                      {
                                       if(*Level)
                                          kx_set_volume(that->hw,
                                             that->hw->cb.mixer_controls[MIXER_REC].pgm_name,
                                                       (channel==0)?that->hw->cb.mixer_controls[MIXER_REC].reg_left:
                                                       that->hw->cb.mixer_controls[MIXER_REC].reg_right,0x80000000);
                                       else
                                          kx_set_volume(that->hw,
                                             that->hw->cb.mixer_controls[MIXER_REC].pgm_name,
                                                       (channel==0)?that->hw->cb.mixer_controls[MIXER_REC].reg_left:
                                                       that->hw->cb.mixer_controls[MIXER_REC].reg_right,
                                                       that->mixer_data[TOPO_RECIN_VOLUME][channel],
                                                       that->hw->cb.mixer_controls[MIXER_REC].max_vol);
                                      }
                                    break;
                                  case TOPO_SYNTH1_MUTE:
                                    debug(DPROP,"set Synth MUTE to %d channel=%d\n",(*Level),channel);
                                        that->mixer_data[TOPO_SYNTH1_MUTE][channel]=*Level;

                                        if(channel<=1 && that->hw->cb.mixer_controls[MIXER_SYNTH].pgm_name[0]==0)
                                        {
                                         if(*Level)
                                           kx_midi_set_volume(that->hw,channel,0x80000000);
                                         else
                                           kx_midi_set_volume(that->hw,channel,that->mixer_data[TOPO_SYNTH1_VOLUME][channel]);
                                        }
                                        else
                                        {
                                           if(channel<=1 && that->hw->cb.mixer_controls[MIXER_SYNTH].pgm_name[0])
                                           {
                                            if(*Level)
                                              kx_set_volume(that->hw,
                                                  that->hw->cb.mixer_controls[MIXER_SYNTH].pgm_name,
                                                            (channel==0)?that->hw->cb.mixer_controls[MIXER_SYNTH].reg_left:
                                                            that->hw->cb.mixer_controls[MIXER_SYNTH].reg_right,0x80000000);
                                            else
                                              kx_set_volume(that->hw,
                                                  that->hw->cb.mixer_controls[MIXER_SYNTH].pgm_name,
                                                            (channel==0)?that->hw->cb.mixer_controls[MIXER_SYNTH].reg_left:
                                                            that->hw->cb.mixer_controls[MIXER_SYNTH].reg_right,
                                                            that->mixer_data[TOPO_SYNTH1_VOLUME][channel],
                                                            that->hw->cb.mixer_controls[MIXER_SYNTH].max_vol);
                                           }
                                        }
                                    break;
                                  default:
                                    debug(DWDM,"!!! set MUTE unknown node=%d\n",PropertyRequest->Node);
                                    break;
                                }
                                PropertyRequest->ValueSize = sizeof(LONG);
                                ntStatus = STATUS_SUCCESS;
                    }  else debug(DWDM,"!!! bad id in topo::mute\n");
                }  else debug(DWDM,"!!! bad value size in topo::mute\n");
            }  else debug(DWDM,"!!! bad inst size in topo::mute\n");

        }
         else 
        {
            debug(DWDM,"!!! Unsupported Verb for MUTE\n");
        }
    }
    else debug(DWDM,"!!! bad node(=-1) in topo::mute\n");

    if(!NT_SUCCESS(ntStatus))
      debug(DWDM,"!! NOT OK in MUTE\n");

    return ntStatus;
}

#pragma code_seg("PAGE")
NTSTATUS CMiniportTopology::PropertyPrivate(IN PPCPROPERTY_REQUEST PropertyRequest)
{
 PAGED_CODE();
 CMiniportTopology *that=(CMiniportTopology *)(IMiniportTopology *)PropertyRequest->MajorTarget;
 if(that)
 {
  if(that->magic==TOPOLOGY_MAGIC)
   return process_property(((CAdapterCommon *)that->AdapterCommon),that->hw,PropertyRequest,TOPOLOGY_MAGIC);
 }
 else
 {
    debug(DWDM,"!!! Topology majortarget=0! in topo::private\n");
    return STATUS_INVALID_PARAMETER;
 }
 return STATUS_INVALID_PARAMETER;
}

#pragma code_seg()
NTSTATUS CMiniportTopology::EventHandler
(
    IN      PPCEVENT_REQUEST      EventRequest
)
{
    ASSERT(EventRequest);

    // The major target is the object pointer to the topology miniport.
    CMiniportTopology *that =
        (CMiniportTopology *)(PMINIPORTTOPOLOGY(EventRequest->MajorTarget));

    if(!that)
    {
     debug(DWDM,"!!! Topology majortarget=0! in EventHandler\n");
     return STATUS_INVALID_PARAMETER;
    }

    if(that->magic!=TOPOLOGY_MAGIC)
    {
     debug(DWDM,"!!! Bad topology magic!\n");
     return STATUS_INVALID_PARAMETER;
    }

    // Validate the node.
    if (EventRequest->Node != TOPO_MASTER_VOLUME && EventRequest->Node != TOPO_MASTER_MUTE)
    {
        debug(DWDM,"!!! Bad event in Topology::EventHandler\n");
        return STATUS_INVALID_PARAMETER;
    }

    // What is to do?
    switch (EventRequest->Verb)
    {
        // Do we support event handling?!?
        case PCEVENT_VERB_SUPPORT:
            // debug(DWDM,"BasicSupport Query for Event.\n");
            break;

        // We should add the event now!
        case PCEVENT_VERB_ADD:
            // debug(DWDM,"Adding Event\n");

            // If we have the interface and EventEntry is defined ...
            if ((EventRequest->EventEntry) && (that->PortEvents))
            {
                that->PortEvents->AddEventToEventList (EventRequest->EventEntry);
            }
            else
            {
                return STATUS_UNSUCCESSFUL;
            }
            break;

        case PCEVENT_VERB_REMOVE:
            // debug(DWDM,"Removing event\n");
            // We cannot remove the event but we can stop generating the
            // events. However, it also doesn't hurt to always generate them ...
            break;

        default:
            debug(DWDM,"!!! unknown command\n");
            return STATUS_INVALID_PARAMETER;
    }

    return STATUS_SUCCESS;
}

#pragma code_seg()
STDMETHODIMP_(void) CMiniportTopology::ServiceEvent (int mask)
{
    //
    // Generate an event for the master volume (as an example)
    //
    // debug(DWDM,"CMiniportTopology::ServiceEvent\n");
    if (PortEvents)
    {
        PortEvents->GenerateEventList (NULL, KSEVENT_CONTROL_CHANGE,
                                         FALSE, ULONG(-1), TRUE,
                                         TOPO_MASTER_VOLUME);
    }
}

#pragma code_seg()
STDMETHODIMP_(void) CMiniportTopology::PowerChangeNotify(IN      POWER_STATE     NewState)
{
    debug(DWDM,"[CMiniportTopology::PowerChangeState] - do nothing\n");
    PowerState = NewState.DeviceState;
}
           

#if 0
#pragma code_seg("PAGE")
NTSTATUS CMiniportTopology::GetPinName(  __in    PIRP _pIrp,__in    PKSP_PIN _pKsPPin,__out   PVOID _pvData)
{
    PAGED_CODE();
    NTSTATUS status = STATUS_NOT_SUPPORTED;

    ULONG   ulBufferLen = IoGetCurrentIrpStackLocation(_pIrp)->Parameters.DeviceIoControl.OutputBufferLength;
    // Determine if this is just a query for the length of the
    // buffer needed, or a query for the actual data.
    // TestName is defined as a constant to show an arbitary name 
    // for demo purposes. You might want to dynamically change the
    // pin name when it’s needed.
    #define TestName L"My test pin name"
    if (!ulBufferLen)
    {
        // Return just the size of the string needed.
        _pIrp->IoStatus.Information = sizeof(TestName);
        status = STATUS_BUFFER_OVERFLOW;
    }
    else
    {
        RtlZeroMemory(_pvData, ulBufferLen);
        RtlCopyMemory((PVOID)_pvData, (PVOID)TestName, sizeof(TestName));
        _pIrp->IoStatus.Information = sizeof(TestName);
        status = STATUS_SUCCESS;

   }

   // 3551: Windows 7 calls this FIXME
   // debug(DWDM,"CMiniportTopology::GetPinName called\n");

   return status;
}
#endif
