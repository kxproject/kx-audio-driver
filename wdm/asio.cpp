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
#include "gsif/gsif2.h"
#include "gsif/kxgsif.h"

extern "C" NTKERNELAPI PHYSICAL_ADDRESS MmGetPhysicalAddress (__in PVOID BaseAddress);

// #define USE_PAGED_POOL

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

static void *MapMemory(int size,VOID *buffer, PMDL &mdl,MEMORY_CACHING_TYPE cache)
{
    PVOID userVAToReturn;

    // Allocate and initalize an MDL that describes the buffer
    mdl = IoAllocateMdl(buffer,size,FALSE,FALSE,NULL);

    if(!mdl)  
    {
        return(NULL);
    }

    // Finish building the MDL -- Fill in the "page portion" 
    MmBuildMdlForNonPagedPool(mdl);

    // MmProbeAndLockPages(mdl,UserMode,IoModifyAccess);

    mdl->MdlFlags|=MDL_MAPPING_CAN_FAIL;

    __try 
    {
      userVAToReturn=MmMapLockedPagesSpecifyCache(mdl,UserMode,cache,NULL,FALSE,NormalPagePriority);
    }
     __except(EXCEPTION_EXECUTE_HANDLER)
    {
      userVAToReturn=NULL;
    }

    // If we get NULL back, the request didn't work.
    // I'm thinkin' that's better than a bugcheck anyway.
    if(!userVAToReturn)  
    {
        IoFreeMdl(mdl);        
        mdl=0;
        return(NULL);
    }

    return(userVAToReturn);
} 


static void *CreateAndMapMemory(int size,PVOID &buffer, PMDL &mdl,MEMORY_CACHING_TYPE cache)
{
    size=(size/PAGE_SIZE+1)*PAGE_SIZE;
 
    // Allocate a buffer to share with the application
#if defined(USE_PAGED_POOL)
    buffer = ExAllocatePoolWithTag(NonPagedPool,size,'saXk');
#else
    PHYSICAL_ADDRESS highest,lowest,boundary;

    highest.LowPart=0x7fffffff; // 2Gb limit
    highest.HighPart=0x0;

    lowest.LowPart=0x0;
    lowest.HighPart=0x0;

    boundary.LowPart=0;
    boundary.HighPart=0;

    buffer=MmAllocateContiguousMemorySpecifyCache(size,lowest,highest,boundary,cache);
    RtlZeroMemory(buffer,size);
#endif

    if(!buffer)
    {
      debug(DWDM,"Failed to allocate %d bytes of memory !\n",size);
      return(NULL);
    }

    void *ret=MapMemory(size,buffer,mdl,cache);
    if(ret==0)
    {
     debug(DWDM,"Failed to map %d bytes of memory !\n",size);
     ExFreePool(buffer);
     buffer=NULL;
     mdl=0;
    }

    return ret;
} 

int CASIO::asio_init(int asio_method_)
{
  if( !(asio_method_&KXASIO_METHOD_OLD) && !(asio_method_&KXASIO_METHOD_NEW) )
  {
   debug(DWDM,"kx asio (kernel): incorrect param for asio_init(%d)\n",asio_method_);
   return -2;
  }

  if(asio_hw->asio_notification_krnl.asio_method || is_asio)
  {
   debug(DWDM,"kx asio (kernel): asio already initialized(%d)\n",asio_method_);
   return -2;
  }

  asio_latency=-480; // 10ms = 480samples
  asio_bps=-1;

  asio_hw->asio_notification_krnl.pb_buf=-1;
  asio_hw->asio_notification_krnl.rec_buf=-1;
  asio_hw->asio_notification_krnl.toggle=-1;
  asio_hw->asio_notification_krnl.asio_method=asio_method_;
  asio_hw->asio_notification_krnl.n_voice=-1;
  asio_hw->asio_notification_krnl.semi_buff=0;
  asio_hw->asio_notification_krnl.active=0;
  asio_hw->asio_notification_krnl.cur_pos=0;
  asio_hw->asio_notification_krnl.kevent=0; // not initialized, since it is assigned by property.cpp

  asio_notification_user=(asio_notification_t *)MapMemory(sizeof(asio_notification_t),&asio_hw->asio_notification_krnl,asio_notification_mdl,MmCached);
  // this is software-only buffer, cache it freely (MmCached)

  is_asio=1;

  return 0;
}

