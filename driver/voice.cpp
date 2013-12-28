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

void kx_setup_playback(kx_hw *hw,int num)
{
	int i;
	if(num<0 || num>=KX_NUMBER_OF_VOICES)
	 return;

	kx_voice *voice=&hw->voicetable[num];

        for(i = 0; i < (voice->usage & VOICE_FLAGS_STEREO ? 2 : 1); i++) 
        {
        	kx_writeptr_multiple(hw, num+i, 
        		IFATN, 0xffff, // attn=max, filter=max [none]
        		DCYSUSV, 0x0, // env is disabled by default...
        		VTFT, 0xffff, // 3538e: 0x0, // filter target -- muted
        			// 3538d: was 0xffff
                                // changed according to awe guide
        		CVCF, 0xffff, // 3538e: 0x0, // current filter -- muted
        			// 3538d: was 0xffff
                                // changed according to awe guide
        		IP, 0, 
        		PTAB, 0x0,
        		REGLIST_END);
	}

	if(voice->usage & VOICE_FLAGS_STEREO) 
	{
		// Set stereo bit
		kx_writeptr(hw, CPF, num, CPF_STEREO_MASK);
		kx_writeptr(hw, CPF, num + 1, CPF_STEREO_MASK);
	} else
	{
		kx_writeptr(hw, CPF, num, 0);
	}

	dword cra=0;

        // engine latencies...
        if(voice->usage & VOICE_FLAGS_16BIT)
	  cra = 24+4; // 16 bit
	 else
	  cra = 56+4; // 8 bit

//	if(voice->usage & VOICE_FLAGS_STEREO)
//	  cra = cra*2; // stereo

        voice->param.start += cra;

	for(i = 0; i < (voice->usage & VOICE_FLAGS_STEREO ? 2 : 1); i++)
	{
		if(!hw->is_10k2)
		{
		 dword r=0;
		 r=((voice->param.send_routings&0xf)<<16)|
		   ((voice->param.send_routings&0xf00)<<12)|
                   ((voice->param.send_routings&0xf0000)<<8)|
                   ((voice->param.send_routings&0xf000000)<<4);
                 kx_writeptr(hw, FXRT_K1, num + i, r);
		}

        dword unfiltered=0;
        if(voice->param.initial_pitch==0xe000)
         unfiltered=0x80808080;

		if(hw->is_10k2) // else
		{
		 kx_writeptr_multiple(hw, num+i,
                    FXRT1_K2, (voice->param.send_routings)|unfiltered,
                    FXRT2_K2, (voice->param.send_xroutings)|unfiltered,
                    // send e,f,g,h
                    FXAMOUNT_K2, (kx_fxamount(hw,(voice[i].param.send_e))&0xff) |
                 			((kx_fxamount(hw,(voice[i].param.send_f))&0xff)<<8) |
                                        ((kx_fxamount(hw,(voice[i].param.send_g))&0xff)<<16) |
                                        ((kx_fxamount(hw,(voice[i].param.send_h))&0xff)<<24),
                    REGLIST_END);
		}

		kx_writeptr(hw, PTAB, num + i, (kx_fxamount(hw,voice[i].param.send_a) << 8) | kx_fxamount(hw,voice[i].param.send_b));

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
		     
		kx_writeptr_multiple(hw, num + i,
				// CSL, ST, CA
				    SDL, voice->param.endloop | (kx_fxamount(hw,voice[i].param.send_d) << 24),
				    SCSA, voice->param.startloop | (kx_fxamount(hw,voice[i].param.send_c) << 24),
				    QKBCA, (voice->param.start) | (voice->param.filterQ<<28) | interpolation | ((voice->usage & VOICE_FLAGS_16BIT) ? 0 : QKBCA_8BITSELECT),
				    // Clear filter delay memory
				    Z1, 0,
				    Z2, 0,
				    // Invalidate maps
				    MAPA, MAP_PTI_MASK | (hw->silentpage.dma_handle * 2),
				    MAPB, MAP_PTI_MASK | (hw->silentpage.dma_handle * 2),
				// modulation envelope
//				    CVCF, CVCF_CURRENTFILTER_MASK,
//				    VTFT, VTFT_FILTERTARGET_MASK,
				    IFATN, (voice->param.initial_fc << 8) | voice->param.initial_attn,
				    LFOVAL1, voice->param.lfo1delay,
				    FMMOD, (voice->param.modlfo2fc&0xff)|((voice->param.modlfo2pitch&0xff)<<8),
				    TREMFRQ, (voice->param.modlfofreq&0xff)|((voice->param.modlfo2vol&0xff)<<8),
				    FM2FRQ2, (voice->param.viblfofreq&0xff)|((voice->param.viblfo2pitch&0xff)<<8),
				    ENVVAL, (voice->param.modatk==0xff)?0xbfff:voice->param.moddelay,
				// volume envelope 
				    VEHA, (voice->param.volatk&0x7f)|((voice->param.volhld&0xff)<<8)|(voice->param.volatk&VEHA_PHASE0),
                                    LFOVAL2, voice->param.lfo2delay,
				    VEV, (voice->param.volatk==0xff)?0xbfff:voice->param.voldelay,
				    ATKHLDM, (voice->param.modatk&0x7f)|((voice->param.modhld&0xff)<<8)|(voice->param.modatk&ATKHLDM_PHASE0),
				    DCYSUSM, (voice->param.modsustain<<8)|(voice->param.moddecay),
				// filter envelope 
				    PEFE_FILTERAMOUNT, voice->param.filter_amount,
				// pitch envelope 
				    PEFE_PITCHAMOUNT, voice->param.pitch_amount,
				    // Invalidate maps once again... (fixme: 15/06/2003)
				    MAPA, MAP_PTI_MASK | (hw->silentpage.dma_handle * 2),
				    MAPB, MAP_PTI_MASK | (hw->silentpage.dma_handle * 2),
				    emuCCR, 0, // 17/01/03
				    REGLIST_END);
	}

        if(voice->usage&VOICE_OPEN_NOTIFY)
        {
         kx_voice_irq_enable(hw,num);
        }
}

