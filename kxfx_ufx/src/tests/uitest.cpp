
#include "stdafx.h"
#include "_kxu.h"
//#include "utestplg/eqg10.h"
#include "utestplg/eqp1.h"
//#include "utestplg/eqp5.h"
//#include "utestplg/rev2.h"
//#include "utestplg/testctrl.h"
//#include "utestplg/surcom.h"
//#include "utestplg/djcf.h"
//#include "utestplg/peak.h"
//#include "utestplg/tubedrive.h"
//#include "utestplg/triod.h"
//#include "utestplg/u128.h"
#include "uproto.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// uApp

BEGIN_MESSAGE_MAP(uApp, CWinApp)
END_MESSAGE_MAP()

uApp theApp;


BOOL uApp::InitInstance()
{
	Enable3dControls();
	uInit();

	plg = new Eqp1;

	uPluginContainer* upc = (uPluginContainer*) plg->create_cp(NULL);
	ASSERT(upc);
	plg->cp = upc;
	plg->set_defaults();

	m_pMainWnd = (CWnd*) upc;

	//upc->CenterWindow();
	upc->ShowWindow(SW_SHOW);

	return TRUE;
}

BOOL uApp::ExitInstance()
{ 
	delete plg;
	uEnd();

	return CWinApp::ExitInstance();
}
