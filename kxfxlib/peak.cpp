// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov, 2002-2004.
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

// --------------- the above should be a 'standard' beginning of any .cpp file

// effect class
#include "peak.h"
// effect source
#include "da_peak.cpp"

#define P_RESET	0

int iPeakPlugin::request_microcode() // generally is called after init()
{
	publish_microcode(peak);

	// you should free code and info massives in ::close() if allocated dynamically
	return 0;
}

const char *iPeakPlugin::get_plugin_description(int id)
{
 plugin_description(peak);
}

iKXPluginGUI *iPeakPlugin::create_cp(kDialog *parent,kFile *mf)
{
	iPeakPluginDlg *tmp;
	tmp = new iPeakPluginDlg(this, parent,mf);
	tmp->create();
	return tmp;
}

int iPeakPlugin::get_user_interface()
{
	return IKXPLUGIN_OWNER;
}

/* Plugin UI Implementation */

BEGIN_MESSAGE_MAP(iPeakPluginDlg, CKXPluginGUI)
      ON_WM_TIMER()
END_MESSAGE_MAP()

iPeakPluginDlg::iPeakPluginDlg(iPeakPlugin *plg, kDialog *parent_,kFile *mf_) 
 : CKXPluginGUI(parent_,mf_) 
{ 
	  plugin=plg;

          timer_id=(UINT_PTR)-1;
          mode=1;
          max_l=-120;
          max_r=-120;

          peak=NULL;

          // defaults: found in the skin file, too
          pm_offset=12;
          pm_width=460;
          pm_min=-121.3f;
          pm_max=6.0f;

          pm_voffset=12;
          pm_height=460;
          pm_vmin=-121.3f;
          pm_vmax=6.0f;
}


iPeakPluginDlg::~iPeakPluginDlg()
{
		if(peak)
		{
		 peak->destroy();
		 delete peak;
		 peak=0;
		}
         	turn_on(0);
}


int iPeakPluginDlg::turn_on(int what)
{
 if((what==1) && (timer_id==-1))
 {
  timer_id=SetTimer(4321+plugin->pgm_id,70,NULL);
  						// FIXME: timer ID should be unique
  						// perhaps, we will need a better code 
  						// to handle this
  if(timer_id==0)
   timer_id=(UINT_PTR)-1;
 }
 else
 {
	if(timer_id!=(UINT_PTR)-1)
	{
	 KillTimer(timer_id);
	 timer_id=(UINT_PTR)-1;
	}
 }
 return 0;
}


void iPeakPluginDlg::init()
{
	CKXPluginGUI::init();

	peak=new kPeak;

	if(mode==0)
	 peak->set_bitmaps(mf.load_image(mf.get_profile("spectrum","level")),
	                 mf.load_image(mf.get_profile("spectrum","level_high")));
	else
	 peak->set_bitmaps(mf.load_image(mf.get_profile("spectrum","vlevel")),
	                 mf.load_image(mf.get_profile("spectrum","vlevel_high")));

	char buf[128];
	if(mf.get_profile("spectrum","offset",buf,sizeof(buf))==0)
	{
	 sscanf(buf,"%d",&pm_offset);
	}
	if(mf.get_profile("spectrum","width",buf,sizeof(buf))==0)
	{
	 sscanf(buf,"%d",&pm_width);
	}
	if(mf.get_profile("spectrum","min_level",buf,sizeof(buf))==0)
	{
	 sscanf(buf,"%f",&pm_min);
	 pm_min/=10.0f;
	}
	if(mf.get_profile("spectrum","max_level",buf,sizeof(buf))==0)
	{
	 sscanf(buf,"%f",&pm_max);
	 pm_max/=10.0f;
	}
	if(mf.get_profile("spectrum","voffset",buf,sizeof(buf))==0)
	{
	 sscanf(buf,"%d",&pm_voffset);
	}
	if(mf.get_profile("spectrum","height",buf,sizeof(buf))==0)
	{
	 sscanf(buf,"%d",&pm_height);
	}
	if(mf.get_profile("spectrum","vmin_level",buf,sizeof(buf))==0)
	{
	 sscanf(buf,"%f",&pm_vmin);
	 pm_vmin/=10.0f;
	}
	if(mf.get_profile("spectrum","vmax_level",buf,sizeof(buf))==0)
	{
	 sscanf(buf,"%f",&pm_vmax);
	 pm_vmax/=10.0f;
	}

	// custom dialog initialization here...
	// MAKEUP_DLG(Caption, Width, Height,ext_height)
	if(mode==0)
	 create_dialog(peak_name, peak->get_peak_width()+10, peak->get_peak_height()+20,0,KXFX_NO_RESET); // force ext_height=0; no_reset=1
	else
	 create_dialog(peak_name, peak->get_peak_width()+10, peak->get_peak_height()+20,0,KXFX_NO_RESET|KXFX_NO_RESIZE); // force ext_height=0; no_reset=1

	peak->create(peak_name,align_x,align_y,this,mode);
	peak->show();

        // create 'reset' button
        create_button(p_reset,P_RESET,"reset",0,peak->get_peak_height(),mode==0?40:peak->get_peak_width());

        // remove 'group' control:
        grpbox.hide();

	// add all GUI controls
	turn_on(1);

	redraw();
}

