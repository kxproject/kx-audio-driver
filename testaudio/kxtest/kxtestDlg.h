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


// kxtestDlg.h : header file
//

#pragma once

#include "DevDesc.h"
#include "thread.h"
#include "driver.h"
#include "afxwin.h"


// time sources
//
#define	TS_NONE			0		// no suitable timers
#define	TS_PERF			1		// use windows performance counter
#define	TS_TSC			2		// use CPU's tsc counter

// playback timing method
//
#define	TM_CALCSTART	0		// calculate from buffer start time
#define	TM_NOTIFY		1		// use notification
#define	TM_POLLING		2		// continuous poll for position

// audio buffer helpers
//
void FillBuffer(LPVOID pBuffer, int nSize, DWORD dwLSample, DWORD dwRSample, WAVEFORMATEXTENSIBLE& wFmt);
void FillPulse(LPVOID pBuffer, int nSize, BOOL bStereo, WAVEFORMATEXTENSIBLE& wFmt);

CString bin2str(DWORD val, BOOL diff = FALSE);

#define WM_APP_SHOWSAMPLE	(WM_APP + 100)
#define WM_APP_SHOWREC		(WM_APP + 101)
#define WM_APP_CLEARALL		(WM_APP + 102)

// CkxtestDlg dialog
class CkxtestDlg : public CDialog
{
// Construction
public:
	CkxtestDlg(CWnd* pParent = NULL);	// standard constructor

	CFont	m_Font;

	AsioDriverList	m_AsioDrivers;

	CDescList		m_MmIEnum, m_MmOEnum;		// MM devices
	CDescList		m_DsIEnum, m_DsOEnum;		// DirectSound
	CDescList		m_KsIEnum, m_KsOEnum;		// Kernel Streaming
	CDescList		m_AsioIEnum, m_AsioOEnum;	// ASIO
	CDescList		m_KxEnum;					// list of kx cards

	THREAD EnumKxDevices(LPVOID pParam);
	THREAD EnumMmDevices(LPVOID pParam);
	THREAD EnumKsDevices(LPVOID pParam);
	THREAD EnumAsioDevices(LPVOID pParam);
	THREAD EnumDsIDevices(LPVOID pParam);		// takes a long time - separated
	THREAD EnumDsODevices(LPVOID pParam);

	BOOL EnumDevices();

	// enum helpers
	//
	static HWND m_hwndMain;

	static BOOL CALLBACK DsIEnumProc(LPGUID lpGUID,
		LPCTSTR lpszDesc,
		LPCTSTR lpszDrvName,
		LPVOID lpContext);

	static BOOL CALLBACK DsOEnumProc(LPGUID lpGUID,
		LPCTSTR lpszDesc,
		LPCTSTR lpszDrvName,
		LPVOID lpContext);

	void GetMmCaps(CMmDesc* pDesc, BOOL bRec);

	void GetIKsCaps(CKsAudCapFilter* pFilter, CKsDesc* pDesc);
	void GetOKsCaps(CKsAudRenFilter* pFilter, CKsDesc* pDesc);

	void GetAsioCaps(IASIO* iDrv, CAsioDesc* pDesc);

	// UI helpers
	//
	void UpdateFmtUi(BOOL bRec);
	void UpdateAsioCS(BOOL bRec, CAsioDesc* pDesc);

	DWORD	m_LSample, m_LRecSample;	// played and recorded samples
	DWORD	m_RSample, m_RRecSample;
	BOOL	m_bPulse;						// latency measurement

	BOOL	m_bDisableTsc;		// for APM-enabled systems which clock freq may change
	int		m_TimeSource;
	int		m_TimingMethod;

	DWORD64	m_CounterFreq;						// clocks per second

	DWORD64 GetClock();

	DWORD64	m_PbStartTime, m_PbPulseTime;
	DWORD64 m_RecStartTime;

	// find the most precise timer for the system
	// this is may take a long time for measuring CPU speed
	// so we run it in parallel thread
	THREAD DetectClockMethod(LPVOID pParam);

