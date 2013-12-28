// kX Driver
// Copyright (c) Eugene Gavrilov, 2007-2008.
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


#ifndef OSMAC_H_
#define OSMAC_H_

#define KX_API(a,b) a b

#ifdef KX_INTERNAL
  
  #define my_strcpy strcpy
  #define my_strncpy strncpy
  #define my_strcmp strcmp
  #define my_memset memset
  #define my_memcpy memcpy

  // this is from
  // /Developer/SDKs/MacOSX10.4u.sdk/System/Library/Frameworks/Kernel.framework/Versions/A/Headers/string.h
  #include <string.h>

#endif

#ifndef __IOKIT_IOLOCKS_H
 #define IORecursiveLock	void
#endif

typedef struct
{
 IORecursiveLock *lock;
 const char *file;
 const char *name;
 int line;
 int kx_lock;
}spinlock_t;

struct kx_hw;

KX_API(void,kx_spin_lock_init(kx_hw *hw,spinlock_t *,const char *name));
KX_API(void,kx_lock_acquire(kx_hw *hw, spinlock_t *, unsigned long *,const char *file,int line));
KX_API(void,kx_lock_release(kx_hw *hw, spinlock_t *, unsigned long *,const char *file,int line));

#ifdef KX_DEBUG
 #define kx_lock_acquire(a,b,c) kx_lock_acquire(a,b,c,__FILE__,__LINE__)
 #define kx_lock_release(a,b,c) kx_lock_release(a,b,c,__FILE__,__LINE__)
#else
 #define kx_lock_acquire(a,b,c) kx_lock_acquire(a,b,c,NULL,__LINE__)
 #define kx_lock_release(a,b,c) kx_lock_release(a,b,c,NULL,__LINE__)
#endif

#ifndef _IOBUFFERMEMORYDESCRIPTOR_H
class IOBufferMemoryDescriptor;
#endif

struct memhandle
{
	// note: this is for 32-bit OS only
	size_t size;
	void * addr;		// virtual
	dword dma_handle;	// physical

	IOBufferMemoryDescriptor *desc;
};

#endif
