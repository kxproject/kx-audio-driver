#pragma once

BOOL RegisterCMHotKey( HINSTANCE hInstance );
BOOL UnregisterCMHotKey( HINSTANCE hInstance );
void GetHotKeyString( CM_HOTKEY &cmHotKey, LPTSTR psz );

#define HOTM_QUERYEATCOMMAND			WM_USER + 12700
#define HOTM_SETHOTKEY					WM_USER + 12701
#define HOTM_GETHOTKEY					WM_USER + 12702
