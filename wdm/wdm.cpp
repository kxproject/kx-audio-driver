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


// #define USE_MULTY_WAVE 1

#include "common.h"

#include "interface/guids.h"

#include "wdm/tbl_topo.h"
#include "gsif/gsif2.h"

#define MAX_MINIPORTS (MAX_MPU_DEVICES+MAX_WAVE_DEVICES+1+MAX_SYNTH_DEVICES+1+1)

#pragma code_seg()
int first_call=1;
KDPC dpc_ac3;
KDPC dpc_irq;
void parse_names(CAdapterCommon *adapter);

/////////////////////////////////////////////////////////////////////
// Begin INIT section
#pragma code_seg("PAGE")

DRIVER_ADD_DEVICE wdm_AddDevice;

extern "C" NTSTATUS wdm_StartDevice(
    IN      PDEVICE_OBJECT  pDeviceObject,  // Context for the class driver.
    IN      PIRP            pIrp,           // Context for the class driver.
    IN      PRESOURCELIST   ResourceList    // List of hardware resources.
);

#pragma code_seg("PAGE")
extern "C" NTSTATUS DriverEntry(IN PDRIVER_OBJECT   DriverObject,IN PUNICODE_STRING  RegistryPathName)
{
    PAGED_CODE();

    first_call=1;

    debug(DWDM,"kX Audio Driver -- DriverEntry\n");

    if(sizeof(kx_fpu_state)<sizeof(KFLOATING_SAVE))
    {
     debug(DWDM,"HALT: inconsistent fpu state & fpu massive\n");
     return STATUS_UNSUCCESSFUL;
    }

    NTSTATUS ntStatus = PcInitializeAdapterDriver( DriverObject,
                                                   RegistryPathName,
                                                   wdm_AddDevice);
    return ntStatus;
}


#define MAX_KX_BOARDS   16
PDEVICE_OBJECT physical_device_objects[MAX_KX_BOARDS]={0,};

#pragma code_seg("PAGE")
NTSTATUS wdm_AddDevice(IN PDRIVER_OBJECT   DriverObject,IN PDEVICE_OBJECT   PhysicalDeviceObject)

{
    PAGED_CODE();

    if(first_call)
    {
     first_call=0;
     KeInitializeDpc(&dpc_ac3,dpc_ac3_func,NULL);
     KeInitializeDpc(&dpc_irq,dpc_irq_func,NULL);

     KeSetImportanceDpc(&dpc_ac3,HighImportance);
     KeSetImportanceDpc(&dpc_irq,HighImportance);
    }

    for(int i=0;i<MAX_KX_BOARDS;i++)
    {
     if(physical_device_objects[i]==0)
     {
      physical_device_objects[i]=PhysicalDeviceObject;
      debug(DWDM,"adding device: phys_device: %x [#%d]\n",physical_device_objects[i],i);
      break;
     }
    }

    NTSTATUS ret=PcAddAdapterDevice( DriverObject,
                               PhysicalDeviceObject,
                               PCPFNSTARTDEVICE( wdm_StartDevice ),
                               MAX_MINIPORTS,
                               0);

    return ret;
}

