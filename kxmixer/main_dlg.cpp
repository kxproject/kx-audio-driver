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

#include "main_dlg.h"
#include "kxdspdlg.h"
#include "spectrum_dlg.h"

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// hide & destroy
#define destroy(a) ((a)?(a)->destroy():0)

CMainDialog::CMainDialog()
    : CKXDialog()
{
    cur_page=-1;
    mixer_notify_lock=0;
}


BEGIN_MESSAGE_MAP(CMainDialog, CKXDialog)
        ON_LBN_SELCHANGE(IDg_SFLIST, OnSelectSoundfont)
    ON_CBN_SELCHANGE(IDg_AC97SOURCE, OnChangeAC97Source)
    ON_CBN_SELCHANGE(IDg_AC973DSE, OnChangeAC973DSE)
        ON_CBN_SELCHANGE(IDg_DSPFREQ, OnChangeDSPFreq)
        ON_CBN_SELCHANGE(IDg_PASSTHRU, OnChangeAC3Passthru)
        ON_CBN_SELCHANGE(IDg_ECARD_SPDIF0, OnChangeECardSpdif)
        ON_CBN_SELCHANGE(IDg_ECARD_SPDIF1, OnChangeECardSpdif)
        ON_CBN_SELCHANGE(IDg_SPDIF_RECORDING, OnChangeSpdifRecording)
        ON_CBN_SELCHANGE(IDg_ZSNB_SOURCE, OnChangeZsnbSource)
        ON_CBN_SELCHANGE(IDg_P16V_RECORDING, OnChangeP16vRecording)
    ON_WM_VSCROLL()
END_MESSAGE_MAP()

#include "sfbank_dlg.cpp"

//#define USE_CHRONO

#ifdef USE_CHRONO
void chrono(const TCHAR *msg)
{
 static dword prev=0x0;
 if((prev==0) || (msg==NULL))
  prev=GetTickCount();
 if(msg!=NULL)
 {
  dword new_t=GetTickCount();
  debug("%s - %d ms\n",msg,new_t-prev);
  prev=new_t;
 }
}
#else
 #define chrono(a)
#endif

/////////////////////////////////////////////////////////////////////////////

int CMainDialog::select_device(int dev_)
{
    CKXDialog::select_device(dev_);

    is_aps=0;
    is_51=0;
    is_10k2=0;
    is_doo=0;
    is_a2=0;
    is_k2ex=0;
    is_zsnb=0;

        /*
    dsp_microcode m;
    if(!ikx_t->enum_microcode("prolog",&m))
        prolog_pgm=m.pgm;
    else
    {
        prolog_pgm=-1;
    }

    if(!ikx_t->enum_microcode("epilog",&m))
        epilog_pgm=m.pgm;
    else
    {
        epilog_pgm=-1;
    }
        */

    // Find out, if we are EmuAPS or Live!
    int result=0;
    int ret_b=0;
    int set=0;

    if(ikx_t->get_dword(KX_DWORD_IS_APS,(dword *)&is_aps))
    {
        is_aps=0; // error
    }

    if(ikx_t->get_dword(KX_DWORD_IS_51,(dword *)&is_51))
    {
        is_51=0; // error
    }

    if(ikx_t->get_dword(KX_DWORD_IS_A2,(dword *)&is_a2))
    {
        is_a2=0; // error
    }

    if(ikx_t->get_dword(KX_DWORD_IS_A2EX,(dword *)&is_k2ex))
    {
        is_k2ex=0; // error
    }

    if(ikx_t->get_dword(KX_DWORD_IS_A2ZSNB,(dword *)&is_zsnb))
    {
        is_zsnb=0; // error
    }

    if(ikx_t->get_dword(KX_DWORD_AC97_PRESENT,(dword *)&has_ac97))
    {
        has_ac97=0; // error
    }

    if(ikx_t->get_dword(KX_DWORD_IS_10K2,(dword *)&is_10k2))
    {
        is_10k2=0; // error
    }

    if(ikx_t->get_hw_parameter(KX_HW_DOO,(dword *)&is_doo))
    {
        is_doo=0; // error
    }

    char *cmd;
    cmd=GetCommandLine();

    if(strstr(cmd,"--no51")!=NULL)
     is_51=0;
    if(strstr(cmd,"--51")!=NULL)
     is_51=1;

    if(strstr(cmd,"--aps")!=NULL)
     is_aps=1;
    if(strstr(cmd,"--noaps")!=NULL)
     is_aps=0;

    if(strstr(cmd,"--10k2")!=NULL)
     is_10k2=1;
    if(strstr(cmd,"--no10k2")!=NULL)
     is_10k2=0;

    if(strstr(cmd,"--a2")!=NULL)
     is_a2=1;
    if(strstr(cmd,"--noa2")!=NULL)
     is_a2=0;

    if(strstr(cmd,"--k2ex")!=NULL)
     is_k2ex=1;
    if(strstr(cmd,"--nok2ex")!=NULL)
     is_k2ex=0;

    if(strstr(cmd,"--zsnb")!=NULL)
     is_zsnb=1;
    if(strstr(cmd,"--nozsnb")!=NULL)
     is_zsnb=0;

    if(strstr(cmd,"--ac97")!=0)
     has_ac97=1;
    if(strstr(cmd,"--noac97")!=0)
     has_ac97=0;

    set_text(mf.get_profile("lang","mixer.name")+" - "+ikx_t->get_device_name());

    return 0;
}

void CMainDialog::init()
{
    CKXDialog::init("mixer");

        SetIcon(AfxGetApp()->LoadIcon(IDR_ICON), TRUE);         // Set big icon
//        SetIcon(AfxGetApp()->LoadIcon(IDR_ICON), FALSE);      // Set small icon

    select_device();

    rest_position();

        // retrive last page from the registry
        dword def_page=0;
        kSettings cfg;
        cfg.read_abs("Mixer","DefPage",&def_page);

        // spectrum - intially invisible
        spectrum.create("Spectrum",80,10,450,250,this);
        //add_tool("Spectrum Analyzer",&spectrum,IDg_SPECTRUM);
        spectrum.turn_on(0);

        kRect r;
        r.left=0; r.top=0; r.right=30; r.bottom=30;
        ac97_source.create(r,CBS_DROPDOWNLIST /*| CBS_AUTOHSCROLL | WS_VSCROLL*/,IDg_AC97SOURCE,this);
        ac97_source.add_string(mf.get_profile("ac97","mic"));
        ac97_source.add_string(mf.get_profile("ac97","cd"));
        ac97_source.add_string(mf.get_profile("ac97","video"));
        ac97_source.add_string(mf.get_profile("ac97","aux"));
        ac97_source.add_string(mf.get_profile("ac97","linein"));
        ac97_source.add_string(mf.get_profile("ac97","stereomix"));
        ac97_source.add_string(mf.get_profile("ac97","monomix"));
        ac97_source.add_string(mf.get_profile("ac97","phone"));
        add_tool((char *)(LPCTSTR)mf.get_profile("ac97","source"),&ac97_source,IDg_AC97SOURCE);
        ac97_source.set_dropped_width(100);

        r.left=0; r.top=0; r.right=30; r.bottom=30;
        ac97_3dse.create(r,CBS_DROPDOWNLIST /*| CBS_AUTOHSCROLL | WS_VSCROLL*/,IDg_AC973DSE,this);
        ac97_3dse.add_string(mf.get_profile("ac97","3d_off"));
        ac97_3dse.add_string(mf.get_profile("ac97","3d_low"));
        ac97_3dse.add_string(mf.get_profile("ac97","3d_norm"));
        ac97_3dse.add_string(mf.get_profile("ac97","3d_high"));
        add_tool((char *)(LPCTSTR)mf.get_profile("ac97","3dse"),&ac97_3dse,IDg_AC973DSE);
        ac97_3dse.set_dropped_width(100);

        r.left=0; r.top=0; r.right=30; r.bottom=30;
        ac3_passthru.create(r,CBS_DROPDOWNLIST /*| CBS_AUTOHSCROLL | WS_VSCROLL*/,IDg_PASSTHRU,this);
        ac3_passthru.add_string(mf.get_profile("ac97","3d_off"));
        ac3_passthru.add_string("SPDIF 0");
        ac3_passthru.add_string("SPDIF 1");
        ac3_passthru.add_string("SPDIF 2");
        add_tool((char *)(LPCTSTR)mf.get_profile("lang","buttons.passthru.tooltip"),&ac3_passthru,IDg_PASSTHRU);
        ac3_passthru.set_dropped_width(100);

        r.left=0; r.top=0; r.right=30; r.bottom=30;
        dsp_freq.create(r,CBS_DROPDOWNLIST /*| CBS_AUTOHSCROLL| WS_VSCROLL*/,IDg_DSPFREQ,this);
        dsp_freq.add_string("44100");
        dsp_freq.add_string("48000");
        dsp_freq.add_string("96000");
        add_tool((char *)(LPCTSTR)mf.get_profile("locals","spdif_freq"),&dsp_freq,IDg_DSPFREQ);
        dsp_freq.set_dropped_width(100);

        r.left=0; r.top=0; r.right=30; r.bottom=30;
        ecard_spdif0.create(r,CBS_DROPDOWNLIST /*| CBS_AUTOHSCROLL| WS_VSCROLL*/,IDg_ECARD_SPDIF0,this);
        ecard_spdif0.add_string("E-Card");
        ecard_spdif0.add_string("E-Drive");
        ecard_spdif0.add_string("CD-ROM");
        add_tool((char *)(LPCTSTR)mf.get_profile("locals","ecard_spdif0"),&ecard_spdif0,IDg_ECARD_SPDIF0);
        ecard_spdif0.set_dropped_width(100);

        r.left=0; r.top=0; r.right=30; r.bottom=30;
        spdif_recording.create(r,CBS_DROPDOWNLIST /*| CBS_AUTOHSCROLL| WS_VSCROLL*/,IDg_SPDIF_RECORDING,this);
        spdif_recording.add_string(mf.get_profile("ac97","3d_off"));
        spdif_recording.add_string("CD SPDIF");
        spdif_recording.add_string("Coaxial");
        spdif_recording.add_string("SPDIF C");
        spdif_recording.add_string("I2S 0");
        spdif_recording.add_string("I2S 1");
        spdif_recording.add_string("I2S 2");
        add_tool((char *)(LPCTSTR)mf.get_profile("locals","spdif_recording"),&spdif_recording,IDg_SPDIF_RECORDING);
        spdif_recording.set_dropped_width(100);

        r.left=0; r.top=0; r.right=30; r.bottom=30;
        zsnb_source.create(r,CBS_DROPDOWNLIST /*| CBS_AUTOHSCROLL| WS_VSCROLL*/,IDg_ZSNB_SOURCE,this);

        zsnb_source.add_string(mf.get_profile("ac97","linein"));
        zsnb_source.add_string(mf.get_profile("ac97","mic"));

        add_tool((char *)(LPCTSTR)mf.get_profile("locals","zsnb_source"),&zsnb_source,IDg_ZSNB_SOURCE);
        zsnb_source.set_dropped_width(100);

        r.left=0; r.top=0; r.right=30; r.bottom=30;
        p16v_recording.create(r,CBS_DROPDOWNLIST /* | CBS_AUTOHSCROLL| WS_VSCROLL*/,IDg_P16V_RECORDING,this);
        p16v_recording.add_string("CD SPDIF");
        p16v_recording.add_string("Coaxial");
        p16v_recording.add_string("SPDIF C");
        p16v_recording.add_string("I2S 0");
        p16v_recording.add_string("I2S 1");
        p16v_recording.add_string("I2S 2");
        p16v_recording.add_string("10k2 0");
        p16v_recording.add_string("10k2 1");
        p16v_recording.add_string("10k2 2");
        add_tool((char *)(LPCTSTR)mf.get_profile("locals","p16v_recording"),&p16v_recording,IDg_P16V_RECORDING);
        p16v_recording.set_dropped_width(100);

        r.left=0; r.top=0; r.right=30; r.bottom=30;
        ecard_spdif1.create(r,CBS_DROPDOWNLIST /*| CBS_AUTOHSCROLL| WS_VSCROLL*/,IDg_ECARD_SPDIF1,this);
        ecard_spdif1.add_string("E-Card");
        ecard_spdif1.add_string("E-Drive");
        add_tool((char *)(LPCTSTR)mf.get_profile("locals","ecard_spdif1"),&ecard_spdif1,IDg_ECARD_SPDIF1);
        ecard_spdif1.set_dropped_width(100);

        r.left=75; r.top=15; r.right=545; r.bottom=120;
        sf_list.create(r,LBS_NOTIFY | LBS_NOINTEGRALHEIGHT | LBS_EXTENDEDSEL | WS_VSCROLL | WS_BORDER, 
               IDg_SFLIST,this);
        add_tool((char *)(LPCTSTR)mf.get_profile("locals","sflist"),&sf_list,IDg_SFLIST);

        r.left=75; r.top=130; r.right=545; r.bottom=230;
        sf_info.create( r, WS_VSCROLL | WS_BORDER | LBS_NOSEL ,
            IDg_SFINFO,this);
        add_tool((char *)(LPCTSTR)mf.get_profile("locals","sfinfo"),&sf_info,IDg_SFINFO);

        r.left=180; r.top=240; r.right=545; r.bottom=260;
        sf_progress.create(r,0,IDg_SFMEM,this);
        add_tool((char *)(LPCTSTR)mf.get_profile("locals","mem"),&sf_progress,IDg_SFMEM);

        memset(&sliders[0],0,sizeof(slider_t)*MAX_SLIDERS);

    redraw_window(def_page);
    show();

    set_dragging(gui_get_show_content());
}

int CMainDialog::check_device(const char *param)
{
 if((strstr(param,"is_51")!=0) && (is_51==0)) return -1;
 if((strstr(param,"isnt_51")!=0) && (is_51!=0)) return -1;
 if((strstr(param,"is_aps")!=0) && (is_aps==0)) return -1;
 if((strstr(param,"isnt_aps")!=0) && (is_aps!=0)) return -1;
 if((strstr(param,"is_10k2")!=0) && (is_10k2==0)) return -1;
 if((strstr(param,"isnt_10k2")!=0) && (is_10k2!=0)) return -1;

 if((strstr(param,"is_a2")!=0) && (is_a2==0)) return -1;
 if((strstr(param,"isnt_a2")!=0) && (is_a2!=0)) return -1;

 if((strstr(param,"is_k2ex")!=0) && (is_k2ex==0)) return -1;
 if((strstr(param,"isnt_k2ex")!=0) && (is_k2ex!=0)) return -1;

 if((strstr(param,"is_zsnb")!=0) && (is_zsnb==0)) return -1;
 if((strstr(param,"isnt_zsnb")!=0) && (is_zsnb!=0)) return -1;

 if((strstr(param,"has_ac97")!=0) && (has_ac97==0)) return -1;
 if((strstr(param,"hasnt_ac97")!=0) && (has_ac97!=0)) return -1;

 if((strstr(param,"is_doo")!=0) && (is_doo==0)) return -1;
 if((strstr(param,"isnt_doo")!=0) && (is_doo!=0)) return -1;

 return 0;
}

