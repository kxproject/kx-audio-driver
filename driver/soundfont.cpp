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

#define MAX_SOUNDFONTS	1024

KX_API(int,kx_load_soundfont(kx_hw *hw,kx_sound_font *in))
{
 if(!in)
 {
  debug(DERR,"kX sf: NULL argument for load sf\n");
  return -4;
 }

 int id=0;
 kx_sound_font *sf=NULL;

 /*
 // really outdated piece of code?..
 if((in->header.sfman_id&KX_VIENNA_BANK_MASK)==KX_VIENNA_BANK_MAGIC) // vienna bank
 {
     id=in->header.sfman_id;
     int param[2]; param[0]=0; param[1]=0;
     kx_unload_soundfont(hw,id,param); // do not free the 'sample_data' structure

     sample_data=(short *)param[0];

     // re-size if necessary
     if(sample_data)
      if(in->header.sample_len!=param[1])
      {
       (hw->cb.free_func)(hw->cb.call_with,sample_data);
       sample_data=0;
      }
 }
 */

 (hw->cb.malloc_func)(hw->cb.call_with,in->size-in->header.sample_len-4,(void **)&sf,KX_NONPAGED);
 if(sf==NULL)
 {
   debug(DLIB,"kX: sf: Not enough memory for presets (%d needed)\n",in->size-in->header.sample_len-4);
   return -1;
 }

 // alloc cached memory (soundfont stuff is accessed by the synth; soundfont audio data is not modified)
 if((hw->cb.lmem_alloc_func)(hw->cb.call_with,in->header.sample_len+4,(void **)&sf->sample_data,KX_CACHED)!=0)
 {
  (hw->cb.free_func)(hw->cb.call_with,sf);
  debug(DLIB,"kX: sf: Not enough memory for samples (%d needed)\n",in->header.sample_len);
  return -1;
 }
 memset(hw->cb.lmem_get_addr_func(hw->cb.call_with,(void **)&sf->sample_data,0,NULL),0,in->header.sample_len);

 unsigned long flags=0;
 kx_lock_acquire(hw,&hw->sf_lock, &flags);

 // find free 'id'
 // -- legacy comment: [only if not vienna bank]
 if(id==0)
  for(int i=1;i<MAX_SOUNDFONTS;i++)
  {
         struct list *item;
         id=i;
         for_each_list_entry(item, &hw->sf)
         {
				kx_sound_font *sf_;
                sf_ = list_item(item, kx_sound_font, list);
                if(sf_)
                 if(sf_->id==id)
                  id=0;
         }
         if(id!=0) // found
          break;
  }

 if(id==0) // not found
 {
  kx_lock_release(hw,&hw->sf_lock,&flags);

  (hw->cb.lmem_free_func)(hw->cb.call_with,(void **)&sf->sample_data);
  (hw->cb.free_func)(hw->cb.call_with,sf);

  debug(DERR,"!! no more ids available for SoundFonts\n");
  return -2;
 }

 // save sf->sample_data for reference
 short *sdata_tmp=sf->sample_data;

 memcpy(sf,in,in->size-in->header.sample_len-4);
 sf->id=id;
 sf->sample_data=sdata_tmp;

 debug(DLIB,"-- SoundFont Loading [%x]; new id=%d (partial)\n",sf,sf->id);
 sf->list.next=sf->list.prev=NULL;

 // translate pointers
 sf->presets=(sfPresetHeader *)(&sf->data);
 sf->preset_bags=(sfModGenBag *)((uintptr_t)(&sf->data)+(uintptr_t)sf->preset_bags);
 sf->pmodlists=(sfModList *)((uintptr_t)(&sf->data)+(uintptr_t)sf->pmodlists);
 sf->pgenlists=(sfGenList *)((uintptr_t)(&sf->data)+(uintptr_t)sf->pgenlists);
 sf->insts=(sfInst *)((uintptr_t)(&sf->data)+(uintptr_t)sf->insts);
 sf->inst_bags=(sfModGenBag *)((uintptr_t)(&sf->data)+(uintptr_t)sf->inst_bags);
 sf->imodlists=(sfModList *)((uintptr_t)(&sf->data)+(uintptr_t)sf->imodlists);
 sf->igenlists=(sfGenList *)((uintptr_t)(&sf->data)+(uintptr_t)sf->igenlists);
 sf->samples=(sfSample *)((uintptr_t)(&sf->data)+(uintptr_t)sf->samples);

 list_add(&sf->list, &hw->sf);

 kx_lock_release(hw,&hw->sf_lock,&flags);

 return id;
}

