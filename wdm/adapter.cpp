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

#include "gsif/gsif2.h"
#include "gsif/kxgsif.h"
#include "wdm/miniwave_p16v.h"

extern "C" NTKERNELAPI PHYSICAL_ADDRESS MmGetPhysicalAddress (__in PVOID BaseAddress);


/*****************************************************************************
 * NewAdapterCommon()
 *****************************************************************************
 * Create a new adapter common object.
 */
#pragma code_seg("PAGE")
NTSTATUS NewAdapterCommon
(
    OUT     PUNKNOWN *  Unknown,
    IN      REFCLSID,
    IN      PUNKNOWN    UnknownOuter    OPTIONAL,
    IN      POOL_TYPE   PoolType
)
{
    PAGED_CODE();

    ASSERT(Unknown);

    STD_CREATE_BODY_
    (
        CAdapterCommon,
        Unknown,
        UnknownOuter,
        PoolType,
        PADAPTERCOMMON
    );
}   

/*****************************************************************************
 * CAdapterCommon::Init()
 *****************************************************************************
 * Initialize an adapter common object.
 */
#pragma code_seg("PAGE")
NTSTATUS
CAdapterCommon::
Init
(
    IN      PRESOURCELIST   ResourceList,
    IN      PDEVICE_OBJECT  DeviceObject_
)
{
    PAGED_CODE();

    debug(DWDM,"[CAdapter::Init]\n");

    ASSERT(ResourceList);
    ASSERT(DeviceObject_);

    NTSTATUS ntStatus = 0;
    magic=ADAPTER_MAGIC;

    DeviceObject=DeviceObject_;
    physical_device_object=0;

    gsif_interface=NULL;
    gsif_device.Buffer=0;

    for(int i=0;i<MAX_MPU_DEVICES;i++)
     Uart[i]=NULL;
    for(int i=0;i<MAX_SYNTH_DEVICES;i++)
     Synth[i]=NULL;
    for(int i=0;i<MAX_WAVE_DEVICES;i++)
     Wave[i]=NULL;
    WaveHQ=NULL;

    Topology=NULL;

    KeInitializeSpinLock(&dpc_lock);

    PowerState=PowerDeviceD0;
    InterruptSync=NULL;

    synth_count=0;
    uart_count=0;
    wave_count=0;

    is_vista=0;

    #ifndef NTDDI_VISTA
      #define NTDDI_WIN6                          0x06000000
      #define NTDDI_VISTA                         NTDDI_WIN6 
    #endif

    if(RtlIsNtDdiVersionAvailable(NTDDI_VISTA)==TRUE) is_vista=1;

    m_io_base = ResourceList->FindTranslatedPort(0)->u.Port.Start.LowPart;
    m_irq = ResourceList->FindTranslatedInterrupt(0)->u.Interrupt.Level;

    if(NT_SUCCESS(ntStatus))
    {
        //
        // Hook up the interrupt.
        //
        ntStatus = PcNewInterruptSync(                              // See portcls.h
                                        &InterruptSync,             // Save object ptr
                                        NULL,                       // OuterUnknown(optional).
                                        ResourceList,               // He gets IRQ from ResourceList.
                                        0,                          // Resource Index
                                        InterruptSyncModeNormal     // Run ISRs once until we get SUCCESS
                                     );
        if (NT_SUCCESS(ntStatus) && InterruptSync)
        {
            ntStatus = InterruptSync->RegisterServiceRoutine(InterruptServiceRoutine,PVOID(this),TRUE); //  run this ISR first
            if (!NT_SUCCESS(ntStatus))
             debug(DWDM,"!!! RegisterServiceRoutine failed\n");
            else
            {
              ntStatus = InterruptSync->Connect();
              // 'hw' is not available yet...
              if(hw)
              {
               debug(DERR,"!! internal error -- interruptsync object invalid!\n");
              }
            }

            // if we could not connect or register the ISR, release the object.
            if (!NT_SUCCESS (ntStatus))
            {
                debug(DWDM,"!!! Adapter init: errors occured (i.e. ->Connect())\n");
                InterruptSync->Disconnect();
                InterruptSync->Release();
                InterruptSync = NULL;
            }
        }
    } 
    return ntStatus;
}

