
//.............................................................................

#include "ukx.h"
// #include "_stddbg.h"
#include "upresetman.h"

//.............................................................................

typedef unsigned char byte;

#define PRESET_PREFIX		'#'

#define KXGUID_LENGTH		37
#define KEY_KXPLUGINS		"SOFTWARE\\More kX\\Plugins\\"

#define _openkey(hkey, access, id)												\
	int rv = RegCreateKeyEx(HKEY_CURRENT_USER, key, NULL,						\
		NULL, 0, KEY_ALL_ACCESS, NULL, &hkey, NULL);							\
	if (rv) {																	\
		trace("<kxu> <%s> " #id ", key open failed [%x]\n", plgname, rv);	\
		return rv;																\
	}
// end of _openkey

#define _closekey(hkey) if (hkey) RegCloseKey(hkey);

//.............................................................................

uPresetManager::uPresetManager()
{
	lbuf = (int*) chbuf;
}

uPresetManager::~uPresetManager()
{
}

int uPresetManager::EnumPlugins(int index, char* guid, int maxnamesize, char* name)
{
	HKEY hKey = 0;
	HKEY hSubKey = 0;
	char* buf = guid;
	DWORD keylen = KXGUID_LENGTH;

	if (buf == NULL) buf = chbuf;
	int rv = RegCreateKeyEx(HKEY_CURRENT_USER, KEY_KXPLUGINS, NULL,	
		NULL, 0, KEY_READ, NULL, &hKey, NULL);
	if (rv) return rv;

	rv = RegEnumKeyEx(hKey, index, buf, &keylen, NULL, NULL, NULL, NULL);
	
	if ((rv == ERROR_SUCCESS) & (keylen == (KXGUID_LENGTH - 1)))
	{ 
		int rv = RegCreateKeyEx(hKey, buf, NULL,	
			NULL, 0, KEY_READ, NULL, &hSubKey, NULL);
		if (rv) rv = ERROR_NO_MORE_ITEMS;
		if ((rv == ERROR_SUCCESS) & (name != NULL))
		{
			rv = RegQueryValueEx(hSubKey, "name", 0, NULL, (byte*) name, (DWORD*) &maxnamesize);
		}
	}

	_closekey(hKey);
	return rv;
}

int uPresetManager::SelectPlugin(const char* guid, const char* name, const char* path)
{
	strcpy(key, KEY_KXPLUGINS); strcat(key, guid);
	HKEY hKey = 0;
	_openkey(hKey, KEY_ALL_ACCESS, uPresetManager::SelectPlugin);

	if (name != NULL)
	{
		strcpy(plgname, name);
		rv = RegSetValueEx(hKey, "name", NULL, REG_SZ, 
			(byte*) plgname, DWORD(strlen(plgname))); 
		ASSERT(!rv);
	}

	if (path != NULL)
	{
		strcpy(chbuf, path);
		rv = RegSetValueEx(hKey, NULL, NULL, REG_SZ, 
			(byte*) chbuf, DWORD(strlen(chbuf))); 
		ASSERT(!rv);
	}

	_closekey(hKey);
	return 0;
}

int uPresetManager::WriteFactory(int npresets, int nparams, const char* data)
{
	HKEY hKey = 0;
	_openkey(hKey, KEY_WRITE, uPresetManager::WriteFactory());

    lbuf[0] = PRESETID_FACTORY;
    char name[256] = {PRESET_PREFIX};
    const int presetsize = nparams * sizeof(int);

    // copy and write first preset as default
    memcpy(lbuf + 1, data + sizeof(const char*), presetsize);
    RegSetValueEx(hKey, "#[Default]", 0, REG_BINARY, 
        (byte*) lbuf, presetsize + sizeof(PRESETID_FACTORY)); 

	// write all presets
	for (int i = 0; i < npresets; i++)
	{	
        strncpy(name + 1, *(const char**) data, sizeof(name) - 1);
        data += sizeof(const char*);
        memcpy(lbuf + 1, data, presetsize);
        data += presetsize;
        RegSetValueEx(hKey, name, 0, REG_BINARY, 
            (byte*) lbuf, presetsize +  + sizeof(PRESETID_FACTORY));
	}

	_closekey(hKey);
	return 0;
}

int uPresetManager::EnumPresets(int index, int maxnamesize, char *presetname)
{
	ASSERT(presetname);
	HKEY hKey = 0;
	_openkey(hKey, KEY_WRITE, uPresetManager::EnumPresets());
	rv = 0;
	int i = 0, presetindex = 0, namesize;
	DWORD type;

	while (!rv)
	{
		type = 0;
		namesize = maxnamesize;
		chbuf[0] = 0;
		rv = RegEnumValue(hKey, i, chbuf, (DWORD*) &namesize, 0, &type, NULL, NULL);
		if ((type == REG_BINARY) & (chbuf[0] == PRESET_PREFIX))
		{
			if (presetindex == index) {strcpy(presetname, &chbuf[1]); break;}
			presetindex++;
		}
		i++;
	}
	_closekey(hKey);
	return rv;
}

int uPresetManager::ReadPreset(const char* presetname, int nparams, int *params, int* flags)
{
	ASSERT(presetname);
	ASSERT(params);
	HKEY hKey = 0;
	_openkey(hKey, KEY_WRITE, uPresetManager::ReadPreset());
	rv = 0;
	nparams = (nparams + 1) << 2; 
	int i = 0, namesize, paramsize;
	DWORD type;

	while (!rv)
	{
		type = 0;
		paramsize = nparams;
		namesize = sizeof chbuf;
		chbuf[0] = 0; chbuf[1] = 0;
		rv = RegEnumValue(hKey, i, chbuf, (DWORD*) &namesize, 0, 
			&type, (byte*) &lbuf[64], (DWORD*) &paramsize);
		if (rv == ERROR_MORE_DATA) rv = 0;
		if ((type == REG_BINARY) & (chbuf[0] == PRESET_PREFIX))
		{
			if (!_strcmpi(presetname, &chbuf[1])){
				memcpy(params, &lbuf[65], paramsize - 4);
				if (flags) *flags = lbuf[64];
				break;
			}	
		}
		i++;
	}
	_closekey(hKey);
	//_dbg_check_print((void*) !rv, "<kxu> <%s> uPresetManager::ReadPreset(), preset not found (%s)\n", plgname, presetname);
	return rv;
}

int uPresetManager::WritePreset(const char *presetname, int nparams, const int *params)
{
	ASSERT(presetname);
	ASSERT(params);
	HKEY hKey = 0;
	_openkey(hKey, KEY_WRITE, uPresetManager::WritePreset());
	
	nparams <<= 2;
	int presetsize  = nparams + 4;	

	chbuf[presetsize] = PRESET_PREFIX;
	strcpy(&chbuf[presetsize + 1], presetname);

	rv = RegQueryValueEx(hKey, &chbuf[presetsize], NULL, 
		NULL, (byte*) lbuf, (DWORD*) &presetsize);
	if ((rv == 0) && (lbuf[0] == PRESETID_FACTORY)) {rv = UPM_ERROR_FACTORYREPLACE; goto exit;}
	memcpy(&lbuf[1], params, nparams);
	lbuf[0] = PRESETID_USER;
	presetsize  = nparams + 4;
	rv = RegSetValueEx(hKey, &chbuf[presetsize], NULL, 
		REG_BINARY, (byte*) lbuf, presetsize);
	ASSERT(!rv);
exit:
	_closekey(hKey);
	return rv;
}

int uPresetManager::RemovePreset(const char* presetname)
{
	ASSERT(presetname);
	HKEY hKey = 0;
	_openkey(hKey, KEY_WRITE, uPresetManager::RemovePreset());

	chbuf[0] = PRESET_PREFIX;
	strcpy(&chbuf[1], presetname);
	rv = RegDeleteValue(hKey, chbuf);

	_closekey(hKey);
	//_dbg_check_print((void*) !rv, "<kxu> <%s> uPresetManager::RemovePreset(), preset not found (%s)\n", plgname, presetname);
	return rv;
}

#define BR	"\n"

int uPresetManager::ExportPresets(HWND /* parent */, FILE* file)
{
	fprintf(file, "UPP1.0 ; UFX Plugin Presets File" BR);
	fprintf(file, BR "; %s" BR, plgname);
	int i = int(strlen(KEY_KXPLUGINS));
	fprintf(file, "guid %s" BR, &key[i]);

	i = 0;
	int j;
	int exppresetscount = 0;
	char str[128];
	int data[128];
	int flag;
	while (!EnumPresets(i, 128, str))
	{
		FillMemory(data, 128, 0x88);
		ReadPreset(str, 128, data, &flag);
		if (flag == PRESETID_USER)
		{
			fprintf(file, "\"%s\"", str);
			j = 0;
			while (data[j] != 0x88888888)
			{
				fprintf(file, ", %i", data[j]);
				j++;
			}
			exppresetscount++;
			fprintf(file, ";" BR);
		}
		i++;
	}

	return exppresetscount;
}

int get_upp_line(FILE* file, char** line)
{
	int rv;
	char* rc;
	char* str = line[0];
	str[0] = 0;
	if ((rv = fscanf(file, "%[^\n]", str)) == EOF) return EOF;
	fgetc(file);
	if ((rc = strrchr(str, ';')) != NULL) rc[0] = 0;
	while ((str[0] == ' ') || (str[0] == '\t') || (str[0] == '\10')) str++;
	if (str[0] != 0) 
	{
		if (rc == NULL) rc = str + strlen(str); 
		rc--;
		while ((rc[0] == ' ') || (rc[0] == '\t') || (rc[0] == '\10')) rc--;
		rc[1] = 0;
	}
	line[0] = str;
	return rv;
}

#define BR "\n"

int uPresetManager::ImportPresets(HWND parent, FILE* file)
{
	int data[128];
	int data2[128];
	int  nparams = 0;
	int flag;
	
	int  imppresetscount = 0;
	int  validplugin = 0;
	char str[512];
	char* line = str;
	char* token;
	char* presetname;


	while (get_upp_line(file, &line) != EOF)
	{
		if (line[0])
		{
			if (_strcmpi(line, "UPP1.0")) goto error;
			break;
		}
	}

	while (get_upp_line(file, &line) != EOF)
	{
		if (line[0])
		{
			if ((line[0] == '"') && validplugin)
			{
				int r;

				line++;
				presetname = strtok(line, "\"");
				nparams = 0;
				line = strtok(NULL, "\0");
				do 
				{
					if((r = sscanf(line, " , %ld%[^\n]", &data[nparams], line)) < 1) goto error;
					nparams++;
				} while (r > 1);

				if (nparams)
				{
					flag = 0;
					r = ReadPreset(presetname, nparams, data2, &flag);
					
					if ((r != UPM_ERROR_NOTFOUND) && (memcmp(data, data2, nparams << 2)))
					{
						char text[256];

						if (flag == PRESETID_USER)
						{
							sprintf(text, "Preset with the name \"%s\" is already exist." BR
								"Do you want to replace it with one being imported?" BR BR
								"\"No\" - import new preset with the different name." BR
								"\"Cancel\" - abort import operation." BR, presetname);
							r = MessageBox(parent, text, "Import Presets", MB_ICONQUESTION | MB_YESNOCANCEL);
						}
						else
						{
							sprintf(text, "The \"%s\" is factory preset and it cannot be replaced with "
								"user preset being imported." BR
								"Do you want import new preset with the different name?" BR BR
								"(\"Cancel\" - abort import operation.)" BR, presetname);
							r = MessageBox(parent, text, "Import Presets", MB_ICONQUESTION | MB_YESNOCANCEL);
							r = r ^ (r >> 2);
						}

						switch (r)
						{
						case IDYES:
							break;

						case IDNO:
							FindUnicumPresetName(text, presetname);
							strcpy(presetname, text);
							break;

						case IDCANCEL:
							return imppresetscount;
						}
					}

					WritePreset(presetname, nparams, data);
					imppresetscount++;
				}
			}

			else
			{
				token = strtok(line, " \t\10");
				if (!_strcmpi(token, "guid"))
				{
					token = strtok(NULL, " \t\10");
					int i = int(strlen(KEY_KXPLUGINS));
					validplugin = !_strcmpi(token, &key[i]);
				}
				else goto error;
			}
			
		}
	}

	return imppresetscount;

error:
	MessageBox(parent, "Invalid format.", "Import Presets", MB_ICONEXCLAMATION);
	return imppresetscount;
}

int	uPresetManager::FindUnicumPresetName(char* unicum, const char* base)
{
	int data[64];
	int flag;
	int i = 2;
	char name[128];
	strcpy(name, base);
	int l = int(strlen(name)) - 1;
	while ((isdigit(name[l])) && (l > 2)) {name[l] = 0; l--;}
	if (name[l] == ' ') name[l] = 0;
	do {
		sprintf(unicum, "%s %i", name, i); i++;
	} while (ReadPreset(unicum, 64, data, &flag) != UPM_ERROR_NOTFOUND);
	return 0;
}
