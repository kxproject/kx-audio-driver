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
// kxsfi.cpp
//------------------------------------------------------------------------
#include "kxsfi.h"
#include "vers.h"
#include "kxsfiEditor.h"

// added for 2.4 VST SDK
AudioEffect* createEffectInstance (audioMasterCallback audioMaster)
{
        KXSFI* effect = new KXSFI (audioMaster);	// Create the AudioEffect
        return effect;
}
#include "vstplugmain.cpp"

//-----------------------------------------------------------------------------
KXSFI::KXSFI (audioMasterCallback audioMaster) :	// plugin constructor
  AudioEffectX (audioMaster, kNumPrograms, kNumParams)
{	
	MidiProgramsChanged(false);		// initialy reset these flags
	newChunkFlag = false;		
	resetFlag = false;
	firstRunFlag = true;			// set at plug start
	count = 0;

	ikx=NULL;

	if (audioMaster)
	{
		setNumInputs (0);
		setNumOutputs (0);
		canProcessReplacing ();
		// hasVu (false);     // deprecated
		// hasClip (false);   // deprecated
		isSynth (true);	
		setUniqueID ('KXSF');
		sampleRate = getSampleRate();
		programsAreChunks(true);		// programs are 'formatless' chunks	
	 }

	initMidiDevices();					// init kX and midi devices
	clearsfmem(128);					// clear sf memory

	initSFnames();						// init dummy sfdata midiprogramnames for SX
	initSFchunk();						// init empty vst chunk

	setProgram(0);						// inform host of the current program nr.
	setEditor (new KXSFIEditor (this));	// we have our own vstgui editor

}

