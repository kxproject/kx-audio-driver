//  kX Project audio driver for Mac OS X
//  Created by Eugene Gavrilov.
//  Copyright 2008-2014 Eugene Gavrilov. All rights reserved.
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


#include "AudioDevice.h"
#include "AudioEngine.h"

#define DBGCLASS "kXAudioDevice"

#undef debug
#include "cedebug.h"

#define super IOAudioDevice
OSDefineMetaClassAndStructors(kXAudioDevice, IOAudioDevice)

bool kXAudioDevice::init(OSDictionary *dictionary)
{
    debug(DBGCLASS"[%p]::init: --- driver start-up, version %s\n",this,DRIVER_VERSION);
    
    // PE_enter_debugger("start-up");
    
    pciDevice=NULL;
    deviceMap=NULL;
    hw=NULL;
    interruptEventSource=NULL;
    epilog_pgm=-1;
    is_muted=0;
    master_volume[0]=0x80000000;
    master_volume[1]=0x80000000;
    
    engine=NULL;
    
    fpga_fw=0;
    fpga_fw_offset=0;
    fpga_fw_size=0;
    
    return super::init(dictionary);
}

bool kXAudioDevice::initHardware(IOService *provider)
{
    bool result = false;
    IOWorkLoop *workLoop_=NULL;
    
    debug(DBGCLASS"[%p]::initHardware(%p)\n", this, provider);
    
    if (!super::initHardware(provider))
    {
        goto Done;
    }
    
    // Get the PCI device provider
    pciDevice = OSDynamicCast(IOPCIDevice, provider);
    if (!pciDevice)
    {
        goto Done;
    }
    
    workLoop_ = getWorkLoop();
    if (!workLoop_)
    {
        debug(DBGCLASS"[%p]::initHardware: failed to get work loop\n",this);
        goto Done;
    }
    
    // Create an interrupt event source through which to receive interrupt callbacks
    // In this case, we only want to do work at primary interrupt time, so
    // we create an IOFilterInterruptEventSource which makes a filtering call
    // from the primary interrupt interrupt who's purpose is to determine if
    // our secondary interrupt handler is to be called.  In our case, we
    // can do the work in the filter routine and then return false to
    // indicate that we do not want our secondary handler called
    interruptEventSource = IOFilterInterruptEventSource::filterInterruptEventSource(this,
                                                                                    kXAudioDevice::interruptHandler,
                                                                                    kXAudioDevice::interruptFilter,
                                                                                    provider);
    if (!interruptEventSource)
    {
        goto Done;
    }
    
    // In order to allow the interrupts to be received, the interrupt event source must be
    // added to the IOWorkLoop
    // Additionally, interrupts will not be firing until the interrupt event source is
    // enabled by calling interruptEventSource->enable()
    workLoop_->addEventSource(interruptEventSource);
    
    UInt8 bus,device,function;
    UInt32 dev_id,subsys_id,chiprev_id;
    
    word port;
    
    bus = pciDevice->getBusNumber();
    device = pciDevice->getDeviceNumber();
    function = pciDevice->getFunctionNumber();
    
    dev_id = pciDevice->configRead32(kIOPCIConfigVendorID);
    subsys_id = pciDevice->configRead32(kIOPCIConfigSubSystemVendorID);
    chiprev_id = pciDevice->configRead32(kIOPCIConfigRevisionID);
    
    port = pciDevice->configRead16(kIOPCIConfigBaseAddress0);
    port=port&0xfffe;
    
    debug(DBGCLASS"[%p]::initHardware: bus/dev/func: %d/%d/%d; device/subsys/chip: %08x:%08x:%08x - at port %x\n",this,bus,device,function,(unsigned int)dev_id,(unsigned int)subsys_id,(unsigned int)chiprev_id,(unsigned int)port);
    
    // Config a map for the PCI config base registers
    // We need to keep this map around until we're done accessing the registers
    deviceMap = pciDevice->mapDeviceMemoryWithRegister(kIOPCIConfigBaseAddress0);
    if (!deviceMap)
    {
        goto Done;
    }
    
    debug(DBGCLASS"[%p]::initHardware: I/O range @0x%x (%08lx) mapped into %08x [up to %08x]\n",
          this,
          kIOPCIConfigBaseAddress0,
          (unsigned long)deviceMap->getPhysicalAddress(),
          (unsigned) deviceMap->getVirtualAddress(),
          (unsigned int)(deviceMap->getVirtualAddress()+deviceMap->getLength()-1));
    
    // Enable the PCI memory access - the kernel will panic if this isn't done before accessing the
    // mapped registers
    pciDevice->setMemoryEnable(true);
    pciDevice->setIOEnable(true);
    pciDevice->setBusMasterEnable(true);
    
    setManufacturerName("Eugene Gavrilov, kX Project");
    setDeviceTransportType(kIOAudioDeviceTransportTypePCI);
    
    kx_callbacks cb;
    memset(&cb,0,sizeof(cb));
    
    cb.call_with=this;
    cb.irql=0x0; // unused
    cb.io_base=deviceMap->getPhysicalAddress();
    
    cb.device=dev_id;
    cb.subsys=subsys_id;
    cb.chip_rev=chiprev_id;
    cb.bus=bus;
    cb.dev=device;
    cb.func=function;
    
    cb.malloc_func=&kXAudioDevice::malloc_func;
    cb.send_message=&kXAudioDevice::send_message;
    cb.free_func=&kXAudioDevice::free_func;
    cb.lmem_alloc_func=&kXAudioDevice::lmem_alloc_func;
    cb.lmem_free_func=&kXAudioDevice::lmem_free_func;
    cb.lmem_get_addr_func=&kXAudioDevice::lmem_get_addr_func;
    cb.notify_func=&kXAudioDevice::notify_func;
    
    cb.debug_func=&kXAudioDevice::debug_func;
    cb.pci_alloc=&kXAudioDevice::pci_alloc;
    cb.pci_free=&kXAudioDevice::pci_free;
    cb.get_physical=&kXAudioDevice::get_physical;
    cb.save_fpu_state=&kXAudioDevice::save_fpu_state;
    cb.rest_fpu_state=&kXAudioDevice::rest_fpu_state;
    cb.sync=&kXAudioDevice::sync;
    cb.usleep=&kXAudioDevice::usleep;
    
    // default parameters:
    kx_defaults(NULL,&cb);
    
    int ret;
    ret=kx_init(&hw,&cb,0);
    
    if(ret || hw==NULL)
    {
        debug(DBGCLASS"[%p]::init: failed [%d]\n",this,ret);
        
        if(hw)
        {
            kx_close(&hw);
            hw=NULL;
        }
        
        goto Done;
    }
    
    hw->initialized|=KX_DEVICE_INITED;
    
    // re-set defaults based on hardware features
    kx_defaults(hw,NULL);
    
    char device_name[KX_MAX_STRING];
    strncpy(device_name,"kX ",KX_MAX_STRING);
    strncat(device_name,hw->card_name,KX_MAX_STRING);
    
    setDeviceName(device_name);
    setDeviceShortName("kXAudio");
    
    // The interruptEventSource needs to be enabled to allow interrupts to start firing
    if(interruptEventSource)
        interruptEventSource->enable();
    
    if (!createAudioEngine())
    {
        goto Done;
    }
    
    
    result = true;
    
Done:
    
    if (!result)
    {
        if(interruptEventSource)
        {
            interruptEventSource->disable();
            
            if (workLoop_)
            {
                workLoop_->removeEventSource(interruptEventSource);
            }
            
            interruptEventSource->release();
            interruptEventSource = NULL;
        }
        
        if(hw)
        {
            kx_close(&hw);
            hw=0;
        }
        
        if (deviceMap)
        {
            deviceMap->release();
            deviceMap = NULL;
        }
        
        pciDevice=NULL;
    }
    
    return result;
}

