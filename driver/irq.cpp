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

// #define CE_OPTIMIZE

void kx_timer_irq_handler(kx_hw *hw);
void system_timer_func(void *data,int what);

#if defined(_MSC_VER)
#pragma code_seg()
#endif
#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif

#if defined(_MSC_VER)
#pragma code_seg()
#endif
dword kx_adjust_position(kx_hw *hw,dword qkbca,dword semi_buf);
dword kx_adjust_position(kx_hw *hw,dword qkbca,dword semi_buf)
{
  // position is usually 28 or 32 samples behind, due to PCI caching
#if 1
  if(hw->is_10k2)
    qkbca+=28;
  else
    qkbca+=32;

  if(qkbca>=semi_buf*2)
   qkbca-=semi_buf*2;
#endif
  return qkbca;
}

#if defined(_MSC_VER)
#pragma code_seg()
#endif
#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif

static inline int kx_record_irq_critical_handler(kx_hw *hw,int where)
{
    // this should be @ DIRQl

    // where: full_buff:1, half_buff:0
    // will fill-in buffer: 1; 0
    // will use hardware buffer: 1; 0
    // actual hardware position: 0; 1
    // will set rec_buf: 0; 1 (current hardware position)

    if(!hw->can_passthru) // 16-bit
    {
            size_t user_size=0;

            // get number of outputs
            word *outputs_[MAX_ASIO_INPUTS+1];
            int i;
            int cnt=0;
            for(i=0;i<MAX_ASIO_INPUTS;i++)
            {
             if(hw->asio_inputs[i].kernel_addr)
             {
              outputs_[cnt]=(word *)hw->asio_inputs[i].kernel_addr;
              if(where) 
                outputs_[cnt]+=(hw->asio_inputs[i].size>>2); // /2; /2 because of 'word'
              user_size=hw->asio_inputs[i].size;
              cnt++;
             }
            }

            outputs_[cnt]=0;
            word *input=(word *)hw->mtr_buffer.addr;

            user_size>>=2;
            if(where)
             input+=user_size*cnt; 

            word **outputs=outputs_;
            while(*outputs) 
            {
             register int ii;
             register word *out;

             out = *outputs;
             for(ii = (int)user_size; --ii >= 0; )
              *(out + ii) = *(input + ii*cnt);
             outputs++;
             input++;
            }
    }
     else // can_passthru:
    {
         if(hw->cur_asio_in_bps!=32) // 16-bit in 32-bit
         {
            size_t user_size=0;

            // get number of outputs
            word *outputs_[MAX_ASIO_INPUTS+1];
            int i;
            int cnt=0;
            for(i=0;i<MAX_ASIO_INPUTS;i++)
            {
             if(hw->asio_inputs[i].kernel_addr)
             {
              outputs_[cnt]=(word *)hw->asio_inputs[i].kernel_addr;
              if(where) 
                outputs_[cnt]+=(hw->asio_inputs[i].size>>2); // /2; /2 because of 'word'
              user_size=hw->asio_inputs[i].size;
              cnt++;
             }
            }

            outputs_[cnt]=0;
            word *input=(word *)hw->mtr_buffer.addr;

            cnt<<=1; // multiple *2

            user_size>>=2; // /2 /2 because of 'word'
            if(where)
             input+=user_size*cnt; // input is 'word', but buffer is 'dword'; cnt is already <<=1

            input++; // skip first LSB

            word **outputs=outputs_;
            while(*outputs) 
            {
             register int ii;
             register word *out;

             out = *outputs;
             for(ii = (int)user_size; --ii >= 0; )
              *(out + ii) = *(input + ii*cnt); // cnt is already <<=1
             outputs++;
             input+=2; // skip next LSB
            }
         }
         else // true 32-bit in 32-bit
         {
            size_t user_size=0;

            // get number of outputs
            dword *outputs_[MAX_ASIO_INPUTS+1];
            int i;
            int cnt=0;
            for(i=0;i<MAX_ASIO_INPUTS;i++)
            {
             if(hw->asio_inputs[i].kernel_addr)
             {
              outputs_[cnt]=(dword *)hw->asio_inputs[i].kernel_addr;
              if(where) 
                outputs_[cnt]+=(hw->asio_inputs[i].size>>3); // /4 /2 because of 'dword'
              user_size=hw->asio_inputs[i].size;
              cnt++;
             }
            }

            outputs_[cnt]=0;
            dword *input=(dword *)hw->mtr_buffer.addr;

            user_size>>=3; // /2 /4
            if(where)
             input+=user_size*cnt;

            dword **outputs=outputs_;
            while(*outputs) 
            {
             register int ii;
             register dword *out;

             out = *outputs;
             for(ii = (int)user_size; --ii >= 0; )
              *(out + ii) = *(input + ii*cnt);
             outputs++;
             input++;
            }
         }
    }

    hw->asio_notification_krnl.rec_buf=1-where; // rec_buf: full_buf: 0, half_buf: 1

    //debug(DLIB,"IRQ: [rec] where=%d in=%x out=%x phys=%x\n",where,input,outputs[0],kx_readptr(hw,FXIDX,0));

    int old_toggle=hw->asio_notification_krnl.toggle;
    hw->asio_notification_krnl.toggle=kx_get_asio_position(hw,0); // do not read it again

    if((hw->asio_notification_krnl.asio_method&KXASIO_METHOD_SEND_EVENT) && hw->asio_notification_krnl.toggle!=-1 &&
       (old_toggle!=hw->asio_notification_krnl.toggle))
     return 1; // need additional processing

    return 0; // processed
}

