//**************************************************************************//
//                          FILE INFORMATION                                //
//                                                                          //
//  (C) Copyright 1998-2003 TASCAM, Inc.                                    //
//      All rights reserved. Use of copyright				    //
//      notice is precautionary and does imply publication.                 //
//									    //
//  Created:........1/1/2003                                                //
//	Owner:..........Joe Bibbo (Nemesys Music Technology, INC.)    	    //
//                                                                          //
//	Copyright Notice:-                                                  //
//	All materials in this file are considered private and confidential. //
//									    //
//      Description: This file contains the definitions for v2.x GSIF       //
//									    //
//**************************************************************************//

#ifndef _GSIF2_HEADER               // Prevent multiple definitions
#define _GSIF2_HEADER

#include "gsifnt.h"

//===========================================================================//
//              GSIF    Global constants                                     //
//===========================================================================//

#define GSIF2_VER                       0x20001
#define GSIF2_VER_MAJOR                 0x2
#define GSIF2_VER_MINOR                 0x1

#define GSIF_MAXMIDIPORTS               8

//
// Stream types, used in the the GSIF_API_STARTAUDIO/STOPAUDIO
//
#define STREAMTYPE_AUDIO                0x1
#define STREAMTYPE_MIDI                 0x2

//
// Open Midi  flags
//
#define OM_PORTENABLED                 0x01     // Channel enabled.

//
// Midi Output Port properties
//
#define OP_TIMESTAMP                   0x01     // Hardware handles time stamp
#define OP_UNIQUEPORTNAME              0x02     //

//
// Midi Input Port properties
//
#define IP_TIMESTAMP                   0x01     // Hardware handles time stamp
#define IP_UNIQUEPORTNAME              0x02     //

typedef enum
{
   eMidiEvent=0,
   eBufferedMidi=1
} MIDIEVENTTYPE;

//===========================================================================//
//              GSIF  MIDI MACROS                                            //
//===========================================================================//
#define IS_MIDIEVENT(a)     ((a->byEventType&0xF) == eMidiEvent)
#define IS_BUFFEREDMIDI(a)  ((a->byEventType&0xF) == eBufferedMidi)

#define MIDI_PORT(a)         (a->byEventType >> 4)
#define MIDI_TYPE(a)         (a->byEventType & 0xF)
#define MAKE_MIDIEVENT(a,b)  (a<<4) | (b)

//===========================================================================//
//              GSIF    Global structures                                    //
//===========================================================================//

typedef struct GSIF_GETPORTNAME
{
        DWORD           nPort;
        BOOL            bInPort;
        char            szPortName[GSIF_MAXSZ]; //
} GSIF_GETPORTNAME, *PGSIF_GETPORTNAME;

typedef struct GSIF_MIDICAPS
{
        DWORD           dwNumIns;
        DWORD           dwNumOuts;
        DWORD           dwInProperties;
        DWORD           dwOutProperties;

        char            szPortInName[GSIF_MAXSZ];  // This base name you want displayed.
        char            szPortOutName[GSIF_MAXSZ]; // This base name you want displayed.

} GSIF_MIDICAPS, *PGSIF_MIDICAPS;

typedef struct tagGSIFOPENMIDI
{
        DWORD           dwInFlags[GSIF_MAXMIDIPORTS];     // see OM_xxxx flags
        DWORD           dwOutFlags[GSIF_MAXMIDIPORTS];    // see OM_xxxx flags

        //
        // Callback resides in GSIF client driver, and associated driver
        // extension pointer.
        //
        PFNMIDICALLBACK pfnMIDIIsrGS;
        PVOID           pGsDriverContext;

        //
        // Callback resides in Hw GSIF driver, and associated driver
        // extension pointer.
        //
        PFNMIDICALLBACK pfnMIDIIsrHw;
        PVOID           pHwDriverContext;

        //
        // Raw midi data interface for drivers that need to pass us data rather
        // than complete midi events.
		PFNMIDIUART     pfnMIDIUartGS;
		PVOID           pReserved[3];
        
} GSIF_OPENMIDI, *PGSIF_OPENMIDI;

typedef struct GSIF_MIDIEVENT
{
        BYTE            byEventType;      // MS Nibble - Port # : LS Nibble MIDIEVENTTYPE
        BYTE            byData1;          // Midi Data 1
        BYTE            byData2;          // Midi Data 2
        BYTE            byData3;          // Midi Data 3
        _int64          ddTimeStamp;      // 64-bit time stamp (see docs for use)
} GSIF_MIDIEVENT, *PGSIF_MIDIEVENT;

typedef struct GSIF_RUNNINGSTATUS
{
        DWORD           dwISRLevel;                  //  
        DWORD           dwMIDIISRLevel;                  //
        _int64          ddLastFrameProfile;          //  
        _int64          ddLastEngineProfile;         //
        DWORD           dwLastDMAPosition;           //  
        DWORD           dwCPUSpeed;                  // CPU speed in MHz
} GSIF_RUNNINGSTATUS, *PGSIF_RUNNINGSTATUS;

#endif

