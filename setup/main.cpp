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


#include "stdafx.h"
#include "debug.h"

#define IDABOUT     32771
#define IDINSTALL   32772
#define IDUNINSTALL     32773

int CSetupDialog::on_command(int wp,int lp)
{
 if(LOWORD(wp)==IDABOUT)
 {
  OnAbout();
  return 1;
 }
 if(LOWORD(wp)==IDINSTALL)
 {
  OnSetup();
  return 1;
 }
 if(LOWORD(wp)==IDUNINSTALL)
 {
  OnRemove();
  return 1;
 }
 if(LOWORD(wp)==IDCANCEL)
 {
  PostQuitMessage(0);
  return 1;
 }
 return kDialog::on_command(wp,lp);
}

CSetupApp theApp;

kFile mf;
TCHAR tmp_str[1024];


void CLicenseDialog::init()
{
 kDialog::init();

 set_background(mf.load_image(_T("setup/infodlg.jpg")),0xf7f7f7,90,kMETHOD_BLEND);
 size_to_content();

 kRect r;
 r.left=355; r.top=60; r.right=r.left+27; r.bottom=r.top+14;
 if(!mf.get_profile(_T("installer"),_T("license.agree"),tmp_str,sizeof(tmp_str)))
 {
   _stscanf(tmp_str,_T("%d %d %d %d"),&r.left,&r.top,&r.right,&r.bottom);
 }

 b_ok.create((LPCTSTR)mf.get_profile(_T("setup"),_T("agree")),r,this,IDOK,kMETHOD_TRANS);
 b_ok.set_bitmaps(mf.load_image(_T("setup/buttons/ok.bmp")),
    mf.load_image(_T("setup/buttons/ok_o.bmp")),NULL
        /*,mf.load_image(_T("setup/buttons/ok_s.bmp"))*/);
 b_ok.size_to_content();
 b_ok.show();

 r.left=325; r.top=60; r.right=r.left+27; r.bottom=r.top+14;
 if(!mf.get_profile(_T("installer"),_T("license.cancel"),tmp_str,sizeof(tmp_str)))
 {
   _stscanf(tmp_str,_T("%d %d %d %d"),&r.left,&r.top,&r.right,&r.bottom);
 }

 b_cancel.create((LPCTSTR)mf.get_profile(_T("setup"),_T("cancel")),r,this,IDCANCEL,kMETHOD_TRANS);
 b_cancel.set_bitmaps(mf.load_image(_T("setup/buttons/cancel.bmp")),
   mf.load_image(_T("setup/buttons/cancel_o.bmp")),NULL
   /*,mf.load_image(_T("setup/buttons/cancel_s.bmp"))*/);
 b_cancel.size_to_content();
 b_cancel.show();

 get_rect_abs(r);
 ScreenToClient(r);

 kRect r1;
 if(!mf.get_profile(_T("installer"),_T("listbox"),tmp_str,sizeof(tmp_str)))
 {
   _stscanf(tmp_str,_T("%d %d %d %d"),&r1.left,&r1.top,&r1.right,&r1.bottom);
   r.left+=r1.left;
   r.top+=r1.top;
   r.right-=r1.right;
   r.bottom-=r1.bottom;
 }
 else
 {
   r.left+=5;
   r.top+=5;
   r.right-=5;
   r.bottom-=37;
 }

 kString s;

 char *tmp=
 #include "license.h"
 ;

 s.Format(A2W(tmp));
 
 b_license.create(r,LBS_NOINTEGRALHEIGHT | WS_VSCROLL | WS_BORDER | LBS_NOSEL,1234,this);
 TCHAR *p=(TCHAR *)(LPCTSTR)s;
 while(p && *p)
 {
  TCHAR *d=_tcsstr(p,_T("\n"));
  if(d)
  {
   *d=0;
   b_license.add_string(p);
   p=d+1;
  }
  else
  {
   b_license.add_string(p);
   break;
  }
 }
 b_license.ShowWindow(SW_SHOW);
 b_license.RedrawWindow();
}