#pragma code_seg("PAGE")
NTSTATUS InstallSubdevice(
    IN      PVOID               Context1,
    IN      PVOID               Context2,
    IN      PWCHAR              Name,
    IN      REFGUID             PortClassId,
    IN      PFNCREATEINSTANCE   MiniportCreate,
    IN      PUNKNOWN            UnknownAdapter,   
    IN      PRESOURCELIST       ResourceList,
    OUT     PUNKNOWN *          OutPortUnknown      OPTIONAL
)
{
    PAGED_CODE();

    ASSERT(Context1);
    ASSERT(Context2);
    ASSERT(Name);
    ASSERT(ResourceList);

    PPORT port;

    NTSTATUS ntStatus=PcNewPort(&port,PortClassId);

    if (NT_SUCCESS(ntStatus))
    {
        PMINIPORT miniport;
        ntStatus = MiniportCreate((PUNKNOWN *)&miniport,GUID_NULL,NULL,NonPagedPool);

        if (NT_SUCCESS(ntStatus))
        {
            //
            // Init the port driver and miniport in one go.
            //
            ntStatus = port->Init( (PDEVICE_OBJECT)Context1,
                                   (PIRP)Context2,
                                   miniport,
                                   UnknownAdapter,
                   ResourceList);

            if (NT_SUCCESS(ntStatus))
            {
                //
                // Register the subdevice (port/miniport combination).
                //
                ntStatus = PcRegisterSubdevice( (PDEVICE_OBJECT)Context1,
                                                Name,
                                                port    );
                if (!(NT_SUCCESS(ntStatus)))
                {
                    debug(DWDM,"!!! InstallSubdevice: PcRegisterSubdevice failed\n");
                }
                else
                {
                    if (OutPortUnknown)
                    {
                        //
                        //  Failure here doesn't cause the entire routine to fail.
                        //
                        (void) port->QueryInterface
                        (
                            IID_IUnknown,
                            (PVOID *) OutPortUnknown
                        );
                    }
                }
            }
            else
            {
                debug(DWDM,"!!! InstallSubdevice: port->Init failed\n");
            }

            //
            // We don't need the miniport any more.  Either the port has it,
            // or we've failed, and it should be deleted.
            //
            miniport->Release();
        }
        else
        {
            debug(DWDM,"!!! InstallSubdevice: PcNewMiniport failed\n");
        }

        port->Release();
    }
    else
    {
        debug(DWDM,"!!! InstallSubdevice: PcNewPort failed\n");
    }

    return ntStatus;
}

#pragma code_seg("PAGE")
NTSTATUS check_win98(void)
{
    PAGED_CODE();

    // create a wave port
    PPORT pPort = 0;
    NTSTATUS ntStatus = PcNewPort (&pPort,CLSID_PortWaveCyclic);

    if(NT_SUCCESS(ntStatus))
    {
        // query for the event interface which is not supported in Win98 gold.
        PPORTEVENTS pPortEvents = 0;
        ntStatus = pPort->QueryInterface (IID_IPortEvents, 
                                         (PVOID *)&pPortEvents);
        if (!NT_SUCCESS (ntStatus))
        {
            debug(DWDM,"!!! InstallSubdevice: PcNewPort failed\n");
            ntStatus = STATUS_UNSUCCESSFUL;     // change error code.
        }
        else
        {
            pPortEvents->Release ();
        }
        pPort->Release ();
   }
   return ntStatus;
}


