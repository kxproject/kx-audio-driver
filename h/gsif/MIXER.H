//***************************************************************************
// Driver:
//              gsiddrv.sys
//
// File:
//              mixer.h
//
// Description:
//              Defines all the neccessary device extensions, structures and
//              prototype for the NT-style driver.
//
//***************************************************************************

#ifndef __MIXER_H_
#define __MIXER_H_


#define BIT_TEST(value,bit)         (value & (bit))
#define BIT_SET(value,bit)          value |=  (bit)
#define BIT_CLR(value,bit)          value &= (~(bit))


#define MAX_OUTPUTS                64
#define MAX_INPUTS                 32
#define GHW    pG->hw

//
// Hardware type flags
//
#define HWTYPE_DSOUNDIF          0x0          // 2 channel vanilla flavor
#define HWTYPE_DSOUNDMULTI       0x1          // Multi-channel DS i/f
#define HWTYPE_CUSTOMMULTI       0x2          // Multi-channel custom i/f

//
// Hardware Control Flags
#define HW_MIDIPORTSOPEN         0x1          // Midi ports are currently open
#define HW_TIMESTAMPMIDI         0x2          // If set then Giga will time stamp data
#define HW_TIMESTAMP_INSAMPLES   0x4          // If set then time stamp in samples off hardware's audio, else clock cycles

//
// This structure represents all the possible types of hardware
// adapter we support.
//
#define HWADAPTERTYPE_DSOUND_VXD        1
#define HWADAPTERTYPE_DSOUND_NT         2
#define HWADAPTERTYPE_GSIF_VXD          3
#define HWADAPTERTYPE_GSIF_NT           4

//
// Used by the .dwControl field to determine characteristics about
// the channel strip.
//
#define  CHANCTL_DISABLE         0x1            //  Channel should not be processed.
#define  CHANCTL_MONO            0x2            //
#define  CHANCTL_SENDTOGROUP     0x4            //  This is a grouped channel strip
#define  CHANCTL_LOCKED          0x8            //  A restriced plugin owns this strip
#define  CHANCTL_HWOUTPUT	     0x10			//  HWCHANNEL only
#define  CHANCTL_HWINPUT		 0x20           //  HWCHANNEL only
#define  CHANCTL_BYPASS_EQ       0x40           // EQ bypass
#define  CHANCTL_BYPASS_DYN      0x80           // Dynamic bypass
#define  CHANCTL_BYPASS_HPF      0x100          // HPF bypass

//===========================================================================//
//             Structures                                                    //
//===========================================================================//
                                  //
                                  // ==== MIDI Trace Buffer ============
                                  //
#define MIDITRACEBUFFER_SIZE      1024
#define TRACE_CTRL_IGNORESYSTEM   0x1

typedef struct tagMIDITRACEBUFFER
{
        DWORD       dwControl;
        DWORD       dwSizeMask;            // Circle buffer mask (must be 2^n)
        DWORD       dwPutIndex;
        DWORD       dwGetIndex;
        GSIF_MIDIEVENT cirBuf[1];          // Circular buffer of Midi events
} MIDITRACEBUFFER, * PMIDITRACEBUFFER;
                                  //
                                  // ==== Hw Channels =================
                                  //

typedef struct tagHWChannel
{
        DWORD   dwControl;                   // see CHANCTL_xxx
        BYTE    byMonitor;
        PULONG   dwHwOutBufs;                             // Hardware's DMA buffer
} HWCHANNEL, * PHWCHANNEL;

                                  //
                                  // ==== HW Output configuration ====
                                  //
