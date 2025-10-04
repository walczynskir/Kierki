#pragma once
#include <map>
#include "GameData.h"
#include "OwnToolbar.h"
#include "StatusBar.h"
#include "FontFactory.h"
#include <rcommon/LanguageManager.h>
#include <rcommon/drawutl.h>
#include <commctrl.h>
#include <optional>




enum T_TABPAGES
{
	TAB_GAME = 0,
	TAB_RESULTS,
	TAB_HELP
};


class CHeartsData
{
public:

	CHeartsData(HWND a_hWnd)  { m_dpiContext.emplace(a_hWnd);  };

	static CHeartsData* GetData(HWND a_hWnd) { return reinterpret_cast<CHeartsData*>(::GetWindowLongPtr(a_hWnd, GWLP_USERDATA)); };
	static void SetData(HWND a_hWnd, CHeartsData* a_pData) { ::SetWindowLongPtr(a_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(a_pData)); };

	HWND GetTabWnd(enum T_TABPAGES a_enPage) { return m_mapTabs[a_enPage]; }

	HWND GetGameWnd()  { return m_mapTabs[TAB_GAME]; }
	void SetGameWnd(HWND a_hWnd) { m_mapTabs[TAB_GAME] = a_hWnd; }

	HWND GetResultsWnd()  { return m_mapTabs[TAB_RESULTS]; }
	void SetResultsWnd(HWND a_hWnd) { m_mapTabs[TAB_RESULTS] = a_hWnd; }

	HWND GetHelpWnd()  { return m_mapTabs[TAB_HELP]; }
	void SetHelpWnd(HWND a_hWnd) { m_mapTabs[TAB_HELP] = a_hWnd; }

	short GetTabsCount() const { ASSERT(m_hWndTab != NULL); return TabCtrl_GetItemCount(m_hWndTab); };

	std::map<enum T_TABPAGES, HWND> m_mapTabs;

	// game data
	GameData m_gameData;
	LanguageManager m_langManager;
	std::optional<RDraw::DpiContext> m_dpiContext;

	bool m_bHelpVisible = false;

	// internal windows (toolbar, statusbar, tabcontrol
	HWND m_hWndTab{};
	COwnToolbar m_toolbar;
	CStatusBar m_statusbar;

};

