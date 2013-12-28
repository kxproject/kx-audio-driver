// kX WDM Audio Driver
// Copyright (c) Eugene Gavrilov, 2003-2014.
// All rights reserved

// p16v / alice3 

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
KSDATARANGE_AUDIO PinDataRangesStreamRecording[6] =
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
        32,     // Minimum number of bits per sample.
        32,     // Maximum number of bits per channel.
        96000,  // Minimum rate.
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
         32,
         32,
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
        32,      // Minimum number of bits per sample.
        32,     // Maximum number of bits per channel.
        48000,   // Minimum rate.
        48000// Maximum rate.
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
         32,
         32,
         48000,
         48000
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
        32,      // Minimum number of bits per sample.
        32,     // Maximum number of bits per channel.
        44100,   // Minimum rate.
        44100// Maximum rate.
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
         32,
         32,
         44100,
         44100
    }
};

static
KSDATARANGE_AUDIO PinDataRangesStreamPlayback[8] =
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
        8,      // Max number of channels. fixed
        32,     // Minimum number of bits per sample.
        32,     // Maximum number of bits per channel.
        96000,  // Minimum rate.
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
         8,
         32, 
         32,
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
        8,      // Max number of channels. fixed
        32,     // Minimum number of bits per sample.
        32,     // Maximum number of bits per channel.
        192000,  // Minimum rate.
        192000   // Maximum rate.
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
         8,
         32, 
         32,
         192000,
         192000
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
        8,      // Max number of channels. fixed
        32,     // Minimum number of bits per sample.
        32,    // Maximum number of bits per channel.
        44100,  // Minimum rate.
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
         8,
         32, 
         32,
         44100,
         44100
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
        8,      // Max number of channels. fixed
        32,     // Minimum number of bits per sample.
        32,     // Maximum number of bits per channel.
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
            STATICGUIDOF(KSDATAFORMAT_SPECIFIER_DSOUND)
         },
         8,
         32, 
         32,
         48000,
         48000
    }
};


static PKSDATARANGE PinDataRangePointersPlayback[8] =
{
    PKSDATARANGE(&PinDataRangesStreamPlayback[0]),
    PKSDATARANGE(&PinDataRangesStreamPlayback[1]),
    PKSDATARANGE(&PinDataRangesStreamPlayback[2]),
    PKSDATARANGE(&PinDataRangesStreamPlayback[3]),
    PKSDATARANGE(&PinDataRangesStreamPlayback[4]),
    PKSDATARANGE(&PinDataRangesStreamPlayback[5]),
    PKSDATARANGE(&PinDataRangesStreamPlayback[6]),
    PKSDATARANGE(&PinDataRangesStreamPlayback[7])
};

static PKSDATARANGE PinDataRangePointersRecording[6] =
{
    PKSDATARANGE(&PinDataRangesStreamRecording[0]),
    PKSDATARANGE(&PinDataRangesStreamRecording[1]),
    PKSDATARANGE(&PinDataRangesStreamRecording[2]),
    PKSDATARANGE(&PinDataRangesStreamRecording[3]),
    PKSDATARANGE(&PinDataRangesStreamRecording[4]),
    PKSDATARANGE(&PinDataRangesStreamRecording[5])
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
MiniportPins[] =
{
    // Wave Out Streaming Pin (Renderer)
    {
        1,1,0,
        NULL,
        {
            0,
            NULL,
            0,
            NULL,
            SIZEOF_ARRAY(PinDataRangePointersPlayback),
            PinDataRangePointersPlayback,
            KSPIN_DATAFLOW_IN,
            KSPIN_COMMUNICATION_SINK,
            &KSNODETYPE_LEGACY_AUDIO_CONNECTOR,
            &TOPO_WAVEOUTHQ_NAME,
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
            &TOPO_WAVEOUTHQ_NAME,
            0
        }
    },
    // Wave In Streaming Pin (Capture)
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
            &KSNODETYPE_LEGACY_AUDIO_CONNECTOR,
            &TOPO_WAVEINHQ_NAME,  // this name shows up as the recording panel name in SoundVol.
            0
        }
    },

    // Wave In Bridge Pin (Capture - To Topology)
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
            &KSNODETYPE_LINE_CONNECTOR,
            &TOPO_WAVEINHQ_NAME,
            0
        }
    }
};

