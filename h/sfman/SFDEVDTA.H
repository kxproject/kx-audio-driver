/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//   SFDEVDTA.H                                                            //
//                                                                         //
//   Description : Header file for the SoundFont Management portions of a  //
//                 single SoundFont Compatible Device.                     //
//                                                                         //
//   Copyright (c) Creative Technology Ltd. / E-mu Systems, Inc.           //
//                 1996-1997. All rights reserved.                         //
//                                                                         //
//   Notes:        This header file is INDEPENDENT of the 'SFMAN.H'        //
//                 header file. Any similarities in the prototypes or      //
//                 values should be viewed as purely coincidental.         //
//                                                                         //
//   Revision:     1.02  09/09/97                                          //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef __SFDEVDTA_H
#define __SFDEVDTA_H

// Note to developer:
// Here is support for including basic data type definitions for
// developing ring 0 code for VToolsD or standard MSDDK techniques.
// You may need to modify this section to meet your compiler needs.

#if defined(WIN32)
	#ifndef _INC_WINDOWS
	# ifdef VTOOLSD
	#  ifdef __cplusplus
	#   include <vtoolscp.h>
	#  else
	#   include <vtoolsc.h>
	#  endif
	# else
	#  include <basedef.h>
	# endif
	#endif
#endif

///// E R R O R  C O D E S  /////////////////////////////////////////////////

/* 
    The following is a comprehensive list of the API error message values
*/

/*
--------------------------------
      Error Category
--------------------------------
*/

#define     SFDEVERR_BASE_INVALID                     100L
#define     SFDEVERR_BASE_CONTENTION                  200L
#define     SFDEVERR_BASE_UNDEFINE                    900L

/*
--------------------------------
      No Error
--------------------------------
*/
#define     SFDEVERR_NO_ERR                       0L

/*
--------------------------------
      Invalid Error Messages
--------------------------------
*/
#define     SFDEVERR_DEVICE_DRV_INVALID       (SFDEVERR_BASE_INVALID+1)
#define     SFDEVERR_USERID_INVALID           (SFDEVERR_BASE_INVALID+2)
#define     SFDEVERR_SYNTH_INVALID            (SFDEVERR_BASE_INVALID+5)
#define     SFDEVERR_BANK_INDEX_INVALID       (SFDEVERR_BASE_INVALID+6)
#define     SFDEVERR_PATHNAME_INVALID         (SFDEVERR_BASE_INVALID+7)
#define     SFDEVERR_FORMAT_INVALID           (SFDEVERR_BASE_INVALID+8)
#define     SFDEVERR_PRESET_INDEX_INVALID     (SFDEVERR_BASE_INVALID+9)
#define     SFDEVERR_MSG_INVALID              (SFDEVERR_BASE_INVALID+10)
#define     SFDEVERR_ACCESS_NOT_PERMITTED     (SFDEVERR_BASE_INVALID+11)
#define     SFDEVERR_VERSION_INVALID          (SFDEVERR_BASE_INVALID+12)
#define     SFDEVERR_DEVICE_INVALID           (SFDEVERR_BASE_INVALID+13)
#define     SFDEVERR_SUPPORT_INVALID          (SFDEVERR_BASE_INVALID+14)
#define     SFDEVERR_INVALID_ROM		    (SFDEVERR_BASE_INVALID+15)
#define     SFDEVERR_INVALID_MEMORY_TYPE      (SFDEVERR_BASE_INVALID+16)

/*
--------------------------------
   Contention Error Messages
--------------------------------
*/

#define     SFDEVERR_DLL_BUSY                 (SFDEVERR_BASE_CONTENTION+1)
#define     SFDEVERR_DEVICE_BUSY              (SFDEVERR_BASE_CONTENTION+2)
#define     SFDEVERR_RESOURCE_INSUFFICIENT    (SFDEVERR_BASE_CONTENTION+3)
#define     SFDEVERR_SYSMEM_INSUFFICIENT      (SFDEVERR_BASE_CONTENTION+4)
#define     SFDEVERR_SOUND_MEM_INSUFFICIENT   (SFDEVERR_BASE_CONTENTION+5)

/*
--------------------------------
   Undefined Error Messages
--------------------------------
*/

#define     SFDEVERR_UNDEFINE_ERROR           (SFDEVERR_BASE_UNDEFINE+1)

/////  S F D E V M A N   F u n c t i o n   M e s s a g e s  //////////////////////////
  
/* 
    The following is a comprehensive list of the API command message values
*/

/*
--------------------------------
      Function Category
--------------------------------
*/
#define SFDEV_BASE_INIT                       100
#define SFDEV_BASE_CONFIG                     200
#define SFDEV_BASE_QUERY                      300
  
