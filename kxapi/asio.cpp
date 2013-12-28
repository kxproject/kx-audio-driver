// kX Driver Interface
// Copyright (c) Eugene Gavrilov, 2001-2005.
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


#include "stdafx.h"
#include "vers.h"

#include "..\kstream\kscommon.h"
CKSUSER  *KSUSER=NULL;

// #define USE_DSOUND_ASIO

// ASIO
int iKX::init_kstream(void)
{
    if(KSUSER==0)
     KSUSER=new CKSUSER;

    int ret=0;

    // pins/filters
    pRenderEnumerator=0;
    pRenderFilter=0;
    pRenderPin=0;

    HRESULT hr=0;

    int n_chn=2;
    int sr=48000;
    int bps=16;
    int valid_bps=16;

    // enumerate audio renderers
    pRenderEnumerator = new CKsEnumFilters(&hr);
    if((pRenderEnumerator==NULL)||!SUCCEEDED(hr))
    {
        debug("!!asio: Failed to allocate CKsEnumFilters [%x]\n",GetLastError());
        ret=-1;
        goto CLEANUP;
    }

    GUID  cats_render[] = { STATIC_KSCATEGORY_AUDIO, STATIC_KSCATEGORY_RENDER };

    hr=pRenderEnumerator->EnumFilters(
                eAudRen,            // create audio render filters ...
                cats_render,      // ... of these categories
                2,                  // There are 2 categories
                TRUE,               // While you're at it, enumerate the pins
                FALSE,              // ... but don't bother with nodes
                TRUE                // Instantiate the filters
            );

    if(!SUCCEEDED(hr))
    {
        debug("asio: CKsEnumFilters::EnumFilters failed [%x]\n",GetLastError());

        ret=-2;
        goto CLEANUP;
    }

    LISTPOS pos=pRenderEnumerator->m_listFilters.GetHeadPosition();

    int found=0;
    while(pRenderEnumerator->m_listFilters.GetNext(pos,(CKsFilter **)&pRenderFilter))
    {
         if(pRenderFilter)
         {
             debug("asio: checking '%s' vs '%s'...\n",pRenderFilter->m_szFilterName,wave_path);

             if(strstr(pRenderFilter->m_szFilterName,"kx_wave0")!=0 &&
                strcmp(pRenderFilter->m_szFilterName,wave_path)==NULL)
             {
              found=1;
              break;
             }
         }
         else
          debug("!!asio: pRenderFilter is NULL\n");
    }

    if(found==0)
    {
         debug("!!asio: init_device(): no compatible device found [render filters not found]\n");

         ret=-3;
         goto CLEANUP;
    }

    // pRenderFilter is OK

    // instantiate the RENDER pin
    // use WAVEFORMATEXTENSIBLE to describe wave format
    WAVEFORMATEXTENSIBLE wfx;

    wfx.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
    wfx.Format.nChannels = (WORD)n_chn;
    wfx.Format.nSamplesPerSec = sr;

    wfx.Format.wBitsPerSample = (WORD) bps;
    wfx.Samples.wValidBitsPerSample = (WORD) valid_bps;
    wfx.Format.nBlockAlign = wfx.Format.nChannels * wfx.Format.wBitsPerSample / 8;
                      
    wfx.Format.nAvgBytesPerSec = wfx.Format.nSamplesPerSec * wfx.Format.nBlockAlign;
    
    wfx.Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE)-sizeof(WAVEFORMATEX);

    wfx.dwChannelMask=0;

    for(int t=0;t<n_chn;t++)
      wfx.dwChannelMask|=(1<<t);

    wfx.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
    
    debug("asio: trying to open pb %dchn/%dHz/%d(%d)bits...\n",
       wfx.Format.nChannels,wfx.Format.nSamplesPerSec,wfx.Format.wBitsPerSample,wfx.Samples.wValidBitsPerSample);

    pRenderPin = pRenderFilter->CreateRenderPin(&wfx.Format, FALSE);
    if(pRenderPin)
    {
      debug("asio: pb pin opened successfully [%d channels; pin_id: %d]\n",n_chn,pRenderPin->GetId());

      // dump pin data
      debug("asio: pb pin: fmt sz: %d fl: %d samplesz: %d\nasio: pb pin: tag: %d nchn: %d nSmpl/sec: %d nAvg: %d block: %d bps: %d cb: %d\n",
                         pRenderPin->m_pksDataFormatWfx->DataFormat.FormatSize,
                         pRenderPin->m_pksDataFormatWfx->DataFormat.Flags,
                         pRenderPin->m_pksDataFormatWfx->DataFormat.SampleSize,
                         pRenderPin->m_pksDataFormatWfx->WaveFormatEx.wFormatTag,
                         pRenderPin->m_pksDataFormatWfx->WaveFormatEx.nChannels,
                         pRenderPin->m_pksDataFormatWfx->WaveFormatEx.nSamplesPerSec,
                         pRenderPin->m_pksDataFormatWfx->WaveFormatEx.nAvgBytesPerSec,
                         pRenderPin->m_pksDataFormatWfx->WaveFormatEx.nBlockAlign,
                         pRenderPin->m_pksDataFormatWfx->WaveFormatEx.wBitsPerSample,
                         pRenderPin->m_pksDataFormatWfx->WaveFormatEx.cbSize);
    }
    else
     ret=-11;

