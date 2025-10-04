#include "StdAfx.h"
#include "HelpWnd.h"
#include "Kierki.h"
#include "resource.h"
#include "FontFactory.h"
#include <rcommon/RSystemExc.h>
#include <rcommon/ROwnExc.h>
#include <rcommon/RMemDC.h>
#include <rcommon/drawutl.h>
#include <richedit.h>
#include <algorithm>
#include <CommCtrl.h>
#include <wininet.h>
#include <fstream>
#include <sstream>
#include <string>
#include <external/json.hpp> // Include the JSON library
#include "HelpJson.h"
#pragma comment(lib, "wininet.lib")



constexpr TCHAR cc_sWindowClass[] = _T("HELPWND");	// game window class name
constexpr int cc_dxPanelGap = 20;		// gap before checkbox to show instruction
constexpr int cc_dxPanelCtrlGap = 2;		// gap between controls
constexpr int cc_dyLabelGap = 4;		// gap for label to be "in line" with other controls

class HelpWndData
{
public:

	// Constructor that initializes m_lang
	HelpWndData(const LanguageManager& langManager) : m_lang(langManager) {};

	static HelpWndData* GetData(HWND a_hWnd) { return reinterpret_cast<HelpWndData*>(::GetWindowLongPtr(a_hWnd, GWLP_USERDATA)); };
	static void SetData(HWND a_hWnd, HelpWndData* a_pData) { ::SetWindowLongPtr(a_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(a_pData)); };

	HWND m_hWndRich = nullptr;
	HWND m_hWndPanel = nullptr;
	const LanguageManager& m_lang;
	CHelpJson	m_jsonHelp;
	CRegData* m_pRegData = nullptr;	
	HBITMAP m_hBmpBackground{};

};


static LRESULT CALLBACK	HelpWnd_WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK Panel_WndProc(HWND a_hDlg, UINT a_iMsg, WPARAM a_wParam, LPARAM a_lParam);

inline static LRESULT OnNCCreate(HWND a_hWnd, LPCREATESTRUCT a_pCreateStruct);
inline static LRESULT OnCreate(HWND a_hWnd);
inline static void OnNcDestroy(HWND a_hWnd);
inline static void OnSize(HWND a_hWnd, int a_dxWidth, int a_dyHeight);
inline static void OnPaint(HWND a_hWnd);
inline static void OnCommand(HWND a_hWnd, WPARAM a_wParam, LPARAM a_lParam);
inline static void OnSelChange(HWND a_hWnd);

inline static void OnAppRefresh(HWND a_hWnd);
inline static void OnSetBrightness(HWND a_hWnd, BYTE a_btBrightness);
inline static BYTE OnGetBrightness(HWND a_hWnd, BOOL* a_pSet);

static BOOL OnInitDialog(HWND a_hDlg, HelpWndData* a_pData);

static std::string GetSelectedSection(HWND a_hWnd);

static void LoadInstructions(HWND a_hWndHelp , const std::string& a_sSection);
void LoadInstructionsAndRedraw(HWND a_hWndHelp, const std::string& a_sSection);
BOOL SetSection(HWND a_hWnd, const std::string& a_sSection);
static void NextSection(HWND a_hWnd, bool a_bNext);
static void ShowHelpChanged(HWND a_hWnd);
void EnableDisableButtons(HWND a_hWnd);

inline static void Draw(HWND a_hWnd, HDC a_hDC);

static void AlignPanelControls(HWND a_hWnd);
static int GetControlTextWidth(HWND a_hWndCtrl);

void SetFont(HWND a_hWnd);

bool IsRtfText(const tstring& a_sText);
void SetRichEditText(HWND a_hWnd, const tstring& a_sText);
void SetRichEditFont(HWND a_hWndRich, HFONT a_hFont);
CHARFORMAT2 CharFormatFromFont(HFONT a_hFont);


BOOL HelpWnd_Register(HINSTANCE a_hInst)
{
	if (::LoadLibrary(_T("Msftedit.dll")) == NULL)
		throw RSystemExc(_T("LOAD_MSFTEDIT_DLL"));

	WNDCLASSEX l_wcex {};

	l_wcex.cbSize = sizeof(WNDCLASSEX);
	l_wcex.lpfnWndProc = HelpWnd_WndProc;
	l_wcex.hInstance = a_hInst;
	l_wcex.lpszClassName = cc_sWindowClass;
	return ::RegisterClassEx(&l_wcex);
}

