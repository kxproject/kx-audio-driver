// kX SoundFont Manager (sfman32)
// Copyright (c) Eugene Gavrilov, 2004.
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


#include <windows.h>
#include <stdio.h>

// driver part
#include "SFDEVMAN.H"

// user part
#include "SFMAN.H"

// reg part
#include "SFMANREG.H"

#include "debug.h"

#define MAX_SF_PROVIDERS	16
#define MAX_SF_DEVICES		(MAX_SF_PROVIDERS*2)

#define REG_ROOT 		"Software\\SoundFont\\Compatible Devices"

// #include "sfman32u.h"

#define check_dev(a)  		if((int)a>=total_devices) return SFERR_DEVICE_NOT_FOUND;
#define check_handle(a) 	if(!device_handles[a].handle) return SFERR_DEVICE_INVALID;
#define check(a) 		if(!a) return SFERR_UNDEFINE_ERROR;

typedef struct
{
 char path[MAX_PATH];
 char entry[64];
 HINSTANCE instance;
 PFNSFDEVMANAGER manager;
 int num_devs;
}sf_providers_t;

typedef struct
{
 SFDEVHANDLE handle;
 int provider;
 int subdev;
 int cubase;
}sf_handles_t;

sf_handles_t device_handles[MAX_SF_DEVICES];
sf_providers_t sf_providers[MAX_SF_PROVIDERS];
int total_devices=0;

HINSTANCE hinst=0;
int process_count=0;

inline LRESULT call_driver(SFDEVINDEX dev,UINT msg, LPARAM p1,LPARAM p2)
{
 if(sf_providers[device_handles[dev].provider].manager)
  return sf_providers[device_handles[dev].provider].manager(device_handles[dev].handle,msg,p1,p2);
 else
  return SFERR_DEVICE_NOT_FOUND;
}

LRESULT CDECL SF_GetNumDevs(WORD *d)
{
 if(d)
  *d=(WORD)total_devices;

// debug("kx sfman32: GetNumDevs [%d]\n",total_devices);
 return SFERR_NOERR;
}

// Device functions which do not require allocation (opening)
LRESULT CDECL SF_GetDevCaps101(SFDEVINDEX ndx, CSFCapsObject *caps)
{
 // debug("kx sfman32: GetDevCaps (1.01) - %x %d\n",caps,caps?caps->m_SizeOf:0);

 check_dev(ndx);
 check(caps);

 CSFDevObject10x dev;
 memset(&dev,0,sizeof(dev));
 dev.m_SizeOf=SFDEV_CAPSIZE_102; // sizeof(CSFDevObject)

 LRESULT ret=call_driver(ndx,SFDEV_GET_DEVICE_CAPS,device_handles[ndx].subdev,(LPARAM)&dev);
 if(ret==SFERR_NOERR)
 {
  // check for Vienna / sfedt32.dll
  if(caps && caps->m_SizeOf==0)
  {
    debug("kx sfman32: hack for vienna32 [zero-sized caps->m_SizeOf] - Vienna programmers, FIX IT! :-)\n");
    caps->m_SizeOf=sizeof(CSFCapsObject);
  }

  if(caps && caps->m_SizeOf>=sizeof(CSFCapsObject))
  {
   memset(caps,0,sizeof(CSFCapsObject));
   caps->m_SizeOf=sizeof(CSFCapsObject);
   caps->m_RouterID=0;
   caps->m_DevCaps=dev.m_DevCaps;
   caps->m_MoreDevCaps=0;       /* More capabilities                        */
   caps->m_RomID=dev.m_RomId;   /* ID for a Sound ROM used in a device      */
   caps->m_RomVer=dev.m_RomVer; /* Version for a Sound ROM used in a device */

   strncpy(caps->m_DevMgrName,sf_providers[device_handles[ndx].provider].path,sizeof(caps->m_DevMgrName));   /* File Name of the Device Manager DLL.     */ 

   strncpy(caps->m_DevName,dev.m_DevName,sizeof(caps->m_DevName));      /* MMSystem name for MIDI Output Device     */
   strncpy(caps->m_DevMgrEntry,sf_providers[device_handles[ndx].provider].entry,sizeof(caps->m_DevMgrEntry));  /* Entry point inside the Device Manager DLL*/

   return ret;
  }
  if(caps)
   caps->m_SizeOf=sizeof(CSFCapsObject);

  return SFERR_NOERR; // SFERR_RESOURCE_INSUFFICIENT;
 }
 else return ret;
}