void CSetupDialog::init()
{
 kDialog::init();

 HICON m_hIcon = AfxGetApp()->LoadIcon(IDR_SMALL_ICON);
 SetIcon(m_hIcon, TRUE);            // Set big icon
 SetIcon(m_hIcon, FALSE);       // Set small icon

 set_background(mf.load_image(_T("setup/background.jpg")),0xf7f7f7,90,kMETHOD_COPY);
 size_to_content();

 SetWindowText(_T("kX Setup"));
 set_dragging(gui_get_show_content());

 kRect win_rect;
 get_rect_abs(win_rect);

 kRect r;

 r.left=473;
 r.top=180;
 r.right=r.left+65;
 r.bottom=r.top+28;
 if(!mf.get_profile(_T("installer"),_T("install"),tmp_str,sizeof(tmp_str)))
 {
   _stscanf(tmp_str,_T("%d %d %d %d"),&r.left,&r.top,&r.right,&r.bottom);
 }


 b_setup.create((LPCTSTR)mf.get_profile(_T("setup"),_T("setup")),0,r,this,IDINSTALL,kMETHOD_COPY);
 b_setup.set_bitmaps(mf.load_image(_T("setup/buttons/install.bmp")),
   mf.load_image(_T("setup/buttons/install_o.bmp")),NULL
   /*,mf.load_image(_T("setup/buttons/install_s.bmp"))*/);
 b_setup.size_to_content();
 b_setup.show();

 r.left=473;
 r.top=208;
 r.right=r.left+65;
 r.bottom=r.top+28;
 if(!mf.get_profile(_T("installer"),_T("about"),tmp_str,sizeof(tmp_str)))
 {
   _stscanf(tmp_str,_T("%d %d %d %d"),&r.left,&r.top,&r.right,&r.bottom);
 }


 b_about.create((LPCTSTR)mf.get_profile(_T("setup"),_T("about")),0,r,this,IDABOUT,kMETHOD_COPY);
 b_about.set_bitmaps(mf.load_image(_T("setup/buttons/info.bmp")),
    mf.load_image(_T("setup/buttons/info_o.bmp")),NULL
    /*,mf.load_image(_T("setup/buttons/info_s.bmp"))*/);
 b_about.size_to_content();
 b_about.show();

 r.left=473;
 r.top=236;
 r.right=r.left+65;
 r.bottom=r.top+28;
 if(!mf.get_profile(_T("installer"),_T("quit"),tmp_str,sizeof(tmp_str)))
 {
   _stscanf(tmp_str,_T("%d %d %d %d"),&r.left,&r.top,&r.right,&r.bottom);
 }

 b_quit.create((LPCTSTR)mf.get_profile(_T("setup"),_T("quit")),0,r,this,IDCANCEL,kMETHOD_COPY);
 b_quit.set_bitmaps(mf.load_image(_T("setup/buttons/quit.bmp")),
   mf.load_image(_T("setup/buttons/quit_o.bmp")),NULL
   /*,mf.load_image(_T("setup/buttons/quit_s.bmp"))*/);
 b_quit.size_to_content();
 b_quit.show();

 r.left=473;
 r.top=269;
 r.right=r.left+65;
 r.bottom=r.top+28;
 if(!mf.get_profile(_T("installer"),_T("uninstall"),tmp_str,sizeof(tmp_str)))
 {
   _stscanf(tmp_str,_T("%d %d %d %d"),&r.left,&r.top,&r.right,&r.bottom);
 }

 b_remove.create((LPCTSTR)mf.get_profile(_T("setup"),_T("uninstall2")),0,r,this,IDUNINSTALL,kMETHOD_COPY);
 b_remove.set_bitmaps(mf.load_image(_T("setup/buttons/uninstall.bmp")),
   mf.load_image(_T("setup/buttons/uninstall_o.bmp")),NULL
   /*,mf.load_image(_T("setup/buttons/uninstall_s.bmp"))*/);
 b_remove.size_to_content();
 b_remove.show();
}

void CAboutDialog::init()
{
 kDialog::init();

 set_background(mf.load_image(_T("setup/infodlg.jpg")),0xf7f7f7,90,kMETHOD_BLEND);
 size_to_content();

 kRect r;
 r.left=355; r.top=60; r.right=r.left+27; r.bottom=r.top+14;
 if(!mf.get_profile(_T("installer"),_T("info"),tmp_str,sizeof(tmp_str)))
 {
   _stscanf(tmp_str,_T("%d %d %d %d"),&r.left,&r.top,&r.right,&r.bottom);
 }

 b_ok.create(_T("Ok"),0,r,this,IDOK,kMETHOD_TRANS);
 b_ok.set_bitmaps(mf.load_image(_T("setup/buttons/ok.bmp")),
  mf.load_image(_T("setup/buttons/ok_o.bmp")),NULL
  /*,mf.load_image(_T("setup/buttons/ok_s.bmp"))*/);
 b_ok.size_to_content();
 b_ok.show();

 get_rect_abs(r);
 ScreenToClient(r);

 kRect r1;
 if(!mf.get_profile(_T("installer"),_T("copyright"),tmp_str,sizeof(tmp_str)))
 {
   _stscanf(tmp_str,_T("%d %d %d %d"),&r1.left,&r1.top,&r1.right,&r1.bottom);
   r.left+=r1.left;
   r.top+=r1.top;
   r.right-=r1.right;
   r.bottom-=r1.bottom;
 }
 else
 {
  r.left+=5;
  r.top+=5;
  r.right-=5;
  r.bottom-=37;
 }

 kString s;
 s.Format(_T("kX Audio Driver Setup (%s)\n%s\nAll Rights Reserved"),KX_DRIVER_VERSION_STR,KX_COPYRIGHT_STR_R);
 
 info.create((LPCTSTR)s,SS_CENTER|SS_SUNKEN,r,this,0);
 info.show();
}

