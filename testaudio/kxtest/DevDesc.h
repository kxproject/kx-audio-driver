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


// DevDesc.h : header file
//

#pragma once

#include "ks\ks.h"
#include "asio\asiolist.h"
#include "asio\iasiodrv.h"

// device modes
//
#define	DRV_MM		0
#define	DRV_DS		1
#define	DRV_KS		2
#define	DRV_ASIO	3
#define	DRV_KX		4

// format descriptions
//
#define FMT_NONE	0
#define FMT_STD		1		// WAVEFORMATEX
#define FMT_EXT		2		// WAVEFORMATEXTENSIBLE

#define SR_44100	0		// sample rates
#define SR_48000	1
#define SR_96000	2
#define SR_192000	3

#define BD_16		0		// bit depths
#define BD_24		1
#define BD_24_32	2		// 24 padded to 32
#define BD_32		3

extern const UINT sr2PbId[];	// maps CDeviceDesc::m_Fmt to dialog controls
extern const UINT sr2RecId[];
extern const UINT bd2PbId[];
extern const UINT bd2RecId[];

extern const DWORD i2sr[];		// maps CDeviceDesc::m_Fmt indices to real values
extern const WORD i2bd1[];
extern const WORD i2bd2[];

#define Zero(s)		ZeroMemory(&s, sizeof(s))

void FillFmt(WAVEFORMATEXTENSIBLE& wFmt, DWORD sr, WORD bps, WORD vbps, BOOL bExt);

class CDeviceDesc: public CObject
{
	DECLARE_DYNAMIC(CDeviceDesc)
public:
	TCHAR	m_Name[256];
	int		m_Fmt[4][4];	// 44.1,48,96,192 x 16,24,24(32),32

	CDeviceDesc(LPCTSTR szName)
	{
		ZeroMemory(&m_Fmt, sizeof(m_Fmt));
		_tcscpy_s(m_Name, szName);
	}

	BOOL FindFirstFormat(int* sr = NULL, int* bd = NULL);
	BOOL FindClosestMatch(int& sr, int& bd);
};

class CDescList: public TList<CDeviceDesc>
{
public:
	~CDescList()
	{
		CDeviceDesc* pItem;

		while(RemoveHead(&pItem))
			delete pItem;
	}
};

class CMmDesc: public CDeviceDesc
{
	DECLARE_DYNAMIC(CMmDesc)
public:
	UINT	m_ID;

	CMmDesc(UINT nId, LPCTSTR szName)
		:CDeviceDesc(szName)
		,m_ID(nId)
	{
	}
};

class CDsDesc: public CDeviceDesc
{
	DECLARE_DYNAMIC(CDsDesc)
public:
	GUID	m_GUID;

	CDsDesc(LPGUID pGuid, LPCTSTR szName)
		:CDeviceDesc(szName)
	{
		memcpy(&m_GUID, pGuid, sizeof(m_GUID));
	}
};

class CKsDesc: public CDeviceDesc
{
	DECLARE_DYNAMIC(CKsDesc)
public:
	TCHAR	m_FilterPath[MAX_PATH];

	CKsDesc(CKsFilter* pFilter)
		:CDeviceDesc(pFilter->m_FriendyName)
	{
		_tcscpy_s(m_FilterPath, pFilter->m_szFilterName);
	}
};

class CAsioDesc: public CDeviceDesc
{
	DECLARE_DYNAMIC(CAsioDesc)
public:
	int				m_DrvId;
	int				m_Channel;
	ASIOSampleType	m_Type;
	ASIOClockSource	m_ClockSources[16];
	long			m_NumCS;
	long			m_CS;

	CAsioDesc(int nId, int nCh, ASIOSampleType type, LPCTSTR szName)
		: CDeviceDesc(szName)
		, m_DrvId(nId)
		, m_Channel(nCh)
		, m_Type(type)
	{
	}
};

class CKxDesc: public CDeviceDesc
{
	DECLARE_DYNAMIC(CKxDesc)
public:
	UINT	m_ID;
	iKX		m_iKX;
	int		m_Plugin;
	int		m_LPbGpr, m_LRecGpr;
	int		m_RPbGpr, m_RRecGpr;

	CKxDesc(UINT nId, LPCTSTR szName, int nPlugin, int niL, int noL, int niR, int noR)
		:CDeviceDesc(szName)
		,m_ID(nId)
		,m_Plugin(nPlugin)
		,m_LPbGpr(noL)
		,m_LRecGpr(niL)
		,m_RPbGpr(noR)
		,m_RRecGpr(niR)
	{
		m_iKX.init(m_ID);
	}
};
