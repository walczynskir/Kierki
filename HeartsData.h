#pragma once
#include <vector>
#include "GameData.h"
#include "OwnToolbar.h"
#include <rcommon/LanguageManager.h>
#include <rcommon/drawutl.h>
#include <commctrl.h>
#include <optional>







class CHeartsData
{
public:

	CHeartsData(HWND a_hWnd) : m_hWndTab(NULL), m_hWndStatusbar(NULL) { m_dpiContext.emplace(a_hWnd);  };

	static CHeartsData* GetData(HWND a_hWnd) { return reinterpret_cast<CHeartsData*>(::GetWindowLongPtr(a_hWnd, GWLP_USERDATA)); };
	static void SetData(HWND a_hWnd, CHeartsData* a_pData) { ::SetWindowLongPtr(a_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(a_pData)); };


	HWND GetTabWnd(UINT a_iTab) { ASSERT(m_arrTabs.size() >= a_iTab); return m_arrTabs[a_iTab]; }

	HWND GetGameWnd() { ASSERT(m_arrTabs.size() > 0); return m_arrTabs[0]; }
	void SetGameWnd(HWND a_hWnd) { ASSERT(m_arrTabs.size() == 0); m_arrTabs.push_back(a_hWnd); }

	HWND GetResultWnd() { ASSERT(m_arrTabs.size() >= 2); return m_arrTabs[1]; }
	void SetResultWnd(HWND a_hWnd) { ASSERT(m_arrTabs.size() == 1); m_arrTabs.push_back(a_hWnd); }
	short GetTabsCount() const { ASSERT(m_hWndTab != NULL); return TabCtrl_GetItemCount(m_hWndTab); };

	HWND m_hWndStatusbar;
	HWND m_hWndTab;

	COwnToolbar m_toolbar;
	std::vector<HWND> m_arrTabs;

	// ribbon data

	// game data
	GameData m_gameData;
	LanguageManager m_langManager;
	std::optional<RDraw::DpiContext> m_dpiContext;


};

