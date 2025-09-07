// KeyDlg.cpp : Options dialog handlers
//

#include "stdafx.h"
#include "LogonDlg.h"
#include "resource.h"


class RLogonDlgData
{
public:
	RLogonDlgData(const tstring& a_sName, LanguageManager* a_pLangMan, bool* a_pLogonDlg) : m_sName(a_sName), m_pLangManager(a_pLangMan), m_pLogonDlg(a_pLogonDlg)
	{
	}
	tstring m_sName;
	LanguageManager* m_pLangManager;
	bool* m_pLogonDlg;
};


static INT_PTR CALLBACK LogonDlgProc(HWND a_hDlg, UINT a_iMsg, WPARAM a_wParam, LPARAM a_lParam);
inline static void OnInitDialog(HWND a_hDlg, RLogonDlgData* a_pData);
inline static bool OnCommand(HWND a_hDlg, UINT a_idCtrl);
static RLogonDlgData* GetRData(HWND a_hDlg);
inline static bool OnOk(HWND a_hDlg);



bool LogonDlg_DoModal(HWND a_hWndParent, tstring* a_psName, LanguageManager* a_pLangMan, bool* a_pLogonDlg)
{
	RLogonDlgData l_data(*a_psName, a_pLangMan, a_pLogonDlg);
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
	// remeber data pointer
	::SetWindowLongPtr(a_hDlg, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(a_pData));

	// set name
	::SetWindowText(::GetDlgItem(a_hDlg, IDC_NAME), a_pData->m_sName.c_str());

	// load languages
	HWND l_hWndLang = GetDlgItem(a_hDlg, IDC_LOGON_LANGUAGE);
	for (const auto& l_lang : *(a_pData->m_pLangManager))
		::SendMessage(l_hWndLang, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(l_lang.m_sLangName.c_str()));

	// set current language
	auto l_autoLangName = a_pData->m_pLangManager->GetCurrentLangName();
	if (l_autoLangName.has_value())
	{
		int l_idx = (int)::SendMessage(l_hWndLang, CB_FINDSTRINGEXACT, -1, reinterpret_cast<LPARAM>(l_autoLangName->c_str()));
		if (l_idx != CB_ERR)
			::SendMessage(l_hWndLang, CB_SETCURSEL, l_idx, 0);
	}

	// set logondlg checkbox
	::SendMessage(::GetDlgItem(a_hDlg, IDC_LOGON_NOTSHOW), BM_SETCHECK,
		*(a_pData->m_pLogonDlg) ? BST_UNCHECKED : BST_CHECKED, 0L);

	HWND l_hWndParent = ::GetParent(a_hDlg);
	ASSERT(l_hWndParent != NULL);
	RECT l_rectParent;
	::GetWindowRect(l_hWndParent, &l_rectParent);
	RECT l_rectDlg;
	::GetWindowRect(a_hDlg, &l_rectDlg);
	int l_iLeft = l_rectParent.left;
	int l_iTop = l_rectParent.top;

	::SetWindowPos(a_hDlg, NULL, l_iLeft, l_iTop, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
}


bool OnCommand(HWND a_hDlg, UINT a_idCtrl)
{
	switch (a_idCtrl)
	{
	case IDOK:
		return OnOk(a_hDlg);

	case IDCANCEL:
		::EndDialog(a_hDlg, IDCANCEL);
		return true;
	}
	return false;
}


bool OnOk(HWND a_hDlg)
{
	TCHAR l_sName[128];
	::GetWindowText(::GetDlgItem(a_hDlg, IDC_NAME), l_sName, ArraySize(l_sName));
	GetData(a_hDlg)->m_sName = l_sName;


	HWND l_hWndLang = GetDlgItem(a_hDlg, IDC_LOGON_LANGUAGE);
	// get index of selected item
	int l_iSel = (int)::SendMessage(l_hWndLang, CB_GETCURSEL, 0, 0);

	if (l_iSel != CB_ERR) 
	{
		TCHAR l_sLang[64]; // buffer for string
		::SendMessage(l_hWndLang, CB_GETLBTEXT, l_iSel, (LPARAM)l_sLang);
		GetData(a_hDlg)->m_pLangManager->SetLanguage(l_sLang);
	}

	// get logondlg checkbox
	*(GetData(a_hDlg)->m_pLogonDlg) = (::SendMessage(::GetDlgItem(a_hDlg, IDC_LOGON_NOTSHOW), BM_GETCHECK, 0, 0L) == BST_UNCHECKED);

	::EndDialog(a_hDlg, IDOK);
	return true;
}
