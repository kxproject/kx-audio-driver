// kX ASIO Driver
// Copyright (c) Eugene Gavrilov, 2001-2005.
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


#ifndef _KXAsio_
#define _KXAsio_

#include "asio/asiosys.h"
#include "rpc.h"
#include "rpcndr.h"
#ifndef COM_NO_WINDOWS_H
 #include <windows.h>
 #include "ole2.h"
#endif

#include "asio/combase.h"
#include "asio/iasiodrv.h"

#define MAX_STREAMS		16

class KXAsio : public IASIO, public CUnknown
{
public:
	#define KXASIO_LATENCY		1

	#define KXASIO_FREQ		2
	#define KXASIO_BPS		3
	#define KXASIO_N_PB_CHN		4
	#define KXASIO_N_REC_CHN	5
	#define KXASIO_ASIO_METHOD	6
	int get_config(int what);
	void set_config(int what,int val);

	volatile bool done; // break the loop in asio thread
        HANDLE ASIOThreadHandle;

	KXAsio(LPUNKNOWN pUnk, HRESULT *phr,int dev_num);
	~KXAsio();

	void recalc_latencies(int bf_=0);
	void recalc_sr(int sr_=0);

    int set_defaults(int save_settings=0);

	DECLARE_IUNKNOWN;

	// Factory method
	static CUnknown *CreateInstance0(LPUNKNOWN pUnk, HRESULT *phr);
	static CUnknown *CreateInstance1(LPUNKNOWN pUnk, HRESULT *phr);
	static CUnknown *CreateInstance2(LPUNKNOWN pUnk, HRESULT *phr);
	static CUnknown *CreateInstance3(LPUNKNOWN pUnk, HRESULT *phr);
	static CUnknown *CreateInstance4(LPUNKNOWN pUnk, HRESULT *phr);
	static CUnknown *CreateInstance5(LPUNKNOWN pUnk, HRESULT *phr);
	static CUnknown *CreateInstance6(LPUNKNOWN pUnk, HRESULT *phr);
	static CUnknown *CreateInstance7(LPUNKNOWN pUnk, HRESULT *phr);

	// IUnknown
	virtual HRESULT STDMETHODCALLTYPE NonDelegatingQueryInterface(REFIID riid,void **ppvObject);

	iKX *ikx;
	int device_num;

	ASIOBool init (void* sysRef);
	void getDriverName (char *name);	// max 32 bytes incl. terminating zero
	long getDriverVersion ();
	void getErrorMessage (char *string);	// max 128 bytes incl.

	ASIOError start ();
	ASIOError stop ();

	ASIOError getChannels (long *numInputChannels, long *numOutputChannels);
	ASIOError getLatencies (long *inputLatency, long *outputLatency);
	ASIOError getBufferSize (long *minSize, long *maxSize,
		long *preferredSize, long *granularity);

	ASIOError canSampleRate (ASIOSampleRate sampleRate);
	ASIOError getSampleRate (ASIOSampleRate *sampleRate);
	ASIOError setSampleRate (ASIOSampleRate sampleRate);
	ASIOError getClockSources (ASIOClockSource *clocks, long *numSources);
	ASIOError setClockSource (long index);

	ASIOError getSamplePosition (ASIOSamples *sPos, ASIOTimeStamp *tStamp);
	ASIOError getChannelInfo (ASIOChannelInfo *info);

	ASIOError createBuffers (ASIOBufferInfo *bufferInfos, long numChannels,
		long bufferSize, ASIOCallbacks *callbacks);
	ASIOError disposeBuffers ();

	ASIOError controlPanel ();
	ASIOError future (long selector, void *opt);
	ASIOError outputReady ();

	void bufferSwitch ();
	long getMilliSeconds () {return milliSeconds;}

	long toggle;
	long blockFrames;

	bool active, started;
	double sampleRate;

        // config
        int num_inputs,num_outputs;
	int bps;
        // sampling rate is already declared

	void bufferSwitchX ();

	int ASIOThread(void);

	__int64 samplePosition;
	ASIOCallbacks *callbacks;
	ASIOTime asioTime;
	void *inputBuffers[MAX_STREAMS * 2];
	void *outputBuffers[MAX_STREAMS * 2];
	int inputBufferSizes[MAX_STREAMS * 2];
	int outputBufferSizes[MAX_STREAMS * 2];

	long inMap[MAX_STREAMS];
	long outMap[MAX_STREAMS];
	long inputLatency;
	long outputLatency;
	long activeInputs;
	long activeOutputs;

	long milliSeconds;
	bool timeInfoMode;
	bool tcRead;
	char errorMessage[128];
};

#endif