#if defined(_MSC_VER)
#pragma code_seg()
#endif
static inline int kx_record_irq_dispatch_handler(kx_hw *hw,int where)
{
 if(hw->asio_notification_krnl.asio_method&KXASIO_METHOD_SEND_EVENT)
 {
  if(hw->asio_notification_krnl.kevent)
   hw->cb.notify_func(hw->asio_notification_krnl.kevent,LLA_NOTIFY_EVENT);
 }

 return 0;
}

#if defined(_MSC_VER)
#pragma code_seg()
#endif
#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif

static inline int kx_voice_irq_critical_handler(kx_hw *hw)
{
 // should distinguish between half/full event
 // set pb_buf=0/1

 if(!hw->hw_lock.kx_lock)
 {
     dword qkbca;
     dword ptr_reg=(QKBCA<<16)|(hw->asio_notification_krnl.n_voice);

     dword old_ptr=inpd(hw->port+PTR);
     outpd(hw->port+PTR,ptr_reg);
     qkbca=inpd(hw->port+DATA);
     outpd(hw->port+PTR,old_ptr);

     qkbca=kx_calc_position(hw,hw->asio_notification_krnl.n_voice,(qkbca&QKBCA_CURRADDR_MASK));
     hw->asio_notification_krnl.cur_pos=qkbca;

     // for comparison ONLY
     qkbca=kx_adjust_position(hw,qkbca,hw->asio_notification_krnl.semi_buff);
     
     // fullbuff: 0; halfbuf: 1

     if(qkbca>=hw->asio_notification_krnl.semi_buff)
              hw->asio_notification_krnl.pb_buf=1; // current hardware position
     else
              hw->asio_notification_krnl.pb_buf=0;
 }
 else // lock is acquired; someone is accessing PTR
 {
   debug(DLIB,"note: mutual ptr access\n");

   if(hw->asio_notification_krnl.pb_buf!=-1)
   {
       hw->asio_notification_krnl.pb_buf=1-hw->asio_notification_krnl.pb_buf;
       
       if(hw->asio_notification_krnl.pb_buf==1)
           hw->asio_notification_krnl.cur_pos=hw->asio_notification_krnl.semi_buff;
       else
           hw->asio_notification_krnl.cur_pos=0;
   }
   else
    hw->asio_notification_krnl.pb_buf=1;
 }

 int old_toggle=hw->asio_notification_krnl.toggle;
 hw->asio_notification_krnl.toggle=kx_get_asio_position(hw,0); // do not read it again

 if((hw->asio_notification_krnl.asio_method&KXASIO_METHOD_SEND_EVENT) && hw->asio_notification_krnl.toggle!=-1 &&
   old_toggle!=hw->asio_notification_krnl.toggle)
  return 1; // need additional processing

 return 0; // processed
}

