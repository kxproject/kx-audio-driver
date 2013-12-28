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

#include "resource.h"

enum {
	tLauncher = 1,
};

extern HWND hWndMixer;

kXPCEditor::kXPCEditor(AudioEffect *effect_)
	: AEffGUIEditor(effect_)
{
	effect=effect_;

	effect->setEditor(this); // notify effect that ‘this is the editor’

	// init bitmap handles
	hButton = 0;

	// load the background bitmap
	hBackground  = new CBitmap(IDB_BACKGROUND);

	// init the size of the plugin
	rect.left   = 0;
	rect.top    = 0;
	rect.right  = (short) hBackground->getWidth();
	rect.bottom = (short) hBackground->getHeight();
}

kXPCEditor::~kXPCEditor()
{
	// free bitmaps
	if (hBackground)
		hBackground->forget();
	hBackground = 0;
}

//-----------------------------------------------------------------------------
bool kXPCEditor::open(void *ptr)
{
	// always call this
	AEffGUIEditor::open(ptr);

	// load some bitmaps
	if (!hButton)
		hButton = new CBitmap(IDB_BUTTON);

	//--init background frame-----------------------------------------------
	CRect size(0, 0, hBackground->getWidth(), hBackground->getHeight());
	frame = new CFrame(size, ptr, this);
	frame->setBackground(hBackground);

	// Button
	CPoint p(0,0);
	size(hBackground->getWidth() - hButton->getWidth() - 12,
		hBackground->getHeight() - hButton->getHeight() / 2 - 2,
		hBackground->getWidth() - 12, 
		hBackground->getHeight() - 2 );
	bLauncher = new CKickButton(size, this, tLauncher, hButton->getHeight() / 2,
		hButton, p);
	frame->addView (bLauncher);

	return true;
}

//-----------------------------------------------------------------------------
void kXPCEditor::close()
{
	delete frame; // fixme, frame also deletes all control added with "frame->addView"
	frame = 0;

	if (hButton)
		hButton->forget();
	hButton = 0;
}

//-----------------------------------------------------------------------------
void kXPCEditor::valueChanged(CDrawContext* context, CControl* control)
{
	VstInt32 tag = control->getTag();

	if(control->getValue()!=0.0)
	{
	 switch (tag)
	 {
		case tLauncher:
			// launch kX Control Window
			((kXPC*) effect)->ensureKX();
			if(hWndMixer) // 'Post' here
			  PostMessage(hWndMixer, WM_KXMIXER_TWEAK_VSTI, (WPARAM) ((kXPC*) effect)->DeviceId, 0);
			else
			  MessageBox(NULL,"Launch kX Mixer first!","kXi VST",MB_OK|MB_ICONINFORMATION);
			break;
	 }
	}
}
