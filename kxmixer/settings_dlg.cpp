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

// Selection dialog classes
class CSettingsDlg : public kDialog
{
public:
 ~CSettingsDlg() { /*save_settings(SETTINGS_AUTO);*/ };
 iKX *ikx_t;

 const char *get_class_name() { return "kXSettings"; };

 kButton b_cancel;

 kStatic label;
 kCombo b_list[5];

 virtual void init()
 {
  kDialog::init();

  ::set_font(this,font,"setup_buffers");

  set_tool_font(font);

  //SetClassLong(m_hWnd,GCL_STYLE,GetClassLong(m_hWnd,GCL_STYLE)|0x00020000); // CS_DROPSHADOW

  char tmp_str[256];

  unsigned int transp_color=0x0;
  unsigned int alpha=90;
  unsigned int cc_method=0;

  if(mf.get_profile("setup_buffers","background",tmp_str,sizeof(tmp_str)))
  {
   tmp_str[0]=0;
  }
  gui_set_background(this,&mf,tmp_str,transp_color,alpha,cc_method,_T("mixer/settings_dlg.jpg"));

  set_dragging(gui_get_show_content());

  SetWindowText(mf.get_profile("lang","setup_buffers.name"));
  
  if(!mf.get_profile("setup_buffers","cancel",tmp_str,sizeof(tmp_str)))
  {
   gui_create_button(this,&b_cancel,IDCANCEL,tmp_str,mf);
   b_cancel.show();
  }

  kRect r; r.left=r.top=r.bottom=r.right=0;

  // label
  if(!mf.get_profile("setup_buffers","label",tmp_str,sizeof(tmp_str)))
  {
   dword fg=0,bk=0;
   sscanf(tmp_str,"%d %d %d %d %x %x",&r.left,&r.top,&r.right,&r.bottom,&fg,&bk);
   label.create((char *)(LPCTSTR)mf.get_profile("setup_buffers","label.text"),r.left,r.top,r.right,r.bottom,this,fg,bk);
   label.show();
  }

  int value=-1;

  r.left=10; r.top=10; r.right=200; r.bottom=80;
  if(!mf.get_profile("setup_buffers","tank",tmp_str,sizeof(tmp_str)))
  {
   sscanf(tmp_str,"%d %d %d %d",&r.left,&r.top,&r.right,&r.bottom);
  }
  b_list[0].create(r,CBS_DROPDOWNLIST/* | CBS_AUTOHSCROLL | WS_VSCROLL*/,1000,this);
  add_tool((char *)(LPCTSTR)mf.get_profile("lang","setup_buffers.tanksize"),&b_list[0],1000);
  if(font.get_font())
   b_list[0].set_font(font);

  ikx_t->get_buffers(KX_TANKMEM_BUFFER,&value);
  b_list[0].add_string("2048 KB");
  b_list[0].add_string("1024 KB");
  b_list[0].add_string("512 KB");
  b_list[0].add_string("256 KB");
  b_list[0].add_string("128 KB");
  b_list[0].add_string("64 KB");
  b_list[0].add_string("32 KB");
  b_list[0].add_string("16 KB");
  b_list[0].add_string(mf.get_profile("lang","setup_buffers.none"));
  switch(value)
  {
    case 2048*1024: b_list[0].set_selection(0); break;
        case 1024*1024: b_list[0].set_selection(1); break;
        case 512*1024: b_list[0].set_selection(2); break;
        case 256*1024: b_list[0].set_selection(3); break;
        case 128*1024: b_list[0].set_selection(4); break;
        case 64*1024: b_list[0].set_selection(5); break;
        case 32*1024: b_list[0].set_selection(6); break;
        case 16*1024: b_list[0].set_selection(7); break;
        case 0: b_list[0].set_selection(8); break;
        default: b_list[0].set_selection(-1); break;
  }
  b_list[0].set_dropped_width(140);
  b_list[0].show();

  r.left=10; r.top=35; r.right=200; r.bottom=110;
  if(!mf.get_profile("setup_buffers","pb",tmp_str,sizeof(tmp_str)))
  {
   sscanf(tmp_str,"%d %d %d %d",&r.left,&r.top,&r.right,&r.bottom);
  }

  b_list[1].create(r, CBS_DROPDOWNLIST /*| CBS_AUTOHSCROLL | WS_VSCROLL*/,1001,this);
  add_tool((char *)(LPCTSTR)mf.get_profile("lang","setup_buffers.pb_size"),&b_list[1],1001);
  if(font.get_font())
   b_list[1].set_font(font);

  ikx_t->get_buffers(KX_PLAYBACK_BUFFER,&value);
  b_list[1].add_string("240 bytes (~0.25 ms)");
  b_list[1].add_string("504 bytes (~0.53 ms)");
  b_list[1].add_string("1008 bytes (~1.05 ms)");
  b_list[1].add_string("2040 bytes (~2.12 ms)");
  b_list[1].add_string("4080 bytes (~4.25 ms)");
  b_list[1].add_string("8184 bytes (~8.53 ms)");
  b_list[1].add_string("8352 bytes (~8.70 ms)");
  b_list[1].add_string("9600 bytes (~10.00 ms)");
  b_list[1].add_string("16368 bytes (~17.05 ms)");

  switch(value)
  {
    case 240: b_list[1].set_selection(0); break;
    case 504: b_list[1].set_selection(1); break;
        case 1008: b_list[1].set_selection(2); break;
        case 2040: b_list[1].set_selection(3); break;
        case 4080: b_list[1].set_selection(4); break;
        case 8184: b_list[1].set_selection(5); break;
        case 8352: b_list[1].set_selection(6); break;
        case 9600: b_list[1].set_selection(7); break;
        case 16368: b_list[1].set_selection(8); break;
        default: b_list[1].set_selection(-1); break;
  }
  b_list[1].set_dropped_width(140);
  b_list[1].show();

  r.left=10; r.top=60; r.right=200; r.bottom=130;
  if(!mf.get_profile("setup_buffers","rec",tmp_str,sizeof(tmp_str)))
  {
   sscanf(tmp_str,"%d %d %d %d",&r.left,&r.top,&r.right,&r.bottom);
  }

  b_list[2].create(r,CBS_DROPDOWNLIST /*| CBS_AUTOHSCROLL | WS_VSCROLL*/,1002,this);
  add_tool((char *)(LPCTSTR)mf.get_profile("lang","setup_buffers.rec_size"),&b_list[2],1002);
  if(font.get_font())
   b_list[2].set_font(font);

  ikx_t->get_buffers(KX_RECORD_BUFFER,&value);

  b_list[2].add_string("384 bytes");
  b_list[2].add_string("448 bytes");
  b_list[2].add_string("512 bytes");
  b_list[2].add_string("640 bytes");
  b_list[2].add_string("768 bytes");
  b_list[2].add_string("896 bytes");
  b_list[2].add_string("1024 bytes");
  b_list[2].add_string("1280 bytes");
  b_list[2].add_string("1536 bytes");
  b_list[2].add_string("1792 bytes");
  b_list[2].add_string("2048 bytes");
  b_list[2].add_string("2560 bytes");
  b_list[2].add_string("3072 bytes");
  b_list[2].add_string("3584 bytes");
  b_list[2].add_string("4096 bytes");
  b_list[2].add_string("5120 bytes");
  b_list[2].add_string("6144 bytes");
  b_list[2].add_string("7168 bytes");
  b_list[2].add_string("8192 bytes");
  b_list[2].add_string("10240 bytes");
  b_list[2].add_string("12288 bytes");
  b_list[2].add_string("14366 bytes");
  b_list[2].add_string("16384 bytes");
  b_list[2].add_string("20480 bytes");
  b_list[2].add_string("24576 bytes");
  b_list[2].add_string("28672 bytes");
  b_list[2].add_string("32768 bytes");
  b_list[2].add_string("40960 bytes");
  b_list[2].add_string("49152 bytes");
  b_list[2].add_string("57344 bytes");
  b_list[2].add_string("65536 bytes");

  switch(value)
  {
        case 384: b_list[2].set_selection(0); break;
        case 448: b_list[2].set_selection(1); break;
        case 512: b_list[2].set_selection(2); break;
        case 640: b_list[2].set_selection(3); break;
        case 768: b_list[2].set_selection(4); break;
        case 896: b_list[2].set_selection(5); break;
        case 1024: b_list[2].set_selection(6); break;
        case 1280: b_list[2].set_selection(7); break;
        case 1536: b_list[2].set_selection(8); break;
        case 1792: b_list[2].set_selection(9); break;
        case 2048: b_list[2].set_selection(10); break;
        case 2560: b_list[2].set_selection(11); break;
        case 3072: b_list[2].set_selection(12); break;
        case 3584: b_list[2].set_selection(13); break;
        case 4096: b_list[2].set_selection(14); break;
        case 5120: b_list[2].set_selection(15); break;
        case 6144: b_list[2].set_selection(16); break;
        case 7168: b_list[2].set_selection(17); break;
        case 8192: b_list[2].set_selection(18); break;
        case 10240: b_list[2].set_selection(19); break;
        case 12288: b_list[2].set_selection(20); break;
        case 14366: b_list[2].set_selection(21); break;
        case 16384: b_list[2].set_selection(22); break;
        case 20480: b_list[2].set_selection(23); break;
        case 24576: b_list[2].set_selection(24); break;
        case 28672: b_list[2].set_selection(25); break;
        case 32768: b_list[2].set_selection(26); break;
        case 40960: b_list[2].set_selection(27); break;
        case 49152: b_list[2].set_selection(28); break;
        case 57344: b_list[2].set_selection(29); break;
        case 65536: b_list[2].set_selection(30); break;
        default: b_list[2].set_selection(-1); break;
  }
  b_list[2].set_dropped_width(140);
  b_list[2].show();

  r.left=10; r.top=85; r.right=200; r.bottom=155;
  if(!mf.get_profile("setup_buffers","ac3",tmp_str,sizeof(tmp_str)))
  {
   sscanf(tmp_str,"%d %d %d %d",&r.left,&r.top,&r.right,&r.bottom);
  }

  b_list[3].create(r, CBS_DROPDOWNLIST /*| CBS_AUTOHSCROLL | WS_VSCROLL*/, 1003, this);
  add_tool((char *)(LPCTSTR)mf.get_profile("lang","setup_buffers.num_ac3"),&b_list[3],1003);
  if(font.get_font())
   b_list[3].set_font(font);

  b_list[3].set_dropped_width(140);
  b_list[3].add_string("2 buffers");
  b_list[3].add_string("4 buffers");
  b_list[3].add_string("6 buffers");
  b_list[3].add_string("8 buffers");
  b_list[3].add_string("16 buffers");

  ikx_t->get_buffers(KX_AC3_BUFFERS,&value);
  switch(value)
  {
   case 2: b_list[3].set_selection(0); break;
   case 4: b_list[3].set_selection(1); break;
   case 6: b_list[3].set_selection(2); break;
   case 8: b_list[3].set_selection(3); break;
   case 16:  b_list[3].set_selection(4); break;
   default:  b_list[3].set_selection(-1); break;
  }
  b_list[3].ShowWindow(SW_SHOW);


  r.left=10; r.top=110; r.right=200; r.bottom=180;
  if(!mf.get_profile("setup_buffers","gsif",tmp_str,sizeof(tmp_str)))
  {
   sscanf(tmp_str,"%d %d %d %d",&r.left,&r.top,&r.right,&r.bottom);
  }

  b_list[4].create(r, CBS_DROPDOWNLIST /*| CBS_AUTOHSCROLL | WS_VSCROLL*/,1004, this);
  add_tool((char *)(LPCTSTR)mf.get_profile("lang","setup_buffers.gsif"),&b_list[4],1004);
  if(font.get_font())
   b_list[4].set_font(font);

  b_list[4].set_dropped_width(140);
  b_list[4].add_string("16 samples");
  b_list[4].add_string("32 samples");
  b_list[4].add_string("64 samples");
  b_list[4].add_string("128 samples");
  b_list[4].add_string("256 samples");
  b_list[4].add_string("512 samples");
  b_list[4].add_string("1024 samples");
  b_list[4].add_string("2048 samples");
  b_list[4].add_string("4096 samples");

  value=-1;
  ikx_t->get_buffers(KX_GSIF_BUFFER,&value);
  switch(value)
  {
   case 16:  b_list[4].set_selection(0); break;
   case 32:  b_list[4].set_selection(1); break;
   case 64:  b_list[4].set_selection(2); break;
   case 128:  b_list[4].set_selection(3); break;
   case 256:  b_list[4].set_selection(4); break;
   case 512:  b_list[4].set_selection(5); break;
   case 1024:  b_list[4].set_selection(6); break;
   case 2048:  b_list[4].set_selection(7); break;
   case 4096:  b_list[4].set_selection(8); break;
   default:  b_list[4].set_selection(-1); break;
  }
  b_list[4].ShowWindow(SW_SHOW);

  CenterWindow();
 };
 afx_msg void OnChange0() 
 { 
  switch(b_list[0].get_selection())
  {
    case 0: ikx_t->set_buffers(KX_TANKMEM_BUFFER,2048*1024); break;
        case 1: ikx_t->set_buffers(KX_TANKMEM_BUFFER,1024*1024); break;
        case 2: ikx_t->set_buffers(KX_TANKMEM_BUFFER,512*1024); break;
        case 3: ikx_t->set_buffers(KX_TANKMEM_BUFFER,256*1024); break;
        case 4: ikx_t->set_buffers(KX_TANKMEM_BUFFER,128*1024); break;
        case 5: ikx_t->set_buffers(KX_TANKMEM_BUFFER,64*1024); break;
        case 6: ikx_t->set_buffers(KX_TANKMEM_BUFFER,32*1024); break;
        case 7: ikx_t->set_buffers(KX_TANKMEM_BUFFER,16*1024); break;
        case 8: ikx_t->set_buffers(KX_TANKMEM_BUFFER,0); break;
        default: b_list[0].set_selection(-1); break;
  }
 };
 afx_msg void OnChange1() 
 { 
  switch(b_list[1].get_selection())
  {
    case 0: ikx_t->set_buffers(KX_PLAYBACK_BUFFER,240); break;
        case 1: ikx_t->set_buffers(KX_PLAYBACK_BUFFER,504); break;
        case 2: ikx_t->set_buffers(KX_PLAYBACK_BUFFER,1008); break;
        case 3: ikx_t->set_buffers(KX_PLAYBACK_BUFFER,2040); break;
        case 4: ikx_t->set_buffers(KX_PLAYBACK_BUFFER,4080); break;
        case 5: ikx_t->set_buffers(KX_PLAYBACK_BUFFER,8184); break;
        case 6: ikx_t->set_buffers(KX_PLAYBACK_BUFFER,8352); break;
        case 7: ikx_t->set_buffers(KX_PLAYBACK_BUFFER,9600); break;
        case 8: ikx_t->set_buffers(KX_PLAYBACK_BUFFER,16368); break;
        default: b_list[1].set_selection(-1); break;
  }
 };
 afx_msg void OnChange2() 
 { 
  switch(b_list[2].get_selection())
  {
        case 0: ikx_t->set_buffers(KX_RECORD_BUFFER,384); break;
        case 1: ikx_t->set_buffers(KX_RECORD_BUFFER,448); break;
        case 2: ikx_t->set_buffers(KX_RECORD_BUFFER,512); break;
        case 3: ikx_t->set_buffers(KX_RECORD_BUFFER,640); break;
        case 4: ikx_t->set_buffers(KX_RECORD_BUFFER,768); break;
        case 5: ikx_t->set_buffers(KX_RECORD_BUFFER,896); break;
        case 6: ikx_t->set_buffers(KX_RECORD_BUFFER,1024); break;
        case 7: ikx_t->set_buffers(KX_RECORD_BUFFER,1280); break;
        case 8: ikx_t->set_buffers(KX_RECORD_BUFFER,1536); break;
        case 9: ikx_t->set_buffers(KX_RECORD_BUFFER,1792); break;
        case 10: ikx_t->set_buffers(KX_RECORD_BUFFER,2048); break;
        case 11: ikx_t->set_buffers(KX_RECORD_BUFFER,2560); break;
        case 12: ikx_t->set_buffers(KX_RECORD_BUFFER,3072); break;
        case 13: ikx_t->set_buffers(KX_RECORD_BUFFER,3584); break;
        case 14: ikx_t->set_buffers(KX_RECORD_BUFFER,4096); break;
        case 15: ikx_t->set_buffers(KX_RECORD_BUFFER,5120); break;
        case 16: ikx_t->set_buffers(KX_RECORD_BUFFER,6144); break;
        case 17: ikx_t->set_buffers(KX_RECORD_BUFFER,7168); break;
        case 18: ikx_t->set_buffers(KX_RECORD_BUFFER,8192); break;
        case 19: ikx_t->set_buffers(KX_RECORD_BUFFER,10240); break;
        case 20: ikx_t->set_buffers(KX_RECORD_BUFFER,12288); break;
        case 21: ikx_t->set_buffers(KX_RECORD_BUFFER,14366); break;
        case 22: ikx_t->set_buffers(KX_RECORD_BUFFER,16384); break;
        case 23: ikx_t->set_buffers(KX_RECORD_BUFFER,20480); break;
        case 24: ikx_t->set_buffers(KX_RECORD_BUFFER,24576); break;
        case 25: ikx_t->set_buffers(KX_RECORD_BUFFER,28672); break;
        case 26: ikx_t->set_buffers(KX_RECORD_BUFFER,32768); break;
        case 27: ikx_t->set_buffers(KX_RECORD_BUFFER,40960); break;
        case 28: ikx_t->set_buffers(KX_RECORD_BUFFER,49152); break;
        case 29: ikx_t->set_buffers(KX_RECORD_BUFFER,57344); break;
        case 30: ikx_t->set_buffers(KX_RECORD_BUFFER,65536); break;
        default: b_list[2].set_selection(-1); break;
  }
 };
 afx_msg void OnChange3() 
 { 
  switch(b_list[3].get_selection())
  {
   case 0: ikx_t->set_buffers(KX_AC3_BUFFERS,2); break;
   case 1: ikx_t->set_buffers(KX_AC3_BUFFERS,4); break;
   case 2: ikx_t->set_buffers(KX_AC3_BUFFERS,6); break;
   case 3: ikx_t->set_buffers(KX_AC3_BUFFERS,8); break;
   case 4: ikx_t->set_buffers(KX_AC3_BUFFERS,16); break;
  }
 };
 afx_msg void OnChange4() 
 { 
  switch(b_list[4].get_selection())
  {
   case 0: ikx_t->set_buffers(KX_GSIF_BUFFER,16); break;
   case 1: ikx_t->set_buffers(KX_GSIF_BUFFER,32); break;
   case 2: ikx_t->set_buffers(KX_GSIF_BUFFER,64); break;
   case 3: ikx_t->set_buffers(KX_GSIF_BUFFER,128); break;
   case 4: ikx_t->set_buffers(KX_GSIF_BUFFER,256); break;
   case 5: ikx_t->set_buffers(KX_GSIF_BUFFER,512); break;
   case 6: ikx_t->set_buffers(KX_GSIF_BUFFER,1024); break;
   case 7: ikx_t->set_buffers(KX_GSIF_BUFFER,2048); break;
   case 8: ikx_t->set_buffers(KX_GSIF_BUFFER,4096); break;
  }
 };

 afx_msg void OnCancel() { DestroyWindow(); };
 afx_msg void OnOK() { DestroyWindow(); };
 void on_post_nc_destroy() { delete this; settings_window=0; };

 DECLARE_MESSAGE_MAP()
};
BEGIN_MESSAGE_MAP(CSettingsDlg, kDialog)
    ON_CBN_SELCHANGE(1000, OnChange0)
        ON_CBN_SELCHANGE(1001, OnChange1)
        ON_CBN_SELCHANGE(1002, OnChange2)
        ON_CBN_SELCHANGE(1003, OnChange3)
        ON_CBN_SELCHANGE(1004, OnChange4)
END_MESSAGE_MAP()
