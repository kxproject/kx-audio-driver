// kX Driver Interface
// Copyright (c) Eugene Gavrilov, 2001-2005.

//  kX Project audio driver for Mac OS X
//  Created by Eugene Gavrilov.
//  Copyright 2008 Eugene Gavrilov. All rights reserved.
//  www.kxproject.com

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
#include "vers.h"

#ifdef WIN32
    #include "interface-win.cpp"
    #define api_alloc(a) LocalAlloc(LMEM_FIXED|LMEM_ZEROINIT,(a))
    #define api_free(a) LocalFree((HLOCAL)(a))
#elif defined(__APPLE__)
    #include "interface-osx.cpp"
    #define api_alloc(a) malloc(a)
    #define api_free(a) free(a)
#endif

iKX* iKX::create(int id)
{
  iKX *kx=NULL;

  kx = new iKX();

  if(!kx) return NULL;

  int ret=kx->init(id);

  if(ret==0) // ok?
       return kx;

  kx->destroy();

  return NULL;
}

void iKX::destroy(void)
{
  close();

  delete this;
}


int iKX::get_device_num()
{
 return device_num;
}

const char *iKX::get_device_name()
{
 return device_name;
}

const char *iKX::get_error_string()
{
 return error_name;
}


int iKX::set_buffers(int id,int val)
{
 get_property g;

    switch(id)
    {
        case KX_TANKMEM_BUFFER:
        case KX_PLAYBACK_BUFFER:
                case KX_RECORD_BUFFER:
                case KX_AC3_BUFFERS:
                case KX_GSIF_BUFFER:
           g.what=id;
           break;
                default:
                   debug("iKX set_buffers: invalid call [%d]\n",id);
                   return -1;
    }
    g.val=(dword)val;
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_SET|KX_PROP_SET_BUFFERS,&g,sizeof(g),&ret_b);
    if(ret || g.what==-1) // not found
     return -1;
    return 0;
}

int iKX::get_buffers(int id,int *val)
{
 get_property g;

    switch(id)
    {
        case KX_TANKMEM_BUFFER:
        case KX_PLAYBACK_BUFFER:
                case KX_RECORD_BUFFER:
                case KX_AC3_BUFFERS:
                case KX_GSIF_BUFFER:
           g.what=id;
           break;
                default:
                   debug("iKX get_buffers: invalid call [%d]\n",id);
                   return -1;
    }
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_GET_BUFFERS,&g,sizeof(g),&ret_b);
    if(ret || g.what==-1) // not found
     return -1;
        *val=(int)g.val;
    return 0;
}


int iKX::ac97_read(byte reg,word *val)
{
    ac97_property ac97;
    ac97.reg=reg;
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_AC97,&ac97,sizeof(ac97),&ret_b);
    if(ret)
     return ret;
        if(val)*val=ac97.val;
        return 0;
}

int iKX::ac97_write(byte reg,word val)
{
    ac97_property ac97;
    ac97.reg=reg;
    ac97.val=val;
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_SET|KX_PROP_AC97,&ac97,sizeof(ac97),&ret_b);
    if(ret)
     return ret;
        return 0;
}

int iKX::ac97_reset()
{
    ac97_property ac97;
    ac97.reg=0x0;
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_SET|KX_PROP_AC97,&ac97,sizeof(ac97),&ret_b);
    if(ret)
     return ret;
        return 0;
}

int iKX::ptr_read(dword reg,dword chn,dword *val)
{
    dword_property dw;
    dw.reg=reg;
    dw.chn=chn;
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_PTR,&dw,sizeof(dw),&ret_b);
    if(ret)
     return ret;
    
    if(val)*val=dw.val;
     return 0;
}

int iKX::ptr_write(dword reg,dword chn,dword val)
{
    dword_property dw;
    dw.reg=reg;
    dw.chn=chn;
    dw.val=val;
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_SET|KX_PROP_PTR,&dw,sizeof(dw),&ret_b);
    if(ret)
     return ret;

    return 0;
}

int iKX::fn0_read(dword reg,dword *val)
{
    dword_property dw;
    dw.reg=reg;
    dw.chn=0x0;
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_FN0,&dw,sizeof(dw),&ret_b);
    if(ret)
     return ret;

    if(val)*val=dw.val;
     return 0;
}


int iKX::fn0_write(dword reg,dword val)
{
    dword_property dw;
    dw.reg=reg;
    dw.chn=0x0;
    dw.val=val;
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_SET|KX_PROP_FN0,&dw,sizeof(dw),&ret_b);
    if(ret)
     return ret;


    return 0;
}

int iKX::gpio_read(dword *in)
{
    dword_property dw;
    dw.reg=0;
    dw.chn=0x0;
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_GPIO_GET,&dw,sizeof(dw),&ret_b);
    if(ret)
     return ret;

        if(in) *in=dw.val;

        return 0;
}

int iKX::gpio_write(dword out)
{
    dword_property dw;
    dw.reg=out;
    dw.chn=0x0;
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_GPIO_SET,&dw,sizeof(dw),&ret_b);
    if(ret)
     return ret;

        return 0;
}

int iKX::p16v_read(dword reg,dword *val)
{
    dword_property dw;
    dw.reg=reg;
    dw.chn=0x0;
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_P16V,&dw,sizeof(dw),&ret_b);
    if(ret)
     return ret;
        if(val)*val=dw.val;
        return 0;
}

int iKX::p16v_write(dword reg,dword val)
{
    dword_property dw;
    dw.reg=reg;
    dw.chn=0x0;
    dw.val=val;
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_SET|KX_PROP_P16V,&dw,sizeof(dw),&ret_b);
    if(ret)
     return ret;
        return 0;
}

int iKX::get_hw_parameter(int id,dword *val)
{
    dword_property dw;
    dw.reg=id;
    dw.chn=0x0;
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_HW_PARAM,&dw,sizeof(dw),&ret_b);
    if(ret)
     return ret;
        if(val)
         *val=dw.val;
        return dw.reg; // 0-success
}

int iKX::set_hw_parameter(int id,dword val)
{
    dword_property dw;
    dw.reg=id;
    dw.chn=0x0;
    dw.val=val;
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_SET|KX_PROP_HW_PARAM,&dw,sizeof(dw),&ret_b);
    if(ret)
     return ret;
        return 0;
}

int iKX::get_dword(int n,dword *val)
{
    get_property get;
    get.what=n;
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_GET_DWORD,&get,sizeof(get),&ret_b);
    if(ret || get.what==-1) // prop not found
     return -1;
        if(val)*val=get.val;
    return 0;
}

int iKX::get_string(int n,char *str)
{
    get_property get;
    get.what=n;
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_GET_STRING,&get,sizeof(get),&ret_b);
    if(ret || get.what==-1) // prop not found
     return -1;
    strncpy(str,get.str,KX_MAX_STRING);
    return 0;
}

