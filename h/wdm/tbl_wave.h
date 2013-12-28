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


static
KSDATARANGE PinDataRangesBridge[] =
{
   {
      sizeof(KSDATARANGE),
      0,
      0,
      0,
      STATICGUIDOF(KSDATAFORMAT_TYPE_AUDIO),
      STATICGUIDOF(KSDATAFORMAT_SUBTYPE_ANALOG),
      STATICGUIDOF(KSDATAFORMAT_SPECIFIER_NONE)
   }
};


static
KSDATARANGE_AUDIO PinDataRangesStreamRecording[16] =
{
    // -------- Recording settings
    {
        {
            sizeof(KSDATARANGE_AUDIO),
            0,
            0,
            0,
            STATICGUIDOF(KSDATAFORMAT_TYPE_AUDIO),
            STATICGUIDOF(KSDATAFORMAT_SUBTYPE_PCM),
            STATICGUIDOF(KSDATAFORMAT_SPECIFIER_WAVEFORMATEX)
        },
        2,      // Max number of channels.
        16,      // Minimum number of bits per sample.
        16,     // Maximum number of bits per channel.
        48000,   // Minimum rate.
        48000   // Maximum rate.
    },
    {
    	{
    	    sizeof(KSDATARANGE_AUDIO),
    	    0,
    	    0,
    	    0,
            STATICGUIDOF(KSDATAFORMAT_TYPE_AUDIO),
            STATICGUIDOF(KSDATAFORMAT_SUBTYPE_PCM),
            STATICGUIDOF(KSDATAFORMAT_SPECIFIER_DSOUND)
         },
         2,
         16,
         16,
         48000,
         48000
    },
    // this is necessary for direct spdif recording
    {
        {
            sizeof(KSDATARANGE_AUDIO),
            0,
            0,
            0,
            STATICGUIDOF(KSDATAFORMAT_TYPE_AUDIO),
            STATICGUIDOF(KSDATAFORMAT_SUBTYPE_PCM),
            STATICGUIDOF(KSDATAFORMAT_SPECIFIER_WAVEFORMATEX)
        },
        2,      // Max number of channels.
        16,      // Minimum number of bits per sample.
        16,     // Maximum number of bits per channel.
        96000,   // Minimum rate.
        96000   // Maximum rate.
    },
    {
    	{
    	    sizeof(KSDATARANGE_AUDIO),
    	    0,
    	    0,
    	    0,
            STATICGUIDOF(KSDATAFORMAT_TYPE_AUDIO),
            STATICGUIDOF(KSDATAFORMAT_SUBTYPE_PCM),
            STATICGUIDOF(KSDATAFORMAT_SPECIFIER_DSOUND)
         },
         2,
         16,
         16,
         96000,
         96000
    },
    {
        {
            sizeof(KSDATARANGE_AUDIO),
            0,
            0,
            0,
            STATICGUIDOF(KSDATAFORMAT_TYPE_AUDIO),
            STATICGUIDOF(KSDATAFORMAT_SUBTYPE_PCM),
            STATICGUIDOF(KSDATAFORMAT_SPECIFIER_WAVEFORMATEX)
        },
        2,      // Max number of channels.
        16,      // Minimum number of bits per sample.
        16,     // Maximum number of bits per channel.
        44100,   // Minimum rate.
        44100   // Maximum rate.
    },
    {
    	{
    	    sizeof(KSDATARANGE_AUDIO),
    	    0,
    	    0,
    	    0,
            STATICGUIDOF(KSDATAFORMAT_TYPE_AUDIO),
            STATICGUIDOF(KSDATAFORMAT_SUBTYPE_PCM),
            STATICGUIDOF(KSDATAFORMAT_SPECIFIER_DSOUND)
         },
         2,
         16,
         16,
         44100,
         44100
    },

    // Note: the rest is removed by default in VISTA
    #define REC_FORMATS_VISTA_LIMIT 6

    {
        {
            sizeof(KSDATARANGE_AUDIO),
            0,
            0,
            0,
            STATICGUIDOF(KSDATAFORMAT_TYPE_AUDIO),
            STATICGUIDOF(KSDATAFORMAT_SUBTYPE_PCM),
            STATICGUIDOF(KSDATAFORMAT_SPECIFIER_WAVEFORMATEX)
        },
        2,      // Max number of channels.
        16,      // Minimum number of bits per sample.
        16,     // Maximum number of bits per channel.
        32000,   // Minimum rate.
        32000   // Maximum rate.
    },
    {
    	{
    	    sizeof(KSDATARANGE_AUDIO),
    	    0,
    	    0,
    	    0,
            STATICGUIDOF(KSDATAFORMAT_TYPE_AUDIO),
            STATICGUIDOF(KSDATAFORMAT_SUBTYPE_PCM),
            STATICGUIDOF(KSDATAFORMAT_SPECIFIER_DSOUND)
         },
         2,
         16,
         16,
         32000,
         32000
    },
    {
        {
            sizeof(KSDATARANGE_AUDIO),
            0,
            0,
            0,
            STATICGUIDOF(KSDATAFORMAT_TYPE_AUDIO),
            STATICGUIDOF(KSDATAFORMAT_SUBTYPE_PCM),
            STATICGUIDOF(KSDATAFORMAT_SPECIFIER_WAVEFORMATEX)
        },
        2,      // Max number of channels.
        16,      // Minimum number of bits per sample.
        16,     // Maximum number of bits per channel.
        22050,   // Minimum rate.
        22050   // Maximum rate.
    },
    {
    	{
    	    sizeof(KSDATARANGE_AUDIO),
    	    0,
    	    0,
    	    0,
            STATICGUIDOF(KSDATAFORMAT_TYPE_AUDIO),
            STATICGUIDOF(KSDATAFORMAT_SUBTYPE_PCM),
            STATICGUIDOF(KSDATAFORMAT_SPECIFIER_DSOUND)
         },
         2,
         16,
         16,
         22050,
         22050
    },
    {
        {
            sizeof(KSDATARANGE_AUDIO),
            0,
            0,
            0,
            STATICGUIDOF(KSDATAFORMAT_TYPE_AUDIO),
            STATICGUIDOF(KSDATAFORMAT_SUBTYPE_PCM),
            STATICGUIDOF(KSDATAFORMAT_SPECIFIER_WAVEFORMATEX)
        },
        2,      // Max number of channels.
        16,      // Minimum number of bits per sample.
        16,     // Maximum number of bits per channel.
        16000,   // Minimum rate.
        16000   // Maximum rate.
    },
    {
    	{
    	    sizeof(KSDATARANGE_AUDIO),
    	    0,
    	    0,
    	    0,
            STATICGUIDOF(KSDATAFORMAT_TYPE_AUDIO),
            STATICGUIDOF(KSDATAFORMAT_SUBTYPE_PCM),
            STATICGUIDOF(KSDATAFORMAT_SPECIFIER_DSOUND)
         },
         2,
         16,
         16,
         16000,
         16000
    },
    {
        {
            sizeof(KSDATARANGE_AUDIO),
            0,
            0,
            0,
            STATICGUIDOF(KSDATAFORMAT_TYPE_AUDIO),
            STATICGUIDOF(KSDATAFORMAT_SUBTYPE_PCM),
            STATICGUIDOF(KSDATAFORMAT_SPECIFIER_WAVEFORMATEX)
        },
        2,      // Max number of channels.
        16,      // Minimum number of bits per sample.
        16,     // Maximum number of bits per channel.
        11025,   // Minimum rate.
        11025   // Maximum rate.
    },
    {
    	{
    	    sizeof(KSDATARANGE_AUDIO),
    	    0,
    	    0,
    	    0,
            STATICGUIDOF(KSDATAFORMAT_TYPE_AUDIO),
            STATICGUIDOF(KSDATAFORMAT_SUBTYPE_PCM),
            STATICGUIDOF(KSDATAFORMAT_SPECIFIER_DSOUND)
         },
         2,
         16,
         16,
         11025,
         11025
    },
    {
        {
            sizeof(KSDATARANGE_AUDIO),
            0,
            0,
            0,
            STATICGUIDOF(KSDATAFORMAT_TYPE_AUDIO),
            STATICGUIDOF(KSDATAFORMAT_SUBTYPE_PCM),
            STATICGUIDOF(KSDATAFORMAT_SPECIFIER_WAVEFORMATEX)
        },
        2,      // Max number of channels.
        16,      // Minimum number of bits per sample.
        16,     // Maximum number of bits per channel.
        8000,   // Minimum rate.
        8000   // Maximum rate.
    },
    {
    	{
    	    sizeof(KSDATARANGE_AUDIO),
    	    0,
    	    0,
    	    0,
            STATICGUIDOF(KSDATAFORMAT_TYPE_AUDIO),
            STATICGUIDOF(KSDATAFORMAT_SUBTYPE_PCM),
            STATICGUIDOF(KSDATAFORMAT_SPECIFIER_DSOUND)
         },
         2,
         16,
         16,
         8000,
         8000
    }
};

