// kX Setup
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


#include "stdafx.h"
#include "debug.h"
#include <direct.h>

#include "util.h"

#include "sfman\sfmanreg.h"
#include "../kxmixer/resource.h"
#include "interface\kxapi.h"
#include "asio.h"

int failure=0;

int kx_uninstall_driver();
int kx_install_driver();
int kx_clean_all();

int complex_ask(TCHAR *t,int cleanup_only,int procedure,int hw_type)
{
 // procedure: 0 - remove, 1 - install
 // hw_type: 0 - 10kx 1 - joystick
 int ret=0;
 kString s;

 if(cleanup_only==1)
 {
  if(procedure==0)
   // remove the following device
   s.Format("%s\n%s\n(%s)",mf.get_profile(_T("setup"),_T("setup7")),get_friendly_name(t),t);
  else
   s.Format("Internal error here [%d %d %d] - answer 'no' and report bug #10029\n",cleanup_only,procedure,hw_type);
 }
 else
 {
  if(procedure==1)
  {
   // do you wish to install?
   s.Format("%s\n%s\n(%s)",mf.get_profile(_T("setup"),_T("setup9")),get_friendly_name(t),t);
  }
  if(procedure==0) // 'remove' under win98 in order to install later
  {
   s.Format("%s:\n%s\n(%s)",mf.get_profile(_T("setup"),_T("setup8")),get_friendly_name(t),t);
  }
 }

  ret=MessageBox(NULL,(LPCTSTR)s,_T("kX Setup"),MB_YESNO);

 if(ret==IDYES)
  return 1;
 else
  return 0;
}

int check_version_and_uninstall(int force)
{
 kSettings cfg;
 TCHAR version[64];

 if(!force)
 if(cfg.read_abs(_T("Setup"),_T("Version"),version,sizeof(version))==0)
 {
  TCHAR cur_version[64];
  _stprintf(cur_version,"%s",KX_DRIVER_VERSION_STR);

  if(_tcscmp(version,cur_version)!=0)
  {
    TCHAR tmp_str[512];
    sprintf(tmp_str,mf.get_profile(_T("setup"),_T("setup13")),version);

    if(MessageBox(NULL,tmp_str,_T("kX Setup"),MB_YESNO)==IDYES)
    {
     force=1;
     debug("kxsetup: version '%s' found - force uninstall [per user request]\n",version);
    }
  }
 }

 if(force)
   kx_uninstall_driver();

 return 0;
}

