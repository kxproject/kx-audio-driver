// kX Driver Interface
// Copyright (c) Eugene Gavrilov, 2001-2005.

//  kX Project audio driver for Mac OS X
//  Created by Eugene Gavrilov.
//  Copyright 2008 Eugene Gavrilov. All rights reserved.
//  www.kxproject.com

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

#include "..\kstream\kscommon.h"

const char *iKX::get_winmm_name(int what)
{
 switch(what)
 {
     case KX_WINMM_MIXER:   return mixer_name;
     case KX_WINMM_WAVE:    return wave_name;
     case KX_WINMM_SYNTH:   return synth_name;
     case KX_WINMM_SYNTH2:  return synth2_name;
     case KX_WINMM_UART:    return uart_name;
     case KX_WINMM_UART2:   return uart2_name;
     case KX_WINMM_CTRL:    return ctrl_name;
     case KX_WINMM_WAVE23:  return wave23_name;
     case KX_WINMM_WAVE45:  return wave45_name;
     case KX_WINMM_WAVE67:  return wave67_name;
     case KX_WINMM_WAVEHQ:  return waveHQ_name;
     default: return NULL;
 }
}


iKX::iKX()
{
    error_name[0]=0;

        hWave=NULL;
        hTopo=NULL;
        hUart=NULL;
        hUart2=NULL;
        hSynth=NULL;
        hSynth2=NULL;

        is_10k2=0;

        memset(&guid_,0,sizeof(guid_));

        memset(&wave_path[0],0,sizeof(wave_path));
        memset(&topo_path[0],0,sizeof(topo_path));
        /*
        memset(&uart_path[0],0,sizeof(uart_path));
        memset(&synth2_path[0],0,sizeof(synth2_path));
        memset(&uart2_path[0],0,sizeof(uart2_path));
        memset(&synth_path[0],0,sizeof(synth_path));
        memset(&ctrl_path[0],0,sizeof(ctrl_path));
        */

        memset(&mixer_name[0],0,sizeof(mixer_name));
        memset(&wave_name[0],0,sizeof(mixer_name));
        memset(&wave23_name[0],0,sizeof(mixer_name));
        memset(&wave45_name[0],0,sizeof(mixer_name));
        memset(&wave67_name[0],0,sizeof(mixer_name));
        memset(&waveHQ_name[0],0,sizeof(mixer_name));
        memset(&uart_name[0],0,sizeof(mixer_name));
        memset(&uart2_name[0],0,sizeof(mixer_name));
        memset(&synth_name[0],0,sizeof(mixer_name));
        memset(&synth2_name[0],0,sizeof(mixer_name));
        memset(&ctrl_name[0],0,sizeof(ctrl_name));

        memset(&device_name[0],0,sizeof(device_name));

        for(int i=0;i<KX_MAX_WAVE_DEVICES;i++)
         mixer_handler[i]=0;

        device_num=-1;

        // asio init
        asio_inited=0;
        last_asio_voice=-1;

         asio_iks_property=NULL;
         asio_secondary_buffer=NULL;
         asio_prim_buffer=NULL;
         asio_ds=NULL;

         pRenderEnumerator=0;
         pRenderFilter=0;
         pRenderPin=0;

        asio_method=0;
        asio_user_event=0;
        memset(asio_mem_table,0,sizeof(asio_mem_table));

        asio_notification=0;
}

iKX::~iKX()
{
    close();
}

int iKX::init_winmm()
{
    // check for Vista and Windows7:
    OSVERSIONINFOEX ver;
    memset(&ver,0,sizeof(ver));
    ver.dwOSVersionInfoSize=sizeof(ver);

    if(GetVersionEx((OSVERSIONINFO *)&ver)) // ok?
    {
     if(ver.dwPlatformId==VER_PLATFORM_WIN32_NT && ver.dwMajorVersion>=6)
     {
      debug("iKX(): init_winmm: mixers are not supported. Vista+ workaround\n");
      return 0;
     }
    }

    int ret=-1;

    for(unsigned int i=0;i<mixerGetNumDevs();i++)
    {
        HMIXER mixer=0;
        int to_close=1;

        // open mixer
        if(mixerOpen(&mixer,i,0,0,MIXER_OBJECTF_MIXER)!=MMSYSERR_NOERROR)
        {
            debug("iKX(): mixerOpen failed [%x]\n",GetLastError());
            continue; // next mixer
        }
        // get mixer caps
        MIXERCAPS mc; memset(&mc,0,sizeof(mc));
        if(mixerGetDevCaps(i,&mc,sizeof(mc))!=MMSYSERR_NOERROR)
        {
            debug("iKX(): mixerGetDevCaps() failed [%x]\n",GetLastError());
            mixerClose(mixer);
            continue;
        }

        // debug("iKX(): init_winmm: looking for '%s': '%s' [#%d out of %d]\n",wave_name,mc.szPname,i,mixerGetNumDevs());

        if(strncmp(mc.szPname,wave_name,strlen(wave_name)-4)!=NULL) // yes, this should be 'wave_name', not 'mixer_name';
        { mixerClose(mixer); continue; }

        if(strcmp(mc.szPname,wave_name)==0)
         { mixer_handler[0]=(uintptr_t)mixer; to_close=0; ret=0; }
        else
         if(strcmp(mc.szPname,wave23_name)==0)
          { mixer_handler[1]=(uintptr_t)mixer; to_close=0;  ret=0; }
         else
          if(strcmp(mc.szPname,wave45_name)==0)
           { mixer_handler[2]=(uintptr_t)mixer; to_close=0;  ret=0; }
          else
           if(strcmp(mc.szPname,wave67_name)==0)
            { mixer_handler[3]=(uintptr_t)mixer; to_close=0;  ret=0; }
           else
           if(strcmp(mc.szPname,waveHQ_name)==0)
             { mixer_handler[4]=(uintptr_t)mixer; to_close=0; ret=0; }
            else
            {
             debug("kXAPI: WARNING: incorrect init_winmm() code: %s\n",mc.szPname);
            }
        if(to_close)
         mixerClose(mixer);
//      break;
    }
    if(ret==-1)
    {
     debug("iKX(): init_winmm: number of mixers: %d\n",mixerGetNumDevs());
    }

    return ret;
}

