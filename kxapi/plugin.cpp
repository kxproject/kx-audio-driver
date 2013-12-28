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

#if defined(WIN32)

#undef CDC
#include "gui/kGui.h"
#include "interface/kxplugingui.h"
#include "defplugingui.h"

#include "dane/danesrc.h"


int iKX::get_object_type(const char *fname)
{
 char *p=strrchr(fname,'.');
 if(p)
 {
  if(stricmp(p,".da")==0) return KX_OBJECT_DANE;
  if(stricmp(p,".rifx")==0) return KX_OBJECT_RIFX;
  if(stricmp(p,".kxl")==0) return KX_OBJECT_KXL;
  if(stricmp(p,".kxa")==0) return KX_OBJECT_ADDON;
 }
 return -1; // error
}

int iKX::delete_object(iKXPlugin *plugin)
{
 if(plugin)
 {
    HINSTANCE inst=plugin->instance;
    delete plugin;
    if(inst)
      FreeLibrary(inst);
 }
 return 0;
}

int iKX::delete_object(iKXAddOn *addon)
{
 if(addon)
 {
    HINSTANCE inst=addon->instance;
    delete addon;
    if(inst)
      FreeLibrary(inst);
 }
 return 0;
}

void *iKX::instantiate_object(const char *library,int id) // returns 0 if failed
{
 int type=get_object_type(library);
 int ver=KXPLUGIN_VERSION;

 char *func="publish_plugins";

 switch(type)
 {
  case KX_OBJECT_RIFX:
  case KX_OBJECT_DANE:
  {
     strcpy(error_name,"Error loading Dane Source");

     if(id!=0)
     {
       debug("iKX plugin: instantiate source with id=%d\n",id);
       return NULL;
     }

     iKXDaneSource *dane;
     dane=new iKXDaneSource();

     if(dane==0)
     {
       debug("iKX plugin: instantiate: no more memory\n");
       return NULL;
     }

     if(dane->init(library,this)==0)
     {
        return dane;
     }
     debug("iKX plugin: instantiate: init() failed [id: %d; %s]\n",id,dane->name_?dane->name_:NULL);

     delete dane;

     return NULL;
 }
 case KX_OBJECT_ADDON:
   func="publish_addons";
   ver=KXADDON_VERSION;
   // fall thru
 case KX_OBJECT_KXL:
 {
    HINSTANCE inst;
    inst=LoadLibrary(library);
    if(inst!=0)
    {
     kxplugin_publish_t pp;
     pp=(kxplugin_publish_t)GetProcAddress(inst,func);
     if(pp)
     {
      uintptr_t ret=0;
      uintptr_t result;

      try
      {
       result=pp(KXPLUGIN_GET_VERSION,ver,&ret);
      }
      catch(...)
      {
       debug("iKXPlugin: exception on getting plugin version\n");
       result=(uintptr_t)-1;
      }

      if(result || (ret!=(dword)ver))
      {
       debug("iKX plugin: version=%d; should be %d; result=%d\n",ret,ver,result);
       strcpy(error_name,"Invalid Plugin Version");
       return 0;
      }

      ret=0;
      try
      {
       result=pp(KXPLUGIN_INSTANTIATE,id,&ret);
      }
      catch(...)
      {
       debug("iKXPlugin: exception on instantiation\n");
       result=(uintptr_t)-1;
      }
      if((result==0) && (ret!=0)) // ok
      {
       if(type==KX_OBJECT_KXL)
       {
         iKXPlugin *plugin=(iKXPlugin *)ret;
         plugin->instance=inst;
         plugin->ikx=this;
         plugin->kxdevice=device_num;
         plugin->cp=NULL;
         plugin->pm=NULL;

         // init() is NOT called by the constructor
         plugin->init();

         return plugin;
       }
       else
        if(type==KX_OBJECT_ADDON)
        {
         iKXAddOn *addon=(iKXAddOn *)ret;
         addon->instance=inst;

         return addon;
        }
        else
        {
         debug("iKXPlugin: internal error\n");
         return NULL;
        }
      }
      debug("iKX plugin: instantiate: pp(instantiate) failed [%d]\n",result);
      strcpy(error_name,"Error instantiating plugin");
      // error
      FreeLibrary(inst);
      return 0;
     }
     debug("iKX plugin: instantiate: getprocaddress invalid\n");
    }
    debug("iKX plugin: instantiate: loadlibrary failed [%x]\n",GetLastError());
    strcpy(error_name,"Error loading Plugin DLL");
 }
 }
 return 0;
}