int iKX::get_routing(int ndx, dword *routing, dword *xrouting)
{
    routing_property r;
    r.ndx=ndx;
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_ROUTING,&r,sizeof(r),&ret_b);
    if(ret || r.ndx==-1) // not found
     return -1;
        if(routing)
         *routing=r.routing;
        if(xrouting)
         *xrouting=r.xrouting;
        return 0;
}

int iKX::set_routing(int ndx, dword routing, dword xrouting)
{
    routing_property r;
    r.ndx=ndx;
    r.routing=routing;
    r.xrouting=xrouting;
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_SET|KX_PROP_ROUTING,&r,sizeof(r),&ret_b);
    if(ret || r.ndx==-1) // not found
     return -1;
    return 0;
}

int iKX::get_send_amount(int ndx, byte *amount)
{
    routing_property r;
    r.ndx=ndx;
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_AMOUNT,&r,sizeof(r),&ret_b);
    if(ret || r.ndx==-1) // not found
     return -1;
        *amount=(byte)r.routing;
        return 0;
}

int iKX::set_send_amount(int ndx, byte amount)
{
    routing_property r;
    r.ndx=ndx;
    r.routing=amount;
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_SET|KX_PROP_AMOUNT,&r,sizeof(r),&ret_b);
    if(ret || r.ndx==-1) // not found
     return -1;
    return 0;
}

int iKX::set_dsp_register(int pgm,const char *name,dword val)
{
    dsp_register_property p;
    strncpy(p.name,name,MAX_GPR_NAME);
    p.val=val;
    p.pgm=pgm;
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_SET|KX_PROP_DSP_REGISTER_NAME,&p,sizeof(p),&ret_b);
    if(ret)
     return ret;
    return 0;
}

int iKX::get_dsp_register(int pgm,const char *name,dword *val)
{
    dsp_register_property p;
    strncpy(p.name,name,MAX_GPR_NAME);
    p.pgm=pgm;

    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_DSP_REGISTER_NAME,&p,sizeof(p),&ret_b);
    if(ret || p.id==0xffff)
     return -1;
        if(val)*val=p.val;
        return 0;
}

int iKX::set_tram_addr(int pgm,const char *name,dword addr) // addr is _logical_
{
    dsp_register_property p;
    strncpy(p.name,name,MAX_GPR_NAME);
    p.val=addr;
    p.pgm=pgm;
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_SET|KX_PROP_TRAM_ADDR_NAME,&p,sizeof(p),&ret_b);
    if(ret)
     return ret;
    return 0;
}

int iKX::get_tram_addr(int pgm,const char *name,dword *addr)
{
    dsp_register_property p;
    strncpy(p.name,name,MAX_GPR_NAME);
    p.pgm=pgm;

    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_TRAM_ADDR_NAME,&p,sizeof(p),&ret_b);
    if(ret || p.id==0xffff)
     return -1;
        if(addr) *addr=p.val;
        return 0;
}

int iKX::set_tram_flag(int pgm,const char *name,dword flag) // flag is TRAM_READ || TRAM_WRITE
{
    dsp_register_property p;
    strncpy(p.name,name,MAX_GPR_NAME);
    p.val=flag;
    p.pgm=pgm;
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_SET|KX_PROP_TRAM_FLAG_NAME,&p,sizeof(p),&ret_b);
    if(ret)
     return ret;
    return 0;
}

int iKX::get_tram_flag(int pgm,const char *name,dword *flag)
{
    dsp_register_property p;
    strncpy(p.name,name,MAX_GPR_NAME);
    p.pgm=pgm;

    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_TRAM_FLAG_NAME,&p,sizeof(p),&ret_b);
    if(ret || p.id==0xffff)
     return -1;
        if(flag)*flag=p.val;
        return 0;
}

int iKX::set_tram_addr(int pgm,word id,dword addr) // addr is _logical_
{
    dsp_register_property p;
        p.id=id;
    p.val=addr;
    p.pgm=pgm;
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_SET|KX_PROP_TRAM_ADDR_ID,&p,sizeof(p),&ret_b);
    if(ret)
     return ret;
    return 0;
}

int iKX::get_tram_addr(int pgm,word id,dword *addr)
{
    dsp_register_property p;
        p.id=id;
    p.pgm=pgm;

    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_TRAM_ADDR_ID,&p,sizeof(p),&ret_b);
    if(ret || p.id==0xffff)
     return -1;
        if(addr) *addr=p.val;
        return 0;
}

int iKX::set_tram_flag(int pgm,word id,dword flag) // flag is TRAM_READ || TRAM_WRITE
{
    dsp_register_property p;
        p.id=id;
    p.val=flag;
    p.pgm=pgm;
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_SET|KX_PROP_TRAM_FLAG_ID,&p,sizeof(p),&ret_b);
    if(ret)
     return ret;
    return 0;
}

int iKX::get_tram_flag(int pgm,word id,dword *flag)
{
    dsp_register_property p;
    p.pgm=pgm;
    p.id=id;

    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_TRAM_FLAG_ID,&p,sizeof(p),&ret_b);
    if(ret || p.id==0xffff)
     return -1;
        if(flag)*flag=p.val;
        return 0;
}

int iKX::write_instruction(int pgm,int offset,word op,word z,word w,word x,word y,int valid)
{
    dsp_instruction_property p;
    p.pgm=pgm;
    p.offset=offset;
    p.op=op; p.z=z; p.w=w; p.x=x; p.y=y; p.valid=valid;

    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_INSTRUCTION_WRITE,&p,sizeof(p),&ret_b);
    if(ret || p.pgm==0xffff)
     return -1;
        return 0;
}

int iKX::read_instruction(int pgm,int offset,word *op,word *z,word *w,word *x,word *y)
{
    dsp_instruction_property p;
    p.pgm=pgm;
    p.offset=offset;

    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_INSTRUCTION_READ,&p,sizeof(p),&ret_b);
    if(ret || p.pgm==0xffff)
     return -1;

    if(op) *op=p.op;
        if(z) *z=p.z;
        if(w) *w=p.w;
        if(x) *x=p.x;
        if(y) *y=p.y;
        return 0;
}

int iKX::set_dsp_register(int pgm,word id,dword val)
{
    dsp_register_property p;
    p.id=id;
    p.val=val;
    p.pgm=pgm;
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_SET|KX_PROP_DSP_REGISTER_ID,&p,sizeof(p),&ret_b);
    if(ret)
     return -1;
    return 0;
}

int iKX::get_dsp_register(int pgm,word id,dword *val)
{
    dsp_register_property p;
    p.id=id;
    p.pgm=pgm;
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_DSP_REGISTER_ID,&p,sizeof(p),&ret_b);
    if(ret || p.id==0xffff) // not found
     return -1;
        if(val)*val=p.val;
        return 0;
}