int iKX::init(int device,int ignore_winmm)
{
    int i=0;
    char string[1024];
    int ret=-1;

    if(device_num!=-1) // double init
    {
     strcpy(error_name,"interface already initialized");
     debug("iKX init(): already initialized\n");
     return -7;
    }

    strcpy(error_name,"no error");

    // asio init
    asio_inited=0;
    last_asio_voice=-1;

    asio_method=0;

     asio_iks_property=NULL;
     asio_secondary_buffer=NULL;
     asio_prim_buffer=NULL;
     asio_ds=NULL;

     pRenderEnumerator=0;
     pRenderFilter=0;
     pRenderPin=0;

    memset(asio_mem_table,0,sizeof(asio_mem_table));

    memset(&guid_,0,sizeof(guid_));

    device_num=device;
    
    HDEVINFO hInfo=SetupDiGetClassDevs((GUID *)&KSCATEGORY_AUDIO,NULL,NULL,DIGCF_DEVICEINTERFACE |DIGCF_PRESENT);
    
    if(hInfo==INVALID_HANDLE_VALUE)
    { 
     debug("iKX init(): setupdigetclassdevs failed [%x]\n",GetLastError());
     strcpy(error_name,"error enumerating devices");
     return ret; 
    }
    
    SP_DEVINFO_DATA devinfo;
    devinfo.cbSize=sizeof(devinfo);

    while(SetupDiEnumDeviceInfo(hInfo,i,&devinfo))
    {
        i++;
        if(SetupDiGetDeviceInstanceId(hInfo,&devinfo,string,sizeof(string),NULL))
        {
            if(
               (strstr(string,"VEN_1102&DEV_0002")==NULL) &&
               (strstr(string,"VEN_1102&DEV_0004")==NULL) &&
               (strstr(string,"VEN_1102&DEV_0008")==NULL) 
              ) // FIXME: bad test, actually :(
            { 
              continue; 
            }
        }
        else
        {
         continue;
         }

        SP_DEVICE_INTERFACE_DATA data;
        data.cbSize=sizeof(data);
        int j=0;
        while(SetupDiEnumDeviceInterfaces(hInfo,&devinfo,(GUID *)&KSCATEGORY_AUDIO,j++,&data)==TRUE)
        {
            DWORD ReqLen=0;
            SetupDiGetInterfaceDeviceDetail (
                hInfo,
                &data,
                NULL,
                0,
                &ReqLen,
            NULL
            );

                PSP_INTERFACE_DEVICE_DETAIL_DATA m_Detail = PSP_INTERFACE_DEVICE_DETAIL_DATA(new char[ReqLen]);

                if ( m_Detail )
                {
                        m_Detail->cbSize = sizeof SP_INTERFACE_DEVICE_DETAIL_DATA;

                        if(SetupDiGetInterfaceDeviceDetail (
                            hInfo,
                            &data,
                            m_Detail,
                            ReqLen,
                            &ReqLen,
                            NULL
                            )==TRUE)
                        {
                            if(strstr(m_Detail->DevicePath,"kx_topo1")!=NULL)
                            {
                             HKEY key=SetupDiOpenDeviceInterfaceRegKey(hInfo,&data,0,KEY_ALL_ACCESS);
                             if(key!=INVALID_HANDLE_VALUE)
                             {
                                      DWORD type; type=REG_SZ;
                                      char tmp_str[256];
                                      DWORD len; len=(DWORD)sizeof(tmp_str);
                                      if(RegQueryValueEx(key,"FriendlyName",NULL,&type,(unsigned char *)tmp_str,&len)==ERROR_SUCCESS)
                                      {
                                        strncpy(mixer_name,tmp_str,sizeof(mixer_name));
                                      }
                                      RegCloseKey(key);
                             }
                             strncpy(topo_path,m_Detail->DevicePath,sizeof(topo_path));
                            } else
                            if(strstr(m_Detail->DevicePath,"kx_wave0")!=NULL)
                            {
                             HKEY key=SetupDiOpenDeviceInterfaceRegKey(hInfo,&data,0,KEY_ALL_ACCESS);
                             if(key!=INVALID_HANDLE_VALUE)
                             {
                                      DWORD type; type=REG_SZ;
                                      char tmp_str[256];
                                      DWORD len; len=(DWORD)sizeof(tmp_str);
                                      if(RegQueryValueEx(key,"FriendlyName",NULL,&type,(unsigned char *)tmp_str,&len)==ERROR_SUCCESS)
                                      {
                                        strncpy(wave_name,tmp_str,sizeof(wave_name));

                                        strncpy(wave23_name,tmp_str,sizeof(wave_name));
                                        char *p=strstr(wave23_name," 0/1");
                                        if(p)
                                        {
                                         *p=0;
                                         strcpy(wave45_name,wave23_name);
                                         strcpy(wave67_name,wave23_name);
                                         strcpy(waveHQ_name,wave23_name);

                                         strcat(wave23_name," 2/3");
                                         strcat(wave45_name," 4/5");
                                         strcat(wave67_name," 6/7");
                                         strcat(waveHQ_name," HQ");
                                        }
                                        else
                                        {
                                         wave23_name[0]=0;
                                         debug("ikX API: not all the 4 wave devices were found... strange\n");
                                        }
                                      }
                                      RegCloseKey(key);
                             }
                             strncpy(wave_path,m_Detail->DevicePath,sizeof(wave_path));
                            } else
                            if(strstr(m_Detail->DevicePath,"kx_uart2")!=NULL)
                            {
                             //strncpy(uart2_path,m_Detail->DevicePath,sizeof(uart2_path));
                             HKEY key=SetupDiOpenDeviceInterfaceRegKey(hInfo,&data,0,KEY_ALL_ACCESS);
                             if(key!=INVALID_HANDLE_VALUE)
                             {
                                      DWORD type; type=REG_SZ;
                                      char tmp_str[256];
                                      DWORD len; len=(DWORD)sizeof(tmp_str);
                                      if(RegQueryValueEx(key,"FriendlyName",NULL,&type,(unsigned char *)tmp_str,&len)==ERROR_SUCCESS)
                                      {
                                        strncpy(uart2_name,tmp_str,sizeof(uart2_name));
                                      }
                                      RegCloseKey(key);
                             }
                            } else
                            if(strstr(m_Detail->DevicePath,"kx_uart1")!=NULL)
                            {
                             //strncpy(uart_path,m_Detail->DevicePath,sizeof(uart_path));
                             HKEY key=SetupDiOpenDeviceInterfaceRegKey(hInfo,&data,0,KEY_ALL_ACCESS);
                             if(key!=INVALID_HANDLE_VALUE)
                             {
                                      DWORD type; type=REG_SZ;
                                      char tmp_str[256];
                                      DWORD len; len=(DWORD)sizeof(tmp_str);
                                      if(RegQueryValueEx(key,"FriendlyName",NULL,&type,(unsigned char *)tmp_str,&len)==ERROR_SUCCESS)
                                      {
                                        strncpy(uart_name,tmp_str,sizeof(uart_name));
                                      }
                                      RegCloseKey(key);
                             }
                            } else
                            if(strstr(m_Detail->DevicePath,"kx_ctrl")!=NULL)
                            {
                             //strncpy(ctrl_path,m_Detail->DevicePath,sizeof(ctrl_path));
                             HKEY key=SetupDiOpenDeviceInterfaceRegKey(hInfo,&data,0,KEY_ALL_ACCESS);
                             if(key!=INVALID_HANDLE_VALUE)
                             {
                                      DWORD type; type=REG_SZ;
                                      char tmp_str[256];
                                      DWORD len; len=(DWORD)sizeof(tmp_str);
                                      if(RegQueryValueEx(key,"FriendlyName",NULL,&type,(unsigned char *)tmp_str,&len)==ERROR_SUCCESS)
                                      {
                                        strncpy(ctrl_name,tmp_str,sizeof(ctrl_name));
                                      }
                                      RegCloseKey(key);
                             }
                            } else
                            if(strstr(m_Detail->DevicePath,"kx_synth2")!=NULL)
                            {
                             //strncpy(synth2_path,m_Detail->DevicePath,sizeof(synth2_path));
                             HKEY key=SetupDiOpenDeviceInterfaceRegKey(hInfo,&data,0,KEY_ALL_ACCESS);
                             if(key!=INVALID_HANDLE_VALUE)
                             {
                                      DWORD type; type=REG_SZ;
                                      char tmp_str[256];
                                      DWORD len; len=(DWORD)sizeof(tmp_str);
                                      if(RegQueryValueEx(key,"FriendlyName",NULL,&type,(unsigned char *)tmp_str,&len)==ERROR_SUCCESS)
                                      {
                                        strncpy(synth2_name,tmp_str,sizeof(synth2_name));
                                      }
                                      RegCloseKey(key);
                             }
                            } else
                            if(strstr(m_Detail->DevicePath,"kx_synth1")!=NULL)
                            {
                             //strncpy(synth_path,m_Detail->DevicePath,sizeof(synth_path));
                             HKEY key=SetupDiOpenDeviceInterfaceRegKey(hInfo,&data,0,KEY_ALL_ACCESS);
                             if(key!=INVALID_HANDLE_VALUE)
                             {
                                      DWORD type; type=REG_SZ;
                                      char tmp_str[256];
                                      DWORD len; len=(DWORD)sizeof(tmp_str);
                                      if(RegQueryValueEx(key,"FriendlyName",NULL,&type,(unsigned char *)tmp_str,&len)==ERROR_SUCCESS)
                                      {
                                        strncpy(synth_name,tmp_str,sizeof(synth_name));
                                      }
                                      RegCloseKey(key);
                             }
                            }
                        } // if(SetupDiGetInterfaceDeviceDetail)
                        delete m_Detail;
                } // if detail -- SetupDiGetInterfaceDeviceDetail
       } // while enum interfaces

       if(wave_path[0] && topo_path[0]) // have necessary interfaces:
       {
           if(device!=0)
           { 
             device--; 
             // clean-up:
             mixer_name[0]=0; topo_path[0]=0;
             wave_name[0]=0;
             wave23_name[0]=0;
             wave45_name[0]=0;
             wave67_name[0]=0;
             waveHQ_name[0]=0;  wave_path[0]=0;
             synth_name[0]=0;
             synth2_name[0]=0;
             uart_name[0]=0;
             uart2_name[0]=0;
             ctrl_name[0]=0;

             continue; 
           }
           // right device with correct # found:
           break;
       }
    } // enum dev info
    SetupDiDestroyDeviceInfoList(hInfo);

    if(wave_path[0])
    {
        hWave = (void *)CreateFile(
        wave_path,
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
        );
        if((HANDLE)hWave == INVALID_HANDLE_VALUE)
        {
                debug("iKX init(): error opening wave device [%x]\n",GetLastError());
                strcpy(error_name,"error opening wave device");
        return GetLastError();
        }   
    } 

    if(topo_path[0])
    {
        hTopo = (void *)CreateFile(
        topo_path,
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
        );
        if((HANDLE)hTopo == INVALID_HANDLE_VALUE)
        {
            debug("iKX init() error opening topology device [%x]\n",GetLastError());
            strcpy(error_name,"error opening topology device");
        return GetLastError();
        }   
    } 

    if(hTopo && hWave)
    {
                char card_name[KX_MAX_STRING];
                get_string(KX_STR_CARD_NAME,card_name);

                if(!mixer_name[0])
                 sprintf(mixer_name,"kX Mixer %s 0/1",card_name);
                if(!wave_name[0])
                  sprintf(wave_name,"kX Wave %s 0/1",card_name);
                if(!wave23_name[0])
                  sprintf(wave23_name,"kX Wave %s 2/3",card_name);
                if(!wave45_name[0])
                  sprintf(wave45_name,"kX Wave %s 4/5",card_name);
                if(!wave67_name[0])
                  sprintf(wave67_name,"kX Wave %s 6/7",card_name);
                if(!waveHQ_name[0])
                  sprintf(waveHQ_name,"kX Wave %s HQ",card_name);
                if(!uart2_name[0])
                  sprintf(uart2_name,"kX Uart2 %s",card_name);
                if(!uart_name[0])
                  sprintf(uart_name,"kX Uart %s",card_name);
                if(!synth_name[0])
                  sprintf(synth_name,"kX Synth %s",card_name);
                if(!synth2_name[0])
                  sprintf(synth2_name,"kX Synth2 %s",card_name);
                if(!ctrl_name[0])
                  sprintf(ctrl_name,"kX Control %s",card_name);

        ret=init_winmm();
        if(!ignore_winmm)
        {
         if(ret)
         {
          debug("iKX init() error initializing WinMM subsystem [%x]\n",GetLastError());
          strcpy(error_name,"error initializing WinMM subsystem");
         }
        }
        else
        {
         debug("iKX init(): ignore_winmm=1; btw, result was: %d [mixer_num=%d %d %d %d]\n",ret,mixer_handler[0],mixer_handler[1],mixer_handler[2],mixer_handler[3]);
//       if(mixer_handler[0]==-1)
//        mixer_num[0]=0;
        }
    }
    else
    {
     strcpy(error_name,"kX device not found");
    }

    if(ret==0)
    {
        // set emulation
        is_10k2=0;
    if(get_dword(KX_DWORD_IS_10K2,(dword *)&is_10k2))
    {
     debug("iKX init(): incorrect driver version\n");
     strcpy(error_name,"incorrect driver version");
     ret=-100;
    }

        // set device name
        if(get_string(KX_STR_CARD_NAME,device_name))
         ret=-200;
    }

    return ret;
}

