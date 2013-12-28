// kX Audio Driver API / kX Audio Driver Plugin Interface
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

#include "midirouter.h"
#include "kxdspdlg.h"
#include "notify.h"
#include "addonmgr.h"

#include "translate_dsp.cpp"

iKXPlugin *iKXPluginManager::find_plugin_ex(int pgm_id,const char *guid,int **x,int **y)
{
 plugin_list_t *t=0;
 iKXPlugin *r=find_plugin(pgm_id,guid,&t);
 if(t)
 {
  if(x) *x=&t->x;
  if(y) *y=&t->y;
 }
 return r;
}

iKXPlugin *iKXPluginManager::enum_plugin(int number) // 0..inf
{
 plugin_list_t *t=plugin_list;
 for(int i=0;i<number;i++)
 {
  t=t->next;
  if(t==0)
   return NULL;
 }
 if(t)
   return t->plugin;
 else
   return NULL;
}

int iKXPluginManager::describe_plugin(int number,plugin_list_t *info) // 0..inf
{
 plugin_list_t *t=plugin_list;
 if(!plugin_list)
	 return -2;
 for(int i=0;i<number;i++)
 {
  t=t->next;
  if(t==0)
   return -1;
 }
 memcpy(info,t,sizeof(plugin_list_t));
 info->next=0;
 return 0;
}

int iKXPluginManager::get_plugin_type(int pgm_id,const char *guid)
{
 plugin_list_t *t=0;
 iKXPlugin *r=find_plugin(pgm_id,guid,&t);
 if(r && t)
  return t->plugin_type;
 return -1;
}

int iKXPluginManager::get_plugin_file(int pgm_id,const char *guid,char *fname) // fname is supposed to be at least MAX_PATH
{
 plugin_list_t *t=0;
 iKXPlugin *r=find_plugin(pgm_id,guid,&t);
 if(t)
 {
  strncpy(fname,t->file_name,MAX_PATH);
  return 0;
 }
 return -1;
}

iKXPlugin *iKXPluginManager::find_plugin(int pgm_id,const char *guid)
{
 return find_plugin(pgm_id,guid,NULL);
}

iKXPlugin *iKXPluginManager::find_plugin_by_id(int pgm_id,const char *guid,plugin_list_t **pl)
{
 plugin_list_t *plist=plugin_list;

 while(plist)
 {
  if(plist->plugin->pgm_id==pgm_id && ((guid==NULL) || (strcmp(plist->guid,guid)==NULL)))
  {
   if(pl) { *pl=plist; }
   return plist->plugin;
  }
  plist=plist->next;
 }

 return NULL;
}

iKXPlugin *iKXPluginManager::find_plugin_by_guid(const char *guid,int *pgm_id,plugin_list_t **pl)
{
 plugin_list_t *plist=plugin_list;

 while(plist)
 {
  if(strcmp(plist->guid,guid)==NULL)
  {
   if(pl) { *pl=plist; }
   if(pgm_id)
    *pgm_id=plist->plugin->pgm_id;
   return plist->plugin;
  }
  plist=plist->next;
 }

 return NULL;
}

iKXPlugin *iKXPluginManager::find_plugin(int pgm_id,const char *guid,plugin_list_t **pl)
{
 iKXPlugin *plg=NULL;
 if(pl) *pl=0;

 plg=find_plugin_by_id(pgm_id,guid,pl);
 if(plg)
  return plg;

 // not found: dynamic loading of pre-registered plugins (by guid)
 if(guid)
 {
     	// try to dynamically load plugin dll (according to GUID)
        if(load_plugin(guid,&pgm_id,&plg,pl)!=0)
        {
         kSettings cfg;

         cfg.delete_value_abs("Plugins",guid);
         cfg.delete_key_abs("Plugins",guid);

         return NULL;
        }
 }

 // provide 'default' tweaker if possible
 if(plg==NULL)
 {
  static int first=1;
  if(first)
  {
    MessageBox(NULL,"Attention: unknown DSP microcode was uploaded\n"
    "There's no corresponding plugin for this microcode\n"
    "It is recommended to unload it and register the corresponding plugin library","kX Plugin Manager",MB_OK|MB_ICONINFORMATION);

    first=0;
  }
    plg=new iKXPlugin;

    plg->pgm_id=pgm_id;

    plugin_list_t *new_plugin=(plugin_list_t *)malloc(sizeof(plugin_list_t));

    new_plugin->plugin=plg;
    new_plugin->next=plugin_list;
    new_plugin->x=-1;
    new_plugin->y=-1;
    new_plugin->reserved=0;
    new_plugin->size_of=sizeof(plugin_list_t);
    strncpy(new_plugin->file_name,(LPCTSTR)"none",sizeof(new_plugin->file_name));

    new_plugin->plugin_type=3;
    strncpy(new_plugin->guid,plg->get_plugin_description(IKX_PLUGIN_GUID),KX_MAX_STRING);

    plugin_list=new_plugin;

    // update parser data
     if(parser_t!=NULL)
        parser_t->event(plg->pgm_id,plg,IKX_LOAD);

   kSettings cfg(ikx_t->get_device_name());

    if(load_plugin_settings(plg,cfg)) // failed
      plg->set_defaults();

    // set default kX DSP sub-window positions here
    if(new_plugin->x==-1)
    {
       realign_plugin(new_plugin);
    }
 }
 return plg;
}

int iKXPluginManager::have_plugin(const char *guid)
{
 plugin_list_t *t=plugin_list;
 while(t)
 {
  if((strcmp(t->guid,guid)==0) && (t->plugin))
   return t->plugin->pgm_id;

  t=t->next;
 }

 return 0;
}

