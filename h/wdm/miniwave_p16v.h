// kX WDM Audio Driver
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


// p16v / alice3 

#ifndef _MINIWAVE_P16V_H_
#define _MINIWAVE_P16V_H_

class CMiniportWaveStreamHQ;

class CMiniportWaveHQ
:   public IMiniportWaveCyclic,
    public IPowerNotify,
    public IPinCount,
    public CUnknown
{
public:
    SAFE_DESTRUCTORS

    PADAPTERCOMMON      AdapterCommon;      // Adapter common object.
    PPORTWAVECYCLIC     Port;               // Callback interface.

    DECLARE_STD_UNKNOWN();
    DEFINE_STD_CONSTRUCTOR(CMiniportWaveHQ);

    ~CMiniportWaveHQ();

    IMP_IPowerNotify;
    DEVICE_POWER_STATE      PowerState;

    IMP_IPinCount;

    int magic;
    kx_hw *hw;

    CMiniportWaveStreamHQ *pb_stream;

    IMP_IMiniportWaveCyclic;

    static NTSTATUS PropertyCpuResources(IN      PPCPROPERTY_REQUEST PropertyRequest);
    static NTSTATUS PropertyVolume(IN      PPCPROPERTY_REQUEST PropertyRequest);
    static NTSTATUS PropertySamplingRate(IN      PPCPROPERTY_REQUEST PropertyRequest);
};

class CMiniportWaveStreamHQ
:   public IMiniportWaveCyclicStream,
    public IDrmAudioStream,
    public CUnknown
{
public:
    SAFE_DESTRUCTORS
    DECLARE_STD_UNKNOWN();
    DEFINE_STD_CONSTRUCTOR(CMiniportWaveStreamHQ);

    ~CMiniportWaveStreamHQ();

    CMiniportWaveHQ	*Miniport;       // Miniport that created us.
    PSERVICEGROUP       ServiceGroup;       // For notification.
    myKSDATAFORMAT	DataFormat2;
    POOL_TYPE		PoolType;
    ULONG		Pin;

    IMP_IMiniportWaveCyclicStream;

    int magic;
    kx_hw *hw;

    virtual void my_isr(void);

    PDMACHANNEL source_channel;
    int source_buffer_size;

    int vol1[MAX_TOPOLOGY_CHANNELS];
    int vol2[MAX_TOPOLOGY_CHANNELS];

    int Open();
    int Parse();

    int nchannels,cur_freq,bps;
    
    int opened;

    // Current state/position
    int State;

    int capture;

    virtual NTSTATUS Init(
    		  IN CMiniportWaveHQ *Miniport_,
		  IN POOL_TYPE PoolType,
    		  IN ULONG Pin_,
    		  IN PKSDATAFORMAT DataFormat_,
    		  OUT PDMACHANNEL *DmaChannel_,
    		  OUT PSERVICEGROUP *ServiceGroup_,
    		  IN int Capture_);

    IMP_IDrmAudioStream;
    static NTSTATUS PropertyVolume(IN      PPCPROPERTY_REQUEST PropertyRequest);
    static NTSTATUS PropertySamplingRate(IN      PPCPROPERTY_REQUEST PropertyRequest);

    int update_volume();
};

#endif
