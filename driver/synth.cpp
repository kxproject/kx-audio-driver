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

#define LIMITVALUE(x, a, b) do { if((x) < (a)) (x) = (a); else if((x) > (b)) (x) = (b); } while (0)

// Math Calculators
// ---------------------------------------

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
static inline int timecent_to_msec(int timecent) // correct FIXME: optimize
{
	return (int)(1000 * kx_pow2((double)timecent / 1200.0));
}

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
static inline int abscent_to_mHz(int abscents)
{
	return (int)(8176.0 * kx_pow2((double)abscents / 1200.0));
}

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
// search an index for specified time from given time table
static inline int calc_parm_search(int msec, int *table)
{
	int left = 1, right = 127, mid;
	while(left < right)
	{
		mid = (left + right) / 2;
		if(msec < (int)table[mid])
			left = mid + 1;
		else
			right = mid;
	}
	return left;
}


#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
static inline dword calc_filterQ(int Q) // correct, checked; input: in cB
{
 Q = Q/12;

 LIMITVALUE(Q,0,15);

 return Q;
}


#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
static inline int calc_delay_ms(int ms) // envvol / envval; cannot be calculated; quite close to APS
{
	LIMITVALUE(ms,0,21000);

	return (0x8000 - ms * 1000 / 666); // alsa: /725 - AWE timing
}

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
static inline int calc_delay(int amount) // envvol / envval; cannot be calculated; quite close to APS
{
	return calc_delay_ms(timecent_to_msec(amount));
}

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
static inline int calc_hold_ms(int hldmsec,kx_hw *hw) // very close; correct (APS)
{
	int up;

	if(hw->synth_compat&KX_SYNTH_COMPAT_HOLD)
	 up = (int)((0x7f * 882 - hldmsec*10) / 882); // awe: /92; btw, perhaps it is 84.5?..
	else
	 up = (int)((0x7f * 875 - hldmsec*10) / 875);

        // fixme: error is +/- 1

	LIMITVALUE(up,1,127);

	return up;
}

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
static inline int calc_hold(int amount,int note,int key2hold,kx_hw *hw) // very close; correct (APS)
{
 if(amount<=-12000)
  return 0x7f;

 if(key2hold)
   amount+=(note-60)*(-key2hold);

 return calc_hold_ms(timecent_to_msec(amount),hw);
}

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
static inline int calc_sustain(int amount,int max_=1000) // max is always 1000 (if specs are ok)
{
	// pre-3533: int val = 127 - (amount * 127) / max_;
	int val = (max_-amount)*10/75;

	LIMITVALUE(val,0,127);
	return val;
}


// attack & decay/release time table (msec)
static int attack_time_tbl_new[128] = {
32767, 5656, 3999, 2828, 2378, 2000, 1682, 1414, 1297, 1189, 1091, 1000, 917, 841, 771, 649,
622, 595, 570, 546, 523, 501, 479, 459, 439, 421, 403, 386, 370, 354, 339, 325,
311, 298, 285, 273, 262, 251, 240, 230, 220, 211, 202, 193, 185, 177, 170, 163,
156, 149, 143, 137, 131, 126, 120, 115, 110, 106, 101,  97,  93,  89,  85,  83,
78, 75, 72, 69, 66, 63, 60, 58, 55, 53, 51, 49, 47, 45, 43, 41,
39, 38, 36, 35, 33, 32, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21,
20, 19, 18, 18, 17, 16, 15, 15, 14, 14, 13, 13, 12, 12, 11, 11,
10, 10, 9,  9,  9,  8,  8,  8,  7,  7,  7,  7,  6,  6,  6,  5
};

static int attack_time_tbl_old[128] = {
32767, 32767, 5989, 4235, 2994, 2518, 2117, 1780, 1497, 1373, 1259, 1154, 1058, 970, 890, 816,
707, 691, 662, 634, 607, 581, 557, 533, 510, 489, 468, 448, 429, 411, 393, 377,
361, 345, 331, 317, 303, 290, 278, 266, 255, 244, 234, 224, 214, 205, 196, 188,
180, 172, 165, 158, 151, 145, 139, 133, 127, 122, 117, 112, 107, 102, 98, 94,
90, 86, 82, 79, 75, 72, 69, 66, 63, 61, 58, 56, 53, 51, 49, 47,
45, 43, 41, 39, 37, 36, 34, 33, 31, 30, 29, 28, 26, 25, 24, 23,
22, 21, 20, 19, 19, 18, 17, 16, 16, 15, 15, 14, 13, 13, 12, 12,
11, 11, 10, 10, 10, 9, 9, 8, 8, 8, 8, 7, 7, 7, 6, 0,
};

static int decay_time_tbl_new[128] = {
41487, 20744, 14668, 10372, 8722, 7334,6186, 5186, 4756, 4361, 3999, 3667, 3363, 3084, 2828, 2595,
2593, 2483, 2378, 2277, 2181, 2088, 2000, 1915, 1834, 1756, 1682, 1611, 1542, 1477, 1414, 1354,
1297, 1242, 1189, 1139, 1091, 1044, 958,  918 , 879,  842,  806,  772,  739,  708,  678,  649,
622,  595,  570,  546,  523,  501,  479,  459,  440,  421,  403,  386,  370,  354, 339, 325,
311, 298, 285, 273, 262, 251, 240, 230, 220, 211, 202, 193, 185, 177, 170, 163,
156, 149, 143, 137, 131, 126, 120, 115, 110, 106, 101,  97,  93,  89,  85,  82,
78, 75, 72, 69, 66, 63, 60, 58, 55, 53, 51, 49, 47, 45, 43, 41,
39, 38, 36, 35, 33, 32, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21
};

static int decay_time_tbl_old[128] = {
32767, 32767, 22614, 15990, 11307, 9508, 7995, 6723, 5653, 5184, 4754, 4359, 3997, 3665, 3361, 3082,
2828, 2765, 2648, 2535, 2428, 2325, 2226, 2132, 2042, 1955, 1872, 1793, 1717, 1644, 1574, 1507,
1443, 1382, 1324, 1267, 1214, 1162, 1113, 1066, 978, 936, 897, 859, 822, 787, 754, 722,
691, 662, 634, 607, 581, 557, 533, 510, 489, 468, 448, 429, 411, 393, 377, 361,
345, 331, 317, 303, 290, 278, 266, 255, 244, 234, 224, 214, 205, 196, 188, 180,
172, 165, 158, 151, 145, 139, 133, 127, 122, 117, 112, 107, 102, 98, 94, 90,
86, 82, 79, 75, 72, 69, 66, 63, 61, 58, 56, 53, 51, 49, 47, 45,
43, 41, 39, 37, 36, 34, 33, 31, 30, 29, 28, 26, 25, 24, 23, 22,
};