int CMainDialog::create_button(kButton *b,UINT id,char *param,char *guid)
{
 // flags: is_51 is_aps is_10k2 has_ac97 isnt_51 isnt_aps isnt_10k2 hasnt_ac97
 if(check_device(param))
  return -1;

 char ref_button[64]; ref_button[0]=0;
 char tmp_str[128];
 char tmp_static[64]; tmp_static[0]=0;
 int x=-1,y=-1;
 int checkbox=0;

 switch(id)
 {
  case IDg_DOO:
  case IDg_SWAPREAR:
  case IDg_AC97_MICBOOST:
  case IDg_BYPASS:
  case IDg_P_CSW:
  case IDg_SPDIF_DECODE:
  case IDg_AC97_A2:
   checkbox=1;
 }

 sscanf(param,"%s %d %d %s",&ref_button[0],&x,&y,&tmp_static[0]);
 
 int type=1;
 if(!mf.get_profile(ref_button,"type",tmp_str,sizeof(tmp_str)))
 {
  if(strstr(tmp_str,"skin")==0) type=0;
 }

 if(guid)
 {
  if(!mf.get_profile(ref_button,"action",guid,KX_MAX_STRING)) // has 'action'
  {
   if(!manager->is_supported(guid))
   {
    debug("kxmixer: skin-declared action not supported [%s]\n",guid);
    return -1;
   }
  }
 }

 kRect r;
 r.left=x; r.top=y; r.right=r.left+32; r.bottom=r.top+32;
 if(strstr(tmp_static,"t=")!=0)
 {
  HBITMAP bm=mf.load_image(tmp_static+2);
  static_images[cur_static].create("name",x,y,bm,this,kMETHOD_TRANS);
  static_images[cur_static].size_to_content();
  kRect s_r;
  static_images[cur_static].get_rect_abs(s_r);
  r.top+=(s_r.bottom-s_r.top);
  r.bottom+=(s_r.bottom-s_r.top);
  cur_static++;
 }

 if(type==1)
 {
  char tooltip[128]; strcpy(tooltip,"");
  if(!mf.get_profile(ref_button,"tooltip",tmp_str,sizeof(tmp_str)))
  {
   strcpy(tooltip,tmp_str);
  }
  char bm1[64],bm2[64],bm3[64],bm4[64],bm5[64];
  bm1[0]=0; bm2[0]=0; bm3[0]=0; bm4[0]=0; bm5[0]=0;
  if(!mf.get_profile(ref_button,"image",tmp_str,sizeof(tmp_str)))
  {
   sscanf(tmp_str,"%s %s %s %s %s",&bm1[0],&bm2[0],&bm3[0],&bm4[0],&bm5[0]);
   HBITMAP bm_n,bm_o,bm_s,bm_def,bm_dis;
   bm_n=bm1[0]?mf.load_image(bm1):0;
   bm_o=bm2[0]?mf.load_image(bm2):0;
   bm_s=bm3[0]?mf.load_image(bm3):0;
   bm_def=bm4[0]?mf.load_image(bm4):0;
   bm_dis=bm5[0]?mf.load_image(bm5):0;
   if(bm_n==0)
   {
        debug(_T("kxmixer: create_button failed\n"));
    MessageBox((LPCTSTR)mf.get_profile("errors","image"),
     (LPCTSTR)mf.get_profile("errors","severe"),MB_OK|MB_ICONSTOP);
    exit(32);
   }

  int tmp_method=0;
  if(!mf.get_profile(ref_button,"method",tmp_str,sizeof(tmp_str)))
  {
   if(strstr(tmp_str,"blend")) tmp_method|=kMETHOD_BLEND;
   if(strstr(tmp_str,"trans")) tmp_method|=kMETHOD_TRANS;   
  }
  if(tmp_method==0)
   tmp_method=kMETHOD_COPY;

   if(checkbox)
   {
//    -- for testing only --
//    tmp_method=kMETHOD_DEFAULT;
//    bm1[0]=0;

    ((kCheckButton *)b)->create(tooltip[0]?tooltip:NULL,r,this,id,tmp_method,0);

    // skin file =<unchecked normal> <unchecked over> <checked normal> <checked over> [<disabled>]
    // set_bitmaps(): unchecked,unchecked_over,checked,checked_over,unchecked_disabled=0,checked_disabled=0
    if(bm1[0])
      ((kCheckButton *)b)->set_bitmaps(bm_n,bm_o,bm_s,bm_def,bm_dis,bm_dis);
   }
   else
   {
     b->create(tooltip[0]?tooltip:NULL,type?BS_OWNERDRAW:0,r,this,id,tmp_method);

     if(bm_o==0) bm_o=bm_n;
     if(bm_s==0) bm_s=bm_n;

     b->set_bitmaps(bm_n,bm_o,bm_s,bm_def,bm_dis);
   }

   if(checkbox)
   {
           // some more speculations
           dword val=0;
           kCheckButton *cb=(kCheckButton *)b;
           switch(id)
           {
            case IDg_DOO: if((!ikx_t->get_hw_parameter(KX_HW_DOO,&val)) && (val!=0)) { cb->set_check(1); } else cb->set_check(0); break;
            case IDg_SWAPREAR: if((!ikx_t->get_hw_parameter(KX_HW_SWAP_FRONT_REAR,&val)) && (val!=0)) { cb->set_check(1); } else cb->set_check(0); break;
            case IDg_BYPASS: if((!ikx_t->get_hw_parameter(KX_HW_SPDIF_BYPASS,&val)) && (val!=0)) { cb->set_check(1);  } else cb->set_check(0); break;
            case IDg_SPDIF_DECODE: if((!ikx_t->get_hw_parameter(KX_HW_SPDIF_DECODE,&val)) && (val!=0)) { cb->set_check(1);  } else cb->set_check(0); break;
           //   case IDg_PASSTHRU: if((!ikx_t->get_hw_parameter(KX_HW_AC3_PASSTHROUGH,&val)) && (val!=0)) { cb->set_check(1);  } break;
           //   case IDg_PHONES: if((!ikx_t->get_hw_parameter(KX_HW_DETECT_HEADPHONES,&val)) && (val!=0)) { cb->set_check(1);  } break;
            case IDg_P_CSW: if((!ikx_t->get_hw_parameter(KX_HW_ROUTE_PH_TO_CSW,&val)) && (val!=0)) { cb->set_check(1); } else cb->set_check(0); break;
            case IDg_AC97_MICBOOST: 
                {
                  word v;
                  if((!ikx_t->ac97_read(AC97_REG_MIC_VOL,&v))&&((v&0x40))) { cb->set_check(1);  } else cb->set_check(0); break;
                }
                break;
                case IDg_AC97_A2:
                    {
                     dword v=0;
                     if(ikx_t->get_hw_parameter(KX_HW_K2_AC97,&v)==0)
                     {
                      if(v)
                       cb->set_check(1);
                      else
                       cb->set_check(0);
                     }
                    }
                    break;
           }

   }
  }

  // after create:
  if(!mf.get_profile(ref_button,"alpha",tmp_str,sizeof(tmp_str)))
  {
   int alpha=90;
   sscanf(tmp_str,"%d",&alpha);
   b->set_alpha(alpha);
  }
  if(!mf.get_profile(ref_button,"transp_color",tmp_str,sizeof(tmp_str)))
  {
   dword t_c=RGB(0xff,0xff,0xff);
   sscanf(tmp_str,"%x",&t_c);
   b->set_transpcolor(t_c);
  }
  if(bm1[0])
   b->size_to_content();
 }
 else // normal button
 {
  char caption[64]; strcpy(caption,"<none>");
  int wd=30;
  int ht=10;
  if(!mf.get_profile(ref_button,"caption",tmp_str,sizeof(tmp_str)))
  {
   strncpy(caption,tmp_str,sizeof(caption));
  }
  if(!mf.get_profile(ref_button,"wd",tmp_str,sizeof(tmp_str)))
  {
    sscanf(tmp_str,"%d",&wd); r.right=r.left+wd;
  }
  if(!mf.get_profile(ref_button,"ht",tmp_str,sizeof(tmp_str)))
  {
    sscanf(tmp_str,"%d",&ht); r.bottom=r.top+ht;
  }
  b->create((LPCTSTR)caption,(DWORD)BS_PUSHBUTTON,r,this,(UINT)id,kMETHOD_DEFAULT);
  if(!mf.get_profile(ref_button,"tooltip",tmp_str,sizeof(tmp_str)))
  {
   add_tool(tmp_str,b,id);
  }
 }

 return 0;
}

int CMainDialog::create_combo(kCombo *b,UINT id,char *param)
{
 if(check_device(param))
  return -1;

 char tmp_str[64];
 kRect r; r.left=0; r.top=0; r.right=100; r.bottom=40;

 sscanf(param,"%s %d %d %d %d",&tmp_str[0],&r.left,&r.top,&r.right,&r.bottom);

 HBITMAP bm=mf.load_image(tmp_str);
 static_images[cur_static].create("name",r.left,r.top,bm,this,kMETHOD_TRANS);
 static_images[cur_static].size_to_content();
 kRect s_r;
 static_images[cur_static].get_rect_abs(s_r);
 r.top+=(s_r.bottom-s_r.top);
 r.bottom+=(s_r.bottom-s_r.top);
 cur_static++;

 b->reposition(r);
 b->set_dropped_width(r.right-r.left+5);

 word val=0;
 switch(id)
 {
    case IDg_AC97SOURCE:
         ikx_t->ac97_read(AC97_REG_REC_SELECT,&val);
         b->set_selection(val&0x7);
         break;
    case IDg_AC973DSE:
          if((!ikx_t->ac97_read(AC97_REG_RESET,&val)) && ((val >> 10) & 0x1f)) // supported
          {
            if(!ikx_t->ac97_read(AC97_REG_GENERAL,&val))
            {
                if(val&0x2000)
                {
                    ikx_t->ac97_read(AC97_REG_3D_CTRL,&val);
                    int sel=val&0x3;
                    // on certain cards lower bits are set to 0
                    if(sel==0)
                     sel=(val>>2)&0x3;

                    b->set_selection(sel);
                }
                else b->set_selection(0);
            }
            else b->set_selection(0);
          }
          break;
    case IDg_DSPFREQ:
        {
         dword val2;
         if(!ikx_t->get_hw_parameter(KX_HW_SPDIF_FREQ,&val2))
         {
          b->set_selection(val2&0x3);
         }
        }
     break;
    case IDg_PASSTHRU:
        {
         dword val2;
         if(!ikx_t->get_hw_parameter(KX_HW_AC3_PASSTHROUGH,&val2))
         {
          b->set_selection(val2&0x3);
         }
        }
     break;
    case IDg_ECARD_SPDIF0:
        {
         dword val2;
         if(is_aps)
          if(!ikx_t->get_hw_parameter(KX_HW_ECARD_ROUTING,&val2))
         {
          switch(val2&0xffff)
          {
           case 0: // ecard
            b->set_selection(0); break;
           case 1: // edrive
            b->set_selection(1); break;
           case 4: // cdrom
            b->set_selection(2); break;
          }
         }
        }
     break; 
    case IDg_SPDIF_RECORDING:
        {
         dword val2;
         if(is_10k2 && !ikx_t->get_hw_parameter(KX_HW_SPDIF_RECORDING,&val2))
         {
          b->set_selection(val2&0x7);
         }
        }
    break;  
    case IDg_ZSNB_SOURCE:
        {
         dword val2;
         if(is_10k2 && !ikx_t->get_hw_parameter(KX_HW_A2ZSNB_SOURCE,&val2))
         {
          b->set_selection(val2&0x1);
         }
        }
    break;  
    case IDg_P16V_RECORDING:
        {
         dword val2;
         if(is_a2 && !ikx_t->get_hw_parameter(KX_HW_P16V_REC_ROUTING,&val2))
         {
          switch(val2)
          {
           case 0x00: b->set_selection(0); break;
           case 0x10: b->set_selection(1); break;
           case 0x20: b->set_selection(2); break;
           case 0x01: b->set_selection(3); break;
           case 0x11: b->set_selection(4); break;
           case 0x21: b->set_selection(5); break;
           case 0x02: b->set_selection(6); break;
           case 0x12: b->set_selection(7); break;
           case 0x22: b->set_selection(8); break;
           default:
            break;
          }
         }
        }
    break;  
    case IDg_ECARD_SPDIF1:
        {
         dword val2;
         if(is_aps)
          if(!ikx_t->get_hw_parameter(KX_HW_ECARD_ROUTING,&val2))
         {
          switch((val2>>16)&0xffff)
          {
           case 0: // ecard
            b->set_selection(0); break;
           case 1: // edrive
            b->set_selection(1); break;
//         case 4: // cdrom
//          b->set_selection(2); break;
          }
         }
        }
    break;  
 }

 return 0;
}

int CMainDialog::get_volume(char *l_n,char *r_n,int i,int patch)
{
    int result=0;
    dword val;

    if(l_n!=NULL)
    {
        if(ikx_t->get_dsp_register(patch,l_n,&val))
        {
            kString s;
            s.Format("Error getting DSP register: patch:%d l_n=%s r_n=%s\n",patch,l_n,r_n);
            MessageBox((LPCTSTR)s);
            return 0;
        }   

        if(val==0)
            sliders[i].mute=1;
        else
            sliders[i].mute=0;

        if((val&0x80000000) && !(sliders[i].type&TYPE_NEGATIVE)) // phase
        {
         sliders[i].phase=1;
         val=(dword)(-((int)val));
        }                
        else
         sliders[i].phase=0;

        if(sliders[i].type&TYPE_DB)
        {
                        #define RES_MIN_X   0x2000
                        #define RES_MIN     96.370798434988f
                        #define RES_FACTOR  (255.0f/(RES_MIN+12.041199826559f))

                        // 20*log(0x20000000/0x7fffffff,10)+12 => 0.0dB
                        // -144.5 .. val .. +12 dB
                        if(val<RES_MIN_X) val=RES_MIN_X;

                        float v=(20.0f*log10f((float)val/2147483647.0f)+12.041199826559f+RES_MIN)*RES_FACTOR;

                        sliders[i].vol=0xff-(int)v;

                        // debug("kxmixer: '%s': %d %f %x\n",l_n,0xff-(int)v,v,val);
        }
        else
        if(sliders[i].type&TYPE_NEGATIVE)
        {
          if(val==0)
           sliders[i].vol=0xff;
          else
           sliders[i].vol=(val>>23)-0x100;
        }
        else
          sliders[i].vol=0xff-(val>>23);

        result=1;
    }
    if(r_n!=NULL)
    {
        if(ikx_t->get_dsp_register(patch,r_n,&val))
            return 0;

        if(val==0)
            sliders[i].mute=1;
        else
            sliders[i].mute=0;

        if((val&0x80000000) && !(sliders[i].type&TYPE_NEGATIVE)) // phase
        {
         sliders[i].phase=1;
         val=(dword)(-((int)val));
        }
        else
         sliders[i].phase=0;

        if(sliders[i].type&TYPE_DB)
        {
                // 20*log(0x20000000/0x7fffffff,10)+12 => 0.0dB
                // -144.5 .. val .. +12 dB
                if(val<RES_MIN_X) val=RES_MIN_X;

                float v=(20.0f*log10f((float)val/2147483647.0f)+12.041199826559f+RES_MIN)*RES_FACTOR;

                sliders[i].pan=0xff-(int)v;
        }
        else
        if(sliders[i].type&TYPE_NEGATIVE)
        {
          if(val==0)
           sliders[i].pan=0xff;
          else
           sliders[i].pan=(val>>23)-0x100;
        }
        else
           sliders[i].pan=0xff-(val>>23);

        result=1;
    }
    return result;
}

int CMainDialog::set_volume(char *l_n,char *r_n,int i,int patch)
{
    dword val;

    if(l_n!=NULL)
    {
      if(sliders[i].type&TYPE_DB)
      {
        switch(0xff-sliders[i].vol)
        {
         case 0: val=0x0; break;
         case 255: val=0x7fffffff; break;
         case 240: val=0x40000000; break;
         case 226: val=0x20000000; break;
         case 212: val=0x10000000; break;
         case 198: val=0x08000000; break;
         default:
          {
           float v=(powf(10.0f,((0xff-sliders[i].vol)/RES_FACTOR-12.041199826559f-RES_MIN)/20.0f))*2147483647.0f;
           val=(dword)(int)v;
          }
        }
      }
      else
      if(sliders[i].type&TYPE_NEGATIVE)
      {
        val=(sliders[i].vol+0x100)<<23;

        if(val==0xFF800000)
         val=0x0; // muted
      }
      else
      {
        val=(0xff-sliders[i].vol)<<23;

        if(val==0x7F800000)
         val=0x7fffffff;
      }

      if(sliders[i].mute==1)
            val=0;

      if(sliders[i].phase)
         val=(dword)(-((int)val));

      if(ikx_t->set_dsp_register(patch,l_n,val))
            return 0;
    }
    if(r_n!=NULL)
    {
      if(sliders[i].type&TYPE_DB)
      {
        switch(0xff-sliders[i].pan)
        {
         case 0: val=0x0; break;
         case 255: val=0x7fffffff; break;
         case 240: val=0x40000000; break;
         case 226: val=0x20000000; break;
         case 212: val=0x10000000; break;
         case 198: val=0x08000000; break;
                 default:
          {
           float v=(powf(10.0f,((0xff-sliders[i].pan)/RES_FACTOR-12.041199826559f-RES_MIN)/20.0f))*2147483647.0f;
           val=(dword)(int)v;
          }
        }
      }
      else
      if(sliders[i].type&TYPE_NEGATIVE)
      {
        val=(sliders[i].vol+0x100)<<23;

        if(val==0xFF800000)
         val=0x0; // muted
      }
      else
      {
        val=(0xff-sliders[i].pan)<<23;

        if(val==0x7F800000)
         val=0x7fffffff;
      }

      if(sliders[i].mute==1)
            val=0;

      if(sliders[i].phase)
         val=(dword)(-((int)val));

      if(ikx_t->set_dsp_register(patch,r_n,val))
            return 0;

    }
    return 1;
}

