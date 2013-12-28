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
#include "../setup/asio.h"

#define MAX_LRU 5

extern CMainDialog *mixer_window;

// UINT_PTR timer=NULL;
// #define TIMER_RESOLUTION 300 // 300ms = ~3 times per second

BEGIN_MESSAGE_MAP(CControlApp, CWinApp)
    ON_COMMAND(ID_HELP, OnHelp)
END_MESSAGE_MAP()

CControlApp theApp;

iKXManager *manager=NULL;

HMIXER mixer=NULL;
extern HWND systray;

void update_mm_keyboard(dword onoff);
void signal_mm_keyboard(int msg);

int force_restart=0;

int set_skin(int id);
void init_skin(void);
int add_skin(const char *file_name,int force=0);

void init_cmax();
void close_cmax();

int post_setup_sequence(void);
int shell_integration();
int sanity_check();
int check_first_run();
int create_systray_window();
void tray_icon(int cmd);

int exist(char *n);

UINT taskbar_recreated=0;

DEFINE_GUID(KSDATAFORMAT_SUBTYPE_DOLBY_AC3_SPDIF,0x00000092, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);

typedef struct
{
  char name[KX_MAX_STRING+8];
  int id;
}fx_list_t;

static int __cdecl compare_fx(const void *elem1, const void *elem2 )
{
 return strcmp(((fx_list_t *)elem1)->name,((fx_list_t *)elem2)->name);
}

#define MAX_PAGES   10
static char page_guids[MAX_PAGES][KX_MAX_STRING];
static char def_page_guid[KX_MAX_STRING];

