// Kierki.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Kierki.h"
#include "resource.h"
#include <rcommon/ExceptionHandler.h>
#include <rcommon/RException.h>
#include <rcommon/ROwnExc.h>
#include <rcommon/RSystemExc.h>
#include <commctrl.h>
#include <commdlg.h>
#include <rcommon/rstring.h>
#include "GameWnd.h"
#include "ResultWnd.h"
#include "HelpWnd.h"
#include "HelpJson.h"
#include "LogonDlg.h"
#include "OptionsDlg.h"
#include "HeartsGame.h"
#include "layout.h"
#include "FontFactory.h"
#include <RCards/RCards.h>
#include "HeartsData.h"
#include <rcommon/DrawUtl.h>
#include <rcommon/RMemDC.h>
#include <rcommon/RMessageBox.h>
#include <rcommon/SafeWndProc.hpp>

// for better drawing
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "version.lib")

#pragma todo("Test next dealer comprehensively.")
#pragma todo("Test saving game comprehensively, especially who is the next dealer.")
// TODO (bug) Asks for saving just after saving a game
// TODO (bug) sometimes trump color is not displayed in the window title
// TODO (bug) some card edges not painted correctly - it seems something wrong after drawing at least one card form player's cards
// TODO (bug / improvement) - m_regData can't be in HeartsGame
// TODO (bug) - force correct directory (Games) - check saving and loading the game - it behaves strangely, starts from incorrect directory
// TODO (bug?) check behaviour of cover, when app used for the first time
// TODO (bug) About system button doesn't work
// TODO (bug) bad rendering of covers (background)
// TODO (bug) during recover if I selected card very quickly it has a problem with inverting card - maybe replacing SetCapture is correct approach
// TODO (improvement) implement help for every game
// TODO (improvement) TAB key behaviour in dialogs
// TODO (improvement) add log messaging to exception handling (maybe even with log levels and no messagebox)
// TODO (improvement) refactor StatusBar - cc_btSliderMax and similar should be part of StatusBar class - as it is specialized class for this app, not general class
// TODO (improvement) change fonts also for tooltips for fancy style
// TODO (improvement) Move main window functionalities such as own toolbar and status bar to separate class
// TODO (improvement) replace SetCapture with TrackMouseEvent in GameWnd
// TODO (improvement) implement DPI awareness, replace AdjustWindowRectEx with AdjustWindowRectExForDpi
// TODO (improvement) disabling items in own toolbar, as previously with menu items: check and remove function OnEnterMenuLoop
// TODO (improvement) clean up / refactor fancy style (the same font and size for fancy and regular styles)
// TODO (improvement) put static for selecting covers in rcommon library
// TODO (improvement) restore possibility of taking bitmap from other resources
// TODO (improvement) should be a part of RCards module, to be refactored
// TODO (improvement) improve settings - move some items to different position and allow to set other items currently hidden
// TODO (improvement) handle nlohmann json library in a proper way - downloading from repository
// TODO (improvement) allow selecting different table blanket
// TODO (improvement) add more languages
// TODO (improvement) OwnToolbar - add AdjustRect function
// TODO (improvement) Rethink OwnToolbar architecture, add to library of components
// TODO (improvement) instead of a few structures in GameWndData regarding positions, prepare one structure for all positions regarding players
// TODO (improvement) remove unused resources
// TODO (improvement) check all resources for unicode
// TODO (improvement) remove unused code	
// TODO (improvement) add option to load other cards decks and/or graphics
// TODO (improvement) Instead of using bitmap as a background for the results I can use function to draw notebook, and then draw table on the right side of the vertical red line ;-)
// TODO (improvement) get rid of use of RListCtrl in ResultWnd - would be much easier, flexible and effective to just draw it by ourselves
// TODO (improvement) add all sizes from layout.h into registry



#define IDC_MAINTAB 1000

static const TCHAR cc_sWindowClass[] = _T("HEARTS");	// the main window class name

// Forward declarations of functions included in this code module:
inline static ATOM RegisterKierki(HINSTANCE a_hInst);
inline static int RunApp(HINSTANCE a_hInst, HINSTANCE a_hPrevInstance, LPTSTR a_lpCmdLine, int a_nCmdShow);
inline static int RunAppThrow(HINSTANCE a_hInst, HINSTANCE a_hPrevInstance, LPTSTR a_lpCmdLine, int a_nCmdShow);
inline static HWND InitInstance(HINSTANCE a_hInst, int a_nCmdShow);
static LRESULT Kierki_WndProc(HWND a_hWnd, UINT a_iMsg, WPARAM a_wParam, LPARAM a_lParam);	// internal use, as throws exceptions

static inline void ConfigureOwnToolbar(HWND a_hWnd);
static void AddTab(HWND a_hWnd, HWND a_hTabItemWnd, LPTSTR a_sTabName);

inline static LRESULT OnCreate(HWND a_hWnd);
inline static void OnPaint(HWND a_hWnd);
inline static void OnDestroy(HWND a_hWnd);
inline static void OnClose(HWND a_hWnd);
inline static void OnNcDestroy(HWND a_hWnd);
inline static void OnSize(HWND a_hWnd, WPARAM a_wParam, int a_dxWidth, int a_dyHeight);
inline static void OnDpiChanged(HWND a_hWnd, LPRECT a_pNewRect);
inline static void OnHScroll(HWND a_hWnd, HWND a_hWndScroll);
inline static LRESULT OnCtlColorStatic(HWND a_hWnd, WPARAM a_wParam, LPARAM a_lParam);


