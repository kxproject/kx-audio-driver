// kX Manager
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

#include "cmaxafx.h"
#include "main_dlg.h"
#include "edit_dlg.h"
#include "kxcode.h"
#include "kxdspDlg.h"
#include "info_dlg.h"
#include "speaker_dlg.h"
#include "router_dlg.h"
#include "p16v_r_dlg.h"
#include "remote_dlg.h"
#include "midirouter.h"
#include "iostatus_dlg.h"
#include "notify.h"
#include "addonmgr.h"

#include "synthcompat_dlg.cpp"
#include "compat_dlg.cpp"
#include "notify_dlg.cpp"

CMainDialog *mixer_window=0;
CKxdspDlg *dsp_window=0;
CInfoDialog *info_window=0;
CSpeakerDialog *sptest_window=0;
CIOStatusDialog *iostatus_window=0;
CEditDialog *editor_window=0;
CRouterDlg *router_window=0;
CRemoteDlg *remote_window=0;
CMidiRouterDlg *midi_window=0;
CP16VRouterDlg *p16v_router_window=0;
CSynthSettingsDlg *synth_compat_window=0;
CCompatSettingsDlg *drv_compat_window=0;
CNotifySettingsDlg *notify_window=0;

class CSettingsDlg;
CSettingsDlg *settings_window=0;

#include "settings_dlg.cpp"

extern HWND systray;
int exist(char *n);

iKXManager::iKXManager()
{
 memset(ikx_devices,0,sizeof(ikx_devices));
 memset(plugin_managers,0,sizeof(plugin_managers));
 memset(notifiers,0,sizeof(notifiers));
 memset(midi_parsers,0,sizeof(midi_parsers));

 addon_mgr=0;

 current_device=-10;
}

iKXManager::~iKXManager()
{
}



void launch_editor(int pgm,const char *da_source)
{
 if(!editor_window)
 {
  editor_window=new CEditDialog();
  editor_window->select_device();
  editor_window->create((kDialog *)kDialog::FromHandle(systray));
  editor_window->set_that((kDialog **)&editor_window);
  if(editor_window->select_pgm(pgm,da_source)) // error
  {
   editor_window->destroy();
   editor_window=NULL;
   return;
  }
  editor_window->show();
 }
  else 
 { 
  if((editor_window->pgm_id!=pgm) && (pgm!=-1))
  {
    editor_window->ikx_t=manager->get_ikx();
    if(editor_window->select_pgm(pgm,da_source)) // re-select by kX DSP
    {
     editor_window->destroy();
     editor_window=NULL;
     return;
    }
  }
  editor_window->show(); 
 }
}

void launch_info()
{
 if(!info_window)
 {
        kLogo logo;
        CWnd *parent=NULL;

        if(parent==0) parent=(kDialog *)kDialog::FromHandle(systray);

        HBITMAP bm=mf.load_image("mixer/logo.jpg");
        
	logo.create(WS_POPUP,0,"kX Mixer",
	 100,100,
         bm,kMETHOD_BLEND,parent);

	logo.SetWindowPos(NULL, GetSystemMetrics(SM_CXFULLSCREEN)/2-150, 
	  GetSystemMetrics(SM_CYFULLSCREEN)/2-75,
	   -1, -1,
	  SWP_NOSIZE);

	logo.set_alpha(90);
	logo.size_to_content();
	logo.ShowWindow(SW_SHOW);
	logo.redraw();

        Sleep(1000);

	logo.ShowWindow(SW_HIDE);
	logo.redraw();
	logo.Invalidate();
	logo.destroy();

        if(mixer_window) mixer_window->redraw();
        if(dsp_window) dsp_window->redraw();
        if(editor_window) editor_window->redraw();
        if(router_window) router_window->redraw();
        if(p16v_router_window) p16v_router_window->redraw();
        if(settings_window) settings_window->redraw();
        if(midi_window) midi_window->redraw();
        if(synth_compat_window) synth_compat_window->redraw();
        if(drv_compat_window) drv_compat_window->redraw();
        if(notify_window) notify_window->redraw();

  info_window=new CInfoDialog(manager->get_ikx());
  info_window->create((kDialog *)kDialog::FromHandle(systray));
  info_window->set_that((kDialog **)&info_window);

  info_window->show();
 } else info_window->show();
}


extern void launch_asio(void);
extern int get_mixer_folder(TCHAR *folder);

