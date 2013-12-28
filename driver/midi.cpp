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

KX_API(int,kx_midi_get_volume(kx_hw *hw,int chn,dword *vol))
{
 if(chn<0 || chn>1 || !vol)
  return -1;
 *vol=hw->synth_vol[chn];
 return 0;
}

KX_API(int,kx_midi_set_volume(kx_hw *hw,int chn,dword vol))
{
 if(chn<0 || chn>1)
  return -1;

 if(vol<0xffa00000 && vol!=0)
  vol=0xffa00000;

 hw->synth_vol[chn]=vol;

 if(vol<=0xffa00000/*(dword)(-96*0x10000)*/ && vol!=0) // mute
 {
   hw->synth_vol[chn]=0xffa00000;

   if(chn==0) // fixme: for Left ONLY
   for(int i=0;i<KX_NUMBER_OF_VOICES;i++)
   {
    if(voice_usage(hw->voicetable[i].usage)==VOICE_USAGE_MIDI)
    {
        for(int j=0;j<(hw->voicetable[i].usage & VOICE_FLAGS_STEREO ? 2 : 1);j++)
        {
             kx_writeptr(hw,IFATN_ATTENUATION,i,0xff); // mute
        }
    }
   }
 }
 return 0;
}


// midi message parser
KX_API(int,kx_midi_play_buffer(kx_midi_state *midi,byte *buff,int len))
{
 kx_hw *hw=midi->hw;
 if(!hw)
 {
  return -1;
 }

 if((midi->inited!=0) && (buff!=0) && (len!=0))
 {
  while(len>0)
  {
      switch(buff[0]&0xf0)
      {
        case 0x80: // note_off
            if(len<3)
            {
             debug(DLIB,"!!! Incomplete on data\n");
             return 0;
            }
            kx_midinote_play(midi,buff[0]&0xf,buff[1],0);

            buff+=3; len-=3;
            break;
        case 0x90: // note_on || note_off
            if(len<3)
            {
             debug(DLIB,"!!! Incomplete on/off data\n");
             return 0;
            }
            kx_midinote_play(midi,buff[0]&0xf,buff[1],buff[2]);

            buff+=3; len-=3;
            break;
        case 0xa0: // key pressure
            if(len<3)
            {
             debug(DLIB,"!!! Incomplete keypressure data\n");
             return 0;
            }
            kx_midi_changes(midi,buff[0]&0xf,CHANGE_KEYPRESSURE,buff[1],buff[2]);
            buff+=3; len-=3;
                        break;
                case 0xb0: // control
                    if(len<3)
                    {
                         debug(DLIB,"!!! Incomplete control data\n");
             return 0;
            }
            kx_midi_changes(midi,buff[0]&0xf,CHANGE_CONTROL,buff[1],buff[2]);
            buff+=3; len-=3;
            break;
        case 0xc0: // program change
            if(len<2)
            {
             debug(DLIB,"!!! Incomplete pgch data\n");
             return 0;
            }
            midi->channels[buff[0]&0xf].program=buff[1];
            buff+=2; len-=2;

            debug(DMIDI,"program: %d\n",midi->channels[buff[0]].program);
            break;
        case 0xd0: // ch pressure
            if(len<2)
            {
             debug(DLIB,"!!! Incomplete chpressure data\n");
             return 0;
            }
            kx_midi_changes(midi,buff[0]&0xf,CHANGE_CHANNELPRESSURE,buff[1],0);
            buff+=2; len-=2;
            break;
        case 0xe0: // pitch bend
                    if(len<3)
                    {
                         debug(DLIB,"!!! Incomplete pitch data\n");
             return 0;
            }
            kx_midi_changes(midi,buff[0]&0xf,CHANGE_PITCHBEND,buff[1],buff[2]);
            buff+=3; len-=3;
            break;
        case 0xf0: // sysex etc
            switch(buff[0]&0xf)
            {
             case 0x0: // sysex
                debug(DLIB,"!!! Synth SysEx\n");
                int i;
                i=0;
                while((len>0) && (buff[i]!=0xf7))
                {
                 debug(DNONE," %x ",buff[i]);
                 len--; i++;
                }
                if(len==0 && buff[i]!=0xf7)
                {
                 debug(DERR,"!!! SysEx NOT TERMINATED!\n");
                 return 0;
                }

                // F0 7F 7F 04 01 LL MM F7
                // Where LL is bist 0-7 and MM= bits 7-13 of 14 bit master volume setting.
                if(memcmp(buff,KX_SYSEX_GMGS_VOLUME_INITIAL,
                    KX_SYSEX_GMGS_VOLUME_INITIAL_SIZE)==0)
                {
                   // int master_vol=(buff[5]&0x7f)+((buff[6]&0x7f)<<7);
                   hw->cb.send_message(hw->cb.call_with,i+1,buff);
                }

                debug(DLIB,"\n");
                len--;
                break;
             case 0x1: // mtc quarter
                if(len<2)
                {
                 debug(DLIB,"!!!Synth 0xf1 size<2\n");
                 return 0;
                }
                len-=2;
                buff+=2;
                break;
             case 0x2: // song pos
                if(len<3)
                {
                 debug(DLIB,"!!!Synth 0xf1 size<3\n");
                 return 0;
                }
                len-=3;
                buff+=3;
                break;
             case 0x3: // song select
                if(len<2)
                {
                 debug(DLIB,"!!!Synth 0xf3 size<2\n");
                 return 0;
                }
                len-=2;
                buff+=2;
                break;
             case 0x6: // tune_request?
                debug(DMIDI,"!!!Synth Tune request?\n");
                len--;
                buff++;
                break;
             case 0x8: // clock?
                debug(DMIDI,"!!!Synth Clock!\n");
                len--;
                buff++;
                break;
             case 0xa: // start
                debug(DMIDI,"!!!Synth Start!\n");
                len--;
                buff++;
                break;
             case 0xb: // continue
                debug(DMIDI,"!!!Synth Continue!\n");
                len--;
                buff++;
                break;
             case 0xc: // stop
                debug(DMIDI,"!!!Synth Stop!\n");
                len--;
                buff++;
                break;
             case 0xe: // sensing
                debug(DMIDI,"!!!Synth Sensing!\n");
                len--;
                buff++;
                break;
             case 0xf: // reset
                debug(DMIDI,"!!!Synth Reset!\n");
                len--;
                buff++;
                break;
             default:
                debug(DLIB,"!!!Synth Unknown Midi SysEX extension (4,5,9,d): %x\n",buff[0]&0xf);
                len--;
                buff++;
                return 0;
            }
            break;
        default:
            debug(DLIB,"!!! Unknown MIDI header [%x]\n",buff[0]);
            return 0;
      }
  }
  if(len<0)
  {
   debug(DLIB,"!!! midi: strange: final len=%d\n",len);
  }
 }
 return 0;
}

