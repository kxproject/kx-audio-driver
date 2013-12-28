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

#include "stdinc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

void CKXDialog::rest_position(char *p)
{
 if(p!=0)
  prefix=p;

 if(prefix==0)
  prefix=(char *)get_class_name();

 int bm_width=get_bitmap_width();
 if(bm_width<=0)
  bm_width=600;
 int bm_height=get_bitmap_height();
 if(bm_height<=0)
  bm_height=340;
 int prev_x=GetSystemMetrics(SM_CXFULLSCREEN)/2-bm_width/2,
     prev_y=GetSystemMetrics(SM_CYFULLSCREEN)/2-bm_height/2;

 kSettings cfg;
 char tmp_str[21];
 char tmp_key[21];

 sprintf(tmp_key,"Prev%sX",prefix);
 if(cfg.read_abs("Mixer",tmp_key,tmp_str,21)==0)
   sscanf(tmp_str,"%d",&prev_x);

 sprintf(tmp_key,"Prev%sY",prefix);
 if(cfg.read_abs("Mixer",tmp_key,tmp_str,21)==0)
   sscanf(tmp_str,"%d",&prev_y);

 if((prev_x<GetSystemMetrics(SM_CXFULLSCREEN)-16) &&
	   (prev_y<GetSystemMetrics(SM_CYFULLSCREEN)-16)&&
	   (prev_x>-bm_width+16) &&
	   (prev_y>-bm_height+16) &&
	   (prev_x>-2048) &&
	   (prev_y>-2048))
 {
 	SetWindowPos(NULL, prev_x, 
   		prev_y,
    		-1, -1,
   		SWP_NOSIZE|SWP_NOZORDER|SWP_NOREDRAW|SWP_NOACTIVATE);
 } else CenterWindow();
}

void CKXDialog::save_position(char *p)
{
	if(p!=0)
	 prefix=p;

	if(prefix==0)
	 prefix=(char *)get_class_name();

        // save previous win coords
        kSettings cfg;
	char tmp_str[21];
        char tmp_key[21];

        kRect r;

        get_rect_abs(r);
        sprintf(tmp_str,"%d",r.left);
        sprintf(tmp_key,"Prev%sX",prefix);
        cfg.write_abs("Mixer",tmp_key,tmp_str);
        sprintf(tmp_str,"%d",r.top);
        sprintf(tmp_key,"Prev%sY",prefix);
        cfg.write_abs("Mixer",tmp_key,tmp_str);
}

void CKXDialog::init(char *str)
{
  kDialog::init();

  ::set_font(this,font,str);
  set_tool_font(font);
  prefix=str;

  // handle 'OnTop' option
  CMenu* sys_menu = GetSystemMenu(FALSE);
  ASSERT((IDg_ONTOP & 0xFFF0) == IDg_ONTOP);
  ASSERT(IDg_ONTOP < 0xF000);

  dword on_top=0;

  kSettings cfg;

  if(cfg.read_abs("Mixer","OnTop",&on_top)==0)
  {
                 if(on_top)
                 {
                   ::SetWindowPos(this->m_hWnd,HWND_TOPMOST,-1,-1,-1,-1,SWP_NOMOVE|SWP_NOSIZE);
                 }
  }

  if(sys_menu != NULL)
  {
    sys_menu->AppendMenu(MF_SEPARATOR);
    sys_menu->AppendMenu(MF_STRING|MF_UNCHECKED, IDg_ONTOP, mf.get_profile("locals","always_on_top"));
    sys_menu->CheckMenuItem(IDg_ONTOP,(on_top==0)?MF_UNCHECKED:MF_CHECKED);
  }
}

void CKXDialog::on_sys_command(int nID, int lParam)
{
	if ((nID & 0xFFF0) == IDg_ONTOP)
	{
	     LONG t=GetWindowLong(this->m_hWnd,	GWL_EXSTYLE);
	     HWND p;
	     if(t&WS_EX_TOPMOST)
	      p=HWND_NOTOPMOST;
	     else
	      p=HWND_TOPMOST;

             CMenu* sys_menu = GetSystemMenu(FALSE);
             if(sys_menu)
               sys_menu->CheckMenuItem(IDg_ONTOP,(p==HWND_NOTOPMOST)?MF_UNCHECKED:MF_CHECKED);

             ::SetWindowPos(this->m_hWnd,p,-1,-1,-1,-1,SWP_NOMOVE|SWP_NOSIZE);

             dword on_top=(p==HWND_TOPMOST)?1:0;

             kSettings cfg;
             cfg.write_abs("Mixer","OnTop",on_top);
	}
	else
	{
		kDialog::on_sys_command(nID, lParam);
	}
}