static void __cdecl thrdFunc(void *)
{ 
 TCHAR tmp_str[MAX_PATH];
 get_mixer_folder(tmp_str);
 _tcscat(tmp_str,_T("kxhelp.chm"));

 if(exist(tmp_str))
 {
  HtmlHelp(systray,tmp_str,HH_DISPLAY_TOPIC,0);
 }
  else
   if(MessageBox(NULL,mf.get_profile("menu","help_file"),"kX",MB_YESNO)==IDYES)
   {
    ShellExecute(NULL,"open","http://www.kxproject.com/help.php",NULL,NULL,SW_SHOWMAXIMIZED);
   }
}

void iKXManager::launch(int what,int device_,uintptr_t p1,uintptr_t p2)
{
 if(device_==-1)
  device_=current_device;
 if(device_<0)
  device_=0;

 switch(what)
 {
        case LAUNCH_INFO: launch_info(); break;
        case LAUNCH_DSP:  
             {
                if(!dsp_window)
                {
                 dsp_window=new CKxdspDlg();
                 dsp_window->create((kDialog *)kDialog::FromHandle(systray));
                 dsp_window->set_that((kDialog **)&dsp_window);
                 dsp_window->select_device();
                 dsp_window->show();
                } 
                 else 
                {
                 dsp_window->show();
                }
              }
              break;
        case LAUNCH_SPTEST: 
		{
                    if(!sptest_window)
                    {
                     sptest_window=new CSpeakerDialog(get_ikx());
                     sptest_window->create((kDialog *)kDialog::FromHandle(systray));
                     sptest_window->set_that((kDialog **)&sptest_window);
                     sptest_window->show();
                    } 
                     else 
                    {
                     sptest_window->show();
                    }
		}
        	break;
        case LAUNCH_EDITOR: launch_editor((int)p1,(const char *)p2); break;
        case LAUNCH_ROUTER: 
        	{
                      if(!router_window)
                      {
                       router_window=new CRouterDlg();
                       router_window->create((kDialog *)kDialog::FromHandle(systray));
                       router_window->set_that((kDialog **)&router_window);
                       router_window->select_device(device_);
                       router_window->show();
                      } 
                      else 
                      {
                       if(router_window->ikx_t!=get_ikx(device_))
                       {
                        router_window->select_device(device_);
                       }
                       router_window->show();
                      }

                      if((int)p1!=-1)
                       if(router_window)
                       {
                        debug("kxmixer: router: select 'asio' tree\n");
                       }
        	}
        	break;
        case LAUNCH_P16V_ROUTER: 
        	{
                      if(!p16v_router_window)
                      {
                       p16v_router_window=new CP16VRouterDlg();
                       p16v_router_window->create((kDialog *)kDialog::FromHandle(systray));
                       p16v_router_window->set_that((kDialog **)&p16v_router_window);
                       p16v_router_window->select_device(device_);
                       p16v_router_window->show();
                      } 
                      else 
                      {
                       if(p16v_router_window->ikx_t!=get_ikx(device_))
                       {
                        p16v_router_window->select_device(device_);
                       }
                       p16v_router_window->show();
                      }
        	}
        	break;
        case LAUNCH_SETTINGS: 
        	{
                   if(!settings_window)
                   {
                    settings_window=new CSettingsDlg;
                    settings_window->ikx_t=get_ikx();
                    settings_window->create((kDialog *)kDialog::FromHandle(systray));
                    settings_window->show();
                   } else settings_window->show();
        	}
        	break;
        case LAUNCH_REMOTE: 
        	{
                      if(!remote_window)
                      {
                       remote_window=new CRemoteDlg();
                       remote_window->create((kDialog *)kDialog::FromHandle(systray));
                       remote_window->set_that((kDialog **)&remote_window);
                       remote_window->select_device(device_);
                       remote_window->show();
                      } 
                       else 
                      {
                       if(remote_window->ikx_t!=get_ikx())
                       {
                        remote_window->select_device();
                       }
                       remote_window->show();
                      }
        	}
        	break;
        case LAUNCH_MIDI: 
        	{
                   if(!midi_window)
                   {
                    midi_window=new CMidiRouterDlg();
                    midi_window->create((kDialog *)kDialog::FromHandle(systray));
                    midi_window->select_device(device_);
                    midi_window->set_that((kDialog **)&midi_window);
                    midi_window->show();
                   }
                    else 
                   {
                    if(midi_window->parser_t!=manager->get_parser(device_))
                    {
                      midi_window->select_device(device_);
                    }
                    midi_window->show();
                   }
        	}
        	break;
        case LAUNCH_MIXER: 
        	{
                     if(!mixer_window)
                     {
                      mixer_window=new CMainDialog();
                      mixer_window->create((kDialog *)kDialog::FromHandle(systray));
                      mixer_window->set_that((kDialog **)&mixer_window);
                      mixer_window->select_device();
                      mixer_window->show();
                     } else 
                     {
                      mixer_window->show();
                     }
                     if((int)p1!=-1)
                      mixer_window->redraw_window((int)p1);
        	}
        	break;
        case LAUNCH_ASIO: launch_asio(); break;
        case LAUNCH_NOTIFY: 
            {
                           if(!notify_window)
                           {
                            notify_window=new CNotifySettingsDlg(&notify_window);

                            notify_window->create((kDialog *)NULL);
                            notify_window->show();

                           } else notify_window->show();
            }
             break;
        case LAUNCH_SYNTH_CFG: 
        	{
                           if(!synth_compat_window)
                           {
                            synth_compat_window=new CSynthSettingsDlg(get_ikx(),&synth_compat_window);

                            synth_compat_window->create((kDialog *)NULL);
                            synth_compat_window->show();

                           } else synth_compat_window->show();
	       	}
        	break;
        case LAUNCH_IO_STATUS:  
        	{
                      if(!iostatus_window)
                      {
                       iostatus_window=new CIOStatusDialog(get_ikx());
                       iostatus_window->create((kDialog *)kDialog::FromHandle(systray));
                       iostatus_window->set_that((kDialog **)&iostatus_window);
                       iostatus_window->show();
                      } else iostatus_window->show();
        	}
        	break;
        case LAUNCH_COMPAT_CFG: 
        	{
                           if(!drv_compat_window)
                           {
                            drv_compat_window=new CCompatSettingsDlg(get_ikx(),&drv_compat_window);

                            drv_compat_window->create((kDialog *)NULL);
                            drv_compat_window->show();

                           } else drv_compat_window->show();
        	}
        	break;
        case LAUNCH_HELP:
                 {
                	 if(_beginthread(thrdFunc,0,NULL)==-1) // error
                	 {
                                thrdFunc(NULL);
                         }
                 }
                 break;
 }
}