// decay/release time: search from time table
#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
static inline int calc_decay_ms(int amount,kx_hw *hw) // checked; quite close to APS
{
 if(hw->synth_compat&KX_SYNTH_COMPAT_LEG_REL)
  return calc_parm_search(amount, decay_time_tbl_old)&0x7f;
 else
  return calc_parm_search(amount, decay_time_tbl_new)&0x7f;
}

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
static inline int calc_release_ms(int amount,kx_hw *hw) // checked; quite close to APS
{
 if(hw->synth_compat&KX_SYNTH_COMPAT_LEG_REL)
  return calc_parm_search(amount, decay_time_tbl_old)&0x7f;
 else
  return calc_parm_search(amount, decay_time_tbl_new)&0x7f;
}

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
static inline int calc_decay(int amount,int note,int note2decay,kx_hw *hw)
{
 if(amount<=-12000)
  return 0x7f;
 return calc_decay_ms(timecent_to_msec((note2decay==0)?amount:(note-60)*(-note2decay)+amount),hw);
}

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
static inline int calc_release(int amount,kx_hw *hw)
{
 if(amount<=-12000)
  return 0x7f;
 return calc_release_ms(timecent_to_msec(amount),hw);
}

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
static inline int calc_127_1200_6(int p)
{
 p=p*127/(1200*6);
 if(p>127)
  p=127;
 return p;
}

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
static inline int calc_127_1200(int p)
{
 p=p*127/1200;
 if(p>127)
  p=127;
 return p;
}

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
// attack time: search from time table
static inline int calc_attack_ms(int amount,kx_hw *hw)
{
 int ret;

 if(hw->synth_compat&KX_SYNTH_COMPAT_LEG_ATT)
	ret=calc_parm_search(amount, attack_time_tbl_old);
 else	
 	ret=calc_parm_search(amount, attack_time_tbl_new);
 return ret;
}

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
static inline int calc_attack(int amount,kx_hw *hw)
{
 if(amount<=-12000)
  return 0x80; // minimum

 return calc_attack_ms(timecent_to_msec(amount),hw);
}

// MIDI/SF2 Calculators
// ---------------------------------------

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
static inline void recalc_reverb(kx_hw *hw,int num,kx_midi_state *midi,int chn)
{
       for(int i=0;i<(hw->voicetable[num].usage & VOICE_FLAGS_STEREO ? 2 : 1);i++) // in any case;
       {
       dword rev=hw->voicetable[num].param.sf2_params[16]+midi->channels[chn].nrpn_sf_data[16];

       // 8.4.8 MIDI Continuous Controller 91 to Reverb Effects Send
       // Amount = 200 tenths of a percent

       // int rv=((int)hw->cb.def_amount[DEF_SYNTH_AMOUNT_C])*(rev*255/1000+midi->channels[chn].reverb*255/127)/255;
       // int rv=rev*midi->channels[chn].reverb*(hw->cb.def_amount[DEF_SYNTH_AMOUNT_C])/1000/127+(int)hw->cb.def_amount[DEF_SYNTH_AMOUNT_C]/10;
       // int rv=rev*midi->channels[chn].reverb/500+(int)hw->cb.def_amount[DEF_SYNTH_AMOUNT_C];
       int rv=((rev>>2)+(midi->channels[chn].reverb*50/127))*(int)hw->cb.def_amount[DEF_SYNTH_AMOUNT_C]/255;

       if(midi->channels[chn].nrpn_awe_data[26]!=AWE_UNUSED)
        rv=midi->channels[chn].nrpn_awe_data[26];

       LIMITVALUE(rv,0,255);

       rev=(dword)rv;
              
       hw->voicetable[num+i].param.send_c=rev;

       }
}

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
static inline void recalc_chorus(kx_hw *hw,int num,kx_midi_state *midi,int chn)
{
       for(int i=0;i<(hw->voicetable[num].usage & VOICE_FLAGS_STEREO ? 2 : 1);i++) // in any case;
       {
       dword cho=hw->voicetable[num].param.sf2_params[15]+midi->channels[chn].nrpn_sf_data[15];

       // int ch=((int)hw->cb.def_amount[DEF_SYNTH_AMOUNT_D])*(cho*255/1000+midi->channels[chn].chorus*255/127)/255;
       // int ch=cho*midi->channels[chn].chorus*(hw->cb.def_amount[DEF_SYNTH_AMOUNT_D])/1000/127+(int)hw->cb.def_amount[DEF_SYNTH_AMOUNT_D]/10;
       // int ch=cho*midi->channels[chn].chorus/500+(int)hw->cb.def_amount[DEF_SYNTH_AMOUNT_D];

       // 8.4.9 MIDI Continuous Controller 93 to Chorus Effects Send
       // Amount = 200 tenths of a percent

       int ch=((cho>>2)+(midi->channels[chn].chorus*50/127))*(int)hw->cb.def_amount[DEF_SYNTH_AMOUNT_D]/255;

       if(midi->channels[chn].nrpn_awe_data[25]!=AWE_UNUSED)
        ch=midi->channels[chn].nrpn_awe_data[25];

       LIMITVALUE(ch,0,255);

       cho=(dword)ch;

       hw->voicetable[num+i].param.send_d=cho;
       }
}

static int vol2db_table[127]=
{
710, 
420, 360, 325, 300, 280, 265, 251, 240, 
229, 220, 212, 204, 197, 191, 185, 179, 
174, 169, 165, 160, 156, 152, 148, 144, 
141, 137, 134, 131, 128, 125, 122, 119, 
117, 114, 111, 109, 107, 104, 102, 100, 
98, 96, 94, 92, 90, 88, 86, 84, 
82, 80, 79, 77, 75, 74, 72, 71, 
69, 68, 66, 65, 63, 62, 60, 59, 
58, 56, 55, 54, 52, 51, 50, 49, 
48, 46, 45, 44, 43, 42, 41, 40, 
39, 37, 36, 35, 34, 33, 32, 31, 
30, 29, 28, 28, 27, 26, 25, 24, 
23, 22, 21, 20, 19, 19, 18, 17, 
16, 15, 14, 14, 13, 12, 11, 10, 
10, 9, 8, 7, 7, 6, 5, 4, 
4, 3, 2, 2, 1, 0
};