void kXAudioDevice::stop(IOService *provider)
{
    debug(DBGCLASS"[%p]::stop: interrupts disabled\n",this);
    
    // Assuming we don't need interrupts after stopping the audio engine, we can disable them here
    if(interruptEventSource)
        interruptEventSource->disable();
    
    super::stop(provider);
}

void kXAudioDevice::free()
{
    debug(DBGCLASS"[%p]::free()\n", this);
    
    // When our device is being stopped and torn down, we should go ahead and remove
    // the interrupt event source from the IOWorkLoop
    // Additionally, we'll go ahead and release the interrupt event source since it isn't
    // needed any more
    
    if (interruptEventSource)
    {
        interruptEventSource->disable();
        
        IOWorkLoop *wl = getWorkLoop();
        if (wl)
        {
            wl->removeEventSource(interruptEventSource);
        }
        
        interruptEventSource->release();
        interruptEventSource = NULL;
    }
    
    if(hw)
    {
        if((hw->initialized&KX_DEVICE_INITED) && !(hw->initialized&KX_ENGINE_INITED))
        {
            debug(DBGCLASS"[%p]::free_all() - close iKX interface\n",this);
            kx_close(&hw);
            hw=0;
        }
        else
        {
            debug(DBGCLASS"[%p]::free_all() - dereference iKX interface\n",this);
            hw->initialized&=(~KX_DEVICE_INITED);
            hw=NULL;
        }
    }
    else
        debug(DBGCLASS"[%p]::free_all() - iKX interface already closed\n",this);
    
    if (deviceMap)
    {
        deviceMap->release();
        deviceMap = NULL;
    }
    
    pciDevice=NULL;
    
    super::free();
}

bool kXAudioDevice::createAudioEngine()
{
    bool result = false;
    kXAudioEngine *audioEngine = NULL;
    // IOAudioControl *control;
    
    debug(DBGCLASS"[%p]::createAudioEngine()\n", this);
    
    audioEngine = new kXAudioEngine;
    if (!audioEngine)
    {
        goto Done;
    }
    
    // Init the new audio engine with the device registers so it can access them if necessary
    // The audio engine subclass could be defined to take any number of parameters for its
    // initialization - use it like a constructor
    if (!audioEngine->init(hw))
    {
        goto Done;
    }
    
    create_audio_controls(audioEngine);
    
    // Active the audio engine - this will cause the audio engine to have start() and initHardware() called on it
    // After this function returns, that audio engine should be ready to begin vending audio services to the system
    activateAudioEngine(audioEngine);
    // Once the audio engine has been activated, release it so that when the driver gets terminated,
    // it gets freed
    audioEngine->release();
    
    result = true;
    
Done:
    
    engine=audioEngine;
    
    if (!result && (audioEngine != NULL))
    {
        audioEngine->release();
    }
    
    return result;
}

void kXAudioDevice::interruptHandler(OSObject *owner, IOInterruptEventSource *source, int count)
{
    kXAudioDevice *that = OSDynamicCast(kXAudioDevice, owner);
    // If the filter returned true, this function would be called on the IOWorkLoop
    
    if(that && that->hw)
    {
        while(1)
        {
            int ret=kx_interrupt_deferred(that->hw); // this calls indirectly kXAudioDevice::notify_func()
            
            if(ret==KX_IRQ_NONE)
                return;
            
            // NOP
        }
    }
}

bool kXAudioDevice::interruptFilter(OSObject *owner, IOFilterInterruptEventSource *source)
{
    kXAudioDevice *that = OSDynamicCast(kXAudioDevice, owner);
    
    // We've cast the audio engine from the owner which we passed in when we created the interrupt
    // event source
    if (that && that->hw)
    {
        dword ipr=inpd(that->hw->port+IPR);
        if(ipr==0)
            return false; // not our IRQ, no need to queue interruptHandler
        
        sync_data dta;
        dta.what=KX_SYNC_IPR_IRQ;
        dta.hw=that->hw;
        that->sync(&dta); // this calls kx_sync() -> kx_critical_handler()
        
        if(dta.ret==0 && dta.irq_mask) // our IRQ and have something to process in DPC?
        {
            if(dta.irq_mask&IRQ_VOICE)
            {
                // check for voice interrupt. if yes, call kXAudioEngine::takeTimeStamp();
                // otherwise this is queued in DPC
                // event from ASIO ('buffer switch')
                
                if(that->engine)
                {
                    if(that->hw && (that->hw->asio_notification_krnl.pb_buf==0))
                    {
                        that->engine->takeTimeStamp();
                        
                        // need to reset 'toggle', because for 10k1 cards it is always 1
                        that->hw->asio_notification_krnl.toggle=-1;
                        
                    } // pb_buf==0?
                } // engine
                
                dta.irq_mask&=(~IRQ_VOICE);
            }
            
            if(dta.irq_mask)
            {
                return true; // still need to queue interruptHandler
            }
        }
        return false;
    }
    
    return false;
}