int iKX::close()
{
 if(asio_inited)
 {
  asio_close();

  asio_inited=0;
  last_asio_voice=-1;
  asio_method=0;

   asio_iks_property=NULL;
   asio_secondary_buffer=NULL;
   asio_prim_buffer=NULL;
   asio_ds=NULL;

   pRenderEnumerator=0;
   pRenderFilter=0;
   pRenderPin=0;

  memset(asio_mem_table,0,sizeof(asio_mem_table));
  if(asio_user_event)
  {
   CloseHandle(asio_user_event);
   asio_user_event=0;
  }
 }

 if(hWave)
  CloseHandle((HANDLE)hWave);
 hWave=0;
 if(hTopo)
  CloseHandle((HANDLE)hTopo);
 hTopo=0;
 if(hSynth)
  CloseHandle((HANDLE)hSynth);
 hSynth=0;
 if(hUart)
  CloseHandle((HANDLE)hUart);
 hUart=0;

 wave_path[0]=0;
 topo_path[0]=0;

 /*synth_path[0]=0;
 synth2_path[0]=0;
 uart_path[0]=0;
 uart2_path[0]=0;
 ctrl_path[0]=0;
 */

 device_num=-1;
 for(int i=0;i<KX_MAX_WAVE_DEVICES;i++)
 {
   if(mixer_handler[i])
   {
    mixerClose((HMIXER)mixer_handler[i]);
    mixer_handler[i]=0;
   }
 }

 return 0;
}

