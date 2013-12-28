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

const UINT sr2PbId[4] = {IDC_PB_44100, IDC_PB_48000, IDC_PB_96000, IDC_PB_192000};
const UINT sr2RecId[4] = {IDC_REC_44100, IDC_REC_48000, IDC_REC_96000, IDC_REC_192000};
const UINT bd2PbId[4] = {IDC_PB_16, IDC_PB_24, IDC_PB_24_32, IDC_PB_32};
const UINT bd2RecId[4] = {IDC_REC_16, IDC_REC_24, IDC_REC_24_32, IDC_REC_32};

const DWORD i2sr[4] = {44100, 48000, 96000, 192000};
const WORD i2bd1[4] = {16, 24, 32, 32};
const WORD i2bd2[4] = {16, 24, 24, 32};

BOOL CDeviceDesc::FindFirstFormat(int* sr, int* bd)
{
	int i;

	for (i = 0; i < 16; i++)
		if (FMT_NONE != ((int*)m_Fmt)[i])
			break;

	if (16 == i)
		return FALSE;

	if (sr) *sr = i / 4;
	if (bd) *bd = i % 4;

	return TRUE;
}

BOOL CDeviceDesc::FindClosestMatch(int& sr, int& bd)
{
	if (FMT_NONE != m_Fmt[sr][bd])
		return TRUE;

	int i;

	for (i = bd; i < 4; i++)
		if (FMT_NONE != m_Fmt[sr][i])
		{
			bd = i;
			return TRUE;
		}

	for (i = bd - 1; i >= 0; i--)
		if (FMT_NONE != m_Fmt[sr][i])
		{
			bd = i;
			return TRUE;
		}

	for (i = sr; i < 4; i++)
		if (FMT_NONE != m_Fmt[i][bd])
		{
			sr = i;
			return TRUE;
		}

	for (i = sr - 1; i >= 0; i--)
		if (FMT_NONE != m_Fmt[i][bd])
		{
			sr = i;
			return TRUE;
		}

	return FindFirstFormat(&sr, &bd);
}

IMPLEMENT_DYNAMIC(CDeviceDesc, CObject)
IMPLEMENT_DYNAMIC(CMmDesc, CDeviceDesc)
IMPLEMENT_DYNAMIC(CDsDesc, CDeviceDesc)
IMPLEMENT_DYNAMIC(CKsDesc, CDeviceDesc)
IMPLEMENT_DYNAMIC(CAsioDesc, CDeviceDesc)
IMPLEMENT_DYNAMIC(CKxDesc, CDeviceDesc)

THREAD_IMP CkxtestDlg::EnumMmDevices(LPVOID pParam)
{
	START_TIMING;

	ASSERT(pParam);
	CkxtestDlg& dlg = *reinterpret_cast<CkxtestDlg*>(pParam);

	UINT i;
	CMmDesc* pDesc;

	for (i = 0; i < waveInGetNumDevs(); i++)
	{
		WAVEINCAPS wiCaps;
		waveInGetDevCaps(i, &wiCaps, sizeof(wiCaps));

		VERIFY(pDesc = new CMmDesc(i, wiCaps.szPname));

		dlg.GetMmCaps(pDesc, TRUE);

		if (pDesc->FindFirstFormat())
			dlg.m_MmIEnum.AddTail(pDesc);
		else
			delete pDesc;

		CHECK_QUIT(0);
	}

	for (i = 0; i < waveOutGetNumDevs(); i++)
	{
		WAVEOUTCAPS woCaps;
		waveOutGetDevCaps(i, &woCaps, sizeof(woCaps));

		VERIFY(pDesc = new CMmDesc(i, woCaps.szPname));

		dlg.GetMmCaps(pDesc, FALSE);

		if (pDesc->FindFirstFormat())
			dlg.m_MmOEnum.AddTail(pDesc);
		else
			delete pDesc;

		CHECK_QUIT(0);
	}

	REPORT_TIMING("EnumMmDevices");
	return 0;
}