/*****************************************************************************
 * CAdapterCommon::~CAdapterCommon()
 *****************************************************************************
 * Destructor.
 */
#pragma code_seg()
CAdapterCommon::~CAdapterCommon(void)
{
    debug(DWDM,"[CAdapterCommon::~CAdapterCommon]\n");

    if(magic!=ADAPTER_MAGIC)
    {
     debug(DWDM,"!!! ~CAdapterCommon: Magic!=%x (%x)\n",ADAPTER_MAGIC,magic);
     return;
    }

    // invalidate this
    magic=0;

    if(hw)
    {
        close_gsif(this);

        kx_close(&hw);
        hw=NULL;
    }

    if (InterruptSync)
    {
        InterruptSync->Disconnect();
        InterruptSync->Release();
        InterruptSync = NULL;
    }

    for(int i=0;i<MAX_MPU_DEVICES;i++)
     if(Uart[i])
      debug(DWDM,"!!! not all uart devices were cleared\n");

    for(int i=0;i<MAX_WAVE_DEVICES;i++)
     if(Wave[i])
      debug(DWDM,"!!! not all wave devices were cleared\n");

    if(WaveHQ)
     debug(DWDM,"!! not all waveHQ devices were cleared\n");

    for(int i=0;i<MAX_SYNTH_DEVICES;i++)
     if(Synth[i])
      debug(DWDM,"!!! not all synth devices were cleared\n");

    if(Topology)
      debug(DWDM,"!!! Topology device was not cleared\n");

    if(Ctrl)
      debug(DWDM,"!!! Ctrl device was not cleared\n");

    debug(DWDM,"---- kX driver instance terminated\n");
}

/*****************************************************************************
 * CAdapterCommon::NonDelegatingQueryInterface()
 *****************************************************************************
 * Obtains an interface.
 */
