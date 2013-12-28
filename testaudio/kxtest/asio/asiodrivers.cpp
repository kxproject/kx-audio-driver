#include "stdafx.h"

#include "asiodrivers.h"

AsioDrivers* asioDrivers = 0;

bool loadAsioDriver(char *name);

bool loadAsioDriver(char *name)
{
	if(!asioDrivers)
		asioDrivers = new AsioDrivers();
	if(asioDrivers)
		return asioDrivers->loadDriver(name);
	return false;
}

#include "iasiodrv.h"

extern IASIO* theAsioDriver;

AsioDrivers::AsioDrivers() : AsioDriverList()
{
	curIndex = -1;
}

AsioDrivers::~AsioDrivers()
{
}

bool AsioDrivers::getCurrentDriverName(char *name)
{
	if(curIndex >= 0)
		return asioGetDriverName(curIndex, name, 32) == 0 ? true : false;
	name[0] = 0;
	return false;
}

long AsioDrivers::getDriverNames(char **names, long maxDrivers)
{
	for(long i = 0; i < asioGetNumDev() && i < maxDrivers; i++)
		asioGetDriverName(i, names[i], 32);
	return asioGetNumDev() < maxDrivers ? asioGetNumDev() : maxDrivers;
}

bool AsioDrivers::loadDriver(char *name)
{
	char dname[64];
	char curName[64];

	for(long i = 0; i < asioGetNumDev(); i++)
	{
		if(!asioGetDriverName(i, dname, 32) && !strcmp(name, dname))
		{
			curName[0] = 0;
			getCurrentDriverName(curName);	// in case we fail...
			removeCurrentDriver();

			if(!asioOpenDriver(i, (void **)&theAsioDriver))
			{
				curIndex = i;
				return true;
			}
			else
			{
				theAsioDriver = 0;
				if(curName[0] && strcmp(dname, curName))
					loadDriver(curName);	// try restore
			}
			break;
		}
	}
	return false;
}

void AsioDrivers::removeCurrentDriver()
{
	if(curIndex != -1)
		asioCloseDriver(curIndex);
	curIndex = -1;
}