THREAD_IMP CkxtestDlg::EnumDsIDevices(LPVOID pParam)
{
	START_TIMING;

	ASSERT(pParam);
	CkxtestDlg& dlg = *reinterpret_cast<CkxtestDlg*>(pParam);

	DirectSoundCaptureEnumerate(&DsIEnumProc, &dlg.m_DsIEnum);

	REPORT_TIMING("EnumDsIDevices");
	return 0;
}

THREAD_IMP CkxtestDlg::EnumDsODevices(LPVOID pParam)
{
	START_TIMING;

	ASSERT(pParam);
	CkxtestDlg& dlg = *reinterpret_cast<CkxtestDlg*>(pParam);

	DirectSoundEnumerate(&DsOEnumProc, &dlg.m_DsOEnum);

	REPORT_TIMING("EnumDsODevices");
	return 0;
}

THREAD_IMP CkxtestDlg::EnumKsDevices(LPVOID pParam)
{
	START_TIMING;

	ASSERT(pParam);
	CkxtestDlg& dlg = *reinterpret_cast<CkxtestDlg*>(pParam);

	CKsFilter* pFilter;
	CKsDesc* pDesc;
	LISTPOS rPos;
	HRESULT hr;

	CKsEnumFilters ksEnum(&hr);
	if (!SUCCEEDED(hr))
		return 0;
/*
	GUID  aguidIEnumCats[] = {STATIC_KSCATEGORY_AUDIO, STATIC_KSCATEGORY_CAPTURE};
	ksEnum.EnumFilters(eAudCap, aguidIEnumCats, 2, TRUE, FALSE, TRUE);

	CHECK_QUIT(0);

	rPos = ksEnum.m_listFilters.GetHeadPosition();

	while (ksEnum.m_listFilters.GetNext(rPos, &pFilter))
	{
		VERIFY(pDesc = new CKsDesc(pFilter));

		dlg.GetIKsCaps((CKsAudCapFilter*)pFilter, pDesc);

		if (pDesc->FindFirstFormat())
			dlg.m_KsIEnum.AddTail(pDesc);
		else
			delete pDesc;

		CHECK_QUIT(0);
	}
*/
	GUID  aguidOEnumCats[] = {STATIC_KSCATEGORY_AUDIO, STATIC_KSCATEGORY_RENDER};
	ksEnum.EnumFilters(eAudRen, aguidOEnumCats, 2, TRUE, FALSE, TRUE);

	CHECK_QUIT(0);

	rPos = ksEnum.m_listFilters.GetHeadPosition();

	while (ksEnum.m_listFilters.GetNext(rPos, &pFilter))
	{
		VERIFY(pDesc = new CKsDesc(pFilter));

		dlg.GetOKsCaps((CKsAudRenFilter*)pFilter, pDesc);

		if (pDesc->FindFirstFormat())
			dlg.m_KsOEnum.AddTail(pDesc);
		else
			delete pDesc;

		CHECK_QUIT(0);
	}

	REPORT_TIMING("EnumKsDevices");
	return 0;
}

