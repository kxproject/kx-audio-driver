// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov and Max Mikhailov, 2001-2005.
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

#include "stdafx.h"
#include "interface/idane.h"

// --------------- the above should be a 'standard' beginning of any .cpp file

// all effect classes:
#include "autowah.h"
#include "flanger.h"
#include "3dsound.h"
#include "apsfuzz.h"
#include "EQ_Bandpass.h"
#include "EQ_Highpass.h"
#include "EQ_Highshelf.h"
#include "EQ_Lowpass.h"
#include "EQ_Lowshelf.h"
#include "EQ_Notch.h"
#include "EQ_Peaking.h"
#include "Freq_Splitter.h"
#include "apscomp.h"
#include "apscompsc.h"
#include "Phat_EQ_Mono.h"
#include "Phat_EQ_Stereo.h"
#include "wavegen.h"
#include "everb.h"
#include "decimator.h"
#include "amp.h"
#include "gain.h"
#include "wavegen2.h"
#include "pitch.h"
#include "crossfade.h"
#include "stmix.h"
#include "mx6.h"
#include "apsexp.h"
#include "apsexp_plus.h"
#include "wavegen3.h"
#include "dynamica.h"
#include "NoiseGate2T.h"
#include "NoiseGate2Ts.h"
#include "cleax4reverb.h"
#include "info.h"
#include "Cross_2nd.h"
#include "Cross_4th.h"
#include "osc.h"
#include "TheDelay.h"
#include "TheSmallDelay.h"
#include "TimeBalanceV2.h"
#include "Phaser.h"
#include "morebass.h"
#include "ReverbEax2.h"
// total: 43 .cpp-based effects

// include 'pseudo'-plugins
#include "da_encode4.cpp"
#include "da_overdrive.cpp"
#include "da_clreverb.cpp"
#include "da_prologica.cpp"
#include "da_ringmod.cpp"
#include "da_overdrive2.cpp"
#include "da_vocoder.cpp"
#include "da_stvocoder.cpp"
#include "da_voldc.cpp"
#include "da_ts.cpp"
#include "da_monomix.cpp"
#include "da_stereomix.cpp"
#include "da_b2b.cpp"
#include "da_b2bv2.cpp"
#include "da_downmix.cpp"
#include "da_cnv51to2.cpp"
#include "da_16to32.cpp"
#include "da_ac3passthrough.cpp"
#include "da_ac3passthru.cpp"
#include "da_surrounderlt.cpp"
#include "da_cleax3reverb.cpp"
#include "da_hphsp.cpp"
#include "da_hphsp2.cpp"
#include "da_Pos3DFX.cpp"
#include "da_ac3passthru_x.cpp"
#include "da_xor.cpp"
#include "da_booblegum.cpp"
#include "da_vibrato.cpp"
#include "da_HarmonicsGen.cpp"
#include "da_agc.cpp"
#include "da_Dither.cpp"
#include "da_Feedback_Destroyer.cpp"
#include "da_Leslie.cpp"
#include "da_Prologic.cpp"
#include "da_ProPhaser.cpp"
#include "da_Sputnik.cpp"
#include "da_tremolo.cpp"
#include "da_virtual5_1.cpp"
#include "da_Wibrato.cpp"
// total: 38 .da-based effects