void kx_setup(kWindow *w,int remove_only)
{
 int driver_removed=0;
 int total=0;

 failure=0;

 kx_close();

 debug(_T("kxsetup: %s the driver [kx_install]\n"),remove_only?_T("uninstall"):_T("install"));

 BOOL reboot=0;

 if(!remove_only)
  MessageBox(w?w->get_wnd():NULL,(LPCTSTR)mf.get_profile(_T("setup"),_T("setup1")),_T("kX Setup"),MB_OK|MB_ICONINFORMATION);

 if(remove_only)
 {
  // don't :) ask to completely uninstall driver files / registry entries
  // if(MessageBox(w?w->get_wnd():NULL,(LPCTSTR)mf.get_profile(_T("setup"),_T("setup10")),_T("kX Setup"),MB_YESNO)==IDYES)
  {
   check_version_and_uninstall(1);
   driver_removed=1;
  }
 }

 // enumerate all the devices
 {
    HDEVINFO DeviceInfoSet;
    SP_DEVINFO_DATA DeviceInfoData;
    DWORD i,err;

    debug(_T("kxsetup: enumerate all the devices\n"));
    
    // Create a Device Information Set with all present devices.
    DeviceInfoSet = SetupDiGetClassDevs(NULL, // All Classes
        0,
        0, 
        DIGCF_ALLCLASSES/*|DIGCF_PRESENT*/);
    if (DeviceInfoSet == INVALID_HANDLE_VALUE)
    {
        debug(_T("kxsetup: SetupDiGetClassDevs() failed\n"));
        kString s;
        MessageBox(NULL,(LPCTSTR)mf.get_profile(_T("errors"),_T("setup11")),
                        (LPCTSTR)mf.get_profile(_T("errors"),_T("severe")),
                        MB_OK|MB_ICONSTOP);
        return;
    }

   
    //  Enumerate through all Devices.

    DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    for (i=0;SetupDiEnumDeviceInfo(DeviceInfoSet,i,&DeviceInfoData);i++)
    {
        DWORD DataT=0;
        LPTSTR p=0,buffer = NULL;
        DWORD buffersize = 0;

        while (SetupDiGetDeviceRegistryProperty(
            DeviceInfoSet,
            &DeviceInfoData,
            SPDRP_HARDWAREID,
            &DataT,
            (PBYTE)buffer,
            buffersize,
            &buffersize)!=TRUE || (buffer==NULL))
        {
            int e=GetLastError();
            if (e == ERROR_INVALID_DATA)
            {
                // May be a Legacy Device with no HardwareID. Continue.
                break;
            }
            else if (e == ERROR_INSUFFICIENT_BUFFER || buffer==NULL)
            {
                // We need to change the buffer size.
                if (buffer) 
                    LocalFree(buffer);
                buffer = (TCHAR *)LocalAlloc(LPTR,buffersize);
            }
            else
            {
                // Unknown Failure.
                SetLastError(e);
                goto cleanup_DeviceInfo;
            }            
        }

        int e=GetLastError();

        if (e == ERROR_INVALID_DATA)
        {
         continue;
        }

        if (e != 0)
        {
            kString s;
            s.Format((LPCTSTR)mf.get_profile(_T("errors"),_T("setup6")),e,e);
            MessageBox(NULL,(LPCTSTR)s,_T("Ok"),MB_OK|MB_ICONSTOP);
            continue;
        }
        
        // Compare each entry in the buffer multi-sz list with our HardwareID.
        for (p=buffer;p&&(*p)&&(p<&buffer[buffersize]);p+=lstrlen(p)+sizeof(TCHAR))
        {
            if( (_tcsstr(p,_T("VEN_1102&DEV_0002"))!=NULL) || // 10k1
                (_tcsstr(p,_T("VEN_1102&DEV_0004"))!=NULL) || // 10k2
                (_tcsstr(p,_T("VEN_1102&DEV_0008"))!=NULL)    // 10k2
              )
            {
             total++;

             if(remove_only)
             {
                 if(complex_ask(p,remove_only,0,0))
                 {
                    debug(_T("kxsetup: found device to remove... -- DIF_REMOVE [%s]\n"),p);
                    if (!SetupDiCallClassInstaller(DIF_REMOVE,DeviceInfoSet,&DeviceInfoData))
                    {
                        err=GetLastError();
                        MessageBox(NULL,(LPCTSTR)mf.get_profile(_T("errors"),_T("setup7")),p,MB_OK|MB_ICONSTOP);
                    }
                 }
             }
             else
             {
               if(complex_ask(p,remove_only,1,0))
               {
                 reboot=FALSE;

                 if(!driver_removed)
                 {
                  check_version_and_uninstall(0);
                  driver_removed=1;
                 }

                 debug(_T("kxsetup install via UpdateDriverForPlugAndPlayDevice\n"));
                 if(UpdateDriverForPlugAndPlayDevices(w?w->get_wnd():NULL,p,_T(".\\kx.inf"),INSTALLFLAG_FORCE,&reboot)==TRUE)
                 {
                    debug(_T("kxsetup: method 1.1 succeeded!\n"));
                 }
                 else
                 {
                    debug(_T("kxsetup: method 1.1 failed [%x]\n"),GetLastError());
                    failure=1;
                 }
               }
             } // not remove_only?

             break;
            } // if id ok?
        } // for each id
        
        if (buffer) LocalFree(buffer);
    }

    //
    //  Cleanup.
    //    
cleanup_DeviceInfo:
    err = GetLastError();
    SetupDiDestroyDeviceInfoList(DeviceInfoSet);
    SetLastError(err);
  }

  if(remove_only)
  {
    return;
  }

  debug("kxsetup: total number of devices: %d\n",total);

  kx_install_driver();

  if(failure)
    MessageBox(w?w->get_wnd():NULL,_T("Warning: failed to update the driver"),_T("kX Setup"),MB_OK|MB_ICONSTOP);

  if(total==0)
     MessageBox(w?w->get_wnd():NULL,(LPCTSTR)mf.get_profile(_T("setup"),_T("setup19")),_T("kX Setup"),MB_OK|MB_ICONSTOP);
  else
  {
     MessageBox(w?w->get_wnd():NULL,(LPCTSTR)mf.get_profile(_T("setup"),_T("setup18")),_T("kX Setup"),MB_OK);

     prepare_reboot(w);
  }
}

void register_startup(void)
{
  // register 'startup' item
  {
    TCHAR dir[MAX_PATH]; dir[0]='"'; GetCurrentDirectory(MAX_PATH-1,dir+1);
    strcat(dir,"\\kxmixer.exe\" --startup");
    HKEY hKey=0;
    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"),NULL,KEY_ALL_ACCESS,&hKey)!=ERROR_SUCCESS)
    {
            RegCreateKey(HKEY_LOCAL_MACHINE,"Software\\Microsoft\\Windows\\CurrentVersion\\Run",&hKey);
    }
    if(RegSetValueEx(hKey,"kX Mixer",0,REG_SZ,(const unsigned char *)dir,(DWORD)strlen(dir))!=ERROR_SUCCESS)
     debug("!! kxsetup: failed to add 'run' entry\n");
    RegCloseKey(hKey);
  }
}

