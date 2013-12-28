//sfArkLib String functions
// copyright 1998-2000 Andy Inman, andyi@melodymachine.com

#include <string.h>

const char *GetFileExt(const char *FileName)
{
  // return pointer to (final) file extension including the dot e.g. '.txt'
  const char *p;

  for(p = FileName + strlen(FileName); p > FileName &&  *p != '.'; p--) {}
  if (*p == '.')  p = FileName + strlen(FileName);
	return p;
}

