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

KX_API(int,kx_wave_open(kx_hw *hw,kx_voice_buffer *buffer,int usage,int sampling_rate,int device_))
{
 debug(DSTATE,"wave_open:: Sampling rate: %d %s %s\n",sampling_rate,
  usage&VOICE_FLAGS_STEREO?"Stereo":"Mono",
  usage&VOICE_FLAGS_16BIT?"16 bit":"8 bit");

 if(sampling_rate<KX_MIN_RATE || sampling_rate>KX_MAX_RATE)
 {
  debug(DLIB,"wave_open: what a sampling rate? (%d)\n",sampling_rate);
  return -1;
 }

 int num=kx_voice_alloc(hw,usage);
 if(num==-1)
 {
  debug(DLIB,"wave_open: error allocating voice\n");
  return -2;
 }

 kx_voice *voice=&hw->voicetable[num];

 buffer->pageindex=-1;

 voice->buffer.size=buffer->size;
 voice->buffer.physical=buffer->physical;
 voice->buffer.addr=buffer->addr;
 voice->buffer.pageindex=buffer->pageindex;
 voice->buffer.that=buffer->that;
 voice->buffer.notify=buffer->notify;
 voice->buffer.instance=buffer->instance;

#if defined(__APPLE__) && defined(__MACH__) // MacOSX
 voice->buffer.desc=buffer->desc;
#endif

 if(kx_alloc_buffer(hw,num))
 {
  debug(DLIB,"wave_open: error allocating buffer for voice\n");
  kx_voice_free(hw,num);
  return -1;
 }

 int bytespersample=((usage&VOICE_FLAGS_STEREO?2:1) *
 		    (usage&VOICE_FLAGS_16BIT?2:1));
 voice->sampling_rate=sampling_rate;
 voice->bytespersample=bytespersample;

 // calc pitch
 voice->param.initial_pitch = (word) (kx_srToPitch(kx_sr_coeff(hw,sampling_rate)) >> 8);
 voice->param.pitch_target = kx_samplerate_to_linearpitch(kx_sr_coeff(hw,sampling_rate));

 voice->param.startloop = (voice->buffer.pageindex << 12) / bytespersample;
 voice->param.endloop = voice->param.startloop + voice->buffer.size / bytespersample;
 voice->param.start = voice->param.startloop;

 for(int i = 0; i < (voice->usage & VOICE_FLAGS_STEREO ? 2 : 1); i++)
 {
    voice[i].param.send_a = ((i==0)?0xff:0);
    voice[i].param.send_b = (voice->usage&VOICE_FLAGS_STEREO?(i==1?0xff:0):0xff);
    voice[i].param.send_c = 0x0;
    voice[i].param.send_d = 0x0;
    voice[i].param.send_e = 0x0;
    voice[i].param.send_f = 0x0;
    voice[i].param.send_g = 0x0;
    voice[i].param.send_h = 0x0;
 }

 if(device_>ROUTING_LAST)
 {
  debug(DLIB,"!! [routing]: device_ invalid (wave_open): %d\n",device_);
  device_=DEF_WAVE01_ROUTING;
 }
 voice->param.send_routings = hw->cb.def_routings[device_];
 voice->param.send_xroutings = hw->cb.def_xroutings[device_];

 if(hw->is_10k2)
 {
   // 3543: new volume scheme:
   voice->param.initial_cv = 0x8000;
   voice->param.volume_target = 0x8000;

   // fixme 3538c awe guide recommends 0x0
   // for both cur volume & vol target
   // [both controlled by volume_target]
 }
 else
 {
   voice->param.initial_cv = 0xffff;
   voice->param.volume_target = 0xffff;
 }

 voice->state=VOICE_STATE_STOPPED;

 // soundfont parameters (default)
 voice->param.fc_target = 0xffff;
 voice->param.current_fc = 0xffff;

 voice->param.initial_fc = 0xff;
 voice->param.initial_attn = 0x00;

 voice->param.voldelay=0x8000; // FIXME 3538e 0xbfff; // 3538/11.11.2004 0x8000; // VEV
 voice->param.volatk=0x7f; // atkhldv
 voice->param.volhld=0x7f;
 voice->param.voldecay = 0x7f;
 voice->param.volsustain = 0x7f;
 voice->param.volrelease = 0; // not used

 voice->param.moddelay=0x8000; // FIXME 3538e 0xbfff; // 3538/11.11.2004 0x8000; // ENVVAL
 voice->param.modatk=0x0; // FIXME 3538e 0x7f; // ATKHLDM
 voice->param.modhld=0x0; // FIXME 3538e 0x7f;
 voice->param.moddecay=0x7f; // DCYSUSM
 voice->param.modsustain=0x7f;
 voice->param.modrelease=0; // not used
 
 voice->param.filter_amount=0x7f; // FIXME 3538e 0x0; // part of PEFE [3538: was 0x7f]
 voice->param.pitch_amount=0x0; // part of PEFE
 voice->param.lfo1delay=0x8000; // LFOVAL1
 voice->param.lfo2delay=0x8000; // LFOVAL2
 voice->param.modlfofreq=0; // TREMFRQ
 voice->param.modlfo2vol=0;
 voice->param.modlfo2fc=0; // FMMOD
 voice->param.modlfo2pitch=0;
 voice->param.viblfofreq=0; // fm2frq2
 voice->param.viblfo2pitch=0;
 voice->param.filterQ=0;

 voice->param.sample_type=0;
 voice->param.loop_type=1; // continuous

 voice->param.interpolate=0;

 voice->hw_pos=0;
 voice->total_played=0;

 debug(DSTATE,"wave opened (%d) & playback setup...\n",num);
 kx_setup_playback(hw,num);

 my_memset(&hw->voicetable[num].timer,0,sizeof(kx_timer));
 voice->timer.status=TIMER_UNINSTALLED;

 if(!(usage&VOICE_OPEN_NOTIMER))
  kx_wave_set_notify(hw,num,buffer->notify);

 voice->usage=usage;
 if(usage&VOICE_FLAGS_STEREO)
  voice[1].usage=usage;

 return num;
}

