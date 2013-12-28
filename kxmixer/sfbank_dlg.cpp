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


// inlined

class CSFBankDlg : public kDialog
{
public:
 kButton b_ok;
 kButton b_cancel;
 kStatic label;
 kStatic fname;
 kCombo c_synth;
 CEdit e;

 kString fname_str;

 void set_file(kString tmp) 
 { 
  char name[_MAX_FNAME];
  _splitpath((LPCTSTR)tmp,NULL,NULL,name,NULL);
  if(strlen(name)>22)
  {
   name[21]=0;
   strcat(name,"...");
  }
  fname_str=name;
 };

 int bank;
 int subsynth;

 virtual void init()
 {
  kDialog::init();

  ::set_font(this,font,"sfbank");

  set_tool_font(font);

  char tmp_str[256];

  //SetClassLong(m_hWnd,GCL_STYLE,GetClassLong(m_hWnd,GCL_STYLE)|0x00020000); // CS_DROPSHADOW

  bank=0;
  subsynth=0;

  unsigned int transp_color=0x0;
  unsigned int alpha=90;
  unsigned int cc_method=0;

  set_dragging(gui_get_show_content());

  if(mf.get_profile("sfbank","background",tmp_str,sizeof(tmp_str)))
  {
   tmp_str[0]=0;
  }

  gui_set_background(this,&mf,tmp_str,transp_color,alpha,cc_method,_T("mixer/sfbank_dlg.jpg"));

  SetWindowText(mf.get_profile("lang","sfbank.name"));

  if(!mf.get_profile("sfbank","ok_button",tmp_str,sizeof(tmp_str)))
  {
   gui_create_button(this,&b_ok,IDOK,tmp_str,mf);
   b_ok.show();
  }
  if(!mf.get_profile("sfbank","cancel_button",tmp_str,sizeof(tmp_str)))
  {
   gui_create_button(this,&b_cancel,IDCANCEL,tmp_str,mf);
   b_cancel.show();
  }

  RECT r; memset(&r,0,sizeof(r));

  // label
  if(!mf.get_profile("sfbank","label",tmp_str,sizeof(tmp_str)))
  {
   dword fg=0,bk=0;
   sscanf(tmp_str,"%d %d %d %d %x %x",&r.left,&r.top,&r.right,&r.bottom,&fg,&bk);
   label.create((char *)(LPCTSTR)mf.get_profile("sfbank","label.text"),r.left,r.top,r.right,r.bottom,this,fg,bk);
   label.show();
   label.SetFont(font);
  }

  // file name
  r.left=140; r.top=18; r.right=200; r.bottom=38;
  if(!mf.get_profile("sfbank","fname",tmp_str,sizeof(tmp_str)))
  {
   dword fg=0x0,bk=0xffffff;
   sscanf(tmp_str,"%d %d %d %d %x %x",&r.left,&r.top,&r.right,&r.bottom,&fg,&bk);
   fname.create((char *)(LPCTSTR)fname_str,r.left,r.top,r.right,r.bottom,this,fg,bk);
   fname.show();
   fname.SetFont(font);
  }

  r.left=140; r.top=40; r.right=200; r.bottom=60;
  if(!mf.get_profile("sfbank","input",tmp_str,sizeof(tmp_str)))
  {
   sscanf(tmp_str,"%d %d %d %d",&r.left,&r.top,&r.right,&r.bottom);
  }

  e.Create(0,r,this,1000);
  e.SetWindowText("0");
  e.ShowWindow(SW_SHOW);
  if(font.get_font())
   e.SetFont(font);
//  add_tool((char *)(LPCTSTR)mf.get_profile("lang","sfbank.sfbank"),&e,1000);

  kRect rr;
  rr.left=140; rr.top=40; rr.right=200; rr.bottom=60;
  if(!mf.get_profile("sfbank","synth",tmp_str,sizeof(tmp_str)))
  {
   sscanf(tmp_str,"%d %d %d %d",&rr.left,&rr.top,&rr.right,&rr.bottom);
  }

  c_synth.create(rr,CBS_DROPDOWNLIST /*| CBS_AUTOHSCROLL | WS_VSCROLL*/,2000,this);
  add_tool((char *)(LPCTSTR)mf.get_profile("lang","sfbank.subsynth"),&c_synth,2000);
  if(font.get_font())
   c_synth.set_font(font);

  c_synth.add_string(mf.get_profile("lang","sfbank.both"));
  c_synth.add_string("Synth1");
  c_synth.add_string("Synth2");
  c_synth.set_selection(0);
  c_synth.show();
 };

 void change_bank()
 {
  kString tmp;
  e.GetWindowText(tmp);
  if(tmp.GetLength()==0)
  {
   bank=0;
   return;
  }

  int bnk=0;
  if( (sscanf((LPCTSTR)tmp,"%d",&bnk)!=1) || (bnk<0) || (bnk>250))
  {
    MessageBox(mf.get_profile("lang","sfbank.error"),mf.get_profile("errors","error"));
    e.SetWindowText("0");
    bank=0;
  }
   else bank=bnk;
 };

 afx_msg void OnChange() 
 { 
  switch(c_synth.get_selection())
  {
   case 0: subsynth=0; break;
   case 1: subsynth=1; break;
   case 2: subsynth=2; break;
  }
 };

 DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CSFBankDlg, kDialog)
 ON_CONTROL(EN_UPDATE,1000,change_bank)
 ON_CBN_SELCHANGE(2000, OnChange)
END_MESSAGE_MAP()