LRESULT CDECL SF_GetRouterID   ( SFDEVINDEX ndx, DWORD *p) 
{
 // debug("kx sfman32: get router ID {null}\n");

 check_dev(ndx);
 check(p);

 if(p)
  *p=0;

 return SFERR_NOERR;
}

LRESULT CDECL SF_IsDeviceFree101  ( SFDEVINDEX ndx) 
{
 // debug("kx sfman32: IsDeviceFree\n");

 check_dev(ndx);

 return call_driver(ndx,SFDEV_IS_DEVICE_FREE,device_handles[ndx].subdev,0);
}

LRESULT CDECL SF_Open101          ( SFDEVINDEX ndx ) 
{
 // debug("kx sfman32: Open [%d]\n",ndx);

 check_dev(ndx);

 if(device_handles[ndx].handle!=0)
 {
  if(device_handles[ndx].cubase)
  {
   debug("kx sfman32: Cubase SX finally decided to open the device -- nice ;)\n");
   device_handles[ndx].cubase=0;
   return SFERR_NOERR;
  }
  else
  {
   debug("kx sfman32: ERROR device already opened\n");
   return SFERR_DEVICE_BUSY;
  }
 }

 return call_driver(ndx,SFDEV_OPEN,(LPARAM)&device_handles[ndx].handle,device_handles[ndx].subdev);
}

LRESULT CDECL SF_Close101         ( SFDEVINDEX ndx) 
{
 // debug("kx sfman32: Close [%d]\n",ndx);

 check_dev(ndx);

 if(device_handles[ndx].handle)
 {
  LRESULT ret=call_driver(ndx,SFDEV_CLOSE,0,0);
  if(ret==0)
   device_handles[ndx].handle=0;   
  return ret;
 }
 else
  return SFERR_DEVICE_INVALID;
}

// Query functions
LRESULT CDECL SF_IsMIDIBankUsed (SFDEVINDEX ndx, WORD *a)
{
 // debug("kx sfman32: IsMidiBankUsed [%d/%x/%d]\n",a?*a:0,a,ndx);

 check_dev(ndx);
 // cubase sx seems to forget to open the device!..
 // check_handle(ndx);
 if(!device_handles[ndx].handle)
 {
  debug("kx sfman32: Cubase SX problem: IsMIDIBankUsed called before SF_Open [hack provided] -- Cubase programmers! FIX IT! btw, there's no need to call GetSynthEmulation so often...\n");
  if(call_driver(ndx,SFDEV_OPEN,(LPARAM)&device_handles[ndx].handle,device_handles[ndx].subdev)==SFERR_NOERR)
  {
   device_handles[ndx].cubase=1;
  }
 }
 check(a);

 LRESULT ret=call_driver(ndx,SFDEV_IS_MIDI_BANK_USED,(LPARAM)a,0);

 return ret;
}

LRESULT CDECL SF_QueryStaticSampleMemorySize101 ( SFDEVINDEX ndx, PDWORD max_mem, PDWORD avl_mem)
{
 // debug("kx sfman32: QueryStaticSampleMemorySize\n");

 check_dev(ndx);
 check_handle(ndx);

 return call_driver(ndx,SFDEV_QUERY_STATIC_SMEM_SIZE,(LPARAM)max_mem,(LPARAM)avl_mem);
}

// Synthesizer Emulation Functions
LRESULT CDECL SF_GetAllSynthEmulations101 ( SFDEVINDEX ndx, CSFBufferObject *b) 
{
 // debug("kx sfman32: GetAllSynthEmulations\n");

 check_dev(ndx);
 check_handle(ndx);
 check(b);
 
 CSFDevBufferObject buf;
 buf.m_Size=b->m_Size;
 buf.m_Buffer=b->m_Buffer;
 buf.m_Flag=b->m_Flag;

 LRESULT ret=call_driver(ndx,SFDEV_QUERY_SYNTH_SUPPORT,(LRESULT)&buf,0);
 if(ret==SFERR_NOERR)
 {
  b->m_Size=buf.m_Size;
  b->m_Flag=buf.m_Flag;
  b->m_SizeUsed=buf.m_Var.m_SizeUsed;
 }
 return ret;
}