KX_API(int,kx_wave_start(kx_hw *hw,int num))
{
 if(num<0 || num>=KX_NUMBER_OF_VOICES)
  return -2;

 if(voice_usage(hw->voicetable[num].usage)==VOICE_USAGE_FREE)
  return -1;

 debug(DSTATE,"wave started (%d)\n",num);

 if(hw->voicetable[num].timer.status!=TIMER_UNINSTALLED)
   kx_timer_enable(hw,&hw->voicetable[num].timer);

 if(hw->voicetable[num].state==VOICE_STATE_STOPPED)
  kx_voice_start(hw,num);
 return 0;
}

KX_API(int,kx_wave_start_multiple(kx_hw *hw,int num,dword low,dword high))
{
 if(num<0 || num>=KX_NUMBER_OF_VOICES)
  return -2;

 if(voice_usage(hw->voicetable[num].usage)==VOICE_USAGE_FREE)
  return -1;

 debug(DSTATE,"multiple wave started (%d)\n",num);

 if(hw->voicetable[num].timer.status!=TIMER_UNINSTALLED)
   kx_timer_enable(hw,&hw->voicetable[num].timer);

 if(hw->voicetable[num].state==VOICE_STATE_STOPPED)
  kx_voice_start_multiple(hw,low,high);
 return 0;
}

KX_API(int,kx_wave_stop(kx_hw *hw,int num))
{
 if(num<0 || num>=KX_NUMBER_OF_VOICES)
  return -2;

 if(voice_usage(hw->voicetable[num].usage)==VOICE_USAGE_FREE)
  return -1;

 debug(DSTATE,"wave stopped (%d)\n",num);

 if(hw->voicetable[num].timer.status!=TIMER_UNINSTALLED)
   kx_timer_disable(hw,&hw->voicetable[num].timer);

 if(hw->voicetable[num].state==VOICE_STATE_STARTED)
   kx_voice_stop(hw,num);
 return 0;
}