inline static void OnCommand(HWND a_hWnd, WPARAM a_wParam, LPARAM a_lParam);

inline static void OnMouseMove(HWND a_hWnd, LONG a_x, LONG a_y);
inline static void OnMouseLeave(HWND a_hWnd);
inline static void OnLButtonDown(HWND a_hWnd, int a_x, int a_y);
inline static void OnLButtonUp(HWND a_hWnd, int a_x, int a_y);

inline static void OnGetMinMaxInfo(HWND a_hWnd, LPMINMAXINFO a_lpMinMaxInfo);

inline static LRESULT OnNotify(HWND a_hWnd, int a_idCtrl, LPNMHDR a_pNmHdr);
inline static LRESULT OnNotifyTab(HWND a_hWnd, LPNMHDR a_pNmHdr);
inline static LRESULT OnNotifySliderTooltip(HWND a_hWnd, LPNMHDR a_pNmHdr);

inline static BOOL OnTabSelChange(HWND a_hWnd);

inline static void OnGameNew(HWND a_hWnd);
inline static void OnGameNew2(HWND a_hWnd);

inline static void OnGameOptions(HWND a_hWnd);
inline static void OnGameSave(HWND a_hWnd);
inline static void OnGameSaveAs(HWND a_hWnd);
inline static void OnGameOpen(HWND a_hWnd);
inline static void OnEnterMenuLoop(HWND a_hWnd);

inline static void OnToolbarHover(HWND a_hWnd, int a_idItem, LPCTSTR a_sName);
inline static void OnToolbarLeave(HWND a_hWnd);


inline static void OnAppNextSerie(HWND a_hWnd);
inline static void OnAppGameOver(HWND a_hWnd);
inline static void OnAppTrumpsChosen(HWND a_hWnd);
inline static void OnAppSetTitle(HWND a_hWnd);
inline static void OnAppPlayCard(HWND a_hWnd);
inline static void OnAppReady(HWND a_hWnd);
inline static void OnAppChooseTrumps(HWND a_hWnd);
inline static void OnAppConfirmTrick(HWND a_hWnd);

inline static void SetTitle(HWND a_hWnd);
inline static HWND GetCurrentTab(HWND a_hWnd);
static void SetActiveTab(HWND a_hDlg, HWND a_hWndInnerTab, T_SERIE a_enSerie = E_SR_NULL);

static void ResizeTab(HWND a_hWnd);

inline static void AddResultTab(HWND a_hWnd, T_SERIE a_enSerie);

inline static void SetInnerTab(HWND a_hWnd, HWND a_hWndInnerTab, T_SERIE a_enSerie = E_SR_NULL);
inline static void SetResultsTab(HWND a_hWnd, T_SERIE a_enSerie);
inline static void SetGameTab(HWND a_hWnd);
inline static void SetHelpTab(HWND a_hWnd);

// TODO remove this function, replace with CStatusBar::SetLabeLText()
static void SetStatusBarText(HWND a_hWnd, UINT a_idStr);

static void SetFont(HWND a_hWnd);

inline static void CalculateWinSize(HWND a_hWnd, LPSIZE a_pSize);

static void EnableSaveMenu(HWND a_hWnd, BOOL a_bEnable);



static const TCHAR cc_cAsterisk = _T('*');

constexpr BYTE cc_btSliderMax = 255;
constexpr BYTE cc_btSliderMin = 0;
constexpr int cc_iSliderWidth = static_cast<int>((cc_btSliderMax - cc_btSliderMin) / 2);





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

	// for SetTimer();
	BOOL l_bSuppress = FALSE;
	::SetUserObjectInformationW(GetProcessWindowStation(), UOI_TIMERPROC_EXCEPTION_SUPPRESSION, &l_bSuppress, sizeof(l_bSuppress));

	// SetProcessDPIAware(); // For classic DPI awareness
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

	// GDI+ for better graphics
	ULONG_PTR l_gdiplusToken;
	Gdiplus::GdiplusStartupInput l_gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&l_gdiplusToken, &l_gdiplusStartupInput, nullptr);

	// Perform application initialization:
	HWND l_hWndMain;
	try
	{
		l_hWndMain = InitInstance(a_hInst, a_nCmdShow);
	}
	catch (RException& l_exc)
	{
		TCHAR l_sTitle[MAX_PATH];
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

	Gdiplus::GdiplusShutdown(l_gdiplusToken);
	::RCards_DoneCards();
	return static_cast<int>(l_msg.wParam);
}


ATOM RegisterKierki(HINSTANCE a_hInst)
{
	WNDCLASSEX l_wcex;

	l_wcex.cbSize			= sizeof(WNDCLASSEX); 
	l_wcex.style			= CS_HREDRAW | CS_VREDRAW;
	l_wcex.lpfnWndProc		= SafeWndProc<Kierki_WndProc>;
	l_wcex.cbClsExtra		= 0;
	l_wcex.cbWndExtra		= 0;
	l_wcex.hInstance		= a_hInst;
	l_wcex.hIcon			= ::LoadIcon(a_hInst, MAKEINTRESOURCE(IDI_KIERKI));
	l_wcex.hCursor			= ::LoadCursor(NULL, IDC_ARROW);
	l_wcex.hbrBackground	= NULL;
	l_wcex.lpszMenuName		= NULL;
	l_wcex.lpszClassName	= cc_sWindowClass;
	l_wcex.hIconSm			= ::LoadIcon(a_hInst, MAKEINTRESOURCE(IDI_KIERKI));
	return ::RegisterClassEx(&l_wcex);
}


