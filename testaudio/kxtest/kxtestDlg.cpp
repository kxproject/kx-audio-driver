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


// kxtestDlg.cpp : implementation file
//

#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <math.h>

#include "kxtest.h"
#include "InputDlg.h"
#include "ProgDlg.h"
#include ".\kxtestdlg.h"
#include "SettingsDlg.h"

void FillBuffer(LPVOID pBuffer, int nSize, DWORD dwLSample, DWORD dwRSample, WAVEFORMATEXTENSIBLE& wFmt)
{
	int i;
	DWORD64 dwBigBlock;

	switch (wFmt.Format.wBitsPerSample)
	{
	case 16:
		dwLSample = (dwLSample >> 16) | (dwRSample & 0xFFFF0000);
		dwBigBlock = (DWORD64)dwLSample | ((DWORD64)dwLSample << 32);

		for (i = 0; i < nSize / 8; i++)
			((DWORD64*)pBuffer)[i] = dwBigBlock;
		break;

	case 24:
		dwLSample = (dwLSample >> 8) | ((dwRSample << 16) & 0xFF000000);
		dwRSample >>= 16;

		for (i = 0; i < nSize / 6; i++)
		{
			*(DWORD*)(((BYTE*)pBuffer) + i * 6) = dwLSample;
			*(WORD*)(((BYTE*)pBuffer) + i * 6 + 4) = (WORD)dwRSample;
		}
		break;

	case 32:
		dwBigBlock = (DWORD64)dwLSample | ((DWORD64)dwRSample << 32);

		for (i = 0; i < nSize / 8; i++)
			((DWORD64*)pBuffer)[i] = dwBigBlock;
		break;

	default:
		ASSERT(FALSE);
	}
}

void FillPulse(LPVOID pBuffer, int nSize, BOOL bStereo, WAVEFORMATEXTENSIBLE& wFmt)
{
	ZeroMemory(pBuffer, nSize);

	switch (wFmt.Format.wBitsPerSample)
	{
	case 16:
		((WORD*)pBuffer)[nSize / 4] = 0x7FFF;

		if (bStereo)
			((WORD*)pBuffer)[nSize / 4 + 1] = 0x7FFF;
		break;

	case 24:
		*(WORD*)(((BYTE*)pBuffer) + nSize / 3) = 0xFFFF;
		*(((BYTE*)pBuffer) + nSize / 3 + 2) = 0x007F;

		if (bStereo)
		{
			*(WORD*)(((BYTE*)pBuffer) + nSize / 3 + 3) = 0xFFFF;
			*(((BYTE*)pBuffer) + nSize / 3 + 5) = 0x007F;
		}
		break;

	case 32:
		((DWORD*)pBuffer)[nSize / 8] = 0x7FFFFFFF;

		if (bStereo)
			((DWORD*)pBuffer)[nSize / 8 + 4] = 0x7FFFFFFF;
		break;

	default:
		ASSERT(FALSE);
	}
}

// CkxtestDlg dialog

CkxtestDlg::CkxtestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CkxtestDlg::IDD, pParent)
	, m_TimeSource(TS_NONE)
	, m_TimingMethod(TM_CALCSTART)
	, m_bPlaying(FALSE)
	, m_bPulse(FALSE)
	, m_LSample(0x55555555)
	, m_RSample(0x55555555)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_PbThread.m_pfnThreadProc	= PbThread;
	m_PbThread.m_pThreadParams	= this;
	m_PbThread.m_bAutoDelete	= FALSE;

	m_RecThread.m_pfnThreadProc	= RecThread;
	m_RecThread.m_pThreadParams	= this;
	m_RecThread.m_bAutoDelete	= FALSE;

	m_FDThread.m_pfnThreadProc	= AsioFDThread;
	m_FDThread.m_pThreadParams	= this;
	m_FDThread.m_bAutoDelete	= FALSE;
}

void CkxtestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LSRC, m_LSrc);
	DDX_Control(pDX, IDC_LKX, m_LKx);
	DDX_Control(pDX, IDC_LXOR, m_LXor);
	DDX_Control(pDX, IDC_LSRCVAL, m_LSrcVal);
	DDX_Control(pDX, IDC_LKXVAL, m_LKxVal);
	DDX_Control(pDX, IDC_LPULSE, m_LPulse);
	DDX_Control(pDX, IDC_LDIFFVAL, m_LDiffVal);
	DDX_Control(pDX, IDC_RSRC, m_RSrc);
	DDX_Control(pDX, IDC_RKX, m_RKx);
	DDX_Control(pDX, IDC_RXOR, m_RXor);
	DDX_Control(pDX, IDC_RSRCVAL, m_RSrcVal);
	DDX_Control(pDX, IDC_RKXVAL, m_RKxVal);
	DDX_Control(pDX, IDC_RPULSE, m_RPulse);
	DDX_Control(pDX, IDC_RDIFFVAL, m_RDiffVal);
	DDX_Control(pDX, IDC_PB_DEVICE, m_PB_Device);
	DDX_Control(pDX, IDC_REC_DEVICE, m_Rec_Device);
	DDX_Control(pDX, IDC_PB_ASIOCS, m_PbAsioCS);
	DDX_Control(pDX, IDC_REC_ASIOCS, m_RecAsioCS);
	DDX_Control(pDX, IDC_PB_ASIOCP, m_PbAsioCP);
	DDX_Control(pDX, IDC_REC_ASIOCP, m_RecAsioCP);
	DDX_Control(pDX, IDC_PLAY, m_Play);
	DDX_Control(pDX, IDC_KXINFO, m_KxInfo);
}

