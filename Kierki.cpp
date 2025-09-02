// Kierki.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Kierki.h"
#include <rcommon/ExceptionHandler.h>
#include <rcommon/RException.h>
#include <rcommon/ROwnExc.h>
#include <rcommon/RSystemExc.h>
#include <commctrl.h>
#include <commdlg.h>
#include <rcommon/rstring.h>
#include "GameWnd.h"
#include "ResultWnd.h"
#include "LogonDlg.h"
#include "OptionsDlg.h"
#include "GameData.h"
#include "layout.h"
#include <RCards/RCards.h>
#include "HeartsData.h"
#include <rcommon/DrawUtl.h>
#include <rcommon/RMemDC.h>



#pragma todo("change pass indicator image, as it is ugly")
#pragma todo("check behaviour of cover, when app used for the first time")
#pragma todo("Add Own Toolbar behaviour on LButtonDown")
#pragma todo("Add statusbar messages on Hover on OwnToolbar items")
#pragma todo("Rethink OwnToolbar architecture, add to library of components")
#pragma todo("MessageBox (DecisionBox) centered over main wnd")
#pragma todo("disabling items in own toolbar, as previously with menu items: check and remove function OnEnterMenuLoop")
#pragma todo("About system button doesn't work")
#pragma todo("remove unused resources")
#pragma todo("check all resources for unicode")
#pragma todo("remove unused code")	
#pragma todo("bad rendering of covers (background)")
#pragma todo("refactor of cover selecting")
#pragma todo("possibility of changing pass button")
#pragma todo("improve color picker / add to toolbar")
#pragma todo("improve selecting covers, at least a few from own reources, not from exes or dlls")
#pragma todo("improve loading cards - check how they bmps are loaded")
#pragma todo("add option to load other cards decks")
#pragma todo("do refactor of all graphics")




#define IDC_MAINTAB 1000

static const TCHAR c_sWindowClass[] = _T("HEARTS");	// the main window class name

// Forward declarations of functions included in this code module:
inline static ATOM RegisterKierki(HINSTANCE a_hInst);
inline static int RunApp(HINSTANCE a_hInst, HINSTANCE a_hPrevInstance, LPTSTR a_lpCmdLine, int a_nCmdShow);
inline static int RunAppThrow(HINSTANCE a_hInst, HINSTANCE a_hPrevInstance, LPTSTR a_lpCmdLine, int a_nCmdShow);
inline static HWND InitInstance(HINSTANCE a_hInst, int a_nCmdShow);
static LRESULT CALLBACK	Kierki_WndProc(HWND a_hWnd, UINT a_iMsg, WPARAM a_wParam, LPARAM a_lParam);

static inline void SetupOwnToolbar(HWND a_hWnd);
inline static void ConfigureStatusbar(HWND a_hWndStatusbar);
static void AddTab(HWND a_hWnd, LPTSTR a_sTabName);

inline static LRESULT OnCreate(HWND a_hWnd);
inline static void OnPaint(HWND a_hWnd);
inline static void OnDestroy(HWND a_hWnd);
inline static void OnClose(HWND a_hWnd);
inline static void OnNcDestroy(HWND a_hWnd);
inline static void OnSize(HWND a_hWnd, WPARAM a_wParam, int a_dxWidth, int a_dyHeight);

inline static void OnCommand(HWND a_hWnd, WPARAM a_wParam, LPARAM a_lParam);

inline static void OnMouseMove(HWND a_hWnd, LONG a_x, LONG a_y);
inline static void OnLButtonDown(HWND a_hWnd, int a_x, int a_y);
inline static void OnLButtonUp(HWND a_hWnd, int a_x, int a_y);

inline static void OnGetMinMaxInfo(HWND a_hWnd, LPMINMAXINFO a_lpMinMaxInfo);

inline static void OnNotify(HWND a_hWnd, int a_idCtrl, LPNMHDR a_pNmHdr);
inline static void OnNotifyTab(HWND a_hWnd, LPNMHDR a_pNmHdr);
inline static void OnTabSelChange(HWND a_hWnd);

inline static void OnGameNew(HWND a_hWnd);
inline static void OnGameOptions(HWND a_hWnd);
inline static void OnGameSave(HWND a_hWnd);
inline static void OnGameSaveAs(HWND a_hWnd);
inline static void OnGameOpen(HWND a_hWnd);
inline static void OnEnterMenuLoop(HWND a_hWnd);

inline static void OnAppNextSerie(HWND a_hWnd);
inline static void OnAppGameOver(HWND a_hWnd);
inline static void OnAppTrumpsChosen(HWND a_hWnd);
inline static void OnAppSetTitle(HWND a_hWnd);
inline static void OnAppPlayCard(HWND a_hWnd);
inline static void OnAppReady(HWND a_hWnd);
inline static void OnAppChooseTrumps(HWND a_hWnd);
inline static void OnAppConfirmTrick(HWND a_hWnd);

static void Draw(HWND a_hWnd, HDC a_hDC);

inline static void ConfigureMenu(HWND a_hWNd);

inline static void SetTitle(HWND a_hWnd);
inline static void GetTabRect(HWND a_hWnd, LPRECT a_pRectTab);
inline static HWND GetCurTab(HWND a_hWnd);
static void SetActiveTab(HWND a_hDlg, int a_iTab);
static void ResizeTab(HWND a_hWnd, int a_dxWidth, int a_dyHeight);

inline static void AddResultTab(HWND a_hWnd, T_SERIE a_enSerie);