int launch_menu()
{
  kMenu menu;
  kMenu device_menu;
  kMenu plugin_menu;
  kMenu settings_menu;
  kMenu internet_menu;
  kMenu skin_menu;
  kMenu lang_menu;
  kMenu addon_menu;
  kMenu tray_launcher_menu;

  menu.create();
  device_menu.create();
  plugin_menu.create();
  settings_menu.create();
  internet_menu.create();
  skin_menu.create();
  lang_menu.create();
  addon_menu.create();
  tray_launcher_menu.create();

  // top menu:
  // ---------
  menu.add(mf.get_profile("menu","mixer"),2);
  menu.add(mf.get_profile("menu","dsp"),3);
  menu.add(mf.get_profile("menu","router"),8);

  dword is_a2=0;
  if(!manager->get_ikx()->get_dword(KX_DWORD_IS_A2,&is_a2))
  {
    if(is_a2)
     menu.add(mf.get_profile("menu","p16v_router"),76);
  }

  menu.add(mf.get_profile("menu","editor"),6);
  menu.add(mf.get_profile("menu","midi"),25);
  if(manager->get_notifier())
    menu.add(mf.get_profile("menu","remote"),24);
  menu.add(mf.get_profile("menu","sfman"),26);
  menu.add(mf.get_profile("menu","console"),74);

  manager->addon_mgr->menu(KXADDON_MENU_TOP,&menu);

  menu.separator();

  // device menu
  // -----------
  manager->create_devices_menu(&device_menu);
  menu.add(mf.get_profile("menu","device"),&device_menu);

  // settings
  // --------
  settings_menu.add(mf.get_profile("menu","saveset"),10);
  settings_menu.add(mf.get_profile("menu","loadset"),11);
  settings_menu.add(mf.get_profile("menu","reset_globset"),64);
  settings_menu.add(mf.get_profile("menu","resetset"),12);

  {
   kSettings cfg;
   {
        char key[2]; 
        char str[1024];
        int size=sizeof(str)-1;

        kString recent[MAX_LRU];

        for(int i=0;i<MAX_LRU;i++)
        {
          key[0]='a'+i; key[1]=0;
          size=sizeof(str)-1;
          
          if(cfg.read_abs("Folders",key,str,size)==0)
          {
            FILE *f;
            f=fopen(str,"rb");
            if(f==NULL)
             cfg.delete_value_abs("Folders",key);
            else
            {
             recent[i]=(kString)"... "+(kString)str;
             fclose(f);
            }
          }
        }

        key[0]='z'; key[1]=0;
        size=sizeof(str)-1;

        int pos=-1;
        int first=0;

        if(cfg.read_abs("Folders",key,str,size)==0)
         pos=(str[0]-'a')&0xf;

        pos++;
        if(pos>=MAX_LRU)
         pos=0;

        for(int i=pos;i<MAX_LRU;i++)
         if(strlen((LPCTSTR)recent[i])!=0)
         {
          if(first==0)
          { first=1; settings_menu.separator();  }
          settings_menu.add((LPCTSTR)recent[i],MENU_MRU_BASE+i);
         }
        for(int i=0;i<pos;i++)
         if(strlen((LPCTSTR)recent[i])!=0)
         {
          if(first==0)
          { first=1; settings_menu.separator(); }
          settings_menu.add((LPCTSTR)recent[i],MENU_MRU_BASE+i);
         }
   }
  }

  settings_menu.separator();
//  AppendMenu(settings_menu,MFT_SEPARATOR,0,"");
  settings_menu.add(mf.get_profile("menu","setupbuf"),14);
  settings_menu.add(mf.get_profile("lang","settings.synth"),67);
  settings_menu.add(mf.get_profile("lang","settings.compat"),72);
  if((manager->get_current_device()>=0) && (manager->get_notifier()))
  {
   settings_menu.add(mf.get_profile("lang","settings.osd_notifications"),65);
  }
  settings_menu.add(mf.get_profile("lang","iostatus.name"),69);

  manager->addon_mgr->menu(KXADDON_MENU_SETTINGS,&settings_menu);

  settings_menu.separator();
  settings_menu.add(mf.get_profile("menu","asio"),68);
  settings_menu.add(mf.get_profile("menu","guidgen"),23);
  settings_menu.separator();

  {
   dword enabled=1;
   kSettings cfg;
   cfg.read_abs("Mixer","Splash",&enabled);
   settings_menu.add(mf.get_profile("lang","settings.splash"),59,((enabled==1)?MF_CHECKED:MF_UNCHECKED));

   dword tts=1;
   cfg.read_abs("Mixer","Tooltips",&tts);
   settings_menu.add(mf.get_profile("lang","settings.tooltips"),63,((tts)?MF_CHECKED:MF_UNCHECKED));

   tts=0;
   cfg.read_abs("Mixer","EnableBalloonTooltips",&tts);
   settings_menu.add(mf.get_profile("lang","settings.balloon"),66,((tts)?MF_CHECKED:MF_UNCHECKED));

   dword sc=1;
   cfg.read_abs("Mixer","ShowWindowContent",&sc);
   settings_menu.add(mf.get_profile("lang","settings.show_content"),77,((sc)?MF_CHECKED:MF_UNCHECKED));

   settings_menu.separator();
  }

  // remote-ir specific
  if((manager->get_current_device()>=0) && (manager->get_notifier()))
  {
   kSettings cfg(manager->get_ikx()->get_device_name());

   cfg.read("General","RemoteIR",&manager->get_notifier()->remote_ir_enabled);
   settings_menu.add(mf.get_profile("lang","settings.remote_ir"),61,((manager->get_notifier()->remote_ir_enabled)?MF_CHECKED:MF_UNCHECKED));

   cfg.read("General","OSD",&manager->get_notifier()->osd_enabled);
   settings_menu.add(mf.get_profile("lang","settings.osd"),60,((manager->get_notifier()->osd_enabled==1)?MF_CHECKED:MF_UNCHECKED));

   dword cfg_automute=0;
   cfg.read("hw_params","Headphones",&cfg_automute);
   settings_menu.add(mf.get_profile("lang","settings.headphones"),70,((cfg_automute)?MF_CHECKED:MF_UNCHECKED));
  }
  {
   kSettings cfg;
   dword mm_keyboard_enabled=0;
   cfg.read_abs("Mixer","MMKeyboard",&mm_keyboard_enabled);
   settings_menu.add(mf.get_profile("lang","settings.mm_keyboard"),62,((mm_keyboard_enabled)?MF_CHECKED:MF_UNCHECKED));
  }
  {
   tray_launcher_menu.add("kX Mixer (last page)",MENU_TRAYCFG_BASE+9);

   char tmp_str[64];
   for(int i=0;i<MAX_PAGES;i++)
   {
    if(!page_guids[i] || !page_guids[0][0])
     break;

    if(mf.get_profile(page_guids[i],"name",tmp_str,sizeof(tmp_str))==0)
    {
     tray_launcher_menu.add(tmp_str,MENU_TRAYCFG_BASE2+i);
    }
   }
   tray_launcher_menu.separator();
   
   tray_launcher_menu.add("Router",MENU_TRAYCFG_BASE+1);
   tray_launcher_menu.add("kX Automation",MENU_TRAYCFG_BASE+2);
   tray_launcher_menu.add("Remote control",MENU_TRAYCFG_BASE+3);
   tray_launcher_menu.add("kX Editor",MENU_TRAYCFG_BASE+4);
   tray_launcher_menu.add("24/96 Router",MENU_TRAYCFG_BASE+5);
   tray_launcher_menu.add("kX DSP",MENU_TRAYCFG_BASE+6);
   tray_launcher_menu.add("Speaker Test",MENU_TRAYCFG_BASE+7);
   tray_launcher_menu.add("I/O Status",MENU_TRAYCFG_BASE+8);

   settings_menu.add("Double click",&tray_launcher_menu);
  }

  menu.add(mf.get_profile("menu","settings"),&settings_menu);

  plugin_menu.add(mf.get_profile("menu","add_eff"),19);

  if(manager->get_pm()->enum_plugin(0))
  {
   plugin_menu.separator();

   fx_list_t fx_list[256];

   int num=0,cnt=0;
   while(1)
   {
    iKXPlugin *plg=manager->get_pm()->enum_plugin(num);
    if(plg==0)
     break;

    if(plg->get_user_interface())
    {
      sprintf(fx_list[cnt].name,"%s [%d]",plg->name,plg->pgm_id); // 'actual', not 'original'
      fx_list[cnt].id=plg->pgm_id;
      cnt++;
    }
    num++;
   }

   if(cnt)
     qsort(fx_list,cnt,sizeof(fx_list_t),compare_fx);

   for(int i=0;i<cnt;i++)
   {
    plugin_menu.add(fx_list[i].name,MENU_PLUGIN_BASE+fx_list[i].id);
   }
  }
  menu.add(mf.get_profile("menu","effects"),&plugin_menu);

  // Add-Ons

  addon_menu.add(mf.get_profile("menu","add_addon"),75);
  addon_menu.separator();
  manager->addon_mgr->menu(KXADDON_MENU_ADDONS,&addon_menu);
  menu.add(mf.get_profile("menu","addons"),&addon_menu);

  // Skins

  skin_menu.add(mf.get_profile("menu","add_skin"),18);

  {
   int ndx=0;
   kSettings cfg;

   skin_menu.separator();
   while(1)
   {
    char key_name[KX_MAX_STRING]; int keyname_size=sizeof(key_name);
    char value[MAX_PATH]; int value_size=sizeof(value);

    if(cfg.enum_abs(ndx,"Skins",key_name,keyname_size,value,value_size)==0)
    {
     if(strstr(key_name,".name")!=0)
     {
      skin_menu.add(value,ndx+MENU_SKINS_BASE);
     }
    } else
     break;
    ndx++;
   }
  }

  menu.add(mf.get_profile("menu","skin"),&skin_menu);

  // Internet menu items:
  internet_menu.add("www.kxproject.com",15);
  internet_menu.add(mf.get_profile("menu","check"),16);
  internet_menu.add(mf.get_profile("menu","online"),78);
  internet_menu.add(mf.get_profile("menu","download_s"),17);
  internet_menu.add(mf.get_profile("menu","download_e"),20);
  manager->addon_mgr->menu(KXADDON_MENU_INTERNET,&internet_menu);
  internet_menu.separator();
  internet_menu.add(mf.get_profile("menu","donate"),73);
  menu.add(mf.get_profile("menu","internet"),&internet_menu);
  

  lang_menu.add("English",21);
  lang_menu.separator();

  lang_menu.add("Brazilian Portuguese",37);
  lang_menu.add("Bulgarian",38);
  lang_menu.add("Chinese (Simplified)",43);
  lang_menu.add("Chinese (Traditional)",44);
  lang_menu.add("Croatian",39);
  lang_menu.add("Danish",48);
  lang_menu.add("Deutsch",27);
  lang_menu.add("Dutch",36);
  lang_menu.add("French",31);
  lang_menu.add("French 2",32);
  lang_menu.add("Greek",42);
  lang_menu.add("Hungarian",40);
  lang_menu.add("Italian",33);
  lang_menu.add("Japanese",46);
  lang_menu.add("Korean",41);
  lang_menu.add("Norwegian",35);
  lang_menu.add("Polish",28);
  lang_menu.add("Romanian",30);
  lang_menu.add("Russian",22);
  lang_menu.add("Spanish",29);
  lang_menu.add("Swedish",34);
  lang_menu.add("Thai",47);
  lang_menu.add("Turkish",49);
  lang_menu.add("Ukrainian",45);

  menu.add(mf.get_profile("menu","language"),&lang_menu);

  menu.separator();
  menu.add(mf.get_profile("menu","about"),4);
  menu.add(mf.get_profile("menu","cpanel"),71);
  menu.add(mf.get_profile("menu","help"),7);
  menu.add(mf.get_profile("menu","sptest"),5);

  manager->addon_mgr->menu(KXADDON_MENU_BOTTOM,&menu);

  menu.separator();
  menu.add(mf.get_profile("menu","quit"),1);

  POINT p;
  GetCursorPos(&p);
  
  SetForegroundWindow(systray);
  int ret=menu.track(TPM_RIGHTALIGN|TPM_BOTTOMALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON|TPM_RETURNCMD,
        p.x,p.y,(uintptr_t)systray);
  
  // menus are automatically destroyed?

  return ret;  
}


