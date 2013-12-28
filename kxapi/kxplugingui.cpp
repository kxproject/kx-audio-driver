// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov and Max Mikhailov, 2001-2005.
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

#include "interface/kxapi.h"
#undef CDC
#include "gui/kGui.h"

#include "interface/kxplugingui.h"
#include "defplugingui.h"

#include "fxsave.cpp"

#ifdef _DEBUG
 #define new DEBUG_NEW
 #undef THIS_FILE
 static char THIS_FILE[] = __FILE__;
#endif


BEGIN_MESSAGE_MAP(CKXPluginGUI, kDialog)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_CONTROL_RANGE(CBN_SELCHANGE, kCOMBO_CONTROL_BASE, (kCOMBO_CONTROL_BASE+kCONTROL_TOTAL), OnCombo)
END_MESSAGE_MAP()


CKXPluginGUI::CKXPluginGUI(kDialog *parent_,kFile *mf_):
 kDialog()
{
 mf.attach_skin(mf_);
 k_parent=parent_;
}


int CKXPluginGUI::create()
{
 if(kDialog::create((kDialog *)k_parent,
   ((WS_POPUP | WS_SYSMENU | WS_MINIMIZEBOX | WS_TABSTOP | WS_BORDER)&(~WS_MAXIMIZEBOX)))==0)
 {
  set_dragging(gui_get_show_content());

  return 0;
 }
 else
  return -1;
}

int CKXPluginGUI::draw_nc(kDraw &)
{
 return 1;
}

void CKXPluginGUI::create_dialog(const char *caption,int width, int height, int ext_height,int flags)
{
 if(get_plugin())
 {
  HINSTANCE inst=get_plugin()->instance;
  char module[512];
  if(GetModuleFileName((HMODULE)inst,module,sizeof(module))>0)
  {
  	// try to attach the built-in skin...
  	if(mf.set_skin(module)==0)
  	 mf.set_attach_priority(1);
  }
 }

  resize_dialog(caption,width,height,ext_height,flags);

  add_tool((char *)(LPCTSTR)mf.get_profile("lang","preset"),&preset,kPRESET_ID);
}

