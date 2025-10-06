// OptionsTimeDlg.cpp : Time option dialog handlers
//
#include "stdafx.h"
#include "resource.h"
#include "OptionsDlg.h"
#include "OptionsTimeDlg.h"
#include <rcommon/RColorWnd.h>
#include <rcommon/SafeWndProc.hpp>
#include <commctrl.h>

inline static void OnInitDialog(HWND a_hDlg, CRegData::RTimeRegData* a_pData);
static INT_PTR OptionsTimeDlgProc(HWND a_hDlg, UINT a_iMsg, WPARAM a_wParam, LPARAM a_lParam);
inline static void SetCtrlValues(HWND a_hDlg);
static CRegData::RTimeRegData* GetRData(HWND a_hDlg);
inline static void GetCtrlValues(HWND a_hDlg);


HWND CreateOptTimeDlg(HWND a_hParent, CRegData::RBaseRegData* a_pData, LPVOID a_pObj)
{
	return ::CreateDialogParam(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_OPT_TIME), 
		a_hParent, SafeDialogProc<OptionsTimeDlgProc>, reinterpret_cast<LPARAM>(a_pData));
}


static void OnInitDialog(HWND a_hDlg, CRegData::RTimeRegData* a_pData)
{
	::SetWindowLongPtr(a_hDlg, GWL_USERDATA, reinterpret_cast<LONG_PTR>(a_pData));
	::SendMessage(::GetDlgItem(a_hDlg, IDS_AFTERCARD), UDM_SETRANGE32, 0, 32000);
	::SendMessage(::GetDlgItem(a_hDlg, IDS_AFTERTRICK), UDM_SETRANGE32, 0, 32000);
	::SendMessage(::GetDlgItem(a_hDlg, IDS_AFTERPASS), UDM_SETRANGE32, 0, 32000);
	UDACCEL l_accel;
	l_accel.nSec = 1;
	l_accel.nInc = 100;
	::SendMessage(::GetDlgItem(a_hDlg, IDS_AFTERCARD), UDM_SETACCEL, 1, reinterpret_cast<LPARAM>(&l_accel));
	::SendMessage(::GetDlgItem(a_hDlg, IDS_AFTERTRICK), UDM_SETACCEL, 1, reinterpret_cast<LPARAM>(&l_accel));
	::SendMessage(::GetDlgItem(a_hDlg, IDS_AFTERPASS), UDM_SETACCEL, 1, reinterpret_cast<LPARAM>(&l_accel));

	SetCtrlValues(a_hDlg);
}


static INT_PTR OptionsTimeDlgProc(HWND a_hDlg, UINT a_iMsg, WPARAM a_wParam, LPARAM a_lParam)
{
	switch (a_iMsg)
	{
	case WM_INITDIALOG:
		OnInitDialog(a_hDlg, reinterpret_cast<CRegData::RTimeRegData*>(a_lParam));
		return TRUE;

	case WM_GETVALUES:
		GetCtrlValues(a_hDlg);
		::SetWindowLong(a_hDlg, DWL_MSGRESULT, TRUE);
		return TRUE; // must be TRUE too return proper LRESULT

	}
	return FALSE;
}


void SetCtrlValues(HWND a_hDlg)
{
	CRegData::RTimeRegData* l_pData = GetRData(a_hDlg);
	TCHAR l_sBuf[128];
	_ltot_s(l_pData->m_iWaitThrowTime, l_sBuf, ArraySize(l_sBuf), 10);
	::SetWindowText(::GetDlgItem(a_hDlg, IDC_AFTERCARD), l_sBuf);
	_ltot_s(l_pData->m_iWaitGetTime, l_sBuf, ArraySize(l_sBuf), 10);
	::SetWindowText(::GetDlgItem(a_hDlg, IDC_AFTERTRICK), l_sBuf);
	_ltot_s(l_pData->m_iWaitPassTime, l_sBuf, ArraySize(l_sBuf), 10);
	::SetWindowText(::GetDlgItem(a_hDlg, IDC_AFTERPASS), l_sBuf);
}


static CRegData::RTimeRegData* GetRData(HWND a_hDlg)
{
	#pragma warning(disable: 4312)
	return reinterpret_cast<CRegData::RTimeRegData*>(::GetWindowLong(a_hDlg, GWL_USERDATA));
	#pragma warning(default: 4312)
}


static void GetCtrlValues(HWND a_hDlg)
{
	CRegData::RTimeRegData* l_pData = GetRData(a_hDlg);
	TCHAR l_sBuf[128];
	::GetWindowText(::GetDlgItem(a_hDlg, IDC_AFTERCARD), l_sBuf, ArraySize(l_sBuf));
	l_pData->m_iWaitThrowTime = _tstol(l_sBuf);

	::GetWindowText(::GetDlgItem(a_hDlg, IDC_AFTERTRICK), l_sBuf, ArraySize(l_sBuf));
	l_pData->m_iWaitGetTime = _tstol(l_sBuf);

	::GetWindowText(::GetDlgItem(a_hDlg, IDC_AFTERPASS), l_sBuf, ArraySize(l_sBuf));
	l_pData->m_iWaitPassTime = _tstol(l_sBuf);
}