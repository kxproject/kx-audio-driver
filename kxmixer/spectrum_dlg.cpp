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


// Spectrum.cpp : implementation file
//

#include "stdinc.h"

#include "spectrum_dlg.h"
#include "main_dlg.h"

// #include "kxrmaa.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKXSpectrum

CKXSpectrum::CKXSpectrum()
{
    parent=NULL;
    on=0;
    last=0;
    memset(&prev,0,sizeof(prev));

    step_size=-1;
    step_gap=-1;

    timer_id=(UINT_PTR)-1;

#if 0
    for(int i=0;i<MAX_PEAK_METERS;i++)
     meters[i]=0;

    view=-1;
#endif
}

CKXSpectrum::~CKXSpectrum()
{
    turn_on(0);
}

BOOL CKXSpectrum::create(char *name,int x,int y,int wd, int ht, CMainDialog* pParentWnd)
{
    kRect r(x,y,x+wd,y+ht);

    ASSERT(m_hWnd);

    BOOL ret=kWindow::create(NULL, _T("STATIC"),r,SS_OWNERDRAW,0,pParentWnd);
    parent=pParentWnd;

    set_method(kMETHOD_OWNER);

    if(parent)
    {
        parent->add_tool(name,this,0);
        if(parent->get_font()->get_font())
          SetFont(*parent->get_font());
    }

    return ret;
}