typedef struct _tagHardware FAR * PHARDWARE;
typedef struct _tagHardware
{
   DWORD  dwFsHardware;
   DWORD  dwFsEngine;
   DWORD  dwChansPerOutput;       // Stereo = 2, Mono = 1
   DWORD  dwBitsPerSample;
   DWORD  dwHwActiveOutputs;      // 0 to MAX_OUT_CHANNELS-1. This represents
                                  // the number of active outputs
   DWORD  dwHwActiveInputs;       // 0 to MAX_OUT_CHANNELS-1. This represents
                                  // the number of active inputs
   DWORD  dwHwOutMask;            // Size-1 (bytes) of each output buffer
                                  // NOTE: Buffer sizes must be 2^n.
   //
   // This is a running output index, which doesn't actually get use
   // by the engine. The 4 MSB will be used for security and should not
   // be used by to represent the output index.
   //
   DWORD  dwHwOutWriteIndex;      // Write index of the engine.
   
   HWCHANNEL arOutputs[MAX_OUTPUTS];
   HWCHANNEL arInputs[MAX_OUTPUTS];
    
   BYTE      byGSIFMidiOutRouting[GSIF_MAXMIDIPORTS];
   BYTE      byGSIFMidiInRouting[GSIF_MAXMIDIPORTS];

   DWORD     dwControl;
   PVOID            pfnMIDIIsrHw;         // Current Hardware Isr
   PVOID            pHwDriverContext;     //

   _int64    ddRefTime;

} HARDWARE;

typedef struct tagHWAdapter  FAR * LPHWADAPTER;
typedef struct tagHWAdapter
{
    DWORD dwType;                     // See HWADAPTERTYPE_xxxx

    DWORD dwGSIFver;

    //
    // Use by DirectSound VXD drivers.
    //
    PVOID pDriverDesc;                //
    PVOID pHAL;
    PVOID pBufferIF;

    //
    // Use by DirectSound NT drivers.
    //

    //
    // Use by GSIF VXD Driver.
    //
    PVOID pGSIFDriverVxD;             // GSIFCARDS structure.

    //
    // Use by GSIF NT drivers.
    //
    PVOID pGSIFDriverNt;              // GSIF_GETINTERFACE structure.


    //
    // Hw output/input configuration.
    //
    PHARDWARE        pOuts;
    PVOID            pSynth;

} HWADAPTER, * PHWADAPTER;

typedef struct tagEWGlobal_Objs FAR * PEWGLOBALS;
typedef struct tagEWGlobal_Objs
{
     DWORD       dwSize;             // Size of WaveStream's Global heap.
     DWORD       dwSize16;           // Size of just the 16-bit shared area.

     PEWGLOBALS  pThis_u;            // User mode pointer (MSG32 thread) to this object.
     PEWGLOBALS  pThis_k;            // Kernel mode pointer to this object.

     HARDWARE    hw;         //
} EWGLOBALS;


#define IS_OUT_ENABLED(a, i)  !(((PHARDWARE)a)->arOutputs[i].dwControl & CHANCTL_DISABLE)
#define SET_OUT_ENABLED(a, i) ((PHARDWARE)a)->arOutputs[i].dwControl &= (~CHANCTL_DISABLE)
#define CLR_OUT_ENABLED(a, i) ((PHARDWARE)a)->arOutputs[i].dwControl |= CHANCTL_DISABLE

#define IS_IN_ENABLED(a, i)  !(((PHARDWARE)a)->arInputs[i].dwControl & CHANCTL_DISABLE)
#define SET_IN_ENABLED(a, i) ((PHARDWARE)a)->arInputs[i].dwControl &= (~CHANCTL_DISABLE)
#define CLR_IN_ENABLED(a, i) ((PHARDWARE)a)->arInputs[i].dwControl |= CHANCTL_DISABLE


//
// EWTHUNK.sys to VRTKRNLD, required if using Giga technology.
//
typedef DWORD (__cdecl   *PFNSCHEDULESYNTH)(DWORD, DWORD);
typedef DWORD (__stdcall *PFNSTARTSCHEDULER)(DWORD);
typedef DWORD (__stdcall *PFNSTOPSCHEDULER)();
typedef DWORD (__stdcall *PFNPROCESSVXDREQUEST)(DWORD,PVOID,PVOID,DWORD);

typedef void  (__stdcall *PFNRESETENGINE)();
#endif // __MIXER_H_

