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

#include "cmaxafx.h"

#include "main_dlg.h"
#include "edit_dlg.h"
#include "kxcode.h"
#include "kxdspDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern CMainDialog *mixer_window;

/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CKxdspDlg, CKXDialog)
        ON_NOTIFY_EX( TTN_NEEDTEXT, 0, OnToolTipNotify )
END_MESSAGE_MAP()

void CKxdspDlg::init()
{
        for(int i=0;i<MAX_PATCHES;i++)
        {
        	patches[i]=NULL;
        }

	create_tooltip(TTS_ALWAYSTIP);

	kSettings cfg;
	dsp_conn_size=-1;
	dsp_wnd_size=-1;

	cfg.read_abs("Mixer","DSPConnSize",&dsp_conn_size);
	cfg.read_abs("Mixer","DSPWindowSize",&dsp_wnd_size);

        // create and resize
	resize_dsp_window();
}

void CKxdspDlg::resize_dsp_window()
{
	// save current position & hide window

	save_position();
	hide();

	generic_init("dsp",(dsp_wnd_size==-1)?"background":
	 (dsp_wnd_size==0)?"background_x":"background_sm");
	create_status();
	select_device();

	show();
}

void CKxdspDlg::create_status(void)
{
        kColor fg=0xffffff;
        kColor bk=0xb0b000;
        kRect r; r.left=r.top=r.bottom=r.right=0;

        char tmp_str[256];

        if(status.m_hWnd) status.destroy();

        if(!mf.get_profile("dsp","status_line",tmp_str,sizeof(tmp_str)))
        {
           // x,y,wd,ht
           sscanf(tmp_str,"%d %d %d %d %x %x",&r.left,&r.top,&r.right,&r.bottom,&fg,&bk);
        }
        status.create("",r.left,r.top,r.right,r.bottom,this,fg,bk);
        status.show();
}

int CKxdspDlg::select_device(int dev_)
{
	CKXDialog::select_device(dev_);

	// first: cleanup
        for(int i=0;i<MAX_PATCHES;i++)
        {
         if(patches[i])
         {
          patches[i]->DestroyWindow();
          delete patches[i];
          patches[i]=0;
         }
        }

        device_init("dsp");

        SetWindowText(mf.get_profile("lang","dsp.name")+" - "+ikx_t->get_device_name());

        redraw_window();
        // initial window placement
        RedrawWindow();

        return 0;
}

