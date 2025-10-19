#pragma once
#include <map>
#include "HeartsGame.h"
#include "OwnToolbar.h"
#include "StatusBar.h"
#include "FontFactory.h"
#include <rcommon/LanguageManager.h>
#include <rcommon/ROwnExc.h>
#include <rcommon/drawutl.h>
#include <commctrl.h>
#include <optional>




enum class TabPages
{
	GAME = 0,
	RESULTS,
	HELP
};


class CHeartsData
{
public:

	CHeartsData(HWND a_hWnd)  { m_dpiContext.emplace(a_hWnd);  };

	static CHeartsData* GetData(HWND a_hWnd) { return reinterpret_cast<CHeartsData*>(::GetWindowLongPtr(a_hWnd, GWLP_USERDATA)); };
	static void SetData(HWND a_hWnd, CHeartsData* a_pData) { ::SetWindowLongPtr(a_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(a_pData)); };

	HeartsGame& GetGameData() 
	{
		if (!m_gameData.has_value())
			throw ROwnExc(_T("HeartsGame not initialized"));
		return *m_gameData;
	};


	HWND GetTabWnd(TabPages a_enPage) { return m_mapTabs[a_enPage]; }

	HWND GetGameWnd()  { return m_mapTabs[TabPages::GAME]; }
	void SetGameWnd(HWND a_hWnd) { m_mapTabs[TabPages::GAME] = a_hWnd; }

	HWND GetResultsWnd()  { return m_mapTabs[TabPages::RESULTS]; }
	void SetResultsWnd(HWND a_hWnd) { m_mapTabs[TabPages::RESULTS] = a_hWnd; }

	HWND GetHelpWnd()  { return m_mapTabs[TabPages::HELP]; }
	void SetHelpWnd(HWND a_hWnd) { m_mapTabs[TabPages::HELP] = a_hWnd; }

	short GetTabsCount() const { ASSERT(m_hWndTab != NULL); return TabCtrl_GetItemCount(m_hWndTab); };

	std::map<TabPages, HWND> m_mapTabs;
	void EmplaceGameData(const tstring& a_sName1, const tstring& a_sName2, const tstring& a_sName3, const tstring& a_sName4)
	{
		m_gameData.emplace(a_sName1, a_sName2, a_sName3, a_sName4);
	}

	LanguageManager m_langManager;
	std::optional<RDraw::DpiContext> m_dpiContext;

	bool m_bHelpVisible = false;

	// internal windows (toolbar, statusbar, tabcontrol
	HWND m_hWndTab{};
	COwnToolbar m_toolbar;
	CStatusBar m_statusbar;
	CRegData m_regData;

private:
	// game data - I can't initiate it in constructor, as I need to pass player names
	std::optional<HeartsGame> m_gameData;
};