// calculate volume attenuation
#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
static inline void recalc_volume(kx_hw *hw,int num,kx_midi_state *midi,int chn,int vel)
{
	int vol;

	int velocity=(hw->voicetable[num].param.sf2_params[47]==-1)?vel:
         (hw->voicetable[num].param.sf2_params[47]);

        // 8.4.1 MIDI Note-On Velocity to Initial Attenuation [960cB]
        // 8.4.5 MIDI Continuous Controller 7 to Initial Attenuation [960cB]
        // 8.4.7 MIDI Continuous Controller 11 to Initial Attenuation [960cB]

	vol = (velocity * midi->channels[chn].vol * midi->channels[chn].expression) / (127*127);

	// calc to attenuation (linear->log)
	LIMITVALUE(vol,0,127);

	vol=vol2db_table[vol]; // (int)(-20.0f*10.0f*kx_log10((float)vol/127));
        int isyn=((int)(hw->synth_vol[0])/0x10000*10);

	if(hw->synth_compat&KX_SYNTH_COMPAT_ATTN)
	{
	 // adjust volumes
	 vol=vol*(384)/(0x92); // * (100 / 375) / (0x92/1440)
	 isyn=isyn*(384)/(0x92);
	}

        int iattn=hw->voicetable[num].param.sf2_params[48]+midi->channels[chn].nrpn_sf_data[48];
        if(iattn<0)
         iattn=0;

        iattn=iattn-isyn+vol; // fixme: mono volume?..

        if(iattn<=0)
         iattn=0x0;
        else
        {
         if(hw->synth_compat&KX_SYNTH_COMPAT_ATTN)
          iattn = iattn * 0x92 / 1440; // 0.986dB step or 0.0986 cB step
         else
          iattn = iattn * 100 / 375; // 0.375dB step or 0.0375 cB step
          
          // was: *0x92/1440
          // creative uses 0x92/1440! // 0.98dB step
          // FIXME NOW
        }
        LIMITVALUE(iattn,0,255);

	hw->voicetable[num].param.initial_attn=iattn;

	// (((CTDWORD)(0x10 | (~iv & 0x0F)) << (((~iv >> 4) & 0x0F) + 12)) & 0xFFFF0000L)
	hw->voicetable[num].param.volume_target=(((dword)(0x10 | ((dword)~iattn & 0x0F)) << ((((dword)~iattn >> 4) & 0x0F) + 12)) & 0xFFFF0000);

    // 3543 new scheme:
    if(hw->is_10k2)
    {
     if(hw->voicetable[num].param.volume_target==0xffff)
      hw->voicetable[num].param.volume_target=0x8000;
     else
      hw->voicetable[num].param.volume_target>>=1;
    }

	hw->voicetable[num].param.initial_cv=hw->voicetable[num].param.volume_target;
}

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
static inline void recalc_cutoff(kx_hw *hw,int num,kx_midi_state *midi,int chn)
{
   int velocity=(hw->voicetable[num].param.sf2_params[47]==-1)?hw->voicetable[num].vel:
    (hw->voicetable[num].param.sf2_params[47]);

   // Initial filter cutoff (FC) (correct) (checked)
   int fc=hw->voicetable[num].param.sf2_params[8]+midi->channels[chn].nrpn_sf_data[8];

   if(midi->channels[chn].nrpn_awe_data[21]!=AWE_UNUSED)
   {
     // translate Hz -> absolute cents...
     // unit: 62Hz; 100..8000Hz
     // abs cent-> mHz: (int)(8176.0 * kx_pow2((double)abscents / 1200.0))
     // mHz -> abs cent: log2(hz/8.176)*1200.0

     if(midi->channels[chn].nrpn_awe_data[21]!=0)
       fc=(int)(kx_log10(((float)midi->channels[chn].nrpn_awe_data[21]*62.0f+100.0f)/8.176f)*3986.313713864834f);
       // 3986.313713864834: (1/log10(2))*1200
   }

   if(hw->synth_compat&KX_SYNTH_COMPAT_842)
   {
    if(velocity<64) // 8.4.2 MIDI Note-On Velocity to Filter Cutoff [-2400 cents]
     // fc=fc-(127-velocity)*2400/127;
     fc=fc-(64-velocity)*2400/64;
     // cl aud2ex doesn't use it
     // FIXME
   }

   LIMITVALUE(fc,1500,13500);

   fc = (fc+0xf)/0x1d-0x99;
   // 8.176*(1.000577790 [2^(cents(e.g.13500)/1200)] pow(cents)) //

   LIMITVALUE(fc,0,0xff);

   if(midi->channels[chn].soft_pedal==1) // on
     fc-=0x9e;

   fc+=(midi->channels[chn].sound_brightness-64)*4;

   LIMITVALUE(fc,0,0xff);

   hw->voicetable[num].param.initial_fc=fc;
}

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
static inline void recalc_hold2(kx_hw *hw,int num,kx_midi_state *midi,int chn)
{
   int volrel_add=(midi->channels[chn].sound_release-64)*2400/64; // 2400 timecents = 4 secs
   int modrel_add=0;

   if(midi->channels[chn].hold_2==1) // on
   {
    volrel_add+=1900; // 1900 timecents = 3 secs
    modrel_add+=1900;
   }

   // ModRelease:
   if(midi->channels[chn].nrpn_awe_data[9]==AWE_UNUSED)
    hw->voicetable[num].param.modrelease=calc_release(midi->channels[chn].nrpn_sf_data[30]+hw->voicetable[num].param.sf2_params[30]+modrel_add,hw); // same as decay
   else
    hw->voicetable[num].param.modrelease=calc_release_ms(midi->channels[chn].nrpn_awe_data[9]*4,hw);

   // VolRelease:
   if(midi->channels[chn].nrpn_awe_data[15]==AWE_UNUSED)
     hw->voicetable[num].param.volrelease=calc_release(midi->channels[chn].nrpn_sf_data[38]+hw->voicetable[num].param.sf2_params[38]+volrel_add,hw); // same as decay
   else
    hw->voicetable[num].param.volrelease=calc_release_ms(midi->channels[chn].nrpn_awe_data[15]*4,hw);

   LIMITVALUE(hw->voicetable[num].param.volrelease,0,0x7f);
   LIMITVALUE(hw->voicetable[num].param.modrelease,0,0x7f);
}

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
static inline void recalc_pan(kx_hw *hw,int num,kx_midi_state *midi,int chn)
{
       if(hw->voicetable[num].usage&VOICE_FLAGS_STEREO)
       {
       		debug(DLIB,"!! pan not supported for stereo sources FIXME\n");
       }
       else
       {
         int sf_pan=hw->voicetable[num].param.sf2_params[17]+midi->channels[chn].nrpn_sf_data[17];
         // 8.4.6 MIDI Continuous Controller 10 to Pan Position
         // Amount = 1000 tenths of a percent (!! per Vienna: 500!)

         // sf_pan=-500...500
         LIMITVALUE(sf_pan,-500,500);

         int midi_pan=0;
         if(midi->channels[chn].pan<64) // left: range is 0..63 -> 64 values
          midi_pan=-(64-midi->channels[chn].pan)*500/64;
         if(midi->channels[chn].pan>64) // right: range is 65..127 -> 63 values
          midi_pan=(midi->channels[chn].pan-64)*500/63;

         midi_pan+=sf_pan;
         LIMITVALUE(midi_pan,-500,500);

         int send_a,send_b;
         if(midi_pan<=0)
         {
          send_a=128-midi_pan*127/500;
          send_b=128+midi_pan*128/500;
         }
         else
         {
          send_a=128-midi_pan*128/500;
          send_b=128+midi_pan*127/500;
         }
//         debug(DWDM,"pan: %x %x\n",send_a,send_b);

/*         if(hw->voicetable[num].param.sample_type&rightSample) // right sample
          send_a=0;
         if(hw->voicetable[num].param.sample_type&leftSample) // left sample
          send_b=0;

         debug(DWDM,"pan after l&r: %x %x\n",send_a,send_b);
*/
         // process balance:
         if(midi->channels[chn].balance<64) // left
          send_b=send_b*midi->channels[chn].balance/64;
         if(midi->channels[chn].balance>64) // right
          send_a=send_a*(127-midi->channels[chn].balance)/63;

         hw->voicetable[num].param.send_a=(dword)send_a;
         hw->voicetable[num].param.send_b=(dword)send_b;
       }
}

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
static inline void recalc_modulation(kx_hw *hw,int num,kx_midi_state *midi,int chn)
{
 // FMMOD
 {
 int modlfo2pitch;
 if(midi->channels[chn].nrpn_awe_data[17]==AWE_UNUSED)
 {
  modlfo2pitch=calc_127_1200(hw->voicetable[num].param.sf2_params[5]+midi->channels[chn].nrpn_sf_data[5]);
 }
 else
  modlfo2pitch=midi->channels[chn].nrpn_awe_data[17];

  if(!(hw->synth_compat&KX_SYNTH_COMPAT_LEG_MOD))
  {
    // 8.4.4 MIDI Continuous Controller 1 to Vibrato LFO Pitch Depth [50 cents / max excursion]
    // modlfo2pitch += calc_127_1200(50 * midi->channels[chn].modulation / 127);

    // 8.4.3 MIDI Channel Pressure to Vibrato LFO Pitch Depth [50 cents / max excursion]
    // modlfo2pitch += calc_127_1200(50 * midi->channels[chn].chpressure / 127);

    // do nothing
  }
  else
  {
    modlfo2pitch += calc_127_1200(25 * midi->channels[chn].modulation / 127);
    modlfo2pitch += calc_127_1200(25 * midi->channels[chn].chpressure / 127);
  }

  LIMITVALUE(modlfo2pitch, -128, 127);
  hw->voicetable[num].param.modlfo2pitch=modlfo2pitch;
 }

 {
 int modlfo2fc;
 if(midi->channels[chn].nrpn_awe_data[23]==AWE_UNUSED)
 {
   modlfo2fc=hw->voicetable[num].param.sf2_params[10]+midi->channels[chn].nrpn_sf_data[10];
   modlfo2fc=modlfo2fc*127/(1200*3);
 }
 else
  modlfo2fc=midi->channels[chn].nrpn_awe_data[23]*2;

 LIMITVALUE(modlfo2fc, -128, 127);

 hw->voicetable[num].param.modlfo2fc=modlfo2fc;
 }

 // FM2FRQ2
 {
 int viblfo2pitch;
 if(midi->channels[chn].nrpn_awe_data[18]==AWE_UNUSED)
 {
   viblfo2pitch=calc_127_1200(hw->voicetable[num].param.sf2_params[6]+midi->channels[chn].nrpn_sf_data[6]);
 }
 else
  viblfo2pitch=midi->channels[chn].nrpn_awe_data[18]; // 3538m?
 
 if(!(hw->synth_compat&KX_SYNTH_COMPAT_LEG_MOD))
 {
    // Signed 2's complement, +/- one octave extremes

    // 8.4.4 MIDI Continuous Controller 1 to Vibrato LFO Pitch Depth [50 cents / max excursion]
    viblfo2pitch += calc_127_1200(50 * midi->channels[chn].modulation / 127);

    // 8.4.3 MIDI Channel Pressure to Vibrato LFO Pitch Depth [50 cents / max excursion]
    viblfo2pitch += calc_127_1200(50 * midi->channels[chn].chpressure / 127);

    // was: (50 * midi->channels[chn].modulation) / 1200;
    // (same)
 }

 LIMITVALUE(viblfo2pitch, -128, 127);

 hw->voicetable[num].param.viblfo2pitch=viblfo2pitch;
 }

 int divisor=42;
 if(!(hw->synth_compat&KX_SYNTH_COMPAT_FREQ))
  divisor=39;
   
 {
 int viblfofreq;
 if(midi->channels[chn].nrpn_awe_data[3]==AWE_UNUSED)
 {
  viblfofreq=hw->voicetable[num].param.sf2_params[24]+midi->channels[chn].nrpn_sf_data[24];

  viblfofreq=abscent_to_mHz(viblfofreq)/divisor; // *0.039Hz step awe: 0.042 step
  // note: creative uses 0.042?
  // fixme
 }
 else
  viblfofreq=midi->channels[chn].nrpn_awe_data[3]*2;

 LIMITVALUE(viblfofreq, 0, 255);

 hw->voicetable[num].param.viblfofreq=viblfofreq;
 }

 // TREMFRQ
 {
 int modlfofreq;
 if(midi->channels[chn].nrpn_awe_data[1]==AWE_UNUSED)
 {
  modlfofreq=hw->voicetable[num].param.sf2_params[22]+midi->channels[chn].nrpn_sf_data[22];
  modlfofreq=abscent_to_mHz(modlfofreq)/divisor; // *0.039Hz step awe: 0.042 step
  // note: creative uses 0.042?
  // FIXME
 }
 else
  modlfofreq=midi->channels[chn].nrpn_awe_data[1]*2;

 LIMITVALUE(modlfofreq, 0, 255);
 hw->voicetable[num].param.modlfofreq=modlfofreq;
 }

 {
 int modlfo2vol;
 if(midi->channels[chn].nrpn_awe_data[20]==AWE_UNUSED)
 {
  modlfo2vol=hw->voicetable[num].param.sf2_params[13]+midi->channels[chn].nrpn_sf_data[13];
  if(hw->synth_compat&KX_SYNTH_COMPAT_LFOVOL)
   modlfo2vol=modlfo2vol*127/120; // +/- 12dB range; input is in cB
  // note: creative doesn't correct +/- 12dB range, assuming +/- 12.7 dB range
  // fixme
 }
 else
  modlfo2vol=midi->channels[chn].nrpn_awe_data[20]*2;

 LIMITVALUE(modlfo2vol, -128, 127);

 hw->voicetable[num].param.modlfo2vol=modlfo2vol;
 }
}

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
static inline void recalc_filter(kx_hw *hw,int num,kx_midi_state *midi,int chn)
{
 // PEFE
 // modenv2pitch
 if(midi->channels[chn].nrpn_awe_data[19]==AWE_UNUSED)
   hw->voicetable[num].param.pitch_amount=calc_127_1200(hw->voicetable[num].param.sf2_params[7]+midi->channels[chn].nrpn_sf_data[7]);
 else
   hw->voicetable[num].param.pitch_amount=midi->channels[chn].nrpn_awe_data[19];

 // modenv2fc
 if(midi->channels[chn].nrpn_awe_data[24]==AWE_UNUSED)
  hw->voicetable[num].param.filter_amount=calc_127_1200_6(hw->voicetable[num].param.sf2_params[11]+midi->channels[chn].nrpn_sf_data[11]);
 else
  hw->voicetable[num].param.filter_amount=midi->channels[chn].nrpn_awe_data[24];
}

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
static inline void recalc_filterQ(kx_hw *hw,int num,kx_midi_state *midi,int chn)
{
 // FilterQ (correct) (checked)
 if(midi->channels[chn].nrpn_awe_data[22]==AWE_UNUSED)
   hw->voicetable[num].param.filterQ=calc_filterQ(hw->voicetable[num].param.sf2_params[9]+midi->channels[chn].nrpn_sf_data[9]);
 else
   hw->voicetable[num].param.filterQ=midi->channels[chn].nrpn_awe_data[22]/8;

 int tmp=(int)hw->voicetable[num].param.filterQ+(midi->channels[chn].resonance-64)/4;

 LIMITVALUE(tmp,0,15);

 hw->voicetable[num].param.filterQ=(dword)tmp;
}

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
static inline void recalc_sf(kx_hw *hw,int num,kx_midi_state *midi,int chn,int note)
{
 // LFO1 Delay(Mod) 0x8000-666n (usec); sf: timecents
 // (correct)
 if(midi->channels[chn].nrpn_awe_data[0]==AWE_UNUSED)
  hw->voicetable[num].param.lfo1delay=calc_delay(hw->voicetable[num].param.sf2_params[21]+midi->channels[chn].nrpn_sf_data[21]);
 else
  hw->voicetable[num].param.lfo1delay=calc_delay_ms(midi->channels[chn].nrpn_awe_data[0]*4);

 // LFO2 Delay(Vib): correct
 if(midi->channels[chn].nrpn_awe_data[2]==AWE_UNUSED)
  hw->voicetable[num].param.lfo2delay=calc_delay(hw->voicetable[num].param.sf2_params[23]+midi->channels[chn].nrpn_sf_data[23]);
 else
  hw->voicetable[num].param.lfo2delay=calc_delay_ms(midi->channels[chn].nrpn_awe_data[2]*4);

 // ModDelay 0x8000-666n (usec); sf: timecents correct
 if(midi->channels[chn].nrpn_awe_data[4]==AWE_UNUSED)
  hw->voicetable[num].param.moddelay=calc_delay(hw->voicetable[num].param.sf2_params[25]+midi->channels[chn].nrpn_sf_data[25]);
 else
  hw->voicetable[num].param.moddelay=calc_delay_ms(midi->channels[chn].nrpn_awe_data[4]*4);

 // VolDelay: correct
 if(midi->channels[chn].nrpn_awe_data[10]==AWE_UNUSED)
  hw->voicetable[num].param.voldelay=calc_delay(hw->voicetable[num].param.sf2_params[33]+midi->channels[chn].nrpn_sf_data[33]);
 else
  hw->voicetable[num].param.voldelay=calc_delay_ms(midi->channels[chn].nrpn_awe_data[10]*4);

 // ModAttack (wrong) & ModHold (correct,checked)
 if(midi->channels[chn].legato)
 {
   hw->voicetable[num].param.modatk=0xff; // minimum
 }
 else
 {
  if(midi->channels[chn].nrpn_awe_data[5]==AWE_UNUSED)
   hw->voicetable[num].param.modatk=calc_attack(hw->voicetable[num].param.sf2_params[26]+midi->channels[chn].nrpn_sf_data[26],hw);
  else
   hw->voicetable[num].param.modatk=calc_attack_ms(midi->channels[chn].nrpn_awe_data[5],hw);

  // 3538c
  if(hw->voicetable[num].param.modatk>=0x80)
  {
   hw->voicetable[num].param.modatk=0xff; // minimum
  }
  else
  {
   LIMITVALUE(hw->voicetable[num].param.modatk,0,127);
  }
 }

 if(midi->channels[chn].nrpn_awe_data[6]==AWE_UNUSED)
  hw->voicetable[num].param.modhld=calc_hold(hw->voicetable[num].param.sf2_params[27]+midi->channels[chn].nrpn_sf_data[27],
                                  note,
                                  hw->voicetable[num].param.sf2_params[31]+midi->channels[chn].nrpn_sf_data[31],hw);
 else
  hw->voicetable[num].param.modhld=calc_hold_ms(midi->channels[chn].nrpn_awe_data[6],hw);

 // VolAttack (???) & VolHold (correct,checked)

 if(midi->channels[chn].legato)
 {
   hw->voicetable[num].param.volatk=0xff; // minimum
 }
 else
 {
  if(midi->channels[chn].nrpn_awe_data[11]==AWE_UNUSED)
   hw->voicetable[num].param.volatk=calc_attack(hw->voicetable[num].param.sf2_params[34]+midi->channels[chn].nrpn_sf_data[34],hw);
  else
   hw->voicetable[num].param.volatk=calc_attack_ms(midi->channels[chn].nrpn_awe_data[11],hw);

  // sound_attack
  hw->voicetable[num].param.volatk+=256+(64-midi->channels[chn].sound_attack);
  if(hw->voicetable[num].param.volatk<256)
   hw->voicetable[num].param.volatk=0;
  else
   hw->voicetable[num].param.volatk-=256;
  // sound_attack

  if(hw->voicetable[num].param.volatk>=0x80)
  {
   hw->voicetable[num].param.volatk=0xff; // minimum
  }
  else
  {
    LIMITVALUE(hw->voicetable[num].param.volatk, 0, 127);
  }
 }

 if(midi->channels[chn].nrpn_awe_data[12]==AWE_UNUSED)
  hw->voicetable[num].param.volhld=calc_hold(hw->voicetable[num].param.sf2_params[35]+midi->channels[chn].nrpn_sf_data[35],
                                  note,
                                  hw->voicetable[num].param.sf2_params[39]+midi->channels[chn].nrpn_sf_data[39],hw);
 else
  hw->voicetable[num].param.volhld=calc_hold_ms(midi->channels[chn].nrpn_awe_data[12],hw);

 // ModSustain: in -0.1% - correct, checked
 if(midi->channels[chn].nrpn_awe_data[8]==AWE_UNUSED)
  hw->voicetable[num].param.modsustain=calc_sustain(hw->voicetable[num].param.sf2_params[29]+midi->channels[chn].nrpn_sf_data[29],1000);
 else
  hw->voicetable[num].param.modsustain=midi->channels[chn].nrpn_awe_data[8];
  

 // VolSustain: in cB attn - correct,checked
 if(midi->channels[chn].nrpn_awe_data[14]==AWE_UNUSED)
  hw->voicetable[num].param.volsustain=calc_sustain(hw->voicetable[num].param.sf2_params[37]+midi->channels[chn].nrpn_sf_data[37],1000);
 else
  hw->voicetable[num].param.volsustain=midi->channels[chn].nrpn_awe_data[14];

 // ModDecay:
 if(midi->channels[chn].nrpn_awe_data[7]==AWE_UNUSED)
  hw->voicetable[num].param.moddecay=calc_decay(hw->voicetable[num].param.sf2_params[28]+midi->channels[chn].nrpn_sf_data[28],
                                note,
                                hw->voicetable[num].param.sf2_params[32]+midi->channels[chn].nrpn_sf_data[32],hw);
 else
  hw->voicetable[num].param.moddecay=calc_decay_ms(midi->channels[chn].nrpn_awe_data[7]*4,hw);

 // VolDecay:
 if(midi->channels[chn].nrpn_awe_data[13]==AWE_UNUSED)
  hw->voicetable[num].param.voldecay=calc_decay(hw->voicetable[num].param.sf2_params[36]+midi->channels[chn].nrpn_sf_data[36],
                                note,
                                hw->voicetable[num].param.sf2_params[40]+midi->channels[chn].nrpn_sf_data[40],hw)
                                |DCYSUSV_CHANNELENABLE_MASK;
 else
  hw->voicetable[num].param.voldecay=calc_decay_ms(midi->channels[chn].nrpn_awe_data[13]*4,hw)|DCYSUSV_CHANNELENABLE_MASK;
}