LRESULT CDECL SF_GetSynthEmulation101     ( SFDEVINDEX ndx, CSFBufferObject *b) 
{
 // debug("kx sfman32: GetSynthEmulation\n");

 check_dev(ndx);
 check_handle(ndx);
 check(b);

 CSFDevBufferObject buf;
 buf.m_Size=b->m_Size;
 buf.m_Buffer=b->m_Buffer;
 buf.m_Flag=b->m_Flag;

 LRESULT ret=call_driver(ndx,SFDEV_GET_SYNTH_EMULATION,(LRESULT)&buf,0);
 if(ret==SFERR_NOERR)
 {
  b->m_Size=buf.m_Size;
  b->m_Flag=buf.m_Flag;
  b->m_SizeUsed=buf.m_Var.m_SizeUsed;
 }
 return ret;
}


LRESULT CDECL SF_SelectSynthEmulation101 ( SFDEVINDEX ndx, WORD emu) 
{
 // debug("kx sfman32: SelectSynthEmulation\n");

 check_dev(ndx);
 check_handle(ndx);

 return call_driver(ndx,SFDEV_SELECT_SYNTH_EMULATION,emu,0);
}

// Sound Bank management functions
LRESULT CDECL SF_LoadBank101  ( SFDEVINDEX ndx, CSFMIDILocation *p,CSFBufferObject *b) 
{
 // debug("kx sfman32: LoadBank\n");

 check_dev(ndx);
 check_handle(ndx);
 check(p);
 check(b);

 CSFDevBufferObject buf;
 buf.m_Size=b->m_Size;
 buf.m_Buffer=b->m_Buffer;
 buf.m_Flag=b->m_Flag;

 LPARAM ret=call_driver(ndx,SFDEV_LOAD_BANK,(LPARAM)p->m_BankIndex,(LPARAM)&buf);
 b->m_SizeUsed=buf.m_Var.m_SizeUsed;
 return ret;
}


LRESULT CDECL SF_GetLoadedBankDescriptor ( SFDEVINDEX ndx, CSFMIDILocation *p,CSFBufferObject *b) 
{
 // debug("kx sfman32: GetLoadedBankDescriptor\n");

 check_dev(ndx);
 check_handle(ndx);
 check(p);
 check(b);
 
 CSFDevBufferObject buf;
 buf.m_Size=b->m_Size;
 buf.m_Buffer=b->m_Buffer;
 buf.m_Flag=b->m_Flag;

 LPARAM ret=call_driver(ndx,SFDEV_GET_BANK_DESCRIPTOR,(LPARAM)p->m_BankIndex,(LPARAM)&buf);
 b->m_SizeUsed=buf.m_Var.m_SizeUsed;
 return ret;
}
                                        
LRESULT CDECL SF_GetLoadedBankPathname ( SFDEVINDEX ndx, CSFMIDILocation *p,CSFBufferObject *b) 
{
 // debug("kx sfman32: GetLoadedBankPathname\n");

 check_dev(ndx);
 check_handle(ndx);
 check(p);
 check(b);

 CSFDevBufferObject buf;
 buf.m_Size=b->m_Size;
 buf.m_Buffer=b->m_Buffer;
 buf.m_Flag=p->m_BankIndex;

 LPARAM ret=call_driver(ndx,SFDEV_GET_BANK_PATHNAME,(LPARAM)&buf,0);
 b->m_SizeUsed=buf.m_Var.m_SizeUsed;
 return ret;
}

LRESULT CDECL SF_GetUserBank10u ( DWORD ndx, SFMIDIBANK bb,CSFBufferObject *b) 
{
 CSFMIDILocation p;
 p.m_BankIndex=bb.m_BankNum;

 return SF_GetLoadedBankPathname((SFDEVINDEX)ndx,&p,b);
}

LRESULT CDECL SF_ClearLoadedBank101 ( SFDEVINDEX ndx, CSFMIDILocation *p) 
{
 // debug("kx sfman32: ClearLoadedBank\n");

 check_dev(ndx);
 check_handle(ndx);
 check(p);

 return call_driver(ndx,SFDEV_CLEAR_BANK,(LPARAM)p->m_BankIndex,0);
}