int iKX::translate_microcode(int pgm,int place,int pos_pgm)
{
    microcode_property p;
    p.cmd=KX_PROP_MICROCODE_TRANSLATE;
    p.p1=place;
    p.p2=pos_pgm;
    p.pgm=pgm;
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_MICROCODE,&p,sizeof(p),&ret_b);
    if(ret)
     return -1;
    if(p.cmd)
     return p.cmd;
    return 0;
}

int iKX::dsp_go()
{
    microcode_property p;
    p.cmd=KX_PROP_MICROCODE_DSP_GO;
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_MICROCODE,&p,sizeof(p),&ret_b);
    if(ret)
     return -1;
    return 0;
}

int iKX::dsp_stop()
{
    microcode_property p;
    p.cmd=KX_PROP_MICROCODE_DSP_STOP;
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_MICROCODE,&p,sizeof(p),&ret_b);
    if(ret)
     return -1;
    return 0;
}

int iKX::dsp_clear() // unloads all uploaded microcode; frees all hw resources; assumes dsp_stop();
{
    microcode_property p;
    p.cmd=KX_PROP_MICROCODE_DSP_CLEAR;
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_MICROCODE,&p,sizeof(p),&ret_b);
    if(ret)
     return -1;
    return 0;
}

int iKX::enable_microcode(int pgm)
{
    microcode_property p;
    p.cmd=KX_PROP_MICROCODE_ENABLE;
    p.p1=0;
    p.p2=0;
    p.pgm=pgm;
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_MICROCODE,&p,sizeof(p),&ret_b);
    if(ret)
     return -1;
    if(p.cmd)
     return p.cmd;
    return 0;
}

int iKX::set_microcode_bypass(int pgm,int state)
{
    microcode_property p;
    p.cmd=(state?KX_PROP_MICROCODE_BYPASS_ON:KX_PROP_MICROCODE_BYPASS_OFF);
    p.p1=0;
    p.p2=0;
    p.pgm=pgm;
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_MICROCODE,&p,sizeof(p),&ret_b);
    if(ret)
     return -1;
    if(p.cmd)
     return p.cmd;
    return 0;
}

int iKX::untranslate_microcode(int pgm)
{
    microcode_property p;
    p.cmd=KX_PROP_MICROCODE_UNTRANSLATE;
    p.p1=0;
    p.p2=0;
    p.pgm=pgm;
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_MICROCODE,&p,sizeof(p),&ret_b);
    if(ret)
     return -1;
    if(p.cmd)
     return p.cmd;
    return 0;
}

int iKX::disable_microcode(int pgm)
{
    microcode_property p;
    p.cmd=KX_PROP_MICROCODE_DISABLE;
    p.p1=0;
    p.p2=0;
    p.pgm=pgm;
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_MICROCODE,&p,sizeof(p),&ret_b);
    if(ret)
     return -1;
    if(p.cmd)
     return p.cmd;
    return 0;
}

int iKX::set_microcode_flag(int pgm,dword state) // (dsp_microcode.flag)
{
    microcode_property p;
    p.cmd=KX_PROP_MICROCODE_SET_FLAG;
    p.p1=state;
    p.p2=0;
    p.pgm=pgm;
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_MICROCODE,&p,sizeof(p),&ret_b);
    if(ret)
     return -1;
    if(p.cmd)
     return p.cmd;
    return 0;
}

int iKX::get_microcode_flag(int pgm,dword *state) // (dsp_microcode.flag)
{
    microcode_property p;
    p.cmd=KX_PROP_MICROCODE_GET_FLAG;
    p.p1=0;
    p.p2=0;
    p.pgm=pgm;
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_MICROCODE,&p,sizeof(p),&ret_b);
    if(ret)
     return -1;

    if(state)
     *state=p.p1;

    if(p.cmd)
     return p.cmd;
    return 0;
}

int iKX::unload_microcode(int pgm)
{
    microcode_property p;
    p.cmd=KX_PROP_MICROCODE_UNLOAD;
    p.p1=0;
    p.p2=0;
    p.pgm=pgm;
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_MICROCODE,&p,sizeof(p),&ret_b);
    if(ret)
     return -1;
    if(p.cmd)
     return p.cmd;
    return 0;
}

int iKX::connect_microcode(int pgm1,word src,int pgm2,word dst)
{
    microcode_connect_property p;
    p.op=0;
    p.pgm1=pgm1;
    p.pgm2=pgm2;
    p.src_w=src;
    p.dst_w=dst;
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_MICROCODE_CONNECT_ID,&p,sizeof(p),&ret_b);
    if(ret)
     return -1;
    if(p.op)
     return p.op;
    return 0;
}

int iKX::disconnect_microcode(int pgm1,word src)
{
    microcode_connect_property p;
    p.op=0;
    p.pgm1=pgm1;
    p.src_w=src;
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_MICROCODE_DISCONNECT_ID,&p,sizeof(p),&ret_b);
    if(ret)
     return -1;
    if(p.op)
     return p.op;
    return 0;
}

int iKX::connect_microcode(int pgm1,const char *src,int pgm2,const char *dst)
{
    microcode_connect_property p;
    p.op=0;
    p.pgm1=pgm1;
    p.pgm2=pgm2;
    strncpy(p.src_c,src,MAX_GPR_NAME);
    strncpy(p.dst_c,dst,MAX_GPR_NAME);
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_MICROCODE_CONNECT_NAME,&p,sizeof(p),&ret_b);
    if(ret)
     return -1;
    if(p.op)
     return p.op;
    return 0;
}
int iKX::disconnect_microcode(int pgm1,const char *src)
{
    microcode_connect_property p;
    p.op=0;
    p.pgm1=pgm1;
    strncpy(p.src_c,src,MAX_GPR_NAME);
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_MICROCODE_DISCONNECT_NAME,&p,sizeof(p),&ret_b);
    if(ret)
     return -1;
    if(p.op)
     return p.op;
    return 0;
}


int iKX::enum_microcode(int pgm,dsp_microcode *mc)
{
    microcode_enum_property p;
    p.pgm=pgm;
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_MICROCODE_ENUM_ID,&p,sizeof(p),&ret_b);
    if(ret)
     return -1;
    
    memcpy(mc,&p.m,sizeof(dsp_microcode));
           
    return 0;
}

int iKX::enum_microcode(const char *pgm_id,dsp_microcode *mc)
{
    microcode_enum_property p;
    strncpy(p.name,pgm_id,KX_MAX_STRING);
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_MICROCODE_ENUM_NAME,&p,sizeof(p),&ret_b);
    if(ret)
     return -1;
        memcpy(mc,&p.m,sizeof(dsp_microcode));
    return 0;
}

