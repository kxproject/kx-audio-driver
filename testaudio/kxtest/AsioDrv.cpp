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


#include "stdafx.h"

#include "kxtest.h"
#include "kxtestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static void PbBufferSwitch(long, ASIOBool) {};

static void*				RecLBuffers[2];
static void*				RecRBuffers[2];
static ASIOSampleType		RecType;
static DWORD				RecLSample, RecRSample;
static CCriticalSection*	RecLock;

static inline WORD swap16(WORD a)
{
	return MAKEWORD(HIBYTE(a), LOBYTE(a));
}

static inline DWORD swap24(DWORD a)
{
	return (a & 0xFF00) | ((a & 0xFF0000) >> 16) | ((a & 0xFF) << 16);
}

static inline DWORD swap32(DWORD a)
{
	return MAKELONG(swap16(HIWORD(a)), swap16(LOWORD(a)));
}

static void RecBufferSwitch(long iBuf, ASIOBool)
{
	CAutoLock lock(*RecLock);

	switch (RecType)
	{
	case ASIOSTInt16LSB:
		RecLSample = ((DWORD)(*(WORD*)RecLBuffers[iBuf])) << 16;
		RecRSample = ((DWORD)(*(WORD*)RecRBuffers[iBuf])) << 16;
		break;

	case ASIOSTInt16MSB:
		RecLSample = ((DWORD)swap16(*(WORD*)RecLBuffers[iBuf])) << 16;
		RecRSample = ((DWORD)swap16(*(WORD*)RecRBuffers[iBuf])) << 16;
		break;

	case ASIOSTInt24LSB:
	case ASIOSTInt32LSB24:
		RecLSample = (*(DWORD*)RecLBuffers[iBuf] & 0xFFFFFF) << 8;
		RecRSample = (*(DWORD*)RecRBuffers[iBuf] & 0xFFFFFF) << 8;
		break;

	case ASIOSTInt24MSB:
	case ASIOSTInt32MSB24:
		RecLSample = swap24(*(DWORD*)RecLBuffers[iBuf] & 0xFFFFFF) << 8;
		RecRSample = swap24(*(DWORD*)RecRBuffers[iBuf] & 0xFFFFFF) << 8;
		break;

	case ASIOSTInt32LSB:
		RecLSample = *(DWORD*)RecLBuffers[iBuf];
		RecRSample = *(DWORD*)RecRBuffers[iBuf];
		break;

	case ASIOSTInt32MSB:
		RecLSample = swap32(*(DWORD*)RecLBuffers[iBuf]);
		RecRSample = swap32(*(DWORD*)RecRBuffers[iBuf]);
		break;
	}
};

static void sampleRateChanged(ASIOSampleRate) {};
static long asioMessages(long, long, void*, double*) { return 0; };
static ASIOTime* bufferSwitchTimeInfo(ASIOTime*, long, ASIOBool) { return 0; };

static ASIOCallbacks PbCB = {PbBufferSwitch, sampleRateChanged, asioMessages, bufferSwitchTimeInfo};
static ASIOCallbacks RecCB = {RecBufferSwitch, sampleRateChanged, asioMessages, bufferSwitchTimeInfo};

CAsioPbDrv::CAsioPbDrv(CkxtestDlg* pDlg)
	: CPbDriver(pDlg)
	, m_pDesc(NULL)
	, m_pIDrv(NULL)
	, m_bSleep(TRUE)
{
	ASSERT(m_pDlg);
}

CAsioPbDrv::~CAsioPbDrv()
{
	if (m_pIDrv)
	{
		m_pIDrv->disposeBuffers();
		m_pDlg->m_AsioDrivers.asioCloseDriver(m_pDesc->m_DrvId);
	}

	CoUninitialize();
}

BOOL CAsioPbDrv::Init()
{
	ASSERT(m_pDlg->m_PbDesc);
	ASSERT_KINDOF(CAsioDesc, m_pDlg->m_PbDesc);

	m_pDesc = (CAsioDesc*)m_pDlg->m_PbDesc;

	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	if ((0 != m_pDlg->m_AsioDrivers.asioOpenDriver(m_pDesc->m_DrvId, (LPVOID*)&m_pIDrv)) ||
		(ASIOTrue != m_pIDrv->init(m_pDlg->m_hwndMain)))
	{
		TRACE("ASIO playback init: can't open driver.\n");
		return FALSE;
	}

	long mins, maxs, gran;

	m_pIDrv->getBufferSize(&mins, &maxs, &m_Size, &gran);

	ASIOSampleRate sr;
	m_pIDrv->getSampleRate(&sr);

	if ((DWORD)sr != i2sr[m_pDlg->m_PbSR])
		m_pIDrv->setSampleRate(i2sr[m_pDlg->m_PbSR]);

	m_pIDrv->setClockSource(m_pDesc->m_CS);

	ASIOBufferInfo bi[2] =
	{
		ASIOFalse, m_pDesc->m_Channel, NULL, NULL,
		ASIOFalse, m_pDesc->m_Channel + 1, NULL, NULL
	};

	if (ASE_OK != m_pIDrv->createBuffers(bi, 2, m_Size, &PbCB))
	{
		TRACE("ASIO playback init: can't create buffers.\n");
		return FALSE;
	}

	m_L1 = bi[0].buffers[0];
	m_L2 = bi[0].buffers[1];
	m_R1 = bi[1].buffers[0];
	m_R2 = bi[1].buffers[1];

	return TRUE;
}

