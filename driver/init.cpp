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

#include "frname.cpp"

const char *copyright=KX_COPYRIGHT_STR;

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
}

// bus=0xff for autoscan PCI
int pci_init(kx_hw *hw);
int pci_init(kx_hw *hw)
{
 if((hw->cb.io_base==0)||(hw->standalone)) // do autoscan
 {
 word wdata;
 byte bdata;
 dword device;
 dword subsys;
 byte chip_rev;
 byte bus,dev,func;

 int cnt=hw->standalone;


    for(bus = 0 ; bus <= 0xfe ; bus++)
      for(dev = 0 ; dev < 32 ; dev++)
        for(func = 0 ; func < 8 ; func++)
        {
          pcibios_read_config_dword(bus,((dev<<3)+func),PCI_DEVID,&device);

          if((device==0x021102) || (device==0x041102) || (device==0x081102)) // Emu10kx + Creative
          {
             pcibios_read_config_dword(bus,((dev<<3)+func),PCI_SUBSYSID_R,&subsys);
             pcibios_read_config_byte(bus,((dev<<3)+func),PCI_REVISION_ID, &chip_rev);
             pcibios_read_config_word(bus,((dev<<3)+func),0x10,&wdata);
             hw->port=wdata&0xfffe;
             if(inpd(hw->port)==0xffffffff) // disabled
             {
              debug(DLIB,"-Found disabled device [%x] - skipped\n",subsys);
              continue;
             }

             if(cnt>1)
             {
              cnt--;
              continue;
             }

             pcibios_read_config_byte(bus,((dev<<3)+func),0x3c,&bdata);
             hw->irq=bdata;

             hw->pci_device=device;
             hw->pci_subsys=subsys;
             hw->pci_chiprev=chip_rev;
             hw->pci_bus=bus;
             hw->pci_dev=dev;
             hw->pci_func=func;
             goto FOUND;
          }
        }

    // here we get only if no emu10kx is present
    debug(DLIB,"PCI: no Emu10kx found\n");
    return 2; // error: not found

 } // end autoscan
 else // provided device/subsys/chip_rev & bus/dev/fn
 {
  /// use WDM supplied resources
  hw->port=hw->cb.io_base;
  hw->irq=hw->cb.irql;

  hw->pci_device=hw->cb.device; // should be real
  hw->pci_subsys=hw->cb.subsys;
  hw->pci_chiprev=hw->cb.chip_rev;

  hw->pci_bus=hw->cb.bus; // may be unreal
  hw->pci_dev=hw->cb.dev;
  hw->pci_func=hw->cb.func;
 }

FOUND: // bus,dev,func contain right values

  if(hw->pci_device!=0x021102 && hw->pci_device!=0x041102 && hw->pci_device!=0x081102) // Emu10kx + Creative
  {
   debug(DLIB,"PCI: bad device/subsys supplied [%x]\n",hw->pci_device);
   return 3;
  }

  debug(DLIB,"Hw PCI device: dev: %x sub: %x rev: %x\nPCI: resources: io:%x irq:%x\n"
   "Hw PCI location: bus: %xh dev: %xh func: %xh (invalid under 98se)\n",hw->pci_device,hw->pci_subsys,hw->pci_chiprev,
   hw->port,hw->irq,
   hw->pci_bus,hw->pci_dev,hw->pci_func);

 hw->is_aps=0;
 hw->is_51=0;
 hw->has_surdac=0;
 hw->have_ac97=0; // assume '0' [autdetect]
 hw->lack_ac97=0;
 hw->is_k8=0;
 hw->is_a4=0;
 hw->is_edsp=0;
 hw->is_10k2=0;
 hw->is_zsnb=0;
 hw->is_cardbus=0;
 hw->can_k8_passthru=0;

 hw->drum_channel=10;

 char tmp_str[KX_MAX_STRING];
 kx_get_friendly_name(hw->pci_device,hw->pci_subsys,hw->pci_chiprev,tmp_str,
                hw->is_51,hw->has_surdac,
        hw->is_aps,hw->is_10k2,hw->is_a2,hw->is_a2ex,hw->is_k8,hw->is_a4,hw->is_edsp,
        hw->have_ac97,hw->lack_ac97,hw->is_zsnb,hw->is_cardbus);

 hw->can_k8_passthru=hw->is_k8;

 char *p=&tmp_str[strlen(tmp_str)];
 *p=' '; p++; *p='['; p++;
 itoax(p,hw->port); p+=4;
 *p=']'; p++; *p=0;

 strncpy(hw->card_name,tmp_str,KX_MAX_STRING);
 debug(DLIB,"card name: '%s'\n",hw->card_name);

 if(hw->is_10k2) 
  hw->can_passthru=1;
/* else
 {
  if(hw->pci_chiprev>=7)
   hw->can_passthru=1; // FIXME: this is not true... (imho)
  else
   hw->can_passthru=0;
 }
*/

 /* FIXME
    PCI_COMMAND |= PCI_COMMAND_IO
    if PCI_LATENCY_TIMER<32 -> =32
    enable bus mastering
 */
 return 0;
}

