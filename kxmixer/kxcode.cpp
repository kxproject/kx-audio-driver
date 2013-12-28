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


// kxdspDlg.cpp : implementation file
//

#include "stdinc.h"

#include "cmaxafx.h"

#include "edit_dlg.h"
#include "kxcode.h"
#include "kxdspDlg.h"
#include "midirouter.h"

#include "rename_dsp.cpp"

CKxCode::CKxCode()
{
 parent=NULL;
 capture=0;
 is_moving=0;
 pgm_id=-1;
 plugin=0;

 cc_link_progress=0xffff00;
 cc_progress_text=0xffff00;
 cc_progress_bk=0x800000;

 flag=0;
 offset=DSP_MICROCODE_NOT_TRANSLATED;
 code=NULL;
 code_size=0;
 info=NULL;
 info_size=0;
 itramsize=0;
 xtramsize=0;
 memset(name,0,sizeof(name));

 set_method(kMETHOD_OWNER);
}

CKxCode::~CKxCode()
{
 if(plugin)
 {
  if(plugin->dsp_wnd)
  {
   delete plugin->dsp_wnd;
   plugin->dsp_wnd=0;
  }
 }
 if(code)
 {
  free(code); code=0; code_size=0;
 }
 if(info)
 {
  free(info); info=0; info_size=0;
 }
 // should not deal with plugin
 plugin=NULL;
}

void CKxCode::on_destroy()
{
/* if(b_minimax.get_wnd())
 {
  b_minimax.destroy();
 }
*/
 if(plugin)
 {
  if(plugin->dsp_wnd)
  {
   delete plugin->dsp_wnd;
   plugin->dsp_wnd=0;
  }
 }

 if(code)
 {
  free(code); code=0; code_size=0;
 }
 if(info)
 {
  free(info); info=0; info_size=0;
 }
 flag=0;
 offset=DSP_MICROCODE_NOT_TRANSLATED;
 itramsize=0;
 xtramsize=0;
 pgm_id=-1;

 // should not deal with plugin
 plugin=NULL;

 parent=NULL;
 capture=0;
 is_moving=0;

 memset(name,0,sizeof(name));

 kDialog::on_destroy();
}

int CKxCode::draw(kDraw &d)
{
 do_paint(d.get_dc());

 return 0; // don't draw default
}

void CKxCode::do_paint(HDC dc,int x,int y)
{
 kDraw d(dc);
 if(plugin)
  if(plugin->dsp_wnd)
  {
   if(!(flag&MICROCODE_HIDDEN))
    plugin->dsp_wnd->draw(&d,x,y,flag,name,pgm_id);
  }
}