int process_menu(int ret)
{
CHANGE_DEVICE:
 if(ret>=MENU_TRAYCFG_BASE)
 {
  switch(ret-MENU_TRAYCFG_BASE)
  {
   case 1: strcpy(def_page_guid,"AE4B96B4-EEF7-45e3-B4F0-906B6AFCF874"); break;
   case 2: strcpy(def_page_guid,"A93AC348-DE37-4d81-90B2-A5A781E7B339"); break;
   case 3: strcpy(def_page_guid,"9F210647-2C16-4146-9287-889FD8413999"); break;
   case 4: strcpy(def_page_guid,"5AC145A5-5B70-4f2a-8B3B-60C319E7D017"); break;
   case 5: strcpy(def_page_guid,"A131B9F6-AC77-4b03-86CD-2F7078933B3D"); break;
   case 6: strcpy(def_page_guid,"30B35A5D-56F8-430e-9F15-98DBD6718BEF"); break;
   case 7: strcpy(def_page_guid,"4783FA97-5DFC-4152-9139-C8F7FFD4E700"); break;
   case 8: strcpy(def_page_guid,"FEF86ED4-9DB3-4390-8A29-4DF92B9A1581"); break;

   default:
   case 9: strcpy(def_page_guid,"41438001-B4A0-40ff-BF0D-F3A515D2A987"); break;
  }
  kSettings g_cfg;
  g_cfg.write_abs("Mixer","TrayClick",def_page_guid);

  return TRUE;
 }
 else
  if(ret>=MENU_TRAYCFG_BASE2)
  {
   kSettings g_cfg;

   strcpy(def_page_guid,get_mixer_page_guid(ret-MENU_TRAYCFG_BASE2));
   g_cfg.write_abs("Mixer","TrayClick",def_page_guid);

   return TRUE;
  }

 if(ret>=MENU_ADDON_BASE)
 {
  manager->addon_mgr->process_menu(ret);
  return TRUE;
 }

 if(ret>=MENU_DEVID_BASE) // select default:
 {
  manager->change_device(ret-MENU_DEVID_BASE,NULL);
  return TRUE;
 }

 if(ret>=MENU_PLUGIN_BASE) // activate plugin
 {
    manager->get_pm()->tweak_plugin(ret-MENU_PLUGIN_BASE);
    return TRUE;
 }

 if((ret>=MENU_SKINS_BASE) && (ret<=MENU_SKINS_BASE+90))
 {
    set_skin(ret-MENU_SKINS_BASE);
    return TRUE;
 }

 switch(ret)
 {
    case 1: // quit
/*          NOTIFYICONDATA nid;
            nid.cbSize=sizeof(nid);
            nid.hWnd=systray;
            nid.uID=1;
            Shell_NotifyIcon(NIM_DELETE,&nid);
*/
        PostQuitMessage(0);
        break;
    case 2: // open mixer
        manager->launch(LAUNCH_MIXER);
               break;
       case 3: // open DSP
        manager->launch(LAUNCH_DSP);
        break;
       case 4: // open info
        manager->launch(LAUNCH_INFO);
        break;
       case 5: // open speaker
        manager->launch(LAUNCH_SPTEST);
        break;
       case 8: // open router
        manager->launch(LAUNCH_ROUTER);
        break;
       case 9: // open fx router
        manager->launch(LAUNCH_MIDI);
        break;
       case 6:
         manager->launch(LAUNCH_EDITOR,-1,-1,NULL); // activates current or creates new window
        break;
       case 7: // help // NOTE: cross-referenced
        {
         manager->launch(LAUNCH_HELP);
        }
        break;
       case 10: // save
        manager->save_settings(SETTINGS_GUI,NULL,KX_SAVED_ALL|KX_SAVED_NO_VERSION|KX_SAVED_NO_CARDNAME);
        break;

       case 11: // restore
        manager->restore_settings(SETTINGS_GUI); // will init/close_plugins(), too (if appropriate)
        manager->save_settings(SETTINGS_AUTO);
        ret=manager->get_current_device()+MENU_DEVID_BASE;
        goto CHANGE_DEVICE;

       case 12: // reset
        if(MessageBox(NULL,(LPCTSTR)mf.get_profile(_T("setup"),_T("setup12")),_T("kX Mixer"),MB_YESNO)==IDYES)
        {
         manager->reset_settings(); // resets plugin defaults, too; init/close_plugins(), too
         manager->save_settings(SETTINGS_AUTO);
         ret=manager->get_current_device()+MENU_DEVID_BASE;
         goto CHANGE_DEVICE;
        }
        break;

       case 64: // global reset
        if(MessageBox(NULL,(LPCTSTR)mf.get_profile(_T("setup"),_T("setup11")),_T("kX Mixer"),MB_YESNO)==IDYES)
        {
         g_reset_settings(0); // force=0
         force_restart=1;

         PostMessage(systray,WM_QUIT,0,0);
        }
        break;

       case 14:
        manager->launch(LAUNCH_SETTINGS);
        break;
       case 15:
        {
         ShellExecute(NULL,"open","http://www.kxproject.com",NULL,NULL,SW_SHOWMAXIMIZED);
        }
        break;
       case 16:
       {
            kString tmp;
            tmp.Format("http://www.kxproject.com/update.php?version=%x",KX_VERSION_DWORD);

            tmp+="&driver=";
            char str[128];
            if(!manager->get_ikx()->get_string(KX_STR_DRIVER_VERSION,str))
            {
                tmp+=str;
            } else tmp+="unknown";
            tmp+="&card=";
            tmp+=manager->get_ikx()->get_device_name();
            tmp+="&db=";
            if(!manager->get_ikx()->get_string(KX_STR_DB_NAME,str))
            {
                tmp+=str;
            } else tmp+="unknown";
            tmp+="&ac97=";
            if(!manager->get_ikx()->get_string(KX_STR_AC97CODEC_NAME,str))
            {
                tmp+=str;
            } else tmp+="unknown";

            dword dev=0,subsys=0,rev=0;
            manager->get_ikx()->get_dword(KX_DWORD_DEVICE,&dev);
            manager->get_ikx()->get_dword(KX_DWORD_SUBSYS,&subsys);
            manager->get_ikx()->get_dword(KX_DWORD_CHIPREV,&rev);
            kString tmp2; tmp2.Format("&pci_device=0x%x&pci_subsys=0x%x&pci_chiprev=0x%x",dev,subsys,rev);
            tmp+=tmp2;

            OSVERSIONINFO ver;
            ver.dwOSVersionInfoSize=sizeof(ver);
            GetVersionEx(&ver);
            tmp2.Format("&os_ver_major=0x%x&&os_ver_minor=0x%x&os_ver_id=0x%x&os_ver_build=0x%x",ver.dwMajorVersion,ver.dwMinorVersion,
             ver.dwPlatformId,ver.dwBuildNumber);
            tmp+=tmp2;

            ShellExecute(NULL,"open",(LPCTSTR)tmp,NULL,NULL,SW_SHOWMAXIMIZED);
        }
        break;
       case 17:
        {
         ShellExecute(NULL,"open","http://www.kxproject.com/skins.php",NULL,NULL,SW_SHOWMAXIMIZED);
        }
        break;
       case 18:
        {
            restore_cwd("kxs");
                        CFileDialog *f_d = new CFileDialog(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_EXPLORER,
                          "kX Skins (*.kxs)|*.kxs||",NULL);
                        if(f_d)
                        {
                            char tmp_cwd[MAX_PATH];
                            GetCurrentDirectory(sizeof(tmp_cwd),tmp_cwd);
                            f_d->m_ofn.lpstrInitialDir=tmp_cwd;

                            if(f_d->DoModal()==IDCANCEL)
                            {
                             delete f_d;
                             break;
                            }

                            kString fname=f_d->GetPathName();
                            delete f_d;

                            save_cwd("kxs");

                            if(fname.GetLength()==0) // cancel or empty
                            {
                             break;
                            }
                                // register skin
                                if(add_skin(fname))
                                {
                                    // failed:
                                    MessageBox(NULL,mf.get_profile("errors","badskin"),"kX Mixer",MB_OK|MB_ICONEXCLAMATION);
                                }
                        }
                 }
        break;
       case 19:
        {
                manager->get_pm()->register_plugin(NULL);
        }
        break;
       case 20:
        {
         ShellExecute(NULL,"open","http://www.kxproject.com/dsp.php",NULL,NULL,SW_SHOWMAXIMIZED);
        }
        break;
#define language(logical,id1,id2)                       \
       case logical:                                \
        {                                                                       \
          mf.set_language(id1);                                                 \
          kSettings cfg;                                                        \
          LANGID current_language=id2;                                          \
          cfg.write_abs("Mixer","Language",(unsigned int)current_language);     \
          manager->change_device(-1,NULL);                                  \
          break;                                                                \
        }

    language(21,0x0,0x0); // english
    language(22,0x419,0x419); // russian
    language(27,0x407,0x407); // deutsch
    language(28,0x415,0x415); // polish
    language(29,0x80a,0x80a); // spanish
    language(30,0x418,0x418); // romanian
    language(31,0x40c,0x40c); // french I
    language(32,0x40c0,0x40c); // french ][
    language(33,0x410,0x410); // italian
    language(34,0x41d,0x41d); // swedish
    language(35,0x414,0x414); // norwegian
    language(36,0x413,0x413); // dutch
    language(37,0x416,0x416); // brasilian portuguese
    language(38,0x402,0x402); // bulgarian
    language(39,0x41a,0x41a); // croatian
    language(40,0x40e,0x40e); // hungarian
    language(41,0x412,0x412); // korean
    language(42,0x408,0x408); // greek
    language(43,0x804,0x804); // chinese simplified
    language(44,0x404,0x404); // chinese traditional
    language(45,0x422,0x422); // ukrainian
    language(46,0x411,0x411); // japanese
    language(47,0x41e,0x41e); // thai
    language(48,0x406,0x406); // danish
    language(49,0x41f,0x419); // turkish
#undef language

       case 23:
         char guid[KX_MAX_STRING];
         manager->get_ikx()->generate_guid(guid);
         if(OpenClipboard(systray))
         {
          HGLOBAL mem=GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE|GMEM_ZEROINIT,strlen(guid)+1);
          if(mem)
          {
           LPVOID m=GlobalLock(mem);
           strcpy((char *)m,guid);

           EmptyClipboard();
           
           if(SetClipboardData( CF_TEXT, mem)==0)
           {
            // kString tmp;
            // tmp.Format("E: %x",GetLastError());
            // MessageBox(NULL,(LPCTSTR)tmp,"e",MB_OK);
           }
           else
           {
            MessageBox(NULL,(char *)m,mf.get_profile("menu","guidclip"),MB_OK|MB_ICONINFORMATION);
           }
           GlobalUnlock(mem);
          }
          CloseClipboard();
         }
         break;
       case 24:
        manager->launch(LAUNCH_REMOTE);
        break;
       case 25:
        manager->launch(LAUNCH_MIDI);
        break;
       case 26:
        manager->launch(LAUNCH_MIXER,-1,5);
        break;
       case 59:
        {
            dword enabled=1;
            kSettings cfg;
            cfg.read_abs("Mixer","Splash",&enabled);

            if(enabled) enabled=0; else enabled=1;

            cfg.write_abs("Mixer","Splash",enabled);
        }
        break;
       case 60:
        {
            kSettings cfg(manager->get_ikx()->get_device_name());
            manager->get_notifier()->osd_enabled=1-(manager->get_notifier()->osd_enabled?1:0);

            cfg.write("General","OSD",manager->get_notifier()->osd_enabled);
        }
        break;
       case 61:
        {
            kSettings cfg(manager->get_ikx()->get_device_name());
            manager->get_notifier()->remote_ir_enabled=1-(manager->get_notifier()->remote_ir_enabled?1:0);

            cfg.write("General","RemoteIR",manager->get_notifier()->remote_ir_enabled);
        }
        break;
       case 62:
        {
            dword mm_keyboard_enabled=0;
            kSettings cfg;

            cfg.read_abs("Mixer","MMKeyboard",&mm_keyboard_enabled);
            mm_keyboard_enabled=1-(mm_keyboard_enabled?1:0);
            update_mm_keyboard(mm_keyboard_enabled);
            cfg.write_abs("Mixer","MMKeyboard",mm_keyboard_enabled);
        }
        break;
       case 63:
        {
            kSettings cfg;
            dword tt=1;
            cfg.read_abs("Mixer","Tooltips",&tt);
            tt=1-(tt?1:0);
            cfg.write_abs("Mixer","Tooltips",tt);

            if(tt)
             gui_set_tooltip_timings(-1,-1,-1); // enabled
            else
             gui_set_tooltip_timings(10000,500,10000);
        }
        break;
       case 66:
        {
            kSettings cfg;
            dword tt=0;
            cfg.read_abs("Mixer","EnableBalloonTooltips",&tt);
            tt=1-(tt?1:0);
            cfg.write_abs("Mixer","EnableBalloonTooltips",tt);

            gui_set_tooltip_type(tt);
        }
        break;
       case 65: // osd notifications
        {
            manager->launch(LAUNCH_NOTIFY);
        }
        break;
       case 67: // synth compat
        {
            manager->launch(LAUNCH_SYNTH_CFG);
        }
        break;
       case 68: // asio
        {
            manager->launch(LAUNCH_ASIO);
        }
        break;
       case 69: // io status
        {
            manager->launch(LAUNCH_IO_STATUS);
        }
        break;
       case 70: // automute headphones
        {
            kSettings cfg(manager->get_ikx()->get_device_name());
            dword cfg_automute=0;
            cfg.read("hw_params","Headphones",&cfg_automute);
            cfg_automute=1-(cfg_automute?1:0);
            cfg.write("hw_params","Headphones",cfg_automute);
        }
        break;
       case 71: // multimedia properties
        {
            _spawnlp(_P_NOWAIT,"rundll32.exe","rundll32.exe","shell32.dll,Control_RunDLL MMSYS.CPL,@0",NULL);
            // "D:\WINXP\system32\rundll32.exe" shell32.dll,Control_RunDLL "D:\WINXP\System32\MMSYS.CPL",@0
        }
        break;
       case 72: // driver compatibility
        {
            manager->launch(LAUNCH_COMPAT_CFG);
        }
        break;
       case 73:
        {
         ShellExecute(NULL,"open","http://www.kxproject.com/donate.php",NULL,NULL,SW_SHOWMAXIMIZED);
        }
        break;
       case 74: // kX Console
        {
         extern void consolidate();
         consolidate();
        }
        break;
       case 75: // register add-on
        {
                        // register add-on
                        manager->addon_mgr->register_addon(NULL); // error is displayed by the function
                }
                 break;
       case 76:
        {
            manager->launch(LAUNCH_P16V_ROUTER);
        }
        break;
      case 77:
        {
            kSettings cfg;
            dword sc=1;
            cfg.read_abs("Mixer","ShowWindowContent",&sc);
            sc=1-(sc?1:0);
            cfg.write_abs("Mixer","ShowWindowContent",sc);

            gui_set_show_content(sc);
        }
        break;  
       case 78:
       {
            kString tmp;
            tmp.Format("http://www.kxproject.com/online.php?version=%x",KX_VERSION_DWORD);

            tmp+="&driver=";
            char str[128];
            if(!manager->get_ikx()->get_string(KX_STR_DRIVER_VERSION,str))
            {
                tmp+=str;
            } else tmp+="unknown";
            tmp+="&card=";
            tmp+=manager->get_ikx()->get_device_name();
            tmp+="&db=";
            if(!manager->get_ikx()->get_string(KX_STR_DB_NAME,str))
            {
                tmp+=str;
            } else tmp+="unknown";
            tmp+="&ac97=";
            if(!manager->get_ikx()->get_string(KX_STR_AC97CODEC_NAME,str))
            {
                tmp+=str;
            } else tmp+="unknown";

            dword dev=0,subsys=0,rev=0;
            manager->get_ikx()->get_dword(KX_DWORD_DEVICE,&dev);
            manager->get_ikx()->get_dword(KX_DWORD_SUBSYS,&subsys);
            manager->get_ikx()->get_dword(KX_DWORD_CHIPREV,&rev);
            kString tmp2; tmp2.Format("&pci_device=0x%x&pci_subsys=0x%x&pci_chiprev=0x%x",dev,subsys,rev);
            tmp+=tmp2;

            OSVERSIONINFO ver;
            ver.dwOSVersionInfoSize=sizeof(ver);
            GetVersionEx(&ver);
            tmp2.Format("&os_ver_major=0x%x&&os_ver_minor=0x%x&os_ver_id=0x%x&os_ver_build=0x%x",ver.dwMajorVersion,ver.dwMinorVersion,
             ver.dwPlatformId,ver.dwBuildNumber);
            tmp+=tmp2;

            ShellExecute(NULL,"open",(LPCTSTR)tmp,NULL,NULL,SW_SHOWMAXIMIZED);
        }
        break;
       default:
        if(ret!=0)
        {
         if((ret>=MENU_MRU_BASE) && (ret<=MENU_MRU_BASE+MAX_LRU))
         {
            char key[2]; 
            char str[1024];
            int size=sizeof(str)-1;

            key[0]='a'+ret-MENU_MRU_BASE; key[1]=0;
            size=sizeof(str)-1;

            kSettings cfg;

            if(cfg.read_abs("Folders",key,str,size)==0)
            {
               kString f=str;
               manager->restore_settings(SETTINGS_FROMFILE,&f);
           manager->save_settings(SETTINGS_AUTO);
               ret=manager->get_current_device()+MENU_DEVID_BASE;
               goto CHANGE_DEVICE;
        }
         }
         else
           debug("? kxmixer: unknown command (%d)\n",ret);
        }
    break;
 }
 return 0;
}