static int kx_pci_buffers_init(kx_hw *hw)
{
    int ret=0;

    hw->tankmem.size = hw->cb.tram_size;
    hw->virtualpagetable.size = MAXPAGES * sizeof(dword);
    hw->silentpage.size = KX_PAGE_SIZE;
    hw->mtr_buffer.size=65536; // always

    if(!hw->cb.pci_alloc || hw->standalone)
         return 0;

    ret=hw->cb.pci_alloc(hw->cb.call_with,&hw->virtualpagetable,KX_NONCACHED);
    if(ret)
    {
     debug(DLIB,"Error allocating PCI for PT\n");
     return ret;
    }

    ret=hw->cb.pci_alloc(hw->cb.call_with,&hw->silentpage,KX_NONCACHED);
    if(ret)
    { 
      debug(DLIB,"Error allocating PCI for first page\n");
      hw->cb.pci_free(hw->cb.call_with,&hw->virtualpagetable);
      return ret; 
    }

    ret=hw->cb.pci_alloc(hw->cb.call_with,&hw->mtr_buffer,KX_NONCACHED);
    if(ret)
    {
     debug(DLIB,"Error allocating PCI for MTR buffer\n");
     return ret;
    }

    if(hw->cb.tram_size>0)
    {
            ret=hw->cb.pci_alloc(hw->cb.call_with,&hw->tankmem,KX_NONCACHED);
            if(ret)
            { 
               debug(DLIB,"Error allocating PCI for TRAM\n");
               hw->cb.pci_free(hw->cb.call_with,&hw->virtualpagetable);
               hw->cb.pci_free(hw->cb.call_with,&hw->silentpage);
                   hw->cb.pci_free(hw->cb.call_with,&hw->mtr_buffer);
               hw->cb.tram_size=0;
               return ret; 
            }
            memset(hw->tankmem.addr,0,hw->tankmem.size);
    }

    hw->initialized|=KX_BUFFERS_INITED;

    return ret;
}

KX_API(int,kx_set_tram_size(kx_hw *hw,int new_size))
{
 dword trbs=0;
 switch(new_size)
 {
         case 16*1024: trbs=TRBS_BUFFSIZE_16K; break;
         case 32*1024: trbs=TRBS_BUFFSIZE_32K; break;
         case 64*1024: trbs=TRBS_BUFFSIZE_64K; break;
         case 128*1024: trbs=TRBS_BUFFSIZE_128K; break;
         case 256*1024: trbs=TRBS_BUFFSIZE_256K; break;
         case 512*1024: trbs=TRBS_BUFFSIZE_512K; break;
         case 1024*1024: trbs=TRBS_BUFFSIZE_1024K; break;
         case 2048*1024: trbs=TRBS_BUFFSIZE_2048K; break;
         case 0: // disable TRAM
            trbs=0x0;
            break;
         default:
            debug(DLIB,"!!! Wrong tank_size (%d) specified\n",new_size);
            return -5;
 }

 if(hw)
 {
  if(hw->cb.tram_size!=new_size)
  {
   debug(DLIB,"Changing TRAM size to %dkb\n",new_size/1024);

   dword prev_hcfg=kx_readfn0(hw,HCFG_K1);
   kx_writefn0(hw, HCFG_K1, prev_hcfg|HCFG_LOCKTANKCACHE_MASK);

   // stop DSP
   kx_dsp_stop(hw);

   if(hw->tankmem.addr)
     hw->cb.pci_free(hw->cb.call_with,&hw->tankmem);

   hw->tankmem.addr=0;

   hw->cb.tram_size=new_size;
   hw->tankmem.size = hw->cb.tram_size;
   if(hw->cb.tram_size>0)
   {
          if(hw->cb.pci_alloc(hw->cb.call_with,&hw->tankmem,KX_NONCACHED))
          { 
            debug(DLIB,"Error allocating PCI for TRAM\n");

            hw->cb.tram_size=0;
            hw->tankmem.size = 0;
            hw->tankmem.addr = 0;
            kx_dsp_go(hw);

            return -10; 
          }
          memset((dword *)hw->tankmem.addr,0,hw->tankmem.size);
   }
   else
   {
    hw->cb.tram_size=0;
    hw->tankmem.size=0;
    hw->tankmem.addr=0;
   }
   kx_writeptr_multiple(hw, 0,
                TRBA, hw->cb.tram_size?hw->tankmem.dma_handle:0,
                TRBS, trbs, 
                REGLIST_END);

   prev_hcfg&=~HCFG_LOCKTANKCACHE_MASK;
   kx_writefn0(hw, HCFG_K1, prev_hcfg|((hw->cb.tram_size==0)?HCFG_LOCKTANKCACHE_MASK:0));

   // go DSP
   kx_dsp_go(hw);
  }
 } else return -1;
 return 0;
}

static int kx_pci_buffers_close(kx_hw *hw)
{
 if(!hw->cb.pci_alloc || !hw->cb.pci_free || hw->standalone)
 {
  debug(DLIB,"!!! NB: pci_buffers_close skipped\n");
  return 0;
  }

 if(hw->virtualpagetable.addr)
   hw->cb.pci_free(hw->cb.call_with,&hw->virtualpagetable);
 if(hw->silentpage.addr)
  hw->cb.pci_free(hw->cb.call_with,&hw->silentpage);
 if(hw->mtr_buffer.addr)
  hw->cb.pci_free(hw->cb.call_with,&hw->mtr_buffer);
 if(hw->tankmem.addr)
  hw->cb.pci_free(hw->cb.call_with,&hw->tankmem);

 hw->initialized&=~KX_BUFFERS_INITED;
 return 0;
}