int iKXPluginManager::tweak_plugin(const char *guid,kDialog *pp)
{
 int pgm=have_plugin(guid);

 if(pgm!=0)
  return tweak_plugin(pgm,pp);

 return -1;
}

int iKXPluginManager::tweak_plugin(int pgm_id,kDialog *pp)
{
 iKXPlugin *plg=find_plugin(pgm_id,NULL);

 if(plg)
 {
  extern int check_guid(const char *guid,int fake=0);
  if(check_guid(plg->get_plugin_description(IKX_PLUGIN_GUID))==0)
   return 0;

  if(plg->cp)
  {
   kSettings cfg(ikx_t->get_device_name());

   save_plugin_settings(plg,cfg);
   try
   {
    plg->cp->extended_gui(PLUGINGUI_FOREGROUND);
   }
   catch(...)
   {
    debug("kX mixer: error calling plugin GUI - an exception\n");
   }
  }
  else
  {
   switch(plg->get_user_interface())
   {
    case IKXPLUGIN_NONE:
    	     return -1;
    case IKXPLUGIN_OWNER:
    case IKXPLUGIN_SIMPLE:
    	{
    	  try
    	  {
             plg->cp=plg->create_cp(pp,&mf);

             if(plg->cp)
             {
                  dword flg=0;
                  if(ikx_t->get_microcode_flag(pgm_id,&flg)==0)
                  {
                   ikx_t->set_microcode_flag(pgm_id,flg|MICROCODE_OPENED);
                  }

                  plg->cp->init_gui();

                  plg->cp->extended_gui(PLUGINGUI_SET_ICON,(uintptr_t)AfxGetApp()->LoadIcon(IDR_ICON), (uintptr_t)TRUE);		// Set big icon

                  // restore plugin position
                  char section[128];
                  char guid[KX_MAX_STRING];
                  sprintf(section,"pgm_%d",pgm_id);

                  kSettings cfg(ikx_t->get_device_name());

                  dword max_x=GetSystemMetrics(SM_CXFULLSCREEN);
                  dword max_y=GetSystemMetrics(SM_CYFULLSCREEN);

                  dword x=max_x/2;
                  dword y=max_y/2;

                  if(cfg.read(section,"tweak_x",&x)==0 &&
                     cfg.read(section,"tweak_y",&y)==0 &&
                     cfg.read(section,"guid",guid,sizeof(guid))==0)
                  {
                   if( (strcmp(plg->get_plugin_description(IKX_PLUGIN_GUID),guid)==0) &&
                       x<max_x-5 && 
                       y<max_y-5 && 
                       x>=-10 && 
                       y>=-10)
                   {
                    plg->cp->extended_gui(PLUGINGUI_SET_POSITION,(dword)x,(dword)y);
                   }
                   else
                    plg->cp->extended_gui(PLUGINGUI_CENTER);
                  }
                   else
                    plg->cp->extended_gui(PLUGINGUI_CENTER);
              }
              plg->cp->show_gui();
              return 0;
          }
          catch(...)
          {
             debug("kX Mixer: an exception on creating plugin window...\n");
             plg->cp=0;
          }
        }
             break;
   }
  }
 }
 return -1;
}

int iKXPluginManager::register_plugin(const char *file_name,int gui)
{
  kString fname;

  if(file_name==NULL)
  {
  	restore_cwd("plugin");
        CFileDialog *f_d = new CFileDialog(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_EXPLORER,
          "kX Plugin (*.kxl), Dane Source (*.da), RIFX (*.rifx)|*.kxl;*.da;*.rifx||",NULL);
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
        	save_cwd("plugin");

        	fname=f_d->GetPathName();
        	delete f_d;

        	if(fname.GetLength()==0) // cancel or empty
        	{
        	 return 0;
        	}
        } else return 0;
  } else fname=file_name;

  debug("kxmixer: plugin api: registering '%s'\n",(LPCTSTR)fname);

  int need_destructor=0;
  if(ikx_t==NULL)
  {
   ikx_t=new iKX();
   need_destructor=1;
  }
  	
  int count=ikx_t->get_object_count((char *)(LPCTSTR)fname);
  debug("kxmixer: total %d plugins found\n",count);
  if(count>0)
  {
   for(int i=0;i<count;i++)
   {
    char p_guid[KX_MAX_STRING];
    if(ikx_t->get_object_guid((char *)(LPCTSTR)fname,i,p_guid)==0)
    {
    	kSettings cfg;
        cfg.write_abs("Plugins",p_guid,(TCHAR *)(LPCTSTR)fname);

        char tmp_str[512];
        strcpy(tmp_str,p_guid);
        strcat(tmp_str,".name");

        if(ikx_t->get_object_name((char *)(LPCTSTR)fname,i,p_guid)==0) // ok
        {
         cfg.write_abs("Plugins",tmp_str,p_guid); // p_guid is already a name, not GUID
        }
        debug("kxmixer: registered [%d] %s:%s\n",i,tmp_str,p_guid);
    }
   }
   if(gui)
   {
    kString t;
    t.Format(mf.get_profile("lang","dsp.register"),count);
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
   ikx_t=NULL;
  }

 return count;
}

int iKXPluginManager::load_plugin(const char *guid,int *pgm_id,iKXPlugin **plg,plugin_list_t *pl)
{
    plugin_list_t *t=0;
    int ret=load_plugin(guid,pgm_id,plg,&t);
    if(pl)
    {
     memcpy(pl,t,sizeof(plugin_list_t));
    }
    return ret;
}

