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
//  Version 2.0 ... 1/10/2003
//**************************************************************************//

#ifndef _GSIFNT_HEADER               // Prevent multiple definitions
#define _GSIFNT_HEADER

typedef PVOID PDIOCPARAMETERS;

#include "gsif.h"

//===========================================================================//
//              GSIF    Global constants                                     //
//===========================================================================//
#define IOCTL_GSIF_GETINTERFACE        CTL_CODE(FILE_DEVICE_NSSYS, 0x4001, \
                                                METHOD_IN_DIRECT, FILE_ANY_ACCESS)

#define GSIF2_REGISTRY_PATH       L"SOFTWARE\\TASCAM\\Hardware\\GSIF"
#define REG_GETINTERFACECALLBACK  L"GetInterfaceCallback"
#define REG_DEVICEEXTENSION       L"DeviceExtension"

// new for v2.1 of GSIF, supports 64-bit addresses (only valid on XP64/Vista64)
#define REG_GETINTERFACECALLBACK_HIGH  L"GetInterfaceCallback64"
#define REG_DEVICEEXTENSION_HIGH       L"DeviceExtension64"

//
// ======================= GSIF APIs Commands ===========================
// GigaX to Hw Driver communication
//
#define GSIF_API_GETHWBUFFERINFO           GSIF_API_ENUMERATESYSDRIVERS+1

//
// NT-style driver callbacks
//
typedef DWORD (__stdcall *PFNDRIVERREQUEST)(DWORD,DWORD,PVOID, PVOID);
typedef DWORD (__stdcall *PFNDRIVERISR)(DWORD, PVOID);

//
// Callbacks used for GSIF 2.0
//
struct GSIF_MIDIEVENT;
typedef DWORD (__stdcall *PFNMIDICALLBACK)(DWORD, GSIF_MIDIEVENT *, PVOID);
typedef DWORD (__stdcall *PFNMIDIUART)(DWORD, DWORD, unsigned char *, PVOID);

#ifndef REMOVE_DDKDEFINITIONS
//===========================================================================//
//              GSIF  Interfaces                                             //
//===========================================================================//

#define STATIC_IID_IGSIFInterface\
    0xb253e1, 0x99d, 0x11d5, 0x9a, 0x47, 0x0, 0xe0, 0x29, 0x9, 0x5c, 0x67
DEFINE_GUIDSTRUCT("00B253E1-099D-11d5-9A47-00E029095C67", IID_IGSIFInterface);
#define IID_IGSIFInterface DEFINE_GUIDNAMED(IID_IGSIFInterface)


DEFINE_GUID(GSIFCLASS_INTERFACE, 
0x1a1dd540, 0xbd5, 0x11d5, 0x9a, 0x47, 0x0, 0xe0, 0x29, 0x9, 0x5c, 0x67);

/*****************************************************************************
 * IGSIFInterface
 *****************************************************************************
 */
DECLARE_INTERFACE_(IGSIFInterface,IUnknown)
{
    STDMETHOD_(NTSTATUS,ProcessCommand)
    (   THIS_
        IN    DWORD     dwCommand,
        IN    DWORD     dwCardId,
        IN    PVOID     pContext
    )   PURE;

    STDMETHOD_(NTSTATUS,RegisterCallbacks)
    (   THIS_
        IN    PFNDRIVERREQUEST fnDispatchRoutine,
        IN    PFNDRIVERISR     fnHardwareIsr,
        IN    PVOID            pContext
    )   PURE;
};


typedef IGSIFInterface *PGSIFINTERFACE;

#define IMP_IGSIFInterface\
      STDMETHODIMP_(NTSTATUS) ProcessCommand\
      (   IN    DWORD     dwCommand,\
          IN    DWORD     dwCardId,\
          IN    PVOID     pContext\
      );\
      STDMETHODIMP_(NTSTATUS) RegisterCallbacks\
      (   IN    PFNDRIVERREQUEST fnDispatchRoutine,\
          IN    PFNDRIVERISR     fnHardwareIsr,\
          IN    PVOID            pContext\
      );\

//===========================================================================//
//              GSIF    Global structures                                    //
//===========================================================================//
typedef struct tGSIFGETINTERFACE
{
        char            szSysName[GSIF_MAXSZ]; // Name of Hardware's sys driver.
        PGSIFINTERFACE  pInterface;            // pointer to your driver GSIF
                                               // interface.
        int             nNumOfCards;           // # of cards this interface is
                                               // controlling.
        char            szReserved1[GSIF_MAXSZ];
        DWORD           dwBaseCardId;          // This is set by NStation, for
                                               // Eg. dwBaseCardId = 0x1000
                                               // Card Id 1: 0x1000
                                               // Card Id 2: 0x1001
                                               // .
                                               // .
                                               // Card Id n: 0x100(n-1)
        DWORD           dwReserved2;           // system reserved.
} GSIF_GETINTERFACE, *PGSIF_GETINTERFACE;



typedef NTSTATUS (__stdcall *PFNGETINTERFACECALLBACK)(PGSIF_GETINTERFACE, PVOID);

#endif   // ifndef REMOVE_DDKDEFINITIONS

typedef struct HWBUFFERINFO
{
        DWORD           dwControl;             // system control.
        DWORD           dwNumOfSamples;        // # sammles your HW is set to
                                               // interrupt.
        DWORD           dwCurSampleRate;       // Current GSIF Sampling Rate.
        DWORD           dwReserved3;           // system reserved.
}HWBUFFERINFO, * PHWBUFFERINFO;
#endif