int check_guid(const char *guid,int fake=0)
{
    if((strcmp(guid,"631d8de5-11bc-4c3d-a0d2-f079977fd184")==NULL) || // epilog
       (strcmp(guid,"ceffc302-ea28-44df-873f-d3df1ba31736")==NULL ) // epilog
      )
      {
         if(!fake) manager->launch(LAUNCH_MIXER,-1,1);
         return 0;
      }
    else
     if((strcmp(guid,"313149FA-24FB-4f08-9C22-EB38B651BA58")==NULL)) // xrouting
     {
         if(!fake) manager->launch(LAUNCH_MIXER,-1,2);
         return 0;
     }
      else
       if( (strcmp(guid,"2b8b7fa8-98b9-4f6e-81a0-400d3ba39c6f")==NULL) || // fxbus/2
           (strcmp(guid,"131f1059-f384-4403-abd0-16ef6025bb9b")==NULL) ||
           (strcmp(guid,"d25a7874-7c00-47ca-8ad3-1b13106bde91")==NULL) // FXBusX
         )
       {
         if(!fake) manager->launch(LAUNCH_ROUTER);
         return 0;
       }
       else
        if(strcmp(guid,"85E09152-D489-4dee-8D60-6A509DC70901")==0) // p16v router
        {
         if(!fake) manager->launch(LAUNCH_P16V_ROUTER);
         return 0;
        }

    // process all built-in guids as well
    if(strcmp(guid,"A93AC348-DE37-4d81-90B2-A5A781E7B339")==NULL) // automation
    {
     if(!fake) manager->launch(LAUNCH_MIDI);
     return 0;
    }
    else
    if(strcmp(guid,"9F210647-2C16-4146-9287-889FD8413999")==NULL) // remote control
    {
     if(!fake) manager->launch(LAUNCH_REMOTE);
     return 0;
    }
    else
    if(strcmp(guid,"5AC145A5-5B70-4f2a-8B3B-60C319E7D017")==NULL) // kx editor
    {
     if(!fake) manager->launch(LAUNCH_EDITOR);
     return 0;
    }
    else
    if(strcmp(guid,"AE4B96B4-EEF7-45e3-B4F0-906B6AFCF874")==NULL) // kx router
    {
     if(!fake) manager->launch(LAUNCH_ROUTER);
     return 0;
    }
    else
    if(strcmp(guid,"A131B9F6-AC77-4b03-86CD-2F7078933B3D")==NULL) // p16v router
    {
     if(!fake) manager->launch(LAUNCH_P16V_ROUTER);
     return 0;
    }
    else
    if(strcmp(guid,"30B35A5D-56F8-430e-9F15-98DBD6718BEF")==NULL) // kx dsp
    {
     if(!fake) manager->launch(LAUNCH_DSP);
     return 0;
    }
    else
    if(strcmp(guid,"57A7DAE5-98FA-4927-82AE-253EAE66651D")==NULL) // synth compat
    {
     if(!fake) manager->launch(LAUNCH_SYNTH_CFG);
     return 0;
    }
    else
    if(strcmp(guid,"109CBB5D-455F-494d-AF09-D9262383260A")==NULL) // driver compat
    {
     if(!fake) manager->launch(LAUNCH_COMPAT_CFG);
     return 0;
    }
    else
    if(strcmp(guid,"850B96C6-6993-4461-8473-07B664F4F608")==NULL) // setup buffers
    {
     if(!fake) manager->launch(LAUNCH_SETTINGS);
     return 0;
    }
    else
    if(strcmp(guid,"4783FA97-5DFC-4152-9139-C8F7FFD4E700")==NULL) // sptest
    {
     if(!fake) manager->launch(LAUNCH_SPTEST);
     return 0;
    }
    else
    if(strcmp(guid,"06442AFE-AEC8-4c0f-9FE8-9D9E41CB6205")==NULL) // about
    {
     if(!fake) manager->launch(LAUNCH_INFO);
     return 0;
    }
    else
    if(strcmp(guid,"FEF86ED4-9DB3-4390-8A29-4DF92B9A1581")==NULL) // io status
    {
     if(!fake) manager->launch(LAUNCH_IO_STATUS);
     return 0;
    }
    else
    if(strcmp(guid,"0C370F50-88FF-4262-AB79-A1E5BA7513F3")==NULL) // ASIO
    {
     if(!fake) manager->launch(LAUNCH_ASIO);
     return 0;
    }
    else
    if(strcmp(guid,"4AB01E2C-112B-4537-9C33-ADCF40659FD6")==NULL) // notify
    {
     if(!fake) manager->launch(LAUNCH_NOTIFY);
     return 0;
    }
    else
    if(strcmp(guid,"97156A09-66F2-459c-9E60-C79AC2888146")==NULL) // help
    {
     if(!fake) manager->launch(LAUNCH_HELP);
     return 0;
    }
    else
    if(strcmp(guid,"41438001-B4A0-40ff-BF0D-F3A515D2A987")==NULL) // kx mixer (prev. page)
    {
     if(!fake) manager->launch(LAUNCH_MIXER);
     return 0;
    }
    else
    {
      // check page GUIDs
      int page=get_mixer_page_by_guid(guid);
      if(page!=-1)
      {
       if(!fake) manager->launch(LAUNCH_MIXER,-1,page);
       return 0;
      }
    }

    return -1;
}