static inline void recalc_gp(kx_hw *hw,int num,kx_midi_state *midi,int chn)
{
 for(int i=0;i<(hw->voicetable[num].usage&VOICE_FLAGS_STEREO?2:1);i++)
 {
       hw->voicetable[num+i].param.send_e=midi->channels[chn].gp1+hw->cb.def_amount[DEF_SYNTH_AMOUNT_E];
       hw->voicetable[num+i].param.send_f=midi->channels[chn].gp2+hw->cb.def_amount[DEF_SYNTH_AMOUNT_F];
       hw->voicetable[num+i].param.send_g=midi->channels[chn].gp3+hw->cb.def_amount[DEF_SYNTH_AMOUNT_G];
       hw->voicetable[num+i].param.send_h=midi->channels[chn].gp4+hw->cb.def_amount[DEF_SYNTH_AMOUNT_H];
 }
}

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
static inline void recalc_pitch(kx_hw *hw,int num,kx_midi_state *midi,int chn)
{
 dword ip;
 ip=hw->voicetable[num].param.sf2_pitch;

 // NRPN
 if(midi->channels[chn].nrpn_awe_data[16]!=AWE_UNUSED)
 {
  ip=midi->channels[chn].nrpn_awe_data[16]*4096/1200;
 }
 // MIDI pitch bend / sense
 ip += (midi->channels[chn].pitch-0x2000) * midi->channels[chn].pb_sensivity / 0x6000;

 // 8.4.10 MIDI Pitch Wheel to Initial Pitch Controlled by MIDI Pitch Wheel Sensitivity
 // The amount of this modulator is 12700 Cents.

 // GM RPNs
 // master fine is in 100/8192 cents [8192 = 100 cents]
 // coarse is in semitones
 ip+=(4096*((midi->channels[chn].master_fine_tuning-0x2000)*1000/8192+
            (midi->channels[chn].master_coarse_tuning-64)*1000)/12000);

 hw->voicetable[num].param.initial_pitch=ip;

 hw->voicetable[num].param.pitch_target=(dword)kx_pow2((float)ip/(float)0x1000);
}

