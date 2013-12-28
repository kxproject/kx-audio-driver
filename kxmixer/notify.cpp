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

#include "notify.h"
#include "midirouter.h"
#include "notify_gui.h"
#include "addonmgr.h"

extern void process_custom(iKXNotifier *that,int msg);

/////////////////////////////////////////////////////////////////////////////

iKXNotifier::iKXNotifier()
{
    midi_in=NULL;
/*
    seq=0;
    dir=0;
    last=0;
    vol_step=0;
    vol_cnt=0;
*/
    device_num_t=-1;

    last_pt_on=0;
    last_3d_on=0;
    last_sw_on=0;
    last_spdif=0;
    last_spdif_ac3=0;

    osd_icon=NULL;
    osd_volume=NULL;

    parser_t=NULL;
    ikx_t=NULL;
    pm_t=NULL;

    memset(&header,0,sizeof(header));

    prev_gpio=0xff;
    is_edsp=0;
}

iKXNotifier::~iKXNotifier()
{
    close();

    if(osd_icon)
    {
     osd_icon->DestroyWindow();
     delete osd_icon;
     osd_icon=NULL;
    }
    if(osd_volume)
    {
     osd_volume->DestroyWindow();
     delete osd_volume;
         osd_volume=NULL;
    }
}

int iKXNotifier::close()
{
    if(midi_in)
    {
        midiInReset(midi_in);
        if(header.lpData)
        {
            midiInUnprepareHeader(midi_in,&header,sizeof(header));
            memset(&header,0,sizeof(header));
        }
        midiInStop(midi_in);
        midiInClose(midi_in);
        midi_in=NULL;
    }
    if(osd_icon)
    {
     osd_icon->DestroyWindow();
     delete osd_icon;
     osd_icon=NULL;
    }
    if(osd_volume)
    {
     osd_volume->DestroyWindow();
     delete osd_volume;
         osd_volume=NULL;
    }

    return 0;
}

int iKXNotifier::init(int device_num_)
{
    close();

    ikx_t=manager->get_ikx(device_num_);
    if(ikx_t==NULL)
     return -5;

    ikx_t->get_dword(KX_DWORD_IS_EDSP,&is_edsp);

    pm_t=manager->get_pm(device_num_);

    UINT midi_device_num=ikx_t->get_winmm_device(KX_WINMM_CTRL,0);

    if(midi_device_num==0xffffffff)
    {
            debug("kX remote: midi devices not found\n");
            close();
            return -5;
    }

    // create OSD windows
    osd_icon=new COSDIcon;
    osd_icon->create(this);

    osd_volume=new COSDVolume;
    osd_volume->create(this);

    kSettings cfg;
    dword disabled=0;
    if((cfg.read_abs("Mixer","NoControl",&disabled)==0)&&(disabled==1))
    {
            debug("kX remote/kX Control: initialization aborted due to 'NoControl' flag\n");
            close();
            return -6;
    }

        MMRESULT result=midiInOpen(&midi_in,midi_device_num,(DWORD_PTR)osd_icon->get_wnd(),0,CALLBACK_WINDOW);
        if(result!=MMSYSERR_NOERROR)
        {
            debug("kX remote: error opening midi in device\n");
            close();
            return -6;
        }
        header.lpData=buffer;
        header.dwBufferLength=sizeof(buffer);
        header.dwFlags=0;

        if(midiInPrepareHeader(midi_in,&header,sizeof(header))!=MMSYSERR_NOERROR)
        {
            debug("kX remote: error preparing header\n");
            close();
            return -7;
        }

        if(midiInAddBuffer(midi_in,&header,sizeof(header))!=MMSYSERR_NOERROR)
        {
            debug("kX remote: error adding buffer\n");
            close();
            return -9;
        }

        if(midiInStart(midi_in)!=MMSYSERR_NOERROR)
        {
            debug("kX remote: error starting midi-in\n");
            close();
            return -8;
        }
        device_num_t=device_num_;

        return 0;
}

int iKXNotifier::stop()
{
 if(midi_in)
 {
  midiInReset(midi_in);
  midiInStop(midi_in);
 }

 return 0;
}

