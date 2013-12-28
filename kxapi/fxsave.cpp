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

// inlined
// -------

class CSavePresetDlg : public kDialog
{
public:
 CSavePresetDlg(kFile *mf_,char *templ,char *prm):kDialog() { mf=mf_; strncpy(new_name,templ,MAX_PRESET_NAME); prompt_text=prm; };
 kFile *mf;
 kButton b_ok;
 kButton b_cancel;
 #define RENAME_EDIT	0x1000
 kString prompt_text;
 kStatic prompt;
 CEdit b_new_name;
 char new_name[MAX_PRESET_NAME];

 const char *get_class_name() { return "kXSavePreset"; };

 virtual void init()
 {
  kDialog::init();

  // font: not applicable at the moment...
  // ::set_font(this,font,"preset");
  // t_t.SetFont(&font);

  char tmp_str[256];

  //SetClassLong(m_hWnd,GCL_STYLE,GetClassLong(m_hWnd,GCL_STYLE)|0x00020000); // CS_DROPSHADOW

  unsigned int transp_color=0x0;
  unsigned int alpha=90;
  unsigned int cc_method=0;

  set_dragging(gui_get_show_content());

  if(mf->get_profile("preset","background",tmp_str,sizeof(tmp_str)))
   tmp_str[0]=0;

  gui_set_background(this,mf,tmp_str,transp_color,alpha,(unsigned int &)cc_method,_T("mixer/preset_dlg.jpg"));

  set_text((LPCTSTR)mf->get_profile("lang","preset.name"));

  if(!mf->get_profile("preset","ok_button",tmp_str,sizeof(tmp_str)))
  {
   gui_create_button(this,&b_ok,IDOK,tmp_str,*mf);
   b_ok.show();
  }
  if(!mf->get_profile("preset","cancel_button",tmp_str,sizeof(tmp_str)))
  {
   gui_create_button(this,&b_cancel,IDCANCEL,tmp_str,*mf);
   b_cancel.show();
  }

  kRect r; 

  r.left=10; r.top=10; r.right=200; r.bottom=20;
  COLORREF fg=0xffffff;
  COLORREF bk=0xb0b000;

  if(!mf->get_profile("preset","preset_name",tmp_str,sizeof(tmp_str)))
  {
   sscanf(tmp_str,"%d %d %d %d %x %x",&r.left,&r.top,&r.right,&r.bottom,&fg,&bk);
   prompt.create((LPCTSTR)prompt_text,r.left,r.top,r.right,r.bottom,this,fg,bk);
   prompt.show();
  }

  r.left=10; r.top=40; r.right=200; r.bottom=20; 
  if(!mf->get_profile("preset","edit",tmp_str,sizeof(tmp_str)))
  {
   sscanf(tmp_str,"%d %d %d %d",&r.left,&r.top,&r.right,&r.bottom);
  }

  r.right+=r.left;
  r.bottom+=r.top;

  RECT rr; rr.left=r.x1; rr.right=r.x2; rr.top=r.y1; rr.bottom=r.y2;
  b_new_name.Create(/*ES_LOWERCASE*/0,rr,this,RENAME_EDIT);
  b_new_name.SetWindowText(new_name);
  b_new_name.ShowWindow(SW_SHOW);
  b_new_name.SetFont(font);

  return;
 };

 void editor_change(void)
 {
  kString tmp;
  b_new_name.GetWindowText(tmp);
  strcpy(new_name,tmp);
 };
 
 DECLARE_MESSAGE_MAP()
};
BEGIN_MESSAGE_MAP(CSavePresetDlg, kDialog)
 ON_CONTROL(EN_UPDATE,RENAME_EDIT,editor_change)
END_MESSAGE_MAP()
