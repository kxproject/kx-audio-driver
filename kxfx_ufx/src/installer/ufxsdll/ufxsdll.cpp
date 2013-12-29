
#include "ufxsdll.h"
#include "kxapi.h"
#include "vers.h"

// #pragma comment(lib, "kxapi.lib")

#include "kali/string.h"
#include "kali/settings.h"

// ............................................................................

//#if _DEBUG
//    void trace(const char* format, ...)
//    {
//    	TCHAR text[512];
//    	va_list arglist;
//    	va_start(arglist, format);
//    	_vsnprintf(text, sizeof(text)/sizeof(TCHAR), format, arglist);
//        va_end(arglist);
//        ::OutputDebugString(text);
//    }
//#else
//    #define trace(...) __noop
//#endif

// ............................................................................

struct kX
{
    operator bool ()    const {return !!p;}
    iKX* operator -> () const {return p;}
    const char* path()  const {return path_;}

    int device(int index) const
    {
        if (!p)
            return -1;
        p->close();
        return p->init(index);
    }

    kX() : p(0)
    {
        initPath();
        if (!::LoadLibrary("kxapi.dll"))
            return;
        p = new iKX;
        p->init();
    }

    ~kX() {close();}

private:
    iKX* p;
    kali::string path_;

    void close() const
    {
        if (!p)
            return;
        p->close();
        delete p;
    }

    void initPath()
    {
        path_ = Settings(HKEY_CURRENT_USER, "Software\\kX\\Plugins")
            .get("ceffc302-ea28-44df-873f-d3df1ba31736", "");
        if (char* e = strrchr(path_, '\\'))
            *e = 0;
        ::SetDllDirectory(path_);
    }

} const kx;

static int start_mixer_on_exit = 0;

// ............................................................................

uintptr_t apiVer()
{   
    // this is a big hack. there's no way to find out kx's pluginapi version through kxapi
    // so we check the version of installed kxfxlib.kxl instead

    uintptr_t ver = KXPLUGIN_VERSION; // let's assume it's OK if some error occurs

    typedef int (*F)(int, int, uintptr_t*);
    HMODULE module;
    F publish_plugins;

    if ((module = LoadLibrary("kxfxlib.kxl")) &&
        (publish_plugins = (F) ::GetProcAddress(module, "publish_plugins")))
    {
        publish_plugins(KXPLUGIN_GET_VERSION, 0, &ver);
        ::FreeLibrary(module);
    }

    return ver;
}

int checkVer()
{   
    if (!kx)
        return UFXSETUP_ERROR_NO_KX;

    if (apiVer() != KXPLUGIN_VERSION)
        return UFXSETUP_ERROR_OLD_KX;

    return 0;
}

// ............................................................................

int pre()
{
    // close kxmixer
    HWND handle = FindWindow(NULL, "KXDefWindow");
	if (handle)
    {
        start_mixer_on_exit = 1;
        ::SendMessage(handle, WM_CLOSE, 0, 0);
    }

    return 0;
}

// ............................................................................

int doit()
{
    if (!kx)
        return UFXSETUP_ERROR_NO_KX;

    // rename ~all '10 Band EQ' to 'EQ G10'
    int i = 0;
    dsp_microcode mc;
    while (!kx.device(i++))
        for (int j = 1; j < 20; j++) // eq is 7th for A1 and 8th for A2
			    if (!kx->enum_microcode(j, &mc) &&
                    !strcmp(mc.name, "10 Band EQ"))
				        kx->set_microcode_name(j, "EQ G10");
    return 0;
}

// ............................................................................

int post()
{
    if (!start_mixer_on_exit)
        return 0;

    // start kxmixer
    STARTUPINFO si = {sizeof(si)};
    PROCESS_INFORMATION pi = {0};
    kali::string path = kx.path();
    path.append("\\kxmixer.exe");
    ::CreateProcess(path, "--startup", 0, 0, 0, 0, 0, 0, &si, &pi);
    if (pi.hProcess)
        ::WaitForInputIdle(pi.hProcess, 4000);
    ::CloseHandle(pi.hProcess);
    ::CloseHandle(pi.hThread);
    return 0;
}

// ............................................................................

int entry(int func)
{
    int ret = UFXSETUP_ERROR_WRONG_FUNC;
    
    switch (func)
    {
    case UFXSETUP_FUNC_CHK_VERSION:
        ret = checkVer();
        break;

    case UFXSETUP_FUNC_PRE:
        ret = pre();
        break;

    case UFXSETUP_FUNC_DO:
        ret = doit();
        break;
        
    case UFXSETUP_FUNC_POST:
        ret = post();
        break;

    default:
        break;
    }

    trace("entry: %i, %i\n", func, ret);

    return ret;
}
