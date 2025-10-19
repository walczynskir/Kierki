// OptionsDlg.cpp : Options dialog handlers
//

#include "stdafx.h"
#include "OptionsDlg.h"
#include "resource.h"
#include "kierki.h"
#include <tchar.h>
#include <commctrl.h>
#include <rcommon/RHyperlinkWnd.h>
#include <RCards/resource.h>
#include <rcommon/RSystemExc.h>
#include <rcommon/SafeWndProc.hpp>


typedef struct S_OPTDATA
{
	CRegData* m_pData; 
	LanguageManager& m_langManager;
} TOptData, *LPOptData;

class ROptionsData
{
public:
	ROptionsData(CRegData* a_pRegData, LanguageManager& a_langManager)
	{
		m_ahDlg[0].m_pfnCreateDlg = static_cast<RCREATEDLGPROC>(CreateOptViewDlg);
		m_ahDlg[0].m_pRegData = &(a_pRegData->m_regView);
		::LoadString(::GetModuleHandle(NULL), IDS_TABTITLE_VIEW, m_ahDlg[0].m_sTitle, ArraySize(m_ahDlg[0].m_sTitle));

		m_ahDlg[1].m_pfnCreateDlg = static_cast<RCREATEDLGPROC>(CreateOptTimeDlg);
		m_ahDlg[1].m_pRegData = &(a_pRegData->m_regTime);
		::LoadString(::GetModuleHandle(NULL), IDS_TABTITLE_TIME, m_ahDlg[1].m_sTitle, ArraySize(m_ahDlg[0].m_sTitle));

		m_ahDlg[2].m_pfnCreateDlg = static_cast<RCREATEDLGPROC>(CreateOptPlayerDlg);
		m_ahDlg[2].m_pRegData = &(a_pRegData->m_regPlayers);
		::LoadString(::GetModuleHandle(NULL), IDS_TABTITLE_PLAYER, m_ahDlg[2].m_sTitle, ArraySize(m_ahDlg[0].m_sTitle));

		m_ahDlg[3].m_pfnCreateDlg = static_cast<RCREATEDLGPROC>(CreateOptRulesDlg);
		m_ahDlg[3].m_pRegData = &(a_pRegData->m_regRules);
		m_ahDlg[3].m_pObj = static_cast<LPVOID>(&(a_langManager));
		::LoadString(::GetModuleHandle(NULL), IDS_TABTITLE_RULES, m_ahDlg[3].m_sTitle, ArraySize(m_ahDlg[0].m_sTitle));
	}

	class RTabData
	{
	public:
		RTabData() : m_hWnd(NULL), m_pfnCreateDlg(NULL), m_pRegData(NULL), m_pObj(NULL), m_sTitle(_T("")) {}
		HWND m_hWnd;
		TCHAR m_sTitle[MAX_TABTITLE];
		RCREATEDLGPROC m_pfnCreateDlg;
		CRegData::RBaseRegData* m_pRegData;
		LPVOID  m_pObj;
	};
	RTabData	m_ahDlg[4]{};
};



static INT_PTR OptionsDlgProc(HWND a_hDlg, UINT a_iMsg, WPARAM a_wParam, LPARAM a_lParam);
inline static bool GetCtrlValues(HWND a_hDlg);
inline static void OnInitDialog(HWND a_hDlg, LPOptData a_pData);
inline static void OnNotify(HWND a_hDlg, int a_idCtrl, LPNMHDR a_pNmHdr);
inline static void OnNotifyTab(HWND a_hDlg, LPNMHDR a_pNmHdr);
inline static void OnTabSelChange(HWND a_hDlg);
inline static void OnDestroy(HWND a_hDlg);

inline static void GetProductVersion(LPTSTR a_sVersion, UINT a_iSize);

inline static void SetActiveTab(HWND a_hDlg, int a_iTab);
static ROptionsData* GetRData(HWND a_hDlg);