// Sound Preset management functions
LRESULT CDECL SF_LoadPreset101 ( SFDEVINDEX ndx, CSFMIDILocation *p1,CSFMIDILocation *p2,CSFBufferObject *b) 
{
 // debug("kx sfman32: LoadPreset\n");

 check_dev(ndx);
 check_handle(ndx);
 check(p1);
 check(p2);
 check(b);

 CSFDevBufferObject buf;
 buf.m_Size=b->m_Size;
 buf.m_Buffer=b->m_Buffer;
 buf.m_Flag=b->m_Flag;

 buf.m_Var.m_MIDILocation.m_BankIndex=p1->m_BankIndex;
 buf.m_Var.m_MIDILocation.m_PresetIndex=p1->m_PresetIndex;

 CSFDevParamObject param;
 param.m_BankIndex=p2->m_BankIndex;
 param.m_PresetIndex=p2->m_PresetIndex;

 LPARAM ret=call_driver(ndx,SFDEV_LOAD_PRESET,(LPARAM)&param,(LPARAM)&buf);
 b->m_SizeUsed=buf.m_Var.m_SizeUsed;
 return ret;
}


LRESULT CDECL SF_GetLoadedPresetDescriptor ( SFDEVINDEX ndx, CSFMIDILocation *p,CSFBufferObject *b) 
{
 // debug("kx sfman32: GetLoadedPresetDescriptor\n");

 check_dev(ndx);
 check_handle(ndx);
 check(p);
 check(b);

 CSFDevBufferObject buf;
 buf.m_Size=b->m_Size;
 buf.m_Buffer=b->m_Buffer;
 buf.m_Flag=b->m_Flag;

 CSFDevParamObject param;
 param.m_BankIndex=p->m_BankIndex;
 param.m_PresetIndex=p->m_PresetIndex;

 LPARAM ret=call_driver(ndx,SFDEV_GET_PRESET_DESCRIPTOR,(LPARAM)&param,(LPARAM)&buf);
 b->m_SizeUsed=buf.m_Var.m_SizeUsed;
 return ret;
}


LRESULT CDECL SF_ClearLoadedPreset101 (SFDEVINDEX ndx, CSFMIDILocation *p) 
{
 // debug("kx sfman32: ClearLoadedPreset\n");

 check_dev(ndx);
 check_handle(ndx);
 check(p);

 return call_driver(ndx,SFDEV_CLEAR_PRESET,(LPARAM)p->m_BankIndex,(LPARAM)p->m_PresetIndex);
}


// Sound Waveform management functions
LRESULT CDECL SF_LoadWaveform  ( SFDEVINDEX ndx, CSFMIDILocation *p,CSFBufferObject *b) 
{
 // debug("kx sfman32: LoadWaveform\n");

 check_dev(ndx);
 check_handle(ndx);
 check(p);
 check(b);

 CSFDevBufferObject buf;
 buf.m_Size=b->m_Size;
 buf.m_Buffer=b->m_Buffer;
 buf.m_Flag=b->m_Flag;

 CSFDevParamObject param;
 param.m_BankIndex=p->m_BankIndex;
 param.m_PresetIndex=p->m_PresetIndex;

 LPARAM ret=call_driver(ndx,SFDEV_LOAD_WAVEFORM,(LPARAM)&param,(LPARAM)&buf);
 b->m_SizeUsed=buf.m_Var.m_SizeUsed;
 return ret;
}

LRESULT CDECL SF_ClearLoadedWaveform  ( SFDEVINDEX ndx, CSFMIDILocation *p) 
{
 // debug("kx sfman32: ClearLoadedWaveform\n");

 check_dev(ndx);
 check_handle(ndx);
 check(p);

 return call_driver(ndx,SFDEV_CLEAR_WAVEFORM,(LPARAM)p->m_BankIndex,(LPARAM)p->m_PresetIndex);
}

// Device Specific Message
LRESULT CDECL SF_DeviceSpecificMessage ( SFDEVINDEX ndx, UINT msg, LPARAM p1, LPARAM p2) 
{
 debug("kx sfman32: DeviceSpecificMessage\n");

 check_dev(ndx);
 check_handle(ndx);

 return call_driver(ndx,msg,p1,p2);
}


