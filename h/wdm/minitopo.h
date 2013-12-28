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


#ifndef _MINITOPO_H_
#define _MINITOPO_H_

class CMiniportTopology
:   
    public IMiniportTopology,
    public IPowerNotify,
    public CUnknown
    // ,public IPinName     // Windows 7-specific, 3551
{
public:
    SAFE_DESTRUCTORS

    PADAPTERCOMMON      AdapterCommon;      // Adapter common object.
    PPORTEVENTS         PortEvents;
    PPORTTOPOLOGY       Port;               // Callback interface.

    // Topology "mixer" values
    #define MAX_TOPOLOGY_ITEMS 19
    #define MAX_TOPOLOGY_CHANNELS 6
    dword mixer_data[MAX_TOPOLOGY_ITEMS][MAX_TOPOLOGY_CHANNELS];
    int magic;

    DECLARE_STD_UNKNOWN();
    DEFINE_STD_CONSTRUCTOR(CMiniportTopology);

    IMP_IPowerNotify;
    // 3551: IMP_IPinName;

    ~CMiniportTopology();

    kx_hw *hw;

    DEVICE_POWER_STATE      PowerState;

    STDMETHODIMP 
    GetDescription
    (   OUT     PPCFILTER_DESCRIPTOR *  OutFilterDescriptor
    );
    STDMETHODIMP 
    DataRangeIntersection
    (   IN      ULONG           PinId
    ,   IN      PKSDATARANGE    DataRange
    ,   IN      PKSDATARANGE    MatchingDataRange
    ,   IN      ULONG           OutputBufferLength
    ,   OUT     PVOID           ResultantFormat     OPTIONAL
    ,   OUT     PULONG          ResultantFormatLength
    );
    STDMETHODIMP Init
    (
        IN      PUNKNOWN        UnknownAdapter,
        IN      PRESOURCELIST   ResourceList,
        IN      PPORTTOPOLOGY   Port
    );
    STDMETHODIMP_(void) ServiceEvent (int mask);
    static NTSTATUS EventHandler(IN      PPCEVENT_REQUEST      EventRequest);
    static NTSTATUS PropertyCpuResources(IN PPCPROPERTY_REQUEST   PropertyRequest);
    static NTSTATUS PropertyLevel(IN PPCPROPERTY_REQUEST PropertyRequest);
    static NTSTATUS PropertyOnOff(IN PPCPROPERTY_REQUEST PropertyRequest);
    static NTSTATUS PropertyPrivate(IN PPCPROPERTY_REQUEST PropertyRequest);
    static NTSTATUS PropertySuperMixTable(IN PPCPROPERTY_REQUEST PropertyRequest);
    static NTSTATUS PropertySuperMixCaps(IN PPCPROPERTY_REQUEST PropertyRequest);
};

NTSTATUS BasicSupportHandler(IN PPCPROPERTY_REQUEST   PropertyRequest);

#endif