#if defined(_MSC_VER)
#pragma code_seg()
#endif
static inline int kx_voice_irq_dispatch_handler(kx_hw *hw)
{
    if(hw->asio_notification_krnl.asio_method&KXASIO_METHOD_SEND_EVENT)
    {
      if(hw->asio_notification_krnl.kevent)
       hw->cb.notify_func(hw->asio_notification_krnl.kevent,LLA_NOTIFY_EVENT);
    }

    return 0;
}

#if defined(_MSC_VER)
#pragma code_seg()
#endif
#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif

void kx_mpuin_irq_handler(kx_hw *hw,int where);
void kx_mpuin_irq_handler(kx_hw *hw,int where)
{
 byte val;
 unsigned long flags=0;
 if(hw->have_mpu==0)
  debug(DLIB,"!!! mpu=0, but MPU interrupt came!\n");

 // MPU interrupts disabled in adapter's KX_SYNC_IPR_IRQ handler

    while(kx_mpu_read_data(hw,&val,where)!=-1)
    {
//     debug(DNONE,"[%02d] ",val);

     kx_lock_acquire(hw,&hw->mpu_lock[where], &flags);

     struct list *item;

     for_each_list_entry(item, &hw->mpu_buffers[where])
     {
        kx_mpu_buffer *b;
           b = list_item(item, kx_mpu_buffer, list);
           if(!b)
            continue;

           if((b->mpu_head==b->mpu_tail+1)||(b->mpu_tail+1-MAX_MPU_BUFFER == b->mpu_head))
           {
               // buffer overflow: nothing to do :(
               debug(DLIB,"!!! mpu-in buffer overflow!\n");
               break;
           }
           else
           {
            b->mpu_buffer[b->mpu_tail]=val;
            b->mpu_tail++;
            if(b->mpu_tail>=MAX_MPU_BUFFER)
             b->mpu_tail=0;
           }
     }

     kx_lock_release(hw,&hw->mpu_lock[where], &flags);
    }
//    debug(DLIB,"\n");

    // re-enable MPUIN interrupt (INTE)
    sync_data s;
    s.hw=hw;
    s.what=KX_SYNC_MPUIN;
    s.ret=where;

    hw->cb.sync(hw->cb.call_with,&s);
}

#if defined(_MSC_VER)
#pragma code_seg()
#endif
#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif


void kx_mpuout_irq_handler(kx_hw *hw,int where);
void kx_mpuout_irq_handler(kx_hw *hw,int where)
{
    unsigned long flags;

    extern void write_mpu_data(kx_hw *hw,byte data,int where);
    extern byte read_mpu_state(kx_hw *hw,int where);

    // MPU interrupts disabled in adapter's KX_SYNC_IPR_IRQ handler

    kx_lock_acquire(hw,&hw->uartout_lock, &flags);

    int turn_on=1;

    while(1)
    {
       if(hw->uart_out_tail[where]==hw->uart_out_head[where])
       {
        turn_on=0; // already done in adapter.cpp/IRQ handler; so, don't turn on

        break;
       }
       else
       {
    if((read_mpu_state(hw,where) & (byte)MUSTAT_ORDYN) == 0)
    {
          write_mpu_data(hw,hw->uart_out_buffer[where][hw->uart_out_head[where]],where);
        }
    else
    {
      break;
        }

        hw->uart_out_head[where]++;
        if(hw->uart_out_head[where]>=MAX_UART_BUFFER)
            hw->uart_out_head[where]=0;
       }
    }

    kx_lock_release(hw,&hw->uartout_lock, &flags);

    sync_data s;
    s.hw=hw;
    s.what=KX_SYNC_MPUOUT; // re-enable MPUOUT interrupt (INTE)
    s.ret=where;
    s.turn_on=turn_on;

    hw->cb.sync(hw->cb.call_with,&s);
}

#if defined(_MSC_VER)
#pragma code_seg()
// this is ALSO required, since we call send_message() here
#endif
void kx_spdif_irq_handler(kx_hw *hw);
void kx_spdif_irq_handler(kx_hw *hw)
{
 hw->cb.send_message(hw->cb.call_with,KX_SYSEX_SIZE,KX_SYSEX_SPDIF);

 debug(DLIB,"SPDIF irq [%x/%s]\n",hw->port,hw->card_name);
}

