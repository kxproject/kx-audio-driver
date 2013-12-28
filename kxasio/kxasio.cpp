// kX ASIO Driver
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

#include "interface/kxmixer.h"

static DWORD __stdcall ASIOThread_ (void *param);

CLSID IID_ASIO_DRIVER[8] = 
{
 // {5C0B12B1-F582-4935-BFAE-85267A6BE13D}
 { 0x5c0b12b1, 0xf582, 0x4935, { 0xbf, 0xae, 0x85, 0x26, 0x7a, 0x6b, 0xe1, 0x3d } },
 // 5771C802-034B-4ac1-BE54-36DADD447207
 { 0x5771C802, 0x034B, 0x4ac1, { 0xBE, 0x54, 0x36, 0xDA, 0xDD, 0x44, 0x72, 0x07 } },
 // 8A1080D5-A695-498a-8188-2F0B57870A7F
 { 0x8A1080D5, 0xA695, 0x498a, { 0x81, 0x88, 0x2F, 0x0B, 0x57, 0x87, 0x0A, 0x7F } },
 // 6732CDC3-569B-4541-9060-D7290050E7A3
 { 0x6732CDC3, 0x569B, 0x4541, { 0x90, 0x60, 0xD7, 0x29, 0x00, 0x50, 0xE7, 0xA3 } },
 // FEDE17F2-7108-4c64-9801-1627B75ACFC9
 { 0xFEDE17F2, 0x7108, 0x4c64, { 0x98, 0x01, 0x16, 0x27, 0xB7, 0x5A, 0xCF, 0xC9 } },
 // 2B156F48-B8EF-4a1e-95DF-E384E8B64842
 { 0x2B156F48, 0xB8EF, 0x4a1e, { 0x95, 0xDF, 0xE3, 0x84, 0xE8, 0xB6, 0x48, 0x42 } },
 // 2D98497D-A875-481d-A3D3-7A159642101F
 { 0x2D98497D, 0xA875, 0x481d, { 0xA3, 0xD3, 0x7A, 0x15, 0x96, 0x42, 0x10, 0x1F } },
 // 66649E8B-9864-43ee-AE93-558E90D58BB3
 { 0x66649E8B, 0x9864, 0x43ee, { 0xAE, 0x93, 0x55, 0x8E, 0x90, 0xD5, 0x8B, 0xB3 } }
};

CFactoryTemplate g_Templates[9] = {
    {L"kXASIO", &IID_ASIO_DRIVER[0], KXAsio::CreateInstance0},
    {L"kXASIO", &IID_ASIO_DRIVER[1], KXAsio::CreateInstance1},
    {L"kXASIO", &IID_ASIO_DRIVER[2], KXAsio::CreateInstance2},
    {L"kXASIO", &IID_ASIO_DRIVER[3], KXAsio::CreateInstance3}, 
    {L"kXASIO", &IID_ASIO_DRIVER[4], KXAsio::CreateInstance4},
    {L"kXASIO", &IID_ASIO_DRIVER[5], KXAsio::CreateInstance5},
    {L"kXASIO", &IID_ASIO_DRIVER[6], KXAsio::CreateInstance6},
    {L"kXASIO", &IID_ASIO_DRIVER[7], KXAsio::CreateInstance7}  
};
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);

CUnknown* KXAsio::CreateInstance0(LPUNKNOWN pUnk, HRESULT *phr)
{
    return (CUnknown*)new KXAsio (pUnk,phr,0);
};

CUnknown* KXAsio::CreateInstance1(LPUNKNOWN pUnk, HRESULT *phr)
{
    return (CUnknown*)new KXAsio (pUnk,phr,1);
};

CUnknown* KXAsio::CreateInstance2(LPUNKNOWN pUnk, HRESULT *phr)
{
    return (CUnknown*)new KXAsio (pUnk,phr,2);
};

CUnknown* KXAsio::CreateInstance3(LPUNKNOWN pUnk, HRESULT *phr)
{
    return (CUnknown*)new KXAsio (pUnk,phr,3);
};

CUnknown* KXAsio::CreateInstance4(LPUNKNOWN pUnk, HRESULT *phr)
{
    return (CUnknown*)new KXAsio (pUnk,phr,4);
};

CUnknown* KXAsio::CreateInstance5(LPUNKNOWN pUnk, HRESULT *phr)
{
    return (CUnknown*)new KXAsio (pUnk,phr,5);
};

CUnknown* KXAsio::CreateInstance6(LPUNKNOWN pUnk, HRESULT *phr)
{
    return (CUnknown*)new KXAsio (pUnk,phr,6);
};

CUnknown* KXAsio::CreateInstance7(LPUNKNOWN pUnk, HRESULT *phr)
{
    return (CUnknown*)new KXAsio (pUnk,phr,7);
};