int CKxCode::refresh(void)
{
        dsp_microcode mc;
        char tmp_name[KX_MAX_STRING*2];

        if(!parent)
         return FALSE;

        if(pgm_id==-1)
         return FALSE;

        if(parent->ikx_t->enum_microcode(pgm_id,&mc)==0)
        {
         strncpy(name,mc.name,sizeof(name));
         strncpy(m_copyright,mc.copyright,sizeof(m_copyright));
         strncpy(m_comment,mc.comment,sizeof(m_comment));
         strncpy(m_engine,mc.engine,sizeof(m_engine));
         strncpy(m_created,mc.created,sizeof(m_created));
         strncpy(m_guid,mc.guid,sizeof(m_guid));

         sprintf(tmp_name,"[%d] %s",pgm_id,mc.name);
         flag=mc.flag;
         offset=mc.offset;
         itramsize=mc.itramsize;
         xtramsize=mc.xtramsize;

         if(flag&MICROCODE_ENABLED)
         {
          strcat(tmp_name,(LPCTSTR)mf.get_profile("lang","translate.enabled")); 

          if(flag&MICROCODE_BYPASS)
           strcat(tmp_name,(LPCTSTR)mf.get_profile("lang","translate.bypass"));
         }
         else 
         {
          if(flag&MICROCODE_TRANSLATED)
           strcat(tmp_name,(LPCTSTR)mf.get_profile("lang","translate.disabled"));
          else
           strcat(tmp_name,(LPCTSTR)mf.get_profile("lang","translate.not_translated"));
         }
        }
         else 
        {
        	return FALSE;
        }

        if(info)
        {
               free(info);
               info=0;
        }
        if(code)
        {
               free(code);
               code=0;
        }

        plugin=parent->pm_t->find_plugin(pgm_id,m_guid); // find plugin; load if necessary

        if(plugin==0)
        {
         // unable to load plugin
         debug("kxmixer: unable to load plugin [kxcode / 188]\n");
//         return FALSE;
        }

	info_size=mc.info_size;
	code_size=mc.code_size;
	if(info_size)
	{
          info=(dsp_register_info *)malloc(info_size); memset(info,0,info_size);
	}
	else
	  info=NULL;

	if(code_size)
	{
          code=(dsp_code *)malloc(code_size); memset(code,0,code_size);
	}
	else
	  code=NULL;

        if(parent->ikx_t->get_microcode(pgm_id,code,code_size,info,info_size))
        {
        	MessageBox((LPCTSTR)mf.get_profile("errors","get_microcode"),"kX Mixer",MB_OK|MB_ICONEXCLAMATION);
        	return FALSE;
        }

        if(plugin)
        {
         if(!plugin->dsp_wnd)
         {
          plugin->dsp_wnd=plugin->create_dsp_wnd(parent,this,&mf);
         }
        
         // resize here
         plugin->dsp_wnd->n_ins=0;
         plugin->dsp_wnd->n_outs=0;

         for(dword i=0;i<info_size/sizeof(dsp_register_info);i++)
         {
          if((info[i].type&GPR_MASK)==GPR_INPUT) plugin->dsp_wnd->n_ins++;
          if((info[i].type&GPR_MASK)==GPR_OUTPUT) plugin->dsp_wnd->n_outs++;
         }

	}

        kSize sz; sz.cx=0; sz.cy=0;
        if(plugin)
         if(plugin->dsp_wnd)
          plugin->dsp_wnd->get_window_rect(name,flag,&sz);

	SetWindowPos(NULL,-1,-1,sz.cx,sz.cy,SWP_NOMOVE|SWP_NOZORDER);

	SetWindowText(name);

	char tmp_str[256];

        if(plugin && plugin->dsp_wnd)
        {
            // update colors
            plugin->dsp_wnd->configure();

            if(!mf.get_profile("dsp","cc_link",tmp_str,sizeof(tmp_str)))
             sscanf(tmp_str,"%x",&cc_link_progress);
            if(!mf.get_profile("dsp","cc_tooltip",tmp_str,sizeof(tmp_str)))
             sscanf(tmp_str,"%x %x",&cc_progress_text,&cc_progress_bk);
        }

        if(mc.flag&MICROCODE_HIDDEN)
         hide();
        else
         show();

        if(b_minimax.get_wnd()!=0)
        {
         b_minimax.destroy();
        }

        int px=-9,py=1;

        if(!mf.get_profile("dsp","minimax",tmp_str,sizeof(tmp_str)))
         sscanf(tmp_str,"%d %d",&px,&py);

        sprintf(tmp_str,"buttons.dsp_plugin.%s %d %d",(flag&MICROCODE_MINIMIZED)?"restore":"minimize",sz.cx+px,py);
        gui_create_button(this,&b_minimax,IDg_MINIMAX,tmp_str,mf);

        b_minimax.show();

	if(::IsWindow(m_hWnd))
	{
	 parent->add_tool(LPSTR_TEXTCALLBACK,this,0x1000+pgm_id);
	 def_tooltip.Format("%s",tmp_name);
	 RedrawWindow();
	}

	return TRUE;
}


BOOL CKxCode::create(int x,int y,int pgm_id_,CKxdspDlg *parent_)
{
	pgm_id=pgm_id_;
	parent=parent_;

	RECT win_pos;
	win_pos.left=x; win_pos.right=win_pos.left+10;
	win_pos.top=y; win_pos.bottom=win_pos.top+10;

	LPCTSTR tmp_class=AfxRegisterWndClass(CS_DBLCLKS, 0, 0, 0);

	BOOL ret=CWnd::CreateEx(0,tmp_class, name, WS_CHILD|WS_CLIPSIBLINGS, win_pos, parent, 0x1000+pgm_id);

    set_font(*parent->get_font());

    set_dragging(1); // always '1'
    refresh();

	return ret;
}

