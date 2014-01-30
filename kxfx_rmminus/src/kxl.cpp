
// kX DSP Resource Meter demo library
// Copyright (c) Max Mikhailov, 2009

// Permission to use, copy, modify, distribute, and sell this software
// for any purpose is hereby granted without fee, provided that the above
// copyright notice appears in all copies and that both that copyright
// notice and this permission notice appear in supporting documentation.
// The author makes no representations about the suitability of this
// software for any purpose. It is provided "as is" without express or
// implied warranty.

// kxl.cpp
// kX Plugin interface

#include "includes.h"
#include "helpers.h"
#include "rm.dsp.h"
#include "rm.display.h"

// ............................................................................

template <typename Plugin>
struct PluginBox : iKXDSPWindow
{
    PluginBox(kDialog* parent, kWindow* window, Plugin* plugin, kFile* file) :
        iKXDSPWindow(parent, window, plugin, file),
        counter(plugin->ikx),
        display(counter)
    {}

    int draw(kDraw* dc, int x, int y, int, KXAPI_CONST char*, int)
    {
        bool dragging = x && y;
        if (!dragging)
            update(dc->get_dc());
        display.draw(dc->get_dc(), x, y, dragging);
        return 0;
    }

    void update(HDC dc)
    {
        HWND handle = ::WindowFromDC(dc);
        if (!handle)
            return;

        if (!tooltips)
            display.createTooltips(tooltips, plugin->instance, handle);
        counter.update(tooltips);

        // workaround for kX since v.38.
        // the "min/max" button is hardcoded as a separate control
        // thus messing up the very idea of the iKXDSPWindow.
        // we have to hide it:
        handle = ::FindWindowEx(handle, 0, WC_BUTTON, 0);
        if (handle && ::IsWindowVisible(handle))
            ::ShowWindow(handle, SW_HIDE);
    }

    int get_window_rect(char*, int, kSize* size)
    {
        size->cx = display.width() + 1;
        size->cy = display.height() + 1;
        return 0;
    }

    int configure()                                     {return  0;}
    int set_connector_size(int)                         {return  0;}
    int get_connection_rect(int, int, int, kRect*)      {return ~0;}
    int find_gpr(KXAPI_CONST kPoint*, int*, int, char*) {return ~0;}

private:
    rm::Counter counter;
    rm::Display display;
    Tooltips    tooltips;
};

// ............................................................................

struct Plugin : iKXPlugin
{
    static int entry(int command, uintptr_t* ret)
    {
        switch (command)
        {
        case KXPLUGIN_GET_NAME:
            strncpy((char*) ret, rm::dsp::name, KX_MAX_STRING);
            return 0;
        case KXPLUGIN_GET_GUID:
            strncpy((char*) ret, rm::dsp::guid, KX_MAX_STRING);
            return 0;
        case KXPLUGIN_INSTANTIATE:
            *ret = (uintptr_t) new Plugin;
            return 0;
        default:
            *ret = NULL;
            return ~0;
        }
    }

    int request_microcode()
    {
        info      = 0;
        code      = 0;
        info_size = 0;
        code_size = 0;
        itramsize = 0;
        xtramsize = 0;
        strncpy(name, rm::dsp::name, sizeof(name));
        return 0;
    }

    KXAPI_CONST char* get_plugin_description(int id)
    {
        using namespace rm;

        switch (id)
        {
            case IKX_PLUGIN_NAME:      return (char*) dsp::name;
            case IKX_PLUGIN_GUID:      return (char*) dsp::guid;
            case IKX_PLUGIN_ENGINE:    return (char*) dsp::engine;
            case IKX_PLUGIN_CREATED:   return (char*) dsp::created;
            case IKX_PLUGIN_COMMENT:   return (char*) dsp::comment;
            case IKX_PLUGIN_COPYRIGHT: return (char*) dsp::copyright;
        }

        return 0;
    }

    iKXDSPWindow* create_dsp_wnd(kDialog* parent, kWindow* window, kFile* file)
    {
        return new PluginBox<Plugin>(parent, window, this, file);
    }

    ~Plugin()
    {
        delete iKXPlugin::dsp_wnd;
        dsp_wnd = 0;
    }
};

// ............................................................................

extern "C" __declspec(dllexport)
int publish_plugins(int command, int param, uintptr_t* ret)
{
    switch (command)
    {
        case KXPLUGIN_GET_COUNT:
            *ret = 1;
            return 0;

        case KXPLUGIN_GET_VERSION:
            *ret = KXPLUGIN_VERSION;
            return 0;

        case KXPLUGIN_GET_NAME:
        case KXPLUGIN_GET_GUID:
        case KXPLUGIN_INSTANTIATE:
            if (param == 0)
                return Plugin::entry(command, ret);
    }

    return ~0;
}

// ............................................................................