HWND InitInstance(HINSTANCE a_hInst, int a_nCmdShow)
{
	TCHAR l_sTitle[128];
	::LoadString(a_hInst, IDS_APP_TITLE, l_sTitle, ArraySize(l_sTitle));

	HWND l_hWndKierki = ::CreateWindow(cc_sWindowClass, l_sTitle, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, a_hInst, NULL);
	if (l_hWndKierki == NULL)
		throw RSystemExc(_T("HWNDKIERKI"));

	CHeartsData* l_pData = CHeartsData::GetData(l_hWndKierki);
	if (!l_pData)
		throw ROwnExc(_T("Window data not created"));

	// automatic language detection (languages available for use)
	l_pData->m_langManager.DetectLanguages(IDS_LANG_NAME);
	CRegData& l_reg = l_pData->m_regData;
	l_pData->m_langManager.SetLanguage(l_reg.m_regRules.m_idLanguage);

	CFontFactory::SetFontNames(l_reg.m_regHidden.m_sNormalFont, l_reg.m_regHidden.m_sFancyFont);
	CFontFactory::Instance().SetFontStyle(l_reg.m_regView.m_bFancyStyle ? FontStyle::Fancy : FontStyle::Normal);

	tstring l_sName = l_reg.GetPlayerName(E_DL_1);
	if (l_reg.m_regRules.m_bLogonDlg)
	{
		if (!LogonDlg_DoModal(l_hWndKierki, &l_sName, &(l_pData->m_langManager), &(l_reg.m_regRules.m_bLogonDlg)))
		{
			::DestroyWindow(l_hWndKierki);
			return NULL;
		}

		if (l_pData->m_langManager.GetCurrentLangID().has_value())
			if (l_reg.m_regRules.m_idLanguage != l_pData->m_langManager.GetCurrentLangID().value())
				l_reg.m_regRules.m_idLanguage = l_pData->m_langManager.GetCurrentLangID().value();

		l_reg.Serialize();	// save settings
	}

	// create own toolbar setup
	ConfigureOwnToolbar(l_hWndKierki);

	l_pData->m_hWndTab = ::CreateWindow(WC_TABCONTROL, NULL,
	   WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
	   0, 0, CW_USEDEFAULT, 18, 
	   l_hWndKierki, reinterpret_cast<HMENU>(IDC_MAINTAB), a_hInst, NULL);

	if (l_pData->m_hWndTab == NULL)
		throw RSystemExc(_T("HWNDTAB"));

	if (!HelpWnd_Register(a_hInst))
		throw RSystemExc(_T("REGISTER_HWNDHELP"));

	l_pData->SetHelpWnd(HelpWnd_Create(WS_CHILD, l_pData->m_hWndTab, l_pData->m_langManager, l_reg));

	if (l_pData->GetHelpWnd() == NULL)
		throw RSystemExc(_T("HWNDHELP"));

	HelpWnd_LoadInstructions(l_pData->GetHelpWnd(), c_sJsonSect_HowToUseApp);

	TCHAR l_sTabTitle[128]{};
	if (l_reg.m_regRules.m_bHelpVisible)
	{
		::LoadString(::GetModuleHandle(NULL), IDS_HELPTITLE_GAME, l_sTabTitle, ArraySize(l_sTabTitle));
		AddTab(l_hWndKierki, l_pData->GetHelpWnd(), l_sTabTitle);
	}

	if (GameWnd_Register(a_hInst) == NULL)
		throw RSystemExc(_T("REGISTER_HWNDGAME"));

	l_pData->SetGameWnd(GameWnd_Create(0, WS_CHILD, 
	   0, 0, CW_USEDEFAULT, CW_USEDEFAULT, 
		l_pData->m_hWndTab, a_hInst, l_hWndKierki, &(l_pData->GetGameData()), l_reg));

	if (l_pData->GetGameWnd() == NULL)
		throw RSystemExc(_T("CREATE_HWNDGAME"));

	::LoadString(::GetModuleHandle(NULL), IDS_TABTITLE_GAME, l_sTabTitle, ArraySize(l_sTabTitle));
	AddTab(l_hWndKierki, l_pData->GetGameWnd(), l_sTabTitle);

	if (ResultWnd_Register(a_hInst) == NULL)
		throw RSystemExc(_T("REGISTER_HWNDRESULTS"));

	l_pData->SetResultsWnd(ResultWnd_Create(0, WS_CHILD, 
	   0, 0, CW_USEDEFAULT, CW_USEDEFAULT, 
		l_pData->m_hWndTab, a_hInst, E_SR_1, &(l_pData->GetGameData()), l_reg));

	if (l_pData->GetResultsWnd() == NULL)
		throw RSystemExc(_T("CREATE_HWNDRESULTS"));

	if (!l_pData->m_statusbar.Create(l_hWndKierki, a_hInst, cc_btSliderMin, cc_btSliderMax, cc_iSliderWidth, IDI_BRIGHTNESSDOWN, IDI_BRIGHTNESSUP, l_reg.m_regHidden.m_clrStatusBarColor))
		throw RSystemExc(_T("CREATE_STATUSBAR"));

	// set fonts
	SetFont(l_hWndKierki);

	// must be after all windows are created
	if (l_reg.m_regRules.m_bHelpVisible)
		SetActiveTab(l_hWndKierki, l_pData->GetHelpWnd());
	else
		SetActiveTab(l_hWndKierki, l_pData->GetGameWnd());

	::SetWindowPos(l_hWndKierki, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE);
	::ShowWindow(l_hWndKierki, a_nCmdShow);
	::UpdateWindow(l_hWndKierki);

	l_reg.SetPlayerName(E_DL_1, l_sName);
	l_reg.m_regPlayers.Serialize();

	return l_hWndKierki;
}


