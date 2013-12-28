// sfArkLib	ZLIB interface
// Copyright 1998-2000 Andy Inman, andyi@melodymachine.com

#include	<stdio.h>
#include	<string.h>

#include	"wcc.h"
#include	"zlib.h"

extern "C" uLong __declspec(dllimport) _adler32 (uLong adler, const Bytef *buf, uInt len);
extern "C" int __declspec(dllimport) _uncompress (Bytef *dest,   uLongf *destLen,
                                   const Bytef *source, uLong sourceLen);

ULONG	UnMemcomp(const BYTE *InBuf, int InBytes, BYTE *OutBuf, int OutBufLen)
{
    // Uncompress buffer using ZLIBs uncompress function...
    ULONG	OutBytes = OutBufLen;
    int Result = _uncompress(OutBuf, &OutBytes, InBuf, InBytes);
    if (Result != Z_OK)				// uncompress failed?
    {
        printf("ZLIB uncompress failed: %d\n", Result);
        //sprintf(MsgTxt, "ZLIB uncompress failed: %d", Result);
        //msg(MsgTxt, MSG_PopUp);
        OutBytes = 0;
        if (Result == Z_MEM_ERROR)
            GlobalErrorFlag = SFARKLIB_ERR_MALLOC;
        else
            GlobalErrorFlag = SFARKLIB_ERR_CORRUPT;
    }

    return OutBytes;
}

