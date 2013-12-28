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
#include "hw/8010x.h"
#include "iostatus_dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define UPDATE_FREQ	200

BEGIN_MESSAGE_MAP(CIOStatusDialog, CKXDialog)
      ON_WM_TIMER()
END_MESSAGE_MAP()

void CIOStatusDialog::on_destroy()
{
	if(timer_id!=(UINT_PTR)-1)
	{
	 KillTimer(timer_id);
	 timer_id=(UINT_PTR)-1;
	}
}

void get_bits(char *str,dword stat)
{
 int bits=0,freq=0;

 if(stat&1)
 {
  switch((stat>>1)&0x7)
  {
   case 0: bits=24; break;
   case 4: bits=20; break;
   case 2: bits=22; break;
   case 1: bits=23; break;
   case 5: bits=24; break;
   case 3: bits=21; break;
   default: bits=-24; break;
  }
 }
 else
 {
  switch((stat>>1)&0x7)
  {
   case 4: bits=16; break;
   case 2: bits=18; break;
   case 1: bits=19; break;
   case 5: bits=20; break;
   case 3: bits=17; break;
   default: bits=-20; break;
  }
 }

 if((stat&0xf0)!=0)
 {
  switch((stat&0xf0)>>4)
  {
   case 1: freq=16000; break;
   case 2: freq=0; break;
   case 3: freq=32000; break;
   case 4: freq=12000; break;
   case 5: freq=11025; break;
   case 6: freq=8000; break;
   case 7: freq=0; break;
   case 8: freq=192000; break;
   case 9: freq=24000; break;
   case 10: freq=96000; break;
   case 11: freq=48000; break;
   case 12: freq=176400; break;
   case 13: freq=22050; break;
   case 14: freq=88200; break;
   case 15: freq=44100; break;
  }
 }
 else
  freq=0;

 sprintf(str,"%s%dbits/",
  (bits<0)?"max. ":"",abs(bits));

 char tmp1[32];

 if(freq==0)
  strcat(str,"SR unspec.");
 else
 {
  sprintf(tmp1,"%dHz",freq);
  strcat(str,tmp1);
 }
}