#if defined(_MSC_VER)
#pragma code_seg()
#endif
void kx_dsp_irq_handler(kx_hw *hw)
{
 if(hw->ac3_pt_state.method==KX_AC3_PASSTHRU_XTRAM)
 {
  if(hw->ac3_pt_state.callback)
   hw->ac3_pt_state.callback((void *)hw->ac3_pt_state.instance);
 }
 else
  debug(DLIB,"!! invalid DSP irq [unexpected]!\n");
}

#if defined(_MSC_VER)
#pragma code_seg()
#endif
#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif

inline KX_API(dword,kx_get_irq_pending(kx_hw *hw))
{
 if(hw->standalone)
  return inpd(hw->port + IPR);
 else
  return hw->irq_pending;
}

#if defined(_MSC_VER)
#pragma code_seg()
#endif
#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif

inline KX_API(void,kx_clear_irq_pending(kx_hw *hw,dword pat))
{
 if(hw->standalone)
 {
  outpd(hw->port + IPR,pat);
 }
 else
 { 
    sync_data s;
    s.what=KX_SYNC_CLEAR_IPR;
    s.hw=hw;
    s.irq_mask=pat;

    hw->cb.sync(hw->cb.call_with,&s);
 }
}

#if defined(_MSC_VER)
#pragma code_seg()
#endif
#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif

inline KX_API(int,kx_interrupt_critical(kx_hw *hw))
{
    // don't use kx_get_irq_pending() and kx_clear_irq_pending()
    dword ipr=inpd(hw->port + IPR);

    if(ipr==0)
     return -1; // not our IRQ

    // don't use kx_get_irq_pending() and kx_clear_irq_pending()
    outpd(hw->port + IPR,ipr); // clear IRQ

    hw->irq_pending|=ipr;

    if(hw->irq_pending&IPR_EFXBUFFULL)
    {
      if(!kx_record_irq_critical_handler(hw,1))
       hw->irq_pending&=(~IPR_EFXBUFFULL);
    }
    if(hw->irq_pending&IPR_EFXBUFHALFFULL)
    {
      if(!kx_record_irq_critical_handler(hw,0))
       hw->irq_pending&=(~IPR_EFXBUFHALFFULL);
    }
    if(hw->irq_pending&IRQ_VOICE)
    {
      if(!kx_voice_irq_critical_handler(hw))
       hw->irq_pending&=(~IRQ_VOICE);
    }

     // we should clear the particular INTE bit to calm down the chip...
     // we have some time to process the DPC (depending on the FIFO size)

     dword mask=0;

     if(hw->irq_pending&IRQ_MPUIN)
      mask|=INTE_MIDIRXENABLE;
     if(hw->irq_pending&IRQ_MPUOUT)
      mask|=INTE_MIDITXENABLE;
     if(hw->irq_pending&IRQ_MPUIN2)
      mask|=INTE_K2_MIDIRXENABLE;
     if(hw->irq_pending&IRQ_MPUOUT2)
      mask|=INTE_K2_MIDITXENABLE;

      if(mask)
       outpd(hw->port+INTE,inpd(hw->port + INTE)&(~mask));

     return 0;
}

// the following operations -must- always be synchronized
// in Windows this is done in ISR context (DIRQ) or with KeSynchronizeExecution

#if defined(_MSC_VER)
#pragma code_seg()
#endif
#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif

void kx_sync(sync_data *s)
{
 kx_hw *hw=s->hw;

 switch(s->what)
 {
  case KX_SYNC_IPR_IRQ:
       {
          s->ret=kx_interrupt_critical(hw); // it will clear ipr bits already processed
          s->irq_mask=hw->irq_pending;
       }
       break;
  case KX_SYNC_CLEAR_IPR:
        hw->irq_pending&=(~s->irq_mask);
        break;
  case KX_SYNC_IRQ_ENABLE:
        {
         dword val = inpd(hw->port + INTE) | s->irq_mask;
         outpd(hw->port + INTE,val);

         break;
        }
  case KX_SYNC_IRQ_DISABLE:
        {
         dword val = inpd(hw->port + INTE) & (~s->irq_mask);
         outpd(hw->port + INTE,val);

         break;
        }
  case KX_SYNC_MPUIN:
        {
           // re-enable the interrupt (disabled in kx_interrupt_critical handler)
           dword old_inte=inpd(hw->port+INTE);

           if(s->ret)
           {
            if(old_inte&INTE_K2_MIDIRXENABLE)
             debug(DERR,"!!! mpu_in2 IRQ was enabled in irq handler\n");

            outpd(hw->port + INTE,old_inte | INTE_K2_MIDIRXENABLE);

            // kx_clear_irq_pending(hw,IRQ_MPUIN2);
            hw->irq_pending&=(~IRQ_MPUIN2);
           }
           else
           {
            if(old_inte&INTE_MIDIRXENABLE)
             debug(DERR,"!!! mpu_in IRQ was enabled in irq handler\n");

            outpd(hw->port + INTE,old_inte | INTE_MIDIRXENABLE);

            // kx_clear_irq_pending(hw,IRQ_MPUIN);
            hw->irq_pending&=(~IRQ_MPUIN);
           }

           break;
        }
  case KX_SYNC_MPUOUT:
        {
                dword old_inte=inpd(hw->port+INTE);

                if(s->ret)
                {
                  if(old_inte&INTE_K2_MIDITXENABLE)
                   debug(DERR,"!!! mpu_out2 was enabled in the irq handler\n");
                }
                else
                {
                  if(old_inte&INTE_MIDITXENABLE)
                   debug(DERR,"!!! mpu_out was enabled in the irq handler\n");
                }

                if(s->turn_on)
                {
                 // turn IRQs ON:
                 if(s->ret)
                 {
                  outpd(hw->port + INTE,old_inte | INTE_K2_MIDITXENABLE);
                  // kx_clear_irq_pending(hw,IRQ_MPUOUT2);
                  hw->irq_pending&=(~IRQ_MPUOUT2);
                 }
                 else
                 {
                  outpd(hw->port + INTE,old_inte | INTE_MIDITXENABLE);
                  // kx_clear_irq_pending(hw,IRQ_MPUOUT);
                  hw->irq_pending&=(~IRQ_MPUOUT);
                 }
                }
                else
                {
                 if(s->ret)
                  hw->irq_pending&=(~IRQ_MPUOUT2); // kx_clear_irq_pending(hw,IRQ_MPUOUT2);
                 else
                  hw->irq_pending&=(~IRQ_MPUOUT); // kx_clear_irq_pending(hw,IRQ_MPUOUT);
                }

        break;
        }
  default:
    debug(DERR,"!! fatal error: invalid sync opcode! [%x]\n",s->what);
    break;
 }
}


// returns 1 if irq parsed; 0- if not emu10kx
// data - any value
#if defined(_MSC_VER)
#pragma code_seg()
// this is ALSO required, since we call send_message() here
#endif
#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif

KX_API(int,kx_interrupt_deferred(kx_hw *hw))
{
    dword irq_pending;

    irq_pending=kx_get_irq_pending(hw);

    if(irq_pending==0)
    {
        return KX_IRQ_NONE;
    }

    int ret=0;

    if(irq_pending&IRQ_TIMER) 
    {
        // acknowledge interrupt
        kx_clear_irq_pending(hw,  IRQ_TIMER);
        kx_timer_irq_handler(hw);
        irq_pending&=~IRQ_TIMER;
    }

/*
    // never used:

    if((irq_pending&IPR_ADCBUFFULL)||(irq_pending&IPR_ADCBUFHALFFULL))
    {
         // acknowledge interrupt
         kx_clear_irq_pending(hw,  IPR_ADCBUFFULL|IPR_ADCBUFHALFFULL);

         debug(DERR,"adcrec irq should not happen\n");
         if(hw->cb.timer_func &&
           voice_usage(hw->voicetable[KX_REC_GENERIC].usage)==VOICE_USAGE_PLAYBACK)
            hw->cb.timer_func(hw->voicetable[KX_REC_GENERIC].buffer.that);

         irq_pending&=~(IPR_ADCBUFFULL|IPR_ADCBUFHALFFULL);
    }

    if((irq_pending&IPR_SPDIFBUFHALFFUL_K2)||(irq_pending&IPR_SPDIFBUFFUL_K2))
    {
         // acknowledge interrupt
         kx_clear_irq_pending(hw,  IPR_SPDIFBUFHALFFUL_K2|IPR_SPDIFBUFFUL_K2);

         debug(DERR,"spdif_rec irq should not happen\n");
         if(hw->cb.timer_func &&
           voice_usage(hw->voicetable[KX_REC_SPDIF].usage)==VOICE_USAGE_PLAYBACK)
            hw->cb.timer_func(hw->voicetable[KX_REC_SPDIF].buffer.that);

         irq_pending&=~(IPR_SPDIFBUFHALFFUL_K2|IPR_SPDIFBUFFUL_K2);
    }
*/

    if(irq_pending&IRQ_MPUIN)
    {
        // acknowledge interrupt
        // kx_clear_irq_pending(hw,   IRQ_MPUIN);
        // (should be done in sync code)

        kx_mpuin_irq_handler(hw,0);

        irq_pending&=~IRQ_MPUIN;
        ret |=KX_IRQ_MPUIN; // more processing required
    }

    if(irq_pending&IRQ_MPUIN2)
    {
        // acknowledge interrupt
        // kx_clear_irq_pending(hw,  IRQ_MPUIN2);
        // (should be done in sync code)

        kx_mpuin_irq_handler(hw,1);

        irq_pending&=~IRQ_MPUIN2;
        ret |=KX_IRQ_MPUIN2; // more processing required
    }

    if(irq_pending&IRQ_MPUOUT)
    {
        // kx_clear_irq_pending(hw,  IRQ_MPUOUT);
        // (should be done in sync code)

        kx_mpuout_irq_handler(hw,0);

        irq_pending&=~IRQ_MPUOUT;

        // 3537rc1: we don't need this
        // ret |=KX_IRQ_MPUOUT;
    }

    if(irq_pending&IRQ_MPUOUT2)
    {
        // kx_clear_irq_pending(hw,  IRQ_MPUOUT2);
        // (should be done in sync code)

        kx_mpuout_irq_handler(hw,1);

        irq_pending&=~IRQ_MPUOUT2;

        // 3537rc1: we don't need this
        // ret |=KX_IRQ_MPUOUT2;
    }

    if(irq_pending&IRQ_PCIBUSERROR) 
    {
        // acknowledge interrupt
        kx_clear_irq_pending(hw,  IRQ_PCIBUSERROR);

        debug(DERR,"!!! PCI bus error found - and PCI IRQ is disabled now!\n");
        hw->cb.send_message(hw->cb.call_with,KX_SYSEX_SIZE,KX_SYSEX_PCIBUSERROR);
        kx_irq_disable(hw,INTE_PCIERRORENABLE);

        irq_pending&=~IRQ_PCIBUSERROR;
    }

    if(irq_pending&IRQ_DSP) 
    {
        // acknowledge interrupt
        kx_clear_irq_pending(hw,  IRQ_DSP);

        kx_dsp_irq_handler(hw);
        irq_pending&=~IRQ_DSP;
    }

    if(irq_pending&IPR_MUTE) 
    {
        // acknowledge interrupt
        kx_clear_irq_pending(hw,  IPR_MUTE);

        if(hw->cb.send_message)
        {
             debug(DLIB," -- h/w mute\n");
             hw->cb.send_message(hw->cb.call_with,KX_SYSEX_REMOTE_SIZE,KX_SYSEX_VOLMUTE);
        }

        irq_pending&=~IPR_MUTE;

        // 3538l: we don't need this
        // ret|=KX_IRQ_VOLUMES_MUTE;
    }

    if(irq_pending&IPR_VOLINCR) 
    {
        // acknowledge interrupt
        kx_clear_irq_pending(hw,  IPR_VOLINCR);

        if(hw->cb.send_message)
        {
             debug(DLIB," -- h/w volincr\n");
             hw->cb.send_message(hw->cb.call_with,KX_SYSEX_REMOTE_SIZE,KX_SYSEX_VOLINCR);
        }

        irq_pending&=~IPR_VOLINCR;

        // 3538l: we don't need this
        // ret|=KX_IRQ_VOLUMES_VOLINCR;
    }

    if(irq_pending&IPR_VOLDECR) 
    {
        // acknowledge interrupt
        kx_clear_irq_pending(hw,  IPR_VOLDECR);

        if(hw->cb.send_message)
        {
             debug(DLIB," -- h/w voldecr\n");
             hw->cb.send_message(hw->cb.call_with,KX_SYSEX_REMOTE_SIZE,KX_SYSEX_VOLDECR);
        }

        irq_pending&=~IPR_VOLDECR;

        // 3538l: we don't need this
        // ret|=KX_IRQ_VOLUMES_VOLDECR;
    }
    if(irq_pending&IRQ_GPIO)
    {
        kx_clear_irq_pending(hw, IPR_GPIO_CHANGE);
        system_timer_func(hw,LLA_NOTIFY_SYSTEM);
        irq_pending&=~IPR_GPIO_CHANGE;
    }

    if(irq_pending&IRQ_SPDIF)
    {
        // acknowledge interrupt
        kx_clear_irq_pending(hw,  IRQ_SPDIF);
        kx_spdif_irq_handler(hw);
        irq_pending&=~IRQ_SPDIF;
    }

    if(irq_pending&IRQ_VOICE)
    {
        // acknowledge interrupt
        kx_clear_irq_pending(hw,  IRQ_VOICE);
        kx_voice_irq_dispatch_handler(hw);
        irq_pending&=~IRQ_VOICE;

        // 3538l: we don't need this
        // ret|=KX_IRQ_VOICE;
    }

    if(irq_pending&IPR_EFXBUFFULL)
    {
         kx_clear_irq_pending(hw,IPR_EFXBUFFULL);
         kx_record_irq_dispatch_handler(hw,1);
         irq_pending&=(~IPR_EFXBUFFULL);
    }

    if(irq_pending&IPR_EFXBUFHALFFULL)
    {
         kx_clear_irq_pending(hw,IPR_EFXBUFHALFFULL);
         kx_record_irq_dispatch_handler(hw,0);
         irq_pending&=(~IPR_EFXBUFHALFFULL);
    }

    if(irq_pending)
    {
        debug(DERR,"!!! Unhandled IRQ: %x\n",irq_pending);
        // ret |=KX_IRQ_UNKNOWN;
        // kx_clear_irq_pending(hw,   irq_pending);
        // (not necessary since not a real h/w access)
    }

    return ret;
}