int CASIO::asio_alloc_output(int chn,size_t size,void **addr,int rate,int bps)
{
   if(asio_bps==-1)
    asio_bps=bps;

   if(asio_bps!=bps)
   {
    debug(DWDM,"!! kxasio: cannot mix '%d' and '%d' streams\n",bps,asio_bps);
    return -2;
   }

   if(!is_asio)
   {
    debug(DWDM,"!! kxasio (kernel): not initialized\n");
    return -2;
   }

   if(bps==32)
   {
    if(!asio_hw->can_passthru)
    {
        debug(DWDM,"!!! asio error: 24/32 not supported on your card\n");
        return -1;
    }
    if(chn<0 || chn>=MAX_ASIO_OUTPUTS/2)
    {
        debug(DWDM,"!!! asio error: alloc_output: chn=%d\n",chn);
        return -1;
    }
   }
   else
   {
    if(chn>=MAX_ASIO_OUTPUTS || chn<0)
    {
     debug(DWDM,"!!! asio error: alloc_output: chn=%d\n",chn);
     return -1;
    }
   }

   void *user_addr=*addr,*kernel_addr=0;
   PMDL asio_mdl=0;

   if(asio_hw->asio_notification_krnl.asio_method&KXASIO_METHOD_NEW)
   {
    // we should allocate the buffer ourselves
    user_addr=CreateAndMapMemory(size,kernel_addr,asio_mdl,MmNonCached); // for hardware output use MmNonCached
    if(user_addr)
    {
      *addr=user_addr;
    }
    else
    {
     debug(DWDM,"!!! asio error: alloc_output: error allocating memory...\n");
     return -2;
    }
   }

   kx_voice_buffer buffer;
   buffer.size=size;
   PHYSICAL_ADDRESS a;
   a=MmGetPhysicalAddress(*addr);
   buffer.physical=a.LowPart;
   buffer.addr=*addr;
   buffer.that=that_;
   buffer.notify=-asio_latency; // if negative - then in samples, not 'ms'

   int first=1;

   if(asio_magic==GSIF_MAGIC)
   {
     if(bps!=16)
     {
       debug(DWDM,"'%d' bps not supported for GSIF at the moment\n",bps);
       return -3;
     }
   }

   {
     for(int t=0;t<KX_NUMBER_OF_VOICES;t++)
       if(asio_hw->voicetable[t].asio_id==that_)
       { first=0; break; } // already have one
   }

   int need_notifier=VOICE_OPEN_NOTIMER; // this will not create timer by default

   if(first)
   {
      // note: gsif requires generic timer notification[s]
      // (only for the first gsif voice)
      if(asio_hw->asio_notification_krnl.asio_method&KXASIO_METHOD_USE_HWIRQ)
       need_notifier=VOICE_OPEN_NOTIFY; // [half/full notification]
      else
       if(asio_hw->asio_notification_krnl.asio_method&KXASIO_METHOD_USE_HWTIMER)
        need_notifier=0; // this will create timer
   }

   int i=-1;

   i=kx_allocate_multichannel(asio_hw,bps,rate,need_notifier,&buffer,DEF_ASIO_ROUTING+chn);

   if(i>=0) // success?
   {
               asio_hw->voicetable[i].asio_id=that_;
               asio_hw->voicetable[i].asio_mdl=asio_mdl;
               asio_hw->voicetable[i].asio_user_addr=user_addr;
               asio_hw->voicetable[i].asio_kernel_addr=kernel_addr;

               asio_hw->voicetable[i].asio_channel=chn;

               if(first)
               {
                 asio_hw->asio_notification_krnl.n_voice=i;
                 asio_hw->asio_notification_krnl.semi_buff=
                   (asio_hw->voicetable[i].param.endloop-asio_hw->voicetable[i].param.startloop)/2;
               }
   }

   return i;
}