void CKxCode::on_mouse_l_down(kPoint point,int flags)
{
	if(!capture)
	{
	  // check, if it is 'connect'
	  int connect=0;
	  kRect rect;
	  get_rect(rect);

	  int j;
	  for(j=0;j<plugin->dsp_wnd->n_ins;j++)
	  {
	  	int gpr=-1;

	  	if(plugin && plugin->dsp_wnd)
	  	if(plugin->dsp_wnd->find_gpr(&point,&gpr,flag)==0)
	  	{
	  		connect=1;
	  		int tmp=0;
	  		gpr_to_connect=gpr;
                        break;
	  	}
	  }
	  for(j=0;j<plugin->dsp_wnd->n_outs;j++)
	  {
	  	int gpr=-1;
	  	if(plugin && plugin->dsp_wnd)
	  	if(plugin->dsp_wnd->find_gpr(&point,&gpr,flag)==0)
	  	{
	  		connect=1;
	  		gpr_to_connect=gpr;
	  	}
	  }
	  if(!connect)
	  {
	  	// nothing: single click
		 BringWindowToTop();
		 SetCapture();
		 capture=1;
		 is_moving=1;
		 // ClientToScreen(&point);
		 capture_pos.left=point.x;
		 capture_pos.top=point.y;
	  }
	  else
	  {
	    if(flag&MICROCODE_TRANSLATED)
	    {
	  	BringWindowToTop();
	  	SetCapture();
	  	capture=1;
	  	is_moving=0;
	  	ClientToScreen(point);
		capture_pos.left=point.x;
		capture_pos.top=point.y;
	  	// info[gpr_to_connect].num,
	  	// info[gpr_to_connect].translated);
	    }	
	  }
	}
}

void CKxCode::on_mouse_l_up(kPoint point,int flags)
{
	if(capture)
	{
	  if(is_moving)
	  {
		ReleaseCapture();
		capture=0;
		is_moving=0;
                parent->RedrawWindow();
	  }
	  else // try to connect here
	  {
		ReleaseCapture();
		capture=0;

                // find target
                ClientToScreen(point);
                CKxCode *pgm=NULL;
                int gpr_num=0;

                if(parent->find_pgm_gpr(&point,&pgm,&gpr_num)==0)
                {
                  if((pgm==NULL) || (pgm->flag&MICROCODE_TRANSLATED))
                  {
                  	int fl=!(GetKeyState(VK_MENU)&0x8000);

                    dword plain_conn=0;
                    kSettings cfg;
                    cfg.read_abs("Mixer","PlainConn",&plain_conn);

                    if(plain_conn)
                     fl=(1-fl);

                        // special case: check fxbus/xrouting/prolog/epilog
                        // check _source_
                        if(
                           strcmp(m_guid,"631d8de5-11bc-4c3d-a0d2-f079977fd184")==0 || // prolog
                           strcmp(m_guid,"ceffc302-ea28-44df-873f-d3df1ba31736")==0 || // epilog
                           strcmp(m_guid,"2b8b7fa8-98b9-4f6e-81a0-400d3ba39c6f")==0 || // fxbus
                           strcmp(m_guid,"131f1059-f384-4403-abd0-16ef6025bb9b")==0 || // fxbus2
                           strcmp(m_guid,"d25a7874-7c00-47ca-8ad3-1b13106bde91")==0 || // fxbusx [new]
                           strcmp(m_guid,"313149FA-24FB-4f08-9C22-EB38B651BA58")==0) // xrouting
                         fl=0;

                        parent->pm_t->connect(plugin,gpr_to_connect,
                          pgm?pgm->plugin:NULL,gpr_num,fl);

		  	gpr_to_connect=-1;
		  }
		}
		parent->redraw_window();
		return;
	  }
	}
}

