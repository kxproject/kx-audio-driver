
// Spectrum Analyzer demo library
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

#include "sa.h"
#include "os.window.h"

// ............................................................................

struct InfoTool
{
    typedef HWND Handle;

    void update(const char* left_, const char* right_)
    {
        ::SetWindowText(left, left_); 
        ::SetWindowText(right, right_);
    }

    void toggle(HMODULE module, Handle parent, const char* name)
    {
        if (::IsWindow(handle))
        {
            ::DestroyWindow(handle);
            return;
        }

        using namespace sa::config;

        handle = os::Window::utility(module, parent, name);
        left   = widget(handle, infoLeftRect);
        right  = widget(handle, infoRightRect);
        ::MoveWindow(handle, x, y, infoRect.Width, infoRect.Height, 0);
        ::ShowWindow(handle, SW_SHOW);
        ::SetFocus(parent);
    }

    void move(int dx, int dy)
    {
        x += dx;
        y += dy;
        if (::IsWindow(handle))
        {
            RECT r;
            ::GetWindowRect(handle, &r);
            ::MoveWindow(handle, 
                x = r.left + dx, y = r.top  + dy, 
                r.right - r.left, r.bottom - r.top, TRUE);
        }  
    }

    explicit InfoTool(int* xy) : 
        x(xy[0]), y(xy[1]), handle(0), left(0), right(0) {}

private:
    int&   x;
    int&   y;
    Handle handle;
    Handle left;
    Handle right;

private:
    InfoTool(const InfoTool&);
    InfoTool& operator = (const InfoTool&);

    static Handle widget(Handle parent, const sa::config::Rect& r)
    {
        Handle handle = ::CreateWindowEx(0, WC_STATIC, 0, 
            WS_VISIBLE | WS_CHILD | SS_NOPREFIX, 
            r.X, r.Y, r.Width, r.Height, parent, 0, 0, 0);
        ::SendMessage(handle, WM_SETFONT, (WPARAM) 
            GetStockObject(DEFAULT_GUI_FONT), TRUE);
        return handle;
    }
};

// ............................................................................

template <typename Plugin>
struct PluginWindow : iKXPluginGUI
{
    void init_gui()  {}
    void hide_gui()  {::ShowWindow(handle, SW_HIDE);}
    void close_gui() {::DestroyWindow(handle);}

    void show_gui()  
    {
        ::ShowWindow(handle, 
            parameter[maximized] ? SW_MAXIMIZE : SW_SHOW);
    }

    uintptr_t extended_gui(int command, uintptr_t p1, uintptr_t p2)
    {
        switch (command)
        {
        case PLUGINGUI_FOREGROUND:
            if (::IsIconic(handle))
                ::ShowWindow(handle, SW_RESTORE);
            ::SetForegroundWindow(handle);
            break;
        case PLUGINGUI_SET_ICON: 
            ::SendMessage(handle, WM_SETICON, p2, p1);
            break;
        }

        return 0;
    }

    PluginWindow(Plugin* plugin, int* parameter) : 
        parameter(parameter), 
        plugin(plugin), 
        handle(0), 
        view(0),
        info(parameter + ix)
    {
        os::Window::main(plugin->instance, this, plugin->name);
        ::MoveWindow(handle, parameter[x], parameter[y], 
            parameter[w], parameter[h], 0);
        view = new View(handle, plugin, &info, parameter + saOptionsOffset);
    }

    ~PluginWindow() 
    {
        plugin->cp = 0;
        delete view;
    }

    enum Parameters
    {
        version,
        x, y, w, h, 
        maximized,
        topmost,
        ix, iy,

        saOptionsOffset,
        ParametersCount = saOptionsOffset
            + sa::options::OptionsCount
    };

    static void defaults(int* parameter)
    {
        parameter[version]   = sa::options::version;
        parameter[x]         = sa::config::windowRect.X; 
        parameter[y]         = sa::config::windowRect.Y;
        parameter[w]         = sa::config::windowRect.Width;
        parameter[h]         = sa::config::windowRect.Height;
        parameter[maximized] = 0;
        parameter[topmost]   = 0;
        parameter[ix]        = sa::config::infoRect.X;
        parameter[iy]        = sa::config::infoRect.Y;

        sa::options::defaults(parameter + saOptionsOffset);
    }