int CMainDialog::get_slider_profile(char *ref_slider,char *templ_slider,char *sl,
                  char *tmp_str,int tmp_str_size)
{
  if(!mf.get_profile(ref_slider,sl,tmp_str,tmp_str_size))
  {
   return 0;
  }
  else
     if(!mf.get_profile(templ_slider,sl,tmp_str,tmp_str_size))
     {
      return 0;
     }
     else
        if(!mf.get_profile(_T("sliders.default"),sl,tmp_str,tmp_str_size))
        {
         return 0;
        }
        else
           if(!mf.get_profile(_T("sliders.layout"),sl,tmp_str,tmp_str_size))
           {
            return 0;
           }
  return -1;
}

int CMainDialog::create_slider(char *param)
{
 // flags: is_51 is_aps is_10k2 has_ac97 isnt_51 isnt_aps isnt_ hasnt_ac97

 chrono(NULL);
 chrono(" -- slider -- begin");
  
 if(check_device(param))
  return -1;

 chrono(" -- slider -- device checked");

 char ref_slider[64]; ref_slider[0]=0;
 char templ_slider[128]; strcpy(templ_slider,"sliders.default");
 char tmp_str[128];
 int x=0,y=0;

 sscanf(param,"%s %d %d",&ref_slider[0],&x,&y);

 if(!get_slider_profile(ref_slider,templ_slider,"template",tmp_str,sizeof(tmp_str))==0)
  strcpy(templ_slider,tmp_str);

 int sl_type=-1;
 int mode=0;
 int sl_knob=-1;
 int sl_indi=-1;

 if(!mf.get_profile(ref_slider,"type",tmp_str,sizeof(tmp_str)))
 {
  if(strstr(tmp_str,"noskin")==0) { sl_type=1; }
  if(strstr(tmp_str,"noknob")!=0) { sl_knob=0; }
  if(strstr(tmp_str,"noindi")!=0) { sl_indi=0; }

  if(strstr(tmp_str,"skin")==0) { if(sl_type==-1) sl_type=0; }
  if(strstr(tmp_str,"knob")!=0) { if(sl_knob==-1) sl_knob=1; }
  if(strstr(tmp_str,"indi")!=0) { if(sl_indi==-1) sl_indi=1; }
 }
 if(!mf.get_profile(templ_slider,"type",tmp_str,sizeof(tmp_str)))
 {
  if(strstr(tmp_str,"noskin")==0) { sl_type=1; }
  if(strstr(tmp_str,"noknob")!=0) { sl_knob=0; }
  if(strstr(tmp_str,"noindi")!=0) { sl_indi=0; }

  if(strstr(tmp_str,"skin")==0) { if(sl_type==-1) sl_type=0; }
  if(strstr(tmp_str,"knob")!=0) { if(sl_knob==-1) sl_knob=1; }
  if(strstr(tmp_str,"indi")!=0) { if(sl_indi==-1) sl_indi=1; }
 }
 if(!mf.get_profile(_T("sliders.default"),"type",tmp_str,sizeof(tmp_str)))
 {
  if(strstr(tmp_str,"skin")==0) { if(sl_type==-1) sl_type=0; }
  if(strstr(tmp_str,"knob")!=0) { if(sl_knob==-1) sl_knob=1; }
  if(strstr(tmp_str,"indi")!=0) { if(sl_indi==-1) sl_indi=1; }
 }
 if(sl_type==-1) sl_type=1;
 if(sl_knob==-1) sl_knob=0;
 if(sl_indi==-1) sl_indi=0;
 
 if(!mf.get_profile(ref_slider,"mode",tmp_str,sizeof(tmp_str)))
 {
  if(strstr(tmp_str,"stereo")!=0) mode|=TYPE_STEREO;
  if(strstr(tmp_str,"mute")!=0) mode|=TYPE_MUTE;
  if(strstr(tmp_str,"omni")!=0) mode|=TYPE_OMNI;
  if(strstr(tmp_str,"mixer")!=0) mode|=TYPE_MIXER;
  if(strstr(tmp_str,"ac97")!=0) mode|=TYPE_AC97;
  if(strstr(tmp_str,"gpr")!=0) mode|=TYPE_DSP;
  if(strstr(tmp_str,"amount")!=0) mode|=TYPE_AMOUNT;
  if(strstr(tmp_str,"agc")!=0) mode|=TYPE_AGC;
  if(strstr(tmp_str,"in_db12")!=0) mode|=TYPE_DB;
  if(strstr(tmp_str,"negative")!=0) mode|=TYPE_NEGATIVE;
 }
 if(mode==0)
  return -1;

 chrono(" -- slider -- much str-str");

 if(mf.get_profile(ref_slider,"tooltip",tmp_str,sizeof(tmp_str))) // FAILED
 {
  strcpy(sliders[cur_slider].name,"?");
 } else strncpy(sliders[cur_slider].name,tmp_str,sizeof(sliders[cur_slider].name));

 kx_assignment_info assignments[MAX_MIXER_CONTROLS];
 memset(assignments,0,sizeof(assignments));

 for(int a_cnt=0;a_cnt<MAX_MIXER_CONTROLS;a_cnt++)
 {
  assignments[a_cnt].level=a_cnt;
  if(ikx_t->get_dsp_assignments(&assignments[a_cnt])!=0) // error?
   assignments[a_cnt].level=-1;
 }

 // ---- Draw top image

 if(!mf.get_profile(ref_slider,"top",tmp_str,sizeof(tmp_str)))
 {
  kRect r;
  r.left=x; r.top=y; r.right=r.left+70; r.bottom=r.top+20;

  HBITMAP bm=mf.load_image(tmp_str);

  int tmp_method=0;
  if(!get_slider_profile(ref_slider,templ_slider,"top_method",tmp_str,sizeof(tmp_str)))
  {
   if(strstr(tmp_str,"blend")) tmp_method|=kMETHOD_BLEND;
   if(strstr(tmp_str,"trans")) tmp_method|=kMETHOD_TRANS;   
  }
  if(tmp_method==0)
   tmp_method=kMETHOD_COPY;

  int tmp_x=0,tmp_y=0;
  if(!get_slider_profile(ref_slider,templ_slider,"top_align",tmp_str,sizeof(tmp_str)))
  {
   sscanf(tmp_str,"%d %d",&tmp_x,&tmp_y);
   r.left+=tmp_x; r.right+=tmp_x;
   r.top+=tmp_y; r.bottom+=tmp_y;
  }

  static_images[cur_static].create(sliders[cur_slider].name,x,y,bm,this,tmp_method);

  if(!get_slider_profile(ref_slider,templ_slider,"top_alpha",tmp_str,sizeof(tmp_str)))
  {
   int alpha=90;
   sscanf(tmp_str,"%d",&alpha);
   static_images[cur_static].set_alpha(alpha);
  }

  if(!get_slider_profile(ref_slider,templ_slider,"top_transp_color",tmp_str,sizeof(tmp_str)))
  {
   unsigned int t_c=RGB(0xff,0xff,0xff);
   sscanf(tmp_str,"%x",&t_c);
   static_images[cur_static].set_transpcolor(t_c);
  }

  static_images[cur_static].size_to_content();
  cur_static++;
 }

 chrono(" -- slider -- static images created");

 // -- process ID parameters

 int mx_id=0;

 if(!mf.get_profile(ref_slider,"id",tmp_str,sizeof(tmp_str)))
 {
   int id=0;
   if(mode&TYPE_MIXER)
   {
        sscanf(tmp_str,"%d",&id);
        switch(id)
        {
         case 0: mx_id=KX_MIXER_MASTER; break;
         case 1: mx_id=KX_MIXER_WAVE; break;
         case 2: mx_id=KX_MIXER_SYNTH; break;
         case 3: mx_id=KX_MIXER_REC; break;
         case 4: mx_id=KX_MIXER_WAVE23; break;
         case 5: mx_id=KX_MIXER_WAVE45; break;
         case 6: mx_id=KX_MIXER_WAVE67; break;
         case 7: mx_id=KX_MIXER_LINEIN; break;
         case 8: mx_id=KX_MIXER_MICIN; break;
         case 9: mx_id=KX_MIXER_WAVEHQ; break;
         default:
          return -1;
        }

CREATE_WINMM:
        if(ikx_t->mixer(KX_PROP_GET,mx_id,0,&sliders[cur_slider].vol)) // error?
         return -1;

        if(mode&TYPE_STEREO)
          ikx_t->mixer(KX_PROP_GET,mx_id,1,&sliders[cur_slider].pan);

        ikx_t->mixer(KX_PROP_GET,mx_id+1,0,&sliders[cur_slider].mute);
        if(sliders[cur_slider].mute)
           sliders[cur_slider].mute=1;

        sliders[cur_slider].vol>>=8;
        sliders[cur_slider].pan>>=8;
        sliders[cur_slider].vol=255-sliders[cur_slider].vol;
        sliders[cur_slider].pan=255-sliders[cur_slider].pan;
   }
   else if(mode&TYPE_AC97)
   {
    sscanf(tmp_str,"%x",&id);
    if(!mf.get_profile(ref_slider,"bits",tmp_str,sizeof(tmp_str)))
    {
      sscanf(tmp_str,"%d",&sliders[cur_slider].bits);
    } else return -1;

    word val;
    if(!ikx_t->ac97_read((byte)id,&val))
    {
        if(val&0x8000) // muted
            sliders[cur_slider].mute=1;
        else
            sliders[cur_slider].mute=0;

        byte mask=(1<<sliders[cur_slider].bits)-1;
        byte multy=256/(1<<sliders[cur_slider].bits);

        if(mode&TYPE_STEREO)
        {
         sliders[cur_slider].vol=((val>>8)&mask)*multy>255?255:((val>>8)&mask)*multy;
         sliders[cur_slider].pan=(val&mask)*multy>255?255:(val&mask)*multy;

         // 3537: removed
         // 3537d: back
         if(id==AC97_REG_REC_GAIN)
         {
           sliders[cur_slider].vol=255-sliders[cur_slider].vol;
           sliders[cur_slider].pan=255-sliders[cur_slider].pan;
         }
        }
        else
        {
         sliders[cur_slider].vol=(val&mask)*multy>255?255:(val&mask)*multy;
         sliders[cur_slider].pan=-1;
        }
    }
   } 
   else if(mode&TYPE_AMOUNT)
   {
    sscanf(tmp_str,"%d",&id);
    byte amount;
    if(!ikx_t->get_send_amount(id,&amount))
    {
          sliders[cur_slider].vol=(int)(0xff-amount);
    } else return -1;
   }
   else if(mode&TYPE_DSP)
   {
     dsp_microcode m;
     if(!ikx_t->enum_microcode(tmp_str,&m))
       id=m.pgm;
     else
       return -1;

     char m_name[128]; strcpy(m_name,tmp_str);

     if(!mf.get_profile(ref_slider,"gpr1",tmp_str,sizeof(tmp_str)))
       strncpy(sliders[cur_slider].gpr1,tmp_str,sizeof(sliders[cur_slider].gpr1));

     if(!mf.get_profile(ref_slider,"gpr2",tmp_str,sizeof(tmp_str)))
       strncpy(sliders[cur_slider].gpr2,tmp_str,sizeof(sliders[cur_slider].gpr2));

     // check if the DSP is assigned
     for(int a_cnt=0;a_cnt<MAX_MIXER_CONTROLS;a_cnt++)
     {
     if(assignments[a_cnt].level==-1)
      continue;
     if(strcmp(assignments[a_cnt].pgm,m_name)==0)
     {
       if(strcmp(sliders[cur_slider].gpr1,assignments[a_cnt].reg_left)==0)
       {
        debug("kxmixer: found assignment [%s.%s] - [%s.%s] @ %d\n",
         assignments[a_cnt].pgm,assignments[a_cnt].reg_left,
         m_name,sliders[cur_slider].gpr1,a_cnt);
        // found

        // for Vista, remove assignment, if the mixer is not accessible
        // 'mixer' mode: 0 (Master) 1 (Wave) 2 (Synth) 3 (Rec) 4 (Wave23) 5 (Wave45) 6 (Wave67)
        //               7 (LineIn) 8 (MicIn) 9 (WaveHQ)
        switch(a_cnt)
        {
         case MIXER_KX0:
           mx_id=KX_MIXER_LINEIN;
           id=7;
           break;
         case MIXER_KX1:
           mx_id=KX_MIXER_MICIN;
           id=8;
           break;
         case MIXER_MASTER:
           mx_id=KX_MIXER_MASTER;
           id=0;
           break;
         case MIXER_WAVE:
           mx_id=KX_MIXER_WAVE;
           id=1;
           break;
         case MIXER_SYNTH:
           mx_id=KX_MIXER_SYNTH;
           id=2;
           break;
         case MIXER_REC:
           mx_id=KX_MIXER_REC;
           id=3;
           break;
        }

        int tmp_vol=1234;
        if(ikx_t->mixer(KX_PROP_GET,mx_id,0,&tmp_vol)==0) // ok?
        {
          mode&=~TYPE_DSP;
          mode|=TYPE_MIXER;
          goto CREATE_WINMM;
        }
        // otherwise: keep it DSP-based
        // restore id
        id=m.pgm;
       }
     }
     }

     sliders[cur_slider].type=mode; // pre-set .type before calling get_volume

     if(!get_volume(sliders[cur_slider].gpr1,sliders[cur_slider].gpr2,cur_slider,id))
      return -1;
   } else if(mode&TYPE_AGC)
   {
    dword v;
    if(ikx_t->get_hw_parameter(KX_HW_ECARD_ADC_GAIN,&v)==0)
    {
     sliders[cur_slider].vol=255-(v&0xff);
     sliders[cur_slider].pan=255-((v>>8)&0xff);
    }
   }
    else
      return -1;

   chrono(" -- slider -- initial set-up");

   sliders[cur_slider].type=mode;
   sliders[cur_slider].id=id;

   if(sliders[cur_slider].type&TYPE_STEREO)
   {
          if(sliders[cur_slider].vol==sliders[cur_slider].pan)
            sliders[cur_slider].omni=1;
          else
            sliders[cur_slider].omni=0;
   }
   else
          sliders[cur_slider].omni=-1;

   if(!(sliders[cur_slider].type&TYPE_MUTE))
    sliders[cur_slider].mute=-1;

   if(!mf.get_profile(ref_slider,"name",tmp_str,sizeof(tmp_str)))
   {
     sliders[cur_slider].name_s=new kStatic;

     COLORREF fg=0xffffff;
     COLORREF bk=0xb0b000;
     int name_height=18;
     int name_width=70;

     if(!get_slider_profile(ref_slider,templ_slider,"name_height",tmp_str,sizeof(tmp_str)))
      sscanf(tmp_str,"%d",&name_height);

     char tmp2[64];
     if(!get_slider_profile(ref_slider,templ_slider,"name_colors",tmp2,sizeof(tmp2)))
      sscanf(tmp2,"%x %x",&fg,&bk);

     if(!get_slider_profile(ref_slider,templ_slider,"name_width",tmp_str,sizeof(tmp_str)))
      sscanf(tmp_str,"%d",&name_width);

     int name_align_x=0,name_align_y=0;

     if(!get_slider_profile(ref_slider,templ_slider,"name_align",tmp_str,sizeof(tmp_str)))
      sscanf(tmp_str,"%d %d",&name_align_x,&name_align_y);

     kRect r; 
     r.left=x+name_align_x;
     r.top=y+name_align_y;
     r.right=r.left+name_width;
     r.bottom=r.top+name_height;

     mf.get_profile(ref_slider,"name",tmp_str,sizeof(tmp_str));

     sliders[cur_slider].name_s->create(
      tmp_str,r.left,r.top,r.right-r.left,r.bottom-r.top,this,fg,bk);

     sliders[cur_slider].type|=TYPE_NAMED;
   }

   // -- draw slider: current r.(x,y,x2,y2) --> top coords shifted down (+70,+25);
  if(sl_knob==0)
  {
   sliders[cur_slider].vol_s=new kSlider;
   if(sliders[cur_slider].type&TYPE_STEREO)
    sliders[cur_slider].pan_s=new kSlider;

   // load bitmaps (for size calculations)
   char bm1[64],bm2[64],bm3[64],bm4[64];
   bm1[0]=0; bm2[0]=0; bm3[0]=0; bm4[0]=0;
   if(!get_slider_profile(ref_slider,templ_slider,"slider_images",tmp_str,sizeof(tmp_str)))
   {
    sscanf(tmp_str,"%s %s %s %s",&bm1[0],&bm2[0],&bm3[0],&bm4[0]);
    HBITMAP bm_n,bm_o,bm_s,bm_d;
    bm_n=bm1[0]?mf.load_image(bm1):0;
    bm_o=bm2[0]?mf.load_image(bm2):0;
    bm_s=bm3[0]?mf.load_image(bm3):0;
    bm_d=bm4[0]?mf.load_image(bm4):0;
    if(bm_n==0)
    {
        debug(_T("kxmixer: slider's create_button failed\n"));
    MessageBox((LPCTSTR)mf.get_profile("errors","image"),
     (LPCTSTR)mf.get_profile("errors","severe"),MB_OK|MB_ICONSTOP);
    exit(32);
    }
    if(sliders[cur_slider].vol_s) sliders[cur_slider].vol_s->set_bitmaps(bm_n,bm_o,bm_s,bm_d);
    if(sliders[cur_slider].pan_s) sliders[cur_slider].pan_s->set_bitmaps(bm_n,bm_o,bm_s,bm_d);
   }
   else
   {
    HBITMAP bm_n,bm_o,bm_s,bm_d;

    bm_n=mf.load_image(mf.get_profile("sliders.default","default_slider"));

    bm_o=mf.load_image(mf.get_profile("sliders.default","slider_thumb"));
    bm_s=mf.load_image(mf.get_profile("sliders.default","slider_thumb_d"));
    bm_d=mf.load_image(mf.get_profile("sliders.default","slider_thumb_s"));
    if(bm_n==0)
    {
        debug(_T("kxmixer: slider's create_button2 failed\n"));
    MessageBox((LPCTSTR)mf.get_profile("errors","image"),
     (LPCTSTR)mf.get_profile("errors","severe"),MB_OK|MB_ICONSTOP);
    exit(32);
    }

    if(sliders[cur_slider].vol_s) sliders[cur_slider].vol_s->set_bitmaps(bm_n,bm_o,bm_s,bm_d);
    if(sliders[cur_slider].pan_s) sliders[cur_slider].pan_s->set_bitmaps(bm_n,bm_o,bm_s,bm_d);

    chrono(" -- slider -- slider bitmaps uploaded");
   }

   int sl_x1=0,sl_x2=0,sl_y1=0,sl_y2=0;

   if(sliders[cur_slider].type&TYPE_STEREO)
   {
     if(!get_slider_profile(ref_slider,templ_slider,"stereo_slider_align",tmp_str,sizeof(tmp_str)))
      sscanf(tmp_str,"%d %d %d %d",&sl_x1,&sl_y1,&sl_x2,&sl_y2);
   }
   else
   {
     if(!get_slider_profile(ref_slider,templ_slider,"mono_slider_align",tmp_str,sizeof(tmp_str)))
      sscanf(tmp_str,"%d %d",&sl_x1,&sl_y1);
   }
   kRect r;
   r.left=sl_x1+x;
   r.right=r.left+sliders[cur_slider].vol_s->get_slider_width();
   r.top=sl_y1+y;
   r.bottom=r.top+sliders[cur_slider].vol_s->get_slider_height(); // arbitrary; will be changed by size_to_content

   sliders[cur_slider].vol_s->create(sliders[cur_slider].name,TBS_VERT | TBS_RIGHT | TBS_NOTICKS,
        r,this,IDg_CONTROL+cur_slider,kMETHOD_TRANS);
   sliders[cur_slider].vol_s->SetWindowText(sliders[cur_slider].name);
   sliders[cur_slider].vol_s->set_range(0,255);
   sliders[cur_slider].vol_s->set_pos(sliders[cur_slider].vol);

   if(sliders[cur_slider].type&TYPE_STEREO)
   {
    r.left=sl_x2+x;
    r.top=sl_y2+y;
    r.right=r.left+sliders[cur_slider].pan_s->get_slider_width();
    r.bottom=r.top+sliders[cur_slider].pan_s->get_slider_height();
    
    sliders[cur_slider].pan_s->create(sliders[cur_slider].name,TBS_VERT | TBS_LEFT | TBS_NOTICKS,
        r,this,IDg_CONTROL2+cur_slider,kMETHOD_TRANS);
    sliders[cur_slider].pan_s->SetWindowText(sliders[cur_slider].name);
    sliders[cur_slider].pan_s->set_range(0,255);
    sliders[cur_slider].pan_s->set_pos(sliders[cur_slider].pan);
   }

   if(sliders[cur_slider].type&TYPE_DB)
   {
       // fill-in kRect r;
       r.left=2; r.top=60;
       if(!get_slider_profile(ref_slider,templ_slider,
       (sliders[cur_slider].type&TYPE_STEREO)?"center_pos":"center_pos_mono",tmp_str,sizeof(tmp_str)))
         sscanf(tmp_str,"%d %d",&r.left,&r.top);

       r.left+=x; r.top+=y;
       r.right=r.left+5;
       r.bottom=r.top+5;

       sliders[cur_slider].center=new kButton;

       sliders[cur_slider].center->create((LPCTSTR)mf.get_profile("locals","center"),BS_OWNERDRAW,
          r,this,
          IDg_CONTROL_CENTER+cur_slider,kMETHOD_TRANS);

       sliders[cur_slider].center->set_bitmaps(mf.load_image("mixer/buttons/zero.bmp"),mf.load_image("mixer/buttons/zero_o.bmp"),mf.load_image("mixer/buttons/zero_s.bmp"),mf.load_image("mixer/buttons/zero_o.bmp"));
       sliders[cur_slider].center->size_to_content();
   }

   chrono(" -- slider -- slider created");
  } // knob
  else
  {
   // TODO FIXME NOW !!!
   sliders[cur_slider].knob=new kKnob();
  }

  if(sl_indi)
  {
   // add 'indicators'
   int sl_x1=0,sl_y1=0,sl_wd=40,sl_ht=14,sl_wd_gap=3,sl_ht_gap=3;
   int sl_x2=0,sl_y2=0,noc=6;

   if(sliders[cur_slider].type&TYPE_STEREO)
   {
     if(!get_slider_profile(ref_slider,templ_slider,"stereo_indi_align",tmp_str,sizeof(tmp_str)))
     {
      sscanf(tmp_str,"%d %d %d %d %d %d %d %d %d",&sl_x1,&sl_y1,&sl_x2,&sl_y2,&sl_wd,&sl_ht,&sl_wd_gap,&sl_ht_gap,&noc);
     }
   }
   else
   {
     if(!get_slider_profile(ref_slider,templ_slider,"mono_indi_align",tmp_str,sizeof(tmp_str)))
     {
      sscanf(tmp_str,"%d %d %d %d %d %d %d",&sl_x1,&sl_y1,&sl_wd,&sl_ht,&sl_wd_gap,&sl_ht_gap,&noc);
     }
   }
   COLORREF fg=0xffffff;
   COLORREF bk=0xb0b000;
   COLORREF brd=0x0;

   if(!get_slider_profile(ref_slider,templ_slider,"indi_colors",tmp_str,sizeof(tmp_str)))
   {
    sscanf(tmp_str,"%x %x %x",&fg,&bk,&brd);
   }

   // create
   sliders[cur_slider].indi_l=new kIndicator;
   sliders[cur_slider].indi_l->create("indi_l","0dB",this,
        x+sl_x1,y+sl_y1,noc,sl_wd,sl_ht,sl_wd_gap,sl_ht_gap);
   sliders[cur_slider].indi_l->set_colors(fg,bk,brd);
   
   if(sliders[cur_slider].type&TYPE_STEREO)
   {
     sliders[cur_slider].indi_r=new kIndicator;
     sliders[cur_slider].indi_r->create("indi_r","0dB",this,
            x+sl_x2,y+sl_y2,noc,sl_wd,sl_ht,sl_wd_gap,sl_ht_gap);
     sliders[cur_slider].indi_r->set_colors(fg,bk,brd);
   }
   if(update_indi_info(cur_slider))
   {
    // failed
    if(sliders[cur_slider].indi_l)
    {
     destroy(sliders[cur_slider].indi_l);
     delete sliders[cur_slider].indi_l;
     sliders[cur_slider].indi_l=0;
    }
    if(sliders[cur_slider].indi_r)
    {
     destroy(sliders[cur_slider].indi_r);
     delete sliders[cur_slider].indi_r;
     sliders[cur_slider].indi_r=0;
    }
   }

   chrono(" -- slider -- indicator created");
  }

  kRect r;

   if(sliders[cur_slider].type&TYPE_MUTE)
   {
    sliders[cur_slider].mute_b=new kButton;

    int sl_x1=0,sl_y1=0;
        if(!get_slider_profile(ref_slider,templ_slider,(sliders[cur_slider].type&TYPE_STEREO)?"mute_align":"mute_align_mono",tmp_str,sizeof(tmp_str)))
        {
         sscanf(tmp_str,"%d %d",&sl_x1,&sl_y1);
        }
        r.left=sl_x1+x;
        r.right=r.left+20;
        r.top=sl_y1+y;
        r.bottom=r.top+20;

    sliders[cur_slider].mute_b->create(sliders[cur_slider].mute?(LPCTSTR)mf.get_profile("locals","unmute"):(LPCTSTR)mf.get_profile("locals","mute"),BS_OWNERDRAW,r,this,IDg_CONTROL_MUTE+cur_slider,kMETHOD_TRANS);

    if(!sliders[cur_slider].mute)
        sliders[cur_slider].mute_b->set_bitmaps(mf.load_image("mixer/buttons/point.bmp"),mf.load_image("mixer/buttons/point_o.bmp"),mf.load_image("mixer/buttons/point_s.bmp"),mf.load_image("mixer/buttons/point_o.bmp"));
    else
        sliders[cur_slider].mute_b->set_bitmaps(mf.load_image("mixer/buttons/point_off.bmp"),mf.load_image("mixer/buttons/point_off_o.bmp"),mf.load_image("mixer/buttons/point_off_s.bmp"),mf.load_image("mixer/buttons/point_o.bmp"));

    sliders[cur_slider].mute_b->size_to_content();
   }
   if(sliders[cur_slider].omni!=-1)
   {
    int sl_x1=0,sl_y1=0;
        if(!get_slider_profile(ref_slider,templ_slider,"linked_align",tmp_str,sizeof(tmp_str)))
        {
         sscanf(tmp_str,"%d %d",&sl_x1,&sl_y1);
        }

        r.left=sl_x1+x;
        r.right=r.left+20;
        r.top=sl_y1+y;
        r.bottom=r.top+20;

    sliders[cur_slider].omni_b=new kButton;
    sliders[cur_slider].omni_b->create(sliders[cur_slider].omni?(LPCTSTR)mf.get_profile("locals","unlinked"):(LPCTSTR)mf.get_profile("locals","linked"),BS_OWNERDRAW,r,this,IDg_CONTROL_OMNI+cur_slider,kMETHOD_TRANS);

    if(sliders[cur_slider].omni)
        sliders[cur_slider].omni_b->set_bitmaps(mf.load_image("mixer/buttons/omni.bmp"),mf.load_image("mixer/buttons/omni_o.bmp"),mf.load_image("mixer/buttons/omni_s.bmp"),mf.load_image("mixer/buttons/omni_o.bmp"));
    else
        sliders[cur_slider].omni_b->set_bitmaps(mf.load_image("mixer/buttons/omni_off.bmp"),mf.load_image("mixer/buttons/omni_off_o.bmp"),mf.load_image("mixer/buttons/omni_off_s.bmp"),mf.load_image("mixer/buttons/omni_o.bmp"));

    sliders[cur_slider].omni_b->size_to_content();
   }

   chrono(" -- slider -- mute/omni buttons added");

   cur_slider++;
 } else return -1;

 return 0; 
}

