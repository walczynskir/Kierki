#pragma once

#include "resource.h"
#include "RRegData.h"
#include "OptionsViewDlg.h"
#include "OptionsTimeDlg.h"
#include "OptionsPlayerDlg.h"
#include "OptionsRulesDlg.h"
#include "HeartsData.h"

#define WM_GETVALUES (WM_USER + 1)

typedef HWND (*RCREATEDLGPROC)(HWND a_hParent, RRegData::RBaseRegData* a_pRegData, LPVOID a_pObj);

#define MAX_TABTITLE 128


UINT OptionsDlg_DoModal(HWND a_hWndParent, RRegData* a_pData, CHeartsData* a_pHeartsData);