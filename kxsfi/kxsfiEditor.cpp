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


//---------------------------------------------------------------------------
// kxsfiEditor.cpp
//---------------------------------------------------------------------------

#include "kxsfiEditor.h"

#ifndef __kxsfi__
 #include "kxsfi.h"
#endif

#include <math.h>
#include <stdlib.h>	
#include <stdio.h>

#include "clabel.cpp"

#define rx_offs	60	// offset used to "auto size/place" gui objects

//-----------------------------------------------------------------------------
// KXSFIEditor - constructor
//-----------------------------------------------------------------------------
KXSFIEditor::KXSFIEditor (AudioEffect *effect) 
	:	AEffGUIEditor (effect)	
{
	frame = 0;
	rect.left   = 0;			// dimensions interface window
	rect.top    = 0;
	rect.right  = kWidth;
	rect.bottom = kHight;
}

//-----------------------------------------------------------------------------
KXSFIEditor::~KXSFIEditor () // destructor
{}

//-----------------------------------------------------------------------------
bool KXSFIEditor::open (void *ptr)
{
	AEffGUIEditor::open (ptr);	// execute default VST open code

	// get version
	int version = getVstGuiVersion ();
	int verMaj = (version & 0xFF00) >> 16;
	int verMin = (version & 0x00FF);

	cfs = new CFileSelector((AudioEffectX*) effect);


	//--Draw frame-----------------------------------------------
	CRect size (0, 0, kWidth, kHight);
	frame = new CFrame (size, ptr, this);
	CPoint point (0, 0);


	//-- Draw 'load soundfont' label + control ------------------
	size (0, 0, 50, 30);		// groote
	size.offset (kWidth-rx_offs, 10);	// plaats
	loadlabel = new CLabel (size, "Load SF",1);
	point (0,0);
	loadButton = new CKickButton (size, this, kKickTag1, 15, NULL, point);	
	if (loadlabel)
	{
		loadlabel->setFont (kNormalFontSmall);
		loadlabel->setFontColor (kGreyCColor);
		loadlabel->setBackColor (kBlackCColor);
		frame->addView (loadlabel);
		frame->addView (loadButton);
	}

	//-- Draw 'clear soundfont' label + control --------------------------
	size (0, 0, 50, 30);		// groote
	size.offset (kWidth-rx_offs, 60 );		// plaats
	clearlabel = new CLabel (size, "Clear SF", 1);
	point (0,0);
	clearButton = new CKickButton (size, this, kKickTag2, 15, NULL, point);
	if (clearlabel)
	{
		clearlabel->setFont (kNormalFontSmall);
		clearlabel->setFontColor(kGreyCColor);
		clearlabel->setBackColor (kBlackCColor);
		frame->addView (clearlabel);
		frame->addView (clearButton);
	}
	
	//-- Draw 'reload soundfont' label + control --------------------------
	size (0, 0, 50, 30);		// groote
	size.offset (kWidth-rx_offs, 110 );	// plaats
	reloadlabel = new CLabel (size, "Reload", 1);
	point (0,0);
	reloadButton = new CKickButton (size, this, kKickTag3, 15, NULL, point);
	if (reloadlabel)
	{
		reloadlabel->setFont (kNormalFontSmall);
		reloadlabel->setFontColor(kGreyCColor);
		reloadlabel->setBackColor (kBlackCColor);
		frame->addView (reloadlabel);
		frame->addView (reloadButton);
	}	

	//-- Draw 'reset' label + control --------------------------
	size (0, 0, 50, 30);		// groote
	size.offset (kWidth-rx_offs, 160 );	// plaats
	resetlabel = new CLabel (size, "Reset", 1);
	point (0,0);
	resetButton = new CKickButton (size, this, kKickTag4, 15, NULL, point);
	if (resetlabel)
	{
		resetlabel->setFont (kNormalFontSmall);
		resetlabel->setFontColor(kWhiteCColor);
		resetlabel->setBackColor (kBlackCColor);
		frame->addView (resetlabel);
		frame->addView (resetButton);
	}
	
	//-- Draw a Statuslabel --------------------------------------
	size (0, 0, kWidth-(rx_offs+10), 18);		// groote
	size.offset (4, kHight-30);	// plaats
	statuslabel = new CLabel (size, "Status label...", 0);
	if (statuslabel)
	{
		statuslabel->setFont (kNormalFontSmall);
		statuslabel->setFontColor (kWhiteCColor);
		statuslabel->setBackColor (kGreyCColor);
		frame->addView (statuslabel);
	}

	//-- Draw the bank labels and controls ----------------------------------------
	for (int i=0; i < maxbanks;i++)
	{		
		CRect size (0, 0, kWidth-(rx_offs+10), 18);		// groote
		size.offset (4, 10 + i*20 );	// plaats
		sprintf( tempstr,"(bank %d) --------", i);
		banklabel[i] = new CLabel (size, tempstr, 0 );
		bankbutton[i] = new COnOffButton (size, this, (ktag+i), NULL);
		if (banklabel[i])
		{
			banklabel[i]->setFont (kNormalFontSmall);
			banklabel[i]->setFontColor(kWhiteCColor);
			banklabel[i]->setBackColor (kBlackCColor);
			banklabel[i]->setStatus (0);
			frame->addView (banklabel[i]);
			frame->addView (bankbutton[i]);
		}
	}

	// display last status msg from plug
	statuslabel->setLabel ( ((KXSFI*)effect)->globalmsg );	

	// list and display all loaded sf
	listSFbanks();						
	
	return true;
}
//-----------------------------------------------------------------------------
void KXSFIEditor::close ()
{
	if (frame)				// remove the frame.
		delete frame;
	frame = 0;

	// set to zero all pointers (security)
	loadButton  = 0;
	clearButton = 0;
	reloadButton = 0;
	resetButton = 0;
	statuslabel = 0;
	clearlabel = 0;
	loadlabel = 0;
	reloadlabel = 0;
	resetlabel = 0;
	cfs = 0;

	for (int i=0; i < maxbanks;i++)
	{
		banklabel[i] = 0;
		bankbutton[i] = 0;
	}
}
//-----------------------------------------------------------------------------
void KXSFIEditor::resume ()
{
	
}
//-----------------------------------------------------------------------------
void KXSFIEditor::suspend () 
{

}
//-----------------------------------------------------------------------------
void KXSFIEditor::setParameter (long index, float value)
{	
	if (!frame)	// test if the plug is opened
		return;	
}