// returns pgm id or 0 if failed
int iKX::load_microcode(const char *name,const dsp_code *code,int code_size,
    const dsp_register_info *info,int info_size,int itramsize,int xtramsize,
        const char *copyright,
        const char *engine,
        const char *created,
        const char *comment,
        const char *guid,
        int force_pgm_id)
{
 char *m=NULL;
 dsp_microcode dsp_m;
 m=(char *)api_alloc(4+       // magic
            sizeof(dsp_microcode)+
            4+ // magic
            info_size+
            code_size+
            4+ // force_pgm_id
            4);  // magic
 if(m)
 {
    *(dword *)m=LOAD_MICROCODE_MAGIC;

    memset(&dsp_m,0,sizeof(dsp_microcode));
    if(name) strncpy(dsp_m.name,name,KX_MAX_STRING);
        if(copyright) strncpy(dsp_m.copyright,copyright,KX_MAX_STRING);
        if(engine) strncpy(dsp_m.engine,engine,KX_MAX_STRING);
        if(created) strncpy(dsp_m.created,created,KX_MAX_STRING);
        if(comment) strncpy(dsp_m.comment,comment,KX_MAX_STRING);
        if(guid) strncpy(dsp_m.guid,guid,KX_MAX_STRING);
        dsp_m.code_size=code_size;
        dsp_m.info_size=info_size;
        dsp_m.itramsize=itramsize;
        dsp_m.xtramsize=xtramsize;

        memcpy(m+4,&dsp_m,sizeof(dsp_microcode));
        *(dword *)(m+4+sizeof(dsp_microcode))=LOAD_MICROCODE_MAGIC;
        memcpy(m+4+sizeof(dsp_microcode)+4,info,info_size);
        memcpy(m+4+sizeof(dsp_microcode)+4+info_size,code,code_size);
        *(dword *)(m+4+sizeof(dsp_microcode)+4+info_size+code_size)=force_pgm_id;
        *(dword *)(m+4+sizeof(dsp_microcode)+4+info_size+code_size+4)=LOAD_MICROCODE_MAGIC;

    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_LOAD_MICROCODE,m,
      4+(int)sizeof(dsp_microcode)+4+info_size+code_size+4+4,&ret_b);
    if(ret)
    {
     api_free(m);
     return 0;
     }

    int id=(int) (*(dword *)m);
    api_free(m);
    return id;
 }
  else
  {
   debug("iKX load_microcode(): LocalAlloc failed [%x]\n",GetLastError());
  }
 return 0;
}

int iKX::update_microcode(int pgm_id,const char *name,dsp_code *code,int code_size,
        dsp_register_info *info,int info_size,int itramsize,int xtramsize,
        const char *copyright,
        const char *engine,
        const char *created,
        const char *comment,
        const char *guid,
        unsigned int flag)
{
 char *m=NULL;
 dsp_microcode dsp_m;
 m=(char *)api_alloc(4+       // magic
            sizeof(dsp_microcode)+
            4+ // magic
            info_size+
            code_size+
            4+ // flag
            4+ // pgm_id
            4);  // magic
 if(m)
 {
    *(dword *)m=UPDATE_MICROCODE_MAGIC;

    memset(&dsp_m,0,sizeof(dsp_microcode));
    if(name) strncpy(dsp_m.name,name,KX_MAX_STRING);
        if(copyright) strncpy(dsp_m.copyright,copyright,KX_MAX_STRING);
        if(engine) strncpy(dsp_m.engine,engine,KX_MAX_STRING);
        if(created) strncpy(dsp_m.created,created,KX_MAX_STRING);
        if(comment) strncpy(dsp_m.comment,comment,KX_MAX_STRING);
        if(guid) strncpy(dsp_m.guid,guid,KX_MAX_STRING);
        dsp_m.code_size=code_size;
        dsp_m.info_size=info_size;
        dsp_m.itramsize=itramsize;
        dsp_m.xtramsize=xtramsize;

        memcpy(m+4,&dsp_m,sizeof(dsp_microcode));
        *(dword *)(m+4+sizeof(dsp_microcode))=UPDATE_MICROCODE_MAGIC;
        memcpy(m+4+sizeof(dsp_microcode)+4,info,info_size);
        memcpy(m+4+sizeof(dsp_microcode)+4+info_size,code,code_size);
        *(dword *)(m+4+sizeof(dsp_microcode)+4+info_size+code_size)=flag;
        *(dword *)(m+4+sizeof(dsp_microcode)+4+info_size+code_size+4)=pgm_id;
        *(dword *)(m+4+sizeof(dsp_microcode)+4+info_size+code_size+4+4)=UPDATE_MICROCODE_MAGIC;

    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_UPDATE_MICROCODE,m,
      4+(int)sizeof(dsp_microcode)+4+info_size+code_size+4+4+4,&ret_b);
    if(ret)
    {
     api_free(m);
     return 0;
     }

    int id=(int) (*(dword *)m);
    api_free(m);
    return id;
 }
  else
  {
   debug("iKX update_microcode(): LocalAlloc failed [%x]\n",GetLastError());
  }
 return 0;
}


int iKX::get_microcode(int pgm,dsp_code *code,int code_size,
    dsp_register_info *info,int info_size)
{
 dword *m=NULL;
 m=(dword *)api_alloc(4+4+        // magic
                code_size+4+4+    // code; code_size
                info_size+4+4+    // info; info_size
                4);       
 if(m)
 {
    m[0]=GET_MICROCODE_MAGIC;
    m[1]=(dword)code_size;
    m[2]=(dword)info_size;
    m[3]=(dword)pgm;

    int code_off=4;
    int info_off=code_off+code_size/4+1;
    int last_off=info_off+info_size/4+1;
    m[last_off]=GET_MICROCODE_MAGIC;

    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_GET_MICROCODE,m,(last_off+1)*4,&ret_b);
    if(ret || m[last_off]!=GET_MICROCODE_MAGIC)
    {
     api_free(m);
     return -2;
     }

        // copy mem
    if(code_size && code)
        memcpy(code,&m[code_off],code_size);
    if(info_size && info)
        memcpy(info,&m[info_off],info_size);

        ret=(int)m[0];
    api_free(m);
    return ret;
 }
 return -1;
}

int iKX::reset_microcode(void)
{
    dword_property p;
    p.reg=1; // true; reset confirm
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_RESET_MICROCODE,&p,sizeof(p),&ret_b);
    if(ret || p.reg) // error
     return -1;
        return 0;
}


// SoundFont
// ---------