//-----------------------------------------------------------------------------
KXSFI::~KXSFI ()			//plugin destructor
{
	clearsfmem(maxbanks+1);	// wipe soundfonts on exit

	if(ikx)					// close ikx interface
	{
	 ikx->close();
	 delete ikx;	
	 ikx=NULL;
	}
}
//-------------------------------------------------------------------------------
VstInt32 KXSFI::canDo (char* text)
{
	if (!strcmp (text, "receiveVstEvents"))
		return 1;
	if (!strcmp (text, "receiveVstMidiEvent"))
		return 1;
	if (!strcmp (text, "midiProgramNames"))
		return 1;
	return -1;	// explicitly can't do; 0 => don't know
}
//-----------------------------------------------------------------------
void KXSFI::process (float **inputs, float **outputs, VstInt32 sampleFrames)
{
		if ( newChunkFlag || resetFlag )
		{	
			if(firstRunFlag)
			{
				count++;				
				if ( count < newchunk_timeout ) return;
			}
			
			suspend();
		}
}
//-----------------------------------------------------------------------------
void KXSFI::processReplacing (float **inputs, float **outputs, VstInt32 sampleFrames)
{	
		if ( newChunkFlag || resetFlag )
		{	
			if(firstRunFlag)
			{
				count++;				
				if ( count < newchunk_timeout ) return;
			}
			
			suspend();
		}
}
//-----------------------------------------------------------------------------
void KXSFI::resume ()
{
	if (editor)
		((KXSFIEditor*)editor)->resume ();
	
	// wantEvents ();	// tell host we want VSTMidiEvents
        // deprecated in 2.4 VST
        AudioEffectX::resume();

}
//-----------------------------------------------------------------------------
void KXSFI::suspend ()
{
	// All notes off here. CC 120/123 status = 176--191, data1 = 120, data2 = 0
	// status=176, data1=120, data2=0;	
	for ( int ch=0; ch <16;ch++ ) 
	{	 
	 ikx->send_synth(0,ch+0xb0+(120<<8));
	}
			
	if (resetFlag)
	{
		resetPlug();			// reset the whole plug
		resetFlag = false;		// toggle flag
	}
	
	if (newChunkFlag || firstRunFlag)		// load saved sf files from the VSTchunk.
	{
		firstRunFlag = false;		// toggle flag. Is only set ones during plug start
		newChunkFlag = false;		// toggle flag now, to prevent reload on next suspend
		loadSFchunk(maxbanks);		// load sf set from chunk		
	}

	if (editor)
		((KXSFIEditor*)editor)->suspend ();

}
//--------------------------------------------------------------------------
//getChunk means 'dear plug please give me your data for persistent storage'.
//--------------------------------------------------------------------------
VstInt32 KXSFI::getChunk (void** data, bool isPreset)
{
	*data = &projectData;
	return sizeof (MyProjectData);
}
//--------------------------------------------------------------------------
// Host gives data; setChunk; 'hi plug here's what you gave me for storage, load it back in'.
//--------------------------------------------------------------------------
VstInt32 KXSFI::setChunk (void* data, VstInt32 byteSize, bool isPreset)
{	
	if (byteSize != sizeof (MyProjectData))
		return 0;
	projectData = *(MyProjectData*)data;


	newChunkFlag = true;	// notify plug we have a new chunk to load
		
	return 1;
}
//--------------------------------------------------------------------------------------
// Load sf files from vst chunk, and set loadstatus.
// loadstatus; -1 no sfpath in chunk, -2 error, -3 cant open file, 0 success
//------------------------------------------------------------------------------------
void  KXSFI::loadSFchunk(int clr)
{
	clearsfmem(clr);
	setProgramName (projectData.prgname);	// use the chunks program name


	for (int id=0; id< maxbanks;id++)
	{		
		if ( strncmp((projectData.paths[id]), "empty", 5) == 0)
		{
			loadstatus[id] = -1; // is empty
		}
		else
			parse_sf( projectData.paths[id], id, kxsynth);
	}

	eNumSFbanks();				// update midiprogramnames&banknames

	if (editor)
		((KXSFIEditor*)editor)->listSFbanks();	// update gui if editor is open.

}
//-----------------------------------------------------------------------------------------
// VST MIDI events handler
//-----------------------------------------------------------------------------------------
VstInt32 KXSFI::processEvents (VstEvents* ev)
{
	
	for (long i = 0; i < ev->numEvents; i++)
	{
		if ((ev->events[i])->type != kVstMidiType)
			continue;

		VstMidiEvent* event = (VstMidiEvent*)ev->events[i];
                // [e]: changed 'char'->'unsigned char'
		unsigned char *midiData = (unsigned char *)event->midiData;

		// send all midi data to kX Synth (directSynth method)
		ikx->send_synth(0,midiData[0] + ((midiData[1])<<8) +(midiData[2]<<16));

		event++;
	}
	return 1;	// tell host we want more
}
//-----------------------------------------------------------------------------
void KXSFI::setProgram (VstInt32 program)	
{
	if (program < 0 || program >= kNumPrograms)
		return;

	curProgram = program;	
}
//-----------------------------------------------------------------------------
// set the current program name (max. 24 characters)
void KXSFI::setProgramName (char *name)
{
	strcpy( projectData.prgname, name); 
}
//-----------------------------------------------------------------------------
void KXSFI::getProgramName (char *name) 
{
	strcpy(name, projectData.prgname);
}
//-------------------------------------------------------------------------
bool KXSFI::getProgramNameIndexed (VstInt32 category, VstInt32 index, char* text)
{	
	if (index < kNumPrograms)
	{
		strcpy (text, projectData.prgname);
		return true;
	}
	return false;

}
//-----------------------------------------------------------------------------
// Called by editor if setparameterAutomated ( I dont use it anymore)
//-----------------------------------------------------------------------------
void KXSFI::setParameter (VstInt32 index, float value)
{}
//------------------------------------------------------------------------
// midi program names:
//------------------------------------------------------------------------
VstInt32 KXSFI::getCurrentMidiProgram (VstInt32 channel, MidiProgramName* mpn)
{
	if (channel < 0 || channel >= 16 || !mpn)
		return -1;

	VstInt32 prg = channel;
	mpn->thisProgramIndex = prg;

	fillProgram (channel, prg, mpn);	//get the patch name
	return prg;							//and also return it's nr
}
//------------------------------------------------------------------------
VstInt32 KXSFI::getMidiProgramName (VstInt32 channel, MidiProgramName* mpn)
{
	VstInt32 prg = mpn->thisProgramIndex;
	
	if ( prg < 0 || prg > (maxbanks * 128) )
		return 0;				

	fillProgram (channel, prg, mpn);	// get midi preset name
	
	return (maxbanks * 128);			// return total num patches
}
//------------------------------------------------------------------------
// Fill out one midi program. Input: midi channel and vstprg
void KXSFI::fillProgram (VstInt32 channel, VstInt32 prg, MidiProgramName* mpn)
{
	int banknr = prg/128;			// calc vst bank number 0 -- maxbanks
	int p_nr = prg - (banknr*128);	// calc vst patch number 0 -- 127

	strcpy(mpn->name, mysfData.sfpname[prg]);	// get correct patch name, and parse it
	mpn->midiProgram = (char)p_nr;				// parse correct patch nr (0--127)
	mpn->midiBankMsb = (char)banknr;					// parse correct bank select nr
	mpn->midiBankLsb = -1;
	mpn->parentCategoryIndex = banknr;			// parse correct bank/category
	mpn->reserved = 0;
	mpn->flags = 0;

}
//------------------------------------------------------------------------
// IN  >'thisCategoryIndex' nr.
// OUT > name, and number of banks

