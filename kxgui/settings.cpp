// kX XML Settings Library
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

void kSettings::translate_section(const TCHAR *section,kString *ret)
{
 if(mode==kSETTINGS_INI)
 {
  (*ret)=section;
  (*ret)+=_T(" - ");
  TCHAR *p=card_name;
  while(*p)
  {
   if(*p=='[') *p='(';
   if(*p==']') *p=')';
   p++;
  }
  (*ret)+=card_name;
 }
 if(mode==kSETTINGS_REGISTRY)
 {
  (*ret)=_T("Config\\");
  (*ret)+=card_name;
  (*ret)+=_T("\\");
  (*ret)+=section;
 }
}

void kSettings::write(const TCHAR *section_,TCHAR *key,dword value,int i)
{
 kString tmp_section;
 if(flag&KX_SAVED_NO_CARDNAME)
  tmp_section=section_;
 else
  translate_section(section_,&tmp_section);

 write_abs((LPCTSTR)tmp_section,key,value,i);
}

void kSettings::write(const TCHAR *section_,const TCHAR *key,TCHAR *value)
{
 kString tmp_section;
 if(flag&KX_SAVED_NO_CARDNAME)
  tmp_section=section_;
 else
  translate_section(section_,&tmp_section);

 write_abs((LPCTSTR)tmp_section,key,value);
}

int kSettings::read(const TCHAR *section_,TCHAR *key,dword *value,int i)
{
 kString tmp_section;
 if(flag&KX_SAVED_NO_CARDNAME)
  tmp_section=section_;
 else
  translate_section(section_,&tmp_section);

 return read_abs((LPCTSTR)tmp_section,key,value,i);
}

int kSettings::read(const TCHAR *section_,const TCHAR *key,TCHAR *value,int value_size)
{
 kString tmp_section;
 if(flag&KX_SAVED_NO_CARDNAME)
  tmp_section=section_;
 else
  translate_section(section_,&tmp_section);

 return read_abs((LPCTSTR)tmp_section,key,value,value_size);
}

int kSettings::delete_key(const TCHAR *section_,const TCHAR *key,int complete)
{
 kString tmp_section;
 if(flag&KX_SAVED_NO_CARDNAME)
  tmp_section=section_;
 else
  translate_section(section_,&tmp_section);

 return delete_key_abs((LPCTSTR)tmp_section,key,complete);
}

int kSettings::delete_value(const TCHAR *section_, const TCHAR *value)
{
 kString tmp_section;
 if(flag&KX_SAVED_NO_CARDNAME)
  tmp_section=section_;
 else
  translate_section(section_,&tmp_section);

 return delete_value_abs((LPCTSTR)tmp_section,value);
}


void kSettings::write_abs(const TCHAR *section,const TCHAR *key_,dword value,int i)
{
 TCHAR str2[KX_MAX_STRING];
 if(i)
  _stprintf(str2,_T("%s_%d"),key_,i);
 else
  _tcscpy(str2,key_);

 if(mode==kSETTINGS_INI)
 {
  TCHAR str[32];
  _stprintf(str,_T("0x%x"),value);

  WritePrivateProfileString(section,str2,str,(LPCTSTR)cfg_file);
 }
 if(mode==kSETTINGS_REGISTRY)
 {
  HKEY tt;
  if(RegCreateKeyEx(hkey,section,NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&tt,NULL)==ERROR_SUCCESS)
  {
   RegSetValueEx(tt,str2,NULL,REG_DWORD,(unsigned char *)&value,sizeof(DWORD));
   RegCloseKey(tt);
  }
 }
}

void kSettings::write_abs(const TCHAR *section,const TCHAR *key,const TCHAR *value)
{
 if(mode==kSETTINGS_INI)
   WritePrivateProfileString(section,key,value,(LPCTSTR)cfg_file);

 if(mode==kSETTINGS_REGISTRY)
 {
  HKEY tt;
  if(RegCreateKeyEx(hkey,section,NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&tt,NULL)==ERROR_SUCCESS)
  {
   RegSetValueEx(tt,key,NULL,REG_SZ,(CONST BYTE*)value,(DWORD)(_tcslen(value)+1)*sizeof(TCHAR));
   RegCloseKey(tt);
  }
 }
}