// if *pgm_id==-1 - load_microcode() after loading plugin
// returns <-10 if error is serious and there was no MessageBox
int iKXPluginManager::load_plugin(const char *guid,int *pgm_id,iKXPlugin **plg,plugin_list_t **pl)
{
    iKXPlugin *plugin=NULL;
    int found=-1;
    int is_new_plugin=0;

    char fname[512];

    kSettings cfg;

    if(cfg.read_abs("Plugins",guid,fname,sizeof(fname))!=0)
    {
                return -20;
    }

    int count=ikx_t->get_object_count(fname);
    if(count>0)
    {
      for(int i=0;i<count;i++)
      {
              char new_guid[KX_MAX_STRING];
              if(ikx_t->get_object_guid(fname,i,new_guid)==0)
               if((strcmp(new_guid,guid)==0))
               {
                 found=i;
                 break;
               }
      }
    }
     else 
    {
     return -22;
    }

    if(found==-1)
     return -23;


/*    if(strcmp(guid,"6e302861-0df9-4b21-b91a-6631220da97d")==0)
    {
     __asm
     {
      int 3;
     }
    }
*/

    plugin=(iKXPlugin *)ikx_t->instantiate_object(fname,found);
    if(!plugin)
    {
     MessageBox(NULL,mf.get_profile("errors","instant2"),"kX Mixer",MB_OK|MB_ICONEXCLAMATION);
     return -1;
    }
    plugin->pm=this;

    if(plugin->request_microcode())
    {
    	MessageBox(NULL,mf.get_profile("errors","populate"),"kX Mixer",MB_OK|MB_ICONEXCLAMATION);
    	ikx_t->delete_object(plugin);
    	return -2;
    }

    // should load plugin
    if(*pgm_id<=0)
    {
    		is_new_plugin=1;

                if(plugin->event(IKX_LOAD))
                {
                    MessageBox(NULL,mf.get_profile("errors","plinit"),"kX Mixer",MB_OK|MB_ICONEXCLAMATION);
                    ikx_t->delete_object(plugin);
                    return -3;
                }
                char new_copyright[KX_MAX_STRING]; new_copyright[0]=0;
                char new_comment[KX_MAX_STRING]; new_comment[0]=0;
                char new_engine[KX_MAX_STRING]; new_engine[0]=0;
                char new_created[KX_MAX_STRING]; new_created[0]=0;
                char new_guid[KX_MAX_STRING]; new_guid[0]=0;

      		if(plugin->get_plugin_description(IKX_PLUGIN_COPYRIGHT))
      			strncpy(new_copyright,plugin->get_plugin_description(IKX_PLUGIN_COPYRIGHT),KX_MAX_STRING);
      		if(plugin->get_plugin_description(IKX_PLUGIN_COMMENT))
      			strncpy(new_comment,plugin->get_plugin_description(IKX_PLUGIN_COMMENT),KX_MAX_STRING);
      		if(plugin->get_plugin_description(IKX_PLUGIN_ENGINE))
      			strncpy(new_engine,plugin->get_plugin_description(IKX_PLUGIN_ENGINE),KX_MAX_STRING);
      		if(plugin->get_plugin_description(IKX_PLUGIN_CREATED))
      			strncpy(new_created,plugin->get_plugin_description(IKX_PLUGIN_CREATED),KX_MAX_STRING);
      		if(plugin->get_plugin_description(IKX_PLUGIN_GUID))
      			strncpy(new_guid,plugin->get_plugin_description(IKX_PLUGIN_GUID),KX_MAX_STRING);

                // restore plugin: with absolute id = -(*pgm_id);
                // plugin->name is filled with 'original' name by '->request_microcode'
                 *pgm_id=ikx_t->load_microcode(plugin->name,plugin->code,
                	plugin->code_size,
                	plugin->info,
                	plugin->info_size,
                	plugin->itramsize,
                	plugin->xtramsize,
                        new_copyright,
                        new_engine,
                        new_created,
                        new_comment,
                        new_guid,-(*pgm_id));
    }

    if(*pgm_id<=0)
    {
    	MessageBox(NULL,mf.get_profile("errors","load_m"),"kX Mixer",MB_OK|MB_ICONEXCLAMATION);
    	ikx_t->delete_object(plugin);
    	return -4;
    }

    plugin->pgm_id=*pgm_id;

    plugin_list_t *new_plugin=(plugin_list_t *)malloc(sizeof(plugin_list_t));

    new_plugin->plugin=plugin;
    new_plugin->next=plugin_list;
    new_plugin->x=-1;
    new_plugin->y=-1;
    new_plugin->reserved=0;
    new_plugin->size_of=sizeof(plugin_list_t);
    strncpy(new_plugin->file_name,(LPCTSTR)fname,sizeof(new_plugin->file_name));

    new_plugin->plugin_type=ikx_t->get_object_type(fname);
    strncpy(new_plugin->guid,plugin->get_plugin_description(IKX_PLUGIN_GUID),KX_MAX_STRING);

    plugin_list=new_plugin;

    // update parser data
     if(parser_t!=NULL)
        parser_t->event(plugin->pgm_id,plugin,IKX_LOAD);

    if(is_new_plugin)
     plugin->set_defaults();
    else
    {
     kSettings cfg(ikx_t->get_device_name());

     if(load_plugin_settings(plugin,cfg)) // failed
      plugin->set_defaults();
    }

    // set default window positions here
    if(new_plugin->x==-1)
    {
       realign_plugin(new_plugin);
    }

    if(pl)
     *pl=new_plugin;

    if(plg)
     *plg=plugin;

    return 0;
}

