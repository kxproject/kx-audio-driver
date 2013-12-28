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

CDsPbDrv::CDsPbDrv(CkxtestDlg* pDlg)
	: CPbDriver(pDlg)
	, m_pDS(NULL)
	, m_pDsBuf(NULL)
{
	ASSERT(m_pDlg);
}

CDsPbDrv::~CDsPbDrv()
{
	if (m_pDS)
		m_pDS->Release();
}

BOOL CDsPbDrv::Init()
{
	ASSERT(m_pDlg->m_PbDesc);
	ASSERT_KINDOF(CDsDesc, m_pDlg->m_PbDesc);

	int nFormat = m_pDlg->m_PbDesc->m_Fmt[m_pDlg->m_PbSR][m_pDlg->m_PbBD];
	ASSERT(FMT_NONE != nFormat);

	FillFmt(m_Fmt, i2sr[m_pDlg->m_PbSR], i2bd1[m_pDlg->m_PbBD], i2bd2[m_pDlg->m_PbBD],
		(nFormat & FMT_EXT) ? TRUE : FALSE);

	CDsDesc& dsDesc = *reinterpret_cast<CDsDesc*>(m_pDlg->m_PbDesc);

	if (DS_OK != DirectSoundCreate(&dsDesc.m_GUID, &m_pDS, NULL))
	{
		TRACE("DS playback init: DirectSoundCreate() failed.\n");
		return FALSE;
	}

	if (DS_OK != m_pDS->SetCooperativeLevel(m_pDlg->m_hwndMain, DSSCL_PRIORITY))
	{
		TRACE("DS playback init: SetCooperativeLevel() failed.\n");
		return FALSE;
	}

	Zero(m_DsDesc);

	m_DsDesc.dwSize				= sizeof(m_DsDesc);
	m_DsDesc.dwFlags			= DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_STICKYFOCUS
		| DSBCAPS_GLOBALFOCUS | DSBCAPS_LOCHARDWARE;

	m_DsDesc.dwBufferBytes		= BUFSIZE(DS_BUFFER, m_Fmt);
	m_DsDesc.lpwfxFormat		= (WAVEFORMATEX*)&m_Fmt;
	m_DsDesc.guid3DAlgorithm	= GUID_NULL;

	if (DS_OK != m_pDS->CreateSoundBuffer(&m_DsDesc, &m_pDsBuf, NULL))
	{
		TRACE("DS playback init: CreateSoundBuffer() failed.\n");
		return FALSE;
	}

	return TRUE;
}

BOOL CDsPbDrv::Start()
{
	ASSERT(m_pDS);
	ASSERT(m_pDsBuf);

	return (DS_OK == m_pDsBuf->Play(0, 0, DSBPLAY_LOOPING)) ? TRUE : FALSE;
}

BOOL CDsPbDrv::Stop()
{
	ASSERT(m_pDS);
	ASSERT(m_pDsBuf);

	return (DS_OK == m_pDsBuf->Stop()) ? TRUE : FALSE;
}

BOOL CDsPbDrv::Update(DWORD dwLSample, DWORD dwRSample)
{
	LPVOID	pBuffer;
	DWORD	dwSize;

	ASSERT(m_pDS);
	ASSERT(m_pDsBuf);

	if (DS_OK != m_pDsBuf->Lock(0, 0, &pBuffer, &dwSize, NULL, NULL, DSBLOCK_ENTIREBUFFER))
	{
		TRACE("DS playback: Lock() failed.\n");
		return FALSE;
	}

	if (m_pDlg->m_bPulse)
		::FillPulse(pBuffer, dwSize, TRUE, m_Fmt);
	else
		::FillBuffer(pBuffer, dwSize, dwLSample, dwRSample, m_Fmt);

	if (DS_OK != m_pDsBuf->Unlock(pBuffer, dwSize, NULL, 0))
	{
		TRACE("DS playback: Unlock() failed.\n");
		return FALSE;
	}

	Sleep(PB_WAIT);
	return TRUE;
}

CDsRecDrv::CDsRecDrv(CkxtestDlg* pDlg)
	: CRecDriver(pDlg)
	, m_pDS(NULL)
	, m_pDsBuf(NULL)
{
	ASSERT(m_pDlg);
}

CDsRecDrv::~CDsRecDrv()
{
	if (m_pDS)
		m_pDS->Release();
}