void unregister_startup(void)
{
    HKEY hKey;
    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"),NULL,KEY_ALL_ACCESS,&hKey)==ERROR_SUCCESS)
    {
        if(RegDeleteValue(hKey,_T("kX Mixer"))!=ERROR_SUCCESS)
         debug("!! kxsetup: failed to remove 'run' entry\n");
        RegCloseKey(hKey);
    }
    else
     debug("!! kxsetup: failed to open 'run' key\n");
}

void remove_inf_files(void)
{
  TCHAR win_dir[MAX_PATH]; GetWindowsDirectory(win_dir,MAX_PATH);

  for(int i=0;i<1000;i++)
  {
        TCHAR file_name[24];        _stprintf(file_name,_T("oem%d.inf"),i);
        TCHAR file_path[MAX_PATH];  _stprintf(file_path,_T("%s\\inf\\%s"),win_dir,file_name);

        int to_del=0;
        FILE *f;
        f=_tfopen(file_path,_T("rt"));
        if(f)
        {
              char *m;
              m=(char *)malloc(8192);
              if(m)
              {
                memset(m,0,8192);
                fread(m,1,8192-1,f);
                if(
                    (strstr(m,_T("kX Audio Driver"))!=NULL) ||
                    (strstr(m,_T("PCI\\VEN_1102&DEV_0002"))!=NULL) ||
                    (strstr(m,_T("PCI\\VEN_1102&DEV_0008"))!=NULL) ||
                    (strstr(m,_T("PCI\\VEN_1102&DEV_0004"))!=NULL)
                  )
                {
                    to_del=1;
                }
                free(m);
              }

              fclose(f);

              if(to_del)
              {
                   // uninstall INF file
                   SetupUninstallOEMInf(file_name,SUOI_FORCEDELETE,NULL);
                   
                   // delete INF file
                   _tunlink(file_path);
              }
        }
  } // for
}

void install_inf_file(void)
{
    TCHAR dir[MAX_PATH];
    TCHAR cwd[MAX_PATH];

    GetCurrentDirectory(MAX_PATH,dir);
    _tcscpy(cwd,dir);
    _tcscat(dir,_T("\\kx.inf"));

    SetupCopyOEMInf(dir,cwd,SPOST_PATH,0,NULL,0,0,0);
}

void install_vsti(void)
{
    char *pm=getenv("ProgramFiles");

#if defined(_WIN64)
    int i=0;
    TCHAR cwd[MAX_PATH]; GetCurrentDirectory(MAX_PATH,cwd);
AGAIN:
#endif

    TCHAR dir[MAX_PATH]; _stprintf(dir,_T("%s\\VSTPlugins"),pm);
    if(GetFileAttributes(dir)==INVALID_FILE_ATTRIBUTES) // try another folder:
    {
        _stprintf(dir,_T("%s\\Steinberg\\VSTPlugins"),pm);
        if(GetFileAttributes(dir)==INVALID_FILE_ATTRIBUTES)
        {
            _stprintf(dir,_T("%s\\Steinberg"),pm);
            CreateDirectory(dir,NULL);
            _stprintf(dir,_T("%s\\Steinberg\\VSTPlugins"),pm);
            CreateDirectory(dir,NULL);
        }
    }
    TCHAR file[MAX_PATH]; 
    
    _tcscpy(file,dir);
    _tcscat(file,_T("\\kXi.dll"));

    debug(_T("kxsetup: installing '%s': ret: %d\n"),file,CopyFile(_T("kXi.dll"),file,FALSE)); // overwrite

    _tcscpy(file,dir);
    _tcscat(file,_T("\\kxsfi.dll"));

    debug(_T("kxsetup: installing '%s': ret: %d\n"),file,CopyFile(_T("kxsfi.dll"),file,FALSE)); // overwrite

    #if defined(_WIN64)
    if(i!=0)
    {
        SetCurrentDirectory(cwd);
        return;
    }
    i++;
    pm=getenv("ProgramFiles(x86)");

    _stprintf(dir,_T("%s\\kX Project"),pm);
    SetCurrentDirectory(dir);

    goto AGAIN;
    #endif
}

