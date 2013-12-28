// kX WDM Audio Driver
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


#include "common.h"
#include "interface/kx_ioctl.h"

#include "hw/ecard.h"

#include "gsif/gsif2.h"
#include "gsif/kxgsif.h"

#define prep_in(type); type *in; \
              in = (type *)&inst[1]; \
              if(req->InstanceSize-sizeof(my_prop)!=sizeof(type)) \
               { debug(DWDM,"!!! Instance too little (instance: %d) my_prop: %d, (type:%d)\n",req->InstanceSize,sizeof(my_prop),sizeof(type)); return STATUS_INVALID_PARAMETER; }

#define prep_out(type); type *out; \
              out = (type *)req->Value; \
              if(req->ValueSize!=sizeof(type)) \
              { debug(DWDM,"!!! Value too little (%d)\n",req->ValueSize); return STATUS_INVALID_PARAMETER; }
#define prep_ds_in(type); type *in; \
              in = (type *)&inst[1]; \
              if(req->InstanceSize-sizeof(my_prop)<sizeof(type)) \
               { debug(DWDM,"!!! Instance too little (instance: %d) my_prop: (%d) type: %d\n",req->InstanceSize,sizeof(my_prop),sizeof(type)); return STATUS_INVALID_PARAMETER; }

typedef struct
{
    kx_property_instance inst;
}my_prop;
static NTSTATUS actual_process(CAdapterCommon *adapter,kx_hw *hw,CMiniportWaveStream *that2,my_prop *inst,PPCPROPERTY_REQUEST req);

#pragma code_seg("PAGE")

NTSTATUS process_property(CAdapterCommon *adapter,kx_hw *hw,PPCPROPERTY_REQUEST req,dword where)
{
 PAGED_CODE();

 CMiniportWaveStream *that2=NULL;
 my_prop *inst=NULL;

 if(where!=WAVE_MAGIC && where!=TOPOLOGY_MAGIC)
 {
  debug(DWDM,"!!! Unknown magic (%x)\n",where);
  return STATUS_INVALID_PARAMETER;
 }

 if(req->PropertyItem->Flags&(~(PCPROPERTY_ITEM_FLAG_SET|
                              PCPROPERTY_ITEM_FLAG_GET)))
 {
   debug(DWDM,"!!! Unknown verb %d\n",req->PropertyItem->Flags);
   return STATUS_INVALID_PARAMETER;
 }

 if(where==WAVE_MAGIC)
 {
      if(req->InstanceSize<sizeof(kx_property_instance)+16)
      {
       debug(DWDM,"!!! no instance data\n");
       return STATUS_INVALID_PARAMETER;
      }

      inst=(my_prop *)((uintptr_t)req->Instance+16);
      req->InstanceSize-=16;

      if(inst->inst.magic!=PROPERTY_MAGIC)
      {
       debug(DWDM,"!!! incorrect magic (%x)\n",inst->inst.magic);
       ULONG *data=(ULONG *)req->Instance;
       debug(DWDM,"data: %08x %08x %08x %08x %08x %08x %08x %08x\n",
        data[0],
        data[1],
        data[2],
        data[3],
        data[4],
        data[5],
        data[6],
        data[7]);
                
       return STATUS_INVALID_PARAMETER;
      }

      if(req->MinorTarget==NULL)
      {
       debug(DWDM,"!!! No minor target in ::private_prop for wave\n");
       return STATUS_INVALID_PARAMETER;
      }

      PUNKNOWN pu=req->MinorTarget;
      that2=(CMiniportWaveStream *)PMINIPORTWAVECYCLICSTREAM(pu);
      if(that2)
       if((that2->magic!=WAVEINSTREAM_MAGIC) && (that2->magic!=WAVEOUTSTREAM_MAGIC))
          {
           debug(DWDM,"!!! Bad wavestream magic :%x\n",that2->magic);
           return STATUS_INVALID_PARAMETER; // 25/06/03
          }

      if(!(inst->inst.prop&(KX_PROP_GET|KX_PROP_SET)))
      {
       debug(DWDM,"!!! incorrect set/get operation\n");
       return STATUS_INVALID_PARAMETER;
      }
 }
 else // where != WAVE_MAGIC
 {
      if(req->InstanceSize<sizeof(kx_property_instance))
      {
       debug(DWDM,"!!! no instance data\n");
       return STATUS_INVALID_PARAMETER;
      }

      inst=(my_prop *)req->Instance;

      if(inst->inst.magic!=PROPERTY_MAGIC)
      {
       debug(DWDM,"!!! incorrect instance data (%x)\n",inst->inst.magic);
       for(dword i=0;i<req->InstanceSize;i++)
        debug(DNONE,"%p ",((uintptr_t)req->Instance+i));
       debug(DNONE,"\n");
       return STATUS_INVALID_PARAMETER;
      }

      if(!(inst->inst.prop&(KX_PROP_GET|KX_PROP_SET)))
      {
       debug(DWDM,"!!! incorrect set/get operation\n");
       return STATUS_INVALID_PARAMETER;
      }
 }
 return actual_process(adapter,hw,that2,inst,req);
}

