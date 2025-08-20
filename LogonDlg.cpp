// KeyDlg.cpp : Options dialog handlers
//

#include "stdafx.h"
#include "LogonDlg.h"
#include "resource.h"


class RLogonDlgData
{
public:
	RLogonDlgData(const tstring& a_sName) : m_sName(a_sName)
	{
	}
	tstring m_sName;
};


static INT_PTR CALLBACK LogonDlgProc(HWND a_hDlg, UINT a_iMsg, WPARAM a_wParam, LPARAM a_lParam);
inline static void OnInitDialog(HWND a_hDlg, RLogonDlgData* a_pData);
inline static bool OnCommand(HWND a_hDlg, UINT a_idCtrl);
static RLogonDlgData* GetRData(HWND a_hDlg);


bool LogonDlg_DoModal(HWND a_hWndParent, tstring* a_psName)
{
	RLogonDlgData l_data(*a_psName);
	INT_PTR l_iRes = ::DialogBoxParam(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_LOGON), a_hWndParent, LogonDlgProc, reinterpret_cast<LPARAM>(&l_data));
	if (l_iRes == IDOK)
	{
		*a_psName = l_data.m_sName;
	}
	return l_iRes == IDOK;
}


RLogonDlgData* GetData(HWND a_hDlg)
{
#pragma warning(disable: 4312)
	return reinterpret_cast<RLogonDlgData*>(::GetWindowLongPtr(a_hDlg, GWLP_USERDATA));
#pragma warning(default: 4312)
}


static INT_PTR CALLBACK LogonDlgProc(HWND a_hDlg, UINT a_iMsg, WPARAM a_wParam, LPARAM a_lParam)
{
	switch (a_iMsg)
	{
	case WM_INITDIALOG:
		OnInitDialog(a_hDlg, reinterpret_cast<RLogonDlgData*>(a_lParam));
		return TRUE;
	case WM_COMMAND:
		if (OnCommand(a_hDlg, LOWORD(a_wParam)))
		{
			return TRUE;
		}
		break;
	}
	return FALSE;
}


void OnInitDialog(HWND a_hDlg, RLogonDlgData* a_pData)
{
#pragma warning(disable: 4244)
	::SetWindowLongPtr(a_hDlg, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(a_pData));
#pragma warning(default: 4244)
	::SetWindowText(::GetDlgItem(a_hDlg, IDC_NAME), a_pData->m_sName.c_str());
	HWND l_hWndParent = ::GetParent(a_hDlg);
	ASSERT(l_hWndParent != NULL);
	RECT l_rectParent;
	::GetWindowRect(l_hWndParent, &l_rectParent);
	RECT l_rectDlg;
	::GetWindowRect(a_hDlg, &l_rectDlg);
	int l_iLeft = l_rectParent.left + (RectWidth(l_rectParent) - RectWidth(l_rectDlg)) / 2;
	int l_iTop = l_rectParent.top + (RectHeight(l_rectParent) - RectHeight(l_rectDlg)) / 2;

	::SetWindowPos(a_hDlg, NULL, l_iLeft, l_iTop, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
}


bool OnCommand(HWND a_hDlg, UINT a_idCtrl)
{
	switch (a_idCtrl)
	{
	case IDOK:
		{
			TCHAR l_sName[128];
			::GetWindowText(::GetDlgItem(a_hDlg, IDC_NAME), l_sName, ArraySize(l_sName));
			GetData(a_hDlg)->m_sName = l_sName;
			::EndDialog(a_hDlg, a_idCtrl);
			return true;
		}
		break;

	case IDCANCEL:
		::EndDialog(a_hDlg, a_idCtrl);
		return true;
	}
	return false;
}
