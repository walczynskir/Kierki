#pragma once

#include "TypeDefs.h"
#include "HeartsGame.h"


ATOM GameWnd_Register(HINSTANCE a_hInst);
HWND GameWnd_Create(DWORD a_dwStyleEx, DWORD a_dwStyle, int a_x, int a_y,
	int a_dx, int a_dy, HWND a_hWndParent, HINSTANCE a_hInst, HWND a_hWndApp, HeartsGame* a_pGameData, CRegData& a_regData);
void GameWnd_NewDeal(HWND a_hWnd, bool a_bOpen);
void GameWnd_NewDeal2(HWND a_hWnd);
void GameWnd_Refresh(HWND a_hWnd);