extern void system_timer_func(void *data,int what);

KX_API(int,kx_init(kx_hw **hw_,kx_callbacks *cb,int standalone))
{
 kx_hw *hw=NULL;
 (cb->malloc_func)(cb->call_with,sizeof(kx_hw),(void **)&hw,KX_NONPAGED);
 if(hw==NULL)
  return -2;

 my_memset(hw,0,sizeof(kx_hw));

 *hw_=hw;
 hw->standalone=(byte)standalone;
 my_memcpy(&hw->cb,cb,sizeof(kx_callbacks));

 my_strncpy(hw->kx_version,KX_DRIVER_VERSION_STR,KX_MAX_STRING);
 my_strncpy(hw->kx_date,__DATE__" "__TIME__,KX_MAX_STRING);

 my_strncpy(hw->kx_driver,"kX Audio Driver"
 #ifndef KX_DEBUG
  " (Release)",
 #else
  " (Debug)",
 #endif
           KX_MAX_STRING
 );

 hw->initialized=0;

 hw->power_state=KX_POWER_NORMAL;

 hw->mtr_buffer_size=0;

 hw->is_a2ex=0;
 hw->is_a2=0;
 hw->have_ac97=0; // assume autodetect
 hw->lack_ac97=0;
 hw->spdif_decode=0;
 hw->spdif_recording=0;

 hw->ext_flags=0;

 hw->synth_compat=KX_SYNTH_DEFAULTS;

 hw->irq_pending=0;

 hw->cur_asio_in_bps=0;
 hw->card_frequency=48000;

 int i;
 for(i=0;i<MAX_MPU_DEVICES;i++)
 {
  hw->uart_out_tail[i]=0;
  hw->uart_out_head[i]=0;
 }

 debug(DLIB,"--- kX Software Abstraction Level Library init ---\n%s\nversion: %s\n"KX_COPYRIGHT_STR"\nLibrary Compiled "__DATE__", "__TIME__"\n\n",
  hw->kx_driver,
  KX_DRIVER_VERSION_STR);

 // Set up the initial settings
 hw->ecard.spdif1=KX_EC_SOURCE_EDRIVE;
 hw->ecard.spdif0=KX_EC_SOURCE_ECARD;
 hw->ecard.adc_gain=EC_DEFAULT_ADC_GAIN;
 hw->ecard.control_bits=EC_RAW_RUN_MODE|(dword)(hw->ecard.spdif1<<KX_EC_SPDIF1_SHIFT)|(dword)(hw->ecard.spdif0<<KX_EC_SPDIF0_SHIFT);

 my_memset(&hw->sys_timer,0,sizeof(kx_timer));
 hw->sys_timer.status=TIMER_UNINSTALLED;

 my_memset(&hw->p16v_pb_timer,0,sizeof(kx_timer));
 hw->p16v_pb_timer.status=TIMER_UNINSTALLED;

 my_memset(&hw->p16v_rec_timer,0,sizeof(kx_timer));
 hw->p16v_rec_timer.status=TIMER_UNINSTALLED;

 my_memset(hw->p16v_volumes,0,sizeof(hw->p16v_volumes));

 hw->p16v_pb_opened=0;
 hw->p16v_rec_opened=0;

 my_memset(&hw->ac3_pt_state,0,sizeof(kx_ac3_passthru));

 kx_spin_lock_init(hw,&hw->hw_lock,"hw");
 kx_spin_lock_init(hw,&hw->timer_lock,"timer");
 kx_spin_lock_init(hw,&hw->uartout_lock,"uart");

 for(i=0;i<MAX_MPU_DEVICES;i++)
  kx_spin_lock_init(hw,&hw->mpu_lock[i],"mpu");

 kx_spin_lock_init(hw,&hw->k_lock,"generic");
 kx_spin_lock_init(hw,&hw->pt_lock,"pt");
 kx_spin_lock_init(hw,&hw->ac97_lock,"ac97");
 kx_spin_lock_init(hw,&hw->dsp_lock,"dsp");
 kx_spin_lock_init(hw,&hw->sf_lock,"sf");

 // voice init
 my_memset(hw->voicetable,0,sizeof(hw->voicetable));
 hw->last_voice=0;
 
 for(i = 0; i < KX_NUMBER_OF_VOICES; i++)
 {
  hw->voicetable[i].usage = VOICE_USAGE_FREE;
  hw->voicetable[i].asio_channel = 0xffffffff;
 }

 for(i = 0; i < KX_NUMBER_OF_REC_VOICES; i++)
 {
  hw->rec_voicetable[i].usage = VOICE_USAGE_FREE;
 }

 hw->midi[0]=NULL;
 hw->midi[1]=NULL;

 hw->pt_spdif=0;

 hw->sys_timer.timer_func=system_timer_func;
 hw->sys_timer.data=hw;

 hw->p16v_pb_timer.timer_func=hw->cb.notify_func;
 hw->p16v_pb_timer.data=0;

 hw->p16v_rec_timer.timer_func=hw->cb.notify_func;
 hw->p16v_rec_timer.data=0;

 memset(hw->asio_inputs,0,sizeof(hw->asio_inputs));

 memset(&hw->asio_notification_krnl,0,sizeof(hw->asio_notification_krnl));

 hw->asio_notification_krnl.pb_buf=-1;
 hw->asio_notification_krnl.rec_buf=-1;
 hw->asio_notification_krnl.toggle=-1;
 hw->asio_notification_krnl.asio_method=0;
 hw->asio_notification_krnl.n_voice=-1;
 hw->asio_notification_krnl.semi_buff=0;
 hw->asio_notification_krnl.active=0;
 hw->asio_notification_krnl.cur_pos=0;
 hw->asio_notification_krnl.kevent=0;

 int res=kx_pci_buffers_init(hw);

   if(!res)
   {
        kx_soundfont_init(hw);

        res=pci_init(hw);

        // inital swap set-up
        #if !defined(__APPLE__) && !defined(__MACH__) // MacOSX
        if(!hw->lack_ac97)
          hw->dsp_flags=KX_DSP_SWAP_REAR;
        else 
          hw->dsp_flags=0;
        #endif

        if(!res)
        {
             // this should be after pci_init(), but before hal_init()
             // this should run in 'standalone' mode
             if(hw->is_10k2)
             {
              hw->opcode_shift=E10K2_OP_SHIFT_LOW;
              hw->high_operand_shift=E10K2_OP_SHIFT_HI;
              hw->first_instruction=E10K2_MICROCODE_BASE;
              hw->microcode_size=E10K2_MAX_INSTRUCTIONS;
             }
             else
             {
              hw->opcode_shift=E10K1_OP_SHIFT_LOW;
              hw->high_operand_shift=E10K1_OP_SHIFT_HI;
              hw->first_instruction=E10K1_MICROCODE_BASE;
              hw->microcode_size=E10K1_MAX_INSTRUCTIONS;
             }

             if(standalone)
               goto OK;

             res=kx_bufmgr_init(hw);
             if(!res)
             {
                    res=kx_timer_init(hw);
                    if(!res)
                    {
                        res=kx_hal_init(hw);
                        if(!res)
                        {
                                goto OK;
                        }
                        kx_timer_close(hw);
                        kx_bufmgr_close(hw);
                        debug(DLIB,"!! HAL initialization failed\n");
                    }
                    kx_bufmgr_close(hw);
                    debug(DLIB,"!!! timer initialization failed\n");
             }
             else
                debug(DLIB,"!!! bufmgr initialization failed\n");
       }
       else
         debug(DLIB,"!!! PCI init failed\n");

       kx_pci_buffers_close(hw);
   }

OK:
 if(res)
  return res;

 kx_midi_set_volume(hw,0,0x0);
 kx_midi_set_volume(hw,1,0x0);

 if((!hw->is_aps)&&(!hw->is_10k2)&&!standalone&&hw->have_ac97) // enable master volume (note: DOO)
  kx_ac97write(hw,AC97_REG_MASTER_VOL,0x0);

 if(hw->is_edsp)
  hw->gp_ins=0xff;
 else
  hw->gp_ins=kx_get_gp_inputs(hw);

 debug(DLIB,"--- Library init completed ---\n");

 return res;
}