void CKXPluginGUI::resize_dialog(const char *caption,int width, int height, int ext_height,int flags)
{
		align_x=5;
		align_y=font_height*12/10+7;
		char tmp_str[128];
                if(!mf.get_profile("plugins","align",tmp_str,sizeof(tmp_str)))
                {
                 sscanf(tmp_str,"%d %d",&align_x,&align_y);
                }

		if(ext_height==-1)
		{
		   ext_height=font_height*12/10+10;
		}

		if(!(flags&KXFX_NO_RESIZE))
		{
		 if(width<font_width*MAX_PRESET_NAME)
		  width=font_width*MAX_PRESET_NAME;
		}

        	kString tmp_header;
        	tmp_header=" ";
        	tmp_header+=(get_plugin()?get_plugin()->name[0]?get_plugin()->name:caption:caption);
        	if(get_plugin() && (width>=font_width*MAX_PRESET_NAME))
        	{
        	 tmp_header+=" - ";
        	 tmp_header+=get_plugin()->ikx->get_device_name();
        	}
        	set_text((LPCTSTR)tmp_header);

        	CRect client,window; 
        	GetWindowRect(&window);
        	GetClientRect(&client);

                int header_height=window.Height()-client.Height();

        	// set dialog size and caption
        	SetWindowPos(NULL, -1, -1, width+align_x, height+header_height+ext_height+align_y,
        		SWP_NOMOVE|SWP_NOZORDER|SWP_NOREDRAW|SWP_NOACTIVATE);

        	{
                    if(mf.get_profile("plugins","background",tmp_str,sizeof(tmp_str)))
                     tmp_str[0]=0;
                    else
                    {
                     if(strstr(tmp_str," tiled")!=0)
                     {
                      char ttt[64];
                      sprintf(ttt," size=%d %d",width+align_x,height+header_height+ext_height+align_y);
                      strcat(tmp_str,ttt);
                     }
                    }

                    unsigned int transp_color=0x0;
                    unsigned int alpha=90;
                    unsigned int cc_method=0;

                    if(tmp_str[0])
                     gui_set_background(this,&mf,tmp_str,transp_color,alpha,cc_method,_T("mixer/plg_tile.jpg"));
                }

        	dword bg=0x0,fg=0xffffff;
        	kRect r;
        	r.left=r.top=r.bottom=r.right=0;

                if(!mf.get_profile("plugins","header",tmp_str,sizeof(tmp_str)))
                {
                 sscanf(tmp_str,"%x %x %d %d %d %d",&fg,&bg,
                   &r.left,&r.top,&r.right,&r.bottom);
                }

                if(::IsWindow(w_label.get_wnd()))
                 w_label.destroy();

        	w_label.create((LPCTSTR)tmp_header,r.left,r.top,width+align_x+r.right,align_y+r.bottom,this,fg,bg);
        	w_label.show();

        	// minimize + close buttons
        	r.left=width+align_x;
        	r.top=0;

        	if(!mf.get_profile("plugins","mini_button",tmp_str,sizeof(tmp_str)))
        	{
        	 int t1=0,t2=0;
        	 sscanf(tmp_str,"%d %d",&t1,&t2);
        	 r.left+=t1;
        	 r.top+=t2;
        	}
        	r.right=r.left+1; r.bottom=r.top+1;

        	sprintf(tmp_str,"buttons.plugin.mini %d %d",r.left,r.top);

        	if(::IsWindow(b_minimize.get_wnd()))
        	 b_minimize.destroy();

        	gui_create_button(this,&b_minimize,kMINI_ID,tmp_str,mf);
        	b_minimize.show();

        	r.left=width+align_x;
        	r.top=0;

        	if(!mf.get_profile("plugins","close_button",tmp_str,sizeof(tmp_str)))
        	{
        	 int t1=0,t2=0;
        	 sscanf(tmp_str,"%d %d",&t1,&t2);
        	 r.left+=t1;
        	 r.top+=t2;
        	}
        	r.right=r.left+1; r.bottom=r.top+1;

        	sprintf(tmp_str,"buttons.plugin.close %d %d",r.left,r.top);

        	if(::IsWindow(b_close.get_wnd()))
        	 b_close.destroy();

        	gui_create_button(this,&b_close,kCLOSE_ID,tmp_str,mf);
        	b_close.show();

        	// add groupbox control
        	r.left = align_x+2; r.top = align_y+0; r.right = align_x+width - 2 - 6; r.bottom = align_y+height-2;

        	if(::IsWindow(grpbox.get_wnd()))
        	 grpbox.destroy();

        	grpbox.create("", _T("BUTTON"),r, BS_GROUPBOX, 0, this);
		    grpbox.set_font(font);
        	grpbox.show();

        	if(!(flags&KXFX_NO_RESET))
        	{
        	 r.left=2;
        	 r.top=5;

        	 char button_name[64];
        	 if(!mf.get_profile("plugins","def_button",tmp_str,sizeof(tmp_str)))
        	 {
        	  sscanf(tmp_str,"%s %d %d",button_name,&r.left,&r.top);
        	 }
        	 sprintf(tmp_str,"%s %d %d",button_name,r.left+align_x,r.top+height+align_y);

        	 if(::IsWindow(b_reset.get_wnd()))
        	  b_reset.destroy();

                 gui_create_button(this,&b_reset,kRESET_ID,tmp_str,mf);
                 b_reset.show();

        	 r.left=22;
        	 r.top=5;

        	 if(!mf.get_profile("plugins","mute_button",tmp_str,sizeof(tmp_str)))
        	 {
        	  sscanf(tmp_str,"%s %d %d",button_name,&r.left,&r.top);
        	 }
                 sprintf(tmp_str,"%s %d %d",button_name,r.left+align_x,r.top+height+align_y);

                 if(::IsWindow(b_mute.get_wnd()))
                  b_mute.destroy();

                 gui_create_button(this,&b_mute,kMUTE_ID,tmp_str,mf);

        	 r.left=42;
        	 r.top=5;

        	 if(!mf.get_profile("plugins","bypass_button",tmp_str,sizeof(tmp_str)))
        	 {
        	  sscanf(tmp_str,"%s %d %d",button_name,&r.left,&r.top);
        	 }
        	 sprintf(tmp_str,"%s %d %d",button_name,r.left+align_x,r.top+height+align_y);

        	 if(::IsWindow(b_bypass.get_wnd()))
        	  b_bypass.destroy();

                 gui_create_button(this,&b_bypass,kBYPASS_ID,tmp_str,mf);

                 update_buttons();
        	}

                dword p[8];

                if(!mf.get_profile("plugins","preset",tmp_str,sizeof(tmp_str)))
                {
                 sscanf(tmp_str,"%x %x %x %x %x %x %x %x",
                  &p[0],&p[1],&p[2],&p[3],&p[4],&p[5],&p[6],&p[7]);
                }
                if(!(flags&KXFX_NO_RESET))
                {
                 if(mf.get_profile("plugins","preset_button",tmp_str,sizeof(tmp_str)))
                 {
                  b_bypass.get_rect_abs(r);

                  r.left=r.right+4; r.right=width-align_x;
                  r.top=align_y+height+5;
                  r.bottom=r.top + font_height*12/10;
                 }
                 else
                 {
                   r.bottom=-1;
                   r.right=-1;

                   sscanf(tmp_str,"%d %d %d %d",&r.left,&r.top,&r.right,&r.bottom);

                   if(r.right==-1)
                    r.right=width-align_x;

                   if(r.bottom==-1)
                    r.bottom=r.top+font_height*12/10;
                 }

                 if(::IsWindow(preset.get_wnd()))
                  preset.destroy();

                 preset.create(preset_name,r,this,kPRESET_ID,
                   p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);

                 preset.set_font(font);

                 update_preset_name();
                 preset.show();
                }
}