static
KSDATARANGE_AUDIO PinDataRangesStreamPlaybackGen[2] =
{
    {
        {
            sizeof(KSDATARANGE_AUDIO),
            0,
            0,
            0,
            STATICGUIDOF(KSDATAFORMAT_TYPE_AUDIO),
            STATICGUIDOF(KSDATAFORMAT_SUBTYPE_PCM),
            STATICGUIDOF(KSDATAFORMAT_SPECIFIER_WAVEFORMATEX)
        },
        2,      // Max number of channels. fixed
        16,      // Minimum number of bits per sample.
        16,     // Maximum number of bits per channel.
        KX_MIN_RATE,   // Minimum rate.
        KX_MAX_RATE   // Maximum rate.
    },
    {
    	{
    	    sizeof(KSDATARANGE_AUDIO),
    	    0,
    	    0,
    	    0,
            STATICGUIDOF(KSDATAFORMAT_TYPE_AUDIO),
            STATICGUIDOF(KSDATAFORMAT_SUBTYPE_PCM),
            STATICGUIDOF(KSDATAFORMAT_SPECIFIER_DSOUND)
         },
         2,
         16, 
         16,
         KX_MIN_RATE,
         KX_MAX_RATE
    }
};

static
KSDATARANGE_AUDIO PinDataRangesStreamPlaybackMin[2] =
{
    {
        {
            sizeof(KSDATARANGE_AUDIO),
            0,
            0,
            0,
            STATICGUIDOF(KSDATAFORMAT_TYPE_AUDIO),
            STATICGUIDOF(KSDATAFORMAT_SUBTYPE_PCM),
            STATICGUIDOF(KSDATAFORMAT_SPECIFIER_WAVEFORMATEX)
        },
        2,      // Max number of channels. fixed
        16,      // Minimum number of bits per sample.
        16,     // Maximum number of bits per channel.
        48000,   // Minimum rate.
        48000 // Maximum rate.
    },
    {
    	{
    	    sizeof(KSDATARANGE_AUDIO),
    	    0,
    	    0,
    	    0,
            STATICGUIDOF(KSDATAFORMAT_TYPE_AUDIO),
            STATICGUIDOF(KSDATAFORMAT_SUBTYPE_PCM),
            STATICGUIDOF(KSDATAFORMAT_SPECIFIER_DSOUND)
         },
         2,
         16, 
         16,
	     48000,
         48000
    }
};

static
KSDATARANGE_AUDIO PinDataRangesStreamPlaybackMultichannel[2] =
{
    {
        {
            sizeof(KSDATARANGE_AUDIO),
            0,
            0,
            0,
            STATICGUIDOF(KSDATAFORMAT_TYPE_AUDIO),
            STATICGUIDOF(KSDATAFORMAT_SUBTYPE_PCM),
            STATICGUIDOF(KSDATAFORMAT_SPECIFIER_WAVEFORMATEX)
        },
        REAL_WAVE_CHANNELS,      // Max number of channels. fixed
        16,      // Minimum number of bits per sample.
        32,     // Maximum number of bits per channel.
        48000,   // Minimum rate.
        48000 // Maximum rate.
    },
    {
    	{
    	    sizeof(KSDATARANGE_AUDIO),
    	    0,
    	    0,
    	    0,
            STATICGUIDOF(KSDATAFORMAT_TYPE_AUDIO),
            STATICGUIDOF(KSDATAFORMAT_SUBTYPE_PCM),
            STATICGUIDOF(KSDATAFORMAT_SPECIFIER_DSOUND)
         },
         REAL_WAVE_CHANNELS,
         16, 
         32,
	     48000,
         48000
    }
};

static
KSDATARANGE_AUDIO PinDataRangesStreamPlaybackHiFi[2] =
{
    {
        {
            sizeof(KSDATARANGE_AUDIO),
            0,
            0,
            0,
            STATICGUIDOF(KSDATAFORMAT_TYPE_AUDIO),
            STATICGUIDOF(KSDATAFORMAT_SUBTYPE_PCM),
            STATICGUIDOF(KSDATAFORMAT_SPECIFIER_WAVEFORMATEX)
        },
        REAL_WAVE_CHANNELS,      // Max number of channels [8]
        16,      // Minimum number of bits per sample.
        32,     // Maximum number of bits per channel.
        KX_MIN_RATE,   // Minimum rate.
        KX_MAX_RATE   // Maximum rate.
    },
    {
    	{
    	    sizeof(KSDATARANGE_AUDIO),
    	    0,
    	    0,
    	    0,
            STATICGUIDOF(KSDATAFORMAT_TYPE_AUDIO),
            STATICGUIDOF(KSDATAFORMAT_SUBTYPE_PCM),
            STATICGUIDOF(KSDATAFORMAT_SPECIFIER_DSOUND)
         },
         REAL_WAVE_CHANNELS,	// max channels
         16,	// min bps
         32,	// max bps
         KX_MIN_RATE,
         KX_MAX_RATE
    }
};

static
KSDATARANGE_AUDIO PinDataRangesStreamPlaybackAC3[4] =
{
  {
    {
      sizeof(KSDATARANGE_AUDIO),
      0,
      0,
      0,
      STATICGUIDOF(KSDATAFORMAT_TYPE_AUDIO),
      STATICGUIDOF(KSDATAFORMAT_SUBTYPE_DOLBY_AC3_SPDIF),
      STATICGUIDOF(KSDATAFORMAT_SPECIFIER_WAVEFORMATEX)
    },
    2,      // Max number of channels.
    16,     // Minimum number of bits per sample.
    16,     // Maximum number of bits per channel.
    48000,  // Minimum rate.
    48000   // Maximum rate.
  },
  {
    {
      sizeof(KSDATARANGE_AUDIO),
      0,
      0,
      0,
      STATICGUIDOF(KSDATAFORMAT_TYPE_AUDIO),
      STATICGUIDOF(KSDATAFORMAT_SUBTYPE_DOLBY_AC3_SPDIF),
      STATICGUIDOF(KSDATAFORMAT_SPECIFIER_DSOUND)
    },
    2,      // Max number of channels.
    16,     // Minimum number of bits per sample.
    16,     // Maximum number of bits per channel.
    48000,  // Minimum rate.
    48000   // Maximum rate.
  },
    {
        {
            sizeof(KSDATARANGE_AUDIO),
            0,
            0,
            0,
            STATICGUIDOF(KSDATAFORMAT_TYPE_AUDIO),
            STATICGUIDOF(KSDATAFORMAT_SUBTYPE_PCM),
            STATICGUIDOF(KSDATAFORMAT_SPECIFIER_WAVEFORMATEX)
        },
        2,      // Max number of channels. fixed
        16,      // Minimum number of bits per sample.
        16,     // Maximum number of bits per channel.
        48000,   // Minimum rate.
        48000 // Maximum rate.
    },
    {
    	{
    	    sizeof(KSDATARANGE_AUDIO),
    	    0,
    	    0,
    	    0,
            STATICGUIDOF(KSDATAFORMAT_TYPE_AUDIO),
            STATICGUIDOF(KSDATAFORMAT_SUBTYPE_PCM),
            STATICGUIDOF(KSDATAFORMAT_SPECIFIER_DSOUND)
         },
         2,
         16, 
         16,
	     48000,
         48000
    }
};

