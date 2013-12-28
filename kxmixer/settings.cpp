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

extern HWND systray;

#include "notify.h"
#include "addonmgr.h"

// inlined
#include "settings2_dlg.cpp"

static int init_quality(LPDIRECTSOUND8 ds)
{
        LPDIRECTSOUNDBUFFER prim_buffer = NULL;
        LPDIRECTSOUNDBUFFER buffer = NULL;
    IKsPropertySet *property=NULL;
    int err=0;
        HRESULT hr;

        // Get the primary buffer 
        DSBUFFERDESC dsbd;
        ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
        dsbd.dwSize        = sizeof(DSBUFFERDESC);
        dsbd.dwFlags       = DSBCAPS_PRIMARYBUFFER;
        dsbd.dwBufferBytes = 0;
        dsbd.lpwfxFormat   = NULL;

    if( FAILED( hr = ds->CreateSoundBuffer( &dsbd, &prim_buffer, NULL ) ) )
    {
            MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","dx1"),"kX Mixer",MB_OK|MB_ICONEXCLAMATION);
            goto FAILED_ERROR;
        }

        WAVEFORMATEX wfx;
        ZeroMemory( &wfx, sizeof(WAVEFORMATEX) ); 
        wfx.wFormatTag      = WAVE_FORMAT_PCM; 
        wfx.nChannels       = (WORD) 2;
        wfx.nSamplesPerSec  = 48000; 
        wfx.wBitsPerSample  = (WORD) 16; 
        wfx.nBlockAlign     = wfx.wBitsPerSample / 8 * wfx.nChannels;
        wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

        if( FAILED( hr = prim_buffer->SetFormat(&wfx) ) )
        {
            MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","dx2"),"kX Mixer",MB_OK|MB_ICONEXCLAMATION);
            goto FAILED_ERROR;
        }

        ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
        dsbd.dwSize        = sizeof(DSBUFFERDESC);
        dsbd.dwFlags = DSBCAPS_LOCHARDWARE | DSBCAPS_GLOBALFOCUS;
        dsbd.dwBufferBytes = 4096;

    WAVEFORMATEX format;
    format.cbSize=sizeof(format);
    format.nChannels=1;
    format.nSamplesPerSec=32000;
    format.wBitsPerSample=16;
    format.nBlockAlign=format.nChannels*format.wBitsPerSample/8;
    format.nAvgBytesPerSec=format.nSamplesPerSec*format.wBitsPerSample/8;
    format.wFormatTag=1; // PCM
        dsbd.lpwfxFormat     = &format;

    if( FAILED( hr = ds->CreateSoundBuffer( &dsbd, &buffer, NULL ) ) )
    {
            // MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","dx3"),"kX Mixer",MB_OK|MB_ICONEXCLAMATION);
            debug("!! kxmixer: error initializing primary sound buffer [%x, probably, HQ]\n",hr);
            goto FAILED_ERROR;
        }

        buffer->QueryInterface(IID_IKsPropertySet,(void **)&property);
    if(!property)
    {
           MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","dx4"),"kX Mixer",MB_OK|MB_ICONEXCLAMATION);
           goto FAILED_ERROR;
    }
    else
    {
               ULONG inst=-1;
               ULONG prop=KSAUDIO_QUALITY_ADVANCED;
               hr=property->Set(
                   KSPROPSETID_Audio,
                   KSPROPERTY_AUDIO_QUALITY,
                   NULL,0,//&inst,sizeof(inst),
                   &prop,sizeof(prop));
               if(FAILED(hr))
               {
                err=GetLastError();
                kString s;
                s.Format((LPCTSTR)mf.get_profile("errors","cp"),err,hr);
                MessageBox(NULL,(LPCTSTR)s,
                    "kX Mixer",MB_OK|MB_ICONEXCLAMATION);
                goto FAILED_ERROR;
               }

               property->Release();
               property=0;
    }
    if(buffer)
    {
     buffer->Release();
     buffer=0;
    }
    if(prim_buffer)
    {
     prim_buffer->Release();
     prim_buffer=0;
    }
    
    return 0;

FAILED_ERROR:
    if(property)
    {
        property->Release();
        property=0;
    }
    if(buffer)
    {
        buffer->Release();
        buffer=0;
    }
    if(prim_buffer)
    {
        prim_buffer->Release();
        prim_buffer=0;
    }
    SetLastError(err);
    return -1;
}



