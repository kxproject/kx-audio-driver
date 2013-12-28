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

// InputDlg.cpp : implementation file
//

#include "stdafx.h"
#include "kxtest.h"
#include "InputDlg.h"


// CInputDlg dialog

IMPLEMENT_DYNAMIC(CInputDlg, CDialog)
CInputDlg::CInputDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInputDlg::IDD, pParent)
	, m_Sample(_T(""))
{
}

void CInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SAMPLE, m_Sample);
	DDV_MaxChars(pDX, m_Sample, 10);
}


BEGIN_MESSAGE_MAP(CInputDlg, CDialog)
END_MESSAGE_MAP()


// CInputDlg message handlers