int CKxdspDlg::redraw_window(void)
{
 // create all microcode windows
 dword i;
 int x=40,y=10;

 int size;
 dsp_microcode *mc=NULL;
 size=ikx_t->enum_microcode(mc,0);
 if(size>0)
 {
  mc=(dsp_microcode *)malloc(size);
  if(mc)
  {
   if(ikx_t->enum_microcode(mc,size)==0)
   {
        for(i=0;i<size/sizeof(dsp_microcode);i++) // unefficient! FIXME
        {
        	  // find existing patch
        	  int found=0;
                  int j;
        	  for(j=0;j<MAX_PATCHES;j++)
        	  {
                	   if(patches[j])
                	   {
                	    if(patches[j]->pgm_id==mc[i].pgm)
                	    { found=1; break; }
                	   }
        	  }
        	  if(!found) // not found: add new window
        	  {
                	     // find free place
                	     int num=-1;
                	     for(j=0;j<MAX_PATCHES;j++)
                	     {
                	      if(patches[j]==NULL)
                	      { num=j; break; }
                	     }
                	     if(num==-1)
                	     {
                	     	MessageBox(mf.get_profile("errors","patches"));
                	     	return 0;
                	     }
                	     patches[num]=new CKxCode();

                	     int pos_x,pos_y;

                             // microcode was loaded, but there was no window for it:
                             // try to get kxDSP window position (stored)
                             int *pl_x=0,*pl_y=0;
                             if(pm_t->find_plugin_ex(mc[i].pgm,mc[i].guid,&pl_x,&pl_y)
                              && (pl_x) && (*pl_x!=-1) && (pl_y) && (*pl_y!=-1)) // found
                             {
                              pos_x=*pl_x; pos_y=*pl_y;
                             }
                             else
                             {
                              pos_x=x; pos_y=y;
                             }

                             if(patches[num]->create(pos_x,pos_y,mc[i].pgm,this)==TRUE)
                             {
                              patches[num]->show();
                              kRect r;
                              patches[num]->get_rect_abs(r);

                              x+=(r.right-r.left)+10;
                              if(x>get_bitmap_width()-(r.right-r.left)) { x=40; y+=(r.bottom-r.top)+10; }
                              if(y>get_bitmap_height()-(r.bottom-r.top)) { x=40; y=10; }
                             }

                             if(patches[num]->plugin && patches[num]->plugin->dsp_wnd)
                               patches[num]->plugin->dsp_wnd->set_connector_size(dsp_conn_size);
        	  } 
         } // for
   } // enum ok
   free(mc);
  } // malloc ok
 } // size>0

        pm_t->update_plugins(1); // unloads unnecessary plugins

        // status line
        int instr=0,gprs=0,itram_gprs=0,xtram_gprs=0,itramsize=0,xtramsize=0;

        for(i=0;i<MAX_PATCHES;i++)
        {
         if(patches[i])
         {
          if(patches[i]->refresh()==FALSE) // delete all affected windows
          {
           patches[i]->ShowWindow(SW_HIDE);
           patches[i]->DestroyWindow();
           delete patches[i];
           patches[i]=0;
          }
          else
          {
           if(patches[i]->flag&MICROCODE_TRANSLATED)
           {
             itramsize+=patches[i]->itramsize;
             xtramsize+=patches[i]->xtramsize;
             instr+=patches[i]->code_size/sizeof(dsp_code);

             for(dword j=0;j<patches[i]->info_size/sizeof(dsp_register_info);j++)
             {
              if((patches[i]->info[j].type&GPR_MASK)==GPR_ITRAM)
               itram_gprs++;
               else
              if((patches[i]->info[j].type&GPR_MASK)==GPR_XTRAM)
               xtram_gprs++;
               else
              if((patches[i]->info[j].type&GPR_MASK)!=GPR_INPUT &&
                 (patches[i]->info[j].type&GPR_MASK)!=GPR_TRAMA) 
                {
                 // check if they are physicals
                 if(patches[i]->info[j].num<KX_CONST ||
                    patches[i]->info[j].num>KX_DSPLAST)
                   gprs++;
                }
             }
           }
          }
         }
        }

        int buf_size=0;
        ikx_t->get_buffers(KX_TANKMEM_BUFFER,&buf_size);

        kString tmp;
        tmp.Format((LPCTSTR)mf.get_profile("lang","dsp.status_text"),
          instr,ikx_t->get_dsp()?E10K2_MAX_INSTRUCTIONS:E10K1_MAX_INSTRUCTIONS,
          gprs,ikx_t->get_dsp()?0x200:0x100,
          itram_gprs,ikx_t->get_dsp()?192:128,
          xtram_gprs,ikx_t->get_dsp()?64:32,
          itramsize,8192,
          xtramsize*2/1024,buf_size/1024);

        status.SetWindowText(tmp);

        RedrawWindow();
        return 1;
}

void CKxdspDlg::on_destroy()
{
 for(int i=0;i<MAX_PATCHES;i++)
 {
  if(patches[i])
  {
   patches[i]->DestroyWindow();
   delete patches[i];
   patches[i]=0;
  }
 }
 if(card_name.m_hWnd) card_name.DestroyWindow();

 CKXDialog::on_destroy();
}