int iKXManager::launch(char *guid)
{
 // should be first to override plugins
 if(check_guid(guid)==0) // o.k.?
   return 0;

 iKXPluginManager *pm=get_pm();
 if(pm)
  if(pm->tweak_plugin(guid)==0)
   return 0;

 if(addon_mgr->launch(guid)==0)
  return 0;

 return -1;
}

int iKXManager::is_supported(char *guid)
{
 // should be first to override plugins
 if(check_guid(guid,1)==0) // o.k.?
   return 1;

 iKXPluginManager *pm=get_pm();
 if(pm)
  if(pm->have_plugin(guid)>0)
   return 1;

 if(addon_mgr->is_supported(guid))
   return 1;
 
 return 0;
}


iKX *iKXManager::get_ikx(int device_)
{
 if(device_==-1) device_=current_device;
 if(device_>=0)
  return ikx_devices[device_];
 else
  return NULL;
}

iKXPluginManager *iKXManager::get_pm(int device_)
{
 if(device_==-1) device_=current_device;
 if(device_>=0)
  return plugin_managers[device_];
 else
  return NULL;
}

iKXNotifier *iKXManager::get_notifier(int device_)
{
 if(device_==-1) device_=current_device;
 if(device_>=0)
  return notifiers[device_];
 else
  return NULL;
}