// NB! should be aligned... (don`t use pack(1))
typedef struct
{
    union
    {
        struct
        {
            GUID    Set;
            ULONG   Id;
            ULONG   Flags;
        };
        LONGLONG    Alignment;
    };
    kx_property_instance inst;
}my_prop;

typedef struct
{
    union
    {
        struct
        {
            GUID    Set;
            ULONG   Id;
            ULONG   Flags;
        };
        LONGLONG    Alignment;
    };
    
    ULONG  PinId;
    ULONG  Reserved;

    ULONG padding[4];

    kx_property_instance inst;
}my_kspin_prop;

typedef struct
{
    kx_property_instance inst;
}my_ds_prop;

int iKX::raw_ctrl(dword func,void *prop,int psize,void *buff,int bsize,int *ret_bytes)
{
    ULONG           ulBytesReturned=0;
    BOOL            fSuccess=FALSE;
    HANDLE      hDev=NULL;

    switch(func&0xf0000000)
    {
        case KX_WAVE: hDev=(HANDLE)hWave; break;
        case KX_UART: hDev=(HANDLE)hUart; break;
        case KX_SYNTH: hDev=(HANDLE)hSynth; break;
        case KX_TOPO: hDev=(HANDLE)hTopo; break;
    }
    if(hDev==0)
    {
     debug("iKX raw_ctrl: bad device func [%x]\n",func);
     strcpy(error_name,"bad device func");
     return -4;
     }

    // Make the final call.
    fSuccess = DeviceIoControl(hDev, IOCTL_KS_PROPERTY,
                                prop,psize,
                                buff, bsize,
                                &ulBytesReturned, NULL);
    if(ret_bytes)
     *ret_bytes=ulBytesReturned;
    return (fSuccess==TRUE)?0:-10;
}

int iKX::dsound_property(void *iks,dword func,void *buff,int bsize,int *ret_bytes)
{
    ULONG           ulBytesReturned=0;
    BOOL            fSuccess=FALSE;

    if(func&0xf0000000)
    {
     debug("iKX dsound_property: incorrect call [func=%x]\n",func);
     return -40;
    }

    // Prepare the property structure sent down.
    my_ds_prop *prop;
    int psize=bsize+sizeof(my_ds_prop);
    prop=(my_ds_prop *)LocalAlloc(LMEM_FIXED|LMEM_ZEROINIT,psize);
    if(prop==0)
    {
     debug("iKX dsound_property: LocalAlloc [%d] failed\n",psize);
     return -2;
    }

    prop->inst.magic=PROPERTY_MAGIC;
    prop->inst.prop=func&0x0fffffff;
    memcpy(&prop[1],buff,bsize);

    // Make the final call.
    if(func&KX_PROP_GET)
      fSuccess = ((IKsPropertySet *)iks)->Get(KSPROPSETID_Private,KSPROPERTY_PRIVATE,
        prop,psize,buff,bsize,&ulBytesReturned);
    else
    if(func&KX_PROP_SET)
      fSuccess = ((IKsPropertySet *)iks)->Set(KSPROPSETID_Private,KSPROPERTY_PRIVATE,
        prop,psize,buff,bsize);
    else
      fSuccess = -40;

    if(ret_bytes)
     *ret_bytes=ulBytesReturned;

    LocalFree((HLOCAL)prop);
    return fSuccess;
}