int kXAudioDevice::create_audio_controls(IOAudioEngine *audioEngine)
{
    // enumerate DSP effects, find 'epilog'
    epilog_pgm=-1;
    prolog_pgm=-1;
    
    // find microcode 'epilog'
    unsigned long flags=0;
    struct list *item;
    kx_lock_acquire(hw,&hw->dsp_lock,&flags);
    for_each_list_entry(item,&hw->microcodes)
    {
        dsp_microcode *m=list_item(item,dsp_microcode,list);
        if(!m) continue;
        if(strncmp(m->name,"epilog",KX_MAX_STRING)==NULL)
        {
            epilog_pgm=m->pgm;
            break;
        }
    }
    kx_lock_release(hw,&hw->dsp_lock,&flags);
    
    for_each_list_entry(item,&hw->microcodes)
    {
        dsp_microcode *m=list_item(item,dsp_microcode,list);
        if(!m) continue;
        if(strncmp(m->name,"prolog",KX_MAX_STRING)==NULL)
        {
            prolog_pgm=m->pgm;
            break;
        }
    }
    kx_lock_release(hw,&hw->dsp_lock,&flags);
    
    kx_set_dsp_register(hw,prolog_pgm,"in0vol",0x2000*65535);
    kx_set_dsp_register(hw,prolog_pgm,"in1vol",0x2000*65535);
    kx_set_dsp_register(hw,prolog_pgm,"in2vol",0x2000*65535);
    kx_set_dsp_register(hw,prolog_pgm,"in3vol",0x2000*65535);
    kx_set_dsp_register(hw,prolog_pgm,"in4vol",0x2000*65535);
    kx_set_dsp_register(hw,prolog_pgm,"in5vol",0x2000*65535);
    
    {
        IOAudioLevelControl *control=NULL;
        
        // Create a left & right output volume control with an int range from 0 to 65535
        // and a db range from -100.0 to 0.0
        // Once each control is added to the audio engine, they should be released
        // so that when the audio engine is done with them, they get freed properly
        control = IOAudioLevelControl::createVolumeControl(65535,   // Initial value
                                                           0,       // min value
                                                           65535,   // max value
                                                           (-100 << 16) + (0),  // -100.0 in IOFixed (16.16)
                                                           0,       // max 0.0 in IOFixed
                                                           kIOAudioControlChannelIDAll,
                                                           kIOAudioControlChannelNameAll,
                                                           0,       // control ID - driver-defined
                                                           kIOAudioControlUsageOutput);
        if (!control)
            goto Done;
        
        control->setLinearScale(false);
        control->setValueChangeHandler((IOAudioControl::IntValueChangeHandler)volumeChangeHandler, this);
        audioEngine->addDefaultAudioControl(control);
        control->release();
        
        for(int i=0;i<8;i++)
        {
            // order is not important actually
            const char *names[]=
            {
                kIOAudioControlChannelNameLeft,
                kIOAudioControlChannelNameRight,
                kIOAudioControlChannelNameCenter,
                kIOAudioControlChannelNameLeftRear,
                kIOAudioControlChannelNameRightRear,
                kIOAudioControlChannelNameSub,
                "BackLeft",
                "BackRight" };
            
            int ids[]=
            {
                kIOAudioControlChannelIDDefaultLeft,
                kIOAudioControlChannelIDDefaultRight,
                kIOAudioControlChannelIDDefaultCenter,
                kIOAudioControlChannelIDDefaultLeftRear,
                kIOAudioControlChannelIDDefaultRightRear,
                kIOAudioControlChannelIDDefaultSub,
                kIOAudioControlChannelIDDefaultSub+1,
                kIOAudioControlChannelIDDefaultSub+2
            };
            
            control = IOAudioLevelControl::createVolumeControl(65535,   // Initial value
                                                               0,       // min value
                                                               65535,   // max value
                                                               (-100 << 16) + (0),  // min -100.0 in IOFixed (16.16)
                                                               0,       // max 0.0 in IOFixed
                                                               ids[i],
                                                               names[i],
                                                               0,       // control ID - driver-defined
                                                               kIOAudioControlUsageOutput);
            if (!control)
                goto Done;
            
            control->setLinearScale(false);
            control->setValueChangeHandler((IOAudioControl::IntValueChangeHandler)volumeChangeHandler, this);
            audioEngine->addDefaultAudioControl(control);
            control->release();
        } // for
    }
    {
        IOAudioToggleControl *control=NULL;
        
        // Create an output mute control
        control = IOAudioToggleControl::createMuteControl(false,    // initial state - unmuted
                                                          kIOAudioControlChannelIDAll,  // Affects all channels
                                                          kIOAudioControlChannelNameAll,
                                                          0,        // control ID - driver-defined
                                                          kIOAudioControlUsageOutput);
        
        if (!control) {
            goto Done;
        }
        
        control->setValueChangeHandler((IOAudioControl::IntValueChangeHandler)outputMuteChangeHandler, this);
        audioEngine->addDefaultAudioControl(control);
        control->release();
    }
    
    {   // input control
        
        IOAudioLevelControl *control=NULL;
        
        for(int i = 0; i < 8; i++)
        {
            const char *names[]=
            {
                kIOAudioControlChannelNameLeft,
                kIOAudioControlChannelNameRight,
                kIOAudioControlChannelNameCenter,
                kIOAudioControlChannelNameLeftRear,
                kIOAudioControlChannelNameRightRear,
                kIOAudioControlChannelNameSub,
                "BackLeft",
                "BackRight" };
            
            int ids[]=
            {
                kIOAudioControlChannelIDDefaultLeft,
                kIOAudioControlChannelIDDefaultRight,
                kIOAudioControlChannelIDDefaultCenter,
                kIOAudioControlChannelIDDefaultLeftRear,
                kIOAudioControlChannelIDDefaultRightRear,
                kIOAudioControlChannelIDDefaultSub,
                kIOAudioControlChannelIDDefaultSub+1,
                kIOAudioControlChannelIDDefaultSub+2
            };
            control = IOAudioLevelControl::createVolumeControl(65535,   // Initial value
                                                               0,       // min value
                                                               65535,   // max value
                                                               (-100 << 16) + (0),
                                                               0,
                                                               ids[i],
                                                               names[i],
                                                               0,       // control ID - driver-defined
                                                               kIOAudioControlUsageInput);
            if (!control) {
                goto Done;
            }
            
            control->setValueChangeHandler((IOAudioControl::IntValueChangeHandler)gainChangeHandler, this);
            audioEngine->addDefaultAudioControl(control);
            control->release();
        }
    }
Done:
    return 0;
}

