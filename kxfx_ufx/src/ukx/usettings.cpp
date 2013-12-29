
#include "ukx.h"
#include "usettings.h"

//.............................................................................          

uSettings::uSettings(uPlugin* plugin)
{    
	key[0] = 0;        
	kss = new kSettings(plugin->ikx->get_device_name());
	sprintf(key, "pgm_%i", plugin->pgm_id);
}

uSettings::~uSettings()
{    
	if (kss) 
        delete kss;
}

//.............................................................................      

int uSettings::set(char* id, DWORD data)
{    
	if (!kss)
        return ~0;
	kss->write(key, id, (UINT) data);
	return 0;
}

int uSettings::get(char* id, DWORD* data)
{    
    return kss ? kss->read(key, id, data) : ~0;
}