int iKX::load_soundfont(kx_sound_font *fnt,int partial) // returns id or <0 -error
{
 int ret;
 int ret_b;
 int id;

 int sample_len=fnt->header.sample_len;
 int fnt_size=fnt->size;
 char *p=(char *)((uintptr_t)fnt+(uintptr_t)fnt_size-(uintptr_t)sample_len-4-1);

 if(*(dword *)((uintptr_t)p+(uintptr_t)sample_len)!=KX_SOUNDFONT_MAGIC)
 {
  debug("iKX load_sf: invalid fnt\n");
  return -10;
 }

 ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_SOUNDFONT_LOAD,fnt,fnt_size-sample_len-4,&ret_b);
 if(ret==0)
 {
  id=fnt->id; // save id
  if(id<=0)
   return id;

  int count=sample_len+4;
  int pos=0;

 if(!partial) 
 {
  while(count>0)
  {
   sf_load_sample_property sf_l;
   int sfl_size=(count<(4096-12))?count:(4096-12);
   sf_l.id=id;
   sf_l.size=sfl_size;
   sf_l.pos=pos;
   memcpy(&sf_l.data[0],p,sf_l.size);
   ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_SOUNDFONT_LOAD_SMPL,&sf_l,sizeof(sf_l),&ret_b);

   count-=sfl_size;
   pos+=sfl_size;
   p+=sfl_size;
  }
 }
 if(ret==0) // all was ok
  ret=id;
 }
 return ret;
}

int iKX::load_soundfont_x(kx_sound_font *fnt,const char *fname,long f_pos) // returns id or <0 -error
{
 FILE *f=fopen(fname,"rb");
 if(f==0)
  return -5;
 fseek(f,f_pos,SEEK_SET);

 int ret;
 int ret_b;
 int id;

 int sample_len=fnt->header.sample_len;
 int fnt_size=fnt->size;

 char *p_t=(char *)((uintptr_t)fnt+(uintptr_t)fnt_size-(uintptr_t)sample_len-4-1);

 if(*(dword *)p_t!=KX_SOUNDFONT_MAGIC)
 {
  debug("iKX load_sf_x: invalid fnt\n");
  fclose(f);
  return -10;
 }

 ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_SOUNDFONT_LOAD,fnt,fnt_size-sample_len-4,&ret_b);
 if(ret==0)
 {
  id=fnt->id; // save id
  if(id<=0)
  {
   fclose(f);
   return id;
  }

  int count=sample_len+4;
  int pos=0;

  while(count>0)
  {
   sf_load_sample_property sf_l;
   int sfl_size=(count<(4096-12))?count:(4096-12);
   sf_l.id=id;
   sf_l.size=sfl_size;
   sf_l.pos=pos;

   char tmp_mem[4096-12];
   fread(tmp_mem,4096-12,1,f);

   memcpy(&sf_l.data[0],tmp_mem,sf_l.size);
   ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_SOUNDFONT_LOAD_SMPL,&sf_l,sizeof(sf_l),&ret_b);

   count-=sfl_size;
   pos+=sfl_size;
  }

 if(ret==0) // all was ok
  ret=id;
 }

 fclose(f);
 return ret;
}

int iKX::unload_soundfont(int id)
{
 int ret;
 int ret_b;

 int result=id;

 ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_SOUNDFONT_UNLOAD,&result,sizeof(result),&ret_b);
 if(ret==0)
  ret=result;
 return ret;
}

int iKX::get_voice_info(kx_voice_info *vi)
{
 int ret;
 int ret_b;

 ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_VOICE_INFO,vi,sizeof(kx_voice_info)*64,&ret_b);
 return ret;
}

int iKX::get_spectral_info(kx_voice_info *vi)
{
 int ret;
 int ret_b;

 ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_SPECTRAL_INFO,vi,sizeof(kx_voice_info)*64,&ret_b);
 return ret;
}

int iKX::reset_settings()
{
 int ret;
 int ret_b;
 dword v=0;

 ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_RESET_SETTINGS,&v,sizeof(v),&ret_b);
 return ret;
}

int iKX::reset_db()
{
 int ret;
 int ret_b;
 dword v=0;

 ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_RESET_DB,&v,sizeof(v),&ret_b);
 return ret;
}

int iKX::reset_voices()
{
 int ret;
 int ret_b;
 dword v=0;

 ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_RESET_VOICES,&v,sizeof(v),&ret_b);
 return ret;
}


int iKX::enum_soundfonts(sfHeader *hdr,int size) // if size==0 - ret is # of bytes needed
{
 int ret;
 int ret_b;

 if(size==0)
 {
   sf_query_property q;
   q.size=0;
   ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_SOUNDFONT_QUERY,&q,sizeof(q),&ret_b);
   if(ret==0)
    ret=q.size;
 }
 else
 {
   if(hdr==0 || size<=0)
    return -3;
   int *p=(int *)hdr;
   *p=size;
   ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_SOUNDFONT_ENUM,hdr,size,&ret_b);
 }
 return ret;
}

int iKX::get_preset_description(int bank,int preset,char *name)
{
 int ret;
 int ret_b;

   sfpreset_query_property p;
   p.bank=bank;
   p.preset=preset;
   ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_SOUNDFONT_PRESET_DESCR,&p,sizeof(p),&ret_b);
   if(ret==0)
   {
    if((p.bank==0) && (p.preset==0)) // if set -- ok
     strncpy(name,p.name,20);
    else
     return -1; // error
   }

 return ret;
}


int iKX::enum_microcode(dsp_microcode *mc,int size) // if size==0 - ret is # of bytes needed
{
 int ret;
 int ret_b;

 if(size==0)
 {
   dword sz=0;
   ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_MICROCODE_QUERY,&sz,sizeof(sz),&ret_b);
   if(ret==0)
    ret=sz;
 }
 else
 {
   if(mc==0 || size<=0)
    return -3;
   int *p=(int *)mc;
   *p=size;
   ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_MICROCODE_ENUM_ALL,mc,size,&ret_b);
 }
 return ret;
}


int iKX::set_dsp(int what)
{
 is_10k2=what;
 return 0;
}

int iKX::get_dsp(void)
{
 return is_10k2;
}

dword iKX::get_version_dword()
{
 return KX_VERSION_DWORD;
}

const char *iKX::get_version()
{
 return KX_DRIVER_VERSION_STR;
}

int iKX::mute()
{
    dword tmp=1;
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_MUTE,&tmp,sizeof(tmp),&ret_b);
    if(ret)
     return ret;
        return 0;
}

int iKX::unmute()
{
    dword tmp=1;
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_UNMUTE,&tmp,sizeof(tmp),&ret_b);
    if(ret)
     return ret;
        return 0;
}

int iKX::send_synth(int synth_id,dword data)
{
    kx_synth_data prop;
    prop.synth_id=synth_id;
    prop.data=data;
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_SOUNDFONT_SYNTH,&prop,sizeof(prop),&ret_b);
    if(ret)
     return ret;
        return 0;
}

int iKX::get_connections(int pgm,kxconnections *out,int size)
{
 int ret_b=0;

 if(size==0)
 {
  int tmp=pgm;
  int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_GET_CONNECTIONS_QUERY,&tmp,sizeof(tmp),&ret_b);
  if(ret)
   return -12;
  return tmp;
 }
 else
 {
  int *tmp;
  tmp=(int *)out;
  tmp[0]=pgm; tmp[1]=size;
  int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_GET_CONNECTIONS,tmp,size,&ret_b);
  if(ret)
   return -14;
  return 0;
 }
}

