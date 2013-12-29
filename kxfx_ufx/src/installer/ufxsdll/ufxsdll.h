
#ifndef UFXSETUP_DLL_H
#define UFXSETUP_DLL_H

extern "C"
#ifdef UFXSETUP_EXPORTS
__declspec(dllexport)
#endif

int entry(int func); // the only dll function 

// UFXSETUP functions:

#define UFXSETUP_FUNC_CHK_VERSION   0 // checks for kx and its version
#define UFXSETUP_FUNC_PRE           1 // close kxmixer
#define UFXSETUP_FUNC_DO            3 // does the job (fixing eqg10 name)
#define UFXSETUP_FUNC_POST          2 // start kxmixer

// error codes:

#define UFXSETUP_ERROR_NO_ERROR     0
#define UFXSETUP_ERROR_WRONG_FUNC   1 
#define UFXSETUP_ERROR_NO_KX        2   
#define UFXSETUP_ERROR_OLD_KX       3
#define UFXSETUP_ERROR_UNEXPECTED  -1

#endif  // UFXSETUP_DLL_H
