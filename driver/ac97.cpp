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

static int kx_ac97_state(kx_hw *hw);

static inline char itoax_s(dword v)
{
 if(v>=10)
 {
  return ((char)v-10)+'a';
 }
 else
  return (char)v+'0';
}

static inline void itoax(char *str,dword val)
{
 *str=itoax_s((val&0xf000)>>12); str++;
 *str=itoax_s((val&0xf00)>>8); str++;
 *str=itoax_s((val&0xf0)>>4); str++;
 *str=itoax_s((val&0xf)); str++;
 *str=0;
}

static int init_sigmatel_9744(kx_hw *hw)
{
	// patch for SigmaTel
	kx_ac97write(hw, AC97_SIGMATEL_CIC1, 0xabba);
	kx_ac97write(hw, AC97_SIGMATEL_CIC2, 0x0000);
	kx_ac97write(hw, AC97_SIGMATEL_BIAS1, 0xabba);
	kx_ac97write(hw, AC97_SIGMATEL_BIAS2, 0x0002);
	kx_ac97write(hw, AC97_REG_MULTICHANNEL_SELECT, 0x0000);
	return 0;
}

static int init_tritech_41x8(kx_hw *hw)
{
	debug(DLIB,"-- AC97 hardware [tritech 41x8] reset\n");
	kx_ac97write(hw, 0x26,3); // FIXME: @opensource: 0x300
	kx_ac97write(hw, 0x26,0);
	kx_ac97write(hw, AC97_REG_SURROUND_MASTER, 0); // @opensource only
	kx_ac97write(hw, 0x3a, 0); // @opensource only
	return 0;
}

static int init_emicro_28028(kx_hw *hw)
{
	debug(DLIB,"-- AC97 EMicro 28028 (fff0==4300) init [experimental]\n");
	kx_ac97write(hw, 0x7a,kx_ac97read(hw,0x7a)&0xfffd);
	return 0;
}

static int init_cl_591x(kx_hw *hw)
{
	debug(DLIB,"-- CirrusLogic &fff0==591x support [experimental]\n");
	kx_ac97write(hw,0x5e,0x80);
	return 0;
}

static int init_sigmatel_9708(kx_hw *hw)
{
	word word_72;
	word word_6c;

        init_tritech_41x8(hw); // FIXME: @creative: actually, only this step is done

        kx_ac97write(hw,AC97_SIGMATEL_DAC2INVERT, 0x0); // "Dac2Invert" // phase is inverted; 4to2 is off 

	word_72 = kx_ac97read(hw, AC97_SIGMATEL_BIAS2) & 0x8000;
	word_6c = kx_ac97read(hw, AC97_SIGMATEL_ANALOG);

	if((word_72==0) && (word_6c==0))
	{
		kx_ac97write(hw, AC97_SIGMATEL_CIC1, 0xabba);
		kx_ac97write(hw, AC97_SIGMATEL_CIC2, 0x1000);
		kx_ac97write(hw, AC97_SIGMATEL_BIAS1, 0xabba);
		kx_ac97write(hw, AC97_SIGMATEL_BIAS2, 0x0007);
	} else 
	  if((word_72==0x8000) && (word_6c==0))
	  {
		kx_ac97write(hw, AC97_SIGMATEL_CIC1, 0xabba);
		kx_ac97write(hw, AC97_SIGMATEL_CIC2, 0x1001);
		kx_ac97write(hw, AC97_SIGMATEL_DAC2INVERT, 0x0008);
	  } else 
	  if((word_72==0x8000) && (word_6c==0x0080)) 
	  {
		kx_ac97write(hw, AC97_SIGMATEL_CIC2, 0x8380); // FIXME
          }
	kx_ac97write(hw, AC97_REG_MULTICHANNEL_SELECT, 0x380 /* FIXME */);

	return 0;
}

