
// Spectrum Analyzer demo library
// Copyright (c) Max Mikhailov, 2009 

// Permission to use, copy, modify, distribute, and sell this software
// for any purpose is hereby granted without fee, provided that the above
// copyright notice appears in all copies and that both that copyright
// notice and this permission notice appear in supporting documentation.
// The author makes no representations about the suitability of this
// software for any purpose. It is provided "as is" without express or 
// implied warranty.

// os.window.h 
// OS window maintenance 

#ifndef OS_WINDOW_INCLUDED
#define OS_WINDOW_INCLUDED

#include "includes.h"

// ............................................................................

namespace os {

// ............................................................................

struct Window
{
    typedef HWND Handle;

    template <typename T>
    static Handle main(HMODULE module, T* window, const char* name)
    {
        preinit(module, WindowProc<T>::thunk, name);
        Handle handle = ::CreateWindowEx(0, name, name, 
            WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 
            CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, module, window);
        if (!handle)
            trace("%s: CreateWindowEx failed [%i]\n", FUNCTION_, ::GetLastError());

        return handle;
    }

    static Handle utility(HMODULE module, Handle parent, const char* name)
    {
        preinit(module, ::DefWindowProc, name);
        Handle handle = ::CreateWindowEx(WS_EX_TOOLWINDOW, name, name, 
            WS_POPUPWINDOW | WS_CAPTION, CW_USEDEFAULT, CW_USEDEFAULT, 
            CW_USEDEFAULT, CW_USEDEFAULT, parent, 0, module, 0);
        if (!handle)
            trace("%s: CreateWindowEx failed [%i]\n", FUNCTION_, ::GetLastError());

        return handle;
    }

private:

    static bool preinit(HMODULE module, WNDPROC function, const char* name)
    {
        WNDCLASSEX wcx;
        wcx.cbSize		  = sizeof(wcx);
        wcx.style	      = CS_DBLCLKS | CS_OWNDC;
        wcx.hInstance     = module;
        wcx.lpfnWndProc   = function;
        wcx.lpszMenuName  = name;
        wcx.lpszClassName = name;
        wcx.cbClsExtra    = 0;
        wcx.cbWndExtra    = 0;
        wcx.hIcon		  = 0;
        wcx.hIconSm		  = 0;
        wcx.hCursor       = ::LoadCursor(0, IDC_ARROW);
        wcx.hbrBackground = ::GetSysColorBrush(COLOR_3DFACE);
    	
        return !!::RegisterClassEx(&wcx);
    }

    #pragma warning(push)
    #pragma warning(disable: 4706)

    template <typename T>
    struct WindowProc
    {
        static LRESULT CALLBACK thunk(Handle handle, UINT msg, WPARAM wparam, LPARAM lparam)
        {
            T* window;

            if (msg == WM_NCCREATE 
                && lparam
                && (window = (T*) ((CREATESTRUCT*) lparam)->lpCreateParams))
            {
                window->handle = handle;
			    ::SetWindowLongPtr(handle, GWLP_USERDATA, (LONG) (LONG_PTR) window);
            }
            else
                window = (T*) (LONG_PTR) ::GetWindowLongPtr(handle, GWLP_USERDATA);

            if (window)
                return dispatch(window, msg, wparam, lparam);

            return ::DefWindowProc(handle, msg, wparam, lparam);
        }
        
    private:

        static LRESULT dispatch(T* window, UINT msg, WPARAM wparam, LPARAM lparam)
        {
            const HWND handle = window->handle;

            switch (msg)
            {
            case WM_NCDESTROY:
                ::SetWindowLongPtr(handle, GWLP_USERDATA, 0);
                delete window;
                return 0;

            case WM_MOVE:
            case WM_SIZE:
            {
                RECT r;
                ::GetWindowRect(handle, &r);
                bool normal = !::IsIconic(handle) && !::IsZoomed(handle);
                (msg == WM_MOVE) ? window->moved(r.left, r.top, normal)
                    : window->resized(r.right - r.left, r.bottom - r.top, normal);
                return 0;
            }

            case WM_WINDOWPOSCHANGING:
                window->repositioned();
                return 0;

            case WM_LBUTTONDBLCLK:
                if (window->view)
                    window->view->mouseDoubleClick();
                return 0;

            case WM_KEYDOWN:
                window->keyDown(int(wparam), int(lparam));
                return 0;

            case WM_ERASEBKGND:
                return 0;

            case WM_PAINT:   
                if (window->view)
                    window->view->draw();
                ValidateRect(handle, 0);
                return 0;
            }

            return ::DefWindowProc(handle, msg, wparam, lparam);
        }
    };

    #pragma warning(pop)
};

// ............................................................................

} // ~ namespace os

// ............................................................................

#endif // ~ OS_WINDOW_INCLUDED