void *iKX::instantiate_object(const char *library,const char *guid) // returns 0 if failed
{
 int type=get_object_type(library);
 int ver=KXPLUGIN_VERSION;

 char *func="publish_plugins";

 switch(type)
 {
  case KX_OBJECT_RIFX:
  case KX_OBJECT_DANE:
  {
     strcpy(error_name,"Error loading Dane Source");

     if(guid==0)
     {
       debug("iKX plugin: instantiate source with guid=NULL\n");
       return NULL;
     }

     iKXDaneSource *dane;
     dane=new iKXDaneSource();

     if(dane==0)
     {
       debug("iKX plugin: instantiate: no more memory\n");
       return NULL;
     }

     if(dane->init(library,this)==0)
     {
        return dane;
     }
     debug("iKX plugin: instantiate: init() failed [guid: %s; %s]\n",guid,dane->name_?dane->name_:NULL);

     delete dane;

     return NULL;
 }
 case KX_OBJECT_ADDON:
   func="publish_addons";
   ver=KXADDON_VERSION;
   // fall thru
 case KX_OBJECT_KXL:
 {
    HINSTANCE inst;
    inst=LoadLibrary(library);
    if(inst!=0)
    {
     kxplugin_publish_t pp;
     pp=(kxplugin_publish_t)GetProcAddress(inst,func);
     if(pp)
     {
      uintptr_t ret=0;
      uintptr_t result;

      try
      {
       result=pp(KXPLUGIN_GET_VERSION,ver,&ret);
      }
      catch(...)
      {
       debug("iKXPlugin: exception on getting plugin version\n");
       result=(uintptr_t)-1;
      }

      if(result || (ret!=(dword)ver))
      {
       debug("iKX plugin: version=%d; should be %d; result=%d\n",ret,ver,result);
       strcpy(error_name,"Invalid Plugin Version");
       return 0;
      }

      ret=0;
      try
      {
       result=pp(KXPLUGIN_INSTANTIATE_BY_GUID,(uintptr_t)guid,&ret);
      }
      catch(...)
      {
       debug("iKXPlugin: exception on instantiation\n");
       result=(uintptr_t)-1;
      }
      if((result==0) && (ret!=0)) // ok
      {
       if(type==KX_OBJECT_KXL)
       {
         iKXPlugin *plugin=(iKXPlugin *)ret;
         plugin->instance=inst;
         plugin->ikx=this;
         plugin->kxdevice=device_num;
         plugin->cp=NULL;
         plugin->pm=NULL;

         // init() is NOT called by the constructor
         plugin->init();

         return plugin;
       }
       else
        if(type==KX_OBJECT_ADDON)
        {
         iKXAddOn *addon=(iKXAddOn *)ret;
         addon->instance=inst;

         return addon;
        }
        else
        {
         debug("iKXPlugin: internal error\n");
         return NULL;
        }
      }
      debug("iKX plugin: instantiate: pp(instantiate) failed [%d]\n",result);
      strcpy(error_name,"Error instantiating plugin");
      // error
      FreeLibrary(inst);
      return 0;
     }
     debug("iKX plugin: instantiate: getprocaddress invalid\n");
    }
    debug("iKX plugin: instantiate: loadlibrary failed [%x]\n",GetLastError());
    strcpy(error_name,"Error loading Plugin DLL");
 }
 }
 return 0;
}



int iKX::get_object_count(const char *library) // returns <=0 if failed
{
 int type=get_object_type(library);
 char *func="publish_plugins";
 int ver=KXPLUGIN_VERSION;

 switch(type)
 {
 case KX_OBJECT_DANE:
 case KX_OBJECT_RIFX:
 {
    strcpy(error_name,"Error loading Dane Source");
    iKXDaneSource dane;

    if(dane.init(library,this)==0)
    {
      return 1;
    }
    debug("!! iKX plugin: iKXDaneSource.init failed [%s]\n",library);
    return -5;
 }
 case KX_OBJECT_ADDON:
  func="publish_addons";
  ver=KXADDON_VERSION;
  // fall thru
 case KX_OBJECT_KXL:
 {
    HINSTANCE inst;
    inst=LoadLibrary(library);
    if(inst!=0)
    {
     kxplugin_publish_t pp;
     pp=(kxplugin_publish_t)GetProcAddress(inst,func);
     if(pp)
     {
      uintptr_t ret=0;
      uintptr_t result;

      try
      {
       result=pp(KXPLUGIN_GET_VERSION,ver,&ret);
      }
      catch(...)
      {
       debug("iKXPlugin: exception on getting plugin version\n");
       result=(uintptr_t)-1;
      }
      if(result || (ret!=(dword)ver))
      {
       debug("iKX plugin: get_count(): ret=%d result=%d v=%d\n",ret,result,ver);
       strcpy(error_name,"Invalid Plugin Version");
       return 0;
      }

      uintptr_t num=0;
      try
      {
       ret=pp(KXPLUGIN_GET_COUNT,ver,&num);
      }
      catch(...)
      {
       debug("iKXPlugin: exception on getting plugin count\n");
       ret=(uintptr_t)-1;
      }
      FreeLibrary(inst);
      if(ret==0)
      {
       return (int)num;
      }
      debug("iKX plugin: pp(get_count) failed [%d]\n",ret);
      return 0;
     }
     debug("iKX plugin: get_count(): getprocaddress failed\n");
     strcpy(error_name,"Invalid FX Library");
     FreeLibrary(inst);
     return 0;
    }
    debug("iKX plugin: get_count(): loadlibrary failed [%x]\n",GetLastError());
    strcpy(error_name,"Error loading Plugin DLL");
    return 0;
 }
 }

 return -1;
}