inline static void SetResultsTab(HWND a_hWnd);
inline static void SetPlayTab(HWND a_hWnd);
static void SetStatusBarText(HWND a_hWnd, UINT a_idStr);

inline static void GetMinSize(HWND a_hWnd, LPSIZE a_pSize);

static void EnableSaveMenu(HWND a_hWnd, BOOL a_bEnable);
static const TCHAR cc_cAsterisk = L'*';


#ifdef _DEBUG
#define NOLOGON
#endif

#ifdef _DEBUG
#define NOSAVE
#endif







int APIENTRY _tWinMain(_In_ HINSTANCE a_hInst,
                     _In_opt_ HINSTANCE a_hPrevInstance,
                     _In_ LPTSTR    a_lpCmdLine,
                     _In_ int       a_nCmdShow)
{
	int l_iRes;

	
	// run-time debugging 
	__try
	{
		l_iRes = RunApp(a_hInst, a_hPrevInstance, a_lpCmdLine, a_nCmdShow);
	}
	__except(RecordExceptionInfo(GetExceptionInformation(), _T("WinMain")))
    {
    }
	return l_iRes;

}

int RunApp(HINSTANCE a_hInst,
                     HINSTANCE a_hPrevInstance,
                     LPTSTR    a_lpCmdLine,
                     int       a_nCmdShow)
{
	int l_iRes;
	try
	{
		l_iRes = RunAppThrow(a_hInst, a_hPrevInstance, a_lpCmdLine, a_nCmdShow);
	}
	catch (RException& l_exc)
	{
		RecordExceptionInfo(NULL, l_exc.GetFormattedMsg().c_str());
		return 6;
	}
	return l_iRes;
}


int RunAppThrow(HINSTANCE a_hInst,
                     HINSTANCE a_hPrevInstance,
                     LPTSTR    a_lpCmdLine,
                     int       a_nCmdShow)
{


	RegisterKierki(a_hInst);
	INITCOMMONCONTROLSEX l_icc;

	l_icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
	l_icc.dwICC  = ICC_BAR_CLASSES | ICC_COOL_CLASSES | ICC_TAB_CLASSES;

	InitCommonControlsEx(&l_icc);

// if using for static library we need to set instance of the app for resources to be load (e.g. using LoadBitmap function)
#ifdef RCARDS_DLL
	::RCards_InitCards(NULL);
#else
	::RCards_InitCards(a_hInst);
#endif

	// Perform application initialization:
	HWND l_hWndMain;
	try
	{
		l_hWndMain = InitInstance(a_hInst, a_nCmdShow);
	}
	catch (RException& l_exc)
	{
		TCHAR l_sTitle[1024];
		::LoadString(a_hInst, IDS_MSGBOXTITLE, l_sTitle, ArraySize(l_sTitle));
		::MessageBox(NULL, l_exc.GetFormattedMsg().c_str(), l_sTitle, MB_OK | MB_ICONEXCLAMATION);
		return 1;
	}

	if (l_hWndMain == NULL)
	{
		return 1;
	}
	HACCEL l_hAccel = ::LoadAccelerators(a_hInst, MAKEINTRESOURCE(IDC_KIERKI));

	// Main message loop:
	BOOL l_bRet; // can be -1!
	MSG l_msg;
	while (l_bRet = ::GetMessage(&l_msg, NULL, 0, 0) != 0)
	{
		if (l_bRet == -1)
		{
			ASSERT(FALSE);
		}
		else
		{
			if (!::TranslateAccelerator(l_hWndMain, l_hAccel, &l_msg))
			{
				::TranslateMessage(&l_msg);
				::DispatchMessage(&l_msg);
			}
		}
	}

	::RCards_DoneCards();
	return static_cast<int>(l_msg.wParam);
}


ATOM RegisterKierki(HINSTANCE a_hInst)
{
	WNDCLASSEX l_wcex;

	l_wcex.cbSize			= sizeof(WNDCLASSEX); 
	l_wcex.style			= CS_HREDRAW | CS_VREDRAW;
	l_wcex.lpfnWndProc		= Kierki_WndProc;
	l_wcex.cbClsExtra		= 0;
	l_wcex.cbWndExtra		= sizeof(CHeartsData*);
	l_wcex.hInstance		= a_hInst;
	l_wcex.hIcon			= ::LoadIcon(a_hInst, MAKEINTRESOURCE(IDI_KIERKI));
	l_wcex.hCursor			= ::LoadCursor(NULL, IDC_ARROW);
	l_wcex.hbrBackground	= NULL;
	l_wcex.lpszMenuName		= NULL;
	l_wcex.lpszClassName	= c_sWindowClass;
	l_wcex.hIconSm			= ::LoadIcon(a_hInst, MAKEINTRESOURCE(IDI_KIERKI));
	return ::RegisterClassEx(&l_wcex);
}