LRESULT Kierki_WndProc(HWND a_hWnd, UINT a_iMsg, WPARAM a_wParam, LPARAM a_lParam)
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
		return OnNotify(a_hWnd, static_cast<int>(a_wParam), reinterpret_cast<LPNMHDR>(a_lParam));

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

	case WM_MOUSELEAVE:
		OnMouseLeave(a_hWnd);
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

	case WM_DPICHANGED:
		OnDpiChanged(a_hWnd, reinterpret_cast<LPRECT>(a_lParam));
		break;

	case WM_HSCROLL:
		OnHScroll(a_hWnd, reinterpret_cast<HWND>(a_lParam));
		break;

	case WM_CTLCOLORSTATIC:
		return OnCtlColorStatic(a_hWnd, a_wParam, a_lParam);

	
	case WM_TOOLBARHOVER:
		OnToolbarHover(a_hWnd, static_cast<int>(a_wParam), reinterpret_cast<LPCTSTR>(a_lParam));
		break;

	case WM_TOOLBARLEAVE:
		OnToolbarLeave(a_hWnd);
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
	CHeartsData* l_pData = new CHeartsData(a_hWnd);
	if (l_pData == NULL)
	{
		return -1;
	}
	
	const CRegData& l_reg = l_pData->m_regData;
	l_pData->EmplaceGameData(
		l_reg.GetPlayerName(E_DL_1), l_reg.GetPlayerName(E_DL_2),
		l_reg.GetPlayerName(E_DL_3), l_reg.GetPlayerName(E_DL_4));
	CHeartsData::SetData(a_hWnd, l_pData);

	return 0;
}


void OnClose(HWND a_hWnd)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	if (l_pData->GetGameData().GetGame() == E_GM_NULL)
		return;

	if (l_pData->GetGameData().IsGameSaved())
		return;

	if (!DecisionBox(a_hWnd, IDS_SAVEGAMEQUESTION))
		return;

	SaveGame(a_hWnd);
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
		CalculateWinSize(a_hWnd, &l_size);
		a_lpMinMaxInfo->ptMinTrackSize.x = l_size.cx;
		a_lpMinMaxInfo->ptMinTrackSize.y = l_size.cy;
	}
}


void OnSize(HWND a_hWnd, WPARAM a_wParam, int a_dxWidth, int a_dyHeight)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	l_pData->m_toolbar.Resize(a_dxWidth, a_dyHeight);
	ResizeTab(a_hWnd);
	l_pData->m_statusbar.Resize();
}

// ---------------------------------------------------------
// Painting own toolbar
void OnPaint(
	HWND a_hWnd		//IN game wnd
)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	l_pData->m_toolbar.OnPaint(a_hWnd);
}

void OnDpiChanged(HWND a_hWnd, LPRECT a_pNewRect)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	l_pData->m_dpiContext = RDraw::DpiContext(a_hWnd); // Refresh scale
	::SetWindowPos(a_hWnd, NULL, a_pNewRect->left, a_pNewRect->top,
		RectWidth(*a_pNewRect), RectHeight(*a_pNewRect), SWP_NOZORDER | SWP_NOACTIVATE);
}

void OnHScroll(HWND a_hWnd, HWND a_hWndScroll)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);

	if (a_hWndScroll != l_pData->m_statusbar.GetSliderWnd())
		return;

	HWND l_hWndCurrentTab = GetCurrentTab(a_hWnd);
	if (l_hWndCurrentTab == nullptr)
		return;

	int l_iSliderValue = l_pData->m_statusbar.GetSliderValue();
	::SendMessage(l_hWndCurrentTab, WM_APP_SETBRIGHTNESS, cc_btSliderMax - l_iSliderValue, 0L);

}


LRESULT OnCtlColorStatic(HWND a_hWnd, WPARAM a_wParam, LPARAM a_lParam)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	if (l_pData == nullptr)
		return ::DefWindowProc(a_hWnd, WM_CTLCOLORSTATIC, a_wParam, a_lParam);

	HWND l_hWndCtrl = reinterpret_cast<HWND>(a_lParam);
	if (l_pData->m_statusbar.GetLabelWnd() != l_hWndCtrl)
		return ::DefWindowProc(a_hWnd, WM_CTLCOLORSTATIC, a_wParam, a_lParam);

	return l_pData->m_statusbar.OnLabelColor(reinterpret_cast<HDC>(a_wParam));
}