#pragma code_seg("PAGE")
NTSTATUS wdm_StartDevice(
    IN      PDEVICE_OBJECT  pDeviceObject,  // Context for the class driver.
    IN      PIRP            pIrp,           // Context for the class driver.
    IN      PRESOURCELIST   ResourceList    // List of hardware resources.
)
{
    PAGED_CODE();

    ASSERT(pDeviceObject);
    ASSERT(pIrp);
    ASSERT(ResourceList);

    NTSTATUS ntStatus;

    kx_hw *hw=NULL;

    ntStatus=check_win98();

    if(!NT_SUCCESS(ntStatus))
    {
     debug(DWDM,"!!! OS Check failed - Win98 not supported\n");
     return ntStatus;
    }

    PADAPTERCOMMON pAdapterCommon = NULL;
    PUNKNOWN pUnknownCommon;

    PUNKNOWN    unknownTopology   = NULL;
    PUNKNOWN    unknownWave3      = NULL;
    PUNKNOWN    unknownWave0      = NULL;
    PUNKNOWN    unknownWave1      = NULL;
    PUNKNOWN    unknownWave2      = NULL;
    PUNKNOWN    unknownWave4      = NULL;
    PUNKNOWN    unknownUart       = NULL;
    PUNKNOWN    unknownUart2      = NULL;
    PUNKNOWN    unknownSynth      = NULL;
    PUNKNOWN    unknownSynth2     = NULL;
    PUNKNOWN    unknownCtrl       = NULL;

    // create a new adapter common object
    ntStatus = NewAdapterCommon( &pUnknownCommon,
                                 IID_IAdapterCommon,
                                 NULL,
                                 NonPagedPool );
    if (NT_SUCCESS(ntStatus))
    {
        ASSERT( pUnknownCommon );

        // query for the IAdapterCommon interface
        ntStatus = pUnknownCommon->QueryInterface( IID_IAdapterCommon,
                                                   (PVOID *)&pAdapterCommon );
        if (NT_SUCCESS(ntStatus))
        {
            // Initialize the object
            ntStatus = pAdapterCommon->Init( ResourceList,
                                             pDeviceObject );
            if (NT_SUCCESS(ntStatus))
            {
                // register with PortCls for power-managment services
                ntStatus = PcRegisterAdapterPowerManagement( (PUNKNOWN)pAdapterCommon,
                                                             pDeviceObject );
                if(!NT_SUCCESS(ntStatus))
                {
                   debug(DWDM,"!!! Error registering with Power Management %d\n",ntStatus);
                }
            } else debug(DWDM,"!!! StartDevice - error padaptercommon->init\n");
        }

        // release the IUnknown on adapter common
        pUnknownCommon->Release();
    }

    if(NT_SUCCESS(ntStatus))
    {
        ntStatus = InstallSubdevice(
                                    pDeviceObject,
                                    pIrp,
                                    L"kx_topo1",
                                    CLSID_PortTopology, // port class
                                    create_topology, // miniport creation func
                                    pAdapterCommon,
                                    ResourceList,
                                    &unknownTopology
                                    ); 
    }

    CAdapterCommon *adapter=NULL;

    if(unknownTopology)
    {
        adapter=(CAdapterCommon *)pAdapterCommon;
        if(adapter && (adapter->magic==ADAPTER_MAGIC))
        {
            hw=adapter->hw;
        }
        else
        {
            debug(DWDM,"!!! Bad Adapter Magic\n");
            ntStatus=STATUS_UNSUCCESSFUL;
        }
    }

    if(NT_SUCCESS(ntStatus))
    {
        ntStatus = InstallSubdevice(
                                    pDeviceObject,
                                    pIrp,
                                    L"kx_wave0",
                                    CLSID_PortWaveCyclic, // port class
                                    create_wave, // miniport creation func
                                    pAdapterCommon,
                                    ResourceList,
                                    &unknownWave0
                                    ); 
    }

    if(NT_SUCCESS(ntStatus))
    {
        ntStatus = InstallSubdevice(
                                    pDeviceObject,
                                    pIrp,
                                    L"kx_wave1",
                                    CLSID_PortWaveCyclic, // port class
                                    create_wave, // miniport creation func
                                    pAdapterCommon,
                                    ResourceList,
                                    &unknownWave1
                                    ); 
    }

    if(NT_SUCCESS(ntStatus))
    {
        ntStatus = InstallSubdevice(
                                    pDeviceObject,
                                    pIrp,
                                    L"kx_wave2",
                                    CLSID_PortWaveCyclic, // port class
                                    create_wave, // miniport creation func
                                    pAdapterCommon,
                                    ResourceList,
                                    &unknownWave2
                                    ); 
    }

    if(NT_SUCCESS(ntStatus))
    {
        ntStatus = InstallSubdevice(
                                    pDeviceObject,
                                    pIrp,
                                    L"kx_wave3",
                                    CLSID_PortWaveCyclic, // port class
                                    create_wave, // miniport creation func
                                    pAdapterCommon,
                                    ResourceList,
                                    &unknownWave3
                                    ); 
    }

    if(hw && hw->is_a2 && !hw->is_k8 && !hw->is_edsp) // a2, but not k4 (dev0008, a2value, a2zsnb, any E-DSP)
     if(NT_SUCCESS(ntStatus))
     {
        ntStatus = InstallSubdevice(
                                    pDeviceObject,
                                    pIrp,
                                    L"kx_wave4",
                                    CLSID_PortWaveCyclic, // port class
                                    create_waveHQ, // miniport creation func
                                    pAdapterCommon,
                                    ResourceList,
                                    &unknownWave4
                                    ); 
            if (!unknownWave4)
            {
                debug(DWDM,"!!! StartDevice - NO WAVE4/HQ\n");
            }
     }

    if(NT_SUCCESS(ntStatus))
    {

        ntStatus = InstallSubdevice(
                                    pDeviceObject,
                                    pIrp,
                                    L"kx_synth1",
                                    CLSID_PortMidi, // port class
                                    create_synth, // miniport creation func
                                    pAdapterCommon,
                                    ResourceList,
                                    &unknownSynth
                                    );
    }

    if(NT_SUCCESS(ntStatus))
    {

        ntStatus = InstallSubdevice(
                                    pDeviceObject,
                                    pIrp,
                                    L"kx_synth2",
                                    CLSID_PortMidi, // port class
                                    create_synth, // miniport creation func
                                    pAdapterCommon,
                                    ResourceList,
                                    &unknownSynth2
                                    );
    }

    if(hw && (hw->have_mpu>=1))
    {
            if(NT_SUCCESS(ntStatus))
            {
                ntStatus = InstallSubdevice(
                                            pDeviceObject,
                                            pIrp,
                                            L"kx_uart1",
                                            CLSID_PortMidi, // port class 
                                            create_uart, // miniport creation func
                                            pAdapterCommon,
                                            ResourceList,
                                            &unknownUart
                                            ); 
            }
    }

    if(NT_SUCCESS(ntStatus))
    {
        ntStatus = InstallSubdevice(
                                    pDeviceObject,
                                    pIrp,
                                    L"kx_ctrl",
                                    CLSID_PortMidi, // port class 
                                    create_ctrl, // miniport creation func
                                    pAdapterCommon,
                                    ResourceList,
                                    &unknownCtrl
                                    ); 
    }


    if(hw && (hw->have_mpu>1))
    {
            if(NT_SUCCESS(ntStatus))
            {
                ntStatus = InstallSubdevice(
                                            pDeviceObject,
                                            pIrp,
                                            L"kx_uart2",
                                            CLSID_PortMidi, // port class 
                                            create_uart, // miniport creation func
                                            pAdapterCommon,
                                            ResourceList,
                                            &unknownUart2
                                            ); 
                    if (unknownUart2)
                    {
                        unknownUart2->Release();
                    } else debug(DWDM,"!!! StartDevice - NO UART 2\n");
            }
    }

    if (unknownTopology)
    {
#if 0
        if (unknownWave1)
        {
            // register wave <=> topology connections
            PcRegisterPhysicalConnection( pDeviceObject,
                                        unknownWave1,
                                        WAVE_NODE_OUTPUT,
                                        unknownTopology,
                                        #ifdef USE_MULTY_WAVE
                                         TOPO_WAVEOUT23_SOURCE
                                        #else
                                         TOPO_WAVEOUT01_SOURCE
                                        #endif
                                        );
        }
        if (unknownWave2)
        {
            // register wave <=> topology connections
            PcRegisterPhysicalConnection( pDeviceObject,
                                        unknownWave2,
                                        WAVE_NODE_OUTPUT,
                                        unknownTopology,
                                        #ifdef USE_MULTY_WAVE
                                         TOPO_WAVEOUT45_SOURCE
                                        #else
                                         TOPO_WAVEOUT01_SOURCE
                                        #endif
                                        );
        }
        if (unknownWave3)
        {
            // register wave <=> topology connections
            PcRegisterPhysicalConnection( pDeviceObject,
                                        unknownWave3,
                                        WAVE_NODE_OUTPUT,
                                        unknownTopology,
                                        #ifdef USE_MULTY_WAVE
                                         TOPO_WAVEOUT67_SOURCE
                                        #else
                                         TOPO_WAVEOUT01_SOURCE
                                        #endif
                                        );
        }
        if (unknownWave4)
        {
            // register wave <=> topology connections
            PcRegisterPhysicalConnection( pDeviceObject,
                                        unknownWave4,
                                        WAVE_NODE_OUTPUT,
                                        unknownTopology,
                                        #ifdef USE_MULTY_WAVE
                                         TOPO_WAVEOUTHQ_SOURCE
                                        #else
                                         TOPO_WAVEOUT01_SOURCE
                                        #endif
                                        );
        }
        // should be last for 98/me (2/jan/2004)
        if (unknownWave0)
        {
            // register wave <=> topology connections
            PcRegisterPhysicalConnection( pDeviceObject,
                                        unknownTopology,
                                        TOPO_WAVEIN_DEST,
                                        unknownWave0,
                                        WAVE_NODE_INPUT );
            PcRegisterPhysicalConnection( pDeviceObject,
                                        unknownWave0,
                                        WAVE_NODE_OUTPUT,
                                        unknownTopology,
                                        TOPO_WAVEOUT01_SOURCE );
        }
#else
        // should be last for 98/me (2/jan/2004)
        if (unknownWave0)
        {
            // register wave <=> topology connections
            PcRegisterPhysicalConnection( pDeviceObject,
                                        unknownTopology,
                                        TOPO_WAVEIN_DEST,
                                        unknownWave0,
                                        WAVE_NODE_INPUT );

            PcRegisterPhysicalConnection( pDeviceObject,
                                        unknownWave0,
                                        WAVE_NODE_OUTPUT,
                                        unknownTopology,
                                        TOPO_WAVEOUT01_SOURCE );
        }

        if (unknownSynth)
        {
            // register synth <=> topology connection
            PcRegisterPhysicalConnection( pDeviceObject,
                                        unknownSynth,
                                        1,
                                        unknownTopology,
                                        TOPO_SYNTH1_SOURCE );
        }
        if (unknownSynth2)
        {
            // register synth <=> topology connection
            PcRegisterPhysicalConnection( pDeviceObject,
                                        unknownSynth2,
                                        1,
                                        unknownTopology,
                                        TOPO_SYNTH1_SOURCE);
        }

#endif
    }

    if(adapter)
    {
        for(int i=0;i<MAX_WAVE_DEVICES;i++)
        {
         if(adapter->Wave[i])
         {
          if(adapter->Wave[i]->magic==WAVE_MAGIC)
          {
            if(adapter->Wave[i]->wave_number!=i)
             debug(DWDM,"!!! type=%d number=%d\n",i,adapter->Wave[i]->wave_number);
          }
          else 
           debug(DWDM,"!!! (wdm): magic = %x [%d]\n",adapter->Wave[i]->magic,i);
         }
          else
           debug(DWDM,"!!! (wdm): wdm[%d] = NULL\n",i);
        }

        // try to get PDO via portclass extension...
        if(adapter->Topology && adapter->Topology->Port)
        {
                ULONG ret_b=0;
                wchar_t name[128];
                int found=-1;
                if(NT_SUCCESS(adapter->Topology->Port->GetDeviceProperty(DevicePropertyPhysicalDeviceObjectName,sizeof(name),name,&ret_b)))
                {
                  for(int i=0;i<MAX_KX_BOARDS;i++)
                  {
                    if(physical_device_objects[i]==0)
                     continue;

                    // look for physical device object here:
                    wchar_t name2[128];
                    ULONG ret_b2=0;
                    if(NT_SUCCESS(IoGetDeviceProperty(physical_device_objects[i],DevicePropertyPhysicalDeviceObjectName,sizeof(name2),name2,&ret_b2)))
                    {
                             if(ret_b2==ret_b && memcmp(name,name2,ret_b)==0)
                             {
                              // found
                              found=i;
                              break;
                             }
                    }
              }
                }
                if(found!=-1)
                {
                 adapter->physical_device_object=physical_device_objects[found];
                 debug(DWDM,"kx: using [%x/%d] as physical device object\n",adapter->physical_device_object,found);
                 physical_device_objects[found]=0;

                 parse_names(adapter);
                 register_gsif(adapter);
                }
                else
                 debug(DWDM,"!!! kx: PDO object not found\n");
        } else debug(DWDM,"!!! kx: [start] incorrect topology / port\n");
    }

    //
    // Release the unknowns.
    //
    if (unknownTopology)
    {
        unknownTopology->Release();
    } else debug(DWDM,"!!! StartDevice - NO TOPOLOGY\n");
    if (unknownWave3)
    {
        unknownWave3->Release();
    } else debug(DWDM,"!!! StartDevice - NO WAVE3\n");
    if (unknownWave0)
    {
        unknownWave0->Release();
    } else debug(DWDM,"!!! StartDevice - NO WAVE0\n");
    if (unknownWave1)
    {
        unknownWave1->Release();
    } else debug(DWDM,"!!! StartDevice - NO WAVE1\n");
    if (unknownWave2)
    {
        unknownWave2->Release();
    } else debug(DWDM,"!!! StartDevice - NO WAVE2\n");
    if (unknownUart)
    {
        unknownUart->Release();
    } else debug(DWDM,"!!! StartDevice - NO UART1\n");
    if (unknownSynth)
    {
        unknownSynth->Release();
    } else debug(DWDM,"!!! StartDevice - NO SYNTH\n");
    if (unknownSynth2)
    {
        unknownSynth2->Release();
    } else debug(DWDM,"!!! StartDevice - NO SYNTH2\n");
    if (unknownCtrl)
    {
        unknownCtrl->Release();
    } else debug(DWDM,"!!! StartDevice - NO CTRL\n");
    if (unknownWave4)
    {
        unknownWave4->Release();
    } 

    if (pAdapterCommon)
    {
           pAdapterCommon->Release();
    }

/*
    // safety test [guids]
    UNICODE_STRING guid; memset(&guid,0,sizeof(guid));

    RtlStringFromGUID(GSIFCLASS_INTERFACE,&guid);
    debug(DWDM,"GSIF guid: %S\n",guid.Buffer);
    RtlFreeUnicodeString(&guid); memset(&guid,0,sizeof(guid));

    RtlStringFromGUID(TOPO_WAVEIN_NAME,&guid);
    debug(DWDM,"TOPO_WAVEIN_NAME guid: %S\n",guid.Buffer);
    RtlFreeUnicodeString(&guid); memset(&guid,0,sizeof(guid));

    RtlStringFromGUID(DSPROPSETID_EAX20_BufferProperties,&guid);
    debug(DWDM,"DSPROPSETID_EAX20_BufferProperties guid: %S\n",guid.Buffer);
    RtlFreeUnicodeString(&guid); memset(&guid,0,sizeof(guid));

    RtlStringFromGUID(KSDATAFORMAT_SUBTYPE_DOLBY_AC3_SPDIF,&guid);
    debug(DWDM,"KSDATAFORMAT_SUBTYPE_DOLBY_AC3_SPDIF guid: %S\n",guid.Buffer);
    RtlFreeUnicodeString(&guid); memset(&guid,0,sizeof(guid));

    RtlStringFromGUID(IID_IUnknown,&guid);
    debug(DWDM,"IID_IUnknown guid: %S\n",guid.Buffer);
    RtlFreeUnicodeString(&guid); memset(&guid,0,sizeof(guid));
*/
    return ntStatus;
}