#pragma code_seg("PAGE")
static NTSTATUS actual_process(CAdapterCommon *adapter,kx_hw *hw,CMiniportWaveStream *that2,my_prop *inst,PPCPROPERTY_REQUEST req)
{
 PAGED_CODE();

 switch(inst->inst.prop)
 {
  case KX_PROP_VOICE_ROUTING+KX_PROP_SET:
    {
     if(that2)
     {
        CMiniportWaveOutStream *that3;
        that3=(CMiniportWaveOutStream *)that2;

                prep_ds_in(voice_routing_req);              

            kx_wave_set_parameter(that2->hw,
              that3->channels[0],0,KX_VOICE_ROUTINGS,in->routing);
            kx_wave_set_parameter(that2->hw,
              that3->channels[0],0,KX_VOICE_XROUTINGS,in->xrouting);

            if(!(that2->hw->voicetable[that3->channels[0]].usage&VOICE_FLAGS_STEREO))
              kx_wave_set_parameter(that2->hw,
                that3->channels[0],1,KX_VOICE_PAN,KX_MIN_VOLUME);

         } else { debug(DWDM,"!!! that2=0 in actual_process\n"); return STATUS_INVALID_PARAMETER; }
    }
    break;
  case KX_PROP_VOICE_ROUTING+KX_PROP_GET:
    {
     if(that2)
     {
                prep_ds_in(voice_routing_req);              
                prep_out(voice_routing_req);

        CMiniportWaveOutStream *that3;
        that3=(CMiniportWaveOutStream *)that2;

            kx_wave_get_parameter(that2->hw,
              that3->channels[0],0,KX_VOICE_ROUTINGS,&out->routing);
            kx_wave_get_parameter(that2->hw,
              that3->channels[0],0,KX_VOICE_XROUTINGS,&out->xrouting);

         } else { debug(DWDM,"!!! that2=0 in actual_process\n"); return STATUS_INVALID_PARAMETER; }
    }
    break;
  case KX_PROP_ASIO+KX_PROP_GET:
    {
     if(that2)
     {
            prep_ds_in(asio_property);
            prep_out(asio_property);

            int i;
            if(in->magic!=KX_ASIO_MAGIC)
            {
                debug(DWDM,"!!! asio magic invalid\n");
                return STATUS_INVALID_PARAMETER;
            }

             CMiniportWaveOutStream *that3=(CMiniportWaveOutStream *)that2;
             if(that3->magic!=WAVEOUTSTREAM_MAGIC)
             {
                debug(DWDM,"!!! asio wavestream magic invalid!\n");
                return STATUS_INVALID_PARAMETER;
             }

             if((that3->is_asio==0)&&(in->cmd!=KX_ASIO_INIT))
             {
                debug(DWDM,"!!! asio not initialized in kernel\n");
                return STATUS_INVALID_PARAMETER;
             }

             if(in->cmd==KX_ASIO_WAIT)
             {
                   out->rate=that3->asio_wait(in->rate,in->ptr);

                   return STATUS_SUCCESS;
             }
             else
             if(in->cmd==KX_ASIO_TIMER)
             {
                   if(in->rate==0)
                   {
                    // reset
                    out->chn=ExSetTimerResolution(0,FALSE);
                    out->rate=0;
                    debug(DWDM,"kx: asio: timer restored to %d\n",out->chn);
                    return STATUS_SUCCESS;
                   }
                   else
                    if(in->rate==1) // set
                    {
                        out->chn=ExSetTimerResolution(in->chn,TRUE);
                        out->rate=0;
                        debug(DWDM,"kx: asio: timer set to %d [requested: %d]\n",out->chn,in->chn);
                        return STATUS_SUCCESS;
                    }
                    else
                    {
                     debug(DWDM,"!! asio invalid timer param [%d]\n",in->rate);
                     return STATUS_INVALID_PARAMETER;
                    }
             }
             else
             if(in->cmd==KX_ASIO_INIT)
             {
                int ret=(that3->asio_init(in->chn))?STATUS_INVALID_PARAMETER:STATUS_SUCCESS;

                if(ret==0 && that3->asio_notification_user)
                 out->notification=that3->asio_notification_user;
                else
                 out->notification=0;

                if(ret==0 && in->user_event)
                {
                 if(!NT_SUCCESS(
                    ObReferenceObjectByHandle(in->user_event,SYNCHRONIZE,*ExEventObjectType,
                                              KernelMode,(PVOID *)&hw->asio_notification_krnl.kevent,NULL)))
                    debug(DWDM,"!! asio: failed to reference user-level event\n");
                }

                return ret;
             }
             else
             if(in->cmd==KX_ASIO_CLOSE)
             {
               return (that3->asio_close())?STATUS_INVALID_PARAMETER:STATUS_SUCCESS;
             }
             else
             if(in->cmd==KX_ASIO_GET_POSITION)
             {
                out->position=that3->asio_get_position(0);
             }
             else
             if(in->cmd==KX_ASIO_ALLOC_OUTPUT)
             {
                out->addr=in->addr;
                out->cmd=that3->asio_alloc_output(in->chn,in->size,&out->addr,in->rate,in->bps);
                if(out->cmd<0)
                 return STATUS_INVALID_PARAMETER;
             }
             else
             if(in->cmd==KX_ASIO_ALLOC_INPUT)
             {
                out->addr=in->addr;
                out->cmd=that3->asio_alloc_input(in->chn,in->size,&out->addr,in->rate,in->bps);
                if(out->cmd<0)
                 return STATUS_INVALID_PARAMETER;
             }
             else
             if(in->cmd==KX_ASIO_FREE_OUTPUT) // outputs only
             {
                out->cmd=that3->asio_free_output(in->chn);
                if(out->cmd<0)
                 return STATUS_INVALID_PARAMETER;
             }
             else
             if(in->cmd==KX_ASIO_FREE_INPUT)
             {
                out->cmd=that3->asio_free_input(in->chn);
                if(out->cmd<0)
                 return STATUS_INVALID_PARAMETER;
             }
             else
             if(in->cmd==KX_ASIO_START)
             {
              if(that3->asio_start())
               debug(DWDM,"!! note: nothing to start/stop [property,325]\n");
             }
             else
              if(in->cmd==KX_ASIO_STOP)
              {
               if(that3->asio_stop())
                debug(DWDM,"!! note: nothing to start/stop [property,325]\n");
              }
              else
             {
                 debug(DWDM,"!!! Unknown ASIO command\n");
                 return STATUS_INVALID_PARAMETER;
             }
          } 
           else  // no that2
          {
             debug(DWDM,"!!! no that2 (ASIO)\n");
             return STATUS_INVALID_PARAMETER;
          }
    }
    break;
  case KX_PROP_AC97+KX_PROP_GET:
    {
    prep_in(ac97_property);
    prep_out(ac97_property);
    out->val=kx_ac97read(hw,in->reg);
    }
    break;
  case KX_PROP_AC97+KX_PROP_SET:
    {
    prep_in(ac97_property);
    if(in->reg==0) // rest
    {
     kx_ac97reset(hw);
    }
    else
    {
      kx_ac97write(hw,in->reg,in->val);
    }
    }
    break;
  case KX_PROP_GET_STRING+KX_PROP_GET:
    {
    prep_in(get_property);
    prep_out(get_property);
    if(kx_getstring(hw,in->what,&out->str[0])) // failed
    {
        out->str[0]=0;
        out->what=-1;
    }
    }
    break;
  case KX_PROP_GET_DWORD+KX_PROP_GET:
    {
    prep_in(get_property);
    prep_out(get_property);
    if(kx_getdword(hw,in->what,&out->val)) // failed
    {
        out->what=-1; // failed
        return STATUS_INVALID_PARAMETER;
    }
    }
    break;
  case KX_PROP_INITPASSTHRU+KX_PROP_GET:
    {
     prep_in(dword_property);
     prep_out(dword_property);

     int method=0;
     void *instance_=(void *)((uintptr_t)hw&0xffffffff);    // need to make it 32-bit friendly, since dword_property.pntr is used

     if(hw->ac3_pt_state.instance)
     {
        debug(DWDM,"initpassthru: in-use!!\n");
        return STATUS_INVALID_PARAMETER;
     }

     if(kx_init_passthru(hw,instance_,&method)==0)
     {
      out->reg=(dword)method;
      out->pntr=instance_;
      out->chn=0x0;
     }
     else
      return STATUS_INVALID_PARAMETER;
    }
    break;
  case KX_PROP_CLOSEPASSTHRU+KX_PROP_GET:
    {
     prep_in(dword_property);
     prep_out(dword_property);

     if(kx_close_passthru(hw,in->pntr)!=0)
      return STATUS_INVALID_PARAMETER;
    }
    break;
  case KX_PROP_SETPASSTHRU+KX_PROP_GET:
    {
     prep_in(dword_property);
     prep_out(dword_property);

     if(kx_set_passthru(hw,in->pntr,(int)in->reg)!=0)
      return STATUS_INVALID_PARAMETER;
    }
    break;

  case KX_PROP_HW_PARAM+KX_PROP_GET:
    {
            prep_in(dword_property);
            prep_out(dword_property);
            if(in->reg==KX_HW_KX3D)
            {
             out->reg=0;
             if(adapter && adapter->Wave[0])
              out->val=adapter->Wave[0]->kx3d_compat;
            }
            else
             if(in->reg==KX_HW_8PS)
             {
              out->reg=0;
              if(adapter && adapter->Wave[0])
               out->val=adapter->Wave[0]->kx3d_sp8ps;
             }
             else
              out->reg=kx_get_hw_parameter(hw,in->reg,&out->val);
    }
    break;
  case KX_PROP_HW_PARAM+KX_PROP_SET:
    {
            prep_in(dword_property);
            if(in->reg==KX_HW_KX3D)
            {
             if(adapter && adapter->Wave[0])
              adapter->Wave[0]->kx3d_compat=in->val;
            }
            else
            {
             if(in->reg==KX_HW_8PS)
             {
              if(adapter && adapter->Wave[0])
               adapter->Wave[0]->kx3d_sp8ps=in->val;
             }
             else
              if(kx_set_hw_parameter(hw,in->reg,in->val))
                return STATUS_INVALID_PARAMETER;
            }
    }
    break;
  case KX_PROP_FN0+KX_PROP_GET:
    {
    prep_in(dword_property);
    prep_out(dword_property);
    out->val=kx_readfn0(hw,in->reg);
    }
    break;
  case KX_PROP_FN0+KX_PROP_SET:
    {
    prep_in(dword_property);
    kx_writefn0(hw,in->reg,in->val);
    }
    break;
  case KX_PROP_FPGA+KX_PROP_GET:
    {
    prep_in(dword_property);
    prep_out(dword_property);
    out->val=kx_readfpga(hw,in->reg);
    }
    break;
  case KX_PROP_FPGA+KX_PROP_SET:
    {
    prep_in(dword_property);
    kx_writefpga(hw,in->reg,in->val);
    }
    break;
  case KX_PROP_CLOCK+KX_PROP_GET:
    {
    prep_in(dword_property);
    prep_out(dword_property);
    out->val=hw->card_frequency;
    }
    break;
  case KX_PROP_CLOCK+KX_PROP_SET:
    {
    prep_in(dword_property);
    kx_set_clock(hw,in->val);
    }
    break;
  case KX_PROP_FPGALINK+KX_PROP_SET:
    {
    prep_in(dword_property);
    kx_fpga_link_src2dst(hw,in->reg,in->val);
    }
    break;
  case KX_PROP_FPGA_FIRMWARE+KX_PROP_GET:
    {
      if(req->InstanceSize-sizeof(my_prop)>4)
      {
          fpga_firmware_property *in;
          in = (fpga_firmware_property *)&inst[1];

          if(req->InstanceSize-sizeof(my_prop)==in->size+4)
          {
            if(kx_upload_fpga_firmware(hw,(byte *)((&in->size)+1),in->size))
             return STATUS_INVALID_PARAMETER;
          }
          else
          {
            debug(DWDM,"!! Firmware instance too small [2]\n");
            return STATUS_INVALID_PARAMETER;
          }
      }
      else
      {
       debug(DWDM,"!! Firmware instance too small\n");
       return STATUS_INVALID_PARAMETER;
      }
    }
    break;
  case KX_PROP_GPIO_GET+KX_PROP_GET:
    {
    prep_in(dword_property);
    prep_out(dword_property);
    out->val=kx_get_gp_inputs(hw);
    }
    break;
  case KX_PROP_GPIO_SET+KX_PROP_GET:
    {
    prep_in(dword_property);
    kx_set_gp_outputs(hw,(byte)in->reg);
    }
    break;
  case KX_PROP_P16V+KX_PROP_GET:
    {
    prep_in(dword_property);
    prep_out(dword_property);
    out->val=kx_readp16v(hw,in->reg,in->chn);
    }
    break;
  case KX_PROP_PTR+KX_PROP_GET:
    {
    prep_in(dword_property);
    prep_out(dword_property);
    out->val=kx_readptr(hw,in->reg,in->chn);
    }
    break;
  case KX_PROP_P16V+KX_PROP_SET:
    {
    prep_in(dword_property);
    kx_writep16v(hw,in->reg,in->chn,in->val);
    }
    break;
  case KX_PROP_PTR+KX_PROP_SET:
    {
    prep_in(dword_property);
    kx_writeptr(hw,in->reg,in->chn,in->val);
    }
    break;
  case KX_PROP_SPDIF_I2S_STATE+KX_PROP_GET:
    {
    prep_out(kx_spdif_i2s_status);
    kx_get_spdif_i2s_status(hw,out);
    }
    break;
  case KX_PROP_ROUTING+KX_PROP_SET:
    {
    prep_in(routing_property);
    if(kx_set_routing(hw,in->ndx,in->routing,in->xrouting)) // failed
    {
        return STATUS_INVALID_PARAMETER;
    }
    }
    break;
  case KX_PROP_ROUTING+KX_PROP_GET:
    {
    prep_in(routing_property);
    prep_out(routing_property);
    if(kx_get_routing(hw,in->ndx,&out->routing,&out->xrouting))
    {
        out->ndx=-1; // failed flag
    }
    }
    break;
  case KX_PROP_AMOUNT+KX_PROP_SET:
    {
    prep_in(routing_property);
    if(kx_set_fx_amount(hw,in->ndx,(byte)in->routing)) // failed
    {
        return STATUS_INVALID_PARAMETER;
    }
    }
    break;
  case KX_PROP_AMOUNT+KX_PROP_GET:
    {
    prep_in(routing_property);
    prep_out(routing_property);
    if(kx_get_fx_amount(hw,in->ndx,(byte *)&out->routing))
    {
        out->ndx=-1; // failed flag
    }
    }
    break;
  case KX_PROP_P16V_VOLUME+KX_PROP_GET:
    {
    prep_in(routing_property);
    prep_out(routing_property);
    if(kx_get_p16v_volume(hw,in->ndx,(dword *)&out->routing))
      out->ndx=-1;
    }
    break;
  case KX_PROP_P16V_VOLUME+KX_PROP_SET:
    {
    prep_in(routing_property);
    if(kx_set_p16v_volume(hw,in->ndx,in->routing))
        return STATUS_INVALID_PARAMETER;
    }
    break;
  case KX_PROP_SET_BUFFERS+KX_PROP_SET:
    {
    prep_in(get_property);
    switch(in->what)
    {
        case KX_TANKMEM_BUFFER:
            {
             int ret=kx_set_tram_size(hw,in->val);
             if(ret)
              return STATUS_INVALID_PARAMETER;
            }
            break;
        case KX_PLAYBACK_BUFFER:
            hw->cb.pb_buffer=in->val/8/3;
            hw->cb.pb_buffer*=24;
            if(hw->cb.pb_buffer<240)
             hw->cb.pb_buffer=240;
            if(hw->cb.pb_buffer>32760)
             hw->cb.pb_buffer=32760;
            break;
                case KX_RECORD_BUFFER:
                    hw->cb.rec_buffer=in->val; break;
                case KX_AC3_BUFFERS:
                    hw->cb.ac3_buffers=in->val; break;
                case KX_GSIF_BUFFER:
                    switch(in->val)
                    {
                     case 16:
                     case 32:
                     case 64:
                     case 128:
                     case 256:
                     case 512:
                     case 1024:
                     case 2048:
                     case 4096:
                            if(adapter)
                             if(adapter->gsif_interface)
                             {
                              adapter->gsif_interface->gsif_latency=in->val;
                              break;
                             }

                            // fall thru:
                         default:
                            debug(DERR,"Incorrect gsif size / interface pointer [%d]\n",in->val);
                            return STATUS_INVALID_PARAMETER;
                        }
                    break;
                default:
                    return STATUS_INVALID_PARAMETER;
    }
    }
    break;
  case KX_PROP_GET_BUFFERS+KX_PROP_GET:
    {
    prep_in(get_property);
    prep_out(get_property);
    switch(in->what)
    {
        case KX_TANKMEM_BUFFER:
            out->val=hw->cb.tram_size; break;
        case KX_PLAYBACK_BUFFER:
            out->val=hw->cb.pb_buffer; break;
                case KX_RECORD_BUFFER:
                    out->val=hw->cb.rec_buffer; break;
                case KX_AC3_BUFFERS:
                    out->val=hw->cb.ac3_buffers; break;
                case KX_GSIF_BUFFER:
                    if(adapter)
                     if(adapter->gsif_interface)
                     {
                      out->val=adapter->gsif_interface->gsif_latency;
                      break;
                     }

                        // fall thru:
                        debug(DWDM,"Invalid GSIF pointer\n");
                default:
                    out->what=-1; break;
    }
    }
    break;
  case KX_PROP_DSP_REGISTER_NAME+KX_PROP_GET:
    {
    prep_in(dsp_register_property);
    prep_out(dsp_register_property);
    if(kx_get_dsp_register(hw,in->pgm,in->name,&out->val)) // error
    {
        out->id=0xffff;
    }
    }
    break;
  case KX_PROP_DSP_REGISTER_ID+KX_PROP_GET:
    {
    prep_in(dsp_register_property);
    prep_out(dsp_register_property);
    if(kx_get_dsp_register(hw,in->pgm,in->id,&out->val)) // error
    {
        out->id=0xffff;
    }
    }
    break;
  case KX_PROP_DSP_REGISTER_NAME+KX_PROP_SET:
    {
    prep_in(dsp_register_property);
    if(kx_set_dsp_register(hw,in->pgm,in->name,in->val)) // error
    {
        return STATUS_INVALID_PARAMETER;
    }
    }
    break;
  case KX_PROP_DSP_REGISTER_ID+KX_PROP_SET:
    {
    prep_in(dsp_register_property);
    if(kx_set_dsp_register(hw,in->pgm,in->id,in->val)) // error
    {
        return STATUS_INVALID_PARAMETER;
    }
    }
    break;
  case KX_PROP_TRAM_ADDR_NAME+KX_PROP_GET:
    {
    prep_in(dsp_register_property);
    prep_out(dsp_register_property);
    if(kx_get_tram_addr(hw,in->pgm,in->name,&out->val)) // error
    {
        out->id=0xffff;
    }
    }
    break;
  case KX_PROP_TRAM_ADDR_ID+KX_PROP_GET:
    {
    prep_in(dsp_register_property);
    prep_out(dsp_register_property);
    if(kx_get_tram_addr(hw,in->pgm,in->id,&out->val)) // error
    {
        out->id=0xffff;
    }
    }
    break;
  case KX_PROP_TRAM_ADDR_NAME+KX_PROP_SET:
    {
    prep_in(dsp_register_property);
    if(kx_set_tram_addr(hw,in->pgm,in->name,in->val)) // error
    {
        return STATUS_INVALID_PARAMETER;
    }
    }
    break;
  case KX_PROP_TRAM_ADDR_ID+KX_PROP_SET:
    {
    prep_in(dsp_register_property);
    if(kx_set_tram_addr(hw,in->pgm,in->id,in->val)) // error
    {
        return STATUS_INVALID_PARAMETER;
    }
    }
    break;
  case KX_PROP_TRAM_FLAG_NAME+KX_PROP_GET:
    {
    prep_in(dsp_register_property);
    prep_out(dsp_register_property);
    if(kx_get_tram_flag(hw,in->pgm,in->name,&out->val)) // error
    {
        out->id=0xffff;
    }
    }
    break;
  case KX_PROP_INSTRUCTION_WRITE+KX_PROP_GET:
    {
    prep_in(dsp_instruction_property);
    prep_out(dsp_instruction_property);
    if(kx_write_instruction(hw,in->pgm,in->offset,in->op,in->z,in->w,in->x,in->y,in->valid))
    {
     out->pgm=0xffff;
    }
    }
    break;
  case KX_PROP_INSTRUCTION_READ+KX_PROP_GET:
    {
    prep_in(dsp_instruction_property);
    prep_out(dsp_instruction_property);
    if(kx_read_instruction(hw,in->pgm,in->offset,&out->op,&out->z,&out->w,&out->x,&out->y))
    {
     out->pgm=0xffff;
    } else out->pgm=0;
    }
    break;
  case KX_PROP_TRAM_FLAG_ID+KX_PROP_GET:
    {
    prep_in(dsp_register_property);
    prep_out(dsp_register_property);
    if(kx_get_tram_flag(hw,in->pgm,in->id,&out->val)) // error
    {
        out->id=0xffff;
    }
    }
    break;
  case KX_PROP_TRAM_FLAG_NAME+KX_PROP_SET:
    {
    prep_in(dsp_register_property);
    if(kx_set_tram_flag(hw,in->pgm,in->name,in->val)) // error
    {
        return STATUS_INVALID_PARAMETER;
    }
    }
    break;
  case KX_PROP_TRAM_FLAG_ID+KX_PROP_SET:
    {
    prep_in(dsp_register_property);
    if(kx_set_tram_flag(hw,in->pgm,in->id,in->val)) // error
    {
        return STATUS_INVALID_PARAMETER;
    }
    }
    break;
  case KX_PROP_RESET_MICROCODE+KX_PROP_GET:
    {
    prep_in(dword_property);
    prep_out(dword_property);
    if(in->reg==1)
    {
     debug(DWDM,"Microcode reset request\n");
     kx_dsp_reset(hw);
     out->reg=0;
    } 
    else { debug(DWDM,"!!! Bad parameter for reset\n"); out->reg=-1; }
    }
    break;
  case KX_PROP_RESET_DB+KX_PROP_GET:
    {
        dword detect_db(kx_hw *hw);

        if(hw)
         detect_db(hw);
    }
    break;
  case KX_PROP_RESET_VOICES+KX_PROP_GET:
    {
        if(hw)
         kx_reset_voices(hw);
    }
    break;
  case KX_PROP_RESET_SETTINGS+KX_PROP_GET: // save settings
    {
        adapter->ResetSettings(); // fx amounts + routing + buffers + tramsize
        if(hw)
        {
            detect_db(hw);

                kx_set_hw_parameter(hw,KX_HW_DOO,0);
                        if(hw->is_10k2)
                         kx_set_hw_parameter(hw,KX_HW_SPDIF_FREQ,1); // 48000
                        if(hw->is_aps)
                        {
                         kx_set_hw_parameter(hw,KX_HW_ECARD_ROUTING,KX_EC_SOURCE_ECARD|(KX_EC_SOURCE_EDRIVE<<16));
                         kx_set_hw_parameter(hw,KX_HW_ECARD_ADC_GAIN,EC_DEFAULT_ADC_GAIN);
                        }
                        kx_set_hw_parameter(hw,KX_HW_SPDIF_BYPASS,0);
                        kx_set_hw_parameter(hw,KX_HW_SWAP_FRONT_REAR,hw->have_ac97?1:0);
                        kx_set_hw_parameter(hw,KX_HW_ROUTE_PH_TO_CSW,0);

                        kx_set_hw_parameter(hw,KX_HW_DRUM_CHANNEL,10);

                        if(hw->is_10k2)
                        {
                         kx_set_hw_parameter(hw,KX_HW_SPDIF_DECODE,0);
                         kx_set_hw_parameter(hw,KX_HW_SPDIF_RECORDING,0);
                         kx_set_hw_parameter(hw,KX_HW_AC3_PASSTHROUGH,0);
                         kx_set_hw_parameter(hw,KX_HW_K2_AC97,0);

                         if(hw->is_zsnb)
                          kx_set_hw_parameter(hw,KX_HW_A2ZSNB_SOURCE,KX_ZSNB_MICIN);
                        }
                        if(hw->is_a2)
                        {
                         kx_set_hw_parameter(hw,KX_HW_P16V_PB_ROUTING,P16V_ROUTE_SPDIF|P16V_ROUTE_P_SPDIF|P16V_ROUTE_I2S|P16V_ROUTE_P_I2S);
                         // assume '0/1' as def routing

                         kx_set_hw_parameter(hw,KX_HW_P16V_REC_ROUTING,P16V_REC_I2S);

                         for(int j=0;j<8;j++)
                          hw->p16v_volumes[j]=0;
                        }
                        kx_set_hw_parameter(hw,KX_HW_SYNTH_COMPATIBILITY,KX_SYNTH_DEFAULTS);
                        kx_set_hw_parameter(hw,KX_HW_COMPAT,0);

                        if(adapter && adapter->Wave[0])
                        {
                         adapter->Wave[0]->kx3d_compat=0;
                         adapter->Wave[0]->kx3d_sp8ps=0;
                        }
                }
    }
    break;
  case KX_PROP_DSP_ASSIGN+KX_PROP_SET:
    {
      prep_in(kx_assignment_info);
      if(in->level<MAX_MIXER_CONTROLS && in->level>=0)
      {
        strncpy(hw->cb.mixer_controls[in->level].pgm_name,in->pgm,KX_MAX_STRING);
        strncpy(hw->cb.mixer_controls[in->level].reg_left,in->reg_left,KX_MAX_STRING);
        strncpy(hw->cb.mixer_controls[in->level].reg_right,in->reg_right,KX_MAX_STRING);

        hw->cb.mixer_controls[in->level].max_vol=in->max_vol;
      }
      else
        return STATUS_INVALID_PARAMETER;
    }
    break;
  case KX_PROP_DSP_ASSIGN+KX_PROP_GET:
    {
    prep_out(kx_assignment_info);
    prep_in(kx_assignment_info);
    if(in->level<MAX_MIXER_CONTROLS && in->level>=0)
    {
        strncpy(out->pgm,hw->cb.mixer_controls[in->level].pgm_name,KX_MAX_STRING);
        strncpy(out->reg_left,hw->cb.mixer_controls[in->level].reg_left,KX_MAX_STRING);
        strncpy(out->reg_right,hw->cb.mixer_controls[in->level].reg_right,KX_MAX_STRING);

        out->max_vol=hw->cb.mixer_controls[in->level].max_vol;
    }
    else
            return STATUS_INVALID_PARAMETER;
    }
    break;
  case KX_PROP_TEXT2ID+KX_PROP_GET:
    {
    prep_out(kx_text2id);
    prep_in(kx_text2id);
    if(kx_translate_text2id(hw,in))
     return STATUS_INVALID_PARAMETER;
        memcpy(out,in,sizeof(kx_text2id));
    }
    break;
  case KX_PROP_MICROCODE+KX_PROP_GET: // lots of funcs
    {
    prep_in(microcode_property);
    prep_out(microcode_property);
    switch(in->cmd)
    {
        case KX_PROP_MICROCODE_UNTRANSLATE:
            out->cmd=kx_untranslate_microcode(hw,in->pgm);
            break;
        case KX_PROP_MICROCODE_ENABLE:
            out->cmd=kx_enable_microcode(hw,in->pgm);
            break;
        case KX_PROP_MICROCODE_DISABLE:
            out->cmd=kx_disable_microcode(hw,in->pgm);
            break;
        case KX_PROP_MICROCODE_TRANSLATE:
            out->cmd=kx_translate_microcode(hw,in->pgm,in->p1,in->p2);
            break;
        case KX_PROP_MICROCODE_UNLOAD:
            out->cmd=kx_unload_microcode(hw,in->pgm);
            break;
        case KX_PROP_MICROCODE_DSP_GO:
            kx_dsp_go(hw);
            break;
        case KX_PROP_MICROCODE_DSP_STOP:
            kx_dsp_stop(hw);
            break;
        case KX_PROP_MICROCODE_DSP_CLEAR:
            kx_dsp_clear(hw);
            break;
        case KX_PROP_MICROCODE_BYPASS_ON:
            out->cmd=kx_set_microcode_bypass(hw,in->pgm,1);
            break;
        case KX_PROP_MICROCODE_BYPASS_OFF:
            out->cmd=kx_set_microcode_bypass(hw,in->pgm,0);
            break;
        case KX_PROP_MICROCODE_SET_FLAG:
            out->cmd=kx_set_microcode_flag(hw,in->pgm,in->p1);
            break;
        case KX_PROP_MICROCODE_GET_FLAG:
            out->cmd=kx_get_microcode_flag(hw,in->pgm,(dword *)&out->p1);
            break;
        default:
            debug(DWDM,"!!! Bad cmd in prop::microcode()\n");
            return STATUS_INVALID_PARAMETER;
    }
    }
    break;
  case KX_PROP_MICROCODE_CONNECT_NAME+KX_PROP_GET:
    {
    prep_in(microcode_connect_property);
    prep_out(microcode_connect_property);
    out->op=kx_connect_microcode(hw,in->pgm1,in->src_c,in->pgm2,in->dst_c);
    }
    break;
  case KX_PROP_MICROCODE_CONNECT_ID+KX_PROP_GET:
    {
    prep_in(microcode_connect_property);
    prep_out(microcode_connect_property);
    out->op=kx_connect_microcode(hw,in->pgm1,in->src_w,in->pgm2,in->dst_w);
    }
    break;
  case KX_PROP_MICROCODE_DISCONNECT_NAME+KX_PROP_GET:
    {
    prep_in(microcode_connect_property);
    prep_out(microcode_connect_property);
    out->op=kx_disconnect_microcode(hw,in->pgm1,in->src_c);
    }
    break;
  case KX_PROP_MICROCODE_DISCONNECT_ID+KX_PROP_GET:
    {
    prep_in(microcode_connect_property);
    prep_out(microcode_connect_property);
    out->op=kx_disconnect_microcode(hw,in->pgm1,in->src_w);
    }
    break;
  case KX_PROP_MICROCODE_ENUM_ID+KX_PROP_GET:
    {
    prep_in(microcode_enum_property);
    prep_out(microcode_enum_property);

    if(kx_enum_microcode(hw,in->pgm,&out->m))
      return STATUS_INVALID_PARAMETER;
    }
    break;
  case KX_PROP_MICROCODE_ENUM_NAME+KX_PROP_GET:
    {
    prep_in(microcode_enum_property);
    prep_out(microcode_enum_property);

    if(kx_enum_microcode(hw,in->name,&out->m))
      return STATUS_INVALID_PARAMETER;
    }
    break;
  case KX_PROP_MICROCODE_SET_NAME+KX_PROP_GET:
    {
    prep_in(microcode_enum_property);
    prep_out(microcode_enum_property);
    if(kx_set_microcode_name(hw,in->pgm,in->name,in->m.flag))
      return STATUS_INVALID_PARAMETER;
    }
    break;
  case KX_PROP_LOAD_MICROCODE+KX_PROP_GET:
    {
        char *m=(char *)&inst[1];
        if(m)
        {
           if(req->InstanceSize<(4+sizeof(dsp_microcode)+4+4+4))
           {
            debug(DWDM,"!!! Instance size invalid (load_microcode());\n");
            return STATUS_INVALID_PARAMETER;
           }
           if( (*(dword *)m!=LOAD_MICROCODE_MAGIC) ||
               (*(dword *)(m+4+sizeof(dsp_microcode))!=LOAD_MICROCODE_MAGIC))
           {
            debug(DWDM,"!!! Bad magic 2 for load_microcode()\n");
            return STATUS_INVALID_PARAMETER;
           }
           dsp_microcode *tmp_m=(dsp_microcode *)(m+4);
                   dsp_register_info *info=(dsp_register_info *)(m+4+sizeof(dsp_microcode)+4);
           dsp_code *code=(dsp_code *)(m+4+sizeof(dsp_microcode)+4+tmp_m->info_size);
           int *force_pgm_id=(int *)(m+4+sizeof(dsp_microcode)+4+tmp_m->info_size+tmp_m->code_size);
           if(*(dword *)(m+4+sizeof(dsp_microcode)+4+tmp_m->info_size+tmp_m->code_size+4)!=LOAD_MICROCODE_MAGIC)
           {
            debug(DWDM,"!!! Bad magic 3 for load_micrcode();\n");
            return STATUS_INVALID_PARAMETER;
           }

           if(req->InstanceSize-sizeof(my_prop)!=(4+sizeof(dsp_microcode)+4+tmp_m->info_size+tmp_m->code_size+4+4))
           {
            debug(DWDM,"!!! Bad instance size II load_microcode(): %d vs %d\n",
             req->InstanceSize,(4+sizeof(dsp_microcode)+4+tmp_m->info_size+tmp_m->code_size+4+4));
            return STATUS_INVALID_PARAMETER;
           }

           int pgm=kx_load_microcode(hw,tmp_m->name,code,tmp_m->code_size,
              info,tmp_m->info_size,tmp_m->itramsize,tmp_m->xtramsize,
              tmp_m->copyright,
              tmp_m->engine,
              tmp_m->created,
              tmp_m->comment,
              tmp_m->guid,*force_pgm_id);
           if(pgm==0)
            debug(DWDM,"!!! -- load_microcode() failed\n");

           dword *out=(dword *)req->Value;
           if(out)
           {
             out[0]=(dword)pgm;
           }
            else return STATUS_INVALID_PARAMETER;
        } else return STATUS_INVALID_PARAMETER;
    }
    break;
  case KX_PROP_UPDATE_MICROCODE+KX_PROP_GET:
    {
        char *m=(char *)&inst[1];
        if(m)
        {
           if(req->InstanceSize<(4+sizeof(dsp_microcode)+4+4+4+4))
           {
            debug(DWDM,"!!! Instance size invalid (update_microcode());\n");
            return STATUS_INVALID_PARAMETER;
           }
           if( (*(dword *)m!=UPDATE_MICROCODE_MAGIC) ||
               (*(dword *)(m+4+sizeof(dsp_microcode))!=UPDATE_MICROCODE_MAGIC))
           {
            debug(DWDM,"!!! Bad magic 2 for update_microcode()\n");
            return STATUS_INVALID_PARAMETER;
           }
           dsp_microcode *tmp_m=(dsp_microcode *)(m+4);
                   dsp_register_info *info=(dsp_register_info *)(m+4+sizeof(dsp_microcode)+4);
           dsp_code *code=(dsp_code *)(m+4+sizeof(dsp_microcode)+4+tmp_m->info_size);
           unsigned int flag=*(unsigned int *)(m+4+sizeof(dsp_microcode)+4+tmp_m->info_size+tmp_m->code_size);
           int pgm_id=*(int *)(m+4+sizeof(dsp_microcode)+4+tmp_m->info_size+tmp_m->code_size+4);
           if(*(dword *)(m+4+sizeof(dsp_microcode)+4+tmp_m->info_size+tmp_m->code_size+4+4)!=UPDATE_MICROCODE_MAGIC)
           {
            debug(DWDM,"!!! Bad magic 3 for update_micrcode();\n");
            return STATUS_INVALID_PARAMETER;
           }

                   if(req->InstanceSize-sizeof(my_prop)!=(4+sizeof(dsp_microcode)+4+tmp_m->info_size+tmp_m->code_size+4+4+4))
                   {
                    debug(DWDM,"!!! Bad instance size II load_microcode(): %d vs %d\n",
                     req->InstanceSize,(4+sizeof(dsp_microcode)+4+tmp_m->info_size+tmp_m->code_size+4+4+4));
                    return STATUS_INVALID_PARAMETER;
                   }

           int ret=kx_update_microcode(hw,pgm_id,tmp_m->name,code,tmp_m->code_size,
              info,tmp_m->info_size,tmp_m->itramsize,tmp_m->xtramsize,
              tmp_m->copyright,
              tmp_m->engine,
              tmp_m->created,
              tmp_m->comment,
              tmp_m->guid,flag);

                   dword *out=(dword *)req->Value;
                   if(out)
                   {
                     out[0]=(dword)ret;
                   }
                    else return STATUS_INVALID_PARAMETER;
        } else return STATUS_INVALID_PARAMETER;
    }
    break;
  case KX_PROP_GET_MICROCODE+KX_PROP_GET:
    {
        dword *m=(dword *)&inst[1];
        if(m)
        {
           if(m[0]!=GET_MICROCODE_MAGIC)
           {
            debug(DWDM,"!!! Bad magic for get_microcode()\n");
            return STATUS_INVALID_PARAMETER;
           }
           int code_off=4;
           int info_off=code_off+(m[1]+1)/4+1;
                   int last_off=info_off+(m[2]+1)/4+1;
                   int pgm=(int)m[3];
                   dword *res=(dword *)req->Value;

                   dsp_code *code=(dsp_code *)&m[code_off];
                   dsp_register_info *info=(dsp_register_info *)&m[info_off];

           if((int)(req->InstanceSize-sizeof(my_prop))!=(last_off+1)*4)
           { 
             debug(DWDM,"!!! get_microcode() instance too little (%d) (%d)\n",
               req->InstanceSize-sizeof(kx_property_instance)-1,
               (last_off+1)*4);
              return STATUS_INVALID_PARAMETER; 
           }
           if(m[last_off]!=GET_MICROCODE_MAGIC)
           {
            debug(DWDM,"!!! bad second magic for get_microcode()\n");
            return STATUS_INVALID_PARAMETER;
           }
           int ret=kx_get_microcode(hw,pgm,code,m[1],info,m[2]);
           if(ret)
            debug(DWDM,"!!! -- get_microcode failed (%d)\n",ret);

           dword *out=(dword *)req->Value;
           if(out)
           {
             out[0]=(dword)ret;
             memcpy(&out[info_off],&m[info_off],m[2]);
             memcpy(&out[code_off],&m[code_off],m[1]);
             out[last_off]=GET_MICROCODE_MAGIC;
           }
            else return STATUS_INVALID_PARAMETER;
        } else return STATUS_INVALID_PARAMETER;
    }
    break;
  case KX_PROP_SOUNDFONT_LOAD+KX_PROP_GET:
    {
        kx_sound_font *in = (kx_sound_font *)&inst[1];
        kx_sound_font *out = (kx_sound_font *)req->Value;
        out->id=kx_load_soundfont(hw,in);
    }
    break;
  case KX_PROP_SOUNDFONT_LOAD_SMPL+KX_PROP_GET:
    {
    prep_in(sf_load_sample_property);
    if(kx_load_soundfont_samples(hw,in))
     return STATUS_INVALID_PARAMETER;
    }
    break;
  case KX_PROP_SOUNDFONT_UNLOAD+KX_PROP_GET:
    {
    prep_in(int);
    prep_out(int);
    *out=kx_unload_soundfont(hw,*in);
    }
    break;
  case KX_PROP_SOUNDFONT_QUERY+KX_PROP_GET:
    {
    prep_in(int);
    prep_out(int);
        *out=kx_enum_soundfonts(hw,NULL,0);
    }
    break;
  case KX_PROP_SOUNDFONT_ENUM+KX_PROP_GET:
    {
    sfHeader *out=(sfHeader*)req->Value;
    int *size=(int *)&inst[1];
    if(kx_enum_soundfonts(hw,out,*size))
    {
     // error
     return STATUS_INVALID_PARAMETER;
    }
    }
    break;
  case KX_PROP_SOUNDFONT_PRESET_DESCR+KX_PROP_GET:
    {
    prep_in(sfpreset_query_property);
    prep_out(sfpreset_query_property);
    if(kx_sf_get_preset_description(hw,in->bank,in->preset,out->name)==0)
    {
     out->bank=0; out->preset=0;
    }
    else
    {
     out->bank=-1;
    }
    }
    break;
  case KX_PROP_SOUNDFONT_SYNTH+KX_PROP_GET:
    {
     prep_in(kx_synth_data);
     int cmd=in->data&0xf0;
     if((cmd==0xc0) || (cmd==0xd0))
     {
      if(hw->midi[in->synth_id&1])
       kx_midi_play_buffer(hw->midi[in->synth_id&1],(byte *)&in->data,2);
      else
       debug(DWDM,"kXdirectSynth: no midi device opened\n");
     }
     if((cmd==0x80) || (cmd==0x90) || (cmd==0xa0) || (cmd==0xb0) || (cmd==0xe0))
     {
      if(hw->midi[in->synth_id&1])
       kx_midi_play_buffer(hw->midi[in->synth_id&1],(byte *)&in->data,3);
      else
       debug(DWDM,"kXdirectSynth: no midi device opened\n");
     }
         // 0xf0: skip
    }
    break;
  case KX_PROP_MICROCODE_QUERY+KX_PROP_GET:
    {
    prep_in(int);
    prep_out(int);
        *out=kx_enum_microcode(hw,(dsp_microcode *)NULL,0);
    }
    break;
  case KX_PROP_GET_CONNECTIONS_QUERY+KX_PROP_GET:
    {
    prep_in(int);
    prep_out(int);
        *out=kx_get_connections(hw,*in,NULL,0);
    }
    break;
  case KX_PROP_GET_CONNECTIONS+KX_PROP_GET:
    {
    if(req->ValueSize<4 || (req->InstanceSize-sizeof(my_prop))<8)
    {
     debug(DWDM,"!! get_connections(): valuesize=%d instancesize=%d\n",req->ValueSize,req->InstanceSize);
     return STATUS_INVALID_PARAMETER;
    }

    int *out=(int *)req->Value;
    int *pgm=(int *)&inst[1];
    int *size=(int *)((uintptr_t) &inst[1] + 4);
    if(req->ValueSize==(*size))
     {
      if(kx_get_connections(hw,*pgm,(kxconnections *)req->Value,*size))
       return STATUS_INVALID_PARAMETER;
     }
     else
     {
      debug(DWDM,"!! incorrect size in get_connections() [%d; %d]\n",
       req->ValueSize,*size);
      return STATUS_INVALID_PARAMETER;
     }
    }
    break;
  case KX_PROP_MICROCODE_ENUM_ALL+KX_PROP_GET:
    {
    dsp_microcode *mc=(dsp_microcode *)req->Value;
    int *size=(int *)&inst[1];
    if(kx_enum_microcode(hw,mc,*size))
    {
     // error
     return STATUS_INVALID_PARAMETER;
    }
    }
    break;
  case KX_PROP_ASIO_CAPS+KX_PROP_GET:
    {
     if(req->InstanceSize-sizeof(my_prop)==req->ValueSize && req->ValueSize>=(sizeof(int)+sizeof(kx_caps)))
     {
         int *in_size=(int *)&inst[1];
         int *out_size=(int *)req->Value;

         *out_size=*in_size;
         kx_caps *caps=(kx_caps *)(out_size+1);

         if(kx_get_device_caps(hw,caps,out_size))
          return STATUS_INVALID_PARAMETER;
     }
      else return STATUS_INVALID_PARAMETER;
    }
    break;
  case KX_PROP_VOICE_INFO+KX_PROP_GET:
    {
    kx_voice_info *out=(kx_voice_info *)req->Value;
    if(req->InstanceSize-sizeof(my_prop)!=sizeof(kx_voice_info)*64)
    {
     debug(DWDM,"invalid instances for kx_get_voice_info()\n");
     return STATUS_INVALID_PARAMETER;
    }
    kx_get_voice_info(hw,KX_VOICE_INFO_USAGE,out);
    }
    break;
  case KX_PROP_SPECTRAL_INFO+KX_PROP_GET:
    {
    kx_voice_info *out=(kx_voice_info *)req->Value;
    if(req->InstanceSize-sizeof(my_prop)!=sizeof(kx_voice_info)*64)
    {
     debug(DWDM,"invalid instances for kx_get_voice_info()\n");
     return STATUS_INVALID_PARAMETER;
    }
    kx_get_voice_info(hw,KX_VOICE_INFO_SPECTRAL,out);
    }
    break;
  case KX_PROP_MUTE+KX_PROP_GET:
    {
      kx_mute(hw);
    }
    break;
  case KX_PROP_UNMUTE+KX_PROP_GET:
    {
      kx_unmute(hw);
    }
    break;
  default:
    debug(DWDM,"!!! Unknown property req (%x)\n",inst->inst.prop);
    return STATUS_INVALID_PARAMETER;
 }

 return STATUS_SUCCESS;
}