HWND HelpWnd_Create( DWORD a_dwStyle, HWND a_hWndParent, const LanguageManager& a_lang, CRegData* a_pRegData)
{
	HelpWndData* l_pData = new HelpWndData(a_lang);
	HWND l_hWndHelp = ::CreateWindowEx(0, cc_sWindowClass, NULL, WS_CHILD | WS_CLIPSIBLINGS,
			0, 0, 0, 0, a_hWndParent, NULL, NULL, reinterpret_cast<LPVOID>(l_pData));

	if (l_hWndHelp == NULL)
	{
		delete l_pData;
		throw RSystemExc(_T("CREATE_HELPWND"));
	}

	l_pData->m_pRegData = a_pRegData;
	l_pData->m_jsonHelp.load(a_lang);
	l_pData->m_hWndPanel = ::CreateDialogParam(::GetModuleHandle(nullptr),
		MAKEINTRESOURCEW(IDD_HELP), l_hWndHelp, Panel_WndProc, reinterpret_cast<LPARAM>(l_pData));

	l_pData->m_hWndRich = ::CreateWindowEx(WS_EX_TRANSPARENT | WS_EX_CLIENTEDGE, MSFTEDIT_CLASS, _T(""),
		a_dwStyle | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY | WS_VSCROLL | WS_VISIBLE ,
		0, 0, 0, 0, l_hWndHelp, NULL, nullptr, NULL);
	if (l_pData->m_hWndRich == NULL)
		throw RSystemExc(_T("CREATE_HWNDHELP_RICHEDIT"));


	l_pData->m_hBmpBackground = (HBITMAP)::LoadImage(
		::GetModuleHandle(nullptr),
		MAKEINTRESOURCE(IDB_PAPER), // Use full path or resource ID
		IMAGE_BITMAP,
		0, 0, LR_CREATEDIBSECTION
	);
	if (l_pData->m_hBmpBackground == nullptr)
		throw RSystemExc(_T("LOAD_BACKGROUND_HELP"));

	SetFont(l_hWndHelp);

	return l_hWndHelp;
}


LRESULT CALLBACK HelpWnd_WndProc(HWND a_hWnd, UINT a_iMsg, WPARAM a_wParam, LPARAM a_lParam)
{
	try
	{
		switch (a_iMsg)
		{
		case WM_NCCREATE:
			return OnNCCreate(a_hWnd, reinterpret_cast<LPCREATESTRUCT>(a_lParam));

		case WM_CREATE:
			return OnCreate(a_hWnd);

		case WM_SIZE:
			OnSize(a_hWnd, LOWORD(a_lParam), HIWORD(a_lParam));
			break;

		case WM_NCDESTROY:
			OnNcDestroy(a_hWnd);
			break;

		case WM_COMMAND:
			OnCommand(a_hWnd, a_wParam, a_lParam);
			break;

		case WM_PAINT:
			OnPaint(a_hWnd);
			break;

		case WM_APP_REFRESH:
			OnAppRefresh(a_hWnd);
			break;

		case WM_APP_SETBRIGHTNESS:
			OnSetBrightness(a_hWnd, static_cast<BYTE>(a_wParam));
			break;

		case WM_APP_GETBRIGHTNESS:
			return OnGetBrightness(a_hWnd, reinterpret_cast<BOOL*>(a_lParam));

		default:
			return ::DefWindowProc(a_hWnd, a_iMsg, a_wParam, a_lParam);
		}
	}
	catch (const RSystemExc& l_exc) 
	{
		return 0; // abort safely
	}
	catch (const ROwnExc& l_exc) 
	{
		return 0;
	}
	catch (...)
	{
	}

	return 0;
}


INT_PTR CALLBACK Panel_WndProc(HWND a_hDlg, UINT a_iMsg, WPARAM a_wParam, LPARAM a_lParam)
{
	switch (a_iMsg)
	{
	case WM_INITDIALOG:
		return OnInitDialog(a_hDlg, reinterpret_cast<HelpWndData*>(a_lParam));

	case WM_COMMAND:
		::SendMessage(::GetParent(a_hDlg), WM_COMMAND, a_wParam, a_lParam);
		return TRUE;
	}
	return FALSE;
}

