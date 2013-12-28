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

void write_mpu_data(kx_hw *hw,byte data,int where);
void write_mpu_data(kx_hw *hw,byte data,int where)
{
	int offset=0;
	if(where)
	 offset=2;

      	if(hw->is_10k2)
      	{
      	  kx_writeptrb(hw,MPUDATA_10K2+offset,0,data);
      	}
      	else
      	{
      	  outp(hw->port + MUDATA_K1, data);
      	}
}

inline byte read_mpu_data(kx_hw *hw,int where)
{
	byte ret;

	int offset=0;
	if(where)
	 offset=2;

      	if(hw->is_10k2)
      	{
      	  ret=kx_readptrb(hw,MPUDATA_10K2+offset,0);
      	}
      	else
      	{
      	  ret=inp(hw->port + MUDATA_K1);
      	}
      	return ret;
}

byte read_mpu_state(kx_hw *hw,int where);
byte read_mpu_state(kx_hw *hw,int where)
{
	int offset=0;
	if(where)
	 offset=2;

	byte state;
	if(hw->is_10k2)
	{
	 state=kx_readptrb(hw,MPUSTAT_10K2+offset,0);
	}
	else
	{
	 state=inp(hw->port + MUSTAT_K1);
	}
	return state;
}

inline void write_mpu_cmd(kx_hw *hw,byte cmd,int where)
{
	int offset=0;
	if(where)
	 offset=2;

      	if(hw->is_10k2)
      	{
      	  kx_writeptrb(hw,MPUCMD_10K2+offset,0,cmd);
      	}
      	else
      	{
      	  outp(hw->port + MUCMD_K1, cmd);
      	}
}


KX_API(int,kx_mpu_write_data(kx_hw *hw, byte data,int offset))
{
   if(hw->uart_out_tail[offset]==hw->uart_out_head[offset])
   {
	if((read_mpu_state(hw,offset) & (byte)MUSTAT_ORDYN) == 0)
	{
		write_mpu_data(hw,data,offset);
	}
	 else  // the device is busy: 
	{
		return -1;
	}
   }	
   else
   {
   	return kx_mpu_write_buffer(hw,offset,&data,1);
   }

   return 0;
}

KX_API(int,kx_mpu_write_buffer(kx_hw *hw,int offset,byte *data,int len,int *actual_len))
{
		int need_init=0;
		int done=0;

		unsigned long flags;
		kx_lock_acquire(hw,&hw->uartout_lock, &flags);

		if(hw->uart_out_head[offset]==hw->uart_out_tail[offset])
		{
		 need_init=1;
		}

		while(done<len)
		{
        		if((hw->uart_out_head[offset]==hw->uart_out_tail[offset]+1) || 
        		   (hw->uart_out_tail[offset]+1-MAX_UART_BUFFER == hw->uart_out_head[offset]))
                        {
                         break;
                        }
                        hw->uart_out_buffer[offset][hw->uart_out_tail[offset]]=*data;
                        data++;
                        done++;
                        hw->uart_out_tail[offset]++;
                        if(hw->uart_out_tail[offset]>=MAX_UART_BUFFER)
                         hw->uart_out_tail[offset]=0;
                }
                kx_lock_release(hw,&hw->uartout_lock, &flags);

		if(need_init) // create a IRQ handler
		{
			if(offset)
			 kx_irq_enable(hw,INTE_K2_MIDITXENABLE);
			else
			 kx_irq_enable(hw,INTE_MIDITXENABLE);
		}

		if(actual_len)
		 *actual_len=done;

		if(done==len)
		 return 0;
		else
		 return -1;
}

KX_API(int,kx_mpu_read_data(kx_hw *hw, byte *data,int offset))
{
	int ret;

        int state=read_mpu_state(hw,offset);

	if((state & MUSTAT_IRDYN) == 0) 
	{
		*data = read_mpu_data(hw,offset);
		ret = 0;
	} else
		ret = -1;

	return ret;
}

int kx_mpu_init(kx_hw *hw)
{
 debug(DLIB,"mpu init\n");

 for(int i=0;i<MAX_MPU_DEVICES;i++)
   init_list(&hw->mpu_buffers[i]);

 // defaults:
 hw->have_mpu=0;
 kx_irq_disable(hw,INTE_MIDIRXENABLE);
 if(hw->is_10k2) 
  kx_irq_disable(hw,INTE_K2_MIDIRXENABLE); 

 if(kx_mpu_reset(hw,0,0))
 {
  hw->have_mpu=0;
  return 1;
 } else hw->have_mpu++;

 if(hw->is_10k2)
 {
  if(kx_mpu_reset(hw,1,0)==0) // ok
  {
   hw->have_mpu++;
  } else debug(DERR,"!!! NB! 10k2 UART2 reset failed\n");
 }

 return 0;
}

int kx_mpu_close(kx_hw *hw)
{
 if(hw->have_mpu)
 {
  for(int i=0;i<hw->have_mpu;i++)
    kx_mpu_reset(hw,i,0);

  kx_irq_disable(hw,INTE_MIDIRXENABLE);
  if(hw->is_10k2)
    kx_irq_disable(hw,INTE_K2_MIDIRXENABLE);
  hw->have_mpu=0;
 }
 return 0;
}