HWND InitInstance(HINSTANCE a_hInst, int a_nCmdShow)
{
	TCHAR l_sTitle[128];
	::LoadString(a_hInst, IDS_APP_TITLE, l_sTitle, ArraySize(l_sTitle));

	HWND l_hWndKierki = ::CreateWindow(c_sWindowClass, l_sTitle, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, a_hInst, NULL);
	if (l_hWndKierki == NULL)
	{
		throw RSystemExc(::GetLastError(), _T("HWNDKIERKI"));
	}

	CHeartsData* l_pData = CHeartsData::GetData(l_hWndKierki);
	ASSERT(l_pData != NULL);

	// create own toolbar setup
	SetupOwnToolbar(l_hWndKierki);

	l_pData->m_hWndTab = ::CreateWindow(WC_TABCONTROL, NULL, 
	   WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, 
	   0, 0, CW_USEDEFAULT, 18, 
	   l_hWndKierki, reinterpret_cast<HMENU>(IDC_MAINTAB), a_hInst, NULL);

	if (l_pData->m_hWndTab == NULL)
	{
		throw RSystemExc(::GetLastError(), _T("HWNDTAB"));
	}

	HFONT l_hFont = GetStockFont(DEFAULT_GUI_FONT);
	::SendMessage(l_pData->m_hWndTab, WM_SETFONT, reinterpret_cast<WPARAM>(l_hFont), 1);

	VERIFY(GameWnd_Register(a_hInst) != NULL);
	l_pData->SetGameWnd(GameWnd_Create(0, WS_CHILD | WS_VISIBLE, 
	   0, 0, CW_USEDEFAULT, CW_USEDEFAULT, 
	   l_hWndKierki, a_hInst, l_hWndKierki, &(l_pData->m_gameData)));

	if (l_pData->GetGameWnd() == NULL)
	{
		throw RSystemExc(::GetLastError(), _T("HWNDGAME"));
	}

	TCHAR l_sTabTitle[128];
	::LoadString(::GetModuleHandle(NULL), IDS_TABTITLE_GAME, l_sTabTitle, ArraySize(l_sTabTitle));
	AddTab(l_hWndKierki, l_sTabTitle);

	VERIFY(ResultWnd_Register(a_hInst) != NULL);
	l_pData->SetResultWnd(ResultWnd_Create(0, WS_CHILD, 
	   0, 0, CW_USEDEFAULT, CW_USEDEFAULT, 
	   l_hWndKierki, a_hInst, E_SR_1, &(l_pData->m_gameData)));

	if (l_pData->GetResultWnd() == NULL)
	{
		throw RSystemExc(::GetLastError(), _T("HWNDRESULTS"));
	}

	l_pData->m_hWndStatusbar = ::CreateWindow(STATUSCLASSNAME, NULL,
	   WS_CHILD | SBARS_SIZEGRIP | WS_VISIBLE, 
	   0, 0, CW_USEDEFAULT, 18, 
	   l_hWndKierki, NULL, a_hInst, NULL);
	if (l_pData->m_hWndStatusbar == NULL)
	{
		throw RSystemExc(::GetLastError());
	}
	ConfigureStatusbar(l_pData->m_hWndStatusbar);

	::SetWindowPos(l_hWndKierki, NULL, 0, 0, 1, 1, SWP_NOZORDER | SWP_NOMOVE);
	::ShowWindow(l_hWndKierki, a_nCmdShow);
	::UpdateWindow(l_hWndKierki);

	tstring l_sName = l_pData->m_gameData.m_regData.GetPlayerName(E_DL_1);
#ifndef NOLOGON
	if (!LogonDlg_DoModal(l_hWndKierki, &l_sName))
	{
		::DestroyWindow(l_hWndKierki);
		return NULL;
	}
#endif

	l_pData->m_gameData.m_regData.SetPlayerName(E_DL_1, l_sName);
	l_pData->m_gameData.m_regData.m_regPlayers.Serialize();

	return l_hWndKierki;
}



LRESULT CALLBACK Kierki_WndProc(HWND a_hWnd, UINT a_iMsg, WPARAM a_wParam, LPARAM a_lParam)
{
	switch (a_iMsg) 
	{
	case WM_CREATE:
		return OnCreate(a_hWnd);

	case WM_GETMINMAXINFO:
		OnGetMinMaxInfo(a_hWnd, reinterpret_cast<LPMINMAXINFO>(a_lParam));
		break;

	case WM_SIZE:
		OnSize(a_hWnd, a_wParam, LOWORD(a_lParam), HIWORD(a_lParam));
		break;

	case WM_PAINT:
		OnPaint(a_hWnd);
		break;

	case WM_NOTIFY:
		OnNotify(a_hWnd, static_cast<int>(a_wParam), reinterpret_cast<LPNMHDR>(a_lParam));
		break;

	case WM_CLOSE:
		OnClose(a_hWnd);
		return ::DefWindowProc(a_hWnd, a_iMsg, a_wParam, a_lParam);

	case WM_DESTROY:
		OnDestroy(a_hWnd);
		break;

	case WM_NCDESTROY:
		OnNcDestroy(a_hWnd);
		break;

	case WM_COMMAND:
		OnCommand(a_hWnd, a_wParam, a_lParam);
		break;

	case WM_MOUSEMOVE:
		OnMouseMove(a_hWnd, LOWORD(a_lParam), HIWORD(a_lParam));
		break;

	case WM_LBUTTONDOWN:
		OnLButtonDown(a_hWnd, GET_X_LPARAM(a_lParam), GET_Y_LPARAM(a_lParam));
		break;

	case WM_LBUTTONUP:
		OnLButtonUp(a_hWnd, GET_X_LPARAM(a_lParam), GET_Y_LPARAM(a_lParam));
		break;

	case WM_ENTERMENULOOP:
		OnEnterMenuLoop(a_hWnd);
		break;

	case WM_APP_NEXTSERIE:
		OnAppNextSerie(a_hWnd);
		break;

	case WM_APP_GAMEOVER:
		OnAppGameOver(a_hWnd);
		break;

	case WM_APP_TRUMPSCHOSEN:
		OnAppTrumpsChosen(a_hWnd);
		break;

	case WM_APP_SETTITLE:
		OnAppSetTitle(a_hWnd);
		break;

	case WM_APP_PLAYCARD:
		OnAppPlayCard(a_hWnd);
		break;

	case WM_APP_READY:
		OnAppReady(a_hWnd);
		break;

	case WM_APP_CHOOSETRUMPS:
		OnAppChooseTrumps(a_hWnd);
		break;

	case WM_APP_CONFIRMTRICK:
		OnAppConfirmTrick(a_hWnd);
		break;

	default:
		return ::DefWindowProc(a_hWnd, a_iMsg, a_wParam, a_lParam);
	}
	return 0;
}


