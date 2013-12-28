/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//   SFMAN.H                                                               //
//                                                                         //
//   Description : Header file for acccesing the SoundFont Master Manager  //
//                                                                         //
//   Copyright (c) Creative Technology Ltd. / E-mu Systems, Inc.           //
//                 1996-1997. All rights reserved.                         //
//                                                                         //
//   Notes:        This header file is INDEPENDENT of the 'SFDEVMAN.H'     //
//                 header file. Any similarities in the prototypes or      //
//                 data values should be viewed as purely coincidental.    //
//                                                                         //
//   Revision:     1.01                                                    //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef __SFMAN_H
#define __SFMAN_H

///// W I N D O W S   I N C L U D E S  //////////////////////////////////////

#include <windows.h>
#include <winbase.h>

///// E R R O R  C O D E S  /////////////////////////////////////////////////

/* 
    The following is a comprehensive list of the API error message values
*/

/*
--------------------------------
      Error Category
--------------------------------
*/

#define     SFERR_BASE_INVALID             100L
#define     SFERR_BASE_CONTENTION          200L
#define     SFERR_BASE_API                 400L
#define     SFERR_BASE_EXIT                800L
#define     SFERR_BASE_UNDEFINE            900L

/*
--------------------------------
      General Error Messages
--------------------------------
*/

#define     SFERR_NOERR                      0L
#define     SFERR_UNDEFINE_ERROR            (SFERR_BASE_UNDEFINE+1) 

/*
--------------------------------
   API Error Messages
--------------------------------
*/
 
#define     SFERR_LOAD_DLL_FAIL            (SFERR_BASE_API+1)
#define     SFERR_GET_ENTRYPOINT_FAIL      (SFERR_BASE_API+2)
#define     SFERR_DEVICE_NOT_FOUND         (SFERR_BASE_API+3)
#define     SFERR_INTERFACE_NOT_SUPPORTED  (SFERR_BASE_API+4)
#define     SFERR_LAST_API                 (SFERR_BASE_API+5)

/*
--------------------------------
   Invalid Error Messages
--------------------------------
*/

#define     SFERR_SYNTH_INVALID            (SFERR_BASE_INVALID+5)
#define     SFERR_BANK_INDEX_INVALID       (SFERR_BASE_INVALID+6)
#define     SFERR_PATHNAME_INVALID         (SFERR_BASE_INVALID+7)
#define     SFERR_FORMAT_INVALID           (SFERR_BASE_INVALID+8)
#define     SFERR_PRESET_INDEX_INVALID     (SFERR_BASE_INVALID+9)
#define     SFERR_ACCESS_NOT_PERMITTED     (SFERR_BASE_INVALID+11)
#define     SFERR_VERSION_INVALID          (SFERR_BASE_INVALID+12)
#define     SFERR_DEVICE_INVALID           (SFERR_BASE_INVALID+13)
#define     SFERR_SUPPORT_INVALID          (SFERR_BASE_INVALID+14)
#define     SFERR_INVALID_SOUND_ROM        (SFERR_BASE_INVALID+15)
#define     SFERR_NOT_STATIC_MEMORY        (SFERR_BASE_INVALID+16)
#define     SFERR_LAST_INVALID             (SFERR_BASE_INVALID+17)

/*
--------------------------------
   Contention Error Messages
--------------------------------
*/

#define     SFERR_DLL_BUSY                 (SFERR_BASE_CONTENTION+1)
#define     SFERR_DEVICE_BUSY              (SFERR_BASE_CONTENTION+2)
#define     SFERR_RESOURCE_INSUFFICIENT    (SFERR_BASE_CONTENTION+3)
#define     SFERR_SYSMEM_INSUFFICIENT      (SFERR_BASE_CONTENTION+4)
#define     SFERR_SOUNDMEM_INSUFFICIENT    (SFERR_BASE_CONTENTION+5)
#define     SFERR_LAST_CONTENTION          (SFERR_BASE_CONTENTION+6)



///////////////////////////////////////////////////////////////////////////////////
//  S U P P O R T E D   I N T E R F A C E     I D                              //
///////////////////////////////////////////////////////////////////////////////////

// Currently supported Interface ID

enum INTERFACEID {

    ID_SFMANL100API = 0x00010000,
    ID_SFMANL101API = 0x00010001,

} ;


