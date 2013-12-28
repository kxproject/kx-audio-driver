#ifndef __PROTO_H__
#define __PROTO_H__


#ifdef __cplusplus
extern "C"
{
#endif

//
// kernel.c
//
BOOL LoadKernelFilter(LPCTSTR driverName, LPCTSTR driverImage, HANDLE * ph);
BOOL UnloadKernelFilter(HANDLE hDevice, LPCTSTR driverName, BOOL bRemoveService);

#ifdef __cplusplus
}
#endif

#endif //__PROTO_H__