KX_API(int,kx_mpu_reset(kx_hw *hw,int where,int irqs))
{
 if(where>=MAX_MPU_DEVICES)
  return -1;

	byte status;
	
	if(where==0)
	  kx_irq_disable(hw,INTE_MIDIRXENABLE);
	else
	  kx_irq_disable(hw,INTE_K2_MIDIRXENABLE);

	write_mpu_cmd(hw,MUCMD_RESET,where);

	kx_wcwait(hw, 8);

	write_mpu_cmd(hw,MUCMD_RESET,where);

	kx_wcwait(hw, 8);

	write_mpu_cmd(hw,MUCMD_ENTERUARTMODE,where);

	kx_wcwait(hw, 8);

	status = read_mpu_data(hw,where);

	if(status == 0xfe)
	{
	  if(irqs) // enable IRQs only when necessary!
	  {
		if(where==0)
                  kx_irq_enable(hw,INTE_MIDIRXENABLE);
                else
                  kx_irq_enable(hw,INTE_K2_MIDIRXENABLE);
          }
          return 0;
	}
	else
	{
		if(where==0)
                  kx_irq_disable(hw,INTE_MIDIRXENABLE); // disable, if not found
                else
                  kx_irq_disable(hw,INTE_K2_MIDIRXENABLE); // disable, if not found
		return -1;
	}
}

KX_API(int,kx_mpu_read_buffer_open(kx_hw *hw,void **inst,int where)) // returns instance & 0 if ok
{
 if(where>=MAX_MPU_DEVICES)
  return -1;
 if(hw->have_mpu)
 {
    kx_mpu_buffer *new_b;
    (hw->cb.malloc_func)(hw->cb.call_with,sizeof(kx_mpu_buffer),(void **)&new_b,KX_NONPAGED);
    if(new_b==0)
    {
     *inst=0;
     return -1;
    }
    memset(new_b,0,sizeof(kx_mpu_buffer));
    new_b->instance=new_b;
    *inst=new_b;

    unsigned long flags=0;
    kx_lock_acquire(hw,&hw->mpu_lock[where], &flags);

     // reset MIDI and enable corresponding IRQ if necessary [first open]
     if(hw->mpu_buffers[where].next==(&hw->mpu_buffers[where]))
     {
//      debug(DLIB,"mpu: reset+irq [%d] - enabled\n",where);
      kx_mpu_reset(hw,where,1);
     }
     
     list_add(&new_b->list, &hw->mpu_buffers[where]);

    kx_lock_release(hw,&hw->mpu_lock[where], &flags);
    return 0;
 } else return -1;
}

KX_API(int,kx_mpu_read_buffer_close(kx_hw *hw,void *inst,int where))
{
 if(where>=MAX_MPU_DEVICES)
  return -1;

 if(hw->have_mpu)
 {
  struct list *item;

  unsigned long flags=0;
  kx_lock_acquire(hw,&hw->mpu_lock[where], &flags);

  for_each_list_entry(item, &hw->mpu_buffers[where])
  {
  	kx_mpu_buffer *b;
        b = list_item(item, kx_mpu_buffer, list);
        if(!b || b->instance!=inst)
         continue;

        // found
        list_del(&b->list);

	// disable corresponding IRQ if necessary
        if(hw->mpu_buffers[where].next==(&hw->mpu_buffers[where]))
        {
//         debug(DLIB,"mpu: reset+irq [%d] - disabled\n",where);
         kx_mpu_reset(hw,where,0);
        }

        kx_lock_release(hw,&hw->mpu_lock[where], &flags);
        (hw->cb.free_func)(hw->cb.call_with,b);
        return 0;
  }
  debug(DERR,"!!! (internal) wrong instance in MPU read buffer close()\n");
  kx_lock_release(hw,&hw->mpu_lock[where], &flags);
  return -2;
 }
 return -3;
}

KX_API(int,kx_mpu_read_buffer(kx_hw *hw, void *instance,byte *data,int where))
{
 if(where>=MAX_MPU_DEVICES)
  return -1;

 if(hw->have_mpu)
 {
  struct list *item;

  unsigned long flags=0;
  kx_lock_acquire(hw,&hw->mpu_lock[where], &flags);

  for_each_list_entry(item, &hw->mpu_buffers[where])
  {
  	kx_mpu_buffer *b;
        b = list_item(item, kx_mpu_buffer, list);
        if(!b || b->instance!=instance)
         continue;
  
        if(b->mpu_head==b->mpu_tail)
        {
          kx_lock_release(hw,&hw->mpu_lock[where], &flags);
          return -1;
        }

        *data=b->mpu_buffer[b->mpu_head];
        b->mpu_head++;
        if(b->mpu_head>=MAX_MPU_BUFFER)
         b->mpu_head=0;
        kx_lock_release(hw,&hw->mpu_lock[where], &flags);
        return 0;
  }
  debug(DERR,"!!! (internal error): unknown instance for UART (reading)\n");
  kx_lock_release(hw,&hw->mpu_lock[where], &flags);
  return 0;
 } else return -2;
}
