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


// SettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "kxtest.h"
#include "kxtestDlg.h"

#include "SettingsDlg.h"


// CSettingsDlg dialog

IMPLEMENT_DYNAMIC(CSettingsDlg, CDialog)
CSettingsDlg::CSettingsDlg(CkxtestDlg* pParent)
	: CDialog(CSettingsDlg::IDD, NULL)
	, m_pParent(pParent)
	, m_DisableTsc(FALSE)
{
}

void CSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_DISABLETSC, m_DisableTsc);
}


BEGIN_MESSAGE_MAP(CSettingsDlg, CDialog)
END_MESSAGE_MAP()


// CSettingsDlg message handlers

BOOL CSettingsDlg::OnInitDialog()
{
	static const UINT tm2id[3] = {IDC_CALCSTART, IDC_NOTIFY, IDC_POLLING};

	m_DisableTsc = m_pParent->m_bDisableTsc;

	CDialog::OnInitDialog();

	CheckRadioButton(IDC_CALCSTART, IDC_POLLING, tm2id[m_pParent->m_TimingMethod]);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSettingsDlg::OnOK()
{
	if (!UpdateData(TRUE))
		return;

	m_pParent->m_bDisableTsc = m_DisableTsc;

	switch (GetCheckedRadioButton(IDC_CALCSTART, IDC_POLLING))
	{
	case IDC_CALCSTART:	m_pParent->m_TimingMethod = TM_CALCSTART;	break;
	case IDC_NOTIFY:	m_pParent->m_TimingMethod = TM_NOTIFY;		break;
	case IDC_POLLING:	m_pParent->m_TimingMethod = TM_POLLING;		break;
	default:
		ASSERT(FALSE);
	}

	theApp.WriteProfileInt("Settings", "Disable TSC", m_DisableTsc);
	theApp.WriteProfileInt("Settings", "Time Source", m_pParent->m_TimingMethod);

	EndDialog(IDOK);
}
