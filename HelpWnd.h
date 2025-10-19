#pragma once
#include <rcommon/rstring.h>
#include <rcommon/LanguageManager.h>
#include "RegData.h"


HWND HelpWnd_Create(DWORD a_dwStyle, HWND a_hWndParent, const LanguageManager& a_lang, CRegData& a_regData);
BOOL HelpWnd_Register(HINSTANCE a_hInst);
void HelpWnd_LoadInstructions(HWND a_hWndHelp, const std::string& a_sSection);

#define HelpWnd_Refresh(a_hWnd) (::SendMessage(a_hWnd, WM_APP_REFRESH, 0, 0L));