VstInt32 KXSFI::getMidiProgramCategory (VstInt32 channel, MidiProgramCategory* cat)
{
	cat->parentCategoryIndex = -1;	// -1:no parent category
	cat->flags = 0;					// reserved, none defined yet, zero.

	VstInt32 category = cat->thisCategoryIndex;

	strcpy (cat->name, mysfData.sfbname[category]);

	return maxbanks;	// number of available categories (banks) on that channel.
						// If 0, no categories supported.
}
//---------------------------------------------------------------------------
void KXSFI::initSFnames()	// init empty dummy banks&patches struct
{
	char name[64];

	for(int i=0; i < maxbanks; i++)
	{			
		sprintf(name, "(bank %02d) --- ", i);
		strcpy(mysfData.sfbname[i], name);	// set bank name
		
			
			for( int p=0; p < 128; p++)	// set all patch names
			{
				sprintf(name, "%03d empty", p);				
				//the right patch store location = (i * 128) + p
				strcpy(mysfData.sfpname[(i*128)+p] , name);			
			}
	}	
}
//--------------------------------------------------------------------------
void KXSFI::initSFchunk()	// init empty vst chunk struct
{
	for (int s=0; s < maxbanks; s++)	
	{
		loadstatus[s] = -1;	
		strcpy (projectData.paths[s], "empty");
	}
	
	strcpy(projectData.prgname, "Init SF Set");
}
//--------------------------------------------------------------------------
// Get currently loaded kX SF banks and format it into mysfData struct
//--------------------------------------------------------------------------
void KXSFI::eNumSFbanks()	
{	
	int id;
	int sz=-3;

	initSFnames();	// re-init midi program struct

    sz=ikx->enum_soundfonts(0,0);

    if(sz>0)
       	{
			sfHeader *hdr;
       		hdr=(sfHeader *)malloc(sz);
       		if(hdr)
       		{
       			if(!ikx->enum_soundfonts(hdr,sz))
       			{
				 char tmpstr[128];	// to small can cause crash!

        	     for(dword i=0;i < sz/sizeof(sfHeader) ;i++) 
       				{
					 
					id = hdr[i].sfman_id;		// get sfman_id (is real bank nr)				

					// get and save bank name

					sprintf( tmpstr ,"(bank %02d) %s" , id,  &hdr[i].name[0] );
					strncpy(mysfData.sfbname[id] , (tmpstr), 64); //not more then 64 char

					// print all patch names at location = id*128 + p				
					for(int p=0; p < 128; p++)					
						{					
						if(!ikx->get_preset_description( id, p, tmpstr))
							sprintf( mysfData.sfpname[(id*128)+p], "%03d %s", p, tmpstr);
						else							
							sprintf( mysfData.sfpname[(id*128)+p], "%03d %s", p, "empty");						
						}						
					}
				}							
       		}
			free(hdr);
		}

	MidiProgramsChanged(true);	// signal host that all midiprogramnames have changed
}

