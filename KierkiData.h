#pragma once

#include <vector>
#include <commctrl.h>
#include "GameData.h"
#include "RRegData.h"



class KierkiData
{
public:

	KierkiData(void)
	{
		m_hWndTab = NULL;
	}

	~KierkiData(void)
	{
	}

	HWND GetGameWnd() { ASSERT(m_arrTabs.size() > 0); return m_arrTabs[0]; }
	void SetGameWnd(HWND a_hWnd) { ASSERT(m_arrTabs.size() == 0); m_arrTabs.push_back(a_hWnd); }
	HWND GetTabWnd(UINT a_iTab) { ASSERT(m_arrTabs.size() >= a_iTab); return m_arrTabs[a_iTab]; }
//	short GetTabsCount() const { ASSERT(m_hWndTab != NULL); TabCtrl_GetItemCount(m_hWndTab); };

	HWND m_hWndStatusbar;
	HWND m_hWndTab;
	std::vector<HWND> m_arrTabs;

	// game data
	GameData m_gameData;
	RRegData m_regData;
};