void iKXManager::reset_settings(void)
{
   dword has_ac97=0;
   dword is_a2=0;

   iKX *ikx=get_ikx();
   if(!ikx)
   {
    debug("kxmixer: internal error: reset settings: ikx=NULL\n");
    return;
   }

   if(ikx->get_dword(KX_DWORD_AC97_PRESENT,(dword *)&has_ac97))
   {
    has_ac97=0; // error
   }

   if(ikx->get_dword(KX_DWORD_IS_A2,(dword *)&is_a2))
   {
    is_a2=0; // error
   }

//   #define DB(A) MessageBox(NULL,A,"kX Mixer",MB_OK)
 #define DB(A)

//   DB("about to mute...\n");
   ikx->mute();

   DB("closing plugins...\n");
   get_pm()->close_plugins(); // this will unload parser data, too
   
   DB("ikx->reset...\n");
   ikx->reset_settings();

   DB("ac97 reset...\n");
   if(has_ac97)
    ikx->ac97_reset();

   DB("microcode reset...\n");
   ikx->reset_microcode();

   DB("Mixer stuff...\n");
  // unmute & init mixer defaults
   int val=0xffff;
   ikx->mixer(KX_PROP_SET,KX_MIXER_MASTER,0,&val);
   ikx->mixer(KX_PROP_SET,KX_MIXER_MASTER,1,&val);
   ikx->mixer(KX_PROP_SET,KX_MIXER_WAVE,0,&val);
   ikx->mixer(KX_PROP_SET,KX_MIXER_WAVE,1,&val);
   ikx->mixer(KX_PROP_SET,KX_MIXER_WAVE23,0,&val);
   ikx->mixer(KX_PROP_SET,KX_MIXER_WAVE23,1,&val);
   ikx->mixer(KX_PROP_SET,KX_MIXER_WAVE45,0,&val);
   ikx->mixer(KX_PROP_SET,KX_MIXER_WAVE45,1,&val);
   ikx->mixer(KX_PROP_SET,KX_MIXER_WAVE67,0,&val);
   ikx->mixer(KX_PROP_SET,KX_MIXER_WAVE67,1,&val);
   if(is_a2)
   {
    ikx->mixer(KX_PROP_SET,KX_MIXER_WAVEHQ,0,&val);
    ikx->mixer(KX_PROP_SET,KX_MIXER_WAVEHQ,1,&val);
   }
   ikx->mixer(KX_PROP_SET,KX_MIXER_SYNTH,0,&val);
   ikx->mixer(KX_PROP_SET,KX_MIXER_SYNTH,1,&val);
   ikx->mixer(KX_PROP_SET,KX_MIXER_REC,0,&val);
   ikx->mixer(KX_PROP_SET,KX_MIXER_REC,1,&val);

   val=0;

   ikx->mixer(KX_PROP_SET,KX_MIXER_LINEIN,0,&val);
   ikx->mixer(KX_PROP_SET,KX_MIXER_LINEIN,1,&val);
   ikx->mixer(KX_PROP_SET,KX_MIXER_MICIN,0,&val);
   ikx->mixer(KX_PROP_SET,KX_MIXER_MICIN,1,&val);

   val=0;
   ikx->mixer(KX_PROP_SET,KX_MIXER_SYNTH_MUTE,0,&val);
   ikx->mixer(KX_PROP_SET,KX_MIXER_REC_MUTE,0,&val);
   ikx->mixer(KX_PROP_SET,KX_MIXER_WAVE_MUTE,0,&val);
   ikx->mixer(KX_PROP_SET,KX_MIXER_WAVE23_MUTE,0,&val);
   ikx->mixer(KX_PROP_SET,KX_MIXER_WAVE45_MUTE,0,&val);
   ikx->mixer(KX_PROP_SET,KX_MIXER_WAVE67_MUTE,0,&val);
   if(is_a2)
   {
    ikx->mixer(KX_PROP_SET,KX_MIXER_WAVEHQ_MUTE,0,&val);
   }
   ikx->mixer(KX_PROP_SET,KX_MIXER_MASTER_MUTE,0,&val);

   val=1;
   ikx->mixer(KX_PROP_SET,KX_MIXER_LINEIN_MUTE,0,&val);
   ikx->mixer(KX_PROP_SET,KX_MIXER_MICIN_MUTE,0,&val);

//   DB("UnMute...\n");
   ikx->unmute();

   int dev_list[KX_MAX_WAVE_DEVICES]=
   {
    KX_WINMM_WAVE,
    KX_WINMM_WAVE23,
    KX_WINMM_WAVE45,
    KX_WINMM_WAVE67,
    KX_WINMM_WAVEHQ
   };

   DB("DirectSound8 enumerate...\n");
   for(int ii=0;ii<KX_MAX_WAVE_DEVICES;ii++)
   {
        LPGUID dx8=ikx->get_dsound_guid(dev_list[ii]);
        LPDIRECTSOUND8 ds;
        if(DirectSoundCreate8(dx8,&ds,NULL)!=DS_OK)
        {
            MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","dx0"),"kX Mixer",MB_OK|MB_ICONEXCLAMATION);
        }
        else
        {
            // DB("set cooperative level ...\n");
            if(ds->SetCooperativeLevel(GetTopWindow(NULL),DSSCL_PRIORITY)==DS_OK)
            {
                // DB("set spk config ...\n");
                if(ds->SetSpeakerConfig(is_a2?DSSPEAKER_7POINT1:DSSPEAKER_5POINT1)==DS_OK)
                {
                    // more init...
                    if(!init_quality(ds))
                    {
                        // nothing more...
                        debug("kxmixer: quality for '%s' set\n",ikx->get_winmm_name(dev_list[ii]));
                    }
                }
                else MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","dx5"),"kX Mixer",MB_OK|MB_ICONEXCLAMATION);
            }
            else MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","dx6"),"kX Mixer",MB_OK|MB_ICONEXCLAMATION);
            ds->Release();
            ds=NULL;
        }
  }

  DB("init plugins ...\n");
  // reset plugin settings here
  // reset_microcode() was already called
  get_pm()->init_plugins();

  DB("reset plg settings...\n");
  get_pm()->reset_all_plugin_settings();

  kSettings cfg(get_ikx()->get_device_name());

  char tmp_asio[32];
  // reset ASIO settings
  sprintf(tmp_asio,"AsioMethod_%d",ikx->get_device_num());
  cfg.write_abs(_T("ASIO"),tmp_asio,_T("131073")); // KXASIO_METHOD_DEFAULT=(KXASIO_METHOD_SLEEP|KXASIO_METHOD_NEW)

  sprintf(tmp_asio,"BPS_%d",ikx->get_device_num());
  cfg.write_abs(_T("ASIO"),tmp_asio,_T("16"));

  sprintf(tmp_asio,"Freq_%d",ikx->get_device_num());
  cfg.write_abs(_T("ASIO"),tmp_asio,_T("48000"));

  sprintf(tmp_asio,"Pb_%d",ikx->get_device_num());
  cfg.write_abs(_T("ASIO"),tmp_asio,_T("16"));

  sprintf(tmp_asio,"Rec_%d",ikx->get_device_num());
  cfg.write_abs(_T("ASIO"),tmp_asio,_T("16"));

  sprintf(tmp_asio,"Latency_%d",ikx->get_device_num());
  cfg.write_abs(_T("ASIO"),tmp_asio,_T("384")); //  8ms


  cfg.write("hw_params","Headphones",0,0);

  DB("kX Control reset...\n");
  if(get_parser()!=NULL)
     get_parser()->reset();
}

// --------------