IOReturn kXAudioDevice::volumeChangeHandler(IOService *target, IOAudioControl *volumeControl, SInt32 oldValue, SInt32 newValue)
{
    IOReturn result = kIOReturnBadArgument;
    kXAudioDevice *audioDevice;
    
    audioDevice = (kXAudioDevice *)target;
    if (audioDevice)
    {
        result = audioDevice->volumeChanged(volumeControl, oldValue, newValue);
    }
    
    return result;
}

IOReturn kXAudioDevice::volumeChanged(IOAudioControl *volumeControl, SInt32 oldValue, SInt32 newValue)
{
    if (volumeControl && hw && epilog_pgm!=-1)
    {
        int id=volumeControl->getChannelID();
        
        const char *regs_gen[]=
        {
            "none",
            "out0vol", // front left/right; osx 1,2
            "out1vol",
            "out17vol", // center, lfe; osx 3,4
            "out18vol",
            "out8vol", // rear left/right; osx 5,6
            "out9vol",
            "out30vol", // back left/right; osx 7,8
            "out31vol"
        };
        const char *regs_aps[]=
        {
            "none",
            "out0vol",
            "out1vol",
            "out4vol",
            "out5vol",
            "out2vol",
            "out3vol",
            "none",
            "none"
        };
        const char *regs_edsp[]=
        {
            "none",
            "out0vol",
            "out1vol",
            "out6vol",
            "out7vol",
            "out4vol",
            "out5vol",
            "out8vol",
            "out9vol"
        };
        
        const char **regs;
        if(hw->is_aps)
            regs=regs_aps;
        else
            if(hw->is_edsp)
                regs=regs_edsp;
            else
                regs=regs_gen;
        
        dword vol;
        
        if(id==0) // all channels
        {
            // newValue is from 0 to 65535 (-100dB .. 0dB), linear
            // 0xffff = 0dB = 0x80000000
            // 0x8000 = 50% = 0xc0000000
            
            if(newValue==0)
                vol=0x0;
            else
                vol=(dword)((newValue+1)*(-32768));
            
            master_volume[0]=vol;
            master_volume[1]=vol;
            kx_set_dsp_register(hw,epilog_pgm,"MasterL",is_muted?0x0:master_volume[0]);
            kx_set_dsp_register(hw,epilog_pgm,"MasterR",is_muted?0x0:master_volume[1]);
        }
        else
        {
            // newValue is from 0 to 65535 (-100dB .. 0dB), linear
            // 0xffff = 0dB = 0x20000000
            // 0x8000 = 50% = 0x10000000
            
            if(newValue==0)
                vol=0x0;
            else
                vol=(dword)((newValue+1)*0x2000);
            
            kx_set_dsp_register(hw,epilog_pgm,regs[id],vol); // these are +12dB volumes
        }
        
        debug(DBGCLASS"[%p]::volumeChanged(%p, %d, %d): chn: %d -> %x; %d (%s)\n",
              this, volumeControl, (int)oldValue, (int)newValue, (int)volumeControl->getChannelID(),(unsigned int)vol,epilog_pgm,regs[id]);
    }
    
    // Add hardware volume code change
    
    return kIOReturnSuccess;
}

IOReturn kXAudioDevice::outputMuteChangeHandler(IOService *target, IOAudioControl *muteControl, SInt32 oldValue, SInt32 newValue)
{
    IOReturn result = kIOReturnBadArgument;
    kXAudioDevice *audioDevice;
    
    audioDevice = (kXAudioDevice *)target;
    if (audioDevice)
    {
        result = audioDevice->outputMuteChanged(muteControl, oldValue, newValue);
    }
    
    return result;
}

IOReturn kXAudioDevice::outputMuteChanged(IOAudioControl *muteControl, SInt32 oldValue, SInt32 newValue)
{
    debug(DBGCLASS"[%p]::outputMuteChanged(%p, %d, %d)\n", this, muteControl, (int)oldValue, (int)newValue);
    
    // Add output mute code here
    if(muteControl && hw && epilog_pgm!=-1)
    {
        if(newValue)
            is_muted=1;
        else
            is_muted=0;
        
        kx_set_dsp_register(hw,epilog_pgm,"MasterL",is_muted?0x0:master_volume[0]);
        kx_set_dsp_register(hw,epilog_pgm,"MasterR",is_muted?0x0:master_volume[1]);
    }
    
    return kIOReturnSuccess;
}

/*
 
 // Audio Controls
 // UNUSED
 
 // Create a left & right input gain control with an int range from 0 to 65535
 // and a db range from 0 to 22.5
 control = IOAudioLevelControl::createVolumeControl(65535,   // Initial value
 0,       // min value
 65535,   // max value
 0,       // min 0.0 in IOFixed
 (22 << 16) + (32768),    // 22.5 in IOFixed (16.16)
 kIOAudioControlChannelIDDefaultLeft,
 kIOAudioControlChannelNameLeft,
 0,       // control ID - driver-defined
 kIOAudioControlUsageInput);
 if (!control) {
 goto Done;
 }
 
 control->setValueChangeHandler((IOAudioControl::IntValueChangeHandler)gainChangeHandler, this);
 audioEngine->addDefaultAudioControl(control);
 control->release();
 
 control = IOAudioLevelControl::createVolumeControl(65535,  // Initial value
 0,      // min value
 65535,  // max value
 0,      // min 0.0 in IOFixed
 (22 << 16) + (32768),   // max 22.5 in IOFixed (16.16)
 kIOAudioControlChannelIDDefaultRight,   // Affects right channel
 kIOAudioControlChannelNameRight,
 0,      // control ID - driver-defined
 kIOAudioControlUsageInput);
 if (!control) {
 goto Done;
 }
 
 control->setValueChangeHandler((IOAudioControl::IntValueChangeHandler)gainChangeHandler, this);
 audioEngine->addDefaultAudioControl(control);
 control->release();
 
 // Create an input mute control
 control = IOAudioToggleControl::createMuteControl(false,   // initial state - unmuted
 kIOAudioControlChannelIDAll, // Affects all channels
 kIOAudioControlChannelNameAll,
 0,       // control ID - driver-defined
 kIOAudioControlUsageInput);
 
 if (!control) {
 goto Done;
 }
 
 control->setValueChangeHandler((IOAudioControl::IntValueChangeHandler)inputMuteChangeHandler, this);
 audioEngine->addDefaultAudioControl(control);
 control->release();
 */

