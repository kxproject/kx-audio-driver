// kX Driver
// Copyright (c) Eugene Gavrilov, 2001-2014.
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


#include "kx.h"

KX_API(int,kx_rec_open(kx_hw *hw,kx_voice_buffer *buffer,int usage,int sampling_rate,int rec_subdevice))
{
 debug(DLIB,"kx_rec_open: sampling rate: %d %s %s; [%s]\n",sampling_rate,
  usage&VOICE_FLAGS_STEREO?"Stereo":"Mono",
  usage&VOICE_FLAGS_16BIT?"16 bit":"8 bit",rec_subdevice?"Direct":"Generic");

 if((rec_subdevice!=0 && (!hw->is_10k2)) || rec_subdevice>1)
 {
  debug(DLIB,"! rec[open]: 10k2 only for spdif rec\n");
  return -8;
 }

 if(!(usage&VOICE_FLAGS_16BIT))
 {
  debug(DLIB,"! no 8bit support!\n");
  return 2;
 }

 unsigned long flags=0;
 kx_lock_acquire(hw,&hw->k_lock, &flags);

 kx_rec_voice *voice=&hw->rec_voicetable[rec_subdevice];

 if(voice_usage(voice->usage)!=VOICE_USAGE_FREE)
 {
  debug(DLIB,"! REC [%d] busy\n",rec_subdevice);
  kx_lock_release(hw,&hw->k_lock, &flags);
  return 1; // Busy
 }

 if(rec_subdevice==0)
 {
     switch(sampling_rate)
     {
        	case 48000:
        		voice->rec_set = ADCSR_SAMPLERATE_48;
        		break;
        	case 44100:
        		voice->rec_set = ADCSR_SAMPLERATE_44;
        		break;
        	case 32000:
        		voice->rec_set = ADCSR_SAMPLERATE_32;
        		break;
        	case 24000:
        		voice->rec_set = ADCSR_SAMPLERATE_24;
        		break;
        	case 22050:
        		voice->rec_set = ADCSR_SAMPLERATE_22;
        		break;
        	case 16000:
        		voice->rec_set = ADCSR_SAMPLERATE_16;
        		break;
        	case 11025:
        		voice->rec_set = (hw->is_10k2?ADCSR_SAMPLERATE_11_K2:ADCSR_SAMPLERATE_11_K1);
        		break;
        	case 8000:
        		voice->rec_set = (hw->is_10k2?ADCSR_SAMPLERATE_8_K2:ADCSR_SAMPLERATE_8_K1);
        		break;
        	default:
                    debug(DLIB,"rec_open: what a sampling rate? (%d)\n",sampling_rate);
                    kx_lock_release(hw,&hw->k_lock, &flags);
                    return -1;
     }

     if(hw->is_10k2)
     {
       voice->rec_set |= ADCSR_LCHANENABLE_K2;

       if(usage&VOICE_FLAGS_STEREO)
         voice->rec_set |= ADCSR_RCHANENABLE_K2;
     }
     else
     {
       voice->rec_set |= ADCSR_LCHANENABLE_K1;

       if(usage&VOICE_FLAGS_STEREO)
          voice->rec_set |= ADCSR_RCHANENABLE_K1;
     }
 }
 else
  if(rec_subdevice==1)
  {
   if(!(usage&VOICE_FLAGS_STEREO))
   {
    debug(DLIB,"rec[open]: only stereo data for spdif rec supported\n");
    kx_lock_release(hw,&hw->k_lock, &flags);
    return -10;
   }

   voice->rec_set=((hw->spdif_recording-1)&0x7)<<0x7;
  }
  else
  {
   debug(DLIB,"rec[open]: incorrect subdevice!\n");
   kx_lock_release(hw,&hw->k_lock, &flags);
   return -9;
  }

 voice->buffer.addr=buffer->addr;
 voice->buffer.size=buffer->size;
 voice->buffer.that=buffer->that;
 voice->buffer.notify=buffer->notify;
 voice->buffer.physical=buffer->physical;
 voice->buffer.instance=buffer->instance;

 // int bytespersample=((usage&VOICE_FLAGS_STEREO?2:1) * (usage&VOICE_FLAGS_16BIT?2:1));

 voice->state=VOICE_STATE_STOPPED;

 voice->rec_pos=0;

 memset((byte *)voice->buffer.addr,0,
     voice->buffer.size);

 switch(voice->buffer.size)
 {
        case 384: voice->rec_size=ADCBS_BUFSIZE_384; break;
        case 448: voice->rec_size=ADCBS_BUFSIZE_448; break;
        case 512: voice->rec_size=ADCBS_BUFSIZE_512; break;
        case 640: voice->rec_size=ADCBS_BUFSIZE_640; break;
        case 768: voice->rec_size=ADCBS_BUFSIZE_768; break;
        case 896: voice->rec_size=ADCBS_BUFSIZE_896; break;
        case 1024: voice->rec_size=ADCBS_BUFSIZE_1024; break;
        case 1280: voice->rec_size=ADCBS_BUFSIZE_1280; break;
        case 1536: voice->rec_size=ADCBS_BUFSIZE_1536; break;
        case 1792: voice->rec_size=ADCBS_BUFSIZE_1792; break;
        case 2048: voice->rec_size=ADCBS_BUFSIZE_2048; break;
        case 2560: voice->rec_size=ADCBS_BUFSIZE_2560; break;
        case 3072: voice->rec_size=ADCBS_BUFSIZE_3072; break;
        case 3584: voice->rec_size=ADCBS_BUFSIZE_3584; break;
        case 4096: voice->rec_size=ADCBS_BUFSIZE_4096; break;
        case 5120: voice->rec_size=ADCBS_BUFSIZE_5120; break;
        case 6144: voice->rec_size=ADCBS_BUFSIZE_6144; break;
        case 7168: voice->rec_size=ADCBS_BUFSIZE_7168; break;
        case 8192: voice->rec_size=ADCBS_BUFSIZE_8192; break;
        case 10240: voice->rec_size=ADCBS_BUFSIZE_10240; break;
        case 12288: voice->rec_size=ADCBS_BUFSIZE_12288; break;
        case 14366: voice->rec_size=ADCBS_BUFSIZE_14366; break;
        case 16384: voice->rec_size=ADCBS_BUFSIZE_16384; break;
        case 20480: voice->rec_size=ADCBS_BUFSIZE_20480; break;
        case 24576: voice->rec_size=ADCBS_BUFSIZE_24576; break;
        case 28672: voice->rec_size=ADCBS_BUFSIZE_28672; break;
        case 32768: voice->rec_size=ADCBS_BUFSIZE_32768; break;
        case 40960: voice->rec_size=ADCBS_BUFSIZE_40960; break;
        case 49152: voice->rec_size=ADCBS_BUFSIZE_49152; break;
        case 57344: voice->rec_size=ADCBS_BUFSIZE_57344; break;
        case 65536: voice->rec_size=ADCBS_BUFSIZE_65536; break;
        default:
        	debug(DLIB,"! incorrect rec buffer size (%d)\n",buffer->size);
                kx_lock_release(hw,&hw->k_lock, &flags);
        	return 4;
 }

 my_memset(&voice->timer,0,sizeof(kx_timer));
 voice->timer.status=TIMER_UNINSTALLED;

 voice->state=VOICE_STATE_STOPPED;

 voice->usage=VOICE_USAGE_PLAYBACK;
 kx_lock_release(hw,&hw->k_lock, &flags);

 switch(rec_subdevice)
 {
  case 0:
      kx_writeptr(hw,ADCBS,0,0);
      kx_writeptr(hw,ADCBA,0,voice->buffer.physical);
      kx_writeptr(hw,ADCSR,0,0);
      break;
  case 1:
      kx_writeptr(hw,SPRC,0,0); // control+address
      kx_writeptr(hw,SPRA,0,voice->buffer.physical);
      break;
 }

 kx_rec_set_notify(hw,rec_subdevice,buffer->notify);

 return 0; // ok
}

