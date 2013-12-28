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

#define Zero(s)		ZeroMemory(&s, sizeof(s))

void FillFmt(
	WAVEFORMATEXTENSIBLE& wFmt,
	DWORD sr,
	WORD bps,
	WORD vbps,
	BOOL bExt)
{
	Zero(wFmt);

	wFmt.Format.wFormatTag		= bExt ? WAVE_FORMAT_EXTENSIBLE : WAVE_FORMAT_PCM;
	wFmt.Format.nChannels		= 2;
	wFmt.Format.nSamplesPerSec	= sr;
	wFmt.Format.wBitsPerSample	= bps;
	wFmt.Format.nBlockAlign		= wFmt.Format.wBitsPerSample / 8 * wFmt.Format.nChannels;
	wFmt.Format.nAvgBytesPerSec	= wFmt.Format.nSamplesPerSec * wFmt.Format.nBlockAlign;

	if (bExt)
	{
		wFmt.Format.cbSize					= 22;
		wFmt.Samples.wValidBitsPerSample	= vbps;
		wFmt.dwChannelMask					= KSAUDIO_SPEAKER_STEREO;
		wFmt.SubFormat						= KSDATAFORMAT_SUBTYPE_PCM;
	}
}

void MmCheck(CMmDesc* pDesc, int i, int j,
	DWORD sr, WORD bps, WORD vbps, BOOL bRec)
{
	WAVEFORMATEXTENSIBLE wFmt;
	
	pDesc->m_Fmt[i][j] = FMT_NONE;

	FillFmt(wFmt, sr, bps, vbps, FALSE);	// check WAVEFORMATEX

	if (MMSYSERR_NOERROR == ((bRec) ?
		waveInOpen(NULL, pDesc->m_ID, (LPCWAVEFORMATEX)&wFmt, NULL, NULL, WAVE_FORMAT_DIRECT | WAVE_FORMAT_QUERY) :
		waveOutOpen(NULL, pDesc->m_ID, (LPCWAVEFORMATEX)&wFmt, NULL, NULL, WAVE_FORMAT_DIRECT | WAVE_FORMAT_QUERY)))
		pDesc->m_Fmt[i][j] = FMT_STD;

	FillFmt(wFmt, sr, bps, vbps, TRUE);		// check WAVEFORMATEXTENSIBLE

	if (MMSYSERR_NOERROR == ((bRec) ?
		waveInOpen(NULL, pDesc->m_ID, (LPCWAVEFORMATEX)&wFmt, NULL, NULL, WAVE_FORMAT_DIRECT | WAVE_FORMAT_QUERY) :
		waveOutOpen(NULL, pDesc->m_ID, (LPCWAVEFORMATEX)&wFmt, NULL, NULL, WAVE_FORMAT_DIRECT | WAVE_FORMAT_QUERY)))
		pDesc->m_Fmt[i][j] |= FMT_EXT;
}

void CkxtestDlg::GetMmCaps(CMmDesc* pDesc, BOOL bRec)
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			MmCheck(pDesc, i, j, i2sr[i], i2bd1[j], i2bd2[j], bRec);
}

void DsICheck(LPDIRECTSOUNDCAPTURE pDS, CDsDesc* pDesc, int i, int j,
	DWORD sr, WORD bps, WORD vbps)
{
	WAVEFORMATEXTENSIBLE wFmt;
	LPDIRECTSOUNDCAPTUREBUFFER pBuf;
	DSCBUFFERDESC desc;

	Zero(desc);

	desc.dwSize				= sizeof(desc);
	desc.lpwfxFormat		= (WAVEFORMATEX*)&wFmt;

	pDesc->m_Fmt[i][j] = FMT_NONE;

	FillFmt(wFmt, sr, bps, vbps, FALSE);	// check WAVEFORMATEX
	desc.dwBufferBytes = wFmt.Format.nAvgBytesPerSec / 4;

	if (DS_OK == pDS->CreateCaptureBuffer(&desc, &pBuf, NULL))
	{
		pBuf->Release();
		pDesc->m_Fmt[i][j] = FMT_STD;
	}

	FillFmt(wFmt, sr, bps, vbps, TRUE);		// check WAVEFORMATEXTENSIBLE
	desc.dwBufferBytes = wFmt.Format.nAvgBytesPerSec / 4;

	if (DS_OK == pDS->CreateCaptureBuffer(&desc, &pBuf, NULL))
	{
		pBuf->Release();
		pDesc->m_Fmt[i][j] |= FMT_EXT;
	}
}