BEGIN_MESSAGE_MAP(CkxtestDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_PLAY, OnBnClickedPlay)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_LINC, OnBnClickedLInc)
	ON_BN_CLICKED(IDC_LDEC, OnBnClickedLDec)
	ON_BN_CLICKED(IDC_LROL, OnBnClickedLRol)
	ON_BN_CLICKED(IDC_LROR, OnBnClickedLRor)
	ON_BN_CLICKED(IDC_LZERO, OnBnClickedLZero)
	ON_BN_CLICKED(IDC_LASK, OnBnClickedLAsk)
	ON_BN_CLICKED(IDC_LRND, OnBnClickedLRnd)
	ON_BN_CLICKED(IDC_LPULSE, OnBnClickedLPulse)
	ON_BN_CLICKED(IDC_RINC, OnBnClickedRInc)
	ON_BN_CLICKED(IDC_RDEC, OnBnClickedRDec)
	ON_BN_CLICKED(IDC_RROL, OnBnClickedRRol)
	ON_BN_CLICKED(IDC_RROR, OnBnClickedRRor)
	ON_BN_CLICKED(IDC_RZERO, OnBnClickedRZero)
	ON_BN_CLICKED(IDC_RASK, OnBnClickedRAsk)
	ON_BN_CLICKED(IDC_RRND, OnBnClickedRRnd)
	ON_BN_CLICKED(IDC_RPULSE, OnBnClickedRPulse)
	ON_CBN_SELCHANGE(IDC_PB_DEVICE, OnCbnSelchangePbDevice)
	ON_CBN_SELCHANGE(IDC_REC_DEVICE, OnCbnSelchangeRecDevice)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_PB_MM, IDC_PB_KX, OnBnClickedPbDriver)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_PB_44100, IDC_PB_192000, OnBnClickedPbSr)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_PB_16, IDC_PB_32, OnBnClickedPbBd)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_REC_MM, IDC_REC_KX, OnBnClickedRecDriver)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_REC_44100, IDC_REC_192000, OnBnClickedRecSr)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_REC_16, IDC_REC_32, OnBnClickedRecBd)
	ON_CBN_SELCHANGE(IDC_PB_ASIOCS, OnCbnSelchangePbAsiocs)
	ON_CBN_SELCHANGE(IDC_REC_ASIOCS, OnCbnSelchangeRecAsiocs)
	ON_BN_CLICKED(IDC_PB_ASIOCP, OnBnClickedPbAsiocp)
	ON_BN_CLICKED(IDC_REC_ASIOCP, OnBnClickedRecAsiocp)
	ON_BN_CLICKED(IDC_SETTINGS, OnBnClickedSettings)
	ON_MESSAGE(WM_APP_SHOWSAMPLE, OnShowSample)
	ON_MESSAGE(WM_APP_SHOWREC, OnShowRec)
	ON_MESSAGE(WM_APP_CLEARALL, OnClearAll)
	ON_STN_CLICKED(IDC_KXINFO, &CkxtestDlg::OnStnClickedKxinfo)
END_MESSAGE_MAP()


// CkxtestDlg message handlers

HWND CkxtestDlg::m_hwndMain = NULL;

