// kX Mixer
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


#include "stdinc.h"
#include "main_dlg.h"
#include "notify.h"

#include "addonmgr.h"

#include "sfman/sfmanreg.h"

static int kx_found=0;

extern kFile mf2;
extern HWND systray;
int add_skin(const char *fname,int force);

int get_mixer_folder(TCHAR *folder)
{
 GetModuleFileName(NULL,folder,MAX_PATH);

 TCHAR *p=0;
 p = _tcsrchr(folder,'\\');
 if(p)
 {
     p++;
     if(_tcschr(p,'"'))
     {
      *p=0;
      _tcscat(folder,_T("\""));
     }
     else
     {
      *p=0;
     }
 }

 return 0;
}


BOOL CALLBACK MyEnumWindowsProc(
    HWND hwnd,  // handle to parent window
    LPARAM lParam   // application-defined value
   )
{
    char name[1024];
    GetWindowText(hwnd,name,1024);
    if(strcmp(name,KX_DEF_WINDOW)==NULL) // found; linked
    {
     if(lParam==0)
     {
      PostMessage(hwnd,WM_KXMIXER_PROCESS_MENU,0,2); // open kxmixer window (main)
                                                     // 3551: was SendMessage, changed to PostMessage to avoid lock-ups if kxmixer has hung up
     }
     if(lParam==1)
     {
      kx_found++;
     }
     if(lParam==2) // process command line
     {
           debug("Mixer: executing %s\n",GetCommandLine());
           if(OpenClipboard(hwnd))
           {
            HGLOBAL mem=GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE|GMEM_ZEROINIT,strlen(GetCommandLine())+1);
            if(mem)
            {
             LPVOID m=GlobalLock(mem);
             strcpy((char *)m,GetCommandLine());

             EmptyClipboard();
             
             SetClipboardData(CF_TEXT, mem);
             GlobalUnlock(mem);
            }
            CloseClipboard();
           }
       SendMessage(hwnd,WM_KXMIXER_PROCESS_CMDLINE,0,0);
     }
     if(lParam==3) // close
     {
       SendMessage(hwnd,WM_CLOSE,0,0);
     }
    }
    return TRUE;
}

int sanity_check()
{
    // sanity check
    if(gui_get_version()!=KX_VERSION_DWORD)
    {
     MessageBox(NULL,"Fatal error: Incorrect kX GUI API version. Reinstall kX Software","Error",MB_OK|MB_ICONSTOP);
     return -1;
    }

        // sanity check
    {
     iKX test;
     if(test.get_version_dword()!=KX_VERSION_DWORD)
     {
      MessageBox(NULL,"Fatal error: Incorrect kX API version. Reinstall kX Software","Error",MB_OK|MB_ICONSTOP);
      return -2;
     }
    }

    // sanity check [kxfxlib.kxl]
    {
        int err=0;

            HMODULE mm;
            mm=LoadLibrary("kxfxlib.kxl");
            if(mm)
            {
                        kxplugin_publish_t pp;
                        pp=(kxplugin_publish_t)GetProcAddress(mm,"publish_plugins");
                        if(pp)
                        {
                          uintptr_t ret;
                          if(pp(KXPLUGIN_GET_FXLIB_VER,0,&ret)==0)
                          {
                           if(ret!=KX_VERSION_DWORD)
                            err=4;
                          } else err=3;
                        } else err=2;
                
                FreeLibrary(mm);
            } else err=1;

            if(err)
            {
             switch(err)
             {
                case 1:
                case 2: MessageBox(NULL,"Fatal error: Incorrect kX FX Library. Reinstall kX Software","Error",MB_OK|MB_ICONSTOP); break;
                case 3:
                case 4: MessageBox(NULL,"Fatal error: Incorrect kX FX Library version. Reinstall kX Software","Error",MB_OK|MB_ICONSTOP); break;
         }
         debug("kxmixer: kxfxlib error is %d\n",err);
             return -3;
            }
    }

    // sanity check [kxaddons.kxa]
    {
        int err=0;

            HMODULE mm;
            mm=LoadLibrary("kxaddons.kxa");
            if(mm)
            {
                        kxaddon_publish_t pp;
                        pp=(kxaddon_publish_t)GetProcAddress(mm,"publish_addons");
                        if(pp)
                        {
                          uintptr_t ret;
                          if(pp(KXADDON_GET_LIB_VER,0,&ret)==0)
                          {
                           if(ret!=KX_VERSION_DWORD)
                            err=4;
                          } else err=3;
                        } else err=2;
                
                FreeLibrary(mm);
            } else err=1;

            if(err)
            {
             switch(err)
             {
                case 1:
                case 2: MessageBox(NULL,"Fatal error: Incorrect kX Add-on Library. Reinstall kX Software","Error",MB_OK|MB_ICONSTOP); break;
                case 3:
                case 4: MessageBox(NULL,"Fatal error: Incorrect kX Add-on Library version. Reinstall kX Software","Error",MB_OK|MB_ICONSTOP); break;
         }
         debug("kxmixer: kxaddon error is %d\n",err);
             return -3;
            }
    }

    return 0;
}