int check_requirements(void)
{
 debug(_T("kxsetup: checking requirements...\n"));
 HMODULE m=LoadLibrary(_T("dsound.dll"));
 if(m==0)
 {
  MessageBox(NULL,(LPCTSTR)mf.get_profile(_T("errors"),_T("setup5")),_T("kX Setup"),MB_OK|MB_ICONSTOP);
  return -1;
 }
 if(GetProcAddress(m,"DirectSoundCreate8")==0)
 {
  MessageBox(NULL,(LPCTSTR)mf.get_profile(_T("errors"),_T("setup5")),_T("kX Setup"),MB_OK|MB_ICONSTOP);
  FreeLibrary(m);
  return -2;
 }
 FreeLibrary(m);

 return 0;
}

void CSetupDialog::OnSetup()
{
 CWnd *w=GetDlgItem(IDINSTALL);
 if(w)
  w->ShowWindow(SW_HIDE);

 w=GetDlgItem(IDUNINSTALL);
 if(w)
  w->ShowWindow(SW_HIDE);

 w=GetDlgItem(IDABOUT);
 if(w)
  w->ShowWindow(SW_HIDE);

 w=GetDlgItem(IDCANCEL);
 if(w)
  w->ShowWindow(SW_HIDE);

 EnableWindow(FALSE);

 if(!check_requirements())
 {
   debug(_T("kxsetup: displaying 'license' dialog\n"));

   CLicenseDialog dlg;
   int ret=dlg.do_modal(this);

   if(ret==1)
   {
     SetCursor(LoadCursor(NULL,IDC_WAIT));

     kx_setup(&dlg,0);
   }
   else
   {
    debug(_T("kxsetup: license not accepted\n"));
    MessageBox((LPCTSTR)mf.get_profile(_T("setup"),_T("setup6")),_T("kX Setup"),MB_OK);
   }
 }
 EnableWindow();

 SendMessage(WM_COMMAND,IDCANCEL);
}

void CSetupDialog::OnRemove()
{
 if(::MessageBox(NULL,(LPCTSTR)mf.get_profile(_T("setup"),_T("setup5")),_T("kX Setup"),MB_YESNO)!=IDYES)
    return;

 CWnd *w=GetDlgItem(IDINSTALL);
 if(w)
  w->ShowWindow(SW_HIDE);

 w=GetDlgItem(IDUNINSTALL);
 if(w)
  w->ShowWindow(SW_HIDE);

 w=GetDlgItem(IDABOUT);
 if(w)
  w->ShowWindow(SW_HIDE);

 w=GetDlgItem(IDCANCEL);
 if(w)
  w->ShowWindow(SW_HIDE);

 EnableWindow(FALSE);

 SetCursor(LoadCursor(NULL,IDC_WAIT));
 kx_setup(NULL,1);

 EnableWindow();
 
 ::MessageBox(NULL,(LPCTSTR)mf.get_profile(_T("setup"),_T("uninstall")),_T("kX Setup"),MB_OK|MB_ICONINFORMATION);

 SendMessage(WM_COMMAND,IDCANCEL);
}

void CSetupDialog::OnAbout()
{
 CAboutDialog d;
 d.do_modal(this);
}

BOOL CSetupApp::InitInstance()
{
        int ret=mf.init();

        if(_tcsstr(GetCommandLine(),_T("--asio"))!=NULL)
        {
            // MessageBox(NULL,(LPCTSTR)mf.get_profile(_T("setup"),_T("setup5")),_T("kX Setup"),MB_YESNO);
            debug(_T("kxsetup: --asio option - update ASIO registry\n"));
            update_asio_drivers();
            return FALSE;
        }

        if(_tcsstr(GetCommandLine(),_T("--clean"))!=NULL)
        {
            // MessageBox(NULL,(LPCTSTR)mf.get_profile(_T("setup"),_T("setup5")),_T("kX Setup"),MB_YESNO);
            debug(_T("kxsetup: --clean option - remove the software & the devices only\n"));
            kx_setup(NULL,1);
            // MessageBox(NULL,(LPCTSTR)mf.get_profile(_T("setup"),_T("uninstall")),_T("kX Setup"),MB_OK|MB_ICONINFORMATION);
            return FALSE;
        }

        if(_tcsstr(GetCommandLine(),_T("--reset"))!=NULL)
        {
            debug(_T("kxsetup: --reset option: reset settings\n"));
            reset_kx_settings();
            return FALSE;
        }

        if(_tcsstr(GetCommandLine(),_T("--install"))!=NULL)
        {
            debug(_T("kxsetup: --install option: install everything\n"));
            kx_setup(NULL,0);
            return FALSE;
        }

        if(ret)
        {
            MessageBox(NULL,_T("Fatal error: file kxskin.kxs not found\nApplication aborted"),_T("Severe error"),MB_OK|MB_ICONSTOP);
            exit(5);
        }
        else
        {
            HICON def_icon=AfxGetApp()->LoadIcon(IDR_SMALL_ICON);
            gui_set_default_icon(def_icon);

            CSetupDialog dlg;
            // m_pMainWnd = &dlg;
            dlg.do_modal(NULL);
        }

        return FALSE;
}
