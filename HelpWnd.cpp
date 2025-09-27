#include "StdAfx.h"
#include "HelpWnd.h"
#include "resource.h"
#include <rcommon/RSystemExc.h>
#include <rcommon/ROwnExc.h>
#include <rcommon/RMemDC.h>
#include <rcommon/drawutl.h>
#include <richedit.h>
#include <CommCtrl.h>
#include <wininet.h>
#include <fstream>
#include <sstream>
#include <string>
#include <external/json.hpp> // Include the JSON library
#include "HelpJson.h"
#pragma comment(lib, "wininet.lib")





class HelpWndData
{
public:

	// Constructor that initializes m_lang
	HelpWndData(const LanguageManager& langManager) : m_lang(langManager) {};

	static HelpWndData* GetData(HWND a_hWnd) { return reinterpret_cast<HelpWndData*>(::GetWindowLongPtr(a_hWnd, GWLP_USERDATA)); };
	static void SetData(HWND a_hWnd, HelpWndData* a_pData) { ::SetWindowLongPtr(a_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(a_pData)); };

	HWND m_hWndRich = nullptr;
	HWND m_hWndPanel = nullptr;
	HFONT m_hFont = nullptr;
	const LanguageManager& m_lang;
	HelpJson	m_jsonHelp;
	RRegData* m_pRegData = nullptr;	
	HBITMAP m_hBmpBackground{};

};


constexpr TCHAR c_sWindowClass[] = _T("HELPWND");	// game window class name

static LRESULT CALLBACK	HelpWnd_WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK Panel_WndProc(HWND a_hDlg, UINT a_iMsg, WPARAM a_wParam, LPARAM a_lParam);

inline static LRESULT OnNCCreate(HWND a_hWnd, LPCREATESTRUCT a_pCreateStruct);
inline static LRESULT OnCreate(HWND a_hWnd);
inline static void OnNcDestroy(HWND a_hWnd);
inline static void OnSize(HWND a_hWnd, int a_dxWidth, int a_dyHeight);
inline static void OnPaint(HWND a_hWnd);
inline static void OnCommand(HWND a_hWnd, WPARAM a_wParam, LPARAM a_lParam);
inline static void OnSelChange(HWND a_hWnd);

BOOL OnInitDialog(HWND a_hDlg, HelpWndData* a_pData);

static void LoadInstructions(HWND a_hWndHelp , const std::string& a_sSection);
void LoadInstructionsAndRedraw(HWND a_hWndHelp, const std::string& a_sSection);
BOOL SetSection(HWND a_hWnd, const std::string& a_sSection);
static void NextSection(HWND a_hWnd, bool a_bNext);
static void ShowHelpChanged(HWND a_hWnd);
void EnableDisableButtons(HWND a_hWnd);

inline static void Draw(HWND a_hWnd, HDC a_hDC);

static void AlignCheckboxRight(HWND a_hDlg, int a_idCheck, int a_iWidth);




BOOL HelpWnd_Register(HINSTANCE a_hInst)
{

	if (::LoadLibrary(_T("Msftedit.dll")) == NULL)
		throw RSystemExc(_T("LOAD_MSFTEDIT_DLL"));

	WNDCLASSEX l_wcex {};

	l_wcex.cbSize = sizeof(WNDCLASSEX);
	l_wcex.lpfnWndProc = HelpWnd_WndProc;
	l_wcex.hInstance = a_hInst;
	l_wcex.lpszClassName = c_sWindowClass;
	return ::RegisterClassEx(&l_wcex);
}

