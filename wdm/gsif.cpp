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


// Product Information for GSIF(R), Version 1.70
// (C)1998 NemeSys Music Technology, Inc.
// GSIF 2
// (C) Copyright 1998-2003 TASCAM, Inc.

#include "common.h"
#include "gsif/gsif2.h"
#include "gsif/kxgsif.h"

// don't change:
#define DEF_GSIF_MODE 		(KXASIO_METHOD_OLD|KXASIO_METHOD_USE_HWTIMER)
#define DEF_GSIF_BUFFER_SIZE	0x2000

// change:
#define DEF_LATENCY	256		// 5 ms

NTSTATUS GetInterface(PGSIF_GETINTERFACE pAddInterface, CAdapterCommon *adapter);

void close_gsif(CAdapterCommon *adapter)
{
   debug(DGSIF,"close gsif\n");

   // DisableGSIFInterface(DeviceObject):
   if(adapter->gsif_device.Buffer)
   {
    HANDLE hReg = NULL;
    UNICODE_STRING Entry;
    DWORD          dwValue;

    if(IoSetDeviceInterfaceState(&adapter->gsif_device, FALSE)!=STATUS_SUCCESS)
     debug(DWDM,"gsif: Failed to set interface state\n");

    if(IoOpenDeviceInterfaceRegistryKey(&adapter->gsif_device,
                                    KEY_ALL_ACCESS,
                                    &hReg)==STATUS_SUCCESS)
    {
       dwValue = 0;
       RtlInitUnicodeString( &Entry, REG_GETINTERFACECALLBACK );
       ZwSetValueKey(hReg, &Entry, 0, REG_DWORD, (PVOID) &dwValue, sizeof(DWORD));

       RtlInitUnicodeString( &Entry, REG_DEVICEEXTENSION );
       ZwSetValueKey(hReg, &Entry, 0, REG_DWORD, (PVOID) &dwValue, sizeof(DWORD));

       ZwClose(hReg);
    }
     else
      debug(DWDM,"gsif: Error opening registry key\n");

    // free symbolic link
    UNICODE_STRING        LinkName;

    unsigned short buf1[30]; memcpy(buf1,L"\\DosDevices\\kxgsif_",19*2);
    char port_n[10]; sprintf(port_n,"%x",adapter->hw->port&0xffff);
    for(int i=0;i<5;i++)
     buf1[19+i]=port_n[i];

    debug(DGSIF,"deleting symbolic link: %ws\n",buf1);
    
    RtlInitUnicodeString(&LinkName, buf1);
    if(IoDeleteSymbolicLink(&LinkName)!=STATUS_SUCCESS)
     debug(DWDM,"gsif: Error deleting symbolic link\n");

    // free device name
    RtlFreeUnicodeString(&adapter->gsif_device);
    adapter->gsif_device.Buffer=0;
   }

   if(adapter->gsif_interface)
   {
    delete adapter->gsif_interface;
    adapter->gsif_interface=0;
   }

   debug(DWDM,"kxgsif: closed\n");
}

static NTSTATUS create_gsif_interface(PUNKNOWN * pInterface)
{
   STD_CREATE_BODY_WITH_TAG(CGSIFInterface, pInterface, NULL, NonPagedPool, 'sgXk'); //  DmMp
   return STATUS_SUCCESS;
}