#pragma code_seg("PAGE")
void parse_names(CAdapterCommon *adapter)
{
    PAGED_CODE();

    // try getting device interfaces
    PWSTR list;

    NTSTATUS st=IoGetDeviceInterfaces(&KSGUID_AUDIO,adapter->physical_device_object,DEVICE_INTERFACE_INCLUDE_NONACTIVE,&list);
    if(NT_SUCCESS(st))
    {
     unsigned short *p=list;
     while(*p)
     {
        UNICODE_STRING interface_unicode;
        RtlInitUnicodeString(&interface_unicode, p);

        if(wcsstr(p,adapter->device_name+4)!=NULL) // check hardware id: Win2k ONLY
        {
             HANDLE reg;
             st=IoOpenDeviceInterfaceRegistryKey(&interface_unicode,KEY_ALL_ACCESS,&reg);
             if(NT_SUCCESS(st))
             {
              UNICODE_STRING key_name;
              RtlInitUnicodeString(&key_name,L"FriendlyName");

              char tmp_buffer[512]; memset(&tmp_buffer,0,sizeof(tmp_buffer));
              ULONG ret_b=0;

              if(NT_SUCCESS(ZwQueryValueKey(reg,&key_name,KeyValuePartialInformation,tmp_buffer,sizeof(tmp_buffer),&ret_b)))
              {
                int to_skip=0;
                char card_name[KX_MAX_STRING];
                strncpy(card_name,adapter->hw->card_name,KX_MAX_STRING);

                if(wcsstr(p,L"kx_synth2")!=0)
                 sprintf(tmp_buffer,"kX Synth2 %s",card_name);
                 else
                if(wcsstr(p,L"kx_synth1")!=0)
                 sprintf(tmp_buffer,"kX Synth %s",card_name);
                 else
                if(wcsstr(p,L"kx_uart2")!=0)
                 sprintf(tmp_buffer,"kX Uart2 %s",card_name);
                 else
                if(wcsstr(p,L"kx_uart1")!=0)
                 sprintf(tmp_buffer,"kX Uart %s",card_name);
                 else
                if(wcsstr(p,L"kx_wave0")!=0)
                 sprintf(tmp_buffer,"kX Wave %s 0/1",card_name);
                 else
                if(wcsstr(p,L"kx_wave1")!=0)
                 sprintf(tmp_buffer,"kX Wave %s 2/3",card_name);
                 else
                if(wcsstr(p,L"kx_wave2")!=0)
                 sprintf(tmp_buffer,"kX Wave %s 4/5",card_name);
                 else
                if(wcsstr(p,L"kx_wave3")!=0)
                 sprintf(tmp_buffer,"kX Wave %s 6/7",card_name);
                 else
                if(wcsstr(p,L"kx_wave4")!=0)
                 sprintf(tmp_buffer,"kX Wave %s HQ",card_name);
                 else
                if(wcsstr(p,L"kx_topo1")!=0)
                 sprintf(tmp_buffer,"kX Mixer %s 0/1",card_name);
                 else
                if(wcsstr(p,L"kx_topo2")!=0)
                 sprintf(tmp_buffer,"kX Mixer %s 2/3",card_name);
                 else
                if(wcsstr(p,L"kx_topo3")!=0)
                 sprintf(tmp_buffer,"kX Mixer %s 4/5",card_name);
                 else
                if(wcsstr(p,L"kx_topo4")!=0)
                 sprintf(tmp_buffer,"kX Mixer %s 6/7",card_name);
                 else
                if(wcsstr(p,L"kx_ctrl")!=0)
                 sprintf(tmp_buffer,"kX Control %s",card_name);
                 else to_skip=1;

                if(!to_skip)
                {
                  ANSI_STRING val_a;
                  RtlInitAnsiString(&val_a,tmp_buffer);
                  UNICODE_STRING val_u;
                  RtlAnsiStringToUnicodeString(&val_u,&val_a,TRUE);

                  ZwSetValueKey(reg,&key_name,0,REG_SZ,val_u.Buffer,wcslen(val_u.Buffer)*2+2);
                  RtlFreeUnicodeString(&val_u);
                }
              }
              ZwClose(reg);
           } else debug(DERR,"!!! FAILED opening Device Interfaces Registry key! [%x]\n",st);
        } // correct hardware id?

        // skip current and goto next
        while(*p)
         p++;
        p++;
     } // while(*p)

     ExFreePool(list);
    } else debug(DWDM,"!!! failed getting device interfaces! [%x]\n",st);
}
/*
BOOLEAN RunningOnNT(void)
{
    OBJECT_ATTRIBUTES oa;
    UNICODE_STRING sam;
    HANDLE hKey;
    NTSTATUS status;

    RtlInitUnicodeString(&sam, L"\\Registry\\Machine\\SAM");

    InitializeObjectAttributes(&oa, &sam, OBJ_CASE_INSENSITIVE, NULL, NULL);

    status = ZwOpenKey(&hKey, 0, &oa);

    if ( NT_SUCCESS(status) )
    {
        ZwClose(hKey);
        return TRUE;
    }
    else
*/