BOOL CkxtestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_bDisableTsc	= theApp.GetProfileInt("Settings", "Disable TSC", FALSE);
	m_TimeSource	= theApp.GetProfileInt("Settings", "Time Source", FALSE);
	m_TimingMethod	= theApp.GetProfileInt("Settings", "Time Method", TM_CALCSTART);

	m_Font.CreateFont(8, 0, 0, 0, 0, 0, 0, 0,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		OUT_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		FIXED_PITCH,
		"Courier");

	m_LSrc.SetFont(&m_Font);
	m_LKx.SetFont(&m_Font);
	m_LXor.SetFont(&m_Font);
	m_RSrc.SetFont(&m_Font);
	m_RKx.SetFont(&m_Font);
	m_RXor.SetFont(&m_Font);

	m_hwndMain = GetSafeHwnd();

	// set default format (48/16)
	m_PbSR = m_RecSR = SR_48000;
	m_PbBD = m_RecBD = BD_16;

	CheckRadioButton(IDC_REC_16, IDC_REC_32, IDC_REC_16);
	CheckRadioButton(IDC_PB_16, IDC_PB_32, IDC_PB_16);
	CheckRadioButton(IDC_REC_44100, IDC_REC_192000, IDC_REC_48000);
	CheckRadioButton(IDC_PB_44100, IDC_PB_192000, IDC_PB_48000);

	if (!EnumDevices())
	{
		EndDialog(-1);
		return FALSE;
	}

	// disable absent devices and select the default
	//
	BOOL bISet = FALSE;
	BOOL bOSet = FALSE;

	if (m_MmIEnum.IsEmpty())							// MM
		GetDlgItem(IDC_REC_MM)->EnableWindow(FALSE);
	else
	{
		CheckRadioButton(IDC_REC_MM, IDC_REC_KX, IDC_REC_MM);
		OnBnClickedRecDriver(IDC_REC_MM);
		bISet = TRUE;
	}

	if (m_MmOEnum.IsEmpty())
		GetDlgItem(IDC_PB_MM)->EnableWindow(FALSE);
	else
	{
		CheckRadioButton(IDC_PB_MM, IDC_PB_KX, IDC_PB_MM);
		OnBnClickedPbDriver(IDC_PB_MM);
		bOSet = TRUE;
	}

	if (m_DsIEnum.IsEmpty())							// DirectSound
		GetDlgItem(IDC_REC_DS)->EnableWindow(FALSE);
	else if (!bISet)
	{
		CheckRadioButton(IDC_REC_MM, IDC_REC_KX, IDC_REC_DS);
		OnBnClickedRecDriver(IDC_REC_DS);
		bISet = TRUE;
	}

	if (m_DsOEnum.IsEmpty())
		GetDlgItem(IDC_PB_DS)->EnableWindow(FALSE);
	else if (!bOSet)
	{
		CheckRadioButton(IDC_PB_MM, IDC_PB_KX, IDC_PB_DS);
		OnBnClickedPbDriver(IDC_PB_DS);
		bOSet = TRUE;
	}

	if (m_KsIEnum.IsEmpty())							// Kernel Streaming
		GetDlgItem(IDC_REC_KS)->EnableWindow(FALSE);
	else if (!bISet)
	{
		CheckRadioButton(IDC_REC_MM, IDC_REC_KX, IDC_REC_KS);
		OnBnClickedRecDriver(IDC_REC_KS);
		bISet = TRUE;
	}

	if (m_KsOEnum.IsEmpty())
		GetDlgItem(IDC_PB_KS)->EnableWindow(FALSE);
	else if (!bOSet)
	{
		CheckRadioButton(IDC_PB_MM, IDC_PB_KX, IDC_PB_KS);
		OnBnClickedPbDriver(IDC_PB_KS);
		bOSet = TRUE;
	}

	if (m_AsioIEnum.IsEmpty())							// ASIO
		GetDlgItem(IDC_REC_ASIO)->EnableWindow(FALSE);
	else if (!bISet)
	{
		CheckRadioButton(IDC_REC_MM, IDC_REC_KX, IDC_REC_ASIO);
		OnBnClickedRecDriver(IDC_REC_ASIO);
		bISet = TRUE;
	}

	if (m_AsioOEnum.IsEmpty())
		GetDlgItem(IDC_PB_ASIO)->EnableWindow(FALSE);
	else if (!bOSet)
	{
		CheckRadioButton(IDC_PB_MM, IDC_PB_KX, IDC_PB_ASIO);
		OnBnClickedPbDriver(IDC_PB_ASIO);
		bOSet = TRUE;
	}

	if (m_KxEnum.IsEmpty())							// GPR
	{
		GetDlgItem(IDC_REC_KX)->EnableWindow(FALSE);
		GetDlgItem(IDC_PB_KX)->EnableWindow(FALSE);
	}
	else
	{
		if (!bISet)
		{
			CheckRadioButton(IDC_REC_MM, IDC_REC_KX, IDC_REC_KX);
			OnBnClickedRecDriver(IDC_REC_KS);
			bISet = TRUE;
		}

		if (!bOSet)
		{
			CheckRadioButton(IDC_PB_MM, IDC_PB_KX, IDC_PB_KX);
			OnBnClickedPbDriver(IDC_PB_KX);
			bOSet = TRUE;
		}
	}

	if (!bISet || !bOSet)
	{
		AfxMessageBox("There is no recording or playback devices detected");
		EndDialog(-1);
		return FALSE;
	}

	if (TS_NONE == m_TimeSource)
		AfxMessageBox("There is no precise timers detected. Some features will be disabled.",
			MB_OK | MB_ICONERROR);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

BOOL CkxtestDlg::EnumDevices()
{
	START_TIMING;

	CThreadPool pool;

	pool.Add(new CWinThread(DetectClockMethod, this));

	EnumAsioDevices(this);

	pool.Add(new CWinThread(EnumKxDevices, this));
	pool.Add(new CWinThread(EnumMmDevices, this));
	pool.Add(new CWinThread(EnumKsDevices, this));
	//pool.Add(new CWinThread(EnumAsioDevices, this));
	//pool.Add(new CWinThread(EnumDsODevices, this));
	//pool.Add(new CWinThread(EnumDsIDevices, this));

	int nThreads = pool.GetCount();

	CProgressDlg dlg("Checking hardware...");

	dlg.Create(this);
	dlg.SetRange(0, nThreads);

	while (!pool.IsEmpty())
	{
		if (dlg.CheckCancelButton())
		{
			dlg.SetPos(0, "Cancelling...");
			pool.StopAll(2000);
			return FALSE;
		}

		pool.RemoveComlpeted();
		dlg.SetPos(nThreads - pool.GetCount());
	}

	REPORT_TIMING("EnumDevices");
	return TRUE;
}

void CkxtestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
		CDialog(IDD_ABOUTBOX).DoModal();
	else
		CDialog::OnSysCommand(nID, lParam);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CkxtestDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
		CDialog::OnPaint();
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CkxtestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CkxtestDlg::OnBnClickedPlay()
{
	if (m_bPlaying)
		PlayStop();
	else
		PlayStart();
}

