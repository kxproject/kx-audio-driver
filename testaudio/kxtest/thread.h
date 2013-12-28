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


// Threading stuff
//

#define THREAD		static UINT AFX_CDECL
#define THREAD_IMP	UINT
/*
#define START_THREAD(var, proc) \
	CWinThread var(proc, this);\
	var.m_bAutoDelete = FALSE;\
	var.CreateThread();
*/
#define STOP_THREAD(var) (var).PostThreadMessage(WM_QUIT, 0, 0)

#define CHECK_QUIT(ret) \
{\
	MSG msg;\
	while (PeekMessage(&msg, (HWND)INVALID_HANDLE_VALUE, 0, 0, PM_NOREMOVE))\
		if (WM_QUIT == msg.message)\
			return (ret);\
}

#define WAIT_FOR(var, timeout) ((HANDLE)var && (WAIT_TIMEOUT == WaitForSingleObject(var, timeout)))
#define RUNNING(var) WAIT_FOR(var, 0)

#define CLEANUP_AUDIO_THREAD(var, sync) \
{\
	VERIFY(!WAIT_FOR(var, 2000));\
	CloseHandle(sync);\
	var.m_hThread = NULL;\
}

#define EXIT_THREAD(dlg, sync) \
{\
	SetEvent(dlg.sync);\
	return 0;\
}

#define TERMINATE(dlg, sync) \
{\
	dlg.m_bPlaying = FALSE;\
	EXIT_THREAD(dlg, sync); \
}

class CAutoLock
{
public:
	CCriticalSection* m_CS;

	CAutoLock(CCriticalSection& s)
		:m_CS(&s)
	{
		m_CS->Lock();
	}

	~CAutoLock()
	{
		m_CS->Unlock();
	}
};

class CThreadPool: public TList<CWinThread>
{
public:
	void Add(CWinThread* pItem)
	{
		AddTail(pItem);
		pItem->m_bAutoDelete = FALSE;
		pItem->CreateThread();
	}

	void RemoveComlpeted()
	{
		CWinThread* pItem;
		LISTPOS pos = GetHeadPosition();
		LISTPOS current = pos;
		while (GetNext(pos, &pItem))
		{
			if (!RUNNING(*pItem))
			{
				RemoveAt(current);
				delete pItem;
			}
			current = pos;
		}
	}

	void StopAll(DWORD t)
	{
		CWinThread* pItem;
		while (GetHead(&pItem))
		{
			if (RUNNING(*pItem))
			{
				STOP_THREAD(*pItem);
				WAIT_FOR(*pItem, t);
			}
			RemoveHead(&pItem);
			delete pItem;
		}
	}

	~CThreadPool()
	{
		StopAll(500);
	}
};

#define START_TIMING		clock_t ctTime = clock()
#define REPORT_TIMING(n)	TRACE("%s time %.3f\n", n, (double)(clock() - ctTime) / CLOCKS_PER_SEC)