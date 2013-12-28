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

#include "stdafx.h"

#undef CDC
#include "gui/kGui.h"

#include "interface/kxplugingui.h"

CKXDSPWindow::CKXDSPWindow(kDialog *parent_,kWindow *that_,iKXPlugin *plugin_,kFile *mf_) :
 iKXDSPWindow(parent_,that_,plugin_,mf_)
{ 
 cc_trans_bk=0x500000;
 cc_trans_pen=0xffff00;
 cc_untrans_bk=0x300000;
 cc_untrans_pen=0xc0;
 cc_enabled_text=0xffffff;
 cc_enabled_text_bk=0xb0b000;
 cc_dis_trans_text=0x000000;
 cc_dis_trans_bk=0x808080;
 cc_dis_untrans_text=0x0;
 cc_dis_untrans_bk=0x808080;
 cc_in_bk=0x0000ff;
 cc_in_pen=0xffff00;
 cc_out_bk=0xff0000;
 cc_out_pen=0xffff00;
 cc_shadow=0x606000;

 conn_size=0;
 header_height=0;
 wnd_ht=wnd_wd=0;
}

int CKXDSPWindow::configure()
{
 if(mf)
 {
        char tmp_str[128];
        if(!mf->get_profile("dsp","cc_translated",tmp_str,sizeof(tmp_str)))
         sscanf(tmp_str,"%x %x",&cc_trans_bk,&cc_trans_pen);
        if(!mf->get_profile("dsp","cc_untranslated",tmp_str,sizeof(tmp_str)))
         sscanf(tmp_str,"%x %x",&cc_untrans_bk,&cc_untrans_pen);
        if(!mf->get_profile("dsp","cc_enabled",tmp_str,sizeof(tmp_str)))
         sscanf(tmp_str,"%x %x",&cc_enabled_text,&cc_enabled_text_bk);
        if(!mf->get_profile("dsp","cc_disabled_translated",tmp_str,sizeof(tmp_str)))
         sscanf(tmp_str,"%x %x",&cc_dis_trans_text,&cc_dis_trans_bk);
        if(!mf->get_profile("dsp","cc_disabled_untranslated",tmp_str,sizeof(tmp_str)))
         sscanf(tmp_str,"%x %x",&cc_dis_untrans_text,&cc_dis_untrans_bk);
        if(!mf->get_profile("dsp","cc_in",tmp_str,sizeof(tmp_str)))
         sscanf(tmp_str,"%x %x",&cc_in_bk,&cc_in_pen);
        if(!mf->get_profile("dsp","cc_out",tmp_str,sizeof(tmp_str)))
         sscanf(tmp_str,"%x %x",&cc_out_bk,&cc_out_pen);
        if(!mf->get_profile("dsp","cc_shadow",tmp_str,sizeof(tmp_str)))
         sscanf(tmp_str,"%x",&cc_shadow);
 }
 return 0;
}


iKXDSPWindow* iKXPlugin::create_dsp_wnd(kDialog *parent_,kWindow *that_,kFile *mf_)
{
 CKXDSPWindow *w=new CKXDSPWindow(parent_,that_,this,mf_);
 return (iKXDSPWindow *)w;
}

