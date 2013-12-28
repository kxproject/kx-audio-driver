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

static const byte ac97_indexes[MAX_AC97_VALUES]=
{
AC97_REG_MASTER_VOL         ,    // line control
AC97_REG_SURROUND_DAC_VOL   ,
AC97_REG_MONO_VOL           ,    // play control
/*AC97_REG_MASTER_TONE*/0x8 ,
AC97_REG_PC_BEEP_VOL        ,
AC97_REG_PHONE_VOL          ,
AC97_REG_MIC_VOL            ,
AC97_REG_LINE_VOL           ,
AC97_REG_CD_VOL             ,
AC97_REG_VIDEO_VOL          ,
AC97_REG_AUX_VOL            ,
AC97_REG_PCM_VOL            ,

AC97_REG_REC_SELECT         ,
AC97_REG_REC_GAIN           ,
AC97_REG_REC_GAIN_MIC       ,
AC97_REG_GENERAL            ,
AC97_REG_3D_CTRL            ,
// AC97_REG_MODEM_RATE         0x24   // Reserved in 2.1?
AC97_REG_PWR_DWN_CTRL       ,
AC97_REG_PWR_DWN_STAT       ,

AC97_REG_EXTENDED_ID		,	/* Extended Audio ID */
//AC97_REG_EXTENDED_STATUS	0x2a	/* Extended Audio Status */
AC97_PCM_FRONT_DAC_RATE 	,	/* PCM Front DAC Rate */
//AC97_REG_PCM_SURR_DAC_RATE	0x2e	/* PCM Surround DAC Rate */
//AC97_REG_PCM_LFE_DAC_RATE	0x30	/* PCM LFE DAC Rate */
//AC97_REG_PCM_LR_DAC_RATE	0x32	/* PCM LR DAC Rate */
//AC97_REG_PCM_MIC_ADC_RATE	0x34	/* PCM MIC ADC Rate */
AC97_REG_CENTER_LFE_MASTER, /* Center + LFE Master Volume */
AC97_REG_SURROUND_MASTER	,	/* Surround (Rear) Master Volume */
AC97_REG_MULTICHANNEL_SELECT	,	// Vendor-specific: ST AC9708
0
};