void register_gsif(CAdapterCommon *adapter)
{
  debug(DGSIF,"registering interface\n");

  // DeviceObject is already created (adapter->DeviceObject)
  // CreateDevice(), AttachDeviceToDeviceStack() ...

  // RegisterGSIFDriver():
  NTSTATUS st=IoRegisterDeviceInterface(adapter->physical_device_object,
                                 (LPGUID)&GSIFCLASS_INTERFACE,
                                 NULL,
                                 &adapter->gsif_device);
  if((st == STATUS_SUCCESS) || (st==STATUS_OBJECT_NAME_EXISTS))
  {
      if(st==STATUS_OBJECT_NAME_EXISTS)
        debug(DGSIF,"Not an error - interface already exists\n");

      HANDLE hReg = NULL;
      UNICODE_STRING Entry;
      DWORD          dwValue;

      debug(DGSIF,"LinkName = %ws\n",adapter->gsif_device.Buffer);
      if(IoSetDeviceInterfaceState(&adapter->gsif_device, TRUE)!=STATUS_SUCCESS)
       debug(DWDM,"gsif: Error setting interface state\n");

      if(IoOpenDeviceInterfaceRegistryKey(&adapter->gsif_device,
                                       KEY_ALL_ACCESS,
                                       &hReg)==STATUS_SUCCESS)
      {

    #if defined(_WIN64)
          //
    	  // 1/07/2007
    	  // New for v2.1 of GSIF is the support of 64-bit address. Make
    	  // sure this code get compile into your driver if you are building
    	  // a 64-bit driver.
    	  //
          unsigned long ulAddrLo, ulAddrHi;

    	  ulAddrLo = (unsigned long) ( (DWORD_PTR)GetInterface & 0xFFFFFFFF );
    	  ulAddrHi = (unsigned long) ( (DWORD_PTR)GetInterface >> 32 );

    	  RtlInitUnicodeString( &Entry, REG_GETINTERFACECALLBACK );
          ZwSetValueKey(hReg, &Entry, 0, REG_DWORD, (PVOID) &ulAddrLo, sizeof(ulAddrLo));
          RtlInitUnicodeString( &Entry, REG_GETINTERFACECALLBACK_HIGH );
          ZwSetValueKey(hReg, &Entry, 0, REG_DWORD, (PVOID) &ulAddrHi, sizeof(ulAddrHi));

      	  ulAddrLo = (unsigned long) ( (DWORD_PTR)adapter & 0xFFFFFFFF );
    	  ulAddrHi = (unsigned long) ( (DWORD_PTR)adapter >> 32 );

          RtlInitUnicodeString( &Entry, REG_DEVICEEXTENSION );
          ZwSetValueKey(hReg, &Entry, 0, REG_DWORD, (PVOID) &ulAddrLo, sizeof(ulAddrLo));
          RtlInitUnicodeString( &Entry, REG_DEVICEEXTENSION_HIGH );
          ZwSetValueKey(hReg, &Entry, 0, REG_DWORD, (PVOID) &ulAddrHi, sizeof(ulAddrHi));
     #else
          RtlInitUnicodeString( &Entry, REG_GETINTERFACECALLBACK );

          dwValue = (DWORD) GetInterface;
          ZwSetValueKey(hReg, &Entry, 0, REG_DWORD, (PVOID) &dwValue, sizeof(DWORD));

          RtlInitUnicodeString( &Entry, REG_DEVICEEXTENSION );
          dwValue = (DWORD) adapter;
          ZwSetValueKey(hReg, &Entry, 0, REG_DWORD, (PVOID) &dwValue, sizeof(DWORD));

     #endif

      ZwClose(hReg);
      } else debug(DWDM,"gsif: Error opening registry key\n");
   } 
    else 
    {
    debug(DWDM,"gsif: register device interface failed: %x\n",st);
    return;
    }

   PUNKNOWN pInterface;
   if(create_gsif_interface(&pInterface)!=STATUS_SUCCESS)
   {
    debug(DWDM,"gsif: error instantiating GSIF interface\n");
    return;
   }
   else
    debug(DGSIF,"interface instantiated ok\n");

   adapter->gsif_interface = (CGSIFInterface *)(PGSIFINTERFACE) pInterface;

   adapter->gsif_interface->init(adapter);

   // the rest of 'AddDevice':
   UNICODE_STRING        LinkName;

   unsigned short buf1[30]; memcpy(buf1,L"\\DosDevices\\kxgsif_",19*2);
   char port_n[10]; sprintf(port_n,"%x",adapter->hw->port&0xffff);
   for(int i=0;i<5;i++)
    buf1[19+i]=port_n[i];

   debug(DGSIF,"Creating symbolic link: %ws\n",buf1);
   
   RtlInitUnicodeString(&LinkName, buf1);
   if(IoCreateSymbolicLink(&LinkName, &adapter->gsif_device)!=STATUS_SUCCESS)
    debug(DWDM,"gsif: error creating symbolic link\n");
   else
   {
    debug(DGSIF,"symbolic link created\n");
    debug(DWDM,"kxgsif: initialized\n");
   }
}