// this will re-create plugin list
int iKXPluginManager::init_plugins(void)
{
 dsp_microcode *m=NULL;
 int size;
 size=ikx_t->enum_microcode(m,0);
 if(size>0)
 {
 	m=(dsp_microcode *)malloc(size);
 	if(m)
 	{
 	 if(ikx_t->enum_microcode(m,size)==0)
 	 {
        	 for(dword i=0;i<size/sizeof(dsp_microcode);i++)
        	 {
        	   find_plugin(m[i].pgm,m[i].guid);
        	 }
         }
 	 free(m);
 	}
 }
 return 0;
}

int iKXPluginManager::unload_plugin(iKXPlugin *plg)
{
 plugin_list_t *plist=plugin_list;
 plugin_list_t *pprev=NULL;

 while(plist)
 {
  if(plist->plugin==plg)
  {
   if(pprev!=NULL)
   {
    pprev->next=plist->next;
   }
   else // remove first one
   {
    plugin_list=plist->next;
   }

   if(plg)
   {
    // destroy parser data
     if(parser_t!=NULL)
        parser_t->event(plg->pgm_id,plg,IKX_UNLOAD);

    plg->event(IKX_UNLOAD);
     
    if(plg->cp)
    {
     try
     {
      plg->cp->hide_gui();
      plg->cp->close_gui();
     }
     catch(...)
     {
      debug("kX mixer: error calling plugin GUI - an exception\n");
     }
     plg->cp=NULL;
    }

    ikx_t->delete_object(plg);
   }
   plist->plugin=NULL;
   free(plist);
   return 0;
  }
  pprev=plist;
  plist=plist->next;
 }
 return -1;
}

int iKXPluginManager::close_plugins(void)
{
        // destroy parser data
         if(parser_t!=NULL)
            parser_t->reset();

        // destroy kX DSP data
        if(dsp_window)
         dsp_window->invalidate_patches();

        while(plugin_list)
        {
        	if(plugin_list->plugin)
        	{
        	 plugin_list->plugin->event(IKX_UNLOAD);

                 // should not save settings here
                 if(plugin_list->plugin->cp)
                 {
                  try
                  {
                   plugin_list->plugin->cp->hide_gui();
                   plugin_list->plugin->cp->close_gui();
                  }
                  catch(...)
                  {
                   debug("kX mixer: error calling plugin GUI - an exception\n");
                  }
                  plugin_list->plugin->cp=NULL;
        	 }
        	 ikx_t->delete_object(plugin_list->plugin);
        	 plugin_list->plugin=NULL;
        	}
        	plugin_list_t *prev=plugin_list;
        	plugin_list=plugin_list->next;
                free(prev);
        }

        return 0;
}

int iKXPluginManager::update_plugins(int where) // update plugin lists
{
 // update newer plugins
 init_plugins();

AGAIN:
 // remove elder plugins
 plugin_list_t *plist=plugin_list;
 plugin_list_t *pprev=NULL;

 while(plist)
 {
  if(plist->plugin)
  {
   dsp_microcode mc;
   if(ikx_t->enum_microcode(plist->plugin->pgm_id,&mc)!=0)
   {
    // unload
    unload_plugin(plist->plugin);
    // fix list pointers OR goto AGAIN
    goto AGAIN;        
   }
  }
  pprev=plist;
  plist=plist->next;
 }

 if(dsp_window && (where==0))
    dsp_window->redraw_window();

 manager->addon_mgr->event(KXADDON_EVENT_GENERIC,(void *)IKX_DSP_CHANGED,0);

 return 0;
}

int iKXPluginManager::reset_all_plugin_settings(void)
{
 plugin_list_t *plist=plugin_list;
 while(plist)
 {
  if(plist->plugin)
  {
   plist->plugin->set_defaults();

   /*
   // outdated code: since FXMix1/2
   // get and set 'original' name
   // note: special case: xSumm_r[6] & xSumm_c[7]
   if(strcmp(plist->guid,"DD2F9FDA-75DB-44a2-8DA2-6B183D421BB1")==0) // xsumm_r/_c
   {
    if(plist->plugin->pgm_id==6)
    {
     strcpy(plist->plugin->name,"xsumm_r");
    }
    if(plist->plugin->pgm_id==7)
    {
     strcpy(plist->plugin->name,"xsumm_c");
    }
   }
   else
   */
   {
    strncpy(plist->plugin->name,plist->plugin->get_plugin_description(IKX_PLUGIN_NAME),KX_MAX_STRING);
   }
   ikx_t->set_microcode_name(plist->plugin->pgm_id,plist->plugin->name);
  }
  realign_plugin(plist);
  plist=plist->next;
 }
 return 0;
}

