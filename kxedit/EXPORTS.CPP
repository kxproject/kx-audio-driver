#include "precomp.h"
#include "misc.h"
#include "hkeyctrl.h"
#include "cedit.h"

#ifdef _ACTIVEX
#include "initguid.h"
#include "editx.h"
#include "editx_i.c"
#include "ceditx.h"
#include "rangex.h"
#include "posx.h"
#include "langx.h"
#include "globalsx.h"
#include "hotkeyx.h"
#include "rectx.h"


CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
    OBJECT_ENTRY(CLSID_CodeMax, CEditX)
    OBJECT_ENTRY(CLSID_Range, CRangeX)
    OBJECT_ENTRY(CLSID_Position, CPositionX)
    OBJECT_ENTRY(CLSID_Language, CLanguageX)
    OBJECT_ENTRY(CLSID_Globals, CGlobalsX)
    OBJECT_ENTRY(CLSID_HotKey, CHotKeyX)
    OBJECT_ENTRY(CLSID_Rect, CRectX)
END_OBJECT_MAP()
#endif

HINSTANCE hInstance;

extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /* lpReserved */)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        ::hInstance = hInstance;
        RegisterLineToolTip();
        InitCommonControls();
        RegisterCMHotKey( hInstance );

#ifdef _ACTIVEX
        CMRegisterControl( CM_VERSION );
        _Module.Init(ObjectMap, hInstance);
        DisableThreadLibraryCalls(hInstance);
#endif
    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {
        UnregisterLineToolTip();
        UnregisterCMHotKey( hInstance );
#ifdef _ACTIVEX
        _Module.Term();
        CMUnregisterControl();
#endif
    }
    return 1;   // ok
}

extern "C" CME_CODE __cdecl CMRegisterControl( DWORD dwVersion )
{
    if ( dwVersion == CM_VERSION )
    {
        CEdit::g_dwVersion = dwVersion;
        return ( CEdit::Register() ? CME_SUCCESS : CME_FAILURE );
    }
    else
    {
        return CME_BADARGUMENT;
    }
}

extern "C" CME_CODE __cdecl CMUnregisterControl()
{
    CEdit::Unregister();
    return CME_SUCCESS;
}

extern "C" void __cdecl CMSetFindReplaceMRUList( LPCTSTR pszMRUList, BOOL bFind )
{
    if ( bFind )
    {
        if ( CEdit::g_FindReplaceData.m_pszFindMRUList )
        {
            free( CEdit::g_FindReplaceData.m_pszFindMRUList );
            CEdit::g_FindReplaceData.m_pszFindMRUList = NULL;
        }
    }
    else
    {
        if ( CEdit::g_FindReplaceData.m_pszReplaceMRUList )
        {
            free( CEdit::g_FindReplaceData.m_pszReplaceMRUList );
            CEdit::g_FindReplaceData.m_pszReplaceMRUList = NULL;
        }
    }

    if ( pszMRUList )
    {
        int nLen = _tcslen( pszMRUList );
        int cb = ( nLen + 1 ) * sizeof( TCHAR );
        cb = min( CM_FIND_REPLACE_MRU_BUFF_SIZE, cb );
        LPTSTR pszNewList = ( LPTSTR ) malloc( cb + 1 );
        if ( bFind )
        {
            CEdit::g_FindReplaceData.m_pszFindMRUList = pszNewList;
        }
        else
        {
            CEdit::g_FindReplaceData.m_pszReplaceMRUList = pszNewList;
        }
        _tcsncpy( pszNewList, pszMRUList, nLen );
        pszNewList[ nLen ] = _T('\0');
    }
}

extern "C" void __cdecl CMGetFindReplaceMRUList( LPTSTR pszMRUList, BOOL bFind )
{
    LPCTSTR psz = bFind ? CEdit::g_FindReplaceData.m_pszFindMRUList : 
                          CEdit::g_FindReplaceData.m_pszReplaceMRUList;
    if ( psz )
    {   _tcscpy( pszMRUList, psz );
    }
    else
    {
        *pszMRUList = _T('\0');
    }
}

