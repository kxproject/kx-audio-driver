// Header for sfArkDLL
// copyright 1998-2001, Andy Inman, andyi@melodymachine.com

#if !defined(SFARKDLL_H)	// Make sure we only get included once
#define SFARKDLL_H

// SFARKDLL_EXPORTS should be UNDEFINED for applications using sfArkDLL
#ifdef SFARKDLL_EXPORTS
	#define SFARKDLL_API __declspec(dllexport)
#else
	#define SFARKDLL_API __declspec(dllimport)
#endif

// Some max sizes (use these, don't change them)...
#define	SFARK_MAX_FILENAME	256							// Longest single filename handled (or directory name)
#define	SFARK_MAX_FILEPATH	1024						// Longest full path handled
#define	SFARK_MAX_MSGTEXT	(SFARK_MAX_FILEPATH + 1024)	// Longest message sfArkShowMsg() must handle

// Error codes...
#define SFARKLIB_SUCCESS			0	// No error
#define	SFARKLIB_ERR_INIT			-1	// Failed to initialise
#define	SFARKLIB_ERR_MALLOC			-2	// Failed to allocate memory
#define	SFARKLIB_ERR_SIGNATURE		-3	// header does not contain "sfArk" signature (corrput, or not a sfArk file)
#define	SFARKLIB_ERR_HEADERCHECK	-4	// sfArk file has a corrupt header (looks like a sfArk file but is corrupt)
#define	SFARKLIB_ERR_INCOMPATIBLE	-5	// sfArk file is incompatible (e.g. from sfArk V1.x)
#define	SFARKLIB_ERR_UNSUPPORTED	-6	// sfArk file uses unsupported feature (some features supported by sfArk *may* not be supported by sfArkDLL)
#define	SFARKLIB_ERR_CORRUPT		-7	// got invalid compressed data (file is corrupted)
#define	SFARKLIB_ERR_FILECHECK		-8	// file checksum failed (file is corrupted)
#define	SFARKLIB_ERR_FILEIO			-9	// File i/o error (e.g. out of disk spcae, invalid drive/directory, check OS error)
#define	SFARKLIB_ERR_LICENSE		-10 // License included not agreed by user
#define	SFARKLIB_ERR_PARAMETER		-11 // Application called DLL function with invalid parameters
#define	SFARKLIB_ERR_UNDEFINED		-12 // Shouldn't happen

// Flags used with sfArkShowMessage() function...
#define	SFARK_MSG_SameLine		(1 << 0)	// Overwrite previous line of text with this one
#define	SFARK_MSG_AppendLine	(1 << 1)	// Append to previous line of text
#define	SFARK_MSG_PopUp			(1 << 2)	// "pop-up",to alert user - used for errors etc.

// sfArk functions exported by the DLL...
// The following will be supported in all future sfArkDLL versions...
SFARKDLL_API unsigned short sfArkGetLibVersion(void);				// Get version number of sfArk library
SFARKDLL_API unsigned short sfArkGetDLLVersion(void);				// Get version number of sfArkDLL
SFARKDLL_API unsigned short sfArkGetAPIVersion(void);				// Get version number of sfArkDLL API

// The following *may* change with future DLL versions...
SFARKDLL_API bool sfArkCompressionSupported(void);					// Determine if compression is supported
SFARKDLL_API long sfArkSetCallback(int CallbackId, void *callback); // Set callback address
SFARKDLL_API long sfArkDecompress(LPCSTR sfArkFile, LPCSTR sf2File);// Decompress a file
SFARKDLL_API long sfArkCompress(LPCSTR sf2File, LPCSTR sfArkFile);	// Compress a file
SFARKDLL_API long sfArkGetLastError(void);							// Get last error
SFARKDLL_API LPCSTR sfArkGetLastErrorText(void);					// Get last error text
SFARKDLL_API void sfArkTerminate(void);								// Terminate DLL (doesn't unload DLL)

// Typedefs for Callback functions...
typedef void __cdecl SFARK_CB_ShowMessage(LPCSTR MessageText, int Flags);	// Message display function
typedef void __cdecl SFARK_CB_UpdateProgress(int ProgressPercent);			// Progress indication
typedef bool __cdecl SFARK_CB_GetLicenseAgreement(LPCSTR LicenseText);	// Display/confirm license
typedef void __cdecl SFARK_CB_DisplayNotes(LPCSTR NotesFileName);		// Display notes text file

// ID numbers for Callbacks (used with sfArkSetCallback)...
#define	SFARK_CBID_ShowMessage			0
#define	SFARK_CBID_UpdateProgress		1
#define	SFARK_CBID_GetLicenseAgreement	2
#define	SFARK_CBID_DisplayNotes			3



#endif // SFARKDLL_H