int CKxdspDlg::draw(kDraw &d)
{
                HDC tmp_dc=d.get_dc();
		
		kRect rr;
              	get_rect(rr);

                HDC dc=CreateCompatibleDC(tmp_dc);
                HBITMAP memBM = CreateCompatibleBitmap(tmp_dc,rr.width(),rr.height());
                HBITMAP prevBM = (HBITMAP)SelectObject(dc,memBM);

              if(!(k_method&kMETHOD_TILED) || (get_bitmap_width()==rr.width() && get_bitmap_height()==rr.height()))
              {
         		BitBlt(dc,
                          	0, 0,
                  	        get_bitmap_width(),
                  	        get_bitmap_height(),
                  	        get_dialog_dc(),
                  	        0, 0,
         			SRCCOPY);
              }
              else
              {
              		for(int x=0;x<rr.right;x+=get_bitmap_width())
              		 for(int y=0;y<rr.bottom;y+=get_bitmap_height())
              		 {
                                BitBlt(dc,
                                	x, y,
                                        get_bitmap_width(),
                                        get_bitmap_height(),
                                        get_dialog_dc(),
                                        0, 0,
                                       SRCCOPY); 
              		 }
              }


                // redraw all children: dc
                for(int i=0;i<MAX_PATCHES;i++)
                {
                 if(patches[i])
                 {
                  RECT r;
                  patches[i]->GetWindowRect(&r);
                  ScreenToClient(&r);
                  patches[i]->do_paint(dc,r.left,r.top);
                 }
                }

                // draw all the links

                    HPEN c_p;
                    HPEN prev_p;
 
                    dword cc_link=0xffff00;

                    char tmp_str[128];

                    if(!mf.get_profile("dsp","link_color",tmp_str,sizeof(tmp_str)))
                    {
                     sscanf(tmp_str,"%x",&cc_link);
                    }

                    c_p=CreatePen(PS_SOLID,0,cc_link);
                    prev_p=(HPEN)SelectObject(dc,c_p);

                    for(int source=0;source<MAX_PATCHES;source++)
                    {
                     if(patches[source] && (patches[source]->pgm_id!=-1))
                     {
                        int n_outs=0;
                        for(dword output=0;output<patches[source]->info_size/sizeof(dsp_register_info);output++)
                        {
                             if((patches[source]->info[output].type&GPR_MASK)==GPR_OUTPUT)
                             {
                              n_outs++;
                              for(int target=0;target<MAX_PATCHES;target++)
                              {
                               if(patches[target] && (patches[target]->pgm_id!=-1))
                               {
                                int n_ins=0;
                                for(dword input=0;input<patches[target]->info_size/sizeof(dsp_register_info);input++)
                                {
                                 if((patches[target]->info) && (patches[target]->info[input].type&GPR_MASK)==GPR_INPUT &&
                                    (!(patches[target]->flag&MICROCODE_HIDDEN)) &&
                                    (!(patches[source]->flag&MICROCODE_HIDDEN)) )
                                 {
                                    n_ins++;
                                    if((patches[source]->info[output].translated!=DSP_REG_NOT_TRANSLATED)
                                    && (patches[source]->info[output].translated==
                                     patches[target]->info[input].translated))
                                    {   
                                     RECT src,tgt;
                                     patches[target]->GetWindowRect(&tgt);
                                     ScreenToClient(&tgt);
                                     patches[source]->GetWindowRect(&src);
                                     ScreenToClient(&src);

                                     kRect r_s,r_t;

				     if(patches[source]->plugin)
				     {
                                      if(patches[source]->plugin->dsp_wnd)
                                      {
                                       patches[source]->plugin->dsp_wnd->
                                         get_connection_rect(n_outs,1,patches[source]->flag,&r_s);
                                      }
                                       else break;
                                     }

                                     if(patches[target]->plugin)
                                     {
                                      if(patches[target]->plugin->dsp_wnd)
                                      {
                                       patches[target]->plugin->dsp_wnd->
                                         get_connection_rect(n_ins,0,patches[target]->flag,&r_t);
                                      } 
                                      else break;
                                     }

                                     MoveToEx(dc,src.left+(r_s.left+r_s.right)/2,
                                       src.top+(r_s.top+r_s.bottom)/2,NULL);
                                     LineTo(dc,tgt.left+(r_t.left+r_t.right)/2,
                                       tgt.top+(r_t.top+r_t.bottom)/2);
                                    }
                                 }
                                }
                               }
                              }
                             }
                        }
                     }
                    }
                    SelectObject(dc,prev_p);
                    DeleteObject(c_p);

                BitBlt(tmp_dc,
                      	0, 0,
              	        rr.width(),
              	        rr.height(),
              	        dc,
              	        0, 0,
     			SRCCOPY);
     		SelectObject(dc,prevBM);
     		DeleteObject(memBM);
                DeleteDC(dc);

                return 0;
}