KX_API(int,kx_load_soundfont_samples(kx_hw *hw,sf_load_sample_property *sf_p))
{
 unsigned long flags=0;
 kx_lock_acquire(hw,&hw->sf_lock, &flags);

 struct list *item;
 for_each_list_entry(item, &hw->sf) 
 {
 	kx_sound_font *sf;
        sf = list_item(item, kx_sound_font, list);
        if(sf)
        {
        	if(sf->id==sf_p->id)
        	{
        		if(sf_p->pos+sf_p->size<=(int)sf->header.sample_len+4)
        		{
        		 char *pp=(char *)((hw->cb.lmem_get_addr_func)(hw->cb.call_with,(void **)&sf->sample_data,sf_p->pos,NULL));

        		 if(pp)
        		  memcpy(pp,&sf_p->data[0],sf_p->size);
/*        		 if(sf_p->size!=4096-12) // last item
        		 {
                           if(*(dword *)((dword)sf+(dword)sf->size-4-1)!=KX_SOUNDFONT_MAGIC)
                           {
                            debug(DLIB,"-- !!! NB! Soundfont is not consistent after last load\n");
                           }
        		 }
*/
                         kx_lock_release(hw,&hw->sf_lock,&flags);
                         return 0;
        		} else 
        		{ 
        		 debug(DLIB,"load sf samples: too long block (%d)\n",sf_p->pos);
                         kx_lock_release(hw,&hw->sf_lock,&flags);
        		 return -1;
        		}
        	}
        }
 }
 debug(DLIB,"sf load samples: unknown ID (%d)\n",sf_p->id);
 kx_lock_release(hw,&hw->sf_lock,&flags);
 return -2;
}


KX_API(int,kx_unload_soundfont(kx_hw *hw,int id))
{
  unsigned long flags=0;
  kx_lock_acquire(hw,&hw->sf_lock, &flags);

  struct list *item;
  for_each_list_entry(item, &hw->sf) 
  {
  	kx_sound_font *sf;
        sf = list_item(item, kx_sound_font, list);
        if(sf)
        if(sf->id==id)
        {
          debug(DLIB,"-- SoundFont Unload [%x] id=%d\n",sf,sf->id);
          list_del(&sf->list);
          kx_lock_release(hw,&hw->sf_lock,&flags);

          (hw->cb.lmem_free_func)(hw->cb.call_with,(void **)&sf->sample_data);
          (hw->cb.free_func)(hw->cb.call_with,sf);

          return 0;
        }
  }

  kx_lock_release(hw,&hw->sf_lock,&flags);

// debug(DLIB,"ERROR: unload() failed: sf (%d) not found\n",id);
 return -1; // not found
}

KX_API(int,kx_enum_soundfonts(kx_hw *hw,sfHeader *hdr,int size))
{
  if(!(hw->initialized&KX_SF_INITED))
  {
   debug(DERR,"SF enum w/o being initialized\n");
   return -1;
  }
  unsigned long flags=0;
  kx_lock_acquire(hw,&hw->sf_lock, &flags);
  int total=0;

  struct list *item;
  for_each_list_entry(item, &hw->sf) 
  {
	kx_sound_font *sf;  	
        sf = list_item(item, kx_sound_font, list);
        total++; // just count soundfonts
  }

  if(size==0) // just tell how much mem is needed
  {
   kx_lock_release(hw,&hw->sf_lock,&flags);
   return total*(int)sizeof(sfHeader);
  }

  if(((dword)size!=total*sizeof(sfHeader)) || (!hdr)) // not match
  {
   kx_lock_release(hw,&hw->sf_lock,&flags);
   return -1;
  }

  total=0;
  for_each_list_entry(item, &hw->sf) 
  {
	kx_sound_font *sf;  	
        sf = list_item(item, kx_sound_font, list);
        memcpy(&hdr[total],&sf->header,sizeof(sfHeader));
        hdr[total].rom_ver.minor=(word)sf->id;
        total++;
  }

  kx_lock_release(hw,&hw->sf_lock,&flags);
  return 0;
}

