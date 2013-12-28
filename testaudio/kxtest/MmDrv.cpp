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

CMmPbDrv::CMmPbDrv(CkxtestDlg* pDlg)
	: CPbDriver(pDlg)
	, m_Handle(NULL)
	, m_pBuffer(NULL)
{
	ASSERT(m_pDlg);
}

CMmPbDrv::~CMmPbDrv()
{
	if (m_Handle)
	{
		waveOutUnprepareHeader(m_Handle, &m_Header, sizeof(m_Header));
		waveOutClose(m_Handle);
	}

	if (m_pBuffer)
		free(m_pBuffer);
}

BOOL CMmPbDrv::Init()
{
	ASSERT(m_pDlg->m_PbDesc);
	ASSERT_KINDOF(CMmDesc, m_pDlg->m_PbDesc);

	int nFormat = m_pDlg->m_PbDesc->m_Fmt[m_pDlg->m_PbSR][m_pDlg->m_PbBD];
	ASSERT(FMT_NONE != nFormat);

	FillFmt(m_Fmt, i2sr[m_pDlg->m_PbSR], i2bd1[m_pDlg->m_PbBD], i2bd2[m_pDlg->m_PbBD],
		(nFormat & FMT_EXT) ? TRUE : FALSE);

	CMmDesc& mmDesc = *reinterpret_cast<CMmDesc*>(m_pDlg->m_PbDesc);

	if (MMSYSERR_NOERROR != waveOutOpen(&m_Handle, mmDesc.m_ID,
		(LPCWAVEFORMATEX)&m_Fmt, NULL, NULL, WAVE_FORMAT_DIRECT))
	{
		TRACE("MM playback init: waveOutOpen() failed.\n");
		return FALSE;
	}

	m_Size = BUFSIZE(MM_BUFFER, m_Fmt);
	VERIFY(m_pBuffer = malloc(m_Size));

	Zero(m_Header);

	m_Header.lpData			= (LPSTR)m_pBuffer;
	m_Header.dwBufferLength	= m_Size;
	m_Header.dwFlags		= WHDR_BEGINLOOP | WHDR_ENDLOOP;
	m_Header.dwLoops		= 0xFFFF;

	if (MMSYSERR_NOERROR != waveOutPrepareHeader(m_Handle, &m_Header, sizeof(m_Header)))
	{
		TRACE("MM playback init: waveOutPrepareHeader() failed.\n");
		return FALSE;
	}

	return TRUE;
}

BOOL CMmPbDrv::Start()
{
	ASSERT(m_Handle);

	return (MMSYSERR_NOERROR == waveOutWrite(m_Handle, &m_Header, sizeof(m_Header))) ? TRUE : FALSE;
}

BOOL CMmPbDrv::Stop()
{
	ASSERT(m_Handle);

	return (MMSYSERR_NOERROR == waveOutReset(m_Handle)) ? TRUE : FALSE;
}

BOOL CMmPbDrv::Update(DWORD dwLSample, DWORD dwRSample)
{
	ASSERT(m_Handle);

	if (m_pDlg->m_bPulse)
		::FillPulse(m_pBuffer, m_Size, TRUE, m_Fmt);
	else
		::FillBuffer(m_pBuffer, m_Size, dwLSample, dwRSample, m_Fmt);

	if (!Start())
		return FALSE;

	Sleep(PB_WAIT);
	return TRUE;
}

CMmRecDrv::CMmRecDrv(CkxtestDlg* pDlg)
	: CRecDriver(pDlg)
	, m_Handle(NULL)
	, m_pB1(NULL)
	, m_pB2(NULL)
{
	ASSERT(m_pDlg);

	Zero(m_Hdr1);
	Zero(m_Hdr2);
}

CMmRecDrv::~CMmRecDrv()
{
	if (m_Handle)
	{
		MMRESULT r;

		if (MMSYSERR_NOERROR != (r = waveInReset(m_Handle)))
			TRACE("MM recording destructor: waveInReset() failed, code 0x%x.\n", r);

		if (MMSYSERR_NOERROR != (r = waveInUnprepareHeader(m_Handle, &m_Hdr2, sizeof(m_Hdr2))))
			TRACE("MM recording destructor: waveInUnprepareHeader() failed, code 0x%x.\n", r);

		if (MMSYSERR_NOERROR != (r = waveInUnprepareHeader(m_Handle, &m_Hdr1, sizeof(m_Hdr1))))
			TRACE("MM recording destructor: waveInUnprepareHeader() failed, code 0x%x.\n", r);

		if (MMSYSERR_NOERROR != (r = waveInClose(m_Handle)))
			TRACE("MM recording destructor: waveInClose() failed, code 0x%x.\n", r);
	}

	if (m_pB1)
		free(m_pB1);

	if (m_pB2)
		free(m_pB2);
}