int iKX::driver_property(void *iks,dword func,void *buff,int bsize,int *ret_bytes)
{
    if(pRenderPin==NULL || iks!=(void *)pRenderPin->GetHandle())
     return dsound_property(iks,func,buff,bsize,ret_bytes);
    else
    {
    ULONG           ulBytesReturned=0;
    BOOL            fSuccess=FALSE;
    HANDLE          hDev=NULL;

    if(func&0xf0000000)
    {
     debug("iKX dsound_property: incorrect call [func=%x]\n",func);
     return -40;
    }

    // Prepare the property structure sent down.
    my_kspin_prop *prop;
    int psize=bsize+sizeof(my_kspin_prop);
    prop=(my_kspin_prop *)LocalAlloc(LMEM_FIXED|LMEM_ZEROINIT,psize);

    // debug("iKX: driver_prop: %d, data: %d, psize:%d\n",sizeof(my_prop),bsize,psize);
    
    if(prop==0)
    {
     debug("iKX ctrl: localalloc failed [%d; %x]\n",psize,GetLastError());
     return -2;
    }

    if(func&KX_PROP_GET)
     prop->Flags = KSPROPERTY_TYPE_GET;
    else
    if(func&KX_PROP_SET)
     prop->Flags = KSPROPERTY_TYPE_SET;
    else
    {
     LocalFree((HLOCAL)prop);
     debug("iKX ctrl: incorrect func [%x]\n",func);
     return -3;
     }

    prop->Set = KSPROPSETID_Private;
    prop->Id = KSPROPERTY_PRIVATE;
    prop->Flags |= KSPROPERTY_TYPE_TOPOLOGY;

    prop->inst.magic=PROPERTY_MAGIC;
    prop->inst.prop=func&0x0fffffff;
    memcpy(&prop[1],buff,bsize);
    
    prop->PinId=pRenderPin->GetId();
    prop->Reserved=0x0;

    // Make the final call.
    fSuccess = DeviceIoControl(pRenderPin->GetHandle(), IOCTL_KS_PROPERTY,
                                prop,psize,
                                buff, bsize,
                                &ulBytesReturned, NULL);
    if(!fSuccess)
     debug("! iKX::driver_property: error calling DeviceIoControl: %d; %d; [%08x; %08x;%08x]\n",
       fSuccess,GetLastError(),pRenderPin->GetHandle(),
       (HANDLE)hWave,(HANDLE)hTopo);

    if(ret_bytes)
     *ret_bytes=ulBytesReturned;
    LocalFree((HLOCAL)prop);
    return fSuccess==TRUE?0:-10;
   }
}

int iKX::set_dsound_routing(void *sd,dword routing,dword xrouting)
{
 LPDIRECTSOUNDBUFFER buff=(LPDIRECTSOUNDBUFFER)sd;
 IKsPropertySet *property;
 
 buff->QueryInterface(IID_IKsPropertySet,(void **)&property);
 if(property)
 {
  voice_routing_req voice;
  voice.routing=routing;
  voice.xrouting=xrouting;
  int ret=dsound_property(property,KX_PROP_VOICE_ROUTING|KX_PROP_SET,&voice,sizeof(voice));
  property->Release();
  return ret;
 } 
 else
 {
  debug("iKX set_dsound_routing failed: query interface [%x]\n",GetLastError());
 }
 return -5;
}

int iKX::get_dsound_routing(void *sd,dword *routing,dword *xrouting)
{
 LPDIRECTSOUNDBUFFER buff=(LPDIRECTSOUNDBUFFER)sd;
 IKsPropertySet *property;
 
 buff->QueryInterface(IID_IKsPropertySet,(void **)&property);
 if(property)
 {
  voice_routing_req voice;
  int ret=dsound_property(property,KX_PROP_VOICE_ROUTING|KX_PROP_GET,&voice,sizeof(voice));
  if(ret==0)
  {
   if(routing)
    *routing=voice.routing;
   if(xrouting)
    *xrouting=voice.xrouting;
  }
  property->Release();
  return ret;
 } 
  else
 {
  debug("iKX get_dsound_routing failed: query interface [%x]\n",GetLastError());
 }
 return -5;
}

int iKX::ctrl(dword func,void *buff,int bsize,int *ret_bytes)
{
    ULONG           ulBytesReturned=0;
    BOOL            fSuccess=FALSE;
    HANDLE          hDev=NULL;

    // Prepare the property structure sent down.
    my_prop *prop;
    int psize=bsize+sizeof(my_prop);
    prop=(my_prop *)LocalAlloc(LMEM_FIXED|LMEM_ZEROINIT,psize);
    if(prop==0)
    {
     debug("iKX ctrl: localalloc failed [%d; %x]\n",psize,GetLastError());
     return -2;
    }

    // debug("iKX: prop: %d, data: %d,psize: %d, func: %d, %d\n",sizeof(my_prop),bsize,psize,func,sizeof(dsp_microcode));
    // debug("fl: %d, code_size: %d, info_size: %d, comment: %d\n",
    //    offsetof(dsp_microcode,flag),offsetof(dsp_microcode,code_size),offsetof(dsp_microcode,info_size),offsetof(dsp_microcode,comment));

    if(func&KX_PROP_GET)
     prop->Flags = KSPROPERTY_TYPE_GET;
    else
    if(func&KX_PROP_SET)
     prop->Flags = KSPROPERTY_TYPE_SET;
    else
    {
     LocalFree((HLOCAL)prop);
     debug("iKX ctrl: incorrect func [%x]\n",func);
     return -3;
     }

    prop->Set = KSPROPSETID_Private;
    prop->Id = KSPROPERTY_PRIVATE;
    prop->inst.magic=PROPERTY_MAGIC;
    prop->inst.prop=func&0x0fffffff;
    memcpy(&prop[1],buff,bsize);

    switch(func&0xf0000000)
    {
        case KX_WAVE: hDev=(HANDLE)hWave; break;
        case KX_UART: hDev=(HANDLE)hUart; break;
        case KX_SYNTH: hDev=(HANDLE)hSynth; break;
        case KX_TOPO: hDev=(HANDLE)hTopo; break;
    }
    if(hDev==0)
    {
     debug("iKX ctrl: incorrect func device [%x]\n",func);
     LocalFree((HLOCAL)prop);
     return -4;
     }

    // Make the final call.
    fSuccess = DeviceIoControl(hDev, IOCTL_KS_PROPERTY,
                                prop,psize,
                                buff, bsize,
                                &ulBytesReturned, NULL);
    if(ret_bytes)
     *ret_bytes=ulBytesReturned;
    LocalFree((HLOCAL)prop);
    return fSuccess==TRUE?0:-10;
}

