// OptionsPlayerDlg.cpp : Player option dialog handlers
//
#include "stdafx.h"
#include "resource.h"
#include "OptionsDlg.h"
#include "OptionsPlayerDlg.h"
#include <rcommon/RColorWnd.h>
#include <commctrl.h>

inline static void OnInitDialog(HWND a_hDlg, CRegData::RPlayersRegData* a_pData);
static INT_PTR CALLBACK OptionsPlayerDlgProc(HWND a_hDlg, UINT a_iMsg, WPARAM a_wParam, LPARAM a_lParam);
inline static void SetCtrlValues(HWND a_hDlg);
static CRegData::RPlayersRegData* GetRData(HWND a_hDlg);
inline static void GetCtrlValues(HWND a_hDlg);

#define MAX_NAME (128)


HWND CreateOptPlayerDlg(HWND a_hParent, CRegData::RBaseRegData* a_pData, LPVOID a_pObj)
{
	return ::CreateDialogParam(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_OPT_PLAYERS), 
		a_hParent, OptionsPlayerDlgProc, reinterpret_cast<LPARAM>(a_pData));
}


static void OnInitDialog(HWND a_hDlg, CRegData::RPlayersRegData* a_pData)
{
#pragma warning(disable: 4244)
	::SetWindowLongPtr(a_hDlg, GWL_USERDATA, reinterpret_cast<LONG_PTR>(a_pData));
#pragma warning(default: 4244)

	::SendMessage(::GetDlgItem(a_hDlg, IDC_PLAYER1), EM_LIMITTEXT, MAX_NAME - 1, 0);
	::SendMessage(::GetDlgItem(a_hDlg, IDC_PLAYER2), EM_LIMITTEXT, MAX_NAME - 1, 0);
	::SendMessage(::GetDlgItem(a_hDlg, IDC_PLAYER3), EM_LIMITTEXT, MAX_NAME - 1, 0);
	::SendMessage(::GetDlgItem(a_hDlg, IDC_PLAYER4), EM_LIMITTEXT, MAX_NAME - 1, 0);
	SetCtrlValues(a_hDlg);
}


static INT_PTR CALLBACK OptionsPlayerDlgProc(HWND a_hDlg, UINT a_iMsg, WPARAM a_wParam, LPARAM a_lParam)
{
	switch (a_iMsg)
	{
	case WM_INITDIALOG:
		OnInitDialog(a_hDlg, reinterpret_cast<CRegData::RPlayersRegData*>(a_lParam));
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
	CRegData::RPlayersRegData* l_pData = GetRData(a_hDlg);
	::SetWindowText(::GetDlgItem(a_hDlg, IDC_PLAYER1), l_pData->m_arrPlayersNames[0].c_str());
	::SetWindowText(::GetDlgItem(a_hDlg, IDC_PLAYER2), l_pData->m_arrPlayersNames[1].c_str());
	::SetWindowText(::GetDlgItem(a_hDlg, IDC_PLAYER3), l_pData->m_arrPlayersNames[2].c_str());
	::SetWindowText(::GetDlgItem(a_hDlg, IDC_PLAYER4), l_pData->m_arrPlayersNames[3].c_str());
}


static CRegData::RPlayersRegData* GetRData(HWND a_hDlg)
{
	#pragma warning(disable: 4312)
	return reinterpret_cast<CRegData::RPlayersRegData*>(::GetWindowLong(a_hDlg, GWL_USERDATA));
	#pragma warning(default: 4312)
}


static void GetCtrlValues(HWND a_hDlg)
{
	CRegData::RPlayersRegData* l_pData = GetRData(a_hDlg);
	TCHAR l_sBuf[MAX_NAME];

	::GetWindowText(::GetDlgItem(a_hDlg, IDC_PLAYER1), l_sBuf, ArraySize(l_sBuf));
	l_pData->m_arrPlayersNames[0] = l_sBuf;

	::GetWindowText(::GetDlgItem(a_hDlg, IDC_PLAYER2), l_sBuf, ArraySize(l_sBuf));
	l_pData->m_arrPlayersNames[1] = l_sBuf;

	::GetWindowText(::GetDlgItem(a_hDlg, IDC_PLAYER3), l_sBuf, ArraySize(l_sBuf));
	l_pData->m_arrPlayersNames[2] = l_sBuf;

	::GetWindowText(::GetDlgItem(a_hDlg, IDC_PLAYER4), l_sBuf, ArraySize(l_sBuf));
	l_pData->m_arrPlayersNames[3] = l_sBuf;

}