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


#include "AudioDevice.h"
#include "AudioEngine.h"

#define DBGCLASS "kXAudioEngine"

#undef debug
#include "cedebug.h"

#define super IOAudioEngine
OSDefineMetaClassAndStructors(kXAudioEngine, IOAudioEngine)

    // ----- Configuration ------

bool kXAudioEngine::init(kx_hw *hw_)
{
    bool result = false;
    
    dump_addr();
    
    debug("kXAudioEngine[%p]::init\n", this);
    
    hw = hw_;
    
    if (!super::init(NULL))
    {
        goto Done;
    }
    
    hw->initialized|=KX_ENGINE_INITED;
    
    result = true;
    
    sampling_rate=48000;
    
    if(hw->is_10k2)
		bps=32;
    else
		bps=16;
    
    n_channels=8; // should be <= MAX_CHANNELS_

		// Use 4 times the buffer size to prevent some sort of underrun on
		// Mavericks (related to Timer Coalescing?) causing playback crackle
		// until streams are restart (e.g. stop/start playback)
    n_frames = (int)(4 * (hw->mtr_buffer.size * 8 / bps / n_channels));

    debug("kXAudioEngine[%p]::init - n_frames=%d\n", this, n_frames);
    
    is_running=0;
    
    bzero(out_streams, sizeof(out_streams));
    bzero(in_streams, sizeof(in_streams));
    
		// configure kX to ASIO-style I/O
    if(hw)
    {
		hw->asio_notification_krnl.pb_buf=-1;
		hw->asio_notification_krnl.rec_buf=-1;
		hw->asio_notification_krnl.toggle=-1;
		hw->asio_notification_krnl.asio_method=KXASIO_METHOD_SEND_EVENT; // notify via deferred, but catch up directly in the OSX IRQ handler
		hw->asio_notification_krnl.n_voice=-1;
		hw->asio_notification_krnl.semi_buff=0;
		hw->asio_notification_krnl.active=0;
		hw->asio_notification_krnl.cur_pos=0;
		hw->asio_notification_krnl.kevent=this;
		
		hw->cur_asio_in_bps=bps;
    }
    
Done:
    
    return result;
}

bool kXAudioEngine::initHardware(IOService *provider)
{
    bool result = false;
    IOAudioSampleRate initialSampleRate;
    IOAudioStream *audioStream;
    
    debug("kXAudioEngine[%p]::initHardware(%p)\n", this, provider);
    
    if (!super::initHardware(provider)) {
        goto Done;
    }
    
		// Setup the initial sample rate for the audio engine
    initialSampleRate.whole = sampling_rate;
    initialSampleRate.fraction = 0;
    
    char device_name[KX_MAX_STRING];
    strncpy(device_name,"kX ",KX_MAX_STRING);
    strncat(device_name,hw->card_name,KX_MAX_STRING);
    
    setDescription(device_name);
    
    setSampleRate(&initialSampleRate);
    
    setClockDomain(); // =kIOAudioNewClockDomain
    
		// calculate kx_sample_offset
    
    if(hw->is_10k2)
    {
			// setSampleOffset(28); // 28 samples
		setSampleLatency(28+1);
    }
    else
    {
			// setSampleOffset(32); // 32 samples
		setSampleLatency(32+1);
    }
    
		// Allocate our input and output buffers
    for(int i=0;i<n_channels;i++)
    {
			// Create an IOAudioStream for each buffer and add it to this audio engine
		audioStream = createNewAudioStream(i,kIOAudioStreamDirectionOutput, 1*n_frames*bps/8);
		if (!audioStream) {
			goto Done;
		}
		
		addAudioStream(audioStream);
		out_streams[i]=audioStream;
    }
    
		// recording
    audioStream = createNewAudioStream(0,kIOAudioStreamDirectionInput, n_channels*n_frames*bps/8);
    if (!audioStream) {
		goto Done;
    }
    
    addAudioStream(audioStream);
    in_streams[0]=audioStream;
    
    dump_addr();
    
		// Set the number of sample frames in each buffer
    setNumSampleFramesPerBuffer(n_frames);
    
    result = true;
    
Done:
    if(result==false)
		free_all();
    
    return result;
}