#include "interface/mixernames.h"


int iKX::mixer(int op,int j,int channel,int *ret)
{
    int res=-10;
    dword val[2]={0,0};

    if(j<0 || j>=KX_MIXER_LAST)
    {
        debug("iKX mixer: incorrect call [op=%d j=%d chn=%d]\n",op,j,channel);
        return -1;
    }

    HMIXER mixer=0;

    // open mixer

      for(int ttt=0;ttt<KX_MAX_WAVE_DEVICES;ttt++)
      { 
    switch(j)
    {
     case KX_MIXER_WAVE23:
     case KX_MIXER_WAVE23_MUTE: ttt=1; break;
     case KX_MIXER_WAVE45:
     case KX_MIXER_WAVE45_MUTE: ttt=2; break;
     case KX_MIXER_WAVE67:
     case KX_MIXER_WAVE67_MUTE: ttt=3; break;
     case KX_MIXER_WAVEHQ:
     case KX_MIXER_WAVEHQ_MUTE: ttt=4; break;
    }
    if(ttt==KX_MAX_WAVE_DEVICES-1) // 4; 'HQ'
     if(mixer_handler[ttt]==0)
      continue;

/*
        if(mixerOpen(&mixer,mixer_num[ttt],0,0,MIXER_OBJECTF_MIXER)!=MMSYSERR_NOERROR)
        {
            debug("iKX mixer: mixerOpen failed [%x] #%d\n",GetLastError(),mixer_num[ttt]);
            strcpy(error_name,"error opening mixer");
            return -2;
        }
*/
    mixer=(HMIXER)mixer_handler[ttt];

    if(!mixer)
     break;
//--------
//          MIXERCAPS mc;
//          mixerGetDevCaps(mixer_num[ttt],&mc,sizeof(mc));
//          debug("****** -- %s: [ttt=%d; #=%d]\n",mc.szPname,ttt,mixer_num[ttt]);
//--------
        int id=-1;
    for(int cnt=0;cnt<20;cnt++)
    {
                 MIXERLINECONTROLS lc;
                 MIXERCONTROL mc;

                 lc.cbStruct=sizeof(lc);
                 lc.dwControlID=cnt;
                 lc.cControls=1;
                 lc.cbmxctrl=sizeof(mc);
                 lc.pamxctrl=&mc;
                 mc.cbStruct=sizeof(lc);

                 MMRESULT rr=mixerGetLineControls((HMIXEROBJ)mixer,&lc,MIXER_GETLINECONTROLSF_ONEBYID);
                 if(rr!=MMSYSERR_NOERROR)
                 {
                  // debug("ikX: mmsystem: bug: %x [%x] -- %d %d %d [cnt=%d]\n",GetLastError(),rr,j,mixer_handler[ttt],ttt,cnt);
                  continue;
                 }
                 id=-1;
// ----
//                 debug("(tmp) kxmixer: [%d] %s/%s [ttt=%d]; type: %x\n",cnt,mc.szName,mc.szShortName,ttt,mc.dwControlType);
// ----
                 // all names correspond to GUIDs
                 if((strcmp(mc.szName,_KX_MASTERV_NAME)==0) && (j==KX_MIXER_MASTER))
                  id=cnt;
                 else if((strcmp(mc.szName,_KX_MASTERM_NAME)==0) && (j==KX_MIXER_MASTER_MUTE))
                  id=cnt;
                 else if((strcmp(mc.szName,_KX_SYNTH1V_NAME)==0) && (j==KX_MIXER_SYNTH))
                  id=cnt;
                 else if((strcmp(mc.szName,_KX_SYNTH1M_NAME)==0) && (j==KX_MIXER_SYNTH_MUTE))
                  id=cnt;
                 else if((strcmp(mc.szName,_KX_RECINV_NAME)==0) && (j==KX_MIXER_REC))
                  id=cnt;
                 else if((strcmp(mc.szName,_KX_RECINM_NAME)==0) && (j==KX_MIXER_REC_MUTE))
                  id=cnt;

                 else if((strcmp(mc.szName,_KX_LINEIN_NAME)==0) && (j==KX_MIXER_LINEIN))
                  id=cnt;
                 else if((strcmp(mc.szName,_KX_LINEIN_MUTE_NAME)==0) && (j==KX_MIXER_LINEIN_MUTE))
                  id=cnt;
                 else if((strcmp(mc.szName,_KX_MICIN_NAME)==0) && (j==KX_MIXER_MICIN))
                  id=cnt;
                 else if((strcmp(mc.szName,_KX_MICIN_MUTE_NAME)==0) && (j==KX_MIXER_MICIN_MUTE))
                  id=cnt;

                 else
                 {
                 // these are usually translated by Windows into 'Volume' and 'Mute',
                 // and coincide with 'CD Player' and 'SW Synth' controls...

                 if((strcmp(mc.szName,_KX_WAVEV_NAME)==0) && (j==KX_MIXER_WAVE))
                  id=cnt;
                 else if((strcmp(mc.szName,_KX_WAVEM_NAME)==0) && (j==KX_MIXER_WAVE_MUTE))
                  id=cnt;
                 else if((strcmp(mc.szName,_KX_WAVEV_NAME)==0 || strcmp(mc.szName,_KX_WAVEOUT23V_NAME)==0) && (j==KX_MIXER_WAVE23))
                  id=cnt;
                 else if((strcmp(mc.szName,_KX_WAVEM_NAME)==0 || strcmp(mc.szName,_KX_WAVEOUT23M_NAME)==0) && (j==KX_MIXER_WAVE23_MUTE))
                  id=cnt;
                 else if((strcmp(mc.szName,_KX_WAVEV_NAME)==0 || strcmp(mc.szName,_KX_WAVEOUT45V_NAME)==0) && (j==KX_MIXER_WAVE45))
                  id=cnt;
                 else if((strcmp(mc.szName,_KX_WAVEM_NAME)==0 || strcmp(mc.szName,_KX_WAVEOUT45M_NAME)==0) && (j==KX_MIXER_WAVE45_MUTE))
                  id=cnt;
                 else if((strcmp(mc.szName,_KX_WAVEV_NAME)==0 || strcmp(mc.szName,_KX_WAVEOUT67V_NAME)==0) && (j==KX_MIXER_WAVE67))
                  id=cnt;
                 else if((strcmp(mc.szName,_KX_WAVEM_NAME)==0 || strcmp(mc.szName,_KX_WAVEOUT67M_NAME)==0) && (j==KX_MIXER_WAVE67_MUTE))
                  id=cnt;
                 else if((strcmp(mc.szName,_KX_WAVEV_NAME)==0 || strcmp(mc.szName,_KX_WAVEOUTHQV_NAME)==0) && (j==KX_MIXER_WAVEHQ))
                  id=cnt;
                 else if((strcmp(mc.szName,_KX_WAVEM_NAME)==0 || strcmp(mc.szName,_KX_WAVEOUTHQM_NAME)==0) && (j==KX_MIXER_WAVEHQ_MUTE))
                  id=cnt;
                 else if((strcmp(mc.szName,"Volume")==0) && (j==KX_MIXER_WAVE || j==KX_MIXER_WAVE23 || j==KX_MIXER_WAVE45 || j==KX_MIXER_WAVE67 || j==KX_MIXER_WAVEHQ))
                  id=cnt;
                 else if((strcmp(mc.szName,"Mute")==0) && (j==KX_MIXER_WAVE_MUTE || j==KX_MIXER_WAVE23_MUTE || j==KX_MIXER_WAVE45_MUTE || j==KX_MIXER_WAVE67_MUTE || j==KX_MIXER_WAVEHQ_MUTE))
                  id=cnt;
                 }

                 if(id==-1)
                 {
                  // debug(" not found -- /* %s [%d] - %d */\n",mc.szName,j,cnt);

                  // fight with localization stuff
                  if(j==KX_MIXER_WAVE || j==KX_MIXER_WAVE23 || j==KX_MIXER_WAVE45 || j==KX_MIXER_WAVE67 || j==KX_MIXER_WAVEHQ)
                  {
                   // Wave Volume / Wave Mute have following ids:
                   //   Wave 0/1 device: 2,3
                   //   Wave 2/3, 4/5, 6/7 devices: 0,1
                   if(ttt==0 && cnt==2)
                     id=2;
                   else
                    if(ttt!=0 && cnt==0)
                      id=0;
                    else
                       continue;
                  }
                   else
                  if(j==KX_MIXER_WAVE_MUTE || j==KX_MIXER_WAVE23_MUTE || j==KX_MIXER_WAVE45_MUTE || j==KX_MIXER_WAVE67_MUTE || j==KX_MIXER_WAVEHQ_MUTE)
                  {
                   if(ttt==0 && cnt==3)
                     id=3;
                   else
                    if(ttt!=0 && cnt==1)
                      id=1;
                    else
                      continue;
                  }
                   else
                    continue;
                 }
//                 debug("setting '%s' [%d; cnt=%d; ttt=%d]\n",mc.szName,j,cnt,ttt);

                 MIXERCONTROLDETAILS dets;
                 memset(&dets,0,sizeof(dets));
                 dets.cbStruct=sizeof(MIXERCONTROLDETAILS);
                 dets.dwControlID=id;
                 dets.cMultipleItems=0;
                 dets.paDetails=&val;

                 if(j==KX_MIXER_MASTER_MUTE || 
                    j==KX_MIXER_SYNTH_MUTE || 
                    j==KX_MIXER_WAVE_MUTE ||
                    j==KX_MIXER_WAVE23_MUTE ||
                    j==KX_MIXER_WAVE45_MUTE ||
                    j==KX_MIXER_WAVE67_MUTE ||
                    j==KX_MIXER_WAVEHQ_MUTE ||
                    j==KX_MIXER_LINEIN_MUTE ||
                    j==KX_MIXER_MICIN_MUTE ||
                    j==KX_MIXER_REC_MUTE) // mutes
                 {
                        dets.cbDetails=sizeof(MIXERCONTROLDETAILS_UNSIGNED)*2;
                        dets.cChannels=1;
                 }
                 else
                 {
                        dets.cbDetails=sizeof(MIXERCONTROLDETAILS_UNSIGNED);
                        dets.cChannels=2;
                 }

                 if(mixerGetControlDetails((HMIXEROBJ)mixer,&dets,MIXER_GETCONTROLDETAILSF_VALUE)!=MMSYSERR_NOERROR)
                 {
                    debug("iKX mixer: getcontroldetails failed [%x]\n",GetLastError());
                    // error
                    // mixerClose(mixer);
                    return -3;
                 }

                 if(op&KX_PROP_GET)
                 {
                    // mixerClose(mixer);
                    if(channel==0)
                        *ret=val[0];
                    else
                        *ret=val[1];
                    return 0;
                 }

                 if(channel==0)
                    val[0]=*ret;
                 else
                    val[1]=*ret;

                  if(mixerSetControlDetails((HMIXEROBJ)mixer,&dets,MIXER_SETCONTROLDETAILSF_VALUE)!=MMSYSERR_NOERROR)
                  {
                    debug("iKX mixer: setcontroldetails failed [%x]\n",GetLastError());
                    // error
                    // mixerClose(mixer);
                    return -5;
                  }
                  // success -- next mixer
                  res=0;
                  break;
    } // for each control 

    if(id==-1) // still not found
    {
     // debug("kxmixer: [warning] mixer api: control '%d' not found [ttt=%d]\n",j,ttt);
    }

    // mixerClose(mixer);

    if((j==KX_MIXER_REC)||(j==KX_MIXER_REC_MUTE)||(j==KX_MIXER_WAVE23)||(j==KX_MIXER_WAVE45)||(j==KX_MIXER_WAVE67)||(j==KX_MIXER_WAVEHQ)
      ||(j==KX_MIXER_WAVE23_MUTE)||(j==KX_MIXER_WAVE45_MUTE)||(j==KX_MIXER_WAVE67_MUTE)||(j==KX_MIXER_WAVEHQ_MUTE))
     break;
    } // for each wave device [ttt]
    
    return res;
}


