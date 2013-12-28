//**************************************************************************//
//                          FILE INFORMATION                                //
//                                                                          //
//  (C) Copyright 1998-2003 TASCAM, Inc.                                    //
//      All rights reserved. Use of copyright				    //
//      notice is precautionary and does imply publication.                 //
//									    //
//  Created:........1/1/99						    //
//	Owner:..........Joe Bibbo (Nemesys Music Technology, INC.)    	    //
//                                                                          //
//	Copyright Notice:-                                                  //
//	All materials in this file are considered private and confidential. //
//									    //
//	Description: This file contains a working example of the GSIF.      //	
//									    //
//**************************************************************************//

#ifndef _GSIF_HEADER               // Prevent multiple definitions
#define _GSIF_HEADER

//===========================================================================//
//              GSIF    Global constants                                     //
//===========================================================================//

#define GSIF_VXDID              0x349D
#define STR_GSIFVXD             "VRTKRNLD"
#define GSIF_MAXSZ              64
#define GSIF_MAXCHANNELS        64

#define GSIF_MAXCARDS           8

#define GSIF_VER                0x10070

#define GSIF_DEF_OUTBUFFERSIZE  0x2000
#define GSIF_DEF_INBUFFERSIZE   0x2000

//
// ======================= GSIF Commands ===============================
// Hw Driver to GigaX communication
//
#define GSIF_IOCTL_REGISTER                0x2000     //
#define GSIF_IOCTL_GETREGISTER             0x2001     //
#define GSIF_IOCTL_REMOVEADDITIONALDS      0x2002     // Only use this if you
                                                      // have additional DS HALs
//
// The HW driver can use this, along with the GSIF_VERSION structure to
// determine what version of GSIF is currently running on the system.
//
// .dwGSVERSION                      Notes
//   0 or >= 0x10052               Original version of GSIF
//   0x10070 or greater            Added support for multiple cards, waveOut
//                                 driver opens, variable output buffer sizes.
//   0x20000 or greater            Added support for kernel mode midi and
//                                 input channels.
//
//
#define GSIF_IOCTL_GETVERSION              0x2003
//
// v1.7 or greater driver should use this extended commands when regis
//
#define GSIF_IOCTL_REGISTEREX              0x2004     //
#define GSIF_IOCTL_RESET                   0x2005     //

//
// ======================= GSIF APIs Commands ===========================
// GigaX to Hw Driver communication
//
#define GSIF_API_GETVERSION                0x1000
#define GSIF_API_GETHWCAPS                 0x1001
#define GSIF_API_OPENAUDIO                 0x1002
#define GSIF_API_CLOSEAUDIO                0x1003
#define GSIF_API_STARTAUDIO                0x1004
#define GSIF_API_STOPAUDIO                 0x1005
#define GSIF_API_RESETAUDIO                0x1006
#define GSIF_API_GETPOSITION               0x1007
#define GSIF_API_FLUSHBUFFER               0x1008   // Only sent if HC_FLUSHBUFFER bit is set
#define GSIF_API_ENUMERATESYSDRIVERS       0x1009   // Only sent if HC_FLUSHBUFFER bit is set
#define GSIF_API_GETCURRENTADAPTER         0x1010   // Only sent if HC_FLUSHBUFFER bit is set
#define GSIF_API_HOOKTIMER                 0x1011   // Only sent if HC_FLUSHBUFFER bit is set
#define GSIF_API_UNHOOKTIMER               0x1012   // Only sent if HC_FLUSHBUFFER bit is set

// v2.0 or greater specific commands
#define GSIF_API_V2_GETMIDICAPS            0x1013   //
#define GSIF_API_V2_OPENMIDIPORTS          0x1014
#define GSIF_API_V2_CLOSEMIDIPORTS         0x1015
#define GSIF_API_V2_GETPORTNAME            0x1016
#define GSIF_API_V2_GETCHANNELNAMES        0x1017

//
// ======================= GSIF Error codes ============================
//
#define GSIF_NOERROR                       0x0
#define GSIF_INVALIDPARAMETER              0x1
#define GSIF_OPENFAILED                    0x2
#define GSIF_STARTFAILED                   0x3
#define GSIF_RESETFAILED                   0x4
#define GSIF_STOPFAILED                    0x5
#define GSIF_CLOSEFAILED                   0x6
#define GSIF_SYSERROR                      0x7

