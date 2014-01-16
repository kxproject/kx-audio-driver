#ifndef _U_SETTINGS_H_
#define _U_SETTINGS_H_

//.............................................................................

class uSettings
{
public:
	uSettings(uPlugin* plugin);
	~uSettings();
	
	int set(char* id, DWORD data); 
	int get(char* id, DWORD* data); 
	
private:
	char key[32];
	kSettings* kss;
};

//.............................................................................

#endif // _U_SETTINGS_H_