IOReturn kXAudioDevice::gainChangeHandler(IOService *target, IOAudioControl *gainControl, SInt32 oldValue, SInt32 newValue)
{
    IOReturn result = kIOReturnBadArgument;
    kXAudioDevice *audioDevice;
    
    audioDevice = (kXAudioDevice *)target;
    if (audioDevice) {
        result = audioDevice->gainChanged(gainControl, oldValue, newValue);
    }
    
    return result;
}

IOReturn kXAudioDevice::gainChanged(IOAudioControl *gainControl, SInt32 oldValue, SInt32 newValue)
{
    debug(DBGCLASS"[%p]::gainChanged(%p, %ld, %ld)\n", this, gainControl, oldValue, newValue);
    
        if (gainControl) {
            debug("\t-> Channel %ld\n", gainControl->getChannelID());
        }
    
    // Add hardware gain change code here
    if (gainControl && prolog_pgm!=-1){
        dword gain;
        int id = gainControl->getChannelID();
        gain=(dword)((newValue+1)*(0x2000)); // these are +12dB volumes
        
        if(id == 1){
            kx_set_dsp_register(hw,prolog_pgm,"in0vol",gain);
        }
        if(id == 2){
            kx_set_dsp_register(hw,prolog_pgm,"in1vol",gain);
        }
        if(id == 3){
            kx_set_dsp_register(hw,prolog_pgm,"in2vol",gain);
        }
        if(id == 4){
            kx_set_dsp_register(hw,prolog_pgm,"in3vol",gain);
        }
        if(id == 5){
            kx_set_dsp_register(hw,prolog_pgm,"in4vol",gain);
        }
        if(id == 6){
            kx_set_dsp_register(hw,prolog_pgm,"in5vol",gain);
        }
        if(id == 7){
            kx_set_dsp_register(hw,prolog_pgm,"in6vol",gain);
        }
        if(id == 8){
            kx_set_dsp_register(hw,prolog_pgm,"in7vol",gain);
        }
    }
    
    return kIOReturnSuccess;
}
/*
 IOReturn kXAudioDevice::inputMuteChangeHandler(IOService *target, IOAudioControl *muteControl, SInt32 oldValue, SInt32 newValue)
 {
 IOReturn result = kIOReturnBadArgument;
 kXAudioDevice *audioDevice;
 
 audioDevice = (kXAudioDevice *)target;
 if (audioDevice) {
 result = audioDevice->inputMuteChanged(muteControl, oldValue, newValue);
 }
 
 return result;
 }
 
 IOReturn kXAudioDevice::inputMuteChanged(IOAudioControl *muteControl, SInt32 oldValue, SInt32 newValue)
 {
 debug(DBGCLASS"[%p]::inputMuteChanged(%p, %ld, %ld)\n", this, muteControl, oldValue, newValue);
 
 // Add input mute change code here
 
 return kIOReturnSuccess;
 }
 */


#define prep_in(type) type *in; in=(type *)(((dword *)inStruct+1));
#define prep_out(type) type *out; out=(type *)(((dword *)outStruct+1));