int kx_soundfont_init(kx_hw *hw)
{
 init_list(&hw->sf);
 hw->initialized|=KX_SF_INITED;
 return 0;
}

int kx_soundfont_close(kx_hw *hw)
{
  if(!(hw->initialized&KX_SF_INITED))
  {
   debug(DERR,"SF close w/o being initialized\n");
   return -1;
  }
  hw->initialized&=~KX_SF_INITED;

  for(int i=1;i<MAX_SOUNDFONTS;i++)
  {
        kx_unload_soundfont(hw,i);
  }

 return 0;
}


#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
KX_API(int,kx_sf_get_preset_description(kx_hw *hw,int bank,int preset,char *name))
{
  unsigned long flags=0;
  kx_lock_acquire(hw,&hw->sf_lock, &flags);

  int sf_bank=0;

  struct list *item;
  for_each_list_entry(item, &hw->sf) 
  {
	kx_sound_font *sf;  	
        sf = list_item(item, kx_sound_font, list);

        // look for preset
        for(int aa=0;aa<sf->header.presets-1;aa++) // 'EOP' is last; so: -1
        {
          if( ((sf->presets[aa].bank+sf->header.sfman_id)==(dword)bank) &&
              (
               (sf->presets[aa].preset==preset) ||
               (preset==-1) // check if bank is used?
              )
            )
          {
           if(preset!=-1)
             strncpy(name,sf->presets[aa].name,sizeof(sf->presets[aa].name));
           else
             *(int *)name=sf_bank;             
           kx_lock_release(hw,&hw->sf_lock,&flags);
           return 0;
          }
        }
        sf_bank++;
  }

  kx_lock_release(hw,&hw->sf_lock,&flags);

  return -1;
}