void uninstall_vsti(void)
{
    char *pm=getenv("ProgramFiles");

#if defined(_WIN64)
    int i=0;
    TCHAR cwd[MAX_PATH]; GetCurrentDirectory(MAX_PATH,cwd);
AGAIN:
#endif

    TCHAR dir[MAX_PATH]; _stprintf(dir,_T("%s\\VSTPlugins"),pm);
    if(GetFileAttributes(dir)==INVALID_FILE_ATTRIBUTES) // try another folder:
    {
        _stprintf(dir,_T("%s\\Steinberg\\VSTPlugins"),pm);
        if(GetFileAttributes(dir)==INVALID_FILE_ATTRIBUTES)
        {
            // nop
        }
    }
    TCHAR file[MAX_PATH]; 
    
    _tcscpy(file,dir);
    _tcscat(file,_T("\\kXi.dll"));

    debug(_T("kxsetup: removing '%s', ret: %d\n"),file,_tunlink(file));

    _tcscpy(file,dir);
    _tcscat(file,_T("\\kxsfi.dll"));

    debug(_T("kxsetup: removing '%s', ret: %d\n"),file,_tunlink(file));

    #if defined(_WIN64)
    if(i!=0)
    {
        SetCurrentDirectory(cwd);
        return;
    }
    i++;
    pm=getenv("ProgramFiles(x86)");

    _stprintf(dir,_T("%s\\kX Project"),pm);
    SetCurrentDirectory(dir);

    goto AGAIN;
    #endif
}

void unregister_sfman_manually(void)
{
    // unregister sfman32 manually
    SHDeleteKey(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\SoundFont\\Compatible Devices\\kxapi.dll"));
    // SHDeleteKey is reported to access 32-bit view instead of 64-bit view under WinXP x64, workaround:
    RegDeleteKey(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\SoundFont\\Compatible Devices\\kxapi.dll"));

    #if defined(_WIN64)
    RegDeleteKeyEx(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\SoundFont\\Compatible Devices\\kxapi.dll"),KEY_WOW64_32KEY,0);
    #endif
}

void unregister_sfman(void)
{
    // SoundFont Manager un-registration
    try
    {
        HINSTANCE sfm=LoadLibrary(_T("sfman32.dll"));
        if(sfm!=NULL)
        {
         typedef LRESULT (__cdecl * SFReg_UnRegisterDevice_t)(LPSTR name);
         SFReg_UnRegisterDevice_t f;
         f=(SFReg_UnRegisterDevice_t)GetProcAddress(sfm,"SF_UnRegisterDevice");
         if(f)
         {
           debug(_T("kxsetup: unregistering sfman32 (normal)\n"));
           f("kxapi.dll");
         }
         FreeLibrary(sfm);
        }
    }
    catch(...)
    {
        debug(_T("kxsetup: sfman32.dll caused an error. un-registering manually...\n"));
    }

    unregister_sfman_manually();
}

void register_sfman(void)
{
  // SoundFont Manager registration
  HINSTANCE sfm=NULL;
  try
  {
    sfm=LoadLibrary("sfman32.dll");
    if(sfm==NULL)
    {
     MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","sfman"),"kX Setup",MB_OK|MB_ICONEXCLAMATION);
    }
    else
    {
     typedef LRESULT (__cdecl * SFReg_RegisterDevice_t)(PSF_REGISTERINFO10x info);
     SFReg_RegisterDevice_t f;
     f=(SFReg_RegisterDevice_t)GetProcAddress(sfm,"SF_RegisterDevice");
     if(f)
     {
       SF_REGISTERINFO10x ri;
       memset(&ri,0,sizeof(ri));
       strcpy(ri.szManagerName,"kxapi.dll");
       strcpy(ri.szManagerEntryPointName,"_kx_sfont_entry@16");

       // if(MessageBox(NULL,"Do you wish to register with sfman32.dll?\n(this may cause a crash)","kX Mixer",MB_YESNO)==IDYES)
       {
         if(f(&ri)!=0) // SF_NOERR
          {;}
         // (else?) MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","sfman2"),"kX Setup",MB_OK|MB_ICONEXCLAMATION);
       }
     } // else MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","sfman3"),"kX Setup",MB_OK|MB_ICONEXCLAMATION);

     FreeLibrary(sfm);
     sfm=0;
    }
  }
  catch(...)
  {
   debug("kxmixer: sfman32.dll caused an exception -- will register with sfman manually...\n");
   if(sfm)
       FreeLibrary(sfm);
  }

  // uncompatible registration method
  HKEY hKey;
  if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,"Software\\SoundFont\\Compatible Devices\\kxapi.dll",NULL,KEY_ALL_ACCESS,&hKey)!=ERROR_SUCCESS)
  {
    DWORD r;
    RegCreateKeyEx(HKEY_LOCAL_MACHINE,"Software\\SoundFont\\Compatible Devices\\kxapi.dll",NULL,NULL,0,KEY_ALL_ACCESS,NULL,&hKey,&r);
  }
  if(hKey)
  {
    RegSetValueEx(hKey,"Manager Entry Point",NULL,REG_SZ,(unsigned char *)"_kx_sfont_entry@16",19);
    RegCloseKey(hKey);
  }

  // register 32-bit, too
#if defined(_WIN64)
  if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,"Software\\SoundFont\\Compatible Devices\\kxapi.dll",NULL,KEY_ALL_ACCESS|KEY_WOW64_32KEY,&hKey)!=ERROR_SUCCESS)
  {
    DWORD r;
    RegCreateKeyEx(HKEY_LOCAL_MACHINE,"Software\\SoundFont\\Compatible Devices\\kxapi.dll",NULL,NULL,0,KEY_ALL_ACCESS|KEY_WOW64_32KEY,NULL,&hKey,&r);
  }
  if(hKey)
  {
    RegSetValueEx(hKey,"Manager Entry Point",NULL,REG_SZ,(unsigned char *)"_kx_sfont_entry@16",19);
    RegCloseKey(hKey);
  }