void kx_voice_init_cache(kx_hw *hw,int i)
{
	unsigned sample,ccis,cs;

	if(hw->voicetable[i].usage & VOICE_FLAGS_STEREO) 
	{
	  ccis=28; // 24+4 for 16 bit
	  cs=4;
	}
	else
	{
	  ccis=30;
	  cs=2;
	}


     	if(hw->voicetable[i].usage & VOICE_FLAGS_16BIT) 
      	{
      	 sample=0x0;
      	} 
      	 else 
      	{
      	 sample=0x80808080;
      	 ccis*=2; // 56+4 for 8 bit
      	}

        // fill-in cache
      	for(dword j=0; j<16 /*cs*/; j++) // change: 2012-03-04: 3552
      	{
            kx_writeptr(hw, CD0 + j, i, sample);
            if(hw->voicetable[i].usage & VOICE_FLAGS_STEREO)
              kx_writeptr(hw, CD0 + j, i+1, sample);
        }

      	// Reset cache 
      	kx_writeptr(hw, emuCCR, i, 0);
        if(hw->voicetable[i].usage & VOICE_FLAGS_STEREO)
        	kx_writeptr(hw, emuCCR, i + 1, 0);

        // these bits are set automatically by hardware:
//        if(hw->voicetable[i].usage & VOICE_FLAGS_STEREO)
//         ccr|=CCR_INTERLEAVEDSAMPLES;
//        if(hw->voicetable[i].usage & VOICE_FLAGS_16BIT)
//         ccr|=CCR_WORDSIZEDSAMPLES;

        // init cache: cra=0; ccis=ccis
      	kx_writeptr(hw, CCR_CACHEINVALIDSIZE, i, ccis);
        if(hw->voicetable[i].usage & VOICE_FLAGS_STEREO)
        	kx_writeptr(hw, CCR_CACHEINVALIDSIZE, i + 1, ccis);
}