//
// Store app data in window structure
//
LRESULT OnNCCreate(HWND a_hWnd, LPCREATESTRUCT a_pCreateStruct)
{
	// store app data in window structure
	HelpWndData* l_pData = reinterpret_cast<HelpWndData*>(a_pCreateStruct->lpCreateParams);
	if (l_pData == NULL)
	{
		return FALSE;
	}

	HelpWndData::SetData(a_hWnd, l_pData);

	return TRUE;
}


//
// load instructions from GitHub repository or resources
//
LRESULT OnCreate(HWND a_hWnd)
{
	// currently placeholder for future implementation (maybe I will move the logic of instruction's loading here)
	return 0;
}



void OnNcDestroy(HWND a_hWnd)
{
	HelpWndData* l_pData = HelpWndData::GetData(a_hWnd);
	delete l_pData;
}


void OnSize(HWND a_hWnd, int a_dxWidth, int a_dyHeight)
{
	HelpWndData* l_pData = HelpWndData::GetData(a_hWnd);

	if (l_pData == NULL)
		return;

	// here it is possible I don't have yet other windows created, as I am creating them after OnCreate
	// as it is easier 
	if ((l_pData->m_hWndPanel == nullptr) || (l_pData->m_hWndRich == nullptr))
		return;

	HDWP l_hdwp = ::BeginDeferWindowPos(2);

	if (l_hdwp == NULL)
		throw RSystemExc(_T("HELPWND:BEGIN_DEFER_WINDOW_POS"));
	
	l_hdwp = ::DeferWindowPos(l_hdwp, l_pData->m_hWndPanel, NULL, 0, 0, a_dxWidth, 36, SWP_NOZORDER);
	if (l_hdwp == NULL)
		throw RSystemExc(_T("HELPWND:DEFER_WINDOW_POS_DIALOG"));

	l_hdwp = ::DeferWindowPos(l_hdwp, l_pData->m_hWndRich, NULL, 0, 36, a_dxWidth, a_dyHeight - 36, SWP_NOZORDER);
	if (l_hdwp == NULL)
		throw RSystemExc(_T("HELPWND:DEFER_WINDOW_POS_RICHEDIT"));

	if (!::EndDeferWindowPos(l_hdwp))
		throw RSystemExc(_T("HELPWND:END_DEFER_WINDOW_POS"));

#pragma todo("Check how to do it in Defer batch above ")
	AlignPanelControls(a_hWnd);
}


// ---------------------------------------------------------
// Painting
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



void OnAppRefresh(HWND a_hWnd)
{
	HelpWndData* l_pData = HelpWndData::GetData(a_hWnd);
	SetFont(a_hWnd);
	::RedrawWindow(l_pData->m_hWndRich, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW);
}


void OnSetBrightness(HWND a_hWnd, BYTE a_btBrightness)
{
	HelpWndData* l_pData = HelpWndData::GetData(a_hWnd);
	l_pData->m_pRegData->m_regAuto.m_btAlphaHelpBackground = a_btBrightness;
	::RedrawWindow(a_hWnd, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW);
}


BYTE OnGetBrightness(HWND a_hWnd, BOOL* a_pSet)
{
	HelpWndData* l_pData = HelpWndData::GetData(a_hWnd);
	if (l_pData == nullptr)
		return 0;
	*a_pSet = TRUE;
	return l_pData->m_pRegData->m_regAuto.m_btAlphaHelpBackground;

}