/*
--------------------------------
      Setup Messages
--------------------------------
*/
#define SFDEV_OPEN                        (SFDEV_BASE_INIT+1)
#define SFDEV_CLOSE                       (SFDEV_BASE_INIT+2)
#define SFDEV_ISHANDLE                    (SFDEV_BASE_INIT+3)        
#define SFDEV_GET_NUM_DEVS                (SFDEV_BASE_INIT+4)       
#define SFDEV_GET_DEVICE_NODE             (SFDEV_BASE_INIT+5)      
#define SFDEV_GET_DEVICE_CAPS             (SFDEV_BASE_INIT+6)     
 
/*
--------------------------------
      Configuration Messages
--------------------------------
*/

#define SFDEV_REFRESH                     (SFDEV_BASE_CONFIG+1)
#define SFDEV_CONF_RESERVED_1             (SFDEV_BASE_CONFIG+2)
#define SFDEV_SELECT_SYNTH_EMULATION      (SFDEV_BASE_CONFIG+3)
#define SFDEV_LOAD_BANK                   (SFDEV_BASE_CONFIG+4)
#define SFDEV_CLEAR_BANK                  (SFDEV_BASE_CONFIG+5)
#define SFDEV_LOAD_PRESET                 (SFDEV_BASE_CONFIG+6)
#define SFDEV_CLEAR_PRESET                (SFDEV_BASE_CONFIG+7)
#define SFDEV_CONF_RESERVED_2             (SFDEV_BASE_CONFIG+8)
#define SFDEV_LOAD_WAVEFORM               (SFDEV_BASE_CONFIG+9)    
#define SFDEV_CLEAR_WAVEFORM              (SFDEV_BASE_CONFIG+10)    

/*
--------------------------------
      Management Messages
--------------------------------
*/

#define SFDEV_QUERY_RESERVED_1            (SFDEV_BASE_QUERY+1)
#define SFDEV_QUERY_RESERVED_2            (SFDEV_BASE_QUERY+2)
#define SFDEV_QUERY_SYNTH_SUPPORT         (SFDEV_BASE_QUERY+3)
#define SFDEV_QUERY_RESERVED_3            (SFDEV_BASE_QUERY+4)
#define SFDEV_GET_SYNTH_EMULATION         (SFDEV_BASE_QUERY+5)
#define SFDEV_GET_BANK_DESCRIPTOR         (SFDEV_BASE_QUERY+6)
#define SFDEV_GET_PRESET_DESCRIPTOR       (SFDEV_BASE_QUERY+7)
#define SFDEV_QUERY_STATIC_SMEM_SIZE      (SFDEV_BASE_QUERY+8)
#define SFDEV_GET_MANAGER_VERSION         (SFDEV_BASE_QUERY+9)
#define SFDEV_IS_MIDI_BANK_USED           (SFDEV_BASE_QUERY+10)
#define SFDEV_QUERY_RESERVED_5            (SFDEV_BASE_QUERY+11)
#define SFDEV_IS_DEVICE_FREE              (SFDEV_BASE_QUERY+12)     
#define SFDEV_GET_BANK_PATHNAME           (SFDEV_BASE_QUERY+13)     
#define SFDEV_GET_DRIVER_VERSION          (SFDEV_BASE_QUERY+14)    
#define SFDEV_GET_WAVEFORM_DESCRIPTOR     (SFDEV_BASE_QUERY+15)    
#define SFDEV_QUERY_SOUNDFONT_SUPPORT     (SFDEV_BASE_QUERY+16)

///////////// O T H E R   D E F I N E S  ///////////////////////////

#define SFDEV_SOURCE_PERCUSSIVE 0x80 
#define SFDEV_TARGET_PERC_BASE  0x80

///////////////////////////////////////////////////////////////////////////////////
//  D A T A   S T R U C T U R E S   A N D   E N U M E R A T I O N   T A B L E S  //
///////////////////////////////////////////////////////////////////////////////////

#pragma pack (2)

/*
--------------------------------
      Enumeration Tables
--------------------------------
*/

/* 
    The following is a comprehensive list of the data structures used in the API  
*/

enum SFDEVMANCAPS {
    SFDEVMANCAP_DYNAMIC_MEM_CAP          = 0x80000000,
    SFDEVMANCAP_SOFTWARE_SYNTH_CAP       = 0x40000000,
    SFDEVMANCAP_HIDDEN_DEVICE_CAP        = 0x20000000,
    SFDEVMANCAP_NON_WAVEFORM_SUPPORT_CAP = 0x10000000,
    SFDEVMANCAP_QUERY_SFVER_CAP          = 0x08000000

};

enum SFDEVMANFLAGS {
    SFDEVMANFLAG_OPER_FILE,
    SFDEVMANFLAG_OPER_MEMORY
};