void CkxtestDlg::PlayStart()
{
	ASSERT(!m_bPlaying);
	ASSERT(!RUNNING(m_PbThread));
	ASSERT(!RUNNING(m_RecThread));
	ASSERT(!RUNNING(m_FDThread));

	VERIFY(m_hPbSync = CreateEvent(NULL, FALSE, FALSE, NULL));
	VERIFY(m_hRecSync = CreateEvent(NULL, FALSE, FALSE, NULL));
	VERIFY(m_hNotify = CreateEvent(NULL, TRUE, FALSE, NULL));

	m_bPlaying = TRUE;
	m_Play.SetWindowText("Stop");

	if ((DRV_ASIO == m_PbDrv) && (DRV_ASIO == m_RecDrv) &&
		(((CAsioDesc*)m_PbDesc)->m_DrvId == ((CAsioDesc*)m_RecDesc)->m_DrvId))
	{
		m_FD = TRUE;
		VERIFY(m_FDThread.CreateThread());
	}
	else
	{
		m_FD = FALSE;
		VERIFY(m_PbThread.CreateThread());
	}

	if (WAIT_TIMEOUT == WaitForSingleObject(m_hPbSync, 2000))
	{
		TRACE("ERROR: Playback thread sync timeout\n");
		ASSERT(FALSE);
		exit(-1);
	}

	if (!m_bPlaying)	// playback thread failed to init
	{
		if (m_FD)
			CLEANUP_AUDIO_THREAD(m_FDThread, m_hPbSync)
		else
			CLEANUP_AUDIO_THREAD(m_PbThread, m_hPbSync)

		CloseHandle(m_hNotify);
		m_Play.SetWindowText("Play");
		PostMessage(WM_APP_CLEARALL);
		return;
	}

	if (!m_FD)
	{
		VERIFY(m_RecThread.CreateThread());

		if (WAIT_TIMEOUT == WaitForSingleObject(m_hRecSync, 2000))
		{
			TRACE("ERROR: Recording thread sync timeout\n");
			ASSERT(FALSE);
			exit(-1);
		}

		if (!m_bPlaying)	// recording thread failed to init
		{
			CLEANUP_AUDIO_THREAD(m_PbThread, m_hPbSync);
			CLEANUP_AUDIO_THREAD(m_RecThread, m_hRecSync);

			CloseHandle(m_hNotify);

			m_Play.SetWindowText("Play");
			PostMessage(WM_APP_CLEARALL);
			return;
		}
	}

	SetEvent(m_hNotify);	// start threads

	if (DRV_KX == m_RecDrv)
		GetKxRegs();
}

void CkxtestDlg::PlayStop()
{
	m_Lock.Lock();

	if (!m_bPlaying)
	{
		m_Lock.Unlock();
		return;
	}

	m_bPlaying = FALSE;

	m_Lock.Unlock();

	if (m_FD)
		CLEANUP_AUDIO_THREAD(m_FDThread, m_hPbSync)
	else
	{
		CLEANUP_AUDIO_THREAD(m_PbThread, m_hPbSync)
		CLEANUP_AUDIO_THREAD(m_RecThread, m_hRecSync)
	}

	CloseHandle(m_hNotify);

	m_KxInfo.SetWindowText(NULL);
	m_Play.SetWindowText("Play");
	PostMessage(WM_APP_CLEARALL);
}

void CkxtestDlg::OnDestroy()
{
    PlayStop();
	CDialog::OnDestroy();
}

CString bin2str(DWORD val, BOOL diff)
{
	char bin[33];
	strcpy_s(bin, diff ? "................................" :
		"00000000000000000000000000000000");
	
	for (int i = 31; i >= 0; i--)
	{
		if (val & 1)
			bin[i] = diff ? 'X' : '1';

		val >>= 1;
	}

	CString s(bin);

	s.Insert(24, ' ');
	s.Insert(16, ' ');
	s.Insert(8, ' ');

	return s;
}

void CkxtestDlg::UpdateFmtUi(BOOL bRec)
{
	int i;
	CDeviceDesc* pDesc = bRec ? m_RecDesc : m_PbDesc;
	ASSERT(pDesc);

	// sample rate has priority
	for (i = 0; i < 4; i++)
		GetDlgItem(bRec ? sr2RecId[i] : sr2PbId[i])->EnableWindow(
			(FMT_NONE != (pDesc->m_Fmt[i][0] | pDesc->m_Fmt[i][1] |
			pDesc->m_Fmt[i][2] | pDesc->m_Fmt[i][3])) ? TRUE : FALSE);

	// bit depth depends on current sample rate selected
	for (i = 0; i < 4; i++)
		GetDlgItem(bRec ? bd2RecId[i] : bd2PbId[i])->EnableWindow(
			(FMT_NONE != pDesc->m_Fmt[bRec ? m_RecSR : m_PbSR][i]) ? TRUE : FALSE);
}

void CkxtestDlg::OnCbnSelchangePbDevice()
{
	PlayStop();

	int nSel = m_PB_Device.GetCurSel();
	ASSERT(CB_ERR != nSel);

	m_PbDesc = (CDeviceDesc*)m_PB_Device.GetItemDataPtr(nSel);
	ASSERT(m_PbDesc);

	VERIFY(m_PbDesc->FindClosestMatch(m_PbSR, m_PbBD));

	CheckRadioButton(IDC_PB_44100, IDC_PB_192000, sr2PbId[m_PbSR]);
	CheckRadioButton(IDC_PB_16, IDC_PB_32, bd2PbId[m_PbBD]);
	UpdateFmtUi(FALSE);

	if (m_PbDesc->IsKindOf(RUNTIME_CLASS(CAsioDesc)))
		UpdateAsioCS(FALSE, reinterpret_cast<CAsioDesc*>(m_PbDesc));
}