// ----------------------------------------------------------------------------------------------
// iterators
// ----------------------------------------------------------------------------------------------
#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
KX_API(int,kx_synth_changes(kx_midi_state *midi,int chn,int note,int what))
{
 kx_hw *hw=midi->hw;
 int i;

 for(i=0;i<KX_NUMBER_OF_VOICES;i++)
 {
   if((voice_usage(hw->voicetable[i].usage)==VOICE_USAGE_MIDI) &&
        (hw->voicetable[i].channel==chn) &&
        ((note==-1) || (hw->voicetable[i].note==note)) &&
        (hw->voicetable[i].synth_id==(uintptr_t)midi))
   {
        switch(what)
        {
         case CHANGE_MODULATION: //  chpressure & modulation
         	{
         	kx_fpu_state state;
		 hw->cb.save_fpu_state(&state);
        	 recalc_modulation(hw,i,midi,chn);
        	hw->cb.rest_fpu_state(&state);
         	 kx_writeptr_multiple(hw,i,
         	   FMMOD,(hw->voicetable[i].param.modlfo2fc&0xff)|((hw->voicetable[i].param.modlfo2pitch&0xff)<<8),
                   FM2FRQ2,(hw->voicetable[i].param.viblfofreq&0xff)|((hw->voicetable[i].param.viblfo2pitch&0xff)<<8),
                   TREMFRQ, (hw->voicetable[i].param.modlfofreq&0xff)|((hw->voicetable[i].param.modlfo2vol&0xff)<<8),
                   REGLIST_END);
                }
         	break;
         case CHANGE_PITCHBEND: 
         	{
         	kx_fpu_state state;
		 hw->cb.save_fpu_state(&state);
                  recalc_pitch(hw,i,midi,chn);
                 hw->cb.rest_fpu_state(&state);

         	kx_writeptr_multiple(hw,i,
         	 PTAB_PITCHTARGET,hw->voicetable[i].param.pitch_target,
         	 IP,hw->voicetable[i].param.initial_pitch,
         	 REGLIST_END);

                // FIXME: we could use set_audio_parameter(KX_VOICE_PITCH+KX_VOICE_UPDATE);
                //  -- performs realtime QKBCA/Interprom update
                }
         	break;
         case CHANGE_VOLUME:
         	recalc_volume(hw,i,midi,chn,hw->voicetable[i].vel);
                kx_writeptr(hw, IFATN_ATTENUATION, i, hw->voicetable[i].param.initial_attn);
                // fixme?? do we need to set cvcf/vtft when envvol==0xbfff?
         	break;
         case CHANGE_PAN:
         	recalc_pan(hw,i,midi,chn);
         	kx_wave_set_parameter(hw,i,0,KX_VOICE_PAN+KX_VOICE_UPDATE,0);
         	break;
         case CHANGE_REVERB:
         	recalc_reverb(hw,i,midi,chn);
         	kx_wave_set_parameter(hw,i,0,KX_VOICE_SEND_C+KX_VOICE_UPDATE,0);
         	break;
         case CHANGE_CHORUS:
         	recalc_chorus(hw,i,midi,chn);
         	kx_wave_set_parameter(hw,i,0,KX_VOICE_SEND_D+KX_VOICE_UPDATE,0);
         	break;
         case CHANGE_SOFT_PEDAL:
         	{
		 kx_fpu_state state;
		 hw->cb.save_fpu_state(&state);
		  recalc_cutoff(hw,i,midi,chn);
                 hw->cb.rest_fpu_state(&state);

                 if(hw->synth_compat&KX_SYNTH_COMPAT_REAL_FC) 
                  kx_writeptr(hw,IFATN_FILTERCUTOFF,i,hw->voicetable[i].param.initial_fc);
                }
                break;
         case CHANGE_PEFE:
         	recalc_filter(hw,i,midi,chn);
         	kx_writeptr_multiple(hw,i,
				// filter envelope 
				    PEFE_FILTERAMOUNT, hw->voicetable[i].param.filter_amount,
				// pitch envelope 
				    PEFE_PITCHAMOUNT, hw->voicetable[i].param.pitch_amount,
				    REGLIST_END);
         	break;
         case CHANGE_FILTERQ:
         	recalc_filterQ(hw,i,midi,chn);

         	{
         	 if(hw->synth_compat&KX_SYNTH_COMPAT_REAL_RESONANCE)
         	 {
         	 if(hw->is_10k2)
         	 {
        		 dword regptr;
        		 unsigned long flags=0;

        		 regptr = (((QKBCA) << 16) & PTR_ADDRESS_MASK) | (i & PTR_CHANNELNUM_MASK);

        		 hw->voicetable[i].param.filterQ&=0xf;

        		 kx_lock_acquire(hw,&hw->hw_lock, &flags);

        		 outpd(hw->port + PTR,regptr);

        		 byte old=inp(hw->port + DATA + 3);

        		 old=(byte)((old&0x0f)|(hw->voicetable[i].param.filterQ<<4));
        		 outp(hw->port + DATA + 3,old);

        		 kx_lock_release(hw,&hw->hw_lock, &flags);
        	  }
        	  else
        	  {
        		 kx_writeptr(hw, QKBCA_RESONANCEQ, i, hw->voicetable[i].param.filterQ);
        	  }
        	 }
		}
         	break;
         case CHANGE_HOLD2:
         	{
		kx_fpu_state state;
		 hw->cb.save_fpu_state(&state);
         	 recalc_hold2(hw,i,midi,chn);
         	hw->cb.rest_fpu_state(&state);
                // cannot be changed realtime...
                }
         	break;
         case CHANGE_GP1:
         	recalc_gp(hw,i,midi,chn);
         	kx_wave_set_parameter(hw,i,0,KX_VOICE_SEND_E+KX_VOICE_UPDATE,0);
         	break;
         case CHANGE_GP2:
         	recalc_gp(hw,i,midi,chn);
         	kx_wave_set_parameter(hw,i,0,KX_VOICE_SEND_F+KX_VOICE_UPDATE,0);
         	break;
         case CHANGE_GP3:
         	recalc_gp(hw,i,midi,chn);
         	kx_wave_set_parameter(hw,i,0,KX_VOICE_SEND_G+KX_VOICE_UPDATE,0);
         	break;
         case CHANGE_GP4:
         	recalc_gp(hw,i,midi,chn);
         	kx_wave_set_parameter(hw,i,0,KX_VOICE_SEND_H+KX_VOICE_UPDATE,0);
         	break;
         default:
         	debug(DLIB,"!!! Unknown 'what' in synth_changes (%d)!\n",what);
         	break;
        }
   }
 }
 return 0;
}