int iKX::set_microcode_name(int pgm_id,const char *new_name,int what)
{
    microcode_enum_property p;
    strncpy(p.name,new_name,KX_MAX_STRING);
    p.pgm=pgm_id;
    p.m.flag=what;
        
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_MICROCODE_SET_NAME,&p,sizeof(p),&ret_b);
    if(ret)
     return -1;
    return 0;
}

int iKX::get_spdif_i2s_status(kx_spdif_i2s_status *st)
{
 int ret;
 int ret_b;

 ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_SPDIF_I2S_STATE,st,sizeof(kx_spdif_i2s_status),&ret_b);
 return ret;
}

int iKX::get_dsp_assignments(kx_assignment_info *ai)
{
 int ret;
 int ret_b;

 ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_DSP_ASSIGN,ai,sizeof(kx_assignment_info),&ret_b);
 return ret;
}

int iKX::set_dsp_assignments(kx_assignment_info *ai)
{
 int ret;
 int ret_b;

 ret=ctrl(KX_TOPO|KX_PROP_SET|KX_PROP_DSP_ASSIGN,ai,sizeof(kx_assignment_info),&ret_b);
 return ret;
}

int iKX::translate_text2id(kx_text2id *ti)
{
 int ret;
 int ret_b;

 ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_TEXT2ID,ti,sizeof(kx_text2id),&ret_b);
 return ret;
}