int CMainDialog::update_indi_info(int i)
{
 TCHAR value[32];
 int val_l=0,val_r=0;

 int l_muted=0,r_muted=0;

 if(sliders[i].mute)
  { l_muted=1; r_muted=1; }

 if(sliders[i].mute_b==0)
  { l_muted=0; r_muted=0; }

 char *format=NULL;

 if(sliders[i].indi_l && sliders[i].vol_s) 
  val_l=sliders[i].vol_s->get_pos();

 if(sliders[i].indi_r && sliders[i].pan_s) 
  val_r=sliders[i].pan_s->get_pos();

/* if()
 {
  // 0xff-(val>>23)
  if(val_l!=255)
   val_l=(1/((255-val_l)/255))*(-6);
  else
   l_muted=1;

  if(val_r!=255)
   val_r=(1/((255-val_r)/255))*(-6);
  else
   r_muted=1;

  format="%2.0f";
 }
  else
*/
 if((sliders[i].type&TYPE_AMOUNT)||(sliders[i].type&TYPE_MIXER)||(sliders[i].type&TYPE_DSP)||(sliders[i].type&TYPE_AGC))
 {
  if(sliders[i].type&TYPE_DB)
  {
     // (20.0f*logf((float)val/2147483647.0f)+12.041199826559f+144.535597741225f)*1.628593788869f
     val_l=(int)((255.0f-(float)val_l)/RES_FACTOR-RES_MIN);
     val_r=(int)((255.0f-(float)val_r)/RES_FACTOR-RES_MIN);

     if(val_l<=(int)(-RES_MIN)) l_muted=1;
     if(val_r<=(int)(-RES_MIN)) r_muted=1;

     format="%d";
  }
  else
  if(sliders[i].type&TYPE_NEGATIVE)
  {
     val_l=100-val_l*100/255;
     val_r=100-val_r*100/255;

     if(val_l==0)
      l_muted=1;

     if(val_r==0)
      r_muted=1;

     format="%d";
  }
  else
  {
     val_l=100-val_l*100/255;
     val_r=100-val_r*100/255;

     if(val_l==0)
      l_muted=1;

     if(val_r==0)
      r_muted=1;

     format="%d";
  }
 }
  else
 if(sliders[i].type&TYPE_AC97)
 {
  int scale=-3*(1<<sliders[i].bits)/2;
  int med=0;

  switch(sliders[i].id)
  {
    case AC97_REG_PHONE_VOL: // tad in
    case AC97_REG_LINE_VOL: // line
    case AC97_REG_CD_VOL: // cd
    case AC97_REG_VIDEO_VOL: // vid
    case AC97_REG_AUX_VOL: // aux
    case AC97_REG_PCM_VOL:
    case AC97_REG_MIC_VOL: // mic
        med=64; // 1000B (8) is 0dB
        break;
    case AC97_REG_REC_GAIN:
// 3537c: scale=-scale;
// 3537d:
    scale=-scale;
        val_l=255-val_l;
        val_r=255-val_r;
        break;
  }
  word v;
  if((ikx_t->ac97_read((byte)sliders[i].id,&v)==0))
  {
    if((sliders[i].id==AC97_REG_MIC_VOL)&&(v&0x40))
    {
      med+=107;
    }
    if(v&0x8000)
    {
     l_muted=1;
     r_muted=1;
    }
  }
  format="%2d";
  val_l=(val_l-med)*scale/255;
  val_r=(val_r-med)*scale/255;
 }

 if(sliders[i].indi_l && sliders[i].vol_s) 
 {
  if(!l_muted)
  {
    sprintf(value,format,val_l);
    sliders[i].indi_l->set_value(value);
  }
  else
    sliders[i].indi_l->set_value("~inf");
 }

 if(sliders[i].indi_r && sliders[i].pan_s) 
 {
  if(!r_muted)
  {
    sprintf(value,format,val_r);
    sliders[i].indi_r->set_value(value);
  }
  else
    sliders[i].indi_r->set_value("~inf");
 }

 return 0;
}