KX_API(int,kx_midi_stop(kx_midi_state *midi))
{
 kx_hw *hw=midi->hw;

 if(hw)
 {
    if(midi->inited)
    {
     for(int i=0;i<MAX_MIDI_CHANNELS;i++)
     {
      kx_midi_changes(midi,i,CHANGE_CONTROL,64,0); // sustain off
      kx_midi_changes(midi,i,CHANGE_CONTROL,66,0); // sostenuto off
      kx_midi_changes(midi,i,CHANGE_CONTROL,69,0); // hold2 off
      kx_midi_changes(midi,i,CHANGE_CONTROL,123,0); // all notes off
     }
    }
 }
 else
 {
  return -1;
 }
 return 0;
}

static void kx_reset_all_controllers(kx_midi_state *midi,int chn,int ales)
{
 kx_hw *hw=midi->hw;

 if(!midi->inited)
  return;

 if(!hw)
  return;

 // per specification: [GM Midi system Level 1 Jul 1998]
 kx_midi_changes(midi,chn,CHANGE_CONTROL,1,0); // modulation
 kx_midi_changes(midi,chn,CHANGE_CONTROL,11,127); // expression
 kx_midi_changes(midi,chn,CHANGE_CONTROL,64,0); // sustain off
 kx_midi_changes(midi,chn,CHANGE_CONTROL,66,0); // sostenuto off
 kx_midi_changes(midi,chn,CHANGE_CONTROL,67,0); // soft pedal off
 kx_midi_changes(midi,chn,CHANGE_CONTROL,69,0); // hold2 off
 kx_midi_changes(midi,chn,CHANGE_PITCHBEND,0x0,0x40); // pitch: 0x40<<7=0x2000 

 kx_midi_changes(midi,chn,CHANGE_CONTROL,2,0); // breath
 kx_midi_changes(midi,chn,CHANGE_CONTROL,4,0); // foot
 kx_midi_changes(midi,chn,CHANGE_CONTROL,5,0); // portamento time
 kx_midi_changes(midi,chn,CHANGE_CONTROL,68,0); // legato
 kx_midi_changes(midi,chn,CHANGE_CONTROL,65,0); // portamento_on/off

 kx_midi_changes(midi,chn,CHANGE_CONTROL,71,64); // sound resonance
 kx_midi_changes(midi,chn,CHANGE_CONTROL,72,64); // sound release
 kx_midi_changes(midi,chn,CHANGE_CONTROL,73,64); // sound attact
 kx_midi_changes(midi,chn,CHANGE_CONTROL,74,64); // sound brightness

 kx_midi_changes(midi,chn,CHANGE_CONTROL,84,0); // portamento control

 kx_midi_changes(midi,chn,CHANGE_CONTROL,16,0); // gp1
 kx_midi_changes(midi,chn,CHANGE_CONTROL,17,0); // gp2
 kx_midi_changes(midi,chn,CHANGE_CONTROL,18,0); // gp3
 kx_midi_changes(midi,chn,CHANGE_CONTROL,19,0); // gp4

 midi->channels[chn].data_entry_lsb=0x0;
 midi->channels[chn].nrpn_lsb=0x0;
 midi->channels[chn].rpn_lsb=0x0;
 midi->channels[chn].data_entry_msb=0x0;
 midi->channels[chn].nrpn_msb=0x0;
 midi->channels[chn].rpn_msb=0x0;
 midi->channels[chn].multiplier=1;
 midi->channels[chn].last_rpn=-1;

 kx_midi_changes(midi,chn,CHANGE_CHANNELPRESSURE,0,0);

 // per specs: DO NOT reset
 if(ales)
 {
  kx_midi_changes(midi,chn,CHANGE_CONTROL,7,100); // vol
  kx_midi_changes(midi,chn,CHANGE_CONTROL,10,64); // pan
  kx_midi_changes(midi,chn,CHANGE_CONTROL,8,64); // balance

  kx_midi_changes(midi,chn,CHANGE_CONTROL,91,0); // reverb 
  kx_midi_changes(midi,chn,CHANGE_CONTROL,93,0); // chorus 
  kx_midi_changes(midi,chn,CHANGE_CONTROL,92,0); // tremulo depth
  kx_midi_changes(midi,chn,CHANGE_CONTROL,94,0); // celeste depth
  kx_midi_changes(midi,chn,CHANGE_CONTROL,95,0); // phaser depth

  kx_midi_changes(midi,chn,CHANGE_CONTROL,0,0); // bank lsb
  kx_midi_changes(midi,chn,CHANGE_CONTROL,32,0); // bank msb
 }

 // RPN
 midi->channels[chn].pb_sensivity=2048; // 2 semitones
 midi->channels[chn].master_fine_tuning=0x2000;
 midi->channels[chn].master_coarse_tuning=0x40;

 // NRPN
 int j;
 for(j=0;j<AWE_PARAMETERS;j++)
   midi->channels[chn].nrpn_awe_data[j]=AWE_UNUSED;
 
 for(j=0;j<SF_PARAMETERS;j++)
   midi->channels[chn].nrpn_sf_data[j]=0;

 midi->drum_channel=hw->drum_channel;
}

