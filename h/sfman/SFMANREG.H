/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//   SFMANREG.H                                                            //
//                                                                         //
//   Description : SoundFont Manager header file for acccesing             //
//                 device registration functions                           //
//                                                                         //
//   Copyright (c) Creative Technology Ltd. / E-mu Systems, Inc.           //
//                 1996-1997. All rights reserved.                         //
//                                                                         //
//   Revision:     1.0                                                     //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef __SFMANREG_H
#define __SFMANREG_H

#include <windows.h>
#include <winbase.h>

///// E R R O R  C O D E S  /////////////////////////////////////////////////

#define     SFERR_BASE_REGISTRY            300

#define     SFERR_FAIL_CREATEKEY           (SFERR_BASE_REGISTRY+1)
#define     SFERR_OLD_VERSION              (SFERR_BASE_REGISTRY+2)
#define     SFERR_INCOMPLETE_ENTRY         (SFERR_BASE_REGISTRY+3)
#define     SFERR_INVALID_KEY              (SFERR_BASE_REGISTRY+4)
#define     SFERR_INVALID_ENTRY            (SFERR_BASE_REGISTRY+5)
#define     SFERR_ENTRYSTRING_TOO_LONG     (SFERR_BASE_REGISTRY+6)
#define     SFERR_LAST_REG                 (SFERR_BASE_REGISTRY+7)

///// D A T A  S T R U C T U R E S /////////////////////////////////////////
  
#pragma pack(2)
typedef struct SF_REGISTERINFOTAG {
    CHAR  szManagerName[20];         	// File Name for Device Manager
    CHAR  szManagerEntryPointName[20]; 	// Device Function exporting  
} SF_REGISTERINFO10x, *PSF_REGISTERINFO10x ;

typedef struct {
    CHAR  szManufacturerProductName[40];  // This "friendly name"
    CHAR  szDeviceCategory[4] ;           // 1 = h/w synth; 0=soft.
    CHAR  szMMSystemCompatible[4] ;       // 1 = Yes, 0 = "Hidden" device
    CHAR  szManagerName[20];
    CHAR  szManagerEntryPointName[20];
    CHAR  szMIDIDriverName[20];
    CHAR  szDeviceName[20];               // String appear in driver
    CHAR  szVersion[8];
} SF_REGISTERINFO10u, *PSF_REGISTERINFO10u ;

#pragma pack()

///// F U N C T I O N    P R O T O T Y P E S ///////////////////////////////


#endif   / * __SFMANREG_H */