typedef struct
{
 GUID guid;
 iKX *kx;
 int what;
}dx_enum_context;

BOOL CALLBACK my_enum(
  LPGUID lpGuid,            
  LPCSTR lpcstrDescription,  
  LPCSTR ,       
  LPVOID lpContext          
)
{
 dx_enum_context *context=(dx_enum_context *)lpContext;

 if(strcmp(lpcstrDescription,context->kx->get_winmm_name(context->what))==NULL)
 {
  memcpy(&context->guid,lpGuid,sizeof(GUID));
  return FALSE; // found
 }
 return TRUE; // go on
}

int iKX::init_directx(int what)
{
    dx_enum_context context;

    context.kx=this;
    context.what=what;
    memset(&context.guid,0,sizeof(GUID));

    if(DirectSoundEnumerate(&my_enum,&context)==DS_OK)
    {
     memcpy(&guid_,&context.guid,sizeof(GUID));
    }
    else
    {
     debug("iKX init_directX: DirectSoundEnumerate failed\n");
    }
    return -1;
}

LPGUID iKX::get_dsound_guid(int where)
{
 // GUID null_guid;
 // memset(&null_guid,0,sizeof(null_guid));
 // if(memcmp(&null_guid,&guid,sizeof(GUID))==0) // not inited
 init_directx(where);
 return &guid_;
}

