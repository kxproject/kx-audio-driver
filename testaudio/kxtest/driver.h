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

#define PB_WAIT			20		// 100 hz polling
#define REC_WAIT		20		// 100 hz polling

#define MM_BUFFER		80		// milliseconds
#define DS_BUFFER		40
#define KS_BUFFER		40

#define BUFSIZE(size, fmt) ((fmt).Format.nAvgBytesPerSec * (size) / 1000)

class CkxtestDlg;

// abstract driver classes
//
class CDriver
{
public:
	CkxtestDlg*	m_pDlg;

	CDriver(CkxtestDlg* pDlg)
		: m_pDlg(pDlg)
	{
	};

	virtual ~CDriver() {};

	virtual BOOL Init()			= 0;
	virtual BOOL Start()		= 0;
	virtual BOOL Stop()			= 0;
};

class CPbDriver: public CDriver
{
public:
	CPbDriver(CkxtestDlg* pDlg)
		: CDriver(pDlg)
	{
	};

	virtual BOOL Update(DWORD dwLSample, DWORD dwRSample) = 0;
};

class CRecDriver: public CDriver
{
public:
	CRecDriver(CkxtestDlg* pDlg)
		: CDriver(pDlg)
	{
	};

	virtual BOOL Update(DWORD &dwLRec, DWORD &dwRRec) = 0;
};

// Multimedia (legacy PCM) drivers
//
class CMmPbDrv: public CPbDriver
{
public:
	WAVEFORMATEXTENSIBLE	m_Fmt;
	HWAVEOUT				m_Handle;
	WAVEHDR					m_Header;
	void*					m_pBuffer;
	long					m_Size;

	CMmPbDrv(CkxtestDlg* pDlg);
	~CMmPbDrv();

	BOOL Init();
	BOOL Start();
	BOOL Stop();
	BOOL Update(DWORD dwLSample, DWORD dwRSample);
};

class CMmRecDrv: public CRecDriver
{
public:
	WAVEFORMATEXTENSIBLE	m_Fmt;
	HWAVEIN					m_Handle;
	WAVEHDR					m_Hdr1, m_Hdr2;
	void*					m_pB1;
	void*					m_pB2;
	void*					m_pBuffer;

	CMmRecDrv(CkxtestDlg* pDlg);
	~CMmRecDrv();

	BOOL Init();
	BOOL Start();
	BOOL Stop();
	BOOL Update(DWORD &dwLRec, DWORD &dwRRec);
};

// DirectSound drivers
//
class CDsPbDrv: public CPbDriver
{
public:
	WAVEFORMATEXTENSIBLE	m_Fmt;
	LPDIRECTSOUND			m_pDS;
	LPDIRECTSOUNDBUFFER		m_pDsBuf;
	DSBUFFERDESC			m_DsDesc;

	CDsPbDrv(CkxtestDlg* pDlg);
	~CDsPbDrv();

	BOOL Init();
	BOOL Start();
	BOOL Stop();
	BOOL Update(DWORD dwLSample, DWORD dwRSample);
};

class CDsRecDrv: public CRecDriver
{
public:
	WAVEFORMATEXTENSIBLE		m_Fmt;
	LPDIRECTSOUNDCAPTURE		m_pDS;
	LPDIRECTSOUNDCAPTUREBUFFER	m_pDsBuf;
	DSCBUFFERDESC				m_DsDesc;

	CDsRecDrv(CkxtestDlg* pDlg);
	~CDsRecDrv();

	BOOL Init();
	BOOL Start();
	BOOL Stop();
	BOOL Update(DWORD &dwLRec, DWORD &dwRRec);
};

// Kernel Streaming drivers
//
class CKsPbDrv: public CPbDriver
{
public:
	WAVEFORMATEXTENSIBLE	m_Fmt;
	CKsAudRenFilter*		m_Filter;
	CKsAudRenPin*			m_Pin;
	KSSTREAM_HEADER			m_Header;
	OVERLAPPED				m_Ovr;
	void*					m_pBuffer;

	CKsPbDrv(CkxtestDlg* pDlg);
	~CKsPbDrv();

	BOOL Init();
	BOOL Start();
	BOOL Stop();
	BOOL Update(DWORD dwLSample, DWORD dwRSample);
};

class CKsRecDrv: public CRecDriver
{
public:
	WAVEFORMATEXTENSIBLE		m_Fmt;
	CKsAudCapFilter*		m_Filter;
	CKsAudCapPin*			m_Pin;
	KSSTREAM_HEADER			m_Header;
	OVERLAPPED				m_Ovr;
	void*					m_pBuffer;

	CKsRecDrv(CkxtestDlg* pDlg);
	~CKsRecDrv();

	BOOL Init();
	BOOL Start();
	BOOL Stop();
	BOOL Update(DWORD &dwLRec, DWORD &dwRRec);
};

// ASIO drivers
//
class CAsioPbDrv: public CPbDriver
{
public:
	CAsioDesc*	m_pDesc;
	IASIO*		m_pIDrv;
	long		m_Size;
	void		*m_L1, *m_L2, *m_R1, *m_R2;
	BOOL		m_bSleep;

	CAsioPbDrv(CkxtestDlg* pDlg);
	~CAsioPbDrv();

	BOOL Init();
	BOOL Start();
	BOOL Stop();
	BOOL Update(DWORD dwLSample, DWORD dwRSample);

	void AsioFillBuffer(void* pBuffer, DWORD dwSample);
};

class CAsioRecDrv: public CRecDriver
{
public:
	CAsioDesc*			m_pDesc;
	IASIO*				m_pIDrv;
	BOOL				m_CoUsed;
	CCriticalSection	m_Lock;

	CAsioRecDrv(CkxtestDlg* pDlg);
	~CAsioRecDrv();

	BOOL Init();
	BOOL Start();
	BOOL Stop();
	BOOL Update(DWORD &dwLRec, DWORD &dwRRec);
};

// ASIO full duplex driver
//
class CAsioFDDrv: public CAsioPbDrv, public CAsioRecDrv
{
public:
	CAsioFDDrv(CkxtestDlg* pDlg);

	DWORD GetSleep();
	BOOL Init();
};

// Kx DSP (direct access to GPR's) drivers
//
class CKxPbDrv: public CPbDriver
{
public:
	CKxDesc*	m_pDesc;

	CKxPbDrv(CkxtestDlg* pDlg);
	~CKxPbDrv();

	BOOL Init();
	BOOL Start();
	BOOL Stop();
	BOOL Update(DWORD dwLSample, DWORD dwRSample);
};

class CKxRecDrv: public CRecDriver
{
public:
	CKxDesc* m_pDesc;

	CKxRecDrv(CkxtestDlg* pDlg);
	~CKxRecDrv();

	BOOL Init();
	BOOL Start();
	BOOL Stop();
	BOOL Update(DWORD &dwLRec, DWORD &dwRRec);
};
