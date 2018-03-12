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


#include "AudioDevice.h"
#include "AudioEngine.h"

#include "IOAudioBlitterLib.h"

#undef debug
#include "cedebug.h"

#ifndef KX_INTERNAL
	#error invalid configuration
#endif

IOReturn kXAudioEngine::clipOutputSamples(const void *mixBuf, void *sampleBuf, UInt32 firstSampleFrame, UInt32 numSampleFrames, const IOAudioStreamFormat *streamFormat, IOAudioStream *audioStream)
{
	// just some checks
	#ifdef DEBUGGING
	int i;
	int n_voice=-1;
	int sz=-1;
	for(i=0;i<KX_NUMBER_OF_VOICES;i++)
	{
		if(hw->voicetable[i].buffer.addr==sampleBuf)
		{
			sz=hw->voicetable[i].buffer.size;
			n_voice=i;
			break;
		}
	}
	if(i==KX_NUMBER_OF_VOICES)
	{
		debug("!! kXAudioEngine[%p]::clipOutputSamples: invalid buffer [%08x]\n",this,sampleBuf);
		return 0;
	}
	if(firstSampleFrame+numSampleFrames>n_frames || (firstSampleFrame+numSampleFrames)*bps/8>sz)
	{
		debug("!! kXAudioEngine[%p]::clipOutputSamples: invalid offset [buf: %d, %08x first: %d num: %d [%04x, %04x]\n",this,i,
			  sampleBuf,
			  firstSampleFrame,numSampleFrames,
			  firstSampleFrame*1*bps/8,
			  (firstSampleFrame+numSampleFrames)*1*bps/8);
		return 0;
	}
	#endif
	
	int frsamples=firstSampleFrame * streamFormat->fNumChannels;
    UInt8 *outputBuf = &(((UInt8 *)sampleBuf)[frsamples * streamFormat->fBitWidth / 8]);
	float *fMixBuf = ((float *)mixBuf) + frsamples;
    
    // Calculate the number of actual samples to convert
    int num_samples = numSampleFrames * streamFormat->fNumChannels;

	#if 1
	if(bps==32)
		Float32ToNativeInt32_X86(fMixBuf, (SInt32 *)outputBuf, num_samples);
	else
		if(bps==16)
			Float32ToNativeInt16_X86(fMixBuf, (SInt16 *)outputBuf, num_samples);
	#else
	{
		SInt32 *out=(SInt32 *)outputBuf;
		for(int i=0;i<frsamples;i++)
		{
			*out=(SInt32)(*fMixBuf*(float)0x7fffffff);
		}
	}
	#endif
	
	return kIOReturnSuccess;
}

IOReturn kXAudioEngine::convertInputSamples(const void *sampleBuf, void *destBuf, UInt32 firstSampleFrame, UInt32 numSampleFrames, const IOAudioStreamFormat *streamFormat, IOAudioStream *audioStream)
{
	#ifdef DEBUGGING
	 // just some checks
	int i;
	int sz=-1;
	for(i=0;i<KX_NUMBER_OF_VOICES;i++)
	{
		if(hw->voicetable[i].buffer.addr==sampleBuf)
		{
			sz=hw->voicetable[i].buffer.size;
			break;
		}
	}
	if(i==KX_NUMBER_OF_VOICES)
	{
		debug("!! kXAudioEngine[%p]::convertInputSamples: invalid buffer [%08x] audio_stream: %08x stream_format: %08x\n",this,sampleBuf,audioStream,streamFormat);
		return 0;
	}
	
	if(firstSampleFrame+numSampleFrames>n_frames || (firstSampleFrame+numSampleFrames)*bps/8>sz)
	{
		debug("!! kXAudioEngine[%p]::convertInputSamples: invalid offset [%08x first: %d num: %d [%04x, %04x]\n",this,
			  sampleBuf,
			  firstSampleFrame,numSampleFrames,
			  firstSampleFrame*1*bps/8,
			  (firstSampleFrame+numSampleFrames)*1*bps/8);
		return 0;
	}
	#endif
	
    int num_samples = numSampleFrames * streamFormat->fNumChannels;
	UInt8 *inputBuf = &(((UInt8 *)sampleBuf)[firstSampleFrame * streamFormat->fNumChannels * streamFormat->fBitWidth / 8]);

	#if 1
	if(bps==32)
		NativeInt32ToFloat32_X86((SInt32 *)inputBuf,(Float32 *)destBuf,num_samples);
	else
		if(bps==16)
			NativeInt16ToFloat32_X86((SInt16 *)inputBuf,(Float32 *)destBuf,num_samples);
	#endif
    return kIOReturnSuccess;
}