int iKXPluginManager::save_plugin_settings(iKXPlugin *plg,kSettings &cfg)
{
 plugin_list_t *plugin_cache;
 if(find_plugin(plg->pgm_id,plg->get_plugin_description(IKX_PLUGIN_GUID),&plugin_cache)!=plg)
  return -5;

 dsp_microcode mc;
 if(ikx_t->enum_microcode(plg->pgm_id,&mc)==0)
 {
   plg->event(IKX_SAVE_SETTINGS);

   char section[128];
   sprintf(section,"pgm_%d",plg->pgm_id);

   cfg.write(section,"guid",plugin_cache->guid);
   cfg.write(section,"name",mc.name);
   cfg.write(section,"offset",mc.offset);
   cfg.write(section,"pos_x",plugin_cache->x);
   cfg.write(section,"pos_y",plugin_cache->y);

   if(plg->cp)
   {
    // save window pos if possible
    int x=0,y=0;
    if(plg->cp->extended_gui(PLUGINGUI_GET_POSITION,(uintptr_t)&x,(uintptr_t)&y)==0)
    {
     cfg.write(section,"tweak_x",x);
     cfg.write(section,"tweak_y",y);
    }
    mc.flag|=MICROCODE_OPENED;
   }
    else
     mc.flag&=(~MICROCODE_OPENED);

   cfg.write(section,"flag",mc.flag);

   for(int i=0;i<plg->get_param_count();i++)
   {
    kxparam_t v;
    if(plg->get_param(i,&v)==0)
    {
     char tmp[32];
     char tmp2[32];
     sprintf(tmp,"0x%x",(dword)v); // warning: AMD64 WIN64 _WIN64: 32/64 bit mismatch
     sprintf(tmp2,"p%d",i);
     cfg.write(section,tmp2,tmp);
    }
   }

   int size=ikx_t->get_connections(plg->pgm_id,0,0);
   if(size>0)
   {
    kxconnections *conn=(kxconnections *)malloc(size);
    if(conn)
    {
     if(ikx_t->get_connections(plg->pgm_id,conn,size)==0)
     {
      for(dword j=0;j<size/sizeof(kxconnections);j++)
      {
       char tmp_key[64];
       char tmp_val[64];
       sprintf(tmp_key,"conn_%x",conn[j].this_num);
       sprintf(tmp_val,"%d 0x%x",conn[j].pgm_id,conn[j].num);
       cfg.write(section,tmp_key,tmp_val);
      }
     }
     free(conn);
    }
   }

   plg->save_plugin_settings(cfg);

   return 0;
 }
 return -1;
}

int iKXPluginManager::load_plugin_settings(iKXPlugin *plg,kSettings &cfg,int load_connections)
{
 plugin_list_t *plugin_cache;
 if(find_plugin(plg->pgm_id,plg->get_plugin_description(IKX_PLUGIN_GUID),&plugin_cache)!=plg)
  return -5;

 dsp_microcode mc;
 if(ikx_t->enum_microcode(plg->pgm_id,&mc)==0)
 {
   char section[128];
   sprintf(section,"pgm_%d",plg->pgm_id);

   char tmp_guid[128];
   if(cfg.read(section,"guid",tmp_guid,sizeof(tmp_guid))==0)
   {
     if(strcmp(tmp_guid,plugin_cache->guid)==0)
     {
       plg->event(IKX_LOAD_SETTINGS);

       if(cfg.read(section,"name",tmp_guid,sizeof(tmp_guid))==0)
       {
        ikx_t->set_microcode_name(plg->pgm_id,tmp_guid);
        strncpy(plg->name,tmp_guid,sizeof(plg->name));
       }
       // 'tmp_guid' is now 'name'!

       plg->load_plugin_settings(cfg);

       int n_params=plg->get_param_count();
       if(n_params>0)
       {
	   kxparam_t *params=(kxparam_t *)malloc(n_params*sizeof(kxparam_t));
	   if(params)
	   {
			memset(params,0,n_params*sizeof(kxparam_t));

               for(int i=0;i<n_params;i++)
               {
                    char tmp[32];
                    char tmp2[32];
                    sprintf(tmp,"p%d",i);
                    if(cfg.read(section,tmp,tmp2,sizeof(tmp2))==0)
                    {
                      // AMD64 WIN64 _WIN64
                      // note: 32/64 bit mismatch
                      int tmp=0;
                      if(sscanf(tmp2,"%x",&tmp)!=1)
                        params[i]=0;
                      else
                        params[i]=tmp;
                    }
               }
               plg->set_all_params(params);
	       free(params);
	   }
       }
       // load x,y
       dword t;
       if(cfg.read(section,"pos_x",&t)==0)
        plugin_cache->x=t;
       if(cfg.read(section,"pos_y",&t)==0)
        plugin_cache->y=t;

       // load connections here! 
       if(load_connections)
       {
           int size=ikx_t->get_connections(plg->pgm_id,0,0);
           if(size>0)
           {
            kxconnections *conn=(kxconnections *)malloc(size);
            if(conn)
            {
             if(ikx_t->get_connections(plg->pgm_id,conn,size)==0)
             {
              for(dword j=0;j<size/sizeof(kxconnections);j++)
              {
               char tmp_key[64];
               char tmp_val[64];
               sprintf(tmp_key,"conn_%x",conn[j].this_num);
               if(cfg.read(section,tmp_key,tmp_val,sizeof(tmp_val))==0)
               {
                 int pgm_id=0;
                 dword reg=0;
                 if(sscanf(tmp_val,"%d %x",&pgm_id,&reg)==2)
                 {
                  ikx_t->connect_microcode(plg->pgm_id,conn[j].this_num,pgm_id,(word)reg);
                 }
               }
              }
             }
             free(conn);
            }
           }
       }

       dword flag=0;

       if(cfg.read(section,"flag",&flag)==0)
       {
        if(flag&MICROCODE_OPENED)
          tweak_plugin(plg->pgm_id);
       }

       return 0;
     }
   }
 }
 return -1;
}

