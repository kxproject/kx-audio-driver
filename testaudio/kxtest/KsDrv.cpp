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

CKsPbDrv::CKsPbDrv(CkxtestDlg* pDlg)
	: CPbDriver(pDlg)
	, m_Filter(NULL)
	, m_Pin(NULL)
	, m_pBuffer(NULL)
{
	ASSERT(m_pDlg);
	m_Ovr.hEvent = INVALID_HANDLE_VALUE;
}

CKsPbDrv::~CKsPbDrv()
{
	if (m_Pin)
		m_Pin->ClosePin();

	if (m_Filter)
		delete m_Filter;

	if (INVALID_HANDLE_VALUE != m_Ovr.hEvent)
		CloseHandle(m_Ovr.hEvent);

	if (m_pBuffer)
		free(m_pBuffer);
}

BOOL CKsPbDrv::Init()
{
	ASSERT(m_pDlg->m_PbDesc);
	ASSERT_KINDOF(CKsDesc, m_pDlg->m_PbDesc);

	int nFormat = m_pDlg->m_PbDesc->m_Fmt[m_pDlg->m_PbSR][m_pDlg->m_PbBD];
	ASSERT(FMT_NONE != nFormat);

	FillFmt(m_Fmt, i2sr[m_pDlg->m_PbSR], i2bd1[m_pDlg->m_PbBD], i2bd2[m_pDlg->m_PbBD],
		(nFormat & FMT_EXT) ? TRUE : FALSE);

	CKsDesc& ksDesc = *reinterpret_cast<CKsDesc*>(m_pDlg->m_PbDesc);

	try
	{
		HRESULT r;

		if (!(m_Filter = new CKsAudRenFilter(ksDesc.m_FilterPath, &r)) ||
			(!SUCCEEDED(r)))
			throw "CKsAudRenFilter constructor";

		if (S_OK != m_Filter->Instantiate())
			throw "Instantiate()";

		if (S_OK != m_Filter->EnumeratePins())
			throw "EnumeratePins()";

		if (!(m_Pin = m_Filter->CreateRenderPin((WAVEFORMATEX*)&m_Fmt, TRUE)))
			throw "CreateRenderPin()";

	} catch (const char* error)
	{
		TRACE("KS playback init: %s failed.\n", error);
		return FALSE;
	}

	Zero(m_Header);

	m_Header.Size			= sizeof(m_Header);
	m_Header.FrameExtent	= BUFSIZE(KS_BUFFER, m_Fmt);
	m_Header.DataUsed		= m_Header.FrameExtent;
	m_Header.OptionsFlags	= KSSTREAM_HEADER_OPTIONSF_LOOPEDDATA;

	VERIFY(m_Header.Data = m_pBuffer = malloc(m_Header.FrameExtent));

	m_Ovr.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	return TRUE;
}

BOOL CKsPbDrv::Start()
{
	ASSERT(m_Filter);
	ASSERT(m_Pin);

	HRESULT r;
	
	if (S_OK != (r = m_Pin->SetState(KSSTATE_RUN)))
	{
		TRACE("KS playback init: SetState() failed, code 0x%x\n", r);
		return FALSE;
	}

	if (S_OK != (r = m_Pin->WriteData(&m_Header, &m_Ovr)))
	{
		TRACE("KS playback init: WriteData() failed, code 0x%x\n", r);
		return FALSE;
	}

	return TRUE;
}

BOOL CKsPbDrv::Stop()
{
	ASSERT(m_Filter);
	ASSERT(m_Pin);
	
	return SUCCEEDED(m_Pin->SetState(KSSTATE_STOP)) ? TRUE : FALSE;
}

BOOL CKsPbDrv::Update(DWORD dwLSample, DWORD dwRSample)
{
	ASSERT(m_Filter);
	ASSERT(m_Pin);

	if (m_pDlg->m_bPulse)
		::FillPulse(m_pBuffer, m_Header.DataUsed, TRUE, m_Fmt);
	else
		::FillBuffer(m_pBuffer, m_Header.DataUsed, dwLSample, dwRSample, m_Fmt);

	Sleep(PB_WAIT);
	return TRUE;
}

CKsRecDrv::CKsRecDrv(CkxtestDlg* pDlg)
	: CRecDriver(pDlg)
	, m_Filter(NULL)
	, m_Pin(NULL)
	, m_pBuffer(NULL)
{
	ASSERT(m_pDlg);
	m_Ovr.hEvent = INVALID_HANDLE_VALUE;
}

