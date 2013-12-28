// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov, 2001-2008.
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

// --------------- the above should be a 'standard' beginning of any .cpp file

// effect class
#include "epilog.h"
// effect source
#include "da_epilog.cpp"

inline void my_alloc(int ,void **ret,size_t sz)
{
 *ret=malloc(sz);
}
inline void my_free(int ,void *h)
{
 free(h);
}

int iEpilogPlugin::request_microcode() // generally is called after init()
{
    	char *microcode_name;
    	int last_gpr=0x8000;
    	int last_input_gpr=0x4000;
    	int cur_info=0;
    	int cur_code=0;

    	dword is_aps=0,is_10k2=0,dsp_flags=0,is_51=0,tmp=0,is_bad_sb22x=0,is_a2=0,is_k8=0;
    	dword can_passthru=0;
        dword is_edsp=0;
    	int is_doo=0;

        int need_spdif_swap=0;
        int output_r=0;
        int input_r=0;

        dsp_code *dyn_code=0;
        dsp_register_info *dyn_info=0;

        // from driver internals
	#define KX_DSP_SWAP_REAR		0x1
	#define KX_DSP_ROUTE_PH_TO_CSW		0x2

        ikx->get_dword(KX_DWORD_IS_APS,&is_aps);
        ikx->get_dword(KX_DWORD_IS_51,&is_51);
        ikx->get_dword(KX_DWORD_IS_10K2,&is_10k2);
        ikx->get_dword(KX_DWORD_CAN_PASSTHRU,&can_passthru);
        ikx->get_dword(KX_DWORD_IS_BAD_SB22X,&is_bad_sb22x);
        ikx->get_dword(KX_DWORD_IS_A2,&is_a2);
        ikx->get_dword(KX_DWORD_IS_K8,&is_k8);
        ikx->get_dword(KX_DWORD_IS_EDSP,&is_edsp);

        dword hw_compat=0;
        dword subsys=0;
        ikx->get_hw_parameter(KX_HW_COMPAT,&hw_compat);
        ikx->get_dword(KX_DWORD_SUBSYS,&subsys);

        if((is_a2 || (is_a2==0 && subsys==0x00521102)) && !is_k8 && !is_edsp)
        {
         // any audigy2-based board + sb0160 [a1]
         need_spdif_swap=1;
        }

        if(hw_compat&KX_HW_FORCE_SPDIF_SWAP)
         need_spdif_swap=1-need_spdif_swap;

        tmp=0;
        if(ikx->get_hw_parameter(KX_HW_DOO,&tmp)==0)
         if(tmp) is_doo=1;
        tmp=0;
        if(ikx->get_hw_parameter(KX_HW_SWAP_FRONT_REAR,&tmp)==0)
         if(tmp) dsp_flags|=KX_DSP_SWAP_REAR;
        tmp=0;
        if(ikx->get_hw_parameter(KX_HW_ROUTE_PH_TO_CSW,&tmp)==0)
         if(tmp) dsp_flags|=KX_DSP_ROUTE_PH_TO_CSW;

        int hw=0;

        #define EPILOG_SOURCE

	#include "legacy/prepare.h"
	begin_microcode("epilog");
    #include "../kxfxlib/da_epilog.cpp"
	end_microcode();

	#include "legacy/process.h"
	begin_microcode("epilog");
    #include "../kxfxlib/da_epilog.cpp"

    strncpy(name, epilog_name, sizeof(name));

    code=dyn_code;
    info=dyn_info;

	// you should free code and info massives in ::close() if allocated dynamically
	return 0;
}

const char *iEpilogPlugin::get_plugin_description(int id)
{
	plugin_description(epilog);
}

int iEpilogPlugin::close()
{
 if(code)
 {
  free((void *)code);
  code=NULL;
 }
 if(info)
 {
  free((void *)info);
  info=NULL;
 }

 return 0;
}

class iEpilogDSPWindow: public CKXDSPWindow
{
public:
 iEpilogDSPWindow(kDialog *parent_,kWindow *that_,iKXPlugin *plugin_,kFile *mf_)
  : CKXDSPWindow(parent_,that_,plugin_,mf_) { };
 virtual int find_gpr(const kPoint *pt,int *gpr,int flag,char *name=NULL);
};

iKXDSPWindow *iEpilogPlugin::create_dsp_wnd(kDialog *parent_,kWindow *that_,kFile *mf_)
{
 return new iEpilogDSPWindow(parent_,that_,this,mf_);
}

int iEpilogDSPWindow::find_gpr(const kPoint *pt,int *gpr,int flag,char *out_name)
{
 char l_name[KX_MAX_STRING]; l_name[0]=0;
 char tmp_name[KX_MAX_STRING]; tmp_name[0]=0;

 int ret=CKXDSPWindow::find_gpr(pt,gpr,flag,l_name);

 if(ret==0)
 {
  plugin->ikx->get_description(KX_EPILOG_NAME,plugin->info[*gpr].num,tmp_name);

  if(out_name)
  {
   out_name[0]=0;
   strcpy(out_name,tmp_name);
   strcat(out_name,l_name);
  }
 }

 return ret;
}
