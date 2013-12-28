// kX VSTi Interface
// Copyright (c) Max Mikhailov, Eugene Gavrilov, 2002-2014.
// All rights reserved

/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */



#ifndef __kXVSTi__
#define __kXVSTi__

#define PLUGIN_TITLE	"kXi"	

enum
{
	kNumPrograms = 1,
	kNumOutputs = 0,
	kNumParams = 0,
};

//------------------------------------------------------------------------------------------
class kXPC : public AudioEffectX
{
public:
	kXPC(audioMasterCallback audioMaster);
	~kXPC();

	virtual void process(float **inputs, float **outputs, VstInt32 sampleframes){};
	virtual void processReplacing(float **inputs, float **outputs, VstInt32 sampleframes){};
	virtual VstInt32 processEvents(VstEvents* events);

	virtual void resume();

	virtual VstInt32 getChunk(void** data, bool isPreset = false);
	virtual VstInt32 setChunk(void* data, VstInt32 byteSize, bool isPreset = false);
	
	virtual bool getEffectName(char* name);
	virtual bool getVendorString(char *name);
	virtual bool getProductString(char *name);
	virtual VstInt32 getVendorVersion() {return 1;}
	virtual VstInt32 canDo(char* text);

	int DeviceId;

	int settings_mode;

	int init_failed;

	void *get_chunk_data;

	void ensureKX();

        // sf stuff:
	void setProgram(VstInt32 program);
	void setProgramName(char *name);
	void getProgramName(char *name);
	char program_name[kNumPrograms][25];
};

//------------------------------------------------------------------------------------------
// kX Mixer - VSTi inter-communications
//------------------------------------------------------------------------------------------

#include "interface/kxmixer.h"

#endif