LRESULT OnCreate(HWND a_hWnd)
{
	// store app data in window structure
	CHeartsData* l_pData = new CHeartsData();
	if (l_pData == NULL)
	{
		return -1;
	}
	
	CHeartsData::SetData(a_hWnd, l_pData);
	ConfigureMenu(a_hWnd);

	return 0;
}


void OnClose(HWND a_hWnd)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
#ifdef LOGIN_DIALOG
	if (l_pData->m_gameData.GetGame() != E_GM_NOTHING)
	{
		if (DecisionBox(a_hWnd, IDS_SAVEGAMEQUESTION))
		{
			SaveGame(a_hWnd);
		}
	}
#endif
}


void OnDestroy(HWND a_hWnd)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	l_pData->m_toolbar.DeleteBitmaps();
	::PostQuitMessage(0);
}


void OnNcDestroy(HWND a_hWnd)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	delete l_pData;
}


void OnGetMinMaxInfo(HWND a_hWnd, LPMINMAXINFO a_lpMinMaxInfo)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	// first call is before OnCreate!
	if (l_pData != NULL)
	{
		SIZE l_size;
		GetMinSize(a_hWnd, &l_size);
		a_lpMinMaxInfo->ptMinTrackSize.x = l_size.cx;
		a_lpMinMaxInfo->ptMinTrackSize.y = l_size.cy;
	}
}


void OnSize(HWND a_hWnd, WPARAM a_wParam, int a_dxWidth, int a_dyHeight)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	ResizeTab(a_hWnd, a_dxWidth, a_dyHeight);
	l_pData->m_toolbar.Resize(a_dxWidth);
	::SendMessage(l_pData->m_hWndStatusbar, WM_SIZE, a_wParam, MAKELPARAM(a_dxWidth, a_dyHeight));
}

// ---------------------------------------------------------
// Painting own toolbar
void OnPaint(
	HWND a_hWnd		//IN game wnd
)
{
	PAINTSTRUCT l_ps;
	HDC l_hdc = ::BeginPaint(a_hWnd, &l_ps);

	RECT l_rectWin;
	::GetClientRect(a_hWnd, &l_rectWin);
	{
		// prepare correct DC
#ifdef _DEBUG 
		HDC l_hPaintDC = l_hdc;
#else
		RMemDC l_memDC(l_hdc, &l_rectWin);
		HDC l_hPaintDC = l_memDC;
#endif
		Draw(a_hWnd, l_hPaintDC);
	}
	::EndPaint(a_hWnd, &l_ps);
}


void ConfigureMenu(HWND a_hWnd)
{

	HMENU hMenu = ::GetMenu(a_hWnd);
	HMENU hFileMenu = ::GetSubMenu(hMenu, 0); // 0 is the index of the "File" popup menu

	// Load the bitmap images from the resources
	HBITMAP hbmNewGame = ::LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_NEWGAME));

	// Set the icon for the "New Game" menu item
	if (hbmNewGame)
	{
		MENUITEMINFO l_mii;	
		l_mii.cbSize = sizeof(MENUITEMINFO);
		l_mii.fMask = MIIM_BITMAP;
		l_mii.hbmpItem = hbmNewGame;

		SetMenuItemInfo(hFileMenu, IDM_GAME_NEW, FALSE, &l_mii);
	}
}


void ResizeTab(HWND a_hWnd, int a_dxWidth, int a_dyHeight)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	RECT l_rectKierki;
	::GetClientRect(a_hWnd, &l_rectKierki);
	RECT l_rectStatusbar = { 0 };

	// TODO here to change if own toolbar can change position
	int l_dyRibbon = l_pData->m_toolbar.GetExtent();

	::GetWindowRect(l_pData->m_hWndStatusbar, &l_rectStatusbar);

	int l_dxTab = RectWidth(l_rectKierki);
	int l_dyTab = RectHeight(l_rectKierki) - RectHeight(l_rectStatusbar) - l_dyRibbon ;

	RECT l_rectTab;
	::SetRect(&l_rectTab, 0, 0, l_dxTab, l_dyTab); 
	TabCtrl_AdjustRect(l_pData->m_hWndTab, FALSE, &l_rectTab); 

	// Size the tab control to fit the client area.
	HDWP l_hdwp = ::BeginDeferWindowPos(static_cast<int>(l_pData->m_arrTabs.size()) + 1); 
	l_hdwp = ::DeferWindowPos(l_hdwp, l_pData->m_hWndTab, NULL, 0, l_dyRibbon, 
		l_dxTab, l_dyTab, 
		SWP_NOZORDER);

	for (UINT l_iTab = 0; l_iTab < l_pData->m_arrTabs.size(); l_iTab++)
	{
		l_hdwp = ::DeferWindowPos(l_hdwp, 
			l_pData->m_arrTabs[l_iTab], HWND_TOP, 
			l_rectTab.left, l_rectTab.top + l_dyRibbon,
			RectWidth(l_rectTab), RectHeight(l_rectTab), 0);
	}
	::EndDeferWindowPos(l_hdwp); 
}


