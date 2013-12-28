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

#include "wdm/tbl_uart.h"

#pragma code_seg("PAGE")
NTSTATUS create_uart
(
    OUT     PUNKNOWN *  Unknown,
    IN      REFCLSID,
    IN      PUNKNOWN    UnknownOuter    OPTIONAL,
    IN      POOL_TYPE   PoolType
)
{
    PAGED_CODE();

    ASSERT(Unknown);

    STD_CREATE_BODY_(CMiniportUart,Unknown,UnknownOuter,PoolType,PMINIPORTMIDI);
}

#pragma code_seg("PAGE")
STDMETHODIMP CMiniportUart::GetDescription(OUT PPCFILTER_DESCRIPTOR *  OutFilterDescriptor)
{
    PAGED_CODE();

    ASSERT(OutFilterDescriptor);

    if(uart_number==0)
    {
     MiniportPins[0].KsPinDescriptor.Name=&KX_UARTOUT_NAME;
     MiniportPins[1].KsPinDescriptor.Name=&KX_UARTOUT_NAME;
     MiniportPins[2].KsPinDescriptor.Name=&KX_UARTIN_NAME;
     MiniportPins[3].KsPinDescriptor.Name=&KX_UARTIN_NAME;
    }
    else
    {
     MiniportPins[0].KsPinDescriptor.Name=&KX_UARTOUT2_NAME;
     MiniportPins[1].KsPinDescriptor.Name=&KX_UARTOUT2_NAME;
     MiniportPins[2].KsPinDescriptor.Name=&KX_UARTIN2_NAME;
     MiniportPins[3].KsPinDescriptor.Name=&KX_UARTIN2_NAME;
    }

    *OutFilterDescriptor = &MiniportFilterDescriptor;

    return STATUS_SUCCESS;
}


#pragma code_seg("PAGE")
STDMETHODIMP CMiniportUart::NonDelegatingQueryInterface(REFIID  Interface,PVOID * Object)
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
    	debug(DWDM,"!!! - (UART) try to delegate to ref_iid unknown\n");
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
CMiniportUart::~CMiniportUart(void)
{
    PAGED_CODE();

    debug(DWDM,"[CMiniportUart::~CMiniportUart]\n");

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
        if((uart_number<MAX_MPU_DEVICES) && (uart_number>=0))
    	  ((CAdapterCommon *)AdapterCommon)->Uart[uart_number]=NULL;

	((CAdapterCommon *)AdapterCommon)->uart_count--;

        AdapterCommon->Release();
        AdapterCommon = NULL;
    }
}

#pragma code_seg("PAGE")
STDMETHODIMP CMiniportUart::Init(
				IN      PUNKNOWN        UnknownAdapter,
				IN      PRESOURCELIST   ResourceList,
                                IN      PPORTMIDI       Port_,
                                OUT     PSERVICEGROUP * ServiceGroup_)
{
    PAGED_CODE();

    ASSERT(ResourceList);
    ASSERT(Port_);
    ASSERT(ServiceGroup_);

    debug(DWDM,"[CMiniportUart::Init]\n");

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
        	debug(DWDM,"!!! Error creating ServiceGroup in CMiniUart::Init (%d)\n",ntStatus);
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
     uart_number=((CAdapterCommon *)AdapterCommon)->uart_count;
     ((CAdapterCommon *)AdapterCommon)->uart_count++;
     if(uart_number<MAX_MPU_DEVICES)
      ((CAdapterCommon *)AdapterCommon)->Uart[uart_number]=this;
    }

    if(!NT_SUCCESS(ntStatus))
     debug(DWDM,"!!! MiniportUart::Init failed\n");

    return ntStatus;
}