int iKX::get_object_name(const char *library,int id,char *ret_name) // returns <0 if failed 0 - success
{
 int type=get_object_type(library);
 char *func="publish_plugins";
 int ver=KXPLUGIN_VERSION;

 switch(type)
 {
  case KX_OBJECT_DANE:
  case KX_OBJECT_RIFX:
  {
    if(id!=0)
      return -2;

    iKXDaneSource dane;

    if(dane.init(library,this)==0)
    {
       strncpy(ret_name,dane.name,KX_MAX_STRING);
       return 0;
    }
        debug("iKX plugin: get_name(): dane.init failed\n");

    return -5;
 }
 case KX_OBJECT_ADDON:
  func="publish_addons";
  ver=KXADDON_VERSION;
  // fall thru
 case KX_OBJECT_KXL:
 {
   HINSTANCE inst;
   inst=LoadLibrary(library);
   if(inst!=0)
   {
    kxplugin_publish_t pp;
    pp=(kxplugin_publish_t)GetProcAddress(inst,func);
    if(pp)
    {
     uintptr_t ret=0;
     uintptr_t result;

     try
     {
      result=pp(KXPLUGIN_GET_VERSION,ver,&ret);
     }
     catch(...)
     {
      result=(uintptr_t)-1;
      debug("iKXPlugin: exception on plugin actions 1\n");
     }
     if(result || (ret!=(uintptr_t)ver))
     {
      debug("iKX plugin: get_name(): version invalid [res=%d ret=%d v=%d]\n",result,ret,ver);
      strcpy(error_name,"Invalid Plugin Version");
      return -5;
     }

     try
     {
      ret=pp(KXPLUGIN_GET_NAME,id,(uintptr_t *)ret_name);
     }
     catch(...)
     {
      ret=(uintptr_t)-1;
      debug("iKXPlugin: exception on plugin actions 2\n");
     }
     FreeLibrary(inst);
     if(ret==0)
     {
      return 0;
     }
     debug("iKX plugin: get_name() pp(get_name) failed [%d]\n",ret);
     return -1;
    }
    debug("iKX plugin: get_name(): getprocaddress failed\n");
    FreeLibrary(inst);
    return -2;
   }
   debug("iKX plugin: get_name(): loadlibrary failed\n");
   return -3;
 }
 }

 return -1;
}