void iPeakPluginDlg::OnTimer(UINT_PTR)
{
	// recalc
        dword left,right;
        plugin->get_dsp_register("result_l",&left);
        plugin->get_dsp_register("result_r",&right);
        plugin->set_dsp_register("result_l",0);
        plugin->set_dsp_register("result_r",0);

        #define UNITY_GAIN_COEFF 0x78000000L // level is 0.25
        #define MAGIC_SHIFT    24

	float left_f = ((float)(long)(left-0x78000000L)/16777216.0f+(float)(long)(left-0x78000000L)/33554432.0f);
        float right_f = ((float)(long)(right-0x78000000L)/16777216.0f+(float)(long)(right-0x78000000L)/33554432.0f);

        if(left_f<-120.0) left_f=-120.0; // cannot use pm_min, since '-120.0dB' is used elsewhere
        if(right_f<-120.0) right_f=-120.0;

        if(left_f>pm_max) left_f=pm_max;
        if(right_f>pm_max) right_f=pm_max;

        if(left_f>max_l)
         max_l=left_f;
        if(right_f>max_r)
         max_r=right_f;

        if(mode==0)
        {
        // level is -115,3 .. +6(max) = |121.3| (min)
        // width is 460 (total is 481)
        // [-- stored in the skin file]

        int separator_l=pm_offset+pm_width-(int) ((float)pm_width*((left_f-pm_max))/pm_min);
        int separator_r=pm_offset+pm_width-(int) ((float)pm_width*((right_f-pm_max))/pm_min);

        peak->set_value(separator_l,separator_r,0,0);
        peak->redraw();

        char final[128];
        char temp[32];

        strcpy(final,"      L ");

        if(left_f!=-120)
         sprintf(temp," %+ 06.1f dB    ",left_f);
        else
         sprintf(temp,"  -infinity    ");

        strcat(final,temp);
        strcat(final,"R ");

        if(right_f!=-120)
         sprintf(temp," %+ 06.1f dB    ",right_f);
        else
         sprintf(temp," -infinity     ");

        strcat(final,temp);
        strcat(final,"  |      L ");

        if(max_l!=-120)
         sprintf(temp," %+ 03.1f dB    ",max_l);
        else
         sprintf(temp," -infinity     ");

        strcat(final,temp);
        strcat(final,"R ");

        if(max_r!=-120)
         sprintf(temp," %+ 03.1f dB    ",max_r);
        else
         sprintf(temp," -infinity     ");

        strcat(final,temp);

        HDC dc=::GetDC(m_hWnd);
         SetTextColor(dc,0x0);
         SetTextAlign(dc,TA_RIGHT);
         HFONT t=(HFONT)SelectObject(dc,(HFONT)font);
         ExtTextOut(dc,peak->get_peak_width()+align_x, peak->get_peak_height()+align_y, ETO_CLIPPED|ETO_OPAQUE,NULL,final,(UINT)strlen(final),NULL);
         SelectObject(dc,t);
        ::ReleaseDC(m_hWnd,dc);
       }
       else
       {
        int separator_l=pm_voffset+(int) ((float)pm_height*((left_f-pm_vmax))/pm_vmin);
        int separator_r=pm_voffset+(int) ((float)pm_height*((right_f-pm_vmax))/pm_vmin);

        char tmp[32];
        char tmp1[32];
        char tmp2[32];
        if(max_l!=-120)
         sprintf(tmp1,"%03.1fdB",max_l);
        else
         sprintf(tmp1,"-Inf ");

        if(max_r!=-120)
         sprintf(tmp2,"%03.1fdB",max_r);
        else
         sprintf(tmp2,"-Inf ");

        sprintf(tmp,"%s %s",tmp1,tmp2);

        peak->set_value(separator_l,separator_r,0,0);
        peak->redraw();

        HDC dc=::GetDC(m_hWnd);
         SetTextColor(dc,0x0);
         SetTextAlign(dc,TA_CENTER);
         HFONT t=(HFONT)SelectObject(dc,(HFONT)font);
         ExtTextOut(dc,peak->get_peak_width()/2+align_x,peak->get_peak_height()-16+align_y, ETO_CLIPPED|ETO_OPAQUE,NULL,tmp,(UINT)strlen(tmp),NULL);
         SelectObject(dc,t);
        ::ReleaseDC(m_hWnd,dc);
       }
}

