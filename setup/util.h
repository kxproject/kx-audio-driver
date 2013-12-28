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


int prepare_reboot(kWindow *w)
{
 debug(_T("kxsetup: prepare_reboot()\n"));

 if(MessageBox(w?w->get_wnd():NULL,(LPCTSTR)mf.get_profile(_T("setup"),_T("setup3")),_T("kX Setup"),MB_OKCANCEL|MB_ICONINFORMATION)==IDOK)
 {

  if(_tcsstr(GetCommandLine(),_T("--noreboot")))
  {
   debug(_T("kxsetup: reboot suspeneded by --noreboot option\n"));
   return 0;
  }

  debug(_T("kxsetup: rebooting\n"));
  if(ExitWindowsEx(EWX_REBOOT,0)==0)
  {
     HANDLE hToken;  
     TOKEN_PRIVILEGES tkp;  
      
     // Get a token for this process. 
      
     if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
     {
        MessageBox(w?w->get_wnd():NULL,(LPCTSTR)mf.get_profile(_T("setup"),_T("setup4")),_T("kX Setup"),MB_OK|MB_ICONINFORMATION);
     }
     else
     {
          // Get the LUID for the shutdown privilege. 
          LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, 
                  &tkp.Privileges[0].Luid); 
           
          tkp.PrivilegeCount = 1;  // one privilege to set    
          tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
           
          // Get the shutdown privilege for this process. 
          AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0); 
           
          // Cannot test the return value of AdjustTokenPrivileges. 
           
          if (GetLastError() != ERROR_SUCCESS) 
          {
            MessageBox(w?w->get_wnd():NULL,(LPCTSTR)mf.get_profile(_T("setup"),_T("setup4")),_T("kX Setup"),MB_OK|MB_ICONINFORMATION);
          }
          else
          {
            // Shut down the system and force all applications to close. 
           
            if (!ExitWindowsEx(EWX_REBOOT, 0)) 
            {
              MessageBox(w?w->get_wnd():NULL,(LPCTSTR)mf.get_profile(_T("setup"),_T("setup4")),_T("kX Setup"),MB_OK|MB_ICONINFORMATION);
            }
          }
      }
  }
 }
 return 0;
}

#include "../driver/frname.cpp"
TCHAR tmp_buff[256];
TCHAR *get_friendly_name(const TCHAR *t)
{
 if(_tcsstr(t,_T("VEN_1102&DEV_7002"))!=0)
  return _T("Joystick");
 if(_tcsstr(t,_T("VEN_1102&DEV_7003"))!=0)
  return _T("Joystick");

 _tcsncpy(tmp_buff,t,256);
 TCHAR *p;
 
 p=_tcsstr(tmp_buff,"SUBSYS_");
 unsigned int subsys=0;
 if(p)
 {
  p+=7;
  _stscanf(p,"%x",&subsys);
 }
 p=_tcsstr(tmp_buff,"DEV_");
 unsigned int devid=0;
 if(p)
 {
  p+=4;
  _stscanf(p,"%x",&devid);
 }
 p=_tcsstr(tmp_buff,"VEN_");
 unsigned int vendor=0;
 if(p)
 {
  p+=4;
  _stscanf(p,"%x",&vendor);
 }
 p=_tcsstr(tmp_buff,"REV_");
 unsigned int chiprev=0;
 if(p)
 {
  p+=4;
  _stscanf(p,"%x",&chiprev);
 }

 devid=(devid<<16)|vendor;


  unsigned char tmp[12];
  char tmp2[8];
  kx_get_friendly_name(devid,subsys,chiprev,tmp_buff,
     tmp[0],tmp[1],tmp[2],tmp[3],tmp[4],tmp[5],tmp[6],tmp[7],tmp[8],tmp2[0],tmp2[1],tmp[8],tmp[9]);

 debug("found device [%08x %08x %08x [%s] -> '%s']\n",devid,subsys,chiprev,t,tmp_buff);

  return tmp_buff;
}

#include "interface/kxmixer.h"

volatile static int kx_found=-1;
static BOOL CALLBACK MyEnumWindowsProc(HWND hwnd,LPARAM)
{
    TCHAR name[1024];
    GetWindowText(hwnd,name,1024);
    if(_tcscmp(name,_T(KX_DEF_WINDOW))==NULL)
    {
     if(kx_found!=-1)
     {
      SendMessage(hwnd,WM_CLOSE,0,0);
      kx_found++;
     }
    } 
    return TRUE;
}

int kx_close()
{
    kx_found=0;

    EnumWindows(MyEnumWindowsProc,0); // look for KX Windows

    if(kx_found!=0)
    {
        debug(_T("kxsetup: kx mixer found\n"));
        // found mixer: close it once more...
        kx_found=0;
        EnumWindows(MyEnumWindowsProc,1);

        Sleep(2000);
        kx_found=0;

        EnumWindows(MyEnumWindowsProc,0);
        if(kx_found!=0)
        {
            MessageBox(NULL,(LPCTSTR)mf.get_profile(_T("errors"),_T("setup8")),_T("kX Setup"),MB_OK|MB_ICONSTOP);
            return 1;
        }
    }

    return 0;
}

void create_reg(HKEY where,const char *path,char *item,char *value)
{
  HKEY hKey;
  if(RegOpenKeyEx(where,path,NULL,KEY_ALL_ACCESS,&hKey)!=ERROR_SUCCESS)
  {
    DWORD r;
    RegCreateKeyEx(where,path,NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,&r);
  }
  if(hKey)
  {
    RegSetValueEx(hKey,item,NULL,REG_SZ,(unsigned char *)value,(DWORD)(strlen(value)+1));
    RegCloseKey(hKey);
  }
}
