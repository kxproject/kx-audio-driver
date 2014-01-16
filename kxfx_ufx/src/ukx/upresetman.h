
// This is definitely the most stupid code i have ever seen.

#ifndef _U_PRESETMAN_H_
#define _U_PRESETMAN_H_

#define UPM_ERROR_UNKNOWN			-1
#define UPM_ERROR_BADNAME			-2
#define UPM_ERROR_FACTORYREPLACE	-3
#define UPM_ERROR_NOTFOUND			ERROR_NO_MORE_ITEMS

#define PRESETID_FACTORY	'tcaf'
#define PRESETID_USER		'resu'

class uPresetManager  
{
public:
	uPresetManager();
	virtual ~uPresetManager();

	int RemovePreset(const char* presetname);
	int WritePreset(const char* presetname, int nparams, const int* params);
	int ReadPreset(const char* presetname, int nparams, int* params, int* flags = NULL);
	int EnumPresets(int index, int maxnamesize, char* presetname);
	int WriteFactory(int npresets, int nparams, const char* data);
	int SelectPlugin(const char* guid, const char* name = NULL, const char* path = NULL);
	int EnumPlugins(int index, char* guid, int maxnamesize = NULL, char* name = NULL);

	int ExportPresets(HWND parent, FILE* file);
	int ImportPresets(HWND parent, FILE* file);
	
private:
	char plgname[128];
	char key[256];
	char chbuf[620];
	int* lbuf;

private:
	int	FindUnicumPresetName(char* unicum, const char* base);
};

template <int N>
struct Preset
{
    const char* name;
    int     value[N];
};

#endif // _U_PRESETMAN_H_