static void SetupOwnToolbar(HWND a_hWnd)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);


	l_pData->m_toolbar.SetExtent(56);
	l_pData->m_toolbar.SetSpacing(3);
	// all bitmaps will be released in OnDestroy (with m_toolbar.DeleteBitmaps)	
	// new game icon
	TCHAR l_sNewGame[128];
	::LoadString(::GetModuleHandle(NULL), IDS_NEWGAME, l_sNewGame, ArraySize(l_sNewGame));
	HBITMAP l_hBmpNewGame = RDraw::LoadImageFromResource(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_PNG_NEWGAME), L"PNG");
	l_pData->m_toolbar.AddItem(l_sNewGame, IDM_GAME_NEW, l_hBmpNewGame, OWNTOOLBAR_TYPE_BMP, true);

	// open game icon
	TCHAR l_sOpenGame[128];
	::LoadString(::GetModuleHandle(NULL), IDS_OPENGAME, l_sOpenGame, ArraySize(l_sOpenGame));
	HBITMAP l_hBmpOpenGame = RDraw::LoadImageFromResource(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_PNG_OPENGAME), L"PNG");
	l_pData->m_toolbar.AddItem(l_sOpenGame, IDM_GAME_OPEN, l_hBmpOpenGame, OWNTOOLBAR_TYPE_BMP, true);

	// save game icon
	TCHAR l_sSaveGame[128];
	::LoadString(::GetModuleHandle(NULL), IDS_SAVEGAME, l_sSaveGame, ArraySize(l_sSaveGame));
	HBITMAP l_hBmpSaveGame = RDraw::LoadImageFromResource(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_PNG_SAVEGAME), L"PNG");
	l_pData->m_toolbar.AddItem(l_sSaveGame, IDM_GAME_SAVE, l_hBmpSaveGame, OWNTOOLBAR_TYPE_BMP, true);

	// save game as icon
	TCHAR l_sSaveGameAs[128];
	::LoadString(::GetModuleHandle(NULL), IDS_SAVEGAMEAS, l_sSaveGameAs, ArraySize(l_sSaveGameAs));
	HBITMAP l_hBmpSaveGameAs = RDraw::LoadImageFromResource(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_PNG_SAVEGAMEAS), L"PNG");
	l_pData->m_toolbar.AddItem(l_sSaveGameAs, IDM_GAME_SAVEAS, l_hBmpSaveGameAs, OWNTOOLBAR_TYPE_BMP, true);

	// options
	TCHAR l_sOptions[128];
	::LoadString(::GetModuleHandle(NULL), IDS_OPTIONS, l_sOptions, ArraySize(l_sOptions));
	HBITMAP l_hBmpOptions = RDraw::LoadImageFromResource(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_PNG_OPTIONS), L"PNG");
	l_pData->m_toolbar.AddItem(l_sOptions, IDM_GAME_OPTIONS, l_hBmpOptions, OWNTOOLBAR_TYPE_BMP, false);

}



void ConfigureStatusbar(HWND a_hWndStatusbar)
{
}


void AddTab(HWND a_hWnd, LPTSTR a_sTabName)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	TCITEM l_tabItem;
	l_tabItem.mask = TCIF_TEXT;

	l_tabItem.pszText = a_sTabName;
	::SendMessage(l_pData->m_hWndTab, TCM_INSERTITEM, 0, reinterpret_cast<LPARAM>(&l_tabItem));
}


// ---------------------------------------------------------
// Draws own toolbar
void
Draw(
	HWND a_hWnd,	//IN game wnd
	HDC a_hDC		//IN device context
)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	RECT l_rectWin;
	::GetClientRect(a_hWnd, &l_rectWin);

	// TODO here to change if own toolbar can change position (horizontal / vertical)
	l_rectWin.bottom = l_pData->m_toolbar.GetExtent();

	l_pData->m_toolbar.Draw(a_hDC, l_rectWin);

}


void OnNotify(HWND a_hWnd, int a_idCtrl, LPNMHDR a_pNmHdr)
{
	switch (a_idCtrl)
	{
	case IDC_MAINTAB:
		OnNotifyTab(a_hWnd, a_pNmHdr);
		break;
	}
}


void OnNotifyTab(HWND a_hWnd, LPNMHDR a_pNmHdr)
{
	switch (a_pNmHdr->code)
	{
		case TCN_SELCHANGE:
			OnTabSelChange(a_hWnd);
			break;
	}
}


void OnCommand(HWND a_hWnd, WPARAM a_wParam, LPARAM a_lParam)
{

	switch (LOWORD(a_wParam))
	{

	case IDM_GAME_NEW:
		OnGameNew(a_hWnd);
		break;
	case IDM_GAME_OPTIONS:
		OnGameOptions(a_hWnd);
		break;

	case IDM_GAME_SAVE:
		OnGameSave(a_hWnd);
		break;

	case IDM_GAME_SAVEAS:
		OnGameSaveAs(a_hWnd);
		break;

	case IDM_GAME_OPEN:
		OnGameOpen(a_hWnd);
		break;

	case IDM_GAME_EXIT:
		::SendMessage(a_hWnd, WM_CLOSE, 0, 0L);
		break;

	}
}


