/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//   SFDEVMAN.H                                                            //
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
//   Revision:     1.01  08/04/97                                          //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef __SFDEVMAN_H
#define __SFDEVMAN_H

#include <windows.h>
#include <winbase.h>
 
/////////////// GLOBAL MACRO AND EXTERN DEFINITION ///////////////////////////
      
/*  
    The basic type define of a SoundFont Device Handle, or the type of the value
    returned when a SoundFont Device is opened which uniquely identifies that
    device
*/

typedef DWORD_PTR SFDEVHANDLE; 

/*
    The basic type define of a SoundFont Device Entry Function, or the message 
    function. This message function will receive all necessary messages from 
    the application who wishes to access its API.  
*/
 
typedef LRESULT (WINAPI *PFNSFDEVMANAGER) (SFDEVHANDLE, UINT, LPARAM, LPARAM);

#include "sfdevdta.h"

#endif      /* __SFDEVMAN_H */