KX_API(int,kx_midinote_play(kx_midi_state *midi,int chn,int note,int vel))
{
 kx_hw *hw=midi->hw;

 if(vel==0) // note off
 {
   debug(DMIDI,"off [%d %d] chn=%d\n",note,vel,chn);

   for(int n=0;n<KX_NUMBER_OF_VOICES;n++)
   {
     if((voice_usage(hw->voicetable[n].usage)==VOICE_USAGE_MIDI) &&
         (hw->voicetable[n].channel==chn) &&
         (hw->voicetable[n].note==note) &&
         (hw->voicetable[n].synth_id==(uintptr_t)midi)
         )
     {
      if(!midi->channels[chn].sustain)
      {
       if(!(hw->voicetable[n].usage&VOICE_SYNTH_SOSTENUTO))
         kx_synth_release(midi,n);
       else
         hw->voicetable[n].usage|=VOICE_SYNTH_SOSTENUTO_FINISHED;
       // if smth is added here -> add to 'case 64: //sustain'
      }
      else
      {
       hw->voicetable[n].usage|=VOICE_SYNTH_SUSTAINED;
       if(hw->voicetable[n].usage&VOICE_SYNTH_SOSTENUTO)
        hw->voicetable[n].usage|=VOICE_SYNTH_SOSTENUTO_FINISHED;
      }
     }
   }
 }
 else // note on
 {
        kx_synth_start(midi,chn,note,vel);
 }
 return 0;
}