int iKX::get_description(int what,int p,char *name)
{
 int found=1;

 // modify tooltip here...
 dword is_aps=0,is_10k2_=0,dsp_flags=0,is_51=0,is_a2ex=0,is_edsp=0,tmp;

 // from driver internals
 #define KX_DSP_SWAP_REAR           0x1
 #define KX_DSP_ROUTE_PH_TO_CSW         0x2
 #define KX_FLAG_DOO                0x4

 get_dword(KX_DWORD_IS_APS,&is_aps);
 get_dword(KX_DWORD_IS_51,&is_51);
 get_dword(KX_DWORD_IS_10K2,&is_10k2_);
 get_dword(KX_DWORD_IS_A2EX,&is_a2ex);
 get_dword(KX_DWORD_IS_EDSP,&is_edsp);

 if(get_hw_parameter(KX_HW_SWAP_FRONT_REAR,&tmp)==0)
   if(tmp) dsp_flags|=KX_DSP_SWAP_REAR;
 if(get_hw_parameter(KX_HW_ROUTE_PH_TO_CSW,&tmp)==0)
   if(tmp) dsp_flags|=KX_DSP_ROUTE_PH_TO_CSW;
 if(get_hw_parameter(KX_HW_DOO,&tmp)==0)
   if(tmp) dsp_flags|=KX_FLAG_DOO;

 kString s;
 s="?";

 switch(what)
 {
  case KX_CARD_NAME:
    {
     found=0;
    }
    break;
  case KX_OUTPUT_NAME:
    // translate KX_OUT(a) -> 0x4000+b
    switch(p)
    {
     case KX_OUT(0): p=0x4000; break;
     case KX_OUT(1): p=0x4001; break;
     case KX_OUT(2): p=0x4002; break;
     case KX_OUT(3): p=0x4003; break;
     case KX_OUT(4): p=0x4004; break;
     case KX_OUT(5): p=0x4005; break;
     case KX_OUT(6): p=0x4006; break;
     case KX_OUT(7): p=0x4007; break;
     case KX_OUT(8): p=0x4008; break;
     case KX_OUT(9): p=0x4009; break;
     case KX_OUT(0x11): p=0x400a; break;
     case KX_OUT(0x12): p=0x400b; break;
     case KX_OUT(0x30): p=0x400c; break;
     case KX_OUT(0x31): p=0x400d; break;

     case KX_OUT(0x20): if(is_10k2_) { p=-1; s="Pseudo AC97 L"; } break;
     case KX_OUT(0x21): if(is_10k2_) { p=-1; s="Pseudo AC97 R"; } break;
     case KX_OUT(0x28): if(is_10k2_) { p=-1; s="k2 Digital Rear L"; } break;
     case KX_OUT(0x29): if(is_10k2_) { p=-1; s="k2 Digital Rear R"; } break;
    }
    if(p==-1)
     break;

        // fall-thru:
  case KX_EPILOG_NAME:
    {
             if(is_aps)
             {
               switch(p)
               {
                case 0x4000: s="A1 "; break;
                case 0x4001: s="A2 "; break;
                case 0x4002: s="D1 L "; break;
                case 0x4003: s="D1 R "; break;
                case 0x4004: s="A3/D2 L "; break;
                case 0x4005: s="A4/D2 R "; break;
                case 0x4006: s="A5 "; break;
                case 0x4007: s="A6 "; break;
                case 0x4008: s="A7 "; break;
                case 0x4009: s="A8 "; break;
//                case 0x400a: s="unused "; break;
//                case 0x400b: s="unused "; break;
//                case 0x400c: s="unused "; break;
//                case 0x400d: s="unused "; break;
                default: found=0; break;
               }
             }
             else
              {
               switch(p)
               {
                case 0x4000: 
                    if(dsp_flags&KX_FLAG_DOO) 
                      s="Analog Front Left (muted) "; 
                    else
                      s="Analog Front Left / I2S0 Left ";
                    if((dsp_flags&KX_DSP_SWAP_REAR)&&(!(dsp_flags&KX_FLAG_DOO)))
                     s+="[swapped] ";
                    break;
                case 0x4001: 
                    if(dsp_flags&KX_FLAG_DOO) 
                      s="Analog Front Right (muted) "; 
                    else
                      s="Analog Front Right / I2S0 Right ";
                    if(dsp_flags&KX_DSP_SWAP_REAR)
                     s+="[swapped] ";
                    break;
                case 0x4002: 
                    if(dsp_flags&KX_FLAG_DOO)
                      s="Digital Front / SPDIF0 Left "; 
                    else
                      s="Generic SPDIF Left ";
                    break;
                case 0x4003:
                    if(dsp_flags&KX_FLAG_DOO)
                      s="Digital Front / SPDIF0 Right "; 
                    else
                      s="Generic SPDIF Right ";
                    break;
                case 0x4004:
                    if((dsp_flags&KX_FLAG_DOO)||(is_10k2_))
                      s="Digital Center / SPDIF1 Left "; 
                    else
                      s="SPDIF1 Left ";
                    break;
                case 0x4005:
                    if((dsp_flags&KX_FLAG_DOO)||(is_10k2_))
                      s="Digital SubWoofer / SPDIF1 Right "; 
                    else
                      s="SPDIF1 Right ";
                    break;
                case 0x4006:
                      s="Headphones Left / SPDIF2 Left "; 
                    break;
                case 0x4007:
                      s="Headphones Right / SPDIF2 Right "; 
                    break;
                case 0x4008:
                    if(dsp_flags&KX_FLAG_DOO) 
                      s="Digital Rear / SPDIF3 Left "; 
                    else
                      s="Rear Left (Analog and Digital) ";
                    if(dsp_flags&KX_DSP_SWAP_REAR)
                     s+="[swapped] ";
                    break;
                case 0x4009:
                    if(dsp_flags&KX_FLAG_DOO) 
                      s="Digital Rear / SPDIF3 Right "; 
                    else
                      s="Rear Right (Analog and Digital) ";
                    if(dsp_flags&KX_DSP_SWAP_REAR)
                     s+="[swapped] ";
                    break;
                case 0x400a:
                    if(!(dsp_flags&KX_FLAG_DOO))
                      s="Analog Center "; 
                    else
                      s="Analog Center (muted) ";
                    break;
                case 0x400b:
                    if(!(dsp_flags&KX_FLAG_DOO))
                      s="Analog SubWoofer ";
                    else
                      s="Analog SubWoofer (muted) ";
                    break;
                case 0x400c:
                   if(is_10k2_)
                    s="Side Left or Rear Center / I2S2 Left ";
                   else
                    found=0;
                   break;
                case 0x400d:
                   if(is_10k2_)
                    s="Side Right / I2S2 Right ";
                   else
                    found=0;
                   break;
                default:
                   found=0;
                   break;
               }
              }  // not aps?
    };
    break;
  case KX_INPUT_NAME:
    // translate KX_IN(a) -> 0x8000+a
    p=((p-KX_IN(0))/2)*4+(p&1)+0x8000;
    // fall-thru...

  case KX_PROLOG_NAME:
    {
                switch(p)
                {
                 case 0x8000:
                    if(is_aps)
                     s="A1 ";
                    else
                     s="AC97 Left ";
                    break;
                 case 0x8001:
                    if(is_aps)
                     s="A2 ";
                    else
                     s="AC97 Right ";
                    break;
                 case 0x8004:
                    if(is_aps)
                     s="D1 Left ";
                    else
                     s="CD SPDIF Left ";
                    break;
                 case 0x8005:
                    if(is_aps)
                     s="D1 Right ";
                    else
                     s="CD SPDIF Right ";
                    break; 
                 case 0x8008:
                    s="I2S / SPDIF C Left ";
                    break;
                 case 0x8009:
                    s="I2S / SPDIF C Right ";
                    break;
                 case 0x800c:
                    if(is_aps)
                     s="D2 Left ";
                    else
                     s="Coaxial / Optical Left ";
                    break;
                 case 0x800d:
                    if(is_aps)
                     s="D2 Right ";
                    else
                     s="Coaxial / Optical Right ";
                    break;
                 case 0x8010:
                    if(is_aps)
                     s="A3 ";
                    else
                    {
                     if(is_a2ex)
                      s="Line1/Mic1 Left ";
                     else
                      s="Line2/Mic2 Left ";
                    }
                    break;
                 case 0x8011:
                    if(is_aps)
                     s="A4 ";
                    else
                    {
                     if(is_a2ex)
                      s="Line1/Mic1 Right ";
                     else
                      s="Line2/Mic2 Right ";
                    }
                    break;
                 case 0x8014:
                    if(is_aps)
                     s="A5 ";
                    else
                    {
                     if(is_a2ex)
                      s="Line2 Left ";
                     else
                     {
                      if(is_10k2_)
                       s="AC97 In 2 Left ";
                      else
                       s="Unknown 2 Left ";
                     }
                    }
                    break;
                 case 0x8015:
                    if(is_aps)
                     s="A6 ";
                    else
                    {
                     if(is_a2ex)
                      s="Line2 Right ";
                     else
                     {
                      if(is_10k2_)
                       s="AC97 In 2 Right ";
                      else
                       s="Unknown 2 Right ";
                     }
                    }
                    break;
                 case 0x8018:
                    if(is_aps)
                     s="A7 ";
                    else
                    {
                     if(is_a2ex)
                      s="Line3 Left ";
                     else
                      s="Aux2 Left ";
                    }
                    break;
                 case 0x8019:
                    if(is_aps)
                     s="A8 ";
                    else
                    {
                     if(is_a2ex)
                      s="Line3 Right ";
                     else
                      s="Aux2 Right ";
                    }
                    break;
                 default:
                    found=0;
                }
    }
    break;
   case KX_HW_NAME:
    switch(p)
    {
        case KX_IN_SPDIFA:
            if(is_aps)
             s="D1 In";
            else
              s="CD SPDIF In";
            break;
        case KX_IN_SPDIFB:
            if(is_aps)
             s="D2 In";
            else
              s="Coaxial/Optical SPDIF In";
            break;
        case KX_IN_SPDIFC:
             s="ZVideo I2S In";
            break;
        case KX_IN_I2S0:
            if(is_aps)
             s="A3/A4 In";
            else
             if(is_a2ex)
              s="Line1/Mic1 In";
              else
                s="Line2/Mic2 In";
            break;
        case KX_IN_I2S1:
            if(is_aps)
             s="A5/A6 In";
            else
             if(is_a2ex)
              s="Line2 In";
              else
               if(is_10k2_)
                s="AC97/2 In";
               else
                s="Unknown 10k1 In";
            break;
        case KX_IN_I2S2:
            if(is_aps)
             s="A7/A8";
            else
             if(is_a2ex)
              s="Aux In";
              else
                s="Aux2 In";
            break;
        case KX_IN_AC97:
            if(is_aps)
             s="A1/A2 In";
            else
              s="AC97 In";
            break;
// ----
        case KX_OUT_SPDIF0:
            if(is_aps)
             s="D1 Out";
            else
             s="SPDIF Out / Digital Front";
            break;
        case KX_OUT_SPDIF1:
            if(is_aps)
             s="D2 / A3/A4 Out";
            else
             s="Digital Center+LFE Out";
            break;
        case KX_OUT_SPDIF2:
            if(is_aps)
             s="A5/A6 Out";
            else
             s="Headphones Out";
            break;
        case KX_OUT_SPDIF3:
            if(is_10k2_)
             s="Digital Rear Out";
            else
             found=0;
            break;
        case KX_OUT_I2S0:
            if(is_aps)
             s="A1/A2 Out";
            else
             s="Analog Front Out";
            break;
        case KX_OUT_I2S1:
            if(is_aps)
             found=0;
            else
             s="Analog Center+LFE";
            break;
        case KX_OUT_I2S2:
            if(is_10k2_)
             s="Side Out / Rear Center";
            else
             found=0;
            break;           
        case KX_OUT_I2S3:
            if(is_aps)
             s="A7/A8 Out";
            else
             if(is_10k2_)
              s="Analog Rear Out";
             else
              s="Rear Out";
            break;
/*      case KX_OUT_AC97_0:
            if(is_10k2)
             s="";
            else
             found=0;
            break;
        case KX_OUT_AC97_1:
            if(is_10k2)
             found=0;
            else
             if(is_51)
              s="Analog Center/LFE";
             else
              found=0;
            break;
        case KX_OUT_AC97_2:
            if(is_10k2)
             s="";
            else
             found=0;
            break;
*/
        case KX_OUT_REC:
            s="WinMM Recording";
            break;
        default:
            found=0;
    }
    break;
 }

 if(found)
  strncpy(name,(const char *)s,KX_MAX_STRING);

 return (1-found);
}