void OnMouseMove(HWND a_hWnd, LONG a_x, LONG a_y)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	POINT l_pt = { a_x, a_y };

	// check if mouse is over toolbar

	UINT l_iPrevHover = l_pData->m_toolbar.GetHover();
	UINT l_iNewHover = l_pData->m_toolbar.HitTest(l_pt);

	// hover on the same item as before?
	if (l_iPrevHover == l_iNewHover)
	{
		return;
	}


	l_pData->m_toolbar.SetHover(l_iNewHover);
	if (l_pData->m_toolbar.IsHover())
		::SetCapture(a_hWnd);
	else
		if (!(GetAsyncKeyState(VK_LBUTTON) != 0))
			::ReleaseCapture();
	
	// invalidate toolbar
	l_pData->m_toolbar.InvalidateRect(a_hWnd, l_iPrevHover);
	l_pData->m_toolbar.InvalidateRect(a_hWnd, l_iNewHover);

}


void OnLButtonDown(HWND a_hWnd, int a_x, int a_y)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	POINT l_pt = { a_x, a_y };

	// check if mouse is over toolbar

	l_pData->m_toolbar.SetHover(l_pData->m_toolbar.HitTest(l_pt));


	// invalidate toolbar
	if (l_pData->m_toolbar.IsHover())
	{
		l_pData->m_toolbar.SetPressed(l_pData->m_toolbar.GetHover());
		::SetCapture(a_hWnd);
	}
	l_pData->m_toolbar.InvalidateRect(a_hWnd, l_pData->m_toolbar.GetHover());
}


void OnLButtonUp(HWND a_hWnd, int a_x, int a_y)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	POINT l_pt = { a_x, a_y };

	// check if mouse is over toolbar

	UINT l_iHover = l_pData->m_toolbar.HitTest(l_pt);

	::ReleaseCapture();
	l_pData->m_toolbar.SetPressed(-1);
	l_pData->m_toolbar.InvalidateRect(a_hWnd, l_pData->m_toolbar.GetHover());
	if (l_iHover != -1)
	{
		::SendMessage(a_hWnd, WM_COMMAND, MAKEWPARAM(l_pData->m_toolbar.GetHoverIdCmd(), 0), 0);
	}

}


void OnEnterMenuLoop(HWND a_hWnd)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	EnableSaveMenu(a_hWnd, l_pData->m_gameData.GetGame() != E_GM_NOTHING);
}


void OnTabSelChange(HWND a_hWnd)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	SetActiveTab(a_hWnd, TabCtrl_GetCurSel(l_pData->m_hWndTab));
}


HWND GetCurTab(HWND a_hWnd)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	HWND l_hWnd = l_pData->GetTabWnd(TabCtrl_GetCurSel(l_pData->m_hWndTab));
	return l_hWnd;
}


bool DecisionBox(HWND a_hWnd, UINT a_idPrompt)
{
	TCHAR l_sTitle[128];
	TCHAR l_sPrompt[128];
	::LoadString(::GetModuleHandle(NULL), IDS_DECISION, l_sTitle, ArraySize(l_sTitle));
	::LoadString(::GetModuleHandle(NULL), a_idPrompt, l_sPrompt, ArraySize(l_sPrompt));
	return (::MessageBox(a_hWnd, l_sPrompt, l_sTitle, MB_YESNO | MB_ICONQUESTION) == IDYES);
}


// ---------------------------------------------------------
// Next serie of game
//
void NextSerie(HWND a_hWnd)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);

	short l_nTabCount = TabCtrl_GetItemCount(l_pData->m_hWndTab);
	for (short l_nTab = l_nTabCount; l_nTab > (l_pData->m_gameData.GetSerie() + 2); l_nTab--)
	{
		TabCtrl_DeleteItem(l_pData->m_hWndTab, l_nTab - 1);
	}
	l_nTabCount = TabCtrl_GetItemCount(l_pData->m_hWndTab);

	for (short l_nTab = l_nTabCount + 1; l_nTab <= (l_pData->m_gameData.GetSerie() + 2); l_nTab++)
	{
		AddResultTab(a_hWnd, static_cast<T_SERIE>(l_nTab - 2));
	}
	
	// add new result tabs as needed
	SetTitle(a_hWnd);
}


void OnGameNew(HWND a_hWnd)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	if (l_pData->m_gameData.GetGame() != E_GM_NOTHING)
	{
		if (!DecisionBox(a_hWnd, IDS_NEWGAME))
		{
			return;
		}
	}
	StartGame(a_hWnd, false);
}



void OnGameSave(HWND a_hWnd)
{
	SaveGame(a_hWnd);
}


void OnGameSaveAs(HWND a_hWnd)
{
	TCHAR l_sFile[1024];
	if (!GetOpenSaveFile(a_hWnd, false, l_sFile, ArraySize(l_sFile)))
	{
		return;
	}
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	l_pData->m_gameData.Save(l_sFile);
}


void OnGameOpen(HWND a_hWnd)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	if (l_pData->m_gameData.GetGame() != E_GM_NOTHING)
	{
		if (DecisionBox(a_hWnd, IDS_SAVEGAMEBEFOREOPENQUESTION))
		{
			SaveGame(a_hWnd);
		}
	}
	TCHAR l_sFile[1024];
	ZeroMemory(l_sFile, sizeof(l_sFile));
	if (!GetOpenSaveFile(a_hWnd, true, l_sFile, ArraySize(l_sFile)))
	{
		return;
	}
	l_pData->m_gameData.Restore(l_sFile);
	StartGame(a_hWnd, true);
}