STDMETHODIMP KXAsio::NonDelegatingQueryInterface (REFIID riid, void ** ppv)
{
 for(int i=0;i<8;i++)
    if (riid == IID_ASIO_DRIVER[i])
    {
        return GetInterface (this, ppv);
    }
    return CUnknown::NonDelegatingQueryInterface (riid, ppv);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//      Register ASIO Driver
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
extern LONG RegisterAsioDriver (CLSID,char *,char *,char *,char *);
extern LONG UnregisterAsioDriver (CLSID,char *,char *);

//
// Server registration, called on REGSVR32.EXE "the dllname.dll"
//
HRESULT _declspec(dllexport) DllRegisterServer()
{
    LONG    rc;
    char    errstr[128];

    rc = RegisterAsioDriver (IID_ASIO_DRIVER[0],"kxasio.dll","kX ASIO Driver","kX ASIO","Apartment");
    rc = RegisterAsioDriver (IID_ASIO_DRIVER[1],"kxasio.dll","kX ASIO Driver 1","kX ASIO 1","Apartment");
    rc = RegisterAsioDriver (IID_ASIO_DRIVER[2],"kxasio.dll","kX ASIO Driver 2","kX ASIO 2","Apartment");
    rc = RegisterAsioDriver (IID_ASIO_DRIVER[3],"kxasio.dll","kX ASIO Driver 3","kX ASIO 3","Apartment");
    rc = RegisterAsioDriver (IID_ASIO_DRIVER[4],"kxasio.dll","kX ASIO Driver 4","kX ASIO 4","Apartment");
    rc = RegisterAsioDriver (IID_ASIO_DRIVER[5],"kxasio.dll","kX ASIO Driver 5","kX ASIO 5","Apartment");
    rc = RegisterAsioDriver (IID_ASIO_DRIVER[6],"kxasio.dll","kX ASIO Driver 6","kX ASIO 6","Apartment");
    rc = RegisterAsioDriver (IID_ASIO_DRIVER[7],"kxasio.dll","kX ASIO Driver 7","kX ASIO 7","Apartment");

    if(rc) 
    {
        sprintf(errstr,"Register Server failed ! (%d)",rc);
        MessageBox(0,(LPCTSTR)errstr,(LPCTSTR)"kX ASIO Driver",MB_OK|MB_ICONSTOP);
                debug("kxasio: register failed: error 0x%x\n",rc);
        return -1;
    }

    return S_OK;
}

//
// Server unregistration
//
HRESULT _declspec(dllexport) DllUnregisterServer()
{
    LONG    rc;
    char    errstr[128];

    rc = UnregisterAsioDriver (IID_ASIO_DRIVER[0],"kxasio.dll","kX ASIO Driver");
    rc = UnregisterAsioDriver (IID_ASIO_DRIVER[1],"kxasio.dll","kX ASIO Driver 1");
    rc = UnregisterAsioDriver (IID_ASIO_DRIVER[2],"kxasio.dll","kX ASIO Driver 2");
    rc = UnregisterAsioDriver (IID_ASIO_DRIVER[3],"kxasio.dll","kX ASIO Driver 3");
    rc = UnregisterAsioDriver (IID_ASIO_DRIVER[4],"kxasio.dll","kX ASIO Driver 4");
    rc = UnregisterAsioDriver (IID_ASIO_DRIVER[5],"kxasio.dll","kX ASIO Driver 5");
    rc = UnregisterAsioDriver (IID_ASIO_DRIVER[6],"kxasio.dll","kX ASIO Driver 6");
    rc = UnregisterAsioDriver (IID_ASIO_DRIVER[7],"kxasio.dll","kX ASIO Driver 7");

    if (rc) 
    {
                debug("kxasio: unregister failed\n");
        sprintf(errstr,"Unregister Server failed ! (%d)",rc);
        MessageBox(0,(LPCTSTR)errstr,(LPCTSTR)"kX ASIO Driver",MB_OK|MB_ICONSTOP);
        return -1;
    }

    return S_OK;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
KXAsio::KXAsio (LPUNKNOWN pUnk, HRESULT *phr,int device_num_)
    : CUnknown("kXASIO", pUnk, phr)
//------------------------------------------------------------------------------------------

{
    int i;

    ikx=NULL;
    done=false;

    // init defaults...
    set_defaults(0);

    ASIOThreadHandle=NULL;
    device_num=device_num_;

    active = false;
    started = false;
    timeInfoMode = false;
    tcRead = false;

    for (i = 0; i <  MAX_STREAMS; i++)
    {
        inputBuffers[i] = 0;
        inMap[i] = 0;
        inputBufferSizes[i]=0;
    }
    for (i = 0; i < MAX_STREAMS; i++)
    {
        outputBuffers[i] = 0;
        outMap[i] = 0;
        outputBufferSizes[i]=0;
    }

    inputLatency=blockFrames;
    outputLatency=blockFrames;

    callbacks = 0;
    activeInputs = activeOutputs = 0;
    toggle = 0;

    memset(&asioTime,0,sizeof(asioTime));
    milliSeconds = (long)((double)(blockFrames * 1000) / sampleRate);

    strcpy(errorMessage,"No kX ASIO Error");

    debug("kxasio: constructor [done]\n");
}

void KXAsio::recalc_latencies(int blockFrames_)
{
    if(blockFrames_!=0)
         blockFrames=blockFrames_;

    if(blockFrames<=0)
         blockFrames=384; // default latency;
    if(blockFrames>16384)
         blockFrames=16384;

    inputLatency = blockFrames;     // typically
    outputLatency = blockFrames;
    samplePosition = 0;

    milliSeconds = (long)((double)(blockFrames * 1000) / sampleRate);
}

void KXAsio::recalc_sr(int sr_)
{
    if(sr_!=0)
     sampleRate = (ASIOSampleRate)sr_;

    milliSeconds = (long)((double)(blockFrames * 1000) / sampleRate);
    asioTime.timeInfo.sampleRate = sampleRate;
}

//------------------------------------------------------------------------------------------
KXAsio::~KXAsio ()
{
//  timeEndPeriod(1);

    debug("kxasio: destructor\n");

    stop ();
    disposeBuffers ();

    if(ikx)
    {
     ikx->asio_close();
     ikx->close();
     delete ikx;
     ikx=NULL;
    }
}

//------------------------------------------------------------------------------------------
void KXAsio::getDriverName (char *name)
{
    strcpy(name,"kX ASIO ");
    if(ikx)
    {
        strcat(name,ikx->get_device_name());
    }
    else
    {
        strcat(name,KX_DRIVER_VERSION_STR);
    }
}

//------------------------------------------------------------------------------------------
long KXAsio::getDriverVersion ()
{
    return KX_ASIO_VERSION;
}

//------------------------------------------------------------------------------------------
void KXAsio::getErrorMessage (char *string)
{
    strcpy (string, errorMessage);
}

//------------------------------------------------------------------------------------------
ASIOBool KXAsio::init (void* )
{
    debug("kxasio: init\n");
    if (active)
    {
        debug("kxasio: init(): already inited\n");
        return true;
    }

    // pre-3536?
    LARGE_INTEGER counter_frequency;
    if(QueryPerformanceCounter(&counter_frequency)==0)
    {
         MessageBox(NULL,"Your hardware doesn't support High-resolution performance counter","kX ASIO Fatal Error",MB_OK|MB_ICONSTOP);
         return false;
    }

    strcpy (errorMessage,"kX ASIO Driver open failure");

    ikx=new iKX(); 
    if(ikx==NULL)
    {
     debug("kxasio: error creating iKX instance\n");
     return false;
    }

    if(ikx->get_version_dword()!=KX_VERSION_DWORD)
    {
     debug("kxasio: version mismatch\n");
     MessageBox(NULL,"Incorrect iKX API version. Reinstall kX Software","Error",MB_OK|MB_ICONSTOP);
     return false;
    }

    if(ikx->init(device_num))
    {
     ikx->close();
     debug("kxasio: error initializing iKX\n");
     strcpy (errorMessage,"kX Init error: ");
     strcat (errorMessage,ikx->get_error_string());
     debug("kxasio: ikx error: %s\n",errorMessage);
     return false;
    }

    // init defaults...
    set_defaults(0);

    int asio_method=get_config(KXASIO_ASIO_METHOD);

    debug("kxasio: using method: %08x\n",asio_method);
    if(ikx->asio_init(asio_method))
    {
     debug("kxasio: error calling asio_init() [m: %d]\n",asio_method);
     strcpy (errorMessage,"kX ASIO Init error: ");
     strcat (errorMessage,ikx->get_error_string());
     return false;
    }

    // configure using selected device here...
    set_defaults(0);

    int ttt;
    ttt=get_config(KXASIO_BPS); if(ttt!=-1) bps=ttt;
    ttt=get_config(KXASIO_LATENCY); if(ttt!=-1) blockFrames=ttt;
    ttt=get_config(KXASIO_FREQ); if(ttt!=-1) sampleRate=(ASIOSampleRate)ttt;
    ttt=get_config(KXASIO_N_REC_CHN); if(ttt!=-1) num_inputs=ttt;
    ttt=get_config(KXASIO_N_PB_CHN); if(ttt!=-1) num_outputs=ttt;

    recalc_latencies(blockFrames);
    recalc_sr((int)sampleRate);

    // save settings
    set_config(KXASIO_BPS,bps);
    set_config(KXASIO_LATENCY,blockFrames);
    set_config(KXASIO_FREQ,(int)sampleRate);
    set_config(KXASIO_N_REC_CHN,num_inputs);
    set_config(KXASIO_N_PB_CHN,num_outputs);
    
    active = true;
    debug("kxasio: init ok [dev:%d blFr:%d sr:%d in:%d out:%d]\n",device_num,
           blockFrames,(int)sampleRate,num_inputs,num_outputs);

    return true;
}

//------------------------------------------------------------------------------------------
ASIOError KXAsio::start ()
{
//  debug("kxasio: start()\n");
    if (callbacks)
    {
        started = false;
        samplePosition = 0;
        toggle = 0;

        // ----
        DWORD asioId;
        done = false;
        ASIOThreadHandle = CreateThread (0, 0, ASIOThread_, this, 0, &asioId);

        if(SetThreadPriority(ASIOThreadHandle,THREAD_PRIORITY_TIME_CRITICAL)==0)
        {
           debug("kxasio: error setting thread priority\n");
        }
        else
           debug("kxasio: thread priority set to critical\n");

        started = true;

        if(ikx)
         ikx->asio_start();
        // ----

        return ASE_OK;
    }
    return ASE_NotPresent;
}

//------------------------------------------------------------------------------------------
ASIOError KXAsio::stop ()
{
    if(started)
    {
//      debug("kxasio: stop()\n");
        started = false;

        // --- deactivate 'hardware'
        if(ikx)
         ikx->asio_stop();

        done = true;
        if (ASIOThreadHandle)
            WaitForSingleObject(ASIOThreadHandle, 3000);

        ASIOThreadHandle = 0;
        // ---

        // clean-up the buffers:
        for (int i = 0; i < MAX_STREAMS; i++)
        {
                if(inputBuffers[i])
                         memset(inputBuffers[i],0,inputBufferSizes[i]);
        }
        for (int i = 0; i < MAX_STREAMS; i++)
        {
                if(outputBuffers[i])
                         memset(outputBuffers[i],0,outputBufferSizes[i]);
        }
    }
    
    return ASE_OK;
}

//------------------------------------------------------------------------------------------
ASIOError KXAsio::getChannels (long *numInputChannels, long *numOutputChannels)
{
    *numInputChannels = num_inputs;
    *numOutputChannels = num_outputs;
    return ASE_OK;
}

//------------------------------------------------------------------------------------------
ASIOError KXAsio::getLatencies (long *_inputLatency, long *_outputLatency)
{
    *_inputLatency = inputLatency;
    *_outputLatency = outputLatency;
    return ASE_OK;
}

//------------------------------------------------------------------------------------------
ASIOError KXAsio::getBufferSize (long *minSize, long *maxSize,
    long *preferredSize, long *granularity)
{
    *minSize = *maxSize = *preferredSize = blockFrames;     // allow this size only
    *granularity = 0;
    return ASE_OK;
}

//------------------------------------------------------------------------------------------
ASIOError KXAsio::canSampleRate (ASIOSampleRate sampleRate_)
{
    if (sampleRate_ == sampleRate)      // allow these rates only
        return ASE_OK;
    return ASE_NoClock;
}

//------------------------------------------------------------------------------------------
ASIOError KXAsio::getSampleRate (ASIOSampleRate *sampleRate)
{
    *sampleRate = this->sampleRate;
    return ASE_OK;
}

//------------------------------------------------------------------------------------------
ASIOError KXAsio::setSampleRate (ASIOSampleRate sampleRate_)
{
    if (sampleRate_ != sampleRate)
    {
        debug("kxasio: changing sampling rate not permitted [%lf->%lf]\n",sampleRate_,this->sampleRate);
        return ASE_NoClock;
    }

    sampleRate = sampleRate_;
    asioTime.timeInfo.sampleRate = sampleRate_;
    asioTime.timeInfo.flags |= kSampleRateChanged;
    milliSeconds = (long)((double)(blockFrames * 1000) / this->sampleRate);
    // if (callbacks && callbacks->sampleRateDidChange)
    //    callbacks->sampleRateDidChange (this->sampleRate);
    // if (callbacks && callbacks->asioMessage)
    //    callbacks->asioMessage(kAsioResetRequest,0,0,0);
    return ASE_OK;
}

//------------------------------------------------------------------------------------------
ASIOError KXAsio::getClockSources (ASIOClockSource *clocks, long *numSources)
{
    // internal
    clocks->index = 0;
    clocks->associatedChannel = -1;
    clocks->associatedGroup = -1;
    clocks->isCurrentSource = ASIOTrue;
    strcpy(clocks->name, "kX Internal");
    *numSources = 1;
    return ASE_OK;
}

//------------------------------------------------------------------------------------------
ASIOError KXAsio::setClockSource (long index)
{
    if (!index)
    {
        asioTime.timeInfo.flags |= kClockSourceChanged;
        return ASE_OK;
    }
        debug("kxasio: setclocksource invalid [%d]\n",index);
    return ASE_NotPresent;
}

//------------------------------------------------------------------------------------------
ASIOError KXAsio::getChannelInfo (ASIOChannelInfo *info)
{
    if (info->channel < 0 || (info->isInput ? (info->channel >= num_inputs) : (info->channel >= num_outputs)))
    {
        debug("kxasio: invalid getChannelInfo call [%d %d]\n",info->isInput,info->channel);
        return ASE_InvalidParameter;
    }

    info->type = (bps==16)?ASIOSTInt16LSB:ASIOSTInt32LSB;
    info->channelGroup = info->channel/2;
    info->isActive = ASIOFalse;
    long i;
    if (info->isInput)
    {
        for (i = 0; i < activeInputs; i++)
        {
            if (inMap[i] == info->channel)
            {
                info->isActive = ASIOTrue;
                break;
            }
        }
    }
    else
    {
        for (i = 0; i < activeOutputs; i++)
        {
            if (outMap[i] == info->channel)
            {
                info->isActive = ASIOTrue;
                break;
            }
        }
    }
    sprintf(info->name, "%s %02d",info->isInput?"kX In":"kX Out",info->channel);
    return ASE_OK;
}

static int extend_memory(size_t size)
{
        // page commit
        SIZE_T minimum=0,maximum=0;
        if(GetProcessWorkingSetSize(GetCurrentProcess(),&minimum,&maximum))
        {
         debug("btw, min=%d max=%d\n",minimum/1024,maximum/1024);

         minimum+=size;
         if(maximum<minimum+size) maximum=minimum+size;
         debug("kxasio: extending working space by %d bytes [min=%d max=%d]...\n",size,minimum,maximum);
         if(SetProcessWorkingSetSize(GetCurrentProcess(),minimum,maximum)==0)
         {
           char tmp_str[128];
           sprintf(tmp_str,"Error increasing current process's virtual memory amount. You should have Administrator privileges. Error code: %x\n",GetLastError());
           MessageBox(NULL,tmp_str,"kX ASIO",MB_OK|MB_ICONSTOP);
           return ASE_NoMemory;
         } 
        } else debug("!! kxasio: GetProcessWorkingSetSize error\n",GetLastError());
        return 0;
}

static inline char *get_asio_error_string(int ret)
{
 switch(ret)
 {
  case -100:
    return "Internal Error: ASIO was not initialized correctly";
  case -102:
    return "Internal Error: invalid arguments for asio_alloc()";
  case -104:
    return "Internal Error: VirtualAlloc failed";
  case -105:
    return "VirtualLock failed ('-5' bug). Compact your memory and retry";
  case -40:
    return "Internal Error: incorrect DSound_property call";
  case -2:
    return "dsound_property LocalAlloc() failed";
  case E_INVALIDARG:
    return "E_INVALIDARG: Too big latency and too many channels -- resetting latency...";
  case E_NOTIMPL:   
    return "'Hardware acceleration' was not set to 'Full'. See readme.html for details";
  default:
    return "Unknown error";
 }
}

//------------------------------------------------------------------------------------------
ASIOError KXAsio::createBuffers (ASIOBufferInfo *bufferInfos, long numChannels,
    long bufferSize, ASIOCallbacks *callbacks)
{
    ASIOBufferInfo *info = bufferInfos;
    long i;
    bool notEnoughMem = false;

    activeInputs = 0;
    activeOutputs = 0;
    blockFrames = bufferSize;

    for (i = 0; i < numChannels; i++, info++)
    {
        if (info->isInput)
        {
            if (info->channelNum < 0 || info->channelNum >= (num_inputs))
                goto error;
            inMap[activeInputs] = info->channelNum;

            int ret;
AGAIN1:
            int cnt;
            for(cnt=0;cnt<3;cnt++)
            {
                ret=ikx->asio_alloc(ASIO_INPUT,info->channelNum,(void **)&inputBuffers[activeInputs],blockFrames * 2 * bps / 8,(int)sampleRate,bps);

                if (inputBuffers[activeInputs] && ret==0)
                {
                    info->buffers[0] = inputBuffers[activeInputs];
                    info->buffers[1] = (void *)((uintptr_t)inputBuffers[activeInputs] + (uintptr_t)(blockFrames*bps/8));
                    inputBufferSizes[activeInputs]=blockFrames * 2 * bps / 8;
                    memset(info->buffers[0],0,(blockFrames*bps/8));
                    memset(info->buffers[1],0,(blockFrames*bps/8));
                    break;
                }
                debug("!! kxasio: asio_alloc failed: input #%d [size=%d] ret=%d -- extending pool by %d bytes; GetLastError()=%x\n",
                  activeInputs,blockFrames*2*bps/8,ret,blockFrames*2*2*bps/8,GetLastError());
                if(ret==E_INVALIDARG)
                {
                 set_config(KXASIO_LATENCY,1024);
                 break;
                }
                extend_memory(blockFrames * 2 * 2 * bps / 8);
            }
            if(ret || (inputBuffers[activeInputs]==0))
            {
                char name[256];
                sprintf(name,"Error creating input buffer #%d: error = %d [size=%d]\nGetLastError()=%x\n\n%s\n\nRetry?",
                  activeInputs,ret,blockFrames*2*bps/8,GetLastError(),get_asio_error_string(ret));
                if(MessageBox(NULL,name,"kX ASIO",((ret==E_INVALIDARG)?MB_OK:MB_YESNO)|MB_ICONSTOP)==IDYES)
                 goto AGAIN1;
                info->buffers[0] = info->buffers[1] = 0;
                notEnoughMem = true;

                set_defaults(1);
                break;
            }
            activeInputs++;
            if (activeInputs > num_inputs)
            {
error:
                disposeBuffers();
                return ASE_InvalidParameter;
            }
        }
        else    // output           
        {
            if (info->channelNum < 0 || info->channelNum >= num_outputs)
                goto error;
            outMap[activeOutputs] = info->channelNum;
AGAIN2:
            int ret;
            for(int cnt=0;cnt<3;cnt++)
            {
                ret=ikx->asio_alloc(ASIO_OUTPUT,info->channelNum,(void **)&outputBuffers[activeOutputs],blockFrames * 2 * bps/8,(int)sampleRate,bps);

                if (outputBuffers[activeOutputs] && ret==0)
                {
                    info->buffers[0] = outputBuffers[activeOutputs];
                    info->buffers[1] = (void *)((uintptr_t)outputBuffers[activeOutputs] + (uintptr_t)blockFrames*bps/8);
                    outputBufferSizes[activeOutputs]=blockFrames * 2 * bps / 8;

                    memset(info->buffers[0],0,(blockFrames*bps/8));
                    memset(info->buffers[1],0,(blockFrames*bps/8));
                    break;
                }
                debug("kxasio: asio_alloc failed: output #%d [size=%d] ret=%d -- extending pool by %d bytes; GetLastError()=%x\n",
                  activeOutputs,blockFrames*2*bps/8,ret,blockFrames*2*2*bps/8,GetLastError());
                if(ret==E_INVALIDARG)
                {
                 set_config(KXASIO_LATENCY,1024);
                 break;
                }
                extend_memory(blockFrames * 2 * 2 * bps/8);
            }
            if(ret || (outputBuffers[activeOutputs]==0))
            {
                char name[256];
                sprintf(name,"Error creating output buffer #%d: error = %d [size=%d]\nGetLastError()=%x\n\n%s\n\nRetry?",
                  activeOutputs,ret,blockFrames*2*bps/8,GetLastError(),get_asio_error_string(ret));
                if(MessageBox(NULL,name,"kX ASIO",((ret==E_INVALIDARG)?MB_OK:MB_YESNO)|MB_ICONSTOP)==IDYES)
                 goto AGAIN2;
                info->buffers[0] = info->buffers[1] = 0;
                notEnoughMem = true;

                set_config(KXASIO_LATENCY,1024);
                set_config(KXASIO_BPS,16);

                int freq=48000; ikx->get_clock(&freq);

                set_config(KXASIO_FREQ,freq);
                set_config(KXASIO_N_PB_CHN,16);
                set_config(KXASIO_N_REC_CHN,16);
                break;
            }
            activeOutputs++;
            if (activeOutputs > num_outputs)
            {
                activeOutputs--;
                disposeBuffers();
                return ASE_InvalidParameter;
            }
        }
    }       
    if (notEnoughMem)
    {
        strcpy (errorMessage,"not enough virtual memory");
        disposeBuffers();
                debug("kxasio: alloc(): not enough memory\n");
        return ASE_NoMemory;
    }

    this->callbacks = callbacks;

    if(callbacks && callbacks->asioMessage && (callbacks->asioMessage(kAsioSupportsTimeInfo, 0, 0, 0)))
    {
        timeInfoMode = true;
        asioTime.timeInfo.speed = 1.;
        asioTime.timeInfo.systemTime.hi = asioTime.timeInfo.systemTime.lo = 0;
        asioTime.timeInfo.samplePosition.hi = asioTime.timeInfo.samplePosition.lo = 0;
        asioTime.timeInfo.sampleRate = sampleRate;
        asioTime.timeInfo.flags = kSystemTimeValid | kSamplePositionValid | kSampleRateValid;

        asioTime.timeCode.speed = 1.;
        asioTime.timeCode.timeCodeSamples.lo = asioTime.timeCode.timeCodeSamples.hi = 0;

        if(tcRead)
          asioTime.timeCode.flags = kTcValid | kTcRunning;
        else
          asioTime.timeCode.flags = 0;
    }
    else
    {
        timeInfoMode = false;   
        memset(&asioTime,0,sizeof(asioTime));
    }
    return ASE_OK;
}

//---------------------------------------------------------------------------------------------
ASIOError KXAsio::disposeBuffers()
{
    long i;

    callbacks = 0;
    stop();
    for (i = 0; i < activeInputs; i++)
    {
        if(ikx)
                 ikx->asio_free(ASIO_INPUT,inputBuffers[i]);

                inputBufferSizes[i]=0;
    }

    activeInputs = 0;

    for (i = 0; i < activeOutputs; i++)
    {
        if(ikx)
         ikx->asio_free(ASIO_OUTPUT,outputBuffers[i]);

        outputBufferSizes[i]=0;
    }

    activeOutputs = 0;
    return ASE_OK;
}

int KXAsio::get_config(int what)
{
    int ret=-1;

    char key_name[32];

    switch(what)
    {
            case KXASIO_FREQ: sprintf(key_name,"Freq_%d",device_num); break;
            case KXASIO_BPS: sprintf(key_name,"BPS_%d",device_num); break;
            case KXASIO_N_PB_CHN: sprintf(key_name,"Pb_%d",device_num); break;
            case KXASIO_N_REC_CHN: sprintf(key_name,"Rec_%d",device_num); break;
            case KXASIO_LATENCY: sprintf(key_name,"Latency_%d",device_num); break;
            case KXASIO_ASIO_METHOD: 
               sprintf(key_name,"AsioMethod_%d",device_num); 
               ret=KXASIO_METHOD_DEFAULT;
               break;
            default:
             debug("kxasio: invalid get_config(%d)\n",what);
             return -1;
    }

    HKEY hKey;
    if(RegOpenKeyEx(HKEY_CURRENT_USER,"Software\\kX\\ASIO",NULL,KEY_ALL_ACCESS,&hKey)==ERROR_SUCCESS)
    {
            DWORD len=0;
            LONG r_ret;
            DWORD type=REG_SZ;

            r_ret=RegQueryValueEx(hKey,(LPCTSTR)key_name,NULL,&type,NULL,&len);
            if((r_ret==ERROR_SUCCESS)||(r_ret==ERROR_MORE_DATA))
            {
                        char *reg_item;
                        reg_item=(char *)malloc(1+len);
                        if(reg_item)
                        {
                            if(RegQueryValueEx(hKey,(LPCTSTR)key_name,NULL,&type,(unsigned char *)reg_item,&len)==ERROR_SUCCESS)
                            {
                                if(sscanf(reg_item,"%d",&ret)!=1)
                                 ret=-1;
                            }
                                        free(reg_item);
                        }
            }

            RegCloseKey(hKey);

    }

    return ret;
}

void KXAsio::set_config(int what,int val)
{
    HKEY hKey;
    char key_name[32];

    switch(what)
    {
             case KXASIO_LATENCY: sprintf(key_name,"Latency_%d",device_num); break;
             case KXASIO_BPS: sprintf(key_name,"BPS_%d",device_num); break;
             case KXASIO_FREQ: sprintf(key_name,"Freq_%d",device_num); break;
             case KXASIO_N_PB_CHN: sprintf(key_name,"Pb_%d",device_num); break;
             case KXASIO_N_REC_CHN: sprintf(key_name,"Rec_%d",device_num); break;
             case KXASIO_ASIO_METHOD: sprintf(key_name,"AsioMethod_%d",device_num); break;
             default: debug("kxasio: invalid set_config(%d,%d)\n",what,val); return;
    }

    if(RegOpenKeyEx(HKEY_CURRENT_USER,"Software\\kX\\ASIO",NULL,KEY_ALL_ACCESS,&hKey)!=ERROR_SUCCESS)
    {
             RegCreateKey(HKEY_CURRENT_USER,"Software\\kX",&hKey);
             RegCloseKey(hKey);

             if(RegCreateKey(HKEY_CURRENT_USER,"Software\\kX\\ASIO",&hKey)!=ERROR_SUCCESS)
                return;
    }

    char value[10];
    sprintf(value,"%d",val);
    RegSetValueEx(hKey,(LPCTSTR)key_name,0,REG_SZ,(const unsigned char *)value,strlen(value));

    RegCloseKey(hKey);
}

//---------------------------------------------------------------------------------------------
ASIOError KXAsio::future (long selector, void* opt)
{
    ASIOTransportParameters* tp = (ASIOTransportParameters*)opt;
    switch (selector)
    {
        case kAsioEnableTimeCodeRead:   /*tcRead = true;*/  return ASE_NotPresent;
        case kAsioDisableTimeCodeRead:  /*tcRead = false;*/ return ASE_NotPresent;
        case kAsioSetInputMonitor:      return ASE_NotPresent;  
        case kAsioCanInputMonitor:      return ASE_NotPresent;  
        case kAsioCanTimeInfo:          return ASE_NotPresent;
        case kAsioCanTimeCode:          return ASE_NotPresent;
    }
    return ASE_NotPresent;
}

//------------------------------------------------------------------------------------------
ASIOError KXAsio::getSamplePosition (ASIOSamples *sPos, ASIOTimeStamp *tStamp)
{
    __int64 nanoSeconds = (__int64) ((__int64)timeGetTime()) * (__int64)1000000;
    tStamp->lo = (unsigned long)nanoSeconds;
    tStamp->hi = (unsigned long)(nanoSeconds>>32);

        // FIXME
        // update samplePosition according to the hardware...

    sPos->hi = (unsigned long)(samplePosition>>32);
    sPos->lo = (unsigned long)samplePosition;

    return ASE_OK;
}


//---------------------------------------------------------------------------------------------
void KXAsio::bufferSwitch ()
{
    if (started && callbacks)
    {
        getSamplePosition (&asioTime.timeInfo.samplePosition, &asioTime.timeInfo.systemTime);

        if (timeInfoMode)
            bufferSwitchX ();
        else
            callbacks->bufferSwitch (toggle, ASIOTrue);
        toggle = 1-toggle;

        samplePosition += blockFrames;
    }
}

//---------------------------------------------------------------------------------------------
// asio2 buffer switch
void KXAsio::bufferSwitchX ()
{
    if (tcRead)
    {   // Create a fake time code, which is 10 minutes ahead of the card's sample position
        // Please note that for simplicity here time code will wrap after 32 bit are reached
                // asioTime.timeCode.timeCodeSamples.lo = (long)(asioTime.timeInfo.samplePosition.lo + 600.0 * sampleRate);
        // asioTime.timeCode.timeCodeSamples.hi = 0;
    }
    callbacks->bufferSwitchTimeInfo (&asioTime, toggle, ASIOTrue); 
    asioTime.timeInfo.flags &= ~(kSampleRateChanged | kClockSourceChanged);
}

//---------------------------------------------------------------------------------------------
ASIOError KXAsio::outputReady ()
{
    return ASE_NotPresent;
}

//------------------------------------------------------------------------------------------

static DWORD __stdcall ASIOThread_ (void *param)
{
 KXAsio *theDriver=(KXAsio *)param;
 return (DWORD) theDriver->ASIOThread();
}

int KXAsio::ASIOThread(void)
{
    // Vista optimization
    HANDLE hMmTask = NULL;
    DWORD dwMmTaskIndex = 0;

    HMODULE avrt = LoadLibrary("Avrt.dll");
    if(avrt)
    {

    enum AVRT_PRIORITY
    {
    AVRT_PRIORITY_CRITICAL=2,
    AVRT_PRIORITY_HIGH=1,
    AVRT_PRIORITY_LOW=-1,
    AVRT_PRIORITY_NORMAL=0
    };

            typedef HANDLE (WINAPI *AvSetMmThreadCharacteristics_t)(LPCTSTR TaskName,LPDWORD TaskIndex);
            typedef BOOL (WINAPI *AvSetMmThreadPriority_t)(HANDLE AvrtHandle,AVRT_PRIORITY Priority);

            AvSetMmThreadCharacteristics_t avSetMmThreadCharacteristics=(AvSetMmThreadCharacteristics_t)
               GetProcAddress(avrt,"AvSetMmThreadCharacteristicsA");

            AvSetMmThreadPriority_t avSetMmThreadPriority=(AvSetMmThreadPriority_t)
               GetProcAddress(avrt,"AvSetMmThreadPriority");

            if(avSetMmThreadCharacteristics && avSetMmThreadPriority)
            {
                 // Register this thread with MMCSS
                 hMmTask = avSetMmThreadCharacteristics("Pro Audio",&dwMmTaskIndex); // used to be "Audio"

                 // Set thread priority to HIGH via MMCSS
                 if( NULL != hMmTask )
                 {
                     if(avSetMmThreadPriority(hMmTask, AVRT_PRIORITY_CRITICAL))
                      debug("kxasio: Vista-specific thread priority set to critical\n");
                     else
                      debug("kxasio: could not set Vista-specific thread priority! [%d]\n",GetLastError());

                 } else debug("kxasio: Could not set thread characteristics [NULL]\n");
            } else debug("kxasio: Could not find appropriate entry points in avrt.dll\n");

            FreeLibrary(avrt);
    }

 MMRESULT rr=timeBeginPeriod(1);
 
 if(rr==TIMERR_NOERROR)
  debug("kxasio: time period set to 1,0 msec\n");
 else
 {
  if(rr==TIMERR_NOCANDO)
   debug("kxasio: value not supported (nocando - 1 msec) !\n");

  debug("kxasio: failed to set time period to 1msec; %x !\n",GetLastError());
 }

 // 3552 change:
 int tmp_new_timer=0;
 ikx->asio_timer(1,5000,tmp_new_timer); // input is in 10000-units, set to 0.5ms

 HANDLE timer=NULL;
 timer=CreateWaitableTimer(NULL,FALSE,NULL); // synchro timer
 if(timer==NULL)
 {
  strcpy(errorMessage,"Error creating system timer");
  debug("kxasio: error creating timer\n");
  return false;
 }
 debug("kxasio: timer created\n");

 double tm_delay=(1000.0 * (double)blockFrames / sampleRate / 8.0); // 8.0 is a constant; 8 times per buffer
 if(tm_delay<1.0) tm_delay=1.0;

 LARGE_INTEGER when;
 when.QuadPart=(__int64)(tm_delay*1000000.0);

 if(SetWaitableTimer(timer,&when,(DWORD)tm_delay,NULL,NULL,FALSE)==0)
 {
      DWORD err=GetLastError();
      char tmp[128];
      debug("kxasio: Error setting waitable timer: %x (%d)\n",err,err);
      return false;
 }

 int asio_method=ikx->get_asio_method();
 HANDLE asio_event=ikx->get_asio_event();

 do
 {
   if(ikx)
   {
     // ret=1 - switch buffers
     if(ikx->asio_wait(toggle,&done))
     {
      bufferSwitch();
     }
     else
     {
         // time has not come yet
         // asio_wait() will process KXASIO_METHOD_WAIT_IN_KERNEL itself

         if(asio_method&KXASIO_METHOD_SLEEP)
          Sleep(1);
         else
          if(asio_method&KXASIO_METHOD_THREAD)
            SwitchToThread(); // sleep
          else
           if(asio_method&KXASIO_METHOD_TIMER)
            WaitForSingleObject(timer,(DWORD)tm_delay);
          else
           if(asio_method&KXASIO_METHOD_SEND_EVENT && asio_event)
             WaitForSingleObject(asio_event,100);
           else
            Sleep(0); // KXASIO_METHOD_NOWAIT
     }
   }
   else
    break;
 }while(!done);

 if(timer)
 {
  CloseHandle(timer);
  timer=NULL;
 }

 ikx->asio_timer(0,0,tmp_new_timer); // setget=0 -> reset value

 timeEndPeriod(1);

 return 0;
}

#include "cpl.cpp"