#endif
}

TCHAR *reg_list[]=
{
    _T("CLSID\\{5C0B12B1-F582-4935-BFAE-85267A6BE13D}"),
    _T("SOFTWARE\\ASIO\\kX ASIO Driver"),
    _T("CLSID\\{5771C802-034B-4ac1-BE54-36DADD447207}"),
    _T("SOFTWARE\\ASIO\\kX ASIO Driver 1"),
    _T("CLSID\\{8A1080D5-A695-498a-8188-2F0B57870A7F}"),
    _T("SOFTWARE\\ASIO\\kX ASIO Driver 2"),
    _T("CLSID\\{6732CDC3-569B-4541-9060-D7290050E7A3}"),
    _T("SOFTWARE\\ASIO\\kX ASIO Driver 3"),
    _T("CLSID\\{FEDE17F2-7108-4c64-9801-1627B75ACFC9}"),
    _T("SOFTWARE\\ASIO\\kX ASIO Driver 4"),
    _T("CLSID\\{2B156F48-B8EF-4a1e-95DF-E384E8B64842}"),
    _T("SOFTWARE\\ASIO\\kX ASIO Driver 5"),
    _T("CLSID\\{2D98497D-A875-481d-A3D3-7A159642101F}"),
    _T("SOFTWARE\\ASIO\\kX ASIO Driver 6"),
    _T("CLSID\\{66649E8B-9864-43ee-AE93-558E90D58BB3}"),
    _T("SOFTWARE\\ASIO\\kX ASIO Driver 7"),

    NULL
};

void unregister_asio_manually(void)
{
  // manually remove registry items, too

  int i=0;
  while(reg_list[i])
  {
    SHDeleteKey(HKEY_CLASSES_ROOT,reg_list[i]);
    SHDeleteKey(HKEY_LOCAL_MACHINE,reg_list[i+1]);

    RegDeleteKey(HKEY_CLASSES_ROOT,reg_list[i]);
    RegDeleteKey(HKEY_LOCAL_MACHINE,reg_list[i+1]);

  #if defined(_WIN64)
    RegDeleteKeyEx(HKEY_CLASSES_ROOT,reg_list[i],KEY_WOW64_32KEY,0);
    RegDeleteKeyEx(HKEY_LOCAL_MACHINE,reg_list[i+1],KEY_WOW64_32KEY,0);
  #endif

    i+=2;
  }
}

void unregister_asio(void)
{
  // call regsvr32
  {
    TCHAR dir[MAX_PATH];
    GetSystemDirectory(dir,sizeof(dir));
    TCHAR regsvr_path[MAX_PATH];
    sprintf(regsvr_path,"%s\\%s",dir,"regsvr32.exe");

    dir[0]='"';
    GetCurrentDirectory(MAX_PATH-1,dir+1);
    _tcscat(dir,"\\kxasio.dll\"");

    debug("kxmixer: calling regsvr to unregister ASIO [%s %s]\n",regsvr_path,dir);
    debug("kxmixer: regsvr32 returned: %d\n",spawnl(P_WAIT,regsvr_path,regsvr_path,"/s /u",dir,NULL));
  }

  // call regsvr32 for x86-component, too
#if defined(_WIN64)
  // call regsvr32
  {
    TCHAR dir[MAX_PATH];
    GetSystemWow64Directory(dir,sizeof(dir));
    TCHAR regsvr_path[MAX_PATH];
    sprintf(regsvr_path,"%s\\%s",dir,"regsvr32.exe");

    _stprintf(dir,_T("\"%s\\kX Project\\kxasio.dll\""),getenv("ProgramFiles(x86)"));

    debug("kxmixer: calling regsvr to unregister ASIO [%s %s]\n",regsvr_path,dir);
    debug("kxmixer: regsvr32 returned: %d\n",spawnl(P_WAIT,regsvr_path,regsvr_path,"/s /u",dir,NULL));
  }
#endif

  unregister_asio_manually();
}