void CKXPluginGUI::create_vfader(kFader &fader,UINT ctrl_id,const char *label,int minval,int maxval,int x,int y,int wd,int ht)
{
        	kRect r;

        	x+=align_x;
        	y+=align_y;

        	r.left = x; r.top = y;
        	r.right = x+ wd; r.bottom = y + font_height;

        	RECT rr; rr.left=r.left; rr.top=r.top; rr.bottom=r.bottom; rr.right=r.right;
        	fader.svalue.Create(ES_CENTER|ES_READONLY, rr, this, ctrl_id + kHIDDEN_CONTROL_BASE);
		    fader.svalue.SetFont(font);
        	fader.svalue.ShowWindow(SW_SHOW);

        	r.left = (x + wd / 2) - 10; r.top = y + font_height;
        	r.right = (x + wd / 2) + 20; r.bottom = y + ht - font_height;

                /*
        	fader.slider.SetBitmaps(
			mf.load_image(mf.get_profile("all","default_slider")),
          		mf.load_image(mf.get_profile("all","slider_thumb")),
          		mf.load_image(mf.get_profile("all","slider_thumb_d")),
          		mf.load_image(mf.get_profile("all","slider_thumb_s")));
                */
        	fader.slider.create(NULL,TBS_VERT|TBS_RIGHT|TBS_AUTOTICKS, r, this, ctrl_id + kCONTROL_BASE);
        	fader.slider.set_range(minval, maxval, FALSE);
        	fader.slider.set_tic_freq((int) (0.5 + (double) (maxval- minval) / 4.0));
        	fader.slider.show();

        	r.left = x; r.top = y + ht - font_height;
        	r.right = x + wd; r.bottom = r.top + font_height;
        	fader.slabel.create(label, SS_CENTER, r, this);
		    fader.slabel.set_font(font);
        	fader.slabel.show();
}

void CKXPluginGUI::create_combo(kCombo &combo,UINT ctrl_id,const char *label,int x,int y,int wd,int n_items,int dropped_width)
{
        	kRect r;

        	if(n_items==-1)
        	 n_items=7;

        	x+=align_x;
        	y+=align_y;

        	r.left = x; r.top = y;
        	r.right = x+ wd; r.bottom = y + font_height * 14 / 10 * (n_items+1);

        	combo.create(r,CBS_DROPDOWNLIST | WS_VISIBLE, ctrl_id+kCOMBO_CONTROL_BASE,this);
        	if(dropped_width==-1)
        	 dropped_width=wd;

        	combo.set_dropped_width(dropped_width);
        	combo.set_font(font);
        	add_tool(label,&combo,ctrl_id+kCOMBO_CONTROL_BASE);
        	combo.show();
}