#pragma code_seg("PAGE")
STDMETHODIMP
CAdapterCommon::
NonDelegatingQueryInterface
(
    REFIID  Interface,
    PVOID * Object
)
{
    PAGED_CODE();

    ASSERT(Object);

    if (IsEqualGUIDAligned(Interface,IID_IUnknown))
    {
        *Object = PVOID(PUNKNOWN(PADAPTERCOMMON(this)));
    }
    else
    if (IsEqualGUIDAligned(Interface,IID_IAdapterCommon))
    {
        *Object = PVOID(PADAPTERCOMMON(this));
    }
	// 3551
    else if (IsEqualGUIDAligned(Interface, IID_IAdapterPowerManagement2))
    {
        *Object = PVOID(PADAPTERPOWERMANAGMENT2(this));
    }
    else
    if (IsEqualGUIDAligned(Interface,IID_IAdapterPowerManagment))
    {
        *Object = PVOID(PADAPTERPOWERMANAGMENT(this));
    }
    else
    {
    	debug(DWDM,"!!! - (Adapter) try to delegate to ref_iid :"
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

/*****************************************************************************
 * CAdapterCommon::GetInterruptSync()
 *****************************************************************************
 * Get a pointer to the interrupt synchronization object.
 */
#pragma code_seg("PAGE")
STDMETHODIMP_(PINTERRUPTSYNC) CAdapterCommon::GetInterruptSync(void)
{
    PAGED_CODE();

    return InterruptSync;
}

#pragma code_seg()
STDMETHODIMP_(NTSTATUS) CAdapterCommon::QueryPowerChangeState(IN      POWER_STATE     NewState)
{
    debug(DWDM,"[CAdapterCommon::QueryPowerChangeState]\n");

    // Check here to see of a legitimate state is being requested
    // based on the device state and fail the call if the device/driver
    // cannot support the change requested.  Otherwise, return STATUS_SUCCESS.
    // Note: A QueryPowerChangeState() call is not guaranteed to always preceed
    // a PowerChangeState() call.
    // is this actually a state change
    // switch on new state
    switch( NewState.DeviceState )
    {
            case PowerDeviceD0:
            case PowerDeviceD1:
            case PowerDeviceD2:
            case PowerDeviceD3:
            	return STATUS_SUCCESS;
            default:
                debug(DWDM,"!!! Unknown Device Power State\n");
    }

    return STATUS_INVALID_PARAMETER;
}

/*****************************************************************************
 * CAdapterCommon::QueryDeviceCapabilities()
 *****************************************************************************
 * Called at startup to get the caps for the device.  This structure provides
 * the system with the mappings between system power state and device power
 * state.  This typically will not need modification by the driver.
 * 
 */
#pragma code_seg()
STDMETHODIMP_(NTSTATUS)
CAdapterCommon::
QueryDeviceCapabilities
(
    IN      PDEVICE_CAPABILITIES    PowerDeviceCaps
)
{
    debug(DPROP,"[CAdapterCommon::QueryDeviceCapabilities]\n");

    return STATUS_SUCCESS;
}

#pragma code_seg()
STDMETHODIMP_(void) CAdapterCommon::PowerChangeState(IN      POWER_STATE     NewState)
{
    debug(DWDM,"[CAdapterCommon::PowerChangeState]: do nothing\n");

    PowerState = NewState.DeviceState;
}

// 3551
#pragma code_seg()
STDMETHODIMP_(void) CAdapterCommon::PowerChangeState2(__in DEVICE_POWER_STATE _NewDeviceState, __in SYSTEM_POWER_STATE _NewSystemState)
{
    debug(DWDM,"[CAdapterCommon::PowerChangeState2]: do nothing (%d->%d)\n",_NewDeviceState,_NewSystemState);

    PowerState = _NewDeviceState;

    return;
}

/*****************************************************************************
 * InterruptServiceRoutine()
 *****************************************************************************
 * ISR.
 */

KSYNCHRONIZE_ROUTINE tmp_sync_interrupt;

#pragma code_seg()
BOOLEAN tmp_sync_interrupt(void *context)
{
 kx_sync((sync_data *)context);
 return TRUE;
}


#pragma code_seg()
void SynchronizedHandler(void *call_with,sync_data *s)
{
 CAdapterCommon *that = (CAdapterCommon *) call_with;

 if(!s || !s->hw || !that || that->magic!=ADAPTER_MAGIC)
 {
  // CAdapterCommon is probably already destroyed
  return;
 }

 kx_hw *hw=s->hw;
 PKINTERRUPT kinterrupt=that->InterruptSync->GetKInterrupt();

 if(kinterrupt)
 {
   DWORD irql=KeGetCurrentIrql();

   if(irql<=DISPATCH_LEVEL) // non-irq call:
      KeSynchronizeExecution(kinterrupt,tmp_sync_interrupt,s); // actually kx_sync()
   else
   {
     // if running on DIRQ level, KeSynch. causes a lock-up in SMP/HT environment...
     // just launch it...
     // we are already @ DIRQ, running interrupt service routine
     kx_sync(s);
   }
 }
 else
 {
   debug(DWDM,"possible sync error: no kInterrupt object [kx_sync]\n");
   kx_sync(s);
 }
}


#pragma code_seg()
NTSTATUS InterruptServiceRoutine(IN      PINTERRUPTSYNC  InterruptSync,IN      PVOID           DynamicContext)
{
    ASSERT(InterruptSync);
    ASSERT(DynamicContext);

    CAdapterCommon *that = (CAdapterCommon *) DynamicContext;
    if(!that || that->magic!=ADAPTER_MAGIC || that->hw==NULL)
    {
      debug(DWDM,"!!! Null Dynamic context in adapter::ISR\n");
      return STATUS_UNSUCCESSFUL;
    }

    // in order to improve overall system performance:
    dword ipr=inpd(that->hw->port + IPR);
    if(ipr==0)
     return STATUS_UNSUCCESSFUL;

    sync_data dta;
    dta.what=KX_SYNC_IPR_IRQ;
    dta.hw=that->hw;

    SynchronizedHandler(that->hw->cb.call_with,&dta); // will call kx_sync(dta) -> kx_interrupt_critical()

    if(dta.ret==0)
    {
           if(dta.irq_mask)
             KeInsertQueueDpc(&dpc_irq,that,0); // this will call kx_interrupt_deferred()

           return STATUS_SUCCESS;
    }
    else
           return STATUS_UNSUCCESSFUL; // not our IRQ
}

#pragma code_seg()
VOID dpc_irq_func(
    IN PKDPC Dpc,
    IN PVOID DeferredContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    )
{
    CAdapterCommon *that=(CAdapterCommon *)SystemArgument1;

    KeAcquireSpinLockAtDpcLevel(&that->dpc_lock);

    // process...
    while(1)
    {
        int ret=kx_interrupt_deferred(that->hw);
        if(ret==KX_IRQ_NONE)
        {
         KeReleaseSpinLockFromDpcLevel(&that->dpc_lock);
         return;
        }

    	if(ret&KX_IRQ_MPUIN)
    	{
    		ret&=(~KX_IRQ_MPUIN);

            if(that->Uart[0])
            {
             if(that->Uart[0]->ServiceGroup && that->Uart[0]->Port)
               that->Uart[0]->Port->
                  Notify(that->Uart[0]->ServiceGroup);
            }
            if(that->Ctrl) // KX_HW_NO_UART_ROUTING is checked in Read handler
            {
             if(that->Ctrl->ServiceGroup && that->Ctrl->Port)
               that->Ctrl->Port->Notify(that->Ctrl->ServiceGroup);
            }
	    }
    	if(ret&KX_IRQ_MPUIN2)
    	{
    		ret&=(~KX_IRQ_MPUIN2);

            if(that->Uart[1])
            {
             if(that->Uart[1]->ServiceGroup && that->Uart[1]->Port)
               that->Uart[1]->Port->
                  Notify(that->Uart[1]->ServiceGroup);
            }
            if(that->Ctrl) // KX_HW_NO_UART_ROUTING is checked in Read handler
            {
             if(that->Ctrl->ServiceGroup && that->Ctrl->Port)
               that->Ctrl->Port->Notify(that->Ctrl->ServiceGroup);
            }
	    }

    	if(ret)
    	{
    		debug(DWDM,"note: irq code invalid [%x]\n",ret);
    	}
    }

    KeReleaseSpinLockFromDpcLevel(&that->dpc_lock);
}

#pragma code_seg()
void send_message(void *call_with,int len,const void *b)
{
 CAdapterCommon *ca=(CAdapterCommon *)call_with;
 if(ca)
 {
  if(ca->Ctrl)
  {
   ca->Ctrl->output_buffer(len,(byte *)b);
   if(ca->Ctrl->ServiceGroup && ca->Ctrl->Port)
    ca->Ctrl->Port->Notify(ca->Ctrl->ServiceGroup);
  }
 }
}

#pragma code_seg()
void malloc_func(void *this_,int size, void **b,int where)
{
 char *ptr=NULL;
 if(where==KX_NONPAGED)
 {
  ptr = new (NonPagedPool,'pnXk') char[size];
  /*
  // process-dependent quota...

  __try
  {
        ptr=(char *)ExAllocatePoolWithQuotaTag(NonPagedPool,size,'qpXk');
        if(ptr)
        {
            RtlZeroMemory(ptr,size);
        }
  }
  __except(EXCEPTION_EXECUTE_HANDLER)
  {
  	debug(DWDM,"error allocating memory [%d bytes]\n",size);
  }
  */
 }
 else
  ptr = new (PagedPool,'ppXk') char [size];

 (*b)=ptr;
}

#pragma code_seg()
void free_func(void *this_,void *b)
{
 if(b)
  delete b;
}

#pragma code_seg()
void usleep(int microseconds)
{
 KeStallExecutionProcessor((ULONG)microseconds);
}

#pragma code_seg()
int lmem_alloc_func(void *call_with,int len,void **lm,kx_cpu_cache_type_t cache)
{
 lmem **mm=(lmem **)lm;
 *mm=NULL;

 lmem *m=(lmem *)ExAllocatePoolWithTag(NonPagedPool,sizeof(lmem),'lmKX');
 if(m)
 {
  MEMORY_CACHING_TYPE cache_type=MmNonCached;

  if(cache==KX_NONCACHED)
   cache_type=MmNonCached;
  else
   if(cache==KX_CACHED)
    cache_type=MmCached;

  if(allocate_large_memory(m,len,cache_type)==0)
  {
   *mm=m;
   return 0;
  }
  else
  {
   ExFreePool(m);
   return -2;
  }
 }
 else
  return -1;
}

#pragma code_seg()
int lmem_free_func(void *call_with,void **lm)
{
 lmem **mm=(lmem **)lm;

 if(*mm)
 {
  free_large_memory(*mm);
  ExFreePool(*mm);
  *mm=NULL;
 }
 return 0;
}

#pragma code_seg()
void * lmem_get_addr_func(void *call_with,void **lm,int offset,__int64 *phys_addr)
{
 lmem **mm=(lmem **)lm;

 if(*mm)
 {
  void *pntr=(void *)((uintptr_t)((*mm)->addr)+offset);
  if(phys_addr)
  {
     PHYSICAL_ADDRESS a=MmGetPhysicalAddress(pntr);
     *phys_addr=a.QuadPart;
  }
  return pntr;
 }
 // bugcheck here?..
 return 0;
}

#pragma code_seg()
void notify_func(void *data,int what)
{
 if(what==LLA_NOTIFY_EVENT) // special case
 {
  PRKEVENT event=(PRKEVENT)data;
  KeSetEvent(event,0,FALSE);
  return;
 }

 if(what==LLA_NOTIFY_TIMER)
 {
      CMiniportWaveStream *s=(CMiniportWaveStream *)data;
      CMiniportWaveStreamHQ *hq=(CMiniportWaveStreamHQ *)data;
      CGSIFInterface *g=(CGSIFInterface *)s;
      if(s)
      {
       if((s->magic==WAVEINSTREAM_MAGIC) || (s->magic==WAVEOUTSTREAM_MAGIC))
       {
          int ok=0;

          if(s->Miniport)
          {
           if(s->Miniport->Port)
           {
            if(s->ServiceGroup)
            {
              s->my_isr();
              ok=1;
            }
           }
          }

          if(!ok)
          {
           debug(DERR,"!! timer_func: magic=wave_stream; invalid pointers\n");
          }
       } // wave magic
       else
        if(hq->magic==WAVEHQSTREAM_MAGIC)
        {
         int ok=0;

         if(hq->Miniport)
         {
           if(s->Miniport->Port)
           {
            if(s->ServiceGroup)
            {
              s->my_isr();
              ok=1;
            }
           }
         }

          if(!ok)
          {
           debug(DERR,"!! timer_func: magic=wave_streamHQ; invalid pointers\n");
          }
        }
        else
        if(g->asio_magic==GSIF_MAGIC)
        {
        	g->asio_notify(LLA_NOTIFY_TIMER,LLA_NOTIFY_GSIF);
        }
         else
         if(g->asio_magic==ASIO_MAGIC)
         {
         	g->asio_notify(LLA_NOTIFY_TIMER,LLA_NOTIFY_ASIO);
         }
         else
          // ASIO_MAGIC not supported - doesn't need timer
          debug(DERR,"timer_notify: unknown magic [%x; %x]\n",data,g->asio_magic);

      } else debug(DERR,"timer_notify: NULL data\n");

      return;
 }

 if(what==LLA_NOTIFY_SYSTEM)
 {
     debug(DERR,"timer_notify: system event\n");
     return;
 }

 if(what!=LLA_NOTIFY_FULL && what!=LLA_NOTIFY_HALF)
 {
    debug(DERR,"timer_notify: invalid event [%d]\n",what);
    return;
 }

 CMiniportWaveOutStream *s=(CMiniportWaveOutStream *)data;

 // debug(-1,"- per-voice notify [%d %d]\n",data,what);

 if(s)
 {
  if(s->asio_magic==ASIO_MAGIC)
  {
   s->asio_notify(what,LLA_NOTIFY_ASIO);
  }
   else 
  {
   CGSIFInterface *g=(CGSIFInterface *)data;
   if(g)
   {
    if(g->asio_magic==GSIF_MAGIC)
    {
     g->asio_notify(what,LLA_NOTIFY_GSIF);
     return;
    }
    else
     debug(DERR,"!! invalid notify func [%d %d]\n",data,what);
   }
  }
 } else debug(DERR,"!! invalid per-voice data [%x %x]\n",data,what);
}

#pragma code_seg()
void get_physical_func(void *call_with,kx_voice_buffer *buff,int offset,__int64 *aa)
{
 PHYSICAL_ADDRESS a=MmGetPhysicalAddress(((byte *)buff->addr)+offset);
 *aa=a.QuadPart;
}

#pragma code_seg()
int debug_func(int where,const char *__format, ... )
{
#ifdef KX_DEBUG
 if(where>=0)
 {
    char my_internal_buf[2048];
    va_list ap;
    va_start(ap, __format);
    
    if(_vsnprintf(my_internal_buf, 2048, __format, ap))
    {
     switch(where)
     {
      case DERR:
        if(strstr(my_internal_buf,"!!!")==0)
         DbgPrint("!!! kX Error: ");
        else
         DbgPrint("kX Error: ");
        break;
      case DKX3D:
        DbgPrint("kx_3d: ");
        break;
      case DLIB:
      	DbgPrint("kx_lib: ");
      	break;
      case DAC3:
      	DbgPrint("kx_ac3: ");
      	break;
      case DPROP:
      	DbgPrint("kx_prop: ");
      	break;
      case DSTATE:
      	DbgPrint("kx_state: ");
      	break;
      case DDB:
      	DbgPrint("kx_db: ");
      	break;
      case DASIO:
      	DbgPrint("kx_asio: ");
      	break;
      case DGSIF:
      	DbgPrint("kx_gsif: ");
      	break;
      case DSFNT:
      	DbgPrint("kx_sf: ");
      	break;
      case DWDM:
        DbgPrint("kx_wdm: ");
      	break;
      case DMIDI:
      	DbgPrint("kx_midi: ");
      	break;
      case DBUFF:
        DbgPrint("kx_buff: ");
        break;
      case DPV:
      	DbgPrint("kx_p16v: ");
      	break;
      default:
      	// ??? Unknown debug flag
      	DbgPrint("kx_UNKN: ");
      	break;
      case DNONE:
        // nothing
        break;
      }
      DbgPrint(my_internal_buf);
    } else DbgPrint("!!! too long debug string\n");
    va_end(ap);
 }
#endif
 return 0;
}

#if (NTDDI_VERSION >= NTDDI_WIN2K)
extern "C"
NTKERNELAPI
__out_bcount_opt (NumberOfBytes) PVOID
MmAllocateContiguousMemorySpecifyCache (
    __in SIZE_T NumberOfBytes,
    __in PHYSICAL_ADDRESS LowestAcceptableAddress,
    __in PHYSICAL_ADDRESS HighestAcceptableAddress,
    __in_opt PHYSICAL_ADDRESS BoundaryAddressMultiple,
    __in MEMORY_CACHING_TYPE CacheType
    );
#endif

#pragma code_seg()
int pci_alloc(void *this_,struct memhandle *m,kx_cpu_cache_type_t cache_type)
{
 PHYSICAL_ADDRESS highest,lowest,boundary;

 highest.LowPart=0x7fffffff; // 2Gb limit
 highest.HighPart=0x0;

 lowest.LowPart=0x0;
 lowest.HighPart=0x0;

 boundary.LowPart=0;
 boundary.HighPart=0;

 MEMORY_CACHING_TYPE cache=MmNonCached;
 if(cache_type==KX_CACHED)
  cache=MmCached;
 else
  if(cache_type==KX_NONCACHED)
   cache=MmNonCached;

 m->addr=MmAllocateContiguousMemorySpecifyCache(m->size,lowest,highest,boundary,cache);
 if(m->addr)
 {
  m->dma_handle=(dword )(MmGetPhysicalAddress((void *)m->addr)).LowPart;
  RtlZeroMemory((void *)m->addr,m->size);
  return 0;
 } 
  else 
 {
  m->addr=0;
  m->dma_handle=0;
  m->size=0;
 }
 return -10;
}

#pragma code_seg()
void pci_free(void *this_,struct memhandle *m)
{
 if(m->addr)
 {
   MmFreeContiguousMemory((void *)m->addr);
   m->addr=0;
   m->dma_handle=0;
   m->size=0;
 }
}

#undef kx_lock_acquire
#undef kx_lock_release

#define DPC_SPINLOCK		0x1
#define REG_SPINLOCK		0x2
// #define IRQ_SPINLOCK		0x4
#define DPC_SPINLOCK_IRQ	0x29938570	// for IRQ

#pragma code_seg()
KX_API(void,kx_lock_acquire(kx_hw *hw,spinlock_t *l, unsigned long *irq,const char *file,int line))
{
 KIRQL level=KeGetCurrentIrql();

      if(level==DISPATCH_LEVEL)
      {
       KeAcquireSpinLockAtDpcLevel((PKSPIN_LOCK)&l->spin_lock);

       if(l->method)
       {
          debug(DERR,"spinlock error: nested spinlock! [%s %d] m:%d (2/DPC)\n",file?file:"(null)",line,l->method);
          #ifdef KX_DEBUG
            debug(DERR,"!!! was @ %s %d [%s]\n",l->file?l->file:"NULL",l->line,l->name);
          #endif
       }

       l->method|=DPC_SPINLOCK;
       *irq=DPC_SPINLOCK_IRQ;
      }
      else
       if(level<DISPATCH_LEVEL)
       {
        KeAcquireSpinLock((PKSPIN_LOCK)&l->spin_lock,(PKIRQL)irq);

        if(l->method)
        {
          debug(DERR,"spinlock error: nested spinlock! [%s %d] m:%d (1/REG)\n",file?file:"(null)",line,l->method);
          #ifdef KX_DEBUG
            debug(DERR,"!!! was @ %s %d [%s]\n",l->file?l->file:"NULL",l->line,l->name);
          #endif
        }

        l->method|=REG_SPINLOCK;
       }
       else
        if(level>DISPATCH_LEVEL)
        {
         debug(DERR,"internal spinlock error: invalid spinlock mode (%s)\n",l->name);
         #ifdef KX_DEBUG
            debug(DERR,"!!! was @ %s %d [%s]\n",l->file?l->file:"NULL",l->line,l->name);
            debug(DERR,"!!! is @ %s %d [%s]\n",file?file:"NULL",line,l->name);
         #endif
        }

 l->kx_lock++;

 if(l->kx_lock>1)
 {
  debug(DERR,"!!! BEWARE Spin_lock: %s (%d) -- irql: %d winlock: %d\n",file?file:"NULL",line,level,l->spin_lock);
  #ifdef KX_DEBUG
    debug(DERR,"!!! was @ %s %d\n",l->file?l->file:"NULL",l->line);
  #endif
 }

#ifdef KX_DEBUG
 l->file=file;
 l->line=line;
#endif
}

#pragma code_seg()
KX_API(void,kx_lock_release(kx_hw *hw, spinlock_t *l, unsigned long *irq,const char *file,int line))
{
 if(l->kx_lock==0)
  debug(DERR," !! INTERNAL CODE ERROR = incorrect spin_lock (=0) : %s (%d)\n",file?file:"NULL",line);
 else
  l->kx_lock--;

 if(l->kx_lock)
 {
  debug(DERR," !! releasing incorrect spin_lock (was not 1:%d) : %s (%d)\n",l->kx_lock,file?file:"NULL",line);
  #ifdef KX_DEBUG
    debug(DERR,"!!! was @ %s %d\n",l->file?l->file:"NULL",l->line);
  #endif
 }
 else
 {
  l->line=0;
  l->file=0;
 }

 KIRQL level=KeGetCurrentIrql();

       if(level!=DISPATCH_LEVEL)
       {
        debug(DERR,"spinlock error: running @ %d IRQl for generic '%s' spinlock!\n",
         level,l->name);
        debug(DERR,"spinlock: at %s %d\n",file?file:"NULL",line);
        #ifdef KX_DEBUG
          debug(DERR,"!!! was @ %s %d\n",l->file?l->file:"NULL",l->line);
        #endif
       }

       if(l->method&DPC_SPINLOCK)
       {
        if(l->method&REG_SPINLOCK)
         debug(DERR,"spinlock error: DPC spinlock being released, while regular spinlock pending! [%d]\n",l->method);

        if(*irq==DPC_SPINLOCK_IRQ)
        {
          // OK
          l->method&=(~DPC_SPINLOCK);
          KeReleaseSpinLockFromDpcLevel((PKSPIN_LOCK)&l->spin_lock);
        }
        else
        {
          debug(DERR,"spinlock error: DPC spinlock invalid [bad irq: %x]; %s %d\n",*irq,l->name,l->method);
          l->method&=(~REG_SPINLOCK);
          KeReleaseSpinLock((PKSPIN_LOCK)&l->spin_lock,(KIRQL)(*irq));
        }
       }
       else
       {
        if(l->method&REG_SPINLOCK)
        {
         if(*irq!=DPC_SPINLOCK_IRQ)
         {
           // OK
           l->method&=(~REG_SPINLOCK);
           KeReleaseSpinLock((PKSPIN_LOCK)&l->spin_lock,(KIRQL)(*irq));
         }
         else
         {
          debug(DERR,"spinlock error: reg. spinlock invalid [should have been DPC]; %s %d %d\n",l->name,l->method,level);
          l->method&=(~DPC_SPINLOCK);
          KeReleaseSpinLockFromDpcLevel((PKSPIN_LOCK)&l->spin_lock);
         }
        }
       }
}

#pragma code_seg()
KX_API(void,kx_spin_lock_init(kx_hw *hw, spinlock_t *l,const char *name))
{
 memset(l,0,sizeof(spinlock_t));
 KeInitializeSpinLock((PKSPIN_LOCK)&l->spin_lock);

 l->name=name;
}

#pragma code_seg()
void save_fpu_state(kx_fpu_state *state)
{
 KeSaveFloatingPointState((PKFLOATING_SAVE)state);
}

#pragma code_seg()
void rest_fpu_state(kx_fpu_state *state)
{
 KeRestoreFloatingPointState((PKFLOATING_SAVE)state);
}

#pragma code_seg()
void CAdapterCommon::ResetSettings(kx_callbacks *cb)
{
 kx_defaults(hw,cb);
}


#pragma code_seg()
int CAdapterCommon::my_init(dword device,dword subsys,byte chip_rev,
      byte bus,byte dev,byte func)
{
 if(!hw)
 {
  kx_callbacks cb;

  cb.call_with=this;

  cb.irql=(byte)m_irq;
  cb.io_base=(word)m_io_base;

  cb.device=device;
  cb.subsys=subsys;
  cb.chip_rev=chip_rev;
  cb.bus=bus;
  cb.dev=dev;
  cb.func=func;

  cb.send_message=&send_message;
  cb.debug_func=&debug_func;
  cb.malloc_func=&malloc_func;
  cb.free_func=&free_func;
  cb.pci_alloc=&pci_alloc;
  cb.pci_free=&pci_free;
  cb.get_physical=&get_physical_func;
  cb.save_fpu_state=&save_fpu_state;
  cb.rest_fpu_state=&rest_fpu_state;
  cb.notify_func=&notify_func;
  cb.sync=&SynchronizedHandler;
  cb.lmem_alloc_func=&lmem_alloc_func;
  cb.lmem_free_func=&lmem_free_func;
  cb.lmem_get_addr_func=&lmem_get_addr_func;
  cb.usleep=&usleep;

  ResetSettings(&cb);

  int ret=kx_init(&hw,&cb,0); // not standalone
  if(ret)
  {
   debug(DWDM,"!!! kx_init() failed (%d)\n",ret);
   if(hw)
   {
    kx_close(&hw);
    hw=NULL;
   }
   return ret;
  }

  ResetSettings(&hw->cb); // necessary in order to fill-in hw-dependent parts...

 } else debug(DWDM,"!!! Double init called with hw set\n");
 return 0;
}

