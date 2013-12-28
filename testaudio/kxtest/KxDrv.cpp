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

CKxPbDrv::CKxPbDrv(CkxtestDlg* pDlg)
	: CPbDriver(pDlg)
	, m_pDesc(NULL)
{
	ASSERT(m_pDlg);
}

CKxPbDrv::~CKxPbDrv()
{
}

BOOL CKxPbDrv::Init()
{
	ASSERT(m_pDlg->m_PbDesc);
	ASSERT_KINDOF(CKxDesc, m_pDlg->m_PbDesc);

	m_pDesc = (CKxDesc*)m_pDlg->m_PbDesc;

	return TRUE;
}

BOOL CKxPbDrv::Start()
{
	return TRUE;
}

BOOL CKxPbDrv::Stop()
{
	return TRUE;
}

BOOL CKxPbDrv::Update(DWORD dwLSample, DWORD dwRSample)
{
	m_pDesc->m_iKX.set_dsp_register(m_pDesc->m_Plugin, m_pDesc->m_LPbGpr, dwLSample);
	m_pDesc->m_iKX.set_dsp_register(m_pDesc->m_Plugin, m_pDesc->m_RPbGpr, dwRSample);

	Sleep(PB_WAIT);
	return TRUE;
}

CKxRecDrv::CKxRecDrv(CkxtestDlg* pDlg)
	: CRecDriver(pDlg)
	, m_pDesc(NULL)
{
	ASSERT(m_pDlg);
}

CKxRecDrv::~CKxRecDrv()
{
}

BOOL CKxRecDrv::Init()
{
	ASSERT(m_pDlg->m_RecDesc);
	ASSERT_KINDOF(CKxDesc, m_pDlg->m_RecDesc);

	m_pDesc = (CKxDesc*)m_pDlg->m_RecDesc;

	return TRUE;
}

BOOL CKxRecDrv::Start()
{
	return TRUE;
}

BOOL CKxRecDrv::Stop()
{
	return TRUE;
}

BOOL CKxRecDrv::Update(DWORD &dwLRec, DWORD &dwRRec)
{
	m_pDesc->m_iKX.get_dsp_register(m_pDesc->m_Plugin, m_pDesc->m_LRecGpr, (dword*)&dwLRec);
	m_pDesc->m_iKX.get_dsp_register(m_pDesc->m_Plugin, m_pDesc->m_RRecGpr, (dword*)&dwRRec);

	Sleep(REC_WAIT);
	return TRUE;
}