void CKxCode::on_mouse_move(kPoint point,int flags)
{
	if(capture)
	{
	  if(is_moving)
	  {
		kRect r;
		get_rect_abs(r);
		parent->ScreenToClient(r);
		int dx=point.x-capture_pos.left;
		int dy=point.y-capture_pos.top;
		r.left+=dx;
		r.right+=dx;
		r.top+=dy;
		r.bottom+=dy;
		reposition(r);

		int *pl_x=0,*pl_y=0;
		parent->pm_t->find_plugin_ex(pgm_id,m_guid,&pl_x,&pl_y);
		if(pl_x!=0)
		 *pl_x=r.left;
		if(pl_y!=0)
		 *pl_y=r.top;

                parent->RedrawWindow(NULL,NULL,RDW_NOCHILDREN|RDW_INVALIDATE);
          }
          else // connecting...
          {
          	ClientToScreen(point);
          	kRect r;
          	parent->get_rect_abs(r);
          	if(r.point_in_rect(point))
          	{
          		parent->RedrawWindow(NULL,NULL,RDW_NOCHILDREN|RDW_INVALIDATE|RDW_INTERNALPAINT|RDW_UPDATENOW);

                        HDC dc=::GetDC(parent->m_hWnd);
                        POINT pt1,pt2;
                        pt1.x=capture_pos.left;
                        pt1.y=capture_pos.top;
                        pt2.x=point.x;
                        pt2.y=point.y;
                        ::ScreenToClient(parent->get_wnd(),&pt1);
                        ::ScreenToClient(parent->get_wnd(),&pt2);

                        MoveToEx(dc,pt1.x,pt1.y,NULL);

			HPEN c_p=CreatePen(PS_SOLID,0,cc_link_progress);
                        HPEN p_o=(HPEN)SelectObject(dc,c_p);
                        LineTo(dc,pt2.x,pt2.y);

                        CKxCode *pgm=NULL;
                        int gpr=-1;
                        char tmp1[KX_MAX_STRING*4];
                        if(parent->find_pgm_gpr(&point,&pgm,&gpr,tmp1)==0)
                        {
                        kString s;
                        if(pgm && (gpr>=0))
                        {
                         s=(kString)" " + (kString)tmp1 + (kString)" ";
                         SetTextColor(dc,cc_progress_text);
                         SetBkColor(dc,cc_progress_bk);

                         // doesn't work...
                         //kFont tmp;
                         //::set_font(this,tmp,"dsp_tooltip");

                         //HGDIOBJ prev_f=::SelectObject(dc,(HGDIOBJ)(HFONT)tmp.get_font());

                         HFONT prev_f=(HFONT)SelectObject(dc,(HFONT)font);
                         ExtTextOut(dc,pt2.x+4,pt2.y-18,ETO_OPAQUE,NULL,(LPCTSTR)s,s.GetLength(),NULL);
                         SelectObject(dc,prev_f);

                         //DeleteObject((HFONT)tmp);
                        }
                        }

                        SelectObject(dc,p_o);
			DeleteObject(c_p);
                        ::ReleaseDC(parent->get_wnd(),dc);
                }
          }
	}
	else
	{
         static kPoint prev_pos;
         if(prev_pos.x!=point.x || prev_pos.y!=point.y)
         {
           parent->update_tooltip();
           prev_pos.x=point.x;
           prev_pos.y=point.y;
         }
	}
}