declare_plugin_class(overdrive,iOverdrive);
declare_plugin_class(clreverb,iCLReverb);
declare_plugin_class(prologica,iProLogica);
declare_plugin_class(ringmod,iRingMod);
declare_plugin_class(overdrive2,iOverdrive2);
declare_plugin_class(vocoder,iVocoder);
declare_plugin_class(stvocoder,iStVocoder);
declare_plugin_class(voldc,iVolDC);
declare_plugin_class(encode4,iEncode4);
declare_plugin_class(ts,iTs);
declare_plugin_class(monomix,iMonoMix);
declare_plugin_class(stereomix,iStereoMix);
declare_plugin_class(b2b,iB2B);
declare_plugin_class(b2bv2,iB2BV2);
declare_plugin_class(downmix,iDownmix);
declare_plugin_class(cnv51to20,iCnv51To20);
declare_plugin_class(do16to32,i16To32);
declare_plugin_class(ac3passthrough,iPassThrough);
declare_plugin_class(ac3passthru,iPassThru);
declare_plugin_class(surrounderlt,iSurrounderLt);
declare_plugin_class(CLEAX3Reverb,iCLEAX3Reverb);
declare_plugin_class(hphsp,iHPhSp);
declare_plugin_class(hphsp2,iHPhSp2);
declare_plugin_class(pos3dfx,iPos3DFX);
declare_plugin_class(ac3passthru_x,iPassThruX);
declare_plugin_class(xor,iXor);
declare_plugin_class(booblegum,iBoobleGum);
declare_plugin_class(vibrato,iVibrato);
declare_plugin_class(HarmonicsGen,iHarmonicsGen);
declare_plugin_class(agc,iAgc);
declare_plugin_class(Dither,iDither);
declare_plugin_class(Feedback_Destroyer,iFeedbackDestroyer);
declare_plugin_class(Leslie_Horn,iLeslieHorn);
declare_plugin_class(Prologic,iPrologic);
declare_plugin_class(ProPhaser,iProPhaser);
declare_plugin_class(Sputnik,iSputnik);
declare_plugin_class(tremolo,iTremolo);
declare_plugin_class(virtual51,iVirtual51);
declare_plugin_class(Wibrato,iWibrato);
// total: 38 .da-based effects

kxplugin_list_t plugin_list[]=
{
 { ts_name,ts_guid },
 { clreverb_name,clreverb_guid },
 { CLEAX3Reverb_name,CLEAX3Reverb_guid },
 { CLEAX4Reverb_name,CLEAX4Reverb_guid },
 { overdrive_name,overdrive_guid },
 { overdrive2_name,overdrive2_guid },
 { ringmod_name,ringmod_guid },
 { vocoder_name,vocoder_guid },
 { stvocoder_name,stvocoder_guid },
 { voldc_name,voldc_guid },
 { crossfade_name,crossfade_guid },
 { mx6_name,mx6_guid },
 { gain_name, gain_guid },
 { amp_name, amp_guid },
 { prologica_name,prologica_guid },
 { soundgen_name,soundgen_guid },
 { encode4_name,encode4_guid },
 { monomix_name,monomix_guid },
 { stereomix_name,stereomix_guid },
 { stmix_name,stmix_guid },
 { downmix_name,downmix_guid },
 { cnv51to20_name,cnv51to20_guid },
 { surrounderlt_name, surrounderlt_guid },
 { autowah_name,autowah_guid },
 { flanger_name,flanger_guid },
 { apsfuzz_name,apsfuzz_guid },
 { apscomp_name,apscomp_guid }, 
 { apscomp_sc_name,apscomp_sc_guid },
 { pitch_name,pitch_guid },
 { everb_name, everb_guid },
 { EQ_Bandpass_name,EQ_Bandpass_guid},
 { EQ_Highshelf_name,EQ_Highshelf_guid},
 { EQ_Highpass_name,EQ_Highpass_guid},
 { EQ_Lowpass_name,EQ_Lowpass_guid},
 { EQ_Lowshelf_name,EQ_Lowshelf_guid},
 { EQ_Notch_name,EQ_Notch_guid},
 { EQ_Peaking_name,EQ_Peaking_guid},
 { Freq_Splitter_name,Freq_Splitter_guid},
 { Phat_EQ_Mono_name,Phat_EQ_Mono_guid },
 { Phat_EQ_Stereo_name,Phat_EQ_Stereo_guid },
 { ac3passthru_name,ac3passthru_guid },
 { ac3passthrough_name,ac3passthrough_guid },
 { ac3passthru_x_name,ac3passthru_x_guid },
 { b2b_name,b2b_guid },
 { b2bv2_name,b2bv2_guid },
 { do16to32_name,do16to32_guid },
 { decimator_name, decimator_guid },
 { wavegen2_name,wavegen2_guid },
 { hphsp_name, hphsp_guid },
 { hphsp2_name, hphsp2_guid },
 { pos3dfx_name, pos3dfx_guid },
 { wavegen_name, wavegen_guid },
 { apsexp_name, apsexp_guid },
 { apsexp_plus_name, apsexp_plus_guid },
 { wavegen3_name, wavegen3_guid },
 { dynamica_name,dynamica_guid },
 { NoiseGate2T_name,NoiseGate2T_guid },
 { NoiseGate2Ts_name,NoiseGate2Ts_guid },
 { xor_name,xor_guid },
 { Crossover_2_name,Crossover_2_guid },
 { Crossover_4_name,Crossover_4_guid },
 { booblegum_name,booblegum_guid },
 { vibrato_name,vibrato_guid },
 { info_name,info_guid },
 { osc_name, osc_guid },
 { TheDelay_name, TheDelay_guid },
 { TheSmallDelay_name, TheSmallDelay_guid },
 { TimeBalanceV2_name, TimeBalanceV2_guid },
 { HarmonicsGen_name, HarmonicsGen_guid },
 { Phaser_name, Phaser_guid },
 { MoreBass_name, MoreBass_guid },
 { ReverbEax2_name, ReverbEax2_guid },
 { agc_name, agc_guid },
 { Dither_name, Dither_guid },
 { Feedback_Destroyer_name, Feedback_Destroyer_guid },
 { Leslie_Horn_name, Leslie_Horn_guid },
 { Prologic_name, Prologic_guid },
 { ProPhaser_name, ProPhaser_guid },
 { Sputnik_name, Sputnik_guid },
 { tremolo_name, tremolo_guid },
 { virtual51_name, virtual51_guid },
 { Wibrato_name, Wibrato_guid },

 // total: 81 effects

// ,{ demo_name,demo_guid }
};