void DsOCheck(LPDIRECTSOUND pDS, CDsDesc* pDesc, int i, int j,
	DWORD sr, WORD bps, WORD vbps)
{
	WAVEFORMATEXTENSIBLE wFmt;
	LPDIRECTSOUNDBUFFER pBuf;
	DSBUFFERDESC desc;

	Zero(desc);

	desc.dwSize				= sizeof(desc);
	desc.dwFlags			= DSBCAPS_GLOBALFOCUS | DSBCAPS_LOCHARDWARE;
	desc.lpwfxFormat		= (WAVEFORMATEX*)&wFmt;
	desc.guid3DAlgorithm	= GUID_NULL;

	pDesc->m_Fmt[i][j] = FMT_NONE;

	FillFmt(wFmt, sr, bps, vbps, FALSE);	// check WAVEFORMATEX
	desc.dwBufferBytes = wFmt.Format.nAvgBytesPerSec / 4;

	if (DS_OK == pDS->CreateSoundBuffer(&desc, &pBuf, NULL))
	{
		pBuf->Release();
		pDesc->m_Fmt[i][j] = FMT_STD;
	}

	FillFmt(wFmt, sr, bps, vbps, TRUE);		// check WAVEFORMATEXTENSIBLE
	desc.dwBufferBytes = wFmt.Format.nAvgBytesPerSec / 4;

	if (DS_OK == pDS->CreateSoundBuffer(&desc, &pBuf, NULL))
	{
		pBuf->Release();
		pDesc->m_Fmt[i][j] |= FMT_EXT;
	}
}

BOOL CkxtestDlg::DsIEnumProc(LPGUID lpGUID,
	LPCTSTR lpszDesc,
	LPCTSTR lpszDrvName,
	LPVOID lpContext)
{
	ASSERT(lpszDesc);
	ASSERT(lpContext);

	CHECK_QUIT(FALSE);

	if (!lpGUID)		// skip mapper device
		return TRUE;

	LPDIRECTSOUNDCAPTURE pDS;

	if (DS_OK != DirectSoundCaptureCreate(lpGUID, &pDS, NULL))
		return TRUE;

	CDsDesc* pDesc = new CDsDesc(lpGUID, lpszDesc);

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			DsICheck(pDS, pDesc, i, j, i2sr[i], i2bd1[j], i2bd2[j]);

	if (pDesc->FindFirstFormat())
		reinterpret_cast<CDescList*>(lpContext)->AddTail(pDesc);
	else
		delete pDesc;

	pDS->Release();
	return TRUE;
}

BOOL CkxtestDlg::DsOEnumProc(LPGUID lpGUID,
	LPCTSTR lpszDesc,
	LPCTSTR lpszDrvName,
	LPVOID lpContext)
{
	ASSERT(lpszDesc);
	ASSERT(lpContext);

	CHECK_QUIT(FALSE);

	if (!lpGUID)		// skip mapper device
		return TRUE;

	LPDIRECTSOUND pDS;

	if (DS_OK != DirectSoundCreate(lpGUID, &pDS, NULL))
		return TRUE;

	if (DS_OK != pDS->SetCooperativeLevel(m_hwndMain, DSSCL_PRIORITY))
	{
		pDS->Release();
		return TRUE;
	}

	CDsDesc* pDesc = new CDsDesc(lpGUID, lpszDesc);

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			DsOCheck(pDS, pDesc, i, j, i2sr[i], i2bd1[j], i2bd2[j]);

	if (pDesc->FindFirstFormat())
		reinterpret_cast<CDescList*>(lpContext)->AddTail(pDesc);
	else
		delete pDesc;

	pDS->Release();
	return TRUE;
}