void CKxdspDlg::on_mouse_r_up(kPoint p,int )
{
	kSettings cfg;

	SetForegroundWindow();

	// create context menu
	kMenu menu;

	menu.create();

        kMenu plugin_menu;
        plugin_menu.create();

        pm_t->create_plugins_menu(&plugin_menu);

        menu.add(mf.get_profile("menu","eff_list"),&plugin_menu);
        menu.add(mf.get_profile("menu","refresh"),2);

        menu.separator();
        menu.add(mf.get_profile("menu","register_plugin"),4);

        menu.separator();

        menu.add(mf.get_profile("lang","dsp.realign"),8);
        menu.add(mf.get_profile("menu","reset_dsp"),1);
        menu.add(mf.get_profile("menu","clear_dsp"),7);

        menu.separator();
        menu.add(mf.get_profile("lang","dsp_plugin.minimize_all"),10);
        menu.add(mf.get_profile("lang","dsp_plugin.restore_all"),11);

        kMenu dsp_sz_menu; dsp_sz_menu.create();
        kMenu conn_sz_menu; conn_sz_menu.create();

        dsp_sz_menu.add(mf.get_profile("lang","dsp.default"),12);
        dsp_sz_menu.add(mf.get_profile("lang","dsp.large"),13);
        dsp_sz_menu.add(mf.get_profile("lang","dsp.small"),14);

        conn_sz_menu.add(mf.get_profile("lang","dsp.default"),15);
        conn_sz_menu.add(mf.get_profile("lang","dsp.large"),16);
        conn_sz_menu.add(mf.get_profile("lang","dsp.huge"),17);

        menu.add(mf.get_profile("lang","dsp.window_size"),&dsp_sz_menu);
        menu.add(mf.get_profile("lang","dsp.conn_size"),&conn_sz_menu);


        menu.separator();
        
        {
         dword p=0; cfg.read_abs("Mixer","EnhDSP",&p);
         menu.add(mf.get_profile("lang","translate.option"),9,(p?MF_CHECKED:0));
        }
        {
         dword p=0; cfg.read_abs("Mixer","PlainConn",&p);
         menu.add(mf.get_profile("lang","connect.option"),18,((!p)?MF_CHECKED:0));
        }

        CPoint p_s;
        p_s.x=p.x; p_s.y=p.y;
        ClientToScreen(&p_s); 
        int ret=menu.track(TPM_LEFTBUTTON|TPM_RIGHTBUTTON|TPM_RETURNCMD,
					p_s.x,p_s.y,this);
        switch(ret)
        {
        	case 1: // reset DSP
        		ikx_t->mute();
        		pm_t->close_plugins();
        		ikx_t->reset_microcode();
                        pm_t->init_plugins();
        		pm_t->reset_all_plugin_settings();
        		ikx_t->unmute();
                        manager->save_settings(SETTINGS_AUTO); // will reset plugin info in settings file
                        redraw_window(); 
                        RedrawWindow();
                        redraw_window();
                        if(mixer_window)
                         mixer_window->redraw_window(mixer_window->cur_page);
                        break;
        	case 2: // refresh
        		redraw_window();
        		break;
        	case 4: // register plugin
			{
			pm_t->register_plugin(NULL);
                        }
			return;
		case 9:
			{
				dword p=0; cfg.read_abs("Mixer","EnhDSP",&p);
				if(p)
				 p=0;
				else
				 p=1;

				cfg.write_abs("Mixer","EnhDSP",p);
			}
			return;
    	case 18:
			{
				dword p=0; cfg.read_abs("Mixer","PlainConn",&p);
				if(p)
				 p=0;
				else
				 p=1;

				cfg.write_abs("Mixer","PlainConn",p);
			}
			return;
		case 8: // re-align
			{
			    int num=0;
			    while(1)
			    {
			     plugin_list_t nfo;
			     if(pm_t->describe_plugin(num,&nfo))
			      break;

                             if(nfo.plugin)
                             {
                             	if(pm_t->realign_plugin(num))
                             	{
                             		pm_t->describe_plugin(num,&nfo);
                             		for(int i=0;i<MAX_PATCHES;i++)
                             		{
                             		 if(patches[i] && (patches[i]->pgm_id==nfo.plugin->pgm_id))
                             		 {
                             		 	RECT r;
                             		 	patches[i]->GetWindowRect(&r);
                                                patches[i]->MoveWindow(nfo.x,nfo.y,r.right-r.left,r.bottom-r.top);
                                	 }
                                	}	
                                }
                             }
                             num++;
                            }
                            redraw_window();
                            RedrawWindow();
			}
			return;
	     case 7: // clear
	     	pm_t->clear_dsp();

                redraw_window(); 
                RedrawWindow();
		if(mixer_window)
                   mixer_window->redraw_window(mixer_window->cur_page);
	     	return;
	    case 10: // mini all
	    	{
                     for(int i=0;i<MAX_PATCHES;i++)
                     {
                      dword t=0;

                      if(patches[i] && patches[i]->pgm_id>0)
                      {
                       if(ikx_t->get_microcode_flag(patches[i]->pgm_id,&t)==0)
                        ikx_t->set_microcode_flag(patches[i]->pgm_id,t|MICROCODE_MINIMIZED);
                      }
                     }
                     redraw_window();
	    	}
	    	break;
	    case 11: // rest all
	    	{
                     for(int i=0;i<MAX_PATCHES;i++)
                     {
                      dword t=0;

                      if(patches[i] && patches[i]->pgm_id>0)
                      {
                       if(ikx_t->get_microcode_flag(patches[i]->pgm_id,&t)==0)
                        ikx_t->set_microcode_flag(patches[i]->pgm_id,t&(~MICROCODE_MINIMIZED));
                      }
                     }
                     redraw_window();
	    	}
	    	break;
	    case 12: dsp_wnd_size=-1; break;
	    case 13: dsp_wnd_size=0; break;
	    case 14: dsp_wnd_size=1; break;
	    case 15: dsp_conn_size=-1; break;
	    case 16: dsp_conn_size=0; break;
	    case 17: dsp_conn_size=1; break;
        }

        if(ret>=12 && ret<=17)
        {
	 cfg.write_abs("Mixer","DSPConnSize",dsp_conn_size);
	 cfg.write_abs("Mixer","DSPWindowSize",dsp_wnd_size);

	 resize_dsp_window();
	}

        if(ret>=MENU_PLUGIN_BASE)
        {
          // enumerate registered effects here...
          kSettings cfg;

          iKXPlugin *plg=0;

          ret=pm_t->load_plugin(ret,&plg);

          if(ret<-10)
          {
                 switch(MessageBox(mf.get_profile("errors","instant"),"kX Mixer",MB_OK|MB_ICONEXCLAMATION|MB_YESNO))
                 {
                  case IDYES:
                    pm_t->unregister_plugin(ret);
                    break;
                 }
          }
           else
          {
                 if(ret==0)
                 {
                  if(plg)
                  {
                    pm_t->move_plugin_window(plg->pgm_id,p.x,p.y);

                    // translate
                    if(pm_t->perform_translate(this,plg)==0)
                    {
                    	// enable
                    	ikx_t->enable_microcode(plg->pgm_id); 
                    	plg->event(IKX_ENABLE);
                    }

                    redraw_window();
                   }
                   else
                   {
                     // plg=pl->plugin;
                     debug("well, a very bad thing here...\n");
                     // FIXME NOW
                     ASSERT((plg==NULL));
                   }
                 } else debug("kxmixer: failed to upload plugin [error message displayed]: %d\n",ret);
          }

          redraw_window();
          return;
        }
}