BOOL CAsioPbDrv::Start()
{
	ASSERT(m_pIDrv);

	return (ASE_OK == m_pIDrv->start()) ? TRUE : FALSE;
}

BOOL CAsioPbDrv::Stop()
{
	ASSERT(m_pIDrv);

	return (ASE_OK == m_pIDrv->stop()) ? TRUE : FALSE;
}

BOOL CAsioPbDrv::Update(DWORD dwLSample, DWORD dwRSample)
{
	AsioFillBuffer(m_L1, dwLSample);
	AsioFillBuffer(m_L2, dwLSample);
	AsioFillBuffer(m_R1, dwRSample);
	AsioFillBuffer(m_R2, dwRSample);

	if (m_bSleep)
		Sleep(PB_WAIT);

	return TRUE;
}

void CAsioPbDrv::AsioFillBuffer(void* pBuffer, DWORD dwSample)
{
	ASSERT(pBuffer);

	switch (i2bd1[m_pDlg->m_PbBD])
	{
	case 16: dwSample >>= 16;	break;
	case 24: dwSample >>= 8;	break;
	}

	DWORD64 dwBigBlock;
	WORD w;
	int i;

	switch (m_pDesc->m_Type)
	{
	case ASIOSTInt16MSB:
		dwSample = swap16((WORD)dwSample);
		// pass thru
	case ASIOSTInt16LSB:
		dwSample |= dwSample << 16;
		dwBigBlock = (DWORD64)dwSample | ((DWORD64)dwSample << 32);

		for (i = 0; i < m_Size / 4; i++)
			((DWORD64*)pBuffer)[i] = dwBigBlock;

		break;

	case ASIOSTInt24MSB:
		dwSample = swap24(dwSample);
		// pass thru
	case ASIOSTInt24LSB:
		w = (WORD)(dwSample >> 8);
		dwSample |= (dwSample << 24) & 0xFF000000;

		for (i = 0; i < m_Size / 2; i++)
		{
			*(DWORD*)(((BYTE*)pBuffer) + i * 6) = dwSample;
			*(WORD*)(((BYTE*)pBuffer) + i * 6 + 4) = w;
		}
		break;

	case ASIOSTInt32MSB24:
		dwSample = swap24(dwSample);
		goto fill_32;

	case ASIOSTInt32MSB:
		dwSample = swap32(dwSample);
		// pass thru
	case ASIOSTInt32LSB24:
	case ASIOSTInt32LSB:
	fill_32:
		dwBigBlock = (DWORD64)dwSample | ((DWORD64)dwSample << 32);

		for (i = 0; i < m_Size / 2; i++)
			((DWORD64*)pBuffer)[i] = dwBigBlock;
		break;
	}
}

CAsioRecDrv::CAsioRecDrv(CkxtestDlg* pDlg)
	: CRecDriver(pDlg)
	, m_pDesc(NULL)
	, m_pIDrv(NULL)
	, m_CoUsed(FALSE)
{
	ASSERT(m_pDlg);
}

CAsioRecDrv::~CAsioRecDrv()
{
	if (m_pIDrv)
	{
		m_pIDrv->disposeBuffers();
		m_pDlg->m_AsioDrivers.asioCloseDriver(m_pDesc->m_DrvId);
	}

	if (m_CoUsed)
		CoUninitialize();
}

BOOL CAsioRecDrv::Init()
{
	ASSERT(m_pDlg->m_RecDesc);
	ASSERT_KINDOF(CAsioDesc, m_pDlg->m_RecDesc);

	m_pDesc = (CAsioDesc*)m_pDlg->m_RecDesc;

	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	m_CoUsed = TRUE;

	if ((0 != m_pDlg->m_AsioDrivers.asioOpenDriver(m_pDesc->m_DrvId, (LPVOID*)&m_pIDrv)) ||
		(ASIOTrue != m_pIDrv->init(m_pDlg->m_hwndMain)))
	{
		TRACE("ASIO recording init: can't open driver.\n");
		return FALSE;
	}

	long mins, maxs, pref, gran;

	m_pIDrv->getBufferSize(&mins, &maxs, &pref, &gran);

	ASIOSampleRate sr;
	m_pIDrv->getSampleRate(&sr);

	if ((DWORD)sr != i2sr[m_pDlg->m_PbSR])
		m_pIDrv->setSampleRate(i2sr[m_pDlg->m_PbSR]);

	m_pIDrv->setClockSource(m_pDesc->m_CS);

	ASIOBufferInfo bi[2] =
	{
		ASIOTrue, m_pDesc->m_Channel, NULL, NULL,
		ASIOTrue, m_pDesc->m_Channel + 1, NULL, NULL
	};

	if (ASE_OK != m_pIDrv->createBuffers(bi, 2, pref, &RecCB))
	{
		TRACE("ASIO recording init: can't create buffers.\n");
		return FALSE;
	}

	memcpy(RecLBuffers, bi[0].buffers, sizeof(bi[0].buffers));
	memcpy(RecRBuffers, bi[1].buffers, sizeof(bi[1].buffers));

	RecType		= m_pDesc->m_Type;
	RecLock		= &m_Lock;

	return TRUE;
}

