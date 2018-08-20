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

#define my_alloc(hw,ret,sz)  (hw->cb.malloc_func)(hw->cb.call_with,sz,(void **)ret,KX_NONPAGED)
#define my_free(hw,h) (hw->cb.free_func)(hw->cb.call_with,h)

#include "../kxfxlib/da_surrounder2.cpp"
#include "../kxfxlib/da_xrouting.cpp"
#include "../kxfxlib/da_stchorus.cpp"
#include "../kxfxlib/da_reverblt.cpp"
#include "../kxfxlib/da_fxmix2.cpp"
#include "../kxfxlib/da_fxbus.cpp"
#include "../kxfxlib/da_FXBusX.cpp"
#include "../kxfxlib/da_eq10a.cpp"
#include "../kxfxlib/da_timbre.cpp"
#include "../kxfxlib/da_peak.cpp"
#include "../kxfxlib/da_p16v.cpp"

KX_API(int,kx_connect_microcode(kx_hw *hw,int pgm1,char *src,int pgm2,word dst_n));

KX_API(int,kx_dsp_reset(kx_hw *hw))
{
 kx_dsp_clear(hw);

 int prolog_pgm=0,epilog_pgm=0,xrouting_pgm=0,reverblt_pgm=0,// xsumm_r_pgm=0,xsumm_c_pgm=0,
   fx_mix2_pgm=0,p16v_pgm=0,
   stchorus_pgm=0,fxbus_pgm=0,eq10a_pgm=0,peak_pgm=0,surrounder2_pgm=0;

 if(hw->can_passthru)
 {
    if(hw->can_k8_passthru)
 	fxbus_pgm=kx_load_microcode(hw,FXBusX_name,FXBusX_10k8_code,FXBusX_10k8_code_size,FXBusX_info,FXBusX_10k8_info_size,FXBusX_itramsize,FXBusX_xtramsize,
 	  FXBusX_copyright,FXBusX_engine,FXBusX_created,FXBusX_comment,FXBusX_guid);
    else
 	fxbus_pgm=kx_load_microcode(hw,FXBusX_name,FXBusX_10k2_code,FXBusX_10k2_code_size,FXBusX_info,FXBusX_10k2_info_size,FXBusX_itramsize,FXBusX_xtramsize,
 	  FXBusX_copyright,FXBusX_engine,FXBusX_created,FXBusX_comment,FXBusX_guid);
 }
 else
 {
 	fxbus_pgm=kx_load_microcode(hw,fxbus_name,fxbus_code,0,fxbus_info,(int)(hw->is_10k2?sizeof(fxbus_info):sizeof(fxbus_info)/2),fxbus_itramsize,fxbus_xtramsize,
 	  fxbus_copyright,fxbus_engine,fxbus_created,fxbus_comment,fxbus_guid);
 }

 {
    	const char *microcode_name=NULL;
    	int last_gpr=0x8000;
    	int last_input_gpr=0x4000;
    	int itramsize=0;
    	int xtramsize=0;
    	int code_size=0;
    	int info_size=0;
    	dsp_code *dyn_code=0;
        dsp_register_info *dyn_info=0;
    	int cur_info=0;
    	int cur_code=0;

        // int is_aps=hw->is_aps;
        // int is_10k2=hw->is_10k2;
        // dword dsp_flags=hw->dsp_flags;
        // int is_51=hw->is_51;
        // int can_passthru=hw->can_passthru;
		int is_edsp=hw->is_edsp;
		// int output_r=0;
		int input_r=0;
        // int is_k8=hw->is_k8;
	        
        #define PROLOG_SOURCE

	    #include "legacy/prepare.h"
	      begin_microcode("prolog");
        #include "../kxfxlib/da_prolog.cpp"
	    end_microcode();

	    #include "legacy/process.h"
	      begin_microcode("prolog");
        #include "../kxfxlib/da_prolog.cpp"

        // should not call end_microcode()

     	prolog_pgm=kx_load_microcode(hw,prolog_name,dyn_code,code_size,dyn_info,info_size,itramsize,xtramsize,
     	  prolog_copyright,
     	  prolog_engine,
     	  prolog_created,
     	  prolog_comment,
     	  prolog_guid);

     	my_free(hw,dyn_code);
     	my_free(hw,dyn_info);
 }	
 {	
        reverblt_pgm=kx_load_microcode(hw,reverblt_name,reverblt_code,sizeof(reverblt_code),reverblt_info,sizeof(reverblt_info),reverblt_itramsize,reverblt_xtramsize,
           reverblt_copyright,reverblt_engine,reverblt_created,reverblt_comment,reverblt_guid);
 }	
 {	
	
        xrouting_pgm=kx_load_microcode(hw,xrouting_name,xrouting_code,sizeof(xrouting_code),xrouting_info,sizeof(xrouting_info),xrouting_itramsize,xrouting_xtramsize,
           xrouting_copyright,xrouting_engine,xrouting_created,xrouting_comment,xrouting_guid);
 }	
 {	
	
        stchorus_pgm=kx_load_microcode(hw,stchorus_name,stchorus_code,sizeof(stchorus_code),stchorus_info,sizeof(stchorus_info),stchorus_itramsize,stchorus_xtramsize,
           stchorus_copyright,stchorus_engine,stchorus_created,stchorus_comment,stchorus_guid);
 }	
 {	
	
        fx_mix2_pgm=kx_load_microcode(hw,fx_mix2_name,fx_mix2_code,sizeof(fx_mix2_code),fx_mix2_info,sizeof(fx_mix2_info),fx_mix2_itramsize,fx_mix2_xtramsize,
           fx_mix2_copyright,fx_mix2_engine,fx_mix2_created,fx_mix2_comment,fx_mix2_guid);
 }	

 {	
   if(hw->is_a2 && !hw->is_edsp)
   {	
        p16v_pgm=kx_load_microcode(hw,p16v_name,p16v_code,sizeof(p16v_code),p16v_info,sizeof(p16v_info),p16v_itramsize,p16v_xtramsize,
           p16v_copyright,p16v_engine,p16v_created,p16v_comment,p16v_guid);
   }
 }	

 const char *eq_name=NULL;
 {
  if(hw->is_10k2)
  {
 	eq10a_pgm=kx_load_microcode(hw,eq10a_name,eq10a_code,sizeof(eq10a_code),eq10a_info,sizeof(eq10a_info),eq10a_itramsize,eq10a_xtramsize,
 	  eq10a_copyright,eq10a_engine,eq10a_created,eq10a_comment,eq10a_guid);
 	eq_name=eq10a_name;
  }
  else
  {
 	eq10a_pgm=kx_load_microcode(hw,timbre_name,timbre_code,sizeof(timbre_code),timbre_info,sizeof(timbre_info),timbre_itramsize,timbre_xtramsize,
 	  timbre_copyright,timbre_engine,timbre_created,timbre_comment,timbre_guid);
 	eq_name=timbre_name;
  }
 }
 {
    if(hw->is_a2)
 	  surrounder2_pgm=kx_load_microcode(hw,surrounder2_name,surrounder2_code_71DP,sizeof(surrounder2_code_71DP),
         surrounder2_info,sizeof(surrounder2_info),surrounder2_itramsize,surrounder2_xtramsize,
 	     surrounder2_copyright,surrounder2_engine,surrounder2_created,surrounder2_comment,surrounder2_guid);
    else
 	  surrounder2_pgm=kx_load_microcode(hw,surrounder2_name,surrounder2_code_51DP,sizeof(surrounder2_code_51DP),
         surrounder2_info,sizeof(surrounder2_info)-2*sizeof(dsp_register_info),surrounder2_itramsize,surrounder2_xtramsize,
 	     surrounder2_copyright,surrounder2_engine,surrounder2_created,surrounder2_comment,surrounder2_guid);
 }
 
 {
    	const char *microcode_name=NULL;
    	int last_gpr=0x8000;
    	int last_input_gpr=0x4000;
    	int itramsize=0;
    	int xtramsize=0;
    	int code_size=0;
    	int info_size=0;
    	dsp_code *dyn_code=0; dsp_register_info *dyn_info=0;
    	int cur_info=0;
    	int cur_code=0;

        int is_aps=hw->is_aps;
        int is_10k2=hw->is_10k2;
        dword dsp_flags=hw->dsp_flags;
        int is_51=hw->is_51;
        int can_passthru=hw->can_passthru;
        int is_bad_sb22x=hw->is_bad_sb22x;
		int is_edsp=hw->is_edsp;
		int output_r=0;
		int input_r=0;
        // int is_k8=hw->is_k8;
		
        dword is_doo=0;
        kx_get_hw_parameter(hw,KX_HW_DOO,&is_doo);

        int need_spdif_swap=0;

        if((hw->is_a2 || (hw->is_a2==0 && hw->cb.subsys==0x00521102)) && !hw->is_k8 && !hw->is_edsp)
        {
         // any audigy2-based board + sb0160 [a1]
         // FIXME !! need to check if is_k8 are affected, too
         need_spdif_swap=1;
        }

        if(hw->ext_flags&KX_HW_FORCE_SPDIF_SWAP)
         need_spdif_swap=1-need_spdif_swap;

            #define EPILOG_SOURCE

        	#include "legacy/prepare.h"
        	begin_microcode("epilog");
            #include "../kxfxlib/da_epilog.cpp"
        	end_microcode();

        	#include "legacy/process.h"
        	begin_microcode("epilog");
            #include "../kxfxlib/da_epilog.cpp"

            // should not call end_microcode()

            epilog_pgm=0;

        	epilog_pgm=kx_load_microcode(hw,epilog_name,dyn_code,code_size,dyn_info,info_size,itramsize,xtramsize,
        	  epilog_copyright,
        	  epilog_engine,
        	  epilog_created,
        	  epilog_comment,
        	  epilog_guid);

	my_free(hw,dyn_code);
	my_free(hw,dyn_info);
 }	

 {
 	peak_pgm=kx_load_microcode(hw,peak_name,peak_code,sizeof(peak_code),peak_info,sizeof(peak_info),peak_itramsize,peak_xtramsize,
 	  peak_copyright,peak_engine,peak_created,peak_comment,peak_guid);
 }


 // translate
 if(prolog_pgm<=0 || epilog_pgm<=0 || xrouting_pgm<=0 || stchorus_pgm<=0  || reverblt_pgm<=0
   /* || xsumm_r_pgm<=0 || xsumm_c_pgm<=0 */
   || fx_mix2_pgm<=0
   || peak_pgm<=0 || surrounder2_pgm<=0)
 {
  debug(DLIB,"!!! Error loading microcode\n");
  }

 {
  int ret;

  ret=kx_translate_microcode(hw,fxbus_pgm,KX_MICROCODE_BEFORE);
  if(ret)
   debug(DLIB,"!!! Error translating fxbus microcode (%d)\n",ret);

  ret=kx_translate_microcode(hw,epilog_pgm,KX_MICROCODE_BEFORE);
  if(ret)
   debug(DLIB,"!!! Error translating epilog microcode (%d)\n",ret);

  ret=kx_translate_microcode(hw,prolog_pgm,KX_MICROCODE_ANY);
  if(ret)
   debug(DLIB,"!!! Error translating prolog microcode (%d)\n",ret);

  ret=kx_translate_microcode(hw,fx_mix2_pgm,KX_MICROCODE_ANY);
  if(ret)
   debug(DLIB,"!!! Error translating fx_mix2 microcode (%d)\n",ret);

  if(p16v_pgm)
  {
   ret=kx_translate_microcode(hw,p16v_pgm,KX_MICROCODE_ANY);
   if(ret)
    debug(DLIB,"!!! Error translating p16v microcode (%d)\n",ret);
  }

  ret=kx_translate_microcode(hw,reverblt_pgm);
  if(ret)
   debug(DLIB,"!!! Error translating reverb microcode (%d)\n",ret);

  ret=kx_translate_microcode(hw,stchorus_pgm);
  if(ret)
   debug(DLIB,"!!! Error translating chorus microcode (%d)\n",ret);

  ret=kx_translate_microcode(hw,peak_pgm,KX_MICROCODE_BEFORE);
  if(ret)
   debug(DLIB,"!!! Error translating peak microcode (%d)\n",ret);

  ret=kx_translate_microcode(hw,eq10a_pgm,KX_MICROCODE_BEFORE);
  if(ret)
   debug(DLIB,"!!! Error translating eq10a microcode (%d)\n",ret);

  ret=kx_translate_microcode(hw,surrounder2_pgm,KX_MICROCODE_BEFORE);
  if(ret)
   debug(DLIB,"!!! Error translating surrounder2 microcode (%d)\n",ret);

  ret=kx_translate_microcode(hw,xrouting_pgm,KX_MICROCODE_BEFORE);
  if(ret)
   debug(DLIB,"!!! Error translating xrouting microcode (%d)\n",ret);

  // connect xrouting to prolog & FXBUSes
  kx_connect_microcode(hw,xrouting_pgm,"WaveL",fxbus_pgm,(word)(KX_GET_SEND_A(hw->cb.def_routings[DEF_WAVE01_ROUTING])+0x8000));
  kx_connect_microcode(hw,xrouting_pgm,"WaveR",fxbus_pgm,(word)(KX_GET_SEND_B(hw->cb.def_routings[DEF_WAVE01_ROUTING])+0x8000));
  kx_connect_microcode(hw,xrouting_pgm,"SynthL",fxbus_pgm,(word)(KX_GET_SEND_A(hw->cb.def_routings[DEF_SYNTH1_ROUTING])+0x8000));
  kx_connect_microcode(hw,xrouting_pgm,"SynthR",fxbus_pgm,(word)(KX_GET_SEND_B(hw->cb.def_routings[DEF_SYNTH1_ROUTING])+0x8000));
  kx_connect_microcode(hw,xrouting_pgm,"fL_nw_In",fxbus_pgm,(word)(KX_GET_SEND_A(hw->cb.def_routings[DEF_AC3_LEFT_ROUTING])+0x8000));
  kx_connect_microcode(hw,xrouting_pgm,"center_n_In",fxbus_pgm,(word)(KX_GET_SEND_A(hw->cb.def_routings[DEF_AC3_CENTER_ROUTING])+0x8000));
  kx_connect_microcode(hw,xrouting_pgm,"fR_ne_In",fxbus_pgm,(word)(KX_GET_SEND_A(hw->cb.def_routings[DEF_AC3_RIGHT_ROUTING])+0x8000));
  kx_connect_microcode(hw,xrouting_pgm,"sL_sw_In",fxbus_pgm,(word)(KX_GET_SEND_A(hw->cb.def_routings[DEF_AC3_SLEFT_ROUTING])+0x8000));
  kx_connect_microcode(hw,xrouting_pgm,"sR_se_In",fxbus_pgm,(word)(KX_GET_SEND_A(hw->cb.def_routings[DEF_AC3_SRIGHT_ROUTING])+0x8000));
  kx_connect_microcode(hw,xrouting_pgm,"lfe_In",fxbus_pgm,(word)(KX_GET_SEND_A(hw->cb.def_routings[DEF_AC3_SUBWOOFER_ROUTING])+0x8000));

  kx_connect_microcode(hw,xrouting_pgm,"w_In",fxbus_pgm,(word)(KX_GET_SEND_A(hw->cb.def_routings[DEF_3D_LEFT_ROUTING])+0x8000));
  kx_connect_microcode(hw,xrouting_pgm,"e_In",fxbus_pgm,(word)(KX_GET_SEND_A(hw->cb.def_routings[DEF_3D_RIGHT_ROUTING])+0x8000));
  kx_connect_microcode(hw,xrouting_pgm,"s_In",fxbus_pgm,(word)(KX_GET_SEND_A(hw->cb.def_routings[DEF_AC3_SCENTER_ROUTING])+0x8000));
  kx_connect_microcode(hw,xrouting_pgm,"top_In",fxbus_pgm,(word)(KX_GET_SEND_A(hw->cb.def_routings[DEF_3D_TOP_ROUTING])+0x8000));

  // connect to fx_mix2
  kx_connect_microcode(hw,reverblt_pgm,"inl",fx_mix2_pgm,"out_l1");
  kx_connect_microcode(hw,reverblt_pgm,"inr",fx_mix2_pgm,"out_r1");

  kx_connect_microcode(hw,xrouting_pgm,"FX1_nw",reverblt_pgm,"outfl");
  kx_connect_microcode(hw,xrouting_pgm,"FX1_ne",reverblt_pgm,"outfr");

  kx_connect_microcode(hw,xrouting_pgm,"FX1_sL",reverblt_pgm,"outrl");
  kx_connect_microcode(hw,xrouting_pgm,"FX1_sR",reverblt_pgm,"outrr");

  // connect to fx_mix2
  kx_connect_microcode(hw,stchorus_pgm,"inl",fx_mix2_pgm,"out_l2");
  kx_connect_microcode(hw,stchorus_pgm,"inr",fx_mix2_pgm,"out_r2");

  // note: FX2 doesn't use 'FX2_fL/fR'
  kx_connect_microcode(hw,xrouting_pgm,"FX2_L",stchorus_pgm,"outfl");
  kx_connect_microcode(hw,xrouting_pgm,"FX2_R",stchorus_pgm,"outfr");
  kx_connect_microcode(hw,xrouting_pgm,"FX2_sL",stchorus_pgm,"outrl");
  kx_connect_microcode(hw,xrouting_pgm,"FX2_sR",stchorus_pgm,"outrr");

  // connect fx_mix2
  kx_connect_microcode(hw,fx_mix2_pgm,"mono_in1",fxbus_pgm,(word)(KX_GET_SEND_C(hw->cb.def_routings[DEF_SYNTH1_ROUTING])+0x8000));
  kx_connect_microcode(hw,fx_mix2_pgm,"mono_in2",fxbus_pgm,(word)(KX_GET_SEND_D(hw->cb.def_routings[DEF_SYNTH1_ROUTING])+0x8000));

  kx_connect_microcode(hw,fx_mix2_pgm,"in0_l",fxbus_pgm,(word)(KX_GET_SEND_A(hw->cb.def_routings[DEF_WAVE01_ROUTING])+0x8000));
  kx_connect_microcode(hw,fx_mix2_pgm,"in0_r",fxbus_pgm,(word)(KX_GET_SEND_B(hw->cb.def_routings[DEF_WAVE01_ROUTING])+0x8000));
  kx_connect_microcode(hw,fx_mix2_pgm,"in1_l",fxbus_pgm,(word)(KX_GET_SEND_A(hw->cb.def_routings[DEF_SYNTH1_ROUTING])+0x8000));
  kx_connect_microcode(hw,fx_mix2_pgm,"in1_r",fxbus_pgm,(word)(KX_GET_SEND_B(hw->cb.def_routings[DEF_SYNTH1_ROUTING])+0x8000));

  // connect inputs to routing
  kx_connect_microcode(hw,xrouting_pgm,"In0",prolog_pgm,"in0");
  kx_connect_microcode(hw,xrouting_pgm,"In1",prolog_pgm,"in1");
  kx_connect_microcode(hw,xrouting_pgm,"In2",prolog_pgm,"in2");
  kx_connect_microcode(hw,xrouting_pgm,"In3",prolog_pgm,"in3");
  kx_connect_microcode(hw,xrouting_pgm,"In4",prolog_pgm,"in4");
  kx_connect_microcode(hw,xrouting_pgm,"In5",prolog_pgm,"in5");
  kx_connect_microcode(hw,xrouting_pgm,"In6",prolog_pgm,"in6");
  kx_connect_microcode(hw,xrouting_pgm,"In7",prolog_pgm,"in7");
  kx_connect_microcode(hw,xrouting_pgm,"In8",prolog_pgm,"in8");
  kx_connect_microcode(hw,xrouting_pgm,"In9",prolog_pgm,"in9");
  kx_connect_microcode(hw,xrouting_pgm,"In10",prolog_pgm,"in10");
  kx_connect_microcode(hw,xrouting_pgm,"In11",prolog_pgm,"in11");
  kx_connect_microcode(hw,xrouting_pgm,"In12",prolog_pgm,"in12");
  kx_connect_microcode(hw,xrouting_pgm,"In13",prolog_pgm,"in13");

  // connect routing to epilog

  kx_connect_microcode(hw,xrouting_pgm,"stereo_l",surrounder2_pgm,"in_L");
  kx_connect_microcode(hw,xrouting_pgm,"stereo_r",surrounder2_pgm,"in_R");
  kx_connect_microcode(hw,xrouting_pgm,"nw",surrounder2_pgm,"in_nw");
  kx_connect_microcode(hw,xrouting_pgm,"ne",surrounder2_pgm,"in_ne");
  kx_connect_microcode(hw,xrouting_pgm,"sw",surrounder2_pgm,"in_sw");
  kx_connect_microcode(hw,xrouting_pgm,"se",surrounder2_pgm,"in_se");
  kx_connect_microcode(hw,xrouting_pgm,"n",surrounder2_pgm,"in_n");
  kx_connect_microcode(hw,xrouting_pgm,"lfe",surrounder2_pgm,"in_lfe");
  kx_connect_microcode(hw,xrouting_pgm,"w",surrounder2_pgm,"in_w");
  kx_connect_microcode(hw,xrouting_pgm,"e",surrounder2_pgm,"in_e");
  kx_connect_microcode(hw,xrouting_pgm,"s",surrounder2_pgm,"in_s");
  kx_connect_microcode(hw,xrouting_pgm,"top",surrounder2_pgm,"in_top");

  kx_connect_microcode(hw,epilog_pgm,"RecL",xrouting_pgm,"RecL");
  kx_connect_microcode(hw,epilog_pgm,"RecR",xrouting_pgm,"RecR");

  kx_connect_microcode(hw,surrounder2_pgm,"fL",eq10a_pgm,"inl");
  kx_connect_microcode(hw,surrounder2_pgm,"fR",eq10a_pgm,"inr");
  kx_connect_microcode(hw,peak_pgm,"peak_l",eq10a_pgm,"outl");
  kx_connect_microcode(hw,peak_pgm,"peak_r",eq10a_pgm,"outr");

  if(p16v_pgm)
  {
   if(hw->dsp_flags&KX_DSP_SWAP_REAR)
   {
    kx_connect_microcode(hw,p16v_pgm,"in7",fx_mix2_pgm,"in2_l");
    kx_connect_microcode(hw,p16v_pgm,"in8",fx_mix2_pgm,"in2_r");
   }
   else
   {
    kx_connect_microcode(hw,p16v_pgm,"in1",fx_mix2_pgm,"in2_l");
    kx_connect_microcode(hw,p16v_pgm,"in2",fx_mix2_pgm,"in2_r");
   }
  }

  // APS correspondence:
  // front: a1,a2 [out0,1]
  // rear: d1 [out 2,3]
  // c+sw: a3 a4 d2 [out 4,5]
  
  kx_connect_microcode(hw,eq10a_pgm,"outl",epilog_pgm,"out0");
  kx_connect_microcode(hw,eq10a_pgm,"outr",epilog_pgm,"out1");

  if(!hw->is_aps)
  {
   kx_connect_microcode(hw,eq10a_pgm,"outl",epilog_pgm,"out2");
   kx_connect_microcode(hw,eq10a_pgm,"outr",epilog_pgm,"out3");

   if(!hw->is_edsp)
   {
      kx_connect_microcode(hw,surrounder2_pgm,"sL",epilog_pgm,"out8");
      kx_connect_microcode(hw,surrounder2_pgm,"sR",epilog_pgm,"out9");
      kx_connect_microcode(hw,surrounder2_pgm,"Center",epilog_pgm,"out17");
      kx_connect_microcode(hw,surrounder2_pgm,"LFE",epilog_pgm,"out18");
      kx_connect_microcode(hw,surrounder2_pgm,"Center",epilog_pgm,"out4");
      kx_connect_microcode(hw,surrounder2_pgm,"LFE",epilog_pgm,"out5");
      kx_connect_microcode(hw,eq10a_pgm,"outl",epilog_pgm,"out6");
      kx_connect_microcode(hw,eq10a_pgm,"outr",epilog_pgm,"out7");
   }
   else
   {
      kx_connect_microcode(hw,surrounder2_pgm,"sL",epilog_pgm,"out4");
      kx_connect_microcode(hw,surrounder2_pgm,"sR",epilog_pgm,"out5");
      kx_connect_microcode(hw,surrounder2_pgm,"Center",epilog_pgm,"out6");
      kx_connect_microcode(hw,surrounder2_pgm,"LFE",epilog_pgm,"out7");
      kx_connect_microcode(hw,surrounder2_pgm,"sCenter1",epilog_pgm,"out8");
      kx_connect_microcode(hw,surrounder2_pgm,"sCenter2",epilog_pgm,"out9");

      // not connected: out17,out18 -> e-dsp 10,11 
      // not connected: out30,out31 -> e-dsp 12,13
   }

   if(hw->is_a2 && !hw->is_edsp)
   {
    kx_connect_microcode(hw,surrounder2_pgm,"sCenter1",epilog_pgm,"out30"); // sCenter
    kx_connect_microcode(hw,surrounder2_pgm,"sCenter2",epilog_pgm,"out31"); // sCenter
   }
  }
  else
  {
   kx_connect_microcode(hw,surrounder2_pgm,"sL",epilog_pgm,"out2");
   kx_connect_microcode(hw,surrounder2_pgm,"sR",epilog_pgm,"out3");
   kx_connect_microcode(hw,surrounder2_pgm,"Center",epilog_pgm,"out4");
   kx_connect_microcode(hw,surrounder2_pgm,"LFE",epilog_pgm,"out5");
  }

  // default asio routings
  if(hw->is_10k2 || (!hw->is_51))
  {
   kx_connect_microcode(hw,xrouting_pgm,"RecL",epilog_pgm,"asio0");
   kx_connect_microcode(hw,xrouting_pgm,"RecR",epilog_pgm,"asio1");

   kx_connect_microcode(hw,prolog_pgm,"in0",epilog_pgm,"asio2");
   kx_connect_microcode(hw,prolog_pgm,"in1",epilog_pgm,"asio3");
   kx_connect_microcode(hw,prolog_pgm,"in2",epilog_pgm,"asio4");
   kx_connect_microcode(hw,prolog_pgm,"in3",epilog_pgm,"asio5");
   kx_connect_microcode(hw,prolog_pgm,"in4",epilog_pgm,"asio6");
   kx_connect_microcode(hw,prolog_pgm,"in5",epilog_pgm,"asio7");
   kx_connect_microcode(hw,prolog_pgm,"in6",epilog_pgm,"asio8");
   kx_connect_microcode(hw,prolog_pgm,"in7",epilog_pgm,"asio9");
   kx_connect_microcode(hw,prolog_pgm,"in8",epilog_pgm,"asio10");
   kx_connect_microcode(hw,prolog_pgm,"in9",epilog_pgm,"asio11");

   kx_connect_microcode(hw,prolog_pgm,"in10",epilog_pgm,"asio12");
   kx_connect_microcode(hw,prolog_pgm,"in11",epilog_pgm,"asio13");
   kx_connect_microcode(hw,prolog_pgm,"in12",epilog_pgm,"asio14");
   kx_connect_microcode(hw,prolog_pgm,"in13",epilog_pgm,"asio15");
  }
  else
  {
   kx_connect_microcode(hw,xrouting_pgm,"RecL",epilog_pgm,"asio4");
   kx_connect_microcode(hw,xrouting_pgm,"RecR",epilog_pgm,"asio5");

   kx_connect_microcode(hw,prolog_pgm,"in0",epilog_pgm,"asio6");
   kx_connect_microcode(hw,prolog_pgm,"in1",epilog_pgm,"asio7");
   kx_connect_microcode(hw,prolog_pgm,"in2",epilog_pgm,"asio8");
   kx_connect_microcode(hw,prolog_pgm,"in3",epilog_pgm,"asio9");
//   kx_connect_microcode(hw,prolog_pgm,"in4",epilog_pgm,"asio8");
//   kx_connect_microcode(hw,prolog_pgm,"in5",epilog_pgm,"asio9");
   kx_connect_microcode(hw,prolog_pgm,"in6",epilog_pgm,"asio10");
   kx_connect_microcode(hw,prolog_pgm,"in7",epilog_pgm,"asio11");

   kx_connect_microcode(hw,prolog_pgm,"in8",epilog_pgm,"asio12");
   kx_connect_microcode(hw,prolog_pgm,"in9",epilog_pgm,"asio13");

//   kx_connect_microcode(hw,prolog_pgm,"in10",epilog_pgm,"asio14");
//   kx_connect_microcode(hw,prolog_pgm,"in11",epilog_pgm,"asio15");
   kx_connect_microcode(hw,prolog_pgm,"in12",epilog_pgm,"asio14");
   kx_connect_microcode(hw,prolog_pgm,"in13",epilog_pgm,"asio15");
  }                                      

  kx_enable_microcode(hw,fxbus_pgm);
  kx_enable_microcode(hw,prolog_pgm);
  kx_enable_microcode(hw,epilog_pgm);
  kx_enable_microcode(hw,xrouting_pgm);
  kx_enable_microcode(hw,reverblt_pgm);
  kx_enable_microcode(hw,stchorus_pgm);
  kx_enable_microcode(hw,fx_mix2_pgm);
  kx_enable_microcode(hw,eq10a_pgm);
  kx_enable_microcode(hw,peak_pgm);
  kx_enable_microcode(hw,surrounder2_pgm);
  if(p16v_pgm)
   kx_enable_microcode(hw,p16v_pgm);
	 
#if defined(__APPLE__)
	 debug(DLIB,"Note: microcode EQ set to 'bypass' for OSX\n");
	 debug(DLIB,"Note: microcode ReverbLT disabled for OSX\n");
	 debug(DLIB,"Note: microcode Chorus disabled for OSX\n");
	 
	 kx_set_microcode_bypass(hw,eq10a_pgm,1);
	 kx_disable_microcode(hw,reverblt_pgm);
	 kx_disable_microcode(hw,stchorus_pgm);
     if(hw->is_edsp){
         kx_connect_microcode(hw,prolog_pgm,"in0",epilog_pgm,"asio0");
         kx_connect_microcode(hw,prolog_pgm,"in1",epilog_pgm,"asio1");
         kx_connect_microcode(hw,prolog_pgm,"in2",epilog_pgm,"asio2");
         kx_connect_microcode(hw,prolog_pgm,"in3",epilog_pgm,"asio3");
         kx_connect_microcode(hw,prolog_pgm,"in4",epilog_pgm,"asio4");
         kx_connect_microcode(hw,prolog_pgm,"in5",epilog_pgm,"asio5");
         kx_connect_microcode(hw,prolog_pgm,"in6",epilog_pgm,"asio6");
         kx_connect_microcode(hw,prolog_pgm,"in7",epilog_pgm,"asio7");
     }
#endif
 }

 debug(DLIB,"Microcode compiled: %s %s; %s%s%s; with '%s'\n",
  __DATE__,__TIME__,
   (hw->dsp_flags&KX_DSP_SWAP_REAR)?"[Rear/Front swapped]":"",
   (hw->dsp_flags&KX_DSP_ROUTE_PH_TO_CSW)?"[Phones Routed to CSW]":"",
   (hw->dsp_flags==0)?"[Standard microcode]":"",
   eq_name?eq_name:"Unknown"
  );

 // enable microcode processing
 kx_dsp_go(hw);

 return 0;
}

int kx_dsp_close(kx_hw *hw)
{
 kx_dsp_clear(hw);
 hw->initialized&=~KX_DSP_INITED;
 return 0;
}

int kx_dsp_init(kx_hw *hw)
{
 init_list(&hw->microcodes);
 hw->initialized|=KX_DSP_INITED;

 debug(DLIB,"DSP initialization\n");
 return kx_dsp_reset(hw);
}