void KsICheck(CKsAudCapFilter* pFilter, CKsDesc* pDesc, int i, int j,
	DWORD sr, WORD bps, WORD vbps)
{
	WAVEFORMATEXTENSIBLE wFmt;
	CKsAudCapPin* pPin;

	pDesc->m_Fmt[i][j] = FMT_NONE;

	FillFmt(wFmt, sr, bps, vbps, FALSE);	// check WAVEFORMATEX

	if (NULL != (pPin = pFilter->CreateCapturePin((LPWAVEFORMATEX)&wFmt, FALSE)))
	{
		pPin->ClosePin();
		pDesc->m_Fmt[i][j] = FMT_STD;
	}

	FillFmt(wFmt, sr, bps, vbps, TRUE);		// check WAVEFORMATEXTENSIBLE

	if (NULL != (pPin = pFilter->CreateCapturePin((LPWAVEFORMATEX)&wFmt, FALSE)))
	{
		pPin->ClosePin();
		pDesc->m_Fmt[i][j] |= FMT_EXT;
	}
}

void KsOCheck(CKsAudRenFilter* pFilter, CKsDesc* pDesc, int i, int j,
	DWORD sr, WORD bps, WORD vbps)
{
	WAVEFORMATEXTENSIBLE wFmt;

	pDesc->m_Fmt[i][j] = FMT_NONE;

	FillFmt(wFmt, sr, bps, vbps, FALSE);	// check WAVEFORMATEX

	if (pFilter->CanCreateRenderPin((LPWAVEFORMATEX)&wFmt))
		pDesc->m_Fmt[i][j] = FMT_STD;

	FillFmt(wFmt, sr, bps, vbps, TRUE);		// check WAVEFORMATEXTENSIBLE

	if (pFilter->CanCreateRenderPin((LPWAVEFORMATEX)&wFmt))
		pDesc->m_Fmt[i][j] |= FMT_EXT;
}

void CkxtestDlg::GetIKsCaps(CKsAudCapFilter* pFilter, CKsDesc* pDesc)
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			KsICheck(pFilter, pDesc, i, j, i2sr[i], i2bd1[j], i2bd2[j]);
}

void CkxtestDlg::GetOKsCaps(CKsAudRenFilter* pFilter, CKsDesc* pDesc)
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			KsOCheck(pFilter, pDesc, i, j, i2sr[i], i2bd1[j], i2bd2[j]);
}

void CkxtestDlg::GetAsioCaps(IASIO* iDrv, CAsioDesc* pDesc)
{
	static const ASIOSampleRate rates[4] = {44100., 48000., 96000., 192000.};
	int i;

	for (i = 0; i < 4; i++)
	{
		if (ASE_OK == iDrv->canSampleRate(rates[i]))
			switch (pDesc->m_Type)
			{
			case ASIOSTInt16MSB:
			case ASIOSTInt16LSB:
				pDesc->m_Fmt[i][0] = FMT_STD;
				break;

			case ASIOSTInt24MSB:
			case ASIOSTInt24LSB:
				pDesc->m_Fmt[i][1] = FMT_STD;
				break;

			case ASIOSTInt32MSB24:
			case ASIOSTInt32LSB24:
				pDesc->m_Fmt[i][2] = FMT_STD;
				break;

			case ASIOSTInt32MSB:
			case ASIOSTInt32LSB:
				pDesc->m_Fmt[i][3] = FMT_STD;
				break;
			}
	}

	iDrv->getClockSources(pDesc->m_ClockSources, &pDesc->m_NumCS);

	for (i = 0; i < pDesc->m_NumCS; i++)
		if (ASIOTrue == pDesc->m_ClockSources[i].isCurrentSource)
		{
			pDesc->m_CS = i;
			break;
		}
}
