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

THREAD_IMP CkxtestDlg::PbThread(LPVOID pParam)
{
	ASSERT(pParam);
	CkxtestDlg& dlg = *reinterpret_cast<CkxtestDlg*>(pParam);

	CPbDriver* pDrv;

	switch (dlg.m_PbDrv)
	{
	case DRV_MM:	pDrv = new CMmPbDrv(&dlg);		break;
	case DRV_DS:	pDrv = new CDsPbDrv(&dlg);		break;
	case DRV_KS:	pDrv = new CKsPbDrv(&dlg);		break;
	case DRV_ASIO:	pDrv = new CAsioPbDrv(&dlg);	break;
	case DRV_KX:	pDrv = new CKxPbDrv(&dlg);		break;

	default:
		ASSERT(FALSE);
		TERMINATE(dlg, m_hPbSync);
	}

	ASSERT(pDrv);

	if (!pDrv->Init())
	{
		delete pDrv;
		TERMINATE(dlg, m_hPbSync);
	}

	AfxGetThread()->SetThreadPriority(THREAD_PRIORITY_TIME_CRITICAL);

	DWORD dwLSample, dwRSample;
	dlg.GetSample(dwLSample, dwRSample);

	pDrv->Update(dwLSample, dwRSample);

	//int nTimeSource = dlg.GetTimeSource();
	//DWORD64 dwClockFreq = dlg.GetCounterFreq();

	TRACE("playback thread: ready - wait for sync.\n");
	SetEvent(dlg.m_hPbSync);	// we're ready

	if (WAIT_TIMEOUT == WaitForSingleObject(dlg.m_hNotify, 2000))
	{
		delete pDrv;

		TRACE("playback thread: sync timeout.\n");
		TERMINATE(dlg, m_hPbSync);
	}

	if (dlg.m_bPlaying)
	{
		pDrv->Start();

		DWORD64 dwStartTime = dlg.GetClock();

		TRACE("playback thread: started at %I64u.\n", dwStartTime);
		dlg.PostMessage(WM_APP_SHOWSAMPLE);
	}
	else
	{
		delete pDrv;

		TRACE("playback thread: terminated.\n");
		EXIT_THREAD(dlg, m_hPbSync);
	}

	while (dlg.m_bPlaying)
	{
		if (dlg.m_bPulse)
		{
		}
		else
		{
			DWORD dwLSampleCopy, dwRSampleCopy;

			dlg.GetSample(dwLSampleCopy, dwRSampleCopy);

			if ((dwLSample != dwLSampleCopy) || (dwRSample != dwRSampleCopy))
			{
				dwLSample = dwLSampleCopy;
				dwRSample = dwRSampleCopy;

				pDrv->Update(dwLSample, dwRSample);

				dlg.PostMessage(WM_APP_SHOWSAMPLE);
			}
			else
				Sleep(20);
		}
	}

	pDrv->Stop();
	delete pDrv;

	TRACE("playback thread: terminated.\n");
	EXIT_THREAD(dlg, m_hPbSync);
}