int shell_integration()
{
        // shell integration
        if(strstr(GetCommandLine(),"--shell")!=NULL)
        {
            EnumWindows(MyEnumWindowsProc,2);
            return -1;
        }

        // force quit
        if(strstr(GetCommandLine(),"--quit")!=NULL)
        {
            EnumWindows(MyEnumWindowsProc,3);
            return -2;
        }

        // look for uploaded mixers
        kx_found=0;
    EnumWindows(MyEnumWindowsProc,1); // look for KX Mixer
    if(kx_found!=0)
    {
       EnumWindows(MyEnumWindowsProc,0); // activate it
       return -3;
    }

    return 0;
}

static HRESULT CreateLink(LPCSTR lpszPathObj, 
    LPCSTR lpszPathLink, LPSTR lpszDesc,LPSTR path,LPSTR params) 
{ 
    HRESULT hres; 
    IShellLink *psl; 
 
    // Get a pointer to the IShellLink interface. 
    hres = CoCreateInstance(CLSID_ShellLink, NULL, 
        CLSCTX_INPROC_SERVER, IID_IShellLink, (void **)&psl); 
    if (SUCCEEDED(hres)) { 
        IPersistFile* ppf; 
 
        // Set the path to the shortcut target and add the 
        // description. 
        psl->SetPath(lpszPathObj); 
        if(path)
         psl->SetWorkingDirectory(path);
        psl->SetDescription(lpszDesc); 
        if(params)
         psl->SetArguments(params);
 
       // Query IShellLink for the IPersistFile interface for saving the 
       // shortcut in persistent storage. 
        hres = psl->QueryInterface(IID_IPersistFile, 
            (void **)&ppf); 
 
        if (SUCCEEDED(hres)) { 
            WORD wsz[MAX_PATH]; 
 
            // Ensure that the string is ANSI. 
            MultiByteToWideChar(CP_ACP, 0, lpszPathLink, -1, 
                wsz, MAX_PATH); 
 
            // Save the link by calling IPersistFile::Save. 
            hres = ppf->Save(wsz, TRUE); 
            ppf->Release(); 
        }
        psl->Release(); 
    } 
    return hres; 
} 

static void create_reg(HKEY where,const char *path,char *item,char *value)
{
 HKEY hKey;
  if(RegOpenKeyEx(where,path,NULL,KEY_ALL_ACCESS,&hKey)!=ERROR_SUCCESS)
  {
    DWORD r;
    RegCreateKeyEx(where,path,NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,&r);
  }
  if(hKey)
  {
    RegSetValueEx(hKey,item,NULL,REG_SZ,(unsigned char *)value,strlen(value)+1);
    RegCloseKey(hKey);
  }
}