void kx_voice_start(kx_hw *hw,int num)
{
	if(num<0 || num>=KX_NUMBER_OF_VOICES)
	 return;

	kx_voice *voice=&hw->voicetable[num];

	voice->state=VOICE_STATE_STARTED;

    debug(DSTATE,"voice started (%d)\n",num);

    if(voice->param.loop_type!=0) // 1 || 3 -- looped
    {
  	 kx_voice_stop_on_loop_disable(hw, num);
  	 if(voice->usage & VOICE_FLAGS_STEREO)
  	   kx_voice_stop_on_loop_disable(hw, num + 1);
  	}
  	else // 0 : not looped
  	{
  	 kx_voice_stop_on_loop_enable(hw, num);
  	 if(voice->usage & VOICE_FLAGS_STEREO)
  	   kx_voice_stop_on_loop_enable(hw, num + 1);
  	}

  	kx_voice_init_cache(hw,num);

	for(int j = 0; j < (voice->usage & VOICE_FLAGS_STEREO ? 2 : 1); j++)
	{
	 if(j==0)
		kx_writeptr_multiple(hw, num + j,
				    // CVCF, (voice->param.initial_cv << 16) | (voice->param.current_fc&0xffff),
                                    // (kx_voice_start is not used in SF/ASIO)
				    VTFT, (voice->param.volume_target << 16) | (voice->param.fc_target&0xffff),
				    DCYSUSV, ((voice->param.volsustain&0x7f) << 8) | (voice->param.voldecay&0xff),
				     // |DCYSUSV_CHANNELENABLE_MASK in soundfont
				    PTAB,(voice->param.pitch_target<<16) | (kx_fxamount(hw,voice[j].param.send_a) << 8) | kx_fxamount(hw,voice[j].param.send_b),
				    CPF,(voice->param.pitch_target<<16)|(voice->usage&VOICE_FLAGS_STEREO?CPF_STEREO_MASK:0),
				    IP,voice->param.initial_pitch,
				    REGLIST_END);
	 else
		kx_writeptr_multiple(hw, num + j,
				    // CVCF, (voice->param.initial_cv << 16) | (voice->param.current_fc&0xffff),
				    // (kx_voice_start is not used in SF/ASIO)
				    VTFT, (voice->param.volume_target << 16) | (voice->param.fc_target&0xffff),
				    DCYSUSV, ((voice->param.volsustain&0x7f) << 8) | (voice->param.voldecay&0xff),
				     // |DCYSUSV_CHANNELENABLE_MASK in soundfont
				    PTAB,(voice->param.pitch_target<<16) | (kx_fxamount(hw,voice[j].param.send_a) << 8) | kx_fxamount(hw,voice[j].param.send_b),
				    // CPF_CURRENTPITCH,voice->param.pitch_target,
				    IP,voice->param.initial_pitch,
				    REGLIST_END);

	  //debug(DWDM,"== [num+j: %d] vtft: %x, send_a: %x, send_b: %x\n",
	  //	num+j,voice->param.volume_target,
	  //	kx_fxamount(hw,voice[j].param.send_a),
	  //	kx_fxamount(hw,voice[j].param.send_b));
	}
}