void kXAudioEngine::free()
{
    debug("kXAudioEngine[%p]::free()\n", this);
    
    free_all();
    
    super::free();
    
    hw=NULL;
}

void kXAudioEngine::stop(IOService *provider)
{
    debug("kXAudioEngine[%p]::stop(%p) - %s\n", this, provider, is_running?"[still running!]":"");
    
		// Add code to shut down hardware (beyond what is needed to simply stop the audio engine)
		// There may be nothing needed here
    free_all();
    
    super::stop(provider);
}

void kXAudioEngine::free_all()
{
    debug("kXAudioEngine[%p]::free_all() - %s\n",this,is_running?"[still running!]":"");
    if(is_running)
		stopAudioEngine();
    
		// free buffers
    setNumSampleFramesPerBuffer(0);
    
    for(int i=0;i<n_channels;i++)
    {
		if(out_streams[i])
		{
			out_streams[i]->setSampleBuffer (NULL, 0);
			out_streams[i]->release();
			out_streams[i]=NULL;
		}
		if(in_streams[i])
		{
			in_streams[i]->setSampleBuffer (NULL, 0);
			in_streams[i]->release();
			in_streams[i]=NULL;
		}
		
		if(hw)
		{
			freeAudioStream(i,kIOAudioStreamDirectionInput);
			freeAudioStream(i,kIOAudioStreamDirectionOutput);
		}
    }
    
    if(hw)
    {
		if((hw->initialized&KX_ENGINE_INITED) && !(hw->initialized&KX_DEVICE_INITED))
		{
			debug("kXAudioEngine[%p]::free_all() - close iKX interface\n",this);
			kx_close(&hw);
			hw=0;
		}
		else
		{
			debug("kXAudioEngine[%p]::free_all() - dereference iKX interface\n",this);
			hw->initialized&=(~KX_ENGINE_INITED);
			hw=NULL;
		}
    }
    else
		debug("kXAudioEngine[%p]::free_all() - iKX interface already closed\n",this);
}

IOBufferMemoryDescriptor *kXAudioEngine::my_alloc_contiguous(mach_vm_size_t size, void **addr, dword *phys)
{
    if(size<PAGE_SIZE)
		size=PAGE_SIZE;
    
#ifdef DEBUGGING
    size += 2 * PAGE_SIZE;
#endif	
		//void *addr=IOMallocContiguous(size+PAGE_SIZE+PAGE_SIZE,alignment,phys);	
    
    mach_vm_address_t mask = 0x000000007FFFFFFFULL & ~(PAGE_SIZE - 1);
    
    IOBufferMemoryDescriptor *desc =
    IOBufferMemoryDescriptor::inTaskWithPhysicalMask(
													 kernel_task,
													 kIODirectionInOut | kIOMemoryPhysicallyContiguous,
													 size,
													 mask);
    
    if(desc)
    {
		desc->prepare();
		
		IOPhysicalAddress pa = desc->getPhysicalAddress();
		
		if (pa & ~mask)
			debug("kXAudioEngine[%p]::my_alloc_contiguous() - memory misaligned or beyond 2GB limit (%p)\n", this, (void *)pa);
		
		*phys = (dword)pa;
		*addr = desc->getBytesNoCopy();
		
#ifdef DEBUGGING
		memset(addr,0x11,PAGE_SIZE);
		memset((UInt8 *)addr+PAGE_SIZE+size,0x22,PAGE_SIZE);
		
		*((UInt8 *)addr) += PAGE_SIZE;
		*phys += PAGE_SIZE;
#endif
    }
    else
		debug("kXAudioEngine[%p]::my_alloc_contiguous() - allocation failed\n",this);
    
    return desc;
}