void g_reset_settings(int force) // 0: generic 1-first-time 2-inv. version
{
  debug("kxmixer: resetting global settings...\n");
  debug("kxmixer: deleting registry entries...\n");

  SHDeleteKey(HKEY_CURRENT_USER,"Software\\kX\\Mixer");
  // this also deletes current language/logo/...

  RegDeleteKey(HKEY_CURRENT_USER,"Software\\kX\\Folders");

  if(force==1) // first-time, no 'Setup\\Version' key at all...
  {
    SHDeleteKey(HKEY_CURRENT_USER,"Software\\kX\\Plugins");
    SHDeleteKey(HKEY_CURRENT_USER,"Software\\kX\\Addons");
    SHDeleteKey(HKEY_CURRENT_USER,"Software\\kX\\Skins");
    SHDeleteKey(HKEY_CURRENT_USER,"Software\\kX\\Setup");
  }
  if(force==2) // inv. version found
  {
    if(MessageBox(NULL,mf.get_profile(_T("setup"),_T("setup15")),_T("kX Mixer"),MB_YESNO)!=IDYES)
    {
      SHDeleteKey(HKEY_CURRENT_USER,"Software\\kX\\Plugins");
      SHDeleteKey(HKEY_CURRENT_USER,"Software\\kX\\Addons");
      SHDeleteKey(HKEY_CURRENT_USER,"Software\\kX\\Skins");
      SHDeleteKey(HKEY_CURRENT_USER,"Software\\kX\\Setup");
    }
  }
  if(force==0) // generic
  {
    if(MessageBox(NULL,mf.get_profile(_T("setup"),_T("setup16")),_T("kX Mixer"),MB_YESNO)==IDYES)
    {
      SHDeleteKey(HKEY_CURRENT_USER,"Software\\kX\\Plugins");
      SHDeleteKey(HKEY_CURRENT_USER,"Software\\kX\\Addons");
      SHDeleteKey(HKEY_CURRENT_USER,"Software\\kX\\Skins");
      SHDeleteKey(HKEY_CURRENT_USER,"Software\\kX\\Setup");
    }
  }

  // call setup's reset_kx_settings();
  {
         TCHAR mixer_folder[MAX_PATH]; mixer_folder[0]='"'; mixer_folder[1]=0;
         get_mixer_folder(mixer_folder);
         _tcscat(mixer_folder,_T("kxsetup.exe"));

         MessageBox(NULL,(LPCTSTR)mf.get_profile("setup","setup20"),_T("kX Mixer"),MB_OK|MB_ICONINFORMATION);
         HINSTANCE hi=ShellExecute(NULL,"open",mixer_folder,"--reset",NULL,SW_SHOWDEFAULT);

         debug("kxmixer: launching setup: %s %d %d %d\n",mixer_folder,(intptr_t)hi,GetLastError(),errno);
  }

  // register plugins
  {
      iKXPluginManager pm;
      iKX kx;
      TCHAR file_name[MAX_PATH];

      strcpy(file_name,"kxfxlib.kxl");
      if(pm.register_plugin(file_name,0)<=0)
       MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","fxregister"),"kX Mixer Error",MB_OK|MB_ICONEXCLAMATION);

      // register FX Pack effects
      strcpy(file_name,"kxefxtube.kxl");
      if(pm.register_plugin(file_name,0)<=0)
       debug("! kxmixer: kxefxtube not registered\n");

      strcpy(file_name,"kxefxreverb.kxl");
      if(pm.register_plugin(file_name,0)<=0)
       debug("! kxmixer: kxefxreverb not registered\n");

      strcpy(file_name,"kxfxpack.kxl");
      if(pm.register_plugin(file_name,0)<=0)
       debug("! kxmixer: kxefxpack not registered\n");

      strcpy(file_name,"kxdynamica.kxl");
      if(pm.register_plugin(file_name,0)<=0)
       debug("! kxmixer: kxefxdynamica not registered\n");

      strcpy(file_name,"kxfxmixy42.kxl");
      if(pm.register_plugin(file_name,0)<=0)
       debug("! kxmixer: kxfxmixy42 not registered\n");

      strcpy(file_name,"kxfxmixy82.kxl");
      if(pm.register_plugin(file_name,0)<=0)
       debug("! kxmixer: kxfxmixy82 not registered\n");

      strcpy(file_name,"kxfxloudness.kxl");
      if(pm.register_plugin(file_name,0)<=0)
       debug("! kxmixer: kxfxloudness not registered\n");

      strcpy(file_name,"kxfxrouter.kxl");
      if(pm.register_plugin(file_name,0)<=0)
       debug("! kxmixer: kxfxrouter not registered\n");

      strcpy(file_name,"kxfxadc.kxl");
      if(pm.register_plugin(file_name,0)<=0)
       debug("! kxmixer: kxfxadc not registered\n");

      strcpy(file_name,"ProFx.kxl");
      if(pm.register_plugin(file_name,0)<=0)
       debug("! kxmixer: profx not registered\n");

      strcpy(file_name,"asynth.kxl");
      if(pm.register_plugin(file_name,0)<=0)
       debug("! kxmixer: asynth not registered\n");

      strcpy(file_name,"drumsynth.kxl");
      if(pm.register_plugin(file_name,0)<=0)
       debug("! kxmixer: drumsynth not registered\n");

      strcpy(file_name,"organ.kxl");
      if(pm.register_plugin(file_name,0)<=0)
       debug("! kxmixer: organ not registered\n");

      strcpy(file_name,"ufx.kxl");
      if(pm.register_plugin(file_name,0)<=0)
       debug("! kxmixer: ufx not registered\n");

      strcpy(file_name,"kxm120.kxl");
      if(pm.register_plugin(file_name,0)<=0)
       debug("! kxmixer: kxm120 not registered\n");

      strcpy(file_name,"sa.minus.kxl");
      if(pm.register_plugin(file_name,0)<=0)
       debug("! kxmixer: sa.minus not registered\n");

      // register all the microeffects (.da)
      TCHAR sdk_path[MAX_PATH]; _stprintf(sdk_path,_T("SDK\\kxfx_kxm120\\da"));

      _stprintf(file_name,"%s\\kxm100.da",sdk_path);
      pm.register_plugin(file_name,0);
      _stprintf(file_name,"%s\\kxm110.da",sdk_path);
      pm.register_plugin(file_name,0);
      // _stprintf(file_name,"%s\\kxm120.da",sdk_path);
      // pm.register_plugin(file_name,0);
      _stprintf(file_name,"%s\\kxm150.da",sdk_path);
      pm.register_plugin(file_name,0);
      _stprintf(file_name,"%s\\kxm160.da",sdk_path);
      pm.register_plugin(file_name,0);
      _stprintf(file_name,"%s\\kxm170.da",sdk_path);
      pm.register_plugin(file_name,0);
      _stprintf(file_name,"%s\\kxm180.da",sdk_path);
      pm.register_plugin(file_name,0);
      _stprintf(file_name,"%s\\kxm190.da",sdk_path);
      pm.register_plugin(file_name,0);
      _stprintf(file_name,"%s\\kxm200.da",sdk_path);
      pm.register_plugin(file_name,0);
      _stprintf(file_name,"%s\\kxm210.da",sdk_path);
      pm.register_plugin(file_name,0);
      _stprintf(file_name,"%s\\kxm220.da",sdk_path);
      pm.register_plugin(file_name,0);
      _stprintf(file_name,"%s\\kxm300.da",sdk_path);
      pm.register_plugin(file_name,0);
      _stprintf(file_name,"%s\\kxm310.da",sdk_path);
      pm.register_plugin(file_name,0);
      _stprintf(file_name,"%s\\kxm320.da",sdk_path);
      pm.register_plugin(file_name,0);
      _stprintf(file_name,"%s\\kxm330.da",sdk_path);
      pm.register_plugin(file_name,0);
      _stprintf(file_name,"%s\\kxm400.da",sdk_path);
      pm.register_plugin(file_name,0);
      _stprintf(file_name,"%s\\kxm500.da",sdk_path);
      pm.register_plugin(file_name,0);
      _stprintf(file_name,"%s\\kxm510.da",sdk_path);
      pm.register_plugin(file_name,0);
      _stprintf(file_name,"%s\\kxm520.da",sdk_path);
      pm.register_plugin(file_name,0);
      _stprintf(file_name,"%s\\kxm550.da",sdk_path);
      pm.register_plugin(file_name,0);
      _stprintf(file_name,"%s\\kxm560.da",sdk_path);
      pm.register_plugin(file_name,0);
      _stprintf(file_name,"%s\\kxm600.da",sdk_path);
      pm.register_plugin(file_name,0);
      _stprintf(file_name,"%s\\kxm700.da",sdk_path);
      pm.register_plugin(file_name,0);
      _stprintf(file_name,"%s\\kxm800.da",sdk_path);
      pm.register_plugin(file_name,0);
      _stprintf(file_name,"%s\\kxm810.da",sdk_path);
      pm.register_plugin(file_name,0);
      _stprintf(file_name,"%s\\kxm820.da",sdk_path);
      pm.register_plugin(file_name,0);
      _stprintf(file_name,"%s\\kxm830.da",sdk_path);
      pm.register_plugin(file_name,0);
      _stprintf(file_name,"%s\\kxm900.da",sdk_path);
      pm.register_plugin(file_name,0);
      _stprintf(file_name,"%s\\kxm910.da",sdk_path);
      pm.register_plugin(file_name,0);
      
      _stprintf(file_name,"%s\\matrix4x4_m_v1.0.da",sdk_path);
      pm.register_plugin(file_name,0);
      _stprintf(file_name,"%s\\microbit_v1.0.da",sdk_path);
      pm.register_plugin(file_name,0);
      _stprintf(file_name,"%s\\microchorus_v2.0.da",sdk_path);
      pm.register_plugin(file_name,0);
      _stprintf(file_name,"%s\\microdelay_m_v1.0.da",sdk_path);
      pm.register_plugin(file_name,0);
      _stprintf(file_name,"%s\\microdelay_x_v1.0.da",sdk_path);
      pm.register_plugin(file_name,0);
      _stprintf(file_name,"%s\\micropan_v1.0.da",sdk_path);
      pm.register_plugin(file_name,0);
      _stprintf(file_name,"%s\\microphaser_v1.0.da",sdk_path);
      pm.register_plugin(file_name,0);
      _stprintf(file_name,"%s\\microwah-wah_v1.0.da",sdk_path);
      pm.register_plugin(file_name,0);
      _stprintf(file_name,"%s\\space_80_v1.5.da",sdk_path);
      pm.register_plugin(file_name,0);
  }

  // register addons
  {
    debug("kxmixer: registering add-ons\n");
    if(manager->addon_mgr) // there are add-ons currently active
    {
     manager->addon_mgr->close();

     manager->addon_mgr->register_addon("kxaddons.kxa",0);
     manager->addon_mgr->init(KXADDON_SYSTEM_WIDE); // global init
     manager->addon_mgr->reset_all_addon_settings(KXADDON_SYSTEM_WIDE);

     kSettings cfg;
     manager->addon_mgr->save_all_addon_settings(KXADDON_SYSTEM_WIDE,cfg);
    }
    else
    {
     iKXAddOnManager *addon_mgr=new iKXAddOnManager;

     addon_mgr->register_addon("kxaddons.kxa",0);
     addon_mgr->init(KXADDON_SYSTEM_WIDE);
     addon_mgr->reset_all_addon_settings(KXADDON_SYSTEM_WIDE);

     kSettings cfg;
     addon_mgr->save_all_addon_settings(KXADDON_SYSTEM_WIDE,cfg);
     addon_mgr->close();
     delete addon_mgr;
    }
  }

  // register skins
  {
    debug("kxmixer: registering skins...\n");

    add_skin("kxskin.kxs",1); // force

    // now we should detach
    mf.detach_skin();
    mf2.close();
  }

  // asio def device is no longer used
  // cfg.write_abs("ASIO","DefDevice",(dword)0);

  kSettings cfg;     
  cfg.write_abs(_T("Setup"),_T("Version"),KX_DRIVER_VERSION_STR);

  debug("kxmixer: reset global settings finished\n");

  return;
}