static PKSDATARANGE PinDataRangePointersPlaybackGen[4] =
{
    PKSDATARANGE(&PinDataRangesStreamPlaybackMin[0]),
    PKSDATARANGE(&PinDataRangesStreamPlaybackMin[1]),

    PKSDATARANGE(&PinDataRangesStreamPlaybackGen[0]),
    PKSDATARANGE(&PinDataRangesStreamPlaybackGen[1])
};

static PKSDATARANGE PinDataRangePointersPlaybackHiFi[4] =
{
    PKSDATARANGE(&PinDataRangesStreamPlaybackMin[0]),
    PKSDATARANGE(&PinDataRangesStreamPlaybackMin[1]),
    PKSDATARANGE(&PinDataRangesStreamPlaybackHiFi[0]),
    PKSDATARANGE(&PinDataRangesStreamPlaybackHiFi[1]),
};

static PKSDATARANGE PinDataRangePointersPlaybackAC3[4] =
{
    PKSDATARANGE(&PinDataRangesStreamPlaybackAC3[0]),
    PKSDATARANGE(&PinDataRangesStreamPlaybackAC3[1]),

    PKSDATARANGE(&PinDataRangesStreamPlaybackAC3[2]),
    PKSDATARANGE(&PinDataRangesStreamPlaybackAC3[3])  
};

// Vista formats:

static PKSDATARANGE PinDataRangePointersPlaybackMultichannelOnly[2] =
{
    PKSDATARANGE(&PinDataRangesStreamPlaybackMultichannel[0]),
    PKSDATARANGE(&PinDataRangesStreamPlaybackMultichannel[1]),
};

static PKSDATARANGE PinDataRangePointersPlaybackMinimumOnly[2] =
{
    PKSDATARANGE(&PinDataRangesStreamPlaybackMin[0]),
    PKSDATARANGE(&PinDataRangesStreamPlaybackMin[1]),
};

static PKSDATARANGE PinDataRangePointersRecording[16] =
{
    PKSDATARANGE(&PinDataRangesStreamRecording[0]),
    PKSDATARANGE(&PinDataRangesStreamRecording[1]),
    PKSDATARANGE(&PinDataRangesStreamRecording[2]),
    PKSDATARANGE(&PinDataRangesStreamRecording[3]),
    PKSDATARANGE(&PinDataRangesStreamRecording[4]),
    PKSDATARANGE(&PinDataRangesStreamRecording[5]),

    // REC_FORMATS_VISTA_LIMIT
    PKSDATARANGE(&PinDataRangesStreamRecording[6]),
    PKSDATARANGE(&PinDataRangesStreamRecording[7]),
    PKSDATARANGE(&PinDataRangesStreamRecording[8]),
    PKSDATARANGE(&PinDataRangesStreamRecording[9]),
    PKSDATARANGE(&PinDataRangesStreamRecording[10]),
    PKSDATARANGE(&PinDataRangesStreamRecording[11]),
    PKSDATARANGE(&PinDataRangesStreamRecording[12]),
    PKSDATARANGE(&PinDataRangesStreamRecording[13]),
    PKSDATARANGE(&PinDataRangesStreamRecording[14]),
    PKSDATARANGE(&PinDataRangesStreamRecording[15])
};

static
PKSDATARANGE PinDataRangePointersBridge[] =
{
    &PinDataRangesBridge[0]
};


/*****************************************************************************
 * MiniportPins
 *****************************************************************************
 * List of pins.
 */

static
PCPIN_DESCRIPTOR 
MiniportPins23[] =
{
    // Wave Out Streaming Pin (Renderer)
    {
        KX_NUMBER_OF_VOICES,KX_NUMBER_OF_VOICES,0,
        NULL,
        {
            0,
            NULL,
            0,
            NULL,
            SIZEOF_ARRAY(PinDataRangePointersPlaybackGen),
            PinDataRangePointersPlaybackGen,
            KSPIN_DATAFLOW_IN,
            KSPIN_COMMUNICATION_SINK,
            &KSCATEGORY_AUDIO,
            &TOPO_WAVEOUT23_NAME,
            0
        }
    },
    
    // Wave Out Bridge Pin (Renderer)
    {
        0,0,0,
        NULL,
        {
            0,
            NULL,
            0,
            NULL,
            SIZEOF_ARRAY(PinDataRangePointersBridge),
            PinDataRangePointersBridge,
            KSPIN_DATAFLOW_OUT,
            KSPIN_COMMUNICATION_BRIDGE,
            &KSNODETYPE_LINE_CONNECTOR,
            &TOPO_WAVEOUT23_NAME,
            0
        }
    }
};
static
PCPIN_DESCRIPTOR 
MiniportPins45[] =
{
    // Wave Out Streaming Pin (Renderer)
    {
        KX_NUMBER_OF_VOICES,KX_NUMBER_OF_VOICES,0,
        NULL,
        {
            0,
            NULL,
            0,
            NULL,
            SIZEOF_ARRAY(PinDataRangePointersPlaybackGen),
            PinDataRangePointersPlaybackGen,
            KSPIN_DATAFLOW_IN,
            KSPIN_COMMUNICATION_SINK,
            &KSCATEGORY_AUDIO,
            &TOPO_WAVEOUT45_NAME,
            0
        }
    },
    
    // Wave Out Bridge Pin (Renderer)
    {
        0,0,0,
        NULL,
        {
            0,
            NULL,
            0,
            NULL,
            SIZEOF_ARRAY(PinDataRangePointersBridge),
            PinDataRangePointersBridge,
            KSPIN_DATAFLOW_OUT,
            KSPIN_COMMUNICATION_BRIDGE,
            &KSNODETYPE_LINE_CONNECTOR,
            &TOPO_WAVEOUT45_NAME,
            0
        }
    }
};
static
PCPIN_DESCRIPTOR 
MiniportPins67[] =
{
    // Wave Out Streaming Pin (Renderer)
    {
        KX_NUMBER_OF_VOICES,KX_NUMBER_OF_VOICES,0,
        NULL,
        {
            0,
            NULL,
            0,
            NULL,
            SIZEOF_ARRAY(PinDataRangePointersPlaybackGen),
            PinDataRangePointersPlaybackGen,
            KSPIN_DATAFLOW_IN,
            KSPIN_COMMUNICATION_SINK,
            &KSCATEGORY_AUDIO,
            &TOPO_WAVEOUT67_NAME,
            0
        }
    },
    
    // Wave Out Bridge Pin (Renderer)
    {
        0,0,0,
        NULL,
        {
            0,
            NULL,
            0,
            NULL,
            SIZEOF_ARRAY(PinDataRangePointersBridge),
            PinDataRangePointersBridge,
            KSPIN_DATAFLOW_OUT,
            KSPIN_COMMUNICATION_BRIDGE,
            &KSNODETYPE_LINE_CONNECTOR,
            &TOPO_WAVEOUT67_NAME,
            0
        }
    }
};