int iKX::get_object_guid(const char *library,int id,char *ret_name) // returns 0 if failed
{
 int type=get_object_type(library);
 char *func="publish_plugins";
 int ver=KXPLUGIN_VERSION;

 switch(type)
 {
  case KX_OBJECT_RIFX:
  case KX_OBJECT_DANE:
  {
    if(id!=0)
      return -2;

    iKXDaneSource dane;

    if(dane.init(library,this)==0)
    {
       strncpy(ret_name,dane.guid,KX_MAX_STRING);
       return 0;
    }
        debug("iKX plugin: get_guid(): dane.init failed\n");

    return -5;
 }
 case KX_OBJECT_ADDON:
    func="publish_addons";
    ver=KXADDON_VERSION;
        // fall thru
 case KX_OBJECT_KXL:
 {
 HINSTANCE inst;
 inst=LoadLibrary(library);
 if(inst!=0)
 {
  kxplugin_publish_t pp;
  pp=(kxplugin_publish_t)GetProcAddress(inst,func);
  if(pp)
  {
   uintptr_t ret=0;
   uintptr_t result;

   try
   {
    result=pp(KXPLUGIN_GET_VERSION,id,&ret);
   }
   catch(...)
   {
    result=(uintptr_t)-1;
    debug("iKXPlugin: exception on plugin actions 3\n");
   }
   if(result || (ret!=(dword)ver))
   {
    debug("iKX plugin: get_guid(): version invalid: res=%d ret=%d v=%d\n",result,ret,ver);
    strcpy(error_name,"Invalid Plugin Version");
    return 0;
   }

   try
   {
    ret=pp(KXPLUGIN_GET_GUID,id,(uintptr_t *)ret_name);
   }
   catch(...)
   {
    ret=(uintptr_t)-1;
    debug("iKXPlugin: exception on plugin actions 4\n");
   }
   FreeLibrary(inst);
   if(ret==0)
   {
    return 0;
   }
   debug("iKX plugin: get_guid(): pp(get_guid) failed [%d]\n",ret);
   return -1;
  }
  debug("iKX plugin: get_guid(): getprocaddress failed\n");
  strcpy(error_name,"Invalid FX Library");
  FreeLibrary(inst);
  return -2;
 }
 debug("iKX plugin: get_guid(): loadlibrary failed\n");
 strcpy(error_name,"FX Library not found");
 return -3;
 }
 }

 return -5;
}


// --------------------------------------
// iKXPlugin implementation
// --------------------------------------

iKXPlugin::iKXPlugin()
{
 ikx=NULL; kxdevice=-1; pgm_id=-1; 

 itramsize=0;
 xtramsize=0;
 code_size=0;
 info_size=0;
 info=NULL;
 code=NULL;
 memset(name,0,sizeof(name));
 cp=NULL;
 dsp_wnd=NULL;

 // note: vtable is not initialized yet
 // don't call init(); here!
}

iKXPlugin::~iKXPlugin()
{ 
 close();

 if(dsp_wnd) { delete dsp_wnd; dsp_wnd=NULL; }
 if(ikx) { ikx=NULL; kxdevice=-1; } pgm_id=-1;
 if(cp) { cp->close_gui(); cp=0; }
}

int iKXPlugin::get_version()
{
 return KXPLUGIN_VERSION;
}

int iKXPlugin::set_dsp_register(word id,dword val)
{
 return ikx->set_dsp_register(pgm_id,id,val);
}

int iKXPlugin::set_dsp_register(const char *id,dword val)
{
 return ikx->set_dsp_register(pgm_id,id,val);
}

int iKXPlugin::get_dsp_register(word id,dword *val)
{
 return ikx->get_dsp_register(pgm_id,id,val);
}
int iKXPlugin::get_dsp_register(const char *id,dword *val)
{
 return ikx->get_dsp_register(pgm_id,id,val);
}

int iKXPlugin::set_tram_addr(word id,dword addr) // relative to itram_start
{
 return ikx->set_tram_addr(pgm_id,id,addr);
}
int iKXPlugin::get_tram_addr(word id,dword *addr)
{
 return ikx->get_tram_addr(pgm_id,id,addr);
}
int iKXPlugin::set_tram_flag(word id,dword flag)
{
 return ikx->set_tram_flag(pgm_id,id,flag);
}
int iKXPlugin::get_tram_flag(word id,dword *flag)
{
 return ikx->get_tram_flag(pgm_id,id,flag);
}

int iKXPlugin::write_instruction(int offset,word op,word z,word w,word x,word y,dword valid)
{
 return ikx->write_instruction(pgm_id,offset,op,z,w,x,y,valid);
}

int iKXPlugin::read_instruction(int offset,word *op,word *z,word *w,word *x,word *y)
{
 return ikx->read_instruction(pgm_id,offset,op,z,w,x,y);
}


// --------- empty implementations -----------
int iKXPlugin::init() // general initialization of plugin-specific data; microcode not loaded yet
{
 return 0;
}

int iKXPlugin::close() // microcode is already unloaded
{
 return 0;
}

int iKXPlugin::request_microcode() // generally is called after init()
{
 info=NULL;
 code=NULL;
 info_size=0;
 code_size=0;
 itramsize=0;
 xtramsize=0;
 name[0]=0;
 return 0;
}

int iKXPlugin::event(int ev)
{
 if(cp)
 {
  cp->extended_gui(PLUGINGUI_EVENT,ev);
 }
 return 0;
}

int iKXPlugin::event_ex(int ev,void *p1)
{
 if(cp)
 {
  cp->extended_gui(PLUGINGUI_EVENT_EX,ev,(uintptr_t)p1);
 }
 return 0;
}