int iKXPluginManager::load_all_plugin_settings(kSettings &cfg)
{
    int cnt=0;
    char key[16];

    close_plugins();
    ikx_t->dsp_clear();

    while(1)
    {
     sprintf(key,"mc_%d",cnt);
     cnt++;
     dword id;
     if(cfg.read("microcode",key,&id)==0)
     {
      if(id==0) // the last one
       break;

      char section[128];
      sprintf(section,"pgm_%d",id);

      char tmp_guid[128];
      if(cfg.read(section,"guid",tmp_guid,sizeof(tmp_guid))==0)
      {
        id=-(int)id;

        if(load_plugin(tmp_guid,(int *)&id,NULL)==0) // load_plugin modifies *id;
        {
         // should parse 'offset' and 'flag'
         dword flag=0;
         if(cfg.read(section,"flag",&flag)==0)
         {
          // should translate if necessary
          if(flag&MICROCODE_TRANSLATED)
          {
           dword offset;
           cfg.read(section,"offset",&offset);
           ikx_t->translate_microcode(id,KX_MICROCODE_ABSOLUTE,offset);
          }
          if(flag&MICROCODE_ENABLED)
          {
           ikx_t->enable_microcode(id);

           // cannot set 'bypass' flag at the moment...
          }
         }
        }
      }
     } else break;
    }

    // restore connections here
    plugin_list_t *plist=plugin_list;

    while(plist)
    {
     if(plist->plugin)
     {
      load_plugin_settings(plist->plugin,cfg,1);

      char section[128];
      sprintf(section,"pgm_%d",plist->plugin->pgm_id);
      dword flag=0;
      if(cfg.read(section,"flag",&flag)==0)
      {
         // set bypass option here
         if(flag&MICROCODE_BYPASS)
          ikx_t->set_microcode_bypass(plist->plugin->pgm_id,1);
      }
     }
     plist=plist->next;
    }

    ikx_t->dsp_go();

    return 0;
}

int iKXPluginManager::save_all_plugin_settings(kSettings &cfg)
{
 int cnt=0;     
 char key[16];

 plugin_list_t *plist=plugin_list;

 while(plist)
 {
  if(plist->plugin)
  {
   save_plugin_settings(plist->plugin,cfg);
   sprintf(key,"mc_%d",cnt);
   cfg.write("microcode",key,plist->plugin->pgm_id);
   cnt++;
  }
  plist=plist->next;
 }
 sprintf(key,"mc_%d",cnt);
 cfg.write("microcode",key,0);
 return 0;
}

int iKXPluginManager::realign_plugin(int num)
{
 plugin_list_t *t=plugin_list;
 for(int i=0;i<num;i++)
 {
  t=t->next;
  if(t==0)
   return -1;
 }
 return realign_plugin(t);
}

int iKXPluginManager::realign_plugin(plugin_list_t *new_plugin)
{
       int ret=0;
       if(strcmp(new_plugin->guid,"631d8de5-11bc-4c3d-a0d2-f079977fd184")==0 ) // prolog
       {
        new_plugin->x=64;
        new_plugin->y=369;
        ret=1;
       } else
       if((strcmp(new_plugin->guid,"80100005-0ADF-11D6-BFBC-D4F706E10C52")==0) || 
          (strcmp(new_plugin->guid,"80100004-0ADF-11D6-BFBC-D4F706E10C52")==0)) // eq10a or timbre
       {
        new_plugin->x=546;
        new_plugin->y=27;
        ret=1;
       } else
       if(strcmp(new_plugin->guid,"ceffc302-ea28-44df-873f-d3df1ba31736")==0 ) // epilog
       {
        new_plugin->x=700;
        new_plugin->y=193;
        ret=1;
       } else
       if(strcmp(new_plugin->guid,"2b8b7fa8-98b9-4f6e-81a0-400d3ba39c6f")==0 || // fxbus
          strcmp(new_plugin->guid,"d25a7874-7c00-47ca-8ad3-1b13106bde91")==0) // fxbusx new
       {
        new_plugin->x=5;
        new_plugin->y=27;
        ret=1;
       } else 
       if(strcmp(new_plugin->guid,"bba48159-60af-4772-a931-c74547820353")==0) // spatial
       {
        new_plugin->x=405;
        new_plugin->y=27;
        ret=1;
       } else
       if(strcmp(new_plugin->guid,"0c37979a-ac18-4898-900d-5c6d33557421")==0) // surrounder+
       {
        new_plugin->x=402;
        new_plugin->y=27;
        ret=1;
       } else
       if(strcmp(new_plugin->guid,"313149FA-24FB-4f08-9C22-EB38B651BA58")==0) // xrouting
       {
        new_plugin->x=324;
        new_plugin->y=27;
        ret=1;
       } else
       if(strcmp(new_plugin->guid,"80100003-0ADF-11D6-BFBC-D4F706E10C52")==0) // chorus
       {
        new_plugin->x=194;
        new_plugin->y=280;
        ret=1;
       } else
       if(strcmp(new_plugin->guid,"80100001-0ADF-11D6-BFBC-D4F706E10C52")==0) // reverb
       {
        new_plugin->x=197;
        new_plugin->y=203;
        ret=1;
       } else
       if(strcmp(new_plugin->guid,"cb8abf40-a6b2-417f-aaa5-3402466e40e8")==0) // peak
       {
        new_plugin->x=694;
        new_plugin->y=27;
        ret=1;
       } else
       if(strcmp(new_plugin->guid,"fa7c5e74-1b53-4ac3-b54f-93559e0018bf")==0) // fxmix2
       {
        new_plugin->x=120;
        new_plugin->y=203;
        ret=1;
       } else
       if(strcmp(new_plugin->guid,"85E09152-D489-4dee-8D60-6A509DC70901")==0) // p16v
       {
        new_plugin->x=57;
        new_plugin->y=207;
        ret=1;
       }
       return ret;
}

iKXManager *iKXPluginManager::get_manager()
{
 return manager;
}