CLEANUP:

    return ret;
}

int iKX::asio_alloc(int fl,int chn,void **addr,int size,int rate,int bps) // >=0 - ok;
{
 if((!asio_inited) || (!asio_iks_property))
 {
  debug("!! asio_alloc: asio_init=0\n");
  return -100;
 }

 if(((fl==ASIO_INPUT) && chn>=MAX_ASIO_INPUTS) ||
    ((fl==ASIO_OUTPUT) && chn>=MAX_ASIO_OUTPUTS) ||
    (addr==NULL) ||
    (size<=0) ||
    (chn<0) ||
    (asio_mem_table[chn+(fl==ASIO_INPUT?0:MAX_ASIO_INPUTS)]!=0))
 {
  debug("!! asio_alloc: bad args: flag=%d chn=%d; size=%d rate: %d\n",fl,chn,size,rate);
  return -102;
 }

 // allocate
 if(asio_method&KXASIO_METHOD_OLD)
 {
   *addr=VirtualAlloc(NULL,size,MEM_COMMIT,PAGE_READWRITE);
   if(*addr==NULL)
   {
    debug("!! asio_alloc: virtual alloc failed [%x]\n",GetLastError());
    return -104;
   }

   if(VirtualLock(*addr,size)==0)
   {
    debug("!! asio_alloc: virtual lock failed [%x]\n",GetLastError());
    VirtualFree(*addr,0,MEM_RELEASE);
    return -105;
   }
 }
 else
 {
   *addr=0;
 }

 int ret;
 int ret_b;
 asio_property a; memset(&a,0,sizeof(a));
 a.cmd=(fl==ASIO_INPUT)?KX_ASIO_ALLOC_INPUT:KX_ASIO_ALLOC_OUTPUT;
 a.chn=chn;
 a.size=size;
 a.addr=*addr;
 a.rate=rate;
 a.bps=bps;
 a.magic=KX_ASIO_MAGIC;
 ret=driver_property(asio_iks_property,KX_PROP_GET|KX_PROP_ASIO,&a,sizeof(a),&ret_b);
 if(ret==0)
 {
        if(a.cmd<0) // failed
        {
          if(asio_method&KXASIO_METHOD_OLD)
           VirtualFree(*addr,0,MEM_RELEASE);

          ret=a.cmd;
          return ret;
        }
        else 
        {
         // everything's ok:
         if(asio_method&KXASIO_METHOD_NEW)
           *addr=(void*)a.addr;

         memset(*addr,0,size);
         asio_mem_table[chn+(fl==ASIO_INPUT?0:MAX_ASIO_INPUTS)]=*addr;

         if(fl==ASIO_OUTPUT)
          last_asio_voice=a.cmd;
        }
 }
  else 
 {
  debug("!! asio_alloc: driver_property failed [%x]; ret=%x\n",GetLastError(),ret);
  if(asio_method&=KXASIO_METHOD_OLD)
    VirtualFree(*addr,0,MEM_RELEASE);
 }
 return ret;
}