// #define REPARSE_SF

#ifdef REPARSE_SF
 KX_API(void,kx_synth_parse(kx_voice_param *param));
#endif

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
KX_API(int,kx_synth_start(kx_midi_state *midi,int chn,int note,int vel))
{
 dword fl1=0,fl2=0;
 int cnt=0;
 kx_hw *hw=midi->hw;

 debug(DMIDI,"[%d/%d:%d] on [%d %d] chn=%d\n",
 				   midi->channels[chn].bank_lsb,
 				   midi->channels[chn].bank_msb,
                                   midi->channels[chn].program,
				   note,
 			           vel,
 			           chn);

 kx_voice_param *table=0;
 hw->cb.malloc_func(hw->cb.call_with,sizeof(kx_voice_param)*KX_MAX_SF_VOICES,(void **)&table,KX_NONPAGED);
 if(!table)
 {
  debug(DLIB,"synth_start: no more memory\n");
  return 0;
 }
 cnt=sf_find_voice(midi,table,chn,note,vel);

 if(cnt<=0)
 {
  debug(DSFNT,"-- no SF data for this voice\n");
  (hw->cb.free_func)(hw->cb.call_with,table);
  return 0;
 }

 dword unique=(dword)((dword)kx_readfn0(hw,WC_SAMPLECOUNTER)^(dword)(uintptr_t)midi);

 kx_fpu_state state;
 hw->cb.save_fpu_state(&state);

 for(int j=0; j<cnt;j++)
 {
       // find...
       int num=kx_voice_alloc(hw,table[j].sf2_usage);
       if(num==-1)
       {
        debug(DLIB,"kx_synth_start: no free hw voices\n");
        continue;
       }

       // fill non-param variables
       hw->voicetable[num].state=VOICE_STATE_STOPPED;
       hw->voicetable[num].hw_pos=0;
       hw->voicetable[num].total_played=0;

       hw->voicetable[num].channel=chn;
       hw->voicetable[num].vel=(byte)vel;
       hw->voicetable[num].note=note;
       hw->voicetable[num].synth_id=(uintptr_t)midi;

       int bytespersample=((table[j].sf2_usage&VOICE_FLAGS_STEREO?2:1) *
       		    (table[j].sf2_usage&VOICE_FLAGS_16BIT?2:1));
       hw->voicetable[num].bytespersample=bytespersample;

       memcpy(&hw->voicetable[num].param,&table[j],sizeof(kx_voice_param));

       hw->voicetable[num].param.send_routings=hw->cb.def_routings[(midi->synth_num?DEF_SYNTH2_ROUTING:DEF_SYNTH1_ROUTING)+chn];
       hw->voicetable[num].param.send_xroutings=hw->cb.def_xroutings[(midi->synth_num?DEF_SYNTH2_ROUTING:DEF_SYNTH1_ROUTING)+chn];

       // recalculate parameters according to current midi state: vol, reverb, etc
       recalc_reverb(hw,num,midi,chn);
       recalc_chorus(hw,num,midi,chn);
       recalc_volume(hw,num,midi,chn,vel);
       recalc_pan(hw,num,midi,chn);
       recalc_modulation(hw,num,midi,chn);
       recalc_pitch(hw,num,midi,chn);
       recalc_sf(hw,num,midi,chn,note);
       recalc_filter(hw,num,midi,chn);
       recalc_filterQ(hw,num,midi,chn);
       recalc_hold2(hw,num,midi,chn); // hold2
       recalc_cutoff(hw,num,midi,chn); // soft pedal / cutoff
       recalc_gp(hw,num,midi,chn);

       debug(DSFNT," -- hw note[%d]: vol=%x usage=%x\n",num,hw->voicetable[num].param.volume_target,
        table[j].sf2_usage//hw->voicetable[num].usage
        );

       // voice may have already been mapped [checked by 'kx_alloc_buffer']
       // FIXME NOW !! 3543
       // #error and here...
       hw->voicetable[num].buffer.pageindex=-1;
       hw->voicetable[num].buffer.size=hw->voicetable[num].param.tmp_buffer_size;
       hw->voicetable[num].buffer.physical=hw->voicetable[num].param.tmp_buffer_physical;
       hw->voicetable[num].buffer.addr=hw->voicetable[num].param.tmp_buffer_addr;
       hw->voicetable[num].buffer.notify=10;
       hw->voicetable[num].buffer.that=0;
       hw->voicetable[num].buffer.instance=0;

       if(kx_alloc_buffer(hw,num))
       {
        debug(DLIB,"!! kx_synth_start: error allocating buffer for voice: size=%d phy:%x addr: %x\n",
         hw->voicetable[num].buffer.size,hw->voicetable[num].buffer.physical,
         hw->voicetable[num].buffer.addr);
        kx_voice_free(hw,num);
        continue;
       }

       // startloop & endloop & start were relative to soundfont's sample_data address space
       hw->voicetable[num].param.startloop += (hw->voicetable[num].buffer.pageindex << 12) / bytespersample;
       hw->voicetable[num].param.endloop += (hw->voicetable[num].buffer.pageindex << 12) / bytespersample;
       hw->voicetable[num].param.start += (hw->voicetable[num].buffer.pageindex << 12) / bytespersample;

       #ifdef REPARSE_SF
        kx_synth_parse(&hw->voicetable[num].param);
       #endif

       kx_setup_playback(hw,num);

       if(num>=32)
        fl2|=(1<<(num-32));
       else
        fl1|=(1<<num);

       my_memset(&hw->voicetable[num].timer,0,sizeof(kx_timer));
       hw->voicetable[num].timer.status=TIMER_UNINSTALLED;

       // process exclusive_class
       if(hw->voicetable[num].param.sf2_params[57])
       {
        for(int i=0;i<KX_NUMBER_OF_VOICES;i++)
        {
              if(
         	  ((voice_usage(hw->voicetable[i].usage)==VOICE_USAGE_MIDI) ||
         	   (voice_usage(hw->voicetable[i].usage)==VOICE_USAGE_RELEASING))
         	    &&
		  (hw->voicetable[i].channel==chn) &&
         	  (hw->voicetable[i].synth_id==(uintptr_t)midi) &&
         	  (i!=num) &&
         	  (hw->voicetable[i].param.sf2_params[57]==hw->voicetable[num].param.sf2_params[57])
                )
         	 kx_synth_term(midi,i);
        }
       }
       hw->voicetable[num].unique=unique;

       hw->voicetable[num].usage=table[j].sf2_usage|VOICE_USAGE_TEMP; // finalize usage
       if(table[j].sf2_usage&VOICE_FLAGS_STEREO)
        hw->voicetable[num+1].usage=table[j].sf2_usage|VOICE_USAGE_TEMP;
 }

 hw->cb.rest_fpu_state(&state);

 kx_voice_start_multiple(hw,fl1,fl2);

 for(int i=0;i<KX_NUMBER_OF_VOICES;i++)
  if((hw->voicetable[i].usage&VOICE_USAGE_TEMP) && (hw->voicetable[i].unique==unique))
  {
   hw->voicetable[i].usage&=~(VOICE_USAGE_TEMP);
  }

 (hw->cb.free_func)(hw->cb.call_with,table);

 return 0;
}