int iKXManager::save_settings(int flag,kString *fname_,dword kx_saved_flag)
{
 iKX *ikx=get_ikx();
 if(!ikx)
 {
  debug("kxmixer: internal error: save settings: ikx=NULL\n");
  return -1;
 }

 kString fname;

 int i;

 dword has_ac97=0;

 if(ikx->get_dword(KX_DWORD_AC97_PRESENT,(dword *)&has_ac97))
 {
    has_ac97=0; // error
 }

 dword is_a2=0;

 if(ikx->get_dword(KX_DWORD_IS_A2,(dword *)&is_a2))
 {
    is_a2=0; // error
 }

 if(flag&SETTINGS_GUI)
 {
    restore_cwd("kx");
        CFileDialog *f_d = new CFileDialog(FALSE,NULL,"kxdefault.kx",OFN_HIDEREADONLY|OFN_EXPLORER|OFN_OVERWRITEPROMPT,
          "kX Settings (*.kx)|*.kx||",CWnd::FromHandle(systray));
        if(f_d)
        {
                char tmp_cwd[MAX_PATH];
                GetCurrentDirectory(sizeof(tmp_cwd),tmp_cwd);
                f_d->m_ofn.lpstrInitialDir=tmp_cwd;

            if(f_d->DoModal()==IDCANCEL)
            {
             delete f_d;
             return -1;
            }
            fname=f_d->GetPathName();
            delete f_d;

            save_cwd("kx");

            if(fname.GetLength()==0) // cancel or empty
            {
             return -2;
            }
            if(fname.Find(".kx")==-1)
             fname+=".kx";
            _unlink((LPCTSTR)fname);
        } else return -3;

        CSaveRestoreDlg dlg(kx_saved_flag);
        if(dlg.do_modal(NULL)!=IDOK)
         return 0;
        kx_saved_flag=dlg.flag;

        save_lru((char *)(LPCTSTR)fname);
 }
  else
 if(flag&SETTINGS_AUTO)
 {
  ; // nop
 }
  else
  if(fname_)
   fname=*fname_;
  else
  {
   debug("kxmixer: save_settings Internal error 1\n");
   return -4;
  }

 char *f;
 if(flag&SETTINGS_AUTO)
  f=NULL;
 else
  f=(char *)(LPCTSTR)fname;

 kSettings cfg(ikx->get_device_name(),f,kx_saved_flag);

 kString tmp2;
 tmp2.Format("device_%d",ikx->get_device_num());
 cfg.write_abs("General",(LPCTSTR)tmp2,ikx->get_device_name());

 cfg.write("General","descr","kX Saved Settings");
 cfg.write("General","version",KX_DRIVER_VERSION_STR);
 cfg.write("General","flag",kx_saved_flag);

 if(kx_saved_flag&KX_SAVED_ROUTING)
 {
   for(i=0;i<=ROUTING_LAST;i++)
   {
    dword r,xr;
    ikx->get_routing(i,&r,&xr);
    cfg.write("routing","routing",r,i);
    cfg.write("routing","xrouting",xr,i);
   }
 }

 if(kx_saved_flag&KX_SAVED_AMOUNT)
 {
   for(i=0;i<=MAX_DEF_AMOUNT;i++)
   {
    byte a;
    ikx->get_send_amount(i,&a);
    cfg.write("amount","amount",a,i);
   }

   if(is_a2)
   {
    for(i=0;i<8;i++)
    {
     dword v=0;
     if(ikx->get_p16v_volume(i,&v)==0)
      cfg.write("amount","p16v_amount",v,i);
    }
   }
  }

 int v;

 if(kx_saved_flag&KX_SAVED_BUFFERS)
 {
 if(ikx->get_buffers(KX_TANKMEM_BUFFER,&v)==0)
  cfg.write("buffers","tankmem",v);
 if(ikx->get_buffers(KX_PLAYBACK_BUFFER,&v)==0)
  cfg.write("buffers","playback",v);
 if(ikx->get_buffers(KX_GSIF_BUFFER,&v)==0)
  cfg.write("buffers","gsif",v);
 if(ikx->get_buffers(KX_RECORD_BUFFER,&v)==0)
  cfg.write("buffers","record",v);
 if(ikx->get_buffers(KX_AC3_BUFFERS,&v)==0)
  cfg.write("buffers","ac3",v);
 }

 if(kx_saved_flag&KX_SAVED_HWPARAMS)
 {
  // hw parameters
  dword d;
  if(ikx->get_hw_parameter(KX_HW_DOO,&d)==0)
   cfg.write("hw_params","doo",d);
  if(ikx->get_hw_parameter(KX_HW_ECARD_ROUTING,&d)==0)
   cfg.write("hw_params","ecard_routing",d);
  if(ikx->get_hw_parameter(KX_HW_ECARD_ADC_GAIN,&d)==0)
   cfg.write("hw_params","ecard_adc_gain",d);
  if(ikx->get_hw_parameter(KX_HW_SPDIF_FREQ,&d)==0)
   cfg.write("hw_params","spdif_freq",d);
  if(ikx->get_hw_parameter(KX_HW_SPDIF_BYPASS,&d)==0)
   cfg.write("hw_params","spdif_bypass",d);
  if(ikx->get_hw_parameter(KX_HW_SWAP_FRONT_REAR,&d)==0)
   cfg.write("hw_params","swap_f_r",d);
  if(ikx->get_hw_parameter(KX_HW_ROUTE_PH_TO_CSW,&d)==0)
   cfg.write("hw_params","route_ph2csw",d);
  if(ikx->get_hw_parameter(KX_HW_SPDIF_DECODE,&d)==0)
   cfg.write("hw_params","spdif_decode",d);
  if(ikx->get_hw_parameter(KX_HW_SPDIF_RECORDING,&d)==0)
   cfg.write("hw_params","spdif_recording",d);
  if(ikx->get_hw_parameter(KX_HW_COMPAT,&d)==0)
   cfg.write("hw_params","compat",d);
   //  NOT USED: cfg.write("hw_params","wave_mix",d);
  if(ikx->get_hw_parameter(KX_HW_P16V_PB_ROUTING,&d)==0)
   cfg.write("hw_params","p16v_pb_routing",d);
  if(ikx->get_hw_parameter(KX_HW_P16V_REC_ROUTING,&d)==0)
   cfg.write("hw_params","p16v_rec_routing",d);
  if(ikx->get_hw_parameter(KX_HW_AC3_PASSTHROUGH,&d)==0)
   cfg.write("hw_params","ac3_passthru",d);
  if(ikx->get_hw_parameter(KX_HW_SYNTH_COMPATIBILITY,&d)==0)
   cfg.write("hw_params","synth_compat",d);

  if(ikx->get_hw_parameter(KX_HW_K2_AC97,&d)==0)
   cfg.write("hw_params","ac97_line2",d);

  if(ikx->get_hw_parameter(KX_HW_A2ZSNB_SOURCE,&d)==0)
   cfg.write("hw_params","a2zsnb_src",d);

  if(ikx->get_hw_parameter(KX_HW_KX3D,&d)==0)
   cfg.write("hw_params","kx3d",d);
  if(ikx->get_hw_parameter(KX_HW_8PS,&d)==0)
   cfg.write("hw_params","sp8ps",d);

  if(ikx->get_hw_parameter(KX_HW_DRUM_CHANNEL,&d)==0)
   cfg.write("hw_params","drum_channel",d);

  for(int i=0;i<MAX_MIXER_CONTROLS;i++)
  {
     kx_assignment_info ai;
     ai.level=i;
     if(ikx->get_dsp_assignments(&ai)==0)
     {
      TCHAR name[KX_MAX_STRING];
      sprintf(name,"slider_pgm%d_name",i);
      cfg.write("hw_params",name,ai.pgm);

      sprintf(name,"slider_pgm%d_left",i);
      cfg.write("hw_params",name,ai.reg_left);

      sprintf(name,"slider_pgm%d_right",i);
      cfg.write("hw_params",name,ai.reg_right);

      sprintf(name,"slider_pgm%d_max",i);
      cfg.write("hw_params",name,ai.max_vol);
      }
  }
 }

 if(kx_saved_flag&KX_SAVED_MIXER)
 {
  int v=0;

  // mixer
  ikx->mixer(KX_PROP_GET,KX_MIXER_MASTER,0,&v);
  cfg.write("mixer","master_l",v);
  ikx->mixer(KX_PROP_GET,KX_MIXER_MASTER,1,&v);
  cfg.write("mixer","master_r",v);
  ikx->mixer(KX_PROP_GET,KX_MIXER_WAVE,0,&v);
  cfg.write("mixer","wave_l",v);
  ikx->mixer(KX_PROP_GET,KX_MIXER_WAVE,1,&v);
  cfg.write("mixer","wave_r",v);
  ikx->mixer(KX_PROP_GET,KX_MIXER_WAVE23,0,&v);
  cfg.write("mixer","wave23_l",v);
  ikx->mixer(KX_PROP_GET,KX_MIXER_WAVE23,1,&v);
  cfg.write("mixer","wave23_r",v);
  ikx->mixer(KX_PROP_GET,KX_MIXER_WAVE45,0,&v);
  cfg.write("mixer","wave45_l",v);
  ikx->mixer(KX_PROP_GET,KX_MIXER_WAVE45,1,&v);
  cfg.write("mixer","wave45_r",v);
  ikx->mixer(KX_PROP_GET,KX_MIXER_WAVE67,0,&v);
  cfg.write("mixer","wave67_l",v);
  ikx->mixer(KX_PROP_GET,KX_MIXER_WAVE67,1,&v);
  cfg.write("mixer","wave67_r",v);
  if(is_a2)
  {
   ikx->mixer(KX_PROP_GET,KX_MIXER_WAVEHQ,0,&v);
   cfg.write("mixer","waveHQ_l",v);
   ikx->mixer(KX_PROP_GET,KX_MIXER_WAVEHQ,1,&v);
   cfg.write("mixer","waveHQ_r",v);
  }
  ikx->mixer(KX_PROP_GET,KX_MIXER_SYNTH,0,&v);
  cfg.write("mixer","synth_l",v);
  ikx->mixer(KX_PROP_GET,KX_MIXER_SYNTH,1,&v);
  cfg.write("mixer","synth_r",v);
  ikx->mixer(KX_PROP_GET,KX_MIXER_REC,0,&v);
  cfg.write("mixer","rec_l",v);
  ikx->mixer(KX_PROP_GET,KX_MIXER_REC,1,&v);
  cfg.write("mixer","rec_r",v);

  ikx->mixer(KX_PROP_GET,KX_MIXER_SYNTH_MUTE,0,&v);
  cfg.write("mixer","synth_mute",v);
  ikx->mixer(KX_PROP_GET,KX_MIXER_REC_MUTE,0,&v);
  cfg.write("mixer","rec_mute",v);
  ikx->mixer(KX_PROP_GET,KX_MIXER_WAVE_MUTE,0,&v);
  cfg.write("mixer","wave_mute",v);
  ikx->mixer(KX_PROP_GET,KX_MIXER_WAVE23_MUTE,0,&v);
  cfg.write("mixer","wave23_mute",v);
  ikx->mixer(KX_PROP_GET,KX_MIXER_WAVE45_MUTE,0,&v);
  cfg.write("mixer","wave45_mute",v);
  ikx->mixer(KX_PROP_GET,KX_MIXER_WAVE67_MUTE,0,&v);
  cfg.write("mixer","wave67_mute",v);
  if(is_a2)
  {
   ikx->mixer(KX_PROP_GET,KX_MIXER_WAVEHQ_MUTE,0,&v);
   cfg.write("mixer","waveHQ_mute",v);
  }
  ikx->mixer(KX_PROP_GET,KX_MIXER_MASTER_MUTE,0,&v);
  cfg.write("mixer","master_mute",v);

  ikx->mixer(KX_PROP_GET,KX_MIXER_LINEIN,0,&v);
  cfg.write("mixer","linein_l",v);
  ikx->mixer(KX_PROP_GET,KX_MIXER_LINEIN,1,&v);
  cfg.write("mixer","linein_r",v);
  ikx->mixer(KX_PROP_GET,KX_MIXER_MICIN,0,&v);
  cfg.write("mixer","micin_l",v);
  ikx->mixer(KX_PROP_GET,KX_MIXER_MICIN,1,&v);
  cfg.write("mixer","micin_r",v);
  ikx->mixer(KX_PROP_GET,KX_MIXER_LINEIN_MUTE,0,&v);
  cfg.write("mixer","linein_mute",v);
  ikx->mixer(KX_PROP_GET,KX_MIXER_MICIN_MUTE,0,&v);
  cfg.write("mixer","micin_mute",v);
 }

 if((kx_saved_flag&KX_SAVED_AC97) && (has_ac97))
 {
  for(i=2;i<0x7f;i+=2)
  {
   word ac97;
   if(ikx->ac97_read((byte)i,&ac97)==0)
    cfg.write("ac97","reg",ac97,i);
  }
 }

 if(kx_saved_flag&KX_SAVED_SOUNDFONTS)
 {
  // write soundfont information
  int sz=ikx->enum_soundfonts(NULL,0);
  if(sz>0)
  {
    sfHeader *hdr=(sfHeader *)malloc(sz);
    if(hdr)
    {
        if(ikx->enum_soundfonts(hdr,sz)==0) // ok
        {
            kString tmp;
            for(dword i=0;i<sz/sizeof(sfHeader);i++)
            {
                tmp.Format("n%d",sz/sizeof(sfHeader)-i);
                cfg.write("soundfonts",(LPCTSTR)tmp,hdr[i].sfman_file_name);

                tmp.Format("n%d_bank",sz/sizeof(sfHeader)-i);
                cfg.write("soundfonts",(TCHAR*)(LPCTSTR)tmp,hdr[i].sfman_id);

                tmp.Format("n%d_synth",sz/sizeof(sfHeader)-i);
                cfg.write("soundfonts",(TCHAR*)(LPCTSTR)tmp,hdr[i].subsynth);
            }
                        tmp.Format("n%d",sz/sizeof(sfHeader)+1);
            cfg.write("soundfonts",(LPCTSTR)tmp,"<last>");
        }
        free(hdr);
    }
  } else cfg.write("soundfonts","n1","<last>");
 }
 if(kx_saved_flag&KX_SAVED_DSP)
 {
  get_pm()->save_all_plugin_settings(cfg);
 }
 // save parser data
 if(kx_saved_flag&KX_SAVED_AUTOMATION)
 {
  if(get_parser()!=NULL)
     get_parser()->save_settings(cfg);
 }

 if(kx_saved_flag&KX_SAVED_ADDONS)
 {
  if(addon_mgr)
   addon_mgr->save_all_addon_settings(KXADDON_PER_DEVICE,cfg);
  else
   debug("kxmixer: internal error: addon mgr is NULL\n");
 }
 return 0;
}

