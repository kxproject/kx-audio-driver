// File Name:
//                nssys.h
//
// Contents:
//                Constants, structures, and function
//                declarations specific to this driver.
//
#ifndef __NSSYS_H__
#define __NSSYS_H__

#define STR_NS_NT_DEVICE_NAME    L"\\Device\\GSGSIF"
#define STR_NS_WIN32_DEVICE_NAME L"\\DosDevices\\GSGSIF"
#define STR_NS_WIN32_DEVICE       "\\\\.\\GSGSIF"        // Used by user-mode dll.

typedef enum {
	eTestLoopBack = 0,
	eTestSineWave = 1,
	eTestWav      = 2,
} TESTMODE;


//
// Commands send down from NMSHWDLL.
//
#define NMSHWDLL_CMD_VERIFY			0x100
#define NMSHWDLL_CMD_GETNUMADAP		0x101
#define NMSHWDLL_CMD_GETHWCAPS		0x102
#define NMSHWDLL_CMD_SETHARDWARE	0x103
#define NMSHWDLL_CMD_GETSTATUS          0x104
#define NMSHWDLL_CMD_GETVERSION         0x105
#define NMSHWDLL_CMD_GETHWBUFFINFO      0x106
#define NMSHWDLL_CMD_GETMIDICAPS        0x107
#define NMSHWDLL_CMD_STARTAUDIOTEST     0x108
#define NMSHWDLL_CMD_STOPAUDIOTEST      0x109
#define NMSHWDLL_CMD_GETRUNNINGINFO     0x110
#define NMSHWDLL_CMD_GETPORTNAME        0x111
#define NMSHWDLL_CMD_STRESSLEVEL        0x112
#define NMSHWDLL_CMD_GETCHANNAMES       0x113

//**************************************************************
// UserMode Consts:
//                IOCTLs
// Description:
//                Provide a Specitalized IOCTL Code Set
//                to the WaveStreamWaveTable UserMode driver
//
// Access:
//                Allocated from NON-PAGED POOL
//                Available at any IRQL
//**************************************************************
#define FILE_DEVICE_NSSYS       53434
#define IOCTL_NSSYS_INIT              CTL_CODE(FILE_DEVICE_NSSYS, 2048,          \
                                         METHOD_IN_DIRECT, FILE_ANY_ACCESS)
#define IOCTL_NSSYS_SENDFILTERCOMMAND CTL_CODE(FILE_DEVICE_NSSYS, 2049,          \
                                         METHOD_NEITHER, FILE_ANY_ACCESS)
#define IOCTL_NSSYS_TEST              CTL_CODE(FILE_DEVICE_NSSYS, 2050,          \
                                         METHOD_IN_DIRECT, FILE_ANY_ACCESS)
#define IOCTL_NSSYS_GETINTERFACE      CTL_CODE(FILE_DEVICE_NSSYS, 2051,          \
                                         METHOD_IN_DIRECT, FILE_ANY_ACCESS)
#define IOCTL_NSSYS_HARDWARECMD		  CTL_CODE(FILE_DEVICE_NSSYS, 2052,          \
                                         METHOD_NEITHER, FILE_ANY_ACCESS)
#define IOCTL_NSSYS_CLEANUP               CTL_CODE(FILE_DEVICE_NSSYS, 2053,          \
                                         METHOD_NEITHER, FILE_ANY_ACCESS)
#define IOCTL_NSSYS_EWCTLCMD	      CTL_CODE(FILE_DEVICE_NSSYS, 2054,          \
                                         METHOD_NEITHER, FILE_ANY_ACCESS)

#endif // __NSSYS_DRIVER__