int iKX::asio_free(int fl,void *addr)
{
 if( (!asio_inited) || (!asio_iks_property))
 {
  debug("!! asio_free: not inited\n");
  return -100;
 }

 int i=0;
 int found=0;

 // find 'voice'
 if(fl==ASIO_INPUT)
 {
  for(i=0;i<MAX_ASIO_INPUTS;i++)
   if(asio_mem_table[i]==addr)
   {
    found=1;
    break;
   }
 }
 else
 if(fl==ASIO_OUTPUT)
 {
  for(i=MAX_ASIO_INPUTS;i<MAX_ASIO_INPUTS+MAX_ASIO_OUTPUTS;i++)
   if(asio_mem_table[i]==addr)
   {
     found=1;
     break;
   }
 }
 else 
 {
  debug("asio_free: invalid address/flag: %x %x\n",fl,addr);
  return -204;
 }

 if(!found) // bad addr
 {
  debug("asio_free: address not found [%x %x]\n",fl,addr);
  strcpy(error_name,"Trying to 'Free' unallocated ASIO buffer");
  return -201;
 }

 int ret=0;
 if(fl==ASIO_OUTPUT) // outputs only
 {
    int ret_b;
    asio_property a; memset(&a,0,sizeof(a));
    a.cmd=KX_ASIO_FREE_OUTPUT;
    a.chn=i-MAX_ASIO_INPUTS;
    a.magic=KX_ASIO_MAGIC;
    a.addr=asio_mem_table[i];

    ret=driver_property(asio_iks_property,KX_PROP_GET|KX_PROP_ASIO,&a,sizeof(a),&ret_b);
 }
 else
 if(fl==ASIO_INPUT)
 {
    int ret_b;
    asio_property a; memset(&a,0,sizeof(a));
    a.cmd=KX_ASIO_FREE_INPUT;
    a.chn=i;
    a.magic=KX_ASIO_MAGIC;
    a.addr=asio_mem_table[i];

    ret=driver_property(asio_iks_property,KX_PROP_GET|KX_PROP_ASIO,&a,sizeof(a),&ret_b);
 }

 if(asio_method&KXASIO_METHOD_OLD)
 {
  if(VirtualFree(asio_mem_table[i],0,MEM_RELEASE)==0) // error
  {
    debug("asio_free: virtual free failed [%x]\n",GetLastError());
    ret=-(int)GetLastError();
  }
 }
 asio_mem_table[i]=NULL;
 return ret;
}

int iKX::asio_start(void)
{
 if((!asio_inited) || (!asio_iks_property))
 {
  debug("asio_start: not inited\n");
  return -100;
 }

 int ret;
 int ret_b;
 asio_property a; memset(&a,0,sizeof(a));
 a.cmd=KX_ASIO_START;
 a.magic=KX_ASIO_MAGIC;

 ret=driver_property(asio_iks_property,KX_PROP_GET|KX_PROP_ASIO,&a,sizeof(a),&ret_b);
 return ret;
}

int iKX::asio_get_position(void)
{
 if((!asio_inited) || (!asio_iks_property))
 {
  debug("asio_start: not inited\n");
  return -100;
 }

 int ret;
 int ret_b;
 asio_property a; memset(&a,0,sizeof(a));
 a.cmd=KX_ASIO_GET_POSITION;
 a.magic=KX_ASIO_MAGIC;

 ret=driver_property(asio_iks_property,KX_PROP_GET|KX_PROP_ASIO,&a,sizeof(a),&ret_b);
 if(ret==0)
 {
  return a.position;
 }
 return ret;
}

int iKX::asio_stop(void)
{
 if((!asio_inited) || (!asio_iks_property))
 {
  debug("asio_stop: not inited\n");
  return -100;
 }

 int ret;
 int ret_b;
 asio_property a; memset(&a,0,sizeof(a));
 a.cmd=KX_ASIO_STOP;
 a.magic=KX_ASIO_MAGIC;

 ret=driver_property(asio_iks_property,KX_PROP_GET|KX_PROP_ASIO,&a,sizeof(a),&ret_b);
 return ret;
}

