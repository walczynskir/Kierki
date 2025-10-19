// OptionsRulesDlg.cpp : Miscellanuous dialog handlers
//
#include "stdafx.h"
#include "resource.h"
#include "OptionsDlg.h"
#include "OptionsTimeDlg.h"
#include "Kierki.h"
#include <rcommon/RColorWnd.h>
#include <commctrl.h>
#include <rcommon/LanguageManager.h>
#include <rcommon/RSystemExc.h>
#include <rcommon/SafeWndProc.hpp>


typedef struct S_OPTRULESDATA
{
	CRegData::RRulesRegData* m_pData;
	LanguageManager* m_pLangManager;
} TOptRulesData, *LPOptRulesData;

inline static void OnInitDialog(HWND a_hDlg, LPOptRulesData a_pData);
static INT_PTR OptionsRulesDlgProc(HWND a_hDlg, UINT a_iMsg, WPARAM a_wParam, LPARAM a_lParam);
inline static void SetCtrlValues(HWND a_hDlg);
static LPOptRulesData GetRData(HWND a_hDlg);
inline static void GetCtrlValues(HWND a_hDlg);


HWND CreateOptRulesDlg(HWND a_hParent, CRegData::RBaseRegData* a_pData, LPVOID a_pObj)
{
	LPOptRulesData l_pData = new TOptRulesData; // to be deleted in WM_NCDESTROY
	l_pData->m_pData = static_cast<CRegData::RRulesRegData*>(a_pData);
	l_pData->m_pLangManager = static_cast<LanguageManager*>(a_pObj);

	HWND l_hDlg = ::CreateDialogParam(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_OPT_RULES), 
		a_hParent, SafeDialogProc<OptionsRulesDlgProc>, reinterpret_cast<LPARAM>(l_pData));

	if (l_hDlg == NULL)
	{
		delete l_pData;
		throw RSystemExc(_T("CreateDialogParam failed"));
	}
	return l_hDlg;
}


static inline void OnInitDialog(HWND a_hDlg, LPOptRulesData a_pData)
{
	::SetWindowLongPtr(a_hDlg, GWL_USERDATA, reinterpret_cast<LONG_PTR>(a_pData));

	// load languages
	HWND l_hWndLang = GetDlgItem(a_hDlg, IDC_RULES_LANGUAGE);

	for (const auto& l_lang : *(a_pData->m_pLangManager))
		::SendMessage(l_hWndLang, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(l_lang.m_sLangName.c_str()));

	SetCtrlValues(a_hDlg);
}

static inline void OnDestroy(HWND a_hDlg)
{
	LPOptRulesData l_pData = GetRData(a_hDlg);
	delete l_pData;
}



static INT_PTR OptionsRulesDlgProc(HWND a_hDlg, UINT a_iMsg, WPARAM a_wParam, LPARAM a_lParam)
{
	switch (a_iMsg)
	{
	case WM_INITDIALOG:
		OnInitDialog(a_hDlg, reinterpret_cast<LPOptRulesData>(a_lParam));
		return TRUE;

	case WM_APP_GETVALUES:
		GetCtrlValues(a_hDlg);
		::SetWindowLong(a_hDlg, DWL_MSGRESULT, TRUE);
		return TRUE; // must be TRUE too return proper LRESULT

	case WM_DESTROY:
		OnDestroy(a_hDlg);
	}
	return FALSE;
}

void SetCtrlValues(HWND a_hDlg)
{
	LPOptRulesData l_pData = GetRData(a_hDlg);
	::SendMessage(::GetDlgItem(a_hDlg, IDC_RULES_CONFIRMTRICK), BM_SETCHECK, 
		l_pData->m_pData->m_bConfirmTrick ? BST_CHECKED : BST_UNCHECKED, 0L);

	HWND l_hWndLang = ::GetDlgItem(a_hDlg, IDC_RULES_LANGUAGE);

	auto l_autoLangName = l_pData->m_pLangManager->GetCurrentLangName();
	if (l_autoLangName.has_value())
	{
		int l_idx = (int)::SendMessage(l_hWndLang, CB_FINDSTRINGEXACT, -1, reinterpret_cast<LPARAM>(l_autoLangName->c_str()));
		if (l_idx != CB_ERR) 
			::SendMessage(l_hWndLang, CB_SETCURSEL, l_idx, 0);
		
	}
	HWND l_hWndShowDlg = ::GetDlgItem(a_hDlg, IDC_RULES_SHOWDLG);

	// set Logondlg checkbox
	::SendMessage(::GetDlgItem(a_hDlg, IDC_RULES_SHOWDLG), BM_SETCHECK,
		l_pData->m_pData->m_bLogonDlg ? BST_CHECKED : BST_UNCHECKED, 0L);

	// set show help checkbox
	::SendMessage(::GetDlgItem(a_hDlg, IDC_RULES_HELPVISIBLE), BM_SETCHECK,
		l_pData->m_pData->m_bHelpVisible ? BST_CHECKED : BST_UNCHECKED, 0L);

}


static LPOptRulesData GetRData(HWND a_hDlg)
{
	return reinterpret_cast<LPOptRulesData>(::GetWindowLong(a_hDlg, GWL_USERDATA));
}


static void GetCtrlValues(HWND a_hDlg)
{
	LPOptRulesData l_pData = GetRData(a_hDlg);
	l_pData->m_pData->m_bConfirmTrick =
		(::SendMessage(::GetDlgItem(a_hDlg, IDC_RULES_CONFIRMTRICK), BM_GETCHECK, 0, 0L)) == BST_CHECKED;


	HWND l_hWndLang = ::GetDlgItem(a_hDlg, IDC_RULES_LANGUAGE);
	// get index of selected item
	int l_iSel = (int)::SendMessage(l_hWndLang, CB_GETCURSEL, 0, 0);

	if (l_iSel == CB_ERR)
		return;
	
	TCHAR l_sLang[64]; // buffer for string
	::SendMessage(l_hWndLang, CB_GETLBTEXT, l_iSel, (LPARAM)l_sLang);

	auto l_autoLangId = l_pData->m_pLangManager->GetLangId(l_sLang);
	if (l_autoLangId.has_value())
		l_pData->m_pData->m_idLanguage = l_autoLangId.value();

	// get logondlg checkbox
	l_pData->m_pData->m_bLogonDlg = (::SendMessage(::GetDlgItem(a_hDlg, IDC_RULES_SHOWDLG), BM_GETCHECK, 0, 0L) == BST_CHECKED);

	// get ShowHelp checkbox
	l_pData->m_pData->m_bHelpVisible = (::SendMessage(::GetDlgItem(a_hDlg, IDC_RULES_HELPVISIBLE), BM_GETCHECK, 0, 0L) == BST_CHECKED);

}