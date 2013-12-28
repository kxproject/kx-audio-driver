//***************************************************************************
// Driver:
//              HwSys.sys
//
// File:
//              HwSys.h
//
// Description:
//              Defines all the neccessary device extensions, structures and
//              prototype for the NT-style driver.
//
//***************************************************************************

#ifndef __HW_SYS__
#define __HW_SYS__

#define FILE_DEVICE_HWSYS       53335

#define STR_HW_NT_DEVICE_NAME    L"\\Device\\HWGSIF"
#define STR_HW_WIN32_DEVICE_NAME L"\\DosDevices\\HWGSIF"
#define STR_HW_WIN32_DEVICE       "\\\\.\\HWGSIF1"        // Used by user-mode dll.

#define IOCTL_HWSYS_ENABLEINTERFACE     CTL_CODE(FILE_DEVICE_HWSYS, 2048,          \
                                                 METHOD_IN_DIRECT, FILE_ANY_ACCESS)
#define IOCTL_HWSYS_DISABLEINTERFACE     CTL_CODE(FILE_DEVICE_HWSYS, 2049,          \
                                                 METHOD_IN_DIRECT, FILE_ANY_ACCESS)
#define IOCTL_HWSYS_REGISTERINTERFACE    CTL_CODE(FILE_DEVICE_HWSYS, 2050,          \
                                                 METHOD_IN_DIRECT, FILE_ANY_ACCESS)
#endif
