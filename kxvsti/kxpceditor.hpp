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


#ifndef __kXPCEditor__
#define __kXPCEditor__

class kXPCEditor : public AEffGUIEditor, public CControlListener 
{
friend class AudioEffect;

public:
	kXPCEditor(AudioEffect *effect);
	virtual ~kXPCEditor();

protected:
	virtual bool open(void *ptr);
	virtual void close();

	virtual void valueChanged(CDrawContext* context, CControl* control);

private:

	// Effect (for reference)
	AudioEffect *effect;

	// Controls
	CKickButton *bLauncher;

	// Bitmaps
	CBitmap *hBackground;
	CBitmap *hButton;

};

#endif