int check_first_run()
{
        kSettings cfg;

        int force_setup=0;

        TCHAR version[KX_MAX_STRING];
        if(cfg.read_abs(_T("Setup"),_T("Version"),version,sizeof(version))==0)
        {
         TCHAR cur_version[KX_MAX_STRING];
         _stprintf(cur_version,"%s",KX_DRIVER_VERSION_STR);

         if(_tcscmp(version,cur_version)!=0)
         {
          debug("kxmixer: version '%s' found - force 'post_setup'\n",version);
          force_setup=2; // inv. version
         }
        }
        else
         force_setup=1; // first time

        if(force_setup)
        {
        debug("kxmixer: general post-setup initialization [per-driver]\n");
        CoInitialize(NULL);
                g_reset_settings(force_setup);
                CoUninitialize();
        }

        return 0;
}


extern HMIXER mixer;

void quit_kxmixer(int force_restart)
{
          if(mixer)
          {
           mixerClose(mixer);
           mixer=NULL;
          }

          debug("kxmixer: saving settings...\n");

          if(manager)
          {
           manager->close(force_restart);
           delete manager;
           manager=NULL;
          }
           
      debug("kxmixer: settings saved...\n");

      tray_icon(NIM_DELETE);
}

void kx_shutdown(void)
{
  HWND w=FindWindow("Shell_TrayWnd",NULL);
  if(w)
  {
   INPUT in;

         in.type=INPUT_KEYBOARD;
         in.ki.wVk=(WORD)VK_LWIN;
         in.ki.wScan=0;
         in.ki.dwFlags=0;
         in.ki.time=0;
         in.ki.dwExtraInfo=0;
         SendInput(1,&in,sizeof(INPUT));

         in.type=INPUT_KEYBOARD;
         in.ki.wVk=(WORD)VK_LWIN;
         in.ki.wScan=0;
         in.ki.dwFlags=KEYEVENTF_KEYUP;
         in.ki.time=0;
         in.ki.dwExtraInfo=0;
         SendInput(1,&in,sizeof(INPUT));

//   SendMessage(w,WM_ACTIVATE,WA_ACTIVE,0);
//   SendMessage(w,WM_SETFOCUS,0,0);
   PostMessage(w,WM_CLOSE,0,0);
  }
}