int kx_state_save(kx_hw *hw)
{
 int i;

 hw->prev_state.mbs=kx_readptr(hw,MBS,0);
 hw->prev_state.mba=kx_readptr(hw,MBA,0);
 hw->prev_state.fxbs=kx_readptr(hw,FXBS,0);
 hw->prev_state.fxba=kx_readptr(hw,FXBA,0);
 hw->prev_state.adcbs=kx_readptr(hw,ADCBS,0);
 hw->prev_state.adcba=kx_readptr(hw,ADCBA,0);
 hw->prev_state.adcsr=kx_readptr(hw,ADCSR,0);

 hw->prev_state.scs0=kx_readptr(hw,SCS0,0);
 hw->prev_state.scs1=kx_readptr(hw,SCS1,0);
 hw->prev_state.scs2=kx_readptr(hw,SCS2,0);

 hw->prev_state.cliel=kx_readptr(hw,CLIEL,0);
 hw->prev_state.clieh=kx_readptr(hw,CLIEH,0);
 hw->prev_state.soll=kx_readptr(hw,SOLL,0);
 hw->prev_state.solh=kx_readptr(hw,SOLH,0);

 hw->prev_state.ptba=kx_readptr(hw,PTBA,0);
 hw->prev_state.trba=kx_readptr(hw,TRBA,0);
 hw->prev_state.trbs=kx_readptr(hw,TRBS,0);

 hw->prev_state.pcb=kx_readptr(hw,PCB,0);

 hw->prev_state.ehc=kx_readptr(hw,EHC,0);

 hw->prev_state.inte=kx_readfn0(hw,INTE);
 hw->prev_state.soc=kx_readptr(hw,SOC,0);
 hw->prev_state.hcfg_k1=kx_readfn0(hw,HCFG_K1);

 if(hw->is_10k2)
 {
  if(!hw->is_edsp)
   hw->prev_state.hcfg_k2=kx_readfn0(hw,HCFG_K2);

  hw->prev_state.fxwcl=kx_readptr(hw,FXWCL,0);
  hw->prev_state.fxwch=kx_readptr(hw,FXWCH,0);

  hw->prev_state.hliel=kx_readptr(hw,HLIEL,0);
  hw->prev_state.hlieh=kx_readptr(hw,HLIEH,0);
  hw->prev_state.spra=kx_readptr(hw,SPRA,0);
  hw->prev_state.sprc=kx_readptr(hw,SPRC,0);

  hw->prev_state.ttda=kx_readptr(hw,TTDA,0);
  hw->prev_state.ttdd=kx_readptr(hw,TTDD,0);
  hw->prev_state.ttb=kx_readptr(hw,TTB,0);
  hw->prev_state.tdof=kx_readptr(hw,TDOF,0);
  hw->prev_state.tblsz=kx_readptr(hw,TBLSZ,0);
 }
  else
   hw->prev_state.fxwc_k1=kx_readptr(hw,FXWC_K1,0);

 hw->prev_state.timer=(dword)kx_readfn0w(hw,TIMER);

 if(hw->is_10k2)
   hw->prev_state.dbg=kx_readptr(hw,DBG_10K2,0);
 else
   hw->prev_state.dbg=kx_readptr(hw,DBG_10K1,0);

 int total_mem;
 if(hw->is_10k2)
  total_mem=0xf00;
 else
  total_mem=0x800;

 for(i=0;i<total_mem;i++)
  hw->prev_state.dsp_data[i]=kx_readptr(hw,0x100+i,0);

 for(i = 0; i < KX_NUMBER_OF_VOICES; i++) 
 {
	 for(int j=0;j<0x30;j++)
		 hw->prev_state.per_voice[i][j]=kx_readptr(hw,j,i);
	 if(hw->is_10k2)
	 {
		 hw->prev_state.per_voice[i][0x30]=kx_readptr(hw,FXRT2_K2,i);
		 hw->prev_state.per_voice[i][0x31]=kx_readptr(hw,FXRT1_K2,i);
		 hw->prev_state.per_voice[i][0x32]=kx_readptr(hw,FXAMOUNT_K2,i);
	 }
 }

 if(hw->have_ac97)
 {
      i=0;
      while(1)
      {
        hw->prev_state.ac97_values[i]=kx_ac97read(hw,ac97_indexes[i]);
        i++;
        if(ac97_indexes[i]==0)
         break;
      }
 }

 if(hw->is_a2)
 {
   if(hw->is_k8)
   {
      for(i=p17vSRCSel;i<=p17vI2SR;i++)
      {
       if(i==p17vSoftResetSRPMixer)
        continue;
       hw->prev_state.p16v_values[i-p17vSRCSel]=kx_readp16v(hw,i,0);
      }
      hw->prev_state.recvl=kx_readp16v(hw,p17vRecVolL,0);
      hw->prev_state.recvh=kx_readp16v(hw,p17vRecVolH,0);
   }
   else
   {
      for(i=p16vSRCSel;i<=p16vAudOutEnb;i++)
       hw->prev_state.p16v_values[i-p16vSRCSel]=kx_readp16v(hw,i,0);

      hw->prev_state.recvl=kx_readp16v(hw,p16vRecVolL,0);
      hw->prev_state.recvh=kx_readp16v(hw,p16vRecVolH,0);

      hw->prev_state.recsel=kx_readp16v(hw,p16vRecSel,0);
   }
   
   hw->prev_state.sa=kx_readp16v(hw,p16vSA,0);
   hw->prev_state.phcfg=kx_readfn0(hw,pHCFG);
 }

 return 0;
}


