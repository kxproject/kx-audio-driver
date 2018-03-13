// kX Mixer / kX Audio Driver
// Copyright (c) Eugene Gavrilov, 2001-2018.
// All rights reserved

// this code is shared between kxmixer and kxsetup
// for simplicity, it is in a header file now

// force64/force32 explanation:
//   if kxmixer is 32-bit and needs to run 64-bit kxsetup, force64 is true
//   code should check if it is running on true 64-bit Windows, if yes, modify path accordingly (to point to 64-bit kxsetup.exe)
// for ASIO registration, we may need to force 32-bit path instead,
// when 64-bit kxsetup attempts to register 32-bit ASIO DLL

int get_mixer_folder(TCHAR *folder,const TCHAR *filename,bool force64,bool force32)
{
 #if defined(_WIN64)
	UNREFERENCED_PARAMETER(force64);
 #endif
 #if !defined(_WIN64)
	UNREFERENCED_PARAMETER(force32);
 #endif

 folder[0]=0;

 #if defined(_WIN64)
 	if(force32)
 	{
 		// override path, use 32-bit DLL instead:
		_stprintf(folder,_T("\"%s\\kX Project\\%s\""),getenv("ProgramFiles(x86)"),filename);
		return 0;
	}
 #endif	

 #if !defined(_WIN64)
 	if(force64)
 	{
 		// check if we are running on 64-bit Windows in 32-bit mode
 		typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
 		LPFN_ISWOW64PROCESS fnIsWow64Process;
 		fnIsWow64Process = (LPFN_ISWOW64PROCESS) GetProcAddress( GetModuleHandle(TEXT("kernel32")),"IsWow64Process");
 		if(NULL != fnIsWow64Process)
 		{
 			BOOL bIsWow64 = FALSE;
 			if (fnIsWow64Process(GetCurrentProcess(),&bIsWow64))
 			{
 				if(bIsWow64 && getenv("ProgramW6432"))
 				{
 					// all checks OK, we are running 32-bit app on a 64-bit Windows, need to construct path to 64-bit environment
 					_stprintf(folder,_T("\"%s\\kX Project\\%s\""),getenv("ProgramW6432"),filename);
 					return 0;
 				}
 			}
 		}
 	}
 #endif

 GetModuleFileName(NULL,folder,MAX_PATH);

 TCHAR *p=0;
 p = _tcsrchr(folder,'\\');
 if(p)
 {
     p++;
     if(_tcschr(p,'"')) // module file name is [ "c:\a\b c\d.exe" ]
     {
      *p=0;
      _tcscat(folder,filename);
      _tcscat(folder,_T("\""));
     }
     else // module file name is [ c:\a\b\c.exe ]
     {
      *p=0;
      _tcscat(folder,filename);
      if(_tcschr(folder,' ')) // contains spaces?
      {
      	TCHAR tmp[MAX_PATH];
      	tmp[0]='"';
      	_tcscpy(tmp+1,folder);
      	_tcscat(tmp,_T("\""));
      	_tcscpy(folder,tmp);
      }
     }
 }

 return 0;
}