void ResizeTab(HWND a_hWnd)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	RECT l_rectKierki;
	::GetClientRect(a_hWnd, &l_rectKierki);

	RECT l_rectStatusbar {};

	// TODO here to change if own toolbar can change position
	int l_dyRibbon = l_pData->m_toolbar.GetExtent();

	l_pData->m_statusbar.GetWindowRect(&l_rectStatusbar);

	int l_dxTab = RectWidth(l_rectKierki);
	int l_dyTab = RectHeight(l_rectKierki) - RectHeight(l_rectStatusbar) - l_dyRibbon;
	if (l_dyTab <= 0)
		return;

	// resize tab control
	::SetWindowPos(l_pData->m_hWndTab, a_hWnd, 
		0, l_dyRibbon, l_dxTab, l_dyTab, SWP_NOZORDER);
	RECT l_rectTab = { 0, 0, l_dxTab, l_dyTab };

	TabCtrl_AdjustRect(l_pData->m_hWndTab, FALSE, &l_rectTab);

	for (const auto& [l_iKey, l_hWnd] : l_pData->m_mapTabs)
		::SetWindowPos(l_pData->m_mapTabs[l_iKey], l_pData->m_hWndTab,
			l_rectTab.left, l_rectTab.top, RectWidth(l_rectTab), RectHeight(l_rectTab), SWP_NOZORDER);
}



static void ConfigureOwnToolbar(HWND a_hWnd)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);

	l_pData->m_toolbar.SetExtent(56);
	l_pData->m_toolbar.SetSpacing(3);
	// all bitmaps will be released in OnDestroy (with m_toolbar.DeleteBitmaps)	
	// new game icon
	TCHAR l_sNewGame[128];
	::LoadString(::GetModuleHandle(NULL), IDS_NEWGAME, l_sNewGame, ArraySize(l_sNewGame));
	HBITMAP l_hBmpNewGame = RDraw::LoadImageFromResource(::GetModuleHandle(NULL), IDR_PNG_NEWGAME, _T("PNG"));
	l_pData->m_toolbar.AddItem(a_hWnd, l_sNewGame, IDM_GAME_NEW, l_hBmpNewGame, OWNTOOLBAR_TYPE_BMP, true);

	// open game icon
	TCHAR l_sOpenGame[128];
	::LoadString(::GetModuleHandle(NULL), IDS_OPENGAME, l_sOpenGame, ArraySize(l_sOpenGame));
	HBITMAP l_hBmpOpenGame = RDraw::LoadImageFromResource(::GetModuleHandle(NULL), IDR_PNG_OPENGAME, _T("PNG"));
	l_pData->m_toolbar.AddItem(a_hWnd, l_sOpenGame, IDM_GAME_OPEN, l_hBmpOpenGame, OWNTOOLBAR_TYPE_BMP, true);

	// save game icon
	TCHAR l_sSaveGame[128];
	::LoadString(::GetModuleHandle(NULL), IDS_SAVEGAME, l_sSaveGame, ArraySize(l_sSaveGame));
	HBITMAP l_hBmpSaveGame = RDraw::LoadImageFromResource(::GetModuleHandle(NULL), IDR_PNG_SAVEGAME, _T("PNG"));
	l_pData->m_toolbar.AddItem(a_hWnd, l_sSaveGame, IDM_GAME_SAVE, l_hBmpSaveGame, OWNTOOLBAR_TYPE_BMP, true);

	// save game as icon
	TCHAR l_sSaveGameAs[128];
	::LoadString(::GetModuleHandle(NULL), IDS_SAVEGAMEAS, l_sSaveGameAs, ArraySize(l_sSaveGameAs));
	HBITMAP l_hBmpSaveGameAs = RDraw::LoadImageFromResource(::GetModuleHandle(NULL), IDR_PNG_SAVEGAMEAS, _T("PNG"));
	l_pData->m_toolbar.AddItem(a_hWnd, l_sSaveGameAs, IDM_GAME_SAVEAS, l_hBmpSaveGameAs, OWNTOOLBAR_TYPE_BMP, true);

	// options
	TCHAR l_sOptions[128];
	::LoadString(::GetModuleHandle(NULL), IDS_OPTIONS, l_sOptions, ArraySize(l_sOptions));
	HBITMAP l_hBmpOptions = RDraw::LoadImageFromResource(::GetModuleHandle(NULL), IDR_PNG_OPTIONS, _T("PNG"));
	l_pData->m_toolbar.AddItem(a_hWnd, l_sOptions, IDM_GAME_OPTIONS, l_hBmpOptions, OWNTOOLBAR_TYPE_BMP, false);

}


void AddTab(HWND a_hWnd, HWND a_hTabItemWnd, LPTSTR a_sTabName)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	TCITEM l_tabItem{};
	l_tabItem.mask = TCIF_TEXT | TCIF_PARAM;
	l_tabItem.pszText = a_sTabName;
	l_tabItem.lParam = reinterpret_cast<LPARAM>(a_hTabItemWnd);
	TabCtrl_InsertItem(l_pData->m_hWndTab, TabCtrl_GetItemCount(l_pData->m_hWndTab), reinterpret_cast<LPARAM>(&l_tabItem));
}


LRESULT OnNotify(HWND a_hWnd, int a_idCtrl, LPNMHDR a_pNmHdr)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);

	if (a_pNmHdr->hwndFrom == l_pData->m_hWndTab)
		return OnNotifyTab(a_hWnd, a_pNmHdr);
	else if (a_pNmHdr->hwndFrom == l_pData->m_statusbar.GetSliderTooltipWnd())
		return OnNotifySliderTooltip(a_hWnd, a_pNmHdr);

	return 0;
}


