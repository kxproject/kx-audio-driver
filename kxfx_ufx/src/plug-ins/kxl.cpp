
// kxufx.dll

#include "kxl.h"   

// ............................................................................

#define PLUGIN_ENTRY
#include "kxl.list.h" 
#undef  PLUGIN_ENTRY
#define COLLECT_DLL_PLUGINS
const plugin_entry_t plugin_entry[] =
{
    #include "kxl.list.h"
};								
const int plugins_count = sizeof(plugin_entry) / sizeof(plugin_entry_t);
#undef  COLLECT_DLL_PLUGINS       

// ............................................................................

// the only function exported from dll
KXPLUGIN_EXPORT publish_plugins(int command, int param, uintptr_t *ret)
{
	switch(command)
	{
		case KXPLUGIN_GET_COUNT:	 // number of plugins in this dll
			*ret = plugins_count;
			return 0;

		case KXPLUGIN_GET_VERSION:	 // version of kx pligin specification
			*ret = KXPLUGIN_VERSION; // version declared in kxapi.h plugin compiled with
  			return 0;
		
		case KXPLUGIN_GET_NAME:		 // title of plugin to be shown in menus, kxdsp etc.
		case KXPLUGIN_GET_GUID:		 // guid to identify plugin
		case KXPLUGIN_INSTANTIATE:	 // actual request to create plugin
			if (param < plugins_count) 
			    return plugin_entry[param](command, ret);
	}

	*ret = 0;
	return 1;
}