void CKxCode::on_mouse_r_up(kPoint p,int flags)
{
	kPoint ori_p=p;

	int gpr=0;
	if(plugin && plugin->dsp_wnd)
	if(plugin->dsp_wnd->find_gpr(&p,&gpr,flag)==0) // right click over connection
	{
        	// create context menu
        	if(((info[gpr].type&GPR_MASK)==GPR_OUTPUT)||
        	   ((info[gpr].type&GPR_MASK)==GPR_INPUT))
        	{
        	    if(!(GetKeyState(VK_MENU)&0x8000))
        	    {
        	    	kMenu menu;

        	    	menu.create();

                	SetForegroundWindow();
                        menu.add(((info[gpr].type&GPR_MASK)==GPR_INPUT)?
                          mf.get_profile("menu","disc_in") :
                          mf.get_profile("menu","disc_out"),
                          1);
                        menu.separator();

			kMenu plugin_menu;
			plugin_menu.create();
                        parent->pm_t->create_plugins_menu(&plugin_menu);

                        menu.add(mf.get_profile("menu","eff_list"),&plugin_menu);

                        ClientToScreen(p);
                        int ret=menu.track(TPM_LEFTBUTTON|TPM_RIGHTBUTTON|TPM_RETURNCMD,
                					p.x,p.y,this);

                        if(ret==1)
                        {
        		   	parent->ikx_t->disconnect_microcode(pgm_id,(word)info[gpr].num);
            		   	parent->redraw_window();
                        }
                        else
                         if(ret>=MENU_PLUGIN_BASE)
                         {
                                       iKXPlugin *plg=0;

                                       ret=parent->pm_t->load_plugin(ret,&plg);
                                       if(ret<-10)
                                       {
                                              switch(MessageBox(mf.get_profile("errors","instant"),"kX Mixer",MB_OK|MB_ICONEXCLAMATION|MB_YESNO))
                                              {
                                               case IDYES:
                                                 parent->pm_t->unregister_plugin(ret);
                                                 break;
                                              }
                                       }
                                        else
                                       {
                                              if(ret==0)
                                              {
                                               if(plg)
                                               {
                                                 // adjust p.x, p.y
                                                 kRect r; parent->get_rect_abs(r);
                                                 p.x-=r.x1;
                                                 p.y-=r.y1;

                                                 p.y-=18; // fixme: header size
                                                 if(ori_p.x<=5) // 'left' inputs
                                                 {
                                                  get_rect(r);
                                                  p.x-=10;
                                                  p.x-=r.x2;
                                                 } 
                                                 else
                                                  p.x+=10;

                                                 if(p.x<0)
                                                  p.x=0;

                                                 if(p.y<0)
                                                  p.y=0;

                                                 parent->pm_t->move_plugin_window(plg->pgm_id,p.x,p.y);

                                                 // translate
                                                 if(parent->pm_t->perform_translate(parent,plg)==0)
                                                 {
                                                 	// enable
                                                 	parent->ikx_t->enable_microcode(plg->pgm_id); 
                                                 	plg->event(IKX_ENABLE);

                                                        // connect here
                                                        debug("kxmixer: connect here (not implemented) FIXME\n");
                                                 }
                                                }
                                                else
                                                {
                                                  // plg=pl->plugin;
                                                  debug("well, a very bad thing here...\n");
                                                  // FIXME NOW
                                                  ASSERT((plg==NULL));
                                                }
                                              }
                                       }

                                       parent->redraw_window();
                         }
                        return;
                    }
                    else
                    {
        		   	parent->ikx_t->disconnect_microcode(pgm_id,(word)info[gpr].num);
            		   	parent->redraw_window();
            		   	return;
                    }
                }
	}
	else
	{
        	// create context menu
        	kMenu menu;
        	menu.create();
        	SetForegroundWindow();

                // create items
                if(flag&MICROCODE_ENABLED)
                {
                 menu.add(mf.get_profile("menu","disable"),2);

                  if(strstr(m_comment,"$nobypass")==0)
                  {
                   menu.add(mf.get_profile("menu","bypass"),14,
                      ((flag&MICROCODE_BYPASS)?MF_CHECKED:MF_UNCHECKED)
                      );
                  }
                }
                else
                {
                 if(offset!=DSP_MICROCODE_NOT_TRANSLATED)
                 {
                  menu.add(mf.get_profile("menu","enable"),1);
                 }
                }

                if(offset==DSP_MICROCODE_NOT_TRANSLATED)
                 menu.add(mf.get_profile("menu","translate"),4);
                else
                 menu.add(mf.get_profile("menu","untranslate"),6);

                menu.separator();
                int added=0;
                if(plugin)
                {
		     if(parent->pm_t->get_plugin_type(pgm_id,m_guid)==KX_OBJECT_DANE)
		     {
                        menu.add(mf.get_profile("menu","edit_dump"),5);
                        menu.add(mf.get_profile("menu","edit_source"),12);
                        added=1;
                     }
                }
                if(added==0)
                 menu.add(mf.get_profile("menu","edit"),5);

                menu.separator();

                if(plugin)
                {
                  if(plugin->get_user_interface() ||
                   (strcmp(m_guid,"631d8de5-11bc-4c3d-a0d2-f079977fd184")==NULL) || // prolog
                   (strcmp(m_guid,"ceffc302-ea28-44df-873f-d3df1ba31736")==NULL) || // epilog
                   (strcmp(m_guid,"313149FA-24FB-4f08-9C22-EB38B651BA58")==NULL) || // xrouter
                   (strcmp(m_guid,"2b8b7fa8-98b9-4f6e-81a0-400d3ba39c6f")==NULL) || // fxbus
                   (strcmp(m_guid,"d25a7874-7c00-47ca-8ad3-1b13106bde91")==NULL) || // fxbusx
                   /* d25a7874-7c00-47ca-8ad3-1b13106bde91: fxbusx [new] should have 'tweak' */
                   (strcmp(m_guid,"131f1059-f384-4403-abd0-16ef6025bb9b")==NULL)) // fxbus2
                	menu.add(mf.get_profile("menu","tweak"),9);

                  // add 'presets' menu, too:
                  kMenu submenu;
                  submenu.create();

                  int pr=plugin->populate_presets(&submenu);
                  if(pr>=0)
                  {
                   submenu.separator();
                   submenu.add(mf.get_profile("menu","presets.import"),16);
                   if(pr==0)
                    submenu.add(mf.get_profile("menu","presets.export"),15);

                   menu.add(mf.get_profile("menu","presets"),&submenu);
                  }
                  else
                  {
                   submenu.destroy();
                  }

                  menu.add(mf.get_profile("menu","set_defs"),10);

                  menu.separator();

                  if(offset!=DSP_MICROCODE_NOT_TRANSLATED && (flag&MICROCODE_ENABLED))
                   menu.add(mf.get_profile("menu","fxctrl"),13);
                }
                menu.add(mf.get_profile("menu","info"),8);
                menu.add(mf.get_profile("menu","rename"),11);

                if(flag&MICROCODE_MINIMIZED)
                 menu.add(mf.get_profile("lang","dsp_plugin.restore"),17);
                else
                 menu.add(mf.get_profile("lang","dsp_plugin.minimize"),18);

                menu.separator();
                
                kMenu plugin_menu;
                plugin_menu.create();
                parent->pm_t->create_plugins_menu(&plugin_menu);
                menu.add(mf.get_profile("menu","swap"),&plugin_menu);

                menu.add(mf.get_profile("menu","unload"),3);

                ClientToScreen(p);

                int ret=menu.track(TPM_LEFTBUTTON|TPM_RIGHTBUTTON|TPM_RETURNCMD,
        					p.x,p.y,this);

                switch(ret)
                {
                	case 1: 
                		parent->ikx_t->enable_microcode(pgm_id); 
                		if(plugin)
                		  plugin->event(IKX_ENABLE);
                		parent->redraw_window(); 
                		return;
                	case 14: 
                		if(flag&MICROCODE_BYPASS)
                		{
                		 parent->ikx_t->set_microcode_bypass(pgm_id,0); 
                		 if(plugin)
                		   plugin->event(IKX_BYPASS_OFF);
                		}
                		else
                		{
                		 parent->ikx_t->set_microcode_bypass(pgm_id,1); 
                		 if(plugin)
                		   plugin->event(IKX_BYPASS_ON);
                		}
                		parent->redraw_window(); 
                		return;
                	case 2: 
                		parent->ikx_t->disable_microcode(pgm_id); 
                		if(plugin) 
                		  plugin->event(IKX_DISABLE); 
                		parent->redraw_window(); 
                		return;
                	case 3: 
                		if(plugin) 
                		{
                		 // event is sent by unload_plugin
                		 parent->pm_t->unload_plugin(plugin);
                		 plugin=0; 
                		}
	               		parent->ikx_t->unload_microcode(pgm_id); 
                		pgm_id=-1; 
                		parent->redraw_window(); 
                		return;
                	case 6: 
                		if(plugin)
                		 plugin->event(IKX_UNTRANSLATE);
                		parent->ikx_t->untranslate_microcode(pgm_id); 
                		parent->redraw_window(); 
                		return;
                	case 4:
                		{
                		if(parent->pm_t->perform_translate(parent,plugin)==0)
                		  parent->redraw_window();
                		}
                		return;
                	case 5:
                            manager->launch(LAUNCH_EDITOR,-1,(dword)pgm_id,NULL);
                            break;
                	case 8: // Plugin Info
                		{
                        	   	kString tmp;
 					tmp.Format(mf.get_profile("lang","dsp.info"),
                        	   	 name,
					 m_copyright,
                                         m_comment,
                                         m_engine,
                                         m_created,
                                         m_guid,
                                         code_size/sizeof(dsp_code),
                                         info_size/sizeof(dsp_register_info),
                                         itramsize,
                                         xtramsize);
                                        MessageBox((LPCTSTR)tmp,mf.get_profile("lang","dsp.infotitle"),MB_OK|MB_ICONINFORMATION);
                		}
                		return;
                	case 9: // Tweak plugin
                		{
                			if(plugin)
                			{
                			       parent->pm_t->tweak_plugin(pgm_id);
                			}
                		}
                		return;
                	case 10: // set defaults
                		{
                			if(plugin)
                			  plugin->set_defaults();
                		}
                		return;
                	case 11: // rename
                		{
                        		CRenameDlg dlg;
                        		strcpy(dlg.new_name,name);

                                        // remove spaces
                        		while(dlg.new_name[strlen(dlg.new_name)-1]==' ')
                        		 dlg.new_name[strlen(dlg.new_name)-1]='\x0';

                        		// remove spaces
                        		while(dlg.new_name[0]==' ')
                        		 strcpy(dlg.new_name,&dlg.new_name[1]);

                                	int s=dlg.do_modal(parent);
                                        if(s==IDOK)
                                        {
                                		parent->ikx_t->set_microcode_name(pgm_id,dlg.new_name);
                                		parent->redraw_window();
                        		}
                		}
                		return;
                        case 12: // edit source
                        	{
                                 if(plugin)
                                 {
		                   if(parent->pm_t->get_plugin_type(pgm_id,m_guid)==KX_OBJECT_DANE)
		                   {
		                       char tmp_name[MAX_PATH];
		                       parent->pm_t->get_plugin_file(pgm_id,m_guid,tmp_name);
                                       manager->launch(LAUNCH_EDITOR,-1,pgm_id,(uintptr_t)(LPCTSTR)tmp_name);
                                   }
                                 }
                        	}
                        	return;
			case 13: // assign
				{
					manager->launch(LAUNCH_MIDI);
					if(midi_window)
                                         midi_window->select_plugin(plugin);
				}
				return;
                        case 16: // import
                          	{
                                CFileDialog *f_d = new CFileDialog(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_EXPLORER,
                                  "kX Plugin Presets (*.kxp)|*.kxp||",this);
                                if(f_d)
                                {
                                	restore_cwd("kxp");

                                	char tmp_cwd[MAX_PATH];
                                	GetCurrentDirectory(sizeof(tmp_cwd),tmp_cwd);
                                	f_d->m_ofn.lpstrInitialDir=tmp_cwd;

                                	if(f_d->DoModal()==IDCANCEL)
                                	{
                                	 delete f_d;
                                	 break;
                                	}

                                	save_cwd("kxp");

                                	kString fname=f_d->GetPathName();
                                	delete f_d;

                                	if(fname.GetLength()==0) // cancel or empty
                                	{
                                	 break;
                                	}

                                	if(plugin)
                                	 plugin->import_presets((TCHAR *)(LPCTSTR)fname);
                                }
                                }
                                break;

                        case 15: // export
                          	{

                          	char tmp_name[MAX_PATH]; tmp_name[0]=0;

                          	if(plugin)
                          	{
                          	 const char *pn=plugin->get_plugin_description(IKX_PLUGIN_NAME);
                          	 if(pn)
                          	 {
                          	  strncpy(tmp_name,pn,MAX_PATH);
                          	  strcat(tmp_name,".kxp");
                          	 }

                                CFileDialog *f_d = new CFileDialog(FALSE,NULL,tmp_name,OFN_HIDEREADONLY|OFN_EXPLORER|OFN_OVERWRITEPROMPT,
                                  "kX Plugin Presets (*.kxp)|*.kxp||",this);
                                if(f_d)
                                {
                                	restore_cwd("kxp");

                                	char tmp_cwd[MAX_PATH];
                                	GetCurrentDirectory(sizeof(tmp_cwd),tmp_cwd);
                                	f_d->m_ofn.lpstrInitialDir=tmp_cwd;

                                	if(f_d->DoModal()==IDCANCEL)
                                	{
                                	 delete f_d;
                                	 break;
                                	}

                                	save_cwd("kxp");

                                	kString fname=f_d->GetPathName();
                                	delete f_d;

                                	if(fname.GetLength()==0) // cancel or empty
                                	{
                                	 break;
                                	}

                                        strncpy(tmp_name,(LPCTSTR)fname,MAX_PATH);
                                        if(strstr(tmp_name,".kxp")==0)
                                         strcat(tmp_name,".kxp");
                                	plugin->export_presets(tmp_name);
                                }
                                }
                                }
                                break;
                        case 17: // restore
                        	{
                        	 dword t=0;
                        	 if(parent->ikx_t->get_microcode_flag(pgm_id,&t)==0)
                        	  parent->ikx_t->set_microcode_flag(pgm_id,t&(~MICROCODE_MINIMIZED));

                        	 parent->redraw_window();
                        	}
                        	break;
                        case 18: // minimize
                        	{
                        	 dword t=0;
                        	 if(parent->ikx_t->get_microcode_flag(pgm_id,&t)==0)
                        	  parent->ikx_t->set_microcode_flag(pgm_id,t|MICROCODE_MINIMIZED);

                        	 parent->redraw_window();
                        	}
                        	break;
			default:
			    if(ret>=MENU_PLUGIN_BASE) // 500
			    {
                                  // enumerate registered effects here...
                                  kSettings cfg;

                                  iKXPlugin *plg=0;

                                  ret=parent->pm_t->load_plugin(ret,&plg);

                                  if(ret<-10)
                                  {
                                         switch(MessageBox(mf.get_profile("errors","instant"),"kX Mixer",MB_OK|MB_ICONEXCLAMATION|MB_YESNO))
                                         {
                                          case IDYES:
                                            parent->pm_t->unregister_plugin(ret);
                                            break;
                                         }
                                  }
                                   else
                                  {
                                         if(ret==0)
                                         {
                                          if(plg)
                                          {
                                            // get current x/y
                                            int *pl_x=0,*pl_y=0;
                                            parent->pm_t->find_plugin_ex(pgm_id,m_guid,&pl_x,&pl_y);
                                                                                        
                                            parent->pm_t->move_plugin_window(plg->pgm_id,*pl_x,*pl_y);

                                            debug("kxmixer: FIXME save connections here\n");

                                            // unload preset plugin...
                                            if(plugin) 
                                            {
                                              // event is sent by unload_plugin
                                              parent->pm_t->unload_plugin(plugin);
                                              plugin=0; 
                                            }
                                            parent->ikx_t->unload_microcode(pgm_id); 
                                            pgm_id=-1; 

                                            // translate new one
                                            if(parent->pm_t->perform_translate(parent,plg)==0)
                                            {
                                            	// enable
                                            	parent->ikx_t->enable_microcode(plg->pgm_id); 
                                            	plg->event(IKX_ENABLE);

                                            	debug("kxmixer: FIXME restore connections here\n");
                                            }

                                            // do not call 'redraw_window' directly, since
                                            // it will destroy (*this)
                                            ::PostMessage(parent->get_wnd(),WM_COMMAND,IDg_REDRAW_KXDSP,0);
                                           }
                                           else
                                           {
                                             // plg=pl->plugin;
                                             debug("well, a very bad thing here...\n");
                                             // FIXME NOW
                                             ASSERT((plg==NULL));
                                           }
                                         } else debug("kxmixer: failed to upload plugin [error message displayed]: %d\n",ret);
                                  } // ret
			    }
			    else
			     // IKXPLUGIN_PRESETS_CUSTOM=100
                            if((ret>=IKXPLUGIN_PRESETS_CUSTOM)&&(ret<(IKXPLUGIN_PRESETS_BUILTIN+IKXPLUGIN_MAX_PRESETS)))
                            {
                              if(plugin)
                               if(plugin->apply_preset(ret)==0)
                               {
                                 // sync();
                               }
                               // update_preset_name();
                            }
                            break;
                }
        }
}