iKXMidiParser *iKXManager::get_parser(int device_)
{
 if(device_==-1) device_=current_device;
 if(device_>=0)
  return midi_parsers[device_];
 else
  return NULL;
}

int iKXManager::init(TCHAR *cmdline)
{
        // initialize kX Add-on manager
        addon_mgr=new iKXAddOnManager;
        if(addon_mgr)
        {
         addon_mgr->init(KXADDON_SYSTEM_WIDE); // global init

         kSettings cfg;
         addon_mgr->load_all_addon_settings(KXADDON_SYSTEM_WIDE,cfg);
        }

        // enumerate and initialize the devices...
        for(int dev_id=0;dev_id<MAX_KX_DEVICES;dev_id++)
        {
        	ikx_devices[dev_id]=new iKX;

        	if(ikx_devices[dev_id])
        	{
        		if(ikx_devices[dev_id]->init(dev_id)!=0) // error
        		{
        			if(dev_id==0)
        			{
        				debug("No iKX devices found\n");

                                        kString tmp=mf.get_profile("errors","no_devs");
                                        kString s;
                                        s.Format("%s",ikx_devices[dev_id]->get_error_string());
                                        tmp+="\nkX Error: ";
                                        tmp+=s;
                                        MessageBox(NULL,(LPCTSTR)tmp,"kX Mixer",MB_OK);

                                        delete ikx_devices[dev_id];
                                        return -1;
        			}
        			ikx_devices[dev_id]->close();
        			delete ikx_devices[dev_id];
        			ikx_devices[dev_id]=NULL;
        			break;
        		}
        		else // init ok -- process startup (if necessary)
        		{
        		       kSettings cfg(ikx_devices[dev_id]->get_device_name());

        		       current_device=dev_id;

        		       plugin_managers[dev_id]=new iKXPluginManager;
        		       plugin_managers[dev_id]->ikx_t=ikx_devices[dev_id];
        		       plugin_managers[dev_id]->parser_t=NULL;

                               // init 'kX Control' + 'CMidiParser'
                               notifiers[dev_id]=new iKXNotifier;
                               if(notifiers[dev_id]->init(dev_id))
                               {
                                    debug("! error initializing remote control for device %d\n",dev_id);
                                     notifiers[dev_id]->close();

                                    delete notifiers[dev_id];
                                    notifiers[dev_id]=NULL;
                               }
                               else
                               {
                               
                                     notifiers[dev_id]->remote_ir_enabled=1;
                                     cfg.read("General","RemoteIR",&notifiers[dev_id]->remote_ir_enabled);

                                     notifiers[dev_id]->osd_enabled=1;
                                     cfg.read("General","OSD",&notifiers[dev_id]->osd_enabled);

                                     midi_parsers[dev_id]=new iKXMidiParser(notifiers[dev_id]);
                                     // we should restore settings anyway
                                     // cannot yet restore settings, since plugins aren't uploaded
                                     // midi_parsers[dev_id]->restore_settings(cfg);

                                     plugin_managers[dev_id]->parser_t=midi_parsers[dev_id];
                                     notifiers[dev_id]->parser_t=midi_parsers[dev_id];
                               }


                               // process initialization (if necessary)
                               TCHAR version[KX_MAX_STRING];

                               int force_setup=0;
                               if(cfg.read(_T("General"),_T("Setup"),version,sizeof(version))==0)
                               {
                                  TCHAR cur_version[KX_MAX_STRING];
                                  _stprintf(cur_version,"%s",KX_DRIVER_VERSION_STR);

                                  if(_tcscmp(version,cur_version)!=0)
                                  {
                                   // ask to 'reinit'
                                   TCHAR tmp_str[512];
                                   sprintf(tmp_str,(LPCTSTR)mf.get_profile(_T("setup"),_T("setup14")),version,ikx_devices[dev_id]->get_device_name());
                                   if(MessageBox(NULL,tmp_str,_T("kX Mixer"),MB_YESNO)==IDYES)
                                   {
                                    debug("kxmixer: settings version '%s' found for device [%s] - force 'reset settings'\n",version,ikx_devices[dev_id]->get_device_name());
                                    force_setup=1;
                                   }
                                   else
                                   {
                                    // mark settings as 'correct' -- up to user :)
                                    cfg.write(_T("General"),_T("Setup"),KX_DRIVER_VERSION_STR);
                                    cfg.write(_T("General"),_T("version"),KX_DRIVER_VERSION_STR);
                                   }
                                  }
                               }
                               else
                               {
                                debug("kxmixer: general per-device [%d/%s] first-time initialization\n",dev_id,ikx_devices[dev_id]->get_device_name());
                                force_setup=1;
                               }

                               if(addon_mgr)
                               {
                                 addon_mgr->init(KXADDON_PER_DEVICE); // per-device init

                                 // this is done below by restore_settings()
                                 // addon_mgr->load_all_addon_settings(KXADDON_PER_DEVICE,cfg);
                               }

                               if(force_setup)
                               {
                               		debug("kxmixer: resetting settings for '%d' device\n",dev_id);
                                    reset_settings();
                                    save_settings(SETTINGS_AUTO);
                                    cfg.write(_T("General"),_T("Setup"),KX_DRIVER_VERSION_STR);
                               }

                               // upload settings on startup only
                               if((strstr(cmdline,"--startup")!=NULL) && (!force_setup))
                               {
                                       restore_settings(SETTINGS_AUTO|SETTINGS_MAY_NOT_EXIST);
                               } // startup?
                               else
                               {
                                // only initialize plugin settings
                                plugin_managers[dev_id]->init_plugins();

                                if(midi_parsers[dev_id])
                                 midi_parsers[dev_id]->restore_settings(cfg);

                                if(addon_mgr)
                                 addon_mgr->load_all_addon_settings(KXADDON_PER_DEVICE,cfg);
                               }

                               dword cfg_automute=0;
                               cfg.read("hw_params","Headphones",&cfg_automute);

                               if(cfg_automute)
                               {
                                dword gpio;
                                if(!ikx_devices[dev_id]->gpio_read(&gpio))
                                 notifiers[dev_id]->process_gpio((byte)gpio,0);
                               }
        		}
        	}
        }

        return 0;
}