int CKXSpectrum::draw(kDraw &d) 
{
    CRect rect;
    GetWindowRect((LPRECT)rect);

    HDC dc_=d.get_dc();

    HDC dc=CreateCompatibleDC(dc_);
    HBITMAP memBM = CreateCompatibleBitmap(dc_,rect.right-rect.left,rect.bottom-rect.top);
    HBITMAP prevBM = (HBITMAP)SelectObject(dc,memBM);

    {
              HBRUSH c_b;
                  HBRUSH prev_b;

              c_b=CreateSolidBrush(0x400000);
              prev_b=(HBRUSH)SelectObject(dc,c_b);
              Rectangle(dc,0,0,rect.right,rect.bottom);
                  SelectObject(dc,prev_b);
              DeleteObject(c_b);
    }

    rect.bottom-=rect.top;
    rect.right-=rect.left;
    rect.left=0;
    rect.top=0;

    if(step_size==-1)
    {
     char tmp_str[256];
     if(mf.get_profile("page_spectrum","bar_width",tmp_str,sizeof(tmp_str))==0)
      sscanf(tmp_str,"%d",&step_size);
     else
      step_size=7;
    }

    if(step_gap==-1)
    {
     char tmp_str[256];
     if(mf.get_profile("page_spectrum","bar_gap",tmp_str,sizeof(tmp_str))==0)
      sscanf(tmp_str,"%d",&step_gap);
     else
      step_gap=5;
    }


    float ratio=(float)rect.right/(64.0f*step_size);

        int i;

    switch(/*view*/0)
    {
    case 0:
            kx_voice_info vi[64+2];
            parent->ikx_t->get_voice_info(vi);
            for(i=0;i<64;i++)
            {
             int color;
             int len=(rect.bottom*(int)vi[i].cur_vol)/0xffff;
             if(len>rect.bottom)
              len=rect.bottom;

                 int tmp=(rect.bottom-len)/2;
             if(tmp>255)
              tmp=255;

             if(vi[i].usage&VOICE_USAGE_PLAYBACK)
             {
              color=0x0000ff+(tmp<<8);

              if(vi[i].usage&VOICE_STATUS_24BIT)
               color=0x008000;
              else
              {
               if(vi[i].usage&VOICE_STATUS_AC3)
                color=0x990000;
               else
               if(vi[i].usage&VOICE_STATUS_AC3PT)
                color=0xc0c000;
               else
               {
                if(vi[i].usage&VOICE_STATUS_BUFFERED)
                 color=0x990099+(tmp<<8);
               }
              }
             }
             else
              if(vi[i].usage&VOICE_USAGE_MIDI)
               color=0xff0000+(tmp<<8);
              else
               if(vi[i].usage&VOICE_USAGE_RELEASING)
               {
                color=0x00ff00+(tmp<<8);
               }
               else
                if(vi[i].usage&VOICE_USAGE_ASIO)
                 color=0x00ff00+(tmp<<16);
                else
                 color=0xffffff;

              HBRUSH c_b;
              HPEN c_p;
                  HPEN prev_p;
                  HBRUSH prev_b;

              c_b=CreateSolidBrush(0xffff00);
                  c_p=CreatePen(PS_SOLID,0,0xffff00);
              prev_b=(HBRUSH)SelectObject(dc,c_b);
                  prev_p=(HPEN)SelectObject(dc,c_p);
              Rectangle(dc,(int)(i*step_size*ratio),0,(int)(i*step_size*ratio+step_gap*ratio),rect.bottom);
          SelectObject(dc,prev_p);
                  SelectObject(dc,prev_b);
              DeleteObject(c_b);
                  DeleteObject(c_p);

              c_b=CreateSolidBrush(color);
              c_p=CreatePen(PS_SOLID,0,color);
              SelectObject(dc,c_b);
              SelectObject(dc,c_p);
                  Rectangle(dc,(int)(i*step_size*ratio),rect.bottom-len,(int)(i*step_size*ratio+step_gap*ratio),rect.bottom);
          SelectObject(dc,prev_p);
                  SelectObject(dc,prev_b);
              DeleteObject(c_b);
              DeleteObject(c_p);

             if(vi[i].usage&VOICE_USAGE_PLAYBACK)
              color=0x0000ff;
             else
              if(vi[i].usage&VOICE_USAGE_MIDI)
               color=0xff0000;
              else
               if(vi[i].usage&VOICE_USAGE_RELEASING)
               {
                color=0x400000;
               }
               else
                if(vi[i].usage&VOICE_USAGE_ASIO)
                 color=0x00ff00;
                else
                 color=0xffffff;

              c_b=CreateSolidBrush(color);
              c_p=CreatePen(PS_SOLID,0,color);
              SelectObject(dc,c_b);
              SelectObject(dc,c_p);
                  Rectangle(dc,(int)(i*step_size*ratio),0,(int)(i*step_size*ratio+step_gap*ratio),step_gap);
          SelectObject(dc,prev_p);
                  SelectObject(dc,prev_b);
              DeleteObject(c_b);
              DeleteObject(c_p);

              int w=rect.bottom-len-1;
              if(w<0)
               w=0;
              c_p=CreatePen(PS_SOLID,0,0x0);
              SelectObject(dc,c_p);
                  MoveToEx(dc,(int)(i*step_size*ratio),w,NULL);
                  LineTo(dc,(int)(i*step_size*ratio+step_gap*ratio),w);
          SelectObject(dc,prev_p);
              DeleteObject(c_p);
            }
            break;
/*        case 1:   
            {
              if(level==NULL)
              {
                level=mf.load_image(mf.get_profile("spectrum","level"));
                if(level)
                {
                 // create level dc here...
                 BITMAP bmInfo;
                 GetObject(level,sizeof(bmInfo), &bmInfo);
                 level_width=bmInfo.bmWidth;
                 level_height=bmInfo.bmHeight;

                 level_dc=CreateCompatibleDC(NULL);
                 prev_level=(HBITMAP)SelectObject(level_dc,level);
                         if(prev_level==0)
                         {
                          // already used: duplicate bitmap:
                          level2=(HBITMAP)CopyImage(level,IMAGE_BITMAP,0,0,0);
                          prev_level=(HBITMAP)SelectObject(level_dc,level2);
                          level=level2;
                         }
                }
                level_high=mf.load_image(mf.get_profile("spectrum","level_high"));
                if(level_high)
                {
                 level_high_dc=CreateCompatibleDC(NULL);
                 prev_level_high=(HBITMAP)SelectObject(level_high_dc,level_high);
                         if(prev_level_high==0)
                         {
                          // already used: duplicate bitmap:
                          level_high2=(HBITMAP)CopyImage(level_high,IMAGE_BITMAP,0,0,0);
                          prev_level=(HBITMAP)SelectObject(level_dc,level_high2);
                          level_high=level_high2;
                         }
                }
              }
              if((level==NULL)||(level_high==NULL))
               break;

                  dsp_microcode *mc=NULL;
                  dword size=ikx->enum_microcode(mc,0);
          int cnt=0;
          int pos_y=0;
                  if(size>0)
                  {
                   mc=(dsp_microcode *)malloc(size);
                   if(mc)
                   {
                    if(ikx->enum_microcode(mc,size)==0)
                    {
                         for(dword j=0;j<size/sizeof(dsp_microcode);j++)
                         {
                               dword left, right;
                               if( (strcmp(mc[j].guid,"cb8abf40-a6b2-417f-aaa5-3402466e40e8")==NULL) &&
                                   (ikx->get_dsp_register(mc[j].pgm,"result_l",&left)==0) &&
                                   (ikx->get_dsp_register(mc[j].pgm,"result_r",&right)==0) )
                               {
                                   if(cnt>=MAX_PEAK_METERS)
                                    break;

                                           HFONT prev_f=0;
                                           if(parent->get_font()->get_font())
                                            prev_f=(HFONT)SelectObject(dc,parent->get_font()->get_font());

                                   ikx->set_dsp_register(mc[j].pgm,"result_l",0);
                                   ikx->set_dsp_register(mc[j].pgm,"result_r",0);

                                           #define UNITY_GAIN_COEFF 0x78000000L // это для уровня 0.25
                                           #define MAGIC_SHIFT    24

                                           float left_f = ((float)(long)(left-0x78000000L)/16777216.0f+(float)(long)(left-0x78000000L)/33554432.0f);
                                           float right_f = ((float)(long)(right-0x78000000L)/16777216.0f+(float)(long)(right-0x78000000L)/33554432.0f);

                                           int clipping_l=0,clipping_r=0;

                                           left_f=-left_f+2.0f;
                                           right_f=-right_f+2.0f;

                               if(left_f>120) left_f=120;
                               if(right_f>120) right_f=120;

                               if(left_f<2.0) { clipping_l=(int)(left_f*100.0f); left_f=0; }
                               if(right_f<2.0) { clipping_r=(int)(right_f*100.0f); right_f=0; }

                                           int separator_l=(int)((double)level_width*(120.0-(double)left_f)/120.0);
                                           int separator_r=(int)((double)level_width*(120.0-(double)right_f)/120.0);

                                           SIZE sz;
                                           sz.cx=40; sz.cy=20;
                                           char tmp[KX_MAX_STRING+32];
                                           sprintf(tmp,"  -%06.2f -%06.2f -- %s ",left_f-2.0,right_f-2.0,mc[j].name);
                                           GetTextExtentPoint32(dc,tmp,strlen(tmp),&sz);

                                           pos_y=cnt*(level_height+sz.cy+2);

                                           BitBlt(dc,
                                            0, pos_y,
                                                level_width,
                                                level_height,
                                                level_dc,
                                                0, 0,
                                           SRCCOPY);

                                           BitBlt(dc,
                                                0, pos_y,
                                                    separator_l,
                                                    level_height/2,
                                                    level_high_dc,
                                                    0, 0,
                                           SRCCOPY);

                                           BitBlt(dc,
                                                0, level_height/2+pos_y,
                                                    separator_r,
                                                    level_height/2,
                                                    level_high_dc,
                                                    0, level_height/2,
                                           SRCCOPY);

                                           if(clipping_l)
                                           { 
                                              HBRUSH c_b;
                                              HPEN c_p;
                                                  HPEN prev_p;
                                                  HBRUSH prev_b;

                                              c_b=CreateSolidBrush(0xff);
                                                  c_p=CreatePen(PS_SOLID,0,0xff);
                                              prev_b=(HBRUSH)SelectObject(dc,c_b);
                                                  prev_p=(HPEN)SelectObject(dc,c_p);
                                              Rectangle(dc,level_width-7,pos_y+level_height/4-5,
                                                           level_width-2,pos_y+level_height/4+10);
                                          SelectObject(dc,prev_p);
                                                  SelectObject(dc,prev_b);
                                              DeleteObject(c_b);
                                                  DeleteObject(c_p);
                                           }
                                           if(clipping_r)
                                           { 
                                              HBRUSH c_b;
                                              HPEN c_p;
                                                  HPEN prev_p;
                                                  HBRUSH prev_b;

                                              c_b=CreateSolidBrush(0xff);
                                                  c_p=CreatePen(PS_SOLID,0,0xff);
                                              prev_b=(HBRUSH)SelectObject(dc,c_b);
                                                  prev_p=(HPEN)SelectObject(dc,c_p);
                                              Rectangle(dc,level_width-7,pos_y+level_height/2+level_height/4-5,
                                                           level_width-2,pos_y+level_height/2+level_height/4+10);
                                          SelectObject(dc,prev_p);
                                                  SelectObject(dc,prev_b);
                                              DeleteObject(c_b);
                                                  DeleteObject(c_p);
                                           }

                                           SetTextColor(dc,0x0);
                                           SetTextAlign(dc,TA_RIGHT);
                                           ExtTextOut(dc,level_width-10, pos_y+level_height, ETO_CLIPPED,NULL,tmp,strlen(tmp),NULL);

                                           if(prev_f)
                                            SelectObject(dc,prev_f);

                                           cnt++;
                               } // result_l found
                         } // for each pgm
                    } // enum ok
                    free(mc);
                   } // alloc ok
                  } // enum ok

                  if(cnt==0)
                  {
                   SetTextColor(dc,0x0);
                   ExtTextOut(dc,0,level_height/2-16,ETO_CLIPPED,NULL,mf.get_profile("lang","spectrum.help"),strlen(mf.get_profile("spectrum","help")),NULL);
                  }
            }
            break;
*/
        }
        GetWindowRect(rect);

        BitBlt(dc_,
                0, 0,
                rect.right-rect.left,
                rect.bottom-rect.top,
                dc,
                0, 0,
            SRCCOPY);
        SelectObject(dc,prevBM);
        DeleteObject(memBM);
        DeleteDC(dc);

        return 0;
}