void CIOStatusDialog::OnTimer(UINT_PTR)
{
 kString t;

 dword is_a2=0;
 ikx_t->get_dword(KX_DWORD_IS_A2,(dword *)&is_a2);
 dword is_k2=0;
 ikx_t->get_dword(KX_DWORD_IS_10K2,(dword *)&is_k2);

 kx_spdif_i2s_status st;
 if(!ikx_t->get_spdif_i2s_status(&st))
 {
 	char tmp[256];

 	t+="SPDIF Inputs:  [R-Rate Locked; S-Stream Locked; V-Valid Audio]\r\n";

 	if(st.spdif.channel_status_a!=0xffffffff && st.spdif.channel_status_a!=0)
 	{
   	 sprintf(tmp,"     spdif A: %x Freq: %d %s%s%s",st.spdif.channel_status_a,
   	            (st.spdif.srt_status_a&0xffffff)*1831/10000,
   	            st.spdif.srt_status_a&SRCS_RATELOCKED?" [R]":"",
   	            st.spdif.srt_status_a&SRCS_SPDIFLOCKED?" [S]":"",
   	            st.spdif.srt_status_a&SRCS_VALIDAUDIO?" [V]":"");
         if(is_a2)
         {
         	strcat(tmp," || ");
         	char tmp_x[128];
         	get_bits(tmp_x,st.spdif.channel_status_a_x);
         	strcat(tmp,tmp_x);
         }
   	}
   	else
   	{
   		sprintf(tmp,"     spdif A: unused");
   	}
   	 
   	t+=tmp;
   	t+="\r\n";

   	if(st.spdif.channel_status_b!=0xffffffff && st.spdif.channel_status_b!=0)
   	{
   	 sprintf(tmp,"     spdif B: %x Freq: %d %s%s%s",st.spdif.channel_status_b,
   	            (st.spdif.srt_status_b&0xffffff)*1831/10000,
   	            st.spdif.srt_status_b&SRCS_RATELOCKED?" [R]":"",
   	            st.spdif.srt_status_b&SRCS_SPDIFLOCKED?" [S]":"",
   	            st.spdif.srt_status_b&SRCS_VALIDAUDIO?" [V]":"");
         if(is_a2)
         {
         	char tmp_x[128];
         	get_bits(tmp_x,st.spdif.channel_status_b_x);
         	strcat(tmp," || ");
         	strcat(tmp,tmp_x);
         }
        }
   	else
   	 sprintf(tmp,"     spdif B: unused");

   	t+=tmp;
   	t+="\r\n";

   	if(st.spdif.channel_status_c!=0xffffffff && st.spdif.channel_status_c!=0)
   	{
   	 sprintf(tmp,"     spdif C: %x Freq: %d %s%s%s",st.spdif.channel_status_c,
   	            (st.spdif.srt_status_c&0xffffff)*1831/10000,
   	            st.spdif.srt_status_c&SRCS_RATELOCKED?" [R]":"",
   	            st.spdif.srt_status_c&SRCS_SPDIFLOCKED?" [S]":"",
   	            st.spdif.srt_status_c&SRCS_VALIDAUDIO?" [V]":"");
         if(is_a2)
         {
         	char tmp_x[128];
         	strcat(tmp," || ");
         	get_bits(tmp_x,st.spdif.channel_status_c_x);
         	strcat(tmp,tmp_x);
         }
        }
   	else
   	 sprintf(tmp,"     spdif C: unused or same as B");

   	t+=tmp;
        t+="\r\n";

        if(is_k2)
        {
   	t+="I2S Inputs:";
   	sprintf(tmp,"   (0): %dHz %s (1): %dHz %s (2): %dHz %s\r\n",
   	  (st.i2s.srt_status_0&SRT_ESTSAMPLERATE)*375/2048,
   	  (st.i2s.srt_status_0&SRT_RATELOCKED)?"[R]":"(!)",
   	  (st.i2s.srt_status_1&SRT_ESTSAMPLERATE)*375/2048,
   	  (st.i2s.srt_status_1&SRT_RATELOCKED)?"[R]":"(!)",
   	  (st.i2s.srt_status_2&SRT_ESTSAMPLERATE)*375/2048,
   	  (st.i2s.srt_status_2&SRT_RATELOCKED)?"[R]":"(!)");
   	t+=tmp;
   	}

   	int bb=24;
   	if(is_k2)
   	{
   	 // read SOC [bypass]
   	 dword t=0;
   	 if(ikx_t->ptr_read(SOC,0,&t)==0)
   	 {
   	  if((t&0xf00)==0xf00)
   	   bb=24;
   	  else
   	   bb=20;
   	 }
   	}

	t+="SPDIF Outputs:\r\n";
   	sprintf(tmp,"     (0): %x (1): %x (2): %x (%d bits / %d Hz)\r\n",
   	  st.spdif.scs0,st.spdif.scs1,st.spdif.scs2,
   	  bb,
   	  (st.spdif.spo_sr==0)?44100:(st.spdif.spo_sr==1)?48000:96000);
   	t+=tmp;

   	if(is_a2)
   	{
   		t+="SPDIF Extended status:\r\n     ";
         	char tmp_x[128];

         	get_bits(tmp_x,st.spdif.scs0x);
                sprintf(tmp,"(0): %s  ",tmp_x);
                t+=tmp;
         	get_bits(tmp_x,st.spdif.scs1x);
                sprintf(tmp,"(1): %s  ",tmp_x);
                t+=tmp;
         	get_bits(tmp_x,st.spdif.scs2x);
                sprintf(tmp,"(2): %s\r\n",tmp_x);
                t+=tmp;

                t+="p16v: PB: ";

                dword tt=0;
                ikx_t->ptr_read(EHC,0,&tt);
                if(tt&EHC_SRC48_MASK)
                {
                 switch((tt>>12)&0xf)
                 {
                  case 2: t+="192000Hz"; break;
                  case 4: t+="96000Hz"; break;
                  case 8: t+="44100Hz"; break;
                  case 0xc: t+="(muted)"; break;
                 }
                }
                 else
                  t+="(bypass)";

                t+=" I2S Out:";

                if(tt&EHC_SRCMULTI_I2S_MASK)
                {
                 switch((tt>>8)&0xf)
                 {
                  case 2: t+="192000Hz"; break;
                  case 4: t+="96000Hz"; break;
                  case 8: t+="44100Hz"; break;
                  case 0xc: t+="(muted)"; break;
                 }
                }
                 else
                t+="(bypass)";

                t+=" SPDIF Out:";

                if(tt&EHC_SRCMULTI_SPDIF_MASK)
                {
                 switch((tt>>4)&0xf)
                 {
                  case 2: t+="192000Hz"; break;
                  case 4: t+="96000Hz"; break;
                  case 8: t+="44100Hz"; break;
                  case 0xc: t+="(muted)"; break;
                 }
                }
                 else
                t+="(bypass)";

                t+="\r\n";

                #define p16v_freq(a) (((a)==0)?44100:(((a)==1)?48000:(((a)==2)?96000:((a)==3)?192000:0)))
                sprintf(tmp,"     Rec: (0): %d %s (1): %d %s (2): %d %s (3): %d %s",
                  
                  p16v_freq((st.p16v>>16)&0x7),
                  (st.p16v&(1<<19))?"[R]":"[U]",
                  p16v_freq((st.p16v>>20)&0x7),
                  (st.p16v&(1<<23))?"[R]":"[U]",
                  p16v_freq((st.p16v>>24)&0x7),
                  (st.p16v&(1<<27))?"[R]":"[U]",
                  p16v_freq((st.p16v>>28)&0x7),
                  (st.p16v&(1<<31))?"[R]":"[U]");
                t+=tmp;
   	}

 } else t="Error getting spdif / i2s status";

 b_edit.SetWindowText((LPCTSTR)t);
 b_edit.RedrawWindow();
}