int iKXPlugin::set_defaults() // resets GPRs to defaults; should call set_dsp_register(...) etc...
{
 int ret=0;
 if(info && pgm_id>0)
 {
  for(dword i=0;i<info_size/sizeof(dsp_register_info);i++)
  {
   if((info[i].type&GPR_MASK)==GPR_CONTROL)
   {
    ret+=set_dsp_register(info[i].num,info[i].p);
   }
  }
 }
 return 0;
}

int iKXPlugin::set_param(int ndx,kxparam_t value) // 0...count
{
 int count=-1;
 if(info && info_size)
 {
  for(dword i=0;i<info_size/sizeof(dsp_register_info);i++)
  {
   if((info[i].type&GPR_MASK)==GPR_CONTROL)
    count++;
   if(count==ndx)
   {
    if(!set_dsp_register(info[i].num,(dword)value))
    {
     // 'sync' to UI if UI class is 'kX'
     if(cp) 
      if(((iKXPluginGUI*) cp)->extended_gui(PLUGINGUI_GET_CLASS,0,0)==1)
      {
       ((CKXPluginGUI *)cp)->sync(ndx);
      }
     return 0;
    }
    else
     return -2;
   }
  }
 }
 return count;
}

int iKXPlugin::get_param(int ndx,kxparam_t *value)
{
 int count=-1;
 if(info && info_size)
 {
  for(dword i=0;i<info_size/sizeof(dsp_register_info);i++)
  {
   if((info[i].type&GPR_MASK)==GPR_CONTROL)
    count++;
   if(count==ndx)
   {
    dword ret;
    if(!get_dsp_register(info[i].num,&ret))
    {
     *value=(kxparam_t)ret;
     return 0;
    }
    else
     return -2;
   }
  }
 }
 return count;
}

int iKXPlugin::set_all_params(kxparam_t *values) // at least [count]
{
 int ret=0;
 for(int i=0;i<get_param_count();i++)
  ret+=set_param(i,values[i]);
 return ret;
}


int iKXPlugin::get_all_params(kxparam_t *values) // at least [count]
{
 int ret=0;
 for(int i=0;i<get_param_count();i++)
  ret+=get_param(i,&values[i]);
 return ret;
}

int iKXPlugin::describe_param(int ndx,kx_fxparam_descr *descr)
{
 int count=-1;
 if(info && info_size)
 {
  for(dword i=0;i<info_size/sizeof(dsp_register_info);i++)
  {
   if((info[i].type&GPR_MASK)==GPR_CONTROL)
    count++;
   if(count==ndx)
   {
    strncpy(descr->name,info[i].name,KX_MAX_STRING);    // note: descr's size is KX_MAX_STRING, info[i].name is only MAX_GPR_NAME
    descr->type=KX_FXPARAM_LEVEL;
    descr->min_value=(dword)0x0;
    descr->max_value=(dword)0x7fffffff;
    return 0;
   }
  }
 }
 return count;
}

int iKXPlugin::get_param_count(void)
{
 int count=-1;
 if(info && info_size)
 {
  count=0;
  for(dword i=0;i<info_size/sizeof(dsp_register_info);i++)
  {
   if((info[i].type&GPR_MASK)==GPR_CONTROL)
    count++;
  }
 }
 return count;
}

iKXPluginGUI* iKXPlugin::create_cp(kDialog *parent,kFile *mf)
{
 if(get_user_interface()==IKXPLUGIN_SIMPLE)
 {
    iPluginDlg *tmp;
    tmp = new iPluginDlg(this, parent, mf);
    tmp->create();
    return tmp;
 }
  else
 {
   return NULL;
 }
}

int iKXPlugin::get_user_interface()
{
 if(get_param_count()>0)
 {
   // for FXBus it get_param_count() returns 0
   if(
      strcmp(get_plugin_description(IKX_PLUGIN_GUID),"631d8de5-11bc-4c3d-a0d2-f079977fd184")==0 ||   // prolog
      strcmp(get_plugin_description(IKX_PLUGIN_GUID),"313149FA-24FB-4f08-9C22-EB38B651BA58")==0 ||  // xrouting
      strcmp(get_plugin_description(IKX_PLUGIN_GUID),"ceffc302-ea28-44df-873f-d3df1ba31736")==0      // epilog
     )
      return IKXPLUGIN_NONE;

   return IKXPLUGIN_SIMPLE;
 }
 else
   return IKXPLUGIN_NONE;
}