BOOL CKxdspDlg::OnToolTipNotify( UINT id, NMHDR * pNMHDR, LRESULT * pResult )
{
    TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR;
    UINT_PTR nID =pNMHDR->idFrom;
    if (pTTT->uFlags & TTF_IDISHWND)
    {
    	    nID=::GetDlgCtrlID((HWND)nID);

    	    int found=0;
    	    for(int i=0;i<MAX_PATCHES;i++)
    	    {
    	     if(patches[i])
    	      if((patches[i]->GetDlgCtrlID())==(int)nID)
    	      {
    	       if(!(patches[i]->flag&MICROCODE_HIDDEN))
    	       {
    	        pTTT->lpszText=patches[i]->set_tooltip();
    	        found=1;
    	       }
    	      }
    	    }

    	    if(!found)
              pTTT->lpszText="???";

            pTTT->hinst = NULL;
            return(TRUE);
    }
    pTTT->hinst=NULL;
    pTTT->lpszText="???";
    return(TRUE);
}

void CKxdspDlg::invalidate_patches()
{
 for(int i=0;i<MAX_PATCHES;i++)
 {
  if(patches[i])
  {
   patches[i]->plugin=0;
  }
 }
}

int CKxdspDlg::find_pgm_gpr(kPoint *pt,CKxCode **pgm,int *reg,char *tooltip)
{
 for(int i=0;i<MAX_PATCHES;i++)
 {
  if(patches[i])
  {
   kRect w_r;
   patches[i]->get_rect_abs(w_r);
   if(w_r.point_in_rect(*pt))
   {
    *pgm=patches[i];
    kPoint point=*pt;
    patches[i]->ScreenToClient((LPPOINT)&point.x);

    if(patches[i]->plugin && patches[i]->plugin->dsp_wnd)
      return patches[i]->plugin->dsp_wnd->find_gpr(&point,reg,patches[i]->flag,tooltip);
    else
     return -1;
   }
  }
 }
 return -1;
}


int CKxdspDlg::on_command(int wParam, int lParam)
{
 if(wParam==IDg_REDRAW_KXDSP)
 {
  redraw_window();
  return 0;
 }

 return CKXDialog::on_command(wParam,lParam);
}