void CIOStatusDialog::init()
{
 generic_init("io_status");

 timer_id=SetTimer(2255,UPDATE_FREQ,NULL);
 if(timer_id==0)
  timer_id=(UINT_PTR)-1;

 RECT r;

 GetWindowRect(&r);
 r.right-=r.left;
 r.bottom-=r.top;
 r.left=r.top=0;

 r.left+=5;
 r.top+=5;
 r.bottom-=25;
 r.right-=110;

 char tmp_str[128];

 // position
 if(!mf.get_profile("io_status","listbox",tmp_str,sizeof(tmp_str)))
 {
  sscanf(tmp_str,"%d %d %d %d",&r.left,&r.top,&r.right,&r.bottom);
 }

 SetWindowText(mf.get_profile("lang","iostatus.name")+" - "+ikx_t->get_device_name());
 set_dragging(gui_get_show_content());

 if(!mf.get_profile("io_status","copy_button",tmp_str,sizeof(tmp_str)))
 {
  gui_create_button(this,&b_copy,IDg_COPY,tmp_str,mf);
  b_copy.show();
 }

 // reposition window
 CenterWindow(NULL);
 
 b_edit.Create(ES_MULTILINE | ES_AUTOVSCROLL | WS_BORDER |
             ES_READONLY | WS_VSCROLL,r,this,0 ); // 0- any ID number
 b_edit.SetWindowText(__T(""));
 b_edit.ShowWindow(SW_SHOW);

 b_edit.SetFont(font);
}

int CIOStatusDialog::on_command(int wParam, int lParam)
{
	if(wParam==IDg_COPY)
	{
	 kString s;
	 b_edit.GetWindowText(s);

         if(::OpenClipboard(get_wnd()))
         {
          HGLOBAL mem=GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE|GMEM_ZEROINIT,strlen((LPCTSTR)s)+1);
          if(mem)
          {
           LPVOID m=GlobalLock(mem);
           strcpy((char *)m,(LPCTSTR)s);

           EmptyClipboard();
           
           SetClipboardData(CF_TEXT, mem);
           GlobalUnlock(mem);
          }
          CloseClipboard();
         }
	}
	return CKXDialog::on_command(wParam,lParam);
}