char *CKxCode::set_tooltip()
{
    static char tip[KX_MAX_STRING*4];

    kPoint point;
    GetCursorPos(point);
    ScreenToClient(point);

    kPoint pt=point;

    // check, if it is in or out
    int inout=0;
    int gpr_num=-1;

    kRect rect;
    GetClientRect(rect);

    if(plugin && plugin->dsp_wnd)
    {
     if(plugin->dsp_wnd->find_gpr(&pt,&gpr_num,flag,tip)==0)
     {
        	inout=1;
     }
    }

 if(inout && (gpr_num>=0))
 {
  return (char *)(LPCTSTR)tip;
 }
 else
  return (char *)(LPCTSTR)def_tooltip;
}

void CKxCode::on_mouse_l_dbl(kPoint ,int )
{
 if(plugin)
  parent->pm_t->tweak_plugin(pgm_id);
}

int CKxCode::on_command(int wParam, int lParam)
{
 if(wParam==IDg_MINIMAX)
 {
  dword t=0;
  if(parent->ikx_t->get_microcode_flag(pgm_id,&t)==0)
  {
   if(parent->ikx_t->set_microcode_flag(pgm_id,t^MICROCODE_MINIMIZED)==0)
    ::PostMessage(parent->get_wnd(),WM_COMMAND,IDg_REDRAW_KXDSP,0);
    // cannot call parent->redraw_window() directly...
  }

  return 0;
 }

 return kDialog::on_command(wParam,lParam);
}