int iKXPluginManager::notify(iKXPlugin *plg,int event)
{
 switch(event)
 {
  case IKX_CLOSE_CP:
   {
   kSettings cfg(ikx_t->get_device_name());
   save_plugin_settings(plg,cfg);
   }
   return 0;
  case IKX_DSP_CHANGED:
   {
    update_plugins(0);
   }
   return 0;
  case IKX_CLOSE_AND_REOPEN:
   {
    // postpone async re-open
    if(plg->cp)
    {
     HWND plg_wnd=(HWND)plg->cp->extended_gui(PLUGINGUI_GET_HWND);
     PostMessage(plg_wnd,WM_CLOSE,0,0);
    }
    extern HWND systray;
    PostMessage(systray,WM_KXMIXER_TWEAK,(WPARAM)this,(LPARAM)plg);
   }
   return 0;
  default:
   debug("kxmixer: invalid notify event [plugin manager]: %d\n",event);
   return -1;
 }
}

int iKXPluginManager::notify_ex(iKXPlugin *plg,int event,void *p1)
{
 switch(event)
 {
  case 0:
  default:
   debug("kxmixer: invalid notify_ex event [plugin manager]: %d %x\n",event,p1);
   return -1;
 }
}

int iKXPluginManager::clear_dsp(void)
{
   ikx_t->mute();
   close_plugins();
   ikx_t->dsp_clear();
   ikx_t->dsp_go();
   init_plugins();
   reset_all_plugin_settings();
   ikx_t->unmute();
   
   manager->save_settings(SETTINGS_AUTO); // will reset plugin info in settings file

   return 0;
}

typedef struct
{
	char name[KX_MAX_STRING];
	char guid[KX_MAX_STRING];
	int ndx;
	int category;
}effect_list_t;

static int __cdecl compare (const void *elem1, const void *elem2 )
{
 return strcmp(((effect_list_t *)elem1)->name,((effect_list_t *)elem2)->name);
}

int iKXPluginManager::create_plugins_menu(kMenu *plugin_menu)
{
	kSettings cfg;

	#define MAX_PLUGIN_CATEGORIES	30
        kMenu categorized_menus[MAX_PLUGIN_CATEGORIES];
        kString plugin_categories[MAX_PLUGIN_CATEGORIES];
        int menu_exists[MAX_PLUGIN_CATEGORIES];

	// create items
        for(int i=0;i<MAX_PLUGIN_CATEGORIES;i++)
        {
         menu_exists[i]=0;
         char tmp[4]; sprintf(tmp,"%d",i);
         mf.get_profile("categories",tmp,"unknown",plugin_categories[i]);
        }

        #define MAX_EFFECTS	256
        effect_list_t *effect_list;
        effect_list=(effect_list_t *)malloc(sizeof(effect_list_t)*MAX_EFFECTS);
        memset(effect_list,0,sizeof(effect_list_t)*MAX_EFFECTS);
        int cur_effect=0;

        // enumerate registered effects here...
        int ndx=0;

           while(1)
           {
            char key_name[KX_MAX_STRING]; int keyname_size=sizeof(key_name);
            char value[MAX_PATH]; int value_size=sizeof(value);
            if(cfg.enum_abs(ndx,"Plugins",key_name,keyname_size,value,value_size)==0)
            {
             if(strstr(key_name,".name")!=0)
             {
              *(strstr(key_name,".name"))=0;
              strncpy(effect_list[cur_effect].name,value,KX_MAX_STRING);
              strncpy(effect_list[cur_effect].guid,key_name,KX_MAX_STRING);
              effect_list[cur_effect].ndx=ndx;
              
              kString tmp;
              mf.get_profile("categories",key_name,"-1",tmp);
              sscanf((LPCTSTR)tmp,"%d",&effect_list[cur_effect].category);

              cur_effect++;
             }
            } else
            {
             break;
            }
            ndx++;
           }

        // qsort them here
        qsort(effect_list,cur_effect,sizeof(effect_list_t),compare);

        for(int i=0;i<cur_effect;i++)
        {
        	if(effect_list[i].category>=0 && effect_list[i].category<MAX_PLUGIN_CATEGORIES)
        	{
        	 if(menu_exists[effect_list[i].category]==0)
        	 {
        	  categorized_menus[effect_list[i].category].create();
        	  menu_exists[effect_list[i].category]=1;
        	 }
        	 categorized_menus[effect_list[i].category].add(effect_list[i].name,effect_list[i].ndx+MENU_PLUGIN_BASE);
        	}
        }

        for(int i=0;i<MAX_PLUGIN_CATEGORIES;i++)
        {
          if(menu_exists[i])
        	  plugin_menu->add((LPCTSTR)plugin_categories[i],&categorized_menus[i]);
        }
        plugin_menu->separator();
        for(int i=0;i<cur_effect;i++)
        {
        	if(effect_list[i].category<0 || effect_list[i].category>MAX_PLUGIN_CATEGORIES)
        	{
        	 plugin_menu->add(effect_list[i].name,effect_list[i].ndx+MENU_PLUGIN_BASE);
        	}
        }
        free(effect_list);

        return 0;
}

int iKXPluginManager::get_version()
{
 return KXPLUGINMANAGER_VERSION;
}

int iKXPluginManager::unregister_plugin(int ndx)
{
	kSettings cfg;

        ndx-=MENU_PLUGIN_BASE;

        char key_name[KX_MAX_STRING+5]; 
        int keyname_size=sizeof(key_name);

        if(cfg.enum_abs(ndx,"Plugins",key_name,keyname_size,NULL,0)==0)
        {
               if(strstr(key_name,".name")!=0)
               {
                 *(strstr(key_name,".name"))=0;

                 cfg.delete_value_abs("Plugins",key_name);
                 cfg.delete_key_abs("Plugins",key_name,1); // complete (via shell)
                 strcat(key_name,".name");
                 cfg.delete_value_abs("Plugins",key_name);

                 return 0;
               }
        }

        return -1;
}