void CkxtestDlg::OnBnClickedPbDriver(UINT nID)
{
	PlayStop();
	m_PB_Device.ResetContent();

	CDescList* pList;
	CDeviceDesc* pDesc;

	m_PbAsioCP.EnableWindow(IDC_PB_ASIO == nID ? TRUE : FALSE);
	m_PbAsioCS.EnableWindow(IDC_PB_ASIO == nID ? TRUE : FALSE);

	switch (nID)
	{
	case IDC_PB_MM:		m_PbDrv = DRV_MM;	pList = &m_MmOEnum;		break;
	case IDC_PB_DS:		m_PbDrv = DRV_DS;	pList = &m_DsOEnum;		break;
	case IDC_PB_KS:		m_PbDrv = DRV_KS;	pList = &m_KsOEnum;		break;
	case IDC_PB_ASIO:	m_PbDrv = DRV_ASIO;	pList = &m_AsioOEnum;	break;
	case IDC_PB_KX:		m_PbDrv = DRV_KX;	pList = &m_KxEnum;		break;

	default:
		ASSERT(FALSE);
		return;
	}

	BOOL bPulse = ((m_PbDrv != DRV_KX) && (m_RecDrv != DRV_KX) &&
		(TS_NONE != m_TimeSource)) ? TRUE : FALSE;

	m_LPulse.EnableWindow(bPulse);
	m_RPulse.EnableWindow(bPulse);

	LISTPOS rPos = pList->GetHeadPosition();

	while (pList->GetNext(rPos, &pDesc))
		m_PB_Device.SetItemDataPtr(m_PB_Device.AddString(pDesc->m_Name), pDesc);

	m_PB_Device.SetCurSel(0);
	OnCbnSelchangePbDevice();
}

void CkxtestDlg::OnBnClickedPbSr(UINT nID)
{
	PlayStop();

	switch (nID)
	{
	case IDC_PB_44100:	m_PbSR = SR_44100;	break;
	case IDC_PB_48000:	m_PbSR = SR_48000;	break;
	case IDC_PB_96000:	m_PbSR = SR_96000;	break;
	case IDC_PB_192000:	m_PbSR = SR_192000;	break;
	default:
		ASSERT(FALSE);
	}

	int nSel = m_PB_Device.GetCurSel();
	ASSERT(CB_ERR != nSel);

	VERIFY(m_PbDesc->FindClosestMatch(m_PbSR, m_PbBD));

	CheckRadioButton(IDC_PB_16, IDC_PB_32, bd2PbId[m_PbBD]);
	UpdateFmtUi(FALSE);
}

void CkxtestDlg::OnBnClickedPbBd(UINT nID)
{
	PlayStop();

	switch (nID)
	{
	case IDC_PB_16:		m_PbBD = BD_16;		break;
	case IDC_PB_24:		m_PbBD = BD_24;		break;
	case IDC_PB_24_32:	m_PbBD = BD_24_32;	break;
	case IDC_PB_32:		m_PbBD = BD_32;		break;
	default:
		ASSERT(FALSE);
	}
}

void CkxtestDlg::OnCbnSelchangeRecDevice()
{
	PlayStop();

	int nSel = m_Rec_Device.GetCurSel();
	ASSERT(CB_ERR != nSel);

	m_RecDesc = (CDeviceDesc*)m_Rec_Device.GetItemDataPtr(nSel);
	ASSERT(m_RecDesc);

	VERIFY(m_RecDesc->FindClosestMatch(m_RecSR, m_RecBD));

	CheckRadioButton(IDC_REC_44100, IDC_REC_192000, sr2RecId[m_RecSR]);
	CheckRadioButton(IDC_REC_16, IDC_REC_32, bd2RecId[m_RecBD]);
	UpdateFmtUi(TRUE);

	if (m_RecDesc->IsKindOf(RUNTIME_CLASS(CAsioDesc)))
		UpdateAsioCS(TRUE, reinterpret_cast<CAsioDesc*>(m_RecDesc));
}

void CkxtestDlg::OnBnClickedRecDriver(UINT nID)
{
	PlayStop();
	m_Rec_Device.ResetContent();

	CDescList* pList;
	CDeviceDesc* pDesc;

	m_RecAsioCP.EnableWindow(IDC_REC_ASIO == nID ? TRUE : FALSE);
	m_RecAsioCS.EnableWindow(IDC_REC_ASIO == nID ? TRUE : FALSE);

	switch (nID)
	{
	case IDC_REC_MM:	m_RecDrv = DRV_MM;		pList = &m_MmIEnum;		break;
	case IDC_REC_DS:	m_RecDrv = DRV_DS;		pList = &m_DsIEnum;		break;
	case IDC_REC_KS:	m_RecDrv = DRV_KS;		pList = &m_KsIEnum;		break;
	case IDC_REC_ASIO:	m_RecDrv = DRV_ASIO;	pList = &m_AsioIEnum;	break;
	case IDC_REC_KX:	m_RecDrv = DRV_KX;		pList = &m_KxEnum;		break;

	default:
		ASSERT(FALSE);
		return;
	}

	BOOL bPulse = ((m_PbDrv != DRV_KX) && (m_RecDrv != DRV_KX) &&
		(TS_NONE != m_TimeSource)) ? TRUE : FALSE;

	m_LPulse.EnableWindow(bPulse);
	m_RPulse.EnableWindow(bPulse);

	LISTPOS rPos = pList->GetHeadPosition();

	while (pList->GetNext(rPos, &pDesc))
		m_Rec_Device.SetItemDataPtr(m_Rec_Device.AddString(pDesc->m_Name), pDesc);

	m_Rec_Device.SetCurSel(0);
	OnCbnSelchangeRecDevice();
}