static struct {
	dword id;
	dword mask;
	const char *name;
	int  (*init)  (kx_hw *hw);
} ac97_codec_ids[] = {
	{0x43525900, 0xffffffff, "Cirrus Logic CS4297"    , NULL},		// used 'crystal'
	{0x43525903, 0xffffffff, "Cirrus Logic CS4297r3"  ,	NULL},
        {0x43525910, 0xfffffff0, "Cirrus Logic CS4297A (&fff0==591x)"    , init_cl_591x}, // used 'crystal' (config); dest
	{0x43525913, 0xffffffff, "Cirrus Logic CS4297A"   , NULL},		// used
	{0x43525914, 0xffffffff, "Cirrus Logic (unknown)"   , NULL},		// used
	{0x43525923, 0xffffffff, "Cirrus Logic CS4298"    , NULL},
	{0x4352592B, 0xffffffff, "Cirrus Logic CS4294"    , NULL},
	{0x43525931, 0xffffffff, "Cirrus Logic CS4299"    , NULL},
	{0x43525934, 0xffffffff, "Cirrus Logic CS4299"    , NULL},
	{0x454D4328, 0xffffffff, "EMicro UNKNOWN (4328)", NULL },
	{0x454D4308, 0xffffff0f, "EMicro TOS28028 (ff0f)==4308"	      , init_tritech_41x8},		// used; (config)
        {0x454D4300, 0xfffffff0, "EMicro TOS28028 (fff0)==4300"	      , init_emicro_28028},		// used; (config)
	{0x54524100, 0xffffff00, "TriTech TR A4? (xx)"	      , NULL},
	{0x54524103, 0xffffffff, "TriTech TR28023/CT1297", init_tritech_41x8},
	{0x54524106, 0xffffffff, "TriTech TR28026"        , NULL},
	{0x54524108, 0xffffff0f, "TriTech TR28028 (41x8)"        , init_tritech_41x8},		// used(config)
	{0x54524123, 0xffffffff, "TriTech TR A5/CT1297 TAT"	      , NULL},		// used
	{0x83847600, 0xffffffff, "SigmaTel STAC unk"      , NULL},
	{0x83847604, 0xffffffff, "SigmaTel STAC9701/3/4/5", NULL},
	{0x83847605, 0xffffffff, "SigmaTel STAC9704"      , NULL},
	{0x83847608, 0xffffffff, "SigmaTel STAC9708",	init_sigmatel_9708}, // used(config)
	{0x83847609, 0xffffffff, "SigmaTel STAC9721/23",	NULL},
	{0x83847644, 0xffffffff, "SigmaTel STAC9744/45",	init_sigmatel_9744},
	{0x83847656, 0xffffffff, "SigmaTel STAC9756/57",	init_sigmatel_9744},
	{0x83847658, 0xffffffff, "SigmaTel STAC9758",	NULL}, // sb 22x
	{0x00000000, 0x00000000, NULL, NULL}
};

static const char *ac97_stereo_enhancements[] =
{
	// from the AC97 specification
	 "No 3D Stereo Enhancement",                       //   0
	 "Analog Devices Phat Stereo",                     //   1
	 "Creative Stereo Enhancement",                    //   2
	 "National Semi 3D Stereo Enhancement",            //   3
	 "YAMAHA Ymersion",                                //   4
	 "BBE 3D Stereo Enhancement",                      //   5
	 "Crystal Semi 3D Stereo Enhancement",             //   6
	 "Qsound QXpander",                                //   7
	 "Spatializer 3D Stereo Enhancement",              //   8
	 "SRS 3D Stereo Enhancement",                      //   9
	 "Platform Tech 3D Stereo Enhancement",            //  10
	 "AKM 3D Audio",                                   //  11
	 "Aureal Stereo Enhancement",                      //  12
	 "Aztech 3D Enhancement",                          //  13
	 "Binaura 3D Audio Enhancement",                   //  14
	 "ESS Technology Stereo Enhancement",              //  15
	 "Harman International VMAx",                      //  16
	 "Nvidea 3D Stereo Enhancement",                   //  17
	 "Philips Incredible Sound",                       //  18
	 "Texas Instruments 3D Stereo Enhancement",        //  19
	 "VLSI Technology 3D Stereo Enhancement",          //  20
	 "TriTech 3D Stereo Enhancement",                  //  21
	 "Realtek 3D Stereo Enhancement",                  //  22
	 "Samsung 3D Stereo Enhancement",                  //  23
	 "Wolfson Microelectronics 3D Enhancement",        //  24
	 "Delta Integration 3D Enhancement",               //  25
	 "SigmaTel 3D Enhancement",                        //  26
	 "Winbond 3D Stereo Enhancement",                  //  27
	 "Rockwell 3D Stereo Enhancement",                 //  28
	 "Reserved (29)",                                  //  29
	 "Reserved (30)",                                  //  30
	 "Reserved (31)"                                   //  31
};