CKsRecDrv::~CKsRecDrv()
{
	if (m_Pin)
		m_Pin->ClosePin();

	if (m_Filter)
		delete m_Filter;

	if (INVALID_HANDLE_VALUE != m_Ovr.hEvent)
		CloseHandle(m_Ovr.hEvent);

	if (m_pBuffer)
		free(m_pBuffer);
}

BOOL CKsRecDrv::Init()
{
	ASSERT(m_pDlg->m_RecDesc);
	ASSERT_KINDOF(CKsDesc, m_pDlg->m_RecDesc);

	int nFormat = m_pDlg->m_RecDesc->m_Fmt[m_pDlg->m_RecSR][m_pDlg->m_RecBD];
	ASSERT(FMT_NONE != nFormat);

	FillFmt(m_Fmt, i2sr[m_pDlg->m_RecSR], i2bd1[m_pDlg->m_RecBD], i2bd2[m_pDlg->m_RecBD],
		(nFormat & FMT_EXT) ? TRUE : FALSE);

	CKsDesc& ksDesc = *reinterpret_cast<CKsDesc*>(m_pDlg->m_RecDesc);

	try
	{
		HRESULT r;

		if (!(m_Filter = new CKsAudCapFilter(ksDesc.m_FilterPath, &r)) ||
			(!SUCCEEDED(r)))
			throw "CKsAudCapFilter constructor";

		if (S_OK != m_Filter->Instantiate())
			throw "Instantiate()";

		if (S_OK != m_Filter->EnumeratePins())
			throw "EnumeratePins()";

		if (!(m_Pin = m_Filter->CreateCapturePin((WAVEFORMATEX*)&m_Fmt, FALSE)))
			throw "CreateRenderPin()";

	} catch (const char* error)
	{
		TRACE("KS recording init: %s failed.\n", error);
		return FALSE;
	}

	Zero(m_Header);

	m_Header.Size			= sizeof(m_Header);
	m_Header.FrameExtent	= BUFSIZE(KS_BUFFER, m_Fmt);

	VERIFY(m_Header.Data = m_pBuffer = malloc(m_Header.FrameExtent));

	m_Ovr.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	return TRUE;
}

BOOL CKsRecDrv::Start()
{
	ASSERT(m_Filter);
	ASSERT(m_Pin);

	HRESULT r;
	
	if (S_OK != (r = m_Pin->SetState(KSSTATE_RUN)))
	{
		TRACE("KS recording init: SetState() failed, code 0x%x\n", r);
		return FALSE;
	}

	if (S_OK != (r = m_Pin->ReadData(&m_Header, &m_Ovr)))
	{
		TRACE("KS recording init: ReadData() failed, code 0x%x\n", r);
		return FALSE;
	}

	return TRUE;
}

BOOL CKsRecDrv::Stop()
{
	ASSERT(m_Filter);
	ASSERT(m_Pin);
	
	return SUCCEEDED(m_Pin->SetState(KSSTATE_STOP)) ? TRUE : FALSE;
}

BOOL CKsRecDrv::Update(DWORD &dwLRec, DWORD &dwRRec)
{
	ASSERT(m_Filter);
	ASSERT(m_Pin);

	WaitForSingleObject(m_Ovr.hEvent, INFINITE);

	if (!m_Header.DataUsed)
		return FALSE;

	SetEvent(m_Ovr.hEvent);

	if (m_pDlg->m_bPulse)
		//::FillPulse(m_pBuffer, m_Header.DataUsed, TRUE, m_Fmt);
		;
	else
		switch (m_Fmt.Format.wBitsPerSample)
		{
		case 16:
			dwLRec = (DWORD)*(WORD*)m_pBuffer << 16;
			dwRRec = (DWORD)*((WORD*)m_pBuffer + 1) << 16;
			break;

		case 24:
			dwLRec = ((DWORD)*(WORD*)((BYTE*)m_pBuffer) << 8) |
				((DWORD)*(((BYTE*)m_pBuffer) + 2) << 24);
			dwRRec = ((DWORD)*(WORD*)(((BYTE*)m_pBuffer) + 3) << 8) |
				((DWORD)*(((BYTE*)m_pBuffer) + 5) << 24);
			break;

		case 32:
			dwLRec = *(DWORD*)m_pBuffer;
			dwRRec = *((DWORD*)m_pBuffer + 1);
		}

	return TRUE;
}