static
PCPIN_DESCRIPTOR 
MiniportPinsHiFi[] =
{
    // Wave Out Streaming Pin (Renderer) - WAVE_NODE_WAVE_OUT
    {
        KX_NUMBER_OF_VOICES,KX_NUMBER_OF_VOICES,0,
        NULL,
        {
            0,
            NULL,
            0,
            NULL,
            SIZEOF_ARRAY(PinDataRangePointersPlaybackHiFi),
            PinDataRangePointersPlaybackHiFi,
            KSPIN_DATAFLOW_IN,
            KSPIN_COMMUNICATION_SINK,
            &KSCATEGORY_AUDIO,
            &TOPO_WAVEOUT01_NAME,
            0
        }
    },
    
    // Wave Out Bridge Pin (Renderer) - WAVE_NODE_OUTPUT
    {
        0,0,0,
        NULL,
        {
            0,
            NULL,
            0,
            NULL,
            SIZEOF_ARRAY(PinDataRangePointersBridge),
            PinDataRangePointersBridge,
            KSPIN_DATAFLOW_OUT,
            KSPIN_COMMUNICATION_NONE,
            &KSCATEGORY_AUDIO,
            &TOPO_WAVEOUT01_NAME,
            0
        }
    },
    
    // Wave In Streaming Pin (Capture) - WAVE_NODE_WAVE_IN
    {
        1,1,0,
        NULL,
        {
            0,
            NULL,
            0,
            NULL,
            SIZEOF_ARRAY(PinDataRangePointersRecording),
            PinDataRangePointersRecording,
            KSPIN_DATAFLOW_OUT,
            KSPIN_COMMUNICATION_SINK,
            &PINNAME_CAPTURE,
            // this name shows up as the recording panel name in SoundVol.
            &MY_KX_RECMIXER_NAME, // &TOPO_WAVEIN_NAME, // &TOPO_WAVEIN_NAME or &MY_KX_RECMIXER_NAME
            0
        }
    },

    // Wave In Bridge Pin (Capture - To Topology) - WAVE_NODE_INPUT
    {
        0,0,0,
        NULL,
        {
            0,
            NULL,
            0,
            NULL,
            SIZEOF_ARRAY(PinDataRangePointersBridge),
            PinDataRangePointersBridge,
            KSPIN_DATAFLOW_IN,
            KSPIN_COMMUNICATION_BRIDGE,
            &KSCATEGORY_AUDIO,
            &TOPO_WAVEIN_NAME, // &MY_KX_RECMIXER_NAME or &TOPO_WAVEIN_NAME,
            0
        }
    },

    // AC3 SPDIF Digital SINK - WAVE_NODE_SPDIF
    {
    	1,1,0,
    	NULL,
    	{
    		0,
    		NULL,
    		0,
    		NULL,
    		SIZEOF_ARRAY(PinDataRangePointersPlaybackAC3),
            PinDataRangePointersPlaybackAC3,
	        KSPIN_DATAFLOW_IN,
	        KSPIN_COMMUNICATION_SINK,
	        &KSCATEGORY_AUDIO,
	        &WAVE_AC3_NODE_NAME,
	        0
        }
    }

#define USE_WAVE_NODE_SPDIFOUTPUT
#ifdef USE_WAVE_NODE_SPDIFOUTPUT
    // AC3 SPDIF Digital Output - WAVE_NODE_SPDIFOUTPUT
    ,{
        0,0,0,
    	NULL,
    	{
    		0,
    		NULL,
    		0,
    		NULL,
    		SIZEOF_ARRAY(PinDataRangePointersBridge),
            PinDataRangePointersBridge,
	        KSPIN_DATAFLOW_OUT,
	        KSPIN_COMMUNICATION_BRIDGE,
	        &KSNODETYPE_SPDIF_INTERFACE,
	        &WAVE_AC3_NODE_NAME,
	        0
        }
    }
#endif
};