int iKXManager::restore_settings(int flag,kString *fname_,dword kx_saved_flag)
{
    iKX *ikx=get_ikx();
    if(!ikx)
    {
     debug("kxmixer: internal error: restore settings: ikx=NULL\n");
     return -2;
    }

    int i;
    dword v;

    kString fname;

    dword has_ac97=0;

    if(ikx->get_dword(KX_DWORD_AC97_PRESENT,&has_ac97))
    {
        has_ac97=0; // error
    }

    dword is_a2=0;

    if(ikx->get_dword(KX_DWORD_IS_A2,&is_a2))
    {
        is_a2=0; // error
    }

    if(flag&SETTINGS_GUI)
    {
              restore_cwd("kx");
              CFileDialog *f_d = new CFileDialog(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_EXPLORER,
              "kX Settings (*.kx)|*.kx||",CWnd::FromHandle(systray));
              if(f_d)
              {
                char tmp_cwd[MAX_PATH];
                GetCurrentDirectory(sizeof(tmp_cwd),tmp_cwd);
                f_d->m_ofn.lpstrInitialDir=tmp_cwd;

                if(f_d->DoModal()==IDCANCEL)
                {
                 delete f_d;
                 return -1;
                }

                save_cwd("kx");

                fname=f_d->GetPathName();
                delete f_d;

                if(fname.GetLength()==0) // cancel or empty
                {
                 return -2;
                }
                save_lru((char *)(LPCTSTR)fname);
              } else return -3;
    } else 
    if(flag&SETTINGS_AUTO)
    {
     ; // nop
    }
  else
  if(fname_)
   fname=*fname_;
  else
  {
   debug("kxmixer: rest_settings Internal error 1\n");
   return -4;
  }

    char *f;
    if(flag&SETTINGS_AUTO)
     f=NULL;
    else
     f=(char *)(LPCTSTR)fname;

    char tmp_str[KX_MAX_STRING];

    kSettings cfg(ikx->get_device_name(),f,0);

   if(cfg.read("General","flag",&kx_saved_flag)!=0)
   {
    cfg.set_flag(KX_SAVED_NO_CARDNAME);

    if(cfg.read("General","flag",&kx_saved_flag)!=0)
    {
     MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","ini_file"),"Error",MB_OK|MB_ICONEXCLAMATION);
     return -5;
    }
   }
   cfg.set_flag(kx_saved_flag);

   // fixme: modify kx_saved_flag according to GUI settings here

    cfg.read("General","descr",tmp_str,sizeof(tmp_str));
    if(strcmp(tmp_str,"kX Saved Settings")!=0)
    {
      if(!(flag&SETTINGS_MAY_NOT_EXIST))
       MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","ini_file"),"Error",MB_OK|MB_ICONEXCLAMATION);

      if(flag&SETTINGS_MAY_NOT_EXIST) // reset in this case
        reset_settings();
      return -6;
    }
    cfg.read("General","version",tmp_str,sizeof(tmp_str));
    if(strcmp(tmp_str,KX_DRIVER_VERSION_STR)!=0)
    {
     if(!(kx_saved_flag&KX_SAVED_NO_VERSION))
     {
      MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","ini_file_ver"),(LPCTSTR)mf.get_profile("errors","error"),MB_OK|MB_ICONEXCLAMATION);
      return -7;
     }
    }

   int bad_routing=0;

   if(kx_saved_flag&KX_SAVED_ROUTING)
    for(i=0;i<=ROUTING_LAST;i++)
    {
      dword r,xr;
      if((cfg.read("routing","routing",&r,i)==0) &&
       (cfg.read("routing","xrouting",&xr,i)==0))
      {
         ikx->set_routing(i,r,xr);
      }
      else
      {
        if(!bad_routing)
         MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","ini_file2"),"Error",MB_OK|MB_ICONEXCLAMATION);
        bad_routing=1;
        debug("kxmixer: !! note: re-setting routings [legacy settings]\n");
      }
    }

   if(kx_saved_flag&KX_SAVED_AMOUNT)
   {
    for(i=0;i<=MAX_DEF_AMOUNT;i++)
    {
    dword a;
    if(cfg.read("amount","amount",&a,i)==0)
    ikx->set_send_amount(i,(byte)a);
    else
    MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","ini_file3"),"Error",MB_OK|MB_ICONEXCLAMATION);
    }

    if(is_a2)
    {
     for(i=0;i<8;i++)
     {
      dword v=0;
      if(cfg.read("amount","p16v_amount",&v,i)==0)
       ikx->set_p16v_volume(i,v);
     }
    }
   }

   // MUTE the outputs
   ikx->mute();

   if(kx_saved_flag&KX_SAVED_BUFFERS)
   {
    if(cfg.read("buffers","tankmem",&v)==0)
    ikx->set_buffers(KX_TANKMEM_BUFFER,v);
    else
    MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","ini_file4"),"Error",MB_OK|MB_ICONEXCLAMATION);

    if(cfg.read("buffers","playback",&v)==0)
    ikx->set_buffers(KX_PLAYBACK_BUFFER,v);
    else
    MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","ini_file4"),"Error",MB_OK|MB_ICONEXCLAMATION);

    if(cfg.read("buffers","gsif",&v)==0)
     ikx->set_buffers(KX_GSIF_BUFFER,v);

    if(cfg.read("buffers","record",&v)==0)
    ikx->set_buffers(KX_RECORD_BUFFER,v);
    else
    MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","ini_file4"),"Error",MB_OK|MB_ICONEXCLAMATION);

    if(cfg.read("buffers","ac3",&v)==0)
    ikx->set_buffers(KX_AC3_BUFFERS,v);
    else
    MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","ini_file4"),"Error",MB_OK|MB_ICONEXCLAMATION);
   }

   if(kx_saved_flag&KX_SAVED_HWPARAMS)
   {
    // hw parameters
    dword d;

    if(cfg.read("hw_params","doo",&d)==0)
     ikx->set_hw_parameter(KX_HW_DOO,d);
    //else
      //MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","ini_file5"),"Error",MB_OK|MB_ICONEXCLAMATION);
      //debug("kxmixer: setting hw param 'doo': %s\n",(LPCTSTR)mf.get_profile("errors","ini_file5"));

    if(cfg.read("hw_params","spdif_freq",&d)==0)
     ikx->set_hw_parameter(KX_HW_SPDIF_FREQ,d);
    if(cfg.read("hw_params","ecard_routing",&d)==0)
     ikx->set_hw_parameter(KX_HW_ECARD_ROUTING,d);
    if(cfg.read("hw_params","ecard_adc_gain",&d)==0)
     ikx->set_hw_parameter(KX_HW_ECARD_ADC_GAIN,d);
    if(cfg.read("hw_params","spdif_bypass",&d)==0)
     ikx->set_hw_parameter(KX_HW_SPDIF_BYPASS,d);
    if(cfg.read("hw_params","synth_compat",&d)==0)
     ikx->set_hw_parameter(KX_HW_SYNTH_COMPATIBILITY,d);

     // NOT USED: if(cfg.read("hw_params","wave_mix",&d)==0)
    if(cfg.read("hw_params","compat",&d)==0)
     ikx->set_hw_parameter(KX_HW_COMPAT,d);


    if(cfg.read("hw_params","p16v_pb_routing",&d)==0)
     ikx->set_hw_parameter(KX_HW_P16V_PB_ROUTING,d);
    if(cfg.read("hw_params","p16v_rec_routing",&d)==0)
     ikx->set_hw_parameter(KX_HW_P16V_REC_ROUTING,d);

    if(cfg.read("hw_params","swap_f_r",&d)==0)
     ikx->set_hw_parameter(KX_HW_SWAP_FRONT_REAR,d);
    //else
      //MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","ini_file5"),"Error",MB_OK|MB_ICONEXCLAMATION);
      //debug("kxmixer: setting hw param 'swap f&r': %s\n",(LPCTSTR)mf.get_profile("errors","ini_file5"));

    if(cfg.read("hw_params","route_ph2csw",&d)==0)
     ikx->set_hw_parameter(KX_HW_ROUTE_PH_TO_CSW,d);
    //else
      //MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","ini_file5"),"Error",MB_OK|MB_ICONEXCLAMATION);
      //debug("kxmixer: setting hw param 'router ph&csw': %s\n",(LPCTSTR)mf.get_profile("errors","ini_file5"));

    if(cfg.read("hw_params","spdif_decode",&d)==0)
     ikx->set_hw_parameter(KX_HW_SPDIF_DECODE,d);
    if(cfg.read("hw_params","spdif_recording",&d)==0)
     ikx->set_hw_parameter(KX_HW_SPDIF_RECORDING,d);
    if(cfg.read("hw_params","ac3_passthru",&d)==0)
     ikx->set_hw_parameter(KX_HW_AC3_PASSTHROUGH,d);

    if(cfg.read("hw_params","ac97_line2",&d)==0)
     ikx->set_hw_parameter(KX_HW_K2_AC97,d);

    if(cfg.read("hw_params","a2zsnb_src",&d)==0)
     ikx->set_hw_parameter(KX_HW_A2ZSNB_SOURCE,d);

    if(cfg.read("hw_params","kx3d",&d)==0)
     ikx->set_hw_parameter(KX_HW_KX3D,d);
    if(cfg.read("hw_params","sp8ps",&d)==0)
     ikx->set_hw_parameter(KX_HW_8PS,d);

    if(cfg.read("hw_params","drum_channel",&d)==0)
     ikx->set_hw_parameter(KX_HW_DRUM_CHANNEL,d);

    for(int i=0;i<MAX_MIXER_CONTROLS;i++)
    {
       TCHAR name[32];
       kx_assignment_info ai; memset(&ai,0,sizeof(ai));
       ai.level=i;
       ai.max_vol=0x80000000;

       sprintf(name,"slider_pgm%d_name",i);
       if(cfg.read("hw_params",name,ai.pgm,sizeof(ai.pgm))==0)
       {
        sprintf(name,"slider_pgm%d_left",i);
        if(cfg.read("hw_params",name,ai.reg_left,sizeof(ai.reg_left))) continue;

        sprintf(name,"slider_pgm%d_right",i);
        if(cfg.read("hw_params",name,ai.reg_right,sizeof(ai.reg_right))) continue;

        sprintf(name,"slider_pgm%d_max",i);
        dword max_vol=0x80000000;
        if(cfg.read("hw_params",name,&max_vol)) continue;
        ai.max_vol=max_vol;

        ikx->set_dsp_assignments(&ai);
       }
    }
   }

   if((kx_saved_flag&KX_SAVED_AC97)&&has_ac97)
   {
    for(i=2;i<0x7f;i+=2)
    {
        dword ac97;
        if(cfg.read("ac97","reg",&ac97,i)==0)
        {
        ikx->ac97_write((byte)i,(word)ac97);
        } else MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","ini_file6"),"Error",MB_OK|MB_ICONEXCLAMATION);
    }
   }

   // restore microcode
   if(kx_saved_flag&KX_SAVED_DSP)
   {
    get_pm()->load_all_plugin_settings(cfg);
   }

   // load parser data
   if(kx_saved_flag&KX_SAVED_AUTOMATION)
   {
    if(get_parser()!=NULL)
       get_parser()->restore_settings(cfg);
   }

  if(kx_saved_flag&KX_SAVED_MIXER)
  {
    // mixer
    if(cfg.read("mixer","master_l",&v)==0)
    ikx->mixer(KX_PROP_SET,KX_MIXER_MASTER,0,(int *)&v);
    else
    MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","ini_file8"),"Error",MB_OK|MB_ICONEXCLAMATION);
    if(cfg.read("mixer","master_r",&v)==0)
    ikx->mixer(KX_PROP_SET,KX_MIXER_MASTER,1,(int *)&v);
    else
    MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","ini_file8"),"Error",MB_OK|MB_ICONEXCLAMATION);

    if(cfg.read("mixer","wave_l",&v)==0)
    ikx->mixer(KX_PROP_SET,KX_MIXER_WAVE,0,(int *)&v);
    else
    MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","ini_file8"),"Error",MB_OK|MB_ICONEXCLAMATION);
    if(cfg.read("mixer","wave_r",&v)==0)
    ikx->mixer(KX_PROP_SET,KX_MIXER_WAVE,1,(int *)&v);
    else
    MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","ini_file8"),"Error",MB_OK|MB_ICONEXCLAMATION);

    if(cfg.read("mixer","wave23_l",&v)==0)
    ikx->mixer(KX_PROP_SET,KX_MIXER_WAVE23,0,(int *)&v);
    else
    MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","ini_file8"),"Error",MB_OK|MB_ICONEXCLAMATION);
    if(cfg.read("mixer","wave23_r",&v)==0)
    ikx->mixer(KX_PROP_SET,KX_MIXER_WAVE23,1,(int *)&v);
    else
    MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","ini_file8"),"Error",MB_OK|MB_ICONEXCLAMATION);

    if(cfg.read("mixer","wave45_l",&v)==0)
    ikx->mixer(KX_PROP_SET,KX_MIXER_WAVE45,0,(int *)&v);
    else
    MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","ini_file8"),"Error",MB_OK|MB_ICONEXCLAMATION);
    if(cfg.read("mixer","wave45_r",&v)==0)
    ikx->mixer(KX_PROP_SET,KX_MIXER_WAVE45,1,(int *)&v);
    else
    MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","ini_file8"),"Error",MB_OK|MB_ICONEXCLAMATION);

    if(cfg.read("mixer","wave67_l",&v)==0)
    ikx->mixer(KX_PROP_SET,KX_MIXER_WAVE67,0,(int *)&v);
    else
    MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","ini_file8"),"Error",MB_OK|MB_ICONEXCLAMATION);
    if(cfg.read("mixer","wave67_r",&v)==0)
    ikx->mixer(KX_PROP_SET,KX_MIXER_WAVE67,1,(int *)&v);
    else
    MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","ini_file8"),"Error",MB_OK|MB_ICONEXCLAMATION);

    if(is_a2)
    {
     if(cfg.read("mixer","waveHQ_l",&v)==0)
     ikx->mixer(KX_PROP_SET,KX_MIXER_WAVEHQ,0,(int *)&v);
//     else
//     MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","ini_file8"),"Error",MB_OK|MB_ICONEXCLAMATION);
     if(cfg.read("mixer","waveHQ_r",&v)==0)
     ikx->mixer(KX_PROP_SET,KX_MIXER_WAVEHQ,1,(int *)&v);
//     else
//     MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","ini_file8"),"Error",MB_OK|MB_ICONEXCLAMATION);
    }
     
    if(cfg.read("mixer","synth_l",&v)==0)
    ikx->mixer(KX_PROP_SET,KX_MIXER_SYNTH,0,(int *)&v);
    else
    MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","ini_file8"),"Error",MB_OK|MB_ICONEXCLAMATION);
    if(cfg.read("mixer","synth_r",&v)==0)
    ikx->mixer(KX_PROP_SET,KX_MIXER_SYNTH,1,(int *)&v);
    else
    MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","ini_file8"),"Error",MB_OK|MB_ICONEXCLAMATION);

    if(cfg.read("mixer","rec_l",&v)==0)
    ikx->mixer(KX_PROP_SET,KX_MIXER_REC,0,(int *)&v);
    else
    MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","ini_file8"),"Error",MB_OK|MB_ICONEXCLAMATION);
    if(cfg.read("mixer","rec_l",&v)==0)
    ikx->mixer(KX_PROP_SET,KX_MIXER_REC,1,(int *)&v);
    else
    MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","ini_file8"),"Error",MB_OK|MB_ICONEXCLAMATION);

    if(cfg.read("mixer","linein_l",&v)==0)
     ikx->mixer(KX_PROP_SET,KX_MIXER_LINEIN,0,(int *)&v);

    if(cfg.read("mixer","linein_r",&v)==0)
     ikx->mixer(KX_PROP_SET,KX_MIXER_LINEIN,1,(int *)&v);

    if(cfg.read("mixer","micin_l",&v)==0)
     ikx->mixer(KX_PROP_SET,KX_MIXER_MICIN,0,(int *)&v);

    if(cfg.read("mixer","micin_r",&v)==0)
     ikx->mixer(KX_PROP_SET,KX_MIXER_MICIN,1,(int *)&v);

    if(cfg.read("mixer","synth_mute",&v)==0)
    ikx->mixer(KX_PROP_SET,KX_MIXER_SYNTH_MUTE,0,(int *)&v);
    else
    MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","ini_file8"),"Error",MB_OK|MB_ICONEXCLAMATION);

    if(cfg.read("mixer","rec_mute",&v)==0)
    ikx->mixer(KX_PROP_SET,KX_MIXER_REC_MUTE,0,(int *)&v);
    else
    MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","ini_file8"),"Error",MB_OK|MB_ICONEXCLAMATION);

    if(cfg.read("mixer","wave_mute",&v)==0)
    ikx->mixer(KX_PROP_SET,KX_MIXER_WAVE_MUTE,0,(int *)&v);
    else
    MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","ini_file8"),"Error",MB_OK|MB_ICONEXCLAMATION);

    if(cfg.read("mixer","wave23_mute",&v)==0)
    ikx->mixer(KX_PROP_SET,KX_MIXER_WAVE23_MUTE,0,(int *)&v);
    else
    MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","ini_file8"),"Error",MB_OK|MB_ICONEXCLAMATION);

    if(cfg.read("mixer","wave45_mute",&v)==0)
    ikx->mixer(KX_PROP_SET,KX_MIXER_WAVE45_MUTE,0,(int *)&v);
    else
    MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","ini_file8"),"Error",MB_OK|MB_ICONEXCLAMATION);

    if(cfg.read("mixer","wave67_mute",&v)==0)
    ikx->mixer(KX_PROP_SET,KX_MIXER_WAVE67_MUTE,0,(int *)&v);
    else
    MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","ini_file8"),"Error",MB_OK|MB_ICONEXCLAMATION);

    if(is_a2)
    {
     if(cfg.read("mixer","waveHQ_mute",&v)==0)
     ikx->mixer(KX_PROP_SET,KX_MIXER_WAVEHQ_MUTE,0,(int *)&v);
     //else
     //MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","ini_file8"),"Error",MB_OK|MB_ICONEXCLAMATION);
    }

    if(cfg.read("mixer","linein_mute",&v)==0)
     ikx->mixer(KX_PROP_SET,KX_MIXER_LINEIN_MUTE,0,(int *)&v);

    if(cfg.read("mixer","micin_mute",&v)==0)
     ikx->mixer(KX_PROP_SET,KX_MIXER_MICIN_MUTE,0,(int *)&v);

    if(cfg.read("mixer","master_mute",&v)==0)
    ikx->mixer(KX_PROP_SET,KX_MIXER_MASTER_MUTE,0,(int *)&v);
    else
    MessageBox(NULL,(LPCTSTR)mf.get_profile("errors","ini_file8"),"Error",MB_OK|MB_ICONEXCLAMATION);
  }

  ikx->unmute();

   if(kx_saved_flag&KX_SAVED_SOUNDFONTS)
   {
    // unload current soundfonts
    int sz=ikx->enum_soundfonts(NULL,0);
    if(sz>0)
    {
        sfHeader *hdr=(sfHeader *)malloc(sz);
        if(hdr)
        {
            if(ikx->enum_soundfonts(hdr,sz)==0) // ok
            {
                for(dword i=0;i<sz/sizeof(sfHeader);i++)
                {
                    ikx->unload_soundfont(hdr[i].rom_ver.minor);
                }
            }
            free(hdr);
        }
    }
    // now, upload all soundfonts
    dword cnt=1;
    while(1)
    {
     kString tmp;
     char str[256]; str[0]=0;
     tmp.Format("n%d",cnt);
     cfg.read("soundfonts",(LPCTSTR)tmp,str,sizeof(str));
     if(str[0]==0 || (strcmp(str,"$none$")==0) || (strcmp(str,"<last>")==0))
      break;

     dword bank=0;
     tmp.Format("n%d_bank",cnt);
     cfg.read("soundfonts",(TCHAR*)(LPCTSTR)tmp,&bank);

     dword subsynth=0;
     tmp.Format("n%d_synth",cnt);
     cfg.read("soundfonts",(TCHAR*)(LPCTSTR)tmp,&subsynth);

     ikx->parse_soundfont(str,NULL,bank,subsynth); // and upload it automatically
     cnt++;
    }
 }

 if(kx_saved_flag&KX_SAVED_ADDONS)
 {
  if(addon_mgr)
   addon_mgr->load_all_addon_settings(KXADDON_PER_DEVICE,cfg);
  else
   debug("kxmixer: internal error: addon mgr is NULL\n");
 }

 return 0;
}