UINT OptionsDlg_DoModal(HWND a_hWndParent, CRegData* a_pData, LanguageManager& a_langManager)
{
	TOptData l_optData{ a_pData, a_langManager };
	RHyperlinkWnd_RegisterClass();
	INT_PTR l_iRes = ::DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_OPTIONS), a_hWndParent, SafeDialogProc<OptionsDlgProc>, reinterpret_cast<LPARAM>(&l_optData));
	return static_cast<UINT>(l_iRes);
}


ROptionsData* GetRData(HWND a_hDlg)
{
	#pragma warning(disable: 4312)
	return reinterpret_cast<ROptionsData*>(::GetWindowLongPtr(a_hDlg, GWLP_USERDATA));
	#pragma warning(default: 4312)

}


static INT_PTR OptionsDlgProc(HWND a_hDlg, UINT a_iMsg, WPARAM a_wParam, LPARAM a_lParam)
{
	switch (a_iMsg)
	{
	case WM_INITDIALOG:
		OnInitDialog(a_hDlg, reinterpret_cast<LPOptData>(a_lParam));
		return TRUE;

	case WM_COMMAND:
		if ((LOWORD(a_wParam) == IDOK) || (LOWORD(a_wParam) == IDSAVE))
		{
			if (GetCtrlValues(a_hDlg))
			{
				EndDialog(a_hDlg, LOWORD(a_wParam));
			}
			return TRUE;
		}
		else if (LOWORD(a_wParam) == IDCANCEL)
		{
			EndDialog(a_hDlg, LOWORD(a_wParam));
			return TRUE;
		}
		break;

	case WM_DESTROY:
		OnDestroy(a_hDlg);
		break;

	case WM_NOTIFY:
		OnNotify(a_hDlg, static_cast<int>(a_wParam), reinterpret_cast<LPNMHDR>(a_lParam));
		break;

	}
	return FALSE;
}


bool GetCtrlValues(HWND a_hDlg)
{
	ROptionsData* l_pData = GetRData(a_hDlg);
	for (int l_iAt = 0; l_iAt < ArraySize(l_pData->m_ahDlg); l_iAt++)
	{
		if (l_pData->m_ahDlg[l_iAt].m_hWnd != NULL)
		{
			if (::SendMessage(l_pData->m_ahDlg[l_iAt].m_hWnd, WM_APP_GETVALUES, 0, 0) != TRUE)
			{
				return false;
			}
		}
	}
	return true;
}


static void OnInitDialog(HWND a_hDlg, LPOptData a_pData)
{
	ROptionsData* l_pOptData = new ROptionsData(a_pData->m_pData, a_pData->m_langManager);	// delete in OnDestroy()

	::SetWindowLongPtr(a_hDlg, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(l_pOptData));	

	TCHAR l_sAuthorName[128];
	::LoadString(::GetModuleHandle(NULL), IDS_INFO_AUTHORCONTACT, l_sAuthorName, ArraySize(l_sAuthorName));
	::SetWindowText(::GetDlgItem(a_hDlg, IDC_INFO_MAILTO), l_sAuthorName);
	TCHAR l_sAuthorEMail[128];
	::LoadString(::GetModuleHandle(NULL), IDS_INFO_AUTHORURL, l_sAuthorEMail, ArraySize(l_sAuthorEMail));
	RHyperlinkWnd_SetUrl(::GetDlgItem(a_hDlg, IDC_INFO_MAILTO), l_sAuthorEMail);

	HWND l_hTabWnd = ::GetDlgItem(a_hDlg, IDC_TAB);
	// sets tabs
	for (int l_iTab = 0; l_iTab < ArraySize(l_pOptData->m_ahDlg); l_iTab++)
	{
		TCITEM l_tabItem;
		l_tabItem.mask = TCIF_TEXT;

		l_tabItem.pszText = l_pOptData->m_ahDlg[l_iTab].m_sTitle;
		l_tabItem.cchTextMax = static_cast<int>(_tcslen(l_pOptData->m_ahDlg[l_iTab].m_sTitle));
		::SendMessage(l_hTabWnd, TCM_INSERTITEM, l_iTab, reinterpret_cast<LPARAM>(&l_tabItem));
	}
	
	TabCtrl_SetCurSel(l_hTabWnd, 0);
	SetActiveTab(a_hDlg, 0);

	TCHAR l_sVersion[128];
	GetProductVersion(l_sVersion, ArraySize(l_sVersion));
	::SetWindowText(::GetDlgItem(a_hDlg, IDC_VERSION), l_sVersion);
}


