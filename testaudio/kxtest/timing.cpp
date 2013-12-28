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

inline DWORD64 ReadTsc()
{
	__asm	rdtsc
}

inline BOOL CheckForTsc()
{
	__try
	{
		ReadTsc();
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return FALSE;
	}

	return TRUE;
}

THREAD_IMP CkxtestDlg::DetectClockMethod(LPVOID pParam)
{
	START_TIMING;

	ASSERT(pParam);
	CkxtestDlg& dlg = *reinterpret_cast<CkxtestDlg*>(pParam);

	CWinThread* pThread = AfxGetThread();
	ASSERT(pThread);

	if (dlg.m_bDisableTsc || !CheckForTsc())
	{
		if (QueryPerformanceFrequency((LARGE_INTEGER*)&dlg.m_CounterFreq))
			dlg.m_TimeSource = TS_PERF;
		else
			dlg.m_TimeSource = TS_NONE;

		return 0;
	}

	// measure CPU's clock freq
	//
	TIMECAPS tCaps;
	timeGetDevCaps(&tCaps, sizeof(tCaps));

	MSG msg;
	PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE);
	Sleep(0);

	pThread->SetThreadPriority(THREAD_PRIORITY_TIME_CRITICAL);

	timeBeginPeriod(tCaps.wPeriodMin);
	DWORD tStart = timeGetTime();
	DWORD64 tscStart = ReadTsc();

	Sleep(500);

	DWORD64 tscEnd = ReadTsc();
	DWORD tEnd = timeGetTime();
	timeEndPeriod(tCaps.wPeriodMin);

	pThread->SetThreadPriority(THREAD_PRIORITY_NORMAL);

	dlg.m_CounterFreq = (tscEnd - tscStart) * 500 / (tEnd - tStart);

	// very unlikely that the performance counter can be more precise
	// than TSC, but we have to check.
	//
	DWORD64 RerfFreq;
	if (QueryPerformanceFrequency((LARGE_INTEGER*)&RerfFreq) && (RerfFreq > dlg.m_CounterFreq))
	{
		dlg.m_CounterFreq	= RerfFreq;
		dlg.m_TimeSource	= TS_PERF;
		return 0;
	}

	dlg.m_TimeSource = TS_TSC;

	REPORT_TIMING("DetectClockMethod");
	return 0;
}

DWORD64 CkxtestDlg::GetClock()
{
	switch (m_TimeSource)
	{
	case TS_PERF:
		DWORD64 i;
		QueryPerformanceCounter((LARGE_INTEGER*)&i);
		return i;

	case TS_TSC:
		return ReadTsc();

	default:
		ASSERT(FALSE);
		return 0;
	}
}
