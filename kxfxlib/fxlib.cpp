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
#include "reverblt.h"
#include "stchorus.h"
#include "epilog.h"
#include "prolog.h"
#include "fxbus.h"
#include "fxbus2.h"
#include "fxbusx.h"
#include "timbre.h"
#include "eq10a.h"
#include "peak.h"
#include "surrounder2.h"
#include "k1lt.h"
#include "k2lt.h"
#include "asio.h"
#include "src.h"
// total: 15 .cpp effects

// include 'pseudo'-plugins
#include "da_chorus.cpp"
#include "da_delay.cpp"
#include "da_summ.cpp"
#include "da_vol.cpp"
#include "da_stvol.cpp"
#include "da_xsumm.cpp"
#include "da_pan.cpp"
#include "da_panx2.cpp"
#include "da_xrouting.cpp"
#include "da_x4.cpp"
#include "da_div4.cpp"
#include "da_p16v.cpp"
#include "da_epiloglt_k1.cpp"
#include "da_epiloglt_k2.cpp"
#include "da_prologlt.cpp"
#include "da_fxmix.cpp"
#include "da_fxmix2.cpp"
#include "da_delay_b.cpp"
#include "da_delay_a.cpp"
#include "da_phase.cpp"
// total: 20 .da effects

declare_plugin_class(chorus,iChorus);
declare_plugin_class(delay,iDelay);
declare_plugin_class(summ,iSumm);
declare_plugin_class(vol,iVol);
declare_plugin_class(stvol,iStVol);
declare_plugin_class(xsumm,iXSumm);
declare_plugin_class(phase,iPhase);
declare_plugin_class(pan,iPan);
declare_plugin_class(panx2,iPanX2);
declare_plugin_class(xrouting,iXRouting);
declare_plugin_class(x4,iX4);
declare_plugin_class(div4,iDiv4);
declare_plugin_class(prologlt,iPrologLite);
declare_plugin_class(p16v,iP16V);
declare_plugin_class(epiloglt_k1,iEpilogLiteK1);
declare_plugin_class(epiloglt_k2,iEpilogLiteK2);
declare_plugin_class(fx_mix,iFXMix);
declare_plugin_class(fx_mix2,iFXMix2);
declare_plugin_class(delay_b,iDelayB);
declare_plugin_class(delay_a,iDelayA);
// total: 20 .da effects

kxplugin_list_t plugin_list[]=
{
 { reverblt_name,reverblt_guid },
 { chorus_name,chorus_guid },
 { stchorus_name,stchorus_guid },
 { delay_name,delay_guid },
 { fxbus_name,fxbus_guid },
 { fxbus2_name,fxbus2_guid },
 { prolog_name,prolog_guid },
 { xrouting_name, xrouting_guid },
 { epilog_name,epilog_guid},
 { summ_name,summ_guid },
 { xsumm_name,xsumm_guid },
 { vol_name,vol_guid },
 { stvol_name,stvol_guid },
 { phase_name,phase_guid },
 { pan_name,pan_guid },
 { panx2_name,panx2_guid },
 { x4_name,x4_guid },
 { div4_name,div4_guid },
 { surrounder2_name,surrounder2_guid },
 { timbre_name,timbre_guid },
 { eq10a_name,eq10a_guid },
 { peak_name,peak_guid },
 { prologlt_name, prologlt_guid },
 { p16v_name, p16v_guid },
 { epiloglt_k1_name, epiloglt_k1_guid },
 { epiloglt_k2_name, epiloglt_k2_guid },
 { fx_mix_name, fx_mix_guid },
 { fx_mix2_name, fx_mix2_guid },
 { delay_b_name, delay_b_guid },
 { delay_a_name, delay_a_guid },
 { k1lt_name,k1lt_guid },
 { k2lt_name,k2lt_guid },
 { src_name,src_guid },
 { asio_name,asio_guid },
 { FXBusX_name, FXBusX_guid },
 // total: 35 effects
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

        instantiate_plugin(reverblt,iReverbLitePlugin);
        instantiate_plugin(chorus,iChorusPlugin);
        instantiate_plugin(delay,iDelayPlugin);
        instantiate_plugin(prolog,iPrologPlugin);
        instantiate_plugin(summ,iSummPlugin);
        instantiate_plugin(vol,iVolPlugin);
        instantiate_plugin(stvol,iStVolPlugin);
        instantiate_plugin(epilog,iEpilogPlugin);
        instantiate_plugin(stchorus,iStereoChorusBPlugin);
        instantiate_plugin(xsumm,iXSummPlugin);
        instantiate_plugin(timbre,iTimbrePlugin);
        instantiate_plugin(eq10a,iEq10aPlugin);
        instantiate_plugin(fxbus,iFXBusPlugin);
        instantiate_plugin(fxbus2,iFXBus2Plugin);
        instantiate_plugin(phase,iPhasePlugin);
        instantiate_plugin(pan,iPanPlugin);
        instantiate_plugin(panx2,iPanX2Plugin);
        instantiate_plugin(peak,iPeakPlugin);
        instantiate_plugin(xrouting,iXRoutingPlugin);
        instantiate_plugin(surrounder2,iSurrounder2Plugin);
        instantiate_plugin(x4,iX4Plugin);
        instantiate_plugin(div4,iDiv4Plugin);
        instantiate_plugin(prologlt,iPrologLitePlugin);
        instantiate_plugin(p16v,iP16VPlugin);
        instantiate_plugin(epiloglt_k1,iEpilogLiteK1Plugin);
        instantiate_plugin(epiloglt_k2,iEpilogLiteK2Plugin);
        instantiate_plugin(fx_mix,iFXMixPlugin);
        instantiate_plugin(fx_mix2,iFXMix2Plugin);
        instantiate_plugin(FXBusX,iFXBusXPlugin);
        instantiate_plugin(delay_b,iDelayBPlugin);
        instantiate_plugin(delay_a,iDelayAPlugin);
        instantiate_plugin(k1lt,iK1ltPlugin);
        instantiate_plugin(k2lt,iK2ltPlugin);
        instantiate_plugin(src,iSrcPlugin);
        instantiate_plugin(asio,iAsioPlugin);
        // total: 35 effects
       }
       break;
 }	
 *ret=0;
 return -1;
}