int CASIO::asio_alloc_input(int chn,size_t size,void **addr,int rate,int bps)
{
   if(!is_asio)
   {
    debug(DWDM,"kxasio (kernel): not initialized\n");
    return -2;
   }

   if(asio_bps==-1)
    asio_bps=bps;

   if(asio_bps!=bps)
   {
    debug(DWDM,"kxasio: cannot mix '%d' and '%d' streams\n",bps,asio_bps);
    return -2;
   }

   asio_hw->cur_asio_in_bps=bps;

   asio_physical_descr_t *ins=0;

   if(chn>=MAX_ASIO_INPUTS || chn<0)
   {
        debug(DWDM,"!!! asio error: alloc_input: chn=%d\n",chn);
        return -1;
   }
   ins=asio_hw->asio_inputs;

   if(ins[chn].kernel_addr)
   {
            debug(DWDM,"!!! asio: alloc_output: channel already used (%d)\n",chn);
            return -1;
   }

   dword fxwc=0;
   int cnt=0;

   for(int i=0;i<MAX_ASIO_INPUTS;i++)
   {
    if(asio_hw->asio_inputs[i].kernel_addr)
    {
     cnt++;

     if(asio_hw->can_passthru)
     {
      fxwc|=1<<(i<<1);
      fxwc|=1<<((i<<1)+1);
     }
     else
      fxwc|=1<<i;

     if(asio_hw->asio_inputs[i].size!=size)
     {
      debug(DWDM,"!!! asio: incompatible latencies between clients! (%d; %d)\n",
       size,asio_hw->asio_inputs[i].size);
      return -1;
     }
    }
   }

   if((cnt+1)*size > (size_t)(65536/(asio_hw->can_passthru?2:1)))
   {
      debug(DWDM,"!!! asio: too HIGH latency for too many channels: cnt: %d, size: %d, pt: %d, chn: %d %p\n",
            cnt,size,asio_hw->can_passthru,chn,size);
      return -1;
   }

   cnt++;

   if(asio_hw->can_passthru)
   {
    fxwc|=(1<<(chn<<1));
    fxwc|=(1<<((chn<<1)+1));
   }
   else
    fxwc|=(1<<(chn));

        void *user_addr=*addr,*kernel_addr=0;
        PMDL asio_mdl=0;

        if(asio_hw->asio_notification_krnl.asio_method&KXASIO_METHOD_NEW)
        {
         // we should allocate the buffer ourselves
         user_addr=CreateAndMapMemory(size,kernel_addr,asio_mdl,MmCached);
           // for hardware input processed by software (interleaving), use MmCached

         if(user_addr)
         {
           *addr=user_addr;
         }
         else
         {
          debug(DWDM,"!!! asio error: alloc_input: error allocating memory...\n");
          return -2;
         }
         ins[chn].mdl=asio_mdl;
        }
        else
        {
         ins[chn].mdl=IoAllocateMdl(*addr,size,FALSE,FALSE,NULL);
        }

        PHYSICAL_ADDRESS a;
        a=MmGetPhysicalAddress(addr);
        ins[chn].physical=a.LowPart;

        void *tmp_addr;

        if(asio_hw->asio_notification_krnl.asio_method&KXASIO_METHOD_OLD)
        {
              if(ins[chn].mdl)
              {
                MmProbeAndLockPages((PMDL)ins[chn].mdl,UserMode,IoModifyAccess);
                ((PMDL)ins[chn].mdl)->MdlFlags|=MDL_MAPPING_CAN_FAIL;

                __try 
                {
                 tmp_addr=MmMapLockedPagesSpecifyCache((PMDL)ins[chn].mdl,KernelMode,MmCached,NULL,FALSE,NormalPagePriority);
                 // for hardware input processed by software (interleaving), use MmCached
                } 
                 __except(EXCEPTION_EXECUTE_HANDLER)
                {
                  tmp_addr=NULL;
                }

                if(tmp_addr==0)
                {
                    debug(DWDM,"!!! asio: alloc_input(): MmMapLockedPages() failed\n");
                    MmUnlockPages((PMDL)ins[chn].mdl);
                        IoFreeMdl((PMDL)ins[chn].mdl);
                    return -1;
                }
              }
               else 
              {
                debug(DWDM,"!!! asio: alloc_input(): IoAllocateMdl failed\n");
                return -1;
              }
        }
        else
         tmp_addr=kernel_addr;

        debug(DASIO,"asio: input [%d]: addr:%p ph: %x size:%d va=%x [fl=%x]\n",
                chn,tmp_addr,a.LowPart,size,((PMDL)ins[chn].mdl)->MappedSystemVa,((PMDL)ins[chn].mdl)->MdlFlags);

        ins[chn].magic=that_;
        ins[chn].size=size;

        // as fast as possible:
        if(asio_hw->can_passthru)
        {
         if(bps==32)
          asio_hw->mtr_buffer_size=size*cnt;
         else
          asio_hw->mtr_buffer_size=(size*2)*cnt;
        }
        else
        {
         asio_hw->mtr_buffer_size=size*cnt;
        }

        kx_mtrec_select(asio_hw,fxwc);

        ins[chn].kernel_addr=tmp_addr;
        ins[chn].user_addr=*addr;

        debug(DASIO,"asio: alloc_input(2) (chn=%d; addr=%p; size=%d); total#=%d bps: %d memsize=%d fxwc=%x\n",
          chn,addr,size,cnt,bps,asio_hw->mtr_buffer_size,fxwc);

        return chn; // ok
}

