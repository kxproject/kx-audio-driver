//**@@@*@@@****************************************************
//
// Microsoft Windows
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//**@@@*@@@****************************************************

//
// FileName:    kssample.h
//
// Abstract:    
//      All of the necessary includes in one place
//

#pragma once
#ifndef KS__COMMONKS_H
#define KS__COMMONKS_H

// #define _WIN32_WINNT 0x0501
// #define WINVER 0x0501

#include <windows.h>
#include <mmsystem.h>
#include <ks.h>
#include <ksmedia.h>
#include <tchar.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <setupapi.h>
#include <devioctl.h>


#define SAFE_DELETE(x) delete x; x = NULL;

#include "kscommon.h"

#endif // KS__COMMONKS_H