NTSTATUS GetInterface(PGSIF_GETINTERFACE pAddInterface, CAdapterCommon *adapter)
{
    debug(DGSIF,"GetInterface()\n");

    strcpy(pAddInterface->szSysName, "kx.sys");

    pAddInterface->nNumOfCards = 1;
    pAddInterface->pInterface  = adapter->gsif_interface;

    // also: dwBaseCardId (set by NStation);

    return STATUS_SUCCESS;
}

void CGSIFInterface::init(CAdapterCommon *adapter_)
{
    // gsif-specific init
    adapter = adapter_;

    fnGSIFRequest=NULL;
    fnGSIFIsr=NULL;
    pContext=NULL;

    // asio init
    asio_hw=adapter_->hw;
    that_=this;
    asio_magic=GSIF_MAGIC;

    // gsif-specific init:
    buffer_size=0;
    buffer_position=0;
    gsif_latency=DEF_LATENCY;

    // asio init:
    asio_latency=gsif_latency;
    asio_notification_user=0;
    asio_notification_mdl=0;
    asio_bps=0;

    debug(DGSIF,"init ASIO/GSIF emulation layer\n");
}

#pragma code_seg("PAGE")
/*****************************************************************************
 * CGSIFInterface::NonDelegatingQueryInterface()
 *****************************************************************************
 * Destructor for miniport object.  Let go of the port reference.
 */
STDMETHODIMP
CGSIFInterface::NonDelegatingQueryInterface(IN  REFIID  Interface,
                                            OUT PVOID * Object)
{
    PAGED_CODE();

    ASSERT(Object);

    if (IsEqualGUIDAligned(Interface, IID_IUnknown))
    {
        *Object = PVOID(PUNKNOWN(this));
    }
    else if (IsEqualGUIDAligned(Interface, IID_IGSIFInterface))
    {
        *Object = PVOID(PGSIFINTERFACE(this));
    }
    else
    {
        *Object = NULL;
    }

    if (*Object)
    {
        PUNKNOWN(*Object)->AddRef();
        return STATUS_SUCCESS;
    }

    return STATUS_INVALID_PARAMETER;
}

#pragma code_seg()

/*****************************************************************************
 * CGSIFInterface::~CGSIFInterface()
 *****************************************************************************
 * Destructor for miniport object.  Let go of the port reference.
 */
CGSIFInterface::~CGSIFInterface()
{
    if(is_asio)
    {
     if(asio_hw)
     {
        debug(DGSIF,"closing ASIO emulation layer\n");
        asio_close();
     }
     asio_hw=0;
     is_asio=0;
    }
}

/*****************************************************************************
 * CGSIFInterface::ProcessCommand()
 *****************************************************************************
 *  This is the general command processing routine. These commands are being
 *  sent from the Giga-compatible application to the hardware driver.
 *
 */