BOOL CMmRecDrv::Init()
{
	ASSERT(m_pDlg->m_RecDesc);
	ASSERT_KINDOF(CMmDesc, m_pDlg->m_RecDesc);

	int nFormat = m_pDlg->m_RecDesc->m_Fmt[m_pDlg->m_RecSR][m_pDlg->m_RecBD];
	ASSERT(FMT_NONE != nFormat);

	FillFmt(m_Fmt, i2sr[m_pDlg->m_RecSR], i2bd1[m_pDlg->m_RecBD], i2bd2[m_pDlg->m_RecBD],
		(nFormat & FMT_EXT) ? TRUE : FALSE);

	CMmDesc& mmDesc = *reinterpret_cast<CMmDesc*>(m_pDlg->m_RecDesc);

	MMRESULT r;

	if (MMSYSERR_NOERROR != (r = waveInOpen(&m_Handle, mmDesc.m_ID,
		(LPCWAVEFORMATEX)&m_Fmt, NULL, NULL, WAVE_FORMAT_DIRECT)))
	{
		TRACE("MM recording init: waveOutOpen() failed, code 0x%x.\n", r);
		return FALSE;
	}

	m_Hdr1.dwBufferLength	= BUFSIZE(MM_BUFFER, m_Fmt);
	VERIFY(m_Hdr1.lpData = (LPSTR)(m_pB1 = malloc(m_Hdr1.dwBufferLength)));

	m_Hdr2.dwBufferLength	= BUFSIZE(MM_BUFFER, m_Fmt);
	VERIFY(m_Hdr2.lpData = (LPSTR)(m_pB2 = malloc(m_Hdr2.dwBufferLength)));

	m_pBuffer = m_pB1;

	if (MMSYSERR_NOERROR != (r = waveInPrepareHeader(m_Handle, &m_Hdr1, sizeof(m_Hdr1))))
	{
		TRACE("MM recording destructor: waveInPrepareHeader() failed, code 0x%x.\n", r);
		return FALSE;
	}

	if (MMSYSERR_NOERROR != (r = waveInPrepareHeader(m_Handle, &m_Hdr2, sizeof(m_Hdr2))))
	{
		TRACE("MM recording destructor: waveInPrepareHeader() failed, code 0x%x.\n", r);
		return FALSE;
	}

	if (MMSYSERR_NOERROR != (r = waveInAddBuffer(m_Handle, &m_Hdr1, sizeof(m_Hdr1))))
	{
		TRACE("MM recording destructor: waveInAddBuffer() failed, code 0x%x.\n", r);
		return FALSE;
	}

	if (MMSYSERR_NOERROR != (r = waveInAddBuffer(m_Handle, &m_Hdr2, sizeof(m_Hdr2))))
	{
		TRACE("MM recording destructor: waveInAddBuffer() failed, code 0x%x.\n", r);
		return FALSE;
	}

	return TRUE;
}

BOOL CMmRecDrv::Start()
{
	ASSERT(m_Handle);
	return (MMSYSERR_NOERROR == waveInStart(m_Handle)) ? TRUE : FALSE;
}

BOOL CMmRecDrv::Stop()
{
	ASSERT(m_Handle);
	return (MMSYSERR_NOERROR == waveInReset(m_Handle)) ? TRUE : FALSE;
}

BOOL CMmRecDrv::Update(DWORD &dwLRec, DWORD &dwRRec)
{
	if (m_Hdr1.dwFlags & WHDR_DONE)
	{
		waveInAddBuffer(m_Handle, &m_Hdr1, sizeof(m_Hdr1));
		m_pBuffer = m_pB1;
	}

	if (m_Hdr2.dwFlags & WHDR_DONE)
	{
		waveInAddBuffer(m_Handle, &m_Hdr2, sizeof(m_Hdr2));
		m_pBuffer = m_pB2;
	}

	if (m_pDlg->m_bPulse)
		;
		//FillPulse(pBuffer, dwSize, TRUE, wFmt);
	else
	{
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
	}

	Sleep(REC_WAIT);
	return TRUE;
}
