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


#include "stdafx.h"

#define _KX_NO_KGUI_CLASSES
#include "gui/ksettings.h"

HWND hWndMixer=0; // kX Mixer hWnd shared between all of the kxpc instances

typedef struct
{
 #define KXVSTI_MAGIC	('kXVi')
 unsigned int magic;
 unsigned int size; // header size
 unsigned int settings_mode;
 unsigned int device_id;

 unsigned int fsize; // file size
}kxvsti_settings;

//-----------------------------------------------------------------------------------------
// kxpc
//-----------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------
kXPC::kXPC(audioMasterCallback audioMaster):AudioEffectX(audioMaster, kNumPrograms, kNumParams)
{
	get_chunk_data=NULL;
	init_failed=0;

	settings_mode=KX_SAVED_AUTOMATION|KX_SAVED_DSP|KX_SAVED_NO_VERSION|KX_SAVED_NO_CARDNAME;

	DeviceId=0;

	for(int i=0;i<kNumPrograms;i++)
	 sprintf(program_name[i],"(default %d)",i+1);

	if (audioMaster)
	{
		srand(GetTickCount());

		debug("kXi VST: construction\n");
		setNumInputs(0);	// no inputs
		setNumOutputs(0);	// no outputs
		// hasVu(false);
		// hasClip(false);
		isSynth();
		programsAreChunks();	// this allows to save whatever you want
		setUniqueID('8010');	// Registered ID
		editor = new kXPCEditor(this);
		if(!editor) 
		{
		 init_failed = true;
		 debug("! kXi VST: error instantiating kXPCEditor object\n");
		}
		else
		 ensureKX();
	}
	suspend();
}

//-----------------------------------------------------------------------------------------
kXPC::~kXPC()
{
	debug("kXi VST: destruction\n");

	if(get_chunk_data)
	{
	 free(get_chunk_data);
	 get_chunk_data=NULL;
	}
}

//-----------------------------------------------------------------------------------------
bool kXPC::getEffectName(char* name)
{
	strcpy(name, PLUGIN_TITLE);
	return true;
}

bool kXPC::getVendorString(char* name)
{
	strcpy(name, "kX Project");
	return true;
}

bool kXPC::getProductString(char* name)
{
	strcpy(name, PLUGIN_TITLE);
	return true;
}

//-----------------------------------------------------------------------------------------
VstInt32 kXPC::canDo(char* text)
{
	if (!strcmp(text, "receiveVstEvents"))
		return 1;
	if (!strcmp(text, "receiveVstMidiEvent"))
		return 1;
	return -1;	// explicitly can't do; 0 => don't know
}

//-----------------------------------------------------------------------------------------
void kXPC::resume()
{
	// wantEvents();
        AudioEffectX::resume();
}

//-----------------------------------------------------------------------------------------
VstInt32 kXPC::processEvents(VstEvents* ev)
{
	static VstMidiEvent* event; 
	static LPARAM midiData;

	if((hWndMixer == 0) || (DeviceId < 0))
	  return 0; // don't want more

	for(VstInt32 i = 0; i < ev->numEvents; i++)
	{
		if((ev->events[i])->type != kVstMidiType)
		  continue;

		event = (VstMidiEvent*) ev->events[i];
		midiData = (*((LPARAM*) event->midiData));

		// call kX Control
		SendMessage(hWndMixer, WM_KXMIXER_MESSAGE, (WPARAM) DeviceId, midiData);
                // 'Send' may reduce latency...
	}
	return 1; // want more
}