int CASIO::asio_free_output(int asio_chn)
{
   if(!is_asio)
   {
    debug(DWDM,"kxasio (kernel): not initialized\n");
    return -2;
   }

  int ret=-1;

  if((asio_bps==16) || (asio_bps==32))
  {
  for(int i=0;i<KX_NUMBER_OF_VOICES;i++)
  {
   if((asio_hw->voicetable[i].asio_id==that_) &&
      (asio_hw->voicetable[i].asio_channel==(dword)asio_chn))
   {
    if(asio_hw->voicetable[i].usage&VOICE_USAGE_ASIO) // asio voice
    {
      debug(DASIO,"asio: free_output (chn=%d; hw=%d)\n",asio_chn,i);
      ret=kx_wave_close(asio_hw,i);
      asio_hw->voicetable[i].asio_id=0;
      asio_hw->voicetable[i].asio_channel=0xffffffff;

      // free memory
      if(asio_hw->asio_notification_krnl.asio_method&KXASIO_METHOD_NEW)
      {
       if(asio_hw->voicetable[i].asio_user_addr && asio_hw->voicetable[i].asio_mdl)
       {
        MmUnmapLockedPages(asio_hw->voicetable[i].asio_user_addr, (PMDL)asio_hw->voicetable[i].asio_mdl);
        asio_hw->voicetable[i].asio_user_addr=0;
       }
       if(asio_hw->voicetable[i].asio_mdl)
       {
        IoFreeMdl((PMDL)asio_hw->voicetable[i].asio_mdl);
        asio_hw->voicetable[i].asio_mdl=0;
       }
       if(asio_hw->voicetable[i].asio_kernel_addr)
       {
        #if defined(USE_PAGED_POOL)
        ExFreePool(asio_hw->voicetable[i].asio_kernel_addr);
        #else
        MmFreeContiguousMemory(asio_hw->voicetable[i].asio_kernel_addr);
        #endif
        asio_hw->voicetable[i].asio_kernel_addr=0;
       }
      }

      return 0;
    }
    else
    {
     debug(DWDM,"!!! asio: trying to free non-asio voice! (%d)\n",asio_chn);
     return -2;
    }
   }
  }
  }
  else
  {
   debug(DWDM,"!! asio: cannot free %d-bit voice [%d]\n",asio_bps,asio_chn);
   return -5;
  }

  debug(DWDM,"!!! asio: free output voice %d?\n",asio_chn);
  return -3;
}