static PCPROPERTY_ITEM Properties[] =
{
    { // pin
        &KSPROPSETID_Audio,
        KSPROPERTY_AUDIO_SAMPLING_RATE,
        KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWave::PropertySamplingRate
    },
    { // any
        &KSPROPSETID_Audio,
        KSPROPERTY_AUDIO_CPU_RESOURCES,
	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_BASICSUPPORT,
        CMiniportWave::PropertyCpuResources
    },
   // DirectSound 3-D Properties
   // Buffer
   // ---------------------
   { // pin
        &KSPROPSETID_DirectSound3DBuffer,
        KSPROPERTY_DIRECTSOUND3DBUFFER_ALL,
        KSPROPERTY_TYPE_SET|KSPROPERTY_TYPE_GET,
        CMiniportWaveOutStream::Property3DB
   },
   { // pin
        &KSPROPSETID_DirectSound3DBuffer,
        KSPROPERTY_DIRECTSOUND3DBUFFER_CONEANGLES,
        KSPROPERTY_TYPE_SET|KSPROPERTY_TYPE_GET,
        CMiniportWaveOutStream::Property3DB
   },
   { // pin
        &KSPROPSETID_DirectSound3DBuffer,
        KSPROPERTY_DIRECTSOUND3DBUFFER_CONEORIENTATION,
        KSPROPERTY_TYPE_SET|KSPROPERTY_TYPE_GET,
        CMiniportWaveOutStream::Property3DB
   },
   { // pin
        &KSPROPSETID_DirectSound3DBuffer,
        KSPROPERTY_DIRECTSOUND3DBUFFER_CONEOUTSIDEVOLUME,
        KSPROPERTY_TYPE_SET|KSPROPERTY_TYPE_GET,
        CMiniportWaveOutStream::Property3DB
   },
   { // pin
        &KSPROPSETID_DirectSound3DBuffer,
        KSPROPERTY_DIRECTSOUND3DBUFFER_MAXDISTANCE,
        KSPROPERTY_TYPE_SET|KSPROPERTY_TYPE_GET,
        CMiniportWaveOutStream::Property3DB
   },
   { // pin
        &KSPROPSETID_DirectSound3DBuffer,
        KSPROPERTY_DIRECTSOUND3DBUFFER_MINDISTANCE,
        KSPROPERTY_TYPE_SET|KSPROPERTY_TYPE_GET,
        CMiniportWaveOutStream::Property3DB
   },
   { // pin
        &KSPROPSETID_DirectSound3DBuffer,
        KSPROPERTY_DIRECTSOUND3DBUFFER_MODE,
        KSPROPERTY_TYPE_SET|KSPROPERTY_TYPE_GET,
        CMiniportWaveOutStream::Property3DB
   },
   { // pin
        &KSPROPSETID_DirectSound3DBuffer,
        KSPROPERTY_DIRECTSOUND3DBUFFER_POSITION,
        KSPROPERTY_TYPE_SET|KSPROPERTY_TYPE_GET,
        CMiniportWaveOutStream::Property3DB
   },
   { // pin
        &KSPROPSETID_DirectSound3DBuffer,
        KSPROPERTY_DIRECTSOUND3DBUFFER_VELOCITY,
        KSPROPERTY_TYPE_SET|KSPROPERTY_TYPE_GET,
        CMiniportWaveOutStream::Property3DB
   },
   // DirectSound 3-D Properties
   // Listener
   // ---------------------
   { // pin
        &KSPROPSETID_DirectSound3DListener,
	KSPROPERTY_DIRECTSOUND3DLISTENER_ALL,
        KSPROPERTY_TYPE_SET|KSPROPERTY_TYPE_GET,
        CMiniportWaveOutStream::Property3DL
   },
   { // pin
        &KSPROPSETID_DirectSound3DListener,
	KSPROPERTY_DIRECTSOUND3DLISTENER_ALLOCATION,
        KSPROPERTY_TYPE_SET|KSPROPERTY_TYPE_GET,
        CMiniportWaveOutStream::Property3DL
   },
   { // pin
        &KSPROPSETID_DirectSound3DListener,
	KSPROPERTY_DIRECTSOUND3DLISTENER_BATCH,
        KSPROPERTY_TYPE_SET|KSPROPERTY_TYPE_GET,
        CMiniportWaveOutStream::Property3DL
   },
   { // pin
        &KSPROPSETID_DirectSound3DListener,
	KSPROPERTY_DIRECTSOUND3DLISTENER_DISTANCEFACTOR,
        KSPROPERTY_TYPE_SET|KSPROPERTY_TYPE_GET,
        CMiniportWaveOutStream::Property3DL
   },
   { // pin
        &KSPROPSETID_DirectSound3DListener,
	KSPROPERTY_DIRECTSOUND3DLISTENER_DOPPLERFACTOR,
        KSPROPERTY_TYPE_SET|KSPROPERTY_TYPE_GET,
        CMiniportWaveOutStream::Property3DL
   },
   { // pin
        &KSPROPSETID_DirectSound3DListener,
	KSPROPERTY_DIRECTSOUND3DLISTENER_ORIENTATION,
        KSPROPERTY_TYPE_SET|KSPROPERTY_TYPE_GET,
        CMiniportWaveOutStream::Property3DL
   },
   { // pin
        &KSPROPSETID_DirectSound3DListener,
	KSPROPERTY_DIRECTSOUND3DLISTENER_POSITION,
        KSPROPERTY_TYPE_SET|KSPROPERTY_TYPE_GET,
        CMiniportWaveOutStream::Property3DL
   },
   { // pin
        &KSPROPSETID_DirectSound3DListener,
	KSPROPERTY_DIRECTSOUND3DLISTENER_ROLLOFFFACTOR,
        KSPROPERTY_TYPE_SET|KSPROPERTY_TYPE_GET,
        CMiniportWaveOutStream::Property3DL
   },
   { // pin
        &KSPROPSETID_DirectSound3DListener,
	KSPROPERTY_DIRECTSOUND3DLISTENER_VELOCITY,
        KSPROPERTY_TYPE_SET|KSPROPERTY_TYPE_GET,
        CMiniportWaveOutStream::Property3DL
   },
    // ------- EAX 1.0 properties
    // eax 1.0 'listner'
    {
    	&DSPROPSETID_EAX10_ReverbProperties,
        DSPROPERTY_EAX10_ALL,
        KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX1L
    },
    {
    	&DSPROPSETID_EAX10_ReverbProperties,
        DSPROPERTY_EAX10_ENVIRONMENT,
        KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX1L
    },
    {
    	&DSPROPSETID_EAX10_ReverbProperties,
        DSPROPERTY_EAX10_VOLUME,
        KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX1L
    },
    {
    	&DSPROPSETID_EAX10_ReverbProperties,
        DSPROPERTY_EAX10_DECAYTIME,
        KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX1L
    },
    {
    	&DSPROPSETID_EAX10_ReverbProperties,
        DSPROPERTY_EAX10_DAMPING,
        KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX1L
    },
    // eax 1.0 'buffer'
    {
    	&DSPROPSETID_EAX10BUFFER_ReverbProperties,
    	DSPROPERTY_EAX10BUFFER_ALL,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX1B
    },
    {
    	&DSPROPSETID_EAX10BUFFER_ReverbProperties,
    	DSPROPERTY_EAX10BUFFER_REVERBMIX,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX1B
    },
    // ------- EAX 2.0 properties
    // eax 2.0 'listner'
    {
    	&DSPROPSETID_EAX20_ListenerProperties,
    	DSPROPERTY_EAX20LISTENER_NONE,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX2L
    },
    {
    	&DSPROPSETID_EAX20_ListenerProperties,
    	DSPROPERTY_EAX20LISTENER_ALLPARAMETERS,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX2L
    },
    {
    	&DSPROPSETID_EAX20_ListenerProperties,
    	DSPROPERTY_EAX20LISTENER_ROOM,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX2L
    },
    {
    	&DSPROPSETID_EAX20_ListenerProperties,
    	DSPROPERTY_EAX20LISTENER_ROOMHF,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX2L
    },
    {
    	&DSPROPSETID_EAX20_ListenerProperties,
    	DSPROPERTY_EAX20LISTENER_ROOMROLLOFFFACTOR,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX2L
    },
    {
    	&DSPROPSETID_EAX20_ListenerProperties,
    	DSPROPERTY_EAX20LISTENER_DECAYTIME,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX2L
    },
    {
    	&DSPROPSETID_EAX20_ListenerProperties,
    	DSPROPERTY_EAX20LISTENER_DECAYHFRATIO,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX2L
    },
    {
    	&DSPROPSETID_EAX20_ListenerProperties,
    	DSPROPERTY_EAX20LISTENER_REFLECTIONS,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX2L
    },
    {
    	&DSPROPSETID_EAX20_ListenerProperties,
    	DSPROPERTY_EAX20LISTENER_REFLECTIONSDELAY,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX2L
    },
    {
    	&DSPROPSETID_EAX20_ListenerProperties,
    	DSPROPERTY_EAX20LISTENER_REVERB,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX2L
    },
    {
    	&DSPROPSETID_EAX20_ListenerProperties,
    	DSPROPERTY_EAX20LISTENER_REVERBDELAY,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX2L
    },
    {
    	&DSPROPSETID_EAX20_ListenerProperties,
    	DSPROPERTY_EAX20LISTENER_ENVIRONMENT,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX2L
    },
    {
    	&DSPROPSETID_EAX20_ListenerProperties,
    	DSPROPERTY_EAX20LISTENER_ENVIRONMENTSIZE,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX2L
    },
    {
    	&DSPROPSETID_EAX20_ListenerProperties,
    	DSPROPERTY_EAX20LISTENER_ENVIRONMENTDIFFUSION,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX2L
    },
    {
    	&DSPROPSETID_EAX20_ListenerProperties,
    	DSPROPERTY_EAX20LISTENER_AIRABSORPTIONHF,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX2L
    },
    {
    	&DSPROPSETID_EAX20_ListenerProperties,
    	DSPROPERTY_EAX20LISTENER_FLAGS,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX2L
    },
    // eax 2.0 'buffer'
    {
    	&DSPROPSETID_EAX20_BufferProperties,
    	DSPROPERTY_EAX20BUFFER_NONE,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX2B
    },
    {
    	&DSPROPSETID_EAX20_BufferProperties,
    	DSPROPERTY_EAX20BUFFER_ALLPARAMETERS,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX2B
    },
    {
    	&DSPROPSETID_EAX20_BufferProperties,
    	DSPROPERTY_EAX20BUFFER_DIRECT,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX2B
    },
    {
    	&DSPROPSETID_EAX20_BufferProperties,
    	DSPROPERTY_EAX20BUFFER_DIRECTHF,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX2B
    },
    {
    	&DSPROPSETID_EAX20_BufferProperties,
    	DSPROPERTY_EAX20BUFFER_ROOM,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX2B
    },
    {
    	&DSPROPSETID_EAX20_BufferProperties,
    	DSPROPERTY_EAX20BUFFER_ROOMHF,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX2B
    },
    {
    	&DSPROPSETID_EAX20_BufferProperties,
    	DSPROPERTY_EAX20BUFFER_ROOMROLLOFFFACTOR,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX2B
    },
    {
    	&DSPROPSETID_EAX20_BufferProperties,
    	DSPROPERTY_EAX20BUFFER_OBSTRUCTION,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX2B
    },
    {
    	&DSPROPSETID_EAX20_BufferProperties,
    	DSPROPERTY_EAX20BUFFER_OBSTRUCTIONLFRATIO,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX2B
    },
    {
    	&DSPROPSETID_EAX20_BufferProperties,
    	DSPROPERTY_EAX20BUFFER_OCCLUSION,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX2B
    },
    {
    	&DSPROPSETID_EAX20_BufferProperties,
    	DSPROPERTY_EAX20BUFFER_OCCLUSIONLFRATIO,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX2B
    },
    {
    	&DSPROPSETID_EAX20_BufferProperties,
    	DSPROPERTY_EAX20BUFFER_OCCLUSIONROOMRATIO,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX2B
    },
    {
    	&DSPROPSETID_EAX20_BufferProperties,
    	DSPROPERTY_EAX20BUFFER_OUTSIDEVOLUMEHF,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX2B
    },
    {
    	&DSPROPSETID_EAX20_BufferProperties,
    	DSPROPERTY_EAX20BUFFER_AIRABSORPTIONFACTOR,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX2B
    },
    {
    	&DSPROPSETID_EAX20_BufferProperties,
    	DSPROPERTY_EAX20BUFFER_FLAGS,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX2B
    },
    // ------- EAX 3.0 properties
    // eax 3.0 'listner'
    {
    	&DSPROPSETID_EAX30_ListenerProperties,
    	DSPROPERTY_EAX30LISTENER_NONE,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3L
    },
    {
    	&DSPROPSETID_EAX30_ListenerProperties,
    	DSPROPERTY_EAX30LISTENER_ALLPARAMETERS,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3L
    },
    {
    	&DSPROPSETID_EAX30_ListenerProperties,
    	DSPROPERTY_EAX30LISTENER_ROOM,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3L
    },
    {
    	&DSPROPSETID_EAX30_ListenerProperties,
    	DSPROPERTY_EAX30LISTENER_ROOMHF,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3L
    },
    {
    	&DSPROPSETID_EAX30_ListenerProperties,
    	DSPROPERTY_EAX30LISTENER_ROOMLF,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3L
    },
    {
    	&DSPROPSETID_EAX30_ListenerProperties,
    	DSPROPERTY_EAX30LISTENER_ROOMROLLOFFFACTOR,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3L
    },
    {
    	&DSPROPSETID_EAX30_ListenerProperties,
    	DSPROPERTY_EAX30LISTENER_DECAYTIME,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3L
    },
    {
    	&DSPROPSETID_EAX30_ListenerProperties,
    	DSPROPERTY_EAX30LISTENER_DECAYHFRATIO,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3L
    },
    {
    	&DSPROPSETID_EAX30_ListenerProperties,
    	DSPROPERTY_EAX30LISTENER_DECAYLFRATIO,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3L
    },
    {
    	&DSPROPSETID_EAX30_ListenerProperties,
    	DSPROPERTY_EAX30LISTENER_REFLECTIONS,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3L
    },
    {
    	&DSPROPSETID_EAX30_ListenerProperties,
    	DSPROPERTY_EAX30LISTENER_REFLECTIONSDELAY,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3L
    },
    {
    	&DSPROPSETID_EAX30_ListenerProperties,
    	DSPROPERTY_EAX30LISTENER_REFLECTIONSPAN,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3L
    },
    {
    	&DSPROPSETID_EAX30_ListenerProperties,
    	DSPROPERTY_EAX30LISTENER_REVERB,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3L
    },
    {
    	&DSPROPSETID_EAX30_ListenerProperties,
    	DSPROPERTY_EAX30LISTENER_REVERBDELAY,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3L
    },
    {
    	&DSPROPSETID_EAX30_ListenerProperties,
    	DSPROPERTY_EAX30LISTENER_REVERBPAN,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3L
    },
    {
    	&DSPROPSETID_EAX30_ListenerProperties,
    	DSPROPERTY_EAX30LISTENER_ENVIRONMENT,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3L
    },
    {
    	&DSPROPSETID_EAX30_ListenerProperties,
    	DSPROPERTY_EAX30LISTENER_ENVIRONMENTSIZE,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3L
    },
    {
    	&DSPROPSETID_EAX30_ListenerProperties,
    	DSPROPERTY_EAX30LISTENER_ENVIRONMENTDIFFUSION,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3L
    },
    {
    	&DSPROPSETID_EAX30_ListenerProperties,
    	DSPROPERTY_EAX30LISTENER_ECHOTIME,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3L
    },
    {
    	&DSPROPSETID_EAX30_ListenerProperties,
    	DSPROPERTY_EAX30LISTENER_ECHODEPTH,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3L
    },
    {
    	&DSPROPSETID_EAX30_ListenerProperties,
    	DSPROPERTY_EAX30LISTENER_MODULATIONTIME,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3L
    },
    {
    	&DSPROPSETID_EAX30_ListenerProperties,
    	DSPROPERTY_EAX30LISTENER_MODULATIONDEPTH,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3L
    },
    {
    	&DSPROPSETID_EAX30_ListenerProperties,
    	DSPROPERTY_EAX30LISTENER_HFREFERENCE,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3L
    },
    {
    	&DSPROPSETID_EAX30_ListenerProperties,
    	DSPROPERTY_EAX30LISTENER_LFREFERENCE,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3L
    },
    {
    	&DSPROPSETID_EAX30_ListenerProperties,
    	DSPROPERTY_EAX30LISTENER_AIRABSORPTIONHF,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3L
    },
    {
    	&DSPROPSETID_EAX30_ListenerProperties,
    	DSPROPERTY_EAX30LISTENER_FLAGS,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3L
    },
    // eax 3.0 'buffer'
    {
    	&DSPROPSETID_EAX30_BufferProperties,
    	DSPROPERTY_EAX30BUFFER_NONE,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3B
    },
    {
    	&DSPROPSETID_EAX30_BufferProperties,
    	DSPROPERTY_EAX30BUFFER_ALLPARAMETERS,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3B
    },
    {
    	&DSPROPSETID_EAX30_BufferProperties,
    	DSPROPERTY_EAX30BUFFER_OBSTRUCTIONPARAMETERS,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3B
    },
    {
    	&DSPROPSETID_EAX30_BufferProperties,
    	DSPROPERTY_EAX30BUFFER_OCCLUSIONPARAMETERS,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3B
    },
    {
    	&DSPROPSETID_EAX30_BufferProperties,
    	DSPROPERTY_EAX30BUFFER_EXCLUSIONPARAMETERS,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3B
    },
    {
    	&DSPROPSETID_EAX30_BufferProperties,
    	DSPROPERTY_EAX30BUFFER_DIRECT,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3B
    },
    {
    	&DSPROPSETID_EAX30_BufferProperties,
    	DSPROPERTY_EAX30BUFFER_DIRECTHF,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3B
    },
    {
    	&DSPROPSETID_EAX30_BufferProperties,
    	DSPROPERTY_EAX30BUFFER_ROOM,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3B
    },
    {
    	&DSPROPSETID_EAX30_BufferProperties,
    	DSPROPERTY_EAX30BUFFER_ROOMHF,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3B
    },
    {
    	&DSPROPSETID_EAX30_BufferProperties,
    	DSPROPERTY_EAX30BUFFER_ROOMROLLOFFFACTOR,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3B
    },
    {
    	&DSPROPSETID_EAX30_BufferProperties,
    	DSPROPERTY_EAX30BUFFER_OBSTRUCTION,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3B
    },
    {
    	&DSPROPSETID_EAX30_BufferProperties,
    	DSPROPERTY_EAX30BUFFER_OBSTRUCTIONLFRATIO,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3B
    },
    {
    	&DSPROPSETID_EAX30_BufferProperties,
    	DSPROPERTY_EAX30BUFFER_OCCLUSION,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3B
    },
    {
    	&DSPROPSETID_EAX30_BufferProperties,
    	DSPROPERTY_EAX30BUFFER_OCCLUSIONLFRATIO,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3B
    },
    {
    	&DSPROPSETID_EAX30_BufferProperties,
    	DSPROPERTY_EAX30BUFFER_OCCLUSIONROOMRATIO,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3B
    },
    {
    	&DSPROPSETID_EAX30_BufferProperties,
    	DSPROPERTY_EAX30BUFFER_OCCLUSIONDIRECTRATIO,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3B
    },
    {
    	&DSPROPSETID_EAX30_BufferProperties,
    	DSPROPERTY_EAX30BUFFER_EXCLUSION,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3B
    },
    {
    	&DSPROPSETID_EAX30_BufferProperties,
    	DSPROPERTY_EAX30BUFFER_EXCLUSIONLFRATIO,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3B
    },
    {
    	&DSPROPSETID_EAX30_BufferProperties,
    	DSPROPERTY_EAX30BUFFER_OUTSIDEVOLUMEHF,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3B
    },
    {
    	&DSPROPSETID_EAX30_BufferProperties,
    	DSPROPERTY_EAX30BUFFER_AIRABSORPTIONFACTOR,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3B
    },
    {
    	&DSPROPSETID_EAX30_BufferProperties,
    	DSPROPERTY_EAX30BUFFER_DOPPLERFACTOR,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3B
    },
    {
    	&DSPROPSETID_EAX30_BufferProperties,
    	DSPROPERTY_EAX30BUFFER_ROLLOFFFACTOR,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3B
    },
    {
    	&DSPROPSETID_EAX30_BufferProperties,
    	DSPROPERTY_EAX30BUFFER_FLAGS,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyEAX3B
    },
// --------------
    // ------- I3DL2 properties
    // I3DL2 'listner'
    {
    	&DSPROPSETID_I3DL2_ListenerProperties,
    	DSPROPERTY_I3DL2LISTENER_ALL,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyI3DL2L
    },
    {
    	&DSPROPSETID_I3DL2_ListenerProperties,
    	DSPROPERTY_I3DL2LISTENER_ROOM,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyI3DL2L
    },
    {
    	&DSPROPSETID_I3DL2_ListenerProperties,
    	DSPROPERTY_I3DL2LISTENER_ROOMHF,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyI3DL2L
    },
    {
    	&DSPROPSETID_I3DL2_ListenerProperties,
    	DSPROPERTY_I3DL2LISTENER_ROOMROLLOFFFACTOR,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyI3DL2L
    },
    {
    	&DSPROPSETID_I3DL2_ListenerProperties,
    	DSPROPERTY_I3DL2LISTENER_DECAYTIME,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyI3DL2L
    },
    {
    	&DSPROPSETID_I3DL2_ListenerProperties,
    	DSPROPERTY_I3DL2LISTENER_DECAYHFRATIO,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyI3DL2L
    },
    {
    	&DSPROPSETID_I3DL2_ListenerProperties,
    	DSPROPERTY_I3DL2LISTENER_REFLECTIONS,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyI3DL2L
    },
    {
    	&DSPROPSETID_I3DL2_ListenerProperties,
    	DSPROPERTY_I3DL2LISTENER_REFLECTIONSDELAY,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyI3DL2L
    },
    {
    	&DSPROPSETID_I3DL2_ListenerProperties,
    	DSPROPERTY_I3DL2LISTENER_REVERB,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyI3DL2L
    },
    {
    	&DSPROPSETID_I3DL2_ListenerProperties,
    	DSPROPERTY_I3DL2LISTENER_REVERBDELAY,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyI3DL2L
    },
    {
    	&DSPROPSETID_I3DL2_ListenerProperties,
    	DSPROPERTY_I3DL2LISTENER_DIFFUSION,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyI3DL2L
    },
    {
    	&DSPROPSETID_I3DL2_ListenerProperties,
    	DSPROPERTY_I3DL2LISTENER_DENSITY,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyI3DL2L
    },
    {
    	&DSPROPSETID_I3DL2_ListenerProperties,
    	DSPROPERTY_I3DL2LISTENER_HFREFERENCE,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyI3DL2L
    },
    // I3DL2 'buffer'
    {
    	&DSPROPSETID_I3DL2_BufferProperties,
    	DSPROPERTY_I3DL2BUFFER_ALL,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyI3DL2B
    },
    {
    	&DSPROPSETID_I3DL2_BufferProperties,
    	DSPROPERTY_I3DL2BUFFER_DIRECT,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyI3DL2B
    },
    {
    	&DSPROPSETID_I3DL2_BufferProperties,
    	DSPROPERTY_I3DL2BUFFER_DIRECTHF,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyI3DL2B
    },
    {
    	&DSPROPSETID_I3DL2_BufferProperties,
    	DSPROPERTY_I3DL2BUFFER_ROOM,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyI3DL2B
    },
    {
    	&DSPROPSETID_I3DL2_BufferProperties,
    	DSPROPERTY_I3DL2BUFFER_ROOMHF,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyI3DL2B
    },
    {
    	&DSPROPSETID_I3DL2_BufferProperties,
    	DSPROPERTY_I3DL2BUFFER_ROOMROLLOFFFACTOR,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyI3DL2B
    },
    {
    	&DSPROPSETID_I3DL2_BufferProperties,
    	DSPROPERTY_I3DL2BUFFER_OBSTRUCTION,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyI3DL2B
    },
    {
    	&DSPROPSETID_I3DL2_BufferProperties,
    	DSPROPERTY_I3DL2BUFFER_OBSTRUCTIONLFRATIO,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyI3DL2B
    },
    {
    	&DSPROPSETID_I3DL2_BufferProperties,
    	DSPROPERTY_I3DL2BUFFER_OCCLUSION,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyI3DL2B
    },
    {
    	&DSPROPSETID_I3DL2_BufferProperties,
    	DSPROPERTY_I3DL2BUFFER_OCCLUSIONLFRATIO,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyI3DL2B
    },
    {
    	&DSPROPSETID_I3DL2_BufferProperties,
    	DSPROPERTY_I3DL2BUFFER_OBSTRUCTIONALL,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyI3DL2B
    },
    {
    	&DSPROPSETID_I3DL2_BufferProperties,
    	DSPROPERTY_I3DL2BUFFER_OCCLUSIONALL,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveOutStream::PropertyI3DL2B
    },
// ---------------
    { // pin
        &KSPROPSETID_Audio,
        KSPROPERTY_AUDIO_QUALITY,
        KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWave::PropertySamplingRate
    },
    { // pin
        &KSPROPSETID_Audio, 
        KSPROPERTY_AUDIO_VOLUMELEVEL,
        KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET | KSPROPERTY_TYPE_BASICSUPPORT,
        CMiniportWave::PropertyVolume
    },
    { // my property
        &KSPROPSETID_Private,
        KSPROPERTY_PRIVATE,
        KSPROPERTY_TYPE_GET|KSPROPERTY_TYPE_SET,
        CMiniportWave::PropertyPrivate
    },
// --------------
    { // filter
        &KSPROPSETID_Audio,
        KSPROPERTY_AUDIO_DYNAMIC_SAMPLING_RATE,
        KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWave::PropertySamplingRate
    },
    { // filter
        &KSPROPSETID_Audio,
        KSPROPERTY_AUDIO_MIX_LEVEL_CAPS,
        KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_BASICSUPPORT,
        CMiniportWave::PropertyMixLevelCaps
    },
    { // filter
        &KSPROPSETID_Audio,
        KSPROPERTY_AUDIO_MIX_LEVEL_TABLE,
        KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET | KSPROPERTY_TYPE_BASICSUPPORT,
        CMiniportWave::PropertyMixLevel
    },
   { // filter
        &KSPROPSETID_Audio,
	KSPROPERTY_AUDIO_CHANNEL_CONFIG,
        KSPROPERTY_TYPE_SET|KSPROPERTY_TYPE_GET|KSPROPERTY_TYPE_BASICSUPPORT,
        CMiniportWave::PropertyChannelConfig
   },
   {
   	&KSPROPSETID_Audio,
   	KSPROPERTY_AUDIO_STEREO_SPEAKER_GEOMETRY,
   	KSPROPERTY_TYPE_SET|KSPROPERTY_TYPE_GET|KSPROPERTY_TYPE_BASICSUPPORT,
   	CMiniportWave::PropertySpeakerGeometry
   }
};