int kSettings::read_abs(const TCHAR *section,const TCHAR *key_,dword *value,int i)
{
 TCHAR str[32];

 TCHAR str2[KX_MAX_STRING];
 if(i)
  _stprintf(str2,_T("%s_%d"),key_,i);
 else
  _tcscpy(str2,key_);

 if(mode==kSETTINGS_INI)
 {
  GetPrivateProfileString((LPCTSTR)section,str2,_T("$none$"),str,sizeof(str),(LPCTSTR)cfg_file);
  if(_tcscmp(str,_T("$none$"))==0)
   return -1;
  _stscanf(str,_T("%x"),value);
  return 0;
 }
 if(mode==kSETTINGS_REGISTRY)
 {
  HKEY tt;
  if(RegOpenKeyEx(hkey,section,NULL,KEY_ALL_ACCESS,&tt)==ERROR_SUCCESS)
  {
   DWORD type=REG_DWORD;
   DWORD data_buffer=sizeof(DWORD);

   int ret=RegQueryValueEx(tt,str2,NULL,&type,(LPBYTE)value,&data_buffer);
   RegCloseKey(tt);
   if(ret==ERROR_SUCCESS)
    return 0;

   return -1;
  }
  return -2;
 }
 return -3;
}

int kSettings::read_abs(const TCHAR *section,const TCHAR *key,TCHAR *value,int max_value_size)
{
 if(mode==kSETTINGS_INI)
 {
  GetPrivateProfileString(section,key,_T("$none$"),value,max_value_size,(LPCTSTR)cfg_file);
  if(_tcscmp(value,_T("$none$"))==NULL)
   return -1;
  return 0;
 }
 if(mode==kSETTINGS_REGISTRY)
 {
  HKEY tt;
  if(RegOpenKeyEx(hkey,section,NULL,KEY_ALL_ACCESS,&tt)==ERROR_SUCCESS)
  {
   DWORD type=REG_SZ;
   DWORD data_buffer=max_value_size;

   int ret=RegQueryValueEx(tt,key,NULL,&type,(LPBYTE)value,&data_buffer);
   RegCloseKey(tt);
   if(ret==ERROR_SUCCESS)
    return 0;

   return -1;
  }
  return -2;
 }
 return -3;
}

void kSettings::write_bin_abs(const TCHAR *section,const TCHAR *key,void *mem,int size)
{
 if(mode==kSETTINGS_INI)
 {
  TCHAR *tmp;
  tmp=(TCHAR *)malloc((size+1)*sizeof(TCHAR)*2);
  if(tmp)
  {
   unsigned char *pointer=(unsigned char *)mem;
   int i;

   for(i=0;i<size;i++)
   {
    tmp[i*2]=((*pointer&0xf0)>>4)+'0';
    tmp[i*2+1]=(*pointer&0xf)+'0';
    pointer++;
   }
   tmp[i*2]=0;

   write_abs(section,key,tmp);
   free(tmp);
  }
 }
 if(mode==kSETTINGS_REGISTRY)
 {
  HKEY tt;
  if(RegCreateKeyEx(hkey,section,NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&tt,NULL)==ERROR_SUCCESS)
  {
   RegSetValueEx(tt,key,NULL,REG_BINARY,(CONST BYTE*)mem,size);
   RegCloseKey(tt);
  }
 }
}