///////////////////////////////////////////////////////////////////////////////////
//  D A T A   S T R U C T U R E S   A N D   E N U M E R A T I O N   T A B L E S  //
///////////////////////////////////////////////////////////////////////////////////

#pragma pack(2)

/////  E N U M  /////

enum SFBANKSTYLE {
    
    /* Style 0, and the many names that go along with it... */
    SFBANK_AWE32,
    SFBANK_CC0_ONLY = SFBANK_AWE32,
    SFBANK_GS       = SFBANK_AWE32,
    SFBANK_DEFAULT  = SFBANK_AWE32,

    /* Style 1, and the many names that go along with it... */
    SFBANK_MMA,
    SFBANK_CC0_CC32 = SFBANK_MMA,
};

typedef enum SFBANKSTYLE*    PSFBANKSTYLE ;

enum SFFLAG_OPERATION {
    SFFLAG_OPER_FILE,
    SFFLAG_OPER_MEMORY
};

enum SBANK {
    GENERAL_MIDI = 0,
    ROLAND_GS,
    MT_32,
    USER_CUSTOM_BANK
};

enum TYPEINDEX {
    REVERB_CHORUS = 0,
    QSOUND
};

            
enum SUBINDEX {
    REVERB = 0,
    CHORUS,
    BASS,
    TREBLE
};
            
enum VARIINDEX {
    ROOM_1 = 0,
    ROOM_2,
    ROOM_3,
    HALL_1,
    HALL_2,
    PLATE,
    DELAY,
    PANNING_DELAY,

    CHORUS_1 = 0,
    CHORUS_2,
    CHORUS_3,
    CHORUS_4,
    FEEDBACK_DELAY,
    FLANGER,
    SHORT_DELAY,
    SHORT_DELAY_FB
};

typedef struct {
    BYTE m_BankMSB;
    BYTE m_BankLSB;
} SFMIDIBankByResolution;

typedef struct {
    BYTE m_BankCC0;
    BYTE m_BankCC32;
} SFMIDIBankByValue;

typedef union {
    SFMIDIBankByResolution  st_BankRes;
    SFMIDIBankByValue       st_BankVal;
    WORD                    m_BankNum;
} SFMIDIBANK, *PSFMIDIBANK;


#define	SFMANCAP_VIENNA_EXTENSIONS_CAP     	0x00000200
#define SFMANCAP_DYNAMIC_MEM_CAP            	0x80000000
#define SFMANCAP_SOFTWARE_SYNTH_CAP         	0x40000000
#define SFMANCAP_NONMMSYSTEM_COMPATIBLE_CAP 	0x20000000
#define SFMANCAP_NONWAVEFORM_SUPPORT_CAP    	0x10000000

   
///// S T R U C T /////

/*
This CSFMIDILocation is usually used when required to MIDI bank/preset 
locaions to the DLL. The PSFPARAMOBJECT type define a pointer to the
parameter object.
*/
typedef struct { 
    WORD            m_BankIndex;    
    WORD            m_PresetIndex;    
} CSFMIDILocation, SFMIDILOCATION, *PSFMIDILOCATION ;

/*
The CSFBufferObject is used to specify a buffer area to the DLL. This buffer
area will usually be filled with some strings or values. In some occassions,
the buffer itself, actually carries information to the DLL, like filename
etc. The PSFBUFFEROBJECT type define a pointer to the buffer object.
*/

typedef struct {

    DWORD   m_Size;         /* The size of the buffer attached */
    LPSTR   m_Buffer;       /* The pointer to the buffer itself */
    DWORD   m_SizeUsed;     /* The number of buffer characters used by DLL */
    WORD    m_Flag;         /* Scratch Variable */

} CSFBufferObject, *PSFBUFFEROBJECT;

/*
The CSFCapsObject is used to for querying device capabilities. This buffer
area will be filled with the capabilities of a particular SoundFont Device. 
The PSFCAPSOBJECT type define a pointer to the device object.
*/

typedef struct {
    WORD    m_SizeOf;           /* sizeof this structure                    */
    DWORD   m_RouterID;         /* SF Device Router ID                      */
    DWORD   m_DevCaps;          /* Capabilities                             */ 
    DWORD   m_MoreDevCaps;      /* More capabilities                        */
    DWORD   m_RomID;            /* ID for a Sound ROM used in a device      */
    DWORD   m_RomVer;           /* Version for a Sound ROM used in a device */
    CHAR    m_DevName[40];      /* MMSystem name for MIDI Output Device     */
    CHAR    m_DevMgrName[20];   /* File Name of the Device Manager DLL.     */
    CHAR    m_DevMgrEntry[20];  /* Entry point inside the Device Manager DLL*/
} CSFCapsObject , *PSFCAPSOBJECT ;

