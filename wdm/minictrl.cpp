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

#include "wdm/tbl_ctrl.h"

#pragma code_seg("PAGE")
NTSTATUS create_ctrl
(
    OUT     PUNKNOWN *  Unknown,
    IN      REFCLSID,
    IN      PUNKNOWN    UnknownOuter    OPTIONAL,
    IN      POOL_TYPE   PoolType
)
{
    PAGED_CODE();

    ASSERT(Unknown);

    STD_CREATE_BODY_(CMiniportCtrl,Unknown,UnknownOuter,PoolType,PMINIPORTMIDI);
}

#pragma code_seg("PAGE")
STDMETHODIMP CMiniportCtrl::GetDescription(OUT PPCFILTER_DESCRIPTOR *  OutFilterDescriptor)
{
    PAGED_CODE();

    ASSERT(OutFilterDescriptor);

    *OutFilterDescriptor = &MiniportFilterDescriptor;

    return STATUS_SUCCESS;
}


#pragma code_seg("PAGE")
STDMETHODIMP CMiniportCtrl::NonDelegatingQueryInterface(REFIID  Interface,PVOID * Object)
{
    PAGED_CODE();

    ASSERT(Object);

    if (IsEqualGUIDAligned(Interface,IID_IUnknown))
    {
        *Object = PVOID(PUNKNOWN(PMINIPORTMIDI(this)));
    }
    else
    if (IsEqualGUIDAligned(Interface,IID_IMiniport))
    {
        *Object = PVOID(PMINIPORT(this));
    }
    else
    if (IsEqualGUIDAligned(Interface,IID_IMiniportMidi))
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
        debug(DWDM,"!!! - (CTRL) try to delegate to ref_iid unknown\n");
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
        //
        // We reference the interface for the caller.
        //
        PUNKNOWN(*Object)->AddRef();
        return STATUS_SUCCESS;
    }

    return STATUS_INVALID_PARAMETER;
}

#pragma code_seg("PAGE")
CMiniportCtrl::~CMiniportCtrl(void)
{
    PAGED_CODE();

    debug(DWDM,"[CMiniportCtrl::~CMiniportCtrl]\n");

    if (ServiceGroup)
    {
        ServiceGroup->Release();
        ServiceGroup = NULL;
    }
    if(Port)
    {   
        Port->Release();
        Port = NULL;
    }
    if (AdapterCommon)
    {
    ((CAdapterCommon *)AdapterCommon)->Ctrl=NULL;

        AdapterCommon->Release();
        AdapterCommon = NULL;
    }
}

#pragma code_seg("PAGE")
STDMETHODIMP CMiniportCtrl::Init(
                IN      PUNKNOWN        UnknownAdapter,
                IN      PRESOURCELIST   ResourceList,
                                IN      PPORTMIDI       Port_,
                                OUT     PSERVICEGROUP * ServiceGroup_)
{
    PAGED_CODE();

    ASSERT(ResourceList);
    ASSERT(Port_);
    ASSERT(ServiceGroup_);

    debug(DWDM,"[CMiniportCtrl::Init]\n");

    Port = Port_;
    Port->AddRef();

    PowerState=PowerDeviceD0;

    capture_streams=0;
    render_streams=0;
    ServiceGroup=NULL;
    AdapterCommon=NULL;

    NTSTATUS ntStatus =
        UnknownAdapter->QueryInterface
        (
            IID_IAdapterCommon,
            (PVOID *) &AdapterCommon
        );

    if(NT_SUCCESS(ntStatus))
    {
        ntStatus = PcNewServiceGroup(&ServiceGroup,NULL);
        if (NT_SUCCESS(ntStatus) && !ServiceGroup)   //  keep any error
        {
            debug(DWDM,"!!! Error creating ServiceGroup in CMiniCtrl::Init (%d)\n",ntStatus);
        }

        if (NT_SUCCESS(ntStatus))
        {
            *ServiceGroup_ = ServiceGroup;
            ServiceGroup->AddRef();
        Port->RegisterServiceGroup(ServiceGroup);
        }
    }

    if( !NT_SUCCESS(ntStatus) )
    {
        if (ServiceGroup)
        {
            ServiceGroup->Release();
            ServiceGroup = NULL;
        }

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
     ((CAdapterCommon *)AdapterCommon)->Ctrl=this;

     kx_spin_lock_init(hw,&lock,"control"); 
     ctrl_pos=0;
    }

    if(!NT_SUCCESS(ntStatus))
     debug(DWDM,"!!! MiniportCtrl::Init failed\n");

    return ntStatus;
}