// save/restore folder name
void save_cwd(const char *id)
{
  kSettings cfg;

  char buffer[MAX_PATH];
  GetCurrentDirectory(sizeof(buffer),buffer);
  cfg.write_abs("Folders",id,buffer);
}


void restore_cwd(const char *id)
{
  kSettings cfg;

  char buffer[MAX_PATH];

  if(cfg.read_abs("Folders",id,buffer,MAX_PATH)==0)
    SetCurrentDirectory(buffer);
}

int exist(char *n)
{
    FILE *f;
    f=fopen(n,"rb");
    if(f==NULL)
    {
     return 0;
    }
    fclose(f);
    return 1;
}

#define MAX_LRU 5

void save_lru(char *f)
{
 kSettings cfg;

 char key[2]; 
 char str[1024];
 int size=sizeof(str)-1;

 int i;
 for(i=0;i<MAX_LRU;i++)
 {
   key[0]='a'+i; key[1]=0;
   size=sizeof(str)-1;
   
   if(cfg.read_abs("Folders",key,str,size)==0)
   {
    if(strcmp(f,str)==0)
    {
     break;
    }
   }
   else // Save
   {
    cfg.write_abs("Folders",key,f);
    break;
   }
 }

 if(i==MAX_LRU) // replace
 {
  int pos=-1;

  key[0]='z';
  size=sizeof(str)-1;

  if(cfg.read_abs("Folders",key,str,size)==0)
   pos=(str[0]-'a')&0xf;

  pos++;
  if(pos>=MAX_LRU) 
   pos=0;

  key[0]='a'+pos; key[1]=0;
  cfg.write_abs("Folders",key,f);

  key[0]='z'; key[1]=0;
  str[0]=pos+'a';
  cfg.write_abs("Folders",key,str);
 }
}