void CKXPluginGUI::create_hfader(kFader &fader,UINT ctrl_id,const char *label,int minval,int maxval,int left,int top,int width,int label_width,int value_symbols)
{
        	left+=align_x;
       		top+=align_y;

        	kRect r;
        	r.left = left; r.top = top + 5;
        	r.right = left + label_width; r.bottom = top + font_height+5;

        	fader.slabel.create(label, SS_CENTER , r, this);
		    fader.slabel.set_font(font);
        	fader.slabel.show();

        	r.left = left + label_width + 5; r.top = top + 5;
        	r.right = left + label_width + value_symbols*font_width; r.bottom = top + font_height+5;

        	RECT rr;
        	rr.left=r.left; rr.right=r.right; rr.top=r.top; rr.bottom=r.bottom;

        	fader.svalue.Create(ES_LEFT|ES_READONLY, rr, this, ctrl_id + kHIDDEN_CONTROL_BASE);
		    fader.svalue.SetFont(font);
        	fader.svalue.ShowWindow(SW_SHOW);

        	r.left = left + label_width + value_symbols*font_width; r.top = top;
        	r.right = left + width; r.bottom = top + 2*font_height;

                /*
        	fader.slider.SetBitmaps(
			mf.load_image(mf.get_profile("all","default_slider")),
          		mf.load_image(mf.get_profile("all","slider_thumb")),
          		mf.load_image(mf.get_profile("all","slider_thumb_d")),
          		mf.load_image(mf.get_profile("all","slider_thumb_s")));
                */

        	fader.slider.create(NULL,TBS_HORZ|TBS_BOTTOM|TBS_AUTOTICKS, r, this, ctrl_id + kCONTROL_BASE);
        	fader.slider.set_range(minval, maxval, FALSE); // don't redraw
        	fader.slider.set_tic_freq((int) (0.5 + (double) (maxval - minval) / 4.0));
        	fader.slider.show();
}

void CKXPluginGUI::create_hslider(kSlider &fader,UINT ctrl_id,const char *label,int minval,int maxval,int left,int top,int wd,int ht)
{
        	left+=align_x;
       		top+=align_y;

        	kRect r;
        	r.left = left; r.top = top;
        	r.right = left + wd; r.bottom = top + ht;

                /*
        	fader.slider.SetBitmaps(
			mf.load_image(mf.get_profile("all","default_slider")),
          		mf.load_image(mf.get_profile("all","slider_thumb")),
          		mf.load_image(mf.get_profile("all","slider_thumb_d")),
          		mf.load_image(mf.get_profile("all","slider_thumb_s")));
                */
        	fader.create(label,TBS_HORZ|TBS_BOTTOM|TBS_AUTOTICKS, r, this, ctrl_id + kCONTROL_BASE);
        	fader.set_range(minval, maxval);
        	fader.set_tic_freq((int) (0.5 + (double) (maxval - minval) / 4.0));
        	fader.show();

        	if(label)
        	 add_tool(label,&fader,ctrl_id+kCONTROL_BASE);
}

void CKXPluginGUI::create_vslider(kSlider &fader,UINT ctrl_id,const char *label,int minval,int maxval,int x,int y,int wd,int ht)
{
        	kRect r;

        	x+=align_x;
        	y+=align_y;

        	r.left = x; r.top = y;
        	r.right = x+ wd; r.bottom = y + ht;
                /*
        	fader.slider.SetBitmaps(
			mf.load_image(mf.get_profile("all","default_slider")),
          		mf.load_image(mf.get_profile("all","slider_thumb")),
          		mf.load_image(mf.get_profile("all","slider_thumb_d")),
          		mf.load_image(mf.get_profile("all","slider_thumb_s")));
                */
        	fader.create(NULL,TBS_VERT|TBS_RIGHT|TBS_AUTOTICKS, r, this, ctrl_id + kCONTROL_BASE);
        	fader.set_range(minval, maxval);
        	fader.set_tic_freq((int) (0.5 + (double) (maxval- minval) / 4.0));
        	fader.show();

        	if(label)
        	 add_tool(label,&fader,ctrl_id+kCONTROL_BASE);
}


void CKXPluginGUI::create_checkbox(kCheckButton &checkbox,UINT ctrl_id,const char *caption,int left,int top,int width)
{
        	left+=align_x;
        	top+=align_y;

        	kRect r;
		r.left = left; r.top = top; r.right = left + width, r.bottom = top + font_height;
		checkbox.create(caption, r, this, ctrl_id + kCONTROL_BASE,kMETHOD_DEFAULT,0);
		checkbox.set_font(font);
		checkbox.show();
}

