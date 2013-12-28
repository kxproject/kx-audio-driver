// kX Audio Driver API / kX Audio Driver Plugin Interface / kX Add-on Interface
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
#include "addonmgr.h"

// ---------------------------------------------------

iKXAddOnManager::iKXAddOnManager()
{
 list=0;
}


iKXAddOnManager::~iKXAddOnManager()
{
 close();
}

void iKXAddOnManager::init(int stage)
{
 int need_destructor=0;

 iKX *ikx_t=manager->get_ikx();
 if(ikx_t==0)
 {
  need_destructor=1;
  ikx_t=new iKX();
 }

 // upload all the add-ons
 // do NOT restore settings [done by iKXManager later]
 // stage: 0: generic; 1: per-dev
 // generate menu_base for each add-on

 kSettings cfg;

 int ndx=0;

 // keep mnu_idx global
 static int mnu_idx=MENU_ADDON_BASE;

 while(1)
 {
  char key_name[KX_MAX_STRING]; int keyname_size=sizeof(key_name);
  char value[MAX_PATH]; int value_size=sizeof(value);

  if(cfg.enum_abs(ndx,"Addons",key_name,keyname_size,value,value_size)==0)
  {
   if(strstr(key_name,".name")==0)
   {
    // key_name is 'guid', value is path
    iKXAddOn *addon=(iKXAddOn *)ikx_t->instantiate_object(value,key_name);

    if(addon)
    {
     if(((addon->addon_type&KXADDON_PER_DEVICE) && (stage==KXADDON_PER_DEVICE)) ||
        ((addon->addon_type&KXADDON_SYSTEM_WIDE) && (stage==KXADDON_SYSTEM_WIDE)))
     {
      addon->kx_manager=manager;
      addon->menu_base=mnu_idx;

      int ret=0;

      try
      {
      ret=addon->init((stage==KXADDON_SYSTEM_WIDE)?-1:manager->get_current_device());
      }
       catch(...)
      {
       debug("kxmixer: addon mgr: an exception while executing init()\n");
       ret=-1;
      }

      if(ret==0)
      {
         kx_addon_list *item=(kx_addon_list *)malloc(sizeof(kx_addon_list));

         item->addon=addon;
         strcpy(item->guid,key_name);
         item->device_n=stage;
         item->next=list;

         list=item;

         mnu_idx+=MAX_ADDON_MENU_ITEMS;
      }
      else
       delete addon;
     }
     else
      delete addon; // fixme
    }
   }
  }
   else
  {
   break;
  }
  ndx++;
 }

 if(need_destructor)
 {
  delete ikx_t;
  ikx_t=0;
 }
}

void iKXAddOnManager::close()
{
 // unload all the add-ons
 // (settings are already saved by iKXManager)
 while(list)
 {
  try
  {
   list->addon->close();
  }
  catch(...)
  {
   debug("kxmixer: addonmgr: close() failed an exception\n");
  }
  delete list->addon;
  list->addon=NULL;

  kx_addon_list *p=list->next;
  free(list);
  list=p;
 }
}

int iKXAddOnManager::register_addon(char *file_name,int gui)
{
  kString fname;

  if(file_name==NULL)
  {
  	restore_cwd("kxa");
        CFileDialog *f_d = new CFileDialog(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_EXPLORER,
          "kX Add-Ons (*.kxa)|*.kxa||",NULL);
        if(f_d)
        {
               	char tmp_cwd[MAX_PATH];
               	GetCurrentDirectory(sizeof(tmp_cwd),tmp_cwd);
               	f_d->m_ofn.lpstrInitialDir=tmp_cwd;

        	if(f_d->DoModal()==IDCANCEL)
        	{
        	 delete f_d;
        	 return 0;
        	}
        	save_cwd("kxa");

        	fname=f_d->GetPathName();
        	delete f_d;

        	if(fname.GetLength()==0) // cancel or empty
        	{
        	 return 0;
        	}
        } else return 0;
  } else fname=file_name;

  int need_destructor=0;

  iKX *ikx_t=manager->get_ikx();
  if(ikx_t==0)
  {
   need_destructor=1;
   ikx_t=new iKX();
  }

  debug("kxmixer: add-on: registering '%s'\n",(LPCTSTR)fname);
  	
  int count=ikx_t->get_object_count((char *)(LPCTSTR)fname);
  if(count>0 && (ikx_t->get_object_type((char *)(LPCTSTR)fname)==KX_OBJECT_ADDON))
  {
   for(int i=0;i<count;i++)
   {
    char p_guid[KX_MAX_STRING];
    if(ikx_t->get_object_guid((char *)(LPCTSTR)fname,i,p_guid)==0)
    {
    	kSettings cfg;
        cfg.write_abs("Addons",p_guid,(TCHAR *)(LPCTSTR)fname);

        char tmp_str[512];
        strcpy(tmp_str,p_guid);
        strcat(tmp_str,".name");

        if(ikx_t->get_object_name((char *)(LPCTSTR)fname,i,p_guid)==0) // ok
        {
         cfg.write_abs("Addons",tmp_str,p_guid); // p_guid is already a name, not GUID
        }

        debug("kxmixer: registered [%d] %s:%s\n",i,tmp_str,p_guid);
    }
   }
   if(gui)
   {
    kString t;
    t.Format(mf.get_profile("lang","addon.register"),count);
    MessageBox(NULL,(LPCTSTR)t,"kX Mixer",MB_OK|MB_ICONINFORMATION);
   }
  } else // count<=0
  {
   if(gui)
   {
    MessageBox(NULL,mf.get_profile("errors","badlib"),"kX Mixer",MB_OK|MB_ICONEXCLAMATION);
   }
  }

  if(need_destructor)
  {
   delete ikx_t;
   ikx_t=0;
  }

 return count;
}