void iKXManager::close(int force_restart)
{
	  if(addon_mgr)
	  {
	   kSettings cfg;
	   if(!force_restart)
	    addon_mgr->save_all_addon_settings(KXADDON_SYSTEM_WIDE,cfg);
           // save global settings, do not delete addon_mgr
	  }

	  close_all_windows();

	  for(int dev_id=0;dev_id<MAX_KX_DEVICES;dev_id++)
	  {
	   current_device=dev_id;

	   if(get_ikx()==0)
	    continue;

	   if(!force_restart)
	    save_settings(SETTINGS_AUTO);

           // should be done above
	   // if(!force_restart)
	   // addon_mgr->save_all_addon_settings(KXADDON_PER_DEVICE,cfg);

	   if(midi_parsers[dev_id])
	   {
	    if(notifiers[dev_id])
	     notifiers[dev_id]->parser_t=NULL;
	    if(plugin_managers[dev_id])
	     plugin_managers[dev_id]->parser_t=NULL;

	    delete midi_parsers[dev_id];
	    midi_parsers[dev_id]=0;
	   }
	   
           if(notifiers[dev_id])
           {
            delete notifiers[dev_id];
            notifiers[dev_id]=NULL;
           }

           if(get_pm())
           {
             if(!force_restart)
              get_pm()->close_plugins();

             delete plugin_managers[dev_id];
             plugin_managers[dev_id]=0;
           }

           destroy_decoder(dev_id);

           delete ikx_devices[dev_id];
           ikx_devices[dev_id]=NULL;
	  }

	  if(addon_mgr)
	  {
	   addon_mgr->close();
	   delete addon_mgr;
	   addon_mgr=NULL;
	  }
}