void kXAudioEngine::my_free_contiguous(IOBufferMemoryDescriptor *desc, mach_vm_size_t size)
{
#ifdef DEBUGGING
    if(size<PAGE_SIZE)
		size=PAGE_SIZE;
    
		//	addr=((UInt8 *)addr)-PAGE_SIZE;
    UInt8 *buff = (UInt8 *)desc->getBytesNoCopy();
    
		// check
    for(int i=0;i<PAGE_SIZE;i++)
    {
		if(buff[i]!=0x11)
		{
			debug("!! kXAudioEngine[%p]::my_free_contiguous: buffer was damaged before! offset: %d\n",this,i);
			break;
		}
		if(buff[i+PAGE_SIZE+size]!=0x22)
		{
			debug("!! kXAudioEngine[%p]::my_free_contiguous: buffer was damaged after! offset: %d\n",this,i);
			break;
		}
    }
#endif
    desc->release();
}

void kXAudioEngine::freeAudioStream(int chn,IOAudioStreamDirection direction)
{
    if(direction==kIOAudioStreamDirectionOutput)
    {
		for(int i=0;i<KX_NUMBER_OF_VOICES;i++)
		{
			if(hw->voicetable[i].asio_id==this && hw->voicetable[i].asio_channel==(dword)chn)
			{
				kx_wave_close(hw,i);
				
				hw->voicetable[i].asio_id=0;
				hw->voicetable[i].asio_channel=0xffffffff;
				
				hw->voicetable[i].asio_mdl=0;
				hw->voicetable[i].asio_kernel_addr=0;
				hw->voicetable[i].asio_user_addr=0;
				
				if(hw->voicetable[i].buffer.desc && hw->voicetable[i].buffer.addr && hw->voicetable[i].buffer.size!=0)
				{
						//my_free_contiguous(hw->voicetable[i].buffer.addr,hw->voicetable[i].buffer.size);
					my_free_contiguous(hw->voicetable[i].buffer.desc, hw->voicetable[i].buffer.size);
					bzero(&(hw->voicetable[i].buffer), sizeof(hw->voicetable[i].buffer));
				}
			}
		}
    }
    else
    {
		if(direction==kIOAudioStreamDirectionInput)
		{
			if (hw->mtr_buffer.desc && hw->mtr_buffer.addr && hw->mtr_buffer.size != 0)
			{
				my_free_contiguous(hw->mtr_buffer.desc, hw->mtr_buffer.size);
				bzero(&(hw->mtr_buffer), sizeof(hw->mtr_buffer));
			}
		}
    }
}

