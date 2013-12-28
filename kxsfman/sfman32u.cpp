// kX SoundFont Manager (sfman32)
// Copyright (c) Eugene Gavrilov, 2004.
// All rights reserved

/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */


// sfman.h-based stuff [seems to be useless].

typedef struct SFMANL100uTAG {

    LRESULT (CDECL *SF_GetVersion) ( PDWORD ) ;
    LRESULT (CDECL *SF_Open)       ( DWORD ) ;
    LRESULT (CDECL *SF_Close)      ( DWORD )        ;

    LRESULT (CDECL *SF_GetNumDevs) ( PWORD ) ;
    LRESULT (CDECL *SF_GetDevCaps) ( WORD , PSFDEVOBJECT10u ) ;
    LRESULT (CDECL *SF_GetDevID)   ( WORD, PDWORD ) ;
    LRESULT (CDECL *SF_QueryStaticMemorySize) ( DWORD, PDWORD, PDWORD ) ;

    LRESULT (CDECL *SF_QuerySynSupport) ( DWORD , PSFBUFFEROBJECT ) ;
    LRESULT (CDECL *SF_GetSynBank) ( DWORD , PSFBUFFEROBJECT ) ;
    LRESULT (CDECL *SF_SelectSynBank) ( DWORD, WORD ) ;

    LRESULT (CDECL *SF_LoadUserBank) ( DWORD , SFMIDIBANK, PSFBUFFEROBJECT ) ;
    LRESULT (CDECL *SF_GetUserBank) ( DWORD, SFMIDIBANK, PSFBUFFEROBJECT ) ;
    LRESULT (CDECL *SF_ClearUserBank) ( DWORD, SFMIDIBANK) ;
    LRESULT (CDECL *SF_GetUBankParent) ( DWORD, PSFMIDIBANK ) ;

    LRESULT (CDECL *SF_LoadUserPreset) ( DWORD, PPARAMOBJECT, PSFBUFFEROBJECT ) ;
    LRESULT (CDECL *SF_ClearUserPreset) ( DWORD , SFMIDIBANK, WORD ) ;
    LRESULT (CDECL *SF_GetUserPreset) ( DWORD, PPARAMOBJECT, PSFBUFFEROBJECT ) ;

    LRESULT (CDECL *SF_IsDeviceFree) ( DWORD ) ;
} SFMANL100uAPI, *PSFMANL100uAPI;

LRESULT CDECL SF_GetDevCaps10u(SFDEVINDEX ndx, CSFDevObject10u *caps)
{
 check_dev(ndx);
 check(caps);

 if(caps && caps->m_SizeOf==sizeof(CSFCapsObject)) // actually, 10x, not '10u'
 {
  debug("kx sfman32: trying caps 1.01, not 1.00\n");
  return SF_GetDevCaps10x(ndx,(CSFCapsObject *)caps);
 }

 CSFDevObject10x dev;
 memset(&dev,0,sizeof(dev));
 dev.m_SizeOf=SFDEV_CAPSIZE_102; // sizeof(CSFDevObject)

 LRESULT ret=call_driver(ndx,SFDEV_GET_DEVICE_CAPS,device_handles[ndx].subdev,(LPARAM)&dev);
 if(ret==SFERR_NOERR)
 {
  if(caps && caps->m_SizeOf>=sizeof(CSFDevObject10u))
  {
   memset(caps,0,sizeof(CSFDevObject10u));

   caps->m_SizeOf=sizeof(CSFDevObject10u);
   caps->m_DevID=ndx;
   caps->m_DevCaps=dev.m_DevCaps;
   caps->m_RomID=dev.m_RomId;   /* ID for a Sound ROM used in a device      */
   caps->m_RomVer=dev.m_RomVer; /* Version for a Sound ROM used in a device */
   strncpy(caps->m_DevName,dev.m_DevName,sizeof(caps->m_DevName));      /* MMSystem name for MIDI Output Device     */

   return ret;
  }

  return SFERR_RESOURCE_INSUFFICIENT;
 }
 else return ret;
}

LRESULT CDECL SF_IsDeviceFree10u  ( DWORD ndx) 
{
 return SF_IsDeviceFree10x((SFDEVINDEX)ndx);
}

LRESULT CDECL SF_Open10u          ( DWORD ndx ) 
{
 debug("kx sfman32: open (10u): %x\n",ndx); // FIXME NOW!!!
 return SF_Open10x((SFDEVINDEX)ndx);
}

LRESULT CDECL SF_Close10u         ( DWORD ndx) 
{
 return SF_Close10x((SFDEVINDEX)ndx);
}

