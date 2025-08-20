// OptionsTimeDlg.cpp : Time option dialog handlers
//
#include "stdafx.h"
#include "resource.h"
#include "OptionsDlg.h"
#include "OptionsTimeDlg.h"
#include <rcommon/RColorWnd.h>
#include <commctrl.h>

inline static void OnInitDialog(HWND a_hDlg, RRegData::RRulesRegData* a_pData);
static INT_PTR CALLBACK OptionsRulesDlgProc(HWND a_hDlg, UINT a_iMsg, WPARAM a_wParam, LPARAM a_lParam);
inline static void SetCtrlValues(HWND a_hDlg);
static RRegData::RRulesRegData* GetRData(HWND a_hDlg);
inline static void GetCtrlValues(HWND a_hDlg);


HWND CreateOptRulesDlg(HWND a_hParent, RRegData::RBaseRegData* a_pData, LPVOID a_pObj)
{
	return ::CreateDialogParam(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_OPT_RULES), 
		a_hParent, OptionsRulesDlgProc, reinterpret_cast<LPARAM>(a_pData));
}


static void OnInitDialog(HWND a_hDlg, RRegData::RRulesRegData* a_pData)
{
#pragma warning(disable: 4244)
	::SetWindowLongPtr(a_hDlg, GWL_USERDATA, reinterpret_cast<LONG_PTR>(a_pData));
#pragma warning(default: 4244)
	SetCtrlValues(a_hDlg);
}


static INT_PTR CALLBACK OptionsRulesDlgProc(HWND a_hDlg, UINT a_iMsg, WPARAM a_wParam, LPARAM a_lParam)
{
	switch (a_iMsg)
	{
	case WM_INITDIALOG:
		OnInitDialog(a_hDlg, reinterpret_cast<RRegData::RRulesRegData*>(a_lParam));
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
	RRegData::RRulesRegData* l_pData = GetRData(a_hDlg);
	::SendMessage(::GetDlgItem(a_hDlg, IDC_RULES_CONFIRMTRICK), BM_SETCHECK, 
		l_pData->m_bConfirmTrick ? BST_CHECKED : BST_UNCHECKED, 0L);
}


static RRegData::RRulesRegData* GetRData(HWND a_hDlg)
{
	#pragma warning(disable: 4312)
	return reinterpret_cast<RRegData::RRulesRegData*>(::GetWindowLong(a_hDlg, GWL_USERDATA));
	#pragma warning(default: 4312)
}


static void GetCtrlValues(HWND a_hDlg)
{
	RRegData::RRulesRegData* l_pData = GetRData(a_hDlg);
	l_pData->m_bConfirmTrick = 
		(::SendMessage(::GetDlgItem(a_hDlg, IDC_RULES_CONFIRMTRICK), BM_GETCHECK, 0, 0L)) == BST_CHECKED;
}