int iKXPluginManager::load_plugin(int ndx,iKXPlugin **plg_)
{
	kSettings cfg;

        ndx-=MENU_PLUGIN_BASE;

        if(plg_)
         *plg_=NULL;

        char key_name[KX_MAX_STRING+5]; 
        int keyname_size=sizeof(key_name);

        if(cfg.enum_abs(ndx,"Plugins",key_name,keyname_size,NULL,0)==0)
        {
               if(strstr(key_name,".name")!=0)
               {
                *(strstr(key_name,".name"))=0;
                int pgm_id=0; // load microcode
                iKXPlugin *plg=0;

                int ret=load_plugin(key_name,&pgm_id,&plg);

                if(plg_)
                 *plg_=plg;

                return ret;
               }
        }
        return -1;
}

int iKXPluginManager::perform_translate(kDialog *that,iKXPlugin *plugin,int gui_based)
{
	kSettings cfg;
	dword param=0;

	cfg.read_abs("Mixer","EnhDSP",&param);

	int ret=-1234;

	if(param==1 && gui_based==1)
	{
        	CTranslateDlg dlg;
        	int s=dlg.do_modal(that);
                if(s==IDOK)
                {
        		ret=ikx_t->translate_microcode(plugin->pgm_id,dlg.c_mode,dlg.c_pgm);
        		if((ret==0) && plugin)
        		  plugin->event(IKX_TRANSLATE);
        	}
        	else
        	  return -10;
        }
        else
        {
        		ret=ikx_t->translate_microcode(plugin->pgm_id);
        		if((ret==0) && plugin)
        		 plugin->event(IKX_TRANSLATE);
        }

        if(gui_based)
         switch(ret)
         {
          case -1: MessageBox(that->get_wnd(),(LPCTSTR)mf.get_profile("lang","translate.no_itram"),(LPCTSTR)mf.get_profile("errors","translate"),MB_OK|MB_ICONEXCLAMATION); break;
          case -2: MessageBox(that->get_wnd(),(LPCTSTR)mf.get_profile("lang","translate.no_xtram"),(LPCTSTR)mf.get_profile("errors","translate"),MB_OK|MB_ICONEXCLAMATION); break;
          case -3: MessageBox(that->get_wnd(),(LPCTSTR)mf.get_profile("lang","translate.no_gpr"),(LPCTSTR)mf.get_profile("errors","translate"),MB_OK|MB_ICONEXCLAMATION); break;
          case -4: MessageBox(that->get_wnd(),(LPCTSTR)mf.get_profile("lang","translate.bad_trama"),(LPCTSTR)mf.get_profile("errors","translate"),MB_OK|MB_ICONEXCLAMATION); break;
          case -5: MessageBox(that->get_wnd(),(LPCTSTR)mf.get_profile("lang","translate.no_tram"),(LPCTSTR)mf.get_profile("errors","translate"),MB_OK|MB_ICONEXCLAMATION); break;
          case -6: MessageBox(that->get_wnd(),(LPCTSTR)mf.get_profile("lang","translate.bad_reg"),(LPCTSTR)mf.get_profile("errors","translate"),MB_OK|MB_ICONEXCLAMATION); break;
          case -7: MessageBox(that->get_wnd(),(LPCTSTR)mf.get_profile("lang","translate.no_mc"),(LPCTSTR)mf.get_profile("errors","translate"),MB_OK|MB_ICONEXCLAMATION); break;
          case 0: break;
          default:
               MessageBox(that->get_wnd(),(LPCTSTR)mf.get_profile("errors","translate"),"kX Mixer",MB_OK|MB_ICONEXCLAMATION); break;
         }

        return ret;
}

int iKXPluginManager::move_plugin_window(int pgm_id,int x,int y)
{
  int *pl_x=0,*pl_y=0;
  find_plugin_ex(pgm_id,NULL,&pl_x,&pl_y);
  if(pl_x)
   *pl_x=x;
  if(pl_y)
   *pl_y=y;

  return 0;
}

int iKXPluginManager::move_plugin_window(iKXPlugin *plg,int x,int y)
{
 return move_plugin_window(plg->pgm_id,x,y);
}

int iKXPluginManager::connect(iKXPlugin *plg1,int reg1,iKXPlugin *plg2,int reg2,int multiple)
{
    int ret=ikx_t->connect_microcode((plg1?plg1->pgm_id:-1),(plg1?plg1->info[reg1].num:(word)reg1),
         (plg2?plg2->pgm_id:-1),plg2?plg2->info[reg2].num:(word)reg2);

    if(multiple && (ret==0) && plg1)
    {
     // perform additional connection
     int src_reg=reg1;
     int dst_reg=reg2;
     src_reg++; dst_reg++;

     while(1)
     {
      // check if there are no more regs
      if(plg1->info_size<=(int)(src_reg*sizeof(dsp_register_info)))
       break;
      if(plg2 && plg2->info_size<=(int)(dst_reg*sizeof(dsp_register_info)))
       break;

      if(plg1->info[src_reg].type!=plg1->info[reg1].type)
       { src_reg++; continue; }
      if(plg2 && plg2->info[dst_reg].type!=plg2->info[reg2].type)
       { dst_reg++; continue; }
      // check if physical register has become invalid?..
      if(plg2==NULL)
      {
       debug("kxmixer: we need to check if phys. register is still valid [%x]\n",
        dst_reg);
      }

      ret=ikx_t->connect_microcode(plg1->pgm_id,(word)plg1->info[src_reg].num,
       plg2?plg2->pgm_id:-1,plg2?plg2->info[dst_reg].num:(word)dst_reg);
      if(ret)
       break;
      src_reg++;
      dst_reg++;
     }
    }

    return 0;
}