int CMainDialog::create_page(int page)
{
 char *page_name=get_mixer_page_guid(page);
 if(page_name==NULL || page_name[0]==0)
 {
  debug("kxmixer: could not create page %d\n");
  return -1;
 }

 int ret=create_page(page_name);
/*
 for(i=0;i<MAX_PAGES;i++)
 {
  if(::IsWindow(menu_pages[i]))
  {
    menu_pages[i].set_default(i==page?1:0);
    menu_pages[i].RedrawWindow();
  }
 }
*/
 return ret;
}


int CMainDialog::create_page(char *page_name)
{
 char tmp_str[128];
 cur_static=0;
 cur_slider=0;

 chrono(NULL);
 chrono("-- start creating the page");

 ::set_font(this,font,page_name);

 if(font.get_font())
 {
  ac97_source.SetFont(font);
  ac97_3dse.SetFont(font);
  ac3_passthru.SetFont(font);
  dsp_freq.SetFont(font);
  sf_list.SetFont(font);
  sf_info.SetFont(font);
  set_tool_font(font);
  ecard_spdif0.SetFont(font);
  ecard_spdif1.SetFont(font);
  spdif_recording.SetFont(font);
  zsnb_source.SetFont(font);
  p16v_recording.SetFont(font);
 }

 if(ikx_t->get_hw_parameter(KX_HW_DOO,(dword *)&is_doo))
  is_doo=0; // error

 // analyze [page] section
 int type=1;
 if(!mf.get_profile(page_name,"type",tmp_str,sizeof(tmp_str)))
 {
  if(strstr(tmp_str,"skin")!=0) type=1; else type=0;
 }

 if(type==1)
 {
     if(mf.get_profile(page_name,"background",tmp_str,sizeof(tmp_str)))
     {
      tmp_str[0]=0;
     }

     unsigned int cc_method=0;
     unsigned int transp_color=0;
     unsigned int alpha=90;

     gui_set_background(this,&mf,tmp_str,transp_color,alpha,cc_method,_T(""));
 }
 else
 {
    int wd=600;
    int ht=340;
    if(!mf.get_profile(page_name,"wd",tmp_str,sizeof(tmp_str)))
    {
     sscanf(tmp_str,"%d",&wd);
    }
    if(!mf.get_profile(page_name,"ht",tmp_str,sizeof(tmp_str)))
    {
     sscanf(tmp_str,"%d",&ht);
    }
    VERIFY(SetWindowPos(NULL, -1, -1, wd,ht,
    SWP_NOMOVE|SWP_NOZORDER|SWP_NOREDRAW|SWP_NOACTIVATE));
 }
 chrono("-- background created");

 if(!mf.get_profile(page_name,"tooltip",tmp_str,sizeof(tmp_str)))
 {
  add_tool(tmp_str,this,0);
 }

 // process main buttons:
 dword button_flag=0;
 dword button_flag2=0;

 if(!mf.get_profile(page_name,"sfload_button",tmp_str,sizeof(tmp_str)))
 {
  create_button(&b_sfload,IDg_SFLOAD,tmp_str);
  if(::IsWindow(b_sfload)) button_flag|=4;
 }
 if(!mf.get_profile(page_name,"sfunload_button",tmp_str,sizeof(tmp_str)))
 {
  create_button(&b_sfunload,IDg_SFUNLOAD,tmp_str);
  if(::IsWindow(b_sfunload)) button_flag|=8;
 }
 if(!mf.get_profile(page_name,"sfreload_button",tmp_str,sizeof(tmp_str)))
 {
  create_button(&b_sfreload,IDg_SFRELOAD,tmp_str);
  if(::IsWindow(b_sfreload)) button_flag|=0x2000000;
 }
 if(!mf.get_profile(page_name,"sfunloadall_button",tmp_str,sizeof(tmp_str)))
 {
  create_button(&b_sfunloadall,IDg_SFUNLOADALL,tmp_str);
  if(::IsWindow(b_sfunloadall)) button_flag2|=0x4;
 }
 if(!mf.get_profile(page_name,"sfcload_button",tmp_str,sizeof(tmp_str)))
 {
  create_button(&b_sfcload,IDg_SFCLOAD,tmp_str);
  if(::IsWindow(b_sfcload)) button_flag|=0x4000000;
 }
 if(!mf.get_profile(page_name,"sfcsave_button",tmp_str,sizeof(tmp_str)))
 {
  create_button(&b_sfcsave,IDg_SFCSAVE,tmp_str);
  if(::IsWindow(b_sfcsave)) button_flag|=0x8000000;
 }
 if(!mf.get_profile(page_name,"spectrum_button",tmp_str,sizeof(tmp_str)))
 {
  create_button(&b_specmode,IDg_SPECMODE,tmp_str);
  if(::IsWindow(b_specmode)) button_flag|=0x10;
 }
 if(!mf.get_profile(page_name,"ac97_source",tmp_str,sizeof(tmp_str)) && has_ac97)
 {
  if(create_combo(&ac97_source,IDg_AC97SOURCE,tmp_str)==0 && ::IsWindow(ac97_source)) 
   button_flag|=0x20;
 }

 word val;
 if((!mf.get_profile(page_name,"ac97_3dse",tmp_str,sizeof(tmp_str)))
   && (has_ac97)
   && ((!ikx_t->ac97_read(AC97_REG_RESET,&val)) && ((val >> 10) & 0x1f)) // supported
   )
 {
  if(create_combo(&ac97_3dse,IDg_AC973DSE,tmp_str)==0 && ::IsWindow(ac97_3dse)) 
    button_flag|=0x40;
 }

 kRect r;
 if(!mf.get_profile(page_name,"spectrum",tmp_str,sizeof(tmp_str)))
 {
   sscanf(tmp_str,"%d %d %d %d",&r.left,&r.top,&r.right,&r.bottom);
   spectrum.reposition(r);
   spectrum.turn_on(1);

   kSettings cfg;
   dword smethod=0;
   cfg.read_abs("Mixer","SpecMode",&smethod);

   spectrum.set_view(smethod);
   button_flag|=0x80;
   if(button_flag&0x10) // spectrum button
    button_flag|=0x100;
 }

 int need_sf_info=0;
 if(!mf.get_profile(page_name,"sf_list",tmp_str,sizeof(tmp_str)))
 {
   r.left=75; r.top=15; r.right=545; r.bottom=120;

   sscanf(tmp_str,"%d %d %d %d",&r.left,&r.top,&r.right,&r.bottom);

   sf_list.reposition(r);
   button_flag|=0x200;
   need_sf_info=1;
 }
 if(!mf.get_profile(page_name,"sf_mem",tmp_str,sizeof(tmp_str)))
 {
   need_sf_info=1;
 }

 chrono("-- buttons parsed");

 int total_sf_size=0;
 int total_sf2s=0;
 if(need_sf_info)
 {
    int sz;
    sz=ikx_t->enum_soundfonts(0,0);
    total_sf_size=0;
    sf_list.reset();
    if(sz>0)
    {
         sfHeader *hdr;
         hdr=(sfHeader *)malloc(sz);
         if(hdr)
         {
            if(!ikx_t->enum_soundfonts(hdr,sz))
            {
               for(dword i=0;i<sz/sizeof(sfHeader);i++)
               {
                   kString s;
                   // "#%d" is linked!
                   s.Format("#%d (%s %d) '%s' [%d KB]",
                    hdr[i].rom_ver.minor,
                    mf.get_profile("locals","bank"),
                    hdr[i].sfman_id,
//                    hdr[i].subsynth?(hdr[i].subsynth==1?"Synth1":"Synth2"):"",
                    hdr[i].name,hdr[i].sample_len/1024);
                   sf_list.add_string((LPCTSTR)s);
                   total_sf_size+=hdr[i].sample_len;
                   total_sf2s++;
               }
               total_sf2s=sz/sizeof(sfHeader);
            }
            free(hdr);
         }
    }  else 
    {
      sf_list.add_string((LPCTSTR)mf.get_profile("locals","no_sfs")); 
    }
 } // need_sf_info

 if(!mf.get_profile(page_name,"sf_info",tmp_str,sizeof(tmp_str)))
 {
   r.left=75; r.top=130; r.right=545; r.bottom=230;
   sscanf(tmp_str,"%d %d %d %d",&r.left,&r.top,&r.right,&r.bottom);

   sf_info.reposition(r);
   sf_info.reset();
   if(total_sf2s>0)
   {
    kString s;
    s.Format((LPCTSTR)mf.get_profile("locals","sf"),
      total_sf2s,total_sf2s==1?"":"s",
      total_sf_size/1024);
    sf_info.add_string(s);
   }
   button_flag|=0x400;
 }

 if(!mf.get_profile(page_name,"sf_mem",tmp_str,sizeof(tmp_str)))
 {
   r.left=180; r.top=240; r.right=545; r.bottom=260;
   COLORREF bar_color=0xffff00,bk_color=0x9a1b12;

   sscanf(tmp_str,"%d %d %d %d %x %x",&r.left,&r.top,&r.right,&r.bottom,&bar_color,&bk_color);
   sf_progress.reposition(r);

   MEMORYSTATUS st;
   st.dwLength=sizeof(MEMORYSTATUS);
   GlobalMemoryStatus(&st);

   int pos;
   pos=(int)((__int64)total_sf_size*(__int64)100/((__int64)st.dwTotalPhys/3));
      
   sf_progress.set_range(0,100);
   sf_progress.set_pos(pos);
   sf_progress.set_step(3);
   sf_progress.set_bar_color(bar_color);
   sf_progress.set_bk_color(bk_color);

   kString s;
   s.Format("%s: %d KB (%d%%)",(LPCTSTR)mf.get_profile("locals","mem"),total_sf_size/1024,pos);
   add_tool((LPTSTR)(LPCTSTR)s,&sf_progress,IDg_SFMEM);
   button_flag|=0x800;
 }

 chrono("-- soundfont data processed");

  // buttons
  char button_num[16];
  int i=0;
  for(int j=0;j<MAX_BUTTONS;j++)
  {
    sprintf(button_num,"button%d",j);

    if(!mf.get_profile(page_name,button_num,tmp_str,sizeof(tmp_str)))
    {
     create_button(&buttons[i],IDg_BUTTON+i,tmp_str,button_guids[i]);
     if(::IsWindow(buttons[i]))
     {
           buttons[i].show();
           i++;
     }  
    }
    else break;
  }
  for(int j=0;j<MAX_BUTTONS;j++)
  {
    sprintf(button_num,"button%d",j);

    if(!mf.get_profile("all",button_num,tmp_str,sizeof(tmp_str)))
    {
       create_button(&buttons[i],IDg_BUTTON+i,tmp_str,button_guids[i]);
       if(::IsWindow(buttons[i]))
       {
               buttons[i].show();
               i++;
       }
    }
    else break;
  }

 chrono("-- page buttons created");

 // label
 if(!mf.get_profile(page_name,"label",tmp_str,sizeof(tmp_str)))
 {
  dword fg,bk;
  sscanf(tmp_str,"%d %d %d %d %x %x",&r.left,&r.top,&r.right,&r.bottom,&fg,&bk);
  label.create((char *)(LPCTSTR)mf.get_profile(page_name,"label.text"),r.left,r.top,r.right,r.bottom,this,fg,bk);
  label.show();
 }

 // --- draw default window elements
 if(!mf.get_profile(page_name,"quit_button",tmp_str,sizeof(tmp_str)))
 {
  create_button(&b_quit,IDg_QUIT,tmp_str);
  b_quit.show();
 } 
  else
 if(!mf.get_profile("all","quit_button",tmp_str,sizeof(tmp_str)))
 {
  create_button(&b_quit,IDg_QUIT,tmp_str);
  b_quit.show();
 }
 if(!mf.get_profile(page_name,"mini_button",tmp_str,sizeof(tmp_str)))
 {
  create_button(&b_mini,IDg_MINI,tmp_str);
  b_mini.show();
 }
  else
 if(!mf.get_profile("all","mini_button",tmp_str,sizeof(tmp_str)))
 {
  create_button(&b_mini,IDg_MINI,tmp_str);
  b_mini.show();
 }

 dword val2;
 if((!mf.get_profile(page_name,"dsp_freq",tmp_str,sizeof(tmp_str))) &&
   (!ikx_t->get_hw_parameter(KX_HW_SPDIF_FREQ,&val2))
   )
 {
   if(create_combo(&dsp_freq,IDg_DSPFREQ,tmp_str)==0 && ::IsWindow(dsp_freq)) 
    button_flag|=0x1000;
 }

 if((!mf.get_profile(page_name,"dsp_doo",tmp_str,sizeof(tmp_str))) &&
   (!ikx_t->get_hw_parameter(KX_HW_DOO,&val2))
   )
 {
  create_button(&b_doo,IDg_DOO,tmp_str);
  if(::IsWindow(b_doo)) button_flag|=0x2000;
 }
 if((!mf.get_profile(page_name,"spdif_decode",tmp_str,sizeof(tmp_str))) &&
   (!ikx_t->get_hw_parameter(KX_HW_SPDIF_DECODE,&val2))
   )
 {
  create_button(&b_spdif_decode,IDg_SPDIF_DECODE,tmp_str);
  if(::IsWindow(b_spdif_decode)) button_flag|=0x20000000;
 }
 if((!mf.get_profile(page_name,"dsp_swap_rear",tmp_str,sizeof(tmp_str))) &&
   (!ikx_t->get_hw_parameter(KX_HW_SWAP_FRONT_REAR,&val2))
   )
 {
  create_button(&b_swap_rear,IDg_SWAPREAR,tmp_str);
  if(::IsWindow(b_swap_rear)) button_flag|=0x4000;
 }
 if((!mf.get_profile(page_name,"dsp_bypass",tmp_str,sizeof(tmp_str))) &&
   (!ikx_t->get_hw_parameter(KX_HW_SPDIF_BYPASS,&val2))
   )
 {
  create_button(&b_bypass,IDg_BYPASS,tmp_str);
  if(::IsWindow(b_bypass)) button_flag|=0x8000;
 }
 if((!mf.get_profile(page_name,"dsp_passthru",tmp_str,sizeof(tmp_str))) &&
   (!ikx_t->get_hw_parameter(KX_HW_AC3_PASSTHROUGH,&val2))
   )
 {
  if(create_combo(&ac3_passthru,IDg_PASSTHRU,tmp_str)==0 && ::IsWindow(ac3_passthru)) 
   button_flag|=0x400000;
 }
 /*
 if((!mf.get_profile(page_name,"dsp_detect_phones",tmp_str,sizeof(tmp_str))) &&
   (!ikx_t->get_hw_parameter(KX_HW_DETECT_HEADPHONES,&val2))
   )
 {
  create_button(&b_phones,IDg_PHONES,tmp_str);
  if(::IsWindow(b_phones)) button_flag|=0x10000;
 }
 */
 if((!mf.get_profile(page_name,"dsp_route_csw",tmp_str,sizeof(tmp_str))) &&
   (!ikx_t->get_hw_parameter(KX_HW_ROUTE_PH_TO_CSW,&val2))
   )
 {
  create_button(&b_p_csw,IDg_P_CSW,tmp_str);
  if(::IsWindow(b_p_csw)) button_flag|=0x20000;
 }
 if(!mf.get_profile(page_name,"ac97_reset",tmp_str,sizeof(tmp_str)))
 {
  create_button(&b_ac97_reset,IDg_AC97RESET,tmp_str);
  if(::IsWindow(b_ac97_reset)) button_flag|=0x80000;
 }
 if(!mf.get_profile(page_name,"ac97_a2",tmp_str,sizeof(tmp_str)))
 {
  create_button(&b_ac97_a2,IDg_AC97_A2,tmp_str);
  if(::IsWindow(b_ac97_a2)) button_flag2|=0x8;
 }

 device_init(page_name);
 button_flag|=0x200000;

 if(!mf.get_profile(page_name,"ac97_micboost",tmp_str,sizeof(tmp_str)))
 {
  create_button(&b_ac97_micboost,IDg_AC97_MICBOOST,tmp_str);
  if(::IsWindow(b_ac97_micboost)) button_flag|=0x100000;
 }

 if((!mf.get_profile(page_name,"ecard_spdif0",tmp_str,sizeof(tmp_str))) &&
   (!ikx_t->get_hw_parameter(KX_HW_ECARD_ROUTING,&val2))
   )
 {
   if(create_combo(&ecard_spdif0,IDg_ECARD_SPDIF0,tmp_str)==0 && ::IsWindow(ecard_spdif0)) 
     button_flag|=0x800000;
 }

 if((!mf.get_profile(page_name,"ecard_spdif1",tmp_str,sizeof(tmp_str))) &&
   (!ikx_t->get_hw_parameter(KX_HW_ECARD_ROUTING,&val2))
   )
 {
   if(create_combo(&ecard_spdif1,IDg_ECARD_SPDIF1,tmp_str)==0 && ::IsWindow(ecard_spdif1)) 
     button_flag|=0x1000000;
 }

 if((!mf.get_profile(page_name,"spdif_recording",tmp_str,sizeof(tmp_str))) &&
   (!ikx_t->get_hw_parameter(KX_HW_SPDIF_RECORDING,&val2))
   )
 {
   if(create_combo(&spdif_recording,IDg_SPDIF_RECORDING,tmp_str)==0 && ::IsWindow(spdif_recording)) 
     button_flag|=0x80000000;
 }

 if((!mf.get_profile(page_name,"zsnb_source",tmp_str,sizeof(tmp_str))) &&
   (!ikx_t->get_hw_parameter(KX_HW_A2ZSNB_SOURCE,&val2))
   )
 {
   if(create_combo(&zsnb_source,IDg_ZSNB_SOURCE,tmp_str)==0 && ::IsWindow(zsnb_source)) 
     button_flag2|=0x10;
 }

 if((!mf.get_profile(page_name,"p16v_recording",tmp_str,sizeof(tmp_str))) &&
   (!ikx_t->get_hw_parameter(KX_HW_P16V_REC_ROUTING,&val2))
   )
 {
   if(create_combo(&p16v_recording,IDg_P16V_RECORDING,tmp_str)==0 && ::IsWindow(p16v_recording)) 
     button_flag2|=0x1;
 }

 chrono("-- additional buttons added");

 // sliders
 char slider_num[12];
 for(i=0;i<MAX_SLIDERS;i++)
 {
    sprintf(slider_num,"slider%d",i);
    if(!mf.get_profile(page_name,slider_num,tmp_str,sizeof(tmp_str)))
    {
     create_slider(tmp_str);
    }
 }

 chrono("-- sliders created");

 for(i=0;i<MAX_IMAGES;i++)
 {
  sprintf(slider_num,"image%d",i);
  if(!mf.get_profile(page_name,slider_num,tmp_str,sizeof(tmp_str)))
  {
     int x=0,y=0;
     int method=kMETHOD_TRANS;
     char img[512]; img[0]=0;
     char smethod[32]; smethod[0]=0;
     sscanf(tmp_str,"%d %d %s %s",&x,&y,img,smethod);
     HBITMAP bm=mf.load_image(img);
     if(strstr(smethod,"copy")!=0)
      method=kMETHOD_COPY;
     static_images[cur_static].create("name",x,y,bm,this,method);
     static_images[cur_static].size_to_content();
     cur_static++;
     if(cur_static>=MAX_IMAGES)
      break;
  }
 }

 chrono("-- static images created");

 if(button_flag&0x4) b_sfload.show();
 // if(button_flag&0x8) b_sfunload.show();
 if(button_flag&0x10) b_specmode.show();
 if(button_flag&0x20) ac97_source.show();
 if(button_flag&0x40) ac97_3dse.show();
 if(button_flag&0x80) spectrum.show();
 if(button_flag&0x100) b_specmode.show();
 if(button_flag&0x200) sf_list.show();
 if(button_flag&0x400) sf_info.show();
 if(button_flag&0x800) sf_progress.show();
 if(button_flag&0x1000) dsp_freq.show();
 if(button_flag&0x2000) b_doo.show();
 if(button_flag&0x4000) b_swap_rear.show();
 if(button_flag&0x8000) b_bypass.show();
// if(button_flag&0x10000) b_phones.show();
 if(button_flag&0x20000) b_p_csw.show();
 if(button_flag&0x80000) b_ac97_reset.show();
 if(button_flag&0x100000) b_ac97_micboost.show();
 if(button_flag&0x200000) card_name.show();
 if(button_flag&0x400000) ac3_passthru.show();
 if(button_flag&0x800000) ecard_spdif0.show();
 if(button_flag&0x1000000) ecard_spdif1.show();
// if(button_flag&0x2000000) b_sfreload.show();
 if(button_flag&0x4000000) b_sfcload.show();
 if(button_flag&0x8000000) b_sfcsave.show();
 if(button_flag&0x20000000) b_spdif_decode.show();
 if(button_flag&0x80000000) spdif_recording.show();

 if(button_flag2&0x1) p16v_recording.show();
 if(button_flag2&0x4) b_sfunloadall.show();
 if(button_flag2&0x8) b_ac97_a2.show();
 if(button_flag2&0x10) zsnb_source.show();

 chrono("-- subwindows shown");

 for(i=0;i<MAX_IMAGES;i++)
 {
  if(::IsWindow(static_images[i])) static_images[i].show();
 }

 chrono("-- static images shown");

 for(i=0;i<MAX_SLIDERS;i++)
 {
  if(sliders[i].type)
  {
   if(sliders[i].vol_s) sliders[i].vol_s->show();
   if(sliders[i].pan_s) sliders[i].pan_s->show();
   if(sliders[i].mute_b) sliders[i].mute_b->show();
   if(sliders[i].center) sliders[i].center->show();
   if(sliders[i].omni_b) sliders[i].omni_b->show();
   if(sliders[i].name_s) sliders[i].name_s->show();
   if(sliders[i].edit) sliders[i].edit->ShowWindow(SW_SHOW);
   if(sliders[i].indi_l) sliders[i].indi_l->show();
   if(sliders[i].indi_r) sliders[i].indi_r->show();
   if(sliders[i].knob) sliders[i].knob->show();
  }
 }

 chrono("-- sliders shown");

 return 0;
}