void Draw(HWND a_hWnd, HDC a_hDC)
{
	HelpWndData* l_pData = HelpWndData::GetData(a_hWnd);

	RECT l_rect;
	::GetClientRect(a_hWnd, &l_rect);

	if (l_pData->m_hBmpBackground)
	{
		HDC l_hdcMem = ::CreateCompatibleDC(a_hDC);
		HBITMAP hOldBmp = (HBITMAP)::SelectObject(l_hdcMem, l_pData->m_hBmpBackground);

		BITMAP l_bmp;
		::GetObject(l_pData->m_hBmpBackground, sizeof(BITMAP), &l_bmp);

		::StretchBlt(a_hDC, 0, 0, l_rect.right, l_rect.bottom,
			l_hdcMem, 0, 0, l_bmp.bmWidth, l_bmp.bmHeight, SRCCOPY);

		::SelectObject(l_hdcMem, hOldBmp);
		::DeleteDC(l_hdcMem);
	}

	RDraw::BlendOverlay(a_hDC, l_rect, l_pData->m_pRegData->m_regHidden.m_clrTintHelpBackground, l_pData->m_pRegData->m_regAuto.m_btAlphaHelpBackground);

}

 

static void OnCommand(HWND a_hWnd, WPARAM a_wParam, LPARAM a_lParam)
{
	switch (LOWORD(a_wParam))
	{
	case IDC_HELP_SECTION:
		if (HIWORD(a_wParam) == CBN_SELCHANGE)
			OnSelChange(a_hWnd);
		break;

	case IDC_HELP_LEFT:
		if (HIWORD(a_wParam) == BN_CLICKED)
			NextSection(a_hWnd, false);
		break;

	case IDC_HELP_RIGHT:
		if (HIWORD(a_wParam) == BN_CLICKED)
			NextSection(a_hWnd, true);
		break;

	case IDC_HELP_SHOWHELP:
		if (HIWORD(a_wParam) == BN_CLICKED)
			ShowHelpChanged(a_hWnd);
		break;
	}

}

BOOL OnInitDialog(HWND a_hDlg, HelpWndData* a_pData)
{
	HWND l_hWndSection = ::GetDlgItem(a_hDlg, IDC_HELP_SECTION);
	auto l_vect = a_pData->m_jsonHelp.getSectionTitles();	
	int l_iSize = l_vect.size();

	for (int l_iAt = 0; l_iAt < l_iSize; l_iAt++)
	{
		int l_idx = ::SendMessage(l_hWndSection, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(l_vect[l_iAt].second.c_str()));
		::SendMessage(l_hWndSection, CB_SETITEMDATA, l_idx, static_cast<LPARAM>(l_iAt)); // Store index
	}

	::SendMessage(::GetDlgItem(a_hDlg, IDC_HELP_SHOWHELP), BM_SETCHECK, BST_UNCHECKED, 0);

	return TRUE;
}


inline static void OnSelChange(HWND a_hWnd)
{
	std::string l_sSection = GetSelectedSection(a_hWnd);
	LoadInstructionsAndRedraw(a_hWnd, l_sSection);
	EnableDisableButtons(a_hWnd);
}


BOOL SetSection(HWND a_hWnd, const std::string& a_sSection)
{
	HelpWndData* l_pData = HelpWndData::GetData(a_hWnd);

	HWND l_hWndSection = ::GetDlgItem(l_pData->m_hWndPanel, IDC_HELP_SECTION);
	auto l_vect = l_pData->m_jsonHelp.getSectionTitles();
	int l_iSize = l_vect.size();

	for (int l_iAt = 0; l_iAt < l_iSize; l_iAt++)
	{
		if (l_vect[l_iAt].first == a_sSection)
		{
			::SendMessage(l_hWndSection, CB_SETCURSEL, l_iAt, 0);
			EnableDisableButtons(a_hWnd);
			return TRUE;
		}
	}
	return FALSE;
}