void kx_voice_start_multiple(kx_hw *hw,dword low,dword high)
{
 debug(DSTATE,"multiple voices started (x%x x%x)\n",low,high);

 int synth_mode=0;

 for(int what=0;what<2/*3*/;what++)
 for(int cnt=0;cnt<2;cnt++)
 {
    int start,end,shift;
    dword compare;

    if(cnt==0)
     { start=0; end=32; compare=low; shift=0; }
    else
     { start=32; end=64; compare=high; shift=32; }

    for(int i=start;i<end;i++)
    {
        if(compare&(1<<(i-shift)))
        {
            kx_voice *voice=&hw->voicetable[i];

            if(voice->usage&VOICE_USAGE_MIDI)
              synth_mode=1;
            else
              synth_mode=0;

            if(what==0)
            {
             	hw->voicetable[i].state=VOICE_STATE_STARTED;

                if(hw->voicetable[i].param.loop_type!=0) // 1 || 3 -- looped
                {
              	 kx_voice_stop_on_loop_disable(hw, i);
              	 if(hw->voicetable[i].usage & VOICE_FLAGS_STEREO)
              	   kx_voice_stop_on_loop_disable(hw, i+1);
              	}
              	else // not looped
              	{
              	 kx_voice_stop_on_loop_enable(hw, i);
              	 if(hw->voicetable[i].usage & VOICE_FLAGS_STEREO)
              	   kx_voice_stop_on_loop_enable(hw, i+1);
              	}

    	        kx_voice_init_cache(hw,i);
           }
           else
           if(what==1)
           {
            	for(int j = 0; j < (voice->usage & VOICE_FLAGS_STEREO ? 2 : 1); j++)
            	{
            	 if(synth_mode)
            	 {
            	   if(j==0)
            		kx_writeptr_multiple(hw, j + i,
            				    CVCF, (voice->param.initial_cv << 16) | (voice->param.current_fc&0xffff),
            				    IFATN,(voice->param.initial_fc << 8) | voice->param.initial_attn,
            				    VTFT,(voice->param.volume_target << 16) | (voice->param.fc_target&0xffff),
            				    DCYSUSV, ((voice->param.volsustain&0x7f) << 8) | (voice->param.voldecay&0xff),
            				     // |DCYSUSV_CHANNELENABLE_MASK in soundfont
            				    PTAB,(voice->param.pitch_target<<16) | (kx_fxamount(hw,voice[j].param.send_a) << 8) | kx_fxamount(hw,voice[j].param.send_b),
            				    CPF,(voice->param.pitch_target<<16)|(voice->usage&VOICE_FLAGS_STEREO?CPF_STEREO_MASK:0),
            				    IP,voice->param.initial_pitch,
            				    REGLIST_END);
            	   else
            		kx_writeptr_multiple(hw, j + i,
            				    CVCF, (voice->param.initial_cv << 16) | (voice->param.current_fc&0xffff),
            				    IFATN,(voice->param.initial_fc << 8) | voice->param.initial_attn,
            				    VTFT,(voice->param.volume_target << 16) | (voice->param.fc_target&0xffff),
            				    DCYSUSV, ((voice->param.volsustain&0x7f) << 8) | (voice->param.voldecay&0xff),
            				     // |DCYSUSV_CHANNELENABLE_MASK in soundfont
            				    PTAB,(voice->param.pitch_target<<16) | (kx_fxamount(hw,voice[j].param.send_a) << 8) | kx_fxamount(hw,voice[j].param.send_b),
            				    // CPF_CURRENTPITCH,voice->param.pitch_target,
            				    IP,voice->param.initial_pitch,
            				    REGLIST_END);
            	 }
            	 else  // not synth mode
            	 {
            	   if(j==0)
            		kx_writeptr_multiple(hw, j + i,
    //        				    CVCF, (voice->param.initial_cv << 16) | (voice->param.current_fc&0xffff),
    //        				    IFATN,(voice->param.initial_fc << 8) | voice->param.initial_attn,
            				    VTFT,(voice->param.volume_target << 16) | (voice->param.fc_target&0xffff),
            				    DCYSUSV, ((voice->param.volsustain&0x7f) << 8) | (voice->param.voldecay&0xff),
            				     // |DCYSUSV_CHANNELENABLE_MASK in soundfont
            				    PTAB,(voice->param.pitch_target<<16) | (kx_fxamount(hw,voice[j].param.send_a) << 8) | kx_fxamount(hw,voice[j].param.send_b),
            				    CPF,(voice->param.pitch_target<<16)|(voice->usage&VOICE_FLAGS_STEREO?CPF_STEREO_MASK:0),
            				    IP,voice->param.initial_pitch,
            				    REGLIST_END);
            	   else
            		kx_writeptr_multiple(hw, j + i,
    //        				    CVCF, (voice->param.initial_cv << 16) | (voice->param.current_fc&0xffff),
    //        				    IFATN,(voice->param.initial_fc << 8) | voice->param.initial_attn,
                                // note: 3543: voice[j] is used for VTFT
            				    VTFT,(voice[j].param.volume_target << 16) | (voice->param.fc_target&0xffff),
            				    DCYSUSV, ((voice->param.volsustain&0x7f) << 8) | (voice->param.voldecay&0xff),
            				     // |DCYSUSV_CHANNELENABLE_MASK in soundfont
            				    PTAB,(voice->param.pitch_target<<16) | (kx_fxamount(hw,voice[j].param.send_a) << 8) | kx_fxamount(hw,voice[j].param.send_b),
            				    // CPF_CURRENTPITCH,voice->param.pitch_target,
            				    IP,voice->param.initial_pitch,
            				    REGLIST_END);
    	         } // synth mode
    	       } // for
    	   } // what==1
	    } // low&high (if compare)
	} // start & end
 }	// cnt
}