/*
This CParamObject is usually used when required to pass several indices to
the DLL. This happens when configuring an effects type, synth emulation or
during query session. The PPARAMOBJECT type define a pointer to the
parameter object.
*/
typedef struct {

    enum SBANK      m_SBankIndex;   /* Follows strictly to the available   */
                                    /* Synth Emulation available           */

    SFMIDIBANK      m_UBankIndex;   /* Support either 0-127 bank only      */
                                    /*  ( use m_UBankIndex.wBankNum )      */
                                    /*               OR                    */
                                    /*         128x128 bank                */
                                    /*  (use m_UBankIndex.st_BankVal       */
                                    /*    OR m_UBankIndex.st_BankRes )     */


    WORD            m_PresetIndex;  /* Preset Index from 0 to 127 */
   
    enum TYPEINDEX  m_TypeIndex;    /* Follows strictly to the available Effects Type available */
    WORD            m_SubIndex;     /* A sub index of the Type Index */
    WORD            m_VariIndex[6]; /* 6 sets of variations! Extendibility thought */

} CParamObject, *PPARAMOBJECT ;


#pragma pack() 

/////  D E F I N E S  ///////////////////////////////////////////////////////

#ifndef DLLAPI
#define DLLAPI _declspec (dllimport)
#endif

#define SF_MASTER_MANAGER_FILENAME "SFMAN32.DLL"
#define SF_FUNCTION_TABLE_NAME "SFManager"

#define SFMAN_GET_ROUTING_INDEX(dwID, by1, by2, by3, by4)             \
  {                                                                   \
  (by1) = (BYTE)(((dwID)    ) & 0x7F);                                \
  (by2) = (BYTE)(((dwID)>> 8) & 0x7F);                                \
  (by3) = (BYTE)(((dwID)>>16) & 0x7F);                                \
  (by4) = (BYTE)(((dwID)>>24) & 0x7F);                                \
  }

#define SFMAN_PERCUSSIVE_BANK 0x8000

/////  S F M A N   F u n c t i o n   T a b l e   ////////////////////////////

#if defined(_MSC_VER) || defined (__WATCOMC__)
	#ifndef CDECL
        #define CDECL  _cdecl
	#endif
#elif defined(__BORLANDC__)
	#ifndef CDECL 
		#define CDECL  __cdecl
	#endif
#else
    #ifndef CDECL
        #define CDECL  _cdecl
    #endif
#endif

/* --------------------------------
      Function Category
-------------------------------- */
#define SFDEV_BASE_VIENNA			2000

/* --------------------------------
      Vienna Messages
-------------------------------- */
#define SFDEV_VIENNA_START				(SFDEV_BASE_VIENNA+1)
#define SFDEV_VIENNA_END				(SFDEV_BASE_VIENNA+2)
#define SFDEV_VIENNA_LOAD_SAMPLE		(SFDEV_BASE_VIENNA+3)
#define SFDEV_VIENNA_FREE_SAMPLE		(SFDEV_BASE_VIENNA+4)
#define SFDEV_VIENNA_PLAY_SAMPLE		(SFDEV_BASE_VIENNA+5)
#define SFDEV_VIENNA_LOAD_PRESET		(SFDEV_BASE_VIENNA+6)
#define SFDEV_VIENNA_FREE_PRESET		(SFDEV_BASE_VIENNA+7)
#define SFDEV_VIENNA_NOTE_ON			(SFDEV_BASE_VIENNA+9)
#define SFDEV_VIENNA_NOTE_OFF			(SFDEV_BASE_VIENNA+10)
#define SFDEV_VIENNA_CONTROLLER			(SFDEV_BASE_VIENNA+11)
#define SFDEV_VIENNA_PITCHBEND			(SFDEV_BASE_VIENNA+12)
#define SFDEV_VIENNA_PRESSURE			(SFDEV_BASE_VIENNA+13)
#define SFDEV_VIENNA_SYSEX				(SFDEV_BASE_VIENNA+14)
#define SFDEV_VIENNA_GET_DRAM_SIZE		(SFDEV_BASE_VIENNA+16)