// Helper: align dialog controls to be at the middle (vertically) of the Panel
// must be run after SetFont
//
void AlignPanelControls(HWND a_hWnd)
{
	HelpWndData* l_pData = HelpWndData::GetData(a_hWnd);


	// TODO do it in DeferWindowPos batch

	// controls, unfortunately we need to do all resizing manually
	HWND l_hWndLabelSect = ::GetDlgItem(l_pData->m_hWndPanel, IDC_HELP_SECTIONLABEL);
	HWND l_hWndLeft = ::GetDlgItem(l_pData->m_hWndPanel, IDC_HELP_LEFT);
	HWND l_hWndSect = ::GetDlgItem(l_pData->m_hWndPanel, IDC_HELP_SECTION);
	HWND l_hWndRight = ::GetDlgItem(l_pData->m_hWndPanel, IDC_HELP_RIGHT);
	HWND l_hWndCheck = ::GetDlgItem(l_pData->m_hWndPanel, IDC_HELP_SHOWHELP);

	// combobox changes the height after the change of font
	RECT l_rectSection{};
	::GetWindowRect(l_hWndSect, &l_rectSection);
	int l_dyMax = RectHeight(l_rectSection);

	int l_dxLabelSect = GetControlTextWidth(l_hWndLabelSect);
	int l_dxCheck = GetControlTextWidth(l_hWndCheck) + GetSystemMetrics(SM_CXMENUCHECK) + 6;

	RECT l_rectPanel{};
	::GetClientRect(l_pData->m_hWndPanel, &l_rectPanel);
	int l_dyPos = (RectHeight(l_rectPanel) - l_dyMax) / 2;

	int l_xNext = 2;
	::SetWindowPos(l_hWndLabelSect, NULL, l_xNext, l_dyPos + cc_dyLabelGap,		// label needs special treatments to be "inline", hence +4
		l_dxLabelSect + cc_dxPanelCtrlGap, l_dyMax,
		SWP_NOZORDER | SWP_NOACTIVATE);

	l_xNext = l_xNext + l_dxLabelSect + cc_dxPanelCtrlGap;

	int l_dxBtn = l_dyMax * 2;
	::SetWindowPos(l_hWndLeft, NULL, l_xNext, l_dyPos,
		l_dxBtn, l_dyMax,
		SWP_NOZORDER | SWP_NOACTIVATE);

	l_xNext = l_xNext + l_dxBtn;
	int l_dxSection = RectWidth(l_rectPanel) - (cc_dxPanelCtrlGap + l_dxLabelSect + cc_dxPanelCtrlGap + l_dxBtn + l_dxBtn + cc_dxPanelGap + l_dxCheck + cc_dxPanelCtrlGap);
	::SetWindowPos(l_hWndSect, NULL, l_xNext, l_dyPos,
		l_dxSection, l_dyMax,
		SWP_NOZORDER | SWP_NOACTIVATE);

	l_xNext = l_xNext + l_dxSection;
	::SetWindowPos(l_hWndRight, NULL, l_xNext, l_dyPos,
		l_dxBtn, l_dyMax,
		SWP_NOZORDER | SWP_NOACTIVATE);

	l_xNext = l_xNext + l_dxBtn + cc_dxPanelGap;

	::SetWindowPos(l_hWndCheck, NULL, l_xNext, l_dyPos,
		l_dxCheck, l_dyMax,
		SWP_NOZORDER | SWP_NOACTIVATE);

}



void SetFont(HWND a_hWnd)
{
	HelpWndData* l_pData = HelpWndData::GetData(a_hWnd);
	HFONT l_hFont = CFontFactory::Instance().GetFont(a_hWnd);

	SetRichEditFont(l_pData->m_hWndRich, l_hFont);

	::SendMessage(l_pData->m_hWndPanel, WM_SETFONT, reinterpret_cast<WPARAM>(l_hFont), TRUE);
	HWND l_hWndChild = ::GetWindow(l_pData->m_hWndPanel, GW_CHILD);
	while (l_hWndChild)
	{
		::SendMessage(l_hWndChild, WM_SETFONT, reinterpret_cast<WPARAM>(l_hFont), TRUE);
		l_hWndChild = ::GetWindow(l_hWndChild, GW_HWNDNEXT);
	}

	AlignPanelControls(a_hWnd);
	LoadInstructions(a_hWnd, GetSelectedSection(a_hWnd));
	
}


void SetRichEditFont(HWND a_hWndRich, HFONT a_hFont)
{
	::SendMessage(a_hWndRich, WM_SETFONT, reinterpret_cast<WPARAM>(a_hFont), TRUE);

	CHARFORMAT2 l_cf = CharFormatFromFont(a_hFont);
	::SendMessage(a_hWndRich, EM_SETSEL, 0, -1); // Select all
	::SendMessage(a_hWndRich, EM_SETCHARFORMAT, SCF_ALL, reinterpret_cast<LPARAM>(& l_cf));
}

void HelpWnd_LoadInstructions(HWND a_hWndHelp, const std::string& a_sSection)
{
	HelpWndData* l_pData = HelpWndData::GetData(a_hWndHelp);
	SetSection(a_hWndHelp, a_sSection);
	LoadInstructionsAndRedraw(a_hWndHelp, a_sSection);
}


