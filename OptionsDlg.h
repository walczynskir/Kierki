#pragma once

#include "resource.h"
#include "RRegData.h"
#include "OptionsViewDlg.h"
#include "OptionsTimeDlg.h"
#include "OptionsPlayerDlg.h"
#include "OptionsRulesDlg.h"

#define WM_GETVALUES (WM_USER + 1)

typedef HWND (*RCREATEDLGPROC)(HWND a_hParent, RRegData::RBaseRegData* a_pRegData, LPVOID a_pObj);

#define MAX_TABTITLE 128

class ROptionsData
{
public:
	ROptionsData(RRegData* a_pRegData)	
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
		::LoadString(::GetModuleHandle(NULL), IDS_TABTITLE_RULES, m_ahDlg[3].m_sTitle, ArraySize(m_ahDlg[0].m_sTitle));
	} 

	class RTabData
	{
	public:
		RTabData() : m_hWnd(NULL), m_pfnCreateDlg(NULL), m_pRegData(NULL), m_pObj(NULL), m_sTitle(_T("")) {}
		HWND m_hWnd;
		TCHAR m_sTitle[MAX_TABTITLE];
		RCREATEDLGPROC m_pfnCreateDlg;
		RRegData::RBaseRegData* m_pRegData;
		LPVOID  m_pObj;
	};
	RTabData	m_ahDlg[4];
};


UINT OptionsDlg_DoModal(HWND a_hWndParent, RRegData* a_pData);