THREAD_IMP CkxtestDlg::RecThread(LPVOID pParam)
{
	ASSERT(pParam);
	CkxtestDlg& dlg = *reinterpret_cast<CkxtestDlg*>(pParam);

	CRecDriver* pDrv;

	switch (dlg.m_RecDrv)
	{
	case DRV_MM:	pDrv = new CMmRecDrv(&dlg);		break;
	case DRV_DS:	pDrv = new CDsRecDrv(&dlg);		break;
	case DRV_KS:	pDrv = new CKsRecDrv(&dlg);		break;
	case DRV_ASIO:	pDrv = new CAsioRecDrv(&dlg);	break;
	case DRV_KX:	pDrv = new CKxRecDrv(&dlg);		break;

	default:
		ASSERT(FALSE);
		TERMINATE(dlg, m_hRecSync);
	}

	ASSERT(pDrv);

	if (!pDrv->Init())
	{
		delete pDrv;
		TERMINATE(dlg, m_hRecSync);
	}

	AfxGetThread()->SetThreadPriority(THREAD_PRIORITY_TIME_CRITICAL);

	DWORD dwLRec, dwLSample;
	DWORD dwRRec, dwRSample;

	dlg.GetSample(dwLSample, dwRSample);
	dwLRec = dwRRec = 0;

	//int nTimeSource = dlg.GetTimeSource();

	TRACE("recording thread: ready - wait for sync.\n");
	SetEvent(dlg.m_hRecSync);	// we're ready

	if (WAIT_TIMEOUT == WaitForSingleObject(dlg.m_hNotify, 2000))
	{
		delete pDrv;

		TRACE("recording thread: sync timeout.\n");
		TERMINATE(dlg, m_hRecSync);
	}

	if (dlg.m_bPlaying)
	{
		pDrv->Start();

		DWORD64 dwStartTime = dlg.GetClock();

		TRACE("recording thread: started at %I64u.\n", dwStartTime);

		dlg.SetRecSample(dwLRec, dwRRec);
		dlg.PostMessage(WM_APP_SHOWREC);
	}
	else
	{
		delete pDrv;

		TRACE("recording thread: terminated.\n");
		EXIT_THREAD(dlg, m_hRecSync);
	}

	while (dlg.m_bPlaying)
	{
		DWORD dwLRecCopy, dwLSampleCopy;
		DWORD dwRRecCopy, dwRSampleCopy;

		dlg.GetSample(dwLSampleCopy, dwRSampleCopy);

		pDrv->Update(dwLRecCopy, dwRRecCopy);

		if ((dwLRec != dwLRecCopy) || (dwLSample != dwLSampleCopy) ||
			(dwRRec != dwRRecCopy) || (dwRSample != dwRSampleCopy))
		{
			dwLRec = dwLRecCopy;
			dwLSample = dwLSampleCopy;

			dwRRec = dwRRecCopy;
			dwRSample = dwRSampleCopy;

			dlg.SetRecSample(dwLRec, dwRRec);
			dlg.PostMessage(WM_APP_SHOWREC);
		}
	}

	pDrv->Stop();
	delete pDrv;

	TRACE("recording thread: terminated.\n");
	EXIT_THREAD(dlg, m_hRecSync);
}

THREAD_IMP CkxtestDlg::AsioFDThread(LPVOID pParam)
{
	ASSERT(pParam);
	CkxtestDlg& dlg = *reinterpret_cast<CkxtestDlg*>(pParam);

	CAsioFDDrv drv(&dlg);

	if (!drv.Init())
		TERMINATE(dlg, m_hPbSync);

	AfxGetThread()->SetThreadPriority(THREAD_PRIORITY_TIME_CRITICAL);

	DWORD dwLRec, dwLSample;
	DWORD dwRRec, dwRSample;

	dlg.GetSample(dwLSample, dwRSample);

	drv.CAsioPbDrv::Update(dwLSample, dwRSample);
	dwLRec = dwRRec = 0;

	//int nTimeSource = dlg.GetTimeSource();
	//DWORD64 dwClockFreq = dlg.GetCounterFreq();

	TRACE("ASIO FD thread: ready - wait for sync.\n");
	SetEvent(dlg.m_hPbSync);	// we're ready

	if (WAIT_TIMEOUT == WaitForSingleObject(dlg.m_hNotify, 2000))
	{
		TRACE("ASIO FD thread: sync timeout.\n");
		TERMINATE(dlg, m_hPbSync);
	}

	if (dlg.m_bPlaying)
	{
		drv.CAsioPbDrv::Start();

		DWORD64 dwStartTime = dlg.GetClock();

		TRACE("ASIO FD thread: started at %I64u.\n", dwStartTime);
		dlg.PostMessage(WM_APP_SHOWSAMPLE);
	}
	else
	{
		TRACE("ASIO FD thread: terminated.\n");
		EXIT_THREAD(dlg, m_hPbSync);
	}

	while (dlg.m_bPlaying)
	{
		if (dlg.m_bPulse)
		{
		}
		else
		{
			DWORD dwLRecCopy, dwLSampleCopy;
			DWORD dwRRecCopy, dwRSampleCopy;

			dlg.GetSample(dwLSampleCopy, dwRSampleCopy);

			if ((dwLSample != dwLSampleCopy) ||
				(dwRSample != dwRSampleCopy))
			{
				dwLSample = dwLSampleCopy;
				dwRSample = dwRSampleCopy;

				drv.CAsioPbDrv::Update(dwLSample, dwRSample);

				dlg.PostMessage(WM_APP_SHOWSAMPLE);
			}

			drv.CAsioRecDrv::Update(dwLRecCopy, dwRRecCopy);

			if ((dwLRec != dwLRecCopy) ||
				(dwRRec != dwRRecCopy))
			{
				dwLRec = dwLRecCopy;
				dwRRec = dwRRecCopy;

				dlg.SetRecSample(dwLRec, dwRRec);
				dlg.PostMessage(WM_APP_SHOWREC);
			}
		}
	}

	drv.CAsioPbDrv::Stop();

	TRACE("playback thread: terminated.\n");
	EXIT_THREAD(dlg, m_hPbSync);
}
