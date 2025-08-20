#pragma once

#include "TypeDefs.h"
#include "GameData.h"

ATOM GameWnd_Register(HINSTANCE a_hInst);
HWND GameWnd_Create(DWORD a_dwStyleEx, DWORD a_dwStyle, int a_x, int a_y,
	int a_dx, int a_dy, HWND a_hWndParent, HINSTANCE a_hInst, HWND a_hWndApp, GameData* a_pGameData);
void GameWnd_NewDeal(HWND a_hWnd, bool a_bOpen);
void GameWnd_Refresh(HWND a_hWnd);