void CKXPluginGUI::create_label(kStatic &label,UINT ctrl_id,const char *caption,int left,int top,int width)
{
        	left+=align_x;
        	top+=align_y;

		kRect r;
		r.left = left; r.top = top; r.right = left + width, r.bottom = top + font_height;
		label.create(caption, SS_LEFT, r, this, ctrl_id + kCONTROL_BASE);
		label.set_font(font);
		label.show();
}

void CKXPluginGUI::create_button(kButton &button,UINT ctrl_id,const char *caption,int left,int top,int width)
{
        	left+=align_x;
        	top+=align_y;

		kRect r;
		r.left = left; r.top = top; r.right = left + width, r.bottom = top + font_height*12/10;
                button.create(caption, BS_PUSHBUTTON, r, this, ctrl_id + kCONTROL_BASE,kMETHOD_DEFAULT);
		button.set_font(font);
		button.show();
}

#define IDg_ONTOP (UINT)((0x10+(uintptr_t)m_hWnd)&0xff0)

void CKXPluginGUI::init()
{
 kDialog::init();

 char tmp_str[16];
 if(!mf.get_profile("plugins","drop_shadow",tmp_str,sizeof(tmp_str)))
 {
  int drop=0;
  sscanf(tmp_str,"%d",&drop);
  if(drop)
    SetClassLong(m_hWnd,GCL_STYLE,GetClassLong(m_hWnd,GCL_STYLE)|0x00020000); // CS_DROPSHADOW
 }

	 CDC *dc=GetDC();

         gui_create_point_font(font,mf,"plugins","font","90 Tahoma",*dc);

         // calculate generic font height
         HFONT prev_f=(HFONT)SelectObject(*dc,font.get_font());

         SIZE sz;
         GetTextExtentPoint32(*dc,"QqYyGgÉé¨¸",10,&sz);
         font_height=sz.cy;
         GetTextExtentPoint32(*dc,"O",1,&sz);
         font_width=sz.cx;

         SelectObject(*dc,prev_f);

         ReleaseDC(dc);

         set_tool_font(font);

 CMenu* sys_menu = GetSystemMenu(FALSE);
 sys_menu->AppendMenu(MF_SEPARATOR);
 sys_menu->AppendMenu(MF_STRING|MF_UNCHECKED, IDg_ONTOP, mf.get_profile("locals","always_on_top"));
 sys_menu->CheckMenuItem(IDg_ONTOP,MF_UNCHECKED);
}

void CKXPluginGUI::on_sys_command(int wParam,int lParam)
{
	if (((unsigned)wParam & 0xFFF0) == IDg_ONTOP)
	{
	     LONG t=GetWindowLong(this->m_hWnd,	GWL_EXSTYLE);
	     HWND p;
	     if(t&WS_EX_TOPMOST)
	      p=HWND_NOTOPMOST;
	     else
	      p=HWND_TOPMOST;

             CMenu* sys_menu = GetSystemMenu(FALSE);
             sys_menu->CheckMenuItem(IDg_ONTOP,(p==HWND_NOTOPMOST)?MF_UNCHECKED:MF_CHECKED);

             ::SetWindowPos(this->m_hWnd,p,-1,-1,-1,-1,SWP_NOMOVE|SWP_NOSIZE);
	}
	else
	{
		if(wParam==SC_MAXIMIZE)
			return;
		if(wParam==SC_CLOSE)
		{
                   if(get_plugin())
                    get_plugin()->notify(IKX_CLOSE_CP);
		}
		kDialog::on_sys_command(wParam, lParam);
	}
}

void CKXPluginGUI::update_preset_name()
{
 if(strcmp((LPCTSTR)preset_name,mf.get_profile("lang","preset.default"))!=0)
  preset_name=mf.get_profile("lang","preset.custom");

 char tmp_buff[KX_MAX_STRING];
 if(get_plugin())
 {
  if(get_plugin()->get_current_preset(tmp_buff)==0)
   preset_name=tmp_buff;
 }

 preset.set_text((LPCTSTR)((kString)" "+(kString)preset_name+(kString)" "));
// preset.text=preset_name;
 preset.redraw();
}