KX_API(int, kx_close(kx_hw **hw_))
{
 kx_hw *hw=*hw_;

 if(!(hw->sys_timer.status&TIMER_UNINSTALLED))
 {
  // dangerous: should have been already uninstalled
  debug(DERR,"sys timer still active\n");
  kx_timer_disable(hw,&hw->sys_timer);
  kx_timer_uninstall(hw,&hw->sys_timer);
 }

 // mute audio for now
 if(!hw->standalone)
 {
     hw->midi[0]=NULL;
     hw->midi[1]=NULL;

     kx_timer_close(hw); // nop

     kx_soundfont_close(hw);

     kx_hal_close(hw); 

     kx_bufmgr_close(hw);

     kx_pci_buffers_close(hw);
 }

 debug(DLIB,"--- Library done ---\n");
 (hw->cb.free_func)(hw->cb.call_with,hw);
 *hw_=NULL;
 hw_=NULL;

 return 0;
}

KX_API(int,kx_getstring(kx_hw *hw,int what,char *buff))
{
 char *p=NULL;
 switch(what)
 {
    case KX_STR_DRIVER_NAME:
        p=hw->kx_driver;
        break;
    case KX_STR_DRIVER_DATE:
        p=hw->kx_date;
        break;
    case KX_STR_DRIVER_VERSION:
        p=hw->kx_version;
        break;
    case KX_STR_CARD_NAME:
        p=hw->card_name;
        break;
    case KX_STR_AC97CODEC_NAME:
        p=hw->ac97_codec_name;
        break;
    case KX_STR_AC973D_NAME:
        p=hw->ac97_3d_name;
        break;
    case KX_STR_DB_NAME:
        p=hw->db_name;
        break;
    default:
        p=NULL;
 }
 if(p)
 {
  my_strncpy(buff,p,KX_MAX_STRING);
  return 0;
 }
 else
  return 1;
}