static PCPROPERTY_ITEM Properties[] =
{
    { // any
        &KSPROPSETID_Audio,
        KSPROPERTY_AUDIO_CPU_RESOURCES,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_BASICSUPPORT,
        CMiniportWaveHQ::PropertyCpuResources
    },
    { // pin
        &KSPROPSETID_Audio, 
        KSPROPERTY_AUDIO_VOLUMELEVEL,
        KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET | KSPROPERTY_TYPE_BASICSUPPORT,
        CMiniportWaveHQ::PropertyVolume
    },
    { // pin
        &KSPROPSETID_Audio,
        KSPROPERTY_AUDIO_SAMPLING_RATE,
        KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveHQ::PropertySamplingRate
    },
    { // filter
        &KSPROPSETID_Audio,
        KSPROPERTY_AUDIO_DYNAMIC_SAMPLING_RATE,
        KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET,
        CMiniportWaveHQ::PropertySamplingRate
    }
};

static PCPROPERTY_ITEM PropertiesFilter[] = 
{
    { // any
        &KSPROPSETID_Audio,
        KSPROPERTY_AUDIO_CPU_RESOURCES,
    	KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_BASICSUPPORT,
        CMiniportWaveHQ::PropertyCpuResources
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
        &AutomationSRC,         // AutomationTable
        &KSNODETYPE_SRC,        // Type
        NULL                    // Name
    },
    {
        0,                      // Flags
        &AutomationVolume,      // AutomationTable
        &KSNODETYPE_VOLUME,     // Type
        &TOPO_WAVEOUTHQ_VOLUME_NAME      // Name 
    },
    {
        0,                      // Flags
        NULL,                   // AutomationTable
        &KSNODETYPE_SUPERMIX,   // Type
        &TOPO_WAVEOUTHQ_NAME    // Name 
    },
    {
        0,                      // Flags
        &AutomationVolume,	// AutomationTable
        &KSNODETYPE_VOLUME,     // Type
        &TOPO_WAVEOUTHQ_VOLUME_NAME	// Name 
    },
    {
        0,                      // Flags
        NULL,                   // AutomationTable
        &KSNODETYPE_SUM,        // Type
        &TOPO_WAVEOUTHQ_NAME// Name
    }, 
    {
        0,                      // Flags
        NULL,                   // AutomationTable
        &KSNODETYPE_ADC,        // Type
        NULL			// Name 
    }
};

enum
{
 WAVE_DAC=0,
 WAVE_SRC,
 WAVE_VOLUME1=2,
 WAVE_SUPERMIX,
 WAVE_VOLUME2=4,
 WAVE_SUM,
 WAVE_ADC
};


/*****************************************************************************
 * MiniportConnections
 *****************************************************************************
 * List of connections.
 */
static
PCCONNECTION_DESCRIPTOR MiniportConnections[] =
{
    { PCFILTER_NODE,  WAVE_NODE_WAVE_OUT,  WAVE_VOLUME1,	    1 },    // DirectSound sequence
    { WAVE_VOLUME1,   0,  WAVE_SUPERMIX,    1 },
    { WAVE_SUPERMIX,  0,  WAVE_VOLUME2,     1 },
    { WAVE_VOLUME2,   0,  WAVE_SRC,         1 },
    { WAVE_SRC,       0,  WAVE_SUM,	    1 },
    { WAVE_SUM,       0,  WAVE_DAC,         1 },    // Stream in to DAC.
    { WAVE_DAC,       0,  PCFILTER_NODE,    WAVE_NODE_OUTPUT },    // DAC to Bridge.

    { PCFILTER_NODE,  WAVE_NODE_INPUT,  WAVE_ADC,         1 },    // Bridge in to ADC.
    { WAVE_ADC,       0,  PCFILTER_NODE,    WAVE_NODE_WAVE_IN }    // ADC to stream pin (capture).
};

/*****************************************************************************
 * MiniportFilterDescriptor
 *****************************************************************************
 * Complete miniport description.
 */
static
PCFILTER_DESCRIPTOR 
MiniportFilterDescriptor =
{
    0,                                  // Version
    &AutomationFilter,                  // AutomationTable
    sizeof(PCPIN_DESCRIPTOR),           // PinSize
    SIZEOF_ARRAY(MiniportPins),         // PinCount
    MiniportPins,                       // Pins
    sizeof(PCNODE_DESCRIPTOR),          // NodeSize
    SIZEOF_ARRAY(MiniportNodes),        // NodeCount
    MiniportNodes,                      // Nodes
    SIZEOF_ARRAY(MiniportConnections),  // ConnectionCount
    MiniportConnections,                // Connections
    0,                                  // CategoryCount
    NULL                                // Categories
};