const kx_ac97_registers_t kx_ac97_registers[]=
{
 { AC97_REG_MASTER_VOL,0x0,"MasterVolume" },		// 0x2
 { AC97_REG_MONO_VOL,0x801f,"TAD-Out" }, // TAD-Out: 8000 in 10k2's // 0x6
 { AC97_REG_PC_BEEP_VOL,0x801e,"PCBeep" }, // 0xA

 { AC97_REG_PHONE_VOL,0x8008,"TAD-In" }, // Tad-In 0xC
 { AC97_REG_MIC_VOL,0x8008,"Mic-In" }, // muted; w/o 20db boost 0xE
 { AC97_REG_LINE_VOL,0x8808,"Line-In" },  // 0x10
 { AC97_REG_CD_VOL,0x8808,"CD-In" },      // 0x12
 { AC97_REG_VIDEO_VOL,0x8808,"Video-In" }, // 8000 in 10k2's 0x14
 { AC97_REG_AUX_VOL,0x8808,"AUX-In" }, // 0x16

 { AC97_REG_PCM_VOL,0x0808,"PCM Vol" }, // 0x18 0dB attenuation

 { AC97_REG_REC_SELECT,0x404,"Rec Select" }, // 0x1a -- LineIn
 { AC97_REG_REC_GAIN,0x0,"Rec Gain" }, // 0x1c -- analog gain
 { AC97_REG_REC_GAIN_MIC,0x0,"Rec Mic Gain" }, // 0x1e
 { AC97_REG_GENERAL,0x200,"General" }, // Mic -> MonoOut; 3dSE disabled // 0x20
 { AC97_REG_3D_CTRL,0x0,"SE" },	// 3dSE disabled // 0x22
// { AC97_REG_PWR_DWN_CTRL,0xf,"Power" }, // 0x26
 { 0,0, NULL}
};


KX_API(word,kx_ac97read(kx_hw *hw,byte index))
{
 unsigned long flags=0;
 dword timeout=0x10000;

 if(!hw->have_ac97)
 {
  debug(DERR,"!!! AC97 access on non-ac97 card\n");
  return 0;
 }

 if(index&1)
 {
  debug(DERR,"!!! AC97 reg is x1 (%d)\n",index);
  return 0;
 }

 kx_lock_acquire(hw,&hw->ac97_lock, &flags);

 outp(hw->port+AC97ADDRESS,index);
 while((!(inp(hw->port+AC97ADDRESS)&AC97ADDRESS_READY))&&(timeout))
  timeout--;
 word val=inpw(hw->port+AC97DATA);

 kx_lock_release(hw,&hw->ac97_lock, &flags);

 return val;
}

KX_API(void,kx_ac97write(kx_hw *hw,byte index, word data))
{
 unsigned long flags=0;
 dword timeout=0x10000;

 if(index&1)
 {
  debug(DERR,"!!! AC97 reg is x1 (%d)\n",index);
  return;
 }

 if(!hw->have_ac97)
 {
  debug(DERR,"!!! AC97 access on non-ac97 card\n");
  return;
 }

 kx_lock_acquire(hw,&hw->ac97_lock, &flags);

 outp(hw->port+AC97ADDRESS,index);
 while((!(inp(hw->port+AC97ADDRESS)&AC97ADDRESS_READY))&&(timeout))
  timeout--;
 if(timeout)
  outpw(hw->port+AC97DATA,data);

 kx_lock_release(hw,&hw->ac97_lock,&flags);
}

int kx_ac97_init(kx_hw *hw)
{
 if(!hw->is_aps && !hw->is_edsp)
 {
   // hw->is_a2ex doesn't mean we don't have 'ac97'
   if(!hw->lack_ac97)
   {
        hw->have_ac97=1; // FIXME: really, check for bit AC97ADDRESS_READY @ AC97ADDRESS;

        // reset & init with default values
        kx_ac97reset(hw);
        return 0;
   }
   else
   {
    hw->have_ac97=0;
     return 0;
   }
 }
 else
 {
   debug(DERR,"!!! ac97_init() for APS/E-DSP\n");
   return -1;
 }
}

static int kx_ac97_state(kx_hw *hw)
{
 int i=0;
 while(1)
 {
   debug(DERR,"AC97 reg %x is %x (should be %x)\n",
    kx_ac97_registers[i].index,    
    kx_ac97read(hw,kx_ac97_registers[i].index),
    kx_ac97_registers[i].def_value);
  i++;
  if(kx_ac97_registers[i].index==0) break;
 }
 return 0;
}

int kx_ac97_close(kx_hw *hw)
{
 return 0;
}