// returns number of filled in values
#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
int sf_find_voice(kx_midi_state *midi,kx_voice_param *table,int chn,int note,int vel) // 0,if no voice match
{
  int cnt=0;

  kx_hw *hw=midi->hw;

  unsigned long flags=0;
  kx_lock_acquire(hw,&hw->sf_lock, &flags);

  word bank;

  switch(hw->synth_compat&KX_SYNTH_COMPAT_BANK)
  {
   case KX_SYNTH_COMPAT_BANK_NORMAL:
   default:
   	bank=midi->channels[chn].bank_lsb+midi->channels[chn].bank_msb;
   	break;
   case KX_SYNTH_COMPAT_BANK_LSB:
	bank=midi->channels[chn].bank_lsb;
   	break;
   case KX_SYNTH_COMPAT_BANK_MSB:
	bank=midi->channels[chn].bank_msb;
   	break;
   case KX_SYNTH_COMPAT_BANK_LSB_MSB:
	bank=midi->channels[chn].bank_lsb+(midi->channels[chn].bank_msb<<7);
   	break;
  }

AGAIN:

  struct list *item;
  for_each_list_entry(item, &hw->sf)
  {
	    kx_sound_font *sf;  	
        sf = list_item(item, kx_sound_font, list);

        if(sf->header.subsynth!=0)
        {
         if(sf->header.subsynth==1 && midi->synth_num!=0) continue;
         if(sf->header.subsynth==2 && midi->synth_num!=1) continue;
        }

        debug(DSFNT,"- SoundFont #%d ('%s')\n",sf->id,sf->header.name);

        // look for preset
        for(int aa=0;aa<sf->header.presets-1;aa++) // 'EOP' is last; so: -1
        {
          if(
             (
               (chn!=(midi->drum_channel-1)) // found non-drum channels
              &&
               (
                (sf->presets[aa].preset==midi->channels[chn].program) 
               && 
                ((sf->presets[aa].bank+sf->header.sfman_id)==bank)
               )
             )  
            ||
             (
               (chn==(midi->drum_channel-1))
              &&
               ( 
                 (sf->presets[aa].preset==midi->channels[chn].program) 
               && 
                 (sf->header.sfman_id==bank)
               &&
                 (sf->presets[aa].bank==128)
               )
             )
            )
          {
            // just sanity check. need them more for other .._bags
            if(sf->presets[aa].preset_bag_ndx>=sf->presets[aa+1].preset_bag_ndx)
            {
            	debug(DLIB,"!!! Incorrect soundfont: bad preset bag [inst: %d bank: %d/%d]\n",
            	  midi->channels[chn].program,midi->channels[chn].bank_lsb,midi->channels[chn].bank_msb);
                kx_lock_release(hw,&hw->sf_lock,&flags);
            	return 0;
            }

            int has_gz=0;
            // look for Preset Global Zone
            if(sf->pgenlists[sf->preset_bags[sf->presets[aa].preset_bag_ndx+1].gen_ndx-1].gen_oper!=41) // not instrument
            {
             has_gz=1;
            }

            sf_parameters_t inst[SF_PARAMETERS],preset[SF_PARAMETERS],
               inst_gz[SF_PARAMETERS],preset_gz[SF_PARAMETERS];

            memset(&preset[0],0,sizeof(sf_parameters_t)*SF_PARAMETERS);
            memset(&preset_gz[0],0,sizeof(sf_parameters_t)*SF_PARAMETERS);

            sf_parameters_t *current;
            if(has_gz)
             current=&preset_gz[0];
            else
             current=&preset[0];

            debug(DSFNT,"- preset #%d ('%s')\n",aa,sf->presets[aa].name);

            // parse all preset bags (generators)
            for(int bb=sf->presets[aa].preset_bag_ndx;bb<sf->presets[aa+1].preset_bag_ndx;bb++)
            {
               for(int gg=sf->preset_bags[bb].gen_ndx;gg<sf->preset_bags[bb+1].gen_ndx;gg++)
               {
                 if(sf->pgenlists[gg].gen_oper<SF_PARAMETERS) 
                 {
                  if(sf->pgenlists[gg].gen_oper!=41) // instrument?
                  {
                  	current[sf->pgenlists[gg].gen_oper].gen=sf->pgenlists[gg].gen_amount.amount_w;
                  	current[sf->pgenlists[gg].gen_oper].flag=1;
                  } else // not instrument
                  {
                  	// begin new instrument
                  	int has_i_gz=0;
                  	int inst_n=sf->pgenlists[gg].gen_amount.amount_w;

                        memset(&inst_gz[0],0,sizeof(sf_parameters_t)*SF_PARAMETERS);
                        memset(&inst[0],0,sizeof(sf_parameters_t)*SF_PARAMETERS); 
                        // memcpy(&inst[0],sf_defaults,sizeof(sf_parameters_t)*SF_PARAMETERS);

                        // look for Instrument Global Zone
                        if(sf->igenlists[sf->inst_bags[sf->insts[inst_n].inst_bag_ndx+1].gen_ndx-1].gen_oper!=53) // not sample
                        {
                         has_i_gz=1;
                        }
                        sf_parameters_t *cur2;
                        if(has_i_gz)
                         cur2=&inst_gz[0];
                        else
                         cur2=&inst[0];

                        for(int cc=sf->insts[inst_n].inst_bag_ndx;cc<sf->insts[inst_n+1].inst_bag_ndx;cc++)
                        {
                            for(int dd=sf->inst_bags[cc].gen_ndx;dd<sf->inst_bags[cc+1].gen_ndx;dd++)
                            {
                                    if(sf->igenlists[dd].gen_oper<SF_PARAMETERS)
                                    {
                                       if(sf->igenlists[dd].gen_oper!=53) // sample #
                                       {
                                  	cur2[sf->igenlists[dd].gen_oper].gen=sf->igenlists[dd].gen_amount.amount_w;
                                  	cur2[sf->igenlists[dd].gen_oper].flag=1;
                                       }
                                     else // sample
                                     {

                                     // process global zones...
                                     for(int i=0;i<SF_PARAMETERS;i++)
                                     {
                                        if(!inst[i].flag) // use default or global zone
                                        {
                                     	 inst[i].gen=inst_gz[i].flag?inst_gz[i].gen:inst[i].gen;
                                     	 if(inst_gz[i].flag)
                                     	  inst[i].flag=1;
                                     	} // else:
                                     	  // there's local instr zone generator: ignore anything else

                                     	if(!preset[i].flag)
                                     	{
                                     	  preset[i].gen=preset_gz[i].flag?preset_gz[i].gen:preset[i].gen;
                                     	  if(preset_gz[i].flag)
                                     	   preset[i].flag=1;
                                     	} // else
                                     	  // there's local preset zone generator: ignore anything else
                                     }

                                     // first of all: check ranges

                                     // key range
                                     if(inst[43].flag) // keyrange
                                     {
                                        if(inst[43].gen&0x8080)
                                         break;
                                     	if(note<(int)(short)(inst[43].gen&0xff) ||
                                     	   note>(int)(short)((inst[43].gen>>8)&0xff))
                                     	   break;
                                     }
                                     if(preset[43].flag) // keyrange
                                     {
                                        if(preset[43].gen&0x8080)
                                         break;
                                     	if(note<(int)((short)(preset[43].gen&0xff)) ||
                                     	   note>(int)((short)((preset[43].gen>>8)&0xff)))
                                     	   break;
                                     }
                                     // velocity range
                                     if(inst[44].flag) // velrange
                                     {
                                     	if(vel<(int)(short)(inst[44].gen&0xff) ||
                                     	   vel>(int)(short)((inst[44].gen>>8)&0xff))
                                     	{
                                     	   break;
                                     	  }
                                     }
                                     if(preset[44].flag) // velrange
                                     {
                                     	if(vel<(int)((short)(preset[44].gen&0xff)) ||
                                     	   vel>(int)((short)((preset[44].gen>>8)&0xff)))
                                     	  {
                                     	   break;
                                     	  }
                                     }

                                     // finalize settings
                                     int nsample=sf->igenlists[dd].gen_amount.amount_w;

                                     int start_addr=(int)(short)inst[0].gen+(int)((short)inst[4].gen)*32768;
                                     int end_addr=(int)(short)inst[1].gen+(int)((short)inst[12].gen)*32768;
                                     int start_loop=(int)(short)inst[2].gen+(int)((short)inst[45].gen)*32768;
                                     int end_loop=(int)(short)inst[3].gen+(int)((short)inst[50].gen)*32768;

                                     // ----------------------- final code -----------------------
                                     table[cnt].sf2_usage=VOICE_USAGE_MIDI|VOICE_FLAGS_16BIT|VOICE_FLAGS_MONO;

                                     for(int ttt=0;ttt<SF_PARAMETERS;ttt++)
                                     {
                                      switch(ttt)
                                      {
                                       case 51: // coarse tune, semitones
                                       case 52: // fune tune, cents
                                       case 9: // initial_filterQ (cB)
                                       case 15: // chorus %
                                       case 16: // rev %
                                       case 17: // pan %
                                       case 29: // modenv sustain, %
                                       case 37: // volenv sustain, cB
                                       case 48: // init. attn, cB
                                       case 11: // modenv to filter cutoff
                                       case 7: // modenv to pitch
                                       case 5: // modlfo to pitch
                                       case 6: // viblfo to pitch
                                       case 10: // modlfo to filter cutoff
                                       case 13: // modlfo to vol
                                       default:
                                        table[cnt].sf2_params[ttt]=(int)(short)inst[ttt].gen+(int)(short)preset[ttt].gen;
                                        break;
                                       case 21:
                                       case 23: // molfo delay / viblfo delay
                                       case 25: // vol env params
                                       case 26:
                                       case 27:
                                       case 28:
                                       case 30:
                                       case 33: // mod env params
                                       case 34:
                                       case 35:
                                       case 36:
                                       case 38:
                                       	// timings in 'msec'<->abs cents - special case
                                       	if(inst[ttt].flag)
                                       	 table[cnt].sf2_params[ttt]=(int)(short)inst[ttt].gen+(int)(short)preset[ttt].gen;
                                       	else
                                       	 table[cnt].sf2_params[ttt]=-12000+(int)(short)preset[ttt].gen;
                                       	break;
                                       case 22:
                                       case 24: // modlfo / viblfo freqs
                                       case 31:
                                       case 32:
                                       case 39:
                                       case 40: // keynum to something...
                                       	 table[cnt].sf2_params[ttt]=(int)(short)inst[ttt].gen+(int)(short)preset[ttt].gen;
                                       	break;
                                       case 8: // initial filter cutoff
                                       	if(inst[ttt].flag)
                                       	 table[cnt].sf2_params[ttt]=(int)(short)inst[ttt].gen+(preset[ttt].flag?(int)(short)preset[ttt].gen:0);
                                       	else
                                       	 table[cnt].sf2_params[ttt]=13500+(int)(short)preset[ttt].gen; // 20000 Hz
                                        break;
                                      }
                                     }

                                     if(inst[47].flag && inst[47].gen!=(word)-1) // force_velocity
                                     {
                                      vel=(int)(short)inst[47].gen;
                                      table[cnt].sf2_params[47]=vel;
                                     }
                                     else
                                      table[cnt].sf2_params[47]=-1;

                                     if(inst[57].flag) // instr layer only; exclusive_class
                                      table[cnt].sf2_params[57]=inst[57].gen;
                                     else
                                      table[cnt].sf2_params[57]=0;

                                     if(inst[54].flag) // sample_mode
                                      table[cnt].sf2_params[54]=inst[54].gen&0x3;
                                     else
                                      table[cnt].sf2_params[54]=0;

                                     if(inst[56].flag) // scale_tune
                                      table[cnt].sf2_params[56]=inst[56].gen;
                                     else
                                      table[cnt].sf2_params[56]=100;

                                     // -----------

                                     // calculate offset=pitch [initial]
                                     int offset;
                                     int ori_key;

                                     if(inst[58].flag && ((short)inst[58].gen)!=-1)
                                      ori_key=(int)(short)inst[58].gen;
                                     else
                                      ori_key=(int)(short)sf->samples[nsample].original_key;

                                     if(ori_key==-1)
                                      ori_key=60;

                                     if(inst[46].flag) // force_key_num
                                     {
                                 	offset = (int)((short)inst[46].gen - ori_key) * 4096;
                                     }
                                     else 
                                     {
                                  	offset = (note - ori_key) * 4096;
                                     }
                                     offset/=12;

                                     // scale_tune
                                     offset = (offset * (table[cnt].sf2_params[56]+midi->channels[chn].nrpn_sf_data[56])) / 100;

                                     int tmp1= (int)(4096 * (
                                          (table[cnt].sf2_params[51]+midi->channels[chn].nrpn_sf_data[51])*100 + 
                                          (table[cnt].sf2_params[52]+midi->channels[chn].nrpn_sf_data[52]) + 
                                          (int)(short)sf->samples[nsample].correction
                                          ) ); // coarse&fine&correction
                                     tmp1=tmp1/1200;
                                     offset+=tmp1;

                                     // root pitch = sr2pitch(sample_rate): returns 0xe000 for 48000; 0xde0b - 44100
                                     offset+=(int)(kx_srToPitch(kx_sr_coeff(hw,sf->samples[nsample].sample_rate)) >> 8);
                                     table[cnt].sf2_pitch = (word)offset;

				     // defaults: (re-written on DCYSUSV_ON)
                                     // ------------------------
                                     table[cnt].fc_target = 0xffff;
                                     table[cnt].current_fc = 0xffff;

                                     // ----

                                     start_addr+=sf->samples[nsample].start;
                                     end_addr+=sf->samples[nsample].end;

                                     if(table[cnt].sf2_params[54]==2)
                                      table[cnt].sf2_params[54]=0;

                                     table[cnt].loop_type=table[cnt].sf2_params[54];

                                     if(table[cnt].sf2_params[54]) // has any loop: 1 || 3
                                     {
                                       start_loop+=sf->samples[nsample].start_loop;
                                       end_loop+=sf->samples[nsample].end_loop;
                                     }
                                     else // 0 || [2->0]
                                     {
                                     /*
                                       // the audio stops 28-samples after the loop
                                       // (for 16-bit audio -- due to cache)
                                       // 3538g: 28/2 -> 28 for both
                                       // pre-3538: was end_addr for start only
                                       if(hw->is_10k2)
                                       {
                                        start_loop=end_addr+28;
                                        end_loop=end_addr+28;
                                       }
                                       else
                                       {
                                        start_loop=end_addr-32;
                                        end_loop=end_addr-32;
                                       }

                                       // kx_voice_start will set SOLEH/SOLEL bits
                                      */
                                       // pre-3536b
                                       start_loop=end_addr+8/2;
                                       end_loop=end_addr+40/2;
                                       end_addr+=46/2;

                                       table[cnt].sf2_params[54]=table[cnt].loop_type=1;
                                     }

                                     // (unused) 
                                     // sf->samples[nsample].sample_link

                                     table[cnt].sample_type=sf->samples[nsample].sample_type&0xf;

                                     __int64 a;
                                     void *start_=((hw->cb.lmem_get_addr_func)(hw->cb.call_with,(void **)&sf->sample_data,start_addr*2,&a));

                                     table[cnt].start = (dword) ((((uintptr_t)start_)&0xfff)/2); // lowest offset
                                     table[cnt].startloop = start_loop-start_addr+table[cnt].start;
                                     table[cnt].endloop = end_loop-start_addr+table[cnt].start;
                                     table[cnt].end = end_addr-start_addr+table[cnt].start;

                                     if(a==0)
                                     {
                                      debug(DLIB,"Error remapping phys2linear\n");
                                      kx_lock_release(hw,&hw->sf_lock,&flags);
                                      return 0;
                                     }

                                     // #error this is wrong, need to pass more data for kx_alloc_buffer()
                                     // FIXME NOW !! 3543

                                     // buffer.size
                                     table[cnt].tmp_buffer_size=((table[cnt].end * 2 / KX_PAGE_SIZE) + (((table[cnt].end * 2) % KX_PAGE_SIZE) ? 1 : 0))*KX_PAGE_SIZE;
                                     // buffer.physical
                                     table[cnt].tmp_buffer_physical=(dword)a; // final physical address [this is only first part!]
                                     // buffer.addr
                                     table[cnt].tmp_buffer_addr=(void *)((uintptr_t)start_&0xfffff000);

                                     table[cnt].interpolate=0;

                                     cnt++;

                                     if(cnt>=KX_MAX_SF_VOICES)
                                     {
                                      kx_lock_release(hw,&hw->sf_lock,&flags);
                                      return cnt;
                                      }

                                     // ----------------------- end final code -------------------
                                     }
                                    } else debug(DLIB,"!! Incorrect igenlist generator [%d]\n",
                                      sf->igenlists[dd].gen_oper);
                            } // for all generators in instr (dd)
                            cur2=&inst[0];
                            memset(&inst[0],0,sizeof(sf_parameters_t)*SF_PARAMETERS);
                            // memcpy(&inst[0],sf_defaults,sizeof(sf_parameters_t)*SF_PARAMETERS);
                        } // for all bags with generators in instr (cc)
                  } // was instrument
                 } else debug(DLIB,"!!! Unknown SF generator [%xh]\n",sf->pgenlists[gg].gen_oper);
               } // for k: all gens in preset_bags[]

               current=&preset[0];
               memset(&preset[0],0,sizeof(sf_parameters_t)*SF_PARAMETERS);
            } // for all bags with generators in preset
          } // found needed preset
        } // each preset headers

        if(cnt!=0) // found already
         break;
  } // for each soundfont

  if(cnt==0 && bank!=0) // not found
  {
    bank=0;
    goto AGAIN; // re-parse all the soundfonts
  }

  kx_lock_release(hw,&hw->sf_lock,&flags);

  return cnt;
}