const char *iKXPlugin::get_plugin_description(int id)
{
 switch(id)
 {
  case IKX_PLUGIN_NAME:
    return "none"; // 'const' 'original'
  case IKX_PLUGIN_COPYRIGHT:
    return "Copyright (c) Eugene Gavrilov, 2001-2005";
  case IKX_PLUGIN_ENGINE:
    return "10kX";
  case IKX_PLUGIN_CREATED:
    return __DATE__;
  case IKX_PLUGIN_COMMENT:
    return "";
  case IKX_PLUGIN_GUID:
    return "00000000-0000-0000-000000000000";
  default:
    return NULL;
 }
}

const kxparam_t *iKXPlugin::get_plugin_presets()
{
 return NULL;
}

int iKXPlugin::populate_presets(kMenu *mnu)
{
   // built-in presets first...
   int need_sep=0;
   int to_ret=-1;

    try
    {
       const kxparam_t *t=get_plugin_presets();
       
       if(t)
       {
          int num=get_param_count();
          int ndx=0;

          while(1)
          {
           const char *name=(char *)t[0];
           if(name==0)
            break;

           mnu->add(name,IKXPLUGIN_PRESETS_BUILTIN+ndx);
           to_ret=1;
           need_sep=1;
           ndx++;
           t+=(num+1);
          }
       }
    }
    catch(...)
    {
     debug("Incorrect plugin: %d [pointer from populate_presets()]\n",__LINE__);
    }


   int ndx=0;
   kSettings cfg;
   char full_key[127+KX_MAX_STRING];
   sprintf(full_key,"Plugins\\%s",get_plugin_description(IKX_PLUGIN_GUID));

   char *value; 
   value=(char *)malloc((get_param_count()+1)*sizeof(kxparam_t));

      while(1)
      {
       char key_name[KX_MAX_STRING+256]; 

       if(cfg.enum_abs(ndx,full_key,key_name,sizeof(key_name),value,(get_param_count()+1)*sizeof(kxparam_t))==0)
       {
        if(key_name[0]!='#')
        {
          if(need_sep)
          {
           mnu->separator();
           need_sep=0;
          }
          mnu->add(key_name,IKXPLUGIN_PRESETS_CUSTOM+ndx);
          to_ret=0;
        }
       }
        else
       {
        break;
       }
       ndx++;
      }

   free(value);
      return to_ret;
}

int iKXPlugin::populate_presets(HMENU mnu)
{
   // built-in presets first...
   int need_sep=0;
   int to_ret=-1;

    try
    {
       const kxparam_t *t=get_plugin_presets();
       
       if(t)
       {
          int num=get_param_count();
          int ndx=0;

          while(1)
          {
           char *name=(char *)t[0];
           if(name==0)
            break;

           AppendMenu(mnu,MFT_STRING,IKXPLUGIN_PRESETS_BUILTIN+ndx,name);
           to_ret=1;
           need_sep=1;
           ndx++;
           t+=(num+1);
          }
       }
    }
    catch(...)
    {
     debug("Incorrect plugin: %d [pointer from populate_presets()]\n",__LINE__);
    }


   int ndx=0;
   kSettings cfg;
   char full_key[127+KX_MAX_STRING];
   sprintf(full_key,"Plugins\\%s",get_plugin_description(IKX_PLUGIN_GUID));

      while(1)
      {
       char key_name[KX_MAX_STRING+256]; 
       char value[MAX_PATH]; 

       if(cfg.enum_abs(ndx,full_key,key_name,sizeof(key_name),value,sizeof(value))==0)
       {
        if(key_name[0]!='#')
        {
          if(need_sep)
          {
           AppendMenu(mnu,MFT_SEPARATOR,0,"");
           need_sep=0;
          }
          AppendMenu(mnu,MFT_STRING,IKXPLUGIN_PRESETS_CUSTOM+ndx,key_name);
          to_ret=0;
        }
       }
        else
       {
        break;
       }
       ndx++;
      }

      return to_ret;
}

