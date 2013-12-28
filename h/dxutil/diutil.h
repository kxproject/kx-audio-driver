//-----------------------------------------------------------------------------
// File: DIUtil.h
//
// Desc: DirectInput support using action mapping
//
// Copyright (C) 1995-2000 Microsoft Corporation. All Rights Reserved.
//-----------------------------------------------------------------------------
#ifndef DIUTIL_H
#define DIUTIL_H
#include <dinput.h>




//-----------------------------------------------------------------------------
// Name: class CInputDeviceManager
// Desc: Input device manager using DX8 action mapping
//-----------------------------------------------------------------------------
class CInputDeviceManager
{
    HWND                 m_hWnd;
    TCHAR*               m_strUserName;

    LPDIRECTINPUT8       m_pDI;
    LPDIRECTINPUTDEVICE8 m_pdidDevices[20];
    DWORD                m_dwNumDevices;
    DIACTIONFORMAT       m_diaf;

public:
    // Device control
    HRESULT AddDevice( const DIDEVICEINSTANCE* pdidi, LPDIRECTINPUTDEVICE8 pdidDevice );
    HRESULT GetDevices( LPDIRECTINPUTDEVICE8** ppDevices, DWORD* pdwNumDevices );
    HRESULT ConfigureDevices( HWND hWnd, IUnknown* pSurface, VOID* pCallback, DWORD dwFlags );
    VOID UnacquireDevices();

    // Construction
    HRESULT SetActionFormat( DIACTIONFORMAT& diaf, BOOL bReenumerate );
    HRESULT Create( HWND hWnd, TCHAR* strUserName, DIACTIONFORMAT& diaf );

    CInputDeviceManager();
    ~CInputDeviceManager();
};

#endif