    void resized(int width, int height, bool normal)
    {
        if (normal)
        {
            parameter[w] = width;
            parameter[h] = height;
        }

        if (view)
        {
            parameter[maximized] = ::IsZoomed(handle);
            view->resized();
        }
    }

    void moved(int left, int top, bool normal)
    {
        if (normal)
        {
            info.move(left - parameter[x], top - parameter[y]);
            parameter[x] = left;
            parameter[y] = top;
        }
    }

    void repositioned() 
    {
        info.move(0, 0);
    }

    void keyDown(int key, int flags)
    {
        if ((key == 'A') && (0 > ::GetKeyState(VK_CONTROL)))
        {
            parameter[topmost] = !parameter[topmost];
            ::SetWindowPos(handle, parameter[topmost] 
                ? HWND_TOPMOST : HWND_NOTOPMOST,
                0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        }

        if (view)
            view->keyDown(key, flags);
    }

public:
    typedef sa::View<Plugin, InfoTool> View;
    int*         parameter;
    Plugin*      plugin;
    HWND         handle;
    View*        view;
    InfoTool     info;
    UsingGdiPlus ugp;
};

// ............................................................................

struct Plugin : iKXPlugin
{
    static int entry(int command, uintptr_t* ret)										
    {	
	    switch (command)																		
	    {																					
	    case KXPLUGIN_GET_NAME:
            strncpy((char*) ret, sa::dsp::name, KX_MAX_STRING);						
  		    return 0;																
	    case KXPLUGIN_GET_GUID:					
            strncpy((char*) ret, sa::dsp::guid, KX_MAX_STRING);						
  		    return 0;																		
	    case KXPLUGIN_INSTANTIATE:
		    *ret = (uintptr_t) new Plugin;
		    return 0;																		
	    default:																			
		    *ret = NULL;																	
		    return 1;																		
	    }	
    }

    int request_microcode()
    {
        using namespace sa;

        info      = dsp::info;                              	
        code      = dsp::code;                              	
        info_size = sizeof(dsp::info);                 	
        code_size = sizeof(dsp::code);                 	
        itramsize = dsp::itramsize;                    	
        xtramsize = dsp::itramsize;                     	
        strncpy(name, dsp::name, sizeof(name));
        return 0;
    }

    const char* get_plugin_description(int id)
    {
        using namespace sa;

	    switch (id)																		
	    {																					
	        case IKX_PLUGIN_NAME:      return dsp::name;																		
	        case IKX_PLUGIN_GUID:      return dsp::guid;	
	        case IKX_PLUGIN_ENGINE:    return dsp::engine;	
	        case IKX_PLUGIN_CREATED:   return dsp::created;	
	        case IKX_PLUGIN_COMMENT:   return dsp::comment;
            case IKX_PLUGIN_COPYRIGHT: return dsp::copyright;
	    }

        return 0;
    }

    int get_user_interface() 
    {
        return IKXPLUGIN_OWNER;
    }

    iKXPluginGUI* create_cp(kDialog*, kFile*) 
    {
        return new Window(this, parameter);
    }

    int get_param(int index, kxparam_t* value)
    {
        *value = parameter[index];
        return 0;
    }

    int set_all_params(kxparam_t* value)
    {
        if (value[0] != sa::options::version)
            return set_defaults();

        for (int i = 0; i < ParametersCount; i++)
            parameter[i] = (int)value[i];

        return 0;
    }

    int get_param_count()    
    {
        return ParametersCount;
    }

    int set_defaults() 
    {
        Window::defaults(parameter);
        return 0;
    }

    Plugin()                 
    {
        set_defaults();
    }

private:
    typedef PluginWindow<Plugin> Window;
    enum {ParametersCount = Window::ParametersCount};
    int parameter[ParametersCount];
};

// ............................................................................

// extern "C" __declspec(dllexport) 
// int publish_plugins(int command, int param, dword* ret)
declare_publish_plugins
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

	return 1;
}

// ............................................................................