void kx_voice_stop_multiple(kx_hw *hw,dword low,dword high)
{
 debug(DSTATE,"multiple voices stopped (x%x x%x)\n",low,high);

 int i;
 for(i=0;i<32;i++)
 {
  if(low&(1<<i))
  {
   hw->voicetable[i].state=VOICE_STATE_STOPPED;

   for(int j=0;j<(hw->voicetable[i].usage&VOICE_FLAGS_STEREO?2:1);j++)
    kx_writeptr_multiple(hw, i+j,
					PTAB_PITCHTARGET, 0,
					CPF_CURRENTPITCH, 0, 
					IP, 0,
					DCYSUSV,0x7f, // fixme3: my thought
					VTFT, VTFT_FILTERTARGET_MASK, // 0xffff [per awe specs]
					CVCF, CVCF_CURRENTFILTER_MASK, // 0xffff [per awe specs]
   			REGLIST_END);
  }
 }
 
 for(i=32;i<64;i++)
 {
  if(high&(1<<(i-32)))
  {
   hw->voicetable[i].state=VOICE_STATE_STOPPED;

   for(int j=0;j<(hw->voicetable[i].usage&VOICE_FLAGS_STEREO?2:1);j++)
    kx_writeptr_multiple(hw, i+j,
					PTAB_PITCHTARGET, 0,
					CPF_CURRENTPITCH, 0, 
					IP, 0,
					DCYSUSV,0x7f, // fixme3: my thought
					VTFT, VTFT_FILTERTARGET_MASK,  // 0xffff [per awe specs]
					CVCF, CVCF_CURRENTFILTER_MASK, // 0xffff [per awe specs]
   			REGLIST_END);
  }
 }
}

void kx_voice_stop(kx_hw *hw,int num)
{
	if(num<0 || num>=KX_NUMBER_OF_VOICES)
	 return;
	 	
	kx_voice *voice = &hw->voicetable[num];
	int i;

        debug(DSTATE,"voice stopped (%d)\n",num);

	voice->state=VOICE_STATE_STOPPED;

	for(i = 0; i < (voice->usage & VOICE_FLAGS_STEREO ? 2 : 1); i++) 
	{
		kx_writeptr_multiple(hw, num + i,
					PTAB_PITCHTARGET, 0,
					CPF_CURRENTPITCH, 0, 
					IP, 0,
					DCYSUSV,0x7f, // fixme3: my thought
					VTFT, VTFT_FILTERTARGET_MASK, //  [per awe specs]
					CVCF, CVCF_CURRENTFILTER_MASK, //  [per awe specs]
					REGLIST_END);
	}
}

int kx_voice_alloc(kx_hw *hw, int usage)
{
	kx_voice *voicetable = hw->voicetable;
	int pass=0;
	int i=0;
	int found=0;
AGAIN:
	unsigned long int_flags=0;

	kx_lock_acquire(hw,&hw->k_lock, &int_flags);

	usage=(usage&(~0xff))|VOICE_USAGE_TEMP;

    int last_voice = hw->last_voice;

    if(hw->ext_flags&KX_HW_ALT_CHN_ALLOCATION)
      last_voice=0;

	if(usage & VOICE_FLAGS_STEREO) 
	{
		for(i = (last_voice&0x7e); i < KX_NUMBER_OF_VOICES; i += 2)
			if((voicetable[i].usage==VOICE_USAGE_FREE) && (voicetable[i + 1].usage==VOICE_USAGE_FREE))
			{
				voicetable[i].usage = usage;
				voicetable[i + 1].usage = usage;
				last_voice=i+2;
				found=1;
				break;
			}
		if(!found)
		for(i = 0; i < (last_voice&0x7e); i += 2)
			if((voicetable[i].usage==VOICE_USAGE_FREE) && (voicetable[i + 1].usage==VOICE_USAGE_FREE))
			{
				voicetable[i].usage = usage;
				voicetable[i + 1].usage = usage;
				last_voice=i+2;
				found=1;
				break;
			}
	}
	 else 
	{
		for(i = last_voice; i < KX_NUMBER_OF_VOICES; i++)
			if(voicetable[i].usage==VOICE_USAGE_FREE) 
			{
				voicetable[i].usage = usage;
				last_voice=i+1;
				found=1;
				break;
			}
		if(!found)
		for(i = 0; i < last_voice; i++)
			if(voicetable[i].usage==VOICE_USAGE_FREE) 
			{
				voicetable[i].usage = usage;
				last_voice=i+1;
				found=1;
				break;
			}
	}

	if(last_voice>=KX_NUMBER_OF_VOICES)
	 last_voice=0;

    hw->last_voice=last_voice;

	kx_lock_release(hw,&hw->k_lock, &int_flags);


	if(!found) // not found 'free' ones
	{
		if(pass<2) // first or second time
		{
		    if(kx_steal_voices(hw,usage)==0) // found
		    {
		     pass++;
                     goto AGAIN;
                    }
                }
                debug(DLIB,"kx_alloc_voice: not found\n");
		return -1;
	}

	int num=i;

	for(i = 0; i < (usage & VOICE_FLAGS_STEREO ? 2 : 1); i++) 
	{
		kx_writeptr_multiple(hw, num + i, 	IFATN, 0xffff,
							DCYSUSV, 0x00, // envelope is off
							VTFT, 0xffff, // 3538e: 0x0, //  [per awe specs]
									// 3538d was 0xffff
							CVCF, 0xffff, // 3538e: 0x0, //  [per awe specs]
									// 3538d was 0xffff
							PTAB_PITCHTARGET, 0,
							// fixme2 was IP, 0,
							REGLIST_END);
	}

	hw->voicetable[num].synth_id=0;
	hw->voicetable[num].unique=0;

        debug(DSTATE,"voice allocated (%d)\n",num);

	return num;
}