extern "C" CME_CODE __cdecl CMRegisterLanguage( LPCTSTR pszName, CM_LANGUAGE *pLang )
{
    return CEdit::RegisterLanguage( pszName, pLang );
}

extern "C" CME_CODE __cdecl CMUnregisterLanguage( LPCTSTR pszName )
{
    return ( CEdit::UnregisterLanguage( pszName ) ? CME_SUCCESS : CME_FAILURE );
}

extern "C" CME_CODE __cdecl CMUnregisterAllLanguages()
{
    return CEdit::UnregisterAllLanguages();
}

extern "C" int __cdecl CMGetLanguageDef( LPCTSTR pszName, CM_LANGUAGE *pLang )
{
    return CEdit::GetLanguageDef( pszName, pLang );
}

extern "C" int __cdecl CMGetHotKeys( LPBYTE pBuff )
{
    return CEdit::GetHotKeys( pBuff );
}

extern "C" CME_CODE __cdecl CMSetHotKeys( const LPBYTE pBuff )
{
    BOOL bResult = CEdit::SetHotKeys( pBuff );
    #ifdef _ACTIVEX
    CEditX::ResetAccelTable();
    #endif
    return ( bResult ? CME_SUCCESS : CME_FAILURE );
}

extern "C" void __cdecl CMResetDefaultHotKeys()
{
    CEdit::SetDefaultHotKeys();
    #ifdef _ACTIVEX
    CEditX::ResetAccelTable();
    #endif
}

extern "C" int __cdecl CMGetHotKeysForCmd( WORD wCmd, CM_HOTKEY *pHotKeys )
{
    return CEdit::FindHotKeysForCommand( wCmd, pHotKeys );
}

extern "C" CME_CODE __cdecl CMRegisterHotKey( CM_HOTKEY *pHotKey, WORD wCmd )
{
    if ( pHotKey )
    {
        CEdit::RegisterHotKey( *pHotKey, wCmd );
        #ifdef _ACTIVEX
        CEditX::ResetAccelTable();
        #endif
        return CME_SUCCESS;
    }
    else
    {
        return CME_BADARGUMENT;
    }
}

extern "C" CME_CODE __cdecl CMUnregisterHotKey( CM_HOTKEY *pHotKey )
{
    if ( pHotKey )
    {
        return ( CEdit::UnregisterHotKey( *pHotKey ) ? CME_SUCCESS : CME_FAILURE );
        #ifdef _ACTIVEX
        CEditX::ResetAccelTable();
        #endif
    }
    else
    {
        return CME_BADARGUMENT;
    }
}

extern "C" CME_CODE __cdecl CMRegisterCommand( WORD wCmd, LPCTSTR pszName, LPCTSTR pszDesc )
{
    if ( ( wCmd >= CMD_USER_BASE ) && pszName && pszDesc )
    {
        return ( CEdit::RegisterCommand( wCmd, pszName, pszDesc ) ? CME_SUCCESS : CME_FAILURE );
    }
    else
    {
        return CME_BADARGUMENT;
    }
}

extern "C" CME_CODE __cdecl CMUnregisterCommand( WORD wCmd )
{
    if ( wCmd >= CMD_USER_BASE )
    {
        return ( CEdit::UnregisterCommand( wCmd ) ? CME_SUCCESS : CME_FAILURE );
    }
    else
    {
        return CME_BADARGUMENT;
    }
}

extern "C" void __cdecl CMGetCommandString( WORD wCmd, BOOL bDescription, LPTSTR pszBuff, int nBuffLen )
{
    CEdit::GetCommandString( wCmd, bDescription, pszBuff, nBuffLen );
}

extern "C" int __cdecl CMGetMacro( int nMacro, LPBYTE pMacroBuff )
{
    return CEdit::GetMacro( nMacro, pMacroBuff );
}

extern "C" int __cdecl CMSetMacro( int nMacro, const LPBYTE pMacroBuff )
{
    return CEdit::SetMacro( nMacro, pMacroBuff );
}

#ifdef _ACTIVEX
/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow(void)
{
    return (_Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib
    return _Module.RegisterServer(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
    _Module.UnregisterServer();
    return S_OK;
}

#endif