#pragma code_seg("PAGE")
STDMETHODIMP CMiniportCtrl::NewStream(
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

    debug(DWDM,"[CMiniportCtrl::NewStream]\n");
    NTSTATUS ntStatus = STATUS_SUCCESS;

    *ServiceGroup_ = NULL;

    CMiniportCtrlStream *pStream =
        new(PoolType,'scXk') CMiniportCtrlStream(OuterUnknown);

    if (pStream)
    {
        pStream->instance0=0;
        pStream->instance1=0;
        pStream->hw=0;
        pStream->inited=0;

        pStream->AddRef();

        ntStatus =
            pStream->Init(this,Capture);

        if (NT_SUCCESS(ntStatus))
        {
            *Stream = PMINIPORTMIDISTREAM(pStream);
            (*Stream)->AddRef();

            if (Capture)
            {
                  capture_streams++;
                *ServiceGroup_ = ServiceGroup;
                (*ServiceGroup_)->AddRef();
            }
            else
            {
                  render_streams++;
                *ServiceGroup_ = NULL;
            }
        }

        pStream->Release();
    }
    else
    {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }

    return ntStatus;
}

#pragma code_seg()
STDMETHODIMP_(void) CMiniportCtrl::Service(void)
{
}

#pragma code_seg("PAGE")
STDMETHODIMP CMiniportCtrl::DataRangeIntersection
    (   IN      ULONG           PinId
    ,   IN      PKSDATARANGE    DataRange
    ,   IN      PKSDATARANGE    MatchingDataRange
    ,   IN      ULONG           OutputBufferLength
    ,   OUT     PVOID           ResultantFormat
    ,   OUT     PULONG          ResultantFormatLength
    )
    {
        PAGED_CODE();
        return STATUS_NOT_IMPLEMENTED;
    }

// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------

#pragma code_seg("PAGE")
STDMETHODIMP CMiniportCtrlStream::NonDelegatingQueryInterface(    REFIID  Interface,    PVOID * Object)
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
    debug(DWDM,"!!! - (CtrlStream) try to delegate to ref_iid unknown\n");
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
STDMETHODIMP CMiniportCtrlStream::SetFormat(IN      PKSDATAFORMAT   Format)
{
    PAGED_CODE();

    ASSERT(Format);

    debug(DWDM,"!!! ignore CMiniportCtrlStream::SetFormat\n");

    return STATUS_SUCCESS;
}

#pragma code_seg("PAGE")
CMiniportCtrlStream::~CMiniportCtrlStream(void)
{
    PAGED_CODE();

    debug(DWDM,"[~CMiniportCtrlStream]\n");

    if(hw)
    {
      if(instance0)
      {
        kx_mpu_read_buffer_close(hw,instance0,0);
        instance0=0;
      }
      if(instance1)
      {
        kx_mpu_read_buffer_close(hw,instance1,1);
        instance1=0;
      }
      if(!(hw->sys_timer.status&TIMER_UNINSTALLED))
      {
       kx_timer_disable(hw,&hw->sys_timer);
       kx_timer_uninstall(hw,&hw->sys_timer);
      }
    }

    if(Miniport)
    {
         if(Capture)
         {
          if(inited==1)
            Miniport->capture_streams--;
         }
         else
         {
          if(inited==1)
            Miniport->render_streams--;
         }
         inited=0;

         Miniport->Release();
         Miniport=NULL;
    }
}

#pragma code_seg("PAGE")
STDMETHODIMP CMiniportCtrlStream::SetState( IN      KSSTATE     NewState)
{
    PAGED_CODE();
    debug(DPROP,"[CMiniportCtrlStream::SetState] state=%d\n",NewState);
    state=NewState;

    return STATUS_SUCCESS;
}

#pragma code_seg("PAGE")
NTSTATUS CMiniportCtrlStream::Init(IN CMiniportCtrl * pMiniport,BOOLEAN Capture_)
{
 PAGED_CODE();

 Miniport=pMiniport;
 Miniport->AddRef();

 hw=Miniport->hw;
 Capture=Capture_;

 if(Capture)
 {
    if((!hw->is_10k2) || (hw->is_a2ex)) // (not audigy1,2) or (audigy2 plat ex)
    {
        // 3537rc1 -- removed
        /*
        if(kx_mpu_reset(hw,0,1)) // enable IRQs
         debug(DWDM,"!! error resetting MPU device for kX Control\n");
        */
        if(kx_mpu_read_buffer_open(hw,&instance0,0)) // error
        {
         debug(DWDM,"!!! error opening MPU read buffer\n");
         instance0=0;
         return STATUS_UNSUCCESSFUL;
        }
    }
    if(hw->is_10k2) // a1/a2 or a2ex
    {
        // 3537rc1 -- removed
        /*
        if(kx_mpu_reset(hw,1,1)) // enable IRQs
         debug(DWDM,"!! error resetting MPU device for kX Control\n");
        */

        if(kx_mpu_read_buffer_open(hw,&instance1,1)) // error
        {
         debug(DWDM,"!!! error opening secondary MPU read buffer\n");
         instance1=0;
         return STATUS_UNSUCCESSFUL;
        }
    }
    if(hw->sys_timer.status&TIMER_UNINSTALLED)
    {
     kx_timer_install(hw,&hw->sys_timer,hw->card_frequency); // once per second
     kx_timer_enable(hw,&hw->sys_timer);
    }
 }
  else 
 {
  instance0=0;
  instance1=0;
 }
 inited=1;

 return STATUS_SUCCESS;
}