//-----------------------------------------------------------------------------
// called when something changes in the UI (mouse, key..)
//-----------------------------------------------------------------------------
void KXSFIEditor::valueChanged (CDrawContext* context, CControl* control)
{

	int tag = control->getTag();	

	if ( tag >= ktag && tag <= ktag+maxbanks ) // 'menu' selection
	{
		int n = tag-ktag;		// derive base nr - 0 ~ maxbanks

		if (banklabel[n]->status == 1)
		{
			// unselect this one
			banklabel[n]->setBackColor (kBlackCColor);			
			banklabel[n]->setStatus(0);
			if (getLabelSelect() == -1)
				loadlabel->setFontColor (kGreyCColor);
				clearlabel->setFontColor (kGreyCColor);	
				loadlabel->setBackColor (kBlackCColor);
				clearlabel->setBackColor (kBlackCColor);
			return;
		}
			else
			{	// select this one
				banklabel[n]->setBackColor (kBlueCColor);				
				banklabel[n]->setStatus(1);

				for (int i=0; i <maxbanks;i++)
				{
					if((banklabel[i]->status == 1)  &&  (i != n))
					{	// unselect others
						banklabel[i]->setBackColor (kBlackCColor);						
						banklabel[i]->setStatus(0);
					}
				}
				// loadstatus;  -1 no sfpath in chunk, -2 error, -3 cant open file, 0 success
				if (getLabelSelect() != -1)
				{
					if ( ((KXSFI*)effect)->loadstatus[n] != -1)
					{
						clearlabel->setFontColor (kWhiteCColor);
						clearlabel->setBackColor (kBlueCColor);
					}
					else
					{
						clearlabel->setFontColor (kGreyCColor);
						clearlabel->setBackColor (kBlackCColor);
					}

					loadlabel->setFontColor (kWhiteCColor);
					loadlabel->setBackColor (kBlueCColor);		
				}
				return;
			}
	}
	
	// load, clear, reload and reset buttons ---------------------

	int id = getLabelSelect();	// get the id of the selected bank

	switch (control->getTag())	
	{
	
	case kKickTag2:	// Clear Soudfont ------------------------------------
		if (control->getValue() < 0.5f) break;	// only on mouse down click
		
		if ( id == -1)	// quit if none selected
		{
			statuslabel->setLabel ("Select a bank to clear..!");
			return;
		}
		((KXSFI*)effect)->guiClearSf(id);
		listSFbanks();
		break;


	case kKickTag1:	// Load Soundfont -----------------------------------
		if (control->getValue() < 0.5f) break;	// only on mouse down click
		if ( id == -1)
		{
			statuslabel->setLabel ("Select a bank to load..!");
			return;
		}
		nativefs(id);	// open file selector	
		break;


	case kKickTag3:	// reload all Soundfont --------------------------------
		if (control->getValue() > 0.5f)		// on mouse press
		{
			if(!isEmpty)
			{			
				((KXSFI*)effect)->newChunkFlag = true;
				reloadlabel->setBackColor (kBlueCColor);
			}
			else 
			{
				statuslabel->setLabel ("Nothing to reload..!");
				break;
			}
		}
		else { reloadlabel->setBackColor (kBlackCColor);}	// on mouse release	
		break;



	case kKickTag4:	// Reset Plug --------------------------------
		if (control->getValue() > 0.5f)		// on mouse press
		{
			((KXSFI*)effect)->resetFlag = true;
			resetlabel->setBackColor (kBlueCColor);
			break;		
		}
		else { resetlabel->setBackColor (kBlackCColor);}	// on mouse release	
		break;		

	default:
		control->setDirty();	// was: update()
	}
}
//--------------------------------------------------------------------------
// Update/display the pathnames of loaded sf banks in the bank labels
//--------------------------------------------------------------------------
void KXSFIEditor::listSFbanks()
{
	if (!frame)	// test if the plug is opened
		return;

	isEmpty = true;
	int lstat;
	char name[256];

	// -1 no sfpath in chunk, -2 error, -3 cant open file, 0 success	

	for (int i=0; i < maxbanks ;i++)
	{
		strcpy(name, ((KXSFI*)effect)->projectData.paths[i] );
		lstat = ((KXSFI*)effect)->loadstatus[i];
		if (lstat != -1) isEmpty = false;

		switch (lstat)			
		{
		case -2:		
			sprintf(tempstr,"(bank %d) Error - %s", i, name);
			banklabel[i]->setFontColor (kRedCColor);
			break;
		case -3:		
			sprintf(tempstr,"(bank %d) Can't open - %s", i, name);
			banklabel[i]->setFontColor (kRedCColor);
			break;
		case 0:		
			sprintf(tempstr,"(bank %d) %s", i, name);
			banklabel[i]->setFontColor (kYellowCColor);
			break;
		
		default:
			sprintf(tempstr,"(bank %d) --------", i);
			banklabel[i]->setFontColor (kWhiteCColor);
		}

		banklabel[i]->setLabel (tempstr);
	}

	// update 'Reload button' font color; 1 - no banks loaded, 0 - bank present

	if(isEmpty)
		reloadlabel->setFontColor(kGreyCColor);	 // gray out if all empty
	else
		reloadlabel->setFontColor(kYellowCColor); // highlight Reload font

	// update 'Clear button' gui colors	

	if ( getLabelSelect() != -1)
	{
		if ( ((KXSFI*)effect)->loadstatus[getLabelSelect()] != -1)
		{
			clearlabel->setFontColor (kWhiteCColor);
			clearlabel->setBackColor (kBlueCColor);
		}
		else
		{
			clearlabel->setFontColor (kGreyCColor);
			clearlabel->setBackColor (kBlackCColor);
		}
	}

}
//----------------------------------------------------------------------------------------
int KXSFIEditor::getLabelSelect()	// find and return selected gui banklabel. 
{
	for (int i=0; i <maxbanks;i++)
			{
				if(banklabel[i]->status == 1) return i;					
			} 

	return -1;	// -1 none selected
}
//-------------------------------------------------------------------------------------
void KXSFIEditor::printstatus(char *text) // called by plug to display something
{
	statuslabel->setLabel(text);
}
//--------------------------------------------------------------------------------------
void KXSFIEditor::nativefs(int id)	// open file selector
{		
		VstFileSelect myfs;
		memset (&myfs, 0, sizeof (VstFileSelect));
		VstFileType sf2Type ("Soundfont File", "sf2", "SF2", "sf2");
		myfs.command     = kVstFileLoad;
		myfs.type        = kVstFileType;
		strcpy (myfs.title, "Load SoundFont");
		myfs.nbFileTypes = 1;
		myfs.fileTypes   = &sf2Type;	
		myfs.returnPath  = new char[1024];
		myfs.initialPath = 0;
		
			
			if ( cfs->run(&myfs) )
			{		
				((KXSFI*)effect)->guiLoadSf( id, myfs.returnPath); //load file
				listSFbanks();
			}

		delete []myfs.returnPath;
		if (myfs.initialPath)
			delete []myfs.initialPath;
	
}
//-----------------------------------------------------------------------------------------
