
#ifndef INCLUDES_INCLUDED
#define INCLUDES_INCLUDED

// ............................................................................

#ifndef _WIN32_WINNT
    #define _WIN32_WINNT            0x501
#endif
#define _CRT_SECURE_NO_WARNINGS     1 // disable megalomania warnings

#pragma warning(push, 1)

#include <windows.h>
#include <commctrl.h>
#include <gdiplus.h>
#include <stdio.h>
#include <math.h>

#include "interface/kxapi.h"

#undef small
#undef min
#undef max

#pragma warning(pop)

// enable extra warnings:
#pragma warning(default: 4191) // unsafe conversion from 'type of expression' to 'type required'
#pragma warning(default: 4242) // (another) conversion from 'type1' to 'type2', possible loss of data
#pragma warning(default: 4254) // (another) conversion from 'type1' to 'type2', possible loss of data
#pragma warning(default: 4263) // member function does not override any base class virtual member function
#pragma warning(default: 4264) // no override available for virtual member function from base 'class'; function is hidden
#pragma warning(default: 4265) // class has virtual functions, but destructor is not virtual
#pragma warning(default: 4266) // no override available for virtual member function from base 'type'; function is hidden
#pragma warning(default: 4296) // expression is always false
#pragma warning(default: 4431) // missing type specifier - int assumed
#pragma warning(default: 4640) // construction of local static object is not thread-safe

// ............................................................................

#endif // INCLUDES_INCLUDED
