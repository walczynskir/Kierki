#pragma once

#include "resource.h"


#define WM_APP_NEXTSERIE	 (WM_APP)
#define WM_APP_GAMEOVER		 (WM_APP + 1)
#define WM_APP_TRUMPSCHOSEN	 (WM_APP + 2)
#define WM_APP_SETTITLE		 (WM_APP + 3)
#define WM_APP_PLAYCARD		 (WM_APP + 4)
#define WM_APP_READY		 (WM_APP + 5)
#define WM_APP_CHOOSETRUMPS  (WM_APP + 6)
#define WM_APP_CONFIRMTRICK  (WM_APP + 7)
#define WM_APP_SETBRIGHTNESS (WM_APP + 8)
#define WM_APP_GETBRIGHTNESS (WM_APP + 9)
#define WM_APP_REFRESH		 (WM_APP + 10)
#define WM_APP_SETSERIE		 (WM_APP + 11)
#define WM_APP_GETVALUES	 (WM_APP + 12)
#define WM_APP_HUMANMOVE	 (WM_APP + 13)




void StartGame(HWND a_hWnd, bool a_bOpen);
bool DecisionBox(HWND a_hWnd, UINT a_idPrompt);
void SaveGame(HWND a_hWnd);
bool GetOpenSaveFile(HWND a_hWnd, bool a_bOpen, LPTSTR a_psFile, DWORD a_iSizeFile);

#define RefreshTab(a_hWnd) ::SendMessage((a_hWnd), WM_APP_REFRESH, 0, 0)