int process_rpns(kx_midi_state *midi,int chn);
int process_rpns(kx_midi_state *midi,int chn) // 0: NRPN; 1: RPN
{
 kx_hw *hw=midi->hw;
 if(midi->channels[chn].last_rpn==0) // RPNS
 {
     switch(midi->channels[chn].rpn_msb)
     {
        case 0:
           switch(midi->channels[chn].rpn_lsb)
           {
            case 0: // pitch bend range (sensivity)
                midi->channels[chn].pb_sensivity=1024*midi->channels[chn].data_entry_msb+(1024*midi->channels[chn].data_entry_lsb)/100;
                        kx_synth_changes(midi,chn,-1,CHANGE_PITCHBEND);
                        // debug(DLIB,"PB sense: %d\n",midi->channels[chn].pb_sensivity);
                break;
            case 1: // master fine tuning
                midi->channels[chn].master_fine_tuning=((midi->channels[chn].data_entry_msb&0x7f)<<7)+(midi->channels[chn].data_entry_lsb&0x7f);
                        kx_synth_changes(midi,chn,-1,CHANGE_PITCHBEND);
                break;
            case 2: // master coarse tuning
                        midi->channels[chn].master_coarse_tuning=(midi->channels[chn].data_entry_msb&0x7f); // w/o data_lsb
                        kx_synth_changes(midi,chn,-1,CHANGE_PITCHBEND);
                break;
            case 0x7f: // RPN reset
                midi->channels[chn].pb_sensivity=2048;
                midi->channels[chn].master_fine_tuning=0x2000;
                midi->channels[chn].master_coarse_tuning=0x40;
                        kx_synth_changes(midi,chn,-1,CHANGE_PITCHBEND);
                break;
                // GM2: Modulation Depth Range (Vibrato Depth Range)
            default:
                  debug(DNRPN,"Unknown RPN [%x.%x] --> %x.%x\n",
                   midi->channels[chn].rpn_msb,midi->channels[chn].rpn_lsb,
                   midi->channels[chn].data_entry_msb,midi->channels[chn].data_entry_lsb);
           } // switch lsb
           break;
        default:
           debug(DNRPN,"Unknown RPN [%x.%x] --> %x.%x\n",
            midi->channels[chn].rpn_msb,midi->channels[chn].rpn_lsb,
            midi->channels[chn].data_entry_msb,midi->channels[chn].data_entry_lsb);
     } // switch msb
 }
 else // NRPNs
 if(midi->channels[chn].last_rpn==1) // NRPN
 {
     switch(midi->channels[chn].nrpn_msb)
     {
        case 1: // GS/XG
           switch(midi->channels[chn].nrpn_lsb)
           {
            case 8: // vibrato rate
            case 9: // vibrato depth
            case 10: // vibrato delay
            case 32: // fcutoff freq
            case 33: // filter resonance
            case 99: // attack time
            case 100: // decay time
            case 102: // release time
                /*FIXME*/;
           }
           break;
        case 120: // Emu/Creative NRPN
        case 127:
           switch(midi->channels[chn].nrpn_lsb)
           {
            case 100: midi->channels[chn].multiplier+=100; return 0;
                case 101: midi->channels[chn].multiplier+=1000; return 0;
                case 102: midi->channels[chn].multiplier+=10000; return 0;
                default:
                  {
                   int is_nrpn_sf=(midi->channels[chn].nrpn_msb==120)?1:0;

                   int nrpn=midi->channels[chn].nrpn_lsb&0x7f;

                   if(is_nrpn_sf)
                    nrpn+=midi->channels[chn].multiplier;

                   midi->channels[chn].multiplier=0;

                   if(nrpn<0)
                    return 0;

                   if((!is_nrpn_sf) && (nrpn>=AWE_PARAMETERS)) // ignore
                    return 0;
                   if((is_nrpn_sf) && (nrpn>=SF_PARAMETERS)) // ignore
                    return 0; // FIXME: implement when 'generators' are done

                   int data=((midi->channels[chn].data_entry_msb&0x7f)<<7)+(midi->channels[chn].data_entry_lsb&0x7f);
                   data-=0x2000;

                   debug(DNRPN,"nRPN: %d %d [%d]\n",nrpn,data,is_nrpn_sf);

                   if(is_nrpn_sf)
                   {
                     // multiply by 2 if range > +/-8192
                     switch(nrpn)
                     {
                      case 5:
                      case 6:
                      case 7:
                      case 8:
                      case 10:
                      case 11:
                      case 21:
                      case 22:
                      case 23:
                      case 24:
                      case 25:
                      case 26:
                      case 27:
                      case 28:
                      case 30:
                      case 33:
                      case 34:
                      case 35:
                      case 36:
                      case 38:
                       data=data*2;
                     }
                     midi->channels[chn].nrpn_sf_data[nrpn]=data; // fixme: multiplier
                   }
                   else
                     midi->channels[chn].nrpn_awe_data[nrpn]=data;

                   // realtime updates
                   if(!is_nrpn_sf)
                   {
                    switch(nrpn)
                    {
                     case 1: // lfo1(mod) freq -- 0..127; 0.084Hz
                        // (unsigned short)((val * 2) & 0xff);
                        kx_synth_changes(midi,chn,-1,CHANGE_MODULATION);
                        break;
                     case 3: // lfo2(vol) freq
                        // (unsigned short)((val * 2) & 0xff);
                        kx_synth_changes(midi,chn,-1,CHANGE_MODULATION);
                        break;
                     case 16: // ip -- -8192..8191; cents
                        // (short)(val * 4096 / 1200);
                        kx_synth_changes(midi,chn,-1,CHANGE_PITCHBEND);
                        break;
                     case 17: // lfo1 to pitch -- -127..127 9.375 cents
                        // (unsigned short)(val & 0xff);
                        kx_synth_changes(midi,chn,-1,CHANGE_MODULATION);
                        break;
                     case 18: // lfo2 to pitch -- -127..127 9.375 cents
                      // (unsigned short)(val & 0xff);
                        kx_synth_changes(midi,chn,-1,CHANGE_MODULATION);
                        break;

                     case 20: // lfo1 to vol -- 0..127; 0.1875 dB
                      // (unsigned short)((val * 2) & 0xff);
                        kx_synth_changes(midi,chn,-1,CHANGE_MODULATION);
                        break;

                     case 21: // init. cutoff -- 0..127; 62 Hz
                      // (unsigned short)((val * 2) & 0xff);
                        kx_synth_changes(midi,chn,-1,CHANGE_SOFT_PEDAL);
                        break;

                     case 23: // lfo1 to fc (cutoff) -- -64..63; 56.25 cents
                      // (unsigned short)((val * 2) & 0xff);
                        kx_synth_changes(midi,chn,-1,CHANGE_MODULATION);
                        break;
                    }
                  }
                  else // realtime sf2.1 NRPNs
                  {
                   switch(nrpn)
                   {
                    case 15: // chorus
                     kx_synth_changes(midi,chn,-1,CHANGE_CHORUS);
                     break;
                    case 16: // reverb
                     kx_synth_changes(midi,chn,-1,CHANGE_REVERB);
                     break;
                    case 17: // pan
                     kx_synth_changes(midi,chn,-1,CHANGE_PAN);
                     break;

                    case 5: // modlfo2pitch
                    case 10: // modlfo2fc
                    case 6: // viblfo2pitch
                    case 24: // freqviblfo
                    case 22: // freqmodlfo
                    case 13: // modlfo2vol
                     kx_synth_changes(midi,chn,-1,CHANGE_MODULATION);
                     break;

                    case 7: // modenv2pitch
                    case 11: // modenv2fc
                     kx_synth_changes(midi,chn,-1,CHANGE_PEFE);
                     break;

                    case 9: // filter Q
                     kx_synth_changes(midi,chn,-1,CHANGE_FILTERQ);
                     break;

                    case 8: // init. fc
                     kx_synth_changes(midi,chn,-1,CHANGE_SOFT_PEDAL);
                     break;
                    case 30: // releasemodenv
                    case 38: // releasevolenv
                     kx_synth_changes(midi,chn,-1,CHANGE_HOLD2);
                     break;

                    case 21: // delaymodlfo
                    case 23: // delayviblfo

                    case 25: // delaymodenv
                    case 33: // delayvolenv

                    case 26: // attackmodenv
                    case 27: // holdmodenv
                    case 31: // keynum2modenvhold

                    case 28: // decaymodenv
                    case 29: // sustainmodenv
                    case 32: // keynum2modenvdecay

                    case 34: // attackvolenv
                    case 35: // holdvolenv
                    case 36: // decayvolenv
                    case 37: // sustainvolenv

                    case 39: // keynum2volenvhold
                    case 40: // keynum2volenvdecay
                         break; // NOTHING FIXME !!!

                    case 48: // init. attn
                     kx_synth_changes(midi,chn,-1,CHANGE_VOLUME);
                     break;

                    case 51: // coarse tune
                    case 52: // fine tune
             kx_synth_changes(midi,chn,-1,CHANGE_PITCHBEND); // FIXME: not realtime yet
                     break;
                   }
                  } // not sf21
               } // default lsb (not >100)
           } // switch lsb
           break;
        default: // msb
           debug(DNRPN,"Unknown NRPN [%x.%x] --> %x.%x\n",
            midi->channels[chn].nrpn_msb,midi->channels[chn].nrpn_lsb,
            midi->channels[chn].data_entry_msb,midi->channels[chn].data_entry_lsb);
     } // switch msb
 } // last rpn ==1
 return 0;
}