void OnGameOptions(HWND a_hWnd)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	if (OptionsDlg_DoModal(a_hWnd, &(l_pData->m_gameData.m_regData)) == IDCANCEL)
	{
		return;
	}
	l_pData->m_gameData.m_regData.Serialize();
	GameWnd_Refresh(l_pData->GetGameWnd());
	ResultWnd_Refresh(l_pData->GetResultWnd());

	::RedrawWindow(GetCurTab(a_hWnd), NULL, NULL, 
		RDW_INVALIDATE | RDW_ERASE | RDW_ERASENOW | RDW_UPDATENOW);
}


void OnAppNextSerie(HWND a_hWnd)
{
	NextSerie(a_hWnd);
}


void OnAppGameOver(HWND a_hWnd)
{
	// wyœwietlenie wyników i ewentualne rozdanie nowej serii
	SetResultsTab(a_hWnd);
	if (DecisionBox(a_hWnd, IDS_PROMPT_NEWGAME))
	{
		CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
		l_pData->m_gameData.GameOver();
		StartGame(a_hWnd, false);
	}
}

void OnAppTrumpsChosen(HWND a_hWnd)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	SetTitle(a_hWnd);
	SetStatusBarText(a_hWnd, IDS_PLAYCARD) ;
}


void OnAppSetTitle(HWND a_hWnd)
{
	SetTitle(a_hWnd);
}


void OnAppPlayCard(HWND a_hWnd)
{
	SetStatusBarText(a_hWnd, IDS_PLAYCARD);
}


void OnAppReady(HWND a_hWnd)
{
	SetStatusBarText(a_hWnd, IDS_READY);
}


void OnAppChooseTrumps(HWND a_hWnd)
{
	SetStatusBarText(a_hWnd, IDS_CHOOSETRUMPS);
}


void OnAppConfirmTrick(HWND a_hWnd)
{
	SetStatusBarText(a_hWnd, IDS_CONFIRMTRICK);
}

void SetTitle(HWND a_hWnd)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	::SetWindowText(a_hWnd, l_pData->m_gameData.CreateTitle().c_str());
}


void AddResultTab(HWND a_hWnd, T_SERIE a_enSerie)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	TCHAR l_sTitle[128];

	TCITEM l_tabItem;
	l_tabItem.mask = TCIF_TEXT;

	::LoadString(::GetModuleHandle(NULL), IDS_TABTITLE_RESULT, l_sTitle, ArraySize(l_sTitle));
	_sntprintf_s(l_sTitle, ArraySize(l_sTitle), _TRUNCATE, tstring(l_sTitle).c_str(), static_cast<int>(a_enSerie) + 1);
	l_tabItem.pszText = l_sTitle;
	l_tabItem.cchTextMax = static_cast<int>(_tcslen(l_sTitle));

	TabCtrl_InsertItem(l_pData->m_hWndTab, TabCtrl_GetItemCount(l_pData->m_hWndTab) + 1, &l_tabItem);
}


void SetStatusBarText(
	HWND a_hWnd,	//IN window
	UINT a_idStr	//IN string id to display
	)
{
	TCHAR l_sText[512];
	::LoadString(::GetModuleHandle(NULL), a_idStr, l_sText, ArraySize(l_sText));
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	::SendMessage(l_pData->m_hWndStatusbar, SB_SETTEXT, 0, reinterpret_cast<LPARAM>(l_sText));
}


void SetResultsTab(HWND a_hWnd)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	TabCtrl_SetCurSel(l_pData->m_hWndTab, TabCtrl_GetItemCount(l_pData->m_hWndTab) - 1);
	SetActiveTab(a_hWnd, TabCtrl_GetItemCount(l_pData->m_hWndTab) - 1);
}


void SetPlayTab(HWND a_hWnd)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	TabCtrl_SetCurSel(l_pData->m_hWndTab, 0);
	SetActiveTab(a_hWnd, 0);
}


// ---------------------------------------------------------
// Zmiana wymiarów widoku na wymiary ramki
//
void GetMinSize(HWND a_hWnd, LPSIZE a_pSize)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);

	a_pSize->cx = C_VIEW_WIDTH;
	a_pSize->cy = C_VIEW_HEIGHT;
	RECT l_rectSB;
	::GetWindowRect(l_pData->m_hWndStatusbar, &l_rectSB);

	RECT l_rectTab;
	::SetRect(&l_rectTab, 0, 0, a_pSize->cx, a_pSize->cy); 
	TabCtrl_AdjustRect(l_pData->m_hWndTab, TRUE, &l_rectTab); 

	// TODO here to change if own toolbar can change position
	a_pSize->cy = 
		::GetSystemMetrics(SM_CYCAPTION) +	// nag³ówek okna
		::GetSystemMetrics(SM_CYMENU) +		
		l_pData->m_toolbar.GetExtent() +
		::GetSystemMetrics(SM_CYFRAME) * 2 + 
		RectHeight(l_rectSB) + 					// wysokosæ status bar'a
		RectHeight(l_rectTab);	// nag³ówek okna

	a_pSize->cx = RectWidth(l_rectTab) + 2 * ::GetSystemMetrics(SM_CXFRAME);
}