// the only exported function
// --------------------------

declare_publish_plugins
{
 switch(command)
 {
  case KXPLUGIN_GET_COUNT:
  	{
         *ret=sizeof(plugin_list)/sizeof(kxplugin_list_t);
  	}
  	return 0;
  case KXPLUGIN_GET_VERSION:
  	*ret=KXPLUGIN_VERSION;
  	return 0;
  case KXPLUGIN_GET_FXLIB_VER:
  	*ret=KX_VERSION_DWORD;
  	return 0;
  case KXPLUGIN_GET_NAME:
  	if(param<sizeof(plugin_list)/sizeof(kxplugin_list_t))
  	{
  	 strncpy((char *)ret,plugin_list[param].name,KX_MAX_STRING);
  	 return 0;
  	}
  	break;
  case KXPLUGIN_GET_GUID:
  	if(param<sizeof(plugin_list)/sizeof(kxplugin_list_t))
  	{
  	 strncpy((char *)ret,plugin_list[param].guid,KX_MAX_STRING);
  	 return 0;
  	}
  	break;	
  case KXPLUGIN_INSTANTIATE:
       if(param<sizeof(plugin_list)/sizeof(kxplugin_list_t))
       {

        // to avoid numbering stuff

//        instantiate_plugin(demo,iDemoPlugin);

        instantiate_plugin(ts,iTsPlugin);
        instantiate_plugin(overdrive,iOverdrivePlugin);
        instantiate_plugin(clreverb,iCLReverbPlugin);
        instantiate_plugin(CLEAX3Reverb,iCLEAX3ReverbPlugin);
        instantiate_plugin(CLEAX4Reverb,iCLEAX4ReverbPlugin);
        instantiate_plugin(prologica,iProLogicaPlugin);
        instantiate_plugin(ringmod,iRingModPlugin);
        instantiate_plugin(autowah,iAutoWahPlugin);
        instantiate_plugin(overdrive2,iOverdrive2Plugin);
        instantiate_plugin(flanger,iFlangerPlugin);
        instantiate_plugin(vocoder,iVocoderPlugin);
        instantiate_plugin(stvocoder,iStVocoderPlugin);
        instantiate_plugin(soundgen,iSoundGenPlugin);
        instantiate_plugin(voldc,iVolDCPlugin);
        instantiate_plugin(apsfuzz,iAPSFuzzPlugin);
        instantiate_plugin(encode4,iEncode4Plugin);
        instantiate_plugin(monomix,iMonoMixPlugin);
        instantiate_plugin(stereomix,iStereoMixPlugin);
        instantiate_plugin(stmix,iStmixPlugin);
        instantiate_plugin(EQ_Bandpass,iEQ_BandpassPlugin);
        instantiate_plugin(EQ_Highpass,iEQ_HighpassPlugin);
        instantiate_plugin(EQ_Highshelf,iEQ_HighshelfPlugin);
        instantiate_plugin(EQ_Lowpass,iEQ_LowpassPlugin);
        instantiate_plugin(EQ_Lowshelf,iEQ_LowshelfPlugin);
        instantiate_plugin(EQ_Notch,iEQ_NotchPlugin);
        instantiate_plugin(EQ_Peaking,iEQ_PeakingPlugin);
        instantiate_plugin(Freq_Splitter,iFreq_SplitterPlugin);
        instantiate_plugin(b2b,iB2BPlugin);
        instantiate_plugin(b2bv2,iB2BV2Plugin);
        instantiate_plugin(ac3passthrough,iPassThroughPlugin);
        instantiate_plugin(ac3passthru,iPassThruPlugin);
        instantiate_plugin(ac3passthru_x,iPassThruXPlugin);
        instantiate_plugin(do16to32,i16To32Plugin);
        instantiate_plugin(downmix,iDownmixPlugin);
        instantiate_plugin(cnv51to20,iCnv51To20Plugin);
        instantiate_plugin(apscomp,iAPSCompPlugin);
        instantiate_plugin(apscomp_sc,iAPSCompSCPlugin);
        instantiate_plugin(Phat_EQ_Mono,iPhat_EQ_MonoPlugin);
        instantiate_plugin(Phat_EQ_Stereo,iPhat_EQ_StereoPlugin);
        instantiate_plugin(everb,iEverbPlugin);
        instantiate_plugin(surrounderlt,iSurrounderLtPlugin);
        instantiate_plugin(decimator,iDecimatorPlugin);
        instantiate_plugin(gain,iGainPlugin);
        instantiate_plugin(amp,iAmpPlugin);
        instantiate_plugin(wavegen,iWavegenPlugin);
        instantiate_plugin(wavegen2,iWavegen2Plugin);
        instantiate_plugin(pitch,iPitchPlugin);
        instantiate_plugin(crossfade,iCrossfadePlugin);
        instantiate_plugin(hphsp,iHPhSpPlugin);
        instantiate_plugin(hphsp2,iHPhSp2Plugin);
        instantiate_plugin(pos3dfx,iPos3DFXPlugin);
        instantiate_plugin(mx6,iMx6Plugin);
        instantiate_plugin(apsexp,iAPSExpPlugin);
        instantiate_plugin(apsexp_plus,iAPSExpPlusPlugin);
        instantiate_plugin(wavegen3,iWavegen3Plugin);
        instantiate_plugin(dynamica,iDynamicaPlugin);
        instantiate_plugin(NoiseGate2T,iNoiseGate2TPlugin);
        instantiate_plugin(NoiseGate2Ts,iNoiseGate2TsPlugin);
        instantiate_plugin(xor,iXorPlugin);
        instantiate_plugin(info,iInfoPlugin);
        instantiate_plugin(Crossover_2,iCross_2_Plugin);
        instantiate_plugin(Crossover_4,iCross_4_Plugin);
        instantiate_plugin(booblegum,iBoobleGumPlugin);
        instantiate_plugin(vibrato,iVibratoPlugin);
        instantiate_plugin(osc,iOscPlugin);
        instantiate_plugin(TheDelay,iTheDelayPlugin);
        instantiate_plugin(TheSmallDelay,iTheSmallDelayPlugin);
        instantiate_plugin(TimeBalanceV2,iTimeBalanceV2Plugin);
        instantiate_plugin(HarmonicsGen,iHarmonicsGenPlugin);
        instantiate_plugin(Phaser,iPhaserPlugin);
        instantiate_plugin(MoreBass,iMoreBassPlugin);
        instantiate_plugin(ReverbEax2,iReverbEax2Plugin);
        instantiate_plugin(agc,iAgcPlugin);
        instantiate_plugin(Dither,iDitherPlugin);
        instantiate_plugin(Feedback_Destroyer,iFeedbackDestroyerPlugin);
        instantiate_plugin(Leslie_Horn,iLeslieHornPlugin);
        instantiate_plugin(Prologic,iPrologicPlugin);
        instantiate_plugin(ProPhaser,iProPhaserPlugin);
        instantiate_plugin(Sputnik,iSputnikPlugin);
        instantiate_plugin(tremolo,iTremoloPlugin);
        instantiate_plugin(virtual51,iVirtual51Plugin);
        instantiate_plugin(Wibrato,iWibratoPlugin);

        // total: 81 effects
       }
       break;
 }	
 *ret=0;
 return -1;
}