int CMainDialog::destroy_page()
{
 if(cur_page!=-1)
 {
  for(int i=0;i<MAX_BUTTONS;i++)
   destroy(&buttons[i]);

  destroy(&b_sfload);
  destroy(&b_specmode);
  destroy(&b_sfunload);
  destroy(&b_sfunloadall);
  destroy(&b_sfreload);
  destroy(&b_sfcload);
  destroy(&b_sfcsave);

  destroy(&b_quit);
  destroy(&b_mini);

  destroy(&b_ac97_reset);
  destroy(&b_ac97_a2);
  destroy(&b_doo);
  destroy(&b_spdif_decode);
  destroy(&card_name);
//  destroy(&b_phones);
  destroy(&b_bypass);
  destroy(&b_p_csw);
  destroy(&b_swap_rear);
  destroy(&b_ac97_micboost);

  destroy(&label);

  for(int i=0;i<MAX_IMAGES;i++)
  {
   destroy(&static_images[i]);
  }

  for(int i=0;i<MAX_SLIDERS;i++)
  {
   if(sliders[i].vol_s)
   {
    destroy(sliders[i].vol_s);
    delete sliders[i].vol_s;
    sliders[i].vol_s=0;
   }
   if(sliders[i].pan_s)
   {
    destroy(sliders[i].pan_s);
    delete sliders[i].pan_s;
    sliders[i].pan_s=0;
   }
   if(sliders[i].edit)
   {
    // destroy(sliders[i].edit);
    sliders[i].edit->DestroyWindow();
    delete sliders[i].edit;
    sliders[i].edit=0;
   }
   if(sliders[i].name_s)
   {
    destroy(sliders[i].name_s);
    delete sliders[i].name_s;
    sliders[i].name_s=0;
   }
   if(sliders[i].omni_b)
   {
    destroy(sliders[i].omni_b);
    delete sliders[i].omni_b;
    sliders[i].omni_b=0;
   }
   if(sliders[i].mute_b)
   {
    destroy(sliders[i].mute_b);
    delete sliders[i].mute_b;
    sliders[i].mute_b=0;
   }
   if(sliders[i].center)
   {
    destroy(sliders[i].center);
    delete sliders[i].center;
    sliders[i].center=0;
   }
   if(sliders[i].indi_l)
   {
    destroy(sliders[i].indi_l);
    delete sliders[i].indi_l;
    sliders[i].indi_l=0;
   }
   if(sliders[i].indi_r)
   {
    destroy(sliders[i].indi_r);
    delete sliders[i].indi_r;
    sliders[i].indi_r=0;
   }
   if(sliders[i].knob)
   {
    destroy(sliders[i].knob);
    delete sliders[i].knob;
    sliders[i].knob=0;
   }
   memset(&sliders[i],0,sizeof(slider_t));
  }

  sf_list.hide();
  sf_info.hide();
  sf_progress.hide();

  spectrum.turn_on(0);
  spectrum.hide();

  ac97_source.hide();
  ac97_3dse.hide();
  dsp_freq.hide();
  ac3_passthru.hide();
  ecard_spdif0.hide();
  ecard_spdif1.hide();
  spdif_recording.hide();
  zsnb_source.hide();
  p16v_recording.hide();
  cur_page=-1;
 }
 return 0;
}

void CMainDialog::on_destroy()
{
    int last_page=cur_page;
    if(cur_page!=-1)
    {
                kSettings cfg;
                cfg.write_abs("Mixer","DefPage",(unsigned int)cur_page);

                destroy_page();

                destroy(&spectrum);

                destroy(&sf_list);
                destroy(&sf_info);
                destroy(&sf_progress);

                destroy(&ac97_source);
                destroy(&ac97_3dse);
                destroy(&ac3_passthru);
                destroy(&dsp_freq);

                destroy(&ecard_spdif0);
                destroy(&ecard_spdif1);
                destroy(&spdif_recording);
                destroy(&zsnb_source);
                destroy(&p16v_recording);

                manager->save_settings(SETTINGS_AUTO);

                cur_page=-1;
        }
        //WinHelp(0L, HELP_QUIT);
    CKXDialog::on_destroy();
}