KX_API(int,kx_wave_stop_multiple(kx_hw *hw,int num,dword low,dword high))
{
 if(num<0 || num>=KX_NUMBER_OF_VOICES)
  return -2;

 if(voice_usage(hw->voicetable[num].usage)==VOICE_USAGE_FREE)
  return -1;

 debug(DSTATE,"multiple wave stopped (%d = %x %x)\n",num,low,high);

 if(hw->voicetable[num].timer.status!=TIMER_UNINSTALLED)
   kx_timer_disable(hw,&hw->voicetable[num].timer);

 if(hw->voicetable[num].state==VOICE_STATE_STARTED)
   kx_voice_stop_multiple(hw,low,high);
 return 0;
}

KX_API(int,kx_wave_close(kx_hw *hw,int num))
{
 if(num<0 || num>=KX_NUMBER_OF_VOICES)
  return -2;

 if(voice_usage(hw->voicetable[num].usage)==VOICE_USAGE_FREE)
  return -1;

 debug(DSTATE,"wave closed (%d)\n",num);

 kx_wave_stop(hw,num);
 if(!(hw->voicetable[num].usage&VOICE_OPEN_NOTIMER))
  kx_wave_set_notify(hw,num,0);
 kx_voice_free(hw,num);
 kx_free_buffer(hw,num);
 return 0;
}

KX_API(int,kx_wave_set_notify(kx_hw *hw,int num,int interval)) // in ms
{
 if(num<0 || num>=KX_NUMBER_OF_VOICES)
 {
  debug(DERR,"kx_wave_set_notify: invalid voice # [%d]\n",num);
  return -2;
 }

 if(voice_usage(hw->voicetable[num].usage)==VOICE_USAGE_FREE)
  return -1;

 kx_timer_uninstall(hw,&hw->voicetable[num].timer);

 if(interval!=0)
 {
    hw->voicetable[num].buffer.notify=interval;
    hw->voicetable[num].timer.timer_func=hw->cb.notify_func;
    hw->voicetable[num].timer.data=hw->voicetable[num].buffer.that;

    __int64 delay;

    if(interval>0)
    {
     delay=hw->card_frequency*interval/1000; // it is in ms
    }
    else
    {
     delay=-interval; // it is in samples
    }
    kx_timer_install(hw,&hw->voicetable[num].timer,(dword)delay);
 }

 return 0;
}

KX_API(dword,kx_calc_position(kx_hw *hw,int num,dword qkbca)) // position is in samples
{
 	if(qkbca < hw->voicetable[num].param.start)
    {
        if(hw->voicetable[num].bytespersample)
 		  qkbca += hw->voicetable[num].buffer.size / hw->voicetable[num].bytespersample - hw->voicetable[num].param.start;
        else
        {
          qkbca = 0;
          debug(DWDM,"kx_calc_position: invalid bytes per sample! [%d], %x, %x\n",hw->voicetable[num].bytespersample,qkbca,hw->voicetable[num].param.start);
        }
    }
 	else
 		qkbca -= hw->voicetable[num].param.start;

    return qkbca;
}

KX_API(int,kx_wave_get_position(kx_hw *hw,int num,int *pos))
{
 if(num<0 || num>=KX_NUMBER_OF_VOICES)
  return -2;

 if(voice_usage(hw->voicetable[num].usage)==VOICE_USAGE_FREE)
  return -1;

 dword hw_pos;
 dword diff;

 kx_voice *voice=&hw->voicetable[num];

	// There is no actual start yet
/*	if(voice->state!=VOICE_STATE_STARTED) 
	{
		hw_pos = voice->hw_pos;
	}
	 else 
*/	{
		// hw_pos in sample units
        hw_pos = kx_calc_position(hw,num,kx_readptr(hw, QKBCA, num)&QKBCA_CURRADDR_MASK);
		hw_pos *= voice->bytespersample;
	}

	diff = (voice->buffer.size + hw_pos - voice->hw_pos) % voice->buffer.size;

	// unsigned long flags=0;
	// kx_lock_acquire(hw,&hw->k_lock, &flags);
	 voice->total_played += diff;
	 voice->hw_pos = hw_pos;
	// kx_lock_release(hw,&hw->k_lock, &flags);

        *pos=hw_pos;

	return 0;
}

