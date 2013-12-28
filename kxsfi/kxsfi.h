// kX VSTi-based SoundFont Manager
// Copyright (c) LeMury and Eugene Gavrilov, 2004.
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


//------------------------------------------------------------------------
// kxsfi.h
//------------------------------------------------------------------------

#define KXSFI_VERSION	5

#include <windows.h>
#include <malloc.h>
#include <stdio.h>


// necessary, since otherwise kxapi.h will re-define CPoint
class CString;
#define _AFX
#include "interface/kxapi.h"

#include "audioeffectx.h"

#include "vstcontrols.h"

#define maxbanks 11				// number of banks, labels, controls
#define kxsynth 0				// target synth for sf loading  0, 1 or 2
#define kxdevice 0				// target kX device for directSynth (0 -first card for now)

#define newchunk_timeout 100	// > 10 should be ok .Used to force 
								// chunk loading from procces if suspend doesn't get called.

enum
{	
	kNumPrograms = 1,	// we use only 1 program
	kNumParams = 0,		// set to 0 so we dont see default editor
	
	// control tags
	kKickTag1,		// load
	kKickTag2,		// clear
	kKickTag3,		// reload
	kKickTag4,		// reset
	ktag,			// menu bars
};

//-----------------------------------------------------------
struct Sfdata						// placeholder for midi banks/patchs list
{	
	char sfbname[maxbanks][64];		// holds banknames
	char sfpname[maxbanks*128][64];	// holds *128 patchnames	
};
//------------------------------------------------------------
struct MyProjectData				// vst chunk for data saving/storing
{
	char prgname[24];				// fxb program name
	char paths[maxbanks][MAX_PATH];		// sf file path names
};
//-------------------------------------------------------------

//--------------------------------------------------------------------------------------
class KXSFI : public AudioEffectX
{

public:
	KXSFI (audioMasterCallback audioMaster);
	~KXSFI ();

	virtual bool getEffectName (char* name);
	virtual bool getProductString (char* text);
	virtual bool getVendorString (char* text);
	virtual VstInt32 getVendorVersion ();

	virtual VstPlugCategory KXSFI::getPlugCategory (){ return (kPlugCategSynth);}

	virtual void process (float **inputs, float **outputs, VstInt32 sampleFrames);
	virtual void processReplacing (float **inputs, float **outputs, VstInt32 sampleFrames);
	virtual VstInt32 processEvents (VstEvents* events);

	virtual void suspend ();
	virtual void resume ();
	virtual VstInt32 getChunk (void** data, bool isPreset);
	virtual VstInt32 setChunk (void* data, VstInt32 byteSize, bool isPreset);
	
	virtual void setProgram (VstInt32 program);
	virtual void setProgramName (char *name);
	virtual void getProgramName (char *name);
	virtual bool getProgramNameIndexed (VstInt32 category, VstInt32 index, char* text);

	virtual void setParameter (VstInt32 index, float value);

	virtual VstInt32 canDo (char* text);
	virtual VstInt32 getMidiProgramName (VstInt32 channel, MidiProgramName* midiProgramName);
	virtual VstInt32 getCurrentMidiProgram (VstInt32 channel, MidiProgramName* currentProgram);
	virtual VstInt32 getMidiProgramCategory (VstInt32 channel, MidiProgramCategory* category);
	virtual bool hasMidiProgramsChanged (VstInt32 channel);

	virtual void MidiProgramsChanged(bool flag);	// set all channels midichange flags
	virtual void initMidiDevices(void);				// init kX 
										
	virtual void loadSFchunk(int clr);				// load sf set from chunk
	virtual int  findKxRom_ver( int banknr);		// find kX sf rom_ver#

	virtual int parse_sf(char *filename, int id, int dest_synth);
	virtual void guiLoadSf( int id, char* filename);
	virtual void guiClearSf( int id);

	virtual void initSFnames();			// init sfdata struct
	virtual void initSFchunk();			// init vstchunk struct
	virtual void eNumSFbanks();			// enumerate loaded kX sf banks
	virtual void clearsfmem(int amount);// wipe  soundfonts
	virtual void resetPlug();

	bool midiChangedFlags[16];	// used to signal host that midiPrograms has changed
	bool newChunkFlag;			// if true - suspend will read new vst chunk
	bool resetFlag;
	bool firstRunFlag;
	int count;					// used for timeout. see procces()

	iKX *ikx;					// ikx interface


	char globalmsg[127];		// default status string displayed on opening editor

	MyProjectData projectData;	// our vstchunk data
	Sfdata mysfData;			// our sf midi bank/patch list
	int loadstatus[maxbanks];	// results of loading actions.
								// -3 can't find, -2 error, -1 empty, 0 success

protected:	

	void fillProgram (VstInt32 channel, VstInt32 prg, MidiProgramName* mpn);
};
