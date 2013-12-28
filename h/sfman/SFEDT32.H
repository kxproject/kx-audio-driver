/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//   SFEDT32.H                                                             //
//                                                                         //
//   Description : Header file for acccesing the SoundFont Edit Engine     //
//                                                                         //
//   Copyright (c) Creative Technology Ltd. / E-mu Systems, Inc.           //
//                 1997-1998. All rights reserved.                         //
//                                                                         //
//   Revision:     1.00                                                    //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef __SFEDT32_H
#define __SFEDT32_H

///// W I N D O W S   I N C L U D E S  //////////////////////////////////////

#include <windows.h>
#include <winbase.h>
#include "sfedtype.h"

///// E R R O R  C O D E S  /////////////////////////////////////////////////

#define     SFERR_NOERR                     0L
#define     SFERR_BASE_API                  400L


#define     SFERR_INTERFACE_NOT_SUPPORTED   (SFERR_BASE_API+4)

///////////////////////////////////////////////////////////////////////////////////
//  S U P P O R T E D   I N T E R F A C E     I D                              //
///////////////////////////////////////////////////////////////////////////////////

// Currently supported Interface ID

enum INTERFACEID {

    ID_SFEDITAPI100 = 0x00010000,

} ;




///////////////////////////////////////////////////////////////////////////////////
//  D A T A   S T R U C T U R E S   A N D   E N U M E R A T I O N   T A B L E S  //
///////////////////////////////////////////////////////////////////////////////////

#pragma pack(2)

/////  D E F I N E S  ///////////////////////////////////////////////////////

#ifndef DLLAPI
#define DLLAPI _declspec (dllimport)
#endif

#define SF_EDIT_ENGINE_FILENAME "SFEDT32.DLL"
#define SF_EDIT_ENGINE_NAME "SFEditEngine"

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


typedef struct {

    LRESULT (CDECL *SF_QueryInterface) ( INTERFACEID, PDWORD );

} SFEDIT, *PSFEDIT;


