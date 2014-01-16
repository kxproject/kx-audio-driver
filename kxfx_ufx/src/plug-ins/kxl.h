
#ifndef _KXL_H_
#define _KXL_H_   

#include "../ufxkx.h"     

// ............................................................................  

#define KXPLUGIN_EXPORT extern "C" int __declspec(dllexport) __cdecl 

typedef int (*plugin_entry_t)(int, uintptr_t*);  
 
// ............................................................................

#endif // _KXL_H_