int CASIO::asio_free_input(int chn)
{
   if(!is_asio)
   {
    debug(DWDM,"kxasio (kernel): not initialized\n");
    return -2;
   }

   asio_physical_descr_t *ins=0;

   if(chn>=MAX_ASIO_INPUTS || chn<0)
   {
        debug(DWDM,"!!! asio error: free_input: chn=%d\n",chn);
        return -1;
   }
   ins=asio_hw->asio_inputs;

   if(ins[chn].magic!=that_)
   {
       debug(DWDM,"!!! asio: free_output: channel used by other client (%d) - bad magic\n",chn);
       return -2;
   }

   void *kernel_addr=ins[chn].kernel_addr;
   void *user_addr=ins[chn].user_addr;

   // before anything else!
   ins[chn].kernel_addr=0;
   ins[chn].user_addr=0;
   ins[chn].physical=0;
   ins[chn].magic=0;
   ins[chn].size=0;

                if(ins[chn].mdl)
                {
                 if(asio_hw->asio_notification_krnl.asio_method&KXASIO_METHOD_OLD)
                 {
                   MmUnmapLockedPages(kernel_addr,(PMDL)ins[chn].mdl);
                   MmUnlockPages((PMDL)ins[chn].mdl);
                 }
                 else
                   MmUnmapLockedPages(user_addr,(PMDL)ins[chn].mdl);
                 
                 IoFreeMdl((PMDL)ins[chn].mdl);

                 if(asio_hw->asio_notification_krnl.asio_method&KXASIO_METHOD_NEW)
                  #if defined(USE_PAGED_POOL)
                  ExFreePool(kernel_addr);
                  #else
                  MmFreeContiguousMemory(kernel_addr);
                  #endif
                }

                int cnt=0;
                dword fxwc=0;
                dword size=0;
                for(int i=0;i<MAX_ASIO_INPUTS;i++)
                {
                 if(asio_hw->asio_inputs[i].kernel_addr)
                 {
                  cnt++;
                  if(!asio_hw->can_passthru)
                   fxwc|=1<<i;
                  else
                  {
                   fxwc|=1<<(i<<1);
                   fxwc|=1<<((i<<1)+1);
                  }
                  size=asio_hw->asio_inputs[i].size;
                 }
                }

                // update FXWC and mtr_buffer_size
                if(asio_hw->can_passthru)
                {
                 if(asio_hw->cur_asio_in_bps==32)
                  asio_hw->mtr_buffer_size=size*cnt;
                 else
                  asio_hw->mtr_buffer_size=(size*2)*cnt;
                }
                else
                {
                 asio_hw->mtr_buffer_size=size*cnt;
                }

                kx_mtrec_select(asio_hw,fxwc);
                debug(DASIO,"asio: after free_output (chn=%d); total#=%d req. memsize=%d fxwc=%x\n",chn,cnt,asio_hw->mtr_buffer_size,fxwc);

                return 0; // ok
}

int CASIO::asio_close()
{
   if(!is_asio)
   {
    debug(DWDM,"kxasio (kernel): not initialized\n");
    return -2;
   }

   asio_stop();

   int cnt=0;
   for(int i=0;i<KX_NUMBER_OF_VOICES;i++)
   {
         if( (asio_hw->voicetable[i].usage&VOICE_USAGE_ASIO) &&
             (asio_hw->voicetable[i].asio_id==that_))
         {
          cnt++;
          asio_free_output(asio_hw->voicetable[i].asio_channel);
         }
   }

   if(cnt)
   {
         if(asio_magic!=GSIF_MAGIC)
          debug(DWDM,"!! NB! ASIO output was closed incorrectly (cleanup)\n");
   }

   // free ASIO rec, too...
   for(int i=0;i<MAX_ASIO_INPUTS;i++)
   {
         if(asio_hw->asio_inputs[i].kernel_addr && (asio_hw->asio_inputs[i].magic==that_))
         {
           debug(DWDM,"!! NB! ASIO input was closed incorrectly (cleanup)\n");
           kx_mtrec_stop(asio_hw);
           break;
         }
   }

   for(int i=0;i<MAX_ASIO_INPUTS;i++)
   {
          if(asio_hw->asio_inputs[i].kernel_addr && (asio_hw->asio_inputs[i].magic==that_))
          {
            asio_free_input(i);
          }
   }

   if(asio_notification_user && asio_notification_mdl)
   {
         MmUnmapLockedPages(asio_notification_user, asio_notification_mdl);
         asio_notification_user=0;
   }

   if(asio_notification_mdl)
   {
         IoFreeMdl(asio_notification_mdl);
         asio_notification_mdl=0;
   }

        asio_hw->asio_notification_krnl.pb_buf=-1;
        asio_hw->asio_notification_krnl.rec_buf=-1;
        asio_hw->asio_notification_krnl.toggle=-1;
        asio_hw->asio_notification_krnl.asio_method=0;
        asio_hw->asio_notification_krnl.n_voice=-1;
        asio_hw->asio_notification_krnl.semi_buff=0;
        asio_hw->asio_notification_krnl.active=0;
        asio_hw->asio_notification_krnl.cur_pos=0;

        if(asio_hw->asio_notification_krnl.kevent)
        {
            ObDereferenceObject(asio_hw->asio_notification_krnl.kevent); // accepts PVOID, input is (PRKEVENT)
            asio_hw->asio_notification_krnl.kevent=0;
        }

        return 0;
}