static PCPROPERTY_ITEM PropertiesFilter[] = 
{
    { // any
        &KSPROPSETID_Audio,
        KSPROPERTY_AUDIO_CPU_RESOURCES,
	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_BASICSUPPORT,
        CMiniportWave::PropertyCpuResources
    }
};

DEFINE_PCAUTOMATION_TABLE_PROP (Automation, Properties);
#define AutomationDAC Automation
#define Automation3D Automation
#define AutomationSRC Automation
#define AutomationVolume Automation

DEFINE_PCAUTOMATION_TABLE_PROP (AutomationFilter, PropertiesFilter);

/*****************************************************************************
 * TopologyNodes
 *****************************************************************************
 * List of nodes.
 */

static
PCNODE_DESCRIPTOR MiniportNodes[] =
{
    {
        0,                      // Flags
        &AutomationDAC,         // AutomationTable
        &KSNODETYPE_DAC,        // Type
        NULL			// Name 
    },
    {
        0,                      // Flags
        NULL,                   // AutomationTable
        &KSNODETYPE_SUM,        // Type
        &TOPO_WAVEOUT01_NAME// Name
    }, 
    {
        0,                      // Flags
        &AutomationVolume,      // AutomationTable
        &KSNODETYPE_VOLUME,     // Type
        &TOPO_WAVEOUT01_VOLUME_NAME      // Name 
    },
    {
        0,                      // Flags
        NULL,                   // AutomationTable
        &KSNODETYPE_SUPERMIX,   // Type
        &TOPO_WAVEOUT01_NAME// Name 
    },
    {
        0,                      // Flags
        &AutomationVolume,	// AutomationTable
        &KSNODETYPE_VOLUME,     // Type
        &TOPO_WAVEOUT01_VOLUME_NAME	// Name 
    },
    {
        0,                      // Flags
        &AutomationSRC,         // AutomationTable
        &KSNODETYPE_SRC,        // Type
        NULL                    // Name
    },

    // these are for recording and SPDIF and 3-D only
    {
        0,                      // Flags
        NULL,                   // AutomationTable
        &KSNODETYPE_ADC,        // Type
        NULL			// Name 
    },
    {
    	0,
    	NULL,
    	&KSNODETYPE_SPDIF_INTERFACE,
    	NULL
    },
    {
    	0,
    	&Automation3D,		// DirectSound3D
    	&KSNODETYPE_3D_EFFECTS,
    	NULL
    }
};