typedef struct {

    ////////////////////////////////////////////
    // Bank Level Operations
    ////////////////////////////////////////////

    // Read Operations
    STATUS (CDECL *sfNew) (HSFBANK*);
    STATUS (CDECL *sfChangeIntSrcFile) ( HSFBANK, PCHAR );
    STATUS (CDECL *sfSetINFO) ( HSFBANK, UINT, PVOID );
    STATUS (CDECL *sfSave) ( HSFBANK, PCHAR );
    STATUS (CDECL *sfSaveToOrg) ( HSFBANK, PCHAR, BOOL);
    STATUS (CDECL *sfSaveAsToOrg) ( HSFBANK, PCHAR, BOOL, DWORD);

    // Write Operations
    STATUS (CDECL *sfOpen) ( PCHAR, HSFBANK *);
    STATUS (CDECL *sfClose) ( HSFBANK );
    STATUS (CDECL *sfRelease) ( HSFBANK );
    STATUS (CDECL *sfLoadInfo) ( HSFBANK );
    STATUS (CDECL *sfLoadFile) ( HSFBANK );
    STATUS (CDECL *sfGetINFOSize) ( HSFBANK, UINT, PUINT );
    STATUS (CDECL *sfGetINFO) ( HSFBANK, UINT, UINT, PVOID );

    ///////////////////////////////////////////////////
    // Preset Level Operations
    ///////////////////////////////////////////////////

    // preset read funtions' prototypes
    STATUS (CDECL *sfPresetCount) ( HSFBANK, PWORD );
    STATUS (CDECL *sfPresetGet) ( HSFBANK, WORD, WORD, HPRESETOBJ * );
    STATUS (CDECL *sfPresetGetFirst) ( HSFBANK, HPRESETOBJ * );
    STATUS (CDECL *sfPresetGetNext) ( HPRESETOBJ, HPRESETOBJ * );
    STATUS (CDECL *sfPresetLayerCount) ( HPRESETOBJ, PWORD );
    STATUS (CDECL *sfPresetGetName) ( HPRESETOBJ, PCHAR );
    STATUS (CDECL *sfPresetGetBankID) ( HPRESETOBJ, PWORD );
    STATUS (CDECL *sfPresetGetPresetID) ( HPRESETOBJ, PWORD );
               
    // preset write functions' prototypes
    STATUS (CDECL *sfPresetNew) ( HSFBANK, PCHAR, WORD, WORD, HINSTROBJ, HPRESETOBJ * );
    STATUS (CDECL *sfPresetCopy) ( HSFBANK, HPRESETOBJ, WORD, WORD, PCHAR, HPRESETOBJ * );
    STATUS (CDECL *sfPresetDelete) ( HSFBANK, HPRESETOBJ );
    STATUS (CDECL *sfPresetSetName) ( HSFBANK, HPRESETOBJ, PCHAR );
    STATUS (CDECL *sfPresetSetBankID) ( HSFBANK, HPRESETOBJ, WORD );
    STATUS (CDECL *sfPresetSetPresetID) ( HSFBANK, HPRESETOBJ, WORD );

    //***************************************************************************
    //* LAYER LEVEL
    //***************************************************************************

    // layer read functions' prototypes
    STATUS (CDECL *sfLayerGet) ( HPRESETOBJ, HINSTROBJ, HLAYEROBJ * );
    STATUS (CDECL *sfLayerGetGlobal) ( HPRESETOBJ, HLAYEROBJ * );
    STATUS (CDECL *sfLayerGetFirst) ( HPRESETOBJ, HLAYEROBJ * );
    STATUS (CDECL *sfLayerGetNext) ( HLAYEROBJ, HLAYEROBJ * );
    STATUS (CDECL *sfLayerGetInstrument) ( HLAYEROBJ, HINSTROBJ * );
    STATUS (CDECL *sfLayerGetKeyRange) ( HLAYEROBJ, PSFPRESETGEN );
    STATUS (CDECL *sfLayerGetVelRange) ( HLAYEROBJ, PSFPRESETGEN );
    STATUS (CDECL *sfLayerGetFirstGen) ( HLAYEROBJ, PSFPRESETGEN );
    STATUS (CDECL *sfLayerGetNextGen) ( HLAYEROBJ, PSFPRESETGEN );
    STATUS (CDECL *sfLayerGetFirstMod) ( HLAYEROBJ, PSFPRESETMOD );
    STATUS (CDECL *sfLayerGetNextMod) ( HLAYEROBJ, PSFPRESETMOD );

    // Vienna 2.2 - SoundFont 2.1
    STATUS (CDECL *sfLayerGetFirstModEx) ( HLAYEROBJ, HLAYEROBJ _FAR_ *, WORD, PSFPRESETMOD, PSFMODTYPE, HMODTOKENOBJ _FAR_ *);
    STATUS (CDECL *sfLayerGetNextModEx) ( HLAYEROBJ, HLAYEROBJ _FAR_ *, WORD, PSFPRESETMOD, PSFMODTYPE, HMODTOKENOBJ );
                                       
    // layer write functions' prototypes
    STATUS (CDECL *sfLayerNew) ( HPRESETOBJ, HINSTROBJ, HLAYEROBJ * );
    STATUS (CDECL *sfLayerNewGlobal) ( HPRESETOBJ, HLAYEROBJ * );
    STATUS (CDECL *sfLayerDelete) ( HPRESETOBJ, HLAYEROBJ );
    STATUS (CDECL *sfLayerChangeInstrument) ( HLAYEROBJ, HINSTROBJ );
    STATUS (CDECL *sfLayerSetKeyRange) ( HLAYEROBJ, PSFPRESETGEN );
    STATUS (CDECL *sfLayerSetVelRange) ( HLAYEROBJ, PSFPRESETGEN );
    STATUS (CDECL *sfLayerSetGen) ( HLAYEROBJ, PSFPRESETGEN );
    STATUS (CDECL *sfLayerDelGen) ( HLAYEROBJ, PSFPRESETGEN );
    STATUS (CDECL *sfLayerCopyGlobal) ( HLAYEROBJ, HPRESETOBJ, HLAYEROBJ * );
    STATUS (CDECL *sfLayerCopy) ( HLAYEROBJ, HPRESETOBJ, HLAYEROBJ * );
    STATUS (CDECL *sfLayerSetMod) ( HLAYEROBJ, PSFPRESETMOD );
    STATUS (CDECL *sfLayerDelMod) ( HLAYEROBJ, PSFPRESETMOD );

    // Vienna 2.2 - SoundFont 2.1
    STATUS (CDECL *sfLayerSetModEx) ( HLAYEROBJ _FAR_ *, WORD, PSFPRESETMOD );

    //***************************************************************************
    //* LAYER INSTANCE LEVEL
    //***************************************************************************

    // layer instance read functions' prototypes
    STATUS (CDECL *sfLayerInstanceGetCurr) ( HLAYEROBJ, HLAYERINSTOBJ * );
    STATUS (CDECL *sfLayerInstanceGetFirst) ( HLAYEROBJ, HLAYERINSTOBJ * );
    STATUS (CDECL *sfLayerInstanceGetNext) ( HLAYERINSTOBJ, HLAYERINSTOBJ * );

    // layer instance write functions' prototypes
    STATUS (CDECL *sfLayerInstanceDel) ( HLAYEROBJ, HLAYERINSTOBJ );
    STATUS (CDECL *sfLayerInstanceSetCurr) ( HLAYEROBJ, HLAYERINSTOBJ );
    STATUS (CDECL *sfLayerInstanceKeepCurr) ( HLAYEROBJ );
    STATUS (CDECL *sfLayerInstanceCopy) ( HLAYERINSTOBJ, HLAYEROBJ, HLAYERINSTOBJ *);
    STATUS (CDECL *sfLayerInstanceNew) ( HLAYEROBJ, HLAYERINSTOBJ * );


    //***************************************************************************
    //* INSTRUMENT LEVEL
    //***************************************************************************
   
    // instrument read functions' prototypes
    STATUS (CDECL *sfInstrCount) ( HSFBANK, PWORD );
    STATUS (CDECL *sfInstrGet) ( HSFBANK, const PCHAR, HINSTROBJ * );
    STATUS (CDECL *sfInstrGetFirst) ( HSFBANK, HINSTROBJ * );
    STATUS (CDECL *sfInstrGetNext) ( HINSTROBJ, HINSTROBJ * );
    STATUS (CDECL *sfInstrZoneCount) ( HINSTROBJ, PWORD );
    STATUS (CDECL *sfInstrGetName) ( HINSTROBJ, PCHAR );

    // instrument write functions' prototypes
    STATUS (CDECL *sfInstrNew) ( HSFBANK, PCHAR, HSMPLOBJ, HINSTROBJ * );
    STATUS (CDECL *sfInstrCopy) ( HSFBANK, HINSTROBJ, PCHAR, HINSTROBJ * );
    STATUS (CDECL *sfInstrDelete) ( HSFBANK, HINSTROBJ );
    STATUS (CDECL *sfInstrSetName) ( HSFBANK, HINSTROBJ, PCHAR );


    //***************************************************************************
    //* ZONE LEVEL
    //***************************************************************************
   
    // Zone read functions' prototypes
    STATUS (CDECL *sfZoneGet) ( HINSTROBJ, HSMPLOBJ, HZONEOBJ * );
    STATUS (CDECL *sfZoneGetGlobal) ( HINSTROBJ, HZONEOBJ * );
    STATUS (CDECL *sfZoneGetFirst) ( HINSTROBJ, HZONEOBJ * );
    STATUS (CDECL *sfZoneGetNext) ( HZONEOBJ, HZONEOBJ * );
    STATUS (CDECL *sfZoneGetSample) ( HZONEOBJ, HSMPLOBJ * );
    STATUS (CDECL *sfZoneGetKeyRange) ( HZONEOBJ, PSFINSTGEN );
    STATUS (CDECL *sfZoneGetVelRange) ( HZONEOBJ, PSFINSTGEN );
    STATUS (CDECL *sfZoneGetFirstGen) ( HZONEOBJ, PSFINSTGEN );
    STATUS (CDECL *sfZoneGetNextGen) ( HZONEOBJ, PSFINSTGEN );
    STATUS (CDECL *sfZoneGetFirstMod) ( HZONEOBJ, PSFINSTMOD );
    STATUS (CDECL *sfZoneGetNextMod) ( HZONEOBJ, PSFINSTMOD );

    // Vienna 2.2 - SoundFont 2.1
    STATUS (CDECL *sfZoneGetFirstModEx) ( HZONEOBJ, HZONEOBJ _FAR_ *, WORD, PSFINSTMOD, PSFMODTYPE, HMODTOKENOBJ _FAR_ *);
    STATUS (CDECL *sfZoneGetNextModEx) ( HZONEOBJ, HZONEOBJ _FAR_ *, WORD, PSFINSTMOD, PSFMODTYPE, HMODTOKENOBJ );


    // Zone write functions' prototypes
    STATUS (CDECL *sfZoneNew) ( HINSTROBJ, HSMPLOBJ, HZONEOBJ * );
    STATUS (CDECL *sfZoneNewGlobal) ( HINSTROBJ, HZONEOBJ * );
    STATUS (CDECL *sfZoneDelete) ( HINSTROBJ, HZONEOBJ );
    STATUS (CDECL *sfZoneChangeSample) ( HZONEOBJ, HSMPLOBJ );
    STATUS (CDECL *sfZoneSetKeyRange) ( HZONEOBJ, PSFINSTGEN );
    STATUS (CDECL *sfZoneSetVelRange) ( HZONEOBJ, PSFINSTGEN );
    STATUS (CDECL *sfZoneSetGen) ( HZONEOBJ, PSFINSTGEN );
    STATUS (CDECL *sfZoneDelGen) ( HZONEOBJ, PSFINSTGEN );
    STATUS (CDECL *sfZoneCopyGlobal) ( HZONEOBJ, HINSTROBJ, HZONEOBJ * );
    STATUS (CDECL *sfZoneCopy) ( HZONEOBJ, HINSTROBJ, HZONEOBJ * );
    STATUS (CDECL *sfZoneSetMod) ( HZONEOBJ, PSFINSTMOD );
    STATUS (CDECL *sfZoneDelMod) ( HZONEOBJ, PSFINSTMOD );

    // Vienna 2.2 - SoundFont 2.1
    STATUS (CDECL *sfZoneSetModEx) ( HZONEOBJ _FAR_ *, WORD, PSFINSTMOD );

    //***************************************************************************
    //* ZONE INSTANCE LEVEL
    //***************************************************************************
   
    // Zone instance read functions' prototypes
    STATUS (CDECL *sfZoneInstanceGetCurr) ( HZONEOBJ, HZONEINSTOBJ * );
    STATUS (CDECL *sfZoneInstanceGetFirst) ( HZONEOBJ, HZONEINSTOBJ * );
    STATUS (CDECL *sfZoneInstanceGetNext) ( HZONEINSTOBJ, HZONEINSTOBJ * );

    // Zone instance write functions' prototypes
    STATUS (CDECL *sfZoneInstanceDel) ( HZONEOBJ, HZONEINSTOBJ );
    STATUS (CDECL *sfZoneInstanceSetCurr) ( HZONEOBJ, HZONEINSTOBJ );
    STATUS (CDECL *sfZoneInstanceKeepCurr) ( HZONEOBJ );
    STATUS (CDECL *sfZoneInstanceCopy) ( HZONEINSTOBJ, HZONEOBJ, HZONEINSTOBJ * );
    STATUS (CDECL *sfZoneInstanceNew) ( HZONEOBJ, HZONEINSTOBJ * );


    //***************************************************************************
    //* SAMPLE LEVEL
    //***************************************************************************
   
    // sample read functions' prototypes
    STATUS (CDECL *sfSmplCount) ( HSFBANK, PWORD );
    STATUS (CDECL *sfSmplGet) ( HSFBANK, const PCHAR, HSMPLOBJ * );
    STATUS (CDECL *sfSmplGetFirst) ( HSFBANK, HSMPLOBJ * );
    STATUS (CDECL *sfSmplGetNext) ( HSMPLOBJ, HSMPLOBJ * );
    STATUS (CDECL *sfSmplGetDataSize) ( HSMPLOBJ, UINT, PUINT );
    STATUS (CDECL *sfSmplGetData) ( HSMPLOBJ, UINT, UINT, PVOID );
    STATUS (CDECL *sfSmplGetLinkedSmpl) ( HSMPLOBJ, HSMPLOBJ * );

    // sample write functions' prototypes

    STATUS (CDECL *sfSmplNewROM) ( HSFBANK, PCHAR, DWORD, DWORD, DWORD, DWORD, HSMPLOBJ * );
    STATUS (CDECL *sfSmplExtract) ( HSFBANK, HSMPLOBJ, PCHAR * );
    STATUS (CDECL *sfSmplExport2Wave) ( HSFBANK, HSMPLOBJ, PCHAR );
    STATUS (CDECL *sfSmplDelete) ( HSFBANK, HSMPLOBJ );
    STATUS (CDECL *sfSmplSetData) ( HSFBANK, HSMPLOBJ, UINT, PVOID );
    STATUS (CDECL *sfSmplLink) ( HSFBANK, HSMPLOBJ, HSMPLOBJ );
    STATUS (CDECL *sfSmplUnLink) ( HSMPLOBJ, HSMPLOBJ );

    /* Vienna 2.1 */
    STATUS (CDECL *sfSPSmplDelete) ( HSFBANK, HSMPLOBJ );
    STATUS (CDECL *sfSPSmplExport2Wave) ( HSFBANK, HSMPLOBJ, PCHAR );

    //***************************************************************************
    //* INTER-BANK LEVEL
    //***************************************************************************

    STATUS (CDECL *sfImportSmplSF2) (HSFBANK,HSMPLOBJ,HSFBANK,PCHAR,HSMPLOBJ *);
    STATUS (CDECL *sfImportSmplWave) ( PCHAR, HSFBANK, PCHAR, HSMPLOBJ * );
    STATUS (CDECL *sfImportInstr) ( HSFBANK, HINSTROBJ, HSFBANK, PCHAR, HINSTROBJ * );
    STATUS (CDECL *sfImportPreset) ( HSFBANK, HPRESETOBJ, HSFBANK, WORD, WORD, PCHAR, HPRESETOBJ * );

    /* Vienna 2.1 */
    STATUS (CDECL *sfCheckDuplicateSmpl) ( HSFBANK, BOOL );
    STATUS (CDECL *sfSPImportSmplSF2) (HSFBANK,HSMPLOBJ,HSFBANK,PCHAR,HSMPLOBJ *);
    STATUS (CDECL *sfSPImportInstr) ( HSFBANK, HINSTROBJ, HSFBANK, PCHAR, HINSTROBJ * );
    STATUS (CDECL *sfSPImportPreset) ( HSFBANK, HPRESETOBJ, HSFBANK, WORD, WORD, PCHAR, HPRESETOBJ * );


} SFEDITAPI_100, *PSFEDITAPI_100;


#endif   / * __SFEDT32_H */