int iKX::asio_init(int method_)
{
 if(asio_inited==0)
 {
  // set kX part
  for(int i=0;i<MAX_ASIO_OUTPUTS+MAX_ASIO_INPUTS;i++)
  {
    asio_mem_table[i]=NULL;
  }
  // set HW part
  last_asio_voice=-1;
  asio_method=method_;

  // init DirectX part...
  #ifdef USE_DSOUND_ASIO
  HRESULT hr;
  LPGUID dx8=get_dsound_guid(KX_WINMM_WAVE);
  
  if(DirectSoundCreate8(dx8,&asio_ds,NULL)!=DS_OK)
  {
    strcpy(error_name,"Error initializing kX ASIO interface (DS)");
        debug("asio_init: error initializing kX ASIO interface\n");
    goto FAILED_ERROR;
  }
  if(asio_ds->SetCooperativeLevel(GetTopWindow(NULL),DSSCL_PRIORITY)!=DS_OK)
  {
    strcpy(error_name,"Error setting kX ASIO cooperative level");
        debug("asio_init: error setting cooperative level\n");
    goto FAILED_ERROR;  
  }

  // Get the primary buffer 
  DSBUFFERDESC dsbd;
  ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
  dsbd.dwSize        = sizeof(DSBUFFERDESC);
  dsbd.dwFlags       = DSBCAPS_PRIMARYBUFFER;
  dsbd.dwBufferBytes = 0;
  dsbd.lpwfxFormat   = NULL;

  if( FAILED( hr = asio_ds->CreateSoundBuffer( &dsbd, &asio_prim_buffer, NULL ) ) )
  {
    strcpy(error_name,"Error initializing primary kX ASIO buffer");
        debug("asio_init: error initializing primary kX ASIO buffer\n");
    goto FAILED_ERROR;
  }

  WAVEFORMATEX wfx;
  ZeroMemory( &wfx, sizeof(WAVEFORMATEX) ); 
  wfx.wFormatTag      = WAVE_FORMAT_PCM; 
  wfx.nChannels       = (WORD) 2;
  wfx.nSamplesPerSec  = 48000; 
  wfx.wBitsPerSample  = (WORD) 16; 
  wfx.nBlockAlign     = (WORD)(wfx.wBitsPerSample / 8 * wfx.nChannels);
  wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

  if( FAILED( hr = asio_prim_buffer->SetFormat(&wfx) ) )
  {
    strcpy(error_name,"Error setting primary kX ASIO buffer format");
        debug("asio_init: error setting primary kX asio buffer format\n");
    goto FAILED_ERROR;
  }

  ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
  dsbd.dwSize        = sizeof(DSBUFFERDESC);
  dsbd.dwFlags = DSBCAPS_LOCHARDWARE | DSBCAPS_GLOBALFOCUS;
  dsbd.dwBufferBytes = 4096;

  WAVEFORMATEX format;
  format.cbSize=sizeof(format);
  format.nChannels=2;
  format.nSamplesPerSec=48000;
  format.wBitsPerSample=16;
  format.nBlockAlign=(WORD)(format.nChannels*format.wBitsPerSample/8);
  format.nAvgBytesPerSec=format.nSamplesPerSec*wfx.nBlockAlign;
  format.wFormatTag=1; // PCM
  dsbd.lpwfxFormat     = &format;

  if( FAILED( hr = asio_ds->CreateSoundBuffer( &dsbd, &asio_secondary_buffer, NULL ) ) )
  {
    strcpy(error_name,"Error initializing secondary kX ASIO buffer");
        debug("asio_init: error initializing secondary kX ASIO buffer\n");
    goto FAILED_ERROR;
  }

  asio_secondary_buffer->QueryInterface(IID_IKsPropertySet,(void **)&asio_iks_property);
  if(!asio_iks_property)
  {
       strcpy(error_name,"Failed to query for kX ASIO property");
           debug("asio_init: failed to query for kX ASIO property\n");
       goto FAILED_ERROR;
  }
  #else
   // initialize asio_iks_property
   if(init_kstream()==0)
   {
    asio_iks_property=(IKsPropertySet *)pRenderPin->GetHandle();
   }
   else
    goto FAILED_ERROR;
  #endif

  asio_user_event=::CreateEvent(NULL, 0, 0, NULL);

  int ret;
  int ret_b;
  asio_property a; memset(&a,0,sizeof(a));

  a.cmd=KX_ASIO_INIT;
  a.chn=asio_method;
  a.magic=KX_ASIO_MAGIC;
  a.user_event=asio_user_event;
  ret=driver_property(asio_iks_property,KX_PROP_GET|KX_PROP_ASIO,&a,sizeof(a),&ret_b);
  if(ret)
   goto FAILED_ERROR;

  asio_inited=1;
  if(a.notification)
   asio_notification=a.notification;

  return 0;

  FAILED_ERROR:

  // clean-up:
  #ifdef USE_DSOUND_ASIO
  if(asio_iks_property)
  {
    asio_iks_property->Release();
    asio_iks_property=0;
  }
  if(asio_secondary_buffer)
  {
    asio_secondary_buffer->Release();
    asio_secondary_buffer=0;
  }
  if(asio_prim_buffer)
  {
    asio_prim_buffer->Release();
    asio_prim_buffer=0;
  }
  if(asio_ds)
  {
    asio_ds->Release();
    asio_ds=NULL;
  }
  #else
   // clean-up
   if(pRenderPin)
   {
    pRenderPin->ClosePin();
    pRenderPin=0;
   }

   if(pRenderEnumerator)
   {
    delete pRenderEnumerator;
    pRenderEnumerator=0;
    pRenderFilter=0;
    pRenderPin=0;
   }
   asio_iks_property=0;

   if(KSUSER)
   {
    delete KSUSER;
    KSUSER=0;
   }
  #endif
 }

 // else:
 return -5;
}