void register_asio(void)
{
  // register ASIO
  {
    TCHAR dir[MAX_PATH];
    GetSystemDirectory(dir,sizeof(dir));
    TCHAR regsvr_path[MAX_PATH];
    sprintf(regsvr_path,"%s\\%s",dir,"regsvr32.exe");

    dir[0]='"';
    GetCurrentDirectory(MAX_PATH-1,dir+1);
    _tcscat(dir,"\\kxasio.dll\"");

    debug("kxmixer: calling regsvr to register ASIO [%s %s]\n",regsvr_path,dir);
    debug("kxmixer: regsvr32 returned: %d\n",spawnl(P_WAIT,regsvr_path,regsvr_path,"/s",dir,NULL));
  }

  // call regsvr32 for x86-component, too
#if defined(_WIN64)
  {
    TCHAR dir[MAX_PATH];
    GetSystemWow64Directory(dir,sizeof(dir));
    TCHAR regsvr_path[MAX_PATH];
    sprintf(regsvr_path,"%s\\%s",dir,"regsvr32.exe");

    _stprintf(dir,_T("\"%s\\kX Project\\kxasio.dll\""),getenv("ProgramFiles(x86)"));

    debug("kxmixer: calling regsvr to unregister ASIO [%s %s]\n",regsvr_path,dir);
    debug("kxmixer: regsvr32 returned: %d\n",spawnl(P_WAIT,regsvr_path,regsvr_path,"/s",dir,NULL));
  }
#endif
}

void unregister_shell(void)
{
  // shell un-registration:
        RegDeleteKey(HKEY_CLASSES_ROOT,".kx");
        SHDeleteKey(HKEY_CLASSES_ROOT,"kXSettings");

        RegDeleteKey(HKEY_CLASSES_ROOT,".da");
        SHDeleteKey(HKEY_CLASSES_ROOT,"kXDaneSource");

        RegDeleteKey(HKEY_CLASSES_ROOT,".kxp");
        SHDeleteKey(HKEY_CLASSES_ROOT,"kXPluginPresets");

        RegDeleteKey(HKEY_CLASSES_ROOT,".kxsfc");
        SHDeleteKey(HKEY_CLASSES_ROOT,"kXSoundFontCollection");

        RegDeleteKey(HKEY_CLASSES_ROOT,".rifx");
        SHDeleteKey(HKEY_CLASSES_ROOT,"kXRifxBinary");

        RegDeleteKey(HKEY_CLASSES_ROOT,".kxl");
        SHDeleteKey(HKEY_CLASSES_ROOT,"kXFXLibrary");

        RegDeleteKey(HKEY_CLASSES_ROOT,".kxa");
        SHDeleteKey(HKEY_CLASSES_ROOT,"kXAddOn");

        RegDeleteKey(HKEY_CLASSES_ROOT,".kxs");
        SHDeleteKey(HKEY_CLASSES_ROOT,"kXSkinFile");
}

