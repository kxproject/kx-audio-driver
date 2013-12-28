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


#ifndef _MINISYNTH_H_
#define _MINISYNTH_H_

class CMiniportSynth : 
  public IMiniportMidi, 
  public IPowerNotify,
  public CUnknown
{
    SAFE_DESTRUCTORS

public:
    PADAPTERCOMMON      AdapterCommon;      // Adapter common object.
    PPORTMIDI           Port;
    dword magic;

    int synth_number;
    kx_hw *hw;

    IMP_IMiniportMidi;

    IMP_IPowerNotify;
    DEVICE_POWER_STATE      PowerState;


    DECLARE_STD_UNKNOWN();
    DEFINE_STD_CONSTRUCTOR(CMiniportSynth);
    ~CMiniportSynth();
};

class CMiniportSynthStream : public IMiniportMidiStream, public CUnknown
{
    SAFE_DESTRUCTORS

public:
    CMiniportSynth * Miniport;            // Parent.
    KSSTATE state;
    dword magic;
    kx_midi_state midi;

    kx_hw *hw;

    IMP_IMiniportMidiStream;

    DECLARE_STD_UNKNOWN();
    DEFINE_STD_CONSTRUCTOR(CMiniportSynthStream);
    ~CMiniportSynthStream();

    // my function
    NTSTATUS Init(IN CMiniportSynth * pMiniport);
};

#endif