void CkxtestDlg::OnBnClickedRecSr(UINT nID)
{
	PlayStop();

	switch (nID)
	{
	case IDC_REC_44100:		m_RecSR = SR_44100;		break;
	case IDC_REC_48000:		m_RecSR = SR_48000;		break;
	case IDC_REC_96000:		m_RecSR = SR_96000;		break;
	case IDC_REC_192000:	m_RecSR = SR_192000;	break;
	default:
		ASSERT(FALSE);
	}

	int nSel = m_Rec_Device.GetCurSel();
	ASSERT(CB_ERR != nSel);

	VERIFY(m_RecDesc->FindClosestMatch(m_RecSR, m_RecBD));

	CheckRadioButton(IDC_REC_16, IDC_REC_32, bd2RecId[m_RecBD]);
	UpdateFmtUi(TRUE);
}

void CkxtestDlg::OnBnClickedRecBd(UINT nID)
{
	PlayStop();

	switch (nID)
	{
	case IDC_REC_16:	m_RecBD = BD_16;	break;
	case IDC_REC_24:	m_RecBD = BD_24;	break;
	case IDC_REC_24_32:	m_RecBD = BD_24_32;	break;
	case IDC_REC_32:	m_RecBD = BD_32;	break;
	default:
		ASSERT(FALSE);
	}
}

void CkxtestDlg::UpdateAsioCS(BOOL bRec, CAsioDesc* pDesc)
{
	CComboBox* pCBox = bRec ? &m_RecAsioCS : &m_PbAsioCS;

	pCBox->ResetContent();

	for (int i = 0; i < pDesc->m_NumCS; i++)
	{
		pCBox->AddString(pDesc->m_ClockSources[i].name);

		if (ASIOTrue == pDesc->m_ClockSources[i].isCurrentSource)
			pCBox->SetCurSel(i);
	}
}

void CkxtestDlg::OnCbnSelchangePbAsiocs()
{
	PlayStop();

	int nSel = m_PB_Device.GetCurSel();
	ASSERT(CB_ERR != nSel);

	CAsioDesc* pDesc = (CAsioDesc*)m_PB_Device.GetItemDataPtr(nSel);

	ASSERT(pDesc);
	ASSERT_KINDOF(CAsioDesc, pDesc);

	int nCS = m_PbAsioCS.GetCurSel();
	ASSERT(CB_ERR != nCS);

	pDesc->m_CS = nCS;
}

void CkxtestDlg::OnCbnSelchangeRecAsiocs()
{
	PlayStop();

	int nSel = m_Rec_Device.GetCurSel();
	ASSERT(CB_ERR != nSel);

	CAsioDesc* pDesc = (CAsioDesc*)m_Rec_Device.GetItemDataPtr(nSel);

	ASSERT(pDesc);
	ASSERT_KINDOF(CAsioDesc, pDesc);

	int nCS = m_RecAsioCS.GetCurSel();
	ASSERT(CB_ERR != nCS);

	pDesc->m_CS = nCS;
}

void CkxtestDlg::OnBnClickedPbAsiocp()
{
	PlayStop();

	int nSel = m_PB_Device.GetCurSel();
	ASSERT(CB_ERR != nSel);

	CAsioDesc* pDesc = (CAsioDesc*)m_PB_Device.GetItemDataPtr(nSel);
	ASSERT(pDesc);
	ASSERT_KINDOF(CAsioDesc, pDesc);

	IASIO* iDrv;

	m_AsioDrivers.asioOpenDriver(pDesc->m_DrvId, (LPVOID*)&iDrv);
	ASSERT(iDrv);

	if ((ASIOTrue != iDrv->init(m_hwndMain)) || (ASE_OK != iDrv->controlPanel()))
		AfxMessageBox("ASIO Error", MB_OK | MB_ICONERROR);

	m_AsioDrivers.asioCloseDriver(pDesc->m_DrvId);
}

void CkxtestDlg::OnBnClickedRecAsiocp()
{
	PlayStop();

	int nSel = m_Rec_Device.GetCurSel();
	ASSERT(CB_ERR != nSel);

	CAsioDesc* pDesc = (CAsioDesc*)m_Rec_Device.GetItemDataPtr(nSel);
	ASSERT(pDesc);
	ASSERT_KINDOF(CAsioDesc, pDesc);

	IASIO* iDrv;

	m_AsioDrivers.asioOpenDriver(pDesc->m_DrvId, (LPVOID*)&iDrv);
	ASSERT(iDrv);

	if ((ASIOTrue != iDrv->init(m_hwndMain)) || (ASE_OK != iDrv->controlPanel()))
		AfxMessageBox("ASIO Error", MB_OK | MB_ICONERROR);

	m_AsioDrivers.asioCloseDriver(pDesc->m_DrvId);
}

