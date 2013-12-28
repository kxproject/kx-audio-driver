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

void kx_timer_irq_handler(kx_hw *hw)
{
	struct kx_timer *t;
	struct list *item;

	unsigned long flags=0;

    kx_lock_acquire(hw,&hw->timer_lock, &flags);

	for_each_list_entry(item, &hw->timers) 
	{
		t = list_item(item, struct kx_timer, list);
                if(!t)
                 continue;
		if(t->status & TIMER_ACTIVE) 
		{
			t->counter++;
			if(t->counter==t->target) 
			{
				t->counter = 0;
                if(t->timer_func)
                   t->timer_func(t->data,LLA_NOTIFY_TIMER);
			}
		}
	}

	kx_lock_release(hw,&hw->timer_lock,&flags);

	return;
}

void kx_timer_install(kx_hw *hw, struct kx_timer *timer, dword delay)
{
	struct kx_timer *t;
	struct list *item;
	unsigned long flags=0;

	if(delay < 5)
		delay = 5;

    kx_lock_acquire(hw,&hw->timer_lock, &flags);

	timer->delay = delay;
	timer->status = TIMER_INSTALLED;
	timer->target = timer->delay / (hw->timer_delay < 1024 ? hw->timer_delay : 1024);
	timer->counter = timer->target - 1;

	list_add(&timer->list, &hw->timers);

	if(hw->timer_delay > delay) 
	{
		if(hw->timer_delay==TIMER_STOPPED)
			kx_irq_enable(hw,INTE_INTERVALTIMERENB);

		hw->timer_delay = delay;
		delay = (delay < 1024 ? delay : 1024);

		kx_writefn0w(hw, TIMER, (word)delay);

		for_each_list_entry(item, &hw->timers)
		{
			t = list_item(item, struct kx_timer, list);
			if(!t)
			 continue;

			t->target = t->delay / delay;
			// force scheduling on the next interrupt
			t->counter = t->target - 1;
		}
	}

	kx_lock_release(hw,&hw->timer_lock, &flags);

	return;
}

void kx_timer_uninstall(kx_hw *hw, struct kx_timer *timer)
{
	struct kx_timer *t;
	struct list *item;
	dword delay = TIMER_STOPPED;
	unsigned long flags=0;

	if(timer->status==TIMER_UNINSTALLED)
		return;

	kx_lock_acquire(hw,&hw->timer_lock, &flags);

	list_del(&timer->list);

	for_each_list_entry(item, &hw->timers)
	{
		t = list_item(item, struct kx_timer, list);
		if(!t)
		 continue;

		if(t->delay < delay)
			delay = t->delay;
	}

	if(hw->timer_delay != delay)
	{
		hw->timer_delay = delay;

		if(delay==TIMER_STOPPED)
			kx_irq_disable(hw,INTE_INTERVALTIMERENB);
		else {
			delay = (delay < 1024 ? delay : 1024);

			kx_writefn0w(hw, TIMER, (word)delay);

			for_each_list_entry(item, &hw->timers)
			{
				t = list_item(item, struct kx_timer, list);
				if(!t)
				 continue;

				t->target = t->delay / delay;
				t->counter = t->target - 1;
			}
		}
	}

	kx_lock_release(hw,&hw->timer_lock, &flags);

	timer->status = TIMER_UNINSTALLED;

	return;
}

void kx_timer_enable(kx_hw *hw, struct kx_timer *timer)
{
	unsigned long flags=0;

	kx_lock_acquire(hw,&hw->timer_lock, &flags);
	timer->status |= TIMER_ACTIVE;
	kx_lock_release(hw,&hw->timer_lock, &flags);

	return;
}

void kx_timer_disable(kx_hw *hw, struct kx_timer *timer)
{
	unsigned long flags=0;

	kx_lock_acquire(hw,&hw->timer_lock, &flags);
	timer->status &= ~TIMER_ACTIVE;
	kx_lock_release(hw,&hw->timer_lock, &flags);
	return;
}


int kx_timer_init(kx_hw *hw)
{
	init_list(&hw->timers);
	hw->timer_delay = TIMER_STOPPED;
	return 0;
}

int kx_timer_close(kx_hw *hw)
{
	return 0;
}