SFMANL101API list101x=
{
    SF_GetNumDevs,
    SF_GetDevCaps101,
    SF_GetRouterID,
    SF_IsDeviceFree101,
    SF_Open101,
    SF_Close101,
    SF_IsMIDIBankUsed,
    SF_QueryStaticSampleMemorySize101,
    SF_GetAllSynthEmulations101,
    SF_GetSynthEmulation101,
    SF_SelectSynthEmulation101,
    SF_LoadBank101,
    SF_GetLoadedBankDescriptor,
    SF_GetLoadedBankPathname,
    SF_ClearLoadedBank101,
    SF_LoadPreset101,
    SF_GetLoadedPresetDescriptor,
    SF_ClearLoadedPreset101,
    SF_LoadWaveform,
    SF_ClearLoadedWaveform,
    SF_DeviceSpecificMessage
};

SFMANL100API list100x=
{
    SF_GetNumDevs,
    SF_GetDevCaps101,
    SF_GetRouterID,
    SF_IsDeviceFree101,
    SF_Open101,
    SF_Close101,
    SF_IsMIDIBankUsed,
    SF_QueryStaticSampleMemorySize101,
    SF_GetAllSynthEmulations101,
    SF_GetSynthEmulation101,
    SF_SelectSynthEmulation101,
    SF_LoadBank101,
    SF_GetLoadedBankDescriptor,
//    SF_GetLoadedBankPathname,
    SF_ClearLoadedBank101,
    SF_LoadPreset101,
    SF_GetLoadedPresetDescriptor,
    SF_ClearLoadedPreset101,
    SF_LoadWaveform,
    SF_ClearLoadedWaveform,
    SF_DeviceSpecificMessage
};

LRESULT CDECL SF_QueryInterface(INTERFACEID id, DWORD_PTR *dw)
{
 switch(id)
 {
  case ID_SFMANL100API:
  	#ifdef USE_U
  	*dw=(DWORD_PTR)&list100u;
  	#else
        *dw=(DWORD_PTR)&list100x;
  	#endif
  	debug("kx sfman32: using interface %s -- Sonar programmers: USE THE RECENT SDK! [1.01]\n","1.00/u");
  	return SFERR_NOERR;
  case ID_SFMANL101API:
  	*dw=(DWORD_PTR)&list101x;
  	// debug("kx sfman32: using interface %s\n","1.01/x");
  	return SFERR_NOERR;
 }
 debug("kx sfman32: interface '%d' is not supported\n",id);
 return SFERR_INTERFACE_NOT_SUPPORTED;
}

extern "C" __declspec(dllexport) SFMANAGER SFManager={ &SF_QueryInterface };

extern "C" __declspec(dllexport) LRESULT __cdecl SF_RegisterDevice ( void *a)
{
 // check versions...
 SF_REGISTERINFO10x *r1=(SF_REGISTERINFO10x *)a;
 SF_REGISTERINFO10u *r2=(SF_REGISTERINFO10u *)a;

 CHAR szManagerName[20];
 CHAR szManagerEntryPointName[20];

 if(r1->szManagerEntryPointName[0]==0)
 {
  // assume 10u
  strncpy(szManagerName,r2->szManagerName,20);  
  strncpy(szManagerEntryPointName,r2->szManagerEntryPointName,20);
 }
 else
 {
  // 10x
  strncpy(szManagerName,r1->szManagerName,20);
  strncpy(szManagerEntryPointName,r1->szManagerEntryPointName,20);
 }

 debug("kx sfman32: registering %s.%s\n",szManagerName,szManagerEntryPointName);

 int ret=SFERR_NOERR;

 char tmp[512];
 sprintf(tmp,"%s\\%s",REG_ROOT,szManagerName);

 HKEY key;
 if(RegCreateKey(HKEY_LOCAL_MACHINE,tmp,&key)==ERROR_SUCCESS)
 {
  if(RegSetValue(key,"Manager Entry Point",REG_SZ,szManagerEntryPointName,(DWORD)strlen(szManagerEntryPointName))!=ERROR_SUCCESS)
   ret=SFERR_FAIL_CREATEKEY;

  RegCloseKey(key);
  return ret;
 }
  return SFERR_FAIL_CREATEKEY;
}