int iKXNotifier::resume(int force)
{
 if(midi_in)
 {
        midiInReset(midi_in);
        midiInStop(midi_in);

        header.lpData=buffer;
        header.dwBufferLength=sizeof(buffer);
        header.dwFlags=0;

        if(midiInPrepareHeader(midi_in,&header,sizeof(header))!=MMSYSERR_NOERROR)
        {
            debug("kX remote: error preparing header [after suspend]\n");
            close();
            return -7;
        }

        if(midiInAddBuffer(midi_in,&header,sizeof(header))!=MMSYSERR_NOERROR)
        {
            debug("kX remote: error adding buffer [after suspend]\n");
            close();
            return -9;
        }

        if(midiInStart(midi_in)!=MMSYSERR_NOERROR)
        {
            debug("kX remote: error starting midi-in [after suspend]\n");
            close();
            return -8;
        }
 }

 return 0;
}

LRESULT iKXNotifier::callback_long(WPARAM w,LPARAM l)
{
 MIDIHDR *hdr=(MIDIHDR *)l;

 char *p=hdr->lpData;

 if(hdr->dwBytesRecorded>0)
 {
  // process RemoteIR data here:
   if((memcmp(hdr->lpData,KX_SYSEX_REMOTE2_INITIAL,KX_SYSEX_REMOTE2_INITIAL_SIZE)==0) 
      && (hdr->dwBytesRecorded==KX_SYSEX_REMOTE2_SIZE)
     ) 
   {
    switch((int)(*(word *)(&p[8])))
    {
        case 0x0100: // cmss
            process_message(KXR_X_CMSS); break;
        case 0x0200: // mute
            process_message(KXR_X_MUTE); break;
        case 0x0101:
            process_message(KXR_X_VOLINCR); break;
        case 0x4101:
            process_message(KXR_X_VOLDECR); break;
        default:
                if(p[8]==0x2) // gpio on a2ex
                {
                 process_gpio(p[9],1);
                }
                else
             debug("kxmixer: unknown remote2 notification [%x %x %x]\n",
               p[8],p[9],p[10]);
            break;
    }
   }
   else
   if((memcmp(hdr->lpData,KX_SYSEX_REMOTE_INITIAL,KX_SYSEX_REMOTE_INITIAL_SIZE)==0) 
      && (hdr->dwBytesRecorded==KX_SYSEX_REMOTE_SIZE)
     )
   {
    int mode=0;
    switch((int)(*(word *)(&p[10])) + ((p[7]==0x9)?0:0x10000))
    {
        case 0x12f50: mode=KXR_SPEAKER; // fall through
        case 0x12e51: // '1'
            process_message(KXR_1|mode); break;
    case 0x10f70: mode=KXR_SPEAKER; // fall through
        case 0x10e71: // '2' 
            process_message(KXR_2|mode); break;
    case 0x0f70: mode=KXR_SPEAKER; // fall through
    case 0x0e71: // 'return'
        process_message(KXR_RETURN|mode); break;
    case 0x7708: mode=KXR_SPEAKER; // fall through
        case 0x7609: // '3' 
            process_message(KXR_3|mode); break;
    case 0x2f50: mode=KXR_SPEAKER; // fall through
    case 0x2e51: // '4'
        process_message(KXR_4|mode); break;
    case 0x5f20: mode=KXR_SPEAKER; // fall through
        case 0x5e21: // '5' 
            process_message(KXR_5|mode); break;
    case 0x15f20: mode=KXR_SPEAKER; // fall through
        case 0x15e21: // 'stop/eject'
            process_message(KXR_STOP|mode); break;
    case 0x601f: mode=KXR_SPEAKER; // fall through
        case 0x611e: // '6' 
            process_message(KXR_6|mode); break;
    case 0x1601f: mode=KXR_SPEAKER; // fall through
    case 0x1611e: // 'play/pause'
        process_message(KXR_PLAY|mode); break;
    case 0x16f10: mode=KXR_SPEAKER; // fall through
        case 0x16e11: // '7' 
            process_message(KXR_7|mode); break;
    case 0x6f10: mode=KXR_SPEAKER; // fall through
        case 0x6e11: // 'start/menu'
            process_message(KXR_START|mode); break;
    case 0x13f40: mode=KXR_SPEAKER; // fall through
        case 0x13e41: // '8' 
            process_message(KXR_8|mode); break;
    case 0x3f40: mode=KXR_SPEAKER; // fall through
    case 0x3e41: // 'options'
        process_message(KXR_OPTIONS|mode); break;
    case 0x1106f: mode=KXR_SPEAKER; // fall through
        case 0x1116e: // '9' 
            process_message(KXR_9|mode); break;
    case 0x106f: mode=KXR_SPEAKER; // fall through
        case 0x116e: // 'display'
            process_message(KXR_DISPLAY|mode); break;
    case 0x7f00: mode=KXR_SPEAKER; // fall through
        case 0x7e01: // '0'
            process_message(KXR_0|mode); break;
    case 0x1403f: mode=KXR_SPEAKER; // fall through
        case 0x1413e: // 'slow'
            process_message(KXR_SLOW|mode); break;
    case 0x1007f: mode=KXR_SPEAKER; // fall through
        case 0x1017e: // 'prev'
            process_message(KXR_PREV|mode); break;
    case 0x205f: mode=KXR_SPEAKER; // fall through
        case 0x215e: // 'next'
            process_message(KXR_NEXT|mode); break;
    case 0x007f: mode=KXR_SPEAKER; // fall through
        case 0x017e: // 'step'
            process_message(KXR_STEP|mode); break;
    case 0x877: mode=KXR_SPEAKER; // fall through
        case 0x0976: // 'mute'
            process_message(KXR_MUTE|mode); break;
    case 0x3847: mode=KXR_SPEAKER; // fall through
        case 0x3946: // 'vol-'
            process_message(KXR_VOLINCR|mode); break;
    case 0x13847: mode=KXR_SPEAKER; // fall through
        case 0x13946: // 'vol+'
            process_message(KXR_VOLDECR|mode); break;
    case 0x4f30: mode=KXR_SPEAKER; // fall through
        case 0x4e31: // 'eax'
            process_message(KXR_EAX|mode); break;
    case 0x403f: mode=KXR_SPEAKER; // fall through
        case 0x413e: // 'close/cancel'
            process_message(KXR_CLOSE|mode); break;
    case 0x1205f: mode=KXR_SPEAKER; // fall through
        case 0x1215e: // 'up'
            process_message(KXR_UP|mode); break;
    case 0x1502f: mode=KXR_SPEAKER; // fall through
        case 0x1512e: // 'fw'
            process_message(KXR_FW|mode); break;
    case 0x14f30: mode=KXR_SPEAKER; // fall through
        case 0x14e31: // 'down'
            process_message(KXR_DOWN|mode); break;
    case 0x11f60: mode=KXR_SPEAKER; // fall through
        case 0x11e61: // 'rw'
            process_message(KXR_RW|mode); break;
    case 0x17f00: mode=KXR_SPEAKER; // fall through
        case 0x17e01: // 'select/ok'
            process_message(KXR_SELECT|mode); break;

        case 0x17906: // 'power'
            process_message(KXR_POWER); break;
        case 0x1710e: // 'cmss'
            process_message(KXR_CMSS); break;
        case 0x1314e: // 'rec'
            process_message(KXR_REC); break;

        default:
            debug("!! kxmixer: unknown remoteIR command: %x.%x.%x [%x]\n",
             p[10],p[11],p[7],(int)(*(word *)(&p[10])) + (p[7]==0x9?0:0x10000));
    }
  }
  else
  if(memcmp(hdr->lpData,KX_SYSEX_GMGS_VOLUME_INITIAL,KX_SYSEX_GMGS_VOLUME_INITIAL_SIZE)==0) // GM Master Volume
  {
    int master_vol=((p[5]&0x7f)+((p[6]&0x7f)<<7))<<2;
    if(master_vol==0)
     master_vol=1;

        ikx_t->mixer(KX_PROP_SET,
            KX_MIXER_SYNTH,0,&master_vol);
        ikx_t->mixer(KX_PROP_SET,
            KX_MIXER_SYNTH,1,&master_vol);
  }
  else
  if(memcmp(hdr->lpData,KX_SYSEX_INITIAL,KX_SYSEX_INITIAL_SIZE)==0) // notification
  {
     char *p=(char *)hdr->lpData;
     switch(p[KX_SYSEX_FUNCTION])
     {
      case KX_SYSEX_NOTIFY_F:
        switch(p[KX_SYSEX_SUBFUNCTION])
        {
         case 1:
            {
                debug("kxmixer: PCI Bus error\n");
                dword hw_compat=0;
                ikx_t->get_hw_parameter(KX_HW_COMPAT,&hw_compat);
                if(!(hw_compat&KX_HW_DISABLE_PCI_NOTIFY))
                    MessageBox(NULL,"Warning: PCI bus error detected.\nPlease verify your card is installed properly in a PCI slot.\nIf audio card is not accessible, turn off your PC, wait some time and reboot.","kX Mixer",MB_OK|MB_ICONSTOP);
                break;
            }
         case 2: process_spdif(); break;
         case 3: process_notify(ICON_AC3_PT_ON);  break;
         case 4: process_notify(ICON_AC3_PT_OFF); break;
         case 5: process_notify(ICON_AC3_SW_ON);  break;
         case 6: process_notify(ICON_AC3_SW_OFF); break;
         case 7: process_notify(ICON_3D_START); break;
         case 8: process_notify(ICON_3D_STOP); break;
         case 9: process_edsp(); break;
         default:
          debug("kxmixer: unknown notification subfunction [%x]\n",p[KX_SYSEX_SUBFUNCTION]);
          break;
        }
        break;
      case KX_SYSEX_GPIO_F:
        {
         byte tmp=p[KX_SYSEX_GPIO_POS1]+(p[KX_SYSEX_GPIO_POS2]?0x80:0);
         process_gpio(tmp,0);
        }
        break;
      default:
        debug("kxmixer: unknown driver notification code [%x.%x.%x]\n",p[7],p[8],p[9]);
     }
  }
//  else
//   debug("kxmixer: unknown notification from the driver\n");

  if(midiInAddBuffer(midi_in,&header,sizeof(header))!=MMSYSERR_NOERROR)
  {
    debug("kX remote: error adding buffer (2)\n");
    close();
    return -9;
  }
 }

 return 0;
}