#if defined(_MSC_VER)
#pragma code_seg()
#endif
#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif

KX_API(void, kx_irq_enable(kx_hw *hw, dword irq_mask))
{
    sync_data s;
    s.what=KX_SYNC_IRQ_ENABLE;
    s.hw=hw;
    s.irq_mask=irq_mask;

    hw->cb.sync(hw->cb.call_with,&s);
}

#if defined(_MSC_VER)
#pragma code_seg()
#endif
#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif

KX_API(void, kx_irq_disable(kx_hw *hw, dword irq_mask))
{
    sync_data s;
    s.what=KX_SYNC_IRQ_DISABLE;
    s.hw=hw;
    s.irq_mask=irq_mask;

    hw->cb.sync(hw->cb.call_with,&s);
}

#if defined(_MSC_VER)
#pragma code_seg()
#endif
KX_API(void, kx_voice_stop_on_loop_enable(kx_hw *hw, dword voicenum))
{
    if(voicenum >= 32)
        kx_writeptr(hw, SOLH | ((0x0100 | (voicenum - 32)) << 16), 0, 1);
    else
        kx_writeptr(hw, SOLL | ((0x0100 | voicenum) << 16), 0, 1);
}

#if defined(_MSC_VER)
#pragma code_seg()
#endif
KX_API(void, kx_voice_stop_on_loop_disable(kx_hw *hw, dword voicenum))
{
    if(voicenum >= 32)
        kx_writeptr(hw, SOLH | ((0x0100 | (voicenum - 32)) << 16), 0, 0);
    else
        kx_writeptr(hw, SOLL | ((0x0100 | voicenum) << 16), 0, 0);
}