/* --------------------------------
      Enumeration Tables
-------------------------------- */

enum SFDEVMANCAPSEX {
		SFDEVMANCAP_VIENNA				= 0x00000200
	};

/* --------------------------------
      Data Structures
-------------------------------- */

#pragma pack(2)

typedef struct {

    DWORD   dwcbSize;           
    DWORD   dwFlags;            
    DWORD_PTR   dwSampleHandle;    
    DWORD   dwSampleOffset;    
    DWORD   dwReserved;
    WORD    wFormatTag;
    DWORD   dwSampleSize;       
    DWORD   dwSamplesPerSec;   
    WORD    wBitsPerSample;
    WORD    wChannels;
    short   iSample[1];

} CViSmplObject, *LPVISMPLOBJECT;

typedef struct {

    DWORD   dwFlags;       
    DWORD   dwReserved;
    DWORD   dwSampleHandle;
    LONG    lTuning;
    DWORD   dwStart;      
    DWORD   dwEnd;           
    DWORD   dwStartLoop;
    DWORD   dwEndLoop;

} CViPlayObject, *LPVIPLAYOBJECT;

#pragma pack()


typedef WORD SFDEVINDEX;

typedef struct SFMANAGERTAG {

    LRESULT (CDECL *SF_QueryInterface) ( INTERFACEID, PDWORD_PTR );

} SFMANAGER, *PSFMANAGER ;

typedef struct SFMANL101TAG {
    // Global functions
    LRESULT (CDECL *SF_GetNumDevs) ( PWORD ) ;

    // Device functions which do not require allocation (opening)
    LRESULT (CDECL *SF_GetDevCaps)    ( SFDEVINDEX, PSFCAPSOBJECT ) ;
    LRESULT (CDECL *SF_GetRouterID)   ( SFDEVINDEX, PDWORD ) ;
    LRESULT (CDECL *SF_IsDeviceFree)  ( SFDEVINDEX ) ;
    LRESULT (CDECL *SF_Open)          ( SFDEVINDEX ) ;
    LRESULT (CDECL *SF_Close)         ( SFDEVINDEX ) ;
 
    // Query functions
    LRESULT (CDECL *SF_IsMIDIBankUsed) (SFDEVINDEX, PWORD);
    LRESULT (CDECL *SF_QueryStaticSampleMemorySize) ( SFDEVINDEX, PDWORD, PDWORD ) ;

    // Synthesizer Emulation Functions
    LRESULT (CDECL *SF_GetAllSynthEmulations) ( SFDEVINDEX, PSFBUFFEROBJECT ) ;
    LRESULT (CDECL *SF_GetSynthEmulation)     ( SFDEVINDEX, PSFBUFFEROBJECT ) ;
    LRESULT (CDECL *SF_SelectSynthEmulation)  ( SFDEVINDEX, WORD ) ;

    // Sound Bank management functions
    LRESULT (CDECL *SF_LoadBank)  ( SFDEVINDEX, PSFMIDILOCATION, PSFBUFFEROBJECT ) ;
    LRESULT (CDECL *SF_GetLoadedBankDescriptor) ( SFDEVINDEX, PSFMIDILOCATION,
                                            PSFBUFFEROBJECT ) ;
    LRESULT (CDECL *SF_GetLoadedBankPathname) ( SFDEVINDEX, PSFMIDILOCATION,
                                          PSFBUFFEROBJECT ) ;
    LRESULT (CDECL *SF_ClearLoadedBank) ( SFDEVINDEX, PSFMIDILOCATION ) ;

    // Sound Preset management functions
    LRESULT (CDECL *SF_LoadPreset) ( SFDEVINDEX, PSFMIDILOCATION,
                               PSFMIDILOCATION, PSFBUFFEROBJECT ) ;
    LRESULT (CDECL *SF_GetLoadedPresetDescriptor) ( SFDEVINDEX, PSFMIDILOCATION,

                                              PSFBUFFEROBJECT ) ;
    LRESULT (CDECL *SF_ClearLoadedPreset) (SFDEVINDEX, PSFMIDILOCATION ) ;
 
    // Sound Waveform management functions
    LRESULT (CDECL *SF_LoadWaveform)  ( SFDEVINDEX, PSFMIDILOCATION, PSFBUFFEROBJECT ) ;
    LRESULT (CDECL *SF_ClearLoadedWaveform)  ( SFDEVINDEX, PSFMIDILOCATION ) ;

    // Device Specific Message
    LRESULT (CDECL *SF_DeviceSpecificMessage) ( SFDEVINDEX, UINT, LPARAM, LPARAM ) ;

} SFMANL101API, *PSFMANL101API;