IOAudioStream *kXAudioEngine::createNewAudioStream(int chn,IOAudioStreamDirection direction, UInt32 sampleBufferSize)
{
    IOAudioStream *audioStream = new IOAudioStream;
    
    if (audioStream) 
    {
        if (!audioStream->initWithAudioEngine(this, direction, 1)) 
		{
            audioStream->release();
        } else {
            IOAudioSampleRate rate;
			
			void *sampleBuffer=NULL;
			
			IOAudioStreamFormat format = {
				1,												// num channels
				kIOAudioStreamSampleFormatLinearPCM,			// sample format
				kIOAudioStreamNumericRepresentationSignedInt,	// numeric format
				bps,											// bit depth
				bps,											// bit width
				kIOAudioStreamAlignmentHighByte,				// high byte aligned - unused because bit depth == bit width
				kIOAudioStreamByteOrderLittleEndian,			// little endian
				true,											// format is mixable
				0												// driver-defined tag - unused by this driver
			};
			
			kx_voice_buffer buffer;
			bzero(&buffer, sizeof(buffer));
			
			buffer.desc = my_alloc_contiguous(sampleBufferSize, &(buffer.addr), &(buffer.physical));
			
			if (!buffer.desc)
				return NULL;
			
			buffer.size=sampleBufferSize;
			buffer.that=this;
			buffer.notify=-n_frames;
			
			sampleBuffer=buffer.addr;
			
				// allocate memory:
			if(direction==kIOAudioStreamDirectionOutput)
			{				
				int need_notifier=VOICE_OPEN_NOTIMER;
				if(chn==0) // first voice?
					need_notifier|=VOICE_OPEN_NOTIFY; // half/full buffer
				
				int mapping[]=
				{ //2,3,4,5,6,7,8,9 - kX:  front, rear, center+lfe, back
				  //1,2,3,4,5,6,7,8 - OSX: front, center+lfe, rear, back
					2,3,6,7,4,5,8,9 };
					// wave 2/3 - front
					// wave 6/7 - center+lfe
					// wave 4/5 - rear
					// 8/9 - rear center/etc.
				
				int i=kx_allocate_multichannel(hw,bps,sampling_rate,need_notifier,&buffer,DEF_ASIO_ROUTING+mapping[chn]); // start with 2/3
				
				if(i>=0)
				{
					hw->voicetable[i].asio_id=this;
					hw->voicetable[i].asio_mdl=0;
					hw->voicetable[i].asio_user_addr=0;
					hw->voicetable[i].asio_kernel_addr=0;
					hw->voicetable[i].asio_channel=chn;
					
					if(chn==0) // first?
					{
						hw->asio_notification_krnl.n_voice=i;
						hw->asio_notification_krnl.semi_buff=(hw->voicetable[i].param.endloop-hw->voicetable[i].param.startloop)/2;
					}
				}
			}
			else
				if(direction==kIOAudioStreamDirectionInput)
				{					
					format.fNumChannels = 8;
					
					hw->mtr_buffer.desc = buffer.desc;
					hw->mtr_buffer.addr = buffer.addr;
					hw->mtr_buffer.size = buffer.size;
					hw->mtr_buffer.dma_handle = buffer.physical; 
					
					//kx_writeptr(hw,FXIDX,0,0);

					//debug("createNewAudioStream FXBA:\n");
					//kx_writeptr_prof(hw, FXBA, 0, hw->mtr_buffer.dma_handle);
					kx_writeptr(hw, FXBA, 0, hw->mtr_buffer.dma_handle);

					dword ch = (1 << (format.fNumChannels * 2)) - 1;	// 24bit needs 2 physical channels
					debug("createNewAudioStream FXWCH/FXWC_K1: %x\n", ch);

					if(hw->is_10k2)
					{
						kx_writeptr(hw,FXWCL, 0, 0);
						kx_writeptr(hw,FXWCH, 0, ch);
					}
					else
						kx_writeptr(hw,FXWC_K1, 0, ch << 16);
				}
			
				// As part of creating a new IOAudioStream, its sample buffer needs to be set
				// It will automatically create a mix buffer should it be needed
			if(sampleBuffer && sampleBufferSize)
				audioStream->setSampleBuffer(sampleBuffer, sampleBufferSize);
            
				// This device only allows a single format and a choice of 2 different sample rates
            rate.fraction = 0;
            rate.whole = sampling_rate;
            audioStream->addAvailableFormat(&format, &rate, &rate);
            
				// Finally, the IOAudioStream's current format needs to be indicated
            audioStream->setFormat(&format);
        }
    }
    
    return audioStream;
}

