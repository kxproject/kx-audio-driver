// kX Setup
// Copyright (c) Eugene Gavrilov, 2001-2011.
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


TCHAR *asio_guids[]=
{
 _T("{5C0B12B1-F582-4935-BFAE-85267A6BE13D}"),
 _T("{5771C802-034B-4ac1-BE54-36DADD447207}"),
 _T("{8A1080D5-A695-498a-8188-2F0B57870A7F}"),
 _T("{6732CDC3-569B-4541-9060-D7290050E7A3}"),
 _T("{FEDE17F2-7108-4c64-9801-1627B75ACFC9}"),
 _T("{2B156F48-B8EF-4a1e-95DF-E384E8B64842}"),
 _T("{2D98497D-A875-481d-A3D3-7A159642101F}"),
 _T("{66649E8B-9864-43ee-AE93-558E90D58BB3}"),
 NULL
};

#define MAX_ASIO_KEY    256

void get_current_asio_driver(TCHAR *key_name,BYTE cur_descr[MAX_ASIO_KEY],BYTE cur_clsid[MAX_ASIO_KEY],dword flag)
{
    HKEY hKey=0;
    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,key_name,0,KEY_QUERY_VALUE|flag,&hKey)==ERROR_SUCCESS)
    {
           BYTE data[MAX_ASIO_KEY];
           DWORD size=sizeof(data);
           DWORD type=0;

           memset(data,0,sizeof(data));
           if(RegQueryValueEx(hKey,(LPCTSTR)_T("Description"),0,&type,data,&size)==ERROR_SUCCESS)
            strncpy((TCHAR *)cur_descr,(const TCHAR *)data,MAX_ASIO_KEY);

           memset(data,0,sizeof(data));
           type=0;
           size=sizeof(data);

           if(RegQueryValueEx(hKey,(LPCTSTR)_T("CLSID"),0,&type,data,&size)==ERROR_SUCCESS)
            strncpy((TCHAR *)cur_clsid,(const TCHAR *)data,MAX_ASIO_KEY);

           RegCloseKey(hKey);
    }
}

int set_current_asio_driver(TCHAR *key,BYTE *new_descr,BYTE *new_clsid,dword flag)
{
    HKEY hKey=0;
    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,key,0,KEY_ALL_ACCESS|flag,&hKey)==ERROR_SUCCESS)
    {
         RegSetValueEx(hKey,(LPCTSTR)_T("Description"),0,REG_SZ,(const BYTE *)new_descr,(DWORD)_tcslen((const TCHAR *)new_descr));
         RegSetValueEx(hKey,(LPCTSTR)_T("CLSID"),0,REG_SZ,(const BYTE *)new_clsid,(DWORD)_tcslen((const TCHAR *)new_clsid));
         RegCloseKey(hKey);

         return 0;
    }
    else
     return -1;
}

int update_asio_drivers(void) // returns 0 if OK, -1 if kxsetup needs to re-launch to update the settings
{
 for(int i=0;i<8;i++)
 {
  TCHAR key_name[MAX_ASIO_KEY];
  BYTE cur_descr[MAX_ASIO_KEY];
  BYTE cur_clsid[MAX_ASIO_KEY];

  if(i==0)
   _tcscpy(key_name,_T("SOFTWARE\\ASIO\\kX ASIO Driver"));
  else
   _stprintf(key_name,_T("SOFTWARE\\ASIO\\kX ASIO Driver %d"),i);

  iKX *ikx_t=iKX::create(i);
  dword flag=0;

#if defined(_WIN64)
AGAIN:
#endif
  memset(cur_descr,0,sizeof(cur_descr));
  memset(cur_clsid,0,sizeof(cur_clsid));
  get_current_asio_driver(key_name,cur_descr,cur_clsid,flag);

  if(ikx_t)
  {
   const TCHAR *name=ikx_t->get_device_name();

   if(name)
   {
        TCHAR new_descr[MAX_ASIO_KEY];

        _stprintf(new_descr,_T("kX ASIO %s"),name);

        // compare current and previous
        if(_tcscmp((const TCHAR *)cur_descr,(const TCHAR *)new_descr)!=NULL || _tcscmp((const TCHAR *)cur_clsid,(const TCHAR *)asio_guids[i])!=NULL) // differ?
        {
          if(set_current_asio_driver(key_name,(BYTE *)new_descr,(BYTE *)asio_guids[i],flag)==-1) // error? need to relaunch with elevated priv.
          {
            debug("!! kxsetup: ASIO registry could not be written, need to re-launch with elevated priveleges\n");
            ikx_t->destroy();
            return -1;
          }
          debug("kxsetup: ASIO driver %d set to %s\n",i,new_descr);
        }
        else
         debug("kxsetup: ASIO driver %d unchanged (%s)\n",i,new_descr);

#if defined(_WIN64)
        if(flag==0)
        {
             flag=KEY_WOW64_32KEY;
             goto AGAIN;
        }
#endif
   }

   ikx_t->destroy();
  }
  else  // no device, remove registry items
  {
        if(cur_descr[0]!=0 || cur_clsid[0]!=0) // non-empty? need to remove
        {
            HKEY hKey;
            if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,key_name,0,KEY_ALL_ACCESS|flag,&hKey)==ERROR_SUCCESS)
            {
                RegDeleteValue(hKey,(LPCTSTR)_T("Description"));
                RegDeleteValue(hKey,(LPCTSTR)_T("CLSID"));

                RegCloseKey(hKey);
            }
            else
            {
                debug("!! kxsetup: ASIO registry could not be written, need to re-launch with elevated priveleges\n");
                return -1;
            }

            debug("kxsetup: ASIO driver %d set to (none)\n",i);
        }
        else
            debug("kxsetup: ASIO driver %d unchanged (none)\n",i);

#if defined(_WIN64)
        if(flag==0)
        {
             flag=KEY_WOW64_32KEY;
             goto AGAIN;
        }
#endif
  }
 } // for

 return 0;
}