int CKXDSPWindow::draw(kDraw *dc,int x,int y,int flag,const char *name,int pgm_id)
{
 kRect rect;
 that->get_rect(rect);

 HBRUSH c_b;
 HPEN c_p;
 HPEN prev_p;
 HBRUSH prev_b;

 c_b=CreateSolidBrush(cc_shadow);
 prev_b=(HBRUSH)SelectObject(dc->get_dc(),c_b);
 c_p=CreatePen(PS_SOLID,0,cc_shadow);
 prev_p=(HPEN)SelectObject(dc->get_dc(),c_p);

 Rectangle(dc->get_dc(),x+3,y+5,x+rect.right+3,y+rect.bottom+5);

 SelectObject(dc->get_dc(),prev_b);
 SelectObject(dc->get_dc(),prev_p);
 DeleteObject(c_b);
 DeleteObject(c_p);

 if(flag&MICROCODE_TRANSLATED) // uploaded 
 {
  c_b=CreateSolidBrush(cc_trans_bk);
  c_p=CreatePen(PS_SOLID,0,cc_trans_pen);
 }
 else // untranslated
 {
  c_b=CreateSolidBrush(cc_untrans_bk);
  c_p=CreatePen(PS_SOLID,0,cc_untrans_pen);
 }

 prev_b=(HBRUSH)SelectObject(dc->get_dc(),c_b);
 prev_p=(HPEN)SelectObject(dc->get_dc(),c_p);
 Rectangle(dc->get_dc(),x,y,x+rect.right,y+rect.bottom);
 if(flag&MICROCODE_ENABLED)
 {
  SetTextColor(dc->get_dc(),cc_enabled_text);
  SetBkColor(dc->get_dc(),cc_enabled_text_bk);
 }
 else
 {
  if(flag&MICROCODE_TRANSLATED)
  {
   SetTextColor(dc->get_dc(),cc_dis_trans_text);
   SetBkColor(dc->get_dc(),cc_dis_trans_bk);
  }
  else
  {
   SetTextColor(dc->get_dc(),cc_dis_untrans_text);
   SetBkColor(dc->get_dc(),cc_dis_untrans_bk);
  }
 }

 HFONT prev_f=0;
 if(parent->get_font()->get_font())
  prev_f=(HFONT)SelectObject(dc->get_dc(),parent->get_font()->get_font());

 ExtTextOut(dc->get_dc(),x+1,y+1,ETO_OPAQUE,NULL,name,(UINT)strlen(name),NULL);

 if((x+rect.right-25>x+3) && (y+rect.bottom-18*2>y))
 {
  char tmp_tmp[16];
  sprintf(tmp_tmp,"[%d]",pgm_id);
  SetTextColor(dc->get_dc(),cc_enabled_text);
  SetBkColor(dc->get_dc(),flag&MICROCODE_TRANSLATED?cc_trans_bk:cc_untrans_bk);
  ExtTextOut(dc->get_dc(),x+rect.right-25,y+rect.bottom-18,ETO_OPAQUE,NULL,tmp_tmp,(UINT)strlen(tmp_tmp),NULL);
 }

 if(prev_f)
  SelectObject(dc->get_dc(),prev_f);
 SelectObject(dc->get_dc(),prev_p);
 SelectObject(dc->get_dc(),prev_b);
 DeleteObject(c_b);
 DeleteObject(c_p);

 c_b=CreateSolidBrush(cc_in_bk);
 c_p=CreatePen(PS_SOLID,0,cc_in_pen);
 prev_b=(HBRUSH)SelectObject(dc->get_dc(),c_b);
 prev_p=(HPEN)SelectObject(dc->get_dc(),c_p);

 if(!(flag&MICROCODE_MINIMIZED))
 {
     for(int j=0;j<n_ins;j++)
     {
      Rectangle(dc->get_dc(),x,y+header_height+2+j*conn_size,
                   x+conn_size*7/10,y+header_height+2+j*conn_size+conn_size*4/10);
     }
 }
  else
   if(n_ins)
    Rectangle(dc->get_dc(),x,y+header_height,
                          x+conn_size*7/10,
                          y+header_height+conn_size*4/10);

 SelectObject(dc->get_dc(),prev_p);
 SelectObject(dc->get_dc(),prev_b);
 DeleteObject(c_b);
 DeleteObject(c_p);

 c_b=CreateSolidBrush(cc_out_bk);
 c_p=CreatePen(PS_SOLID,0,cc_out_pen);
 prev_b=(HBRUSH)SelectObject(dc->get_dc(),c_b);
 prev_p=(HPEN)SelectObject(dc->get_dc(),c_p);

 if(!(flag&MICROCODE_MINIMIZED))
 {
     for(int j=0;j<n_outs;j++)
     {
      Rectangle(dc->get_dc(),x+rect.right,y+header_height+2+j*conn_size,
                   x+rect.right-conn_size*7/10,y+header_height+2+j*conn_size+conn_size*4/10);
     }
 }
  else
   if(n_outs)
    Rectangle(dc->get_dc(),x+rect.right,y+header_height,
                          x+rect.right-conn_size*7/10,
                          y+header_height+conn_size*4/10);
 
 SelectObject(dc->get_dc(),prev_p);
 SelectObject(dc->get_dc(),prev_b);
 DeleteObject(c_b);
 DeleteObject(c_p);

 return 0;
}