#define GSIF_MIDIBUFFEROVERRUN             0x8

//
// Sample Rate flags. (kHz)
//
#define FS_32000                       0x01
#define FS_44100                       0x02
#define FS_48000                       0x04
#define FS_88200                       0x08
#define FS_96000                       0x10
// v2.0 or greater only
#define FS_176400                      0x20
#define FS_192000                      0x40
#define FS_PULLDOWN                    0x80
#define FS_PULLUP                      0x100

//
// Sample width flags. (bits per sample)
//
#define SW_16                          0x0100
#define SW_20                          0x0200
#define SW_24                          0x0400
#define SW_32                          0x0800

//
// Hardware Communication flags. These flags determine how the
// driver wants GigaSampler to talk to it.
//
#define HC_MULTICLIENT                 0x01     // Driver supports mulitclient
#define HC_SUPPLYBUFFERS               0x02     // On open, GS should supply buffers.
#define HC_CHANNELSINTERLEAVED         0x04     // left/right channels interleaved in
                                               // the same output buffer.
#define HC_FLUSHBUFFER                 0x08     // This should be use only if driver double buffers
                                                // data, and can handle a flush from VRTKRNLD.
#define HC_OPENWAVEOUTDRV              0x10     // This flag should be set if the GSIF driver wants
                                                // us to open the MME driver first.
#define HC_MULTICHANNELINTERLEAVED     0x20     // This flag should be set if we are using
                                                // multichannel intereleave.

//
// Open Audio flags
//
#define OA_CHANNELENABLE               0x01     // Channel enabled.


//
// Input/Output channel capability flags.
//
#define OC_DIGITAL                     0x01     // Channel is digital
#define OC_DIGITAL_ADAT                0x02     // Channel is digital - ADAT
#define OC_DIGITAL_SPDIF               0x04     // Channel is digital - SPDIF
#define OC_VIRTUAL                     0x08     // Channel is virtual
#define OC_HASFRIENDLY_NAME            0x10     // Has Friendly name, uses GSIF_API_V2_GETCHANNELNAMES


//
// Reset GSIF flags
//
#define RS_NEWSAMPLERATE               0x01     // New sample rate
#define RS_NEWBUFFERSIZE               0x02     // New buffer size
#define RS_NEWBITDEPTH                 0x04     // New bit depth
#define RS_LEAVEHARDWAREOFF            0x08     // After reset leave hardware off 



//===========================================================================//
//              GSIF    Global structures                                    //
//===========================================================================//
typedef struct tGSIFREGISTER
{
        char            szVxDName[GSIF_MAXSZ]; // Name of Hardware's vxd.
        DWORD           dwCallback;            // Callback routine
                                               // When you are unregistering this
                                               // should be set to 0.
        BOOL            bSuccess;              // TRUE if everything is good.
        char            szDSName[GSIF_MAXSZ];  // Set to null if you don't have
                                               // a DirectSound HAL.
} GSIF_REGISTER, *PGSIF_REGISTER;

typedef struct tGSIFREGISTEREX
{
        char            szVxDName[GSIF_MAXSZ]; // Name of Hardware's vxd.
        DWORD           dwCallback;            // Callback routine
                                               // When you are unregistering this
                                               // should be set to 0.
        BOOL            bSuccess;              // TRUE if everything is good.
        char            szDSName[GSIF_MAXSZ];  // Set to null if you don't have
                                               // a DirectSound HAL.
        //
        // Additions to v1.7 GSIF. Allows multiple cards.
        //
        DWORD           dwCardId;              // Card ID. Used in all EX commands
                                               // use.

} GSIF_REGISTEREX, *PGSIF_REGISTEREX;

typedef struct tGSIFVERSION
{
        DWORD           dwMajor;
        DWORD           dwMinor;
        DWORD           dwRevision;
        DWORD           dwCardIdentifier;
        DWORD           dwGSVERSION;
} GSIF_VERSION, *PGSIF_VERSION;

