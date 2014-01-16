
#ifndef _U_FRAMEWORK_H_
#define _U_FRAMEWORK_H_	

//.............................................................................

#include "dbg.h"
#include "../rsrc/ursrc.h"

//.............................................................................
#include "udrawlib.h"
#include "ucore.h"

kxuCore* uGetCore();
int uInit(); 
int uEnd();

// .......

// uColors
#define UC_WHITE	~0UL
#define UC_LIGHT	~1UL
#define UC_GREY		~2UL	// main
#define UC_DARK		~3UL
#define UC_BLACK	~4UL
#define UC_HOVER	~5UL	// default: ~yellow
#define UC_NEUTRAL	UC_HOVER
#define UC_GOOD		~6UL	// default: ~green
#define UC_BAD		~7UL	// default: ~red

//.............................................................................
#include "ucontrol.h"

// Controls 
#include "uecombo.h"
#include "ulabel.h" 
#include "ubutton.h"
#include "uaknob.h"
#include "uvfader.h"
#include "uhfader.h"
#include "uimgbutton.h"
#include "ucheckbox.h"
#include "utooltip.h"
#include "udrawbox.h"
#include "umeter.h"
#include "uenterdlg.h"

//.............................................................................

#endif // _U_FRAMEWORK_H_