int iKX::get_p16v_volume(int chn,dword *value)
{
    routing_property r;
    r.ndx=chn;
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_P16V_VOLUME,&r,sizeof(r),&ret_b);
    if(ret || r.ndx==-1) // not found
     return -1;
        if(value)
         *value=r.routing;
        return 0;
}


int iKX::set_p16v_volume(int chn,dword value)
{
    routing_property r;
    r.ndx=chn;
    r.routing=value;
    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_SET|KX_PROP_P16V_VOLUME,&r,sizeof(r),&ret_b);
    if(ret || r.ndx==-1) // not found
     return -1;
    return 0;
}

int iKX::get_device_caps(kx_caps *caps,int *sz)
{
 int ret_b=0;

 if(!caps || !sz)
  return -2;

 int *buff=(int *)api_alloc((*sz)+sizeof(int));
 if(buff)
 {
     memset(buff,0,(*sz)+sizeof(int));
     buff[0]=*sz;

     int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_ASIO_CAPS,buff,(*sz)+(int)sizeof(int),&ret_b);
     if(ret) // error
     {
      api_free(buff);
      return -1;
     }
     *sz=*buff;
     memcpy(caps,buff+1,*sz);

     api_free(buff);
     return 0;
 }
 else
  return -3;
}

int iKX::close_passthru(void *instance)
{
    dword_property dw;
    dw.reg=0x0;
    dw.chn=0x0;
    dw.pntr_padding=0;  // need to do this to fill-in higher 64-bit part
    dw.pntr=instance;

    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_CLOSEPASSTHRU,&dw,sizeof(dw),&ret_b);
    return ret;
}

int iKX::init_passthru(void **instance,int *method)
{
    dword_property dw;
    dw.reg=0x0;
    dw.chn=0x0;
    dw.val=0x0;
    dw.pntr_padding=0;  // need to do this to fill-in higher 64-bit part
    dw.pntr=0x0;

    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_INITPASSTHRU,&dw,sizeof(dw),&ret_b);

    if(ret==0)
    {
      if(instance)
       *instance=dw.pntr;

      if(method)
       *method=(int)dw.reg;
    }

    return ret;
}

int iKX::set_passthru(void *instance,int onoff)
{
    dword_property dw;
    dw.reg=(dword)onoff;
    dw.chn=0x0;
    dw.pntr_padding=0;  // need to do this to fill-in higher 64-bit part
    dw.pntr=instance;

    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_SETPASSTHRU,&dw,sizeof(dw),&ret_b);
    return ret;
}


int iKX::upload_fpga_firmware(byte *data,int size)
{
#if defined(WIN32)
    fpga_firmware_property *request;
    
    request=(fpga_firmware_property *)api_alloc(size + sizeof(fpga_firmware_property));
    
    if(request)
    {
        request->size=size;
        memcpy(request+1,data,size);
        
        int ret_b=0;
        int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_FPGA_FIRMWARE,request,sizeof(fpga_firmware_property)+size,&ret_b);
        
        api_free(request);
        
        return ret;
    }
    else
        return -2;
#elif defined(__APPLE__)
    fpga_firmware_property *request;
    
    request=(fpga_firmware_property *)api_alloc(PAGE_SIZE);
    
    if(request)
    {
        int first=1;
        int ret=0;
        
        /*
        dword crc=0x0055aa55;
        for(int i=0;i<size;i++)
            crc=crc+data[i]*i;
        printf("=== size: %d; crc=%08x\n",size,crc);
        */
        
        while(size>0)
        {
#define MAX_FPGA_BUFFER (PAGE_SIZE-128)
            
            int sz=(size>MAX_FPGA_BUFFER)?MAX_FPGA_BUFFER:size;
            int to_send=0;
            
            if(first)
            {
                sz-=4;
                request[0].size=size;
                request[1].size=sz;
                
                memcpy(request+2,data,sz);
                
                to_send=sz+(int)sizeof(int)+(int)sizeof(int);
            }
            else
            {
                memcpy(request+1,data,sz);
                
                to_send=sz+(int)sizeof(int);
                
                request->size=sz;
            }

            data+=sz;
            size-=sz;
            
            dword prop;
            if(first)
            {
                first=0;
                prop=KX_TOPO|KX_PROP_GET|KX_PROP_FPGA_FW_START;
            }
            else
            {
                if(size>0)
                    prop=KX_TOPO|KX_PROP_GET|KX_PROP_FPGA_FW_CONT;
                else
                    prop=KX_TOPO|KX_PROP_GET|KX_PROP_FPGA_FW_END;
            }
            
            int ret_b=0;
            ret=ctrl(prop,request,to_send,&ret_b);
            if(ret)
                break;
        }
        api_free(request);
        
        return ret;
    }
    else
        return -2;
#endif
}

int iKX::fpga_read(byte reg,byte *val)
{
    dword_property dw;
    dw.reg=reg;
    dw.chn=0x0;

    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_FPGA,&dw,sizeof(dw),&ret_b);

    if(ret)
     return ret;
    
    if(val)
      *val=(byte)dw.val;
    
    return 0;
}

int iKX::fpga_write(byte reg,byte val)
{
    dword_property dw;
    dw.reg=reg;
    dw.chn=0x0;
    dw.val=val;

    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_SET|KX_PROP_FPGA,&dw,sizeof(dw),&ret_b);

    if(ret)
     return ret;
    
    return 0;
}

int iKX::fpga_link_src2dst(dword src, dword dst)
{
    dword_property dw;
    dw.reg=src;
    dw.chn=0x0;
    dw.val=dst;

    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_SET|KX_PROP_FPGALINK,&dw,sizeof(dw),&ret_b);

    if(ret)
     return ret;
    
    return 0;
}

int iKX::set_clock(int frequency)
{
    dword_property dw;
    dw.reg=0;
    dw.chn=0x0;
    dw.val=frequency;

    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_SET|KX_PROP_CLOCK,&dw,sizeof(dw),&ret_b);

    if(ret)
     return ret;
    
    return 0;

}

int iKX::get_clock(int *frequency)
{
    dword_property dw;
    dw.reg=0;
    dw.chn=0x0;
    dw.val=0;

    int ret_b=0;
    int ret=ctrl(KX_TOPO|KX_PROP_GET|KX_PROP_CLOCK,&dw,sizeof(dw),&ret_b);

    if(ret)
     return ret;

    if(frequency)
     *frequency=dw.val;
    
    return 0;

}