int iKXPlugin::apply_preset(int ndx)
{
   int success=0;

   if(ndx>=IKXPLUGIN_PRESETS_CUSTOM && ndx<=IKXPLUGIN_PRESETS_CUSTOM+IKXPLUGIN_MAX_PRESETS)
   {
       kSettings cfg;

       char full_key[127+KX_MAX_STRING];
       sprintf(full_key,"Plugins\\%s",get_plugin_description(IKX_PLUGIN_GUID));

       char key_name[KX_MAX_STRING+256]; 

       try
       {
           dword cnt=get_param_count();

           kxparam_t *value=(kxparam_t *)malloc((cnt+1)*sizeof(kxparam_t));

           if(value)
           {
               // add two 'guard bytes' (due to kSettings.enum_abs limitations...)
               value[cnt-1]=0x12345678;
               value[cnt]=0x87654321;

               if(cfg.enum_abs(ndx-IKXPLUGIN_PRESETS_CUSTOM,full_key,key_name,sizeof(key_name),
                  (char *)value,cnt*sizeof(kxparam_t))==0)
               {
                if(value[cnt-1]!=0x12345678 && value[cnt]==0x87654321)
                {
                  set_all_params(value);
                  success=1;
                }
                else
                {
                 cfg.delete_value_abs(full_key,key_name);
                 debug("kxapi: presets: invalid preset [%d; %s; %s]\n",ndx,key_name,full_key);
                }
               }

               free(value);
           }
           else
            debug("kxapi: presets: not enough memory [%d]\n",cnt);
       }
       catch(...)
       {
         debug("kxapi: presets: exception when trying to apply preset\n");
       };
    }

    if(ndx>=IKXPLUGIN_PRESETS_BUILTIN)
    {
         // check built-in presets
         try
         {
            const kxparam_t *t=get_plugin_presets();
            
            if(t)
            {
               int num=get_param_count();

               set_all_params((kxparam_t *)&t[(num+1)*(ndx-IKXPLUGIN_PRESETS_BUILTIN)+1]);

               success=1;
            }
         }
         catch(...)
         {
          debug("Incorrect plugin: %d [pointer from get_plugin_presets()] -- apply preset()\n",__LINE__);
         }
    }
    if(success)
     return 0;
    else
     return -1;
}

int iKXPlugin::save_preset(const char *new_name)
{
 kSettings cfg;

 char full_key[127+KX_MAX_STRING];
 sprintf(full_key,"Plugins\\%s",get_plugin_description(IKX_PLUGIN_GUID));

  kxparam_t *data=0;
  int cnt=get_param_count();
  data=(kxparam_t *)malloc(cnt*sizeof(kxparam_t));
  if(data)
  {
   get_all_params(data);

   cfg.write_bin_abs(full_key,new_name,data,cnt*sizeof(kxparam_t));

   free(data);
  }
   else return -1;

  return 0;
}

int iKXPlugin::delete_preset(const char *new_name)
{
  kSettings cfg;

  char full_key[127+KX_MAX_STRING];
  sprintf(full_key,"Plugins\\%s",get_plugin_description(IKX_PLUGIN_GUID));

  cfg.delete_value_abs(full_key,new_name);

  return 0;
}

int iKXPlugin::get_current_preset(char *ret_name)
{
 int found=0;

 kxparam_t *data=0;
 int cnt=get_param_count();
 if(cnt)
    data=(kxparam_t *)malloc(cnt*sizeof(kxparam_t));

 if(data)
 {
    get_all_params(data);

    kSettings cfg;

    char full_key[127+KX_MAX_STRING];
    sprintf(full_key,"Plugins\\%s",get_plugin_description(IKX_PLUGIN_GUID));

    int ndx=0;

       while(1)
       {
        char key_name[KX_MAX_STRING+256]; 
        kxparam_t value[256];

        if(cfg.enum_abs(ndx,full_key,key_name,sizeof(key_name),(TCHAR *)value,sizeof(value))==0)
        {
          if(memcmp(value,data,cnt*sizeof(kxparam_t))==0)
          {
           strncpy(ret_name,key_name,KX_MAX_STRING);
           found=1;
           goto OK;
          }
        }
         else
        {
         break;
        }
        ndx++;
       }

    // second, check built-in presets
    try
    {
       const kxparam_t *t=get_plugin_presets();
       
       if(t)
       {
          int num=get_param_count();

          while(1)
          {
           char *name=(char *)t[0];
           if(name==0)
            break;

           if(memcmp(data,&t[1],num*sizeof(kxparam_t))==0)
           {
            strncpy(ret_name,name,KX_MAX_STRING);
            found=1;
            goto OK;
           }

           t+=(num+1);
          }
       }
    }
    catch(...)
    {
     debug("Incorrect plugin: %d [pointer from get_current_preset()]\n",__LINE__);
    }

OK:
    free(data);
 }
 
 return found?0:-1;
}

#define PRESET_VERSION  0x1