void CkxtestDlg::OnBnClickedSettings()
{
	CSettingsDlg(this).DoModal();
}

LRESULT CkxtestDlg::OnShowSample(WPARAM wParam, LPARAM lParam)
{
	DWORD dwLSample, dwRSample;
	GetSample(dwLSample, dwRSample);

	m_LSrc.SetWindowText(bin2str(dwLSample));
	m_RSrc.SetWindowText(bin2str(dwRSample));

	CString s;

	s.Format("%08X (%.11f)", dwLSample, (double)(int)dwLSample / 0x80000000);
	m_LSrcVal.SetWindowText(s);

	s.Format("%08X (%.11f)", dwRSample, (double)(int)dwRSample / 0x80000000);
	m_RSrcVal.SetWindowText(s);

	return 0;
}

LRESULT CkxtestDlg::OnShowRec(WPARAM wParam, LPARAM lParam)
{
	DWORD dwLSample, dwRSample;
	DWORD dwLRecorded, dwRRecorded;

	GetSample(dwLSample, dwRSample);
	GetRecSample(dwLRecorded, dwRRecorded);

	m_LKx.SetWindowText(bin2str(dwLRecorded));
	m_LXor.SetWindowText(bin2str(dwLSample ^ dwLRecorded, TRUE));

	m_RKx.SetWindowText(bin2str(dwRRecorded));
	m_RXor.SetWindowText(bin2str(dwRSample ^ dwRRecorded, TRUE));

	CString s;

	double fRec = (double)(int)dwLRecorded / 0x80000000;
	double fDiff = fabs(fRec - (double)(int)dwLSample / 0x80000000);

	s.Format("%08X (%.11f)", dwLRecorded, fRec);
	m_LKxVal.SetWindowText(s);

	s.Format("%08X (%.11f)", abs((long)dwLRecorded - (long)dwLSample), fDiff);
	m_LDiffVal.SetWindowText(s);

	fRec = (double)(int)dwRRecorded / 0x80000000;
	fDiff = fabs(fRec - (double)(int)dwRSample / 0x80000000);

	s.Format("%08X (%.11f)", dwRRecorded, fRec);
	m_RKxVal.SetWindowText(s);

	s.Format("%08X (%.11f)", abs((long)dwRRecorded - (long)dwRSample), fDiff);
	m_RDiffVal.SetWindowText(s);

	return 0;
}

LRESULT CkxtestDlg::OnClearAll(WPARAM wParam, LPARAM lParam)
{
	m_LSrc.SetWindowText(NULL);
	m_LSrcVal.SetWindowText(NULL);
	m_LKx.SetWindowText(NULL);
	m_LKxVal.SetWindowText(NULL);
	m_LXor.SetWindowText(NULL);
	m_LDiffVal.SetWindowText(NULL);

	m_RSrc.SetWindowText(NULL);
	m_RSrcVal.SetWindowText(NULL);
	m_RKx.SetWindowText(NULL);
	m_RKxVal.SetWindowText(NULL);
	m_RXor.SetWindowText(NULL);
	m_RDiffVal.SetWindowText(NULL);

	return 0;
}

void CkxtestDlg::OnBnClickedLInc()
{
	CAutoLock lock(m_Lock);

	switch (m_PbBD)
	{
	case BD_16:		m_LSample = (DWORD)(HIWORD(m_LSample) + 1) << 16; break;
	case BD_24:
	case BD_24_32:	m_LSample = (((int)m_LSample >> 8) + 1) << 8; break;
	case BD_32:		m_LSample++; break;
	}
}

void CkxtestDlg::OnBnClickedLDec()
{
	CAutoLock lock(m_Lock);

	switch (m_PbBD)
	{
	case BD_16:		m_LSample = (DWORD)(HIWORD(m_LSample) - 1) << 16; break;
	case BD_24:
	case BD_24_32:	m_LSample = (((int)m_LSample >> 8) - 1) << 8; break;
	case BD_32:		m_LSample--; break;
	}
}

void CkxtestDlg::OnBnClickedLRol()
{
	CAutoLock lock(m_Lock);

	DWORD temp = m_LSample;

	if (BD_16 == m_PbBD)
	{
		__asm rol word ptr temp + 2, 1;
		temp &= 0xFFFF0000;
	}
	else
		__asm rol temp, 1;

	m_LSample = temp;
}

void CkxtestDlg::OnBnClickedLRor()
{
	CAutoLock lock(m_Lock);

	DWORD temp = m_LSample;

	if (BD_16 == m_PbBD)
	{
		__asm ror word ptr temp + 2, 1;
		temp &= 0xFFFF0000;
	}
	else
		__asm ror temp, 1;

	m_LSample = temp;
}

void CkxtestDlg::OnBnClickedLZero()
{
	CAutoLock lock(m_Lock);

	m_LSample = 0;
}

void CkxtestDlg::OnBnClickedLAsk()
{
	CInputDlg dlg;

	if (IDOK != dlg.DoModal())
		return;

	CAutoLock lock(m_Lock);

	sscanf_s(dlg.m_Sample, "%X", &m_LSample);

	switch (m_PbBD)
	{
	case BD_16:		m_LSample <<= 16; break;
	case BD_24:
	case BD_24_32:	m_LSample <<= 8; break;
	}
}