LRESULT iKXNotifier::callback_short(WPARAM w,LPARAM l)
{
 // debug("[%x.%x.%x]\n",l&0xff,(l&0xff00)>>8,(l&0xff0000)>>16);
 if(parser_t)
  parser_t->parse_cc((dword)l);
 return 0;
}

int iKXNotifier::process_message(int message)
{
 if(manager->addon_mgr->event(KXADDON_EVENT_REMOTEIR,(void *)(intptr_t)message,0)==0)
  return 0;

 if(!remote_ir_enabled)
 {
  if(!(message>KXR_X_MUTE && message<=KXR_KEYB_LAST))
   return 0;
 }

/* dword curr=GetTickCount();

 if(curr-last<=VOL_TIMEOUT)
 {
  vol_step=7000;
  seq++;
 }
 else
 {
  seq=0;
 }

 last=curr;
*/
 int processed=0;
 int mult=10;

 switch(message&0xff)
 {
    case KXR_MUTE:
    case KXR_X_MUTE:
    case KXR_KEYB_MUTE:
        {
        int mute=0;
        ikx_t->mixer(KX_PROP_GET,KX_MIXER_MASTER_MUTE,0,&mute);
        if(mute) mute=1;
        mute=1-mute;
        // vol_cnt=0;
        ikx_t->mixer(KX_PROP_SET,KX_MIXER_MASTER_MUTE,0,&mute);
        processed=1;
        }
        break;
    case KXR_VOLDECR:
        mult=15;
    case KXR_X_VOLDECR:
    case KXR_KEYB_VOLDECR:
        {
/*      if(dir!=-1)
        {
         vol_step=VOL_STEP;
         seq=0;
        }
        vol_cnt=vol_step/VOL_STEP;
        dir=-1;
*/
        #define VOL_STEP    0x500

                int vol_l=0,vol_r=0;
                ikx_t->mixer(KX_PROP_GET,KX_MIXER_MASTER,0,&vol_l);
                ikx_t->mixer(KX_PROP_GET,KX_MIXER_MASTER,1,&vol_r);
                vol_l-=VOL_STEP*mult/10; if(vol_l<0) vol_l=0; if(vol_l>0xffff) vol_l=0xffff;
                vol_r-=VOL_STEP*mult/10; if(vol_r<0) vol_r=0; if(vol_r>0xffff) vol_r=0xffff;
                ikx_t->mixer(KX_PROP_SET,KX_MIXER_MASTER,0,&vol_l);
                ikx_t->mixer(KX_PROP_SET,KX_MIXER_MASTER,1,&vol_r);

        processed=1;
        }
        break;
    case KXR_VOLINCR:
        mult=15;
    case KXR_X_VOLINCR:
    case KXR_KEYB_VOLINCR:
        {
                /*
        if(dir!=1)
        {
         vol_step=VOL_STEP;
         seq=0;
        }
        dir=1;
        vol_cnt=vol_step/VOL_STEP;
                */

                int vol_l=0,vol_r=0;
                ikx_t->mixer(KX_PROP_GET,KX_MIXER_MASTER,0,&vol_l);
                ikx_t->mixer(KX_PROP_GET,KX_MIXER_MASTER,1,&vol_r);
                vol_l+=VOL_STEP*mult/10; if(vol_l<0) vol_l=0; if(vol_l>0xffff) vol_l=0xffff;
                vol_r+=VOL_STEP*mult/10; if(vol_r<0) vol_r=0; if(vol_r>0xffff) vol_r=0xffff;
                ikx_t->mixer(KX_PROP_SET,KX_MIXER_MASTER,0,&vol_l);
                ikx_t->mixer(KX_PROP_SET,KX_MIXER_MASTER,1,&vol_r);

        processed=1;
        }
        break;
        default:
            process_custom(this,message);
            break;
 }

 if(processed)
 {
/*   if(vol_cnt>0)
    osd_volume->SetTimer(SUBVOL_TIMER_ID,VOL_LATENCY/vol_cnt,NULL);
   osd_volume->on_timer(SUBVOL_TIMER_ID);
*/
   osd_notify(ICON_VOLUME,NULL);
 }

 return 0;
}