unsigned int iKX::get_winmm_device(int what,int subdevice)
{
 UINT ret=0xffffffff;

 const char *device=get_winmm_name((what==KX_WINMM_MIXER)?KX_WINMM_WAVE:what);
 if(device==0)
  return ret;

 if(subdevice==0)
 {
      switch(what)
      {
       case KX_WINMM_MIXER:
             if(mixerGetNumDevs()) // else causes unsigned infinite loop
               for(UINT i=0;i<mixerGetNumDevs();i++)
               {
                MIXERCAPS caps;
                if(mixerGetDevCaps(i,&caps,sizeof(caps))==MMSYSERR_NOERROR)
                {
                  if(strcmp(caps.szPname,device)==NULL)
                  {
                   ret=i;
                   break;
                  }
                } else break;
               }
             break;
       case KX_WINMM_WAVE:
       case KX_WINMM_WAVE23:
       case KX_WINMM_WAVE45:
       case KX_WINMM_WAVE67:
       case KX_WINMM_WAVEHQ:
             if(waveInGetNumDevs()) // else causes unsigned infinite loop
               for(UINT i=0;i<waveInGetNumDevs();i++)
               {
                WAVEINCAPS caps;
                if(waveInGetDevCaps(i,&caps,sizeof(caps))==MMSYSERR_NOERROR)
                {
                  if(strcmp(caps.szPname,device)==NULL)
                  {
                   ret=i;
                   break;
                  }
                } else break;
               }
             break;
       case KX_WINMM_SYNTH:
       case KX_WINMM_SYNTH2:
       case KX_WINMM_UART:
       case KX_WINMM_UART2:
       case KX_WINMM_CTRL:
             if(midiInGetNumDevs()) // else causes unsigned infinite loop
               for(UINT i=0;i<midiInGetNumDevs();i++)
               {
                MIDIINCAPS caps;
                if(midiInGetDevCaps(i,&caps,sizeof(caps))==MMSYSERR_NOERROR)
                {
                  if(strcmp(caps.szPname,device)==NULL)
                  {
                   ret=i;
                   break;
                  }
                } else break;
               }
             break;
      }
 }
 else
 {
      switch(what)
      {
       case KX_WINMM_MIXER:
            debug("kxapi: incorrect get_winmm call(mixer)\n");
            break;
       case KX_WINMM_WAVE:
       case KX_WINMM_WAVE23:
       case KX_WINMM_WAVE45:
       case KX_WINMM_WAVE67:
       case KX_WINMM_WAVEHQ:
             if(waveOutGetNumDevs()) // else causes unsigned infinite loop
               for(UINT i=0;i<waveOutGetNumDevs();i++)
               {
                WAVEOUTCAPS caps;
                if(waveOutGetDevCaps(i,&caps,sizeof(caps))==MMSYSERR_NOERROR)
                {
                  if(strcmp(caps.szPname,device)==NULL)
                  {
                   ret=i;
                   break;
                  }
                } else break;
               }
             break;
       case KX_WINMM_SYNTH:
       case KX_WINMM_SYNTH2:
       case KX_WINMM_UART:
       case KX_WINMM_UART2:
       case KX_WINMM_CTRL:
             if(midiOutGetNumDevs()) // else causes unsigned infinite loop
               for(UINT i=0;i<midiOutGetNumDevs();i++)
               {
                MIDIOUTCAPS caps;
                if(midiOutGetDevCaps(i,&caps,sizeof(caps))==MMSYSERR_NOERROR)
                {
                  if(strcmp(caps.szPname,device)==NULL)
                  {
                   ret=i;
                   break;
                  }
                } else break;
               }
             break;
      }
 }
 return ret;
}

int iKX::generate_guid(char *guid)
{
  // generate GUID here:
  GUID m_guid = GUID_NULL;
  ::CoCreateGuid(&m_guid);

  sprintf(guid,"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
   m_guid.Data1,
   m_guid.Data2,
   m_guid.Data3,
   m_guid.Data4[0],m_guid.Data4[1],
   m_guid.Data4[2],m_guid.Data4[3],m_guid.Data4[4],
   m_guid.Data4[5],m_guid.Data4[6],m_guid.Data4[7]);

  return 0;
}