BOOL CAsioRecDrv::Start()
{
	ASSERT(m_pIDrv);

	return (ASE_OK == m_pIDrv->start()) ? TRUE : FALSE;
}

BOOL CAsioRecDrv::Stop()
{
	ASSERT(m_pIDrv);

	return (ASE_OK == m_pIDrv->stop()) ? TRUE : FALSE;
}

BOOL CAsioRecDrv::Update(DWORD &dwLRec, DWORD &dwRRec)
{
	m_Lock.Lock();

	dwLRec = RecLSample;
	dwRRec = RecRSample;

	m_Lock.Unlock();

	Sleep(REC_WAIT);
	return TRUE;
}

CAsioFDDrv::CAsioFDDrv(CkxtestDlg* pDlg)
	: CAsioPbDrv(pDlg)
	, CAsioRecDrv(pDlg)
{
	m_bSleep = FALSE;
}

BOOL CAsioFDDrv::Init()
{
	ASSERT(CAsioPbDrv::m_pDlg->m_PbDesc);
	ASSERT(CAsioPbDrv::m_pDlg->m_RecDesc);
	ASSERT_KINDOF(CAsioDesc, CAsioPbDrv::m_pDlg->m_PbDesc);
	ASSERT_KINDOF(CAsioDesc, CAsioPbDrv::m_pDlg->m_RecDesc);

	CAsioPbDrv::m_pDesc = (CAsioDesc*)CAsioPbDrv::m_pDlg->m_PbDesc;
	CAsioRecDrv::m_pDesc = (CAsioDesc*)CAsioPbDrv::m_pDlg->m_RecDesc;

	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	if ((0 != CAsioPbDrv::m_pDlg->m_AsioDrivers.asioOpenDriver(CAsioPbDrv::m_pDesc->m_DrvId, (LPVOID*)&(CAsioPbDrv::m_pIDrv))) ||
		(ASIOTrue != CAsioPbDrv::m_pIDrv->init(CAsioPbDrv::m_pDlg->m_hwndMain)))
	{
		TRACE("ASIO FD init: can't open driver.\n");
		return FALSE;
	}

	long mins, maxs, gran;

	CAsioPbDrv::m_pIDrv->getBufferSize(&mins, &maxs, &(CAsioPbDrv::m_Size), &gran);

	ASIOSampleRate sr;
	CAsioPbDrv::m_pIDrv->getSampleRate(&sr);

	if ((DWORD)sr != i2sr[CAsioPbDrv::m_pDlg->m_PbSR])
		CAsioPbDrv::m_pIDrv->setSampleRate(i2sr[CAsioPbDrv::m_pDlg->m_PbSR]);

	CAsioPbDrv::m_pIDrv->setClockSource(CAsioPbDrv::m_pDesc->m_CS);

	ASIOBufferInfo bi[4] =
	{
		ASIOFalse,	CAsioPbDrv::m_pDesc->m_Channel,			NULL, NULL,
		ASIOFalse,	CAsioPbDrv::m_pDesc->m_Channel + 1,		NULL, NULL,
		ASIOTrue,	CAsioRecDrv::m_pDesc->m_Channel,		NULL, NULL,
		ASIOTrue,	CAsioRecDrv::m_pDesc->m_Channel + 1,	NULL, NULL
	};

	if (ASE_OK != CAsioPbDrv::m_pIDrv->createBuffers(bi, 4, CAsioPbDrv::m_Size, &RecCB))
	{
		TRACE("ASIO FD init: can't create buffers.\n");
		return FALSE;
	}

	CAsioPbDrv::m_L1 = bi[0].buffers[0];
	CAsioPbDrv::m_L2 = bi[0].buffers[1];
	CAsioPbDrv::m_R1 = bi[1].buffers[0];
	CAsioPbDrv::m_R2 = bi[1].buffers[1];

	memcpy(RecLBuffers, bi[2].buffers, sizeof(bi[2].buffers));
	memcpy(RecRBuffers, bi[3].buffers, sizeof(bi[3].buffers));

	RecType		= CAsioRecDrv::m_pDesc->m_Type;
	RecLock		= &(CAsioRecDrv::m_Lock);

	return TRUE;
}