void kx_voice_free(kx_hw *hw,int voice_num)
{
	int i;
	unsigned long flags=0;

	if(voice_num<0 || voice_num>=KX_NUMBER_OF_VOICES)
	 return;

	if(hw->voicetable[voice_num].usage==VOICE_USAGE_FREE)
		return;

    if(hw->voicetable[voice_num].usage&VOICE_OPEN_NOTIFY)
    {
        kx_voice_irq_disable(hw,voice_num);
    }

	for(i = 0; i < ( hw->voicetable[voice_num].usage & VOICE_FLAGS_STEREO ? 2 : 1); i++)
	{
		kx_writeptr_multiple(hw, voice_num + i,   
							IFATN, IFATN_FILTERCUTOFF_MASK | IFATN_ATTENUATION_MASK,
                					DCYSUSV,0, // env. is off
							VTFT, 0xffff, // 3538e: 0x0, // 3538d was 0xffff
							CVCF, 0xffff, // 3538e: 0x0, // 3538d was 0xffff
							IP, 0,
							PTAB_PITCHTARGET, 0,
							CPF, 0,
							emuCCR,0,
							REGLIST_END);
	}

	kx_lock_acquire(hw,&hw->k_lock, &flags);

	if(hw->voicetable[voice_num].usage & VOICE_FLAGS_STEREO)
		 hw->voicetable[voice_num+1].usage = VOICE_USAGE_FREE;
	hw->voicetable[voice_num].usage = VOICE_USAGE_FREE;

	kx_lock_release(hw,&hw->k_lock, &flags);

        debug(DSTATE,"voice freed (%d)\n",voice_num);

	return;
}

struct note_info
{
 dword vol;
 int size;
 int ignore;
};