KX_API(dword,kx_getdword(kx_hw *hw,int what,dword *ret))
{
  dword r=0;
  switch(what)
  {
    case KX_DWORD_CARD_PORT:
        *ret=(dword)hw->port;
        break;
    case KX_DWORD_CARD_IRQ:
        *ret=(dword)hw->irq;
        break;
    case KX_DWORD_AC97_CAPS:
        *ret=(dword)hw->ac97_cap;
        break;
    case KX_DWORD_AC97_ID1:
        *ret=(dword)hw->ac97_id1;
        break;
    case KX_DWORD_AC97_ID2:
        *ret=(dword)hw->ac97_id2;
        break;
    case KX_DWORD_AC97_IS20:
        *ret=(dword)hw->ac97_is_20;
        break;
    case KX_DWORD_AC97_EXTID:
        *ret=(dword)hw->ac97_extid;
        break;
    case KX_DWORD_AC97_PRESENT:
        *ret=(dword)hw->have_ac97;
        break;
    case KX_DWORD_IS_51:
        *ret=(dword)hw->is_51;
        break;
    case KX_DWORD_IS_APS:
        *ret=(dword)hw->is_aps;
        break;  
    case KX_DWORD_IS_A2EX:
        *ret=(dword)hw->is_a2ex;
        break;
    case KX_DWORD_IS_A2:
        *ret=(dword)hw->is_a2;
        break;
    case KX_DWORD_IS_10K2:
        *ret=(dword)hw->is_10k2;
        break;
    case KX_DWORD_HAVE_MPU:
        *ret=(dword)hw->have_mpu;
        break;
    case KX_DWORD_SUBSYS:
        *ret=(dword)hw->pci_subsys;
        break;
    case KX_DWORD_CHIPREV:
        *ret=(dword)hw->pci_chiprev;
        break;
    case KX_DWORD_DEVICE:
        *ret=(dword)hw->pci_device;
        break;
    case KX_DWORD_CAN_PASSTHRU:
        *ret=(dword)hw->can_passthru;
        break;
    case KX_DWORD_IS_A4:
        *ret=(dword)hw->is_a4;
        break;
    case KX_DWORD_IS_K8:
        *ret=(dword)hw->is_k8;
        break;
    case KX_DWORD_IS_EDSP:
        *ret=(dword)hw->is_edsp;
        break;
    case KX_DWORD_IS_BAD_SB22X:
        *ret=(dword)hw->is_bad_sb22x;
        break;
    case KX_DWORD_IS_A2ZSNB:
        *ret=(dword)hw->is_zsnb;
        break;
    case KX_DWORD_IS_CARDBUS:
        *ret=(dword)hw->is_cardbus;
        break;
    case KX_DWORD_CAN_K8_PASSTHRU:
        *ret=(dword)hw->can_k8_passthru;
        break;
    default:
        *ret=0;
        r=(dword)-1; // not found
 }
 // debug(DLIB,"getting dword: %x, %x, %x\n",(unsigned)what,(unsigned)*ret,(unsigned)r);
    
 return r;
}

KX_API(int,kx_get_voice_info(kx_hw *hw,int what,kx_voice_info *vi))
{
 unsigned long flags=0;

 switch(what)
 {
 case KX_VOICE_INFO_USAGE:
     int i;

     kx_lock_acquire(hw,&hw->hw_lock, &flags);

     for(i=0;i<KX_NUMBER_OF_VOICES;i++)
     {
      vi[i].usage=hw->voicetable[i].usage;
      if(voice_usage(hw->voicetable[i].usage)==VOICE_USAGE_PLAYBACK)
      {
        if(hw->voicetable[i].synth_id&VOICE_STATUS_AC3)
         vi[i].usage|=VOICE_STATUS_AC3;
        if(hw->voicetable[i].synth_id&VOICE_STATUS_BUFFERED)
         vi[i].usage|=VOICE_STATUS_BUFFERED;
        if(hw->voicetable[i].synth_id&VOICE_STATUS_24BIT)
         vi[i].usage|=VOICE_STATUS_24BIT;
        if(hw->voicetable[i].synth_id&VOICE_STATUS_AC3PT)
         vi[i].usage|=VOICE_STATUS_AC3PT;
      }
      outpd(hw->port + PTR,((CVCF << 16) & PTR_ADDRESS_MASK) | (i & PTR_CHANNELNUM_MASK));
      vi[i].cur_vol=(inpd(hw->port + DATA)&0xffff0000)>>16;
      if(hw->is_10k2) // 3543 change
       vi[i].cur_vol<<=1;
     }

     kx_lock_release(hw,&hw->hw_lock, &flags);
     return 0;
 case KX_VOICE_INFO_SPECTRAL:
     // FIXME: unimplemented
     return 0;
 }
 return -1;
}

static kx_caps emu10k1_caps[]=
{
 { KX_CAPS_10K1, 16, 48000, 16, 16, "10kX 16 bit/48 kHz [16+16]",   -1,-1,-1 },
 { KX_CAPS_10K1, 16, 44100, 16,  0, "10kX 16 bit/44.1 kHz [16+0]",  -1,-1,-1 },
 { KX_CAPS_10K1, 16, 48000, 8,   8, "10kX 16 bit/48 kHz [8+8]",     -1,-1,-1 },
 { KX_CAPS_10K1, 16, 44100, 8,   0, "10kX 16 bit/44.1 kHz [8+0]",   -1,-1,-1 },
 { KX_CAPS_10K1, 16, 48000, 2,   2, "10kX 16 bit/48 kHz [2+2]",     -1,-1,-1 },
 { KX_CAPS_10K1, 16, 44100, 2,   0, "10kX 16 bit/44.1 kHz [2+0]",   -1,-1,-1 }
};