LRESULT CDECL SF_QueryStaticSampleMemorySize10u ( DWORD ndx, PDWORD max_mem, PDWORD avl_mem)
{
 return SF_QueryStaticSampleMemorySize10x((SFDEVINDEX)ndx,max_mem,avl_mem);
}

LRESULT CDECL SF_GetAllSynthEmulations10u ( DWORD ndx, CSFBufferObject *b) 
{
 return SF_GetAllSynthEmulations10x((SFDEVINDEX)ndx,b);
}


LRESULT CDECL SF_GetSynthEmulation10u     ( DWORD ndx, CSFBufferObject *b) 
{
 return SF_GetSynthEmulation10x((SFDEVINDEX)ndx,b);
}


LRESULT CDECL SF_SelectSynthEmulation10u  ( DWORD ndx, WORD emu) 
{
 return SF_SelectSynthEmulation10x((SFDEVINDEX)ndx,emu);
}

LRESULT CDECL SF_LoadBank10u  ( DWORD ndx, SFMIDIBANK p,CSFBufferObject *b) 
{
 CSFMIDILocation m;
 m.m_BankIndex=p.m_BankNum; 
 return SF_LoadBank10x((SFDEVINDEX)ndx,&m,b);
}

LRESULT CDECL SF_ClearLoadedBank10u ( DWORD ndx, SFMIDIBANK b) 
{
 CSFMIDILocation m;
 m.m_BankIndex=b.m_BankNum;

 return SF_ClearLoadedBank10x((SFDEVINDEX)ndx,&m);
}

LRESULT CDECL SF_LoadPreset10u ( DWORD ndx, CParamObject *pp,CSFBufferObject *b)
{
 CSFMIDILocation p1;
 CSFMIDILocation p2;

 // FIXME!!! this is invalid -- no specs, however
 p1.m_BankIndex=pp->m_UBankIndex.m_BankNum;
 p2.m_BankIndex=pp->m_UBankIndex.m_BankNum;
 p1.m_PresetIndex=pp->m_PresetIndex;
 p2.m_PresetIndex=pp->m_PresetIndex;

 return SF_LoadPreset10x((SFDEVINDEX)ndx,&p1,&p2,b);
}

LRESULT CDECL SF_GetUserPreset10u( DWORD ndx, CParamObject *pp, CSFBufferObject *b)
{
 CSFMIDILocation p1;

 // FIXME!!! this is invalid -- no specs, however
 p1.m_BankIndex=pp->m_UBankIndex.m_BankNum;
 p1.m_PresetIndex=pp->m_PresetIndex;

 return SF_GetLoadedPresetDescriptor((SFDEVINDEX)ndx,&p1,b);
}

LRESULT CDECL SF_ClearLoadedPreset10u (DWORD ndx, SFMIDIBANK b, WORD preset) 
{
 CSFMIDILocation p;
 p.m_BankIndex=b.m_BankNum;
 p.m_PresetIndex=preset;

 return SF_ClearLoadedPreset10x((SFDEVINDEX)ndx,&p);
}


LRESULT CDECL SF_GetVersion( DWORD *v)
{
 if(v)
  *v=0x20002; // FIXME !!! no specs

 return SFERR_NOERR;
}

LRESULT CDECL SF_GetDevID(WORD ndx, DWORD *id)
{
 if(id)
  *id=(DWORD)ndx; // FIXME !!! no specs

 debug("kx sfman32: getdevid [api 1.0]: %x/%x for %d\n",id,id?*id:0,ndx);

 return SFERR_NOERR;
}

LRESULT CDECL SF_GetUBankParent10u( DWORD ndx, PSFMIDIBANK bnk)
{
 return SF_IsMIDIBankUsed((SFDEVINDEX)ndx, &bnk->m_BankNum);
}

SFMANL100uAPI list100u=
{
    SF_GetVersion,
    SF_Open10u,
    SF_Close10u,

    SF_GetNumDevs,
    SF_GetDevCaps10u,
    SF_GetDevID,
    SF_QueryStaticSampleMemorySize10u,

    SF_GetAllSynthEmulations10u,
    SF_GetSynthEmulation10u,
    SF_SelectSynthEmulation10u,

    SF_LoadBank10u,
    SF_GetUserBank10u,
    SF_ClearLoadedBank10u,
    SF_GetUBankParent10u,

    SF_LoadPreset10u,
    SF_ClearLoadedPreset10u,
    SF_GetUserPreset10u,

    SF_IsDeviceFree10u
};