extern "C" __declspec(dllexport) LRESULT __cdecl SF_UnRegisterDevice (void *a) 
{
 LPSTR txt1=(LPSTR) a;
 LPSTR txt2=((SF_REGISTERINFO10u *)a)->szManagerName;

 LPSTR txt;
 if(strstr(txt1,".DLL")!=NULL || strstr(txt1,".dll")!=NULL)
  txt=txt1;
 else
  if(strstr(txt2,".DLL")!=NULL || strstr(txt2,".dll")!=NULL)
   txt=txt2;
   else
   {
    debug("kx sfman32: invalid unregister call\n");
    return SFERR_INVALID_KEY;
   }

 int ret=SFERR_NOERR;

 char tmp[512];
 sprintf(tmp,"%s\\%s",REG_ROOT,txt);

 HKEY key;
 if(RegOpenKey(HKEY_LOCAL_MACHINE,tmp,&key)==ERROR_SUCCESS)
 {
  RegDeleteValue(key,"Manager Entry Point");
  RegCloseKey(key);

  if(RegOpenKey(HKEY_LOCAL_MACHINE,REG_ROOT,&key)==ERROR_SUCCESS)
  {
   RegDeleteKey(key,txt);
   RegCloseKey(key);
  }

  return ret;
 }
  return SFERR_INVALID_KEY;
}

void upload_provider(int ndx,char *path,char *entry)
{
 memset(&sf_providers[ndx],0,sizeof(sf_providers[ndx]));

 strcpy(sf_providers[ndx].path,path);
 strcpy(sf_providers[ndx].entry,entry);
 sf_providers[ndx].instance=LoadLibrary(sf_providers[ndx].path);
 if(sf_providers[ndx].instance)
 {
  sf_providers[ndx].manager=(PFNSFDEVMANAGER)GetProcAddress(sf_providers[ndx].instance,sf_providers[ndx].entry);

  if(sf_providers[ndx].manager)
  {
   LPARAM tmp=0;
   if(sf_providers[ndx].manager(0,SFDEV_GET_NUM_DEVS,(LPARAM)&tmp,0)==0)
   {
    sf_providers[ndx].num_devs=(int)tmp;

    for(int i=0;i<tmp;i++)
    {
     device_handles[total_devices+i].provider=ndx;
     device_handles[total_devices+i].subdev=i;
    }
    total_devices+=(int)tmp;
    debug("kx sfman32: '%s' provider registered [%d subdevices]\n",path,tmp);
   } else debug("kx sfman32: '%s' provider failed to enumerate\n",path);
  } else debug("kx sfman32: '%s' provider doesn't have '%s' entry\n",path,entry);
 } else debug("kx sfman32: '%s' provider could not be loaded\n",path);
}

void init(void)
{
 debug("kx sfman32: init\n");

 memset(sf_providers,0,sizeof(sf_providers));
 memset(device_handles,0,sizeof(device_handles));

 // FIXME parse registry instead!
 upload_provider(0,"KXAPI.DLL","_kx_sfont_entry@16");
 upload_provider(1,"AWEMAN32.DLL","AWEManager");
 // debug("kx sfman32: %d active devices\n",total_devices);
}

void close(void)
{
 for(int i=0;i<total_devices;i++)
 {
  if(device_handles[i].handle)
  {
    debug("kx sfman32: device is still opened [%d]\n",i);

    sf_providers[device_handles[i].provider].manager(device_handles[i].handle,SFDEV_CLOSE,0,0);
    device_handles[i].handle=0;
  }
 }
 for(int i=0;i<MAX_SF_PROVIDERS;i++)
 {
  if(sf_providers[i].instance)
  {
   FreeLibrary(sf_providers[i].instance);

   memset(&sf_providers[i],0,sizeof(sf_providers[i]));
  }
 }

 total_devices=0;
 debug("kx sfman32: close\n");
}

extern "C" BOOL    WINAPI   DllMain (HANDLE hInstance, 
                            ULONG ulReason,
                            LPVOID )
{
    switch (ulReason) 
    {
		case DLL_PROCESS_ATTACH:
			process_count++;
			if(process_count==1)
			{
			 hinst=(HINSTANCE)hInstance;
			 init();
			}
			break;
		case DLL_PROCESS_DETACH:
			process_count--;
			if(process_count==0)
			 close();
			break;
    }
    return TRUE;
}