static kx_caps emu10k2_caps_44[]=
{
 { KX_CAPS_10K1, 16, 44100, 16, 16, "10kX 16 bit/44.1 kHz [16+16]",   -1,-1,-1 },
 { KX_CAPS_10K1, 16, 44100, 8,   8, "10kX 16 bit/44.1 kHz [8+8]",     -1,-1,-1 },
 { KX_CAPS_10K1, 16, 44100, 2,   2, "10kX 16 bit/44.1 kHz [2+2]",     -1,-1,-1 },
 { KX_CAPS_10K2, 32, 44100, 8,   0, "10k2 24 bit/44.1 kHz [8+0]",     -1,-1,-1 },
 { KX_CAPS_10K2, 32, 44100, 8,   8, "10k2 24 bit/44.1 kHz [8+8]",     -1,-1,-1 },
};

static kx_caps emu10k2_caps[]=
{
 { KX_CAPS_10K2, 32, 48000, 8,   0, "10k2 24 bit/48 kHz [8+0]",     -1,-1,-1 },
 { KX_CAPS_10K2, 32, 48000, 8,   8, "10k2 24 bit/48 kHz [8+8]",     -1,-1,-1 },
};

/*
static kx_caps p16v_caps[]=
{
 { KX_CAPS_P16V, 32, 96000, 8,   8, "HQ P16V 24 bit/96 kHz [8+8]",  -1,-1,-1 }
};

static kx_caps p17v_caps[]=
{
 { KX_CAPS_P17V, 32, 96000, 8,   8, "HQ P17V 24 bit/96 kHz [8+8]",  -1,-1,-1 }
};

static kx_caps edsp_caps[]=
{
 { KX_CAPS_EDSP, 32, 96000, 8,   8, "E-DSP 24 bit/48 kHz [8+8]",     -1,-1,-1 }
};
*/

KX_API(int,kx_get_device_caps(kx_hw *hw,kx_caps *caps,int *sz))
{
 int final_sz=0;

 if(!sz || *sz<=(int)sizeof(kx_caps))
  return -3;

 // copy data
 // E-DSP specific settings in 44.1 mode:
 if(hw->card_frequency==44100)
 {
    if(*sz>=(int)sizeof(emu10k2_caps_44))
    {
        memcpy(caps,&emu10k2_caps_44,sizeof(emu10k2_caps_44));
        *sz-=(int)sizeof(emu10k2_caps_44);
        final_sz+=(int)sizeof(emu10k2_caps_44);
        caps+=sizeof(emu10k2_caps_44)/sizeof(kx_caps);
    }
    *sz=final_sz;
    return 0;
 }

 // 10k1-only settings:
 if(*sz>=(int)sizeof(emu10k1_caps))
 {
   memcpy(caps,&emu10k1_caps,sizeof(emu10k1_caps));
   *sz-=(int)sizeof(emu10k1_caps);
   final_sz+=(int)sizeof(emu10k1_caps);
   caps+=sizeof(emu10k1_caps)/sizeof(kx_caps);
 }

 // 10k2-only settings:
 if(hw->is_10k2)
 if(*sz>=(int)sizeof(emu10k2_caps))
 {
   memcpy(caps,&emu10k2_caps,sizeof(emu10k2_caps));
   *sz-=(int)sizeof(emu10k2_caps);
   final_sz+=(int)sizeof(emu10k2_caps);
   caps+=sizeof(emu10k2_caps)/sizeof(kx_caps);
 }

 // p16v-only settings:
 /*
 if(hw->is_a2 && !hw->is_k8 && !hw->is_edsp)
 if(*sz>=sizeof(p16v_caps))
 {
   memcpy(caps,&p16v_caps,sizeof(p16v_caps));
   *sz-=sizeof(p16v_caps);
   final_sz+=sizeof(p16v_caps);
   caps+=sizeof(p16v_caps)/sizeof(kx_caps);
 }
 */

 // p17v-only settings:
 /*
 if(hw->is_k8 && !hw->is_edsp)
 if(*sz>=sizeof(p17v_caps))
 {
   memcpy(caps,&p17v_caps,sizeof(p17v_caps));
   *sz-=sizeof(p17v_caps);
   final_sz+=sizeof(p17v_caps);
   caps+=sizeof(p17v_caps)/sizeof(kx_caps);
 }
 */

 // E-DSP only settings:
 /*
 if(hw->is_edsp)
 if(*sz>=sizeof(edsp_caps))
 {
   memcpy(caps,&edsp_caps,sizeof(edsp_caps));
   *sz-=sizeof(edsp_caps);
   final_sz+=sizeof(edsp_caps);

   // FIXME NOW 3543 !! change sample rate on the fly!
   caps+=sizeof(edsp_caps)/sizeof(kx_caps);
 }
 */

 *sz=final_sz;
 return 0;
}

// buffer settings

#define DEFAULT_TANKMEM_SIZE        (256*1024)

