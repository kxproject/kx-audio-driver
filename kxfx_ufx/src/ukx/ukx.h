#ifndef _UKX_H_
#define _UKX_H_      

// .................................. 
#include "../ufw/ufw.h"

// .................................. 
#include "ukxsdk.h"

#pragma warning(default: 4263) // member function does not override any base class virtual member function

// ..................................  

class uPlugin;
class uPluginView;
#include "uplgboxview.h"
#include "usettings.h"
#include "uplugin.h"
#include "uplgview.h"    
#include "upresetman.h"
#include "uplgcont.h"
#ifndef NO_Z
#include "uplgboxcont.h"
#endif

// ..................................  

#define ZM(p) memset(p, 0, sizeof(p));

// ..................................

#endif // _UKX_H_