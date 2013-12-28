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

// Translation dialog classes
class CTranslateDlg : public kDialog
{
public:
 kButton b_ok;
 kButton b_cancel;
 CComboBox b_pgm;
 kStatic label;
 CComboBox b_mode;
 int c_mode,c_pgm;

 const char *get_class_name() { return "kXTranslate"; };

 virtual void init()
 {
  kDialog::init();

  ::set_font(this,font,"translate");
  set_tool_font(font);

  char tmp_str[256];

  unsigned int transp_color=0x0;
  unsigned int alpha=90;
  unsigned int cc_method=0;

  set_dragging(gui_get_show_content());

  if(mf.get_profile("translate","background",tmp_str,sizeof(tmp_str)))
  {
   tmp_str[0]=0;
  }

  gui_set_background(this,&mf,tmp_str,transp_color,alpha,cc_method,_T("dsp/translate_dlg.jpg"));

  SetWindowText(mf.get_profile("lang","translate.name"));

  if(!mf.get_profile("translate","translate_button",tmp_str,sizeof(tmp_str)))
  {
   gui_create_button(this,&b_ok,IDOK,tmp_str,mf);
   b_ok.ShowWindow(SW_SHOW);
  }
  if(!mf.get_profile("translate","cancel_button",tmp_str,sizeof(tmp_str)))
  {
   gui_create_button(this,&b_cancel,IDCANCEL,tmp_str,mf);
   b_cancel.ShowWindow(SW_SHOW);
  }

  RECT r; memset(&r,0,sizeof(r));

  // label
  if(!mf.get_profile("translate","label",tmp_str,sizeof(tmp_str)))
  {
   dword fg=0,bk=0;
   sscanf(tmp_str,"%d %d %d %d %x %x",&r.left,&r.top,&r.right,&r.bottom,&fg,&bk);
   label.create((char *)(LPCTSTR)mf.get_profile("translate","label.text"),r.left,r.top,r.right,r.bottom,this,fg,bk);
   label.SetFont(font);
   label.show();
  }

  r.left=10; r.top=10; r.right=200; r.bottom=100;
  if(!mf.get_profile("translate","l1",tmp_str,sizeof(tmp_str)))
  {
   sscanf(tmp_str,"%d %d %d %d",&r.left,&r.top,&r.right,&r.bottom);
  }

  b_mode.Create(CBS_DROPDOWNLIST /*| CBS_AUTOHSCROLL | WS_VSCROLL*/,r,this,1001);
  b_mode.AddString((LPCTSTR)mf.get_profile("lang","translate.p1"));
  b_mode.AddString((LPCTSTR)mf.get_profile("lang","translate.p2"));
  b_mode.AddString((LPCTSTR)mf.get_profile("lang","translate.p3"));
  b_mode.SetDroppedWidth(190);
  b_mode.SetCurSel(0);
  b_mode.ShowWindow(SW_SHOW);
  b_mode.SetFont(font);
  c_mode=0;

  r.left=10; r.top=50; r.right=200; r.bottom=100;
  if(!mf.get_profile("translate","l2",tmp_str,sizeof(tmp_str)))
  {
   sscanf(tmp_str,"%d %d %d %d",&r.left,&r.top,&r.right,&r.bottom);
  }

  b_pgm.Create(CBS_DROPDOWNLIST /*| CBS_AUTOHSCROLL | WS_VSCROLL*/,r,this,1002);
  b_pgm.AddString("...not implemented...");
  b_pgm.SetDroppedWidth(190);
  b_pgm.SetCurSel(0);
  b_pgm.ShowWindow(SW_SHOW);
  b_pgm.SetFont(font);
  c_pgm=-1;
 };
 afx_msg void OnChange1()
 {
  c_mode=b_mode.GetCurSel();
 };
 afx_msg void OnChange2()
 {
  c_pgm=b_pgm.GetCurSel();
 };
 
 DECLARE_MESSAGE_MAP()
};
BEGIN_MESSAGE_MAP(CTranslateDlg, kDialog)
    ON_CBN_SELCHANGE(1001, OnChange1)
        ON_CBN_SELCHANGE(1002, OnChange2)
END_MESSAGE_MAP()