IOReturn kXAudioEngine::performAudioEngineStart()
{
		// debug("kXAudioEngine[%p]::performAudioEngineStart() - %d\n", this,is_running);
    
		// When performAudioEngineStart() gets called, the audio engine should be started from the beginning
		// of the sample buffer.  Because it is starting on the first sample, a new timestamp is needed
		// to indicate when that sample is being read from/written to.  The function takeTimeStamp() 
		// is provided to do that automatically with the current time.
		// By default takeTimeStamp() will increment the current loop count in addition to taking the current
		// timestamp.  Since we are starting a new audio engine run, and not looping, we don't want the loop count
		// to be incremented.  To accomplish that, false is passed to takeTimeStamp(). 
    
    dword low=0,high=0;
    int first=-1;
    
    for(int i=0;i<KX_NUMBER_OF_VOICES;i++)
    {
		if(hw->voicetable[i].usage&VOICE_USAGE_ASIO && hw->voicetable[i].asio_id==this)
		{
			if(i>=32)
				high|=(1<<(i-32));
			else
				low|=(1<<i);
			if(first==-1)
				first=i;
		}
    }
    
    if(first!=-1)
    {
			// zero sample position, too
		for(int i=0;i<32;i++)
		{
			if(low&(1<<i))
				kx_wave_set_position(hw,i,0);
			if(high&(1<<i))
				kx_wave_set_position(hw,i+32,0);
		}
		
			// debug("kXAudioEngine[%p]::performAudioEngineStart: start audio [%d; %x %x]\n",this,first,(unsigned)low,(unsigned)high);
		kx_wave_start_multiple(hw,first,low,high);
    }
    
		// recording
    dword sz;
    switch(hw->mtr_buffer.size)
    {
        case 384: sz=ADCBS_BUFSIZE_384; break;
        case 448: sz=ADCBS_BUFSIZE_448; break;
        case 512: sz=ADCBS_BUFSIZE_512; break;
        case 640: sz=ADCBS_BUFSIZE_640; break;
        case 768: sz=ADCBS_BUFSIZE_768; break;
        case 896: sz=ADCBS_BUFSIZE_896; break;
        case 1024: sz=ADCBS_BUFSIZE_1024; break;
        case 1280: sz=ADCBS_BUFSIZE_1280; break;
        case 1536: sz=ADCBS_BUFSIZE_1536; break;
        case 1792: sz=ADCBS_BUFSIZE_1792; break;
        case 2048: sz=ADCBS_BUFSIZE_2048; break;
        case 2560: sz=ADCBS_BUFSIZE_2560; break;
        case 3072: sz=ADCBS_BUFSIZE_3072; break;
        case 3584: sz=ADCBS_BUFSIZE_3584; break;
        case 4096: sz=ADCBS_BUFSIZE_4096; break;
        case 5120: sz=ADCBS_BUFSIZE_5120; break;
        case 6144: sz=ADCBS_BUFSIZE_6144; break;
        case 7168: sz=ADCBS_BUFSIZE_7168; break;
        case 8192: sz=ADCBS_BUFSIZE_8192; break;
        case 10240: sz=ADCBS_BUFSIZE_10240; break;
        case 12288: sz=ADCBS_BUFSIZE_12288; break;
        case 14366: sz=ADCBS_BUFSIZE_14366; break;
        case 16384: sz=ADCBS_BUFSIZE_16384; break;
        case 20480: sz=ADCBS_BUFSIZE_20480; break;
        case 24576: sz=ADCBS_BUFSIZE_24576; break;
        case 28672: sz=ADCBS_BUFSIZE_28672; break;
        case 32768: sz=ADCBS_BUFSIZE_32768; break;
        case 40960: sz=ADCBS_BUFSIZE_40960; break;
        case 49152: sz=ADCBS_BUFSIZE_49152; break;
        case 57344: sz=ADCBS_BUFSIZE_57344; break;
        default:
        case 65536: sz=ADCBS_BUFSIZE_65536; break;
    }
    
    //debug("performAudioEngineStart FXBS:\n");
    //kx_writeptr_prof(hw, FXBS, 0, sz);
    kx_writeptr(hw, FXBS, 0, sz);
    
    hw->asio_notification_krnl.active=1;
    
    is_running=1;
    
    takeTimeStamp(false);
    
    return kIOReturnSuccess;
}

IOReturn kXAudioEngine::performAudioEngineStop()
{
    dump_addr();
    
		// debug("kXAudioEngine[%p]::performAudioEngineStop() - %d\n", this,is_running);
    
    hw->asio_notification_krnl.active=0;
    
    dword low=0,high=0;
    int first=-1;
    
    for(int i=0;i<KX_NUMBER_OF_VOICES;i++)
    {
		if(hw->voicetable[i].usage&VOICE_USAGE_ASIO && hw->voicetable[i].asio_id==this)
		{
			if(i>=32)
				high|=(1<<(i-32));
			else
				low|=(1<<i);
			if(first==-1)
				first=i;
		}
    }
    
    if(first!=-1)
    {
			// debug("kXAudioEngine[%p]::performAudioEngineStop: stop audio [%d; %x %x]\n",this,first,(unsigned)low,(unsigned)high);
		kx_wave_stop_multiple(hw,first,low,high);
		
			// zero sample position, too
		for(int i=0;i<32;i++)
		{
			if(low&(1<<i))
				kx_wave_set_position(hw,i,0);
			if(high&(1<<i))
				kx_wave_set_position(hw,i+32,0);
		}
    }
    
		// recording
    //debug("performAudioEngineStop FXBS:\n");
    //kx_writeptr_prof(hw, FXBS, 0, 0);
    kx_writeptr(hw, FXBS, 0, 0);
    
    is_running=0;
    
    return kIOReturnSuccess;
}

