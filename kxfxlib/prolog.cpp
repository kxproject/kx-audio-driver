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

// effect class
#include "prolog.h"
// effect source
#include "da_prolog.cpp"

// defined in 'epilog.cpp'
extern void my_alloc(int ,void **ret,size_t sz);
extern void my_free(int ,void *h);

int iPrologPlugin::request_microcode() // generally is called after init()
{
	strncpy(name, prolog_name, sizeof(name));

    	char *microcode_name;
    	int last_gpr=0x8000;
    	int last_input_gpr=0x4000;
    	int cur_info=0;
    	int cur_code=0;

    	dword is_aps=0,is_10k2=0,dsp_flags=0,is_51=0,is_edsp=0,tmp;
        int input_r=0;

        dsp_code *dyn_code=0;
        dsp_register_info *dyn_info=0;

        // from driver internals
	#define KX_DSP_SWAP_REAR		0x1
	#define KX_DSP_ROUTE_PH_TO_CSW		0x2

        ikx->get_dword(KX_DWORD_IS_APS,&is_aps);
        ikx->get_dword(KX_DWORD_IS_51,&is_51);
        ikx->get_dword(KX_DWORD_IS_10K2,&is_10k2);
        ikx->get_dword(KX_DWORD_IS_EDSP,&is_edsp);

        if(ikx->get_hw_parameter(KX_HW_SWAP_FRONT_REAR,&tmp)==0)
         if(tmp) dsp_flags|=KX_DSP_SWAP_REAR;
        if(ikx->get_hw_parameter(KX_HW_ROUTE_PH_TO_CSW,&tmp)==0)
         if(tmp) dsp_flags|=KX_DSP_ROUTE_PH_TO_CSW;

        int hw=0;

        #define PROLOG_SOURCE

	#include "legacy/prepare.h"
	begin_microcode("prolog");
        #include "../kxfxlib/da_prolog.cpp"
	end_microcode();

	#include "legacy/process.h"
	begin_microcode("prolog");
        #include "../kxfxlib/da_prolog.cpp"

    code=dyn_code;
    info=dyn_info;

	// you should free code and info massives in ::close() if allocated dynamically
	return 0;
}

const char *iPrologPlugin::get_plugin_description(int id)
{
	plugin_description(prolog);
}

int iPrologPlugin::close()
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

class iPrologDSPWindow: public CKXDSPWindow
{
public:
 iPrologDSPWindow(kDialog *parent_,kWindow *that_,iKXPlugin *plugin_,kFile *mf_)
  : CKXDSPWindow(parent_,that_,plugin_,mf_) { };
 virtual int find_gpr(const kPoint *pt,int *gpr,int flag,char *name=NULL);
};

iKXDSPWindow *iPrologPlugin::create_dsp_wnd(kDialog *parent_,kWindow *that_,kFile *mf_)
{
 return new iPrologDSPWindow(parent_,that_,this,mf_);
}

int iPrologDSPWindow::find_gpr(const kPoint *pt,int *gpr,int flag,char *out_name)
{
 char l_name[KX_MAX_STRING]; l_name[0]=0;
 char tmp_name[KX_MAX_STRING]; tmp_name[0]=0;

 int ret=CKXDSPWindow::find_gpr(pt,gpr,flag,l_name);

 if(ret==0)
 {
  plugin->ikx->get_description(KX_PROLOG_NAME,plugin->info[*gpr].num,tmp_name);

  if(out_name)
  {
   out_name[0]=0;
   strcpy(out_name,tmp_name);
   strcat(out_name,l_name);
  }
 }

 return ret;
}