KX_API(int,kx_synth_release(kx_midi_state *midi,int ch))
{
 kx_hw *hw=midi->hw;
 return kx_synth_release(hw,ch);
}

KX_API(int,kx_synth_release(kx_hw *hw,int ch))
{

 if(ch<0 || ch>=KX_NUMBER_OF_VOICES)
 {
  debug(DLIB,"!!! internal error: ch<0 | ch>63 in kx_synth_release (%d)\n",ch);
  return -2;
 }

 if(voice_usage(hw->voicetable[ch].usage)==VOICE_USAGE_FREE)
 {
  debug(DLIB,"!!! internal error: usage==free in kx_synth_release (%d)\n",hw->voicetable[ch].usage);
  return -1;
 }

 if(hw->voicetable[ch].param.sf2_params[54]==3) // loop until key released
 {
  hw->voicetable[ch].param.sf2_params[54]=0;

  dword end_addr=hw->voicetable[ch].param.end;

  if(hw->is_10k2)
  {
   hw->voicetable[ch].param.startloop=end_addr-28; // pre-3538g: was just end_addr
   hw->voicetable[ch].param.endloop=end_addr-28; // pre-3538g: was just end_addr
  }
  else
  {
   hw->voicetable[ch].param.startloop=end_addr-32; // pre-3538g: was just end_addr
   hw->voicetable[ch].param.endloop=end_addr-32; // pre-3538g: was just end_addr
  }

  /*
  // pre-3536b:
  // fixme: should check, if sample is bi-directional etc
  hw->voicetable[ch].param.startloop=end_addr+8/2;
  hw->voicetable[ch].param.endloop=end_addr+40/2;
  hw->voicetable[ch].param.end+=46/2;
  */

  kx_writeptr_multiple(hw,ch,
    SCSA_LOOPSTARTADDR,hw->voicetable[ch].param.startloop,
    SDL_LOOPENDADDR,hw->voicetable[ch].param.endloop,
    REGLIST_END);

  kx_voice_stop_on_loop_enable(hw, ch);
  if(hw->voicetable[ch].usage & VOICE_FLAGS_STEREO)
    kx_voice_stop_on_loop_enable(hw, ch + 1);
 }

 dword dcysusm = DCYSUSM_PHASE1_MASK | hw->voicetable[ch].param.modrelease;
 dword dcysusv = DCYSUSV_PHASE1_MASK | (hw->voicetable[ch].param.volrelease&0x7f) | DCYSUSV_CHANNELENABLE_MASK;

 kx_writeptr_multiple(hw, ch,
  DCYSUSM, dcysusm,
  DCYSUSV, dcysusv,
  REGLIST_END);

 unsigned long flags=0;
 kx_lock_acquire(hw,&hw->k_lock, &flags);

 if(hw->voicetable[ch].usage & VOICE_FLAGS_STEREO)
 	 hw->voicetable[ch+1].usage = VOICE_USAGE_RELEASING;
 hw->voicetable[ch].usage = VOICE_USAGE_RELEASING;

 kx_lock_release(hw,&hw->k_lock, &flags);

 return 0;
}

KX_API(int,kx_synth_term(kx_midi_state *midi,int ch))
{
 return kx_synth_term(midi->hw,ch);
}

KX_API(int,kx_synth_term(kx_hw *hw,int ch))
{
 if(ch<0 || ch>=KX_NUMBER_OF_VOICES)
  return -2;

 kx_voice_stop(hw,ch);

 kx_voice_free(hw,ch);

 kx_free_buffer(hw,ch);

 return 0;
}
