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


//------------------------------------------------------------------------------------
// kxsfiEditor.h
//------------------------------------------------------------------------------------

#include "vstgui.h"
#include "cfileselector.h"

#define maxbanks 11	// number of banks, labels, kickcontrols

enum	// gui frame size
{
	kWidth = 600,
	kHight = 270
};

class CLabel;	// custom label object

//-----------------------------------------------------------------------------------
class KXSFIEditor : public AEffGUIEditor, public CControlListener
{
public:
	KXSFIEditor (AudioEffect *effect);
	virtual ~KXSFIEditor ();

	void suspend ();
	void resume ();
	virtual int getLabelSelect();			// returns current selected banklabel
	virtual void listSFbanks();				// updates menu display
	virtual void printstatus(char *text);	// used by plug to display in the status bar
	virtual void nativefs(int id);			// open fileselector

	char tempstr[256];
	int isEmpty;							// if true -- no SFs uploaded

protected:
	virtual bool open (void *ptr);
	virtual void close ();
	void setParameter (long index, float value);

private:
	void valueChanged (CDrawContext* context, CControl* control);

	//labels and controls
	CKickButton		*loadButton;
	CKickButton		*clearButton;
	CKickButton		*reloadButton;
	CKickButton		*resetButton;
	CLabel			*statuslabel;
	CLabel			*loadlabel;
	CLabel			*clearlabel;
	CLabel			*reloadlabel;
	CLabel			*resetlabel;

//	CLabelSw		*banklabel[maxbanks];


	CLabel			*banklabel[maxbanks];
	COnOffButton	*bankbutton[maxbanks];

	CFileSelector	*cfs;

};

//------------------------------------------------------------------------------------