LRESULT CALLBACK WndProc( HWND  hwnd,   // handle of window
    UINT  uMsg, // message identifier
    WPARAM  wParam, // first message parameter
    LPARAM  lParam  // second message parameter
   )
{
    int ret=(int)lParam; // cmd: 2-open mixer; 9-open automation

    if(manager && manager->addon_mgr)
    {
     kx_win_message win_msg;

     win_msg.wparam=wParam;
     win_msg.lparam=lParam;
     if(manager->addon_mgr->event(KXADDON_EVENT_WINMSG,(void *)(uintptr_t)uMsg,&win_msg)==0) // message processed
      return win_msg.ret;
    }

    if(uMsg==WM_POWERBROADCAST)
    {
      int i;

      switch(wParam)
      {
       // case PBT_APMPOWERSTATUSCHANGE:
       // case PBT_APMRESUMEAUTOMATIC:
       case PBT_APMQUERYSUSPEND:
        debug("kxmixer: querying suspend...\n");
        return TRUE; // grant

       case PBT_APMRESUMECRITICAL:
        debug("kxmixer: resuming after -critical- suspend...\n");
                // stop any Control activity...
                for(i=0;i<MAX_KX_DEVICES;i++)
                {
                 if(manager->get_notifier(i))
                 {
                   manager->get_notifier(i)->resume(1);
                 }
                }

                if(manager->addon_mgr)
                 manager->addon_mgr->event(KXADDON_EVENT_POWER,(void *)KXADDON_EVENT_POWER_RESUME,0);
                break;

       case PBT_APMRESUMESUSPEND:
        debug("kxmixer: resuming after suspend...\n");
                // stop any Control activity...
                for(i=0;i<MAX_KX_DEVICES;i++)
                {
                 if(manager->get_notifier(i))
                 {
                   manager->get_notifier(i)->resume();
                 }
                }

                if(manager->addon_mgr)
                 manager->addon_mgr->event(KXADDON_EVENT_POWER,(void *)KXADDON_EVENT_POWER_RESUME,0);
        break;
       case PBT_APMSUSPEND:
        debug("kxmixer: suspending...\n");
                // stop any Control activity...
                for(i=0;i<MAX_KX_DEVICES;i++)
                {
                 if(manager->get_notifier(i))
                 {
                  manager->get_notifier(i)->stop();
                 }
                }

                if(manager->addon_mgr)
                 manager->addon_mgr->event(KXADDON_EVENT_POWER,(void *)KXADDON_EVENT_POWER_SLEEP,0);
        break;
       default:
        debug("kxmixer: power event [%d]\n",wParam);
      }
    }

    switch(uMsg)
    {
    case WM_KXMIXER_TWEAK_VSTI:
        // lparam: unused(reserved)
        if(wParam>=MAX_KX_DEVICES) wParam=MAX_KX_DEVICES-1;
        if(manager->get_ikx((int)wParam))
         manager->launch(LAUNCH_MIDI,(int)wParam);
        return TRUE;

    case WM_KXMIXER_TWEAK_ROUTER:
        // lparam: unused(reserved)
        if(wParam>=MAX_KX_DEVICES) wParam=MAX_KX_DEVICES-1;
        if(manager->get_ikx((int)wParam))
        {
         manager->launch(LAUNCH_ROUTER,(int)wParam,1);
        }
        return TRUE;

    case WM_KXMIXER_SAVE_SETTINGS:
        if(wParam>=MAX_KX_DEVICES) wParam=MAX_KX_DEVICES-1;
        debug("kxmixer: [vsti:] save kx settings here... [device: %d fl: %x]\n",wParam,lParam);
                if(manager->get_ikx((int)wParam))
                {
                 char path[MAX_PATH];
                 sprintf(path, "%s\\kxtmp%d.kx", getenv("TEMP")?getenv("TEMP"):".", lParam&0xffff);
                 kString path_;
                 path_=path;
                 int ret=manager->save_settings(SETTINGS_FROMFILE,&path_,(dword)((lParam>>16)&0xffff));
                 if(ret==0) // ok
                   return TRUE;
                 else
                   debug("kxmixer: save_settings failed (%d)\n",ret);
                }
                return FALSE;
    case WM_KXMIXER_REST_SETTINGS:

        if(wParam>=MAX_KX_DEVICES) wParam=MAX_KX_DEVICES-1;

        debug("kxmixer: [vsti:] restore kx settings here... [device: %d fl: %x]\n",wParam,lParam);
                if(manager->get_ikx((int)wParam))
                {
                 char path[MAX_PATH];
                 kString path_;
                 sprintf(path, "%s\\kxtmp%d.kx", getenv("TEMP")?getenv("TEMP"):".", lParam&0xffff);
                 path_=path;
                 int ret=manager->restore_settings(SETTINGS_FROMFILE,&path_,(dword)((lParam>>16)&0xffff));
         if(ret==0) // ok
                   return TRUE;
                 else
                   debug("kxmixer: save_settings failed (%d)\n",ret);
                }
                return FALSE;

    case WM_KXMIXER_MESSAGE:
        // wparam: device; lparam: midi data
        if(wParam>=MAX_KX_DEVICES) wParam=MAX_KX_DEVICES-1;

        if(manager->get_parser((int)wParam))
         manager->get_parser((int)wParam)->parse_cc((dword)lParam);

        return TRUE;

    case WM_CLOSE:
        /*
            NOTIFYICONDATA nid;
            nid.cbSize=sizeof(nid);
            nid.hWnd=systray;
            nid.uID=1;
            Shell_NotifyIcon(NIM_DELETE,&nid);
            */
                PostQuitMessage(0);
            break;
        case MM_MIXM_CONTROL_CHANGE:
            if(mixer_window && (HMIXER)wParam==mixer)
             mixer_window->mixer_notify();

                // check if it is 'master mute'
                // lParam is control ID /  mixerGetLineControls  / MIXERCONTROL
                MIXERLINECONTROLS md;
                MIXERCONTROL mc;
                memset(&md,0,sizeof(md));
                md.cbStruct=sizeof(md);
                md.cControls=1;
                md.cbmxctrl=sizeof(MIXERCONTROL);
                md.pamxctrl=&mc;
                md.dwControlID=(dword)lParam;

                if(mixerGetLineControls((HMIXEROBJ)mixer,&md,MIXER_GETLINECONTROLSF_ONEBYID)==MMSYSERR_NOERROR)
                {
                 if(strstr(mc.szName,"Mute")!=0)
                 {
                  tray_icon(NIM_MODIFY);
                 }
                }

            break;
    case WM_KXMIXER_DISPLAY_MENU:
                  UINT uID; 
                  UINT uMouseMsg; 
                
                  uID = (UINT) wParam; 
                  uMouseMsg = (UINT) lParam; 

                  ret=0;

                  if(uMouseMsg==WM_LBUTTONDBLCLK)
                  {
                   manager->launch(def_page_guid);
                   return TRUE;
                  }

                  if (uMouseMsg == WM_RBUTTONUP)
                  {
                    ret=launch_menu();
                  }
                  // fall through

    case WM_KXMIXER_PROCESS_MENU:
           process_menu(ret);
               return TRUE;

        case WM_KXMIXER_TWEAK:
               {
                  iKXPlugin *plg=(iKXPlugin *)lParam;
                  iKXPluginManager *mgr=(iKXPluginManager *)wParam;

                  debug("kxmixer: tweak [%d]\n",plg->pgm_id);
                  mgr->tweak_plugin(plg->pgm_id);
               }
               return TRUE;

    case WM_QUERYENDSESSION:
        debug("kxmixer: querying shutdown -- quitting the kX mixer\n");

        quit_kxmixer(0);
        PostQuitMessage(0); // useless

        return TRUE; // allow

    case WM_ENDSESSION:
        debug("kxmixer: session ended\n");
        // PostQuitMessage(0); // cannot do this
        return 0; // processed

        case WM_TIMER:
            //debug("Timer\n");

            break;

        case WM_KXMIXER_PROCESS_CMDLINE:
            if(OpenClipboard(systray))
            {
                char tmp_str[512];
            SIZE_T size=GlobalSize(GetClipboardData(CF_TEXT));
                        HGLOBAL m=GetClipboardData(CF_TEXT);
                        strncpy(tmp_str,(LPTSTR)m,sizeof(tmp_str));
                        EmptyClipboard();
                CloseClipboard();
                if(strstr(tmp_str,"--shell"))
                {
                 char *p;
                 p=strstr(tmp_str,"--load-settings");
                 if(p)
                 {
                  p+=16;
                  kString f;
                  f=p;
                  manager->restore_settings(SETTINGS_FROMFILE,&f);
                          process_menu(manager->get_current_device()+MENU_DEVID_BASE);
                 }
                 else
                 {
                 p=strstr(tmp_str,"--register-dane");
                 if(p)
                 {
                  p+=16;
                  manager->get_pm()->register_plugin(p);
                 }
                 else
                 {
                 p=strstr(tmp_str,"--register-rifx");
                 if(p)
                 {
                  p+=16;
                          manager->get_pm()->register_plugin(p);
                 }
                 else
                 {
                 p=strstr(tmp_str,"--import-preset");
                 if(p)
                 {
                  p+=16;
                          debug("kxmixer: import preset: '%s'\n",p);
                          // find preset GUID
                          kSettings cfg(NULL,p);
                          char guid[KX_MAX_STRING];
                          if(cfg.read_abs("kXPreset","GUID",guid,sizeof(guid))==0)
                          {
                           int pgm_id=manager->get_pm()->have_plugin(guid);
                           if(pgm_id!=0) // returns pgm_id
                           {
                            iKXPlugin *plg=manager->get_pm()->find_plugin(pgm_id,guid);
                            if(plg)
                            {
                             if(plg->import_presets(p))
                              MessageBox(NULL,mf.get_profile("lang","preset.import.error1"),"kX Mixer",MB_OK|MB_ICONSTOP);
                             else
                              MessageBox(NULL,mf.get_profile("lang","preset.import.error4"),"kX Mixer",MB_OK|MB_ICONSTOP);
                            }
                            else
                             MessageBox(NULL,mf.get_profile("lang","preset.import.error2"),"kX Mixer",MB_OK|MB_ICONSTOP);
                           } else 
                            MessageBox(NULL,mf.get_profile("lang","preset.import.error2"),"kX Mixer",MB_OK|MB_ICONSTOP);
                          }
                           else 
                            MessageBox(NULL,mf.get_profile("lang","preset.import.error3"),"kX Mixer",MB_OK|MB_ICONSTOP);
                 }
                 else
                 {
                 p=strstr(tmp_str,"--register-quiet-kxl");
                 if(p)
                 {
                  p+=21;
                          manager->get_pm()->register_plugin(p,0);
                 }
                 else
                 {
                 p=strstr(tmp_str,"--register-kxl");
                 if(p)
                 {
                  p+=15;
                          manager->get_pm()->register_plugin(p);
                 }
                 else
                 {
                 p=strstr(tmp_str,"--register-kxa");
                 if(p)
                 {
                  p+=15;
                          manager->addon_mgr->register_addon(p);
                 }
                 else
                 {
                 p=strstr(tmp_str,"--register-skin");
                 if(p)
                 {
                  p+=16;
                  add_skin(p);
                 }
                 else
                 {
                 p=strstr(tmp_str,"--edit-dane");
                 if(p)
                 {
                  p+=12;
                  manager->launch(LAUNCH_EDITOR,-1,-1,(uintptr_t)p);
                 }
                 else
                 {
                           p=strstr(tmp_str,"--sfc");
                           if(p)
                           {
                           p+=6;
                           // launch_editor(-1,p);
                           debug("Load SoundFont Collection: '%s'\n",p);
                           // FIXME: sfc currently uses simple 'kx' files!
                           }
                           else
                           {
                           debug("Unknown command line: %s\n",tmp_str);
                           }
                  }
                 }
                 }
                 }
                 }
                 }
                 }
                 }
                 }
                }
            }
                return TRUE;
        case WM_HOTKEY:
            {
             signal_mm_keyboard((dword)wParam);
            }
            break;
    default:
        if(uMsg==taskbar_recreated)
        {
         debug("kxmixer: phoenix-explorer resurrected\n");
         tray_icon(NIM_DELETE);
         tray_icon(NIM_ADD);
        }
        break;
        }
        return DefWindowProc(hwnd,uMsg,wParam,lParam);
}