STDMETHODIMP CGSIFInterface::ProcessCommand(IN DWORD dwCommand,
                                            IN DWORD dwCardId,
                                            IN PVOID pContext)
{
   NTSTATUS status = GSIF_NOERROR;

   switch (dwCommand)
   {
      case GSIF_API_GETVERSION:
      {
         //
         // Description: Get the version information of driver/hardware.
         //              At least fill in the major/minor fields. The other
         //              two (dwRevision and dwCardIndetifier) are optional.
         //
         //  pContext = PGSIF_VERSION structure
         //
         PGSIF_VERSION pVer = (PGSIF_VERSION)pContext;
         debug(DGSIF,"kxgsif: GSIF_GETVERSION\n");

         pVer->dwMajor      = 2;
         pVer->dwMinor      = KX_ASIO_VERSION;
         pVer->dwRevision   = 0;
         pVer->dwGSVERSION  = GSIF2_VER;
         pVer->dwCardIdentifier = adapter->hw->port;
         break;
      }

      case GSIF_API_GETHWCAPS:
      {
         //
         // Description: Get the capabilities of the driver/hardware.
         //              Please fill in all the fields.
         //
         //  pContext = PGSIF_HWCAPS structure
         //
         PGSIF_HWCAPSEX pCaps = (PGSIF_HWCAPSEX)pContext;
         debug(DGSIF,"kxgsif: GSIF_GETHWCAPS\n");

         memset(pCaps, 0, sizeof(GSIF_HWCAPSEX));
         sprintf(pCaps->szProductName, "kX GSIF - %s",adapter->hw->card_name);

         pCaps->dwNumOfOuts    = MAX_ASIO_OUTPUTS;
         pCaps->dwNumOfIns     = 0;
         switch(adapter->hw->card_frequency)
         {
            default: debug(DLIB,"!! invalid card frequency\n"); /* fall through */
            case 48000: pCaps->dwSampleRates  = FS_48000; break;
            case 44100: pCaps->dwSampleRates  = FS_44100; break;
         }
         pCaps->dwSampleWidths = SW_16;
         pCaps->dwHWCommFlags  = ((DEF_GSIF_MODE&KXASIO_METHOD_OLD)?HC_SUPPLYBUFFERS:0)/*|HC_MULTICLIENT*/;

         pCaps->dwMMEDriverId = 0;

         for(dword i = 0; i<pCaps->dwNumOfOuts; i++)
         {
            pCaps->dwOutCaps[i] = OC_DIGITAL;               // All outputs are digital
         }
         for(dword i = 0; i<pCaps->dwNumOfIns; i++)
         {
            pCaps->dwInCaps[i] = OC_DIGITAL;               // All inputs are digital
         }

         // 2^n size:
         pCaps->dwReqOutputBufSize  = DEF_GSIF_BUFFER_SIZE;         // Default size of 0x2000
         pCaps->dwReqInputBufSize   = DEF_GSIF_BUFFER_SIZE;         // will be used, if you set
                                                      // to 0.

         
         asio_latency=gsif_latency;
         break;
      }

      case GSIF_API_OPENAUDIO:
         //
         // Description: Acquire/open the audio channels that GSIF will
         //              be using. The hardware does not start yet, just setup
         //              the hardware.
         //
         //  pContext = PGSIF_OPENAUDIO structure
         //
         {
         PGSIF_OPENAUDIO pOa = (PGSIF_OPENAUDIO) pContext;
         debug(DGSIF,"GSIF_API_OPENAUDIO, buffersize = %x\n",pOa->dwBufferSize);

         if((adapter->hw->card_frequency==48000 && pOa->dwFormat!=(FS_48000|SW_16)) ||
            (adapter->hw->card_frequency==44100 && pOa->dwFormat!=(FS_44100|SW_16)))
         {
          debug(DWDM,"gsif: Incorrect gsif format [%x]\n",pOa->dwFormat);
          return GSIF_OPENFAILED;
         }

         asio_latency=gsif_latency;

         if(asio_init(DEF_GSIF_MODE)) // will set is_asio=1;
         {
          debug(DWDM,"gsif: failed to init asio/gsif layer\n");
          return GSIF_OPENFAILED;
         }

         buffer_size=pOa->dwBufferSize;
         buffer_position=0;
         int err=0;

         if(buffer_size==0)
         {
          debug(DWDM,"kxgsif: buffer size is 0 [openaudio]\n");
          buffer_size=0x2000; // don't change
         }

         asio_latency=gsif_latency;

             for(int i=0;i<GSIF_MAXCHANNELS;i++)
             {
               if(pOa->dwOutFlags[i]&OA_CHANNELENABLE)
               {
                void *b=pOa->dwOutBuffers[i];
                int chn=-1;

                // b is either mem_addr or 0
                chn=asio_alloc_output(i,buffer_size,&b,adapter->hw->card_frequency,16);
                if(chn<0) // failed
                {
                 err=1;

                 if(DEF_GSIF_MODE&KXASIO_METHOD_NEW) // we allocate
                  b=0;
                }
                if((DEF_GSIF_MODE&KXASIO_METHOD_OLD) && b!=pOa->dwOutBuffers[i]) // error
                {
                 debug(DERR,"gsif: Internal Error #2/358\n");
                 err=1;
                 break;
                }
                if(!err && b && (DEF_GSIF_MODE&KXASIO_METHOD_NEW))
                {
                 // b contains user-level memory -- re-map to kernel-level
                 b=asio_hw->voicetable[chn].asio_kernel_addr;
                }
                pOa->dwOutBuffers[i]=(PULONG)b;
               }
               else
               {
                if(DEF_GSIF_MODE&KXASIO_METHOD_NEW) // we allocate buffers
                 pOa->dwOutBuffers[i]=0;
               }
               //if(pOa->dwInFlags[i]&OA_CHANNELENABLE)
               //{
               // dword b=pOa->dwInBuffers[i];
               // asio_alloc_input(i,pOa->dwBufferSize,b);
               // pOa->dwInBuffers[i]=b;
               //}
             }
             if(err)
              return GSIF_OPENFAILED;
         }
         break;

      case GSIF_API_CLOSEAUDIO:
         //
         // Description: Release/close the audio channels that GSIF is using.
         //
         //  pContext = NULL.
         //
         debug(DGSIF,"kxgsif: GSIF_API_CLOSEAUDIO\n");
         asio_close(); // will set is_asio=0;
         break;

      case GSIF_API_STARTAUDIO:
         //
         // Description: Kick off the audio channel (in-synch).
         //
         //  pContext = NULL.
         //
         debug(DGSIF,"kxgsif: GSIF_API_STARTAUDIO\n");
         buffer_position=0;
         if(pContext==0) // pre-2.0
         {
          asio_start();
         }
         else
         {
          DWORD *p=(DWORD *)pContext;
          if((*p)&STREAMTYPE_AUDIO)
           asio_start();
//          if((*p)&STREAMTYPE_MIDI)
//           ...
// FIXME
         }
         break;

      case GSIF_API_STOPAUDIO:
         //
         // Description: Stop the audio channel.
         //
         //  ptr->lpvOutBuffer = NULL.
         //  ptr->lpvInBuffer  = NULL.
         //
         debug(DGSIF,"kxgsif: GSIF_API_STOPAUDIO\n");

         if(pContext==0) // pre-2.0
         {
          asio_stop();
         }
         else
         {
          DWORD *p=(DWORD *)pContext;
          if((*p)&STREAMTYPE_AUDIO)
           asio_stop();
//          if((*p)&STREAMTYPE_MIDI)
//           ...
// FIXME
         }
         buffer_position=0;
         break;

      case GSIF_API_RESETAUDIO:
         //
         // Description: Reset the audio channels.
         // this function is no longer used
         //
         //  ptr->lpvOutBuffer = NULL.
         //  ptr->lpvInBuffer  = NULL.
         //
         debug(DGSIF,"kxgsif: GSIF_API_RESETAUDIO\n");
         buffer_position=0;

         break;

/*      case GSIF_API_GETPOSITION:
         //
         // Description: Return the byte position.
         //
         //  ptr->lpvOutBuffer = NULL.
         //  ptr->lpvInBuffer  = NULL.
         //

         // asio_get_position(1) returns #of samples, not bytes
         status = asio_get_position(1)*2 + buffer_position;  // Return position.

         debug(DGSIF,"buffer position: %x\n",status);
         break;
*/
      case GSIF_API_GETHWBUFFERINFO:
         {
         //
         // Description: Return the information about the HW buffer
         //
         //  pContext = pointer to a HWBUFFERINFO structure
         //
         PHWBUFFERINFO pBufInfo = (PHWBUFFERINFO) pContext;

         /*
         // legacy

         pBufInfo->dwNumOfSamples = buffer_size/2; // number of samples (16bits=2bytes)

         if(asio_hw && asio_hw->is_10k2)
          pBufInfo->dwNumOfSamples/=2;
         */
         pBufInfo->dwNumOfSamples=asio_latency; // in samples
         } 
         break;
      // GSIF2-specific stuff:
      case GSIF_API_V2_GETMIDICAPS:
         {
          GSIF_MIDICAPS *caps=(GSIF_MIDICAPS *)pContext;

          caps->dwNumIns=0; // FIXME hw->have_mpu;
          caps->dwNumOuts=0; // FIXME hw->have_mpu;
          caps->dwInProperties=0; // _UNIQUEPORTNAME, _TIMESTAMP
          caps->dwOutProperties=0;
          // sprintf(caps->szPortInName,"kX Uart In %x",hw->port);
          // sprintf(caps->szPortOutName,"kX Uart Out %x",hw->port);
         }
      	 break;
//      case GSIF_API_V2_GETPORTNAME:
//        // never called, since _UNIQUEPORTNAME not set
//        break;
/*      case GSIF_API_V2_OPENMIDIPORTS:
        {
         GSIF_OPENMIDI *open_midi=(GSIF_OPENMIDI *)pContext;
        }
        break;
      case GSIF_API_V2_CLOSEMIDIPORTS:
        {
         GSIF_OPENMIDI *open_midi=(GSIF_OPENMIDI *)pContext;
        }
        break;
*/
   }

   return status;
}

