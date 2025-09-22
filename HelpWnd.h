#pragma once
#include <rcommon/rstring.h>
#include <rcommon/LanguageManager.h>
#include "RRegData.h"


HWND HelpWnd_Create(DWORD a_dwExStyle, DWORD a_dwStyle, HWND a_hWndParent, const LanguageManager& a_lang, RRegData* a_pRegData);
BOOL HelpWnd_Register(HINSTANCE a_hInst);
void HelpWnd_SetFont(HWND a_hWnd, const tstring& a_sFont);
void HelpWnd_LoadInstructions(HWND a_hWndHelp, const std::string& a_sSection);