int CMainDialog::on_command(int wParam, int lParam)
{
    if((wParam>=IDg_BUTTON) && (wParam<IDg_CONTROL))
    {
        debug("kxmixer: launch %d (%s)\n",wParam-IDg_BUTTON,button_guids[wParam-IDg_BUTTON]);

        if(!manager->launch(button_guids[wParam-IDg_BUTTON])) // success?
         return 1;
        else
        {
         debug("kxmixer: invalid!\n");
         return 1;
        }
    }
    if(wParam==0xf0f0)
    {
        destroy_page();
        debug("kxmixer: create page: %d\n",lParam);
        cur_page=(int)lParam;
        create_page(cur_page);
        return 1;
    }
    switch(wParam)
    {
                case IDg_AC97RESET:
                    ikx_t->ac97_reset();
                    SendMessage(WM_COMMAND,0xf0f0,cur_page);
                    return 1;
                case IDg_AC97_A2:
                    {
                     dword v=0;
                     if(ikx_t->get_hw_parameter(KX_HW_K2_AC97,&v)==0)
                     {
                      v&=1;
                      ikx_t->set_hw_parameter(KX_HW_K2_AC97,1-v);

                      SendMessage(WM_COMMAND,0xf0f0,cur_page);
                     }
                     else
                      debug("kxmixer: error (re)setting ac97-a2 parameter [0x19]\n");
                    }
                    return 1;
                case IDg_DOO:
                    {
                     dword v;
                     if(!ikx_t->get_hw_parameter(KX_HW_DOO,&v))
                     {
                      ikx_t->set_hw_parameter(KX_HW_DOO,1-v);
                      SendMessage(WM_COMMAND,0xf0f0,cur_page);
                      }
                     else
                      MessageBox("Error setting 'Digital Output Only mode'");
                    }
                    return 1;
                case IDg_SPDIF_DECODE:
                    {
                     dword v;
                     if(!ikx_t->get_hw_parameter(KX_HW_SPDIF_DECODE,&v))
                     {
                      ikx_t->set_hw_parameter(KX_HW_SPDIF_DECODE,1-(v?1:0));
                      SendMessage(WM_COMMAND,0xf0f0,cur_page);
                      }
                     else
                      MessageBox("Error setting 'SPDIF Decode' mode");
                    }
                    return 1;
                    
                case IDg_SWAPREAR:
                    {
                     dword v;
                     if(!ikx_t->get_hw_parameter(KX_HW_SWAP_FRONT_REAR,&v))
                     {
                      ikx_t->set_hw_parameter(KX_HW_SWAP_FRONT_REAR,1-v);
                      // reload_custom_epilog();
                      SendMessage(WM_COMMAND,0xf0f0,cur_page);
                      }
                     else
                      MessageBox("Error setting 'Swap Front & Rear mode'");
                    }
                    return 1;
                case IDg_BYPASS:
                    {
                     dword v;
                     if(!ikx_t->get_hw_parameter(KX_HW_SPDIF_BYPASS,&v))
                     {
                      ikx_t->set_hw_parameter(KX_HW_SPDIF_BYPASS,1-(v?1:0));
                          SendMessage(WM_COMMAND,0xf0f0,cur_page);
                     }
                     else
                      MessageBox("Error setting 'SPDIF Bypass' mode");
                    }
                    return 1;
                case IDg_PASSTHRU:
                    {
                     dword v;
                     if(!ikx_t->get_hw_parameter(KX_HW_AC3_PASSTHROUGH,&v))
                     {
                      v=1-v;

                      ikx_t->set_hw_parameter(KX_HW_AC3_PASSTHROUGH,v);

              SendMessage(WM_COMMAND,0xf0f0,cur_page);
                     }
                     else
                      MessageBox("Error setting 'AC-3 Passthru' mode");
                    }
                    return 1;
                        /*
                case IDg_PHONES:
                    {
                     dword v;
                     if(!ikx_t->get_hw_parameter(KX_HW_DETECT_HEADPHONES,&v))
                     {
                      ikx_t->set_hw_parameter(KX_HW_DETECT_HEADPHONES,1-v);
                          SendMessage(WM_COMMAND,0xf0f0,cur_page);
                      }
                     else
                      MessageBox("Error setting 'Detect Headphones' mode");
                    }
                    return 1;
                        */
                case IDg_P_CSW:
                    {
                     dword v;
                     if(!ikx_t->get_hw_parameter(KX_HW_ROUTE_PH_TO_CSW,&v))
                     {
                      ikx_t->set_hw_parameter(KX_HW_ROUTE_PH_TO_CSW,1-v);
                      // reload_custom_epilog();
                      SendMessage(WM_COMMAND,0xf0f0,cur_page);
                      }
                     else
                      MessageBox("Error setting 'Route headphones to Center/Subwoofer' mode");
                    }
                    return 1;
                case IDg_AC97_MICBOOST:
                    {
                     word v;
                     if(!ikx_t->ac97_read(AC97_REG_MIC_VOL,&v))
                      ikx_t->ac97_write(AC97_REG_MIC_VOL,(v&0xffbf)|((v&0x40)?0:0x40));
                     else
                      MessageBox("Error setting 'Mic Boost' mode");
                    }
                    SendMessage(WM_COMMAND,0xf0f0,cur_page);
                    return 1;
        case IDg_SFLOAD:
             {  
                restore_cwd("sf2");
                        CFileDialog *f_d = new CFileDialog(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_EXPLORER|OFN_ALLOWMULTISELECT,
                          "SoundFont Files (*.sf2), sfArk Files (*.sfArk)|*.sf2;*.sfArk||",this);
                        if(f_d)
                        {
                            char tmp_cwd[MAX_PATH];
                            GetCurrentDirectory(sizeof(tmp_cwd),tmp_cwd);
                            f_d->m_ofn.lpstrInitialDir=tmp_cwd;

                            char file_list[256*40];
                            memset(file_list,0,sizeof(file_list));
                            f_d->m_ofn.lpstrFile=file_list;
                            f_d->m_ofn.nMaxFile=sizeof(file_list);

                            if(f_d->DoModal()==IDCANCEL)
                            {
                             delete f_d;
                             return 1;
                            }

                            save_cwd("sf2");

                            POSITION pos;
                                pos=f_d->GetStartPosition();

                            while(pos)
                            {
                                    kString fname=f_d->GetNextPathName(pos);

                                    if(fname.GetLength()==0) // cancel or empty
                                    {
                                     continue;
                                    }

                                    CSFBankDlg dlg;
                                    dlg.set_file(fname);

                                    if(dlg.do_modal(this)==IDCANCEL)
                                    {
                                      continue;
                                    }

                                        SetCursor(LoadCursor(NULL,IDC_WAIT));
                                        int id=ikx_t->parse_soundfont((char *)(LPCTSTR)fname,NULL,dlg.bank,dlg.subsynth); // & load (auto)
                                        SetCursor(LoadCursor(NULL,IDC_ARROW));

                                    if(id<=0) 
                                    {
                                     MessageBox((LPCTSTR)mf.get_profile("errors","sfload"),"kX Mixer",MB_OK|MB_ICONEXCLAMATION);
                                    }
                                }
                                delete f_d;
                                manager->save_settings(SETTINGS_AUTO);
                            redraw_window(cur_page);
                        }
                     }
                     return 1;

                case IDg_SFUNLOAD:
                     {
                        int items[64];
                        int total=sf_list.get_multiple_selection(64,items);

                        if(total!=LB_ERR)
                        {
                         for(int i=0;i<total;i++)
                         {
                          int id;
                          kString selection;
                          sf_list.get_text(items[i],selection);
                          debug("kxmixer: unload: %d[%d/%s]\n",i,items[i],(LPCTSTR)selection);
                          sscanf(((LPCTSTR)selection)+1,"%d",&id);
                          ikx_t->unload_soundfont(id);
                         }
             manager->save_settings(SETTINGS_AUTO);
                         sf_info.reset();
                        }
                        redraw_window(cur_page);
                     }
                     return 1;
                case IDg_SFRELOAD:
                     {
                       int sz=ikx_t->enum_soundfonts(NULL,0);
                       if(sz>0)
                       {
                              sfHeader *hdr=(sfHeader *)malloc(sz);
                              if(hdr)
                              {
                                if(ikx_t->enum_soundfonts(hdr,sz)==0) // ok
                                {
                                        int items[64];
                                        int total=sf_list.get_multiple_selection(64,items);

                                        if(total!=LB_ERR)
                                        {
                                         for(int i=0;i<total;i++)
                                         {
                                          int id;
                                          kString selection;
                                          sf_list.get_text(items[i],selection);
                                          sscanf(((LPCTSTR)selection)+1,"%d",&id);
                                          
                                          ikx_t->unload_soundfont(id);

                                          for(dword tt=0;tt<sz/sizeof(sfHeader);tt++)
                                          {
                                           if(hdr[tt].rom_ver.minor==id)
                                           {
                                            ikx_t->parse_soundfont(hdr[tt].sfman_file_name,NULL,hdr[tt].sfman_id,hdr[tt].subsynth);
                                           }
                                          }
                                         }
                             manager->save_settings(SETTINGS_AUTO);
                                         sf_info.reset();
                                        }
                                        redraw_window(cur_page);
                                }
                                free(hdr);
                             }
                        }
                     }
                     return 1;
                case IDg_SFUNLOADALL:
                     {
                       int sz=ikx_t->enum_soundfonts(NULL,0);
                       if(sz>0)
                       {
                              sfHeader *hdr=(sfHeader *)malloc(sz);
                              if(hdr)
                              {
                                if(ikx_t->enum_soundfonts(hdr,sz)==0) // ok
                                {
                                     debug("kxmixer: unload sfs [%d / %d]\n",sz,sz/sizeof(sfHeader));
                                         for(int i=0;i<(int)(sz/sizeof(sfHeader));i++)
                                         {
                                          ikx_t->unload_soundfont(hdr[i].rom_ver.minor);
                                         }
                             manager->save_settings(SETTINGS_AUTO);
                                         sf_info.reset();
                                         redraw_window(cur_page);
                                }
                                free(hdr);
                             }
                        }
                     }
                     return 1;
                case IDg_SFCSAVE:
                     {
                      manager->save_settings(SETTINGS_GUI,NULL,KX_SAVED_SOUNDFONTS|KX_SAVED_NO_VERSION|KX_SAVED_NO_CARDNAME);
                      redraw_window(cur_page);
                     }
                     return 1;
                case IDg_SFCLOAD:
                     {
                      manager->restore_settings(SETTINGS_GUI,NULL,KX_SAVED_SOUNDFONTS|KX_SAVED_NO_VERSION|KX_SAVED_NO_CARDNAME);
                      redraw_window(cur_page);
                     }
                     return 1;

                case IDg_SPECMODE:
                          spectrum.hide();
                          {
                          int view=spectrum.get_view();
                          view++;
                          if(view>=SPECTRUM_VIEW_LAST)
                            view=0;

                          spectrum.set_view(view);
                          }
                          spectrum.show();

                          // save in registry
                          {
                          kSettings cfg;
                          cfg.write_abs("Mixer","SpecMode",(unsigned int)spectrum.get_view());
                          }
                          return 1;
                default:
                    break;
    }

    int i=0;
    // mute
    if(wParam>=IDg_CONTROL_MUTE && wParam <IDg_CONTROL_MUTE+MAX_SLIDERS)
    {
        i=wParam-IDg_CONTROL_MUTE;
        if(sliders[i].mute!=-1)
        {
            sliders[i].mute=1-sliders[i].mute;

                        if(!sliders[i].mute)
                        {
                             sliders[i].mute_b->set_bitmaps(mf.load_image("mixer/buttons/point.bmp"),mf.load_image("mixer/buttons/point_o.bmp"),mf.load_image("mixer/buttons/point_s.bmp"),mf.load_image("mixer/buttons/point_o.bmp"));
                        }
                        else
                        {
                             sliders[i].mute_b->set_bitmaps(mf.load_image("mixer/buttons/point_off.bmp"),mf.load_image("mixer/buttons/point_off_o.bmp"),mf.load_image("mixer/buttons/point_off_s.bmp"),mf.load_image("mixer/buttons/point_o.bmp"));
                        }
                        sliders[i].mute_b->SetWindowText(sliders[i].mute?(LPCTSTR)mf.get_profile("locals","unmute"):(LPCTSTR)mf.get_profile("locals","mute"));
                        add_tool(sliders[i].mute?(char *)(LPCTSTR)mf.get_profile("locals","unmute"):(char *)(LPCTSTR)mf.get_profile("locals","mute"),sliders[i].mute_b,wParam);

                        sliders[i].mute_b->size_to_content();

            update_slider_info(i);
            update_indi_info(i);
        }
        return 1;
    }

    // omni
    if(wParam>=IDg_CONTROL_OMNI && wParam <IDg_CONTROL_OMNI+MAX_SLIDERS)
    {
        i=wParam-IDg_CONTROL_OMNI;
        if(sliders[i].omni!=-1)
        {
            sliders[i].omni=1-sliders[i].omni;
                        if(sliders[i].omni)
                            sliders[i].omni_b->set_bitmaps(mf.load_image("mixer/buttons/omni.bmp"),mf.load_image("mixer/buttons/omni_o.bmp"),mf.load_image("mixer/buttons/omni_s.bmp"),mf.load_image("mixer/buttons/omni_o.bmp"));
                        else
                            sliders[i].omni_b->set_bitmaps(mf.load_image("mixer/buttons/omni_off.bmp"),mf.load_image("mixer/buttons/omni_off_o.bmp"),mf.load_image("mixer/buttons/omni_off_s.bmp"),mf.load_image("mixer/buttons/omni_o.bmp"));

            sliders[i].omni_b->SetWindowText(sliders[i].omni?(char *)(LPCTSTR)mf.get_profile("locals","unlinked"):(char *)(LPCTSTR)mf.get_profile("locals","linked"));
                        add_tool(sliders[i].omni?(char *)(LPCTSTR)mf.get_profile("locals","unlinked"):(char *)(LPCTSTR)mf.get_profile("locals","linked"),sliders[i].omni_b,wParam);

                        sliders[i].omni_b->size_to_content();
                        update_slider_info(i);
                        update_indi_info(i);
        }
        return 1;
    }


    if(wParam>=IDg_CONTROL_CENTER && wParam <IDg_CONTROL_CENTER+MAX_SLIDERS)
    {
        i=wParam-IDg_CONTROL_CENTER;

        sliders[i].vol=255-226;
        sliders[i].pan=255-226;

        sliders[i].omni=1;

        if(sliders[i].vol_s)
        {
         sliders[i].vol_s->set_pos(255-226);
         sliders[i].vol_s->redraw();
        }

        if(sliders[i].pan_s)
        {
         sliders[i].pan_s->set_pos(255-226);
         sliders[i].pan_s->redraw();
        }

        if(sliders[i].omni_b)
        {
          sliders[i].omni_b->set_bitmaps(mf.load_image("mixer/buttons/omni.bmp"),mf.load_image("mixer/buttons/omni_o.bmp"),mf.load_image("mixer/buttons/omni_s.bmp"),mf.load_image("mixer/buttons/omni_o.bmp"));
                  sliders[i].omni_b->SetWindowText(sliders[i].omni?(char *)(LPCTSTR)mf.get_profile("locals","unlinked"):(char *)(LPCTSTR)mf.get_profile("locals","linked"));
                  add_tool(sliders[i].omni?(char *)(LPCTSTR)mf.get_profile("locals","unlinked"):(char *)(LPCTSTR)mf.get_profile("locals","linked"),sliders[i].omni_b,IDg_CONTROL_OMNI+i);
                  sliders[i].omni_b->size_to_content();
                  sliders[i].omni_b->redraw();
                }

        update_slider_info(i);
        update_indi_info(i);
        return 1;
    }

    return CKXDialog::on_command(wParam,lParam);
}

void CMainDialog::OnSelectSoundfont()
{
    int id=-1;
    char selection[128];


    if(sf_list.get_text(sf_list.get_selection(),selection)>=0)
    {
     if(strlen(selection)>2)
      sscanf((selection)+1,"%d",&id);
     else
      return;
    }
    else
     return;

    // SoundFont Select
    int sz=ikx_t->enum_soundfonts(0,0);
    int total_sf2s=0;
    int total_sf_size=0;

    if(sz>0)
    {
      sfHeader *hdr;
      hdr=(sfHeader *)malloc(sz);
      if(hdr)
      {
       memset(hdr,0,sz);
       if(!ikx_t->enum_soundfonts(hdr,sz))
       {
         for(dword i=0;i<sz/sizeof(sfHeader);i++)
         {
          total_sf2s++;
          if(hdr[i].rom_ver.minor==id) // found
          {
           total_sf_size+=hdr[i].sample_len;
           kString output;

               if(::IsWindow(sf_info.m_hWnd)) sf_info.reset();

               output.Format("Name: %s",         
                hdr[i].name[0]?hdr[i].name:"<not specified>");
           if(::IsWindow(sf_info.m_hWnd)) sf_info.add_string(output);

               output.Format("File name: %s",         
                hdr[i].sfman_file_name[0]?hdr[i].sfman_file_name:"<not specified>");
           if(::IsWindow(sf_info.m_hWnd)) sf_info.add_string(output);

               output.Format("Engine: %s  ; Bank: %d",
                hdr[i].engine[0]?hdr[i].engine:"<not specified>",hdr[i].sfman_id);
           if(::IsWindow(sf_info.m_hWnd)) sf_info.add_string(output);

               output.Format("Target Synth: %s",
                hdr[i].subsynth?(hdr[i].subsynth==1?"Synth1":"Synth2"):"Both Synths");
           if(::IsWindow(sf_info.m_hWnd)) sf_info.add_string(output);

           output.Format("Copyright: %s",
             hdr[i].copyright[0]?hdr[i].copyright:"<not specified>");
               if(::IsWindow(sf_info.m_hWnd)) sf_info.add_string(output);

               output.Format("Engineer: %s",
                 hdr[i].engineer[0]?hdr[i].engineer:"<not specified>");
               if(::IsWindow(sf_info.m_hWnd)) sf_info.add_string(output);

               output.Format("Presets: %d Instruments: %d Samples: %d Total Size: %dKb",
                hdr[i].presets,hdr[i].insts,hdr[i].samples,hdr[i].sample_len/1024);
               if(::IsWindow(sf_info.m_hWnd)) sf_info.add_string(output);

               if(::IsWindow(b_sfunload.m_hWnd)) b_sfunload.show();
               if(::IsWindow(b_sfreload.m_hWnd)) b_sfreload.show();
          }
         } // for each soundfont
             if(total_sf2s>0)
             {
              kString s;
              s.Format((LPCTSTR)mf.get_profile("locals","sf"),
                total_sf2s,total_sf2s==1?"":"s",
                total_sf_size/1024);
              sf_info.add_string(s);
              free(hdr);
              return;
             }
       }
       free(hdr);
      }
    } 
        if(::IsWindow(b_sfunload.m_hWnd)) b_sfunload.hide();
        if(::IsWindow(b_sfreload.m_hWnd)) b_sfreload.hide();
}

void CMainDialog::OnChangeAC97Source()
{
    if(ikx_t->ac97_write(AC97_REG_REC_SELECT,(ac97_source.get_selection()&7)+
        ((ac97_source.get_selection()&7)<<8)))
        MessageBox("Error setting AC97 property","kX Mixer");
}

void CMainDialog::OnChangeDSPFreq()
{
    if(ikx_t->set_hw_parameter(KX_HW_SPDIF_FREQ,dsp_freq.get_selection()&3))
        MessageBox("Error setting SPDIF frequency","kX Mixer");
    SendMessage(WM_COMMAND,0xf0f0,cur_page);
}

void CMainDialog::OnChangeAC3Passthru()
{
    if(ikx_t->set_hw_parameter(KX_HW_AC3_PASSTHROUGH,ac3_passthru.get_selection()&3))
        MessageBox("Error setting AC3 PASSTHRU mode","kX Mixer");
    SendMessage(WM_COMMAND,0xf0f0,cur_page);
}


void CMainDialog::OnChangeAC973DSE()
{
    int val3d=ac97_3dse.get_selection();
    int ret=0;

    word val;
    if(val3d==0) // turn off
    {
        if(!ikx_t->ac97_read(AC97_REG_GENERAL,&val))
        {
            ikx_t->ac97_write(AC97_REG_GENERAL,val&0xdfff); // bit 0x2000
            ret=1;
        }
    }
    else
    {
        if(!ikx_t->ac97_read(AC97_REG_GENERAL,&val))
        {
            if(!ikx_t->ac97_write(AC97_REG_GENERAL,val|0x2000)) // bit 0x2000
            {
                ikx_t->ac97_write(AC97_REG_3D_CTRL,(val3d&0x3)+((val3d&0x3)<<2));
                ret=1;
            }
        }
    }
    if(!ret)
        MessageBox("Error setting 3DSE property","kX Mixer");
}