int iKXAddOnManager::menu(int menu_stage,kMenu *mnu) // returns 0 if something was added/modified
{
 int ret=-1;

 // for each
 kx_addon_list *lst=list;
 while(lst)
 {
  if(lst->addon)
  {
   if(lst->addon->needed_menu_stage&menu_stage)
    if(lst->addon->menu(menu_stage,mnu)==0)
     ret=0;
  }
  lst=lst->next;
 }
 return ret;
}

int iKXAddOnManager::event(int event,void *p1,void *p2)
{
 int ret=-1;

 // for each
 kx_addon_list *lst=list;
 while(lst)
 {
  if(lst->addon)
  {
   if(lst->addon->event_mask&event)
    ret=lst->addon->event(event,p1,p2);
  }
  lst=lst->next;
 }

 return ret;
}

int iKXAddOnManager::notify(iKXAddOn *,int event,void *p1,void *p2)
{
 // no kX Manager events currently defined
 return 0;
}

int iKXAddOnManager::save_all_addon_settings(int where,kSettings &cfg)
{
 // for each
 kx_addon_list *lst=list;
 while(lst)
 {
  if(lst->addon)
  {
    lst->addon->save_settings(where,cfg);
  }
  lst=lst->next;
 }

 return 0;
}

int iKXAddOnManager::load_all_addon_settings(int where,kSettings &cfg)
{
 // for each
 kx_addon_list *lst=list;
 while(lst)
 {
  if(lst->addon)
  {
    lst->addon->load_settings(where,cfg);
  }
  lst=lst->next;
 }

 return 0;
}

int iKXAddOnManager::reset_all_addon_settings(int where)
{
 // for each
 kx_addon_list *lst=list;
 while(lst)
 {
  if(lst->addon)
  {
    lst->addon->reset_settings(where);
  }
  lst=lst->next;
 }

 return 0;
}

int iKXAddOnManager::process_menu(int cmd)
{
 // for each
 kx_addon_list *lst=list;
 while(lst)
 {
  if(lst->addon && (lst->addon->event_mask&KXADDON_EVENT_MENU))
  {
    if(cmd>=lst->addon->menu_base && cmd<lst->addon->menu_base+MAX_ADDON_MENU_ITEMS)
    {
     lst->addon->event(KXADDON_EVENT_MENU,(void *)(intptr_t)cmd,0);
     break;
    }
  }
  lst=lst->next;
 }

 return 0;
}

int iKXAddOnManager::launch(char *guid)
{
 // for each
 kx_addon_list *lst=list;
 while(lst)
 {
  if(lst->addon && (lst->addon->event_mask&KXADDON_EVENT_ACTIVATE))
  {
     if(((lst->addon->addon_type&KXADDON_PER_DEVICE) && (manager->get_current_device()==lst->device_n)) ||
        (lst->addon->addon_type&KXADDON_SYSTEM_WIDE))
     return lst->addon->activate();
  }
  lst=lst->next;
 }

 return -1;
}

int iKXAddOnManager::is_supported(char *guid)
{
 // for each
 kx_addon_list *lst=list;
 while(lst)
 {
  if(lst->addon && (lst->addon->event_mask&KXADDON_EVENT_ACTIVATE))
  {
     if(((lst->addon->addon_type&KXADDON_PER_DEVICE) && (manager->get_current_device()==lst->device_n)) ||
        (lst->addon->addon_type&KXADDON_SYSTEM_WIDE))
     return 1;
  }
  lst=lst->next;
 }

 return 0;
}