// pragma is required, since we call send_message() here
#pragma code_seg()
STDMETHODIMP CMiniportCtrlStream::Write
(
    IN      PVOID       BufferAddress,
    IN      ULONG       Length,
    OUT     PULONG      BytesWritten
)
{
 if(Miniport && !Capture && inited)
 {
  if(!BufferAddress) 
    Length=0;

  if(Length)
  {
    hw->cb.send_message(hw->cb.call_with,Length,BufferAddress);
  }
  (*BytesWritten)=Length;
 }
 return STATUS_SUCCESS;
}

#pragma code_seg()
STDMETHODIMP CMiniportCtrlStream::Read
(
    IN      PVOID   BufferAddress,
    IN      ULONG   Length,
    OUT     PULONG  BytesRead
)
{
    ASSERT(BufferAddress);
    ASSERT(BytesRead);

    (*BytesRead) = 0;
    if(Capture && inited && Length)
    {
      byte *p=(byte *)BufferAddress;
      int cnt=0;
      if(Miniport->ctrl_pos>0)
      {
            unsigned long flags;
            kx_lock_acquire(hw,&Miniport->lock, &flags);

            int amount;
            if((int)Length<Miniport->ctrl_pos)
             amount=Length;
            else
             amount=Miniport->ctrl_pos;

            memcpy(p,&Miniport->ctrl_buffer[0],amount);

            if(amount!=Miniport->ctrl_pos)
            {
             memcpy(&Miniport->ctrl_buffer[0],&Miniport->ctrl_buffer[amount],Miniport->ctrl_pos-amount);
             Miniport->ctrl_pos-=amount;
            }
            else
             Miniport->ctrl_pos=0;

            kx_lock_release(hw,&Miniport->lock,&flags);

                cnt+=amount;
                Length-=amount;
                p+=amount;
      }

      while(Length)
      {
        int have_data=0;

        byte val=0xfe;
        if(instance0)
        {
         if(kx_mpu_read_buffer(hw,instance0,&val,0)!=-1)
          have_data=1;
        }
        if(!have_data)
         if(instance1) // to avoid double 'mpu_read'...
         {
          if(kx_mpu_read_buffer(hw,instance1,&val,1)!=-1)
           have_data=1;
         }

        if(!have_data || (hw->ext_flags&KX_HW_NO_UART_ROUTING))
         break;

        *p=val;
        p++;
        Length--;
        cnt++;
     } // while

     (*BytesRead)=cnt;

    } else return STATUS_INVALID_DEVICE_REQUEST;

    return STATUS_SUCCESS;
}

#pragma code_seg()
STDMETHODIMP_(void) CMiniportCtrl::PowerChangeNotify(IN      POWER_STATE     NewState)
{
    debug(DWDM,"[CMiniportCtrl::PowerChangeState] - do nothing\n");
    PowerState = NewState.DeviceState;
}

#pragma code_seg()
void CMiniportCtrl::output_buffer(int size,byte *buff)
{
    // note: input buffer might be stored in Paged Pool due to several reasons, added in 3551
    int target=-1;

    unsigned long flags=0;
    kx_lock_acquire(hw,&lock, &flags);
    
    if(size>0 && buff)
    {
        if(ctrl_pos+size<MAX_CTRL_BUFFER_SIZE)
        {
            target=ctrl_pos;
            // was: memcpy(&ctrl_buffer[ctrl_pos],buff,size);
            ctrl_pos+=size;
        } else debug(DWDM,"!! ctrl buffer overflow [%d; %d]\n",size,ctrl_pos);
    }

    kx_lock_release(hw,&lock,&flags);

    if(target!=-1)
        memcpy(&ctrl_buffer[target],buff,size);
}
