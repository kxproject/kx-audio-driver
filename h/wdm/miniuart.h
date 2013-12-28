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


#ifndef _MINIUART_H_
#define _MINIUART_H_

class CMiniportUart :public IMiniportMidi, public IPowerNotify, public CUnknown
{
    SAFE_DESTRUCTORS
public:
    PADAPTERCOMMON      AdapterCommon;      // Adapter common object.
    PPORTMIDI           Port;               // Callback interface.
    PSERVICEGROUP	ServiceGroup;    

    int capture_streams;
    int render_streams;

    int uart_number;

    IMP_IPowerNotify;
    DEVICE_POWER_STATE      PowerState;

    kx_hw *hw;

    DECLARE_STD_UNKNOWN();
    DEFINE_STD_CONSTRUCTOR(CMiniportUart);

    IMP_IMiniportMidi;


    ~CMiniportUart();
};

class CMiniportMidiStreamUart
:   public IMiniportMidiStream,
    public CUnknown
{
    SAFE_DESTRUCTORS

    CMiniportUart * Miniport;            // Parent.
    BOOLEAN         Capture;             // Whether this is capture.
    KSSTATE	state;
   

public:
    DECLARE_STD_UNKNOWN();
    DEFINE_STD_CONSTRUCTOR(CMiniportMidiStreamUart);

    ~CMiniportMidiStreamUart();

    kx_hw *hw;
    void *instance;
    int inited;

    IMP_IMiniportMidiStream;

    #define SYSEX_BUFFER_SIZE	32
    byte sysex_buffer[SYSEX_BUFFER_SIZE];
    int sysex_pos;
    int sysex_state;
    // states:
    #define SYSEX_NONE		0
    #define SYSEX_PARSING	1
    #define SYSEX_OVERFLOW	2

    // my function
    NTSTATUS Init(IN CMiniportUart * pMiniport,BOOLEAN Capture_);
};

#endif
