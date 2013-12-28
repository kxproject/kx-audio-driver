/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//   SFCVRT.H                                                              //
//                                                                         //
//   Description : Header file for acccesing the SoundFont Management      //
//                 System's File Format Conversion Module                  //
//                                                                         //
//   Copyright (c) Creative Technology Ltd. / E-mu Systems, Inc.           //
//                 1996-1997. All rights reserved.                         //
//                                                                         //
//   Revision:     1.00                                                    //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef __SFCNVRT_H
#define __SFCNVRT_H

///// W I N D O W S   I N C L U D E S  //////////////////////////////////////

#include <windows.h>
#include <winbase.h>
#include "sfman.h"

                      
///////////////////////////////////////////////////////////////////////////////////
//  D A T A   S T R U C T U R E S   A N D   E N U M E R A T I O N   T A B L E S  //
///////////////////////////////////////////////////////////////////////////////////

#pragma pack(2)

/////  E N U M  /////

/*
   This enumeration list is the series of "tokens" which uniquely
   identify a single file format
*/               

typedef enum SffTokenTag {
    SFENUM_FORMAT_SBK = 0x4B42532E,
    SFENUM_FORMAT_SF2 = 0x3246532E,
    SFENUM_FORMAT_WAV = 0x5641572E
} SFFFTOKEN, *PSFFFTOKEN;                                    

//typedef SFFFTOKEN*    PSFFFTOKEN ;

  
typedef enum InterfaceIdTag {
	ID_SFCONV100API
} INTERFACEAPIID;

typedef INTERFACEAPIID*	PINTERFACEAPIID;
///// S T R U C T /////

#pragma pack() 

/////  D E F I N E S  ///////////////////////////////////////////////////////

#define SF_FILE_FORMAT_CONVERTER_FILENAME "SFCVRT32.DLL"
#define SF_CVRT_FUNCTION_TABLE_NAME "SFConverter"
#define SF_CURRENT_SFCNVRT_INTERFACE_ID ID_SFMANL100API
#define SF_SBK_FORMAT_DISPLAY_NAME "SoundFont 1.0 Bank"
#define SF_SF2_FORMAT_DISPLAY_NAME "SoundFont 2.0 Bank"
#define SF_SBK_FORMAT_EXTENSION "*.SBK"
#define SF_SF2_FORMAT_EXTENSION "*.SF2"
#define NO_OF_INFORMATS 1
#define NO_OF_OUTFORMATS 1

                                    
/////  S F C V R T   F u n c t i o n   T a b l e   ////////////////////////////


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


typedef struct SFCONVERTERTAG {

    LRESULT (CDECL *SF_QueryInterface) ( INTERFACEAPIID, PDWORD );

} SFCONVERTER, *PSFCONVERTER ;


typedef struct SFCVRTL100TAG {

    // Informational functions
    LRESULT (CDECL *SF_GetNumFormatsSupported) ( PDWORD, PDWORD ) ;
    LRESULT (CDECL *SF_GetSupportedFormats)    ( PSFFFTOKEN, PSFFFTOKEN ) ;
    LRESULT (CDECL *SF_IsFormatSupported)      ( SFFFTOKEN, BOOL, PBOOL ) ;
    LRESULT (CDECL *SF_GetFormatDisplayName)   ( SFFFTOKEN, PSFBUFFEROBJECT ) ;
    LRESULT (CDECL *SF_GetFormatExtensions)    ( SFFFTOKEN, PSFBUFFEROBJECT ) ;

    // Conversion functions
    LRESULT (CDECL *SF_ConvertFile) ( PSFBUFFEROBJECT, PSFBUFFEROBJECT, SFFFTOKEN ) ;

} SFCVRTL100API , *PSFCVRTL100API ;


#endif   / * __SFMAN_H */