void CControlApp::OnHelp()
{ 
 process_menu(7); 
}

int get_mixer_page_by_guid(const char *guid)
{
 for(int i=0;i<MAX_PAGES;i++)
 {
  if(strcmp(guid,page_guids[i])==0)
   return i;
 }

 return -1;
}

char *get_mixer_page_guid(int n)
{
 return page_guids[n];
}

BOOL CControlApp::InitInstance()
{
    debug("kxmixer -- ("__DATE__","__TIME__")\n");

    manager=new iKXManager;

    LPTSTR cmdline=GetCommandLine();

    if(strstr(cmdline,"--global")!=NULL) // wait 1 sec since prev. kxmixer might be active..
     Sleep(1000);

    if(sanity_check())
    {
     free(manager);
     return FALSE;
    }
    
    if(shell_integration())
    {
     free(manager);
     return FALSE;
    }

    if(mf.init())
    {
        free(manager);
        MessageBox(NULL,"Fatal error: Error initializing Skin Engine\nThe kX Audio Driver wasn't installed correctly\nPlease re-run Setup","Severe error",MB_OK|MB_ICONSTOP);
        exit(10);
    }

    taskbar_recreated=RegisterWindowMessage(TEXT("TaskbarCreated"));

        // process initialization stuff / kx setup here
        check_first_run();

        init_cmax();
        
        Enable3dControls();

        if(create_systray_window())
        {
         free(manager);
         return FALSE;
        }

        // restore 'global' settings:
        kSettings g_cfg;

        init_skin();

        dword logo_enabled=1;
        g_cfg.read_abs("Mixer","Splash",&logo_enabled);

        kLogo logo;
    HBITMAP bm=mf.load_image("mixer/logo.jpg");
    if(bm==0)
    {
        debug("kX mixer: logo not found\n");
        free(manager);
        exit(11);
    }

    if(logo_enabled)
    {
            logo.create(WS_POPUP,0,"kX Mixer",
             100,100,
                 bm,kMETHOD_BLEND);

            logo.SetWindowPos(NULL, GetSystemMetrics(SM_CXFULLSCREEN)/2-150, 
              GetSystemMetrics(SM_CYFULLSCREEN)/2-75,
               -1, -1,
              SWP_NOSIZE|SWP_NOZORDER|SWP_NOREDRAW|SWP_NOACTIVATE);

            logo.set_alpha(90);
            logo.size_to_content();
            logo.ShowWindow(SW_SHOW);
            logo.RedrawWindow();

            Sleep(500);
    }

        dword tt=1;
        g_cfg.read_abs("Mixer","Tooltips",&tt);

        if(tt)
         gui_set_tooltip_timings(-1,-1,-1); // enabled
        else
         gui_set_tooltip_timings(10000,500,10000);

        tt=0;
        g_cfg.read_abs("Mixer","EnableBalloonTooltips",&tt);
        gui_set_tooltip_type(tt);

        dword sc=1;
        g_cfg.read_abs("Mixer","ShowWindowContent",&sc);
        gui_set_show_content(sc);

        if(strstr(cmdline,"--startup")!=NULL)
        {
            if(logo_enabled)
                 logo.SetWindowText((LPCTSTR)mf.get_profile("locals","restore"));
        }

        if(manager->init(cmdline))
         return FALSE;

        if(logo_enabled)
        {
         logo.ShowWindow(SW_HIDE);
         logo.DestroyWindow();
        }

        // acquire the list of pages
        memset(page_guids,0,sizeof(page_guids));
        for(int i=0;i<MAX_PAGES;i++)
        {
         char tmp_link[64];
         sprintf(tmp_link,"page%d",i);
         if(mf.get_profile("skin",tmp_link,page_guids[i],sizeof(page_guids[i])))
          break;
        }
        strcpy(def_page_guid,"41438001-B4A0-40ff-BF0D-F3A515D2A987"); // main page

        g_cfg.read_abs("Mixer","TrayClick",def_page_guid,sizeof(def_page_guid));

          // select current device (from the registry)
          // will (re)init default plugin parameters, too
          manager->change_device(-3,NULL);

          tray_icon(NIM_ADD);

          // create timer
//          timer=SetTimer(systray,0,TIMER_RESOLUTION,NULL); 

          // update ASIO configuration
          if(update_asio_drivers())
          {
               TCHAR mixer_folder[MAX_PATH]; mixer_folder[0]='"'; mixer_folder[1]=0;
               get_mixer_folder(mixer_folder);
               _tcscat(mixer_folder,_T("kxsetup.exe"));

               MessageBox(NULL,mf.get_profile("setup","setup21"),_T("kX Mixer"),MB_OK|MB_ICONINFORMATION);

               HINSTANCE hi=ShellExecute(NULL,"open",mixer_folder,"--asio",NULL,SW_SHOWDEFAULT);

               debug("kxmixer: launching setup to update ASIO drivers: %s %d %d %d\n",mixer_folder,(intptr_t)hi,GetLastError(),errno);
          }

      // quit kX Mixer if '--once'

      if(strstr(cmdline,"--once")==0)
      {
              update_mm_keyboard(-1);

                  // begin message loop
                  MSG msg;
                  while(GetMessage(&msg, NULL, 0U, 0U))
                  {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                  }
          }
//          if(timer)
//          {
//           KillTimer(systray,timer);
//          }

          quit_kxmixer(force_restart);

      if(force_restart)
      {
           debug("kxmixer: re-starting...\n");
           execlp("kxmixer.exe","kxmixer.exe","--global",0);

           // should not get here
           debug("kxmixer: re-launch failed [%x]\n",errno);
      }

      return FALSE; 
}


BOOL CControlApp::ExitInstance()
{
    close_cmax();
    return CWinApp::ExitInstance();
}