LRESULT OnNotifyTab(HWND a_hWnd, LPNMHDR a_pNmHdr)
{
	switch (a_pNmHdr->code)
	{
		case TCN_SELCHANGE:
			return OnTabSelChange(a_hWnd);
	}
	return 0;
}



LRESULT OnNotifySliderTooltip(HWND a_hWnd, LPNMHDR a_pNmHdr)
{
	switch (a_pNmHdr->code)
	{
	case TTN_NEEDTEXT:
		{
			CHeartsData* l_pData = CHeartsData::GetData(a_hWnd); // notify comes from the slider, not for the tooltip
			return l_pData->m_statusbar.OnTooltipNeedText(a_pNmHdr);

		}
	}
	return 0;
}



void OnCommand(HWND a_hWnd, WPARAM a_wParam, LPARAM a_lParam)
{

	switch (LOWORD(a_wParam))
	{

	case IDM_GAME_NEW:
		OnGameNew2(a_hWnd);
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
	l_pData->m_toolbar.OnMouseMove(a_hWnd, a_x, a_y);
}


void OnMouseLeave(HWND a_hWnd)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	l_pData->m_toolbar.OnMouseLeave(a_hWnd);
}

void OnLButtonDown(HWND a_hWnd, int a_x, int a_y)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	l_pData->m_toolbar.OnLButtonDown(a_hWnd, a_x, a_y);
}


void OnLButtonUp(HWND a_hWnd, int a_x, int a_y)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	l_pData->m_toolbar.OnLButtonUp(a_hWnd, a_x, a_y);
}


void OnToolbarHover(HWND a_hWnd, int a_idItem, LPCTSTR a_sName)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	l_pData->m_statusbar.SetLabelText(a_sName);
}


void OnToolbarLeave(HWND a_hWnd)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	l_pData->m_statusbar.SetLabelText(_T(""));
}


void OnEnterMenuLoop(HWND a_hWnd)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	EnableSaveMenu(a_hWnd, l_pData->GetGameData().GetGame() != E_GM_NULL);
}


BOOL OnTabSelChange(HWND a_hWnd)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);

	TCITEM l_tabItem{};
	l_tabItem.mask = TCIF_PARAM;
	TabCtrl_GetItem(l_pData->m_hWndTab, TabCtrl_GetCurSel(l_pData->m_hWndTab), &l_tabItem);

	if (l_tabItem.lParam == 0)
		throw ROwnExc(_T("OnTabSelChange - incorrect Tab Handle"));

	HWND l_hWndInnerTab = reinterpret_cast<HWND>(l_tabItem.lParam);
	T_SERIE l_enSerie = E_SR_NULL;

	// in case of results tab, we need to deduce which serie it is
	if (l_hWndInnerTab == l_pData->GetResultsWnd())
	{
		// deduce serie, by counting how many previous tabs are result tabs
		short l_nTabCount = 0;
		for (short l_nTab = TabCtrl_GetCurSel(l_pData->m_hWndTab) - 1; l_nTab >= 0; l_nTab--)
		{
			l_tabItem.mask = TCIF_PARAM;
			l_tabItem.lParam = 0;
			TabCtrl_GetItem(l_pData->m_hWndTab, l_nTab, &l_tabItem);
			if (l_tabItem.lParam == reinterpret_cast<LPARAM>(l_pData->GetResultsWnd()))
			{
				l_nTabCount++;
			}
		}
		l_enSerie = static_cast<T_SERIE>(l_nTabCount);

	}
	SetActiveTab(a_hWnd, l_hWndInnerTab, l_enSerie);
	return FALSE;
}


HWND GetCurrentTab(HWND a_hWnd)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);

	TCITEM l_tabItem{};
	l_tabItem.mask = TCIF_PARAM;
	TabCtrl_GetItem(l_pData->m_hWndTab, TabCtrl_GetCurSel(l_pData->m_hWndTab), &l_tabItem);
	ASSERT(l_tabItem.lParam != 0);
	return reinterpret_cast<HWND>(l_tabItem.lParam);
}


bool DecisionBox(HWND a_hWnd, UINT a_idPrompt)
{
	TCHAR l_sPrompt[128];
	::LoadString(::GetModuleHandle(NULL), a_idPrompt, l_sPrompt, ArraySize(l_sPrompt));

	return DecisionMessageBox(a_hWnd, l_sPrompt);
}


// ---------------------------------------------------------
// Next serie of game
//
void NextSerie(HWND a_hWnd)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);

	short l_nTabCount = TabCtrl_GetItemCount(l_pData->m_hWndTab);
	for (short l_nTab = l_nTabCount - 1; l_nTab >= 0; l_nTab--)
	{
		// remove all result tabs
		TCITEM l_tabItem{};
		l_tabItem.mask = TCIF_PARAM;
		TabCtrl_GetItem(l_pData->m_hWndTab, l_nTab, &l_tabItem);
		if (l_tabItem.lParam == reinterpret_cast<LPARAM>(l_pData->GetResultsWnd()))
		{
			TabCtrl_DeleteItem(l_pData->m_hWndTab, l_nTab);
		}
	}

	for (short l_nSerie = E_SR_1; l_nSerie <= (l_pData->GetGameData().GetSerie()); l_nSerie++)
	{
		AddResultTab(a_hWnd, static_cast<T_SERIE>(l_nSerie));
	}
	
	// correct window title
	SetTitle(a_hWnd);
}