kString font_list;
void set_font(kWindow *that,kFont &ff,const char *section)
{
     char font_name[32]; strcpy(font_name,"Tahoma");
     int font_size=90;
     char tmp_font_str[128]; strcpy(tmp_font_str,"90 Tahoma");

     if(!mf.get_profile("all","font",tmp_font_str,sizeof(tmp_font_str)))
     {
          sscanf(tmp_font_str,"%d",&font_size);
          char *p=strchr(tmp_font_str,' ');
          if(p)
           strncpy(font_name,p+1,sizeof(font_name));
     }

     if(!mf.get_profile(section,"font",tmp_font_str,sizeof(tmp_font_str)))
     {
          sscanf(tmp_font_str,"%d",&font_size);
          char *p=strchr(tmp_font_str,' ');
          if(p)
           strncpy(font_name,p+1,sizeof(font_name));
     }

     // try embedded fonts
     char *p=strstr(font_name,".ttf");
     if(p)
     {
          p+=4;
          *p=0;
          p++;
          // font_name points to ttf file; p points to actual font name

          // check, if it was already embedded
          kString tmp;
          tmp+="{";
          tmp+=font_name;
          tmp+="}";
          if(strstr((LPCTSTR)font_list,(LPCTSTR)tmp)==0) // not found
          {
        if(GetProcAddress(LoadLibrary("gdi32.dll"),"AddFontMemResourceEx"))
        {
                int error; size_t size;
                    void *mem=mf.load_data(font_name,&size,&error);
                    if((size>0) && (error==0) && mem)
                    {
                      DWORD num=1;
                      typedef HANDLE (WINAPI *AddFontMemResourceEx_t)(PVOID,DWORD,PVOID,DWORD*);
                      AddFontMemResourceEx_t func=(AddFontMemResourceEx_t)GetProcAddress(LoadLibrary("gdi32.dll"),"AddFontMemResourceEx");

                      if(func && (func(mem,size,0,&num)==0) )
                       debug("Error embedding fonts!\n");
                      else
                      {
                       font_list+="{";
                       font_list+=font_name;
                       font_list+="}";
                      }
                      free(mem);
                    }
            } // function was found in gdi32.dll
          }  // cached

          strncpy(font_name,p,sizeof(font_name));
     } // ".ttf" ?

     HDC dc=::GetDC(that->get_wnd());

     sprintf(tmp_font_str,"%d %s",font_size,font_name);
     gui_create_point_font(*that->get_font(),mf,section,"font",tmp_font_str,dc);

         ::ReleaseDC(that->get_wnd(),dc);
}