void CKXPluginGUI::update_buttons()
{
  iKXPlugin *plg=get_plugin();

  if(plg)
  {
     // --------- read the present state
     dsp_microcode m; m.flag=0;
     plg->ikx->enum_microcode(plg->pgm_id,&m);

     if(m.flag&MICROCODE_ENABLED)
     {
      if(strstr(m.comment,"$nobypass")==0)
      {
         if(m.flag&MICROCODE_BYPASS)
          b_bypass.set_check(1);
         else
          b_bypass.set_check(0);

         b_bypass.show();
         b_bypass.redraw();
      }

      b_mute.set_check(0);
     } 
      else 
     {
      b_mute.set_check(1);
      b_bypass.hide();
     }

	  if(::IsWindow(b_mute.get_wnd()))
	  {
		b_mute.show();
		b_mute.redraw();
	  }
  }
}

int CKXPluginGUI::on_command(int lParam,int wParam)
{
  int ndx = lParam - kCONTROL_BASE;

  if((ndx>=0) && (ndx<kCONTROL_TOTAL))
  {
  		kCheckButton *cb=0;
  		cb=(kCheckButton *)GetDlgItem(lParam);
  		if(cb /*&& (strcmp(cb->get_class_name(),_T("kGuiCheckButton"))==0)*/)
  		{
		 controls_enabled = FALSE;
		 get_plugin()->set_param(ndx, cb->get_check());
		 controls_enabled = TRUE;

		 return 1; // _should_ return 1: message processed
		}
  }

  switch(ndx)
  {
  		case kMUTE_ID-kCONTROL_BASE:
  			if(get_plugin())
  			{
  			 dsp_microcode m;
  			 int pgm_id=get_plugin()->pgm_id;
  			 if(get_plugin()->ikx->enum_microcode(pgm_id,&m)==0)
  			 {
  			  if(m.flag&MICROCODE_ENABLED)
  			   get_plugin()->ikx->disable_microcode(pgm_id);
  			  else
  			   get_plugin()->ikx->enable_microcode(pgm_id);

  			  update_buttons();
  			 }
  			}
  			return 1;
  		case kBYPASS_ID-kCONTROL_BASE:
  			if(get_plugin())
  			{
  			 dsp_microcode m;
  			 int pgm_id=get_plugin()->pgm_id;
  			 if(get_plugin()->ikx->enum_microcode(pgm_id,&m)==0)
  			 {
  			  if(m.flag&MICROCODE_BYPASS)
  			   get_plugin()->ikx->set_microcode_bypass(pgm_id,0);
  			  else
  			   get_plugin()->ikx->set_microcode_bypass(pgm_id,1);

  			  update_buttons();
  			 }
  			}
  			return 1;
		case kCLOSE_ID-kCONTROL_BASE: // close
			PostMessage(WM_SYSCOMMAND,SC_CLOSE,0);
			return 1;
		case kMINI_ID-kCONTROL_BASE: // minimize
			PostMessage(WM_SYSCOMMAND,SC_MINIMIZE,0);
			return 1;
		case kRESET_ID-kCONTROL_BASE:
			get_plugin()->set_defaults();
			preset_name=mf.get_profile("lang","preset.default");
			update_preset_name();
			sync();
			return 1; // _should_ return 1: message processed
		case kPRESET_ID-kCONTROL_BASE:
			{
                         iKXPlugin *plg=get_plugin();
                         if(!plg)
                         {
                          debug("kxapi: plugin: invalid GUI w/o plugins\n");
                          break;
                         }

			 kMenu menu;
			 menu.create();
			 SetForegroundWindow();

                         menu.add(mf.get_profile("lang","preset.save"),1);
                         menu.add(mf.get_profile("lang","preset.delete"),2);
                         menu.separator();
                         menu.add(mf.get_profile("lang","preset.default"),3);

                         int pr=plg->populate_presets(&menu);
                         if(pr>=0)
                         {
                          menu.separator();
                          menu.add(mf.get_profile("menu","presets.import"),4);
                          if(pr==0)
                           menu.add(mf.get_profile("menu","presets.export"),5);
                         }

                         POINT pt;
                         GetCursorPos(&pt);
                         int ret=menu.track(TPM_LEFTBUTTON|TPM_RIGHTBUTTON|TPM_RETURNCMD,
                                			pt.x,pt.y,this);

                         menu.destroy();
			

                         switch(ret)
                         {
                          case 1: // save
                          	{
                          	 CSavePresetDlg dlg(&mf,(char *)(LPCTSTR)preset_name,(char *)(LPCTSTR)mf.get_profile("lang","preset.savedlg"));
                          	 dlg.set_font(font);
                          	 if(dlg.do_modal(this)==IDCANCEL)
                        	 {
                        	  return 1;
                        	 }

                                 if(plg)
                                    plg->save_preset(dlg.new_name);

                                 update_preset_name();
                          	}
                          	break;
                          case 2: // delete
                          	{
                          	 if(
                          	  (strcmp((LPCTSTR)mf.get_profile("lang","preset.default"),(LPCTSTR)preset_name)!=NULL)
                          	  &&
                          	  (strcmp((LPCTSTR)mf.get_profile("lang","preset.custom"),(LPCTSTR)preset_name)!=NULL)
                          	   )
                          	 {
                          	  kString t=mf.get_profile("lang","preset.confirm");
                          	  char msg[256+KX_MAX_STRING];
                          	  sprintf(msg,(LPCTSTR)t,(LPCTSTR)preset_name);

                          	  if(MessageBox(msg,"",MB_YESNO)==IDYES)
                          	  {
                          	    if(plg)
                          	     plg->delete_preset((TCHAR *)(LPCTSTR)preset_name);

                          	    update_preset_name();
                                  }
                                 }
                          	}
                          	break;
                          case 3: // default
                           {
                            SendMessage(WM_COMMAND,kRESET_ID,0);
                            sync();
                            break;
                           }
                          case 4: // import
                          	{
                                CFileDialog *f_d = new CFileDialog(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_EXPLORER,
                                  "kX Plugin Presets (*.kxp)|*.kxp||",this);
                                if(f_d)
                                {
                                	char tmp_cwd[MAX_PATH];
                                	GetCurrentDirectory(sizeof(tmp_cwd),tmp_cwd);
                                	f_d->m_ofn.lpstrInitialDir=tmp_cwd;

                                	if(f_d->DoModal()==IDCANCEL)
                                	{
                                	 delete f_d;
                                	 break;
                                	}

                                	kString fname=f_d->GetPathName();
                                	delete f_d;

                                	if(fname.GetLength()==0) // cancel or empty
                                	{
                                	 break;
                                	}

                                	if(plg)
                                	 plg->import_presets((TCHAR *)(LPCTSTR)fname);
                                }
                                }
                                break;

                          case 5: // export
                          	{

                          	char tmp_name[MAX_PATH]; tmp_name[0]=0;

                          	if(plg)
                          	{
                          	 const char *pn=plg->get_plugin_description(IKX_PLUGIN_NAME);
                          	 if(pn)
                          	 {
                          	  strncpy(tmp_name,pn,MAX_PATH);
                          	  strcat(tmp_name,".kxp");
                          	 }

                                CFileDialog *f_d = new CFileDialog(FALSE,NULL,tmp_name,OFN_HIDEREADONLY|OFN_EXPLORER|OFN_OVERWRITEPROMPT,
                                  "kX Plugin Presets (*.kxp)|*.kxp||",this);
                                if(f_d)
                                {
                                	char tmp_cwd[MAX_PATH];
                                	GetCurrentDirectory(sizeof(tmp_cwd),tmp_cwd);
                                	f_d->m_ofn.lpstrInitialDir=tmp_cwd;

                                	if(f_d->DoModal()==IDCANCEL)
                                	{
                                	 delete f_d;
                                	 break;
                                	}

                                	kString fname=f_d->GetPathName();
                                	delete f_d;

                                	if(fname.GetLength()==0) // cancel or empty
                                	{
                                	 break;
                                	}
                                        strncpy(tmp_name,(LPCTSTR)fname,MAX_PATH);
                                        if(strstr(tmp_name,".kxp")==0)
                                         strcat(tmp_name,".kxp");
                                	plg->export_presets(tmp_name);
                                }
                                }
                                }
                                break;

                          default:
                           {
                            
                            if((ret>=IKXPLUGIN_PRESETS_CUSTOM)&&(ret<(IKXPLUGIN_PRESETS_BUILTIN+IKXPLUGIN_MAX_PRESETS)))
                            {
                               if(plg->apply_preset(ret)==0)
                               {
                                 sync();
                               }
                               update_preset_name();
                            }
                             else
                              if(ret!=0)
                               debug("kxapi: invalid after-preset menu id [%d]\n",ret);
                           }
                           break;
                         }
			}
			return 1;
	}
	return kDialog::on_command(wParam,lParam);
}

