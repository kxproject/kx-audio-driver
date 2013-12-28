/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//   SFEDTYPE.H                                                            //
//                                                                         //
//   Description : Header file for acccesing the SoundFont Edit Engine     //
//                 (Data types definitions)                                //
//                                                                         //
//   Copyright (c) Creative Technology Ltd. / E-mu Systems, Inc.           //
//                 1997-1998. All rights reserved.                         //
//                                                                         //
//   Revision:     1.00                                                    //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef _SFEDTYPE_H
#define _SFEDTYPE_H

#pragma pack(1)


//*********************************************************************
//*  Error codes
//*********************************************************************

#define SUCCESS                             0
#define STATUS_SUCCESS                      0

#define FAIL                                1
#define STATUS_ERROR                        1
#define STATUS_READ_ERROR                   2
#define STATUS_OPEN_ERROR                   3
#define STATUS_WRITE_ERROR                  4
#define STATUS_CREATE_ERROR                 5
#define STATUS_DELETE_ERROR                 6
#define STATUS_INDEX_OVERFLOW               7

#define STATUS_NOT_FOUND                    10
#define STATUS_OUT_OF_MEM                   11

// Vienna 2.1
#define STATUS_LINK_FAIL                    50
#define STATUS_SAVE_ORG_FAIL                51
#define STATUS_DIFF_ROM_VER                 52


#define STATUS_FORMAT_ERROR                 0x1000

// possible types of format error

#define FE_RIFF                             0

//General - from 100
#define STATUS_BUFFER_TOO_SMALL             100
#define STATUS_8BIT_SMPL_SIZE               101

//Bank - from 200
#define STATUS_INVALID_INFO_FIELD           200
#define STATUS_INVALID_BANK_ID              201

//Preset - from 250
#define STATUS_PRESET_NOT_FOUND             250
#define STATUS_NO_MORE_PRESET               251
#define STATUS_DUPLICATE_PRESET_NAME        252
#define STATUS_DUPLICATE_ID_PAIR            253
#define STATUS_INVALID_PRESET_ID            254

//Layer - from 300
#define STATUS_LAYER_NOT_FOUND              300
#define STATUS_NO_MORE_LAYER                301
#define STATUS_GLOBAL_LAYER_EXISTED         302
#define STATUS_INVALID_LAYER                303

//Layer Instance - from 350
#define STATUS_NO_MORE_LAYER_INSTANCE       350
#define STATUS_INVALID_LAYER_INSTANCE       351
#define STATUS_LAYER_INSTANCE_NOT_FOUND     352

//Instrument - from 400
#define STATUS_INSTR_NOT_FOUND              400
#define STATUS_NO_MORE_INSTR                401
#define STATUS_DUPLICATE_INSTR_NAME         402
#define STATUS_INSTR_STILL_IN_USE           403

//Zone - from 450
#define STATUS_ZONE_NOT_FOUND               450
#define STATUS_NO_MORE_ZONE                 451
#define STATUS_GLOBAL_ZONE_EXISTED          452
#define STATUS_INVALID_ZONE                 453

//Zone Instance - from 500
#define STATUS_NO_MORE_ZONE_INSTANCE        500
#define STATUS_INVALID_ZONE_INSTANCE        501
#define STATUS_ZONE_INSTANCE_NOT_FOUND      502

//Generators - from 550
#define STATUS_INVALID_GEN                  550
#define STATUS_INVALID_GEN_VALUE            551
#define STATUS_GEN_NOT_FOUND                552
#define STATUS_NO_MORE_GEN                  553
#define STATUS_CURR_GEN_NOT_FOUND           554

//Sample - from 600
#define STATUS_SMPL_NOT_FOUND               600
#define STATUS_SMPL_STILL_IN_USE            601
#define STATUS_SMPL_LINKAGE_ERR             602
#define STATUS_SMPL_SAME                    603
#define STATUS_SMPL_NOT_PLAYED              604
#define STATUS_NO_MORE_SMPL                 605
#define STATUS_DUPLICATE_SMPL_NAME          606
#define STATUS_INVALID_SMPL_FIELD           607
#define STATUS_INVALID_LOOP_POINTS          608

/* Vienna 2.1 */
#define STATUS_DUPLICATE_SAMPLE             609
#define STATUS_INVALID_SMPL_TYPE            610
#define STATUS_STEREO_SMPL_STILL_IN_USE     611
#define STATUS_ROM_VER_ERROR                612

/* Vienna 2.11 - TDK 8710 */
#define STATUS_AWE_DIFF_DEV                 675

//Modulators - from 700
#define STATUS_INVALID_MOD                  700
#define STATUS_INVALID_MOD_VALUE            701
#define STATUS_MOD_NOT_FOUND                702
#define STATUS_NO_MORE_MOD                  703
#define STATUS_CURR_MOD_NOT_FOUND           704

//*********************************************************************
//*  Basic data types
//*********************************************************************

// pointer types
#if defined(_DLL32) || defined(_WIN32)
    #define _FAR_
#else
    #define _FAR_   __far
#endif


//definition for NULL pointer
#if !defined(NULL)
    #ifdef __cplusplus
        #define NULL    0
    #else
        #define NULL    ((void _FAR_ *)0)
    #endif
#endif  // NULL


