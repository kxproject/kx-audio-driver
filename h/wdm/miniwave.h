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


#ifndef _MINIWAVE_H_
#define _MINIWAVE_H_

class kSource;
class kListener;

class CMiniportWave
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
    DEFINE_STD_CONSTRUCTOR(CMiniportWave);

    ~CMiniportWave();

    IMP_IPowerNotify;
    DEVICE_POWER_STATE      PowerState;

    IMP_IPinCount;

    int wave_number;

    int magic;
    kx_hw *hw;

    IMP_IMiniportWaveCyclic;

    static NTSTATUS PropertyChannelConfig(IN      PPCPROPERTY_REQUEST PropertyRequest);
    static NTSTATUS PropertyCpuResources(IN      PPCPROPERTY_REQUEST PropertyRequest);
    static NTSTATUS PropertyVolume(IN      PPCPROPERTY_REQUEST PropertyRequest);
    static NTSTATUS PropertyMixLevel(IN      PPCPROPERTY_REQUEST PropertyRequest);
    static NTSTATUS PropertyMixLevelCaps(IN      PPCPROPERTY_REQUEST PropertyRequest);

    static NTSTATUS PropertySpeakerGeometry(IN      PPCPROPERTY_REQUEST PropertyRequest);
    static NTSTATUS PropertySamplingRate(IN      PPCPROPERTY_REQUEST PropertyRequest);
    static NTSTATUS PropertyPrivate(IN      PPCPROPERTY_REQUEST PropertyRequest);

    friend class CMiniportWaveInStream;
    friend class CMiniportWaveOutStream;
    friend class CMiniportWaveStream;

    kListener listener;
    dword kx3d_compat;
    int kx3d_sp8ps;

    spinlock_t listener_lock;
};

typedef struct
{
  ULONG          	 Flags;
  ULONG          	 Control;
  WAVEFORMATEXTENSIBLE   WaveFormatEx;
} myKSDSOUND_BUFFERDESC;

typedef struct
{
  KSDATAFORMAT            DataFormat;
  myKSDSOUND_BUFFERDESC   BufferDesc;
} myKSDATAFORMAT;

class CMiniportWaveStream
:   public IMiniportWaveCyclicStream,
    public IDrmAudioStream,
    public CUnknown
{
public:
    SAFE_DESTRUCTORS
    DECLARE_STD_UNKNOWN();
    DEFINE_STD_CONSTRUCTOR(CMiniportWaveStream);

    CMiniportWave	*Miniport;       // Miniport that created us.
    PSERVICEGROUP       ServiceGroup;       // For notification.
    myKSDATAFORMAT	DataFormat2;
    POOL_TYPE		PoolType;
    ULONG		Pin;

    int magic;
    kx_hw *hw;

    // DataFormat after parsing
    int	current_freq;		// w/o doppler applied
    int current_freq_multiplier; // =doppler_factor*1000
    #define DOPPLER_MULT	1000

    virtual void my_isr(void);

    // Current state/position
    int State;

    virtual NTSTATUS Init(
    		  IN CMiniportWave *Miniport,
		  IN POOL_TYPE PoolType,
    		  IN ULONG Pin_,
    		  IN PKSDATAFORMAT DataFormat_,
    		  OUT PDMACHANNEL *DmaChannel_,
    		  OUT PSERVICEGROUP *ServiceGroup_);

    IMP_IDrmAudioStream;
};

class CASIO
{
public:
    kx_hw *asio_hw;
    void *that_;
    dword asio_magic;

    // ASIO stuff
    asio_notification_t *asio_notification_user;
    PMDL asio_notification_mdl;

    PRKEVENT krnl_event;

    int is_asio;

    int asio_latency; // in samples
    int asio_bps;

    int asio_init(int asio_method);
    int asio_alloc_output(int chn,size_t size,void **addr,int rate,int bps);
    int asio_alloc_input(int chn,size_t size,void **addr,int rate,int bps);
    int asio_free_output(int chn);
    int asio_free_input(int chn);
    int asio_close();

    int asio_start();
    int asio_stop();
    int asio_get_position(int mode); // 0 - std 0/1; 1 - byte
    int asio_wait(int toggle,volatile bool *done);

    int asio_notify(int evnt,int nterface);

    // lla: "low-latency audio"
    // interface:
    #define LLA_NOTIFY_ASIO	0
    #define LLA_NOTIFY_GSIF	1
};

class CMiniportWaveOutStream : public CMiniportWaveStream, public CASIO
{
public:
    SAFE_DESTRUCTORS
    DECLARE_STD_UNKNOWN();
    // std constructor here
    CMiniportWaveOutStream(PUNKNOWN pUnknownOuter) : CMiniportWaveStream(pUnknownOuter)
     { 
      source_channel=0;
     };