void CKXPluginGUI::sync(int)
{
 // nop
}

iKXPlugin *CKXPluginGUI::get_plugin()
{ 
 debug("Pure virtual function: fxstdgui::get_plugin()\n");
 return NULL;
}

void CKXPluginGUI::init_gui()
{
}

void CKXPluginGUI::show_gui()
{
	show();
}

void CKXPluginGUI::hide_gui()
{
	hide();
}

void CKXPluginGUI::close_gui() // assumes 'DestroyWindow'
{
	if(get_plugin())
	 get_plugin()->notify(IKX_CLOSE_CP);

	DestroyWindow();
}

uintptr_t CKXPluginGUI::extended_gui(int command,uintptr_t p1,uintptr_t p2)
{
 // main commands:
 switch(command)
 {
   case PLUGINGUI_SET_ICON:
   	SetIcon((HICON)p1,(BOOL)p2);
   	break;
   case PLUGINGUI_CENTER:
 	CenterWindow();
 	break;
   case PLUGINGUI_FOREGROUND:
   	ShowWindow(SW_RESTORE);
 	SetForegroundWindow();
 	break;
   case PLUGINGUI_GET_CLASS:
   	return PLUGINGUI_CLASS_CKXPLUGIN;
   case PLUGINGUI_EVENT:
   	switch(p1)
   	{
   	 case IKX_ENABLE:
   	 case IKX_DISABLE:
   	 case IKX_BYPASS_ON:
   	 case IKX_BYPASS_OFF:
   	 	update_buttons();
   	 	break;
   	 case IKX_UNTRANSLATE:
   	 case IKX_UNLOAD:
   	 	send_message(WM_COMMAND,kCLOSE_ID,0);
  	 	break;
   	}
   	break;
   case PLUGINGUI_SET_POSITION:
        // restore position:
        move((int)p1,(int)p2);
        break;
   case PLUGINGUI_GET_POSITION:
   	{
        kRect rr;
        get_rect_abs(rr);
        if(p1)
         *(int *)p1=rr.left;
        if(p2)
         *(int *)p2=rr.top;
        }
   	break;
   case PLUGINGUI_GET_HWND:
   	{
   	 return (uintptr_t)get_wnd();
   	}
   	break;
   default:
   	return (uintptr_t)-1;
 }
 return 0;
}