// supports MONO/Stereo; 16bit ONLY
KX_API(int,kx_rec_close(kx_hw *hw,int rec_subdevice))
{
// debug(DLIB,"rec_close()\n");
 if((rec_subdevice!=0 && (!hw->is_10k2)) || rec_subdevice>1)
 {
  debug(DLIB,"! rec [close]: incorrect subdevice\n");
  return 6;
 }

 kx_rec_stop(hw,rec_subdevice);

 kx_timer_uninstall(hw,&hw->rec_voicetable[rec_subdevice].timer);

 hw->rec_voicetable[rec_subdevice].usage=VOICE_USAGE_FREE;

 return 0;
}

KX_API(int,kx_rec_start(kx_hw *hw,int rec_subdevice))
{
// debug(DLIB,"rec_start()\n");
 if((rec_subdevice!=0 && (!hw->is_10k2)) || rec_subdevice>1)
 {
  debug(DLIB,"! rec [start]: incorrect subdevice\n");
  return 6;
 }

 kx_rec_voice *voice=&hw->rec_voicetable[rec_subdevice];

 if(voice_usage(voice->usage)==VOICE_USAGE_PLAYBACK)
 {
  memset((byte *)voice->buffer.addr,0,voice->buffer.size);

//  kx_irq_enable(hw, (rec_subdevice==0)?INTE_ADCBUFENABLE:INTE_SPDIF_BUFFUL_ENABLE_K2);

  if(rec_subdevice==0)
   kx_writeptr(hw, ADCSR, 0, voice->rec_set);
  else
   kx_writeptr(hw, SPRC,0,0);

  kx_timer_enable(hw,&voice->timer);
  voice->state=VOICE_STATE_STARTED;

  if(rec_subdevice==0)
    kx_writeptr(hw, ADCBS,0,voice->rec_size);
  else
    kx_writeptr(hw,SPRC,0,voice->rec_set|voice->rec_size);

  // call for the first time
  hw->cb.notify_func(voice->buffer.that,LLA_NOTIFY_TIMER);

  } else return -2;
 return 0;
}