void CKXDialog::generic_init(char *str,char *bk_sect_name)
{
	CKXDialog::init(str);

	SetIcon(AfxGetApp()->LoadIcon(IDR_ICON), TRUE);			// Set big icon
//	SetIcon(AfxGetApp()->LoadIcon(IDR_ICON), FALSE);		// Set small icon

        char tmp_str[256];

        unsigned int transp_color=0x0;
        unsigned int alpha=0;
        unsigned int cc_method=0;

        if(mf.get_profile(str,bk_sect_name,
          tmp_str,sizeof(tmp_str)))
        {
         tmp_str[0]=0;
        }

        gui_set_background(this,&mf,tmp_str,transp_color,alpha,cc_method,"");

        if(b_quit.m_hWnd) b_quit.destroy();
           if(!mf.get_profile(str,"quit_button",tmp_str,sizeof(tmp_str)))
           {
            gui_create_button(this,&b_quit,IDOK,tmp_str,mf);
            b_quit.show();
           }

        if(b_mini.m_hWnd) b_mini.destroy();
           if(!mf.get_profile(str,"mini_button",tmp_str,sizeof(tmp_str)))
           {
            gui_create_button(this,&b_mini,IDg_MINI,tmp_str,mf);
            b_mini.show();
           }

        if(b_info.m_hWnd) b_info.destroy();
           if(!mf.get_profile(str,"info_button",tmp_str,sizeof(tmp_str)))
           {
            gui_create_button(this,&b_info,IDg_INFO,tmp_str,mf);
            b_info.show();
           }

        if(b_save_settings.m_hWnd) b_save_settings.destroy();
           if(!mf.get_profile(str,"save_settings",tmp_str,sizeof(tmp_str)))
           {
            gui_create_button(this,&b_save_settings,IDg_SAVE_SETTINGS,tmp_str,mf);
            b_save_settings.show();
           }

        if(b_load_settings.m_hWnd) b_load_settings.destroy();
           if(!mf.get_profile(str,"load_settings",tmp_str,sizeof(tmp_str)))
           {
            gui_create_button(this,&b_load_settings,IDg_LOAD_SETTINGS,tmp_str,mf);
            b_load_settings.show();
           }

           // label
         if(label.m_hWnd) label.destroy();
           if(!mf.get_profile(str,"label",tmp_str,sizeof(tmp_str)))
           {
            dword fg=0,bk=0;
            RECT r; memset(&r,0,sizeof(r));
            sscanf(tmp_str,"%d %d %d %d %x %x",&r.left,&r.top,&r.right,&r.bottom,&fg,&bk);
            label.create((char *)(LPCTSTR)mf.get_profile(str,"label.text"),r.left,r.top,r.right,r.bottom,this,fg,bk);
            label.show();
           }


        rest_position();

        set_dragging(gui_get_show_content());
        show();
}

void CKXDialog::device_init(char *str)
{
	if(card_name.m_hWnd) card_name.DestroyWindow();

	char tmp_str[256];

        // re-instantiate device
        dword p[]=
        {
          0xb0b000,0xffff00,0xb0b000,0xffffff,
          0xb0b000,0xffff00,0xb0b000,0x660000
        };

        if(!mf.get_profile(str,"card_name",tmp_str,sizeof(tmp_str)))
        {
         kRect r; r.left=r.top=r.bottom=r.right=0;

         sscanf(tmp_str,"%d %d %d %d %x %x %x %x %x %x %x %x",&r.left,&r.top,&r.right,&r.bottom,
          &p[0],&p[1],&p[2],&p[3],&p[4],&p[5],&p[6],&p[7]);

         kRect r_w;
         get_rect(r_w);

         if(r.left<0)
          r.left=r_w.width()+r.left;

         if(r.top<0)
          r.top=r_w.height()+r.top;

         r.right+=r.left;
         r.bottom+=r.top;

         card_name.create(ikx_t->get_device_name(),r,this,IDg_CARDNAME,
          p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);

         add_tool((char *)(LPCTSTR)mf.get_profile("locals","select_card"),&card_name,IDg_CARDNAME);

         card_name.show();
        }
}


int CKXDialog::on_command(int wParam, int lParam)
{
	switch(wParam)
	{
		case IDg_INFO:
		    manager->launch(LAUNCH_INFO);
                    return 1;
                case IDg_MINI:
                    SendMessage(WM_SYSCOMMAND,SC_MINIMIZE);
                    return 1;
                case IDg_CARDNAME:
                    {
                     manager->change_device(-2,this);
                     return 1;
                    }
                case IDg_SAVE_SETTINGS:
                    manager->save_settings(SETTINGS_GUI,NULL,get_flag()|KX_SAVED_NO_VERSION|KX_SAVED_NO_CARDNAME);
                    select_device();
                    return 1;
                case IDg_LOAD_SETTINGS:
                    manager->restore_settings(SETTINGS_GUI,NULL,get_flag()|KX_SAVED_NO_VERSION|KX_SAVED_NO_CARDNAME);
                    select_device();
                    return 1;
	}
	return kDialog::on_command(wParam,lParam);
}