void register_shell(void)
{
        TCHAR tmp_file[MAX_PATH];
        TCHAR mixer_file[MAX_PATH]; mixer_file[0]='"';
        GetCurrentDirectory(MAX_PATH-1,mixer_file+1);
        _tcscat(mixer_file,_T("\\kxmixer.exe\""));
        _stprintf(tmp_file,"%s,%d",mixer_file,-IDR_ICON_KX);
        TCHAR cmd[MAX_PATH];

        create_reg(HKEY_CLASSES_ROOT,".kx",NULL,"kXSettings");
        create_reg(HKEY_CLASSES_ROOT,"kXSettings",NULL,"kX Settings file");
        create_reg(HKEY_CLASSES_ROOT,"kXSettings\\DefaultIcon",NULL,tmp_file);
        create_reg(HKEY_CLASSES_ROOT,"kXSettings\\shell",NULL,"Restore");
        sprintf(cmd,"%s --shell --load-settings %%1",mixer_file);
        create_reg(HKEY_CLASSES_ROOT,"kXSettings\\shell\\Restore\\command",NULL,cmd);

        sprintf(tmp_file,"%s,%d",mixer_file,-IDR_ICON_DA);
        create_reg(HKEY_CLASSES_ROOT,".da",NULL,"kXDaneSource");
        create_reg(HKEY_CLASSES_ROOT,"kXDaneSource",NULL,"kX Dane Source file");
        create_reg(HKEY_CLASSES_ROOT,"kXDaneSource\\shell",NULL,"Register");
        create_reg(HKEY_CLASSES_ROOT,"kXDaneSource\\DefaultIcon",NULL,tmp_file);
        sprintf(cmd,"%s --shell --register-dane %%1",mixer_file);
        create_reg(HKEY_CLASSES_ROOT,"kXDaneSource\\shell\\Register\\command",NULL,cmd);
        sprintf(cmd,"%s --shell --edit-dane %%1",mixer_file);
        create_reg(HKEY_CLASSES_ROOT,"kXDaneSource\\shell\\Edit\\command",NULL,cmd);

        sprintf(tmp_file,"%s,%d",mixer_file,-IDR_ICON_KXP);
        create_reg(HKEY_CLASSES_ROOT,".kxp",NULL,"kXPluginPresets");
        create_reg(HKEY_CLASSES_ROOT,"kXPluginPresets",NULL,"kX Plugin Presets");
        create_reg(HKEY_CLASSES_ROOT,"kXPluginPresets\\shell",NULL,"Import Preset");
        create_reg(HKEY_CLASSES_ROOT,"kXPluginPresets\\DefaultIcon",NULL,tmp_file);
        sprintf(cmd,"%s --shell --import-preset %%1",mixer_file);
        create_reg(HKEY_CLASSES_ROOT,"kXPluginPresets\\shell\\Import Preset\\command",NULL,cmd);

        sprintf(tmp_file,"%s,%d",mixer_file,-IDR_ICON_SFC);
        create_reg(HKEY_CLASSES_ROOT,".kxsfc",NULL,"kXSoundFontCollection");
        create_reg(HKEY_CLASSES_ROOT,"kXSoundFontCollection",NULL,"kX SoundFont Collection");
        create_reg(HKEY_CLASSES_ROOT,"kXSoundFontCollection\\shell",NULL,"Load");
        create_reg(HKEY_CLASSES_ROOT,"kXSoundFontCollection\\DefaultIcon",NULL,tmp_file);
        sprintf(cmd,"%s --shell --sfc %%1",mixer_file);
        create_reg(HKEY_CLASSES_ROOT,"kXSoundFontCollection\\shell\\Load\\command",NULL,cmd);

        sprintf(tmp_file,"%s,%d",mixer_file,-IDR_ICON_RIFX);
        create_reg(HKEY_CLASSES_ROOT,".rifx",NULL,"kXRifxBinary");
        create_reg(HKEY_CLASSES_ROOT,"kXRifxBinary",NULL,"kX RIFX Binary file");
        create_reg(HKEY_CLASSES_ROOT,"kXRifxBinary\\DefaultIcon",NULL,tmp_file);
        create_reg(HKEY_CLASSES_ROOT,"kXRifxBinary\\shell",NULL,"Register");
        sprintf(cmd,"%s --shell --register-rifx %%1",mixer_file);
        create_reg(HKEY_CLASSES_ROOT,"kXRifxBinary\\shell\\Register\\command",NULL,cmd);
        sprintf(cmd,"%s --shell --edit-rifx %%1",mixer_file);
        create_reg(HKEY_CLASSES_ROOT,"kXRifxBinary\\shell\\Open in kX Editor\\command",NULL,cmd);

        sprintf(tmp_file,"%s,%d",mixer_file,-IDR_ICON_KXL);
        create_reg(HKEY_CLASSES_ROOT,".kxl",NULL,"kXFXLibrary");
        create_reg(HKEY_CLASSES_ROOT,"kXFXLibrary",NULL,"kX DSP Effects Library");
        create_reg(HKEY_CLASSES_ROOT,"kXFXLibrary\\DefaultIcon",NULL,tmp_file);
        create_reg(HKEY_CLASSES_ROOT,"kXFXLibrary\\shell",NULL,"Register");
        sprintf(cmd,"%s --shell --register-kxl %%1",mixer_file);
        create_reg(HKEY_CLASSES_ROOT,"kXFXLibrary\\shell\\Register\\command",NULL,cmd);

        sprintf(tmp_file,"%s,%d",mixer_file,-IDR_ICON_KXA);
        create_reg(HKEY_CLASSES_ROOT,".kxa",NULL,"kXAddOn");
        create_reg(HKEY_CLASSES_ROOT,"kXAddOn",NULL,"kX Add-On");
        create_reg(HKEY_CLASSES_ROOT,"kXAddOn\\DefaultIcon",NULL,tmp_file);
        create_reg(HKEY_CLASSES_ROOT,"kXAddOn\\shell",NULL,"Register");
        sprintf(cmd,"%s --shell --register-kxa %%1",mixer_file);
        create_reg(HKEY_CLASSES_ROOT,"kXAddOn\\shell\\Register\\command",NULL,cmd);

        sprintf(tmp_file,"%s,%d",mixer_file,-IDR_ICON_KXS);
        create_reg(HKEY_CLASSES_ROOT,".kxs",NULL,"kXSkinFile");
        create_reg(HKEY_CLASSES_ROOT,"kXSkinFile",NULL,"kX Skin File");
        create_reg(HKEY_CLASSES_ROOT,"kXSkinFile\\DefaultIcon",NULL,tmp_file);
        create_reg(HKEY_CLASSES_ROOT,"kXSkinFile\\shell",NULL,"Register");
        sprintf(cmd,"%s --shell --register-skin %%1",mixer_file);
        create_reg(HKEY_CLASSES_ROOT,"kXSkinFile\\shell\\Register\\command",NULL,cmd);

        SHChangeNotify(SHCNE_ASSOCCHANGED,0,0,0);
}