#if defined(_MSC_VER)
#pragma code_seg()
#endif
KX_API(void, kx_voice_irq_enable(kx_hw *hw, dword voicenum))
{
    if(voicenum >= 32)
        kx_writeptr(hw, CLIEH | ((0x0100 | (voicenum - 32)) << 16), 0, 1);
    else
        kx_writeptr(hw, CLIEL | ((0x0100 | voicenum) << 16), 0, 1);
    
    if(hw->is_10k2)
    {
        if(voicenum >= 32)
            kx_writeptr(hw, HLIEH | ((0x0100 | (voicenum - 32)) << 16), 0, 1);
        else
            kx_writeptr(hw, HLIEL | ((0x0100 | voicenum) << 16), 0, 1);
    }
}

#if defined(_MSC_VER)
#pragma code_seg()
#endif
KX_API(void, kx_voice_irq_disable(kx_hw *hw, dword voicenum))
{
    if(voicenum >= 32)
        kx_writeptr(hw, CLIEH | ((0x0100 | (voicenum - 32)) << 16), 0, 0);
    else
        kx_writeptr(hw, CLIEL | ((0x0100 | voicenum) << 16), 0, 0);
    if(hw->is_10k2)
    {
        if(voicenum >= 32)
            kx_writeptr(hw, HLIEH | ((0x0100 | (voicenum - 32)) << 16), 0, 0);
        else
            kx_writeptr(hw, HLIEL | ((0x0100 | voicenum) << 16), 0, 0);
    }
}
/*
#pragma code_seg()
KX_API(void, kx_voice_irq_ack(kx_hw *hw, dword voicenum))
{
    if(voicenum >= 32)
        kx_writeptr(hw, CLIPH | ((0x0100 | (voicenum - 32)) << 16), 0, 0);
    else
        kx_writeptr(hw, CLIPL | ((0x0100 | voicenum) << 16), 0, 0);
}
*/

#if defined(_MSC_VER)
#pragma code_seg()
#endif
#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif

KX_API(int,kx_get_asio_position(kx_hw *hw,int reget))
{
 asio_notification_t *krnl=&hw->asio_notification_krnl;

 if(krnl->n_voice!=-1 && reget) // note: when running at raised IRQl, set reget=0
 {
   dword qkbca=0; // =kx_calc_position(hw,krnl->n_voice,(kx_readptr(hw,QKBCA,krnl->n_voice)&QKBCA_CURRADDR_MASK));
     
   if(hw->hw_lock.kx_lock)
   {
       debug(DLIB,"kx_get_asio_position: shared access\n");
   }
   else
   {
#if defined(__APPLE__) && defined(__MACH__) // MacOSX
   // in OS X we cannot acquire spinlocks here, or at least should not to
   dword ptr_reg=(QKBCA<<16)|(hw->asio_notification_krnl.n_voice);
   dword old_ptr=inpd(hw->port+PTR);
   outpd(hw->port+PTR,ptr_reg);
   qkbca=inpd(hw->port+DATA);
   outpd(hw->port+PTR,old_ptr);
#elif defined(_WIN32) || defined(_WINDOWS) || defined(WIN32)
   qkbca = kx_readptr(hw,QKBCA,krnl->n_voice)&QKBCA_CURRADDR_MASK;
#else
    #error Unsupported architecture
#endif

   qkbca=kx_calc_position(hw,krnl->n_voice,qkbca);
   // adjust for 28/32 sample delay only in the IRQ handler.. and for comparison only
   // qkbca=kx_adjust_position(hw,qkbca,hw->asio_notification_krnl.semi_buff);

   krnl->cur_pos=qkbca; // -hw->voicetable[hw->asio_notification_krnl.n_voice].param.startloop; // 3545b
     
   }
     
   if(krnl->cur_pos>=krnl->semi_buff)
        krnl->pb_buf=1;
   else
        krnl->pb_buf=0;
 }
 if(krnl->rec_buf==-1) // pb only case
  return krnl->pb_buf;

 if(krnl->pb_buf==-1) // rec only case
  return krnl->rec_buf;

 if(krnl->rec_buf==krnl->pb_buf)
  return krnl->pb_buf;

 return -1;
}