void LoadInstructionsAndRedraw(HWND a_hWndHelp, const std::string& a_sSection)
{
	LoadInstructions(a_hWndHelp, a_sSection);
	HelpWndData* l_pData = HelpWndData::GetData(a_hWndHelp);
	::RedrawWindow(l_pData->m_hWndRich, nullptr, nullptr, RDW_ERASENOW | RDW_INVALIDATE | RDW_UPDATENOW);
}


void LoadInstructions(HWND a_hWndHelp, const std::string& a_sSection)
{
	HelpWndData* l_pData = HelpWndData::GetData(a_hWndHelp);
	auto l_vect = l_pData->m_jsonHelp.getInstructions(a_sSection);
	tstring l_sInstructions;

	for (const auto& l_sLine : l_vect)
		l_sInstructions += l_sLine + _T('\n');

	SetRichEditText(a_hWndHelp, l_sInstructions);
}


static void NextSection(HWND a_hWnd, bool a_bNext)
{
	HelpWndData* l_pData = HelpWndData::GetData(a_hWnd);
	HWND l_hWndSection = ::GetDlgItem(l_pData->m_hWndPanel, IDC_HELP_SECTION);

	int l_idx = ::SendMessage(l_hWndSection, CB_GETCURSEL, 0, 0);

	auto l_vect = l_pData->m_jsonHelp.getSectionTitles();
	int l_iSize = l_vect.size();

	if (a_bNext) // iteration forward
	{
		if (l_idx < l_iSize - 1)
		{
			::SendMessage(l_hWndSection, CB_SETCURSEL, ++l_idx, 0);
			LoadInstructionsAndRedraw(a_hWnd, l_vect[l_idx].first);
			EnableDisableButtons(a_hWnd);
		}
	}
	else
	{
		if (l_idx > 0)
		{
			::SendMessage(l_hWndSection, CB_SETCURSEL, --l_idx, 0);
			LoadInstructionsAndRedraw(a_hWnd, l_vect[l_idx].first);
			EnableDisableButtons(a_hWnd);
		}

	}

}

static void ShowHelpChanged(HWND a_hWnd)
{
	HelpWndData* l_pData = HelpWndData::GetData(a_hWnd);
	HWND l_hWndShowHelp = ::GetDlgItem(l_pData->m_hWndPanel, IDC_HELP_SHOWHELP);
	BOOL l_bChecked = (::SendMessage(l_hWndShowHelp, BM_GETCHECK, 0, 0) == BST_CHECKED);
	if (l_bChecked)
	{
		l_pData->m_pRegData->m_regRules.m_bHelpVisible = false;
		l_pData->m_pRegData->m_regRules.Serialize();
	}

}


void EnableDisableButtons(HWND a_hWnd)
{
	HelpWndData* l_pData = HelpWndData::GetData(a_hWnd);
	HWND l_hWndSection = ::GetDlgItem(l_pData->m_hWndPanel, IDC_HELP_SECTION);
	int l_idx = ::SendMessage(l_hWndSection, CB_GETCURSEL, 0, 0);
	auto l_vect = l_pData->m_jsonHelp.getSectionTitles();
	int l_iSize = l_vect.size();
	HWND l_hWndLeft = ::GetDlgItem(l_pData->m_hWndPanel, IDC_HELP_LEFT);
	HWND l_hWndRight = ::GetDlgItem(l_pData->m_hWndPanel, IDC_HELP_RIGHT);
	::EnableWindow(l_hWndLeft, (l_idx > 0));
	::EnableWindow(l_hWndRight, (l_idx < l_iSize - 1));
}

bool IsRtfText(const tstring& a_sText)
{
	return a_sText.substr(0, 5) == _T("{\\rtf");
}


