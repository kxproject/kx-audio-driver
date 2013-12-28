//**@@@*@@@****************************************************
//
// Microsoft Windows
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//**@@@*@@@****************************************************

//
// FileName:    util.h
//
// Abstract:    
//      This is the header file for general helper functions
//

#pragma once

//
// Get the size for a WAVEFORMATEX-based structure.  It properly handles extended
// structures (such as WAVEFORMATEXTENSIBLE), and WAVE_FORMAT_PCM structures with bogus sizes.
//
DWORD GetWfxSize(const WAVEFORMATEX* pwfxSrc);


// Debug Trace Macro's

#define TRACE_LOWEST (6)
#define TRACE_LOW (5)
#define TRACE_NORMAL (4)
#define TRACE_HIGH (3)
#define TRACE_HIGHEST (2)
#define TRACE_ERROR (1)
#define TRACE_MUTED (0)
#define TRACE_ALWAYS (-1)

#if DBG
#define TRACE_ENTER() DebugPrintf(TRACE_LOWEST, TEXT("%s Enter"), __FUNCTION__ "()")
#define TRACE_LEAVE()  DebugPrintf(TRACE_LOWEST, TEXT("%s Leave"), __FUNCTION__ "()")
#define TRACE_LEAVE_HRESULT(hr) DebugPrintf(TRACE_LOWEST, TEXT("%s Leave, returning %#08x"), __FUNCTION__,hr)
void DebugPrintf(LONG lDebugLevel, LPCTSTR pszFormat, ... );
//The higher the numer, the more the debug output
// Range is from 0 to 6.
extern LONG g_lDebugLevel;
#else
#define TRACE_ENTER() 
#define TRACE_LEAVE() 
#define TRACE_LEAVE_HRESULT(hr)
#define DebugPrintf (void)
#endif

// test tone generation
BOOL 
GeneratePCMTestTone
( 
    LPVOID          pDataBuffer, 
    DWORD           cbDataBuffer,
    UINT            nSamplesPerSec,
    UINT            nChannels,
    WORD            wBitsPerSample,
    double          dFreq,
    double          dAmpFactor=1.0
);