void CkxtestDlg::OnBnClickedLRnd()
{
	CAutoLock lock(m_Lock);

	if (BD_16 == m_PbBD)
		m_LSample = (0xFFFFi64 * rand() / RAND_MAX) << 16;
	else
		m_LSample = 0xFFFFFFFFi64 * rand() / RAND_MAX;
}

void CkxtestDlg::OnBnClickedLPulse()
{
	PlayStop();

	m_bPulse = (BST_CHECKED == m_LPulse.GetCheck()) ? TRUE : FALSE;
}

void CkxtestDlg::OnBnClickedRInc()
{
	CAutoLock lock(m_Lock);

	switch (m_PbBD)
	{
	case BD_16:		m_RSample = (DWORD)(HIWORD(m_RSample) + 1) << 16; break;
	case BD_24:
	case BD_24_32:	m_RSample = (((int)m_RSample >> 8) + 1) << 8; break;
	case BD_32:		m_RSample++; break;
	}
}

void CkxtestDlg::OnBnClickedRDec()
{
	CAutoLock lock(m_Lock);

	switch (m_PbBD)
	{
	case BD_16:		m_RSample = (DWORD)(HIWORD(m_RSample) - 1) << 16; break;
	case BD_24:
	case BD_24_32:	m_RSample = (((int)m_RSample >> 8) - 1) << 8; break;
	case BD_32:		m_RSample--; break;
	}
}

void CkxtestDlg::OnBnClickedRRol()
{
	CAutoLock lock(m_Lock);

	DWORD temp = m_RSample;

	if (BD_16 == m_PbBD)
	{
		__asm rol word ptr temp + 2, 1;
		temp &= 0xFFFF0000;
	}
	else
		__asm rol temp, 1;

	m_RSample = temp;
}

void CkxtestDlg::OnBnClickedRRor()
{
	CAutoLock lock(m_Lock);

	DWORD temp = m_RSample;

	if (BD_16 == m_PbBD)
	{
		__asm ror word ptr temp + 2, 1;
		temp &= 0xFFFF0000;
	}
	else
		__asm ror temp, 1;

	m_RSample = temp;
}

void CkxtestDlg::OnBnClickedRZero()
{
	CAutoLock lock(m_Lock);

	m_RSample = 0;
}

void CkxtestDlg::OnBnClickedRAsk()
{
	CInputDlg dlg;

	if (IDOK != dlg.DoModal())
		return;

	CAutoLock lock(m_Lock);

	sscanf_s(dlg.m_Sample, "%X", &m_RSample);

	switch (m_PbBD)
	{
	case BD_16:		m_RSample <<= 16; break;
	case BD_24:
	case BD_24_32:	m_RSample <<= 8; break;
	}
}

void CkxtestDlg::OnBnClickedRRnd()
{
	CAutoLock lock(m_Lock);

	if (BD_16 == m_PbBD)
		m_RSample = (0xFFFFi64 * rand() / RAND_MAX) << 16;
	else
		m_RSample = 0xFFFFFFFFi64 * rand() / RAND_MAX;
}

void CkxtestDlg::OnBnClickedRPulse()
{
	PlayStop();

	m_bPulse = (BST_CHECKED == m_RPulse.GetCheck()) ? TRUE : FALSE;
}

void CkxtestDlg::GetKxRegs()
{
	ASSERT(m_RecDesc);
	ASSERT_KINDOF(CKxDesc, m_RecDesc);

	iKX& iKX = ((CKxDesc*)m_RecDesc)->m_iKX;

	if (1 != iKX.get_dsp())	// 10k2?
		return;

	kx_voice_info vi[64];
	iKX.get_voice_info(vi);

	int i, v = -1;
	for (i = 0; i < 64; i++)
		if (vi[i].usage & VOICE_USAGE_ASIO)
		{
			v = i;
			break;
		};

	if (v < 0)
		return;

	if (!v && (vi[62].usage & VOICE_USAGE_ASIO))
		v = 62;		// wrapped

	CString msg = iKX.get_device_name();
	msg += "\r\nCh  PTAB         VTFT        FXRT1        FXRT2";

	for (i = 0; i < 4; i++)
	{
		int ch = (v + i) & 63;

		if (!(vi[ch].usage & VOICE_USAGE_ASIO))
			break;	// 16-bit

		dword ptab, vtft, fxrt1, fxrt2;

		iKX.ptr_read(1, ch, &ptab);
		iKX.ptr_read(3, ch, &vtft);
		iKX.ptr_read(0x7e, ch, &fxrt1);
		iKX.ptr_read(0x7c, ch, &fxrt2);

		char s[64];
		sprintf_s(s, sizeof(s), "\r\n%02x  %08x  %08x  %08x  %08x",
			ch, ptab, vtft, fxrt1, fxrt2);

		msg += s;
	}

	m_KxInfo.SetWindowText(msg);
}

void CkxtestDlg::OnStnClickedKxinfo()
{
	if (DRV_KX == m_RecDrv)
		GetKxRegs();

	CString msg;
	m_KxInfo.GetWindowText(msg);

	if (msg.IsEmpty() || !OpenClipboard())
		return;

	EmptyClipboard();

	HGLOBAL hMsg = GlobalAlloc(GMEM_MOVEABLE, msg.GetLength() + 1);

	char* s = (char*)GlobalLock(hMsg);

	strcpy_s(s, msg.GetLength() + 1, msg);

	GlobalUnlock(hMsg);
	SetClipboardData(CF_TEXT, hMsg);

	CloseClipboard();
}