int CASIO::asio_notify(int event,int nterface)
{
 if(asio_hw)
 {
    if(nterface==LLA_NOTIFY_GSIF) // handles 'buffer_position'
    {
     // gsif
     if(asio_magic==GSIF_MAGIC)
     {
      CGSIFInterface *gsif=(CGSIFInterface *)this;
      if(gsif->fnGSIFIsr && gsif->pContext)
      {
              switch(event)
              {
/*
                   // not used for GSIF

                   case LLA_NOTIFY_FULL:
                       // process only 'full buffer' case
                       debug(DGSIF,"-notify-[%x/%x]\n",gsif->buffer_position,gsif->buffer_size);
                       // gsif->buffer_position+=gsif->buffer_size;
                       break;
                   case LLA_NOTIFY_HALF:
                       // half: nothing (10k2 only)
                       break;
*/
                   case LLA_NOTIFY_TIMER:
                        // two methods:
                        dword bp1;
                        
                        bp1=asio_latency*2;
                            // dword bp2=asio_get_position(1);

                        //debug(DGSIF,"-timer-[%x/%x]\n",gsif->buffer_position+bp1,
                        //                                  gsif->buffer_size);
                        gsif->buffer_position+=bp1;

                        gsif->fnGSIFIsr(gsif->buffer_position,gsif->pContext);
                        break;
                   default:
                        debug(DERR,"invalid notification [%d %d]\n",event,nterface);
                        break;
                 }
      } else debug(DERR,"ISR: %x Context: %x\n",gsif->fnGSIFIsr,gsif->pContext);
     } else debug(DERR,"invalid magic [%x] (GSIF)\n",asio_magic);
    }
    else
    if(nterface==LLA_NOTIFY_ASIO)
    {
      if(asio_magic==ASIO_MAGIC)
      {
/*
        // signaled elsewhere
        switch(event)
        {
              case LLA_NOTIFY_FULL:
                asio_hw->asio_notification_krnl.pb_buf=0;
                break;
              case LLA_NOTIFY_HALF:
                asio_hw->asio_notification_krnl.pb_buf=1;
                break;
              case LLA_NOTIFY_TIMER:
                // nop
                // (kx_get_asio_position(,1) will set pb_buf)
                break;
              default:
                debug(DERR,"asio notification (should not happen): %d\n",event);
                return 0;
        }
*/
        if(event!=LLA_NOTIFY_TIMER)
        {
          debug(DERR,"asio notification (should not happen): %d\n",event);
          return 0;
        }

        int old_toggle=asio_hw->asio_notification_krnl.toggle;
        asio_hw->asio_notification_krnl.toggle=kx_get_asio_position(asio_hw,1); // re-read current pb_buf

        if(asio_hw->asio_notification_krnl.kevent && (asio_hw->asio_notification_krnl.asio_method&KXASIO_METHOD_SEND_EVENT)
           && old_toggle!=asio_hw->asio_notification_krnl.toggle)
          KeSetEvent((PRKEVENT)asio_hw->asio_notification_krnl.kevent,0,FALSE);
      }
      else
       debug(DERR,"invalid magic [%x] (ASIO)\n",asio_magic);  
    }
     else debug(DERR,"invalid notification interface! [%d/%d]\n",event,nterface);
 } else debug(DERR,"asio_hw==0 in gsif\n");
 return 0;
}

