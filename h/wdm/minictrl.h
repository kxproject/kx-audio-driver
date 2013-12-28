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


#ifndef _MINICTRL_H_
#define _MINICTRL_H_

class CMiniportCtrl :public IMiniportMidi, public IPowerNotify, public CUnknown
{
    SAFE_DESTRUCTORS
public:
    PADAPTERCOMMON      AdapterCommon;      // Adapter common object.
    PPORTMIDI           Port;               // Callback interface.
    PSERVICEGROUP	ServiceGroup;    

    IMP_IPowerNotify;
    DEVICE_POWER_STATE      PowerState;

    int capture_streams;
    int render_streams;

    kx_hw *hw;

    DECLARE_STD_UNKNOWN();
    DEFINE_STD_CONSTRUCTOR(CMiniportCtrl);

    IMP_IMiniportMidi;

    void output_buffer(int size,byte *buff);

    #define MAX_CTRL_BUFFER_SIZE	4096
    byte ctrl_buffer[MAX_CTRL_BUFFER_SIZE];
    int ctrl_pos;

    spinlock_t lock;

    ~CMiniportCtrl();
};

class CMiniportCtrlStream
:   public IMiniportMidiStream,
    public CUnknown
{
    SAFE_DESTRUCTORS

    CMiniportCtrl * Miniport;            // Parent.
    BOOLEAN         Capture;             // Whether this is capture.
    KSSTATE	state;
   
public:
    DECLARE_STD_UNKNOWN();
    DEFINE_STD_CONSTRUCTOR(CMiniportCtrlStream);

    ~CMiniportCtrlStream();

    kx_hw *hw;
    void *instance0,*instance1;
    int inited;

    IMP_IMiniportMidiStream;

    // my function
    NTSTATUS Init(IN CMiniportCtrl * pMiniport,BOOLEAN Capture_);
};

#endif