int CKXDSPWindow::find_gpr(const kPoint *pt,int *gpr,int flag,char *name)
{
    kRect rect;
    that->get_rect(rect);

    int inout=-1;
    int gpr_num=-1;
    int n=-1;

    if(flag&MICROCODE_MINIMIZED)
    {
     kRect test_rect(0,header_height,conn_size*7/10,header_height+conn_size*4/10);

     if(test_rect.point_in_rect(*pt) && n_ins)
     {
      n=0;
      inout=GPR_INPUT;
     }
     else
     {
      kRect test_rect(rect.right-conn_size*7/10,header_height,
     			      rect.right,header_height+conn_size*4/10);
      if(test_rect.point_in_rect(*pt) && n_outs)
      {
       n=0;
       inout=GPR_OUTPUT;
      }
      else
       return -1;
     }
    }
    else
    {
            int j;
            for(j=0;j<n_ins;j++)
            {
                kRect test_rect;
                test_rect.left=0; test_rect.top=header_height+2+j*conn_size;
                test_rect.right=conn_size*7/10; test_rect.bottom=header_height+2+j*conn_size+conn_size*4/10;
                if(test_rect.point_in_rect(*pt))
                {
                	inout=GPR_INPUT;
                	n=j;
                        break;
                }
            }

            for(j=0;j<n_outs;j++)
            {
                kRect test_rect;
                test_rect.left=rect.right-conn_size*7/10;
                test_rect.top=header_height+2+j*conn_size;
                test_rect.right=rect.right;
                test_rect.bottom=header_height+2+j*conn_size+conn_size*4/10;
                if(test_rect.point_in_rect(*pt))
                {
                	inout=GPR_OUTPUT;
                	n=j;
                	break;
                }
            }
   }

 if(inout!=-1 && n!=-1)
 {
  gpr_num=plugin->get_nth_register(inout,n);

  if(gpr_num!=-1)
  {
   *gpr=gpr_num;
   if(name)
    sprintf(name,"[%x] %s",plugin->info[gpr_num].num,plugin->info[gpr_num].name);

   return 0;
  }
 }
 return -1;
}

int CKXDSPWindow::get_window_rect(char *name,int flag,kSize *sz)
{
        HDC dc=::GetDC(that->get_wnd());
        if(dc)
        {
           if(conn_size==0)
   	    conn_size=11;//*GetSystemMetrics(SM_CXSCREEN)/800;

           HFONT prev_f=0;
           if(parent->get_font()->get_font())
             prev_f=(HFONT)SelectObject(dc,parent->get_font()->get_font());

           char tmp2_str[KX_MAX_STRING+10];
           strcpy(tmp2_str," ");
           strcat(tmp2_str,name);
           strcat(tmp2_str,"   "); // since we have 'minimax' button
           strncpy(name,tmp2_str,KX_MAX_STRING);
           SIZE szz;
           GetTextExtentPoint32(dc,name,(UINT)strlen(name),&szz);
           sz->cx=szz.cx;
           sz->cy=szz.cy;
           if(prev_f)
            SelectObject(dc,prev_f);

           ::ReleaseDC(that->get_wnd(),dc);

           sz->cx+=2;
           header_height=sz->cy+2;
           sz->cy=2+header_height;

           if(!(flag&MICROCODE_MINIMIZED))
            sz->cy+=conn_size*(n_ins>n_outs?n_ins:n_outs);

           wnd_wd=sz->cx;
           wnd_ht=sz->cy;

           return 0;
        }
        return -1;
}

int CKXDSPWindow::get_connection_rect(int io_num,int type,int fl,kRect *pos)
{
 if(fl&MICROCODE_MINIMIZED)
  pos->top=header_height;
 else
  pos->top=header_height+2-conn_size+conn_size*io_num;

 pos->bottom=pos->top+conn_size*4/10;

 if(type==0) // input
 {
  pos->left=0;
  pos->right=0;
 }
 else // output
 {
  pos->left=wnd_wd;
  pos->right=pos->left;
 }
 
 return 0;
}

int CKXDSPWindow::set_connector_size(int sz)
{
 switch(sz)
 {
  case -1: conn_size=11; break;
  case 0: conn_size=15; break;
  case 1: conn_size=20; break;
 }
 return 0;
}
