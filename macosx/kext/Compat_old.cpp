//  kX Project audio driver for Mac OS X
//  Created by Eugene Gavrilov.
//	Copyright 2008-2014 Eugene Gavrilov. All rights reserved.
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


#include <IOKit/audio/IOAudioDevice.h>
#include <IOKit/audio/IOAudioEngine.h>
#include <IOKit/audio/IOAudioControl.h>
#include <IOKit/audio/IOAudioLevelControl.h>
#include <IOKit/audio/IOAudioToggleControl.h>
#include <IOKit/audio/IOAudioDefines.h>
#include <IOKit/IOLib.h>
#include <IOKit/pci/IOPCIDevice.h>
#include <IOKit/IOFilterInterruptEventSource.h>

#include "driver/kx.h"

#include "AudioDevice.h"
#include "AudioEngine.h"

#undef debug
#include "cedebug.h"

#define super IOAudioDevice

#undef kx_lock_acquire
#undef kx_lock_release

void kXAudioDevice::malloc_func(int len,void **b,int where)
{
	int *mem=(int *)IOMalloc(len+sizeof(int));
	if(mem)
	{
		*mem=len;
		mem++;
		*b=mem;
		
		memset(*b,0,len);
	}
	else
		*b=NULL;
}

void kXAudioDevice::free_func(void *buff)
{
	int *mem=(int *)buff;
	if(mem)
	{
		mem--;
		IOFree(mem,*mem);
	}
}

int kXAudioDevice::lmem_alloc_func(int len,void **lm,kx_cpu_cache_type_t cache_type)
{
	IOLog("kXAudioDevice[%p]::lmem_alloc_func: NOT SUPPORTED (FIXME)\n",this);
	
	if(lm)
		*lm=NULL;
	
	return -1;
}

int kXAudioDevice::lmem_free_func(void **lm)
{
	IOLog("kXAudioDevice[%p]::lmem_alloc_func: NOT SUPPORTED (FIXME)\n",this);
	return -1;
}

void * kXAudioDevice::lmem_get_addr_func(void **lm,int offset,__int64 *physical)
{
	IOLog("kXAudioDevice[%p]::lmem_alloc_func: NOT SUPPORTED (FIXME)\n",this);
	return NULL;
}

int kXAudioDevice::pci_alloc(struct memhandle *h,kx_cpu_cache_type_t cache_type)
{
	IOPhysicalAddress phys_addr=0;
	h->addr=IOMallocContiguous(h->size,PAGE_SIZE,&phys_addr);
	h->dma_handle=(dword)phys_addr; // FIXME NOW !!
	
	if(h->addr)
		return 0;
	else
		return -1;
}

void kXAudioDevice::pci_free(struct memhandle *h)
{
	IOFreeContiguous((void *)h->addr,h->size);
	h->addr=0;
	h->dma_handle=0;
	h->size=0;
}


void kXAudioDevice::send_message(int len,const void *b)
{
	// fixme !!
	debug("kXAudioDevice[%p]::send_message: notification message\n",this);
}

void kXAudioDevice::notify_func(void *data,int what)
{
	if(what==LLA_NOTIFY_TIMER)
	{
		debug("!! kXAudioDevice::notify: timer notification [should not happen!]\n");
	}
	else
	if(what==LLA_NOTIFY_EVENT)
	{
		#if 0
		// this is no longer necessary
		// event from ASIO ('buffer switch')
		// move this code to kXAudioDevice::interruptFilter and remove KXASIO_METHOD_SEND_EVENT
		if(data)
		{
			kXAudioEngine *that=(kXAudioEngine *)data;
			
			if(that->hw && that->hw->asio_notification_krnl.pb_buf==0)
			{
				// cur_pos is ~0 samples
				if(that->hw->asio_notification_krnl.cur_pos<=10)
					that->takeTimeStamp();
				else
					debug("!! kXAudioDevice::notify: invalid current position [%x]\n",that->hw->asio_notification_krnl.cur_pos);
			}
		}
		#else
		debug("!! kXAudioDevice::notify: invalid notification event\n");
		#endif
	}
	else
		debug("!! kXAudioDevice::notify: invalid notification event [%d]\n",what);
}

void kXAudioDevice::sync(sync_data*s)
{
	// this should call kx_sync(s); synchronized with any ISR/IRQ handlers
	// FIXME !!
	kx_sync(s);
}

int kXAudioDevice::debug_func(int where,const char *__format, ... )
{
	if(where<=0) return 0; // ignore this log
	
	char string[256];
	
	const char *subop="??";
	switch(where)
	{
		case  DERR: subop="ERROR"; break;
		case  DLIB: subop="HAL"; break;
	}
	
	va_list ap;
	va_start(ap, __format);
	vsnprintf(string,sizeof(string),__format,ap);
	IOLog("kXAudioDevice [%s] %s",subop,string);
	va_end(ap);
	
	return 0;
}

void kXAudioDevice::get_physical(kx_voice_buffer *buff,int offset,__int64 *physical)
{
	*physical=(__int64)((byte *)(buff->physical+offset));
}

void kXAudioDevice::save_fpu_state(kx_fpu_state *state)
{
	// fixme !!
}

void kXAudioDevice::rest_fpu_state(kx_fpu_state *state)
{
	// fixme !!
}

#undef kx_spin_lock_init
KX_API(void,kx_spin_lock_init(kx_hw *hw,spinlock_t *lock,const char *name))
{
	lock->lock=IORecursiveLockAlloc();
	lock->name=name;
	
	lock->file=NULL;
	lock->line=-1;
	lock->kx_lock=0;
}

#undef kx_lock_acquire
KX_API(void,kx_lock_acquire(kx_hw *hw, spinlock_t *lock, unsigned long *,const char *file,int line))
{
	IORecursiveLockLock(lock->lock);
	lock->kx_lock++;
	lock->file=file;
	lock->line=line;
}

#undef kx_lock_release
KX_API(void,kx_lock_release(kx_hw *hw, spinlock_t *lock, unsigned long *,const char *file,int line))
{
	lock->kx_lock--;
	lock->file=file;
	lock->line=line;
	IORecursiveLockUnlock(lock->lock);
}

double kx_log10(register double _x_)
{
	// FIXME
	return 0.0;
}

double kx_pow2(register double _y_)
{
	// fixme !!
	return 0.0;
}

double kx_pow10(register double y)
{
	// fixme !!
	return 0.0;
}

double kx_sqrt(register double y)
{
	// FIXME !!
	return 0.0;
}
