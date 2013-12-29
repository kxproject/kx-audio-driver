// ............................................................................

#ifndef _H_UITEST_
#define _H_UITEST_

// ............................................................................

class uApp : public CWinApp
{
public:
	uApp();

protected:
	iKXPlugin* plg;

public:
	virtual BOOL InitInstance();
	virtual BOOL ExitInstance();

	DECLARE_MESSAGE_MAP()
};

// ............................................................................

#endif // _H_UITEST_