KX_API(int,kx_wave_set_position(kx_hw *hw,int num,int pos))
{
 if(num<0 || num>=KX_NUMBER_OF_VOICES) // > because we DON'T support 'rec' pseudo channel #64
  return -2;

 if(voice_usage(hw->voicetable[num].usage)==VOICE_USAGE_FREE)
  return -1;

 kx_voice *voice=&hw->voicetable[num];

	// There is no actual start yet
/*	if(voice->state!=VOICE_STATE_STARTED)
	{
		voice->hw_pos = pos;
	}
	 else 
*/
	{
		// hw_pos in sample units
		kx_writeptr(hw, QKBCA_CURRADDR, num,(pos/voice->bytespersample)+voice->param.start);

		voice->hw_pos = pos;
	}
	return 0;
}

KX_API(int,kx_wave_set_parameter(kx_hw *hw,int num, int ch, int param,dword value))
{
 if(num<0 || num>=KX_NUMBER_OF_VOICES)
  return -2;

 if(voice_usage(hw->voicetable[num].usage)==VOICE_USAGE_FREE)
  return -1;

 kx_voice *voice=&hw->voicetable[num];
 int i=0;

 switch(param)
 {
  case KX_VOICE_VOLUME:
  	if(ch<0 || ch>1)
  	{
  	 debug(DLIB,"!! invalid ch(%d) in wave_set_param [vol]\n",ch);
  	 return -5;
  	}
  	if((ch==1)&&(!(voice->usage&VOICE_FLAGS_STEREO)))
  	{
  	 // debug(DLIB,"!! invalid ch(1) for mono stream\n");
  	 return -7;
  	}
    // 3543 new code:
  	value=calc_volume(hw,value,(hw->is_10k2?0x8000:0xffff));

  	voice[ch].param.volume_target=value;
  	voice[ch].param.initial_cv=value;
        // fall through

  case KX_VOICE_VOLUME+KX_VOICE_UPDATE:

	if(voice->state==VOICE_STATE_STARTED)
	{
	 //old1=kx_readptr(hw,VTFT,num+ch);
	 //old1&=0xffff;
	 //old1|=(dword)(voice[ch].param.volume_target << 16);
         //kx_writeptr_multiple(hw, num + ch,
	//		    VTFT, old1,
	//		    REGLIST_END);
	  kx_writeptr_multiple(hw,num+ch,
	     VTFT_VOLUME,voice[ch].param.volume_target,
// FIXME 3538e             CVCF_CURRENTVOL,voice[ch].param.initial_cv,
             REGLIST_END);
	}
  	break;

  case KX_VOICE_PAN: 
  	if(ch<0 || ch>1)
  	{
  	 debug(DLIB,"!! invalid ch(-1) in wave_set_param [pan]\n");
  	 return -5;
  	}

  	value=calc_volume(hw,value,0xff); // note: this will be transformed back to logarithms for 10k2

  	if(ch==0)
  	{
  	 if(!(voice->usage & VOICE_FLAGS_STEREO)) // mono
  	  voice->param.send_a=value;
  	 else
  	 {
  	  voice->param.send_a=value;
  	  if(hw->ext_flags&KX_HW_WAVE_PAN)
           voice->param.send_b=0xff-value; // if 'balance'; otherwise  =0;
          else
           voice->param.send_b=0;
  	 }
  	}
  	else
  	{
  	 if(!(voice->usage &VOICE_FLAGS_STEREO)) // mono
  	  voice->param.send_b=value;
  	 else
  	 {
  	  voice[1].param.send_b=value;
  	  if(hw->ext_flags&KX_HW_WAVE_PAN)
           voice[1].param.send_a=0xff-value; // if 'balance'; otherwise  =0;
          else
           voice[1].param.send_a=0;
  	 }
  	}
        // fall through

  case KX_VOICE_PAN+KX_VOICE_UPDATE:
	if(voice->state==VOICE_STATE_STARTED)
	 for(i = 0; i < (voice->usage & VOICE_FLAGS_STEREO ? 2 : 1); i++)
	 {
	  kx_set_fx_amount(hw,3,num,i);
         }

	break;

  case KX_VOICE_PITCH:
  	if((int)value<0)
  	{
  	 debug(DLIB,"!! setting pitch to %d!\n",value);
  	 value=100;
  	}
  	if(value>192000)
  	{
  	 value=192000;
  	}

	voice->param.initial_pitch = (word) (kx_srToPitch(kx_sr_coeff(hw,value)) >> 8);
	voice->param.pitch_target = kx_samplerate_to_linearpitch(kx_sr_coeff(hw,value));
	voice->sampling_rate = value;
        // fall through 

  case KX_VOICE_PITCH+KX_VOICE_UPDATE:
        {
		dword interpolation=QKBCA_INTERPROM_0;

		if(voice->param.initial_pitch!=0xe000 && (hw->ext_flags&KX_HW_WAVE_INTERPROM))
		{
		if(voice->param.initial_pitch<=0xe000+3*4096/12) // 
		 interpolation=QKBCA_INTERPROM_0;
		else
		 if(voice->param.initial_pitch<=0xe000+6*4096/12) // 
		  interpolation=QKBCA_INTERPROM_1;
		 else
		  if(voice->param.initial_pitch<=0xe000+9*4096/12) // 
		    interpolation=QKBCA_INTERPROM_2;
		  else
		   if(voice->param.initial_pitch<=0xe000+12*4096/12) // 
		    interpolation=QKBCA_INTERPROM_3;
		   else
		    if(voice->param.initial_pitch<=0xe000+15*4096/12) // 
		     interpolation=QKBCA_INTERPROM_4;
		    else
		     if(voice->param.initial_pitch<=0xe000+18*4096/12) // 
		      interpolation=QKBCA_INTERPROM_5;
		     else
		      if(voice->param.initial_pitch<=0xe000+21*4096/12) // 
		       interpolation=QKBCA_INTERPROM_6;
		      else
		       interpolation=QKBCA_INTERPROM_7;
		}

	for(i = 0; i < (voice->usage & VOICE_FLAGS_STEREO ? 2 : 1); i++)
	{
		if(voice[0].state==VOICE_STATE_STARTED)
		{
		 // perform real-time interprom update here:
		 if(hw->is_10k2)
                 {
                    dword regptr;
                    unsigned long flags=0;

                    regptr = (((QKBCA) << 16) & PTR_ADDRESS_MASK) | ((num+i) & PTR_CHANNELNUM_MASK);

                    kx_lock_acquire(hw,&hw->hw_lock, &flags);

                    outpd(hw->port + PTR,regptr);
                    dword val=inpd(hw->port+DATA);
                    val=(val&(~QKBCA_INTERPROMMASK))|(interpolation);

                    // outpd(hw->port + DATA,val);
                    outp(hw->port + DATA + 3,(byte)(val>>24));

                    kx_lock_release(hw,&hw->hw_lock, &flags);

                    kx_writeptr_multiple(hw, num+i,
                      PTAB_PITCHTARGET, voice->param.pitch_target,
//                      CPF_CURRENTPITCH, voice->param.pitch_target,
                      // CPF's values are inherited for 'right' channel
//                      IP, voice->param.initial_pitch,
                      REGLIST_END);
                 }
                 else
                 {
                 	// 10k1 doesn't support partial register r/w
        		 // dword cur_addr=(kx_readptr(hw,QKBCA,num+i)&(~QKBCA_INTERPROMMASK));
        		 //kx_writeptr(hw, QKBCA, num + i, interpolation | cur_addr);
        		 kx_writeptr_multiple(hw, num+i,
        		   QKBCA_INTERPROM, (interpolation>>0x19),
                           PTAB_PITCHTARGET, voice->param.pitch_target,
//                           CPF_CURRENTPITCH, voice->param.pitch_target,
                           // CPF's values are inherited for 'right' channel (FIXME? -- CPF?)
//                           IP, voice->param.initial_pitch,
                           REGLIST_END);
                 }
		}
		else
		{
		 // voice is already set-up, but QKBCA should be updated (const)
		 kx_writeptr(hw, QKBCA_INTERPROM, num + i, interpolation>>0x19);
		}
	}
	}
   	break;
  case KX_VOICE_ROUTINGS:
  	voice->param.send_routings=value;
        // fall through 

  case KX_VOICE_ROUTINGS+KX_VOICE_UPDATE:	
	for(i = 0; i < (voice->usage & VOICE_FLAGS_STEREO ? 2 : 1); i++) 
	{
		// NOTE: ANY MODIFICATIONS LINKED TO KX3D/MINIPORTWAVE::Apply
		if(!hw->is_10k2)
		{
		 dword r=0;
		 r=((voice->param.send_routings&0xf)<<16)|
		   ((voice->param.send_routings&0xf00)<<12)|
                   ((voice->param.send_routings&0xf0000)<<8)|
                   ((voice->param.send_routings&0xf000000)<<4);
                 kx_writeptr(hw, FXRT_K1, num + i, r);
		}
		else
		{
         dword unfiltered=0;
         if(voice->param.initial_pitch==0xe000)
          unfiltered=0x80808080;

		 kx_writeptr(hw, FXRT1_K2, num + i, voice->param.send_routings|unfiltered);
		}
	}
	break;
  case KX_VOICE_XROUTINGS:
  	voice->param.send_xroutings=value;
        // fall through 

  case KX_VOICE_XROUTINGS+KX_VOICE_UPDATE:	
	for(i = 0; i < (voice->usage & VOICE_FLAGS_STEREO ? 2 : 1); i++) 
	{
		if(hw->is_10k2)
		{
         dword unfiltered=0;
         if(voice->param.initial_pitch==0xe000)
          unfiltered=0x80808080;

		 kx_writeptr(hw, FXRT2_K2, num + i, voice->param.send_xroutings|unfiltered);
		}
	}
	break;
  case KX_VOICE_SEND_A:
  	voice->param.send_a=value;
        // fall through 

  case KX_VOICE_SEND_A+KX_VOICE_UPDATE:	
  	if(ch!=0) { debug(DERR,"voice_send_a: ch!=0\n"); }
  	kx_set_fx_amount(hw,1,num,ch);
	break;
  case KX_VOICE_SEND_B:
  	voice->param.send_b=value;
        // fall through 

  case KX_VOICE_SEND_B+KX_VOICE_UPDATE:	
  	if(ch!=0) { debug(DERR,"voice_send_b: ch!=0\n"); }
  	kx_set_fx_amount(hw,2,num,ch);
	break;
  case KX_VOICE_SEND_C:
  	voice->param.send_c=value;
        // fall through 

  case KX_VOICE_SEND_C+KX_VOICE_UPDATE:	
  	if(ch!=0) { debug(DERR,"voice_send_c: ch!=0\n"); }
  	kx_set_fx_amount(hw,4,num,ch);
        break;
  case KX_VOICE_SEND_D:	
  	voice->param.send_d=value;
        // fall through 

  case KX_VOICE_SEND_D+KX_VOICE_UPDATE:	
  	if(ch!=0) { debug(DERR,"voice_send_d: ch!=0\n"); }
  	kx_set_fx_amount(hw,8,num,ch);	
        break;

  case KX_VOICE_SEND_E:
  	voice->param.send_e=value;
        // fall through

  case KX_VOICE_SEND_E+KX_VOICE_UPDATE:
  	if(hw->is_10k2)
  	{
  	 if(ch!=0) { debug(DERR,"voice_send_e: ch!=0\n"); }
  	 kx_set_fx_amount(hw,0x10,num,ch);
  	}
  	break;
  case KX_VOICE_SEND_F:
  	voice->param.send_f=value;
        // fall through

  case KX_VOICE_SEND_F+KX_VOICE_UPDATE:
  	if(hw->is_10k2)
  	{
  	 if(ch!=0) { debug(DERR,"voice_send_f: ch!=0\n"); }
  	 kx_set_fx_amount(hw,0x20,num,ch);
  	}
  	break;
  case KX_VOICE_SEND_G:
  	voice->param.send_g=value;
        // fall through

  case KX_VOICE_SEND_G+KX_VOICE_UPDATE:
  	if(hw->is_10k2)
  	{
  	 if(ch!=0) { debug(DERR,"voice_send_g: ch!=0\n"); }
  	 kx_set_fx_amount(hw,0x40,num,ch);
  	}
  	break;
  case KX_VOICE_SEND_H:
  	voice->param.send_h=value;
        // fall through

  case KX_VOICE_SEND_H+KX_VOICE_UPDATE:
  	if(hw->is_10k2)
  	{
  	 if(ch!=0) { debug(DERR,"voice_send_h: ch!=0\n"); }
  	 kx_set_fx_amount(hw,0x80,num,ch);
  	}
  	break;

  default:
   return -4;
 }
 return 0;
}

