//**************************************************************************//
//                          FILE INFORMATION                                //
//                                                                          //
//  (C) Copyright 1997 Nemesys Music Technology Inc.						//
//      All rights reserved. Use of copyright								//
//      notice is precautionary and does imply publication.                 //
//																		    //
//  Created:........11/1/97													//
//	Owner:..........Wayne Scott												//
//																		    //
//	Description:....Global definitions for GigaSampler					    //
//																		    //
//**************************************************************************//
#ifdef __cplusplus
#pragma once
#endif

#ifndef _HWREGDEFS_H
#define _HWREGDEFS_H

#ifndef _T
#define _T(a)    a
#endif

#define	GS_MAXNUM_MIDIDEVICES 16	//Maximum number of midi channels

#define GSCommRegSubKey                 "Software\\TASCAM\\Hardware\\GSIF2DDK\\Settings"
#define GSCommRegLogSubKey              "Software\\TASCAM\\Hardware\\Log"


//Registry Key Stuff
#define HWRegSettings                           _T("Settings")
#define HWRegDebug                              _T("Debug")
#define HWRegLoaderRackVisible                  _T("LoaderRackVisible")
#define HWRegMixerRackVisible                   _T("MixerRackVisible")
#define HWRegFrameLeft                          _T("MainFrameLeft")
#define HWRegFrameTop                           _T("MainFrameTop")
#define HWRegLoaderView                         _T("LoaderView")
#define HWRegPatchEditor                        _T("PatchEditor")
#define HWRegSequencer                          _T("Sequencer")
#define HWRegWaveEditor                         _T("WaveEditor")
#define HWRegMixerMidiChannels                  _T("MixerMidiChannels")
#define HWRegMixerChannelOffset                 _T("ChannelOffset")
#define HWRegTwoChannelView                     _T("TwoChannelView")
#define HWRegColorScheme                        _T("ColorScheme")
#define HWRegUserName                           _T("UserName")
#define HWRegCompanyName                        _T("CompanyName")
#define HWRegInstallDirectory                   _T("InstallDirectory")
#define HWRegPerformance                        _T("Performance")
#define HWRegPerformanceSaveMixerInfo           _T("PeformanceSaveMixerInfo")
#define HWRegPerformanceSaveConfigInfo          _T("PeformanceSaveConfigInfo")
#define HWRegDebugLevel                         _T("DebugLevel")
#define HWRegVCTrace                            _T("VCTrace")
#define HWRegInstrumentPropTop                  _T("InstrumentPropTop")
#define HWRegInstrumentPropLeft                 _T("InstrumentPropLeft")
#define HWRegPerformancePropTop                 _T("PerformancePropTop")
#define HWRegPerformancePropLeft                _T("PerformancePropLeft")
#define HWRegInstrumentLayoutTop                _T("InstrumentLayoutTop")
#define HWRegInstrumentLayoutLeft               _T("InstrumentLayoutLeft")
#define HWRegDiskManagerTop                     _T("DiskManagerTop")
#define HWRegDiskManagerLeft                    _T("DiskManagerLeft")
#define HWRegFirstTimeFlag                      _T("FirstTimeFlag")
#define HWRegWorkingSpaceDefault                _T("GSWrkSpcDefault")
#define HWRegCleanShutDown                      _T("CleanShutDown")
#define HWRegShowKiloPage                       _T("ShowKiloPage")
#define HWRegACBitDepth                         _T("ACBitDepth")
#define HWRegACSyncToMidiStart                  _T("ACSyncToMidiStart")
#define HWRegACFileAutoNumber                   _T("ACFileAutoNumber")
#define HWRegAudioCaptureDlgTop                 _T("AudioCaptureDlgTop")
#define HWRegAudioCaptureDlgLeft                _T("AudioCaptureDlgLeft")
#define HWRegAudioCaptureView                   _T("AudioCaptureView")
#define HWRegAudioCaptureDir                    _T("AudioCaptureDir")
#define HWRegACCaptureBufferSize                _T("ACCaptureBufferSize")
#define HWRegMasterAttenuation                  _T("MasterAttenuation")
#define HWRegRegistrationPrompt                 _T("RegistrationPrompt")
#define HWRegCDKey                              _T("CDKey")
#define HWRegHardwareID                         _T("HardwareID")
#define HWRegCardID                             _T("CardID")
#define HWRegHardwareType                       _T("HardwareType")
#define HWRegLeadFrames                         _T("LeadFrames")
#define HWRegBitWidth                           _T("BitWidth")
#define HWRegChanPerOut							_T("ChannelsPerOut")
#define HWRegMultiMode                          _T("MultiMode")
#define HWRegUpgraded                           _T("Upgraded")