typedef struct tagGSIFHWCAPS
{
        char            szProductName[GSIF_MAXSZ];
        DWORD           dwNumOfOuts;
        DWORD           dwNumOfIns;
        DWORD           dwSampleRates;          // see FS_xxxx flags
        DWORD           dwSampleWidths;         // see SW_xxxx flags
        DWORD           dwHWCommFlags;          // see HC_xxxx flags
} GSIF_HWCAPS, *PGSIF_HWCAPS;

typedef struct tagGSIFHWCAPSEX
{
        char            szProductName[GSIF_MAXSZ];
        DWORD           dwNumOfOuts;
        DWORD           dwNumOfIns;
        DWORD           dwSampleRates;          // see FS_xxxx flags
        DWORD           dwSampleWidths;         // see SW_xxxx flags
        DWORD           dwHWCommFlags;          // see HC_xxxx flags

        //
        // v1.7 additions.
        //
        DWORD           dwMMEDriverId;          // Only use if HC_OPENWAVEOUTDRV is set
        DWORD           dwOutCaps[GSIF_MAXCHANNELS]; // see OC_xxxx flags
        DWORD           dwInCaps[GSIF_MAXCHANNELS];  // see OC_xxxx flags
        //
        // Requests a different buffersize than our default 0x2000. If you
        // leave this field 0 then we will assume default, otherwsize select
        // a 2^n (in bytes) which is at least 0x1000 or greater.
        //
        DWORD           dwReqOutputBufSize;     // The 2^n size of output buffer.
        DWORD           dwReqInputBufSize;      // The 2^n size of input buffer.
        //
        // Multichannel interleaved data.
        //
        DWORD           dwSampleSeparation;     // Only used if HC_MULTICHANNELINTERLEAVED
                                                // is set. Separation in bytes.
        DWORD           dwReserved[64];         // Reserved for future expansion.

} GSIF_HWCAPSEX, *PGSIF_HWCAPSEX;

typedef struct tagGSIFOPENAUDIO
{
        DWORD           dwOutFlags[GSIF_MAXCHANNELS];    // see OA_xxxx flags
        PULONG          dwOutBuffers[GSIF_MAXCHANNELS];  // Hw output buffers.
        DWORD           dwInFlags[GSIF_MAXCHANNELS];     // Not used.
        PULONG          dwInBuffers[GSIF_MAXCHANNELS];   // Not used.
        DWORD           dwBufferSize;                    // must be a 2^n size.
        DWORD           dwFormat;                        // uses FS_xxxx and
                                                         //      SW_xxxx flags.
} GSIF_OPENAUDIO, *PGSIF_OPENAUDIO;

typedef struct tagGSIFRESET
{
        DWORD           dwCardId;                        // CardId returned in register
        DWORD           dwControl;                       // see RS_xxxx flags
        DWORD           dwSamplingRate;                  // New Sample Rate.
        DWORD           dwReqOutputBufSize;              // New Buffer Size.
        DWORD           dwBitDepth;                      // New Bit Depth.
        DWORD           dwReserved1;                     // Reserved for future
        DWORD           dwReserved2;                     // Reserved for future
} GSIF_RESET, *PGSIF_RESET;

typedef struct tagGSIFCHANNELNAMES
{
        char            szIns[GSIF_MAXCHANNELS][GSIF_MAXSZ];
        char            szOuts[GSIF_MAXCHANNELS][GSIF_MAXSZ];
} GSIF_CHANNELNAMES, *PGSIF_CHANNELNAMES;

//===========================================================================//
//              GSIF    Function prototypes                                  //
//===========================================================================//

//
// Callback protocol of the vrtkrnld
//
typedef DWORD (CALLBACK *GSIF_IOCTL)  ();

//
// Callback protocol of the Audio hardware driver.
//
#ifndef REMOVE_DDKDEFINITIONS
typedef DWORD (CALLBACK *GSIF_CALLBACK)  (DWORD dwService, DWORD dwDDB, DWORD hDevice, PDIOCPARAMETERS ptr);
typedef DWORD (CALLBACK *GSIF_CALLBACKEX)(DWORD dwService, DWORD dwDDB, DWORD hDevice, PDIOCPARAMETERS ptr, DWORD dwCardId);
#endif

#endif