void CMainDialog::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    kString name;

    CWnd *w=(CWnd *)pScrollBar;
    w->GetWindowText(name);

    int i;
    for(i=0;i<MAX_SLIDERS;i++)
    {
        if(sliders[i].name)
            if(strcmp(sliders[i].name,
                        (LPCSTR)name)==NULL)
            {
                if(sliders[i].omni==0)
                {
                    if(sliders[i].pan_s)
                        sliders[i].pan=sliders[i].pan_s->get_pos();
                    if(sliders[i].vol_s)
                        sliders[i].vol=sliders[i].vol_s->get_pos();
                }
                else
                {
                    int pos=((kSlider *)w)->get_pos();
                    sliders[i].vol=sliders[i].pan=pos;
                        
                    if(sliders[i].pan_s)
                        sliders[i].pan_s->set_pos(pos);
                    if(sliders[i].vol_s)
                        sliders[i].vol_s->set_pos(pos);
                }
                if(sliders[i].pan_s)
                 sliders[i].pan_s->redraw();
                if(sliders[i].vol_s)
                 sliders[i].vol_s->redraw();

                update_slider_info(i);
                update_indi_info(i);
            }
    }
    kDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CMainDialog::update_slider_info(int i)
{
    int ret=0;
    int ret_b=0;
    int vol;
    int pan;

    if(sliders[i].type&TYPE_DSP)
    {
     set_volume(sliders[i].gpr1,sliders[i].type&TYPE_STEREO?sliders[i].gpr2:0,i,sliders[i].id);
    }
    if(sliders[i].type&TYPE_MIXER)
    {
      if(mixer_notify_lock==0)
      {
        mixer_notify_lock++;

            switch(sliders[i].id)
            {
                    case 0: // master
                        vol=(255-sliders[i].vol)*256;
                        pan=(255-sliders[i].pan)*256;

                        if(vol==0xff00) vol=0xffff;
                        if(pan==0xff00) pan=0xffff;
                        
                        ikx_t->mixer(KX_PROP_SET,
                            KX_MIXER_MASTER,0,&vol);
                        ikx_t->mixer(KX_PROP_SET,
                            KX_MIXER_MASTER,1,&pan);
                        ikx_t->mixer(KX_PROP_SET,
                            KX_MIXER_MASTER_MUTE,0,&sliders[i].mute);
                        break;
                    case 1: // wave
                        vol=(255-sliders[i].vol)*256;
                        pan=(255-sliders[i].pan)*256;

                        if(vol==0xff00) vol=0xffff;
                        if(pan==0xff00) pan=0xffff;

                        ikx_t->mixer(KX_PROP_SET,
                            KX_MIXER_WAVE,0,&vol);
                        ikx_t->mixer(KX_PROP_SET,
                            KX_MIXER_WAVE,1,&pan);
                        ikx_t->mixer(KX_PROP_SET,
                            KX_MIXER_WAVE_MUTE,0,&sliders[i].mute);
                        break;
                    case 2: // synth
                        vol=(255-sliders[i].vol)*256;
                        pan=(255-sliders[i].pan)*256;

                        if(vol==0xff00) vol=0xffff;
                        if(pan==0xff00) pan=0xffff;

                        ikx_t->mixer(KX_PROP_SET,
                            KX_MIXER_SYNTH,0,&vol);
                        ikx_t->mixer(KX_PROP_SET,
                            KX_MIXER_SYNTH,1,&pan);
                        ikx_t->mixer(KX_PROP_SET,
                            KX_MIXER_SYNTH_MUTE,0,&sliders[i].mute);
                        break;
                    case 3: // rec
                            vol=(255-sliders[i].vol)*256;
                            pan=(255-sliders[i].pan)*256;

                        if(vol==0xff00) vol=0xffff;
                        if(pan==0xff00) pan=0xffff;

                            ikx_t->mixer(KX_PROP_SET,
                                KX_MIXER_REC,0,&vol);
                            ikx_t->mixer(KX_PROP_SET,
                                KX_MIXER_REC,1,&pan);
                            ikx_t->mixer(KX_PROP_SET,
                                KX_MIXER_REC_MUTE,0,&sliders[i].mute);
                            break;
                    case 4: // wave23
                        vol=(255-sliders[i].vol)*256;
                        pan=(255-sliders[i].pan)*256;

                        if(vol==0xff00) vol=0xffff;
                        if(pan==0xff00) pan=0xffff;

                        ikx_t->mixer(KX_PROP_SET,
                            KX_MIXER_WAVE23,0,&vol);
                        ikx_t->mixer(KX_PROP_SET,
                            KX_MIXER_WAVE23,1,&pan);
                        ikx_t->mixer(KX_PROP_SET,
                            KX_MIXER_WAVE23_MUTE,0,&sliders[i].mute);
                        break;
                    case 5: // wave45
                        vol=(255-sliders[i].vol)*256;
                        pan=(255-sliders[i].pan)*256;

                        if(vol==0xff00) vol=0xffff;
                        if(pan==0xff00) pan=0xffff;

                        ikx_t->mixer(KX_PROP_SET,
                            KX_MIXER_WAVE45,0,&vol);
                        ikx_t->mixer(KX_PROP_SET,
                            KX_MIXER_WAVE45,1,&pan);
                        ikx_t->mixer(KX_PROP_SET,
                            KX_MIXER_WAVE45_MUTE,0,&sliders[i].mute);
                        break;
                    case 6: // wave67
                        vol=(255-sliders[i].vol)*256;
                        pan=(255-sliders[i].pan)*256;

                        if(vol==0xff00) vol=0xffff;
                        if(pan==0xff00) pan=0xffff;

                        ikx_t->mixer(KX_PROP_SET,
                            KX_MIXER_WAVE67,0,&vol);
                        ikx_t->mixer(KX_PROP_SET,
                            KX_MIXER_WAVE67,1,&pan);
                        ikx_t->mixer(KX_PROP_SET,
                            KX_MIXER_WAVE67_MUTE,0,&sliders[i].mute);
                        break;
                    case 7: // linein
                        vol=(255-sliders[i].vol)*256;
                        pan=(255-sliders[i].pan)*256;

                        if(vol==0xff00) vol=0xffff;
                        if(pan==0xff00) pan=0xffff;

                        ikx_t->mixer(KX_PROP_SET,
                            KX_MIXER_LINEIN,0,&vol);
                        ikx_t->mixer(KX_PROP_SET,
                            KX_MIXER_LINEIN,1,&pan);
                        ikx_t->mixer(KX_PROP_SET,
                            KX_MIXER_LINEIN_MUTE,0,&sliders[i].mute);
                        break;
                    case 8: // micin
                        vol=(255-sliders[i].vol)*256;
                        pan=(255-sliders[i].pan)*256;

                        if(vol==0xff00) vol=0xffff;
                        if(pan==0xff00) pan=0xffff;

                        ikx_t->mixer(KX_PROP_SET,
                            KX_MIXER_MICIN,0,&vol);
                        ikx_t->mixer(KX_PROP_SET,
                            KX_MIXER_MICIN,1,&pan);
                        ikx_t->mixer(KX_PROP_SET,
                            KX_MIXER_MICIN_MUTE,0,&sliders[i].mute);
                        break;
                    case 9: // waveHQ
                        vol=(255-sliders[i].vol)*256;
                        pan=(255-sliders[i].pan)*256;

                        if(vol==0xff00) vol=0xffff;
                        if(pan==0xff00) pan=0xffff;

                        ikx_t->mixer(KX_PROP_SET,
                            KX_MIXER_WAVEHQ,0,&vol);
                        ikx_t->mixer(KX_PROP_SET,
                            KX_MIXER_WAVEHQ,1,&pan);
                        ikx_t->mixer(KX_PROP_SET,
                            KX_MIXER_WAVEHQ_MUTE,0,&sliders[i].mute);
                        break;
                }
                mixer_notify_lock--;
           }
        } // type_mixer
        if(sliders[i].type&TYPE_AC97)
        {
           dword val=0;
           byte bits=(1<<sliders[i].bits);

           if(sliders[i].vol>(bits-1)*(256/bits))
             sliders[i].vol=(bits-1)*(256/bits);
           if(sliders[i].pan>(bits-1)*(256/bits))
             sliders[i].pan=(bits-1)*(256/bits);

           if(sliders[i].type&TYPE_STEREO)
           {
            if(sliders[i].id!=AC97_REG_REC_GAIN)
            {
             val=(sliders[i].vol/(256/bits))<<8;
             val|=(sliders[i].pan/(256/bits));
            }
    // 3537d: back
            else
            {
             val=((255-sliders[i].vol)/(256/bits))<<8;
             val|=((255-sliders[i].pan)/(256/bits));
            }
           }
           else
           {
            val=(sliders[i].vol/(256/bits));
           }

           if(sliders[i].mute)
            val|=0x8000;

           // special cases
           if(sliders[i].id==AC97_REG_MIC_VOL)
           {
            word tmp;
            ikx_t->ac97_read((byte)AC97_REG_MIC_VOL,&tmp);
            tmp&=0x40;
            val|=tmp;
           }

           if(sliders[i].id==AC97_REG_PC_BEEP_VOL)
            val&=0xfffe;

       ikx_t->ac97_write((byte)sliders[i].id,(word)val);
        }

    if(sliders[i].type&TYPE_AMOUNT)
    {
        ikx_t->set_send_amount(sliders[i].id,(byte)(0xff-sliders[i].vol));
    }

    if(sliders[i].type&TYPE_AGC)
    {
        dword val=
         (0xff-(sliders[i].vol&0xff))+
         ((0xff-(sliders[i].pan&0xff))<<8);
        ikx_t->set_hw_parameter(KX_HW_ECARD_ADC_GAIN,val);
    }
}


int CMainDialog::mixer_notify()
{
 if(mixer_notify_lock==0)
 {
 mixer_notify_lock++;

 for(int i=0;i<MAX_SLIDERS;i++)
 {
  if(sliders[i].type&TYPE_MIXER)
  {
    int mx_id=-1;
        switch(sliders[i].id)
        {
         case 0: mx_id=KX_MIXER_MASTER; break;
         case 1: mx_id=KX_MIXER_WAVE; break;
         case 2: mx_id=KX_MIXER_SYNTH; break;
         case 3: mx_id=KX_MIXER_REC; break;
         case 4: mx_id=KX_MIXER_WAVE23; break;
         case 5: mx_id=KX_MIXER_WAVE45; break;
         case 6: mx_id=KX_MIXER_WAVE67; break;
         case 7: mx_id=KX_MIXER_LINEIN; break;
         case 8: mx_id=KX_MIXER_MICIN; break;
         case 9: mx_id=KX_MIXER_WAVEHQ; break;
         default:
          break;
        }
        if(mx_id==-1)
         continue;

        int new_pan=-1,new_vol=-1,new_mute=-1,new_omni=-1;

        ikx_t->mixer(KX_PROP_GET,mx_id,0,&new_vol);
        ikx_t->mixer(KX_PROP_GET,mx_id,1,&new_pan);
        ikx_t->mixer(KX_PROP_GET,mx_id+1,0,&new_mute);
        if(new_mute)
          new_mute=1;

        if(new_vol!=sliders[i].vol && new_vol!=-1)
        {
         sliders[i].vol=new_vol;

         sliders[i].vol>>=8;
         sliders[i].vol=255-sliders[i].vol;

         if(sliders[i].vol_s)
         {
          sliders[i].vol_s->set_pos(sliders[i].vol);
          sliders[i].vol_s->redraw();
         }
        }

        if(new_pan!=sliders[i].pan && new_pan!=-1)
        {
         sliders[i].pan=new_pan;

         sliders[i].pan>>=8;
         sliders[i].pan=255-sliders[i].pan;

         if(sliders[i].pan_s)
         {
          sliders[i].pan_s->set_pos(sliders[i].pan);
          sliders[i].pan_s->redraw();
         }
        }

        if(sliders[i].type&TYPE_STEREO)
        {
               if(sliders[i].vol==sliders[i].pan)
                new_omni=1;
               else
                new_omni=0;
        }

        if(sliders[i].mute_b && sliders[i].mute!=new_mute && new_mute!=-1)
        {
           sliders[i].mute=new_mute;

           if(!sliders[i].mute)
                sliders[i].mute_b->set_bitmaps(mf.load_image("mixer/buttons/point.bmp"),mf.load_image("mixer/buttons/point_o.bmp"),mf.load_image("mixer/buttons/point_s.bmp"),mf.load_image("mixer/buttons/point_o.bmp"));
           else
                sliders[i].mute_b->set_bitmaps(mf.load_image("mixer/buttons/point_off.bmp"),mf.load_image("mixer/buttons/point_off_o.bmp"),mf.load_image("mixer/buttons/point_off_s.bmp"),mf.load_image("mixer/buttons/point_o.bmp"));

           sliders[i].mute_b->SetWindowText(sliders[i].mute?(LPCTSTR)mf.get_profile("locals","unmute"):(LPCTSTR)mf.get_profile("locals","mute"));
           add_tool(sliders[i].mute?(char *)(LPCTSTR)mf.get_profile("locals","unmute"):(char *)(LPCTSTR)mf.get_profile("locals","mute"),sliders[i].mute_b,IDg_CONTROL_MUTE+i);

           sliders[i].mute_b->size_to_content();

           sliders[i].mute_b->redraw();
        }

        if(sliders[i].omni_b && sliders[i].omni!=new_omni && new_omni!=-1)
        {
           sliders[i].omni=new_omni;

           if(sliders[i].omni)
            sliders[i].omni_b->set_bitmaps(mf.load_image("mixer/buttons/omni.bmp"),mf.load_image("mixer/buttons/omni_o.bmp"),mf.load_image("mixer/buttons/omni_s.bmp"),mf.load_image("mixer/buttons/omni_o.bmp"));
           else
            sliders[i].omni_b->set_bitmaps(mf.load_image("mixer/buttons/omni_off.bmp"),mf.load_image("mixer/buttons/omni_off_o.bmp"),mf.load_image("mixer/buttons/omni_off_s.bmp"),mf.load_image("mixer/buttons/omni_o.bmp"));

           sliders[i].omni_b->SetWindowText(sliders[i].omni?(char *)(LPCTSTR)mf.get_profile("locals","unlinked"):(char *)(LPCTSTR)mf.get_profile("locals","linked"));
           add_tool(sliders[i].omni?(char *)(LPCTSTR)mf.get_profile("locals","unlinked"):(char *)(LPCTSTR)mf.get_profile("locals","linked"),sliders[i].omni_b,IDg_CONTROL_OMNI+i);

           sliders[i].omni_b->size_to_content();

           sliders[i].omni_b->redraw();
        }

        update_indi_info(i);
   }
  } // for

  mixer_notify_lock--;
 }
 return 0;
}

void CMainDialog::OnChangeECardSpdif()
{
 dword val=0;
 switch(ecard_spdif0.get_selection())
 {
  case 0: break;
  case 1: val|=1; break;
  case 2: val|=4; break;
  default:
   return;
 }
 switch(ecard_spdif1.get_selection())
 {
  case 0: break;
  case 1: val|=0x10000; break;
  default:
   return;
 }
 ikx_t->set_hw_parameter(KX_HW_ECARD_ROUTING,val);
}

void CMainDialog::OnChangeSpdifRecording()
{
 dword val=spdif_recording.get_selection()&0x7;
 ikx_t->set_hw_parameter(KX_HW_SPDIF_RECORDING,val);
}

void CMainDialog::OnChangeZsnbSource()
{
 dword val=zsnb_source.get_selection()&0x3;
 ikx_t->set_hw_parameter(KX_HW_A2ZSNB_SOURCE,val);
}

void CMainDialog::OnChangeP16vRecording()
{
 dword val=p16v_recording.get_selection();
 switch(val)
 {
  case 0:ikx_t->set_hw_parameter(KX_HW_P16V_REC_ROUTING,0x00); break;
  case 1:ikx_t->set_hw_parameter(KX_HW_P16V_REC_ROUTING,0x10); break;
  case 2:ikx_t->set_hw_parameter(KX_HW_P16V_REC_ROUTING,0x20); break;
  case 3:ikx_t->set_hw_parameter(KX_HW_P16V_REC_ROUTING,0x01); break;
  case 4:ikx_t->set_hw_parameter(KX_HW_P16V_REC_ROUTING,0x11); break;
  case 5:ikx_t->set_hw_parameter(KX_HW_P16V_REC_ROUTING,0x21); break;
  case 6:ikx_t->set_hw_parameter(KX_HW_P16V_REC_ROUTING,0x02); break;
  case 7:ikx_t->set_hw_parameter(KX_HW_P16V_REC_ROUTING,0x12); break;
  case 8:ikx_t->set_hw_parameter(KX_HW_P16V_REC_ROUTING,0x22); break;
  default:
   break;
 }
}