void CKXSpectrum::turn_on(int onoff)
{
  on=onoff; 

#if 0  
  if(onoff==0)
  {
   for(int i=0;i<MAX_PEAK_METERS;i++)
     if(meters[i])
     {
      delete meters[i];
      meters[i]=0;
     }
  }
  else
  {
     int num=0;
     int total=0;
     while(1)
     {
       iKXPlugin *plugin=parent->pm_t->enum_plugin(num);
       if(plugin==0)
        break;
       if(strcmp(plugin->get_plugin_description(IKX_PLUGIN_GUID),"cb8abf40-a6b2-417f-aaa5-3402466e40e8")==0)
       {
        meters[total]=plugin->create_cp(parent,&mf);
        meters[total]->show();
        total++;
       }
       num++;
     }
  }
#endif

 if(onoff==1)
 {
  if(timer_id==(UINT_PTR)-1)
  {
   timer_id=SetTimer(1234,70,NULL);
   if(timer_id==0)
    timer_id=(UINT_PTR)-1;
  }
 }
 else
 {
    if(timer_id!=(UINT_PTR)-1)
    {
     KillTimer(timer_id);
     timer_id=(UINT_PTR)-1;
    }
 }
}

BEGIN_MESSAGE_MAP(CKXSpectrum, kWindow)
        ON_WM_TIMER()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKXSpectrum message handlers

void CKXSpectrum::OnTimer(UINT_PTR id)
{
  if(on)
    redraw();

  kWindow::OnTimer(id);  
}

void CKXSpectrum::on_destroy()
{
 turn_on(0);

 kWindow::on_destroy();
}
