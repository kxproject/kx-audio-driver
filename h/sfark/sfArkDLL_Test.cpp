// sfArkDLL_Test ... Win32 console test program for sfArkDLL
// sfArkLib/sfArkDLL source & documentation is copyright 1998-2001 Andy Inman
// email: andyi@melodymachine.com

// Make sure sfArkDLL.dll file resides in the output directory!

char	ThisProgVersion[] = "2.0";

#define	EXPECT_SFARK_API_VERSION	100		// Excpect sfArk API version 1.00

#include <stdio.h>

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#undef	WIN32_LEAN_AND_MEAN

#include "..\sfArkDLL\sfArkDLL.h"	// sfArkDLL header


// ============================================================================
// Application supplied callback functions (optional).....

// Text message display...
void __cdecl MyShowMessage(LPCSTR MessageText, int Flags)
{
	SFARK_CB_ShowMessage *pf = MyShowMessage;	// Type-check the function definition

	// The library calls this to display a text message to the user.
	// You may display these messages to the user, or just ignore them
	// in many cases.  Note, if Flags SFARK_MSG_PopUp is set, then
	// the message generally relates to an error condition, and you
	// should probably display it, unless you are going to do all error
	// handling in your application.

	// If you don't define this callback, no messages will be displayed,
	// by the DLL, except for SFARK_MSG_PopUp which displays windows message box.

	if (Flags & SFARK_MSG_PopUp)
	  printf("*** IMPORTANT: %s\n", MessageText);
	else
	  printf("Info: %s\n", MessageText);
}


// Display a "notes" text file...
void __cdecl MyDisplayNotes(LPCSTR NotesFileName)
{
	SFARK_CB_DisplayNotes *pf = MyDisplayNotes;	// Type-check the function definition

	// Display the "notes" text file optionally included with a sfArk file.
	// You SHOULD display this text, as it was intended by the soundfont author
	// that the end user should see it.  However, you may wish to have
	// a user preferences setting to determine whether or not to display this file,
	// or ask each time if the user wants to see the file before displaying it.
	// Whether or not you display it, you may want to ask the user whether
	// to keep or delete the file.

	// If you don't define this call back, the DLL attempts to display the
	// Notes file using NotePad (etc.)

	printf("NOTES file: %s\n", NotesFileName);	// For testing, just display the filename 
	return;
}

// Get user agreement to "license"
bool __cdecl MyGetLicenseAgreement(LPCSTR LicenseText)
{
	SFARK_CB_GetLicenseAgreement *pf = MyGetLicenseAgreement;	// Type-check the function definition

	// Display the text of a license agreement and return true if user agrees

	// IMPORTANT: Under licensing terms to use sfArkDLL your application MUST
	// display the license text file included with a sfArk file, and the user
	// must be asked if he agrees.  If he does not agree, you must return FALSE
	// from this function so that decompression will not continue.
	// The license text file must NOT be deleted, and must be left in the
	// same directory as the decompressed SF2 file.  If your application moves
	// the SF2 file, then it must move the license file with it.

	// If you don't define this call back, the DLL displays the license
	// text using a Windows message box, and the user needs to click the Yes button.

	printf("LICENSE text:\n%s\n", LicenseText);	// display the text of the file
	return false;
	//return true // ONLY for testing... see above.
}

// Progress display
void __cdecl MyUpdateProgress(int ProgressPercent)
{
	SFARK_CB_UpdateProgress *pf = MyUpdateProgress;	// Type-check the function definition

	// This will be called by the library for every 1% of the process completed
	// If you don't define this callback, no action is performed by the DLL.

	printf("Progress %d%%\r", ProgressPercent);
}



// ******************************* M A I N ************************************

int main(int argc,char *argv[])
{
	// Print welcome message...
	printf("sfArkDLL_Test V%s - Test Program for sfArkDLL (sfArk Decompression Library)\n", ThisProgVersion);
	printf("sfArk SoundFont Compression is copyright 1998-2001, melodymachine.com\n\n");

	// Display the path to the dll that is in use (just to check its where you expect)...
	HMODULE ModuleHandle = GetModuleHandle("sfArkDLL");
	char ModuleFileName[SFARK_MAX_FILEPATH];
	GetModuleFileName(ModuleHandle, ModuleFileName, SFARK_MAX_FILEPATH);
	printf("Using sfArkDLL in: %s\n", ModuleFileName);

	char CurrentDirectory[SFARK_MAX_FILEPATH];
	GetModuleFileName(ModuleHandle, ModuleFileName, SFARK_MAX_FILEPATH);
	GetCurrentDirectory(SFARK_MAX_FILEPATH, CurrentDirectory);
	printf("Current directory: %s\n", CurrentDirectory);
	
	// Display the sfArkLib engine, DLL and API versions...
	printf("sfArkLib version: %d\n", sfArkGetLibVersion());
	printf("sfArkDLL version: %d\n", sfArkGetDLLVersion());
	printf("sfArkDLL API version: %d\n\n", sfArkGetAPIVersion());

	// If API version is not what you expect, that means API has changed
	// and it is risky to continue.  
	// In practice, we will change major version number (100, 200, 300) only if
	// the new API is not compatible with the prervious API, and change minor
	// version (100, 101, 102...) when we add features.
	// So, if you expect version 120, then anything up to 199 should be ok,
	// but 100-119 may lack some functionality that you need.
	// The above GetVersion functions will remain unchanged in all future versions of sfArkDLL.
	if (sfArkGetAPIVersion() < EXPECT_SFARK_API_VERSION  
	 || sfArkGetAPIVersion()/100 > EXPECT_SFARK_API_VERSION/100 )
	{
		printf("Can't handle this API version.  Terminating.\n");
		return 3;
	}

	// See if the DLL supports compression (DLL will display a message if not).
	// If your application is trying to compress a file, and this function returns
	// false, you can exit now.  If you only want to decompress a file,
	// you don't need to call this.
	if (sfArkCompressionSupported())  printf("DLL supports compression.\n\n.");

	// Set the callbacks (all are optional, see functions above for default behaviour)
	//sfArkSetCallback(SFARK_CBID_ShowMessage, MyShowMessage);
	//sfArkSetCallback(SFARK_CBID_DisplayNotes, MyDisplayNotes);
	//sfArkSetCallback(SFARK_CBID_GetLicenseAgreement, MyGetLicenseAgreement);
	sfArkSetCallback(SFARK_CBID_UpdateProgress, MyUpdateProgress);

	// Check the command-line parameters were specified...
	if (argc != 3)
	{
		printf("usage: sfArkLibTest <Source.sfArk> <Target.SF2>\n");
		return 1;
	}

	// Now decompress the file...
	printf("Decompressing %s to %s...\n", argv[1], argv[2]);

	if (sfArkDecompress(argv[1], argv[2]))
	{
		printf("Successful!\n");	
		return 0;
	}
	else
	{
		int err = sfArkGetLastError();
		printf("Failed!  Error: %d %s\n", sfArkGetLastError(), sfArkGetLastErrorText());
		return 2;
	}
}