void OnGameNew2(HWND a_hWnd)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	SetGameTab(a_hWnd);
	GameWnd_NewDeal2(l_pData->GetGameWnd());
}


void OnGameNew(HWND a_hWnd)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	if (l_pData->GetGameData().GetGame() != E_GM_NULL)
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
	TCHAR l_sFile[MAX_PATH];
	if (!GetOpenSaveFile(a_hWnd, false, l_sFile, ArraySize(l_sFile)))
	{
		return;
	}
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	l_pData->GetGameData().Save(l_sFile);
}


void OnGameOpen(HWND a_hWnd)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	HeartsGame& l_gameData = l_pData->GetGameData();
	if (l_gameData.GetGame() != E_GM_NULL && !l_gameData.IsGameSaved())
	{
		if (DecisionBox(a_hWnd, IDS_SAVEGAMEBEFOREOPENQUESTION))
		{
			SaveGame(a_hWnd);
		}
	}


	TCHAR l_sFile[MAX_PATH] = { 0 };
	if (!GetOpenSaveFile(a_hWnd, true, l_sFile, ArraySize(l_sFile)))
	{
		return;
	}
	l_gameData.Restore(l_sFile);
	StartGame(a_hWnd, true);
}


void OnGameOptions(HWND a_hWnd)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);

	CRegData& l_reg = l_pData->m_regData;
	if (OptionsDlg_DoModal(a_hWnd, &l_reg, l_pData->m_langManager) == IDCANCEL)
	{
		return;
	}
	l_reg.Serialize();

	CFontFactory::Instance().SetFontStyle(l_reg.m_regView.m_bFancyStyle ? FontStyle::Fancy : FontStyle::Normal);

	// TODO replace with for loop

	SetFont(a_hWnd);
	std::ranges::for_each(l_pData->m_mapTabs, [](const auto& l_pair) {
		RefreshTab(l_pair.second);
		});

	l_pData->m_statusbar.Refresh();
	::RedrawWindow(GetCurrentTab(a_hWnd), NULL, NULL, 
		RDW_INVALIDATE | RDW_ERASE | RDW_ERASENOW | RDW_UPDATENOW);
}


void OnAppNextSerie(HWND a_hWnd)
{
	NextSerie(a_hWnd);
}


void OnAppGameOver(HWND a_hWnd)
{
	// wyświetlenie wyników i ewentualne rozdanie nowej serii
	SetResultsTab(a_hWnd, E_SR_4);
	if (DecisionBox(a_hWnd, IDS_PROMPT_NEWGAME))
	{
		CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
		l_pData->GetGameData().GameOver();
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
	::SetWindowText(a_hWnd, l_pData->GetGameData().CreateTitle().c_str());
}


void AddResultTab(HWND a_hWnd, T_SERIE a_enSerie)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	TCHAR l_sTitle[128] { };

	TCITEM l_tabItem {};
	l_tabItem.mask = TCIF_TEXT | TCIF_PARAM;

	::LoadString(::GetModuleHandle(NULL), IDS_TABTITLE_RESULT, l_sTitle, ArraySize(l_sTitle));
	_sntprintf_s(l_sTitle, ArraySize(l_sTitle), _TRUNCATE, tstring(l_sTitle).c_str(), static_cast<int>(a_enSerie) + 1);
	l_tabItem.pszText = l_sTitle;
	l_tabItem.cchTextMax = static_cast<int>(_tcslen(l_sTitle));
	l_tabItem.lParam = reinterpret_cast<LPARAM>(l_pData->GetResultsWnd());

	// insert at the end
	TabCtrl_InsertItem(l_pData->m_hWndTab, TabCtrl_GetItemCount(l_pData->m_hWndTab), &l_tabItem);
}


void SetStatusBarText(
	HWND a_hWnd,	//IN window
	UINT a_idStr	//IN string id to display
)
{
	TCHAR l_sText[512]{};
	::LoadString(::GetModuleHandle(NULL), a_idStr, l_sText, ArraySize(l_sText));
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	l_pData->m_statusbar.SetLabelText(l_sText);
}


void SetResultsTab(HWND a_hWnd, T_SERIE a_enSerie)
{
	SetInnerTab(a_hWnd, CHeartsData::GetData(a_hWnd)->GetResultsWnd(), a_enSerie);
}


void SetGameTab(HWND a_hWnd)
{
	SetInnerTab(a_hWnd, CHeartsData::GetData(a_hWnd)->GetGameWnd(), E_SR_NULL);
}

void SetHelpTab(HWND a_hWnd)
{
	SetInnerTab(a_hWnd, CHeartsData::GetData(a_hWnd)->GetHelpWnd(), E_SR_NULL);
}


void SetInnerTab(HWND a_hWnd, HWND a_hWndInnerTab, T_SERIE a_enSerie)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	short l_iTabCount = TabCtrl_GetItemCount(l_pData->m_hWndTab);
	// find Game tab 
	for (short l_iTab = 0; l_iTab < l_iTabCount; l_iTab++)
	{
		TCITEM l_tabItem{};
		l_tabItem.mask = TCIF_PARAM;
		TabCtrl_GetItem(l_pData->m_hWndTab, l_iTab, &l_tabItem);
		if (l_tabItem.lParam == reinterpret_cast<LPARAM>(a_hWndInnerTab))
		{
			TabCtrl_SetCurSel(l_pData->m_hWndTab, l_iTab);
			SetActiveTab(a_hWnd, a_hWndInnerTab, a_enSerie);
			return;
		}
	}
}


