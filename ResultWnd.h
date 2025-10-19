#pragma once

#include "TypeDefs.h"
#include "HeartsGame.h"
#include "RegData.h"


ATOM ResultWnd_Register(HINSTANCE a_hInst);
HWND ResultWnd_Create(DWORD a_dwStyleEx, DWORD a_dwStyle, int a_x, int a_y,
	int a_dx, int a_dy, HWND a_hWndParent, HINSTANCE a_hInst, T_SERIE a_enSerie, HeartsGame* a_pGameData, CRegData& a_regData);

#define ResultWnd_Refresh(a_hWnd) (::SendMessage(a_hWnd, WM_APP_REFRESH, 0, 0L));

#define ResultWnd_SetSerie(a_hWnd, a_enSerie) (::SendMessage(a_hWnd, WM_APP_SETSERIE, static_cast<WPARAM>(a_enSerie), 0L));