    IMP_IMiniportWaveCyclicStream;
    ~CMiniportWaveOutStream();

    int n_channels; // physical channels
    int n_chn; // per channel (multiplier) - for source

    int ac3_pt_method; // same as hw->ac3_pt_state.method

    kx_timer ac3_timer;
    static void ac3_timer_func(void *data,int what);

    dword out_type;
    #define OUT_AC3		1
    #define OUT_BUFFERED	2

    int bps; // bits per sample

    virtual void my_isr(void);

    // multy wave-out
    #define REAL_WAVE_CHANNELS	8
    #define PHYS_WAVE_CHANNELS	(REAL_WAVE_CHANNELS*2)
    int channels[PHYS_WAVE_CHANNELS];
    PDMACHANNEL	dma_channels[PHYS_WAVE_CHANNELS];

    // ac-3
    ac3_state ac3_state;
    int cur_ac3_buffer;
    int ac3_pt_pos;

    // cached hw->cb.. values
    int cb_ac3_buffers,cb_pb_buffers;

    int source_buffer_size;
    PDMACHANNEL source_channel;

    // ac-3 stuff
    static void ac3_fill_buffer(byte **start,byte **end,struct ac3_state_s *state,int size);

    // dsp irq handler
    static void ac3_pt_handler(void *t);

    int vol1[MAX_TOPOLOGY_CHANNELS];
    int vol2[MAX_TOPOLOGY_CHANNELS];

    int Parse();
    NTSTATUS Open();

    virtual NTSTATUS Init(
    		  IN CMiniportWave *Miniport,
		  IN POOL_TYPE PoolType,
    		  IN ULONG Pin_,
    		  IN PKSDATAFORMAT DataFormat_,
    		  OUT PDMACHANNEL *DmaChannel_,
    		  OUT PSERVICEGROUP *ServiceGroup_);

    kSource source;
    int kx3d_attn;

    static NTSTATUS PropertyVolume(IN      PPCPROPERTY_REQUEST PropertyRequest);
    static NTSTATUS PropertySamplingRate(IN      PPCPROPERTY_REQUEST PropertyRequest);

    // 3-d
    static NTSTATUS Property3DL(IN      PPCPROPERTY_REQUEST PropertyRequest);
    static NTSTATUS Property3DB(IN      PPCPROPERTY_REQUEST PropertyRequest);

    // generic
    static NTSTATUS PropertyEAL(int id, IN      PPCPROPERTY_REQUEST PropertyRequest);
    static NTSTATUS PropertyEAB(int id, IN      PPCPROPERTY_REQUEST PropertyRequest);

    // api-specific
    static NTSTATUS PropertyEAX1L(IN      PPCPROPERTY_REQUEST PropertyRequest);
    static NTSTATUS PropertyEAX2L(IN      PPCPROPERTY_REQUEST PropertyRequest);
    static NTSTATUS PropertyEAX3L(IN      PPCPROPERTY_REQUEST PropertyRequest);
    static NTSTATUS PropertyI3DL2L(IN      PPCPROPERTY_REQUEST PropertyRequest);
    
    static NTSTATUS PropertyEAX1B(IN      PPCPROPERTY_REQUEST PropertyRequest);
    static NTSTATUS PropertyEAX2B(IN      PPCPROPERTY_REQUEST PropertyRequest);
    static NTSTATUS PropertyEAX3B(IN      PPCPROPERTY_REQUEST PropertyRequest);
    static NTSTATUS PropertyI3DL2B(IN      PPCPROPERTY_REQUEST PropertyRequest);
};

class CMiniportWaveInStream : public CMiniportWaveStream
{
public:
    SAFE_DESTRUCTORS
    DECLARE_STD_UNKNOWN();
    CMiniportWaveInStream(PUNKNOWN pUnknownOuter)
    : CMiniportWaveStream(pUnknownOuter)
    {
    };

    PDMACHANNEL DmaChannel;
    int buffer_size;

    ~CMiniportWaveInStream();
    IMP_IMiniportWaveCyclicStream;

    int Parse();
    NTSTATUS Open();

    int is_rec;
    int isStereo;

    int rec_subdevice;

    virtual void my_isr(void);

    virtual NTSTATUS Init(
    		  IN CMiniportWave *Miniport,
		  IN POOL_TYPE PoolType,
    		  IN ULONG Pin_,
    		  IN PKSDATAFORMAT DataFormat_,
    		  OUT PDMACHANNEL *DmaChannel_,
    		  OUT PSERVICEGROUP *ServiceGroup_);
};


#endif