// ---------------------------------------------------------
// calculation of main window size
//
void CalculateWinSize(HWND a_hWnd, LPSIZE a_pSize)
{
	const CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);

	RECT l_rectTab;
	::SetRect(&l_rectTab, 0, 0, C_VIEW_WIDTH, C_VIEW_HEIGHT);
	TabCtrl_AdjustRect(l_pData->m_hWndTab, TRUE, &l_rectTab);
	RECT l_rectSB;

	l_pData->m_statusbar.GetWindowRect(&l_rectSB);

	RECT l_rectAdjusted = { 0, 0, RectWidth(l_rectTab), RectHeight(l_rectTab) + RectHeight(l_rectSB) + l_pData->m_toolbar.GetExtent() };
	::AdjustWindowRectEx(&l_rectAdjusted, WS_OVERLAPPEDWINDOW, FALSE, 0);

	a_pSize->cx = RectWidth(l_rectAdjusted);
	a_pSize->cy = RectHeight(l_rectAdjusted);
}


void SetActiveTab(HWND a_hWnd, HWND a_hWndInnerTab, T_SERIE a_enSerie)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);

	HWND l_hWndResult = l_pData->GetResultsWnd();
	if ((a_hWndInnerTab == l_hWndResult) && (a_enSerie != E_SR_NULL))
	{
		ResultWnd_SetSerie(l_hWndResult, static_cast<T_SERIE>(a_enSerie));
	}

	for (const auto& l_pair : l_pData->m_mapTabs) 
	{
		HWND l_hWnd = l_pair.second;

		if (l_hWnd == a_hWndInnerTab)
		{
			::ShowWindow(l_hWnd, SW_SHOW);
			::SetFocus(l_hWnd);
		}
		else
		{
			::ShowWindow(l_hWnd, SW_HIDE);
		}

	}

	RECT l_rect;
	::GetWindowRect(a_hWnd, &l_rect);
	ResizeTab(a_hWnd);

	// set value for brightness slider
	BOOL l_bSet = FALSE;
	BYTE l_btBrigthness = static_cast<BYTE>(::SendMessage(a_hWndInnerTab, WM_APP_GETBRIGHTNESS, 0, reinterpret_cast<LPARAM>(&l_bSet)));
	if (l_bSet)
		l_pData->m_statusbar.SetSliderValue(cc_btSliderMax - l_btBrigthness);

}


bool GetOpenSaveFile(HWND a_hWnd, bool a_bOpen, LPTSTR a_psFile, DWORD a_iSizeFile)
{
	OPENFILENAME l_ofn = { 0 };
	l_ofn.lStructSize = sizeof(l_ofn);
	l_ofn.hwndOwner = a_hWnd;

	TCHAR l_sFilterBuf[512] = {};
	TCHAR l_sFilterExtBuf[256] = {};

	::LoadString(::GetModuleHandle(NULL), IDS_KIERKIFILTERNAME, l_sFilterBuf, ArraySize(l_sFilterBuf));
	::LoadString(::GetModuleHandle(NULL), IDS_KIERKIFILTEREXT, l_sFilterExtBuf, ArraySize(l_sFilterExtBuf));

	// prepare filter string - it’s a double-null-terminated list of null-terminated string. Just easier using tstring
	tstring l_sFilter = l_sFilterBuf;
	l_sFilter = l_sFilter + _T(" (") + cc_cAsterisk + l_sFilterExtBuf + _T(')');
	
	l_sFilter.push_back(_T('\0'));
	l_sFilter += cc_cAsterisk;
	l_sFilter += l_sFilterExtBuf;
	l_sFilter.push_back(_T('\0'));
	l_sFilter.push_back(_T('\0'));

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


// TODO currently not used - but could be useful if we want to disable save menu when no game is active
void EnableSaveMenu(HWND a_hWnd, BOOL a_bEnable)
{
	HMENU l_hMenuMain = ::GetMenu(a_hWnd);
	HMENU l_hMenuGame = ::GetSubMenu(l_hMenuMain, 0);

	MENUITEMINFO l_mii = {};
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
	if (!l_pData->GetGameData().IsFile())
	{
		TCHAR l_sFileBuf[MAX_PATH];
		if (!GetOpenSaveFile(a_hWnd, false, l_sFileBuf, ArraySize(l_sFileBuf)))
		{
			return;
		}
		l_sFile = l_sFileBuf;
	}
	else
	{
		l_sFile = l_pData->GetGameData().GetFile();
	}
	l_pData->GetGameData().Save(l_sFile.c_str());
}


void StartGame(HWND a_hWnd, bool a_bOpen)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	SetGameTab(a_hWnd);
	if (a_bOpen)
	{
		l_pData->GetGameData().NextGame(FALSE);
	}
	else
	{
		l_pData->GetGameData().NewGame();
	}
	NextSerie(a_hWnd);
	GameWnd_NewDeal(l_pData->GetGameWnd(), a_bOpen);
}

void SetFont(HWND a_hWnd)
{
	CHeartsData* l_pData = CHeartsData::GetData(a_hWnd);
	HFONT l_hFont = CFontFactory::Instance().GetFont(l_pData->m_hWndTab);
	::SendMessage(l_pData->m_hWndTab, WM_SETFONT, reinterpret_cast<WPARAM>(l_hFont), 0);
}