int kx_state_restore(kx_hw *hw)
{
 int i;

 i=0;
 if(hw->have_ac97)
 {
  while(1)
  {
   kx_ac97write(hw,ac97_indexes[i],hw->prev_state.ac97_values[i]);
   i++;
   if(ac97_indexes[i]==0)
     break;
  }
 }

 kx_writeptr_multiple(hw, 0,
 			    PCB,hw->prev_state.pcb,
 			    EHC,(hw->is_a2?(hw->prev_state.ehc|EHC_SRCMULTI_I2S_96):hw->prev_state.ehc),
                            // first, set this bit (will clear later)

			    CLIEL, hw->prev_state.cliel,
			    CLIEH, hw->prev_state.clieh,
			    SOLL, hw->prev_state.soll,
			    SOLH, hw->prev_state.solh,

                            MBS, hw->prev_state.mbs,
                            MBA, hw->prev_state.mba,
                            FXBS, hw->prev_state.fxbs,
                            FXBA, hw->prev_state.fxba,
                            ADCBS, hw->prev_state.adcbs,
                            ADCBA, hw->prev_state.adcba,
                            ADCSR, hw->prev_state.adcsr,

			    REGLIST_END);

 if(hw->is_10k2)
 {
 	kx_writeptr_multiple(hw, 0,
 	   HLIEL, hw->prev_state.hliel,
           HLIEH, hw->prev_state.hlieh,
           SPRA, hw->prev_state.spra,
           SPRC,hw->prev_state.sprc,

           FXWCL,hw->prev_state.fxwcl,
           FXWCH,hw->prev_state.fxwch,

           TTDA,hw->prev_state.ttda,
           TTDD,hw->prev_state.ttdd,
           TTB,hw->prev_state.ttb,
           TDOF,hw->prev_state.tdof,
           TBLSZ,hw->prev_state.tblsz,

           REGLIST_END);
 }
 else
  kx_writeptr(hw,FXWC_K1,0,hw->prev_state.fxwc_k1);

 for(i = 0; i < KX_NUMBER_OF_VOICES; i++) 
 {
	 for(int j=0;j<0x30;j++)
		 kx_writeptr(hw,j,i,hw->prev_state.per_voice[i][j]);
	 
	 if(hw->is_10k2)
	 {
		 kx_writeptr(hw,FXRT2_K2,i,hw->prev_state.per_voice[i][0x30]);
		 kx_writeptr(hw,FXRT1_K2,i,hw->prev_state.per_voice[i][0x31]);
		 kx_writeptr(hw,FXAMOUNT_K2,i,hw->prev_state.per_voice[i][0x32]);
	 }
 }

 int total_mem;
 if(hw->is_10k2)
  total_mem=0xf00;
 else
  total_mem=0x800;
 for(i=0;i<total_mem;i++)
 {
  kx_writeptr(hw,0x100+i,0,hw->prev_state.dsp_data[i]);
 }

 kx_writeptr_multiple(hw, 0,
	SCS0, hw->prev_state.scs0,
	SCS1, hw->prev_state.scs1,
	SCS2, hw->prev_state.scs2,

 	PTBA,hw->prev_state.ptba,
 	TRBA,hw->prev_state.trba,
 	TRBS,hw->prev_state.trbs,
 	REGLIST_END);

 kx_writefn0(hw,HCFG_K1,hw->prev_state.hcfg_k1);

 if(hw->is_10k2 && !hw->is_edsp)
 {
  kx_writefn0(hw,HCFG_K2,hw->prev_state.hcfg_k2);
 }

 kx_writefn0w(hw,TIMER, (word)hw->prev_state.timer);
 kx_writefn0(hw,INTE, hw->prev_state.inte);

 if(hw->is_a2)
 {
   if(hw->is_k8)
   {
      for(i=p17vSRCSel;i<=p17vI2SR;i++)
      {
       if(i==p17vSoftResetSRPMixer)
        continue;
       kx_writep16v(hw,i,0,hw->prev_state.p16v_values[i-p17vSRCSel]);
      }

      kx_writep16v(hw,p17vRecVolL,0,hw->prev_state.recvl);
      kx_writep16v(hw,p17vRecVolH,0,hw->prev_state.recvh);
   }
   else
   {
      for(i=p16vSRCSel;i<=p16vAudOutEnb;i++)
       kx_writep16v(hw,i,0,hw->prev_state.p16v_values[i-p16vSRCSel]);

      kx_writep16v(hw,p16vRecVolL,0,hw->prev_state.recvl);
      kx_writep16v(hw,p16vRecVolH,0,hw->prev_state.recvh);

      kx_writep16v(hw,p16vRecSel,0,hw->prev_state.recsel);
   }

   kx_writep16v(hw,p16vSA,0,hw->prev_state.sa);
   kx_writefn0(hw,pHCFG,hw->prev_state.phcfg);

   kx_writeptr(hw,EHC,0,hw->prev_state.ehc); // set/clear EHC_SRCMULTI_I2S_96 now
 }

 kx_writeptr(hw, SOC, 0, hw->prev_state.soc);

 if(hw->is_10k2)
  kx_writeptr(hw,DBG_10K2,0,hw->prev_state.dbg);
 else
  kx_writeptr(hw,DBG_10K1,0,hw->prev_state.dbg);

 return 0;
}

