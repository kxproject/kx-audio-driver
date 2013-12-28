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


#ifndef _ADAPTER_H_
#define _ADAPTER_H_

DECLARE_INTERFACE_(IAdapterCommon,IUnknown)
{
    DEFINE_ABSTRACT_UNKNOWN()           // For IUnknown

    STDMETHOD_(NTSTATUS,Init)
    (   THIS_
        IN      PRESOURCELIST   ResourceList,
        IN      PDEVICE_OBJECT  DeviceObject
    )   PURE;
    
    STDMETHOD_(PINTERRUPTSYNC,GetInterruptSync)
    (   THIS
    )   PURE;
};

typedef IAdapterCommon *PADAPTERCOMMON;


NTSTATUS NewAdapterCommon(
    OUT     PUNKNOWN *  Unknown,
    IN      REFCLSID,
    IN      PUNKNOWN    UnknownOuter    OPTIONAL,
    IN      POOL_TYPE   PoolType
);

class CGSIFInterface;

class CAdapterCommon
:   public IAdapterCommon,
    public IAdapterPowerManagement,
    public IAdapterPowerManagement2, // 3551
    public CUnknown
    
{
    SAFE_DESTRUCTORS

public:
    DECLARE_STD_UNKNOWN();
    DEFINE_STD_CONSTRUCTOR(CAdapterCommon);
    ~CAdapterCommon();

    PINTERRUPTSYNC          InterruptSync;
    DEVICE_POWER_STATE      PowerState;
    PDEVICE_OBJECT  	    DeviceObject;

    CMiniportTopology *Topology;

    PDEVICE_OBJECT physical_device_object;

    CMiniportUart *Uart[MAX_MPU_DEVICES];
    #define MAX_WAVE_DEVICES 4
    CMiniportWaveHQ *WaveHQ;
    CMiniportWave *Wave[MAX_WAVE_DEVICES];
    #define MAX_SYNTH_DEVICES 2
    CMiniportSynth *Synth[MAX_SYNTH_DEVICES];
    // NOTE: MAX_.._DEVICES are ignored in ~CAdapterCommon

    CMiniportCtrl *Ctrl;

    int magic;

    int synth_count;
    int uart_count;
    int wave_count;

    int is_vista;

    kx_hw *hw;
    unsigned long m_io_base;
    int m_irq;

    KSPIN_LOCK dpc_lock;

    wchar_t device_name[256];

    void ResetSettings(kx_callbacks *cb=NULL);
    int my_init(dword device,dword subsys,byte chip_rev,
      byte bus,byte dev,byte func);

    // gsif-related stuff
    UNICODE_STRING gsif_device;
    CGSIFInterface *gsif_interface;

    /*****************************************************************************
     * IAdapterCommon methods
     */
    STDMETHODIMP_(NTSTATUS) Init
    (
        IN      PRESOURCELIST   ResourceList,
        IN      PDEVICE_OBJECT  DeviceObject
    );
    STDMETHODIMP_(PINTERRUPTSYNC) GetInterruptSync
    (   void
    );
    
    /*************************************************************************
     * IAdapterPowerManagement implementation
     *
     * This macro is from PORTCLS.H.  It lists all the interface's functions.
     */
    IMP_IAdapterPowerManagement2; // 3551
    // this is implied by above:
    //   IMP_IAdapterPowerManagement;
};

NTSTATUS InterruptServiceRoutine(IN      PINTERRUPTSYNC  InterruptSync, IN      PVOID           DynamicContext);

#endif
