// AboutDlg.cpp : about dlg
//
#include "stdafx.h"
#include "AboutDlg.h"
#include "resource.h"
#include <stdio.h>
#include <tchar.h>
#include <rcommon/rstring.h>
#include <rcommon/RHyperlinkWnd.h>


static INT_PTR CALLBACK AboutDlgProc(HWND a_hDlg, UINT a_iMsg, WPARAM a_wParam, LPARAM a_lParam);
inline static void OnInitDialog(HWND a_hDlg);
inline static void GetProductVersion(LPTSTR a_sVersion, UINT a_iSize);

UINT AboutDlg_DoModal(HWND a_hWndParent)
{
	RHyperlinkWnd_RegisterClass();
	INT_PTR l_iRes = ::DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_INFO), a_hWndParent, AboutDlgProc, 0);
	return static_cast<UINT>(l_iRes);
}


static INT_PTR CALLBACK AboutDlgProc(HWND a_hDlg, UINT a_iMsg, WPARAM a_wParam, LPARAM a_lParam)
{
	switch (a_iMsg)
	{
	case WM_INITDIALOG:
		OnInitDialog(a_hDlg);
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(a_wParam) == IDOK || LOWORD(a_wParam) == IDCANCEL)
		{
			::EndDialog(a_hDlg, LOWORD(a_wParam));
			return TRUE;
		}
		else if (LOWORD(a_wParam) == IDB_SYSTEMINFO)
		{
		}
		break;
	}
	return FALSE;
}


static void OnInitDialog(HWND a_hDlg)
{
	TCHAR l_sVersion[128];
	GetProductVersion(l_sVersion, ArraySize(l_sVersion));
	TCHAR l_sText[128];
	::GetWindowText(::GetDlgItem(a_hDlg, IDC_INFO_VERSION), l_sText, ArraySize(l_sText));
	tstring l_sDisplay = tstring(l_sText) + tstring(_T(" ")) + tstring(l_sVersion);
	::SetWindowText(::GetDlgItem(a_hDlg, IDC_INFO_VERSION), l_sDisplay.c_str());

	::LoadString(::GetModuleHandle(NULL), IDS_INFO_AUTHORCONTACT, l_sText, ArraySize(l_sText));
	::SetWindowText(::GetDlgItem(a_hDlg, IDC_INFO_MAILTO), l_sText);
	::LoadString(::GetModuleHandle(NULL), IDS_INFO_AUTHORURL, l_sText, ArraySize(l_sText));
	RHyperlinkWnd_SetUrl(::GetDlgItem(a_hDlg, IDC_INFO_MAILTO), l_sText);
}


void GetProductVersion(LPTSTR a_sVersion, UINT a_iSize)
{
	ZeroMemory(a_sVersion, a_iSize);
	DWORD l_dw;
	TCHAR l_sFileName[MAX_PATH];

	if (::GetModuleFileName(NULL, l_sFileName, MAX_PATH) == 0)
	{
		return;
	}
	DWORD l_dwSize = ::GetFileVersionInfoSize(l_sFileName, &l_dw);
	if (l_dwSize == 0)
	{
		return;
	}
	LPBYTE l_pData = new BYTE[l_dwSize];
	if (::GetFileVersionInfo(l_sFileName, NULL, l_dwSize, l_pData) == 0)
	{
		delete[] l_pData;
		return;
	}
	UINT l_iSize;

	VS_FIXEDFILEINFO* l_pVSInfo;
	if (!::VerQueryValue(l_pData, TEXT("\\"), (LPVOID*)&l_pVSInfo, &l_iSize))
	{
		delete[] l_pData;
		return;
	}

	_sntprintf_s(a_sVersion, a_iSize,_TRUNCATE, _T("%d.%d.%d.%d"), 
		HIWORD(l_pVSInfo->dwProductVersionMS),
		LOWORD(l_pVSInfo->dwProductVersionMS),
		HIWORD(l_pVSInfo->dwProductVersionLS),
		LOWORD(l_pVSInfo->dwProductVersionLS));

	delete[] l_pData;
}


