
// kX DSP Resource Meter demo library
// Copyright (c) Max Mikhailov, 2009

// Permission to use, copy, modify, distribute, and sell this software
// for any purpose is hereby granted without fee, provided that the above
// copyright notice appears in all copies and that both that copyright
// notice and this permission notice appear in supporting documentation.
// The author makes no representations about the suitability of this
// software for any purpose. It is provided "as is" without express or
// implied warranty.

// helpers.h
// helper utilities

#ifndef HELPERS_INCLUDED
#define HELPERS_INCLUDED

#include "includes.h"
#include "kali/dbgutils.h"

// ............................................................................

template <typename T> T min(T a, T b) {return a < b ? a : b;}
template <typename T> T max(T a, T b) {return a > b ? a : b;}

// ............................................................................

struct UsingGdiPlus
{
    UsingGdiPlus() : token(0)
    {
        Gdiplus::GdiplusStartupInput input;
        Gdiplus::GdiplusStartup(&token, &input, 0);
    }

    ~UsingGdiPlus()
    {
        Gdiplus::GdiplusShutdown(token);
    }

private:
    ULONG_PTR token;
};

// ............................................................................

struct Timer
{
    template <typename T, void (T::*F) (int)>
    void start(T* ptr, unsigned time)
    {
        stop();
        window  = ptr->handle;
        handler = ptr;
        ::SetTimer(window, (UINT_PTR) this, time, thunk<T, F>);
    }

    void stop()
    {
        if (window)
            ::KillTimer(window, (UINT_PTR) this);
        window  = 0;
        handler = 0;
        counter = 0;
    }

    Timer() : window(0), handler(0) {}
    ~Timer() {stop();}

private:
    HWND  window;
    void* handler;
    int   counter;

    Timer(const Timer&);
    Timer& operator = (const Timer&);

    template <typename T, void (T::*F) (int)>
    static VOID CALLBACK thunk(HWND, UINT, UINT_PTR ptr, DWORD)
    {
        Timer* timer = (Timer*) ptr;
        if (timer && timer->handler)
            (((T*) timer->handler)->*F)(++timer->counter);
    }
};

// ............................................................................

struct Tooltips
{
    void attach(int index, int x, int y, int w, int h)
    {
        TOOLINFO ti;
        RECT rect   = {x, y, x + w, y + h};
        ti.cbSize   = sizeof(TOOLINFO);
        ti.uFlags   = TTF_SUBCLASS;
        ti.hwnd     = parent;
        ti.uId      = index + 1;
        ti.rect     = rect;
        ti.hinst    = module;
        ti.lpszText = 0;
        ti.lParam   = 0;

        if (!::SendMessage(handle, TTM_ADDTOOL, 0, (LPARAM) &ti))
            trace("%s: TTM_ADDTOOL failed [%i]\n", FUNCTION_, ::GetLastError());
    }

    void update(int index, const char* text)
    {
        TOOLINFO ti;
        ti.cbSize   = sizeof(TOOLINFO);
        ti.uFlags   = 0;
        ti.hwnd     = parent;
        ti.uId      = index + 1;
        ti.rect     = RECT();
        ti.hinst    = module;
        ti.lpszText = (char*) (text);
        ti.lParam   = 0;

        if (!::SendMessage(handle, TTM_UPDATETIPTEXT, 0, (LPARAM) &ti))
            trace("%s: TTM_UPDATETIPTEXT failed [%i]\n", FUNCTION_, ::GetLastError());
    }

    void create(HMODULE module_, HWND parent_)
    {
        module = module_;
        parent = parent_;
        handle = ctor(module, parent);

        // just to enable multiline tooltips w/o responding to TTN_GETDISPINFO:
        ::SendMessage(handle, TTM_SETMAXTIPWIDTH, 0, 3333);
    }

    operator bool() const {return !!handle;}

    Tooltips() : module(0), handle(0) {}
    ~Tooltips() {destroy();}

private:
    HMODULE module;
    HWND    parent;
    HWND    handle;

    Tooltips(const Tooltips&);
    Tooltips& operator = (const Tooltips&);
    template <typename T> operator T () const;

    void destroy()
    {
        ::DestroyWindow(handle);
        handle = 0;
    }

    static HWND ctor(HMODULE module, HWND parent)
    {
        HWND handle = ::CreateWindowEx(WS_EX_TOPMOST,
            TOOLTIPS_CLASS, 0, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            parent, 0, module, 0);

        if (!handle)
            trace("%s: CreateWindowEx failed [%i]\n", FUNCTION_, ::GetLastError());

        return handle;
    }
};

// ............................................................................

struct Settings
{
    explicit Settings(const char* key)
        : handle(ctor(HKEY_CURRENT_USER, key)) {}

    Settings(const Settings& root, const char* key)
        : handle(ctor(root.handle, key)) {}

    Settings(const Settings& root, int index)
        : handle(ctor(root, index)) {}

    ~Settings() {::RegCloseKey(handle);}

    void set(const char* name, int value)
	{
		::RegSetValueEx(handle, name, 0, REG_DWORD,
            (BYTE*) &value, sizeof(value));
	}

    int get(const char* name, int default_) const
	{
		DWORD size = sizeof(default_);
		::RegQueryValueEx(handle, name,
            0, 0, (BYTE*) &default_, &size);
		return default_;
	}

    const char* subKey(char (&key)[MAX_PATH], int index) const
    {
        FILETIME unused;
        DWORD size = sizeof(key);
        ::RegEnumKeyEx(handle, index,
            key, &size, 0, 0, 0, &unused);
        return key;
    }

    operator bool() const {return !!handle;}

private:
    typedef ::HKEY Handle;
    Handle handle;

    static Handle ctor(Handle root, const char* key)
    {
        Handle handle = 0;
        REGSAM access = KEY_READ | KEY_WRITE
            | KEY_CREATE_SUB_KEY | KEY_ENUMERATE_SUB_KEYS;
        if (::RegCreateKeyEx(root, key, 0,
            0, 0, access, NULL, &handle, NULL))
                trace("%s: - cannot open %x\\%s\n",
                    FUNCTION_, root, key);
        return handle;
	}

    static Handle ctor(const Settings& root, int index)
    {
        char key[MAX_PATH] = "";
        root.subKey(key, index);
        return *key ? ctor(root.handle, key) : 0;
	}

private:
    Settings(const Settings&);
    Settings& operator = (const Settings&);
    template <typename T> operator T () const;
};

// ............................................................................
// never try this at home

template <typename E, E N>
struct EnumNames
{
    const char* operator [] (int i) const {return names(i);}

    EnumNames()
    {
        if (!names(0))
            _<E(0)>(sizeof(FUNCSIG_)
                - sizeof("EnumNames()"));
    }

private:

    enum {Size = N};

    static const char*& names(int i)
    {
        static const char* aux[Size] = {0};
        return aux[i];
    }

    template <E I> inline_ static void _(int offset)
    {
        static char aux[] = FUNCSIG_;
        names(I) = aux + offset;
        _<E(I + 1)>(offset);
        aux[sizeof(aux) - sizeof(">(int)")] = 0;
    }

    template <> static void _ <Size> (int) {}
};

// ............................................................................

#endif // ~ HELPERS_INCLUDED