#if !defined(_INC_WINDOWS)
enum {                      //TRUE and FALSE are defined by WINDOWS
    FALSE = 0,
    TRUE = 1
};
#endif  //_INC_WINDOWS


#if defined(_INC_WINDOWS) && !defined(_WIN32)

    // "WINDOWS.H" for 16-bits included

    // data types
    typedef signed int          INT;
    typedef signed short        SHORT;
    typedef unsigned long       ULONG;
    typedef double              DOUBLE;
    typedef unsigned int        STATUS;
    typedef int                 HFILE;

    // pointer types
    #define PBYTE               LPBYTE
    #define PWORD               LPWORD
    #define PDWORD              LPDWORD
    #define PINT                LPINT
    #define PLONG               LPLONG
    #define PULONG              LPULONG
    #define PVOID               LPVOID

    typedef BOOL _FAR_          *PBOOL;
    typedef char _FAR_          *PCHAR;
    typedef UINT _FAR_          *PUINT;
    typedef SHORT _FAR_         *PSHORT;
    typedef DOUBLE _FAR_        *PDOUBLE;
    typedef HFILE _FAR_         *PHFILE;

#else

    // "WINDOWS.H" not included or 32-bits compilation

    // data types
    typedef int                 BOOL;
    typedef unsigned char       BYTE;
    typedef unsigned short      WORD;
    typedef unsigned long       DWORD;
    typedef signed short        SHORT;
    typedef signed int          INT;
    typedef unsigned int        UINT;
    typedef signed long         LONG;
    typedef unsigned long       ULONG;
    typedef double              DOUBLE;
    typedef unsigned int        STATUS;
    typedef int                 HFILE;

    // pointer types
    typedef BOOL _FAR_          *PBOOL;
    typedef BYTE _FAR_          *PBYTE;
    typedef WORD _FAR_          *PWORD;
    typedef DWORD _FAR_         *PDWORD;
    typedef SHORT _FAR_         *PSHORT;
    typedef INT _FAR_           *PINT;
    typedef UINT _FAR_          *PUINT;
    typedef LONG _FAR_          *PLONG;
    typedef ULONG _FAR_         *PULONG;
    typedef DOUBLE _FAR_        *PDOUBLE;
    typedef char _FAR_          *PCHAR;
    typedef void _FAR_          *PVOID;
    typedef HFILE _FAR_         *PHFILE;

#endif  //_INC_WINDOWS && _WIN32

// Vienna 2.2 - SoundFont 2.1 Enumerator Value
typedef enum ModTypeTag {
   sfmtMaster,
   sfmtGlobal,
   sfmtLocal
}SFMODTYPE, _FAR_ *PSFMODTYPE;

//*********************************************************************
//*  Basic SoundFont data types
//*********************************************************************

typedef struct 
{
    WORD    wModSrcOper;
    WORD    wModDestOper;
    SHORT   shModAmount;
    WORD    wModAmtSrcOper;
    WORD    wModTransOper;
}
SFPRESETMOD, _FAR_ *PSFPRESETMOD;

typedef struct 
{
    WORD        wGenOper;
    WORD        genAmount;
}
SFPRESETGEN, _FAR_ *PSFPRESETGEN;

typedef struct 
{
    WORD    wModSrcOper;
    WORD    wModDestOper;
    SHORT   shModAmount;
    WORD    wModAmtSrcOper;
    WORD    wModTransOper;
}
SFINSTMOD, _FAR_ *PSFINSTMOD;

typedef struct 
{
    WORD        wGenOper;
    WORD        genAmount;
}
SFINSTGEN, _FAR_ *PSFINSTGEN;

/* Vienna 2.2 - SoundFont 2.1 */
typedef struct {

	DWORD sizeOf;     // size of sfEditDevInfo structure

	PCHAR pchProduct; // pointer to product string
	DWORD prodSize;   // size of product string
	PCHAR pchROM;     // pointer to ROM string
	DWORD romSize;    // size of ROM string
	PCHAR pchISng;    // pointer to sound engine string
	DWORD isngSize;   // size of sound engine string

	DWORD baseAddr;   // base address
	DWORD devVer;     // version number of the device
	DWORD sfLowVer;   // low SoundFont support version
	DWORD sfHighVer;  // high SoundFont support version

} sfEditDevInfo, _FAR_ *PSFEDTDEVINFO;

//*********************************************************************
//*  SoundFont Object
//*********************************************************************

//bank
typedef void _FAR_ *    HSFBANK;

//preset
typedef void _FAR_ *    HPRESETOBJ;
typedef void _FAR_ *    HLAYEROBJ;
typedef void _FAR_ *    HLAYERINSTOBJ;
typedef void _FAR_ *    HLAYERMODINSTOBJ;

//instrument
typedef void _FAR_ *    HINSTROBJ;
typedef void _FAR_ *    HZONEOBJ;
typedef void _FAR_ *    HZONEINSTOBJ;
typedef void _FAR_ *    HZONEMODINSTOBJ;

//sample
typedef void _FAR_ *    HSMPLOBJ;

//AWEMAN
typedef void _FAR_ *    HPLAYOBJ;

//Vienna 2.2 - SoundFont 2.1
typedef void _FAR_ *	HMODTOKENOBJ;




#endif  // _SFEDTYPE_H