enum
{
 WAVE_DAC=0,
 WAVE_SUM,
 WAVE_VOLUME1=2,
 WAVE_SUPERMIX,
 WAVE_VOLUME2=4,
 WAVE_SRC,
 // these are for recording and spdif and 3-d only
 WAVE_ADC,
 WAVE_SPDIF,
 WAVE_3D_EFFECT
};

/*****************************************************************************
 * MiniportConnections
 *****************************************************************************
 * List of connections.
 */
static
PCCONNECTION_DESCRIPTOR MiniportConnectionsHiFi[] =
{
    { PCFILTER_NODE,  WAVE_NODE_WAVE_OUT,  WAVE_VOLUME1,	    1 },    // DirectSound sequence
    { WAVE_VOLUME1,   0,  WAVE_3D_EFFECT,   1 },    // DirectSound 3D
    { WAVE_3D_EFFECT, 0,  WAVE_SUPERMIX,    1 },
    { WAVE_SUPERMIX,  0,  WAVE_VOLUME2,     1 },
    { WAVE_VOLUME2,   0,  WAVE_SRC,         1 },
    { WAVE_SRC,       0,  WAVE_SUM,	    1 },
    { WAVE_SUM,       0,  WAVE_DAC,         1 },    // Stream in to DAC.
    { WAVE_DAC,       0,  PCFILTER_NODE,    WAVE_NODE_OUTPUT },    // DAC to Bridge.

    { PCFILTER_NODE,  WAVE_NODE_SPDIF,WAVE_SPDIF,	     1 },
#ifdef USE_WAVE_NODE_SPDIFOUTPUT
    { WAVE_SPDIF,     0,  PCFILTER_NODE,	    WAVE_NODE_SPDIFOUTPUT } ,
#else
    { WAVE_SPDIF,     0,  WAVE_SUM,	    1} ,
#endif
    { PCFILTER_NODE,  WAVE_NODE_INPUT,  WAVE_ADC,         1 },    // Bridge in to ADC.
    { WAVE_ADC,       0,  PCFILTER_NODE,    WAVE_NODE_WAVE_IN }    // ADC to stream pin (capture).
};