/*****************************************************************************
 * CGSIFInterface::RegisterCallbacks()
 *****************************************************************************
 *  This method will be called by the Giga compatible application to register
 *  a couple of callback routines. There are e hardware driver.
 *
 */
STDMETHODIMP
CGSIFInterface::RegisterCallbacks(IN PFNDRIVERREQUEST fnDriverRequest_,
                                  IN PFNDRIVERISR     fnDriverIsr_,
                                  IN PVOID            pContext_)
{
   debug(DGSIF,"RegisterCallbacks()\n");

   //
   // fnDriverRequest() is a general purpose routine that you can use
   // to control the Giga-application. It response to all the
   // GSIF_IOCTL_xxxxxxx commands. The prototype is as follows:
   //
   // DWORD _stdcall DriverRequest(DWORD dwCommand,  // GSIF_IOCTL_xxxxx
   //                              DWORD dwCardId,   // Card Id o
   //                              PVOID pContext,   // Pointer to a structure,
   //                                                // depends
   //                              PVOID pInterface);// return 'this' pointer.
   //
   // fnDriverIsr() is heartbeat that the Giga application will use
   // to schedule itself. If you have your hardware setup to transfer
   // blocks of data in 256 sample chunks then every interrupt you will
   // call the GSIF interface, letting the application know the hardware's
   // position.
   //
   // DWORD _stdcall Heartbeat(DWORD dwHardwarePosition,
   //                          PVOID pContext);
   // The GigaX will return the number of samples that it
   // processed. If you driver does a data copy before the data is
   // going to the hardware, then your driver can use this number to
   // determine the valid data.
   //
   // NOTE: this sample driver is not really a hardware driver so it is
   // not physically attached to any hardware and therefore can not
   // demonstrate how to use this function.
   //

   //
   // For now, I will just save them in my driver extension space.
   //

   fnGSIFRequest = fnDriverRequest_;
   fnGSIFIsr     = fnDriverIsr_;
   pContext      = pContext_;

   debug(DGSIF,"drvR: %x Isr: %x Ctxt: %x\n",fnGSIFRequest,fnGSIFIsr,pContext);

   //
   // For example, if I wanted to RESET the GSIF client application, I
   // would simply call this:
   //
   // GSIF_RESET resetParams;
   // 'setup resetParams to define the new parameters (see gsif.h)'
   // m_pDrvExtn->fnGSIFRequest(GSIF_IOCTL_RESET, 0, &resetParams, this);
   //
   return STATUS_SUCCESS;

}