int iKXPlugin::export_presets(const char *file_name)
{
 unlink(file_name);

 int cnt=get_param_count();

    kSettings cfg;
    kSettings exp_file(NULL,file_name);

    char full_key[127+KX_MAX_STRING];
    sprintf(full_key,"Plugins\\%s",get_plugin_description(IKX_PLUGIN_GUID));

    exp_file.write_abs("kXPreset","Size",cnt);
    exp_file.write_abs("kXPreset","Version",PRESET_VERSION);
    exp_file.write_abs("kXPreset","GUID",get_plugin_description(IKX_PLUGIN_GUID));

    int ndx=0;

    while(1)
    {
     char key_name[KX_MAX_STRING+256]; 
     kxparam_t value[256];

     if(cfg.enum_abs(ndx,full_key,key_name,sizeof(key_name),(TCHAR *)value,sizeof(value))==0)
     {
       char key[KX_MAX_STRING+10];
       sprintf(key,"%d.name",ndx);
       exp_file.write_abs(get_plugin_description(IKX_PLUGIN_GUID),key,key_name);
       sprintf(key,"%d.data",ndx);
       exp_file.write_bin_abs(get_plugin_description(IKX_PLUGIN_GUID),key,value,cnt*sizeof(kxparam_t));
     }
      else
     {
      break;
     }
     ndx++;
    }

 return 0;
}

int iKXPlugin::import_presets(const char *file_name)
{
 int ori_cnt=get_param_count();
 dword cnt=0;

    kSettings cfg;
    kSettings exp_file(NULL,file_name);

    exp_file.read_abs("kXPreset","Version",&cnt);
    if(cnt<PRESET_VERSION)
    {
     debug("kxapi: invalid preset version: %d %d\n",cnt,PRESET_VERSION);
     return -3;
    }

    char tmp_guid[KX_MAX_STRING];
    exp_file.read_abs("kXPreset","GUID",tmp_guid,sizeof(tmp_guid));
    if(strncmp(tmp_guid,get_plugin_description(IKX_PLUGIN_GUID),KX_MAX_STRING)!=0)
    {
        debug("kxapi: invalid preset GUID\n");
        return -2;
    }

    exp_file.read_abs("kXPreset","Size",&cnt);
    if((int)cnt!=ori_cnt)
    {
        debug("kxapi: invalid preset size [%d.%d]\n",cnt,ori_cnt);
    return -1;
    }

    int ndx=0;
    char full_key[127+KX_MAX_STRING];
    sprintf(full_key,"Plugins\\%s",get_plugin_description(IKX_PLUGIN_GUID));

    while(1)
    {
     char key[KX_MAX_STRING+256]; 
     char key_name[KX_MAX_STRING+128];
     kxparam_t value[256];

     sprintf(key,"%d.name",ndx);

     if(exp_file.read_abs(get_plugin_description(IKX_PLUGIN_GUID),key,key_name,sizeof(key_name))==0)
     {
       sprintf(key,"%d.data",ndx);
       int sz=sizeof(value);
       if(exp_file.read_bin_abs(get_plugin_description(IKX_PLUGIN_GUID),key,value,&sz)==0)
       {
        if(sz==(int)(cnt*sizeof(kxparam_t)))
         cfg.write_bin_abs(full_key,key_name,value,cnt*sizeof(kxparam_t));
       }
     }
      else
     {
      break;
     }
     ndx++;
    }

 return 0;
}


int iKXPlugin::notify(int event)
{
 if(pm && pgm_id)
  return pm->notify(this,event);
 else
 {
  debug("kxmixer: iKXPlugin::notify: pm is NULL [%x %d]\n",pm,pgm_id);
  return -1;
 }
}

int iKXPlugin::notify_ex(int event,void *p1)
{
 if(pm && pgm_id)
  return pm->notify_ex(this,event,p1);
 else
 {
  debug("kxmixer: iKXPlugin::notify: pm is NULL [%x %d]\n",pm,pgm_id);
  return -1;
 }
}

int iKXPlugin::get_nth_register(int mask,int n) // returns a pointer to info[]
{
            int tmp=0;
            for(dword i=0;i<info_size/sizeof(dsp_register_info);i++)        
            {
                  if(((info[i].type&GPR_MASK)==mask)) 
                  {
                   if(tmp==n)
                   {
                    return i;
                    break;
                   }
                   tmp++;
                  }
                }

                return -1;
}

int iKXPlugin::save_plugin_settings(kSettings &)
{
 return 0;
}

int iKXPlugin::load_plugin_settings(kSettings &)
{
 return 0;
}

#elif defined(__APPLE__)
#warning iKXPlugin and presets API not implemented
#endif
