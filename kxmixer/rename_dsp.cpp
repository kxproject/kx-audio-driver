// kX Mixer
// Copyright (c) Eugene Gavrilov, 2001-2005.
// All rights reserved

// inlined

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


class CRenameDlg : public kDialog
{
public:
 kButton b_ok;
 kButton b_cancel;
 #define RENAME_EDIT	0x1000
 CEdit b_new_name;
 kStatic b_ori_name;
 kStatic label;

 char new_name[KX_MAX_STRING];

 const char *get_class_name() { return "kXRename"; };

 virtual void init()
 {
  kDialog::init();

  ::set_font(this,font,"rename");
  set_tool_font(font);

  char tmp_str[256];

  //SetClassLong(m_hWnd,GCL_STYLE,GetClassLong(m_hWnd,GCL_STYLE)|0x00020000); // CS_DROPSHADOW

  unsigned int transp_color=0x0;
  unsigned int alpha=90;
  unsigned int cc_method=0;

  set_dragging(gui_get_show_content());

  if(mf.get_profile("rename","background",tmp_str,sizeof(tmp_str)))
  {
   tmp_str[0]=0;
  }

  gui_set_background(this,&mf,tmp_str,transp_color,alpha,cc_method,_T("dsp/rename_dlg.jpg"));

  SetWindowText(mf.get_profile("lang","rename.name"));

  if(!mf.get_profile("rename","ok_button",tmp_str,sizeof(tmp_str)))
  {
   gui_create_button(this,&b_ok,IDOK,tmp_str,mf);
   b_ok.ShowWindow(SW_SHOW);
  }
  if(!mf.get_profile("rename","cancel_button",tmp_str,sizeof(tmp_str)))
  {
   gui_create_button(this,&b_cancel,IDCANCEL,tmp_str,mf);
   b_cancel.ShowWindow(SW_SHOW);
  }

  RECT r; 

  // label
  if(!mf.get_profile("rename","label",tmp_str,sizeof(tmp_str)))
  {
   dword fg=0,bk=0;
   RECT r; memset(&r,0,sizeof(r));

   sscanf(tmp_str,"%d %d %d %d %x %x",&r.left,&r.top,&r.right,&r.bottom,&fg,&bk);
   label.create((char *)(LPCTSTR)mf.get_profile("rename","label.text"),r.left,r.top,r.right,r.bottom,this,fg,bk);
   label.ShowWindow(SW_SHOW);
  }
 
  r.left=10; r.top=10; r.right=200; r.bottom=20;
  COLORREF fg=0xffffff;
  COLORREF bk=0xb0b000;

  if(!mf.get_profile("rename","ori_name",tmp_str,sizeof(tmp_str)))
  {
   sscanf(tmp_str,"%d %d %d %d %x %x",&r.left,&r.top,&r.right,&r.bottom,&fg,&bk);
   b_ori_name.create(new_name,r.left,r.top,r.right,r.bottom,this,fg,bk);
   b_ori_name.ShowWindow(SW_SHOW);
  }

  r.left=10; r.top=40; r.right=200; r.bottom=20; 
  if(!mf.get_profile("rename","edit",tmp_str,sizeof(tmp_str)))
  {
   sscanf(tmp_str,"%d %d %d %d",&r.left,&r.top,&r.right,&r.bottom);
  }

  r.right+=r.left;
  r.bottom+=r.top;

  b_new_name.Create(/*ES_LOWERCASE*/0,r,this,RENAME_EDIT);
  b_new_name.SetWindowText(new_name);
  b_new_name.ShowWindow(SW_SHOW);
  b_new_name.SetFont(font);

 };

 void editor_change(void)
 {
  kString tmp;
  b_new_name.GetWindowText(tmp);
  strcpy(new_name,tmp);
 };
 
 DECLARE_MESSAGE_MAP()
};
BEGIN_MESSAGE_MAP(CRenameDlg, kDialog)
 ON_CONTROL(EN_UPDATE,RENAME_EDIT,editor_change)
END_MESSAGE_MAP()