static
PCCONNECTION_DESCRIPTOR MiniportConnectionsGen[] =
{
    { PCFILTER_NODE,  WAVE_NODE_WAVE_OUT,  WAVE_VOLUME1,	    1 },    // DirectSound sequence
    { WAVE_VOLUME1,   0,  WAVE_SUPERMIX,    1 },
    { WAVE_SUPERMIX,  0,  WAVE_VOLUME2,     1 },
    { WAVE_VOLUME2,   0,  WAVE_SRC,         1 },
    { WAVE_SRC,       0,  WAVE_SUM,	    1 },
    { WAVE_SUM,       0,  WAVE_DAC,         1 },    // Stream in to DAC.
    { WAVE_DAC,       0,  PCFILTER_NODE,    WAVE_NODE_OUTPUT }    // DAC to Bridge.
};

/*****************************************************************************
 * MiniportFilterDescriptor
 *****************************************************************************
 * Complete miniport description.
 */
static
PCFILTER_DESCRIPTOR 
MiniportFilterDescriptor23 =
{
    0,                                  // Version
    &AutomationFilter,                  // AutomationTable
    sizeof(PCPIN_DESCRIPTOR),           // PinSize
    SIZEOF_ARRAY(MiniportPins23),         // PinCount
    MiniportPins23,                       // Pins
    sizeof(PCNODE_DESCRIPTOR),          // NodeSize
    SIZEOF_ARRAY(MiniportNodes)-3,        // NodeCount
    MiniportNodes,                      // Nodes
    SIZEOF_ARRAY(MiniportConnectionsGen),  // ConnectionCount
    MiniportConnectionsGen,                // Connections
    0,                                  // CategoryCount
    NULL                                // Categories
};

static
PCFILTER_DESCRIPTOR 
MiniportFilterDescriptor45 =
{
    0,                                  // Version
    &AutomationFilter,                  // AutomationTable
    sizeof(PCPIN_DESCRIPTOR),           // PinSize
    SIZEOF_ARRAY(MiniportPins45),         // PinCount
    MiniportPins45,                       // Pins
    sizeof(PCNODE_DESCRIPTOR),          // NodeSize
    SIZEOF_ARRAY(MiniportNodes)-3,        // NodeCount
    MiniportNodes,                      // Nodes
    SIZEOF_ARRAY(MiniportConnectionsGen),  // ConnectionCount
    MiniportConnectionsGen,                // Connections
    0,                                  // CategoryCount
    NULL                                // Categories
};

static
PCFILTER_DESCRIPTOR 
MiniportFilterDescriptor67 =
{
    0,                                  // Version
    &AutomationFilter,                  // AutomationTable
    sizeof(PCPIN_DESCRIPTOR),           // PinSize
    SIZEOF_ARRAY(MiniportPins67),         // PinCount
    MiniportPins67,                       // Pins
    sizeof(PCNODE_DESCRIPTOR),          // NodeSize
    SIZEOF_ARRAY(MiniportNodes)-3,        // NodeCount
    MiniportNodes,                      // Nodes
    SIZEOF_ARRAY(MiniportConnectionsGen),  // ConnectionCount
    MiniportConnectionsGen,                // Connections
    0,                                  // CategoryCount
    NULL                                // Categories
};



static
PCFILTER_DESCRIPTOR 
MiniportFilterDescriptorHiFi =
{
    0,                                  // Version
    &AutomationFilter,                  // AutomationTable
    sizeof(PCPIN_DESCRIPTOR),           // PinSize
    SIZEOF_ARRAY(MiniportPinsHiFi),     // PinCount
    MiniportPinsHiFi,                   // Pins
    sizeof(PCNODE_DESCRIPTOR),          // NodeSize
    SIZEOF_ARRAY(MiniportNodes),        // NodeCount
    MiniportNodes,                      // Nodes
    SIZEOF_ARRAY(MiniportConnectionsHiFi),  // ConnectionCount
    MiniportConnectionsHiFi,                // Connections
    0,                                  // CategoryCount
    NULL                                // Categories
};
