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

kFile mf;
kFile mf2;

void init_skin(void)
{
	kSettings g_cfg;

        char tmp_skin[2048];
        int len=sizeof(tmp_skin);

        if(g_cfg.read_abs("Mixer","DefaultSkin",tmp_skin,len)==0)
        {
                  if(strcmp(mf.get_skin_file(),tmp_skin)!=NULL) // only if saved!=default
                  {
                       if(mf2.set_skin(tmp_skin))
                       {
                        MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","skin2"),"kX Mixer Error",MB_OK|MB_ICONINFORMATION);
                        g_cfg.delete_value_abs("Mixer","DefaultSkin");
                        mf2.close();
                       }
                       else
                       {
                        mf.attach_skin(&mf2);
                       }
                  }
        }

        // restore current language
        dword value=GetUserDefaultLangID();
	if(g_cfg.read_abs("Mixer","Language",&value)==0)
                 mf.set_language((LANGID)value);
}

int set_skin(int id)
{
	kSettings cfg;

        char key_name[256]; int keyname_size=sizeof(key_name);
        char tmp_skin[2048]; int value_size=sizeof(tmp_skin);

        if(cfg.enum_abs(id,"Skins",key_name,keyname_size,tmp_skin,value_size)==0)
        {
         char *tmp=strstr(key_name,".name");
         if(tmp)
         {
         	*tmp=0; // keyname: "GUID"
         	value_size=sizeof(tmp_skin);
                if(cfg.read_abs("Skins",key_name,tmp_skin,value_size)==0)
                {
                    manager->close_all_windows();
                    if(strcmp(mf.get_skin_file(),tmp_skin)!=0) // only if !=default
                    {
                       // value: path
                       mf2.set_language(mf.get_language());
                       if(mf2.set_skin(tmp_skin)==0)
                       {
                        mf.attach_skin(&mf2);
                       }
                       else
                       {
                        mf.detach_skin();
                        mf2.close();
                        MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","skin2"),"kX Mixer Error",MB_OK|MB_ICONINFORMATION);
                        cfg.delete_value_abs("Skins",key_name);
                        strcat(key_name,".name");
                        cfg.delete_value_abs("Skins",key_name);
                       }
                    }
                     else 
                    {
                     mf.detach_skin();
                     mf2.close();
                    }
                } else MessageBox(NULL,"Internal error #3000","Error",MB_OK);
         } else MessageBox(NULL,"Internal error #3001","Error",MB_OK);
        } else MessageBox(NULL,"Internal error #3002","Error",MB_OK);
	
  // save selection
  cfg.write_abs("Mixer","DefaultSkin",(TCHAR *)(LPCTSTR)mf.get_skin_file());

  return 0;
}

int add_skin(const char *fname,int force)
{
  int ret=0;

  manager->close_all_windows();

  kSettings cfg;

  if((strcmp(mf.get_skin_file(),fname)!=0) || force)
  {
     if(mf2.set_skin(fname)==0)
     {
      mf.attach_skin(&mf2);

      kString guid=mf.get_skin_guid();
      cfg.write_abs("Skins",(LPCTSTR)guid,(TCHAR *)fname);
      guid+=".name";
      cfg.write_abs("Skins",(LPCTSTR)guid,(TCHAR *)(LPCTSTR)mf.get_skin_name());

      // save selection
      kSettings cfg;
      cfg.write_abs("Mixer","DefaultSkin",(TCHAR *)(LPCTSTR)mf.get_skin_file());
     }
      else 
     {
      ret=-1;
      mf.detach_skin();
      mf2.close();
     }
  }
  else
  {
      mf.detach_skin();
      mf2.close();
  }

  return ret;
}