int kx_install_driver(void)
{
    debug(_T("kxsetup: install kx drivers\n"));

    kx_clean_all();

    debug(_T("kxsetup: install INF file\n"));
    install_inf_file();

    debug(_T("kxsetup: install VSTi\n"));
    install_vsti();

    debug(_T("kxsetup: register sfman\n"));
    register_sfman();

    debug(_T("kxsetup: register ASIO\n"));
    register_asio();

    debug(_T("kxsetup: register shell\n"));
    register_shell();

    debug(_T("kxsetup: register start-up\n"));
    register_startup();

    debug(_T("kxsetup: update ASIO names\n"));
    update_asio_drivers();

    return 0;
}

int kx_uninstall_driver(void)
{
    debug(_T("kxsetup: uninstall kx drivers\n"));

    kx_clean_all();

    uninstall_vsti();

    debug(_T("kxsetup: deleting oemXX.inf files\n"));
    remove_inf_files();

    debug(_T("kxsetup: unregister sfman32\n"));
    unregister_sfman();

    debug(_T("kxsetup: unregister ASIO\n"));
    unregister_asio();

    debug(_T("kxsetup: unregister shell\n"));
    unregister_shell();

    debug(_T("kxsetup: unregister start-up\n"));
    unregister_startup();

    SHDeleteKey(HKEY_CURRENT_USER,_T("SOFTWARE\\kX"));

    return 0;
}

void reset_kx_settings(void)
{
    debug(_T("kxsetup: resetting settings\n"));

    debug(_T("kxsetup: reset sfman\n"));
    unregister_sfman();
    register_sfman();

    debug(_T("kxsetup: reset ASIO\n"));
    unregister_asio();
    register_asio();
    update_asio_drivers();

    debug(_T("kxsetup: reset shell\n"));
    unregister_shell();
    register_shell();

    debug(_T("kxsetup: reset start-up\n"));
    unregister_startup();
    register_startup();
}

const TCHAR *file_list[]=
{
    _T("cmax20.dll"),
    _T("edspctrl.exe"),
    _T("kxaddons.kxa"),
    _T("kxapi.dll"),
    _T("kxasio.dll"),
    _T("kxctrl.exe"),
    _T("kxdynamica.kxl"),
    _T("kxefx.dll"),
    _T("kxefxreverb.kxl"),
    _T("kxefxtube.kxl"),
    _T("kxfxadc.kxl"),
    _T("kxfxlib.kxl"),
    _T("kxfxloudness.kxl"),
    _T("kxfxmixy42.kxl"),
    _T("kxfxmixy82.kxl"),
    _T("kxfxpack.kxl"),
    _T("kxfxrouter.kxl"),
    _T("kxgui.dll"),
    _T("kXi.dll"),
    _T("kxm120.kxl"),
    _T("kxmixer.exe"),
    _T("kxsetup.exe"),
    _T("kxsfi.dll"),
    _T("sfman32.dll"),
    _T("drivers\\kx.sys"),
    _T("kx.sys"),
    _T("kxefx.kxs"),
    _T("kxhelp.chm"),
    _T("kxskin.kxs"),
    _T("kxTimeBalanceV2A.kxs"),
    _T("kxTimeBalanceV2B.kxs"),   
    NULL
};

int kx_clean_all()
{
    debug(_T("kxsetup: cleaning everything up\n"));
    
    // this is 'legacy' code, since kX no longer installs into system32, except for sfman32.dll
    
    TCHAR sys_dir[MAX_PATH]; GetSystemDirectory(sys_dir,sizeof(sys_dir));
    TCHAR file[MAX_PATH];

    int i=0;
    while(file_list[i])
    {
        _stprintf(file,"%s\\%s",sys_dir,file_list[i]);
        unlink(file);
        i++;
    }

    unregister_asio_manually();
    unregister_shell();
    unregister_sfman_manually();
    unregister_startup();

    return 0;
}