KX_API(int,kx_midi_changes(kx_midi_state *midi,int chn,int what,int note,int how))
{
 int n=0;
 kx_hw *hw=midi->hw;

 switch(what)
 {
  case CHANGE_KEYPRESSURE:
    for(n=0;n<KX_NUMBER_OF_VOICES;n++)
    {
         if((voice_usage(hw->voicetable[n].usage)==VOICE_USAGE_MIDI) &&
               (hw->voicetable[n].channel==chn) &&
               (hw->voicetable[n].note==note) &&
               (hw->voicetable[n].synth_id==(uintptr_t)midi))
         {
           hw->voicetable[n].vel=(byte)how;
         }
        }
        kx_synth_changes(midi,chn,note,CHANGE_VOLUME);
    break;

  case CHANGE_CONTROL:
    switch(note) // = controller
    {
     case 0: // bank lsb
        midi->channels[chn].bank_lsb=how&0x7f;
        break;
     case 32: // bank msb
        midi->channels[chn].bank_msb=how&0x7f;
        break;
     case 1: // modulation depth
            midi->channels[chn].modulation=((byte)how&0x7f);
            kx_synth_changes(midi,chn,-1,CHANGE_MODULATION);
            break;
     case 7: // vol
            midi->channels[chn].vol=((byte)how&0x7f);
            kx_synth_changes(midi,chn,-1,CHANGE_VOLUME);
            break;
     case 10: // pan
            midi->channels[chn].pan=((byte)how&0x7f);
            kx_synth_changes(midi,chn,-1,CHANGE_PAN);
            break;
     case 11: // expression
            midi->channels[chn].expression=(byte)how;
            kx_synth_changes(midi,chn,-1,CHANGE_VOLUME);
            break;
     case 64: // sustain -- hold
        if(midi->channels[chn].sustain && (how<64)) // release
        {
                  for(n=0;n<KX_NUMBER_OF_VOICES;n++)
                  {
                     if((voice_usage(hw->voicetable[n].usage)==VOICE_USAGE_MIDI) &&
                        (hw->voicetable[n].usage&VOICE_SYNTH_SUSTAINED) &&
                           (hw->voicetable[n].channel==chn) &&
                           (hw->voicetable[n].synth_id==(uintptr_t)midi))
                     {
            kx_synth_release(midi,n);
             }  
          }
          midi->channels[chn].sustain=0;
        }
        else
        {
                if((!midi->channels[chn].sustain) && (how>=64)) // sustain
                {
                 midi->channels[chn].sustain=1;
                }
                }
            break;
         case 66: // sostenuto
            if(midi->channels[chn].sostenuto && (how<64)) // release
            {
                    for(n=0;n<KX_NUMBER_OF_VOICES;n++)
                                 if((voice_usage(hw->voicetable[n].usage)==VOICE_USAGE_MIDI) &&
                                       (hw->voicetable[n].channel==chn) &&
                                       (hw->voicetable[n].usage&VOICE_SYNTH_SOSTENUTO) &&
                                       (hw->voicetable[n].synth_id==(uintptr_t)midi))
                                 {
                                    hw->voicetable[n].usage&=~VOICE_SYNTH_SOSTENUTO;
                                    if(hw->voicetable[n].usage&VOICE_SYNTH_SOSTENUTO_FINISHED)
                                      kx_synth_release(midi,n);
                                 }
                                 midi->channels[chn].sostenuto=0;
            }
            else
            {
                if((!midi->channels[chn].sostenuto) && (how>=64)) // sostenuto
                {
                    // mark all current notes as 'sostenuto'
                    for(n=0;n<KX_NUMBER_OF_VOICES;n++)
                                 if((voice_usage(hw->voicetable[n].usage)==VOICE_USAGE_MIDI) &&
                                       (hw->voicetable[n].channel==chn) &&
                                       (hw->voicetable[n].synth_id==(uintptr_t)midi))
                                 {
                                    hw->voicetable[n].usage|=VOICE_SYNTH_SOSTENUTO;
                             }  
                                midi->channels[chn].sostenuto=1;
                }
            }
            break;
         case 67: // soft pedal
            if((midi->channels[chn].soft_pedal==0) && (how>=64)) // turn on
            {
             midi->channels[chn].soft_pedal=1;
                 kx_synth_changes(midi,chn,-1,CHANGE_SOFT_PEDAL);
                }
                else
                 if((midi->channels[chn].soft_pedal==1) && (how<=63)) // turn off
                {
             midi->channels[chn].soft_pedal=0;
                 kx_synth_changes(midi,chn,-1,CHANGE_SOFT_PEDAL);
                }
        break;
         case 68: // legato: remove 'attack' phase
            if((midi->channels[chn].legato==0) && (how>=64)) // turn on
            {
             midi->channels[chn].legato=1;
                 // not real-time
                }
                else
                 if((midi->channels[chn].legato==1) && (how<=63)) // turn off
                {
             midi->channels[chn].legato=0;
                 // not real-time
                }
        break;
     case 69: // Hold2 pedal
        if((midi->channels[chn].hold_2==0) && (how>=64)) // turn on
            {
             midi->channels[chn].hold_2=1;
                 kx_synth_changes(midi,chn,-1,CHANGE_HOLD2);
                }
                else
                 if((midi->channels[chn].hold_2==1) && (how<=63)) // turn off
                {
             midi->channels[chn].hold_2=0;
                 kx_synth_changes(midi,chn,-1,CHANGE_HOLD2);
                }
        break;          
         case 71: // resonance
            midi->channels[chn].resonance=(byte)how;
            kx_synth_changes(midi,chn,-1,CHANGE_FILTERQ); // via filterQ
            break;
         case 72: // release time
            midi->channels[chn].sound_release=(byte)how;
            kx_synth_changes(midi,chn,-1,CHANGE_HOLD2); // via release_time / hold2
            break;
         case 73: // attack time
            midi->channels[chn].sound_attack=(byte)how;
            // not real-time parameter
            break;
         case 74: // brightness = filter cutoff
            midi->channels[chn].sound_brightness=(byte)how;
            kx_synth_changes(midi,chn,-1,CHANGE_SOFT_PEDAL);
            break;
     case 91: // reverb
            midi->channels[chn].reverb=(byte)how;
            kx_synth_changes(midi,chn,-1,CHANGE_REVERB);
            break;
     case 93: // chorus
            midi->channels[chn].chorus=(byte)how;
                kx_synth_changes(midi,chn,-1,CHANGE_CHORUS);
            break;
     case 120: // all sounds off
        for(n=0;n<KX_NUMBER_OF_VOICES;n++)
        {
         if(
           (
            (voice_usage(hw->voicetable[n].usage)==VOICE_USAGE_MIDI) ||
                    (voice_usage(hw->voicetable[n].usage)==VOICE_USAGE_RELEASING)
                   ) &&
            (hw->voicetable[n].synth_id==(uintptr_t)midi)
             &&
             (hw->voicetable[n].channel==chn))
         {
           kx_synth_term(midi,n);
         }
        }
        break;
     case 121: // reset all ctrls
        kx_reset_all_controllers(midi,chn,0);
        break;
     case 122: // local keyb on
        break;
     case 123: // all notes off
        for(n=0;n<KX_NUMBER_OF_VOICES;n++)
        {
         if((voice_usage(hw->voicetable[n].usage)==VOICE_USAGE_MIDI) &&
            (hw->voicetable[n].synth_id==(uintptr_t)midi) &&
            (hw->voicetable[n].channel==chn)
           )
         {
           kx_synth_release(midi,n);
         }
        }
        break;
     case 16:
            midi->channels[chn].gp1=(byte)how;
            kx_synth_changes(midi,chn,-1,CHANGE_GP1);
            break;
     case 17:
            midi->channels[chn].gp2=(byte)how;
            kx_synth_changes(midi,chn,-1,CHANGE_GP2);
            break;
     case 18:
            midi->channels[chn].gp3=(byte)how;
            kx_synth_changes(midi,chn,-1,CHANGE_GP3);
            break;
     case 19:
            midi->channels[chn].gp4=(byte)how;
            kx_synth_changes(midi,chn,-1,CHANGE_GP4);
            break;
// ----------------
     case 2: // breath
            midi->channels[chn].breath=(byte)how;
            // kx_synth_changes(midi,chn,-1,CHANGE_);
            break;
     case 4: // foot
            midi->channels[chn].foot=(byte)how;
            // kx_synth_changes(midi,chn,-1,CHANGE_);
            break;
         case 5: // portamento time
            midi->channels[chn].portamento_time=(byte)how;
            // kx_synth_changes(midi,chn,-1,CHANGE_);
            break;
     case 8:  // balance
            midi->channels[chn].balance=(byte)how;
            kx_synth_changes(midi,chn,-1,CHANGE_PAN);
            break;
         case 65: // portamento on/off
            if((midi->channels[chn].portamento_onoff==0) && (how>=64)) // turn on
            {
             midi->channels[chn].portamento_onoff=1;
                 // kx_synth_changes(midi,chn,-1,CHANGE_);
                }
                else
                 if((midi->channels[chn].portamento_onoff==1) && (how<=63)) // turn off
                {
             midi->channels[chn].portamento_onoff=0;
                 // kx_synth_changes(midi,chn,-1,CHANGE_);
                }
        break;

         case 84: // portamento control
            midi->channels[chn].portamento_ctrl=(byte)how;
            // kx_synth_changes(midi,chn,-1,CHANGE_);
            break;
         case 92: // tremolo level
            midi->channels[chn].tremulo_level=(byte)how;
            // kx_synth_changes(midi,chn,-1,CHANGE_);
            break;
         case 94: // variation send level / celeste level
            midi->channels[chn].celeste_level=(byte)how;
            // kx_synth_changes(midi,chn,-1,CHANGE_);
            break;
         case 95: // phaser level
            midi->channels[chn].phaser_level=(byte)how;
            // kx_synth_changes(midi,chn,-1,CHANGE_);
            break;

         case 12: // effect control 1 [coarse]
         case 13: // effect control 2 [coarse]
         case 70: // variation
         case 75: // sound control 6 decay time
         case 76: // sound control 7 vibrato rate (freq.)
         case 77: // sound control 8 vibrato depth
         case 78: // sound control 9 vibrato delay
         case 79: // sound control 10
         default:
        break;

     case 98: // NRPN lsb fine
        midi->channels[chn].nrpn_lsb=how&0x7f;
                midi->channels[chn].last_rpn=1;
        break;
     case 99: // NRPN msb coarse
        midi->channels[chn].nrpn_msb=how&0x7f;
                midi->channels[chn].last_rpn=1;
        break;
     case 100: // RPN lsb fine
        midi->channels[chn].rpn_lsb=how&0x7f;
        midi->channels[chn].last_rpn=0;
        break;
     case 101: // RPN msb coarse
        midi->channels[chn].rpn_msb=how&0x7f;
                midi->channels[chn].last_rpn=0;
        break;
     case 38: // data entry lsb fine
        midi->channels[chn].data_entry_lsb=how&0x7f;
        if(midi->channels[chn].nrpn_msb!=120) // not SF
         process_rpns(midi,chn);
        break;
     case 6:  // data entry msb coarse
        midi->channels[chn].data_entry_msb=how&0x7f;
        process_rpns(midi,chn); // both, for AWE & SF
        break;
     case 96: // data increment
        if(midi->channels[chn].data_entry_lsb==0x7f)
        {
         midi->channels[chn].data_entry_lsb=0;
                 midi->channels[chn].data_entry_msb++;
        }
         else
                 midi->channels[chn].data_entry_lsb++;
                process_rpns(midi,chn);
                break;
     case 97: // data decerement
        if(midi->channels[chn].data_entry_lsb==0x0)
        {
         midi->channels[chn].data_entry_lsb=0x7f;
                 midi->channels[chn].data_entry_msb--;
        }
         else
                 midi->channels[chn].data_entry_lsb--;
                process_rpns(midi,chn);
                break;

     case 124: // omni off
     case 125: // omni on
     case 126: // mono mode
     case 127: // poly mode
        break;
    }
    break;
  case CHANGE_PITCHBEND:
    midi->channels[chn].pitch=(((word)how)<<7)+(word)note;
    kx_synth_changes(midi,chn,-1,CHANGE_PITCHBEND);
    break;
  case CHANGE_CHANNELPRESSURE:
    midi->channels[chn].chpressure=(byte)note;
    kx_synth_changes(midi,chn,-1,CHANGE_MODULATION);
    break;
  default:
   debug(DLIB,"!!! Unknown what in changes(%d)!\n",what);
   break;
 }
 return 0;
}

KX_API(int,kx_midi_init(kx_hw *hw,kx_midi_state *midi,int synth_))
{
 memset(midi,0,sizeof(kx_midi_state));

 midi->hw=hw;
 midi->inited=1; 
 midi->synth_num=synth_;

 debug(DLIB,"Midi initialized\n");

 for(int i=0;i<16;i++)
 {
  kx_reset_all_controllers(midi,i,1);
  midi->channels[i].program=0x0;
 }

 if(hw->midi[synth_&1]==NULL)
  hw->midi[synth_&1]=midi;
 return 0;
}

KX_API(int,kx_midi_close(kx_midi_state *midi))
{
 if(!midi)
  return -1;
 kx_hw *hw=midi->hw;
 if(!hw)
  return -2;

 kx_midi_stop(midi);

 midi->hw=NULL;
 midi->inited=0;

 if(hw->midi[midi->synth_num&1]==midi)
  hw->midi[midi->synth_num&1]=NULL;

 debug(DLIB,"Midi closed\n");
 return 0;
}
