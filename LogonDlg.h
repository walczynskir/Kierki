#pragma once
#include <rcommon/rstring.h>
#include <rcommon/LanguageManager.h>


bool LogonDlg_DoModal(HWND a_hWndParent, tstring* a_psName, LanguageManager* a_pLangMan, bool* a_pLogonDlg);