KX_API(int,kx_steal_voices(kx_hw *hw,dword usage,int flag))
{
 int found_voice=-1;
 struct note_info notes[KX_NUMBER_OF_VOICES];
 memset(notes,0,sizeof(notes));

 spinlock_t steal_lock;
 kx_spin_lock_init(hw,&steal_lock,"steal");
 unsigned long flags=0;
 kx_lock_acquire(hw,&steal_lock, &flags);

 for(int cnt=0;cnt<2;cnt++)
 {
   int start,end;

   if(cnt==0)
     { start=hw->last_voice; end=KX_NUMBER_OF_VOICES; }
   else
     { start=0; end=hw->last_voice; }

   for(int i=start;i<end;i++)
   {
    if((voice_usage(hw->voicetable[i].usage)==VOICE_USAGE_MIDI) ||
       (voice_usage(hw->voicetable[i].usage)==VOICE_USAGE_RELEASING))
    {
        notes[i].size=hw->voicetable[i].buffer.size;

        // Check if voice is terminated by 'stop on loop' condition (SOLEL/SOLEH)
        dword cpf=kx_readptr(hw,CPF,i);
        if(cpf&CPF_STOP_MASK)
        {
         if(usage&VOICE_FLAGS_STEREO) // but we need stereo...
         {
           kx_synth_term(hw,i);
           notes[i].ignore=1;
           continue;
           // free the voice, but continue searching..
         }
         // else:
         found_voice=i;
         break;
        }

        // Check if voice is looping silence
        if(hw->voicetable[i].param.sf2_params[54]==0) // no loops
        {
         dword CA = kx_calc_position(hw,i,kx_readptr(hw, QKBCA, i)&QKBCA_CURRADDR_MASK);
         if(CA > hw->voicetable[i].param.startloop) // loop is actually _after_ the sample
         {
          // found
          if(usage&VOICE_FLAGS_STEREO) // we require stereo
          {
   	       if((!(i&1)) && // if not even voice - continue
   	  	            ((voice_usage(hw->voicetable[i+1].usage)==VOICE_USAGE_MIDI) ||
                    (voice_usage(hw->voicetable[i+1].usage)==VOICE_USAGE_RELEASING))) 
           {
             // check the next voice, too
             if(hw->voicetable[i+1].param.sf2_params[54]==0) // no loops
             {
              CA = kx_calc_position(hw,i+1,kx_readptr(hw, QKBCA, i+1)&QKBCA_CURRADDR_MASK);
              if(CA > hw->voicetable[i+1].param.startloop) // loop is actually _after_ the sample
              {
               found_voice=i;
               break;
              }
             }
           } // not even... or the next is not good
           // terminate the voice -- for the future; but continue our search...
           kx_synth_term(hw,i);
           notes[i].ignore=1;
           continue;
          }
          else // mono
          {
            found_voice = i; // found & ok
	    break;
	  }
         }
        }

    	dword curvol;
    	
    	curvol=notes[i].vol=kx_readptr(hw, CVCF_CURRENTVOL, i);
    	dword dcysusv=kx_readptr(hw, DCYSUSV, i);
        /*
    	if(dcysusv==0xffff) // a very special case FIXME
    	{
    	 kx_synth_release(hw,i);
    	 notes[i].ignore=1;
    	 continue;
    	}
        */
        dword releasing=(dcysusv&DCYSUSV_PHASE1_MASK)?1:0;

   	if((curvol<0x200) && releasing)
   	{
   	  if(usage&VOICE_FLAGS_STEREO) // we require stereo
   	  {
   	  	// check the second voice-pair
   	  	if((!(i&1)) && // if not even voice - continue
   	  	   ((voice_usage(hw->voicetable[i+1].usage)==VOICE_USAGE_MIDI) ||
                    (voice_usage(hw->voicetable[i+1].usage)==VOICE_USAGE_RELEASING))) 
   	  	{
    			dword curvol2=notes[i+1].vol=kx_readptr(hw, CVCF_CURRENTVOL, i+1);
        		dword releasing2=(kx_readptr(hw,DCYSUSV,i+1)&DCYSUSV_PHASE1_MASK)?1:0;
        		if((curvol2<0x300)&&releasing2)
        		{
        		 found_voice=i;
        		 break;
        		}
   	  	}
   	  	if(curvol<0x100)
   	  	{
   	  	 kx_synth_term(hw,i);
   	  	 notes[i].ignore=1;
   	  	}
	  }
	  else // mono
	  {
   		// The volume is less than the threshold and
   		// the volume envelope is in the decaying state.
   		found_voice=i;
   		break;
   	  }	
   	}

/*
	// bad: else causes 'all sustained' to stay & 'all new' to be replaced on each 'note_on'...
	if((hw->voicetable[i].usage & VOICE_SYNTH_SUSTAINED)||
   	   (hw->voicetable[i].usage & VOICE_SYNTH_SOSTENUTO))
   	  curvol += 0x500;
*/

	if(!releasing)
   	 curvol+=0x1000;

   	notes[i].vol=curvol;
    } // if is midi or releasing
   } // for i
   if(found_voice!=-1)
    break;
 } // cnt

  if(found_voice!=-1) // it's ok to mute the voice only, not note
  {
  	kx_synth_term(hw,found_voice);
  	if(usage&VOICE_FLAGS_STEREO)
  	 kx_synth_term(hw,found_voice+1);
  	hw->last_voice=found_voice;

 	kx_lock_release(hw,&steal_lock, &flags);
  	return 0;
  }
  else
  {
  	if(flags&KX_STEAL_MAINTENANCE) // don't process maximums in this case...
  	{
  	 kx_lock_release(hw,&steal_lock, &flags);
  	 return -1;
  	}

  	// find maximums
      dword vol=0x7fffffff;
	  int i;

	for(i=0;i<KX_NUMBER_OF_VOICES;i++)
  	{
  	      if(hw->voicetable[i].synth_id &&
  	         !notes[i].ignore &&
  	         ((voice_usage(hw->voicetable[i].usage)==VOICE_USAGE_MIDI) ||
                  (voice_usage(hw->voicetable[i].usage)==VOICE_USAGE_RELEASING)
                 )
                )
  	      {
  		for(int j=i+1;j<KX_NUMBER_OF_VOICES;j++)
  		{
  		 if(hw->voicetable[j].synth_id &&
  		    !notes[j].ignore &&
   	            ((voice_usage(hw->voicetable[j].usage)==VOICE_USAGE_MIDI) ||
                     (voice_usage(hw->voicetable[j].usage)==VOICE_USAGE_RELEASING)
                    )
                   )
  		 {
  		   if( (hw->voicetable[i].note==hw->voicetable[j].note)&&
  		       (hw->voicetable[i].channel==hw->voicetable[j].channel)&&
  		       (hw->voicetable[i].synth_id==hw->voicetable[j].synth_id)&&
  		       (hw->voicetable[i].unique==hw->voicetable[j].unique) )
  		   {
  		    if(notes[i].vol>=notes[j].vol)
  		    {
  		     notes[j].ignore=1;
  		    }
  		    else
  		    {
  		     notes[i].ignore=1;
  		     break;
  		    }
  		   }
  		 }
  		}
                if(!notes[i].ignore)
                {
                 if(notes[i].vol<vol)
                 {
                   vol=notes[i].vol;
                   found_voice=i;
                 }
                }	
  	      }
  	}
	
        if(found_voice==-1)
        {
	     kx_lock_release(hw,&steal_lock, &flags);
         return -1;
        }

        // found_voice: minimum among maximums
        int note=hw->voicetable[found_voice].note;
        int chn=hw->voicetable[found_voice].channel;
        uintptr_t synth_id=hw->voicetable[found_voice].synth_id;
        dword unique=hw->voicetable[found_voice].unique;

        int min_voice=-1;
        int min_voice_sz=-1; // on 'size' criteria
        int max_size=0;
        vol=0x7fffffff;

        for(i=0;i<KX_NUMBER_OF_VOICES;i++)
        {
         if((hw->voicetable[i].channel==chn) &&
              (hw->voicetable[i].note==note) &&
              (hw->voicetable[i].synth_id==synth_id) &&
              (hw->voicetable[i].unique==unique))
         {
           if(notes[i].vol<vol)
           {
            if(flag&KX_STEAL_SIZE)
             if(notes[i].size>max_size)
             {
              max_size=notes[i].size;
              min_voice_sz=i;
             }

            min_voice=i;
            vol=notes[i].vol;
           }
         }
        }
        if((flag&KX_STEAL_SIZE) && (min_voice_sz!=-1))
        {
         // debug(DLIB,"note: stolen by size [%d / %d]\n",min_voice,min_voice_sz);
         min_voice=min_voice_sz;
        }

        if(min_voice==-1)
        {
         min_voice=found_voice;
         debug(DERR,"-- !! internal bug in synth/voice 763\n");
        }
        // min_voice: minimum among [found_voice]

        for(int n=0;n<KX_NUMBER_OF_VOICES;n++)
        {
          if( ((voice_usage(hw->voicetable[n].usage)==VOICE_USAGE_MIDI) ||
               (voice_usage(hw->voicetable[n].usage)==VOICE_USAGE_RELEASING)
              ) &&
              (hw->voicetable[n].channel==chn) &&
              (hw->voicetable[n].note==note) &&
              (hw->voicetable[n].synth_id==synth_id) &&
              (hw->voicetable[n].unique==unique)
              )
          {
          	if(n==min_voice)
          	{
        	 kx_synth_term(hw,n);

                 if(usage&VOICE_FLAGS_STEREO) // we require stereo
                 {
                  if(n&1) // odd
                   kx_synth_term(hw,min_voice-1);
                  else // even
                   kx_synth_term(hw,min_voice+1);
                 }
        	}
        	else
        	 kx_synth_release(hw,n);
          }
        }

        hw->last_voice=min_voice;
  }
  kx_lock_release(hw,&steal_lock, &flags);

  return 0;
}