UInt32 kXAudioEngine::getCurrentSampleFrame()
{
		// In order for the erase process to run properly, this function must return the current location of
		// the audio engine - basically a sample counter
		// It doesn't need to be exact, but if it is inexact, it should err towards being before the current location
		// rather than after the current location.  The erase head will erase up to, but not including the sample
		// frame returned by this function.  If it is too large a value, sound data that hasn't been played will be 
		// erased.
    
    if(hw && hw->asio_notification_krnl.n_voice>=0)
    {
		kx_get_asio_position(hw,1); // this simply updates hw->asio_notification_krnl->cur_pos, we ignore return code
									// cur_pos is in samples, for 24/32 streams it is in 16-bit bytes, that is, again, samples
		
			// sanity check
		dword pos=hw->asio_notification_krnl.cur_pos;
		if(pos>=(dword)n_frames)
		{
			debug("!! kXAudioEngine[%p]::getCurrentSampleFrame: invalid pos [%x, %d]\n",this,(unsigned)pos,n_frames);
			pos=0;
		}
		
		return pos; // this is in number of samples
    }
    
    debug("!! kXAudioEngine[%p]::getCurrentSampleFrame: hw = 0\n",this);
    return 0;
}

IOReturn kXAudioEngine::performFormatChange(IOAudioStream *audioStream, const IOAudioStreamFormat *newFormat, const IOAudioSampleRate *newSampleRate)
{
    debug("kXAudioEngine[%p]::peformFormatChange(%p, %p, %p)\n", this, audioStream, newFormat, newSampleRate);
    
    if (newSampleRate)
    {
        switch (newSampleRate->whole) {
            case 48000:
                debug("\t-> 48kHz selected\n");
                
					// Add code to switch hardware to 48kHz
                break;
            default:
					// This should not be possible since we only specified 44100 and 48000 as valid sample rates
                debug("\t Internal Error - unknown sample rate selected.\n");
                break;
        }
    }
    
    dump_addr();
    
    return kIOReturnSuccess;
}


void kXAudioEngine::dump_addr(void)
{
#if 0
    IOLog("== functions: init: %p initHardware: %p free: %p stop: %p free_all: %p alloc: %p free: %p freeAS: %p\n"
		  "== functions: create: %p perfStart: %p perfStop: %p getCW: %p fmt: %p\n"
		  "== functions: clip: %p clip2: %p\n",
		  OSMemberFunctionCast(void *,this,&kXAudioEngine::init),
		  OSMemberFunctionCast(void *,this,&kXAudioEngine::initHardware),
		  OSMemberFunctionCast(void *,this,&kXAudioEngine::free),
		  OSMemberFunctionCast(void *,this,&kXAudioEngine::stop),
		  OSMemberFunctionCast(void *,this,&kXAudioEngine::free_all),
		  OSMemberFunctionCast(void *,this,&kXAudioEngine::my_free_contiguous),
		  OSMemberFunctionCast(void *,this,&kXAudioEngine::my_alloc_contiguous),
		  OSMemberFunctionCast(void *,this,&kXAudioEngine::freeAudioStream),
		  OSMemberFunctionCast(void *,this,&kXAudioEngine::createNewAudioStream),
		  OSMemberFunctionCast(void *,this,&kXAudioEngine::performAudioEngineStart),
		  OSMemberFunctionCast(void *,this,&kXAudioEngine::performAudioEngineStop),
		  OSMemberFunctionCast(void *,this,&kXAudioEngine::getCurrentSampleFrame),
		  OSMemberFunctionCast(void *,this,&kXAudioEngine::performFormatChange),
		  OSMemberFunctionCast(void *,this,&kXAudioEngine::clipOutputSamples),
		  OSMemberFunctionCast(void *,this,&kXAudioEngine::convertInputSamples)
		  );
#endif	  
}