void OnDestroy(HWND a_hDlg)
{
	ROptionsData* l_pData = GetRData(a_hDlg);
	delete l_pData;
}


static void OnNotify(HWND a_hDlg, int a_idCtrl, LPNMHDR a_pNmHdr)
{
	switch (a_idCtrl)
	{
	case IDC_TAB:
		OnNotifyTab(a_hDlg, a_pNmHdr);
		break;
	}
}


static void OnNotifyTab(HWND a_hDlg, LPNMHDR a_pNmHdr)
{
	switch (a_pNmHdr->code)
	{
		case TCN_SELCHANGE:
			OnTabSelChange(a_hDlg);
			break;
	}
}


static void OnTabSelChange(HWND a_hDlg)
{
	SetActiveTab(a_hDlg, TabCtrl_GetCurSel(::GetDlgItem(a_hDlg, IDC_TAB)));
}


void SetActiveTab(HWND a_hDlg, int a_iTab)
{
	ROptionsData* l_pData = GetRData(a_hDlg);

	// hide rest of dialogs
	for (int l_iAt = 0; l_iAt < ArraySize(l_pData->m_ahDlg); l_iAt++)
	{
		if ((l_iAt != a_iTab) && (l_pData->m_ahDlg[l_iAt].m_hWnd != NULL))
		{
			::ShowWindow(l_pData->m_ahDlg[l_iAt].m_hWnd, SW_HIDE);
		}
	}

	if (l_pData->m_ahDlg[a_iTab].m_hWnd == NULL)
	{
		l_pData->m_ahDlg[a_iTab].m_hWnd = l_pData->m_ahDlg[a_iTab].m_pfnCreateDlg(a_hDlg, l_pData->m_ahDlg[a_iTab].m_pRegData, l_pData->m_ahDlg[a_iTab].m_pObj);
	}


	RECT l_rectTab;
	HWND l_hTabWnd = ::GetDlgItem(a_hDlg, IDC_TAB);
	::GetWindowRect(l_hTabWnd, &l_rectTab);
	POINT l_pt = { l_rectTab.left, l_rectTab.top };
	::ScreenToClient(a_hDlg, &l_pt);

	RECT l_rectDlg;
	::SetRect(&l_rectDlg, 0, 0, RectWidth(l_rectTab), RectHeight(l_rectTab)); 
	TabCtrl_AdjustRect(l_hTabWnd, FALSE, &l_rectDlg); 

	::SetWindowPos(l_pData->m_ahDlg[a_iTab].m_hWnd, HWND_TOP, 
		l_pt.x + l_rectDlg.left,  
		l_pt.y + l_rectDlg.top, 
		RectWidth(l_rectDlg), 
		RectHeight(l_rectDlg), SWP_SHOWWINDOW);
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

	_sntprintf_s(a_sVersion, a_iSize, _TRUNCATE, _T("%d.%d.%d.%d"), 
		HIWORD(l_pVSInfo->dwProductVersionMS),
		LOWORD(l_pVSInfo->dwProductVersionMS),
		HIWORD(l_pVSInfo->dwProductVersionLS),
		LOWORD(l_pVSInfo->dwProductVersionLS));

	delete[] l_pData;
}