IOReturn kXAudioDevice::user_request(const void* inStruct, void* outStruct,uint32_t inStructSize, const uint32_t* outStructSize)
{
    // first part is 'prop', the rest is data struct
    // prop is:
    // KX_TOPO/KX_WAVE
    // KX_PROP_SET/KX_PROP_GET
    // function #
    if(inStructSize<sizeof(dword))
        return kIOReturnInvalid;
    
    dword prop = * (dword *)inStruct;
    
#define KX_WAVE     0x10000000
#define KX_TOPO     0x20000000
#define KX_SYNTH    0x40000000
#define KX_UART     0x80000000
    
    prop&=~(KX_TOPO|KX_WAVE);
    
    switch(prop)
    {
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
                return kIOReturnBadArgument;
            }
        }
            break;
        case KX_PROP_INITPASSTHRU+KX_PROP_GET:
        {
            prep_in(dword_property);
            prep_out(dword_property);
            
            int method=0;
            void *instance_=(void *)0x1; // FIXME
            
            if(kx_init_passthru(hw,instance_,&method)==0)
            {
                out->reg=(dword)method;
                out->pntr=instance_;
                out->chn=0x0;
            }
            else
                return kIOReturnBadArgument;
        }
            break;
        case KX_PROP_CLOSEPASSTHRU+KX_PROP_GET:
        {
            prep_in(dword_property);
            prep_out(dword_property);
            
            if(kx_close_passthru(hw,in->pntr)!=0)
                return kIOReturnBadArgument;
        }
            break;
        case KX_PROP_SETPASSTHRU+KX_PROP_GET:
        {
            prep_in(dword_property);
            prep_out(dword_property);
            
            if(kx_set_passthru(hw,in->pntr,(int)in->reg)!=0)
                return kIOReturnBadArgument;
        }
            break;
            
        case KX_PROP_HW_PARAM+KX_PROP_GET:
        {
            prep_in(dword_property);
            prep_out(dword_property);
            if(in->reg==KX_HW_KX3D)
            {
                /*
                 out->reg=0;
                 if(adapter && adapter->Wave[0])
                 out->val=adapter->Wave[0]->kx3d_compat;
                 */
                return kIOReturnBadArgument;
            }
            else
                if(in->reg==KX_HW_8PS)
                {
                    /*
                     out->reg=0;
                     if(adapter && adapter->Wave[0])
                     out->val=adapter->Wave[0]->kx3d_sp8ps;
                     */
                    return kIOReturnBadArgument;
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
                /*
                 if(adapter && adapter->Wave[0])
                 adapter->Wave[0]->kx3d_compat=in->val;
                 */
                return kIOReturnBadArgument;
            }
            else
            {
                if(in->reg==KX_HW_8PS)
                {
                    /*
                     if(adapter && adapter->Wave[0])
                     adapter->Wave[0]->kx3d_sp8ps=in->val;
                     */
                    return kIOReturnBadArgument;
                }
                else
                    if(kx_set_hw_parameter(hw,in->reg,in->val))
                        return kIOReturnBadArgument;
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
        case KX_PROP_FPGALINK+KX_PROP_SET:
        {
            prep_in(dword_property);
            kx_fpga_link_src2dst(hw,in->reg,in->val);
        }
            break;
        case KX_PROP_FPGA_FIRMWARE+KX_PROP_GET:
        {
            prep_in(fpga_firmware_property);
            
            if(inStructSize>=4 && inStructSize>=in->size+sizeof(dword))
            {
                if(kx_upload_fpga_firmware(hw,(byte *)((&in->size)+1),in->size))
                {
                    debug(DBGCLASS"::property: !! fpga firmware not uploaded\n");
                    return kIOReturnBadArgument;
                }
                debug(DBGCLASS"::property: fpga firmware uploaded - %d, %d\n",(int)inStructSize,in->size);
            }
            else
            {
                debug(DBGCLASS"::property: !! Firmware instance too small [2]\n");
                return kIOReturnBadArgument;
            }
        }
            break;
        case KX_PROP_FPGA_FW_START+KX_PROP_GET:
        {
            prep_in(fpga_firmware_property);
            
            if(inStructSize>=12 && fpga_fw==NULL)
            {
                fpga_fw=(byte *)IOMalloc(in->size);
                if(!fpga_fw)
                    return kIOReturnNoMemory;
                
                fpga_fw_size=in->size;
                fpga_fw_offset=0;
                in++;
                inStructSize-=4;
                
                if(in->size<=(fpga_fw_size-fpga_fw_offset) && in->size==(int)(inStructSize-sizeof(int)-sizeof(dword)))
                {
                    memcpy(&fpga_fw[fpga_fw_offset],((int *)(&in->size))+1,in->size);
                    fpga_fw_offset+=in->size;
                    
                    debug(DBGCLASS"::property: began fpga fw upload [%d; %d]\n",fpga_fw_size,fpga_fw_offset);
                }
                else
                    return kIOReturnBadArgument;            }
            else
                return kIOReturnBadArgument;
        }
            break;
        case KX_PROP_FPGA_FW_CONT+KX_PROP_GET:
        {
            prep_in(fpga_firmware_property);
            
            if(inStructSize>=8 && fpga_fw!=NULL)
            {
                if(in->size<=(fpga_fw_size-fpga_fw_offset) && in->size==(int)(inStructSize-sizeof(int)-sizeof(dword)))
                {
                    memcpy(&fpga_fw[fpga_fw_offset],((int *)(&in->size))+1,in->size);
                    fpga_fw_offset+=in->size;
                    
                    // debug(DBGCLASS"::property: continued fpga fw upload [%d; %d]\n",in->size,fpga_fw_offset);
                }
                else
                    return kIOReturnBadArgument;
            }
            else
                return kIOReturnBadArgument;
        }
            break;
        case KX_PROP_FPGA_FW_END+KX_PROP_GET:
        {
            prep_in(fpga_firmware_property);
            
            if(inStructSize>=8 && fpga_fw!=NULL)
            {
                if(in->size<=(fpga_fw_size-fpga_fw_offset) && in->size==(int)(inStructSize-sizeof(int)-sizeof(dword)))
                {
                    memcpy(&fpga_fw[fpga_fw_offset],((int *)(&in->size))+1,in->size);
                    fpga_fw_offset+=in->size;
                    
                    if(fpga_fw_offset==fpga_fw_size)
                    {
                        /*
                         dword crc=0x0055aa55;
                         for(int i=0;i<fpga_fw_size;i++)
                         crc=crc+fpga_fw[i]*i;
                         debug("=== ALL DONE, OK - %d; crc=%08x\n",fpga_fw_size,crc);
                         */
                        int ret=0;
                        
                        if(kx_upload_fpga_firmware(hw,fpga_fw,fpga_fw_size))
                            ret=kIOReturnError;
                        
                        IOFree(fpga_fw,fpga_fw_size);
                        fpga_fw=NULL;
                        fpga_fw_size=0;
                        fpga_fw_offset=0;
                        
                        return ret;
                    }
                }
                else
                    return kIOReturnBadArgument;
            }
            else
                return kIOReturnBadArgument;
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
                return kIOReturnBadArgument;
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
                return kIOReturnBadArgument;
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
                return kIOReturnBadArgument;
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
                        return kIOReturnBadArgument;
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
                    return kIOReturnBadArgument;
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
                    debug(DBGCLASS"::property: Invalid GSIF pointer\n");
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
                return kIOReturnBadArgument;
            }
        }
            break;
        case KX_PROP_DSP_REGISTER_ID+KX_PROP_SET:
        {
            prep_in(dsp_register_property);
            if(kx_set_dsp_register(hw,in->pgm,in->id,in->val)) // error
            {
                return kIOReturnBadArgument;
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
                return kIOReturnBadArgument;
            }
        }
            break;
        case KX_PROP_TRAM_ADDR_ID+KX_PROP_SET:
        {
            prep_in(dsp_register_property);
            if(kx_set_tram_addr(hw,in->pgm,in->id,in->val)) // error
            {
                return kIOReturnBadArgument;
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
                return kIOReturnBadArgument;
            }
        }
            break;
        case KX_PROP_TRAM_FLAG_ID+KX_PROP_SET:
        {
            prep_in(dsp_register_property);
            if(kx_set_tram_flag(hw,in->pgm,in->id,in->val)) // error
            {
                return kIOReturnBadArgument;
            }
        }
            break;
        case KX_PROP_RESET_MICROCODE+KX_PROP_GET:
        {
            prep_in(dword_property);
            prep_out(dword_property);
            if(in->reg==1)
            {
                debug(DBGCLASS"::property: Microcode reset request\n");
                kx_dsp_reset(hw);
                out->reg=0;
            }
            else { debug(DBGCLASS"::property: !!! Bad parameter for reset\n"); out->reg=(dword)-1; }
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
            // FIXME NOW !!reset_settings();
            
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
                
                /*
                 // FIXME
                 adapter->Wave[0]->kx3d_compat=0;
                 adapter->Wave[0]->kx3d_sp8ps=0;
                 */
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
                return kIOReturnBadArgument;
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
                return kIOReturnBadArgument;
        }
            break;
        case KX_PROP_TEXT2ID+KX_PROP_GET:
        {
            prep_out(kx_text2id);
            prep_in(kx_text2id);
            if(kx_translate_text2id(hw,in))
                return kIOReturnBadArgument;
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
                    debug(DBGCLASS"::property: !!! Bad cmd in prop::microcode()\n");
                    return kIOReturnBadArgument;
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
                return kIOReturnBadArgument;
            
            /*
             debug(DBGCLASS"::property: microcode: %d %d %d %d %d\n",
             offsetof(dsp_microcode,list),
             offsetof(dsp_microcode,pgm),
             offsetof(dsp_microcode,name),
             offsetof(dsp_microcode,guid),
             offsetof(dsp_microcode,info_size));
             */
        }
            break;
        case KX_PROP_MICROCODE_ENUM_NAME+KX_PROP_GET:
        {
            prep_in(microcode_enum_property);
            prep_out(microcode_enum_property);
            if(kx_enum_microcode(hw,in->name,&out->m))
                return kIOReturnBadArgument;
        }
            break;
        case KX_PROP_MICROCODE_SET_NAME+KX_PROP_GET:
        {
            prep_in(microcode_enum_property);
            prep_out(microcode_enum_property);
            if(kx_set_microcode_name(hw,in->pgm,in->name,in->m.flag))
                return kIOReturnBadArgument;
        }
            break;
        case KX_PROP_LOAD_MICROCODE+KX_PROP_GET:
        {
            char *m=(char *)((dword *)inStruct+1);
            if(m)
            {
                if(inStructSize<(sizeof(dword)+sizeof(dsp_microcode)+4+4+4))
                {
                    debug(DBGCLASS"::property: !!! Instance size invalid (load_microcode());\n");
                    return kIOReturnBadArgument;
                }
                if( (*(dword *)m!=LOAD_MICROCODE_MAGIC) ||
                   (*(dword *)(m+4+sizeof(dsp_microcode))!=LOAD_MICROCODE_MAGIC))
                {
                    debug(DBGCLASS"::property: !!! Bad magic 2 for load_microcode()\n");
                    return kIOReturnBadArgument;
                }
                dsp_microcode *tmp_m=(dsp_microcode *)(m+4);
                dsp_register_info *info=(dsp_register_info *)(m+4+sizeof(dsp_microcode)+4);
                dsp_code *code=(dsp_code *)(m+4+sizeof(dsp_microcode)+4+tmp_m->info_size);
                int *force_pgm_id=(int *)(m+4+sizeof(dsp_microcode)+4+tmp_m->info_size+tmp_m->code_size);
                if(*(dword *)(m+4+sizeof(dsp_microcode)+4+tmp_m->info_size+tmp_m->code_size+4)!=LOAD_MICROCODE_MAGIC)
                {
                    debug(DBGCLASS"::property: !!! Bad magic 3 for load_micrcode();\n");
                    return kIOReturnBadArgument;
                }
                
                if(inStructSize!=(4+sizeof(dsp_microcode)+4+tmp_m->info_size+tmp_m->code_size+4+4))
                {
                    debug(DBGCLASS"::property: !!! Bad instance size II load_microcode(): %d vs %d\n",
                          (int)inStructSize,(int)((4+sizeof(dsp_microcode)+4+tmp_m->info_size+tmp_m->code_size+4+4)));
                    return kIOReturnBadArgument;
                }
                
                int pgm=kx_load_microcode(hw,tmp_m->name,code,tmp_m->code_size,
                                          info,tmp_m->info_size,tmp_m->itramsize,tmp_m->xtramsize,
                                          tmp_m->copyright,
                                          tmp_m->engine,
                                          tmp_m->created,
                                          tmp_m->comment,
                                          tmp_m->guid,*force_pgm_id);
                if(pgm==0)
                    debug(DBGCLASS"::property: !!! -- load_microcode() failed\n");
                
                dword *out=(dword *)((dword *)outStruct+1);
                if(out)
                {
                    out[0]=(dword)pgm;
                }
                else return kIOReturnBadArgument;
            } else return kIOReturnBadArgument;
        }
            break;
        case KX_PROP_UPDATE_MICROCODE+KX_PROP_GET:
        {
            char *m=(char *)((dword *)inStruct+1);
            if(m)
            {
                if(inStructSize<(4+sizeof(dsp_microcode)+4+4+4+4))
                {
                    debug(DBGCLASS"::property: !!! Instance size invalid (update_microcode());\n");
                    return kIOReturnBadArgument;
                }
                if( (*(dword *)m!=UPDATE_MICROCODE_MAGIC) ||
                   (*(dword *)(m+4+sizeof(dsp_microcode))!=UPDATE_MICROCODE_MAGIC))
                {
                    debug(DBGCLASS"::property: !!! Bad magic 2 for update_microcode()\n");
                    return kIOReturnBadArgument;
                }
                dsp_microcode *tmp_m=(dsp_microcode *)(m+4);
                dsp_register_info *info=(dsp_register_info *)(m+4+sizeof(dsp_microcode)+4);
                dsp_code *code=(dsp_code *)(m+4+sizeof(dsp_microcode)+4+tmp_m->info_size);
                unsigned int flag=*(unsigned int *)(m+4+sizeof(dsp_microcode)+4+tmp_m->info_size+tmp_m->code_size);
                int pgm_id=*(int *)(m+4+sizeof(dsp_microcode)+4+tmp_m->info_size+tmp_m->code_size+4);
                if(*(dword *)(m+4+sizeof(dsp_microcode)+4+tmp_m->info_size+tmp_m->code_size+4+4)!=UPDATE_MICROCODE_MAGIC)
                {
                    debug(DBGCLASS"::property: !!! Bad magic 3 for update_micrcode();\n");
                    return kIOReturnBadArgument;
                }
                
                if(inStructSize-4!=(4+sizeof(dsp_microcode)+4+tmp_m->info_size+tmp_m->code_size+4+4+4))
                {
                    debug(DBGCLASS"::property: !!! Bad instance size II load_microcode(): %d vs %d\n",
                          (int)(inStructSize-4),(int)((4+sizeof(dsp_microcode)+4+tmp_m->info_size+tmp_m->code_size+4+4+4)));
                    return kIOReturnBadArgument;
                }
                
                int ret=kx_update_microcode(hw,pgm_id,tmp_m->name,code,tmp_m->code_size,
                                            info,tmp_m->info_size,tmp_m->itramsize,tmp_m->xtramsize,
                                            tmp_m->copyright,
                                            tmp_m->engine,
                                            tmp_m->created,
                                            tmp_m->comment,
                                            tmp_m->guid,flag);
                
                dword *out=(dword *)((dword *)outStruct+1);
                if(out)
                {
                    out[0]=(dword)ret;
                }
                else return kIOReturnBadArgument;
            } else return kIOReturnBadArgument;
        }
            break;
        case KX_PROP_GET_MICROCODE+KX_PROP_GET:
        {
            dword *m=((dword *)inStruct+1);
            if(m)
            {
                if(m[0]!=GET_MICROCODE_MAGIC)
                {
                    debug(DBGCLASS"::property: !!! Bad magic for get_microcode()\n");
                    return kIOReturnBadArgument;
                }
                int code_off=4;
                int info_off=code_off+(m[1]+1)/4+1;
                int last_off=info_off+(m[2]+1)/4+1;
                int pgm=(int)m[3];
                // dword *res=(dword *)((dword *)outStruct+1);
                
                dsp_code *code=(dsp_code *)&m[code_off];
                dsp_register_info *info=(dsp_register_info *)&m[info_off];
                
                if((int)(inStructSize-4)!=(last_off+1)*4)
                {
                    debug(DBGCLASS"::property: !!! get_microcode() instance too little (%d) (%d)\n",
                          inStructSize-4,
                          (last_off+1)*4);
                    return kIOReturnBadArgument;
                }
                if(m[last_off]!=GET_MICROCODE_MAGIC)
                {
                    debug(DBGCLASS"::property: !!! bad second magic for get_microcode()\n");
                    return kIOReturnBadArgument;
                }
                int ret=kx_get_microcode(hw,pgm,code,m[1],info,m[2]);
                if(ret)
                    debug(DBGCLASS"::property: !!! -- get_microcode failed (%d)\n",ret);
                
                dword *out=(dword *)((dword *)outStruct+1);
                if(out)
                {
                    out[0]=(dword)ret;
                    memcpy(&out[info_off],&m[info_off],m[2]);
                    memcpy(&out[code_off],&m[code_off],m[1]);
                    out[last_off]=GET_MICROCODE_MAGIC;
                }
                else return kIOReturnBadArgument;
            } else return kIOReturnBadArgument;
        }
            break;
        case KX_PROP_SOUNDFONT_LOAD+KX_PROP_GET:
        {
            prep_in(kx_sound_font);
            prep_out(kx_sound_font);
            out->id=kx_load_soundfont(hw,in);
        }
            break;
        case KX_PROP_SOUNDFONT_LOAD_SMPL+KX_PROP_GET:
        {
            prep_in(sf_load_sample_property);
            if(kx_load_soundfont_samples(hw,in))
                return kIOReturnBadArgument;
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
            prep_in(int);
            prep_out(sfHeader);
            
            if(kx_enum_soundfonts(hw,out,*in))
            {
                // error
                return kIOReturnBadArgument;
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
                    debug(DBGCLASS"::property: kXdirectSynth: no midi device opened\n");
            }
            if((cmd==0x80) || (cmd==0x90) || (cmd==0xa0) || (cmd==0xb0) || (cmd==0xe0))
            {
                if(hw->midi[in->synth_id&1])
                    kx_midi_play_buffer(hw->midi[in->synth_id&1],(byte *)&in->data,3);
                else
                    debug(DBGCLASS"::property: kXdirectSynth: no midi device opened\n");
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
            if(inStructSize<8)
            {
                debug(DBGCLASS"::property: !! get_connections(): valuesize=%d instancesize=%d\n",*outStructSize,inStructSize);
                return kIOReturnBadArgument;
            }
            
            int *out=(int *)((dword *)outStruct+1);
            int *pgm=(int *)((dword *)inStruct+1);
            int *size=(int *)((dword *)inStruct+2);
            if(*outStructSize-4==(dword)(*size))
            {
                if(kx_get_connections(hw,*pgm,(kxconnections *)out,*size))
                    return kIOReturnBadArgument;
            }
            else
            {
                debug(DBGCLASS"::property: !! incorrect size in get_connections() [%d; %d]\n",
                      *outStructSize,*size);
                return kIOReturnBadArgument;
            }
        }
            break;
        case KX_PROP_MICROCODE_ENUM_ALL+KX_PROP_GET:
        {
            dsp_microcode *mc=(dsp_microcode *)((dword *)outStruct+1);
            int *size=(int *)((dword *)inStruct+1);
            if(kx_enum_microcode(hw,mc,*size))
            {
                // error
                return kIOReturnBadArgument;
            }
        }
            break;
        case KX_PROP_ASIO_CAPS+KX_PROP_GET:
        {
            if(inStructSize-4==*outStructSize-4 && *outStructSize-4>=(sizeof(int)+sizeof(kx_caps)))
            {
                int *in_size=(int *)((dword *)inStruct+1);
                int *out_size=(int *)((dword *)outStruct+1);
                
                *out_size=*in_size;
                kx_caps *caps=(kx_caps *)(out_size+1);
                
                if(kx_get_device_caps(hw,caps,out_size))
                    return kIOReturnBadArgument;
            }
            else return kIOReturnBadArgument;
        }
            break;
        case KX_PROP_VOICE_INFO+KX_PROP_GET:
        {
            kx_voice_info *out=(kx_voice_info *)((dword *)outStruct+1);
            if(*outStructSize-4!=sizeof(kx_voice_info)*64)
            {
                debug(DBGCLASS"::property: invalid instances for kx_get_voice_info()\n");
                return kIOReturnBadArgument;
            }
            kx_get_voice_info(hw,KX_VOICE_INFO_USAGE,out);
        }
            break;
        case KX_PROP_SPECTRAL_INFO+KX_PROP_GET:
        {
            kx_voice_info *out=(kx_voice_info *)((dword *)outStruct+1);
            if(inStructSize-4!=sizeof(kx_voice_info)*64)
            {
                debug(DBGCLASS"::property: invalid instances for kx_get_voice_info()\n");
                return kIOReturnBadArgument;
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
            debug(DBGCLASS"::property: invalid property: %x; in_sz: %d out_sz: %d\n",(unsigned int)prop,(int)inStructSize,(int)*outStructSize);
            break;
    }
    
    return 0;
}

IOReturn kXAudioDevice::performPowerStateChange(IOAudioDevicePowerState oldPowerState, IOAudioDevicePowerState newPowerState, UInt32 *microsecondsUntilComplete)
{
    // debug(DBGCLASS"[%p]::performPowerStateChange: %d->%d\n",this,oldPowerState,newPowerState);
    
    if(newPowerState==kIOAudioDeviceSleep && (oldPowerState==kIOAudioDeviceActive || oldPowerState==kIOAudioDeviceIdle))
    {
        kx_set_power_state(hw,KX_POWER_SUSPEND);
    }
    
    IOReturn result = super::performPowerStateChange (oldPowerState, newPowerState, microsecondsUntilComplete);
    
    if((newPowerState==kIOAudioDeviceIdle || newPowerState==kIOAudioDeviceActive) && (oldPowerState==kIOAudioDeviceSleep))
    {
        kx_set_power_state(hw,KX_POWER_NORMAL);
    }
    
    return result;
}