/*
--------------------------------
      Data Structures
--------------------------------
*/

/*
This CSFParamObject is usually used when required to pass several indices to
the Manager. This happens when configuring a synth emulation or when querying
the device. The LPSFPARAMOBJECT define a far pointer to the parameter object.
*/
typedef struct CSFDevParamObjectTag{
    DWORD           m_SynthIndex;   /* Synth Emulation Index */
    WORD            m_BankIndex;    /* Bank Index*/
    WORD            m_PresetIndex;  /* Preset Index */   
    WORD            m_SysExData[9]; /* Data for device specific implementation */  
} CSFDevParamObject;

typedef CSFDevParamObject * PSFDEVPARAMOBJECT;

typedef const PSFDEVPARAMOBJECT  PCSFDEVPARAMOBJECT;

/*
The CSFBufferObject is used to specified a buffer area to the Manager. This buffer
area will usually be filled with some strings or values. In some occassions,
the buffer itself, actually carries information to the Manager, such as a filename.
The LPBUFFEROBJECT defines a far pointer to the buffer object.
*/

typedef struct CSFDevMIDILocationTag {
    WORD    m_PresetIndex;
    WORD    m_BankIndex;
}CSFDevMIDILocation;

typedef CSFDevMIDILocation *PSFDEVMIDILOCATION;

typedef const PSFDEVMIDILOCATION PCSFDEVMIDILOCATION;


typedef union CSFDevBufVarTag
{
  CSFDevMIDILocation m_MIDILocation;
  DWORD              m_SizeUsed;
} CSFDevBufVar;

typedef CSFDevBufVar PSFDEVBUFVAR;

typedef const PSFDEVBUFVAR PCSFDEVBUFVAR;


typedef struct CSFDevBufferObjectTag {

    DWORD        m_Size;     /* The size of the buffer attached */
    LPSTR        m_Buffer;   /* The pointer to the buffer itself */
    CSFDevBufVar m_Var;      /* Primary scratch variable */
    WORD         m_Flag;     /* Secondary scratch variable */

} CSFDevBufferObject;

typedef CSFDevBufferObject *PSFDEVBUFFEROBJECT;
 
typedef const PSFDEVBUFFEROBJECT  PCSFDEVBUFFEROBJECT;


/*
The CSFDevObject is used to specify the capibilities of a Manager.
The PSFDEVOBJECT defines a far pointer to the capibilities structure.
*/

#define SFDEV_CAPSIZE_100 330
#define SFDEV_CAPSIZE_102 338
#define SFDEV_CAPSIZE_19u 50

typedef struct CSFDevObjectTag {
    WORD    m_SizeOf;           /* Sizeof this structure */
    DWORD   m_BaseAddr;         /* Base address of device */
    DWORD   m_DevNode;          /* Device node of device */
    DWORD   m_RomId;            /* ROM ID of device */
    DWORD   m_RomVer;           /* ROM version of device */
    DWORD_PTR   m_hTask;            /* Task acquiring device */
    DWORD   m_DevCaps;          /* Device capibilities bit field*/
    char    m_DevName[32];      /* MMSystem name for MIDI Device */
    char    m_SndEngine[16];	  /* Sound Engine employed */
    char    m_SysExData[256];   /* Device specific data */
    DWORD   m_SFVerLow;         /* Earliest version of SF format supported */
    DWORD   m_SFVerHigh;        /* Latest version of SF format supported */
} CSFDevObject10x, *PSFDEVOBJECT10x; 

typedef struct {
    WORD    m_SizeOf;           /* sizeof this structure                    */
    DWORD   m_DevID;            /* SF Device ID                             */
    DWORD   m_DevCaps;          /* 1      = Reverb/Chorus                   */
                                /* 1024   = dynamic memory                  */
                                /* 2048   = h/w synth                       */
                                /* 4096   = MMSYSTEM compatible             */

    DWORD   m_RomID;            /* ROM ID of a device                       */
                                /* ( only for hardware with ROM samples )   */
    DWORD   m_RomVer;           /* ROM Version                              */
    CHAR    m_DevName[32];      /* SF compatible dev name                   */
} CSFDevObject10u , *PSFDEVOBJECT10u ;


/*
The CSFDevSFVersion is used to specify the versions of the SoundFont
File format supported by the Manager. The PSFDEVSFVERSION defines a far 
pointer to the SoundFont File Format version structure.
*/
typedef struct {   
   DWORD      m_SFVerLow;
   DWORD      m_SFVerHigh;
} CSFDevSFVersion;

typedef CSFDevSFVersion *PSFDEVSFVERSION;

typedef const PSFDEVSFVERSION PCSFDEVSFVERSION;

#pragma pack()

#endif