int iKX::asio_close(void)
{
 if(!asio_inited)
 {
  debug("asio_close: not inited\n");
  return -100;
 }

  for(int i=0;i<MAX_ASIO_INPUTS;i++) 
  {
   if(asio_mem_table[i])
     asio_free(ASIO_INPUT,asio_mem_table[i]);
  }
  for(int i=0;i<MAX_ASIO_OUTPUTS;i++)
  {
   if(asio_mem_table[i+MAX_ASIO_INPUTS])
     asio_free(ASIO_OUTPUT,asio_mem_table[i+MAX_ASIO_INPUTS]);
  }

  // hw part
  last_asio_voice=-1;

  if(asio_iks_property)
  {
   int ret;
   int ret_b;
   asio_property a; memset(&a,0,sizeof(a));
   a.notification=asio_notification;
   a.cmd=KX_ASIO_CLOSE;
   a.magic=KX_ASIO_MAGIC;
   ret=driver_property(asio_iks_property,KX_PROP_GET|KX_PROP_ASIO,&a,sizeof(a),&ret_b);

   asio_notification=NULL;
  }

  // free directX part
  // clean-up:
  #ifdef USE_DSOUND_ASIO
  if(asio_iks_property)
  {
    asio_iks_property->Release();
    asio_iks_property=0;
  }
  if(asio_secondary_buffer)
  {
    asio_secondary_buffer->Release();
    asio_secondary_buffer=0;
  }
  if(asio_prim_buffer)
  {
    asio_prim_buffer->Release();
    asio_prim_buffer=0;
  }
  if(asio_ds)
  {
    asio_ds->Release();
    asio_ds=NULL;
  }
  #else
   // free asio_iks_property
   // clean-up
   if(pRenderPin)
   {
    pRenderPin->ClosePin();
    pRenderPin=0;
   }

   if(pRenderEnumerator)
   {
    delete pRenderEnumerator;
    pRenderEnumerator=0;
    pRenderFilter=0;
    pRenderPin=0;
   }

   asio_iks_property=0;

   if(KSUSER)
   {
    delete KSUSER;
    KSUSER=0;
   }
  #endif

  asio_inited=0;
  return 0;
}

int iKX::asio_wait(int dbuffer,volatile bool *done)
{
 if(!asio_inited)
 {
  debug("kxasio: asio_wait: not inited\n");
  return -100;
 }

 if(asio_method&KXASIO_METHOD_WAIT_IN_KERNEL)
 {
       int ret;
       int ret_b;
       asio_property a; memset(&a,0,sizeof(a));
       a.cmd=KX_ASIO_WAIT;
       a.rate=dbuffer;
       a.ptr=done;
       a.magic=KX_ASIO_MAGIC;
       ret=driver_property(asio_iks_property,KX_PROP_GET|KX_PROP_ASIO,&a,sizeof(a),&ret_b);
       if(ret==0)
       {
              return a.rate;
       }
       else
       {
        *done=1;
        debug("kxasio: asio_wait: kernel wait failed. stop asio\n");
       }
       return 0;
 }
 else // in user-level
 {
       int cur;

       if((asio_method&KXASIO_METHOD_MAP_TOGGLE) && asio_notification)
         cur=asio_notification->toggle;
       else
         cur=asio_get_position();

       if((dbuffer==0) && (cur==1))
         return 1;
       if((dbuffer==1) && (cur==0))
         return 1;

       return 0;
 }
}

int iKX::asio_timer(int setget,int value,int &new_value)
{
 if(!asio_inited)
 {
  debug("kxasio: asio_wait: not inited\n");
  return -100;
 }

 int ret;
 int ret_b;
 asio_property a; memset(&a,0,sizeof(a));
 a.cmd=KX_ASIO_TIMER;
 a.rate=setget;
 a.chn=value;
 a.magic=KX_ASIO_MAGIC;
 ret=driver_property(asio_iks_property,KX_PROP_GET|KX_PROP_ASIO,&a,sizeof(a),&ret_b);
 if(ret==0)
 {
    new_value=a.chn;
        return a.rate;
 }
 else
  return -1;
}

int iKX::get_asio_method()
{
 if(asio_notification)
  return asio_notification->asio_method;
 else
 {
  debug("kxasio: invalid parameter for get_asio_method()!\n");
  return 0;
 }
}

HANDLE iKX::get_asio_event()
{
 return asio_user_event;
}