void CKXPluginGUI::OnCombo(UINT id)
{
	kxparam_t value;

	kCombo *c=(kCombo *)GetDlgItem(id);

	if(c)
	{
         value=c->get_selection();
         get_plugin()->set_param(id-kCOMBO_CONTROL_BASE, value);
        }
}


void CKXPluginGUI::OnVScroll(UINT , UINT , CScrollBar* pScrollBar)
{
	// ScrollBar is actually kSlider... 
	kSlider* slider = (kSlider*) pScrollBar;
	int ndx = ::GetDlgCtrlID(slider->get_wnd()) - kCONTROL_BASE;
	long value = VSGETPOS((*slider)); // use VGETPOS (instead of ->get_pos()) because vertical slider is kinda inverted

/*	if (nSBCode == TB_TOP){
		value = default_params[ndx];
		slider->set_pos(value);
	}
*/	
	controls_enabled = FALSE;
	get_plugin()->set_param(ndx, value);
	controls_enabled = TRUE;
}

void CKXPluginGUI::OnHScroll(UINT , UINT , CScrollBar* pScrollBar)
{
	// ScrollBar is actually kSlider... 
	kSlider* slider = (kSlider*) pScrollBar;
	int ndx = ::GetDlgCtrlID(slider->get_wnd()) - kCONTROL_BASE;
	long value = slider->get_pos();

/*	if (nSBCode == TB_TOP){
		value = default_params[ndx];
		VSSETPOS((*slider), value);
	}
*/
	controls_enabled = FALSE;
	get_plugin()->set_param(ndx, value);
	controls_enabled = TRUE;
}