HWND HelpWnd_Create(DWORD a_dwExStyle, DWORD a_dwStyle, HWND a_hWndParent, const LanguageManager& a_lang, RRegData* a_pRegData)
{
	HelpWndData* l_pData = new HelpWndData(a_lang);
	HWND l_hWndHelp = ::CreateWindowEx(0, c_sWindowClass, NULL, WS_CHILD | WS_CLIPSIBLINGS,
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
		throw RSystemExc(_T("LOAD_BACKGROUND_HLP"));

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

		default:
			return ::DefWindowProc(a_hWnd, a_iMsg, a_wParam, a_lParam);
		}
	}
	catch (const RSystemExc& l_exc) 
	{
		TRACE1("%s\n", l_exc.GetFormattedMsg().c_str());
		return FALSE; // abort safely
	}
	catch (const ROwnExc& l_exc) 
	{
		TRACE1("%s\n", l_exc.GetFormattedMsg().c_str());
		return FALSE;
	}
	catch (...)
	{
		TRACE0("Unknown exception\n");
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


	HDWP l_hdwp = ::BeginDeferWindowPos(2);

	if (l_hdwp == NULL)
		throw RSystemExc(_T("HELPWND:BEGIN_DEFER_WINDOW_POS"));
	
	l_hdwp = ::DeferWindowPos(l_hdwp, l_pData->m_hWndPanel, NULL, 0, 0, a_dxWidth, 30, SWP_NOZORDER);
	if (l_hdwp == NULL)
		throw RSystemExc(_T("HELPWND:DEFER_WINDOW_POS_DIALOG"));

	l_hdwp = ::DeferWindowPos(l_hdwp, l_pData->m_hWndRich, NULL, 0, 30, a_dxWidth, a_dyHeight - 30, SWP_NOZORDER);
	if (l_hdwp == NULL)
		throw RSystemExc(_T("HELPWND:DEFER_WINDOW_POS_RICHEDIT"));

	if (!::EndDeferWindowPos(l_hdwp))
		throw RSystemExc(_T("HELPWND:END_DEFER_WINDOW_POS"));

#pragma todo("Check how to do it in Defer batch above ")
	AlignCheckboxRight(l_pData->m_hWndPanel, IDC_HELP_SHOWHELP, a_dxWidth);
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

	RDraw::BlendOverlay(a_hDC, l_rect, l_pData->m_pRegData->m_regHidden.m_btTintHelpBackground, l_pData->m_pRegData->m_regHidden.m_btAlphaHelpBackground);

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

	HelpWndData* l_pData = HelpWndData::GetData(a_hWnd);
	HWND l_hWndSection = ::GetDlgItem(l_pData->m_hWndPanel, IDC_HELP_SECTION);
	int l_iSel = ComboBox_GetCurSel(l_hWndSection);
	if (l_iSel == CB_ERR)
		return;
	int l_iAt = static_cast<int>(::SendMessage(l_hWndSection, CB_GETITEMDATA, l_iSel, 0));
	if (l_iAt == CB_ERR)
		return;

	auto l_vect = l_pData->m_jsonHelp.getSectionTitles();
	LoadInstructionsAndRedraw(a_hWnd, l_vect[l_iAt].first);
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

// Helper: align a checkbox control to the right edge of the dialog
void AlignCheckboxRight(HWND a_hDlg, int a_idCheck, int a_iWidth)
{
	HWND l_hCheck = GetDlgItem(a_hDlg, a_idCheck);
	if (!l_hCheck)
		return;

	// --- Get font ---
	HFONT l_hFont = (HFONT)SendMessage(a_hDlg, WM_GETFONT, 0, 0);
	if (!l_hFont)
		l_hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

	// --- Get checkbox text ---
	TCHAR l_sText[256];
	GetWindowTextW(l_hCheck, l_sText, ArraySize(l_sText));

	// --- Measure text ---
	HDC l_hdc = GetDC(l_hCheck);
	HFONT l_hFontOld = (HFONT)SelectObject(l_hdc, l_hFont);

	SIZE l_size{};
	::GetTextExtentPoint32(l_hdc, l_sText, _tcslen(l_sText), &l_size);

	SelectObject(l_hdc, l_hFontOld);
	ReleaseDC(l_hCheck, l_hdc);

	// --- Compute total width (box + padding + text) ---
	int l_dxCheck = GetSystemMetrics(SM_CXMENUCHECK);
	int l_dxFull = l_dxCheck + 6 + l_size.cx;


	// --- Get current checkbox rect ---
	RECT l_rectCheck{};
	GetWindowRect(l_hCheck, &l_rectCheck);
	MapWindowPoints(NULL, a_hDlg, (LPPOINT)&l_rectCheck, 2);


	// --- Align to right ---
	int l_xNew = a_iWidth - l_dxFull - 2;

	SetWindowPos(l_hCheck, NULL, l_xNew, l_rectCheck.top,
		l_dxFull, RectHeight(l_rectCheck),
		SWP_NOZORDER | SWP_NOACTIVATE);
}



void HelpWnd_SetFont(HWND a_hWnd, const tstring& a_sFont)
{
	HelpWndData* l_pData = HelpWndData::GetData(a_hWnd);

	if (l_pData->m_hFont != NULL)
	{
		::DeleteObject(l_pData->m_hFont);
		l_pData->m_hFont = NULL;
	}

	l_pData->m_hFont = ::CreateFont(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, a_sFont.c_str());

	if (l_pData->m_hFont == NULL)
		throw RSystemExc(_T("HELPWND:CREATE_FONT"));

	::SendMessage(l_pData->m_hWndRich, WM_SETFONT, reinterpret_cast<WPARAM>(l_pData->m_hFont), TRUE);
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
	::InvalidateRect(l_pData->m_hWndRich, nullptr, TRUE);
	::UpdateWindow(l_pData->m_hWndRich);
}


void LoadInstructions(HWND a_hWndHelp, const std::string& a_sSection)
{
	HelpWndData* l_pData = HelpWndData::GetData(a_hWndHelp);
	auto l_vect = l_pData->m_jsonHelp.getInstructions(a_sSection);
	tstring l_sInstructions;

	for (const auto& l_sLine : l_vect)
		l_sInstructions += l_sLine + _T('\n');

	::SetWindowText(l_pData->m_hWndRich, l_sInstructions.c_str());
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