//--------------------------------------------------------------------
void KXSFI::clearsfmem(int amount)	// unload soundfonts
{	
	for (int i=0; i < amount; i++) // FIXME!! why 'amount'?.. [e]
		/* Well, why execute unload 256 banks if currently we only load bank 0--11 maximum?
		Unless ofcourse there's a method to unload all sf other then this one.[lex]*/
	{
		ikx->unload_soundfont(i);
	}
}
//--------------------------------------------------------------------
// call this to set all midichannel's changeflags (true or false)
// to notify host that midiprogramnames have changed
void KXSFI::MidiProgramsChanged (bool flag)
{
	for (int i=0; i <16; i++) { midiChangedFlags[i] = flag;}
}

//--------------------------------------------------------------------
// called by host for each midichannel
bool KXSFI::hasMidiProgramsChanged (VstInt32 channel)
{	
	if (midiChangedFlags[channel] == true)
	{
		midiChangedFlags[channel] = false;	// reset flag		
		return true;						// signal host of change
	}
	else
		return false;	
}

//--------------------------------------------------------------------------------
// Find kX sf rom_ver#. Input is sfman_id. Used for unloading only.
//--------------------------------------------------------------------------------
/* Before we can delete a soundfont we have to get the right rom_ver#,
because upon reloading, kX re-numbers the rom_ver.  FIXME..!   

[e]: this is not rom_ver, by sf_id used by kX SF manager (you mean 'but' instead of 'by'?[lex])
     the algorithm should be changed FIXME!!

[lex]: Ok, maybe I used the wrong naming convention here, but I realy dont'
see another way to find the right corresponding sf_id or whatever it is called
I.e. Loading a sf into bank 9 and then one in bank 3, kX lists this as;
#2(bank 3)
#1(bank 9)
The relationship between kX's SF # and (bank nr) gives me a headache
to sync it with my vsti's GUI:)
*/
int KXSFI::findKxRom_ver( int banknr)
{
	int sz=-3;

       	sz=ikx->enum_soundfonts(0,0);
       	if(sz>0)
       	{       	 
       	  sfHeader *hdr;
       	  hdr=(sfHeader *)malloc(sz);
       		if(hdr)
       		{
       			if(!ikx->enum_soundfonts(hdr,sz))
       			{
					for(dword i=0;i < sz/sizeof(sfHeader);i++)
       				{
						if (banknr == hdr[i].sfman_id){
							sz = hdr[i].rom_ver.minor;
							free (hdr);
							return sz;}       				 
       				}       				
       			}				
       			free(hdr);
       		}
			else sz=-1; 	// not enough mem
       	}		
    return sz;	// return kX sf rom_ver#
}
//-----------------------------------------------------------------------------------------
// Load one sounfont from file into kX soundfont mem.
//-----------------------------------------------------------------------------------------
int KXSFI::parse_sf(char *filename, int id, int dest_synth)
{
	int ret = 0;
        /*

        // [e]: we don't need this since we do not support 'folders'
		[lex]: Ok, but we still need a method to verify/report if a file can not be found!
	FILE *f;
    f=fopen( filename, "rb");
		if(f)
		{
			fclose(f);			
        */
			ret=ikx->parse_soundfont( filename ,NULL, id, dest_synth);
			if(ret<=0)
				loadstatus[id] = -2; // Error loading
			else
				loadstatus[id] = 0; // success! Loaded OK
/*		}
        else
		{
			ret=ikx->compile_soundfont(filename, NULL);
			loadstatus[id] = -3; // can't open or find file
		}
*/
	return loadstatus[id]; // -3 can't find, -2 error, 0 success
}