void SetRichEditText(HWND a_hWnd, const tstring& a_sText)
{
	HelpWndData* l_pData = HelpWndData::GetData(a_hWnd);
	if (!IsRtfText(a_sText))
	{
		SetWindowText(l_pData->m_hWndRich, a_sText.c_str());
		return;
	}

	EDITSTREAM l_es = {};
	l_es.dwCookie = reinterpret_cast<DWORD_PTR>(&a_sText);
	l_es.pfnCallback = [](DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG* pcb) -> DWORD {
		tstring* pText = reinterpret_cast<tstring*>(dwCookie);

		// Defensive: if cb < 0, treat as 0
		size_t requested = (cb > 0) ? static_cast<size_t>(cb) : 0;
		size_t available = pText->size() * sizeof(TCHAR);

		size_t bytesToCopy = std::min(requested, available);

		if (bytesToCopy > 0) {
			memcpy(pbBuff, pText->c_str(), bytesToCopy);
			pText->erase(0, bytesToCopy / sizeof(TCHAR));
		}

		*pcb = static_cast<LONG>(bytesToCopy);
		return 0;
		};
	SendMessage(l_pData->m_hWndRich, EM_STREAMIN, SF_RTF, reinterpret_cast<LPARAM>(&l_es));

}


int GetControlTextWidth(HWND a_hWndCtrl)
{

	// --- Get checkbox text ---
	TCHAR l_sText[256];
	::GetWindowText(a_hWndCtrl, l_sText, ArraySize(l_sText));

	// --- Measure text ---
	HDC l_hdc = ::GetDC(a_hWndCtrl);
	HFONT l_hFont = reinterpret_cast<HFONT>(::SendMessage(a_hWndCtrl, WM_GETFONT, 0, 0));

	HFONT l_hFontOld = (HFONT)::SelectObject(l_hdc, l_hFont);

	SIZE l_size{};
	::GetTextExtentPoint32(l_hdc, l_sText, _tcslen(l_sText), &l_size);

	::SelectObject(l_hdc, l_hFontOld);
	::ReleaseDC(a_hWndCtrl, l_hdc);
	return l_size.cx;
}



std::string GetSelectedSection(HWND a_hWnd)
{
	HelpWndData* l_pData = HelpWndData::GetData(a_hWnd);
	HWND l_hWndSection = ::GetDlgItem(l_pData->m_hWndPanel, IDC_HELP_SECTION);
	int l_iSel = ComboBox_GetCurSel(l_hWndSection);
	if (l_iSel == CB_ERR)
		return "";
	int l_iAt = static_cast<int>(::SendMessage(l_hWndSection, CB_GETITEMDATA, l_iSel, 0));
	if (l_iAt == CB_ERR)
		return "";

	auto l_vect = l_pData->m_jsonHelp.getSectionTitles();
	return l_vect[l_iAt].first;
}

CHARFORMAT2 CharFormatFromFont(HFONT a_hFont)
{
	LOGFONT l_lf;
	::GetObject(a_hFont, sizeof(l_lf), &l_lf);

	CHARFORMAT2 l_cf{};
	l_cf.cbSize = sizeof(l_cf);

	// Font face name
	if (l_lf.lfFaceName[0])
	{
		_tcscpy_s(l_cf.szFaceName, l_lf.lfFaceName);
		l_cf.dwMask |= CFM_FACE;
	}

	// Font size in twips (1 pt = 20 twips)
	if (l_lf.lfHeight != 0)
	{
		l_cf.yHeight = abs(l_lf.lfHeight) * 20;
		l_cf.dwMask |= CFM_SIZE;
	}

	// Bold
	if (l_lf.lfWeight >= FW_BOLD)
	{
		l_cf.dwEffects |= CFE_BOLD;
		l_cf.dwMask |= CFM_BOLD;
	}

	// Italic
	if (l_lf.lfItalic)
	{
		l_cf.dwEffects |= CFE_ITALIC;
		l_cf.dwMask |= CFM_ITALIC;
	}

	// Underline
	if (l_lf.lfUnderline)
	{
		l_cf.dwEffects |= CFE_UNDERLINE;
		l_cf.dwMask |= CFM_UNDERLINE;
	}

	// Strikeout
	if (l_lf.lfStrikeOut)
	{
		l_cf.dwEffects |= CFE_STRIKEOUT;
		l_cf.dwMask |= CFM_STRIKEOUT;
	}

	// Charset
	l_cf.bCharSet = l_lf.lfCharSet;
	l_cf.dwMask |= CFM_CHARSET;

	return l_cf;
}