int kSettings::read_bin_abs(const TCHAR *section,const TCHAR *key,void *mem,int *size)
{
 if((size==0)||(mem==0)) return -4;

 if(mode==kSETTINGS_INI)
 {
   TCHAR *tmp=(TCHAR *)malloc((*size+1)*sizeof(TCHAR)*2);
   if(tmp)
   {
    if(read_abs(section,key,tmp,(*size+1)*2)==0)
    {
     unsigned char *pointer=(unsigned char *)mem;
     for(int i=0;i<*size;i++)
     {
      if(tmp[i*2]==0)
      { *size=i; break; }
      *pointer=(((tmp[i*2]-'0')&0xf)<<4)+((tmp[i*2+1]-'0')&0xf);
      pointer++;
     }
     free(tmp);
     return 0;
    }
    free(tmp);
    return -3;
   }
   return -2;
 }
 if(mode==kSETTINGS_REGISTRY)
 {
  HKEY tt;
  if(RegOpenKeyEx(hkey,section,NULL,KEY_ALL_ACCESS,&tt)==ERROR_SUCCESS)
  {
   DWORD type=REG_BINARY;
   DWORD data_buffer=*size;

   int ret=RegQueryValueEx(tt,key,NULL,&type,(LPBYTE)mem,&data_buffer);
   RegCloseKey(tt);
   if(ret==ERROR_SUCCESS)
    return 0;

   return -1;
  }
  return -2;
 }
 return -3;
}

int kSettings::enum_abs(int ndx,const TCHAR *section,TCHAR *key,int max_keyname_size,TCHAR *value,int max_value_size)
{
 if(mode==kSETTINGS_REGISTRY)
 {
  HKEY tt;
  if(RegOpenKeyEx(hkey,section,NULL,KEY_ALL_ACCESS,&tt)==ERROR_SUCCESS)
  {
   int ret;
   DWORD val_name_size=max_keyname_size;
   DWORD val_size=max_value_size;
   DWORD type=0;
   
   ret=RegEnumValue(tt,ndx,key,&val_name_size,NULL,&type,(LPBYTE)value,&val_size);
   RegCloseKey(tt);
   if(ret==ERROR_SUCCESS)
    return 0;

   return -1;
  }
  return -3;
 }
 return -4;
}

int kSettings::delete_key_abs(const TCHAR *section,const TCHAR *key,int complete)
{
 if(mode==kSETTINGS_REGISTRY)
 {
  HKEY tt;
  if(RegOpenKeyEx(hkey,section,NULL,KEY_ALL_ACCESS,&tt)==ERROR_SUCCESS)
  {
     if(complete==0)
     {
        int ret=0;
        if(RegDeleteKey(tt,key)!=ERROR_SUCCESS)
         ret=-1;
        RegCloseKey(tt);
        return ret;
     }
     else
     {
        SHDeleteKey(tt,key);
        RegCloseKey(tt);
        return 0;
     }
  }
  return -2;
 }
 return -4;
}

int kSettings::delete_value_abs(const TCHAR *section, const TCHAR *value)
{
 if(mode==kSETTINGS_REGISTRY)
 {
  HKEY tt;
  if(RegOpenKeyEx(hkey,section,NULL,KEY_ALL_ACCESS,&tt)==ERROR_SUCCESS)
  {
        int ret=0;
        if(RegDeleteValue(tt,value)!=ERROR_SUCCESS)
         ret=-1;
        RegCloseKey(tt);
        return ret;
  }
  return -2;
 }
 return -4;
}

kSettings::kSettings(const TCHAR *card_name_,const TCHAR *cfg_file_,unsigned int flag_)
{
 mode=0;
 hkey=0;

 if(card_name_)
  _tcsncpy(card_name,card_name_,sizeof(card_name));
 else
  card_name[0]=0;

 flag=flag_;
 if(cfg_file_==0)
 {
  if(RegCreateKeyEx(HKEY_CURRENT_USER,_T("Software\\kX"),NULL,NULL,REG_OPTION_NON_VOLATILE,
   KEY_ALL_ACCESS,NULL,&hkey,NULL)==ERROR_SUCCESS)
  {
   mode=kSETTINGS_REGISTRY;
  }
  else
  {
   hkey=0;
   TCHAR tmp[MAX_PATH]; GetSystemDirectory(tmp,sizeof(tmp));
   cfg_file=tmp;
   cfg_file+=_T("\\kxdefault.kx");
   mode=kSETTINGS_INI;
  }
 }
 else
 {
  cfg_file=cfg_file_;
  hkey=0;
  mode=kSETTINGS_INI;
 }
}

kSettings::~kSettings()
{
 if(hkey)
 {
  RegCloseKey(hkey);
  hkey=0;
 }
 mode=0;
}