KX_API(void,kx_ac97reset(kx_hw *hw))
{
 if(hw->have_ac97!=1)
  return;

 debug(DLIB,"ac97 init\n");

 int err=0;

 kx_ac97write(hw,AC97_REG_RESET,0);
 word cap = kx_ac97read(hw,AC97_REG_RESET);

 word id1=kx_ac97read(hw,AC97_REG_VENDOR_ID1);
 word id2=kx_ac97read(hw,AC97_REG_VENDOR_ID2);
 hw->ac97_id1=id1;
 hw->ac97_id2=id2;

 strncpy(hw->ac97_codec_name,"Unk AC97-codec [",KX_MAX_STRING);
 char tmp_str[16]; 
 itoax(tmp_str,id1);
 strncat(hw->ac97_codec_name,tmp_str,KX_MAX_STRING-strlen(hw->ac97_codec_name)-1);
 itoax(tmp_str,id2); 
 strncat(hw->ac97_codec_name,tmp_str,KX_MAX_STRING-strlen(hw->ac97_codec_name)-1);
 strncat(hw->ac97_codec_name,"]",KX_MAX_STRING-strlen(hw->ac97_codec_name)-1);

 debug(-1,"here it goes: '%s'\n",hw->ac97_codec_name);

 int i=0;
 while(1)
 {
    if(ac97_codec_ids[i].id==0) break;
    if((ac97_codec_ids[i].id&ac97_codec_ids[i].mask)==(dword)((id1 << 16) | id2)) 
    {
	my_strncpy(hw->ac97_codec_name,ac97_codec_ids[i].name,KX_MAX_STRING);
	if(ac97_codec_ids[i].init)
	 ac97_codec_ids[i].init(hw);
    }
    i++;
 }

 i=0;
 while(1)
 {
  kx_ac97write(hw,kx_ac97_registers[i].index,kx_ac97_registers[i].def_value);
  if(kx_ac97read(hw,kx_ac97_registers[i].index)!=kx_ac97_registers[i].def_value)
  {
   debug(DERR,"??? what's that? ac97 mismatch... (reg=%x; val=%x; really: %x)\n",
    kx_ac97_registers[i].index,kx_ac97_registers[i].def_value,
    kx_ac97read(hw,kx_ac97_registers[i].index));
   err++;
  }
  i++;
  if(kx_ac97_registers[i].index==0) break;
 }
 if(err)
  kx_ac97_state(hw);

 // now, set some vars
 word ac97_20;
 ac97_20 = kx_ac97read(hw, AC97_REG_EXTENDED_ID);
 hw->ac97_extid=ac97_20;

 if(ac97_20&0x0080) // has SurroundDAC
 {
  // power-up surdac
  kx_ac97write(hw,AC97_REG_EXTENDED_STATUS,kx_ac97read(hw,AC97_REG_EXTENDED_STATUS)&0xefff);
  // added 10/feb/2003

  if(!hw->has_surdac)
  {
   debug(DERR,"!!! AC97: SurroundDAC found. Card can be 5.1\n");
  }
  else
  {
   kx_ac97write(hw,AC97_REG_SURROUND_DAC_VOL,0x9f1f); // "SurrDAC" should be muted in SB0060 FIMXE? 0x4
   							// this is not exactly 'surdac', but
                                                        // probably, 'headphone volume' (unused)
   kx_ac97write(hw,AC97_REG_SURROUND_MASTER,0x0); // "SurrMaster" = center/lfe for sb0060 // 0x38
  }
 }
 else
 {
  if(hw->has_surdac)
  {
   hw->has_surdac=0;
   debug(DERR,"!!! AC97: Card has NO SurroundDAC, but is marked as '5.1' -> disabled\n");
  }
  kx_ac97write(hw,AC97_REG_SURROUND_MASTER,0x8080);
 }

 ac97_20 &= ~((1<<2)|(1<<4)|(1<<5)|(1<<10)|(1<<11)|(1<<12)|(1<<13));
 hw->ac97_is_20=ac97_20?(byte)1:0;
 if(hw->ac97_is_20)
 {
  kx_ac97write(hw,AC97_REG_PWR_DWN_CTRL,0xf); // power on
 }

 hw->ac97_cap=cap;
 my_strncpy(hw->ac97_3d_name,ac97_stereo_enhancements[(cap >> 10) & 0x1f],KX_MAX_STRING);

 // enable 5.1 support
 if(hw->is_51)
 {
  if(hw->is_10k2) // audigy?
   kx_writeptr(hw, PCB, 0, PCB_10K2);
  else
  {
   int need_hack=0;

   if((dword)(((dword)hw->ac97_id1 << 16) | (dword)hw->ac97_id2)==0x83847658)
   {
    if( (hw->pci_subsys==0x80661102) || (hw->pci_subsys==0x100a1102) )
     need_hack=1;
   }

   if(hw->ext_flags&KX_HW_SB22x)
    need_hack=1-need_hack;
   
   if(need_hack)
   {
    kx_writeptr(hw, PCB, 0, PCB_CNTR|PCB_LFE|PCB_10K2); // sb22x with ac97 codec instead of i2s rear
    kx_ac97write(hw,AC97_REG_CENTER_LFE_MASTER,0x0); // enable center+lfe -> rear
    hw->is_bad_sb22x=1;
   }
   else
   {
    kx_writeptr(hw, PCB, 0, PCB_CNTR|PCB_LFE);
    hw->is_bad_sb22x=0;
   }
  }
 }
 if(hw->is_10k2)
 {
  kx_ac97write(hw,AC97_REG_PCM_VOL,0x9f1f); // mute ac97 PCM
  kx_ac97write(hw,AC97_REG_MASTER_VOL,0x9f1f); // mute audigy master; note: init() mutes/unmutes it,too
  kx_ac97write(hw,AC97_REG_REC_SELECT,0x0505); // stereomix is default
 }
}