void iKXNotifier::osd_notify(int icon,const char *str)
{
   kSettings cfg;
   dword fl=0;

   cfg.read_abs("Mixer","Notifications",&fl);
   fl=fl^0xffffffff;
   icon&=fl;
   if(icon==0)
    return;

   if(icon&ICON_VOLUME)
   {
    osd_volume->start(icon,str?str:"");
   }
   else
   {
    // check for multiple on/off
    if((icon==ICON_AC3_PT_ON)&&(GetTickCount()-last_pt_on<4000))
     return;
    if((icon==ICON_AC3_SW_ON)&&(GetTickCount()-last_sw_on<4000))
     return;
    if((icon==ICON_3D_START)&&(GetTickCount()-last_3d_on<4000))
     return;

    if((icon==ICON_SPDIF))
    {
     if(GetTickCount()-last_spdif<3000)
      return;
     last_spdif=GetTickCount();
    }
    if((icon==ICON_SPDIF_AC3))
    {
     if(GetTickCount()-last_spdif_ac3<3000)
      return;
     last_spdif_ac3=GetTickCount();
    }
    osd_icon->start(icon);
   }
}

void iKXNotifier::process_gpio(byte i,int where)
{
 if(is_edsp)
  return;

 debug("kxmixer: GPIO: %x [%s]\n",i,where?"a2ex":"generic");
 osd_notify(ICON_SPEAKER,"");

 // compare
 if(prev_gpio!=i)
 {
  int pgm_id=0;

  dword d=0;
  ikx_t->get_dword(KX_DWORD_IS_A2ZSNB,&d);
  
  if(d==0)
  debug("kxmixer: speaker config: %s%s%s%s%s%s\n",
   (i&(HCFG_K2_DIGITAL_JACK>>8))?"digital ":"",
   (i&(HCFG_K2_FRONT_JACK>>8))?"front ":"",
   (i&(HCFG_K2_REAR_JACK>>8))?"rear ":"",
   (i&(HCFG_K2_PHONES_JACK>>8))?"ld_phones ":"",
   (i&(HCFG_K2_MULTIPURPOSE_JACK>>8))?"c/lfe/digital ":"",
   (i&2)?"a2ex_digital ":"");
  else
  debug("kxmixer: plug config: %s%s%s%s%s\n",
   !(i&(HCFG_ZSNB_OPT_IN>>8))?"optical_in ":"",
   !(i&(HCFG_ZSNB_LINE_IN>>8))?"line_in ":"",
   !(i&(HCFG_ZSNB_OPT_OUT>>8))?"optical_out ":"",
   !(i&(HCFG_ZSNB_PHONES>>8))?"phones ":"",
   !(i&(HCFG_ZSNB_SPEAKERS>>8))?"multispeaker":"");

   pgm_id=pm_t->have_plugin("ceffc302-ea28-44df-873f-d3df1ba31736");

  if(pgm_id!=0)
  {
      dword val=1234;

      if(prev_gpio==0xff) // first time - no prev. state
      {
       if(ikx_t->get_dsp_register(pgm_id,"out0",&val)==0)
       {
        if(val==0)
         prev_gpio=1; // option was [probably] turned on
        else
         prev_gpio=0;
       }
        else
         prev_gpio=0;
      }

      if((prev_gpio&1)!=(i&1))
      {
       // headphones
       kSettings cfg(ikx_t->get_device_name());

       dword cfg_automute=0;
       cfg.read("hw_params","Headphones",&cfg_automute);

       if(cfg_automute)
       {
        if(i&1)
        {
         debug("kxmixer: muting speakers [headphones plugged in]\n");
         ikx_t->set_dsp_register(pgm_id,"out0vol",0);
         ikx_t->set_dsp_register(pgm_id,"out1vol",0);
         ikx_t->set_dsp_register(pgm_id,"out17vol",0); // center/lfe
         ikx_t->set_dsp_register(pgm_id,"out18vol",0);
         ikx_t->set_dsp_register(pgm_id,"out8vol",0);
         ikx_t->set_dsp_register(pgm_id,"out9vol",0);

         // digital
         ikx_t->set_dsp_register(pgm_id,"out2vol",0);
         ikx_t->set_dsp_register(pgm_id,"out3vol",0);
         ikx_t->set_dsp_register(pgm_id,"out4vol",0);
         ikx_t->set_dsp_register(pgm_id,"out5vol",0);

         // 7.1
         ikx_t->set_dsp_register(pgm_id,"out30vol",0);
         ikx_t->set_dsp_register(pgm_id,"out31vol",0);
        }
        else
        {

         dword vol=0x20000000;

         debug("kxmixer: un-muting speakers [headphones plugged out]\n");
         ikx_t->set_dsp_register(pgm_id,"out0vol",vol);
         ikx_t->set_dsp_register(pgm_id,"out1vol",vol);
         ikx_t->set_dsp_register(pgm_id,"out8vol",vol);
         ikx_t->set_dsp_register(pgm_id,"out9vol",vol);
         ikx_t->set_dsp_register(pgm_id,"out17vol",vol);
         ikx_t->set_dsp_register(pgm_id,"out18vol",vol);

         // digital
         ikx_t->set_dsp_register(pgm_id,"out2vol",vol);
         ikx_t->set_dsp_register(pgm_id,"out3vol",vol);
         ikx_t->set_dsp_register(pgm_id,"out4vol",vol);
         ikx_t->set_dsp_register(pgm_id,"out5vol",vol);

         // 7.1
         ikx_t->set_dsp_register(pgm_id,"out30vol",vol);
         ikx_t->set_dsp_register(pgm_id,"out31vol",vol);
        }
       } else debug("kxmixer: headphones plugging ignored\n");
      }

      prev_gpio=i;
  } else debug("kxmixer: cannot tweak headphones: epilog not found\n");
 }
}

