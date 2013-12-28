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

#include "notify.h"
#include "notify_gui.h"

int COSDVolume::draw(kDraw &d)
{
	kRect rr;
	get_rect(rr);

	HDC tmp_dc=d.get_dc();
	HDC dc=CreateCompatibleDC(tmp_dc);
	HBITMAP memBM = CreateCompatibleBitmap(tmp_dc,rr.width(),rr.height());
        HBITMAP prevBM = (HBITMAP)SelectObject(dc,memBM);

	CRect r;
	GetClientRect((LPRECT)r);

        HBRUSH c_b;
        HPEN c_p;
        HPEN prev_p;
        HBRUSH prev_b;

        if(osd_icon&ICON_VOLUME)
        {
               c_b=CreateSolidBrush(0x300000);
               prev_b=(HBRUSH)SelectObject(dc,c_b);
               c_p=CreatePen(PS_SOLID,0,0xffffff);
               prev_p=(HPEN)SelectObject(dc,c_p);
                 Rectangle(dc,0,0,r.right,r.bottom);
               SelectObject(dc,prev_b);
               DeleteObject(c_b);
               SelectObject(dc,prev_p);
               DeleteObject(c_p);

               int mute,vol_l,vol_r,volume;
               p->ikx_t->mixer(KX_PROP_GET,KX_MIXER_MASTER_MUTE,0,&mute);
               p->ikx_t->mixer(KX_PROP_GET,KX_MIXER_MASTER,0,&vol_l);
               p->ikx_t->mixer(KX_PROP_GET,KX_MIXER_MASTER,1,&vol_r);

               volume=((vol_l+vol_r)/2)*100/65536;

               // transform coordinates
               r.right-=20;
               r.left=(r.right/100);
               r.left=(r.right+20-(r.left*98))/2;

               int wd=(r.right/100)*2-1;

               c_b=CreateSolidBrush(mute?0xff:0x00ff00);
               prev_b=(HBRUSH)SelectObject(dc,c_b);
               c_p=CreatePen(PS_SOLID,0,0x00);
               prev_p=(HPEN)SelectObject(dc,c_p);

               for(int i=0;i<volume;i++)
               {
                Rectangle(dc,r.left+(r.right/100*i),10,
                	      r.left+(r.right/100*(i+1))-wd,10+50);
               }
               SelectObject(dc,prev_b);
               DeleteObject(c_b);

               c_b=CreateSolidBrush(0xffffff);
               prev_b=(HBRUSH)SelectObject(dc,c_b);

               for(int i=volume;i<99;i++)
               {
                Rectangle(dc,r.left+(r.right/100*i),10,
                	      r.left+(r.right/100*(i+1))-wd,10+50);
               }
               SelectObject(dc,prev_p);
               DeleteObject(c_p);
               SelectObject(dc,prev_b);
               DeleteObject(c_b);
        }
        if(strlen((LPCTSTR)osd_text)>0)
        {
               c_b=CreateSolidBrush(0x600000);
               prev_b=(HBRUSH)SelectObject(dc,c_b);

               c_p=CreatePen(PS_SOLID,0,0xffffff);
               prev_p=(HPEN)SelectObject(dc,c_p);

               SetTextColor(dc,0xffffff);
               SetBkColor(dc,0x900000);
               SetTextAlign(dc,TA_CENTER);

               Rectangle(dc,50,5,r.right-50,r.bottom-5);
               ExtTextOut(dc,r.right/2,r.bottom/2,0,NULL,(LPCTSTR)osd_text,strlen((LPCTSTR)osd_text),NULL);

               SelectObject(dc,prev_b);
               DeleteObject(c_b);
               SelectObject(dc,prev_p);
               DeleteObject(c_p);
        }
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

int COSDIcon::draw(kDraw &d)
{
	HDC dc=d.get_dc();

	CRect r;
	GetClientRect((LPRECT)r);

/*        HBRUSH c_b;
        HPEN c_p;
        HPEN prev_p;
        HBRUSH prev_b;
*/
        GetClientRect(r);

        HBITMAP bm=0;
        if(osd_icon&ICON_AC3_PT_ON)
         bm=images[1];
        if(osd_icon&ICON_AC3_SW_ON)
         bm=images[0];
        if(osd_icon&ICON_SPEAKER)
         bm=images[2];
        if(osd_icon&ICON_SPDIF)
         bm=images[3];
        if(osd_icon&ICON_SPDIF_AC3)
         bm=images[4];

        if(osd_icon&ICON_PLAY)
         bm=images[5];
        if(osd_icon&ICON_STOP)
         bm=images[6];
        if(osd_icon&ICON_PAUSE)
         bm=images[7];
        if(osd_icon&ICON_NEXT)
         bm=images[8];
        if(osd_icon&ICON_PREV)
         bm=images[9];
        if(osd_icon&ICON_EDSP)
         bm=images[10];

        if(bm)
        {
        	HDC mem_dc=CreateCompatibleDC(dc);
        	HBITMAP prev_bm=(HBITMAP)SelectObject(mem_dc,bm);
		BitBlt(dc,0, 0, 50, 50,
		            mem_dc, 0, 0, SRCCOPY);
        	SelectObject(mem_dc,prev_bm);
        	DeleteDC(mem_dc);
        }
        return 0;
}

void COSDIcon::on_timer(int ev)
{
 kSettings cfg;

 switch(ev)
 {
  case OSDICON_TIMER_ID:
        dword curr=GetTickCount();
        if(curr-last>=OSD_TIMER)
        {
        	if(osd_on)
        	{
        	       osd_on=0;
                       // stop OSD here...
                       hide();

                       osd_icon=0;
        	}
        	KillTimer(OSDICON_TIMER_ID);
        }
 }
}

void COSDVolume::on_timer(int ev)
{
 kSettings cfg;

 switch(ev)
 {
/*  case SUBVOL_TIMER_ID:
        if(p->vol_cnt>0)
        {
         int vol_l=0,vol_r=0;
         p->ikx_t->mixer(KX_PROP_GET,KX_MIXER_MASTER,0,&vol_l);
         p->ikx_t->mixer(KX_PROP_GET,KX_MIXER_MASTER,1,&vol_r);
         vol_l+=VOL_STEP*p->dir; if(vol_l<0) vol_l=0; if(vol_l>0xffff) vol_l=0xffff;
         vol_r+=VOL_STEP*p->dir; if(vol_r<0) vol_r=0; if(vol_r>0xffff) vol_r=0xffff;
         p->ikx_t->mixer(KX_PROP_SET,KX_MIXER_MASTER,0,&vol_l);
         p->ikx_t->mixer(KX_PROP_SET,KX_MIXER_MASTER,1,&vol_r);

         p->vol_cnt--;

         redraw();
        }
        else
        {
         KillTimer(SUBVOL_TIMER_ID);
         }
        break;
*/
  case OSDVOL_TIMER_ID:
        dword curr=GetTickCount();
        if(curr-last>=OSD_TIMER)
        {
        	if(osd_on)
        	{
        	       osd_on=0;
                       // stop OSD here...
                       hide();

                       osd_icon=0;
        	}
        	KillTimer(OSDVOL_TIMER_ID);
        }
 }
}

intptr_t COSDIcon::message(int msg,int wp,int lp)
{
 switch(msg)
 {
  case MM_MIM_LONGDATA: return p->callback_long(wp,lp);
  case MM_MIM_DATA: return p->callback_short(wp,lp);
  case WM_TIMER: on_timer(wp); return 0;
  case WM_DISPLAYCHANGE:
         {
                  kRect r(GetSystemMetrics(SM_CXSCREEN)-40-50,GetSystemMetrics(SM_CYSCREEN)-80,GetSystemMetrics(SM_CXSCREEN)-40,GetSystemMetrics(SM_CYSCREEN)-30);
                  reposition(r);
                  return 0;
         }
         break;
 }
 return kWindow::message(msg,wp,lp);
}

intptr_t COSDVolume::message(int msg,int wp,int lp)
{
 switch(msg)
 {
  case WM_TIMER: on_timer(wp); return 0;
  case WM_DISPLAYCHANGE:
         {
                  kRect r(40,GetSystemMetrics(SM_CYSCREEN)-160,GetSystemMetrics(SM_CXSCREEN)-40,GetSystemMetrics(SM_CYSCREEN)-90);
                  reposition(r);
                  return 0;
         }
         break;
 }
 return kWindow::message(msg,wp,lp);
}

void COSDVolume::create(iKXNotifier *p_)
{
        p=p_;

        osd_icon=0;
        osd_text="";
        osd_on=0;
        last=0;

	if(get_wnd()==0)
	{
          kRect r(40,GetSystemMetrics(SM_CYSCREEN)-160,GetSystemMetrics(SM_CXSCREEN)-40,GetSystemMetrics(SM_CYSCREEN)-90);

	  _tcsncpy(k_class_name,get_class_name(),sizeof(k_class_name));
	  k_id=0;
	  k_parent=0;
	  k_method=kMETHOD_OWNER;

          CWnd::CreateEx(WS_EX_TOPMOST|WS_EX_TOOLWINDOW,
                     kRegisterWndClass(CS_DBLCLKS|CS_OWNDC|CS_VREDRAW|CS_HREDRAW,
    			 LoadCursor(NULL,IDC_ARROW),
		    	 (HBRUSH) ::GetSysColorBrush(COLOR_3DFACE),
		    	 AfxGetApp()->LoadIcon(IDR_ICON),
		    	 get_class_name()
    	                ),
    	             "kX Remote", 
	             WS_POPUP,
	             r.left,r.top,r.right-r.left,r.bottom-r.top,
	             NULL,NULL,0);
        }
}

void COSDIcon::create(iKXNotifier *p_)
{
	osd_icon=0;
	osd_on=0;
	last=0;

        // load images here...
        images[0]=mf.load_image("mixer/images/ac3_sw.bmp");
        images[1]=mf.load_image("mixer/images/ac3_pt.bmp");
        images[2]=mf.load_image("mixer/images/speaker.bmp");
        images[3]=mf.load_image("mixer/images/spdif_n.bmp");
        images[4]=mf.load_image("mixer/images/spdif_ac3.bmp");

        images[5]=mf.load_image("mixer/images/play.bmp");
        images[6]=mf.load_image("mixer/images/stop.bmp");
        images[7]=mf.load_image("mixer/images/pause.bmp");
        images[8]=mf.load_image("mixer/images/next.bmp");
        images[9]=mf.load_image("mixer/images/prev.bmp");
        images[10]=mf.load_image("mixer/images/edsp.bmp");

        p=p_;

	if(get_wnd()==0)
	{
	  kRect r(GetSystemMetrics(SM_CXSCREEN)-40-50,GetSystemMetrics(SM_CYSCREEN)-80,GetSystemMetrics(SM_CXSCREEN)-40,GetSystemMetrics(SM_CYSCREEN)-30);

	  _tcsncpy(k_class_name,get_class_name(),sizeof(k_class_name));
	  k_id=0;
	  k_parent=0;
	  k_method=kMETHOD_OWNER;

          CWnd::CreateEx(WS_EX_TOPMOST|WS_EX_TOOLWINDOW,
                     kRegisterWndClass(CS_DBLCLKS|CS_OWNDC|CS_VREDRAW|CS_HREDRAW,
    			 LoadCursor(NULL,IDC_ARROW),
		    	 (HBRUSH) ::GetSysColorBrush(COLOR_3DFACE),
		    	 AfxGetApp()->LoadIcon(IDR_ICON),
		    	 get_class_name()
    	                ),
    	             "kX Remote", 
	             WS_POPUP,
	             r.left,r.top,r.right-r.left,r.bottom-r.top,
	             NULL,NULL,0);
        }
}

void COSDIcon::start(int icon)
{
   osd_icon=icon;
   if(osd_on==0)
   {
   	last=GetTickCount();
   	SetTimer(OSDICON_TIMER_ID,OSD_TIMER,NULL);

   	osd_on=1;

        // start OSD here...
        if(p->osd_enabled && osd_icon)
        {
          ShowWindow(SW_SHOWNA);
        }
   }
   else
   {
        if(p->osd_enabled && osd_icon)
        {
         last=GetTickCount();
         redraw();
        }
   }
}

void COSDVolume::start(int icon,const TCHAR *text)
{
   osd_icon=icon;
   osd_text=text;
   if(osd_on==0)
   {
   	last=GetTickCount();
   	SetTimer(OSDVOL_TIMER_ID,OSD_TIMER,NULL);

   	osd_on=1;

        // start OSD here...
        if(p->osd_enabled && osd_icon)
        {
          ShowWindow(SW_SHOWNA);
        }
   }
   else
   {
        if(p->osd_enabled && osd_icon)
        {
         last=GetTickCount();
         redraw();
        }
   }
}