//---------------------------------------------------------------------------------
void KXSFI::initMidiDevices(void)			// kX and midi device initialisation 
{

	// iKx initialisation for soundfont management --------
	ikx = new iKX();

	if(ikx->init(kxdevice)) // try first device for now!; FIXME!! should ask user to choose the device						
	{													// I agree. ToDo [lex]
	 sprintf(globalmsg, " - Error, no kX device!      ( kXSFi - %d.%d )", KX_ASIO_VERSION,KXSFI_VERSION);
	}
	else
	{
		sprintf(globalmsg, "Using kX device:[%d] %s       ( kXSFi - %d.%d )",
							kxdevice ,
							ikx->get_winmm_name(KX_WINMM_SYNTH),
							KX_ASIO_VERSION, KXSFI_VERSION );
	}

 
	if (editor)
		((KXSFIEditor*)editor)->printstatus(globalmsg);
	
}
//--------------------------------------------------------------------------------------------
void KXSFI::resetPlug()				// call this from suspend if plug was running
{
	clearsfmem(maxbanks+1);			// clear used sf memory

	if(ikx)							// close ikx interface
	{
	 ikx->close();
	 delete ikx;	
	}	

	initSFnames();		// init dummy sfdata midiprogramnames for SX
	initSFchunk();		// init empty vst chunk

	initMidiDevices();						// re-init kX and midi devices

	setProgramName (projectData.prgname);	// use the chunks program name and set it
	updateDisplay();						// shows/updates the right Program name

	MidiProgramsChanged (true);				// notify host to rescan midi names

	if (editor)
		((KXSFIEditor*)editor)->listSFbanks();	// update gui if open

}
//------------------------------------------------------------------------
bool KXSFI::getEffectName (char* name)
{
	strcpy (name, "kXSFi");
	return true;
}
//------------------------------------------------------------------------
bool KXSFI::getProductString (char* text)
{
	strcpy (text, "kXSFi");
	return true;
}
//------------------------------------------------------------------------
bool KXSFI::getVendorString (char* text)
{
	strcpy (text, "kX Project");
	return true;
}
//------------------------------------------------------------------------
VstInt32 KXSFI::getVendorVersion ()
{ 
	return (1000+KXSFI_VERSION);
}
//-------------------------------------------------------------------------
void KXSFI::guiLoadSf( int id, char* filename) // called from gui
{
		// unload previous loaded sf first here..!
		int rom_ver =  findKxRom_ver(id);	// find rom_ver
		if (rom_ver > 0 && rom_ver < maxbanks) // FIXME!! [e] probably, '&&'..Yep, my fault. Fixed[lex]
				ikx->unload_soundfont(rom_ver);
		
		int ret = parse_sf( filename, id, kxsynth); // load sf file

		if(ret<0)
		{	//error
			strcpy(projectData.paths[id], filename  );
			if (editor)
				((KXSFIEditor*)editor)->printstatus("Error loading soundfont.!");
		}
		else
		{	// success! 
			strcpy(projectData.paths[id], filename );
			eNumSFbanks();	// update midi program names	
			if (editor)
				((KXSFIEditor*)editor)->printstatus("Load successfully..!");
		}
}
//----------------------------------------------------------------------------
void KXSFI::guiClearSf(int id) // called from gui
{
	switch( loadstatus[id] ) //-1 no sfpath in chunk, -2 error, -3 cant open file, 0 success
	{
	case -3:
	case -2:
		strcpy(projectData.paths[id], "empty" );
		loadstatus[id] = -1;
		if (editor)
			((KXSFIEditor*)editor)->printstatus("Deleted from load list..!");
		break;
		
	case -1:
		if (editor)
			((KXSFIEditor*)editor)->printstatus("Nothing to clear..!");
		break;		

	case 0:
		int rom_ver = findKxRom_ver(id);	// get kX sf rom_ver#
		if (rom_ver <= 0 || rom_ver > maxbanks)
			{ return;}	// error

		ikx->unload_soundfont(rom_ver);		// unload sf
		strcpy(projectData.paths[id], "empty" );
		loadstatus[id] = -1;
		eNumSFbanks();						// update midiprogramnames
		if (editor)
			((KXSFIEditor*)editor)->printstatus("Cleared successfully..!");
		break;
	}
}
//----------------------------------------------------------------------------------