void SetActiveTab(HWND a_hWnd, int a_iTab)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);

	HWND l_hWndGame = l_pData->GetGameWnd();
	HWND l_hWndResult = l_pData->GetResultWnd();
	if (a_iTab == 0)
	{
		::ShowWindow(l_hWndResult, SW_HIDE);
		::ShowWindow(l_hWndGame, SW_SHOW);
		::SetFocus(l_hWndGame);
	}
	else
	{
		::ShowWindow(l_hWndGame, SW_HIDE);
		ResultWnd_SetSerie(l_hWndResult, static_cast<T_SERIE>(a_iTab - 1));
		::ShowWindow(l_hWndResult, SW_SHOW);
		::SetFocus(l_hWndResult);
	}
	RECT l_rect;
	::GetWindowRect(a_hWnd, &l_rect);
	ResizeTab(a_hWnd, RectWidth(l_rect), RectHeight(l_rect));
}


bool GetOpenSaveFile(HWND a_hWnd, bool a_bOpen, LPTSTR a_psFile, DWORD a_iSizeFile)
{
	OPENFILENAME l_ofn;
	ZeroMemory(&l_ofn, sizeof(l_ofn));
	l_ofn.lStructSize = sizeof(l_ofn);
	l_ofn.hwndOwner = a_hWnd;

	TCHAR l_sFilterBuf[512];
	TCHAR l_sFilterExtBuf[256];

	ZeroMemory(l_sFilterBuf, sizeof(l_sFilterBuf));
	::LoadString(::GetModuleHandle(NULL), IDS_KIERKIFILTERNAME, l_sFilterBuf, ArraySize(l_sFilterBuf));
	ZeroMemory(l_sFilterExtBuf, sizeof(l_sFilterExtBuf));
	::LoadString(::GetModuleHandle(NULL), IDS_KIERKIFILTEREXT, l_sFilterExtBuf, ArraySize(l_sFilterExtBuf));

	// prepare filter string - it’s a double-null-terminated list of null-terminated string. Just easier using tstring
	tstring l_sFilter = l_sFilterBuf;
	l_sFilter = l_sFilter + L" (" + cc_cAsterisk + l_sFilterExtBuf + L')';
	
	l_sFilter.push_back(L'\0');
	l_sFilter += cc_cAsterisk;
	l_sFilter += l_sFilterExtBuf;
	l_sFilter.push_back(L'\0');
	l_sFilter.push_back(L'\0');

	l_ofn.lpstrFilter = l_sFilter.c_str();

	l_ofn.Flags = OFN_EXPLORER | OFN_ENABLESIZING | OFN_HIDEREADONLY;
	if (a_bOpen)
	{
		l_ofn.Flags |= (OFN_NOREADONLYRETURN | OFN_FILEMUSTEXIST);
	}
	else
	{
		l_ofn.Flags |= (OFN_NOREADONLYRETURN | OFN_OVERWRITEPROMPT);
	}
	*a_psFile = _T('\0');
	l_ofn.lpstrFile = a_psFile;
	l_ofn.nMaxFile = a_iSizeFile;
	if (a_bOpen)
	{
		if (!::GetOpenFileName(&l_ofn))
		{
			return false;
		}
	}
	else
	{
		if (!::GetSaveFileName(&l_ofn))
		{
			return false;
		}
	}

	// add extension if needed
	if (!a_bOpen)
	{
		LPCTSTR l_psFileExt = a_psFile + _tcslen(a_psFile) - _tcslen(l_sFilterExtBuf);
		if (_tcsncmp(l_psFileExt, l_sFilterExtBuf, _tcslen(l_sFilterExtBuf)) != 0)
		{
			_tcsncat_s(a_psFile, a_iSizeFile,  l_sFilterExtBuf, _TRUNCATE);
		}
	}

	return true;
}


void EnableSaveMenu(HWND a_hWnd, BOOL a_bEnable)
{
	HMENU l_hMenuMain = ::GetMenu(a_hWnd);
	HMENU l_hMenuGame = ::GetSubMenu(l_hMenuMain, 0);

	MENUITEMINFO l_mii;
	ZeroMemory(&l_mii, sizeof(l_mii));
	l_mii.cbSize = sizeof(l_mii);
	l_mii.fMask = MIIM_STATE;
	if (a_bEnable)
	{
		l_mii.fState = MFS_ENABLED;
	}
	else
	{
		l_mii.fState = MFS_DISABLED | MF_GRAYED;
	}

	::SetMenuItemInfo(l_hMenuGame, IDM_GAME_SAVE, FALSE, &l_mii);
	::SetMenuItemInfo(l_hMenuGame, IDM_GAME_SAVEAS, FALSE, &l_mii);
}


void SaveGame(HWND a_hWnd)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	tstring l_sFile;
	if (!l_pData->m_gameData.IsFile())
	{
		TCHAR l_sFileBuf[1024];
		if (!GetOpenSaveFile(a_hWnd, false, l_sFileBuf, ArraySize(l_sFileBuf)))
		{
			return;
		}
		l_sFile = l_sFileBuf;
	}
	else
	{
		l_sFile = l_pData->m_gameData.GetFile();
	}
	l_pData->m_gameData.Save(l_sFile.c_str());
}




void StartGame(HWND a_hWnd, bool a_bOpen)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	SetPlayTab(a_hWnd);
	if (a_bOpen)
	{
		l_pData->m_gameData.NextGame(FALSE);
	}
	else
	{
		l_pData->m_gameData.NewGame();
	}
	NextSerie(a_hWnd);
	GameWnd_NewDeal(l_pData->GetGameWnd(), a_bOpen);
}