int iKXManager::change_device(int new_device,kWindow *parent)
{
 if(new_device==-2) // select via gui
 {
   kMenu menu;
   menu.create();

   create_devices_menu(&menu);

   POINT p;
   GetCursorPos(&p);

   int ret=menu.track(TPM_RIGHTALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON|TPM_RETURNCMD,
   		p.x,p.y,parent?parent:NULL);

   if(ret>=1)
   {
   	new_device=ret-MENU_DEVID_BASE;
   }
   else
    return -1; // cancel
 } // new_device==-1?

 kSettings cfg;
 if(new_device==-3) // get from the registry
 {
        // retrieve recent device from the registry
        new_device=0;
        dword new_device_=0;

        cfg.read_abs("Mixer","DefDevice",&new_device_);
        new_device = new_device_;

        if(new_device<0) new_device=0;
        if(new_device>=MAX_KX_DEVICES)
         new_device=0;
 }

  save_settings(SETTINGS_AUTO);

  if(new_device<0) new_device=0;

  extern HMIXER mixer;
  if(mixer)
  {
   mixerClose(mixer);
   mixer=NULL;
  }

  if(ikx_devices[new_device]==0)
   new_device=0;

// if(strstr(GetCommandLine(),"--no10k2")!=NULL)
//   ikx_devices[dev_id]->set_dsp(0);

        UINT mixer_num=ikx_devices[new_device]->get_winmm_device(KX_WINMM_MIXER,0);

        if(mixer_num!=0xffffffff)
        {
          dword disabled=1;
          if((cfg.read_abs("Mixer","NoMixer",&disabled)==0)&&(disabled==1))
          {
           debug("kxmixer: winmm mixer initialization aborted due to 'NoMixer' flag\n");
           mixer=0;
          }
          else
           mixerOpen(&mixer,mixer_num,(DWORD_PTR)systray,NULL,CALLBACK_WINDOW);
        }
        else
        {
          debug("!!! kxmixer: error opening mixer [%d]\n",mixer_num);
        }

 current_device=new_device;

 get_pm()->init_plugins();

 if(mixer_window)
 {
   mixer_window->select_device();
   mixer_window->redraw_window(mixer_window->cur_page);
 }
 if(dsp_window) 
 {
   dsp_window->select_device();
 }
 if(router_window) router_window->select_device();
 if(p16v_router_window) p16v_router_window->select_device();
 if(settings_window) settings_window->destroy();
 if(midi_window) midi_window->select_device();
 if(remote_window) remote_window->select_device();
 // editor_window -- with separate ikx_t
 if(info_window) info_window->destroy();
 if(sptest_window)  sptest_window->select_device();
 if(iostatus_window) iostatus_window->select_device();

 if(synth_compat_window) synth_compat_window->destroy();
 if(drv_compat_window) drv_compat_window->destroy();
 // if(notify_window) notify_window->destroy();

 // save recent device in the registry -- should be done regardless current device
 cfg.write_abs("Mixer","DefDevice",new_device);

 if(addon_mgr)
  addon_mgr->event(KXADDON_EVENT_GENERIC,(void *)IKX_DEVICE_CHANGED,0);

 return 0;
}

void iKXManager::close_all_windows()
{
        // close all windows

        if(mixer_window) mixer_window->destroy();
        if(dsp_window) dsp_window->destroy();
        if(info_window) info_window->destroy();
        if(sptest_window) sptest_window->destroy();
        if(iostatus_window) iostatus_window->destroy();
        if(editor_window) editor_window->destroy();
        if(router_window) router_window->destroy();
        if(p16v_router_window) p16v_router_window->destroy();
        if(settings_window) settings_window->destroy();
        if(remote_window) remote_window->destroy();
        if(midi_window) midi_window->destroy();

        if(synth_compat_window) synth_compat_window->destroy();
        if(drv_compat_window) drv_compat_window->destroy();
        if(notify_window) notify_window->destroy();

        // FIXME: close all plugin windows
        // (note: may affect plugin settings save/restore -- NB)
}

int iKXManager::get_current_device()
{
	return current_device;
}

int iKXManager::get_version()
{
	return KXMANAGER_VERSION;
}

int iKXManager::create_devices_menu(kMenu *menu,int cur_dev)
{
 if(cur_dev==-1)
  cur_dev=get_current_device();

 for(int dev_id=0;dev_id<MAX_KX_DEVICES;dev_id++)
 {
  if(get_ikx(dev_id))
  {
     kString tmp;
     tmp.Format("%d: %s",dev_id+1,get_ikx(dev_id)->get_device_name());
     menu->add((LPCTSTR)tmp,MENU_DEVID_BASE+dev_id,((dev_id==cur_dev)?MF_CHECKED:0));
  } else 
   break;
 }

 return 0;
}

kFile *iKXManager::get_default_skin(void)
{
 return &mf;
}