#pragma code_seg("PAGE")
STDMETHODIMP CMiniportUart::NewStream(
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

    debug(DWDM,"[CMiniportUart::NewStream]\n");
    NTSTATUS ntStatus = STATUS_SUCCESS;

    /*
    // 3537rc1 -- removed
    if(capture_streams == 0 && render_streams==0)
    {
	if(kx_mpu_reset(hw,uart_number,1)) // enable IRQs
	{
	   debug(DWDM,"!!! CMiniportUart::NewStream - Reset failed\n");
	   ntStatus=STATUS_UNSUCCESSFUL;
	} 
    }
    */
    if(NT_SUCCESS(ntStatus))
    {
              CMiniportMidiStreamUart *pStream =
                  new(PoolType,'sukX') CMiniportMidiStreamUart(OuterUnknown);

              if (pStream)
              {
                  pStream->instance=0;
                  pStream->hw=0;
                  pStream->inited=0;
		  pStream->sysex_pos=0;
		  pStream->sysex_state=SYSEX_NONE;

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
    }

    return ntStatus;
}

#pragma code_seg()
STDMETHODIMP_(void) CMiniportUart::Service(void)
{
}

#pragma code_seg("PAGE")
STDMETHODIMP CMiniportUart::DataRangeIntersection
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
STDMETHODIMP CMiniportMidiStreamUart::NonDelegatingQueryInterface(    REFIID  Interface,    PVOID * Object)
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
	debug(DWDM,"!!! - (UARTStream) try to delegate to ref_iid unknown\n");
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
STDMETHODIMP CMiniportMidiStreamUart::SetFormat(IN      PKSDATAFORMAT   Format)
{
    PAGED_CODE();

    ASSERT(Format);

    debug(DWDM,"!!! ignore CMiniportMidiStreamUart::SetFormat\n");

    return STATUS_SUCCESS;
}

#pragma code_seg("PAGE")
CMiniportMidiStreamUart::~CMiniportMidiStreamUart(void)
{
    PAGED_CODE();

    debug(DWDM,"[~CMiniportMidiStreamUart]\n");

    if(instance && hw)
    {
    	kx_mpu_read_buffer_close(hw,instance,Miniport->uart_number);
    	instance=0;
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
STDMETHODIMP CMiniportMidiStreamUart::SetState( IN      KSSTATE     NewState)
{
    PAGED_CODE();
    debug(DPROP,"[CMiniportMidiStreamUart::SetState] state=%d\n",NewState);
    state=NewState;

    return STATUS_SUCCESS;
}

#pragma code_seg("PAGE")
NTSTATUS CMiniportMidiStreamUart::Init(IN      CMiniportUart * pMiniport,BOOLEAN Capture_)
{
 PAGED_CODE();

 Miniport=pMiniport;
 Miniport->AddRef();

 hw=Miniport->hw;
 Capture=Capture_;

 if(Capture)
 {
    if(kx_mpu_read_buffer_open(hw,&instance,Miniport->uart_number)) // error
    {
     debug(DWDM,"!!! error opening MPU read buffer\n");
     instance=0;
     return STATUS_UNSUCCESSFUL;
    }
 } 
  else 
 {
  instance=0;

  unsigned long flags;
  kx_lock_acquire(hw,&hw->uartout_lock, &flags);
  hw->uart_out_tail[Miniport->uart_number]=hw->uart_out_head[Miniport->uart_number];
  kx_lock_release(hw,&hw->uartout_lock, &flags);
 }
 inited=1;

 return STATUS_SUCCESS;
}

#pragma code_seg()
STDMETHODIMP CMiniportMidiStreamUart::Write
(
    IN      PVOID       BufferAddress_,
    IN      ULONG       Length,
    OUT     PULONG      BytesWritten
)
{
 if(Miniport && !Capture && inited && Length && BufferAddress_)
 {
  int done=0;
  int prev_l=(int)Length;

  byte *p=(byte *)ExAllocatePoolWithTag(NonPagedPool,Length,'mdUA');
  byte *p_buff=p;
  if(p==0)
  {
   debug(DERR,"uart::write note - no more memory [%d]\n",Length);
   return STATUS_INSUFFICIENT_RESOURCES;
  }
  memcpy(p,BufferAddress_,Length);

  int port=Miniport->uart_number;

  while((int)Length>0)
  {
    int actual_len=0;

    if(kx_mpu_write_buffer(hw,port,p,
          (Length>MAX_UART_BUFFER)?(MAX_UART_BUFFER):Length,&actual_len)!=0)
    {
       done+=actual_len;
       p+=actual_len;

//       debug(DWDM,"kx: uart out timeout [sent: %d need: %d]\n",done,Length);

       Length-=actual_len;
       break;
    }

    done+=actual_len;
    p+=actual_len;
    Length-=actual_len;
  }

  ExFreePool(p_buff);

  (*BytesWritten)=done;

  if(done)
   return STATUS_SUCCESS;
  else
   return STATUS_IO_DEVICE_ERROR; // ? STATUS_DEVICE_BUSY;
 }
 debug(DWDM,"uart: invalid request!\n");
 return STATUS_INVALID_DEVICE_REQUEST; 
}

#pragma code_seg()
STDMETHODIMP CMiniportMidiStreamUart::Read
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
      while(Length)
      {
        // get rid of the current buffer, if necessary
        if((sysex_state==SYSEX_OVERFLOW) && (sysex_pos>0))
        {
        	//debug(DWDM,"-- flushing sysex buffer [%x]\n",sysex_buffer[0]);
     		*p=sysex_buffer[0];
     		sysex_pos--;

     		if(*p==0xf7)
     		{
     		 if(sysex_pos==0)
     		 {
     		  //debug(DWDM,"-- sysex buffer flushed ok\n");
     		  sysex_state=SYSEX_NONE;
     		 }
     		 else
     		  debug(DWDM,"!! sysex buffer flushed, but pos=%d\n",sysex_pos);
     		}

     		if(sysex_pos>0) // shift
     		 memcpy(&sysex_buffer[0],&sysex_buffer[1],sysex_pos);

     	      	p++;
     	      	cnt++;
     	      	Length--;
     	      	continue;
        }

        byte val;
      	if(kx_mpu_read_buffer(hw,instance,&val,Miniport->uart_number)==-1)
      	 break;

      	switch(sysex_state)
      	{
      		case SYSEX_NONE:
			if(val==0xf0) // start SysEx
		      	{
		      		sysex_state=SYSEX_PARSING;
		      		sysex_buffer[0]=val;
		      		sysex_pos=1;
		      		//debug(DWDM,"-- sysex started\n");
		      		break;
		      	}
		      	if(val==0xf7) // end SysEx
		      	{
		      		debug(DWDM,"!!! Invalid sysex sequence 2\n");
		      	}
			*p=val;
		      	p++;
		      	cnt++;
		      	Length--;
		      	break;
		case SYSEX_PARSING:
			if(val!=0xf7) // continue SysEx
			{
				sysex_buffer[sysex_pos]=val;
				sysex_pos++;
				if(sysex_pos>=SYSEX_BUFFER_SIZE)
				{
					debug(DWDM,"!!! SysEx too long\n");
					sysex_state=SYSEX_OVERFLOW;
					break;
				}
			}
			else	// 0xf7: finish SysEx
			{
				sysex_buffer[sysex_pos]=val;
				sysex_pos++;
				
				// parse SysEx here...
				//debug(DWDM,"-- got sysex -- parse it now\n");
				//for(int i=0;i<sysex_pos;i++)
				// debug(DNONE,"[%x] ",sysex_buffer[i]);
				//debug(DNONE,"\n");

				if( (memcmp(sysex_buffer,KX_SYSEX_REMOTE_INITIAL,KX_SYSEX_REMOTE_INITIAL_SIZE)==0) &&
				    (sysex_buffer[KX_SYSEX_REMOTE_SIZE-1]==0xf7))
				{
				  // discard Remote IR data
				  sysex_state=SYSEX_NONE;
				  sysex_pos=0;
				  break;
				}
				else
				if( (memcmp(sysex_buffer,KX_SYSEX_REMOTE2_INITIAL,KX_SYSEX_REMOTE2_INITIAL_SIZE)==0) &&
				    (sysex_buffer[KX_SYSEX_REMOTE2_SIZE-1]==0xf7))
				{
				  // discard Remote IR data [a2ex on-board]
				  sysex_state=SYSEX_NONE;
				  sysex_pos=0;
				  break;
				}

                                // if we don't need it --
                                sysex_state=SYSEX_OVERFLOW;
                                //debug(DLIB,"-- flushing unrecognized sysex buffer\n");
                                break;
			}
			break;
		case SYSEX_OVERFLOW:
			if(sysex_pos!=0)
			 debug(DWDM,"!!! internal error parsing SysEx 1\n");

			*p=val;
		      	p++;
		      	cnt++;
		      	Length--;

			if(val==0xf7) // overflowed SysEx finished
			{
				sysex_state=SYSEX_NONE;
				//debug(DWDM,"-- overflowed sysex finished\n");
			}
			break;
      	}
      }
      (*BytesRead)=cnt;

    } else return STATUS_INVALID_DEVICE_REQUEST;

    return STATUS_SUCCESS;
}

#pragma code_seg()
STDMETHODIMP_(void) CMiniportUart::PowerChangeNotify(IN      POWER_STATE     NewState)
{
    debug(DWDM,"[CMiniportUart::PowerChangeState] - do nothing\n");
    PowerState = NewState.DeviceState;
}