	volatile BOOL m_bPlaying;

	int		m_PbDrv, m_PbSR, m_PbBD;
	int		m_RecDrv, m_RecSR, m_RecBD;

	CDeviceDesc*	m_PbDesc;
	CDeviceDesc*	m_RecDesc;

	HANDLE	m_hPbSync, m_hRecSync, m_hNotify;	// thread syncronization

	CCriticalSection	m_Lock;

	inline void GetSample(DWORD &rL, DWORD &rR)
	{
		CAutoLock lock(m_Lock);

		rL = m_LSample;
		rR = m_RSample;
	};

	inline void GetRecSample(DWORD &rL, DWORD &rR)
	{
		CAutoLock lock(m_Lock);

		rL = m_LRecSample;
		rR = m_RRecSample;
	};

	inline void SetRecSample(DWORD dwLRecSample, DWORD dwRRecSample)
	{
		CAutoLock lock(m_Lock);

		m_LRecSample = dwLRecSample;
		m_RRecSample = dwRRecSample;
	};

	THREAD PbThread(LPVOID pParam);
	THREAD RecThread(LPVOID pParam);
	THREAD AsioFDThread(LPVOID pParam);

	CWinThread	m_PbThread, m_RecThread, m_FDThread;

	BOOL m_FD;	// full-duplex flag

	void PlayStart();
	void PlayStop();

// Dialog Data
	enum { IDD = IDD_KXTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedPlay();
	CButton m_Play;
	afx_msg void OnDestroy();
	CStatic m_LSrc;
	CStatic m_LKx;
	CStatic m_LXor;
	CStatic m_LSrcVal;
	CStatic m_LKxVal;
	CStatic m_LDiffVal;
	CButton m_LPulse;
	afx_msg void OnBnClickedLInc();
	afx_msg void OnBnClickedLDec();
	afx_msg void OnBnClickedLRol();
	afx_msg void OnBnClickedLRor();
	afx_msg void OnBnClickedLZero();
	afx_msg void OnBnClickedLAsk();
	afx_msg void OnBnClickedLRnd();
	afx_msg void OnBnClickedLPulse();
	CStatic m_RSrc;
	CStatic m_RKx;
	CStatic m_RXor;
	CStatic m_RSrcVal;
	CStatic m_RKxVal;
	CStatic m_RDiffVal;
	CButton m_RPulse;
	afx_msg void OnBnClickedRInc();
	afx_msg void OnBnClickedRDec();
	afx_msg void OnBnClickedRRol();
	afx_msg void OnBnClickedRRor();
	afx_msg void OnBnClickedRZero();
	afx_msg void OnBnClickedRAsk();
	afx_msg void OnBnClickedRRnd();
	afx_msg void OnBnClickedRPulse();
	CComboBox m_PB_Device;
	CComboBox m_Rec_Device;
	afx_msg void OnCbnSelchangePbDevice();
	afx_msg void OnBnClickedPbDriver(UINT nID);
	afx_msg void OnBnClickedPbSr(UINT nID);
	afx_msg void OnBnClickedPbBd(UINT nID);
	afx_msg void OnCbnSelchangeRecDevice();
	afx_msg void OnBnClickedRecDriver(UINT nID);
	afx_msg void OnBnClickedRecSr(UINT nID);
	afx_msg void OnBnClickedRecBd(UINT nID);
	CComboBox m_PbAsioCS;
	CComboBox m_RecAsioCS;
	afx_msg void OnCbnSelchangePbAsiocs();
	afx_msg void OnCbnSelchangeRecAsiocs();
	CButton m_PbAsioCP;
	CButton m_RecAsioCP;
	afx_msg void OnBnClickedPbAsiocp();
	afx_msg void OnBnClickedRecAsiocp();
	afx_msg void OnBnClickedSettings();
	afx_msg LRESULT OnShowSample(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnShowRec(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnClearAll(WPARAM wParam, LPARAM lParam);
	CStatic m_KxInfo;
	void GetKxRegs();
	afx_msg void OnStnClickedKxinfo();
};