typedef struct SFMANL100TAG {
    // Global functions
    LRESULT (CDECL *SF_GetNumDevs) ( PWORD ) ;

    // Device functions which do not require allocation (opening)
    LRESULT (CDECL *SF_GetDevCaps)    ( SFDEVINDEX, PSFCAPSOBJECT ) ;
    LRESULT (CDECL *SF_GetRouterID)   ( SFDEVINDEX, PDWORD ) ;
    LRESULT (CDECL *SF_IsDeviceFree)  ( SFDEVINDEX ) ;
    LRESULT (CDECL *SF_Open)          ( SFDEVINDEX ) ;
    LRESULT (CDECL *SF_Close)         ( SFDEVINDEX ) ;
 
    // Query functions
    LRESULT (CDECL *SF_IsMIDIBankUsed) (SFDEVINDEX, PWORD);
    LRESULT (CDECL *SF_QueryStaticSampleMemorySize) ( SFDEVINDEX, PDWORD, PDWORD ) ;

    // Synthesizer Emulation Functions
    LRESULT (CDECL *SF_GetAllSynthEmulations) ( SFDEVINDEX, PSFBUFFEROBJECT ) ;
    LRESULT (CDECL *SF_GetSynthEmulation)     ( SFDEVINDEX, PSFBUFFEROBJECT ) ;
    LRESULT (CDECL *SF_SelectSynthEmulation)  ( SFDEVINDEX, WORD ) ;

    // Sound Bank management functions
    LRESULT (CDECL *SF_LoadBank)  ( SFDEVINDEX, PSFMIDILOCATION, PSFBUFFEROBJECT ) ;
    LRESULT (CDECL *SF_GetLoadedBankDescriptor) ( SFDEVINDEX, PSFMIDILOCATION,
                                            PSFBUFFEROBJECT ) ;
//    LRESULT (CDECL *SF_GetLoadedBankPathname) ( SFDEVINDEX, PSFMIDILOCATION,
//                                          PSFBUFFEROBJECT ) ;
    LRESULT (CDECL *SF_ClearLoadedBank) ( SFDEVINDEX, PSFMIDILOCATION ) ;

    // Sound Preset management functions
    LRESULT (CDECL *SF_LoadPreset) ( SFDEVINDEX, PSFMIDILOCATION,
                               PSFMIDILOCATION, PSFBUFFEROBJECT ) ;
    LRESULT (CDECL *SF_GetLoadedPresetDescriptor) ( SFDEVINDEX, PSFMIDILOCATION,

                                              PSFBUFFEROBJECT ) ;
    LRESULT (CDECL *SF_ClearLoadedPreset) (SFDEVINDEX, PSFMIDILOCATION ) ;
 
    // Sound Waveform management functions
    LRESULT (CDECL *SF_LoadWaveform)  ( SFDEVINDEX, PSFMIDILOCATION, PSFBUFFEROBJECT ) ;
    LRESULT (CDECL *SF_ClearLoadedWaveform)  ( SFDEVINDEX, PSFMIDILOCATION ) ;

    // Device Specific Message
    LRESULT (CDECL *SF_DeviceSpecificMessage) ( SFDEVINDEX, UINT, LPARAM, LPARAM ) ;

} SFMANL100API, *PSFMANL100API;


#endif   / * __SFMAN_H */

/*
---------------------------------------------------
CUT HERE WHEN GO PUBLIC ;)

Notes and Vienna vs. APS Manager Expirience:

use SFDEVMANCAP_VIENNA flag for m_DevCaps to be visible from Vienna

CSFDevObject:
	APS Manager returns 0x873e0200 in m_DevCaps 
	m_DevNode - can be NULL
	m_SysExData - can be NULL

APS Manager returns 203 for SFDEV_VIENNA_FREE_PRESET - dunno why...

Vienna 2.3 uses some reserved members of structures with APS e.g:
	CViSmplObject
		dwReserved = 0x00360000
	CViPlayObject
		dwReserved = 0x00000009

END OF CUT HERE
---------------------------------------------------
*/
