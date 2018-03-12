//  kX Project audio driver for Mac OS X
//  Created by Eugene Gavrilov.
//	Copyright 2008-2014 Eugene Gavrilov. All rights reserved.
//  https://github.com/kxproject/ (previously www.kxproject.com)

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


#ifndef _KXAUDIODEVICE_H
#define _KXAUDIODEVICE_H

#include <IOKit/audio/IOAudioDevice.h>
#include <IOKit/audio/IOAudioEngine.h>
#include <IOKit/audio/IOAudioControl.h>
#include <IOKit/audio/IOAudioLevelControl.h>
#include <IOKit/audio/IOAudioToggleControl.h>
#include <IOKit/audio/IOAudioDefines.h>
#include <IOKit/IOLib.h>
#include <IOKit/pci/IOPCIDevice.h>
#include <IOKit/IOFilterInterruptEventSource.h>
#include <IOKit/IOUserClient.h>
#include <AvailabilityMacros.h>

#include "driver/kx.h"
#include "interface/kx_ioctl.h"

#define kXAudioDevice	com_kXProject_driver_kXAudioDriver
#define kXAudioEngine	com_kXProject_driver_kXAudioEngine
#define kXUserClient	com_kXProject_driver_kXUserClient

// additional flags for hw->initialized
#define KX_DEVICE_INITED	0x1000
#define KX_ENGINE_INITED	0x2000

#ifndef KX_INTERNAL
	#error invalid configuration
#endif

struct kXRequest;

class kXAudioEngine;

class kXAudioDevice : public IOAudioDevice
{
    friend class kXAudioEngine;
public:    
    OSDeclareDefaultStructors(kXAudioDevice)
    
    IOPCIDevice						*pciDevice;
    IOMemoryMap						*deviceMap;
	kx_hw							*hw;
	IOFilterInterruptEventSource	*interruptEventSource;
	int								epilog_pgm;
	int								is_muted;
	dword							master_volume[2];
	kXAudioEngine					*engine;
	
	bool init(OSDictionary *dictionary);
	virtual bool initHardware(IOService *provider);
    virtual bool createAudioEngine();
    virtual void free();
	virtual void stop(IOService *provider);
 
	// ---- HAL functions [OS-specific]
    void malloc_func(int len,void **b,int where);
    void send_message(int len,const void *b);
    void free_func(void *buff);
    int lmem_alloc_func(int len,void **lm,kx_cpu_cache_type_t cache_type);
    int lmem_free_func(void **lm);
    void *lmem_get_addr_func(void **lm,int offset,__int64 *physical);
    int pci_alloc(struct memhandle *h,kx_cpu_cache_type_t cache_type);
	void pci_free(struct memhandle *h);
	void sync(sync_data*s);
	void get_physical(kx_voice_buffer *buff,int offset,__int64 *physical);
	    
	static void malloc_func(void *call_with,int len,void **b,int where) { ((kXAudioDevice *)call_with)->malloc_func(len,b,where); };
    static void send_message(void *call_with,int len,const void *b) { ((kXAudioDevice *)call_with)->send_message(len,b); };
    static void free_func(void *call_with,void *buff) { ((kXAudioDevice *)call_with)->free_func(buff); };
    static int lmem_alloc_func(void *call_with,int len,void **lm,kx_cpu_cache_type_t cache_type) { return ((kXAudioDevice *)call_with)->lmem_alloc_func(len,lm,cache_type); };
    static int lmem_free_func(void *call_with,void **lm) { return ((kXAudioDevice *)call_with)->lmem_free_func(lm); };
    static void *lmem_get_addr_func(void *call_with,void **lm,int offset,__int64 *physical) { return ((kXAudioDevice *)call_with)->lmem_get_addr_func(lm,offset,physical); };
    static int pci_alloc(void *call_with,struct memhandle *h,kx_cpu_cache_type_t cache_type) { return ((kXAudioDevice *)call_with)->pci_alloc(h,cache_type); };
    static void pci_free(void *call_with,struct memhandle *h) { ((kXAudioDevice *)call_with)->pci_free(h); };
    static void sync(void *call_with,sync_data*s) { ((kXAudioDevice *)call_with)->sync(s); };
    static void usleep(int microseconds) { IODelay(microseconds); };
	static void get_physical(void *call_with,kx_voice_buffer *buff,int offset,__int64 *physical) { ((kXAudioDevice *)call_with)->get_physical(buff,offset,physical); };

	static int debug_func(int where,const char *__format, ... );
    static void save_fpu_state(kx_fpu_state *state);
    static void rest_fpu_state(kx_fpu_state *state);
	
    static void notify_func(void *data,int what);
	// ---- end of HAL functions
	
	
	// volume controls
    static IOReturn volumeChangeHandler(IOService *target, IOAudioControl *volumeControl, SInt32 oldValue, SInt32 newValue);
    virtual IOReturn volumeChanged(IOAudioControl *volumeControl, SInt32 oldValue, SInt32 newValue);
    
    static IOReturn outputMuteChangeHandler(IOService *target, IOAudioControl *muteControl, SInt32 oldValue, SInt32 newValue);
    virtual IOReturn outputMuteChanged(IOAudioControl *muteControl, SInt32 oldValue, SInt32 newValue);
	
	int create_audio_controls(IOAudioEngine *audioEngine);
	
public:
	virtual IOReturn user_request(const void* inStruct, void* outStruct,uint32_t inStructSize, const uint32_t* outStructSize);

	virtual IOReturn performPowerStateChange(IOAudioDevicePowerState oldPowerState, IOAudioDevicePowerState newPowerState, UInt32 *microsecondsUntilComplete);

protected:
	/*
    static IOReturn gainChangeHandler(IOService *target, IOAudioControl *gainControl, SInt32 oldValue, SInt32 newValue);
    virtual IOReturn gainChanged(IOAudioControl *gainControl, SInt32 oldValue, SInt32 newValue);
    
    static IOReturn inputMuteChangeHandler(IOService *target, IOAudioControl *muteControl, SInt32 oldValue, SInt32 newValue);
    virtual IOReturn inputMuteChanged(IOAudioControl *muteControl, SInt32 oldValue, SInt32 newValue);
	 */
	
	// for FPGA programming under OS X, we need the following
	byte *fpga_fw;
	int fpga_fw_offset;
	int fpga_fw_size;
	
    static void interruptHandler(OSObject *owner, IOInterruptEventSource *source, int count);
    static bool interruptFilter(OSObject *owner, IOFilterInterruptEventSource *source);
};

#endif /* _KXAUDIODEVICE_H */