KX_API(int,kx_wave_setup_b2b(kx_hw *hw,int num,dword new_routings,int high_low,int ab))
{
     dword send_amount_l,vtft_l,send_amount_h,vtft_h;
     if(hw->can_k8_passthru)
     {
      send_amount_l=0x80; //  lowest:  send: 0xc0, vol: 0x8000 -> ((data)>>1)    -> need to >>15 in DSP
      vtft_l=0x8000;         

      send_amount_h=0xff; // send: 0xe0, vol: 0x8000 -> ((data<<1)>>1) -> nop
      vtft_h=0x8000;
     }
     else
     {
      send_amount_l=0x80; //  lowest:  send: 0xc0, vol: 0x8000 -> ((data)>>1)    -> need to >>15 in DSP
      vtft_l=0x8000;

      send_amount_h=0xffffffff; // send: 0xff, vol: 0x1000 -> ((data<<2)>>4) -> need to <<2 in DSP
      vtft_h=0x1000;
     }

    if(ab==0)
    {
     hw->voicetable[num].param.send_a=high_low?send_amount_h:send_amount_l;
     hw->voicetable[num].param.send_b=0;
    }
    else
    {
     hw->voicetable[num].param.send_a=0;
     hw->voicetable[num].param.send_b=high_low?send_amount_h:send_amount_l;
    }
    hw->voicetable[num].param.send_c=0;
    hw->voicetable[num].param.send_d=0;
    hw->voicetable[num].param.send_e=0;
    hw->voicetable[num].param.send_f=0;
    hw->voicetable[num].param.send_g=0;
    hw->voicetable[num].param.send_h=0;

    hw->voicetable[num].param.initial_cv=high_low?vtft_h:vtft_l;
    hw->voicetable[num].param.volume_target=high_low?vtft_h:vtft_l;

    /*
    debug(DWDM,"== b2b configured: voice: %d vtft: %x, send: %x/%x\n",
        num,
    	hw->voicetable[num].param.initial_cv,
        hw->voicetable[num].param.send_a,
        hw->voicetable[num].param.send_b);
    */
    kx_wave_set_parameter(hw,num,0,KX_VOICE_ROUTINGS,new_routings);

    return 0;
}

KX_API(int,kx_wave_get_parameter(kx_hw *hw,int num, int ch,int param,dword *value))
{
 if(num<0 || num>=KX_NUMBER_OF_VOICES)
  return -2;

 if(voice_usage(hw->voicetable[num].usage)==VOICE_USAGE_FREE)
  return -1;

 kx_voice *voice=&hw->voicetable[num];

 switch(param)
 {
  case KX_VOICE_VOLUME:
  	*value=0; // fixme
  	debug(DLIB,"FIXME! no code @get_param::volume\n");
  	break;

  case KX_VOICE_PAN:
  	*value=0; // fixme
  	debug(DLIB,"FIXME! no code @get_param::pan\n");
	break;
  case KX_VOICE_PITCH:
  	*value=voice->sampling_rate;
   	break;
  case KX_VOICE_ROUTINGS:
  	*value=voice->param.send_routings;
  	break;
  case KX_VOICE_XROUTINGS:
  	*value=voice->param.send_xroutings;
  	break;
  default:
   return -4;
 }
 return 0;
}