KX_API(int,kx_rec_stop(kx_hw *hw,int rec_subdevice))
{
// debug(DLIB,"rec_stop()\n");
 if((rec_subdevice!=0 && (!hw->is_10k2)) || rec_subdevice>1)
 {
  debug(DLIB,"! rec [stop]: incorrect subdevice\n");
  return 6;
 }

 kx_rec_voice *voice=&hw->rec_voicetable[rec_subdevice];

 if(voice_usage(voice->usage)==VOICE_USAGE_PLAYBACK)
 {
  if(rec_subdevice==0)
    kx_writeptr_multiple(hw,0,
      ADCBS,0,
      ADCSR,0,
      ADCBS,0,
      ADCSR,0,
      REGLIST_END);
  else
    kx_writeptr(hw,SPRC,0,0);

//  kx_irq_disable(hw, (rec_subdevice==0)?INTE_ADCBUFENABLE:INTE_SPDIF_BUFFUL_ENABLE_K2);

  voice->state=VOICE_STATE_STOPPED;
  kx_timer_disable(hw,&voice->timer);

 } else return -1;
 return 0;
}


KX_API(int,kx_rec_get_position(kx_hw *hw,int *pos,int rec_subdevice))
{
 if((rec_subdevice!=0 && (!hw->is_10k2)) || rec_subdevice>1)
 {
  debug(DLIB,"! rec [getpos]: incorrect subdevice\n");
  return 6;
 }

 kx_rec_voice *voice=&hw->rec_voicetable[rec_subdevice];

 dword hw_pos;

 if((voice_usage(voice->usage)==VOICE_USAGE_PLAYBACK)&&
    (voice->state==VOICE_STATE_STARTED))
 {
     if(rec_subdevice==0)       
      hw_pos=kx_readptr(hw, hw->is_10k2?ADCIDX_K2:ADCIDX_K1,0);
     else
      hw_pos=kx_readptr(hw, SPRI, 0);
 }
 else
     hw_pos=0;

 *pos=hw_pos;

 return 0;
}

KX_API(int,kx_rec_set_notify(kx_hw *hw,int num,int interval)) // in ms
{
 if(num<0 || num>=KX_NUMBER_OF_REC_VOICES)
 {
  debug(DERR,"kx_rec_set_notify: invalid voice # [%d]\n",num);
  return -2;
 }

 if(!hw->is_10k2 && num!=0)
 {
  debug(DERR,"kx_rec_set_notify: direct spdif rec not supported for 10k1 # [%d]\n",num);
  return -2;
 }

 kx_rec_voice *voice=&hw->rec_voicetable[num];

 if(voice_usage(voice->usage)==VOICE_USAGE_FREE)
  return -1;

 kx_timer_uninstall(hw,&voice->timer);

 if(interval!=0)
 {
    voice->buffer.notify=interval;
    voice->timer.timer_func=hw->cb.notify_func;
    voice->timer.data=voice->buffer.that;

    __int64 delay;

    if(interval>0)
    {
     delay=hw->card_frequency*interval/1000; // it is in ms
    }
    else
    {
     delay=-interval; // it is in samples
    }
    kx_timer_install(hw,&voice->timer,(dword)delay);
 }

 return 0;
}