//-----------------------------------------------------------------------------------------
VstInt32 kXPC::getChunk(void** data, bool isPreset)
{
	if(data==NULL)
	{
		debug("!! kXi VST: getChunk: *data=0\n");
		return 0;
	}
	if(*data!=NULL)
	{
		debug("!!! note: getChunk: **data!=0\n");
	}

	char path[MAX_PATH];
	VstInt32 fileid = rand();

	ensureKX();

	if(hWndMixer)
	{
        	if(SendMessage(hWndMixer, WM_KXMIXER_SAVE_SETTINGS, (WPARAM) DeviceId, (fileid&0xffff)|(settings_mode<<16))==FALSE) // not 'Post'!
        	 goto SAVE_ERROR;

        	sprintf(path, "%s\\kxtmp%d.kx", getenv("TEMP")?getenv("TEMP"):".", fileid);

        	struct _stat st;
        	if(_stat(path,&st)==0)
        	{
                	FILE *f;
                	f=fopen(path,"rb");

                	if(f==NULL)
                	{
                		MessageBox(NULL, "Error saving " PLUGIN_TITLE " settings.\n", PLUGIN_TITLE, MB_ICONERROR);
                		return 0;
                	}
                	else
                	{
                         	if(get_chunk_data)
                         	{
                         	 free(get_chunk_data);
                         	 get_chunk_data=NULL;
                         	}

                		*data=(void *)malloc(st.st_size+sizeof(kxvsti_settings));
                		if(*data==NULL)
                		{
                			fclose(f);
                			debug("! kXi VST: getChunk error allocating %d bytes\n",st.st_size);
                			return 0;
                		}
                		get_chunk_data=*data;

                                // init header
                		kxvsti_settings *set=(kxvsti_settings *)*data;

                		set->magic=KXVSTI_MAGIC;
                		set->size=sizeof(kxvsti_settings);
                		set->settings_mode=settings_mode;
                		set->fsize=st.st_size;
                		set->device_id=DeviceId;

                		// read file & store it
                		fread((void *)((uintptr_t)(*data)+set->size),1,set->fsize,f);

                		fclose(f);

						unlink(path);

                		debug("! kXi VST: settings saved\n");

                		return set->fsize+set->size;
                	}
		}
		else
		{
SAVE_ERROR:
			MessageBox(NULL, "Error saving " PLUGIN_TITLE " settings.\n", PLUGIN_TITLE, MB_ICONERROR);
		}
        } 
        
        *data=NULL;

	return 0;
}

//-----------------------------------------------------------------------------------------
VstInt32 kXPC::setChunk(void* data, VstInt32 byteSize, bool isPreset)
{
	if(byteSize==0 || data==NULL)
	{
	 debug("kXi VST: bytesize=0 in setChunk\n");
	 return 0;
	}
	char path[MAX_PATH];
	int fileid=rand();

	ensureKX();

	if(hWndMixer)
	{
        // process data header
        kxvsti_settings *set=(kxvsti_settings *)data;
        if(set->magic!=KXVSTI_MAGIC)
        {
        	debug("! kXi VST: incorrect magic [%x vs %x]\n",set->magic,KXVSTI_MAGIC);
        	return 0;
        }
        if(set->size!=sizeof(kxvsti_settings))
        {
        	debug("! kXi VST: incorrect structure size [%x vs %x]\n",set->size,sizeof(kxvsti_settings));
        	return 0;
        }

	sprintf(path, "%s\\kxtmp%d.kx", getenv("TEMP")?getenv("TEMP"):".", fileid);

	FILE *f;
	f=fopen(path,"wb");

	if(f==NULL)
	{
LOAD_ERROR:
		MessageBox(NULL, "Error restoring " PLUGIN_TITLE " Settings.\n", PLUGIN_TITLE, MB_ICONERROR);
		return 0;
	}
	else
	{
		settings_mode=set->settings_mode;

		fwrite((void *)((uintptr_t)data+set->size),set->fsize,1,f);
		fclose(f);
		if(SendMessage(hWndMixer, WM_KXMIXER_REST_SETTINGS, (WPARAM) DeviceId, (fileid&0xffff)|(settings_mode<<16))==FALSE) // not 'Post'!
		{
                 unlink(path);		 
		 goto LOAD_ERROR;
		}
		unlink(path);
		return byteSize;
	}
	}

	return 0;
}	

//-----------------------------------------------------------------------------------------

void kXPC::ensureKX()
{
	hWndMixer = FindWindow(NULL, KX_DEF_WINDOW);

	if(!hWndMixer)
	{
		debug("! kXi VST: kX mixer not found!\n");
		return;
	}

        // FIXME NOW: this is incorrect: we should let user to specify which kX device to use
	DeviceId=0;
}

void kXPC::setProgram (VstInt32 program)
{
	debug("kXi VST: setting program %d\n",program);
	if(program<kNumPrograms)
	 curProgram=program;
}

void kXPC::setProgramName (char *name)
{
	debug("kXi VST: setting prg name to %s\n",name);
	strcpy (program_name[curProgram], name);
}

void kXPC::getProgramName (char *name)
{
	debug("kXi VST: getting name...\n");
	strcpy (name, program_name[curProgram]);
}