// std
#define DEFAULT_STD_DMA_BUFFER      (9600) // changed to 9600 for 3553; used to be: (4*2088) // from Win Drivers 2088 (0x828) samples

// rec
#define DEFAULT_STD_REC_BUFFER      (16384)

#define DEFAULT_AC3_BUFFERS     4

KX_API(int,kx_defaults(kx_hw *hw,kx_callbacks *cb))
{
  if(cb==0)
  {
   if(hw) debug(DWDM,"[ResetSettings]\n");
   if(hw)
    cb=&hw->cb;
   else
   {
    if(hw) debug(DWDM,"!!! ResetSettings(NULL) without kx_hw\n");
    return -1;
   }
  } else if(hw) debug(DWDM,"[ResetSettings]\n");

  int def_midi_l=FXBUSC;
  int def_midi_r=FXBUSD;
  int def_reverb=FXBUSA;
  int def_chorus=FXBUSB;

  int def_unused=(hw?(hw->is_10k2?0x3f:0xf):FXBUSF);

  cb->def_routings[DEF_WAVE01_ROUTING]=KX_MAKE_ROUTING(FXBUS0,FXBUS1,def_unused,def_unused);
  cb->def_xroutings[DEF_WAVE01_ROUTING]=KX_MAKE_ROUTING(0x3f,0x3f,0x3f,0x3f);

  cb->def_routings[DEF_WAVE23_ROUTING]=KX_MAKE_ROUTING(FXBUS2,FXBUS3,def_unused,def_unused); // front
  cb->def_xroutings[DEF_WAVE23_ROUTING]=KX_MAKE_ROUTING(0x3f,0x3f,0x3f,0x3f);
  cb->def_routings[DEF_WAVE45_ROUTING]=KX_MAKE_ROUTING(FXBUS4,FXBUS5,def_unused,def_unused); // surround
  cb->def_xroutings[DEF_WAVE45_ROUTING]=KX_MAKE_ROUTING(0x3f,0x3f,0x3f,0x3f);
  cb->def_routings[DEF_WAVE67_ROUTING]=KX_MAKE_ROUTING(FXBUS6,FXBUS7,def_unused,def_unused); // center/lfe
  cb->def_xroutings[DEF_WAVE67_ROUTING]=KX_MAKE_ROUTING(0x3f,0x3f,0x3f,0x3f);

  if(hw)
  {
   // ac3passthru will be initialized on the next ResetSettings call
   if(hw->is_10k2)
   {
    cb->def_routings[DEF_AC3PASSTHROUGH_ROUTING]=KX_MAKE_ROUTING(0x1e,0x1f,0x3f,0x3f);
    cb->def_xroutings[DEF_AC3PASSTHROUGH_ROUTING]=KX_MAKE_ROUTING(0x3f,0x3f,0x3f,0x3f);
   }
   else
   {
    // this is not used [10k1 passthru]
    cb->def_routings[DEF_AC3PASSTHROUGH_ROUTING]=KX_MAKE_ROUTING(FXBUSB,FXBUSC,def_unused,def_unused);
    cb->def_xroutings[DEF_AC3PASSTHROUGH_ROUTING]=KX_MAKE_ROUTING(0x3f,0x3f,0x3f,0x3f);
   }
  }

  for(int i=0;i<16;i++)
  {
    cb->def_routings[DEF_SYNTH1_ROUTING+i]=KX_MAKE_ROUTING(def_midi_l,def_midi_r,def_reverb,def_chorus);
        cb->def_xroutings[DEF_SYNTH1_ROUTING+i]=KX_MAKE_ROUTING(0x3f,0x3f,0x3f,0x3f);
    cb->def_routings[DEF_SYNTH2_ROUTING+i]=KX_MAKE_ROUTING(def_midi_l,def_midi_r,def_reverb,def_chorus);
        cb->def_xroutings[DEF_SYNTH2_ROUTING+i]=KX_MAKE_ROUTING(0x3f,0x3f,0x3f,0x3f);
  }

  cb->def_routings[DEF_AC3_LEFT_ROUTING]=KX_MAKE_ROUTING(FXBUS2,def_unused,def_unused,def_unused);
  cb->def_xroutings[DEF_AC3_LEFT_ROUTING]=KX_MAKE_ROUTING(0x3f,0x3f,0x3f,0x3f);
  cb->def_routings[DEF_AC3_RIGHT_ROUTING]=KX_MAKE_ROUTING(FXBUS3,def_unused,def_unused,def_unused);
  cb->def_xroutings[DEF_AC3_RIGHT_ROUTING]=KX_MAKE_ROUTING(0x3f,0x3f,0x3f,0x3f);

  cb->def_routings[DEF_AC3_SLEFT_ROUTING]=KX_MAKE_ROUTING(FXBUS4,def_unused,def_unused,def_unused);
  cb->def_xroutings[DEF_AC3_SLEFT_ROUTING]=KX_MAKE_ROUTING(0x3f,0x3f,0x3f,0x3f);
  cb->def_routings[DEF_AC3_SRIGHT_ROUTING]=KX_MAKE_ROUTING(FXBUS5,def_unused,def_unused,def_unused);
  cb->def_xroutings[DEF_AC3_SRIGHT_ROUTING]=KX_MAKE_ROUTING(0x3f,0x3f,0x3f,0x3f);

  cb->def_routings[DEF_AC3_CENTER_ROUTING]=KX_MAKE_ROUTING(FXBUS6,def_unused,def_unused,def_unused);
  cb->def_xroutings[DEF_AC3_CENTER_ROUTING]=KX_MAKE_ROUTING(0x3f,0x3f,0x3f,0x3f);
  cb->def_routings[DEF_AC3_SUBWOOFER_ROUTING]=KX_MAKE_ROUTING(FXBUS7,def_unused,def_unused,def_unused);
  cb->def_xroutings[DEF_AC3_SUBWOOFER_ROUTING]=KX_MAKE_ROUTING(0x3f,0x3f,0x3f,0x3f);

  cb->def_routings[DEF_3D_LEFT_ROUTING]=KX_MAKE_ROUTING(FXBUS8,def_unused,def_unused,def_unused);
  cb->def_xroutings[DEF_3D_LEFT_ROUTING]=KX_MAKE_ROUTING(0x3f,0x3f,0x3f,0x3f);
  cb->def_routings[DEF_3D_RIGHT_ROUTING]=KX_MAKE_ROUTING(FXBUS9,def_unused,def_unused,def_unused);
  cb->def_xroutings[DEF_3D_RIGHT_ROUTING]=KX_MAKE_ROUTING(0x3f,0x3f,0x3f,0x3f);

  cb->def_routings[DEF_AC3_SCENTER_ROUTING]=KX_MAKE_ROUTING(FXBUSE,def_unused,def_unused,def_unused);
  cb->def_xroutings[DEF_AC3_SCENTER_ROUTING]=KX_MAKE_ROUTING(0x3f,0x3f,0x3f,0x3f);
  cb->def_routings[DEF_3D_TOP_ROUTING]=KX_MAKE_ROUTING(FXBUSF,FXBUS0,FXBUS0,FXBUS0);
  cb->def_xroutings[DEF_3D_TOP_ROUTING]=KX_MAKE_ROUTING(0x3f,0x3f,0x3f,0x3f);
 
  /*
  cb->def_routings[DEF_3D_BOTTOM_ROUTING]=KX_MAKE_ROUTING(FXBUSF,FXBUS0,def_reverb,def_chorus);
  cb->def_xroutings[DEF_3D_BOTTOM_ROUTING]=KX_MAKE_ROUTING(0x3f,0x3f,0x3f,0x3f);
  */

  for(int i=0;i<MAX_ASIO_OUTPUTS;i++)
  {
   // 3543: was i,i+32
   // 3544: now: 32+i*2[+1]
   if(hw&&hw->is_10k2)
    cb->def_routings[DEF_ASIO_ROUTING+i]=
       KX_MAKE_ROUTING(i*2+32,
                       i*2+32+1,
                       0x3f,
                       0x3f);
   else
    cb->def_routings[DEF_ASIO_ROUTING+i]=
       KX_MAKE_ROUTING(i,
                       ((i==0)?FXBUSF:FXBUS0),
                       (i==0xd)?FXBUSF:FXBUSD,
                       (i==0xe)?FXBUSF:FXBUSE);

   cb->def_xroutings[DEF_ASIO_ROUTING+i]=
     KX_MAKE_ROUTING(0x3f,0x3f,0x3f,0x3f);
  }

  cb->def_amount[DEF_SYNTH_AMOUNT_C]=0xff;
  cb->def_amount[DEF_SYNTH_AMOUNT_D]=0xff;
  cb->def_amount[DEF_SYNTH_AMOUNT_E]=0x0;
  cb->def_amount[DEF_SYNTH_AMOUNT_F]=0x0;
  cb->def_amount[DEF_SYNTH_AMOUNT_G]=0x0;
  cb->def_amount[DEF_SYNTH_AMOUNT_H]=0x0;

  cb->pb_buffer=DEFAULT_STD_DMA_BUFFER;
  cb->rec_buffer=DEFAULT_STD_REC_BUFFER;
  cb->ac3_buffers=DEFAULT_AC3_BUFFERS;

  memset(cb->mixer_controls,0,sizeof(cb->mixer_controls));

  strncpy(cb->mixer_controls[MIXER_MASTER].pgm_name,"epilog",KX_MAX_STRING);
  strncpy(cb->mixer_controls[MIXER_MASTER].reg_left,"MasterL",KX_MAX_STRING);
  strncpy(cb->mixer_controls[MIXER_MASTER].reg_right,"MasterR",KX_MAX_STRING);
  cb->mixer_controls[MIXER_MASTER].max_vol=0x80000000; // 3543

  strncpy(cb->mixer_controls[MIXER_REC].pgm_name,"epilog",KX_MAX_STRING);
  strncpy(cb->mixer_controls[MIXER_REC].reg_left,"RecLVol",KX_MAX_STRING);
  strncpy(cb->mixer_controls[MIXER_REC].reg_right,"RecRVol",KX_MAX_STRING);
  cb->mixer_controls[MIXER_REC].max_vol=0x80000000; // 3543

  cb->mixer_controls[MIXER_KX0].max_vol=0x20000000;
  cb->mixer_controls[MIXER_KX1].max_vol=0x20000000;

  cb->tram_size=DEFAULT_TANKMEM_SIZE;

  return 0;
}