void iKXNotifier::process_spdif()
{
 kx_spdif_i2s_status st;
 if(ikx_t->get_spdif_i2s_status(&st)==0)
 {
  // 703ffff + 2108504
  // 303ffff + 2108506
  if((st.spdif.channel_status_b!=0xffffffff) && (st.spdif.channel_status_b!=0))
  {
   if(/*!(st.spdif.srt_status_b&SRCS_VALIDAUDIO)&&*/(st.spdif.channel_status_b&SCS_NOTAUDIODATA))
   {
    osd_notify(ICON_SPDIF_AC3,"");
    debug("kxmixer: incoming ac-3 stream...[coaxial]\n");
    dword do_it=0;
    if(ikx_t->get_hw_parameter(KX_HW_SPDIF_DECODE,&do_it)==0 && (do_it))
    {
     debug("kxmixer: perform decoding...[coaxial]\n");
     // open wavein here...
     {
        dword val=0;
        if(ikx_t->get_hw_parameter(KX_HW_SPDIF_RECORDING,&val)==0)
        {
         if(ikx_t->set_hw_parameter(KX_HW_SPDIF_RECORDING,2)==0) // coaxial
         {
           create_decoder(device_num_t);
           ikx_t->set_hw_parameter(KX_HW_SPDIF_RECORDING,val);
         } else debug("kxmixer: error setting spdif rec status [coaxial]\n");
        } else debug("kxmixer: error getting spdif rec status [coaxial]\n");
     }
    } else debug("kxmixer: ac-3 stream ignored [coaxial]\n");
   }
   else
   {
    osd_notify(ICON_SPDIF,"");
    debug("kxmixer: spdif (generic)\n");

    destroy_decoder(device_num_t);
   }
  }
  else
    if((st.spdif.channel_status_a!=0xffffffff) && (st.spdif.channel_status_a!=0))
    {
     if(/*!(st.spdif.srt_status_b&SRCS_VALIDAUDIO)&&*/(st.spdif.channel_status_a&SCS_NOTAUDIODATA))
     {
      osd_notify(ICON_SPDIF_AC3,"");
      debug("kxmixer: incoming ac-3 stream...[cd-spdif]\n");
      dword do_it=0;
      if(ikx_t->get_hw_parameter(KX_HW_SPDIF_DECODE,&do_it)==0 && (do_it))
      {
       debug("kxmixer: perform decoding...[cd-spdif]\n");
       // open wavein here...
       {
          dword val=0;
          if(ikx_t->get_hw_parameter(KX_HW_SPDIF_RECORDING,&val)==0)
          {
           if(ikx_t->set_hw_parameter(KX_HW_SPDIF_RECORDING,1)==0) // cd-spdif
           {
             create_decoder(device_num_t);
             ikx_t->set_hw_parameter(KX_HW_SPDIF_RECORDING,val);
           } else debug("kxmixer: error setting spdif rec status [cd-spdif]\n");
          } else debug("kxmixer: error getting spdif rec status [cd-spdif]\n");
       }
      } else debug("kxmixer: ac-3 stream ignored [cd-spdif]\n");
     }
     else
     {
      osd_notify(ICON_SPDIF,"");
      debug("kxmixer: spdif (generic)\n");

      destroy_decoder(device_num_t);
     }
    }
   else
   {
    osd_notify(ICON_SPDIF,"");
    debug("kxmixer: spdif sync lost\n");
   }
 }
}

void iKXNotifier::process_notify(int cmd)
{
 switch(cmd)
 {
  case ICON_AC3_PT_ON: debug("kxmixer: ac3 passthru on\n"); osd_notify(cmd,""); break;
  case ICON_AC3_PT_OFF: debug("kxmixer: ac3 passthru off\n"); last_pt_on=GetTickCount(); break;
  case ICON_AC3_SW_ON: debug("kxmixer: ac3 decode on\n"); osd_notify(cmd,""); break;
  case ICON_AC3_SW_OFF: debug("kxmixer: ac3 decode off\n"); last_sw_on=GetTickCount(); break;
  case ICON_3D_START: debug("kxmixer: kX 3D Engine started \n"); osd_notify(cmd,""); break;
  case ICON_3D_STOP: debug("kxmixer: kX 3D Engine stopped\n"); last_3d_on=GetTickCount(); break;
 }
}

void iKXNotifier::process_edsp(void)
{
 debug("kxmixer: e-dsp notification\n");

 osd_notify(ICON_EDSP,"");

 manager->addon_mgr->event(KXADDON_EVENT_GENERIC,(void *)IKX_EDSP_CONFIG_CHANGED,0);
}
