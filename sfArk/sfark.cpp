#include "sfArklib.h"

#include <io.h>
#include <windows.h>

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


// Callback functions needed by sfArkLib (must be supplied by Application) ...
void sfkl_msg(const char * /*MessageText*/, int /*Flags*/)	// Message display function
{
}

void sfkl_UpdateProgress(int /*ProgressPercent*/)		// Progress indication
{
}

bool sfkl_GetLicenseAgreement(const char *LicenseText, const char *LicenseFileName) // Display/confirm license
{
 if(LicenseFileName)
  _unlink(LicenseFileName);

 if(MessageBox(NULL,LicenseText,"License",MB_YESNO)==IDYES)
  return TRUE;
 else
  return FALSE;
}

void sfkl_DisplayNotes(const char * /*NotesText*/, const char * /*NotesFileName*/)	// Display notes text file
{
}