int CASIO::asio_get_position(int mode)
{
  if(!is_asio)
  {
   debug(DWDM,"kxasio (kernel): not initialized\n");
   return -2;
  }

  int ret=kx_get_asio_position(asio_hw,1); // read current pb_buf

  if(mode) // GSIF? return number of samples
   return asio_hw->asio_notification_krnl.cur_pos;

  return ret;
}

int CASIO::asio_start()
{
   int ret=0;
   if(!is_asio)
   {
    debug(DWDM,"kxasio (kernel): not initialized\n");
    return -2;
   }

       dword low=0,high=0,first=-1;

       for(int i=0;i<KX_NUMBER_OF_VOICES;i++)
       {
        if( (asio_hw->voicetable[i].usage&VOICE_USAGE_ASIO) &&
            (asio_hw->voicetable[i].asio_id==that_) ) // asio voice
        {
         if(i>=32)
          high|=(1<<(i-32));
         else
          low|=(1<<i);
         if(first==-1)
          first=i;
        }
       }

       if(first!=-1)
       {
         debug(DASIO,"asio start voice %x\n",low,high);
         kx_wave_start_multiple(asio_hw,first,low,high);
       }
       else
       {
         debug(DWDM,"!! note: nothing to start/stop [asio_start, 723]\n");
         ret=-1;
       }
       
       kx_mtrec_start(asio_hw);

       asio_hw->asio_notification_krnl.active=1;

       return ret;
}

int CASIO::asio_stop()
{
   int ret=0;

   if(!is_asio)
   {
    debug(DWDM,"kxasio (kernel): not initialized\n");
    return -2;
   }

       asio_hw->asio_notification_krnl.active=0;

       dword low=0,high=0,first=-1;

       for(int i=0;i<KX_NUMBER_OF_VOICES;i++)
       {
        if( (asio_hw->voicetable[i].usage&VOICE_USAGE_ASIO) &&
            (asio_hw->voicetable[i].asio_id==that_) ) // asio voice
        {
         if(i>=32)
          high|=(1<<(i-32));
         else
          low|=(1<<i);
         if(first==-1)
          first=i;
        }
       }
       if(first!=-1)
       {
         debug(DASIO,"asio stop voice %x %x\n",low,high);
         kx_wave_stop_multiple(asio_hw,first,low,high);

         // zero sample position, too:
         for(int i=0;i<32;i++)
         {
            if(low&(1<<i))
            {
              kx_wave_set_position(asio_hw,i,0);
            }
         }
         for(int i=0;i<32;i++)
         {
            if(high&(1<<i))
            {
              kx_wave_set_position(asio_hw,i+32,0);
            }
         }
        }
        else
        {
           ret=-1;
        }
    
        kx_mtrec_stop(asio_hw);
        return ret;
}


int CASIO::asio_wait(int toggle,volatile bool *done)
{
 if(!is_asio)
 {
  debug(DWDM,"kxasio (kernel): not initialized\n");
  return -2;
 }

 LARGE_INTEGER delay;
 delay.QuadPart=-10000;   // wait 10000*100us (1ms) relative

 while(asio_hw->asio_notification_krnl.active && done && (*done==0))
 {
       //  kx_get_asio_position() returns current hardware buffer
       if(kx_get_asio_position(asio_hw,1)==(1-toggle)) // re-read current pb_buf
        return 1; // time to switch

       // wait
       if(asio_hw->asio_notification_krnl.asio_method&KXASIO_METHOD_SLEEP)
        KeDelayExecutionThread(KernelMode,FALSE,&delay);

       if((asio_hw->asio_notification_krnl.asio_method&KXASIO_METHOD_SEND_EVENT) && asio_hw->asio_notification_krnl.kevent)
       {
        // 3551 change (was different PRKEVENT* typecast)
        int ret=KeWaitForSingleObject((PRKEVENT)asio_hw->asio_notification_krnl.kevent,Executive,KernelMode,FALSE,&delay);
        if(ret!=STATUS_SUCCESS)
        {
         debug(DERR,"failed to wait for krnl event [%08x]\n",ret);
         break;
        }
       }
 }

 return 0;
}