//
// Logging entries
//
#define HWRegLogVXDEntry                        _T("VXDEntry")
#define HWRegLogAtBootEnable                    _T("VXDEnableAtBoot")
#define HWRegLogAtOpenEnable                    _T("VXDEnableAtOpen")
#define HWRegLogWaveStreamLoad                  _T("WaveStreamLoad")

// NOT FOR USE IN PRODUCTION CODE 
#define HWRegDebugTestAccessLevel               _T("DebugTestAccessLevel")


// HWReg 
#define NMSHWRegSettings						_T("Software\\TASCAM\\Hardware\\GSIF2DDK\\Settings")
#define HWRegMidiInDrv                          _T("MidiInDrv")
#define HWRegWaveOutDrv                         _T("WaveOutDrv")
#define HWRegLoopMode                           _T("LoopMode")
#define HWRegAutoConfiguration                  _T("AutoConfiguration")
#define HWRegOutputSampleRate                   _T("OutputSampleRate")
#define HWRegDither                             _T("Dither")
#define HWRegDitherType                         _T("DitherType")
#define HWRegPolyphony                          _T("Polyphony")
#define HWRegTransitionVoices                   _T("TransitionVoices")
#define HWRegInstallState                       _T("InstallState")
#define HWRegActiveWaveOutDev                   _T("ActiveWaveOutDev")
#define HWRegEnableMap                          _T("EnableMap")
#define HWRegInputEnableMap                          _T("InputEnableMap")
#define HWRegMidi2OutputMap                     _T("Midi2OutputMap")
#define HWRegGSIFDriver			                _T("GSIFDriver")
#define HWRegActiveOuts			                _T("nActiveOuts")
#define HWRegPort0InstanceCnt					_T("Port0Instance") 
#define HWRegPort1InstanceCnt					_T("Port1Instance") 
#define HWRegPort2InstanceCnt					_T("Port2Instance") 
#define HWRegPort3InstanceCnt					_T("Port3Instance") 
#define HWRegGSIFWaveOutDrv						_T("nWaveOutDrvId") 
#define HWRegGSIFReqOutBufSize					_T("OutputBufSize") 
#define HWRegDitherWarning						_T("DitherWarning") 

#define HWRegCheckSum						    _T("LastCheckSum") 

#define HWRegMIDIOutEnableMap                   _T("GSIFMidiOutRouting")
#define HWRegMIDIInEnableMap                   _T("GSIFMidiInRouting")

        
//official Midi out names
//
// In choosing the port names, please realize that gmidi.dll/midi.drv,
// nmshardware.dll, and gstudio.exe need to be rebuilt. Also the following
// rules need to be following choosing names:
//
//  1 The last character in the previous port is the only character to change.
//     ie 'Port 1' 'Port 2' 'Port 3' etc.
//  2. Or the m_DefaultPort must have a numerical value of 1 somewhere in 
//     the string and all characters to left of the 1 position can not change.
//     ie 'Port 1 - Control port' 'Port 2' 'Port 3' etc.
//  3. AND characters strings are limited to 32 chars
//


#define NEM_STR_MIDI_PORT_OUT_1 "GigaOut: Port 1 / Control"
#define NEM_STR_MIDI_PORT_OUT_2 "GigaOut: Port 2"
#define NEM_STR_MIDI_PORT_OUT_3 "GigaOut: Port 3"
#define NEM_STR_MIDI_PORT_OUT_4 "GigaOut: Port 4"

/*
#define NEM_STR_MIDI_PORT_OUT_1 "NemeSys MidiOut: Port 1"
#define NEM_STR_MIDI_PORT_OUT_2 "NemeSys MidiOut: Port 2"
#define NEM_STR_MIDI_PORT_OUT_3 "NemeSys MidiOut: Port 3"
#define NEM_STR_MIDI_PORT_OUT_4 "NemeSys MidiOut: Port 4"
*/

#define MIDI_IN_STR_NONE "<none>"
#define MIDI_IN_VAL_NONE 0xFF

#endif // _HWREGDEFS_H