void iPeakPluginDlg::on_destroy()
{
 turn_on(0);
}

int iPeakPluginDlg::on_command(int lp,int rp)
{
 if(lp-kCONTROL_BASE==P_RESET)
 {
  max_l=-120;
  max_r=-120;
  return 1;
 }
 return CKXPluginGUI::on_command(lp,rp);
}

void iPeakPluginDlg::on_mouse_r_up(kPoint pt,int fl)
{
 kMenu menu;

 menu.create();
 menu.add("Horizontal",100);
 menu.add("Vertical",101);

 POINT pp; pp.x=pt.x; pp.y=pt.y;
 ClientToScreen(&pp); 
 int ret=menu.track(TPM_LEFTBUTTON|TPM_RIGHTBUTTON|TPM_RETURNCMD,
 				pp.x,pp.y,this);

 int old_m=mode;

 switch(ret)
 {
  case 100: mode=0; break;
  case 101: mode=1; break;
 }
 
 if(mode!=old_m)
 {
 	hide();

 	if(peak)
 	{
 	 peak->destroy();
 	 delete peak;
 	 peak=NULL;
 	}

 	peak=new kPeak;

	if(mode==0)
	 peak->set_bitmaps(mf.load_image(mf.get_profile("spectrum","level")),
	                 mf.load_image(mf.get_profile("spectrum","level_high")));
	else
	 peak->set_bitmaps(mf.load_image(mf.get_profile("spectrum","vlevel")),
	                 mf.load_image(mf.get_profile("spectrum","vlevel_high")));

	if(mode==0)
	 resize_dialog(peak_name, peak->get_peak_width()+10, peak->get_peak_height()+20,0,KXFX_NO_RESET); // force ext_height=0; no_reset=1
	else
	 resize_dialog(peak_name, peak->get_peak_width()+10, peak->get_peak_height()+20,0,KXFX_NO_RESET|KXFX_NO_RESIZE); // force ext_height=0; no_reset=1

	peak->create(peak_name,align_x,align_y,this,mode);
	peak->show();

        // create 'reset' button
        if(IsWindow(p_reset))
         p_reset.destroy();

        create_button(p_reset,P_RESET,"reset",0,peak->get_peak_height(),mode==0?40:peak->get_peak_width());

        // remove 'group' control:
        grpbox.hide();

        show();
 }

 CKXPluginGUI::on_mouse_l_up(pt,fl);
}