BOOL CDsRecDrv::Init()
{
	ASSERT(m_pDlg->m_RecDesc);
	ASSERT_KINDOF(CDsDesc, m_pDlg->m_RecDesc);

	int nFormat = m_pDlg->m_RecDesc->m_Fmt[m_pDlg->m_RecSR][m_pDlg->m_RecBD];
	ASSERT(FMT_NONE != nFormat);

	FillFmt(m_Fmt, i2sr[m_pDlg->m_RecSR], i2bd1[m_pDlg->m_RecBD], i2bd2[m_pDlg->m_RecBD],
		(nFormat & FMT_EXT) ? TRUE : FALSE);

	CDsDesc& dsDesc = *reinterpret_cast<CDsDesc*>(m_pDlg->m_RecDesc);

	if (DS_OK != DirectSoundCaptureCreate(&dsDesc.m_GUID, &m_pDS, NULL))
	{
		TRACE("DS recording init: DirectSoundCaptureCreate() failed.\n");
		return FALSE;
	}

	Zero(m_DsDesc);

	m_DsDesc.dwSize			= sizeof(m_DsDesc);
	m_DsDesc.dwBufferBytes	= BUFSIZE(DS_BUFFER, m_Fmt);
	m_DsDesc.lpwfxFormat	= (WAVEFORMATEX*)&m_Fmt;

	if (DS_OK != m_pDS->CreateCaptureBuffer(&m_DsDesc, &m_pDsBuf, NULL))
	{
		TRACE("DS recording init: CreateCaptureBuffer() failed.\n");
		return FALSE;
	}

	return TRUE;
}

BOOL CDsRecDrv::Start()
{
	ASSERT(m_pDS);
	ASSERT(m_pDsBuf);

	return (DS_OK == m_pDsBuf->Start(DSCBSTART_LOOPING)) ? TRUE : FALSE;
}

BOOL CDsRecDrv::Stop()
{
	ASSERT(m_pDS);
	ASSERT(m_pDsBuf);

	return (DS_OK == m_pDsBuf->Stop()) ? TRUE : FALSE;
}

BOOL CDsRecDrv::Update(DWORD &dwLRec, DWORD &dwRRec)
{
	LPVOID	pBuffer;
	DWORD	dwSize, dwCapOffset, dwReadOffset;

	if (DS_OK != m_pDsBuf->GetCurrentPosition(&dwCapOffset, &dwReadOffset))
	{
		TRACE("DS recording: GetCurrentPosition() failed.\n");
		return FALSE;
	}

	if (DS_OK != m_pDsBuf->Lock(0, 0, &pBuffer, &dwSize, NULL, NULL, DSCBLOCK_ENTIREBUFFER))
	{
		TRACE("DS recording: Lock() failed.\n");
		return FALSE;
	}

	if (m_pDlg->m_bPulse)
		;
		//FillPulse(pBuffer, dwSize, TRUE, wFmt);
	else
	{
		DWORD dwLastSample;

		if (dwReadOffset)
			dwLastSample = dwReadOffset - m_Fmt.Format.nBlockAlign;
		else
			dwLastSample = dwSize - m_Fmt.Format.nBlockAlign;
		
		switch (m_Fmt.Format.wBitsPerSample)
		{
		case 16:
			dwLRec = (DWORD)((WORD*)pBuffer)[dwLastSample / 2] << 16;
			dwRRec = (DWORD)((WORD*)pBuffer)[dwLastSample / 2 + 1] << 16;
			break;

		case 24:
			dwLRec = ((DWORD)*(WORD*)(((BYTE*)pBuffer) + dwLastSample) << 8) |
				((DWORD)*(((BYTE*)pBuffer) + dwLastSample + 2) << 24);
			dwRRec = ((DWORD)*(WORD*)(((BYTE*)pBuffer) + dwLastSample + 3) << 8) |
				((DWORD)*(((BYTE*)pBuffer) + dwLastSample + 5) << 24);
			break;

		case 32:
			dwLRec = ((DWORD*)pBuffer)[dwLastSample / 4];
			dwRRec = ((DWORD*)pBuffer)[dwLastSample / 4 + 1];
		}
	}

	if (DS_OK != m_pDsBuf->Unlock(pBuffer, dwSize, NULL, 0))
	{
		TRACE("DS recording: Unlock() failed.\n");
		return FALSE;
	}

	Sleep(REC_WAIT);
	return TRUE;
}