THREAD_IMP CkxtestDlg::EnumAsioDevices(LPVOID pParam)
{
	START_TIMING;

	BOOL bThread = (AfxGetThread() == AfxGetApp()) ? FALSE : TRUE;
	TRACE("ASIO detection%sthreaded\n", bThread ? " " : " not ");

	if (bThread)
		CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	ASSERT(pParam);
	CkxtestDlg& dlg = *reinterpret_cast<CkxtestDlg*>(pParam);

	for (int i = 0; i < dlg.m_AsioDrivers.asioGetNumDev(); i++)
	{
		IASIO* iDrv;
		dlg.m_AsioDrivers.asioOpenDriver(i, (LPVOID*)&iDrv);

		if (iDrv)
		{
			char sDrvName[128];
			long nInChannels, nOutChannels;

			dlg.m_AsioDrivers.asioGetDriverName(i, sDrvName, sizeof(sDrvName));

			if ((ASIOTrue == iDrv->init(m_hwndMain)) && (ASE_OK ==
				iDrv->getChannels(&nInChannels, &nOutChannels)))
			{
				int nCh;
				for (nCh = 0; nCh < nInChannels; nCh += 2)
				{
					ASIOChannelInfo info = {nCh, ASIOTrue};
					iDrv->getChannelInfo(&info);

					CString name;
					name.Format("%s %s", sDrvName, info.name);

					CAsioDesc* pDesc = new CAsioDesc(i, nCh, info.type, name);

					dlg.GetAsioCaps(iDrv, pDesc);

					if (pDesc->FindFirstFormat())
						dlg.m_AsioIEnum.AddTail(pDesc);
					else
						delete pDesc;
				}

				if (bThread)
					CHECK_QUIT(0);

				for (nCh = 0; nCh < nOutChannels; nCh += 2)
				{
					ASIOChannelInfo info = {nCh, ASIOFalse};
					iDrv->getChannelInfo(&info);

					CString name;
					name.Format("%s %s", sDrvName, info.name);

					CAsioDesc* pDesc = new CAsioDesc(i, nCh, info.type, name);

					dlg.GetAsioCaps(iDrv, pDesc);

					if (pDesc->FindFirstFormat())
						dlg.m_AsioOEnum.AddTail(pDesc);
					else
						delete pDesc;
				}

				if (bThread)
					CHECK_QUIT(0);
			}

			dlg.m_AsioDrivers.asioCloseDriver(i);
		}
	}

	if (bThread)
		CoUninitialize();

	REPORT_TIMING("EnumAsioDevices");
	return 0;
}

THREAD_IMP CkxtestDlg::EnumKxDevices(LPVOID pParam)
{
	START_TIMING;

	ASSERT(pParam);
	CkxtestDlg& dlg = *reinterpret_cast<CkxtestDlg*>(pParam);

	static const int kxfmt[4][4] = {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0};

	iKX ikx;

	for (int i = 0; !ikx.init(i); i++)
	{
		dsp_microcode mc;

		if(!ikx.enum_microcode("kxtest", &mc) && (mc.flag & MICROCODE_TRANSLATED) &&
			(mc.flag & MICROCODE_ENABLED) && !(mc.flag & MICROCODE_BYPASS))
		{
			dsp_code* code = (dsp_code*)malloc(mc.code_size);
			dsp_register_info* gprs = (dsp_register_info*)malloc(mc.info_size);

			if (!ikx.get_microcode(mc.pgm, code, mc.code_size, gprs, mc.info_size))
			{
				int il = -1;
				int ol = -1;
				int ir = -1;
				int or = -1;

				for (unsigned r = 0; r < mc.info_size / sizeof(dsp_register_info); r++)
				{
					if (!strcmp(gprs[r].name, "iL"))
						il = gprs[r].num;
					else if (!strcmp(gprs[r].name, "oL"))
						ol = gprs[r].num;
					else if (!strcmp(gprs[r].name, "iR"))
						ir = gprs[r].num;
					else if (!strcmp(gprs[r].name, "oR"))
						or = gprs[r].num;
				}

				if ((il != -1) && (ol != -1) && (ir != -1) && (or != -1))
				{
					ikx.close();

					CKxDesc* pDesc = new CKxDesc(i, ikx.get_device_name(),
						mc.pgm, il, ol, ir, or);

					memcpy(pDesc->m_Fmt, kxfmt, sizeof(pDesc->m_Fmt));

					dlg.m_KxEnum.AddTail(pDesc);
				}
			}

			free(gprs);
			free(code);
		}

		ikx.close();
	}

	REPORT_TIMING("EnumKxDevices");
	return 0;
}
